/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef __ARCH_ARM_MACH_OMAP2_SDRC_H
#define __ARCH_ARM_MACH_OMAP2_SDRC_H

/*
 * OMAP2/3 SDRC/SMS macros and prototypes
 *
 * Copyright (C) 2007-2008, 2012 Texas Instruments, Inc.
 * Copyright (C) 2007-2008 Nokia Corporation
 *
 * Paul Walmsley
 * Tony Lindgren
 * Richard Woodruff
 */
#undef DEBUG

#ifndef __ASSEMBLER__

#include <linux/io.h>

extern void __iomem *omap2_sdrc_base;
extern void __iomem *omap2_sms_base;

#define OMAP_SDRC_REGADDR(reg)			(omap2_sdrc_base + (reg))
#define OMAP_SMS_REGADDR(reg)			(omap2_sms_base + (reg))

/* SDRC global register get/set */

static inline void sdrc_write_reg(u32 val, u16 reg)
{
	writel_relaxed(val, OMAP_SDRC_REGADDR(reg));
}

static inline u32 sdrc_read_reg(u16 reg)
{
	return readl_relaxed(OMAP_SDRC_REGADDR(reg));
}

/* SMS global register get/set */

static inline void sms_write_reg(u32 val, u16 reg)
{
	writel_relaxed(val, OMAP_SMS_REGADDR(reg));
}

static inline u32 sms_read_reg(u16 reg)
{
	return readl_relaxed(OMAP_SMS_REGADDR(reg));
}

extern void omap2_set_globals_sdrc(void __iomem *sdrc, void __iomem *sms);


/**
 * struct omap_sdrc_params - SDRC parameters for a given SDRC clock rate
 * @rate: SDRC clock rate (in Hz)
 * @actim_ctrla: Value to program to SDRC_ACTIM_CTRLA for this rate
 * @actim_ctrlb: Value to program to SDRC_ACTIM_CTRLB for this rate
 * @rfr_ctrl: Value to program to SDRC_RFR_CTRL for this rate
 * @mr: Value to program to SDRC_MR for this rate
 *
 * This structure holds a pre-computed set of register values for the
 * SDRC for a given SDRC clock rate and SDRAM chip.  These are
 * intended to be pre-computed and specified in an array in the board-*.c
 * files.  The structure is keyed off the 'rate' field.
 */
struct omap_sdrc_params {
	unsigned long rate;
	u32 actim_ctrla;
	u32 actim_ctrlb;
	u32 rfr_ctrl;
	u32 mr;
};

#ifdef CONFIG_SOC_HAS_OMAP2_SDRC
void omap2_sdrc_init(struct omap_sdrc_params *sdrc_cs0,
			    struct omap_sdrc_params *sdrc_cs1);
#else
static inline void __init omap2_sdrc_init(struct omap_sdrc_params *sdrc_cs0,
					  struct omap_sdrc_params *sdrc_cs1) {};
#endif

int omap2_sdrc_get_params(unsigned long r,
			  struct omap_sdrc_params **sdrc_cs0,
			  struct omap_sdrc_params **sdrc_cs1);
void omap2_sms_save_context(void);
void omap2_sms_restore_context(void);

struct memory_timings {
	u32 m_type;		/* ddr = 1, sdr = 0 */
	u32 dll_mode;		/* use lock mode = 1, unlock mode = 0 */
	u32 slow_dll_ctrl;	/* unlock mode, dll value for slow speed */
	u32 fast_dll_ctrl;	/* unlock mode, dll value for fast speed */
	u32 base_cs;		/* base chip select to use for calculations */
};

extern void omap2xxx_sdrc_init_params(u32 force_lock_to_unlock_mode);
struct omap_sdrc_params *rx51_get_sdram_timings(void);

u32 omap2xxx_sdrc_dll_is_unlocked(void);
u32 omap2xxx_sdrc_reprogram(u32 level, u32 force);


#else
#define OMAP242X_SDRC_REGADDR(reg)					\
			OMAP2_L3_IO_ADDRESS(OMAP2420_SDRC_BASE + (reg))
#define OMAP243X_SDRC_REGADDR(reg)					\
			OMAP2_L3_IO_ADDRESS(OMAP243X_SDRC_BASE + (reg))
#define OMAP34XX_SDRC_REGADDR(reg)					\
			OMAP2_L3_IO_ADDRESS(OMAP343X_SDRC_BASE + (reg))

#endif	/* __ASSEMBLER__ */

/* Minimum frequency that the SDRC DLL can lock at */
#define MIN_SDRC_DLL_LOCK_FREQ		83000000

/* Scale factor for fixed-point arith in omap3_core_dpll_m2_set_rate() */
#define SDRC_MPURATE_SCALE		8

/* 2^SDRC_MPURATE_BASE_SHIFT: MPU MHz that SDRC_MPURATE_LOOPS is defined for */
#define SDRC_MPURATE_BASE_SHIFT		9

/*
 * SDRC_MPURATE_LOOPS: Number of MPU loops to execute at
 * 2^MPURATE_BASE_SHIFT MHz for SDRC to stabilize
 */
#define SDRC_MPURATE_LOOPS		96

/* SDRC register offsets - read/write with sdrc_{read,write}_reg() */

#define SDRC_SYSCONFIG		0x010
#define SDRC_CS_CFG		0x040
#define SDRC_SHARING		0x044
#define SDRC_ERR_TYPE		0x04C
#define SDRC_DLLA_CTRL		0x060
#define SDRC_DLLA_STATUS	0x064
#define SDRC_DLLB_CTRL		0x068
#define SDRC_DLLB_STATUS	0x06C
#define SDRC_POWER		0x070
#define SDRC_MCFG_0		0x080
#define SDRC_MR_0		0x084
#define SDRC_EMR2_0		0x08c
#define SDRC_ACTIM_CTRL_A_0	0x09c
#define SDRC_ACTIM_CTRL_B_0	0x0a0
#define SDRC_RFR_CTRL_0		0x0a4
#define SDRC_MANUAL_0		0x0a8
#define SDRC_MCFG_1		0x0B0
#define SDRC_MR_1		0x0B4
#define SDRC_EMR2_1		0x0BC
#define SDRC_ACTIM_CTRL_A_1	0x0C4
#define SDRC_ACTIM_CTRL_B_1	0x0C8
#define SDRC_RFR_CTRL_1		0x0D4
#define SDRC_MANUAL_1		0x0D8

#define SDRC_POWER_AUTOCOUNT_SHIFT	8
#define SDRC_POWER_AUTOCOUNT_MASK	(0xffff << SDRC_POWER_AUTOCOUNT_SHIFT)
#define SDRC_POWER_CLKCTRL_SHIFT	4
#define SDRC_POWER_CLKCTRL_MASK		(0x3 << SDRC_POWER_CLKCTRL_SHIFT)
#define SDRC_SELF_REFRESH_ON_AUTOCOUNT	(0x2 << SDRC_POWER_CLKCTRL_SHIFT)

/*
 * These values represent the number of memory clock cycles between
 * autorefresh initiation.  They assume 1 refresh per 64 ms (JEDEC), 8192
 * rows per device, and include a subtraction of a 50 cycle window in the
 * event that the autorefresh command is delayed due to other SDRC activity.
 * The '| 1' sets the ARE field to send one autorefresh when the autorefresh
 * counter reaches 0.
 *
 * These represent optimal values for common parts, it won't work for all.
 * As long as you scale down, most parameters are still work, they just
 * become sub-optimal. The RFR value goes in the opposite direction. If you
 * don't adjust it down as your clock period increases the refresh interval
 * will not be met. Setting all parameters for complete worst case may work,
 * but may cut memory performance by 2x. Due to errata the DLLs need to be
 * unlocked and their value needs run time calibration. A dynamic call is
 * need for that as no single right value exists across production samples.
 *
 * Only the FULL speed values are given. Current code is such that rate
 * changes must be made at DPLLoutx2. The actual value adjustment for low
 * frequency operation will be handled by omap_set_performance()
 *
 * By having the boot loader boot up in the fastest L4 speed available likely
 * will result in something which you can switch between.
 */
#define SDRC_RFR_CTRL_165MHz	(0x00044c00 | 1)
#define SDRC_RFR_CTRL_133MHz	(0x0003de00 | 1)
#define SDRC_RFR_CTRL_100MHz	(0x0002da01 | 1)
#define SDRC_RFR_CTRL_110MHz	(0x0002da01 | 1) /* Need to calc */
#define SDRC_RFR_CTRL_BYPASS	(0x00005000 | 1) /* Need to calc */


/*
 * SMS register access
 */

#define OMAP242X_SMS_REGADDR(reg)					\
		(void __iomem *)OMAP2_L3_IO_ADDRESS(OMAP2420_SMS_BASE + reg)
#define OMAP243X_SMS_REGADDR(reg)					\
		(void __iomem *)OMAP2_L3_IO_ADDRESS(OMAP243X_SMS_BASE + reg)
#define OMAP343X_SMS_REGADDR(reg)					\
		(void __iomem *)OMAP2_L3_IO_ADDRESS(OMAP343X_SMS_BASE + reg)

/* SMS register offsets - read/write with sms_{read,write}_reg() */

#define SMS_SYSCONFIG			0x010
/* REVISIT: fill in other SMS registers here */



#endif
