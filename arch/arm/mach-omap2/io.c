// SPDX-License-Identifier: GPL-2.0-only
/*
 * linux/arch/arm/mach-omap2/io.c
 *
 * OMAP2 I/O mapping code
 *
 * Copyright (C) 2005 Nokia Corporation
 * Copyright (C) 2007-2009 Texas Instruments
 *
 * Author:
 *	Juha Yrjola <juha.yrjola@nokia.com>
 *	Syed Khasim <x0khasim@ti.com>
 *
 * Added OMAP4 support - Santosh Shilimkar <santosh.shilimkar@ti.com>
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/clk.h>

#include <asm/tlb.h>
#include <asm/mach/map.h>

#include <linux/omap-dma.h>

#include "omap_hwmod.h"
#include "soc.h"
#include "iomap.h"
#include "voltage.h"
#include "powerdomain.h"
#include "clockdomain.h"
#include "common.h"
#include "clock.h"
#include "clock2xxx.h"
#include "clock3xxx.h"
#include "sdrc.h"
#include "control.h"
#include "serial.h"
#include "sram.h"
#include "cm2xxx.h"
#include "cm3xxx.h"
#include "cm33xx.h"
#include "cm44xx.h"
#include "prm.h"
#include "cm.h"
#include "prcm_mpu44xx.h"
#include "prminst44xx.h"
#include "prm2xxx.h"
#include "prm3xxx.h"
#include "prm33xx.h"
#include "prm44xx.h"
#include "opp2xxx.h"
#include "omap-secure.h"

/*
 * omap_clk_soc_init: points to a function that does the SoC-specific
 * clock initializations
 */
static int (*omap_clk_soc_init)(void);

/*
 * The machine specific code may provide the extra mapping besides the
 * default mapping provided here.
 */

#if defined(CONFIG_SOC_OMAP2420) || defined(CONFIG_SOC_OMAP2430)
static struct map_desc omap24xx_io_desc[] __initdata = {
	{
		.virtual	= L3_24XX_VIRT,
		.pfn		= __phys_to_pfn(L3_24XX_PHYS),
		.length		= L3_24XX_SIZE,
		.type		= MT_DEVICE
	},
	{
		.virtual	= L4_24XX_VIRT,
		.pfn		= __phys_to_pfn(L4_24XX_PHYS),
		.length		= L4_24XX_SIZE,
		.type		= MT_DEVICE
	},
};

#ifdef CONFIG_SOC_OMAP2420
static struct map_desc omap242x_io_desc[] __initdata = {
	{
		.virtual	= DSP_MEM_2420_VIRT,
		.pfn		= __phys_to_pfn(DSP_MEM_2420_PHYS),
		.length		= DSP_MEM_2420_SIZE,
		.type		= MT_DEVICE
	},
	{
		.virtual	= DSP_IPI_2420_VIRT,
		.pfn		= __phys_to_pfn(DSP_IPI_2420_PHYS),
		.length		= DSP_IPI_2420_SIZE,
		.type		= MT_DEVICE
	},
	{
		.virtual	= DSP_MMU_2420_VIRT,
		.pfn		= __phys_to_pfn(DSP_MMU_2420_PHYS),
		.length		= DSP_MMU_2420_SIZE,
		.type		= MT_DEVICE
	},
};

#endif

#ifdef CONFIG_SOC_OMAP2430
static struct map_desc omap243x_io_desc[] __initdata = {
	{
		.virtual	= L4_WK_243X_VIRT,
		.pfn		= __phys_to_pfn(L4_WK_243X_PHYS),
		.length		= L4_WK_243X_SIZE,
		.type		= MT_DEVICE
	},
	{
		.virtual	= OMAP243X_GPMC_VIRT,
		.pfn		= __phys_to_pfn(OMAP243X_GPMC_PHYS),
		.length		= OMAP243X_GPMC_SIZE,
		.type		= MT_DEVICE
	},
	{
		.virtual	= OMAP243X_SDRC_VIRT,
		.pfn		= __phys_to_pfn(OMAP243X_SDRC_PHYS),
		.length		= OMAP243X_SDRC_SIZE,
		.type		= MT_DEVICE
	},
	{
		.virtual	= OMAP243X_SMS_VIRT,
		.pfn		= __phys_to_pfn(OMAP243X_SMS_PHYS),
		.length		= OMAP243X_SMS_SIZE,
		.type		= MT_DEVICE
	},
};
#endif
#endif

#ifdef	CONFIG_ARCH_OMAP3
static struct map_desc omap34xx_io_desc[] __initdata = {
	{
		.virtual	= L3_34XX_VIRT,
		.pfn		= __phys_to_pfn(L3_34XX_PHYS),
		.length		= L3_34XX_SIZE,
		.type		= MT_DEVICE
	},
	{
		.virtual	= L4_34XX_VIRT,
		.pfn		= __phys_to_pfn(L4_34XX_PHYS),
		.length		= L4_34XX_SIZE,
		.type		= MT_DEVICE
	},
	{
		.virtual	= OMAP34XX_GPMC_VIRT,
		.pfn		= __phys_to_pfn(OMAP34XX_GPMC_PHYS),
		.length		= OMAP34XX_GPMC_SIZE,
		.type		= MT_DEVICE
	},
	{
		.virtual	= OMAP343X_SMS_VIRT,
		.pfn		= __phys_to_pfn(OMAP343X_SMS_PHYS),
		.length		= OMAP343X_SMS_SIZE,
		.type		= MT_DEVICE
	},
	{
		.virtual	= OMAP343X_SDRC_VIRT,
		.pfn		= __phys_to_pfn(OMAP343X_SDRC_PHYS),
		.length		= OMAP343X_SDRC_SIZE,
		.type		= MT_DEVICE
	},
	{
		.virtual	= L4_PER_34XX_VIRT,
		.pfn		= __phys_to_pfn(L4_PER_34XX_PHYS),
		.length		= L4_PER_34XX_SIZE,
		.type		= MT_DEVICE
	},
	{
		.virtual	= L4_EMU_34XX_VIRT,
		.pfn		= __phys_to_pfn(L4_EMU_34XX_PHYS),
		.length		= L4_EMU_34XX_SIZE,
		.type		= MT_DEVICE
	},
};
#endif

#ifdef CONFIG_SOC_TI81XX
static struct map_desc omapti81xx_io_desc[] __initdata = {
	{
		.virtual	= L4_34XX_VIRT,
		.pfn		= __phys_to_pfn(L4_34XX_PHYS),
		.length		= L4_34XX_SIZE,
		.type		= MT_DEVICE
	}
};
#endif

#if defined(CONFIG_SOC_AM33XX) || defined(CONFIG_SOC_AM43XX)
static struct map_desc omapam33xx_io_desc[] __initdata = {
	{
		.virtual	= L4_34XX_VIRT,
		.pfn		= __phys_to_pfn(L4_34XX_PHYS),
		.length		= L4_34XX_SIZE,
		.type		= MT_DEVICE
	},
	{
		.virtual	= L4_WK_AM33XX_VIRT,
		.pfn		= __phys_to_pfn(L4_WK_AM33XX_PHYS),
		.length		= L4_WK_AM33XX_SIZE,
		.type		= MT_DEVICE
	}
};
#endif

#ifdef	CONFIG_ARCH_OMAP4
static struct map_desc omap44xx_io_desc[] __initdata = {
	{
		.virtual	= L3_44XX_VIRT,
		.pfn		= __phys_to_pfn(L3_44XX_PHYS),
		.length		= L3_44XX_SIZE,
		.type		= MT_DEVICE,
	},
	{
		.virtual	= L4_44XX_VIRT,
		.pfn		= __phys_to_pfn(L4_44XX_PHYS),
		.length		= L4_44XX_SIZE,
		.type		= MT_DEVICE,
	},
	{
		.virtual	= L4_PER_44XX_VIRT,
		.pfn		= __phys_to_pfn(L4_PER_44XX_PHYS),
		.length		= L4_PER_44XX_SIZE,
		.type		= MT_DEVICE,
	},
};
#endif

#ifdef CONFIG_SOC_OMAP5
static struct map_desc omap54xx_io_desc[] __initdata = {
	{
		.virtual	= L3_54XX_VIRT,
		.pfn		= __phys_to_pfn(L3_54XX_PHYS),
		.length		= L3_54XX_SIZE,
		.type		= MT_DEVICE,
	},
	{
		.virtual	= L4_54XX_VIRT,
		.pfn		= __phys_to_pfn(L4_54XX_PHYS),
		.length		= L4_54XX_SIZE,
		.type		= MT_DEVICE,
	},
	{
		.virtual	= L4_WK_54XX_VIRT,
		.pfn		= __phys_to_pfn(L4_WK_54XX_PHYS),
		.length		= L4_WK_54XX_SIZE,
		.type		= MT_DEVICE,
	},
	{
		.virtual	= L4_PER_54XX_VIRT,
		.pfn		= __phys_to_pfn(L4_PER_54XX_PHYS),
		.length		= L4_PER_54XX_SIZE,
		.type		= MT_DEVICE,
	},
};
#endif

#ifdef CONFIG_SOC_DRA7XX
static struct map_desc dra7xx_io_desc[] __initdata = {
	{
		.virtual	= L4_CFG_MPU_DRA7XX_VIRT,
		.pfn		= __phys_to_pfn(L4_CFG_MPU_DRA7XX_PHYS),
		.length		= L4_CFG_MPU_DRA7XX_SIZE,
		.type		= MT_DEVICE,
	},
	{
		.virtual	= L3_MAIN_SN_DRA7XX_VIRT,
		.pfn		= __phys_to_pfn(L3_MAIN_SN_DRA7XX_PHYS),
		.length		= L3_MAIN_SN_DRA7XX_SIZE,
		.type		= MT_DEVICE,
	},
	{
		.virtual	= L4_PER1_DRA7XX_VIRT,
		.pfn		= __phys_to_pfn(L4_PER1_DRA7XX_PHYS),
		.length		= L4_PER1_DRA7XX_SIZE,
		.type		= MT_DEVICE,
	},
	{
		.virtual	= L4_PER2_DRA7XX_VIRT,
		.pfn		= __phys_to_pfn(L4_PER2_DRA7XX_PHYS),
		.length		= L4_PER2_DRA7XX_SIZE,
		.type		= MT_DEVICE,
	},
	{
		.virtual	= L4_PER3_DRA7XX_VIRT,
		.pfn		= __phys_to_pfn(L4_PER3_DRA7XX_PHYS),
		.length		= L4_PER3_DRA7XX_SIZE,
		.type		= MT_DEVICE,
	},
	{
		.virtual	= L4_CFG_DRA7XX_VIRT,
		.pfn		= __phys_to_pfn(L4_CFG_DRA7XX_PHYS),
		.length		= L4_CFG_DRA7XX_SIZE,
		.type		= MT_DEVICE,
	},
	{
		.virtual	= L4_WKUP_DRA7XX_VIRT,
		.pfn		= __phys_to_pfn(L4_WKUP_DRA7XX_PHYS),
		.length		= L4_WKUP_DRA7XX_SIZE,
		.type		= MT_DEVICE,
	},
};
#endif

#ifdef CONFIG_SOC_OMAP2420
void __init omap242x_map_io(void)
{
	iotable_init(omap24xx_io_desc, ARRAY_SIZE(omap24xx_io_desc));
	iotable_init(omap242x_io_desc, ARRAY_SIZE(omap242x_io_desc));
}
#endif

#ifdef CONFIG_SOC_OMAP2430
void __init omap243x_map_io(void)
{
	iotable_init(omap24xx_io_desc, ARRAY_SIZE(omap24xx_io_desc));
	iotable_init(omap243x_io_desc, ARRAY_SIZE(omap243x_io_desc));
}
#endif

#ifdef CONFIG_ARCH_OMAP3
void __init omap3_map_io(void)
{
	iotable_init(omap34xx_io_desc, ARRAY_SIZE(omap34xx_io_desc));
}
#endif

#ifdef CONFIG_SOC_TI81XX
void __init ti81xx_map_io(void)
{
	iotable_init(omapti81xx_io_desc, ARRAY_SIZE(omapti81xx_io_desc));
}
#endif

#if defined(CONFIG_SOC_AM33XX) || defined(CONFIG_SOC_AM43XX)
void __init am33xx_map_io(void)
{
	iotable_init(omapam33xx_io_desc, ARRAY_SIZE(omapam33xx_io_desc));
}
#endif

#ifdef CONFIG_ARCH_OMAP4
void __init omap4_map_io(void)
{
	iotable_init(omap44xx_io_desc, ARRAY_SIZE(omap44xx_io_desc));
	omap_barriers_init();
}
#endif

#ifdef CONFIG_SOC_OMAP5
void __init omap5_map_io(void)
{
	iotable_init(omap54xx_io_desc, ARRAY_SIZE(omap54xx_io_desc));
	omap_barriers_init();
}
#endif

#ifdef CONFIG_SOC_DRA7XX
void __init dra7xx_map_io(void)
{
	iotable_init(dra7xx_io_desc, ARRAY_SIZE(dra7xx_io_desc));
	omap_barriers_init();
}
#endif
/*
 * omap2_init_reprogram_sdrc - reprogram SDRC timing parameters
 *
 * Sets the CORE DPLL3 M2 divider to the same value that it's at
 * currently.  This has the effect of setting the SDRC SDRAM AC timing
 * registers to the values currently defined by the kernel.  Currently
 * only defined for OMAP3; will return 0 if called on OMAP2.  Returns
 * -EINVAL if the dpll3_m2_ck cannot be found, 0 if called on OMAP2,
 * or passes along the return value of clk_set_rate().
 */
static int __init _omap2_init_reprogram_sdrc(void)
{
	struct clk *dpll3_m2_ck;
	int v = -EINVAL;
	long rate;

	if (!cpu_is_omap34xx())
		return 0;

	dpll3_m2_ck = clk_get(NULL, "dpll3_m2_ck");
	if (IS_ERR(dpll3_m2_ck))
		return -EINVAL;

	rate = clk_get_rate(dpll3_m2_ck);
	pr_info("Reprogramming SDRC clock to %ld Hz\n", rate);
	v = clk_set_rate(dpll3_m2_ck, rate);
	if (v)
		pr_err("dpll3_m2_clk rate change failed: %d\n", v);

	clk_put(dpll3_m2_ck);

	return v;
}

#ifdef CONFIG_OMAP_HWMOD
static int _set_hwmod_postsetup_state(struct omap_hwmod *oh, void *data)
{
	return omap_hwmod_set_postsetup_state(oh, *(u8 *)data);
}

static void __init __maybe_unused omap_hwmod_init_postsetup(void)
{
	u8 postsetup_state = _HWMOD_STATE_DEFAULT;

	/* Set the default postsetup state for all hwmods */
	omap_hwmod_for_each(_set_hwmod_postsetup_state, &postsetup_state);
}
#else
static inline void omap_hwmod_init_postsetup(void)
{
}
#endif

#ifdef CONFIG_SOC_OMAP2420
void __init omap2420_init_early(void)
{
	omap2_set_globals_tap(OMAP242X_CLASS, OMAP2_L4_IO_ADDRESS(0x48014000));
	omap2_set_globals_sdrc(OMAP2_L3_IO_ADDRESS(OMAP2420_SDRC_BASE),
			       OMAP2_L3_IO_ADDRESS(OMAP2420_SMS_BASE));
	omap2_control_base_init();
	omap2xxx_check_revision();
	omap2_prcm_base_init();
	omap2xxx_voltagedomains_init();
	omap242x_powerdomains_init();
	omap242x_clockdomains_init();
	omap2420_hwmod_init();
	omap_hwmod_init_postsetup();
	omap_clk_soc_init = omap2420_dt_clk_init;
	rate_table = omap2420_rate_table;
}

void __init omap2420_init_late(void)
{
	omap_pm_soc_init = omap2_pm_init;
}
#endif

#ifdef CONFIG_SOC_OMAP2430
void __init omap2430_init_early(void)
{
	omap2_set_globals_tap(OMAP243X_CLASS, OMAP2_L4_IO_ADDRESS(0x4900a000));
	omap2_set_globals_sdrc(OMAP2_L3_IO_ADDRESS(OMAP243X_SDRC_BASE),
			       OMAP2_L3_IO_ADDRESS(OMAP243X_SMS_BASE));
	omap2_control_base_init();
	omap2xxx_check_revision();
	omap2_prcm_base_init();
	omap2xxx_voltagedomains_init();
	omap243x_powerdomains_init();
	omap243x_clockdomains_init();
	omap2430_hwmod_init();
	omap_hwmod_init_postsetup();
	omap_clk_soc_init = omap2430_dt_clk_init;
	rate_table = omap2430_rate_table;
}

void __init omap2430_init_late(void)
{
	omap_pm_soc_init = omap2_pm_init;
}
#endif

/*
 * Currently only board-omap3beagle.c should call this because of the
 * same machine_id for 34xx and 36xx beagle.. Will get fixed with DT.
 */
#ifdef CONFIG_ARCH_OMAP3
void __init omap3_init_early(void)
{
	omap2_set_globals_tap(OMAP343X_CLASS, OMAP2_L4_IO_ADDRESS(0x4830A000));
	omap2_set_globals_sdrc(OMAP2_L3_IO_ADDRESS(OMAP343X_SDRC_BASE),
			       OMAP2_L3_IO_ADDRESS(OMAP343X_SMS_BASE));
	omap2_control_base_init();
	omap3xxx_check_revision();
	omap3xxx_check_features();
	omap2_prcm_base_init();
	omap3xxx_voltagedomains_init();
	omap3xxx_powerdomains_init();
	omap3xxx_clockdomains_init();
	omap3xxx_hwmod_init();
	omap_hwmod_init_postsetup();
	omap_secure_init();
}

void __init omap3430_init_early(void)
{
	omap3_init_early();
	omap_clk_soc_init = omap3430_dt_clk_init;
}

void __init omap35xx_init_early(void)
{
	omap3_init_early();
	omap_clk_soc_init = omap3430_dt_clk_init;
}

void __init omap3630_init_early(void)
{
	omap3_init_early();
	omap_clk_soc_init = omap3630_dt_clk_init;
}

void __init am35xx_init_early(void)
{
	omap3_init_early();
	omap_clk_soc_init = am35xx_dt_clk_init;
}

void __init omap3_init_late(void)
{
	omap_pm_soc_init = omap3_pm_init;
}

void __init ti81xx_init_late(void)
{
	omap_pm_soc_init = omap_pm_nop_init;
}
#endif

#ifdef CONFIG_SOC_TI81XX
void __init ti814x_init_early(void)
{
	omap2_set_globals_tap(TI814X_CLASS,
			      OMAP2_L4_IO_ADDRESS(TI81XX_TAP_BASE));
	omap2_control_base_init();
	omap3xxx_check_revision();
	ti81xx_check_features();
	omap2_prcm_base_init();
	omap3xxx_voltagedomains_init();
	omap3xxx_powerdomains_init();
	ti814x_clockdomains_init();
	dm814x_hwmod_init();
	omap_hwmod_init_postsetup();
	omap_clk_soc_init = dm814x_dt_clk_init;
	omap_secure_init();
}

void __init ti816x_init_early(void)
{
	omap2_set_globals_tap(TI816X_CLASS,
			      OMAP2_L4_IO_ADDRESS(TI81XX_TAP_BASE));
	omap2_control_base_init();
	omap3xxx_check_revision();
	ti81xx_check_features();
	omap2_prcm_base_init();
	omap3xxx_voltagedomains_init();
	omap3xxx_powerdomains_init();
	ti816x_clockdomains_init();
	dm816x_hwmod_init();
	omap_hwmod_init_postsetup();
	omap_clk_soc_init = dm816x_dt_clk_init;
	omap_secure_init();
}
#endif

#ifdef CONFIG_SOC_AM33XX
void __init am33xx_init_early(void)
{
	omap2_set_globals_tap(AM335X_CLASS,
			      AM33XX_L4_WK_IO_ADDRESS(AM33XX_TAP_BASE));
	omap2_control_base_init();
	omap3xxx_check_revision();
	am33xx_check_features();
	omap2_prcm_base_init();
	am33xx_powerdomains_init();
	am33xx_clockdomains_init();
	omap_clk_soc_init = am33xx_dt_clk_init;
	omap_secure_init();
}

void __init am33xx_init_late(void)
{
	omap_pm_soc_init = amx3_common_pm_init;
}
#endif

#ifdef CONFIG_SOC_AM43XX
void __init am43xx_init_early(void)
{
	omap2_set_globals_tap(AM335X_CLASS,
			      AM33XX_L4_WK_IO_ADDRESS(AM33XX_TAP_BASE));
	omap2_control_base_init();
	omap3xxx_check_revision();
	am33xx_check_features();
	omap2_prcm_base_init();
	am43xx_powerdomains_init();
	am43xx_clockdomains_init();
	omap_l2_cache_init();
	omap_clk_soc_init = am43xx_dt_clk_init;
	omap_secure_init();
}

void __init am43xx_init_late(void)
{
	omap_pm_soc_init = amx3_common_pm_init;
}
#endif

#ifdef CONFIG_ARCH_OMAP4
void __init omap4430_init_early(void)
{
	omap2_set_globals_tap(OMAP443X_CLASS,
			      OMAP2_L4_IO_ADDRESS(OMAP443X_SCM_BASE));
	omap2_set_globals_prcm_mpu(OMAP2_L4_IO_ADDRESS(OMAP4430_PRCM_MPU_BASE));
	omap2_control_base_init();
	omap4xxx_check_revision();
	omap4xxx_check_features();
	omap2_prcm_base_init();
	omap4_sar_ram_init();
	omap4_mpuss_early_init();
	omap4_pm_init_early();
	omap44xx_voltagedomains_init();
	omap44xx_powerdomains_init();
	omap44xx_clockdomains_init();
	omap_l2_cache_init();
	omap_clk_soc_init = omap4xxx_dt_clk_init;
	omap_secure_init();
}

void __init omap4430_init_late(void)
{
	omap_pm_soc_init = omap4_pm_init;
}
#endif

#ifdef CONFIG_SOC_OMAP5
void __init omap5_init_early(void)
{
	omap2_set_globals_tap(OMAP54XX_CLASS,
			      OMAP2_L4_IO_ADDRESS(OMAP54XX_SCM_BASE));
	omap2_set_globals_prcm_mpu(OMAP2_L4_IO_ADDRESS(OMAP54XX_PRCM_MPU_BASE));
	omap2_control_base_init();
	omap2_prcm_base_init();
	omap5xxx_check_revision();
	omap4_sar_ram_init();
	omap4_mpuss_early_init();
	omap4_pm_init_early();
	omap54xx_voltagedomains_init();
	omap54xx_powerdomains_init();
	omap54xx_clockdomains_init();
	omap_clk_soc_init = omap5xxx_dt_clk_init;
	omap_secure_init();
}

void __init omap5_init_late(void)
{
	omap_pm_soc_init = omap4_pm_init;
}
#endif

#ifdef CONFIG_SOC_DRA7XX
void __init dra7xx_init_early(void)
{
	omap2_set_globals_tap(DRA7XX_CLASS,
			      OMAP2_L4_IO_ADDRESS(DRA7XX_TAP_BASE));
	omap2_set_globals_prcm_mpu(OMAP2_L4_IO_ADDRESS(OMAP54XX_PRCM_MPU_BASE));
	omap2_control_base_init();
	omap4_pm_init_early();
	omap2_prcm_base_init();
	dra7xxx_check_revision();
	dra7xx_powerdomains_init();
	dra7xx_clockdomains_init();
	omap_clk_soc_init = dra7xx_dt_clk_init;
	omap_secure_init();
}

void __init dra7xx_init_late(void)
{
	omap_pm_soc_init = omap4_pm_init;
}
#endif


void __init omap_sdrc_init(struct omap_sdrc_params *sdrc_cs0,
				      struct omap_sdrc_params *sdrc_cs1)
{
	omap_sram_init();

	if (cpu_is_omap24xx() || omap3_has_sdrc()) {
		omap2_sdrc_init(sdrc_cs0, sdrc_cs1);
		_omap2_init_reprogram_sdrc();
	}
}

int __init omap_clk_init(void)
{
	int ret = 0;

	if (!omap_clk_soc_init)
		return 0;

	ti_clk_init_features();

	omap2_clk_setup_ll_ops();

	ret = omap_control_init();
	if (ret)
		return ret;

	ret = omap_prcm_init();
	if (ret)
		return ret;

	of_clk_init(NULL);

	ti_dt_clk_init_retry_clks();

	ti_dt_clockdomains_setup();

	ret = omap_clk_soc_init();

	return ret;
}
