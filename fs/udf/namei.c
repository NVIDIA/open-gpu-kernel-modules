/*
 * namei.c
 *
 * PURPOSE
 *      Inode name handling routines for the OSTA-UDF(tm) filesystem.
 *
 * COPYRIGHT
 *      This file is distributed under the terms of the GNU General Public
 *      License (GPL). Copies of the GPL can be obtained from:
 *              ftp://prep.ai.mit.edu/pub/gnu/GPL
 *      Each contributing author retains all rights to their own work.
 *
 *  (C) 1998-2004 Ben Fennema
 *  (C) 1999-2000 Stelias Computing Inc
 *
 * HISTORY
 *
 *  12/12/98 blf  Created. Split out the lookup code from dir.c
 *  04/19/99 blf  link, mknod, symlink support
 */

#include "udfdecl.h"

#include "udf_i.h"
#include "udf_sb.h"
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/crc-itu-t.h>
#include <linux/exportfs.h>

static inline int udf_match(int len1, const unsigned char *name1, int len2,
			    const unsigned char *name2)
{
	if (len1 != len2)
		return 0;

	return !memcmp(name1, name2, len1);
}

int udf_write_fi(struct inode *inode, struct fileIdentDesc *cfi,
		 struct fileIdentDesc *sfi, struct udf_fileident_bh *fibh,
		 uint8_t *impuse, uint8_t *fileident)
{
	uint16_t crclen = fibh->eoffset - fibh->soffset - sizeof(struct tag);
	uint16_t crc;
	int offset;
	uint16_t liu = le16_to_cpu(cfi->lengthOfImpUse);
	uint8_t lfi = cfi->lengthFileIdent;
	int padlen = fibh->eoffset - fibh->soffset - liu - lfi -
		sizeof(struct fileIdentDesc);
	int adinicb = 0;

	if (UDF_I(inode)->i_alloc_type == ICBTAG_FLAG_AD_IN_ICB)
		adinicb = 1;

	offset = fibh->soffset + sizeof(struct fileIdentDesc);

	if (impuse) {
		if (adinicb || (offset + liu < 0)) {
			memcpy((uint8_t *)sfi->impUse, impuse, liu);
		} else if (offset >= 0) {
			memcpy(fibh->ebh->b_data + offset, impuse, liu);
		} else {
			memcpy((uint8_t *)sfi->impUse, impuse, -offset);
			memcpy(fibh->ebh->b_data, impuse - offset,
				liu + offset);
		}
	}

	offset += liu;

	if (fileident) {
		if (adinicb || (offset + lfi < 0)) {
			memcpy((uint8_t *)sfi->fileIdent + liu, fileident, lfi);
		} else if (offset >= 0) {
			memcpy(fibh->ebh->b_data + offset, fileident, lfi);
		} else {
			memcpy((uint8_t *)sfi->fileIdent + liu, fileident,
				-offset);
			memcpy(fibh->ebh->b_data, fileident - offset,
				lfi + offset);
		}
	}

	offset += lfi;

	if (adinicb || (offset + padlen < 0)) {
		memset((uint8_t *)sfi->padding + liu + lfi, 0x00, padlen);
	} else if (offset >= 0) {
		memset(fibh->ebh->b_data + offset, 0x00, padlen);
	} else {
		memset((uint8_t *)sfi->padding + liu + lfi, 0x00, -offset);
		memset(fibh->ebh->b_data, 0x00, padlen + offset);
	}

	crc = crc_itu_t(0, (uint8_t *)cfi + sizeof(struct tag),
		      sizeof(struct fileIdentDesc) - sizeof(struct tag));

	if (fibh->sbh == fibh->ebh) {
		crc = crc_itu_t(crc, (uint8_t *)sfi->impUse,
			      crclen + sizeof(struct tag) -
			      sizeof(struct fileIdentDesc));
	} else if (sizeof(struct fileIdentDesc) >= -fibh->soffset) {
		crc = crc_itu_t(crc, fibh->ebh->b_data +
					sizeof(struct fileIdentDesc) +
					fibh->soffset,
			      crclen + sizeof(struct tag) -
					sizeof(struct fileIdentDesc));
	} else {
		crc = crc_itu_t(crc, (uint8_t *)sfi->impUse,
			      -fibh->soffset - sizeof(struct fileIdentDesc));
		crc = crc_itu_t(crc, fibh->ebh->b_data, fibh->eoffset);
	}

	cfi->descTag.descCRC = cpu_to_le16(crc);
	cfi->descTag.descCRCLength = cpu_to_le16(crclen);
	cfi->descTag.tagChecksum = udf_tag_checksum(&cfi->descTag);

	if (adinicb || (sizeof(struct fileIdentDesc) <= -fibh->soffset)) {
		memcpy((uint8_t *)sfi, (uint8_t *)cfi,
			sizeof(struct fileIdentDesc));
	} else {
		memcpy((uint8_t *)sfi, (uint8_t *)cfi, -fibh->soffset);
		memcpy(fibh->ebh->b_data, (uint8_t *)cfi - fibh->soffset,
		       sizeof(struct fileIdentDesc) + fibh->soffset);
	}

	if (adinicb) {
		mark_inode_dirty(inode);
	} else {
		if (fibh->sbh != fibh->ebh)
			mark_buffer_dirty_inode(fibh->ebh, inode);
		mark_buffer_dirty_inode(fibh->sbh, inode);
	}
	return 0;
}

/**
 * udf_find_entry - find entry in given directory.
 *
 * @dir:	directory inode to search in
 * @child:	qstr of the name
 * @fibh:	buffer head / inode with file identifier descriptor we found
 * @cfi:	found file identifier descriptor with given name
 *
 * This function searches in the directory @dir for a file name @child. When
 * found, @fibh points to the buffer head(s) (bh is NULL for in ICB
 * directories) containing the file identifier descriptor (FID). In that case
 * the function returns pointer to the FID in the buffer or inode - but note
 * that FID may be split among two buffers (blocks) so accessing it via that
 * pointer isn't easily possible. This pointer can be used only as an iterator
 * for other directory manipulation functions. For inspection of the FID @cfi
 * can be used - the found FID is copied there.
 *
 * Returns pointer to FID, NULL when nothing found, or error code.
 */
static struct fileIdentDesc *udf_find_entry(struct inode *dir,
					    const struct qstr *child,
					    struct udf_fileident_bh *fibh,
					    struct fileIdentDesc *cfi)
{
	struct fileIdentDesc *fi = NULL;
	loff_t f_pos;
	udf_pblk_t block;
	int flen;
	unsigned char *fname = NULL, *copy_name = NULL;
	unsigned char *nameptr;
	uint8_t lfi;
	uint16_t liu;
	loff_t size;
	struct kernel_lb_addr eloc;
	uint32_t elen;
	sector_t offset;
	struct extent_position epos = {};
	struct udf_inode_info *dinfo = UDF_I(dir);
	int isdotdot = child->len == 2 &&
		child->name[0] == '.' && child->name[1] == '.';
	struct super_block *sb = dir->i_sb;

	size = udf_ext0_offset(dir) + dir->i_size;
	f_pos = udf_ext0_offset(dir);

	fibh->sbh = fibh->ebh = NULL;
	fibh->soffset = fibh->eoffset = f_pos & (sb->s_blocksize - 1);
	if (dinfo->i_alloc_type != ICBTAG_FLAG_AD_IN_ICB) {
		if (inode_bmap(dir, f_pos >> sb->s_blocksize_bits, &epos,
		    &eloc, &elen, &offset) != (EXT_RECORDED_ALLOCATED >> 30)) {
			fi = ERR_PTR(-EIO);
			goto out_err;
		}

		block = udf_get_lb_pblock(sb, &eloc, offset);
		if ((++offset << sb->s_blocksize_bits) < elen) {
			if (dinfo->i_alloc_type == ICBTAG_FLAG_AD_SHORT)
				epos.offset -= sizeof(struct short_ad);
			else if (dinfo->i_alloc_type == ICBTAG_FLAG_AD_LONG)
				epos.offset -= sizeof(struct long_ad);
		} else
			offset = 0;

		fibh->sbh = fibh->ebh = udf_tread(sb, block);
		if (!fibh->sbh) {
			fi = ERR_PTR(-EIO);
			goto out_err;
		}
	}

	fname = kmalloc(UDF_NAME_LEN, GFP_NOFS);
	if (!fname) {
		fi = ERR_PTR(-ENOMEM);
		goto out_err;
	}

	while (f_pos < size) {
		fi = udf_fileident_read(dir, &f_pos, fibh, cfi, &epos, &eloc,
					&elen, &offset);
		if (!fi) {
			fi = ERR_PTR(-EIO);
			goto out_err;
		}

		liu = le16_to_cpu(cfi->lengthOfImpUse);
		lfi = cfi->lengthFileIdent;

		if (fibh->sbh == fibh->ebh) {
			nameptr = fi->fileIdent + liu;
		} else {
			int poffset;	/* Unpaded ending offset */

			poffset = fibh->soffset + sizeof(struct fileIdentDesc) +
					liu + lfi;

			if (poffset >= lfi)
				nameptr = (uint8_t *)(fibh->ebh->b_data +
						      poffset - lfi);
			else {
				if (!copy_name) {
					copy_name = kmalloc(UDF_NAME_LEN,
							    GFP_NOFS);
					if (!copy_name) {
						fi = ERR_PTR(-ENOMEM);
						goto out_err;
					}
				}
				nameptr = copy_name;
				memcpy(nameptr, fi->fileIdent + liu,
					lfi - poffset);
				memcpy(nameptr + lfi - poffset,
					fibh->ebh->b_data, poffset);
			}
		}

		if ((cfi->fileCharacteristics & FID_FILE_CHAR_DELETED) != 0) {
			if (!UDF_QUERY_FLAG(sb, UDF_FLAG_UNDELETE))
				continue;
		}

		if ((cfi->fileCharacteristics & FID_FILE_CHAR_HIDDEN) != 0) {
			if (!UDF_QUERY_FLAG(sb, UDF_FLAG_UNHIDE))
				continue;
		}

		if ((cfi->fileCharacteristics & FID_FILE_CHAR_PARENT) &&
		    isdotdot)
			goto out_ok;

		if (!lfi)
			continue;

		flen = udf_get_filename(sb, nameptr, lfi, fname, UDF_NAME_LEN);
		if (flen < 0) {
			fi = ERR_PTR(flen);
			goto out_err;
		}

		if (udf_match(flen, fname, child->len, child->name))
			goto out_ok;
	}

	fi = NULL;
out_err:
	if (fibh->sbh != fibh->ebh)
		brelse(fibh->ebh);
	brelse(fibh->sbh);
out_ok:
	brelse(epos.bh);
	kfree(fname);
	kfree(copy_name);

	return fi;
}

static struct dentry *udf_lookup(struct inode *dir, struct dentry *dentry,
				 unsigned int flags)
{
	struct inode *inode = NULL;
	struct fileIdentDesc cfi;
	struct udf_fileident_bh fibh;
	struct fileIdentDesc *fi;

	if (dentry->d_name.len > UDF_NAME_LEN)
		return ERR_PTR(-ENAMETOOLONG);

	fi = udf_find_entry(dir, &dentry->d_name, &fibh, &cfi);
	if (IS_ERR(fi))
		return ERR_CAST(fi);

	if (fi) {
		struct kernel_lb_addr loc;

		if (fibh.sbh != fibh.ebh)
			brelse(fibh.ebh);
		brelse(fibh.sbh);

		loc = lelb_to_cpu(cfi.icb.extLocation);
		inode = udf_iget(dir->i_sb, &loc);
		if (IS_ERR(inode))
			return ERR_CAST(inode);
	}

	return d_splice_alias(inode, dentry);
}

static struct fileIdentDesc *udf_add_entry(struct inode *dir,
					   struct dentry *dentry,
					   struct udf_fileident_bh *fibh,
					   struct fileIdentDesc *cfi, int *err)
{
	struct super_block *sb = dir->i_sb;
	struct fileIdentDesc *fi = NULL;
	unsigned char *name = NULL;
	int namelen;
	loff_t f_pos;
	loff_t size = udf_ext0_offset(dir) + dir->i_size;
	int nfidlen;
	udf_pblk_t block;
	struct kernel_lb_addr eloc;
	uint32_t elen = 0;
	sector_t offset;
	struct extent_position epos = {};
	struct udf_inode_info *dinfo;

	fibh->sbh = fibh->ebh = NULL;
	name = kmalloc(UDF_NAME_LEN_CS0, GFP_NOFS);
	if (!name) {
		*err = -ENOMEM;
		goto out_err;
	}

	if (dentry) {
		if (!dentry->d_name.len) {
			*err = -EINVAL;
			goto out_err;
		}
		namelen = udf_put_filename(sb, dentry->d_name.name,
					   dentry->d_name.len,
					   name, UDF_NAME_LEN_CS0);
		if (!namelen) {
			*err = -ENAMETOOLONG;
			goto out_err;
		}
	} else {
		namelen = 0;
	}

	nfidlen = ALIGN(sizeof(struct fileIdentDesc) + namelen, UDF_NAME_PAD);

	f_pos = udf_ext0_offset(dir);

	fibh->soffset = fibh->eoffset = f_pos & (dir->i_sb->s_blocksize - 1);
	dinfo = UDF_I(dir);
	if (dinfo->i_alloc_type != ICBTAG_FLAG_AD_IN_ICB) {
		if (inode_bmap(dir, f_pos >> dir->i_sb->s_blocksize_bits, &epos,
		    &eloc, &elen, &offset) != (EXT_RECORDED_ALLOCATED >> 30)) {
			block = udf_get_lb_pblock(dir->i_sb,
					&dinfo->i_location, 0);
			fibh->soffset = fibh->eoffset = sb->s_blocksize;
			goto add;
		}
		block = udf_get_lb_pblock(dir->i_sb, &eloc, offset);
		if ((++offset << dir->i_sb->s_blocksize_bits) < elen) {
			if (dinfo->i_alloc_type == ICBTAG_FLAG_AD_SHORT)
				epos.offset -= sizeof(struct short_ad);
			else if (dinfo->i_alloc_type == ICBTAG_FLAG_AD_LONG)
				epos.offset -= sizeof(struct long_ad);
		} else
			offset = 0;

		fibh->sbh = fibh->ebh = udf_tread(dir->i_sb, block);
		if (!fibh->sbh) {
			*err = -EIO;
			goto out_err;
		}

		block = dinfo->i_location.logicalBlockNum;
	}

	while (f_pos < size) {
		fi = udf_fileident_read(dir, &f_pos, fibh, cfi, &epos, &eloc,
					&elen, &offset);

		if (!fi) {
			*err = -EIO;
			goto out_err;
		}

		if ((cfi->fileCharacteristics & FID_FILE_CHAR_DELETED) != 0) {
			if (udf_dir_entry_len(cfi) == nfidlen) {
				cfi->descTag.tagSerialNum = cpu_to_le16(1);
				cfi->fileVersionNum = cpu_to_le16(1);
				cfi->fileCharacteristics = 0;
				cfi->lengthFileIdent = namelen;
				cfi->lengthOfImpUse = cpu_to_le16(0);
				if (!udf_write_fi(dir, cfi, fi, fibh, NULL,
						  name))
					goto out_ok;
				else {
					*err = -EIO;
					goto out_err;
				}
			}
		}
	}

add:
	f_pos += nfidlen;

	if (dinfo->i_alloc_type == ICBTAG_FLAG_AD_IN_ICB &&
	    sb->s_blocksize - fibh->eoffset < nfidlen) {
		brelse(epos.bh);
		epos.bh = NULL;
		fibh->soffset -= udf_ext0_offset(dir);
		fibh->eoffset -= udf_ext0_offset(dir);
		f_pos -= udf_ext0_offset(dir);
		if (fibh->sbh != fibh->ebh)
			brelse(fibh->ebh);
		brelse(fibh->sbh);
		fibh->sbh = fibh->ebh =
				udf_expand_dir_adinicb(dir, &block, err);
		if (!fibh->sbh)
			goto out_err;
		epos.block = dinfo->i_location;
		epos.offset = udf_file_entry_alloc_offset(dir);
		/* Load extent udf_expand_dir_adinicb() has created */
		udf_current_aext(dir, &epos, &eloc, &elen, 1);
	}

	/* Entry fits into current block? */
	if (sb->s_blocksize - fibh->eoffset >= nfidlen) {
		fibh->soffset = fibh->eoffset;
		fibh->eoffset += nfidlen;
		if (fibh->sbh != fibh->ebh) {
			brelse(fibh->sbh);
			fibh->sbh = fibh->ebh;
		}

		if (dinfo->i_alloc_type == ICBTAG_FLAG_AD_IN_ICB) {
			block = dinfo->i_location.logicalBlockNum;
			fi = (struct fileIdentDesc *)
					(dinfo->i_data + fibh->soffset -
					 udf_ext0_offset(dir) +
					 dinfo->i_lenEAttr);
		} else {
			block = eloc.logicalBlockNum +
					((elen - 1) >>
						dir->i_sb->s_blocksize_bits);
			fi = (struct fileIdentDesc *)
				(fibh->sbh->b_data + fibh->soffset);
		}
	} else {
		/* Round up last extent in the file */
		elen = (elen + sb->s_blocksize - 1) & ~(sb->s_blocksize - 1);
		if (dinfo->i_alloc_type == ICBTAG_FLAG_AD_SHORT)
			epos.offset -= sizeof(struct short_ad);
		else if (dinfo->i_alloc_type == ICBTAG_FLAG_AD_LONG)
			epos.offset -= sizeof(struct long_ad);
		udf_write_aext(dir, &epos, &eloc, elen, 1);
		dinfo->i_lenExtents = (dinfo->i_lenExtents + sb->s_blocksize
					- 1) & ~(sb->s_blocksize - 1);

		fibh->soffset = fibh->eoffset - sb->s_blocksize;
		fibh->eoffset += nfidlen - sb->s_blocksize;
		if (fibh->sbh != fibh->ebh) {
			brelse(fibh->sbh);
			fibh->sbh = fibh->ebh;
		}

		block = eloc.logicalBlockNum + ((elen - 1) >>
						dir->i_sb->s_blocksize_bits);
		fibh->ebh = udf_bread(dir,
				f_pos >> dir->i_sb->s_blocksize_bits, 1, err);
		if (!fibh->ebh)
			goto out_err;
		/* Extents could have been merged, invalidate our position */
		brelse(epos.bh);
		epos.bh = NULL;
		epos.block = dinfo->i_location;
		epos.offset = udf_file_entry_alloc_offset(dir);

		if (!fibh->soffset) {
			/* Find the freshly allocated block */
			while (udf_next_aext(dir, &epos, &eloc, &elen, 1) ==
				(EXT_RECORDED_ALLOCATED >> 30))
				;
			block = eloc.logicalBlockNum + ((elen - 1) >>
					dir->i_sb->s_blocksize_bits);
			brelse(fibh->sbh);
			fibh->sbh = fibh->ebh;
			fi = (struct fileIdentDesc *)(fibh->sbh->b_data);
		} else {
			fi = (struct fileIdentDesc *)
				(fibh->sbh->b_data + sb->s_blocksize +
					fibh->soffset);
		}
	}

	memset(cfi, 0, sizeof(struct fileIdentDesc));
	if (UDF_SB(sb)->s_udfrev >= 0x0200)
		udf_new_tag((char *)cfi, TAG_IDENT_FID, 3, 1, block,
			    sizeof(struct tag));
	else
		udf_new_tag((char *)cfi, TAG_IDENT_FID, 2, 1, block,
			    sizeof(struct tag));
	cfi->fileVersionNum = cpu_to_le16(1);
	cfi->lengthFileIdent = namelen;
	cfi->lengthOfImpUse = cpu_to_le16(0);
	if (!udf_write_fi(dir, cfi, fi, fibh, NULL, name)) {
		dir->i_size += nfidlen;
		if (dinfo->i_alloc_type == ICBTAG_FLAG_AD_IN_ICB)
			dinfo->i_lenAlloc += nfidlen;
		else {
			/* Find the last extent and truncate it to proper size */
			while (udf_next_aext(dir, &epos, &eloc, &elen, 1) ==
				(EXT_RECORDED_ALLOCATED >> 30))
				;
			elen -= dinfo->i_lenExtents - dir->i_size;
			if (dinfo->i_alloc_type == ICBTAG_FLAG_AD_SHORT)
				epos.offset -= sizeof(struct short_ad);
			else if (dinfo->i_alloc_type == ICBTAG_FLAG_AD_LONG)
				epos.offset -= sizeof(struct long_ad);
			udf_write_aext(dir, &epos, &eloc, elen, 1);
			dinfo->i_lenExtents = dir->i_size;
		}

		mark_inode_dirty(dir);
		goto out_ok;
	} else {
		*err = -EIO;
		goto out_err;
	}

out_err:
	fi = NULL;
	if (fibh->sbh != fibh->ebh)
		brelse(fibh->ebh);
	brelse(fibh->sbh);
out_ok:
	brelse(epos.bh);
	kfree(name);
	return fi;
}

static int udf_delete_entry(struct inode *inode, struct fileIdentDesc *fi,
			    struct udf_fileident_bh *fibh,
			    struct fileIdentDesc *cfi)
{
	cfi->fileCharacteristics |= FID_FILE_CHAR_DELETED;

	if (UDF_QUERY_FLAG(inode->i_sb, UDF_FLAG_STRICT))
		memset(&(cfi->icb), 0x00, sizeof(struct long_ad));

	return udf_write_fi(inode, cfi, fi, fibh, NULL, NULL);
}

static int udf_add_nondir(struct dentry *dentry, struct inode *inode)
{
	struct udf_inode_info *iinfo = UDF_I(inode);
	struct inode *dir = d_inode(dentry->d_parent);
	struct udf_fileident_bh fibh;
	struct fileIdentDesc cfi, *fi;
	int err;

	fi = udf_add_entry(dir, dentry, &fibh, &cfi, &err);
	if (unlikely(!fi)) {
		inode_dec_link_count(inode);
		discard_new_inode(inode);
		return err;
	}
	cfi.icb.extLength = cpu_to_le32(inode->i_sb->s_blocksize);
	cfi.icb.extLocation = cpu_to_lelb(iinfo->i_location);
	*(__le32 *)((struct allocDescImpUse *)cfi.icb.impUse)->impUse =
		cpu_to_le32(iinfo->i_unique & 0x00000000FFFFFFFFUL);
	udf_write_fi(dir, &cfi, fi, &fibh, NULL, NULL);
	dir->i_ctime = dir->i_mtime = current_time(dir);
	mark_inode_dirty(dir);
	if (fibh.sbh != fibh.ebh)
		brelse(fibh.ebh);
	brelse(fibh.sbh);
	d_instantiate_new(dentry, inode);

	return 0;
}

static int udf_create(struct user_namespace *mnt_userns, struct inode *dir,
		      struct dentry *dentry, umode_t mode, bool excl)
{
	struct inode *inode = udf_new_inode(dir, mode);

	if (IS_ERR(inode))
		return PTR_ERR(inode);

	if (UDF_I(inode)->i_alloc_type == ICBTAG_FLAG_AD_IN_ICB)
		inode->i_data.a_ops = &udf_adinicb_aops;
	else
		inode->i_data.a_ops = &udf_aops;
	inode->i_op = &udf_file_inode_operations;
	inode->i_fop = &udf_file_operations;
	mark_inode_dirty(inode);

	return udf_add_nondir(dentry, inode);
}

static int udf_tmpfile(struct user_namespace *mnt_userns, struct inode *dir,
		       struct dentry *dentry, umode_t mode)
{
	struct inode *inode = udf_new_inode(dir, mode);

	if (IS_ERR(inode))
		return PTR_ERR(inode);

	if (UDF_I(inode)->i_alloc_type == ICBTAG_FLAG_AD_IN_ICB)
		inode->i_data.a_ops = &udf_adinicb_aops;
	else
		inode->i_data.a_ops = &udf_aops;
	inode->i_op = &udf_file_inode_operations;
	inode->i_fop = &udf_file_operations;
	mark_inode_dirty(inode);
	d_tmpfile(dentry, inode);
	unlock_new_inode(inode);
	return 0;
}

static int udf_mknod(struct user_namespace *mnt_userns, struct inode *dir,
		     struct dentry *dentry, umode_t mode, dev_t rdev)
{
	struct inode *inode;

	if (!old_valid_dev(rdev))
		return -EINVAL;

	inode = udf_new_inode(dir, mode);
	if (IS_ERR(inode))
		return PTR_ERR(inode);

	init_special_inode(inode, mode, rdev);
	return udf_add_nondir(dentry, inode);
}

static int udf_mkdir(struct user_namespace *mnt_userns, struct inode *dir,
		     struct dentry *dentry, umode_t mode)
{
	struct inode *inode;
	struct udf_fileident_bh fibh;
	struct fileIdentDesc cfi, *fi;
	int err;
	struct udf_inode_info *dinfo = UDF_I(dir);
	struct udf_inode_info *iinfo;

	inode = udf_new_inode(dir, S_IFDIR | mode);
	if (IS_ERR(inode))
		return PTR_ERR(inode);

	iinfo = UDF_I(inode);
	inode->i_op = &udf_dir_inode_operations;
	inode->i_fop = &udf_dir_operations;
	fi = udf_add_entry(inode, NULL, &fibh, &cfi, &err);
	if (!fi) {
		inode_dec_link_count(inode);
		discard_new_inode(inode);
		goto out;
	}
	set_nlink(inode, 2);
	cfi.icb.extLength = cpu_to_le32(inode->i_sb->s_blocksize);
	cfi.icb.extLocation = cpu_to_lelb(dinfo->i_location);
	*(__le32 *)((struct allocDescImpUse *)cfi.icb.impUse)->impUse =
		cpu_to_le32(dinfo->i_unique & 0x00000000FFFFFFFFUL);
	cfi.fileCharacteristics =
			FID_FILE_CHAR_DIRECTORY | FID_FILE_CHAR_PARENT;
	udf_write_fi(inode, &cfi, fi, &fibh, NULL, NULL);
	brelse(fibh.sbh);
	mark_inode_dirty(inode);

	fi = udf_add_entry(dir, dentry, &fibh, &cfi, &err);
	if (!fi) {
		clear_nlink(inode);
		mark_inode_dirty(inode);
		discard_new_inode(inode);
		goto out;
	}
	cfi.icb.extLength = cpu_to_le32(inode->i_sb->s_blocksize);
	cfi.icb.extLocation = cpu_to_lelb(iinfo->i_location);
	*(__le32 *)((struct allocDescImpUse *)cfi.icb.impUse)->impUse =
		cpu_to_le32(iinfo->i_unique & 0x00000000FFFFFFFFUL);
	cfi.fileCharacteristics |= FID_FILE_CHAR_DIRECTORY;
	udf_write_fi(dir, &cfi, fi, &fibh, NULL, NULL);
	inc_nlink(dir);
	dir->i_ctime = dir->i_mtime = current_time(dir);
	mark_inode_dirty(dir);
	d_instantiate_new(dentry, inode);
	if (fibh.sbh != fibh.ebh)
		brelse(fibh.ebh);
	brelse(fibh.sbh);
	err = 0;

out:
	return err;
}

static int empty_dir(struct inode *dir)
{
	struct fileIdentDesc *fi, cfi;
	struct udf_fileident_bh fibh;
	loff_t f_pos;
	loff_t size = udf_ext0_offset(dir) + dir->i_size;
	udf_pblk_t block;
	struct kernel_lb_addr eloc;
	uint32_t elen;
	sector_t offset;
	struct extent_position epos = {};
	struct udf_inode_info *dinfo = UDF_I(dir);

	f_pos = udf_ext0_offset(dir);
	fibh.soffset = fibh.eoffset = f_pos & (dir->i_sb->s_blocksize - 1);

	if (dinfo->i_alloc_type == ICBTAG_FLAG_AD_IN_ICB)
		fibh.sbh = fibh.ebh = NULL;
	else if (inode_bmap(dir, f_pos >> dir->i_sb->s_blocksize_bits,
			      &epos, &eloc, &elen, &offset) ==
					(EXT_RECORDED_ALLOCATED >> 30)) {
		block = udf_get_lb_pblock(dir->i_sb, &eloc, offset);
		if ((++offset << dir->i_sb->s_blocksize_bits) < elen) {
			if (dinfo->i_alloc_type == ICBTAG_FLAG_AD_SHORT)
				epos.offset -= sizeof(struct short_ad);
			else if (dinfo->i_alloc_type == ICBTAG_FLAG_AD_LONG)
				epos.offset -= sizeof(struct long_ad);
		} else
			offset = 0;

		fibh.sbh = fibh.ebh = udf_tread(dir->i_sb, block);
		if (!fibh.sbh) {
			brelse(epos.bh);
			return 0;
		}
	} else {
		brelse(epos.bh);
		return 0;
	}

	while (f_pos < size) {
		fi = udf_fileident_read(dir, &f_pos, &fibh, &cfi, &epos, &eloc,
					&elen, &offset);
		if (!fi) {
			if (fibh.sbh != fibh.ebh)
				brelse(fibh.ebh);
			brelse(fibh.sbh);
			brelse(epos.bh);
			return 0;
		}

		if (cfi.lengthFileIdent &&
		    (cfi.fileCharacteristics & FID_FILE_CHAR_DELETED) == 0) {
			if (fibh.sbh != fibh.ebh)
				brelse(fibh.ebh);
			brelse(fibh.sbh);
			brelse(epos.bh);
			return 0;
		}
	}

	if (fibh.sbh != fibh.ebh)
		brelse(fibh.ebh);
	brelse(fibh.sbh);
	brelse(epos.bh);

	return 1;
}

static int udf_rmdir(struct inode *dir, struct dentry *dentry)
{
	int retval;
	struct inode *inode = d_inode(dentry);
	struct udf_fileident_bh fibh;
	struct fileIdentDesc *fi, cfi;
	struct kernel_lb_addr tloc;

	retval = -ENOENT;
	fi = udf_find_entry(dir, &dentry->d_name, &fibh, &cfi);
	if (IS_ERR_OR_NULL(fi)) {
		if (fi)
			retval = PTR_ERR(fi);
		goto out;
	}

	retval = -EIO;
	tloc = lelb_to_cpu(cfi.icb.extLocation);
	if (udf_get_lb_pblock(dir->i_sb, &tloc, 0) != inode->i_ino)
		goto end_rmdir;
	retval = -ENOTEMPTY;
	if (!empty_dir(inode))
		goto end_rmdir;
	retval = udf_delete_entry(dir, fi, &fibh, &cfi);
	if (retval)
		goto end_rmdir;
	if (inode->i_nlink != 2)
		udf_warn(inode->i_sb, "empty directory has nlink != 2 (%u)\n",
			 inode->i_nlink);
	clear_nlink(inode);
	inode->i_size = 0;
	inode_dec_link_count(dir);
	inode->i_ctime = dir->i_ctime = dir->i_mtime =
						current_time(inode);
	mark_inode_dirty(dir);

end_rmdir:
	if (fibh.sbh != fibh.ebh)
		brelse(fibh.ebh);
	brelse(fibh.sbh);

out:
	return retval;
}

static int udf_unlink(struct inode *dir, struct dentry *dentry)
{
	int retval;
	struct inode *inode = d_inode(dentry);
	struct udf_fileident_bh fibh;
	struct fileIdentDesc *fi;
	struct fileIdentDesc cfi;
	struct kernel_lb_addr tloc;

	retval = -ENOENT;
	fi = udf_find_entry(dir, &dentry->d_name, &fibh, &cfi);

	if (IS_ERR_OR_NULL(fi)) {
		if (fi)
			retval = PTR_ERR(fi);
		goto out;
	}

	retval = -EIO;
	tloc = lelb_to_cpu(cfi.icb.extLocation);
	if (udf_get_lb_pblock(dir->i_sb, &tloc, 0) != inode->i_ino)
		goto end_unlink;

	if (!inode->i_nlink) {
		udf_debug("Deleting nonexistent file (%lu), %u\n",
			  inode->i_ino, inode->i_nlink);
		set_nlink(inode, 1);
	}
	retval = udf_delete_entry(dir, fi, &fibh, &cfi);
	if (retval)
		goto end_unlink;
	dir->i_ctime = dir->i_mtime = current_time(dir);
	mark_inode_dirty(dir);
	inode_dec_link_count(inode);
	inode->i_ctime = dir->i_ctime;
	retval = 0;

end_unlink:
	if (fibh.sbh != fibh.ebh)
		brelse(fibh.ebh);
	brelse(fibh.sbh);

out:
	return retval;
}

static int udf_symlink(struct user_namespace *mnt_userns, struct inode *dir,
		       struct dentry *dentry, const char *symname)
{
	struct inode *inode = udf_new_inode(dir, S_IFLNK | 0777);
	struct pathComponent *pc;
	const char *compstart;
	struct extent_position epos = {};
	int eoffset, elen = 0;
	uint8_t *ea;
	int err;
	udf_pblk_t block;
	unsigned char *name = NULL;
	int namelen;
	struct udf_inode_info *iinfo;
	struct super_block *sb = dir->i_sb;

	if (IS_ERR(inode))
		return PTR_ERR(inode);

	iinfo = UDF_I(inode);
	down_write(&iinfo->i_data_sem);
	name = kmalloc(UDF_NAME_LEN_CS0, GFP_NOFS);
	if (!name) {
		err = -ENOMEM;
		goto out_no_entry;
	}

	inode->i_data.a_ops = &udf_symlink_aops;
	inode->i_op = &udf_symlink_inode_operations;
	inode_nohighmem(inode);

	if (iinfo->i_alloc_type != ICBTAG_FLAG_AD_IN_ICB) {
		struct kernel_lb_addr eloc;
		uint32_t bsize;

		block = udf_new_block(sb, inode,
				iinfo->i_location.partitionReferenceNum,
				iinfo->i_location.logicalBlockNum, &err);
		if (!block)
			goto out_no_entry;
		epos.block = iinfo->i_location;
		epos.offset = udf_file_entry_alloc_offset(inode);
		epos.bh = NULL;
		eloc.logicalBlockNum = block;
		eloc.partitionReferenceNum =
				iinfo->i_location.partitionReferenceNum;
		bsize = sb->s_blocksize;
		iinfo->i_lenExtents = bsize;
		udf_add_aext(inode, &epos, &eloc, bsize, 0);
		brelse(epos.bh);

		block = udf_get_pblock(sb, block,
				iinfo->i_location.partitionReferenceNum,
				0);
		epos.bh = udf_tgetblk(sb, block);
		lock_buffer(epos.bh);
		memset(epos.bh->b_data, 0x00, bsize);
		set_buffer_uptodate(epos.bh);
		unlock_buffer(epos.bh);
		mark_buffer_dirty_inode(epos.bh, inode);
		ea = epos.bh->b_data + udf_ext0_offset(inode);
	} else
		ea = iinfo->i_data + iinfo->i_lenEAttr;

	eoffset = sb->s_blocksize - udf_ext0_offset(inode);
	pc = (struct pathComponent *)ea;

	if (*symname == '/') {
		do {
			symname++;
		} while (*symname == '/');

		pc->componentType = 1;
		pc->lengthComponentIdent = 0;
		pc->componentFileVersionNum = 0;
		elen += sizeof(struct pathComponent);
	}

	err = -ENAMETOOLONG;

	while (*symname) {
		if (elen + sizeof(struct pathComponent) > eoffset)
			goto out_no_entry;

		pc = (struct pathComponent *)(ea + elen);

		compstart = symname;

		do {
			symname++;
		} while (*symname && *symname != '/');

		pc->componentType = 5;
		pc->lengthComponentIdent = 0;
		pc->componentFileVersionNum = 0;
		if (compstart[0] == '.') {
			if ((symname - compstart) == 1)
				pc->componentType = 4;
			else if ((symname - compstart) == 2 &&
					compstart[1] == '.')
				pc->componentType = 3;
		}

		if (pc->componentType == 5) {
			namelen = udf_put_filename(sb, compstart,
						   symname - compstart,
						   name, UDF_NAME_LEN_CS0);
			if (!namelen)
				goto out_no_entry;

			if (elen + sizeof(struct pathComponent) + namelen >
					eoffset)
				goto out_no_entry;
			else
				pc->lengthComponentIdent = namelen;

			memcpy(pc->componentIdent, name, namelen);
		}

		elen += sizeof(struct pathComponent) + pc->lengthComponentIdent;

		if (*symname) {
			do {
				symname++;
			} while (*symname == '/');
		}
	}

	brelse(epos.bh);
	inode->i_size = elen;
	if (iinfo->i_alloc_type == ICBTAG_FLAG_AD_IN_ICB)
		iinfo->i_lenAlloc = inode->i_size;
	else
		udf_truncate_tail_extent(inode);
	mark_inode_dirty(inode);
	up_write(&iinfo->i_data_sem);

	err = udf_add_nondir(dentry, inode);
out:
	kfree(name);
	return err;

out_no_entry:
	up_write(&iinfo->i_data_sem);
	inode_dec_link_count(inode);
	discard_new_inode(inode);
	goto out;
}

static int udf_link(struct dentry *old_dentry, struct inode *dir,
		    struct dentry *dentry)
{
	struct inode *inode = d_inode(old_dentry);
	struct udf_fileident_bh fibh;
	struct fileIdentDesc cfi, *fi;
	int err;

	fi = udf_add_entry(dir, dentry, &fibh, &cfi, &err);
	if (!fi) {
		return err;
	}
	cfi.icb.extLength = cpu_to_le32(inode->i_sb->s_blocksize);
	cfi.icb.extLocation = cpu_to_lelb(UDF_I(inode)->i_location);
	if (UDF_SB(inode->i_sb)->s_lvid_bh) {
		*(__le32 *)((struct allocDescImpUse *)cfi.icb.impUse)->impUse =
			cpu_to_le32(lvid_get_unique_id(inode->i_sb));
	}
	udf_write_fi(dir, &cfi, fi, &fibh, NULL, NULL);
	if (UDF_I(dir)->i_alloc_type == ICBTAG_FLAG_AD_IN_ICB)
		mark_inode_dirty(dir);

	if (fibh.sbh != fibh.ebh)
		brelse(fibh.ebh);
	brelse(fibh.sbh);
	inc_nlink(inode);
	inode->i_ctime = current_time(inode);
	mark_inode_dirty(inode);
	dir->i_ctime = dir->i_mtime = current_time(dir);
	mark_inode_dirty(dir);
	ihold(inode);
	d_instantiate(dentry, inode);

	return 0;
}

/* Anybody can rename anything with this: the permission checks are left to the
 * higher-level routines.
 */
static int udf_rename(struct user_namespace *mnt_userns, struct inode *old_dir,
		      struct dentry *old_dentry, struct inode *new_dir,
		      struct dentry *new_dentry, unsigned int flags)
{
	struct inode *old_inode = d_inode(old_dentry);
	struct inode *new_inode = d_inode(new_dentry);
	struct udf_fileident_bh ofibh, nfibh;
	struct fileIdentDesc *ofi = NULL, *nfi = NULL, *dir_fi = NULL;
	struct fileIdentDesc ocfi, ncfi;
	struct buffer_head *dir_bh = NULL;
	int retval = -ENOENT;
	struct kernel_lb_addr tloc;
	struct udf_inode_info *old_iinfo = UDF_I(old_inode);

	if (flags & ~RENAME_NOREPLACE)
		return -EINVAL;

	ofi = udf_find_entry(old_dir, &old_dentry->d_name, &ofibh, &ocfi);
	if (IS_ERR(ofi)) {
		retval = PTR_ERR(ofi);
		goto end_rename;
	}

	if (ofibh.sbh != ofibh.ebh)
		brelse(ofibh.ebh);

	brelse(ofibh.sbh);
	tloc = lelb_to_cpu(ocfi.icb.extLocation);
	if (!ofi || udf_get_lb_pblock(old_dir->i_sb, &tloc, 0)
	    != old_inode->i_ino)
		goto end_rename;

	nfi = udf_find_entry(new_dir, &new_dentry->d_name, &nfibh, &ncfi);
	if (IS_ERR(nfi)) {
		retval = PTR_ERR(nfi);
		goto end_rename;
	}
	if (nfi && !new_inode) {
		if (nfibh.sbh != nfibh.ebh)
			brelse(nfibh.ebh);
		brelse(nfibh.sbh);
		nfi = NULL;
	}
	if (S_ISDIR(old_inode->i_mode)) {
		int offset = udf_ext0_offset(old_inode);

		if (new_inode) {
			retval = -ENOTEMPTY;
			if (!empty_dir(new_inode))
				goto end_rename;
		}
		retval = -EIO;
		if (old_iinfo->i_alloc_type == ICBTAG_FLAG_AD_IN_ICB) {
			dir_fi = udf_get_fileident(
					old_iinfo->i_data -
					  (old_iinfo->i_efe ?
					   sizeof(struct extendedFileEntry) :
					   sizeof(struct fileEntry)),
					old_inode->i_sb->s_blocksize, &offset);
		} else {
			dir_bh = udf_bread(old_inode, 0, 0, &retval);
			if (!dir_bh)
				goto end_rename;
			dir_fi = udf_get_fileident(dir_bh->b_data,
					old_inode->i_sb->s_blocksize, &offset);
		}
		if (!dir_fi)
			goto end_rename;
		tloc = lelb_to_cpu(dir_fi->icb.extLocation);
		if (udf_get_lb_pblock(old_inode->i_sb, &tloc, 0) !=
				old_dir->i_ino)
			goto end_rename;
	}
	if (!nfi) {
		nfi = udf_add_entry(new_dir, new_dentry, &nfibh, &ncfi,
				    &retval);
		if (!nfi)
			goto end_rename;
	}

	/*
	 * Like most other Unix systems, set the ctime for inodes on a
	 * rename.
	 */
	old_inode->i_ctime = current_time(old_inode);
	mark_inode_dirty(old_inode);

	/*
	 * ok, that's it
	 */
	ncfi.fileVersionNum = ocfi.fileVersionNum;
	ncfi.fileCharacteristics = ocfi.fileCharacteristics;
	memcpy(&(ncfi.icb), &(ocfi.icb), sizeof(ocfi.icb));
	udf_write_fi(new_dir, &ncfi, nfi, &nfibh, NULL, NULL);

	/* The old fid may have moved - find it again */
	ofi = udf_find_entry(old_dir, &old_dentry->d_name, &ofibh, &ocfi);
	udf_delete_entry(old_dir, ofi, &ofibh, &ocfi);

	if (new_inode) {
		new_inode->i_ctime = current_time(new_inode);
		inode_dec_link_count(new_inode);
	}
	old_dir->i_ctime = old_dir->i_mtime = current_time(old_dir);
	new_dir->i_ctime = new_dir->i_mtime = current_time(new_dir);
	mark_inode_dirty(old_dir);
	mark_inode_dirty(new_dir);

	if (dir_fi) {
		dir_fi->icb.extLocation = cpu_to_lelb(UDF_I(new_dir)->i_location);
		udf_update_tag((char *)dir_fi, udf_dir_entry_len(dir_fi));
		if (old_iinfo->i_alloc_type == ICBTAG_FLAG_AD_IN_ICB)
			mark_inode_dirty(old_inode);
		else
			mark_buffer_dirty_inode(dir_bh, old_inode);

		inode_dec_link_count(old_dir);
		if (new_inode)
			inode_dec_link_count(new_inode);
		else {
			inc_nlink(new_dir);
			mark_inode_dirty(new_dir);
		}
	}

	if (ofi) {
		if (ofibh.sbh != ofibh.ebh)
			brelse(ofibh.ebh);
		brelse(ofibh.sbh);
	}

	retval = 0;

end_rename:
	brelse(dir_bh);
	if (nfi) {
		if (nfibh.sbh != nfibh.ebh)
			brelse(nfibh.ebh);
		brelse(nfibh.sbh);
	}

	return retval;
}

static struct dentry *udf_get_parent(struct dentry *child)
{
	struct kernel_lb_addr tloc;
	struct inode *inode = NULL;
	struct fileIdentDesc cfi;
	struct udf_fileident_bh fibh;

	if (!udf_find_entry(d_inode(child), &dotdot_name, &fibh, &cfi))
		return ERR_PTR(-EACCES);

	if (fibh.sbh != fibh.ebh)
		brelse(fibh.ebh);
	brelse(fibh.sbh);

	tloc = lelb_to_cpu(cfi.icb.extLocation);
	inode = udf_iget(child->d_sb, &tloc);
	if (IS_ERR(inode))
		return ERR_CAST(inode);

	return d_obtain_alias(inode);
}


static struct dentry *udf_nfs_get_inode(struct super_block *sb, u32 block,
					u16 partref, __u32 generation)
{
	struct inode *inode;
	struct kernel_lb_addr loc;

	if (block == 0)
		return ERR_PTR(-ESTALE);

	loc.logicalBlockNum = block;
	loc.partitionReferenceNum = partref;
	inode = udf_iget(sb, &loc);

	if (IS_ERR(inode))
		return ERR_CAST(inode);

	if (generation && inode->i_generation != generation) {
		iput(inode);
		return ERR_PTR(-ESTALE);
	}
	return d_obtain_alias(inode);
}

static struct dentry *udf_fh_to_dentry(struct super_block *sb,
				       struct fid *fid, int fh_len, int fh_type)
{
	if (fh_len < 3 ||
	    (fh_type != FILEID_UDF_WITH_PARENT &&
	     fh_type != FILEID_UDF_WITHOUT_PARENT))
		return NULL;

	return udf_nfs_get_inode(sb, fid->udf.block, fid->udf.partref,
			fid->udf.generation);
}

static struct dentry *udf_fh_to_parent(struct super_block *sb,
				       struct fid *fid, int fh_len, int fh_type)
{
	if (fh_len < 5 || fh_type != FILEID_UDF_WITH_PARENT)
		return NULL;

	return udf_nfs_get_inode(sb, fid->udf.parent_block,
				 fid->udf.parent_partref,
				 fid->udf.parent_generation);
}
static int udf_encode_fh(struct inode *inode, __u32 *fh, int *lenp,
			 struct inode *parent)
{
	int len = *lenp;
	struct kernel_lb_addr location = UDF_I(inode)->i_location;
	struct fid *fid = (struct fid *)fh;
	int type = FILEID_UDF_WITHOUT_PARENT;

	if (parent && (len < 5)) {
		*lenp = 5;
		return FILEID_INVALID;
	} else if (len < 3) {
		*lenp = 3;
		return FILEID_INVALID;
	}

	*lenp = 3;
	fid->udf.block = location.logicalBlockNum;
	fid->udf.partref = location.partitionReferenceNum;
	fid->udf.parent_partref = 0;
	fid->udf.generation = inode->i_generation;

	if (parent) {
		location = UDF_I(parent)->i_location;
		fid->udf.parent_block = location.logicalBlockNum;
		fid->udf.parent_partref = location.partitionReferenceNum;
		fid->udf.parent_generation = inode->i_generation;
		*lenp = 5;
		type = FILEID_UDF_WITH_PARENT;
	}

	return type;
}

const struct export_operations udf_export_ops = {
	.encode_fh	= udf_encode_fh,
	.fh_to_dentry   = udf_fh_to_dentry,
	.fh_to_parent   = udf_fh_to_parent,
	.get_parent     = udf_get_parent,
};

const struct inode_operations udf_dir_inode_operations = {
	.lookup				= udf_lookup,
	.create				= udf_create,
	.link				= udf_link,
	.unlink				= udf_unlink,
	.symlink			= udf_symlink,
	.mkdir				= udf_mkdir,
	.rmdir				= udf_rmdir,
	.mknod				= udf_mknod,
	.rename				= udf_rename,
	.tmpfile			= udf_tmpfile,
};
