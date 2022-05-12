dnl #
dnl # commit c42b65e363ce introduce this change
dnl # v4.17-3-gc42b65e363ce
dnl # bitmap: Add bitmap_alloc(), bitmap_zalloc() and bitmap_free()
dnl #
AC_DEFUN([AC_AMDGPU_DRM_BITMAP_FUNCS], [
        AC_KERNEL_DO_BACKGROUND([
                AC_KERNEL_TRY_COMPILE([
                        #include <linux/bitmap.h>
                ],[
                        bitmap_free(NULL);
                        bitmap_alloc(0, 0);
                        bitmap_zalloc(0, 0);
                ],[
                        AC_DEFINE(HAVE_BITMAP_FUNCS,
                                1,
                                [bitmap_free(),bitmap_alloc(),bitmap_zalloc is available])
                ])
        ])
])
