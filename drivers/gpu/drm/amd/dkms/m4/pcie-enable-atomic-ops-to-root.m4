dnl #
dnl # commit 430a23689dea2e36ae5a0fc75a67301fd46b18bf
dnl # Author: Jay Cornwall <Jay.Cornwall@amd.com>
dnl # Date:   Thu Jan 4 19:44:59 2018 -0500
dnl # PCI: Add pci_enable_atomic_ops_to_root()
dnl #
AC_DEFUN([AC_AMDGPU_PCIE_ENABLE_ATOMIC_OPS_TO_ROOT], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE_SYMBOL([
			#include <linux/pci.h>
		], [
			pci_enable_atomic_ops_to_root(NULL, 0);
		], [pci_enable_atomic_ops_to_root], [drivers/pci/pci.c], [
			AC_DEFINE(HAVE_PCIE_ENABLE_ATOMIC_OPS_TO_ROOT, 1,
				[pci_enable_atomic_ops_to_root() exist])
		])
	])
])
