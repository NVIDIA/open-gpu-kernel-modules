#
# This Makefile was automatically generated; do not edit.
#

###########################################################################
# Makefile for NVIDIA Linux GPU driver kernel modules
###########################################################################

# This makefile is read twice: when a user or nvidia-installer invokes
# 'make', this file is read.  It then invokes the Linux kernel's
# Kbuild.  Modern versions of Kbuild will then read the Kbuild file in
# this directory.  However, old versions of Kbuild will instead read
# this Makefile.  For backwards compatibility, when read by Kbuild
# (recognized by KERNELRELEASE not being empty), do nothing but
# include the Kbuild file in this directory.

ifneq ($(KERNELRELEASE),)
  include $(src)/Kbuild
else

  # Determine the location of the Linux kernel source tree, and of the
  # kernel's output tree.  Use this to invoke Kbuild, and pass the paths
  # to the source and output trees to NVIDIA's Kbuild file via
  # NV_KERNEL_{SOURCES,OUTPUT}.

  ifdef SYSSRC
    KERNEL_SOURCES := $(SYSSRC)
  else
    KERNEL_UNAME ?= $(shell uname -r)
    KERNEL_MODLIB := /lib/modules/$(KERNEL_UNAME)
    KERNEL_SOURCES := $(shell ((test -d $(KERNEL_MODLIB)/source && echo $(KERNEL_MODLIB)/source) || (test -d $(KERNEL_MODLIB)/build/source && echo $(KERNEL_MODLIB)/build/source)) || echo $(KERNEL_MODLIB)/build)
  endif

  KERNEL_OUTPUT := $(KERNEL_SOURCES)
  KBUILD_PARAMS :=

  ifdef SYSOUT
    ifneq ($(SYSOUT), $(KERNEL_SOURCES))
      KERNEL_OUTPUT := $(SYSOUT)
      KBUILD_PARAMS := KBUILD_OUTPUT=$(KERNEL_OUTPUT)
    endif
  else
    KERNEL_UNAME ?= $(shell uname -r)
    KERNEL_MODLIB := /lib/modules/$(KERNEL_UNAME)
    # $(filter patter...,text) - Returns all whitespace-separated words in text that
    # do match any of the pattern words, removing any words that do not match.
    # Set the KERNEL_OUTPUT only if either $(KERNEL_MODLIB)/source or
    # $(KERNEL_MODLIB)/build/source path matches the KERNEL_SOURCES.
    ifneq ($(filter $(KERNEL_SOURCES),$(KERNEL_MODLIB)/source $(KERNEL_MODLIB)/build/source),)
      KERNEL_OUTPUT := $(KERNEL_MODLIB)/build
      KBUILD_PARAMS := KBUILD_OUTPUT=$(KERNEL_OUTPUT)
    endif
  endif

  CC ?= cc
  LD ?= ld
  OBJDUMP ?= objdump

  ifndef ARCH
    ARCH := $(shell uname -m | sed -e 's/i.86/i386/' \
      -e 's/armv[0-7]\w\+/arm/' \
      -e 's/aarch64/arm64/' \
      -e 's/ppc64le/powerpc/' \
      -e 's/riscv64/riscv/' \
    )
  endif

  NV_KERNEL_MODULES ?= $(wildcard nvidia nvidia-uvm nvidia-vgpu-vfio nvidia-modeset nvidia-drm nvidia-peermem)
  NV_KERNEL_MODULES := $(filter-out $(NV_EXCLUDE_KERNEL_MODULES), \
                                    $(NV_KERNEL_MODULES))
  INSTALL_MOD_DIR ?= kernel/drivers/video

  NV_VERBOSE ?=
  SPECTRE_V2_RETPOLINE ?= 0

  ifeq ($(NV_VERBOSE),1)
    KBUILD_PARAMS += V=1
  endif
  KBUILD_PARAMS += -C $(KERNEL_SOURCES) M=$(CURDIR)
  KBUILD_PARAMS += ARCH=$(ARCH)
  KBUILD_PARAMS += NV_KERNEL_SOURCES=$(KERNEL_SOURCES)
  KBUILD_PARAMS += NV_KERNEL_OUTPUT=$(KERNEL_OUTPUT)
  KBUILD_PARAMS += NV_KERNEL_MODULES="$(NV_KERNEL_MODULES)"
  KBUILD_PARAMS += INSTALL_MOD_DIR="$(INSTALL_MOD_DIR)"
  KBUILD_PARAMS += NV_SPECTRE_V2=$(SPECTRE_V2_RETPOLINE)

  .PHONY: modules module clean clean_conftest modules_install
  modules clean modules_install:
	@$(MAKE) "LD=$(LD)" "CC=$(CC)" "OBJDUMP=$(OBJDUMP)" $(KBUILD_PARAMS) $@
	@if [ "$@" = "modules" ]; then \
	  for module in $(NV_KERNEL_MODULES); do \
	    if [ -x split-object-file.sh ]; then \
	      ./split-object-file.sh $$module.ko; \
	    fi; \
	  done; \
	fi

  # Compatibility target for scripts that may be directly calling the
  # "module" target from the old build system.

  module: modules

  # Check if the any of kernel module linker scripts exist. If they do, pass
  # them as linker options (via variable NV_MODULE_LD_SCRIPTS) while building
  # the kernel interface object files. These scripts do some processing on the
  # module symbols on which the Linux kernel's module resolution is dependent
  # and hence must be used whenever present.

  LD_SCRIPT ?= $(KERNEL_SOURCES)/scripts/module-common.lds      \
               $(KERNEL_SOURCES)/arch/$(ARCH)/kernel/module.lds \
               $(KERNEL_OUTPUT)/scripts/module.lds
  NV_MODULE_COMMON_SCRIPTS := $(foreach s, $(wildcard $(LD_SCRIPT)), -T $(s))

  # Use $* to match the stem % in the kernel interface file %-linux.o. Replace
  # "nv" with "nvidia" in $* as appropriate: e.g. nv-modeset-linux.o links
  # nvidia-modeset.mod.o and nvidia-modeset/nv-modeset-interface.o. The kernel
  # interface file must have the .mod.o object linked into it: otherwise, the
  # kernel module produced by linking the interface against its corresponding
  # core object file will not be loadable. The .mod.o file is built as part of
  # the MODPOST process (stage 2),  so the rule to build the kernel interface
  # cannot be defined in the *Kbuild files, which are only used during stage 1.

  %-linux.o: modules
	$(LD) $(NV_MODULE_COMMON_SCRIPTS) -r -o $@ \
	  $(subst nv,nvidia,$*).mod.o $(subst nv,nvidia,$*)/$*-interface.o

  # Kbuild's "clean" rule won't clean up the conftest headers on its own, and
  # clean-dirs doesn't appear to work as advertised.
  clean_conftest:
	$(RM) -r conftest
  clean: clean_conftest

endif # KERNELRELEASE
