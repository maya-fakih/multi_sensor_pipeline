import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    config = os.path.join(
        get_package_share_directory('multi_sensor_pipeline'),
        'config',
        'imu_pipeline.yaml'
    )

    return LaunchDescription([
        Node(
            package='multi_sensor_pipeline',
            executable='sensor_driver',
            name='sensor_driver',
            namespace='imu',
            parameters=[config],
            output='screen',
        ),
        Node(
            package='multi_sensor_pipeline',
            executable='filter_node',
            name='filter_node',
            namespace='imu',
            parameters=[config],
            output='screen',
        ),
        Node(
            package = 'multi_sensor_pipeline',
            executable='logger_node',
            name='logger_node',
            namespace='imu',
            parameters=[config],
            output='screen',
        ),
    ])