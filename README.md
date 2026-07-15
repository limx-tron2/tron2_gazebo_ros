<!--
  SPDX-FileCopyrightText: 2024-2026 LimX Dynamics Technology Co., Ltd.
  SPDX-License-Identifier: Apache-2.0
-->

[English](README.md) | [中文](README_zh-CN.md)

# TRON2 ROS workspace (`~/limx_ws/src`)

This directory is the TRON2 source space (catkin `src`) for ROS Noetic. It
contains two families of packages: those on the simulation side and those on
the control side.

## License & attribution

This repository is licensed under the **Apache License, Version 2.0**
(see [`LICENSE`](LICENSE)). Both ROS packages declare
`<license>Apache-2.0</license>` in their `package.xml`, consistent with
the top-level LICENSE.

Companion documents in this repository:

- [`NOTICE`](NOTICE) — required attribution notice.
- [`THIRD_PARTY_NOTICES.md`](THIRD_PARTY_NOTICES.md) — bundled SDK
  binaries (§2), Gazebo / ROS runtime deps (§4), and documentation
  media (§5). Rows still marked `⚠ TO CONFIRM` continue to block the
  first public tag.
- [`SECURITY.md`](SECURITY.md) — how to report a vulnerability;
  private-IP handling policy for `limxsdk-sim/include/limxsdk/apibase.h`.
- [`CONTRIBUTING.md`](CONTRIBUTING.md) — `catkin_make` workflow,
  no-CAD / no-binary policy, DCO sign-off.
- [`CHANGELOG.md`](CHANGELOG.md) — release notes and the remaining
  "Pending owner sign-off" list.

> **⚠ TO CONFIRM.** Any link that previously pointed at
> `../tron2-rl-deploy-ros/LICENSE` is **not** a licence grant for
> *this* repository — the section above, `NOTICE`, and
> `THIRD_PARTY_NOTICES.md` §1 are the authoritative status. Both
> `tron2_gazebo/package.xml` and `limxsdk-sim/package.xml` still
> declare non-open-source `<license>` values pending owner sign-off.

## Scope

**Included** in this repository:

- `tron2_gazebo` — a Gazebo Classic 11 `gazebo_ros_control` plugin
  (`Tron2HWSim`) that exposes a hybrid joint effort interface and
  bridges to the TRON2 SDK RobotCmd / RobotState API. See
  `tron2_gazebo/src/Tron2HWSim.cpp`.
- `limxsdk-sim` — simulation-side C++ SDK. Public headers under
  `limxsdk-sim/include/limxsdk/` plus pre-built binaries under
  `limxsdk-sim/lib/{aarch64,amd64,arm32,win}/`.
- Example worlds, launch files, and controller configs under
  `tron2_gazebo/{worlds,launch,config}/`.
- Reference GIFs and one deployment photo under `doc/`.

**Not included** — by design:

- **No vendor clearance** yet on the four bundled SDK binaries
  (`limxsdk-sim/lib/*/liblimxsdk_sim.so`,
  `limxsdk-sim/lib/win/limxsdk_sim.{dll,lib}`). Provenance,
  SHA-256, and re-distribution license are pending sign-off
  (`THIRD_PARTY_NOTICES.md` §2).
- No control policies (`.onnx`, `.pt`, `.pth`, `.ckpt`).
- No factory calibration values or per-serial calibration files.
- No firmware or bootloader artifacts.
- No motion / trajectory data (rosbags, MCAP, HDF5 captures).
- No production-network IP addresses, hostnames, or credentials
  (see `SECURITY.md`). The one private-IP literal in
  `limxsdk-sim/include/limxsdk/apibase.h` (`10.192.1.2`, an internal
  example) is a documented RFC-1918 placeholder and is enforced by CI.
- No customer-specific or site-specific configuration.

For the deployment stack, model weights, and the real-robot SDK,
see the sibling repositories in the `limx-tron2` organisation.

---

## 1. Directory layout

The recommended layout is:

```text
~/limx_ws/src
├── CMakeLists.txt                 # catkin top-level entry (usually a symlink)
├── robot-description              # robot model description package (top level under src)
├── limxsdk-lowlevel               # low-level SDK package (top level under src)
├── tron2-gazebo-ros               # simulation-side package collection
│   ├── limxsdk-sim
│   └── tron2_gazebo
└── tron2-rl-deploy-ros            # control / deployment package collection
    ├── onnxruntime_sdk
    ├── robot_common
    ├── tron2_controllers
    └── tron2_hw
```

> Note: catkin discovers packages recursively, so grouping into
> subdirectories does not affect the build as long as each ROS package
> contains a valid `package.xml` and `CMakeLists.txt`.

## 2. Requirements

- Ubuntu 20.04
- ROS Noetic (`ros-noetic-desktop-full` is recommended)
- Gazebo 11 (default for ROS Noetic)
- Common dependencies (install as needed):

```bash
sudo apt-get update
sudo apt-get install -y \
  ros-noetic-gazebo-ros-pkgs \
  ros-noetic-gazebo-ros-control \
  ros-noetic-ros-control \
  ros-noetic-ros-controllers \
  ros-noetic-controller-manager \
  ros-noetic-joint-state-controller \
  ros-noetic-rqt-controller-manager \
  ros-noetic-robot-state-publisher \
  libeigen3-dev
```

## 3. Workspace setup

Set up an algorithm-development workspace as follows:

- Open a Bash terminal.

- Create a new directory to hold the workspace. For example, create a
  directory called "limx_ws" under your home directory:

  ```
  mkdir -p ~/limx_ws/src
  ```

- Download the motion-control development interface:

  ```
  cd ~/limx_ws/src
  git clone https://github.com/limxdynamics/limxsdk-lowlevel.git
  ```

- Download the Gazebo simulator:

  ```
  cd ~/limx_ws/src
  git clone https://github.com/limxdynamics/tron1-gazebo-ros.git
  ```

- Download the robot model description:

  ```
  cd ~/limx_ws/src
  git clone https://github.com/limxdynamics/robot-description.git
  ```

Then, from the workspace root:

```bash
cd ~/limx_ws
source /opt/ros/noetic/setup.bash
catkin_make
```

After a successful build, source the environment:

```bash
source ~/limx_ws/devel/setup.bash
```

## 4. Running the examples

### 4.1 Launch the simulation (full deployment)

```bash
cd ~/limx_ws
source /opt/ros/noetic/setup.bash
source devel/setup.bash
# Bring up a full simulation with the Gazebo scene, hardware node, and controller.
roslaunch tron2_hw tron2_hw_sim.launch robot_type:=SF_TRON2A
```

`robot_type` can be switched to match your configuration
(for example `SF_TRON2A` / `WF_TRON2A`).

### 4.2 Controller only (simulation mode)

If you have already started the Gazebo scene manually, you can bring up
just the hardware node and the controller:

```bash
roslaunch tron2_hw tron2_controller_sim.launch robot_type:=SF_TRON2A
```

### 4.3 Real-hardware deployment

```bash
roslaunch tron2_hw tron2_hw.launch robot_type:=SF_TRON2A robot_ip:=10.192.1.2
```

## 5. Simulation vs real-hardware logic (important)

Bottom line: **the core control chain is identical**, namely:

- `tron2_hw_node` (`Tron2HW`) -> `RobotHWLoop` -> `controller_manager` -> `tron2_controller`

The two paths differ on the input side:

- Simulation (`tron2_hw_sim.launch`) additionally publishes `/cmd_vel` and
  `/tron2_controller/set_mode` by default.
- Real hardware (`tron2_hw.launch`) primarily uses the channels that the
  SDK subscribes to.

## 6. Configuration file locations

The main control parameters live at:

- `tron2-rl-deploy-ros/tron2_controllers/config/SF_TRON2A/params.yaml`
- `tron2-rl-deploy-ros/tron2_controllers/config/WF_TRON2A/params.yaml`

## 7. Screenshots / GIFs

### 7.1 Simulation deployment

![SF Gazebo](doc/sfgazebo-ezgif.com-video-to-gif-converter.gif)
![WF Gazebo](doc/wfgazebo.gif)

### 7.2 Real-hardware deployment

Suspend the robot before starting the controller during a real-hardware
deployment.

![Deploy](doc/deploy.jpg)

![SF](doc/sf.GIF)
![WF](doc/wf.GIF)

## 8. FAQ

- Package not found at launch:
  - Confirm you have run `source /opt/ros/noetic/setup.bash`.
  - Confirm you have run `source ~/limx_ws/devel/setup.bash`.
- Build errors after moving directories around:
  - Re-run `catkin_make` under `~/limx_ws`.
- Gazebo plugin / controller fails to load:
  - Confirm that `tron2_gazebo`, `tron2_hw`, and `tron2_controllers`
    have all built successfully.

---

## Verification

The commands below are the same ones CI runs (see
[`.github/workflows/ci.yml`](.github/workflows/ci.yml)); running
them locally before opening a PR saves review round-trips.

```bash
# 1. Workspace build
cd ~/limx_ws
source /opt/ros/noetic/setup.bash
catkin_make
source devel/setup.bash

# 2. package.xml well-formedness
xmllint --noout tron2_gazebo/package.xml limxsdk-sim/package.xml

# 3. Gazebo plugin descriptor
xmllint --noout tron2_gazebo/tron2_hw_sim_plugins.xml

# 4. Committed-binary deny-list (outside limxsdk-sim/lib/)
git ls-files | grep -iE '\.(onnx|pt|pth|ckpt|so|dll|dylib|lib|whl|bag|mcap)$' | \
    grep -v '^limxsdk-sim/lib/'

# 5. Bundled SDK binary checksums (⚠ TO CONFIRM in THIRD_PARTY_NOTICES.md §2)
sha256sum \
  limxsdk-sim/lib/aarch64/liblimxsdk_sim.so \
  limxsdk-sim/lib/amd64/liblimxsdk_sim.so \
  limxsdk-sim/lib/arm32/liblimxsdk_sim.so \
  limxsdk-sim/lib/win/limxsdk_sim.dll \
  limxsdk-sim/lib/win/limxsdk_sim.lib

# 6. Private-IP scan (allowlist: apibase.h placeholder)
grep -RnE '\b(10|172\.(1[6-9]|2[0-9]|3[01])|192\.168)\.[0-9]+\.[0-9]+\b' \
     tron2_gazebo limxsdk-sim | \
     grep -v 'limxsdk-sim/include/limxsdk/apibase.h' | \
     grep -v '127\.0\.0\.1'
```

---

## Cite & support

If you use this simulation stack in academic or public work, please
cite the repository:

```
@misc{limx_tron2_gazebo_ros_2026,
  title  = {TRON2 Gazebo / ROS simulation},
  author = {LimX Dynamics},
  year   = {2026},
  howpublished = {\url{https://github.com/limx-tron2/tron2-gazebo-ros}}
}
```

- **Bug reports / feature requests:**
  [GitHub Issues](https://github.com/limx-tron2/tron2-gazebo-ros/issues).
- **Questions / integration help:**
  [GitHub Discussions](https://github.com/limx-tron2/tron2-gazebo-ros/discussions).
- **Security reports:** email `contact@limxdynamics.com`; see
  [`SECURITY.md`](SECURITY.md).
- **Company / commercial contact:** <https://www.limxdynamics.com>.
