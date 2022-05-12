AC_DEFUN([AC_AMDGPU_DRM_HEADERS], [
	dnl #
	dnl # RHEL 7.x wrapper
	dnl #
	AC_KERNEL_CHECK_HEADERS([drm/drm_backport.h])

	dnl #
	dnl # Optional devices ID for amdgpu driver
	dnl #
	AC_KERNEL_CHECK_HEADERS([drm/amdgpu_pciid.h])

	dnl #
	dnl # commit v4.7-rc2-612-g3b96a0b1407e
	dnl # drm: document drm_auth.c
	dnl #
	AC_KERNEL_CHECK_HEADERS([drm/drm_auth.h])

	dnl #
	dnl # commit v4.7-rc5-1465-g34a67dd7f33f
	dnl # drm: Extract&Document drm_irq.h
	dnl #
	AC_KERNEL_CHECK_HEADERS([drm/drm_irq.h])

	dnl #
	dnl # commit v4.8-rc2-342-g522171951761
	dnl # drm: Extract drm_connector.[hc]
	dnl #
	AC_KERNEL_CHECK_HEADERS([drm/drm_connector.h])

	dnl #
	dnl # commit v4.8-rc2-384-g321a95ae35f2
	dnl # drm: Extract drm_encoder.[hc]
	dnl #
	AC_KERNEL_CHECK_HEADERS([drm/drm_encoder.h])

	dnl #
	dnl # v4.8-rc2-798-g43968d7b806d
	dnl # drm: Extract drm_plane.[hc]
	dnl #
	AC_KERNEL_CHECK_HEADERS([drm/drm_plane.h])

	dnl #
	dnl # commit v4.9-rc2-477-gd8187177b0b1
	dnl # drm: add helper for printing to log or seq_file
	dnl #
	AC_KERNEL_CHECK_HEADERS([drm/drm_print.h])

	dnl #
	dnl # v4.9-rc4-834-g85e634bce01a
	dnl # drm: Extract drm_drv.h
	dnl #
	AC_KERNEL_CHECK_HEADERS([drm/drm_drv.h])

	dnl #
	dnl # commit v4.10-rc8-1407-ga8f8b1d9b870
	dnl # drm: Extract drm_file.h
	dnl #
	AC_KERNEL_CHECK_HEADERS([drm/drm_file.h])

	dnl #
	dnl # commit v4.11-rc3-918-g4834442d70be
	dnl # drm: Extract drm_debugfs.h
	dnl #
	AC_KERNEL_CHECK_HEADERS([drm/drm_debugfs.h])

	dnl #
	dnl # commit v4.11-rc3-927-g7cfdf711ffb0
	dnl # drm: Extract drm_ioctl.h
	dnl #
	AC_KERNEL_CHECK_HEADERS([drm/drm_ioctl.h])

	dnl #
	dnl # commit v4.12-rc1-158-g3ed4351a83ca
	dnl # drm: Extract drm_vblank.[hc]
	dnl #
	AC_KERNEL_CHECK_HEADERS([drm/drm_vblank.h])

	dnl #
	dnl # commit v4.13-rc2-387-ge4672e55d6f3
	dnl # drm: Extract drm_device.h
	dnl #
	AC_KERNEL_CHECK_HEADERS([drm/drm_device.h])

	dnl #
	dnl # v4.13-rc2-421-g4c3dbb2c312c
	dnl # drm: Add GEM backed framebuffer library
	dnl #
	AC_KERNEL_CHECK_HEADERS([drm/drm_gem_framebuffer_helper.h])

	dnl #
	dnl # commit v4.15-rc4-1351-g495eb7f877ab
	dnl # drm: Add some HDCP related #defines
	dnl #
	AC_KERNEL_CHECK_HEADERS([drm/drm_hdcp.h])

	dnl #
	dnl # v4.18-rc1-27-gae891abe7c2c
	dnl # drm/i915: Split audio component to a generic type
	dnl #
	AC_KERNEL_CHECK_HEADERS([drm/drm_audio_component.h])

	dnl #
	dnl # commit v4.19-rc1-154-gd78aa650670d
	dnl # drm: Add drm/drm_util.h header file
	dnl #
	dnl # commit e9eafcb589213395232084a2378e2e90f67feb29
	dnl # drm: move drm_can_sleep() to drm_util.h
	dnl #
	AC_KERNEL_CHECK_HEADERS([drm/drm_util.h])

	dnl #
	dnl # commit v4.19-rc1-160-g72fdb40c1a4b
	dnl # drm: extract drm_atomic_uapi.c
	dnl #
	AC_KERNEL_CHECK_HEADERS([drm/drm_atomic_uapi.h])

	dnl #
	dnl # commit v5.0-rc1-342-gfcd70cd36b9b
	dnl # drm: Split out drm_probe_helper.h
	dnl #
	AC_KERNEL_CHECK_HEADERS([drm/drm_probe_helper.h])

	dnl #
	dnl # v5.4-rc1-214-g4e98f871bcff
	dnl # drm: delete drmP.h + drm_os_linux.h
	dnl #
	AC_KERNEL_CHECK_HEADERS([drm/drmP.h])

	dnl #
	dnl # commit v5.5-rc2-783-g368fd0aad1be
	dnl # drm: Add Reusable task barrier.
	dnl #
	AC_KERNEL_CHECK_HEADERS([drm/task_barrier.h])

	dnl #
	dnl # v5.6-rc5-1258-gc6603c740e0e
	dnl # drm: add managed resources tied to drm_device
	dnl #
	AC_KERNEL_CHECK_HEADERS([drm/drm_managed.h])

	dnl #
	dnl # Required by AC_KERNEL_SUPPORTED_AMD_CHIPS macro
	dnl #
	AC_KERNEL_CHECK_HEADERS([drm/amd_asic_type.h])

	dnl #
	dnl # v5.12-rc3-330-g2916059147ea
	dnl # drm/aperture: Add infrastructure for aperture ownership
	dnl #
	AC_KERNEL_CHECK_HEADERS([drm/drm_aperture.h])

	dnl #
	dnl # v5.7-13141-gca5999fde0a1
	dnl # mm: introduce include/linux/pgtable.h
	dnl #
	AC_KERNEL_CHECK_HEADERS([linux/pgtable.h])
	
	dnl #
	dnl # v4.7-rc2-479-g5b8090747a11
	dnl # drm: Add helper for simple display pipeline
	dnl #
	AC_KERNEL_CHECK_HEADERS([drm/drm_simple_kms_helper.h])
])
