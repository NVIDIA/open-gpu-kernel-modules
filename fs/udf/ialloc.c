/*
 * ialloc.c
 *
 * PURPOSE
 *	Inode allocation handling routines for the OSTA-UDF(tm) filesystem.
 *
 * COPYRIGHT
 *	This file is distributed under the terms of the GNU General Public
 *	License (GPL). Copies of the GPL can be obtained from:
 *		ftp://prep.ai.mit.edu/pub/gnu/GPL
 *	Each contributing author retains all rights to their own work.
 *
 *  (C) 1998-2001 Ben Fennema
 *
 * HISTORY
 *
 *  02/24/99 blf  Created.
 *
 */

#include "udfdecl.h"
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/slab.h>

#include "udf_i.h"
#include "udf_sb.h"

void udf_free_inode(struct inode *inode)
{
	struct super_block *sb = inode->i_sb;
	struct udf_sb_info *sbi = UDF_SB(sb);
	struct logicalVolIntegrityDescImpUse *lvidiu = udf_sb_lvidiu(sb);

	if (lvidiu) {
		mutex_lock(&sbi->s_alloc_mutex);
		if (S_ISDIR(inode->i_mode))
			le32_add_cpu(&lvidiu->numDirs, -1);
		else
			le32_add_cpu(&lvidiu->numFiles, -1);
		udf_updated_lvid(sb);
		mutex_unlock(&sbi->s_alloc_mutex);
	}

	udf_free_blocks(sb, NULL, &UDF_I(inode)->i_location, 0, 1);
}

struct inode *udf_new_inode(struct inode *dir, umode_t mode)
{
	struct super_block *sb = dir->i_sb;
	struct udf_sb_info *sbi = UDF_SB(sb);
	struct inode *inode;
	udf_pblk_t block;
	uint32_t start = UDF_I(dir)->i_location.logicalBlockNum;
	struct udf_inode_info *iinfo;
	struct udf_inode_info *dinfo = UDF_I(dir);
	struct logicalVolIntegrityDescImpUse *lvidiu;
	int err;

	inode = new_inode(sb);

	if (!inode)
		return ERR_PTR(-ENOMEM);

	iinfo = UDF_I(inode);
	if (UDF_QUERY_FLAG(inode->i_sb, UDF_FLAG_USE_EXTENDED_FE)) {
		iinfo->i_efe = 1;
		if (UDF_VERS_USE_EXTENDED_FE > sbi->s_udfrev)
			sbi->s_udfrev = UDF_VERS_USE_EXTENDED_FE;
		iinfo->i_data = kzalloc(inode->i_sb->s_blocksize -
					sizeof(struct extendedFileEntry),
					GFP_KERNEL);
	} else {
		iinfo->i_efe = 0;
		iinfo->i_data = kzalloc(inode->i_sb->s_blocksize -
					sizeof(struct fileEntry),
					GFP_KERNEL);
	}
	if (!iinfo->i_data) {
		iput(inode);
		return ERR_PTR(-ENOMEM);
	}

	err = -ENOSPC;
	block = udf_new_block(dir->i_sb, NULL,
			      dinfo->i_location.partitionReferenceNum,
			      start, &err);
	if (err) {
		iput(inode);
		return ERR_PTR(err);
	}

	lvidiu = udf_sb_lvidiu(sb);
	if (lvidiu) {
		iinfo->i_unique = lvid_get_unique_id(sb);
		inode->i_generation = iinfo->i_unique;
		mutex_lock(&sbi->s_alloc_mutex);
		if (S_ISDIR(mode))
			le32_add_cpu(&lvidiu->numDirs, 1);
		else
			le32_add_cpu(&lvidiu->numFiles, 1);
		udf_updated_lvid(sb);
		mutex_unlock(&sbi->s_alloc_mutex);
	}

	inode_init_owner(&init_user_ns, inode, dir, mode);
	if (UDF_QUERY_FLAG(sb, UDF_FLAG_UID_SET))
		inode->i_uid = sbi->s_uid;
	if (UDF_QUERY_FLAG(sb, UDF_FLAG_GID_SET))
		inode->i_gid = sbi->s_gid;

	iinfo->i_location.logicalBlockNum = block;
	iinfo->i_location.partitionReferenceNum =
				dinfo->i_location.partitionReferenceNum;
	inode->i_ino = udf_get_lb_pblock(sb, &iinfo->i_location, 0);
	inode->i_blocks = 0;
	iinfo->i_lenEAttr = 0;
	iinfo->i_lenAlloc = 0;
	iinfo->i_use = 0;
	iinfo->i_checkpoint = 1;
	iinfo->i_extraPerms = FE_PERM_U_CHATTR;
	udf_update_extra_perms(inode, mode);

	if (UDF_QUERY_FLAG(inode->i_sb, UDF_FLAG_USE_AD_IN_ICB))
		iinfo->i_alloc_type = ICBTAG_FLAG_AD_IN_ICB;
	else if (UDF_QUERY_FLAG(inode->i_sb, UDF_FLAG_USE_SHORT_AD))
		iinfo->i_alloc_type = ICBTAG_FLAG_AD_SHORT;
	else
		iinfo->i_alloc_type = ICBTAG_FLAG_AD_LONG;
	inode->i_mtime = inode->i_atime = inode->i_ctime = current_time(inode);
	iinfo->i_crtime = inode->i_mtime;
	if (unlikely(insert_inode_locked(inode) < 0)) {
		make_bad_inode(inode);
		iput(inode);
		return ERR_PTR(-EIO);
	}
	mark_inode_dirty(inode);

	return inode;
}
