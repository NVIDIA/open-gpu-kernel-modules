// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2007 Lemote Inc. & Institute of Computing Technology
 * Author: Fuxin Zhang, zhangfx@lemote.com
 */
#include <linux/interrupt.h>

#include <asm/irq_cpu.h>
#include <asm/i8259.h>

#include <loongson.h>

static void i8259_irqdispatch(void)
{
	int irq;

	irq = i8259_irq();
	if (irq >= 0)
		do_IRQ(irq);
	else
		spurious_interrupt();
}

asmlinkage void mach_irq_dispatch(unsigned int pending)
{
	if (pending & CAUSEF_IP7)
		do_IRQ(MIPS_CPU_IRQ_BASE + 7);
	else if (pending & CAUSEF_IP6) /* perf counter loverflow */
		return;
	else if (pending & CAUSEF_IP5)
		i8259_irqdispatch();
	else if (pending & CAUSEF_IP2)
		bonito_irqdispatch();
	else
		spurious_interrupt();
}

void __init mach_init_irq(void)
{
	int irq;

	/* init all controller
	 *   0-15	  ------> i8259 interrupt
	 *   16-23	  ------> mips cpu interrupt
	 *   32-63	  ------> bonito irq
	 */

	/* most bonito irq should be level triggered */
	LOONGSON_INTEDGE = LOONGSON_ICU_SYSTEMERR | LOONGSON_ICU_MASTERERR |
	    LOONGSON_ICU_RETRYERR | LOONGSON_ICU_MBOXES;

	/* Sets the first-level interrupt dispatcher. */
	mips_cpu_irq_init();
	init_i8259_irqs();
	bonito_irq_init();

	/* bonito irq at IP2 */
	irq = MIPS_CPU_IRQ_BASE + 2;
	if (request_irq(irq, no_action, IRQF_NO_THREAD, "cascade", NULL))
		pr_err("Failed to request irq %d (cascade)\n", irq);
	/* 8259 irq at IP5 */
	irq = MIPS_CPU_IRQ_BASE + 5;
	if (request_irq(irq, no_action, IRQF_NO_THREAD, "cascade", NULL))
		pr_err("Failed to request irq %d (cascade)\n", irq);
}
