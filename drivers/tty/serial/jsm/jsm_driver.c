// SPDX-License-Identifier: GPL-2.0+
/************************************************************************
 * Copyright 2003 Digi International (www.digi.com)
 *
 * Copyright (C) 2004 IBM Corporation. All rights reserved.
 *
 * Contact Information:
 * Scott H Kilau <Scott_Kilau@digi.com>
 * Wendy Xiong   <wendyx@us.ibm.com>
 *
 *
 ***********************************************************************/
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/slab.h>

#include "jsm.h"

MODULE_AUTHOR("Digi International, https://www.digi.com");
MODULE_DESCRIPTION("Driver for the Digi International Neo and Classic PCI based product line");
MODULE_LICENSE("GPL");

#define JSM_DRIVER_NAME "jsm"
#define NR_PORTS	32
#define JSM_MINOR_START	0

struct uart_driver jsm_uart_driver = {
	.owner		= THIS_MODULE,
	.driver_name	= JSM_DRIVER_NAME,
	.dev_name	= "ttyn",
	.major		= 0,
	.minor		= JSM_MINOR_START,
	.nr		= NR_PORTS,
};

static pci_ers_result_t jsm_io_error_detected(struct pci_dev *pdev,
					pci_channel_state_t state);
static pci_ers_result_t jsm_io_slot_reset(struct pci_dev *pdev);
static void jsm_io_resume(struct pci_dev *pdev);

static const struct pci_error_handlers jsm_err_handler = {
	.error_detected = jsm_io_error_detected,
	.slot_reset = jsm_io_slot_reset,
	.resume = jsm_io_resume,
};

int jsm_debug;
module_param(jsm_debug, int, 0);
MODULE_PARM_DESC(jsm_debug, "Driver debugging level");

static int jsm_probe_one(struct pci_dev *pdev, const struct pci_device_id *ent)
{
	int rc = 0;
	struct jsm_board *brd;
	static int adapter_count;

	rc = pci_enable_device(pdev);
	if (rc) {
		dev_err(&pdev->dev, "Device enable FAILED\n");
		goto out;
	}

	rc = pci_request_regions(pdev, JSM_DRIVER_NAME);
	if (rc) {
		dev_err(&pdev->dev, "pci_request_region FAILED\n");
		goto out_disable_device;
	}

	brd = kzalloc(sizeof(*brd), GFP_KERNEL);
	if (!brd) {
		rc = -ENOMEM;
		goto out_release_regions;
	}

	/* store the info for the board we've found */
	brd->boardnum = adapter_count++;
	brd->pci_dev = pdev;

	switch (pdev->device) {
	case PCI_DEVICE_ID_NEO_2DB9:
	case PCI_DEVICE_ID_NEO_2DB9PRI:
	case PCI_DEVICE_ID_NEO_2RJ45:
	case PCI_DEVICE_ID_NEO_2RJ45PRI:
	case PCI_DEVICE_ID_NEO_2_422_485:
		brd->maxports = 2;
		break;

	case PCI_DEVICE_ID_CLASSIC_4:
	case PCI_DEVICE_ID_CLASSIC_4_422:
	case PCI_DEVICE_ID_NEO_4:
	case PCIE_DEVICE_ID_NEO_4:
	case PCIE_DEVICE_ID_NEO_4RJ45:
	case PCIE_DEVICE_ID_NEO_4_IBM:
		brd->maxports = 4;
		break;

	case PCI_DEVICE_ID_CLASSIC_8:
	case PCI_DEVICE_ID_CLASSIC_8_422:
	case PCI_DEVICE_ID_DIGI_NEO_8:
	case PCIE_DEVICE_ID_NEO_8:
	case PCIE_DEVICE_ID_NEO_8RJ45:
		brd->maxports = 8;
		break;

	default:
		brd->maxports = 1;
		break;
	}

	spin_lock_init(&brd->bd_intr_lock);

	/* store which revision we have */
	brd->rev = pdev->revision;

	brd->irq = pdev->irq;

	switch (pdev->device) {
	case PCI_DEVICE_ID_CLASSIC_4:
	case PCI_DEVICE_ID_CLASSIC_4_422:
	case PCI_DEVICE_ID_CLASSIC_8:
	case PCI_DEVICE_ID_CLASSIC_8_422:

		jsm_dbg(INIT, &brd->pci_dev,
			"jsm_found_board - Classic adapter\n");

		/*
		 * For PCI ClassicBoards
		 * PCI Local Address (.i.e. "resource" number) space
		 * 0	PLX Memory Mapped Config
		 * 1	PLX I/O Mapped Config
		 * 2	I/O Mapped UARTs and Status
		 * 3	Memory Mapped VPD
		 * 4	Memory Mapped UARTs and Status
		 */

		/* Get the PCI Base Address Registers */
		brd->membase = pci_resource_start(pdev, 4);
		brd->membase_end = pci_resource_end(pdev, 4);

		if (brd->membase & 0x1)
			brd->membase &= ~0x3;
		else
			brd->membase &= ~0xF;

		brd->iobase = pci_resource_start(pdev, 1);
		brd->iobase_end = pci_resource_end(pdev, 1);
		brd->iobase = ((unsigned int)(brd->iobase)) & 0xFFFE;

		/* Assign the board_ops struct */
		brd->bd_ops = &jsm_cls_ops;

		brd->bd_uart_offset = 0x8;
		brd->bd_dividend = 921600;

		brd->re_map_membase = ioremap(brd->membase,
						pci_resource_len(pdev, 4));
		if (!brd->re_map_membase) {
			dev_err(&pdev->dev,
				"Card has no PCI Memory resources, failing board.\n");
			rc = -ENOMEM;
			goto out_kfree_brd;
		}

		/*
		 * Enable Local Interrupt 1			(0x1),
		 * Local Interrupt 1 Polarity Active high	(0x2),
		 * Enable PCI interrupt				(0x43)
		 */
		outb(0x43, brd->iobase + 0x4c);

		break;

	case PCI_DEVICE_ID_NEO_2DB9:
	case PCI_DEVICE_ID_NEO_2DB9PRI:
	case PCI_DEVICE_ID_NEO_2RJ45:
	case PCI_DEVICE_ID_NEO_2RJ45PRI:
	case PCI_DEVICE_ID_NEO_2_422_485:
	case PCI_DEVICE_ID_NEO_4:
	case PCIE_DEVICE_ID_NEO_4:
	case PCIE_DEVICE_ID_NEO_4RJ45:
	case PCIE_DEVICE_ID_NEO_4_IBM:
	case PCI_DEVICE_ID_DIGI_NEO_8:
	case PCIE_DEVICE_ID_NEO_8:
	case PCIE_DEVICE_ID_NEO_8RJ45:

		jsm_dbg(INIT, &brd->pci_dev, "jsm_found_board - NEO adapter\n");

		/* get the PCI Base Address Registers */
		brd->membase	= pci_resource_start(pdev, 0);
		brd->membase_end = pci_resource_end(pdev, 0);

		if (brd->membase & 1)
			brd->membase &= ~0x3;
		else
			brd->membase &= ~0xF;

		/* Assign the board_ops struct */
		brd->bd_ops = &jsm_neo_ops;

		brd->bd_uart_offset = 0x200;
		brd->bd_dividend = 921600;

		brd->re_map_membase = ioremap(brd->membase,
						pci_resource_len(pdev, 0));
		if (!brd->re_map_membase) {
			dev_err(&pdev->dev,
				"Card has no PCI Memory resources, failing board.\n");
			rc = -ENOMEM;
			goto out_kfree_brd;
		}

		break;
	default:
		return -ENXIO;
	}

	rc = request_irq(brd->irq, brd->bd_ops->intr, IRQF_SHARED, "JSM", brd);
	if (rc) {
		dev_warn(&pdev->dev, "Failed to hook IRQ %d\n", brd->irq);
		goto out_iounmap;
	}

	rc = jsm_tty_init(brd);
	if (rc < 0) {
		dev_err(&pdev->dev, "Can't init tty devices (%d)\n", rc);
		rc = -ENXIO;
		goto out_free_irq;
	}

	rc = jsm_uart_port_init(brd);
	if (rc < 0) {
		/* XXX: leaking all resources from jsm_tty_init here! */
		dev_err(&pdev->dev, "Can't init uart port (%d)\n", rc);
		rc = -ENXIO;
		goto out_free_irq;
	}

	/* Log the information about the board */
	dev_info(&pdev->dev, "board %d: Digi Classic/Neo (rev %d), irq %d\n",
			adapter_count, brd->rev, brd->irq);

	pci_set_drvdata(pdev, brd);
	pci_save_state(pdev);

	return 0;
 out_free_irq:
	jsm_remove_uart_port(brd);
	free_irq(brd->irq, brd);
 out_iounmap:
	iounmap(brd->re_map_membase);
 out_kfree_brd:
	kfree(brd);
 out_release_regions:
	pci_release_regions(pdev);
 out_disable_device:
	pci_disable_device(pdev);
 out:
	return rc;
}

static void jsm_remove_one(struct pci_dev *pdev)
{
	struct jsm_board *brd = pci_get_drvdata(pdev);
	int i = 0;

	switch (pdev->device) {
	case PCI_DEVICE_ID_CLASSIC_4:
	case PCI_DEVICE_ID_CLASSIC_4_422:
	case PCI_DEVICE_ID_CLASSIC_8:
	case PCI_DEVICE_ID_CLASSIC_8_422:
		/* Tell card not to interrupt anymore. */
		outb(0x0, brd->iobase + 0x4c);
		break;
	default:
		break;
	}

	jsm_remove_uart_port(brd);

	free_irq(brd->irq, brd);
	iounmap(brd->re_map_membase);

	/* Free all allocated channels structs */
	for (i = 0; i < brd->maxports; i++) {
		if (brd->channels[i]) {
			kfree(brd->channels[i]->ch_rqueue);
			kfree(brd->channels[i]->ch_equeue);
			kfree(brd->channels[i]);
		}
	}

	pci_release_regions(pdev);
	pci_disable_device(pdev);
	kfree(brd);
}

static const struct pci_device_id jsm_pci_tbl[] = {
	{ PCI_DEVICE(PCI_VENDOR_ID_DIGI, PCI_DEVICE_ID_NEO_2DB9), 0, 0, 0 },
	{ PCI_DEVICE(PCI_VENDOR_ID_DIGI, PCI_DEVICE_ID_NEO_2DB9PRI), 0, 0, 1 },
	{ PCI_DEVICE(PCI_VENDOR_ID_DIGI, PCI_DEVICE_ID_NEO_2RJ45), 0, 0, 2 },
	{ PCI_DEVICE(PCI_VENDOR_ID_DIGI, PCI_DEVICE_ID_NEO_2RJ45PRI), 0, 0, 3 },
	{ PCI_DEVICE(PCI_VENDOR_ID_DIGI, PCIE_DEVICE_ID_NEO_4_IBM), 0, 0, 4 },
	{ PCI_DEVICE(PCI_VENDOR_ID_DIGI, PCI_DEVICE_ID_DIGI_NEO_8), 0, 0, 5 },
	{ PCI_DEVICE(PCI_VENDOR_ID_DIGI, PCI_DEVICE_ID_NEO_4), 0, 0, 6 },
	{ PCI_DEVICE(PCI_VENDOR_ID_DIGI, PCI_DEVICE_ID_NEO_1_422), 0, 0, 7 },
	{ PCI_DEVICE(PCI_VENDOR_ID_DIGI, PCI_DEVICE_ID_NEO_1_422_485), 0, 0, 8 },
	{ PCI_DEVICE(PCI_VENDOR_ID_DIGI, PCI_DEVICE_ID_NEO_2_422_485), 0, 0, 9 },
	{ PCI_DEVICE(PCI_VENDOR_ID_DIGI, PCIE_DEVICE_ID_NEO_8), 0, 0, 10 },
	{ PCI_DEVICE(PCI_VENDOR_ID_DIGI, PCIE_DEVICE_ID_NEO_4), 0, 0, 11 },
	{ PCI_DEVICE(PCI_VENDOR_ID_DIGI, PCIE_DEVICE_ID_NEO_4RJ45), 0, 0, 12 },
	{ PCI_DEVICE(PCI_VENDOR_ID_DIGI, PCIE_DEVICE_ID_NEO_8RJ45), 0, 0, 13 },
	{ PCI_DEVICE(PCI_VENDOR_ID_DIGI, PCI_DEVICE_ID_CLASSIC_4), 0, 0, 14 },
	{ PCI_DEVICE(PCI_VENDOR_ID_DIGI, PCI_DEVICE_ID_CLASSIC_4_422), 0, 0, 15 },
	{ PCI_DEVICE(PCI_VENDOR_ID_DIGI, PCI_DEVICE_ID_CLASSIC_8), 0, 0, 16 },
	{ PCI_DEVICE(PCI_VENDOR_ID_DIGI, PCI_DEVICE_ID_CLASSIC_8_422), 0, 0, 17 },
	{ 0, }
};
MODULE_DEVICE_TABLE(pci, jsm_pci_tbl);

static struct pci_driver jsm_driver = {
	.name		= JSM_DRIVER_NAME,
	.id_table	= jsm_pci_tbl,
	.probe		= jsm_probe_one,
	.remove		= jsm_remove_one,
	.err_handler    = &jsm_err_handler,
};

static pci_ers_result_t jsm_io_error_detected(struct pci_dev *pdev,
					pci_channel_state_t state)
{
	struct jsm_board *brd = pci_get_drvdata(pdev);

	jsm_remove_uart_port(brd);

	return PCI_ERS_RESULT_NEED_RESET;
}

static pci_ers_result_t jsm_io_slot_reset(struct pci_dev *pdev)
{
	int rc;

	rc = pci_enable_device(pdev);

	if (rc)
		return PCI_ERS_RESULT_DISCONNECT;

	pci_set_master(pdev);

	return PCI_ERS_RESULT_RECOVERED;
}

static void jsm_io_resume(struct pci_dev *pdev)
{
	struct jsm_board *brd = pci_get_drvdata(pdev);

	pci_restore_state(pdev);
	pci_save_state(pdev);

	jsm_uart_port_init(brd);
}

static int __init jsm_init_module(void)
{
	int rc;

	rc = uart_register_driver(&jsm_uart_driver);
	if (!rc) {
		rc = pci_register_driver(&jsm_driver);
		if (rc)
			uart_unregister_driver(&jsm_uart_driver);
	}
	return rc;
}

static void __exit jsm_exit_module(void)
{
	pci_unregister_driver(&jsm_driver);
	uart_unregister_driver(&jsm_uart_driver);
}

module_init(jsm_init_module);
module_exit(jsm_exit_module);
