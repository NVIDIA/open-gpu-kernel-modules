dnl #
dnl # commit 192f1bf7559e895d51f81c3976c5892c8b1e0601
dnl # PCI: Add pci_rebar_bytes_to_size()
dnl #
AC_DEFUN([AC_AMDGPU_PCI_REBAR_BYTES_TO_SIZE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/pci.h>
		], [
			pci_rebar_bytes_to_size(0);
		], [
			AC_DEFINE(HAVE_PCI_REBAR_BYTES_TO_SIZE, 1,
				[pci_rebar_bytes_to_size() is available])
		])
	])
])
