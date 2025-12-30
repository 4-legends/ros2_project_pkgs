#include "cmath"
#include "rclcpp/rclcpp.hpp"
#include "turtlesim/msg/pose.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "turtlebot_catch_em_all/msg/turtle_array.hpp"
#include "turtlebot_catch_em_all/msg/turtle.hpp"
#include "turtlebot_catch_em_all/srv/caught_turtle.hpp"

    
class TurtleSimController : public rclcpp::Node
{
public:
    TurtleSimController() : Node("turtlesim_controller")
    {
        this->declare_parameter<double>("linear_speed", 1.0);
        this->declare_parameter<double>("angular_speed", 2.0);
        this->declare_parameter<double>("distance_tolerance", 0.1);

        linear_speed_ = this->get_parameter("linear_speed").as_double();
        angular_speed_ = this->get_parameter("angular_speed").as_double();
        distance_tolerance_ = this->get_parameter("distance_tolerance").as_double();

        cmd_vel_publisher = this->create_publisher<geometry_msgs::msg::Twist>("turtle1/cmd_vel", 10);
        timer_ = this->create_timer(std::chrono::milliseconds(500), 
                                    std::bind(&TurtleSimController::publishVelocity, this));
        current_pose_subscription_ = this->create_subscription<turtlesim::msg::Pose>( "turtle1/pose", 10, 
                                                                                    std::bind(&TurtleSimController::currentPoseConsumer, this, std::placeholders::_1));
        turtle_to_target_ = this->create_subscription<turtlebot_catch_em_all::msg::TurtleArray>(
            "turtles_to_catch", 10,
            std::bind(&TurtleSimController::goalPoseConsumer, this, std::placeholders::_1));
        caught_turtle_client_ = this->create_client<turtlebot_catch_em_all::srv::CaughtTurtle>("caught_turtle");
        goal_reached_ = true;
        RCLCPP_INFO(this->get_logger(), "TurtleSim Controller node has started");
    }
    
private:
    void publishVelocity(){
        if (goal_reached_) {
            auto stop_msg = geometry_msgs::msg::Twist();
            stop_msg.linear.x = 0.0;
            stop_msg.angular.z = 0.0;
            this->cmd_vel_publisher->publish(stop_msg);
            return;
        }
        auto msg = geometry_msgs::msg::Twist();
        double dx = goal_pose_.x - current_pose_.x;
        double dy = goal_pose_.y - current_pose_.y;

        double distance = std::hypot(dx, dy);         // distance to goal
        double target_theta = std::atan2(dy, dx);    // heading to goal
        double angle_error = target_theta - current_pose_.theta; // difference in heading

        // Normalize angle to [-pi, pi]
        while (angle_error > M_PI) angle_error -= 2 * M_PI;
        while (angle_error < -M_PI) angle_error += 2 * M_PI;

        // Proportional gains
        double K_linear = linear_speed_;
        double K_angular = angular_speed_;

        msg.linear.x = K_linear * distance;
        msg.angular.z = K_angular * angle_error;
        this->cmd_vel_publisher->publish(msg);
    }

    void currentPoseConsumer(const turtlesim::msg::Pose::SharedPtr msg){
        current_pose_ = *msg;
        double dx = goal_pose_.x - current_pose_.x;
        double dy = goal_pose_.y - current_pose_.y;
        double distance = std::hypot(dx, dy);
        if (distance < distance_tolerance_ && !goal_reached_) {
            RCLCPP_INFO(this->get_logger(), "Goal reached!");
            auto request = std::make_shared<turtlebot_catch_em_all::srv::CaughtTurtle::Request>();
            request->name = goal_pose_.name;
            while (!caught_turtle_client_->wait_for_service(std::chrono::seconds(1))) {
                RCLCPP_INFO(this->get_logger(), "Waiting for caught_turtle service to be available...");
            }
            auto result_future = caught_turtle_client_->async_send_request(request);
            goal_reached_ = true;
        }
    }

    void goalPoseConsumer(const turtlebot_catch_em_all::msg::TurtleArray::SharedPtr msg){
        if (goal_reached_) {
            RCLCPP_INFO(this->get_logger(), "New goal received.");
            goal_reached_ = false;
            double distance = std::numeric_limits<double>::max();
            for (const auto & turtle : msg->turtles) {
                double dx = turtle.x - current_pose_.x;
                double dy = turtle.y - current_pose_.y;
                double dist = std::hypot(dx, dy);
                if (dist < distance) {
                    distance = dist;
                    goal_pose_ = turtle;
                }
            }
        }
    }
    
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_publisher;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr current_pose_subscription_;
    rclcpp::Client<turtlebot_catch_em_all::srv::CaughtTurtle>::SharedPtr caught_turtle_client_;
    rclcpp::Subscription<turtlebot_catch_em_all::msg::TurtleArray>::SharedPtr turtle_to_target_;

    turtlesim::msg::Pose current_pose_;
    rclcpp::TimerBase::SharedPtr timer_;
    turtlebot_catch_em_all::msg::Turtle goal_pose_;
    bool goal_reached_ = false;
    double linear_speed_;
    double angular_speed_;
    double distance_tolerance_;
};
    
int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<TurtleSimController>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}