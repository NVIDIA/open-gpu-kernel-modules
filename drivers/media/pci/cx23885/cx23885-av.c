// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Conexant CX23885/7/8 PCIe bridge
 *
 *  AV device support routines - non-input, non-vl42_subdev routines
 *
 *  Copyright (C) 2010  Andy Walls <awalls@md.metrocast.net>
 */

#include "cx23885.h"
#include "cx23885-av.h"
#include "cx23885-video.h"

void cx23885_av_work_handler(struct work_struct *work)
{
	struct cx23885_dev *dev =
			   container_of(work, struct cx23885_dev, cx25840_work);
	bool handled = false;

	v4l2_subdev_call(dev->sd_cx25840, core, interrupt_service_routine,
			 PCI_MSK_AV_CORE, &handled);

	/* Getting here with the interrupt not handled
	   then probbaly flatiron does have pending interrupts.
	*/
	if (!handled) {
		/* clear left and right adc channel interrupt request flag */
		cx23885_flatiron_write(dev, 0x1f,
			cx23885_flatiron_read(dev, 0x1f) | 0x80);
		cx23885_flatiron_write(dev, 0x23,
			cx23885_flatiron_read(dev, 0x23) | 0x80);
	}

	cx23885_irq_enable(dev, PCI_MSK_AV_CORE);
}
