dnl #
dnl # commit c6bde215acfd637708142ae671843b6f0eadbc6d
dnl # Author: Bjorn Helgaas <bhelgaas@google.com>
dnl # Date:   Wed Nov 6 10:11:48 2013 -0700
dnl # PCI: Add pci_upstream_bridge()
dnl #
AC_DEFUN([AC_AMDGPU_PCI_UPSTREAM_BRIDGE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/pci.h>
		], [
			pci_upstream_bridge(NULL);
		], [
			AC_DEFINE(HAVE_PCI_UPSTREAM_BRIDGE, 1,
				[pci_upstream_bridge() is available])
		])
	])
])
