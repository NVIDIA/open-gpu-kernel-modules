// SPDX-License-Identifier: GPL-2.0-only
/*
 * omap_hwmod_2420_data.c - hardware modules present on the OMAP2420 chips
 *
 * Copyright (C) 2009-2011 Nokia Corporation
 * Copyright (C) 2012 Texas Instruments, Inc.
 * Paul Walmsley
 *
 * XXX handle crossbar/shared link difference for L3?
 * XXX these should be marked initdata for multi-OMAP kernels
 */

#include <linux/platform_data/i2c-omap.h>

#include "omap_hwmod.h"
#include "l3_2xxx.h"
#include "l4_2xxx.h"

#include "omap_hwmod_common_data.h"

#include "cm-regbits-24xx.h"
#include "prm-regbits-24xx.h"
#include "i2c.h"
#include "mmc.h"
#include "serial.h"
#include "wd_timer.h"

/*
 * OMAP2420 hardware module integration data
 *
 * All of the data in this section should be autogeneratable from the
 * TI hardware database or other technical documentation.  Data that
 * is driver-specific or driver-kernel integration-specific belongs
 * elsewhere.
 */

/*
 * IP blocks
 */

/* IVA1 (IVA1) */
static struct omap_hwmod_class iva1_hwmod_class = {
	.name		= "iva1",
};

static struct omap_hwmod_rst_info omap2420_iva_resets[] = {
	{ .name = "iva", .rst_shift = 8 },
};

static struct omap_hwmod omap2420_iva_hwmod = {
	.name		= "iva",
	.class		= &iva1_hwmod_class,
	.clkdm_name	= "iva1_clkdm",
	.rst_lines	= omap2420_iva_resets,
	.rst_lines_cnt	= ARRAY_SIZE(omap2420_iva_resets),
	.main_clk	= "iva1_ifck",
};

/* DSP */
static struct omap_hwmod_class dsp_hwmod_class = {
	.name		= "dsp",
};

static struct omap_hwmod_rst_info omap2420_dsp_resets[] = {
	{ .name = "logic", .rst_shift = 0 },
	{ .name = "mmu", .rst_shift = 1 },
};

static struct omap_hwmod omap2420_dsp_hwmod = {
	.name		= "dsp",
	.class		= &dsp_hwmod_class,
	.clkdm_name	= "dsp_clkdm",
	.rst_lines	= omap2420_dsp_resets,
	.rst_lines_cnt	= ARRAY_SIZE(omap2420_dsp_resets),
	.main_clk	= "dsp_fck",
};

/* I2C common */
static struct omap_hwmod_class_sysconfig i2c_sysc = {
	.rev_offs	= 0x00,
	.sysc_offs	= 0x20,
	.syss_offs	= 0x10,
	.sysc_flags	= (SYSC_HAS_SOFTRESET | SYSS_HAS_RESET_STATUS),
	.sysc_fields	= &omap_hwmod_sysc_type1,
};

static struct omap_hwmod_class i2c_class = {
	.name		= "i2c",
	.sysc		= &i2c_sysc,
	.reset		= &omap_i2c_reset,
};

/* I2C1 */
static struct omap_hwmod omap2420_i2c1_hwmod = {
	.name		= "i2c1",
	.main_clk	= "i2c1_fck",
	.prcm		= {
		.omap2 = {
			.module_offs = CORE_MOD,
			.idlest_reg_id = 1,
			.idlest_idle_bit = OMAP2420_ST_I2C1_SHIFT,
		},
	},
	.class		= &i2c_class,
	/*
	 * From mach-omap2/pm24xx.c: "Putting MPU into the WFI state
	 * while a transfer is active seems to cause the I2C block to
	 * timeout. Why? Good question."
	 */
	.flags		= (HWMOD_16BIT_REG | HWMOD_BLOCK_WFI),
};

/* I2C2 */
static struct omap_hwmod omap2420_i2c2_hwmod = {
	.name		= "i2c2",
	.main_clk	= "i2c2_fck",
	.prcm		= {
		.omap2 = {
			.module_offs = CORE_MOD,
			.idlest_reg_id = 1,
			.idlest_idle_bit = OMAP2420_ST_I2C2_SHIFT,
		},
	},
	.class		= &i2c_class,
	.flags		= HWMOD_16BIT_REG,
};

/* mailbox */
static struct omap_hwmod omap2420_mailbox_hwmod = {
	.name		= "mailbox",
	.class		= &omap2xxx_mailbox_hwmod_class,
	.main_clk	= "mailboxes_ick",
	.prcm		= {
		.omap2 = {
			.module_offs = CORE_MOD,
			.idlest_reg_id = 1,
			.idlest_idle_bit = OMAP24XX_ST_MAILBOXES_SHIFT,
		},
	},
};

/*
 * 'mcbsp' class
 * multi channel buffered serial port controller
 */

static struct omap_hwmod_class omap2420_mcbsp_hwmod_class = {
	.name = "mcbsp",
};

static struct omap_hwmod_opt_clk mcbsp_opt_clks[] = {
	{ .role = "pad_fck", .clk = "mcbsp_clks" },
	{ .role = "prcm_fck", .clk = "func_96m_ck" },
};

/* mcbsp1 */
static struct omap_hwmod omap2420_mcbsp1_hwmod = {
	.name		= "mcbsp1",
	.class		= &omap2420_mcbsp_hwmod_class,
	.main_clk	= "mcbsp1_fck",
	.prcm		= {
		.omap2 = {
			.module_offs = CORE_MOD,
			.idlest_reg_id = 1,
			.idlest_idle_bit = OMAP24XX_ST_MCBSP1_SHIFT,
		},
	},
	.opt_clks	= mcbsp_opt_clks,
	.opt_clks_cnt	= ARRAY_SIZE(mcbsp_opt_clks),
};

/* mcbsp2 */
static struct omap_hwmod omap2420_mcbsp2_hwmod = {
	.name		= "mcbsp2",
	.class		= &omap2420_mcbsp_hwmod_class,
	.main_clk	= "mcbsp2_fck",
	.prcm		= {
		.omap2 = {
			.module_offs = CORE_MOD,
			.idlest_reg_id = 1,
			.idlest_idle_bit = OMAP24XX_ST_MCBSP2_SHIFT,
		},
	},
	.opt_clks	= mcbsp_opt_clks,
	.opt_clks_cnt	= ARRAY_SIZE(mcbsp_opt_clks),
};

static struct omap_hwmod_class_sysconfig omap2420_msdi_sysc = {
	.rev_offs	= 0x3c,
	.sysc_offs	= 0x64,
	.syss_offs	= 0x68,
	.sysc_flags	= (SYSC_HAS_SOFTRESET | SYSS_HAS_RESET_STATUS),
	.sysc_fields	= &omap_hwmod_sysc_type1,
};

static struct omap_hwmod_class omap2420_msdi_hwmod_class = {
	.name	= "msdi",
	.sysc	= &omap2420_msdi_sysc,
	.reset	= &omap_msdi_reset,
};

/* msdi1 */
static struct omap_hwmod omap2420_msdi1_hwmod = {
	.name		= "msdi1",
	.class		= &omap2420_msdi_hwmod_class,
	.main_clk	= "mmc_fck",
	.prcm		= {
		.omap2 = {
			.module_offs = CORE_MOD,
			.idlest_reg_id = 1,
			.idlest_idle_bit = OMAP2420_ST_MMC_SHIFT,
		},
	},
	.flags		= HWMOD_16BIT_REG,
};

/* HDQ1W/1-wire */
static struct omap_hwmod omap2420_hdq1w_hwmod = {
	.name		= "hdq1w",
	.main_clk	= "hdq_fck",
	.prcm		= {
		.omap2 = {
			.module_offs = CORE_MOD,
			.idlest_reg_id = 1,
			.idlest_idle_bit = OMAP24XX_ST_HDQ_SHIFT,
		},
	},
	.class		= &omap2_hdq1w_class,
};

/*
 * interfaces
 */

/* L4 CORE -> I2C1 interface */
static struct omap_hwmod_ocp_if omap2420_l4_core__i2c1 = {
	.master		= &omap2xxx_l4_core_hwmod,
	.slave		= &omap2420_i2c1_hwmod,
	.clk		= "i2c1_ick",
	.user		= OCP_USER_MPU | OCP_USER_SDMA,
};

/* L4 CORE -> I2C2 interface */
static struct omap_hwmod_ocp_if omap2420_l4_core__i2c2 = {
	.master		= &omap2xxx_l4_core_hwmod,
	.slave		= &omap2420_i2c2_hwmod,
	.clk		= "i2c2_ick",
	.user		= OCP_USER_MPU | OCP_USER_SDMA,
};

/* IVA <- L3 interface */
static struct omap_hwmod_ocp_if omap2420_l3__iva = {
	.master		= &omap2xxx_l3_main_hwmod,
	.slave		= &omap2420_iva_hwmod,
	.clk		= "core_l3_ck",
	.user		= OCP_USER_MPU | OCP_USER_SDMA,
};

/* DSP <- L3 interface */
static struct omap_hwmod_ocp_if omap2420_l3__dsp = {
	.master		= &omap2xxx_l3_main_hwmod,
	.slave		= &omap2420_dsp_hwmod,
	.clk		= "dsp_ick",
	.user		= OCP_USER_MPU | OCP_USER_SDMA,
};

/* l4_wkup -> wd_timer2 */
static struct omap_hwmod_ocp_if omap2420_l4_wkup__wd_timer2 = {
	.master		= &omap2xxx_l4_wkup_hwmod,
	.slave		= &omap2xxx_wd_timer2_hwmod,
	.clk		= "mpu_wdt_ick",
	.user		= OCP_USER_MPU | OCP_USER_SDMA,
};

/* l4_wkup -> gpio1 */
static struct omap_hwmod_ocp_if omap2420_l4_wkup__gpio1 = {
	.master		= &omap2xxx_l4_wkup_hwmod,
	.slave		= &omap2xxx_gpio1_hwmod,
	.clk		= "gpios_ick",
	.user		= OCP_USER_MPU | OCP_USER_SDMA,
};

/* l4_wkup -> gpio2 */
static struct omap_hwmod_ocp_if omap2420_l4_wkup__gpio2 = {
	.master		= &omap2xxx_l4_wkup_hwmod,
	.slave		= &omap2xxx_gpio2_hwmod,
	.clk		= "gpios_ick",
	.user		= OCP_USER_MPU | OCP_USER_SDMA,
};

/* l4_wkup -> gpio3 */
static struct omap_hwmod_ocp_if omap2420_l4_wkup__gpio3 = {
	.master		= &omap2xxx_l4_wkup_hwmod,
	.slave		= &omap2xxx_gpio3_hwmod,
	.clk		= "gpios_ick",
	.user		= OCP_USER_MPU | OCP_USER_SDMA,
};

/* l4_wkup -> gpio4 */
static struct omap_hwmod_ocp_if omap2420_l4_wkup__gpio4 = {
	.master		= &omap2xxx_l4_wkup_hwmod,
	.slave		= &omap2xxx_gpio4_hwmod,
	.clk		= "gpios_ick",
	.user		= OCP_USER_MPU | OCP_USER_SDMA,
};

/* l4_core -> mailbox */
static struct omap_hwmod_ocp_if omap2420_l4_core__mailbox = {
	.master		= &omap2xxx_l4_core_hwmod,
	.slave		= &omap2420_mailbox_hwmod,
	.user		= OCP_USER_MPU | OCP_USER_SDMA,
};

/* l4_core -> mcbsp1 */
static struct omap_hwmod_ocp_if omap2420_l4_core__mcbsp1 = {
	.master		= &omap2xxx_l4_core_hwmod,
	.slave		= &omap2420_mcbsp1_hwmod,
	.clk		= "mcbsp1_ick",
	.user		= OCP_USER_MPU | OCP_USER_SDMA,
};

/* l4_core -> mcbsp2 */
static struct omap_hwmod_ocp_if omap2420_l4_core__mcbsp2 = {
	.master		= &omap2xxx_l4_core_hwmod,
	.slave		= &omap2420_mcbsp2_hwmod,
	.clk		= "mcbsp2_ick",
	.user		= OCP_USER_MPU | OCP_USER_SDMA,
};

/* l4_core -> msdi1 */
static struct omap_hwmod_ocp_if omap2420_l4_core__msdi1 = {
	.master		= &omap2xxx_l4_core_hwmod,
	.slave		= &omap2420_msdi1_hwmod,
	.clk		= "mmc_ick",
	.user		= OCP_USER_MPU | OCP_USER_SDMA,
};

/* l4_core -> hdq1w interface */
static struct omap_hwmod_ocp_if omap2420_l4_core__hdq1w = {
	.master		= &omap2xxx_l4_core_hwmod,
	.slave		= &omap2420_hdq1w_hwmod,
	.clk		= "hdq_ick",
	.user		= OCP_USER_MPU | OCP_USER_SDMA,
	.flags		= OMAP_FIREWALL_L4 | OCPIF_SWSUP_IDLE,
};

static struct omap_hwmod_ocp_if omap2420_l3__gpmc = {
	.master		= &omap2xxx_l3_main_hwmod,
	.slave		= &omap2xxx_gpmc_hwmod,
	.clk		= "core_l3_ck",
	.user		= OCP_USER_MPU | OCP_USER_SDMA,
};

static struct omap_hwmod_ocp_if *omap2420_hwmod_ocp_ifs[] __initdata = {
	&omap2xxx_l3_main__l4_core,
	&omap2xxx_mpu__l3_main,
	&omap2xxx_dss__l3,
	&omap2xxx_l4_core__mcspi1,
	&omap2xxx_l4_core__mcspi2,
	&omap2xxx_l4_core__l4_wkup,
	&omap2_l4_core__uart1,
	&omap2_l4_core__uart2,
	&omap2_l4_core__uart3,
	&omap2420_l4_core__i2c1,
	&omap2420_l4_core__i2c2,
	&omap2420_l3__iva,
	&omap2420_l3__dsp,
	&omap2xxx_l4_core__timer3,
	&omap2xxx_l4_core__timer4,
	&omap2xxx_l4_core__timer5,
	&omap2xxx_l4_core__timer6,
	&omap2xxx_l4_core__timer7,
	&omap2xxx_l4_core__timer8,
	&omap2xxx_l4_core__timer9,
	&omap2xxx_l4_core__timer10,
	&omap2xxx_l4_core__timer11,
	&omap2xxx_l4_core__timer12,
	&omap2420_l4_wkup__wd_timer2,
	&omap2xxx_l4_core__dss,
	&omap2xxx_l4_core__dss_dispc,
	&omap2xxx_l4_core__dss_rfbi,
	&omap2xxx_l4_core__dss_venc,
	&omap2420_l4_wkup__gpio1,
	&omap2420_l4_wkup__gpio2,
	&omap2420_l4_wkup__gpio3,
	&omap2420_l4_wkup__gpio4,
	&omap2420_l4_core__mailbox,
	&omap2420_l4_core__mcbsp1,
	&omap2420_l4_core__mcbsp2,
	&omap2420_l4_core__msdi1,
	&omap2xxx_l4_core__rng,
	&omap2xxx_l4_core__sham,
	&omap2xxx_l4_core__aes,
	&omap2420_l4_core__hdq1w,
	&omap2420_l3__gpmc,
	NULL,
};

int __init omap2420_hwmod_init(void)
{
	omap_hwmod_init();
	return omap_hwmod_register_links(omap2420_hwmod_ocp_ifs);
}
