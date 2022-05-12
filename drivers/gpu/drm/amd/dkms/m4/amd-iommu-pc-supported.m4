dnl #
dnl # v5.12-rc3-5-gfc1b6620501f iommu/amd: Move a few prototypes to include/linux/amd-iommu.h
dnl # v5.12-rc3-4-gb29a1fc7595a iommu/amd: Remove a few unused exports
dnl # v4.11-rc4-171-gf5863a00e73c x86/events/amd/iommu.c: Modify functions to query max banks and counters
dnl #
AC_DEFUN([AC_AMDGPU_AMD_IOMMU_PC_GET_MAX_BANKS], [
	AC_KERNEL_TRY_COMPILE([
		#include <linux/amd-iommu.h>
	], [
		amd_iommu_pc_get_max_banks(0);
	], [
		AC_DEFINE(HAVE_AMD_IOMMU_PC_GET_MAX_BANKS_DECLARED, 1,
					[amd_iommu_pc_get_max_banks() declared])
	], [
		AC_KERNEL_CHECK_SYMBOL_EXPORT([get_amd_iommu],
		[drivers/iommu/amd/init.c], [
			AC_DEFINE(HAVE_AMD_IOMMU_PC_GET_MAX_BANKS_UINT, 1,
					[amd_iommu_pc_get_max_banks() arg is unsigned int])
		])
	])
])

dnl #
dnl # commit v3.10-rc3-89-g30861ddc9cca
dnl # perf/x86/amd: Add IOMMU Performance Counter resource management
dnl #
AC_DEFUN([AC_AMDGPU_AMD_IOMMU_PC_SUPPORTED], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE_SYMBOL([
			#include <generated/autoconf.h>
		], [
			#ifndef CONFIG_AMD_IOMMU
			#error CONFIG_AMD_IOMMU not enabled
			#endif
		], [amd_iommu_pc_supported], [drivers/iommu/amd/init.c], [
			AC_DEFINE(HAVE_AMD_IOMMU_PC_SUPPORTED, 1,
				[amd_iommu_pc_supported() is available])
			AC_AMDGPU_AMD_IOMMU_PC_GET_MAX_BANKS
		])
	])
])
