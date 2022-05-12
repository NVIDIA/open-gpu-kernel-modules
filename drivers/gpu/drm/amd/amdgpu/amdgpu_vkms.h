/* SPDX-License-Identifier: GPL-2.0+ */

#ifndef _AMDGPU_VKMS_H_
#define _AMDGPU_VKMS_H_

#if defined(HAVE_DRM_NONBLOCKING_COMMIT_SUPPORT)
#define XRES_DEF  1024
#define YRES_DEF   768

#define XRES_MAX  16384
#define YRES_MAX  16384

#define drm_crtc_to_amdgpu_vkms_output(target) \
	container_of(target, struct amdgpu_vkms_output, crtc.base)

extern const struct amdgpu_ip_block_version amdgpu_vkms_ip_block;

struct amdgpu_vkms_output {
	struct amdgpu_crtc crtc;
	struct drm_encoder encoder;
	struct drm_connector connector;
	ktime_t period_ns;
	struct drm_pending_vblank_event *event;
};

#else
extern const struct amdgpu_ip_block_version amdgpu_vkms_ip_block;
#endif

#endif /* _AMDGPU_VKMS_H_ */
