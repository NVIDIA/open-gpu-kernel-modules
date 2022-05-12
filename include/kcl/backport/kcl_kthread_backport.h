/* SPDX-License-Identifier: GPL-2.0 */
#ifndef AMDKCL_KTHREAD_BACKPORT_H
#define AMDKCL_KTHREAD_BACKPORT_H
#include <linux/sched.h>
#include <linux/kthread.h>
#include <kcl/kcl_kthread.h>

#if !defined(HAVE___KTHREAD_SHOULD_PARK)
#define __kthread_should_park __kcl_kthread_should_park
#endif

#if !defined(HAVE_KTHREAD_PARK_XX)
#define kthread_parkme _kcl_kthread_parkme
#define kthread_unpark _kcl_kthread_unpark
#define kthread_park _kcl_kthread_park
#define kthread_should_park _kcl_kthread_should_park
#endif
#endif
