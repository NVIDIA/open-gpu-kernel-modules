// SPDX-License-Identifier: GPL-2.0-only
/*
 * OMFS (as used by RIO Karma) file operations.
 * Copyright (C) 2005 Bob Copeland <me@bobcopeland.com>
 */

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/buffer_head.h>
#include <linux/mpage.h>
#include "omfs.h"

static u32 omfs_max_extents(struct omfs_sb_info *sbi, int offset)
{
	return (sbi->s_sys_blocksize - offset -
		sizeof(struct omfs_extent)) /
		sizeof(struct omfs_extent_entry) + 1;
}

void omfs_make_empty_table(struct buffer_head *bh, int offset)
{
	struct omfs_extent *oe = (struct omfs_extent *) &bh->b_data[offset];

	oe->e_next = ~cpu_to_be64(0ULL);
	oe->e_extent_count = cpu_to_be32(1),
	oe->e_fill = cpu_to_be32(0x22),
	oe->e_entry.e_cluster = ~cpu_to_be64(0ULL);
	oe->e_entry.e_blocks = ~cpu_to_be64(0ULL);
}

int omfs_shrink_inode(struct inode *inode)
{
	struct omfs_sb_info *sbi = OMFS_SB(inode->i_sb);
	struct omfs_extent *oe;
	struct omfs_extent_entry *entry;
	struct buffer_head *bh;
	u64 next, last;
	u32 extent_count;
	u32 max_extents;
	int ret;

	/* traverse extent table, freeing each entry that is greater
	 * than inode->i_size;
	 */
	next = inode->i_ino;

	/* only support truncate -> 0 for now */
	ret = -EIO;
	if (inode->i_size != 0)
		goto out;

	bh = omfs_bread(inode->i_sb, next);
	if (!bh)
		goto out;

	oe = (struct omfs_extent *)(&bh->b_data[OMFS_EXTENT_START]);
	max_extents = omfs_max_extents(sbi, OMFS_EXTENT_START);

	for (;;) {

		if (omfs_is_bad(sbi, (struct omfs_header *) bh->b_data, next))
			goto out_brelse;

		extent_count = be32_to_cpu(oe->e_extent_count);

		if (extent_count > max_extents)
			goto out_brelse;

		last = next;
		next = be64_to_cpu(oe->e_next);
		entry = &oe->e_entry;

		/* ignore last entry as it is the terminator */
		for (; extent_count > 1; extent_count--) {
			u64 start, count;
			start = be64_to_cpu(entry->e_cluster);
			count = be64_to_cpu(entry->e_blocks);

			omfs_clear_range(inode->i_sb, start, (int) count);
			entry++;
		}
		omfs_make_empty_table(bh, (char *) oe - bh->b_data);
		mark_buffer_dirty(bh);
		brelse(bh);

		if (last != inode->i_ino)
			omfs_clear_range(inode->i_sb, last, sbi->s_mirrors);

		if (next == ~0)
			break;

		bh = omfs_bread(inode->i_sb, next);
		if (!bh)
			goto out;
		oe = (struct omfs_extent *) (&bh->b_data[OMFS_EXTENT_CONT]);
		max_extents = omfs_max_extents(sbi, OMFS_EXTENT_CONT);
	}
	ret = 0;
out:
	return ret;
out_brelse:
	brelse(bh);
	return ret;
}

static void omfs_truncate(struct inode *inode)
{
	omfs_shrink_inode(inode);
	mark_inode_dirty(inode);
}

/*
 * Add new blocks to the current extent, or create new entries/continuations
 * as necessary.
 */
static int omfs_grow_extent(struct inode *inode, struct omfs_extent *oe,
			u64 *ret_block)
{
	struct omfs_extent_entry *terminator;
	struct omfs_extent_entry *entry = &oe->e_entry;
	struct omfs_sb_info *sbi = OMFS_SB(inode->i_sb);
	u32 extent_count = be32_to_cpu(oe->e_extent_count);
	u64 new_block = 0;
	u32 max_count;
	int new_count;
	int ret = 0;

	/* reached the end of the extent table with no blocks mapped.
	 * there are three possibilities for adding: grow last extent,
	 * add a new extent to the current extent table, and add a
	 * continuation inode.  in last two cases need an allocator for
	 * sbi->s_cluster_size
	 */

	/* TODO: handle holes */

	/* should always have a terminator */
	if (extent_count < 1)
		return -EIO;

	/* trivially grow current extent, if next block is not taken */
	terminator = entry + extent_count - 1;
	if (extent_count > 1) {
		entry = terminator-1;
		new_block = be64_to_cpu(entry->e_cluster) +
			be64_to_cpu(entry->e_blocks);

		if (omfs_allocate_block(inode->i_sb, new_block)) {
			be64_add_cpu(&entry->e_blocks, 1);
			terminator->e_blocks = ~(cpu_to_be64(
				be64_to_cpu(~terminator->e_blocks) + 1));
			goto out;
		}
	}
	max_count = omfs_max_extents(sbi, OMFS_EXTENT_START);

	/* TODO: add a continuation block here */
	if (be32_to_cpu(oe->e_extent_count) > max_count-1)
		return -EIO;

	/* try to allocate a new cluster */
	ret = omfs_allocate_range(inode->i_sb, 1, sbi->s_clustersize,
		&new_block, &new_count);
	if (ret)
		goto out_fail;

	/* copy terminator down an entry */
	entry = terminator;
	terminator++;
	memcpy(terminator, entry, sizeof(struct omfs_extent_entry));

	entry->e_cluster = cpu_to_be64(new_block);
	entry->e_blocks = cpu_to_be64((u64) new_count);

	terminator->e_blocks = ~(cpu_to_be64(
		be64_to_cpu(~terminator->e_blocks) + (u64) new_count));

	/* write in new entry */
	be32_add_cpu(&oe->e_extent_count, 1);

out:
	*ret_block = new_block;
out_fail:
	return ret;
}

/*
 * Scans across the directory table for a given file block number.
 * If block not found, return 0.
 */
static sector_t find_block(struct inode *inode, struct omfs_extent_entry *ent,
			sector_t block, int count, int *left)
{
	/* count > 1 because of terminator */
	sector_t searched = 0;
	for (; count > 1; count--) {
		int numblocks = clus_to_blk(OMFS_SB(inode->i_sb),
			be64_to_cpu(ent->e_blocks));

		if (block >= searched  &&
		    block < searched + numblocks) {
			/*
			 * found it at cluster + (block - searched)
			 * numblocks - (block - searched) is remainder
			 */
			*left = numblocks - (block - searched);
			return clus_to_blk(OMFS_SB(inode->i_sb),
				be64_to_cpu(ent->e_cluster)) +
				block - searched;
		}
		searched += numblocks;
		ent++;
	}
	return 0;
}

static int omfs_get_block(struct inode *inode, sector_t block,
			  struct buffer_head *bh_result, int create)
{
	struct buffer_head *bh;
	sector_t next, offset;
	int ret;
	u64 new_block;
	u32 max_extents;
	int extent_count;
	struct omfs_extent *oe;
	struct omfs_extent_entry *entry;
	struct omfs_sb_info *sbi = OMFS_SB(inode->i_sb);
	int max_blocks = bh_result->b_size >> inode->i_blkbits;
	int remain;

	ret = -EIO;
	bh = omfs_bread(inode->i_sb, inode->i_ino);
	if (!bh)
		goto out;

	oe = (struct omfs_extent *)(&bh->b_data[OMFS_EXTENT_START]);
	max_extents = omfs_max_extents(sbi, OMFS_EXTENT_START);
	next = inode->i_ino;

	for (;;) {

		if (omfs_is_bad(sbi, (struct omfs_header *) bh->b_data, next))
			goto out_brelse;

		extent_count = be32_to_cpu(oe->e_extent_count);
		next = be64_to_cpu(oe->e_next);
		entry = &oe->e_entry;

		if (extent_count > max_extents)
			goto out_brelse;

		offset = find_block(inode, entry, block, extent_count, &remain);
		if (offset > 0) {
			ret = 0;
			map_bh(bh_result, inode->i_sb, offset);
			if (remain > max_blocks)
				remain = max_blocks;
			bh_result->b_size = (remain << inode->i_blkbits);
			goto out_brelse;
		}
		if (next == ~0)
			break;

		brelse(bh);
		bh = omfs_bread(inode->i_sb, next);
		if (!bh)
			goto out;
		oe = (struct omfs_extent *) (&bh->b_data[OMFS_EXTENT_CONT]);
		max_extents = omfs_max_extents(sbi, OMFS_EXTENT_CONT);
	}
	if (create) {
		ret = omfs_grow_extent(inode, oe, &new_block);
		if (ret == 0) {
			mark_buffer_dirty(bh);
			mark_inode_dirty(inode);
			map_bh(bh_result, inode->i_sb,
					clus_to_blk(sbi, new_block));
		}
	}
out_brelse:
	brelse(bh);
out:
	return ret;
}

static int omfs_readpage(struct file *file, struct page *page)
{
	return block_read_full_page(page, omfs_get_block);
}

static void omfs_readahead(struct readahead_control *rac)
{
	mpage_readahead(rac, omfs_get_block);
}

static int omfs_writepage(struct page *page, struct writeback_control *wbc)
{
	return block_write_full_page(page, omfs_get_block, wbc);
}

static int
omfs_writepages(struct address_space *mapping, struct writeback_control *wbc)
{
	return mpage_writepages(mapping, wbc, omfs_get_block);
}

static void omfs_write_failed(struct address_space *mapping, loff_t to)
{
	struct inode *inode = mapping->host;

	if (to > inode->i_size) {
		truncate_pagecache(inode, inode->i_size);
		omfs_truncate(inode);
	}
}

static int omfs_write_begin(struct file *file, struct address_space *mapping,
			loff_t pos, unsigned len, unsigned flags,
			struct page **pagep, void **fsdata)
{
	int ret;

	ret = block_write_begin(mapping, pos, len, flags, pagep,
				omfs_get_block);
	if (unlikely(ret))
		omfs_write_failed(mapping, pos + len);

	return ret;
}

static sector_t omfs_bmap(struct address_space *mapping, sector_t block)
{
	return generic_block_bmap(mapping, block, omfs_get_block);
}

const struct file_operations omfs_file_operations = {
	.llseek = generic_file_llseek,
	.read_iter = generic_file_read_iter,
	.write_iter = generic_file_write_iter,
	.mmap = generic_file_mmap,
	.fsync = generic_file_fsync,
	.splice_read = generic_file_splice_read,
};

static int omfs_setattr(struct user_namespace *mnt_userns,
			struct dentry *dentry, struct iattr *attr)
{
	struct inode *inode = d_inode(dentry);
	int error;

	error = setattr_prepare(&init_user_ns, dentry, attr);
	if (error)
		return error;

	if ((attr->ia_valid & ATTR_SIZE) &&
	    attr->ia_size != i_size_read(inode)) {
		error = inode_newsize_ok(inode, attr->ia_size);
		if (error)
			return error;
		truncate_setsize(inode, attr->ia_size);
		omfs_truncate(inode);
	}

	setattr_copy(&init_user_ns, inode, attr);
	mark_inode_dirty(inode);
	return 0;
}

const struct inode_operations omfs_file_inops = {
	.setattr = omfs_setattr,
};

const struct address_space_operations omfs_aops = {
	.readpage = omfs_readpage,
	.readahead = omfs_readahead,
	.writepage = omfs_writepage,
	.writepages = omfs_writepages,
	.write_begin = omfs_write_begin,
	.write_end = generic_write_end,
	.bmap = omfs_bmap,
};

