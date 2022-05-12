// SPDX-License-Identifier: GPL-2.0
/*
 * linux/fs/seq_file.c
 *
 * helper functions for making synthetic files from sequences of records.
 * initial implementation -- AV, Oct 2001.
 */
#include <kcl/kcl_seq_file.h>

/* Copied from fs/seq_file.c */
#ifndef HAVE_SEQ_HEX_DUMP
static void seq_set_overflow(struct seq_file *m)
{
	m->count = m->size;
}

/* A complete analogue of print_hex_dump() */
void _kcl_seq_hex_dump(struct seq_file *m, const char *prefix_str, int prefix_type,
		  int rowsize, int groupsize, const void *buf, size_t len,
		  bool ascii)
{
	const u8 *ptr = buf;
	int i, linelen, remaining = len;
	int ret;

	if (rowsize != 16 && rowsize != 32)
		rowsize = 16;

	for (i = 0; i < len && !seq_has_overflowed(m); i += rowsize) {
		linelen = min(remaining, rowsize);
		remaining -= rowsize;

		switch (prefix_type) {
		case DUMP_PREFIX_ADDRESS:
			seq_printf(m, "%s%p: ", prefix_str, ptr + i);
			break;
		case DUMP_PREFIX_OFFSET:
			seq_printf(m, "%s%.8x: ", prefix_str, i);
			break;
		default:
			seq_printf(m, "%s", prefix_str);
			break;
		}

		ret = hex_dump_to_buffer(ptr + i, linelen, rowsize, groupsize,
					m->buf + m->count, m->size - m->count,
					ascii);
		if (ret >= m->size - m->count) {
			seq_set_overflow(m);
		} else {
			m->count += ret;
			seq_putc(m, '\n');
		}
	}
}
EXPORT_SYMBOL(_kcl_seq_hex_dump);
#endif
