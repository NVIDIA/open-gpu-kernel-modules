/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _PERF_UI_SLANG_H_
#define _PERF_UI_SLANG_H_ 1
/*
 * slang versions <= 2.0.6 have a "#if HAVE_LONG_LONG" that breaks
 * the build if it isn't defined. Use the equivalent one that glibc
 * has on features.h.
 */
#include <features.h>
#ifndef HAVE_LONG_LONG
#define HAVE_LONG_LONG __GLIBC_HAVE_LONG_LONG
#endif

#ifdef HAVE_SLANG_INCLUDE_SUBDIR
#include <slang/slang.h>
#else
#include <slang.h>
#endif

#if SLANG_VERSION < 20104
#define slsmg_printf(msg, args...) \
	SLsmg_printf((char *)(msg), ##args)
#define slsmg_vprintf(msg, vargs) \
	SLsmg_vprintf((char *)(msg), vargs)
#define slsmg_write_nstring(msg, len) \
	SLsmg_write_nstring((char *)(msg), len)
#define sltt_set_color(obj, name, fg, bg) \
	SLtt_set_color(obj,(char *)(name), (char *)(fg), (char *)(bg))
#else
#define slsmg_printf SLsmg_printf
#define slsmg_vprintf SLsmg_vprintf
#define slsmg_write_nstring SLsmg_write_nstring
#define sltt_set_color SLtt_set_color
#endif

#define SL_KEY_UNTAB 0x1000

#endif /* _PERF_UI_SLANG_H_ */
