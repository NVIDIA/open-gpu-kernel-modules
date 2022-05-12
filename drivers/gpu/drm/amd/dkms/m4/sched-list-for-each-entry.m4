dnl #
dnl # 4.13 API change
dnl # commit ac6424b981bce1c4bc55675c6ce11bfe1bbfa64f
dnl # Renamed wait_queue_head::task_list -> wait_queue_head::head
dnl # Renamed wait_queue_entry::task_list -> wait_queue_entry::entry
dnl #
AC_DEFUN([AC_AMDGPU_LIST_FOR_EACH_ENTRY], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/wait.h>
		], [
			wait_queue_entry_t *wq_entry = NULL;
			wait_queue_head_t *wq_head = NULL;

			__add_wait_queue(wq_head, wq_entry);
		], [
			AC_DEFINE(HAVE_WAIT_QUEUE_ENTRY, 1,
				[wait_queue_entry_t exists])
		])
	])
])
