// SPDX-License-Identifier: GPL-2.0+
/*
 * CompactPCI Hot Plug Driver
 *
 * Copyright (C) 2002,2005 SOMA Networks, Inc.
 * Copyright (C) 2001 Greg Kroah-Hartman (greg@kroah.com)
 * Copyright (C) 2001 IBM Corp.
 *
 * All rights reserved.
 *
 * Send feedback to <scottm@somanetworks.com>
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched/signal.h>
#include <linux/slab.h>
#include <linux/pci.h>
#include <linux/pci_hotplug.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/atomic.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include "cpci_hotplug.h"

#define DRIVER_AUTHOR	"Scott Murray <scottm@somanetworks.com>"
#define DRIVER_DESC	"CompactPCI Hot Plug Core"

#define MY_NAME	"cpci_hotplug"

#define dbg(format, arg...)					\
	do {							\
		if (cpci_debug)					\
			printk(KERN_DEBUG "%s: " format "\n",	\
				MY_NAME, ## arg);		\
	} while (0)
#define err(format, arg...) printk(KERN_ERR "%s: " format "\n", MY_NAME, ## arg)
#define info(format, arg...) printk(KERN_INFO "%s: " format "\n", MY_NAME, ## arg)
#define warn(format, arg...) printk(KERN_WARNING "%s: " format "\n", MY_NAME, ## arg)

/* local variables */
static DECLARE_RWSEM(list_rwsem);
static LIST_HEAD(slot_list);
static int slots;
static atomic_t extracting;
int cpci_debug;
static struct cpci_hp_controller *controller;
static struct task_struct *cpci_thread;
static int thread_finished;

static int enable_slot(struct hotplug_slot *slot);
static int disable_slot(struct hotplug_slot *slot);
static int set_attention_status(struct hotplug_slot *slot, u8 value);
static int get_power_status(struct hotplug_slot *slot, u8 *value);
static int get_attention_status(struct hotplug_slot *slot, u8 *value);
static int get_adapter_status(struct hotplug_slot *slot, u8 *value);
static int get_latch_status(struct hotplug_slot *slot, u8 *value);

static const struct hotplug_slot_ops cpci_hotplug_slot_ops = {
	.enable_slot = enable_slot,
	.disable_slot = disable_slot,
	.set_attention_status = set_attention_status,
	.get_power_status = get_power_status,
	.get_attention_status = get_attention_status,
	.get_adapter_status = get_adapter_status,
	.get_latch_status = get_latch_status,
};

static int
enable_slot(struct hotplug_slot *hotplug_slot)
{
	struct slot *slot = to_slot(hotplug_slot);
	int retval = 0;

	dbg("%s - physical_slot = %s", __func__, slot_name(slot));

	if (controller->ops->set_power)
		retval = controller->ops->set_power(slot, 1);
	return retval;
}

static int
disable_slot(struct hotplug_slot *hotplug_slot)
{
	struct slot *slot = to_slot(hotplug_slot);
	int retval = 0;

	dbg("%s - physical_slot = %s", __func__, slot_name(slot));

	down_write(&list_rwsem);

	/* Unconfigure device */
	dbg("%s - unconfiguring slot %s", __func__, slot_name(slot));
	retval = cpci_unconfigure_slot(slot);
	if (retval) {
		err("%s - could not unconfigure slot %s",
		    __func__, slot_name(slot));
		goto disable_error;
	}
	dbg("%s - finished unconfiguring slot %s", __func__, slot_name(slot));

	/* Clear EXT (by setting it) */
	if (cpci_clear_ext(slot)) {
		err("%s - could not clear EXT for slot %s",
		    __func__, slot_name(slot));
		retval = -ENODEV;
		goto disable_error;
	}
	cpci_led_on(slot);

	if (controller->ops->set_power) {
		retval = controller->ops->set_power(slot, 0);
		if (retval)
			goto disable_error;
	}

	slot->adapter_status = 0;

	if (slot->extracting) {
		slot->extracting = 0;
		atomic_dec(&extracting);
	}
disable_error:
	up_write(&list_rwsem);
	return retval;
}

static u8
cpci_get_power_status(struct slot *slot)
{
	u8 power = 1;

	if (controller->ops->get_power)
		power = controller->ops->get_power(slot);
	return power;
}

static int
get_power_status(struct hotplug_slot *hotplug_slot, u8 *value)
{
	struct slot *slot = to_slot(hotplug_slot);

	*value = cpci_get_power_status(slot);
	return 0;
}

static int
get_attention_status(struct hotplug_slot *hotplug_slot, u8 *value)
{
	struct slot *slot = to_slot(hotplug_slot);

	*value = cpci_get_attention_status(slot);
	return 0;
}

static int
set_attention_status(struct hotplug_slot *hotplug_slot, u8 status)
{
	return cpci_set_attention_status(to_slot(hotplug_slot), status);
}

static int
get_adapter_status(struct hotplug_slot *hotplug_slot, u8 *value)
{
	struct slot *slot = to_slot(hotplug_slot);

	*value = slot->adapter_status;
	return 0;
}

static int
get_latch_status(struct hotplug_slot *hotplug_slot, u8 *value)
{
	struct slot *slot = to_slot(hotplug_slot);

	*value = slot->latch_status;
	return 0;
}

static void release_slot(struct slot *slot)
{
	pci_dev_put(slot->dev);
	kfree(slot);
}

#define SLOT_NAME_SIZE	6

int
cpci_hp_register_bus(struct pci_bus *bus, u8 first, u8 last)
{
	struct slot *slot;
	char name[SLOT_NAME_SIZE];
	int status;
	int i;

	if (!(controller && bus))
		return -ENODEV;

	/*
	 * Create a structure for each slot, and register that slot
	 * with the pci_hotplug subsystem.
	 */
	for (i = first; i <= last; ++i) {
		slot = kzalloc(sizeof(struct slot), GFP_KERNEL);
		if (!slot) {
			status = -ENOMEM;
			goto error;
		}

		slot->bus = bus;
		slot->number = i;
		slot->devfn = PCI_DEVFN(i, 0);

		snprintf(name, SLOT_NAME_SIZE, "%02x:%02x", bus->number, i);

		slot->hotplug_slot.ops = &cpci_hotplug_slot_ops;

		dbg("registering slot %s", name);
		status = pci_hp_register(&slot->hotplug_slot, bus, i, name);
		if (status) {
			err("pci_hp_register failed with error %d", status);
			goto error_slot;
		}
		dbg("slot registered with name: %s", slot_name(slot));

		/* Add slot to our internal list */
		down_write(&list_rwsem);
		list_add(&slot->slot_list, &slot_list);
		slots++;
		up_write(&list_rwsem);
	}
	return 0;
error_slot:
	kfree(slot);
error:
	return status;
}
EXPORT_SYMBOL_GPL(cpci_hp_register_bus);

int
cpci_hp_unregister_bus(struct pci_bus *bus)
{
	struct slot *slot;
	struct slot *tmp;
	int status = 0;

	down_write(&list_rwsem);
	if (!slots) {
		up_write(&list_rwsem);
		return -1;
	}
	list_for_each_entry_safe(slot, tmp, &slot_list, slot_list) {
		if (slot->bus == bus) {
			list_del(&slot->slot_list);
			slots--;

			dbg("deregistering slot %s", slot_name(slot));
			pci_hp_deregister(&slot->hotplug_slot);
			release_slot(slot);
		}
	}
	up_write(&list_rwsem);
	return status;
}
EXPORT_SYMBOL_GPL(cpci_hp_unregister_bus);

/* This is the interrupt mode interrupt handler */
static irqreturn_t
cpci_hp_intr(int irq, void *data)
{
	dbg("entered cpci_hp_intr");

	/* Check to see if it was our interrupt */
	if ((controller->irq_flags & IRQF_SHARED) &&
	    !controller->ops->check_irq(controller->dev_id)) {
		dbg("exited cpci_hp_intr, not our interrupt");
		return IRQ_NONE;
	}

	/* Disable ENUM interrupt */
	controller->ops->disable_irq();

	/* Trigger processing by the event thread */
	wake_up_process(cpci_thread);
	return IRQ_HANDLED;
}

/*
 * According to PICMG 2.1 R2.0, section 6.3.2, upon
 * initialization, the system driver shall clear the
 * INS bits of the cold-inserted devices.
 */
static int
init_slots(int clear_ins)
{
	struct slot *slot;
	struct pci_dev *dev;

	dbg("%s - enter", __func__);
	down_read(&list_rwsem);
	if (!slots) {
		up_read(&list_rwsem);
		return -1;
	}
	list_for_each_entry(slot, &slot_list, slot_list) {
		dbg("%s - looking at slot %s", __func__, slot_name(slot));
		if (clear_ins && cpci_check_and_clear_ins(slot))
			dbg("%s - cleared INS for slot %s",
			    __func__, slot_name(slot));
		dev = pci_get_slot(slot->bus, PCI_DEVFN(slot->number, 0));
		if (dev) {
			slot->adapter_status = 1;
			slot->latch_status = 1;
			slot->dev = dev;
		}
	}
	up_read(&list_rwsem);
	dbg("%s - exit", __func__);
	return 0;
}

static int
check_slots(void)
{
	struct slot *slot;
	int extracted;
	int inserted;
	u16 hs_csr;

	down_read(&list_rwsem);
	if (!slots) {
		up_read(&list_rwsem);
		err("no slots registered, shutting down");
		return -1;
	}
	extracted = inserted = 0;
	list_for_each_entry(slot, &slot_list, slot_list) {
		dbg("%s - looking at slot %s", __func__, slot_name(slot));
		if (cpci_check_and_clear_ins(slot)) {
			/*
			 * Some broken hardware (e.g. PLX 9054AB) asserts
			 * ENUM# twice...
			 */
			if (slot->dev) {
				warn("slot %s already inserted",
				     slot_name(slot));
				inserted++;
				continue;
			}

			/* Process insertion */
			dbg("%s - slot %s inserted", __func__, slot_name(slot));

			/* GSM, debug */
			hs_csr = cpci_get_hs_csr(slot);
			dbg("%s - slot %s HS_CSR (1) = %04x",
			    __func__, slot_name(slot), hs_csr);

			/* Configure device */
			dbg("%s - configuring slot %s",
			    __func__, slot_name(slot));
			if (cpci_configure_slot(slot)) {
				err("%s - could not configure slot %s",
				    __func__, slot_name(slot));
				continue;
			}
			dbg("%s - finished configuring slot %s",
			    __func__, slot_name(slot));

			/* GSM, debug */
			hs_csr = cpci_get_hs_csr(slot);
			dbg("%s - slot %s HS_CSR (2) = %04x",
			    __func__, slot_name(slot), hs_csr);

			slot->latch_status = 1;
			slot->adapter_status = 1;

			cpci_led_off(slot);

			/* GSM, debug */
			hs_csr = cpci_get_hs_csr(slot);
			dbg("%s - slot %s HS_CSR (3) = %04x",
			    __func__, slot_name(slot), hs_csr);

			inserted++;
		} else if (cpci_check_ext(slot)) {
			/* Process extraction request */
			dbg("%s - slot %s extracted",
			    __func__, slot_name(slot));

			/* GSM, debug */
			hs_csr = cpci_get_hs_csr(slot);
			dbg("%s - slot %s HS_CSR = %04x",
			    __func__, slot_name(slot), hs_csr);

			if (!slot->extracting) {
				slot->latch_status = 0;
				slot->extracting = 1;
				atomic_inc(&extracting);
			}
			extracted++;
		} else if (slot->extracting) {
			hs_csr = cpci_get_hs_csr(slot);
			if (hs_csr == 0xffff) {
				/*
				 * Hmmm, we're likely hosed at this point, should we
				 * bother trying to tell the driver or not?
				 */
				err("card in slot %s was improperly removed",
				    slot_name(slot));
				slot->adapter_status = 0;
				slot->extracting = 0;
				atomic_dec(&extracting);
			}
		}
	}
	up_read(&list_rwsem);
	dbg("inserted=%d, extracted=%d, extracting=%d",
	    inserted, extracted, atomic_read(&extracting));
	if (inserted || extracted)
		return extracted;
	else if (!atomic_read(&extracting)) {
		err("cannot find ENUM# source, shutting down");
		return -1;
	}
	return 0;
}

/* This is the interrupt mode worker thread body */
static int
event_thread(void *data)
{
	int rc;

	dbg("%s - event thread started", __func__);
	while (1) {
		dbg("event thread sleeping");
		set_current_state(TASK_INTERRUPTIBLE);
		schedule();
		if (kthread_should_stop())
			break;
		do {
			rc = check_slots();
			if (rc > 0) {
				/* Give userspace a chance to handle extraction */
				msleep(500);
			} else if (rc < 0) {
				dbg("%s - error checking slots", __func__);
				thread_finished = 1;
				goto out;
			}
		} while (atomic_read(&extracting) && !kthread_should_stop());
		if (kthread_should_stop())
			break;

		/* Re-enable ENUM# interrupt */
		dbg("%s - re-enabling irq", __func__);
		controller->ops->enable_irq();
	}
 out:
	return 0;
}

/* This is the polling mode worker thread body */
static int
poll_thread(void *data)
{
	int rc;

	while (1) {
		if (kthread_should_stop() || signal_pending(current))
			break;
		if (controller->ops->query_enum()) {
			do {
				rc = check_slots();
				if (rc > 0) {
					/* Give userspace a chance to handle extraction */
					msleep(500);
				} else if (rc < 0) {
					dbg("%s - error checking slots", __func__);
					thread_finished = 1;
					goto out;
				}
			} while (atomic_read(&extracting) && !kthread_should_stop());
		}
		msleep(100);
	}
 out:
	return 0;
}

static int
cpci_start_thread(void)
{
	if (controller->irq)
		cpci_thread = kthread_run(event_thread, NULL, "cpci_hp_eventd");
	else
		cpci_thread = kthread_run(poll_thread, NULL, "cpci_hp_polld");
	if (IS_ERR(cpci_thread)) {
		err("Can't start up our thread");
		return PTR_ERR(cpci_thread);
	}
	thread_finished = 0;
	return 0;
}

static void
cpci_stop_thread(void)
{
	kthread_stop(cpci_thread);
	thread_finished = 1;
}

int
cpci_hp_register_controller(struct cpci_hp_controller *new_controller)
{
	int status = 0;

	if (controller)
		return -1;
	if (!(new_controller && new_controller->ops))
		return -EINVAL;
	if (new_controller->irq) {
		if (!(new_controller->ops->enable_irq &&
		     new_controller->ops->disable_irq))
			status = -EINVAL;
		if (request_irq(new_controller->irq,
			       cpci_hp_intr,
			       new_controller->irq_flags,
			       MY_NAME,
			       new_controller->dev_id)) {
			err("Can't get irq %d for the hotplug cPCI controller",
			    new_controller->irq);
			status = -ENODEV;
		}
		dbg("%s - acquired controller irq %d",
		    __func__, new_controller->irq);
	}
	if (!status)
		controller = new_controller;
	return status;
}
EXPORT_SYMBOL_GPL(cpci_hp_register_controller);

static void
cleanup_slots(void)
{
	struct slot *slot;
	struct slot *tmp;

	/*
	 * Unregister all of our slots with the pci_hotplug subsystem,
	 * and free up all memory that we had allocated.
	 */
	down_write(&list_rwsem);
	if (!slots)
		goto cleanup_null;
	list_for_each_entry_safe(slot, tmp, &slot_list, slot_list) {
		list_del(&slot->slot_list);
		pci_hp_deregister(&slot->hotplug_slot);
		release_slot(slot);
	}
cleanup_null:
	up_write(&list_rwsem);
}

int
cpci_hp_unregister_controller(struct cpci_hp_controller *old_controller)
{
	int status = 0;

	if (controller) {
		if (!thread_finished)
			cpci_stop_thread();
		if (controller->irq)
			free_irq(controller->irq, controller->dev_id);
		controller = NULL;
		cleanup_slots();
	} else
		status = -ENODEV;
	return status;
}
EXPORT_SYMBOL_GPL(cpci_hp_unregister_controller);

int
cpci_hp_start(void)
{
	static int first = 1;
	int status;

	dbg("%s - enter", __func__);
	if (!controller)
		return -ENODEV;

	down_read(&list_rwsem);
	if (list_empty(&slot_list)) {
		up_read(&list_rwsem);
		return -ENODEV;
	}
	up_read(&list_rwsem);

	status = init_slots(first);
	if (first)
		first = 0;
	if (status)
		return status;

	status = cpci_start_thread();
	if (status)
		return status;
	dbg("%s - thread started", __func__);

	if (controller->irq) {
		/* Start enum interrupt processing */
		dbg("%s - enabling irq", __func__);
		controller->ops->enable_irq();
	}
	dbg("%s - exit", __func__);
	return 0;
}
EXPORT_SYMBOL_GPL(cpci_hp_start);

int
cpci_hp_stop(void)
{
	if (!controller)
		return -ENODEV;
	if (controller->irq) {
		/* Stop enum interrupt processing */
		dbg("%s - disabling irq", __func__);
		controller->ops->disable_irq();
	}
	cpci_stop_thread();
	return 0;
}
EXPORT_SYMBOL_GPL(cpci_hp_stop);

int __init
cpci_hotplug_init(int debug)
{
	cpci_debug = debug;
	return 0;
}
