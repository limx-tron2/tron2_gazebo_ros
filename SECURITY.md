# Security Policy

## Scope

`tron2-gazebo-ros` ships a Gazebo hardware-simulation plugin
(`tron2_gazebo`) plus a simulation-side C++ SDK (`limxsdk-sim`) that
bridges Gazebo joint state / effort commands to the TRON2 SDK
RobotCmd / RobotState interface consumed by downstream controllers.

Because the plugin sits on the **control path** (joint effort
commands into the simulator, published RobotState back to the
controller), the primary security concerns for this repository are:

- Malicious payloads over the SDK IPC channel that could crash or
  hijack a downstream controller.
- Inadvertent disclosure of internal infrastructure metadata
  (private IPs, hostnames, developer paths) inside bundled binaries,
  headers, or documentation media.
- Supply-chain tampering of the bundled shared libraries
  (`limxsdk-sim/lib/{aarch64,amd64,arm32,win}/`) between publication
  and consumption.
- Malformed URDF / world / xacro that crashes the Gazebo plugin
  loader (`Tron2HWSim`).

Control-path vulnerabilities in the real-robot deployment stack
(`tron2-rl-deploy-ros`, `limxsdk-lowlevel`, etc.) belong to those
repositories, not here.

## Known placeholders and internal identifiers

- `limxsdk-sim/include/limxsdk/apibase.h:35` contains, as a
  documentation example inside a C++ Doxygen comment, the literal
  `10.192.1.2`. This value is a documentation example only; it is
  not the address of any LimX production network. The header comment
  says verbatim: "For simulation, it is typically set to '127.0.0.1',
  while for a real robot, it may be set to '10.192.1.2'." Downstream
  users must substitute their robot's actual IP at runtime.
- Command examples in `README.md`, `README_zh-CN.md`, and
  `tron2_hw/*` launch files use `<robot-ip>` as a placeholder token
  — substitute your robot's IP before running.
- CI (`.github/workflows/ci.yml`) rejects any new private-IP literal
  outside `apibase.h`'s single documented instance.
- `doc/deploy.jpg` and the `doc/*.GIF` / `doc/*.gif` captures are
  subject to a manual EXIF and content review before every release
  (see [`THIRD_PARTY_NOTICES.md`](THIRD_PARTY_NOTICES.md) §5).

## Supported versions

Only the tip of the `main` branch and the most recent tagged release
receive security fixes. Older tags are provided as-is.

| Version   | Supported |
|-----------|-----------|
| `main`    | ✅        |
| Latest tag| ✅        |
| Older tags| ❌        |

## Reporting a vulnerability

**Do not** open a public issue for security reports.

Email: **contact@limxdynamics.com**
Subject prefix: `[tron2-gazebo-ros]`

Please include:

- Affected file(s) and commit / tag.
- A minimal reproducer or proof of concept.
- Impact assessment (e.g., "malformed RobotCmd crashes Tron2HWSim",
  "bundled `.so` on architecture X has unexpected SONAME / RPATH").
- Your preferred disclosure timeline and contact.

We aim to acknowledge reports within **3 business days** and provide
a remediation plan or an initial mitigation within **14 calendar days**.
We support coordinated disclosure; please do not publish details until
a fix or advisory is available.

## Out of scope

- Bugs in third-party components (Gazebo, ros_control, urdfdom, etc.)
  — report those upstream.
- Physical safety of the real robot — report to the deployment
  repositories or to LimX product support.
- Requests to publish production infrastructure IPs, hostnames,
  credentials, or calibration data — intentionally excluded.

## Safe harbor

Good-faith security research that follows this policy will not be
pursued legally by LimX Dynamics. Please respect user privacy, avoid
service disruption, and do not access data beyond what is necessary
to demonstrate the issue.
