#include "rclcpp/rclcpp.hpp"
#include "multi_sensor_interfaces/msg/imu_reading.hpp"
#include <chrono>

class LoggerNode : public rclcpp::Node
{
    public:
        LoggerNode(): Node("logger_node")
        {
            this->declare_parameter("publish_rate_hz", 1.0);
            double publish_rate_hz = this->get_parameter("publish_rate_hz").as_double();
            auto period = std::chrono::milliseconds(static_cast<int>(1000.0 / publish_rate_hz));
            timer_ = this->create_wall_timer(period, std::bind(&LoggerNode::log_callback, this));

            raw_sub_ = this->create_subscription<multi_sensor_interfaces::msg::ImuReading>("raw", 10, std::bind(&LoggerNode::raw_callback, this, std::placeholders::_1));
            filtered_sub_ = this->create_subscription<multi_sensor_interfaces::msg::ImuReading>("filtered", 10, std::bind(&LoggerNode::filter_callback, this, std::placeholders::_1));
        }
    private:
        multi_sensor_interfaces::msg::ImuReading raw_;
        multi_sensor_interfaces::msg::ImuReading filtered_;
        rclcpp::Subscription<multi_sensor_interfaces::msg::ImuReading>::SharedPtr raw_sub_;
        rclcpp::Subscription<multi_sensor_interfaces::msg::ImuReading>::SharedPtr filtered_sub_;
        rclcpp::TimerBase::SharedPtr timer_;

        void raw_callback(const multi_sensor_interfaces::msg::ImuReading &msg)
        {
            this->raw_ = msg;
        }

        void filter_callback(const multi_sensor_interfaces::msg::ImuReading &msg)
        {
            this->filtered_ = msg;
        }

        void log_callback()
        {
            double diff = raw_.accel_x - filtered_.accel_x;
            RCLCPP_INFO(this->get_logger(), "raw: %f filtered: %f diff: %f", raw_.accel_x, filtered_.accel_x, diff);
        }
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<LoggerNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}