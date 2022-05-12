// SPDX-License-Identifier: GPL-2.0
/*
 *  linux/fs/hpfs/buffer.c
 *
 *  Mikulas Patocka (mikulas@artax.karlin.mff.cuni.cz), 1998-1999
 *
 *  general buffer i/o
 */
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/blkdev.h>
#include "hpfs_fn.h"

secno hpfs_search_hotfix_map(struct super_block *s, secno sec)
{
	unsigned i;
	struct hpfs_sb_info *sbi = hpfs_sb(s);
	for (i = 0; unlikely(i < sbi->n_hotfixes); i++) {
		if (sbi->hotfix_from[i] == sec) {
			return sbi->hotfix_to[i];
		}
	}
	return sec;
}

unsigned hpfs_search_hotfix_map_for_range(struct super_block *s, secno sec, unsigned n)
{
	unsigned i;
	struct hpfs_sb_info *sbi = hpfs_sb(s);
	for (i = 0; unlikely(i < sbi->n_hotfixes); i++) {
		if (sbi->hotfix_from[i] >= sec && sbi->hotfix_from[i] < sec + n) {
			n = sbi->hotfix_from[i] - sec;
		}
	}
	return n;
}

void hpfs_prefetch_sectors(struct super_block *s, unsigned secno, int n)
{
	struct buffer_head *bh;
	struct blk_plug plug;

	if (n <= 0 || unlikely(secno >= hpfs_sb(s)->sb_fs_size))
		return;

	if (unlikely(hpfs_search_hotfix_map_for_range(s, secno, n) != n))
		return;

	bh = sb_find_get_block(s, secno);
	if (bh) {
		if (buffer_uptodate(bh)) {
			brelse(bh);
			return;
		}
		brelse(bh);
	}

	blk_start_plug(&plug);
	while (n > 0) {
		if (unlikely(secno >= hpfs_sb(s)->sb_fs_size))
			break;
		sb_breadahead(s, secno);
		secno++;
		n--;
	}
	blk_finish_plug(&plug);
}

/* Map a sector into a buffer and return pointers to it and to the buffer. */

void *hpfs_map_sector(struct super_block *s, unsigned secno, struct buffer_head **bhp,
		 int ahead)
{
	struct buffer_head *bh;

	hpfs_lock_assert(s);

	hpfs_prefetch_sectors(s, secno, ahead);

	cond_resched();

	*bhp = bh = sb_bread(s, hpfs_search_hotfix_map(s, secno));
	if (bh != NULL)
		return bh->b_data;
	else {
		pr_err("%s(): read error\n", __func__);
		return NULL;
	}
}

/* Like hpfs_map_sector but don't read anything */

void *hpfs_get_sector(struct super_block *s, unsigned secno, struct buffer_head **bhp)
{
	struct buffer_head *bh;
	/*return hpfs_map_sector(s, secno, bhp, 0);*/

	hpfs_lock_assert(s);

	cond_resched();

	if ((*bhp = bh = sb_getblk(s, hpfs_search_hotfix_map(s, secno))) != NULL) {
		if (!buffer_uptodate(bh)) wait_on_buffer(bh);
		set_buffer_uptodate(bh);
		return bh->b_data;
	} else {
		pr_err("%s(): getblk failed\n", __func__);
		return NULL;
	}
}

/* Map 4 sectors into a 4buffer and return pointers to it and to the buffer. */

void *hpfs_map_4sectors(struct super_block *s, unsigned secno, struct quad_buffer_head *qbh,
		   int ahead)
{
	char *data;

	hpfs_lock_assert(s);

	cond_resched();

	if (secno & 3) {
		pr_err("%s(): unaligned read\n", __func__);
		return NULL;
	}

	hpfs_prefetch_sectors(s, secno, 4 + ahead);

	if (!hpfs_map_sector(s, secno + 0, &qbh->bh[0], 0)) goto bail0;
	if (!hpfs_map_sector(s, secno + 1, &qbh->bh[1], 0)) goto bail1;
	if (!hpfs_map_sector(s, secno + 2, &qbh->bh[2], 0)) goto bail2;
	if (!hpfs_map_sector(s, secno + 3, &qbh->bh[3], 0)) goto bail3;

	if (likely(qbh->bh[1]->b_data == qbh->bh[0]->b_data + 1 * 512) &&
	    likely(qbh->bh[2]->b_data == qbh->bh[0]->b_data + 2 * 512) &&
	    likely(qbh->bh[3]->b_data == qbh->bh[0]->b_data + 3 * 512)) {
		return qbh->data = qbh->bh[0]->b_data;
	}

	qbh->data = data = kmalloc(2048, GFP_NOFS);
	if (!data) {
		pr_err("%s(): out of memory\n", __func__);
		goto bail4;
	}

	memcpy(data + 0 * 512, qbh->bh[0]->b_data, 512);
	memcpy(data + 1 * 512, qbh->bh[1]->b_data, 512);
	memcpy(data + 2 * 512, qbh->bh[2]->b_data, 512);
	memcpy(data + 3 * 512, qbh->bh[3]->b_data, 512);

	return data;

 bail4:
	brelse(qbh->bh[3]);
 bail3:
	brelse(qbh->bh[2]);
 bail2:
	brelse(qbh->bh[1]);
 bail1:
	brelse(qbh->bh[0]);
 bail0:
	return NULL;
}

/* Don't read sectors */

void *hpfs_get_4sectors(struct super_block *s, unsigned secno,
                          struct quad_buffer_head *qbh)
{
	cond_resched();

	hpfs_lock_assert(s);

	if (secno & 3) {
		pr_err("%s(): unaligned read\n", __func__);
		return NULL;
	}

	if (!hpfs_get_sector(s, secno + 0, &qbh->bh[0])) goto bail0;
	if (!hpfs_get_sector(s, secno + 1, &qbh->bh[1])) goto bail1;
	if (!hpfs_get_sector(s, secno + 2, &qbh->bh[2])) goto bail2;
	if (!hpfs_get_sector(s, secno + 3, &qbh->bh[3])) goto bail3;

	if (likely(qbh->bh[1]->b_data == qbh->bh[0]->b_data + 1 * 512) &&
	    likely(qbh->bh[2]->b_data == qbh->bh[0]->b_data + 2 * 512) &&
	    likely(qbh->bh[3]->b_data == qbh->bh[0]->b_data + 3 * 512)) {
		return qbh->data = qbh->bh[0]->b_data;
	}

	if (!(qbh->data = kmalloc(2048, GFP_NOFS))) {
		pr_err("%s(): out of memory\n", __func__);
		goto bail4;
	}
	return qbh->data;

bail4:
	brelse(qbh->bh[3]);
bail3:
	brelse(qbh->bh[2]);
bail2:
	brelse(qbh->bh[1]);
bail1:
	brelse(qbh->bh[0]);
bail0:
	return NULL;
}
	

void hpfs_brelse4(struct quad_buffer_head *qbh)
{
	if (unlikely(qbh->data != qbh->bh[0]->b_data))
		kfree(qbh->data);
	brelse(qbh->bh[0]);
	brelse(qbh->bh[1]);
	brelse(qbh->bh[2]);
	brelse(qbh->bh[3]);
}	

void hpfs_mark_4buffers_dirty(struct quad_buffer_head *qbh)
{
	if (unlikely(qbh->data != qbh->bh[0]->b_data)) {
		memcpy(qbh->bh[0]->b_data, qbh->data + 0 * 512, 512);
		memcpy(qbh->bh[1]->b_data, qbh->data + 1 * 512, 512);
		memcpy(qbh->bh[2]->b_data, qbh->data + 2 * 512, 512);
		memcpy(qbh->bh[3]->b_data, qbh->data + 3 * 512, 512);
	}
	mark_buffer_dirty(qbh->bh[0]);
	mark_buffer_dirty(qbh->bh[1]);
	mark_buffer_dirty(qbh->bh[2]);
	mark_buffer_dirty(qbh->bh[3]);
}
