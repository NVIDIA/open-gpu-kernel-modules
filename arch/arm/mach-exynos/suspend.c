// SPDX-License-Identifier: GPL-2.0
//
// Copyright (c) 2011-2014 Samsung Electronics Co., Ltd.
//		http://www.samsung.com
//
// Exynos - Suspend support
//
// Based on arch/arm/mach-s3c2410/pm.c
// Copyright (c) 2006 Simtec Electronics
//	Ben Dooks <ben@simtec.co.uk>

#include <linux/init.h>
#include <linux/suspend.h>
#include <linux/syscore_ops.h>
#include <linux/cpu_pm.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/irqchip.h>
#include <linux/irqdomain.h>
#include <linux/of_address.h>
#include <linux/err.h>
#include <linux/regulator/machine.h>
#include <linux/soc/samsung/exynos-pmu.h>
#include <linux/soc/samsung/exynos-regs-pmu.h>

#include <asm/cacheflush.h>
#include <asm/hardware/cache-l2x0.h>
#include <asm/firmware.h>
#include <asm/mcpm.h>
#include <asm/smp_scu.h>
#include <asm/suspend.h>

#include "common.h"
#include "smc.h"

#define REG_TABLE_END (-1U)

#define EXYNOS5420_CPU_STATE	0x28

/**
 * struct exynos_wkup_irq - PMU IRQ to mask mapping
 * @hwirq: Hardware IRQ signal of the PMU
 * @mask: Mask in PMU wake-up mask register
 */
struct exynos_wkup_irq {
	unsigned int hwirq;
	u32 mask;
};

struct exynos_pm_data {
	const struct exynos_wkup_irq *wkup_irq;
	unsigned int wake_disable_mask;

	void (*pm_prepare)(void);
	void (*pm_resume_prepare)(void);
	void (*pm_resume)(void);
	int (*pm_suspend)(void);
	int (*cpu_suspend)(unsigned long);
};

/* Used only on Exynos542x/5800 */
struct exynos_pm_state {
	int cpu_state;
	unsigned int pmu_spare3;
	void __iomem *sysram_base;
	phys_addr_t sysram_phys;
	bool secure_firmware;
};

static const struct exynos_pm_data *pm_data __ro_after_init;
static struct exynos_pm_state pm_state;

/*
 * GIC wake-up support
 */

static u32 exynos_irqwake_intmask = 0xffffffff;

static const struct exynos_wkup_irq exynos3250_wkup_irq[] = {
	{ 73, BIT(1) }, /* RTC alarm */
	{ 74, BIT(2) }, /* RTC tick */
	{ /* sentinel */ },
};

static const struct exynos_wkup_irq exynos4_wkup_irq[] = {
	{ 44, BIT(1) }, /* RTC alarm */
	{ 45, BIT(2) }, /* RTC tick */
	{ /* sentinel */ },
};

static const struct exynos_wkup_irq exynos5250_wkup_irq[] = {
	{ 43, BIT(1) }, /* RTC alarm */
	{ 44, BIT(2) }, /* RTC tick */
	{ /* sentinel */ },
};

static u32 exynos_read_eint_wakeup_mask(void)
{
	return pmu_raw_readl(EXYNOS_EINT_WAKEUP_MASK);
}

static int exynos_irq_set_wake(struct irq_data *data, unsigned int state)
{
	const struct exynos_wkup_irq *wkup_irq;

	if (!pm_data->wkup_irq)
		return -ENOENT;
	wkup_irq = pm_data->wkup_irq;

	while (wkup_irq->mask) {
		if (wkup_irq->hwirq == data->hwirq) {
			if (!state)
				exynos_irqwake_intmask |= wkup_irq->mask;
			else
				exynos_irqwake_intmask &= ~wkup_irq->mask;
			return 0;
		}
		++wkup_irq;
	}

	return -ENOENT;
}

static struct irq_chip exynos_pmu_chip = {
	.name			= "PMU",
	.irq_eoi		= irq_chip_eoi_parent,
	.irq_mask		= irq_chip_mask_parent,
	.irq_unmask		= irq_chip_unmask_parent,
	.irq_retrigger		= irq_chip_retrigger_hierarchy,
	.irq_set_wake		= exynos_irq_set_wake,
#ifdef CONFIG_SMP
	.irq_set_affinity	= irq_chip_set_affinity_parent,
#endif
};

static int exynos_pmu_domain_translate(struct irq_domain *d,
				       struct irq_fwspec *fwspec,
				       unsigned long *hwirq,
				       unsigned int *type)
{
	if (is_of_node(fwspec->fwnode)) {
		if (fwspec->param_count != 3)
			return -EINVAL;

		/* No PPI should point to this domain */
		if (fwspec->param[0] != 0)
			return -EINVAL;

		*hwirq = fwspec->param[1];
		*type = fwspec->param[2];
		return 0;
	}

	return -EINVAL;
}

static int exynos_pmu_domain_alloc(struct irq_domain *domain,
				   unsigned int virq,
				   unsigned int nr_irqs, void *data)
{
	struct irq_fwspec *fwspec = data;
	struct irq_fwspec parent_fwspec;
	irq_hw_number_t hwirq;
	int i;

	if (fwspec->param_count != 3)
		return -EINVAL;	/* Not GIC compliant */
	if (fwspec->param[0] != 0)
		return -EINVAL;	/* No PPI should point to this domain */

	hwirq = fwspec->param[1];

	for (i = 0; i < nr_irqs; i++)
		irq_domain_set_hwirq_and_chip(domain, virq + i, hwirq + i,
					      &exynos_pmu_chip, NULL);

	parent_fwspec = *fwspec;
	parent_fwspec.fwnode = domain->parent->fwnode;
	return irq_domain_alloc_irqs_parent(domain, virq, nr_irqs,
					    &parent_fwspec);
}

static const struct irq_domain_ops exynos_pmu_domain_ops = {
	.translate	= exynos_pmu_domain_translate,
	.alloc		= exynos_pmu_domain_alloc,
	.free		= irq_domain_free_irqs_common,
};

static int __init exynos_pmu_irq_init(struct device_node *node,
				      struct device_node *parent)
{
	struct irq_domain *parent_domain, *domain;

	if (!parent) {
		pr_err("%pOF: no parent, giving up\n", node);
		return -ENODEV;
	}

	parent_domain = irq_find_host(parent);
	if (!parent_domain) {
		pr_err("%pOF: unable to obtain parent domain\n", node);
		return -ENXIO;
	}

	pmu_base_addr = of_iomap(node, 0);

	if (!pmu_base_addr) {
		pr_err("%pOF: failed to find exynos pmu register\n", node);
		return -ENOMEM;
	}

	domain = irq_domain_add_hierarchy(parent_domain, 0, 0,
					  node, &exynos_pmu_domain_ops,
					  NULL);
	if (!domain) {
		iounmap(pmu_base_addr);
		pmu_base_addr = NULL;
		return -ENOMEM;
	}

	/*
	 * Clear the OF_POPULATED flag set in of_irq_init so that
	 * later the Exynos PMU platform device won't be skipped.
	 */
	of_node_clear_flag(node, OF_POPULATED);

	return 0;
}

#define EXYNOS_PMU_IRQ(symbol, name)	IRQCHIP_DECLARE(symbol, name, exynos_pmu_irq_init)

EXYNOS_PMU_IRQ(exynos3250_pmu_irq, "samsung,exynos3250-pmu");
EXYNOS_PMU_IRQ(exynos4210_pmu_irq, "samsung,exynos4210-pmu");
EXYNOS_PMU_IRQ(exynos4412_pmu_irq, "samsung,exynos4412-pmu");
EXYNOS_PMU_IRQ(exynos5250_pmu_irq, "samsung,exynos5250-pmu");
EXYNOS_PMU_IRQ(exynos5420_pmu_irq, "samsung,exynos5420-pmu");

static int exynos_cpu_do_idle(void)
{
	/* issue the standby signal into the pm unit. */
	cpu_do_idle();

	pr_info("Failed to suspend the system\n");
	return 1; /* Aborting suspend */
}
static void exynos_flush_cache_all(void)
{
	flush_cache_all();
	outer_flush_all();
}

static int exynos_cpu_suspend(unsigned long arg)
{
	exynos_flush_cache_all();
	return exynos_cpu_do_idle();
}

static int exynos3250_cpu_suspend(unsigned long arg)
{
	flush_cache_all();
	return exynos_cpu_do_idle();
}

static int exynos5420_cpu_suspend(unsigned long arg)
{
	/* MCPM works with HW CPU identifiers */
	unsigned int mpidr = read_cpuid_mpidr();
	unsigned int cluster = MPIDR_AFFINITY_LEVEL(mpidr, 1);
	unsigned int cpu = MPIDR_AFFINITY_LEVEL(mpidr, 0);

	if (IS_ENABLED(CONFIG_EXYNOS_MCPM)) {
		mcpm_set_entry_vector(cpu, cluster, exynos_cpu_resume);
		mcpm_cpu_suspend();
	}

	pr_info("Failed to suspend the system\n");

	/* return value != 0 means failure */
	return 1;
}

static void exynos_pm_set_wakeup_mask(void)
{
	/*
	 * Set wake-up mask registers
	 * EXYNOS_EINT_WAKEUP_MASK is set by pinctrl driver in late suspend.
	 */
	pmu_raw_writel(exynos_irqwake_intmask & ~BIT(31), S5P_WAKEUP_MASK);
}

static void exynos_pm_enter_sleep_mode(void)
{
	/* Set value of power down register for sleep mode */
	exynos_sys_powerdown_conf(SYS_SLEEP);
	pmu_raw_writel(EXYNOS_SLEEP_MAGIC, S5P_INFORM1);
}

static void exynos_pm_prepare(void)
{
	exynos_set_delayed_reset_assertion(false);

	/* Set wake-up mask registers */
	exynos_pm_set_wakeup_mask();

	exynos_pm_enter_sleep_mode();

	/* ensure at least INFORM0 has the resume address */
	pmu_raw_writel(__pa_symbol(exynos_cpu_resume), S5P_INFORM0);
}

static void exynos3250_pm_prepare(void)
{
	unsigned int tmp;

	/* Set wake-up mask registers */
	exynos_pm_set_wakeup_mask();

	tmp = pmu_raw_readl(EXYNOS3_ARM_L2_OPTION);
	tmp &= ~EXYNOS5_OPTION_USE_RETENTION;
	pmu_raw_writel(tmp, EXYNOS3_ARM_L2_OPTION);

	exynos_pm_enter_sleep_mode();

	/* ensure at least INFORM0 has the resume address */
	pmu_raw_writel(__pa_symbol(exynos_cpu_resume), S5P_INFORM0);
}

static void exynos5420_pm_prepare(void)
{
	unsigned int tmp;

	/* Set wake-up mask registers */
	exynos_pm_set_wakeup_mask();

	pm_state.pmu_spare3 = pmu_raw_readl(S5P_PMU_SPARE3);
	/*
	 * The cpu state needs to be saved and restored so that the
	 * secondary CPUs will enter low power start. Though the U-Boot
	 * is setting the cpu state with low power flag, the kernel
	 * needs to restore it back in case, the primary cpu fails to
	 * suspend for any reason.
	 */
	pm_state.cpu_state = readl_relaxed(pm_state.sysram_base +
					   EXYNOS5420_CPU_STATE);
	writel_relaxed(0x0, pm_state.sysram_base + EXYNOS5420_CPU_STATE);
	if (pm_state.secure_firmware)
		exynos_smc(SMC_CMD_REG, SMC_REG_ID_SFR_W(pm_state.sysram_phys +
							 EXYNOS5420_CPU_STATE),
			   0, 0);

	exynos_pm_enter_sleep_mode();

	/* ensure at least INFORM0 has the resume address */
	if (IS_ENABLED(CONFIG_EXYNOS_MCPM))
		pmu_raw_writel(__pa_symbol(mcpm_entry_point), S5P_INFORM0);

	tmp = pmu_raw_readl(EXYNOS_L2_OPTION(0));
	tmp &= ~EXYNOS_L2_USE_RETENTION;
	pmu_raw_writel(tmp, EXYNOS_L2_OPTION(0));

	tmp = pmu_raw_readl(EXYNOS5420_SFR_AXI_CGDIS1);
	tmp |= EXYNOS5420_UFS;
	pmu_raw_writel(tmp, EXYNOS5420_SFR_AXI_CGDIS1);

	tmp = pmu_raw_readl(EXYNOS5420_ARM_COMMON_OPTION);
	tmp &= ~EXYNOS5420_L2RSTDISABLE_VALUE;
	pmu_raw_writel(tmp, EXYNOS5420_ARM_COMMON_OPTION);

	tmp = pmu_raw_readl(EXYNOS5420_FSYS2_OPTION);
	tmp |= EXYNOS5420_EMULATION;
	pmu_raw_writel(tmp, EXYNOS5420_FSYS2_OPTION);

	tmp = pmu_raw_readl(EXYNOS5420_PSGEN_OPTION);
	tmp |= EXYNOS5420_EMULATION;
	pmu_raw_writel(tmp, EXYNOS5420_PSGEN_OPTION);
}


static int exynos_pm_suspend(void)
{
	exynos_pm_central_suspend();

	/* Setting SEQ_OPTION register */
	pmu_raw_writel(S5P_USE_STANDBY_WFI0 | S5P_USE_STANDBY_WFE0,
		       S5P_CENTRAL_SEQ_OPTION);

	if (read_cpuid_part() == ARM_CPU_PART_CORTEX_A9)
		exynos_cpu_save_register();

	return 0;
}

static int exynos5420_pm_suspend(void)
{
	u32 this_cluster;

	exynos_pm_central_suspend();

	/* Setting SEQ_OPTION register */

	this_cluster = MPIDR_AFFINITY_LEVEL(read_cpuid_mpidr(), 1);
	if (!this_cluster)
		pmu_raw_writel(EXYNOS5420_ARM_USE_STANDBY_WFI0,
				S5P_CENTRAL_SEQ_OPTION);
	else
		pmu_raw_writel(EXYNOS5420_KFC_USE_STANDBY_WFI0,
				S5P_CENTRAL_SEQ_OPTION);
	return 0;
}

static void exynos_pm_resume(void)
{
	u32 cpuid = read_cpuid_part();

	if (exynos_pm_central_resume())
		goto early_wakeup;

	if (cpuid == ARM_CPU_PART_CORTEX_A9)
		exynos_scu_enable();

	if (call_firmware_op(resume) == -ENOSYS
	    && cpuid == ARM_CPU_PART_CORTEX_A9)
		exynos_cpu_restore_register();

early_wakeup:

	/* Clear SLEEP mode set in INFORM1 */
	pmu_raw_writel(0x0, S5P_INFORM1);
	exynos_set_delayed_reset_assertion(true);
}

static void exynos3250_pm_resume(void)
{
	u32 cpuid = read_cpuid_part();

	if (exynos_pm_central_resume())
		goto early_wakeup;

	pmu_raw_writel(S5P_USE_STANDBY_WFI_ALL, S5P_CENTRAL_SEQ_OPTION);

	if (call_firmware_op(resume) == -ENOSYS
	    && cpuid == ARM_CPU_PART_CORTEX_A9)
		exynos_cpu_restore_register();

early_wakeup:

	/* Clear SLEEP mode set in INFORM1 */
	pmu_raw_writel(0x0, S5P_INFORM1);
}

static void exynos5420_prepare_pm_resume(void)
{
	unsigned int mpidr, cluster;

	mpidr = read_cpuid_mpidr();
	cluster = MPIDR_AFFINITY_LEVEL(mpidr, 1);

	if (IS_ENABLED(CONFIG_EXYNOS_MCPM))
		WARN_ON(mcpm_cpu_powered_up());

	if (IS_ENABLED(CONFIG_HW_PERF_EVENTS) && cluster != 0) {
		/*
		 * When system is resumed on the LITTLE/KFC core (cluster 1),
		 * the DSCR is not properly updated until the power is turned
		 * on also for the cluster 0. Enable it for a while to
		 * propagate the SPNIDEN and SPIDEN signals from Secure JTAG
		 * block and avoid undefined instruction issue on CP14 reset.
		 */
		pmu_raw_writel(S5P_CORE_LOCAL_PWR_EN,
				EXYNOS_COMMON_CONFIGURATION(0));
		pmu_raw_writel(0,
				EXYNOS_COMMON_CONFIGURATION(0));
	}
}

static void exynos5420_pm_resume(void)
{
	unsigned long tmp;

	/* Restore the CPU0 low power state register */
	tmp = pmu_raw_readl(EXYNOS5_ARM_CORE0_SYS_PWR_REG);
	pmu_raw_writel(tmp | S5P_CORE_LOCAL_PWR_EN,
		       EXYNOS5_ARM_CORE0_SYS_PWR_REG);

	/* Restore the sysram cpu state register */
	writel_relaxed(pm_state.cpu_state,
		       pm_state.sysram_base + EXYNOS5420_CPU_STATE);
	if (pm_state.secure_firmware)
		exynos_smc(SMC_CMD_REG,
			   SMC_REG_ID_SFR_W(pm_state.sysram_phys +
					    EXYNOS5420_CPU_STATE),
			   EXYNOS_AFTR_MAGIC, 0);

	pmu_raw_writel(EXYNOS5420_USE_STANDBY_WFI_ALL,
			S5P_CENTRAL_SEQ_OPTION);

	if (exynos_pm_central_resume())
		goto early_wakeup;

	pmu_raw_writel(pm_state.pmu_spare3, S5P_PMU_SPARE3);

early_wakeup:

	tmp = pmu_raw_readl(EXYNOS5420_SFR_AXI_CGDIS1);
	tmp &= ~EXYNOS5420_UFS;
	pmu_raw_writel(tmp, EXYNOS5420_SFR_AXI_CGDIS1);

	tmp = pmu_raw_readl(EXYNOS5420_FSYS2_OPTION);
	tmp &= ~EXYNOS5420_EMULATION;
	pmu_raw_writel(tmp, EXYNOS5420_FSYS2_OPTION);

	tmp = pmu_raw_readl(EXYNOS5420_PSGEN_OPTION);
	tmp &= ~EXYNOS5420_EMULATION;
	pmu_raw_writel(tmp, EXYNOS5420_PSGEN_OPTION);

	/* Clear SLEEP mode set in INFORM1 */
	pmu_raw_writel(0x0, S5P_INFORM1);
}

/*
 * Suspend Ops
 */

static int exynos_suspend_enter(suspend_state_t state)
{
	u32 eint_wakeup_mask = exynos_read_eint_wakeup_mask();
	int ret;

	pr_debug("%s: suspending the system...\n", __func__);

	pr_debug("%s: wakeup masks: %08x,%08x\n", __func__,
		  exynos_irqwake_intmask, eint_wakeup_mask);

	if (exynos_irqwake_intmask == -1U
	    && eint_wakeup_mask == EXYNOS_EINT_WAKEUP_MASK_DISABLED) {
		pr_err("%s: No wake-up sources!\n", __func__);
		pr_err("%s: Aborting sleep\n", __func__);
		return -EINVAL;
	}

	if (pm_data->pm_prepare)
		pm_data->pm_prepare();
	flush_cache_all();

	ret = call_firmware_op(suspend);
	if (ret == -ENOSYS)
		ret = cpu_suspend(0, pm_data->cpu_suspend);
	if (ret)
		return ret;

	if (pm_data->pm_resume_prepare)
		pm_data->pm_resume_prepare();

	pr_debug("%s: wakeup stat: %08x\n", __func__,
			pmu_raw_readl(S5P_WAKEUP_STAT));

	pr_debug("%s: resuming the system...\n", __func__);

	return 0;
}

static int exynos_suspend_prepare(void)
{
	int ret;

	/*
	 * REVISIT: It would be better if struct platform_suspend_ops
	 * .prepare handler get the suspend_state_t as a parameter to
	 * avoid hard-coding the suspend to mem state. It's safe to do
	 * it now only because the suspend_valid_only_mem function is
	 * used as the .valid callback used to check if a given state
	 * is supported by the platform anyways.
	 */
	ret = regulator_suspend_prepare(PM_SUSPEND_MEM);
	if (ret) {
		pr_err("Failed to prepare regulators for suspend (%d)\n", ret);
		return ret;
	}

	return 0;
}

static void exynos_suspend_finish(void)
{
	int ret;

	ret = regulator_suspend_finish();
	if (ret)
		pr_warn("Failed to resume regulators from suspend (%d)\n", ret);
}

static const struct platform_suspend_ops exynos_suspend_ops = {
	.enter		= exynos_suspend_enter,
	.prepare	= exynos_suspend_prepare,
	.finish		= exynos_suspend_finish,
	.valid		= suspend_valid_only_mem,
};

static const struct exynos_pm_data exynos3250_pm_data = {
	.wkup_irq	= exynos3250_wkup_irq,
	.wake_disable_mask = ((0xFF << 8) | (0x1F << 1)),
	.pm_suspend	= exynos_pm_suspend,
	.pm_resume	= exynos3250_pm_resume,
	.pm_prepare	= exynos3250_pm_prepare,
	.cpu_suspend	= exynos3250_cpu_suspend,
};

static const struct exynos_pm_data exynos4_pm_data = {
	.wkup_irq	= exynos4_wkup_irq,
	.wake_disable_mask = ((0xFF << 8) | (0x1F << 1)),
	.pm_suspend	= exynos_pm_suspend,
	.pm_resume	= exynos_pm_resume,
	.pm_prepare	= exynos_pm_prepare,
	.cpu_suspend	= exynos_cpu_suspend,
};

static const struct exynos_pm_data exynos5250_pm_data = {
	.wkup_irq	= exynos5250_wkup_irq,
	.wake_disable_mask = ((0xFF << 8) | (0x1F << 1)),
	.pm_suspend	= exynos_pm_suspend,
	.pm_resume	= exynos_pm_resume,
	.pm_prepare	= exynos_pm_prepare,
	.cpu_suspend	= exynos_cpu_suspend,
};

static const struct exynos_pm_data exynos5420_pm_data = {
	.wkup_irq	= exynos5250_wkup_irq,
	.wake_disable_mask = (0x7F << 7) | (0x1F << 1),
	.pm_resume_prepare = exynos5420_prepare_pm_resume,
	.pm_resume	= exynos5420_pm_resume,
	.pm_suspend	= exynos5420_pm_suspend,
	.pm_prepare	= exynos5420_pm_prepare,
	.cpu_suspend	= exynos5420_cpu_suspend,
};

static const struct of_device_id exynos_pmu_of_device_ids[] __initconst = {
	{
		.compatible = "samsung,exynos3250-pmu",
		.data = &exynos3250_pm_data,
	}, {
		.compatible = "samsung,exynos4210-pmu",
		.data = &exynos4_pm_data,
	}, {
		.compatible = "samsung,exynos4412-pmu",
		.data = &exynos4_pm_data,
	}, {
		.compatible = "samsung,exynos5250-pmu",
		.data = &exynos5250_pm_data,
	}, {
		.compatible = "samsung,exynos5420-pmu",
		.data = &exynos5420_pm_data,
	},
	{ /*sentinel*/ },
};

static struct syscore_ops exynos_pm_syscore_ops;

void __init exynos_pm_init(void)
{
	const struct of_device_id *match;
	struct device_node *np;
	u32 tmp;

	np = of_find_matching_node_and_match(NULL, exynos_pmu_of_device_ids, &match);
	if (!np) {
		pr_err("Failed to find PMU node\n");
		return;
	}

	if (WARN_ON(!of_find_property(np, "interrupt-controller", NULL))) {
		pr_warn("Outdated DT detected, suspend/resume will NOT work\n");
		of_node_put(np);
		return;
	}
	of_node_put(np);

	pm_data = (const struct exynos_pm_data *) match->data;

	/* All wakeup disable */
	tmp = pmu_raw_readl(S5P_WAKEUP_MASK);
	tmp |= pm_data->wake_disable_mask;
	pmu_raw_writel(tmp, S5P_WAKEUP_MASK);

	exynos_pm_syscore_ops.suspend	= pm_data->pm_suspend;
	exynos_pm_syscore_ops.resume	= pm_data->pm_resume;

	register_syscore_ops(&exynos_pm_syscore_ops);
	suspend_set_ops(&exynos_suspend_ops);

	/*
	 * Applicable as of now only to Exynos542x. If booted under secure
	 * firmware, the non-secure region of sysram should be used.
	 */
	if (exynos_secure_firmware_available()) {
		pm_state.sysram_phys = sysram_base_phys;
		pm_state.sysram_base = sysram_ns_base_addr;
		pm_state.secure_firmware = true;
	} else {
		pm_state.sysram_base = sysram_base_addr;
	}
}
