#include "rclcpp/rclcpp.hpp"
#include "multi_sensor_interfaces/msg/imu_reading.hpp"
#include <deque>

class FilterNode : public rclcpp::Node
{
    public:
        FilterNode(): Node("filter_node")
        {
            this->declare_parameter("window_size", 10);
            window_size_ = this->get_parameter("window_size").as_int();

            rclcpp::QoS qos(rclcpp::KeepLast(20));
            qos.reliable();

            publisher_ = this->create_publisher<multi_sensor_interfaces::msg::ImuReading>("filtered", qos);
            subscriber_ = this->create_subscription<multi_sensor_interfaces::msg::ImuReading>("raw", qos, std::bind(&FilterNode::filtered_callback, this, std::placeholders::_1));

        }
    private:
        rclcpp::Publisher<multi_sensor_interfaces::msg::ImuReading>::SharedPtr publisher_;
        rclcpp::Subscription<multi_sensor_interfaces::msg::ImuReading>::SharedPtr subscriber_;
        int window_size_;
        std::deque<multi_sensor_interfaces::msg::ImuReading> window_;

        void filtered_callback(const multi_sensor_interfaces::msg::ImuReading &msg)
        {
            auto to_publish = multi_sensor_interfaces::msg::ImuReading();
            window_.push_back(msg);
            while (window_.size() > static_cast<size_t>(this->window_size_))
                window_.pop_front();
            double accel_x_sum = 0.0;
            double accel_y_sum = 0.0;
            double accel_z_sum = 0.0;
            double gyro_x_sum  = 0.0;
            double gyro_y_sum  = 0.0;
            double gyro_z_sum  = 0.0;

            for (const auto &item: window_)
            {
                accel_x_sum += item.accel_x;
                accel_y_sum += item.accel_y;
                accel_z_sum += item.accel_z;
                gyro_x_sum  += item.gyro_x;
                gyro_y_sum  += item.gyro_y;
                gyro_z_sum  += item.gyro_z;
            }
            size_t size = window_.size();
            to_publish.accel_x = accel_x_sum / size;
            to_publish.accel_y = accel_y_sum / size;
            to_publish.accel_z = accel_z_sum / size;
            to_publish.gyro_x  = gyro_x_sum  / size;
            to_publish.gyro_y  = gyro_y_sum  / size;
            to_publish.gyro_z  = gyro_z_sum  / size;
            to_publish.stamp = this->get_clock()->now();
            publisher_->publish(to_publish);
        }

};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<FilterNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}