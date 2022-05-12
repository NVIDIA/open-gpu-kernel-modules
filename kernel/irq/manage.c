// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 1992, 1998-2006 Linus Torvalds, Ingo Molnar
 * Copyright (C) 2005-2006 Thomas Gleixner
 *
 * This file contains driver APIs to the irq subsystem.
 */

#define pr_fmt(fmt) "genirq: " fmt

#include <linux/irq.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/random.h>
#include <linux/interrupt.h>
#include <linux/irqdomain.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/sched/rt.h>
#include <linux/sched/task.h>
#include <linux/sched/isolation.h>
#include <uapi/linux/sched/types.h>
#include <linux/task_work.h>

#include "internals.h"

#if defined(CONFIG_IRQ_FORCED_THREADING) && !defined(CONFIG_PREEMPT_RT)
__read_mostly bool force_irqthreads;
EXPORT_SYMBOL_GPL(force_irqthreads);

static int __init setup_forced_irqthreads(char *arg)
{
	force_irqthreads = true;
	return 0;
}
early_param("threadirqs", setup_forced_irqthreads);
#endif

static void __synchronize_hardirq(struct irq_desc *desc, bool sync_chip)
{
	struct irq_data *irqd = irq_desc_get_irq_data(desc);
	bool inprogress;

	do {
		unsigned long flags;

		/*
		 * Wait until we're out of the critical section.  This might
		 * give the wrong answer due to the lack of memory barriers.
		 */
		while (irqd_irq_inprogress(&desc->irq_data))
			cpu_relax();

		/* Ok, that indicated we're done: double-check carefully. */
		raw_spin_lock_irqsave(&desc->lock, flags);
		inprogress = irqd_irq_inprogress(&desc->irq_data);

		/*
		 * If requested and supported, check at the chip whether it
		 * is in flight at the hardware level, i.e. already pending
		 * in a CPU and waiting for service and acknowledge.
		 */
		if (!inprogress && sync_chip) {
			/*
			 * Ignore the return code. inprogress is only updated
			 * when the chip supports it.
			 */
			__irq_get_irqchip_state(irqd, IRQCHIP_STATE_ACTIVE,
						&inprogress);
		}
		raw_spin_unlock_irqrestore(&desc->lock, flags);

		/* Oops, that failed? */
	} while (inprogress);
}

/**
 *	synchronize_hardirq - wait for pending hard IRQ handlers (on other CPUs)
 *	@irq: interrupt number to wait for
 *
 *	This function waits for any pending hard IRQ handlers for this
 *	interrupt to complete before returning. If you use this
 *	function while holding a resource the IRQ handler may need you
 *	will deadlock. It does not take associated threaded handlers
 *	into account.
 *
 *	Do not use this for shutdown scenarios where you must be sure
 *	that all parts (hardirq and threaded handler) have completed.
 *
 *	Returns: false if a threaded handler is active.
 *
 *	This function may be called - with care - from IRQ context.
 *
 *	It does not check whether there is an interrupt in flight at the
 *	hardware level, but not serviced yet, as this might deadlock when
 *	called with interrupts disabled and the target CPU of the interrupt
 *	is the current CPU.
 */
bool synchronize_hardirq(unsigned int irq)
{
	struct irq_desc *desc = irq_to_desc(irq);

	if (desc) {
		__synchronize_hardirq(desc, false);
		return !atomic_read(&desc->threads_active);
	}

	return true;
}
EXPORT_SYMBOL(synchronize_hardirq);

/**
 *	synchronize_irq - wait for pending IRQ handlers (on other CPUs)
 *	@irq: interrupt number to wait for
 *
 *	This function waits for any pending IRQ handlers for this interrupt
 *	to complete before returning. If you use this function while
 *	holding a resource the IRQ handler may need you will deadlock.
 *
 *	Can only be called from preemptible code as it might sleep when
 *	an interrupt thread is associated to @irq.
 *
 *	It optionally makes sure (when the irq chip supports that method)
 *	that the interrupt is not pending in any CPU and waiting for
 *	service.
 */
void synchronize_irq(unsigned int irq)
{
	struct irq_desc *desc = irq_to_desc(irq);

	if (desc) {
		__synchronize_hardirq(desc, true);
		/*
		 * We made sure that no hardirq handler is
		 * running. Now verify that no threaded handlers are
		 * active.
		 */
		wait_event(desc->wait_for_threads,
			   !atomic_read(&desc->threads_active));
	}
}
EXPORT_SYMBOL(synchronize_irq);

#ifdef CONFIG_SMP
cpumask_var_t irq_default_affinity;

static bool __irq_can_set_affinity(struct irq_desc *desc)
{
	if (!desc || !irqd_can_balance(&desc->irq_data) ||
	    !desc->irq_data.chip || !desc->irq_data.chip->irq_set_affinity)
		return false;
	return true;
}

/**
 *	irq_can_set_affinity - Check if the affinity of a given irq can be set
 *	@irq:		Interrupt to check
 *
 */
int irq_can_set_affinity(unsigned int irq)
{
	return __irq_can_set_affinity(irq_to_desc(irq));
}

/**
 * irq_can_set_affinity_usr - Check if affinity of a irq can be set from user space
 * @irq:	Interrupt to check
 *
 * Like irq_can_set_affinity() above, but additionally checks for the
 * AFFINITY_MANAGED flag.
 */
bool irq_can_set_affinity_usr(unsigned int irq)
{
	struct irq_desc *desc = irq_to_desc(irq);

	return __irq_can_set_affinity(desc) &&
		!irqd_affinity_is_managed(&desc->irq_data);
}

/**
 *	irq_set_thread_affinity - Notify irq threads to adjust affinity
 *	@desc:		irq descriptor which has affinity changed
 *
 *	We just set IRQTF_AFFINITY and delegate the affinity setting
 *	to the interrupt thread itself. We can not call
 *	set_cpus_allowed_ptr() here as we hold desc->lock and this
 *	code can be called from hard interrupt context.
 */
void irq_set_thread_affinity(struct irq_desc *desc)
{
	struct irqaction *action;

	for_each_action_of_desc(desc, action)
		if (action->thread)
			set_bit(IRQTF_AFFINITY, &action->thread_flags);
}

#ifdef CONFIG_GENERIC_IRQ_EFFECTIVE_AFF_MASK
static void irq_validate_effective_affinity(struct irq_data *data)
{
	const struct cpumask *m = irq_data_get_effective_affinity_mask(data);
	struct irq_chip *chip = irq_data_get_irq_chip(data);

	if (!cpumask_empty(m))
		return;
	pr_warn_once("irq_chip %s did not update eff. affinity mask of irq %u\n",
		     chip->name, data->irq);
}

static inline void irq_init_effective_affinity(struct irq_data *data,
					       const struct cpumask *mask)
{
	cpumask_copy(irq_data_get_effective_affinity_mask(data), mask);
}
#else
static inline void irq_validate_effective_affinity(struct irq_data *data) { }
static inline void irq_init_effective_affinity(struct irq_data *data,
					       const struct cpumask *mask) { }
#endif

int irq_do_set_affinity(struct irq_data *data, const struct cpumask *mask,
			bool force)
{
	struct irq_desc *desc = irq_data_to_desc(data);
	struct irq_chip *chip = irq_data_get_irq_chip(data);
	int ret;

	if (!chip || !chip->irq_set_affinity)
		return -EINVAL;

	/*
	 * If this is a managed interrupt and housekeeping is enabled on
	 * it check whether the requested affinity mask intersects with
	 * a housekeeping CPU. If so, then remove the isolated CPUs from
	 * the mask and just keep the housekeeping CPU(s). This prevents
	 * the affinity setter from routing the interrupt to an isolated
	 * CPU to avoid that I/O submitted from a housekeeping CPU causes
	 * interrupts on an isolated one.
	 *
	 * If the masks do not intersect or include online CPU(s) then
	 * keep the requested mask. The isolated target CPUs are only
	 * receiving interrupts when the I/O operation was submitted
	 * directly from them.
	 *
	 * If all housekeeping CPUs in the affinity mask are offline, the
	 * interrupt will be migrated by the CPU hotplug code once a
	 * housekeeping CPU which belongs to the affinity mask comes
	 * online.
	 */
	if (irqd_affinity_is_managed(data) &&
	    housekeeping_enabled(HK_FLAG_MANAGED_IRQ)) {
		const struct cpumask *hk_mask, *prog_mask;

		static DEFINE_RAW_SPINLOCK(tmp_mask_lock);
		static struct cpumask tmp_mask;

		hk_mask = housekeeping_cpumask(HK_FLAG_MANAGED_IRQ);

		raw_spin_lock(&tmp_mask_lock);
		cpumask_and(&tmp_mask, mask, hk_mask);
		if (!cpumask_intersects(&tmp_mask, cpu_online_mask))
			prog_mask = mask;
		else
			prog_mask = &tmp_mask;
		ret = chip->irq_set_affinity(data, prog_mask, force);
		raw_spin_unlock(&tmp_mask_lock);
	} else {
		ret = chip->irq_set_affinity(data, mask, force);
	}
	switch (ret) {
	case IRQ_SET_MASK_OK:
	case IRQ_SET_MASK_OK_DONE:
		cpumask_copy(desc->irq_common_data.affinity, mask);
		fallthrough;
	case IRQ_SET_MASK_OK_NOCOPY:
		irq_validate_effective_affinity(data);
		irq_set_thread_affinity(desc);
		ret = 0;
	}

	return ret;
}

#ifdef CONFIG_GENERIC_PENDING_IRQ
static inline int irq_set_affinity_pending(struct irq_data *data,
					   const struct cpumask *dest)
{
	struct irq_desc *desc = irq_data_to_desc(data);

	irqd_set_move_pending(data);
	irq_copy_pending(desc, dest);
	return 0;
}
#else
static inline int irq_set_affinity_pending(struct irq_data *data,
					   const struct cpumask *dest)
{
	return -EBUSY;
}
#endif

static int irq_try_set_affinity(struct irq_data *data,
				const struct cpumask *dest, bool force)
{
	int ret = irq_do_set_affinity(data, dest, force);

	/*
	 * In case that the underlying vector management is busy and the
	 * architecture supports the generic pending mechanism then utilize
	 * this to avoid returning an error to user space.
	 */
	if (ret == -EBUSY && !force)
		ret = irq_set_affinity_pending(data, dest);
	return ret;
}

static bool irq_set_affinity_deactivated(struct irq_data *data,
					 const struct cpumask *mask, bool force)
{
	struct irq_desc *desc = irq_data_to_desc(data);

	/*
	 * Handle irq chips which can handle affinity only in activated
	 * state correctly
	 *
	 * If the interrupt is not yet activated, just store the affinity
	 * mask and do not call the chip driver at all. On activation the
	 * driver has to make sure anyway that the interrupt is in a
	 * usable state so startup works.
	 */
	if (!IS_ENABLED(CONFIG_IRQ_DOMAIN_HIERARCHY) ||
	    irqd_is_activated(data) || !irqd_affinity_on_activate(data))
		return false;

	cpumask_copy(desc->irq_common_data.affinity, mask);
	irq_init_effective_affinity(data, mask);
	irqd_set(data, IRQD_AFFINITY_SET);
	return true;
}

int irq_set_affinity_locked(struct irq_data *data, const struct cpumask *mask,
			    bool force)
{
	struct irq_chip *chip = irq_data_get_irq_chip(data);
	struct irq_desc *desc = irq_data_to_desc(data);
	int ret = 0;

	if (!chip || !chip->irq_set_affinity)
		return -EINVAL;

	if (irq_set_affinity_deactivated(data, mask, force))
		return 0;

	if (irq_can_move_pcntxt(data) && !irqd_is_setaffinity_pending(data)) {
		ret = irq_try_set_affinity(data, mask, force);
	} else {
		irqd_set_move_pending(data);
		irq_copy_pending(desc, mask);
	}

	if (desc->affinity_notify) {
		kref_get(&desc->affinity_notify->kref);
		if (!schedule_work(&desc->affinity_notify->work)) {
			/* Work was already scheduled, drop our extra ref */
			kref_put(&desc->affinity_notify->kref,
				 desc->affinity_notify->release);
		}
	}
	irqd_set(data, IRQD_AFFINITY_SET);

	return ret;
}

/**
 * irq_update_affinity_desc - Update affinity management for an interrupt
 * @irq:	The interrupt number to update
 * @affinity:	Pointer to the affinity descriptor
 *
 * This interface can be used to configure the affinity management of
 * interrupts which have been allocated already.
 *
 * There are certain limitations on when it may be used - attempts to use it
 * for when the kernel is configured for generic IRQ reservation mode (in
 * config GENERIC_IRQ_RESERVATION_MODE) will fail, as it may conflict with
 * managed/non-managed interrupt accounting. In addition, attempts to use it on
 * an interrupt which is already started or which has already been configured
 * as managed will also fail, as these mean invalid init state or double init.
 */
int irq_update_affinity_desc(unsigned int irq,
			     struct irq_affinity_desc *affinity)
{
	struct irq_desc *desc;
	unsigned long flags;
	bool activated;
	int ret = 0;

	/*
	 * Supporting this with the reservation scheme used by x86 needs
	 * some more thought. Fail it for now.
	 */
	if (IS_ENABLED(CONFIG_GENERIC_IRQ_RESERVATION_MODE))
		return -EOPNOTSUPP;

	desc = irq_get_desc_buslock(irq, &flags, 0);
	if (!desc)
		return -EINVAL;

	/* Requires the interrupt to be shut down */
	if (irqd_is_started(&desc->irq_data)) {
		ret = -EBUSY;
		goto out_unlock;
	}

	/* Interrupts which are already managed cannot be modified */
	if (irqd_affinity_is_managed(&desc->irq_data)) {
		ret = -EBUSY;
		goto out_unlock;
	}

	/*
	 * Deactivate the interrupt. That's required to undo
	 * anything an earlier activation has established.
	 */
	activated = irqd_is_activated(&desc->irq_data);
	if (activated)
		irq_domain_deactivate_irq(&desc->irq_data);

	if (affinity->is_managed) {
		irqd_set(&desc->irq_data, IRQD_AFFINITY_MANAGED);
		irqd_set(&desc->irq_data, IRQD_MANAGED_SHUTDOWN);
	}

	cpumask_copy(desc->irq_common_data.affinity, &affinity->mask);

	/* Restore the activation state */
	if (activated)
		irq_domain_activate_irq(&desc->irq_data, false);

out_unlock:
	irq_put_desc_busunlock(desc, flags);
	return ret;
}

int __irq_set_affinity(unsigned int irq, const struct cpumask *mask, bool force)
{
	struct irq_desc *desc = irq_to_desc(irq);
	unsigned long flags;
	int ret;

	if (!desc)
		return -EINVAL;

	raw_spin_lock_irqsave(&desc->lock, flags);
	ret = irq_set_affinity_locked(irq_desc_get_irq_data(desc), mask, force);
	raw_spin_unlock_irqrestore(&desc->lock, flags);
	return ret;
}

int irq_set_affinity_hint(unsigned int irq, const struct cpumask *m)
{
	unsigned long flags;
	struct irq_desc *desc = irq_get_desc_lock(irq, &flags, IRQ_GET_DESC_CHECK_GLOBAL);

	if (!desc)
		return -EINVAL;
	desc->affinity_hint = m;
	irq_put_desc_unlock(desc, flags);
	/* set the initial affinity to prevent every interrupt being on CPU0 */
	if (m)
		__irq_set_affinity(irq, m, false);
	return 0;
}
EXPORT_SYMBOL_GPL(irq_set_affinity_hint);

static void irq_affinity_notify(struct work_struct *work)
{
	struct irq_affinity_notify *notify =
		container_of(work, struct irq_affinity_notify, work);
	struct irq_desc *desc = irq_to_desc(notify->irq);
	cpumask_var_t cpumask;
	unsigned long flags;

	if (!desc || !alloc_cpumask_var(&cpumask, GFP_KERNEL))
		goto out;

	raw_spin_lock_irqsave(&desc->lock, flags);
	if (irq_move_pending(&desc->irq_data))
		irq_get_pending(cpumask, desc);
	else
		cpumask_copy(cpumask, desc->irq_common_data.affinity);
	raw_spin_unlock_irqrestore(&desc->lock, flags);

	notify->notify(notify, cpumask);

	free_cpumask_var(cpumask);
out:
	kref_put(&notify->kref, notify->release);
}

/**
 *	irq_set_affinity_notifier - control notification of IRQ affinity changes
 *	@irq:		Interrupt for which to enable/disable notification
 *	@notify:	Context for notification, or %NULL to disable
 *			notification.  Function pointers must be initialised;
 *			the other fields will be initialised by this function.
 *
 *	Must be called in process context.  Notification may only be enabled
 *	after the IRQ is allocated and must be disabled before the IRQ is
 *	freed using free_irq().
 */
int
irq_set_affinity_notifier(unsigned int irq, struct irq_affinity_notify *notify)
{
	struct irq_desc *desc = irq_to_desc(irq);
	struct irq_affinity_notify *old_notify;
	unsigned long flags;

	/* The release function is promised process context */
	might_sleep();

	if (!desc || desc->istate & IRQS_NMI)
		return -EINVAL;

	/* Complete initialisation of *notify */
	if (notify) {
		notify->irq = irq;
		kref_init(&notify->kref);
		INIT_WORK(&notify->work, irq_affinity_notify);
	}

	raw_spin_lock_irqsave(&desc->lock, flags);
	old_notify = desc->affinity_notify;
	desc->affinity_notify = notify;
	raw_spin_unlock_irqrestore(&desc->lock, flags);

	if (old_notify) {
		if (cancel_work_sync(&old_notify->work)) {
			/* Pending work had a ref, put that one too */
			kref_put(&old_notify->kref, old_notify->release);
		}
		kref_put(&old_notify->kref, old_notify->release);
	}

	return 0;
}
EXPORT_SYMBOL_GPL(irq_set_affinity_notifier);

#ifndef CONFIG_AUTO_IRQ_AFFINITY
/*
 * Generic version of the affinity autoselector.
 */
int irq_setup_affinity(struct irq_desc *desc)
{
	struct cpumask *set = irq_default_affinity;
	int ret, node = irq_desc_get_node(desc);
	static DEFINE_RAW_SPINLOCK(mask_lock);
	static struct cpumask mask;

	/* Excludes PER_CPU and NO_BALANCE interrupts */
	if (!__irq_can_set_affinity(desc))
		return 0;

	raw_spin_lock(&mask_lock);
	/*
	 * Preserve the managed affinity setting and a userspace affinity
	 * setup, but make sure that one of the targets is online.
	 */
	if (irqd_affinity_is_managed(&desc->irq_data) ||
	    irqd_has_set(&desc->irq_data, IRQD_AFFINITY_SET)) {
		if (cpumask_intersects(desc->irq_common_data.affinity,
				       cpu_online_mask))
			set = desc->irq_common_data.affinity;
		else
			irqd_clear(&desc->irq_data, IRQD_AFFINITY_SET);
	}

	cpumask_and(&mask, cpu_online_mask, set);
	if (cpumask_empty(&mask))
		cpumask_copy(&mask, cpu_online_mask);

	if (node != NUMA_NO_NODE) {
		const struct cpumask *nodemask = cpumask_of_node(node);

		/* make sure at least one of the cpus in nodemask is online */
		if (cpumask_intersects(&mask, nodemask))
			cpumask_and(&mask, &mask, nodemask);
	}
	ret = irq_do_set_affinity(&desc->irq_data, &mask, false);
	raw_spin_unlock(&mask_lock);
	return ret;
}
#else
/* Wrapper for ALPHA specific affinity selector magic */
int irq_setup_affinity(struct irq_desc *desc)
{
	return irq_select_affinity(irq_desc_get_irq(desc));
}
#endif /* CONFIG_AUTO_IRQ_AFFINITY */
#endif /* CONFIG_SMP */


/**
 *	irq_set_vcpu_affinity - Set vcpu affinity for the interrupt
 *	@irq: interrupt number to set affinity
 *	@vcpu_info: vCPU specific data or pointer to a percpu array of vCPU
 *	            specific data for percpu_devid interrupts
 *
 *	This function uses the vCPU specific data to set the vCPU
 *	affinity for an irq. The vCPU specific data is passed from
 *	outside, such as KVM. One example code path is as below:
 *	KVM -> IOMMU -> irq_set_vcpu_affinity().
 */
int irq_set_vcpu_affinity(unsigned int irq, void *vcpu_info)
{
	unsigned long flags;
	struct irq_desc *desc = irq_get_desc_lock(irq, &flags, 0);
	struct irq_data *data;
	struct irq_chip *chip;
	int ret = -ENOSYS;

	if (!desc)
		return -EINVAL;

	data = irq_desc_get_irq_data(desc);
	do {
		chip = irq_data_get_irq_chip(data);
		if (chip && chip->irq_set_vcpu_affinity)
			break;
#ifdef CONFIG_IRQ_DOMAIN_HIERARCHY
		data = data->parent_data;
#else
		data = NULL;
#endif
	} while (data);

	if (data)
		ret = chip->irq_set_vcpu_affinity(data, vcpu_info);
	irq_put_desc_unlock(desc, flags);

	return ret;
}
EXPORT_SYMBOL_GPL(irq_set_vcpu_affinity);

void __disable_irq(struct irq_desc *desc)
{
	if (!desc->depth++)
		irq_disable(desc);
}

static int __disable_irq_nosync(unsigned int irq)
{
	unsigned long flags;
	struct irq_desc *desc = irq_get_desc_buslock(irq, &flags, IRQ_GET_DESC_CHECK_GLOBAL);

	if (!desc)
		return -EINVAL;
	__disable_irq(desc);
	irq_put_desc_busunlock(desc, flags);
	return 0;
}

/**
 *	disable_irq_nosync - disable an irq without waiting
 *	@irq: Interrupt to disable
 *
 *	Disable the selected interrupt line.  Disables and Enables are
 *	nested.
 *	Unlike disable_irq(), this function does not ensure existing
 *	instances of the IRQ handler have completed before returning.
 *
 *	This function may be called from IRQ context.
 */
void disable_irq_nosync(unsigned int irq)
{
	__disable_irq_nosync(irq);
}
EXPORT_SYMBOL(disable_irq_nosync);

/**
 *	disable_irq - disable an irq and wait for completion
 *	@irq: Interrupt to disable
 *
 *	Disable the selected interrupt line.  Enables and Disables are
 *	nested.
 *	This function waits for any pending IRQ handlers for this interrupt
 *	to complete before returning. If you use this function while
 *	holding a resource the IRQ handler may need you will deadlock.
 *
 *	This function may be called - with care - from IRQ context.
 */
void disable_irq(unsigned int irq)
{
	if (!__disable_irq_nosync(irq))
		synchronize_irq(irq);
}
EXPORT_SYMBOL(disable_irq);

/**
 *	disable_hardirq - disables an irq and waits for hardirq completion
 *	@irq: Interrupt to disable
 *
 *	Disable the selected interrupt line.  Enables and Disables are
 *	nested.
 *	This function waits for any pending hard IRQ handlers for this
 *	interrupt to complete before returning. If you use this function while
 *	holding a resource the hard IRQ handler may need you will deadlock.
 *
 *	When used to optimistically disable an interrupt from atomic context
 *	the return value must be checked.
 *
 *	Returns: false if a threaded handler is active.
 *
 *	This function may be called - with care - from IRQ context.
 */
bool disable_hardirq(unsigned int irq)
{
	if (!__disable_irq_nosync(irq))
		return synchronize_hardirq(irq);

	return false;
}
EXPORT_SYMBOL_GPL(disable_hardirq);

/**
 *	disable_nmi_nosync - disable an nmi without waiting
 *	@irq: Interrupt to disable
 *
 *	Disable the selected interrupt line. Disables and enables are
 *	nested.
 *	The interrupt to disable must have been requested through request_nmi.
 *	Unlike disable_nmi(), this function does not ensure existing
 *	instances of the IRQ handler have completed before returning.
 */
void disable_nmi_nosync(unsigned int irq)
{
	disable_irq_nosync(irq);
}

void __enable_irq(struct irq_desc *desc)
{
	switch (desc->depth) {
	case 0:
 err_out:
		WARN(1, KERN_WARNING "Unbalanced enable for IRQ %d\n",
		     irq_desc_get_irq(desc));
		break;
	case 1: {
		if (desc->istate & IRQS_SUSPENDED)
			goto err_out;
		/* Prevent probing on this irq: */
		irq_settings_set_noprobe(desc);
		/*
		 * Call irq_startup() not irq_enable() here because the
		 * interrupt might be marked NOAUTOEN. So irq_startup()
		 * needs to be invoked when it gets enabled the first
		 * time. If it was already started up, then irq_startup()
		 * will invoke irq_enable() under the hood.
		 */
		irq_startup(desc, IRQ_RESEND, IRQ_START_FORCE);
		break;
	}
	default:
		desc->depth--;
	}
}

/**
 *	enable_irq - enable handling of an irq
 *	@irq: Interrupt to enable
 *
 *	Undoes the effect of one call to disable_irq().  If this
 *	matches the last disable, processing of interrupts on this
 *	IRQ line is re-enabled.
 *
 *	This function may be called from IRQ context only when
 *	desc->irq_data.chip->bus_lock and desc->chip->bus_sync_unlock are NULL !
 */
void enable_irq(unsigned int irq)
{
	unsigned long flags;
	struct irq_desc *desc = irq_get_desc_buslock(irq, &flags, IRQ_GET_DESC_CHECK_GLOBAL);

	if (!desc)
		return;
	if (WARN(!desc->irq_data.chip,
		 KERN_ERR "enable_irq before setup/request_irq: irq %u\n", irq))
		goto out;

	__enable_irq(desc);
out:
	irq_put_desc_busunlock(desc, flags);
}
EXPORT_SYMBOL(enable_irq);

/**
 *	enable_nmi - enable handling of an nmi
 *	@irq: Interrupt to enable
 *
 *	The interrupt to enable must have been requested through request_nmi.
 *	Undoes the effect of one call to disable_nmi(). If this
 *	matches the last disable, processing of interrupts on this
 *	IRQ line is re-enabled.
 */
void enable_nmi(unsigned int irq)
{
	enable_irq(irq);
}

static int set_irq_wake_real(unsigned int irq, unsigned int on)
{
	struct irq_desc *desc = irq_to_desc(irq);
	int ret = -ENXIO;

	if (irq_desc_get_chip(desc)->flags &  IRQCHIP_SKIP_SET_WAKE)
		return 0;

	if (desc->irq_data.chip->irq_set_wake)
		ret = desc->irq_data.chip->irq_set_wake(&desc->irq_data, on);

	return ret;
}

/**
 *	irq_set_irq_wake - control irq power management wakeup
 *	@irq:	interrupt to control
 *	@on:	enable/disable power management wakeup
 *
 *	Enable/disable power management wakeup mode, which is
 *	disabled by default.  Enables and disables must match,
 *	just as they match for non-wakeup mode support.
 *
 *	Wakeup mode lets this IRQ wake the system from sleep
 *	states like "suspend to RAM".
 *
 *	Note: irq enable/disable state is completely orthogonal
 *	to the enable/disable state of irq wake. An irq can be
 *	disabled with disable_irq() and still wake the system as
 *	long as the irq has wake enabled. If this does not hold,
 *	then the underlying irq chip and the related driver need
 *	to be investigated.
 */
int irq_set_irq_wake(unsigned int irq, unsigned int on)
{
	unsigned long flags;
	struct irq_desc *desc = irq_get_desc_buslock(irq, &flags, IRQ_GET_DESC_CHECK_GLOBAL);
	int ret = 0;

	if (!desc)
		return -EINVAL;

	/* Don't use NMIs as wake up interrupts please */
	if (desc->istate & IRQS_NMI) {
		ret = -EINVAL;
		goto out_unlock;
	}

	/* wakeup-capable irqs can be shared between drivers that
	 * don't need to have the same sleep mode behaviors.
	 */
	if (on) {
		if (desc->wake_depth++ == 0) {
			ret = set_irq_wake_real(irq, on);
			if (ret)
				desc->wake_depth = 0;
			else
				irqd_set(&desc->irq_data, IRQD_WAKEUP_STATE);
		}
	} else {
		if (desc->wake_depth == 0) {
			WARN(1, "Unbalanced IRQ %d wake disable\n", irq);
		} else if (--desc->wake_depth == 0) {
			ret = set_irq_wake_real(irq, on);
			if (ret)
				desc->wake_depth = 1;
			else
				irqd_clear(&desc->irq_data, IRQD_WAKEUP_STATE);
		}
	}

out_unlock:
	irq_put_desc_busunlock(desc, flags);
	return ret;
}
EXPORT_SYMBOL(irq_set_irq_wake);

/*
 * Internal function that tells the architecture code whether a
 * particular irq has been exclusively allocated or is available
 * for driver use.
 */
int can_request_irq(unsigned int irq, unsigned long irqflags)
{
	unsigned long flags;
	struct irq_desc *desc = irq_get_desc_lock(irq, &flags, 0);
	int canrequest = 0;

	if (!desc)
		return 0;

	if (irq_settings_can_request(desc)) {
		if (!desc->action ||
		    irqflags & desc->action->flags & IRQF_SHARED)
			canrequest = 1;
	}
	irq_put_desc_unlock(desc, flags);
	return canrequest;
}

int __irq_set_trigger(struct irq_desc *desc, unsigned long flags)
{
	struct irq_chip *chip = desc->irq_data.chip;
	int ret, unmask = 0;

	if (!chip || !chip->irq_set_type) {
		/*
		 * IRQF_TRIGGER_* but the PIC does not support multiple
		 * flow-types?
		 */
		pr_debug("No set_type function for IRQ %d (%s)\n",
			 irq_desc_get_irq(desc),
			 chip ? (chip->name ? : "unknown") : "unknown");
		return 0;
	}

	if (chip->flags & IRQCHIP_SET_TYPE_MASKED) {
		if (!irqd_irq_masked(&desc->irq_data))
			mask_irq(desc);
		if (!irqd_irq_disabled(&desc->irq_data))
			unmask = 1;
	}

	/* Mask all flags except trigger mode */
	flags &= IRQ_TYPE_SENSE_MASK;
	ret = chip->irq_set_type(&desc->irq_data, flags);

	switch (ret) {
	case IRQ_SET_MASK_OK:
	case IRQ_SET_MASK_OK_DONE:
		irqd_clear(&desc->irq_data, IRQD_TRIGGER_MASK);
		irqd_set(&desc->irq_data, flags);
		fallthrough;

	case IRQ_SET_MASK_OK_NOCOPY:
		flags = irqd_get_trigger_type(&desc->irq_data);
		irq_settings_set_trigger_mask(desc, flags);
		irqd_clear(&desc->irq_data, IRQD_LEVEL);
		irq_settings_clr_level(desc);
		if (flags & IRQ_TYPE_LEVEL_MASK) {
			irq_settings_set_level(desc);
			irqd_set(&desc->irq_data, IRQD_LEVEL);
		}

		ret = 0;
		break;
	default:
		pr_err("Setting trigger mode %lu for irq %u failed (%pS)\n",
		       flags, irq_desc_get_irq(desc), chip->irq_set_type);
	}
	if (unmask)
		unmask_irq(desc);
	return ret;
}

#ifdef CONFIG_HARDIRQS_SW_RESEND
int irq_set_parent(int irq, int parent_irq)
{
	unsigned long flags;
	struct irq_desc *desc = irq_get_desc_lock(irq, &flags, 0);

	if (!desc)
		return -EINVAL;

	desc->parent_irq = parent_irq;

	irq_put_desc_unlock(desc, flags);
	return 0;
}
EXPORT_SYMBOL_GPL(irq_set_parent);
#endif

/*
 * Default primary interrupt handler for threaded interrupts. Is
 * assigned as primary handler when request_threaded_irq is called
 * with handler == NULL. Useful for oneshot interrupts.
 */
static irqreturn_t irq_default_primary_handler(int irq, void *dev_id)
{
	return IRQ_WAKE_THREAD;
}

/*
 * Primary handler for nested threaded interrupts. Should never be
 * called.
 */
static irqreturn_t irq_nested_primary_handler(int irq, void *dev_id)
{
	WARN(1, "Primary handler called for nested irq %d\n", irq);
	return IRQ_NONE;
}

static irqreturn_t irq_forced_secondary_handler(int irq, void *dev_id)
{
	WARN(1, "Secondary action handler called for irq %d\n", irq);
	return IRQ_NONE;
}

static int irq_wait_for_interrupt(struct irqaction *action)
{
	for (;;) {
		set_current_state(TASK_INTERRUPTIBLE);

		if (kthread_should_stop()) {
			/* may need to run one last time */
			if (test_and_clear_bit(IRQTF_RUNTHREAD,
					       &action->thread_flags)) {
				__set_current_state(TASK_RUNNING);
				return 0;
			}
			__set_current_state(TASK_RUNNING);
			return -1;
		}

		if (test_and_clear_bit(IRQTF_RUNTHREAD,
				       &action->thread_flags)) {
			__set_current_state(TASK_RUNNING);
			return 0;
		}
		schedule();
	}
}

/*
 * Oneshot interrupts keep the irq line masked until the threaded
 * handler finished. unmask if the interrupt has not been disabled and
 * is marked MASKED.
 */
static void irq_finalize_oneshot(struct irq_desc *desc,
				 struct irqaction *action)
{
	if (!(desc->istate & IRQS_ONESHOT) ||
	    action->handler == irq_forced_secondary_handler)
		return;
again:
	chip_bus_lock(desc);
	raw_spin_lock_irq(&desc->lock);

	/*
	 * Implausible though it may be we need to protect us against
	 * the following scenario:
	 *
	 * The thread is faster done than the hard interrupt handler
	 * on the other CPU. If we unmask the irq line then the
	 * interrupt can come in again and masks the line, leaves due
	 * to IRQS_INPROGRESS and the irq line is masked forever.
	 *
	 * This also serializes the state of shared oneshot handlers
	 * versus "desc->threads_oneshot |= action->thread_mask;" in
	 * irq_wake_thread(). See the comment there which explains the
	 * serialization.
	 */
	if (unlikely(irqd_irq_inprogress(&desc->irq_data))) {
		raw_spin_unlock_irq(&desc->lock);
		chip_bus_sync_unlock(desc);
		cpu_relax();
		goto again;
	}

	/*
	 * Now check again, whether the thread should run. Otherwise
	 * we would clear the threads_oneshot bit of this thread which
	 * was just set.
	 */
	if (test_bit(IRQTF_RUNTHREAD, &action->thread_flags))
		goto out_unlock;

	desc->threads_oneshot &= ~action->thread_mask;

	if (!desc->threads_oneshot && !irqd_irq_disabled(&desc->irq_data) &&
	    irqd_irq_masked(&desc->irq_data))
		unmask_threaded_irq(desc);

out_unlock:
	raw_spin_unlock_irq(&desc->lock);
	chip_bus_sync_unlock(desc);
}

#ifdef CONFIG_SMP
/*
 * Check whether we need to change the affinity of the interrupt thread.
 */
static void
irq_thread_check_affinity(struct irq_desc *desc, struct irqaction *action)
{
	cpumask_var_t mask;
	bool valid = true;

	if (!test_and_clear_bit(IRQTF_AFFINITY, &action->thread_flags))
		return;

	/*
	 * In case we are out of memory we set IRQTF_AFFINITY again and
	 * try again next time
	 */
	if (!alloc_cpumask_var(&mask, GFP_KERNEL)) {
		set_bit(IRQTF_AFFINITY, &action->thread_flags);
		return;
	}

	raw_spin_lock_irq(&desc->lock);
	/*
	 * This code is triggered unconditionally. Check the affinity
	 * mask pointer. For CPU_MASK_OFFSTACK=n this is optimized out.
	 */
	if (cpumask_available(desc->irq_common_data.affinity)) {
		const struct cpumask *m;

		m = irq_data_get_effective_affinity_mask(&desc->irq_data);
		cpumask_copy(mask, m);
	} else {
		valid = false;
	}
	raw_spin_unlock_irq(&desc->lock);

	if (valid)
		set_cpus_allowed_ptr(current, mask);
	free_cpumask_var(mask);
}
#else
static inline void
irq_thread_check_affinity(struct irq_desc *desc, struct irqaction *action) { }
#endif

/*
 * Interrupts which are not explicitly requested as threaded
 * interrupts rely on the implicit bh/preempt disable of the hard irq
 * context. So we need to disable bh here to avoid deadlocks and other
 * side effects.
 */
static irqreturn_t
irq_forced_thread_fn(struct irq_desc *desc, struct irqaction *action)
{
	irqreturn_t ret;

	local_bh_disable();
	if (!IS_ENABLED(CONFIG_PREEMPT_RT))
		local_irq_disable();
	ret = action->thread_fn(action->irq, action->dev_id);
	if (ret == IRQ_HANDLED)
		atomic_inc(&desc->threads_handled);

	irq_finalize_oneshot(desc, action);
	if (!IS_ENABLED(CONFIG_PREEMPT_RT))
		local_irq_enable();
	local_bh_enable();
	return ret;
}

/*
 * Interrupts explicitly requested as threaded interrupts want to be
 * preemptible - many of them need to sleep and wait for slow busses to
 * complete.
 */
static irqreturn_t irq_thread_fn(struct irq_desc *desc,
		struct irqaction *action)
{
	irqreturn_t ret;

	ret = action->thread_fn(action->irq, action->dev_id);
	if (ret == IRQ_HANDLED)
		atomic_inc(&desc->threads_handled);

	irq_finalize_oneshot(desc, action);
	return ret;
}

static void wake_threads_waitq(struct irq_desc *desc)
{
	if (atomic_dec_and_test(&desc->threads_active))
		wake_up(&desc->wait_for_threads);
}

static void irq_thread_dtor(struct callback_head *unused)
{
	struct task_struct *tsk = current;
	struct irq_desc *desc;
	struct irqaction *action;

	if (WARN_ON_ONCE(!(current->flags & PF_EXITING)))
		return;

	action = kthread_data(tsk);

	pr_err("exiting task \"%s\" (%d) is an active IRQ thread (irq %d)\n",
	       tsk->comm, tsk->pid, action->irq);


	desc = irq_to_desc(action->irq);
	/*
	 * If IRQTF_RUNTHREAD is set, we need to decrement
	 * desc->threads_active and wake possible waiters.
	 */
	if (test_and_clear_bit(IRQTF_RUNTHREAD, &action->thread_flags))
		wake_threads_waitq(desc);

	/* Prevent a stale desc->threads_oneshot */
	irq_finalize_oneshot(desc, action);
}

static void irq_wake_secondary(struct irq_desc *desc, struct irqaction *action)
{
	struct irqaction *secondary = action->secondary;

	if (WARN_ON_ONCE(!secondary))
		return;

	raw_spin_lock_irq(&desc->lock);
	__irq_wake_thread(desc, secondary);
	raw_spin_unlock_irq(&desc->lock);
}

/*
 * Interrupt handler thread
 */
static int irq_thread(void *data)
{
	struct callback_head on_exit_work;
	struct irqaction *action = data;
	struct irq_desc *desc = irq_to_desc(action->irq);
	irqreturn_t (*handler_fn)(struct irq_desc *desc,
			struct irqaction *action);

	if (force_irqthreads && test_bit(IRQTF_FORCED_THREAD,
					&action->thread_flags))
		handler_fn = irq_forced_thread_fn;
	else
		handler_fn = irq_thread_fn;

	init_task_work(&on_exit_work, irq_thread_dtor);
	task_work_add(current, &on_exit_work, TWA_NONE);

	irq_thread_check_affinity(desc, action);

	while (!irq_wait_for_interrupt(action)) {
		irqreturn_t action_ret;

		irq_thread_check_affinity(desc, action);

		action_ret = handler_fn(desc, action);
		if (action_ret == IRQ_WAKE_THREAD)
			irq_wake_secondary(desc, action);

		wake_threads_waitq(desc);
	}

	/*
	 * This is the regular exit path. __free_irq() is stopping the
	 * thread via kthread_stop() after calling
	 * synchronize_hardirq(). So neither IRQTF_RUNTHREAD nor the
	 * oneshot mask bit can be set.
	 */
	task_work_cancel(current, irq_thread_dtor);
	return 0;
}

/**
 *	irq_wake_thread - wake the irq thread for the action identified by dev_id
 *	@irq:		Interrupt line
 *	@dev_id:	Device identity for which the thread should be woken
 *
 */
void irq_wake_thread(unsigned int irq, void *dev_id)
{
	struct irq_desc *desc = irq_to_desc(irq);
	struct irqaction *action;
	unsigned long flags;

	if (!desc || WARN_ON(irq_settings_is_per_cpu_devid(desc)))
		return;

	raw_spin_lock_irqsave(&desc->lock, flags);
	for_each_action_of_desc(desc, action) {
		if (action->dev_id == dev_id) {
			if (action->thread)
				__irq_wake_thread(desc, action);
			break;
		}
	}
	raw_spin_unlock_irqrestore(&desc->lock, flags);
}
EXPORT_SYMBOL_GPL(irq_wake_thread);

static int irq_setup_forced_threading(struct irqaction *new)
{
	if (!force_irqthreads)
		return 0;
	if (new->flags & (IRQF_NO_THREAD | IRQF_PERCPU | IRQF_ONESHOT))
		return 0;

	/*
	 * No further action required for interrupts which are requested as
	 * threaded interrupts already
	 */
	if (new->handler == irq_default_primary_handler)
		return 0;

	new->flags |= IRQF_ONESHOT;

	/*
	 * Handle the case where we have a real primary handler and a
	 * thread handler. We force thread them as well by creating a
	 * secondary action.
	 */
	if (new->handler && new->thread_fn) {
		/* Allocate the secondary action */
		new->secondary = kzalloc(sizeof(struct irqaction), GFP_KERNEL);
		if (!new->secondary)
			return -ENOMEM;
		new->secondary->handler = irq_forced_secondary_handler;
		new->secondary->thread_fn = new->thread_fn;
		new->secondary->dev_id = new->dev_id;
		new->secondary->irq = new->irq;
		new->secondary->name = new->name;
	}
	/* Deal with the primary handler */
	set_bit(IRQTF_FORCED_THREAD, &new->thread_flags);
	new->thread_fn = new->handler;
	new->handler = irq_default_primary_handler;
	return 0;
}

static int irq_request_resources(struct irq_desc *desc)
{
	struct irq_data *d = &desc->irq_data;
	struct irq_chip *c = d->chip;

	return c->irq_request_resources ? c->irq_request_resources(d) : 0;
}

static void irq_release_resources(struct irq_desc *desc)
{
	struct irq_data *d = &desc->irq_data;
	struct irq_chip *c = d->chip;

	if (c->irq_release_resources)
		c->irq_release_resources(d);
}

static bool irq_supports_nmi(struct irq_desc *desc)
{
	struct irq_data *d = irq_desc_get_irq_data(desc);

#ifdef CONFIG_IRQ_DOMAIN_HIERARCHY
	/* Only IRQs directly managed by the root irqchip can be set as NMI */
	if (d->parent_data)
		return false;
#endif
	/* Don't support NMIs for chips behind a slow bus */
	if (d->chip->irq_bus_lock || d->chip->irq_bus_sync_unlock)
		return false;

	return d->chip->flags & IRQCHIP_SUPPORTS_NMI;
}

static int irq_nmi_setup(struct irq_desc *desc)
{
	struct irq_data *d = irq_desc_get_irq_data(desc);
	struct irq_chip *c = d->chip;

	return c->irq_nmi_setup ? c->irq_nmi_setup(d) : -EINVAL;
}

static void irq_nmi_teardown(struct irq_desc *desc)
{
	struct irq_data *d = irq_desc_get_irq_data(desc);
	struct irq_chip *c = d->chip;

	if (c->irq_nmi_teardown)
		c->irq_nmi_teardown(d);
}

static int
setup_irq_thread(struct irqaction *new, unsigned int irq, bool secondary)
{
	struct task_struct *t;

	if (!secondary) {
		t = kthread_create(irq_thread, new, "irq/%d-%s", irq,
				   new->name);
	} else {
		t = kthread_create(irq_thread, new, "irq/%d-s-%s", irq,
				   new->name);
	}

	if (IS_ERR(t))
		return PTR_ERR(t);

	sched_set_fifo(t);

	/*
	 * We keep the reference to the task struct even if
	 * the thread dies to avoid that the interrupt code
	 * references an already freed task_struct.
	 */
	new->thread = get_task_struct(t);
	/*
	 * Tell the thread to set its affinity. This is
	 * important for shared interrupt handlers as we do
	 * not invoke setup_affinity() for the secondary
	 * handlers as everything is already set up. Even for
	 * interrupts marked with IRQF_NO_BALANCE this is
	 * correct as we want the thread to move to the cpu(s)
	 * on which the requesting code placed the interrupt.
	 */
	set_bit(IRQTF_AFFINITY, &new->thread_flags);
	return 0;
}

/*
 * Internal function to register an irqaction - typically used to
 * allocate special interrupts that are part of the architecture.
 *
 * Locking rules:
 *
 * desc->request_mutex	Provides serialization against a concurrent free_irq()
 *   chip_bus_lock	Provides serialization for slow bus operations
 *     desc->lock	Provides serialization against hard interrupts
 *
 * chip_bus_lock and desc->lock are sufficient for all other management and
 * interrupt related functions. desc->request_mutex solely serializes
 * request/free_irq().
 */
static int
__setup_irq(unsigned int irq, struct irq_desc *desc, struct irqaction *new)
{
	struct irqaction *old, **old_ptr;
	unsigned long flags, thread_mask = 0;
	int ret, nested, shared = 0;

	if (!desc)
		return -EINVAL;

	if (desc->irq_data.chip == &no_irq_chip)
		return -ENOSYS;
	if (!try_module_get(desc->owner))
		return -ENODEV;

	new->irq = irq;

	/*
	 * If the trigger type is not specified by the caller,
	 * then use the default for this interrupt.
	 */
	if (!(new->flags & IRQF_TRIGGER_MASK))
		new->flags |= irqd_get_trigger_type(&desc->irq_data);

	/*
	 * Check whether the interrupt nests into another interrupt
	 * thread.
	 */
	nested = irq_settings_is_nested_thread(desc);
	if (nested) {
		if (!new->thread_fn) {
			ret = -EINVAL;
			goto out_mput;
		}
		/*
		 * Replace the primary handler which was provided from
		 * the driver for non nested interrupt handling by the
		 * dummy function which warns when called.
		 */
		new->handler = irq_nested_primary_handler;
	} else {
		if (irq_settings_can_thread(desc)) {
			ret = irq_setup_forced_threading(new);
			if (ret)
				goto out_mput;
		}
	}

	/*
	 * Create a handler thread when a thread function is supplied
	 * and the interrupt does not nest into another interrupt
	 * thread.
	 */
	if (new->thread_fn && !nested) {
		ret = setup_irq_thread(new, irq, false);
		if (ret)
			goto out_mput;
		if (new->secondary) {
			ret = setup_irq_thread(new->secondary, irq, true);
			if (ret)
				goto out_thread;
		}
	}

	/*
	 * Drivers are often written to work w/o knowledge about the
	 * underlying irq chip implementation, so a request for a
	 * threaded irq without a primary hard irq context handler
	 * requires the ONESHOT flag to be set. Some irq chips like
	 * MSI based interrupts are per se one shot safe. Check the
	 * chip flags, so we can avoid the unmask dance at the end of
	 * the threaded handler for those.
	 */
	if (desc->irq_data.chip->flags & IRQCHIP_ONESHOT_SAFE)
		new->flags &= ~IRQF_ONESHOT;

	/*
	 * Protects against a concurrent __free_irq() call which might wait
	 * for synchronize_hardirq() to complete without holding the optional
	 * chip bus lock and desc->lock. Also protects against handing out
	 * a recycled oneshot thread_mask bit while it's still in use by
	 * its previous owner.
	 */
	mutex_lock(&desc->request_mutex);

	/*
	 * Acquire bus lock as the irq_request_resources() callback below
	 * might rely on the serialization or the magic power management
	 * functions which are abusing the irq_bus_lock() callback,
	 */
	chip_bus_lock(desc);

	/* First installed action requests resources. */
	if (!desc->action) {
		ret = irq_request_resources(desc);
		if (ret) {
			pr_err("Failed to request resources for %s (irq %d) on irqchip %s\n",
			       new->name, irq, desc->irq_data.chip->name);
			goto out_bus_unlock;
		}
	}

	/*
	 * The following block of code has to be executed atomically
	 * protected against a concurrent interrupt and any of the other
	 * management calls which are not serialized via
	 * desc->request_mutex or the optional bus lock.
	 */
	raw_spin_lock_irqsave(&desc->lock, flags);
	old_ptr = &desc->action;
	old = *old_ptr;
	if (old) {
		/*
		 * Can't share interrupts unless both agree to and are
		 * the same type (level, edge, polarity). So both flag
		 * fields must have IRQF_SHARED set and the bits which
		 * set the trigger type must match. Also all must
		 * agree on ONESHOT.
		 * Interrupt lines used for NMIs cannot be shared.
		 */
		unsigned int oldtype;

		if (desc->istate & IRQS_NMI) {
			pr_err("Invalid attempt to share NMI for %s (irq %d) on irqchip %s.\n",
				new->name, irq, desc->irq_data.chip->name);
			ret = -EINVAL;
			goto out_unlock;
		}

		/*
		 * If nobody did set the configuration before, inherit
		 * the one provided by the requester.
		 */
		if (irqd_trigger_type_was_set(&desc->irq_data)) {
			oldtype = irqd_get_trigger_type(&desc->irq_data);
		} else {
			oldtype = new->flags & IRQF_TRIGGER_MASK;
			irqd_set_trigger_type(&desc->irq_data, oldtype);
		}

		if (!((old->flags & new->flags) & IRQF_SHARED) ||
		    (oldtype != (new->flags & IRQF_TRIGGER_MASK)) ||
		    ((old->flags ^ new->flags) & IRQF_ONESHOT))
			goto mismatch;

		/* All handlers must agree on per-cpuness */
		if ((old->flags & IRQF_PERCPU) !=
		    (new->flags & IRQF_PERCPU))
			goto mismatch;

		/* add new interrupt at end of irq queue */
		do {
			/*
			 * Or all existing action->thread_mask bits,
			 * so we can find the next zero bit for this
			 * new action.
			 */
			thread_mask |= old->thread_mask;
			old_ptr = &old->next;
			old = *old_ptr;
		} while (old);
		shared = 1;
	}

	/*
	 * Setup the thread mask for this irqaction for ONESHOT. For
	 * !ONESHOT irqs the thread mask is 0 so we can avoid a
	 * conditional in irq_wake_thread().
	 */
	if (new->flags & IRQF_ONESHOT) {
		/*
		 * Unlikely to have 32 resp 64 irqs sharing one line,
		 * but who knows.
		 */
		if (thread_mask == ~0UL) {
			ret = -EBUSY;
			goto out_unlock;
		}
		/*
		 * The thread_mask for the action is or'ed to
		 * desc->thread_active to indicate that the
		 * IRQF_ONESHOT thread handler has been woken, but not
		 * yet finished. The bit is cleared when a thread
		 * completes. When all threads of a shared interrupt
		 * line have completed desc->threads_active becomes
		 * zero and the interrupt line is unmasked. See
		 * handle.c:irq_wake_thread() for further information.
		 *
		 * If no thread is woken by primary (hard irq context)
		 * interrupt handlers, then desc->threads_active is
		 * also checked for zero to unmask the irq line in the
		 * affected hard irq flow handlers
		 * (handle_[fasteoi|level]_irq).
		 *
		 * The new action gets the first zero bit of
		 * thread_mask assigned. See the loop above which or's
		 * all existing action->thread_mask bits.
		 */
		new->thread_mask = 1UL << ffz(thread_mask);

	} else if (new->handler == irq_default_primary_handler &&
		   !(desc->irq_data.chip->flags & IRQCHIP_ONESHOT_SAFE)) {
		/*
		 * The interrupt was requested with handler = NULL, so
		 * we use the default primary handler for it. But it
		 * does not have the oneshot flag set. In combination
		 * with level interrupts this is deadly, because the
		 * default primary handler just wakes the thread, then
		 * the irq lines is reenabled, but the device still
		 * has the level irq asserted. Rinse and repeat....
		 *
		 * While this works for edge type interrupts, we play
		 * it safe and reject unconditionally because we can't
		 * say for sure which type this interrupt really
		 * has. The type flags are unreliable as the
		 * underlying chip implementation can override them.
		 */
		pr_err("Threaded irq requested with handler=NULL and !ONESHOT for %s (irq %d)\n",
		       new->name, irq);
		ret = -EINVAL;
		goto out_unlock;
	}

	if (!shared) {
		init_waitqueue_head(&desc->wait_for_threads);

		/* Setup the type (level, edge polarity) if configured: */
		if (new->flags & IRQF_TRIGGER_MASK) {
			ret = __irq_set_trigger(desc,
						new->flags & IRQF_TRIGGER_MASK);

			if (ret)
				goto out_unlock;
		}

		/*
		 * Activate the interrupt. That activation must happen
		 * independently of IRQ_NOAUTOEN. request_irq() can fail
		 * and the callers are supposed to handle
		 * that. enable_irq() of an interrupt requested with
		 * IRQ_NOAUTOEN is not supposed to fail. The activation
		 * keeps it in shutdown mode, it merily associates
		 * resources if necessary and if that's not possible it
		 * fails. Interrupts which are in managed shutdown mode
		 * will simply ignore that activation request.
		 */
		ret = irq_activate(desc);
		if (ret)
			goto out_unlock;

		desc->istate &= ~(IRQS_AUTODETECT | IRQS_SPURIOUS_DISABLED | \
				  IRQS_ONESHOT | IRQS_WAITING);
		irqd_clear(&desc->irq_data, IRQD_IRQ_INPROGRESS);

		if (new->flags & IRQF_PERCPU) {
			irqd_set(&desc->irq_data, IRQD_PER_CPU);
			irq_settings_set_per_cpu(desc);
		}

		if (new->flags & IRQF_ONESHOT)
			desc->istate |= IRQS_ONESHOT;

		/* Exclude IRQ from balancing if requested */
		if (new->flags & IRQF_NOBALANCING) {
			irq_settings_set_no_balancing(desc);
			irqd_set(&desc->irq_data, IRQD_NO_BALANCING);
		}

		if (!(new->flags & IRQF_NO_AUTOEN) &&
		    irq_settings_can_autoenable(desc)) {
			irq_startup(desc, IRQ_RESEND, IRQ_START_COND);
		} else {
			/*
			 * Shared interrupts do not go well with disabling
			 * auto enable. The sharing interrupt might request
			 * it while it's still disabled and then wait for
			 * interrupts forever.
			 */
			WARN_ON_ONCE(new->flags & IRQF_SHARED);
			/* Undo nested disables: */
			desc->depth = 1;
		}

	} else if (new->flags & IRQF_TRIGGER_MASK) {
		unsigned int nmsk = new->flags & IRQF_TRIGGER_MASK;
		unsigned int omsk = irqd_get_trigger_type(&desc->irq_data);

		if (nmsk != omsk)
			/* hope the handler works with current  trigger mode */
			pr_warn("irq %d uses trigger mode %u; requested %u\n",
				irq, omsk, nmsk);
	}

	*old_ptr = new;

	irq_pm_install_action(desc, new);

	/* Reset broken irq detection when installing new handler */
	desc->irq_count = 0;
	desc->irqs_unhandled = 0;

	/*
	 * Check whether we disabled the irq via the spurious handler
	 * before. Reenable it and give it another chance.
	 */
	if (shared && (desc->istate & IRQS_SPURIOUS_DISABLED)) {
		desc->istate &= ~IRQS_SPURIOUS_DISABLED;
		__enable_irq(desc);
	}

	raw_spin_unlock_irqrestore(&desc->lock, flags);
	chip_bus_sync_unlock(desc);
	mutex_unlock(&desc->request_mutex);

	irq_setup_timings(desc, new);

	/*
	 * Strictly no need to wake it up, but hung_task complains
	 * when no hard interrupt wakes the thread up.
	 */
	if (new->thread)
		wake_up_process(new->thread);
	if (new->secondary)
		wake_up_process(new->secondary->thread);

	register_irq_proc(irq, desc);
	new->dir = NULL;
	register_handler_proc(irq, new);
	return 0;

mismatch:
	if (!(new->flags & IRQF_PROBE_SHARED)) {
		pr_err("Flags mismatch irq %d. %08x (%s) vs. %08x (%s)\n",
		       irq, new->flags, new->name, old->flags, old->name);
#ifdef CONFIG_DEBUG_SHIRQ
		dump_stack();
#endif
	}
	ret = -EBUSY;

out_unlock:
	raw_spin_unlock_irqrestore(&desc->lock, flags);

	if (!desc->action)
		irq_release_resources(desc);
out_bus_unlock:
	chip_bus_sync_unlock(desc);
	mutex_unlock(&desc->request_mutex);

out_thread:
	if (new->thread) {
		struct task_struct *t = new->thread;

		new->thread = NULL;
		kthread_stop(t);
		put_task_struct(t);
	}
	if (new->secondary && new->secondary->thread) {
		struct task_struct *t = new->secondary->thread;

		new->secondary->thread = NULL;
		kthread_stop(t);
		put_task_struct(t);
	}
out_mput:
	module_put(desc->owner);
	return ret;
}

/*
 * Internal function to unregister an irqaction - used to free
 * regular and special interrupts that are part of the architecture.
 */
static struct irqaction *__free_irq(struct irq_desc *desc, void *dev_id)
{
	unsigned irq = desc->irq_data.irq;
	struct irqaction *action, **action_ptr;
	unsigned long flags;

	WARN(in_interrupt(), "Trying to free IRQ %d from IRQ context!\n", irq);

	mutex_lock(&desc->request_mutex);
	chip_bus_lock(desc);
	raw_spin_lock_irqsave(&desc->lock, flags);

	/*
	 * There can be multiple actions per IRQ descriptor, find the right
	 * one based on the dev_id:
	 */
	action_ptr = &desc->action;
	for (;;) {
		action = *action_ptr;

		if (!action) {
			WARN(1, "Trying to free already-free IRQ %d\n", irq);
			raw_spin_unlock_irqrestore(&desc->lock, flags);
			chip_bus_sync_unlock(desc);
			mutex_unlock(&desc->request_mutex);
			return NULL;
		}

		if (action->dev_id == dev_id)
			break;
		action_ptr = &action->next;
	}

	/* Found it - now remove it from the list of entries: */
	*action_ptr = action->next;

	irq_pm_remove_action(desc, action);

	/* If this was the last handler, shut down the IRQ line: */
	if (!desc->action) {
		irq_settings_clr_disable_unlazy(desc);
		/* Only shutdown. Deactivate after synchronize_hardirq() */
		irq_shutdown(desc);
	}

#ifdef CONFIG_SMP
	/* make sure affinity_hint is cleaned up */
	if (WARN_ON_ONCE(desc->affinity_hint))
		desc->affinity_hint = NULL;
#endif

	raw_spin_unlock_irqrestore(&desc->lock, flags);
	/*
	 * Drop bus_lock here so the changes which were done in the chip
	 * callbacks above are synced out to the irq chips which hang
	 * behind a slow bus (I2C, SPI) before calling synchronize_hardirq().
	 *
	 * Aside of that the bus_lock can also be taken from the threaded
	 * handler in irq_finalize_oneshot() which results in a deadlock
	 * because kthread_stop() would wait forever for the thread to
	 * complete, which is blocked on the bus lock.
	 *
	 * The still held desc->request_mutex() protects against a
	 * concurrent request_irq() of this irq so the release of resources
	 * and timing data is properly serialized.
	 */
	chip_bus_sync_unlock(desc);

	unregister_handler_proc(irq, action);

	/*
	 * Make sure it's not being used on another CPU and if the chip
	 * supports it also make sure that there is no (not yet serviced)
	 * interrupt in flight at the hardware level.
	 */
	__synchronize_hardirq(desc, true);

#ifdef CONFIG_DEBUG_SHIRQ
	/*
	 * It's a shared IRQ -- the driver ought to be prepared for an IRQ
	 * event to happen even now it's being freed, so let's make sure that
	 * is so by doing an extra call to the handler ....
	 *
	 * ( We do this after actually deregistering it, to make sure that a
	 *   'real' IRQ doesn't run in parallel with our fake. )
	 */
	if (action->flags & IRQF_SHARED) {
		local_irq_save(flags);
		action->handler(irq, dev_id);
		local_irq_restore(flags);
	}
#endif

	/*
	 * The action has already been removed above, but the thread writes
	 * its oneshot mask bit when it completes. Though request_mutex is
	 * held across this which prevents __setup_irq() from handing out
	 * the same bit to a newly requested action.
	 */
	if (action->thread) {
		kthread_stop(action->thread);
		put_task_struct(action->thread);
		if (action->secondary && action->secondary->thread) {
			kthread_stop(action->secondary->thread);
			put_task_struct(action->secondary->thread);
		}
	}

	/* Last action releases resources */
	if (!desc->action) {
		/*
		 * Reacquire bus lock as irq_release_resources() might
		 * require it to deallocate resources over the slow bus.
		 */
		chip_bus_lock(desc);
		/*
		 * There is no interrupt on the fly anymore. Deactivate it
		 * completely.
		 */
		raw_spin_lock_irqsave(&desc->lock, flags);
		irq_domain_deactivate_irq(&desc->irq_data);
		raw_spin_unlock_irqrestore(&desc->lock, flags);

		irq_release_resources(desc);
		chip_bus_sync_unlock(desc);
		irq_remove_timings(desc);
	}

	mutex_unlock(&desc->request_mutex);

	irq_chip_pm_put(&desc->irq_data);
	module_put(desc->owner);
	kfree(action->secondary);
	return action;
}

/**
 *	free_irq - free an interrupt allocated with request_irq
 *	@irq: Interrupt line to free
 *	@dev_id: Device identity to free
 *
 *	Remove an interrupt handler. The handler is removed and if the
 *	interrupt line is no longer in use by any driver it is disabled.
 *	On a shared IRQ the caller must ensure the interrupt is disabled
 *	on the card it drives before calling this function. The function
 *	does not return until any executing interrupts for this IRQ
 *	have completed.
 *
 *	This function must not be called from interrupt context.
 *
 *	Returns the devname argument passed to request_irq.
 */
const void *free_irq(unsigned int irq, void *dev_id)
{
	struct irq_desc *desc = irq_to_desc(irq);
	struct irqaction *action;
	const char *devname;

	if (!desc || WARN_ON(irq_settings_is_per_cpu_devid(desc)))
		return NULL;

#ifdef CONFIG_SMP
	if (WARN_ON(desc->affinity_notify))
		desc->affinity_notify = NULL;
#endif

	action = __free_irq(desc, dev_id);

	if (!action)
		return NULL;

	devname = action->name;
	kfree(action);
	return devname;
}
EXPORT_SYMBOL(free_irq);

/* This function must be called with desc->lock held */
static const void *__cleanup_nmi(unsigned int irq, struct irq_desc *desc)
{
	const char *devname = NULL;

	desc->istate &= ~IRQS_NMI;

	if (!WARN_ON(desc->action == NULL)) {
		irq_pm_remove_action(desc, desc->action);
		devname = desc->action->name;
		unregister_handler_proc(irq, desc->action);

		kfree(desc->action);
		desc->action = NULL;
	}

	irq_settings_clr_disable_unlazy(desc);
	irq_shutdown_and_deactivate(desc);

	irq_release_resources(desc);

	irq_chip_pm_put(&desc->irq_data);
	module_put(desc->owner);

	return devname;
}

const void *free_nmi(unsigned int irq, void *dev_id)
{
	struct irq_desc *desc = irq_to_desc(irq);
	unsigned long flags;
	const void *devname;

	if (!desc || WARN_ON(!(desc->istate & IRQS_NMI)))
		return NULL;

	if (WARN_ON(irq_settings_is_per_cpu_devid(desc)))
		return NULL;

	/* NMI still enabled */
	if (WARN_ON(desc->depth == 0))
		disable_nmi_nosync(irq);

	raw_spin_lock_irqsave(&desc->lock, flags);

	irq_nmi_teardown(desc);
	devname = __cleanup_nmi(irq, desc);

	raw_spin_unlock_irqrestore(&desc->lock, flags);

	return devname;
}

/**
 *	request_threaded_irq - allocate an interrupt line
 *	@irq: Interrupt line to allocate
 *	@handler: Function to be called when the IRQ occurs.
 *		  Primary handler for threaded interrupts
 *		  If NULL and thread_fn != NULL the default
 *		  primary handler is installed
 *	@thread_fn: Function called from the irq handler thread
 *		    If NULL, no irq thread is created
 *	@irqflags: Interrupt type flags
 *	@devname: An ascii name for the claiming device
 *	@dev_id: A cookie passed back to the handler function
 *
 *	This call allocates interrupt resources and enables the
 *	interrupt line and IRQ handling. From the point this
 *	call is made your handler function may be invoked. Since
 *	your handler function must clear any interrupt the board
 *	raises, you must take care both to initialise your hardware
 *	and to set up the interrupt handler in the right order.
 *
 *	If you want to set up a threaded irq handler for your device
 *	then you need to supply @handler and @thread_fn. @handler is
 *	still called in hard interrupt context and has to check
 *	whether the interrupt originates from the device. If yes it
 *	needs to disable the interrupt on the device and return
 *	IRQ_WAKE_THREAD which will wake up the handler thread and run
 *	@thread_fn. This split handler design is necessary to support
 *	shared interrupts.
 *
 *	Dev_id must be globally unique. Normally the address of the
 *	device data structure is used as the cookie. Since the handler
 *	receives this value it makes sense to use it.
 *
 *	If your interrupt is shared you must pass a non NULL dev_id
 *	as this is required when freeing the interrupt.
 *
 *	Flags:
 *
 *	IRQF_SHARED		Interrupt is shared
 *	IRQF_TRIGGER_*		Specify active edge(s) or level
 *
 */
int request_threaded_irq(unsigned int irq, irq_handler_t handler,
			 irq_handler_t thread_fn, unsigned long irqflags,
			 const char *devname, void *dev_id)
{
	struct irqaction *action;
	struct irq_desc *desc;
	int retval;

	if (irq == IRQ_NOTCONNECTED)
		return -ENOTCONN;

	/*
	 * Sanity-check: shared interrupts must pass in a real dev-ID,
	 * otherwise we'll have trouble later trying to figure out
	 * which interrupt is which (messes up the interrupt freeing
	 * logic etc).
	 *
	 * Also shared interrupts do not go well with disabling auto enable.
	 * The sharing interrupt might request it while it's still disabled
	 * and then wait for interrupts forever.
	 *
	 * Also IRQF_COND_SUSPEND only makes sense for shared interrupts and
	 * it cannot be set along with IRQF_NO_SUSPEND.
	 */
	if (((irqflags & IRQF_SHARED) && !dev_id) ||
	    ((irqflags & IRQF_SHARED) && (irqflags & IRQF_NO_AUTOEN)) ||
	    (!(irqflags & IRQF_SHARED) && (irqflags & IRQF_COND_SUSPEND)) ||
	    ((irqflags & IRQF_NO_SUSPEND) && (irqflags & IRQF_COND_SUSPEND)))
		return -EINVAL;

	desc = irq_to_desc(irq);
	if (!desc)
		return -EINVAL;

	if (!irq_settings_can_request(desc) ||
	    WARN_ON(irq_settings_is_per_cpu_devid(desc)))
		return -EINVAL;

	if (!handler) {
		if (!thread_fn)
			return -EINVAL;
		handler = irq_default_primary_handler;
	}

	action = kzalloc(sizeof(struct irqaction), GFP_KERNEL);
	if (!action)
		return -ENOMEM;

	action->handler = handler;
	action->thread_fn = thread_fn;
	action->flags = irqflags;
	action->name = devname;
	action->dev_id = dev_id;

	retval = irq_chip_pm_get(&desc->irq_data);
	if (retval < 0) {
		kfree(action);
		return retval;
	}

	retval = __setup_irq(irq, desc, action);

	if (retval) {
		irq_chip_pm_put(&desc->irq_data);
		kfree(action->secondary);
		kfree(action);
	}

#ifdef CONFIG_DEBUG_SHIRQ_FIXME
	if (!retval && (irqflags & IRQF_SHARED)) {
		/*
		 * It's a shared IRQ -- the driver ought to be prepared for it
		 * to happen immediately, so let's make sure....
		 * We disable the irq to make sure that a 'real' IRQ doesn't
		 * run in parallel with our fake.
		 */
		unsigned long flags;

		disable_irq(irq);
		local_irq_save(flags);

		handler(irq, dev_id);

		local_irq_restore(flags);
		enable_irq(irq);
	}
#endif
	return retval;
}
EXPORT_SYMBOL(request_threaded_irq);

/**
 *	request_any_context_irq - allocate an interrupt line
 *	@irq: Interrupt line to allocate
 *	@handler: Function to be called when the IRQ occurs.
 *		  Threaded handler for threaded interrupts.
 *	@flags: Interrupt type flags
 *	@name: An ascii name for the claiming device
 *	@dev_id: A cookie passed back to the handler function
 *
 *	This call allocates interrupt resources and enables the
 *	interrupt line and IRQ handling. It selects either a
 *	hardirq or threaded handling method depending on the
 *	context.
 *
 *	On failure, it returns a negative value. On success,
 *	it returns either IRQC_IS_HARDIRQ or IRQC_IS_NESTED.
 */
int request_any_context_irq(unsigned int irq, irq_handler_t handler,
			    unsigned long flags, const char *name, void *dev_id)
{
	struct irq_desc *desc;
	int ret;

	if (irq == IRQ_NOTCONNECTED)
		return -ENOTCONN;

	desc = irq_to_desc(irq);
	if (!desc)
		return -EINVAL;

	if (irq_settings_is_nested_thread(desc)) {
		ret = request_threaded_irq(irq, NULL, handler,
					   flags, name, dev_id);
		return !ret ? IRQC_IS_NESTED : ret;
	}

	ret = request_irq(irq, handler, flags, name, dev_id);
	return !ret ? IRQC_IS_HARDIRQ : ret;
}
EXPORT_SYMBOL_GPL(request_any_context_irq);

/**
 *	request_nmi - allocate an interrupt line for NMI delivery
 *	@irq: Interrupt line to allocate
 *	@handler: Function to be called when the IRQ occurs.
 *		  Threaded handler for threaded interrupts.
 *	@irqflags: Interrupt type flags
 *	@name: An ascii name for the claiming device
 *	@dev_id: A cookie passed back to the handler function
 *
 *	This call allocates interrupt resources and enables the
 *	interrupt line and IRQ handling. It sets up the IRQ line
 *	to be handled as an NMI.
 *
 *	An interrupt line delivering NMIs cannot be shared and IRQ handling
 *	cannot be threaded.
 *
 *	Interrupt lines requested for NMI delivering must produce per cpu
 *	interrupts and have auto enabling setting disabled.
 *
 *	Dev_id must be globally unique. Normally the address of the
 *	device data structure is used as the cookie. Since the handler
 *	receives this value it makes sense to use it.
 *
 *	If the interrupt line cannot be used to deliver NMIs, function
 *	will fail and return a negative value.
 */
int request_nmi(unsigned int irq, irq_handler_t handler,
		unsigned long irqflags, const char *name, void *dev_id)
{
	struct irqaction *action;
	struct irq_desc *desc;
	unsigned long flags;
	int retval;

	if (irq == IRQ_NOTCONNECTED)
		return -ENOTCONN;

	/* NMI cannot be shared, used for Polling */
	if (irqflags & (IRQF_SHARED | IRQF_COND_SUSPEND | IRQF_IRQPOLL))
		return -EINVAL;

	if (!(irqflags & IRQF_PERCPU))
		return -EINVAL;

	if (!handler)
		return -EINVAL;

	desc = irq_to_desc(irq);

	if (!desc || (irq_settings_can_autoenable(desc) &&
	    !(irqflags & IRQF_NO_AUTOEN)) ||
	    !irq_settings_can_request(desc) ||
	    WARN_ON(irq_settings_is_per_cpu_devid(desc)) ||
	    !irq_supports_nmi(desc))
		return -EINVAL;

	action = kzalloc(sizeof(struct irqaction), GFP_KERNEL);
	if (!action)
		return -ENOMEM;

	action->handler = handler;
	action->flags = irqflags | IRQF_NO_THREAD | IRQF_NOBALANCING;
	action->name = name;
	action->dev_id = dev_id;

	retval = irq_chip_pm_get(&desc->irq_data);
	if (retval < 0)
		goto err_out;

	retval = __setup_irq(irq, desc, action);
	if (retval)
		goto err_irq_setup;

	raw_spin_lock_irqsave(&desc->lock, flags);

	/* Setup NMI state */
	desc->istate |= IRQS_NMI;
	retval = irq_nmi_setup(desc);
	if (retval) {
		__cleanup_nmi(irq, desc);
		raw_spin_unlock_irqrestore(&desc->lock, flags);
		return -EINVAL;
	}

	raw_spin_unlock_irqrestore(&desc->lock, flags);

	return 0;

err_irq_setup:
	irq_chip_pm_put(&desc->irq_data);
err_out:
	kfree(action);

	return retval;
}

void enable_percpu_irq(unsigned int irq, unsigned int type)
{
	unsigned int cpu = smp_processor_id();
	unsigned long flags;
	struct irq_desc *desc = irq_get_desc_lock(irq, &flags, IRQ_GET_DESC_CHECK_PERCPU);

	if (!desc)
		return;

	/*
	 * If the trigger type is not specified by the caller, then
	 * use the default for this interrupt.
	 */
	type &= IRQ_TYPE_SENSE_MASK;
	if (type == IRQ_TYPE_NONE)
		type = irqd_get_trigger_type(&desc->irq_data);

	if (type != IRQ_TYPE_NONE) {
		int ret;

		ret = __irq_set_trigger(desc, type);

		if (ret) {
			WARN(1, "failed to set type for IRQ%d\n", irq);
			goto out;
		}
	}

	irq_percpu_enable(desc, cpu);
out:
	irq_put_desc_unlock(desc, flags);
}
EXPORT_SYMBOL_GPL(enable_percpu_irq);

void enable_percpu_nmi(unsigned int irq, unsigned int type)
{
	enable_percpu_irq(irq, type);
}

/**
 * irq_percpu_is_enabled - Check whether the per cpu irq is enabled
 * @irq:	Linux irq number to check for
 *
 * Must be called from a non migratable context. Returns the enable
 * state of a per cpu interrupt on the current cpu.
 */
bool irq_percpu_is_enabled(unsigned int irq)
{
	unsigned int cpu = smp_processor_id();
	struct irq_desc *desc;
	unsigned long flags;
	bool is_enabled;

	desc = irq_get_desc_lock(irq, &flags, IRQ_GET_DESC_CHECK_PERCPU);
	if (!desc)
		return false;

	is_enabled = cpumask_test_cpu(cpu, desc->percpu_enabled);
	irq_put_desc_unlock(desc, flags);

	return is_enabled;
}
EXPORT_SYMBOL_GPL(irq_percpu_is_enabled);

void disable_percpu_irq(unsigned int irq)
{
	unsigned int cpu = smp_processor_id();
	unsigned long flags;
	struct irq_desc *desc = irq_get_desc_lock(irq, &flags, IRQ_GET_DESC_CHECK_PERCPU);

	if (!desc)
		return;

	irq_percpu_disable(desc, cpu);
	irq_put_desc_unlock(desc, flags);
}
EXPORT_SYMBOL_GPL(disable_percpu_irq);

void disable_percpu_nmi(unsigned int irq)
{
	disable_percpu_irq(irq);
}

/*
 * Internal function to unregister a percpu irqaction.
 */
static struct irqaction *__free_percpu_irq(unsigned int irq, void __percpu *dev_id)
{
	struct irq_desc *desc = irq_to_desc(irq);
	struct irqaction *action;
	unsigned long flags;

	WARN(in_interrupt(), "Trying to free IRQ %d from IRQ context!\n", irq);

	if (!desc)
		return NULL;

	raw_spin_lock_irqsave(&desc->lock, flags);

	action = desc->action;
	if (!action || action->percpu_dev_id != dev_id) {
		WARN(1, "Trying to free already-free IRQ %d\n", irq);
		goto bad;
	}

	if (!cpumask_empty(desc->percpu_enabled)) {
		WARN(1, "percpu IRQ %d still enabled on CPU%d!\n",
		     irq, cpumask_first(desc->percpu_enabled));
		goto bad;
	}

	/* Found it - now remove it from the list of entries: */
	desc->action = NULL;

	desc->istate &= ~IRQS_NMI;

	raw_spin_unlock_irqrestore(&desc->lock, flags);

	unregister_handler_proc(irq, action);

	irq_chip_pm_put(&desc->irq_data);
	module_put(desc->owner);
	return action;

bad:
	raw_spin_unlock_irqrestore(&desc->lock, flags);
	return NULL;
}

/**
 *	remove_percpu_irq - free a per-cpu interrupt
 *	@irq: Interrupt line to free
 *	@act: irqaction for the interrupt
 *
 * Used to remove interrupts statically setup by the early boot process.
 */
void remove_percpu_irq(unsigned int irq, struct irqaction *act)
{
	struct irq_desc *desc = irq_to_desc(irq);

	if (desc && irq_settings_is_per_cpu_devid(desc))
	    __free_percpu_irq(irq, act->percpu_dev_id);
}

/**
 *	free_percpu_irq - free an interrupt allocated with request_percpu_irq
 *	@irq: Interrupt line to free
 *	@dev_id: Device identity to free
 *
 *	Remove a percpu interrupt handler. The handler is removed, but
 *	the interrupt line is not disabled. This must be done on each
 *	CPU before calling this function. The function does not return
 *	until any executing interrupts for this IRQ have completed.
 *
 *	This function must not be called from interrupt context.
 */
void free_percpu_irq(unsigned int irq, void __percpu *dev_id)
{
	struct irq_desc *desc = irq_to_desc(irq);

	if (!desc || !irq_settings_is_per_cpu_devid(desc))
		return;

	chip_bus_lock(desc);
	kfree(__free_percpu_irq(irq, dev_id));
	chip_bus_sync_unlock(desc);
}
EXPORT_SYMBOL_GPL(free_percpu_irq);

void free_percpu_nmi(unsigned int irq, void __percpu *dev_id)
{
	struct irq_desc *desc = irq_to_desc(irq);

	if (!desc || !irq_settings_is_per_cpu_devid(desc))
		return;

	if (WARN_ON(!(desc->istate & IRQS_NMI)))
		return;

	kfree(__free_percpu_irq(irq, dev_id));
}

/**
 *	setup_percpu_irq - setup a per-cpu interrupt
 *	@irq: Interrupt line to setup
 *	@act: irqaction for the interrupt
 *
 * Used to statically setup per-cpu interrupts in the early boot process.
 */
int setup_percpu_irq(unsigned int irq, struct irqaction *act)
{
	struct irq_desc *desc = irq_to_desc(irq);
	int retval;

	if (!desc || !irq_settings_is_per_cpu_devid(desc))
		return -EINVAL;

	retval = irq_chip_pm_get(&desc->irq_data);
	if (retval < 0)
		return retval;

	retval = __setup_irq(irq, desc, act);

	if (retval)
		irq_chip_pm_put(&desc->irq_data);

	return retval;
}

/**
 *	__request_percpu_irq - allocate a percpu interrupt line
 *	@irq: Interrupt line to allocate
 *	@handler: Function to be called when the IRQ occurs.
 *	@flags: Interrupt type flags (IRQF_TIMER only)
 *	@devname: An ascii name for the claiming device
 *	@dev_id: A percpu cookie passed back to the handler function
 *
 *	This call allocates interrupt resources and enables the
 *	interrupt on the local CPU. If the interrupt is supposed to be
 *	enabled on other CPUs, it has to be done on each CPU using
 *	enable_percpu_irq().
 *
 *	Dev_id must be globally unique. It is a per-cpu variable, and
 *	the handler gets called with the interrupted CPU's instance of
 *	that variable.
 */
int __request_percpu_irq(unsigned int irq, irq_handler_t handler,
			 unsigned long flags, const char *devname,
			 void __percpu *dev_id)
{
	struct irqaction *action;
	struct irq_desc *desc;
	int retval;

	if (!dev_id)
		return -EINVAL;

	desc = irq_to_desc(irq);
	if (!desc || !irq_settings_can_request(desc) ||
	    !irq_settings_is_per_cpu_devid(desc))
		return -EINVAL;

	if (flags && flags != IRQF_TIMER)
		return -EINVAL;

	action = kzalloc(sizeof(struct irqaction), GFP_KERNEL);
	if (!action)
		return -ENOMEM;

	action->handler = handler;
	action->flags = flags | IRQF_PERCPU | IRQF_NO_SUSPEND;
	action->name = devname;
	action->percpu_dev_id = dev_id;

	retval = irq_chip_pm_get(&desc->irq_data);
	if (retval < 0) {
		kfree(action);
		return retval;
	}

	retval = __setup_irq(irq, desc, action);

	if (retval) {
		irq_chip_pm_put(&desc->irq_data);
		kfree(action);
	}

	return retval;
}
EXPORT_SYMBOL_GPL(__request_percpu_irq);

/**
 *	request_percpu_nmi - allocate a percpu interrupt line for NMI delivery
 *	@irq: Interrupt line to allocate
 *	@handler: Function to be called when the IRQ occurs.
 *	@name: An ascii name for the claiming device
 *	@dev_id: A percpu cookie passed back to the handler function
 *
 *	This call allocates interrupt resources for a per CPU NMI. Per CPU NMIs
 *	have to be setup on each CPU by calling prepare_percpu_nmi() before
 *	being enabled on the same CPU by using enable_percpu_nmi().
 *
 *	Dev_id must be globally unique. It is a per-cpu variable, and
 *	the handler gets called with the interrupted CPU's instance of
 *	that variable.
 *
 *	Interrupt lines requested for NMI delivering should have auto enabling
 *	setting disabled.
 *
 *	If the interrupt line cannot be used to deliver NMIs, function
 *	will fail returning a negative value.
 */
int request_percpu_nmi(unsigned int irq, irq_handler_t handler,
		       const char *name, void __percpu *dev_id)
{
	struct irqaction *action;
	struct irq_desc *desc;
	unsigned long flags;
	int retval;

	if (!handler)
		return -EINVAL;

	desc = irq_to_desc(irq);

	if (!desc || !irq_settings_can_request(desc) ||
	    !irq_settings_is_per_cpu_devid(desc) ||
	    irq_settings_can_autoenable(desc) ||
	    !irq_supports_nmi(desc))
		return -EINVAL;

	/* The line cannot already be NMI */
	if (desc->istate & IRQS_NMI)
		return -EINVAL;

	action = kzalloc(sizeof(struct irqaction), GFP_KERNEL);
	if (!action)
		return -ENOMEM;

	action->handler = handler;
	action->flags = IRQF_PERCPU | IRQF_NO_SUSPEND | IRQF_NO_THREAD
		| IRQF_NOBALANCING;
	action->name = name;
	action->percpu_dev_id = dev_id;

	retval = irq_chip_pm_get(&desc->irq_data);
	if (retval < 0)
		goto err_out;

	retval = __setup_irq(irq, desc, action);
	if (retval)
		goto err_irq_setup;

	raw_spin_lock_irqsave(&desc->lock, flags);
	desc->istate |= IRQS_NMI;
	raw_spin_unlock_irqrestore(&desc->lock, flags);

	return 0;

err_irq_setup:
	irq_chip_pm_put(&desc->irq_data);
err_out:
	kfree(action);

	return retval;
}

/**
 *	prepare_percpu_nmi - performs CPU local setup for NMI delivery
 *	@irq: Interrupt line to prepare for NMI delivery
 *
 *	This call prepares an interrupt line to deliver NMI on the current CPU,
 *	before that interrupt line gets enabled with enable_percpu_nmi().
 *
 *	As a CPU local operation, this should be called from non-preemptible
 *	context.
 *
 *	If the interrupt line cannot be used to deliver NMIs, function
 *	will fail returning a negative value.
 */
int prepare_percpu_nmi(unsigned int irq)
{
	unsigned long flags;
	struct irq_desc *desc;
	int ret = 0;

	WARN_ON(preemptible());

	desc = irq_get_desc_lock(irq, &flags,
				 IRQ_GET_DESC_CHECK_PERCPU);
	if (!desc)
		return -EINVAL;

	if (WARN(!(desc->istate & IRQS_NMI),
		 KERN_ERR "prepare_percpu_nmi called for a non-NMI interrupt: irq %u\n",
		 irq)) {
		ret = -EINVAL;
		goto out;
	}

	ret = irq_nmi_setup(desc);
	if (ret) {
		pr_err("Failed to setup NMI delivery: irq %u\n", irq);
		goto out;
	}

out:
	irq_put_desc_unlock(desc, flags);
	return ret;
}

/**
 *	teardown_percpu_nmi - undoes NMI setup of IRQ line
 *	@irq: Interrupt line from which CPU local NMI configuration should be
 *	      removed
 *
 *	This call undoes the setup done by prepare_percpu_nmi().
 *
 *	IRQ line should not be enabled for the current CPU.
 *
 *	As a CPU local operation, this should be called from non-preemptible
 *	context.
 */
void teardown_percpu_nmi(unsigned int irq)
{
	unsigned long flags;
	struct irq_desc *desc;

	WARN_ON(preemptible());

	desc = irq_get_desc_lock(irq, &flags,
				 IRQ_GET_DESC_CHECK_PERCPU);
	if (!desc)
		return;

	if (WARN_ON(!(desc->istate & IRQS_NMI)))
		goto out;

	irq_nmi_teardown(desc);
out:
	irq_put_desc_unlock(desc, flags);
}

int __irq_get_irqchip_state(struct irq_data *data, enum irqchip_irq_state which,
			    bool *state)
{
	struct irq_chip *chip;
	int err = -EINVAL;

	do {
		chip = irq_data_get_irq_chip(data);
		if (WARN_ON_ONCE(!chip))
			return -ENODEV;
		if (chip->irq_get_irqchip_state)
			break;
#ifdef CONFIG_IRQ_DOMAIN_HIERARCHY
		data = data->parent_data;
#else
		data = NULL;
#endif
	} while (data);

	if (data)
		err = chip->irq_get_irqchip_state(data, which, state);
	return err;
}

/**
 *	irq_get_irqchip_state - returns the irqchip state of a interrupt.
 *	@irq: Interrupt line that is forwarded to a VM
 *	@which: One of IRQCHIP_STATE_* the caller wants to know about
 *	@state: a pointer to a boolean where the state is to be stored
 *
 *	This call snapshots the internal irqchip state of an
 *	interrupt, returning into @state the bit corresponding to
 *	stage @which
 *
 *	This function should be called with preemption disabled if the
 *	interrupt controller has per-cpu registers.
 */
int irq_get_irqchip_state(unsigned int irq, enum irqchip_irq_state which,
			  bool *state)
{
	struct irq_desc *desc;
	struct irq_data *data;
	unsigned long flags;
	int err = -EINVAL;

	desc = irq_get_desc_buslock(irq, &flags, 0);
	if (!desc)
		return err;

	data = irq_desc_get_irq_data(desc);

	err = __irq_get_irqchip_state(data, which, state);

	irq_put_desc_busunlock(desc, flags);
	return err;
}
EXPORT_SYMBOL_GPL(irq_get_irqchip_state);

/**
 *	irq_set_irqchip_state - set the state of a forwarded interrupt.
 *	@irq: Interrupt line that is forwarded to a VM
 *	@which: State to be restored (one of IRQCHIP_STATE_*)
 *	@val: Value corresponding to @which
 *
 *	This call sets the internal irqchip state of an interrupt,
 *	depending on the value of @which.
 *
 *	This function should be called with preemption disabled if the
 *	interrupt controller has per-cpu registers.
 */
int irq_set_irqchip_state(unsigned int irq, enum irqchip_irq_state which,
			  bool val)
{
	struct irq_desc *desc;
	struct irq_data *data;
	struct irq_chip *chip;
	unsigned long flags;
	int err = -EINVAL;

	desc = irq_get_desc_buslock(irq, &flags, 0);
	if (!desc)
		return err;

	data = irq_desc_get_irq_data(desc);

	do {
		chip = irq_data_get_irq_chip(data);
		if (WARN_ON_ONCE(!chip)) {
			err = -ENODEV;
			goto out_unlock;
		}
		if (chip->irq_set_irqchip_state)
			break;
#ifdef CONFIG_IRQ_DOMAIN_HIERARCHY
		data = data->parent_data;
#else
		data = NULL;
#endif
	} while (data);

	if (data)
		err = chip->irq_set_irqchip_state(data, which, val);

out_unlock:
	irq_put_desc_busunlock(desc, flags);
	return err;
}
EXPORT_SYMBOL_GPL(irq_set_irqchip_state);

/**
 * irq_has_action - Check whether an interrupt is requested
 * @irq:	The linux irq number
 *
 * Returns: A snapshot of the current state
 */
bool irq_has_action(unsigned int irq)
{
	bool res;

	rcu_read_lock();
	res = irq_desc_has_action(irq_to_desc(irq));
	rcu_read_unlock();
	return res;
}
EXPORT_SYMBOL_GPL(irq_has_action);

/**
 * irq_check_status_bit - Check whether bits in the irq descriptor status are set
 * @irq:	The linux irq number
 * @bitmask:	The bitmask to evaluate
 *
 * Returns: True if one of the bits in @bitmask is set
 */
bool irq_check_status_bit(unsigned int irq, unsigned int bitmask)
{
	struct irq_desc *desc;
	bool res = false;

	rcu_read_lock();
	desc = irq_to_desc(irq);
	if (desc)
		res = !!(desc->status_use_accessors & bitmask);
	rcu_read_unlock();
	return res;
}
EXPORT_SYMBOL_GPL(irq_check_status_bit);
