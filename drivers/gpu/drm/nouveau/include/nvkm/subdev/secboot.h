/*
 * Copyright (c) 2016, NVIDIA CORPORATION. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef __NVKM_SECURE_BOOT_H__
#define __NVKM_SECURE_BOOT_H__

#include <core/subdev.h>

enum nvkm_secboot_falcon {
	NVKM_SECBOOT_FALCON_PMU = 0,
	NVKM_SECBOOT_FALCON_RESERVED = 1,
	NVKM_SECBOOT_FALCON_FECS = 2,
	NVKM_SECBOOT_FALCON_GPCCS = 3,
	NVKM_SECBOOT_FALCON_SEC2 = 7,
	NVKM_SECBOOT_FALCON_END = 8,
	NVKM_SECBOOT_FALCON_INVALID = 0xffffffff,
};

extern const char *nvkm_secboot_falcon_name[];

/**
 * @wpr_set: whether the WPR region is currently set
*/
struct nvkm_secboot {
	const struct nvkm_secboot_func *func;
	struct nvkm_acr *acr;
	struct nvkm_subdev subdev;
	struct nvkm_falcon *boot_falcon;
	struct nvkm_falcon *halt_falcon;

	u64 wpr_addr;
	u32 wpr_size;

	bool wpr_set;
};
#define nvkm_secboot(p) container_of((p), struct nvkm_secboot, subdev)

bool nvkm_secboot_is_managed(struct nvkm_secboot *, enum nvkm_secboot_falcon);
int nvkm_secboot_reset(struct nvkm_secboot *, unsigned long);

int gm200_secboot_new(struct nvkm_device *, int, struct nvkm_secboot **);
int gm20b_secboot_new(struct nvkm_device *, int, struct nvkm_secboot **);
int gp102_secboot_new(struct nvkm_device *, int, struct nvkm_secboot **);
int gp108_secboot_new(struct nvkm_device *, int, struct nvkm_secboot **);
int gp10b_secboot_new(struct nvkm_device *, int, struct nvkm_secboot **);

#endif
