/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * segment.h - NILFS Segment constructor prototypes and definitions
 *
 * Copyright (C) 2005-2008 Nippon Telegraph and Telephone Corporation.
 *
 * Written by Ryusuke Konishi.
 *
 */
#ifndef _NILFS_SEGMENT_H
#define _NILFS_SEGMENT_H

#include <linux/types.h>
#include <linux/fs.h>
#include <linux/buffer_head.h>
#include <linux/workqueue.h>
#include "nilfs.h"

struct nilfs_root;

/**
 * struct nilfs_recovery_info - Recovery information
 * @ri_need_recovery: Recovery status
 * @ri_super_root: Block number of the last super root
 * @ri_ri_cno: Number of the last checkpoint
 * @ri_lsegs_start: Region for roll-forwarding (start block number)
 * @ri_lsegs_end: Region for roll-forwarding (end block number)
 * @ri_lseg_start_seq: Sequence value of the segment at ri_lsegs_start
 * @ri_used_segments: List of segments to be mark active
 * @ri_pseg_start: Block number of the last partial segment
 * @ri_seq: Sequence number on the last partial segment
 * @ri_segnum: Segment number on the last partial segment
 * @ri_nextnum: Next segment number on the last partial segment
 */
struct nilfs_recovery_info {
	int			ri_need_recovery;
	sector_t		ri_super_root;
	__u64			ri_cno;

	sector_t		ri_lsegs_start;
	sector_t		ri_lsegs_end;
	u64			ri_lsegs_start_seq;
	struct list_head	ri_used_segments;
	sector_t		ri_pseg_start;
	u64			ri_seq;
	__u64			ri_segnum;
	__u64			ri_nextnum;
};

/* ri_need_recovery */
#define NILFS_RECOVERY_SR_UPDATED	 1  /* The super root was updated */
#define NILFS_RECOVERY_ROLLFORWARD_DONE	 2  /* Rollforward was carried out */

/**
 * struct nilfs_cstage - Context of collection stage
 * @scnt: Stage count, must be accessed via wrappers:
 *        nilfs_sc_cstage_inc(), nilfs_sc_cstage_set(), nilfs_sc_cstage_get()
 * @flags: State flags
 * @dirty_file_ptr: Pointer on dirty_files list, or inode of a target file
 * @gc_inode_ptr: Pointer on the list of gc-inodes
 */
struct nilfs_cstage {
	int			scnt;
	unsigned int		flags;
	struct nilfs_inode_info *dirty_file_ptr;
	struct nilfs_inode_info *gc_inode_ptr;
};

struct nilfs_segment_buffer;

struct nilfs_segsum_pointer {
	struct buffer_head     *bh;
	unsigned int		offset; /* offset in bytes */
};

/**
 * struct nilfs_sc_info - Segment constructor information
 * @sc_super: Back pointer to super_block struct
 * @sc_root: root object of the current filesystem tree
 * @sc_nblk_inc: Block count of current generation
 * @sc_dirty_files: List of files to be written
 * @sc_gc_inodes: List of GC inodes having blocks to be written
 * @sc_iput_queue: list of inodes for which iput should be done
 * @sc_iput_work: work struct to defer iput call
 * @sc_freesegs: array of segment numbers to be freed
 * @sc_nfreesegs: number of segments on @sc_freesegs
 * @sc_dsync_inode: inode whose data pages are written for a sync operation
 * @sc_dsync_start: start byte offset of data pages
 * @sc_dsync_end: end byte offset of data pages (inclusive)
 * @sc_segbufs: List of segment buffers
 * @sc_write_logs: List of segment buffers to hold logs under writing
 * @sc_segbuf_nblocks: Number of available blocks in segment buffers.
 * @sc_curseg: Current segment buffer
 * @sc_stage: Collection stage
 * @sc_finfo_ptr: pointer to the current finfo struct in the segment summary
 * @sc_binfo_ptr: pointer to the current binfo struct in the segment summary
 * @sc_blk_cnt:	Block count of a file
 * @sc_datablk_cnt: Data block count of a file
 * @sc_nblk_this_inc: Number of blocks included in the current logical segment
 * @sc_seg_ctime: Creation time
 * @sc_cno: checkpoint number of current log
 * @sc_flags: Internal flags
 * @sc_state_lock: spinlock for sc_state and so on
 * @sc_state: Segctord state flags
 * @sc_flush_request: inode bitmap of metadata files to be flushed
 * @sc_wait_request: Client request queue
 * @sc_wait_daemon: Daemon wait queue
 * @sc_wait_task: Start/end wait queue to control segctord task
 * @sc_seq_request: Request counter
 * @sc_seq_accept: Accepted request count
 * @sc_seq_done: Completion counter
 * @sc_sync: Request of explicit sync operation
 * @sc_interval: Timeout value of background construction
 * @sc_mjcp_freq: Frequency of creating checkpoints
 * @sc_lseg_stime: Start time of the latest logical segment
 * @sc_watermark: Watermark for the number of dirty buffers
 * @sc_timer: Timer for segctord
 * @sc_task: current thread of segctord
 */
struct nilfs_sc_info {
	struct super_block     *sc_super;
	struct nilfs_root      *sc_root;

	unsigned long		sc_nblk_inc;

	struct list_head	sc_dirty_files;
	struct list_head	sc_gc_inodes;
	struct list_head	sc_iput_queue;
	struct work_struct	sc_iput_work;

	__u64		       *sc_freesegs;
	size_t			sc_nfreesegs;

	struct nilfs_inode_info *sc_dsync_inode;
	loff_t			sc_dsync_start;
	loff_t			sc_dsync_end;

	/* Segment buffers */
	struct list_head	sc_segbufs;
	struct list_head	sc_write_logs;
	unsigned long		sc_segbuf_nblocks;
	struct nilfs_segment_buffer *sc_curseg;

	struct nilfs_cstage	sc_stage;

	struct nilfs_segsum_pointer sc_finfo_ptr;
	struct nilfs_segsum_pointer sc_binfo_ptr;
	unsigned long		sc_blk_cnt;
	unsigned long		sc_datablk_cnt;
	unsigned long		sc_nblk_this_inc;
	time64_t		sc_seg_ctime;
	__u64			sc_cno;
	unsigned long		sc_flags;

	spinlock_t		sc_state_lock;
	unsigned long		sc_state;
	unsigned long		sc_flush_request;

	wait_queue_head_t	sc_wait_request;
	wait_queue_head_t	sc_wait_daemon;
	wait_queue_head_t	sc_wait_task;

	__u32			sc_seq_request;
	__u32			sc_seq_accepted;
	__u32			sc_seq_done;

	int			sc_sync;
	unsigned long		sc_interval;
	unsigned long		sc_mjcp_freq;
	unsigned long		sc_lseg_stime;	/* in 1/HZ seconds */
	unsigned long		sc_watermark;

	struct timer_list	sc_timer;
	struct task_struct     *sc_timer_task;
	struct task_struct     *sc_task;
};

/* sc_flags */
enum {
	NILFS_SC_DIRTY,		/* One or more dirty meta-data blocks exist */
	NILFS_SC_UNCLOSED,	/* Logical segment is not closed */
	NILFS_SC_SUPER_ROOT,	/* The latest segment has a super root */
	NILFS_SC_PRIOR_FLUSH,	/*
				 * Requesting immediate flush without making a
				 * checkpoint
				 */
	NILFS_SC_HAVE_DELTA,	/*
				 * Next checkpoint will have update of files
				 * other than DAT, cpfile, sufile, or files
				 * moved by GC.
				 */
};

/* sc_state */
#define NILFS_SEGCTOR_QUIT	    0x0001  /* segctord is being destroyed */
#define NILFS_SEGCTOR_COMMIT	    0x0004  /* committed transaction exists */

/*
 * Constant parameters
 */
#define NILFS_SC_CLEANUP_RETRY	    3  /*
					* Retry count of construction when
					* destroying segctord
					*/

/*
 * Default values of timeout, in seconds.
 */
#define NILFS_SC_DEFAULT_TIMEOUT    5   /*
					 * Timeout value of dirty blocks.
					 * It triggers construction of a
					 * logical segment with a super root.
					 */
#define NILFS_SC_DEFAULT_SR_FREQ    30  /*
					 * Maximum frequency of super root
					 * creation
					 */

/*
 * The default threshold amount of data, in block counts.
 */
#define NILFS_SC_DEFAULT_WATERMARK  3600

/* super.c */
extern struct kmem_cache *nilfs_transaction_cachep;

/* segment.c */
extern void nilfs_relax_pressure_in_lock(struct super_block *);

extern int nilfs_construct_segment(struct super_block *);
extern int nilfs_construct_dsync_segment(struct super_block *, struct inode *,
					 loff_t, loff_t);
extern void nilfs_flush_segment(struct super_block *, ino_t);
extern int nilfs_clean_segments(struct super_block *, struct nilfs_argv *,
				void **);

int nilfs_attach_log_writer(struct super_block *sb, struct nilfs_root *root);
void nilfs_detach_log_writer(struct super_block *sb);

/* recovery.c */
extern int nilfs_read_super_root_block(struct the_nilfs *, sector_t,
				       struct buffer_head **, int);
extern int nilfs_search_super_root(struct the_nilfs *,
				   struct nilfs_recovery_info *);
int nilfs_salvage_orphan_logs(struct the_nilfs *nilfs, struct super_block *sb,
			      struct nilfs_recovery_info *ri);
extern void nilfs_dispose_segment_list(struct list_head *);

#endif /* _NILFS_SEGMENT_H */
