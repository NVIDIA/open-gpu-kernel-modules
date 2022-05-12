dnl #
dnl # commit v4.15-rc3-1-g82975c46da82
dnl # perf: Export perf_event_update_userpage
dnl #   Export perf_event_update_userpage() so that PMU driver using them,
dnl #   can be built as modules
dnl #
AC_DEFUN([AC_AMDGPU_PERF_EVENT_UPDATE_USERPAGE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_CHECK_SYMBOL_EXPORT([perf_event_update_userpage],[kernel/events/core.c],[
			AC_DEFINE(HAVE_PERF_EVENT_UPDATE_USERPAGE, 1,
				[perf_event_update_userpage() is exported])
		])
	])
])
