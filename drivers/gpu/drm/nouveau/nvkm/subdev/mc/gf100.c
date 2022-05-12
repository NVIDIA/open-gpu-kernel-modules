/*
 * Copyright 2012 Red Hat Inc.
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
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors: Ben Skeggs
 */
#include "priv.h"

static const struct nvkm_mc_map
gf100_mc_reset[] = {
	{ 0x00020000, NVKM_ENGINE_MSPDEC },
	{ 0x00008000, NVKM_ENGINE_MSVLD },
	{ 0x00002000, NVKM_SUBDEV_PMU, 0, true },
	{ 0x00001000, NVKM_ENGINE_GR },
	{ 0x00000100, NVKM_ENGINE_FIFO },
	{ 0x00000080, NVKM_ENGINE_CE, 1 },
	{ 0x00000040, NVKM_ENGINE_CE, 0 },
	{ 0x00000002, NVKM_ENGINE_MSPPP },
	{}
};

static const struct nvkm_mc_map
gf100_mc_intr[] = {
	{ 0x04000000, NVKM_ENGINE_DISP },
	{ 0x00020000, NVKM_ENGINE_MSPDEC },
	{ 0x00008000, NVKM_ENGINE_MSVLD },
	{ 0x00001000, NVKM_ENGINE_GR },
	{ 0x00000100, NVKM_ENGINE_FIFO },
	{ 0x00000040, NVKM_ENGINE_CE, 1 },
	{ 0x00000020, NVKM_ENGINE_CE, 0 },
	{ 0x00000001, NVKM_ENGINE_MSPPP },
	{ 0x40000000, NVKM_SUBDEV_PRIVRING },
	{ 0x10000000, NVKM_SUBDEV_BUS },
	{ 0x08000000, NVKM_SUBDEV_FB },
	{ 0x02000000, NVKM_SUBDEV_LTC },
	{ 0x01000000, NVKM_SUBDEV_PMU },
	{ 0x00200000, NVKM_SUBDEV_GPIO },
	{ 0x00200000, NVKM_SUBDEV_I2C },
	{ 0x00100000, NVKM_SUBDEV_TIMER },
	{ 0x00040000, NVKM_SUBDEV_THERM },
	{ 0x00002000, NVKM_SUBDEV_FB },
	{},
};

void
gf100_mc_intr_unarm(struct nvkm_mc *mc)
{
	struct nvkm_device *device = mc->subdev.device;
	nvkm_wr32(device, 0x000140, 0x00000000);
	nvkm_wr32(device, 0x000144, 0x00000000);
	nvkm_rd32(device, 0x000140);
}

void
gf100_mc_intr_rearm(struct nvkm_mc *mc)
{
	struct nvkm_device *device = mc->subdev.device;
	nvkm_wr32(device, 0x000140, 0x00000001);
	nvkm_wr32(device, 0x000144, 0x00000001);
}

u32
gf100_mc_intr_stat(struct nvkm_mc *mc)
{
	struct nvkm_device *device = mc->subdev.device;
	u32 intr0 = nvkm_rd32(device, 0x000100);
	u32 intr1 = nvkm_rd32(device, 0x000104);
	return intr0 | intr1;
}

void
gf100_mc_intr_mask(struct nvkm_mc *mc, u32 mask, u32 stat)
{
	struct nvkm_device *device = mc->subdev.device;
	nvkm_mask(device, 0x000640, mask, stat);
	nvkm_mask(device, 0x000644, mask, stat);
}

void
gf100_mc_unk260(struct nvkm_mc *mc, u32 data)
{
	nvkm_wr32(mc->subdev.device, 0x000260, data);
}

static const struct nvkm_mc_func
gf100_mc = {
	.init = nv50_mc_init,
	.intr = gf100_mc_intr,
	.intr_unarm = gf100_mc_intr_unarm,
	.intr_rearm = gf100_mc_intr_rearm,
	.intr_mask = gf100_mc_intr_mask,
	.intr_stat = gf100_mc_intr_stat,
	.reset = gf100_mc_reset,
	.unk260 = gf100_mc_unk260,
};

int
gf100_mc_new(struct nvkm_device *device, enum nvkm_subdev_type type, int inst, struct nvkm_mc **pmc)
{
	return nvkm_mc_new_(&gf100_mc, device, type, inst, pmc);
}
