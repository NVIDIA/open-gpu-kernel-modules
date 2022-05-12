/* SPDX-License-Identifier: MIT */
#ifndef _KCL_HEADER_DRM_ENCODER_H_H_
#define _KCL_HEADER_DRM_ENCODER_H_H_

#ifdef HAVE_DRM_DRM_ENCODER_H
#include_next <drm/drm_encoder.h>
#else
#include <drm/drm_crtc.h>
#endif

#endif
