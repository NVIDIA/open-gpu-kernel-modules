/*
 * Copyright 2014 Red Hat Inc.
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
#include <subdev/bios.h>
#include <subdev/bios/bit.h>
#include <subdev/bios/image.h>
#include <subdev/bios/pmu.h>

u32
nvbios_pmuTe(struct nvkm_bios *bios, u8 *ver, u8 *hdr, u8 *cnt, u8 *len)
{
	struct bit_entry bit_p;
	u32 data = 0;

	if (!bit_entry(bios, 'p', &bit_p)) {
		if (bit_p.version == 2 && bit_p.length >= 4)
			data = nvbios_rd32(bios, bit_p.offset + 0x00);
		if (data) {
			*ver = nvbios_rd08(bios, data + 0x00); /* maybe? */
			*hdr = nvbios_rd08(bios, data + 0x01);
			*len = nvbios_rd08(bios, data + 0x02);
			*cnt = nvbios_rd08(bios, data + 0x03);
		}
	}

	return data;
}

u32
nvbios_pmuEe(struct nvkm_bios *bios, int idx, u8 *ver, u8 *hdr)
{
	u8  cnt, len;
	u32 data = nvbios_pmuTe(bios, ver, hdr, &cnt, &len);
	if (data && idx < cnt) {
		data = data + *hdr + (idx * len);
		*hdr = len;
		return data;
	}
	return 0;
}

u32
nvbios_pmuEp(struct nvkm_bios *bios, int idx, u8 *ver, u8 *hdr,
	     struct nvbios_pmuE *info)
{
	u32 data = nvbios_pmuEe(bios, idx, ver, hdr);
	memset(info, 0x00, sizeof(*info));
	switch (!!data * *ver) {
	default:
		info->type = nvbios_rd08(bios, data + 0x00);
		info->data = nvbios_rd32(bios, data + 0x02);
		break;
	}
	return data;
}

bool
nvbios_pmuRm(struct nvkm_bios *bios, u8 type, struct nvbios_pmuR *info)
{
	struct nvbios_pmuE pmuE;
	u8  ver, hdr, idx = 0;
	u32 data;
	memset(info, 0x00, sizeof(*info));
	while ((data = nvbios_pmuEp(bios, idx++, &ver, &hdr, &pmuE))) {
		if (pmuE.type == type && (data = pmuE.data)) {
			info->init_addr_pmu = nvbios_rd32(bios, data + 0x08);
			info->args_addr_pmu = nvbios_rd32(bios, data + 0x0c);
			info->boot_addr     = data + 0x30;
			info->boot_addr_pmu = nvbios_rd32(bios, data + 0x10) +
					      nvbios_rd32(bios, data + 0x18);
			info->boot_size     = nvbios_rd32(bios, data + 0x1c) -
					      nvbios_rd32(bios, data + 0x18);
			info->code_addr     = info->boot_addr + info->boot_size;
			info->code_addr_pmu = info->boot_addr_pmu +
					      info->boot_size;
			info->code_size     = nvbios_rd32(bios, data + 0x20);
			info->data_addr     = data + 0x30 +
					      nvbios_rd32(bios, data + 0x24);
			info->data_addr_pmu = nvbios_rd32(bios, data + 0x28);
			info->data_size     = nvbios_rd32(bios, data + 0x2c);
			return true;
		}
	}
	return false;
}
