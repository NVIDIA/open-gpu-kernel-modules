/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *   Copyright (C) International Business Machines Corp., 2000-2002
 *   Portions Copyright (C) Christoph Hellwig, 2001-2002
 */
#ifndef	_H_JFS_METAPAGE
#define _H_JFS_METAPAGE

#include <linux/pagemap.h>

struct metapage {
	/* Common logsyncblk prefix (see jfs_logmgr.h) */
	u16 xflag;
	u16 unused;
	lid_t lid;
	int lsn;
	struct list_head synclist;
	/* End of logsyncblk prefix */

	unsigned long flag;	/* See Below */
	unsigned long count;	/* Reference count */
	void *data;		/* Data pointer */
	sector_t index;		/* block address of page */
	wait_queue_head_t wait;

	/* implementation */
	struct page *page;
	struct super_block *sb;
	unsigned int logical_size;

	/* Journal management */
	int clsn;
	int nohomeok;
	struct jfs_log *log;
};

/* metapage flag */
#define META_locked	0
#define META_dirty	2
#define META_sync	3
#define META_discard	4
#define META_forcewrite	5
#define META_io		6

#define mark_metapage_dirty(mp) set_bit(META_dirty, &(mp)->flag)

/* function prototypes */
extern int metapage_init(void);
extern void metapage_exit(void);
extern struct metapage *__get_metapage(struct inode *inode,
				  unsigned long lblock, unsigned int size,
				  int absolute, unsigned long new);

#define read_metapage(inode, lblock, size, absolute)\
	 __get_metapage(inode, lblock, size, absolute, false)

#define get_metapage(inode, lblock, size, absolute)\
	 __get_metapage(inode, lblock, size, absolute, true)

extern void release_metapage(struct metapage *);
extern void grab_metapage(struct metapage *);
extern void force_metapage(struct metapage *);

/*
 * hold_metapage and put_metapage are used in conjunction.  The page lock
 * is not dropped between the two, so no other threads can get or release
 * the metapage
 */
extern void hold_metapage(struct metapage *);
extern void put_metapage(struct metapage *);

static inline void write_metapage(struct metapage *mp)
{
	set_bit(META_dirty, &mp->flag);
	release_metapage(mp);
}

static inline void flush_metapage(struct metapage *mp)
{
	set_bit(META_sync, &mp->flag);
	write_metapage(mp);
}

static inline void discard_metapage(struct metapage *mp)
{
	clear_bit(META_dirty, &mp->flag);
	set_bit(META_discard, &mp->flag);
	release_metapage(mp);
}

static inline void metapage_nohomeok(struct metapage *mp)
{
	struct page *page = mp->page;
	lock_page(page);
	if (!mp->nohomeok++) {
		mark_metapage_dirty(mp);
		get_page(page);
		wait_on_page_writeback(page);
	}
	unlock_page(page);
}

/*
 * This serializes access to mp->lsn when metapages are added to logsynclist
 * without setting nohomeok.  i.e. updating imap & dmap
 */
static inline void metapage_wait_for_io(struct metapage *mp)
{
	if (test_bit(META_io, &mp->flag))
		wait_on_page_writeback(mp->page);
}

/*
 * This is called when already holding the metapage
 */
static inline void _metapage_homeok(struct metapage *mp)
{
	if (!--mp->nohomeok)
		put_page(mp->page);
}

static inline void metapage_homeok(struct metapage *mp)
{
	hold_metapage(mp);
	_metapage_homeok(mp);
	put_metapage(mp);
}

extern const struct address_space_operations jfs_metapage_aops;

/*
 * This routines invalidate all pages for an extent.
 */
extern void __invalidate_metapages(struct inode *, s64, int);
#define invalidate_pxd_metapages(ip, pxd) \
	__invalidate_metapages((ip), addressPXD(&(pxd)), lengthPXD(&(pxd)))
#define invalidate_dxd_metapages(ip, dxd) \
	__invalidate_metapages((ip), addressDXD(&(dxd)), lengthDXD(&(dxd)))
#define invalidate_xad_metapages(ip, xad) \
	__invalidate_metapages((ip), addressXAD(&(xad)), lengthXAD(&(xad)))

#endif				/* _H_JFS_METAPAGE */
