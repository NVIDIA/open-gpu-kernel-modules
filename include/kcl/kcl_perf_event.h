/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Performance events:
 *
 *    Copyright (C) 2008-2009, Thomas Gleixner <tglx@linutronix.de>
 *    Copyright (C) 2008-2011, Red Hat, Inc., Ingo Molnar
 *    Copyright (C) 2008-2011, Red Hat, Inc., Peter Zijlstra
 *
 * Data type definitions, declarations, prototypes.
 *
 *    Started by: Thomas Gleixner and Ingo Molnar
 *
 * For licencing details see kernel-base/COPYING
 */
#ifndef AMD_KCL_PERF_EVENT_H
#define AMD_KCL_PERF_EVENT_H
#include <linux/perf_event.h>

#if !defined(HAVE_PERF_EVENT_UPDATE_USERPAGE)
extern void (*_kcl_perf_event_update_userpage)(struct perf_event *event);
#endif
#endif
