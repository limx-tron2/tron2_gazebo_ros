#!/usr/bin/env python3

import rospy
from geometry_msgs.msg import Twist
from std_msgs.msg import String
import pygame
import sys

class JoystickTeleop:
    def __init__(self):
        rospy.init_node('joystick_teleop')
        
        self.cmd_vel_pub = rospy.Publisher('/cmd_vel', Twist, queue_size=1)
        # Optional: publish mode to controller if we implement it
        self.mode_pub = rospy.Publisher('/tron2_controller/set_mode', String, queue_size=1)
        
        pygame.init()
        pygame.joystick.init()
        
        if pygame.joystick.get_count() == 0:
            rospy.logerr("No joystick found!")
            sys.exit(1)
            
        self.joystick = pygame.joystick.Joystick(0)
        self.joystick.init()
        rospy.loginfo(f"Joystick found: {self.joystick.get_name()}")
        
        self.max_vx = rospy.get_param('~max_vx', 1.0)
        self.max_vy = rospy.get_param('~max_vy', 0.5)
        self.max_yaw = rospy.get_param('~max_yaw', 1.0)
        self.deadzone = rospy.get_param('~deadzone', 0.08) # Match sim_tron2.py
        self.button_map = rospy.get_param('/joystick_buttons', {
            'A': 0,
            'B': 1,
            'X': 2,
            'Y': 3,
            'L1': 4,
            'R1': 5,
        })
        
        self.rate = rospy.Rate(50) # 50Hz
        
        self.prev_buttons = [0] * self.joystick.get_numbuttons()
        
    def apply_deadzone(self, value):
        if abs(value) < self.deadzone:
            return 0.0
        return value
        
    def run(self):
        while not rospy.is_shutdown():
            pygame.event.pump()
            
            # Axis mapping from sim_tron2.py:
            # 左摇杆: vx (上/下 -> axis 1), vy (左/右 -> axis 0)
            # 右摇杆: yaw (左/右 -> axis 3)
            
            lx = self.joystick.get_axis(0)
            ly = self.joystick.get_axis(1)
            rx = self.joystick.get_axis(3) if self.joystick.get_numaxes() > 3 else self.joystick.get_axis(2)
            
            twist = Twist()
            # Match sim_tron2.py signs:
            # self.cmd_vel[0] = -self._deadzone(ly) * self.config.max_cmd_vx
            # self.cmd_vel[1] = -self._deadzone(lx) * self.config.max_cmd_vy
            # self.cmd_vel[2] = -self._deadzone(rx) * self.config.max_cmd_yaw
            twist.linear.x = -self.apply_deadzone(ly) * self.max_vx
            twist.linear.y = -self.apply_deadzone(lx) * self.max_vy
            twist.angular.z = -self.apply_deadzone(rx) * self.max_yaw
            
            self.cmd_vel_pub.publish(twist)
            
            # Button mapping:
            # L1 + Y -> WALK
            # L1 + A -> IDLE
            num_buttons = self.joystick.get_numbuttons()
            l1_idx = int(self.button_map.get('L1', 4))
            r1_idx = int(self.button_map.get('R1', 5))
            x_idx = int(self.button_map.get('X', 2))
            y_idx = int(self.button_map.get('Y', 3))

            l1 = self.joystick.get_button(l1_idx) if num_buttons > l1_idx else 0
            r1 = self.joystick.get_button(r1_idx) if num_buttons > r1_idx else 0
            x_btn = self.joystick.get_button(x_idx) if num_buttons > x_idx else 0
            y_btn = self.joystick.get_button(y_idx) if num_buttons > y_idx else 0
            
            if l1 and y_btn:
                self.mode_pub.publish(String(data="WALK"))
                rospy.loginfo("Joystick: Requesting WALK mode")
            elif l1 and x_btn:
                self.mode_pub.publish(String(data="IDLE"))
                rospy.loginfo("Joystick: Requesting IDLE mode")
            
            if r1:
                # In MuJoCo it just zeros cmd_vel. Here it's already doing it if sticks are centered.
                pass

            self.rate.sleep()

if __name__ == '__main__':
    try:
        teleop = JoystickTeleop()
        teleop.run()
    except rospy.ROSInterruptException:
        pass
    finally:
        pygame.quit()
