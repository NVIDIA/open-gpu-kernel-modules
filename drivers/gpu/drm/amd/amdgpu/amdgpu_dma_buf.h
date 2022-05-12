/*
 * Copyright 2019 Advanced Micro Devices, Inc.
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
 */
#ifndef __AMDGPU_DMA_BUF_H__
#define __AMDGPU_DMA_BUF_H__

#include <drm/drm_gem.h>

#if !defined(HAVE_DMA_BUF_OPS_DYNAMIC_MAPPING) && \
	!defined(HAVE_STRUCT_DMA_BUF_OPS_PIN)
struct sg_table *amdgpu_gem_prime_get_sg_table(struct drm_gem_object *obj);
struct drm_gem_object *
amdgpu_gem_prime_import_sg_table(struct drm_device *dev,
				 struct dma_buf_attachment *attach,
				 struct sg_table *sg);
#endif

#ifdef HAVE_DRM_DRV_GEM_PRIME_EXPORT_PI
struct dma_buf *amdgpu_gem_prime_export(struct drm_gem_object *gobj,
#else
struct dma_buf *amdgpu_gem_prime_export(struct drm_device *dev,
					struct drm_gem_object *gobj,
#endif
					int flags);
#if defined(AMDKCL_AMDGPU_DMABUF_OPS)
struct drm_gem_object *amdgpu_gem_prime_import(struct drm_device *dev,
					    struct dma_buf *dma_buf);
#else
int amdgpu_gem_prime_pin(struct drm_gem_object *obj);
void amdgpu_gem_prime_unpin(struct drm_gem_object *obj);
#endif
bool amdgpu_dmabuf_is_xgmi_accessible(struct amdgpu_device *adev,
				      struct amdgpu_bo *bo);
#ifdef HAVE_DRM_DRIVER_GEM_PRIME_RES_OBJ
struct reservation_object *amdgpu_gem_prime_res_obj(struct drm_gem_object *);
#endif

#ifdef HAVE_STRUCT_DRM_DRV_GEM_OPEN_OBJECT_CALLBACK
int amdgpu_gem_prime_mmap(struct drm_gem_object *obj,
                          struct vm_area_struct *vma);
#endif


extern const struct dma_buf_ops amdgpu_dmabuf_ops;

#endif
