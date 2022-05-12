dnl #
dnl # commit v4.8-14095-g768ae309a961
dnl # mm: replace get_user_pages() write/force parameters with gup_flags
dnl #
AC_DEFUN([AC_AMDGPU_GET_USER_PAGES], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE_SYMBOL([
			#include <linux/mm.h>
		], [
			get_user_pages(0, 0, 0, NULL, NULL);
		], [get_user_pages], [mm/gup.c], [
			AC_DEFINE(HAVE_GET_USER_PAGES_GUP_FLAGS, 1,
				[get_user_pages() wants gup_flags parameter])
		], [
			dnl #
			dnl # commit v4.6-rc2-1-gc12d2da56d0e
			dnl # mm/gup: Remove the macro overload API migration helpers
			dnl #         from the get_user*() APIs
			dnl #
			AC_KERNEL_TRY_COMPILE_SYMBOL([
				#include <linux/mm.h>
			], [
				get_user_pages(0, 0, 0, 0, NULL, NULL);
			], [get_user_pages], [mm/gup.c], [
				AC_DEFINE(HAVE_GET_USER_PAGES_6ARGS, 1,
					[get_user_pages() wants 6 args])
			])
		])
	])
])
