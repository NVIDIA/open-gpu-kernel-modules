/*
 * Copyright © 2014 Red Hat
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the name of the copyright holders not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  The copyright holders make no representations
 * about the suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */
#ifndef _KCL_DRM_DP_MST_HELPER_BACKPORT_H_
#define _KCL_DRM_DP_MST_HELPER_BACKPORT_H_

#include <drm/drm_dp_mst_helper.h>

/* Copied from drivers/gpu/drm/drm_dp_mst_topology.c and modified for KCL */
#if !defined(HAVE_DRM_DP_CALC_PBN_MODE_3ARGS)
static inline
int _kcl_drm_dp_calc_pbn_mode(int clock, int bpp, bool dsc)
{
#if defined(HAVE_MUL_U32_U32)
	if (dsc)
		return DIV_ROUND_UP_ULL(mul_u32_u32(clock * (bpp / 16), 64 * 1006),
				8 * 54 * 1000 * 1000);
#endif

	return drm_dp_calc_pbn_mode(clock, bpp);
}
#define drm_dp_calc_pbn_mode _kcl_drm_dp_calc_pbn_mode
#endif

#if defined(HAVE_DRM_DP_ATOMIC_FIND_VCPI_SLOTS)
#if !defined(HAVE_DRM_DP_ATOMIC_FIND_VCPI_SLOTS_5ARGS)
static inline
int _kcl_drm_dp_atomic_find_vcpi_slots(struct drm_atomic_state *state,
				  struct drm_dp_mst_topology_mgr *mgr,
				  struct drm_dp_mst_port *port, int pbn,
				  int pbn_div)
{
	int pbn_backup;
	int req_slots;

	if (pbn_div > 0) {
		pbn_backup = mgr->pbn_div;
		mgr->pbn_div = pbn_div;
	}

	req_slots = drm_dp_atomic_find_vcpi_slots(state, mgr, port, pbn);

	if (pbn_div > 0)
		mgr->pbn_div = pbn_backup;

	return req_slots;
}
#define drm_dp_atomic_find_vcpi_slots _kcl_drm_dp_atomic_find_vcpi_slots
#endif /* HAVE_DRM_DP_ATOMIC_FIND_VCPI_SLOTS_5ARGS */
#endif /* HAVE_DRM_DP_ATOMIC_FIND_VCPI_SLOTS */

#ifndef HAVE_DRM_DP_MST_TOPOLOGY_MGR_RESUME_2ARGS
static inline int
_kcl_drm_dp_mst_topology_mgr_resume(struct drm_dp_mst_topology_mgr *mgr,
			       bool sync)
{
	return drm_dp_mst_topology_mgr_resume(mgr);
}
#define drm_dp_mst_topology_mgr_resume _kcl_drm_dp_mst_topology_mgr_resume
#endif

#endif
