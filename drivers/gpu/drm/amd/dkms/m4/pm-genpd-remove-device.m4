dnl #
dnl # commit v4.17-rc5-40-g924f44869962
dnl # PM / Domains: Drop genpd as in-param for pm_genpd_remove_device()
dnl #
AC_DEFUN([AC_AMDGPU_PM_GENPD_REMOVE_DEVICE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/pm_domain.h>
		], [
			pm_genpd_remove_device(NULL, NULL);
		], [
			AC_DEFINE(HAVE_PM_GENPD_REMOVE_DEVICE_2ARGS, 1,
				[pm_genpd_remove_device() wants 2 arguments])
		])
	])
])

