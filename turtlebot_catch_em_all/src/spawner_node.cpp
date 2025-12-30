#include "rclcpp/rclcpp.hpp"
#include "turtlesim/srv/spawn.hpp"
#include "turtlesim/srv/kill.hpp"
#include "turtlebot_catch_em_all/msg/turtle.hpp"
#include "turtlebot_catch_em_all/msg/turtle_array.hpp"
#include "turtlebot_catch_em_all/srv/caught_turtle.hpp"

class SpawnerNode : public rclcpp::Node
{
public:
    SpawnerNode() : Node("spawner_node")
    {
        this->declare_parameter<int>("spawn_interval_ms", 500);
        this->declare_parameter<int>("max_turtles", 50);
        spawn_interval_ms_ = this->get_parameter("spawn_interval_ms").as_int();
        max_turtles_ = this->get_parameter("max_turtles").as_int();
        spawn_client_ = this->create_client<turtlesim::srv::Spawn>("spawn");
        turtle_array_publisher_ = this->create_publisher<turtlebot_catch_em_all::msg::TurtleArray>("turtles_to_catch", 10);
        kill_client_ = this->create_client<turtlesim::srv::Kill>("kill");
        turtle_array_msg_.turtles.clear();
        caught_turtle_service_ = this->create_service<turtlebot_catch_em_all::srv::CaughtTurtle>(
            "caught_turtle",
            std::bind(&SpawnerNode::handle_caught_turtle, this, std::placeholders::_1, std::placeholders::_2)
        );
        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(spawn_interval_ms_),
            std::bind(&SpawnerNode::spawnner_control_loop, this)
        );

    }
    
private:
    void handle_caught_turtle(
        const std::shared_ptr<turtlebot_catch_em_all::srv::CaughtTurtle::Request> request,
        std::shared_ptr<turtlebot_catch_em_all::srv::CaughtTurtle::Response> response)
    {   
        RCLCPP_INFO(this->get_logger(), "Turtle caught: %s", request->name.c_str());
        auto request_kill = std::make_shared<turtlesim::srv::Kill::Request>();
        request_kill->name = request->name;
        while (!kill_client_->wait_for_service(std::chrono::seconds(1))) {
            RCLCPP_INFO(this->get_logger(), "Waiting for kill service to be available...");
        }
        turtle_to_kill_ = request->name;
        auto result_future = kill_client_->async_send_request(request_kill, std::bind(&SpawnerNode::handle_killed_turtle, this, std::placeholders::_1));
        response->success = true;

    }

    void handle_killed_turtle(rclcpp::Client<turtlesim::srv::Kill>::SharedFuture future)
    {
        turtlesim::srv::Kill::Response::SharedPtr response = future.get();
        RCLCPP_INFO(this->get_logger(), "Turtle killed: %s", turtle_to_kill_.c_str());
        for (auto it = turtle_array_msg_.turtles.begin(); it != turtle_array_msg_.turtles.end(); ++it)
        {
            if (it->name == turtle_to_kill_)
            {
                turtle_array_msg_.turtles.erase(it);
                break;
            }
        }
        turtle_array_publisher_->publish(turtle_array_msg_);
    }

    void spawnner_control_loop(){
        if (static_cast<int>(turtle_array_msg_.turtles.size()) >= max_turtles_) {
            RCLCPP_WARN(this->get_logger(), "Maximum number of turtles reached. Not spawning more.");
            return;
        }
        std::string turtle_name = turtle_prefix + std::to_string(turtle_count_++);
        float x = static_cast<float>(rand() % 10 + 1);
        float y = static_cast<float>(rand() % 10 + 1);
        float theta = static_cast<float>(rand() % 360) * 3.14159f / 180.0f;
        spwan_turtle(turtle_name, x, y, theta);
    }

    void spwan_turtle(const std::string & name, float x, float y, float theta)
    {
        auto request = std::make_shared<turtlesim::srv::Spawn::Request>();
        request->name = name;
        request->x = x;
        request->y = y;
        request->theta = theta;

        turtle_to_save_.name = name;
        turtle_to_save_.x = x;
        turtle_to_save_.y = y;
        turtle_to_save_.theta = theta;

        while (!spawn_client_->wait_for_service(std::chrono::seconds(1))) {
            RCLCPP_INFO(this->get_logger(), "Waiting for spawn service to be available...");
        }

        auto result_future = spawn_client_->async_send_request(request, std::bind(&SpawnerNode::handle_spawned_turtle, this, std::placeholders::_1));
    }

    void handle_spawned_turtle(
        rclcpp::Client<turtlesim::srv::Spawn>::SharedFuture future)
    {
        auto response = future.get();
        if (response->name != turtle_to_save_.name) {
            RCLCPP_ERROR(this->get_logger(), "Spawned turtle name mismatch: expected %s, got %s",
                turtle_to_save_.name.c_str(), response->name.c_str());
            return;
        }
        RCLCPP_INFO(this->get_logger(), "Spawned turtle: %s", response->name.c_str());
        turtlebot_catch_em_all::msg::Turtle new_turtle;
        new_turtle.name = response->name;
        new_turtle.x = turtle_to_save_.x;
        new_turtle.y = turtle_to_save_.y;
        new_turtle.theta = turtle_to_save_.theta;
        turtle_array_msg_.turtles.push_back(new_turtle);
        turtle_array_publisher_->publish(turtle_array_msg_);
    }

    std::string turtle_prefix = "turtlebot_";
    int turtle_count_ = 2;
    int spawn_interval_ms_;
    int max_turtles_;
    turtlebot_catch_em_all::msg::Turtle turtle_to_save_;
    std::string turtle_to_kill_;
    rclcpp::TimerBase::SharedPtr timer_;
    turtlebot_catch_em_all::msg::TurtleArray turtle_array_msg_;
    rclcpp::Client<turtlesim::srv::Spawn>::SharedPtr spawn_client_;
    rclcpp::Client<turtlesim::srv::Kill>::SharedPtr kill_client_;
    rclcpp::Publisher<turtlebot_catch_em_all::msg::TurtleArray>::SharedPtr turtle_array_publisher_;
    rclcpp::Service<turtlebot_catch_em_all::srv::CaughtTurtle>::SharedPtr caught_turtle_service_;
};
    
int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<SpawnerNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}