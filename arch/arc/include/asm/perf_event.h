/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Linux performance counter support for ARC
 *
 * Copyright (C) 2014-2015 Synopsys, Inc. (www.synopsys.com)
 * Copyright (C) 2011-2013 Synopsys, Inc. (www.synopsys.com)
 */

#ifndef __ASM_PERF_EVENT_H
#define __ASM_PERF_EVENT_H

/* Max number of counters that PCT block may ever have */
#define ARC_PERF_MAX_COUNTERS	32

#define ARC_REG_CC_BUILD	0xF6
#define ARC_REG_CC_INDEX	0x240
#define ARC_REG_CC_NAME0	0x241
#define ARC_REG_CC_NAME1	0x242

#define ARC_REG_PCT_BUILD	0xF5
#define ARC_REG_PCT_COUNTL	0x250
#define ARC_REG_PCT_COUNTH	0x251
#define ARC_REG_PCT_SNAPL	0x252
#define ARC_REG_PCT_SNAPH	0x253
#define ARC_REG_PCT_CONFIG	0x254
#define ARC_REG_PCT_CONTROL	0x255
#define ARC_REG_PCT_INDEX	0x256
#define ARC_REG_PCT_INT_CNTL	0x25C
#define ARC_REG_PCT_INT_CNTH	0x25D
#define ARC_REG_PCT_INT_CTRL	0x25E
#define ARC_REG_PCT_INT_ACT	0x25F

#define ARC_REG_PCT_CONFIG_USER	(1 << 18)	/* count in user mode */
#define ARC_REG_PCT_CONFIG_KERN	(1 << 19)	/* count in kernel mode */

#define ARC_REG_PCT_CONTROL_CC	(1 << 16)	/* clear counts */
#define ARC_REG_PCT_CONTROL_SN	(1 << 17)	/* snapshot */

struct arc_reg_pct_build {
#ifdef CONFIG_CPU_BIG_ENDIAN
	unsigned int m:8, c:8, r:5, i:1, s:2, v:8;
#else
	unsigned int v:8, s:2, i:1, r:5, c:8, m:8;
#endif
};

struct arc_reg_cc_build {
#ifdef CONFIG_CPU_BIG_ENDIAN
	unsigned int c:16, r:8, v:8;
#else
	unsigned int v:8, r:8, c:16;
#endif
};

#define PERF_COUNT_ARC_DCLM	(PERF_COUNT_HW_MAX + 0)
#define PERF_COUNT_ARC_DCSM	(PERF_COUNT_HW_MAX + 1)
#define PERF_COUNT_ARC_ICM	(PERF_COUNT_HW_MAX + 2)
#define PERF_COUNT_ARC_BPOK	(PERF_COUNT_HW_MAX + 3)
#define PERF_COUNT_ARC_EDTLB	(PERF_COUNT_HW_MAX + 4)
#define PERF_COUNT_ARC_EITLB	(PERF_COUNT_HW_MAX + 5)
#define PERF_COUNT_ARC_LDC	(PERF_COUNT_HW_MAX + 6)
#define PERF_COUNT_ARC_STC	(PERF_COUNT_HW_MAX + 7)

#define PERF_COUNT_ARC_HW_MAX	(PERF_COUNT_HW_MAX + 8)

/*
 * Some ARC pct quirks:
 *
 * PERF_COUNT_HW_STALLED_CYCLES_BACKEND
 * PERF_COUNT_HW_STALLED_CYCLES_FRONTEND
 *	The ARC 700 can either measure stalls per pipeline stage, or all stalls
 *	combined; for now we assign all stalls to STALLED_CYCLES_BACKEND
 *	and all pipeline flushes (e.g. caused by mispredicts, etc.) to
 *	STALLED_CYCLES_FRONTEND.
 *
 *	We could start multiple performance counters and combine everything
 *	afterwards, but that makes it complicated.
 *
 *	Note that I$ cache misses aren't counted by either of the two!
 */

/*
 * ARC PCT has hardware conditions with fixed "names" but variable "indexes"
 * (based on a specific RTL build)
 * Below is the static map between perf generic/arc specific event_id and
 * h/w condition names.
 * At the time of probe, we loop thru each index and find it's name to
 * complete the mapping of perf event_id to h/w index as latter is needed
 * to program the counter really
 */
static const char * const arc_pmu_ev_hw_map[] = {
	/* count cycles */
	[PERF_COUNT_HW_CPU_CYCLES] = "crun",
	[PERF_COUNT_HW_REF_CPU_CYCLES] = "crun",
	[PERF_COUNT_HW_BUS_CYCLES] = "crun",

	[PERF_COUNT_HW_STALLED_CYCLES_FRONTEND] = "bflush",
	[PERF_COUNT_HW_STALLED_CYCLES_BACKEND] = "bstall",

	/* counts condition */
	[PERF_COUNT_HW_INSTRUCTIONS] = "iall",
	/* All jump instructions that are taken */
	[PERF_COUNT_HW_BRANCH_INSTRUCTIONS] = "ijmptak",
#ifdef CONFIG_ISA_ARCV2
	[PERF_COUNT_HW_BRANCH_MISSES] = "bpmp",
#else
	[PERF_COUNT_ARC_BPOK]         = "bpok",	  /* NP-NT, PT-T, PNT-NT */
	[PERF_COUNT_HW_BRANCH_MISSES] = "bpfail", /* NP-T, PT-NT, PNT-T */
#endif
	[PERF_COUNT_ARC_LDC] = "imemrdc",	/* Instr: mem read cached */
	[PERF_COUNT_ARC_STC] = "imemwrc",	/* Instr: mem write cached */

	[PERF_COUNT_ARC_DCLM] = "dclm",		/* D-cache Load Miss */
	[PERF_COUNT_ARC_DCSM] = "dcsm",		/* D-cache Store Miss */
	[PERF_COUNT_ARC_ICM] = "icm",		/* I-cache Miss */
	[PERF_COUNT_ARC_EDTLB] = "edtlb",	/* D-TLB Miss */
	[PERF_COUNT_ARC_EITLB] = "eitlb",	/* I-TLB Miss */

	[PERF_COUNT_HW_CACHE_REFERENCES] = "imemrdc",	/* Instr: mem read cached */
	[PERF_COUNT_HW_CACHE_MISSES] = "dclm",		/* D-cache Load Miss */
};

#define C(_x)			PERF_COUNT_HW_CACHE_##_x
#define CACHE_OP_UNSUPPORTED	0xffff

static const unsigned arc_pmu_cache_map[C(MAX)][C(OP_MAX)][C(RESULT_MAX)] = {
	[C(L1D)] = {
		[C(OP_READ)] = {
			[C(RESULT_ACCESS)]	= PERF_COUNT_ARC_LDC,
			[C(RESULT_MISS)]	= PERF_COUNT_ARC_DCLM,
		},
		[C(OP_WRITE)] = {
			[C(RESULT_ACCESS)]	= PERF_COUNT_ARC_STC,
			[C(RESULT_MISS)]	= PERF_COUNT_ARC_DCSM,
		},
		[C(OP_PREFETCH)] = {
			[C(RESULT_ACCESS)]	= CACHE_OP_UNSUPPORTED,
			[C(RESULT_MISS)]	= CACHE_OP_UNSUPPORTED,
		},
	},
	[C(L1I)] = {
		[C(OP_READ)] = {
			[C(RESULT_ACCESS)]	= PERF_COUNT_HW_INSTRUCTIONS,
			[C(RESULT_MISS)]	= PERF_COUNT_ARC_ICM,
		},
		[C(OP_WRITE)] = {
			[C(RESULT_ACCESS)]	= CACHE_OP_UNSUPPORTED,
			[C(RESULT_MISS)]	= CACHE_OP_UNSUPPORTED,
		},
		[C(OP_PREFETCH)] = {
			[C(RESULT_ACCESS)]	= CACHE_OP_UNSUPPORTED,
			[C(RESULT_MISS)]	= CACHE_OP_UNSUPPORTED,
		},
	},
	[C(LL)] = {
		[C(OP_READ)] = {
			[C(RESULT_ACCESS)]	= CACHE_OP_UNSUPPORTED,
			[C(RESULT_MISS)]	= CACHE_OP_UNSUPPORTED,
		},
		[C(OP_WRITE)] = {
			[C(RESULT_ACCESS)]	= CACHE_OP_UNSUPPORTED,
			[C(RESULT_MISS)]	= CACHE_OP_UNSUPPORTED,
		},
		[C(OP_PREFETCH)] = {
			[C(RESULT_ACCESS)]	= CACHE_OP_UNSUPPORTED,
			[C(RESULT_MISS)]	= CACHE_OP_UNSUPPORTED,
		},
	},
	[C(DTLB)] = {
		[C(OP_READ)] = {
			[C(RESULT_ACCESS)]	= PERF_COUNT_ARC_LDC,
			[C(RESULT_MISS)]	= PERF_COUNT_ARC_EDTLB,
		},
			/* DTLB LD/ST Miss not segregated by h/w*/
		[C(OP_WRITE)] = {
			[C(RESULT_ACCESS)]	= CACHE_OP_UNSUPPORTED,
			[C(RESULT_MISS)]	= CACHE_OP_UNSUPPORTED,
		},
		[C(OP_PREFETCH)] = {
			[C(RESULT_ACCESS)]	= CACHE_OP_UNSUPPORTED,
			[C(RESULT_MISS)]	= CACHE_OP_UNSUPPORTED,
		},
	},
	[C(ITLB)] = {
		[C(OP_READ)] = {
			[C(RESULT_ACCESS)]	= CACHE_OP_UNSUPPORTED,
			[C(RESULT_MISS)]	= PERF_COUNT_ARC_EITLB,
		},
		[C(OP_WRITE)] = {
			[C(RESULT_ACCESS)]	= CACHE_OP_UNSUPPORTED,
			[C(RESULT_MISS)]	= CACHE_OP_UNSUPPORTED,
		},
		[C(OP_PREFETCH)] = {
			[C(RESULT_ACCESS)]	= CACHE_OP_UNSUPPORTED,
			[C(RESULT_MISS)]	= CACHE_OP_UNSUPPORTED,
		},
	},
	[C(BPU)] = {
		[C(OP_READ)] = {
			[C(RESULT_ACCESS)] = PERF_COUNT_HW_BRANCH_INSTRUCTIONS,
			[C(RESULT_MISS)]	= PERF_COUNT_HW_BRANCH_MISSES,
		},
		[C(OP_WRITE)] = {
			[C(RESULT_ACCESS)]	= CACHE_OP_UNSUPPORTED,
			[C(RESULT_MISS)]	= CACHE_OP_UNSUPPORTED,
		},
		[C(OP_PREFETCH)] = {
			[C(RESULT_ACCESS)]	= CACHE_OP_UNSUPPORTED,
			[C(RESULT_MISS)]	= CACHE_OP_UNSUPPORTED,
		},
	},
	[C(NODE)] = {
		[C(OP_READ)] = {
			[C(RESULT_ACCESS)]	= CACHE_OP_UNSUPPORTED,
			[C(RESULT_MISS)]	= CACHE_OP_UNSUPPORTED,
		},
		[C(OP_WRITE)] = {
			[C(RESULT_ACCESS)]	= CACHE_OP_UNSUPPORTED,
			[C(RESULT_MISS)]	= CACHE_OP_UNSUPPORTED,
		},
		[C(OP_PREFETCH)] = {
			[C(RESULT_ACCESS)]	= CACHE_OP_UNSUPPORTED,
			[C(RESULT_MISS)]	= CACHE_OP_UNSUPPORTED,
		},
	},
};

#endif /* __ASM_PERF_EVENT_H */
