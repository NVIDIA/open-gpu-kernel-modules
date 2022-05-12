// SPDX-License-Identifier: GPL-2.0-only
/*
 *
 * Hardware accelerated Matrox Millennium I, II, Mystique, G100, G200, G400 and G450.
 *
 * (c) 1998-2002 Petr Vandrovec <vandrove@vc.cvut.cz>
 *
 * Portions Copyright (c) 2001 Matrox Graphics Inc.
 *
 * Version: 1.65 2002/08/14
 *
 * See matroxfb_base.c for contributors.
 *
 */


#include "matroxfb_DAC1064.h"
#include "matroxfb_misc.h"
#include "matroxfb_accel.h"
#include "g450_pll.h"
#include <linux/matroxfb.h>

#ifdef NEED_DAC1064
#define outDAC1064 matroxfb_DAC_out
#define inDAC1064 matroxfb_DAC_in

#define DAC1064_OPT_SCLK_PCI	0x00
#define DAC1064_OPT_SCLK_PLL	0x01
#define DAC1064_OPT_SCLK_EXT	0x02
#define DAC1064_OPT_SCLK_MASK	0x03
#define DAC1064_OPT_GDIV1	0x04	/* maybe it is GDIV2 on G100 ?! */
#define DAC1064_OPT_GDIV3	0x00
#define DAC1064_OPT_MDIV1	0x08
#define DAC1064_OPT_MDIV2	0x00
#define DAC1064_OPT_RESERVED	0x10

static void DAC1064_calcclock(const struct matrox_fb_info *minfo,
			      unsigned int freq, unsigned int fmax,
			      unsigned int *in, unsigned int *feed,
			      unsigned int *post)
{
	unsigned int fvco;
	unsigned int p;

	DBG(__func__)
	
	/* only for devices older than G450 */

	fvco = PLL_calcclock(minfo, freq, fmax, in, feed, &p);
	
	p = (1 << p) - 1;
	if (fvco <= 100000)
		;
	else if (fvco <= 140000)
		p |= 0x08;
	else if (fvco <= 180000)
		p |= 0x10;
	else
		p |= 0x18;
	*post = p;
}

/* they must be in POS order */
static const unsigned char MGA1064_DAC_regs[] = {
		M1064_XCURADDL, M1064_XCURADDH, M1064_XCURCTRL,
		M1064_XCURCOL0RED, M1064_XCURCOL0GREEN, M1064_XCURCOL0BLUE,
		M1064_XCURCOL1RED, M1064_XCURCOL1GREEN, M1064_XCURCOL1BLUE,
		M1064_XCURCOL2RED, M1064_XCURCOL2GREEN, M1064_XCURCOL2BLUE,
		DAC1064_XVREFCTRL, M1064_XMULCTRL, M1064_XPIXCLKCTRL, M1064_XGENCTRL,
		M1064_XMISCCTRL,
		M1064_XGENIOCTRL, M1064_XGENIODATA, M1064_XZOOMCTRL, M1064_XSENSETEST,
		M1064_XCRCBITSEL,
		M1064_XCOLKEYMASKL, M1064_XCOLKEYMASKH, M1064_XCOLKEYL, M1064_XCOLKEYH };

static const unsigned char MGA1064_DAC[] = {
		0x00, 0x00, M1064_XCURCTRL_DIS,
		0x00, 0x00, 0x00, 	/* black */
		0xFF, 0xFF, 0xFF,	/* white */
		0xFF, 0x00, 0x00,	/* red */
		0x00, 0,
		M1064_XPIXCLKCTRL_PLL_UP | M1064_XPIXCLKCTRL_EN | M1064_XPIXCLKCTRL_SRC_PLL,
		M1064_XGENCTRL_VS_0 | M1064_XGENCTRL_ALPHA_DIS | M1064_XGENCTRL_BLACK_0IRE | M1064_XGENCTRL_NO_SYNC_ON_GREEN,
		M1064_XMISCCTRL_DAC_8BIT,
		0x00, 0x00, M1064_XZOOMCTRL_1, M1064_XSENSETEST_BCOMP | M1064_XSENSETEST_GCOMP | M1064_XSENSETEST_RCOMP | M1064_XSENSETEST_PDOWN,
		0x00,
		0x00, 0x00, 0xFF, 0xFF};

static void DAC1064_setpclk(struct matrox_fb_info *minfo, unsigned long fout)
{
	unsigned int m, n, p;

	DBG(__func__)

	DAC1064_calcclock(minfo, fout, minfo->max_pixel_clock, &m, &n, &p);
	minfo->hw.DACclk[0] = m;
	minfo->hw.DACclk[1] = n;
	minfo->hw.DACclk[2] = p;
}

static void DAC1064_setmclk(struct matrox_fb_info *minfo, int oscinfo,
			    unsigned long fmem)
{
	u_int32_t mx;
	struct matrox_hw_state *hw = &minfo->hw;

	DBG(__func__)

	if (minfo->devflags.noinit) {
		/* read MCLK and give up... */
		hw->DACclk[3] = inDAC1064(minfo, DAC1064_XSYSPLLM);
		hw->DACclk[4] = inDAC1064(minfo, DAC1064_XSYSPLLN);
		hw->DACclk[5] = inDAC1064(minfo, DAC1064_XSYSPLLP);
		return;
	}
	mx = hw->MXoptionReg | 0x00000004;
	pci_write_config_dword(minfo->pcidev, PCI_OPTION_REG, mx);
	mx &= ~0x000000BB;
	if (oscinfo & DAC1064_OPT_GDIV1)
		mx |= 0x00000008;
	if (oscinfo & DAC1064_OPT_MDIV1)
		mx |= 0x00000010;
	if (oscinfo & DAC1064_OPT_RESERVED)
		mx |= 0x00000080;
	if ((oscinfo & DAC1064_OPT_SCLK_MASK) == DAC1064_OPT_SCLK_PLL) {
		/* select PCI clock until we have setup oscilator... */
		int clk;
		unsigned int m, n, p;

		/* powerup system PLL, select PCI clock */
		mx |= 0x00000020;
		pci_write_config_dword(minfo->pcidev, PCI_OPTION_REG, mx);
		mx &= ~0x00000004;
		pci_write_config_dword(minfo->pcidev, PCI_OPTION_REG, mx);

		/* !!! you must not access device if MCLK is not running !!!
		   Doing so cause immediate PCI lockup :-( Maybe they should
		   generate ABORT or I/O (parity...) error and Linux should
		   recover from this... (kill driver/process). But world is not
		   perfect... */
		/* (bit 2 of PCI_OPTION_REG must be 0... and bits 0,1 must not
		   select PLL... because of PLL can be stopped at this time) */
		DAC1064_calcclock(minfo, fmem, minfo->max_pixel_clock, &m, &n, &p);
		outDAC1064(minfo, DAC1064_XSYSPLLM, hw->DACclk[3] = m);
		outDAC1064(minfo, DAC1064_XSYSPLLN, hw->DACclk[4] = n);
		outDAC1064(minfo, DAC1064_XSYSPLLP, hw->DACclk[5] = p);
		for (clk = 65536; clk; --clk) {
			if (inDAC1064(minfo, DAC1064_XSYSPLLSTAT) & 0x40)
				break;
		}
		if (!clk)
			printk(KERN_ERR "matroxfb: aiee, SYSPLL not locked\n");
		/* select PLL */
		mx |= 0x00000005;
	} else {
		/* select specified system clock source */
		mx |= oscinfo & DAC1064_OPT_SCLK_MASK;
	}
	pci_write_config_dword(minfo->pcidev, PCI_OPTION_REG, mx);
	mx &= ~0x00000004;
	pci_write_config_dword(minfo->pcidev, PCI_OPTION_REG, mx);
	hw->MXoptionReg = mx;
}

#ifdef CONFIG_FB_MATROX_G
static void g450_set_plls(struct matrox_fb_info *minfo)
{
	u_int32_t c2_ctl;
	unsigned int pxc;
	struct matrox_hw_state *hw = &minfo->hw;
	int pixelmnp;
	int videomnp;
	
	c2_ctl = hw->crtc2.ctl & ~0x4007;	/* Clear PLL + enable for CRTC2 */
	c2_ctl |= 0x0001;			/* Enable CRTC2 */
	hw->DACreg[POS1064_XPWRCTRL] &= ~0x02;	/* Stop VIDEO PLL */
	pixelmnp = minfo->crtc1.mnp;
	videomnp = minfo->crtc2.mnp;
	if (videomnp < 0) {
		c2_ctl &= ~0x0001;			/* Disable CRTC2 */
		hw->DACreg[POS1064_XPWRCTRL] &= ~0x10;	/* Powerdown CRTC2 */
	} else if (minfo->crtc2.pixclock == minfo->features.pll.ref_freq) {
		c2_ctl |=  0x4002;	/* Use reference directly */
	} else if (videomnp == pixelmnp) {
		c2_ctl |=  0x0004;	/* Use pixel PLL */
	} else {
		if (0 == ((videomnp ^ pixelmnp) & 0xFFFFFF00)) {
			/* PIXEL and VIDEO PLL must not use same frequency. We modify N
			   of PIXEL PLL in such case because of VIDEO PLL may be source
			   of TVO clocks, and chroma subcarrier is derived from its
			   pixel clocks */
			pixelmnp += 0x000100;
		}
		c2_ctl |=  0x0006;	/* Use video PLL */
		hw->DACreg[POS1064_XPWRCTRL] |= 0x02;
		
		outDAC1064(minfo, M1064_XPWRCTRL, hw->DACreg[POS1064_XPWRCTRL]);
		matroxfb_g450_setpll_cond(minfo, videomnp, M_VIDEO_PLL);
	}

	hw->DACreg[POS1064_XPIXCLKCTRL] &= ~M1064_XPIXCLKCTRL_PLL_UP;
	if (pixelmnp >= 0) {
		hw->DACreg[POS1064_XPIXCLKCTRL] |= M1064_XPIXCLKCTRL_PLL_UP;
		
		outDAC1064(minfo, M1064_XPIXCLKCTRL, hw->DACreg[POS1064_XPIXCLKCTRL]);
		matroxfb_g450_setpll_cond(minfo, pixelmnp, M_PIXEL_PLL_C);
	}
	if (c2_ctl != hw->crtc2.ctl) {
		hw->crtc2.ctl = c2_ctl;
		mga_outl(0x3C10, c2_ctl);
	}

	pxc = minfo->crtc1.pixclock;
	if (pxc == 0 || minfo->outputs[2].src == MATROXFB_SRC_CRTC2) {
		pxc = minfo->crtc2.pixclock;
	}
	if (minfo->chip == MGA_G550) {
		if (pxc < 45000) {
			hw->DACreg[POS1064_XPANMODE] = 0x00;	/* 0-50 */
		} else if (pxc < 55000) {
			hw->DACreg[POS1064_XPANMODE] = 0x08;	/* 34-62 */
		} else if (pxc < 70000) {
			hw->DACreg[POS1064_XPANMODE] = 0x10;	/* 42-78 */
		} else if (pxc < 85000) {
			hw->DACreg[POS1064_XPANMODE] = 0x18;	/* 62-92 */
		} else if (pxc < 100000) {
			hw->DACreg[POS1064_XPANMODE] = 0x20;	/* 74-108 */
		} else if (pxc < 115000) {
			hw->DACreg[POS1064_XPANMODE] = 0x28;	/* 94-122 */
		} else if (pxc < 125000) {
			hw->DACreg[POS1064_XPANMODE] = 0x30;	/* 108-132 */
		} else {
			hw->DACreg[POS1064_XPANMODE] = 0x38;	/* 120-168 */
		}
	} else {
		/* G450 */
		if (pxc < 45000) {
			hw->DACreg[POS1064_XPANMODE] = 0x00;	/* 0-54 */
		} else if (pxc < 65000) {
			hw->DACreg[POS1064_XPANMODE] = 0x08;	/* 38-70 */
		} else if (pxc < 85000) {
			hw->DACreg[POS1064_XPANMODE] = 0x10;	/* 56-96 */
		} else if (pxc < 105000) {
			hw->DACreg[POS1064_XPANMODE] = 0x18;	/* 80-114 */
		} else if (pxc < 135000) {
			hw->DACreg[POS1064_XPANMODE] = 0x20;	/* 102-144 */
		} else if (pxc < 160000) {
			hw->DACreg[POS1064_XPANMODE] = 0x28;	/* 132-166 */
		} else if (pxc < 175000) {
			hw->DACreg[POS1064_XPANMODE] = 0x30;	/* 154-182 */
		} else {
			hw->DACreg[POS1064_XPANMODE] = 0x38;	/* 170-204 */
		}
	}
}
#endif

void DAC1064_global_init(struct matrox_fb_info *minfo)
{
	struct matrox_hw_state *hw = &minfo->hw;

	hw->DACreg[POS1064_XMISCCTRL] &= M1064_XMISCCTRL_DAC_WIDTHMASK;
	hw->DACreg[POS1064_XMISCCTRL] |= M1064_XMISCCTRL_LUT_EN;
	hw->DACreg[POS1064_XPIXCLKCTRL] = M1064_XPIXCLKCTRL_PLL_UP | M1064_XPIXCLKCTRL_EN | M1064_XPIXCLKCTRL_SRC_PLL;
#ifdef CONFIG_FB_MATROX_G
	if (minfo->devflags.g450dac) {
		hw->DACreg[POS1064_XPWRCTRL] = 0x1F;	/* powerup everything */
		hw->DACreg[POS1064_XOUTPUTCONN] = 0x00;	/* disable outputs */
		hw->DACreg[POS1064_XMISCCTRL] |= M1064_XMISCCTRL_DAC_EN;
		switch (minfo->outputs[0].src) {
			case MATROXFB_SRC_CRTC1:
			case MATROXFB_SRC_CRTC2:
				hw->DACreg[POS1064_XOUTPUTCONN] |= 0x01;	/* enable output; CRTC1/2 selection is in CRTC2 ctl */
				break;
			case MATROXFB_SRC_NONE:
				hw->DACreg[POS1064_XMISCCTRL] &= ~M1064_XMISCCTRL_DAC_EN;
				break;
		}
		switch (minfo->outputs[1].src) {
			case MATROXFB_SRC_CRTC1:
				hw->DACreg[POS1064_XOUTPUTCONN] |= 0x04;
				break;
			case MATROXFB_SRC_CRTC2:
				if (minfo->outputs[1].mode == MATROXFB_OUTPUT_MODE_MONITOR) {
					hw->DACreg[POS1064_XOUTPUTCONN] |= 0x08;
				} else {
					hw->DACreg[POS1064_XOUTPUTCONN] |= 0x0C;
				}
				break;
			case MATROXFB_SRC_NONE:
				hw->DACreg[POS1064_XPWRCTRL] &= ~0x01;		/* Poweroff DAC2 */
				break;
		}
		switch (minfo->outputs[2].src) {
			case MATROXFB_SRC_CRTC1:
				hw->DACreg[POS1064_XOUTPUTCONN] |= 0x20;
				break;
			case MATROXFB_SRC_CRTC2:
				hw->DACreg[POS1064_XOUTPUTCONN] |= 0x40;
				break;
			case MATROXFB_SRC_NONE:
#if 0
				/* HELP! If we boot without DFP connected to DVI, we can
				   poweroff TMDS. But if we boot with DFP connected,
				   TMDS generated clocks are used instead of ALL pixclocks
				   available... If someone knows which register
				   handles it, please reveal this secret to me... */			
				hw->DACreg[POS1064_XPWRCTRL] &= ~0x04;		/* Poweroff TMDS */
#endif				
				break;
		}
		/* Now set timming related variables... */
		g450_set_plls(minfo);
	} else
#endif
	{
		if (minfo->outputs[1].src == MATROXFB_SRC_CRTC1) {
			hw->DACreg[POS1064_XPIXCLKCTRL] = M1064_XPIXCLKCTRL_PLL_UP | M1064_XPIXCLKCTRL_EN | M1064_XPIXCLKCTRL_SRC_EXT;
			hw->DACreg[POS1064_XMISCCTRL] |= GX00_XMISCCTRL_MFC_MAFC | G400_XMISCCTRL_VDO_MAFC12;
		} else if (minfo->outputs[1].src == MATROXFB_SRC_CRTC2) {
			hw->DACreg[POS1064_XMISCCTRL] |= GX00_XMISCCTRL_MFC_MAFC | G400_XMISCCTRL_VDO_C2_MAFC12;
		} else if (minfo->outputs[2].src == MATROXFB_SRC_CRTC1)
			hw->DACreg[POS1064_XMISCCTRL] |= GX00_XMISCCTRL_MFC_PANELLINK | G400_XMISCCTRL_VDO_MAFC12;
		else
			hw->DACreg[POS1064_XMISCCTRL] |= GX00_XMISCCTRL_MFC_DIS;

		if (minfo->outputs[0].src != MATROXFB_SRC_NONE)
			hw->DACreg[POS1064_XMISCCTRL] |= M1064_XMISCCTRL_DAC_EN;
	}
}

void DAC1064_global_restore(struct matrox_fb_info *minfo)
{
	struct matrox_hw_state *hw = &minfo->hw;

	outDAC1064(minfo, M1064_XPIXCLKCTRL, hw->DACreg[POS1064_XPIXCLKCTRL]);
	outDAC1064(minfo, M1064_XMISCCTRL, hw->DACreg[POS1064_XMISCCTRL]);
	if (minfo->devflags.accelerator == FB_ACCEL_MATROX_MGAG400) {
		outDAC1064(minfo, 0x20, 0x04);
		outDAC1064(minfo, 0x1F, minfo->devflags.dfp_type);
		if (minfo->devflags.g450dac) {
			outDAC1064(minfo, M1064_XSYNCCTRL, 0xCC);
			outDAC1064(minfo, M1064_XPWRCTRL, hw->DACreg[POS1064_XPWRCTRL]);
			outDAC1064(minfo, M1064_XPANMODE, hw->DACreg[POS1064_XPANMODE]);
			outDAC1064(minfo, M1064_XOUTPUTCONN, hw->DACreg[POS1064_XOUTPUTCONN]);
		}
	}
}

static int DAC1064_init_1(struct matrox_fb_info *minfo, struct my_timming *m)
{
	struct matrox_hw_state *hw = &minfo->hw;

	DBG(__func__)

	memcpy(hw->DACreg, MGA1064_DAC, sizeof(MGA1064_DAC_regs));
	switch (minfo->fbcon.var.bits_per_pixel) {
		/* case 4: not supported by MGA1064 DAC */
		case 8:
			hw->DACreg[POS1064_XMULCTRL] = M1064_XMULCTRL_DEPTH_8BPP | M1064_XMULCTRL_GRAPHICS_PALETIZED;
			break;
		case 16:
			if (minfo->fbcon.var.green.length == 5)
				hw->DACreg[POS1064_XMULCTRL] = M1064_XMULCTRL_DEPTH_15BPP_1BPP | M1064_XMULCTRL_GRAPHICS_PALETIZED;
			else
				hw->DACreg[POS1064_XMULCTRL] = M1064_XMULCTRL_DEPTH_16BPP | M1064_XMULCTRL_GRAPHICS_PALETIZED;
			break;
		case 24:
			hw->DACreg[POS1064_XMULCTRL] = M1064_XMULCTRL_DEPTH_24BPP | M1064_XMULCTRL_GRAPHICS_PALETIZED;
			break;
		case 32:
			hw->DACreg[POS1064_XMULCTRL] = M1064_XMULCTRL_DEPTH_32BPP | M1064_XMULCTRL_GRAPHICS_PALETIZED;
			break;
		default:
			return 1;	/* unsupported depth */
	}
	hw->DACreg[POS1064_XVREFCTRL] = minfo->features.DAC1064.xvrefctrl;
	hw->DACreg[POS1064_XGENCTRL] &= ~M1064_XGENCTRL_SYNC_ON_GREEN_MASK;
	hw->DACreg[POS1064_XGENCTRL] |= (m->sync & FB_SYNC_ON_GREEN)?M1064_XGENCTRL_SYNC_ON_GREEN:M1064_XGENCTRL_NO_SYNC_ON_GREEN;
	hw->DACreg[POS1064_XCURADDL] = 0;
	hw->DACreg[POS1064_XCURADDH] = 0;

	DAC1064_global_init(minfo);
	return 0;
}

static int DAC1064_init_2(struct matrox_fb_info *minfo, struct my_timming *m)
{
	struct matrox_hw_state *hw = &minfo->hw;

	DBG(__func__)

	if (minfo->fbcon.var.bits_per_pixel > 16) {	/* 256 entries */
		int i;

		for (i = 0; i < 256; i++) {
			hw->DACpal[i * 3 + 0] = i;
			hw->DACpal[i * 3 + 1] = i;
			hw->DACpal[i * 3 + 2] = i;
		}
	} else if (minfo->fbcon.var.bits_per_pixel > 8) {
		if (minfo->fbcon.var.green.length == 5) {	/* 0..31, 128..159 */
			int i;

			for (i = 0; i < 32; i++) {
				/* with p15 == 0 */
				hw->DACpal[i * 3 + 0] = i << 3;
				hw->DACpal[i * 3 + 1] = i << 3;
				hw->DACpal[i * 3 + 2] = i << 3;
				/* with p15 == 1 */
				hw->DACpal[(i + 128) * 3 + 0] = i << 3;
				hw->DACpal[(i + 128) * 3 + 1] = i << 3;
				hw->DACpal[(i + 128) * 3 + 2] = i << 3;
			}
		} else {
			int i;

			for (i = 0; i < 64; i++) {		/* 0..63 */
				hw->DACpal[i * 3 + 0] = i << 3;
				hw->DACpal[i * 3 + 1] = i << 2;
				hw->DACpal[i * 3 + 2] = i << 3;
			}
		}
	} else {
		memset(hw->DACpal, 0, 768);
	}
	return 0;
}

static void DAC1064_restore_1(struct matrox_fb_info *minfo)
{
	struct matrox_hw_state *hw = &minfo->hw;

	CRITFLAGS

	DBG(__func__)

	CRITBEGIN

	if ((inDAC1064(minfo, DAC1064_XSYSPLLM) != hw->DACclk[3]) ||
	    (inDAC1064(minfo, DAC1064_XSYSPLLN) != hw->DACclk[4]) ||
	    (inDAC1064(minfo, DAC1064_XSYSPLLP) != hw->DACclk[5])) {
		outDAC1064(minfo, DAC1064_XSYSPLLM, hw->DACclk[3]);
		outDAC1064(minfo, DAC1064_XSYSPLLN, hw->DACclk[4]);
		outDAC1064(minfo, DAC1064_XSYSPLLP, hw->DACclk[5]);
	}
	{
		unsigned int i;

		for (i = 0; i < sizeof(MGA1064_DAC_regs); i++) {
			if ((i != POS1064_XPIXCLKCTRL) && (i != POS1064_XMISCCTRL))
				outDAC1064(minfo, MGA1064_DAC_regs[i], hw->DACreg[i]);
		}
	}

	DAC1064_global_restore(minfo);

	CRITEND
};

static void DAC1064_restore_2(struct matrox_fb_info *minfo)
{
#ifdef DEBUG
	unsigned int i;
#endif

	DBG(__func__)

#ifdef DEBUG
	dprintk(KERN_DEBUG "DAC1064regs ");
	for (i = 0; i < sizeof(MGA1064_DAC_regs); i++) {
		dprintk("R%02X=%02X ", MGA1064_DAC_regs[i], minfo->hw.DACreg[i]);
		if ((i & 0x7) == 0x7) dprintk(KERN_DEBUG "continuing... ");
	}
	dprintk(KERN_DEBUG "DAC1064clk ");
	for (i = 0; i < 6; i++)
		dprintk("C%02X=%02X ", i, minfo->hw.DACclk[i]);
	dprintk("\n");
#endif
}

static int m1064_compute(void* out, struct my_timming* m) {
#define minfo ((struct matrox_fb_info*)out)
	{
		int i;
		int tmout;
		CRITFLAGS

		DAC1064_setpclk(minfo, m->pixclock);

		CRITBEGIN

		for (i = 0; i < 3; i++)
			outDAC1064(minfo, M1064_XPIXPLLCM + i, minfo->hw.DACclk[i]);
		for (tmout = 500000; tmout; tmout--) {
			if (inDAC1064(minfo, M1064_XPIXPLLSTAT) & 0x40)
				break;
			udelay(10);
		}

		CRITEND

		if (!tmout)
			printk(KERN_ERR "matroxfb: Pixel PLL not locked after 5 secs\n");
	}
#undef minfo
	return 0;
}

static struct matrox_altout m1064 = {
	.name	 = "Primary output",
	.compute = m1064_compute,
};

#ifdef CONFIG_FB_MATROX_G
static int g450_compute(void* out, struct my_timming* m) {
#define minfo ((struct matrox_fb_info*)out)
	if (m->mnp < 0) {
		m->mnp = matroxfb_g450_setclk(minfo, m->pixclock, (m->crtc == MATROXFB_SRC_CRTC1) ? M_PIXEL_PLL_C : M_VIDEO_PLL);
		if (m->mnp >= 0) {
			m->pixclock = g450_mnp2f(minfo, m->mnp);
		}
	}
#undef minfo
	return 0;
}

static struct matrox_altout g450out = {
	.name	 = "Primary output",
	.compute = g450_compute,
};
#endif

#endif /* NEED_DAC1064 */

#ifdef CONFIG_FB_MATROX_MYSTIQUE
static int MGA1064_init(struct matrox_fb_info *minfo, struct my_timming *m)
{
	struct matrox_hw_state *hw = &minfo->hw;

	DBG(__func__)

	if (DAC1064_init_1(minfo, m)) return 1;
	if (matroxfb_vgaHWinit(minfo, m)) return 1;

	hw->MiscOutReg = 0xCB;
	if (m->sync & FB_SYNC_HOR_HIGH_ACT)
		hw->MiscOutReg &= ~0x40;
	if (m->sync & FB_SYNC_VERT_HIGH_ACT)
		hw->MiscOutReg &= ~0x80;
	if (m->sync & FB_SYNC_COMP_HIGH_ACT) /* should be only FB_SYNC_COMP */
		hw->CRTCEXT[3] |= 0x40;

	if (DAC1064_init_2(minfo, m)) return 1;
	return 0;
}
#endif

#ifdef CONFIG_FB_MATROX_G
static int MGAG100_init(struct matrox_fb_info *minfo, struct my_timming *m)
{
	struct matrox_hw_state *hw = &minfo->hw;

	DBG(__func__)

	if (DAC1064_init_1(minfo, m)) return 1;
	hw->MXoptionReg &= ~0x2000;
	if (matroxfb_vgaHWinit(minfo, m)) return 1;

	hw->MiscOutReg = 0xEF;
	if (m->sync & FB_SYNC_HOR_HIGH_ACT)
		hw->MiscOutReg &= ~0x40;
	if (m->sync & FB_SYNC_VERT_HIGH_ACT)
		hw->MiscOutReg &= ~0x80;
	if (m->sync & FB_SYNC_COMP_HIGH_ACT) /* should be only FB_SYNC_COMP */
		hw->CRTCEXT[3] |= 0x40;

	if (DAC1064_init_2(minfo, m)) return 1;
	return 0;
}
#endif	/* G */

#ifdef CONFIG_FB_MATROX_MYSTIQUE
static void MGA1064_ramdac_init(struct matrox_fb_info *minfo)
{

	DBG(__func__)

	/* minfo->features.DAC1064.vco_freq_min = 120000; */
	minfo->features.pll.vco_freq_min = 62000;
	minfo->features.pll.ref_freq	 = 14318;
	minfo->features.pll.feed_div_min = 100;
	minfo->features.pll.feed_div_max = 127;
	minfo->features.pll.in_div_min	 = 1;
	minfo->features.pll.in_div_max	 = 31;
	minfo->features.pll.post_shift_max = 3;
	minfo->features.DAC1064.xvrefctrl = DAC1064_XVREFCTRL_EXTERNAL;
	/* maybe cmdline MCLK= ?, doc says gclk=44MHz, mclk=66MHz... it was 55/83 with old values */
	DAC1064_setmclk(minfo, DAC1064_OPT_MDIV2 | DAC1064_OPT_GDIV3 | DAC1064_OPT_SCLK_PLL, 133333);
}
#endif

#ifdef CONFIG_FB_MATROX_G
/* BIOS environ */
static int x7AF4 = 0x10;	/* flags, maybe 0x10 = SDRAM, 0x00 = SGRAM??? */
				/* G100 wants 0x10, G200 SGRAM does not care... */
#if 0
static int def50 = 0;	/* reg50, & 0x0F, & 0x3000 (only 0x0000, 0x1000, 0x2000 (0x3000 disallowed and treated as 0) */
#endif

static void MGAG100_progPixClock(const struct matrox_fb_info *minfo, int flags,
				 int m, int n, int p)
{
	int reg;
	int selClk;
	int clk;

	DBG(__func__)

	outDAC1064(minfo, M1064_XPIXCLKCTRL, inDAC1064(minfo, M1064_XPIXCLKCTRL) | M1064_XPIXCLKCTRL_DIS |
		   M1064_XPIXCLKCTRL_PLL_UP);
	switch (flags & 3) {
		case 0:		reg = M1064_XPIXPLLAM; break;
		case 1:		reg = M1064_XPIXPLLBM; break;
		default:	reg = M1064_XPIXPLLCM; break;
	}
	outDAC1064(minfo, reg++, m);
	outDAC1064(minfo, reg++, n);
	outDAC1064(minfo, reg, p);
	selClk = mga_inb(M_MISC_REG_READ) & ~0xC;
	/* there should be flags & 0x03 & case 0/1/else */
	/* and we should first select source and after that we should wait for PLL */
	/* and we are waiting for PLL with oscilator disabled... Is it right? */
	switch (flags & 0x03) {
		case 0x00:	break;
		case 0x01:	selClk |= 4; break;
		default:	selClk |= 0x0C; break;
	}
	mga_outb(M_MISC_REG, selClk);
	for (clk = 500000; clk; clk--) {
		if (inDAC1064(minfo, M1064_XPIXPLLSTAT) & 0x40)
			break;
		udelay(10);
	}
	if (!clk)
		printk(KERN_ERR "matroxfb: Pixel PLL%c not locked after usual time\n", (reg-M1064_XPIXPLLAM-2)/4 + 'A');
	selClk = inDAC1064(minfo, M1064_XPIXCLKCTRL) & ~M1064_XPIXCLKCTRL_SRC_MASK;
	switch (flags & 0x0C) {
		case 0x00:	selClk |= M1064_XPIXCLKCTRL_SRC_PCI; break;
		case 0x04:	selClk |= M1064_XPIXCLKCTRL_SRC_PLL; break;
		default:	selClk |= M1064_XPIXCLKCTRL_SRC_EXT; break;
	}
	outDAC1064(minfo, M1064_XPIXCLKCTRL, selClk);
	outDAC1064(minfo, M1064_XPIXCLKCTRL, inDAC1064(minfo, M1064_XPIXCLKCTRL) & ~M1064_XPIXCLKCTRL_DIS);
}

static void MGAG100_setPixClock(const struct matrox_fb_info *minfo, int flags,
				int freq)
{
	unsigned int m, n, p;

	DBG(__func__)

	DAC1064_calcclock(minfo, freq, minfo->max_pixel_clock, &m, &n, &p);
	MGAG100_progPixClock(minfo, flags, m, n, p);
}
#endif

#ifdef CONFIG_FB_MATROX_MYSTIQUE
static int MGA1064_preinit(struct matrox_fb_info *minfo)
{
	static const int vxres_mystique[] = { 512,        640, 768,  800,  832,  960,
					     1024, 1152, 1280,      1600, 1664, 1920,
					     2048,    0};
	struct matrox_hw_state *hw = &minfo->hw;

	DBG(__func__)

	/* minfo->capable.cfb4 = 0; ... preinitialized by 0 */
	minfo->capable.text = 1;
	minfo->capable.vxres = vxres_mystique;

	minfo->outputs[0].output = &m1064;
	minfo->outputs[0].src = minfo->outputs[0].default_src;
	minfo->outputs[0].data = minfo;
	minfo->outputs[0].mode = MATROXFB_OUTPUT_MODE_MONITOR;

	if (minfo->devflags.noinit)
		return 0;	/* do not modify settings */
	hw->MXoptionReg &= 0xC0000100;
	hw->MXoptionReg |= 0x00094E20;
	if (minfo->devflags.novga)
		hw->MXoptionReg &= ~0x00000100;
	if (minfo->devflags.nobios)
		hw->MXoptionReg &= ~0x40000000;
	if (minfo->devflags.nopciretry)
		hw->MXoptionReg |=  0x20000000;
	pci_write_config_dword(minfo->pcidev, PCI_OPTION_REG, hw->MXoptionReg);
	mga_setr(M_SEQ_INDEX, 0x01, 0x20);
	mga_outl(M_CTLWTST, 0x00000000);
	udelay(200);
	mga_outl(M_MACCESS, 0x00008000);
	udelay(100);
	mga_outl(M_MACCESS, 0x0000C000);
	return 0;
}

static void MGA1064_reset(struct matrox_fb_info *minfo)
{

	DBG(__func__);

	MGA1064_ramdac_init(minfo);
}
#endif

#ifdef CONFIG_FB_MATROX_G
static void g450_mclk_init(struct matrox_fb_info *minfo)
{
	/* switch all clocks to PCI source */
	pci_write_config_dword(minfo->pcidev, PCI_OPTION_REG, minfo->hw.MXoptionReg | 4);
	pci_write_config_dword(minfo->pcidev, PCI_OPTION3_REG, minfo->values.reg.opt3 & ~0x00300C03);
	pci_write_config_dword(minfo->pcidev, PCI_OPTION_REG, minfo->hw.MXoptionReg);

	if (((minfo->values.reg.opt3 & 0x000003) == 0x000003) ||
	    ((minfo->values.reg.opt3 & 0x000C00) == 0x000C00) ||
	    ((minfo->values.reg.opt3 & 0x300000) == 0x300000)) {
		matroxfb_g450_setclk(minfo, minfo->values.pll.video, M_VIDEO_PLL);
	} else {
		unsigned long flags;
		unsigned int pwr;
		
		matroxfb_DAC_lock_irqsave(flags);
		pwr = inDAC1064(minfo, M1064_XPWRCTRL) & ~0x02;
		outDAC1064(minfo, M1064_XPWRCTRL, pwr);
		matroxfb_DAC_unlock_irqrestore(flags);
	}
	matroxfb_g450_setclk(minfo, minfo->values.pll.system, M_SYSTEM_PLL);
	
	/* switch clocks to their real PLL source(s) */
	pci_write_config_dword(minfo->pcidev, PCI_OPTION_REG, minfo->hw.MXoptionReg | 4);
	pci_write_config_dword(minfo->pcidev, PCI_OPTION3_REG, minfo->values.reg.opt3);
	pci_write_config_dword(minfo->pcidev, PCI_OPTION_REG, minfo->hw.MXoptionReg);

}

static void g450_memory_init(struct matrox_fb_info *minfo)
{
	/* disable memory refresh */
	minfo->hw.MXoptionReg &= ~0x001F8000;
	pci_write_config_dword(minfo->pcidev, PCI_OPTION_REG, minfo->hw.MXoptionReg);
	
	/* set memory interface parameters */
	minfo->hw.MXoptionReg &= ~0x00207E00;
	minfo->hw.MXoptionReg |= 0x00207E00 & minfo->values.reg.opt;
	pci_write_config_dword(minfo->pcidev, PCI_OPTION_REG, minfo->hw.MXoptionReg);
	pci_write_config_dword(minfo->pcidev, PCI_OPTION2_REG, minfo->values.reg.opt2);
	
	mga_outl(M_CTLWTST, minfo->values.reg.mctlwtst);
	
	/* first set up memory interface with disabled memory interface clocks */
	pci_write_config_dword(minfo->pcidev, PCI_MEMMISC_REG, minfo->values.reg.memmisc & ~0x80000000U);
	mga_outl(M_MEMRDBK, minfo->values.reg.memrdbk);
	mga_outl(M_MACCESS, minfo->values.reg.maccess);
	/* start memory clocks */
	pci_write_config_dword(minfo->pcidev, PCI_MEMMISC_REG, minfo->values.reg.memmisc | 0x80000000U);

	udelay(200);
	
	if (minfo->values.memory.ddr && (!minfo->values.memory.emrswen || !minfo->values.memory.dll)) {
		mga_outl(M_MEMRDBK, minfo->values.reg.memrdbk & ~0x1000);
	}
	mga_outl(M_MACCESS, minfo->values.reg.maccess | 0x8000);
	
	udelay(200);
	
	minfo->hw.MXoptionReg |= 0x001F8000 & minfo->values.reg.opt;
	pci_write_config_dword(minfo->pcidev, PCI_OPTION_REG, minfo->hw.MXoptionReg);
	
	/* value is written to memory chips only if old != new */
	mga_outl(M_PLNWT, 0);
	mga_outl(M_PLNWT, ~0);
	
	if (minfo->values.reg.mctlwtst != minfo->values.reg.mctlwtst_core) {
		mga_outl(M_CTLWTST, minfo->values.reg.mctlwtst_core);
	}
	
}

static void g450_preinit(struct matrox_fb_info *minfo)
{
	u_int32_t c2ctl;
	u_int8_t curctl;
	u_int8_t c1ctl;
	
	/* minfo->hw.MXoptionReg = minfo->values.reg.opt; */
	minfo->hw.MXoptionReg &= 0xC0000100;
	minfo->hw.MXoptionReg |= 0x00000020;
	if (minfo->devflags.novga)
		minfo->hw.MXoptionReg &= ~0x00000100;
	if (minfo->devflags.nobios)
		minfo->hw.MXoptionReg &= ~0x40000000;
	if (minfo->devflags.nopciretry)
		minfo->hw.MXoptionReg |=  0x20000000;
	minfo->hw.MXoptionReg |= minfo->values.reg.opt & 0x03400040;
	pci_write_config_dword(minfo->pcidev, PCI_OPTION_REG, minfo->hw.MXoptionReg);

	/* Init system clocks */
		
	/* stop crtc2 */
	c2ctl = mga_inl(M_C2CTL);
	mga_outl(M_C2CTL, c2ctl & ~1);
	/* stop cursor */
	curctl = inDAC1064(minfo, M1064_XCURCTRL);
	outDAC1064(minfo, M1064_XCURCTRL, 0);
	/* stop crtc1 */
	c1ctl = mga_readr(M_SEQ_INDEX, 1);
	mga_setr(M_SEQ_INDEX, 1, c1ctl | 0x20);

	g450_mclk_init(minfo);
	g450_memory_init(minfo);
	
	/* set legacy VGA clock sources for DOSEmu or VMware... */
	matroxfb_g450_setclk(minfo, 25175, M_PIXEL_PLL_A);
	matroxfb_g450_setclk(minfo, 28322, M_PIXEL_PLL_B);

	/* restore crtc1 */
	mga_setr(M_SEQ_INDEX, 1, c1ctl);
	
	/* restore cursor */
	outDAC1064(minfo, M1064_XCURCTRL, curctl);

	/* restore crtc2 */
	mga_outl(M_C2CTL, c2ctl);
	
	return;
}

static int MGAG100_preinit(struct matrox_fb_info *minfo)
{
	static const int vxres_g100[] = {  512,        640, 768,  800,  832,  960,
                                          1024, 1152, 1280,      1600, 1664, 1920,
                                          2048, 0};
	struct matrox_hw_state *hw = &minfo->hw;

        u_int32_t reg50;
#if 0
	u_int32_t q;
#endif

	DBG(__func__)

	/* there are some instabilities if in_div > 19 && vco < 61000 */
	if (minfo->devflags.g450dac) {
		minfo->features.pll.vco_freq_min = 130000;	/* my sample: >118 */
	} else {
		minfo->features.pll.vco_freq_min = 62000;
	}
	if (!minfo->features.pll.ref_freq) {
		minfo->features.pll.ref_freq	 = 27000;
	}
	minfo->features.pll.feed_div_min = 7;
	minfo->features.pll.feed_div_max = 127;
	minfo->features.pll.in_div_min	 = 1;
	minfo->features.pll.in_div_max	 = 31;
	minfo->features.pll.post_shift_max = 3;
	minfo->features.DAC1064.xvrefctrl = DAC1064_XVREFCTRL_G100_DEFAULT;
	/* minfo->capable.cfb4 = 0; ... preinitialized by 0 */
	minfo->capable.text = 1;
	minfo->capable.vxres = vxres_g100;
	minfo->capable.plnwt = minfo->devflags.accelerator == FB_ACCEL_MATROX_MGAG100
			? minfo->devflags.sgram : 1;

	if (minfo->devflags.g450dac) {
		minfo->outputs[0].output = &g450out;
	} else {
		minfo->outputs[0].output = &m1064;
	}
	minfo->outputs[0].src = minfo->outputs[0].default_src;
	minfo->outputs[0].data = minfo;
	minfo->outputs[0].mode = MATROXFB_OUTPUT_MODE_MONITOR;

	if (minfo->devflags.g450dac) {
		/* we must do this always, BIOS does not do it for us
		   and accelerator dies without it */
		mga_outl(0x1C0C, 0);
	}
	if (minfo->devflags.noinit)
		return 0;
	if (minfo->devflags.g450dac) {
		g450_preinit(minfo);
		return 0;
	}
	hw->MXoptionReg &= 0xC0000100;
	hw->MXoptionReg |= 0x00000020;
	if (minfo->devflags.novga)
		hw->MXoptionReg &= ~0x00000100;
	if (minfo->devflags.nobios)
		hw->MXoptionReg &= ~0x40000000;
	if (minfo->devflags.nopciretry)
		hw->MXoptionReg |=  0x20000000;
	pci_write_config_dword(minfo->pcidev, PCI_OPTION_REG, hw->MXoptionReg);
	DAC1064_setmclk(minfo, DAC1064_OPT_MDIV2 | DAC1064_OPT_GDIV3 | DAC1064_OPT_SCLK_PCI, 133333);

	if (minfo->devflags.accelerator == FB_ACCEL_MATROX_MGAG100) {
		pci_read_config_dword(minfo->pcidev, PCI_OPTION2_REG, &reg50);
		reg50 &= ~0x3000;
		pci_write_config_dword(minfo->pcidev, PCI_OPTION2_REG, reg50);

		hw->MXoptionReg |= 0x1080;
		pci_write_config_dword(minfo->pcidev, PCI_OPTION_REG, hw->MXoptionReg);
		mga_outl(M_CTLWTST, minfo->values.reg.mctlwtst);
		udelay(100);
		mga_outb(0x1C05, 0x00);
		mga_outb(0x1C05, 0x80);
		udelay(100);
		mga_outb(0x1C05, 0x40);
		mga_outb(0x1C05, 0xC0);
		udelay(100);
		reg50 &= ~0xFF;
		reg50 |=  0x07;
		pci_write_config_dword(minfo->pcidev, PCI_OPTION2_REG, reg50);
		/* it should help with G100 */
		mga_outb(M_GRAPHICS_INDEX, 6);
		mga_outb(M_GRAPHICS_DATA, (mga_inb(M_GRAPHICS_DATA) & 3) | 4);
		mga_setr(M_EXTVGA_INDEX, 0x03, 0x81);
		mga_setr(M_EXTVGA_INDEX, 0x04, 0x00);
		mga_writeb(minfo->video.vbase, 0x0000, 0xAA);
		mga_writeb(minfo->video.vbase, 0x0800, 0x55);
		mga_writeb(minfo->video.vbase, 0x4000, 0x55);
#if 0
		if (mga_readb(minfo->video.vbase, 0x0000) != 0xAA) {
			hw->MXoptionReg &= ~0x1000;
		}
#endif
		hw->MXoptionReg |= 0x00078020;
	} else if (minfo->devflags.accelerator == FB_ACCEL_MATROX_MGAG200) {
		pci_read_config_dword(minfo->pcidev, PCI_OPTION2_REG, &reg50);
		reg50 &= ~0x3000;
		pci_write_config_dword(minfo->pcidev, PCI_OPTION2_REG, reg50);

		if (minfo->devflags.memtype == -1)
			hw->MXoptionReg |= minfo->values.reg.opt & 0x1C00;
		else
			hw->MXoptionReg |= (minfo->devflags.memtype & 7) << 10;
		if (minfo->devflags.sgram)
			hw->MXoptionReg |= 0x4000;
		mga_outl(M_CTLWTST, minfo->values.reg.mctlwtst);
		mga_outl(M_MEMRDBK, minfo->values.reg.memrdbk);
		udelay(200);
		mga_outl(M_MACCESS, 0x00000000);
		mga_outl(M_MACCESS, 0x00008000);
		udelay(100);
		mga_outw(M_MEMRDBK, minfo->values.reg.memrdbk);
		hw->MXoptionReg |= 0x00078020;
	} else {
		pci_read_config_dword(minfo->pcidev, PCI_OPTION2_REG, &reg50);
		reg50 &= ~0x00000100;
		reg50 |=  0x00000000;
		pci_write_config_dword(minfo->pcidev, PCI_OPTION2_REG, reg50);

		if (minfo->devflags.memtype == -1)
			hw->MXoptionReg |= minfo->values.reg.opt & 0x1C00;
		else
			hw->MXoptionReg |= (minfo->devflags.memtype & 7) << 10;
		if (minfo->devflags.sgram)
			hw->MXoptionReg |= 0x4000;
		mga_outl(M_CTLWTST, minfo->values.reg.mctlwtst);
		mga_outl(M_MEMRDBK, minfo->values.reg.memrdbk);
		udelay(200);
		mga_outl(M_MACCESS, 0x00000000);
		mga_outl(M_MACCESS, 0x00008000);
		udelay(100);
		mga_outl(M_MEMRDBK, minfo->values.reg.memrdbk);
		hw->MXoptionReg |= 0x00040020;
	}
	pci_write_config_dword(minfo->pcidev, PCI_OPTION_REG, hw->MXoptionReg);
	return 0;
}

static void MGAG100_reset(struct matrox_fb_info *minfo)
{
	u_int8_t b;
	struct matrox_hw_state *hw = &minfo->hw;

	DBG(__func__)

	{
#ifdef G100_BROKEN_IBM_82351
		u_int32_t d;

		find 1014/22 (IBM/82351); /* if found and bridging Matrox, do some strange stuff */
		pci_read_config_byte(ibm, PCI_SECONDARY_BUS, &b);
		if (b == minfo->pcidev->bus->number) {
			pci_write_config_byte(ibm, PCI_COMMAND+1, 0);	/* disable back-to-back & SERR */
			pci_write_config_byte(ibm, 0x41, 0xF4);		/* ??? */
			pci_write_config_byte(ibm, PCI_IO_BASE, 0xF0);	/* ??? */
			pci_write_config_byte(ibm, PCI_IO_LIMIT, 0x00);	/* ??? */
		}
#endif
		if (!minfo->devflags.noinit) {
			if (x7AF4 & 8) {
				hw->MXoptionReg |= 0x40;	/* FIXME... */
				pci_write_config_dword(minfo->pcidev, PCI_OPTION_REG, hw->MXoptionReg);
			}
			mga_setr(M_EXTVGA_INDEX, 0x06, 0x00);
		}
	}
	if (minfo->devflags.g450dac) {
		/* either leave MCLK as is... or they were set in preinit */
		hw->DACclk[3] = inDAC1064(minfo, DAC1064_XSYSPLLM);
		hw->DACclk[4] = inDAC1064(minfo, DAC1064_XSYSPLLN);
		hw->DACclk[5] = inDAC1064(minfo, DAC1064_XSYSPLLP);
	} else {
		DAC1064_setmclk(minfo, DAC1064_OPT_RESERVED | DAC1064_OPT_MDIV2 | DAC1064_OPT_GDIV1 | DAC1064_OPT_SCLK_PLL, 133333);
	}
	if (minfo->devflags.accelerator == FB_ACCEL_MATROX_MGAG400) {
		if (minfo->devflags.dfp_type == -1) {
			minfo->devflags.dfp_type = inDAC1064(minfo, 0x1F);
		}
	}
	if (minfo->devflags.noinit)
		return;
	if (minfo->devflags.g450dac) {
	} else {
		MGAG100_setPixClock(minfo, 4, 25175);
		MGAG100_setPixClock(minfo, 5, 28322);
		if (x7AF4 & 0x10) {
			b = inDAC1064(minfo, M1064_XGENIODATA) & ~1;
			outDAC1064(minfo, M1064_XGENIODATA, b);
			b = inDAC1064(minfo, M1064_XGENIOCTRL) | 1;
			outDAC1064(minfo, M1064_XGENIOCTRL, b);
		}
	}
}
#endif

#ifdef CONFIG_FB_MATROX_MYSTIQUE
static void MGA1064_restore(struct matrox_fb_info *minfo)
{
	int i;
	struct matrox_hw_state *hw = &minfo->hw;

	CRITFLAGS

	DBG(__func__)

	CRITBEGIN

	pci_write_config_dword(minfo->pcidev, PCI_OPTION_REG, hw->MXoptionReg);
	mga_outb(M_IEN, 0x00);
	mga_outb(M_CACHEFLUSH, 0x00);

	CRITEND

	DAC1064_restore_1(minfo);
	matroxfb_vgaHWrestore(minfo);
	minfo->crtc1.panpos = -1;
	for (i = 0; i < 6; i++)
		mga_setr(M_EXTVGA_INDEX, i, hw->CRTCEXT[i]);
	DAC1064_restore_2(minfo);
}
#endif

#ifdef CONFIG_FB_MATROX_G
static void MGAG100_restore(struct matrox_fb_info *minfo)
{
	int i;
	struct matrox_hw_state *hw = &minfo->hw;

	CRITFLAGS

	DBG(__func__)

	CRITBEGIN

	pci_write_config_dword(minfo->pcidev, PCI_OPTION_REG, hw->MXoptionReg);
	CRITEND

	DAC1064_restore_1(minfo);
	matroxfb_vgaHWrestore(minfo);
	if (minfo->devflags.support32MB)
		mga_setr(M_EXTVGA_INDEX, 8, hw->CRTCEXT[8]);
	minfo->crtc1.panpos = -1;
	for (i = 0; i < 6; i++)
		mga_setr(M_EXTVGA_INDEX, i, hw->CRTCEXT[i]);
	DAC1064_restore_2(minfo);
}
#endif

#ifdef CONFIG_FB_MATROX_MYSTIQUE
struct matrox_switch matrox_mystique = {
	.preinit	= MGA1064_preinit,
	.reset		= MGA1064_reset,
	.init		= MGA1064_init,
	.restore	= MGA1064_restore,
};
EXPORT_SYMBOL(matrox_mystique);
#endif

#ifdef CONFIG_FB_MATROX_G
struct matrox_switch matrox_G100 = {
	.preinit	= MGAG100_preinit,
	.reset		= MGAG100_reset,
	.init		= MGAG100_init,
	.restore	= MGAG100_restore,
};
EXPORT_SYMBOL(matrox_G100);
#endif

#ifdef NEED_DAC1064
EXPORT_SYMBOL(DAC1064_global_init);
EXPORT_SYMBOL(DAC1064_global_restore);
#endif
MODULE_LICENSE("GPL");
