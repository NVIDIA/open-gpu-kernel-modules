/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _RAID10_H
#define _RAID10_H

/* Note: raid10_info.rdev can be set to NULL asynchronously by
 * raid10_remove_disk.
 * There are three safe ways to access raid10_info.rdev.
 * 1/ when holding mddev->reconfig_mutex
 * 2/ when resync/recovery/reshape is known to be happening - i.e. in code
 *    that is called as part of performing resync/recovery/reshape.
 * 3/ while holding rcu_read_lock(), use rcu_dereference to get the pointer
 *    and if it is non-NULL, increment rdev->nr_pending before dropping the
 *    RCU lock.
 * When .rdev is set to NULL, the nr_pending count checked again and if it has
 * been incremented, the pointer is put back in .rdev.
 */

struct raid10_info {
	struct md_rdev	*rdev, *replacement;
	sector_t	head_position;
	int		recovery_disabled;	/* matches
						 * mddev->recovery_disabled
						 * when we shouldn't try
						 * recovering this device.
						 */
};

struct r10conf {
	struct mddev		*mddev;
	struct raid10_info	*mirrors;
	struct raid10_info	*mirrors_new, *mirrors_old;
	spinlock_t		device_lock;

	/* geometry */
	struct geom {
		int		raid_disks;
		int		near_copies;  /* number of copies laid out
					       * raid0 style */
		int		far_copies;   /* number of copies laid out
					       * at large strides across drives
					       */
		int		far_offset;   /* far_copies are offset by 1
					       * stripe instead of many
					       */
		sector_t	stride;	      /* distance between far copies.
					       * This is size / far_copies unless
					       * far_offset, in which case it is
					       * 1 stripe.
					       */
		int             far_set_size; /* The number of devices in a set,
					       * where a 'set' are devices that
					       * contain far/offset copies of
					       * each other.
					       */
		int		chunk_shift; /* shift from chunks to sectors */
		sector_t	chunk_mask;
	} prev, geo;
	int			copies;	      /* near_copies * far_copies.
					       * must be <= raid_disks
					       */

	sector_t		dev_sectors;  /* temp copy of
					       * mddev->dev_sectors */
	sector_t		reshape_progress;
	sector_t		reshape_safe;
	unsigned long		reshape_checkpoint;
	sector_t		offset_diff;

	struct list_head	retry_list;
	/* A separate list of r1bio which just need raid_end_bio_io called.
	 * This mustn't happen for writes which had any errors if the superblock
	 * needs to be written.
	 */
	struct list_head	bio_end_io_list;

	/* queue pending writes and submit them on unplug */
	struct bio_list		pending_bio_list;
	int			pending_count;

	spinlock_t		resync_lock;
	atomic_t		nr_pending;
	int			nr_waiting;
	int			nr_queued;
	int			barrier;
	int			array_freeze_pending;
	sector_t		next_resync;
	int			fullsync;  /* set to 1 if a full sync is needed,
					    * (fresh device added).
					    * Cleared when a sync completes.
					    */
	int			have_replacement; /* There is at least one
						   * replacement device.
						   */
	wait_queue_head_t	wait_barrier;

	mempool_t		r10bio_pool;
	mempool_t		r10buf_pool;
	struct page		*tmppage;
	struct bio_set		bio_split;

	/* When taking over an array from a different personality, we store
	 * the new thread here until we fully activate the array.
	 */
	struct md_thread	*thread;

	/*
	 * Keep track of cluster resync window to send to other nodes.
	 */
	sector_t		cluster_sync_low;
	sector_t		cluster_sync_high;
};

/*
 * this is our 'private' RAID10 bio.
 *
 * it contains information about what kind of IO operations were started
 * for this RAID10 operation, and about their status:
 */

struct r10bio {
	atomic_t		remaining; /* 'have we finished' count,
					    * used from IRQ handlers
					    */
	sector_t		sector;	/* virtual sector number */
	int			sectors;
	unsigned long		state;
	struct mddev		*mddev;
	/*
	 * original bio going to /dev/mdx
	 */
	struct bio		*master_bio;
	/*
	 * if the IO is in READ direction, then this is where we read
	 */
	int			read_slot;

	struct list_head	retry_list;
	/*
	 * if the IO is in WRITE direction, then multiple bios are used,
	 * one for each copy.
	 * When resyncing we also use one for each copy.
	 * When reconstructing, we use 2 bios, one for read, one for write.
	 * We choose the number when they are allocated.
	 * We sometimes need an extra bio to write to the replacement.
	 */
	struct r10dev {
		struct bio	*bio;
		union {
			struct bio	*repl_bio; /* used for resync and
						    * writes */
			struct md_rdev	*rdev;	   /* used for reads
						    * (read_slot >= 0) */
		};
		sector_t	addr;
		int		devnum;
	} devs[];
};

/* bits for r10bio.state */
enum r10bio_state {
	R10BIO_Uptodate,
	R10BIO_IsSync,
	R10BIO_IsRecover,
	R10BIO_IsReshape,
	R10BIO_Degraded,
/* Set ReadError on bios that experience a read error
 * so that raid10d knows what to do with them.
 */
	R10BIO_ReadError,
/* If a write for this request means we can clear some
 * known-bad-block records, we set this flag.
 */
	R10BIO_MadeGood,
	R10BIO_WriteError,
/* During a reshape we might be performing IO on the
 * 'previous' part of the array, in which case this
 * flag is set
 */
	R10BIO_Previous,
/* failfast devices did receive failfast requests. */
	R10BIO_FailFast,
	R10BIO_Discard,
};
#endif
