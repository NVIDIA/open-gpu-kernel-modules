// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright 2011 Freescale Semiconductor, Inc. All Rights Reserved.
 * Copyright 2011 Linaro Ltd.
 */

#include <linux/io.h>
#include <linux/irq.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>
#include <asm/mach/arch.h>
#include <asm/mach/time.h>

#include "common.h"
#include "hardware.h"

static void __init imx51_init_early(void)
{
	mxc_set_cpu_type(MXC_CPU_MX51);
}

/*
 * The MIPI HSC unit has been removed from the i.MX51 Reference Manual by
 * the Freescale marketing division. However this did not remove the
 * hardware from the chip which still needs to be configured for proper
 * IPU support.
 */
#define MX51_MIPI_HSC_BASE 0x83fdc000
static void __init imx51_ipu_mipi_setup(void)
{
	void __iomem *hsc_addr;

	hsc_addr = ioremap(MX51_MIPI_HSC_BASE, SZ_16K);
	WARN_ON(!hsc_addr);

	/* setup MIPI module to legacy mode */
	imx_writel(0xf00, hsc_addr);

	/* CSI mode: reserved; DI control mode: legacy (from Freescale BSP) */
	imx_writel(imx_readl(hsc_addr + 0x800) | 0x30ff, hsc_addr + 0x800);

	iounmap(hsc_addr);
}

static void __init imx51_m4if_setup(void)
{
	void __iomem *m4if_base;
	struct device_node *np;

	np = of_find_compatible_node(NULL, NULL, "fsl,imx51-m4if");
	if (!np)
		return;

	m4if_base = of_iomap(np, 0);
	of_node_put(np);
	if (!m4if_base) {
		pr_err("Unable to map M4IF registers\n");
		return;
	}

	/*
	 * Configure VPU and IPU with higher priorities
	 * in order to avoid artifacts during video playback
	 */
	writel_relaxed(0x00000203, m4if_base + 0x40);
	writel_relaxed(0x00000000, m4if_base + 0x44);
	writel_relaxed(0x00120125, m4if_base + 0x9c);
	writel_relaxed(0x001901A3, m4if_base + 0x48);
	iounmap(m4if_base);
}

static void __init imx51_dt_init(void)
{
	imx51_ipu_mipi_setup();
	imx_src_init();
	imx51_m4if_setup();
	imx5_pmu_init();
	imx_aips_allow_unprivileged_access("fsl,imx51-aipstz");
}

static void __init imx51_init_late(void)
{
	mx51_neon_fixup();
	imx51_pm_init();
}

static const char * const imx51_dt_board_compat[] __initconst = {
	"fsl,imx51",
	NULL
};

DT_MACHINE_START(IMX51_DT, "Freescale i.MX51 (Device Tree Support)")
	.init_early	= imx51_init_early,
	.init_machine	= imx51_dt_init,
	.init_late	= imx51_init_late,
	.dt_compat	= imx51_dt_board_compat,
MACHINE_END
