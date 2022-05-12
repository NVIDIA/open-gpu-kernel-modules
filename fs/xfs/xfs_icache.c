// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2000-2005 Silicon Graphics, Inc.
 * All Rights Reserved.
 */
#include "xfs.h"
#include "xfs_fs.h"
#include "xfs_shared.h"
#include "xfs_format.h"
#include "xfs_log_format.h"
#include "xfs_trans_resv.h"
#include "xfs_sb.h"
#include "xfs_mount.h"
#include "xfs_inode.h"
#include "xfs_trans.h"
#include "xfs_trans_priv.h"
#include "xfs_inode_item.h"
#include "xfs_quota.h"
#include "xfs_trace.h"
#include "xfs_icache.h"
#include "xfs_bmap_util.h"
#include "xfs_dquot_item.h"
#include "xfs_dquot.h"
#include "xfs_reflink.h"
#include "xfs_ialloc.h"

#include <linux/iversion.h>

/*
 * Allocate and initialise an xfs_inode.
 */
struct xfs_inode *
xfs_inode_alloc(
	struct xfs_mount	*mp,
	xfs_ino_t		ino)
{
	struct xfs_inode	*ip;

	/*
	 * XXX: If this didn't occur in transactions, we could drop GFP_NOFAIL
	 * and return NULL here on ENOMEM.
	 */
	ip = kmem_cache_alloc(xfs_inode_zone, GFP_KERNEL | __GFP_NOFAIL);

	if (inode_init_always(mp->m_super, VFS_I(ip))) {
		kmem_cache_free(xfs_inode_zone, ip);
		return NULL;
	}

	/* VFS doesn't initialise i_mode! */
	VFS_I(ip)->i_mode = 0;

	XFS_STATS_INC(mp, vn_active);
	ASSERT(atomic_read(&ip->i_pincount) == 0);
	ASSERT(ip->i_ino == 0);

	/* initialise the xfs inode */
	ip->i_ino = ino;
	ip->i_mount = mp;
	memset(&ip->i_imap, 0, sizeof(struct xfs_imap));
	ip->i_afp = NULL;
	ip->i_cowfp = NULL;
	memset(&ip->i_df, 0, sizeof(ip->i_df));
	ip->i_flags = 0;
	ip->i_delayed_blks = 0;
	ip->i_diflags2 = mp->m_ino_geo.new_diflags2;
	ip->i_nblocks = 0;
	ip->i_forkoff = 0;
	ip->i_sick = 0;
	ip->i_checked = 0;
	INIT_WORK(&ip->i_ioend_work, xfs_end_io);
	INIT_LIST_HEAD(&ip->i_ioend_list);
	spin_lock_init(&ip->i_ioend_lock);

	return ip;
}

STATIC void
xfs_inode_free_callback(
	struct rcu_head		*head)
{
	struct inode		*inode = container_of(head, struct inode, i_rcu);
	struct xfs_inode	*ip = XFS_I(inode);

	switch (VFS_I(ip)->i_mode & S_IFMT) {
	case S_IFREG:
	case S_IFDIR:
	case S_IFLNK:
		xfs_idestroy_fork(&ip->i_df);
		break;
	}

	if (ip->i_afp) {
		xfs_idestroy_fork(ip->i_afp);
		kmem_cache_free(xfs_ifork_zone, ip->i_afp);
	}
	if (ip->i_cowfp) {
		xfs_idestroy_fork(ip->i_cowfp);
		kmem_cache_free(xfs_ifork_zone, ip->i_cowfp);
	}
	if (ip->i_itemp) {
		ASSERT(!test_bit(XFS_LI_IN_AIL,
				 &ip->i_itemp->ili_item.li_flags));
		xfs_inode_item_destroy(ip);
		ip->i_itemp = NULL;
	}

	kmem_cache_free(xfs_inode_zone, ip);
}

static void
__xfs_inode_free(
	struct xfs_inode	*ip)
{
	/* asserts to verify all state is correct here */
	ASSERT(atomic_read(&ip->i_pincount) == 0);
	ASSERT(!ip->i_itemp || list_empty(&ip->i_itemp->ili_item.li_bio_list));
	XFS_STATS_DEC(ip->i_mount, vn_active);

	call_rcu(&VFS_I(ip)->i_rcu, xfs_inode_free_callback);
}

void
xfs_inode_free(
	struct xfs_inode	*ip)
{
	ASSERT(!xfs_iflags_test(ip, XFS_IFLUSHING));

	/*
	 * Because we use RCU freeing we need to ensure the inode always
	 * appears to be reclaimed with an invalid inode number when in the
	 * free state. The ip->i_flags_lock provides the barrier against lookup
	 * races.
	 */
	spin_lock(&ip->i_flags_lock);
	ip->i_flags = XFS_IRECLAIM;
	ip->i_ino = 0;
	spin_unlock(&ip->i_flags_lock);

	__xfs_inode_free(ip);
}

/*
 * Queue background inode reclaim work if there are reclaimable inodes and there
 * isn't reclaim work already scheduled or in progress.
 */
static void
xfs_reclaim_work_queue(
	struct xfs_mount        *mp)
{

	rcu_read_lock();
	if (radix_tree_tagged(&mp->m_perag_tree, XFS_ICI_RECLAIM_TAG)) {
		queue_delayed_work(mp->m_reclaim_workqueue, &mp->m_reclaim_work,
			msecs_to_jiffies(xfs_syncd_centisecs / 6 * 10));
	}
	rcu_read_unlock();
}

static void
xfs_perag_set_reclaim_tag(
	struct xfs_perag	*pag)
{
	struct xfs_mount	*mp = pag->pag_mount;

	lockdep_assert_held(&pag->pag_ici_lock);
	if (pag->pag_ici_reclaimable++)
		return;

	/* propagate the reclaim tag up into the perag radix tree */
	spin_lock(&mp->m_perag_lock);
	radix_tree_tag_set(&mp->m_perag_tree, pag->pag_agno,
			   XFS_ICI_RECLAIM_TAG);
	spin_unlock(&mp->m_perag_lock);

	/* schedule periodic background inode reclaim */
	xfs_reclaim_work_queue(mp);

	trace_xfs_perag_set_reclaim(mp, pag->pag_agno, -1, _RET_IP_);
}

static void
xfs_perag_clear_reclaim_tag(
	struct xfs_perag	*pag)
{
	struct xfs_mount	*mp = pag->pag_mount;

	lockdep_assert_held(&pag->pag_ici_lock);
	if (--pag->pag_ici_reclaimable)
		return;

	/* clear the reclaim tag from the perag radix tree */
	spin_lock(&mp->m_perag_lock);
	radix_tree_tag_clear(&mp->m_perag_tree, pag->pag_agno,
			     XFS_ICI_RECLAIM_TAG);
	spin_unlock(&mp->m_perag_lock);
	trace_xfs_perag_clear_reclaim(mp, pag->pag_agno, -1, _RET_IP_);
}


/*
 * We set the inode flag atomically with the radix tree tag.
 * Once we get tag lookups on the radix tree, this inode flag
 * can go away.
 */
void
xfs_inode_set_reclaim_tag(
	struct xfs_inode	*ip)
{
	struct xfs_mount	*mp = ip->i_mount;
	struct xfs_perag	*pag;

	pag = xfs_perag_get(mp, XFS_INO_TO_AGNO(mp, ip->i_ino));
	spin_lock(&pag->pag_ici_lock);
	spin_lock(&ip->i_flags_lock);

	radix_tree_tag_set(&pag->pag_ici_root, XFS_INO_TO_AGINO(mp, ip->i_ino),
			   XFS_ICI_RECLAIM_TAG);
	xfs_perag_set_reclaim_tag(pag);
	__xfs_iflags_set(ip, XFS_IRECLAIMABLE);

	spin_unlock(&ip->i_flags_lock);
	spin_unlock(&pag->pag_ici_lock);
	xfs_perag_put(pag);
}

STATIC void
xfs_inode_clear_reclaim_tag(
	struct xfs_perag	*pag,
	xfs_ino_t		ino)
{
	radix_tree_tag_clear(&pag->pag_ici_root,
			     XFS_INO_TO_AGINO(pag->pag_mount, ino),
			     XFS_ICI_RECLAIM_TAG);
	xfs_perag_clear_reclaim_tag(pag);
}

static void
xfs_inew_wait(
	struct xfs_inode	*ip)
{
	wait_queue_head_t *wq = bit_waitqueue(&ip->i_flags, __XFS_INEW_BIT);
	DEFINE_WAIT_BIT(wait, &ip->i_flags, __XFS_INEW_BIT);

	do {
		prepare_to_wait(wq, &wait.wq_entry, TASK_UNINTERRUPTIBLE);
		if (!xfs_iflags_test(ip, XFS_INEW))
			break;
		schedule();
	} while (true);
	finish_wait(wq, &wait.wq_entry);
}

/*
 * When we recycle a reclaimable inode, we need to re-initialise the VFS inode
 * part of the structure. This is made more complex by the fact we store
 * information about the on-disk values in the VFS inode and so we can't just
 * overwrite the values unconditionally. Hence we save the parameters we
 * need to retain across reinitialisation, and rewrite them into the VFS inode
 * after reinitialisation even if it fails.
 */
static int
xfs_reinit_inode(
	struct xfs_mount	*mp,
	struct inode		*inode)
{
	int		error;
	uint32_t	nlink = inode->i_nlink;
	uint32_t	generation = inode->i_generation;
	uint64_t	version = inode_peek_iversion(inode);
	umode_t		mode = inode->i_mode;
	dev_t		dev = inode->i_rdev;
	kuid_t		uid = inode->i_uid;
	kgid_t		gid = inode->i_gid;

	error = inode_init_always(mp->m_super, inode);

	set_nlink(inode, nlink);
	inode->i_generation = generation;
	inode_set_iversion_queried(inode, version);
	inode->i_mode = mode;
	inode->i_rdev = dev;
	inode->i_uid = uid;
	inode->i_gid = gid;
	return error;
}

/*
 * If we are allocating a new inode, then check what was returned is
 * actually a free, empty inode. If we are not allocating an inode,
 * then check we didn't find a free inode.
 *
 * Returns:
 *	0		if the inode free state matches the lookup context
 *	-ENOENT		if the inode is free and we are not allocating
 *	-EFSCORRUPTED	if there is any state mismatch at all
 */
static int
xfs_iget_check_free_state(
	struct xfs_inode	*ip,
	int			flags)
{
	if (flags & XFS_IGET_CREATE) {
		/* should be a free inode */
		if (VFS_I(ip)->i_mode != 0) {
			xfs_warn(ip->i_mount,
"Corruption detected! Free inode 0x%llx not marked free! (mode 0x%x)",
				ip->i_ino, VFS_I(ip)->i_mode);
			return -EFSCORRUPTED;
		}

		if (ip->i_nblocks != 0) {
			xfs_warn(ip->i_mount,
"Corruption detected! Free inode 0x%llx has blocks allocated!",
				ip->i_ino);
			return -EFSCORRUPTED;
		}
		return 0;
	}

	/* should be an allocated inode */
	if (VFS_I(ip)->i_mode == 0)
		return -ENOENT;

	return 0;
}

/*
 * Check the validity of the inode we just found it the cache
 */
static int
xfs_iget_cache_hit(
	struct xfs_perag	*pag,
	struct xfs_inode	*ip,
	xfs_ino_t		ino,
	int			flags,
	int			lock_flags) __releases(RCU)
{
	struct inode		*inode = VFS_I(ip);
	struct xfs_mount	*mp = ip->i_mount;
	int			error;

	/*
	 * check for re-use of an inode within an RCU grace period due to the
	 * radix tree nodes not being updated yet. We monitor for this by
	 * setting the inode number to zero before freeing the inode structure.
	 * If the inode has been reallocated and set up, then the inode number
	 * will not match, so check for that, too.
	 */
	spin_lock(&ip->i_flags_lock);
	if (ip->i_ino != ino) {
		trace_xfs_iget_skip(ip);
		XFS_STATS_INC(mp, xs_ig_frecycle);
		error = -EAGAIN;
		goto out_error;
	}


	/*
	 * If we are racing with another cache hit that is currently
	 * instantiating this inode or currently recycling it out of
	 * reclaimabe state, wait for the initialisation to complete
	 * before continuing.
	 *
	 * XXX(hch): eventually we should do something equivalent to
	 *	     wait_on_inode to wait for these flags to be cleared
	 *	     instead of polling for it.
	 */
	if (ip->i_flags & (XFS_INEW|XFS_IRECLAIM)) {
		trace_xfs_iget_skip(ip);
		XFS_STATS_INC(mp, xs_ig_frecycle);
		error = -EAGAIN;
		goto out_error;
	}

	/*
	 * Check the inode free state is valid. This also detects lookup
	 * racing with unlinks.
	 */
	error = xfs_iget_check_free_state(ip, flags);
	if (error)
		goto out_error;

	/*
	 * If IRECLAIMABLE is set, we've torn down the VFS inode already.
	 * Need to carefully get it back into useable state.
	 */
	if (ip->i_flags & XFS_IRECLAIMABLE) {
		trace_xfs_iget_reclaim(ip);

		if (flags & XFS_IGET_INCORE) {
			error = -EAGAIN;
			goto out_error;
		}

		/*
		 * We need to set XFS_IRECLAIM to prevent xfs_reclaim_inode
		 * from stomping over us while we recycle the inode.  We can't
		 * clear the radix tree reclaimable tag yet as it requires
		 * pag_ici_lock to be held exclusive.
		 */
		ip->i_flags |= XFS_IRECLAIM;

		spin_unlock(&ip->i_flags_lock);
		rcu_read_unlock();

		ASSERT(!rwsem_is_locked(&inode->i_rwsem));
		error = xfs_reinit_inode(mp, inode);
		if (error) {
			bool wake;
			/*
			 * Re-initializing the inode failed, and we are in deep
			 * trouble.  Try to re-add it to the reclaim list.
			 */
			rcu_read_lock();
			spin_lock(&ip->i_flags_lock);
			wake = !!__xfs_iflags_test(ip, XFS_INEW);
			ip->i_flags &= ~(XFS_INEW | XFS_IRECLAIM);
			if (wake)
				wake_up_bit(&ip->i_flags, __XFS_INEW_BIT);
			ASSERT(ip->i_flags & XFS_IRECLAIMABLE);
			trace_xfs_iget_reclaim_fail(ip);
			goto out_error;
		}

		spin_lock(&pag->pag_ici_lock);
		spin_lock(&ip->i_flags_lock);

		/*
		 * Clear the per-lifetime state in the inode as we are now
		 * effectively a new inode and need to return to the initial
		 * state before reuse occurs.
		 */
		ip->i_flags &= ~XFS_IRECLAIM_RESET_FLAGS;
		ip->i_flags |= XFS_INEW;
		xfs_inode_clear_reclaim_tag(pag, ip->i_ino);
		inode->i_state = I_NEW;
		ip->i_sick = 0;
		ip->i_checked = 0;

		spin_unlock(&ip->i_flags_lock);
		spin_unlock(&pag->pag_ici_lock);
	} else {
		/* If the VFS inode is being torn down, pause and try again. */
		if (!igrab(inode)) {
			trace_xfs_iget_skip(ip);
			error = -EAGAIN;
			goto out_error;
		}

		/* We've got a live one. */
		spin_unlock(&ip->i_flags_lock);
		rcu_read_unlock();
		trace_xfs_iget_hit(ip);
	}

	if (lock_flags != 0)
		xfs_ilock(ip, lock_flags);

	if (!(flags & XFS_IGET_INCORE))
		xfs_iflags_clear(ip, XFS_ISTALE);
	XFS_STATS_INC(mp, xs_ig_found);

	return 0;

out_error:
	spin_unlock(&ip->i_flags_lock);
	rcu_read_unlock();
	return error;
}


static int
xfs_iget_cache_miss(
	struct xfs_mount	*mp,
	struct xfs_perag	*pag,
	xfs_trans_t		*tp,
	xfs_ino_t		ino,
	struct xfs_inode	**ipp,
	int			flags,
	int			lock_flags)
{
	struct xfs_inode	*ip;
	int			error;
	xfs_agino_t		agino = XFS_INO_TO_AGINO(mp, ino);
	int			iflags;

	ip = xfs_inode_alloc(mp, ino);
	if (!ip)
		return -ENOMEM;

	error = xfs_imap(mp, tp, ip->i_ino, &ip->i_imap, flags);
	if (error)
		goto out_destroy;

	/*
	 * For version 5 superblocks, if we are initialising a new inode and we
	 * are not utilising the XFS_MOUNT_IKEEP inode cluster mode, we can
	 * simply build the new inode core with a random generation number.
	 *
	 * For version 4 (and older) superblocks, log recovery is dependent on
	 * the i_flushiter field being initialised from the current on-disk
	 * value and hence we must also read the inode off disk even when
	 * initializing new inodes.
	 */
	if (xfs_sb_version_has_v3inode(&mp->m_sb) &&
	    (flags & XFS_IGET_CREATE) && !(mp->m_flags & XFS_MOUNT_IKEEP)) {
		VFS_I(ip)->i_generation = prandom_u32();
	} else {
		struct xfs_buf		*bp;

		error = xfs_imap_to_bp(mp, tp, &ip->i_imap, &bp);
		if (error)
			goto out_destroy;

		error = xfs_inode_from_disk(ip,
				xfs_buf_offset(bp, ip->i_imap.im_boffset));
		if (!error)
			xfs_buf_set_ref(bp, XFS_INO_REF);
		xfs_trans_brelse(tp, bp);

		if (error)
			goto out_destroy;
	}

	trace_xfs_iget_miss(ip);

	/*
	 * Check the inode free state is valid. This also detects lookup
	 * racing with unlinks.
	 */
	error = xfs_iget_check_free_state(ip, flags);
	if (error)
		goto out_destroy;

	/*
	 * Preload the radix tree so we can insert safely under the
	 * write spinlock. Note that we cannot sleep inside the preload
	 * region. Since we can be called from transaction context, don't
	 * recurse into the file system.
	 */
	if (radix_tree_preload(GFP_NOFS)) {
		error = -EAGAIN;
		goto out_destroy;
	}

	/*
	 * Because the inode hasn't been added to the radix-tree yet it can't
	 * be found by another thread, so we can do the non-sleeping lock here.
	 */
	if (lock_flags) {
		if (!xfs_ilock_nowait(ip, lock_flags))
			BUG();
	}

	/*
	 * These values must be set before inserting the inode into the radix
	 * tree as the moment it is inserted a concurrent lookup (allowed by the
	 * RCU locking mechanism) can find it and that lookup must see that this
	 * is an inode currently under construction (i.e. that XFS_INEW is set).
	 * The ip->i_flags_lock that protects the XFS_INEW flag forms the
	 * memory barrier that ensures this detection works correctly at lookup
	 * time.
	 */
	iflags = XFS_INEW;
	if (flags & XFS_IGET_DONTCACHE)
		d_mark_dontcache(VFS_I(ip));
	ip->i_udquot = NULL;
	ip->i_gdquot = NULL;
	ip->i_pdquot = NULL;
	xfs_iflags_set(ip, iflags);

	/* insert the new inode */
	spin_lock(&pag->pag_ici_lock);
	error = radix_tree_insert(&pag->pag_ici_root, agino, ip);
	if (unlikely(error)) {
		WARN_ON(error != -EEXIST);
		XFS_STATS_INC(mp, xs_ig_dup);
		error = -EAGAIN;
		goto out_preload_end;
	}
	spin_unlock(&pag->pag_ici_lock);
	radix_tree_preload_end();

	*ipp = ip;
	return 0;

out_preload_end:
	spin_unlock(&pag->pag_ici_lock);
	radix_tree_preload_end();
	if (lock_flags)
		xfs_iunlock(ip, lock_flags);
out_destroy:
	__destroy_inode(VFS_I(ip));
	xfs_inode_free(ip);
	return error;
}

/*
 * Look up an inode by number in the given file system.  The inode is looked up
 * in the cache held in each AG.  If the inode is found in the cache, initialise
 * the vfs inode if necessary.
 *
 * If it is not in core, read it in from the file system's device, add it to the
 * cache and initialise the vfs inode.
 *
 * The inode is locked according to the value of the lock_flags parameter.
 * Inode lookup is only done during metadata operations and not as part of the
 * data IO path. Hence we only allow locking of the XFS_ILOCK during lookup.
 */
int
xfs_iget(
	struct xfs_mount	*mp,
	struct xfs_trans	*tp,
	xfs_ino_t		ino,
	uint			flags,
	uint			lock_flags,
	struct xfs_inode	**ipp)
{
	struct xfs_inode	*ip;
	struct xfs_perag	*pag;
	xfs_agino_t		agino;
	int			error;

	ASSERT((lock_flags & (XFS_IOLOCK_EXCL | XFS_IOLOCK_SHARED)) == 0);

	/* reject inode numbers outside existing AGs */
	if (!ino || XFS_INO_TO_AGNO(mp, ino) >= mp->m_sb.sb_agcount)
		return -EINVAL;

	XFS_STATS_INC(mp, xs_ig_attempts);

	/* get the perag structure and ensure that it's inode capable */
	pag = xfs_perag_get(mp, XFS_INO_TO_AGNO(mp, ino));
	agino = XFS_INO_TO_AGINO(mp, ino);

again:
	error = 0;
	rcu_read_lock();
	ip = radix_tree_lookup(&pag->pag_ici_root, agino);

	if (ip) {
		error = xfs_iget_cache_hit(pag, ip, ino, flags, lock_flags);
		if (error)
			goto out_error_or_again;
	} else {
		rcu_read_unlock();
		if (flags & XFS_IGET_INCORE) {
			error = -ENODATA;
			goto out_error_or_again;
		}
		XFS_STATS_INC(mp, xs_ig_missed);

		error = xfs_iget_cache_miss(mp, pag, tp, ino, &ip,
							flags, lock_flags);
		if (error)
			goto out_error_or_again;
	}
	xfs_perag_put(pag);

	*ipp = ip;

	/*
	 * If we have a real type for an on-disk inode, we can setup the inode
	 * now.	 If it's a new inode being created, xfs_ialloc will handle it.
	 */
	if (xfs_iflags_test(ip, XFS_INEW) && VFS_I(ip)->i_mode != 0)
		xfs_setup_existing_inode(ip);
	return 0;

out_error_or_again:
	if (!(flags & XFS_IGET_INCORE) && error == -EAGAIN) {
		delay(1);
		goto again;
	}
	xfs_perag_put(pag);
	return error;
}

/*
 * "Is this a cached inode that's also allocated?"
 *
 * Look up an inode by number in the given file system.  If the inode is
 * in cache and isn't in purgatory, return 1 if the inode is allocated
 * and 0 if it is not.  For all other cases (not in cache, being torn
 * down, etc.), return a negative error code.
 *
 * The caller has to prevent inode allocation and freeing activity,
 * presumably by locking the AGI buffer.   This is to ensure that an
 * inode cannot transition from allocated to freed until the caller is
 * ready to allow that.  If the inode is in an intermediate state (new,
 * reclaimable, or being reclaimed), -EAGAIN will be returned; if the
 * inode is not in the cache, -ENOENT will be returned.  The caller must
 * deal with these scenarios appropriately.
 *
 * This is a specialized use case for the online scrubber; if you're
 * reading this, you probably want xfs_iget.
 */
int
xfs_icache_inode_is_allocated(
	struct xfs_mount	*mp,
	struct xfs_trans	*tp,
	xfs_ino_t		ino,
	bool			*inuse)
{
	struct xfs_inode	*ip;
	int			error;

	error = xfs_iget(mp, tp, ino, XFS_IGET_INCORE, 0, &ip);
	if (error)
		return error;

	*inuse = !!(VFS_I(ip)->i_mode);
	xfs_irele(ip);
	return 0;
}

/*
 * The inode lookup is done in batches to keep the amount of lock traffic and
 * radix tree lookups to a minimum. The batch size is a trade off between
 * lookup reduction and stack usage. This is in the reclaim path, so we can't
 * be too greedy.
 */
#define XFS_LOOKUP_BATCH	32

/*
 * Decide if the given @ip is eligible to be a part of the inode walk, and
 * grab it if so.  Returns true if it's ready to go or false if we should just
 * ignore it.
 */
STATIC bool
xfs_inode_walk_ag_grab(
	struct xfs_inode	*ip,
	int			flags)
{
	struct inode		*inode = VFS_I(ip);
	bool			newinos = !!(flags & XFS_INODE_WALK_INEW_WAIT);

	ASSERT(rcu_read_lock_held());

	/* Check for stale RCU freed inode */
	spin_lock(&ip->i_flags_lock);
	if (!ip->i_ino)
		goto out_unlock_noent;

	/* avoid new or reclaimable inodes. Leave for reclaim code to flush */
	if ((!newinos && __xfs_iflags_test(ip, XFS_INEW)) ||
	    __xfs_iflags_test(ip, XFS_IRECLAIMABLE | XFS_IRECLAIM))
		goto out_unlock_noent;
	spin_unlock(&ip->i_flags_lock);

	/* nothing to sync during shutdown */
	if (XFS_FORCED_SHUTDOWN(ip->i_mount))
		return false;

	/* If we can't grab the inode, it must on it's way to reclaim. */
	if (!igrab(inode))
		return false;

	/* inode is valid */
	return true;

out_unlock_noent:
	spin_unlock(&ip->i_flags_lock);
	return false;
}

/*
 * For a given per-AG structure @pag, grab, @execute, and rele all incore
 * inodes with the given radix tree @tag.
 */
STATIC int
xfs_inode_walk_ag(
	struct xfs_perag	*pag,
	int			iter_flags,
	int			(*execute)(struct xfs_inode *ip, void *args),
	void			*args,
	int			tag)
{
	struct xfs_mount	*mp = pag->pag_mount;
	uint32_t		first_index;
	int			last_error = 0;
	int			skipped;
	bool			done;
	int			nr_found;

restart:
	done = false;
	skipped = 0;
	first_index = 0;
	nr_found = 0;
	do {
		struct xfs_inode *batch[XFS_LOOKUP_BATCH];
		int		error = 0;
		int		i;

		rcu_read_lock();

		if (tag == XFS_ICI_NO_TAG)
			nr_found = radix_tree_gang_lookup(&pag->pag_ici_root,
					(void **)batch, first_index,
					XFS_LOOKUP_BATCH);
		else
			nr_found = radix_tree_gang_lookup_tag(
					&pag->pag_ici_root,
					(void **) batch, first_index,
					XFS_LOOKUP_BATCH, tag);

		if (!nr_found) {
			rcu_read_unlock();
			break;
		}

		/*
		 * Grab the inodes before we drop the lock. if we found
		 * nothing, nr == 0 and the loop will be skipped.
		 */
		for (i = 0; i < nr_found; i++) {
			struct xfs_inode *ip = batch[i];

			if (done || !xfs_inode_walk_ag_grab(ip, iter_flags))
				batch[i] = NULL;

			/*
			 * Update the index for the next lookup. Catch
			 * overflows into the next AG range which can occur if
			 * we have inodes in the last block of the AG and we
			 * are currently pointing to the last inode.
			 *
			 * Because we may see inodes that are from the wrong AG
			 * due to RCU freeing and reallocation, only update the
			 * index if it lies in this AG. It was a race that lead
			 * us to see this inode, so another lookup from the
			 * same index will not find it again.
			 */
			if (XFS_INO_TO_AGNO(mp, ip->i_ino) != pag->pag_agno)
				continue;
			first_index = XFS_INO_TO_AGINO(mp, ip->i_ino + 1);
			if (first_index < XFS_INO_TO_AGINO(mp, ip->i_ino))
				done = true;
		}

		/* unlock now we've grabbed the inodes. */
		rcu_read_unlock();

		for (i = 0; i < nr_found; i++) {
			if (!batch[i])
				continue;
			if ((iter_flags & XFS_INODE_WALK_INEW_WAIT) &&
			    xfs_iflags_test(batch[i], XFS_INEW))
				xfs_inew_wait(batch[i]);
			error = execute(batch[i], args);
			xfs_irele(batch[i]);
			if (error == -EAGAIN) {
				skipped++;
				continue;
			}
			if (error && last_error != -EFSCORRUPTED)
				last_error = error;
		}

		/* bail out if the filesystem is corrupted.  */
		if (error == -EFSCORRUPTED)
			break;

		cond_resched();

	} while (nr_found && !done);

	if (skipped) {
		delay(1);
		goto restart;
	}
	return last_error;
}

/* Fetch the next (possibly tagged) per-AG structure. */
static inline struct xfs_perag *
xfs_inode_walk_get_perag(
	struct xfs_mount	*mp,
	xfs_agnumber_t		agno,
	int			tag)
{
	if (tag == XFS_ICI_NO_TAG)
		return xfs_perag_get(mp, agno);
	return xfs_perag_get_tag(mp, agno, tag);
}

/*
 * Call the @execute function on all incore inodes matching the radix tree
 * @tag.
 */
int
xfs_inode_walk(
	struct xfs_mount	*mp,
	int			iter_flags,
	int			(*execute)(struct xfs_inode *ip, void *args),
	void			*args,
	int			tag)
{
	struct xfs_perag	*pag;
	int			error = 0;
	int			last_error = 0;
	xfs_agnumber_t		ag;

	ag = 0;
	while ((pag = xfs_inode_walk_get_perag(mp, ag, tag))) {
		ag = pag->pag_agno + 1;
		error = xfs_inode_walk_ag(pag, iter_flags, execute, args, tag);
		xfs_perag_put(pag);
		if (error) {
			last_error = error;
			if (error == -EFSCORRUPTED)
				break;
		}
	}
	return last_error;
}

/*
 * Grab the inode for reclaim exclusively.
 *
 * We have found this inode via a lookup under RCU, so the inode may have
 * already been freed, or it may be in the process of being recycled by
 * xfs_iget(). In both cases, the inode will have XFS_IRECLAIM set. If the inode
 * has been fully recycled by the time we get the i_flags_lock, XFS_IRECLAIMABLE
 * will not be set. Hence we need to check for both these flag conditions to
 * avoid inodes that are no longer reclaim candidates.
 *
 * Note: checking for other state flags here, under the i_flags_lock or not, is
 * racy and should be avoided. Those races should be resolved only after we have
 * ensured that we are able to reclaim this inode and the world can see that we
 * are going to reclaim it.
 *
 * Return true if we grabbed it, false otherwise.
 */
static bool
xfs_reclaim_inode_grab(
	struct xfs_inode	*ip)
{
	ASSERT(rcu_read_lock_held());

	spin_lock(&ip->i_flags_lock);
	if (!__xfs_iflags_test(ip, XFS_IRECLAIMABLE) ||
	    __xfs_iflags_test(ip, XFS_IRECLAIM)) {
		/* not a reclaim candidate. */
		spin_unlock(&ip->i_flags_lock);
		return false;
	}
	__xfs_iflags_set(ip, XFS_IRECLAIM);
	spin_unlock(&ip->i_flags_lock);
	return true;
}

/*
 * Inode reclaim is non-blocking, so the default action if progress cannot be
 * made is to "requeue" the inode for reclaim by unlocking it and clearing the
 * XFS_IRECLAIM flag.  If we are in a shutdown state, we don't care about
 * blocking anymore and hence we can wait for the inode to be able to reclaim
 * it.
 *
 * We do no IO here - if callers require inodes to be cleaned they must push the
 * AIL first to trigger writeback of dirty inodes.  This enables writeback to be
 * done in the background in a non-blocking manner, and enables memory reclaim
 * to make progress without blocking.
 */
static void
xfs_reclaim_inode(
	struct xfs_inode	*ip,
	struct xfs_perag	*pag)
{
	xfs_ino_t		ino = ip->i_ino; /* for radix_tree_delete */

	if (!xfs_ilock_nowait(ip, XFS_ILOCK_EXCL))
		goto out;
	if (xfs_iflags_test_and_set(ip, XFS_IFLUSHING))
		goto out_iunlock;

	if (XFS_FORCED_SHUTDOWN(ip->i_mount)) {
		xfs_iunpin_wait(ip);
		xfs_iflush_abort(ip);
		goto reclaim;
	}
	if (xfs_ipincount(ip))
		goto out_clear_flush;
	if (!xfs_inode_clean(ip))
		goto out_clear_flush;

	xfs_iflags_clear(ip, XFS_IFLUSHING);
reclaim:

	/*
	 * Because we use RCU freeing we need to ensure the inode always appears
	 * to be reclaimed with an invalid inode number when in the free state.
	 * We do this as early as possible under the ILOCK so that
	 * xfs_iflush_cluster() and xfs_ifree_cluster() can be guaranteed to
	 * detect races with us here. By doing this, we guarantee that once
	 * xfs_iflush_cluster() or xfs_ifree_cluster() has locked XFS_ILOCK that
	 * it will see either a valid inode that will serialise correctly, or it
	 * will see an invalid inode that it can skip.
	 */
	spin_lock(&ip->i_flags_lock);
	ip->i_flags = XFS_IRECLAIM;
	ip->i_ino = 0;
	spin_unlock(&ip->i_flags_lock);

	xfs_iunlock(ip, XFS_ILOCK_EXCL);

	XFS_STATS_INC(ip->i_mount, xs_ig_reclaims);
	/*
	 * Remove the inode from the per-AG radix tree.
	 *
	 * Because radix_tree_delete won't complain even if the item was never
	 * added to the tree assert that it's been there before to catch
	 * problems with the inode life time early on.
	 */
	spin_lock(&pag->pag_ici_lock);
	if (!radix_tree_delete(&pag->pag_ici_root,
				XFS_INO_TO_AGINO(ip->i_mount, ino)))
		ASSERT(0);
	xfs_perag_clear_reclaim_tag(pag);
	spin_unlock(&pag->pag_ici_lock);

	/*
	 * Here we do an (almost) spurious inode lock in order to coordinate
	 * with inode cache radix tree lookups.  This is because the lookup
	 * can reference the inodes in the cache without taking references.
	 *
	 * We make that OK here by ensuring that we wait until the inode is
	 * unlocked after the lookup before we go ahead and free it.
	 */
	xfs_ilock(ip, XFS_ILOCK_EXCL);
	xfs_qm_dqdetach(ip);
	xfs_iunlock(ip, XFS_ILOCK_EXCL);
	ASSERT(xfs_inode_clean(ip));

	__xfs_inode_free(ip);
	return;

out_clear_flush:
	xfs_iflags_clear(ip, XFS_IFLUSHING);
out_iunlock:
	xfs_iunlock(ip, XFS_ILOCK_EXCL);
out:
	xfs_iflags_clear(ip, XFS_IRECLAIM);
}

/*
 * Walk the AGs and reclaim the inodes in them. Even if the filesystem is
 * corrupted, we still want to try to reclaim all the inodes. If we don't,
 * then a shut down during filesystem unmount reclaim walk leak all the
 * unreclaimed inodes.
 *
 * Returns non-zero if any AGs or inodes were skipped in the reclaim pass
 * so that callers that want to block until all dirty inodes are written back
 * and reclaimed can sanely loop.
 */
static void
xfs_reclaim_inodes_ag(
	struct xfs_mount	*mp,
	int			*nr_to_scan)
{
	struct xfs_perag	*pag;
	xfs_agnumber_t		ag = 0;

	while ((pag = xfs_perag_get_tag(mp, ag, XFS_ICI_RECLAIM_TAG))) {
		unsigned long	first_index = 0;
		int		done = 0;
		int		nr_found = 0;

		ag = pag->pag_agno + 1;

		first_index = READ_ONCE(pag->pag_ici_reclaim_cursor);
		do {
			struct xfs_inode *batch[XFS_LOOKUP_BATCH];
			int	i;

			rcu_read_lock();
			nr_found = radix_tree_gang_lookup_tag(
					&pag->pag_ici_root,
					(void **)batch, first_index,
					XFS_LOOKUP_BATCH,
					XFS_ICI_RECLAIM_TAG);
			if (!nr_found) {
				done = 1;
				rcu_read_unlock();
				break;
			}

			/*
			 * Grab the inodes before we drop the lock. if we found
			 * nothing, nr == 0 and the loop will be skipped.
			 */
			for (i = 0; i < nr_found; i++) {
				struct xfs_inode *ip = batch[i];

				if (done || !xfs_reclaim_inode_grab(ip))
					batch[i] = NULL;

				/*
				 * Update the index for the next lookup. Catch
				 * overflows into the next AG range which can
				 * occur if we have inodes in the last block of
				 * the AG and we are currently pointing to the
				 * last inode.
				 *
				 * Because we may see inodes that are from the
				 * wrong AG due to RCU freeing and
				 * reallocation, only update the index if it
				 * lies in this AG. It was a race that lead us
				 * to see this inode, so another lookup from
				 * the same index will not find it again.
				 */
				if (XFS_INO_TO_AGNO(mp, ip->i_ino) !=
								pag->pag_agno)
					continue;
				first_index = XFS_INO_TO_AGINO(mp, ip->i_ino + 1);
				if (first_index < XFS_INO_TO_AGINO(mp, ip->i_ino))
					done = 1;
			}

			/* unlock now we've grabbed the inodes. */
			rcu_read_unlock();

			for (i = 0; i < nr_found; i++) {
				if (batch[i])
					xfs_reclaim_inode(batch[i], pag);
			}

			*nr_to_scan -= XFS_LOOKUP_BATCH;
			cond_resched();
		} while (nr_found && !done && *nr_to_scan > 0);

		if (done)
			first_index = 0;
		WRITE_ONCE(pag->pag_ici_reclaim_cursor, first_index);
		xfs_perag_put(pag);
	}
}

void
xfs_reclaim_inodes(
	struct xfs_mount	*mp)
{
	int		nr_to_scan = INT_MAX;

	while (radix_tree_tagged(&mp->m_perag_tree, XFS_ICI_RECLAIM_TAG)) {
		xfs_ail_push_all_sync(mp->m_ail);
		xfs_reclaim_inodes_ag(mp, &nr_to_scan);
	}
}

/*
 * The shrinker infrastructure determines how many inodes we should scan for
 * reclaim. We want as many clean inodes ready to reclaim as possible, so we
 * push the AIL here. We also want to proactively free up memory if we can to
 * minimise the amount of work memory reclaim has to do so we kick the
 * background reclaim if it isn't already scheduled.
 */
long
xfs_reclaim_inodes_nr(
	struct xfs_mount	*mp,
	int			nr_to_scan)
{
	/* kick background reclaimer and push the AIL */
	xfs_reclaim_work_queue(mp);
	xfs_ail_push_all(mp->m_ail);

	xfs_reclaim_inodes_ag(mp, &nr_to_scan);
	return 0;
}

/*
 * Return the number of reclaimable inodes in the filesystem for
 * the shrinker to determine how much to reclaim.
 */
int
xfs_reclaim_inodes_count(
	struct xfs_mount	*mp)
{
	struct xfs_perag	*pag;
	xfs_agnumber_t		ag = 0;
	int			reclaimable = 0;

	while ((pag = xfs_perag_get_tag(mp, ag, XFS_ICI_RECLAIM_TAG))) {
		ag = pag->pag_agno + 1;
		reclaimable += pag->pag_ici_reclaimable;
		xfs_perag_put(pag);
	}
	return reclaimable;
}

STATIC bool
xfs_inode_match_id(
	struct xfs_inode	*ip,
	struct xfs_eofblocks	*eofb)
{
	if ((eofb->eof_flags & XFS_EOF_FLAGS_UID) &&
	    !uid_eq(VFS_I(ip)->i_uid, eofb->eof_uid))
		return false;

	if ((eofb->eof_flags & XFS_EOF_FLAGS_GID) &&
	    !gid_eq(VFS_I(ip)->i_gid, eofb->eof_gid))
		return false;

	if ((eofb->eof_flags & XFS_EOF_FLAGS_PRID) &&
	    ip->i_projid != eofb->eof_prid)
		return false;

	return true;
}

/*
 * A union-based inode filtering algorithm. Process the inode if any of the
 * criteria match. This is for global/internal scans only.
 */
STATIC bool
xfs_inode_match_id_union(
	struct xfs_inode	*ip,
	struct xfs_eofblocks	*eofb)
{
	if ((eofb->eof_flags & XFS_EOF_FLAGS_UID) &&
	    uid_eq(VFS_I(ip)->i_uid, eofb->eof_uid))
		return true;

	if ((eofb->eof_flags & XFS_EOF_FLAGS_GID) &&
	    gid_eq(VFS_I(ip)->i_gid, eofb->eof_gid))
		return true;

	if ((eofb->eof_flags & XFS_EOF_FLAGS_PRID) &&
	    ip->i_projid == eofb->eof_prid)
		return true;

	return false;
}

/*
 * Is this inode @ip eligible for eof/cow block reclamation, given some
 * filtering parameters @eofb?  The inode is eligible if @eofb is null or
 * if the predicate functions match.
 */
static bool
xfs_inode_matches_eofb(
	struct xfs_inode	*ip,
	struct xfs_eofblocks	*eofb)
{
	bool			match;

	if (!eofb)
		return true;

	if (eofb->eof_flags & XFS_EOF_FLAGS_UNION)
		match = xfs_inode_match_id_union(ip, eofb);
	else
		match = xfs_inode_match_id(ip, eofb);
	if (!match)
		return false;

	/* skip the inode if the file size is too small */
	if ((eofb->eof_flags & XFS_EOF_FLAGS_MINFILESIZE) &&
	    XFS_ISIZE(ip) < eofb->eof_min_file_size)
		return false;

	return true;
}

/*
 * This is a fast pass over the inode cache to try to get reclaim moving on as
 * many inodes as possible in a short period of time. It kicks itself every few
 * seconds, as well as being kicked by the inode cache shrinker when memory
 * goes low.
 */
void
xfs_reclaim_worker(
	struct work_struct *work)
{
	struct xfs_mount *mp = container_of(to_delayed_work(work),
					struct xfs_mount, m_reclaim_work);
	int		nr_to_scan = INT_MAX;

	xfs_reclaim_inodes_ag(mp, &nr_to_scan);
	xfs_reclaim_work_queue(mp);
}

STATIC int
xfs_inode_free_eofblocks(
	struct xfs_inode	*ip,
	void			*args,
	unsigned int		*lockflags)
{
	struct xfs_eofblocks	*eofb = args;
	bool			wait;

	wait = eofb && (eofb->eof_flags & XFS_EOF_FLAGS_SYNC);

	if (!xfs_iflags_test(ip, XFS_IEOFBLOCKS))
		return 0;

	/*
	 * If the mapping is dirty the operation can block and wait for some
	 * time. Unless we are waiting, skip it.
	 */
	if (!wait && mapping_tagged(VFS_I(ip)->i_mapping, PAGECACHE_TAG_DIRTY))
		return 0;

	if (!xfs_inode_matches_eofb(ip, eofb))
		return 0;

	/*
	 * If the caller is waiting, return -EAGAIN to keep the background
	 * scanner moving and revisit the inode in a subsequent pass.
	 */
	if (!xfs_ilock_nowait(ip, XFS_IOLOCK_EXCL)) {
		if (wait)
			return -EAGAIN;
		return 0;
	}
	*lockflags |= XFS_IOLOCK_EXCL;

	if (xfs_can_free_eofblocks(ip, false))
		return xfs_free_eofblocks(ip);

	/* inode could be preallocated or append-only */
	trace_xfs_inode_free_eofblocks_invalid(ip);
	xfs_inode_clear_eofblocks_tag(ip);
	return 0;
}

/*
 * Background scanning to trim preallocated space. This is queued based on the
 * 'speculative_prealloc_lifetime' tunable (5m by default).
 */
static inline void
xfs_blockgc_queue(
	struct xfs_perag	*pag)
{
	rcu_read_lock();
	if (radix_tree_tagged(&pag->pag_ici_root, XFS_ICI_BLOCKGC_TAG))
		queue_delayed_work(pag->pag_mount->m_gc_workqueue,
				   &pag->pag_blockgc_work,
				   msecs_to_jiffies(xfs_blockgc_secs * 1000));
	rcu_read_unlock();
}

static void
xfs_blockgc_set_iflag(
	struct xfs_inode	*ip,
	unsigned long		iflag)
{
	struct xfs_mount	*mp = ip->i_mount;
	struct xfs_perag	*pag;
	int			tagged;

	ASSERT((iflag & ~(XFS_IEOFBLOCKS | XFS_ICOWBLOCKS)) == 0);

	/*
	 * Don't bother locking the AG and looking up in the radix trees
	 * if we already know that we have the tag set.
	 */
	if (ip->i_flags & iflag)
		return;
	spin_lock(&ip->i_flags_lock);
	ip->i_flags |= iflag;
	spin_unlock(&ip->i_flags_lock);

	pag = xfs_perag_get(mp, XFS_INO_TO_AGNO(mp, ip->i_ino));
	spin_lock(&pag->pag_ici_lock);

	tagged = radix_tree_tagged(&pag->pag_ici_root, XFS_ICI_BLOCKGC_TAG);
	radix_tree_tag_set(&pag->pag_ici_root,
			   XFS_INO_TO_AGINO(ip->i_mount, ip->i_ino),
			   XFS_ICI_BLOCKGC_TAG);
	if (!tagged) {
		/* propagate the blockgc tag up into the perag radix tree */
		spin_lock(&ip->i_mount->m_perag_lock);
		radix_tree_tag_set(&ip->i_mount->m_perag_tree,
				   XFS_INO_TO_AGNO(ip->i_mount, ip->i_ino),
				   XFS_ICI_BLOCKGC_TAG);
		spin_unlock(&ip->i_mount->m_perag_lock);

		/* kick off background trimming */
		xfs_blockgc_queue(pag);

		trace_xfs_perag_set_blockgc(ip->i_mount, pag->pag_agno, -1,
				_RET_IP_);
	}

	spin_unlock(&pag->pag_ici_lock);
	xfs_perag_put(pag);
}

void
xfs_inode_set_eofblocks_tag(
	xfs_inode_t	*ip)
{
	trace_xfs_inode_set_eofblocks_tag(ip);
	return xfs_blockgc_set_iflag(ip, XFS_IEOFBLOCKS);
}

static void
xfs_blockgc_clear_iflag(
	struct xfs_inode	*ip,
	unsigned long		iflag)
{
	struct xfs_mount	*mp = ip->i_mount;
	struct xfs_perag	*pag;
	bool			clear_tag;

	ASSERT((iflag & ~(XFS_IEOFBLOCKS | XFS_ICOWBLOCKS)) == 0);

	spin_lock(&ip->i_flags_lock);
	ip->i_flags &= ~iflag;
	clear_tag = (ip->i_flags & (XFS_IEOFBLOCKS | XFS_ICOWBLOCKS)) == 0;
	spin_unlock(&ip->i_flags_lock);

	if (!clear_tag)
		return;

	pag = xfs_perag_get(mp, XFS_INO_TO_AGNO(mp, ip->i_ino));
	spin_lock(&pag->pag_ici_lock);

	radix_tree_tag_clear(&pag->pag_ici_root,
			     XFS_INO_TO_AGINO(ip->i_mount, ip->i_ino),
			     XFS_ICI_BLOCKGC_TAG);
	if (!radix_tree_tagged(&pag->pag_ici_root, XFS_ICI_BLOCKGC_TAG)) {
		/* clear the blockgc tag from the perag radix tree */
		spin_lock(&ip->i_mount->m_perag_lock);
		radix_tree_tag_clear(&ip->i_mount->m_perag_tree,
				     XFS_INO_TO_AGNO(ip->i_mount, ip->i_ino),
				     XFS_ICI_BLOCKGC_TAG);
		spin_unlock(&ip->i_mount->m_perag_lock);
		trace_xfs_perag_clear_blockgc(ip->i_mount, pag->pag_agno, -1,
				_RET_IP_);
	}

	spin_unlock(&pag->pag_ici_lock);
	xfs_perag_put(pag);
}

void
xfs_inode_clear_eofblocks_tag(
	xfs_inode_t	*ip)
{
	trace_xfs_inode_clear_eofblocks_tag(ip);
	return xfs_blockgc_clear_iflag(ip, XFS_IEOFBLOCKS);
}

/*
 * Set ourselves up to free CoW blocks from this file.  If it's already clean
 * then we can bail out quickly, but otherwise we must back off if the file
 * is undergoing some kind of write.
 */
static bool
xfs_prep_free_cowblocks(
	struct xfs_inode	*ip)
{
	/*
	 * Just clear the tag if we have an empty cow fork or none at all. It's
	 * possible the inode was fully unshared since it was originally tagged.
	 */
	if (!xfs_inode_has_cow_data(ip)) {
		trace_xfs_inode_free_cowblocks_invalid(ip);
		xfs_inode_clear_cowblocks_tag(ip);
		return false;
	}

	/*
	 * If the mapping is dirty or under writeback we cannot touch the
	 * CoW fork.  Leave it alone if we're in the midst of a directio.
	 */
	if ((VFS_I(ip)->i_state & I_DIRTY_PAGES) ||
	    mapping_tagged(VFS_I(ip)->i_mapping, PAGECACHE_TAG_DIRTY) ||
	    mapping_tagged(VFS_I(ip)->i_mapping, PAGECACHE_TAG_WRITEBACK) ||
	    atomic_read(&VFS_I(ip)->i_dio_count))
		return false;

	return true;
}

/*
 * Automatic CoW Reservation Freeing
 *
 * These functions automatically garbage collect leftover CoW reservations
 * that were made on behalf of a cowextsize hint when we start to run out
 * of quota or when the reservations sit around for too long.  If the file
 * has dirty pages or is undergoing writeback, its CoW reservations will
 * be retained.
 *
 * The actual garbage collection piggybacks off the same code that runs
 * the speculative EOF preallocation garbage collector.
 */
STATIC int
xfs_inode_free_cowblocks(
	struct xfs_inode	*ip,
	void			*args,
	unsigned int		*lockflags)
{
	struct xfs_eofblocks	*eofb = args;
	bool			wait;
	int			ret = 0;

	wait = eofb && (eofb->eof_flags & XFS_EOF_FLAGS_SYNC);

	if (!xfs_iflags_test(ip, XFS_ICOWBLOCKS))
		return 0;

	if (!xfs_prep_free_cowblocks(ip))
		return 0;

	if (!xfs_inode_matches_eofb(ip, eofb))
		return 0;

	/*
	 * If the caller is waiting, return -EAGAIN to keep the background
	 * scanner moving and revisit the inode in a subsequent pass.
	 */
	if (!(*lockflags & XFS_IOLOCK_EXCL) &&
	    !xfs_ilock_nowait(ip, XFS_IOLOCK_EXCL)) {
		if (wait)
			return -EAGAIN;
		return 0;
	}
	*lockflags |= XFS_IOLOCK_EXCL;

	if (!xfs_ilock_nowait(ip, XFS_MMAPLOCK_EXCL)) {
		if (wait)
			return -EAGAIN;
		return 0;
	}
	*lockflags |= XFS_MMAPLOCK_EXCL;

	/*
	 * Check again, nobody else should be able to dirty blocks or change
	 * the reflink iflag now that we have the first two locks held.
	 */
	if (xfs_prep_free_cowblocks(ip))
		ret = xfs_reflink_cancel_cow_range(ip, 0, NULLFILEOFF, false);
	return ret;
}

void
xfs_inode_set_cowblocks_tag(
	xfs_inode_t	*ip)
{
	trace_xfs_inode_set_cowblocks_tag(ip);
	return xfs_blockgc_set_iflag(ip, XFS_ICOWBLOCKS);
}

void
xfs_inode_clear_cowblocks_tag(
	xfs_inode_t	*ip)
{
	trace_xfs_inode_clear_cowblocks_tag(ip);
	return xfs_blockgc_clear_iflag(ip, XFS_ICOWBLOCKS);
}

#define for_each_perag_tag(mp, next_agno, pag, tag) \
	for ((next_agno) = 0, (pag) = xfs_perag_get_tag((mp), 0, (tag)); \
		(pag) != NULL; \
		(next_agno) = (pag)->pag_agno + 1, \
		xfs_perag_put(pag), \
		(pag) = xfs_perag_get_tag((mp), (next_agno), (tag)))


/* Disable post-EOF and CoW block auto-reclamation. */
void
xfs_blockgc_stop(
	struct xfs_mount	*mp)
{
	struct xfs_perag	*pag;
	xfs_agnumber_t		agno;

	for_each_perag_tag(mp, agno, pag, XFS_ICI_BLOCKGC_TAG)
		cancel_delayed_work_sync(&pag->pag_blockgc_work);
}

/* Enable post-EOF and CoW block auto-reclamation. */
void
xfs_blockgc_start(
	struct xfs_mount	*mp)
{
	struct xfs_perag	*pag;
	xfs_agnumber_t		agno;

	for_each_perag_tag(mp, agno, pag, XFS_ICI_BLOCKGC_TAG)
		xfs_blockgc_queue(pag);
}

/* Scan one incore inode for block preallocations that we can remove. */
static int
xfs_blockgc_scan_inode(
	struct xfs_inode	*ip,
	void			*args)
{
	unsigned int		lockflags = 0;
	int			error;

	error = xfs_inode_free_eofblocks(ip, args, &lockflags);
	if (error)
		goto unlock;

	error = xfs_inode_free_cowblocks(ip, args, &lockflags);
unlock:
	if (lockflags)
		xfs_iunlock(ip, lockflags);
	return error;
}

/* Background worker that trims preallocated space. */
void
xfs_blockgc_worker(
	struct work_struct	*work)
{
	struct xfs_perag	*pag = container_of(to_delayed_work(work),
					struct xfs_perag, pag_blockgc_work);
	struct xfs_mount	*mp = pag->pag_mount;
	int			error;

	if (!sb_start_write_trylock(mp->m_super))
		return;
	error = xfs_inode_walk_ag(pag, 0, xfs_blockgc_scan_inode, NULL,
			XFS_ICI_BLOCKGC_TAG);
	if (error)
		xfs_info(mp, "AG %u preallocation gc worker failed, err=%d",
				pag->pag_agno, error);
	sb_end_write(mp->m_super);
	xfs_blockgc_queue(pag);
}

/*
 * Try to free space in the filesystem by purging eofblocks and cowblocks.
 */
int
xfs_blockgc_free_space(
	struct xfs_mount	*mp,
	struct xfs_eofblocks	*eofb)
{
	trace_xfs_blockgc_free_space(mp, eofb, _RET_IP_);

	return xfs_inode_walk(mp, 0, xfs_blockgc_scan_inode, eofb,
			XFS_ICI_BLOCKGC_TAG);
}

/*
 * Run cow/eofblocks scans on the supplied dquots.  We don't know exactly which
 * quota caused an allocation failure, so we make a best effort by including
 * each quota under low free space conditions (less than 1% free space) in the
 * scan.
 *
 * Callers must not hold any inode's ILOCK.  If requesting a synchronous scan
 * (XFS_EOF_FLAGS_SYNC), the caller also must not hold any inode's IOLOCK or
 * MMAPLOCK.
 */
int
xfs_blockgc_free_dquots(
	struct xfs_mount	*mp,
	struct xfs_dquot	*udqp,
	struct xfs_dquot	*gdqp,
	struct xfs_dquot	*pdqp,
	unsigned int		eof_flags)
{
	struct xfs_eofblocks	eofb = {0};
	bool			do_work = false;

	if (!udqp && !gdqp && !pdqp)
		return 0;

	/*
	 * Run a scan to free blocks using the union filter to cover all
	 * applicable quotas in a single scan.
	 */
	eofb.eof_flags = XFS_EOF_FLAGS_UNION | eof_flags;

	if (XFS_IS_UQUOTA_ENFORCED(mp) && udqp && xfs_dquot_lowsp(udqp)) {
		eofb.eof_uid = make_kuid(mp->m_super->s_user_ns, udqp->q_id);
		eofb.eof_flags |= XFS_EOF_FLAGS_UID;
		do_work = true;
	}

	if (XFS_IS_UQUOTA_ENFORCED(mp) && gdqp && xfs_dquot_lowsp(gdqp)) {
		eofb.eof_gid = make_kgid(mp->m_super->s_user_ns, gdqp->q_id);
		eofb.eof_flags |= XFS_EOF_FLAGS_GID;
		do_work = true;
	}

	if (XFS_IS_PQUOTA_ENFORCED(mp) && pdqp && xfs_dquot_lowsp(pdqp)) {
		eofb.eof_prid = pdqp->q_id;
		eofb.eof_flags |= XFS_EOF_FLAGS_PRID;
		do_work = true;
	}

	if (!do_work)
		return 0;

	return xfs_blockgc_free_space(mp, &eofb);
}

/* Run cow/eofblocks scans on the quotas attached to the inode. */
int
xfs_blockgc_free_quota(
	struct xfs_inode	*ip,
	unsigned int		eof_flags)
{
	return xfs_blockgc_free_dquots(ip->i_mount,
			xfs_inode_dquot(ip, XFS_DQTYPE_USER),
			xfs_inode_dquot(ip, XFS_DQTYPE_GROUP),
			xfs_inode_dquot(ip, XFS_DQTYPE_PROJ), eof_flags);
}
