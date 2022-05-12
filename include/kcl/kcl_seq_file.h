/* SPDX-License-Identifier: GPL-2.0 */
#ifndef AMDKCL_SEQ_FILE_H
#define AMDKCL_SEQ_FILE_H

#include <linux/seq_file.h>

/* Copied from linux/seq_file.h */
#ifndef DEFINE_SHOW_ATTRIBUTE
#define DEFINE_SHOW_ATTRIBUTE(__name)                   \
static int __name ## _open(struct inode *inode, struct file *file)  \
{                                   \
	return single_open(file, __name ## _show, inode->i_private);    \
}                                   \
									\
static const struct file_operations __name ## _fops = {         \
	.owner      = THIS_MODULE,                  \
	.open       = __name ## _open,              \
	.read       = seq_read,                 \
	.llseek     = seq_lseek,                    \
	.release    = single_release,               \
}
#endif

#ifndef HAVE_SEQ_HEX_DUMP
void _kcl_seq_hex_dump(struct seq_file *m, const char *prefix_str, int prefix_type,
		  int rowsize, int groupsize, const void *buf, size_t len,
		  bool ascii);

static inline void seq_hex_dump(struct seq_file *m, const char *prefix_str, int prefix_type,
		int rowsize, int groupsize, const void *buf, size_t len,
		bool ascii)
{
	_kcl_seq_hex_dump(m, prefix_str, prefix_type, rowsize, groupsize, buf, len, ascii);
}
#endif
#endif
