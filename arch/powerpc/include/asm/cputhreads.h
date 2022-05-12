/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_POWERPC_CPUTHREADS_H
#define _ASM_POWERPC_CPUTHREADS_H

#ifndef __ASSEMBLY__
#include <linux/cpumask.h>
#include <asm/cpu_has_feature.h>

/*
 * Mapping of threads to cores
 *
 * Note: This implementation is limited to a power of 2 number of
 * threads per core and the same number for each core in the system
 * (though it would work if some processors had less threads as long
 * as the CPU numbers are still allocated, just not brought online).
 *
 * However, the API allows for a different implementation in the future
 * if needed, as long as you only use the functions and not the variables
 * directly.
 */

#ifdef CONFIG_SMP
extern int threads_per_core;
extern int threads_per_subcore;
extern int threads_shift;
extern cpumask_t threads_core_mask;
#else
#define threads_per_core	1
#define threads_per_subcore	1
#define threads_shift		0
#define has_big_cores		0
#define threads_core_mask	(*get_cpu_mask(0))
#endif

/* cpu_thread_mask_to_cores - Return a cpumask of one per cores
 *                            hit by the argument
 *
 * @threads:	a cpumask of online threads
 *
 * This function returns a cpumask which will have one online cpu's
 * bit set for each core that has at least one thread set in the argument.
 *
 * This can typically be used for things like IPI for tlb invalidations
 * since those need to be done only once per core/TLB
 */
static inline cpumask_t cpu_thread_mask_to_cores(const struct cpumask *threads)
{
	cpumask_t	tmp, res;
	int		i, cpu;

	cpumask_clear(&res);
	for (i = 0; i < NR_CPUS; i += threads_per_core) {
		cpumask_shift_left(&tmp, &threads_core_mask, i);
		if (cpumask_intersects(threads, &tmp)) {
			cpu = cpumask_next_and(-1, &tmp, cpu_online_mask);
			if (cpu < nr_cpu_ids)
				cpumask_set_cpu(cpu, &res);
		}
	}
	return res;
}

static inline int cpu_nr_cores(void)
{
	return nr_cpu_ids >> threads_shift;
}

static inline cpumask_t cpu_online_cores_map(void)
{
	return cpu_thread_mask_to_cores(cpu_online_mask);
}

#ifdef CONFIG_SMP
int cpu_core_index_of_thread(int cpu);
int cpu_first_thread_of_core(int core);
#else
static inline int cpu_core_index_of_thread(int cpu) { return cpu; }
static inline int cpu_first_thread_of_core(int core) { return core; }
#endif

static inline int cpu_thread_in_core(int cpu)
{
	return cpu & (threads_per_core - 1);
}

static inline int cpu_thread_in_subcore(int cpu)
{
	return cpu & (threads_per_subcore - 1);
}

static inline int cpu_first_thread_sibling(int cpu)
{
	return cpu & ~(threads_per_core - 1);
}

static inline int cpu_last_thread_sibling(int cpu)
{
	return cpu | (threads_per_core - 1);
}

static inline u32 get_tensr(void)
{
#ifdef	CONFIG_BOOKE
	if (cpu_has_feature(CPU_FTR_SMT))
		return mfspr(SPRN_TENSR);
#endif
	return 1;
}

void book3e_start_thread(int thread, unsigned long addr);
void book3e_stop_thread(int thread);

#endif /* __ASSEMBLY__ */

#define INVALID_THREAD_HWID	0x0fff

#endif /* _ASM_POWERPC_CPUTHREADS_H */

