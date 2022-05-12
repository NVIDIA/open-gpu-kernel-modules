// SPDX-License-Identifier: GPL-2.0-only
/*
 *  linux/arch/arm/kernel/smp_tlb.c
 *
 *  Copyright (C) 2002 ARM Limited, All Rights Reserved.
 */
#include <linux/preempt.h>
#include <linux/smp.h>
#include <linux/uaccess.h>

#include <asm/smp_plat.h>
#include <asm/tlbflush.h>
#include <asm/mmu_context.h>

/**********************************************************************/

/*
 * TLB operations
 */
struct tlb_args {
	struct vm_area_struct *ta_vma;
	unsigned long ta_start;
	unsigned long ta_end;
};

static inline void ipi_flush_tlb_all(void *ignored)
{
	local_flush_tlb_all();
}

static inline void ipi_flush_tlb_mm(void *arg)
{
	struct mm_struct *mm = (struct mm_struct *)arg;

	local_flush_tlb_mm(mm);
}

static inline void ipi_flush_tlb_page(void *arg)
{
	struct tlb_args *ta = (struct tlb_args *)arg;
	unsigned int __ua_flags = uaccess_save_and_enable();

	local_flush_tlb_page(ta->ta_vma, ta->ta_start);

	uaccess_restore(__ua_flags);
}

static inline void ipi_flush_tlb_kernel_page(void *arg)
{
	struct tlb_args *ta = (struct tlb_args *)arg;

	local_flush_tlb_kernel_page(ta->ta_start);
}

static inline void ipi_flush_tlb_range(void *arg)
{
	struct tlb_args *ta = (struct tlb_args *)arg;
	unsigned int __ua_flags = uaccess_save_and_enable();

	local_flush_tlb_range(ta->ta_vma, ta->ta_start, ta->ta_end);

	uaccess_restore(__ua_flags);
}

static inline void ipi_flush_tlb_kernel_range(void *arg)
{
	struct tlb_args *ta = (struct tlb_args *)arg;

	local_flush_tlb_kernel_range(ta->ta_start, ta->ta_end);
}

static inline void ipi_flush_bp_all(void *ignored)
{
	local_flush_bp_all();
}

#ifdef CONFIG_ARM_ERRATA_798181
bool (*erratum_a15_798181_handler)(void);

static bool erratum_a15_798181_partial(void)
{
	asm("mcr p15, 0, %0, c8, c3, 1" : : "r" (0));
	dsb(ish);
	return false;
}

static bool erratum_a15_798181_broadcast(void)
{
	asm("mcr p15, 0, %0, c8, c3, 1" : : "r" (0));
	dsb(ish);
	return true;
}

void erratum_a15_798181_init(void)
{
	unsigned int midr = read_cpuid_id();
	unsigned int revidr = read_cpuid(CPUID_REVIDR);

	/* Brahma-B15 r0p0..r0p2 affected
	 * Cortex-A15 r0p0..r3p3 w/o ECO fix affected
	 * Fixes applied to A15 with respect to the revision and revidr are:
	 *
	 * r0p0-r2p1: No fixes applied
	 * r2p2,r2p3:
	 *	REVIDR[4]: 798181 Moving a virtual page that is being accessed
	 *		   by an active process can lead to unexpected behavior
	 *	REVIDR[9]: Not defined
	 * r2p4,r3p0,r3p1,r3p2:
	 *	REVIDR[4]: 798181 Moving a virtual page that is being accessed
	 *		   by an active process can lead to unexpected behavior
	 *	REVIDR[9]: 798181 Moving a virtual page that is being accessed
	 *		   by an active process can lead to unexpected behavior
	 *		   - This is an update to a previously released ECO.
	 * r3p3:
	 *	REVIDR[4]: Reserved
	 *	REVIDR[9]: 798181 Moving a virtual page that is being accessed
	 *		   by an active process can lead to unexpected behavior
	 *		   - This is an update to a previously released ECO.
	 *
	 * Handling:
	 *	REVIDR[9] set -> No WA
	 *	REVIDR[4] set, REVIDR[9] cleared -> Partial WA
	 *	Both cleared -> Full WA
	 */
	if ((midr & 0xff0ffff0) == 0x420f00f0 && midr <= 0x420f00f2) {
		erratum_a15_798181_handler = erratum_a15_798181_broadcast;
	} else if ((midr & 0xff0ffff0) == 0x410fc0f0 && midr < 0x412fc0f2) {
		erratum_a15_798181_handler = erratum_a15_798181_broadcast;
	} else if ((midr & 0xff0ffff0) == 0x410fc0f0 && midr < 0x412fc0f4) {
		if (revidr & 0x10)
			erratum_a15_798181_handler =
				erratum_a15_798181_partial;
		else
			erratum_a15_798181_handler =
				erratum_a15_798181_broadcast;
	} else if ((midr & 0xff0ffff0) == 0x410fc0f0 && midr < 0x413fc0f3) {
		if ((revidr & 0x210) == 0)
			erratum_a15_798181_handler =
				erratum_a15_798181_broadcast;
		else if (revidr & 0x10)
			erratum_a15_798181_handler =
				erratum_a15_798181_partial;
	} else if ((midr & 0xff0ffff0) == 0x410fc0f0 && midr < 0x414fc0f0) {
		if ((revidr & 0x200) == 0)
			erratum_a15_798181_handler =
				erratum_a15_798181_partial;
	}
}
#endif

static void ipi_flush_tlb_a15_erratum(void *arg)
{
	dmb();
}

static void broadcast_tlb_a15_erratum(void)
{
	if (!erratum_a15_798181())
		return;

	smp_call_function(ipi_flush_tlb_a15_erratum, NULL, 1);
}

static void broadcast_tlb_mm_a15_erratum(struct mm_struct *mm)
{
	int this_cpu;
	cpumask_t mask = { CPU_BITS_NONE };

	if (!erratum_a15_798181())
		return;

	this_cpu = get_cpu();
	a15_erratum_get_cpumask(this_cpu, mm, &mask);
	smp_call_function_many(&mask, ipi_flush_tlb_a15_erratum, NULL, 1);
	put_cpu();
}

void flush_tlb_all(void)
{
	if (tlb_ops_need_broadcast())
		on_each_cpu(ipi_flush_tlb_all, NULL, 1);
	else
		__flush_tlb_all();
	broadcast_tlb_a15_erratum();
}

void flush_tlb_mm(struct mm_struct *mm)
{
	if (tlb_ops_need_broadcast())
		on_each_cpu_mask(mm_cpumask(mm), ipi_flush_tlb_mm, mm, 1);
	else
		__flush_tlb_mm(mm);
	broadcast_tlb_mm_a15_erratum(mm);
}

void flush_tlb_page(struct vm_area_struct *vma, unsigned long uaddr)
{
	if (tlb_ops_need_broadcast()) {
		struct tlb_args ta;
		ta.ta_vma = vma;
		ta.ta_start = uaddr;
		on_each_cpu_mask(mm_cpumask(vma->vm_mm), ipi_flush_tlb_page,
					&ta, 1);
	} else
		__flush_tlb_page(vma, uaddr);
	broadcast_tlb_mm_a15_erratum(vma->vm_mm);
}

void flush_tlb_kernel_page(unsigned long kaddr)
{
	if (tlb_ops_need_broadcast()) {
		struct tlb_args ta;
		ta.ta_start = kaddr;
		on_each_cpu(ipi_flush_tlb_kernel_page, &ta, 1);
	} else
		__flush_tlb_kernel_page(kaddr);
	broadcast_tlb_a15_erratum();
}

void flush_tlb_range(struct vm_area_struct *vma,
                     unsigned long start, unsigned long end)
{
	if (tlb_ops_need_broadcast()) {
		struct tlb_args ta;
		ta.ta_vma = vma;
		ta.ta_start = start;
		ta.ta_end = end;
		on_each_cpu_mask(mm_cpumask(vma->vm_mm), ipi_flush_tlb_range,
					&ta, 1);
	} else
		local_flush_tlb_range(vma, start, end);
	broadcast_tlb_mm_a15_erratum(vma->vm_mm);
}

void flush_tlb_kernel_range(unsigned long start, unsigned long end)
{
	if (tlb_ops_need_broadcast()) {
		struct tlb_args ta;
		ta.ta_start = start;
		ta.ta_end = end;
		on_each_cpu(ipi_flush_tlb_kernel_range, &ta, 1);
	} else
		local_flush_tlb_kernel_range(start, end);
	broadcast_tlb_a15_erratum();
}

void flush_bp_all(void)
{
	if (tlb_ops_need_broadcast())
		on_each_cpu(ipi_flush_bp_all, NULL, 1);
	else
		__flush_bp_all();
}
