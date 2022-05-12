/* SPDX-License-Identifier: MIT */
#ifndef _KCL_HEADER_DRM_VBLANK_H_H_
#define _KCL_HEADER_DRM_VBLANK_H_H_

#ifdef HAVE_DRM_DRM_VBLANK_H
#include_next <drm/drm_vblank.h>
#else
#include <drm/drm_irq.h>
#endif

#endif
