#include "rclcpp/rclcpp.hpp"
#include "example_interfaces/msg/int64.hpp"
    
class NumberCounter : public rclcpp::Node
{
public:
    NumberCounter() : Node("node_name")
    {
        counter_ = 0;
        count_publisher_ = this->create_publisher<example_interfaces::msg::Int64>("number_count", 10);
        num_subscriber_ = this->create_subscription<example_interfaces::msg::Int64>("number", 10,
                                std::bind(&NumberCounter::numCallback, this, std::placeholders::_1));
    }
    
private:
    void numCallback(example_interfaces::msg::Int64 msg){
        counter_++;
        RCLCPP_INFO(this->get_logger(), "%ld number received %d times", msg.data, counter_);
        auto new_msg = example_interfaces::msg::Int64();
        new_msg.data = counter_;
        count_publisher_->publish(new_msg);
    }

    int counter_;
    rclcpp::Publisher<example_interfaces::msg::Int64>::SharedPtr count_publisher_;
    rclcpp::Subscription<example_interfaces::msg::Int64>::SharedPtr num_subscriber_;
};
    
int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<NumberCounter>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}