from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    ld = LaunchDescription()
    number_publisher = Node(
        package="number_pub_and_counter_py",
        executable="number_publisher"
    )

    number_counter = Node(
        package="number_pub_and_counter_py",
        executable="number_counter"
    )

    ld.add_action(number_publisher)
    ld.add_action(number_counter)

    return ld