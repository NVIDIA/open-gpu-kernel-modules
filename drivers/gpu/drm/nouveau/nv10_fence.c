/*
 * Copyright 2012 Red Hat Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors: Ben Skeggs <bskeggs@redhat.com>
 */
#include "nouveau_drv.h"
#include "nouveau_dma.h"
#include "nv10_fence.h"

#include <nvif/push006c.h>

#include <nvhw/class/cl006e.h>

int
nv10_fence_emit(struct nouveau_fence *fence)
{
	struct nvif_push *push = fence->channel->chan.push;
	int ret = PUSH_WAIT(push, 2);
	if (ret == 0) {
		PUSH_MTHD(push, NV06E, SET_REFERENCE, fence->base.seqno);
		PUSH_KICK(push);
	}
	return ret;
}


static int
nv10_fence_sync(struct nouveau_fence *fence,
		struct nouveau_channel *prev, struct nouveau_channel *chan)
{
	return -ENODEV;
}

u32
nv10_fence_read(struct nouveau_channel *chan)
{
	return NVIF_RD32(&chan->user, NV06E, REFERENCE);
}

void
nv10_fence_context_del(struct nouveau_channel *chan)
{
	struct nv10_fence_chan *fctx = chan->fence;
	nouveau_fence_context_del(&fctx->base);
	nvif_object_dtor(&fctx->sema);
	chan->fence = NULL;
	nouveau_fence_context_free(&fctx->base);
}

static int
nv10_fence_context_new(struct nouveau_channel *chan)
{
	struct nv10_fence_chan *fctx;

	fctx = chan->fence = kzalloc(sizeof(*fctx), GFP_KERNEL);
	if (!fctx)
		return -ENOMEM;

	nouveau_fence_context_new(chan, &fctx->base);
	fctx->base.emit = nv10_fence_emit;
	fctx->base.read = nv10_fence_read;
	fctx->base.sync = nv10_fence_sync;
	return 0;
}

void
nv10_fence_destroy(struct nouveau_drm *drm)
{
	struct nv10_fence_priv *priv = drm->fence;
	nouveau_bo_unmap(priv->bo);
	if (priv->bo)
		nouveau_bo_unpin(priv->bo);
	nouveau_bo_ref(NULL, &priv->bo);
	drm->fence = NULL;
	kfree(priv);
}

int
nv10_fence_create(struct nouveau_drm *drm)
{
	struct nv10_fence_priv *priv;

	priv = drm->fence = kzalloc(sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->base.dtor = nv10_fence_destroy;
	priv->base.context_new = nv10_fence_context_new;
	priv->base.context_del = nv10_fence_context_del;
	spin_lock_init(&priv->lock);
	return 0;
}
