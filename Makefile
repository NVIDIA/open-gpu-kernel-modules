###########################################################################
# This is the top level makefile for the NVIDIA Linux kernel module source
# package.
#
# To build: run `make modules`
# To install the build kernel modules: run (as root) `make modules_install`
###########################################################################

include utils.mk

all: modules

nv_kernel_o                = src/nvidia/$(OUTPUTDIR)/nv-kernel.o
nv_kernel_o_binary         = kernel-open/nvidia/nv-kernel.o_binary

nv_modeset_kernel_o        = src/nvidia-modeset/$(OUTPUTDIR)/nv-modeset-kernel.o
nv_modeset_kernel_o_binary = kernel-open/nvidia-modeset/nv-modeset-kernel.o_binary

.PHONY: $(nv_kernel_o) $(nv_modeset_kernel_o) modules modules_install


###########################################################################
# nv-kernel.o is the OS agnostic portion of nvidia.ko
###########################################################################

$(nv_kernel_o):
	$(MAKE) -C src/nvidia

$(nv_kernel_o_binary): $(nv_kernel_o)
	cd $(dir $@) && ln -sf ../../$^ $(notdir $@)


###########################################################################
# nv-modeset-kernel.o is the OS agnostic portion of nvidia-modeset.ko
###########################################################################

$(nv_modeset_kernel_o):
	$(MAKE) -C src/nvidia-modeset

$(nv_modeset_kernel_o_binary): $(nv_modeset_kernel_o)
	cd $(dir $@) && ln -sf ../../$^ $(notdir $@)


###########################################################################
# After the OS agnostic portions are built, descend into kernel-open/ and build
# the kernel modules with kbuild.
###########################################################################

modules: $(nv_kernel_o_binary) $(nv_modeset_kernel_o_binary)
	$(MAKE) -C kernel-open modules


###########################################################################
# Install the built kernel modules using kbuild.
###########################################################################

modules_install:
	$(MAKE) -C kernel-open modules_install


###########################################################################
# clean
###########################################################################

.PHONY: clean nvidia.clean nvidia-modeset.clean kernel-open.clean

clean: nvidia.clean nvidia-modeset.clean kernel-open.clean

nvidia.clean:
	$(MAKE) -C src/nvidia clean

nvidia-modeset.clean:
	$(MAKE) -C src/nvidia-modeset clean

kernel-open.clean:
	$(MAKE) -C kernel-open clean
