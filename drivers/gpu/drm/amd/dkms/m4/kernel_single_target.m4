dnl #
dnl # extract cc, cflags, cppflags
dnl #
AC_DEFUN([AC_KERNEL_SINGLE_TARGET_CFLAGS], [
	AS_IF([test -s .conftest.o.cmd], [
		_base_cflags="-DKBUILD_BASENAME='\"conftest\"' -DKBUILD_MODNAME='\"conftest\"'"
		_base_dir=$(basename $PWD)
		_conftest_cmd=$(head -1 .conftest.o.cmd)

		CC=$(echo $_conftest_cmd | awk -F ' ' '{print $[3]}')
		CFLAGS=$(echo $_conftest_cmd | \
			 sed -e 's| -|\n&|g' | \
			 sed -e "s|\./|${LINUX_OBJ}/|" \
			     -e "s|-I\([[[a-z]]]*\)|-I${LINUX_OBJ}/\1|" \
			     -e "s|-include \([[[a-z]]]*\)|-include ${LINUX_OBJ}/\1|" \
			     -e '/conftest/d' \
			     -e '/KBUILD_/d' \
			     -e "/$_base_dir/d" | \
			 xargs)
		CPPFLAGS=$(echo $CFLAGS | \
			   sed 's| -|\n&|g' | \
			   sed -n '/-I/p; /-include/p; /-isystem/p; /-D/p' | \
			   xargs)

		CFLAGS="$CFLAGS $_base_cflags"
		CPPFLAGS="$CPPFLAGS $_base_cflags"

		AC_SUBST(CC)
		AC_SUBST(CFLAGS)
		AC_SUBST(CPPFLAGS)
	], [
		AC_MSG_ERROR([cannot detect CFLAGS...])
	])
])

dnl #
dnl # v5.3-rc4-54-g54b8ae66ae1a
dnl # kbuild: change *FLAGS_<basetarget>.o to take the path relative to $(obj)
dnl #
AC_DEFUN([AC_KERNEL_FLAGS_TAKE_PATH], [
	AS_IF([grep -qsm 1 "target-stem" ${LINUX}/scripts/Makefile.lib], [
		AC_DEFINE(HAVE_AMDKCL_FLAGS_TAKE_PATH, 1,
			[*FLAGS_<basetarget>.o support to take the path relative to $(obj)])
	])
])

dnl #
dnl # v4.20-rc2-10-ge07db28eea38
dnl # kbuild: fix single target build for external module
dnl #
AC_DEFUN([AC_KERNEL_SINGLE_TARGET], [
	AC_KERNEL_TMP_BUILD_DIR([
		AC_KERNEL_TRY_COMPILE_MODULE([], [], [], [
			SINGLE_TARGET_BUILD_MODVERDIR=.tmp_versions
			AS_IF([test ! -d $SINGLE_TARGET_BUILD_MODVERDIR], [
				SINGLE_TARGET_BUILD_NO_TMP_VERSIONS=1
			], [
				AC_MSG_WARN([compile single target fail expectedly])
			])
		])
		AC_KERNEL_SINGLE_TARGET_CFLAGS
		AC_KERNEL_FLAGS_TAKE_PATH
	])
])
