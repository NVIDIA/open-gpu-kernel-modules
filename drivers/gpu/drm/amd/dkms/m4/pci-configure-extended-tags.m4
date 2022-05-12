dnl #
dnl # commit 62ce94a7a5a54aac80975f5e6731707225d4077e
dnl # PCI: Mark Broadcom HT2100 Root Port Extended Tags as broken
dnl #
AC_DEFUN([AC_AMDGPU_PCI_CONFIGURE_EXTENDED_TAGS], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/pci.h>
		], [
			struct pci_host_bridge bridge;
			bridge.no_ext_tags = 0;
		], [
			AC_DEFINE(HAVE_PCI_CONFIGURE_EXTENDED_TAGS, 1,
				[PCI driver handles extended tags])
		])
	])
])
