# SPDX-License-Identifier: MIT
BACKPORT_OBJS := kcl_drm_fb_helper.o kcl_drm_drv.o kcl_drm_gem_ttm_helper.o \
		kcl_drm_gem_framebuffer_helper.o kcl_drm_gem.o \
		kcl_drm_modeset_helper.o

amdgpu-y += $(addprefix ../backport/,$(BACKPORT_OBJS))

ccflags-y += \
	-I$(FULL_AMD_PATH) \
	-I$(FULL_AMD_PATH)/backport/include \
	-I$(FULL_AMD_PATH)/dkms \
	-include config/config.h \
	-include backport/backport.h

ccflags-y += -DHAVE_CONFIG_H
