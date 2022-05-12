/* SPDX-License-Identifier: MIT */
#ifndef AMDKCL_DRM_CACHE_BACKPORT_H
#define AMDKCL_DRM_CACHE_BACKPORT_H

#include <drm/drm_cache.h>
#include <kcl/kcl_drm_cache.h>

#define drm_arch_can_wc_memory kcl_drm_arch_can_wc_memory

#endif	/* AMDKCL_DRM_CACHE_BACKPORT_H */
