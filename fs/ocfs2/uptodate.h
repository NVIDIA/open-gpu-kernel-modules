/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * uptodate.h
 *
 * Cluster uptodate tracking
 *
 * Copyright (C) 2002, 2004, 2005 Oracle.  All rights reserved.
 */

#ifndef OCFS2_UPTODATE_H
#define OCFS2_UPTODATE_H

/*
 * The caching code relies on locking provided by the user of
 * struct ocfs2_caching_info.  These operations connect that up.
 */
struct ocfs2_caching_operations {
	/*
	 * A u64 representing the owning structure.  Usually this
	 * is the block number (i_blkno or whatnot).  This is used so
	 * that caching log messages can identify the owning structure.
	 */
	u64	(*co_owner)(struct ocfs2_caching_info *ci);

	/* The superblock is needed during I/O. */
	struct super_block *(*co_get_super)(struct ocfs2_caching_info *ci);
	/*
	 * Lock and unlock the caching data.  These will not sleep, and
	 * should probably be spinlocks.
	 */
	void	(*co_cache_lock)(struct ocfs2_caching_info *ci);
	void	(*co_cache_unlock)(struct ocfs2_caching_info *ci);

	/*
	 * Lock and unlock for disk I/O.  These will sleep, and should
	 * be mutexes.
	 */
	void	(*co_io_lock)(struct ocfs2_caching_info *ci);
	void	(*co_io_unlock)(struct ocfs2_caching_info *ci);
};

int __init init_ocfs2_uptodate_cache(void);
void exit_ocfs2_uptodate_cache(void);

void ocfs2_metadata_cache_init(struct ocfs2_caching_info *ci,
			       const struct ocfs2_caching_operations *ops);
void ocfs2_metadata_cache_purge(struct ocfs2_caching_info *ci);
void ocfs2_metadata_cache_exit(struct ocfs2_caching_info *ci);

u64 ocfs2_metadata_cache_owner(struct ocfs2_caching_info *ci);
void ocfs2_metadata_cache_io_lock(struct ocfs2_caching_info *ci);
void ocfs2_metadata_cache_io_unlock(struct ocfs2_caching_info *ci);

int ocfs2_buffer_uptodate(struct ocfs2_caching_info *ci,
			  struct buffer_head *bh);
void ocfs2_set_buffer_uptodate(struct ocfs2_caching_info *ci,
			       struct buffer_head *bh);
void ocfs2_set_new_buffer_uptodate(struct ocfs2_caching_info *ci,
				   struct buffer_head *bh);
void ocfs2_remove_from_cache(struct ocfs2_caching_info *ci,
			     struct buffer_head *bh);
void ocfs2_remove_xattr_clusters_from_cache(struct ocfs2_caching_info *ci,
					    sector_t block,
					    u32 c_len);
int ocfs2_buffer_read_ahead(struct ocfs2_caching_info *ci,
			    struct buffer_head *bh);

#endif /* OCFS2_UPTODATE_H */
