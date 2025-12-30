#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist

class DrawCircleNode(Node):
    def __init__(self):
        super().__init__("draw_circle")

        self.cmd_vel_pub = self.create_publisher(Twist, "/turtle1/cmd_vel", 10)
        self.create_timer(0.5, self.send_velocity_command)

        self.get_logger().info(f"Draw circle node has been started")

    def send_velocity_command(self):
        msg = Twist()
        msg.linear.x = 2.0
        msg.angular.z = 1.0
        self.cmd_vel_pub.publish(msg=msg)


def main(args=None):
    #Intalize rclpy for ros2 comms
    rclpy.init(args=args)

    # Create Node
    node = DrawCircleNode()

    # To keep node alive till we kill the node
    rclpy.spin(node)

    # Destroy's the node and shutdown
    rclpy.shutdown()

if __name__ == '__main__':
    main()
