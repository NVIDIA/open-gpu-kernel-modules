// SPDX-License-Identifier: GPL-2.0
/*
 * Performance events core code:
 *
 *  Copyright (C) 2008 Thomas Gleixner <tglx@linutronix.de>
 *  Copyright (C) 2008-2011 Red Hat, Inc., Ingo Molnar
 *  Copyright (C) 2008-2011 Red Hat, Inc., Peter Zijlstra
 *  Copyright  ©  2009 Paul Mackerras, IBM Corp. <paulus@au1.ibm.com>
 */
#include <kcl/kcl_perf_event.h>

#if !defined(HAVE_PERF_EVENT_UPDATE_USERPAGE)
void (*_kcl_perf_event_update_userpage)(struct perf_event *event);
EXPORT_SYMBOL(_kcl_perf_event_update_userpage);
#endif

void amdkcl_perf_event_init(void)
{
#if !defined(HAVE_PERF_EVENT_UPDATE_USERPAGE)
	_kcl_perf_event_update_userpage = amdkcl_fp_setup("perf_event_update_userpage", NULL);
#endif
}

