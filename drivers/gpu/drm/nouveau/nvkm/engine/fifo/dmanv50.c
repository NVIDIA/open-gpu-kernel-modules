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
#include "channv50.h"

#include <core/client.h>
#include <core/ramht.h>

#include <nvif/class.h>
#include <nvif/cl506e.h>
#include <nvif/unpack.h>

static int
nv50_fifo_dma_new(struct nvkm_fifo *base, const struct nvkm_oclass *oclass,
		  void *data, u32 size, struct nvkm_object **pobject)
{
	struct nvkm_object *parent = oclass->parent;
	union {
		struct nv50_channel_dma_v0 v0;
	} *args = data;
	struct nv50_fifo *fifo = nv50_fifo(base);
	struct nv50_fifo_chan *chan;
	int ret = -ENOSYS;

	nvif_ioctl(parent, "create channel dma size %d\n", size);
	if (!(ret = nvif_unpack(ret, &data, &size, args->v0, 0, 0, false))) {
		nvif_ioctl(parent, "create channel dma vers %d vmm %llx "
				   "pushbuf %llx offset %016llx\n",
			   args->v0.version, args->v0.vmm, args->v0.pushbuf,
			   args->v0.offset);
		if (!args->v0.pushbuf)
			return -EINVAL;
	} else
		return ret;

	if (!(chan = kzalloc(sizeof(*chan), GFP_KERNEL)))
		return -ENOMEM;
	*pobject = &chan->base.object;

	ret = nv50_fifo_chan_ctor(fifo, args->v0.vmm, args->v0.pushbuf,
				  oclass, chan);
	if (ret)
		return ret;

	args->v0.chid = chan->base.chid;

	nvkm_kmap(chan->ramfc);
	nvkm_wo32(chan->ramfc, 0x08, lower_32_bits(args->v0.offset));
	nvkm_wo32(chan->ramfc, 0x0c, upper_32_bits(args->v0.offset));
	nvkm_wo32(chan->ramfc, 0x10, lower_32_bits(args->v0.offset));
	nvkm_wo32(chan->ramfc, 0x14, upper_32_bits(args->v0.offset));
	nvkm_wo32(chan->ramfc, 0x3c, 0x003f6078);
	nvkm_wo32(chan->ramfc, 0x44, 0x01003fff);
	nvkm_wo32(chan->ramfc, 0x48, chan->base.push->node->offset >> 4);
	nvkm_wo32(chan->ramfc, 0x4c, 0xffffffff);
	nvkm_wo32(chan->ramfc, 0x60, 0x7fffffff);
	nvkm_wo32(chan->ramfc, 0x78, 0x00000000);
	nvkm_wo32(chan->ramfc, 0x7c, 0x30000001);
	nvkm_wo32(chan->ramfc, 0x80, ((chan->ramht->bits - 9) << 27) |
				     (4 << 24) /* SEARCH_FULL */ |
				     (chan->ramht->gpuobj->node->offset >> 4));
	nvkm_done(chan->ramfc);
	return 0;
}

const struct nvkm_fifo_chan_oclass
nv50_fifo_dma_oclass = {
	.base.oclass = NV50_CHANNEL_DMA,
	.base.minver = 0,
	.base.maxver = 0,
	.ctor = nv50_fifo_dma_new,
};
