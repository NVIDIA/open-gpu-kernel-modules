// SPDX-License-Identifier: GPL-2.0-only
/*
 * Dynamic DMA mapping support.
 *
 * This implementation is a fallback for platforms that do not support
 * I/O TLBs (aka DMA address translation hardware).
 * Copyright (C) 2000 Asit Mallick <Asit.K.Mallick@intel.com>
 * Copyright (C) 2000 Goutham Rao <goutham.rao@intel.com>
 * Copyright (C) 2000, 2003 Hewlett-Packard Co
 *	David Mosberger-Tang <davidm@hpl.hp.com>
 *
 * 03/05/07 davidm	Switch from PCI-DMA to generic device DMA API.
 * 00/12/13 davidm	Rename to swiotlb.c and add mark_clean() to avoid
 *			unnecessary i-cache flushing.
 * 04/07/.. ak		Better overflow handling. Assorted fixes.
 * 05/09/10 linville	Add support for syncing ranges, support syncing for
 *			DMA_BIDIRECTIONAL mappings, miscellaneous cleanup.
 * 08/12/11 beckyb	Add highmem support
 */

#define pr_fmt(fmt) "software IO TLB: " fmt

#include <linux/cache.h>
#include <linux/dma-direct.h>
#include <linux/dma-map-ops.h>
#include <linux/mm.h>
#include <linux/export.h>
#include <linux/spinlock.h>
#include <linux/string.h>
#include <linux/swiotlb.h>
#include <linux/pfn.h>
#include <linux/types.h>
#include <linux/ctype.h>
#include <linux/highmem.h>
#include <linux/gfp.h>
#include <linux/scatterlist.h>
#include <linux/mem_encrypt.h>
#include <linux/set_memory.h>
#ifdef CONFIG_DEBUG_FS
#include <linux/debugfs.h>
#endif

#include <asm/io.h>
#include <asm/dma.h>

#include <linux/init.h>
#include <linux/memblock.h>
#include <linux/iommu-helper.h>

#define CREATE_TRACE_POINTS
#include <trace/events/swiotlb.h>

#define SLABS_PER_PAGE (1 << (PAGE_SHIFT - IO_TLB_SHIFT))

/*
 * Minimum IO TLB size to bother booting with.  Systems with mainly
 * 64bit capable cards will only lightly use the swiotlb.  If we can't
 * allocate a contiguous 1MB, we're probably in trouble anyway.
 */
#define IO_TLB_MIN_SLABS ((1<<20) >> IO_TLB_SHIFT)

#define INVALID_PHYS_ADDR (~(phys_addr_t)0)

enum swiotlb_force swiotlb_force;

struct io_tlb_mem *io_tlb_default_mem;

/*
 * Max segment that we can provide which (if pages are contingous) will
 * not be bounced (unless SWIOTLB_FORCE is set).
 */
static unsigned int max_segment;

static unsigned long default_nslabs = IO_TLB_DEFAULT_SIZE >> IO_TLB_SHIFT;

static int __init
setup_io_tlb_npages(char *str)
{
	if (isdigit(*str)) {
		/* avoid tail segment of size < IO_TLB_SEGSIZE */
		default_nslabs =
			ALIGN(simple_strtoul(str, &str, 0), IO_TLB_SEGSIZE);
	}
	if (*str == ',')
		++str;
	if (!strcmp(str, "force"))
		swiotlb_force = SWIOTLB_FORCE;
	else if (!strcmp(str, "noforce"))
		swiotlb_force = SWIOTLB_NO_FORCE;

	return 0;
}
early_param("swiotlb", setup_io_tlb_npages);

unsigned int swiotlb_max_segment(void)
{
	return io_tlb_default_mem ? max_segment : 0;
}
EXPORT_SYMBOL_GPL(swiotlb_max_segment);

void swiotlb_set_max_segment(unsigned int val)
{
	if (swiotlb_force == SWIOTLB_FORCE)
		max_segment = 1;
	else
		max_segment = rounddown(val, PAGE_SIZE);
}

unsigned long swiotlb_size_or_default(void)
{
	return default_nslabs << IO_TLB_SHIFT;
}

void __init swiotlb_adjust_size(unsigned long size)
{
	/*
	 * If swiotlb parameter has not been specified, give a chance to
	 * architectures such as those supporting memory encryption to
	 * adjust/expand SWIOTLB size for their use.
	 */
	if (default_nslabs != IO_TLB_DEFAULT_SIZE >> IO_TLB_SHIFT)
		return;
	size = ALIGN(size, IO_TLB_SIZE);
	default_nslabs = ALIGN(size >> IO_TLB_SHIFT, IO_TLB_SEGSIZE);
	pr_info("SWIOTLB bounce buffer size adjusted to %luMB", size >> 20);
}

void swiotlb_print_info(void)
{
	struct io_tlb_mem *mem = io_tlb_default_mem;

	if (!mem) {
		pr_warn("No low mem\n");
		return;
	}

	pr_info("mapped [mem %pa-%pa] (%luMB)\n", &mem->start, &mem->end,
	       (mem->nslabs << IO_TLB_SHIFT) >> 20);
}

static inline unsigned long io_tlb_offset(unsigned long val)
{
	return val & (IO_TLB_SEGSIZE - 1);
}

static inline unsigned long nr_slots(u64 val)
{
	return DIV_ROUND_UP(val, IO_TLB_SIZE);
}

/*
 * Early SWIOTLB allocation may be too early to allow an architecture to
 * perform the desired operations.  This function allows the architecture to
 * call SWIOTLB when the operations are possible.  It needs to be called
 * before the SWIOTLB memory is used.
 */
void __init swiotlb_update_mem_attributes(void)
{
	struct io_tlb_mem *mem = io_tlb_default_mem;
	void *vaddr;
	unsigned long bytes;

	if (!mem || mem->late_alloc)
		return;
	vaddr = phys_to_virt(mem->start);
	bytes = PAGE_ALIGN(mem->nslabs << IO_TLB_SHIFT);
	set_memory_decrypted((unsigned long)vaddr, bytes >> PAGE_SHIFT);
	memset(vaddr, 0, bytes);
}

int __init swiotlb_init_with_tbl(char *tlb, unsigned long nslabs, int verbose)
{
	unsigned long bytes = nslabs << IO_TLB_SHIFT, i;
	struct io_tlb_mem *mem;
	size_t alloc_size;

	if (swiotlb_force == SWIOTLB_NO_FORCE)
		return 0;

	/* protect against double initialization */
	if (WARN_ON_ONCE(io_tlb_default_mem))
		return -ENOMEM;

	alloc_size = PAGE_ALIGN(struct_size(mem, slots, nslabs));
	mem = memblock_alloc(alloc_size, PAGE_SIZE);
	if (!mem)
		panic("%s: Failed to allocate %zu bytes align=0x%lx\n",
		      __func__, alloc_size, PAGE_SIZE);
	mem->nslabs = nslabs;
	mem->start = __pa(tlb);
	mem->end = mem->start + bytes;
	mem->index = 0;
	spin_lock_init(&mem->lock);
	for (i = 0; i < mem->nslabs; i++) {
		mem->slots[i].list = IO_TLB_SEGSIZE - io_tlb_offset(i);
		mem->slots[i].orig_addr = INVALID_PHYS_ADDR;
		mem->slots[i].alloc_size = 0;
	}

	io_tlb_default_mem = mem;
	if (verbose)
		swiotlb_print_info();
	swiotlb_set_max_segment(mem->nslabs << IO_TLB_SHIFT);
	return 0;
}

/*
 * Statically reserve bounce buffer space and initialize bounce buffer data
 * structures for the software IO TLB used to implement the DMA API.
 */
void  __init
swiotlb_init(int verbose)
{
	size_t bytes = PAGE_ALIGN(default_nslabs << IO_TLB_SHIFT);
	void *tlb;

	if (swiotlb_force == SWIOTLB_NO_FORCE)
		return;

	/* Get IO TLB memory from the low pages */
	tlb = memblock_alloc_low(bytes, PAGE_SIZE);
	if (!tlb)
		goto fail;
	if (swiotlb_init_with_tbl(tlb, default_nslabs, verbose))
		goto fail_free_mem;
	return;

fail_free_mem:
	memblock_free_early(__pa(tlb), bytes);
fail:
	pr_warn("Cannot allocate buffer");
}

/*
 * Systems with larger DMA zones (those that don't support ISA) can
 * initialize the swiotlb later using the slab allocator if needed.
 * This should be just like above, but with some error catching.
 */
int
swiotlb_late_init_with_default_size(size_t default_size)
{
	unsigned long nslabs =
		ALIGN(default_size >> IO_TLB_SHIFT, IO_TLB_SEGSIZE);
	unsigned long bytes;
	unsigned char *vstart = NULL;
	unsigned int order;
	int rc = 0;

	if (swiotlb_force == SWIOTLB_NO_FORCE)
		return 0;

	/*
	 * Get IO TLB memory from the low pages
	 */
	order = get_order(nslabs << IO_TLB_SHIFT);
	nslabs = SLABS_PER_PAGE << order;
	bytes = nslabs << IO_TLB_SHIFT;

	while ((SLABS_PER_PAGE << order) > IO_TLB_MIN_SLABS) {
		vstart = (void *)__get_free_pages(GFP_DMA | __GFP_NOWARN,
						  order);
		if (vstart)
			break;
		order--;
	}

	if (!vstart)
		return -ENOMEM;

	if (order != get_order(bytes)) {
		pr_warn("only able to allocate %ld MB\n",
			(PAGE_SIZE << order) >> 20);
		nslabs = SLABS_PER_PAGE << order;
	}
	rc = swiotlb_late_init_with_tbl(vstart, nslabs);
	if (rc)
		free_pages((unsigned long)vstart, order);

	return rc;
}

int
swiotlb_late_init_with_tbl(char *tlb, unsigned long nslabs)
{
	unsigned long bytes = nslabs << IO_TLB_SHIFT, i;
	struct io_tlb_mem *mem;

	if (swiotlb_force == SWIOTLB_NO_FORCE)
		return 0;

	/* protect against double initialization */
	if (WARN_ON_ONCE(io_tlb_default_mem))
		return -ENOMEM;

	mem = (void *)__get_free_pages(GFP_KERNEL,
		get_order(struct_size(mem, slots, nslabs)));
	if (!mem)
		return -ENOMEM;

	mem->nslabs = nslabs;
	mem->start = virt_to_phys(tlb);
	mem->end = mem->start + bytes;
	mem->index = 0;
	mem->late_alloc = 1;
	spin_lock_init(&mem->lock);
	for (i = 0; i < mem->nslabs; i++) {
		mem->slots[i].list = IO_TLB_SEGSIZE - io_tlb_offset(i);
		mem->slots[i].orig_addr = INVALID_PHYS_ADDR;
		mem->slots[i].alloc_size = 0;
	}

	set_memory_decrypted((unsigned long)tlb, bytes >> PAGE_SHIFT);
	memset(tlb, 0, bytes);

	io_tlb_default_mem = mem;
	swiotlb_print_info();
	swiotlb_set_max_segment(mem->nslabs << IO_TLB_SHIFT);
	return 0;
}

void __init swiotlb_exit(void)
{
	struct io_tlb_mem *mem = io_tlb_default_mem;
	size_t size;

	if (!mem)
		return;

	size = struct_size(mem, slots, mem->nslabs);
	if (mem->late_alloc)
		free_pages((unsigned long)mem, get_order(size));
	else
		memblock_free_late(__pa(mem), PAGE_ALIGN(size));
	io_tlb_default_mem = NULL;
}

/*
 * Return the offset into a iotlb slot required to keep the device happy.
 */
static unsigned int swiotlb_align_offset(struct device *dev, u64 addr)
{
	return addr & dma_get_min_align_mask(dev) & (IO_TLB_SIZE - 1);
}

/*
 * Bounce: copy the swiotlb buffer from or back to the original dma location
 */
static void swiotlb_bounce(struct device *dev, phys_addr_t tlb_addr, size_t size,
			   enum dma_data_direction dir)
{
	struct io_tlb_mem *mem = io_tlb_default_mem;
	int index = (tlb_addr - mem->start) >> IO_TLB_SHIFT;
	phys_addr_t orig_addr = mem->slots[index].orig_addr;
	size_t alloc_size = mem->slots[index].alloc_size;
	unsigned long pfn = PFN_DOWN(orig_addr);
	unsigned char *vaddr = phys_to_virt(tlb_addr);
	unsigned int tlb_offset;

	if (orig_addr == INVALID_PHYS_ADDR)
		return;

	tlb_offset = (tlb_addr & (IO_TLB_SIZE - 1)) -
		     swiotlb_align_offset(dev, orig_addr);

	orig_addr += tlb_offset;
	alloc_size -= tlb_offset;

	if (size > alloc_size) {
		dev_WARN_ONCE(dev, 1,
			"Buffer overflow detected. Allocation size: %zu. Mapping size: %zu.\n",
			alloc_size, size);
		size = alloc_size;
	}

	if (PageHighMem(pfn_to_page(pfn))) {
		/* The buffer does not have a mapping.  Map it in and copy */
		unsigned int offset = orig_addr & ~PAGE_MASK;
		char *buffer;
		unsigned int sz = 0;
		unsigned long flags;

		while (size) {
			sz = min_t(size_t, PAGE_SIZE - offset, size);

			local_irq_save(flags);
			buffer = kmap_atomic(pfn_to_page(pfn));
			if (dir == DMA_TO_DEVICE)
				memcpy(vaddr, buffer + offset, sz);
			else
				memcpy(buffer + offset, vaddr, sz);
			kunmap_atomic(buffer);
			local_irq_restore(flags);

			size -= sz;
			pfn++;
			vaddr += sz;
			offset = 0;
		}
	} else if (dir == DMA_TO_DEVICE) {
		memcpy(vaddr, phys_to_virt(orig_addr), size);
	} else {
		memcpy(phys_to_virt(orig_addr), vaddr, size);
	}
}

#define slot_addr(start, idx)	((start) + ((idx) << IO_TLB_SHIFT))

/*
 * Carefully handle integer overflow which can occur when boundary_mask == ~0UL.
 */
static inline unsigned long get_max_slots(unsigned long boundary_mask)
{
	if (boundary_mask == ~0UL)
		return 1UL << (BITS_PER_LONG - IO_TLB_SHIFT);
	return nr_slots(boundary_mask + 1);
}

static unsigned int wrap_index(struct io_tlb_mem *mem, unsigned int index)
{
	if (index >= mem->nslabs)
		return 0;
	return index;
}

/*
 * Find a suitable number of IO TLB entries size that will fit this request and
 * allocate a buffer from that IO TLB pool.
 */
static int find_slots(struct device *dev, phys_addr_t orig_addr,
		size_t alloc_size)
{
	struct io_tlb_mem *mem = io_tlb_default_mem;
	unsigned long boundary_mask = dma_get_seg_boundary(dev);
	dma_addr_t tbl_dma_addr =
		phys_to_dma_unencrypted(dev, mem->start) & boundary_mask;
	unsigned long max_slots = get_max_slots(boundary_mask);
	unsigned int iotlb_align_mask =
		dma_get_min_align_mask(dev) & ~(IO_TLB_SIZE - 1);
	unsigned int nslots = nr_slots(alloc_size), stride;
	unsigned int index, wrap, count = 0, i;
	unsigned long flags;

	BUG_ON(!nslots);

	/*
	 * For mappings with an alignment requirement don't bother looping to
	 * unaligned slots once we found an aligned one.  For allocations of
	 * PAGE_SIZE or larger only look for page aligned allocations.
	 */
	stride = (iotlb_align_mask >> IO_TLB_SHIFT) + 1;
	if (alloc_size >= PAGE_SIZE)
		stride = max(stride, stride << (PAGE_SHIFT - IO_TLB_SHIFT));

	spin_lock_irqsave(&mem->lock, flags);
	if (unlikely(nslots > mem->nslabs - mem->used))
		goto not_found;

	index = wrap = wrap_index(mem, ALIGN(mem->index, stride));
	do {
		if ((slot_addr(tbl_dma_addr, index) & iotlb_align_mask) !=
		    (orig_addr & iotlb_align_mask)) {
			index = wrap_index(mem, index + 1);
			continue;
		}

		/*
		 * If we find a slot that indicates we have 'nslots' number of
		 * contiguous buffers, we allocate the buffers from that slot
		 * and mark the entries as '0' indicating unavailable.
		 */
		if (!iommu_is_span_boundary(index, nslots,
					    nr_slots(tbl_dma_addr),
					    max_slots)) {
			if (mem->slots[index].list >= nslots)
				goto found;
		}
		index = wrap_index(mem, index + stride);
	} while (index != wrap);

not_found:
	spin_unlock_irqrestore(&mem->lock, flags);
	return -1;

found:
	for (i = index; i < index + nslots; i++)
		mem->slots[i].list = 0;
	for (i = index - 1;
	     io_tlb_offset(i) != IO_TLB_SEGSIZE - 1 &&
	     mem->slots[i].list; i--)
		mem->slots[i].list = ++count;

	/*
	 * Update the indices to avoid searching in the next round.
	 */
	if (index + nslots < mem->nslabs)
		mem->index = index + nslots;
	else
		mem->index = 0;
	mem->used += nslots;

	spin_unlock_irqrestore(&mem->lock, flags);
	return index;
}

phys_addr_t swiotlb_tbl_map_single(struct device *dev, phys_addr_t orig_addr,
		size_t mapping_size, size_t alloc_size,
		enum dma_data_direction dir, unsigned long attrs)
{
	struct io_tlb_mem *mem = io_tlb_default_mem;
	unsigned int offset = swiotlb_align_offset(dev, orig_addr);
	unsigned int i;
	int index;
	phys_addr_t tlb_addr;

	if (!mem)
		panic("Can not allocate SWIOTLB buffer earlier and can't now provide you with the DMA bounce buffer");

	if (mem_encrypt_active())
		pr_warn_once("Memory encryption is active and system is using DMA bounce buffers\n");

	if (mapping_size > alloc_size) {
		dev_warn_once(dev, "Invalid sizes (mapping: %zd bytes, alloc: %zd bytes)",
			      mapping_size, alloc_size);
		return (phys_addr_t)DMA_MAPPING_ERROR;
	}

	index = find_slots(dev, orig_addr, alloc_size + offset);
	if (index == -1) {
		if (!(attrs & DMA_ATTR_NO_WARN))
			dev_warn_ratelimited(dev,
	"swiotlb buffer is full (sz: %zd bytes), total %lu (slots), used %lu (slots)\n",
				 alloc_size, mem->nslabs, mem->used);
		return (phys_addr_t)DMA_MAPPING_ERROR;
	}

	/*
	 * Save away the mapping from the original address to the DMA address.
	 * This is needed when we sync the memory.  Then we sync the buffer if
	 * needed.
	 */
	for (i = 0; i < nr_slots(alloc_size + offset); i++) {
		mem->slots[index + i].orig_addr = slot_addr(orig_addr, i);
		mem->slots[index + i].alloc_size =
			alloc_size - (i << IO_TLB_SHIFT);
	}
	tlb_addr = slot_addr(mem->start, index) + offset;
	if (!(attrs & DMA_ATTR_SKIP_CPU_SYNC) &&
	    (dir == DMA_TO_DEVICE || dir == DMA_BIDIRECTIONAL))
		swiotlb_bounce(dev, tlb_addr, mapping_size, DMA_TO_DEVICE);
	return tlb_addr;
}

/*
 * tlb_addr is the physical address of the bounce buffer to unmap.
 */
void swiotlb_tbl_unmap_single(struct device *hwdev, phys_addr_t tlb_addr,
			      size_t mapping_size, enum dma_data_direction dir,
			      unsigned long attrs)
{
	struct io_tlb_mem *mem = io_tlb_default_mem;
	unsigned long flags;
	unsigned int offset = swiotlb_align_offset(hwdev, tlb_addr);
	int index = (tlb_addr - offset - mem->start) >> IO_TLB_SHIFT;
	int nslots = nr_slots(mem->slots[index].alloc_size + offset);
	int count, i;

	/*
	 * First, sync the memory before unmapping the entry
	 */
	if (!(attrs & DMA_ATTR_SKIP_CPU_SYNC) &&
	    (dir == DMA_FROM_DEVICE || dir == DMA_BIDIRECTIONAL))
		swiotlb_bounce(hwdev, tlb_addr, mapping_size, DMA_FROM_DEVICE);

	/*
	 * Return the buffer to the free list by setting the corresponding
	 * entries to indicate the number of contiguous entries available.
	 * While returning the entries to the free list, we merge the entries
	 * with slots below and above the pool being returned.
	 */
	spin_lock_irqsave(&mem->lock, flags);
	if (index + nslots < ALIGN(index + 1, IO_TLB_SEGSIZE))
		count = mem->slots[index + nslots].list;
	else
		count = 0;

	/*
	 * Step 1: return the slots to the free list, merging the slots with
	 * superceeding slots
	 */
	for (i = index + nslots - 1; i >= index; i--) {
		mem->slots[i].list = ++count;
		mem->slots[i].orig_addr = INVALID_PHYS_ADDR;
		mem->slots[i].alloc_size = 0;
	}

	/*
	 * Step 2: merge the returned slots with the preceding slots, if
	 * available (non zero)
	 */
	for (i = index - 1;
	     io_tlb_offset(i) != IO_TLB_SEGSIZE - 1 && mem->slots[i].list;
	     i--)
		mem->slots[i].list = ++count;
	mem->used -= nslots;
	spin_unlock_irqrestore(&mem->lock, flags);
}

void swiotlb_sync_single_for_device(struct device *dev, phys_addr_t tlb_addr,
		size_t size, enum dma_data_direction dir)
{
	if (dir == DMA_TO_DEVICE || dir == DMA_BIDIRECTIONAL)
		swiotlb_bounce(dev, tlb_addr, size, DMA_TO_DEVICE);
	else
		BUG_ON(dir != DMA_FROM_DEVICE);
}

void swiotlb_sync_single_for_cpu(struct device *dev, phys_addr_t tlb_addr,
		size_t size, enum dma_data_direction dir)
{
	if (dir == DMA_FROM_DEVICE || dir == DMA_BIDIRECTIONAL)
		swiotlb_bounce(dev, tlb_addr, size, DMA_FROM_DEVICE);
	else
		BUG_ON(dir != DMA_TO_DEVICE);
}

/*
 * Create a swiotlb mapping for the buffer at @paddr, and in case of DMAing
 * to the device copy the data into it as well.
 */
dma_addr_t swiotlb_map(struct device *dev, phys_addr_t paddr, size_t size,
		enum dma_data_direction dir, unsigned long attrs)
{
	phys_addr_t swiotlb_addr;
	dma_addr_t dma_addr;

	trace_swiotlb_bounced(dev, phys_to_dma(dev, paddr), size,
			      swiotlb_force);

	swiotlb_addr = swiotlb_tbl_map_single(dev, paddr, size, size, dir,
			attrs);
	if (swiotlb_addr == (phys_addr_t)DMA_MAPPING_ERROR)
		return DMA_MAPPING_ERROR;

	/* Ensure that the address returned is DMA'ble */
	dma_addr = phys_to_dma_unencrypted(dev, swiotlb_addr);
	if (unlikely(!dma_capable(dev, dma_addr, size, true))) {
		swiotlb_tbl_unmap_single(dev, swiotlb_addr, size, dir,
			attrs | DMA_ATTR_SKIP_CPU_SYNC);
		dev_WARN_ONCE(dev, 1,
			"swiotlb addr %pad+%zu overflow (mask %llx, bus limit %llx).\n",
			&dma_addr, size, *dev->dma_mask, dev->bus_dma_limit);
		return DMA_MAPPING_ERROR;
	}

	if (!dev_is_dma_coherent(dev) && !(attrs & DMA_ATTR_SKIP_CPU_SYNC))
		arch_sync_dma_for_device(swiotlb_addr, size, dir);
	return dma_addr;
}

size_t swiotlb_max_mapping_size(struct device *dev)
{
	return ((size_t)IO_TLB_SIZE) * IO_TLB_SEGSIZE;
}

bool is_swiotlb_active(void)
{
	return io_tlb_default_mem != NULL;
}
EXPORT_SYMBOL_GPL(is_swiotlb_active);

#ifdef CONFIG_DEBUG_FS

static int __init swiotlb_create_debugfs(void)
{
	struct io_tlb_mem *mem = io_tlb_default_mem;

	if (!mem)
		return 0;
	mem->debugfs = debugfs_create_dir("swiotlb", NULL);
	debugfs_create_ulong("io_tlb_nslabs", 0400, mem->debugfs, &mem->nslabs);
	debugfs_create_ulong("io_tlb_used", 0400, mem->debugfs, &mem->used);
	return 0;
}

late_initcall(swiotlb_create_debugfs);

#endif
