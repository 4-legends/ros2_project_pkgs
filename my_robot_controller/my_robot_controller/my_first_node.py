#!/usr/bin/env python3

import rclpy
from rclpy.node import Node

class MyNode(Node):
    def __init__(self):
        # Node name is first_node
        super().__init__("first_node")
        self.counter = 0
        # self.get_logger().info("Hello from ROS2")
        self.create_timer(1.0, self.timer_callback)
    
    def timer_callback(self):
        self.get_logger().info(f"Hello {self.counter}")
        self.counter += 1


def main(args=None):
    #Intalize rclpy for ros2 comms
    rclpy.init(args=args)

    # Create Node
    node = MyNode()

    # To keep node alive till we kill the node
    rclpy.spin(node)

    # Destroy's the node and shutdown
    rclpy.shutdown()

if __name__ == '__main__':
    main()
