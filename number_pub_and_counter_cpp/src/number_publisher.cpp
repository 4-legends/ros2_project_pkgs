#include "rclcpp/rclcpp.hpp"
#include "example_interfaces/msg/int64.hpp"
    
class NumberPublisher : public rclcpp::Node
{
public:
    NumberPublisher() : Node("number_publisher")
    {
        number_ = 18;
        num_publisher_ = this->create_publisher<example_interfaces::msg::Int64>("number", 10);
        timer_ = this->create_timer(std::chrono::seconds(1), 
                                    std::bind(&NumberPublisher::publishNode, this));
        RCLCPP_INFO(this->get_logger(), "Number Publisher node has started");
    }
    
private:
    void publishNode(){
        auto msg = example_interfaces::msg::Int64();
        msg.data = this->number_;
        this->num_publisher_->publish(msg);
    }
    
    int number_;
    rclcpp::Publisher<example_interfaces::msg::Int64>::SharedPtr num_publisher_;
    rclcpp::TimerBase::SharedPtr timer_;

};
    
int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<NumberPublisher>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}