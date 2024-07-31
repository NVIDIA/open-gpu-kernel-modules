# NVIDIA Linux Open GPU Kernel Module Source

This is the source release of the NVIDIA Linux open GPU kernel modules,
version 560.31.02.


## How to Build

To build:

    make modules -j$(nproc)

To install, first uninstall any existing NVIDIA kernel modules.  Then,
as root:

    make modules_install -j$(nproc)

Note that the kernel modules built here must be used with GSP
firmware and user-space NVIDIA GPU driver components from a corresponding
560.31.02 driver release.  This can be achieved by installing
the NVIDIA GPU driver from the .run file using the `--no-kernel-modules`
option.  E.g.,

    sh ./NVIDIA-Linux-[...].run --no-kernel-modules


## Supported Target CPU Architectures

Currently, the kernel modules can be built for x86_64 or aarch64.
If cross-compiling, set these variables on the make command line:

    TARGET_ARCH=aarch64|x86_64
    CC
    LD
    AR
    CXX
    OBJCOPY

E.g.,

    # compile on x86_64 for aarch64
    make modules -j$(nproc)         \
        TARGET_ARCH=aarch64         \
        CC=aarch64-linux-gnu-gcc    \
        LD=aarch64-linux-gnu-ld     \
        AR=aarch64-linux-gnu-ar     \
        CXX=aarch64-linux-gnu-g++   \
        OBJCOPY=aarch64-linux-gnu-objcopy


## Other Build Knobs

NV_VERBOSE - Set this to "1" to print each complete command executed;
    otherwise, a succinct "CC" line is printed.

DEBUG - Set this to "1" to build the kernel modules as debug.  By default, the
    build compiles without debugging information.  This also enables
    various debug log messages in the kernel modules.

These variables can be set on the make command line.  E.g.,

    make modules -j$(nproc) NV_VERBOSE=1


## Supported Toolchains

Any reasonably modern version of GCC or Clang can be used to build the
kernel modules.  Note that the kernel interface layers of the kernel
modules must be built with the toolchain that was used to build the
kernel.


## Supported Linux Kernel Versions

The NVIDIA open kernel modules support the same range of Linux kernel
versions that are supported with the proprietary NVIDIA kernel modules.
This is currently Linux kernel 4.15 or newer.


## How to Contribute

Contributions can be made by creating a pull request on
https://github.com/NVIDIA/open-gpu-kernel-modules
We'll respond via GitHub.

Note that when submitting a pull request, you will be prompted to accept
a Contributor License Agreement.

This code base is shared with NVIDIA's proprietary drivers, and various
processing is performed on the shared code to produce the source code that is
published here.  This has several implications for the foreseeable future:

* The GitHub repository will function mostly as a snapshot of each driver
  release.

* We do not expect to be able to provide revision history for individual
  changes that were made to NVIDIA's shared code base.  There will likely
  only be one git commit per driver release.

* We may not be able to reflect individual contributions as separate
  git commits in the GitHub repository.

* Because the code undergoes various processing prior to publishing here,
  contributions made here require manual merging to be applied to the shared
  code base.  Therefore, large refactoring changes made here may be difficult to
  merge and accept back into the shared code base.  If you have large
  refactoring to suggest, please contact us in advance, so we can coordinate.


## How to Report Issues

Problems specific to the Open GPU Kernel Modules can be reported in the
Issues section of the https://github.com/NVIDIA/open-gpu-kernel-modules
repository.

Further, any of the existing bug reporting venues can be used to communicate
problems to NVIDIA, such as our forum:

https://forums.developer.nvidia.com/c/gpu-graphics/linux/148

or linux-bugs@nvidia.com.

Please see the 'NVIDIA Contact Info and Additional Resources' section
of the NVIDIA GPU Driver README for details.

Please see the separate [SECURITY.md](SECURITY.md) document if you
believe you have discovered a security vulnerability in this software.


## Kernel Interface and OS-Agnostic Components of Kernel Modules

Most of NVIDIA's kernel modules are split into two components:

* An "OS-agnostic" component: this is the component of each kernel module
  that is independent of operating system.

* A "kernel interface layer": this is the component of each kernel module
  that is specific to the Linux kernel version and configuration.

When packaged in the NVIDIA .run installation package, the OS-agnostic
component is provided as a binary: it is large and time-consuming to
compile, so pre-built versions are provided so that the user does
not have to compile it during every driver installation.  For the
nvidia.ko kernel module, this component is named "nv-kernel.o_binary".
For the nvidia-modeset.ko kernel module, this component is named
"nv-modeset-kernel.o_binary".  Neither nvidia-drm.ko nor nvidia-uvm.ko
have OS-agnostic components.

The kernel interface layer component for each kernel module must be built
for the target kernel.


## Directory Structure Layout

- `kernel-open/`                The kernel interface layer
- `kernel-open/nvidia/`         The kernel interface layer for nvidia.ko
- `kernel-open/nvidia-drm/`     The kernel interface layer for nvidia-drm.ko
- `kernel-open/nvidia-modeset/` The kernel interface layer for nvidia-modeset.ko
- `kernel-open/nvidia-uvm/`     The kernel interface layer for nvidia-uvm.ko

- `src/`                        The OS-agnostic code
- `src/nvidia/`                 The OS-agnostic code for nvidia.ko
- `src/nvidia-modeset/`         The OS-agnostic code for nvidia-modeset.ko
- `src/common/`                 Utility code used by one or more of nvidia.ko and nvidia-modeset.ko
- `nouveau/`                    Tools for integration with the Nouveau device driver


## Nouveau device driver integration

The Python script in the 'nouveau' directory is used to extract some of the
firmware binary images (and related data) encoded in the source code and
store them as distinct files.  These files are used by the Nouveau device
driver to load and communicate with the GSP firmware.

The layout of the binary files is described in nouveau_firmware_layout.ods,
which is an OpenDocument Spreadsheet file, compatible with most spreadsheet
software applications.


## Compatible GPUs

The NVIDIA open kernel modules can be used on any Turing or later GPU (see the
table below).

For details on feature support and limitations, see the NVIDIA GPU driver
end user README here:

https://us.download.nvidia.com/XFree86/Linux-x86_64/560.31.02/README/kernel_open.html

For vGPU support, please refer to the README.vgpu packaged in the vGPU Host
Package for more details.

In the below table, if three IDs are listed, the first is the PCI Device 
ID, the second is the PCI Subsystem Vendor ID, and the third is the PCI
Subsystem Device ID.

| Product Name                                    | PCI ID         |
| ----------------------------------------------- | -------------- |
| NVIDIA TITAN RTX                                | 1E02           |
| NVIDIA GeForce RTX 2080 Ti                      | 1E04           |
| NVIDIA GeForce RTX 2080 Ti                      | 1E07           |
| Quadro RTX 6000                                 | 1E30           |
| Quadro RTX 8000                                 | 1E30 1028 129E |
| Quadro RTX 8000                                 | 1E30 103C 129E |
| Quadro RTX 8000                                 | 1E30 10DE 129E |
| Quadro RTX 6000                                 | 1E36           |
| Quadro RTX 8000                                 | 1E78 10DE 13D8 |
| Quadro RTX 6000                                 | 1E78 10DE 13D9 |
| NVIDIA GeForce RTX 2080 SUPER                   | 1E81           |
| NVIDIA GeForce RTX 2080                         | 1E82           |
| NVIDIA GeForce RTX 2070 SUPER                   | 1E84           |
| NVIDIA GeForce RTX 2080                         | 1E87           |
| NVIDIA GeForce RTX 2060                         | 1E89           |
| NVIDIA GeForce RTX 2080                         | 1E90           |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 1025 1375 |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 1028 08A1 |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 1028 08A2 |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 1028 08EA |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 1028 08EB |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 1028 08EC |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 1028 08ED |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 1028 08EE |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 1028 08EF |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 1028 093B |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 1028 093C |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 103C 8572 |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 103C 8573 |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 103C 8602 |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 103C 8606 |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 103C 86C6 |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 103C 86C7 |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 103C 87A6 |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 103C 87A7 |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 1043 131F |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 1043 137F |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 1043 141F |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 1043 1751 |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 1458 1660 |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 1458 1661 |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 1458 1662 |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 1458 75A6 |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 1458 75A7 |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 1458 86A6 |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 1458 86A7 |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 1462 1274 |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 1462 1277 |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 152D 1220 |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 1558 95E1 |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 1558 97E1 |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 1A58 2002 |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 1A58 2005 |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 1A58 2007 |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 1A58 3000 |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 1A58 3001 |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1E90 1D05 1069 |
| NVIDIA GeForce RTX 2070 Super                   | 1E91           |
| NVIDIA GeForce RTX 2070 Super with Max-Q Design | 1E91 103C 8607 |
| NVIDIA GeForce RTX 2070 Super with Max-Q Design | 1E91 103C 8736 |
| NVIDIA GeForce RTX 2070 Super with Max-Q Design | 1E91 103C 8738 |
| NVIDIA GeForce RTX 2070 Super with Max-Q Design | 1E91 103C 8772 |
| NVIDIA GeForce RTX 2070 Super with Max-Q Design | 1E91 103C 878A |
| NVIDIA GeForce RTX 2070 Super with Max-Q Design | 1E91 103C 878B |
| NVIDIA GeForce RTX 2070 Super with Max-Q Design | 1E91 1043 1E61 |
| NVIDIA GeForce RTX 2070 Super with Max-Q Design | 1E91 1458 1511 |
| NVIDIA GeForce RTX 2070 Super with Max-Q Design | 1E91 1458 75B3 |
| NVIDIA GeForce RTX 2070 Super with Max-Q Design | 1E91 1458 75B4 |
| NVIDIA GeForce RTX 2070 Super with Max-Q Design | 1E91 1458 76B2 |
| NVIDIA GeForce RTX 2070 Super with Max-Q Design | 1E91 1458 76B3 |
| NVIDIA GeForce RTX 2070 Super with Max-Q Design | 1E91 1458 78A2 |
| NVIDIA GeForce RTX 2070 Super with Max-Q Design | 1E91 1458 78A3 |
| NVIDIA GeForce RTX 2070 Super with Max-Q Design | 1E91 1458 86B2 |
| NVIDIA GeForce RTX 2070 Super with Max-Q Design | 1E91 1458 86B3 |
| NVIDIA GeForce RTX 2070 Super with Max-Q Design | 1E91 1462 12AE |
| NVIDIA GeForce RTX 2070 Super with Max-Q Design | 1E91 1462 12B0 |
| NVIDIA GeForce RTX 2070 Super with Max-Q Design | 1E91 1462 12C6 |
| NVIDIA GeForce RTX 2070 Super with Max-Q Design | 1E91 17AA 22C3 |
| NVIDIA GeForce RTX 2070 Super with Max-Q Design | 1E91 17AA 22C5 |
| NVIDIA GeForce RTX 2070 Super with Max-Q Design | 1E91 1A58 2009 |
| NVIDIA GeForce RTX 2070 Super with Max-Q Design | 1E91 1A58 200A |
| NVIDIA GeForce RTX 2070 Super with Max-Q Design | 1E91 1A58 3002 |
| NVIDIA GeForce RTX 2070 Super with Max-Q Design | 1E91 8086 3012 |
| NVIDIA GeForce RTX 2080 Super                   | 1E93           |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1E93 1025 1401 |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1E93 1025 149C |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1E93 1028 09D2 |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1E93 103C 8607 |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1E93 103C 86C7 |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1E93 103C 8736 |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1E93 103C 8738 |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1E93 103C 8772 |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1E93 103C 87A6 |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1E93 103C 87A7 |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1E93 1458 75B1 |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1E93 1458 75B2 |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1E93 1458 76B0 |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1E93 1458 76B1 |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1E93 1458 78A0 |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1E93 1458 78A1 |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1E93 1458 86B0 |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1E93 1458 86B1 |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1E93 1462 12AE |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1E93 1462 12B0 |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1E93 1462 12B4 |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1E93 1462 12C6 |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1E93 1558 50D3 |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1E93 1558 70D1 |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1E93 17AA 22C3 |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1E93 17AA 22C5 |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1E93 1A58 2009 |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1E93 1A58 200A |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1E93 1A58 3002 |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1E93 1D05 1089 |
| Quadro RTX 5000                                 | 1EB0           |
| Quadro RTX 4000                                 | 1EB1           |
| Quadro RTX 5000                                 | 1EB5           |
| Quadro RTX 5000 with Max-Q Design               | 1EB5 1025 1375 |
| Quadro RTX 5000 with Max-Q Design               | 1EB5 1025 1401 |
| Quadro RTX 5000 with Max-Q Design               | 1EB5 1025 149C |
| Quadro RTX 5000 with Max-Q Design               | 1EB5 1028 09C3 |
| Quadro RTX 5000 with Max-Q Design               | 1EB5 103C 8736 |
| Quadro RTX 5000 with Max-Q Design               | 1EB5 103C 8738 |
| Quadro RTX 5000 with Max-Q Design               | 1EB5 103C 8772 |
| Quadro RTX 5000 with Max-Q Design               | 1EB5 103C 8780 |
| Quadro RTX 5000 with Max-Q Design               | 1EB5 103C 8782 |
| Quadro RTX 5000 with Max-Q Design               | 1EB5 103C 8783 |
| Quadro RTX 5000 with Max-Q Design               | 1EB5 103C 8785 |
| Quadro RTX 5000 with Max-Q Design               | 1EB5 1043 1DD1 |
| Quadro RTX 5000 with Max-Q Design               | 1EB5 1462 1274 |
| Quadro RTX 5000 with Max-Q Design               | 1EB5 1462 12B0 |
| Quadro RTX 5000 with Max-Q Design               | 1EB5 1462 12C6 |
| Quadro RTX 5000 with Max-Q Design               | 1EB5 17AA 22B8 |
| Quadro RTX 5000 with Max-Q Design               | 1EB5 17AA 22BA |
| Quadro RTX 5000 with Max-Q Design               | 1EB5 1A58 2005 |
| Quadro RTX 5000 with Max-Q Design               | 1EB5 1A58 2007 |
| Quadro RTX 5000 with Max-Q Design               | 1EB5 1A58 2008 |
| Quadro RTX 5000 with Max-Q Design               | 1EB5 1A58 200A |
| Quadro RTX 4000                                 | 1EB6           |
| Quadro RTX 4000 with Max-Q Design               | 1EB6 1028 09C3 |
| Quadro RTX 4000 with Max-Q Design               | 1EB6 103C 8736 |
| Quadro RTX 4000 with Max-Q Design               | 1EB6 103C 8738 |
| Quadro RTX 4000 with Max-Q Design               | 1EB6 103C 8772 |
| Quadro RTX 4000 with Max-Q Design               | 1EB6 103C 8780 |
| Quadro RTX 4000 with Max-Q Design               | 1EB6 103C 8782 |
| Quadro RTX 4000 with Max-Q Design               | 1EB6 103C 8783 |
| Quadro RTX 4000 with Max-Q Design               | 1EB6 103C 8785 |
| Quadro RTX 4000 with Max-Q Design               | 1EB6 1462 1274 |
| Quadro RTX 4000 with Max-Q Design               | 1EB6 1462 1277 |
| Quadro RTX 4000 with Max-Q Design               | 1EB6 1462 12B0 |
| Quadro RTX 4000 with Max-Q Design               | 1EB6 1462 12C6 |
| Quadro RTX 4000 with Max-Q Design               | 1EB6 17AA 22B8 |
| Quadro RTX 4000 with Max-Q Design               | 1EB6 17AA 22BA |
| NVIDIA GeForce RTX 2070 SUPER                   | 1EC2           |
| NVIDIA GeForce RTX 2070 SUPER                   | 1EC7           |
| NVIDIA GeForce RTX 2080                         | 1ED0           |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1ED0 1025 132D |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1ED0 1028 08ED |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1ED0 1028 08EE |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1ED0 1028 08EF |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1ED0 103C 8572 |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1ED0 103C 8573 |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1ED0 103C 8600 |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1ED0 103C 8605 |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1ED0 1043 138F |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1ED0 1043 15C1 |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1ED0 17AA 3FEE |
| NVIDIA GeForce RTX 2080 with Max-Q Design       | 1ED0 17AA 3FFE |
| NVIDIA GeForce RTX 2070 Super                   | 1ED1           |
| NVIDIA GeForce RTX 2070 Super with Max-Q Design | 1ED1 1025 1432 |
| NVIDIA GeForce RTX 2070 Super with Max-Q Design | 1ED1 103C 8746 |
| NVIDIA GeForce RTX 2070 Super with Max-Q Design | 1ED1 103C 878A |
| NVIDIA GeForce RTX 2070 Super with Max-Q Design | 1ED1 1043 165F |
| NVIDIA GeForce RTX 2070 Super with Max-Q Design | 1ED1 144D C192 |
| NVIDIA GeForce RTX 2070 Super with Max-Q Design | 1ED1 17AA 3FCE |
| NVIDIA GeForce RTX 2070 Super with Max-Q Design | 1ED1 17AA 3FCF |
| NVIDIA GeForce RTX 2070 Super with Max-Q Design | 1ED1 17AA 3FD0 |
| NVIDIA GeForce RTX 2080 Super                   | 1ED3           |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1ED3 1025 1432 |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1ED3 1028 09D1 |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1ED3 103C 8746 |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1ED3 103C 878A |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1ED3 1043 1D61 |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1ED3 1043 1E51 |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1ED3 1043 1F01 |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1ED3 17AA 3FCE |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1ED3 17AA 3FCF |
| NVIDIA GeForce RTX 2080 Super with Max-Q Design | 1ED3 17AA 3FD0 |
| Quadro RTX 5000                                 | 1EF5           |
| NVIDIA GeForce RTX 2070                         | 1F02           |
| NVIDIA GeForce RTX 2060                         | 1F03           |
| NVIDIA GeForce RTX 2060 SUPER                   | 1F06           |
| NVIDIA GeForce RTX 2070                         | 1F07           |
| NVIDIA GeForce RTX 2060                         | 1F08           |
| NVIDIA GeForce GTX 1650                         | 1F0A           |
| NVIDIA GeForce RTX 2070                         | 1F10           |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 1025 132D |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 1025 1342 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 1028 08A1 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 1028 08A2 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 1028 08EA |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 1028 08EB |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 1028 08EC |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 1028 08ED |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 1028 08EE |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 1028 08EF |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 1028 093B |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 1028 093C |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 103C 8572 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 103C 8573 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 103C 8602 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 103C 8606 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 1043 132F |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 1043 136F |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 1043 1881 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 1043 1E6E |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 1458 1658 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 1458 1663 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 1458 1664 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 1458 75A4 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 1458 75A5 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 1458 86A4 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 1458 86A5 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 1462 1274 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 1462 1277 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 1558 95E1 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 1558 97E1 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 1A58 2002 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 1A58 2005 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 1A58 2007 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 1A58 3000 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 1A58 3001 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 1D05 105E |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 1D05 1070 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 1D05 2087 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F10 8086 2087 |
| NVIDIA GeForce RTX 2060                         | 1F11           |
| NVIDIA GeForce RTX 2060                         | 1F12           |
| NVIDIA GeForce RTX 2060 with Max-Q Design       | 1F12 1028 098F |
| NVIDIA GeForce RTX 2060 with Max-Q Design       | 1F12 103C 8741 |
| NVIDIA GeForce RTX 2060 with Max-Q Design       | 1F12 103C 8744 |
| NVIDIA GeForce RTX 2060 with Max-Q Design       | 1F12 103C 878E |
| NVIDIA GeForce RTX 2060 with Max-Q Design       | 1F12 103C 880E |
| NVIDIA GeForce RTX 2060 with Max-Q Design       | 1F12 1043 1E11 |
| NVIDIA GeForce RTX 2060 with Max-Q Design       | 1F12 1043 1F11 |
| NVIDIA GeForce RTX 2060 with Max-Q Design       | 1F12 1462 12D9 |
| NVIDIA GeForce RTX 2060 with Max-Q Design       | 1F12 17AA 3801 |
| NVIDIA GeForce RTX 2060 with Max-Q Design       | 1F12 17AA 3802 |
| NVIDIA GeForce RTX 2060 with Max-Q Design       | 1F12 17AA 3803 |
| NVIDIA GeForce RTX 2070                         | 1F14           |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F14 1025 1401 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F14 1025 1432 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F14 1025 1442 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F14 1025 1446 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F14 1025 147D |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F14 1028 09E2 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F14 1028 09F3 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F14 103C 8607 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F14 103C 86C6 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F14 103C 86C7 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F14 103C 8736 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F14 103C 8738 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F14 103C 8746 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F14 103C 8772 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F14 103C 878A |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F14 103C 878B |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F14 103C 87A6 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F14 103C 87A7 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F14 1043 174F |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F14 1458 1512 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F14 1458 75B5 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F14 1458 75B6 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F14 1458 76B4 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F14 1458 76B5 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F14 1458 78A4 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F14 1458 78A5 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F14 1458 86B4 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F14 1458 86B5 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F14 1462 12AE |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F14 1462 12B0 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F14 1462 12C6 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F14 1558 50D3 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F14 1558 70D1 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F14 1A58 200C |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F14 1A58 2011 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F14 1A58 3002 |
| NVIDIA GeForce RTX 2060                         | 1F15           |
| Quadro RTX 3000                                 | 1F36           |
| Quadro RTX 3000 with Max-Q Design               | 1F36 1028 0990 |
| Quadro RTX 3000 with Max-Q Design               | 1F36 103C 8736 |
| Quadro RTX 3000 with Max-Q Design               | 1F36 103C 8738 |
| Quadro RTX 3000 with Max-Q Design               | 1F36 103C 8772 |
| Quadro RTX 3000 with Max-Q Design               | 1F36 1043 13CF |
| Quadro RTX 3000 with Max-Q Design               | 1F36 1414 0032 |
| NVIDIA GeForce RTX 2060 SUPER                   | 1F42           |
| NVIDIA GeForce RTX 2060 SUPER                   | 1F47           |
| NVIDIA GeForce RTX 2070                         | 1F50           |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F50 1028 08ED |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F50 1028 08EE |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F50 1028 08EF |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F50 103C 8572 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F50 103C 8573 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F50 103C 8574 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F50 103C 8600 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F50 103C 8605 |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F50 17AA 3FEE |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F50 17AA 3FFE |
| NVIDIA GeForce RTX 2060                         | 1F51           |
| NVIDIA GeForce RTX 2070                         | 1F54           |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F54 103C 878A |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F54 17AA 3FCE |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F54 17AA 3FCF |
| NVIDIA GeForce RTX 2070 with Max-Q Design       | 1F54 17AA 3FD0 |
| NVIDIA GeForce RTX 2060                         | 1F55           |
| Quadro RTX 3000                                 | 1F76           |
| Matrox D-Series D2450                           | 1F76 102B 2800 |
| Matrox D-Series D2480                           | 1F76 102B 2900 |
| NVIDIA GeForce GTX 1650                         | 1F82           |
| NVIDIA GeForce GTX 1630                         | 1F83           |
| NVIDIA GeForce GTX 1650                         | 1F91           |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F91 103C 863E |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F91 103C 86E7 |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F91 103C 86E8 |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F91 1043 12CF |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F91 1043 156F |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F91 1414 0032 |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F91 144D C822 |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F91 1462 127E |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F91 1462 1281 |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F91 1462 1284 |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F91 1462 1285 |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F91 1462 129C |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F91 17AA 229F |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F91 17AA 3802 |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F91 17AA 3806 |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F91 17AA 3F1A |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F91 1A58 1001 |
| NVIDIA GeForce GTX 1650 Ti                      | 1F95           |
| NVIDIA GeForce GTX 1650 Ti with Max-Q Design    | 1F95 1025 1479 |
| NVIDIA GeForce GTX 1650 Ti with Max-Q Design    | 1F95 1025 147A |
| NVIDIA GeForce GTX 1650 Ti with Max-Q Design    | 1F95 1025 147B |
| NVIDIA GeForce GTX 1650 Ti with Max-Q Design    | 1F95 1025 147C |
| NVIDIA GeForce GTX 1650 Ti with Max-Q Design    | 1F95 103C 86E7 |
| NVIDIA GeForce GTX 1650 Ti with Max-Q Design    | 1F95 103C 86E8 |
| NVIDIA GeForce GTX 1650 Ti with Max-Q Design    | 1F95 103C 8815 |
| NVIDIA GeForce GTX 1650 Ti with Max-Q Design    | 1F95 1043 1DFF |
| NVIDIA GeForce GTX 1650 Ti with Max-Q Design    | 1F95 1043 1E1F |
| NVIDIA GeForce GTX 1650 Ti with Max-Q Design    | 1F95 144D C838 |
| NVIDIA GeForce GTX 1650 Ti with Max-Q Design    | 1F95 1462 12BD |
| NVIDIA GeForce GTX 1650 Ti with Max-Q Design    | 1F95 1462 12C5 |
| NVIDIA GeForce GTX 1650 Ti with Max-Q Design    | 1F95 1462 12D2 |
| NVIDIA GeForce GTX 1650 Ti with Max-Q Design    | 1F95 17AA 22C0 |
| NVIDIA GeForce GTX 1650 Ti with Max-Q Design    | 1F95 17AA 22C1 |
| NVIDIA GeForce GTX 1650 Ti with Max-Q Design    | 1F95 17AA 3837 |
| NVIDIA GeForce GTX 1650 Ti with Max-Q Design    | 1F95 17AA 3F95 |
| NVIDIA GeForce GTX 1650 Ti with Max-Q Design    | 1F95 1A58 1003 |
| NVIDIA GeForce GTX 1650 Ti with Max-Q Design    | 1F95 1A58 1006 |
| NVIDIA GeForce GTX 1650 Ti with Max-Q Design    | 1F95 1A58 1007 |
| NVIDIA GeForce GTX 1650 Ti with Max-Q Design    | 1F95 1E83 3E30 |
| NVIDIA GeForce GTX 1650                         | 1F96           |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F96 1462 1297 |
| NVIDIA GeForce MX450                            | 1F97           |
| NVIDIA GeForce MX450                            | 1F98           |
| NVIDIA GeForce GTX 1650                         | 1F99           |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F99 1025 1479 |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F99 1025 147A |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F99 1025 147B |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F99 1025 147C |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F99 103C 8815 |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F99 1043 13B2 |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F99 1043 1402 |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F99 1043 1902 |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F99 1462 12BD |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F99 1462 12C5 |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F99 1462 12D2 |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F99 17AA 22DA |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F99 17AA 3F93 |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F99 1E83 3E30 |
| NVIDIA GeForce MX450                            | 1F9C           |
| NVIDIA GeForce GTX 1650                         | 1F9D           |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F9D 1043 128D |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F9D 1043 130D |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F9D 1043 149C |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F9D 1043 185C |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F9D 1043 189C |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F9D 1462 12F4 |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F9D 1462 1302 |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F9D 1462 131B |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F9D 1462 1326 |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F9D 1462 132A |
| NVIDIA GeForce GTX 1650 with Max-Q Design       | 1F9D 1462 132E |
| NVIDIA GeForce MX550                            | 1F9F           |
| NVIDIA GeForce MX550                            | 1FA0           |
| NVIDIA T1000                                    | 1FB0 1028 12DB |
| NVIDIA T1000                                    | 1FB0 103C 12DB |
| NVIDIA T1000                                    | 1FB0 103C 8A80 |
| NVIDIA T1000                                    | 1FB0 10DE 12DB |
| NVIDIA DGX Display                              | 1FB0 10DE 1485 |
| NVIDIA T1000                                    | 1FB0 17AA 12DB |
| NVIDIA T600                                     | 1FB1 1028 1488 |
| NVIDIA T600                                     | 1FB1 103C 1488 |
| NVIDIA T600                                     | 1FB1 103C 8A80 |
| NVIDIA T600                                     | 1FB1 10DE 1488 |
| NVIDIA T600                                     | 1FB1 17AA 1488 |
| NVIDIA T400                                     | 1FB2 1028 1489 |
| NVIDIA T400                                     | 1FB2 103C 1489 |
| NVIDIA T400                                     | 1FB2 103C 8A80 |
| NVIDIA T400                                     | 1FB2 10DE 1489 |
| NVIDIA T400                                     | 1FB2 17AA 1489 |
| NVIDIA T600 Laptop GPU                          | 1FB6           |
| NVIDIA T550 Laptop GPU                          | 1FB7           |
| Quadro T2000                                    | 1FB8           |
| Quadro T2000 with Max-Q Design                  | 1FB8 1028 097E |
| Quadro T2000 with Max-Q Design                  | 1FB8 103C 8736 |
| Quadro T2000 with Max-Q Design                  | 1FB8 103C 8738 |
| Quadro T2000 with Max-Q Design                  | 1FB8 103C 8772 |
| Quadro T2000 with Max-Q Design                  | 1FB8 103C 8780 |
| Quadro T2000 with Max-Q Design                  | 1FB8 103C 8782 |
| Quadro T2000 with Max-Q Design                  | 1FB8 103C 8783 |
| Quadro T2000 with Max-Q Design                  | 1FB8 103C 8785 |
| Quadro T2000 with Max-Q Design                  | 1FB8 103C 87F0 |
| Quadro T2000 with Max-Q Design                  | 1FB8 1462 1281 |
| Quadro T2000 with Max-Q Design                  | 1FB8 1462 12BD |
| Quadro T2000 with Max-Q Design                  | 1FB8 17AA 22C0 |
| Quadro T2000 with Max-Q Design                  | 1FB8 17AA 22C1 |
| Quadro T1000                                    | 1FB9           |
| Quadro T1000 with Max-Q Design                  | 1FB9 1025 1479 |
| Quadro T1000 with Max-Q Design                  | 1FB9 1025 147A |
| Quadro T1000 with Max-Q Design                  | 1FB9 1025 147B |
| Quadro T1000 with Max-Q Design                  | 1FB9 1025 147C |
| Quadro T1000 with Max-Q Design                  | 1FB9 103C 8736 |
| Quadro T1000 with Max-Q Design                  | 1FB9 103C 8738 |
| Quadro T1000 with Max-Q Design                  | 1FB9 103C 8772 |
| Quadro T1000 with Max-Q Design                  | 1FB9 103C 8780 |
| Quadro T1000 with Max-Q Design                  | 1FB9 103C 8782 |
| Quadro T1000 with Max-Q Design                  | 1FB9 103C 8783 |
| Quadro T1000 with Max-Q Design                  | 1FB9 103C 8785 |
| Quadro T1000 with Max-Q Design                  | 1FB9 103C 87F0 |
| Quadro T1000 with Max-Q Design                  | 1FB9 1462 12BD |
| Quadro T1000 with Max-Q Design                  | 1FB9 17AA 22C0 |
| Quadro T1000 with Max-Q Design                  | 1FB9 17AA 22C1 |
| NVIDIA T600 Laptop GPU                          | 1FBA           |
| NVIDIA T500                                     | 1FBB           |
| NVIDIA T1200 Laptop GPU                         | 1FBC           |
| NVIDIA GeForce GTX 1650                         | 1FDD           |
| NVIDIA T1000 8GB                                | 1FF0 1028 1612 |
| NVIDIA T1000 8GB                                | 1FF0 103C 1612 |
| NVIDIA T1000 8GB                                | 1FF0 103C 8A80 |
| NVIDIA T1000 8GB                                | 1FF0 10DE 1612 |
| NVIDIA T1000 8GB                                | 1FF0 17AA 1612 |
| NVIDIA T400 4GB                                 | 1FF2 1028 1613 |
| NVIDIA T400 4GB                                 | 1FF2 103C 1613 |
| NVIDIA T400E                                    | 1FF2 103C 18FF |
| NVIDIA T400 4GB                                 | 1FF2 103C 8A80 |
| NVIDIA T400 4GB                                 | 1FF2 10DE 1613 |
| NVIDIA T400E                                    | 1FF2 10DE 18FF |
| NVIDIA T400 4GB                                 | 1FF2 17AA 1613 |
| NVIDIA T400E                                    | 1FF2 17AA 18FF |
| Quadro T1000                                    | 1FF9           |
| NVIDIA A100-SXM4-40GB                           | 20B0           |
| NVIDIA A100-PG509-200                           | 20B0 10DE 1450 |
| NVIDIA A100-SXM4-80GB                           | 20B2 10DE 1463 |
| NVIDIA A100-SXM4-80GB                           | 20B2 10DE 147F |
| NVIDIA A100-SXM4-80GB                           | 20B2 10DE 1622 |
| NVIDIA A100-SXM4-80GB                           | 20B2 10DE 1623 |
| NVIDIA PG509-210                                | 20B2 10DE 1625 |
| NVIDIA A100-SXM-64GB                            | 20B3 10DE 14A7 |
| NVIDIA A100-SXM-64GB                            | 20B3 10DE 14A8 |
| NVIDIA A100 80GB PCIe                           | 20B5 10DE 1533 |
| NVIDIA A100 80GB PCIe                           | 20B5 10DE 1642 |
| NVIDIA PG506-232                                | 20B6 10DE 1492 |
| NVIDIA A30                                      | 20B7 10DE 1532 |
| NVIDIA A30                                      | 20B7 10DE 1804 |
| NVIDIA A30                                      | 20B7 10DE 1852 |
| NVIDIA A800-SXM4-40GB                           | 20BD 10DE 17F4 |
| NVIDIA A100-PCIE-40GB                           | 20F1 10DE 145F |
| NVIDIA A800-SXM4-80GB                           | 20F3 10DE 179B |
| NVIDIA A800-SXM4-80GB                           | 20F3 10DE 179C |
| NVIDIA A800-SXM4-80GB                           | 20F3 10DE 179D |
| NVIDIA A800-SXM4-80GB                           | 20F3 10DE 179E |
| NVIDIA A800-SXM4-80GB                           | 20F3 10DE 179F |
| NVIDIA A800-SXM4-80GB                           | 20F3 10DE 17A0 |
| NVIDIA A800-SXM4-80GB                           | 20F3 10DE 17A1 |
| NVIDIA A800-SXM4-80GB                           | 20F3 10DE 17A2 |
| NVIDIA A800 80GB PCIe                           | 20F5 10DE 1799 |
| NVIDIA A800 80GB PCIe LC                        | 20F5 10DE 179A |
| NVIDIA A800 40GB Active                         | 20F6 1028 180A |
| NVIDIA A800 40GB Active                         | 20F6 103C 180A |
| NVIDIA A800 40GB Active                         | 20F6 10DE 180A |
| NVIDIA A800 40GB Active                         | 20F6 17AA 180A |
| NVIDIA AX800                                    | 20FD 10DE 17F8 |
| NVIDIA GeForce GTX 1660 Ti                      | 2182           |
| NVIDIA GeForce GTX 1660                         | 2184           |
| NVIDIA GeForce GTX 1650 SUPER                   | 2187           |
| NVIDIA GeForce GTX 1650                         | 2188           |
| NVIDIA GeForce GTX 1660 Ti                      | 2191           |
| NVIDIA GeForce GTX 1660 Ti with Max-Q Design    | 2191 1028 0949 |
| NVIDIA GeForce GTX 1660 Ti with Max-Q Design    | 2191 103C 85FB |
| NVIDIA GeForce GTX 1660 Ti with Max-Q Design    | 2191 103C 85FE |
| NVIDIA GeForce GTX 1660 Ti with Max-Q Design    | 2191 103C 86D6 |
| NVIDIA GeForce GTX 1660 Ti with Max-Q Design    | 2191 103C 8741 |
| NVIDIA GeForce GTX 1660 Ti with Max-Q Design    | 2191 103C 8744 |
| NVIDIA GeForce GTX 1660 Ti with Max-Q Design    | 2191 103C 878D |
| NVIDIA GeForce GTX 1660 Ti with Max-Q Design    | 2191 103C 87AF |
| NVIDIA GeForce GTX 1660 Ti with Max-Q Design    | 2191 103C 87B3 |
| NVIDIA GeForce GTX 1660 Ti with Max-Q Design    | 2191 1043 171F |
| NVIDIA GeForce GTX 1660 Ti with Max-Q Design    | 2191 1043 17EF |
| NVIDIA GeForce GTX 1660 Ti with Max-Q Design    | 2191 1043 18D1 |
| NVIDIA GeForce GTX 1660 Ti with Max-Q Design    | 2191 1414 0032 |
| NVIDIA GeForce GTX 1660 Ti with Max-Q Design    | 2191 1462 128A |
| NVIDIA GeForce GTX 1660 Ti with Max-Q Design    | 2191 1462 128B |
| NVIDIA GeForce GTX 1660 Ti with Max-Q Design    | 2191 1462 12C6 |
| NVIDIA GeForce GTX 1660 Ti with Max-Q Design    | 2191 1462 12CB |
| NVIDIA GeForce GTX 1660 Ti with Max-Q Design    | 2191 1462 12CC |
| NVIDIA GeForce GTX 1660 Ti with Max-Q Design    | 2191 1462 12D9 |
| NVIDIA GeForce GTX 1660 Ti with Max-Q Design    | 2191 17AA 380C |
| NVIDIA GeForce GTX 1660 Ti with Max-Q Design    | 2191 17AA 381D |
| NVIDIA GeForce GTX 1660 Ti with Max-Q Design    | 2191 17AA 381E |
| NVIDIA GeForce GTX 1650 Ti                      | 2192           |
| NVIDIA GeForce GTX 1660 SUPER                   | 21C4           |
| NVIDIA GeForce GTX 1660 Ti                      | 21D1           |
| NVIDIA GeForce RTX 3090 Ti                      | 2203           |
| NVIDIA GeForce RTX 3090                         | 2204           |
| NVIDIA GeForce RTX 3080                         | 2206           |
| NVIDIA GeForce RTX 3070 Ti                      | 2207           |
| NVIDIA GeForce RTX 3080 Ti                      | 2208           |
| NVIDIA GeForce RTX 3080                         | 220A           |
| NVIDIA CMP 90HX                                 | 220D           |
| NVIDIA GeForce RTX 3080                         | 2216           |
| NVIDIA RTX A6000                                | 2230 1028 1459 |
| NVIDIA RTX A6000                                | 2230 103C 1459 |
| NVIDIA RTX A6000                                | 2230 10DE 1459 |
| NVIDIA RTX A6000                                | 2230 17AA 1459 |
| NVIDIA RTX A5000                                | 2231 1028 147E |
| NVIDIA RTX A5000                                | 2231 103C 147E |
| NVIDIA RTX A5000                                | 2231 10DE 147E |
| NVIDIA RTX A5000                                | 2231 17AA 147E |
| NVIDIA RTX A4500                                | 2232 1028 163C |
| NVIDIA RTX A4500                                | 2232 103C 163C |
| NVIDIA RTX A4500                                | 2232 10DE 163C |
| NVIDIA RTX A4500                                | 2232 17AA 163C |
| NVIDIA RTX A5500                                | 2233 1028 165A |
| NVIDIA RTX A5500                                | 2233 103C 165A |
| NVIDIA RTX A5500                                | 2233 10DE 165A |
| NVIDIA RTX A5500                                | 2233 17AA 165A |
| NVIDIA A40                                      | 2235 10DE 145A |
| NVIDIA A10                                      | 2236 10DE 1482 |
| NVIDIA A10G                                     | 2237 10DE 152F |
| NVIDIA A10M                                     | 2238 10DE 1677 |
| NVIDIA H100 NVL                                 | 2321 10DE 1839 |
| NVIDIA H800 PCIe                                | 2322 10DE 17A4 |
| NVIDIA H800                                     | 2324 10DE 17A6 |
| NVIDIA H800                                     | 2324 10DE 17A8 |
| NVIDIA H20                                      | 2329 10DE 198B |
| NVIDIA H20                                      | 2329 10DE 198C |
| NVIDIA H100 80GB HBM3                           | 2330 10DE 16C0 |
| NVIDIA H100 80GB HBM3                           | 2330 10DE 16C1 |
| NVIDIA H100 PCIe                                | 2331 10DE 1626 |
| NVIDIA H200                                     | 2335 10DE 18BE |
| NVIDIA H200                                     | 2335 10DE 18BF |
| NVIDIA H100                                     | 2339 10DE 17FC |
| NVIDIA H800 NVL                                 | 233A 10DE 183A |
| NVIDIA GH200 120GB                              | 2342 10DE 16EB |
| NVIDIA GH200 120GB                              | 2342 10DE 1805 |
| NVIDIA GH200 480GB                              | 2342 10DE 1809 |
| NVIDIA GeForce RTX 3060 Ti                      | 2414           |
| NVIDIA GeForce RTX 3080 Ti Laptop GPU           | 2420           |
| NVIDIA RTX A5500 Laptop GPU                     | 2438           |
| NVIDIA GeForce RTX 3080 Ti Laptop GPU           | 2460           |
| NVIDIA GeForce RTX 3070 Ti                      | 2482           |
| NVIDIA GeForce RTX 3070                         | 2484           |
| NVIDIA GeForce RTX 3060 Ti                      | 2486           |
| NVIDIA GeForce RTX 3060                         | 2487           |
| NVIDIA GeForce RTX 3070                         | 2488           |
| NVIDIA GeForce RTX 3060 Ti                      | 2489           |
| NVIDIA CMP 70HX                                 | 248A           |
| NVIDIA GeForce RTX 3080 Laptop GPU              | 249C           |
| NVIDIA GeForce RTX 3060 Laptop GPU              | 249C 1D05 1194 |
| NVIDIA GeForce RTX 3070 Laptop GPU              | 249D           |
| NVIDIA GeForce RTX 3070 Ti Laptop GPU           | 24A0           |
| NVIDIA GeForce RTX 3060 Laptop GPU              | 24A0 1D05 1192 |
| NVIDIA RTX A4000                                | 24B0 1028 14AD |
| NVIDIA RTX A4000                                | 24B0 103C 14AD |
| NVIDIA RTX A4000                                | 24B0 10DE 14AD |
| NVIDIA RTX A4000                                | 24B0 17AA 14AD |
| NVIDIA RTX A4000H                               | 24B1 10DE 1658 |
| NVIDIA RTX A5000 Laptop GPU                     | 24B6           |
| NVIDIA RTX A4000 Laptop GPU                     | 24B7           |
| NVIDIA RTX A3000 Laptop GPU                     | 24B8           |
| NVIDIA RTX A3000 12GB Laptop GPU                | 24B9           |
| NVIDIA RTX A4500 Laptop GPU                     | 24BA           |
| NVIDIA RTX A3000 12GB Laptop GPU                | 24BB           |
| NVIDIA GeForce RTX 3060                         | 24C7           |
| NVIDIA GeForce RTX 3060 Ti                      | 24C9           |
| NVIDIA GeForce RTX 3080 Laptop GPU              | 24DC           |
| NVIDIA GeForce RTX 3070 Laptop GPU              | 24DD           |
| NVIDIA GeForce RTX 3070 Ti Laptop GPU           | 24E0           |
| NVIDIA RTX A4500 Embedded GPU                   | 24FA           |
| NVIDIA GeForce RTX 3060                         | 2503           |
| NVIDIA GeForce RTX 3060                         | 2504           |
| NVIDIA GeForce RTX 3050                         | 2507           |
| NVIDIA GeForce RTX 3050 OEM                     | 2508           |
| NVIDIA GeForce RTX 3060 Laptop GPU              | 2520           |
| NVIDIA GeForce RTX 3060 Laptop GPU              | 2521           |
| NVIDIA GeForce RTX 3050 Ti Laptop GPU           | 2523           |
| NVIDIA RTX A2000                                | 2531 1028 151D |
| NVIDIA RTX A2000                                | 2531 103C 151D |
| NVIDIA RTX A2000                                | 2531 10DE 151D |
| NVIDIA RTX A2000                                | 2531 17AA 151D |
| NVIDIA GeForce RTX 3060                         | 2544           |
| NVIDIA GeForce RTX 3060 Laptop GPU              | 2560           |
| NVIDIA GeForce RTX 3050 Ti Laptop GPU           | 2563           |
| NVIDIA RTX A2000 12GB                           | 2571 1028 1611 |
| NVIDIA RTX A2000 12GB                           | 2571 103C 1611 |
| NVIDIA RTX A2000 12GB                           | 2571 10DE 1611 |
| NVIDIA RTX A2000 12GB                           | 2571 17AA 1611 |
| NVIDIA GeForce RTX 3050                         | 2582           |
| NVIDIA GeForce RTX 3050                         | 2584           |
| NVIDIA GeForce RTX 3050 Ti Laptop GPU           | 25A0           |
| NVIDIA GeForce RTX 3050Ti Laptop GPU            | 25A0 103C 8928 |
| NVIDIA GeForce RTX 3050Ti Laptop GPU            | 25A0 103C 89F9 |
| NVIDIA GeForce RTX 3060 Laptop GPU              | 25A0 1D05 1196 |
| NVIDIA GeForce RTX 3050 Laptop GPU              | 25A2           |
| NVIDIA GeForce RTX 3050 Ti Laptop GPU           | 25A2 1028 0BAF |
| NVIDIA GeForce RTX 3060 Laptop GPU              | 25A2 1D05 1195 |
| NVIDIA GeForce RTX 3050 Laptop GPU              | 25A5           |
| NVIDIA GeForce MX570                            | 25A6           |
| NVIDIA GeForce RTX 2050                         | 25A7           |
| NVIDIA GeForce RTX 2050                         | 25A9           |
| NVIDIA GeForce MX570 A                          | 25AA           |
| NVIDIA GeForce RTX 3050 4GB Laptop GPU          | 25AB           |
| NVIDIA GeForce RTX 3050 6GB Laptop GPU          | 25AC           |
| NVIDIA GeForce RTX 2050                         | 25AD           |
| NVIDIA RTX A1000                                | 25B0 1028 1878 |
| NVIDIA RTX A1000                                | 25B0 103C 1878 |
| NVIDIA RTX A1000                                | 25B0 103C 8D96 |
| NVIDIA RTX A1000                                | 25B0 10DE 1878 |
| NVIDIA RTX A1000                                | 25B0 17AA 1878 |
| NVIDIA RTX A400                                 | 25B2 1028 1879 |
| NVIDIA RTX A400                                 | 25B2 103C 1879 |
| NVIDIA RTX A400                                 | 25B2 103C 8D95 |
| NVIDIA RTX A400                                 | 25B2 10DE 1879 |
| NVIDIA RTX A400                                 | 25B2 17AA 1879 |
| NVIDIA A16                                      | 25B6 10DE 14A9 |
| NVIDIA A2                                       | 25B6 10DE 157E |
| NVIDIA RTX A2000 Laptop GPU                     | 25B8           |
| NVIDIA RTX A1000 Laptop GPU                     | 25B9           |
| NVIDIA RTX A2000 8GB Laptop GPU                 | 25BA           |
| NVIDIA RTX A500 Laptop GPU                      | 25BB           |
| NVIDIA RTX A1000 6GB Laptop GPU                 | 25BC           |
| NVIDIA RTX A500 Laptop GPU                      | 25BD           |
| NVIDIA GeForce RTX 3050 Ti Laptop GPU           | 25E0           |
| NVIDIA GeForce RTX 3050 Laptop GPU              | 25E2           |
| NVIDIA GeForce RTX 3050 Laptop GPU              | 25E5           |
| NVIDIA GeForce RTX 3050 6GB Laptop GPU          | 25EC           |
| NVIDIA GeForce RTX 2050                         | 25ED           |
| NVIDIA RTX A1000 Embedded GPU                   | 25F9           |
| NVIDIA RTX A2000 Embedded GPU                   | 25FA           |
| NVIDIA RTX A500 Embedded GPU                    | 25FB           |
| NVIDIA GeForce RTX 4090                         | 2684           |
| NVIDIA GeForce RTX 4090 D                       | 2685           |
| NVIDIA GeForce RTX 4070 Ti SUPER                | 2689           |
| NVIDIA RTX 6000 Ada Generation                  | 26B1 1028 16A1 |
| NVIDIA RTX 6000 Ada Generation                  | 26B1 103C 16A1 |
| NVIDIA RTX 6000 Ada Generation                  | 26B1 10DE 16A1 |
| NVIDIA RTX 6000 Ada Generation                  | 26B1 17AA 16A1 |
| NVIDIA RTX 5000 Ada Generation                  | 26B2 1028 17FA |
| NVIDIA RTX 5000 Ada Generation                  | 26B2 103C 17FA |
| NVIDIA RTX 5000 Ada Generation                  | 26B2 10DE 17FA |
| NVIDIA RTX 5000 Ada Generation                  | 26B2 17AA 17FA |
| NVIDIA RTX 5880 Ada Generation                  | 26B3 1028 1934 |
| NVIDIA RTX 5880 Ada Generation                  | 26B3 103C 1934 |
| NVIDIA RTX 5880 Ada Generation                  | 26B3 10DE 1934 |
| NVIDIA RTX 5880 Ada Generation                  | 26B3 17AA 1934 |
| NVIDIA L40                                      | 26B5 10DE 169D |
| NVIDIA L40                                      | 26B5 10DE 17DA |
| NVIDIA L40S                                     | 26B9 10DE 1851 |
| NVIDIA L40S                                     | 26B9 10DE 18CF |
| NVIDIA L20                                      | 26BA 10DE 1957 |
| NVIDIA L20                                      | 26BA 10DE 1990 |
| NVIDIA GeForce RTX 4080 SUPER                   | 2702           |
| NVIDIA GeForce RTX 4080                         | 2704           |
| NVIDIA GeForce RTX 4070 Ti SUPER                | 2705           |
| NVIDIA GeForce RTX 4070                         | 2709           |
| NVIDIA GeForce RTX 4090 Laptop GPU              | 2717           |
| NVIDIA RTX 5000 Ada Generation Laptop GPU       | 2730           |
| NVIDIA GeForce RTX 4090 Laptop GPU              | 2757           |
| NVIDIA RTX 5000 Ada Generation Embedded GPU     | 2770           |
| NVIDIA GeForce RTX 4070 Ti                      | 2782           |
| NVIDIA GeForce RTX 4070 SUPER                   | 2783           |
| NVIDIA GeForce RTX 4070                         | 2786           |
| NVIDIA GeForce RTX 4060 Ti                      | 2788           |
| NVIDIA GeForce RTX 4080 Laptop GPU              | 27A0           |
| NVIDIA RTX 4000 SFF Ada Generation              | 27B0 1028 16FA |
| NVIDIA RTX 4000 SFF Ada Generation              | 27B0 103C 16FA |
| NVIDIA RTX 4000 SFF Ada Generation              | 27B0 10DE 16FA |
| NVIDIA RTX 4000 SFF Ada Generation              | 27B0 17AA 16FA |
| NVIDIA RTX 4500 Ada Generation                  | 27B1 1028 180C |
| NVIDIA RTX 4500 Ada Generation                  | 27B1 103C 180C |
| NVIDIA RTX 4500 Ada Generation                  | 27B1 10DE 180C |
| NVIDIA RTX 4500 Ada Generation                  | 27B1 17AA 180C |
| NVIDIA RTX 4000 Ada Generation                  | 27B2 1028 181B |
| NVIDIA RTX 4000 Ada Generation                  | 27B2 103C 181B |
| NVIDIA RTX 4000 Ada Generation                  | 27B2 10DE 181B |
| NVIDIA RTX 4000 Ada Generation                  | 27B2 17AA 181B |
| NVIDIA L2                                       | 27B6 10DE 1933 |
| NVIDIA L4                                       | 27B8 10DE 16CA |
| NVIDIA L4                                       | 27B8 10DE 16EE |
| NVIDIA RTX 4000 Ada Generation Laptop GPU       | 27BA           |
| NVIDIA RTX 3500 Ada Generation Laptop GPU       | 27BB           |
| NVIDIA GeForce RTX 4080 Laptop GPU              | 27E0           |
| NVIDIA RTX 3500 Ada Generation Embedded GPU     | 27FB           |
| NVIDIA GeForce RTX 4060 Ti                      | 2803           |
| NVIDIA GeForce RTX 4060 Ti                      | 2805           |
| NVIDIA GeForce RTX 4060                         | 2808           |
| NVIDIA GeForce RTX 4070 Laptop GPU              | 2820           |
| NVIDIA GeForce RTX 3050 A Laptop GPU            | 2822           |
| NVIDIA RTX 3000 Ada Generation Laptop GPU       | 2838           |
| NVIDIA GeForce RTX 4070 Laptop GPU              | 2860           |
| NVIDIA GeForce RTX 4060                         | 2882           |
| NVIDIA GeForce RTX 4060 Laptop GPU              | 28A0           |
| NVIDIA GeForce RTX 4050 Laptop GPU              | 28A1           |
| NVIDIA RTX 2000 Ada Generation                  | 28B0 1028 1870 |
| NVIDIA RTX 2000 Ada Generation                  | 28B0 103C 1870 |
| NVIDIA RTX 2000E Ada Generation                 | 28B0 103C 1871 |
| NVIDIA RTX 2000 Ada Generation                  | 28B0 10DE 1870 |
| NVIDIA RTX 2000E Ada Generation                 | 28B0 10DE 1871 |
| NVIDIA RTX 2000 Ada Generation                  | 28B0 17AA 1870 |
| NVIDIA RTX 2000E Ada Generation                 | 28B0 17AA 1871 |
| NVIDIA RTX 2000 Ada Generation Laptop GPU       | 28B8           |
| NVIDIA RTX 1000 Ada Generation Laptop GPU       | 28B9           |
| NVIDIA RTX 500 Ada Generation Laptop GPU        | 28BA           |
| NVIDIA RTX 500 Ada Generation Laptop GPU        | 28BB           |
| NVIDIA GeForce RTX 4060 Laptop GPU              | 28E0           |
| NVIDIA GeForce RTX 4050 Laptop GPU              | 28E1           |
| NVIDIA RTX 2000 Ada Generation Embedded GPU     | 28F8           |
