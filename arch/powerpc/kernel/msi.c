// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright 2006-2007, Michael Ellerman, IBM Corporation.
 */

#include <linux/kernel.h>
#include <linux/msi.h>
#include <linux/pci.h>

#include <asm/machdep.h>

int arch_setup_msi_irqs(struct pci_dev *dev, int nvec, int type)
{
	struct pci_controller *phb = pci_bus_to_host(dev->bus);

	if (!phb->controller_ops.setup_msi_irqs ||
	    !phb->controller_ops.teardown_msi_irqs) {
		pr_debug("msi: Platform doesn't provide MSI callbacks.\n");
		return -ENOSYS;
	}

	/* PowerPC doesn't support multiple MSI yet */
	if (type == PCI_CAP_ID_MSI && nvec > 1)
		return 1;

	return phb->controller_ops.setup_msi_irqs(dev, nvec, type);
}

void arch_teardown_msi_irqs(struct pci_dev *dev)
{
	struct pci_controller *phb = pci_bus_to_host(dev->bus);

	/*
	 * We can be called even when arch_setup_msi_irqs() returns -ENOSYS,
	 * so check the pointer again.
	 */
	if (phb->controller_ops.teardown_msi_irqs)
		phb->controller_ops.teardown_msi_irqs(dev);
}
