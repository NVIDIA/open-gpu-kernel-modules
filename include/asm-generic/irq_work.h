/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_IRQ_WORK_H
#define __ASM_IRQ_WORK_H

static inline bool arch_irq_work_has_interrupt(void)
{
	return false;
}

#endif /* __ASM_IRQ_WORK_H */

