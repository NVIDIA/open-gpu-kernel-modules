// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright IBM Corp. 2001,2008
 *
 * This file contains the IRQ specific code for hvc_console
 *
 */

#include <linux/interrupt.h>

#include "hvc_console.h"

static irqreturn_t hvc_handle_interrupt(int irq, void *dev_instance)
{
	/* if hvc_poll request a repoll, then kick the hvcd thread */
	if (hvc_poll(dev_instance))
		hvc_kick();

	/*
	 * We're safe to always return IRQ_HANDLED as the hvcd thread will
	 * iterate through each hvc_struct.
	 */
	return IRQ_HANDLED;
}

/*
 * For IRQ based systems these callbacks can be used
 */
int notifier_add_irq(struct hvc_struct *hp, int irq)
{
	int rc;

	if (!irq) {
		hp->irq_requested = 0;
		return 0;
	}
	rc = request_irq(irq, hvc_handle_interrupt, hp->flags,
			"hvc_console", hp);
	if (!rc)
		hp->irq_requested = 1;
	return rc;
}

void notifier_del_irq(struct hvc_struct *hp, int irq)
{
	if (!hp->irq_requested)
		return;
	free_irq(irq, hp);
	hp->irq_requested = 0;
}

void notifier_hangup_irq(struct hvc_struct *hp, int irq)
{
	notifier_del_irq(hp, irq);
}
