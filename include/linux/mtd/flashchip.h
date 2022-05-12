/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright © 2000      Red Hat UK Limited
 * Copyright © 2000-2010 David Woodhouse <dwmw2@infradead.org>
 */

#ifndef __MTD_FLASHCHIP_H__
#define __MTD_FLASHCHIP_H__

/* For spinlocks. sched.h includes spinlock.h from whichever directory it
 * happens to be in - so we don't have to care whether we're on 2.2, which
 * has asm/spinlock.h, or 2.4, which has linux/spinlock.h
 */
#include <linux/sched.h>
#include <linux/mutex.h>

typedef enum {
	FL_READY,
	FL_STATUS,
	FL_CFI_QUERY,
	FL_JEDEC_QUERY,
	FL_ERASING,
	FL_ERASE_SUSPENDING,
	FL_ERASE_SUSPENDED,
	FL_WRITING,
	FL_WRITING_TO_BUFFER,
	FL_OTP_WRITE,
	FL_WRITE_SUSPENDING,
	FL_WRITE_SUSPENDED,
	FL_PM_SUSPENDED,
	FL_SYNCING,
	FL_UNLOADING,
	FL_LOCKING,
	FL_UNLOCKING,
	FL_POINT,
	FL_XIP_WHILE_ERASING,
	FL_XIP_WHILE_WRITING,
	FL_SHUTDOWN,
	/* These 2 come from nand_state_t, which has been unified here */
	FL_READING,
	FL_CACHEDPRG,
	/* These 4 come from onenand_state_t, which has been unified here */
	FL_RESETTING,
	FL_OTPING,
	FL_PREPARING_ERASE,
	FL_VERIFYING_ERASE,

	FL_UNKNOWN
} flstate_t;



/* NOTE: confusingly, this can be used to refer to more than one chip at a time,
   if they're interleaved.  This can even refer to individual partitions on
   the same physical chip when present. */

struct flchip {
	unsigned long start; /* Offset within the map */
	//	unsigned long len;
	/* We omit len for now, because when we group them together
	   we insist that they're all of the same size, and the chip size
	   is held in the next level up. If we get more versatile later,
	   it'll make it a damn sight harder to find which chip we want from
	   a given offset, and we'll want to add the per-chip length field
	   back in.
	*/
	int ref_point_counter;
	flstate_t state;
	flstate_t oldstate;

	unsigned int write_suspended:1;
	unsigned int erase_suspended:1;
	unsigned long in_progress_block_addr;
	unsigned long in_progress_block_mask;

	struct mutex mutex;
	wait_queue_head_t wq; /* Wait on here when we're waiting for the chip
			     to be ready */
	int word_write_time;
	int buffer_write_time;
	int erase_time;

	int word_write_time_max;
	int buffer_write_time_max;
	int erase_time_max;

	void *priv;
};

/* This is used to handle contention on write/erase operations
   between partitions of the same physical chip. */
struct flchip_shared {
	struct mutex lock;
	struct flchip *writing;
	struct flchip *erasing;
};


#endif /* __MTD_FLASHCHIP_H__ */
