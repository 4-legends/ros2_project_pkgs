#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from example_interfaces.msg import Int64
from example_interfaces.srv import SetBool
    
    
class NumberCounter(Node):
    def __init__(self):
        super().__init__("number_counter")
        self.logger = self.get_logger()
        self.counter = 0
        self.count_publishers_ = self.create_publisher(Int64, "number_count", 10)
        self.num_subscriber_ = self.create_subscription(Int64, "number", self.numPubCallback, 10)
        self.reset_counter_server_ = self.create_service(SetBool, "reset_counter", self.callback_reset_counter)
        self.logger.info("Number counter node has started")

    def numPubCallback(self, msg: Int64):
        self.counter += 1
        self.logger.info(f"Number received: {msg.data} {self.counter} times")
        new_msg = Int64()
        new_msg.data = self.counter
        self.count_publishers_.publish(msg=new_msg)
    
    def callback_reset_counter(self, request: SetBool.Request, response: SetBool.Response):
        if request.data:
            self.get_logger().info(f"Resetting counter from {self.counter} to 0")
            response.success = True
            response.message = f"Resetting counter from {self.counter} to 0"
            self.counter = 0
        else:
            response.message = f"Data for reuqest is {request.data}, keeping counter at {self.counter}"
        return response
    
    
def main(args=None):
    rclpy.init(args=args)
    node = NumberCounter()
    rclpy.spin(node)
    rclpy.shutdown()
    
    
if __name__ == "__main__":
    main()