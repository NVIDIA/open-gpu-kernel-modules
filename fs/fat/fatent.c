// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2004, OGAWA Hirofumi
 */

#include <linux/blkdev.h>
#include <linux/sched/signal.h>
#include "fat.h"

struct fatent_operations {
	void (*ent_blocknr)(struct super_block *, int, int *, sector_t *);
	void (*ent_set_ptr)(struct fat_entry *, int);
	int (*ent_bread)(struct super_block *, struct fat_entry *,
			 int, sector_t);
	int (*ent_get)(struct fat_entry *);
	void (*ent_put)(struct fat_entry *, int);
	int (*ent_next)(struct fat_entry *);
};

static DEFINE_SPINLOCK(fat12_entry_lock);

static void fat12_ent_blocknr(struct super_block *sb, int entry,
			      int *offset, sector_t *blocknr)
{
	struct msdos_sb_info *sbi = MSDOS_SB(sb);
	int bytes = entry + (entry >> 1);
	WARN_ON(!fat_valid_entry(sbi, entry));
	*offset = bytes & (sb->s_blocksize - 1);
	*blocknr = sbi->fat_start + (bytes >> sb->s_blocksize_bits);
}

static void fat_ent_blocknr(struct super_block *sb, int entry,
			    int *offset, sector_t *blocknr)
{
	struct msdos_sb_info *sbi = MSDOS_SB(sb);
	int bytes = (entry << sbi->fatent_shift);
	WARN_ON(!fat_valid_entry(sbi, entry));
	*offset = bytes & (sb->s_blocksize - 1);
	*blocknr = sbi->fat_start + (bytes >> sb->s_blocksize_bits);
}

static void fat12_ent_set_ptr(struct fat_entry *fatent, int offset)
{
	struct buffer_head **bhs = fatent->bhs;
	if (fatent->nr_bhs == 1) {
		WARN_ON(offset >= (bhs[0]->b_size - 1));
		fatent->u.ent12_p[0] = bhs[0]->b_data + offset;
		fatent->u.ent12_p[1] = bhs[0]->b_data + (offset + 1);
	} else {
		WARN_ON(offset != (bhs[0]->b_size - 1));
		fatent->u.ent12_p[0] = bhs[0]->b_data + offset;
		fatent->u.ent12_p[1] = bhs[1]->b_data;
	}
}

static void fat16_ent_set_ptr(struct fat_entry *fatent, int offset)
{
	WARN_ON(offset & (2 - 1));
	fatent->u.ent16_p = (__le16 *)(fatent->bhs[0]->b_data + offset);
}

static void fat32_ent_set_ptr(struct fat_entry *fatent, int offset)
{
	WARN_ON(offset & (4 - 1));
	fatent->u.ent32_p = (__le32 *)(fatent->bhs[0]->b_data + offset);
}

static int fat12_ent_bread(struct super_block *sb, struct fat_entry *fatent,
			   int offset, sector_t blocknr)
{
	struct buffer_head **bhs = fatent->bhs;

	WARN_ON(blocknr < MSDOS_SB(sb)->fat_start);
	fatent->fat_inode = MSDOS_SB(sb)->fat_inode;

	bhs[0] = sb_bread(sb, blocknr);
	if (!bhs[0])
		goto err;

	if ((offset + 1) < sb->s_blocksize)
		fatent->nr_bhs = 1;
	else {
		/* This entry is block boundary, it needs the next block */
		blocknr++;
		bhs[1] = sb_bread(sb, blocknr);
		if (!bhs[1])
			goto err_brelse;
		fatent->nr_bhs = 2;
	}
	fat12_ent_set_ptr(fatent, offset);
	return 0;

err_brelse:
	brelse(bhs[0]);
err:
	fat_msg(sb, KERN_ERR, "FAT read failed (blocknr %llu)", (llu)blocknr);
	return -EIO;
}

static int fat_ent_bread(struct super_block *sb, struct fat_entry *fatent,
			 int offset, sector_t blocknr)
{
	const struct fatent_operations *ops = MSDOS_SB(sb)->fatent_ops;

	WARN_ON(blocknr < MSDOS_SB(sb)->fat_start);
	fatent->fat_inode = MSDOS_SB(sb)->fat_inode;
	fatent->bhs[0] = sb_bread(sb, blocknr);
	if (!fatent->bhs[0]) {
		fat_msg(sb, KERN_ERR, "FAT read failed (blocknr %llu)",
		       (llu)blocknr);
		return -EIO;
	}
	fatent->nr_bhs = 1;
	ops->ent_set_ptr(fatent, offset);
	return 0;
}

static int fat12_ent_get(struct fat_entry *fatent)
{
	u8 **ent12_p = fatent->u.ent12_p;
	int next;

	spin_lock(&fat12_entry_lock);
	if (fatent->entry & 1)
		next = (*ent12_p[0] >> 4) | (*ent12_p[1] << 4);
	else
		next = (*ent12_p[1] << 8) | *ent12_p[0];
	spin_unlock(&fat12_entry_lock);

	next &= 0x0fff;
	if (next >= BAD_FAT12)
		next = FAT_ENT_EOF;
	return next;
}

static int fat16_ent_get(struct fat_entry *fatent)
{
	int next = le16_to_cpu(*fatent->u.ent16_p);
	WARN_ON((unsigned long)fatent->u.ent16_p & (2 - 1));
	if (next >= BAD_FAT16)
		next = FAT_ENT_EOF;
	return next;
}

static int fat32_ent_get(struct fat_entry *fatent)
{
	int next = le32_to_cpu(*fatent->u.ent32_p) & 0x0fffffff;
	WARN_ON((unsigned long)fatent->u.ent32_p & (4 - 1));
	if (next >= BAD_FAT32)
		next = FAT_ENT_EOF;
	return next;
}

static void fat12_ent_put(struct fat_entry *fatent, int new)
{
	u8 **ent12_p = fatent->u.ent12_p;

	if (new == FAT_ENT_EOF)
		new = EOF_FAT12;

	spin_lock(&fat12_entry_lock);
	if (fatent->entry & 1) {
		*ent12_p[0] = (new << 4) | (*ent12_p[0] & 0x0f);
		*ent12_p[1] = new >> 4;
	} else {
		*ent12_p[0] = new & 0xff;
		*ent12_p[1] = (*ent12_p[1] & 0xf0) | (new >> 8);
	}
	spin_unlock(&fat12_entry_lock);

	mark_buffer_dirty_inode(fatent->bhs[0], fatent->fat_inode);
	if (fatent->nr_bhs == 2)
		mark_buffer_dirty_inode(fatent->bhs[1], fatent->fat_inode);
}

static void fat16_ent_put(struct fat_entry *fatent, int new)
{
	if (new == FAT_ENT_EOF)
		new = EOF_FAT16;

	*fatent->u.ent16_p = cpu_to_le16(new);
	mark_buffer_dirty_inode(fatent->bhs[0], fatent->fat_inode);
}

static void fat32_ent_put(struct fat_entry *fatent, int new)
{
	WARN_ON(new & 0xf0000000);
	new |= le32_to_cpu(*fatent->u.ent32_p) & ~0x0fffffff;
	*fatent->u.ent32_p = cpu_to_le32(new);
	mark_buffer_dirty_inode(fatent->bhs[0], fatent->fat_inode);
}

static int fat12_ent_next(struct fat_entry *fatent)
{
	u8 **ent12_p = fatent->u.ent12_p;
	struct buffer_head **bhs = fatent->bhs;
	u8 *nextp = ent12_p[1] + 1 + (fatent->entry & 1);

	fatent->entry++;
	if (fatent->nr_bhs == 1) {
		WARN_ON(ent12_p[0] > (u8 *)(bhs[0]->b_data +
							(bhs[0]->b_size - 2)));
		WARN_ON(ent12_p[1] > (u8 *)(bhs[0]->b_data +
							(bhs[0]->b_size - 1)));
		if (nextp < (u8 *)(bhs[0]->b_data + (bhs[0]->b_size - 1))) {
			ent12_p[0] = nextp - 1;
			ent12_p[1] = nextp;
			return 1;
		}
	} else {
		WARN_ON(ent12_p[0] != (u8 *)(bhs[0]->b_data +
							(bhs[0]->b_size - 1)));
		WARN_ON(ent12_p[1] != (u8 *)bhs[1]->b_data);
		ent12_p[0] = nextp - 1;
		ent12_p[1] = nextp;
		brelse(bhs[0]);
		bhs[0] = bhs[1];
		fatent->nr_bhs = 1;
		return 1;
	}
	ent12_p[0] = NULL;
	ent12_p[1] = NULL;
	return 0;
}

static int fat16_ent_next(struct fat_entry *fatent)
{
	const struct buffer_head *bh = fatent->bhs[0];
	fatent->entry++;
	if (fatent->u.ent16_p < (__le16 *)(bh->b_data + (bh->b_size - 2))) {
		fatent->u.ent16_p++;
		return 1;
	}
	fatent->u.ent16_p = NULL;
	return 0;
}

static int fat32_ent_next(struct fat_entry *fatent)
{
	const struct buffer_head *bh = fatent->bhs[0];
	fatent->entry++;
	if (fatent->u.ent32_p < (__le32 *)(bh->b_data + (bh->b_size - 4))) {
		fatent->u.ent32_p++;
		return 1;
	}
	fatent->u.ent32_p = NULL;
	return 0;
}

static const struct fatent_operations fat12_ops = {
	.ent_blocknr	= fat12_ent_blocknr,
	.ent_set_ptr	= fat12_ent_set_ptr,
	.ent_bread	= fat12_ent_bread,
	.ent_get	= fat12_ent_get,
	.ent_put	= fat12_ent_put,
	.ent_next	= fat12_ent_next,
};

static const struct fatent_operations fat16_ops = {
	.ent_blocknr	= fat_ent_blocknr,
	.ent_set_ptr	= fat16_ent_set_ptr,
	.ent_bread	= fat_ent_bread,
	.ent_get	= fat16_ent_get,
	.ent_put	= fat16_ent_put,
	.ent_next	= fat16_ent_next,
};

static const struct fatent_operations fat32_ops = {
	.ent_blocknr	= fat_ent_blocknr,
	.ent_set_ptr	= fat32_ent_set_ptr,
	.ent_bread	= fat_ent_bread,
	.ent_get	= fat32_ent_get,
	.ent_put	= fat32_ent_put,
	.ent_next	= fat32_ent_next,
};

static inline void lock_fat(struct msdos_sb_info *sbi)
{
	mutex_lock(&sbi->fat_lock);
}

static inline void unlock_fat(struct msdos_sb_info *sbi)
{
	mutex_unlock(&sbi->fat_lock);
}

void fat_ent_access_init(struct super_block *sb)
{
	struct msdos_sb_info *sbi = MSDOS_SB(sb);

	mutex_init(&sbi->fat_lock);

	if (is_fat32(sbi)) {
		sbi->fatent_shift = 2;
		sbi->fatent_ops = &fat32_ops;
	} else if (is_fat16(sbi)) {
		sbi->fatent_shift = 1;
		sbi->fatent_ops = &fat16_ops;
	} else if (is_fat12(sbi)) {
		sbi->fatent_shift = -1;
		sbi->fatent_ops = &fat12_ops;
	} else {
		fat_fs_error(sb, "invalid FAT variant, %u bits", sbi->fat_bits);
	}
}

static void mark_fsinfo_dirty(struct super_block *sb)
{
	struct msdos_sb_info *sbi = MSDOS_SB(sb);

	if (sb_rdonly(sb) || !is_fat32(sbi))
		return;

	__mark_inode_dirty(sbi->fsinfo_inode, I_DIRTY_SYNC);
}

static inline int fat_ent_update_ptr(struct super_block *sb,
				     struct fat_entry *fatent,
				     int offset, sector_t blocknr)
{
	struct msdos_sb_info *sbi = MSDOS_SB(sb);
	const struct fatent_operations *ops = sbi->fatent_ops;
	struct buffer_head **bhs = fatent->bhs;

	/* Is this fatent's blocks including this entry? */
	if (!fatent->nr_bhs || bhs[0]->b_blocknr != blocknr)
		return 0;
	if (is_fat12(sbi)) {
		if ((offset + 1) < sb->s_blocksize) {
			/* This entry is on bhs[0]. */
			if (fatent->nr_bhs == 2) {
				brelse(bhs[1]);
				fatent->nr_bhs = 1;
			}
		} else {
			/* This entry needs the next block. */
			if (fatent->nr_bhs != 2)
				return 0;
			if (bhs[1]->b_blocknr != (blocknr + 1))
				return 0;
		}
	}
	ops->ent_set_ptr(fatent, offset);
	return 1;
}

int fat_ent_read(struct inode *inode, struct fat_entry *fatent, int entry)
{
	struct super_block *sb = inode->i_sb;
	struct msdos_sb_info *sbi = MSDOS_SB(inode->i_sb);
	const struct fatent_operations *ops = sbi->fatent_ops;
	int err, offset;
	sector_t blocknr;

	if (!fat_valid_entry(sbi, entry)) {
		fatent_brelse(fatent);
		fat_fs_error(sb, "invalid access to FAT (entry 0x%08x)", entry);
		return -EIO;
	}

	fatent_set_entry(fatent, entry);
	ops->ent_blocknr(sb, entry, &offset, &blocknr);

	if (!fat_ent_update_ptr(sb, fatent, offset, blocknr)) {
		fatent_brelse(fatent);
		err = ops->ent_bread(sb, fatent, offset, blocknr);
		if (err)
			return err;
	}
	return ops->ent_get(fatent);
}

/* FIXME: We can write the blocks as more big chunk. */
static int fat_mirror_bhs(struct super_block *sb, struct buffer_head **bhs,
			  int nr_bhs)
{
	struct msdos_sb_info *sbi = MSDOS_SB(sb);
	struct buffer_head *c_bh;
	int err, n, copy;

	err = 0;
	for (copy = 1; copy < sbi->fats; copy++) {
		sector_t backup_fat = sbi->fat_length * copy;

		for (n = 0; n < nr_bhs; n++) {
			c_bh = sb_getblk(sb, backup_fat + bhs[n]->b_blocknr);
			if (!c_bh) {
				err = -ENOMEM;
				goto error;
			}
			/* Avoid race with userspace read via bdev */
			lock_buffer(c_bh);
			memcpy(c_bh->b_data, bhs[n]->b_data, sb->s_blocksize);
			set_buffer_uptodate(c_bh);
			unlock_buffer(c_bh);
			mark_buffer_dirty_inode(c_bh, sbi->fat_inode);
			if (sb->s_flags & SB_SYNCHRONOUS)
				err = sync_dirty_buffer(c_bh);
			brelse(c_bh);
			if (err)
				goto error;
		}
	}
error:
	return err;
}

int fat_ent_write(struct inode *inode, struct fat_entry *fatent,
		  int new, int wait)
{
	struct super_block *sb = inode->i_sb;
	const struct fatent_operations *ops = MSDOS_SB(sb)->fatent_ops;
	int err;

	ops->ent_put(fatent, new);
	if (wait) {
		err = fat_sync_bhs(fatent->bhs, fatent->nr_bhs);
		if (err)
			return err;
	}
	return fat_mirror_bhs(sb, fatent->bhs, fatent->nr_bhs);
}

static inline int fat_ent_next(struct msdos_sb_info *sbi,
			       struct fat_entry *fatent)
{
	if (sbi->fatent_ops->ent_next(fatent)) {
		if (fatent->entry < sbi->max_cluster)
			return 1;
	}
	return 0;
}

static inline int fat_ent_read_block(struct super_block *sb,
				     struct fat_entry *fatent)
{
	const struct fatent_operations *ops = MSDOS_SB(sb)->fatent_ops;
	sector_t blocknr;
	int offset;

	fatent_brelse(fatent);
	ops->ent_blocknr(sb, fatent->entry, &offset, &blocknr);
	return ops->ent_bread(sb, fatent, offset, blocknr);
}

static void fat_collect_bhs(struct buffer_head **bhs, int *nr_bhs,
			    struct fat_entry *fatent)
{
	int n, i;

	for (n = 0; n < fatent->nr_bhs; n++) {
		for (i = 0; i < *nr_bhs; i++) {
			if (fatent->bhs[n] == bhs[i])
				break;
		}
		if (i == *nr_bhs) {
			get_bh(fatent->bhs[n]);
			bhs[i] = fatent->bhs[n];
			(*nr_bhs)++;
		}
	}
}

int fat_alloc_clusters(struct inode *inode, int *cluster, int nr_cluster)
{
	struct super_block *sb = inode->i_sb;
	struct msdos_sb_info *sbi = MSDOS_SB(sb);
	const struct fatent_operations *ops = sbi->fatent_ops;
	struct fat_entry fatent, prev_ent;
	struct buffer_head *bhs[MAX_BUF_PER_PAGE];
	int i, count, err, nr_bhs, idx_clus;

	BUG_ON(nr_cluster > (MAX_BUF_PER_PAGE / 2));	/* fixed limit */

	lock_fat(sbi);
	if (sbi->free_clusters != -1 && sbi->free_clus_valid &&
	    sbi->free_clusters < nr_cluster) {
		unlock_fat(sbi);
		return -ENOSPC;
	}

	err = nr_bhs = idx_clus = 0;
	count = FAT_START_ENT;
	fatent_init(&prev_ent);
	fatent_init(&fatent);
	fatent_set_entry(&fatent, sbi->prev_free + 1);
	while (count < sbi->max_cluster) {
		if (fatent.entry >= sbi->max_cluster)
			fatent.entry = FAT_START_ENT;
		fatent_set_entry(&fatent, fatent.entry);
		err = fat_ent_read_block(sb, &fatent);
		if (err)
			goto out;

		/* Find the free entries in a block */
		do {
			if (ops->ent_get(&fatent) == FAT_ENT_FREE) {
				int entry = fatent.entry;

				/* make the cluster chain */
				ops->ent_put(&fatent, FAT_ENT_EOF);
				if (prev_ent.nr_bhs)
					ops->ent_put(&prev_ent, entry);

				fat_collect_bhs(bhs, &nr_bhs, &fatent);

				sbi->prev_free = entry;
				if (sbi->free_clusters != -1)
					sbi->free_clusters--;

				cluster[idx_clus] = entry;
				idx_clus++;
				if (idx_clus == nr_cluster)
					goto out;

				/*
				 * fat_collect_bhs() gets ref-count of bhs,
				 * so we can still use the prev_ent.
				 */
				prev_ent = fatent;
			}
			count++;
			if (count == sbi->max_cluster)
				break;
		} while (fat_ent_next(sbi, &fatent));
	}

	/* Couldn't allocate the free entries */
	sbi->free_clusters = 0;
	sbi->free_clus_valid = 1;
	err = -ENOSPC;

out:
	unlock_fat(sbi);
	mark_fsinfo_dirty(sb);
	fatent_brelse(&fatent);
	if (!err) {
		if (inode_needs_sync(inode))
			err = fat_sync_bhs(bhs, nr_bhs);
		if (!err)
			err = fat_mirror_bhs(sb, bhs, nr_bhs);
	}
	for (i = 0; i < nr_bhs; i++)
		brelse(bhs[i]);

	if (err && idx_clus)
		fat_free_clusters(inode, cluster[0]);

	return err;
}

int fat_free_clusters(struct inode *inode, int cluster)
{
	struct super_block *sb = inode->i_sb;
	struct msdos_sb_info *sbi = MSDOS_SB(sb);
	const struct fatent_operations *ops = sbi->fatent_ops;
	struct fat_entry fatent;
	struct buffer_head *bhs[MAX_BUF_PER_PAGE];
	int i, err, nr_bhs;
	int first_cl = cluster, dirty_fsinfo = 0;

	nr_bhs = 0;
	fatent_init(&fatent);
	lock_fat(sbi);
	do {
		cluster = fat_ent_read(inode, &fatent, cluster);
		if (cluster < 0) {
			err = cluster;
			goto error;
		} else if (cluster == FAT_ENT_FREE) {
			fat_fs_error(sb, "%s: deleting FAT entry beyond EOF",
				     __func__);
			err = -EIO;
			goto error;
		}

		if (sbi->options.discard) {
			/*
			 * Issue discard for the sectors we no longer
			 * care about, batching contiguous clusters
			 * into one request
			 */
			if (cluster != fatent.entry + 1) {
				int nr_clus = fatent.entry - first_cl + 1;

				sb_issue_discard(sb,
					fat_clus_to_blknr(sbi, first_cl),
					nr_clus * sbi->sec_per_clus,
					GFP_NOFS, 0);

				first_cl = cluster;
			}
		}

		ops->ent_put(&fatent, FAT_ENT_FREE);
		if (sbi->free_clusters != -1) {
			sbi->free_clusters++;
			dirty_fsinfo = 1;
		}

		if (nr_bhs + fatent.nr_bhs > MAX_BUF_PER_PAGE) {
			if (sb->s_flags & SB_SYNCHRONOUS) {
				err = fat_sync_bhs(bhs, nr_bhs);
				if (err)
					goto error;
			}
			err = fat_mirror_bhs(sb, bhs, nr_bhs);
			if (err)
				goto error;
			for (i = 0; i < nr_bhs; i++)
				brelse(bhs[i]);
			nr_bhs = 0;
		}
		fat_collect_bhs(bhs, &nr_bhs, &fatent);
	} while (cluster != FAT_ENT_EOF);

	if (sb->s_flags & SB_SYNCHRONOUS) {
		err = fat_sync_bhs(bhs, nr_bhs);
		if (err)
			goto error;
	}
	err = fat_mirror_bhs(sb, bhs, nr_bhs);
error:
	fatent_brelse(&fatent);
	for (i = 0; i < nr_bhs; i++)
		brelse(bhs[i]);
	unlock_fat(sbi);
	if (dirty_fsinfo)
		mark_fsinfo_dirty(sb);

	return err;
}
EXPORT_SYMBOL_GPL(fat_free_clusters);

struct fatent_ra {
	sector_t cur;
	sector_t limit;

	unsigned int ra_blocks;
	sector_t ra_advance;
	sector_t ra_next;
	sector_t ra_limit;
};

static void fat_ra_init(struct super_block *sb, struct fatent_ra *ra,
			struct fat_entry *fatent, int ent_limit)
{
	struct msdos_sb_info *sbi = MSDOS_SB(sb);
	const struct fatent_operations *ops = sbi->fatent_ops;
	sector_t blocknr, block_end;
	int offset;
	/*
	 * This is the sequential read, so ra_pages * 2 (but try to
	 * align the optimal hardware IO size).
	 * [BTW, 128kb covers the whole sectors for FAT12 and FAT16]
	 */
	unsigned long ra_pages = sb->s_bdi->ra_pages;
	unsigned int reada_blocks;

	if (fatent->entry >= ent_limit)
		return;

	if (ra_pages > sb->s_bdi->io_pages)
		ra_pages = rounddown(ra_pages, sb->s_bdi->io_pages);
	reada_blocks = ra_pages << (PAGE_SHIFT - sb->s_blocksize_bits + 1);

	/* Initialize the range for sequential read */
	ops->ent_blocknr(sb, fatent->entry, &offset, &blocknr);
	ops->ent_blocknr(sb, ent_limit - 1, &offset, &block_end);
	ra->cur = 0;
	ra->limit = (block_end + 1) - blocknr;

	/* Advancing the window at half size */
	ra->ra_blocks = reada_blocks >> 1;
	ra->ra_advance = ra->cur;
	ra->ra_next = ra->cur;
	ra->ra_limit = ra->cur + min_t(sector_t, reada_blocks, ra->limit);
}

/* Assuming to be called before reading a new block (increments ->cur). */
static void fat_ent_reada(struct super_block *sb, struct fatent_ra *ra,
			  struct fat_entry *fatent)
{
	if (ra->ra_next >= ra->ra_limit)
		return;

	if (ra->cur >= ra->ra_advance) {
		struct msdos_sb_info *sbi = MSDOS_SB(sb);
		const struct fatent_operations *ops = sbi->fatent_ops;
		struct blk_plug plug;
		sector_t blocknr, diff;
		int offset;

		ops->ent_blocknr(sb, fatent->entry, &offset, &blocknr);

		diff = blocknr - ra->cur;
		blk_start_plug(&plug);
		/*
		 * FIXME: we would want to directly use the bio with
		 * pages to reduce the number of segments.
		 */
		for (; ra->ra_next < ra->ra_limit; ra->ra_next++)
			sb_breadahead(sb, ra->ra_next + diff);
		blk_finish_plug(&plug);

		/* Advance the readahead window */
		ra->ra_advance += ra->ra_blocks;
		ra->ra_limit += min_t(sector_t,
				      ra->ra_blocks, ra->limit - ra->ra_limit);
	}
	ra->cur++;
}

int fat_count_free_clusters(struct super_block *sb)
{
	struct msdos_sb_info *sbi = MSDOS_SB(sb);
	const struct fatent_operations *ops = sbi->fatent_ops;
	struct fat_entry fatent;
	struct fatent_ra fatent_ra;
	int err = 0, free;

	lock_fat(sbi);
	if (sbi->free_clusters != -1 && sbi->free_clus_valid)
		goto out;

	free = 0;
	fatent_init(&fatent);
	fatent_set_entry(&fatent, FAT_START_ENT);
	fat_ra_init(sb, &fatent_ra, &fatent, sbi->max_cluster);
	while (fatent.entry < sbi->max_cluster) {
		/* readahead of fat blocks */
		fat_ent_reada(sb, &fatent_ra, &fatent);

		err = fat_ent_read_block(sb, &fatent);
		if (err)
			goto out;

		do {
			if (ops->ent_get(&fatent) == FAT_ENT_FREE)
				free++;
		} while (fat_ent_next(sbi, &fatent));
		cond_resched();
	}
	sbi->free_clusters = free;
	sbi->free_clus_valid = 1;
	mark_fsinfo_dirty(sb);
	fatent_brelse(&fatent);
out:
	unlock_fat(sbi);
	return err;
}

static int fat_trim_clusters(struct super_block *sb, u32 clus, u32 nr_clus)
{
	struct msdos_sb_info *sbi = MSDOS_SB(sb);
	return sb_issue_discard(sb, fat_clus_to_blknr(sbi, clus),
				nr_clus * sbi->sec_per_clus, GFP_NOFS, 0);
}

int fat_trim_fs(struct inode *inode, struct fstrim_range *range)
{
	struct super_block *sb = inode->i_sb;
	struct msdos_sb_info *sbi = MSDOS_SB(sb);
	const struct fatent_operations *ops = sbi->fatent_ops;
	struct fat_entry fatent;
	struct fatent_ra fatent_ra;
	u64 ent_start, ent_end, minlen, trimmed = 0;
	u32 free = 0;
	int err = 0;

	/*
	 * FAT data is organized as clusters, trim at the granulary of cluster.
	 *
	 * fstrim_range is in byte, convert values to cluster index.
	 * Treat sectors before data region as all used, not to trim them.
	 */
	ent_start = max_t(u64, range->start>>sbi->cluster_bits, FAT_START_ENT);
	ent_end = ent_start + (range->len >> sbi->cluster_bits) - 1;
	minlen = range->minlen >> sbi->cluster_bits;

	if (ent_start >= sbi->max_cluster || range->len < sbi->cluster_size)
		return -EINVAL;
	if (ent_end >= sbi->max_cluster)
		ent_end = sbi->max_cluster - 1;

	fatent_init(&fatent);
	lock_fat(sbi);
	fatent_set_entry(&fatent, ent_start);
	fat_ra_init(sb, &fatent_ra, &fatent, ent_end + 1);
	while (fatent.entry <= ent_end) {
		/* readahead of fat blocks */
		fat_ent_reada(sb, &fatent_ra, &fatent);

		err = fat_ent_read_block(sb, &fatent);
		if (err)
			goto error;
		do {
			if (ops->ent_get(&fatent) == FAT_ENT_FREE) {
				free++;
			} else if (free) {
				if (free >= minlen) {
					u32 clus = fatent.entry - free;

					err = fat_trim_clusters(sb, clus, free);
					if (err && err != -EOPNOTSUPP)
						goto error;
					if (!err)
						trimmed += free;
					err = 0;
				}
				free = 0;
			}
		} while (fat_ent_next(sbi, &fatent) && fatent.entry <= ent_end);

		if (fatal_signal_pending(current)) {
			err = -ERESTARTSYS;
			goto error;
		}

		if (need_resched()) {
			fatent_brelse(&fatent);
			unlock_fat(sbi);
			cond_resched();
			lock_fat(sbi);
		}
	}
	/* handle scenario when tail entries are all free */
	if (free && free >= minlen) {
		u32 clus = fatent.entry - free;

		err = fat_trim_clusters(sb, clus, free);
		if (err && err != -EOPNOTSUPP)
			goto error;
		if (!err)
			trimmed += free;
		err = 0;
	}

error:
	fatent_brelse(&fatent);
	unlock_fat(sbi);

	range->len = trimmed << sbi->cluster_bits;

	return err;
}
