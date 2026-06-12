#include "tron2_gazebo/Tron2HWSim.h"
#include <gazebo_ros_control/gazebo_ros_control_plugin.h>

namespace tron2_gazebo
{

bool Tron2HWSim::initSim(const std::string &robot_namespace, ros::NodeHandle model_nh,
                          gazebo::physics::ModelPtr parent_model,
                          const urdf::Model *urdf_model,
                          std::vector<transmission_interface::TransmissionInfo> transmissions)
{
    bool ret = DefaultRobotHWSim::initSim(robot_namespace, model_nh, parent_model, urdf_model, transmissions);

    ros::NodeHandle root_nh;
    if (!root_nh.getParam("/Tron2Cfg/init_state/joint_names", jointNames_) || jointNames_.empty())
    {
        ROS_ERROR("Tron2HWSim: missing /Tron2Cfg/init_state/joint_names");
        return false;
    }

    const std::vector<std::string> &gzNames = ej_interface_.getNames();
    std::unordered_map<std::string, bool> gzNameSet;
    for (const auto &n : gzNames) gzNameSet[n] = true;

    hybridJointDatas_.reserve(jointNames_.size());
    for (const auto &name : jointNames_)
    {
        if (!gzNameSet.count(name))
        {
            ROS_ERROR("Tron2HWSim: Gazebo EffortJointInterface has no handle for '%s'", name.c_str());
            return false;
        }
        hybridJointDatas_.push_back(HybridJointData{.joint_ = ej_interface_.getHandle(name)});
        HybridJointData &back = hybridJointDatas_.back();
        hybridJointInterface_.registerHandle(robot_common::HybridJointHandle(
            back.joint_, &back.posDes_, &back.velDes_, &back.kp_, &back.kd_, &back.ff_, &back.mode_));
        cmdBuffer_.emplace(name, std::deque<HybridJointCommand>());
    }
    registerInterface(&hybridJointInterface_);

    if (!model_nh.getParam("gazebo/delay", delay_))
    {
        delay_ = 0.0;
    }
    ROS_INFO("Tron2HWSim: cmd transport delay = %.4f s", delay_);

    registerInterface(&imuSensorInterface_);
    XmlRpc::XmlRpcValue xmlRpcValue;
    if (!model_nh.getParam("gazebo/imus", xmlRpcValue))
    {
        ROS_WARN("Tron2HWSim: no imu specified under gazebo/imus");
    }
    else
    {
        parseImu(xmlRpcValue, parent_model);
    }

    tron2sim_ = limxsdk::Tron2Sim::getInstance();
    if (!tron2sim_->init("127.0.0.1"))
    {
        ROS_ERROR("Tron2HWSim: limxsdk Tron2Sim::init(\"127.0.0.1\") failed.");
        return false;
    }
    robotCmdBuffer_.writeFromNonRT(limxsdk::RobotCmd(jointNames_.size()));
    tron2sim_->subscribeRobotCmdForSim([this](const limxsdk::RobotCmdConstPtr &msg) {
        if (msg->q.size() != jointNames_.size())
        {
            ROS_WARN_THROTTLE(2.0, "Tron2HWSim: RobotCmd size %zu != joint count %zu",
                              msg->q.size(), jointNames_.size());
            return;
        }
        robotCmdBuffer_.writeFromNonRT(*msg);
        firstCmdReceived_.store(true, std::memory_order_relaxed);
    });
    ROS_INFO("Tron2HWSim: limxsdk Tron2Sim bridge ready, joints=%zu", jointNames_.size());

    return ret;
}

void Tron2HWSim::readSim(ros::Time time, ros::Duration period)
{
    gazebo_ros_control::DefaultRobotHWSim::readSim(time, period);

    for (auto &imu : imuDatas_)
    {
        ignition::math::Pose3d pose = imu.linkPtr_->WorldPose();
        imu.ori_[0] = pose.Rot().X();
        imu.ori_[1] = pose.Rot().Y();
        imu.ori_[2] = pose.Rot().Z();
        imu.ori_[3] = pose.Rot().W();

        ignition::math::Vector3d rate = imu.linkPtr_->RelativeAngularVel();
        imu.angularVel_[0] = rate.X();
        imu.angularVel_[1] = rate.Y();
        imu.angularVel_[2] = rate.Z();

        ignition::math::Vector3d gravity = {0., 0., -9.81};
        ignition::math::Vector3d accel =
            imu.linkPtr_->RelativeLinearAccel() - pose.Rot().RotateVectorReverse(gravity);
        imu.linearAcc_[0] = accel.X();
        imu.linearAcc_[1] = accel.Y();
        imu.linearAcc_[2] = accel.Z();
    }

    for (auto &cmd : joint_effort_command_)
        cmd = 0;
    for (auto &cmd : joint_velocity_command_)
        cmd = 0;

    if (tron2sim_ != nullptr)
    {
        limxsdk::RobotState state(jointNames_.size());
        state.stamp = time.toNSec();
        state.motor_names = jointNames_;
        for (size_t i = 0; i < hybridJointDatas_.size(); ++i)
        {
            state.q[i] = static_cast<float>(hybridJointDatas_[i].joint_.getPosition());
            state.dq[i] = static_cast<float>(hybridJointDatas_[i].joint_.getVelocity());
            state.tau[i] = static_cast<float>(hybridJointDatas_[i].joint_.getEffort());
        }
        tron2sim_->publishRobotStateForSim(state);

        if (!imuDatas_.empty())
        {
            const auto &imu = imuDatas_.front();
            limxsdk::ImuData imuMsg;
            imuMsg.stamp = time.toNSec();
            // limxsdk quat layout is [w, x, y, z]; Gazebo Pose3d.Rot() gives [x, y, z, w].
            imuMsg.quat[0] = static_cast<float>(imu.ori_[3]);
            imuMsg.quat[1] = static_cast<float>(imu.ori_[0]);
            imuMsg.quat[2] = static_cast<float>(imu.ori_[1]);
            imuMsg.quat[3] = static_cast<float>(imu.ori_[2]);
            imuMsg.gyro[0] = static_cast<float>(imu.angularVel_[0]);
            imuMsg.gyro[1] = static_cast<float>(imu.angularVel_[1]);
            imuMsg.gyro[2] = static_cast<float>(imu.angularVel_[2]);
            imuMsg.acc[0] = static_cast<float>(imu.linearAcc_[0]);
            imuMsg.acc[1] = static_cast<float>(imu.linearAcc_[1]);
            imuMsg.acc[2] = static_cast<float>(imu.linearAcc_[2]);
            tron2sim_->publishImuDataForSim(imuMsg);
        }
    }
}

void Tron2HWSim::writeSim(ros::Time time, ros::Duration period)
{
    if (!firstCmdReceived_.load(std::memory_order_relaxed))
    {
        DefaultRobotHWSim::writeSim(time, period);
        return;
    }

    if (tron2sim_ != nullptr)
    {
        const limxsdk::RobotCmd cmd = *robotCmdBuffer_.readFromRT();
        if (cmd.q.size() == hybridJointDatas_.size())
        {
            for (size_t i = 0; i < hybridJointDatas_.size(); ++i)
            {
                hybridJointDatas_[i].posDes_ = cmd.q[i];
                hybridJointDatas_[i].velDes_ = cmd.dq[i];
                hybridJointDatas_[i].kp_ = cmd.Kp[i];
                hybridJointDatas_[i].kd_ = cmd.Kd[i];
                hybridJointDatas_[i].ff_ = cmd.tau[i];
            }
        }
    }

    for (auto &joint : hybridJointDatas_)
    {
        auto &buffer = cmdBuffer_.find(joint.joint_.getName())->second;

        if (time == ros::Time(period.toSec()))
        {
            buffer.clear();
        }

        while (!buffer.empty() && buffer.back().stamp_ + ros::Duration(delay_) < time)
        {
            buffer.pop_back();
        }

        buffer.push_front(HybridJointCommand{
            .stamp_ = time,
            .posDes_ = joint.posDes_,
            .velDes_ = joint.velDes_,
            .kp_ = joint.kp_,
            .kd_ = joint.kd_,
            .ff_ = joint.ff_});

        const auto &delayed = buffer.back();
        const double pos = joint.joint_.getPosition();
        const double vel = joint.joint_.getVelocity();
        const double tau = delayed.kp_ * (delayed.posDes_ - pos)
                         + delayed.kd_ * (delayed.velDes_ - vel)
                         + delayed.ff_;
        joint.joint_.setCommand(tau);
    }

    DefaultRobotHWSim::writeSim(time, period);
}

void Tron2HWSim::parseImu(XmlRpc::XmlRpcValue &imuDatas, const gazebo::physics::ModelPtr &parentModel)
{
    ROS_ASSERT(imuDatas.getType() == XmlRpc::XmlRpcValue::TypeStruct);

    for (auto it = imuDatas.begin(); it != imuDatas.end(); ++it)
    {
        if (!it->second.hasMember("frame_id"))
        {
            ROS_ERROR_STREAM("Imu " << it->first << " has no associated frame id.");
            continue;
        }
        else if (!it->second.hasMember("orientation_covariance_diagonal"))
        {
            ROS_ERROR_STREAM("Imu " << it->first << " has no associated orientation covariance diagonal.");
            continue;
        }
        else if (!it->second.hasMember("angular_velocity_covariance"))
        {
            ROS_ERROR_STREAM("Imu " << it->first << " has no associated angular velocity covariance.");
            continue;
        }
        else if (!it->second.hasMember("linear_acceleration_covariance"))
        {
            ROS_ERROR_STREAM("Imu " << it->first << " has no associated linear acceleration covariance.");
            continue;
        }

        XmlRpc::XmlRpcValue oriCov = imuDatas[it->first]["orientation_covariance_diagonal"];
        ROS_ASSERT(oriCov.getType() == XmlRpc::XmlRpcValue::TypeArray && oriCov.size() == 3);
        XmlRpc::XmlRpcValue angularCov = imuDatas[it->first]["angular_velocity_covariance"];
        ROS_ASSERT(angularCov.getType() == XmlRpc::XmlRpcValue::TypeArray && angularCov.size() == 3);
        XmlRpc::XmlRpcValue linearCov = imuDatas[it->first]["linear_acceleration_covariance"];
        ROS_ASSERT(linearCov.getType() == XmlRpc::XmlRpcValue::TypeArray && linearCov.size() == 3);

        std::string frameId = imuDatas[it->first]["frame_id"];
        gazebo::physics::LinkPtr linkPtr = parentModel->GetLink(frameId);
        ROS_ASSERT(linkPtr != nullptr);

        imuDatas_.push_back(ImuData{
            .linkPtr_ = linkPtr,
            .ori_ = {0., 0., 0., 0.},
            .oriCov_ = {static_cast<double>(oriCov[0]), 0., 0., 0., static_cast<double>(oriCov[1]), 0., 0., 0., static_cast<double>(oriCov[2])},
            .angularVel_ = {0., 0., 0.},
            .angularVelCov_ = {static_cast<double>(angularCov[0]), 0., 0., 0., static_cast<double>(angularCov[1]), 0., 0., 0., static_cast<double>(angularCov[2])},
            .linearAcc_ = {0., 0., 0.},
            .linearAccCov_ = {static_cast<double>(linearCov[0]), 0., 0., 0., static_cast<double>(linearCov[1]), 0., 0., 0., static_cast<double>(linearCov[2])}});

        ImuData &imuData = imuDatas_.back();
        imuSensorInterface_.registerHandle(hardware_interface::ImuSensorHandle(
            it->first, frameId, imuData.ori_, imuData.oriCov_,
            imuData.angularVel_, imuData.angularVelCov_,
            imuData.linearAcc_, imuData.linearAccCov_));
    }
}

} // namespace tron2_gazebo

PLUGINLIB_EXPORT_CLASS(tron2_gazebo::Tron2HWSim, gazebo_ros_control::RobotHWSim)
GZ_REGISTER_MODEL_PLUGIN(gazebo_ros_control::GazeboRosControlPlugin)
