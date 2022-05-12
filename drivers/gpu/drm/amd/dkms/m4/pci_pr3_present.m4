dnl #
dnl # v5.4-rc2-37-g52525b7a3cf8
dnl # PCI: Add a helper to check Power Resource Requirements _PR3 existence
dnl #
AC_DEFUN([AC_AMDGPU_PCI_PR3_PRESENT], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_CHECK_SYMBOL_EXPORT([pci_pr3_present], [drivers/pci/pci.c], [
			AC_DEFINE(HAVE_PCI_PR3_PRESENT, 1, [pci_pr3_present() is available])
		])
	])
])
