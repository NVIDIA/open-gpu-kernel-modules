/* SPDX-License-Identifier: MIT */
#ifndef _KCL_DRM_DP_HELPER_BACKPORT_H_
#define _KCL_DRM_DP_HELPER_BACKPORT_H_

#include <kcl/kcl_drm_dp_helper.h>
#include <drm/drm_dp_helper.h>

/*
 * commit v4.19-rc1-100-g5ce70c799ac2
 * drm_dp_cec: check that aux has a transfer function
 */
#if defined(AMDKCL_DRM_DP_CEC_XXX_CHECK_CB)
#define drm_dp_cec_irq _kcl_drm_dp_cec_irq
#define drm_dp_cec_set_edid _kcl_drm_dp_cec_set_edid
#define drm_dp_cec_unset_edid _kcl_drm_dp_cec_unset_edid
#endif

#if !defined(HAVE_DRM_DP_CEC_REGISTER_CONNECTOR_PP)
#define drm_dp_cec_register_connector _kcl_drm_dp_cec_register_connector
#endif
#endif
