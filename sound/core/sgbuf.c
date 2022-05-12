// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Scatter-Gather buffer
 *
 *  Copyright (c) by Takashi Iwai <tiwai@suse.de>
 */

#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>
#include <linux/export.h>
#include <sound/memalloc.h>


/* table entries are align to 32 */
#define SGBUF_TBL_ALIGN		32
#define sgbuf_align_table(tbl)	ALIGN((tbl), SGBUF_TBL_ALIGN)

int snd_free_sgbuf_pages(struct snd_dma_buffer *dmab)
{
	struct snd_sg_buf *sgbuf = dmab->private_data;
	struct snd_dma_buffer tmpb;
	int i;

	if (! sgbuf)
		return -EINVAL;

	vunmap(dmab->area);
	dmab->area = NULL;

	tmpb.dev.type = SNDRV_DMA_TYPE_DEV;
	if (dmab->dev.type == SNDRV_DMA_TYPE_DEV_UC_SG)
		tmpb.dev.type = SNDRV_DMA_TYPE_DEV_UC;
	tmpb.dev.dev = sgbuf->dev;
	for (i = 0; i < sgbuf->pages; i++) {
		if (!(sgbuf->table[i].addr & ~PAGE_MASK))
			continue; /* continuous pages */
		tmpb.area = sgbuf->table[i].buf;
		tmpb.addr = sgbuf->table[i].addr & PAGE_MASK;
		tmpb.bytes = (sgbuf->table[i].addr & ~PAGE_MASK) << PAGE_SHIFT;
		snd_dma_free_pages(&tmpb);
	}

	kfree(sgbuf->table);
	kfree(sgbuf->page_table);
	kfree(sgbuf);
	dmab->private_data = NULL;
	
	return 0;
}

#define MAX_ALLOC_PAGES		32

void *snd_malloc_sgbuf_pages(struct device *device,
			     size_t size, struct snd_dma_buffer *dmab,
			     size_t *res_size)
{
	struct snd_sg_buf *sgbuf;
	unsigned int i, pages, chunk, maxpages;
	struct snd_dma_buffer tmpb;
	struct snd_sg_page *table;
	struct page **pgtable;
	int type = SNDRV_DMA_TYPE_DEV;
	pgprot_t prot = PAGE_KERNEL;

	dmab->area = NULL;
	dmab->addr = 0;
	dmab->private_data = sgbuf = kzalloc(sizeof(*sgbuf), GFP_KERNEL);
	if (! sgbuf)
		return NULL;
	if (dmab->dev.type == SNDRV_DMA_TYPE_DEV_UC_SG) {
		type = SNDRV_DMA_TYPE_DEV_UC;
#ifdef pgprot_noncached
		prot = pgprot_noncached(PAGE_KERNEL);
#endif
	}
	sgbuf->dev = device;
	pages = snd_sgbuf_aligned_pages(size);
	sgbuf->tblsize = sgbuf_align_table(pages);
	table = kcalloc(sgbuf->tblsize, sizeof(*table), GFP_KERNEL);
	if (!table)
		goto _failed;
	sgbuf->table = table;
	pgtable = kcalloc(sgbuf->tblsize, sizeof(*pgtable), GFP_KERNEL);
	if (!pgtable)
		goto _failed;
	sgbuf->page_table = pgtable;

	/* allocate pages */
	maxpages = MAX_ALLOC_PAGES;
	while (pages > 0) {
		chunk = pages;
		/* don't be too eager to take a huge chunk */
		if (chunk > maxpages)
			chunk = maxpages;
		chunk <<= PAGE_SHIFT;
		if (snd_dma_alloc_pages_fallback(type, device,
						 chunk, &tmpb) < 0) {
			if (!sgbuf->pages)
				goto _failed;
			if (!res_size)
				goto _failed;
			size = sgbuf->pages * PAGE_SIZE;
			break;
		}
		chunk = tmpb.bytes >> PAGE_SHIFT;
		for (i = 0; i < chunk; i++) {
			table->buf = tmpb.area;
			table->addr = tmpb.addr;
			if (!i)
				table->addr |= chunk; /* mark head */
			table++;
			*pgtable++ = virt_to_page(tmpb.area);
			tmpb.area += PAGE_SIZE;
			tmpb.addr += PAGE_SIZE;
		}
		sgbuf->pages += chunk;
		pages -= chunk;
		if (chunk < maxpages)
			maxpages = chunk;
	}

	sgbuf->size = size;
	dmab->area = vmap(sgbuf->page_table, sgbuf->pages, VM_MAP, prot);
	if (! dmab->area)
		goto _failed;
	if (res_size)
		*res_size = sgbuf->size;
	return dmab->area;

 _failed:
	snd_free_sgbuf_pages(dmab); /* free the table */
	return NULL;
}

/*
 * compute the max chunk size with continuous pages on sg-buffer
 */
unsigned int snd_sgbuf_get_chunk_size(struct snd_dma_buffer *dmab,
				      unsigned int ofs, unsigned int size)
{
	struct snd_sg_buf *sg = dmab->private_data;
	unsigned int start, end, pg;

	if (!sg)
		return size;

	start = ofs >> PAGE_SHIFT;
	end = (ofs + size - 1) >> PAGE_SHIFT;
	/* check page continuity */
	pg = sg->table[start].addr >> PAGE_SHIFT;
	for (;;) {
		start++;
		if (start > end)
			break;
		pg++;
		if ((sg->table[start].addr >> PAGE_SHIFT) != pg)
			return (start << PAGE_SHIFT) - ofs;
	}
	/* ok, all on continuous pages */
	return size;
}
EXPORT_SYMBOL(snd_sgbuf_get_chunk_size);
