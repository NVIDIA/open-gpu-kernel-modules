# NVIDIA Open GPU Kernel Modules — P2P Blackwell (DKMS)

Fork of [NVIDIA/open-gpu-kernel-modules](https://github.com/NVIDIA/open-gpu-kernel-modules),
driver version **615.71.09**, packaged as a **DKMS module** (`nvidia-open-p2p/615.71.09`)
to enable peer-to-peer over PCI Express (e.g. RTX 5000 Blackwell).

No more manual `make modules_install`: clone, copy to `/usr/src`, `dkms add`,
`dkms build`, `dkms install`. Rebuilds follow kernel updates automatically
(`AUTOINSTALL="yes"`).

Feature support and limitations:
https://us.download.nvidia.com/XFree86/Linux-x86_64/615.71.09/README/kernel_open.html

## Repository layout

```
dkms.conf        DKMS descriptor used for manual install (root = DKMS source root)
version.mk       Driver version (PACKAGE_VERSION in dkms.conf must match)
Makefile         Top-level build: src/nvidia + src/nvidia-modeset, then kernel-open/
kernel-open/     Kernel interface layer (Kbuild) + nvidia-installer template dkms.conf
src/             OS-agnostic code (nv-kernel.o, nv-modeset-kernel.o)
```

> `kernel-open/dkms.conf` is the `nvidia-installer` template (placeholders
> `__VERSION_STRING`, `__JOBS`, …). It is kept for reference only.
> The file DKMS actually reads is the root `dkms.conf`.

## Prerequisites

- Kernel headers for the running kernel, e.g. on CachyOS:
  `sudo pacman -S linux-cachyos-headers` (check `[ -d /lib/modules/$(uname -r)/build ]`)
- `dkms`, `gcc`, `make`
- No kernel sources to ship: DKMS builds against the user's installed headers.

## Install (manual DKMS)

```sh
NAME=nvidia-open-p2p
VER=615.71.09
KVER=$(uname -r)

sudo cp -r . /usr/src/${NAME}-${VER}
sudo dkms add -m ${NAME} -v ${VER}
sudo dkms build -m ${NAME} -v ${VER} -k ${KVER}
sudo dkms install -m ${NAME} -v ${VER} -k ${KVER}
```

Modules built: `nvidia`, `nvidia-uvm`, `nvidia-modeset`, `nvidia-drm`.
Excluded: `nvidia-peermem` (GPUDirect RDMA, needs MLNX_OFED headers),
`nvidia-vgpu-vfio` (vGPU only).
Install target: `/lib/modules/<kernel>/updates/dkms/` on Arch/CachyOS
(DKMS overrides `DEST_MODULE_LOCATION`), `/lib/modules/<kernel>/extramodules/`
where that convention exists.

User-space components must come from the matching 615.71.09 driver release,
installed with `--no-kernel-modules`:

```sh
sh ./NVIDIA-Linux-*.run --no-kernel-modules
```

## Verify

```sh
dkms status | grep nvidia-open-p2p
modinfo nvidia | grep -E 'version:|filename:'
nvidia-smi
```

P2P check (CUDA samples): `p2pBandwidthLatencyTest` should report P2P
enabled between the Blackwell GPUs.

## Update / remove

Driver bump: `PACKAGE_VERSION` in `dkms.conf` and the `/usr/src/<name>-<ver>`
directory name must follow `version.mk`, then re-run the install block above.

```sh
sudo dkms remove -m nvidia-open-p2p -v 615.71.09 --all
sudo rm -rf /usr/src/nvidia-open-p2p-615.71.09
```

## Secure Boot (not yet enabled)

`POST_BUILD` signing hook is stubbed (commented) in `dkms.conf`.
Planned: `sign-file` with a MOK key + `mokutil --import`.

## Manual build (without DKMS)

```sh
make modules -j$(nproc)
sudo make modules_install -j$(nproc)
```

Cross-compile: see `TARGET_ARCH`, `CC`, `LD`, `ARCH` variables (e.g.
`TARGET_ARCH=aarch64 ARCH=arm64 CC=aarch64-linux-gnu-gcc …`).

`NV_VERBOSE=1` prints full commands; `DEBUG=1` builds with debug info.

## License / security

GPLv2, see `COPYING`. Security reports: see `SECURITY.md`.
Upstream issues: https://github.com/NVIDIA/open-gpu-kernel-modules/issues
