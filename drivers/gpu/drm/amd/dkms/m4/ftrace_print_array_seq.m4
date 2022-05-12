dnl #
dnl # commit 0fe7e2764d6f
dnl # add new trace event for page table update
dnl # ftrace_print_array_seq() is exported in v3.19-rc1-6-g6ea22486ba46
dnl #
AC_DEFUN([AC_AMDGPU___PRINT_ARRAY], [
		 AC_KERNEL_DO_BACKGROUND([
			AC_KERNEL_CHECK_SYMBOL_EXPORT([ftrace_print_array_seq], [kernel/trace/trace_output.c], [
			AC_DEFINE(HAVE___PRINT_ARRAY, 1, [__print_array is available])
			], [
				dnl #
				dnl # 645df987f7c
				dnl # trace_print_array_seq() is exported in v4.1-rc3-8-g645df987f7c1
				dnl #
				AC_KERNEL_CHECK_SYMBOL_EXPORT(
					[trace_print_array_seq],
					[kernel/trace/trace_output.c],[
						AC_DEFINE(HAVE___PRINT_ARRAY, 1,
							[__print_array is available])
			])
		])
	])
])
