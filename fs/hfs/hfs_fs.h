/*
 *  linux/fs/hfs/hfs_fs.h
 *
 * Copyright (C) 1995-1997  Paul H. Hargrove
 * (C) 2003 Ardis Technologies <roman@ardistech.com>
 * This file may be distributed under the terms of the GNU General Public License.
 */

#ifndef _LINUX_HFS_FS_H
#define _LINUX_HFS_FS_H

#ifdef pr_fmt
#undef pr_fmt
#endif

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/slab.h>
#include <linux/types.h>
#include <linux/mutex.h>
#include <linux/buffer_head.h>
#include <linux/fs.h>
#include <linux/workqueue.h>

#include <asm/byteorder.h>
#include <linux/uaccess.h>

#include "hfs.h"

#define DBG_BNODE_REFS	0x00000001
#define DBG_BNODE_MOD	0x00000002
#define DBG_CAT_MOD	0x00000004
#define DBG_INODE	0x00000008
#define DBG_SUPER	0x00000010
#define DBG_EXTENT	0x00000020
#define DBG_BITMAP	0x00000040

//#define DBG_MASK	(DBG_EXTENT|DBG_INODE|DBG_BNODE_MOD|DBG_CAT_MOD|DBG_BITMAP)
//#define DBG_MASK	(DBG_BNODE_MOD|DBG_CAT_MOD|DBG_INODE)
//#define DBG_MASK	(DBG_CAT_MOD|DBG_BNODE_REFS|DBG_INODE|DBG_EXTENT)
#define DBG_MASK	(0)

#define hfs_dbg(flg, fmt, ...)					\
do {								\
	if (DBG_##flg & DBG_MASK)				\
		printk(KERN_DEBUG pr_fmt(fmt), ##__VA_ARGS__);	\
} while (0)

#define hfs_dbg_cont(flg, fmt, ...)				\
do {								\
	if (DBG_##flg & DBG_MASK)				\
		pr_cont(fmt, ##__VA_ARGS__);			\
} while (0)


/*
 * struct hfs_inode_info
 *
 * The HFS-specific part of a Linux (struct inode)
 */
struct hfs_inode_info {
	atomic_t opencnt;

	unsigned int flags;

	/* to deal with localtime ugliness */
	int tz_secondswest;

	struct hfs_cat_key cat_key;

	struct list_head open_dir_list;
	spinlock_t open_dir_lock;
	struct inode *rsrc_inode;

	struct mutex extents_lock;

	u16 alloc_blocks, clump_blocks;
	sector_t fs_blocks;
	/* Allocation extents from catlog record or volume header */
	hfs_extent_rec first_extents;
	u16 first_blocks;
	hfs_extent_rec cached_extents;
	u16 cached_start, cached_blocks;

	loff_t phys_size;
	struct inode vfs_inode;
};

#define HFS_FLG_RSRC		0x0001
#define HFS_FLG_EXT_DIRTY	0x0002
#define HFS_FLG_EXT_NEW		0x0004

#define HFS_IS_RSRC(inode)	(HFS_I(inode)->flags & HFS_FLG_RSRC)

/*
 * struct hfs_sb_info
 *
 * The HFS-specific part of a Linux (struct super_block)
 */
struct hfs_sb_info {
	struct buffer_head *mdb_bh;		/* The hfs_buffer
						   holding the real
						   superblock (aka VIB
						   or MDB) */
	struct hfs_mdb *mdb;
	struct buffer_head *alt_mdb_bh;		/* The hfs_buffer holding
						   the alternate superblock */
	struct hfs_mdb *alt_mdb;
	__be32 *bitmap;				/* The page holding the
						   allocation bitmap */
	struct hfs_btree *ext_tree;			/* Information about
						   the extents b-tree */
	struct hfs_btree *cat_tree;			/* Information about
						   the catalog b-tree */
	u32 file_count;				/* The number of
						   regular files in
						   the filesystem */
	u32 folder_count;			/* The number of
						   directories in the
						   filesystem */
	u32 next_id;				/* The next available
						   file id number */
	u32 clumpablks;				/* The number of allocation
						   blocks to try to add when
						   extending a file */
	u32 fs_start;				/* The first 512-byte
						   block represented
						   in the bitmap */
	u32 part_start;
	u16 root_files;				/* The number of
						   regular
						   (non-directory)
						   files in the root
						   directory */
	u16 root_dirs;				/* The number of
						   directories in the
						   root directory */
	u16 fs_ablocks;				/* The number of
						   allocation blocks
						   in the filesystem */
	u16 free_ablocks;			/* the number of unused
						   allocation blocks
						   in the filesystem */
	u32 alloc_blksz;			/* The size of an
						   "allocation block" */
	int s_quiet;				/* Silent failure when
						   changing owner or mode? */
	__be32 s_type;				/* Type for new files */
	__be32 s_creator;			/* Creator for new files */
	umode_t s_file_umask;			/* The umask applied to the
						   permissions on all files */
	umode_t s_dir_umask;			/* The umask applied to the
						   permissions on all dirs */
	kuid_t s_uid;				/* The uid of all files */
	kgid_t s_gid;				/* The gid of all files */

	int session, part;
	struct nls_table *nls_io, *nls_disk;
	struct mutex bitmap_lock;
	unsigned long flags;
	u16 blockoffset;
	int fs_div;
	struct super_block *sb;
	int work_queued;		/* non-zero delayed work is queued */
	struct delayed_work mdb_work;	/* MDB flush delayed work */
	spinlock_t work_lock;		/* protects mdb_work and work_queued */
};

#define HFS_FLG_BITMAP_DIRTY	0
#define HFS_FLG_MDB_DIRTY	1
#define HFS_FLG_ALT_MDB_DIRTY	2

/* bitmap.c */
extern u32 hfs_vbm_search_free(struct super_block *, u32, u32 *);
extern int hfs_clear_vbm_bits(struct super_block *, u16, u16);

/* catalog.c */
extern int hfs_cat_keycmp(const btree_key *, const btree_key *);
struct hfs_find_data;
extern int hfs_cat_find_brec(struct super_block *, u32, struct hfs_find_data *);
extern int hfs_cat_create(u32, struct inode *, const struct qstr *, struct inode *);
extern int hfs_cat_delete(u32, struct inode *, const struct qstr *);
extern int hfs_cat_move(u32, struct inode *, const struct qstr *,
			struct inode *, const struct qstr *);
extern void hfs_cat_build_key(struct super_block *, btree_key *, u32, const struct qstr *);

/* dir.c */
extern const struct file_operations hfs_dir_operations;
extern const struct inode_operations hfs_dir_inode_operations;

/* extent.c */
extern int hfs_ext_keycmp(const btree_key *, const btree_key *);
extern int hfs_free_fork(struct super_block *, struct hfs_cat_file *, int);
extern int hfs_ext_write_extent(struct inode *);
extern int hfs_extend_file(struct inode *);
extern void hfs_file_truncate(struct inode *);

extern int hfs_get_block(struct inode *, sector_t, struct buffer_head *, int);

/* inode.c */
extern const struct address_space_operations hfs_aops;
extern const struct address_space_operations hfs_btree_aops;

extern struct inode *hfs_new_inode(struct inode *, const struct qstr *, umode_t);
extern void hfs_inode_write_fork(struct inode *, struct hfs_extent *, __be32 *, __be32 *);
extern int hfs_write_inode(struct inode *, struct writeback_control *);
extern int hfs_inode_setattr(struct user_namespace *, struct dentry *,
			     struct iattr *);
extern void hfs_inode_read_fork(struct inode *inode, struct hfs_extent *ext,
			__be32 log_size, __be32 phys_size, u32 clump_size);
extern struct inode *hfs_iget(struct super_block *, struct hfs_cat_key *, hfs_cat_rec *);
extern void hfs_evict_inode(struct inode *);
extern void hfs_delete_inode(struct inode *);

/* attr.c */
extern const struct xattr_handler *hfs_xattr_handlers[];

/* mdb.c */
extern int hfs_mdb_get(struct super_block *);
extern void hfs_mdb_commit(struct super_block *);
extern void hfs_mdb_close(struct super_block *);
extern void hfs_mdb_put(struct super_block *);

/* part_tbl.c */
extern int hfs_part_find(struct super_block *, sector_t *, sector_t *);

/* string.c */
extern const struct dentry_operations hfs_dentry_operations;

extern int hfs_hash_dentry(const struct dentry *, struct qstr *);
extern int hfs_strcmp(const unsigned char *, unsigned int,
		      const unsigned char *, unsigned int);
extern int hfs_compare_dentry(const struct dentry *dentry,
		unsigned int len, const char *str, const struct qstr *name);

/* trans.c */
extern void hfs_asc2mac(struct super_block *, struct hfs_name *, const struct qstr *);
extern int hfs_mac2asc(struct super_block *, char *, const struct hfs_name *);

/* super.c */
extern void hfs_mark_mdb_dirty(struct super_block *sb);

/*
 * There are two time systems.  Both are based on seconds since
 * a particular time/date.
 *	Unix:	signed little-endian since 00:00 GMT, Jan. 1, 1970
 *	mac:	unsigned big-endian since 00:00 GMT, Jan. 1, 1904
 *
 * HFS implementations are highly inconsistent, this one matches the
 * traditional behavior of 64-bit Linux, giving the most useful
 * time range between 1970 and 2106, by treating any on-disk timestamp
 * under HFS_UTC_OFFSET (Jan 1 1970) as a time between 2040 and 2106.
 */
#define HFS_UTC_OFFSET 2082844800U

static inline time64_t __hfs_m_to_utime(__be32 mt)
{
	time64_t ut = (u32)(be32_to_cpu(mt) - HFS_UTC_OFFSET);

	return ut + sys_tz.tz_minuteswest * 60;
}

static inline __be32 __hfs_u_to_mtime(time64_t ut)
{
	ut -= sys_tz.tz_minuteswest * 60;

	return cpu_to_be32(lower_32_bits(ut) + HFS_UTC_OFFSET);
}
#define HFS_I(inode)	(container_of(inode, struct hfs_inode_info, vfs_inode))
#define HFS_SB(sb)	((struct hfs_sb_info *)(sb)->s_fs_info)

#define hfs_m_to_utime(time)   (struct timespec64){ .tv_sec = __hfs_m_to_utime(time) }
#define hfs_u_to_mtime(time)   __hfs_u_to_mtime((time).tv_sec)
#define hfs_mtime()		__hfs_u_to_mtime(ktime_get_real_seconds())

static inline const char *hfs_mdb_name(struct super_block *sb)
{
	return sb->s_id;
}

static inline void hfs_bitmap_dirty(struct super_block *sb)
{
	set_bit(HFS_FLG_BITMAP_DIRTY, &HFS_SB(sb)->flags);
	hfs_mark_mdb_dirty(sb);
}

#define sb_bread512(sb, sec, data) ({			\
	struct buffer_head *__bh;			\
	sector_t __block;				\
	loff_t __start;					\
	int __offset;					\
							\
	__start = (loff_t)(sec) << HFS_SECTOR_SIZE_BITS;\
	__block = __start >> (sb)->s_blocksize_bits;	\
	__offset = __start & ((sb)->s_blocksize - 1);	\
	__bh = sb_bread((sb), __block);			\
	if (likely(__bh != NULL))			\
		data = (void *)(__bh->b_data + __offset);\
	else						\
		data = NULL;				\
	__bh;						\
})

#endif
