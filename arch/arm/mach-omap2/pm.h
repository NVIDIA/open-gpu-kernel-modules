/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * OMAP2/3 Power Management Routines
 *
 * Copyright (C) 2008 Nokia Corporation
 * Jouni Hogander
 */
#ifndef __ARCH_ARM_MACH_OMAP2_PM_H
#define __ARCH_ARM_MACH_OMAP2_PM_H

#include <linux/err.h>

#include "powerdomain.h"

#ifdef CONFIG_CPU_IDLE
extern int __init omap3_idle_init(void);
extern int __init omap4_idle_init(void);
#else
static inline int omap3_idle_init(void)
{
	return 0;
}

static inline int omap4_idle_init(void)
{
	return 0;
}
#endif

extern void *omap3_secure_ram_storage;
extern void omap3_pm_off_mode_enable(int);
extern void omap_sram_idle(void);
extern int omap_pm_clkdms_setup(struct clockdomain *clkdm, void *unused);

#if defined(CONFIG_PM_OPP)
extern int omap3_opp_init(void);
extern int omap4_opp_init(void);
#else
static inline int omap3_opp_init(void)
{
	return -EINVAL;
}
static inline int omap4_opp_init(void)
{
	return -EINVAL;
}
#endif

extern int omap3_pm_get_suspend_state(struct powerdomain *pwrdm);
extern int omap3_pm_set_suspend_state(struct powerdomain *pwrdm, int state);

extern u32 enable_off_mode;

#if defined(CONFIG_PM_DEBUG) && defined(CONFIG_DEBUG_FS)
extern void pm_dbg_update_time(struct powerdomain *pwrdm, int prev);
#else
#define pm_dbg_update_time(pwrdm, prev) do {} while (0);
#endif /* CONFIG_PM_DEBUG */

/* 24xx */
extern void omap24xx_idle_loop_suspend(void);
extern unsigned int omap24xx_idle_loop_suspend_sz;

extern void omap24xx_cpu_suspend(u32 dll_ctrl, void __iomem *sdrc_dlla_ctrl,
					void __iomem *sdrc_power);
extern unsigned int omap24xx_cpu_suspend_sz;

/* 3xxx */
extern void omap34xx_cpu_suspend(int save_state);

/* omap3_do_wfi function pointer and size, for copy to SRAM */
extern void omap3_do_wfi(void);
extern unsigned int omap3_do_wfi_sz;
/* ... and its pointer from SRAM after copy */
extern void (*omap3_do_wfi_sram)(void);

extern struct am33xx_pm_sram_addr am33xx_pm_sram;
extern struct am33xx_pm_sram_addr am43xx_pm_sram;

extern void omap3_save_scratchpad_contents(void);

#define PM_RTA_ERRATUM_i608		(1 << 0)
#define PM_SDRC_WAKEUP_ERRATUM_i583	(1 << 1)
#define PM_PER_MEMORIES_ERRATUM_i582	(1 << 2)

#if defined(CONFIG_PM) && defined(CONFIG_ARCH_OMAP3)
extern u16 pm34xx_errata;
#define IS_PM34XX_ERRATUM(id)		(pm34xx_errata & (id))
extern void enable_omap3630_toggle_l2_on_restore(void);
#else
#define IS_PM34XX_ERRATUM(id)		0
static inline void enable_omap3630_toggle_l2_on_restore(void) { }
#endif		/* defined(CONFIG_PM) && defined(CONFIG_ARCH_OMAP3) */

#define PM_OMAP4_ROM_SMP_BOOT_ERRATUM_GICD	(1 << 0)
#define PM_OMAP4_CPU_OSWR_DISABLE		(1 << 1)

#if defined(CONFIG_PM) && (defined(CONFIG_ARCH_OMAP4) ||\
	   defined(CONFIG_SOC_OMAP5) || defined(CONFIG_SOC_DRA7XX))
extern u16 pm44xx_errata;
#define IS_PM44XX_ERRATUM(id)		(pm44xx_errata & (id))
#else
#define IS_PM44XX_ERRATUM(id)		0
#endif

#define OMAP4_VP_CONFIG_ERROROFFSET	0x00
#define OMAP4_VP_VSTEPMIN_VSTEPMIN	0x01
#define OMAP4_VP_VSTEPMAX_VSTEPMAX	0x04
#define OMAP4_VP_VLIMITTO_TIMEOUT_US	200

#ifdef CONFIG_POWER_AVS_OMAP
extern int omap_devinit_smartreflex(void);
extern void omap_enable_smartreflex_on_init(void);
#else
static inline int omap_devinit_smartreflex(void)
{
	return -EINVAL;
}

static inline void omap_enable_smartreflex_on_init(void) {}
#endif

#ifdef CONFIG_TWL4030_CORE
extern int omap3_twl_init(void);
extern int omap4_twl_init(void);
extern int omap3_twl_set_sr_bit(bool enable);
#else
static inline int omap3_twl_init(void)
{
	return -EINVAL;
}
static inline int omap4_twl_init(void)
{
	return -EINVAL;
}
#endif

#if IS_ENABLED(CONFIG_MFD_CPCAP)
extern int omap4_cpcap_init(void);
#else
static inline int omap4_cpcap_init(void)
{
	return -EINVAL;
}
#endif

#ifdef CONFIG_PM
extern void omap_pm_setup_oscillator(u32 tstart, u32 tshut);
extern void omap_pm_get_oscillator(u32 *tstart, u32 *tshut);
extern void omap_pm_setup_sr_i2c_pcb_length(u32 mm);
#else
static inline void omap_pm_setup_oscillator(u32 tstart, u32 tshut) { }
static inline void omap_pm_get_oscillator(u32 *tstart, u32 *tshut) { *tstart = *tshut = 0; }
static inline void omap_pm_setup_sr_i2c_pcb_length(u32 mm) { }
#endif

#ifdef CONFIG_SUSPEND
void omap_common_suspend_init(void *pm_suspend);
#else
static inline void omap_common_suspend_init(void *pm_suspend)
{
}
#endif /* CONFIG_SUSPEND */
#endif
