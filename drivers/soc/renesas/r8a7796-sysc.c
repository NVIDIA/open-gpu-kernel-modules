// SPDX-License-Identifier: GPL-2.0
/*
 * Renesas R-Car M3-W/W+ System Controller
 *
 * Copyright (C) 2016 Glider bvba
 * Copyright (C) 2018-2019 Renesas Electronics Corporation
 */

#include <linux/bits.h>
#include <linux/kernel.h>

#include <dt-bindings/power/r8a7796-sysc.h>

#include "rcar-sysc.h"

static struct rcar_sysc_area r8a7796_areas[] __initdata = {
	{ "always-on",	    0, 0, R8A7796_PD_ALWAYS_ON,	-1, PD_ALWAYS_ON },
	{ "ca57-scu",	0x1c0, 0, R8A7796_PD_CA57_SCU,	R8A7796_PD_ALWAYS_ON,
	  PD_SCU },
	{ "ca57-cpu0",	 0x80, 0, R8A7796_PD_CA57_CPU0,	R8A7796_PD_CA57_SCU,
	  PD_CPU_NOCR },
	{ "ca57-cpu1",	 0x80, 1, R8A7796_PD_CA57_CPU1,	R8A7796_PD_CA57_SCU,
	  PD_CPU_NOCR },
	{ "ca53-scu",	0x140, 0, R8A7796_PD_CA53_SCU,	R8A7796_PD_ALWAYS_ON,
	  PD_SCU },
	{ "ca53-cpu0",	0x200, 0, R8A7796_PD_CA53_CPU0,	R8A7796_PD_CA53_SCU,
	  PD_CPU_NOCR },
	{ "ca53-cpu1",	0x200, 1, R8A7796_PD_CA53_CPU1,	R8A7796_PD_CA53_SCU,
	  PD_CPU_NOCR },
	{ "ca53-cpu2",	0x200, 2, R8A7796_PD_CA53_CPU2,	R8A7796_PD_CA53_SCU,
	  PD_CPU_NOCR },
	{ "ca53-cpu3",	0x200, 3, R8A7796_PD_CA53_CPU3,	R8A7796_PD_CA53_SCU,
	  PD_CPU_NOCR },
	{ "cr7",	0x240, 0, R8A7796_PD_CR7,	R8A7796_PD_ALWAYS_ON },
	{ "a3vc",	0x380, 0, R8A7796_PD_A3VC,	R8A7796_PD_ALWAYS_ON },
	{ "a2vc0",	0x3c0, 0, R8A7796_PD_A2VC0,	R8A7796_PD_A3VC },
	{ "a2vc1",	0x3c0, 1, R8A7796_PD_A2VC1,	R8A7796_PD_A3VC },
	{ "3dg-a",	0x100, 0, R8A7796_PD_3DG_A,	R8A7796_PD_ALWAYS_ON },
	{ "3dg-b",	0x100, 1, R8A7796_PD_3DG_B,	R8A7796_PD_3DG_A },
	{ "a3ir",	0x180, 0, R8A7796_PD_A3IR,	R8A7796_PD_ALWAYS_ON },
};


#ifdef CONFIG_SYSC_R8A77960
const struct rcar_sysc_info r8a77960_sysc_info __initconst = {
	.areas = r8a7796_areas,
	.num_areas = ARRAY_SIZE(r8a7796_areas),
};
#endif /* CONFIG_SYSC_R8A77960 */

#ifdef CONFIG_SYSC_R8A77961
static int __init r8a77961_sysc_init(void)
{
	rcar_sysc_nullify(r8a7796_areas, ARRAY_SIZE(r8a7796_areas),
			  R8A7796_PD_A2VC0);

	return 0;
}

const struct rcar_sysc_info r8a77961_sysc_info __initconst = {
	.init = r8a77961_sysc_init,
	.areas = r8a7796_areas,
	.num_areas = ARRAY_SIZE(r8a7796_areas),
	.extmask_offs = 0x2f8,
	.extmask_val = BIT(0),
};
#endif /* CONFIG_SYSC_R8A77961 */
