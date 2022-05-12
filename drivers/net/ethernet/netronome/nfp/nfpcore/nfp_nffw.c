// SPDX-License-Identifier: (GPL-2.0-only OR BSD-2-Clause)
/* Copyright (C) 2015-2018 Netronome Systems, Inc. */

/*
 * nfp_nffw.c
 * Authors: Jakub Kicinski <jakub.kicinski@netronome.com>
 *          Jason McMullan <jason.mcmullan@netronome.com>
 *          Francois H. Theron <francois.theron@netronome.com>
 */

#include <linux/kernel.h>
#include <linux/slab.h>

#include "nfp.h"
#include "nfp_cpp.h"
#include "nfp_nffw.h"
#include "nfp6000/nfp6000.h"

/* Init-CSR owner IDs for firmware map to firmware IDs which start at 4.
 * Lower IDs are reserved for target and loader IDs.
 */
#define NFFW_FWID_EXT   3 /* For active MEs that we didn't load. */
#define NFFW_FWID_BASE  4

#define NFFW_FWID_ALL   255

/**
 * NFFW_INFO_VERSION history:
 * 0: This was never actually used (before versioning), but it refers to
 *    the previous struct which had FWINFO_CNT = MEINFO_CNT = 120 that later
 *    changed to 200.
 * 1: First versioned struct, with
 *     FWINFO_CNT = 120
 *     MEINFO_CNT = 120
 * 2:  FWINFO_CNT = 200
 *     MEINFO_CNT = 200
 */
#define NFFW_INFO_VERSION_CURRENT 2

/* Enough for all current chip families */
#define NFFW_MEINFO_CNT_V1 120
#define NFFW_FWINFO_CNT_V1 120
#define NFFW_MEINFO_CNT_V2 200
#define NFFW_FWINFO_CNT_V2 200

/* Work in 32-bit words to make cross-platform endianness easier to handle */

/** nfp.nffw meinfo **/
struct nffw_meinfo {
	__le32 ctxmask__fwid__meid;
};

struct nffw_fwinfo {
	__le32 loaded__mu_da__mip_off_hi;
	__le32 mip_cppid; /* 0 means no MIP */
	__le32 mip_offset_lo;
};

struct nfp_nffw_info_v1 {
	struct nffw_meinfo meinfo[NFFW_MEINFO_CNT_V1];
	struct nffw_fwinfo fwinfo[NFFW_FWINFO_CNT_V1];
};

struct nfp_nffw_info_v2 {
	struct nffw_meinfo meinfo[NFFW_MEINFO_CNT_V2];
	struct nffw_fwinfo fwinfo[NFFW_FWINFO_CNT_V2];
};

/** Resource: nfp.nffw main **/
struct nfp_nffw_info_data {
	__le32 flags[2];
	union {
		struct nfp_nffw_info_v1 v1;
		struct nfp_nffw_info_v2 v2;
	} info;
};

struct nfp_nffw_info {
	struct nfp_cpp *cpp;
	struct nfp_resource *res;

	struct nfp_nffw_info_data fwinf;
};

/* flg_info_version = flags[0]<27:16>
 * This is a small version counter intended only to detect if the current
 * implementation can read the current struct. Struct changes should be very
 * rare and as such a 12-bit counter should cover large spans of time. By the
 * time it wraps around, we don't expect to have 4096 versions of this struct
 * to be in use at the same time.
 */
static u32 nffw_res_info_version_get(const struct nfp_nffw_info_data *res)
{
	return (le32_to_cpu(res->flags[0]) >> 16) & 0xfff;
}

/* flg_init = flags[0]<0> */
static u32 nffw_res_flg_init_get(const struct nfp_nffw_info_data *res)
{
	return (le32_to_cpu(res->flags[0]) >> 0) & 1;
}

/* loaded = loaded__mu_da__mip_off_hi<31:31> */
static u32 nffw_fwinfo_loaded_get(const struct nffw_fwinfo *fi)
{
	return (le32_to_cpu(fi->loaded__mu_da__mip_off_hi) >> 31) & 1;
}

/* mip_cppid = mip_cppid */
static u32 nffw_fwinfo_mip_cppid_get(const struct nffw_fwinfo *fi)
{
	return le32_to_cpu(fi->mip_cppid);
}

/* loaded = loaded__mu_da__mip_off_hi<8:8> */
static u32 nffw_fwinfo_mip_mu_da_get(const struct nffw_fwinfo *fi)
{
	return (le32_to_cpu(fi->loaded__mu_da__mip_off_hi) >> 8) & 1;
}

/* mip_offset = (loaded__mu_da__mip_off_hi<7:0> << 8) | mip_offset_lo */
static u64 nffw_fwinfo_mip_offset_get(const struct nffw_fwinfo *fi)
{
	u64 mip_off_hi = le32_to_cpu(fi->loaded__mu_da__mip_off_hi);

	return (mip_off_hi & 0xFF) << 32 | le32_to_cpu(fi->mip_offset_lo);
}

static unsigned int
nffw_res_fwinfos(struct nfp_nffw_info_data *fwinf, struct nffw_fwinfo **arr)
{
	/* For the this code, version 0 is most likely to be
	 * version 1 in this case. Since the kernel driver
	 * does not take responsibility for initialising the
	 * nfp.nffw resource, any previous code (CA firmware or
	 * userspace) that left the version 0 and did set
	 * the init flag is going to be version 1.
	 */
	switch (nffw_res_info_version_get(fwinf)) {
	case 0:
	case 1:
		*arr = &fwinf->info.v1.fwinfo[0];
		return NFFW_FWINFO_CNT_V1;
	case 2:
		*arr = &fwinf->info.v2.fwinfo[0];
		return NFFW_FWINFO_CNT_V2;
	default:
		*arr = NULL;
		return 0;
	}
}

/**
 * nfp_nffw_info_open() - Acquire the lock on the NFFW table
 * @cpp:	NFP CPP handle
 *
 * Return: pointer to nfp_nffw_info object or ERR_PTR()
 */
struct nfp_nffw_info *nfp_nffw_info_open(struct nfp_cpp *cpp)
{
	struct nfp_nffw_info_data *fwinf;
	struct nfp_nffw_info *state;
	u32 info_ver;
	int err;

	state = kzalloc(sizeof(*state), GFP_KERNEL);
	if (!state)
		return ERR_PTR(-ENOMEM);

	state->res = nfp_resource_acquire(cpp, NFP_RESOURCE_NFP_NFFW);
	if (IS_ERR(state->res))
		goto err_free;

	fwinf = &state->fwinf;

	if (sizeof(*fwinf) > nfp_resource_size(state->res))
		goto err_release;

	err = nfp_cpp_read(cpp, nfp_resource_cpp_id(state->res),
			   nfp_resource_address(state->res),
			   fwinf, sizeof(*fwinf));
	if (err < (int)sizeof(*fwinf))
		goto err_release;

	if (!nffw_res_flg_init_get(fwinf))
		goto err_release;

	info_ver = nffw_res_info_version_get(fwinf);
	if (info_ver > NFFW_INFO_VERSION_CURRENT)
		goto err_release;

	state->cpp = cpp;
	return state;

err_release:
	nfp_resource_release(state->res);
err_free:
	kfree(state);
	return ERR_PTR(-EIO);
}

/**
 * nfp_nffw_info_close() - Release the lock on the NFFW table and free state
 * @state:	NFP FW info state
 */
void nfp_nffw_info_close(struct nfp_nffw_info *state)
{
	nfp_resource_release(state->res);
	kfree(state);
}

/**
 * nfp_nffw_info_fwid_first() - Return the first firmware ID in the NFFW
 * @state:	NFP FW info state
 *
 * Return: First NFFW firmware info, NULL on failure
 */
static struct nffw_fwinfo *nfp_nffw_info_fwid_first(struct nfp_nffw_info *state)
{
	struct nffw_fwinfo *fwinfo;
	unsigned int cnt, i;

	cnt = nffw_res_fwinfos(&state->fwinf, &fwinfo);
	if (!cnt)
		return NULL;

	for (i = 0; i < cnt; i++)
		if (nffw_fwinfo_loaded_get(&fwinfo[i]))
			return &fwinfo[i];

	return NULL;
}

/**
 * nfp_nffw_info_mip_first() - Retrieve the location of the first FW's MIP
 * @state:	NFP FW info state
 * @cpp_id:	Pointer to the CPP ID of the MIP
 * @off:	Pointer to the CPP Address of the MIP
 *
 * Return: 0, or -ERRNO
 */
int nfp_nffw_info_mip_first(struct nfp_nffw_info *state, u32 *cpp_id, u64 *off)
{
	struct nffw_fwinfo *fwinfo;

	fwinfo = nfp_nffw_info_fwid_first(state);
	if (!fwinfo)
		return -EINVAL;

	*cpp_id = nffw_fwinfo_mip_cppid_get(fwinfo);
	*off = nffw_fwinfo_mip_offset_get(fwinfo);

	if (nffw_fwinfo_mip_mu_da_get(fwinfo)) {
		int locality_off = nfp_cpp_mu_locality_lsb(state->cpp);

		*off &= ~(NFP_MU_ADDR_ACCESS_TYPE_MASK << locality_off);
		*off |= NFP_MU_ADDR_ACCESS_TYPE_DIRECT << locality_off;
	}

	return 0;
}
