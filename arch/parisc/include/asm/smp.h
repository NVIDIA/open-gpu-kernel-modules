/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_SMP_H
#define __ASM_SMP_H

extern int init_per_cpu(int cpuid);

#if defined(CONFIG_SMP)

/* Page Zero Location PDC will look for the address to branch to when we poke
** slave CPUs still in "Icache loop".
*/
#define PDC_OS_BOOT_RENDEZVOUS     0x10
#define PDC_OS_BOOT_RENDEZVOUS_HI  0x28

#ifndef ASSEMBLY
#include <linux/bitops.h>
#include <linux/threads.h>	/* for NR_CPUS */
#include <linux/cpumask.h>
typedef unsigned long address_t;


/*
 *	Private routines/data
 *
 *	physical and logical are equivalent until we support CPU hotplug.
 */
#define cpu_number_map(cpu)	(cpu)
#define cpu_logical_map(cpu)	(cpu)

extern void smp_send_all_nop(void);

extern void arch_send_call_function_single_ipi(int cpu);
extern void arch_send_call_function_ipi_mask(const struct cpumask *mask);

#endif /* !ASSEMBLY */

#define raw_smp_processor_id()	(current_thread_info()->cpu)

#else /* CONFIG_SMP */

static inline void smp_send_all_nop(void) { return; }

#endif

#define NO_PROC_ID		0xFF		/* No processor magic marker */
#define ANY_PROC_ID		0xFF		/* Any processor magic marker */
static inline int __cpu_disable (void) {
  return 0;
}
static inline void __cpu_die (unsigned int cpu) {
  while(1)
    ;
}

#endif /*  __ASM_SMP_H */
