# Contributing to `tron2-gazebo-ros`

Thanks for helping improve the TRON2 Gazebo simulation stack. This
repository holds a Gazebo `gazebo_ros_control` plugin
(`tron2_gazebo`) and a simulation-side C++ SDK (`limxsdk-sim`) that
bridges the plugin to the TRON2 SDK RobotCmd / RobotState API. The
guidelines below aim to keep the plugin buildable on ROS 1 Noetic +
Gazebo 11 and, where practical, compatible with ROS 2.

## Table of contents

- [Ways to contribute](#ways-to-contribute)
- [Development setup](#development-setup)
- [Repository layout](#repository-layout)
- [Coding conventions](#coding-conventions)
- [Working with the bundled SDK binaries](#working-with-the-bundled-sdk-binaries)
- [Verification before opening a PR](#verification-before-opening-a-pr)
- [Commit messages](#commit-messages)
- [Pull request checklist](#pull-request-checklist)
- [Sign-off (DCO)](#sign-off-dco)
- [Code of conduct](#code-of-conduct)

## Ways to contribute

- Bug reports for the Gazebo plugin, hardware interface, or SDK bridge.
- ROS 2 port work behind `condition="$ROS_VERSION == 2"` blocks.
- Docs, launch-file cleanups, world / config fixes.
- CI improvements (`catkin_make` matrix, `xmllint`, deny-list scans).

We do **not** accept:

- New bundled binaries (`.so`, `.dll`, `.dylib`, `.lib`, wheels)
  without a completed `THIRD_PARTY_NOTICES.md` §2 row (source URL,
  build reproducer, SHA-256, re-distribution license). See
  `⚠ TO CONFIRM` rows in [`THIRD_PARTY_NOTICES.md`](THIRD_PARTY_NOTICES.md).
- CAD sources, control policies, model weights, firmware, factory
  calibration values, motion / bag data, or customer-specific
  configuration. This repo is simulation + bridging code only.
- Media that discloses individuals, office interiors, non-public
  products, or production infrastructure (private IPs, hostnames).

## Development setup

Prerequisites:

- Ubuntu 20.04
- ROS 1 Noetic (`ros-noetic-desktop-full`)
- Gazebo Classic 11 (ships with `ros-noetic-desktop-full`)
- Common ROS packages:

  ```bash
  sudo apt-get update
  sudo apt-get install -y \
    ros-noetic-gazebo-ros-pkgs \
    ros-noetic-gazebo-ros-control \
    ros-noetic-ros-control \
    ros-noetic-ros-controllers \
    ros-noetic-controller-manager \
    ros-noetic-joint-state-controller \
    ros-noetic-robot-state-publisher \
    libeigen3-dev libxml2-utils
  ```

Clone into a catkin workspace alongside the sibling repositories
(`robot-description`, `limxsdk-lowlevel`, `tron2-rl-deploy-ros`):

```bash
mkdir -p ~/limx_ws/src && cd ~/limx_ws/src
git clone https://github.com/limx-tron2/tron2-gazebo-ros.git
# plus robot-description, limxsdk-lowlevel, tron2-rl-deploy-ros

cd ~/limx_ws
source /opt/ros/noetic/setup.bash
catkin_make
source devel/setup.bash
```

## Repository layout

```
tron2-gazebo-ros/
├── tron2_gazebo/          # Gazebo hardware-sim plugin (ROS package)
│   ├── src/Tron2HWSim.cpp # joint effort <-> RobotCmd/RobotState bridge
│   ├── include/…          # plugin headers
│   ├── config/            # controller configs consumed by launch files
│   ├── launch/            # example launch files
│   ├── worlds/            # Gazebo worlds
│   ├── scripts/           # convenience scripts
│   ├── tron2_hw_sim_plugins.xml
│   ├── CMakeLists.txt
│   └── package.xml         (⚠ TO CONFIRM `<license>` — do NOT edit)
├── limxsdk-sim/            # simulation-side SDK (ROS package)
│   ├── include/limxsdk/    # public C++ headers
│   ├── lib/{aarch64,amd64,arm32,win}/   # bundled binaries ⚠ TO CONFIRM
│   ├── CMakeLists.txt
│   └── package.xml         (⚠ TO CONFIRM `<license>` — do NOT edit)
├── doc/                    # README media (⚠ TO CONFIRM EXIF/content)
└── (open-source scaffolding: README, NOTICE, THIRD_PARTY_NOTICES.md,
    SECURITY.md, CONTRIBUTING.md, CHANGELOG.md, .github/, .gitignore)
```

- ROS package names (`tron2_gazebo`, `limxsdk_sim`) are **stable** and
  must not be renamed — downstream `<depend>` declarations rely on
  them.
- Do not commit CAD source files or vendor CAD (this repo owns
  none). See `.gitignore` for the deny-list.
- Do not commit editor / OS junk — see `.gitignore`.

## Coding conventions

- C++14 (matches the current CMake defaults for the two packages).
- 4-space indent for CMake, 2-space for XML, and follow the existing
  Google-ish style in `Tron2HWSim.cpp` / `Tron2HWSim.h`.
- Prefer `#pragma once` in headers.
- Guard ROS 2 additions behind `condition="$ROS_VERSION == 2"` in
  `package.xml` and behind CMake variables in `CMakeLists.txt`.
- Every source and header file should carry an SPDX comment once the
  top-level license is decided (do **not** invent one now — see
  `THIRD_PARTY_NOTICES.md` §1).

## Working with the bundled SDK binaries

`limxsdk-sim/lib/{aarch64,amd64,arm32,win}/` currently contains
pre-built binaries with `⚠ TO CONFIRM` provenance. Until the SDK
owner has completed the `THIRD_PARTY_NOTICES.md` §2 rows:

- Do **not** add new binaries.
- Do **not** rebuild and overwrite existing binaries as part of an
  unrelated PR — binary changes must be their own commit with a
  provenance note and a fresh SHA-256 in `THIRD_PARTY_NOTICES.md`.
- CI enforces a deny-list on `*.so` / `*.dll` / `*.dylib` / `*.lib` /
  `*.whl` / `*.onnx` / `*.pt` / `*.bag` / `*.mcap` outside
  `limxsdk-sim/lib/`.

## Verification before opening a PR

Run all of the following and paste the summary into the PR
description:

```bash
# 1. Full workspace build
cd ~/limx_ws && catkin_make
source devel/setup.bash

# 2. Every package.xml is well-formed XML
xmllint --noout tron2_gazebo/package.xml limxsdk-sim/package.xml

# 3. Gazebo plugin XML sanity
xmllint --noout tron2_gazebo/tron2_hw_sim_plugins.xml

# 4. No unexpected private-IP literals outside apibase.h
grep -RnE '\b(10|172\.(1[6-9]|2[0-9]|3[01])|192\.168)\.[0-9]+\.[0-9]+\b' \
     tron2_gazebo limxsdk-sim | \
     grep -v 'limxsdk-sim/include/limxsdk/apibase.h' | \
     grep -v '127\.0\.0\.1'

# 5. No un-approved bundled binaries
git ls-files | grep -iE '\.(onnx|pt|pth|ckpt|so|dll|dylib|lib|whl|bag|mcap)$' | \
     grep -v '^limxsdk-sim/lib/'

# 6. No unresolved TODO / proprietary markers in top-level docs
grep -RniE 'proprietary|confidential|todo: license|unknown license' \
     README.md THIRD_PARTY_NOTICES.md CHANGELOG.md CONTRIBUTING.md \
     SECURITY.md NOTICE
```

CI runs the same steps; local pre-checks save review round-trips.

Optional smoke test (needs a display or `xvfb`):

```bash
roslaunch tron2_hw tron2_hw_sim.launch robot_type:=SF_TRON2A
```

## Commit messages

Follow Conventional Commits:

```
type(scope): short imperative summary

Longer explanation if needed.

Signed-off-by: Your Name <you@example.com>
```

`type` ∈ `feat | fix | docs | refactor | chore | ci | test | build`.
`scope` is usually a package name (`tron2_gazebo`, `limxsdk-sim`) or
`meta` for repo-wide scaffolding changes.

## Pull request checklist

- [ ] `catkin_make` succeeds against a clean workspace.
- [ ] `package.xml` files pass `xmllint --noout`.
- [ ] `<license>` values in existing `package.xml` files were **not**
      silently changed (they remain a legal / OSPO decision).
- [ ] No new bundled binaries without a completed
      `THIRD_PARTY_NOTICES.md` §2 row.
- [ ] Private-IP scan (verification step 4) is clean, or the new
      literal is documented in `SECURITY.md`.
- [ ] `CHANGELOG.md` has an entry under `## [Unreleased]`.
- [ ] No calibration values, firmware, weights, logs, bags, or media
      that discloses individuals / sites.
- [ ] DCO sign-off on every commit.

## Sign-off (DCO)

We use the [Developer Certificate of Origin](https://developercertificate.org/).
Every commit must be signed off:

```bash
git commit -s -m "your message"
```

Signing off certifies that you have the right to submit the change
under the repository's license (once the top-level license is
resolved — see `THIRD_PARTY_NOTICES.md` §1).

## Code of conduct

Be respectful and constructive. Reports to
`contact@limxdynamics.com`.
