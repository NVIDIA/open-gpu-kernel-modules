/* SPDX-License-Identifier: MIT */
#ifndef AMDKCL_DRM_MM_H
#define AMDKCL_DRM_MM_H

/**
 * interface change in mainline kernel 4.10
 * v4.10-rc5-1060-g4e64e5539d15 drm: Improve drm_mm search (and fix topdown allocation) 
 * with rbtrees
 */

#include <kcl/kcl_drm_mm.h>

#ifndef HAVE_DRM_MM_INSERT_NODE_THREE_PARAMETERS
static inline int _kcl_drm_mm_insert_node(struct drm_mm *mm,
				          struct drm_mm_node *node,
				          u64 size)
{
        return drm_mm_insert_node(mm, node, size, 0, DRM_MM_SEARCH_DEFAULT);
}
#define drm_mm_insert_node _kcl_drm_mm_insert_node
#endif /* HAVE_DRM_MM_INSERT_NODE_THREE_PARAMETERS */

#ifndef HAVE_DRM_MM_INSERT_MODE
#define drm_mm_insert_node_in_range _kcl_drm_mm_insert_node_in_range
#endif

#endif /* AMDKCL_DRM_MM_H */
