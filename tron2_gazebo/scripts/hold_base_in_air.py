#!/usr/bin/env python3
"""
hold_base_in_air.py
Locks a Gazebo model's base pose at a fixed z for a startup window so the
controller can settle to the default joint pose without ground impact /
transient torque crashing physics. Releases automatically after the window.
"""
import rospy
from gazebo_msgs.msg import ModelState, ModelStates
from gazebo_msgs.srv import SetModelState


def wait_for_model(model_name, timeout):
    """Block until model_name appears in /gazebo/model_states, or timeout."""
    deadline = rospy.Time.now() + rospy.Duration(timeout)
    while not rospy.is_shutdown() and rospy.Time.now() < deadline:
        try:
            msg = rospy.wait_for_message("/gazebo/model_states", ModelStates, timeout=1.0)
        except rospy.ROSException:
            continue
        if model_name in msg.name:
            return True
    return False


def main():
    rospy.init_node("hold_base_in_air")
    hold_z = rospy.get_param("~hold_z", 1.5)
    hold_duration = rospy.get_param("~hold_duration", 3.0)
    model_name = rospy.get_param("~model_name", "tron2_robot")
    rate_hz = rospy.get_param("~rate_hz", 100.0)
    spawn_timeout = rospy.get_param("~spawn_timeout", 30.0)

    rospy.loginfo("hold_base_in_air: waiting for /gazebo/set_model_state ...")
    rospy.wait_for_service("/gazebo/set_model_state")
    set_state = rospy.ServiceProxy("/gazebo/set_model_state", SetModelState)

    rospy.loginfo("hold_base_in_air: waiting for model %s in /gazebo/model_states ...", model_name)
    if not wait_for_model(model_name, spawn_timeout):
        rospy.logerr("hold_base_in_air: model %s did not appear within %.1fs, aborting.",
                     model_name, spawn_timeout)
        return

    state = ModelState()
    state.model_name = model_name
    state.pose.position.z = hold_z
    state.pose.orientation.w = 1.0
    state.reference_frame = "world"

    rospy.loginfo(
        "hold_base_in_air: locking %s base at z=%.2f for %.1fs (rate=%.0fHz)",
        model_name, hold_z, hold_duration, rate_hz,
    )
    rate = rospy.Rate(rate_hz)
    end_time = rospy.Time.now() + rospy.Duration(hold_duration)
    while not rospy.is_shutdown() and rospy.Time.now() < end_time:
        try:
            resp = set_state(state)
            if not resp.success:
                rospy.logwarn_throttle(1.0, "set_model_state rejected: %s" % resp.status_message)
        except rospy.ServiceException as e:
            rospy.logwarn_throttle(1.0, "set_model_state failed: %s" % e)
        rate.sleep()
    rospy.loginfo("hold_base_in_air: released")


if __name__ == "__main__":
    main()
