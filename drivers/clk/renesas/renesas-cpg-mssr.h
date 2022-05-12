/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Renesas Clock Pulse Generator / Module Standby and Software Reset
 *
 * Copyright (C) 2015 Glider bvba
 */

#ifndef __CLK_RENESAS_CPG_MSSR_H__
#define __CLK_RENESAS_CPG_MSSR_H__

    /*
     * Definitions of CPG Core Clocks
     *
     * These include:
     *   - Clock outputs exported to DT
     *   - External input clocks
     *   - Internal CPG clocks
     */

struct cpg_core_clk {
	/* Common */
	const char *name;
	unsigned int id;
	unsigned int type;
	/* Depending on type */
	unsigned int parent;	/* Core Clocks only */
	unsigned int div;
	unsigned int mult;
	unsigned int offset;
};

enum clk_types {
	/* Generic */
	CLK_TYPE_IN,		/* External Clock Input */
	CLK_TYPE_FF,		/* Fixed Factor Clock */
	CLK_TYPE_DIV6P1,	/* DIV6 Clock with 1 parent clock */
	CLK_TYPE_DIV6_RO,	/* DIV6 Clock read only with extra divisor */
	CLK_TYPE_FR,		/* Fixed Rate Clock */

	/* Custom definitions start here */
	CLK_TYPE_CUSTOM,
};

#define DEF_TYPE(_name, _id, _type...)	\
	{ .name = _name, .id = _id, .type = _type }
#define DEF_BASE(_name, _id, _type, _parent...)	\
	DEF_TYPE(_name, _id, _type, .parent = _parent)

#define DEF_INPUT(_name, _id) \
	DEF_TYPE(_name, _id, CLK_TYPE_IN)
#define DEF_FIXED(_name, _id, _parent, _div, _mult)	\
	DEF_BASE(_name, _id, CLK_TYPE_FF, _parent, .div = _div, .mult = _mult)
#define DEF_DIV6P1(_name, _id, _parent, _offset)	\
	DEF_BASE(_name, _id, CLK_TYPE_DIV6P1, _parent, .offset = _offset)
#define DEF_DIV6_RO(_name, _id, _parent, _offset, _div)	\
	DEF_BASE(_name, _id, CLK_TYPE_DIV6_RO, _parent, .offset = _offset, .div = _div, .mult = 1)
#define DEF_RATE(_name, _id, _rate)	\
	DEF_TYPE(_name, _id, CLK_TYPE_FR, .mult = _rate)

    /*
     * Definitions of Module Clocks
     */

struct mssr_mod_clk {
	const char *name;
	unsigned int id;
	unsigned int parent;	/* Add MOD_CLK_BASE for Module Clocks */
};

/* Convert from sparse base-100 to packed index space */
#define MOD_CLK_PACK(x)	((x) - ((x) / 100) * (100 - 32))

#define MOD_CLK_ID(x)	(MOD_CLK_BASE + MOD_CLK_PACK(x))

#define DEF_MOD(_name, _mod, _parent...)	\
	{ .name = _name, .id = MOD_CLK_ID(_mod), .parent = _parent }

/* Convert from sparse base-10 to packed index space */
#define MOD_CLK_PACK_10(x)	((x / 10) * 32 + (x % 10))

#define MOD_CLK_ID_10(x)	(MOD_CLK_BASE + MOD_CLK_PACK_10(x))

#define DEF_MOD_STB(_name, _mod, _parent...)	\
	{ .name = _name, .id = MOD_CLK_ID_10(_mod), .parent = _parent }

struct device_node;

enum clk_reg_layout {
	CLK_REG_LAYOUT_RCAR_GEN2_AND_GEN3 = 0,
	CLK_REG_LAYOUT_RZ_A,
	CLK_REG_LAYOUT_RCAR_V3U,
};

    /**
     * SoC-specific CPG/MSSR Description
     *
     * @early_core_clks: Array of Early Core Clock definitions
     * @num_early_core_clks: Number of entries in early_core_clks[]
     * @early_mod_clks: Array of Early Module Clock definitions
     * @num_early_mod_clks: Number of entries in early_mod_clks[]
     *
     * @core_clks: Array of Core Clock definitions
     * @num_core_clks: Number of entries in core_clks[]
     * @last_dt_core_clk: ID of the last Core Clock exported to DT
     * @num_total_core_clks: Total number of Core Clocks (exported + internal)
     *
     * @mod_clks: Array of Module Clock definitions
     * @num_mod_clks: Number of entries in mod_clks[]
     * @num_hw_mod_clks: Number of Module Clocks supported by the hardware
     *
     * @crit_mod_clks: Array with Module Clock IDs of critical clocks that
     *                 should not be disabled without a knowledgeable driver
     * @num_crit_mod_clks: Number of entries in crit_mod_clks[]
     * @reg_layout: CPG/MSSR register layout from enum clk_reg_layout
     *
     * @core_pm_clks: Array with IDs of Core Clocks that are suitable for Power
     *                Management, in addition to Module Clocks
     * @num_core_pm_clks: Number of entries in core_pm_clks[]
     *
     * @init: Optional callback to perform SoC-specific initialization
     * @cpg_clk_register: Optional callback to handle special Core Clock types
     */

struct cpg_mssr_info {
	/* Early Clocks */
	const struct cpg_core_clk *early_core_clks;
	unsigned int num_early_core_clks;
	const struct mssr_mod_clk *early_mod_clks;
	unsigned int num_early_mod_clks;

	/* Core Clocks */
	const struct cpg_core_clk *core_clks;
	unsigned int num_core_clks;
	unsigned int last_dt_core_clk;
	unsigned int num_total_core_clks;
	enum clk_reg_layout reg_layout;

	/* Module Clocks */
	const struct mssr_mod_clk *mod_clks;
	unsigned int num_mod_clks;
	unsigned int num_hw_mod_clks;

	/* Critical Module Clocks that should not be disabled */
	const unsigned int *crit_mod_clks;
	unsigned int num_crit_mod_clks;

	/* Core Clocks suitable for PM, in addition to the Module Clocks */
	const unsigned int *core_pm_clks;
	unsigned int num_core_pm_clks;

	/* Callbacks */
	int (*init)(struct device *dev);
	struct clk *(*cpg_clk_register)(struct device *dev,
					const struct cpg_core_clk *core,
					const struct cpg_mssr_info *info,
					struct clk **clks, void __iomem *base,
					struct raw_notifier_head *notifiers);
};

extern const struct cpg_mssr_info r7s9210_cpg_mssr_info;
extern const struct cpg_mssr_info r8a7742_cpg_mssr_info;
extern const struct cpg_mssr_info r8a7743_cpg_mssr_info;
extern const struct cpg_mssr_info r8a7745_cpg_mssr_info;
extern const struct cpg_mssr_info r8a77470_cpg_mssr_info;
extern const struct cpg_mssr_info r8a774a1_cpg_mssr_info;
extern const struct cpg_mssr_info r8a774b1_cpg_mssr_info;
extern const struct cpg_mssr_info r8a774c0_cpg_mssr_info;
extern const struct cpg_mssr_info r8a774e1_cpg_mssr_info;
extern const struct cpg_mssr_info r8a7790_cpg_mssr_info;
extern const struct cpg_mssr_info r8a7791_cpg_mssr_info;
extern const struct cpg_mssr_info r8a7792_cpg_mssr_info;
extern const struct cpg_mssr_info r8a7794_cpg_mssr_info;
extern const struct cpg_mssr_info r8a7795_cpg_mssr_info;
extern const struct cpg_mssr_info r8a7796_cpg_mssr_info;
extern const struct cpg_mssr_info r8a77965_cpg_mssr_info;
extern const struct cpg_mssr_info r8a77970_cpg_mssr_info;
extern const struct cpg_mssr_info r8a77980_cpg_mssr_info;
extern const struct cpg_mssr_info r8a77990_cpg_mssr_info;
extern const struct cpg_mssr_info r8a77995_cpg_mssr_info;
extern const struct cpg_mssr_info r8a779a0_cpg_mssr_info;

void __init cpg_mssr_early_init(struct device_node *np,
				const struct cpg_mssr_info *info);

    /*
     * Helpers for fixing up clock tables depending on SoC revision
     */

struct mssr_mod_reparent {
	unsigned int clk, parent;
};


extern void cpg_core_nullify_range(struct cpg_core_clk *core_clks,
				   unsigned int num_core_clks,
				   unsigned int first_clk,
				   unsigned int last_clk);
extern void mssr_mod_nullify(struct mssr_mod_clk *mod_clks,
			     unsigned int num_mod_clks,
			     const unsigned int *clks, unsigned int n);
extern void mssr_mod_reparent(struct mssr_mod_clk *mod_clks,
			      unsigned int num_mod_clks,
			      const struct mssr_mod_reparent *clks,
			      unsigned int n);
#endif
