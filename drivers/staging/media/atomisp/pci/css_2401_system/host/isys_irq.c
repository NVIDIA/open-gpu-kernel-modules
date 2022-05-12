// SPDX-License-Identifier: GPL-2.0
/*
 * Support for Intel Camera Imaging ISP subsystem.
 * Copyright (c) 2015, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

#include <system_local.h>
#include "device_access.h"
#include "assert_support.h"
#include "ia_css_debug.h"
#include "isys_irq.h"

#ifndef __INLINE_ISYS2401_IRQ__
/*
 * Include definitions for isys irq private functions. isys_irq.h includes
 * declarations of these functions by including isys_irq_public.h.
 */
#include "isys_irq_private.h"
#endif

/* Public interface */
void isys_irqc_status_enable(const isys_irq_ID_t	isys_irqc_id)
{
	assert(isys_irqc_id < N_ISYS_IRQ_ID);

	ia_css_debug_dtrace(IA_CSS_DEBUG_TRACE, "Setting irq mask for port %u\n",
			    isys_irqc_id);
	isys_irqc_reg_store(isys_irqc_id, ISYS_IRQ_MASK_REG_IDX,
			    ISYS_IRQ_MASK_REG_VALUE);
	isys_irqc_reg_store(isys_irqc_id, ISYS_IRQ_CLEAR_REG_IDX,
			    ISYS_IRQ_CLEAR_REG_VALUE);
	isys_irqc_reg_store(isys_irqc_id, ISYS_IRQ_ENABLE_REG_IDX,
			    ISYS_IRQ_ENABLE_REG_VALUE);
}
