/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * V9FS cache definitions.
 *
 *  Copyright (C) 2009 by Abhishek Kulkarni <adkulkar@umail.iu.edu>
 */

#ifndef _9P_CACHE_H
#define _9P_CACHE_H
#ifdef CONFIG_9P_FSCACHE
#include <linux/fscache.h>
#include <linux/spinlock.h>

extern struct fscache_netfs v9fs_cache_netfs;
extern const struct fscache_cookie_def v9fs_cache_session_index_def;
extern const struct fscache_cookie_def v9fs_cache_inode_index_def;

extern void v9fs_cache_session_get_cookie(struct v9fs_session_info *v9ses);
extern void v9fs_cache_session_put_cookie(struct v9fs_session_info *v9ses);

extern void v9fs_cache_inode_get_cookie(struct inode *inode);
extern void v9fs_cache_inode_put_cookie(struct inode *inode);
extern void v9fs_cache_inode_flush_cookie(struct inode *inode);
extern void v9fs_cache_inode_set_cookie(struct inode *inode, struct file *filp);
extern void v9fs_cache_inode_reset_cookie(struct inode *inode);

extern int __v9fs_cache_register(void);
extern void __v9fs_cache_unregister(void);

extern int __v9fs_fscache_release_page(struct page *page, gfp_t gfp);
extern void __v9fs_fscache_invalidate_page(struct page *page);
extern int __v9fs_readpage_from_fscache(struct inode *inode,
					struct page *page);
extern int __v9fs_readpages_from_fscache(struct inode *inode,
					 struct address_space *mapping,
					 struct list_head *pages,
					 unsigned *nr_pages);
extern void __v9fs_readpage_to_fscache(struct inode *inode, struct page *page);
extern void __v9fs_fscache_wait_on_page_write(struct inode *inode,
					      struct page *page);

static inline int v9fs_fscache_release_page(struct page *page,
					    gfp_t gfp)
{
	return __v9fs_fscache_release_page(page, gfp);
}

static inline void v9fs_fscache_invalidate_page(struct page *page)
{
	__v9fs_fscache_invalidate_page(page);
}

static inline int v9fs_readpage_from_fscache(struct inode *inode,
					     struct page *page)
{
	return __v9fs_readpage_from_fscache(inode, page);
}

static inline int v9fs_readpages_from_fscache(struct inode *inode,
					      struct address_space *mapping,
					      struct list_head *pages,
					      unsigned *nr_pages)
{
	return __v9fs_readpages_from_fscache(inode, mapping, pages,
					     nr_pages);
}

static inline void v9fs_readpage_to_fscache(struct inode *inode,
					    struct page *page)
{
	if (PageFsCache(page))
		__v9fs_readpage_to_fscache(inode, page);
}

static inline void v9fs_uncache_page(struct inode *inode, struct page *page)
{
	struct v9fs_inode *v9inode = V9FS_I(inode);
	fscache_uncache_page(v9inode->fscache, page);
	BUG_ON(PageFsCache(page));
}

static inline void v9fs_fscache_wait_on_page_write(struct inode *inode,
						   struct page *page)
{
	return __v9fs_fscache_wait_on_page_write(inode, page);
}

#else /* CONFIG_9P_FSCACHE */

static inline void v9fs_cache_inode_get_cookie(struct inode *inode)
{
}

static inline void v9fs_cache_inode_put_cookie(struct inode *inode)
{
}

static inline void v9fs_cache_inode_set_cookie(struct inode *inode, struct file *file)
{
}

static inline int v9fs_fscache_release_page(struct page *page,
					    gfp_t gfp) {
	return 1;
}

static inline void v9fs_fscache_invalidate_page(struct page *page) {}

static inline int v9fs_readpage_from_fscache(struct inode *inode,
					     struct page *page)
{
	return -ENOBUFS;
}

static inline int v9fs_readpages_from_fscache(struct inode *inode,
					      struct address_space *mapping,
					      struct list_head *pages,
					      unsigned *nr_pages)
{
	return -ENOBUFS;
}

static inline void v9fs_readpage_to_fscache(struct inode *inode,
					    struct page *page)
{}

static inline void v9fs_uncache_page(struct inode *inode, struct page *page)
{}

static inline void v9fs_fscache_wait_on_page_write(struct inode *inode,
						   struct page *page)
{
	return;
}

#endif /* CONFIG_9P_FSCACHE */
#endif /* _9P_CACHE_H */
