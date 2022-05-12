// SPDX-License-Identifier: GPL-2.0
#include <linux/ftrace.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/mm_types.h>
#include <linux/pgtable.h>

#include <asm/bugs.h>
#include <asm/cacheflush.h>
#include <asm/idmap.h>
#include <asm/memory.h>
#include <asm/smp_plat.h>
#include <asm/suspend.h>
#include <asm/tlbflush.h>

extern int __cpu_suspend(unsigned long, int (*)(unsigned long), u32 cpuid);
extern void cpu_resume_mmu(void);

#ifdef CONFIG_MMU
int cpu_suspend(unsigned long arg, int (*fn)(unsigned long))
{
	struct mm_struct *mm = current->active_mm;
	u32 __mpidr = cpu_logical_map(smp_processor_id());
	int ret;

	if (!idmap_pgd)
		return -EINVAL;

	/*
	 * Function graph tracer state gets incosistent when the kernel
	 * calls functions that never return (aka suspend finishers) hence
	 * disable graph tracing during their execution.
	 */
	pause_graph_tracing();

	/*
	 * Provide a temporary page table with an identity mapping for
	 * the MMU-enable code, required for resuming.  On successful
	 * resume (indicated by a zero return code), we need to switch
	 * back to the correct page tables.
	 */
	ret = __cpu_suspend(arg, fn, __mpidr);

	unpause_graph_tracing();

	if (ret == 0) {
		cpu_switch_mm(mm->pgd, mm);
		local_flush_bp_all();
		local_flush_tlb_all();
		check_other_bugs();
	}

	return ret;
}
#else
int cpu_suspend(unsigned long arg, int (*fn)(unsigned long))
{
	u32 __mpidr = cpu_logical_map(smp_processor_id());
	int ret;

	pause_graph_tracing();
	ret = __cpu_suspend(arg, fn, __mpidr);
	unpause_graph_tracing();

	return ret;
}
#define	idmap_pgd	NULL
#endif

/*
 * This is called by __cpu_suspend() to save the state, and do whatever
 * flushing is required to ensure that when the CPU goes to sleep we have
 * the necessary data available when the caches are not searched.
 */
void __cpu_suspend_save(u32 *ptr, u32 ptrsz, u32 sp, u32 *save_ptr)
{
	u32 *ctx = ptr;

	*save_ptr = virt_to_phys(ptr);

	/* This must correspond to the LDM in cpu_resume() assembly */
	*ptr++ = virt_to_phys(idmap_pgd);
	*ptr++ = sp;
	*ptr++ = virt_to_phys(cpu_do_resume);

	cpu_do_suspend(ptr);

	flush_cache_louis();

	/*
	 * flush_cache_louis does not guarantee that
	 * save_ptr and ptr are cleaned to main memory,
	 * just up to the Level of Unification Inner Shareable.
	 * Since the context pointer and context itself
	 * are to be retrieved with the MMU off that
	 * data must be cleaned from all cache levels
	 * to main memory using "area" cache primitives.
	*/
	__cpuc_flush_dcache_area(ctx, ptrsz);
	__cpuc_flush_dcache_area(save_ptr, sizeof(*save_ptr));

	outer_clean_range(*save_ptr, *save_ptr + ptrsz);
	outer_clean_range(virt_to_phys(save_ptr),
			  virt_to_phys(save_ptr) + sizeof(*save_ptr));
}

extern struct sleep_save_sp sleep_save_sp;

static int cpu_suspend_alloc_sp(void)
{
	void *ctx_ptr;
	/* ctx_ptr is an array of physical addresses */
	ctx_ptr = kcalloc(mpidr_hash_size(), sizeof(u32), GFP_KERNEL);

	if (WARN_ON(!ctx_ptr))
		return -ENOMEM;
	sleep_save_sp.save_ptr_stash = ctx_ptr;
	sleep_save_sp.save_ptr_stash_phys = virt_to_phys(ctx_ptr);
	sync_cache_w(&sleep_save_sp);
	return 0;
}
early_initcall(cpu_suspend_alloc_sp);
