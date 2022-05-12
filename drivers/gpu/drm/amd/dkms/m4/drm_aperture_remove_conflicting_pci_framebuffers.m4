dnl #
dnl # v5.12-rc3-332-g603dc7ed917f drm/aperture: Inline fbdev conflict helpers into aperture helpers
dnl # v5.12-rc3-330-g2916059147ea drm/aperture: Add infrastructure for aperture ownership
dnl #
AC_DEFUN([AC_AMDGPU_DRM_APERTURE_REMOVE_CONFLICTING_PCI_FRAMEBUFFERS], [
	AC_KERNEL_DO_BACKGROUND([
		AS_IF([test x$HAVE_DRM_DRM_APERTURE_H = x ], [
			dnl #
			dnl # v5.3-rc1-540-g0a8459693238 fbdev: drop res_id parameter from remove_conflicting_pci_framebuffers
			dnl #
			AC_KERNEL_TRY_COMPILE([
				struct task_struct;
				#include <linux/fb.h>
			], [
				remove_conflicting_pci_framebuffers(NULL, NULL);
			], [
				AC_DEFINE(HAVE_REMOVE_CONFLICTING_PCI_FRAMEBUFFERS_PP, 1,
					[remove_conflicting_pci_framebuffers() wants p,p args])
				AC_DEFINE(HAVE_REMOVE_CONFLICTING_PCI_FRAMEBUFFERS, 1,
						[remove_conflicting_pci_framebuffers() is available])
			], [
				dnl #
				dnl # v4.19-rc1-110-g4d18975c78f2 fbdev: add remove_conflicting_pci_framebuffers()
				dnl #
				AC_KERNEL_TRY_COMPILE([
					struct task_struct;
					#include <linux/fb.h>
				], [
					remove_conflicting_pci_framebuffers(NULL, 0, NULL);
				], [
					AC_DEFINE(HAVE_REMOVE_CONFLICTING_PCI_FRAMEBUFFERS_PIP, 1,
						[remove_conflicting_pci_framebuffers() wants p,i,p args])
					AC_DEFINE(HAVE_REMOVE_CONFLICTING_PCI_FRAMEBUFFERS, 1,
						[remove_conflicting_pci_framebuffers() is available])
				])
			])
		])
	])
])
