// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * IBM ASM Service Processor Device Driver
 *
 * Copyright (C) IBM Corporation, 2004
 *
 * Author: Max Asböck <amax@us.ibm.com>
 *
 * This driver is based on code originally written by Pete Reynolds
 * and others.
 */

/*
 * The ASM device driver does the following things:
 *
 * 1) When loaded it sends a message to the service processor,
 * indicating that an OS is * running. This causes the service processor
 * to send periodic heartbeats to the OS.
 *
 * 2) Answers the periodic heartbeats sent by the service processor.
 * Failure to do so would result in system reboot.
 *
 * 3) Acts as a pass through for dot commands sent from user applications.
 * The interface for this is the ibmasmfs file system.
 *
 * 4) Allows user applications to register for event notification. Events
 * are sent to the driver through interrupts. They can be read from user
 * space through the ibmasmfs file system.
 *
 * 5) Allows user space applications to send heartbeats to the service
 * processor (aka reverse heartbeats). Again this happens through ibmasmfs.
 *
 * 6) Handles remote mouse and keyboard event interrupts and makes them
 * available to user applications through ibmasmfs.
 *
 */

#include <linux/pci.h>
#include <linux/init.h>
#include <linux/slab.h>
#include "ibmasm.h"
#include "lowlevel.h"
#include "remote.h"

int ibmasm_debug = 0;
module_param(ibmasm_debug, int , S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(ibmasm_debug, " Set debug mode on or off");


static int ibmasm_init_one(struct pci_dev *pdev, const struct pci_device_id *id)
{
	int result;
	struct service_processor *sp;

	if ((result = pci_enable_device(pdev))) {
		dev_err(&pdev->dev, "Failed to enable PCI device\n");
		return result;
	}
	if ((result = pci_request_regions(pdev, DRIVER_NAME))) {
		dev_err(&pdev->dev, "Failed to allocate PCI resources\n");
		goto error_resources;
	}
	/* vnc client won't work without bus-mastering */
	pci_set_master(pdev);

	sp = kzalloc(sizeof(struct service_processor), GFP_KERNEL);
	if (sp == NULL) {
		dev_err(&pdev->dev, "Failed to allocate memory\n");
		result = -ENOMEM;
		goto error_kmalloc;
	}

	spin_lock_init(&sp->lock);
	INIT_LIST_HEAD(&sp->command_queue);

	pci_set_drvdata(pdev, (void *)sp);
	sp->dev = &pdev->dev;
	sp->number = pdev->bus->number;
	snprintf(sp->dirname, IBMASM_NAME_SIZE, "%d", sp->number);
	snprintf(sp->devname, IBMASM_NAME_SIZE, "%s%d", DRIVER_NAME, sp->number);

	result = ibmasm_event_buffer_init(sp);
	if (result) {
		dev_err(sp->dev, "Failed to allocate event buffer\n");
		goto error_eventbuffer;
	}

	result = ibmasm_heartbeat_init(sp);
	if (result) {
		dev_err(sp->dev, "Failed to allocate heartbeat command\n");
		goto error_heartbeat;
	}

	sp->irq = pdev->irq;
	sp->base_address = pci_ioremap_bar(pdev, 0);
	if (!sp->base_address) {
		dev_err(sp->dev, "Failed to ioremap pci memory\n");
		result =  -ENODEV;
		goto error_ioremap;
	}

	result = request_irq(sp->irq, ibmasm_interrupt_handler, IRQF_SHARED, sp->devname, (void*)sp);
	if (result) {
		dev_err(sp->dev, "Failed to register interrupt handler\n");
		goto error_request_irq;
	}

	enable_sp_interrupts(sp->base_address);

	result = ibmasm_init_remote_input_dev(sp);
	if (result) {
		dev_err(sp->dev, "Failed to initialize remote queue\n");
		goto error_send_message;
	}

	result = ibmasm_send_driver_vpd(sp);
	if (result) {
		dev_err(sp->dev, "Failed to send driver VPD to service processor\n");
		goto error_send_message;
	}
	result = ibmasm_send_os_state(sp, SYSTEM_STATE_OS_UP);
	if (result) {
		dev_err(sp->dev, "Failed to send OS state to service processor\n");
		goto error_send_message;
	}
	ibmasmfs_add_sp(sp);

	ibmasm_register_uart(sp);

	return 0;

error_send_message:
	disable_sp_interrupts(sp->base_address);
	ibmasm_free_remote_input_dev(sp);
	free_irq(sp->irq, (void *)sp);
error_request_irq:
	iounmap(sp->base_address);
error_ioremap:
	ibmasm_heartbeat_exit(sp);
error_heartbeat:
	ibmasm_event_buffer_exit(sp);
error_eventbuffer:
	kfree(sp);
error_kmalloc:
        pci_release_regions(pdev);
error_resources:
        pci_disable_device(pdev);

	return result;
}

static void ibmasm_remove_one(struct pci_dev *pdev)
{
	struct service_processor *sp = pci_get_drvdata(pdev);

	dbg("Unregistering UART\n");
	ibmasm_unregister_uart(sp);
	dbg("Sending OS down message\n");
	if (ibmasm_send_os_state(sp, SYSTEM_STATE_OS_DOWN))
		err("failed to get response to 'Send OS State' command\n");
	dbg("Disabling heartbeats\n");
	ibmasm_heartbeat_exit(sp);
	dbg("Disabling interrupts\n");
	disable_sp_interrupts(sp->base_address);
	dbg("Freeing SP irq\n");
	free_irq(sp->irq, (void *)sp);
	dbg("Cleaning up\n");
	ibmasm_free_remote_input_dev(sp);
	iounmap(sp->base_address);
	ibmasm_event_buffer_exit(sp);
	kfree(sp);
	pci_release_regions(pdev);
	pci_disable_device(pdev);
}

static struct pci_device_id ibmasm_pci_table[] =
{
	{ PCI_DEVICE(VENDORID_IBM, DEVICEID_RSA) },
	{},
};

static struct pci_driver ibmasm_driver = {
	.name		= DRIVER_NAME,
	.id_table	= ibmasm_pci_table,
	.probe		= ibmasm_init_one,
	.remove		= ibmasm_remove_one,
};

static void __exit ibmasm_exit (void)
{
	ibmasm_unregister_panic_notifier();
	ibmasmfs_unregister();
	pci_unregister_driver(&ibmasm_driver);
	info(DRIVER_DESC " version " DRIVER_VERSION " unloaded");
}

static int __init ibmasm_init(void)
{
	int result = pci_register_driver(&ibmasm_driver);
	if (result)
		return result;

	result = ibmasmfs_register();
	if (result) {
		pci_unregister_driver(&ibmasm_driver);
		err("Failed to register ibmasmfs file system");
		return result;
	}

	ibmasm_register_panic_notifier();
	info(DRIVER_DESC " version " DRIVER_VERSION " loaded");
	return 0;
}

module_init(ibmasm_init);
module_exit(ibmasm_exit);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");
MODULE_DEVICE_TABLE(pci, ibmasm_pci_table);

