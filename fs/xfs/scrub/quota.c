// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2017 Oracle.  All Rights Reserved.
 * Author: Darrick J. Wong <darrick.wong@oracle.com>
 */
#include "xfs.h"
#include "xfs_fs.h"
#include "xfs_shared.h"
#include "xfs_format.h"
#include "xfs_trans_resv.h"
#include "xfs_mount.h"
#include "xfs_log_format.h"
#include "xfs_trans.h"
#include "xfs_inode.h"
#include "xfs_quota.h"
#include "xfs_qm.h"
#include "scrub/scrub.h"
#include "scrub/common.h"

/* Convert a scrub type code to a DQ flag, or return 0 if error. */
static inline xfs_dqtype_t
xchk_quota_to_dqtype(
	struct xfs_scrub	*sc)
{
	switch (sc->sm->sm_type) {
	case XFS_SCRUB_TYPE_UQUOTA:
		return XFS_DQTYPE_USER;
	case XFS_SCRUB_TYPE_GQUOTA:
		return XFS_DQTYPE_GROUP;
	case XFS_SCRUB_TYPE_PQUOTA:
		return XFS_DQTYPE_PROJ;
	default:
		return 0;
	}
}

/* Set us up to scrub a quota. */
int
xchk_setup_quota(
	struct xfs_scrub	*sc)
{
	xfs_dqtype_t		dqtype;
	int			error;

	if (!XFS_IS_QUOTA_RUNNING(sc->mp) || !XFS_IS_QUOTA_ON(sc->mp))
		return -ENOENT;

	dqtype = xchk_quota_to_dqtype(sc);
	if (dqtype == 0)
		return -EINVAL;
	sc->flags |= XCHK_HAS_QUOTAOFFLOCK;
	mutex_lock(&sc->mp->m_quotainfo->qi_quotaofflock);
	if (!xfs_this_quota_on(sc->mp, dqtype))
		return -ENOENT;
	error = xchk_setup_fs(sc);
	if (error)
		return error;
	sc->ip = xfs_quota_inode(sc->mp, dqtype);
	xfs_ilock(sc->ip, XFS_ILOCK_EXCL);
	sc->ilock_flags = XFS_ILOCK_EXCL;
	return 0;
}

/* Quotas. */

struct xchk_quota_info {
	struct xfs_scrub	*sc;
	xfs_dqid_t		last_id;
};

/* Scrub the fields in an individual quota item. */
STATIC int
xchk_quota_item(
	struct xfs_dquot	*dq,
	xfs_dqtype_t		dqtype,
	void			*priv)
{
	struct xchk_quota_info	*sqi = priv;
	struct xfs_scrub	*sc = sqi->sc;
	struct xfs_mount	*mp = sc->mp;
	struct xfs_quotainfo	*qi = mp->m_quotainfo;
	xfs_fileoff_t		offset;
	xfs_ino_t		fs_icount;
	int			error = 0;

	if (xchk_should_terminate(sc, &error))
		return -ECANCELED;

	/*
	 * Except for the root dquot, the actual dquot we got must either have
	 * the same or higher id as we saw before.
	 */
	offset = dq->q_id / qi->qi_dqperchunk;
	if (dq->q_id && dq->q_id <= sqi->last_id)
		xchk_fblock_set_corrupt(sc, XFS_DATA_FORK, offset);

	sqi->last_id = dq->q_id;

	/*
	 * Warn if the hard limits are larger than the fs.
	 * Administrators can do this, though in production this seems
	 * suspect, which is why we flag it for review.
	 *
	 * Complain about corruption if the soft limit is greater than
	 * the hard limit.
	 */
	if (dq->q_blk.hardlimit > mp->m_sb.sb_dblocks)
		xchk_fblock_set_warning(sc, XFS_DATA_FORK, offset);
	if (dq->q_blk.softlimit > dq->q_blk.hardlimit)
		xchk_fblock_set_corrupt(sc, XFS_DATA_FORK, offset);

	if (dq->q_ino.hardlimit > M_IGEO(mp)->maxicount)
		xchk_fblock_set_warning(sc, XFS_DATA_FORK, offset);
	if (dq->q_ino.softlimit > dq->q_ino.hardlimit)
		xchk_fblock_set_corrupt(sc, XFS_DATA_FORK, offset);

	if (dq->q_rtb.hardlimit > mp->m_sb.sb_rblocks)
		xchk_fblock_set_warning(sc, XFS_DATA_FORK, offset);
	if (dq->q_rtb.softlimit > dq->q_rtb.hardlimit)
		xchk_fblock_set_corrupt(sc, XFS_DATA_FORK, offset);

	/* Check the resource counts. */
	fs_icount = percpu_counter_sum(&mp->m_icount);

	/*
	 * Check that usage doesn't exceed physical limits.  However, on
	 * a reflink filesystem we're allowed to exceed physical space
	 * if there are no quota limits.
	 */
	if (xfs_sb_version_hasreflink(&mp->m_sb)) {
		if (mp->m_sb.sb_dblocks < dq->q_blk.count)
			xchk_fblock_set_warning(sc, XFS_DATA_FORK,
					offset);
	} else {
		if (mp->m_sb.sb_dblocks < dq->q_blk.count)
			xchk_fblock_set_corrupt(sc, XFS_DATA_FORK,
					offset);
	}
	if (dq->q_ino.count > fs_icount || dq->q_rtb.count > mp->m_sb.sb_rblocks)
		xchk_fblock_set_corrupt(sc, XFS_DATA_FORK, offset);

	/*
	 * We can violate the hard limits if the admin suddenly sets a
	 * lower limit than the actual usage.  However, we flag it for
	 * admin review.
	 */
	if (dq->q_id == 0)
		goto out;

	if (dq->q_blk.hardlimit != 0 &&
	    dq->q_blk.count > dq->q_blk.hardlimit)
		xchk_fblock_set_warning(sc, XFS_DATA_FORK, offset);

	if (dq->q_ino.hardlimit != 0 &&
	    dq->q_ino.count > dq->q_ino.hardlimit)
		xchk_fblock_set_warning(sc, XFS_DATA_FORK, offset);

	if (dq->q_rtb.hardlimit != 0 &&
	    dq->q_rtb.count > dq->q_rtb.hardlimit)
		xchk_fblock_set_warning(sc, XFS_DATA_FORK, offset);

out:
	if (sc->sm->sm_flags & XFS_SCRUB_OFLAG_CORRUPT)
		return -ECANCELED;

	return 0;
}

/* Check the quota's data fork. */
STATIC int
xchk_quota_data_fork(
	struct xfs_scrub	*sc)
{
	struct xfs_bmbt_irec	irec = { 0 };
	struct xfs_iext_cursor	icur;
	struct xfs_quotainfo	*qi = sc->mp->m_quotainfo;
	struct xfs_ifork	*ifp;
	xfs_fileoff_t		max_dqid_off;
	int			error = 0;

	/* Invoke the fork scrubber. */
	error = xchk_metadata_inode_forks(sc);
	if (error || (sc->sm->sm_flags & XFS_SCRUB_OFLAG_CORRUPT))
		return error;

	/* Check for data fork problems that apply only to quota files. */
	max_dqid_off = ((xfs_dqid_t)-1) / qi->qi_dqperchunk;
	ifp = XFS_IFORK_PTR(sc->ip, XFS_DATA_FORK);
	for_each_xfs_iext(ifp, &icur, &irec) {
		if (xchk_should_terminate(sc, &error))
			break;
		/*
		 * delalloc extents or blocks mapped above the highest
		 * quota id shouldn't happen.
		 */
		if (isnullstartblock(irec.br_startblock) ||
		    irec.br_startoff > max_dqid_off ||
		    irec.br_startoff + irec.br_blockcount - 1 > max_dqid_off) {
			xchk_fblock_set_corrupt(sc, XFS_DATA_FORK,
					irec.br_startoff);
			break;
		}
	}

	return error;
}

/* Scrub all of a quota type's items. */
int
xchk_quota(
	struct xfs_scrub	*sc)
{
	struct xchk_quota_info	sqi;
	struct xfs_mount	*mp = sc->mp;
	struct xfs_quotainfo	*qi = mp->m_quotainfo;
	xfs_dqtype_t		dqtype;
	int			error = 0;

	dqtype = xchk_quota_to_dqtype(sc);

	/* Look for problem extents. */
	error = xchk_quota_data_fork(sc);
	if (error)
		goto out;
	if (sc->sm->sm_flags & XFS_SCRUB_OFLAG_CORRUPT)
		goto out;

	/*
	 * Check all the quota items.  Now that we've checked the quota inode
	 * data fork we have to drop ILOCK_EXCL to use the regular dquot
	 * functions.
	 */
	xfs_iunlock(sc->ip, sc->ilock_flags);
	sc->ilock_flags = 0;
	sqi.sc = sc;
	sqi.last_id = 0;
	error = xfs_qm_dqiterate(mp, dqtype, xchk_quota_item, &sqi);
	sc->ilock_flags = XFS_ILOCK_EXCL;
	xfs_ilock(sc->ip, sc->ilock_flags);
	if (error == -ECANCELED)
		error = 0;
	if (!xchk_fblock_process_error(sc, XFS_DATA_FORK,
			sqi.last_id * qi->qi_dqperchunk, &error))
		goto out;

out:
	return error;
}
