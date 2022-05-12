/*
 * Copyright © 2006-2017 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <linux/time.h>

#include "intel_atomic.h"
#include "intel_bw.h"
#include "intel_cdclk.h"
#include "intel_de.h"
#include "intel_display_types.h"
#include "intel_psr.h"
#include "intel_sideband.h"

/**
 * DOC: CDCLK / RAWCLK
 *
 * The display engine uses several different clocks to do its work. There
 * are two main clocks involved that aren't directly related to the actual
 * pixel clock or any symbol/bit clock of the actual output port. These
 * are the core display clock (CDCLK) and RAWCLK.
 *
 * CDCLK clocks most of the display pipe logic, and thus its frequency
 * must be high enough to support the rate at which pixels are flowing
 * through the pipes. Downscaling must also be accounted as that increases
 * the effective pixel rate.
 *
 * On several platforms the CDCLK frequency can be changed dynamically
 * to minimize power consumption for a given display configuration.
 * Typically changes to the CDCLK frequency require all the display pipes
 * to be shut down while the frequency is being changed.
 *
 * On SKL+ the DMC will toggle the CDCLK off/on during DC5/6 entry/exit.
 * DMC will not change the active CDCLK frequency however, so that part
 * will still be performed by the driver directly.
 *
 * RAWCLK is a fixed frequency clock, often used by various auxiliary
 * blocks such as AUX CH or backlight PWM. Hence the only thing we
 * really need to know about RAWCLK is its frequency so that various
 * dividers can be programmed correctly.
 */

static void fixed_133mhz_get_cdclk(struct drm_i915_private *dev_priv,
				   struct intel_cdclk_config *cdclk_config)
{
	cdclk_config->cdclk = 133333;
}

static void fixed_200mhz_get_cdclk(struct drm_i915_private *dev_priv,
				   struct intel_cdclk_config *cdclk_config)
{
	cdclk_config->cdclk = 200000;
}

static void fixed_266mhz_get_cdclk(struct drm_i915_private *dev_priv,
				   struct intel_cdclk_config *cdclk_config)
{
	cdclk_config->cdclk = 266667;
}

static void fixed_333mhz_get_cdclk(struct drm_i915_private *dev_priv,
				   struct intel_cdclk_config *cdclk_config)
{
	cdclk_config->cdclk = 333333;
}

static void fixed_400mhz_get_cdclk(struct drm_i915_private *dev_priv,
				   struct intel_cdclk_config *cdclk_config)
{
	cdclk_config->cdclk = 400000;
}

static void fixed_450mhz_get_cdclk(struct drm_i915_private *dev_priv,
				   struct intel_cdclk_config *cdclk_config)
{
	cdclk_config->cdclk = 450000;
}

static void i85x_get_cdclk(struct drm_i915_private *dev_priv,
			   struct intel_cdclk_config *cdclk_config)
{
	struct pci_dev *pdev = to_pci_dev(dev_priv->drm.dev);
	u16 hpllcc = 0;

	/*
	 * 852GM/852GMV only supports 133 MHz and the HPLLCC
	 * encoding is different :(
	 * FIXME is this the right way to detect 852GM/852GMV?
	 */
	if (pdev->revision == 0x1) {
		cdclk_config->cdclk = 133333;
		return;
	}

	pci_bus_read_config_word(pdev->bus,
				 PCI_DEVFN(0, 3), HPLLCC, &hpllcc);

	/* Assume that the hardware is in the high speed state.  This
	 * should be the default.
	 */
	switch (hpllcc & GC_CLOCK_CONTROL_MASK) {
	case GC_CLOCK_133_200:
	case GC_CLOCK_133_200_2:
	case GC_CLOCK_100_200:
		cdclk_config->cdclk = 200000;
		break;
	case GC_CLOCK_166_250:
		cdclk_config->cdclk = 250000;
		break;
	case GC_CLOCK_100_133:
		cdclk_config->cdclk = 133333;
		break;
	case GC_CLOCK_133_266:
	case GC_CLOCK_133_266_2:
	case GC_CLOCK_166_266:
		cdclk_config->cdclk = 266667;
		break;
	}
}

static void i915gm_get_cdclk(struct drm_i915_private *dev_priv,
			     struct intel_cdclk_config *cdclk_config)
{
	struct pci_dev *pdev = to_pci_dev(dev_priv->drm.dev);
	u16 gcfgc = 0;

	pci_read_config_word(pdev, GCFGC, &gcfgc);

	if (gcfgc & GC_LOW_FREQUENCY_ENABLE) {
		cdclk_config->cdclk = 133333;
		return;
	}

	switch (gcfgc & GC_DISPLAY_CLOCK_MASK) {
	case GC_DISPLAY_CLOCK_333_320_MHZ:
		cdclk_config->cdclk = 333333;
		break;
	default:
	case GC_DISPLAY_CLOCK_190_200_MHZ:
		cdclk_config->cdclk = 190000;
		break;
	}
}

static void i945gm_get_cdclk(struct drm_i915_private *dev_priv,
			     struct intel_cdclk_config *cdclk_config)
{
	struct pci_dev *pdev = to_pci_dev(dev_priv->drm.dev);
	u16 gcfgc = 0;

	pci_read_config_word(pdev, GCFGC, &gcfgc);

	if (gcfgc & GC_LOW_FREQUENCY_ENABLE) {
		cdclk_config->cdclk = 133333;
		return;
	}

	switch (gcfgc & GC_DISPLAY_CLOCK_MASK) {
	case GC_DISPLAY_CLOCK_333_320_MHZ:
		cdclk_config->cdclk = 320000;
		break;
	default:
	case GC_DISPLAY_CLOCK_190_200_MHZ:
		cdclk_config->cdclk = 200000;
		break;
	}
}

static unsigned int intel_hpll_vco(struct drm_i915_private *dev_priv)
{
	static const unsigned int blb_vco[8] = {
		[0] = 3200000,
		[1] = 4000000,
		[2] = 5333333,
		[3] = 4800000,
		[4] = 6400000,
	};
	static const unsigned int pnv_vco[8] = {
		[0] = 3200000,
		[1] = 4000000,
		[2] = 5333333,
		[3] = 4800000,
		[4] = 2666667,
	};
	static const unsigned int cl_vco[8] = {
		[0] = 3200000,
		[1] = 4000000,
		[2] = 5333333,
		[3] = 6400000,
		[4] = 3333333,
		[5] = 3566667,
		[6] = 4266667,
	};
	static const unsigned int elk_vco[8] = {
		[0] = 3200000,
		[1] = 4000000,
		[2] = 5333333,
		[3] = 4800000,
	};
	static const unsigned int ctg_vco[8] = {
		[0] = 3200000,
		[1] = 4000000,
		[2] = 5333333,
		[3] = 6400000,
		[4] = 2666667,
		[5] = 4266667,
	};
	const unsigned int *vco_table;
	unsigned int vco;
	u8 tmp = 0;

	/* FIXME other chipsets? */
	if (IS_GM45(dev_priv))
		vco_table = ctg_vco;
	else if (IS_G45(dev_priv))
		vco_table = elk_vco;
	else if (IS_I965GM(dev_priv))
		vco_table = cl_vco;
	else if (IS_PINEVIEW(dev_priv))
		vco_table = pnv_vco;
	else if (IS_G33(dev_priv))
		vco_table = blb_vco;
	else
		return 0;

	tmp = intel_de_read(dev_priv,
			    IS_PINEVIEW(dev_priv) || IS_MOBILE(dev_priv) ? HPLLVCO_MOBILE : HPLLVCO);

	vco = vco_table[tmp & 0x7];
	if (vco == 0)
		drm_err(&dev_priv->drm, "Bad HPLL VCO (HPLLVCO=0x%02x)\n",
			tmp);
	else
		drm_dbg_kms(&dev_priv->drm, "HPLL VCO %u kHz\n", vco);

	return vco;
}

static void g33_get_cdclk(struct drm_i915_private *dev_priv,
			  struct intel_cdclk_config *cdclk_config)
{
	struct pci_dev *pdev = to_pci_dev(dev_priv->drm.dev);
	static const u8 div_3200[] = { 12, 10,  8,  7, 5, 16 };
	static const u8 div_4000[] = { 14, 12, 10,  8, 6, 20 };
	static const u8 div_4800[] = { 20, 14, 12, 10, 8, 24 };
	static const u8 div_5333[] = { 20, 16, 12, 12, 8, 28 };
	const u8 *div_table;
	unsigned int cdclk_sel;
	u16 tmp = 0;

	cdclk_config->vco = intel_hpll_vco(dev_priv);

	pci_read_config_word(pdev, GCFGC, &tmp);

	cdclk_sel = (tmp >> 4) & 0x7;

	if (cdclk_sel >= ARRAY_SIZE(div_3200))
		goto fail;

	switch (cdclk_config->vco) {
	case 3200000:
		div_table = div_3200;
		break;
	case 4000000:
		div_table = div_4000;
		break;
	case 4800000:
		div_table = div_4800;
		break;
	case 5333333:
		div_table = div_5333;
		break;
	default:
		goto fail;
	}

	cdclk_config->cdclk = DIV_ROUND_CLOSEST(cdclk_config->vco,
						div_table[cdclk_sel]);
	return;

fail:
	drm_err(&dev_priv->drm,
		"Unable to determine CDCLK. HPLL VCO=%u kHz, CFGC=0x%08x\n",
		cdclk_config->vco, tmp);
	cdclk_config->cdclk = 190476;
}

static void pnv_get_cdclk(struct drm_i915_private *dev_priv,
			  struct intel_cdclk_config *cdclk_config)
{
	struct pci_dev *pdev = to_pci_dev(dev_priv->drm.dev);
	u16 gcfgc = 0;

	pci_read_config_word(pdev, GCFGC, &gcfgc);

	switch (gcfgc & GC_DISPLAY_CLOCK_MASK) {
	case GC_DISPLAY_CLOCK_267_MHZ_PNV:
		cdclk_config->cdclk = 266667;
		break;
	case GC_DISPLAY_CLOCK_333_MHZ_PNV:
		cdclk_config->cdclk = 333333;
		break;
	case GC_DISPLAY_CLOCK_444_MHZ_PNV:
		cdclk_config->cdclk = 444444;
		break;
	case GC_DISPLAY_CLOCK_200_MHZ_PNV:
		cdclk_config->cdclk = 200000;
		break;
	default:
		drm_err(&dev_priv->drm,
			"Unknown pnv display core clock 0x%04x\n", gcfgc);
		fallthrough;
	case GC_DISPLAY_CLOCK_133_MHZ_PNV:
		cdclk_config->cdclk = 133333;
		break;
	case GC_DISPLAY_CLOCK_167_MHZ_PNV:
		cdclk_config->cdclk = 166667;
		break;
	}
}

static void i965gm_get_cdclk(struct drm_i915_private *dev_priv,
			     struct intel_cdclk_config *cdclk_config)
{
	struct pci_dev *pdev = to_pci_dev(dev_priv->drm.dev);
	static const u8 div_3200[] = { 16, 10,  8 };
	static const u8 div_4000[] = { 20, 12, 10 };
	static const u8 div_5333[] = { 24, 16, 14 };
	const u8 *div_table;
	unsigned int cdclk_sel;
	u16 tmp = 0;

	cdclk_config->vco = intel_hpll_vco(dev_priv);

	pci_read_config_word(pdev, GCFGC, &tmp);

	cdclk_sel = ((tmp >> 8) & 0x1f) - 1;

	if (cdclk_sel >= ARRAY_SIZE(div_3200))
		goto fail;

	switch (cdclk_config->vco) {
	case 3200000:
		div_table = div_3200;
		break;
	case 4000000:
		div_table = div_4000;
		break;
	case 5333333:
		div_table = div_5333;
		break;
	default:
		goto fail;
	}

	cdclk_config->cdclk = DIV_ROUND_CLOSEST(cdclk_config->vco,
						div_table[cdclk_sel]);
	return;

fail:
	drm_err(&dev_priv->drm,
		"Unable to determine CDCLK. HPLL VCO=%u kHz, CFGC=0x%04x\n",
		cdclk_config->vco, tmp);
	cdclk_config->cdclk = 200000;
}

static void gm45_get_cdclk(struct drm_i915_private *dev_priv,
			   struct intel_cdclk_config *cdclk_config)
{
	struct pci_dev *pdev = to_pci_dev(dev_priv->drm.dev);
	unsigned int cdclk_sel;
	u16 tmp = 0;

	cdclk_config->vco = intel_hpll_vco(dev_priv);

	pci_read_config_word(pdev, GCFGC, &tmp);

	cdclk_sel = (tmp >> 12) & 0x1;

	switch (cdclk_config->vco) {
	case 2666667:
	case 4000000:
	case 5333333:
		cdclk_config->cdclk = cdclk_sel ? 333333 : 222222;
		break;
	case 3200000:
		cdclk_config->cdclk = cdclk_sel ? 320000 : 228571;
		break;
	default:
		drm_err(&dev_priv->drm,
			"Unable to determine CDCLK. HPLL VCO=%u, CFGC=0x%04x\n",
			cdclk_config->vco, tmp);
		cdclk_config->cdclk = 222222;
		break;
	}
}

static void hsw_get_cdclk(struct drm_i915_private *dev_priv,
			  struct intel_cdclk_config *cdclk_config)
{
	u32 lcpll = intel_de_read(dev_priv, LCPLL_CTL);
	u32 freq = lcpll & LCPLL_CLK_FREQ_MASK;

	if (lcpll & LCPLL_CD_SOURCE_FCLK)
		cdclk_config->cdclk = 800000;
	else if (intel_de_read(dev_priv, FUSE_STRAP) & HSW_CDCLK_LIMIT)
		cdclk_config->cdclk = 450000;
	else if (freq == LCPLL_CLK_FREQ_450)
		cdclk_config->cdclk = 450000;
	else if (IS_HSW_ULT(dev_priv))
		cdclk_config->cdclk = 337500;
	else
		cdclk_config->cdclk = 540000;
}

static int vlv_calc_cdclk(struct drm_i915_private *dev_priv, int min_cdclk)
{
	int freq_320 = (dev_priv->hpll_freq <<  1) % 320000 != 0 ?
		333333 : 320000;

	/*
	 * We seem to get an unstable or solid color picture at 200MHz.
	 * Not sure what's wrong. For now use 200MHz only when all pipes
	 * are off.
	 */
	if (IS_VALLEYVIEW(dev_priv) && min_cdclk > freq_320)
		return 400000;
	else if (min_cdclk > 266667)
		return freq_320;
	else if (min_cdclk > 0)
		return 266667;
	else
		return 200000;
}

static u8 vlv_calc_voltage_level(struct drm_i915_private *dev_priv, int cdclk)
{
	if (IS_VALLEYVIEW(dev_priv)) {
		if (cdclk >= 320000) /* jump to highest voltage for 400MHz too */
			return 2;
		else if (cdclk >= 266667)
			return 1;
		else
			return 0;
	} else {
		/*
		 * Specs are full of misinformation, but testing on actual
		 * hardware has shown that we just need to write the desired
		 * CCK divider into the Punit register.
		 */
		return DIV_ROUND_CLOSEST(dev_priv->hpll_freq << 1, cdclk) - 1;
	}
}

static void vlv_get_cdclk(struct drm_i915_private *dev_priv,
			  struct intel_cdclk_config *cdclk_config)
{
	u32 val;

	vlv_iosf_sb_get(dev_priv,
			BIT(VLV_IOSF_SB_CCK) | BIT(VLV_IOSF_SB_PUNIT));

	cdclk_config->vco = vlv_get_hpll_vco(dev_priv);
	cdclk_config->cdclk = vlv_get_cck_clock(dev_priv, "cdclk",
						CCK_DISPLAY_CLOCK_CONTROL,
						cdclk_config->vco);

	val = vlv_punit_read(dev_priv, PUNIT_REG_DSPSSPM);

	vlv_iosf_sb_put(dev_priv,
			BIT(VLV_IOSF_SB_CCK) | BIT(VLV_IOSF_SB_PUNIT));

	if (IS_VALLEYVIEW(dev_priv))
		cdclk_config->voltage_level = (val & DSPFREQGUAR_MASK) >>
			DSPFREQGUAR_SHIFT;
	else
		cdclk_config->voltage_level = (val & DSPFREQGUAR_MASK_CHV) >>
			DSPFREQGUAR_SHIFT_CHV;
}

static void vlv_program_pfi_credits(struct drm_i915_private *dev_priv)
{
	unsigned int credits, default_credits;

	if (IS_CHERRYVIEW(dev_priv))
		default_credits = PFI_CREDIT(12);
	else
		default_credits = PFI_CREDIT(8);

	if (dev_priv->cdclk.hw.cdclk >= dev_priv->czclk_freq) {
		/* CHV suggested value is 31 or 63 */
		if (IS_CHERRYVIEW(dev_priv))
			credits = PFI_CREDIT_63;
		else
			credits = PFI_CREDIT(15);
	} else {
		credits = default_credits;
	}

	/*
	 * WA - write default credits before re-programming
	 * FIXME: should we also set the resend bit here?
	 */
	intel_de_write(dev_priv, GCI_CONTROL,
		       VGA_FAST_MODE_DISABLE | default_credits);

	intel_de_write(dev_priv, GCI_CONTROL,
		       VGA_FAST_MODE_DISABLE | credits | PFI_CREDIT_RESEND);

	/*
	 * FIXME is this guaranteed to clear
	 * immediately or should we poll for it?
	 */
	drm_WARN_ON(&dev_priv->drm,
		    intel_de_read(dev_priv, GCI_CONTROL) & PFI_CREDIT_RESEND);
}

static void vlv_set_cdclk(struct drm_i915_private *dev_priv,
			  const struct intel_cdclk_config *cdclk_config,
			  enum pipe pipe)
{
	int cdclk = cdclk_config->cdclk;
	u32 val, cmd = cdclk_config->voltage_level;
	intel_wakeref_t wakeref;

	switch (cdclk) {
	case 400000:
	case 333333:
	case 320000:
	case 266667:
	case 200000:
		break;
	default:
		MISSING_CASE(cdclk);
		return;
	}

	/* There are cases where we can end up here with power domains
	 * off and a CDCLK frequency other than the minimum, like when
	 * issuing a modeset without actually changing any display after
	 * a system suspend.  So grab the display core domain, which covers
	 * the HW blocks needed for the following programming.
	 */
	wakeref = intel_display_power_get(dev_priv, POWER_DOMAIN_DISPLAY_CORE);

	vlv_iosf_sb_get(dev_priv,
			BIT(VLV_IOSF_SB_CCK) |
			BIT(VLV_IOSF_SB_BUNIT) |
			BIT(VLV_IOSF_SB_PUNIT));

	val = vlv_punit_read(dev_priv, PUNIT_REG_DSPSSPM);
	val &= ~DSPFREQGUAR_MASK;
	val |= (cmd << DSPFREQGUAR_SHIFT);
	vlv_punit_write(dev_priv, PUNIT_REG_DSPSSPM, val);
	if (wait_for((vlv_punit_read(dev_priv, PUNIT_REG_DSPSSPM) &
		      DSPFREQSTAT_MASK) == (cmd << DSPFREQSTAT_SHIFT),
		     50)) {
		drm_err(&dev_priv->drm,
			"timed out waiting for CDclk change\n");
	}

	if (cdclk == 400000) {
		u32 divider;

		divider = DIV_ROUND_CLOSEST(dev_priv->hpll_freq << 1,
					    cdclk) - 1;

		/* adjust cdclk divider */
		val = vlv_cck_read(dev_priv, CCK_DISPLAY_CLOCK_CONTROL);
		val &= ~CCK_FREQUENCY_VALUES;
		val |= divider;
		vlv_cck_write(dev_priv, CCK_DISPLAY_CLOCK_CONTROL, val);

		if (wait_for((vlv_cck_read(dev_priv, CCK_DISPLAY_CLOCK_CONTROL) &
			      CCK_FREQUENCY_STATUS) == (divider << CCK_FREQUENCY_STATUS_SHIFT),
			     50))
			drm_err(&dev_priv->drm,
				"timed out waiting for CDclk change\n");
	}

	/* adjust self-refresh exit latency value */
	val = vlv_bunit_read(dev_priv, BUNIT_REG_BISOC);
	val &= ~0x7f;

	/*
	 * For high bandwidth configs, we set a higher latency in the bunit
	 * so that the core display fetch happens in time to avoid underruns.
	 */
	if (cdclk == 400000)
		val |= 4500 / 250; /* 4.5 usec */
	else
		val |= 3000 / 250; /* 3.0 usec */
	vlv_bunit_write(dev_priv, BUNIT_REG_BISOC, val);

	vlv_iosf_sb_put(dev_priv,
			BIT(VLV_IOSF_SB_CCK) |
			BIT(VLV_IOSF_SB_BUNIT) |
			BIT(VLV_IOSF_SB_PUNIT));

	intel_update_cdclk(dev_priv);

	vlv_program_pfi_credits(dev_priv);

	intel_display_power_put(dev_priv, POWER_DOMAIN_DISPLAY_CORE, wakeref);
}

static void chv_set_cdclk(struct drm_i915_private *dev_priv,
			  const struct intel_cdclk_config *cdclk_config,
			  enum pipe pipe)
{
	int cdclk = cdclk_config->cdclk;
	u32 val, cmd = cdclk_config->voltage_level;
	intel_wakeref_t wakeref;

	switch (cdclk) {
	case 333333:
	case 320000:
	case 266667:
	case 200000:
		break;
	default:
		MISSING_CASE(cdclk);
		return;
	}

	/* There are cases where we can end up here with power domains
	 * off and a CDCLK frequency other than the minimum, like when
	 * issuing a modeset without actually changing any display after
	 * a system suspend.  So grab the display core domain, which covers
	 * the HW blocks needed for the following programming.
	 */
	wakeref = intel_display_power_get(dev_priv, POWER_DOMAIN_DISPLAY_CORE);

	vlv_punit_get(dev_priv);
	val = vlv_punit_read(dev_priv, PUNIT_REG_DSPSSPM);
	val &= ~DSPFREQGUAR_MASK_CHV;
	val |= (cmd << DSPFREQGUAR_SHIFT_CHV);
	vlv_punit_write(dev_priv, PUNIT_REG_DSPSSPM, val);
	if (wait_for((vlv_punit_read(dev_priv, PUNIT_REG_DSPSSPM) &
		      DSPFREQSTAT_MASK_CHV) == (cmd << DSPFREQSTAT_SHIFT_CHV),
		     50)) {
		drm_err(&dev_priv->drm,
			"timed out waiting for CDclk change\n");
	}

	vlv_punit_put(dev_priv);

	intel_update_cdclk(dev_priv);

	vlv_program_pfi_credits(dev_priv);

	intel_display_power_put(dev_priv, POWER_DOMAIN_DISPLAY_CORE, wakeref);
}

static int bdw_calc_cdclk(int min_cdclk)
{
	if (min_cdclk > 540000)
		return 675000;
	else if (min_cdclk > 450000)
		return 540000;
	else if (min_cdclk > 337500)
		return 450000;
	else
		return 337500;
}

static u8 bdw_calc_voltage_level(int cdclk)
{
	switch (cdclk) {
	default:
	case 337500:
		return 2;
	case 450000:
		return 0;
	case 540000:
		return 1;
	case 675000:
		return 3;
	}
}

static void bdw_get_cdclk(struct drm_i915_private *dev_priv,
			  struct intel_cdclk_config *cdclk_config)
{
	u32 lcpll = intel_de_read(dev_priv, LCPLL_CTL);
	u32 freq = lcpll & LCPLL_CLK_FREQ_MASK;

	if (lcpll & LCPLL_CD_SOURCE_FCLK)
		cdclk_config->cdclk = 800000;
	else if (intel_de_read(dev_priv, FUSE_STRAP) & HSW_CDCLK_LIMIT)
		cdclk_config->cdclk = 450000;
	else if (freq == LCPLL_CLK_FREQ_450)
		cdclk_config->cdclk = 450000;
	else if (freq == LCPLL_CLK_FREQ_54O_BDW)
		cdclk_config->cdclk = 540000;
	else if (freq == LCPLL_CLK_FREQ_337_5_BDW)
		cdclk_config->cdclk = 337500;
	else
		cdclk_config->cdclk = 675000;

	/*
	 * Can't read this out :( Let's assume it's
	 * at least what the CDCLK frequency requires.
	 */
	cdclk_config->voltage_level =
		bdw_calc_voltage_level(cdclk_config->cdclk);
}

static u32 bdw_cdclk_freq_sel(int cdclk)
{
	switch (cdclk) {
	default:
		MISSING_CASE(cdclk);
		fallthrough;
	case 337500:
		return LCPLL_CLK_FREQ_337_5_BDW;
	case 450000:
		return LCPLL_CLK_FREQ_450;
	case 540000:
		return LCPLL_CLK_FREQ_54O_BDW;
	case 675000:
		return LCPLL_CLK_FREQ_675_BDW;
	}
}

static void bdw_set_cdclk(struct drm_i915_private *dev_priv,
			  const struct intel_cdclk_config *cdclk_config,
			  enum pipe pipe)
{
	int cdclk = cdclk_config->cdclk;
	int ret;

	if (drm_WARN(&dev_priv->drm,
		     (intel_de_read(dev_priv, LCPLL_CTL) &
		      (LCPLL_PLL_DISABLE | LCPLL_PLL_LOCK |
		       LCPLL_CD_CLOCK_DISABLE | LCPLL_ROOT_CD_CLOCK_DISABLE |
		       LCPLL_CD2X_CLOCK_DISABLE | LCPLL_POWER_DOWN_ALLOW |
		       LCPLL_CD_SOURCE_FCLK)) != LCPLL_PLL_LOCK,
		     "trying to change cdclk frequency with cdclk not enabled\n"))
		return;

	ret = sandybridge_pcode_write(dev_priv,
				      BDW_PCODE_DISPLAY_FREQ_CHANGE_REQ, 0x0);
	if (ret) {
		drm_err(&dev_priv->drm,
			"failed to inform pcode about cdclk change\n");
		return;
	}

	intel_de_rmw(dev_priv, LCPLL_CTL,
		     0, LCPLL_CD_SOURCE_FCLK);

	/*
	 * According to the spec, it should be enough to poll for this 1 us.
	 * However, extensive testing shows that this can take longer.
	 */
	if (wait_for_us(intel_de_read(dev_priv, LCPLL_CTL) &
			LCPLL_CD_SOURCE_FCLK_DONE, 100))
		drm_err(&dev_priv->drm, "Switching to FCLK failed\n");

	intel_de_rmw(dev_priv, LCPLL_CTL,
		     LCPLL_CLK_FREQ_MASK, bdw_cdclk_freq_sel(cdclk));

	intel_de_rmw(dev_priv, LCPLL_CTL,
		     LCPLL_CD_SOURCE_FCLK, 0);

	if (wait_for_us((intel_de_read(dev_priv, LCPLL_CTL) &
			 LCPLL_CD_SOURCE_FCLK_DONE) == 0, 1))
		drm_err(&dev_priv->drm, "Switching back to LCPLL failed\n");

	sandybridge_pcode_write(dev_priv, HSW_PCODE_DE_WRITE_FREQ_REQ,
				cdclk_config->voltage_level);

	intel_de_write(dev_priv, CDCLK_FREQ,
		       DIV_ROUND_CLOSEST(cdclk, 1000) - 1);

	intel_update_cdclk(dev_priv);
}

static int skl_calc_cdclk(int min_cdclk, int vco)
{
	if (vco == 8640000) {
		if (min_cdclk > 540000)
			return 617143;
		else if (min_cdclk > 432000)
			return 540000;
		else if (min_cdclk > 308571)
			return 432000;
		else
			return 308571;
	} else {
		if (min_cdclk > 540000)
			return 675000;
		else if (min_cdclk > 450000)
			return 540000;
		else if (min_cdclk > 337500)
			return 450000;
		else
			return 337500;
	}
}

static u8 skl_calc_voltage_level(int cdclk)
{
	if (cdclk > 540000)
		return 3;
	else if (cdclk > 450000)
		return 2;
	else if (cdclk > 337500)
		return 1;
	else
		return 0;
}

static void skl_dpll0_update(struct drm_i915_private *dev_priv,
			     struct intel_cdclk_config *cdclk_config)
{
	u32 val;

	cdclk_config->ref = 24000;
	cdclk_config->vco = 0;

	val = intel_de_read(dev_priv, LCPLL1_CTL);
	if ((val & LCPLL_PLL_ENABLE) == 0)
		return;

	if (drm_WARN_ON(&dev_priv->drm, (val & LCPLL_PLL_LOCK) == 0))
		return;

	val = intel_de_read(dev_priv, DPLL_CTRL1);

	if (drm_WARN_ON(&dev_priv->drm,
			(val & (DPLL_CTRL1_HDMI_MODE(SKL_DPLL0) |
				DPLL_CTRL1_SSC(SKL_DPLL0) |
				DPLL_CTRL1_OVERRIDE(SKL_DPLL0))) !=
			DPLL_CTRL1_OVERRIDE(SKL_DPLL0)))
		return;

	switch (val & DPLL_CTRL1_LINK_RATE_MASK(SKL_DPLL0)) {
	case DPLL_CTRL1_LINK_RATE(DPLL_CTRL1_LINK_RATE_810, SKL_DPLL0):
	case DPLL_CTRL1_LINK_RATE(DPLL_CTRL1_LINK_RATE_1350, SKL_DPLL0):
	case DPLL_CTRL1_LINK_RATE(DPLL_CTRL1_LINK_RATE_1620, SKL_DPLL0):
	case DPLL_CTRL1_LINK_RATE(DPLL_CTRL1_LINK_RATE_2700, SKL_DPLL0):
		cdclk_config->vco = 8100000;
		break;
	case DPLL_CTRL1_LINK_RATE(DPLL_CTRL1_LINK_RATE_1080, SKL_DPLL0):
	case DPLL_CTRL1_LINK_RATE(DPLL_CTRL1_LINK_RATE_2160, SKL_DPLL0):
		cdclk_config->vco = 8640000;
		break;
	default:
		MISSING_CASE(val & DPLL_CTRL1_LINK_RATE_MASK(SKL_DPLL0));
		break;
	}
}

static void skl_get_cdclk(struct drm_i915_private *dev_priv,
			  struct intel_cdclk_config *cdclk_config)
{
	u32 cdctl;

	skl_dpll0_update(dev_priv, cdclk_config);

	cdclk_config->cdclk = cdclk_config->bypass = cdclk_config->ref;

	if (cdclk_config->vco == 0)
		goto out;

	cdctl = intel_de_read(dev_priv, CDCLK_CTL);

	if (cdclk_config->vco == 8640000) {
		switch (cdctl & CDCLK_FREQ_SEL_MASK) {
		case CDCLK_FREQ_450_432:
			cdclk_config->cdclk = 432000;
			break;
		case CDCLK_FREQ_337_308:
			cdclk_config->cdclk = 308571;
			break;
		case CDCLK_FREQ_540:
			cdclk_config->cdclk = 540000;
			break;
		case CDCLK_FREQ_675_617:
			cdclk_config->cdclk = 617143;
			break;
		default:
			MISSING_CASE(cdctl & CDCLK_FREQ_SEL_MASK);
			break;
		}
	} else {
		switch (cdctl & CDCLK_FREQ_SEL_MASK) {
		case CDCLK_FREQ_450_432:
			cdclk_config->cdclk = 450000;
			break;
		case CDCLK_FREQ_337_308:
			cdclk_config->cdclk = 337500;
			break;
		case CDCLK_FREQ_540:
			cdclk_config->cdclk = 540000;
			break;
		case CDCLK_FREQ_675_617:
			cdclk_config->cdclk = 675000;
			break;
		default:
			MISSING_CASE(cdctl & CDCLK_FREQ_SEL_MASK);
			break;
		}
	}

 out:
	/*
	 * Can't read this out :( Let's assume it's
	 * at least what the CDCLK frequency requires.
	 */
	cdclk_config->voltage_level =
		skl_calc_voltage_level(cdclk_config->cdclk);
}

/* convert from kHz to .1 fixpoint MHz with -1MHz offset */
static int skl_cdclk_decimal(int cdclk)
{
	return DIV_ROUND_CLOSEST(cdclk - 1000, 500);
}

static void skl_set_preferred_cdclk_vco(struct drm_i915_private *dev_priv,
					int vco)
{
	bool changed = dev_priv->skl_preferred_vco_freq != vco;

	dev_priv->skl_preferred_vco_freq = vco;

	if (changed)
		intel_update_max_cdclk(dev_priv);
}

static u32 skl_dpll0_link_rate(struct drm_i915_private *dev_priv, int vco)
{
	drm_WARN_ON(&dev_priv->drm, vco != 8100000 && vco != 8640000);

	/*
	 * We always enable DPLL0 with the lowest link rate possible, but still
	 * taking into account the VCO required to operate the eDP panel at the
	 * desired frequency. The usual DP link rates operate with a VCO of
	 * 8100 while the eDP 1.4 alternate link rates need a VCO of 8640.
	 * The modeset code is responsible for the selection of the exact link
	 * rate later on, with the constraint of choosing a frequency that
	 * works with vco.
	 */
	if (vco == 8640000)
		return DPLL_CTRL1_LINK_RATE(DPLL_CTRL1_LINK_RATE_1080, SKL_DPLL0);
	else
		return DPLL_CTRL1_LINK_RATE(DPLL_CTRL1_LINK_RATE_810, SKL_DPLL0);
}

static void skl_dpll0_enable(struct drm_i915_private *dev_priv, int vco)
{
	intel_de_rmw(dev_priv, DPLL_CTRL1,
		     DPLL_CTRL1_HDMI_MODE(SKL_DPLL0) |
		     DPLL_CTRL1_SSC(SKL_DPLL0) |
		     DPLL_CTRL1_LINK_RATE_MASK(SKL_DPLL0),
		     DPLL_CTRL1_OVERRIDE(SKL_DPLL0) |
		     skl_dpll0_link_rate(dev_priv, vco));
	intel_de_posting_read(dev_priv, DPLL_CTRL1);

	intel_de_rmw(dev_priv, LCPLL1_CTL,
		     0, LCPLL_PLL_ENABLE);

	if (intel_de_wait_for_set(dev_priv, LCPLL1_CTL, LCPLL_PLL_LOCK, 5))
		drm_err(&dev_priv->drm, "DPLL0 not locked\n");

	dev_priv->cdclk.hw.vco = vco;

	/* We'll want to keep using the current vco from now on. */
	skl_set_preferred_cdclk_vco(dev_priv, vco);
}

static void skl_dpll0_disable(struct drm_i915_private *dev_priv)
{
	intel_de_rmw(dev_priv, LCPLL1_CTL,
		     LCPLL_PLL_ENABLE, 0);

	if (intel_de_wait_for_clear(dev_priv, LCPLL1_CTL, LCPLL_PLL_LOCK, 1))
		drm_err(&dev_priv->drm, "Couldn't disable DPLL0\n");

	dev_priv->cdclk.hw.vco = 0;
}

static u32 skl_cdclk_freq_sel(struct drm_i915_private *dev_priv,
			      int cdclk, int vco)
{
	switch (cdclk) {
	default:
		drm_WARN_ON(&dev_priv->drm,
			    cdclk != dev_priv->cdclk.hw.bypass);
		drm_WARN_ON(&dev_priv->drm, vco != 0);
		fallthrough;
	case 308571:
	case 337500:
		return CDCLK_FREQ_337_308;
	case 450000:
	case 432000:
		return CDCLK_FREQ_450_432;
	case 540000:
		return CDCLK_FREQ_540;
	case 617143:
	case 675000:
		return CDCLK_FREQ_675_617;
	}
}

static void skl_set_cdclk(struct drm_i915_private *dev_priv,
			  const struct intel_cdclk_config *cdclk_config,
			  enum pipe pipe)
{
	int cdclk = cdclk_config->cdclk;
	int vco = cdclk_config->vco;
	u32 freq_select, cdclk_ctl;
	int ret;

	/*
	 * Based on WA#1183 CDCLK rates 308 and 617MHz CDCLK rates are
	 * unsupported on SKL. In theory this should never happen since only
	 * the eDP1.4 2.16 and 4.32Gbps rates require it, but eDP1.4 is not
	 * supported on SKL either, see the above WA. WARN whenever trying to
	 * use the corresponding VCO freq as that always leads to using the
	 * minimum 308MHz CDCLK.
	 */
	drm_WARN_ON_ONCE(&dev_priv->drm,
			 IS_SKYLAKE(dev_priv) && vco == 8640000);

	ret = skl_pcode_request(dev_priv, SKL_PCODE_CDCLK_CONTROL,
				SKL_CDCLK_PREPARE_FOR_CHANGE,
				SKL_CDCLK_READY_FOR_CHANGE,
				SKL_CDCLK_READY_FOR_CHANGE, 3);
	if (ret) {
		drm_err(&dev_priv->drm,
			"Failed to inform PCU about cdclk change (%d)\n", ret);
		return;
	}

	freq_select = skl_cdclk_freq_sel(dev_priv, cdclk, vco);

	if (dev_priv->cdclk.hw.vco != 0 &&
	    dev_priv->cdclk.hw.vco != vco)
		skl_dpll0_disable(dev_priv);

	cdclk_ctl = intel_de_read(dev_priv, CDCLK_CTL);

	if (dev_priv->cdclk.hw.vco != vco) {
		/* Wa Display #1183: skl,kbl,cfl */
		cdclk_ctl &= ~(CDCLK_FREQ_SEL_MASK | CDCLK_FREQ_DECIMAL_MASK);
		cdclk_ctl |= freq_select | skl_cdclk_decimal(cdclk);
		intel_de_write(dev_priv, CDCLK_CTL, cdclk_ctl);
	}

	/* Wa Display #1183: skl,kbl,cfl */
	cdclk_ctl |= CDCLK_DIVMUX_CD_OVERRIDE;
	intel_de_write(dev_priv, CDCLK_CTL, cdclk_ctl);
	intel_de_posting_read(dev_priv, CDCLK_CTL);

	if (dev_priv->cdclk.hw.vco != vco)
		skl_dpll0_enable(dev_priv, vco);

	/* Wa Display #1183: skl,kbl,cfl */
	cdclk_ctl &= ~(CDCLK_FREQ_SEL_MASK | CDCLK_FREQ_DECIMAL_MASK);
	intel_de_write(dev_priv, CDCLK_CTL, cdclk_ctl);

	cdclk_ctl |= freq_select | skl_cdclk_decimal(cdclk);
	intel_de_write(dev_priv, CDCLK_CTL, cdclk_ctl);

	/* Wa Display #1183: skl,kbl,cfl */
	cdclk_ctl &= ~CDCLK_DIVMUX_CD_OVERRIDE;
	intel_de_write(dev_priv, CDCLK_CTL, cdclk_ctl);
	intel_de_posting_read(dev_priv, CDCLK_CTL);

	/* inform PCU of the change */
	sandybridge_pcode_write(dev_priv, SKL_PCODE_CDCLK_CONTROL,
				cdclk_config->voltage_level);

	intel_update_cdclk(dev_priv);
}

static void skl_sanitize_cdclk(struct drm_i915_private *dev_priv)
{
	u32 cdctl, expected;

	/*
	 * check if the pre-os initialized the display
	 * There is SWF18 scratchpad register defined which is set by the
	 * pre-os which can be used by the OS drivers to check the status
	 */
	if ((intel_de_read(dev_priv, SWF_ILK(0x18)) & 0x00FFFFFF) == 0)
		goto sanitize;

	intel_update_cdclk(dev_priv);
	intel_dump_cdclk_config(&dev_priv->cdclk.hw, "Current CDCLK");

	/* Is PLL enabled and locked ? */
	if (dev_priv->cdclk.hw.vco == 0 ||
	    dev_priv->cdclk.hw.cdclk == dev_priv->cdclk.hw.bypass)
		goto sanitize;

	/* DPLL okay; verify the cdclock
	 *
	 * Noticed in some instances that the freq selection is correct but
	 * decimal part is programmed wrong from BIOS where pre-os does not
	 * enable display. Verify the same as well.
	 */
	cdctl = intel_de_read(dev_priv, CDCLK_CTL);
	expected = (cdctl & CDCLK_FREQ_SEL_MASK) |
		skl_cdclk_decimal(dev_priv->cdclk.hw.cdclk);
	if (cdctl == expected)
		/* All well; nothing to sanitize */
		return;

sanitize:
	drm_dbg_kms(&dev_priv->drm, "Sanitizing cdclk programmed by pre-os\n");

	/* force cdclk programming */
	dev_priv->cdclk.hw.cdclk = 0;
	/* force full PLL disable + enable */
	dev_priv->cdclk.hw.vco = -1;
}

static void skl_cdclk_init_hw(struct drm_i915_private *dev_priv)
{
	struct intel_cdclk_config cdclk_config;

	skl_sanitize_cdclk(dev_priv);

	if (dev_priv->cdclk.hw.cdclk != 0 &&
	    dev_priv->cdclk.hw.vco != 0) {
		/*
		 * Use the current vco as our initial
		 * guess as to what the preferred vco is.
		 */
		if (dev_priv->skl_preferred_vco_freq == 0)
			skl_set_preferred_cdclk_vco(dev_priv,
						    dev_priv->cdclk.hw.vco);
		return;
	}

	cdclk_config = dev_priv->cdclk.hw;

	cdclk_config.vco = dev_priv->skl_preferred_vco_freq;
	if (cdclk_config.vco == 0)
		cdclk_config.vco = 8100000;
	cdclk_config.cdclk = skl_calc_cdclk(0, cdclk_config.vco);
	cdclk_config.voltage_level = skl_calc_voltage_level(cdclk_config.cdclk);

	skl_set_cdclk(dev_priv, &cdclk_config, INVALID_PIPE);
}

static void skl_cdclk_uninit_hw(struct drm_i915_private *dev_priv)
{
	struct intel_cdclk_config cdclk_config = dev_priv->cdclk.hw;

	cdclk_config.cdclk = cdclk_config.bypass;
	cdclk_config.vco = 0;
	cdclk_config.voltage_level = skl_calc_voltage_level(cdclk_config.cdclk);

	skl_set_cdclk(dev_priv, &cdclk_config, INVALID_PIPE);
}

static const struct intel_cdclk_vals bxt_cdclk_table[] = {
	{ .refclk = 19200, .cdclk = 144000, .divider = 8, .ratio = 60 },
	{ .refclk = 19200, .cdclk = 288000, .divider = 4, .ratio = 60 },
	{ .refclk = 19200, .cdclk = 384000, .divider = 3, .ratio = 60 },
	{ .refclk = 19200, .cdclk = 576000, .divider = 2, .ratio = 60 },
	{ .refclk = 19200, .cdclk = 624000, .divider = 2, .ratio = 65 },
	{}
};

static const struct intel_cdclk_vals glk_cdclk_table[] = {
	{ .refclk = 19200, .cdclk =  79200, .divider = 8, .ratio = 33 },
	{ .refclk = 19200, .cdclk = 158400, .divider = 4, .ratio = 33 },
	{ .refclk = 19200, .cdclk = 316800, .divider = 2, .ratio = 33 },
	{}
};

static const struct intel_cdclk_vals cnl_cdclk_table[] = {
	{ .refclk = 19200, .cdclk = 168000, .divider = 4, .ratio = 35 },
	{ .refclk = 19200, .cdclk = 336000, .divider = 2, .ratio = 35 },
	{ .refclk = 19200, .cdclk = 528000, .divider = 2, .ratio = 55 },

	{ .refclk = 24000, .cdclk = 168000, .divider = 4, .ratio = 28 },
	{ .refclk = 24000, .cdclk = 336000, .divider = 2, .ratio = 28 },
	{ .refclk = 24000, .cdclk = 528000, .divider = 2, .ratio = 44 },
	{}
};

static const struct intel_cdclk_vals icl_cdclk_table[] = {
	{ .refclk = 19200, .cdclk = 172800, .divider = 2, .ratio = 18 },
	{ .refclk = 19200, .cdclk = 192000, .divider = 2, .ratio = 20 },
	{ .refclk = 19200, .cdclk = 307200, .divider = 2, .ratio = 32 },
	{ .refclk = 19200, .cdclk = 326400, .divider = 4, .ratio = 68 },
	{ .refclk = 19200, .cdclk = 556800, .divider = 2, .ratio = 58 },
	{ .refclk = 19200, .cdclk = 652800, .divider = 2, .ratio = 68 },

	{ .refclk = 24000, .cdclk = 180000, .divider = 2, .ratio = 15 },
	{ .refclk = 24000, .cdclk = 192000, .divider = 2, .ratio = 16 },
	{ .refclk = 24000, .cdclk = 312000, .divider = 2, .ratio = 26 },
	{ .refclk = 24000, .cdclk = 324000, .divider = 4, .ratio = 54 },
	{ .refclk = 24000, .cdclk = 552000, .divider = 2, .ratio = 46 },
	{ .refclk = 24000, .cdclk = 648000, .divider = 2, .ratio = 54 },

	{ .refclk = 38400, .cdclk = 172800, .divider = 2, .ratio =  9 },
	{ .refclk = 38400, .cdclk = 192000, .divider = 2, .ratio = 10 },
	{ .refclk = 38400, .cdclk = 307200, .divider = 2, .ratio = 16 },
	{ .refclk = 38400, .cdclk = 326400, .divider = 4, .ratio = 34 },
	{ .refclk = 38400, .cdclk = 556800, .divider = 2, .ratio = 29 },
	{ .refclk = 38400, .cdclk = 652800, .divider = 2, .ratio = 34 },
	{}
};

static const struct intel_cdclk_vals rkl_cdclk_table[] = {
	{ .refclk = 19200, .cdclk = 172800, .divider = 4, .ratio =  36 },
	{ .refclk = 19200, .cdclk = 192000, .divider = 4, .ratio =  40 },
	{ .refclk = 19200, .cdclk = 307200, .divider = 4, .ratio =  64 },
	{ .refclk = 19200, .cdclk = 326400, .divider = 8, .ratio = 136 },
	{ .refclk = 19200, .cdclk = 556800, .divider = 4, .ratio = 116 },
	{ .refclk = 19200, .cdclk = 652800, .divider = 4, .ratio = 136 },

	{ .refclk = 24000, .cdclk = 180000, .divider = 4, .ratio =  30 },
	{ .refclk = 24000, .cdclk = 192000, .divider = 4, .ratio =  32 },
	{ .refclk = 24000, .cdclk = 312000, .divider = 4, .ratio =  52 },
	{ .refclk = 24000, .cdclk = 324000, .divider = 8, .ratio = 108 },
	{ .refclk = 24000, .cdclk = 552000, .divider = 4, .ratio =  92 },
	{ .refclk = 24000, .cdclk = 648000, .divider = 4, .ratio = 108 },

	{ .refclk = 38400, .cdclk = 172800, .divider = 4, .ratio = 18 },
	{ .refclk = 38400, .cdclk = 192000, .divider = 4, .ratio = 20 },
	{ .refclk = 38400, .cdclk = 307200, .divider = 4, .ratio = 32 },
	{ .refclk = 38400, .cdclk = 326400, .divider = 8, .ratio = 68 },
	{ .refclk = 38400, .cdclk = 556800, .divider = 4, .ratio = 58 },
	{ .refclk = 38400, .cdclk = 652800, .divider = 4, .ratio = 68 },
	{}
};

static const struct intel_cdclk_vals adlp_a_step_cdclk_table[] = {
	{ .refclk = 19200, .cdclk = 307200, .divider = 2, .ratio = 32 },
	{ .refclk = 19200, .cdclk = 556800, .divider = 2, .ratio = 58 },
	{ .refclk = 19200, .cdclk = 652800, .divider = 2, .ratio = 68 },

	{ .refclk = 24000, .cdclk = 312000, .divider = 2, .ratio = 26 },
	{ .refclk = 24000, .cdclk = 552000, .divider = 2, .ratio = 46 },
	{ .refclk = 24400, .cdclk = 648000, .divider = 2, .ratio = 54 },

	{ .refclk = 38400, .cdclk = 307200, .divider = 2, .ratio = 16 },
	{ .refclk = 38400, .cdclk = 556800, .divider = 2, .ratio = 29 },
	{ .refclk = 38400, .cdclk = 652800, .divider = 2, .ratio = 34 },
	{}
};

static const struct intel_cdclk_vals adlp_cdclk_table[] = {
	{ .refclk = 19200, .cdclk = 172800, .divider = 3, .ratio = 27 },
	{ .refclk = 19200, .cdclk = 192000, .divider = 2, .ratio = 20 },
	{ .refclk = 19200, .cdclk = 307200, .divider = 2, .ratio = 32 },
	{ .refclk = 19200, .cdclk = 556800, .divider = 2, .ratio = 58 },
	{ .refclk = 19200, .cdclk = 652800, .divider = 2, .ratio = 68 },

	{ .refclk = 24000, .cdclk = 176000, .divider = 3, .ratio = 22 },
	{ .refclk = 24000, .cdclk = 192000, .divider = 2, .ratio = 16 },
	{ .refclk = 24000, .cdclk = 312000, .divider = 2, .ratio = 26 },
	{ .refclk = 24000, .cdclk = 552000, .divider = 2, .ratio = 46 },
	{ .refclk = 24400, .cdclk = 648000, .divider = 2, .ratio = 54 },

	{ .refclk = 38400, .cdclk = 179200, .divider = 3, .ratio = 14 },
	{ .refclk = 38400, .cdclk = 192000, .divider = 2, .ratio = 10 },
	{ .refclk = 38400, .cdclk = 307200, .divider = 2, .ratio = 16 },
	{ .refclk = 38400, .cdclk = 556800, .divider = 2, .ratio = 29 },
	{ .refclk = 38400, .cdclk = 652800, .divider = 2, .ratio = 34 },
	{}
};

static int bxt_calc_cdclk(struct drm_i915_private *dev_priv, int min_cdclk)
{
	const struct intel_cdclk_vals *table = dev_priv->cdclk.table;
	int i;

	for (i = 0; table[i].refclk; i++)
		if (table[i].refclk == dev_priv->cdclk.hw.ref &&
		    table[i].cdclk >= min_cdclk)
			return table[i].cdclk;

	drm_WARN(&dev_priv->drm, 1,
		 "Cannot satisfy minimum cdclk %d with refclk %u\n",
		 min_cdclk, dev_priv->cdclk.hw.ref);
	return 0;
}

static int bxt_calc_cdclk_pll_vco(struct drm_i915_private *dev_priv, int cdclk)
{
	const struct intel_cdclk_vals *table = dev_priv->cdclk.table;
	int i;

	if (cdclk == dev_priv->cdclk.hw.bypass)
		return 0;

	for (i = 0; table[i].refclk; i++)
		if (table[i].refclk == dev_priv->cdclk.hw.ref &&
		    table[i].cdclk == cdclk)
			return dev_priv->cdclk.hw.ref * table[i].ratio;

	drm_WARN(&dev_priv->drm, 1, "cdclk %d not valid for refclk %u\n",
		 cdclk, dev_priv->cdclk.hw.ref);
	return 0;
}

static u8 bxt_calc_voltage_level(int cdclk)
{
	return DIV_ROUND_UP(cdclk, 25000);
}

static u8 cnl_calc_voltage_level(int cdclk)
{
	if (cdclk > 336000)
		return 2;
	else if (cdclk > 168000)
		return 1;
	else
		return 0;
}

static u8 icl_calc_voltage_level(int cdclk)
{
	if (cdclk > 556800)
		return 2;
	else if (cdclk > 312000)
		return 1;
	else
		return 0;
}

static u8 ehl_calc_voltage_level(int cdclk)
{
	if (cdclk > 326400)
		return 3;
	else if (cdclk > 312000)
		return 2;
	else if (cdclk > 180000)
		return 1;
	else
		return 0;
}

static u8 tgl_calc_voltage_level(int cdclk)
{
	if (cdclk > 556800)
		return 3;
	else if (cdclk > 326400)
		return 2;
	else if (cdclk > 312000)
		return 1;
	else
		return 0;
}

static void cnl_readout_refclk(struct drm_i915_private *dev_priv,
			       struct intel_cdclk_config *cdclk_config)
{
	if (intel_de_read(dev_priv, SKL_DSSM) & CNL_DSSM_CDCLK_PLL_REFCLK_24MHz)
		cdclk_config->ref = 24000;
	else
		cdclk_config->ref = 19200;
}

static void icl_readout_refclk(struct drm_i915_private *dev_priv,
			       struct intel_cdclk_config *cdclk_config)
{
	u32 dssm = intel_de_read(dev_priv, SKL_DSSM) & ICL_DSSM_CDCLK_PLL_REFCLK_MASK;

	switch (dssm) {
	default:
		MISSING_CASE(dssm);
		fallthrough;
	case ICL_DSSM_CDCLK_PLL_REFCLK_24MHz:
		cdclk_config->ref = 24000;
		break;
	case ICL_DSSM_CDCLK_PLL_REFCLK_19_2MHz:
		cdclk_config->ref = 19200;
		break;
	case ICL_DSSM_CDCLK_PLL_REFCLK_38_4MHz:
		cdclk_config->ref = 38400;
		break;
	}
}

static void bxt_de_pll_readout(struct drm_i915_private *dev_priv,
			       struct intel_cdclk_config *cdclk_config)
{
	u32 val, ratio;

	if (DISPLAY_VER(dev_priv) >= 11)
		icl_readout_refclk(dev_priv, cdclk_config);
	else if (IS_CANNONLAKE(dev_priv))
		cnl_readout_refclk(dev_priv, cdclk_config);
	else
		cdclk_config->ref = 19200;

	val = intel_de_read(dev_priv, BXT_DE_PLL_ENABLE);
	if ((val & BXT_DE_PLL_PLL_ENABLE) == 0 ||
	    (val & BXT_DE_PLL_LOCK) == 0) {
		/*
		 * CDCLK PLL is disabled, the VCO/ratio doesn't matter, but
		 * setting it to zero is a way to signal that.
		 */
		cdclk_config->vco = 0;
		return;
	}

	/*
	 * CNL+ have the ratio directly in the PLL enable register, gen9lp had
	 * it in a separate PLL control register.
	 */
	if (DISPLAY_VER(dev_priv) >= 11 || IS_CANNONLAKE(dev_priv))
		ratio = val & CNL_CDCLK_PLL_RATIO_MASK;
	else
		ratio = intel_de_read(dev_priv, BXT_DE_PLL_CTL) & BXT_DE_PLL_RATIO_MASK;

	cdclk_config->vco = ratio * cdclk_config->ref;
}

static void bxt_get_cdclk(struct drm_i915_private *dev_priv,
			  struct intel_cdclk_config *cdclk_config)
{
	u32 divider;
	int div;

	bxt_de_pll_readout(dev_priv, cdclk_config);

	if (DISPLAY_VER(dev_priv) >= 12)
		cdclk_config->bypass = cdclk_config->ref / 2;
	else if (DISPLAY_VER(dev_priv) >= 11)
		cdclk_config->bypass = 50000;
	else
		cdclk_config->bypass = cdclk_config->ref;

	if (cdclk_config->vco == 0) {
		cdclk_config->cdclk = cdclk_config->bypass;
		goto out;
	}

	divider = intel_de_read(dev_priv, CDCLK_CTL) & BXT_CDCLK_CD2X_DIV_SEL_MASK;

	switch (divider) {
	case BXT_CDCLK_CD2X_DIV_SEL_1:
		div = 2;
		break;
	case BXT_CDCLK_CD2X_DIV_SEL_1_5:
		div = 3;
		break;
	case BXT_CDCLK_CD2X_DIV_SEL_2:
		div = 4;
		break;
	case BXT_CDCLK_CD2X_DIV_SEL_4:
		div = 8;
		break;
	default:
		MISSING_CASE(divider);
		return;
	}

	cdclk_config->cdclk = DIV_ROUND_CLOSEST(cdclk_config->vco, div);

 out:
	/*
	 * Can't read this out :( Let's assume it's
	 * at least what the CDCLK frequency requires.
	 */
	cdclk_config->voltage_level =
		dev_priv->display.calc_voltage_level(cdclk_config->cdclk);
}

static void bxt_de_pll_disable(struct drm_i915_private *dev_priv)
{
	intel_de_write(dev_priv, BXT_DE_PLL_ENABLE, 0);

	/* Timeout 200us */
	if (intel_de_wait_for_clear(dev_priv,
				    BXT_DE_PLL_ENABLE, BXT_DE_PLL_LOCK, 1))
		drm_err(&dev_priv->drm, "timeout waiting for DE PLL unlock\n");

	dev_priv->cdclk.hw.vco = 0;
}

static void bxt_de_pll_enable(struct drm_i915_private *dev_priv, int vco)
{
	int ratio = DIV_ROUND_CLOSEST(vco, dev_priv->cdclk.hw.ref);

	intel_de_rmw(dev_priv, BXT_DE_PLL_CTL,
		     BXT_DE_PLL_RATIO_MASK, BXT_DE_PLL_RATIO(ratio));

	intel_de_write(dev_priv, BXT_DE_PLL_ENABLE, BXT_DE_PLL_PLL_ENABLE);

	/* Timeout 200us */
	if (intel_de_wait_for_set(dev_priv,
				  BXT_DE_PLL_ENABLE, BXT_DE_PLL_LOCK, 1))
		drm_err(&dev_priv->drm, "timeout waiting for DE PLL lock\n");

	dev_priv->cdclk.hw.vco = vco;
}

static void cnl_cdclk_pll_disable(struct drm_i915_private *dev_priv)
{
	intel_de_rmw(dev_priv, BXT_DE_PLL_ENABLE,
		     BXT_DE_PLL_PLL_ENABLE, 0);

	/* Timeout 200us */
	if (intel_de_wait_for_clear(dev_priv, BXT_DE_PLL_ENABLE, BXT_DE_PLL_LOCK, 1))
		drm_err(&dev_priv->drm, "timeout waiting for CDCLK PLL unlock\n");

	dev_priv->cdclk.hw.vco = 0;
}

static void cnl_cdclk_pll_enable(struct drm_i915_private *dev_priv, int vco)
{
	int ratio = DIV_ROUND_CLOSEST(vco, dev_priv->cdclk.hw.ref);
	u32 val;

	val = CNL_CDCLK_PLL_RATIO(ratio);
	intel_de_write(dev_priv, BXT_DE_PLL_ENABLE, val);

	val |= BXT_DE_PLL_PLL_ENABLE;
	intel_de_write(dev_priv, BXT_DE_PLL_ENABLE, val);

	/* Timeout 200us */
	if (intel_de_wait_for_set(dev_priv, BXT_DE_PLL_ENABLE, BXT_DE_PLL_LOCK, 1))
		drm_err(&dev_priv->drm, "timeout waiting for CDCLK PLL lock\n");

	dev_priv->cdclk.hw.vco = vco;
}

static bool has_cdclk_crawl(struct drm_i915_private *i915)
{
	return INTEL_INFO(i915)->has_cdclk_crawl;
}

static void adlp_cdclk_pll_crawl(struct drm_i915_private *dev_priv, int vco)
{
	int ratio = DIV_ROUND_CLOSEST(vco, dev_priv->cdclk.hw.ref);
	u32 val;

	/* Write PLL ratio without disabling */
	val = CNL_CDCLK_PLL_RATIO(ratio) | BXT_DE_PLL_PLL_ENABLE;
	intel_de_write(dev_priv, BXT_DE_PLL_ENABLE, val);

	/* Submit freq change request */
	val |= BXT_DE_PLL_FREQ_REQ;
	intel_de_write(dev_priv, BXT_DE_PLL_ENABLE, val);

	/* Timeout 200us */
	if (intel_de_wait_for_set(dev_priv, BXT_DE_PLL_ENABLE,
				  BXT_DE_PLL_LOCK | BXT_DE_PLL_FREQ_REQ_ACK, 1))
		DRM_ERROR("timeout waiting for FREQ change request ack\n");

	val &= ~BXT_DE_PLL_FREQ_REQ;
	intel_de_write(dev_priv, BXT_DE_PLL_ENABLE, val);

	dev_priv->cdclk.hw.vco = vco;
}

static u32 bxt_cdclk_cd2x_pipe(struct drm_i915_private *dev_priv, enum pipe pipe)
{
	if (DISPLAY_VER(dev_priv) >= 12) {
		if (pipe == INVALID_PIPE)
			return TGL_CDCLK_CD2X_PIPE_NONE;
		else
			return TGL_CDCLK_CD2X_PIPE(pipe);
	} else if (DISPLAY_VER(dev_priv) >= 11) {
		if (pipe == INVALID_PIPE)
			return ICL_CDCLK_CD2X_PIPE_NONE;
		else
			return ICL_CDCLK_CD2X_PIPE(pipe);
	} else {
		if (pipe == INVALID_PIPE)
			return BXT_CDCLK_CD2X_PIPE_NONE;
		else
			return BXT_CDCLK_CD2X_PIPE(pipe);
	}
}

static u32 bxt_cdclk_cd2x_div_sel(struct drm_i915_private *dev_priv,
				  int cdclk, int vco)
{
	/* cdclk = vco / 2 / div{1,1.5,2,4} */
	switch (DIV_ROUND_CLOSEST(vco, cdclk)) {
	default:
		drm_WARN_ON(&dev_priv->drm,
			    cdclk != dev_priv->cdclk.hw.bypass);
		drm_WARN_ON(&dev_priv->drm, vco != 0);
		fallthrough;
	case 2:
		return BXT_CDCLK_CD2X_DIV_SEL_1;
	case 3:
		return BXT_CDCLK_CD2X_DIV_SEL_1_5;
	case 4:
		return BXT_CDCLK_CD2X_DIV_SEL_2;
	case 8:
		return BXT_CDCLK_CD2X_DIV_SEL_4;
	}
}

static void bxt_set_cdclk(struct drm_i915_private *dev_priv,
			  const struct intel_cdclk_config *cdclk_config,
			  enum pipe pipe)
{
	int cdclk = cdclk_config->cdclk;
	int vco = cdclk_config->vco;
	u32 val;
	int ret;

	/* Inform power controller of upcoming frequency change. */
	if (DISPLAY_VER(dev_priv) >= 11 || IS_CANNONLAKE(dev_priv))
		ret = skl_pcode_request(dev_priv, SKL_PCODE_CDCLK_CONTROL,
					SKL_CDCLK_PREPARE_FOR_CHANGE,
					SKL_CDCLK_READY_FOR_CHANGE,
					SKL_CDCLK_READY_FOR_CHANGE, 3);
	else
		/*
		 * BSpec requires us to wait up to 150usec, but that leads to
		 * timeouts; the 2ms used here is based on experiment.
		 */
		ret = sandybridge_pcode_write_timeout(dev_priv,
						      HSW_PCODE_DE_WRITE_FREQ_REQ,
						      0x80000000, 150, 2);

	if (ret) {
		drm_err(&dev_priv->drm,
			"Failed to inform PCU about cdclk change (err %d, freq %d)\n",
			ret, cdclk);
		return;
	}

	if (has_cdclk_crawl(dev_priv) && dev_priv->cdclk.hw.vco > 0 && vco > 0) {
		if (dev_priv->cdclk.hw.vco != vco)
			adlp_cdclk_pll_crawl(dev_priv, vco);
	} else if (DISPLAY_VER(dev_priv) >= 11 || IS_CANNONLAKE(dev_priv)) {
		if (dev_priv->cdclk.hw.vco != 0 &&
		    dev_priv->cdclk.hw.vco != vco)
			cnl_cdclk_pll_disable(dev_priv);

		if (dev_priv->cdclk.hw.vco != vco)
			cnl_cdclk_pll_enable(dev_priv, vco);
	} else {
		if (dev_priv->cdclk.hw.vco != 0 &&
		    dev_priv->cdclk.hw.vco != vco)
			bxt_de_pll_disable(dev_priv);

		if (dev_priv->cdclk.hw.vco != vco)
			bxt_de_pll_enable(dev_priv, vco);
	}

	val = bxt_cdclk_cd2x_div_sel(dev_priv, cdclk, vco) |
		bxt_cdclk_cd2x_pipe(dev_priv, pipe) |
		skl_cdclk_decimal(cdclk);

	/*
	 * Disable SSA Precharge when CD clock frequency < 500 MHz,
	 * enable otherwise.
	 */
	if ((IS_GEMINILAKE(dev_priv) || IS_BROXTON(dev_priv)) &&
	    cdclk >= 500000)
		val |= BXT_CDCLK_SSA_PRECHARGE_ENABLE;
	intel_de_write(dev_priv, CDCLK_CTL, val);

	if (pipe != INVALID_PIPE)
		intel_wait_for_vblank(dev_priv, pipe);

	if (DISPLAY_VER(dev_priv) >= 11 || IS_CANNONLAKE(dev_priv)) {
		ret = sandybridge_pcode_write(dev_priv, SKL_PCODE_CDCLK_CONTROL,
					      cdclk_config->voltage_level);
	} else {
		/*
		 * The timeout isn't specified, the 2ms used here is based on
		 * experiment.
		 * FIXME: Waiting for the request completion could be delayed
		 * until the next PCODE request based on BSpec.
		 */
		ret = sandybridge_pcode_write_timeout(dev_priv,
						      HSW_PCODE_DE_WRITE_FREQ_REQ,
						      cdclk_config->voltage_level,
						      150, 2);
	}

	if (ret) {
		drm_err(&dev_priv->drm,
			"PCode CDCLK freq set failed, (err %d, freq %d)\n",
			ret, cdclk);
		return;
	}

	intel_update_cdclk(dev_priv);

	if (DISPLAY_VER(dev_priv) >= 11 || IS_CANNONLAKE(dev_priv))
		/*
		 * Can't read out the voltage level :(
		 * Let's just assume everything is as expected.
		 */
		dev_priv->cdclk.hw.voltage_level = cdclk_config->voltage_level;
}

static void bxt_sanitize_cdclk(struct drm_i915_private *dev_priv)
{
	u32 cdctl, expected;
	int cdclk, vco;

	intel_update_cdclk(dev_priv);
	intel_dump_cdclk_config(&dev_priv->cdclk.hw, "Current CDCLK");

	if (dev_priv->cdclk.hw.vco == 0 ||
	    dev_priv->cdclk.hw.cdclk == dev_priv->cdclk.hw.bypass)
		goto sanitize;

	/* DPLL okay; verify the cdclock
	 *
	 * Some BIOS versions leave an incorrect decimal frequency value and
	 * set reserved MBZ bits in CDCLK_CTL at least during exiting from S4,
	 * so sanitize this register.
	 */
	cdctl = intel_de_read(dev_priv, CDCLK_CTL);
	/*
	 * Let's ignore the pipe field, since BIOS could have configured the
	 * dividers both synching to an active pipe, or asynchronously
	 * (PIPE_NONE).
	 */
	cdctl &= ~bxt_cdclk_cd2x_pipe(dev_priv, INVALID_PIPE);

	/* Make sure this is a legal cdclk value for the platform */
	cdclk = bxt_calc_cdclk(dev_priv, dev_priv->cdclk.hw.cdclk);
	if (cdclk != dev_priv->cdclk.hw.cdclk)
		goto sanitize;

	/* Make sure the VCO is correct for the cdclk */
	vco = bxt_calc_cdclk_pll_vco(dev_priv, cdclk);
	if (vco != dev_priv->cdclk.hw.vco)
		goto sanitize;

	expected = skl_cdclk_decimal(cdclk);

	/* Figure out what CD2X divider we should be using for this cdclk */
	expected |= bxt_cdclk_cd2x_div_sel(dev_priv,
					   dev_priv->cdclk.hw.cdclk,
					   dev_priv->cdclk.hw.vco);

	/*
	 * Disable SSA Precharge when CD clock frequency < 500 MHz,
	 * enable otherwise.
	 */
	if ((IS_GEMINILAKE(dev_priv) || IS_BROXTON(dev_priv)) &&
	    dev_priv->cdclk.hw.cdclk >= 500000)
		expected |= BXT_CDCLK_SSA_PRECHARGE_ENABLE;

	if (cdctl == expected)
		/* All well; nothing to sanitize */
		return;

sanitize:
	drm_dbg_kms(&dev_priv->drm, "Sanitizing cdclk programmed by pre-os\n");

	/* force cdclk programming */
	dev_priv->cdclk.hw.cdclk = 0;

	/* force full PLL disable + enable */
	dev_priv->cdclk.hw.vco = -1;
}

static void bxt_cdclk_init_hw(struct drm_i915_private *dev_priv)
{
	struct intel_cdclk_config cdclk_config;

	bxt_sanitize_cdclk(dev_priv);

	if (dev_priv->cdclk.hw.cdclk != 0 &&
	    dev_priv->cdclk.hw.vco != 0)
		return;

	cdclk_config = dev_priv->cdclk.hw;

	/*
	 * FIXME:
	 * - The initial CDCLK needs to be read from VBT.
	 *   Need to make this change after VBT has changes for BXT.
	 */
	cdclk_config.cdclk = bxt_calc_cdclk(dev_priv, 0);
	cdclk_config.vco = bxt_calc_cdclk_pll_vco(dev_priv, cdclk_config.cdclk);
	cdclk_config.voltage_level =
		dev_priv->display.calc_voltage_level(cdclk_config.cdclk);

	bxt_set_cdclk(dev_priv, &cdclk_config, INVALID_PIPE);
}

static void bxt_cdclk_uninit_hw(struct drm_i915_private *dev_priv)
{
	struct intel_cdclk_config cdclk_config = dev_priv->cdclk.hw;

	cdclk_config.cdclk = cdclk_config.bypass;
	cdclk_config.vco = 0;
	cdclk_config.voltage_level =
		dev_priv->display.calc_voltage_level(cdclk_config.cdclk);

	bxt_set_cdclk(dev_priv, &cdclk_config, INVALID_PIPE);
}

/**
 * intel_cdclk_init_hw - Initialize CDCLK hardware
 * @i915: i915 device
 *
 * Initialize CDCLK. This consists mainly of initializing dev_priv->cdclk.hw and
 * sanitizing the state of the hardware if needed. This is generally done only
 * during the display core initialization sequence, after which the DMC will
 * take care of turning CDCLK off/on as needed.
 */
void intel_cdclk_init_hw(struct drm_i915_private *i915)
{
	if (DISPLAY_VER(i915) >= 10 || IS_BROXTON(i915))
		bxt_cdclk_init_hw(i915);
	else if (DISPLAY_VER(i915) == 9)
		skl_cdclk_init_hw(i915);
}

/**
 * intel_cdclk_uninit_hw - Uninitialize CDCLK hardware
 * @i915: i915 device
 *
 * Uninitialize CDCLK. This is done only during the display core
 * uninitialization sequence.
 */
void intel_cdclk_uninit_hw(struct drm_i915_private *i915)
{
	if (DISPLAY_VER(i915) >= 10 || IS_BROXTON(i915))
		bxt_cdclk_uninit_hw(i915);
	else if (DISPLAY_VER(i915) == 9)
		skl_cdclk_uninit_hw(i915);
}

static bool intel_cdclk_can_crawl(struct drm_i915_private *dev_priv,
				  const struct intel_cdclk_config *a,
				  const struct intel_cdclk_config *b)
{
	int a_div, b_div;

	if (!has_cdclk_crawl(dev_priv))
		return false;

	/*
	 * The vco and cd2x divider will change independently
	 * from each, so we disallow cd2x change when crawling.
	 */
	a_div = DIV_ROUND_CLOSEST(a->vco, a->cdclk);
	b_div = DIV_ROUND_CLOSEST(b->vco, b->cdclk);

	return a->vco != 0 && b->vco != 0 &&
		a->vco != b->vco &&
		a_div == b_div &&
		a->ref == b->ref;
}

/**
 * intel_cdclk_needs_modeset - Determine if changong between the CDCLK
 *                             configurations requires a modeset on all pipes
 * @a: first CDCLK configuration
 * @b: second CDCLK configuration
 *
 * Returns:
 * True if changing between the two CDCLK configurations
 * requires all pipes to be off, false if not.
 */
bool intel_cdclk_needs_modeset(const struct intel_cdclk_config *a,
			       const struct intel_cdclk_config *b)
{
	return a->cdclk != b->cdclk ||
		a->vco != b->vco ||
		a->ref != b->ref;
}

/**
 * intel_cdclk_can_cd2x_update - Determine if changing between the two CDCLK
 *                               configurations requires only a cd2x divider update
 * @dev_priv: i915 device
 * @a: first CDCLK configuration
 * @b: second CDCLK configuration
 *
 * Returns:
 * True if changing between the two CDCLK configurations
 * can be done with just a cd2x divider update, false if not.
 */
static bool intel_cdclk_can_cd2x_update(struct drm_i915_private *dev_priv,
					const struct intel_cdclk_config *a,
					const struct intel_cdclk_config *b)
{
	/* Older hw doesn't have the capability */
	if (DISPLAY_VER(dev_priv) < 10 && !IS_BROXTON(dev_priv))
		return false;

	return a->cdclk != b->cdclk &&
		a->vco == b->vco &&
		a->ref == b->ref;
}

/**
 * intel_cdclk_changed - Determine if two CDCLK configurations are different
 * @a: first CDCLK configuration
 * @b: second CDCLK configuration
 *
 * Returns:
 * True if the CDCLK configurations don't match, false if they do.
 */
static bool intel_cdclk_changed(const struct intel_cdclk_config *a,
				const struct intel_cdclk_config *b)
{
	return intel_cdclk_needs_modeset(a, b) ||
		a->voltage_level != b->voltage_level;
}

void intel_dump_cdclk_config(const struct intel_cdclk_config *cdclk_config,
			     const char *context)
{
	DRM_DEBUG_DRIVER("%s %d kHz, VCO %d kHz, ref %d kHz, bypass %d kHz, voltage level %d\n",
			 context, cdclk_config->cdclk, cdclk_config->vco,
			 cdclk_config->ref, cdclk_config->bypass,
			 cdclk_config->voltage_level);
}

/**
 * intel_set_cdclk - Push the CDCLK configuration to the hardware
 * @dev_priv: i915 device
 * @cdclk_config: new CDCLK configuration
 * @pipe: pipe with which to synchronize the update
 *
 * Program the hardware based on the passed in CDCLK state,
 * if necessary.
 */
static void intel_set_cdclk(struct drm_i915_private *dev_priv,
			    const struct intel_cdclk_config *cdclk_config,
			    enum pipe pipe)
{
	struct intel_encoder *encoder;

	if (!intel_cdclk_changed(&dev_priv->cdclk.hw, cdclk_config))
		return;

	if (drm_WARN_ON_ONCE(&dev_priv->drm, !dev_priv->display.set_cdclk))
		return;

	intel_dump_cdclk_config(cdclk_config, "Changing CDCLK to");

	for_each_intel_encoder_with_psr(&dev_priv->drm, encoder) {
		struct intel_dp *intel_dp = enc_to_intel_dp(encoder);

		intel_psr_pause(intel_dp);
	}

	/*
	 * Lock aux/gmbus while we change cdclk in case those
	 * functions use cdclk. Not all platforms/ports do,
	 * but we'll lock them all for simplicity.
	 */
	mutex_lock(&dev_priv->gmbus_mutex);
	for_each_intel_dp(&dev_priv->drm, encoder) {
		struct intel_dp *intel_dp = enc_to_intel_dp(encoder);

		mutex_lock_nest_lock(&intel_dp->aux.hw_mutex,
				     &dev_priv->gmbus_mutex);
	}

	dev_priv->display.set_cdclk(dev_priv, cdclk_config, pipe);

	for_each_intel_dp(&dev_priv->drm, encoder) {
		struct intel_dp *intel_dp = enc_to_intel_dp(encoder);

		mutex_unlock(&intel_dp->aux.hw_mutex);
	}
	mutex_unlock(&dev_priv->gmbus_mutex);

	for_each_intel_encoder_with_psr(&dev_priv->drm, encoder) {
		struct intel_dp *intel_dp = enc_to_intel_dp(encoder);

		intel_psr_resume(intel_dp);
	}

	if (drm_WARN(&dev_priv->drm,
		     intel_cdclk_changed(&dev_priv->cdclk.hw, cdclk_config),
		     "cdclk state doesn't match!\n")) {
		intel_dump_cdclk_config(&dev_priv->cdclk.hw, "[hw state]");
		intel_dump_cdclk_config(cdclk_config, "[sw state]");
	}
}

/**
 * intel_set_cdclk_pre_plane_update - Push the CDCLK state to the hardware
 * @state: intel atomic state
 *
 * Program the hardware before updating the HW plane state based on the
 * new CDCLK state, if necessary.
 */
void
intel_set_cdclk_pre_plane_update(struct intel_atomic_state *state)
{
	struct drm_i915_private *dev_priv = to_i915(state->base.dev);
	const struct intel_cdclk_state *old_cdclk_state =
		intel_atomic_get_old_cdclk_state(state);
	const struct intel_cdclk_state *new_cdclk_state =
		intel_atomic_get_new_cdclk_state(state);
	enum pipe pipe = new_cdclk_state->pipe;

	if (!intel_cdclk_changed(&old_cdclk_state->actual,
				 &new_cdclk_state->actual))
		return;

	if (pipe == INVALID_PIPE ||
	    old_cdclk_state->actual.cdclk <= new_cdclk_state->actual.cdclk) {
		drm_WARN_ON(&dev_priv->drm, !new_cdclk_state->base.changed);

		intel_set_cdclk(dev_priv, &new_cdclk_state->actual, pipe);
	}
}

/**
 * intel_set_cdclk_post_plane_update - Push the CDCLK state to the hardware
 * @state: intel atomic state
 *
 * Program the hardware after updating the HW plane state based on the
 * new CDCLK state, if necessary.
 */
void
intel_set_cdclk_post_plane_update(struct intel_atomic_state *state)
{
	struct drm_i915_private *dev_priv = to_i915(state->base.dev);
	const struct intel_cdclk_state *old_cdclk_state =
		intel_atomic_get_old_cdclk_state(state);
	const struct intel_cdclk_state *new_cdclk_state =
		intel_atomic_get_new_cdclk_state(state);
	enum pipe pipe = new_cdclk_state->pipe;

	if (!intel_cdclk_changed(&old_cdclk_state->actual,
				 &new_cdclk_state->actual))
		return;

	if (pipe != INVALID_PIPE &&
	    old_cdclk_state->actual.cdclk > new_cdclk_state->actual.cdclk) {
		drm_WARN_ON(&dev_priv->drm, !new_cdclk_state->base.changed);

		intel_set_cdclk(dev_priv, &new_cdclk_state->actual, pipe);
	}
}

static int intel_pixel_rate_to_cdclk(const struct intel_crtc_state *crtc_state)
{
	struct drm_i915_private *dev_priv = to_i915(crtc_state->uapi.crtc->dev);
	int pixel_rate = crtc_state->pixel_rate;

	if (DISPLAY_VER(dev_priv) >= 10)
		return DIV_ROUND_UP(pixel_rate, 2);
	else if (DISPLAY_VER(dev_priv) == 9 ||
		 IS_BROADWELL(dev_priv) || IS_HASWELL(dev_priv))
		return pixel_rate;
	else if (IS_CHERRYVIEW(dev_priv))
		return DIV_ROUND_UP(pixel_rate * 100, 95);
	else if (crtc_state->double_wide)
		return DIV_ROUND_UP(pixel_rate * 100, 90 * 2);
	else
		return DIV_ROUND_UP(pixel_rate * 100, 90);
}

static int intel_planes_min_cdclk(const struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	struct intel_plane *plane;
	int min_cdclk = 0;

	for_each_intel_plane_on_crtc(&dev_priv->drm, crtc, plane)
		min_cdclk = max(crtc_state->min_cdclk[plane->id], min_cdclk);

	return min_cdclk;
}

int intel_crtc_compute_min_cdclk(const struct intel_crtc_state *crtc_state)
{
	struct drm_i915_private *dev_priv =
		to_i915(crtc_state->uapi.crtc->dev);
	int min_cdclk;

	if (!crtc_state->hw.enable)
		return 0;

	min_cdclk = intel_pixel_rate_to_cdclk(crtc_state);

	/* pixel rate mustn't exceed 95% of cdclk with IPS on BDW */
	if (IS_BROADWELL(dev_priv) && hsw_crtc_state_ips_capable(crtc_state))
		min_cdclk = DIV_ROUND_UP(min_cdclk * 100, 95);

	/* BSpec says "Do not use DisplayPort with CDCLK less than 432 MHz,
	 * audio enabled, port width x4, and link rate HBR2 (5.4 GHz), or else
	 * there may be audio corruption or screen corruption." This cdclk
	 * restriction for GLK is 316.8 MHz.
	 */
	if (intel_crtc_has_dp_encoder(crtc_state) &&
	    crtc_state->has_audio &&
	    crtc_state->port_clock >= 540000 &&
	    crtc_state->lane_count == 4) {
		if (DISPLAY_VER(dev_priv) == 10) {
			/* Display WA #1145: glk,cnl */
			min_cdclk = max(316800, min_cdclk);
		} else if (DISPLAY_VER(dev_priv) == 9 || IS_BROADWELL(dev_priv)) {
			/* Display WA #1144: skl,bxt */
			min_cdclk = max(432000, min_cdclk);
		}
	}

	/*
	 * According to BSpec, "The CD clock frequency must be at least twice
	 * the frequency of the Azalia BCLK." and BCLK is 96 MHz by default.
	 */
	if (crtc_state->has_audio && DISPLAY_VER(dev_priv) >= 9)
		min_cdclk = max(2 * 96000, min_cdclk);

	/*
	 * "For DP audio configuration, cdclk frequency shall be set to
	 *  meet the following requirements:
	 *  DP Link Frequency(MHz) | Cdclk frequency(MHz)
	 *  270                    | 320 or higher
	 *  162                    | 200 or higher"
	 */
	if ((IS_VALLEYVIEW(dev_priv) || IS_CHERRYVIEW(dev_priv)) &&
	    intel_crtc_has_dp_encoder(crtc_state) && crtc_state->has_audio)
		min_cdclk = max(crtc_state->port_clock, min_cdclk);

	/*
	 * On Valleyview some DSI panels lose (v|h)sync when the clock is lower
	 * than 320000KHz.
	 */
	if (intel_crtc_has_type(crtc_state, INTEL_OUTPUT_DSI) &&
	    IS_VALLEYVIEW(dev_priv))
		min_cdclk = max(320000, min_cdclk);

	/*
	 * On Geminilake once the CDCLK gets as low as 79200
	 * picture gets unstable, despite that values are
	 * correct for DSI PLL and DE PLL.
	 */
	if (intel_crtc_has_type(crtc_state, INTEL_OUTPUT_DSI) &&
	    IS_GEMINILAKE(dev_priv))
		min_cdclk = max(158400, min_cdclk);

	/* Account for additional needs from the planes */
	min_cdclk = max(intel_planes_min_cdclk(crtc_state), min_cdclk);

	/*
	 * HACK. Currently for TGL platforms we calculate
	 * min_cdclk initially based on pixel_rate divided
	 * by 2, accounting for also plane requirements,
	 * however in some cases the lowest possible CDCLK
	 * doesn't work and causing the underruns.
	 * Explicitly stating here that this seems to be currently
	 * rather a Hack, than final solution.
	 */
	if (IS_TIGERLAKE(dev_priv)) {
		/*
		 * Clamp to max_cdclk_freq in case pixel rate is higher,
		 * in order not to break an 8K, but still leave W/A at place.
		 */
		min_cdclk = max_t(int, min_cdclk,
				  min_t(int, crtc_state->pixel_rate,
					dev_priv->max_cdclk_freq));
	}

	if (min_cdclk > dev_priv->max_cdclk_freq) {
		drm_dbg_kms(&dev_priv->drm,
			    "required cdclk (%d kHz) exceeds max (%d kHz)\n",
			    min_cdclk, dev_priv->max_cdclk_freq);
		return -EINVAL;
	}

	return min_cdclk;
}

static int intel_compute_min_cdclk(struct intel_cdclk_state *cdclk_state)
{
	struct intel_atomic_state *state = cdclk_state->base.state;
	struct drm_i915_private *dev_priv = to_i915(state->base.dev);
	struct intel_bw_state *bw_state = NULL;
	struct intel_crtc *crtc;
	struct intel_crtc_state *crtc_state;
	int min_cdclk, i;
	enum pipe pipe;

	for_each_new_intel_crtc_in_state(state, crtc, crtc_state, i) {
		int ret;

		min_cdclk = intel_crtc_compute_min_cdclk(crtc_state);
		if (min_cdclk < 0)
			return min_cdclk;

		bw_state = intel_atomic_get_bw_state(state);
		if (IS_ERR(bw_state))
			return PTR_ERR(bw_state);

		if (cdclk_state->min_cdclk[crtc->pipe] == min_cdclk)
			continue;

		cdclk_state->min_cdclk[crtc->pipe] = min_cdclk;

		ret = intel_atomic_lock_global_state(&cdclk_state->base);
		if (ret)
			return ret;
	}

	min_cdclk = cdclk_state->force_min_cdclk;
	for_each_pipe(dev_priv, pipe) {
		min_cdclk = max(cdclk_state->min_cdclk[pipe], min_cdclk);

		if (!bw_state)
			continue;

		min_cdclk = max(bw_state->min_cdclk, min_cdclk);
	}

	return min_cdclk;
}

/*
 * Account for port clock min voltage level requirements.
 * This only really does something on CNL+ but can be
 * called on earlier platforms as well.
 *
 * Note that this functions assumes that 0 is
 * the lowest voltage value, and higher values
 * correspond to increasingly higher voltages.
 *
 * Should that relationship no longer hold on
 * future platforms this code will need to be
 * adjusted.
 */
static int bxt_compute_min_voltage_level(struct intel_cdclk_state *cdclk_state)
{
	struct intel_atomic_state *state = cdclk_state->base.state;
	struct drm_i915_private *dev_priv = to_i915(state->base.dev);
	struct intel_crtc *crtc;
	struct intel_crtc_state *crtc_state;
	u8 min_voltage_level;
	int i;
	enum pipe pipe;

	for_each_new_intel_crtc_in_state(state, crtc, crtc_state, i) {
		int ret;

		if (crtc_state->hw.enable)
			min_voltage_level = crtc_state->min_voltage_level;
		else
			min_voltage_level = 0;

		if (cdclk_state->min_voltage_level[crtc->pipe] == min_voltage_level)
			continue;

		cdclk_state->min_voltage_level[crtc->pipe] = min_voltage_level;

		ret = intel_atomic_lock_global_state(&cdclk_state->base);
		if (ret)
			return ret;
	}

	min_voltage_level = 0;
	for_each_pipe(dev_priv, pipe)
		min_voltage_level = max(cdclk_state->min_voltage_level[pipe],
					min_voltage_level);

	return min_voltage_level;
}

static int vlv_modeset_calc_cdclk(struct intel_cdclk_state *cdclk_state)
{
	struct intel_atomic_state *state = cdclk_state->base.state;
	struct drm_i915_private *dev_priv = to_i915(state->base.dev);
	int min_cdclk, cdclk;

	min_cdclk = intel_compute_min_cdclk(cdclk_state);
	if (min_cdclk < 0)
		return min_cdclk;

	cdclk = vlv_calc_cdclk(dev_priv, min_cdclk);

	cdclk_state->logical.cdclk = cdclk;
	cdclk_state->logical.voltage_level =
		vlv_calc_voltage_level(dev_priv, cdclk);

	if (!cdclk_state->active_pipes) {
		cdclk = vlv_calc_cdclk(dev_priv, cdclk_state->force_min_cdclk);

		cdclk_state->actual.cdclk = cdclk;
		cdclk_state->actual.voltage_level =
			vlv_calc_voltage_level(dev_priv, cdclk);
	} else {
		cdclk_state->actual = cdclk_state->logical;
	}

	return 0;
}

static int bdw_modeset_calc_cdclk(struct intel_cdclk_state *cdclk_state)
{
	int min_cdclk, cdclk;

	min_cdclk = intel_compute_min_cdclk(cdclk_state);
	if (min_cdclk < 0)
		return min_cdclk;

	/*
	 * FIXME should also account for plane ratio
	 * once 64bpp pixel formats are supported.
	 */
	cdclk = bdw_calc_cdclk(min_cdclk);

	cdclk_state->logical.cdclk = cdclk;
	cdclk_state->logical.voltage_level =
		bdw_calc_voltage_level(cdclk);

	if (!cdclk_state->active_pipes) {
		cdclk = bdw_calc_cdclk(cdclk_state->force_min_cdclk);

		cdclk_state->actual.cdclk = cdclk;
		cdclk_state->actual.voltage_level =
			bdw_calc_voltage_level(cdclk);
	} else {
		cdclk_state->actual = cdclk_state->logical;
	}

	return 0;
}

static int skl_dpll0_vco(struct intel_cdclk_state *cdclk_state)
{
	struct intel_atomic_state *state = cdclk_state->base.state;
	struct drm_i915_private *dev_priv = to_i915(state->base.dev);
	struct intel_crtc *crtc;
	struct intel_crtc_state *crtc_state;
	int vco, i;

	vco = cdclk_state->logical.vco;
	if (!vco)
		vco = dev_priv->skl_preferred_vco_freq;

	for_each_new_intel_crtc_in_state(state, crtc, crtc_state, i) {
		if (!crtc_state->hw.enable)
			continue;

		if (!intel_crtc_has_type(crtc_state, INTEL_OUTPUT_EDP))
			continue;

		/*
		 * DPLL0 VCO may need to be adjusted to get the correct
		 * clock for eDP. This will affect cdclk as well.
		 */
		switch (crtc_state->port_clock / 2) {
		case 108000:
		case 216000:
			vco = 8640000;
			break;
		default:
			vco = 8100000;
			break;
		}
	}

	return vco;
}

static int skl_modeset_calc_cdclk(struct intel_cdclk_state *cdclk_state)
{
	int min_cdclk, cdclk, vco;

	min_cdclk = intel_compute_min_cdclk(cdclk_state);
	if (min_cdclk < 0)
		return min_cdclk;

	vco = skl_dpll0_vco(cdclk_state);

	/*
	 * FIXME should also account for plane ratio
	 * once 64bpp pixel formats are supported.
	 */
	cdclk = skl_calc_cdclk(min_cdclk, vco);

	cdclk_state->logical.vco = vco;
	cdclk_state->logical.cdclk = cdclk;
	cdclk_state->logical.voltage_level =
		skl_calc_voltage_level(cdclk);

	if (!cdclk_state->active_pipes) {
		cdclk = skl_calc_cdclk(cdclk_state->force_min_cdclk, vco);

		cdclk_state->actual.vco = vco;
		cdclk_state->actual.cdclk = cdclk;
		cdclk_state->actual.voltage_level =
			skl_calc_voltage_level(cdclk);
	} else {
		cdclk_state->actual = cdclk_state->logical;
	}

	return 0;
}

static int bxt_modeset_calc_cdclk(struct intel_cdclk_state *cdclk_state)
{
	struct intel_atomic_state *state = cdclk_state->base.state;
	struct drm_i915_private *dev_priv = to_i915(state->base.dev);
	int min_cdclk, min_voltage_level, cdclk, vco;

	min_cdclk = intel_compute_min_cdclk(cdclk_state);
	if (min_cdclk < 0)
		return min_cdclk;

	min_voltage_level = bxt_compute_min_voltage_level(cdclk_state);
	if (min_voltage_level < 0)
		return min_voltage_level;

	cdclk = bxt_calc_cdclk(dev_priv, min_cdclk);
	vco = bxt_calc_cdclk_pll_vco(dev_priv, cdclk);

	cdclk_state->logical.vco = vco;
	cdclk_state->logical.cdclk = cdclk;
	cdclk_state->logical.voltage_level =
		max_t(int, min_voltage_level,
		      dev_priv->display.calc_voltage_level(cdclk));

	if (!cdclk_state->active_pipes) {
		cdclk = bxt_calc_cdclk(dev_priv, cdclk_state->force_min_cdclk);
		vco = bxt_calc_cdclk_pll_vco(dev_priv, cdclk);

		cdclk_state->actual.vco = vco;
		cdclk_state->actual.cdclk = cdclk;
		cdclk_state->actual.voltage_level =
			dev_priv->display.calc_voltage_level(cdclk);
	} else {
		cdclk_state->actual = cdclk_state->logical;
	}

	return 0;
}

static int fixed_modeset_calc_cdclk(struct intel_cdclk_state *cdclk_state)
{
	int min_cdclk;

	/*
	 * We can't change the cdclk frequency, but we still want to
	 * check that the required minimum frequency doesn't exceed
	 * the actual cdclk frequency.
	 */
	min_cdclk = intel_compute_min_cdclk(cdclk_state);
	if (min_cdclk < 0)
		return min_cdclk;

	return 0;
}

static struct intel_global_state *intel_cdclk_duplicate_state(struct intel_global_obj *obj)
{
	struct intel_cdclk_state *cdclk_state;

	cdclk_state = kmemdup(obj->state, sizeof(*cdclk_state), GFP_KERNEL);
	if (!cdclk_state)
		return NULL;

	cdclk_state->pipe = INVALID_PIPE;

	return &cdclk_state->base;
}

static void intel_cdclk_destroy_state(struct intel_global_obj *obj,
				      struct intel_global_state *state)
{
	kfree(state);
}

static const struct intel_global_state_funcs intel_cdclk_funcs = {
	.atomic_duplicate_state = intel_cdclk_duplicate_state,
	.atomic_destroy_state = intel_cdclk_destroy_state,
};

struct intel_cdclk_state *
intel_atomic_get_cdclk_state(struct intel_atomic_state *state)
{
	struct drm_i915_private *dev_priv = to_i915(state->base.dev);
	struct intel_global_state *cdclk_state;

	cdclk_state = intel_atomic_get_global_obj_state(state, &dev_priv->cdclk.obj);
	if (IS_ERR(cdclk_state))
		return ERR_CAST(cdclk_state);

	return to_intel_cdclk_state(cdclk_state);
}

int intel_cdclk_init(struct drm_i915_private *dev_priv)
{
	struct intel_cdclk_state *cdclk_state;

	cdclk_state = kzalloc(sizeof(*cdclk_state), GFP_KERNEL);
	if (!cdclk_state)
		return -ENOMEM;

	intel_atomic_global_obj_init(dev_priv, &dev_priv->cdclk.obj,
				     &cdclk_state->base, &intel_cdclk_funcs);

	return 0;
}

int intel_modeset_calc_cdclk(struct intel_atomic_state *state)
{
	struct drm_i915_private *dev_priv = to_i915(state->base.dev);
	const struct intel_cdclk_state *old_cdclk_state;
	struct intel_cdclk_state *new_cdclk_state;
	enum pipe pipe = INVALID_PIPE;
	int ret;

	new_cdclk_state = intel_atomic_get_cdclk_state(state);
	if (IS_ERR(new_cdclk_state))
		return PTR_ERR(new_cdclk_state);

	old_cdclk_state = intel_atomic_get_old_cdclk_state(state);

	new_cdclk_state->active_pipes =
		intel_calc_active_pipes(state, old_cdclk_state->active_pipes);

	ret = dev_priv->display.modeset_calc_cdclk(new_cdclk_state);
	if (ret)
		return ret;

	if (intel_cdclk_changed(&old_cdclk_state->actual,
				&new_cdclk_state->actual)) {
		/*
		 * Also serialize commits across all crtcs
		 * if the actual hw needs to be poked.
		 */
		ret = intel_atomic_serialize_global_state(&new_cdclk_state->base);
		if (ret)
			return ret;
	} else if (old_cdclk_state->active_pipes != new_cdclk_state->active_pipes ||
		   old_cdclk_state->force_min_cdclk != new_cdclk_state->force_min_cdclk ||
		   intel_cdclk_changed(&old_cdclk_state->logical,
				       &new_cdclk_state->logical)) {
		ret = intel_atomic_lock_global_state(&new_cdclk_state->base);
		if (ret)
			return ret;
	} else {
		return 0;
	}

	if (is_power_of_2(new_cdclk_state->active_pipes) &&
	    intel_cdclk_can_cd2x_update(dev_priv,
					&old_cdclk_state->actual,
					&new_cdclk_state->actual)) {
		struct intel_crtc *crtc;
		struct intel_crtc_state *crtc_state;

		pipe = ilog2(new_cdclk_state->active_pipes);
		crtc = intel_get_crtc_for_pipe(dev_priv, pipe);

		crtc_state = intel_atomic_get_crtc_state(&state->base, crtc);
		if (IS_ERR(crtc_state))
			return PTR_ERR(crtc_state);

		if (drm_atomic_crtc_needs_modeset(&crtc_state->uapi))
			pipe = INVALID_PIPE;
	}

	if (intel_cdclk_can_crawl(dev_priv,
				  &old_cdclk_state->actual,
				  &new_cdclk_state->actual)) {
		drm_dbg_kms(&dev_priv->drm,
			    "Can change cdclk via crawl\n");
	} else if (pipe != INVALID_PIPE) {
		new_cdclk_state->pipe = pipe;

		drm_dbg_kms(&dev_priv->drm,
			    "Can change cdclk cd2x divider with pipe %c active\n",
			    pipe_name(pipe));
	} else if (intel_cdclk_needs_modeset(&old_cdclk_state->actual,
					     &new_cdclk_state->actual)) {
		/* All pipes must be switched off while we change the cdclk. */
		ret = intel_modeset_all_pipes(state);
		if (ret)
			return ret;

		drm_dbg_kms(&dev_priv->drm,
			    "Modeset required for cdclk change\n");
	}

	drm_dbg_kms(&dev_priv->drm,
		    "New cdclk calculated to be logical %u kHz, actual %u kHz\n",
		    new_cdclk_state->logical.cdclk,
		    new_cdclk_state->actual.cdclk);
	drm_dbg_kms(&dev_priv->drm,
		    "New voltage level calculated to be logical %u, actual %u\n",
		    new_cdclk_state->logical.voltage_level,
		    new_cdclk_state->actual.voltage_level);

	return 0;
}

static int intel_compute_max_dotclk(struct drm_i915_private *dev_priv)
{
	int max_cdclk_freq = dev_priv->max_cdclk_freq;

	if (DISPLAY_VER(dev_priv) >= 10)
		return 2 * max_cdclk_freq;
	else if (DISPLAY_VER(dev_priv) == 9 ||
		 IS_BROADWELL(dev_priv) || IS_HASWELL(dev_priv))
		return max_cdclk_freq;
	else if (IS_CHERRYVIEW(dev_priv))
		return max_cdclk_freq*95/100;
	else if (DISPLAY_VER(dev_priv) < 4)
		return 2*max_cdclk_freq*90/100;
	else
		return max_cdclk_freq*90/100;
}

/**
 * intel_update_max_cdclk - Determine the maximum support CDCLK frequency
 * @dev_priv: i915 device
 *
 * Determine the maximum CDCLK frequency the platform supports, and also
 * derive the maximum dot clock frequency the maximum CDCLK frequency
 * allows.
 */
void intel_update_max_cdclk(struct drm_i915_private *dev_priv)
{
	if (IS_JSL_EHL(dev_priv)) {
		if (dev_priv->cdclk.hw.ref == 24000)
			dev_priv->max_cdclk_freq = 552000;
		else
			dev_priv->max_cdclk_freq = 556800;
	} else if (DISPLAY_VER(dev_priv) >= 11) {
		if (dev_priv->cdclk.hw.ref == 24000)
			dev_priv->max_cdclk_freq = 648000;
		else
			dev_priv->max_cdclk_freq = 652800;
	} else if (IS_CANNONLAKE(dev_priv)) {
		dev_priv->max_cdclk_freq = 528000;
	} else if (IS_GEMINILAKE(dev_priv)) {
		dev_priv->max_cdclk_freq = 316800;
	} else if (IS_BROXTON(dev_priv)) {
		dev_priv->max_cdclk_freq = 624000;
	} else if (DISPLAY_VER(dev_priv) == 9) {
		u32 limit = intel_de_read(dev_priv, SKL_DFSM) & SKL_DFSM_CDCLK_LIMIT_MASK;
		int max_cdclk, vco;

		vco = dev_priv->skl_preferred_vco_freq;
		drm_WARN_ON(&dev_priv->drm, vco != 8100000 && vco != 8640000);

		/*
		 * Use the lower (vco 8640) cdclk values as a
		 * first guess. skl_calc_cdclk() will correct it
		 * if the preferred vco is 8100 instead.
		 */
		if (limit == SKL_DFSM_CDCLK_LIMIT_675)
			max_cdclk = 617143;
		else if (limit == SKL_DFSM_CDCLK_LIMIT_540)
			max_cdclk = 540000;
		else if (limit == SKL_DFSM_CDCLK_LIMIT_450)
			max_cdclk = 432000;
		else
			max_cdclk = 308571;

		dev_priv->max_cdclk_freq = skl_calc_cdclk(max_cdclk, vco);
	} else if (IS_BROADWELL(dev_priv))  {
		/*
		 * FIXME with extra cooling we can allow
		 * 540 MHz for ULX and 675 Mhz for ULT.
		 * How can we know if extra cooling is
		 * available? PCI ID, VTB, something else?
		 */
		if (intel_de_read(dev_priv, FUSE_STRAP) & HSW_CDCLK_LIMIT)
			dev_priv->max_cdclk_freq = 450000;
		else if (IS_BDW_ULX(dev_priv))
			dev_priv->max_cdclk_freq = 450000;
		else if (IS_BDW_ULT(dev_priv))
			dev_priv->max_cdclk_freq = 540000;
		else
			dev_priv->max_cdclk_freq = 675000;
	} else if (IS_CHERRYVIEW(dev_priv)) {
		dev_priv->max_cdclk_freq = 320000;
	} else if (IS_VALLEYVIEW(dev_priv)) {
		dev_priv->max_cdclk_freq = 400000;
	} else {
		/* otherwise assume cdclk is fixed */
		dev_priv->max_cdclk_freq = dev_priv->cdclk.hw.cdclk;
	}

	dev_priv->max_dotclk_freq = intel_compute_max_dotclk(dev_priv);

	drm_dbg(&dev_priv->drm, "Max CD clock rate: %d kHz\n",
		dev_priv->max_cdclk_freq);

	drm_dbg(&dev_priv->drm, "Max dotclock rate: %d kHz\n",
		dev_priv->max_dotclk_freq);
}

/**
 * intel_update_cdclk - Determine the current CDCLK frequency
 * @dev_priv: i915 device
 *
 * Determine the current CDCLK frequency.
 */
void intel_update_cdclk(struct drm_i915_private *dev_priv)
{
	dev_priv->display.get_cdclk(dev_priv, &dev_priv->cdclk.hw);

	/*
	 * 9:0 CMBUS [sic] CDCLK frequency (cdfreq):
	 * Programmng [sic] note: bit[9:2] should be programmed to the number
	 * of cdclk that generates 4MHz reference clock freq which is used to
	 * generate GMBus clock. This will vary with the cdclk freq.
	 */
	if (IS_VALLEYVIEW(dev_priv) || IS_CHERRYVIEW(dev_priv))
		intel_de_write(dev_priv, GMBUSFREQ_VLV,
			       DIV_ROUND_UP(dev_priv->cdclk.hw.cdclk, 1000));
}

static int dg1_rawclk(struct drm_i915_private *dev_priv)
{
	/*
	 * DG1 always uses a 38.4 MHz rawclk.  The bspec tells us
	 * "Program Numerator=2, Denominator=4, Divider=37 decimal."
	 */
	intel_de_write(dev_priv, PCH_RAWCLK_FREQ,
		       CNP_RAWCLK_DEN(4) | CNP_RAWCLK_DIV(37) | ICP_RAWCLK_NUM(2));

	return 38400;
}

static int cnp_rawclk(struct drm_i915_private *dev_priv)
{
	u32 rawclk;
	int divider, fraction;

	if (intel_de_read(dev_priv, SFUSE_STRAP) & SFUSE_STRAP_RAW_FREQUENCY) {
		/* 24 MHz */
		divider = 24000;
		fraction = 0;
	} else {
		/* 19.2 MHz */
		divider = 19000;
		fraction = 200;
	}

	rawclk = CNP_RAWCLK_DIV(divider / 1000);
	if (fraction) {
		int numerator = 1;

		rawclk |= CNP_RAWCLK_DEN(DIV_ROUND_CLOSEST(numerator * 1000,
							   fraction) - 1);
		if (INTEL_PCH_TYPE(dev_priv) >= PCH_ICP)
			rawclk |= ICP_RAWCLK_NUM(numerator);
	}

	intel_de_write(dev_priv, PCH_RAWCLK_FREQ, rawclk);
	return divider + fraction;
}

static int pch_rawclk(struct drm_i915_private *dev_priv)
{
	return (intel_de_read(dev_priv, PCH_RAWCLK_FREQ) & RAWCLK_FREQ_MASK) * 1000;
}

static int vlv_hrawclk(struct drm_i915_private *dev_priv)
{
	/* RAWCLK_FREQ_VLV register updated from power well code */
	return vlv_get_cck_clock_hpll(dev_priv, "hrawclk",
				      CCK_DISPLAY_REF_CLOCK_CONTROL);
}

static int i9xx_hrawclk(struct drm_i915_private *dev_priv)
{
	u32 clkcfg;

	/*
	 * hrawclock is 1/4 the FSB frequency
	 *
	 * Note that this only reads the state of the FSB
	 * straps, not the actual FSB frequency. Some BIOSen
	 * let you configure each independently. Ideally we'd
	 * read out the actual FSB frequency but sadly we
	 * don't know which registers have that information,
	 * and all the relevant docs have gone to bit heaven :(
	 */
	clkcfg = intel_de_read(dev_priv, CLKCFG) & CLKCFG_FSB_MASK;

	if (IS_MOBILE(dev_priv)) {
		switch (clkcfg) {
		case CLKCFG_FSB_400:
			return 100000;
		case CLKCFG_FSB_533:
			return 133333;
		case CLKCFG_FSB_667:
			return 166667;
		case CLKCFG_FSB_800:
			return 200000;
		case CLKCFG_FSB_1067:
			return 266667;
		case CLKCFG_FSB_1333:
			return 333333;
		default:
			MISSING_CASE(clkcfg);
			return 133333;
		}
	} else {
		switch (clkcfg) {
		case CLKCFG_FSB_400_ALT:
			return 100000;
		case CLKCFG_FSB_533:
			return 133333;
		case CLKCFG_FSB_667:
			return 166667;
		case CLKCFG_FSB_800:
			return 200000;
		case CLKCFG_FSB_1067_ALT:
			return 266667;
		case CLKCFG_FSB_1333_ALT:
			return 333333;
		case CLKCFG_FSB_1600_ALT:
			return 400000;
		default:
			return 133333;
		}
	}
}

/**
 * intel_read_rawclk - Determine the current RAWCLK frequency
 * @dev_priv: i915 device
 *
 * Determine the current RAWCLK frequency. RAWCLK is a fixed
 * frequency clock so this needs to done only once.
 */
u32 intel_read_rawclk(struct drm_i915_private *dev_priv)
{
	u32 freq;

	if (INTEL_PCH_TYPE(dev_priv) >= PCH_DG1)
		freq = dg1_rawclk(dev_priv);
	else if (INTEL_PCH_TYPE(dev_priv) >= PCH_CNP)
		freq = cnp_rawclk(dev_priv);
	else if (HAS_PCH_SPLIT(dev_priv))
		freq = pch_rawclk(dev_priv);
	else if (IS_VALLEYVIEW(dev_priv) || IS_CHERRYVIEW(dev_priv))
		freq = vlv_hrawclk(dev_priv);
	else if (DISPLAY_VER(dev_priv) >= 3)
		freq = i9xx_hrawclk(dev_priv);
	else
		/* no rawclk on other platforms, or no need to know it */
		return 0;

	return freq;
}

/**
 * intel_init_cdclk_hooks - Initialize CDCLK related modesetting hooks
 * @dev_priv: i915 device
 */
void intel_init_cdclk_hooks(struct drm_i915_private *dev_priv)
{
	if (IS_ALDERLAKE_P(dev_priv)) {
		dev_priv->display.set_cdclk = bxt_set_cdclk;
		dev_priv->display.bw_calc_min_cdclk = skl_bw_calc_min_cdclk;
		dev_priv->display.modeset_calc_cdclk = bxt_modeset_calc_cdclk;
		dev_priv->display.calc_voltage_level = tgl_calc_voltage_level;
		/* Wa_22011320316:adlp[a0] */
		if (IS_ADLP_DISPLAY_STEP(dev_priv, STEP_A0, STEP_A0))
			dev_priv->cdclk.table = adlp_a_step_cdclk_table;
		else
			dev_priv->cdclk.table = adlp_cdclk_table;
	} else if (IS_ROCKETLAKE(dev_priv)) {
		dev_priv->display.set_cdclk = bxt_set_cdclk;
		dev_priv->display.bw_calc_min_cdclk = skl_bw_calc_min_cdclk;
		dev_priv->display.modeset_calc_cdclk = bxt_modeset_calc_cdclk;
		dev_priv->display.calc_voltage_level = tgl_calc_voltage_level;
		dev_priv->cdclk.table = rkl_cdclk_table;
	} else if (DISPLAY_VER(dev_priv) >= 12) {
		dev_priv->display.set_cdclk = bxt_set_cdclk;
		dev_priv->display.bw_calc_min_cdclk = skl_bw_calc_min_cdclk;
		dev_priv->display.modeset_calc_cdclk = bxt_modeset_calc_cdclk;
		dev_priv->display.calc_voltage_level = tgl_calc_voltage_level;
		dev_priv->cdclk.table = icl_cdclk_table;
	} else if (IS_JSL_EHL(dev_priv)) {
		dev_priv->display.set_cdclk = bxt_set_cdclk;
		dev_priv->display.bw_calc_min_cdclk = skl_bw_calc_min_cdclk;
		dev_priv->display.modeset_calc_cdclk = bxt_modeset_calc_cdclk;
		dev_priv->display.calc_voltage_level = ehl_calc_voltage_level;
		dev_priv->cdclk.table = icl_cdclk_table;
	} else if (DISPLAY_VER(dev_priv) >= 11) {
		dev_priv->display.set_cdclk = bxt_set_cdclk;
		dev_priv->display.bw_calc_min_cdclk = skl_bw_calc_min_cdclk;
		dev_priv->display.modeset_calc_cdclk = bxt_modeset_calc_cdclk;
		dev_priv->display.calc_voltage_level = icl_calc_voltage_level;
		dev_priv->cdclk.table = icl_cdclk_table;
	} else if (IS_CANNONLAKE(dev_priv)) {
		dev_priv->display.bw_calc_min_cdclk = skl_bw_calc_min_cdclk;
		dev_priv->display.set_cdclk = bxt_set_cdclk;
		dev_priv->display.modeset_calc_cdclk = bxt_modeset_calc_cdclk;
		dev_priv->display.calc_voltage_level = cnl_calc_voltage_level;
		dev_priv->cdclk.table = cnl_cdclk_table;
	} else if (IS_GEMINILAKE(dev_priv) || IS_BROXTON(dev_priv)) {
		dev_priv->display.bw_calc_min_cdclk = skl_bw_calc_min_cdclk;
		dev_priv->display.set_cdclk = bxt_set_cdclk;
		dev_priv->display.modeset_calc_cdclk = bxt_modeset_calc_cdclk;
		dev_priv->display.calc_voltage_level = bxt_calc_voltage_level;
		if (IS_GEMINILAKE(dev_priv))
			dev_priv->cdclk.table = glk_cdclk_table;
		else
			dev_priv->cdclk.table = bxt_cdclk_table;
	} else if (DISPLAY_VER(dev_priv) == 9) {
		dev_priv->display.bw_calc_min_cdclk = skl_bw_calc_min_cdclk;
		dev_priv->display.set_cdclk = skl_set_cdclk;
		dev_priv->display.modeset_calc_cdclk = skl_modeset_calc_cdclk;
	} else if (IS_BROADWELL(dev_priv)) {
		dev_priv->display.bw_calc_min_cdclk = intel_bw_calc_min_cdclk;
		dev_priv->display.set_cdclk = bdw_set_cdclk;
		dev_priv->display.modeset_calc_cdclk = bdw_modeset_calc_cdclk;
	} else if (IS_CHERRYVIEW(dev_priv)) {
		dev_priv->display.bw_calc_min_cdclk = intel_bw_calc_min_cdclk;
		dev_priv->display.set_cdclk = chv_set_cdclk;
		dev_priv->display.modeset_calc_cdclk = vlv_modeset_calc_cdclk;
	} else if (IS_VALLEYVIEW(dev_priv)) {
		dev_priv->display.bw_calc_min_cdclk = intel_bw_calc_min_cdclk;
		dev_priv->display.set_cdclk = vlv_set_cdclk;
		dev_priv->display.modeset_calc_cdclk = vlv_modeset_calc_cdclk;
	} else {
		dev_priv->display.bw_calc_min_cdclk = intel_bw_calc_min_cdclk;
		dev_priv->display.modeset_calc_cdclk = fixed_modeset_calc_cdclk;
	}

	if (DISPLAY_VER(dev_priv) >= 10 || IS_BROXTON(dev_priv))
		dev_priv->display.get_cdclk = bxt_get_cdclk;
	else if (DISPLAY_VER(dev_priv) == 9)
		dev_priv->display.get_cdclk = skl_get_cdclk;
	else if (IS_BROADWELL(dev_priv))
		dev_priv->display.get_cdclk = bdw_get_cdclk;
	else if (IS_HASWELL(dev_priv))
		dev_priv->display.get_cdclk = hsw_get_cdclk;
	else if (IS_VALLEYVIEW(dev_priv) || IS_CHERRYVIEW(dev_priv))
		dev_priv->display.get_cdclk = vlv_get_cdclk;
	else if (IS_SANDYBRIDGE(dev_priv) || IS_IVYBRIDGE(dev_priv))
		dev_priv->display.get_cdclk = fixed_400mhz_get_cdclk;
	else if (IS_IRONLAKE(dev_priv))
		dev_priv->display.get_cdclk = fixed_450mhz_get_cdclk;
	else if (IS_GM45(dev_priv))
		dev_priv->display.get_cdclk = gm45_get_cdclk;
	else if (IS_G45(dev_priv))
		dev_priv->display.get_cdclk = g33_get_cdclk;
	else if (IS_I965GM(dev_priv))
		dev_priv->display.get_cdclk = i965gm_get_cdclk;
	else if (IS_I965G(dev_priv))
		dev_priv->display.get_cdclk = fixed_400mhz_get_cdclk;
	else if (IS_PINEVIEW(dev_priv))
		dev_priv->display.get_cdclk = pnv_get_cdclk;
	else if (IS_G33(dev_priv))
		dev_priv->display.get_cdclk = g33_get_cdclk;
	else if (IS_I945GM(dev_priv))
		dev_priv->display.get_cdclk = i945gm_get_cdclk;
	else if (IS_I945G(dev_priv))
		dev_priv->display.get_cdclk = fixed_400mhz_get_cdclk;
	else if (IS_I915GM(dev_priv))
		dev_priv->display.get_cdclk = i915gm_get_cdclk;
	else if (IS_I915G(dev_priv))
		dev_priv->display.get_cdclk = fixed_333mhz_get_cdclk;
	else if (IS_I865G(dev_priv))
		dev_priv->display.get_cdclk = fixed_266mhz_get_cdclk;
	else if (IS_I85X(dev_priv))
		dev_priv->display.get_cdclk = i85x_get_cdclk;
	else if (IS_I845G(dev_priv))
		dev_priv->display.get_cdclk = fixed_200mhz_get_cdclk;
	else if (IS_I830(dev_priv))
		dev_priv->display.get_cdclk = fixed_133mhz_get_cdclk;

	if (drm_WARN(&dev_priv->drm, !dev_priv->display.get_cdclk,
		     "Unknown platform. Assuming 133 MHz CDCLK\n"))
		dev_priv->display.get_cdclk = fixed_133mhz_get_cdclk;
}
