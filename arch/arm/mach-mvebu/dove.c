/*
 * arch/arm/mach-mvebu/dove.c
 *
 * Marvell Dove 88AP510 System On Chip FDT Board
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <linux/init.h>
#include <linux/mbus.h>
#include <linux/of.h>
#include <linux/soc/dove/pmu.h>
#include <asm/hardware/cache-tauros2.h>
#include <asm/mach/arch.h>
#include "common.h"

static void __init dove_init(void)
{
	pr_info("Dove 88AP510 SoC\n");

#ifdef CONFIG_CACHE_TAUROS2
	tauros2_init(0);
#endif
	BUG_ON(mvebu_mbus_dt_init(false));
	dove_init_pmu();
}

static const char * const dove_dt_compat[] __initconst = {
	"marvell,dove",
	NULL
};

DT_MACHINE_START(DOVE_DT, "Marvell Dove")
	.init_machine	= dove_init,
	.restart	= mvebu_restart,
	.dt_compat	= dove_dt_compat,
MACHINE_END
