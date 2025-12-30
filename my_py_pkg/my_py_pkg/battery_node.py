#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from functools import partial
from custom_robot_interfaces.srv import LedPanelStateChange
    
    
class BatteryNode(Node):
    def __init__(self):
        super().__init__("battery_node")
        self.last_state_change_time = self.get_current_time()
        self.battery_state = "full"
        self.client_ = self.create_client(LedPanelStateChange, "set_led")
        self.timer_ = self.create_timer(1.0, self.battery_monitoring)
        self.get_logger().info(f"Battery Node has started.")
    
    def get_current_time(self):
        seconds, nanoseconds = self.get_clock().now().seconds_nanoseconds()
        return seconds + nanoseconds / 1000000000.0

    def battery_monitoring(self):
        time_now_ = self.get_current_time()
        if self.battery_state == "full":
            if time_now_ - self.last_state_change_time > 4.0:
                self.battery_state = "empty"
                self.last_state_change_time = time_now_
                self.send_req(1)
                self.get_logger().info(f"Battery is empty. Charging now...")
        elif self.battery_state == "empty":
            if time_now_ - self.last_state_change_time > 6.0:
                self.battery_state = "full"
                self.last_state_change_time = time_now_
                self.send_req(0)
                self.get_logger().info(f"Battery is full....")

    def send_req(self, led_state):
        battery_msg = LedPanelStateChange.Request()
        battery_msg.panel.led3 = led_state
        while not self.client_.wait_for_service(1.0):
            self.get_logger().warn("Waiting for Set Led server.....")  

        future = self.client_.call_async(request=battery_msg)
        future.add_done_callback(partial(self.service_callback, request=battery_msg))

    
    def service_callback(self, future, request:LedPanelStateChange.Request):
        response: LedPanelStateChange.Response = future.result()
        self.get_logger().info(f"Request: {request.panel.led3}, response: {response.message}")

def main(args=None):
    rclpy.init(args=args)
    node = BatteryNode()
    rclpy.spin(node)
    rclpy.shutdown()
    
    
if __name__ == "__main__":
    main()