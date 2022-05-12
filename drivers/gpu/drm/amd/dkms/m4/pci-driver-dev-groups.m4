dnl #
dnl # commit ded13b9cfd595adb478a1e371d2282048bba1df5
dnl # PCI: Add support for dev_groups to struct pci_driver
dnl #
AC_DEFUN([AC_AMDGPU_PCI_DRIVER_DEV_GROUPS], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/pci.h>
		], [
			struct pci_driver *pd = NULL;
			pd->dev_groups = NULL;
		], [
			AC_DEFINE(HAVE_PCI_DRIVER_DEV_GROUPS, 1, [struct pci_driver has field dev_groups])
		])
	])
])
