# NVIDIA Linux Open GPU Kernel Module Source

This is the source release of the NVIDIA Linux open GPU kernel modules,
version 515.43.04.


## How to Build

To build:

    make modules -j`nproc`

To install, first uninstall any existing NVIDIA kernel modules.  Then,
as root:

    make modules_install -j`nproc`

Note that the kernel modules built here must be used with gsp.bin
firmware and user-space NVIDIA GPU driver components from a corresponding
515.43.04 driver release.  This can be achieved by installing
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
    make modules -j`nproc`          \
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

    make modules -j`nproc` NV_VERBOSE=1


## Supported Toolchains

Any reasonably modern version of GCC or Clang can be used to build the
kernel modules.  Note that the kernel interface layers of the kernel
modules must be built with the toolchain that was used to build the
kernel.


## Supported Linux Kernel Versions

The NVIDIA open kernel modules support the same range of Linux kernel
versions that are supported with the proprietary NVIDIA kernel modules.
This is currently Linux kernel 3.10 or newer.


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
