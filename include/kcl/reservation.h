/* SPDX-License-Identifier: GPL-2.0 */
#ifndef KCL_RESERVATION_H
#define KCL_RESERVATION_H

#ifndef HAVE_LINUX_DMA_RESV_H
#include <kcl/kcl_reservation.h>

#if defined(HAVE_RESERVATION_OBJECT_STAGED)
static inline void
_kcl_reservation_object_fini(struct reservation_object *obj)
{
	dma_resv_fini(obj);
	kfree(obj->staged);
}
#define amddma_resv_fini _kcl_reservation_object_fini

void _kcl_dma_resv_add_shared_fence(struct dma_resv *obj, struct dma_fence *fence);
#define amddma_resv_add_shared_fence _kcl_dma_resv_add_shared_fence

int _kcl_dma_resv_copy_fences(struct dma_resv *dst, struct dma_resv *src);
#define amddma_resv_copy_fences _kcl_dma_resv_copy_fences

#endif /* HAVE_RESERVATION_OBJECT_STAGED */
#endif /* HAVE_LINUX_DMA_RESV_H */

#endif
