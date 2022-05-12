// SPDX-License-Identifier: GPL-2.0
/*
 * fs/f2fs/inode.c
 *
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
 *             http://www.samsung.com/
 */
#include <linux/fs.h>
#include <linux/f2fs_fs.h>
#include <linux/buffer_head.h>
#include <linux/backing-dev.h>
#include <linux/writeback.h>

#include "f2fs.h"
#include "node.h"
#include "segment.h"
#include "xattr.h"

#include <trace/events/f2fs.h>

void f2fs_mark_inode_dirty_sync(struct inode *inode, bool sync)
{
	if (is_inode_flag_set(inode, FI_NEW_INODE))
		return;

	if (f2fs_inode_dirtied(inode, sync))
		return;

	mark_inode_dirty_sync(inode);
}

void f2fs_set_inode_flags(struct inode *inode)
{
	unsigned int flags = F2FS_I(inode)->i_flags;
	unsigned int new_fl = 0;

	if (flags & F2FS_SYNC_FL)
		new_fl |= S_SYNC;
	if (flags & F2FS_APPEND_FL)
		new_fl |= S_APPEND;
	if (flags & F2FS_IMMUTABLE_FL)
		new_fl |= S_IMMUTABLE;
	if (flags & F2FS_NOATIME_FL)
		new_fl |= S_NOATIME;
	if (flags & F2FS_DIRSYNC_FL)
		new_fl |= S_DIRSYNC;
	if (file_is_encrypt(inode))
		new_fl |= S_ENCRYPTED;
	if (file_is_verity(inode))
		new_fl |= S_VERITY;
	if (flags & F2FS_CASEFOLD_FL)
		new_fl |= S_CASEFOLD;
	inode_set_flags(inode, new_fl,
			S_SYNC|S_APPEND|S_IMMUTABLE|S_NOATIME|S_DIRSYNC|
			S_ENCRYPTED|S_VERITY|S_CASEFOLD);
}

static void __get_inode_rdev(struct inode *inode, struct f2fs_inode *ri)
{
	int extra_size = get_extra_isize(inode);

	if (S_ISCHR(inode->i_mode) || S_ISBLK(inode->i_mode) ||
			S_ISFIFO(inode->i_mode) || S_ISSOCK(inode->i_mode)) {
		if (ri->i_addr[extra_size])
			inode->i_rdev = old_decode_dev(
				le32_to_cpu(ri->i_addr[extra_size]));
		else
			inode->i_rdev = new_decode_dev(
				le32_to_cpu(ri->i_addr[extra_size + 1]));
	}
}

static int __written_first_block(struct f2fs_sb_info *sbi,
					struct f2fs_inode *ri)
{
	block_t addr = le32_to_cpu(ri->i_addr[offset_in_addr(ri)]);

	if (!__is_valid_data_blkaddr(addr))
		return 1;
	if (!f2fs_is_valid_blkaddr(sbi, addr, DATA_GENERIC_ENHANCE))
		return -EFSCORRUPTED;
	return 0;
}

static void __set_inode_rdev(struct inode *inode, struct f2fs_inode *ri)
{
	int extra_size = get_extra_isize(inode);

	if (S_ISCHR(inode->i_mode) || S_ISBLK(inode->i_mode)) {
		if (old_valid_dev(inode->i_rdev)) {
			ri->i_addr[extra_size] =
				cpu_to_le32(old_encode_dev(inode->i_rdev));
			ri->i_addr[extra_size + 1] = 0;
		} else {
			ri->i_addr[extra_size] = 0;
			ri->i_addr[extra_size + 1] =
				cpu_to_le32(new_encode_dev(inode->i_rdev));
			ri->i_addr[extra_size + 2] = 0;
		}
	}
}

static void __recover_inline_status(struct inode *inode, struct page *ipage)
{
	void *inline_data = inline_data_addr(inode, ipage);
	__le32 *start = inline_data;
	__le32 *end = start + MAX_INLINE_DATA(inode) / sizeof(__le32);

	while (start < end) {
		if (*start++) {
			f2fs_wait_on_page_writeback(ipage, NODE, true, true);

			set_inode_flag(inode, FI_DATA_EXIST);
			set_raw_inline(inode, F2FS_INODE(ipage));
			set_page_dirty(ipage);
			return;
		}
	}
	return;
}

static bool f2fs_enable_inode_chksum(struct f2fs_sb_info *sbi, struct page *page)
{
	struct f2fs_inode *ri = &F2FS_NODE(page)->i;

	if (!f2fs_sb_has_inode_chksum(sbi))
		return false;

	if (!IS_INODE(page) || !(ri->i_inline & F2FS_EXTRA_ATTR))
		return false;

	if (!F2FS_FITS_IN_INODE(ri, le16_to_cpu(ri->i_extra_isize),
				i_inode_checksum))
		return false;

	return true;
}

static __u32 f2fs_inode_chksum(struct f2fs_sb_info *sbi, struct page *page)
{
	struct f2fs_node *node = F2FS_NODE(page);
	struct f2fs_inode *ri = &node->i;
	__le32 ino = node->footer.ino;
	__le32 gen = ri->i_generation;
	__u32 chksum, chksum_seed;
	__u32 dummy_cs = 0;
	unsigned int offset = offsetof(struct f2fs_inode, i_inode_checksum);
	unsigned int cs_size = sizeof(dummy_cs);

	chksum = f2fs_chksum(sbi, sbi->s_chksum_seed, (__u8 *)&ino,
							sizeof(ino));
	chksum_seed = f2fs_chksum(sbi, chksum, (__u8 *)&gen, sizeof(gen));

	chksum = f2fs_chksum(sbi, chksum_seed, (__u8 *)ri, offset);
	chksum = f2fs_chksum(sbi, chksum, (__u8 *)&dummy_cs, cs_size);
	offset += cs_size;
	chksum = f2fs_chksum(sbi, chksum, (__u8 *)ri + offset,
						F2FS_BLKSIZE - offset);
	return chksum;
}

bool f2fs_inode_chksum_verify(struct f2fs_sb_info *sbi, struct page *page)
{
	struct f2fs_inode *ri;
	__u32 provided, calculated;

	if (unlikely(is_sbi_flag_set(sbi, SBI_IS_SHUTDOWN)))
		return true;

#ifdef CONFIG_F2FS_CHECK_FS
	if (!f2fs_enable_inode_chksum(sbi, page))
#else
	if (!f2fs_enable_inode_chksum(sbi, page) ||
			PageDirty(page) || PageWriteback(page))
#endif
		return true;

	ri = &F2FS_NODE(page)->i;
	provided = le32_to_cpu(ri->i_inode_checksum);
	calculated = f2fs_inode_chksum(sbi, page);

	if (provided != calculated)
		f2fs_warn(sbi, "checksum invalid, nid = %lu, ino_of_node = %x, %x vs. %x",
			  page->index, ino_of_node(page), provided, calculated);

	return provided == calculated;
}

void f2fs_inode_chksum_set(struct f2fs_sb_info *sbi, struct page *page)
{
	struct f2fs_inode *ri = &F2FS_NODE(page)->i;

	if (!f2fs_enable_inode_chksum(sbi, page))
		return;

	ri->i_inode_checksum = cpu_to_le32(f2fs_inode_chksum(sbi, page));
}

static bool sanity_check_inode(struct inode *inode, struct page *node_page)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
	struct f2fs_inode_info *fi = F2FS_I(inode);
	struct f2fs_inode *ri = F2FS_INODE(node_page);
	unsigned long long iblocks;

	iblocks = le64_to_cpu(F2FS_INODE(node_page)->i_blocks);
	if (!iblocks) {
		set_sbi_flag(sbi, SBI_NEED_FSCK);
		f2fs_warn(sbi, "%s: corrupted inode i_blocks i_ino=%lx iblocks=%llu, run fsck to fix.",
			  __func__, inode->i_ino, iblocks);
		return false;
	}

	if (ino_of_node(node_page) != nid_of_node(node_page)) {
		set_sbi_flag(sbi, SBI_NEED_FSCK);
		f2fs_warn(sbi, "%s: corrupted inode footer i_ino=%lx, ino,nid: [%u, %u] run fsck to fix.",
			  __func__, inode->i_ino,
			  ino_of_node(node_page), nid_of_node(node_page));
		return false;
	}

	if (f2fs_sb_has_flexible_inline_xattr(sbi)
			&& !f2fs_has_extra_attr(inode)) {
		set_sbi_flag(sbi, SBI_NEED_FSCK);
		f2fs_warn(sbi, "%s: corrupted inode ino=%lx, run fsck to fix.",
			  __func__, inode->i_ino);
		return false;
	}

	if (f2fs_has_extra_attr(inode) &&
			!f2fs_sb_has_extra_attr(sbi)) {
		set_sbi_flag(sbi, SBI_NEED_FSCK);
		f2fs_warn(sbi, "%s: inode (ino=%lx) is with extra_attr, but extra_attr feature is off",
			  __func__, inode->i_ino);
		return false;
	}

	if (fi->i_extra_isize > F2FS_TOTAL_EXTRA_ATTR_SIZE ||
			fi->i_extra_isize % sizeof(__le32)) {
		set_sbi_flag(sbi, SBI_NEED_FSCK);
		f2fs_warn(sbi, "%s: inode (ino=%lx) has corrupted i_extra_isize: %d, max: %zu",
			  __func__, inode->i_ino, fi->i_extra_isize,
			  F2FS_TOTAL_EXTRA_ATTR_SIZE);
		return false;
	}

	if (f2fs_has_extra_attr(inode) &&
		f2fs_sb_has_flexible_inline_xattr(sbi) &&
		f2fs_has_inline_xattr(inode) &&
		(!fi->i_inline_xattr_size ||
		fi->i_inline_xattr_size > MAX_INLINE_XATTR_SIZE)) {
		set_sbi_flag(sbi, SBI_NEED_FSCK);
		f2fs_warn(sbi, "%s: inode (ino=%lx) has corrupted i_inline_xattr_size: %d, max: %zu",
			  __func__, inode->i_ino, fi->i_inline_xattr_size,
			  MAX_INLINE_XATTR_SIZE);
		return false;
	}

	if (F2FS_I(inode)->extent_tree) {
		struct extent_info *ei = &F2FS_I(inode)->extent_tree->largest;

		if (ei->len &&
			(!f2fs_is_valid_blkaddr(sbi, ei->blk,
						DATA_GENERIC_ENHANCE) ||
			!f2fs_is_valid_blkaddr(sbi, ei->blk + ei->len - 1,
						DATA_GENERIC_ENHANCE))) {
			set_sbi_flag(sbi, SBI_NEED_FSCK);
			f2fs_warn(sbi, "%s: inode (ino=%lx) extent info [%u, %u, %u] is incorrect, run fsck to fix",
				  __func__, inode->i_ino,
				  ei->blk, ei->fofs, ei->len);
			return false;
		}
	}

	if (f2fs_has_inline_data(inode) &&
			(!S_ISREG(inode->i_mode) && !S_ISLNK(inode->i_mode))) {
		set_sbi_flag(sbi, SBI_NEED_FSCK);
		f2fs_warn(sbi, "%s: inode (ino=%lx, mode=%u) should not have inline_data, run fsck to fix",
			  __func__, inode->i_ino, inode->i_mode);
		return false;
	}

	if (f2fs_has_inline_dentry(inode) && !S_ISDIR(inode->i_mode)) {
		set_sbi_flag(sbi, SBI_NEED_FSCK);
		f2fs_warn(sbi, "%s: inode (ino=%lx, mode=%u) should not have inline_dentry, run fsck to fix",
			  __func__, inode->i_ino, inode->i_mode);
		return false;
	}

	if ((fi->i_flags & F2FS_CASEFOLD_FL) && !f2fs_sb_has_casefold(sbi)) {
		set_sbi_flag(sbi, SBI_NEED_FSCK);
		f2fs_warn(sbi, "%s: inode (ino=%lx) has casefold flag, but casefold feature is off",
			  __func__, inode->i_ino);
		return false;
	}

	if (f2fs_has_extra_attr(inode) && f2fs_sb_has_compression(sbi) &&
			fi->i_flags & F2FS_COMPR_FL &&
			F2FS_FITS_IN_INODE(ri, fi->i_extra_isize,
						i_log_cluster_size)) {
		if (ri->i_compress_algorithm >= COMPRESS_MAX) {
			set_sbi_flag(sbi, SBI_NEED_FSCK);
			f2fs_warn(sbi, "%s: inode (ino=%lx) has unsupported "
				"compress algorithm: %u, run fsck to fix",
				  __func__, inode->i_ino,
				  ri->i_compress_algorithm);
			return false;
		}
		if (le64_to_cpu(ri->i_compr_blocks) >
				SECTOR_TO_BLOCK(inode->i_blocks)) {
			set_sbi_flag(sbi, SBI_NEED_FSCK);
			f2fs_warn(sbi, "%s: inode (ino=%lx) has inconsistent "
				"i_compr_blocks:%llu, i_blocks:%llu, run fsck to fix",
				  __func__, inode->i_ino,
				  le64_to_cpu(ri->i_compr_blocks),
				  SECTOR_TO_BLOCK(inode->i_blocks));
			return false;
		}
		if (ri->i_log_cluster_size < MIN_COMPRESS_LOG_SIZE ||
			ri->i_log_cluster_size > MAX_COMPRESS_LOG_SIZE) {
			set_sbi_flag(sbi, SBI_NEED_FSCK);
			f2fs_warn(sbi, "%s: inode (ino=%lx) has unsupported "
				"log cluster size: %u, run fsck to fix",
				  __func__, inode->i_ino,
				  ri->i_log_cluster_size);
			return false;
		}
	}

	return true;
}

static int do_read_inode(struct inode *inode)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
	struct f2fs_inode_info *fi = F2FS_I(inode);
	struct page *node_page;
	struct f2fs_inode *ri;
	projid_t i_projid;
	int err;

	/* Check if ino is within scope */
	if (f2fs_check_nid_range(sbi, inode->i_ino))
		return -EINVAL;

	node_page = f2fs_get_node_page(sbi, inode->i_ino);
	if (IS_ERR(node_page))
		return PTR_ERR(node_page);

	ri = F2FS_INODE(node_page);

	inode->i_mode = le16_to_cpu(ri->i_mode);
	i_uid_write(inode, le32_to_cpu(ri->i_uid));
	i_gid_write(inode, le32_to_cpu(ri->i_gid));
	set_nlink(inode, le32_to_cpu(ri->i_links));
	inode->i_size = le64_to_cpu(ri->i_size);
	inode->i_blocks = SECTOR_FROM_BLOCK(le64_to_cpu(ri->i_blocks) - 1);

	inode->i_atime.tv_sec = le64_to_cpu(ri->i_atime);
	inode->i_ctime.tv_sec = le64_to_cpu(ri->i_ctime);
	inode->i_mtime.tv_sec = le64_to_cpu(ri->i_mtime);
	inode->i_atime.tv_nsec = le32_to_cpu(ri->i_atime_nsec);
	inode->i_ctime.tv_nsec = le32_to_cpu(ri->i_ctime_nsec);
	inode->i_mtime.tv_nsec = le32_to_cpu(ri->i_mtime_nsec);
	inode->i_generation = le32_to_cpu(ri->i_generation);
	if (S_ISDIR(inode->i_mode))
		fi->i_current_depth = le32_to_cpu(ri->i_current_depth);
	else if (S_ISREG(inode->i_mode))
		fi->i_gc_failures[GC_FAILURE_PIN] =
					le16_to_cpu(ri->i_gc_failures);
	fi->i_xattr_nid = le32_to_cpu(ri->i_xattr_nid);
	fi->i_flags = le32_to_cpu(ri->i_flags);
	if (S_ISREG(inode->i_mode))
		fi->i_flags &= ~F2FS_PROJINHERIT_FL;
	bitmap_zero(fi->flags, FI_MAX);
	fi->i_advise = ri->i_advise;
	fi->i_pino = le32_to_cpu(ri->i_pino);
	fi->i_dir_level = ri->i_dir_level;

	f2fs_init_extent_tree(inode, node_page);

	get_inline_info(inode, ri);

	fi->i_extra_isize = f2fs_has_extra_attr(inode) ?
					le16_to_cpu(ri->i_extra_isize) : 0;

	if (f2fs_sb_has_flexible_inline_xattr(sbi)) {
		fi->i_inline_xattr_size = le16_to_cpu(ri->i_inline_xattr_size);
	} else if (f2fs_has_inline_xattr(inode) ||
				f2fs_has_inline_dentry(inode)) {
		fi->i_inline_xattr_size = DEFAULT_INLINE_XATTR_ADDRS;
	} else {

		/*
		 * Previous inline data or directory always reserved 200 bytes
		 * in inode layout, even if inline_xattr is disabled. In order
		 * to keep inline_dentry's structure for backward compatibility,
		 * we get the space back only from inline_data.
		 */
		fi->i_inline_xattr_size = 0;
	}

	if (!sanity_check_inode(inode, node_page)) {
		f2fs_put_page(node_page, 1);
		return -EFSCORRUPTED;
	}

	/* check data exist */
	if (f2fs_has_inline_data(inode) && !f2fs_exist_data(inode))
		__recover_inline_status(inode, node_page);

	/* try to recover cold bit for non-dir inode */
	if (!S_ISDIR(inode->i_mode) && !is_cold_node(node_page)) {
		f2fs_wait_on_page_writeback(node_page, NODE, true, true);
		set_cold_node(node_page, false);
		set_page_dirty(node_page);
	}

	/* get rdev by using inline_info */
	__get_inode_rdev(inode, ri);

	if (S_ISREG(inode->i_mode)) {
		err = __written_first_block(sbi, ri);
		if (err < 0) {
			f2fs_put_page(node_page, 1);
			return err;
		}
		if (!err)
			set_inode_flag(inode, FI_FIRST_BLOCK_WRITTEN);
	}

	if (!f2fs_need_inode_block_update(sbi, inode->i_ino))
		fi->last_disk_size = inode->i_size;

	if (fi->i_flags & F2FS_PROJINHERIT_FL)
		set_inode_flag(inode, FI_PROJ_INHERIT);

	if (f2fs_has_extra_attr(inode) && f2fs_sb_has_project_quota(sbi) &&
			F2FS_FITS_IN_INODE(ri, fi->i_extra_isize, i_projid))
		i_projid = (projid_t)le32_to_cpu(ri->i_projid);
	else
		i_projid = F2FS_DEF_PROJID;
	fi->i_projid = make_kprojid(&init_user_ns, i_projid);

	if (f2fs_has_extra_attr(inode) && f2fs_sb_has_inode_crtime(sbi) &&
			F2FS_FITS_IN_INODE(ri, fi->i_extra_isize, i_crtime)) {
		fi->i_crtime.tv_sec = le64_to_cpu(ri->i_crtime);
		fi->i_crtime.tv_nsec = le32_to_cpu(ri->i_crtime_nsec);
	}

	if (f2fs_has_extra_attr(inode) && f2fs_sb_has_compression(sbi) &&
					(fi->i_flags & F2FS_COMPR_FL)) {
		if (F2FS_FITS_IN_INODE(ri, fi->i_extra_isize,
					i_log_cluster_size)) {
			atomic_set(&fi->i_compr_blocks,
					le64_to_cpu(ri->i_compr_blocks));
			fi->i_compress_algorithm = ri->i_compress_algorithm;
			fi->i_log_cluster_size = ri->i_log_cluster_size;
			fi->i_compress_flag = le16_to_cpu(ri->i_compress_flag);
			fi->i_cluster_size = 1 << fi->i_log_cluster_size;
			set_inode_flag(inode, FI_COMPRESSED_FILE);
		}
	}

	F2FS_I(inode)->i_disk_time[0] = inode->i_atime;
	F2FS_I(inode)->i_disk_time[1] = inode->i_ctime;
	F2FS_I(inode)->i_disk_time[2] = inode->i_mtime;
	F2FS_I(inode)->i_disk_time[3] = F2FS_I(inode)->i_crtime;
	f2fs_put_page(node_page, 1);

	stat_inc_inline_xattr(inode);
	stat_inc_inline_inode(inode);
	stat_inc_inline_dir(inode);
	stat_inc_compr_inode(inode);
	stat_add_compr_blocks(inode, atomic_read(&fi->i_compr_blocks));

	return 0;
}

struct inode *f2fs_iget(struct super_block *sb, unsigned long ino)
{
	struct f2fs_sb_info *sbi = F2FS_SB(sb);
	struct inode *inode;
	int ret = 0;

	inode = iget_locked(sb, ino);
	if (!inode)
		return ERR_PTR(-ENOMEM);

	if (!(inode->i_state & I_NEW)) {
		trace_f2fs_iget(inode);
		return inode;
	}
	if (ino == F2FS_NODE_INO(sbi) || ino == F2FS_META_INO(sbi))
		goto make_now;

	ret = do_read_inode(inode);
	if (ret)
		goto bad_inode;
make_now:
	if (ino == F2FS_NODE_INO(sbi)) {
		inode->i_mapping->a_ops = &f2fs_node_aops;
		mapping_set_gfp_mask(inode->i_mapping, GFP_NOFS);
	} else if (ino == F2FS_META_INO(sbi)) {
		inode->i_mapping->a_ops = &f2fs_meta_aops;
		mapping_set_gfp_mask(inode->i_mapping, GFP_NOFS);
	} else if (S_ISREG(inode->i_mode)) {
		inode->i_op = &f2fs_file_inode_operations;
		inode->i_fop = &f2fs_file_operations;
		inode->i_mapping->a_ops = &f2fs_dblock_aops;
	} else if (S_ISDIR(inode->i_mode)) {
		inode->i_op = &f2fs_dir_inode_operations;
		inode->i_fop = &f2fs_dir_operations;
		inode->i_mapping->a_ops = &f2fs_dblock_aops;
		inode_nohighmem(inode);
	} else if (S_ISLNK(inode->i_mode)) {
		if (file_is_encrypt(inode))
			inode->i_op = &f2fs_encrypted_symlink_inode_operations;
		else
			inode->i_op = &f2fs_symlink_inode_operations;
		inode_nohighmem(inode);
		inode->i_mapping->a_ops = &f2fs_dblock_aops;
	} else if (S_ISCHR(inode->i_mode) || S_ISBLK(inode->i_mode) ||
			S_ISFIFO(inode->i_mode) || S_ISSOCK(inode->i_mode)) {
		inode->i_op = &f2fs_special_inode_operations;
		init_special_inode(inode, inode->i_mode, inode->i_rdev);
	} else {
		ret = -EIO;
		goto bad_inode;
	}
	f2fs_set_inode_flags(inode);
	unlock_new_inode(inode);
	trace_f2fs_iget(inode);
	return inode;

bad_inode:
	f2fs_inode_synced(inode);
	iget_failed(inode);
	trace_f2fs_iget_exit(inode, ret);
	return ERR_PTR(ret);
}

struct inode *f2fs_iget_retry(struct super_block *sb, unsigned long ino)
{
	struct inode *inode;
retry:
	inode = f2fs_iget(sb, ino);
	if (IS_ERR(inode)) {
		if (PTR_ERR(inode) == -ENOMEM) {
			congestion_wait(BLK_RW_ASYNC, DEFAULT_IO_TIMEOUT);
			goto retry;
		}
	}
	return inode;
}

void f2fs_update_inode(struct inode *inode, struct page *node_page)
{
	struct f2fs_inode *ri;
	struct extent_tree *et = F2FS_I(inode)->extent_tree;

	f2fs_wait_on_page_writeback(node_page, NODE, true, true);
	set_page_dirty(node_page);

	f2fs_inode_synced(inode);

	ri = F2FS_INODE(node_page);

	ri->i_mode = cpu_to_le16(inode->i_mode);
	ri->i_advise = F2FS_I(inode)->i_advise;
	ri->i_uid = cpu_to_le32(i_uid_read(inode));
	ri->i_gid = cpu_to_le32(i_gid_read(inode));
	ri->i_links = cpu_to_le32(inode->i_nlink);
	ri->i_size = cpu_to_le64(i_size_read(inode));
	ri->i_blocks = cpu_to_le64(SECTOR_TO_BLOCK(inode->i_blocks) + 1);

	if (et) {
		read_lock(&et->lock);
		set_raw_extent(&et->largest, &ri->i_ext);
		read_unlock(&et->lock);
	} else {
		memset(&ri->i_ext, 0, sizeof(ri->i_ext));
	}
	set_raw_inline(inode, ri);

	ri->i_atime = cpu_to_le64(inode->i_atime.tv_sec);
	ri->i_ctime = cpu_to_le64(inode->i_ctime.tv_sec);
	ri->i_mtime = cpu_to_le64(inode->i_mtime.tv_sec);
	ri->i_atime_nsec = cpu_to_le32(inode->i_atime.tv_nsec);
	ri->i_ctime_nsec = cpu_to_le32(inode->i_ctime.tv_nsec);
	ri->i_mtime_nsec = cpu_to_le32(inode->i_mtime.tv_nsec);
	if (S_ISDIR(inode->i_mode))
		ri->i_current_depth =
			cpu_to_le32(F2FS_I(inode)->i_current_depth);
	else if (S_ISREG(inode->i_mode))
		ri->i_gc_failures =
			cpu_to_le16(F2FS_I(inode)->i_gc_failures[GC_FAILURE_PIN]);
	ri->i_xattr_nid = cpu_to_le32(F2FS_I(inode)->i_xattr_nid);
	ri->i_flags = cpu_to_le32(F2FS_I(inode)->i_flags);
	ri->i_pino = cpu_to_le32(F2FS_I(inode)->i_pino);
	ri->i_generation = cpu_to_le32(inode->i_generation);
	ri->i_dir_level = F2FS_I(inode)->i_dir_level;

	if (f2fs_has_extra_attr(inode)) {
		ri->i_extra_isize = cpu_to_le16(F2FS_I(inode)->i_extra_isize);

		if (f2fs_sb_has_flexible_inline_xattr(F2FS_I_SB(inode)))
			ri->i_inline_xattr_size =
				cpu_to_le16(F2FS_I(inode)->i_inline_xattr_size);

		if (f2fs_sb_has_project_quota(F2FS_I_SB(inode)) &&
			F2FS_FITS_IN_INODE(ri, F2FS_I(inode)->i_extra_isize,
								i_projid)) {
			projid_t i_projid;

			i_projid = from_kprojid(&init_user_ns,
						F2FS_I(inode)->i_projid);
			ri->i_projid = cpu_to_le32(i_projid);
		}

		if (f2fs_sb_has_inode_crtime(F2FS_I_SB(inode)) &&
			F2FS_FITS_IN_INODE(ri, F2FS_I(inode)->i_extra_isize,
								i_crtime)) {
			ri->i_crtime =
				cpu_to_le64(F2FS_I(inode)->i_crtime.tv_sec);
			ri->i_crtime_nsec =
				cpu_to_le32(F2FS_I(inode)->i_crtime.tv_nsec);
		}

		if (f2fs_sb_has_compression(F2FS_I_SB(inode)) &&
			F2FS_FITS_IN_INODE(ri, F2FS_I(inode)->i_extra_isize,
							i_log_cluster_size)) {
			ri->i_compr_blocks =
				cpu_to_le64(atomic_read(
					&F2FS_I(inode)->i_compr_blocks));
			ri->i_compress_algorithm =
				F2FS_I(inode)->i_compress_algorithm;
			ri->i_compress_flag =
				cpu_to_le16(F2FS_I(inode)->i_compress_flag);
			ri->i_log_cluster_size =
				F2FS_I(inode)->i_log_cluster_size;
		}
	}

	__set_inode_rdev(inode, ri);

	/* deleted inode */
	if (inode->i_nlink == 0)
		clear_inline_node(node_page);

	F2FS_I(inode)->i_disk_time[0] = inode->i_atime;
	F2FS_I(inode)->i_disk_time[1] = inode->i_ctime;
	F2FS_I(inode)->i_disk_time[2] = inode->i_mtime;
	F2FS_I(inode)->i_disk_time[3] = F2FS_I(inode)->i_crtime;

#ifdef CONFIG_F2FS_CHECK_FS
	f2fs_inode_chksum_set(F2FS_I_SB(inode), node_page);
#endif
}

void f2fs_update_inode_page(struct inode *inode)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
	struct page *node_page;
retry:
	node_page = f2fs_get_node_page(sbi, inode->i_ino);
	if (IS_ERR(node_page)) {
		int err = PTR_ERR(node_page);

		if (err == -ENOMEM) {
			cond_resched();
			goto retry;
		} else if (err != -ENOENT) {
			f2fs_stop_checkpoint(sbi, false);
		}
		return;
	}
	f2fs_update_inode(inode, node_page);
	f2fs_put_page(node_page, 1);
}

int f2fs_write_inode(struct inode *inode, struct writeback_control *wbc)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);

	if (inode->i_ino == F2FS_NODE_INO(sbi) ||
			inode->i_ino == F2FS_META_INO(sbi))
		return 0;

	/*
	 * atime could be updated without dirtying f2fs inode in lazytime mode
	 */
	if (f2fs_is_time_consistent(inode) &&
		!is_inode_flag_set(inode, FI_DIRTY_INODE))
		return 0;

	if (!f2fs_is_checkpoint_ready(sbi))
		return -ENOSPC;

	/*
	 * We need to balance fs here to prevent from producing dirty node pages
	 * during the urgent cleaning time when running out of free sections.
	 */
	f2fs_update_inode_page(inode);
	if (wbc && wbc->nr_to_write)
		f2fs_balance_fs(sbi, true);
	return 0;
}

/*
 * Called at the last iput() if i_nlink is zero
 */
void f2fs_evict_inode(struct inode *inode)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
	nid_t xnid = F2FS_I(inode)->i_xattr_nid;
	int err = 0;

	/* some remained atomic pages should discarded */
	if (f2fs_is_atomic_file(inode))
		f2fs_drop_inmem_pages(inode);

	trace_f2fs_evict_inode(inode);
	truncate_inode_pages_final(&inode->i_data);

	if (inode->i_ino == F2FS_NODE_INO(sbi) ||
			inode->i_ino == F2FS_META_INO(sbi))
		goto out_clear;

	f2fs_bug_on(sbi, get_dirty_pages(inode));
	f2fs_remove_dirty_inode(inode);

	f2fs_destroy_extent_tree(inode);

	if (inode->i_nlink || is_bad_inode(inode))
		goto no_delete;

	err = dquot_initialize(inode);
	if (err) {
		err = 0;
		set_sbi_flag(sbi, SBI_QUOTA_NEED_REPAIR);
	}

	f2fs_remove_ino_entry(sbi, inode->i_ino, APPEND_INO);
	f2fs_remove_ino_entry(sbi, inode->i_ino, UPDATE_INO);
	f2fs_remove_ino_entry(sbi, inode->i_ino, FLUSH_INO);

	sb_start_intwrite(inode->i_sb);
	set_inode_flag(inode, FI_NO_ALLOC);
	i_size_write(inode, 0);
retry:
	if (F2FS_HAS_BLOCKS(inode))
		err = f2fs_truncate(inode);

	if (time_to_inject(sbi, FAULT_EVICT_INODE)) {
		f2fs_show_injection_info(sbi, FAULT_EVICT_INODE);
		err = -EIO;
	}

	if (!err) {
		f2fs_lock_op(sbi);
		err = f2fs_remove_inode_page(inode);
		f2fs_unlock_op(sbi);
		if (err == -ENOENT)
			err = 0;
	}

	/* give more chances, if ENOMEM case */
	if (err == -ENOMEM) {
		err = 0;
		goto retry;
	}

	if (err) {
		f2fs_update_inode_page(inode);
		if (dquot_initialize_needed(inode))
			set_sbi_flag(sbi, SBI_QUOTA_NEED_REPAIR);
	}
	sb_end_intwrite(inode->i_sb);
no_delete:
	dquot_drop(inode);

	stat_dec_inline_xattr(inode);
	stat_dec_inline_dir(inode);
	stat_dec_inline_inode(inode);
	stat_dec_compr_inode(inode);
	stat_sub_compr_blocks(inode,
			atomic_read(&F2FS_I(inode)->i_compr_blocks));

	if (likely(!f2fs_cp_error(sbi) &&
				!is_sbi_flag_set(sbi, SBI_CP_DISABLED)))
		f2fs_bug_on(sbi, is_inode_flag_set(inode, FI_DIRTY_INODE));
	else
		f2fs_inode_synced(inode);

	/* for the case f2fs_new_inode() was failed, .i_ino is zero, skip it */
	if (inode->i_ino)
		invalidate_mapping_pages(NODE_MAPPING(sbi), inode->i_ino,
							inode->i_ino);
	if (xnid)
		invalidate_mapping_pages(NODE_MAPPING(sbi), xnid, xnid);
	if (inode->i_nlink) {
		if (is_inode_flag_set(inode, FI_APPEND_WRITE))
			f2fs_add_ino_entry(sbi, inode->i_ino, APPEND_INO);
		if (is_inode_flag_set(inode, FI_UPDATE_WRITE))
			f2fs_add_ino_entry(sbi, inode->i_ino, UPDATE_INO);
	}
	if (is_inode_flag_set(inode, FI_FREE_NID)) {
		f2fs_alloc_nid_failed(sbi, inode->i_ino);
		clear_inode_flag(inode, FI_FREE_NID);
	} else {
		/*
		 * If xattr nid is corrupted, we can reach out error condition,
		 * err & !f2fs_exist_written_data(sbi, inode->i_ino, ORPHAN_INO)).
		 * In that case, f2fs_check_nid_range() is enough to give a clue.
		 */
	}
out_clear:
	fscrypt_put_encryption_info(inode);
	fsverity_cleanup_inode(inode);
	clear_inode(inode);
}

/* caller should call f2fs_lock_op() */
void f2fs_handle_failed_inode(struct inode *inode)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
	struct node_info ni;
	int err;

	/*
	 * clear nlink of inode in order to release resource of inode
	 * immediately.
	 */
	clear_nlink(inode);

	/*
	 * we must call this to avoid inode being remained as dirty, resulting
	 * in a panic when flushing dirty inodes in gdirty_list.
	 */
	f2fs_update_inode_page(inode);
	f2fs_inode_synced(inode);

	/* don't make bad inode, since it becomes a regular file. */
	unlock_new_inode(inode);

	/*
	 * Note: we should add inode to orphan list before f2fs_unlock_op()
	 * so we can prevent losing this orphan when encoutering checkpoint
	 * and following suddenly power-off.
	 */
	err = f2fs_get_node_info(sbi, inode->i_ino, &ni);
	if (err) {
		set_sbi_flag(sbi, SBI_NEED_FSCK);
		f2fs_warn(sbi, "May loss orphan inode, run fsck to fix.");
		goto out;
	}

	if (ni.blk_addr != NULL_ADDR) {
		err = f2fs_acquire_orphan_inode(sbi);
		if (err) {
			set_sbi_flag(sbi, SBI_NEED_FSCK);
			f2fs_warn(sbi, "Too many orphan inodes, run fsck to fix.");
		} else {
			f2fs_add_orphan_inode(inode);
		}
		f2fs_alloc_nid_done(sbi, inode->i_ino);
	} else {
		set_inode_flag(inode, FI_FREE_NID);
	}

out:
	f2fs_unlock_op(sbi);

	/* iput will drop the inode object */
	iput(inode);
}
