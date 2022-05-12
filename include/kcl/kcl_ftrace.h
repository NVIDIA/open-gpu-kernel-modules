/* SPDX-License-Identifier: GPL-2.0 */
#ifndef AMDKCL_FTRACE_H
#define AMDKCL_FTRACE_H

#include <linux/trace_seq.h>
/* Copied from v3.19-rc1-6-g6ea22486ba46 include/trace/ftrace.h */
#if !defined(HAVE___PRINT_ARRAY)
extern const char * ftrace_print_array_seq(struct trace_seq *p, const void *buf, int count,
										   size_t el_size);
#define __print_array(array, count, el_size)				\
		({								\
				BUILD_BUG_ON(el_size != 1 && el_size != 2 &&		\
						     el_size != 4 && el_size != 8);		\
				ftrace_print_array_seq(p, array, count, el_size);	\
		})
#endif

#endif
