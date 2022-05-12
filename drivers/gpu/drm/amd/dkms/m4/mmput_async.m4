dnl #
dnl # v4.14-rc3-117-ga1b2289cef92 android: binder: drop lru lock in isolate callback
dnl # v4.13-4372-g212925802454 mm: oom: let oom_reap_task and exit_mmap run concurrently
dnl # v4.6-6601-gec8d7c14ea14 mm, oom_reaper: do not mmput synchronously from the oom reaper context
dnl #
AC_DEFUN([AC_AMDGPU_MMPUT_ASYNC], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_CHECK_SYMBOL_EXPORT([mmput_async], [kernel/fork.c], [
			AC_DEFINE(HAVE_MMPUT_ASYNC, 1, [mmput_async() is available])
		])
	])
])
