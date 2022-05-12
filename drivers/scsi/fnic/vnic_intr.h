/*
 * Copyright 2008 Cisco Systems, Inc.  All rights reserved.
 * Copyright 2007 Nuova Systems, Inc.  All rights reserved.
 *
 * This program is free software; you may redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef _VNIC_INTR_H_
#define _VNIC_INTR_H_

#include <linux/pci.h>
#include "vnic_dev.h"

/*
 * These defines avoid symbol clash between fnic and enic (Cisco 10G Eth
 * Driver) when both are built with CONFIG options =y
 */
#define vnic_intr_unmask fnic_intr_unmask
#define vnic_intr_mask fnic_intr_mask
#define vnic_intr_return_credits fnic_intr_return_credits
#define vnic_intr_credits fnic_intr_credits
#define vnic_intr_return_all_credits fnic_intr_return_all_credits
#define vnic_intr_legacy_pba fnic_intr_legacy_pba
#define vnic_intr_free fnic_intr_free
#define vnic_intr_alloc fnic_intr_alloc
#define vnic_intr_init fnic_intr_init
#define vnic_intr_clean fnic_intr_clean

#define VNIC_INTR_TIMER_MAX		0xffff

#define VNIC_INTR_TIMER_TYPE_ABS	0
#define VNIC_INTR_TIMER_TYPE_QUIET	1

/* Interrupt control */
struct vnic_intr_ctrl {
	u32 coalescing_timer;		/* 0x00 */
	u32 pad0;
	u32 coalescing_value;		/* 0x08 */
	u32 pad1;
	u32 coalescing_type;		/* 0x10 */
	u32 pad2;
	u32 mask_on_assertion;		/* 0x18 */
	u32 pad3;
	u32 mask;			/* 0x20 */
	u32 pad4;
	u32 int_credits;		/* 0x28 */
	u32 pad5;
	u32 int_credit_return;		/* 0x30 */
	u32 pad6;
};

struct vnic_intr {
	unsigned int index;
	struct vnic_dev *vdev;
	struct vnic_intr_ctrl __iomem *ctrl;	/* memory-mapped */
};

static inline void vnic_intr_unmask(struct vnic_intr *intr)
{
	iowrite32(0, &intr->ctrl->mask);
}

static inline void vnic_intr_mask(struct vnic_intr *intr)
{
	iowrite32(1, &intr->ctrl->mask);
}

static inline void vnic_intr_return_credits(struct vnic_intr *intr,
	unsigned int credits, int unmask, int reset_timer)
{
#define VNIC_INTR_UNMASK_SHIFT		16
#define VNIC_INTR_RESET_TIMER_SHIFT	17

	u32 int_credit_return = (credits & 0xffff) |
		(unmask ? (1 << VNIC_INTR_UNMASK_SHIFT) : 0) |
		(reset_timer ? (1 << VNIC_INTR_RESET_TIMER_SHIFT) : 0);

	iowrite32(int_credit_return, &intr->ctrl->int_credit_return);
}

static inline unsigned int vnic_intr_credits(struct vnic_intr *intr)
{
	return ioread32(&intr->ctrl->int_credits);
}

static inline void vnic_intr_return_all_credits(struct vnic_intr *intr)
{
	unsigned int credits = vnic_intr_credits(intr);
	int unmask = 1;
	int reset_timer = 1;

	vnic_intr_return_credits(intr, credits, unmask, reset_timer);
}

static inline u32 vnic_intr_legacy_pba(u32 __iomem *legacy_pba)
{
	/* read PBA without clearing */
	return ioread32(legacy_pba);
}

void vnic_intr_free(struct vnic_intr *intr);
int vnic_intr_alloc(struct vnic_dev *vdev, struct vnic_intr *intr,
	unsigned int index);
void vnic_intr_init(struct vnic_intr *intr, unsigned int coalescing_timer,
	unsigned int coalescing_type, unsigned int mask_on_assertion);
void vnic_intr_clean(struct vnic_intr *intr);

#endif /* _VNIC_INTR_H_ */
