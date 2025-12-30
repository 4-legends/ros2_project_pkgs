#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from custom_robot_interfaces.msg import LedPanelState
from custom_robot_interfaces.srv import LedPanelStateChange
    
    
class LedPanelNode(Node):
    def __init__(self):
        super().__init__("led_panel_node")
        self.declare_parameter("led1", 0)
        self.declare_parameter("led2", 0)
        self.declare_parameter("led3", 0)
        self.led1 = self.get_parameter("led1").value
        self.led2 = self.get_parameter("led2").value
        self.led3 = self.get_parameter("led3").value
        self.led_panel_state_topic_ = self.create_publisher(LedPanelState, "led_panel_state", 10)
        self.timer_ = self.create_timer(1.0, self.publish_led_state)
        self.led_state_server_ = self.create_service(LedPanelStateChange, "set_led", self.update_led_state)
        self.get_logger().info(f"Starting Led Panel State Node.")
    
    def publish_led_state(self):
        msg = LedPanelState()
        msg.led1 = self.led1
        msg.led2 = self.led2
        msg.led3 = self.led3
        self.led_panel_state_topic_.publish(msg)
    
    def update_led_state(self, request: LedPanelStateChange.Request, response: LedPanelStateChange.Response):
        self.led1 = request.panel.led1
        self.led2 = request.panel.led2
        self.led3 = request.panel.led3
        self.get_logger().info(f"Updated led state to [{self.led1}, {self.led2}, {self.led3}]")
        response.success = True
        response.message = "Successfully updated led State"

        return response
    
    
def main(args=None):
    rclpy.init(args=args)
    node = LedPanelNode()
    rclpy.spin(node)
    rclpy.shutdown()
    
    
if __name__ == "__main__":
    main()