/*
 * super.c
 *
 * PURPOSE
 *  Super block routines for the OSTA-UDF(tm) filesystem.
 *
 * DESCRIPTION
 *  OSTA-UDF(tm) = Optical Storage Technology Association
 *  Universal Disk Format.
 *
 *  This code is based on version 2.00 of the UDF specification,
 *  and revision 3 of the ECMA 167 standard [equivalent to ISO 13346].
 *    http://www.osta.org/
 *    https://www.ecma.ch/
 *    https://www.iso.org/
 *
 * COPYRIGHT
 *  This file is distributed under the terms of the GNU General Public
 *  License (GPL). Copies of the GPL can be obtained from:
 *    ftp://prep.ai.mit.edu/pub/gnu/GPL
 *  Each contributing author retains all rights to their own work.
 *
 *  (C) 1998 Dave Boynton
 *  (C) 1998-2004 Ben Fennema
 *  (C) 2000 Stelias Computing Inc
 *
 * HISTORY
 *
 *  09/24/98 dgb  changed to allow compiling outside of kernel, and
 *                added some debugging.
 *  10/01/98 dgb  updated to allow (some) possibility of compiling w/2.0.34
 *  10/16/98      attempting some multi-session support
 *  10/17/98      added freespace count for "df"
 *  11/11/98 gr   added novrs option
 *  11/26/98 dgb  added fileset,anchor mount options
 *  12/06/98 blf  really hosed things royally. vat/sparing support. sequenced
 *                vol descs. rewrote option handling based on isofs
 *  12/20/98      find the free space bitmap (if it exists)
 */

#include "udfdecl.h"

#include <linux/blkdev.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/parser.h>
#include <linux/stat.h>
#include <linux/cdrom.h>
#include <linux/nls.h>
#include <linux/vfs.h>
#include <linux/vmalloc.h>
#include <linux/errno.h>
#include <linux/mount.h>
#include <linux/seq_file.h>
#include <linux/bitmap.h>
#include <linux/crc-itu-t.h>
#include <linux/log2.h>
#include <asm/byteorder.h>

#include "udf_sb.h"
#include "udf_i.h"

#include <linux/init.h>
#include <linux/uaccess.h>

enum {
	VDS_POS_PRIMARY_VOL_DESC,
	VDS_POS_UNALLOC_SPACE_DESC,
	VDS_POS_LOGICAL_VOL_DESC,
	VDS_POS_IMP_USE_VOL_DESC,
	VDS_POS_LENGTH
};

#define VSD_FIRST_SECTOR_OFFSET		32768
#define VSD_MAX_SECTOR_OFFSET		0x800000

/*
 * Maximum number of Terminating Descriptor / Logical Volume Integrity
 * Descriptor redirections. The chosen numbers are arbitrary - just that we
 * hopefully don't limit any real use of rewritten inode on write-once media
 * but avoid looping for too long on corrupted media.
 */
#define UDF_MAX_TD_NESTING 64
#define UDF_MAX_LVID_NESTING 1000

enum { UDF_MAX_LINKS = 0xffff };

/* These are the "meat" - everything else is stuffing */
static int udf_fill_super(struct super_block *, void *, int);
static void udf_put_super(struct super_block *);
static int udf_sync_fs(struct super_block *, int);
static int udf_remount_fs(struct super_block *, int *, char *);
static void udf_load_logicalvolint(struct super_block *, struct kernel_extent_ad);
static void udf_open_lvid(struct super_block *);
static void udf_close_lvid(struct super_block *);
static unsigned int udf_count_free(struct super_block *);
static int udf_statfs(struct dentry *, struct kstatfs *);
static int udf_show_options(struct seq_file *, struct dentry *);

struct logicalVolIntegrityDescImpUse *udf_sb_lvidiu(struct super_block *sb)
{
	struct logicalVolIntegrityDesc *lvid;
	unsigned int partnum;
	unsigned int offset;

	if (!UDF_SB(sb)->s_lvid_bh)
		return NULL;
	lvid = (struct logicalVolIntegrityDesc *)UDF_SB(sb)->s_lvid_bh->b_data;
	partnum = le32_to_cpu(lvid->numOfPartitions);
	if ((sb->s_blocksize - sizeof(struct logicalVolIntegrityDescImpUse) -
	     offsetof(struct logicalVolIntegrityDesc, impUse)) /
	     (2 * sizeof(uint32_t)) < partnum) {
		udf_err(sb, "Logical volume integrity descriptor corrupted "
			"(numOfPartitions = %u)!\n", partnum);
		return NULL;
	}
	/* The offset is to skip freeSpaceTable and sizeTable arrays */
	offset = partnum * 2 * sizeof(uint32_t);
	return (struct logicalVolIntegrityDescImpUse *)&(lvid->impUse[offset]);
}

/* UDF filesystem type */
static struct dentry *udf_mount(struct file_system_type *fs_type,
		      int flags, const char *dev_name, void *data)
{
	return mount_bdev(fs_type, flags, dev_name, data, udf_fill_super);
}

static struct file_system_type udf_fstype = {
	.owner		= THIS_MODULE,
	.name		= "udf",
	.mount		= udf_mount,
	.kill_sb	= kill_block_super,
	.fs_flags	= FS_REQUIRES_DEV,
};
MODULE_ALIAS_FS("udf");

static struct kmem_cache *udf_inode_cachep;

static struct inode *udf_alloc_inode(struct super_block *sb)
{
	struct udf_inode_info *ei;
	ei = kmem_cache_alloc(udf_inode_cachep, GFP_KERNEL);
	if (!ei)
		return NULL;

	ei->i_unique = 0;
	ei->i_lenExtents = 0;
	ei->i_lenStreams = 0;
	ei->i_next_alloc_block = 0;
	ei->i_next_alloc_goal = 0;
	ei->i_strat4096 = 0;
	ei->i_streamdir = 0;
	init_rwsem(&ei->i_data_sem);
	ei->cached_extent.lstart = -1;
	spin_lock_init(&ei->i_extent_cache_lock);

	return &ei->vfs_inode;
}

static void udf_free_in_core_inode(struct inode *inode)
{
	kmem_cache_free(udf_inode_cachep, UDF_I(inode));
}

static void init_once(void *foo)
{
	struct udf_inode_info *ei = (struct udf_inode_info *)foo;

	ei->i_data = NULL;
	inode_init_once(&ei->vfs_inode);
}

static int __init init_inodecache(void)
{
	udf_inode_cachep = kmem_cache_create("udf_inode_cache",
					     sizeof(struct udf_inode_info),
					     0, (SLAB_RECLAIM_ACCOUNT |
						 SLAB_MEM_SPREAD |
						 SLAB_ACCOUNT),
					     init_once);
	if (!udf_inode_cachep)
		return -ENOMEM;
	return 0;
}

static void destroy_inodecache(void)
{
	/*
	 * Make sure all delayed rcu free inodes are flushed before we
	 * destroy cache.
	 */
	rcu_barrier();
	kmem_cache_destroy(udf_inode_cachep);
}

/* Superblock operations */
static const struct super_operations udf_sb_ops = {
	.alloc_inode	= udf_alloc_inode,
	.free_inode	= udf_free_in_core_inode,
	.write_inode	= udf_write_inode,
	.evict_inode	= udf_evict_inode,
	.put_super	= udf_put_super,
	.sync_fs	= udf_sync_fs,
	.statfs		= udf_statfs,
	.remount_fs	= udf_remount_fs,
	.show_options	= udf_show_options,
};

struct udf_options {
	unsigned char novrs;
	unsigned int blocksize;
	unsigned int session;
	unsigned int lastblock;
	unsigned int anchor;
	unsigned int flags;
	umode_t umask;
	kgid_t gid;
	kuid_t uid;
	umode_t fmode;
	umode_t dmode;
	struct nls_table *nls_map;
};

static int __init init_udf_fs(void)
{
	int err;

	err = init_inodecache();
	if (err)
		goto out1;
	err = register_filesystem(&udf_fstype);
	if (err)
		goto out;

	return 0;

out:
	destroy_inodecache();

out1:
	return err;
}

static void __exit exit_udf_fs(void)
{
	unregister_filesystem(&udf_fstype);
	destroy_inodecache();
}

static int udf_sb_alloc_partition_maps(struct super_block *sb, u32 count)
{
	struct udf_sb_info *sbi = UDF_SB(sb);

	sbi->s_partmaps = kcalloc(count, sizeof(*sbi->s_partmaps), GFP_KERNEL);
	if (!sbi->s_partmaps) {
		sbi->s_partitions = 0;
		return -ENOMEM;
	}

	sbi->s_partitions = count;
	return 0;
}

static void udf_sb_free_bitmap(struct udf_bitmap *bitmap)
{
	int i;
	int nr_groups = bitmap->s_nr_groups;

	for (i = 0; i < nr_groups; i++)
		brelse(bitmap->s_block_bitmap[i]);

	kvfree(bitmap);
}

static void udf_free_partition(struct udf_part_map *map)
{
	int i;
	struct udf_meta_data *mdata;

	if (map->s_partition_flags & UDF_PART_FLAG_UNALLOC_TABLE)
		iput(map->s_uspace.s_table);
	if (map->s_partition_flags & UDF_PART_FLAG_UNALLOC_BITMAP)
		udf_sb_free_bitmap(map->s_uspace.s_bitmap);
	if (map->s_partition_type == UDF_SPARABLE_MAP15)
		for (i = 0; i < 4; i++)
			brelse(map->s_type_specific.s_sparing.s_spar_map[i]);
	else if (map->s_partition_type == UDF_METADATA_MAP25) {
		mdata = &map->s_type_specific.s_metadata;
		iput(mdata->s_metadata_fe);
		mdata->s_metadata_fe = NULL;

		iput(mdata->s_mirror_fe);
		mdata->s_mirror_fe = NULL;

		iput(mdata->s_bitmap_fe);
		mdata->s_bitmap_fe = NULL;
	}
}

static void udf_sb_free_partitions(struct super_block *sb)
{
	struct udf_sb_info *sbi = UDF_SB(sb);
	int i;

	if (!sbi->s_partmaps)
		return;
	for (i = 0; i < sbi->s_partitions; i++)
		udf_free_partition(&sbi->s_partmaps[i]);
	kfree(sbi->s_partmaps);
	sbi->s_partmaps = NULL;
}

static int udf_show_options(struct seq_file *seq, struct dentry *root)
{
	struct super_block *sb = root->d_sb;
	struct udf_sb_info *sbi = UDF_SB(sb);

	if (!UDF_QUERY_FLAG(sb, UDF_FLAG_STRICT))
		seq_puts(seq, ",nostrict");
	if (UDF_QUERY_FLAG(sb, UDF_FLAG_BLOCKSIZE_SET))
		seq_printf(seq, ",bs=%lu", sb->s_blocksize);
	if (UDF_QUERY_FLAG(sb, UDF_FLAG_UNHIDE))
		seq_puts(seq, ",unhide");
	if (UDF_QUERY_FLAG(sb, UDF_FLAG_UNDELETE))
		seq_puts(seq, ",undelete");
	if (!UDF_QUERY_FLAG(sb, UDF_FLAG_USE_AD_IN_ICB))
		seq_puts(seq, ",noadinicb");
	if (UDF_QUERY_FLAG(sb, UDF_FLAG_USE_SHORT_AD))
		seq_puts(seq, ",shortad");
	if (UDF_QUERY_FLAG(sb, UDF_FLAG_UID_FORGET))
		seq_puts(seq, ",uid=forget");
	if (UDF_QUERY_FLAG(sb, UDF_FLAG_GID_FORGET))
		seq_puts(seq, ",gid=forget");
	if (UDF_QUERY_FLAG(sb, UDF_FLAG_UID_SET))
		seq_printf(seq, ",uid=%u", from_kuid(&init_user_ns, sbi->s_uid));
	if (UDF_QUERY_FLAG(sb, UDF_FLAG_GID_SET))
		seq_printf(seq, ",gid=%u", from_kgid(&init_user_ns, sbi->s_gid));
	if (sbi->s_umask != 0)
		seq_printf(seq, ",umask=%ho", sbi->s_umask);
	if (sbi->s_fmode != UDF_INVALID_MODE)
		seq_printf(seq, ",mode=%ho", sbi->s_fmode);
	if (sbi->s_dmode != UDF_INVALID_MODE)
		seq_printf(seq, ",dmode=%ho", sbi->s_dmode);
	if (UDF_QUERY_FLAG(sb, UDF_FLAG_SESSION_SET))
		seq_printf(seq, ",session=%d", sbi->s_session);
	if (UDF_QUERY_FLAG(sb, UDF_FLAG_LASTBLOCK_SET))
		seq_printf(seq, ",lastblock=%u", sbi->s_last_block);
	if (sbi->s_anchor != 0)
		seq_printf(seq, ",anchor=%u", sbi->s_anchor);
	if (UDF_QUERY_FLAG(sb, UDF_FLAG_UTF8))
		seq_puts(seq, ",utf8");
	if (UDF_QUERY_FLAG(sb, UDF_FLAG_NLS_MAP) && sbi->s_nls_map)
		seq_printf(seq, ",iocharset=%s", sbi->s_nls_map->charset);

	return 0;
}

/*
 * udf_parse_options
 *
 * PURPOSE
 *	Parse mount options.
 *
 * DESCRIPTION
 *	The following mount options are supported:
 *
 *	gid=		Set the default group.
 *	umask=		Set the default umask.
 *	mode=		Set the default file permissions.
 *	dmode=		Set the default directory permissions.
 *	uid=		Set the default user.
 *	bs=		Set the block size.
 *	unhide		Show otherwise hidden files.
 *	undelete	Show deleted files in lists.
 *	adinicb		Embed data in the inode (default)
 *	noadinicb	Don't embed data in the inode
 *	shortad		Use short ad's
 *	longad		Use long ad's (default)
 *	nostrict	Unset strict conformance
 *	iocharset=	Set the NLS character set
 *
 *	The remaining are for debugging and disaster recovery:
 *
 *	novrs		Skip volume sequence recognition
 *
 *	The following expect a offset from 0.
 *
 *	session=	Set the CDROM session (default= last session)
 *	anchor=		Override standard anchor location. (default= 256)
 *	volume=		Override the VolumeDesc location. (unused)
 *	partition=	Override the PartitionDesc location. (unused)
 *	lastblock=	Set the last block of the filesystem/
 *
 *	The following expect a offset from the partition root.
 *
 *	fileset=	Override the fileset block location. (unused)
 *	rootdir=	Override the root directory location. (unused)
 *		WARNING: overriding the rootdir to a non-directory may
 *		yield highly unpredictable results.
 *
 * PRE-CONDITIONS
 *	options		Pointer to mount options string.
 *	uopts		Pointer to mount options variable.
 *
 * POST-CONDITIONS
 *	<return>	1	Mount options parsed okay.
 *	<return>	0	Error parsing mount options.
 *
 * HISTORY
 *	July 1, 1997 - Andrew E. Mileski
 *	Written, tested, and released.
 */

enum {
	Opt_novrs, Opt_nostrict, Opt_bs, Opt_unhide, Opt_undelete,
	Opt_noadinicb, Opt_adinicb, Opt_shortad, Opt_longad,
	Opt_gid, Opt_uid, Opt_umask, Opt_session, Opt_lastblock,
	Opt_anchor, Opt_volume, Opt_partition, Opt_fileset,
	Opt_rootdir, Opt_utf8, Opt_iocharset,
	Opt_err, Opt_uforget, Opt_uignore, Opt_gforget, Opt_gignore,
	Opt_fmode, Opt_dmode
};

static const match_table_t tokens = {
	{Opt_novrs,	"novrs"},
	{Opt_nostrict,	"nostrict"},
	{Opt_bs,	"bs=%u"},
	{Opt_unhide,	"unhide"},
	{Opt_undelete,	"undelete"},
	{Opt_noadinicb,	"noadinicb"},
	{Opt_adinicb,	"adinicb"},
	{Opt_shortad,	"shortad"},
	{Opt_longad,	"longad"},
	{Opt_uforget,	"uid=forget"},
	{Opt_uignore,	"uid=ignore"},
	{Opt_gforget,	"gid=forget"},
	{Opt_gignore,	"gid=ignore"},
	{Opt_gid,	"gid=%u"},
	{Opt_uid,	"uid=%u"},
	{Opt_umask,	"umask=%o"},
	{Opt_session,	"session=%u"},
	{Opt_lastblock,	"lastblock=%u"},
	{Opt_anchor,	"anchor=%u"},
	{Opt_volume,	"volume=%u"},
	{Opt_partition,	"partition=%u"},
	{Opt_fileset,	"fileset=%u"},
	{Opt_rootdir,	"rootdir=%u"},
	{Opt_utf8,	"utf8"},
	{Opt_iocharset,	"iocharset=%s"},
	{Opt_fmode,     "mode=%o"},
	{Opt_dmode,     "dmode=%o"},
	{Opt_err,	NULL}
};

static int udf_parse_options(char *options, struct udf_options *uopt,
			     bool remount)
{
	char *p;
	int option;
	unsigned int uv;

	uopt->novrs = 0;
	uopt->session = 0xFFFFFFFF;
	uopt->lastblock = 0;
	uopt->anchor = 0;

	if (!options)
		return 1;

	while ((p = strsep(&options, ",")) != NULL) {
		substring_t args[MAX_OPT_ARGS];
		int token;
		unsigned n;
		if (!*p)
			continue;

		token = match_token(p, tokens, args);
		switch (token) {
		case Opt_novrs:
			uopt->novrs = 1;
			break;
		case Opt_bs:
			if (match_int(&args[0], &option))
				return 0;
			n = option;
			if (n != 512 && n != 1024 && n != 2048 && n != 4096)
				return 0;
			uopt->blocksize = n;
			uopt->flags |= (1 << UDF_FLAG_BLOCKSIZE_SET);
			break;
		case Opt_unhide:
			uopt->flags |= (1 << UDF_FLAG_UNHIDE);
			break;
		case Opt_undelete:
			uopt->flags |= (1 << UDF_FLAG_UNDELETE);
			break;
		case Opt_noadinicb:
			uopt->flags &= ~(1 << UDF_FLAG_USE_AD_IN_ICB);
			break;
		case Opt_adinicb:
			uopt->flags |= (1 << UDF_FLAG_USE_AD_IN_ICB);
			break;
		case Opt_shortad:
			uopt->flags |= (1 << UDF_FLAG_USE_SHORT_AD);
			break;
		case Opt_longad:
			uopt->flags &= ~(1 << UDF_FLAG_USE_SHORT_AD);
			break;
		case Opt_gid:
			if (match_uint(args, &uv))
				return 0;
			uopt->gid = make_kgid(current_user_ns(), uv);
			if (!gid_valid(uopt->gid))
				return 0;
			uopt->flags |= (1 << UDF_FLAG_GID_SET);
			break;
		case Opt_uid:
			if (match_uint(args, &uv))
				return 0;
			uopt->uid = make_kuid(current_user_ns(), uv);
			if (!uid_valid(uopt->uid))
				return 0;
			uopt->flags |= (1 << UDF_FLAG_UID_SET);
			break;
		case Opt_umask:
			if (match_octal(args, &option))
				return 0;
			uopt->umask = option;
			break;
		case Opt_nostrict:
			uopt->flags &= ~(1 << UDF_FLAG_STRICT);
			break;
		case Opt_session:
			if (match_int(args, &option))
				return 0;
			uopt->session = option;
			if (!remount)
				uopt->flags |= (1 << UDF_FLAG_SESSION_SET);
			break;
		case Opt_lastblock:
			if (match_int(args, &option))
				return 0;
			uopt->lastblock = option;
			if (!remount)
				uopt->flags |= (1 << UDF_FLAG_LASTBLOCK_SET);
			break;
		case Opt_anchor:
			if (match_int(args, &option))
				return 0;
			uopt->anchor = option;
			break;
		case Opt_volume:
		case Opt_partition:
		case Opt_fileset:
		case Opt_rootdir:
			/* Ignored (never implemented properly) */
			break;
		case Opt_utf8:
			uopt->flags |= (1 << UDF_FLAG_UTF8);
			break;
		case Opt_iocharset:
			if (!remount) {
				if (uopt->nls_map)
					unload_nls(uopt->nls_map);
				/*
				 * load_nls() failure is handled later in
				 * udf_fill_super() after all options are
				 * parsed.
				 */
				uopt->nls_map = load_nls(args[0].from);
				uopt->flags |= (1 << UDF_FLAG_NLS_MAP);
			}
			break;
		case Opt_uforget:
			uopt->flags |= (1 << UDF_FLAG_UID_FORGET);
			break;
		case Opt_uignore:
		case Opt_gignore:
			/* These options are superseeded by uid=<number> */
			break;
		case Opt_gforget:
			uopt->flags |= (1 << UDF_FLAG_GID_FORGET);
			break;
		case Opt_fmode:
			if (match_octal(args, &option))
				return 0;
			uopt->fmode = option & 0777;
			break;
		case Opt_dmode:
			if (match_octal(args, &option))
				return 0;
			uopt->dmode = option & 0777;
			break;
		default:
			pr_err("bad mount option \"%s\" or missing value\n", p);
			return 0;
		}
	}
	return 1;
}

static int udf_remount_fs(struct super_block *sb, int *flags, char *options)
{
	struct udf_options uopt;
	struct udf_sb_info *sbi = UDF_SB(sb);
	int error = 0;

	if (!(*flags & SB_RDONLY) && UDF_QUERY_FLAG(sb, UDF_FLAG_RW_INCOMPAT))
		return -EACCES;

	sync_filesystem(sb);

	uopt.flags = sbi->s_flags;
	uopt.uid   = sbi->s_uid;
	uopt.gid   = sbi->s_gid;
	uopt.umask = sbi->s_umask;
	uopt.fmode = sbi->s_fmode;
	uopt.dmode = sbi->s_dmode;
	uopt.nls_map = NULL;

	if (!udf_parse_options(options, &uopt, true))
		return -EINVAL;

	write_lock(&sbi->s_cred_lock);
	sbi->s_flags = uopt.flags;
	sbi->s_uid   = uopt.uid;
	sbi->s_gid   = uopt.gid;
	sbi->s_umask = uopt.umask;
	sbi->s_fmode = uopt.fmode;
	sbi->s_dmode = uopt.dmode;
	write_unlock(&sbi->s_cred_lock);

	if ((bool)(*flags & SB_RDONLY) == sb_rdonly(sb))
		goto out_unlock;

	if (*flags & SB_RDONLY)
		udf_close_lvid(sb);
	else
		udf_open_lvid(sb);

out_unlock:
	return error;
}

/*
 * Check VSD descriptor. Returns -1 in case we are at the end of volume
 * recognition area, 0 if the descriptor is valid but non-interesting, 1 if
 * we found one of NSR descriptors we are looking for.
 */
static int identify_vsd(const struct volStructDesc *vsd)
{
	int ret = 0;

	if (!memcmp(vsd->stdIdent, VSD_STD_ID_CD001, VSD_STD_ID_LEN)) {
		switch (vsd->structType) {
		case 0:
			udf_debug("ISO9660 Boot Record found\n");
			break;
		case 1:
			udf_debug("ISO9660 Primary Volume Descriptor found\n");
			break;
		case 2:
			udf_debug("ISO9660 Supplementary Volume Descriptor found\n");
			break;
		case 3:
			udf_debug("ISO9660 Volume Partition Descriptor found\n");
			break;
		case 255:
			udf_debug("ISO9660 Volume Descriptor Set Terminator found\n");
			break;
		default:
			udf_debug("ISO9660 VRS (%u) found\n", vsd->structType);
			break;
		}
	} else if (!memcmp(vsd->stdIdent, VSD_STD_ID_BEA01, VSD_STD_ID_LEN))
		; /* ret = 0 */
	else if (!memcmp(vsd->stdIdent, VSD_STD_ID_NSR02, VSD_STD_ID_LEN))
		ret = 1;
	else if (!memcmp(vsd->stdIdent, VSD_STD_ID_NSR03, VSD_STD_ID_LEN))
		ret = 1;
	else if (!memcmp(vsd->stdIdent, VSD_STD_ID_BOOT2, VSD_STD_ID_LEN))
		; /* ret = 0 */
	else if (!memcmp(vsd->stdIdent, VSD_STD_ID_CDW02, VSD_STD_ID_LEN))
		; /* ret = 0 */
	else {
		/* TEA01 or invalid id : end of volume recognition area */
		ret = -1;
	}

	return ret;
}

/*
 * Check Volume Structure Descriptors (ECMA 167 2/9.1)
 * We also check any "CD-ROM Volume Descriptor Set" (ECMA 167 2/8.3.1)
 * @return   1 if NSR02 or NSR03 found,
 *	    -1 if first sector read error, 0 otherwise
 */
static int udf_check_vsd(struct super_block *sb)
{
	struct volStructDesc *vsd = NULL;
	loff_t sector = VSD_FIRST_SECTOR_OFFSET;
	int sectorsize;
	struct buffer_head *bh = NULL;
	int nsr = 0;
	struct udf_sb_info *sbi;
	loff_t session_offset;

	sbi = UDF_SB(sb);
	if (sb->s_blocksize < sizeof(struct volStructDesc))
		sectorsize = sizeof(struct volStructDesc);
	else
		sectorsize = sb->s_blocksize;

	session_offset = (loff_t)sbi->s_session << sb->s_blocksize_bits;
	sector += session_offset;

	udf_debug("Starting at sector %u (%lu byte sectors)\n",
		  (unsigned int)(sector >> sb->s_blocksize_bits),
		  sb->s_blocksize);
	/* Process the sequence (if applicable). The hard limit on the sector
	 * offset is arbitrary, hopefully large enough so that all valid UDF
	 * filesystems will be recognised. There is no mention of an upper
	 * bound to the size of the volume recognition area in the standard.
	 *  The limit will prevent the code to read all the sectors of a
	 * specially crafted image (like a bluray disc full of CD001 sectors),
	 * potentially causing minutes or even hours of uninterruptible I/O
	 * activity. This actually happened with uninitialised SSD partitions
	 * (all 0xFF) before the check for the limit and all valid IDs were
	 * added */
	for (; !nsr && sector < VSD_MAX_SECTOR_OFFSET; sector += sectorsize) {
		/* Read a block */
		bh = udf_tread(sb, sector >> sb->s_blocksize_bits);
		if (!bh)
			break;

		vsd = (struct volStructDesc *)(bh->b_data +
					      (sector & (sb->s_blocksize - 1)));
		nsr = identify_vsd(vsd);
		/* Found NSR or end? */
		if (nsr) {
			brelse(bh);
			break;
		}
		/*
		 * Special handling for improperly formatted VRS (e.g., Win10)
		 * where components are separated by 2048 bytes even though
		 * sectors are 4K
		 */
		if (sb->s_blocksize == 4096) {
			nsr = identify_vsd(vsd + 1);
			/* Ignore unknown IDs... */
			if (nsr < 0)
				nsr = 0;
		}
		brelse(bh);
	}

	if (nsr > 0)
		return 1;
	else if (!bh && sector - session_offset == VSD_FIRST_SECTOR_OFFSET)
		return -1;
	else
		return 0;
}

static int udf_verify_domain_identifier(struct super_block *sb,
					struct regid *ident, char *dname)
{
	struct domainIdentSuffix *suffix;

	if (memcmp(ident->ident, UDF_ID_COMPLIANT, strlen(UDF_ID_COMPLIANT))) {
		udf_warn(sb, "Not OSTA UDF compliant %s descriptor.\n", dname);
		goto force_ro;
	}
	if (ident->flags & ENTITYID_FLAGS_DIRTY) {
		udf_warn(sb, "Possibly not OSTA UDF compliant %s descriptor.\n",
			 dname);
		goto force_ro;
	}
	suffix = (struct domainIdentSuffix *)ident->identSuffix;
	if ((suffix->domainFlags & DOMAIN_FLAGS_HARD_WRITE_PROTECT) ||
	    (suffix->domainFlags & DOMAIN_FLAGS_SOFT_WRITE_PROTECT)) {
		if (!sb_rdonly(sb)) {
			udf_warn(sb, "Descriptor for %s marked write protected."
				 " Forcing read only mount.\n", dname);
		}
		goto force_ro;
	}
	return 0;

force_ro:
	if (!sb_rdonly(sb))
		return -EACCES;
	UDF_SET_FLAG(sb, UDF_FLAG_RW_INCOMPAT);
	return 0;
}

static int udf_load_fileset(struct super_block *sb, struct fileSetDesc *fset,
			    struct kernel_lb_addr *root)
{
	int ret;

	ret = udf_verify_domain_identifier(sb, &fset->domainIdent, "file set");
	if (ret < 0)
		return ret;

	*root = lelb_to_cpu(fset->rootDirectoryICB.extLocation);
	UDF_SB(sb)->s_serial_number = le16_to_cpu(fset->descTag.tagSerialNum);

	udf_debug("Rootdir at block=%u, partition=%u\n",
		  root->logicalBlockNum, root->partitionReferenceNum);
	return 0;
}

static int udf_find_fileset(struct super_block *sb,
			    struct kernel_lb_addr *fileset,
			    struct kernel_lb_addr *root)
{
	struct buffer_head *bh = NULL;
	uint16_t ident;
	int ret;

	if (fileset->logicalBlockNum == 0xFFFFFFFF &&
	    fileset->partitionReferenceNum == 0xFFFF)
		return -EINVAL;

	bh = udf_read_ptagged(sb, fileset, 0, &ident);
	if (!bh)
		return -EIO;
	if (ident != TAG_IDENT_FSD) {
		brelse(bh);
		return -EINVAL;
	}

	udf_debug("Fileset at block=%u, partition=%u\n",
		  fileset->logicalBlockNum, fileset->partitionReferenceNum);

	UDF_SB(sb)->s_partition = fileset->partitionReferenceNum;
	ret = udf_load_fileset(sb, (struct fileSetDesc *)bh->b_data, root);
	brelse(bh);
	return ret;
}

/*
 * Load primary Volume Descriptor Sequence
 *
 * Return <0 on error, 0 on success. -EAGAIN is special meaning next sequence
 * should be tried.
 */
static int udf_load_pvoldesc(struct super_block *sb, sector_t block)
{
	struct primaryVolDesc *pvoldesc;
	uint8_t *outstr;
	struct buffer_head *bh;
	uint16_t ident;
	int ret;
	struct timestamp *ts;

	outstr = kmalloc(128, GFP_NOFS);
	if (!outstr)
		return -ENOMEM;

	bh = udf_read_tagged(sb, block, block, &ident);
	if (!bh) {
		ret = -EAGAIN;
		goto out2;
	}

	if (ident != TAG_IDENT_PVD) {
		ret = -EIO;
		goto out_bh;
	}

	pvoldesc = (struct primaryVolDesc *)bh->b_data;

	udf_disk_stamp_to_time(&UDF_SB(sb)->s_record_time,
			      pvoldesc->recordingDateAndTime);
	ts = &pvoldesc->recordingDateAndTime;
	udf_debug("recording time %04u/%02u/%02u %02u:%02u (%x)\n",
		  le16_to_cpu(ts->year), ts->month, ts->day, ts->hour,
		  ts->minute, le16_to_cpu(ts->typeAndTimezone));

	ret = udf_dstrCS0toChar(sb, outstr, 31, pvoldesc->volIdent, 32);
	if (ret < 0) {
		strcpy(UDF_SB(sb)->s_volume_ident, "InvalidName");
		pr_warn("incorrect volume identification, setting to "
			"'InvalidName'\n");
	} else {
		strncpy(UDF_SB(sb)->s_volume_ident, outstr, ret);
	}
	udf_debug("volIdent[] = '%s'\n", UDF_SB(sb)->s_volume_ident);

	ret = udf_dstrCS0toChar(sb, outstr, 127, pvoldesc->volSetIdent, 128);
	if (ret < 0) {
		ret = 0;
		goto out_bh;
	}
	outstr[ret] = 0;
	udf_debug("volSetIdent[] = '%s'\n", outstr);

	ret = 0;
out_bh:
	brelse(bh);
out2:
	kfree(outstr);
	return ret;
}

struct inode *udf_find_metadata_inode_efe(struct super_block *sb,
					u32 meta_file_loc, u32 partition_ref)
{
	struct kernel_lb_addr addr;
	struct inode *metadata_fe;

	addr.logicalBlockNum = meta_file_loc;
	addr.partitionReferenceNum = partition_ref;

	metadata_fe = udf_iget_special(sb, &addr);

	if (IS_ERR(metadata_fe)) {
		udf_warn(sb, "metadata inode efe not found\n");
		return metadata_fe;
	}
	if (UDF_I(metadata_fe)->i_alloc_type != ICBTAG_FLAG_AD_SHORT) {
		udf_warn(sb, "metadata inode efe does not have short allocation descriptors!\n");
		iput(metadata_fe);
		return ERR_PTR(-EIO);
	}

	return metadata_fe;
}

static int udf_load_metadata_files(struct super_block *sb, int partition,
				   int type1_index)
{
	struct udf_sb_info *sbi = UDF_SB(sb);
	struct udf_part_map *map;
	struct udf_meta_data *mdata;
	struct kernel_lb_addr addr;
	struct inode *fe;

	map = &sbi->s_partmaps[partition];
	mdata = &map->s_type_specific.s_metadata;
	mdata->s_phys_partition_ref = type1_index;

	/* metadata address */
	udf_debug("Metadata file location: block = %u part = %u\n",
		  mdata->s_meta_file_loc, mdata->s_phys_partition_ref);

	fe = udf_find_metadata_inode_efe(sb, mdata->s_meta_file_loc,
					 mdata->s_phys_partition_ref);
	if (IS_ERR(fe)) {
		/* mirror file entry */
		udf_debug("Mirror metadata file location: block = %u part = %u\n",
			  mdata->s_mirror_file_loc, mdata->s_phys_partition_ref);

		fe = udf_find_metadata_inode_efe(sb, mdata->s_mirror_file_loc,
						 mdata->s_phys_partition_ref);

		if (IS_ERR(fe)) {
			udf_err(sb, "Both metadata and mirror metadata inode efe can not found\n");
			return PTR_ERR(fe);
		}
		mdata->s_mirror_fe = fe;
	} else
		mdata->s_metadata_fe = fe;


	/*
	 * bitmap file entry
	 * Note:
	 * Load only if bitmap file location differs from 0xFFFFFFFF (DCN-5102)
	*/
	if (mdata->s_bitmap_file_loc != 0xFFFFFFFF) {
		addr.logicalBlockNum = mdata->s_bitmap_file_loc;
		addr.partitionReferenceNum = mdata->s_phys_partition_ref;

		udf_debug("Bitmap file location: block = %u part = %u\n",
			  addr.logicalBlockNum, addr.partitionReferenceNum);

		fe = udf_iget_special(sb, &addr);
		if (IS_ERR(fe)) {
			if (sb_rdonly(sb))
				udf_warn(sb, "bitmap inode efe not found but it's ok since the disc is mounted read-only\n");
			else {
				udf_err(sb, "bitmap inode efe not found and attempted read-write mount\n");
				return PTR_ERR(fe);
			}
		} else
			mdata->s_bitmap_fe = fe;
	}

	udf_debug("udf_load_metadata_files Ok\n");
	return 0;
}

int udf_compute_nr_groups(struct super_block *sb, u32 partition)
{
	struct udf_part_map *map = &UDF_SB(sb)->s_partmaps[partition];
	return DIV_ROUND_UP(map->s_partition_len +
			    (sizeof(struct spaceBitmapDesc) << 3),
			    sb->s_blocksize * 8);
}

static struct udf_bitmap *udf_sb_alloc_bitmap(struct super_block *sb, u32 index)
{
	struct udf_bitmap *bitmap;
	int nr_groups = udf_compute_nr_groups(sb, index);

	bitmap = kvzalloc(struct_size(bitmap, s_block_bitmap, nr_groups),
			  GFP_KERNEL);
	if (!bitmap)
		return NULL;

	bitmap->s_nr_groups = nr_groups;
	return bitmap;
}

static int check_partition_desc(struct super_block *sb,
				struct partitionDesc *p,
				struct udf_part_map *map)
{
	bool umap, utable, fmap, ftable;
	struct partitionHeaderDesc *phd;

	switch (le32_to_cpu(p->accessType)) {
	case PD_ACCESS_TYPE_READ_ONLY:
	case PD_ACCESS_TYPE_WRITE_ONCE:
	case PD_ACCESS_TYPE_NONE:
		goto force_ro;
	}

	/* No Partition Header Descriptor? */
	if (strcmp(p->partitionContents.ident, PD_PARTITION_CONTENTS_NSR02) &&
	    strcmp(p->partitionContents.ident, PD_PARTITION_CONTENTS_NSR03))
		goto force_ro;

	phd = (struct partitionHeaderDesc *)p->partitionContentsUse;
	utable = phd->unallocSpaceTable.extLength;
	umap = phd->unallocSpaceBitmap.extLength;
	ftable = phd->freedSpaceTable.extLength;
	fmap = phd->freedSpaceBitmap.extLength;

	/* No allocation info? */
	if (!utable && !umap && !ftable && !fmap)
		goto force_ro;

	/* We don't support blocks that require erasing before overwrite */
	if (ftable || fmap)
		goto force_ro;
	/* UDF 2.60: 2.3.3 - no mixing of tables & bitmaps, no VAT. */
	if (utable && umap)
		goto force_ro;

	if (map->s_partition_type == UDF_VIRTUAL_MAP15 ||
	    map->s_partition_type == UDF_VIRTUAL_MAP20 ||
	    map->s_partition_type == UDF_METADATA_MAP25)
		goto force_ro;

	return 0;
force_ro:
	if (!sb_rdonly(sb))
		return -EACCES;
	UDF_SET_FLAG(sb, UDF_FLAG_RW_INCOMPAT);
	return 0;
}

static int udf_fill_partdesc_info(struct super_block *sb,
		struct partitionDesc *p, int p_index)
{
	struct udf_part_map *map;
	struct udf_sb_info *sbi = UDF_SB(sb);
	struct partitionHeaderDesc *phd;
	int err;

	map = &sbi->s_partmaps[p_index];

	map->s_partition_len = le32_to_cpu(p->partitionLength); /* blocks */
	map->s_partition_root = le32_to_cpu(p->partitionStartingLocation);

	if (p->accessType == cpu_to_le32(PD_ACCESS_TYPE_READ_ONLY))
		map->s_partition_flags |= UDF_PART_FLAG_READ_ONLY;
	if (p->accessType == cpu_to_le32(PD_ACCESS_TYPE_WRITE_ONCE))
		map->s_partition_flags |= UDF_PART_FLAG_WRITE_ONCE;
	if (p->accessType == cpu_to_le32(PD_ACCESS_TYPE_REWRITABLE))
		map->s_partition_flags |= UDF_PART_FLAG_REWRITABLE;
	if (p->accessType == cpu_to_le32(PD_ACCESS_TYPE_OVERWRITABLE))
		map->s_partition_flags |= UDF_PART_FLAG_OVERWRITABLE;

	udf_debug("Partition (%d type %x) starts at physical %u, block length %u\n",
		  p_index, map->s_partition_type,
		  map->s_partition_root, map->s_partition_len);

	err = check_partition_desc(sb, p, map);
	if (err)
		return err;

	/*
	 * Skip loading allocation info it we cannot ever write to the fs.
	 * This is a correctness thing as we may have decided to force ro mount
	 * to avoid allocation info we don't support.
	 */
	if (UDF_QUERY_FLAG(sb, UDF_FLAG_RW_INCOMPAT))
		return 0;

	phd = (struct partitionHeaderDesc *)p->partitionContentsUse;
	if (phd->unallocSpaceTable.extLength) {
		struct kernel_lb_addr loc = {
			.logicalBlockNum = le32_to_cpu(
				phd->unallocSpaceTable.extPosition),
			.partitionReferenceNum = p_index,
		};
		struct inode *inode;

		inode = udf_iget_special(sb, &loc);
		if (IS_ERR(inode)) {
			udf_debug("cannot load unallocSpaceTable (part %d)\n",
				  p_index);
			return PTR_ERR(inode);
		}
		map->s_uspace.s_table = inode;
		map->s_partition_flags |= UDF_PART_FLAG_UNALLOC_TABLE;
		udf_debug("unallocSpaceTable (part %d) @ %lu\n",
			  p_index, map->s_uspace.s_table->i_ino);
	}

	if (phd->unallocSpaceBitmap.extLength) {
		struct udf_bitmap *bitmap = udf_sb_alloc_bitmap(sb, p_index);
		if (!bitmap)
			return -ENOMEM;
		map->s_uspace.s_bitmap = bitmap;
		bitmap->s_extPosition = le32_to_cpu(
				phd->unallocSpaceBitmap.extPosition);
		map->s_partition_flags |= UDF_PART_FLAG_UNALLOC_BITMAP;
		udf_debug("unallocSpaceBitmap (part %d) @ %u\n",
			  p_index, bitmap->s_extPosition);
	}

	return 0;
}

static void udf_find_vat_block(struct super_block *sb, int p_index,
			       int type1_index, sector_t start_block)
{
	struct udf_sb_info *sbi = UDF_SB(sb);
	struct udf_part_map *map = &sbi->s_partmaps[p_index];
	sector_t vat_block;
	struct kernel_lb_addr ino;
	struct inode *inode;

	/*
	 * VAT file entry is in the last recorded block. Some broken disks have
	 * it a few blocks before so try a bit harder...
	 */
	ino.partitionReferenceNum = type1_index;
	for (vat_block = start_block;
	     vat_block >= map->s_partition_root &&
	     vat_block >= start_block - 3; vat_block--) {
		ino.logicalBlockNum = vat_block - map->s_partition_root;
		inode = udf_iget_special(sb, &ino);
		if (!IS_ERR(inode)) {
			sbi->s_vat_inode = inode;
			break;
		}
	}
}

static int udf_load_vat(struct super_block *sb, int p_index, int type1_index)
{
	struct udf_sb_info *sbi = UDF_SB(sb);
	struct udf_part_map *map = &sbi->s_partmaps[p_index];
	struct buffer_head *bh = NULL;
	struct udf_inode_info *vati;
	uint32_t pos;
	struct virtualAllocationTable20 *vat20;
	sector_t blocks = i_size_read(sb->s_bdev->bd_inode) >>
			  sb->s_blocksize_bits;

	udf_find_vat_block(sb, p_index, type1_index, sbi->s_last_block);
	if (!sbi->s_vat_inode &&
	    sbi->s_last_block != blocks - 1) {
		pr_notice("Failed to read VAT inode from the last recorded block (%lu), retrying with the last block of the device (%lu).\n",
			  (unsigned long)sbi->s_last_block,
			  (unsigned long)blocks - 1);
		udf_find_vat_block(sb, p_index, type1_index, blocks - 1);
	}
	if (!sbi->s_vat_inode)
		return -EIO;

	if (map->s_partition_type == UDF_VIRTUAL_MAP15) {
		map->s_type_specific.s_virtual.s_start_offset = 0;
		map->s_type_specific.s_virtual.s_num_entries =
			(sbi->s_vat_inode->i_size - 36) >> 2;
	} else if (map->s_partition_type == UDF_VIRTUAL_MAP20) {
		vati = UDF_I(sbi->s_vat_inode);
		if (vati->i_alloc_type != ICBTAG_FLAG_AD_IN_ICB) {
			pos = udf_block_map(sbi->s_vat_inode, 0);
			bh = sb_bread(sb, pos);
			if (!bh)
				return -EIO;
			vat20 = (struct virtualAllocationTable20 *)bh->b_data;
		} else {
			vat20 = (struct virtualAllocationTable20 *)
							vati->i_data;
		}

		map->s_type_specific.s_virtual.s_start_offset =
			le16_to_cpu(vat20->lengthHeader);
		map->s_type_specific.s_virtual.s_num_entries =
			(sbi->s_vat_inode->i_size -
				map->s_type_specific.s_virtual.
					s_start_offset) >> 2;
		brelse(bh);
	}
	return 0;
}

/*
 * Load partition descriptor block
 *
 * Returns <0 on error, 0 on success, -EAGAIN is special - try next descriptor
 * sequence.
 */
static int udf_load_partdesc(struct super_block *sb, sector_t block)
{
	struct buffer_head *bh;
	struct partitionDesc *p;
	struct udf_part_map *map;
	struct udf_sb_info *sbi = UDF_SB(sb);
	int i, type1_idx;
	uint16_t partitionNumber;
	uint16_t ident;
	int ret;

	bh = udf_read_tagged(sb, block, block, &ident);
	if (!bh)
		return -EAGAIN;
	if (ident != TAG_IDENT_PD) {
		ret = 0;
		goto out_bh;
	}

	p = (struct partitionDesc *)bh->b_data;
	partitionNumber = le16_to_cpu(p->partitionNumber);

	/* First scan for TYPE1 and SPARABLE partitions */
	for (i = 0; i < sbi->s_partitions; i++) {
		map = &sbi->s_partmaps[i];
		udf_debug("Searching map: (%u == %u)\n",
			  map->s_partition_num, partitionNumber);
		if (map->s_partition_num == partitionNumber &&
		    (map->s_partition_type == UDF_TYPE1_MAP15 ||
		     map->s_partition_type == UDF_SPARABLE_MAP15))
			break;
	}

	if (i >= sbi->s_partitions) {
		udf_debug("Partition (%u) not found in partition map\n",
			  partitionNumber);
		ret = 0;
		goto out_bh;
	}

	ret = udf_fill_partdesc_info(sb, p, i);
	if (ret < 0)
		goto out_bh;

	/*
	 * Now rescan for VIRTUAL or METADATA partitions when SPARABLE and
	 * PHYSICAL partitions are already set up
	 */
	type1_idx = i;
	map = NULL; /* supress 'maybe used uninitialized' warning */
	for (i = 0; i < sbi->s_partitions; i++) {
		map = &sbi->s_partmaps[i];

		if (map->s_partition_num == partitionNumber &&
		    (map->s_partition_type == UDF_VIRTUAL_MAP15 ||
		     map->s_partition_type == UDF_VIRTUAL_MAP20 ||
		     map->s_partition_type == UDF_METADATA_MAP25))
			break;
	}

	if (i >= sbi->s_partitions) {
		ret = 0;
		goto out_bh;
	}

	ret = udf_fill_partdesc_info(sb, p, i);
	if (ret < 0)
		goto out_bh;

	if (map->s_partition_type == UDF_METADATA_MAP25) {
		ret = udf_load_metadata_files(sb, i, type1_idx);
		if (ret < 0) {
			udf_err(sb, "error loading MetaData partition map %d\n",
				i);
			goto out_bh;
		}
	} else {
		/*
		 * If we have a partition with virtual map, we don't handle
		 * writing to it (we overwrite blocks instead of relocating
		 * them).
		 */
		if (!sb_rdonly(sb)) {
			ret = -EACCES;
			goto out_bh;
		}
		UDF_SET_FLAG(sb, UDF_FLAG_RW_INCOMPAT);
		ret = udf_load_vat(sb, i, type1_idx);
		if (ret < 0)
			goto out_bh;
	}
	ret = 0;
out_bh:
	/* In case loading failed, we handle cleanup in udf_fill_super */
	brelse(bh);
	return ret;
}

static int udf_load_sparable_map(struct super_block *sb,
				 struct udf_part_map *map,
				 struct sparablePartitionMap *spm)
{
	uint32_t loc;
	uint16_t ident;
	struct sparingTable *st;
	struct udf_sparing_data *sdata = &map->s_type_specific.s_sparing;
	int i;
	struct buffer_head *bh;

	map->s_partition_type = UDF_SPARABLE_MAP15;
	sdata->s_packet_len = le16_to_cpu(spm->packetLength);
	if (!is_power_of_2(sdata->s_packet_len)) {
		udf_err(sb, "error loading logical volume descriptor: "
			"Invalid packet length %u\n",
			(unsigned)sdata->s_packet_len);
		return -EIO;
	}
	if (spm->numSparingTables > 4) {
		udf_err(sb, "error loading logical volume descriptor: "
			"Too many sparing tables (%d)\n",
			(int)spm->numSparingTables);
		return -EIO;
	}
	if (le32_to_cpu(spm->sizeSparingTable) > sb->s_blocksize) {
		udf_err(sb, "error loading logical volume descriptor: "
			"Too big sparing table size (%u)\n",
			le32_to_cpu(spm->sizeSparingTable));
		return -EIO;
	}

	for (i = 0; i < spm->numSparingTables; i++) {
		loc = le32_to_cpu(spm->locSparingTable[i]);
		bh = udf_read_tagged(sb, loc, loc, &ident);
		if (!bh)
			continue;

		st = (struct sparingTable *)bh->b_data;
		if (ident != 0 ||
		    strncmp(st->sparingIdent.ident, UDF_ID_SPARING,
			    strlen(UDF_ID_SPARING)) ||
		    sizeof(*st) + le16_to_cpu(st->reallocationTableLen) >
							sb->s_blocksize) {
			brelse(bh);
			continue;
		}

		sdata->s_spar_map[i] = bh;
	}
	map->s_partition_func = udf_get_pblock_spar15;
	return 0;
}

static int udf_load_logicalvol(struct super_block *sb, sector_t block,
			       struct kernel_lb_addr *fileset)
{
	struct logicalVolDesc *lvd;
	int i, offset;
	uint8_t type;
	struct udf_sb_info *sbi = UDF_SB(sb);
	struct genericPartitionMap *gpm;
	uint16_t ident;
	struct buffer_head *bh;
	unsigned int table_len;
	int ret;

	bh = udf_read_tagged(sb, block, block, &ident);
	if (!bh)
		return -EAGAIN;
	BUG_ON(ident != TAG_IDENT_LVD);
	lvd = (struct logicalVolDesc *)bh->b_data;
	table_len = le32_to_cpu(lvd->mapTableLength);
	if (table_len > sb->s_blocksize - sizeof(*lvd)) {
		udf_err(sb, "error loading logical volume descriptor: "
			"Partition table too long (%u > %lu)\n", table_len,
			sb->s_blocksize - sizeof(*lvd));
		ret = -EIO;
		goto out_bh;
	}

	ret = udf_verify_domain_identifier(sb, &lvd->domainIdent,
					   "logical volume");
	if (ret)
		goto out_bh;
	ret = udf_sb_alloc_partition_maps(sb, le32_to_cpu(lvd->numPartitionMaps));
	if (ret)
		goto out_bh;

	for (i = 0, offset = 0;
	     i < sbi->s_partitions && offset < table_len;
	     i++, offset += gpm->partitionMapLength) {
		struct udf_part_map *map = &sbi->s_partmaps[i];
		gpm = (struct genericPartitionMap *)
				&(lvd->partitionMaps[offset]);
		type = gpm->partitionMapType;
		if (type == 1) {
			struct genericPartitionMap1 *gpm1 =
				(struct genericPartitionMap1 *)gpm;
			map->s_partition_type = UDF_TYPE1_MAP15;
			map->s_volumeseqnum = le16_to_cpu(gpm1->volSeqNum);
			map->s_partition_num = le16_to_cpu(gpm1->partitionNum);
			map->s_partition_func = NULL;
		} else if (type == 2) {
			struct udfPartitionMap2 *upm2 =
						(struct udfPartitionMap2 *)gpm;
			if (!strncmp(upm2->partIdent.ident, UDF_ID_VIRTUAL,
						strlen(UDF_ID_VIRTUAL))) {
				u16 suf =
					le16_to_cpu(((__le16 *)upm2->partIdent.
							identSuffix)[0]);
				if (suf < 0x0200) {
					map->s_partition_type =
							UDF_VIRTUAL_MAP15;
					map->s_partition_func =
							udf_get_pblock_virt15;
				} else {
					map->s_partition_type =
							UDF_VIRTUAL_MAP20;
					map->s_partition_func =
							udf_get_pblock_virt20;
				}
			} else if (!strncmp(upm2->partIdent.ident,
						UDF_ID_SPARABLE,
						strlen(UDF_ID_SPARABLE))) {
				ret = udf_load_sparable_map(sb, map,
					(struct sparablePartitionMap *)gpm);
				if (ret < 0)
					goto out_bh;
			} else if (!strncmp(upm2->partIdent.ident,
						UDF_ID_METADATA,
						strlen(UDF_ID_METADATA))) {
				struct udf_meta_data *mdata =
					&map->s_type_specific.s_metadata;
				struct metadataPartitionMap *mdm =
						(struct metadataPartitionMap *)
						&(lvd->partitionMaps[offset]);
				udf_debug("Parsing Logical vol part %d type %u  id=%s\n",
					  i, type, UDF_ID_METADATA);

				map->s_partition_type = UDF_METADATA_MAP25;
				map->s_partition_func = udf_get_pblock_meta25;

				mdata->s_meta_file_loc   =
					le32_to_cpu(mdm->metadataFileLoc);
				mdata->s_mirror_file_loc =
					le32_to_cpu(mdm->metadataMirrorFileLoc);
				mdata->s_bitmap_file_loc =
					le32_to_cpu(mdm->metadataBitmapFileLoc);
				mdata->s_alloc_unit_size =
					le32_to_cpu(mdm->allocUnitSize);
				mdata->s_align_unit_size =
					le16_to_cpu(mdm->alignUnitSize);
				if (mdm->flags & 0x01)
					mdata->s_flags |= MF_DUPLICATE_MD;

				udf_debug("Metadata Ident suffix=0x%x\n",
					  le16_to_cpu(*(__le16 *)
						      mdm->partIdent.identSuffix));
				udf_debug("Metadata part num=%u\n",
					  le16_to_cpu(mdm->partitionNum));
				udf_debug("Metadata part alloc unit size=%u\n",
					  le32_to_cpu(mdm->allocUnitSize));
				udf_debug("Metadata file loc=%u\n",
					  le32_to_cpu(mdm->metadataFileLoc));
				udf_debug("Mirror file loc=%u\n",
					  le32_to_cpu(mdm->metadataMirrorFileLoc));
				udf_debug("Bitmap file loc=%u\n",
					  le32_to_cpu(mdm->metadataBitmapFileLoc));
				udf_debug("Flags: %d %u\n",
					  mdata->s_flags, mdm->flags);
			} else {
				udf_debug("Unknown ident: %s\n",
					  upm2->partIdent.ident);
				continue;
			}
			map->s_volumeseqnum = le16_to_cpu(upm2->volSeqNum);
			map->s_partition_num = le16_to_cpu(upm2->partitionNum);
		}
		udf_debug("Partition (%d:%u) type %u on volume %u\n",
			  i, map->s_partition_num, type, map->s_volumeseqnum);
	}

	if (fileset) {
		struct long_ad *la = (struct long_ad *)&(lvd->logicalVolContentsUse[0]);

		*fileset = lelb_to_cpu(la->extLocation);
		udf_debug("FileSet found in LogicalVolDesc at block=%u, partition=%u\n",
			  fileset->logicalBlockNum,
			  fileset->partitionReferenceNum);
	}
	if (lvd->integritySeqExt.extLength)
		udf_load_logicalvolint(sb, leea_to_cpu(lvd->integritySeqExt));
	ret = 0;

	if (!sbi->s_lvid_bh) {
		/* We can't generate unique IDs without a valid LVID */
		if (sb_rdonly(sb)) {
			UDF_SET_FLAG(sb, UDF_FLAG_RW_INCOMPAT);
		} else {
			udf_warn(sb, "Damaged or missing LVID, forcing "
				     "readonly mount\n");
			ret = -EACCES;
		}
	}
out_bh:
	brelse(bh);
	return ret;
}

/*
 * Find the prevailing Logical Volume Integrity Descriptor.
 */
static void udf_load_logicalvolint(struct super_block *sb, struct kernel_extent_ad loc)
{
	struct buffer_head *bh, *final_bh;
	uint16_t ident;
	struct udf_sb_info *sbi = UDF_SB(sb);
	struct logicalVolIntegrityDesc *lvid;
	int indirections = 0;

	while (++indirections <= UDF_MAX_LVID_NESTING) {
		final_bh = NULL;
		while (loc.extLength > 0 &&
			(bh = udf_read_tagged(sb, loc.extLocation,
					loc.extLocation, &ident))) {
			if (ident != TAG_IDENT_LVID) {
				brelse(bh);
				break;
			}

			brelse(final_bh);
			final_bh = bh;

			loc.extLength -= sb->s_blocksize;
			loc.extLocation++;
		}

		if (!final_bh)
			return;

		brelse(sbi->s_lvid_bh);
		sbi->s_lvid_bh = final_bh;

		lvid = (struct logicalVolIntegrityDesc *)final_bh->b_data;
		if (lvid->nextIntegrityExt.extLength == 0)
			return;

		loc = leea_to_cpu(lvid->nextIntegrityExt);
	}

	udf_warn(sb, "Too many LVID indirections (max %u), ignoring.\n",
		UDF_MAX_LVID_NESTING);
	brelse(sbi->s_lvid_bh);
	sbi->s_lvid_bh = NULL;
}

/*
 * Step for reallocation of table of partition descriptor sequence numbers.
 * Must be power of 2.
 */
#define PART_DESC_ALLOC_STEP 32

struct part_desc_seq_scan_data {
	struct udf_vds_record rec;
	u32 partnum;
};

struct desc_seq_scan_data {
	struct udf_vds_record vds[VDS_POS_LENGTH];
	unsigned int size_part_descs;
	unsigned int num_part_descs;
	struct part_desc_seq_scan_data *part_descs_loc;
};

static struct udf_vds_record *handle_partition_descriptor(
				struct buffer_head *bh,
				struct desc_seq_scan_data *data)
{
	struct partitionDesc *desc = (struct partitionDesc *)bh->b_data;
	int partnum;
	int i;

	partnum = le16_to_cpu(desc->partitionNumber);
	for (i = 0; i < data->num_part_descs; i++)
		if (partnum == data->part_descs_loc[i].partnum)
			return &(data->part_descs_loc[i].rec);
	if (data->num_part_descs >= data->size_part_descs) {
		struct part_desc_seq_scan_data *new_loc;
		unsigned int new_size = ALIGN(partnum, PART_DESC_ALLOC_STEP);

		new_loc = kcalloc(new_size, sizeof(*new_loc), GFP_KERNEL);
		if (!new_loc)
			return ERR_PTR(-ENOMEM);
		memcpy(new_loc, data->part_descs_loc,
		       data->size_part_descs * sizeof(*new_loc));
		kfree(data->part_descs_loc);
		data->part_descs_loc = new_loc;
		data->size_part_descs = new_size;
	}
	return &(data->part_descs_loc[data->num_part_descs++].rec);
}


static struct udf_vds_record *get_volume_descriptor_record(uint16_t ident,
		struct buffer_head *bh, struct desc_seq_scan_data *data)
{
	switch (ident) {
	case TAG_IDENT_PVD: /* ISO 13346 3/10.1 */
		return &(data->vds[VDS_POS_PRIMARY_VOL_DESC]);
	case TAG_IDENT_IUVD: /* ISO 13346 3/10.4 */
		return &(data->vds[VDS_POS_IMP_USE_VOL_DESC]);
	case TAG_IDENT_LVD: /* ISO 13346 3/10.6 */
		return &(data->vds[VDS_POS_LOGICAL_VOL_DESC]);
	case TAG_IDENT_USD: /* ISO 13346 3/10.8 */
		return &(data->vds[VDS_POS_UNALLOC_SPACE_DESC]);
	case TAG_IDENT_PD: /* ISO 13346 3/10.5 */
		return handle_partition_descriptor(bh, data);
	}
	return NULL;
}

/*
 * Process a main/reserve volume descriptor sequence.
 *   @block		First block of first extent of the sequence.
 *   @lastblock		Lastblock of first extent of the sequence.
 *   @fileset		There we store extent containing root fileset
 *
 * Returns <0 on error, 0 on success. -EAGAIN is special - try next descriptor
 * sequence
 */
static noinline int udf_process_sequence(
		struct super_block *sb,
		sector_t block, sector_t lastblock,
		struct kernel_lb_addr *fileset)
{
	struct buffer_head *bh = NULL;
	struct udf_vds_record *curr;
	struct generic_desc *gd;
	struct volDescPtr *vdp;
	bool done = false;
	uint32_t vdsn;
	uint16_t ident;
	int ret;
	unsigned int indirections = 0;
	struct desc_seq_scan_data data;
	unsigned int i;

	memset(data.vds, 0, sizeof(struct udf_vds_record) * VDS_POS_LENGTH);
	data.size_part_descs = PART_DESC_ALLOC_STEP;
	data.num_part_descs = 0;
	data.part_descs_loc = kcalloc(data.size_part_descs,
				      sizeof(*data.part_descs_loc),
				      GFP_KERNEL);
	if (!data.part_descs_loc)
		return -ENOMEM;

	/*
	 * Read the main descriptor sequence and find which descriptors
	 * are in it.
	 */
	for (; (!done && block <= lastblock); block++) {
		bh = udf_read_tagged(sb, block, block, &ident);
		if (!bh)
			break;

		/* Process each descriptor (ISO 13346 3/8.3-8.4) */
		gd = (struct generic_desc *)bh->b_data;
		vdsn = le32_to_cpu(gd->volDescSeqNum);
		switch (ident) {
		case TAG_IDENT_VDP: /* ISO 13346 3/10.3 */
			if (++indirections > UDF_MAX_TD_NESTING) {
				udf_err(sb, "too many Volume Descriptor "
					"Pointers (max %u supported)\n",
					UDF_MAX_TD_NESTING);
				brelse(bh);
				ret = -EIO;
				goto out;
			}

			vdp = (struct volDescPtr *)bh->b_data;
			block = le32_to_cpu(vdp->nextVolDescSeqExt.extLocation);
			lastblock = le32_to_cpu(
				vdp->nextVolDescSeqExt.extLength) >>
				sb->s_blocksize_bits;
			lastblock += block - 1;
			/* For loop is going to increment 'block' again */
			block--;
			break;
		case TAG_IDENT_PVD: /* ISO 13346 3/10.1 */
		case TAG_IDENT_IUVD: /* ISO 13346 3/10.4 */
		case TAG_IDENT_LVD: /* ISO 13346 3/10.6 */
		case TAG_IDENT_USD: /* ISO 13346 3/10.8 */
		case TAG_IDENT_PD: /* ISO 13346 3/10.5 */
			curr = get_volume_descriptor_record(ident, bh, &data);
			if (IS_ERR(curr)) {
				brelse(bh);
				ret = PTR_ERR(curr);
				goto out;
			}
			/* Descriptor we don't care about? */
			if (!curr)
				break;
			if (vdsn >= curr->volDescSeqNum) {
				curr->volDescSeqNum = vdsn;
				curr->block = block;
			}
			break;
		case TAG_IDENT_TD: /* ISO 13346 3/10.9 */
			done = true;
			break;
		}
		brelse(bh);
	}
	/*
	 * Now read interesting descriptors again and process them
	 * in a suitable order
	 */
	if (!data.vds[VDS_POS_PRIMARY_VOL_DESC].block) {
		udf_err(sb, "Primary Volume Descriptor not found!\n");
		ret = -EAGAIN;
		goto out;
	}
	ret = udf_load_pvoldesc(sb, data.vds[VDS_POS_PRIMARY_VOL_DESC].block);
	if (ret < 0)
		goto out;

	if (data.vds[VDS_POS_LOGICAL_VOL_DESC].block) {
		ret = udf_load_logicalvol(sb,
				data.vds[VDS_POS_LOGICAL_VOL_DESC].block,
				fileset);
		if (ret < 0)
			goto out;
	}

	/* Now handle prevailing Partition Descriptors */
	for (i = 0; i < data.num_part_descs; i++) {
		ret = udf_load_partdesc(sb, data.part_descs_loc[i].rec.block);
		if (ret < 0)
			goto out;
	}
	ret = 0;
out:
	kfree(data.part_descs_loc);
	return ret;
}

/*
 * Load Volume Descriptor Sequence described by anchor in bh
 *
 * Returns <0 on error, 0 on success
 */
static int udf_load_sequence(struct super_block *sb, struct buffer_head *bh,
			     struct kernel_lb_addr *fileset)
{
	struct anchorVolDescPtr *anchor;
	sector_t main_s, main_e, reserve_s, reserve_e;
	int ret;

	anchor = (struct anchorVolDescPtr *)bh->b_data;

	/* Locate the main sequence */
	main_s = le32_to_cpu(anchor->mainVolDescSeqExt.extLocation);
	main_e = le32_to_cpu(anchor->mainVolDescSeqExt.extLength);
	main_e = main_e >> sb->s_blocksize_bits;
	main_e += main_s - 1;

	/* Locate the reserve sequence */
	reserve_s = le32_to_cpu(anchor->reserveVolDescSeqExt.extLocation);
	reserve_e = le32_to_cpu(anchor->reserveVolDescSeqExt.extLength);
	reserve_e = reserve_e >> sb->s_blocksize_bits;
	reserve_e += reserve_s - 1;

	/* Process the main & reserve sequences */
	/* responsible for finding the PartitionDesc(s) */
	ret = udf_process_sequence(sb, main_s, main_e, fileset);
	if (ret != -EAGAIN)
		return ret;
	udf_sb_free_partitions(sb);
	ret = udf_process_sequence(sb, reserve_s, reserve_e, fileset);
	if (ret < 0) {
		udf_sb_free_partitions(sb);
		/* No sequence was OK, return -EIO */
		if (ret == -EAGAIN)
			ret = -EIO;
	}
	return ret;
}

/*
 * Check whether there is an anchor block in the given block and
 * load Volume Descriptor Sequence if so.
 *
 * Returns <0 on error, 0 on success, -EAGAIN is special - try next anchor
 * block
 */
static int udf_check_anchor_block(struct super_block *sb, sector_t block,
				  struct kernel_lb_addr *fileset)
{
	struct buffer_head *bh;
	uint16_t ident;
	int ret;

	if (UDF_QUERY_FLAG(sb, UDF_FLAG_VARCONV) &&
	    udf_fixed_to_variable(block) >=
	    i_size_read(sb->s_bdev->bd_inode) >> sb->s_blocksize_bits)
		return -EAGAIN;

	bh = udf_read_tagged(sb, block, block, &ident);
	if (!bh)
		return -EAGAIN;
	if (ident != TAG_IDENT_AVDP) {
		brelse(bh);
		return -EAGAIN;
	}
	ret = udf_load_sequence(sb, bh, fileset);
	brelse(bh);
	return ret;
}

/*
 * Search for an anchor volume descriptor pointer.
 *
 * Returns < 0 on error, 0 on success. -EAGAIN is special - try next set
 * of anchors.
 */
static int udf_scan_anchors(struct super_block *sb, sector_t *lastblock,
			    struct kernel_lb_addr *fileset)
{
	sector_t last[6];
	int i;
	struct udf_sb_info *sbi = UDF_SB(sb);
	int last_count = 0;
	int ret;

	/* First try user provided anchor */
	if (sbi->s_anchor) {
		ret = udf_check_anchor_block(sb, sbi->s_anchor, fileset);
		if (ret != -EAGAIN)
			return ret;
	}
	/*
	 * according to spec, anchor is in either:
	 *     block 256
	 *     lastblock-256
	 *     lastblock
	 *  however, if the disc isn't closed, it could be 512.
	 */
	ret = udf_check_anchor_block(sb, sbi->s_session + 256, fileset);
	if (ret != -EAGAIN)
		return ret;
	/*
	 * The trouble is which block is the last one. Drives often misreport
	 * this so we try various possibilities.
	 */
	last[last_count++] = *lastblock;
	if (*lastblock >= 1)
		last[last_count++] = *lastblock - 1;
	last[last_count++] = *lastblock + 1;
	if (*lastblock >= 2)
		last[last_count++] = *lastblock - 2;
	if (*lastblock >= 150)
		last[last_count++] = *lastblock - 150;
	if (*lastblock >= 152)
		last[last_count++] = *lastblock - 152;

	for (i = 0; i < last_count; i++) {
		if (last[i] >= i_size_read(sb->s_bdev->bd_inode) >>
				sb->s_blocksize_bits)
			continue;
		ret = udf_check_anchor_block(sb, last[i], fileset);
		if (ret != -EAGAIN) {
			if (!ret)
				*lastblock = last[i];
			return ret;
		}
		if (last[i] < 256)
			continue;
		ret = udf_check_anchor_block(sb, last[i] - 256, fileset);
		if (ret != -EAGAIN) {
			if (!ret)
				*lastblock = last[i];
			return ret;
		}
	}

	/* Finally try block 512 in case media is open */
	return udf_check_anchor_block(sb, sbi->s_session + 512, fileset);
}

/*
 * Find an anchor volume descriptor and load Volume Descriptor Sequence from
 * area specified by it. The function expects sbi->s_lastblock to be the last
 * block on the media.
 *
 * Return <0 on error, 0 if anchor found. -EAGAIN is special meaning anchor
 * was not found.
 */
static int udf_find_anchor(struct super_block *sb,
			   struct kernel_lb_addr *fileset)
{
	struct udf_sb_info *sbi = UDF_SB(sb);
	sector_t lastblock = sbi->s_last_block;
	int ret;

	ret = udf_scan_anchors(sb, &lastblock, fileset);
	if (ret != -EAGAIN)
		goto out;

	/* No anchor found? Try VARCONV conversion of block numbers */
	UDF_SET_FLAG(sb, UDF_FLAG_VARCONV);
	lastblock = udf_variable_to_fixed(sbi->s_last_block);
	/* Firstly, we try to not convert number of the last block */
	ret = udf_scan_anchors(sb, &lastblock, fileset);
	if (ret != -EAGAIN)
		goto out;

	lastblock = sbi->s_last_block;
	/* Secondly, we try with converted number of the last block */
	ret = udf_scan_anchors(sb, &lastblock, fileset);
	if (ret < 0) {
		/* VARCONV didn't help. Clear it. */
		UDF_CLEAR_FLAG(sb, UDF_FLAG_VARCONV);
	}
out:
	if (ret == 0)
		sbi->s_last_block = lastblock;
	return ret;
}

/*
 * Check Volume Structure Descriptor, find Anchor block and load Volume
 * Descriptor Sequence.
 *
 * Returns < 0 on error, 0 on success. -EAGAIN is special meaning anchor
 * block was not found.
 */
static int udf_load_vrs(struct super_block *sb, struct udf_options *uopt,
			int silent, struct kernel_lb_addr *fileset)
{
	struct udf_sb_info *sbi = UDF_SB(sb);
	int nsr = 0;
	int ret;

	if (!sb_set_blocksize(sb, uopt->blocksize)) {
		if (!silent)
			udf_warn(sb, "Bad block size\n");
		return -EINVAL;
	}
	sbi->s_last_block = uopt->lastblock;
	if (!uopt->novrs) {
		/* Check that it is NSR02 compliant */
		nsr = udf_check_vsd(sb);
		if (!nsr) {
			if (!silent)
				udf_warn(sb, "No VRS found\n");
			return -EINVAL;
		}
		if (nsr == -1)
			udf_debug("Failed to read sector at offset %d. "
				  "Assuming open disc. Skipping validity "
				  "check\n", VSD_FIRST_SECTOR_OFFSET);
		if (!sbi->s_last_block)
			sbi->s_last_block = udf_get_last_block(sb);
	} else {
		udf_debug("Validity check skipped because of novrs option\n");
	}

	/* Look for anchor block and load Volume Descriptor Sequence */
	sbi->s_anchor = uopt->anchor;
	ret = udf_find_anchor(sb, fileset);
	if (ret < 0) {
		if (!silent && ret == -EAGAIN)
			udf_warn(sb, "No anchor found\n");
		return ret;
	}
	return 0;
}

static void udf_finalize_lvid(struct logicalVolIntegrityDesc *lvid)
{
	struct timespec64 ts;

	ktime_get_real_ts64(&ts);
	udf_time_to_disk_stamp(&lvid->recordingDateAndTime, ts);
	lvid->descTag.descCRC = cpu_to_le16(
		crc_itu_t(0, (char *)lvid + sizeof(struct tag),
			le16_to_cpu(lvid->descTag.descCRCLength)));
	lvid->descTag.tagChecksum = udf_tag_checksum(&lvid->descTag);
}

static void udf_open_lvid(struct super_block *sb)
{
	struct udf_sb_info *sbi = UDF_SB(sb);
	struct buffer_head *bh = sbi->s_lvid_bh;
	struct logicalVolIntegrityDesc *lvid;
	struct logicalVolIntegrityDescImpUse *lvidiu;

	if (!bh)
		return;
	lvid = (struct logicalVolIntegrityDesc *)bh->b_data;
	lvidiu = udf_sb_lvidiu(sb);
	if (!lvidiu)
		return;

	mutex_lock(&sbi->s_alloc_mutex);
	lvidiu->impIdent.identSuffix[0] = UDF_OS_CLASS_UNIX;
	lvidiu->impIdent.identSuffix[1] = UDF_OS_ID_LINUX;
	if (le32_to_cpu(lvid->integrityType) == LVID_INTEGRITY_TYPE_CLOSE)
		lvid->integrityType = cpu_to_le32(LVID_INTEGRITY_TYPE_OPEN);
	else
		UDF_SET_FLAG(sb, UDF_FLAG_INCONSISTENT);

	udf_finalize_lvid(lvid);
	mark_buffer_dirty(bh);
	sbi->s_lvid_dirty = 0;
	mutex_unlock(&sbi->s_alloc_mutex);
	/* Make opening of filesystem visible on the media immediately */
	sync_dirty_buffer(bh);
}

static void udf_close_lvid(struct super_block *sb)
{
	struct udf_sb_info *sbi = UDF_SB(sb);
	struct buffer_head *bh = sbi->s_lvid_bh;
	struct logicalVolIntegrityDesc *lvid;
	struct logicalVolIntegrityDescImpUse *lvidiu;

	if (!bh)
		return;
	lvid = (struct logicalVolIntegrityDesc *)bh->b_data;
	lvidiu = udf_sb_lvidiu(sb);
	if (!lvidiu)
		return;

	mutex_lock(&sbi->s_alloc_mutex);
	lvidiu->impIdent.identSuffix[0] = UDF_OS_CLASS_UNIX;
	lvidiu->impIdent.identSuffix[1] = UDF_OS_ID_LINUX;
	if (UDF_MAX_WRITE_VERSION > le16_to_cpu(lvidiu->maxUDFWriteRev))
		lvidiu->maxUDFWriteRev = cpu_to_le16(UDF_MAX_WRITE_VERSION);
	if (sbi->s_udfrev > le16_to_cpu(lvidiu->minUDFReadRev))
		lvidiu->minUDFReadRev = cpu_to_le16(sbi->s_udfrev);
	if (sbi->s_udfrev > le16_to_cpu(lvidiu->minUDFWriteRev))
		lvidiu->minUDFWriteRev = cpu_to_le16(sbi->s_udfrev);
	if (!UDF_QUERY_FLAG(sb, UDF_FLAG_INCONSISTENT))
		lvid->integrityType = cpu_to_le32(LVID_INTEGRITY_TYPE_CLOSE);

	/*
	 * We set buffer uptodate unconditionally here to avoid spurious
	 * warnings from mark_buffer_dirty() when previous EIO has marked
	 * the buffer as !uptodate
	 */
	set_buffer_uptodate(bh);
	udf_finalize_lvid(lvid);
	mark_buffer_dirty(bh);
	sbi->s_lvid_dirty = 0;
	mutex_unlock(&sbi->s_alloc_mutex);
	/* Make closing of filesystem visible on the media immediately */
	sync_dirty_buffer(bh);
}

u64 lvid_get_unique_id(struct super_block *sb)
{
	struct buffer_head *bh;
	struct udf_sb_info *sbi = UDF_SB(sb);
	struct logicalVolIntegrityDesc *lvid;
	struct logicalVolHeaderDesc *lvhd;
	u64 uniqueID;
	u64 ret;

	bh = sbi->s_lvid_bh;
	if (!bh)
		return 0;

	lvid = (struct logicalVolIntegrityDesc *)bh->b_data;
	lvhd = (struct logicalVolHeaderDesc *)lvid->logicalVolContentsUse;

	mutex_lock(&sbi->s_alloc_mutex);
	ret = uniqueID = le64_to_cpu(lvhd->uniqueID);
	if (!(++uniqueID & 0xFFFFFFFF))
		uniqueID += 16;
	lvhd->uniqueID = cpu_to_le64(uniqueID);
	udf_updated_lvid(sb);
	mutex_unlock(&sbi->s_alloc_mutex);

	return ret;
}

static int udf_fill_super(struct super_block *sb, void *options, int silent)
{
	int ret = -EINVAL;
	struct inode *inode = NULL;
	struct udf_options uopt;
	struct kernel_lb_addr rootdir, fileset;
	struct udf_sb_info *sbi;
	bool lvid_open = false;

	uopt.flags = (1 << UDF_FLAG_USE_AD_IN_ICB) | (1 << UDF_FLAG_STRICT);
	/* By default we'll use overflow[ug]id when UDF inode [ug]id == -1 */
	uopt.uid = make_kuid(current_user_ns(), overflowuid);
	uopt.gid = make_kgid(current_user_ns(), overflowgid);
	uopt.umask = 0;
	uopt.fmode = UDF_INVALID_MODE;
	uopt.dmode = UDF_INVALID_MODE;
	uopt.nls_map = NULL;

	sbi = kzalloc(sizeof(*sbi), GFP_KERNEL);
	if (!sbi)
		return -ENOMEM;

	sb->s_fs_info = sbi;

	mutex_init(&sbi->s_alloc_mutex);

	if (!udf_parse_options((char *)options, &uopt, false))
		goto parse_options_failure;

	if (uopt.flags & (1 << UDF_FLAG_UTF8) &&
	    uopt.flags & (1 << UDF_FLAG_NLS_MAP)) {
		udf_err(sb, "utf8 cannot be combined with iocharset\n");
		goto parse_options_failure;
	}
	if ((uopt.flags & (1 << UDF_FLAG_NLS_MAP)) && !uopt.nls_map) {
		uopt.nls_map = load_nls_default();
		if (!uopt.nls_map)
			uopt.flags &= ~(1 << UDF_FLAG_NLS_MAP);
		else
			udf_debug("Using default NLS map\n");
	}
	if (!(uopt.flags & (1 << UDF_FLAG_NLS_MAP)))
		uopt.flags |= (1 << UDF_FLAG_UTF8);

	fileset.logicalBlockNum = 0xFFFFFFFF;
	fileset.partitionReferenceNum = 0xFFFF;

	sbi->s_flags = uopt.flags;
	sbi->s_uid = uopt.uid;
	sbi->s_gid = uopt.gid;
	sbi->s_umask = uopt.umask;
	sbi->s_fmode = uopt.fmode;
	sbi->s_dmode = uopt.dmode;
	sbi->s_nls_map = uopt.nls_map;
	rwlock_init(&sbi->s_cred_lock);

	if (uopt.session == 0xFFFFFFFF)
		sbi->s_session = udf_get_last_session(sb);
	else
		sbi->s_session = uopt.session;

	udf_debug("Multi-session=%d\n", sbi->s_session);

	/* Fill in the rest of the superblock */
	sb->s_op = &udf_sb_ops;
	sb->s_export_op = &udf_export_ops;

	sb->s_magic = UDF_SUPER_MAGIC;
	sb->s_time_gran = 1000;

	if (uopt.flags & (1 << UDF_FLAG_BLOCKSIZE_SET)) {
		ret = udf_load_vrs(sb, &uopt, silent, &fileset);
	} else {
		uopt.blocksize = bdev_logical_block_size(sb->s_bdev);
		while (uopt.blocksize <= 4096) {
			ret = udf_load_vrs(sb, &uopt, silent, &fileset);
			if (ret < 0) {
				if (!silent && ret != -EACCES) {
					pr_notice("Scanning with blocksize %u failed\n",
						  uopt.blocksize);
				}
				brelse(sbi->s_lvid_bh);
				sbi->s_lvid_bh = NULL;
				/*
				 * EACCES is special - we want to propagate to
				 * upper layers that we cannot handle RW mount.
				 */
				if (ret == -EACCES)
					break;
			} else
				break;

			uopt.blocksize <<= 1;
		}
	}
	if (ret < 0) {
		if (ret == -EAGAIN) {
			udf_warn(sb, "No partition found (1)\n");
			ret = -EINVAL;
		}
		goto error_out;
	}

	udf_debug("Lastblock=%u\n", sbi->s_last_block);

	if (sbi->s_lvid_bh) {
		struct logicalVolIntegrityDescImpUse *lvidiu =
							udf_sb_lvidiu(sb);
		uint16_t minUDFReadRev;
		uint16_t minUDFWriteRev;

		if (!lvidiu) {
			ret = -EINVAL;
			goto error_out;
		}
		minUDFReadRev = le16_to_cpu(lvidiu->minUDFReadRev);
		minUDFWriteRev = le16_to_cpu(lvidiu->minUDFWriteRev);
		if (minUDFReadRev > UDF_MAX_READ_VERSION) {
			udf_err(sb, "minUDFReadRev=%x (max is %x)\n",
				minUDFReadRev,
				UDF_MAX_READ_VERSION);
			ret = -EINVAL;
			goto error_out;
		} else if (minUDFWriteRev > UDF_MAX_WRITE_VERSION) {
			if (!sb_rdonly(sb)) {
				ret = -EACCES;
				goto error_out;
			}
			UDF_SET_FLAG(sb, UDF_FLAG_RW_INCOMPAT);
		}

		sbi->s_udfrev = minUDFWriteRev;

		if (minUDFReadRev >= UDF_VERS_USE_EXTENDED_FE)
			UDF_SET_FLAG(sb, UDF_FLAG_USE_EXTENDED_FE);
		if (minUDFReadRev >= UDF_VERS_USE_STREAMS)
			UDF_SET_FLAG(sb, UDF_FLAG_USE_STREAMS);
	}

	if (!sbi->s_partitions) {
		udf_warn(sb, "No partition found (2)\n");
		ret = -EINVAL;
		goto error_out;
	}

	if (sbi->s_partmaps[sbi->s_partition].s_partition_flags &
			UDF_PART_FLAG_READ_ONLY) {
		if (!sb_rdonly(sb)) {
			ret = -EACCES;
			goto error_out;
		}
		UDF_SET_FLAG(sb, UDF_FLAG_RW_INCOMPAT);
	}

	ret = udf_find_fileset(sb, &fileset, &rootdir);
	if (ret < 0) {
		udf_warn(sb, "No fileset found\n");
		goto error_out;
	}

	if (!silent) {
		struct timestamp ts;
		udf_time_to_disk_stamp(&ts, sbi->s_record_time);
		udf_info("Mounting volume '%s', timestamp %04u/%02u/%02u %02u:%02u (%x)\n",
			 sbi->s_volume_ident,
			 le16_to_cpu(ts.year), ts.month, ts.day,
			 ts.hour, ts.minute, le16_to_cpu(ts.typeAndTimezone));
	}
	if (!sb_rdonly(sb)) {
		udf_open_lvid(sb);
		lvid_open = true;
	}

	/* Assign the root inode */
	/* assign inodes by physical block number */
	/* perhaps it's not extensible enough, but for now ... */
	inode = udf_iget(sb, &rootdir);
	if (IS_ERR(inode)) {
		udf_err(sb, "Error in udf_iget, block=%u, partition=%u\n",
		       rootdir.logicalBlockNum, rootdir.partitionReferenceNum);
		ret = PTR_ERR(inode);
		goto error_out;
	}

	/* Allocate a dentry for the root inode */
	sb->s_root = d_make_root(inode);
	if (!sb->s_root) {
		udf_err(sb, "Couldn't allocate root dentry\n");
		ret = -ENOMEM;
		goto error_out;
	}
	sb->s_maxbytes = MAX_LFS_FILESIZE;
	sb->s_max_links = UDF_MAX_LINKS;
	return 0;

error_out:
	iput(sbi->s_vat_inode);
parse_options_failure:
	if (uopt.nls_map)
		unload_nls(uopt.nls_map);
	if (lvid_open)
		udf_close_lvid(sb);
	brelse(sbi->s_lvid_bh);
	udf_sb_free_partitions(sb);
	kfree(sbi);
	sb->s_fs_info = NULL;

	return ret;
}

void _udf_err(struct super_block *sb, const char *function,
	      const char *fmt, ...)
{
	struct va_format vaf;
	va_list args;

	va_start(args, fmt);

	vaf.fmt = fmt;
	vaf.va = &args;

	pr_err("error (device %s): %s: %pV", sb->s_id, function, &vaf);

	va_end(args);
}

void _udf_warn(struct super_block *sb, const char *function,
	       const char *fmt, ...)
{
	struct va_format vaf;
	va_list args;

	va_start(args, fmt);

	vaf.fmt = fmt;
	vaf.va = &args;

	pr_warn("warning (device %s): %s: %pV", sb->s_id, function, &vaf);

	va_end(args);
}

static void udf_put_super(struct super_block *sb)
{
	struct udf_sb_info *sbi;

	sbi = UDF_SB(sb);

	iput(sbi->s_vat_inode);
	if (UDF_QUERY_FLAG(sb, UDF_FLAG_NLS_MAP))
		unload_nls(sbi->s_nls_map);
	if (!sb_rdonly(sb))
		udf_close_lvid(sb);
	brelse(sbi->s_lvid_bh);
	udf_sb_free_partitions(sb);
	mutex_destroy(&sbi->s_alloc_mutex);
	kfree(sb->s_fs_info);
	sb->s_fs_info = NULL;
}

static int udf_sync_fs(struct super_block *sb, int wait)
{
	struct udf_sb_info *sbi = UDF_SB(sb);

	mutex_lock(&sbi->s_alloc_mutex);
	if (sbi->s_lvid_dirty) {
		struct buffer_head *bh = sbi->s_lvid_bh;
		struct logicalVolIntegrityDesc *lvid;

		lvid = (struct logicalVolIntegrityDesc *)bh->b_data;
		udf_finalize_lvid(lvid);

		/*
		 * Blockdevice will be synced later so we don't have to submit
		 * the buffer for IO
		 */
		mark_buffer_dirty(bh);
		sbi->s_lvid_dirty = 0;
	}
	mutex_unlock(&sbi->s_alloc_mutex);

	return 0;
}

static int udf_statfs(struct dentry *dentry, struct kstatfs *buf)
{
	struct super_block *sb = dentry->d_sb;
	struct udf_sb_info *sbi = UDF_SB(sb);
	struct logicalVolIntegrityDescImpUse *lvidiu;
	u64 id = huge_encode_dev(sb->s_bdev->bd_dev);

	lvidiu = udf_sb_lvidiu(sb);
	buf->f_type = UDF_SUPER_MAGIC;
	buf->f_bsize = sb->s_blocksize;
	buf->f_blocks = sbi->s_partmaps[sbi->s_partition].s_partition_len;
	buf->f_bfree = udf_count_free(sb);
	buf->f_bavail = buf->f_bfree;
	/*
	 * Let's pretend each free block is also a free 'inode' since UDF does
	 * not have separate preallocated table of inodes.
	 */
	buf->f_files = (lvidiu != NULL ? (le32_to_cpu(lvidiu->numFiles) +
					  le32_to_cpu(lvidiu->numDirs)) : 0)
			+ buf->f_bfree;
	buf->f_ffree = buf->f_bfree;
	buf->f_namelen = UDF_NAME_LEN;
	buf->f_fsid = u64_to_fsid(id);

	return 0;
}

static unsigned int udf_count_free_bitmap(struct super_block *sb,
					  struct udf_bitmap *bitmap)
{
	struct buffer_head *bh = NULL;
	unsigned int accum = 0;
	int index;
	udf_pblk_t block = 0, newblock;
	struct kernel_lb_addr loc;
	uint32_t bytes;
	uint8_t *ptr;
	uint16_t ident;
	struct spaceBitmapDesc *bm;

	loc.logicalBlockNum = bitmap->s_extPosition;
	loc.partitionReferenceNum = UDF_SB(sb)->s_partition;
	bh = udf_read_ptagged(sb, &loc, 0, &ident);

	if (!bh) {
		udf_err(sb, "udf_count_free failed\n");
		goto out;
	} else if (ident != TAG_IDENT_SBD) {
		brelse(bh);
		udf_err(sb, "udf_count_free failed\n");
		goto out;
	}

	bm = (struct spaceBitmapDesc *)bh->b_data;
	bytes = le32_to_cpu(bm->numOfBytes);
	index = sizeof(struct spaceBitmapDesc); /* offset in first block only */
	ptr = (uint8_t *)bh->b_data;

	while (bytes > 0) {
		u32 cur_bytes = min_t(u32, bytes, sb->s_blocksize - index);
		accum += bitmap_weight((const unsigned long *)(ptr + index),
					cur_bytes * 8);
		bytes -= cur_bytes;
		if (bytes) {
			brelse(bh);
			newblock = udf_get_lb_pblock(sb, &loc, ++block);
			bh = udf_tread(sb, newblock);
			if (!bh) {
				udf_debug("read failed\n");
				goto out;
			}
			index = 0;
			ptr = (uint8_t *)bh->b_data;
		}
	}
	brelse(bh);
out:
	return accum;
}

static unsigned int udf_count_free_table(struct super_block *sb,
					 struct inode *table)
{
	unsigned int accum = 0;
	uint32_t elen;
	struct kernel_lb_addr eloc;
	int8_t etype;
	struct extent_position epos;

	mutex_lock(&UDF_SB(sb)->s_alloc_mutex);
	epos.block = UDF_I(table)->i_location;
	epos.offset = sizeof(struct unallocSpaceEntry);
	epos.bh = NULL;

	while ((etype = udf_next_aext(table, &epos, &eloc, &elen, 1)) != -1)
		accum += (elen >> table->i_sb->s_blocksize_bits);

	brelse(epos.bh);
	mutex_unlock(&UDF_SB(sb)->s_alloc_mutex);

	return accum;
}

static unsigned int udf_count_free(struct super_block *sb)
{
	unsigned int accum = 0;
	struct udf_sb_info *sbi = UDF_SB(sb);
	struct udf_part_map *map;
	unsigned int part = sbi->s_partition;
	int ptype = sbi->s_partmaps[part].s_partition_type;

	if (ptype == UDF_METADATA_MAP25) {
		part = sbi->s_partmaps[part].s_type_specific.s_metadata.
							s_phys_partition_ref;
	} else if (ptype == UDF_VIRTUAL_MAP15 || ptype == UDF_VIRTUAL_MAP20) {
		/*
		 * Filesystems with VAT are append-only and we cannot write to
 		 * them. Let's just report 0 here.
		 */
		return 0;
	}

	if (sbi->s_lvid_bh) {
		struct logicalVolIntegrityDesc *lvid =
			(struct logicalVolIntegrityDesc *)
			sbi->s_lvid_bh->b_data;
		if (le32_to_cpu(lvid->numOfPartitions) > part) {
			accum = le32_to_cpu(
					lvid->freeSpaceTable[part]);
			if (accum == 0xFFFFFFFF)
				accum = 0;
		}
	}

	if (accum)
		return accum;

	map = &sbi->s_partmaps[part];
	if (map->s_partition_flags & UDF_PART_FLAG_UNALLOC_BITMAP) {
		accum += udf_count_free_bitmap(sb,
					       map->s_uspace.s_bitmap);
	}
	if (accum)
		return accum;

	if (map->s_partition_flags & UDF_PART_FLAG_UNALLOC_TABLE) {
		accum += udf_count_free_table(sb,
					      map->s_uspace.s_table);
	}
	return accum;
}

MODULE_AUTHOR("Ben Fennema");
MODULE_DESCRIPTION("Universal Disk Format Filesystem");
MODULE_LICENSE("GPL");
module_init(init_udf_fs)
module_exit(exit_udf_fs)
