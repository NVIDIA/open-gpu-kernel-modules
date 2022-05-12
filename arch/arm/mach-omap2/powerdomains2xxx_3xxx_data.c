// SPDX-License-Identifier: GPL-2.0-only
/*
 * OMAP2/3 common powerdomain definitions
 *
 * Copyright (C) 2007-2008, 2011 Texas Instruments, Inc.
 * Copyright (C) 2007-2011 Nokia Corporation
 *
 * Paul Walmsley, Jouni Högander
 */

/*
 * The names for the DSP/IVA2 powerdomains are confusing.
 *
 * Most OMAP chips have an on-board DSP.
 *
 * On the 2420, this is a 'C55 DSP called, simply, the DSP.  Its
 * powerdomain is called the "DSP power domain."  On the 2430, the
 * on-board DSP is a 'C64 DSP, now called (along with its hardware
 * accelerators) the IVA2 or IVA2.1.  Its powerdomain is still called
 * the "DSP power domain." On the 3430, the DSP is a 'C64 DSP like the
 * 2430, also known as the IVA2; but its powerdomain is now called the
 * "IVA2 power domain."
 *
 * The 2420 also has something called the IVA, which is a separate ARM
 * core, and has nothing to do with the DSP/IVA2.
 *
 * Ideally the DSP/IVA2 could just be the same powerdomain, but the PRCM
 * address offset is different between the C55 and C64 DSPs.
 */

#include "powerdomain.h"

#include "prcm-common.h"
#include "prm.h"

/* OMAP2/3-common powerdomains */

/*
 * The GFX powerdomain is not present on 3430ES2, but currently we do not
 * have a macro to filter it out at compile-time.
 */
struct powerdomain gfx_omap2_pwrdm = {
	.name		  = "gfx_pwrdm",
	.prcm_offs	  = GFX_MOD,
	.pwrsts		  = PWRSTS_OFF_RET_ON,
	.pwrsts_logic_ret = PWRSTS_RET,
	.banks		  = 1,
	.pwrsts_mem_ret	  = {
		[0] = PWRSTS_RET, /* MEMRETSTATE */
	},
	.pwrsts_mem_on	  = {
		[0] = PWRSTS_ON,  /* MEMONSTATE */
	},
	.voltdm		  = { .name = "core" },
};

struct powerdomain wkup_omap2_pwrdm = {
	.name		= "wkup_pwrdm",
	.prcm_offs	= WKUP_MOD,
	.pwrsts		= PWRSTS_ON,
	.voltdm		= { .name = "wakeup" },
};
