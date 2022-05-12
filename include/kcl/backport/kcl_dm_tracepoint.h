/* SPDX-License-Identifier: GPL-2.0 */
#ifndef KCL_BACKPORT_KCL_DM_TRACEPOINT_H
#define KCL_BACKPORT_KCL_DM_TRACEPOINT_H

#ifndef DECLARE_EVENT_NOP
#define DECLARE_EVENT_NOP(name, proto, args)				\
	static inline void trace_##name(proto)				\
	{ }								\
	static inline bool trace_##name##_enabled(void)			\
	{								\
		return false;						\
	}

#define TRACE_EVENT_NOP(name, proto, args, struct, assign, print)	\
	DECLARE_EVENT_NOP(name, PARAMS(proto), PARAMS(args))

#define DEFINE_EVENT_NOP(template, name, proto, args)			\
	DECLARE_EVENT_NOP(name, PARAMS(proto), PARAMS(args))
#endif

#ifndef HAVE_DRM_VRR_SUPPORTED
TRACE_EVENT_NOP(amdgpu_dm_connector_atomic_check,
	    TP_PROTO(const struct drm_connector_state *state),
	    TP_ARGS(state), NULL, NULL, NULL);
TRACE_EVENT_NOP(amdgpu_dm_crtc_atomic_check,
	    TP_PROTO(const struct drm_crtc_state *state),
	    TP_ARGS(state), NULL, NULL, NULL);
DEFINE_EVENT_NOP(amdgpu_dm_plane_state_template, amdgpu_dm_plane_atomic_check,
	     TP_PROTO(const struct drm_plane_state *state),
	     TP_ARGS(state));

DEFINE_EVENT_NOP(amdgpu_dm_plane_state_template, amdgpu_dm_atomic_update_cursor,
	     TP_PROTO(const struct drm_plane_state *state),
	     TP_ARGS(state));

DEFINE_EVENT_NOP(amdgpu_dm_atomic_state_template, amdgpu_dm_atomic_commit_tail_begin,
	     TP_PROTO(const struct drm_atomic_state *state),
	     TP_ARGS(state));

DEFINE_EVENT_NOP(amdgpu_dm_atomic_state_template, amdgpu_dm_atomic_commit_tail_finish,
	     TP_PROTO(const struct drm_atomic_state *state),
	     TP_ARGS(state));

DEFINE_EVENT_NOP(amdgpu_dm_atomic_state_template, amdgpu_dm_atomic_check_begin,
	     TP_PROTO(const struct drm_atomic_state *state),
	     TP_ARGS(state));

TRACE_EVENT_NOP(amdgpu_dm_atomic_check_finish,
	    TP_PROTO(const struct drm_atomic_state *state, int res),
	    TP_ARGS(state, res), NULL, NULL, NULL);
#endif /* HAVE_DRM_VRR_SUPPORTED */
#endif /* KCL_BACKPORT_KCL_DM_TRACEPOINT_H */
