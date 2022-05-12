/*
 * Copyright 2021 Red Hat Inc.
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
 */
#include "ior.h"

#include <subdev/timer.h>

static int
ga102_sor_dp_links(struct nvkm_ior *sor, struct nvkm_i2c_aux *aux)
{
	struct nvkm_device *device = sor->disp->engine.subdev.device;
	const u32 soff = nv50_ior_base(sor);
	const u32 loff = nv50_sor_link(sor);
	u32 dpctrl = 0x00000000;
	u32 clksor = 0x00000000;

	switch (sor->dp.bw) {
	case 0x06: clksor |= 0x00000000; break;
	case 0x0a: clksor |= 0x00040000; break;
	case 0x14: clksor |= 0x00080000; break;
	case 0x1e: clksor |= 0x000c0000; break;
	default:
		WARN_ON(1);
		return -EINVAL;
	}

	dpctrl |= ((1 << sor->dp.nr) - 1) << 16;
	if (sor->dp.mst)
		dpctrl |= 0x40000000;
	if (sor->dp.ef)
		dpctrl |= 0x00004000;

	nvkm_mask(device, 0x612300 + soff, 0x007c0000, clksor);

	/*XXX*/
	nvkm_msec(device, 40, NVKM_DELAY);
	nvkm_mask(device, 0x612300 + soff, 0x00030000, 0x00010000);
	nvkm_mask(device, 0x61c10c + loff, 0x00000003, 0x00000001);

	nvkm_mask(device, 0x61c10c + loff, 0x401f4000, dpctrl);
	return 0;
}

static void
ga102_sor_clock(struct nvkm_ior *sor)
{
	struct nvkm_device *device = sor->disp->engine.subdev.device;
	u32 div2 = 0;
	if (sor->asy.proto == TMDS) {
		if (sor->tmds.high_speed)
			div2 = 1;
	}
	nvkm_wr32(device, 0x00ec08 + (sor->id * 0x10), 0x00000000);
	nvkm_wr32(device, 0x00ec04 + (sor->id * 0x10), div2);
}

static const struct nvkm_ior_func
ga102_sor_hda = {
	.route = {
		.get = gm200_sor_route_get,
		.set = gm200_sor_route_set,
	},
	.state = gv100_sor_state,
	.power = nv50_sor_power,
	.clock = ga102_sor_clock,
	.hdmi = {
		.ctrl = gv100_hdmi_ctrl,
		.scdc = gm200_hdmi_scdc,
	},
	.dp = {
		.lanes = { 0, 1, 2, 3 },
		.links = ga102_sor_dp_links,
		.power = g94_sor_dp_power,
		.pattern = gm107_sor_dp_pattern,
		.drive = gm200_sor_dp_drive,
		.vcpi = tu102_sor_dp_vcpi,
		.audio = gv100_sor_dp_audio,
		.audio_sym = gv100_sor_dp_audio_sym,
		.watermark = gv100_sor_dp_watermark,
	},
	.hda = {
		.hpd = gf119_hda_hpd,
		.eld = gf119_hda_eld,
		.device_entry = gv100_hda_device_entry,
	},
};

static const struct nvkm_ior_func
ga102_sor = {
	.route = {
		.get = gm200_sor_route_get,
		.set = gm200_sor_route_set,
	},
	.state = gv100_sor_state,
	.power = nv50_sor_power,
	.clock = ga102_sor_clock,
	.hdmi = {
		.ctrl = gv100_hdmi_ctrl,
		.scdc = gm200_hdmi_scdc,
	},
	.dp = {
		.lanes = { 0, 1, 2, 3 },
		.links = ga102_sor_dp_links,
		.power = g94_sor_dp_power,
		.pattern = gm107_sor_dp_pattern,
		.drive = gm200_sor_dp_drive,
		.vcpi = tu102_sor_dp_vcpi,
		.audio = gv100_sor_dp_audio,
		.audio_sym = gv100_sor_dp_audio_sym,
		.watermark = gv100_sor_dp_watermark,
	},
};

int
ga102_sor_new(struct nvkm_disp *disp, int id)
{
	struct nvkm_device *device = disp->engine.subdev.device;
	u32 hda = nvkm_rd32(device, 0x08a15c);
	if (hda & BIT(id))
		return nvkm_ior_new_(&ga102_sor_hda, disp, SOR, id);
	return nvkm_ior_new_(&ga102_sor, disp, SOR, id);
}
