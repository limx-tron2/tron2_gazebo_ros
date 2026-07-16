# Third-Party Notices

`tron2-gazebo-ros` bundles a Gazebo hardware-simulation plugin
(`tron2_gazebo`) and a simulation-side SDK (`limxsdk-sim`) that
together let TRON2 control stacks drive the robot inside Gazebo.

The repository is licensed under **Apache-2.0** (see [`LICENSE`](LICENSE)
and [`NOTICE`](NOTICE)); both ROS packages declare
`<license>Apache-2.0</license>` consistently. Remaining rows marked
`⚠ TO CONFIRM` below (bundled SDK binaries, doc media, etc.) block the
first public tag until the responsible owners sign off.

> **Status:** items marked `⚠ TO CONFIRM` are pending sign-off from
> the SDK / robotics / product / legal owners. Do not cut a public
> release while any `⚠ TO CONFIRM` entry remains.

---

## 1. ROS package licensing — resolved (2026-07-14)

| Path | `<license>` | Status |
|------|-------------|--------|
| `tron2_gazebo/package.xml` | `Apache-2.0` | ✅ resolved — matches top-level LICENSE |
| `limxsdk-sim/package.xml`  | `Apache-2.0` | ✅ resolved — matches top-level LICENSE |

The repository now ships a top-level `LICENSE` file (Apache-2.0, identical
to the sibling `robot-description/LICENSE`). The bundled SDK binaries in
§2 are covered by the Apache-2.0 grant for LimX-authored sources; their
own provenance / re-distribution status is tracked in §2.

---

## 2. Bundled SDK binaries (`limxsdk-sim/lib/`) — ⚠ TO CONFIRM

`limxsdk-sim` ships four pre-built binaries. They are installed by
`limxsdk-sim/CMakeLists.txt` and consumed at runtime by
`tron2_gazebo/src/Tron2HWSim.cpp`.

| Path | Kind (`file` output) | Size (bytes) | SHA-256 | Provenance | Re-distribution |
|------|----------------------|-------------:|---------|------------|-----------------|
| `limxsdk-sim/lib/aarch64/liblimxsdk_sim.so` | ELF 64-bit LSB shared object, ARM aarch64, GNU/Linux, dynamically linked | 10 449 192 | `75382fa96043a1205bf9ed8649f0a433231f91239f44e46565c8646b9727258f` | ⚠ TO CONFIRM | ⚠ TO CONFIRM |
| `limxsdk-sim/lib/amd64/liblimxsdk_sim.so`   | ELF 64-bit LSB shared object, x86-64, GNU/Linux, dynamically linked     | 11 319 848 | `a133318ff2b1b98cbb84401160facbdc6c58e5065ea68b09056678ccd8c6fa76` | ⚠ TO CONFIRM | ⚠ TO CONFIRM |
| `limxsdk-sim/lib/arm32/liblimxsdk_sim.so`   | ELF 32-bit LSB shared object, ARM, EABI5, GNU/Linux, dynamically linked |  9 587 220 | `6b970a61a457ae0b32573a78c69703843e83bbf026ae6953c5f4114aedd10450` | ⚠ TO CONFIRM | ⚠ TO CONFIRM |
| `limxsdk-sim/lib/win/limxsdk_sim.dll`       | PE32+ executable (DLL) x86-64, MS Windows                               |  5 464 064 | `c21ba55f7126f2a46b6e3d74f69aa23d1257b85712621ed97b57f1c2751835e7` | ⚠ TO CONFIRM | ⚠ TO CONFIRM |
| `limxsdk-sim/lib/win/limxsdk_sim.lib`       | Current ar archive (Windows import library)                             |    151 224 | `a315f4c8e6a5dd50c8a2bc47930c40354c617b8b854b356fbbe1b54abf969eea` | ⚠ TO CONFIRM | ⚠ TO CONFIRM |

Evidence collected 2026-07-16: SHA-256 digests were computed by
`sha256sum` on the tracked working-tree files (see the reproduce command
in §2.1 below); file-type strings are the exact `file(1)` output. These
facts are agent-collected evidence, not a rights determination — the two
right-most columns remain `⚠ TO CONFIRM` pending SDK-owner sign-off.

**Owner action required.** For each binary the SDK owner must:

1. Publish the source repository / release URL, commit / tag, and
   build reproducer used to produce the artifact.
2. Confirm the SHA-256 recorded above matches an authorised build.
3. State the license under which the binary may be re-distributed
   (must be compatible with the top-level license chosen in §1).
4. Confirm that the binary contains no third-party code with
   incompatible redistribution terms.

### 2.1 Reproduce the evidence

```bash
sha256sum \
  limxsdk-sim/lib/aarch64/liblimxsdk_sim.so \
  limxsdk-sim/lib/amd64/liblimxsdk_sim.so \
  limxsdk-sim/lib/arm32/liblimxsdk_sim.so \
  limxsdk-sim/lib/win/limxsdk_sim.dll \
  limxsdk-sim/lib/win/limxsdk_sim.lib
file  limxsdk-sim/lib/aarch64/liblimxsdk_sim.so \
      limxsdk-sim/lib/amd64/liblimxsdk_sim.so \
      limxsdk-sim/lib/arm32/liblimxsdk_sim.so \
      limxsdk-sim/lib/win/limxsdk_sim.dll \
      limxsdk-sim/lib/win/limxsdk_sim.lib
```

Until the two `⚠ TO CONFIRM` columns are cleared, this repository
cannot be published under any open-source license.

---

## 3. SDK headers (`limxsdk-sim/include/limxsdk/`)

Header files author-owned by LimX Dynamics. Their license follows the
resolution of §1. They declare the runtime C++ API consumed by
`tron2_gazebo`. Notes:

- `limxsdk-sim/include/limxsdk/apibase.h` retains a documentation
  example literal `10.192.1.2` in a Doxygen comment (see
  `SECURITY.md`). Command examples in shipped documentation use
  `<robot-ip>` as a placeholder token. Both are agent-cleared per
  the 2026-07-16 owner decision.

---

## 4. Runtime / build dependencies (not redistributed)

The following are runtime dependencies only — this repository does
not vendor them, but downstream users need them to build and run.

| Component | Purpose | License | Source |
|-----------|---------|---------|--------|
| ROS 1 (Noetic) | Build system, plugin loading, `controller_manager` | BSD-3-Clause | http://wiki.ros.org/noetic |
| Gazebo Classic 11 | Physics + rendering | Apache-2.0 | http://gazebosim.org |
| `gazebo_ros_pkgs`, `gazebo_ros_control` | ROS ↔ Gazebo bridge | Apache-2.0 / BSD | ROS distribution |
| `ros_control`, `ros_controllers` | Controller manager framework | BSD-3-Clause | ROS distribution |
| `hardware_interface`, `realtime_tools` | Real-time joint interface | BSD | ROS distribution |
| Eigen3 | Linear algebra | MPL-2.0 | https://eigen.tuxfamily.org |
| `robot_common` | LimX shared ROS utilities | ⚠ TO CONFIRM (LimX package, license inherited from that repo) | LimX sibling repo |

None of the above are bundled here; users obtain and license them
independently.

---

## 5. Documentation media (`doc/`) — ⚠ TO CONFIRM

| Path | Kind | Provenance | License | EXIF-stripped? |
|------|------|------------|---------|:--------------:|
| `doc/deploy.jpg` | Photo of physical deployment | ⚠ TO CONFIRM | ⚠ TO CONFIRM | ⚠ |
| `doc/sf.GIF`, `doc/sfgazebo-ezgif.com-video-to-gif-converter.gif` | Simulation / real-robot capture | ⚠ TO CONFIRM | ⚠ TO CONFIRM | ⚠ |
| `doc/wf.GIF`, `doc/wfgazebo.gif` | Simulation / real-robot capture | ⚠ TO CONFIRM | ⚠ TO CONFIRM | ⚠ |

The two `.GIF` files are large (≈ 50 MB and ≈ 47 MB); confirm the
raw source and whether smaller re-encoded versions should replace
them before publishing.

Before release, run and clean:

```bash
exiftool doc/*.jpg doc/*.gif doc/*.GIF | \
  grep -iE '(gps|serial|make|model|software|author|artist|copyright)'
exiftool -all= doc/*.jpg doc/*.gif doc/*.GIF   # destructive; commit first
```

Also confirm no individual faces, office interiors, license plates,
or non-public products are visible in `deploy.jpg` and the `.GIF`
captures.

---

## 6. What this repository does **not** include

- No control policies (`.onnx`, `.pt`, `.pth`).
- No factory calibration values or per-serial calibration files.
- No motion / bag / trajectory data.
- No firmware.
- No customer- or site-specific configuration.
- No production robot IP addresses or credentials (see
  [`SECURITY.md`](SECURITY.md)).

---

## 7. Update procedure

Whenever a bundled binary, image, or upstream reference is added or
changed:

1. Update the corresponding row in this file.
2. Re-run the SHA-256 checksum command in §2 and the EXIF strip in §5.
3. If the change touches an `⚠ TO CONFIRM` row, block the merge on
   written sign-off from the responsible owner (see
   `.github/CODEOWNERS`).
