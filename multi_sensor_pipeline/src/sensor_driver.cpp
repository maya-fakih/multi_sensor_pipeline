#include "rclcpp/rclcpp.hpp"
#include "multi_sensor_interfaces/msg/imu_reading.hpp"
#include <chrono>
#include <cstdlib>
#include <cmath>

class SensorDriver : public rclcpp::Node
{
    public:
        SensorDriver(): Node("sensor_driver")
        {
            this->declare_parameter("publish_rate_hz", 50.0);
            double publish_rate_hz = this->get_parameter("publish_rate_hz").as_double();
            dt_ = 1.0 / publish_rate_hz;
            auto period = std::chrono::milliseconds(static_cast<int>(1000.0 / publish_rate_hz));
            timer_ = this->create_wall_timer(period, std::bind(&SensorDriver::publish_imu_reading, this));
            publisher_ = this->create_publisher<multi_sensor_interfaces::msg::ImuReading>("raw", 10);
        }
        private:
        rclcpp::Publisher<multi_sensor_interfaces::msg::ImuReading>::SharedPtr publisher_;
        rclcpp::TimerBase::SharedPtr timer_;
        double dt_;
        double t_ = 0.0;

        void publish_imu_reading()
        {
            auto msg = multi_sensor_interfaces::msg::ImuReading();

            // accel: sinusoid, each axis phase-shifted so they're visibly different
            msg.accel_x = std::sin(t_);
            msg.accel_y = std::sin(t_ + 1.0);
            msg.accel_z = std::sin(t_ + 2.0);

            // gyro: random noise
            msg.gyro_x  = -1.0 + (static_cast<double>(rand()) / RAND_MAX) * 2.0;
            msg.gyro_y  = -1.0 + (static_cast<double>(rand()) / RAND_MAX) * 2.0;
            msg.gyro_z  = -1.0 + (static_cast<double>(rand()) / RAND_MAX) * 2.0;

            t_ += dt_;

            msg.stamp = this->get_clock()->now();
            publisher_->publish(msg);
        }

};

int main(int argc, char **argv)
{
    // takes inline override params for yaml vars like
    // --ros-args -p publish_rate_hz:=10
    // and passes these to the init this way it can properly override yaml vars
    rclcpp::init(argc, argv);
    // init as mentioned parses the args before creating nodes
    // initializes dds for node discovery and comms


    // shared pointer reminder: has an internal count taht ++ on create and -- on destroy
    // when count is 0 it automatically disctructs so we dont have dangling pointers or memory leaks
    // unique pointers are also clean but can have only one owner which creates an issue in ros since we have multiple
    // functions and classes accessing this pointer so it has to be shared
    auto node = std::make_shared<SensorDriver>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}