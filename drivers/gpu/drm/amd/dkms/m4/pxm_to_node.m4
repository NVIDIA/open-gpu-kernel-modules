dnl #
dnl # v5.7-20-gf2af6d3978d7
dnl # virtio-mem: Allow to specify an ACPI PXM as nid
dnl #
AC_DEFUN([AC_AMDGPU_PXM_TO_NODE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_CHECK_SYMBOL_EXPORT([pxm_to_node],
		[drivers/acpi/numa/srat.c], [
			AC_DEFINE(HAVE_PXM_TO_NODE, 1,
				[pxm_to_node() is available])
		])
	])
])
