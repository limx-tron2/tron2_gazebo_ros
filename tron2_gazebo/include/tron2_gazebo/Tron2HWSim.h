#pragma once

#include <atomic>
#include <deque>
#include <list>
#include <string>
#include <unordered_map>
#include <vector>

#include <gazebo_ros_control/default_robot_hw_sim.h>
#include <hardware_interface/imu_sensor_interface.h>
#include <hardware_interface/joint_command_interface.h>
#include <realtime_tools/realtime_buffer.h>
#include <robot_common/hardware_interface/HybridJointInterface.h>
#include <robot_common/hardware_interface/ContactSensorInterface.h>
#include <limxsdk/tron2_sim.h>

namespace tron2_gazebo
{

struct HybridJointData
{
    hardware_interface::JointHandle joint_;
    double posDes_{};
    double velDes_{};
    double kp_{};
    double kd_{};
    double ff_{};
    uint8_t mode_{};
};

struct HybridJointCommand
{
    ros::Time stamp_;
    double posDes_{};
    double velDes_{};
    double kp_{};
    double kd_{};
    double ff_{};
};

struct ImuData
{
    gazebo::physics::LinkPtr linkPtr_;
    double ori_[4];
    double oriCov_[9];
    double angularVel_[3];
    double angularVelCov_[9];
    double linearAcc_[3];
    double linearAccCov_[9];
};

class Tron2HWSim : public gazebo_ros_control::DefaultRobotHWSim
{
public:
    bool initSim(const std::string &robot_namespace, ros::NodeHandle model_nh,
                 gazebo::physics::ModelPtr parent_model,
                 const urdf::Model *urdf_model,
                 std::vector<transmission_interface::TransmissionInfo> transmissions) override;

    void readSim(ros::Time time, ros::Duration period) override;
    void writeSim(ros::Time time, ros::Duration period) override;

private:
    void parseImu(XmlRpc::XmlRpcValue &imuDatas, const gazebo::physics::ModelPtr &parentModel);

    robot_common::HybridJointInterface hybridJointInterface_;
    hardware_interface::ImuSensorInterface imuSensorInterface_;

    std::vector<HybridJointData> hybridJointDatas_;
    std::list<ImuData> imuDatas_;

    std::vector<std::string> jointNames_;

    limxsdk::Tron2Sim *tron2sim_{nullptr};
    realtime_tools::RealtimeBuffer<limxsdk::RobotCmd> robotCmdBuffer_;

    std::unordered_map<std::string, std::deque<HybridJointCommand>> cmdBuffer_;
    double delay_{0.0};

    std::atomic<bool> firstCmdReceived_{false};
};

} // namespace tron2_gazebo
