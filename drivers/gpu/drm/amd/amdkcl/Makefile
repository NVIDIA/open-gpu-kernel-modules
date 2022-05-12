# SPDX-License-Identifier: MIT
amdkcl-y += main.o symbols.o kcl_common.o

amdkcl-y += kcl_kernel_params.o
amdkcl-y += dma-buf/dma-resv.o

amdkcl-y += kcl_backlight.o kcl_ioctl.o \
	kcl_kthread.o kcl_io.o kcl_perf_event.o kcl_seq_file.o \
	kcl_suspend.o kcl_pci.o kcl_mm.o kcl_memory.o kcl_sched.o \
	kcl_fence.o kcl_fence_array.o kcl_reservation.o kcl_drm_cache.o \
	kcl_drm_auth.o kcl_drm_fb.o kcl_drm_print.o kcl_fbmem.o \
	kcl_drm_crtc.o kcl_drm_connector.o kcl_drm_atomic_helper.o \
	kcl_device_cgroup.o kcl_mn.o kcl_drm_modes.o kcl_time.o kcl_ftrace.o \
	kcl_acpi_table.o kcl_page_alloc.o kcl_numa.o kcl_fs_read_write.o kcl_drm_aperture.o \
	kcl_drm_drv.o kcl_drm_simple_kms_helper.o kcl_bitmap.o

amdkcl-$(CONFIG_DRM_AMD_DC_HDCP) += kcl_drm_hdcp.o
amdkcl-$(CONFIG_MMU_NOTIFIER) += kcl_mn.o
amdkcl-$(CONFIG_DEBUG_FS) += kcl_debugfs_inode.o
amdkcl-$(CONFIG_SYSFS) += kcl_sysfs_emit.o

CFLAGS_kcl_fence.o := -I$(src)

ccflags-y += \
	-include $(src)/../dkms/config/config.h \
	-include $(src)/kcl_common.h

ccflags-y += -DHAVE_CONFIG_H

obj-m += amdkcl.o
