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
#include <subdev/bios.h>
#include <subdev/bios/dcb.h>
#include <subdev/bios/i2c.h>

u16
dcb_i2c_table(struct nvkm_bios *bios, u8 *ver, u8 *hdr, u8 *cnt, u8 *len)
{
	u16 i2c = 0x0000;
	u16 dcb = dcb_table(bios, ver, hdr, cnt, len);
	if (dcb) {
		if (*ver >= 0x15)
			i2c = nvbios_rd16(bios, dcb + 2);
		if (*ver >= 0x30)
			i2c = nvbios_rd16(bios, dcb + 4);
	}

	if (i2c && *ver >= 0x42) {
		nvkm_warn(&bios->subdev, "ccb %02x not supported\n", *ver);
		return 0x0000;
	}

	if (i2c && *ver >= 0x30) {
		*ver = nvbios_rd08(bios, i2c + 0);
		*hdr = nvbios_rd08(bios, i2c + 1);
		*cnt = nvbios_rd08(bios, i2c + 2);
		*len = nvbios_rd08(bios, i2c + 3);
	} else {
		*ver = *ver; /* use DCB version */
		*hdr = 0;
		*cnt = 16;
		*len = 4;
	}

	return i2c;
}

u16
dcb_i2c_entry(struct nvkm_bios *bios, u8 idx, u8 *ver, u8 *len)
{
	u8  hdr, cnt;
	u16 i2c = dcb_i2c_table(bios, ver, &hdr, &cnt, len);
	if (i2c && idx < cnt)
		return i2c + hdr + (idx * *len);
	return 0x0000;
}

int
dcb_i2c_parse(struct nvkm_bios *bios, u8 idx, struct dcb_i2c_entry *info)
{
	struct nvkm_subdev *subdev = &bios->subdev;
	u8  ver, len;
	u16 ent = dcb_i2c_entry(bios, idx, &ver, &len);
	if (ent) {
		if (ver >= 0x41) {
			u32 ent_value = nvbios_rd32(bios, ent);
			u8 i2c_port = (ent_value >> 0) & 0x1f;
			u8 dpaux_port = (ent_value >> 5) & 0x1f;
			/* value 0x1f means unused according to DCB 4.x spec */
			if (i2c_port == 0x1f && dpaux_port == 0x1f)
				info->type = DCB_I2C_UNUSED;
			else
				info->type = DCB_I2C_PMGR;
		} else
		if (ver >= 0x30) {
			info->type = nvbios_rd08(bios, ent + 0x03);
		} else {
			info->type = nvbios_rd08(bios, ent + 0x03) & 0x07;
			if (info->type == 0x07)
				info->type = DCB_I2C_UNUSED;
		}

		info->drive = DCB_I2C_UNUSED;
		info->sense = DCB_I2C_UNUSED;
		info->share = DCB_I2C_UNUSED;
		info->auxch = DCB_I2C_UNUSED;

		switch (info->type) {
		case DCB_I2C_NV04_BIT:
			info->drive = nvbios_rd08(bios, ent + 0);
			info->sense = nvbios_rd08(bios, ent + 1);
			return 0;
		case DCB_I2C_NV4E_BIT:
			info->drive = nvbios_rd08(bios, ent + 1);
			return 0;
		case DCB_I2C_NVIO_BIT:
			info->drive = nvbios_rd08(bios, ent + 0) & 0x0f;
			if (nvbios_rd08(bios, ent + 1) & 0x01)
				info->share = nvbios_rd08(bios, ent + 1) >> 1;
			return 0;
		case DCB_I2C_NVIO_AUX:
			info->auxch = nvbios_rd08(bios, ent + 0) & 0x0f;
			if (nvbios_rd08(bios, ent + 1) & 0x01)
					info->share = info->auxch;
			return 0;
		case DCB_I2C_PMGR:
			info->drive = (nvbios_rd16(bios, ent + 0) & 0x01f) >> 0;
			if (info->drive == 0x1f)
				info->drive = DCB_I2C_UNUSED;
			info->auxch = (nvbios_rd16(bios, ent + 0) & 0x3e0) >> 5;
			if (info->auxch == 0x1f)
				info->auxch = DCB_I2C_UNUSED;
			info->share = info->auxch;
			return 0;
		case DCB_I2C_UNUSED:
			return 0;
		default:
			nvkm_warn(subdev, "unknown i2c type %d\n", info->type);
			info->type = DCB_I2C_UNUSED;
			return 0;
		}
	}

	if (bios->bmp_offset && idx < 2) {
		/* BMP (from v4.0 has i2c info in the structure, it's in a
		 * fixed location on earlier VBIOS
		 */
		if (nvbios_rd08(bios, bios->bmp_offset + 5) < 4)
			ent = 0x0048;
		else
			ent = 0x0036 + bios->bmp_offset;

		if (idx == 0) {
			info->drive = nvbios_rd08(bios, ent + 4);
			if (!info->drive) info->drive = 0x3f;
			info->sense = nvbios_rd08(bios, ent + 5);
			if (!info->sense) info->sense = 0x3e;
		} else
		if (idx == 1) {
			info->drive = nvbios_rd08(bios, ent + 6);
			if (!info->drive) info->drive = 0x37;
			info->sense = nvbios_rd08(bios, ent + 7);
			if (!info->sense) info->sense = 0x36;
		}

		info->type  = DCB_I2C_NV04_BIT;
		info->share = DCB_I2C_UNUSED;
		return 0;
	}

	return -ENOENT;
}
