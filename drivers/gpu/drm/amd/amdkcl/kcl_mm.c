// SPDX-License-Identifier: GPL-2.0-only
/*
 *  linux/kernel/fork.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */
#include <linux/sched.h>

#ifndef HAVE_MM_ACCESS
struct mm_struct* (*_kcl_mm_access)(struct task_struct *task, unsigned int mode);
EXPORT_SYMBOL(_kcl_mm_access);

static struct mm_struct * __kcl_mm_access_stub(struct task_struct *task, unsigned int mode)
{
	pr_warn_once("This kernel version not support API: mm_access !\n");
	return NULL;
}
#endif

#ifndef HAVE_MMPUT_ASYNC
void (*_kcl_mmput_async)(struct mm_struct *mm);
EXPORT_SYMBOL(_kcl_mmput_async);

void __kcl_mmput_async(struct mm_struct *mm)
{
	pr_warn_once("This kernel version not support API: mmput_async !\n");
}
#endif


#ifndef HAVE_GET_MM_EXE_FILE
struct file *(*_kcl_get_mm_exe_file)(struct mm_struct *mm);
EXPORT_SYMBOL(_kcl_get_mm_exe_file);
#endif

void amdkcl_mm_init(void)
{
#ifndef HAVE_MM_ACCESS
	_kcl_mm_access = amdkcl_fp_setup("mm_access", __kcl_mm_access_stub);
#endif

#ifndef HAVE_MMPUT_ASYNC
	_kcl_mmput_async = amdkcl_fp_setup("mmput_async", __kcl_mmput_async);
#endif

#ifndef HAVE_GET_MM_EXE_FILE
       _kcl_get_mm_exe_file = amdkcl_fp_setup("get_mm_exe_file", NULL);
#endif
}
