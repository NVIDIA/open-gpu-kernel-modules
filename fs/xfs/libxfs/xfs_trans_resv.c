// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2000-2003,2005 Silicon Graphics, Inc.
 * Copyright (C) 2010 Red Hat, Inc.
 * All Rights Reserved.
 */
#include "xfs.h"
#include "xfs_fs.h"
#include "xfs_shared.h"
#include "xfs_format.h"
#include "xfs_log_format.h"
#include "xfs_trans_resv.h"
#include "xfs_mount.h"
#include "xfs_da_format.h"
#include "xfs_da_btree.h"
#include "xfs_inode.h"
#include "xfs_bmap_btree.h"
#include "xfs_quota.h"
#include "xfs_trans.h"
#include "xfs_qm.h"
#include "xfs_trans_space.h"

#define _ALLOC	true
#define _FREE	false

/*
 * A buffer has a format structure overhead in the log in addition
 * to the data, so we need to take this into account when reserving
 * space in a transaction for a buffer.  Round the space required up
 * to a multiple of 128 bytes so that we don't change the historical
 * reservation that has been used for this overhead.
 */
STATIC uint
xfs_buf_log_overhead(void)
{
	return round_up(sizeof(struct xlog_op_header) +
			sizeof(struct xfs_buf_log_format), 128);
}

/*
 * Calculate out transaction log reservation per item in bytes.
 *
 * The nbufs argument is used to indicate the number of items that
 * will be changed in a transaction.  size is used to tell how many
 * bytes should be reserved per item.
 */
STATIC uint
xfs_calc_buf_res(
	uint		nbufs,
	uint		size)
{
	return nbufs * (size + xfs_buf_log_overhead());
}

/*
 * Per-extent log reservation for the btree changes involved in freeing or
 * allocating an extent.  In classic XFS there were two trees that will be
 * modified (bnobt + cntbt).  With rmap enabled, there are three trees
 * (rmapbt).  With reflink, there are four trees (refcountbt).  The number of
 * blocks reserved is based on the formula:
 *
 * num trees * ((2 blocks/level * max depth) - 1)
 *
 * Keep in mind that max depth is calculated separately for each type of tree.
 */
uint
xfs_allocfree_log_count(
	struct xfs_mount *mp,
	uint		num_ops)
{
	uint		blocks;

	blocks = num_ops * 2 * (2 * mp->m_ag_maxlevels - 1);
	if (xfs_sb_version_hasrmapbt(&mp->m_sb))
		blocks += num_ops * (2 * mp->m_rmap_maxlevels - 1);
	if (xfs_sb_version_hasreflink(&mp->m_sb))
		blocks += num_ops * (2 * mp->m_refc_maxlevels - 1);

	return blocks;
}

/*
 * Logging inodes is really tricksy. They are logged in memory format,
 * which means that what we write into the log doesn't directly translate into
 * the amount of space they use on disk.
 *
 * Case in point - btree format forks in memory format use more space than the
 * on-disk format. In memory, the buffer contains a normal btree block header so
 * the btree code can treat it as though it is just another generic buffer.
 * However, when we write it to the inode fork, we don't write all of this
 * header as it isn't needed. e.g. the root is only ever in the inode, so
 * there's no need for sibling pointers which would waste 16 bytes of space.
 *
 * Hence when we have an inode with a maximally sized btree format fork, then
 * amount of information we actually log is greater than the size of the inode
 * on disk. Hence we need an inode reservation function that calculates all this
 * correctly. So, we log:
 *
 * - 4 log op headers for object
 *	- for the ilf, the inode core and 2 forks
 * - inode log format object
 * - the inode core
 * - two inode forks containing bmap btree root blocks.
 *	- the btree data contained by both forks will fit into the inode size,
 *	  hence when combined with the inode core above, we have a total of the
 *	  actual inode size.
 *	- the BMBT headers need to be accounted separately, as they are
 *	  additional to the records and pointers that fit inside the inode
 *	  forks.
 */
STATIC uint
xfs_calc_inode_res(
	struct xfs_mount	*mp,
	uint			ninodes)
{
	return ninodes *
		(4 * sizeof(struct xlog_op_header) +
		 sizeof(struct xfs_inode_log_format) +
		 mp->m_sb.sb_inodesize +
		 2 * XFS_BMBT_BLOCK_LEN(mp));
}

/*
 * Inode btree record insertion/removal modifies the inode btree and free space
 * btrees (since the inobt does not use the agfl). This requires the following
 * reservation:
 *
 * the inode btree: max depth * blocksize
 * the allocation btrees: 2 trees * (max depth - 1) * block size
 *
 * The caller must account for SB and AG header modifications, etc.
 */
STATIC uint
xfs_calc_inobt_res(
	struct xfs_mount	*mp)
{
	return xfs_calc_buf_res(M_IGEO(mp)->inobt_maxlevels,
			XFS_FSB_TO_B(mp, 1)) +
				xfs_calc_buf_res(xfs_allocfree_log_count(mp, 1),
			XFS_FSB_TO_B(mp, 1));
}

/*
 * The free inode btree is a conditional feature. The behavior differs slightly
 * from that of the traditional inode btree in that the finobt tracks records
 * for inode chunks with at least one free inode. A record can be removed from
 * the tree during individual inode allocation. Therefore the finobt
 * reservation is unconditional for both the inode chunk allocation and
 * individual inode allocation (modify) cases.
 *
 * Behavior aside, the reservation for finobt modification is equivalent to the
 * traditional inobt: cover a full finobt shape change plus block allocation.
 */
STATIC uint
xfs_calc_finobt_res(
	struct xfs_mount	*mp)
{
	if (!xfs_sb_version_hasfinobt(&mp->m_sb))
		return 0;

	return xfs_calc_inobt_res(mp);
}

/*
 * Calculate the reservation required to allocate or free an inode chunk. This
 * includes:
 *
 * the allocation btrees: 2 trees * (max depth - 1) * block size
 * the inode chunk: m_ino_geo.ialloc_blks * N
 *
 * The size N of the inode chunk reservation depends on whether it is for
 * allocation or free and which type of create transaction is in use. An inode
 * chunk free always invalidates the buffers and only requires reservation for
 * headers (N == 0). An inode chunk allocation requires a chunk sized
 * reservation on v4 and older superblocks to initialize the chunk. No chunk
 * reservation is required for allocation on v5 supers, which use ordered
 * buffers to initialize.
 */
STATIC uint
xfs_calc_inode_chunk_res(
	struct xfs_mount	*mp,
	bool			alloc)
{
	uint			res, size = 0;

	res = xfs_calc_buf_res(xfs_allocfree_log_count(mp, 1),
			       XFS_FSB_TO_B(mp, 1));
	if (alloc) {
		/* icreate tx uses ordered buffers */
		if (xfs_sb_version_has_v3inode(&mp->m_sb))
			return res;
		size = XFS_FSB_TO_B(mp, 1);
	}

	res += xfs_calc_buf_res(M_IGEO(mp)->ialloc_blks, size);
	return res;
}

/*
 * Per-extent log reservation for the btree changes involved in freeing or
 * allocating a realtime extent.  We have to be able to log as many rtbitmap
 * blocks as needed to mark inuse MAXEXTLEN blocks' worth of realtime extents,
 * as well as the realtime summary block.
 */
static unsigned int
xfs_rtalloc_log_count(
	struct xfs_mount	*mp,
	unsigned int		num_ops)
{
	unsigned int		blksz = XFS_FSB_TO_B(mp, 1);
	unsigned int		rtbmp_bytes;

	rtbmp_bytes = (MAXEXTLEN / mp->m_sb.sb_rextsize) / NBBY;
	return (howmany(rtbmp_bytes, blksz) + 1) * num_ops;
}

/*
 * Various log reservation values.
 *
 * These are based on the size of the file system block because that is what
 * most transactions manipulate.  Each adds in an additional 128 bytes per
 * item logged to try to account for the overhead of the transaction mechanism.
 *
 * Note:  Most of the reservations underestimate the number of allocation
 * groups into which they could free extents in the xfs_defer_finish() call.
 * This is because the number in the worst case is quite high and quite
 * unusual.  In order to fix this we need to change xfs_defer_finish() to free
 * extents in only a single AG at a time.  This will require changes to the
 * EFI code as well, however, so that the EFI for the extents not freed is
 * logged again in each transaction.  See SGI PV #261917.
 *
 * Reservation functions here avoid a huge stack in xfs_trans_init due to
 * register overflow from temporaries in the calculations.
 */


/*
 * In a write transaction we can allocate a maximum of 2
 * extents.  This gives (t1):
 *    the inode getting the new extents: inode size
 *    the inode's bmap btree: max depth * block size
 *    the agfs of the ags from which the extents are allocated: 2 * sector
 *    the superblock free block counter: sector size
 *    the allocation btrees: 2 exts * 2 trees * (2 * max depth - 1) * block size
 * Or, if we're writing to a realtime file (t2):
 *    the inode getting the new extents: inode size
 *    the inode's bmap btree: max depth * block size
 *    the agfs of the ags from which the extents are allocated: 2 * sector
 *    the superblock free block counter: sector size
 *    the realtime bitmap: ((MAXEXTLEN / rtextsize) / NBBY) bytes
 *    the realtime summary: 1 block
 *    the allocation btrees: 2 trees * (2 * max depth - 1) * block size
 * And the bmap_finish transaction can free bmap blocks in a join (t3):
 *    the agfs of the ags containing the blocks: 2 * sector size
 *    the agfls of the ags containing the blocks: 2 * sector size
 *    the super block free block counter: sector size
 *    the allocation btrees: 2 exts * 2 trees * (2 * max depth - 1) * block size
 */
STATIC uint
xfs_calc_write_reservation(
	struct xfs_mount	*mp)
{
	unsigned int		t1, t2, t3;
	unsigned int		blksz = XFS_FSB_TO_B(mp, 1);

	t1 = xfs_calc_inode_res(mp, 1) +
	     xfs_calc_buf_res(XFS_BM_MAXLEVELS(mp, XFS_DATA_FORK), blksz) +
	     xfs_calc_buf_res(3, mp->m_sb.sb_sectsize) +
	     xfs_calc_buf_res(xfs_allocfree_log_count(mp, 2), blksz);

	if (xfs_sb_version_hasrealtime(&mp->m_sb)) {
		t2 = xfs_calc_inode_res(mp, 1) +
		     xfs_calc_buf_res(XFS_BM_MAXLEVELS(mp, XFS_DATA_FORK),
				     blksz) +
		     xfs_calc_buf_res(3, mp->m_sb.sb_sectsize) +
		     xfs_calc_buf_res(xfs_rtalloc_log_count(mp, 1), blksz) +
		     xfs_calc_buf_res(xfs_allocfree_log_count(mp, 1), blksz);
	} else {
		t2 = 0;
	}

	t3 = xfs_calc_buf_res(5, mp->m_sb.sb_sectsize) +
	     xfs_calc_buf_res(xfs_allocfree_log_count(mp, 2), blksz);

	return XFS_DQUOT_LOGRES(mp) + max3(t1, t2, t3);
}

/*
 * In truncating a file we free up to two extents at once.  We can modify (t1):
 *    the inode being truncated: inode size
 *    the inode's bmap btree: (max depth + 1) * block size
 * And the bmap_finish transaction can free the blocks and bmap blocks (t2):
 *    the agf for each of the ags: 4 * sector size
 *    the agfl for each of the ags: 4 * sector size
 *    the super block to reflect the freed blocks: sector size
 *    worst case split in allocation btrees per extent assuming 4 extents:
 *		4 exts * 2 trees * (2 * max depth - 1) * block size
 * Or, if it's a realtime file (t3):
 *    the agf for each of the ags: 2 * sector size
 *    the agfl for each of the ags: 2 * sector size
 *    the super block to reflect the freed blocks: sector size
 *    the realtime bitmap: 2 exts * ((MAXEXTLEN / rtextsize) / NBBY) bytes
 *    the realtime summary: 2 exts * 1 block
 *    worst case split in allocation btrees per extent assuming 2 extents:
 *		2 exts * 2 trees * (2 * max depth - 1) * block size
 */
STATIC uint
xfs_calc_itruncate_reservation(
	struct xfs_mount	*mp)
{
	unsigned int		t1, t2, t3;
	unsigned int		blksz = XFS_FSB_TO_B(mp, 1);

	t1 = xfs_calc_inode_res(mp, 1) +
	     xfs_calc_buf_res(XFS_BM_MAXLEVELS(mp, XFS_DATA_FORK) + 1, blksz);

	t2 = xfs_calc_buf_res(9, mp->m_sb.sb_sectsize) +
	     xfs_calc_buf_res(xfs_allocfree_log_count(mp, 4), blksz);

	if (xfs_sb_version_hasrealtime(&mp->m_sb)) {
		t3 = xfs_calc_buf_res(5, mp->m_sb.sb_sectsize) +
		     xfs_calc_buf_res(xfs_rtalloc_log_count(mp, 2), blksz) +
		     xfs_calc_buf_res(xfs_allocfree_log_count(mp, 2), blksz);
	} else {
		t3 = 0;
	}

	return XFS_DQUOT_LOGRES(mp) + max3(t1, t2, t3);
}

/*
 * In renaming a files we can modify:
 *    the four inodes involved: 4 * inode size
 *    the two directory btrees: 2 * (max depth + v2) * dir block size
 *    the two directory bmap btrees: 2 * max depth * block size
 * And the bmap_finish transaction can free dir and bmap blocks (two sets
 *	of bmap blocks) giving:
 *    the agf for the ags in which the blocks live: 3 * sector size
 *    the agfl for the ags in which the blocks live: 3 * sector size
 *    the superblock for the free block count: sector size
 *    the allocation btrees: 3 exts * 2 trees * (2 * max depth - 1) * block size
 */
STATIC uint
xfs_calc_rename_reservation(
	struct xfs_mount	*mp)
{
	return XFS_DQUOT_LOGRES(mp) +
		max((xfs_calc_inode_res(mp, 4) +
		     xfs_calc_buf_res(2 * XFS_DIROP_LOG_COUNT(mp),
				      XFS_FSB_TO_B(mp, 1))),
		    (xfs_calc_buf_res(7, mp->m_sb.sb_sectsize) +
		     xfs_calc_buf_res(xfs_allocfree_log_count(mp, 3),
				      XFS_FSB_TO_B(mp, 1))));
}

/*
 * For removing an inode from unlinked list at first, we can modify:
 *    the agi hash list and counters: sector size
 *    the on disk inode before ours in the agi hash list: inode cluster size
 *    the on disk inode in the agi hash list: inode cluster size
 */
STATIC uint
xfs_calc_iunlink_remove_reservation(
	struct xfs_mount        *mp)
{
	return xfs_calc_buf_res(1, mp->m_sb.sb_sectsize) +
	       2 * M_IGEO(mp)->inode_cluster_size;
}

/*
 * For creating a link to an inode:
 *    the parent directory inode: inode size
 *    the linked inode: inode size
 *    the directory btree could split: (max depth + v2) * dir block size
 *    the directory bmap btree could join or split: (max depth + v2) * blocksize
 * And the bmap_finish transaction can free some bmap blocks giving:
 *    the agf for the ag in which the blocks live: sector size
 *    the agfl for the ag in which the blocks live: sector size
 *    the superblock for the free block count: sector size
 *    the allocation btrees: 2 trees * (2 * max depth - 1) * block size
 */
STATIC uint
xfs_calc_link_reservation(
	struct xfs_mount	*mp)
{
	return XFS_DQUOT_LOGRES(mp) +
		xfs_calc_iunlink_remove_reservation(mp) +
		max((xfs_calc_inode_res(mp, 2) +
		     xfs_calc_buf_res(XFS_DIROP_LOG_COUNT(mp),
				      XFS_FSB_TO_B(mp, 1))),
		    (xfs_calc_buf_res(3, mp->m_sb.sb_sectsize) +
		     xfs_calc_buf_res(xfs_allocfree_log_count(mp, 1),
				      XFS_FSB_TO_B(mp, 1))));
}

/*
 * For adding an inode to unlinked list we can modify:
 *    the agi hash list: sector size
 *    the on disk inode: inode cluster size
 */
STATIC uint
xfs_calc_iunlink_add_reservation(xfs_mount_t *mp)
{
	return xfs_calc_buf_res(1, mp->m_sb.sb_sectsize) +
			M_IGEO(mp)->inode_cluster_size;
}

/*
 * For removing a directory entry we can modify:
 *    the parent directory inode: inode size
 *    the removed inode: inode size
 *    the directory btree could join: (max depth + v2) * dir block size
 *    the directory bmap btree could join or split: (max depth + v2) * blocksize
 * And the bmap_finish transaction can free the dir and bmap blocks giving:
 *    the agf for the ag in which the blocks live: 2 * sector size
 *    the agfl for the ag in which the blocks live: 2 * sector size
 *    the superblock for the free block count: sector size
 *    the allocation btrees: 2 exts * 2 trees * (2 * max depth - 1) * block size
 */
STATIC uint
xfs_calc_remove_reservation(
	struct xfs_mount	*mp)
{
	return XFS_DQUOT_LOGRES(mp) +
		xfs_calc_iunlink_add_reservation(mp) +
		max((xfs_calc_inode_res(mp, 1) +
		     xfs_calc_buf_res(XFS_DIROP_LOG_COUNT(mp),
				      XFS_FSB_TO_B(mp, 1))),
		    (xfs_calc_buf_res(4, mp->m_sb.sb_sectsize) +
		     xfs_calc_buf_res(xfs_allocfree_log_count(mp, 2),
				      XFS_FSB_TO_B(mp, 1))));
}

/*
 * For create, break it in to the two cases that the transaction
 * covers. We start with the modify case - allocation done by modification
 * of the state of existing inodes - and the allocation case.
 */

/*
 * For create we can modify:
 *    the parent directory inode: inode size
 *    the new inode: inode size
 *    the inode btree entry: block size
 *    the superblock for the nlink flag: sector size
 *    the directory btree: (max depth + v2) * dir block size
 *    the directory inode's bmap btree: (max depth + v2) * block size
 *    the finobt (record modification and allocation btrees)
 */
STATIC uint
xfs_calc_create_resv_modify(
	struct xfs_mount	*mp)
{
	return xfs_calc_inode_res(mp, 2) +
		xfs_calc_buf_res(1, mp->m_sb.sb_sectsize) +
		(uint)XFS_FSB_TO_B(mp, 1) +
		xfs_calc_buf_res(XFS_DIROP_LOG_COUNT(mp), XFS_FSB_TO_B(mp, 1)) +
		xfs_calc_finobt_res(mp);
}

/*
 * For icreate we can allocate some inodes giving:
 *    the agi and agf of the ag getting the new inodes: 2 * sectorsize
 *    the superblock for the nlink flag: sector size
 *    the inode chunk (allocation, optional init)
 *    the inobt (record insertion)
 *    the finobt (optional, record insertion)
 */
STATIC uint
xfs_calc_icreate_resv_alloc(
	struct xfs_mount	*mp)
{
	return xfs_calc_buf_res(2, mp->m_sb.sb_sectsize) +
		mp->m_sb.sb_sectsize +
		xfs_calc_inode_chunk_res(mp, _ALLOC) +
		xfs_calc_inobt_res(mp) +
		xfs_calc_finobt_res(mp);
}

STATIC uint
xfs_calc_icreate_reservation(xfs_mount_t *mp)
{
	return XFS_DQUOT_LOGRES(mp) +
		max(xfs_calc_icreate_resv_alloc(mp),
		    xfs_calc_create_resv_modify(mp));
}

STATIC uint
xfs_calc_create_tmpfile_reservation(
	struct xfs_mount        *mp)
{
	uint	res = XFS_DQUOT_LOGRES(mp);

	res += xfs_calc_icreate_resv_alloc(mp);
	return res + xfs_calc_iunlink_add_reservation(mp);
}

/*
 * Making a new directory is the same as creating a new file.
 */
STATIC uint
xfs_calc_mkdir_reservation(
	struct xfs_mount	*mp)
{
	return xfs_calc_icreate_reservation(mp);
}


/*
 * Making a new symplink is the same as creating a new file, but
 * with the added blocks for remote symlink data which can be up to 1kB in
 * length (XFS_SYMLINK_MAXLEN).
 */
STATIC uint
xfs_calc_symlink_reservation(
	struct xfs_mount	*mp)
{
	return xfs_calc_icreate_reservation(mp) +
	       xfs_calc_buf_res(1, XFS_SYMLINK_MAXLEN);
}

/*
 * In freeing an inode we can modify:
 *    the inode being freed: inode size
 *    the super block free inode counter, AGF and AGFL: sector size
 *    the on disk inode (agi unlinked list removal)
 *    the inode chunk (invalidated, headers only)
 *    the inode btree
 *    the finobt (record insertion, removal or modification)
 *
 * Note that the inode chunk res. includes an allocfree res. for freeing of the
 * inode chunk. This is technically extraneous because the inode chunk free is
 * deferred (it occurs after a transaction roll). Include the extra reservation
 * anyways since we've had reports of ifree transaction overruns due to too many
 * agfl fixups during inode chunk frees.
 */
STATIC uint
xfs_calc_ifree_reservation(
	struct xfs_mount	*mp)
{
	return XFS_DQUOT_LOGRES(mp) +
		xfs_calc_inode_res(mp, 1) +
		xfs_calc_buf_res(3, mp->m_sb.sb_sectsize) +
		xfs_calc_iunlink_remove_reservation(mp) +
		xfs_calc_inode_chunk_res(mp, _FREE) +
		xfs_calc_inobt_res(mp) +
		xfs_calc_finobt_res(mp);
}

/*
 * When only changing the inode we log the inode and possibly the superblock
 * We also add a bit of slop for the transaction stuff.
 */
STATIC uint
xfs_calc_ichange_reservation(
	struct xfs_mount	*mp)
{
	return XFS_DQUOT_LOGRES(mp) +
		xfs_calc_inode_res(mp, 1) +
		xfs_calc_buf_res(1, mp->m_sb.sb_sectsize);

}

/*
 * Growing the data section of the filesystem.
 *	superblock
 *	agi and agf
 *	allocation btrees
 */
STATIC uint
xfs_calc_growdata_reservation(
	struct xfs_mount	*mp)
{
	return xfs_calc_buf_res(3, mp->m_sb.sb_sectsize) +
		xfs_calc_buf_res(xfs_allocfree_log_count(mp, 1),
				 XFS_FSB_TO_B(mp, 1));
}

/*
 * Growing the rt section of the filesystem.
 * In the first set of transactions (ALLOC) we allocate space to the
 * bitmap or summary files.
 *	superblock: sector size
 *	agf of the ag from which the extent is allocated: sector size
 *	bmap btree for bitmap/summary inode: max depth * blocksize
 *	bitmap/summary inode: inode size
 *	allocation btrees for 1 block alloc: 2 * (2 * maxdepth - 1) * blocksize
 */
STATIC uint
xfs_calc_growrtalloc_reservation(
	struct xfs_mount	*mp)
{
	return xfs_calc_buf_res(2, mp->m_sb.sb_sectsize) +
		xfs_calc_buf_res(XFS_BM_MAXLEVELS(mp, XFS_DATA_FORK),
				 XFS_FSB_TO_B(mp, 1)) +
		xfs_calc_inode_res(mp, 1) +
		xfs_calc_buf_res(xfs_allocfree_log_count(mp, 1),
				 XFS_FSB_TO_B(mp, 1));
}

/*
 * Growing the rt section of the filesystem.
 * In the second set of transactions (ZERO) we zero the new metadata blocks.
 *	one bitmap/summary block: blocksize
 */
STATIC uint
xfs_calc_growrtzero_reservation(
	struct xfs_mount	*mp)
{
	return xfs_calc_buf_res(1, mp->m_sb.sb_blocksize);
}

/*
 * Growing the rt section of the filesystem.
 * In the third set of transactions (FREE) we update metadata without
 * allocating any new blocks.
 *	superblock: sector size
 *	bitmap inode: inode size
 *	summary inode: inode size
 *	one bitmap block: blocksize
 *	summary blocks: new summary size
 */
STATIC uint
xfs_calc_growrtfree_reservation(
	struct xfs_mount	*mp)
{
	return xfs_calc_buf_res(1, mp->m_sb.sb_sectsize) +
		xfs_calc_inode_res(mp, 2) +
		xfs_calc_buf_res(1, mp->m_sb.sb_blocksize) +
		xfs_calc_buf_res(1, mp->m_rsumsize);
}

/*
 * Logging the inode modification timestamp on a synchronous write.
 *	inode
 */
STATIC uint
xfs_calc_swrite_reservation(
	struct xfs_mount	*mp)
{
	return xfs_calc_inode_res(mp, 1);
}

/*
 * Logging the inode mode bits when writing a setuid/setgid file
 *	inode
 */
STATIC uint
xfs_calc_writeid_reservation(
	struct xfs_mount	*mp)
{
	return xfs_calc_inode_res(mp, 1);
}

/*
 * Converting the inode from non-attributed to attributed.
 *	the inode being converted: inode size
 *	agf block and superblock (for block allocation)
 *	the new block (directory sized)
 *	bmap blocks for the new directory block
 *	allocation btrees
 */
STATIC uint
xfs_calc_addafork_reservation(
	struct xfs_mount	*mp)
{
	return XFS_DQUOT_LOGRES(mp) +
		xfs_calc_inode_res(mp, 1) +
		xfs_calc_buf_res(2, mp->m_sb.sb_sectsize) +
		xfs_calc_buf_res(1, mp->m_dir_geo->blksize) +
		xfs_calc_buf_res(XFS_DAENTER_BMAP1B(mp, XFS_DATA_FORK) + 1,
				 XFS_FSB_TO_B(mp, 1)) +
		xfs_calc_buf_res(xfs_allocfree_log_count(mp, 1),
				 XFS_FSB_TO_B(mp, 1));
}

/*
 * Removing the attribute fork of a file
 *    the inode being truncated: inode size
 *    the inode's bmap btree: max depth * block size
 * And the bmap_finish transaction can free the blocks and bmap blocks:
 *    the agf for each of the ags: 4 * sector size
 *    the agfl for each of the ags: 4 * sector size
 *    the super block to reflect the freed blocks: sector size
 *    worst case split in allocation btrees per extent assuming 4 extents:
 *		4 exts * 2 trees * (2 * max depth - 1) * block size
 */
STATIC uint
xfs_calc_attrinval_reservation(
	struct xfs_mount	*mp)
{
	return max((xfs_calc_inode_res(mp, 1) +
		    xfs_calc_buf_res(XFS_BM_MAXLEVELS(mp, XFS_ATTR_FORK),
				     XFS_FSB_TO_B(mp, 1))),
		   (xfs_calc_buf_res(9, mp->m_sb.sb_sectsize) +
		    xfs_calc_buf_res(xfs_allocfree_log_count(mp, 4),
				     XFS_FSB_TO_B(mp, 1))));
}

/*
 * Setting an attribute at mount time.
 *	the inode getting the attribute
 *	the superblock for allocations
 *	the agfs extents are allocated from
 *	the attribute btree * max depth
 *	the inode allocation btree
 * Since attribute transaction space is dependent on the size of the attribute,
 * the calculation is done partially at mount time and partially at runtime(see
 * below).
 */
STATIC uint
xfs_calc_attrsetm_reservation(
	struct xfs_mount	*mp)
{
	return XFS_DQUOT_LOGRES(mp) +
		xfs_calc_inode_res(mp, 1) +
		xfs_calc_buf_res(1, mp->m_sb.sb_sectsize) +
		xfs_calc_buf_res(XFS_DA_NODE_MAXDEPTH, XFS_FSB_TO_B(mp, 1));
}

/*
 * Setting an attribute at runtime, transaction space unit per block.
 * 	the superblock for allocations: sector size
 *	the inode bmap btree could join or split: max depth * block size
 * Since the runtime attribute transaction space is dependent on the total
 * blocks needed for the 1st bmap, here we calculate out the space unit for
 * one block so that the caller could figure out the total space according
 * to the attibute extent length in blocks by:
 *	ext * M_RES(mp)->tr_attrsetrt.tr_logres
 */
STATIC uint
xfs_calc_attrsetrt_reservation(
	struct xfs_mount	*mp)
{
	return xfs_calc_buf_res(1, mp->m_sb.sb_sectsize) +
		xfs_calc_buf_res(XFS_BM_MAXLEVELS(mp, XFS_ATTR_FORK),
				 XFS_FSB_TO_B(mp, 1));
}

/*
 * Removing an attribute.
 *    the inode: inode size
 *    the attribute btree could join: max depth * block size
 *    the inode bmap btree could join or split: max depth * block size
 * And the bmap_finish transaction can free the attr blocks freed giving:
 *    the agf for the ag in which the blocks live: 2 * sector size
 *    the agfl for the ag in which the blocks live: 2 * sector size
 *    the superblock for the free block count: sector size
 *    the allocation btrees: 2 exts * 2 trees * (2 * max depth - 1) * block size
 */
STATIC uint
xfs_calc_attrrm_reservation(
	struct xfs_mount	*mp)
{
	return XFS_DQUOT_LOGRES(mp) +
		max((xfs_calc_inode_res(mp, 1) +
		     xfs_calc_buf_res(XFS_DA_NODE_MAXDEPTH,
				      XFS_FSB_TO_B(mp, 1)) +
		     (uint)XFS_FSB_TO_B(mp,
					XFS_BM_MAXLEVELS(mp, XFS_ATTR_FORK)) +
		     xfs_calc_buf_res(XFS_BM_MAXLEVELS(mp, XFS_DATA_FORK), 0)),
		    (xfs_calc_buf_res(5, mp->m_sb.sb_sectsize) +
		     xfs_calc_buf_res(xfs_allocfree_log_count(mp, 2),
				      XFS_FSB_TO_B(mp, 1))));
}

/*
 * Clearing a bad agino number in an agi hash bucket.
 */
STATIC uint
xfs_calc_clear_agi_bucket_reservation(
	struct xfs_mount	*mp)
{
	return xfs_calc_buf_res(1, mp->m_sb.sb_sectsize);
}

/*
 * Adjusting quota limits.
 *    the disk quota buffer: sizeof(struct xfs_disk_dquot)
 */
STATIC uint
xfs_calc_qm_setqlim_reservation(void)
{
	return xfs_calc_buf_res(1, sizeof(struct xfs_disk_dquot));
}

/*
 * Allocating quota on disk if needed.
 *	the write transaction log space for quota file extent allocation
 *	the unit of quota allocation: one system block size
 */
STATIC uint
xfs_calc_qm_dqalloc_reservation(
	struct xfs_mount	*mp)
{
	return xfs_calc_write_reservation(mp) +
		xfs_calc_buf_res(1,
			XFS_FSB_TO_B(mp, XFS_DQUOT_CLUSTER_SIZE_FSB) - 1);
}

/*
 * Turning off quotas.
 *    the quota off logitems: sizeof(struct xfs_qoff_logitem) * 2
 *    the superblock for the quota flags: sector size
 */
STATIC uint
xfs_calc_qm_quotaoff_reservation(
	struct xfs_mount	*mp)
{
	return sizeof(struct xfs_qoff_logitem) * 2 +
		xfs_calc_buf_res(1, mp->m_sb.sb_sectsize);
}

/*
 * End of turning off quotas.
 *    the quota off logitems: sizeof(struct xfs_qoff_logitem) * 2
 */
STATIC uint
xfs_calc_qm_quotaoff_end_reservation(void)
{
	return sizeof(struct xfs_qoff_logitem) * 2;
}

/*
 * Syncing the incore super block changes to disk.
 *     the super block to reflect the changes: sector size
 */
STATIC uint
xfs_calc_sb_reservation(
	struct xfs_mount	*mp)
{
	return xfs_calc_buf_res(1, mp->m_sb.sb_sectsize);
}

void
xfs_trans_resv_calc(
	struct xfs_mount	*mp,
	struct xfs_trans_resv	*resp)
{
	/*
	 * The following transactions are logged in physical format and
	 * require a permanent reservation on space.
	 */
	resp->tr_write.tr_logres = xfs_calc_write_reservation(mp);
	if (xfs_sb_version_hasreflink(&mp->m_sb))
		resp->tr_write.tr_logcount = XFS_WRITE_LOG_COUNT_REFLINK;
	else
		resp->tr_write.tr_logcount = XFS_WRITE_LOG_COUNT;
	resp->tr_write.tr_logflags |= XFS_TRANS_PERM_LOG_RES;

	resp->tr_itruncate.tr_logres = xfs_calc_itruncate_reservation(mp);
	if (xfs_sb_version_hasreflink(&mp->m_sb))
		resp->tr_itruncate.tr_logcount =
				XFS_ITRUNCATE_LOG_COUNT_REFLINK;
	else
		resp->tr_itruncate.tr_logcount = XFS_ITRUNCATE_LOG_COUNT;
	resp->tr_itruncate.tr_logflags |= XFS_TRANS_PERM_LOG_RES;

	resp->tr_rename.tr_logres = xfs_calc_rename_reservation(mp);
	resp->tr_rename.tr_logcount = XFS_RENAME_LOG_COUNT;
	resp->tr_rename.tr_logflags |= XFS_TRANS_PERM_LOG_RES;

	resp->tr_link.tr_logres = xfs_calc_link_reservation(mp);
	resp->tr_link.tr_logcount = XFS_LINK_LOG_COUNT;
	resp->tr_link.tr_logflags |= XFS_TRANS_PERM_LOG_RES;

	resp->tr_remove.tr_logres = xfs_calc_remove_reservation(mp);
	resp->tr_remove.tr_logcount = XFS_REMOVE_LOG_COUNT;
	resp->tr_remove.tr_logflags |= XFS_TRANS_PERM_LOG_RES;

	resp->tr_symlink.tr_logres = xfs_calc_symlink_reservation(mp);
	resp->tr_symlink.tr_logcount = XFS_SYMLINK_LOG_COUNT;
	resp->tr_symlink.tr_logflags |= XFS_TRANS_PERM_LOG_RES;

	resp->tr_create.tr_logres = xfs_calc_icreate_reservation(mp);
	resp->tr_create.tr_logcount = XFS_CREATE_LOG_COUNT;
	resp->tr_create.tr_logflags |= XFS_TRANS_PERM_LOG_RES;

	resp->tr_create_tmpfile.tr_logres =
			xfs_calc_create_tmpfile_reservation(mp);
	resp->tr_create_tmpfile.tr_logcount = XFS_CREATE_TMPFILE_LOG_COUNT;
	resp->tr_create_tmpfile.tr_logflags |= XFS_TRANS_PERM_LOG_RES;

	resp->tr_mkdir.tr_logres = xfs_calc_mkdir_reservation(mp);
	resp->tr_mkdir.tr_logcount = XFS_MKDIR_LOG_COUNT;
	resp->tr_mkdir.tr_logflags |= XFS_TRANS_PERM_LOG_RES;

	resp->tr_ifree.tr_logres = xfs_calc_ifree_reservation(mp);
	resp->tr_ifree.tr_logcount = XFS_INACTIVE_LOG_COUNT;
	resp->tr_ifree.tr_logflags |= XFS_TRANS_PERM_LOG_RES;

	resp->tr_addafork.tr_logres = xfs_calc_addafork_reservation(mp);
	resp->tr_addafork.tr_logcount = XFS_ADDAFORK_LOG_COUNT;
	resp->tr_addafork.tr_logflags |= XFS_TRANS_PERM_LOG_RES;

	resp->tr_attrinval.tr_logres = xfs_calc_attrinval_reservation(mp);
	resp->tr_attrinval.tr_logcount = XFS_ATTRINVAL_LOG_COUNT;
	resp->tr_attrinval.tr_logflags |= XFS_TRANS_PERM_LOG_RES;

	resp->tr_attrsetm.tr_logres = xfs_calc_attrsetm_reservation(mp);
	resp->tr_attrsetm.tr_logcount = XFS_ATTRSET_LOG_COUNT;
	resp->tr_attrsetm.tr_logflags |= XFS_TRANS_PERM_LOG_RES;

	resp->tr_attrrm.tr_logres = xfs_calc_attrrm_reservation(mp);
	resp->tr_attrrm.tr_logcount = XFS_ATTRRM_LOG_COUNT;
	resp->tr_attrrm.tr_logflags |= XFS_TRANS_PERM_LOG_RES;

	resp->tr_growrtalloc.tr_logres = xfs_calc_growrtalloc_reservation(mp);
	resp->tr_growrtalloc.tr_logcount = XFS_DEFAULT_PERM_LOG_COUNT;
	resp->tr_growrtalloc.tr_logflags |= XFS_TRANS_PERM_LOG_RES;

	resp->tr_qm_dqalloc.tr_logres = xfs_calc_qm_dqalloc_reservation(mp);
	if (xfs_sb_version_hasreflink(&mp->m_sb))
		resp->tr_qm_dqalloc.tr_logcount = XFS_WRITE_LOG_COUNT_REFLINK;
	else
		resp->tr_qm_dqalloc.tr_logcount = XFS_WRITE_LOG_COUNT;
	resp->tr_qm_dqalloc.tr_logflags |= XFS_TRANS_PERM_LOG_RES;

	/*
	 * The following transactions are logged in logical format with
	 * a default log count.
	 */
	resp->tr_qm_setqlim.tr_logres = xfs_calc_qm_setqlim_reservation();
	resp->tr_qm_setqlim.tr_logcount = XFS_DEFAULT_LOG_COUNT;

	resp->tr_qm_quotaoff.tr_logres = xfs_calc_qm_quotaoff_reservation(mp);
	resp->tr_qm_quotaoff.tr_logcount = XFS_DEFAULT_LOG_COUNT;

	resp->tr_qm_equotaoff.tr_logres =
		xfs_calc_qm_quotaoff_end_reservation();
	resp->tr_qm_equotaoff.tr_logcount = XFS_DEFAULT_LOG_COUNT;

	resp->tr_sb.tr_logres = xfs_calc_sb_reservation(mp);
	resp->tr_sb.tr_logcount = XFS_DEFAULT_LOG_COUNT;

	/* growdata requires permanent res; it can free space to the last AG */
	resp->tr_growdata.tr_logres = xfs_calc_growdata_reservation(mp);
	resp->tr_growdata.tr_logcount = XFS_DEFAULT_PERM_LOG_COUNT;
	resp->tr_growdata.tr_logflags |= XFS_TRANS_PERM_LOG_RES;

	/* The following transaction are logged in logical format */
	resp->tr_ichange.tr_logres = xfs_calc_ichange_reservation(mp);
	resp->tr_fsyncts.tr_logres = xfs_calc_swrite_reservation(mp);
	resp->tr_writeid.tr_logres = xfs_calc_writeid_reservation(mp);
	resp->tr_attrsetrt.tr_logres = xfs_calc_attrsetrt_reservation(mp);
	resp->tr_clearagi.tr_logres = xfs_calc_clear_agi_bucket_reservation(mp);
	resp->tr_growrtzero.tr_logres = xfs_calc_growrtzero_reservation(mp);
	resp->tr_growrtfree.tr_logres = xfs_calc_growrtfree_reservation(mp);
}
