###########################################################################
# Kbuild fragment for nvidia-drm.ko
###########################################################################

# Get our source file list and conftest list from the common file
include $(src)/nvidia-drm/nvidia-drm-sources.mk

# Linux-specific sources
NVIDIA_DRM_SOURCES += nvidia-drm/nvidia-drm-linux.c

#
# Define NVIDIA_DRM_{SOURCES,OBJECTS}
#

NVIDIA_DRM_OBJECTS = $(patsubst %.c,%.o,$(NVIDIA_DRM_SOURCES))

obj-m += nvidia-drm.o
nvidia-drm-y := $(NVIDIA_DRM_OBJECTS)

NVIDIA_DRM_KO = nvidia-drm/nvidia-drm.ko

NV_KERNEL_MODULE_TARGETS += $(NVIDIA_DRM_KO)

#
# Define nvidia-drm.ko-specific CFLAGS.
#

NVIDIA_DRM_CFLAGS += -I$(src)/nvidia-drm
NVIDIA_DRM_CFLAGS += -UDEBUG -U_DEBUG -DNDEBUG -DNV_BUILD_MODULE_INSTANCES=0

$(call ASSIGN_PER_OBJ_CFLAGS, $(NVIDIA_DRM_OBJECTS), $(NVIDIA_DRM_CFLAGS))

NV_OBJECTS_DEPEND_ON_CONFTEST += $(NVIDIA_DRM_OBJECTS)
