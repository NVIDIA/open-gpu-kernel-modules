/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * helper functions for physically contiguous capture buffers
 *
 * The functions support hardware lacking scatter gather support
 * (i.e. the buffers must be linear in physical memory)
 *
 * Copyright (c) 2008 Magnus Damm
 */
#ifndef _VIDEOBUF_DMA_CONTIG_H
#define _VIDEOBUF_DMA_CONTIG_H

#include <linux/dma-mapping.h>
#include <media/videobuf-core.h>

void videobuf_queue_dma_contig_init(struct videobuf_queue *q,
				    const struct videobuf_queue_ops *ops,
				    struct device *dev,
				    spinlock_t *irqlock,
				    enum v4l2_buf_type type,
				    enum v4l2_field field,
				    unsigned int msize,
				    void *priv,
				    struct mutex *ext_lock);

dma_addr_t videobuf_to_dma_contig(struct videobuf_buffer *buf);
void videobuf_dma_contig_free(struct videobuf_queue *q,
			      struct videobuf_buffer *buf);

#endif /* _VIDEOBUF_DMA_CONTIG_H */
