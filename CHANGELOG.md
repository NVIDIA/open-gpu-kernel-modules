# Changelog

## Release 545 Entries

### [545.29.06] 2023-11-22

#### Fixed

- The brightness control of NVIDIA seems to be broken, [#573](https://github.com/NVIDIA/open-gpu-kernel-modules/issues/573)

### [545.29.02] 2023-10-31

### [545.23.06] 2023-10-17

#### Fixed

- Fix always-false conditional, [#493](https://github.com/NVIDIA/open-gpu-kernel-modules/pull/493) by @meme8383

#### Added

- Added beta-quality support for GeForce and Workstation GPUs. Please see the "Open Linux Kernel Modules" chapter in the NVIDIA GPU driver end user README for details.

## Release 535 Entries

### [535.129.03] 2023-10-31

### [535.113.01] 2023-09-21

#### Fixed

- Fixed building main against current centos stream 8 fails, [#550](https://github.com/NVIDIA/open-gpu-kernel-modules/issues/550) by @airlied

### [535.104.05] 2023-08-22

### [535.98] 2023-08-08

### [535.86.10] 2023-07-31

### [535.86.05] 2023-07-18

### [535.54.03] 2023-06-14

### [535.43.02] 2023-05-30

#### Fixed

- Fixed console restore with traditional VGA consoles.

#### Added

- Added support for Run Time D3 (RTD3) on Ampere and later GPUs.
- Added support for G-Sync on desktop GPUs.

## Release 530 Entries

### [530.41.03] 2023-03-23

### [530.30.02] 2023-02-28

#### Changed

- GSP firmware is now distributed as `gsp_tu10x.bin` and `gsp_ga10x.bin` to better reflect the GPU architectures supported by each firmware file in this release.
    - The .run installer will continue to install firmware to /lib/firmware/nvidia/<version> and the nvidia.ko kernel module will load the appropriate firmware for each GPU at runtime.
  
#### Fixed

- Add support for resizable BAR on Linux when NVreg_EnableResizableBar=1 module param is set. [#3](https://github.com/NVIDIA/open-gpu-kernel-modules/pull/3) by @sjkelly

#### Added

- Support for power management features like Suspend, Hibernate and Resume.

## Release 525 Entries

### [525.147.05] 2023-10-31

#### Fixed

- Fix nvidia_p2p_get_pages(): Fix double-free in register-callback error path, [#557](https://github.com/NVIDIA/open-gpu-kernel-modules/pull/557) by @BrendanCunningham

### [525.125.06] 2023-06-26

### [525.116.04] 2023-05-09

### [525.116.03] 2023-04-25

### [525.105.17] 2023-03-30

### [525.89.02] 2023-02-08

### [525.85.12] 2023-01-30

### [525.85.05] 2023-01-19

#### Fixed

- Fix build problems with Clang 15.0, [#377](https://github.com/NVIDIA/open-gpu-kernel-modules/issues/377) by @ptr1337

### [525.78.01] 2023-01-05

### [525.60.13] 2022-12-05

### [525.60.11] 2022-11-28

#### Fixed

- Fixed nvenc compatibility with usermode clients [#104](https://github.com/NVIDIA/open-gpu-kernel-modules/issues/104)

### [525.53] 2022-11-10

#### Changed

- GSP firmware is now distributed as multiple firmware files: this release has `gsp_tu10x.bin` and `gsp_ad10x.bin` replacing `gsp.bin` from previous releases.
    - Each file is named after a GPU architecture and supports GPUs from one or more architectures. This allows GSP firmware to better leverage each architecture's capabilities.
    - The .run installer will continue to install firmware to `/lib/firmware/nvidia/<version>` and the `nvidia.ko` kernel module will load the appropriate firmware for each GPU at runtime.

#### Fixed

- Add support for IBT (indirect branch tracking) on supported platforms, [#256](https://github.com/NVIDIA/open-gpu-kernel-modules/issues/256) by @rnd-ash
- Return EINVAL when [failing to] allocating memory, [#280](https://github.com/NVIDIA/open-gpu-kernel-modules/pull/280) by @YusufKhan-gamedev
- Fix various typos in nvidia/src/kernel, [#16](https://github.com/NVIDIA/open-gpu-kernel-modules/pull/16) by @alexisgeoffrey
- Added support for rotation in X11, Quadro Sync, Stereo, and YUV 4:2:0 on Turing.

## Release 520 Entries

### [520.61.07] 2022-10-20

### [520.56.06] 2022-10-12

#### Added

- Introduce support for GeForce RTX 4090 GPUs.

### [520.61.05] 2022-10-10

#### Added

- Introduce support for NVIDIA H100 GPUs.

#### Fixed

- Fix/Improve Makefile, [#308](https://github.com/NVIDIA/open-gpu-kernel-modules/pull/308/) by @izenynn
- Make nvLogBase2 more efficient, [#177](https://github.com/NVIDIA/open-gpu-kernel-modules/pull/177/) by @DMaroo
- nv-pci: fixed always true expression, [#195](https://github.com/NVIDIA/open-gpu-kernel-modules/pull/195/) by @ValZapod

## Release 515 Entries

### [515.76] 2022-09-20

#### Fixed

- Improved compatibility with new Linux kernel releases
- Fixed possible excessive GPU power draw on an idle X11 or Wayland desktop when driving high resolutions or refresh rates

### [515.65.07] 2022-10-19

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

