/*
 * Header file for reservations for dma-buf and ttm
 *
 * Copyright(C) 2011 Linaro Limited. All rights reserved.
 * Copyright (C) 2012-2013 Canonical Ltd
 * Copyright (C) 2012 Texas Instruments
 *
 * Authors:
 * Rob Clark <robdclark@gmail.com>
 * Maarten Lankhorst <maarten.lankhorst@canonical.com>
 * Thomas Hellstrom <thellstrom-at-vmware-dot-com>
 *
 * Based on bo.c which bears the following copyright notice,
 * but is dual licensed:
 *
 * Copyright (c) 2006-2009 VMware, Inc., Palo Alto, CA., USA
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE COPYRIGHT HOLDERS, AUTHORS AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * NOTICE:
 * THIS HEADER IS FOR DMA-RESV.H ONLY
 * DO NOT INCLUDE THIS HEADER ANY OTHER PLACE
 * INCLUDE LINUX/DMA-RESV.H OR LINUX/RESERVATION.H INSTEAD
 */
#ifndef KCL_KCL_DMA_RESV_H
#define KCL_KCL_DMA_RESV_H

#include <asm/barrier.h>
#include <kcl/backport/kcl_fence_backport.h>
#include <kcl/kcl_seqlock.h>
#include <kcl/kcl_overflow.h>

struct dma_resv_list;

#if defined(HAVE_DMA_RESV_SEQCOUNT_WW_MUTEX_T)
struct dma_resv {
	struct ww_mutex lock;
	seqcount_ww_mutex_t seq;

	struct dma_fence __rcu *fence_excl;
	struct dma_resv_list __rcu *fence;
};
#elif defined(HAVE_RESERVATION_OBJECT_STAGED)
struct dma_resv {
	struct ww_mutex lock;
	seqcount_t seq;

	struct dma_fence __rcu *fence_excl;
	struct dma_resv_list __rcu *fence;
	struct dma_resv_list *staged;
};
#else
struct dma_resv {
	struct ww_mutex lock;
	seqcount_t seq;

	struct dma_fence __rcu *fence_excl;
	struct dma_resv_list __rcu *fence;
};
#endif

#if !defined(smp_store_mb)
#define smp_store_mb set_mb
#endif
#endif
