dnl #
dnl # v4.16-rc1-1232-g75a57669cbc8 drm/ttm: add ttm_sg_tt_init
dnl # v4.16-rc1-409-g186ca446aea1 drm/prime: make the pages array optional for drm_prime_sg_to_page_addr_arrays
dnl #
AC_DEFUN([AC_AMDGPU_TTM_SG_TT_INIT], [
	AC_KERNEL_DO_BACKGROUND([
		AS_IF([grep -q ttm_sg_tt_init $LINUX/include/drm/ttm/ttm_tt.h > /dev/null 2>&1], [
			AC_DEFINE(HAVE_TTM_SG_TT_INIT, 1, [ttm_sg_tt_init() is available])
		])
	])
])
