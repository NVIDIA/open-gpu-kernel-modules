/*******************************************************************
 * This file is part of the Emulex Linux Device Driver for         *
 * Fibre Channel Host Bus Adapters.                                *
 * Copyright (C) 2017-2018 Broadcom. All Rights Reserved. The term *
 * “Broadcom” refers to Broadcom Inc. and/or its subsidiaries.     *
 * Copyright (C) 2004-2014 Emulex.  All rights reserved.           *
 * EMULEX and SLI are trademarks of Emulex.                        *
 * www.broadcom.com                                                *
 * Portions Copyright (C) 2004-2005 Christoph Hellwig              *
 *                                                                 *
 * This program is free software; you can redistribute it and/or   *
 * modify it under the terms of version 2 of the GNU General       *
 * Public License as published by the Free Software Foundation.    *
 * This program is distributed in the hope that it will be useful. *
 * ALL EXPRESS OR IMPLIED CONDITIONS, REPRESENTATIONS AND          *
 * WARRANTIES, INCLUDING ANY IMPLIED WARRANTY OF MERCHANTABILITY,  *
 * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT, ARE      *
 * DISCLAIMED, EXCEPT TO THE EXTENT THAT SUCH DISCLAIMERS ARE HELD *
 * TO BE LEGALLY INVALID.  See the GNU General Public License for  *
 * more details, a copy of which can be found in the file COPYING  *
 * included with this package.                                     *
 *******************************************************************/

#include <linux/mempool.h>
#include <linux/slab.h>
#include <linux/pci.h>
#include <linux/interrupt.h>

#include <scsi/scsi.h>
#include <scsi/scsi_device.h>
#include <scsi/scsi_transport_fc.h>
#include <scsi/fc/fc_fs.h>

#include "lpfc_hw4.h"
#include "lpfc_hw.h"
#include "lpfc_sli.h"
#include "lpfc_sli4.h"
#include "lpfc_nl.h"
#include "lpfc_disc.h"
#include "lpfc.h"
#include "lpfc_scsi.h"
#include "lpfc_crtn.h"
#include "lpfc_logmsg.h"

#define LPFC_MBUF_POOL_SIZE     64      /* max elements in MBUF safety pool */
#define LPFC_MEM_POOL_SIZE      64      /* max elem in non-DMA safety pool */
#define LPFC_DEVICE_DATA_POOL_SIZE 64   /* max elements in device data pool */
#define LPFC_RRQ_POOL_SIZE	256	/* max elements in non-DMA  pool */
#define LPFC_MBX_POOL_SIZE	256	/* max elements in MBX non-DMA pool */

int
lpfc_mem_alloc_active_rrq_pool_s4(struct lpfc_hba *phba) {
	size_t bytes;
	int max_xri = phba->sli4_hba.max_cfg_param.max_xri;

	if (max_xri <= 0)
		return -ENOMEM;
	bytes = ((BITS_PER_LONG - 1 + max_xri) / BITS_PER_LONG) *
		  sizeof(unsigned long);
	phba->cfg_rrq_xri_bitmap_sz = bytes;
	phba->active_rrq_pool = mempool_create_kmalloc_pool(LPFC_MEM_POOL_SIZE,
							    bytes);
	if (!phba->active_rrq_pool)
		return -ENOMEM;
	else
		return 0;
}

/**
 * lpfc_mem_alloc - create and allocate all PCI and memory pools
 * @phba: HBA to allocate pools for
 * @align: alignment requirement for blocks; must be a power of two
 *
 * Description: Creates and allocates PCI pools lpfc_mbuf_pool,
 * lpfc_hrb_pool.  Creates and allocates kmalloc-backed mempools
 * for LPFC_MBOXQ_t and lpfc_nodelist.  Also allocates the VPI bitmask.
 *
 * Notes: Not interrupt-safe.  Must be called with no locks held.  If any
 * allocation fails, frees all successfully allocated memory before returning.
 *
 * Returns:
 *   0 on success
 *   -ENOMEM on failure (if any memory allocations fail)
 **/
int
lpfc_mem_alloc(struct lpfc_hba *phba, int align)
{
	struct lpfc_dma_pool *pool = &phba->lpfc_mbuf_safety_pool;
	int i;


	phba->lpfc_mbuf_pool = dma_pool_create("lpfc_mbuf_pool", &phba->pcidev->dev,
							LPFC_BPL_SIZE,
							align, 0);
	if (!phba->lpfc_mbuf_pool)
		goto fail;

	pool->elements = kmalloc_array(LPFC_MBUF_POOL_SIZE,
				       sizeof(struct lpfc_dmabuf),
				       GFP_KERNEL);
	if (!pool->elements)
		goto fail_free_lpfc_mbuf_pool;

	pool->max_count = 0;
	pool->current_count = 0;
	for ( i = 0; i < LPFC_MBUF_POOL_SIZE; i++) {
		pool->elements[i].virt = dma_pool_alloc(phba->lpfc_mbuf_pool,
				       GFP_KERNEL, &pool->elements[i].phys);
		if (!pool->elements[i].virt)
			goto fail_free_mbuf_pool;
		pool->max_count++;
		pool->current_count++;
	}

	phba->mbox_mem_pool = mempool_create_kmalloc_pool(LPFC_MBX_POOL_SIZE,
							  sizeof(LPFC_MBOXQ_t));
	if (!phba->mbox_mem_pool)
		goto fail_free_mbuf_pool;

	phba->nlp_mem_pool = mempool_create_kmalloc_pool(LPFC_MEM_POOL_SIZE,
						sizeof(struct lpfc_nodelist));
	if (!phba->nlp_mem_pool)
		goto fail_free_mbox_pool;

	if (phba->sli_rev == LPFC_SLI_REV4) {
		phba->rrq_pool =
			mempool_create_kmalloc_pool(LPFC_RRQ_POOL_SIZE,
						sizeof(struct lpfc_node_rrq));
		if (!phba->rrq_pool)
			goto fail_free_nlp_mem_pool;
		phba->lpfc_hrb_pool = dma_pool_create("lpfc_hrb_pool",
					      &phba->pcidev->dev,
					      LPFC_HDR_BUF_SIZE, align, 0);
		if (!phba->lpfc_hrb_pool)
			goto fail_free_rrq_mem_pool;

		phba->lpfc_drb_pool = dma_pool_create("lpfc_drb_pool",
					      &phba->pcidev->dev,
					      LPFC_DATA_BUF_SIZE, align, 0);
		if (!phba->lpfc_drb_pool)
			goto fail_free_hrb_pool;
		phba->lpfc_hbq_pool = NULL;
	} else {
		phba->lpfc_hbq_pool = dma_pool_create("lpfc_hbq_pool",
			&phba->pcidev->dev, LPFC_BPL_SIZE, align, 0);
		if (!phba->lpfc_hbq_pool)
			goto fail_free_nlp_mem_pool;
		phba->lpfc_hrb_pool = NULL;
		phba->lpfc_drb_pool = NULL;
	}

	if (phba->cfg_EnableXLane) {
		phba->device_data_mem_pool = mempool_create_kmalloc_pool(
					LPFC_DEVICE_DATA_POOL_SIZE,
					sizeof(struct lpfc_device_data));
		if (!phba->device_data_mem_pool)
			goto fail_free_drb_pool;
	} else {
		phba->device_data_mem_pool = NULL;
	}

	return 0;
fail_free_drb_pool:
	dma_pool_destroy(phba->lpfc_drb_pool);
	phba->lpfc_drb_pool = NULL;
 fail_free_hrb_pool:
	dma_pool_destroy(phba->lpfc_hrb_pool);
	phba->lpfc_hrb_pool = NULL;
 fail_free_rrq_mem_pool:
	mempool_destroy(phba->rrq_pool);
	phba->rrq_pool = NULL;
 fail_free_nlp_mem_pool:
	mempool_destroy(phba->nlp_mem_pool);
	phba->nlp_mem_pool = NULL;
 fail_free_mbox_pool:
	mempool_destroy(phba->mbox_mem_pool);
	phba->mbox_mem_pool = NULL;
 fail_free_mbuf_pool:
	while (i--)
		dma_pool_free(phba->lpfc_mbuf_pool, pool->elements[i].virt,
						 pool->elements[i].phys);
	kfree(pool->elements);
 fail_free_lpfc_mbuf_pool:
	dma_pool_destroy(phba->lpfc_mbuf_pool);
	phba->lpfc_mbuf_pool = NULL;
 fail:
	return -ENOMEM;
}

int
lpfc_nvmet_mem_alloc(struct lpfc_hba *phba)
{
	phba->lpfc_nvmet_drb_pool =
		dma_pool_create("lpfc_nvmet_drb_pool",
				&phba->pcidev->dev, LPFC_NVMET_DATA_BUF_SIZE,
				SGL_ALIGN_SZ, 0);
	if (!phba->lpfc_nvmet_drb_pool) {
		lpfc_printf_log(phba, KERN_ERR, LOG_INIT,
				"6024 Can't enable NVME Target - no memory\n");
		return -ENOMEM;
	}
	return 0;
}

/**
 * lpfc_mem_free - Frees memory allocated by lpfc_mem_alloc
 * @phba: HBA to free memory for
 *
 * Description: Free the memory allocated by lpfc_mem_alloc routine. This
 * routine is a the counterpart of lpfc_mem_alloc.
 *
 * Returns: None
 **/
void
lpfc_mem_free(struct lpfc_hba *phba)
{
	int i;
	struct lpfc_dma_pool *pool = &phba->lpfc_mbuf_safety_pool;
	struct lpfc_device_data *device_data;

	/* Free HBQ pools */
	lpfc_sli_hbqbuf_free_all(phba);
	dma_pool_destroy(phba->lpfc_nvmet_drb_pool);
	phba->lpfc_nvmet_drb_pool = NULL;

	dma_pool_destroy(phba->lpfc_drb_pool);
	phba->lpfc_drb_pool = NULL;

	dma_pool_destroy(phba->lpfc_hrb_pool);
	phba->lpfc_hrb_pool = NULL;

	dma_pool_destroy(phba->lpfc_hbq_pool);
	phba->lpfc_hbq_pool = NULL;

	mempool_destroy(phba->rrq_pool);
	phba->rrq_pool = NULL;

	/* Free NLP memory pool */
	mempool_destroy(phba->nlp_mem_pool);
	phba->nlp_mem_pool = NULL;
	if (phba->sli_rev == LPFC_SLI_REV4 && phba->active_rrq_pool) {
		mempool_destroy(phba->active_rrq_pool);
		phba->active_rrq_pool = NULL;
	}

	/* Free mbox memory pool */
	mempool_destroy(phba->mbox_mem_pool);
	phba->mbox_mem_pool = NULL;

	/* Free MBUF memory pool */
	for (i = 0; i < pool->current_count; i++)
		dma_pool_free(phba->lpfc_mbuf_pool, pool->elements[i].virt,
			      pool->elements[i].phys);
	kfree(pool->elements);

	dma_pool_destroy(phba->lpfc_mbuf_pool);
	phba->lpfc_mbuf_pool = NULL;

	/* Free Device Data memory pool */
	if (phba->device_data_mem_pool) {
		/* Ensure all objects have been returned to the pool */
		while (!list_empty(&phba->luns)) {
			device_data = list_first_entry(&phba->luns,
						       struct lpfc_device_data,
						       listentry);
			list_del(&device_data->listentry);
			mempool_free(device_data, phba->device_data_mem_pool);
		}
		mempool_destroy(phba->device_data_mem_pool);
	}
	phba->device_data_mem_pool = NULL;
	return;
}

/**
 * lpfc_mem_free_all - Frees all PCI and driver memory
 * @phba: HBA to free memory for
 *
 * Description: Free memory from PCI and driver memory pools and also those
 * used : lpfc_sg_dma_buf_pool, lpfc_mbuf_pool, lpfc_hrb_pool. Frees
 * kmalloc-backed mempools for LPFC_MBOXQ_t and lpfc_nodelist. Also frees
 * the VPI bitmask.
 *
 * Returns: None
 **/
void
lpfc_mem_free_all(struct lpfc_hba *phba)
{
	struct lpfc_sli *psli = &phba->sli;
	LPFC_MBOXQ_t *mbox, *next_mbox;
	struct lpfc_dmabuf   *mp;

	/* Free memory used in mailbox queue back to mailbox memory pool */
	list_for_each_entry_safe(mbox, next_mbox, &psli->mboxq, list) {
		mp = (struct lpfc_dmabuf *)(mbox->ctx_buf);
		if (mp) {
			lpfc_mbuf_free(phba, mp->virt, mp->phys);
			kfree(mp);
		}
		list_del(&mbox->list);
		mempool_free(mbox, phba->mbox_mem_pool);
	}
	/* Free memory used in mailbox cmpl list back to mailbox memory pool */
	list_for_each_entry_safe(mbox, next_mbox, &psli->mboxq_cmpl, list) {
		mp = (struct lpfc_dmabuf *)(mbox->ctx_buf);
		if (mp) {
			lpfc_mbuf_free(phba, mp->virt, mp->phys);
			kfree(mp);
		}
		list_del(&mbox->list);
		mempool_free(mbox, phba->mbox_mem_pool);
	}
	/* Free the active mailbox command back to the mailbox memory pool */
	spin_lock_irq(&phba->hbalock);
	psli->sli_flag &= ~LPFC_SLI_MBOX_ACTIVE;
	spin_unlock_irq(&phba->hbalock);
	if (psli->mbox_active) {
		mbox = psli->mbox_active;
		mp = (struct lpfc_dmabuf *)(mbox->ctx_buf);
		if (mp) {
			lpfc_mbuf_free(phba, mp->virt, mp->phys);
			kfree(mp);
		}
		mempool_free(mbox, phba->mbox_mem_pool);
		psli->mbox_active = NULL;
	}

	/* Free and destroy all the allocated memory pools */
	lpfc_mem_free(phba);

	/* Free DMA buffer memory pool */
	dma_pool_destroy(phba->lpfc_sg_dma_buf_pool);
	phba->lpfc_sg_dma_buf_pool = NULL;

	dma_pool_destroy(phba->lpfc_cmd_rsp_buf_pool);
	phba->lpfc_cmd_rsp_buf_pool = NULL;

	/* Free the iocb lookup array */
	kfree(psli->iocbq_lookup);
	psli->iocbq_lookup = NULL;

	return;
}

/**
 * lpfc_mbuf_alloc - Allocate an mbuf from the lpfc_mbuf_pool PCI pool
 * @phba: HBA which owns the pool to allocate from
 * @mem_flags: indicates if this is a priority (MEM_PRI) allocation
 * @handle: used to return the DMA-mapped address of the mbuf
 *
 * Description: Allocates a DMA-mapped buffer from the lpfc_mbuf_pool PCI pool.
 * Allocates from generic dma_pool_alloc function first and if that fails and
 * mem_flags has MEM_PRI set (the only defined flag), returns an mbuf from the
 * HBA's pool.
 *
 * Notes: Not interrupt-safe.  Must be called with no locks held.  Takes
 * phba->hbalock.
 *
 * Returns:
 *   pointer to the allocated mbuf on success
 *   NULL on failure
 **/
void *
lpfc_mbuf_alloc(struct lpfc_hba *phba, int mem_flags, dma_addr_t *handle)
{
	struct lpfc_dma_pool *pool = &phba->lpfc_mbuf_safety_pool;
	unsigned long iflags;
	void *ret;

	ret = dma_pool_alloc(phba->lpfc_mbuf_pool, GFP_KERNEL, handle);

	spin_lock_irqsave(&phba->hbalock, iflags);
	if (!ret && (mem_flags & MEM_PRI) && pool->current_count) {
		pool->current_count--;
		ret = pool->elements[pool->current_count].virt;
		*handle = pool->elements[pool->current_count].phys;
	}
	spin_unlock_irqrestore(&phba->hbalock, iflags);
	return ret;
}

/**
 * __lpfc_mbuf_free - Free an mbuf from the lpfc_mbuf_pool PCI pool (locked)
 * @phba: HBA which owns the pool to return to
 * @virt: mbuf to free
 * @dma: the DMA-mapped address of the lpfc_mbuf_pool to be freed
 *
 * Description: Returns an mbuf lpfc_mbuf_pool to the lpfc_mbuf_safety_pool if
 * it is below its max_count, frees the mbuf otherwise.
 *
 * Notes: Must be called with phba->hbalock held to synchronize access to
 * lpfc_mbuf_safety_pool.
 *
 * Returns: None
 **/
void
__lpfc_mbuf_free(struct lpfc_hba * phba, void *virt, dma_addr_t dma)
{
	struct lpfc_dma_pool *pool = &phba->lpfc_mbuf_safety_pool;

	if (pool->current_count < pool->max_count) {
		pool->elements[pool->current_count].virt = virt;
		pool->elements[pool->current_count].phys = dma;
		pool->current_count++;
	} else {
		dma_pool_free(phba->lpfc_mbuf_pool, virt, dma);
	}
	return;
}

/**
 * lpfc_mbuf_free - Free an mbuf from the lpfc_mbuf_pool PCI pool (unlocked)
 * @phba: HBA which owns the pool to return to
 * @virt: mbuf to free
 * @dma: the DMA-mapped address of the lpfc_mbuf_pool to be freed
 *
 * Description: Returns an mbuf lpfc_mbuf_pool to the lpfc_mbuf_safety_pool if
 * it is below its max_count, frees the mbuf otherwise.
 *
 * Notes: Takes phba->hbalock.  Can be called with or without other locks held.
 *
 * Returns: None
 **/
void
lpfc_mbuf_free(struct lpfc_hba * phba, void *virt, dma_addr_t dma)
{
	unsigned long iflags;

	spin_lock_irqsave(&phba->hbalock, iflags);
	__lpfc_mbuf_free(phba, virt, dma);
	spin_unlock_irqrestore(&phba->hbalock, iflags);
	return;
}

/**
 * lpfc_nvmet_buf_alloc - Allocate an nvmet_buf from the
 * lpfc_sg_dma_buf_pool PCI pool
 * @phba: HBA which owns the pool to allocate from
 * @mem_flags: indicates if this is a priority (MEM_PRI) allocation
 * @handle: used to return the DMA-mapped address of the nvmet_buf
 *
 * Description: Allocates a DMA-mapped buffer from the lpfc_sg_dma_buf_pool
 * PCI pool.  Allocates from generic dma_pool_alloc function.
 *
 * Returns:
 *   pointer to the allocated nvmet_buf on success
 *   NULL on failure
 **/
void *
lpfc_nvmet_buf_alloc(struct lpfc_hba *phba, int mem_flags, dma_addr_t *handle)
{
	void *ret;

	ret = dma_pool_alloc(phba->lpfc_sg_dma_buf_pool, GFP_KERNEL, handle);
	return ret;
}

/**
 * lpfc_nvmet_buf_free - Free an nvmet_buf from the lpfc_sg_dma_buf_pool
 * PCI pool
 * @phba: HBA which owns the pool to return to
 * @virt: nvmet_buf to free
 * @dma: the DMA-mapped address of the lpfc_sg_dma_buf_pool to be freed
 *
 * Returns: None
 **/
void
lpfc_nvmet_buf_free(struct lpfc_hba *phba, void *virt, dma_addr_t dma)
{
	dma_pool_free(phba->lpfc_sg_dma_buf_pool, virt, dma);
}

/**
 * lpfc_els_hbq_alloc - Allocate an HBQ buffer
 * @phba: HBA to allocate HBQ buffer for
 *
 * Description: Allocates a DMA-mapped HBQ buffer from the lpfc_hrb_pool PCI
 * pool along a non-DMA-mapped container for it.
 *
 * Notes: Not interrupt-safe.  Must be called with no locks held.
 *
 * Returns:
 *   pointer to HBQ on success
 *   NULL on failure
 **/
struct hbq_dmabuf *
lpfc_els_hbq_alloc(struct lpfc_hba *phba)
{
	struct hbq_dmabuf *hbqbp;

	hbqbp = kzalloc(sizeof(struct hbq_dmabuf), GFP_KERNEL);
	if (!hbqbp)
		return NULL;

	hbqbp->dbuf.virt = dma_pool_alloc(phba->lpfc_hbq_pool, GFP_KERNEL,
					  &hbqbp->dbuf.phys);
	if (!hbqbp->dbuf.virt) {
		kfree(hbqbp);
		return NULL;
	}
	hbqbp->total_size = LPFC_BPL_SIZE;
	return hbqbp;
}

/**
 * lpfc_els_hbq_free - Frees an HBQ buffer allocated with lpfc_els_hbq_alloc
 * @phba: HBA buffer was allocated for
 * @hbqbp: HBQ container returned by lpfc_els_hbq_alloc
 *
 * Description: Frees both the container and the DMA-mapped buffer returned by
 * lpfc_els_hbq_alloc.
 *
 * Notes: Can be called with or without locks held.
 *
 * Returns: None
 **/
void
lpfc_els_hbq_free(struct lpfc_hba *phba, struct hbq_dmabuf *hbqbp)
{
	dma_pool_free(phba->lpfc_hbq_pool, hbqbp->dbuf.virt, hbqbp->dbuf.phys);
	kfree(hbqbp);
	return;
}

/**
 * lpfc_sli4_rb_alloc - Allocate an SLI4 Receive buffer
 * @phba: HBA to allocate a receive buffer for
 *
 * Description: Allocates a DMA-mapped receive buffer from the lpfc_hrb_pool PCI
 * pool along a non-DMA-mapped container for it.
 *
 * Notes: Not interrupt-safe.  Must be called with no locks held.
 *
 * Returns:
 *   pointer to HBQ on success
 *   NULL on failure
 **/
struct hbq_dmabuf *
lpfc_sli4_rb_alloc(struct lpfc_hba *phba)
{
	struct hbq_dmabuf *dma_buf;

	dma_buf = kzalloc(sizeof(struct hbq_dmabuf), GFP_KERNEL);
	if (!dma_buf)
		return NULL;

	dma_buf->hbuf.virt = dma_pool_alloc(phba->lpfc_hrb_pool, GFP_KERNEL,
					    &dma_buf->hbuf.phys);
	if (!dma_buf->hbuf.virt) {
		kfree(dma_buf);
		return NULL;
	}
	dma_buf->dbuf.virt = dma_pool_alloc(phba->lpfc_drb_pool, GFP_KERNEL,
					    &dma_buf->dbuf.phys);
	if (!dma_buf->dbuf.virt) {
		dma_pool_free(phba->lpfc_hrb_pool, dma_buf->hbuf.virt,
			      dma_buf->hbuf.phys);
		kfree(dma_buf);
		return NULL;
	}
	dma_buf->total_size = LPFC_DATA_BUF_SIZE;
	return dma_buf;
}

/**
 * lpfc_sli4_rb_free - Frees a receive buffer
 * @phba: HBA buffer was allocated for
 * @dmab: DMA Buffer container returned by lpfc_sli4_hbq_alloc
 *
 * Description: Frees both the container and the DMA-mapped buffers returned by
 * lpfc_sli4_rb_alloc.
 *
 * Notes: Can be called with or without locks held.
 *
 * Returns: None
 **/
void
lpfc_sli4_rb_free(struct lpfc_hba *phba, struct hbq_dmabuf *dmab)
{
	dma_pool_free(phba->lpfc_hrb_pool, dmab->hbuf.virt, dmab->hbuf.phys);
	dma_pool_free(phba->lpfc_drb_pool, dmab->dbuf.virt, dmab->dbuf.phys);
	kfree(dmab);
}

/**
 * lpfc_sli4_nvmet_alloc - Allocate an SLI4 Receive buffer
 * @phba: HBA to allocate a receive buffer for
 *
 * Description: Allocates a DMA-mapped receive buffer from the lpfc_hrb_pool PCI
 * pool along a non-DMA-mapped container for it.
 *
 * Returns:
 *   pointer to HBQ on success
 *   NULL on failure
 **/
struct rqb_dmabuf *
lpfc_sli4_nvmet_alloc(struct lpfc_hba *phba)
{
	struct rqb_dmabuf *dma_buf;

	dma_buf = kzalloc(sizeof(*dma_buf), GFP_KERNEL);
	if (!dma_buf)
		return NULL;

	dma_buf->hbuf.virt = dma_pool_alloc(phba->lpfc_hrb_pool, GFP_KERNEL,
					    &dma_buf->hbuf.phys);
	if (!dma_buf->hbuf.virt) {
		kfree(dma_buf);
		return NULL;
	}
	dma_buf->dbuf.virt = dma_pool_alloc(phba->lpfc_nvmet_drb_pool,
					    GFP_KERNEL, &dma_buf->dbuf.phys);
	if (!dma_buf->dbuf.virt) {
		dma_pool_free(phba->lpfc_hrb_pool, dma_buf->hbuf.virt,
			      dma_buf->hbuf.phys);
		kfree(dma_buf);
		return NULL;
	}
	dma_buf->total_size = LPFC_NVMET_DATA_BUF_SIZE;
	return dma_buf;
}

/**
 * lpfc_sli4_nvmet_free - Frees a receive buffer
 * @phba: HBA buffer was allocated for
 * @dmab: DMA Buffer container returned by lpfc_sli4_rbq_alloc
 *
 * Description: Frees both the container and the DMA-mapped buffers returned by
 * lpfc_sli4_nvmet_alloc.
 *
 * Notes: Can be called with or without locks held.
 *
 * Returns: None
 **/
void
lpfc_sli4_nvmet_free(struct lpfc_hba *phba, struct rqb_dmabuf *dmab)
{
	dma_pool_free(phba->lpfc_hrb_pool, dmab->hbuf.virt, dmab->hbuf.phys);
	dma_pool_free(phba->lpfc_nvmet_drb_pool,
		      dmab->dbuf.virt, dmab->dbuf.phys);
	kfree(dmab);
}

/**
 * lpfc_in_buf_free - Free a DMA buffer
 * @phba: HBA buffer is associated with
 * @mp: Buffer to free
 *
 * Description: Frees the given DMA buffer in the appropriate way given if the
 * HBA is running in SLI3 mode with HBQs enabled.
 *
 * Notes: Takes phba->hbalock.  Can be called with or without other locks held.
 *
 * Returns: None
 **/
void
lpfc_in_buf_free(struct lpfc_hba *phba, struct lpfc_dmabuf *mp)
{
	struct hbq_dmabuf *hbq_entry;
	unsigned long flags;

	if (!mp)
		return;

	if (phba->sli3_options & LPFC_SLI3_HBQ_ENABLED) {
		hbq_entry = container_of(mp, struct hbq_dmabuf, dbuf);
		/* Check whether HBQ is still in use */
		spin_lock_irqsave(&phba->hbalock, flags);
		if (!phba->hbq_in_use) {
			spin_unlock_irqrestore(&phba->hbalock, flags);
			return;
		}
		list_del(&hbq_entry->dbuf.list);
		if (hbq_entry->tag == -1) {
			(phba->hbqs[LPFC_ELS_HBQ].hbq_free_buffer)
				(phba, hbq_entry);
		} else {
			lpfc_sli_free_hbq(phba, hbq_entry);
		}
		spin_unlock_irqrestore(&phba->hbalock, flags);
	} else {
		lpfc_mbuf_free(phba, mp->virt, mp->phys);
		kfree(mp);
	}
	return;
}

/**
 * lpfc_rq_buf_free - Free a RQ DMA buffer
 * @phba: HBA buffer is associated with
 * @mp: Buffer to free
 *
 * Description: Frees the given DMA buffer in the appropriate way given by
 * reposting it to its associated RQ so it can be reused.
 *
 * Notes: Takes phba->hbalock.  Can be called with or without other locks held.
 *
 * Returns: None
 **/
void
lpfc_rq_buf_free(struct lpfc_hba *phba, struct lpfc_dmabuf *mp)
{
	struct lpfc_rqb *rqbp;
	struct lpfc_rqe hrqe;
	struct lpfc_rqe drqe;
	struct rqb_dmabuf *rqb_entry;
	unsigned long flags;
	int rc;

	if (!mp)
		return;

	rqb_entry = container_of(mp, struct rqb_dmabuf, hbuf);
	rqbp = rqb_entry->hrq->rqbp;

	spin_lock_irqsave(&phba->hbalock, flags);
	list_del(&rqb_entry->hbuf.list);
	hrqe.address_lo = putPaddrLow(rqb_entry->hbuf.phys);
	hrqe.address_hi = putPaddrHigh(rqb_entry->hbuf.phys);
	drqe.address_lo = putPaddrLow(rqb_entry->dbuf.phys);
	drqe.address_hi = putPaddrHigh(rqb_entry->dbuf.phys);
	rc = lpfc_sli4_rq_put(rqb_entry->hrq, rqb_entry->drq, &hrqe, &drqe);
	if (rc < 0) {
		lpfc_printf_log(phba, KERN_ERR, LOG_INIT,
				"6409 Cannot post to HRQ %d: %x %x %x "
				"DRQ %x %x\n",
				rqb_entry->hrq->queue_id,
				rqb_entry->hrq->host_index,
				rqb_entry->hrq->hba_index,
				rqb_entry->hrq->entry_count,
				rqb_entry->drq->host_index,
				rqb_entry->drq->hba_index);
		(rqbp->rqb_free_buffer)(phba, rqb_entry);
	} else {
		list_add_tail(&rqb_entry->hbuf.list, &rqbp->rqb_buffer_list);
		rqbp->buffer_count++;
	}

	spin_unlock_irqrestore(&phba->hbalock, flags);
}
