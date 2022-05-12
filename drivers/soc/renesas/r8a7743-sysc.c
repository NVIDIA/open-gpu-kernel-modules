// SPDX-License-Identifier: GPL-2.0
/*
 * Renesas RZ/G1M System Controller
 *
 * Copyright (C) 2016 Cogent Embedded Inc.
 */

#include <linux/kernel.h>

#include <dt-bindings/power/r8a7743-sysc.h>

#include "rcar-sysc.h"

static const struct rcar_sysc_area r8a7743_areas[] __initconst = {
	{ "always-on",	    0, 0, R8A7743_PD_ALWAYS_ON,	-1, PD_ALWAYS_ON },
	{ "ca15-scu",	0x180, 0, R8A7743_PD_CA15_SCU,	R8A7743_PD_ALWAYS_ON,
	  PD_SCU },
	{ "ca15-cpu0",	 0x40, 0, R8A7743_PD_CA15_CPU0,	R8A7743_PD_CA15_SCU,
	  PD_CPU_NOCR },
	{ "ca15-cpu1",	 0x40, 1, R8A7743_PD_CA15_CPU1,	R8A7743_PD_CA15_SCU,
	  PD_CPU_NOCR },
	{ "sgx",	 0xc0, 0, R8A7743_PD_SGX,	R8A7743_PD_ALWAYS_ON },
};

const struct rcar_sysc_info r8a7743_sysc_info __initconst = {
	.areas = r8a7743_areas,
	.num_areas = ARRAY_SIZE(r8a7743_areas),
};
