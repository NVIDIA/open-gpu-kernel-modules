/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (C) 2012-2013 Samsung Electronics Co., Ltd.
 */

#ifndef _EXFAT_FS_H
#define _EXFAT_FS_H

#include <linux/fs.h>
#include <linux/ratelimit.h>
#include <linux/nls.h>

#define EXFAT_SUPER_MAGIC       0x2011BAB0UL
#define EXFAT_ROOT_INO		1

#define EXFAT_CLUSTERS_UNTRACKED (~0u)

/*
 * exfat error flags
 */
enum exfat_error_mode {
	EXFAT_ERRORS_CONT,	/* ignore error and continue */
	EXFAT_ERRORS_PANIC,	/* panic on error */
	EXFAT_ERRORS_RO,	/* remount r/o on error */
};

/*
 * exfat nls lossy flag
 */
enum {
	NLS_NAME_NO_LOSSY,	/* no lossy */
	NLS_NAME_LOSSY,		/* just detected incorrect filename(s) */
	NLS_NAME_OVERLEN,	/* the length is over than its limit */
};

#define EXFAT_HASH_BITS		8
#define EXFAT_HASH_SIZE		(1UL << EXFAT_HASH_BITS)

/*
 * Type Definitions
 */
#define ES_2_ENTRIES		2
#define ES_ALL_ENTRIES		0

#define DIR_DELETED		0xFFFF0321

/* type values */
#define TYPE_UNUSED		0x0000
#define TYPE_DELETED		0x0001
#define TYPE_INVALID		0x0002
#define TYPE_CRITICAL_PRI	0x0100
#define TYPE_BITMAP		0x0101
#define TYPE_UPCASE		0x0102
#define TYPE_VOLUME		0x0103
#define TYPE_DIR		0x0104
#define TYPE_FILE		0x011F
#define TYPE_CRITICAL_SEC	0x0200
#define TYPE_STREAM		0x0201
#define TYPE_EXTEND		0x0202
#define TYPE_ACL		0x0203
#define TYPE_BENIGN_PRI		0x0400
#define TYPE_GUID		0x0401
#define TYPE_PADDING		0x0402
#define TYPE_ACLTAB		0x0403
#define TYPE_BENIGN_SEC		0x0800
#define TYPE_ALL		0x0FFF

#define MAX_CHARSET_SIZE	6 /* max size of multi-byte character */
#define MAX_NAME_LENGTH		255 /* max len of file name excluding NULL */
#define MAX_VFSNAME_BUF_SIZE	((MAX_NAME_LENGTH + 1) * MAX_CHARSET_SIZE)

/* Enough size to hold 256 dentry (even 512 Byte sector) */
#define DIR_CACHE_SIZE		(256*sizeof(struct exfat_dentry)/512+1)

#define EXFAT_HINT_NONE		-1
#define EXFAT_MIN_SUBDIR	2

/*
 * helpers for cluster size to byte conversion.
 */
#define EXFAT_CLU_TO_B(b, sbi)		((b) << (sbi)->cluster_size_bits)
#define EXFAT_B_TO_CLU(b, sbi)		((b) >> (sbi)->cluster_size_bits)
#define EXFAT_B_TO_CLU_ROUND_UP(b, sbi)	\
	(((b - 1) >> (sbi)->cluster_size_bits) + 1)
#define EXFAT_CLU_OFFSET(off, sbi)	((off) & ((sbi)->cluster_size - 1))

/*
 * helpers for block size to byte conversion.
 */
#define EXFAT_BLK_TO_B(b, sb)		((b) << (sb)->s_blocksize_bits)
#define EXFAT_B_TO_BLK(b, sb)		((b) >> (sb)->s_blocksize_bits)
#define EXFAT_B_TO_BLK_ROUND_UP(b, sb)	\
	(((b - 1) >> (sb)->s_blocksize_bits) + 1)
#define EXFAT_BLK_OFFSET(off, sb)	((off) & ((sb)->s_blocksize - 1))

/*
 * helpers for block size to dentry size conversion.
 */
#define EXFAT_B_TO_DEN_IDX(b, sbi)	\
	((b) << ((sbi)->cluster_size_bits - DENTRY_SIZE_BITS))
#define EXFAT_B_TO_DEN(b)		((b) >> DENTRY_SIZE_BITS)
#define EXFAT_DEN_TO_B(b)		((b) << DENTRY_SIZE_BITS)

/*
 * helpers for fat entry.
 */
#define FAT_ENT_SIZE (4)
#define FAT_ENT_SIZE_BITS (2)
#define FAT_ENT_OFFSET_SECTOR(sb, loc) (EXFAT_SB(sb)->FAT1_start_sector + \
	(((u64)loc << FAT_ENT_SIZE_BITS) >> sb->s_blocksize_bits))
#define FAT_ENT_OFFSET_BYTE_IN_SECTOR(sb, loc)	\
	((loc << FAT_ENT_SIZE_BITS) & (sb->s_blocksize - 1))

/*
 * helpers for bitmap.
 */
#define CLUSTER_TO_BITMAP_ENT(clu) ((clu) - EXFAT_RESERVED_CLUSTERS)
#define BITMAP_ENT_TO_CLUSTER(ent) ((ent) + EXFAT_RESERVED_CLUSTERS)
#define BITS_PER_SECTOR(sb) ((sb)->s_blocksize * BITS_PER_BYTE)
#define BITS_PER_SECTOR_MASK(sb) (BITS_PER_SECTOR(sb) - 1)
#define BITMAP_OFFSET_SECTOR_INDEX(sb, ent) \
	((ent / BITS_PER_BYTE) >> (sb)->s_blocksize_bits)
#define BITMAP_OFFSET_BIT_IN_SECTOR(sb, ent) (ent & BITS_PER_SECTOR_MASK(sb))
#define BITMAP_OFFSET_BYTE_IN_SECTOR(sb, ent) \
	((ent / BITS_PER_BYTE) & ((sb)->s_blocksize - 1))
#define BITS_PER_BYTE_MASK	0x7
#define IGNORED_BITS_REMAINED(clu, clu_base) ((1 << ((clu) - (clu_base))) - 1)

struct exfat_dentry_namebuf {
	char *lfn;
	int lfnbuf_len; /* usually MAX_UNINAME_BUF_SIZE */
};

/* unicode name structure */
struct exfat_uni_name {
	/* +3 for null and for converting */
	unsigned short name[MAX_NAME_LENGTH + 3];
	u16 name_hash;
	unsigned char name_len;
};

/* directory structure */
struct exfat_chain {
	unsigned int dir;
	unsigned int size;
	unsigned char flags;
};

/* first empty entry hint information */
struct exfat_hint_femp {
	/* entry index of a directory */
	int eidx;
	/* count of continuous empty entry */
	int count;
	/* the cluster that first empty slot exists in */
	struct exfat_chain cur;
};

/* hint structure */
struct exfat_hint {
	unsigned int clu;
	union {
		unsigned int off; /* cluster offset */
		int eidx; /* entry index */
	};
};

struct exfat_entry_set_cache {
	struct super_block *sb;
	bool modified;
	unsigned int start_off;
	int num_bh;
	struct buffer_head *bh[DIR_CACHE_SIZE];
	unsigned int num_entries;
};

struct exfat_dir_entry {
	struct exfat_chain dir;
	int entry;
	unsigned int type;
	unsigned int start_clu;
	unsigned char flags;
	unsigned short attr;
	loff_t size;
	unsigned int num_subdirs;
	struct timespec64 atime;
	struct timespec64 mtime;
	struct timespec64 crtime;
	struct exfat_dentry_namebuf namebuf;
};

/*
 * exfat mount in-memory data
 */
struct exfat_mount_options {
	kuid_t fs_uid;
	kgid_t fs_gid;
	unsigned short fs_fmask;
	unsigned short fs_dmask;
	/* permission for setting the [am]time */
	unsigned short allow_utime;
	/* charset for filename input/display */
	char *iocharset;
	/* on error: continue, panic, remount-ro */
	enum exfat_error_mode errors;
	unsigned utf8:1, /* Use of UTF-8 character set */
		 discard:1; /* Issue discard requests on deletions */
	int time_offset; /* Offset of timestamps from UTC (in minutes) */
};

/*
 * EXFAT file system superblock in-memory data
 */
struct exfat_sb_info {
	unsigned long long num_sectors; /* num of sectors in volume */
	unsigned int num_clusters; /* num of clusters in volume */
	unsigned int cluster_size; /* cluster size in bytes */
	unsigned int cluster_size_bits;
	unsigned int sect_per_clus; /* cluster size in sectors */
	unsigned int sect_per_clus_bits;
	unsigned long long FAT1_start_sector; /* FAT1 start sector */
	unsigned long long FAT2_start_sector; /* FAT2 start sector */
	unsigned long long data_start_sector; /* data area start sector */
	unsigned int num_FAT_sectors; /* num of FAT sectors */
	unsigned int root_dir; /* root dir cluster */
	unsigned int dentries_per_clu; /* num of dentries per cluster */
	unsigned int vol_flags; /* volume flags */
	unsigned int vol_flags_persistent; /* volume flags to retain */
	struct buffer_head *boot_bh; /* buffer_head of BOOT sector */

	unsigned int map_clu; /* allocation bitmap start cluster */
	unsigned int map_sectors; /* num of allocation bitmap sectors */
	struct buffer_head **vol_amap; /* allocation bitmap */

	unsigned short *vol_utbl; /* upcase table */

	unsigned int clu_srch_ptr; /* cluster search pointer */
	unsigned int used_clusters; /* number of used clusters */

	struct mutex s_lock; /* superblock lock */
	struct mutex bitmap_lock; /* bitmap lock */
	struct exfat_mount_options options;
	struct nls_table *nls_io; /* Charset used for input and display */
	struct ratelimit_state ratelimit;

	spinlock_t inode_hash_lock;
	struct hlist_head inode_hashtable[EXFAT_HASH_SIZE];

	struct rcu_head rcu;
};

#define EXFAT_CACHE_VALID	0

/*
 * EXFAT file system inode in-memory data
 */
struct exfat_inode_info {
	struct exfat_chain dir;
	int entry;
	unsigned int type;
	unsigned short attr;
	unsigned int start_clu;
	unsigned char flags;
	/*
	 * the copy of low 32bit of i_version to check
	 * the validation of hint_stat.
	 */
	unsigned int version;

	/* hint for cluster last accessed */
	struct exfat_hint hint_bmap;
	/* hint for entry index we try to lookup next time */
	struct exfat_hint hint_stat;
	/* hint for first empty entry */
	struct exfat_hint_femp hint_femp;

	spinlock_t cache_lru_lock;
	struct list_head cache_lru;
	int nr_caches;
	/* for avoiding the race between alloc and free */
	unsigned int cache_valid_id;

	/*
	 * NOTE: i_size_ondisk is 64bits, so must hold ->inode_lock to access.
	 * physically allocated size.
	 */
	loff_t i_size_ondisk;
	/* block-aligned i_size (used in cont_write_begin) */
	loff_t i_size_aligned;
	/* on-disk position of directory entry or 0 */
	loff_t i_pos;
	/* hash by i_location */
	struct hlist_node i_hash_fat;
	/* protect bmap against truncate */
	struct rw_semaphore truncate_lock;
	struct inode vfs_inode;
	/* File creation time */
	struct timespec64 i_crtime;
};

static inline struct exfat_sb_info *EXFAT_SB(struct super_block *sb)
{
	return sb->s_fs_info;
}

static inline struct exfat_inode_info *EXFAT_I(struct inode *inode)
{
	return container_of(inode, struct exfat_inode_info, vfs_inode);
}

/*
 * If ->i_mode can't hold 0222 (i.e. ATTR_RO), we use ->i_attrs to
 * save ATTR_RO instead of ->i_mode.
 *
 * If it's directory and !sbi->options.rodir, ATTR_RO isn't read-only
 * bit, it's just used as flag for app.
 */
static inline int exfat_mode_can_hold_ro(struct inode *inode)
{
	struct exfat_sb_info *sbi = EXFAT_SB(inode->i_sb);

	if (S_ISDIR(inode->i_mode))
		return 0;

	if ((~sbi->options.fs_fmask) & 0222)
		return 1;
	return 0;
}

/* Convert attribute bits and a mask to the UNIX mode. */
static inline mode_t exfat_make_mode(struct exfat_sb_info *sbi,
		unsigned short attr, mode_t mode)
{
	if ((attr & ATTR_READONLY) && !(attr & ATTR_SUBDIR))
		mode &= ~0222;

	if (attr & ATTR_SUBDIR)
		return (mode & ~sbi->options.fs_dmask) | S_IFDIR;

	return (mode & ~sbi->options.fs_fmask) | S_IFREG;
}

/* Return the FAT attribute byte for this inode */
static inline unsigned short exfat_make_attr(struct inode *inode)
{
	unsigned short attr = EXFAT_I(inode)->attr;

	if (S_ISDIR(inode->i_mode))
		attr |= ATTR_SUBDIR;
	if (exfat_mode_can_hold_ro(inode) && !(inode->i_mode & 0222))
		attr |= ATTR_READONLY;
	return attr;
}

static inline void exfat_save_attr(struct inode *inode, unsigned short attr)
{
	if (exfat_mode_can_hold_ro(inode))
		EXFAT_I(inode)->attr = attr & (ATTR_RWMASK | ATTR_READONLY);
	else
		EXFAT_I(inode)->attr = attr & ATTR_RWMASK;
}

static inline bool exfat_is_last_sector_in_cluster(struct exfat_sb_info *sbi,
		sector_t sec)
{
	return ((sec - sbi->data_start_sector + 1) &
		((1 << sbi->sect_per_clus_bits) - 1)) == 0;
}

static inline sector_t exfat_cluster_to_sector(struct exfat_sb_info *sbi,
		unsigned int clus)
{
	return ((sector_t)(clus - EXFAT_RESERVED_CLUSTERS) << sbi->sect_per_clus_bits) +
		sbi->data_start_sector;
}

static inline int exfat_sector_to_cluster(struct exfat_sb_info *sbi,
		sector_t sec)
{
	return ((sec - sbi->data_start_sector) >> sbi->sect_per_clus_bits) +
		EXFAT_RESERVED_CLUSTERS;
}

/* super.c */
int exfat_set_volume_dirty(struct super_block *sb);
int exfat_clear_volume_dirty(struct super_block *sb);

/* fatent.c */
#define exfat_get_next_cluster(sb, pclu) exfat_ent_get(sb, *(pclu), pclu)

int exfat_alloc_cluster(struct inode *inode, unsigned int num_alloc,
		struct exfat_chain *p_chain, bool sync_bmap);
int exfat_free_cluster(struct inode *inode, struct exfat_chain *p_chain);
int exfat_ent_get(struct super_block *sb, unsigned int loc,
		unsigned int *content);
int exfat_ent_set(struct super_block *sb, unsigned int loc,
		unsigned int content);
int exfat_count_ext_entries(struct super_block *sb, struct exfat_chain *p_dir,
		int entry, struct exfat_dentry *p_entry);
int exfat_chain_cont_cluster(struct super_block *sb, unsigned int chain,
		unsigned int len);
int exfat_zeroed_cluster(struct inode *dir, unsigned int clu);
int exfat_find_last_cluster(struct super_block *sb, struct exfat_chain *p_chain,
		unsigned int *ret_clu);
int exfat_count_num_clusters(struct super_block *sb,
		struct exfat_chain *p_chain, unsigned int *ret_count);

/* balloc.c */
int exfat_load_bitmap(struct super_block *sb);
void exfat_free_bitmap(struct exfat_sb_info *sbi);
int exfat_set_bitmap(struct inode *inode, unsigned int clu, bool sync);
void exfat_clear_bitmap(struct inode *inode, unsigned int clu, bool sync);
unsigned int exfat_find_free_bitmap(struct super_block *sb, unsigned int clu);
int exfat_count_used_clusters(struct super_block *sb, unsigned int *ret_count);
int exfat_trim_fs(struct inode *inode, struct fstrim_range *range);

/* file.c */
extern const struct file_operations exfat_file_operations;
int __exfat_truncate(struct inode *inode, loff_t new_size);
void exfat_truncate(struct inode *inode, loff_t size);
int exfat_setattr(struct user_namespace *mnt_userns, struct dentry *dentry,
		  struct iattr *attr);
int exfat_getattr(struct user_namespace *mnt_userns, const struct path *path,
		  struct kstat *stat, unsigned int request_mask,
		  unsigned int query_flags);
int exfat_file_fsync(struct file *file, loff_t start, loff_t end, int datasync);
long exfat_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
long exfat_compat_ioctl(struct file *filp, unsigned int cmd,
				unsigned long arg);

/* namei.c */
extern const struct dentry_operations exfat_dentry_ops;
extern const struct dentry_operations exfat_utf8_dentry_ops;

/* cache.c */
int exfat_cache_init(void);
void exfat_cache_shutdown(void);
void exfat_cache_inval_inode(struct inode *inode);
int exfat_get_cluster(struct inode *inode, unsigned int cluster,
		unsigned int *fclus, unsigned int *dclus,
		unsigned int *last_dclus, int allow_eof);

/* dir.c */
extern const struct inode_operations exfat_dir_inode_operations;
extern const struct file_operations exfat_dir_operations;
unsigned int exfat_get_entry_type(struct exfat_dentry *p_entry);
int exfat_init_dir_entry(struct inode *inode, struct exfat_chain *p_dir,
		int entry, unsigned int type, unsigned int start_clu,
		unsigned long long size);
int exfat_init_ext_entry(struct inode *inode, struct exfat_chain *p_dir,
		int entry, int num_entries, struct exfat_uni_name *p_uniname);
int exfat_remove_entries(struct inode *inode, struct exfat_chain *p_dir,
		int entry, int order, int num_entries);
int exfat_update_dir_chksum(struct inode *inode, struct exfat_chain *p_dir,
		int entry);
void exfat_update_dir_chksum_with_entry_set(struct exfat_entry_set_cache *es);
int exfat_calc_num_entries(struct exfat_uni_name *p_uniname);
int exfat_find_dir_entry(struct super_block *sb, struct exfat_inode_info *ei,
		struct exfat_chain *p_dir, struct exfat_uni_name *p_uniname,
		int num_entries, unsigned int type, struct exfat_hint *hint_opt);
int exfat_alloc_new_dir(struct inode *inode, struct exfat_chain *clu);
int exfat_find_location(struct super_block *sb, struct exfat_chain *p_dir,
		int entry, sector_t *sector, int *offset);
struct exfat_dentry *exfat_get_dentry(struct super_block *sb,
		struct exfat_chain *p_dir, int entry, struct buffer_head **bh,
		sector_t *sector);
struct exfat_dentry *exfat_get_dentry_cached(struct exfat_entry_set_cache *es,
		int num);
struct exfat_entry_set_cache *exfat_get_dentry_set(struct super_block *sb,
		struct exfat_chain *p_dir, int entry, unsigned int type);
int exfat_free_dentry_set(struct exfat_entry_set_cache *es, int sync);
int exfat_count_dir_entries(struct super_block *sb, struct exfat_chain *p_dir);

/* inode.c */
extern const struct inode_operations exfat_file_inode_operations;
void exfat_sync_inode(struct inode *inode);
struct inode *exfat_build_inode(struct super_block *sb,
		struct exfat_dir_entry *info, loff_t i_pos);
void exfat_hash_inode(struct inode *inode, loff_t i_pos);
void exfat_unhash_inode(struct inode *inode);
struct inode *exfat_iget(struct super_block *sb, loff_t i_pos);
int exfat_write_inode(struct inode *inode, struct writeback_control *wbc);
void exfat_evict_inode(struct inode *inode);
int exfat_block_truncate_page(struct inode *inode, loff_t from);

/* exfat/nls.c */
unsigned short exfat_toupper(struct super_block *sb, unsigned short a);
int exfat_uniname_ncmp(struct super_block *sb, unsigned short *a,
		unsigned short *b, unsigned int len);
int exfat_utf16_to_nls(struct super_block *sb,
		struct exfat_uni_name *uniname, unsigned char *p_cstring,
		int len);
int exfat_nls_to_utf16(struct super_block *sb,
		const unsigned char *p_cstring, const int len,
		struct exfat_uni_name *uniname, int *p_lossy);
int exfat_create_upcase_table(struct super_block *sb);
void exfat_free_upcase_table(struct exfat_sb_info *sbi);

/* exfat/misc.c */
void __exfat_fs_error(struct super_block *sb, int report, const char *fmt, ...)
		__printf(3, 4) __cold;
#define exfat_fs_error(sb, fmt, args...)          \
		__exfat_fs_error(sb, 1, fmt, ## args)
#define exfat_fs_error_ratelimit(sb, fmt, args...) \
		__exfat_fs_error(sb, __ratelimit(&EXFAT_SB(sb)->ratelimit), \
		fmt, ## args)
void exfat_msg(struct super_block *sb, const char *lv, const char *fmt, ...)
		__printf(3, 4) __cold;
#define exfat_err(sb, fmt, ...)						\
	exfat_msg(sb, KERN_ERR, fmt, ##__VA_ARGS__)
#define exfat_warn(sb, fmt, ...)					\
	exfat_msg(sb, KERN_WARNING, fmt, ##__VA_ARGS__)
#define exfat_info(sb, fmt, ...)					\
	exfat_msg(sb, KERN_INFO, fmt, ##__VA_ARGS__)

void exfat_get_entry_time(struct exfat_sb_info *sbi, struct timespec64 *ts,
		u8 tz, __le16 time, __le16 date, u8 time_cs);
void exfat_truncate_atime(struct timespec64 *ts);
void exfat_set_entry_time(struct exfat_sb_info *sbi, struct timespec64 *ts,
		u8 *tz, __le16 *time, __le16 *date, u8 *time_cs);
u16 exfat_calc_chksum16(void *data, int len, u16 chksum, int type);
u32 exfat_calc_chksum32(void *data, int len, u32 chksum, int type);
void exfat_update_bh(struct buffer_head *bh, int sync);
int exfat_update_bhs(struct buffer_head **bhs, int nr_bhs, int sync);
void exfat_chain_set(struct exfat_chain *ec, unsigned int dir,
		unsigned int size, unsigned char flags);
void exfat_chain_dup(struct exfat_chain *dup, struct exfat_chain *ec);

#endif /* !_EXFAT_FS_H */
