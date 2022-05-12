// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2006-2007 Silicon Graphics, Inc.
 * Copyright (c) 2014 Christoph Hellwig.
 * All Rights Reserved.
 */
#include "xfs.h"
#include "xfs_shared.h"
#include "xfs_format.h"
#include "xfs_log_format.h"
#include "xfs_trans_resv.h"
#include "xfs_sb.h"
#include "xfs_mount.h"
#include "xfs_inode.h"
#include "xfs_bmap.h"
#include "xfs_alloc.h"
#include "xfs_mru_cache.h"
#include "xfs_trace.h"
#include "xfs_ag_resv.h"
#include "xfs_trans.h"
#include "xfs_filestream.h"

struct xfs_fstrm_item {
	struct xfs_mru_cache_elem	mru;
	xfs_agnumber_t			ag; /* AG in use for this directory */
};

enum xfs_fstrm_alloc {
	XFS_PICK_USERDATA = 1,
	XFS_PICK_LOWSPACE = 2,
};

/*
 * Allocation group filestream associations are tracked with per-ag atomic
 * counters.  These counters allow xfs_filestream_pick_ag() to tell whether a
 * particular AG already has active filestreams associated with it.
 */
int
xfs_filestream_peek_ag(
	xfs_mount_t	*mp,
	xfs_agnumber_t	agno)
{
	struct xfs_perag *pag;
	int		ret;

	pag = xfs_perag_get(mp, agno);
	ret = atomic_read(&pag->pagf_fstrms);
	xfs_perag_put(pag);
	return ret;
}

static int
xfs_filestream_get_ag(
	xfs_mount_t	*mp,
	xfs_agnumber_t	agno)
{
	struct xfs_perag *pag;
	int		ret;

	pag = xfs_perag_get(mp, agno);
	ret = atomic_inc_return(&pag->pagf_fstrms);
	xfs_perag_put(pag);
	return ret;
}

static void
xfs_filestream_put_ag(
	xfs_mount_t	*mp,
	xfs_agnumber_t	agno)
{
	struct xfs_perag *pag;

	pag = xfs_perag_get(mp, agno);
	atomic_dec(&pag->pagf_fstrms);
	xfs_perag_put(pag);
}

static void
xfs_fstrm_free_func(
	void			*data,
	struct xfs_mru_cache_elem *mru)
{
	struct xfs_mount	*mp = data;
	struct xfs_fstrm_item	*item =
		container_of(mru, struct xfs_fstrm_item, mru);

	xfs_filestream_put_ag(mp, item->ag);
	trace_xfs_filestream_free(mp, mru->key, item->ag);

	kmem_free(item);
}

/*
 * Scan the AGs starting at startag looking for an AG that isn't in use and has
 * at least minlen blocks free.
 */
static int
xfs_filestream_pick_ag(
	struct xfs_inode	*ip,
	xfs_agnumber_t		startag,
	xfs_agnumber_t		*agp,
	int			flags,
	xfs_extlen_t		minlen)
{
	struct xfs_mount	*mp = ip->i_mount;
	struct xfs_fstrm_item	*item;
	struct xfs_perag	*pag;
	xfs_extlen_t		longest, free = 0, minfree, maxfree = 0;
	xfs_agnumber_t		ag, max_ag = NULLAGNUMBER;
	int			err, trylock, nscan;

	ASSERT(S_ISDIR(VFS_I(ip)->i_mode));

	/* 2% of an AG's blocks must be free for it to be chosen. */
	minfree = mp->m_sb.sb_agblocks / 50;

	ag = startag;
	*agp = NULLAGNUMBER;

	/* For the first pass, don't sleep trying to init the per-AG. */
	trylock = XFS_ALLOC_FLAG_TRYLOCK;

	for (nscan = 0; 1; nscan++) {
		trace_xfs_filestream_scan(mp, ip->i_ino, ag);

		pag = xfs_perag_get(mp, ag);

		if (!pag->pagf_init) {
			err = xfs_alloc_pagf_init(mp, NULL, ag, trylock);
			if (err) {
				xfs_perag_put(pag);
				if (err != -EAGAIN)
					return err;
				/* Couldn't lock the AGF, skip this AG. */
				continue;
			}
		}

		/* Keep track of the AG with the most free blocks. */
		if (pag->pagf_freeblks > maxfree) {
			maxfree = pag->pagf_freeblks;
			max_ag = ag;
		}

		/*
		 * The AG reference count does two things: it enforces mutual
		 * exclusion when examining the suitability of an AG in this
		 * loop, and it guards against two filestreams being established
		 * in the same AG as each other.
		 */
		if (xfs_filestream_get_ag(mp, ag) > 1) {
			xfs_filestream_put_ag(mp, ag);
			goto next_ag;
		}

		longest = xfs_alloc_longest_free_extent(pag,
				xfs_alloc_min_freelist(mp, pag),
				xfs_ag_resv_needed(pag, XFS_AG_RESV_NONE));
		if (((minlen && longest >= minlen) ||
		     (!minlen && pag->pagf_freeblks >= minfree)) &&
		    (!pag->pagf_metadata || !(flags & XFS_PICK_USERDATA) ||
		     (flags & XFS_PICK_LOWSPACE))) {

			/* Break out, retaining the reference on the AG. */
			free = pag->pagf_freeblks;
			xfs_perag_put(pag);
			*agp = ag;
			break;
		}

		/* Drop the reference on this AG, it's not usable. */
		xfs_filestream_put_ag(mp, ag);
next_ag:
		xfs_perag_put(pag);
		/* Move to the next AG, wrapping to AG 0 if necessary. */
		if (++ag >= mp->m_sb.sb_agcount)
			ag = 0;

		/* If a full pass of the AGs hasn't been done yet, continue. */
		if (ag != startag)
			continue;

		/* Allow sleeping in xfs_alloc_pagf_init() on the 2nd pass. */
		if (trylock != 0) {
			trylock = 0;
			continue;
		}

		/* Finally, if lowspace wasn't set, set it for the 3rd pass. */
		if (!(flags & XFS_PICK_LOWSPACE)) {
			flags |= XFS_PICK_LOWSPACE;
			continue;
		}

		/*
		 * Take the AG with the most free space, regardless of whether
		 * it's already in use by another filestream.
		 */
		if (max_ag != NULLAGNUMBER) {
			xfs_filestream_get_ag(mp, max_ag);
			free = maxfree;
			*agp = max_ag;
			break;
		}

		/* take AG 0 if none matched */
		trace_xfs_filestream_pick(ip, *agp, free, nscan);
		*agp = 0;
		return 0;
	}

	trace_xfs_filestream_pick(ip, *agp, free, nscan);

	if (*agp == NULLAGNUMBER)
		return 0;

	err = -ENOMEM;
	item = kmem_alloc(sizeof(*item), KM_MAYFAIL);
	if (!item)
		goto out_put_ag;

	item->ag = *agp;

	err = xfs_mru_cache_insert(mp->m_filestream, ip->i_ino, &item->mru);
	if (err) {
		if (err == -EEXIST)
			err = 0;
		goto out_free_item;
	}

	return 0;

out_free_item:
	kmem_free(item);
out_put_ag:
	xfs_filestream_put_ag(mp, *agp);
	return err;
}

static struct xfs_inode *
xfs_filestream_get_parent(
	struct xfs_inode	*ip)
{
	struct inode		*inode = VFS_I(ip), *dir = NULL;
	struct dentry		*dentry, *parent;

	dentry = d_find_alias(inode);
	if (!dentry)
		goto out;

	parent = dget_parent(dentry);
	if (!parent)
		goto out_dput;

	dir = igrab(d_inode(parent));
	dput(parent);

out_dput:
	dput(dentry);
out:
	return dir ? XFS_I(dir) : NULL;
}

/*
 * Find the right allocation group for a file, either by finding an
 * existing file stream or creating a new one.
 *
 * Returns NULLAGNUMBER in case of an error.
 */
xfs_agnumber_t
xfs_filestream_lookup_ag(
	struct xfs_inode	*ip)
{
	struct xfs_mount	*mp = ip->i_mount;
	struct xfs_inode	*pip = NULL;
	xfs_agnumber_t		startag, ag = NULLAGNUMBER;
	struct xfs_mru_cache_elem *mru;

	ASSERT(S_ISREG(VFS_I(ip)->i_mode));

	pip = xfs_filestream_get_parent(ip);
	if (!pip)
		return NULLAGNUMBER;

	mru = xfs_mru_cache_lookup(mp->m_filestream, pip->i_ino);
	if (mru) {
		ag = container_of(mru, struct xfs_fstrm_item, mru)->ag;
		xfs_mru_cache_done(mp->m_filestream);

		trace_xfs_filestream_lookup(mp, ip->i_ino, ag);
		goto out;
	}

	/*
	 * Set the starting AG using the rotor for inode32, otherwise
	 * use the directory inode's AG.
	 */
	if (mp->m_flags & XFS_MOUNT_32BITINODES) {
		xfs_agnumber_t	 rotorstep = xfs_rotorstep;
		startag = (mp->m_agfrotor / rotorstep) % mp->m_sb.sb_agcount;
		mp->m_agfrotor = (mp->m_agfrotor + 1) %
		                 (mp->m_sb.sb_agcount * rotorstep);
	} else
		startag = XFS_INO_TO_AGNO(mp, pip->i_ino);

	if (xfs_filestream_pick_ag(pip, startag, &ag, 0, 0))
		ag = NULLAGNUMBER;
out:
	xfs_irele(pip);
	return ag;
}

/*
 * Pick a new allocation group for the current file and its file stream.
 *
 * This is called when the allocator can't find a suitable extent in the
 * current AG, and we have to move the stream into a new AG with more space.
 */
int
xfs_filestream_new_ag(
	struct xfs_bmalloca	*ap,
	xfs_agnumber_t		*agp)
{
	struct xfs_inode	*ip = ap->ip, *pip;
	struct xfs_mount	*mp = ip->i_mount;
	xfs_extlen_t		minlen = ap->length;
	xfs_agnumber_t		startag = 0;
	int			flags = 0;
	int			err = 0;
	struct xfs_mru_cache_elem *mru;

	*agp = NULLAGNUMBER;

	pip = xfs_filestream_get_parent(ip);
	if (!pip)
		goto exit;

	mru = xfs_mru_cache_remove(mp->m_filestream, pip->i_ino);
	if (mru) {
		struct xfs_fstrm_item *item =
			container_of(mru, struct xfs_fstrm_item, mru);
		startag = (item->ag + 1) % mp->m_sb.sb_agcount;
	}

	if (ap->datatype & XFS_ALLOC_USERDATA)
		flags |= XFS_PICK_USERDATA;
	if (ap->tp->t_flags & XFS_TRANS_LOWMODE)
		flags |= XFS_PICK_LOWSPACE;

	err = xfs_filestream_pick_ag(pip, startag, agp, flags, minlen);

	/*
	 * Only free the item here so we skip over the old AG earlier.
	 */
	if (mru)
		xfs_fstrm_free_func(mp, mru);

	xfs_irele(pip);
exit:
	if (*agp == NULLAGNUMBER)
		*agp = 0;
	return err;
}

void
xfs_filestream_deassociate(
	struct xfs_inode	*ip)
{
	xfs_mru_cache_delete(ip->i_mount->m_filestream, ip->i_ino);
}

int
xfs_filestream_mount(
	xfs_mount_t	*mp)
{
	/*
	 * The filestream timer tunable is currently fixed within the range of
	 * one second to four minutes, with five seconds being the default.  The
	 * group count is somewhat arbitrary, but it'd be nice to adhere to the
	 * timer tunable to within about 10 percent.  This requires at least 10
	 * groups.
	 */
	return xfs_mru_cache_create(&mp->m_filestream, mp,
			xfs_fstrm_centisecs * 10, 10, xfs_fstrm_free_func);
}

void
xfs_filestream_unmount(
	xfs_mount_t	*mp)
{
	xfs_mru_cache_destroy(mp->m_filestream);
}
