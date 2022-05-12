// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2016 Freescale Semiconductor, Inc.
 * Copyright 2017~2018 NXP
 *
 * Author: Dong Aisheng <aisheng.dong@nxp.com>
 *
 */

#include <dt-bindings/clock/imx7ulp-clock.h>
#include <linux/clk-provider.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

#include "clk.h"

static const char * const pll_pre_sels[]	= { "sosc", "firc", };
static const char * const spll_pfd_sels[]	= { "spll_pfd0", "spll_pfd1", "spll_pfd2", "spll_pfd3", };
static const char * const spll_sels[]		= { "spll", "spll_pfd_sel", };
static const char * const apll_pfd_sels[]	= { "apll_pfd0", "apll_pfd1", "apll_pfd2", "apll_pfd3", };
static const char * const apll_sels[]		= { "apll", "apll_pfd_sel", };
static const char * const scs_sels[]		= { "dummy", "sosc", "sirc", "firc", "dummy", "apll_sel", "spll_sel", "dummy", };
static const char * const ddr_sels[]		= { "apll_pfd_sel", "dummy", "dummy", "dummy", };
static const char * const nic_sels[]		= { "firc", "ddr_clk", };
static const char * const periph_plat_sels[]	= { "dummy", "nic1_bus_clk", "nic1_clk", "ddr_clk", "apll_pfd2", "apll_pfd1", "apll_pfd0", "upll", };
static const char * const periph_bus_sels[]	= { "dummy", "sosc_bus_clk", "dummy", "firc_bus_clk", "rosc", "nic1_bus_clk", "nic1_clk", "spll_bus_clk", };
static const char * const arm_sels[]		= { "core", "dummy", "dummy", "hsrun_core", };

/* used by sosc/sirc/firc/ddr/spll/apll dividers */
static const struct clk_div_table ulp_div_table[] = {
	{ .val = 1, .div = 1, },
	{ .val = 2, .div = 2, },
	{ .val = 3, .div = 4, },
	{ .val = 4, .div = 8, },
	{ .val = 5, .div = 16, },
	{ .val = 6, .div = 32, },
	{ .val = 7, .div = 64, },
	{ /* sentinel */ },
};

static void __init imx7ulp_clk_scg1_init(struct device_node *np)
{
	struct clk_hw_onecell_data *clk_data;
	struct clk_hw **hws;
	void __iomem *base;

	clk_data = kzalloc(struct_size(clk_data, hws, IMX7ULP_CLK_SCG1_END),
			   GFP_KERNEL);
	if (!clk_data)
		return;

	clk_data->num = IMX7ULP_CLK_SCG1_END;
	hws = clk_data->hws;

	hws[IMX7ULP_CLK_DUMMY]		= imx_clk_hw_fixed("dummy", 0);

	hws[IMX7ULP_CLK_ROSC]		= imx_obtain_fixed_clk_hw(np, "rosc");
	hws[IMX7ULP_CLK_SOSC]		= imx_obtain_fixed_clk_hw(np, "sosc");
	hws[IMX7ULP_CLK_SIRC]		= imx_obtain_fixed_clk_hw(np, "sirc");
	hws[IMX7ULP_CLK_FIRC]		= imx_obtain_fixed_clk_hw(np, "firc");
	hws[IMX7ULP_CLK_UPLL]		= imx_obtain_fixed_clk_hw(np, "upll");

	/* SCG1 */
	base = of_iomap(np, 0);
	WARN_ON(!base);

	/* NOTE: xPLL config can't be changed when xPLL is enabled */
	hws[IMX7ULP_CLK_APLL_PRE_SEL]	= imx_clk_hw_mux_flags("apll_pre_sel", base + 0x508, 0, 1, pll_pre_sels, ARRAY_SIZE(pll_pre_sels), CLK_SET_PARENT_GATE);
	hws[IMX7ULP_CLK_SPLL_PRE_SEL]	= imx_clk_hw_mux_flags("spll_pre_sel", base + 0x608, 0, 1, pll_pre_sels, ARRAY_SIZE(pll_pre_sels), CLK_SET_PARENT_GATE);

	/*							   name		    parent_name	   reg			shift	width	flags */
	hws[IMX7ULP_CLK_APLL_PRE_DIV]	= imx_clk_hw_divider_flags("apll_pre_div", "apll_pre_sel", base + 0x508,	8,	3,	CLK_SET_RATE_GATE);
	hws[IMX7ULP_CLK_SPLL_PRE_DIV]	= imx_clk_hw_divider_flags("spll_pre_div", "spll_pre_sel", base + 0x608,	8,	3,	CLK_SET_RATE_GATE);

	/*						name	 parent_name	 base */
	hws[IMX7ULP_CLK_APLL]		= imx_clk_hw_pllv4("apll",  "apll_pre_div", base + 0x500);
	hws[IMX7ULP_CLK_SPLL]		= imx_clk_hw_pllv4("spll",  "spll_pre_div", base + 0x600);

	/* APLL PFDs */
	hws[IMX7ULP_CLK_APLL_PFD0]	= imx_clk_hw_pfdv2("apll_pfd0", "apll", base + 0x50c, 0);
	hws[IMX7ULP_CLK_APLL_PFD1]	= imx_clk_hw_pfdv2("apll_pfd1", "apll", base + 0x50c, 1);
	hws[IMX7ULP_CLK_APLL_PFD2]	= imx_clk_hw_pfdv2("apll_pfd2", "apll", base + 0x50c, 2);
	hws[IMX7ULP_CLK_APLL_PFD3]	= imx_clk_hw_pfdv2("apll_pfd3", "apll", base + 0x50c, 3);

	/* SPLL PFDs */
	hws[IMX7ULP_CLK_SPLL_PFD0]	= imx_clk_hw_pfdv2("spll_pfd0", "spll", base + 0x60C, 0);
	hws[IMX7ULP_CLK_SPLL_PFD1]	= imx_clk_hw_pfdv2("spll_pfd1", "spll", base + 0x60C, 1);
	hws[IMX7ULP_CLK_SPLL_PFD2]	= imx_clk_hw_pfdv2("spll_pfd2", "spll", base + 0x60C, 2);
	hws[IMX7ULP_CLK_SPLL_PFD3]	= imx_clk_hw_pfdv2("spll_pfd3", "spll", base + 0x60C, 3);

	/* PLL Mux */
	hws[IMX7ULP_CLK_APLL_PFD_SEL]	= imx_clk_hw_mux_flags("apll_pfd_sel", base + 0x508, 14, 2, apll_pfd_sels, ARRAY_SIZE(apll_pfd_sels), CLK_SET_RATE_PARENT | CLK_SET_PARENT_GATE);
	hws[IMX7ULP_CLK_SPLL_PFD_SEL]	= imx_clk_hw_mux_flags("spll_pfd_sel", base + 0x608, 14, 2, spll_pfd_sels, ARRAY_SIZE(spll_pfd_sels), CLK_SET_RATE_PARENT | CLK_SET_PARENT_GATE);
	hws[IMX7ULP_CLK_APLL_SEL]	= imx_clk_hw_mux_flags("apll_sel", base + 0x508, 1, 1, apll_sels, ARRAY_SIZE(apll_sels), CLK_SET_RATE_PARENT | CLK_SET_PARENT_GATE);
	hws[IMX7ULP_CLK_SPLL_SEL]	= imx_clk_hw_mux_flags("spll_sel", base + 0x608, 1, 1, spll_sels, ARRAY_SIZE(spll_sels), CLK_SET_RATE_PARENT | CLK_SET_PARENT_GATE);

	hws[IMX7ULP_CLK_SPLL_BUS_CLK]	= imx_clk_hw_divider_gate("spll_bus_clk", "spll_sel", CLK_SET_RATE_GATE, base + 0x604, 8, 3, 0, ulp_div_table, &imx_ccm_lock);

	/* scs/ddr/nic select different clock source requires that clock to be enabled first */
	hws[IMX7ULP_CLK_SYS_SEL]	= imx_clk_hw_mux2("scs_sel", base + 0x14, 24, 4, scs_sels, ARRAY_SIZE(scs_sels));
	hws[IMX7ULP_CLK_HSRUN_SYS_SEL] = imx_clk_hw_mux2("hsrun_scs_sel", base + 0x1c, 24, 4, scs_sels, ARRAY_SIZE(scs_sels));
	hws[IMX7ULP_CLK_NIC_SEL]	= imx_clk_hw_mux2("nic_sel", base + 0x40, 28, 1, nic_sels, ARRAY_SIZE(nic_sels));
	hws[IMX7ULP_CLK_DDR_SEL]	= imx_clk_hw_mux_flags("ddr_sel", base + 0x30, 24, 2, ddr_sels, ARRAY_SIZE(ddr_sels), CLK_SET_RATE_PARENT | CLK_OPS_PARENT_ENABLE);

	hws[IMX7ULP_CLK_CORE_DIV]	= imx_clk_hw_divider_flags("divcore",	"scs_sel",  base + 0x14, 16, 4, CLK_SET_RATE_PARENT);
	hws[IMX7ULP_CLK_CORE]		= imx_clk_hw_cpu("core", "divcore", hws[IMX7ULP_CLK_CORE_DIV]->clk, hws[IMX7ULP_CLK_SYS_SEL]->clk, hws[IMX7ULP_CLK_SPLL_SEL]->clk, hws[IMX7ULP_CLK_FIRC]->clk);
	hws[IMX7ULP_CLK_HSRUN_CORE_DIV] = imx_clk_hw_divider_flags("hsrun_divcore", "hsrun_scs_sel", base + 0x1c, 16, 4, CLK_SET_RATE_PARENT);
	hws[IMX7ULP_CLK_HSRUN_CORE] = imx_clk_hw_cpu("hsrun_core", "hsrun_divcore", hws[IMX7ULP_CLK_HSRUN_CORE_DIV]->clk, hws[IMX7ULP_CLK_HSRUN_SYS_SEL]->clk, hws[IMX7ULP_CLK_SPLL_SEL]->clk, hws[IMX7ULP_CLK_FIRC]->clk);

	hws[IMX7ULP_CLK_DDR_DIV]	= imx_clk_hw_divider_gate("ddr_clk", "ddr_sel", CLK_SET_RATE_PARENT | CLK_IS_CRITICAL, base + 0x30, 0, 3,
							       0, ulp_div_table, &imx_ccm_lock);

	hws[IMX7ULP_CLK_NIC0_DIV]	= imx_clk_hw_divider_flags("nic0_clk",		"nic_sel",  base + 0x40, 24, 4, CLK_SET_RATE_PARENT | CLK_IS_CRITICAL);
	hws[IMX7ULP_CLK_NIC1_DIV]	= imx_clk_hw_divider_flags("nic1_clk",		"nic0_clk", base + 0x40, 16, 4, CLK_SET_RATE_PARENT | CLK_IS_CRITICAL);
	hws[IMX7ULP_CLK_NIC1_BUS_DIV]	= imx_clk_hw_divider_flags("nic1_bus_clk",	"nic0_clk", base + 0x40, 4,  4, CLK_SET_RATE_PARENT | CLK_IS_CRITICAL);

	hws[IMX7ULP_CLK_GPU_DIV]	= imx_clk_hw_divider("gpu_clk", "nic0_clk", base + 0x40, 20, 4);

	hws[IMX7ULP_CLK_SOSC_BUS_CLK]	= imx_clk_hw_divider_gate("sosc_bus_clk", "sosc", 0, base + 0x104, 8, 3,
							       CLK_DIVIDER_READ_ONLY, ulp_div_table, &imx_ccm_lock);
	hws[IMX7ULP_CLK_FIRC_BUS_CLK]	= imx_clk_hw_divider_gate("firc_bus_clk", "firc", 0, base + 0x304, 8, 3,
							       CLK_DIVIDER_READ_ONLY, ulp_div_table, &imx_ccm_lock);

	imx_check_clk_hws(hws, clk_data->num);

	of_clk_add_hw_provider(np, of_clk_hw_onecell_get, clk_data);
}
CLK_OF_DECLARE(imx7ulp_clk_scg1, "fsl,imx7ulp-scg1", imx7ulp_clk_scg1_init);

static void __init imx7ulp_clk_pcc2_init(struct device_node *np)
{
	struct clk_hw_onecell_data *clk_data;
	struct clk_hw **hws;
	void __iomem *base;

	clk_data = kzalloc(struct_size(clk_data, hws, IMX7ULP_CLK_PCC2_END),
			   GFP_KERNEL);
	if (!clk_data)
		return;

	clk_data->num = IMX7ULP_CLK_PCC2_END;
	hws = clk_data->hws;

	/* PCC2 */
	base = of_iomap(np, 0);
	WARN_ON(!base);

	hws[IMX7ULP_CLK_DMA1]		= imx_clk_hw_gate("dma1", "nic1_clk", base + 0x20, 30);
	hws[IMX7ULP_CLK_RGPIO2P1]	= imx_clk_hw_gate("rgpio2p1", "nic1_bus_clk", base + 0x3c, 30);
	hws[IMX7ULP_CLK_DMA_MUX1]	= imx_clk_hw_gate("dma_mux1", "nic1_bus_clk", base + 0x84, 30);
	hws[IMX7ULP_CLK_CAAM]		= imx_clk_hw_gate("caam", "nic1_clk", base + 0x90, 30);
	hws[IMX7ULP_CLK_LPTPM4]		= imx7ulp_clk_hw_composite("lptpm4",  periph_bus_sels, ARRAY_SIZE(periph_bus_sels), true, false, true, base + 0x94);
	hws[IMX7ULP_CLK_LPTPM5]		= imx7ulp_clk_hw_composite("lptpm5",  periph_bus_sels, ARRAY_SIZE(periph_bus_sels), true, false, true, base + 0x98);
	hws[IMX7ULP_CLK_LPIT1]		= imx7ulp_clk_hw_composite("lpit1",   periph_bus_sels, ARRAY_SIZE(periph_bus_sels), true, false, true, base + 0x9c);
	hws[IMX7ULP_CLK_LPSPI2]		= imx7ulp_clk_hw_composite("lpspi2",  periph_bus_sels, ARRAY_SIZE(periph_bus_sels), true, false, true, base + 0xa4);
	hws[IMX7ULP_CLK_LPSPI3]		= imx7ulp_clk_hw_composite("lpspi3",  periph_bus_sels, ARRAY_SIZE(periph_bus_sels), true, false, true, base + 0xa8);
	hws[IMX7ULP_CLK_LPI2C4]		= imx7ulp_clk_hw_composite("lpi2c4",  periph_bus_sels, ARRAY_SIZE(periph_bus_sels), true, false, true, base + 0xac);
	hws[IMX7ULP_CLK_LPI2C5]		= imx7ulp_clk_hw_composite("lpi2c5",  periph_bus_sels, ARRAY_SIZE(periph_bus_sels), true, false, true, base + 0xb0);
	hws[IMX7ULP_CLK_LPUART4]	= imx7ulp_clk_hw_composite("lpuart4", periph_bus_sels, ARRAY_SIZE(periph_bus_sels), true, false, true, base + 0xb4);
	hws[IMX7ULP_CLK_LPUART5]	= imx7ulp_clk_hw_composite("lpuart5", periph_bus_sels, ARRAY_SIZE(periph_bus_sels), true, false, true, base + 0xb8);
	hws[IMX7ULP_CLK_FLEXIO1]	= imx7ulp_clk_hw_composite("flexio1", periph_bus_sels, ARRAY_SIZE(periph_bus_sels), true, false, true, base + 0xc4);
	hws[IMX7ULP_CLK_USB0]		= imx7ulp_clk_hw_composite("usb0",    periph_plat_sels, ARRAY_SIZE(periph_plat_sels), true, true,  true, base + 0xcc);
	hws[IMX7ULP_CLK_USB1]		= imx7ulp_clk_hw_composite("usb1",    periph_plat_sels, ARRAY_SIZE(periph_plat_sels), true, true,  true, base + 0xd0);
	hws[IMX7ULP_CLK_USB_PHY]	= imx_clk_hw_gate("usb_phy", "nic1_bus_clk", base + 0xd4, 30);
	hws[IMX7ULP_CLK_USDHC0]		= imx7ulp_clk_hw_composite("usdhc0",  periph_plat_sels, ARRAY_SIZE(periph_plat_sels), true, true,  true, base + 0xdc);
	hws[IMX7ULP_CLK_USDHC1]		= imx7ulp_clk_hw_composite("usdhc1",  periph_plat_sels, ARRAY_SIZE(periph_plat_sels), true, true,  true, base + 0xe0);
	hws[IMX7ULP_CLK_WDG1]		= imx7ulp_clk_hw_composite("wdg1",    periph_bus_sels, ARRAY_SIZE(periph_bus_sels), true, true,  true, base + 0xf4);
	hws[IMX7ULP_CLK_WDG2]		= imx7ulp_clk_hw_composite("wdg2",    periph_bus_sels, ARRAY_SIZE(periph_bus_sels), true, true,  true, base + 0x10c);

	imx_check_clk_hws(hws, clk_data->num);

	of_clk_add_hw_provider(np, of_clk_hw_onecell_get, clk_data);

	imx_register_uart_clocks(2);
}
CLK_OF_DECLARE(imx7ulp_clk_pcc2, "fsl,imx7ulp-pcc2", imx7ulp_clk_pcc2_init);

static void __init imx7ulp_clk_pcc3_init(struct device_node *np)
{
	struct clk_hw_onecell_data *clk_data;
	struct clk_hw **hws;
	void __iomem *base;

	clk_data = kzalloc(struct_size(clk_data, hws, IMX7ULP_CLK_PCC3_END),
			   GFP_KERNEL);
	if (!clk_data)
		return;

	clk_data->num = IMX7ULP_CLK_PCC3_END;
	hws = clk_data->hws;

	/* PCC3 */
	base = of_iomap(np, 0);
	WARN_ON(!base);

	hws[IMX7ULP_CLK_LPTPM6]	= imx7ulp_clk_hw_composite("lptpm6",  periph_bus_sels, ARRAY_SIZE(periph_bus_sels), true, false, true, base + 0x84);
	hws[IMX7ULP_CLK_LPTPM7]	= imx7ulp_clk_hw_composite("lptpm7",  periph_bus_sels, ARRAY_SIZE(periph_bus_sels), true, false, true, base + 0x88);

	hws[IMX7ULP_CLK_MMDC]		= clk_hw_register_gate(NULL, "mmdc", "nic1_clk", CLK_SET_RATE_PARENT | CLK_IS_CRITICAL,
							       base + 0xac, 30, 0, &imx_ccm_lock);
	hws[IMX7ULP_CLK_LPI2C6]	= imx7ulp_clk_hw_composite("lpi2c6",  periph_bus_sels, ARRAY_SIZE(periph_bus_sels), true, false, true, base + 0x90);
	hws[IMX7ULP_CLK_LPI2C7]	= imx7ulp_clk_hw_composite("lpi2c7",  periph_bus_sels, ARRAY_SIZE(periph_bus_sels), true, false, true, base + 0x94);
	hws[IMX7ULP_CLK_LPUART6]	= imx7ulp_clk_hw_composite("lpuart6", periph_bus_sels, ARRAY_SIZE(periph_bus_sels), true, false, true, base + 0x98);
	hws[IMX7ULP_CLK_LPUART7]	= imx7ulp_clk_hw_composite("lpuart7", periph_bus_sels, ARRAY_SIZE(periph_bus_sels), true, false, true, base + 0x9c);
	hws[IMX7ULP_CLK_DSI]		= imx7ulp_clk_hw_composite("dsi",     periph_bus_sels, ARRAY_SIZE(periph_bus_sels), true, true,  true, base + 0xa4);
	hws[IMX7ULP_CLK_LCDIF]		= imx7ulp_clk_hw_composite("lcdif",   periph_plat_sels, ARRAY_SIZE(periph_plat_sels), true, true,  true, base + 0xa8);

	hws[IMX7ULP_CLK_VIU]		= imx_clk_hw_gate("viu",   "nic1_clk",	   base + 0xa0, 30);
	hws[IMX7ULP_CLK_PCTLC]		= imx_clk_hw_gate("pctlc", "nic1_bus_clk", base + 0xb8, 30);
	hws[IMX7ULP_CLK_PCTLD]		= imx_clk_hw_gate("pctld", "nic1_bus_clk", base + 0xbc, 30);
	hws[IMX7ULP_CLK_PCTLE]		= imx_clk_hw_gate("pctle", "nic1_bus_clk", base + 0xc0, 30);
	hws[IMX7ULP_CLK_PCTLF]		= imx_clk_hw_gate("pctlf", "nic1_bus_clk", base + 0xc4, 30);

	hws[IMX7ULP_CLK_GPU3D]		= imx7ulp_clk_hw_composite("gpu3d",   periph_plat_sels, ARRAY_SIZE(periph_plat_sels), true, false, true, base + 0x140);
	hws[IMX7ULP_CLK_GPU2D]		= imx7ulp_clk_hw_composite("gpu2d",   periph_plat_sels, ARRAY_SIZE(periph_plat_sels), true, false, true, base + 0x144);

	imx_check_clk_hws(hws, clk_data->num);

	of_clk_add_hw_provider(np, of_clk_hw_onecell_get, clk_data);

	imx_register_uart_clocks(7);
}
CLK_OF_DECLARE(imx7ulp_clk_pcc3, "fsl,imx7ulp-pcc3", imx7ulp_clk_pcc3_init);

static void __init imx7ulp_clk_smc1_init(struct device_node *np)
{
	struct clk_hw_onecell_data *clk_data;
	struct clk_hw **hws;
	void __iomem *base;

	clk_data = kzalloc(struct_size(clk_data, hws, IMX7ULP_CLK_SMC1_END),
			   GFP_KERNEL);
	if (!clk_data)
		return;

	clk_data->num = IMX7ULP_CLK_SMC1_END;
	hws = clk_data->hws;

	/* SMC1 */
	base = of_iomap(np, 0);
	WARN_ON(!base);

	hws[IMX7ULP_CLK_ARM] = imx_clk_hw_mux_flags("arm", base + 0x10, 8, 2, arm_sels, ARRAY_SIZE(arm_sels), CLK_SET_RATE_PARENT);

	imx_check_clk_hws(hws, clk_data->num);

	of_clk_add_hw_provider(np, of_clk_hw_onecell_get, clk_data);
}
CLK_OF_DECLARE(imx7ulp_clk_smc1, "fsl,imx7ulp-smc1", imx7ulp_clk_smc1_init);
