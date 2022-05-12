// SPDX-License-Identifier: GPL-2.0-only
/*
 *  PS3 SMP routines.
 *
 *  Copyright (C) 2006 Sony Computer Entertainment Inc.
 *  Copyright 2006 Sony Corp.
 */

#include <linux/kernel.h>
#include <linux/smp.h>

#include <asm/machdep.h>
#include <asm/udbg.h>

#include "platform.h"

#if defined(DEBUG)
#define DBG udbg_printf
#else
#define DBG pr_debug
#endif

/**
  * ps3_ipi_virqs - a per cpu array of virqs for ipi use
  */

#define MSG_COUNT 4
static DEFINE_PER_CPU(unsigned int [MSG_COUNT], ps3_ipi_virqs);

static void ps3_smp_message_pass(int cpu, int msg)
{
	int result;
	unsigned int virq;

	if (msg >= MSG_COUNT) {
		DBG("%s:%d: bad msg: %d\n", __func__, __LINE__, msg);
		return;
	}

	virq = per_cpu(ps3_ipi_virqs, cpu)[msg];
	result = ps3_send_event_locally(virq);

	if (result)
		DBG("%s:%d: ps3_send_event_locally(%d, %d) failed"
			" (%d)\n", __func__, __LINE__, cpu, msg, result);
}

static void __init ps3_smp_probe(void)
{
	int cpu;

	for (cpu = 0; cpu < 2; cpu++) {
		int result;
		unsigned int *virqs = per_cpu(ps3_ipi_virqs, cpu);
		int i;

		DBG(" -> %s:%d: (%d)\n", __func__, __LINE__, cpu);

		/*
		* Check assumptions on ps3_ipi_virqs[] indexing. If this
		* check fails, then a different mapping of PPC_MSG_
		* to index needs to be setup.
		*/

		BUILD_BUG_ON(PPC_MSG_CALL_FUNCTION    != 0);
		BUILD_BUG_ON(PPC_MSG_RESCHEDULE       != 1);
		BUILD_BUG_ON(PPC_MSG_TICK_BROADCAST   != 2);
		BUILD_BUG_ON(PPC_MSG_NMI_IPI          != 3);

		for (i = 0; i < MSG_COUNT; i++) {
			result = ps3_event_receive_port_setup(cpu, &virqs[i]);

			if (result)
				continue;

			DBG("%s:%d: (%d, %d) => virq %u\n",
				__func__, __LINE__, cpu, i, virqs[i]);

			result = smp_request_message_ipi(virqs[i], i);

			if (result)
				virqs[i] = 0;
			else
				ps3_register_ipi_irq(cpu, virqs[i]);
		}

		ps3_register_ipi_debug_brk(cpu, virqs[PPC_MSG_NMI_IPI]);

		DBG(" <- %s:%d: (%d)\n", __func__, __LINE__, cpu);
	}
}

void ps3_smp_cleanup_cpu(int cpu)
{
	unsigned int *virqs = per_cpu(ps3_ipi_virqs, cpu);
	int i;

	DBG(" -> %s:%d: (%d)\n", __func__, __LINE__, cpu);

	for (i = 0; i < MSG_COUNT; i++) {
		/* Can't call free_irq from interrupt context. */
		ps3_event_receive_port_destroy(virqs[i]);
		virqs[i] = 0;
	}

	DBG(" <- %s:%d: (%d)\n", __func__, __LINE__, cpu);
}

static struct smp_ops_t ps3_smp_ops = {
	.probe		= ps3_smp_probe,
	.message_pass	= ps3_smp_message_pass,
	.kick_cpu	= smp_generic_kick_cpu,
};

void smp_init_ps3(void)
{
	DBG(" -> %s\n", __func__);
	smp_ops = &ps3_smp_ops;
	DBG(" <- %s\n", __func__);
}
