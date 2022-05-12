// SPDX-License-Identifier: GPL-2.0
/*
 * Intel MID platform PM support
 *
 * Copyright (C) 2016, Intel Corporation
 *
 * Author: Andy Shevchenko <andriy.shevchenko@linux.intel.com>
 */

#include <linux/init.h>
#include <linux/pci.h>

#include <asm/cpu_device_id.h>
#include <asm/intel-family.h>
#include <asm/intel-mid.h>

#include "pci.h"

static bool mid_pci_power_manageable(struct pci_dev *dev)
{
	return true;
}

static int mid_pci_set_power_state(struct pci_dev *pdev, pci_power_t state)
{
	return intel_mid_pci_set_power_state(pdev, state);
}

static pci_power_t mid_pci_get_power_state(struct pci_dev *pdev)
{
	return intel_mid_pci_get_power_state(pdev);
}

static pci_power_t mid_pci_choose_state(struct pci_dev *pdev)
{
	return PCI_D3hot;
}

static int mid_pci_wakeup(struct pci_dev *dev, bool enable)
{
	return 0;
}

static bool mid_pci_need_resume(struct pci_dev *dev)
{
	return false;
}

static const struct pci_platform_pm_ops mid_pci_platform_pm = {
	.is_manageable	= mid_pci_power_manageable,
	.set_state	= mid_pci_set_power_state,
	.get_state	= mid_pci_get_power_state,
	.choose_state	= mid_pci_choose_state,
	.set_wakeup	= mid_pci_wakeup,
	.need_resume	= mid_pci_need_resume,
};

/*
 * This table should be in sync with the one in
 * arch/x86/platform/intel-mid/pwr.c.
 */
static const struct x86_cpu_id lpss_cpu_ids[] = {
	X86_MATCH_INTEL_FAM6_MODEL(ATOM_SALTWELL_MID, NULL),
	X86_MATCH_INTEL_FAM6_MODEL(ATOM_SILVERMONT_MID, NULL),
	{}
};

static int __init mid_pci_init(void)
{
	const struct x86_cpu_id *id;

	id = x86_match_cpu(lpss_cpu_ids);
	if (id)
		pci_set_platform_pm(&mid_pci_platform_pm);
	return 0;
}
arch_initcall(mid_pci_init);
