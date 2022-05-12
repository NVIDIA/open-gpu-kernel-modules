/*
 * TI clock drivers support
 *
 * Copyright (C) 2013 Texas Instruments, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#ifndef __LINUX_CLK_TI_H__
#define __LINUX_CLK_TI_H__

#include <linux/clk-provider.h>
#include <linux/clkdev.h>

/**
 * struct clk_omap_reg - OMAP register declaration
 * @offset: offset from the master IP module base address
 * @index: index of the master IP module
 */
struct clk_omap_reg {
	void __iomem *ptr;
	u16 offset;
	u8 index;
	u8 flags;
};

/**
 * struct dpll_data - DPLL registers and integration data
 * @mult_div1_reg: register containing the DPLL M and N bitfields
 * @mult_mask: mask of the DPLL M bitfield in @mult_div1_reg
 * @div1_mask: mask of the DPLL N bitfield in @mult_div1_reg
 * @clk_bypass: struct clk_hw pointer to the clock's bypass clock input
 * @clk_ref: struct clk_hw pointer to the clock's reference clock input
 * @control_reg: register containing the DPLL mode bitfield
 * @enable_mask: mask of the DPLL mode bitfield in @control_reg
 * @last_rounded_rate: cache of the last rate result of omap2_dpll_round_rate()
 * @last_rounded_m: cache of the last M result of omap2_dpll_round_rate()
 * @last_rounded_m4xen: cache of the last M4X result of
 *			omap4_dpll_regm4xen_round_rate()
 * @last_rounded_lpmode: cache of the last lpmode result of
 *			 omap4_dpll_lpmode_recalc()
 * @max_multiplier: maximum valid non-bypass multiplier value (actual)
 * @last_rounded_n: cache of the last N result of omap2_dpll_round_rate()
 * @min_divider: minimum valid non-bypass divider value (actual)
 * @max_divider: maximum valid non-bypass divider value (actual)
 * @max_rate: maximum clock rate for the DPLL
 * @modes: possible values of @enable_mask
 * @autoidle_reg: register containing the DPLL autoidle mode bitfield
 * @idlest_reg: register containing the DPLL idle status bitfield
 * @autoidle_mask: mask of the DPLL autoidle mode bitfield in @autoidle_reg
 * @freqsel_mask: mask of the DPLL jitter correction bitfield in @control_reg
 * @dcc_mask: mask of the DPLL DCC correction bitfield @mult_div1_reg
 * @dcc_rate: rate atleast which DCC @dcc_mask must be set
 * @idlest_mask: mask of the DPLL idle status bitfield in @idlest_reg
 * @lpmode_mask: mask of the DPLL low-power mode bitfield in @control_reg
 * @m4xen_mask: mask of the DPLL M4X multiplier bitfield in @control_reg
 * @auto_recal_bit: bitshift of the driftguard enable bit in @control_reg
 * @recal_en_bit: bitshift of the PRM_IRQENABLE_* bit for recalibration IRQs
 * @recal_st_bit: bitshift of the PRM_IRQSTATUS_* bit for recalibration IRQs
 * @flags: DPLL type/features (see below)
 *
 * Possible values for @flags:
 * DPLL_J_TYPE: "J-type DPLL" (only some 36xx, 4xxx DPLLs)
 *
 * @freqsel_mask is only used on the OMAP34xx family and AM35xx.
 *
 * XXX Some DPLLs have multiple bypass inputs, so it's not technically
 * correct to only have one @clk_bypass pointer.
 *
 * XXX The runtime-variable fields (@last_rounded_rate, @last_rounded_m,
 * @last_rounded_n) should be separated from the runtime-fixed fields
 * and placed into a different structure, so that the runtime-fixed data
 * can be placed into read-only space.
 */
struct dpll_data {
	struct clk_omap_reg	mult_div1_reg;
	u32			mult_mask;
	u32			div1_mask;
	struct clk_hw		*clk_bypass;
	struct clk_hw		*clk_ref;
	struct clk_omap_reg	control_reg;
	u32			enable_mask;
	unsigned long		last_rounded_rate;
	u16			last_rounded_m;
	u8			last_rounded_m4xen;
	u8			last_rounded_lpmode;
	u16			max_multiplier;
	u8			last_rounded_n;
	u8			min_divider;
	u16			max_divider;
	unsigned long		max_rate;
	u8			modes;
	struct clk_omap_reg	autoidle_reg;
	struct clk_omap_reg	idlest_reg;
	u32			autoidle_mask;
	u32			freqsel_mask;
	u32			idlest_mask;
	u32			dco_mask;
	u32			sddiv_mask;
	u32			dcc_mask;
	unsigned long		dcc_rate;
	u32			lpmode_mask;
	u32			m4xen_mask;
	u8			auto_recal_bit;
	u8			recal_en_bit;
	u8			recal_st_bit;
	u8			flags;
};

struct clk_hw_omap;

/**
 * struct clk_hw_omap_ops - OMAP clk ops
 * @find_idlest: find idlest register information for a clock
 * @find_companion: find companion clock register information for a clock,
 *		    basically converts CM_ICLKEN* <-> CM_FCLKEN*
 * @allow_idle: enables autoidle hardware functionality for a clock
 * @deny_idle: prevent autoidle hardware functionality for a clock
 */
struct clk_hw_omap_ops {
	void	(*find_idlest)(struct clk_hw_omap *oclk,
			       struct clk_omap_reg *idlest_reg,
			       u8 *idlest_bit, u8 *idlest_val);
	void	(*find_companion)(struct clk_hw_omap *oclk,
				  struct clk_omap_reg *other_reg,
				  u8 *other_bit);
	void	(*allow_idle)(struct clk_hw_omap *oclk);
	void	(*deny_idle)(struct clk_hw_omap *oclk);
};

/**
 * struct clk_hw_omap - OMAP struct clk
 * @node: list_head connecting this clock into the full clock list
 * @enable_reg: register to write to enable the clock (see @enable_bit)
 * @enable_bit: bitshift to write to enable/disable the clock (see @enable_reg)
 * @flags: see "struct clk.flags possibilities" above
 * @clksel_reg: for clksel clks, register va containing src/divisor select
 * @dpll_data: for DPLLs, pointer to struct dpll_data for this clock
 * @clkdm_name: clockdomain name that this clock is contained in
 * @clkdm: pointer to struct clockdomain, resolved from @clkdm_name at runtime
 * @ops: clock ops for this clock
 */
struct clk_hw_omap {
	struct clk_hw		hw;
	struct list_head	node;
	unsigned long		fixed_rate;
	u8			fixed_div;
	struct clk_omap_reg	enable_reg;
	u8			enable_bit;
	unsigned long		flags;
	struct clk_omap_reg	clksel_reg;
	struct dpll_data	*dpll_data;
	const char		*clkdm_name;
	struct clockdomain	*clkdm;
	const struct clk_hw_omap_ops	*ops;
	u32			context;
	int			autoidle_count;
};

/*
 * struct clk_hw_omap.flags possibilities
 *
 * XXX document the rest of the clock flags here
 *
 * ENABLE_REG_32BIT: (OMAP1 only) clock control register must be accessed
 *     with 32bit ops, by default OMAP1 uses 16bit ops.
 * CLOCK_IDLE_CONTROL: (OMAP1 only) clock has autoidle support.
 * CLOCK_NO_IDLE_PARENT: (OMAP1 only) when clock is enabled, its parent
 *     clock is put to no-idle mode.
 * ENABLE_ON_INIT: Clock is enabled on init.
 * INVERT_ENABLE: By default, clock enable bit behavior is '1' enable, '0'
 *     disable. This inverts the behavior making '0' enable and '1' disable.
 * CLOCK_CLKOUTX2: (OMAP4 only) DPLL CLKOUT and CLKOUTX2 GATE_CTRL
 *     bits share the same register.  This flag allows the
 *     omap4_dpllmx*() code to determine which GATE_CTRL bit field
 *     should be used.  This is a temporary solution - a better approach
 *     would be to associate clock type-specific data with the clock,
 *     similar to the struct dpll_data approach.
 */
#define ENABLE_REG_32BIT	(1 << 0)	/* Use 32-bit access */
#define CLOCK_IDLE_CONTROL	(1 << 1)
#define CLOCK_NO_IDLE_PARENT	(1 << 2)
#define ENABLE_ON_INIT		(1 << 3)	/* Enable upon framework init */
#define INVERT_ENABLE		(1 << 4)	/* 0 enables, 1 disables */
#define CLOCK_CLKOUTX2		(1 << 5)

/* CM_CLKEN_PLL*.EN* bit values - not all are available for every DPLL */
#define DPLL_LOW_POWER_STOP	0x1
#define DPLL_LOW_POWER_BYPASS	0x5
#define DPLL_LOCKED		0x7

/* DPLL Type and DCO Selection Flags */
#define DPLL_J_TYPE		0x1

/* Static memmap indices */
enum {
	TI_CLKM_CM = 0,
	TI_CLKM_CM2,
	TI_CLKM_PRM,
	TI_CLKM_SCRM,
	TI_CLKM_CTRL,
	TI_CLKM_CTRL_AUX,
	TI_CLKM_PLLSS,
	CLK_MAX_MEMMAPS
};

/**
 * struct ti_clk_ll_ops - low-level ops for clocks
 * @clk_readl: pointer to register read function
 * @clk_writel: pointer to register write function
 * @clk_rmw: pointer to register read-modify-write function
 * @clkdm_clk_enable: pointer to clockdomain enable function
 * @clkdm_clk_disable: pointer to clockdomain disable function
 * @clkdm_lookup: pointer to clockdomain lookup function
 * @cm_wait_module_ready: pointer to CM module wait ready function
 * @cm_split_idlest_reg: pointer to CM module function to split idlest reg
 *
 * Low-level ops are generally used by the basic clock types (clk-gate,
 * clk-mux, clk-divider etc.) to provide support for various low-level
 * hadrware interfaces (direct MMIO, regmap etc.), and is initialized
 * by board code. Low-level ops also contain some other platform specific
 * operations not provided directly by clock drivers.
 */
struct ti_clk_ll_ops {
	u32	(*clk_readl)(const struct clk_omap_reg *reg);
	void	(*clk_writel)(u32 val, const struct clk_omap_reg *reg);
	void	(*clk_rmw)(u32 val, u32 mask, const struct clk_omap_reg *reg);
	int	(*clkdm_clk_enable)(struct clockdomain *clkdm, struct clk *clk);
	int	(*clkdm_clk_disable)(struct clockdomain *clkdm,
				     struct clk *clk);
	struct clockdomain * (*clkdm_lookup)(const char *name);
	int	(*cm_wait_module_ready)(u8 part, s16 prcm_mod, u16 idlest_reg,
					u8 idlest_shift);
	int	(*cm_split_idlest_reg)(struct clk_omap_reg *idlest_reg,
				       s16 *prcm_inst, u8 *idlest_reg_id);
};

#define to_clk_hw_omap(_hw) container_of(_hw, struct clk_hw_omap, hw)

bool omap2_clk_is_hw_omap(struct clk_hw *hw);
int omap2_clk_disable_autoidle_all(void);
int omap2_clk_enable_autoidle_all(void);
int omap2_clk_allow_idle(struct clk *clk);
int omap2_clk_deny_idle(struct clk *clk);
unsigned long omap2_dpllcore_recalc(struct clk_hw *hw,
				    unsigned long parent_rate);
int omap2_reprogram_dpllcore(struct clk_hw *clk, unsigned long rate,
			     unsigned long parent_rate);
void omap2xxx_clkt_dpllcore_init(struct clk_hw *hw);
void omap2xxx_clkt_vps_init(void);
unsigned long omap2_get_dpll_rate(struct clk_hw_omap *clk);

void ti_dt_clk_init_retry_clks(void);
void ti_dt_clockdomains_setup(void);
int ti_clk_setup_ll_ops(struct ti_clk_ll_ops *ops);

struct regmap;

int omap2_clk_provider_init(struct device_node *parent, int index,
			    struct regmap *syscon, void __iomem *mem);
void omap2_clk_legacy_provider_init(int index, void __iomem *mem);

int omap3430_dt_clk_init(void);
int omap3630_dt_clk_init(void);
int am35xx_dt_clk_init(void);
int dm814x_dt_clk_init(void);
int dm816x_dt_clk_init(void);
int omap4xxx_dt_clk_init(void);
int omap5xxx_dt_clk_init(void);
int dra7xx_dt_clk_init(void);
int am33xx_dt_clk_init(void);
int am43xx_dt_clk_init(void);
int omap2420_dt_clk_init(void);
int omap2430_dt_clk_init(void);

struct ti_clk_features {
	u32 flags;
	long fint_min;
	long fint_max;
	long fint_band1_max;
	long fint_band2_min;
	u8 dpll_bypass_vals;
	u8 cm_idlest_val;
};

#define TI_CLK_DPLL_HAS_FREQSEL			BIT(0)
#define TI_CLK_DPLL4_DENY_REPROGRAM		BIT(1)
#define TI_CLK_DISABLE_CLKDM_CONTROL		BIT(2)
#define TI_CLK_ERRATA_I810			BIT(3)
#define TI_CLK_CLKCTRL_COMPAT			BIT(4)
#define TI_CLK_DEVICE_TYPE_GP			BIT(5)

void ti_clk_setup_features(struct ti_clk_features *features);
const struct ti_clk_features *ti_clk_get_features(void);
bool ti_clk_is_in_standby(struct clk *clk);
int omap3_noncore_dpll_save_context(struct clk_hw *hw);
void omap3_noncore_dpll_restore_context(struct clk_hw *hw);

int omap3_core_dpll_save_context(struct clk_hw *hw);
void omap3_core_dpll_restore_context(struct clk_hw *hw);

extern const struct clk_hw_omap_ops clkhwops_omap2xxx_dpll;

#ifdef CONFIG_ATAGS
int omap3430_clk_legacy_init(void);
int omap3430es1_clk_legacy_init(void);
int omap36xx_clk_legacy_init(void);
int am35xx_clk_legacy_init(void);
#else
static inline int omap3430_clk_legacy_init(void) { return -ENXIO; }
static inline int omap3430es1_clk_legacy_init(void) { return -ENXIO; }
static inline int omap36xx_clk_legacy_init(void) { return -ENXIO; }
static inline int am35xx_clk_legacy_init(void) { return -ENXIO; }
#endif


#endif
