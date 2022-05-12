dnl #
dnl # mm: add zone device public type memory support
dnl #
AC_DEFUN([AC_AMDGPU_ZONE_DEVICE_PUBLIC], [
	AC_KERNEL_TRY_COMPILE([
		#include <linux/mm.h>
	], [
		is_device_page(NULL);
	], [
		AC_DEFINE(HAVE_ZONE_DEVICE_PUBLIC, 1,
			[is_device_page is available])
	])
])

dnl #
dnl # v5.8-rc4-7-g5143192cd410 mm/migrate: add a flags parameter to migrate_vma
dnl #
AC_DEFUN([AC_AMDGPU_MIGRATE_VMA_PGMAP_OWNER], [
	AC_KERNEL_TRY_COMPILE([
		#include <linux/migrate.h>
	], [
		struct migrate_vma *migrate = NULL;
		migrate->pgmap_owner = NULL;
	], [
		AC_DEFINE(HAVE_MIGRATE_VMA_PGMAP_OWNER, 1,
			[migrate_vma->pgmap_owner is available])
	])

	AC_AMDGPU_ZONE_DEVICE_PUBLIC
])

dnl #
dnl # v5.6-rc3-15-g800bb1c8dc80 mm: handle multiple owners of device private pages in migrate_vma
dnl # v5.6-rc3-14-gf894ddd5ff01 memremap: add an owner field to struct dev_pagemap
dnl #
AC_DEFUN([AC_AMDGPU_HSA_AMD_SVM], [
	AC_KERNEL_TRY_COMPILE([
		#include <linux/memremap.h>
		#if !IS_ENABLED(CONFIG_DEVICE_PRIVATE)
		#error "DEVICE_PRIVATE is a must for svm support"
		#endif
	], [
		struct dev_pagemap *pm = NULL;
		pm->owner = NULL;
	], [
		AC_DEFINE(HAVE_DEV_PAGEMAP_OWNER, 1,
			[dev_pagemap->owner is available])
 
	AC_AMDGPU_MIGRATE_VMA_PGMAP_OWNER
 ])
])

