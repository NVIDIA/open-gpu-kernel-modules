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
#include <subdev/bios/bit.h>
#include <subdev/bios/mxm.h>

u16
mxm_table(struct nvkm_bios *bios, u8 *ver, u8 *hdr)
{
	struct nvkm_subdev *subdev = &bios->subdev;
	struct bit_entry x;

	if (bit_entry(bios, 'x', &x)) {
		nvkm_debug(subdev, "BIT 'x' table not present\n");
		return 0x0000;
	}

	*ver = x.version;
	*hdr = x.length;
	if (*ver != 1 || *hdr < 3) {
		nvkm_warn(subdev, "BIT 'x' table %d/%d unknown\n", *ver, *hdr);
		return 0x0000;
	}

	return x.offset;
}

/* These map MXM v2.x digital connection values to the appropriate SOR/link,
 * hopefully they're correct for all boards within the same chipset...
 *
 * MXM v3.x VBIOS are nicer and provide pointers to these tables.
 */
static u8 g84_sor_map[16] = {
	0x00, 0x12, 0x22, 0x11, 0x32, 0x31, 0x11, 0x31,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static u8 g92_sor_map[16] = {
	0x00, 0x12, 0x22, 0x11, 0x32, 0x31, 0x11, 0x31,
	0x11, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static u8 g94_sor_map[16] = {
	0x00, 0x14, 0x24, 0x11, 0x34, 0x31, 0x11, 0x31,
	0x11, 0x31, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00
};

static u8 g98_sor_map[16] = {
	0x00, 0x14, 0x12, 0x11, 0x00, 0x31, 0x11, 0x31,
	0x11, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

u8
mxm_sor_map(struct nvkm_bios *bios, u8 conn)
{
	struct nvkm_subdev *subdev = &bios->subdev;
	u8  ver, hdr;
	u16 mxm = mxm_table(bios, &ver, &hdr);
	if (mxm && hdr >= 6) {
		u16 map = nvbios_rd16(bios, mxm + 4);
		if (map) {
			ver = nvbios_rd08(bios, map);
			if (ver == 0x10 || ver == 0x11) {
				if (conn < nvbios_rd08(bios, map + 3)) {
					map += nvbios_rd08(bios, map + 1);
					map += conn;
					return nvbios_rd08(bios, map);
				}

				return 0x00;
			}

			nvkm_warn(subdev, "unknown sor map v%02x\n", ver);
		}
	}

	if (bios->version.chip == 0x84 || bios->version.chip == 0x86)
		return g84_sor_map[conn];
	if (bios->version.chip == 0x92)
		return g92_sor_map[conn];
	if (bios->version.chip == 0x94 || bios->version.chip == 0x96)
		return g94_sor_map[conn];
	if (bios->version.chip == 0x98)
		return g98_sor_map[conn];

	nvkm_warn(subdev, "missing sor map\n");
	return 0x00;
}

u8
mxm_ddc_map(struct nvkm_bios *bios, u8 port)
{
	struct nvkm_subdev *subdev = &bios->subdev;
	u8  ver, hdr;
	u16 mxm = mxm_table(bios, &ver, &hdr);
	if (mxm && hdr >= 8) {
		u16 map = nvbios_rd16(bios, mxm + 6);
		if (map) {
			ver = nvbios_rd08(bios, map);
			if (ver == 0x10) {
				if (port < nvbios_rd08(bios, map + 3)) {
					map += nvbios_rd08(bios, map + 1);
					map += port;
					return nvbios_rd08(bios, map);
				}

				return 0x00;
			}

			nvkm_warn(subdev, "unknown ddc map v%02x\n", ver);
		}
	}

	/* v2.x: directly write port as dcb i2cidx */
	return (port << 4) | port;
}
