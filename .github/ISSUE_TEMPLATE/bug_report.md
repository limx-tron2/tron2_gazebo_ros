---
name: Bug report
about: Defect in the Gazebo plugin, hardware interface, or sim SDK
title: "[bug] <short summary>"
labels: bug
assignees: ''
---

## Affected package(s)

- [ ] `tron2_gazebo` (Gazebo hardware-sim plugin)
- [ ] `limxsdk-sim` (simulation-side SDK)
- [ ] Repo scaffolding (CI, docs, templates)

## Affected file(s)

<!-- e.g. tron2_gazebo/src/Tron2HWSim.cpp, limxsdk-sim/include/limxsdk/apibase.h -->

- File(s):
- Commit / tag:

## Environment

- OS + arch: (e.g. Ubuntu 20.04 x86_64)
- ROS distribution: (Noetic / Humble / other)
- Gazebo version: (Classic 11 / Ignition / other)
- Compiler: (gcc / clang, version)
- Robot type / launch args (`robot_type:=SF_TRON2A` / `WF_TRON2A` / …):

## Expected behavior

<!-- What the plugin / SDK should do. -->

## Actual behavior

<!-- Log lines, screenshots, controller output, Gazebo console messages. -->

## Minimal reproduction

```bash
# commands that reproduce it locally, ideally from a fresh workspace
source /opt/ros/noetic/setup.bash
catkin_make
source devel/setup.bash
roslaunch tron2_hw tron2_hw_sim.launch robot_type:=SF_TRON2A
```

## Additional context

<!-- Cross-links to related issues, upstream Gazebo / ros_control quirks, etc. -->

## Checklist

- [ ] I have searched existing issues.
- [ ] I have included the exact commit / tag.
- [ ] I am **not** reporting a security issue (those go to
      `contact@limxdynamics.com` per `SECURITY.md`).
