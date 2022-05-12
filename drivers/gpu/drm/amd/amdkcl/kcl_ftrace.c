// SPDX-License-Identifier: GPL-2.0
/*
 * trace_output.c
 *
 * Copyright (C) 2008 Red Hat Inc, Steven Rostedt <srostedt@redhat.com>
 *
 */
#include <linux/trace_seq.h>

/* Copied from v3.19-rc1-6-g6ea22486ba46 kernel/trace/trace_output.c */
#if !defined(HAVE___PRINT_ARRAY)
const char *
ftrace_print_array_seq(struct trace_seq *p, const void *buf, int count,
					   size_t el_size)
{
	const char *ret = trace_seq_buffer_ptr(p);
	const char *prefix = "";
	void *ptr = (void *)buf;
	size_t buf_len = count * el_size;

	trace_seq_putc(p, '{');

	while (ptr < buf + buf_len) {
		switch (el_size) {
		case 1:
			trace_seq_printf(p, "%s0x%x", prefix,
							 *(u8 *)ptr);
			break;
		case 2:
			trace_seq_printf(p, "%s0x%x", prefix,
							 *(u16 *)ptr);
			break;
		case 4:
			trace_seq_printf(p, "%s0x%x", prefix,
							 *(u32 *)ptr);
			break;
		case 8:
			trace_seq_printf(p, "%s0x%llx", prefix,
							 *(u64 *)ptr);
			break;
		default:
			trace_seq_printf(p, "BAD SIZE:%zu 0x%x", el_size,
							 *(u8 *)ptr);
			el_size = 1;
		}
		prefix = ",";
		ptr += el_size;
	}

	trace_seq_putc(p, '}');
	trace_seq_putc(p, 0);

	return ret;
}
EXPORT_SYMBOL(ftrace_print_array_seq);
#endif
