/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * ocfs2_buffer_head.h
 *
 * Buffer cache handling functions defined
 *
 * Copyright (C) 2002, 2004 Oracle.  All rights reserved.
 */

#ifndef OCFS2_BUFFER_HEAD_IO_H
#define OCFS2_BUFFER_HEAD_IO_H

#include <linux/buffer_head.h>

int ocfs2_write_block(struct ocfs2_super          *osb,
		      struct buffer_head  *bh,
		      struct ocfs2_caching_info   *ci);
int ocfs2_read_blocks_sync(struct ocfs2_super *osb, u64 block,
			   unsigned int nr, struct buffer_head *bhs[]);

/*
 * If not NULL, validate() will be called on a buffer that is freshly
 * read from disk.  It will not be called if the buffer was in cache.
 * Note that if validate() is being used for this buffer, it needs to
 * be set even for a READAHEAD call, as it marks the buffer for later
 * validation.
 */
int ocfs2_read_blocks(struct ocfs2_caching_info *ci, u64 block, int nr,
		      struct buffer_head *bhs[], int flags,
		      int (*validate)(struct super_block *sb,
				      struct buffer_head *bh));

int ocfs2_write_super_or_backup(struct ocfs2_super *osb,
				struct buffer_head *bh);

#define OCFS2_BH_IGNORE_CACHE      1
#define OCFS2_BH_READAHEAD         8

static inline int ocfs2_read_block(struct ocfs2_caching_info *ci, u64 off,
				   struct buffer_head **bh,
				   int (*validate)(struct super_block *sb,
						   struct buffer_head *bh))
{
	int status = 0;

	if (bh == NULL) {
		printk("ocfs2: bh == NULL\n");
		status = -EINVAL;
		goto bail;
	}

	status = ocfs2_read_blocks(ci, off, 1, bh, 0, validate);

bail:
	return status;
}

#endif /* OCFS2_BUFFER_HEAD_IO_H */
