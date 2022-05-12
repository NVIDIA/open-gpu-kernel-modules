// SPDX-License-Identifier: GPL-2.0-only
/*
 * MSDI IP block reset
 *
 * Copyright (C) 2012 Texas Instruments, Inc.
 * Paul Walmsley
 *
 * XXX What about pad muxing?
 */

#include <linux/kernel.h>
#include <linux/err.h>

#include "prm.h"
#include "common.h"
#include "control.h"
#include "omap_hwmod.h"
#include "omap_device.h"
#include "mmc.h"

/*
 * MSDI_CON_OFFSET: offset in bytes of the MSDI IP block's CON register
 *     from the IP block's base address
 */
#define MSDI_CON_OFFSET				0x0c

/* Register bitfields in the CON register */
#define MSDI_CON_POW_MASK			BIT(11)
#define MSDI_CON_CLKD_MASK			(0x3f << 0)
#define MSDI_CON_CLKD_SHIFT			0

/* MSDI_TARGET_RESET_CLKD: clock divisor to use throughout the reset */
#define MSDI_TARGET_RESET_CLKD		0x3ff

/**
 * omap_msdi_reset - reset the MSDI IP block
 * @oh: struct omap_hwmod *
 *
 * The MSDI IP block on OMAP2420 has to have both the POW and CLKD
 * fields set inside its CON register for a reset to complete
 * successfully.  This is not documented in the TRM.  For CLKD, we use
 * the value that results in the lowest possible clock rate, to attempt
 * to avoid disturbing any cards.
 */
int omap_msdi_reset(struct omap_hwmod *oh)
{
	u16 v = 0;
	int c = 0;

	/* Write to the SOFTRESET bit */
	omap_hwmod_softreset(oh);

	/* Enable the MSDI core and internal clock */
	v |= MSDI_CON_POW_MASK;
	v |= MSDI_TARGET_RESET_CLKD << MSDI_CON_CLKD_SHIFT;
	omap_hwmod_write(v, oh, MSDI_CON_OFFSET);

	/* Poll on RESETDONE bit */
	omap_test_timeout((omap_hwmod_read(oh, oh->class->sysc->syss_offs)
			   & SYSS_RESETDONE_MASK),
			  MAX_MODULE_SOFTRESET_WAIT, c);

	if (c == MAX_MODULE_SOFTRESET_WAIT)
		pr_warn("%s: %s: softreset failed (waited %d usec)\n",
			__func__, oh->name, MAX_MODULE_SOFTRESET_WAIT);
	else
		pr_debug("%s: %s: softreset in %d usec\n", __func__,
			 oh->name, c);

	/* Disable the MSDI internal clock */
	v &= ~MSDI_CON_CLKD_MASK;
	omap_hwmod_write(v, oh, MSDI_CON_OFFSET);

	return 0;
}
