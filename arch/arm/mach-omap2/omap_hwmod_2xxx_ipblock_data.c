// SPDX-License-Identifier: GPL-2.0-only
/*
 * omap_hwmod_2xxx_ipblock_data.c - common IP block data for OMAP2xxx
 *
 * Copyright (C) 2011 Nokia Corporation
 * Paul Walmsley
 */

#include <linux/types.h>

#include "omap_hwmod.h"
#include "omap_hwmod_common_data.h"
#include "cm-regbits-24xx.h"
#include "prm-regbits-24xx.h"
#include "wd_timer.h"

/*
 * 'dispc' class
 * display controller
 */

static struct omap_hwmod_class_sysconfig omap2_dispc_sysc = {
	.rev_offs	= 0x0000,
	.sysc_offs	= 0x0010,
	.syss_offs	= 0x0014,
	.sysc_flags	= (SYSC_HAS_SIDLEMODE | SYSC_HAS_MIDLEMODE |
			   SYSC_HAS_SOFTRESET | SYSC_HAS_AUTOIDLE),
	.idlemodes	= (SIDLE_FORCE | SIDLE_NO | SIDLE_SMART |
			   MSTANDBY_FORCE | MSTANDBY_NO | MSTANDBY_SMART),
	.sysc_fields	= &omap_hwmod_sysc_type1,
};

struct omap_hwmod_class omap2_dispc_hwmod_class = {
	.name	= "dispc",
	.sysc	= &omap2_dispc_sysc,
};

/* OMAP2xxx Timer Common */
static struct omap_hwmod_class_sysconfig omap2xxx_timer_sysc = {
	.rev_offs	= 0x0000,
	.sysc_offs	= 0x0010,
	.syss_offs	= 0x0014,
	.sysc_flags	= (SYSC_HAS_SIDLEMODE | SYSC_HAS_CLOCKACTIVITY |
			   SYSC_HAS_ENAWAKEUP | SYSC_HAS_SOFTRESET |
			   SYSC_HAS_AUTOIDLE | SYSS_HAS_RESET_STATUS),
	.idlemodes	= (SIDLE_FORCE | SIDLE_NO | SIDLE_SMART),
	.sysc_fields	= &omap_hwmod_sysc_type1,
};

struct omap_hwmod_class omap2xxx_timer_hwmod_class = {
	.name	= "timer",
	.sysc	= &omap2xxx_timer_sysc,
};

/*
 * 'wd_timer' class
 * 32-bit watchdog upward counter that generates a pulse on the reset pin on
 * overflow condition
 */

static struct omap_hwmod_class_sysconfig omap2xxx_wd_timer_sysc = {
	.rev_offs	= 0x0000,
	.sysc_offs	= 0x0010,
	.syss_offs	= 0x0014,
	.sysc_flags	= (SYSC_HAS_EMUFREE | SYSC_HAS_SOFTRESET |
			   SYSC_HAS_AUTOIDLE | SYSS_HAS_RESET_STATUS),
	.sysc_fields	= &omap_hwmod_sysc_type1,
};

struct omap_hwmod_class omap2xxx_wd_timer_hwmod_class = {
	.name		= "wd_timer",
	.sysc		= &omap2xxx_wd_timer_sysc,
	.pre_shutdown	= &omap2_wd_timer_disable,
	.reset		= &omap2_wd_timer_reset,
};

/*
 * 'gpio' class
 * general purpose io module
 */
static struct omap_hwmod_class_sysconfig omap2xxx_gpio_sysc = {
	.rev_offs	= 0x0000,
	.sysc_offs	= 0x0010,
	.syss_offs	= 0x0014,
	.sysc_flags	= (SYSC_HAS_ENAWAKEUP | SYSC_HAS_SIDLEMODE |
			   SYSC_HAS_SOFTRESET | SYSC_HAS_AUTOIDLE |
			   SYSS_HAS_RESET_STATUS),
	.idlemodes	= (SIDLE_FORCE | SIDLE_NO | SIDLE_SMART),
	.sysc_fields	= &omap_hwmod_sysc_type1,
};

struct omap_hwmod_class omap2xxx_gpio_hwmod_class = {
	.name = "gpio",
	.sysc = &omap2xxx_gpio_sysc,
};

/*
 * 'mailbox' class
 * mailbox module allowing communication between the on-chip processors
 * using a queued mailbox-interrupt mechanism.
 */

static struct omap_hwmod_class_sysconfig omap2xxx_mailbox_sysc = {
	.rev_offs	= 0x000,
	.sysc_offs	= 0x010,
	.syss_offs	= 0x014,
	.sysc_flags	= (SYSC_HAS_CLOCKACTIVITY | SYSC_HAS_SIDLEMODE |
			   SYSC_HAS_SOFTRESET | SYSC_HAS_AUTOIDLE),
	.idlemodes	= (SIDLE_FORCE | SIDLE_NO | SIDLE_SMART),
	.sysc_fields	= &omap_hwmod_sysc_type1,
};

struct omap_hwmod_class omap2xxx_mailbox_hwmod_class = {
	.name	= "mailbox",
	.sysc	= &omap2xxx_mailbox_sysc,
};

/*
 * 'mcspi' class
 * multichannel serial port interface (mcspi) / master/slave synchronous serial
 * bus
 */

static struct omap_hwmod_class_sysconfig omap2xxx_mcspi_sysc = {
	.rev_offs	= 0x0000,
	.sysc_offs	= 0x0010,
	.syss_offs	= 0x0014,
	.sysc_flags	= (SYSC_HAS_CLOCKACTIVITY | SYSC_HAS_SIDLEMODE |
				SYSC_HAS_ENAWAKEUP | SYSC_HAS_SOFTRESET |
				SYSC_HAS_AUTOIDLE | SYSS_HAS_RESET_STATUS),
	.idlemodes	= (SIDLE_FORCE | SIDLE_NO | SIDLE_SMART),
	.sysc_fields	= &omap_hwmod_sysc_type1,
};

struct omap_hwmod_class omap2xxx_mcspi_class = {
	.name	= "mcspi",
	.sysc	= &omap2xxx_mcspi_sysc,
};

/*
 * 'gpmc' class
 * general purpose memory controller
 */

static struct omap_hwmod_class_sysconfig omap2xxx_gpmc_sysc = {
	.rev_offs	= 0x0000,
	.sysc_offs	= 0x0010,
	.syss_offs	= 0x0014,
	.sysc_flags	= (SYSC_HAS_AUTOIDLE | SYSC_HAS_SIDLEMODE |
			   SYSC_HAS_SOFTRESET | SYSS_HAS_RESET_STATUS),
	.idlemodes	= (SIDLE_FORCE | SIDLE_NO | SIDLE_SMART),
	.sysc_fields	= &omap_hwmod_sysc_type1,
};

static struct omap_hwmod_class omap2xxx_gpmc_hwmod_class = {
	.name	= "gpmc",
	.sysc	= &omap2xxx_gpmc_sysc,
};

/*
 * IP blocks
 */

/* L3 */
struct omap_hwmod omap2xxx_l3_main_hwmod = {
	.name		= "l3_main",
	.class		= &l3_hwmod_class,
	.flags		= HWMOD_NO_IDLEST,
};

/* L4 CORE */
struct omap_hwmod omap2xxx_l4_core_hwmod = {
	.name		= "l4_core",
	.class		= &l4_hwmod_class,
	.flags		= HWMOD_NO_IDLEST,
};

/* L4 WKUP */
struct omap_hwmod omap2xxx_l4_wkup_hwmod = {
	.name		= "l4_wkup",
	.class		= &l4_hwmod_class,
	.flags		= HWMOD_NO_IDLEST,
};

/* MPU */
struct omap_hwmod omap2xxx_mpu_hwmod = {
	.name		= "mpu",
	.class		= &mpu_hwmod_class,
	.main_clk	= "mpu_ck",
};

/* IVA2 */
struct omap_hwmod omap2xxx_iva_hwmod = {
	.name		= "iva",
	.class		= &iva_hwmod_class,
};

/* timer3 */
struct omap_hwmod omap2xxx_timer3_hwmod = {
	.name		= "timer3",
	.main_clk	= "gpt3_fck",
	.prcm		= {
		.omap2 = {
			.module_offs = CORE_MOD,
			.idlest_reg_id = 1,
			.idlest_idle_bit = OMAP24XX_ST_GPT3_SHIFT,
		},
	},
	.class		= &omap2xxx_timer_hwmod_class,
	.flags          = HWMOD_SET_DEFAULT_CLOCKACT,
};

/* timer4 */
struct omap_hwmod omap2xxx_timer4_hwmod = {
	.name		= "timer4",
	.main_clk	= "gpt4_fck",
	.prcm		= {
		.omap2 = {
			.module_offs = CORE_MOD,
			.idlest_reg_id = 1,
			.idlest_idle_bit = OMAP24XX_ST_GPT4_SHIFT,
		},
	},
	.class		= &omap2xxx_timer_hwmod_class,
	.flags          = HWMOD_SET_DEFAULT_CLOCKACT,
};

/* timer5 */
struct omap_hwmod omap2xxx_timer5_hwmod = {
	.name		= "timer5",
	.main_clk	= "gpt5_fck",
	.prcm		= {
		.omap2 = {
			.module_offs = CORE_MOD,
			.idlest_reg_id = 1,
			.idlest_idle_bit = OMAP24XX_ST_GPT5_SHIFT,
		},
	},
	.class		= &omap2xxx_timer_hwmod_class,
	.flags          = HWMOD_SET_DEFAULT_CLOCKACT,
};

/* timer6 */
struct omap_hwmod omap2xxx_timer6_hwmod = {
	.name		= "timer6",
	.main_clk	= "gpt6_fck",
	.prcm		= {
		.omap2 = {
			.module_offs = CORE_MOD,
			.idlest_reg_id = 1,
			.idlest_idle_bit = OMAP24XX_ST_GPT6_SHIFT,
		},
	},
	.class		= &omap2xxx_timer_hwmod_class,
	.flags          = HWMOD_SET_DEFAULT_CLOCKACT,
};

/* timer7 */
struct omap_hwmod omap2xxx_timer7_hwmod = {
	.name		= "timer7",
	.main_clk	= "gpt7_fck",
	.prcm		= {
		.omap2 = {
			.module_offs = CORE_MOD,
			.idlest_reg_id = 1,
			.idlest_idle_bit = OMAP24XX_ST_GPT7_SHIFT,
		},
	},
	.class		= &omap2xxx_timer_hwmod_class,
	.flags          = HWMOD_SET_DEFAULT_CLOCKACT,
};

/* timer8 */
struct omap_hwmod omap2xxx_timer8_hwmod = {
	.name		= "timer8",
	.main_clk	= "gpt8_fck",
	.prcm		= {
		.omap2 = {
			.module_offs = CORE_MOD,
			.idlest_reg_id = 1,
			.idlest_idle_bit = OMAP24XX_ST_GPT8_SHIFT,
		},
	},
	.class		= &omap2xxx_timer_hwmod_class,
	.flags          = HWMOD_SET_DEFAULT_CLOCKACT,
};

/* timer9 */
struct omap_hwmod omap2xxx_timer9_hwmod = {
	.name		= "timer9",
	.main_clk	= "gpt9_fck",
	.prcm		= {
		.omap2 = {
			.module_offs = CORE_MOD,
			.idlest_reg_id = 1,
			.idlest_idle_bit = OMAP24XX_ST_GPT9_SHIFT,
		},
	},
	.class		= &omap2xxx_timer_hwmod_class,
	.flags          = HWMOD_SET_DEFAULT_CLOCKACT,
};

/* timer10 */
struct omap_hwmod omap2xxx_timer10_hwmod = {
	.name		= "timer10",
	.main_clk	= "gpt10_fck",
	.prcm		= {
		.omap2 = {
			.module_offs = CORE_MOD,
			.idlest_reg_id = 1,
			.idlest_idle_bit = OMAP24XX_ST_GPT10_SHIFT,
		},
	},
	.class		= &omap2xxx_timer_hwmod_class,
	.flags          = HWMOD_SET_DEFAULT_CLOCKACT,
};

/* timer11 */
struct omap_hwmod omap2xxx_timer11_hwmod = {
	.name		= "timer11",
	.main_clk	= "gpt11_fck",
	.prcm		= {
		.omap2 = {
			.module_offs = CORE_MOD,
			.idlest_reg_id = 1,
			.idlest_idle_bit = OMAP24XX_ST_GPT11_SHIFT,
		},
	},
	.class		= &omap2xxx_timer_hwmod_class,
	.flags          = HWMOD_SET_DEFAULT_CLOCKACT,
};

/* timer12 */
struct omap_hwmod omap2xxx_timer12_hwmod = {
	.name		= "timer12",
	.main_clk	= "gpt12_fck",
	.prcm		= {
		.omap2 = {
			.module_offs = CORE_MOD,
			.idlest_reg_id = 1,
			.idlest_idle_bit = OMAP24XX_ST_GPT12_SHIFT,
		},
	},
	.class		= &omap2xxx_timer_hwmod_class,
	.flags          = HWMOD_SET_DEFAULT_CLOCKACT,
};

/* wd_timer2 */
struct omap_hwmod omap2xxx_wd_timer2_hwmod = {
	.name		= "wd_timer2",
	.class		= &omap2xxx_wd_timer_hwmod_class,
	.main_clk	= "mpu_wdt_fck",
	.prcm		= {
		.omap2 = {
			.module_offs = WKUP_MOD,
			.idlest_reg_id = 1,
			.idlest_idle_bit = OMAP24XX_ST_MPU_WDT_SHIFT,
		},
	},
};

/* UART1 */

struct omap_hwmod omap2xxx_uart1_hwmod = {
	.name		= "uart1",
	.main_clk	= "uart1_fck",
	.flags		= DEBUG_OMAP2UART1_FLAGS | HWMOD_SWSUP_SIDLE_ACT,
	.prcm		= {
		.omap2 = {
			.module_offs = CORE_MOD,
			.idlest_reg_id = 1,
			.idlest_idle_bit = OMAP24XX_EN_UART1_SHIFT,
		},
	},
	.class		= &omap2_uart_class,
};

/* UART2 */

struct omap_hwmod omap2xxx_uart2_hwmod = {
	.name		= "uart2",
	.main_clk	= "uart2_fck",
	.flags		= DEBUG_OMAP2UART2_FLAGS | HWMOD_SWSUP_SIDLE_ACT,
	.prcm		= {
		.omap2 = {
			.module_offs = CORE_MOD,
			.idlest_reg_id = 1,
			.idlest_idle_bit = OMAP24XX_EN_UART2_SHIFT,
		},
	},
	.class		= &omap2_uart_class,
};

/* UART3 */

struct omap_hwmod omap2xxx_uart3_hwmod = {
	.name		= "uart3",
	.main_clk	= "uart3_fck",
	.flags		= DEBUG_OMAP2UART3_FLAGS | HWMOD_SWSUP_SIDLE_ACT,
	.prcm		= {
		.omap2 = {
			.module_offs = CORE_MOD,
			.idlest_reg_id = 2,
			.idlest_idle_bit = OMAP24XX_EN_UART3_SHIFT,
		},
	},
	.class		= &omap2_uart_class,
};

/* dss */

static struct omap_hwmod_opt_clk dss_opt_clks[] = {
	/*
	 * The DSS HW needs all DSS clocks enabled during reset. The dss_core
	 * driver does not use these clocks.
	 */
	{ .role = "tv_clk", .clk = "dss_54m_fck" },
	{ .role = "sys_clk", .clk = "dss2_fck" },
};

struct omap_hwmod omap2xxx_dss_core_hwmod = {
	.name		= "dss_core",
	.class		= &omap2_dss_hwmod_class,
	.main_clk	= "dss1_fck", /* instead of dss_fck */
	.prcm		= {
		.omap2 = {
			.module_offs = CORE_MOD,
			.idlest_reg_id = 1,
		},
	},
	.opt_clks	= dss_opt_clks,
	.opt_clks_cnt = ARRAY_SIZE(dss_opt_clks),
	.flags		= HWMOD_NO_IDLEST | HWMOD_CONTROL_OPT_CLKS_IN_RESET,
};

struct omap_hwmod omap2xxx_dss_dispc_hwmod = {
	.name		= "dss_dispc",
	.class		= &omap2_dispc_hwmod_class,
	.main_clk	= "dss1_fck",
	.prcm		= {
		.omap2 = {
			.module_offs = CORE_MOD,
			.idlest_reg_id = 1,
		},
	},
	.flags		= HWMOD_NO_IDLEST,
	.dev_attr	= &omap2_3_dss_dispc_dev_attr,
};

static struct omap_hwmod_opt_clk dss_rfbi_opt_clks[] = {
	{ .role = "ick", .clk = "dss_ick" },
};

struct omap_hwmod omap2xxx_dss_rfbi_hwmod = {
	.name		= "dss_rfbi",
	.class		= &omap2_rfbi_hwmod_class,
	.main_clk	= "dss1_fck",
	.prcm		= {
		.omap2 = {
			.module_offs = CORE_MOD,
		},
	},
	.opt_clks	= dss_rfbi_opt_clks,
	.opt_clks_cnt	= ARRAY_SIZE(dss_rfbi_opt_clks),
	.flags		= HWMOD_NO_IDLEST,
};

struct omap_hwmod omap2xxx_dss_venc_hwmod = {
	.name		= "dss_venc",
	.class		= &omap2_venc_hwmod_class,
	.main_clk	= "dss_54m_fck",
	.prcm		= {
		.omap2 = {
			.module_offs = CORE_MOD,
		},
	},
	.flags		= HWMOD_NO_IDLEST,
};

/* gpio1 */
struct omap_hwmod omap2xxx_gpio1_hwmod = {
	.name		= "gpio1",
	.flags		= HWMOD_CONTROL_OPT_CLKS_IN_RESET,
	.main_clk	= "gpios_fck",
	.prcm		= {
		.omap2 = {
			.module_offs = WKUP_MOD,
			.idlest_reg_id = 1,
			.idlest_idle_bit = OMAP24XX_ST_GPIOS_SHIFT,
		},
	},
	.class		= &omap2xxx_gpio_hwmod_class,
};

/* gpio2 */
struct omap_hwmod omap2xxx_gpio2_hwmod = {
	.name		= "gpio2",
	.flags		= HWMOD_CONTROL_OPT_CLKS_IN_RESET,
	.main_clk	= "gpios_fck",
	.prcm		= {
		.omap2 = {
			.module_offs = WKUP_MOD,
			.idlest_reg_id = 1,
			.idlest_idle_bit = OMAP24XX_ST_GPIOS_SHIFT,
		},
	},
	.class		= &omap2xxx_gpio_hwmod_class,
};

/* gpio3 */
struct omap_hwmod omap2xxx_gpio3_hwmod = {
	.name		= "gpio3",
	.flags		= HWMOD_CONTROL_OPT_CLKS_IN_RESET,
	.main_clk	= "gpios_fck",
	.prcm		= {
		.omap2 = {
			.module_offs = WKUP_MOD,
			.idlest_reg_id = 1,
			.idlest_idle_bit = OMAP24XX_ST_GPIOS_SHIFT,
		},
	},
	.class		= &omap2xxx_gpio_hwmod_class,
};

/* gpio4 */
struct omap_hwmod omap2xxx_gpio4_hwmod = {
	.name		= "gpio4",
	.flags		= HWMOD_CONTROL_OPT_CLKS_IN_RESET,
	.main_clk	= "gpios_fck",
	.prcm		= {
		.omap2 = {
			.module_offs = WKUP_MOD,
			.idlest_reg_id = 1,
			.idlest_idle_bit = OMAP24XX_ST_GPIOS_SHIFT,
		},
	},
	.class		= &omap2xxx_gpio_hwmod_class,
};

/* mcspi1 */
struct omap_hwmod omap2xxx_mcspi1_hwmod = {
	.name		= "mcspi1",
	.main_clk	= "mcspi1_fck",
	.prcm		= {
		.omap2 = {
			.module_offs = CORE_MOD,
			.idlest_reg_id = 1,
			.idlest_idle_bit = OMAP24XX_ST_MCSPI1_SHIFT,
		},
	},
	.class		= &omap2xxx_mcspi_class,
};

/* mcspi2 */
struct omap_hwmod omap2xxx_mcspi2_hwmod = {
	.name		= "mcspi2",
	.main_clk	= "mcspi2_fck",
	.prcm		= {
		.omap2 = {
			.module_offs = CORE_MOD,
			.idlest_reg_id = 1,
			.idlest_idle_bit = OMAP24XX_ST_MCSPI2_SHIFT,
		},
	},
	.class		= &omap2xxx_mcspi_class,
};

/* gpmc */
struct omap_hwmod omap2xxx_gpmc_hwmod = {
	.name		= "gpmc",
	.class		= &omap2xxx_gpmc_hwmod_class,
	.main_clk	= "gpmc_fck",
	/* Skip reset for CONFIG_OMAP_GPMC_DEBUG for bootloader timings */
	.flags		= HWMOD_NO_IDLEST | DEBUG_OMAP_GPMC_HWMOD_FLAGS,
	.prcm		= {
		.omap2	= {
			.module_offs = CORE_MOD,
		},
	},
};

/* RNG */

static struct omap_hwmod_class_sysconfig omap2_rng_sysc = {
	.rev_offs	= 0x3c,
	.sysc_offs	= 0x40,
	.syss_offs	= 0x44,
	.sysc_flags	= (SYSC_HAS_SOFTRESET | SYSC_HAS_AUTOIDLE |
			   SYSS_HAS_RESET_STATUS),
	.sysc_fields	= &omap_hwmod_sysc_type1,
};

static struct omap_hwmod_class omap2_rng_hwmod_class = {
	.name		= "rng",
	.sysc		= &omap2_rng_sysc,
};

struct omap_hwmod omap2xxx_rng_hwmod = {
	.name		= "rng",
	.main_clk	= "l4_ck",
	.prcm		= {
		.omap2 = {
			.module_offs = CORE_MOD,
			.idlest_reg_id = 4,
			.idlest_idle_bit = OMAP24XX_ST_RNG_SHIFT,
		},
	},
	/*
	 * XXX The first read from the SYSSTATUS register of the RNG
	 * after the SYSCONFIG SOFTRESET bit is set triggers an
	 * imprecise external abort.  It's unclear why this happens.
	 * Until this is analyzed, skip the IP block reset.
	 */
	.flags		= HWMOD_INIT_NO_RESET,
	.class		= &omap2_rng_hwmod_class,
};

/* SHAM */

static struct omap_hwmod_class_sysconfig omap2_sham_sysc = {
	.rev_offs	= 0x5c,
	.sysc_offs	= 0x60,
	.syss_offs	= 0x64,
	.sysc_flags	= (SYSC_HAS_SOFTRESET | SYSC_HAS_AUTOIDLE |
			   SYSS_HAS_RESET_STATUS),
	.sysc_fields	= &omap_hwmod_sysc_type1,
};

static struct omap_hwmod_class omap2xxx_sham_class = {
	.name	= "sham",
	.sysc	= &omap2_sham_sysc,
};

struct omap_hwmod omap2xxx_sham_hwmod = {
	.name		= "sham",
	.main_clk	= "l4_ck",
	.prcm		= {
		.omap2 = {
			.module_offs = CORE_MOD,
			.idlest_reg_id = 4,
			.idlest_idle_bit = OMAP24XX_ST_SHA_SHIFT,
		},
	},
	.class		= &omap2xxx_sham_class,
};

/* AES */

static struct omap_hwmod_class_sysconfig omap2_aes_sysc = {
	.rev_offs	= 0x44,
	.sysc_offs	= 0x48,
	.syss_offs	= 0x4c,
	.sysc_flags	= (SYSC_HAS_SOFTRESET | SYSC_HAS_AUTOIDLE |
			   SYSS_HAS_RESET_STATUS),
	.sysc_fields	= &omap_hwmod_sysc_type1,
};

static struct omap_hwmod_class omap2xxx_aes_class = {
	.name	= "aes",
	.sysc	= &omap2_aes_sysc,
};

struct omap_hwmod omap2xxx_aes_hwmod = {
	.name		= "aes",
	.main_clk	= "l4_ck",
	.prcm		= {
		.omap2 = {
			.module_offs = CORE_MOD,
			.idlest_reg_id = 4,
			.idlest_idle_bit = OMAP24XX_ST_AES_SHIFT,
		},
	},
	.class		= &omap2xxx_aes_class,
};
