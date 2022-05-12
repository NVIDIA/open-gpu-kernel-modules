# SPDX-License-Identifier: GPL-2.0
#
# Makefile for the drm device driver.  This driver provides support for the

#
# In DKMS mode the module can be renamed by passing TTM_NAME as a parameter
# to 'make' if required
#
TTM_NAME = ttm
ccflags-y += \
	-DTTM_NAME="\"$(TTM_NAME)\""

$(TTM_NAME)-y := ttm_tt.o ttm_bo.o ttm_bo_util.o ttm_bo_vm.o ttm_module.o \
	ttm_execbuf_util.o ttm_range_manager.o ttm_resource.o ttm_pool.o \
	ttm_device.o ttm_sys_manager.o
$(TTM_NAME)-$(CONFIG_AGP) += ttm_agp_backend.o

obj-$(CONFIG_DRM_TTM) += $(TTM_NAME).o

ifeq ($(KBUILD_EXTMOD),)
TTM_FULL_PATH := $(srctree)/$(src)
else
TTM_FULL_PATH := $(src)
endif
include $(TTM_FULL_PATH)/backport/Makefile
