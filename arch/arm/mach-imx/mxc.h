/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright 2004-2007, 2010-2015 Freescale Semiconductor, Inc.
 * Copyright (C) 2008 Juergen Beisert (kernel@pengutronix.de)
 */

#ifndef __ASM_ARCH_MXC_H__
#define __ASM_ARCH_MXC_H__

#include <linux/types.h>
#include <soc/imx/cpu.h>

#ifndef __ASM_ARCH_MXC_HARDWARE_H__
#error "Do not include directly."
#endif

#define IMX_DDR_TYPE_LPDDR2		1

#ifndef __ASSEMBLY__

#ifdef CONFIG_SOC_IMX6SL
static inline bool cpu_is_imx6sl(void)
{
	return __mxc_cpu_type == MXC_CPU_IMX6SL;
}
#else
static inline bool cpu_is_imx6sl(void)
{
	return false;
}
#endif

static inline bool cpu_is_imx6dl(void)
{
	return __mxc_cpu_type == MXC_CPU_IMX6DL;
}

static inline bool cpu_is_imx6sx(void)
{
	return __mxc_cpu_type == MXC_CPU_IMX6SX;
}

static inline bool cpu_is_imx6ul(void)
{
	return __mxc_cpu_type == MXC_CPU_IMX6UL;
}

static inline bool cpu_is_imx6ull(void)
{
	return __mxc_cpu_type == MXC_CPU_IMX6ULL;
}

static inline bool cpu_is_imx6ulz(void)
{
	return __mxc_cpu_type == MXC_CPU_IMX6ULZ;
}

static inline bool cpu_is_imx6sll(void)
{
	return __mxc_cpu_type == MXC_CPU_IMX6SLL;
}

static inline bool cpu_is_imx6q(void)
{
	return __mxc_cpu_type == MXC_CPU_IMX6Q;
}

static inline bool cpu_is_imx7d(void)
{
	return __mxc_cpu_type == MXC_CPU_IMX7D;
}

struct cpu_op {
	u32 cpu_rate;
};

int tzic_enable_wake(void);

extern struct cpu_op *(*get_cpu_op)(int *op);
#endif

#define imx_readl	readl_relaxed
#define imx_readw	readw_relaxed
#define imx_writel	writel_relaxed
#define imx_writew	writew_relaxed

#endif /*  __ASM_ARCH_MXC_H__ */
