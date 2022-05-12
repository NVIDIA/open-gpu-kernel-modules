/*
 * Copyright 2013 Red Hat Inc.
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
 * Authors: Ben Skeggs <bskeggs@redhat.com>
 */
#include "gf100.h"
#include "ctxgf100.h"

#include <subdev/bios.h>
#include <subdev/bios/bit.h>
#include <subdev/bios/init.h>
#include <subdev/bios/P0260.h>
#include <subdev/fb.h>

#include <nvif/class.h>

/*******************************************************************************
 * PGRAPH register lists
 ******************************************************************************/

static const struct gf100_gr_init
gm107_gr_init_main_0[] = {
	{ 0x40880c,   1, 0x04, 0x00000000 },
	{ 0x408910,   1, 0x04, 0x00000000 },
	{ 0x408984,   1, 0x04, 0x00000000 },
	{ 0x41a8a0,   1, 0x04, 0x00000000 },
	{ 0x400080,   1, 0x04, 0x003003c2 },
	{ 0x400088,   1, 0x04, 0x0001bfe7 },
	{ 0x40008c,   1, 0x04, 0x00060000 },
	{ 0x400090,   1, 0x04, 0x00000030 },
	{ 0x40013c,   1, 0x04, 0x003901f3 },
	{ 0x400140,   1, 0x04, 0x00000100 },
	{ 0x400144,   1, 0x04, 0x00000000 },
	{ 0x400148,   1, 0x04, 0x00000110 },
	{ 0x400138,   1, 0x04, 0x00000000 },
	{ 0x400130,   2, 0x04, 0x00000000 },
	{ 0x400124,   1, 0x04, 0x00000002 },
	{}
};

static const struct gf100_gr_init
gm107_gr_init_ds_0[] = {
	{ 0x405844,   1, 0x04, 0x00ffffff },
	{ 0x405850,   1, 0x04, 0x00000000 },
	{ 0x405900,   1, 0x04, 0x00000000 },
	{ 0x405908,   1, 0x04, 0x00000000 },
	{}
};

const struct gf100_gr_init
gm107_gr_init_scc_0[] = {
	{ 0x40803c,   1, 0x04, 0x00000010 },
	{}
};

static const struct gf100_gr_init
gm107_gr_init_sked_0[] = {
	{ 0x407010,   1, 0x04, 0x00000000 },
	{ 0x407040,   1, 0x04, 0x40440424 },
	{ 0x407048,   1, 0x04, 0x0000000a },
	{}
};

const struct gf100_gr_init
gm107_gr_init_prop_0[] = {
	{ 0x418408,   1, 0x04, 0x00000000 },
	{ 0x4184a0,   1, 0x04, 0x00000000 },
	{}
};

const struct gf100_gr_init
gm107_gr_init_setup_1[] = {
	{ 0x4188c8,   2, 0x04, 0x00000000 },
	{ 0x4188d0,   1, 0x04, 0x00010000 },
	{ 0x4188d4,   1, 0x04, 0x00010201 },
	{}
};

const struct gf100_gr_init
gm107_gr_init_zcull_0[] = {
	{ 0x418910,   1, 0x04, 0x00010001 },
	{ 0x418914,   1, 0x04, 0x00000301 },
	{ 0x418918,   1, 0x04, 0x00800000 },
	{ 0x418930,   2, 0x04, 0x00000000 },
	{ 0x418980,   1, 0x04, 0x77777770 },
	{ 0x418984,   3, 0x04, 0x77777777 },
	{}
};

const struct gf100_gr_init
gm107_gr_init_gpc_unk_1[] = {
	{ 0x418d00,   1, 0x04, 0x00000000 },
	{ 0x418f00,   1, 0x04, 0x00000400 },
	{ 0x418f08,   1, 0x04, 0x00000000 },
	{ 0x418e08,   1, 0x04, 0x00000000 },
	{}
};

static const struct gf100_gr_init
gm107_gr_init_tpccs_0[] = {
	{ 0x419dc4,   1, 0x04, 0x00000000 },
	{ 0x419dc8,   1, 0x04, 0x00000501 },
	{ 0x419dd0,   1, 0x04, 0x00000000 },
	{ 0x419dd4,   1, 0x04, 0x00000100 },
	{ 0x419dd8,   1, 0x04, 0x00000001 },
	{ 0x419ddc,   1, 0x04, 0x00000002 },
	{ 0x419de0,   1, 0x04, 0x00000001 },
	{ 0x419d0c,   1, 0x04, 0x00000000 },
	{ 0x419d10,   1, 0x04, 0x00000014 },
	{}
};

const struct gf100_gr_init
gm107_gr_init_tex_0[] = {
	{ 0x419ab0,   1, 0x04, 0x00000000 },
	{ 0x419ab8,   1, 0x04, 0x000000e7 },
	{ 0x419abc,   1, 0x04, 0x00000000 },
	{ 0x419acc,   1, 0x04, 0x000000ff },
	{ 0x419ac0,   1, 0x04, 0x00000000 },
	{ 0x419aa8,   2, 0x04, 0x00000000 },
	{ 0x419ad0,   2, 0x04, 0x00000000 },
	{ 0x419ae0,   2, 0x04, 0x00000000 },
	{ 0x419af0,   4, 0x04, 0x00000000 },
	{}
};

static const struct gf100_gr_init
gm107_gr_init_pe_0[] = {
	{ 0x419900,   1, 0x04, 0x000000ff },
	{ 0x41980c,   1, 0x04, 0x00000010 },
	{ 0x419844,   1, 0x04, 0x00000000 },
	{ 0x419838,   1, 0x04, 0x000000ff },
	{ 0x419850,   1, 0x04, 0x00000004 },
	{ 0x419854,   2, 0x04, 0x00000000 },
	{ 0x419894,   3, 0x04, 0x00100401 },
	{}
};

const struct gf100_gr_init
gm107_gr_init_l1c_0[] = {
	{ 0x419c98,   1, 0x04, 0x00000000 },
	{ 0x419cc0,   2, 0x04, 0x00000000 },
	{}
};

static const struct gf100_gr_init
gm107_gr_init_sm_0[] = {
	{ 0x419e30,   1, 0x04, 0x000000ff },
	{ 0x419e00,   1, 0x04, 0x00000000 },
	{ 0x419ea0,   1, 0x04, 0x00000000 },
	{ 0x419ee4,   1, 0x04, 0x00000000 },
	{ 0x419ea4,   1, 0x04, 0x00000100 },
	{ 0x419ea8,   1, 0x04, 0x01000000 },
	{ 0x419ee8,   1, 0x04, 0x00000091 },
	{ 0x419eb4,   1, 0x04, 0x00000000 },
	{ 0x419ebc,   2, 0x04, 0x00000000 },
	{ 0x419edc,   1, 0x04, 0x000c1810 },
	{ 0x419ed8,   1, 0x04, 0x00000000 },
	{ 0x419ee0,   1, 0x04, 0x00000000 },
	{ 0x419f74,   1, 0x04, 0x00005155 },
	{ 0x419f80,   4, 0x04, 0x00000000 },
	{}
};

static const struct gf100_gr_init
gm107_gr_init_l1c_1[] = {
	{ 0x419ccc,   2, 0x04, 0x00000000 },
	{ 0x419c80,   1, 0x04, 0x3f006022 },
	{ 0x419c88,   1, 0x04, 0x00000000 },
	{}
};

static const struct gf100_gr_init
gm107_gr_init_pes_0[] = {
	{ 0x41be50,   1, 0x04, 0x000000ff },
	{ 0x41be04,   1, 0x04, 0x00000000 },
	{ 0x41be08,   1, 0x04, 0x00000004 },
	{ 0x41be0c,   1, 0x04, 0x00000008 },
	{ 0x41be10,   1, 0x04, 0x0e3b8bc7 },
	{ 0x41be14,   2, 0x04, 0x00000000 },
	{ 0x41be3c,   5, 0x04, 0x00100401 },
	{}
};

const struct gf100_gr_init
gm107_gr_init_wwdx_0[] = {
	{ 0x41bfd4,   1, 0x04, 0x00800000 },
	{ 0x41bfdc,   1, 0x04, 0x00000000 },
	{}
};

const struct gf100_gr_init
gm107_gr_init_cbm_0[] = {
	{ 0x41becc,   1, 0x04, 0x00000000 },
	{}
};

static const struct gf100_gr_init
gm107_gr_init_be_0[] = {
	{ 0x408890,   1, 0x04, 0x000000ff },
	{ 0x408850,   1, 0x04, 0x00000004 },
	{ 0x408878,   1, 0x04, 0x00c81603 },
	{ 0x40887c,   1, 0x04, 0x80543432 },
	{ 0x408880,   1, 0x04, 0x0010581e },
	{ 0x408884,   1, 0x04, 0x00001205 },
	{ 0x408974,   1, 0x04, 0x000000ff },
	{ 0x408914,   8, 0x04, 0x00000000 },
	{ 0x408950,   1, 0x04, 0x00000000 },
	{ 0x408954,   1, 0x04, 0x0000ffff },
	{ 0x408958,   1, 0x04, 0x00000034 },
	{ 0x40895c,   1, 0x04, 0x8531a003 },
	{ 0x408960,   1, 0x04, 0x0561985a },
	{ 0x408964,   1, 0x04, 0x04e15c4f },
	{ 0x408968,   1, 0x04, 0x02808833 },
	{ 0x40896c,   1, 0x04, 0x01f02438 },
	{ 0x408970,   1, 0x04, 0x00012c00 },
	{ 0x408988,   1, 0x04, 0x08040201 },
	{ 0x40898c,   1, 0x04, 0x80402010 },
	{}
};

static const struct gf100_gr_init
gm107_gr_init_sm_1[] = {
	{ 0x419e5c,   1, 0x04, 0x00000000 },
	{ 0x419e58,   1, 0x04, 0x00000000 },
	{}
};

static const struct gf100_gr_pack
gm107_gr_pack_mmio[] = {
	{ gm107_gr_init_main_0 },
	{ gk110_gr_init_fe_0 },
	{ gf100_gr_init_pri_0 },
	{ gf100_gr_init_rstr2d_0 },
	{ gf100_gr_init_pd_0 },
	{ gm107_gr_init_ds_0 },
	{ gm107_gr_init_scc_0 },
	{ gm107_gr_init_sked_0 },
	{ gk110_gr_init_cwd_0 },
	{ gm107_gr_init_prop_0 },
	{ gk208_gr_init_gpc_unk_0 },
	{ gf100_gr_init_setup_0 },
	{ gf100_gr_init_crstr_0 },
	{ gm107_gr_init_setup_1 },
	{ gm107_gr_init_zcull_0 },
	{ gf100_gr_init_gpm_0 },
	{ gm107_gr_init_gpc_unk_1 },
	{ gf100_gr_init_gcc_0 },
	{ gk104_gr_init_gpc_unk_2 },
	{ gm107_gr_init_tpccs_0 },
	{ gm107_gr_init_tex_0 },
	{ gm107_gr_init_pe_0 },
	{ gm107_gr_init_l1c_0 },
	{ gf100_gr_init_mpc_0 },
	{ gm107_gr_init_sm_0 },
	{ gm107_gr_init_l1c_1 },
	{ gm107_gr_init_pes_0 },
	{ gm107_gr_init_wwdx_0 },
	{ gm107_gr_init_cbm_0 },
	{ gm107_gr_init_be_0 },
	{ gm107_gr_init_sm_1 },
	{}
};

/*******************************************************************************
 * PGRAPH engine/subdev functions
 ******************************************************************************/

void
gm107_gr_init_400054(struct gf100_gr *gr)
{
	nvkm_wr32(gr->base.engine.subdev.device, 0x400054, 0x2c350f63);
}

void
gm107_gr_init_shader_exceptions(struct gf100_gr *gr, int gpc, int tpc)
{
	struct nvkm_device *device = gr->base.engine.subdev.device;
	nvkm_wr32(device, TPC_UNIT(gpc, tpc, 0x644), 0x00dffffe);
	nvkm_wr32(device, TPC_UNIT(gpc, tpc, 0x64c), 0x00000005);
}

void
gm107_gr_init_504430(struct gf100_gr *gr, int gpc, int tpc)
{
	struct nvkm_device *device = gr->base.engine.subdev.device;
	nvkm_wr32(device, TPC_UNIT(gpc, tpc, 0x430), 0xc0000000);
}

static void
gm107_gr_init_bios_2(struct gf100_gr *gr)
{
	struct nvkm_subdev *subdev = &gr->base.engine.subdev;
	struct nvkm_device *device = subdev->device;
	struct nvkm_bios *bios = device->bios;
	struct bit_entry bit_P;
	if (!bit_entry(bios, 'P', &bit_P) &&
	    bit_P.version == 2 && bit_P.length >= 0x2c) {
		u32 data = nvbios_rd32(bios, bit_P.offset + 0x28);
		if (data) {
			u8 ver = nvbios_rd08(bios, data + 0x00);
			u8 hdr = nvbios_rd08(bios, data + 0x01);
			if (ver == 0x20 && hdr >= 8) {
				data = nvbios_rd32(bios, data + 0x04);
				if (data) {
					u32 save = nvkm_rd32(device, 0x619444);
					nvbios_init(subdev, data);
					nvkm_wr32(device, 0x619444, save);
				}
			}
		}
	}
}

void
gm107_gr_init_bios(struct gf100_gr *gr)
{
	static const struct {
		u32 ctrl;
		u32 data;
	} regs[] = {
		{ 0x419ed8, 0x419ee0 },
		{ 0x419ad0, 0x419ad4 },
		{ 0x419ae0, 0x419ae4 },
		{ 0x419af0, 0x419af4 },
		{ 0x419af8, 0x419afc },
	};
	struct nvkm_device *device = gr->base.engine.subdev.device;
	struct nvkm_bios *bios = device->bios;
	struct nvbios_P0260E infoE;
	struct nvbios_P0260X infoX;
	int E = -1, X;
	u8 ver, hdr;

	while (nvbios_P0260Ep(bios, ++E, &ver, &hdr, &infoE)) {
		if (X = -1, E < ARRAY_SIZE(regs)) {
			nvkm_wr32(device, regs[E].ctrl, infoE.data);
			while (nvbios_P0260Xp(bios, ++X, &ver, &hdr, &infoX))
				nvkm_wr32(device, regs[E].data, infoX.data);
		}
	}
}

static void
gm107_gr_init_gpc_mmu(struct gf100_gr *gr)
{
	struct nvkm_device *device = gr->base.engine.subdev.device;
	struct nvkm_fb *fb = device->fb;

	nvkm_wr32(device, GPC_BCAST(0x0880), 0x00000000);
	nvkm_wr32(device, GPC_BCAST(0x0890), 0x00000000);
	nvkm_wr32(device, GPC_BCAST(0x0894), 0x00000000);
	nvkm_wr32(device, GPC_BCAST(0x08b4), nvkm_memory_addr(fb->mmu_wr) >> 8);
	nvkm_wr32(device, GPC_BCAST(0x08b8), nvkm_memory_addr(fb->mmu_rd) >> 8);
}

#include "fuc/hubgm107.fuc5.h"

static struct gf100_gr_ucode
gm107_gr_fecs_ucode = {
	.code.data = gm107_grhub_code,
	.code.size = sizeof(gm107_grhub_code),
	.data.data = gm107_grhub_data,
	.data.size = sizeof(gm107_grhub_data),
};

#include "fuc/gpcgm107.fuc5.h"

static struct gf100_gr_ucode
gm107_gr_gpccs_ucode = {
	.code.data = gm107_grgpc_code,
	.code.size = sizeof(gm107_grgpc_code),
	.data.data = gm107_grgpc_data,
	.data.size = sizeof(gm107_grgpc_data),
};

static const struct gf100_gr_func
gm107_gr = {
	.oneinit_tiles = gf100_gr_oneinit_tiles,
	.oneinit_sm_id = gf100_gr_oneinit_sm_id,
	.init = gf100_gr_init,
	.init_gpc_mmu = gm107_gr_init_gpc_mmu,
	.init_bios = gm107_gr_init_bios,
	.init_vsc_stream_master = gk104_gr_init_vsc_stream_master,
	.init_zcull = gf117_gr_init_zcull,
	.init_num_active_ltcs = gf100_gr_init_num_active_ltcs,
	.init_rop_active_fbps = gk104_gr_init_rop_active_fbps,
	.init_bios_2 = gm107_gr_init_bios_2,
	.init_fecs_exceptions = gf100_gr_init_fecs_exceptions,
	.init_sked_hww_esr = gk104_gr_init_sked_hww_esr,
	.init_419cc0 = gf100_gr_init_419cc0,
	.init_ppc_exceptions = gk104_gr_init_ppc_exceptions,
	.init_tex_hww_esr = gf100_gr_init_tex_hww_esr,
	.init_504430 = gm107_gr_init_504430,
	.init_shader_exceptions = gm107_gr_init_shader_exceptions,
	.init_400054 = gm107_gr_init_400054,
	.trap_mp = gf100_gr_trap_mp,
	.mmio = gm107_gr_pack_mmio,
	.fecs.ucode = &gm107_gr_fecs_ucode,
	.gpccs.ucode = &gm107_gr_gpccs_ucode,
	.rops = gf100_gr_rops,
	.ppc_nr = 2,
	.grctx = &gm107_grctx,
	.zbc = &gf100_gr_zbc,
	.sclass = {
		{ -1, -1, FERMI_TWOD_A },
		{ -1, -1, KEPLER_INLINE_TO_MEMORY_B },
		{ -1, -1, MAXWELL_A, &gf100_fermi },
		{ -1, -1, MAXWELL_COMPUTE_A },
		{}
	}
};

static const struct gf100_gr_fwif
gm107_gr_fwif[] = {
	{ -1, gf100_gr_load, &gm107_gr },
	{ -1, gf100_gr_nofw, &gm107_gr },
	{}
};

int
gm107_gr_new(struct nvkm_device *device, enum nvkm_subdev_type type, int inst, struct nvkm_gr **pgr)
{
	return gf100_gr_new_(gm107_gr_fwif, device, type, inst, pgr);
}
