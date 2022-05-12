/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1998-2003 Hewlett-Packard Co
 *	David Mosberger-Tang <davidm@hpl.hp.com>
 *	Stephane Eranian <eranian@hpl.hp.com>
 * Copyright (C) 2000, Rohit Seth <rohit.seth@intel.com>
 * Copyright (C) 1999 VA Linux Systems
 * Copyright (C) 1999 Walt Drummond <drummond@valinux.com>
 * Copyright (C) 2003 Silicon Graphics, Inc. All rights reserved.
 *
 * Routines used by ia64 machines with contiguous (or virtually contiguous)
 * memory.
 */
#include <linux/efi.h>
#include <linux/memblock.h>
#include <linux/mm.h>
#include <linux/nmi.h>
#include <linux/swap.h>
#include <linux/sizes.h>

#include <asm/efi.h>
#include <asm/meminit.h>
#include <asm/sections.h>
#include <asm/mca.h>

/* physical address where the bootmem map is located */
unsigned long bootmap_start;

#ifdef CONFIG_SMP
static void *cpu_data;
/**
 * per_cpu_init - setup per-cpu variables
 *
 * Allocate and setup per-cpu data areas.
 */
void *per_cpu_init(void)
{
	static bool first_time = true;
	void *cpu0_data = __cpu0_per_cpu;
	unsigned int cpu;

	if (!first_time)
		goto skip;
	first_time = false;

	/*
	 * get_free_pages() cannot be used before cpu_init() done.
	 * BSP allocates PERCPU_PAGE_SIZE bytes for all possible CPUs
	 * to avoid that AP calls get_zeroed_page().
	 */
	for_each_possible_cpu(cpu) {
		void *src = cpu == 0 ? cpu0_data : __phys_per_cpu_start;

		memcpy(cpu_data, src, __per_cpu_end - __per_cpu_start);
		__per_cpu_offset[cpu] = (char *)cpu_data - __per_cpu_start;
		per_cpu(local_per_cpu_offset, cpu) = __per_cpu_offset[cpu];

		/*
		 * percpu area for cpu0 is moved from the __init area
		 * which is setup by head.S and used till this point.
		 * Update ar.k3.  This move is ensures that percpu
		 * area for cpu0 is on the correct node and its
		 * virtual address isn't insanely far from other
		 * percpu areas which is important for congruent
		 * percpu allocator.
		 */
		if (cpu == 0)
			ia64_set_kr(IA64_KR_PER_CPU_DATA, __pa(cpu_data) -
				    (unsigned long)__per_cpu_start);

		cpu_data += PERCPU_PAGE_SIZE;
	}
skip:
	return __per_cpu_start + __per_cpu_offset[smp_processor_id()];
}

static inline void
alloc_per_cpu_data(void)
{
	size_t size = PERCPU_PAGE_SIZE * num_possible_cpus();

	cpu_data = memblock_alloc_from(size, PERCPU_PAGE_SIZE,
				       __pa(MAX_DMA_ADDRESS));
	if (!cpu_data)
		panic("%s: Failed to allocate %lu bytes align=%lx from=%lx\n",
		      __func__, size, PERCPU_PAGE_SIZE, __pa(MAX_DMA_ADDRESS));
}

/**
 * setup_per_cpu_areas - setup percpu areas
 *
 * Arch code has already allocated and initialized percpu areas.  All
 * this function has to do is to teach the determined layout to the
 * dynamic percpu allocator, which happens to be more complex than
 * creating whole new ones using helpers.
 */
void __init
setup_per_cpu_areas(void)
{
	struct pcpu_alloc_info *ai;
	struct pcpu_group_info *gi;
	unsigned int cpu;
	ssize_t static_size, reserved_size, dyn_size;

	ai = pcpu_alloc_alloc_info(1, num_possible_cpus());
	if (!ai)
		panic("failed to allocate pcpu_alloc_info");
	gi = &ai->groups[0];

	/* units are assigned consecutively to possible cpus */
	for_each_possible_cpu(cpu)
		gi->cpu_map[gi->nr_units++] = cpu;

	/* set parameters */
	static_size = __per_cpu_end - __per_cpu_start;
	reserved_size = PERCPU_MODULE_RESERVE;
	dyn_size = PERCPU_PAGE_SIZE - static_size - reserved_size;
	if (dyn_size < 0)
		panic("percpu area overflow static=%zd reserved=%zd\n",
		      static_size, reserved_size);

	ai->static_size		= static_size;
	ai->reserved_size	= reserved_size;
	ai->dyn_size		= dyn_size;
	ai->unit_size		= PERCPU_PAGE_SIZE;
	ai->atom_size		= PAGE_SIZE;
	ai->alloc_size		= PERCPU_PAGE_SIZE;

	pcpu_setup_first_chunk(ai, __per_cpu_start + __per_cpu_offset[0]);
	pcpu_free_alloc_info(ai);
}
#else
#define alloc_per_cpu_data() do { } while (0)
#endif /* CONFIG_SMP */

/**
 * find_memory - setup memory map
 *
 * Walk the EFI memory map and find usable memory for the system, taking
 * into account reserved areas.
 */
void __init
find_memory (void)
{
	reserve_memory();

	/* first find highest page frame number */
	min_low_pfn = ~0UL;
	max_low_pfn = 0;
	efi_memmap_walk(find_max_min_low_pfn, NULL);
	max_pfn = max_low_pfn;

	memblock_add_node(0, PFN_PHYS(max_low_pfn), 0);

	find_initrd();

	alloc_per_cpu_data();
}

static int __init find_largest_hole(u64 start, u64 end, void *arg)
{
	u64 *max_gap = arg;

	static u64 last_end = PAGE_OFFSET;

	/* NOTE: this algorithm assumes efi memmap table is ordered */

	if (*max_gap < (start - last_end))
		*max_gap = start - last_end;
	last_end = end;
	return 0;
}

static void __init verify_gap_absence(void)
{
	unsigned long max_gap;

	/* Forbid FLATMEM if hole is > than 1G */
	efi_memmap_walk(find_largest_hole, (u64 *)&max_gap);
	if (max_gap >= SZ_1G)
		panic("Cannot use FLATMEM with %ldMB hole\n"
		      "Please switch over to SPARSEMEM\n",
		      (max_gap >> 20));
}

/*
 * Set up the page tables.
 */

void __init
paging_init (void)
{
	unsigned long max_dma;
	unsigned long max_zone_pfns[MAX_NR_ZONES];

	memset(max_zone_pfns, 0, sizeof(max_zone_pfns));
	max_dma = virt_to_phys((void *) MAX_DMA_ADDRESS) >> PAGE_SHIFT;
	max_zone_pfns[ZONE_DMA32] = max_dma;
	max_zone_pfns[ZONE_NORMAL] = max_low_pfn;

	verify_gap_absence();

	free_area_init(max_zone_pfns);
	zero_page_memmap_ptr = virt_to_page(ia64_imva(empty_zero_page));
}
