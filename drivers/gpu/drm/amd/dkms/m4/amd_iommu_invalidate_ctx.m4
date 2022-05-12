dnl #
dnl # commit c7b6bac9c72c5fcbd6e9e12545bd3022c7f21860
dnl # drm, iommu: Change type of pasid to u32
dnl #
AC_DEFUN([AC_AMDGPU_AMD_IOMMU_INVALIDATE_CTX], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/amd-iommu.h>
		], [
			void (*f)(struct pci_dev *pdev, u32 pasid);
			amd_iommu_invalidate_ctx callback = f;
		], [
			AC_DEFINE(HAVE_AMD_IOMMU_INVALIDATE_CTX_PASID_U32, 1,
				[amd_iommu_invalidate_ctx take arg type of pasid as u32])
		])
	])
])
