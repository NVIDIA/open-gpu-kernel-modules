// SPDX-License-Identifier: GPL-2.0
/*
 * Purpose:	PCI Express Port Bus Driver
 * Author:	Tom Nguyen <tom.l.nguyen@intel.com>
 *
 * Copyright (C) 2004 Intel
 * Copyright (C) Tom Long Nguyen (tom.l.nguyen@intel.com)
 */

#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/pm.h>
#include <linux/pm_runtime.h>
#include <linux/init.h>
#include <linux/aer.h>
#include <linux/dmi.h>

#include "../pci.h"
#include "portdrv.h"

/* If this switch is set, PCIe port native services should not be enabled. */
bool pcie_ports_disabled;

/*
 * If the user specified "pcie_ports=native", use the PCIe services regardless
 * of whether the platform has given us permission.  On ACPI systems, this
 * means we ignore _OSC.
 */
bool pcie_ports_native;

/*
 * If the user specified "pcie_ports=dpc-native", use the Linux DPC PCIe
 * service even if the platform hasn't given us permission.
 */
bool pcie_ports_dpc_native;

static int __init pcie_port_setup(char *str)
{
	if (!strncmp(str, "compat", 6))
		pcie_ports_disabled = true;
	else if (!strncmp(str, "native", 6))
		pcie_ports_native = true;
	else if (!strncmp(str, "dpc-native", 10))
		pcie_ports_dpc_native = true;

	return 1;
}
__setup("pcie_ports=", pcie_port_setup);

/* global data */

#ifdef CONFIG_PM
static int pcie_port_runtime_suspend(struct device *dev)
{
	if (!to_pci_dev(dev)->bridge_d3)
		return -EBUSY;

	return pcie_port_device_runtime_suspend(dev);
}

static int pcie_port_runtime_idle(struct device *dev)
{
	/*
	 * Assume the PCI core has set bridge_d3 whenever it thinks the port
	 * should be good to go to D3.  Everything else, including moving
	 * the port to D3, is handled by the PCI core.
	 */
	return to_pci_dev(dev)->bridge_d3 ? 0 : -EBUSY;
}

static const struct dev_pm_ops pcie_portdrv_pm_ops = {
	.suspend	= pcie_port_device_suspend,
	.resume_noirq	= pcie_port_device_resume_noirq,
	.resume		= pcie_port_device_resume,
	.freeze		= pcie_port_device_suspend,
	.thaw		= pcie_port_device_resume,
	.poweroff	= pcie_port_device_suspend,
	.restore_noirq	= pcie_port_device_resume_noirq,
	.restore	= pcie_port_device_resume,
	.runtime_suspend = pcie_port_runtime_suspend,
	.runtime_resume	= pcie_port_device_runtime_resume,
	.runtime_idle	= pcie_port_runtime_idle,
};

#define PCIE_PORTDRV_PM_OPS	(&pcie_portdrv_pm_ops)

#else /* !PM */

#define PCIE_PORTDRV_PM_OPS	NULL
#endif /* !PM */

/*
 * pcie_portdrv_probe - Probe PCI-Express port devices
 * @dev: PCI-Express port device being probed
 *
 * If detected invokes the pcie_port_device_register() method for
 * this port device.
 *
 */
static int pcie_portdrv_probe(struct pci_dev *dev,
					const struct pci_device_id *id)
{
	int type = pci_pcie_type(dev);
	int status;

	if (!pci_is_pcie(dev) ||
	    ((type != PCI_EXP_TYPE_ROOT_PORT) &&
	     (type != PCI_EXP_TYPE_UPSTREAM) &&
	     (type != PCI_EXP_TYPE_DOWNSTREAM) &&
	     (type != PCI_EXP_TYPE_RC_EC)))
		return -ENODEV;

	if (type == PCI_EXP_TYPE_RC_EC)
		pcie_link_rcec(dev);

	status = pcie_port_device_register(dev);
	if (status)
		return status;

	pci_save_state(dev);

	dev_pm_set_driver_flags(&dev->dev, DPM_FLAG_NO_DIRECT_COMPLETE |
					   DPM_FLAG_SMART_SUSPEND);

	if (pci_bridge_d3_possible(dev)) {
		/*
		 * Keep the port resumed 100ms to make sure things like
		 * config space accesses from userspace (lspci) will not
		 * cause the port to repeatedly suspend and resume.
		 */
		pm_runtime_set_autosuspend_delay(&dev->dev, 100);
		pm_runtime_use_autosuspend(&dev->dev);
		pm_runtime_mark_last_busy(&dev->dev);
		pm_runtime_put_autosuspend(&dev->dev);
		pm_runtime_allow(&dev->dev);
	}

	return 0;
}

static void pcie_portdrv_remove(struct pci_dev *dev)
{
	if (pci_bridge_d3_possible(dev)) {
		pm_runtime_forbid(&dev->dev);
		pm_runtime_get_noresume(&dev->dev);
		pm_runtime_dont_use_autosuspend(&dev->dev);
	}

	pcie_port_device_remove(dev);
}

static pci_ers_result_t pcie_portdrv_error_detected(struct pci_dev *dev,
					pci_channel_state_t error)
{
	if (error == pci_channel_io_frozen)
		return PCI_ERS_RESULT_NEED_RESET;
	return PCI_ERS_RESULT_CAN_RECOVER;
}

static pci_ers_result_t pcie_portdrv_slot_reset(struct pci_dev *dev)
{
	pci_restore_state(dev);
	pci_save_state(dev);
	return PCI_ERS_RESULT_RECOVERED;
}

static pci_ers_result_t pcie_portdrv_mmio_enabled(struct pci_dev *dev)
{
	return PCI_ERS_RESULT_RECOVERED;
}

static int resume_iter(struct device *device, void *data)
{
	struct pcie_device *pcie_device;
	struct pcie_port_service_driver *driver;

	if (device->bus == &pcie_port_bus_type && device->driver) {
		driver = to_service_driver(device->driver);
		if (driver && driver->error_resume) {
			pcie_device = to_pcie_device(device);

			/* Forward error message to service drivers */
			driver->error_resume(pcie_device->port);
		}
	}

	return 0;
}

static void pcie_portdrv_err_resume(struct pci_dev *dev)
{
	device_for_each_child(&dev->dev, NULL, resume_iter);
}

/*
 * LINUX Device Driver Model
 */
static const struct pci_device_id port_pci_ids[] = {
	/* handle any PCI-Express port */
	{ PCI_DEVICE_CLASS(((PCI_CLASS_BRIDGE_PCI << 8) | 0x00), ~0) },
	/* subtractive decode PCI-to-PCI bridge, class type is 060401h */
	{ PCI_DEVICE_CLASS(((PCI_CLASS_BRIDGE_PCI << 8) | 0x01), ~0) },
	/* handle any Root Complex Event Collector */
	{ PCI_DEVICE_CLASS(((PCI_CLASS_SYSTEM_RCEC << 8) | 0x00), ~0) },
	{ },
};

static const struct pci_error_handlers pcie_portdrv_err_handler = {
	.error_detected = pcie_portdrv_error_detected,
	.slot_reset = pcie_portdrv_slot_reset,
	.mmio_enabled = pcie_portdrv_mmio_enabled,
	.resume = pcie_portdrv_err_resume,
};

static struct pci_driver pcie_portdriver = {
	.name		= "pcieport",
	.id_table	= &port_pci_ids[0],

	.probe		= pcie_portdrv_probe,
	.remove		= pcie_portdrv_remove,
	.shutdown	= pcie_portdrv_remove,

	.err_handler	= &pcie_portdrv_err_handler,

	.driver.pm	= PCIE_PORTDRV_PM_OPS,
};

static int __init dmi_pcie_pme_disable_msi(const struct dmi_system_id *d)
{
	pr_notice("%s detected: will not use MSI for PCIe PME signaling\n",
		  d->ident);
	pcie_pme_disable_msi();
	return 0;
}

static const struct dmi_system_id pcie_portdrv_dmi_table[] __initconst = {
	/*
	 * Boxes that should not use MSI for PCIe PME signaling.
	 */
	{
	 .callback = dmi_pcie_pme_disable_msi,
	 .ident = "MSI Wind U-100",
	 .matches = {
		     DMI_MATCH(DMI_SYS_VENDOR,
				"MICRO-STAR INTERNATIONAL CO., LTD"),
		     DMI_MATCH(DMI_PRODUCT_NAME, "U-100"),
		     },
	 },
	 {}
};

static void __init pcie_init_services(void)
{
	pcie_aer_init();
	pcie_pme_init();
	pcie_dpc_init();
	pcie_hp_init();
}

static int __init pcie_portdrv_init(void)
{
	if (pcie_ports_disabled)
		return -EACCES;

	pcie_init_services();
	dmi_check_system(pcie_portdrv_dmi_table);

	return pci_register_driver(&pcie_portdriver);
}
device_initcall(pcie_portdrv_init);
