/* SPDX-License-Identifier: GPL-2.0 */
#ifndef AMD_KCL_PERF_EVENT_BACKPORT_H
#define AMD_KCL_PERF_EVENT_BACKPORT_H
#include <kcl/kcl_perf_event.h>
#include <linux/perf_event.h>

#if !defined(HAVE_PERF_EVENT_UPDATE_USERPAGE)
#define perf_event_update_userpage _kcl_perf_event_update_userpage
#endif
#endif
