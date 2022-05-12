// SPDX-License-Identifier: GPL-2.0
/*
 * This file is based on code from OCTEON SDK by Cavium Networks.
 *
 * Copyright (c) 2003-2010 Cavium Networks
 */

#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/slab.h>

#include "octeon-ethernet.h"
#include "ethernet-mem.h"
#include "ethernet-defines.h"

/**
 * cvm_oct_fill_hw_skbuff - fill the supplied hardware pool with skbuffs
 * @pool:     Pool to allocate an skbuff for
 * @size:     Size of the buffer needed for the pool
 * @elements: Number of buffers to allocate
 *
 * Returns the actual number of buffers allocated.
 */
static int cvm_oct_fill_hw_skbuff(int pool, int size, int elements)
{
	int freed = elements;

	while (freed) {
		struct sk_buff *skb = dev_alloc_skb(size + 256);

		if (unlikely(!skb))
			break;
		skb_reserve(skb, 256 - (((unsigned long)skb->data) & 0x7f));
		*(struct sk_buff **)(skb->data - sizeof(void *)) = skb;
		cvmx_fpa_free(skb->data, pool, size / 128);
		freed--;
	}
	return elements - freed;
}

/**
 * cvm_oct_free_hw_skbuff- free hardware pool skbuffs
 * @pool:     Pool to allocate an skbuff for
 * @size:     Size of the buffer needed for the pool
 * @elements: Number of buffers to allocate
 */
static void cvm_oct_free_hw_skbuff(int pool, int size, int elements)
{
	char *memory;

	do {
		memory = cvmx_fpa_alloc(pool);
		if (memory) {
			struct sk_buff *skb =
			    *(struct sk_buff **)(memory - sizeof(void *));
			elements--;
			dev_kfree_skb(skb);
		}
	} while (memory);

	if (elements < 0)
		pr_warn("Freeing of pool %u had too many skbuffs (%d)\n",
			pool, elements);
	else if (elements > 0)
		pr_warn("Freeing of pool %u is missing %d skbuffs\n",
			pool, elements);
}

/**
 * cvm_oct_fill_hw_memory - fill a hardware pool with memory.
 * @pool:     Pool to populate
 * @size:     Size of each buffer in the pool
 * @elements: Number of buffers to allocate
 *
 * Returns the actual number of buffers allocated.
 */
static int cvm_oct_fill_hw_memory(int pool, int size, int elements)
{
	char *memory;
	char *fpa;
	int freed = elements;

	while (freed) {
		/*
		 * FPA memory must be 128 byte aligned.  Since we are
		 * aligning we need to save the original pointer so we
		 * can feed it to kfree when the memory is returned to
		 * the kernel.
		 *
		 * We allocate an extra 256 bytes to allow for
		 * alignment and space for the original pointer saved
		 * just before the block.
		 */
		memory = kmalloc(size + 256, GFP_ATOMIC);
		if (unlikely(!memory)) {
			pr_warn("Unable to allocate %u bytes for FPA pool %d\n",
				elements * size, pool);
			break;
		}
		fpa = (char *)(((unsigned long)memory + 256) & ~0x7fUL);
		*((char **)fpa - 1) = memory;
		cvmx_fpa_free(fpa, pool, 0);
		freed--;
	}
	return elements - freed;
}

/**
 * cvm_oct_free_hw_memory - Free memory allocated by cvm_oct_fill_hw_memory
 * @pool:     FPA pool to free
 * @size:     Size of each buffer in the pool
 * @elements: Number of buffers that should be in the pool
 */
static void cvm_oct_free_hw_memory(int pool, int size, int elements)
{
	char *memory;
	char *fpa;

	do {
		fpa = cvmx_fpa_alloc(pool);
		if (fpa) {
			elements--;
			fpa = (char *)phys_to_virt(cvmx_ptr_to_phys(fpa));
			memory = *((char **)fpa - 1);
			kfree(memory);
		}
	} while (fpa);

	if (elements < 0)
		pr_warn("Freeing of pool %u had too many buffers (%d)\n",
			pool, elements);
	else if (elements > 0)
		pr_warn("Warning: Freeing of pool %u is missing %d buffers\n",
			pool, elements);
}

int cvm_oct_mem_fill_fpa(int pool, int size, int elements)
{
	int freed;

	if (pool == CVMX_FPA_PACKET_POOL)
		freed = cvm_oct_fill_hw_skbuff(pool, size, elements);
	else
		freed = cvm_oct_fill_hw_memory(pool, size, elements);
	return freed;
}

void cvm_oct_mem_empty_fpa(int pool, int size, int elements)
{
	if (pool == CVMX_FPA_PACKET_POOL)
		cvm_oct_free_hw_skbuff(pool, size, elements);
	else
		cvm_oct_free_hw_memory(pool, size, elements);
}
