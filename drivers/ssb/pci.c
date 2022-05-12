/*
 * Sonics Silicon Backplane PCI-Hostbus related functions.
 *
 * Copyright (C) 2005-2006 Michael Buesch <m@bues.ch>
 * Copyright (C) 2005 Martin Langer <martin-langer@gmx.de>
 * Copyright (C) 2005 Stefano Brivio <st3@riseup.net>
 * Copyright (C) 2005 Danny van Dyk <kugelfang@gentoo.org>
 * Copyright (C) 2005 Andreas Jaggi <andreas.jaggi@waterwave.ch>
 *
 * Derived from the Broadcom 4400 device driver.
 * Copyright (C) 2002 David S. Miller (davem@redhat.com)
 * Fixed by Pekka Pietikainen (pp@ee.oulu.fi)
 * Copyright (C) 2006 Broadcom Corporation.
 *
 * Licensed under the GNU/GPL. See COPYING for details.
 */

#include "ssb_private.h"

#include <linux/ssb/ssb.h>
#include <linux/ssb/ssb_regs.h>
#include <linux/slab.h>
#include <linux/pci.h>
#include <linux/delay.h>


/* Define the following to 1 to enable a printk on each coreswitch. */
#define SSB_VERBOSE_PCICORESWITCH_DEBUG		0


/* Lowlevel coreswitching */
int ssb_pci_switch_coreidx(struct ssb_bus *bus, u8 coreidx)
{
	int err;
	int attempts = 0;
	u32 cur_core;

	while (1) {
		err = pci_write_config_dword(bus->host_pci, SSB_BAR0_WIN,
					     (coreidx * SSB_CORE_SIZE)
					     + SSB_ENUM_BASE);
		if (err)
			goto error;
		err = pci_read_config_dword(bus->host_pci, SSB_BAR0_WIN,
					    &cur_core);
		if (err)
			goto error;
		cur_core = (cur_core - SSB_ENUM_BASE)
			   / SSB_CORE_SIZE;
		if (cur_core == coreidx)
			break;

		if (attempts++ > SSB_BAR0_MAX_RETRIES)
			goto error;
		udelay(10);
	}
	return 0;
error:
	pr_err("Failed to switch to core %u\n", coreidx);
	return -ENODEV;
}

int ssb_pci_switch_core(struct ssb_bus *bus,
			struct ssb_device *dev)
{
	int err;
	unsigned long flags;

#if SSB_VERBOSE_PCICORESWITCH_DEBUG
	pr_info("Switching to %s core, index %d\n",
		ssb_core_name(dev->id.coreid), dev->core_index);
#endif

	spin_lock_irqsave(&bus->bar_lock, flags);
	err = ssb_pci_switch_coreidx(bus, dev->core_index);
	if (!err)
		bus->mapped_device = dev;
	spin_unlock_irqrestore(&bus->bar_lock, flags);

	return err;
}

/* Enable/disable the on board crystal oscillator and/or PLL. */
int ssb_pci_xtal(struct ssb_bus *bus, u32 what, int turn_on)
{
	int err;
	u32 in, out, outenable;
	u16 pci_status;

	if (bus->bustype != SSB_BUSTYPE_PCI)
		return 0;

	err = pci_read_config_dword(bus->host_pci, SSB_GPIO_IN, &in);
	if (err)
		goto err_pci;
	err = pci_read_config_dword(bus->host_pci, SSB_GPIO_OUT, &out);
	if (err)
		goto err_pci;
	err = pci_read_config_dword(bus->host_pci, SSB_GPIO_OUT_ENABLE, &outenable);
	if (err)
		goto err_pci;

	outenable |= what;

	if (turn_on) {
		/* Avoid glitching the clock if GPRS is already using it.
		 * We can't actually read the state of the PLLPD so we infer it
		 * by the value of XTAL_PU which *is* readable via gpioin.
		 */
		if (!(in & SSB_GPIO_XTAL)) {
			if (what & SSB_GPIO_XTAL) {
				/* Turn the crystal on */
				out |= SSB_GPIO_XTAL;
				if (what & SSB_GPIO_PLL)
					out |= SSB_GPIO_PLL;
				err = pci_write_config_dword(bus->host_pci, SSB_GPIO_OUT, out);
				if (err)
					goto err_pci;
				err = pci_write_config_dword(bus->host_pci, SSB_GPIO_OUT_ENABLE,
							     outenable);
				if (err)
					goto err_pci;
				msleep(1);
			}
			if (what & SSB_GPIO_PLL) {
				/* Turn the PLL on */
				out &= ~SSB_GPIO_PLL;
				err = pci_write_config_dword(bus->host_pci, SSB_GPIO_OUT, out);
				if (err)
					goto err_pci;
				msleep(5);
			}
		}

		err = pci_read_config_word(bus->host_pci, PCI_STATUS, &pci_status);
		if (err)
			goto err_pci;
		pci_status &= ~PCI_STATUS_SIG_TARGET_ABORT;
		err = pci_write_config_word(bus->host_pci, PCI_STATUS, pci_status);
		if (err)
			goto err_pci;
	} else {
		if (what & SSB_GPIO_XTAL) {
			/* Turn the crystal off */
			out &= ~SSB_GPIO_XTAL;
		}
		if (what & SSB_GPIO_PLL) {
			/* Turn the PLL off */
			out |= SSB_GPIO_PLL;
		}
		err = pci_write_config_dword(bus->host_pci, SSB_GPIO_OUT, out);
		if (err)
			goto err_pci;
		err = pci_write_config_dword(bus->host_pci, SSB_GPIO_OUT_ENABLE, outenable);
		if (err)
			goto err_pci;
	}

out:
	return err;

err_pci:
	pr_err("Error: ssb_pci_xtal() could not access PCI config space!\n");
	err = -EBUSY;
	goto out;
}

/* Get the word-offset for a SSB_SPROM_XXX define. */
#define SPOFF(offset)	((offset) / sizeof(u16))
/* Helper to extract some _offset, which is one of the SSB_SPROM_XXX defines. */
#define SPEX16(_outvar, _offset, _mask, _shift)	\
	out->_outvar = ((in[SPOFF(_offset)] & (_mask)) >> (_shift))
#define SPEX32(_outvar, _offset, _mask, _shift)	\
	out->_outvar = ((((u32)in[SPOFF((_offset)+2)] << 16 | \
			   in[SPOFF(_offset)]) & (_mask)) >> (_shift))
#define SPEX(_outvar, _offset, _mask, _shift) \
	SPEX16(_outvar, _offset, _mask, _shift)

#define SPEX_ARRAY8(_field, _offset, _mask, _shift)	\
	do {	\
		SPEX(_field[0], _offset +  0, _mask, _shift);	\
		SPEX(_field[1], _offset +  2, _mask, _shift);	\
		SPEX(_field[2], _offset +  4, _mask, _shift);	\
		SPEX(_field[3], _offset +  6, _mask, _shift);	\
		SPEX(_field[4], _offset +  8, _mask, _shift);	\
		SPEX(_field[5], _offset + 10, _mask, _shift);	\
		SPEX(_field[6], _offset + 12, _mask, _shift);	\
		SPEX(_field[7], _offset + 14, _mask, _shift);	\
	} while (0)


static inline u8 ssb_crc8(u8 crc, u8 data)
{
	/* Polynomial:   x^8 + x^7 + x^6 + x^4 + x^2 + 1   */
	static const u8 t[] = {
		0x00, 0xF7, 0xB9, 0x4E, 0x25, 0xD2, 0x9C, 0x6B,
		0x4A, 0xBD, 0xF3, 0x04, 0x6F, 0x98, 0xD6, 0x21,
		0x94, 0x63, 0x2D, 0xDA, 0xB1, 0x46, 0x08, 0xFF,
		0xDE, 0x29, 0x67, 0x90, 0xFB, 0x0C, 0x42, 0xB5,
		0x7F, 0x88, 0xC6, 0x31, 0x5A, 0xAD, 0xE3, 0x14,
		0x35, 0xC2, 0x8C, 0x7B, 0x10, 0xE7, 0xA9, 0x5E,
		0xEB, 0x1C, 0x52, 0xA5, 0xCE, 0x39, 0x77, 0x80,
		0xA1, 0x56, 0x18, 0xEF, 0x84, 0x73, 0x3D, 0xCA,
		0xFE, 0x09, 0x47, 0xB0, 0xDB, 0x2C, 0x62, 0x95,
		0xB4, 0x43, 0x0D, 0xFA, 0x91, 0x66, 0x28, 0xDF,
		0x6A, 0x9D, 0xD3, 0x24, 0x4F, 0xB8, 0xF6, 0x01,
		0x20, 0xD7, 0x99, 0x6E, 0x05, 0xF2, 0xBC, 0x4B,
		0x81, 0x76, 0x38, 0xCF, 0xA4, 0x53, 0x1D, 0xEA,
		0xCB, 0x3C, 0x72, 0x85, 0xEE, 0x19, 0x57, 0xA0,
		0x15, 0xE2, 0xAC, 0x5B, 0x30, 0xC7, 0x89, 0x7E,
		0x5F, 0xA8, 0xE6, 0x11, 0x7A, 0x8D, 0xC3, 0x34,
		0xAB, 0x5C, 0x12, 0xE5, 0x8E, 0x79, 0x37, 0xC0,
		0xE1, 0x16, 0x58, 0xAF, 0xC4, 0x33, 0x7D, 0x8A,
		0x3F, 0xC8, 0x86, 0x71, 0x1A, 0xED, 0xA3, 0x54,
		0x75, 0x82, 0xCC, 0x3B, 0x50, 0xA7, 0xE9, 0x1E,
		0xD4, 0x23, 0x6D, 0x9A, 0xF1, 0x06, 0x48, 0xBF,
		0x9E, 0x69, 0x27, 0xD0, 0xBB, 0x4C, 0x02, 0xF5,
		0x40, 0xB7, 0xF9, 0x0E, 0x65, 0x92, 0xDC, 0x2B,
		0x0A, 0xFD, 0xB3, 0x44, 0x2F, 0xD8, 0x96, 0x61,
		0x55, 0xA2, 0xEC, 0x1B, 0x70, 0x87, 0xC9, 0x3E,
		0x1F, 0xE8, 0xA6, 0x51, 0x3A, 0xCD, 0x83, 0x74,
		0xC1, 0x36, 0x78, 0x8F, 0xE4, 0x13, 0x5D, 0xAA,
		0x8B, 0x7C, 0x32, 0xC5, 0xAE, 0x59, 0x17, 0xE0,
		0x2A, 0xDD, 0x93, 0x64, 0x0F, 0xF8, 0xB6, 0x41,
		0x60, 0x97, 0xD9, 0x2E, 0x45, 0xB2, 0xFC, 0x0B,
		0xBE, 0x49, 0x07, 0xF0, 0x9B, 0x6C, 0x22, 0xD5,
		0xF4, 0x03, 0x4D, 0xBA, 0xD1, 0x26, 0x68, 0x9F,
	};
	return t[crc ^ data];
}

static void sprom_get_mac(char *mac, const u16 *in)
{
	int i;
	for (i = 0; i < 3; i++) {
		*mac++ = in[i] >> 8;
		*mac++ = in[i];
	}
}

static u8 ssb_sprom_crc(const u16 *sprom, u16 size)
{
	int word;
	u8 crc = 0xFF;

	for (word = 0; word < size - 1; word++) {
		crc = ssb_crc8(crc, sprom[word] & 0x00FF);
		crc = ssb_crc8(crc, (sprom[word] & 0xFF00) >> 8);
	}
	crc = ssb_crc8(crc, sprom[size - 1] & 0x00FF);
	crc ^= 0xFF;

	return crc;
}

static int sprom_check_crc(const u16 *sprom, size_t size)
{
	u8 crc;
	u8 expected_crc;
	u16 tmp;

	crc = ssb_sprom_crc(sprom, size);
	tmp = sprom[size - 1] & SSB_SPROM_REVISION_CRC;
	expected_crc = tmp >> SSB_SPROM_REVISION_CRC_SHIFT;
	if (crc != expected_crc)
		return -EPROTO;

	return 0;
}

static int sprom_do_read(struct ssb_bus *bus, u16 *sprom)
{
	int i;

	for (i = 0; i < bus->sprom_size; i++)
		sprom[i] = ioread16(bus->mmio + bus->sprom_offset + (i * 2));

	return 0;
}

static int sprom_do_write(struct ssb_bus *bus, const u16 *sprom)
{
	struct pci_dev *pdev = bus->host_pci;
	int i, err;
	u32 spromctl;
	u16 size = bus->sprom_size;

	pr_notice("Writing SPROM. Do NOT turn off the power! Please stand by...\n");
	err = pci_read_config_dword(pdev, SSB_SPROMCTL, &spromctl);
	if (err)
		goto err_ctlreg;
	spromctl |= SSB_SPROMCTL_WE;
	err = pci_write_config_dword(pdev, SSB_SPROMCTL, spromctl);
	if (err)
		goto err_ctlreg;
	pr_notice("[ 0%%");
	msleep(500);
	for (i = 0; i < size; i++) {
		if (i == size / 4)
			pr_cont("25%%");
		else if (i == size / 2)
			pr_cont("50%%");
		else if (i == (size * 3) / 4)
			pr_cont("75%%");
		else if (i % 2)
			pr_cont(".");
		writew(sprom[i], bus->mmio + bus->sprom_offset + (i * 2));
		msleep(20);
	}
	err = pci_read_config_dword(pdev, SSB_SPROMCTL, &spromctl);
	if (err)
		goto err_ctlreg;
	spromctl &= ~SSB_SPROMCTL_WE;
	err = pci_write_config_dword(pdev, SSB_SPROMCTL, spromctl);
	if (err)
		goto err_ctlreg;
	msleep(500);
	pr_cont("100%% ]\n");
	pr_notice("SPROM written\n");

	return 0;
err_ctlreg:
	pr_err("Could not access SPROM control register.\n");
	return err;
}

static s8 sprom_extract_antgain(u8 sprom_revision, const u16 *in, u16 offset,
				u16 mask, u16 shift)
{
	u16 v;
	u8 gain;

	v = in[SPOFF(offset)];
	gain = (v & mask) >> shift;
	if (gain == 0xFF)
		gain = 2; /* If unset use 2dBm */
	if (sprom_revision == 1) {
		/* Convert to Q5.2 */
		gain <<= 2;
	} else {
		/* Q5.2 Fractional part is stored in 0xC0 */
		gain = ((gain & 0xC0) >> 6) | ((gain & 0x3F) << 2);
	}

	return (s8)gain;
}

static void sprom_extract_r23(struct ssb_sprom *out, const u16 *in)
{
	SPEX(boardflags_hi, SSB_SPROM2_BFLHI, 0xFFFF, 0);
	SPEX(opo, SSB_SPROM2_OPO, SSB_SPROM2_OPO_VALUE, 0);
	SPEX(pa1lob0, SSB_SPROM2_PA1LOB0, 0xFFFF, 0);
	SPEX(pa1lob1, SSB_SPROM2_PA1LOB1, 0xFFFF, 0);
	SPEX(pa1lob2, SSB_SPROM2_PA1LOB2, 0xFFFF, 0);
	SPEX(pa1hib0, SSB_SPROM2_PA1HIB0, 0xFFFF, 0);
	SPEX(pa1hib1, SSB_SPROM2_PA1HIB1, 0xFFFF, 0);
	SPEX(pa1hib2, SSB_SPROM2_PA1HIB2, 0xFFFF, 0);
	SPEX(maxpwr_ah, SSB_SPROM2_MAXP_A, SSB_SPROM2_MAXP_A_HI, 0);
	SPEX(maxpwr_al, SSB_SPROM2_MAXP_A, SSB_SPROM2_MAXP_A_LO,
	     SSB_SPROM2_MAXP_A_LO_SHIFT);
}

static void sprom_extract_r123(struct ssb_sprom *out, const u16 *in)
{
	u16 loc[3];

	if (out->revision == 3)			/* rev 3 moved MAC */
		loc[0] = SSB_SPROM3_IL0MAC;
	else {
		loc[0] = SSB_SPROM1_IL0MAC;
		loc[1] = SSB_SPROM1_ET0MAC;
		loc[2] = SSB_SPROM1_ET1MAC;
	}
	sprom_get_mac(out->il0mac, &in[SPOFF(loc[0])]);
	if (out->revision < 3) { 	/* only rev 1-2 have et0, et1 */
		sprom_get_mac(out->et0mac, &in[SPOFF(loc[1])]);
		sprom_get_mac(out->et1mac, &in[SPOFF(loc[2])]);
	}
	SPEX(et0phyaddr, SSB_SPROM1_ETHPHY, SSB_SPROM1_ETHPHY_ET0A, 0);
	SPEX(et1phyaddr, SSB_SPROM1_ETHPHY, SSB_SPROM1_ETHPHY_ET1A,
	     SSB_SPROM1_ETHPHY_ET1A_SHIFT);
	SPEX(et0mdcport, SSB_SPROM1_ETHPHY, SSB_SPROM1_ETHPHY_ET0M, 14);
	SPEX(et1mdcport, SSB_SPROM1_ETHPHY, SSB_SPROM1_ETHPHY_ET1M, 15);
	SPEX(board_rev, SSB_SPROM1_BINF, SSB_SPROM1_BINF_BREV, 0);
	SPEX(board_type, SSB_SPROM1_SPID, 0xFFFF, 0);
	if (out->revision == 1)
		SPEX(country_code, SSB_SPROM1_BINF, SSB_SPROM1_BINF_CCODE,
		     SSB_SPROM1_BINF_CCODE_SHIFT);
	SPEX(ant_available_a, SSB_SPROM1_BINF, SSB_SPROM1_BINF_ANTA,
	     SSB_SPROM1_BINF_ANTA_SHIFT);
	SPEX(ant_available_bg, SSB_SPROM1_BINF, SSB_SPROM1_BINF_ANTBG,
	     SSB_SPROM1_BINF_ANTBG_SHIFT);
	SPEX(pa0b0, SSB_SPROM1_PA0B0, 0xFFFF, 0);
	SPEX(pa0b1, SSB_SPROM1_PA0B1, 0xFFFF, 0);
	SPEX(pa0b2, SSB_SPROM1_PA0B2, 0xFFFF, 0);
	SPEX(pa1b0, SSB_SPROM1_PA1B0, 0xFFFF, 0);
	SPEX(pa1b1, SSB_SPROM1_PA1B1, 0xFFFF, 0);
	SPEX(pa1b2, SSB_SPROM1_PA1B2, 0xFFFF, 0);
	SPEX(gpio0, SSB_SPROM1_GPIOA, SSB_SPROM1_GPIOA_P0, 0);
	SPEX(gpio1, SSB_SPROM1_GPIOA, SSB_SPROM1_GPIOA_P1,
	     SSB_SPROM1_GPIOA_P1_SHIFT);
	SPEX(gpio2, SSB_SPROM1_GPIOB, SSB_SPROM1_GPIOB_P2, 0);
	SPEX(gpio3, SSB_SPROM1_GPIOB, SSB_SPROM1_GPIOB_P3,
	     SSB_SPROM1_GPIOB_P3_SHIFT);
	SPEX(maxpwr_a, SSB_SPROM1_MAXPWR, SSB_SPROM1_MAXPWR_A,
	     SSB_SPROM1_MAXPWR_A_SHIFT);
	SPEX(maxpwr_bg, SSB_SPROM1_MAXPWR, SSB_SPROM1_MAXPWR_BG, 0);
	SPEX(itssi_a, SSB_SPROM1_ITSSI, SSB_SPROM1_ITSSI_A,
	     SSB_SPROM1_ITSSI_A_SHIFT);
	SPEX(itssi_bg, SSB_SPROM1_ITSSI, SSB_SPROM1_ITSSI_BG, 0);
	SPEX(boardflags_lo, SSB_SPROM1_BFLLO, 0xFFFF, 0);

	SPEX(alpha2[0], SSB_SPROM1_CCODE, 0xff00, 8);
	SPEX(alpha2[1], SSB_SPROM1_CCODE, 0x00ff, 0);

	/* Extract the antenna gain values. */
	out->antenna_gain.a0 = sprom_extract_antgain(out->revision, in,
						     SSB_SPROM1_AGAIN,
						     SSB_SPROM1_AGAIN_BG,
						     SSB_SPROM1_AGAIN_BG_SHIFT);
	out->antenna_gain.a1 = sprom_extract_antgain(out->revision, in,
						     SSB_SPROM1_AGAIN,
						     SSB_SPROM1_AGAIN_A,
						     SSB_SPROM1_AGAIN_A_SHIFT);
	if (out->revision >= 2)
		sprom_extract_r23(out, in);
}

/* Revs 4 5 and 8 have partially shared layout */
static void sprom_extract_r458(struct ssb_sprom *out, const u16 *in)
{
	SPEX(txpid2g[0], SSB_SPROM4_TXPID2G01,
	     SSB_SPROM4_TXPID2G0, SSB_SPROM4_TXPID2G0_SHIFT);
	SPEX(txpid2g[1], SSB_SPROM4_TXPID2G01,
	     SSB_SPROM4_TXPID2G1, SSB_SPROM4_TXPID2G1_SHIFT);
	SPEX(txpid2g[2], SSB_SPROM4_TXPID2G23,
	     SSB_SPROM4_TXPID2G2, SSB_SPROM4_TXPID2G2_SHIFT);
	SPEX(txpid2g[3], SSB_SPROM4_TXPID2G23,
	     SSB_SPROM4_TXPID2G3, SSB_SPROM4_TXPID2G3_SHIFT);

	SPEX(txpid5gl[0], SSB_SPROM4_TXPID5GL01,
	     SSB_SPROM4_TXPID5GL0, SSB_SPROM4_TXPID5GL0_SHIFT);
	SPEX(txpid5gl[1], SSB_SPROM4_TXPID5GL01,
	     SSB_SPROM4_TXPID5GL1, SSB_SPROM4_TXPID5GL1_SHIFT);
	SPEX(txpid5gl[2], SSB_SPROM4_TXPID5GL23,
	     SSB_SPROM4_TXPID5GL2, SSB_SPROM4_TXPID5GL2_SHIFT);
	SPEX(txpid5gl[3], SSB_SPROM4_TXPID5GL23,
	     SSB_SPROM4_TXPID5GL3, SSB_SPROM4_TXPID5GL3_SHIFT);

	SPEX(txpid5g[0], SSB_SPROM4_TXPID5G01,
	     SSB_SPROM4_TXPID5G0, SSB_SPROM4_TXPID5G0_SHIFT);
	SPEX(txpid5g[1], SSB_SPROM4_TXPID5G01,
	     SSB_SPROM4_TXPID5G1, SSB_SPROM4_TXPID5G1_SHIFT);
	SPEX(txpid5g[2], SSB_SPROM4_TXPID5G23,
	     SSB_SPROM4_TXPID5G2, SSB_SPROM4_TXPID5G2_SHIFT);
	SPEX(txpid5g[3], SSB_SPROM4_TXPID5G23,
	     SSB_SPROM4_TXPID5G3, SSB_SPROM4_TXPID5G3_SHIFT);

	SPEX(txpid5gh[0], SSB_SPROM4_TXPID5GH01,
	     SSB_SPROM4_TXPID5GH0, SSB_SPROM4_TXPID5GH0_SHIFT);
	SPEX(txpid5gh[1], SSB_SPROM4_TXPID5GH01,
	     SSB_SPROM4_TXPID5GH1, SSB_SPROM4_TXPID5GH1_SHIFT);
	SPEX(txpid5gh[2], SSB_SPROM4_TXPID5GH23,
	     SSB_SPROM4_TXPID5GH2, SSB_SPROM4_TXPID5GH2_SHIFT);
	SPEX(txpid5gh[3], SSB_SPROM4_TXPID5GH23,
	     SSB_SPROM4_TXPID5GH3, SSB_SPROM4_TXPID5GH3_SHIFT);
}

static void sprom_extract_r45(struct ssb_sprom *out, const u16 *in)
{
	static const u16 pwr_info_offset[] = {
		SSB_SPROM4_PWR_INFO_CORE0, SSB_SPROM4_PWR_INFO_CORE1,
		SSB_SPROM4_PWR_INFO_CORE2, SSB_SPROM4_PWR_INFO_CORE3
	};
	u16 il0mac_offset;
	int i;

	BUILD_BUG_ON(ARRAY_SIZE(pwr_info_offset) !=
		     ARRAY_SIZE(out->core_pwr_info));

	if (out->revision == 4)
		il0mac_offset = SSB_SPROM4_IL0MAC;
	else
		il0mac_offset = SSB_SPROM5_IL0MAC;

	sprom_get_mac(out->il0mac, &in[SPOFF(il0mac_offset)]);

	SPEX(et0phyaddr, SSB_SPROM4_ETHPHY, SSB_SPROM4_ETHPHY_ET0A, 0);
	SPEX(et1phyaddr, SSB_SPROM4_ETHPHY, SSB_SPROM4_ETHPHY_ET1A,
	     SSB_SPROM4_ETHPHY_ET1A_SHIFT);
	SPEX(board_rev, SSB_SPROM4_BOARDREV, 0xFFFF, 0);
	SPEX(board_type, SSB_SPROM1_SPID, 0xFFFF, 0);
	if (out->revision == 4) {
		SPEX(alpha2[0], SSB_SPROM4_CCODE, 0xff00, 8);
		SPEX(alpha2[1], SSB_SPROM4_CCODE, 0x00ff, 0);
		SPEX(boardflags_lo, SSB_SPROM4_BFLLO, 0xFFFF, 0);
		SPEX(boardflags_hi, SSB_SPROM4_BFLHI, 0xFFFF, 0);
		SPEX(boardflags2_lo, SSB_SPROM4_BFL2LO, 0xFFFF, 0);
		SPEX(boardflags2_hi, SSB_SPROM4_BFL2HI, 0xFFFF, 0);
	} else {
		SPEX(alpha2[0], SSB_SPROM5_CCODE, 0xff00, 8);
		SPEX(alpha2[1], SSB_SPROM5_CCODE, 0x00ff, 0);
		SPEX(boardflags_lo, SSB_SPROM5_BFLLO, 0xFFFF, 0);
		SPEX(boardflags_hi, SSB_SPROM5_BFLHI, 0xFFFF, 0);
		SPEX(boardflags2_lo, SSB_SPROM5_BFL2LO, 0xFFFF, 0);
		SPEX(boardflags2_hi, SSB_SPROM5_BFL2HI, 0xFFFF, 0);
	}
	SPEX(ant_available_a, SSB_SPROM4_ANTAVAIL, SSB_SPROM4_ANTAVAIL_A,
	     SSB_SPROM4_ANTAVAIL_A_SHIFT);
	SPEX(ant_available_bg, SSB_SPROM4_ANTAVAIL, SSB_SPROM4_ANTAVAIL_BG,
	     SSB_SPROM4_ANTAVAIL_BG_SHIFT);
	SPEX(maxpwr_bg, SSB_SPROM4_MAXP_BG, SSB_SPROM4_MAXP_BG_MASK, 0);
	SPEX(itssi_bg, SSB_SPROM4_MAXP_BG, SSB_SPROM4_ITSSI_BG,
	     SSB_SPROM4_ITSSI_BG_SHIFT);
	SPEX(maxpwr_a, SSB_SPROM4_MAXP_A, SSB_SPROM4_MAXP_A_MASK, 0);
	SPEX(itssi_a, SSB_SPROM4_MAXP_A, SSB_SPROM4_ITSSI_A,
	     SSB_SPROM4_ITSSI_A_SHIFT);
	if (out->revision == 4) {
		SPEX(gpio0, SSB_SPROM4_GPIOA, SSB_SPROM4_GPIOA_P0, 0);
		SPEX(gpio1, SSB_SPROM4_GPIOA, SSB_SPROM4_GPIOA_P1,
		     SSB_SPROM4_GPIOA_P1_SHIFT);
		SPEX(gpio2, SSB_SPROM4_GPIOB, SSB_SPROM4_GPIOB_P2, 0);
		SPEX(gpio3, SSB_SPROM4_GPIOB, SSB_SPROM4_GPIOB_P3,
		     SSB_SPROM4_GPIOB_P3_SHIFT);
	} else {
		SPEX(gpio0, SSB_SPROM5_GPIOA, SSB_SPROM5_GPIOA_P0, 0);
		SPEX(gpio1, SSB_SPROM5_GPIOA, SSB_SPROM5_GPIOA_P1,
		     SSB_SPROM5_GPIOA_P1_SHIFT);
		SPEX(gpio2, SSB_SPROM5_GPIOB, SSB_SPROM5_GPIOB_P2, 0);
		SPEX(gpio3, SSB_SPROM5_GPIOB, SSB_SPROM5_GPIOB_P3,
		     SSB_SPROM5_GPIOB_P3_SHIFT);
	}

	/* Extract the antenna gain values. */
	out->antenna_gain.a0 = sprom_extract_antgain(out->revision, in,
						     SSB_SPROM4_AGAIN01,
						     SSB_SPROM4_AGAIN0,
						     SSB_SPROM4_AGAIN0_SHIFT);
	out->antenna_gain.a1 = sprom_extract_antgain(out->revision, in,
						     SSB_SPROM4_AGAIN01,
						     SSB_SPROM4_AGAIN1,
						     SSB_SPROM4_AGAIN1_SHIFT);
	out->antenna_gain.a2 = sprom_extract_antgain(out->revision, in,
						     SSB_SPROM4_AGAIN23,
						     SSB_SPROM4_AGAIN2,
						     SSB_SPROM4_AGAIN2_SHIFT);
	out->antenna_gain.a3 = sprom_extract_antgain(out->revision, in,
						     SSB_SPROM4_AGAIN23,
						     SSB_SPROM4_AGAIN3,
						     SSB_SPROM4_AGAIN3_SHIFT);

	/* Extract cores power info info */
	for (i = 0; i < ARRAY_SIZE(pwr_info_offset); i++) {
		u16 o = pwr_info_offset[i];

		SPEX(core_pwr_info[i].itssi_2g, o + SSB_SPROM4_2G_MAXP_ITSSI,
			SSB_SPROM4_2G_ITSSI, SSB_SPROM4_2G_ITSSI_SHIFT);
		SPEX(core_pwr_info[i].maxpwr_2g, o + SSB_SPROM4_2G_MAXP_ITSSI,
			SSB_SPROM4_2G_MAXP, 0);

		SPEX(core_pwr_info[i].pa_2g[0], o + SSB_SPROM4_2G_PA_0, ~0, 0);
		SPEX(core_pwr_info[i].pa_2g[1], o + SSB_SPROM4_2G_PA_1, ~0, 0);
		SPEX(core_pwr_info[i].pa_2g[2], o + SSB_SPROM4_2G_PA_2, ~0, 0);
		SPEX(core_pwr_info[i].pa_2g[3], o + SSB_SPROM4_2G_PA_3, ~0, 0);

		SPEX(core_pwr_info[i].itssi_5g, o + SSB_SPROM4_5G_MAXP_ITSSI,
			SSB_SPROM4_5G_ITSSI, SSB_SPROM4_5G_ITSSI_SHIFT);
		SPEX(core_pwr_info[i].maxpwr_5g, o + SSB_SPROM4_5G_MAXP_ITSSI,
			SSB_SPROM4_5G_MAXP, 0);
		SPEX(core_pwr_info[i].maxpwr_5gh, o + SSB_SPROM4_5GHL_MAXP,
			SSB_SPROM4_5GH_MAXP, 0);
		SPEX(core_pwr_info[i].maxpwr_5gl, o + SSB_SPROM4_5GHL_MAXP,
			SSB_SPROM4_5GL_MAXP, SSB_SPROM4_5GL_MAXP_SHIFT);

		SPEX(core_pwr_info[i].pa_5gl[0], o + SSB_SPROM4_5GL_PA_0, ~0, 0);
		SPEX(core_pwr_info[i].pa_5gl[1], o + SSB_SPROM4_5GL_PA_1, ~0, 0);
		SPEX(core_pwr_info[i].pa_5gl[2], o + SSB_SPROM4_5GL_PA_2, ~0, 0);
		SPEX(core_pwr_info[i].pa_5gl[3], o + SSB_SPROM4_5GL_PA_3, ~0, 0);
		SPEX(core_pwr_info[i].pa_5g[0], o + SSB_SPROM4_5G_PA_0, ~0, 0);
		SPEX(core_pwr_info[i].pa_5g[1], o + SSB_SPROM4_5G_PA_1, ~0, 0);
		SPEX(core_pwr_info[i].pa_5g[2], o + SSB_SPROM4_5G_PA_2, ~0, 0);
		SPEX(core_pwr_info[i].pa_5g[3], o + SSB_SPROM4_5G_PA_3, ~0, 0);
		SPEX(core_pwr_info[i].pa_5gh[0], o + SSB_SPROM4_5GH_PA_0, ~0, 0);
		SPEX(core_pwr_info[i].pa_5gh[1], o + SSB_SPROM4_5GH_PA_1, ~0, 0);
		SPEX(core_pwr_info[i].pa_5gh[2], o + SSB_SPROM4_5GH_PA_2, ~0, 0);
		SPEX(core_pwr_info[i].pa_5gh[3], o + SSB_SPROM4_5GH_PA_3, ~0, 0);
	}

	sprom_extract_r458(out, in);

	/* TODO - get remaining rev 4 stuff needed */
}

static void sprom_extract_r8(struct ssb_sprom *out, const u16 *in)
{
	int i;
	u16 o;
	static const u16 pwr_info_offset[] = {
		SSB_SROM8_PWR_INFO_CORE0, SSB_SROM8_PWR_INFO_CORE1,
		SSB_SROM8_PWR_INFO_CORE2, SSB_SROM8_PWR_INFO_CORE3
	};
	BUILD_BUG_ON(ARRAY_SIZE(pwr_info_offset) !=
			ARRAY_SIZE(out->core_pwr_info));

	/* extract the MAC address */
	sprom_get_mac(out->il0mac, &in[SPOFF(SSB_SPROM8_IL0MAC)]);

	SPEX(board_rev, SSB_SPROM8_BOARDREV, 0xFFFF, 0);
	SPEX(board_type, SSB_SPROM1_SPID, 0xFFFF, 0);
	SPEX(alpha2[0], SSB_SPROM8_CCODE, 0xff00, 8);
	SPEX(alpha2[1], SSB_SPROM8_CCODE, 0x00ff, 0);
	SPEX(boardflags_lo, SSB_SPROM8_BFLLO, 0xFFFF, 0);
	SPEX(boardflags_hi, SSB_SPROM8_BFLHI, 0xFFFF, 0);
	SPEX(boardflags2_lo, SSB_SPROM8_BFL2LO, 0xFFFF, 0);
	SPEX(boardflags2_hi, SSB_SPROM8_BFL2HI, 0xFFFF, 0);
	SPEX(ant_available_a, SSB_SPROM8_ANTAVAIL, SSB_SPROM8_ANTAVAIL_A,
	     SSB_SPROM8_ANTAVAIL_A_SHIFT);
	SPEX(ant_available_bg, SSB_SPROM8_ANTAVAIL, SSB_SPROM8_ANTAVAIL_BG,
	     SSB_SPROM8_ANTAVAIL_BG_SHIFT);
	SPEX(maxpwr_bg, SSB_SPROM8_MAXP_BG, SSB_SPROM8_MAXP_BG_MASK, 0);
	SPEX(itssi_bg, SSB_SPROM8_MAXP_BG, SSB_SPROM8_ITSSI_BG,
	     SSB_SPROM8_ITSSI_BG_SHIFT);
	SPEX(maxpwr_a, SSB_SPROM8_MAXP_A, SSB_SPROM8_MAXP_A_MASK, 0);
	SPEX(itssi_a, SSB_SPROM8_MAXP_A, SSB_SPROM8_ITSSI_A,
	     SSB_SPROM8_ITSSI_A_SHIFT);
	SPEX(maxpwr_ah, SSB_SPROM8_MAXP_AHL, SSB_SPROM8_MAXP_AH_MASK, 0);
	SPEX(maxpwr_al, SSB_SPROM8_MAXP_AHL, SSB_SPROM8_MAXP_AL_MASK,
	     SSB_SPROM8_MAXP_AL_SHIFT);
	SPEX(gpio0, SSB_SPROM8_GPIOA, SSB_SPROM8_GPIOA_P0, 0);
	SPEX(gpio1, SSB_SPROM8_GPIOA, SSB_SPROM8_GPIOA_P1,
	     SSB_SPROM8_GPIOA_P1_SHIFT);
	SPEX(gpio2, SSB_SPROM8_GPIOB, SSB_SPROM8_GPIOB_P2, 0);
	SPEX(gpio3, SSB_SPROM8_GPIOB, SSB_SPROM8_GPIOB_P3,
	     SSB_SPROM8_GPIOB_P3_SHIFT);
	SPEX(tri2g, SSB_SPROM8_TRI25G, SSB_SPROM8_TRI2G, 0);
	SPEX(tri5g, SSB_SPROM8_TRI25G, SSB_SPROM8_TRI5G,
	     SSB_SPROM8_TRI5G_SHIFT);
	SPEX(tri5gl, SSB_SPROM8_TRI5GHL, SSB_SPROM8_TRI5GL, 0);
	SPEX(tri5gh, SSB_SPROM8_TRI5GHL, SSB_SPROM8_TRI5GH,
	     SSB_SPROM8_TRI5GH_SHIFT);
	SPEX(rxpo2g, SSB_SPROM8_RXPO, SSB_SPROM8_RXPO2G, 0);
	SPEX(rxpo5g, SSB_SPROM8_RXPO, SSB_SPROM8_RXPO5G,
	     SSB_SPROM8_RXPO5G_SHIFT);
	SPEX(rssismf2g, SSB_SPROM8_RSSIPARM2G, SSB_SPROM8_RSSISMF2G, 0);
	SPEX(rssismc2g, SSB_SPROM8_RSSIPARM2G, SSB_SPROM8_RSSISMC2G,
	     SSB_SPROM8_RSSISMC2G_SHIFT);
	SPEX(rssisav2g, SSB_SPROM8_RSSIPARM2G, SSB_SPROM8_RSSISAV2G,
	     SSB_SPROM8_RSSISAV2G_SHIFT);
	SPEX(bxa2g, SSB_SPROM8_RSSIPARM2G, SSB_SPROM8_BXA2G,
	     SSB_SPROM8_BXA2G_SHIFT);
	SPEX(rssismf5g, SSB_SPROM8_RSSIPARM5G, SSB_SPROM8_RSSISMF5G, 0);
	SPEX(rssismc5g, SSB_SPROM8_RSSIPARM5G, SSB_SPROM8_RSSISMC5G,
	     SSB_SPROM8_RSSISMC5G_SHIFT);
	SPEX(rssisav5g, SSB_SPROM8_RSSIPARM5G, SSB_SPROM8_RSSISAV5G,
	     SSB_SPROM8_RSSISAV5G_SHIFT);
	SPEX(bxa5g, SSB_SPROM8_RSSIPARM5G, SSB_SPROM8_BXA5G,
	     SSB_SPROM8_BXA5G_SHIFT);
	SPEX(pa0b0, SSB_SPROM8_PA0B0, 0xFFFF, 0);
	SPEX(pa0b1, SSB_SPROM8_PA0B1, 0xFFFF, 0);
	SPEX(pa0b2, SSB_SPROM8_PA0B2, 0xFFFF, 0);
	SPEX(pa1b0, SSB_SPROM8_PA1B0, 0xFFFF, 0);
	SPEX(pa1b1, SSB_SPROM8_PA1B1, 0xFFFF, 0);
	SPEX(pa1b2, SSB_SPROM8_PA1B2, 0xFFFF, 0);
	SPEX(pa1lob0, SSB_SPROM8_PA1LOB0, 0xFFFF, 0);
	SPEX(pa1lob1, SSB_SPROM8_PA1LOB1, 0xFFFF, 0);
	SPEX(pa1lob2, SSB_SPROM8_PA1LOB2, 0xFFFF, 0);
	SPEX(pa1hib0, SSB_SPROM8_PA1HIB0, 0xFFFF, 0);
	SPEX(pa1hib1, SSB_SPROM8_PA1HIB1, 0xFFFF, 0);
	SPEX(pa1hib2, SSB_SPROM8_PA1HIB2, 0xFFFF, 0);
	SPEX(cck2gpo, SSB_SPROM8_CCK2GPO, 0xFFFF, 0);
	SPEX32(ofdm2gpo, SSB_SPROM8_OFDM2GPO, 0xFFFFFFFF, 0);
	SPEX32(ofdm5glpo, SSB_SPROM8_OFDM5GLPO, 0xFFFFFFFF, 0);
	SPEX32(ofdm5gpo, SSB_SPROM8_OFDM5GPO, 0xFFFFFFFF, 0);
	SPEX32(ofdm5ghpo, SSB_SPROM8_OFDM5GHPO, 0xFFFFFFFF, 0);

	/* Extract the antenna gain values. */
	out->antenna_gain.a0 = sprom_extract_antgain(out->revision, in,
						     SSB_SPROM8_AGAIN01,
						     SSB_SPROM8_AGAIN0,
						     SSB_SPROM8_AGAIN0_SHIFT);
	out->antenna_gain.a1 = sprom_extract_antgain(out->revision, in,
						     SSB_SPROM8_AGAIN01,
						     SSB_SPROM8_AGAIN1,
						     SSB_SPROM8_AGAIN1_SHIFT);
	out->antenna_gain.a2 = sprom_extract_antgain(out->revision, in,
						     SSB_SPROM8_AGAIN23,
						     SSB_SPROM8_AGAIN2,
						     SSB_SPROM8_AGAIN2_SHIFT);
	out->antenna_gain.a3 = sprom_extract_antgain(out->revision, in,
						     SSB_SPROM8_AGAIN23,
						     SSB_SPROM8_AGAIN3,
						     SSB_SPROM8_AGAIN3_SHIFT);

	/* Extract cores power info info */
	for (i = 0; i < ARRAY_SIZE(pwr_info_offset); i++) {
		o = pwr_info_offset[i];
		SPEX(core_pwr_info[i].itssi_2g, o + SSB_SROM8_2G_MAXP_ITSSI,
			SSB_SPROM8_2G_ITSSI, SSB_SPROM8_2G_ITSSI_SHIFT);
		SPEX(core_pwr_info[i].maxpwr_2g, o + SSB_SROM8_2G_MAXP_ITSSI,
			SSB_SPROM8_2G_MAXP, 0);

		SPEX(core_pwr_info[i].pa_2g[0], o + SSB_SROM8_2G_PA_0, ~0, 0);
		SPEX(core_pwr_info[i].pa_2g[1], o + SSB_SROM8_2G_PA_1, ~0, 0);
		SPEX(core_pwr_info[i].pa_2g[2], o + SSB_SROM8_2G_PA_2, ~0, 0);

		SPEX(core_pwr_info[i].itssi_5g, o + SSB_SROM8_5G_MAXP_ITSSI,
			SSB_SPROM8_5G_ITSSI, SSB_SPROM8_5G_ITSSI_SHIFT);
		SPEX(core_pwr_info[i].maxpwr_5g, o + SSB_SROM8_5G_MAXP_ITSSI,
			SSB_SPROM8_5G_MAXP, 0);
		SPEX(core_pwr_info[i].maxpwr_5gh, o + SSB_SPROM8_5GHL_MAXP,
			SSB_SPROM8_5GH_MAXP, 0);
		SPEX(core_pwr_info[i].maxpwr_5gl, o + SSB_SPROM8_5GHL_MAXP,
			SSB_SPROM8_5GL_MAXP, SSB_SPROM8_5GL_MAXP_SHIFT);

		SPEX(core_pwr_info[i].pa_5gl[0], o + SSB_SROM8_5GL_PA_0, ~0, 0);
		SPEX(core_pwr_info[i].pa_5gl[1], o + SSB_SROM8_5GL_PA_1, ~0, 0);
		SPEX(core_pwr_info[i].pa_5gl[2], o + SSB_SROM8_5GL_PA_2, ~0, 0);
		SPEX(core_pwr_info[i].pa_5g[0], o + SSB_SROM8_5G_PA_0, ~0, 0);
		SPEX(core_pwr_info[i].pa_5g[1], o + SSB_SROM8_5G_PA_1, ~0, 0);
		SPEX(core_pwr_info[i].pa_5g[2], o + SSB_SROM8_5G_PA_2, ~0, 0);
		SPEX(core_pwr_info[i].pa_5gh[0], o + SSB_SROM8_5GH_PA_0, ~0, 0);
		SPEX(core_pwr_info[i].pa_5gh[1], o + SSB_SROM8_5GH_PA_1, ~0, 0);
		SPEX(core_pwr_info[i].pa_5gh[2], o + SSB_SROM8_5GH_PA_2, ~0, 0);
	}

	/* Extract FEM info */
	SPEX(fem.ghz2.tssipos, SSB_SPROM8_FEM2G,
		SSB_SROM8_FEM_TSSIPOS, SSB_SROM8_FEM_TSSIPOS_SHIFT);
	SPEX(fem.ghz2.extpa_gain, SSB_SPROM8_FEM2G,
		SSB_SROM8_FEM_EXTPA_GAIN, SSB_SROM8_FEM_EXTPA_GAIN_SHIFT);
	SPEX(fem.ghz2.pdet_range, SSB_SPROM8_FEM2G,
		SSB_SROM8_FEM_PDET_RANGE, SSB_SROM8_FEM_PDET_RANGE_SHIFT);
	SPEX(fem.ghz2.tr_iso, SSB_SPROM8_FEM2G,
		SSB_SROM8_FEM_TR_ISO, SSB_SROM8_FEM_TR_ISO_SHIFT);
	SPEX(fem.ghz2.antswlut, SSB_SPROM8_FEM2G,
		SSB_SROM8_FEM_ANTSWLUT, SSB_SROM8_FEM_ANTSWLUT_SHIFT);

	SPEX(fem.ghz5.tssipos, SSB_SPROM8_FEM5G,
		SSB_SROM8_FEM_TSSIPOS, SSB_SROM8_FEM_TSSIPOS_SHIFT);
	SPEX(fem.ghz5.extpa_gain, SSB_SPROM8_FEM5G,
		SSB_SROM8_FEM_EXTPA_GAIN, SSB_SROM8_FEM_EXTPA_GAIN_SHIFT);
	SPEX(fem.ghz5.pdet_range, SSB_SPROM8_FEM5G,
		SSB_SROM8_FEM_PDET_RANGE, SSB_SROM8_FEM_PDET_RANGE_SHIFT);
	SPEX(fem.ghz5.tr_iso, SSB_SPROM8_FEM5G,
		SSB_SROM8_FEM_TR_ISO, SSB_SROM8_FEM_TR_ISO_SHIFT);
	SPEX(fem.ghz5.antswlut, SSB_SPROM8_FEM5G,
		SSB_SROM8_FEM_ANTSWLUT, SSB_SROM8_FEM_ANTSWLUT_SHIFT);

	SPEX(leddc_on_time, SSB_SPROM8_LEDDC, SSB_SPROM8_LEDDC_ON,
	     SSB_SPROM8_LEDDC_ON_SHIFT);
	SPEX(leddc_off_time, SSB_SPROM8_LEDDC, SSB_SPROM8_LEDDC_OFF,
	     SSB_SPROM8_LEDDC_OFF_SHIFT);

	SPEX(txchain, SSB_SPROM8_TXRXC, SSB_SPROM8_TXRXC_TXCHAIN,
	     SSB_SPROM8_TXRXC_TXCHAIN_SHIFT);
	SPEX(rxchain, SSB_SPROM8_TXRXC, SSB_SPROM8_TXRXC_RXCHAIN,
	     SSB_SPROM8_TXRXC_RXCHAIN_SHIFT);
	SPEX(antswitch, SSB_SPROM8_TXRXC, SSB_SPROM8_TXRXC_SWITCH,
	     SSB_SPROM8_TXRXC_SWITCH_SHIFT);

	SPEX(opo, SSB_SPROM8_OFDM2GPO, 0x00ff, 0);

	SPEX_ARRAY8(mcs2gpo, SSB_SPROM8_2G_MCSPO, ~0, 0);
	SPEX_ARRAY8(mcs5gpo, SSB_SPROM8_5G_MCSPO, ~0, 0);
	SPEX_ARRAY8(mcs5glpo, SSB_SPROM8_5GL_MCSPO, ~0, 0);
	SPEX_ARRAY8(mcs5ghpo, SSB_SPROM8_5GH_MCSPO, ~0, 0);

	SPEX(rawtempsense, SSB_SPROM8_RAWTS, SSB_SPROM8_RAWTS_RAWTEMP,
	     SSB_SPROM8_RAWTS_RAWTEMP_SHIFT);
	SPEX(measpower, SSB_SPROM8_RAWTS, SSB_SPROM8_RAWTS_MEASPOWER,
	     SSB_SPROM8_RAWTS_MEASPOWER_SHIFT);
	SPEX(tempsense_slope, SSB_SPROM8_OPT_CORRX,
	     SSB_SPROM8_OPT_CORRX_TEMP_SLOPE,
	     SSB_SPROM8_OPT_CORRX_TEMP_SLOPE_SHIFT);
	SPEX(tempcorrx, SSB_SPROM8_OPT_CORRX, SSB_SPROM8_OPT_CORRX_TEMPCORRX,
	     SSB_SPROM8_OPT_CORRX_TEMPCORRX_SHIFT);
	SPEX(tempsense_option, SSB_SPROM8_OPT_CORRX,
	     SSB_SPROM8_OPT_CORRX_TEMP_OPTION,
	     SSB_SPROM8_OPT_CORRX_TEMP_OPTION_SHIFT);
	SPEX(freqoffset_corr, SSB_SPROM8_HWIQ_IQSWP,
	     SSB_SPROM8_HWIQ_IQSWP_FREQ_CORR,
	     SSB_SPROM8_HWIQ_IQSWP_FREQ_CORR_SHIFT);
	SPEX(iqcal_swp_dis, SSB_SPROM8_HWIQ_IQSWP,
	     SSB_SPROM8_HWIQ_IQSWP_IQCAL_SWP,
	     SSB_SPROM8_HWIQ_IQSWP_IQCAL_SWP_SHIFT);
	SPEX(hw_iqcal_en, SSB_SPROM8_HWIQ_IQSWP, SSB_SPROM8_HWIQ_IQSWP_HW_IQCAL,
	     SSB_SPROM8_HWIQ_IQSWP_HW_IQCAL_SHIFT);

	SPEX(bw40po, SSB_SPROM8_BW40PO, ~0, 0);
	SPEX(cddpo, SSB_SPROM8_CDDPO, ~0, 0);
	SPEX(stbcpo, SSB_SPROM8_STBCPO, ~0, 0);
	SPEX(bwduppo, SSB_SPROM8_BWDUPPO, ~0, 0);

	SPEX(tempthresh, SSB_SPROM8_THERMAL, SSB_SPROM8_THERMAL_TRESH,
	     SSB_SPROM8_THERMAL_TRESH_SHIFT);
	SPEX(tempoffset, SSB_SPROM8_THERMAL, SSB_SPROM8_THERMAL_OFFSET,
	     SSB_SPROM8_THERMAL_OFFSET_SHIFT);
	SPEX(phycal_tempdelta, SSB_SPROM8_TEMPDELTA,
	     SSB_SPROM8_TEMPDELTA_PHYCAL,
	     SSB_SPROM8_TEMPDELTA_PHYCAL_SHIFT);
	SPEX(temps_period, SSB_SPROM8_TEMPDELTA, SSB_SPROM8_TEMPDELTA_PERIOD,
	     SSB_SPROM8_TEMPDELTA_PERIOD_SHIFT);
	SPEX(temps_hysteresis, SSB_SPROM8_TEMPDELTA,
	     SSB_SPROM8_TEMPDELTA_HYSTERESIS,
	     SSB_SPROM8_TEMPDELTA_HYSTERESIS_SHIFT);
	sprom_extract_r458(out, in);

	/* TODO - get remaining rev 8 stuff needed */
}

static int sprom_extract(struct ssb_bus *bus, struct ssb_sprom *out,
			 const u16 *in, u16 size)
{
	memset(out, 0, sizeof(*out));

	out->revision = in[size - 1] & 0x00FF;
	pr_debug("SPROM revision %d detected\n", out->revision);
	memset(out->et0mac, 0xFF, 6);		/* preset et0 and et1 mac */
	memset(out->et1mac, 0xFF, 6);

	if ((bus->chip_id & 0xFF00) == 0x4400) {
		/* Workaround: The BCM44XX chip has a stupid revision
		 * number stored in the SPROM.
		 * Always extract r1. */
		out->revision = 1;
		pr_debug("SPROM treated as revision %d\n", out->revision);
	}

	switch (out->revision) {
	case 1:
	case 2:
	case 3:
		sprom_extract_r123(out, in);
		break;
	case 4:
	case 5:
		sprom_extract_r45(out, in);
		break;
	case 8:
		sprom_extract_r8(out, in);
		break;
	default:
		pr_warn("Unsupported SPROM revision %d detected. Will extract v1\n",
			out->revision);
		out->revision = 1;
		sprom_extract_r123(out, in);
	}

	if (out->boardflags_lo == 0xFFFF)
		out->boardflags_lo = 0;  /* per specs */
	if (out->boardflags_hi == 0xFFFF)
		out->boardflags_hi = 0;  /* per specs */

	return 0;
}

static int ssb_pci_sprom_get(struct ssb_bus *bus,
			     struct ssb_sprom *sprom)
{
	int err;
	u16 *buf;

	if (!ssb_is_sprom_available(bus)) {
		pr_err("No SPROM available!\n");
		return -ENODEV;
	}
	if (bus->chipco.dev) {	/* can be unavailable! */
		/*
		 * get SPROM offset: SSB_SPROM_BASE1 except for
		 * chipcommon rev >= 31 or chip ID is 0x4312 and
		 * chipcommon status & 3 == 2
		 */
		if (bus->chipco.dev->id.revision >= 31)
			bus->sprom_offset = SSB_SPROM_BASE31;
		else if (bus->chip_id == 0x4312 &&
			 (bus->chipco.status & 0x03) == 2)
			bus->sprom_offset = SSB_SPROM_BASE31;
		else
			bus->sprom_offset = SSB_SPROM_BASE1;
	} else {
		bus->sprom_offset = SSB_SPROM_BASE1;
	}
	pr_debug("SPROM offset is 0x%x\n", bus->sprom_offset);

	buf = kcalloc(SSB_SPROMSIZE_WORDS_R123, sizeof(u16), GFP_KERNEL);
	if (!buf)
		return -ENOMEM;
	bus->sprom_size = SSB_SPROMSIZE_WORDS_R123;
	sprom_do_read(bus, buf);
	err = sprom_check_crc(buf, bus->sprom_size);
	if (err) {
		/* try for a 440 byte SPROM - revision 4 and higher */
		kfree(buf);
		buf = kcalloc(SSB_SPROMSIZE_WORDS_R4, sizeof(u16),
			      GFP_KERNEL);
		if (!buf)
			return -ENOMEM;
		bus->sprom_size = SSB_SPROMSIZE_WORDS_R4;
		sprom_do_read(bus, buf);
		err = sprom_check_crc(buf, bus->sprom_size);
		if (err) {
			/* All CRC attempts failed.
			 * Maybe there is no SPROM on the device?
			 * Now we ask the arch code if there is some sprom
			 * available for this device in some other storage */
			err = ssb_fill_sprom_with_fallback(bus, sprom);
			if (err) {
				pr_warn("WARNING: Using fallback SPROM failed (err %d)\n",
					err);
				goto out_free;
			} else {
				pr_debug("Using SPROM revision %d provided by platform\n",
					 sprom->revision);
				err = 0;
				goto out_free;
			}
			pr_warn("WARNING: Invalid SPROM CRC (corrupt SPROM)\n");
		}
	}
	err = sprom_extract(bus, sprom, buf, bus->sprom_size);

out_free:
	kfree(buf);
	return err;
}

static void ssb_pci_get_boardinfo(struct ssb_bus *bus,
				  struct ssb_boardinfo *bi)
{
	bi->vendor = bus->host_pci->subsystem_vendor;
	bi->type = bus->host_pci->subsystem_device;
}

int ssb_pci_get_invariants(struct ssb_bus *bus,
			   struct ssb_init_invariants *iv)
{
	int err;

	err = ssb_pci_sprom_get(bus, &iv->sprom);
	if (err)
		goto out;
	ssb_pci_get_boardinfo(bus, &iv->boardinfo);

out:
	return err;
}

static int ssb_pci_assert_buspower(struct ssb_bus *bus)
{
	if (likely(bus->powered_up))
		return 0;

	pr_err("FATAL ERROR: Bus powered down while accessing PCI MMIO space\n");
	if (bus->power_warn_count <= 10) {
		bus->power_warn_count++;
		dump_stack();
	}

	return -ENODEV;
}

static u8 ssb_pci_read8(struct ssb_device *dev, u16 offset)
{
	struct ssb_bus *bus = dev->bus;

	if (unlikely(ssb_pci_assert_buspower(bus)))
		return 0xFF;
	if (unlikely(bus->mapped_device != dev)) {
		if (unlikely(ssb_pci_switch_core(bus, dev)))
			return 0xFF;
	}
	return ioread8(bus->mmio + offset);
}

static u16 ssb_pci_read16(struct ssb_device *dev, u16 offset)
{
	struct ssb_bus *bus = dev->bus;

	if (unlikely(ssb_pci_assert_buspower(bus)))
		return 0xFFFF;
	if (unlikely(bus->mapped_device != dev)) {
		if (unlikely(ssb_pci_switch_core(bus, dev)))
			return 0xFFFF;
	}
	return ioread16(bus->mmio + offset);
}

static u32 ssb_pci_read32(struct ssb_device *dev, u16 offset)
{
	struct ssb_bus *bus = dev->bus;

	if (unlikely(ssb_pci_assert_buspower(bus)))
		return 0xFFFFFFFF;
	if (unlikely(bus->mapped_device != dev)) {
		if (unlikely(ssb_pci_switch_core(bus, dev)))
			return 0xFFFFFFFF;
	}
	return ioread32(bus->mmio + offset);
}

#ifdef CONFIG_SSB_BLOCKIO
static void ssb_pci_block_read(struct ssb_device *dev, void *buffer,
			       size_t count, u16 offset, u8 reg_width)
{
	struct ssb_bus *bus = dev->bus;
	void __iomem *addr = bus->mmio + offset;

	if (unlikely(ssb_pci_assert_buspower(bus)))
		goto error;
	if (unlikely(bus->mapped_device != dev)) {
		if (unlikely(ssb_pci_switch_core(bus, dev)))
			goto error;
	}
	switch (reg_width) {
	case sizeof(u8):
		ioread8_rep(addr, buffer, count);
		break;
	case sizeof(u16):
		WARN_ON(count & 1);
		ioread16_rep(addr, buffer, count >> 1);
		break;
	case sizeof(u32):
		WARN_ON(count & 3);
		ioread32_rep(addr, buffer, count >> 2);
		break;
	default:
		WARN_ON(1);
	}

	return;
error:
	memset(buffer, 0xFF, count);
}
#endif /* CONFIG_SSB_BLOCKIO */

static void ssb_pci_write8(struct ssb_device *dev, u16 offset, u8 value)
{
	struct ssb_bus *bus = dev->bus;

	if (unlikely(ssb_pci_assert_buspower(bus)))
		return;
	if (unlikely(bus->mapped_device != dev)) {
		if (unlikely(ssb_pci_switch_core(bus, dev)))
			return;
	}
	iowrite8(value, bus->mmio + offset);
}

static void ssb_pci_write16(struct ssb_device *dev, u16 offset, u16 value)
{
	struct ssb_bus *bus = dev->bus;

	if (unlikely(ssb_pci_assert_buspower(bus)))
		return;
	if (unlikely(bus->mapped_device != dev)) {
		if (unlikely(ssb_pci_switch_core(bus, dev)))
			return;
	}
	iowrite16(value, bus->mmio + offset);
}

static void ssb_pci_write32(struct ssb_device *dev, u16 offset, u32 value)
{
	struct ssb_bus *bus = dev->bus;

	if (unlikely(ssb_pci_assert_buspower(bus)))
		return;
	if (unlikely(bus->mapped_device != dev)) {
		if (unlikely(ssb_pci_switch_core(bus, dev)))
			return;
	}
	iowrite32(value, bus->mmio + offset);
}

#ifdef CONFIG_SSB_BLOCKIO
static void ssb_pci_block_write(struct ssb_device *dev, const void *buffer,
				size_t count, u16 offset, u8 reg_width)
{
	struct ssb_bus *bus = dev->bus;
	void __iomem *addr = bus->mmio + offset;

	if (unlikely(ssb_pci_assert_buspower(bus)))
		return;
	if (unlikely(bus->mapped_device != dev)) {
		if (unlikely(ssb_pci_switch_core(bus, dev)))
			return;
	}
	switch (reg_width) {
	case sizeof(u8):
		iowrite8_rep(addr, buffer, count);
		break;
	case sizeof(u16):
		WARN_ON(count & 1);
		iowrite16_rep(addr, buffer, count >> 1);
		break;
	case sizeof(u32):
		WARN_ON(count & 3);
		iowrite32_rep(addr, buffer, count >> 2);
		break;
	default:
		WARN_ON(1);
	}
}
#endif /* CONFIG_SSB_BLOCKIO */

/* Not "static", as it's used in main.c */
const struct ssb_bus_ops ssb_pci_ops = {
	.read8		= ssb_pci_read8,
	.read16		= ssb_pci_read16,
	.read32		= ssb_pci_read32,
	.write8		= ssb_pci_write8,
	.write16	= ssb_pci_write16,
	.write32	= ssb_pci_write32,
#ifdef CONFIG_SSB_BLOCKIO
	.block_read	= ssb_pci_block_read,
	.block_write	= ssb_pci_block_write,
#endif
};

static ssize_t ssb_pci_attr_sprom_show(struct device *pcidev,
				       struct device_attribute *attr,
				       char *buf)
{
	struct pci_dev *pdev = container_of(pcidev, struct pci_dev, dev);
	struct ssb_bus *bus;

	bus = ssb_pci_dev_to_bus(pdev);
	if (!bus)
		return -ENODEV;

	return ssb_attr_sprom_show(bus, buf, sprom_do_read);
}

static ssize_t ssb_pci_attr_sprom_store(struct device *pcidev,
					struct device_attribute *attr,
					const char *buf, size_t count)
{
	struct pci_dev *pdev = container_of(pcidev, struct pci_dev, dev);
	struct ssb_bus *bus;

	bus = ssb_pci_dev_to_bus(pdev);
	if (!bus)
		return -ENODEV;

	return ssb_attr_sprom_store(bus, buf, count,
				    sprom_check_crc, sprom_do_write);
}

static DEVICE_ATTR(ssb_sprom, 0600,
		   ssb_pci_attr_sprom_show,
		   ssb_pci_attr_sprom_store);

void ssb_pci_exit(struct ssb_bus *bus)
{
	struct pci_dev *pdev;

	if (bus->bustype != SSB_BUSTYPE_PCI)
		return;

	pdev = bus->host_pci;
	device_remove_file(&pdev->dev, &dev_attr_ssb_sprom);
}

int ssb_pci_init(struct ssb_bus *bus)
{
	struct pci_dev *pdev;

	if (bus->bustype != SSB_BUSTYPE_PCI)
		return 0;

	pdev = bus->host_pci;
	mutex_init(&bus->sprom_mutex);

	return device_create_file(&pdev->dev, &dev_attr_ssb_sprom);
}
