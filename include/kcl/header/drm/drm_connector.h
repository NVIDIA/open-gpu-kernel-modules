/* SPDX-License-Identifier: MIT */
#ifndef _KCL_HEADER_DRM_CONNECTOR_H_H_
#define _KCL_HEADER_DRM_CONNECTOR_H_H_

#ifdef HAVE_DRM_DRM_CONNECTOR_H
#include_next <drm/drm_connector.h>
#else
#include <drm/drm_crtc.h>
#endif

#endif
