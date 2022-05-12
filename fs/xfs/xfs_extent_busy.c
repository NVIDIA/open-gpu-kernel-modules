// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2000-2002,2005 Silicon Graphics, Inc.
 * Copyright (c) 2010 David Chinner.
 * Copyright (c) 2011 Christoph Hellwig.
 * All Rights Reserved.
 */
#include "xfs.h"
#include "xfs_fs.h"
#include "xfs_format.h"
#include "xfs_log_format.h"
#include "xfs_shared.h"
#include "xfs_trans_resv.h"
#include "xfs_sb.h"
#include "xfs_mount.h"
#include "xfs_alloc.h"
#include "xfs_extent_busy.h"
#include "xfs_trace.h"
#include "xfs_trans.h"
#include "xfs_log.h"

void
xfs_extent_busy_insert(
	struct xfs_trans	*tp,
	xfs_agnumber_t		agno,
	xfs_agblock_t		bno,
	xfs_extlen_t		len,
	unsigned int		flags)
{
	struct xfs_extent_busy	*new;
	struct xfs_extent_busy	*busyp;
	struct xfs_perag	*pag;
	struct rb_node		**rbp;
	struct rb_node		*parent = NULL;

	new = kmem_zalloc(sizeof(struct xfs_extent_busy), 0);
	new->agno = agno;
	new->bno = bno;
	new->length = len;
	INIT_LIST_HEAD(&new->list);
	new->flags = flags;

	/* trace before insert to be able to see failed inserts */
	trace_xfs_extent_busy(tp->t_mountp, agno, bno, len);

	pag = xfs_perag_get(tp->t_mountp, new->agno);
	spin_lock(&pag->pagb_lock);
	rbp = &pag->pagb_tree.rb_node;
	while (*rbp) {
		parent = *rbp;
		busyp = rb_entry(parent, struct xfs_extent_busy, rb_node);

		if (new->bno < busyp->bno) {
			rbp = &(*rbp)->rb_left;
			ASSERT(new->bno + new->length <= busyp->bno);
		} else if (new->bno > busyp->bno) {
			rbp = &(*rbp)->rb_right;
			ASSERT(bno >= busyp->bno + busyp->length);
		} else {
			ASSERT(0);
		}
	}

	rb_link_node(&new->rb_node, parent, rbp);
	rb_insert_color(&new->rb_node, &pag->pagb_tree);

	list_add(&new->list, &tp->t_busy);
	spin_unlock(&pag->pagb_lock);
	xfs_perag_put(pag);
}

/*
 * Search for a busy extent within the range of the extent we are about to
 * allocate.  You need to be holding the busy extent tree lock when calling
 * xfs_extent_busy_search(). This function returns 0 for no overlapping busy
 * extent, -1 for an overlapping but not exact busy extent, and 1 for an exact
 * match. This is done so that a non-zero return indicates an overlap that
 * will require a synchronous transaction, but it can still be
 * used to distinguish between a partial or exact match.
 */
int
xfs_extent_busy_search(
	struct xfs_mount	*mp,
	xfs_agnumber_t		agno,
	xfs_agblock_t		bno,
	xfs_extlen_t		len)
{
	struct xfs_perag	*pag;
	struct rb_node		*rbp;
	struct xfs_extent_busy	*busyp;
	int			match = 0;

	pag = xfs_perag_get(mp, agno);
	spin_lock(&pag->pagb_lock);

	rbp = pag->pagb_tree.rb_node;

	/* find closest start bno overlap */
	while (rbp) {
		busyp = rb_entry(rbp, struct xfs_extent_busy, rb_node);
		if (bno < busyp->bno) {
			/* may overlap, but exact start block is lower */
			if (bno + len > busyp->bno)
				match = -1;
			rbp = rbp->rb_left;
		} else if (bno > busyp->bno) {
			/* may overlap, but exact start block is higher */
			if (bno < busyp->bno + busyp->length)
				match = -1;
			rbp = rbp->rb_right;
		} else {
			/* bno matches busyp, length determines exact match */
			match = (busyp->length == len) ? 1 : -1;
			break;
		}
	}
	spin_unlock(&pag->pagb_lock);
	xfs_perag_put(pag);
	return match;
}

/*
 * The found free extent [fbno, fend] overlaps part or all of the given busy
 * extent.  If the overlap covers the beginning, the end, or all of the busy
 * extent, the overlapping portion can be made unbusy and used for the
 * allocation.  We can't split a busy extent because we can't modify a
 * transaction/CIL context busy list, but we can update an entry's block
 * number or length.
 *
 * Returns true if the extent can safely be reused, or false if the search
 * needs to be restarted.
 */
STATIC bool
xfs_extent_busy_update_extent(
	struct xfs_mount	*mp,
	struct xfs_perag	*pag,
	struct xfs_extent_busy	*busyp,
	xfs_agblock_t		fbno,
	xfs_extlen_t		flen,
	bool			userdata) __releases(&pag->pagb_lock)
					  __acquires(&pag->pagb_lock)
{
	xfs_agblock_t		fend = fbno + flen;
	xfs_agblock_t		bbno = busyp->bno;
	xfs_agblock_t		bend = bbno + busyp->length;

	/*
	 * This extent is currently being discarded.  Give the thread
	 * performing the discard a chance to mark the extent unbusy
	 * and retry.
	 */
	if (busyp->flags & XFS_EXTENT_BUSY_DISCARDED) {
		spin_unlock(&pag->pagb_lock);
		delay(1);
		spin_lock(&pag->pagb_lock);
		return false;
	}

	/*
	 * If there is a busy extent overlapping a user allocation, we have
	 * no choice but to force the log and retry the search.
	 *
	 * Fortunately this does not happen during normal operation, but
	 * only if the filesystem is very low on space and has to dip into
	 * the AGFL for normal allocations.
	 */
	if (userdata)
		goto out_force_log;

	if (bbno < fbno && bend > fend) {
		/*
		 * Case 1:
		 *    bbno           bend
		 *    +BBBBBBBBBBBBBBBBB+
		 *        +---------+
		 *        fbno   fend
		 */

		/*
		 * We would have to split the busy extent to be able to track
		 * it correct, which we cannot do because we would have to
		 * modify the list of busy extents attached to the transaction
		 * or CIL context, which is immutable.
		 *
		 * Force out the log to clear the busy extent and retry the
		 * search.
		 */
		goto out_force_log;
	} else if (bbno >= fbno && bend <= fend) {
		/*
		 * Case 2:
		 *    bbno           bend
		 *    +BBBBBBBBBBBBBBBBB+
		 *    +-----------------+
		 *    fbno           fend
		 *
		 * Case 3:
		 *    bbno           bend
		 *    +BBBBBBBBBBBBBBBBB+
		 *    +--------------------------+
		 *    fbno                    fend
		 *
		 * Case 4:
		 *             bbno           bend
		 *             +BBBBBBBBBBBBBBBBB+
		 *    +--------------------------+
		 *    fbno                    fend
		 *
		 * Case 5:
		 *             bbno           bend
		 *             +BBBBBBBBBBBBBBBBB+
		 *    +-----------------------------------+
		 *    fbno                             fend
		 *
		 */

		/*
		 * The busy extent is fully covered by the extent we are
		 * allocating, and can simply be removed from the rbtree.
		 * However we cannot remove it from the immutable list
		 * tracking busy extents in the transaction or CIL context,
		 * so set the length to zero to mark it invalid.
		 *
		 * We also need to restart the busy extent search from the
		 * tree root, because erasing the node can rearrange the
		 * tree topology.
		 */
		rb_erase(&busyp->rb_node, &pag->pagb_tree);
		busyp->length = 0;
		return false;
	} else if (fend < bend) {
		/*
		 * Case 6:
		 *              bbno           bend
		 *             +BBBBBBBBBBBBBBBBB+
		 *             +---------+
		 *             fbno   fend
		 *
		 * Case 7:
		 *             bbno           bend
		 *             +BBBBBBBBBBBBBBBBB+
		 *    +------------------+
		 *    fbno            fend
		 *
		 */
		busyp->bno = fend;
	} else if (bbno < fbno) {
		/*
		 * Case 8:
		 *    bbno           bend
		 *    +BBBBBBBBBBBBBBBBB+
		 *        +-------------+
		 *        fbno       fend
		 *
		 * Case 9:
		 *    bbno           bend
		 *    +BBBBBBBBBBBBBBBBB+
		 *        +----------------------+
		 *        fbno                fend
		 */
		busyp->length = fbno - busyp->bno;
	} else {
		ASSERT(0);
	}

	trace_xfs_extent_busy_reuse(mp, pag->pag_agno, fbno, flen);
	return true;

out_force_log:
	spin_unlock(&pag->pagb_lock);
	xfs_log_force(mp, XFS_LOG_SYNC);
	trace_xfs_extent_busy_force(mp, pag->pag_agno, fbno, flen);
	spin_lock(&pag->pagb_lock);
	return false;
}


/*
 * For a given extent [fbno, flen], make sure we can reuse it safely.
 */
void
xfs_extent_busy_reuse(
	struct xfs_mount	*mp,
	xfs_agnumber_t		agno,
	xfs_agblock_t		fbno,
	xfs_extlen_t		flen,
	bool			userdata)
{
	struct xfs_perag	*pag;
	struct rb_node		*rbp;

	ASSERT(flen > 0);

	pag = xfs_perag_get(mp, agno);
	spin_lock(&pag->pagb_lock);
restart:
	rbp = pag->pagb_tree.rb_node;
	while (rbp) {
		struct xfs_extent_busy *busyp =
			rb_entry(rbp, struct xfs_extent_busy, rb_node);
		xfs_agblock_t	bbno = busyp->bno;
		xfs_agblock_t	bend = bbno + busyp->length;

		if (fbno + flen <= bbno) {
			rbp = rbp->rb_left;
			continue;
		} else if (fbno >= bend) {
			rbp = rbp->rb_right;
			continue;
		}

		if (!xfs_extent_busy_update_extent(mp, pag, busyp, fbno, flen,
						  userdata))
			goto restart;
	}
	spin_unlock(&pag->pagb_lock);
	xfs_perag_put(pag);
}

/*
 * For a given extent [fbno, flen], search the busy extent list to find a
 * subset of the extent that is not busy.  If *rlen is smaller than
 * args->minlen no suitable extent could be found, and the higher level
 * code needs to force out the log and retry the allocation.
 *
 * Return the current busy generation for the AG if the extent is busy. This
 * value can be used to wait for at least one of the currently busy extents
 * to be cleared. Note that the busy list is not guaranteed to be empty after
 * the gen is woken. The state of a specific extent must always be confirmed
 * with another call to xfs_extent_busy_trim() before it can be used.
 */
bool
xfs_extent_busy_trim(
	struct xfs_alloc_arg	*args,
	xfs_agblock_t		*bno,
	xfs_extlen_t		*len,
	unsigned		*busy_gen)
{
	xfs_agblock_t		fbno;
	xfs_extlen_t		flen;
	struct rb_node		*rbp;
	bool			ret = false;

	ASSERT(*len > 0);

	spin_lock(&args->pag->pagb_lock);
	fbno = *bno;
	flen = *len;
	rbp = args->pag->pagb_tree.rb_node;
	while (rbp && flen >= args->minlen) {
		struct xfs_extent_busy *busyp =
			rb_entry(rbp, struct xfs_extent_busy, rb_node);
		xfs_agblock_t	fend = fbno + flen;
		xfs_agblock_t	bbno = busyp->bno;
		xfs_agblock_t	bend = bbno + busyp->length;

		if (fend <= bbno) {
			rbp = rbp->rb_left;
			continue;
		} else if (fbno >= bend) {
			rbp = rbp->rb_right;
			continue;
		}

		if (bbno <= fbno) {
			/* start overlap */

			/*
			 * Case 1:
			 *    bbno           bend
			 *    +BBBBBBBBBBBBBBBBB+
			 *        +---------+
			 *        fbno   fend
			 *
			 * Case 2:
			 *    bbno           bend
			 *    +BBBBBBBBBBBBBBBBB+
			 *    +-------------+
			 *    fbno       fend
			 *
			 * Case 3:
			 *    bbno           bend
			 *    +BBBBBBBBBBBBBBBBB+
			 *        +-------------+
			 *        fbno       fend
			 *
			 * Case 4:
			 *    bbno           bend
			 *    +BBBBBBBBBBBBBBBBB+
			 *    +-----------------+
			 *    fbno           fend
			 *
			 * No unbusy region in extent, return failure.
			 */
			if (fend <= bend)
				goto fail;

			/*
			 * Case 5:
			 *    bbno           bend
			 *    +BBBBBBBBBBBBBBBBB+
			 *        +----------------------+
			 *        fbno                fend
			 *
			 * Case 6:
			 *    bbno           bend
			 *    +BBBBBBBBBBBBBBBBB+
			 *    +--------------------------+
			 *    fbno                    fend
			 *
			 * Needs to be trimmed to:
			 *                       +-------+
			 *                       fbno fend
			 */
			fbno = bend;
		} else if (bend >= fend) {
			/* end overlap */

			/*
			 * Case 7:
			 *             bbno           bend
			 *             +BBBBBBBBBBBBBBBBB+
			 *    +------------------+
			 *    fbno            fend
			 *
			 * Case 8:
			 *             bbno           bend
			 *             +BBBBBBBBBBBBBBBBB+
			 *    +--------------------------+
			 *    fbno                    fend
			 *
			 * Needs to be trimmed to:
			 *    +-------+
			 *    fbno fend
			 */
			fend = bbno;
		} else {
			/* middle overlap */

			/*
			 * Case 9:
			 *             bbno           bend
			 *             +BBBBBBBBBBBBBBBBB+
			 *    +-----------------------------------+
			 *    fbno                             fend
			 *
			 * Can be trimmed to:
			 *    +-------+        OR         +-------+
			 *    fbno fend                   fbno fend
			 *
			 * Backward allocation leads to significant
			 * fragmentation of directories, which degrades
			 * directory performance, therefore we always want to
			 * choose the option that produces forward allocation
			 * patterns.
			 * Preferring the lower bno extent will make the next
			 * request use "fend" as the start of the next
			 * allocation;  if the segment is no longer busy at
			 * that point, we'll get a contiguous allocation, but
			 * even if it is still busy, we will get a forward
			 * allocation.
			 * We try to avoid choosing the segment at "bend",
			 * because that can lead to the next allocation
			 * taking the segment at "fbno", which would be a
			 * backward allocation.  We only use the segment at
			 * "fbno" if it is much larger than the current
			 * requested size, because in that case there's a
			 * good chance subsequent allocations will be
			 * contiguous.
			 */
			if (bbno - fbno >= args->maxlen) {
				/* left candidate fits perfect */
				fend = bbno;
			} else if (fend - bend >= args->maxlen * 4) {
				/* right candidate has enough free space */
				fbno = bend;
			} else if (bbno - fbno >= args->minlen) {
				/* left candidate fits minimum requirement */
				fend = bbno;
			} else {
				goto fail;
			}
		}

		flen = fend - fbno;
	}
out:

	if (fbno != *bno || flen != *len) {
		trace_xfs_extent_busy_trim(args->mp, args->agno, *bno, *len,
					  fbno, flen);
		*bno = fbno;
		*len = flen;
		*busy_gen = args->pag->pagb_gen;
		ret = true;
	}
	spin_unlock(&args->pag->pagb_lock);
	return ret;
fail:
	/*
	 * Return a zero extent length as failure indications.  All callers
	 * re-check if the trimmed extent satisfies the minlen requirement.
	 */
	flen = 0;
	goto out;
}

STATIC void
xfs_extent_busy_clear_one(
	struct xfs_mount	*mp,
	struct xfs_perag	*pag,
	struct xfs_extent_busy	*busyp)
{
	if (busyp->length) {
		trace_xfs_extent_busy_clear(mp, busyp->agno, busyp->bno,
						busyp->length);
		rb_erase(&busyp->rb_node, &pag->pagb_tree);
	}

	list_del_init(&busyp->list);
	kmem_free(busyp);
}

static void
xfs_extent_busy_put_pag(
	struct xfs_perag	*pag,
	bool			wakeup)
		__releases(pag->pagb_lock)
{
	if (wakeup) {
		pag->pagb_gen++;
		wake_up_all(&pag->pagb_wait);
	}

	spin_unlock(&pag->pagb_lock);
	xfs_perag_put(pag);
}

/*
 * Remove all extents on the passed in list from the busy extents tree.
 * If do_discard is set skip extents that need to be discarded, and mark
 * these as undergoing a discard operation instead.
 */
void
xfs_extent_busy_clear(
	struct xfs_mount	*mp,
	struct list_head	*list,
	bool			do_discard)
{
	struct xfs_extent_busy	*busyp, *n;
	struct xfs_perag	*pag = NULL;
	xfs_agnumber_t		agno = NULLAGNUMBER;
	bool			wakeup = false;

	list_for_each_entry_safe(busyp, n, list, list) {
		if (busyp->agno != agno) {
			if (pag)
				xfs_extent_busy_put_pag(pag, wakeup);
			agno = busyp->agno;
			pag = xfs_perag_get(mp, agno);
			spin_lock(&pag->pagb_lock);
			wakeup = false;
		}

		if (do_discard && busyp->length &&
		    !(busyp->flags & XFS_EXTENT_BUSY_SKIP_DISCARD)) {
			busyp->flags = XFS_EXTENT_BUSY_DISCARDED;
		} else {
			xfs_extent_busy_clear_one(mp, pag, busyp);
			wakeup = true;
		}
	}

	if (pag)
		xfs_extent_busy_put_pag(pag, wakeup);
}

/*
 * Flush out all busy extents for this AG.
 */
void
xfs_extent_busy_flush(
	struct xfs_mount	*mp,
	struct xfs_perag	*pag,
	unsigned		busy_gen)
{
	DEFINE_WAIT		(wait);
	int			error;

	error = xfs_log_force(mp, XFS_LOG_SYNC);
	if (error)
		return;

	do {
		prepare_to_wait(&pag->pagb_wait, &wait, TASK_KILLABLE);
		if  (busy_gen != READ_ONCE(pag->pagb_gen))
			break;
		schedule();
	} while (1);

	finish_wait(&pag->pagb_wait, &wait);
}

void
xfs_extent_busy_wait_all(
	struct xfs_mount	*mp)
{
	DEFINE_WAIT		(wait);
	xfs_agnumber_t		agno;

	for (agno = 0; agno < mp->m_sb.sb_agcount; agno++) {
		struct xfs_perag *pag = xfs_perag_get(mp, agno);

		do {
			prepare_to_wait(&pag->pagb_wait, &wait, TASK_KILLABLE);
			if  (RB_EMPTY_ROOT(&pag->pagb_tree))
				break;
			schedule();
		} while (1);
		finish_wait(&pag->pagb_wait, &wait);

		xfs_perag_put(pag);
	}
}

/*
 * Callback for list_sort to sort busy extents by the AG they reside in.
 */
int
xfs_extent_busy_ag_cmp(
	void			*priv,
	const struct list_head	*l1,
	const struct list_head	*l2)
{
	struct xfs_extent_busy	*b1 =
		container_of(l1, struct xfs_extent_busy, list);
	struct xfs_extent_busy	*b2 =
		container_of(l2, struct xfs_extent_busy, list);
	s32 diff;

	diff = b1->agno - b2->agno;
	if (!diff)
		diff = b1->bno - b2->bno;
	return diff;
}
