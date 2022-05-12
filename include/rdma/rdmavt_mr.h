/* SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause */
/*
 * Copyright(c) 2016 Intel Corporation.
 */

#ifndef DEF_RDMAVT_INCMR_H
#define DEF_RDMAVT_INCMR_H

/*
 * For Memory Regions. This stuff should probably be moved into rdmavt/mr.h once
 * drivers no longer need access to the MR directly.
 */
#include <linux/percpu-refcount.h>

/*
 * A segment is a linear region of low physical memory.
 * Used by the verbs layer.
 */
struct rvt_seg {
	void *vaddr;
	size_t length;
};

/* The number of rvt_segs that fit in a page. */
#define RVT_SEGSZ     (PAGE_SIZE / sizeof(struct rvt_seg))

struct rvt_segarray {
	struct rvt_seg segs[RVT_SEGSZ];
};

struct rvt_mregion {
	struct ib_pd *pd;       /* shares refcnt of ibmr.pd */
	u64 user_base;          /* User's address for this region */
	u64 iova;               /* IB start address of this region */
	size_t length;
	u32 lkey;
	u32 offset;             /* offset (bytes) to start of region */
	int access_flags;
	u32 max_segs;           /* number of rvt_segs in all the arrays */
	u32 mapsz;              /* size of the map array */
	atomic_t lkey_invalid;	/* true if current lkey is invalid */
	u8  page_shift;         /* 0 - non unform/non powerof2 sizes */
	u8  lkey_published;     /* in global table */
	struct percpu_ref refcount;
	struct completion comp; /* complete when refcount goes to zero */
	struct rvt_segarray *map[];    /* the segments */
};

#define RVT_MAX_LKEY_TABLE_BITS 23

struct rvt_lkey_table {
	/* read mostly fields */
	u32 max;                /* size of the table */
	u32 shift;              /* lkey/rkey shift */
	struct rvt_mregion __rcu **table;
	/* writeable fields */
	/* protect changes in this struct */
	spinlock_t lock ____cacheline_aligned_in_smp;
	u32 next;               /* next unused index (speeds search) */
	u32 gen;                /* generation count */
};

/*
 * These keep track of the copy progress within a memory region.
 * Used by the verbs layer.
 */
struct rvt_sge {
	struct rvt_mregion *mr;
	void *vaddr;            /* kernel virtual address of segment */
	u32 sge_length;         /* length of the SGE */
	u32 length;             /* remaining length of the segment */
	u16 m;                  /* current index: mr->map[m] */
	u16 n;                  /* current index: mr->map[m]->segs[n] */
};

struct rvt_sge_state {
	struct rvt_sge *sg_list;      /* next SGE to be used if any */
	struct rvt_sge sge;   /* progress state for the current SGE */
	u32 total_len;
	u8 num_sge;
};

static inline void rvt_put_mr(struct rvt_mregion *mr)
{
	percpu_ref_put(&mr->refcount);
}

static inline void rvt_get_mr(struct rvt_mregion *mr)
{
	percpu_ref_get(&mr->refcount);
}

static inline void rvt_put_ss(struct rvt_sge_state *ss)
{
	while (ss->num_sge) {
		rvt_put_mr(ss->sge.mr);
		if (--ss->num_sge)
			ss->sge = *ss->sg_list++;
	}
}

static inline u32 rvt_get_sge_length(struct rvt_sge *sge, u32 length)
{
	u32 len = sge->length;

	if (len > length)
		len = length;
	if (len > sge->sge_length)
		len = sge->sge_length;

	return len;
}

static inline void rvt_update_sge(struct rvt_sge_state *ss, u32 length,
				  bool release)
{
	struct rvt_sge *sge = &ss->sge;

	sge->vaddr += length;
	sge->length -= length;
	sge->sge_length -= length;
	if (sge->sge_length == 0) {
		if (release)
			rvt_put_mr(sge->mr);
		if (--ss->num_sge)
			*sge = *ss->sg_list++;
	} else if (sge->length == 0 && sge->mr->lkey) {
		if (++sge->n >= RVT_SEGSZ) {
			if (++sge->m >= sge->mr->mapsz)
				return;
			sge->n = 0;
		}
		sge->vaddr = sge->mr->map[sge->m]->segs[sge->n].vaddr;
		sge->length = sge->mr->map[sge->m]->segs[sge->n].length;
	}
}

static inline void rvt_skip_sge(struct rvt_sge_state *ss, u32 length,
				bool release)
{
	struct rvt_sge *sge = &ss->sge;

	while (length) {
		u32 len = rvt_get_sge_length(sge, length);

		WARN_ON_ONCE(len == 0);
		rvt_update_sge(ss, len, release);
		length -= len;
	}
}

bool rvt_ss_has_lkey(struct rvt_sge_state *ss, u32 lkey);
bool rvt_mr_has_lkey(struct rvt_mregion *mr, u32 lkey);

#endif          /* DEF_RDMAVT_INCMRH */
