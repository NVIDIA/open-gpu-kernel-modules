// SPDX-License-Identifier: GPL-2.0+

#include <drm/drm_atomic_helper.h>
#include <drm/drm_simple_kms_helper.h>
#include <drm/drm_vblank.h>

#include "amdgpu.h"
#ifdef CONFIG_DRM_AMDGPU_SI
#include "dce_v6_0.h"
#endif
#ifdef CONFIG_DRM_AMDGPU_CIK
#include "dce_v8_0.h"
#endif
#include "dce_v10_0.h"
#include "dce_v11_0.h"
#include "ivsrcid/ivsrcid_vislands30.h"
#include "amdgpu_vkms.h"
#include "amdgpu_display.h"
#include "atom.h"
#include "amdgpu_irq.h"

/**
 * DOC: amdgpu_vkms
 *
 * The amdgpu vkms interface provides a virtual KMS interface for several use
 * cases: devices without display hardware, platforms where the actual display
 * hardware is not useful (e.g., servers), SR-IOV virtual functions, device
 * emulation/simulation, and device bring up prior to display hardware being
 * usable. We previously emulated a legacy KMS interface, but there was a desire
 * to move to the atomic KMS interface. The vkms driver did everything we
 * needed, but we wanted KMS support natively in the driver without buffer
 * sharing and the ability to support an instance of VKMS per device. We first
 * looked at splitting vkms into a stub driver and a helper module that other
 * drivers could use to implement a virtual display, but this strategy ended up
 * being messy due to driver specific callbacks needed for buffer management.
 * Ultimately, it proved easier to import the vkms code as it mostly used core
 * drm helpers anyway.
 */

static const u32 amdgpu_vkms_formats[] = {
	DRM_FORMAT_XRGB8888,
};

static enum hrtimer_restart amdgpu_vkms_vblank_simulate(struct hrtimer *timer)
{
	struct amdgpu_crtc *amdgpu_crtc = container_of(timer, struct amdgpu_crtc, vblank_timer);
	struct drm_crtc *crtc = &amdgpu_crtc->base;
	struct amdgpu_device *adev = drm_to_adev(crtc->dev);
	struct amdgpu_vkms_output *output = drm_crtc_to_amdgpu_vkms_output(crtc);
	u64 ret_overrun;
	bool ret;

	ret_overrun = hrtimer_forward_now(&amdgpu_crtc->vblank_timer,
					  output->period_ns);
	if (ret_overrun != 1)
		DRM_WARN("%s: vblank timer overrun\n", __func__);

	ret = drm_crtc_handle_vblank(crtc);
	if (!ret)
		DRM_ERROR("amdgpu_vkms failure on handling vblank");

	return HRTIMER_RESTART;
}

static int amdgpu_vkms_enable_vblank(struct drm_crtc *crtc)
{
	struct drm_device *dev = crtc->dev;
	unsigned int pipe = drm_crtc_index(crtc);
	struct drm_vblank_crtc *vblank = &dev->vblank[pipe];
	struct amdgpu_vkms_output *out = drm_crtc_to_amdgpu_vkms_output(crtc);
	struct amdgpu_crtc *amdgpu_crtc = to_amdgpu_crtc(crtc);

	drm_calc_timestamping_constants(crtc, &crtc->mode);

	out->period_ns = ktime_set(0, vblank->framedur_ns);
	hrtimer_start(&amdgpu_crtc->vblank_timer, out->period_ns, HRTIMER_MODE_REL);

	return 0;
}

static void amdgpu_vkms_disable_vblank(struct drm_crtc *crtc)
{
	struct amdgpu_crtc *amdgpu_crtc = to_amdgpu_crtc(crtc);

	hrtimer_cancel(&amdgpu_crtc->vblank_timer);
}

static bool amdgpu_vkms_get_vblank_timestamp(struct drm_crtc *crtc,
					     int *max_error,
					     ktime_t *vblank_time,
					     bool in_vblank_irq)
{
	struct drm_device *dev = crtc->dev;
	unsigned int pipe = crtc->index;
	struct amdgpu_vkms_output *output = drm_crtc_to_amdgpu_vkms_output(crtc);
	struct drm_vblank_crtc *vblank = &dev->vblank[pipe];
	struct amdgpu_crtc *amdgpu_crtc = to_amdgpu_crtc(crtc);

	if (!READ_ONCE(vblank->enabled)) {
		*vblank_time = ktime_get();
		return true;
	}

	*vblank_time = READ_ONCE(amdgpu_crtc->vblank_timer.node.expires);
#if defined (HAVE_DRM_VBLANK_USE_KTIME_T)
	if (WARN_ON(ktime_to_us(*vblank_time) == ktime_to_us(vblank->time)))
#else
	if (WARN_ON(ktime_to_us(*vblank_time) == vblank->time.tv_usec))
#endif
		return true;
	/*
	 * To prevent races we roll the hrtimer forward before we do any
	 * interrupt processing - this is how real hw works (the interrupt is
	 * only generated after all the vblank registers are updated) and what
	 * the vblank core expects. Therefore we need to always correct the
	 * timestampe by one frame.
	 */

	*vblank_time  = ktime_sub(*vblank_time, output->period_ns);
	return true;
}

static const struct drm_crtc_funcs amdgpu_vkms_crtc_funcs = {
	.set_config             = drm_atomic_helper_set_config,
	.destroy                = drm_crtc_cleanup,
	.page_flip              = drm_atomic_helper_page_flip,
	.reset                  = drm_atomic_helper_crtc_reset,
	.atomic_duplicate_state = drm_atomic_helper_crtc_duplicate_state,
	.atomic_destroy_state   = drm_atomic_helper_crtc_destroy_state,
#ifdef HAVE_STRUCT_DRM_CRTC_FUNCS_GET_VBLANK_TIMESTAMP
	.enable_vblank		= amdgpu_vkms_enable_vblank,
	.disable_vblank		= amdgpu_vkms_disable_vblank,
	.get_vblank_timestamp	= amdgpu_vkms_get_vblank_timestamp,
#endif
};

static void amdgpu_vkms_crtc_atomic_enable(struct drm_crtc *crtc
#if defined(HAVE_DRM_CRTC_HELPER_FUNCS_ATOMIC_ENABLE_ARG_DRM_ATOMIC_STATE)
					  , struct drm_atomic_state *state)
#elif defined(HAVE_DRM_CRTC_HELPER_FUNCS_HAVE_ATOMIC_ENABLE)
					  , struct drm_crtc_state *state)
#else
					  )
#endif
{
	drm_crtc_vblank_on(crtc);
}

static void amdgpu_vkms_crtc_atomic_disable(struct drm_crtc *crtc,
#if defined(HAVE_DRM_CRTC_HELPER_FUNCS_ATOMIC_ENABLE_ARG_DRM_ATOMIC_STATE)
					   struct drm_atomic_state *state)
#else
					   struct drm_crtc_state *state)
#endif
{
	drm_crtc_vblank_off(crtc);
}

static void amdgpu_vkms_crtc_atomic_flush(struct drm_crtc *crtc,
#if defined(HAVE_DRM_CRTC_HELPER_FUNCS_ATOMIC_CHECK_ARG_DRM_ATOMIC_STATE)
					   struct drm_atomic_state *state)
#else
					   struct drm_crtc_state *state)
#endif
{
	unsigned long flags;
	if (crtc->state->event) {
		spin_lock_irqsave(&crtc->dev->event_lock, flags);

		if (drm_crtc_vblank_get(crtc) != 0)
			drm_crtc_send_vblank_event(crtc, crtc->state->event);
		else
			drm_crtc_arm_vblank_event(crtc, crtc->state->event);

		spin_unlock_irqrestore(&crtc->dev->event_lock, flags);

		crtc->state->event = NULL;
	}
}


static const struct drm_crtc_helper_funcs amdgpu_vkms_crtc_helper_funcs = {
	.atomic_flush	= amdgpu_vkms_crtc_atomic_flush,
#if defined(HAVE_DRM_CRTC_HELPER_FUNCS_HAVE_ATOMIC_ENABLE)
	.atomic_enable	= amdgpu_vkms_crtc_atomic_enable,
#else
	.enable = amdgpu_vkms_crtc_atomic_enable,
#endif
	.atomic_disable	= amdgpu_vkms_crtc_atomic_disable,
};

static int amdgpu_vkms_crtc_init(struct drm_device *dev, struct drm_crtc *crtc,
			  struct drm_plane *primary, struct drm_plane *cursor)
{
	struct amdgpu_device *adev = drm_to_adev(dev);
	struct amdgpu_crtc *amdgpu_crtc = to_amdgpu_crtc(crtc);
	int ret;

	ret = drm_crtc_init_with_planes(dev, crtc, primary, cursor,
					&amdgpu_vkms_crtc_funcs, NULL);
	if (ret) {
		DRM_ERROR("Failed to init CRTC\n");
		return ret;
	}

	drm_crtc_helper_add(crtc, &amdgpu_vkms_crtc_helper_funcs);

	amdgpu_crtc->crtc_id = drm_crtc_index(crtc);
	adev->mode_info.crtcs[drm_crtc_index(crtc)] = amdgpu_crtc;

	amdgpu_crtc->pll_id = ATOM_PPLL_INVALID;
	amdgpu_crtc->encoder = NULL;
	amdgpu_crtc->connector = NULL;
	amdgpu_crtc->vsync_timer_enabled = AMDGPU_IRQ_STATE_DISABLE;

	hrtimer_init(&amdgpu_crtc->vblank_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	amdgpu_crtc->vblank_timer.function = &amdgpu_vkms_vblank_simulate;

	return ret;
}

#ifdef AMDKCL_DRM_CONNECTOR_FUNCS_DPMS_MANDATORY
static int
amdgpu_vkms_connector_dpms(struct drm_connector *connector, int mode)
{
	return 0;
}


static int
amdgpu_vkms_connector_set_property(struct drm_connector *connector,
			 struct drm_property *property,
			 uint64_t val)
{
	return 0;
}
#endif

static const struct drm_connector_funcs amdgpu_vkms_connector_funcs = {
	.fill_modes = drm_helper_probe_single_connector_modes,
	.destroy = drm_connector_cleanup,
	.reset = drm_atomic_helper_connector_reset,
	.atomic_duplicate_state = drm_atomic_helper_connector_duplicate_state,
	.atomic_destroy_state = drm_atomic_helper_connector_destroy_state,
#ifdef AMDKCL_DRM_CONNECTOR_FUNCS_DPMS_MANDATORY
	.set_property = amdgpu_vkms_connector_set_property,
	.dpms = amdgpu_vkms_connector_dpms,
#endif
};

static int amdgpu_vkms_conn_get_modes(struct drm_connector *connector)
{
	struct drm_device *dev = connector->dev;
	struct drm_display_mode *mode = NULL;
	unsigned i;
	static const struct mode_size {
		int w;
		int h;
	} common_modes[] = {
		{ 640,  480},
		{ 720,  480},
		{ 800,  600},
		{ 848,  480},
		{1024,  768},
		{1152,  768},
		{1280,  720},
		{1280,  800},
		{1280,  854},
		{1280,  960},
		{1280, 1024},
		{1440,  900},
		{1400, 1050},
		{1680, 1050},
		{1600, 1200},
		{1920, 1080},
		{1920, 1200},
		{2560, 1440},
		{4096, 3112},
		{3656, 2664},
		{3840, 2160},
		{4096, 2160},
	};

	for (i = 0; i < ARRAY_SIZE(common_modes); i++) {
		mode = drm_cvt_mode(dev, common_modes[i].w, common_modes[i].h, 60, false, false, false);
		drm_mode_probed_add(connector, mode);
	}

	drm_set_preferred_mode(connector, XRES_DEF, YRES_DEF);

	return ARRAY_SIZE(common_modes);
}

static const struct drm_connector_helper_funcs amdgpu_vkms_conn_helper_funcs = {
	.get_modes    = amdgpu_vkms_conn_get_modes,
};

static const struct drm_plane_funcs amdgpu_vkms_plane_funcs = {
	.update_plane		= drm_atomic_helper_update_plane,
	.disable_plane		= drm_atomic_helper_disable_plane,
	.destroy		= drm_plane_cleanup,
	.reset			= drm_atomic_helper_plane_reset,
	.atomic_duplicate_state = drm_atomic_helper_plane_duplicate_state,
	.atomic_destroy_state	= drm_atomic_helper_plane_destroy_state,
};

static void amdgpu_vkms_plane_atomic_update(struct drm_plane *plane,
#if defined(HAVE_STRUCT_DRM_PLANE_HELPER_FUNCS_ATOMIC_CHECK_DRM_ATOMIC_STATE_PARAMS)
					    struct drm_atomic_state *old_state)
#else
					    struct drm_plane_state *old_state)
#endif
{
	return;
}

static int amdgpu_vkms_plane_atomic_check(struct drm_plane *plane,
#if defined(HAVE_STRUCT_DRM_PLANE_HELPER_FUNCS_ATOMIC_CHECK_DRM_ATOMIC_STATE_PARAMS)
					  struct drm_atomic_state *state)
{
	struct drm_plane_state *new_plane_state = kcl_drm_atomic_get_new_plane_state_before_commit(state,
										 plane);
#else
					  struct drm_plane_state *new_plane_state)
{
	struct drm_atomic_state *state = new_plane_state->state;
#endif
	struct drm_crtc_state *crtc_state;
	int ret;

	if (!new_plane_state->fb || WARN_ON(!new_plane_state->crtc))
		return 0;

	crtc_state = drm_atomic_get_crtc_state(state,
					       new_plane_state->crtc);

	if (IS_ERR(crtc_state))
		return PTR_ERR(crtc_state);

	ret = drm_atomic_helper_check_plane_state(new_plane_state, crtc_state,
						  DRM_PLANE_HELPER_NO_SCALING,
						  DRM_PLANE_HELPER_NO_SCALING,
						  false, true);
	if (ret != 0)
		return ret;

	/* for now primary plane must be visible and full screen */
	if (!new_plane_state->visible)
		return -EINVAL;

	return 0;
}

static int amdgpu_vkms_prepare_fb(struct drm_plane *plane,
				  struct drm_plane_state *new_state)
{
	struct amdgpu_framebuffer *afb;
	struct drm_gem_object *obj;
	struct amdgpu_device *adev;
	struct amdgpu_bo *rbo;
	struct list_head list;
	struct ttm_validate_buffer tv;
	struct ww_acquire_ctx ticket;
	uint32_t domain;
	int r;

	if (!new_state->fb) {
		DRM_DEBUG_KMS("No FB bound\n");
		return 0;
	}
	afb = to_amdgpu_framebuffer(new_state->fb);
	obj = drm_gem_fb_get_obj(new_state->fb,0);
	rbo = gem_to_amdgpu_bo(obj);
	adev = amdgpu_ttm_adev(rbo->tbo.bdev);
	INIT_LIST_HEAD(&list);

	tv.bo = &rbo->tbo;
	tv.num_shared = 1;
	list_add(&tv.head, &list);

	r = ttm_eu_reserve_buffers(&ticket, &list, false, NULL);
	if (r) {
		dev_err(adev->dev, "fail to reserve bo (%d)\n", r);
		return r;
	}

	if (plane->type != DRM_PLANE_TYPE_CURSOR)
		domain = amdgpu_display_supported_domains(adev, rbo->flags);
	else
		domain = AMDGPU_GEM_DOMAIN_VRAM;

	r = amdgpu_bo_pin(rbo, domain);
	if (unlikely(r != 0)) {
		if (r != -ERESTARTSYS)
			DRM_ERROR("Failed to pin framebuffer with error %d\n", r);
		ttm_eu_backoff_reservation(&ticket, &list);
		return r;
	}

	r = amdgpu_ttm_alloc_gart(&rbo->tbo);
	if (unlikely(r != 0)) {
		amdgpu_bo_unpin(rbo);
		ttm_eu_backoff_reservation(&ticket, &list);
		DRM_ERROR("%p bind failed\n", rbo);
		return r;
	}

	ttm_eu_backoff_reservation(&ticket, &list);

	afb->address = amdgpu_bo_gpu_offset(rbo);

	amdgpu_bo_ref(rbo);

	return 0;
}

static void amdgpu_vkms_cleanup_fb(struct drm_plane *plane,
				   struct drm_plane_state *old_state)
{
	struct amdgpu_bo *rbo;
	struct drm_gem_object *obj;
	int r;

	if (!old_state->fb)
		return;

	obj = drm_gem_fb_get_obj(old_state->fb,0);
	rbo = gem_to_amdgpu_bo(obj);
	r = amdgpu_bo_reserve(rbo, false);
	if (unlikely(r)) {
		DRM_ERROR("failed to reserve rbo before unpin\n");
		return;
	}

	amdgpu_bo_unpin(rbo);
	amdgpu_bo_unreserve(rbo);
	amdgpu_bo_unref(&rbo);
}

static const struct drm_plane_helper_funcs amdgpu_vkms_primary_helper_funcs = {
	.atomic_update		= amdgpu_vkms_plane_atomic_update,
	.atomic_check		= amdgpu_vkms_plane_atomic_check,
	.prepare_fb		= amdgpu_vkms_prepare_fb,
	.cleanup_fb		= amdgpu_vkms_cleanup_fb,
};

static struct drm_plane *amdgpu_vkms_plane_init(struct drm_device *dev,
						enum drm_plane_type type,
						int index)
{
	struct drm_plane *plane;
	int ret;

	plane = kzalloc(sizeof(*plane), GFP_KERNEL);
	if (!plane)
		return ERR_PTR(-ENOMEM);

	ret = drm_universal_plane_init(dev, plane, 1 << index,
				       &amdgpu_vkms_plane_funcs,
				       amdgpu_vkms_formats,
				       ARRAY_SIZE(amdgpu_vkms_formats),
				       NULL, type, NULL);
	if (ret) {
		kfree(plane);
		return ERR_PTR(ret);
	}

	drm_plane_helper_add(plane, &amdgpu_vkms_primary_helper_funcs);

	return plane;
}

static int amdgpu_vkms_output_init(struct drm_device *dev, struct
				   amdgpu_vkms_output *output, int index)
{
	struct drm_connector *connector = &output->connector;
	struct drm_encoder *encoder = &output->encoder;
	struct drm_crtc *crtc = &output->crtc.base;
	struct drm_plane *primary, *cursor = NULL;
	int ret;

	primary = amdgpu_vkms_plane_init(dev, DRM_PLANE_TYPE_PRIMARY, index);
	if (IS_ERR(primary))
		return PTR_ERR(primary);

	ret = amdgpu_vkms_crtc_init(dev, crtc, primary, cursor);
	if (ret)
		goto err_crtc;

	ret = drm_connector_init(dev, connector, &amdgpu_vkms_connector_funcs,
				 DRM_MODE_CONNECTOR_VIRTUAL);
	if (ret) {
		DRM_ERROR("Failed to init connector\n");
		goto err_connector;
	}

	drm_connector_helper_add(connector, &amdgpu_vkms_conn_helper_funcs);

	ret = drm_simple_encoder_init(dev, encoder, DRM_MODE_ENCODER_VIRTUAL);
	if (ret) {
		DRM_ERROR("Failed to init encoder\n");
		goto err_encoder;
	}
	encoder->possible_crtcs = 1 << index;

	ret = drm_connector_attach_encoder(connector, encoder);
	if (ret) {
		DRM_ERROR("Failed to attach connector to encoder\n");
		goto err_attach;
	}

	drm_mode_config_reset(dev);

	return 0;

err_attach:
	drm_encoder_cleanup(encoder);

err_encoder:
	drm_connector_cleanup(connector);

err_connector:
	drm_crtc_cleanup(crtc);

err_crtc:
	drm_plane_cleanup(primary);

	return ret;
}

#ifndef HAVE_STRUCT_DRM_CRTC_FUNCS_GET_VBLANK_TIMESTAMP
static u32 amdgpu_vkms_vblank_get_counter(struct amdgpu_device *adev, int crtc)
{
	return 0;
}

static void amdgpu_vkms_page_flip(struct amdgpu_device *adev,
			      int crtc_id, u64 crtc_base, bool async)
{
	return;
}

static int amdgpu_vkms_crtc_get_scanoutpos(struct amdgpu_device *adev, int crtc,
					u32 *vbl, u32 *position)
{
	*vbl = 0;
	*position = 0;

	return -EINVAL;
}

static bool amdgpu_vkms_hpd_sense(struct amdgpu_device *adev,
			       enum amdgpu_hpd_id hpd)
{
	return true;
}

static void amdgpu_vkms_hpd_set_polarity(struct amdgpu_device *adev,
				      enum amdgpu_hpd_id hpd)
{
	return;
}

static u32 amdgpu_vkms_hpd_get_gpio_reg(struct amdgpu_device *adev)
{
	return 0;
}

static void amdgpu_vkms_bandwidth_update(struct amdgpu_device *adev)
{
	return;
}

static const struct amdgpu_display_funcs amdgpu_vkms_display_funcs = {
	.bandwidth_update = &amdgpu_vkms_bandwidth_update,
	.vblank_get_counter = &amdgpu_vkms_vblank_get_counter,
	.backlight_set_level = NULL,
	.backlight_get_level = NULL,
	.hpd_sense = &amdgpu_vkms_hpd_sense,
	.hpd_set_polarity = &amdgpu_vkms_hpd_set_polarity,
	.hpd_get_gpio_reg = &amdgpu_vkms_hpd_get_gpio_reg,
	.page_flip = &amdgpu_vkms_page_flip,
	.page_flip_get_scanoutpos = &amdgpu_vkms_crtc_get_scanoutpos,
	.add_encoder = NULL,
	.add_connector = NULL,
};

static int amdgpu_vkms_set_crtc_irq_state(struct amdgpu_device *adev,
					  struct amdgpu_irq_src *source,
					  unsigned type,
					  enum amdgpu_interrupt_state state)
{
	if (type > AMDGPU_CRTC_IRQ_VBLANK6)
		return -EINVAL;

	if (type >= adev->mode_info.num_crtc || !adev->mode_info.crtcs[type]) {
		DRM_DEBUG("invalid crtc %d\n", type);
		return -EINVAL;
	}

	adev->mode_info.crtcs[type]->vsync_timer_enabled = state;

	if (state == AMDGPU_IRQ_STATE_ENABLE)
		amdgpu_vkms_enable_vblank(&adev->mode_info.crtcs[type]->base);
	else
		amdgpu_vkms_disable_vblank(&adev->mode_info.crtcs[type]->base);

	return 0;
}

static const struct amdgpu_irq_src_funcs amdgpu_vkms_crtc_irq_funcs = {
	.set = amdgpu_vkms_set_crtc_irq_state,
	.process = NULL,
};

static int amdgpu_vkms_early_init(void *handle)
{
	struct amdgpu_device *adev = (struct amdgpu_device *)handle;

	adev->crtc_irq.num_types = adev->mode_info.num_crtc;
	adev->crtc_irq.funcs = &amdgpu_vkms_crtc_irq_funcs;

	adev->mode_info.funcs = &amdgpu_vkms_display_funcs;
	adev->mode_info.num_hpd = 1;
	adev->mode_info.num_dig = 1;
	return 0;
}
#endif

const struct drm_mode_config_funcs amdgpu_vkms_mode_funcs = {
	.fb_create = amdgpu_display_user_framebuffer_create,
	.atomic_check = drm_atomic_helper_check,
	.atomic_commit = drm_atomic_helper_commit,
};

static int amdgpu_vkms_sw_init(void *handle)
{
	int r, i;
	struct amdgpu_device *adev = (struct amdgpu_device *)handle;

	adev_to_drm(adev)->max_vblank_count = 0;

	adev_to_drm(adev)->mode_config.funcs = &amdgpu_vkms_mode_funcs;

	adev_to_drm(adev)->mode_config.max_width = XRES_MAX;
	adev_to_drm(adev)->mode_config.max_height = YRES_MAX;

	adev_to_drm(adev)->mode_config.preferred_depth = 24;
	adev_to_drm(adev)->mode_config.prefer_shadow = 1;

	adev_to_drm(adev)->mode_config.fb_base = adev->gmc.aper_base;

	r = amdgpu_display_modeset_create_props(adev);
	if (r)
		return r;

	adev->amdgpu_vkms_output = kcalloc(adev->mode_info.num_crtc, sizeof(struct amdgpu_vkms_output), GFP_KERNEL);

	/* allocate crtcs, encoders, connectors */
	for (i = 0; i < adev->mode_info.num_crtc; i++) {
		r = amdgpu_vkms_output_init(adev_to_drm(adev), &adev->amdgpu_vkms_output[i], i);
		if (r)
			return r;
	}

	drm_kms_helper_poll_init(adev_to_drm(adev));

	adev->mode_info.mode_config_initialized = true;
	return 0;
}

static int amdgpu_vkms_sw_fini(void *handle)
{
	struct amdgpu_device *adev = (struct amdgpu_device *)handle;
	int i = 0;

	for (i = 0; i < adev->mode_info.num_crtc; i++)
		if (adev->mode_info.crtcs[i])
			hrtimer_cancel(&adev->mode_info.crtcs[i]->vblank_timer);

	drm_kms_helper_poll_fini(adev_to_drm(adev));
	drm_mode_config_cleanup(adev_to_drm(adev));

	adev->mode_info.mode_config_initialized = false;

	kfree(adev->mode_info.bios_hardcoded_edid);
	kfree(adev->amdgpu_vkms_output);
	return 0;
}

static int amdgpu_vkms_hw_init(void *handle)
{
	struct amdgpu_device *adev = (struct amdgpu_device *)handle;

	switch (adev->asic_type) {
#ifdef CONFIG_DRM_AMDGPU_SI
	case CHIP_TAHITI:
	case CHIP_PITCAIRN:
	case CHIP_VERDE:
	case CHIP_OLAND:
		dce_v6_0_disable_dce(adev);
		break;
#endif
#ifdef CONFIG_DRM_AMDGPU_CIK
	case CHIP_BONAIRE:
	case CHIP_HAWAII:
	case CHIP_KAVERI:
	case CHIP_KABINI:
	case CHIP_MULLINS:
		dce_v8_0_disable_dce(adev);
		break;
#endif
	case CHIP_FIJI:
	case CHIP_TONGA:
		dce_v10_0_disable_dce(adev);
		break;
	case CHIP_CARRIZO:
	case CHIP_STONEY:
	case CHIP_POLARIS10:
	case CHIP_POLARIS11:
	case CHIP_VEGAM:
		dce_v11_0_disable_dce(adev);
		break;
	case CHIP_TOPAZ:
#ifdef CONFIG_DRM_AMDGPU_SI
	case CHIP_HAINAN:
#endif
		/* no DCE */
		break;
	default:
		break;
	}
	return 0;
}

static int amdgpu_vkms_hw_fini(void *handle)
{
	return 0;
}

static int amdgpu_vkms_suspend(void *handle)
{
	struct amdgpu_device *adev = (struct amdgpu_device *)handle;
	int r;

	r = drm_mode_config_helper_suspend(adev_to_drm(adev));
	if (r)
		return r;
	return amdgpu_vkms_hw_fini(handle);
}

static int amdgpu_vkms_resume(void *handle)
{
	struct amdgpu_device *adev = (struct amdgpu_device *)handle;
	int r;

	r = amdgpu_vkms_hw_init(handle);
	if (r)
		return r;
	return drm_mode_config_helper_resume(adev_to_drm(adev));
}

static bool amdgpu_vkms_is_idle(void *handle)
{
	return true;
}

static int amdgpu_vkms_wait_for_idle(void *handle)
{
	return 0;
}

static int amdgpu_vkms_soft_reset(void *handle)
{
	return 0;
}

static int amdgpu_vkms_set_clockgating_state(void *handle,
					  enum amd_clockgating_state state)
{
	return 0;
}

static int amdgpu_vkms_set_powergating_state(void *handle,
					  enum amd_powergating_state state)
{
	return 0;
}

static const struct amd_ip_funcs amdgpu_vkms_ip_funcs = {
	.name = "amdgpu_vkms",
#ifndef HAVE_STRUCT_DRM_CRTC_FUNCS_GET_VBLANK_TIMESTAMP
	.early_init = amdgpu_vkms_early_init,
#endif
	.late_init = NULL,
	.sw_init = amdgpu_vkms_sw_init,
	.sw_fini = amdgpu_vkms_sw_fini,
	.hw_init = amdgpu_vkms_hw_init,
	.hw_fini = amdgpu_vkms_hw_fini,
	.suspend = amdgpu_vkms_suspend,
	.resume = amdgpu_vkms_resume,
	.is_idle = amdgpu_vkms_is_idle,
	.wait_for_idle = amdgpu_vkms_wait_for_idle,
	.soft_reset = amdgpu_vkms_soft_reset,
	.set_clockgating_state = amdgpu_vkms_set_clockgating_state,
	.set_powergating_state = amdgpu_vkms_set_powergating_state,
};

const struct amdgpu_ip_block_version amdgpu_vkms_ip_block =
{
	.type = AMD_IP_BLOCK_TYPE_DCE,
	.major = 1,
	.minor = 0,
	.rev = 0,
	.funcs = &amdgpu_vkms_ip_funcs,
};

