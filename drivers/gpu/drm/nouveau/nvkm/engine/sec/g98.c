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
#include <engine/sec.h>
#include <engine/fifo.h>
#include "fuc/g98.fuc0s.h"

#include <core/client.h>
#include <core/enum.h>
#include <core/gpuobj.h>

#include <nvif/class.h>

static const struct nvkm_enum g98_sec_isr_error_name[] = {
	{ 0x0000, "ILLEGAL_MTHD" },
	{ 0x0001, "INVALID_BITFIELD" },
	{ 0x0002, "INVALID_ENUM" },
	{ 0x0003, "QUERY" },
	{}
};

static void
g98_sec_intr(struct nvkm_falcon *sec, struct nvkm_fifo_chan *chan)
{
	struct nvkm_subdev *subdev = &sec->engine.subdev;
	struct nvkm_device *device = subdev->device;
	u32 ssta = nvkm_rd32(device, 0x087040) & 0x0000ffff;
	u32 addr = nvkm_rd32(device, 0x087040) >> 16;
	u32 mthd = (addr & 0x07ff) << 2;
	u32 subc = (addr & 0x3800) >> 11;
	u32 data = nvkm_rd32(device, 0x087044);
	const struct nvkm_enum *en =
		nvkm_enum_find(g98_sec_isr_error_name, ssta);

	nvkm_error(subdev, "DISPATCH_ERROR %04x [%s] ch %d [%010llx %s] "
			   "subc %d mthd %04x data %08x\n", ssta,
		   en ? en->name : "UNKNOWN", chan ? chan->chid : -1,
		   chan ? chan->inst->addr : 0,
		   chan ? chan->object.client->name : "unknown",
		   subc, mthd, data);
}

static const struct nvkm_falcon_func
g98_sec = {
	.code.data = g98_sec_code,
	.code.size = sizeof(g98_sec_code),
	.data.data = g98_sec_data,
	.data.size = sizeof(g98_sec_data),
	.intr = g98_sec_intr,
	.sclass = {
		{ -1, -1, G98_SEC },
		{}
	}
};

int
g98_sec_new(struct nvkm_device *device, enum nvkm_subdev_type type, int inst,
	    struct nvkm_engine **pengine)
{
	return nvkm_falcon_new_(&g98_sec, device, type, inst, true, 0x087000, pengine);
}
