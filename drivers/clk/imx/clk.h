/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __MACH_IMX_CLK_H
#define __MACH_IMX_CLK_H

#include <linux/bits.h>
#include <linux/spinlock.h>
#include <linux/clk-provider.h>

extern spinlock_t imx_ccm_lock;

void imx_check_clocks(struct clk *clks[], unsigned int count);
void imx_check_clk_hws(struct clk_hw *clks[], unsigned int count);
#ifndef MODULE
void imx_register_uart_clocks(unsigned int clk_count);
#else
static inline void imx_register_uart_clocks(unsigned int clk_count)
{
}
#endif
void imx_mmdc_mask_handshake(void __iomem *ccm_base, unsigned int chn);
void imx_unregister_clocks(struct clk *clks[], unsigned int count);
void imx_unregister_hw_clocks(struct clk_hw *hws[], unsigned int count);

extern void imx_cscmr1_fixup(u32 *val);

enum imx_pllv1_type {
	IMX_PLLV1_IMX1,
	IMX_PLLV1_IMX21,
	IMX_PLLV1_IMX25,
	IMX_PLLV1_IMX27,
	IMX_PLLV1_IMX31,
	IMX_PLLV1_IMX35,
};

enum imx_sscg_pll_type {
	SCCG_PLL1,
	SCCG_PLL2,
};

enum imx_pll14xx_type {
	PLL_1416X,
	PLL_1443X,
};

/* NOTE: Rate table should be kept sorted in descending order. */
struct imx_pll14xx_rate_table {
	unsigned int rate;
	unsigned int pdiv;
	unsigned int mdiv;
	unsigned int sdiv;
	unsigned int kdiv;
};

struct imx_pll14xx_clk {
	enum imx_pll14xx_type type;
	const struct imx_pll14xx_rate_table *rate_table;
	int rate_count;
	int flags;
};

extern struct imx_pll14xx_clk imx_1416x_pll;
extern struct imx_pll14xx_clk imx_1443x_pll;
extern struct imx_pll14xx_clk imx_1443x_dram_pll;

#define imx_clk_cpu(name, parent_name, div, mux, pll, step) \
	to_clk(imx_clk_hw_cpu(name, parent_name, div, mux, pll, step))

#define clk_register_gate2(dev, name, parent_name, flags, reg, bit_idx, \
				cgr_val, cgr_mask, clk_gate_flags, lock, share_count) \
	to_clk(clk_hw_register_gate2(dev, name, parent_name, flags, reg, bit_idx, \
				cgr_val, cgr_mask, clk_gate_flags, lock, share_count))

#define imx_clk_pllv3(type, name, parent_name, base, div_mask) \
	to_clk(imx_clk_hw_pllv3(type, name, parent_name, base, div_mask))

#define imx_clk_pfd(name, parent_name, reg, idx) \
	to_clk(imx_clk_hw_pfd(name, parent_name, reg, idx))

#define imx_clk_gate_exclusive(name, parent, reg, shift, exclusive_mask) \
	to_clk(imx_clk_hw_gate_exclusive(name, parent, reg, shift, exclusive_mask))

#define imx_clk_fixed(name, rate) \
	to_clk(imx_clk_hw_fixed(name, rate))

#define imx_clk_fixed_factor(name, parent, mult, div) \
	to_clk(imx_clk_hw_fixed_factor(name, parent, mult, div))

#define imx_clk_divider(name, parent, reg, shift, width) \
	to_clk(imx_clk_hw_divider(name, parent, reg, shift, width))

#define imx_clk_divider2(name, parent, reg, shift, width) \
	to_clk(imx_clk_hw_divider2(name, parent, reg, shift, width))

#define imx_clk_divider_flags(name, parent, reg, shift, width, flags) \
	to_clk(imx_clk_hw_divider_flags(name, parent, reg, shift, width, flags))

#define imx_clk_gate(name, parent, reg, shift) \
	to_clk(imx_clk_hw_gate(name, parent, reg, shift))

#define imx_clk_gate_dis(name, parent, reg, shift) \
	to_clk(imx_clk_hw_gate_dis(name, parent, reg, shift))

#define imx_clk_gate2(name, parent, reg, shift) \
	to_clk(imx_clk_hw_gate2(name, parent, reg, shift))

#define imx_clk_gate2_flags(name, parent, reg, shift, flags) \
	to_clk(imx_clk_hw_gate2_flags(name, parent, reg, shift, flags))

#define imx_clk_gate2_shared2(name, parent, reg, shift, share_count) \
	to_clk(imx_clk_hw_gate2_shared2(name, parent, reg, shift, share_count))

#define imx_clk_gate3(name, parent, reg, shift) \
	to_clk(imx_clk_hw_gate3(name, parent, reg, shift))

#define imx_clk_gate4(name, parent, reg, shift) \
	to_clk(imx_clk_hw_gate4(name, parent, reg, shift))

#define imx_clk_mux(name, reg, shift, width, parents, num_parents) \
	to_clk(imx_clk_hw_mux(name, reg, shift, width, parents, num_parents))

#define imx_clk_pllv1(type, name, parent, base) \
	to_clk(imx_clk_hw_pllv1(type, name, parent, base))

#define imx_clk_pllv2(name, parent, base) \
	to_clk(imx_clk_hw_pllv2(name, parent, base))

#define imx_clk_frac_pll(name, parent_name, base) \
	to_clk(imx_clk_hw_frac_pll(name, parent_name, base))

#define imx_clk_sscg_pll(name, parent_names, num_parents, parent,\
				bypass1, bypass2, base, flags) \
	to_clk(imx_clk_hw_sscg_pll(name, parent_names, num_parents, parent,\
				bypass1, bypass2, base, flags))

struct clk *imx_clk_pll14xx(const char *name, const char *parent_name,
		 void __iomem *base, const struct imx_pll14xx_clk *pll_clk);

#define imx_clk_pll14xx(name, parent_name, base, pll_clk) \
	to_clk(imx_clk_hw_pll14xx(name, parent_name, base, pll_clk))

struct clk_hw *imx_dev_clk_hw_pll14xx(struct device *dev, const char *name,
				const char *parent_name, void __iomem *base,
				const struct imx_pll14xx_clk *pll_clk);

struct clk_hw *imx_clk_hw_pllv1(enum imx_pllv1_type type, const char *name,
		const char *parent, void __iomem *base);

struct clk_hw *imx_clk_hw_pllv2(const char *name, const char *parent,
		void __iomem *base);

struct clk_hw *imx_clk_hw_frac_pll(const char *name, const char *parent_name,
			     void __iomem *base);

struct clk_hw *imx_clk_hw_sscg_pll(const char *name,
				const char * const *parent_names,
				u8 num_parents,
				u8 parent, u8 bypass1, u8 bypass2,
				void __iomem *base,
				unsigned long flags);

enum imx_pllv3_type {
	IMX_PLLV3_GENERIC,
	IMX_PLLV3_SYS,
	IMX_PLLV3_USB,
	IMX_PLLV3_USB_VF610,
	IMX_PLLV3_AV,
	IMX_PLLV3_ENET,
	IMX_PLLV3_ENET_IMX7,
	IMX_PLLV3_SYS_VF610,
	IMX_PLLV3_DDR_IMX7,
	IMX_PLLV3_AV_IMX7,
};

struct clk_hw *imx_clk_hw_pllv3(enum imx_pllv3_type type, const char *name,
		const char *parent_name, void __iomem *base, u32 div_mask);

#define PLL_1416X_RATE(_rate, _m, _p, _s)		\
	{						\
		.rate	=	(_rate),		\
		.mdiv	=	(_m),			\
		.pdiv	=	(_p),			\
		.sdiv	=	(_s),			\
	}

#define PLL_1443X_RATE(_rate, _m, _p, _s, _k)		\
	{						\
		.rate	=	(_rate),		\
		.mdiv	=	(_m),			\
		.pdiv	=	(_p),			\
		.sdiv	=	(_s),			\
		.kdiv	=	(_k),			\
	}

struct clk_hw *imx_clk_hw_pllv4(const char *name, const char *parent_name,
			     void __iomem *base);

struct clk_hw *clk_hw_register_gate2(struct device *dev, const char *name,
		const char *parent_name, unsigned long flags,
		void __iomem *reg, u8 bit_idx, u8 cgr_val, u8 cgr_mask,
		u8 clk_gate_flags, spinlock_t *lock,
		unsigned int *share_count);

struct clk * imx_obtain_fixed_clock(
			const char *name, unsigned long rate);

struct clk_hw *imx_obtain_fixed_clock_hw(
			const char *name, unsigned long rate);

struct clk_hw *imx_obtain_fixed_clk_hw(struct device_node *np,
				       const char *name);

struct clk_hw *imx_clk_hw_gate_exclusive(const char *name, const char *parent,
	 void __iomem *reg, u8 shift, u32 exclusive_mask);

struct clk_hw *imx_clk_hw_pfd(const char *name, const char *parent_name,
		void __iomem *reg, u8 idx);

struct clk_hw *imx_clk_hw_pfdv2(const char *name, const char *parent_name,
			     void __iomem *reg, u8 idx);

struct clk_hw *imx_clk_hw_busy_divider(const char *name, const char *parent_name,
				 void __iomem *reg, u8 shift, u8 width,
				 void __iomem *busy_reg, u8 busy_shift);

struct clk_hw *imx_clk_hw_busy_mux(const char *name, void __iomem *reg, u8 shift,
			     u8 width, void __iomem *busy_reg, u8 busy_shift,
			     const char * const *parent_names, int num_parents);

struct clk_hw *imx7ulp_clk_hw_composite(const char *name,
				     const char * const *parent_names,
				     int num_parents, bool mux_present,
				     bool rate_present, bool gate_present,
				     void __iomem *reg);

struct clk_hw *imx_clk_hw_fixup_divider(const char *name, const char *parent,
				  void __iomem *reg, u8 shift, u8 width,
				  void (*fixup)(u32 *val));

struct clk_hw *imx_clk_hw_fixup_mux(const char *name, void __iomem *reg,
			      u8 shift, u8 width, const char * const *parents,
			      int num_parents, void (*fixup)(u32 *val));

static inline struct clk *to_clk(struct clk_hw *hw)
{
	if (IS_ERR_OR_NULL(hw))
		return ERR_CAST(hw);
	return hw->clk;
}

static inline struct clk_hw *imx_clk_hw_pll14xx(const char *name, const char *parent_name,
				  void __iomem *base,
				  const struct imx_pll14xx_clk *pll_clk)
{
	return imx_dev_clk_hw_pll14xx(NULL, name, parent_name, base, pll_clk);
}

static inline struct clk_hw *imx_clk_hw_fixed(const char *name, int rate)
{
	return clk_hw_register_fixed_rate(NULL, name, NULL, 0, rate);
}

static inline struct clk_hw *imx_clk_hw_mux_ldb(const char *name, void __iomem *reg,
			u8 shift, u8 width, const char * const *parents,
			int num_parents)
{
	return clk_hw_register_mux(NULL, name, parents, num_parents,
			CLK_SET_RATE_NO_REPARENT | CLK_SET_RATE_PARENT, reg,
			shift, width, CLK_MUX_READ_ONLY, &imx_ccm_lock);
}

static inline struct clk_hw *imx_clk_hw_fixed_factor(const char *name,
		const char *parent, unsigned int mult, unsigned int div)
{
	return clk_hw_register_fixed_factor(NULL, name, parent,
			CLK_SET_RATE_PARENT, mult, div);
}

static inline struct clk_hw *imx_clk_hw_divider(const char *name,
						const char *parent,
						void __iomem *reg, u8 shift,
						u8 width)
{
	return clk_hw_register_divider(NULL, name, parent, CLK_SET_RATE_PARENT,
				       reg, shift, width, 0, &imx_ccm_lock);
}

static inline struct clk_hw *imx_clk_hw_divider_flags(const char *name,
						   const char *parent,
						   void __iomem *reg, u8 shift,
						   u8 width, unsigned long flags)
{
	return clk_hw_register_divider(NULL, name, parent, flags,
				       reg, shift, width, 0, &imx_ccm_lock);
}

static inline struct clk_hw *imx_clk_hw_divider2(const char *name, const char *parent,
		void __iomem *reg, u8 shift, u8 width)
{
	return clk_hw_register_divider(NULL, name, parent,
			CLK_SET_RATE_PARENT | CLK_OPS_PARENT_ENABLE,
			reg, shift, width, 0, &imx_ccm_lock);
}

static inline struct clk *imx_clk_divider2_flags(const char *name,
		const char *parent, void __iomem *reg, u8 shift, u8 width,
		unsigned long flags)
{
	return clk_register_divider(NULL, name, parent,
			flags | CLK_SET_RATE_PARENT | CLK_OPS_PARENT_ENABLE,
			reg, shift, width, 0, &imx_ccm_lock);
}

static inline struct clk_hw *imx_clk_hw_gate_flags(const char *name, const char *parent,
		void __iomem *reg, u8 shift, unsigned long flags)
{
	return clk_hw_register_gate(NULL, name, parent, flags | CLK_SET_RATE_PARENT, reg,
			shift, 0, &imx_ccm_lock);
}

static inline struct clk_hw *imx_clk_hw_gate(const char *name, const char *parent,
					     void __iomem *reg, u8 shift)
{
	return clk_hw_register_gate(NULL, name, parent, CLK_SET_RATE_PARENT, reg,
				    shift, 0, &imx_ccm_lock);
}

static inline struct clk_hw *imx_dev_clk_hw_gate(struct device *dev, const char *name,
						const char *parent, void __iomem *reg, u8 shift)
{
	return clk_hw_register_gate(dev, name, parent, CLK_SET_RATE_PARENT, reg,
				    shift, 0, &imx_ccm_lock);
}

static inline struct clk_hw *imx_clk_hw_gate_dis(const char *name, const char *parent,
		void __iomem *reg, u8 shift)
{
	return clk_hw_register_gate(NULL, name, parent, CLK_SET_RATE_PARENT, reg,
			shift, CLK_GATE_SET_TO_DISABLE, &imx_ccm_lock);
}

static inline struct clk_hw *imx_clk_hw_gate_dis_flags(const char *name, const char *parent,
		void __iomem *reg, u8 shift, unsigned long flags)
{
	return clk_hw_register_gate(NULL, name, parent, flags | CLK_SET_RATE_PARENT, reg,
			shift, CLK_GATE_SET_TO_DISABLE, &imx_ccm_lock);
}

static inline struct clk_hw *imx_clk_hw_gate2(const char *name, const char *parent,
		void __iomem *reg, u8 shift)
{
	return clk_hw_register_gate2(NULL, name, parent, CLK_SET_RATE_PARENT, reg,
			shift, 0x3, 0x3, 0, &imx_ccm_lock, NULL);
}

static inline struct clk_hw *imx_clk_hw_gate2_flags(const char *name, const char *parent,
		void __iomem *reg, u8 shift, unsigned long flags)
{
	return clk_hw_register_gate2(NULL, name, parent, flags | CLK_SET_RATE_PARENT, reg,
			shift, 0x3, 0x3, 0, &imx_ccm_lock, NULL);
}

static inline struct clk_hw *imx_clk_hw_gate2_shared(const char *name,
		const char *parent, void __iomem *reg, u8 shift,
		unsigned int *share_count)
{
	return clk_hw_register_gate2(NULL, name, parent, CLK_SET_RATE_PARENT, reg,
			shift, 0x3, 0x3, 0, &imx_ccm_lock, share_count);
}

static inline struct clk_hw *imx_clk_hw_gate2_shared2(const char *name,
		const char *parent, void __iomem *reg, u8 shift,
		unsigned int *share_count)
{
	return clk_hw_register_gate2(NULL, name, parent, CLK_SET_RATE_PARENT |
				  CLK_OPS_PARENT_ENABLE, reg, shift, 0x3, 0x3, 0,
				  &imx_ccm_lock, share_count);
}

static inline struct clk_hw *imx_dev_clk_hw_gate_shared(struct device *dev,
				const char *name, const char *parent,
				void __iomem *reg, u8 shift,
				unsigned int *share_count)
{
	return clk_hw_register_gate2(NULL, name, parent, CLK_SET_RATE_PARENT |
					CLK_OPS_PARENT_ENABLE, reg, shift, 0x1,
					0x1, 0, &imx_ccm_lock, share_count);
}

static inline struct clk *imx_clk_gate2_cgr(const char *name,
		const char *parent, void __iomem *reg, u8 shift, u8 cgr_val)
{
	return clk_register_gate2(NULL, name, parent, CLK_SET_RATE_PARENT, reg,
			shift, cgr_val, 0x3, 0, &imx_ccm_lock, NULL);
}

static inline struct clk_hw *imx_clk_hw_gate3(const char *name, const char *parent,
		void __iomem *reg, u8 shift)
{
	return clk_hw_register_gate(NULL, name, parent,
			CLK_SET_RATE_PARENT | CLK_OPS_PARENT_ENABLE,
			reg, shift, 0, &imx_ccm_lock);
}

static inline struct clk_hw *imx_clk_hw_gate3_flags(const char *name,
		const char *parent, void __iomem *reg, u8 shift,
		unsigned long flags)
{
	return clk_hw_register_gate(NULL, name, parent,
			flags | CLK_SET_RATE_PARENT | CLK_OPS_PARENT_ENABLE,
			reg, shift, 0, &imx_ccm_lock);
}

#define imx_clk_gate3_flags(name, parent, reg, shift, flags) \
	to_clk(imx_clk_hw_gate3_flags(name, parent, reg, shift, flags))

static inline struct clk_hw *imx_clk_hw_gate4(const char *name, const char *parent,
		void __iomem *reg, u8 shift)
{
	return clk_hw_register_gate2(NULL, name, parent,
			CLK_SET_RATE_PARENT | CLK_OPS_PARENT_ENABLE,
			reg, shift, 0x3, 0x3, 0, &imx_ccm_lock, NULL);
}

static inline struct clk_hw *imx_clk_hw_gate4_flags(const char *name,
		const char *parent, void __iomem *reg, u8 shift,
		unsigned long flags)
{
	return clk_hw_register_gate2(NULL, name, parent,
			flags | CLK_SET_RATE_PARENT | CLK_OPS_PARENT_ENABLE,
			reg, shift, 0x3, 0x3, 0, &imx_ccm_lock, NULL);
}

#define imx_clk_gate4_flags(name, parent, reg, shift, flags) \
	to_clk(imx_clk_hw_gate4_flags(name, parent, reg, shift, flags))

static inline struct clk_hw *imx_clk_hw_mux(const char *name, void __iomem *reg,
			u8 shift, u8 width, const char * const *parents,
			int num_parents)
{
	return clk_hw_register_mux(NULL, name, parents, num_parents,
			CLK_SET_RATE_NO_REPARENT, reg, shift,
			width, 0, &imx_ccm_lock);
}

static inline struct clk_hw *imx_dev_clk_hw_mux(struct device *dev,
			const char *name, void __iomem *reg, u8 shift,
			u8 width, const char * const *parents, int num_parents)
{
	return clk_hw_register_mux(dev, name, parents, num_parents,
			CLK_SET_RATE_NO_REPARENT | CLK_SET_PARENT_GATE,
			reg, shift, width, 0, &imx_ccm_lock);
}

static inline struct clk *imx_clk_mux2(const char *name, void __iomem *reg,
			u8 shift, u8 width, const char * const *parents,
			int num_parents)
{
	return clk_register_mux(NULL, name, parents, num_parents,
			CLK_SET_RATE_NO_REPARENT | CLK_OPS_PARENT_ENABLE,
			reg, shift, width, 0, &imx_ccm_lock);
}

static inline struct clk_hw *imx_clk_hw_mux2(const char *name, void __iomem *reg,
					     u8 shift, u8 width,
					     const char * const *parents,
					     int num_parents)
{
	return clk_hw_register_mux(NULL, name, parents, num_parents,
				   CLK_SET_RATE_NO_REPARENT |
				   CLK_OPS_PARENT_ENABLE,
				   reg, shift, width, 0, &imx_ccm_lock);
}

static inline struct clk *imx_clk_mux_flags(const char *name,
			void __iomem *reg, u8 shift, u8 width,
			const char * const *parents, int num_parents,
			unsigned long flags)
{
	return clk_register_mux(NULL, name, parents, num_parents,
			flags | CLK_SET_RATE_NO_REPARENT, reg, shift, width, 0,
			&imx_ccm_lock);
}

static inline struct clk_hw *imx_clk_hw_mux2_flags(const char *name,
		void __iomem *reg, u8 shift, u8 width,
		const char * const *parents,
		int num_parents, unsigned long flags)
{
	return clk_hw_register_mux(NULL, name, parents, num_parents,
			flags | CLK_SET_RATE_NO_REPARENT | CLK_OPS_PARENT_ENABLE,
			reg, shift, width, 0, &imx_ccm_lock);
}

static inline struct clk *imx_clk_mux2_flags(const char *name,
		void __iomem *reg, u8 shift, u8 width,
		const char * const *parents,
		int num_parents, unsigned long flags)
{
	return clk_register_mux(NULL, name, parents, num_parents,
			flags | CLK_SET_RATE_NO_REPARENT | CLK_OPS_PARENT_ENABLE,
			reg, shift, width, 0, &imx_ccm_lock);
}

static inline struct clk_hw *imx_clk_hw_mux_flags(const char *name,
						  void __iomem *reg, u8 shift,
						  u8 width,
						  const char * const *parents,
						  int num_parents,
						  unsigned long flags)
{
	return clk_hw_register_mux(NULL, name, parents, num_parents,
				   flags | CLK_SET_RATE_NO_REPARENT,
				   reg, shift, width, 0, &imx_ccm_lock);
}

static inline struct clk_hw *imx_dev_clk_hw_mux_flags(struct device *dev,
						  const char *name,
						  void __iomem *reg, u8 shift,
						  u8 width,
						  const char * const *parents,
						  int num_parents,
						  unsigned long flags)
{
	return clk_hw_register_mux(dev, name, parents, num_parents,
				   flags | CLK_SET_RATE_NO_REPARENT,
				   reg, shift, width, 0, &imx_ccm_lock);
}

struct clk_hw *imx_clk_hw_cpu(const char *name, const char *parent_name,
		struct clk *div, struct clk *mux, struct clk *pll,
		struct clk *step);

#define IMX_COMPOSITE_CORE	BIT(0)
#define IMX_COMPOSITE_BUS	BIT(1)

struct clk_hw *imx8m_clk_hw_composite_flags(const char *name,
					    const char * const *parent_names,
					    int num_parents,
					    void __iomem *reg,
					    u32 composite_flags,
					    unsigned long flags);

#define imx8m_clk_hw_composite_bus(name, parent_names, reg)	\
	imx8m_clk_hw_composite_flags(name, parent_names, \
			ARRAY_SIZE(parent_names), reg, \
			IMX_COMPOSITE_BUS, \
			CLK_SET_RATE_NO_REPARENT | CLK_OPS_PARENT_ENABLE)

#define imx8m_clk_hw_composite_bus_critical(name, parent_names, reg)	\
	imx8m_clk_hw_composite_flags(name, parent_names, ARRAY_SIZE(parent_names), reg, \
			IMX_COMPOSITE_BUS, \
			CLK_SET_RATE_NO_REPARENT | CLK_OPS_PARENT_ENABLE | CLK_IS_CRITICAL)

#define imx8m_clk_hw_composite_core(name, parent_names, reg)	\
	imx8m_clk_hw_composite_flags(name, parent_names, \
			ARRAY_SIZE(parent_names), reg, \
			IMX_COMPOSITE_CORE, \
			CLK_SET_RATE_NO_REPARENT | CLK_OPS_PARENT_ENABLE)

#define imx8m_clk_composite_flags(name, parent_names, num_parents, reg, \
				  flags) \
	to_clk(imx8m_clk_hw_composite_flags(name, parent_names, \
				num_parents, reg, 0, flags))

#define __imx8m_clk_hw_composite(name, parent_names, reg, flags) \
	imx8m_clk_hw_composite_flags(name, parent_names, \
		ARRAY_SIZE(parent_names), reg, 0, \
		flags | CLK_SET_RATE_NO_REPARENT | CLK_OPS_PARENT_ENABLE)

#define __imx8m_clk_composite(name, parent_names, reg, flags) \
	to_clk(__imx8m_clk_hw_composite(name, parent_names, reg, flags))

#define imx8m_clk_hw_composite(name, parent_names, reg) \
	__imx8m_clk_hw_composite(name, parent_names, reg, 0)

#define imx8m_clk_composite(name, parent_names, reg) \
	__imx8m_clk_composite(name, parent_names, reg, 0)

#define imx8m_clk_hw_composite_critical(name, parent_names, reg) \
	__imx8m_clk_hw_composite(name, parent_names, reg, CLK_IS_CRITICAL)

#define imx8m_clk_composite_critical(name, parent_names, reg) \
	__imx8m_clk_composite(name, parent_names, reg, CLK_IS_CRITICAL)

struct clk_hw *imx_clk_hw_divider_gate(const char *name, const char *parent_name,
		unsigned long flags, void __iomem *reg, u8 shift, u8 width,
		u8 clk_divider_flags, const struct clk_div_table *table,
		spinlock_t *lock);
#endif
