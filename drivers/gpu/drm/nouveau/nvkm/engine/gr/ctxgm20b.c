/*
 * Copyright (c) 2015, NVIDIA CORPORATION. All rights reserved.
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
#include "ctxgf100.h"

static void
gm20b_grctx_generate_main(struct gf100_gr *gr, struct gf100_grctx *info)
{
	struct nvkm_device *device = gr->base.engine.subdev.device;
	const struct gf100_grctx_func *grctx = gr->func->grctx;
	u32 idle_timeout;
	int i, tmp;

	gf100_gr_mmio(gr, gr->sw_ctx);

	gf100_gr_wait_idle(gr);

	idle_timeout = nvkm_mask(device, 0x404154, 0xffffffff, 0x00000000);

	grctx->attrib(info);

	grctx->unkn(gr);

	gf100_grctx_generate_floorsweep(gr);

	for (i = 0; i < 8; i++)
		nvkm_wr32(device, 0x4064d0 + (i * 0x04), 0x00000000);

	nvkm_wr32(device, 0x405b00, (gr->tpc_total << 8) | gr->gpc_nr);

	nvkm_wr32(device, 0x408908, nvkm_rd32(device, 0x410108) | 0x80000000);

	for (tmp = 0, i = 0; i < gr->gpc_nr; i++)
		tmp |= ((1 << gr->tpc_nr[i]) - 1) << (i * 4);
	nvkm_wr32(device, 0x4041c4, tmp);

	gm200_grctx_generate_smid_config(gr);

	gf100_gr_wait_idle(gr);

	nvkm_wr32(device, 0x404154, idle_timeout);
	gf100_gr_wait_idle(gr);

	gf100_gr_mthd(gr, gr->method);
	gf100_gr_wait_idle(gr);

	gf100_gr_icmd(gr, gr->bundle);
	grctx->pagepool(info);
	grctx->bundle(info);
}

const struct gf100_grctx_func
gm20b_grctx = {
	.main  = gm20b_grctx_generate_main,
	.unkn  = gk104_grctx_generate_unkn,
	.bundle = gm107_grctx_generate_bundle,
	.bundle_size = 0x1800,
	.bundle_min_gpm_fifo_depth = 0x182,
	.bundle_token_limit = 0x1c0,
	.pagepool = gm107_grctx_generate_pagepool,
	.pagepool_size = 0x8000,
	.attrib = gm107_grctx_generate_attrib,
	.attrib_nr_max = 0x600,
	.attrib_nr = 0x400,
	.alpha_nr_max = 0xc00,
	.alpha_nr = 0x800,
	.sm_id = gm107_grctx_generate_sm_id,
	.rop_mapping = gf117_grctx_generate_rop_mapping,
};
