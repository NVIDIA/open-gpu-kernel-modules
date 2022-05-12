/*
 * Copyright 2008 Stuart Bennett
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
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __NOUVEAU_HW_H__
#define __NOUVEAU_HW_H__

#include "disp.h"
#include "nvreg.h"

#include <subdev/bios/pll.h>

#define MASK(field) ( \
	(0xffffffff >> (31 - ((1 ? field) - (0 ? field)))) << (0 ? field))

#define XLATE(src, srclowbit, outfield) ( \
	(((src) >> (srclowbit)) << (0 ? outfield)) & MASK(outfield))

void NVWriteVgaSeq(struct drm_device *, int head, uint8_t index, uint8_t value);
uint8_t NVReadVgaSeq(struct drm_device *, int head, uint8_t index);
void NVWriteVgaGr(struct drm_device *, int head, uint8_t index, uint8_t value);
uint8_t NVReadVgaGr(struct drm_device *, int head, uint8_t index);
void NVSetOwner(struct drm_device *, int owner);
void NVBlankScreen(struct drm_device *, int head, bool blank);
int nouveau_hw_get_pllvals(struct drm_device *, enum nvbios_pll_type plltype,
			   struct nvkm_pll_vals *pllvals);
int nouveau_hw_pllvals_to_clk(struct nvkm_pll_vals *pllvals);
int nouveau_hw_get_clock(struct drm_device *, enum nvbios_pll_type plltype);
void nouveau_hw_save_vga_fonts(struct drm_device *, bool save);
void nouveau_hw_save_state(struct drm_device *, int head,
			   struct nv04_mode_state *state);
void nouveau_hw_load_state(struct drm_device *, int head,
			   struct nv04_mode_state *state);
void nouveau_hw_load_state_palette(struct drm_device *, int head,
				   struct nv04_mode_state *state);

/* nouveau_calc.c */
extern void nouveau_calc_arb(struct drm_device *, int vclk, int bpp,
			     int *burst, int *lwm);

static inline uint32_t NVReadCRTC(struct drm_device *dev,
					int head, uint32_t reg)
{
	struct nvif_object *device = &nouveau_drm(dev)->client.device.object;
	uint32_t val;
	if (head)
		reg += NV_PCRTC0_SIZE;
	val = nvif_rd32(device, reg);
	return val;
}

static inline void NVWriteCRTC(struct drm_device *dev,
					int head, uint32_t reg, uint32_t val)
{
	struct nvif_object *device = &nouveau_drm(dev)->client.device.object;
	if (head)
		reg += NV_PCRTC0_SIZE;
	nvif_wr32(device, reg, val);
}

static inline uint32_t NVReadRAMDAC(struct drm_device *dev,
					int head, uint32_t reg)
{
	struct nvif_object *device = &nouveau_drm(dev)->client.device.object;
	uint32_t val;
	if (head)
		reg += NV_PRAMDAC0_SIZE;
	val = nvif_rd32(device, reg);
	return val;
}

static inline void NVWriteRAMDAC(struct drm_device *dev,
					int head, uint32_t reg, uint32_t val)
{
	struct nvif_object *device = &nouveau_drm(dev)->client.device.object;
	if (head)
		reg += NV_PRAMDAC0_SIZE;
	nvif_wr32(device, reg, val);
}

static inline uint8_t nv_read_tmds(struct drm_device *dev,
					int or, int dl, uint8_t address)
{
	int ramdac = (or & DCB_OUTPUT_C) >> 2;

	NVWriteRAMDAC(dev, ramdac, NV_PRAMDAC_FP_TMDS_CONTROL + dl * 8,
	NV_PRAMDAC_FP_TMDS_CONTROL_WRITE_DISABLE | address);
	return NVReadRAMDAC(dev, ramdac, NV_PRAMDAC_FP_TMDS_DATA + dl * 8);
}

static inline void nv_write_tmds(struct drm_device *dev,
					int or, int dl, uint8_t address,
					uint8_t data)
{
	int ramdac = (or & DCB_OUTPUT_C) >> 2;

	NVWriteRAMDAC(dev, ramdac, NV_PRAMDAC_FP_TMDS_DATA + dl * 8, data);
	NVWriteRAMDAC(dev, ramdac, NV_PRAMDAC_FP_TMDS_CONTROL + dl * 8, address);
}

static inline void NVWriteVgaCrtc(struct drm_device *dev,
					int head, uint8_t index, uint8_t value)
{
	struct nvif_object *device = &nouveau_drm(dev)->client.device.object;
	nvif_wr08(device, NV_PRMCIO_CRX__COLOR + head * NV_PRMCIO_SIZE, index);
	nvif_wr08(device, NV_PRMCIO_CR__COLOR + head * NV_PRMCIO_SIZE, value);
}

static inline uint8_t NVReadVgaCrtc(struct drm_device *dev,
					int head, uint8_t index)
{
	struct nvif_object *device = &nouveau_drm(dev)->client.device.object;
	uint8_t val;
	nvif_wr08(device, NV_PRMCIO_CRX__COLOR + head * NV_PRMCIO_SIZE, index);
	val = nvif_rd08(device, NV_PRMCIO_CR__COLOR + head * NV_PRMCIO_SIZE);
	return val;
}

/* CR57 and CR58 are a fun pair of regs. CR57 provides an index (0-0xf) for CR58
 * I suspect they in fact do nothing, but are merely a way to carry useful
 * per-head variables around
 *
 * Known uses:
 * CR57		CR58
 * 0x00		index to the appropriate dcb entry (or 7f for inactive)
 * 0x02		dcb entry's "or" value (or 00 for inactive)
 * 0x03		bit0 set for dual link (LVDS, possibly elsewhere too)
 * 0x08 or 0x09	pxclk in MHz
 * 0x0f		laptop panel info -	low nibble for PEXTDEV_BOOT_0 strap
 * 					high nibble for xlat strap value
 */

static inline void
NVWriteVgaCrtc5758(struct drm_device *dev, int head, uint8_t index, uint8_t value)
{
	NVWriteVgaCrtc(dev, head, NV_CIO_CRE_57, index);
	NVWriteVgaCrtc(dev, head, NV_CIO_CRE_58, value);
}

static inline uint8_t NVReadVgaCrtc5758(struct drm_device *dev, int head, uint8_t index)
{
	NVWriteVgaCrtc(dev, head, NV_CIO_CRE_57, index);
	return NVReadVgaCrtc(dev, head, NV_CIO_CRE_58);
}

static inline uint8_t NVReadPRMVIO(struct drm_device *dev,
					int head, uint32_t reg)
{
	struct nvif_object *device = &nouveau_drm(dev)->client.device.object;
	struct nouveau_drm *drm = nouveau_drm(dev);
	uint8_t val;

	/* Only NV4x have two pvio ranges; other twoHeads cards MUST call
	 * NVSetOwner for the relevant head to be programmed */
	if (head && drm->client.device.info.family == NV_DEVICE_INFO_V0_CURIE)
		reg += NV_PRMVIO_SIZE;

	val = nvif_rd08(device, reg);
	return val;
}

static inline void NVWritePRMVIO(struct drm_device *dev,
					int head, uint32_t reg, uint8_t value)
{
	struct nvif_object *device = &nouveau_drm(dev)->client.device.object;
	struct nouveau_drm *drm = nouveau_drm(dev);

	/* Only NV4x have two pvio ranges; other twoHeads cards MUST call
	 * NVSetOwner for the relevant head to be programmed */
	if (head && drm->client.device.info.family == NV_DEVICE_INFO_V0_CURIE)
		reg += NV_PRMVIO_SIZE;

	nvif_wr08(device, reg, value);
}

static inline void NVSetEnablePalette(struct drm_device *dev, int head, bool enable)
{
	struct nvif_object *device = &nouveau_drm(dev)->client.device.object;
	nvif_rd08(device, NV_PRMCIO_INP0__COLOR + head * NV_PRMCIO_SIZE);
	nvif_wr08(device, NV_PRMCIO_ARX + head * NV_PRMCIO_SIZE, enable ? 0 : 0x20);
}

static inline bool NVGetEnablePalette(struct drm_device *dev, int head)
{
	struct nvif_object *device = &nouveau_drm(dev)->client.device.object;
	nvif_rd08(device, NV_PRMCIO_INP0__COLOR + head * NV_PRMCIO_SIZE);
	return !(nvif_rd08(device, NV_PRMCIO_ARX + head * NV_PRMCIO_SIZE) & 0x20);
}

static inline void NVWriteVgaAttr(struct drm_device *dev,
					int head, uint8_t index, uint8_t value)
{
	struct nvif_object *device = &nouveau_drm(dev)->client.device.object;
	if (NVGetEnablePalette(dev, head))
		index &= ~0x20;
	else
		index |= 0x20;

	nvif_rd08(device, NV_PRMCIO_INP0__COLOR + head * NV_PRMCIO_SIZE);
	nvif_wr08(device, NV_PRMCIO_ARX + head * NV_PRMCIO_SIZE, index);
	nvif_wr08(device, NV_PRMCIO_AR__WRITE + head * NV_PRMCIO_SIZE, value);
}

static inline uint8_t NVReadVgaAttr(struct drm_device *dev,
					int head, uint8_t index)
{
	struct nvif_object *device = &nouveau_drm(dev)->client.device.object;
	uint8_t val;
	if (NVGetEnablePalette(dev, head))
		index &= ~0x20;
	else
		index |= 0x20;

	nvif_rd08(device, NV_PRMCIO_INP0__COLOR + head * NV_PRMCIO_SIZE);
	nvif_wr08(device, NV_PRMCIO_ARX + head * NV_PRMCIO_SIZE, index);
	val = nvif_rd08(device, NV_PRMCIO_AR__READ + head * NV_PRMCIO_SIZE);
	return val;
}

static inline void NVVgaSeqReset(struct drm_device *dev, int head, bool start)
{
	NVWriteVgaSeq(dev, head, NV_VIO_SR_RESET_INDEX, start ? 0x1 : 0x3);
}

static inline void NVVgaProtect(struct drm_device *dev, int head, bool protect)
{
	uint8_t seq1 = NVReadVgaSeq(dev, head, NV_VIO_SR_CLOCK_INDEX);

	if (protect) {
		NVVgaSeqReset(dev, head, true);
		NVWriteVgaSeq(dev, head, NV_VIO_SR_CLOCK_INDEX, seq1 | 0x20);
	} else {
		/* Reenable sequencer, then turn on screen */
		NVWriteVgaSeq(dev, head, NV_VIO_SR_CLOCK_INDEX, seq1 & ~0x20);   /* reenable display */
		NVVgaSeqReset(dev, head, false);
	}
	NVSetEnablePalette(dev, head, protect);
}

static inline bool
nv_heads_tied(struct drm_device *dev)
{
	struct nvif_object *device = &nouveau_drm(dev)->client.device.object;
	struct nouveau_drm *drm = nouveau_drm(dev);

	if (drm->client.device.info.chipset == 0x11)
		return !!(nvif_rd32(device, NV_PBUS_DEBUG_1) & (1 << 28));

	return NVReadVgaCrtc(dev, 0, NV_CIO_CRE_44) & 0x4;
}

/* makes cr0-7 on the specified head read-only */
static inline bool
nv_lock_vga_crtc_base(struct drm_device *dev, int head, bool lock)
{
	uint8_t cr11 = NVReadVgaCrtc(dev, head, NV_CIO_CR_VRE_INDEX);
	bool waslocked = cr11 & 0x80;

	if (lock)
		cr11 |= 0x80;
	else
		cr11 &= ~0x80;
	NVWriteVgaCrtc(dev, head, NV_CIO_CR_VRE_INDEX, cr11);

	return waslocked;
}

static inline void
nv_lock_vga_crtc_shadow(struct drm_device *dev, int head, int lock)
{
	/* shadow lock: connects 0x60?3d? regs to "real" 0x3d? regs
	 * bit7: unlocks HDT, HBS, HBE, HRS, HRE, HEB
	 * bit6: seems to have some effect on CR09 (double scan, VBS_9)
	 * bit5: unlocks HDE
	 * bit4: unlocks VDE
	 * bit3: unlocks VDT, OVL, VRS, ?VRE?, VBS, VBE, LSR, EBR
	 * bit2: same as bit 1 of 0x60?804
	 * bit0: same as bit 0 of 0x60?804
	 */

	uint8_t cr21 = lock;

	if (lock < 0)
		/* 0xfa is generic "unlock all" mask */
		cr21 = NVReadVgaCrtc(dev, head, NV_CIO_CRE_21) | 0xfa;

	NVWriteVgaCrtc(dev, head, NV_CIO_CRE_21, cr21);
}

/* renders the extended crtc regs (cr19+) on all crtcs impervious:
 * immutable and unreadable
 */
static inline bool
NVLockVgaCrtcs(struct drm_device *dev, bool lock)
{
	struct nouveau_drm *drm = nouveau_drm(dev);
	bool waslocked = !NVReadVgaCrtc(dev, 0, NV_CIO_SR_LOCK_INDEX);

	NVWriteVgaCrtc(dev, 0, NV_CIO_SR_LOCK_INDEX,
		       lock ? NV_CIO_SR_LOCK_VALUE : NV_CIO_SR_UNLOCK_RW_VALUE);
	/* NV11 has independently lockable extended crtcs, except when tied */
	if (drm->client.device.info.chipset == 0x11 && !nv_heads_tied(dev))
		NVWriteVgaCrtc(dev, 1, NV_CIO_SR_LOCK_INDEX,
			       lock ? NV_CIO_SR_LOCK_VALUE :
				      NV_CIO_SR_UNLOCK_RW_VALUE);

	return waslocked;
}

/* nv04 cursor max dimensions of 32x32 (A1R5G5B5) */
#define NV04_CURSOR_SIZE 32
/* limit nv10 cursors to 64x64 (ARGB8) (we could go to 64x255) */
#define NV10_CURSOR_SIZE 64

static inline int nv_cursor_width(struct drm_device *dev)
{
	struct nouveau_drm *drm = nouveau_drm(dev);

	return drm->client.device.info.family >= NV_DEVICE_INFO_V0_CELSIUS ? NV10_CURSOR_SIZE : NV04_CURSOR_SIZE;
}

static inline void
nv_fix_nv40_hw_cursor(struct drm_device *dev, int head)
{
	/* on some nv40 (such as the "true" (in the NV_PFB_BOOT_0 sense) nv40,
	 * the gf6800gt) a hardware bug requires a write to PRAMDAC_CURSOR_POS
	 * for changes to the CRTC CURCTL regs to take effect, whether changing
	 * the pixmap location, or just showing/hiding the cursor
	 */
	uint32_t curpos = NVReadRAMDAC(dev, head, NV_PRAMDAC_CU_START_POS);
	NVWriteRAMDAC(dev, head, NV_PRAMDAC_CU_START_POS, curpos);
}

static inline void
nv_set_crtc_base(struct drm_device *dev, int head, uint32_t offset)
{
	struct nouveau_drm *drm = nouveau_drm(dev);

	NVWriteCRTC(dev, head, NV_PCRTC_START, offset);

	if (drm->client.device.info.family == NV_DEVICE_INFO_V0_TNT) {
		/*
		 * Hilarious, the 24th bit doesn't want to stick to
		 * PCRTC_START...
		 */
		int cre_heb = NVReadVgaCrtc(dev, head, NV_CIO_CRE_HEB__INDEX);

		NVWriteVgaCrtc(dev, head, NV_CIO_CRE_HEB__INDEX,
			       (cre_heb & ~0x40) | ((offset >> 18) & 0x40));
	}
}

static inline void
nv_show_cursor(struct drm_device *dev, int head, bool show)
{
	struct nouveau_drm *drm = nouveau_drm(dev);
	uint8_t *curctl1 =
		&nv04_display(dev)->mode_reg.crtc_reg[head].CRTC[NV_CIO_CRE_HCUR_ADDR1_INDEX];

	if (show)
		*curctl1 |= MASK(NV_CIO_CRE_HCUR_ADDR1_ENABLE);
	else
		*curctl1 &= ~MASK(NV_CIO_CRE_HCUR_ADDR1_ENABLE);
	NVWriteVgaCrtc(dev, head, NV_CIO_CRE_HCUR_ADDR1_INDEX, *curctl1);

	if (drm->client.device.info.family == NV_DEVICE_INFO_V0_CURIE)
		nv_fix_nv40_hw_cursor(dev, head);
}

static inline uint32_t
nv_pitch_align(struct drm_device *dev, uint32_t width, int bpp)
{
	struct nouveau_drm *drm = nouveau_drm(dev);
	int mask;

	if (bpp == 15)
		bpp = 16;
	if (bpp == 24)
		bpp = 8;

	/* Alignment requirements taken from the Haiku driver */
	if (drm->client.device.info.family == NV_DEVICE_INFO_V0_TNT)
		mask = 128 / bpp - 1;
	else
		mask = 512 / bpp - 1;

	return (width + mask) & ~mask;
}

#endif	/* __NOUVEAU_HW_H__ */
