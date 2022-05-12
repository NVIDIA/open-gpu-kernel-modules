/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2014 NVIDIA CORPORATION.  All rights reserved.
 */

#ifndef MEMORY_TEGRA_MC_H
#define MEMORY_TEGRA_MC_H

#include <linux/bits.h>
#include <linux/io.h>
#include <linux/types.h>

#include <soc/tegra/mc.h>

#define MC_INTSTATUS					0x00
#define MC_INTMASK					0x04
#define MC_ERR_STATUS					0x08
#define MC_ERR_ADR					0x0c
#define MC_GART_ERROR_REQ				0x30
#define MC_EMEM_ADR_CFG					0x54
#define MC_DECERR_EMEM_OTHERS_STATUS			0x58
#define MC_SECURITY_VIOLATION_STATUS			0x74
#define MC_EMEM_ARB_CFG					0x90
#define MC_EMEM_ARB_OUTSTANDING_REQ			0x94
#define MC_EMEM_ARB_TIMING_RCD				0x98
#define MC_EMEM_ARB_TIMING_RP				0x9c
#define MC_EMEM_ARB_TIMING_RC				0xa0
#define MC_EMEM_ARB_TIMING_RAS				0xa4
#define MC_EMEM_ARB_TIMING_FAW				0xa8
#define MC_EMEM_ARB_TIMING_RRD				0xac
#define MC_EMEM_ARB_TIMING_RAP2PRE			0xb0
#define MC_EMEM_ARB_TIMING_WAP2PRE			0xb4
#define MC_EMEM_ARB_TIMING_R2R				0xb8
#define MC_EMEM_ARB_TIMING_W2W				0xbc
#define MC_EMEM_ARB_TIMING_R2W				0xc0
#define MC_EMEM_ARB_TIMING_W2R				0xc4
#define MC_EMEM_ARB_MISC2				0xc8
#define MC_EMEM_ARB_DA_TURNS				0xd0
#define MC_EMEM_ARB_DA_COVERS				0xd4
#define MC_EMEM_ARB_MISC0				0xd8
#define MC_EMEM_ARB_MISC1				0xdc
#define MC_EMEM_ARB_RING1_THROTTLE			0xe0
#define MC_EMEM_ARB_OVERRIDE				0xe8
#define MC_TIMING_CONTROL_DBG				0xf8
#define MC_TIMING_CONTROL				0xfc

#define MC_INT_DECERR_MTS				BIT(16)
#define MC_INT_SECERR_SEC				BIT(13)
#define MC_INT_DECERR_VPR				BIT(12)
#define MC_INT_INVALID_APB_ASID_UPDATE			BIT(11)
#define MC_INT_INVALID_SMMU_PAGE			BIT(10)
#define MC_INT_ARBITRATION_EMEM				BIT(9)
#define MC_INT_SECURITY_VIOLATION			BIT(8)
#define MC_INT_INVALID_GART_PAGE			BIT(7)
#define MC_INT_DECERR_EMEM				BIT(6)

#define MC_ERR_STATUS_TYPE_SHIFT			28
#define MC_ERR_STATUS_TYPE_INVALID_SMMU_PAGE		(0x6 << 28)
#define MC_ERR_STATUS_TYPE_MASK				(0x7 << 28)
#define MC_ERR_STATUS_READABLE				BIT(27)
#define MC_ERR_STATUS_WRITABLE				BIT(26)
#define MC_ERR_STATUS_NONSECURE				BIT(25)
#define MC_ERR_STATUS_ADR_HI_SHIFT			20
#define MC_ERR_STATUS_ADR_HI_MASK			0x3
#define MC_ERR_STATUS_SECURITY				BIT(17)
#define MC_ERR_STATUS_RW				BIT(16)

#define MC_EMEM_ADR_CFG_EMEM_NUMDEV			BIT(0)

#define MC_EMEM_ARB_CFG_CYCLES_PER_UPDATE(x)		((x) & 0x1ff)
#define MC_EMEM_ARB_CFG_CYCLES_PER_UPDATE_MASK		0x1ff

#define MC_EMEM_ARB_OUTSTANDING_REQ_MAX_MASK		0x1ff
#define MC_EMEM_ARB_OUTSTANDING_REQ_HOLDOFF_OVERRIDE	BIT(30)
#define MC_EMEM_ARB_OUTSTANDING_REQ_LIMIT_ENABLE	BIT(31)

#define MC_EMEM_ARB_OVERRIDE_EACK_MASK			0x3

#define MC_TIMING_UPDATE				BIT(0)

static inline u32 tegra_mc_scale_percents(u64 val, unsigned int percents)
{
	val = val * percents;
	do_div(val, 100);

	return min_t(u64, val, U32_MAX);
}

static inline struct tegra_mc *
icc_provider_to_tegra_mc(struct icc_provider *provider)
{
	return container_of(provider, struct tegra_mc, provider);
}

static inline u32 mc_readl(const struct tegra_mc *mc, unsigned long offset)
{
	return readl_relaxed(mc->regs + offset);
}

static inline void mc_writel(const struct tegra_mc *mc, u32 value,
			     unsigned long offset)
{
	writel_relaxed(value, mc->regs + offset);
}

extern const struct tegra_mc_reset_ops tegra_mc_reset_ops_common;

#ifdef CONFIG_ARCH_TEGRA_2x_SOC
extern const struct tegra_mc_soc tegra20_mc_soc;
#endif

#ifdef CONFIG_ARCH_TEGRA_3x_SOC
extern const struct tegra_mc_soc tegra30_mc_soc;
#endif

#ifdef CONFIG_ARCH_TEGRA_114_SOC
extern const struct tegra_mc_soc tegra114_mc_soc;
#endif

#ifdef CONFIG_ARCH_TEGRA_124_SOC
extern const struct tegra_mc_soc tegra124_mc_soc;
#endif

#ifdef CONFIG_ARCH_TEGRA_132_SOC
extern const struct tegra_mc_soc tegra132_mc_soc;
#endif

#ifdef CONFIG_ARCH_TEGRA_210_SOC
extern const struct tegra_mc_soc tegra210_mc_soc;
#endif

/*
 * These IDs are for internal use of Tegra ICC drivers. The ID numbers are
 * chosen such that they don't conflict with the device-tree ICC node IDs.
 */
#define TEGRA_ICC_MC		1000
#define TEGRA_ICC_EMC		1001
#define TEGRA_ICC_EMEM		1002

#endif /* MEMORY_TEGRA_MC_H */
