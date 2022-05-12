/* SPDX-License-Identifier: MIT */
#ifndef __NVKM_MSPPP_PRIV_H__
#define __NVKM_MSPPP_PRIV_H__
#include <engine/msppp.h>

int nvkm_msppp_new_(const struct nvkm_falcon_func *, struct nvkm_device *, enum nvkm_subdev_type,
		    int, struct nvkm_engine **);

void g98_msppp_init(struct nvkm_falcon *);
#endif
