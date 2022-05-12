/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-2-Clause) */
/* Copyright (C) 2015-2017 Netronome Systems, Inc. */

#ifndef NFP6000_NFP6000_H
#define NFP6000_NFP6000_H

#include <linux/errno.h>
#include <linux/types.h>

/* CPP Target IDs */
#define NFP_CPP_TARGET_INVALID          0
#define NFP_CPP_TARGET_NBI              1
#define NFP_CPP_TARGET_QDR              2
#define NFP_CPP_TARGET_ILA              6
#define NFP_CPP_TARGET_MU               7
#define NFP_CPP_TARGET_PCIE             9
#define NFP_CPP_TARGET_ARM              10
#define NFP_CPP_TARGET_CRYPTO           12
#define NFP_CPP_TARGET_ISLAND_XPB       14      /* Shared with CAP */
#define NFP_CPP_TARGET_ISLAND_CAP       14      /* Shared with XPB */
#define NFP_CPP_TARGET_CT_XPB           14
#define NFP_CPP_TARGET_LOCAL_SCRATCH    15
#define NFP_CPP_TARGET_CLS              NFP_CPP_TARGET_LOCAL_SCRATCH

#define NFP_ISL_EMEM0			24

#define NFP_MU_ADDR_ACCESS_TYPE_MASK	3ULL
#define NFP_MU_ADDR_ACCESS_TYPE_DIRECT	2ULL

#define PUSHPULL(_pull, _push)		((_pull) << 4 | (_push) << 0)
#define PUSH_WIDTH(_pushpull)		pushpull_width((_pushpull) >> 0)
#define PULL_WIDTH(_pushpull)		pushpull_width((_pushpull) >> 4)

static inline int pushpull_width(int pp)
{
	pp &= 0xf;

	if (pp == 0)
		return -EINVAL;
	return 2 << pp;
}

static inline int nfp_cppat_mu_locality_lsb(int mode, bool addr40)
{
	switch (mode) {
	case 0 ... 3:
		return addr40 ? 38 : 30;
	default:
		return -EINVAL;
	}
}

int nfp_target_pushpull(u32 cpp_id, u64 address);
int nfp_target_cpp(u32 cpp_island_id, u64 cpp_island_address,
		   u32 *cpp_target_id, u64 *cpp_target_address,
		   const u32 *imb_table);

#endif /* NFP6000_NFP6000_H */
