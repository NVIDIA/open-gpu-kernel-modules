// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Copyright (c) by Jaroslav Kysela <perex@perex.cz>
 *                   Takashi Iwai <tiwai@suse.de>
 * 
 *  Generic memory allocators
 */

#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/dma-mapping.h>
#include <linux/genalloc.h>
#include <linux/vmalloc.h>
#ifdef CONFIG_X86
#include <asm/set_memory.h>
#endif
#include <sound/memalloc.h>

/*
 *
 *  Bus-specific memory allocators
 *
 */

#ifdef CONFIG_HAS_DMA
/* allocate the coherent DMA pages */
static void snd_malloc_dev_pages(struct snd_dma_buffer *dmab, size_t size)
{
	gfp_t gfp_flags;

	gfp_flags = GFP_KERNEL
		| __GFP_COMP	/* compound page lets parts be mapped */
		| __GFP_NORETRY /* don't trigger OOM-killer */
		| __GFP_NOWARN; /* no stack trace print - this call is non-critical */
	dmab->area = dma_alloc_coherent(dmab->dev.dev, size, &dmab->addr,
					gfp_flags);
#ifdef CONFIG_X86
	if (dmab->area && dmab->dev.type == SNDRV_DMA_TYPE_DEV_UC)
		set_memory_wc((unsigned long)dmab->area,
			      PAGE_ALIGN(size) >> PAGE_SHIFT);
#endif
}

/* free the coherent DMA pages */
static void snd_free_dev_pages(struct snd_dma_buffer *dmab)
{
#ifdef CONFIG_X86
	if (dmab->dev.type == SNDRV_DMA_TYPE_DEV_UC)
		set_memory_wb((unsigned long)dmab->area,
			      PAGE_ALIGN(dmab->bytes) >> PAGE_SHIFT);
#endif
	dma_free_coherent(dmab->dev.dev, dmab->bytes, dmab->area, dmab->addr);
}

#ifdef CONFIG_GENERIC_ALLOCATOR
/**
 * snd_malloc_dev_iram - allocate memory from on-chip internal ram
 * @dmab: buffer allocation record to store the allocated data
 * @size: number of bytes to allocate from the iram
 *
 * This function requires iram phandle provided via of_node
 */
static void snd_malloc_dev_iram(struct snd_dma_buffer *dmab, size_t size)
{
	struct device *dev = dmab->dev.dev;
	struct gen_pool *pool = NULL;

	dmab->area = NULL;
	dmab->addr = 0;

	if (dev->of_node)
		pool = of_gen_pool_get(dev->of_node, "iram", 0);

	if (!pool)
		return;

	/* Assign the pool into private_data field */
	dmab->private_data = pool;

	dmab->area = gen_pool_dma_alloc_align(pool, size, &dmab->addr,
					PAGE_SIZE);
}

/**
 * snd_free_dev_iram - free allocated specific memory from on-chip internal ram
 * @dmab: buffer allocation record to store the allocated data
 */
static void snd_free_dev_iram(struct snd_dma_buffer *dmab)
{
	struct gen_pool *pool = dmab->private_data;

	if (pool && dmab->area)
		gen_pool_free(pool, (unsigned long)dmab->area, dmab->bytes);
}
#endif /* CONFIG_GENERIC_ALLOCATOR */
#endif /* CONFIG_HAS_DMA */

/*
 *
 *  ALSA generic memory management
 *
 */

static inline gfp_t snd_mem_get_gfp_flags(const struct device *dev,
					  gfp_t default_gfp)
{
	if (!dev)
		return default_gfp;
	else
		return (__force gfp_t)(unsigned long)dev;
}

/**
 * snd_dma_alloc_pages - allocate the buffer area according to the given type
 * @type: the DMA buffer type
 * @device: the device pointer
 * @size: the buffer size to allocate
 * @dmab: buffer allocation record to store the allocated data
 *
 * Calls the memory-allocator function for the corresponding
 * buffer type.
 *
 * Return: Zero if the buffer with the given size is allocated successfully,
 * otherwise a negative value on error.
 */
int snd_dma_alloc_pages(int type, struct device *device, size_t size,
			struct snd_dma_buffer *dmab)
{
	gfp_t gfp;

	if (WARN_ON(!size))
		return -ENXIO;
	if (WARN_ON(!dmab))
		return -ENXIO;

	size = PAGE_ALIGN(size);
	dmab->dev.type = type;
	dmab->dev.dev = device;
	dmab->bytes = 0;
	dmab->area = NULL;
	dmab->addr = 0;
	dmab->private_data = NULL;
	switch (type) {
	case SNDRV_DMA_TYPE_CONTINUOUS:
		gfp = snd_mem_get_gfp_flags(device, GFP_KERNEL);
		dmab->area = alloc_pages_exact(size, gfp);
		break;
	case SNDRV_DMA_TYPE_VMALLOC:
		gfp = snd_mem_get_gfp_flags(device, GFP_KERNEL | __GFP_HIGHMEM);
		dmab->area = __vmalloc(size, gfp);
		break;
#ifdef CONFIG_HAS_DMA
#ifdef CONFIG_GENERIC_ALLOCATOR
	case SNDRV_DMA_TYPE_DEV_IRAM:
		snd_malloc_dev_iram(dmab, size);
		if (dmab->area)
			break;
		/* Internal memory might have limited size and no enough space,
		 * so if we fail to malloc, try to fetch memory traditionally.
		 */
		dmab->dev.type = SNDRV_DMA_TYPE_DEV;
		fallthrough;
#endif /* CONFIG_GENERIC_ALLOCATOR */
	case SNDRV_DMA_TYPE_DEV:
	case SNDRV_DMA_TYPE_DEV_UC:
		snd_malloc_dev_pages(dmab, size);
		break;
#endif
#ifdef CONFIG_SND_DMA_SGBUF
	case SNDRV_DMA_TYPE_DEV_SG:
	case SNDRV_DMA_TYPE_DEV_UC_SG:
		snd_malloc_sgbuf_pages(device, size, dmab, NULL);
		break;
#endif
	default:
		pr_err("snd-malloc: invalid device type %d\n", type);
		return -ENXIO;
	}
	if (! dmab->area)
		return -ENOMEM;
	dmab->bytes = size;
	return 0;
}
EXPORT_SYMBOL(snd_dma_alloc_pages);

/**
 * snd_dma_alloc_pages_fallback - allocate the buffer area according to the given type with fallback
 * @type: the DMA buffer type
 * @device: the device pointer
 * @size: the buffer size to allocate
 * @dmab: buffer allocation record to store the allocated data
 *
 * Calls the memory-allocator function for the corresponding
 * buffer type.  When no space is left, this function reduces the size and
 * tries to allocate again.  The size actually allocated is stored in
 * res_size argument.
 *
 * Return: Zero if the buffer with the given size is allocated successfully,
 * otherwise a negative value on error.
 */
int snd_dma_alloc_pages_fallback(int type, struct device *device, size_t size,
				 struct snd_dma_buffer *dmab)
{
	int err;

	while ((err = snd_dma_alloc_pages(type, device, size, dmab)) < 0) {
		if (err != -ENOMEM)
			return err;
		if (size <= PAGE_SIZE)
			return -ENOMEM;
		size >>= 1;
		size = PAGE_SIZE << get_order(size);
	}
	if (! dmab->area)
		return -ENOMEM;
	return 0;
}
EXPORT_SYMBOL(snd_dma_alloc_pages_fallback);


/**
 * snd_dma_free_pages - release the allocated buffer
 * @dmab: the buffer allocation record to release
 *
 * Releases the allocated buffer via snd_dma_alloc_pages().
 */
void snd_dma_free_pages(struct snd_dma_buffer *dmab)
{
	switch (dmab->dev.type) {
	case SNDRV_DMA_TYPE_CONTINUOUS:
		free_pages_exact(dmab->area, dmab->bytes);
		break;
	case SNDRV_DMA_TYPE_VMALLOC:
		vfree(dmab->area);
		break;
#ifdef CONFIG_HAS_DMA
#ifdef CONFIG_GENERIC_ALLOCATOR
	case SNDRV_DMA_TYPE_DEV_IRAM:
		snd_free_dev_iram(dmab);
		break;
#endif /* CONFIG_GENERIC_ALLOCATOR */
	case SNDRV_DMA_TYPE_DEV:
	case SNDRV_DMA_TYPE_DEV_UC:
		snd_free_dev_pages(dmab);
		break;
#endif
#ifdef CONFIG_SND_DMA_SGBUF
	case SNDRV_DMA_TYPE_DEV_SG:
	case SNDRV_DMA_TYPE_DEV_UC_SG:
		snd_free_sgbuf_pages(dmab);
		break;
#endif
	default:
		pr_err("snd-malloc: invalid device type %d\n", dmab->dev.type);
	}
}
EXPORT_SYMBOL(snd_dma_free_pages);
