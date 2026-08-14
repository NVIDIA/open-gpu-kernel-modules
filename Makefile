###########################################################################
# This is the top level makefile for the NVIDIA Linux kernel module source
# package.
#
# To build: run `make modules`
# To install the build kernel modules: run (as root) `make modules_install`
###########################################################################

# Experimental use of the Linux kernel's kbuild system
USE_KBUILD ?= 0
export USE_KBUILD

###########################################################################
# rules
###########################################################################

.PHONY: all
all: modules

ifneq ($(USE_KBUILD),1)

###########################################################################
# variables
###########################################################################

nv_kernel_o                = src/nvidia/$(OUTPUTDIR)/nv-kernel.o
nv_kernel_o_binary         = kernel-open/nvidia/nv-kernel.o_binary

nv_modeset_kernel_o        = src/nvidia-modeset/$(OUTPUTDIR)/nv-modeset-kernel.o
nv_modeset_kernel_o_binary = kernel-open/nvidia-modeset/nv-modeset-kernel.o_binary

include utils.mk

###########################################################################
# nv-kernel.o is the OS agnostic portion of nvidia.ko
###########################################################################

.PHONY: $(nv_kernel_o)
$(nv_kernel_o):
	$(MAKE) -C src/nvidia

$(nv_kernel_o_binary): $(nv_kernel_o)
	cd $(dir $@) && ln -sf ../../$^ $(notdir $@)


###########################################################################
# nv-modeset-kernel.o is the OS agnostic portion of nvidia-modeset.ko
###########################################################################

.PHONY: $(nv_modeset_kernel_o)
$(nv_modeset_kernel_o):
	$(MAKE) -C src/nvidia-modeset

$(nv_modeset_kernel_o_binary): $(nv_modeset_kernel_o)
	cd $(dir $@) && ln -sf ../../$^ $(notdir $@)

# Make the OS agnostic binaries a prerequisite of the modules target
modules: $(nv_kernel_o_binary) $(nv_modeset_kernel_o_binary)
endif

###########################################################################
# After the OS agnostic portions are built, descend into kernel-open/ and build
# the kernel modules with kbuild.
###########################################################################

.PHONY: modules
modules:
ifeq ($(USE_KBUILD),1)
# create nv-kernel.o and nv-modeset-kernel.o by building stub modules -- a
# required hack to overcome kbuild limitations regarding the maximum number
# of object files to link into a module.
	$(MAKE) -C kernel-open modules NV_PREPARE_ONLY=1
endif
	$(MAKE) -C kernel-open modules

###########################################################################
# Install the built kernel modules using kbuild.
###########################################################################

.PHONY: modules_install
modules_install: modules
	$(MAKE) -C kernel-open modules_install

###########################################################################
# clean
###########################################################################

.PHONY: clean
clean: nvidia.clean nvidia-modeset.clean kernel-open.clean

.PHONY: nvidia.clean
nvidia.clean:
ifneq ($(USE_KBUILD),1)
	$(MAKE) -C src/nvidia clean
endif

.PHONY: nvidia-modeset.clean
nvidia-modeset.clean:
ifneq ($(USE_KBUILD),1)
	$(MAKE) -C src/nvidia-modeset clean
endif

.PHONY: kernel-open.clean
kernel-open.clean:
	$(MAKE) -C kernel-open clean
