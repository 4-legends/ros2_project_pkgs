#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from example_interfaces.msg import Int64
    
    
class NumberPublisher(Node):
    def __init__(self):
        super().__init__("number_publisher")
        self.number = 18
        self.number_publisher_ = self.create_publisher(Int64, "number", 10)
        self.create_timer(1.0, self.timerCallback)
        self.get_logger().info("Number publisher node started")
    
    def timerCallback(self):
        msg = Int64()
        msg.data = self.number
        self.number_publisher_.publish(msg=msg)

    
def main(args=None):
    rclpy.init(args=args)
    node = NumberPublisher()
    rclpy.spin(node)
    rclpy.shutdown()
    
    
if __name__ == "__main__":
    main()