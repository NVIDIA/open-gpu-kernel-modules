// SPDX-License-Identifier: GPL-2.0-only
/* Kernel thread helper functions.
 *   Copyright (C) 2004 IBM Corporation, Rusty Russell.
 *   Copyright (C) 2009 Red Hat, Inc.
 *
 * Creation is done via kthreadd, so that we get a clean environment
 * even if we're invoked from userspace (think modprobe, hotplug cpu,
 * etc.).
 */

/*
* FIXME: implement below API when kernel version < 4.2
*/
#include <linux/printk.h>
#include <linux/version.h>
#include <kcl/kcl_kthread.h>

#if !defined(HAVE___KTHREAD_SHOULD_PARK)
bool __kcl_kthread_should_park(struct task_struct *k)
{
	pr_warn_once("This kernel version not support API: __kthread_should_park!\n");
	return false;
}
EXPORT_SYMBOL(__kcl_kthread_should_park);
#endif

#if !defined(HAVE_KTHREAD_PARK_XX)
bool (*_kcl_kthread_should_park)(void);
EXPORT_SYMBOL(_kcl_kthread_should_park);

void (*_kcl_kthread_parkme)(void);
EXPORT_SYMBOL(_kcl_kthread_parkme);

void (*_kcl_kthread_unpark)(struct task_struct *k);
EXPORT_SYMBOL(_kcl_kthread_unpark);

int (*_kcl_kthread_park)(struct task_struct *k);
EXPORT_SYMBOL(_kcl_kthread_park);

static bool _kcl_kthread_should_park_stub(void)
{
	pr_warn_once("This kernel version not support API: kthread_should_park!\n");
	return false;
}

static void _kcl_kthread_parkme_stub(void)
{
	pr_warn_once("This kernel version not support API: kthread_parkme!\n");
}

static void _kcl_kthread_unpark_stub(struct task_struct *k)
{
	pr_warn_once("This kernel version not support API: kthread_unpark!\n");
}

static int _kcl_kthread_park_stub(struct task_struct *k)
{
	pr_warn_once("This kernel version not support API: kthread_park!\n");
	return 0;
}
#endif

void amdkcl_kthread_init(void)
{
#if !defined(HAVE_KTHREAD_PARK_XX)
	_kcl_kthread_should_park = amdkcl_fp_setup("kthread_should_park",
						_kcl_kthread_should_park_stub);
	_kcl_kthread_parkme = amdkcl_fp_setup("kthread_parkme",
						_kcl_kthread_parkme_stub);
	_kcl_kthread_unpark = amdkcl_fp_setup("kthread_unpark",
						_kcl_kthread_unpark_stub);
	_kcl_kthread_park = amdkcl_fp_setup("kthread_park",
						_kcl_kthread_park_stub);
#endif
}
