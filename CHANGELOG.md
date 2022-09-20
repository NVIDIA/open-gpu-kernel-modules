# Changelog

## Release 515 Entries

### [515.76] 2022-09-20

#### Fixed

- Improved compatibility with new Linux kernel releases
- Fixed possible excessive GPU power draw on an idle X11 or Wayland desktop when driving high resolutions or refresh rates

### [515.65.01] 2022-08-02

#### Fixed

- Collection of minor fixes to issues, [#6](https://github.com/NVIDIA/open-gpu-kernel-modules/pull/61) by @Joshua-Ashton
- Remove unnecessary use of acpi_bus_get_device().

### [515.57] 2022-06-28

#### Fixed

- Backtick is deprecated, [#273](https://github.com/NVIDIA/open-gpu-kernel-modules/pull/273) by @arch-user-france1

### [515.48.07] 2022-05-31

#### Added

- List of compatible GPUs in README.md.

#### Fixed

- Fix various README capitalizations, [#8](https://github.com/NVIDIA/open-gpu-kernel-modules/pull/8) by @27lx 
- Automatically tag bug report issues, [#15](https://github.com/NVIDIA/open-gpu-kernel-modules/pull/15) by @thebeanogamer
- Improve conftest.sh Script, [#37](https://github.com/NVIDIA/open-gpu-kernel-modules/pull/37) by @Nitepone
- Update HTTP link to HTTPS, [#101](https://github.com/NVIDIA/open-gpu-kernel-modules/pull/101) by @alcaparra
- moved array sanity check to before the array access, [#117](https://github.com/NVIDIA/open-gpu-kernel-modules/pull/117) by @RealAstolfo
- Fixed some typos, [#122](https://github.com/NVIDIA/open-gpu-kernel-modules/pull/122) by @FEDOyt
- Fixed capitalization, [#123](https://github.com/NVIDIA/open-gpu-kernel-modules/pull/123) by @keroeslux
- Fix typos in NVDEC Engine Descriptor, [#126](https://github.com/NVIDIA/open-gpu-kernel-modules/pull/126) from @TrickyDmitriy
- Extranous apostrohpes in a makefile script [sic], [#14](https://github.com/NVIDIA/open-gpu-kernel-modules/issues/14) by @kiroma
- HDMI no audio @ 4K above 60Hz, [#75](https://github.com/NVIDIA/open-gpu-kernel-modules/issues/75) by @adolfotregosa
- dp_configcaps.cpp:405: array index sanity check in wrong place?, [#110](https://github.com/NVIDIA/open-gpu-kernel-modules/issues/110) by @dcb314
- NVRM kgspInitRm_IMPL: missing NVDEC0 engine, cannot initialize GSP-RM, [#116](https://github.com/NVIDIA/open-gpu-kernel-modules/issues/116) by @kfazz
- ERROR: modpost: "backlight_device_register" [...nvidia-modeset.ko] undefined, [#135](https://github.com/NVIDIA/open-gpu-kernel-modules/issues/135) by @sndirsch
- aarch64 build fails, [#151](https://github.com/NVIDIA/open-gpu-kernel-modules/issues/151) by @frezbo

### [515.43.04] 2022-05-11

- Initial release.

