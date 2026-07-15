<!--
Thanks for contributing to tron2-gazebo-ros!
Please fill in the sections below. Delete any that are not applicable.
-->

## Summary

<!-- One paragraph: what and why. -->

## Type of change

- [ ] `fix`     — corrects a defect in the plugin / SDK bridge
- [ ] `feat`    — new capability (plugin option, world, launch, API)
- [ ] `build`   — CMake / catkin / packaging changes
- [ ] `docs`    — README, THIRD_PARTY_NOTICES, CONTRIBUTING, or SECURITY
- [ ] `ci`      — GitHub Actions or verification tooling
- [ ] `chore`   — repo maintenance (deps, formatting, cleanup)

## Affected packages

- [ ] `tron2_gazebo`
- [ ] `limxsdk-sim`
- [ ] Meta / repo-wide (docs, CI, `.gitignore`, `.github/`)

## Verification

Paste the output (or a summary) of the local verification steps from
`CONTRIBUTING.md#verification-before-opening-a-pr`:

```text
catkin_make: ...
xmllint (package.xml): ...
xmllint (plugin descriptor): ...
private-IP scan: ...
committed-binary deny-list: ...
doc marker scan: ...
```

## Provenance & licensing

<!-- Required if the PR touches limxsdk-sim/lib/, doc/, or any package.xml. -->

- [ ] `<license>` values in `tron2_gazebo/package.xml` and
      `limxsdk-sim/package.xml` are **unchanged** (still an owner /
      legal decision — see `THIRD_PARTY_NOTICES.md` §1).
- [ ] No new bundled binary was added under `limxsdk-sim/lib/`, or if
      one was, `THIRD_PARTY_NOTICES.md` §2 has a fully-populated row
      (source URL, build reproducer, SHA-256, redistribution license).
- [ ] No committed binary appears outside `limxsdk-sim/lib/`.
- [ ] `doc/` media (if changed) has been EXIF-stripped and reviewed
      for people / office / non-public product visibility.

## Excluded artifacts

- [ ] This PR does **not** add: control policies (`.onnx`, `.pt`),
      factory calibration values, firmware, rosbags / MCAP, customer
      data, production-network IPs, or credentials.

## Checklist

- [ ] `catkin_make` succeeds against a clean workspace.
- [ ] `CHANGELOG.md` has an entry under `## [Unreleased]`.
- [ ] All commits are DCO-signed (`git commit -s`).
- [ ] CI is expected to pass.

## Related issues

<!-- Fixes #123 / Refs #456 -->
