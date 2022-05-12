/*
 * Copyright 2007-8 Advanced Micro Devices, Inc.
 * Copyright 2008 Red Hat Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors: Dave Airlie
 *          Alex Deucher
 */

#include <drm/amdgpu_drm.h>
#include "amdgpu.h"
#include "amdgpu_i2c.h"
#include "atom.h"
#include "amdgpu_connectors.h"
#include "amdgpu_display.h"
#include <asm/div64.h>

#include <linux/pci.h>
#include <linux/pm_runtime.h>
#include <drm/drm_crtc_helper.h>
#include <drm/drm_edid.h>
#include <drm/drm_gem_framebuffer_helper.h>
#include <drm/drm_fb_helper.h>
#include <drm/drm_fourcc.h>
#include <drm/drm_vblank.h>

static void amdgpu_display_flip_callback(struct dma_fence *f,
					 struct dma_fence_cb *cb)
{
	struct amdgpu_flip_work *work =
		container_of(cb, struct amdgpu_flip_work, cb);

	dma_fence_put(f);
#ifdef HAVE_STRUCT_DRM_CRTC_FUNCS_PAGE_FLIP_TARGET
	schedule_work(&work->flip_work.work);
#else
	schedule_work(&work->flip_work);
#endif
}

static bool amdgpu_display_flip_handle_fence(struct amdgpu_flip_work *work,
					     struct dma_fence **f)
{
	struct dma_fence *fence= *f;

	if (fence == NULL)
		return false;

	*f = NULL;

	if (!dma_fence_add_callback(fence, &work->cb,
				    amdgpu_display_flip_callback))
		return true;

	dma_fence_put(fence);
	return false;
}

#if !defined(HAVE_STRUCT_DRM_CRTC_FUNCS_PAGE_FLIP_TARGET)
static void amdgpu_flip_work_func(struct work_struct *__work)
{
	struct amdgpu_flip_work *work =
		container_of(__work, struct amdgpu_flip_work, flip_work);
	struct amdgpu_device *adev = work->adev;
	struct amdgpu_crtc *amdgpuCrtc = adev->mode_info.crtcs[work->crtc_id];

	struct drm_crtc *crtc = &amdgpuCrtc->base;
	unsigned long flags;
	unsigned i, repcnt = 4;
	int vpos, hpos, stat, min_udelay = 0;
	struct drm_vblank_crtc *vblank = &crtc->dev->vblank[work->crtc_id];

	if (amdgpu_display_flip_handle_fence(work, &work->excl))
		return;

	for (i = 0; i < work->shared_count; ++i)
		if (amdgpu_display_flip_handle_fence(work, &work->shared[i]))
			return;

	/* We borrow the event spin lock for protecting flip_status */
	spin_lock_irqsave(&crtc->dev->event_lock, flags);

	/* If this happens to execute within the "virtually extended" vblank
	 * interval before the start of the real vblank interval then it needs
	 * to delay programming the mmio flip until the real vblank is entered.
	 * This prevents completing a flip too early due to the way we fudge
	 * our vblank counter and vblank timestamps in order to work around the
	 * problem that the hw fires vblank interrupts before actual start of
	 * vblank (when line buffer refilling is done for a frame). It
	 * complements the fudging logic in amdgpu_display_get_crtc_scanoutpos() for
	 * timestamping and amdgpu_get_vblank_counter_kms() for vblank counts.
	 *
	 * In practice this won't execute very often unless on very fast
	 * machines because the time window for this to happen is very small.
	 */
	while (amdgpuCrtc->enabled && --repcnt) {
		/* GET_DISTANCE_TO_VBLANKSTART returns distance to real vblank
		 * start in hpos, and to the "fudged earlier" vblank start in
		 * vpos.
		 */
		stat = amdgpu_display_get_crtc_scanoutpos(adev_to_drm(adev), work->crtc_id,
						  GET_DISTANCE_TO_VBLANKSTART,
						  &vpos, &hpos, NULL, NULL,
						  &crtc->hwmode);

		if ((stat & (DRM_SCANOUTPOS_VALID | DRM_SCANOUTPOS_ACCURATE)) !=
		    (DRM_SCANOUTPOS_VALID | DRM_SCANOUTPOS_ACCURATE) ||
		    !(vpos >= 0 && hpos <= 0))
			break;

		/* Sleep at least until estimated real start of hw vblank */
		min_udelay = (-hpos + 1) * max(vblank->linedur_ns / 1000, 5);
		if (min_udelay > vblank->framedur_ns / 2000) {
			/* Don't wait ridiculously long - something is wrong */
			repcnt = 0;
			break;
		}
		spin_unlock_irqrestore(&crtc->dev->event_lock, flags);
		usleep_range(min_udelay, 2 * min_udelay);
		spin_lock_irqsave(&crtc->dev->event_lock, flags);
	}

	if (!repcnt)
		DRM_DEBUG_DRIVER("Delay problem on crtc %d: min_udelay %d, "
				 "framedur %d, linedur %d, stat %d, vpos %d, "
				 "hpos %d\n", work->crtc_id, min_udelay,
				 vblank->framedur_ns / 1000,
				 vblank->linedur_ns / 1000, stat, vpos, hpos);

	/* Do the flip (mmio) */
	adev->mode_info.funcs->page_flip(adev, work->crtc_id, work->base, work->async);

	/* Set the flip status */
	amdgpuCrtc->pflip_status = AMDGPU_FLIP_SUBMITTED;
	spin_unlock_irqrestore(&crtc->dev->event_lock, flags);


	DRM_DEBUG_DRIVER("crtc:%d[%p], pflip_stat:AMDGPU_FLIP_SUBMITTED, work: %p,\n",
					 amdgpuCrtc->crtc_id, amdgpuCrtc, work);
}
#else
static void amdgpu_display_flip_work_func(struct work_struct *__work)
{
	struct delayed_work *delayed_work =
		container_of(__work, struct delayed_work, work);
	struct amdgpu_flip_work *work =
		container_of(delayed_work, struct amdgpu_flip_work, flip_work);
	struct amdgpu_device *adev = work->adev;
	struct amdgpu_crtc *amdgpu_crtc = adev->mode_info.crtcs[work->crtc_id];

	struct drm_crtc *crtc = &amdgpu_crtc->base;
	unsigned long flags;
	unsigned i;
	int vpos, hpos;

	if (amdgpu_display_flip_handle_fence(work, &work->excl))
		return;

	for (i = 0; i < work->shared_count; ++i)
		if (amdgpu_display_flip_handle_fence(work, &work->shared[i]))
			return;

	/* Wait until we're out of the vertical blank period before the one
	 * targeted by the flip
	 */
	if (amdgpu_crtc->enabled &&
	    (amdgpu_display_get_crtc_scanoutpos(adev_to_drm(adev), work->crtc_id, 0,
						&vpos, &hpos, NULL, NULL,
						&crtc->hwmode)
	     & (DRM_SCANOUTPOS_VALID | DRM_SCANOUTPOS_IN_VBLANK)) ==
	    (DRM_SCANOUTPOS_VALID | DRM_SCANOUTPOS_IN_VBLANK) &&
	    (int)(work->target_vblank -
		  amdgpu_get_vblank_counter_kms(crtc)) > 0) {
		schedule_delayed_work(&work->flip_work, usecs_to_jiffies(1000));
		return;
	}

	/* We borrow the event spin lock for protecting flip_status */
	spin_lock_irqsave(&crtc->dev->event_lock, flags);

	/* Do the flip (mmio) */
	adev->mode_info.funcs->page_flip(adev, work->crtc_id, work->base, work->async);

	/* Set the flip status */
	amdgpu_crtc->pflip_status = AMDGPU_FLIP_SUBMITTED;
	spin_unlock_irqrestore(&crtc->dev->event_lock, flags);


	DRM_DEBUG_DRIVER("crtc:%d[%p], pflip_stat:AMDGPU_FLIP_SUBMITTED, work: %p,\n",
					 amdgpu_crtc->crtc_id, amdgpu_crtc, work);

}
#endif

/*
 * Handle unpin events outside the interrupt handler proper.
 */
static void amdgpu_display_unpin_work_func(struct work_struct *__work)
{
	struct amdgpu_flip_work *work =
		container_of(__work, struct amdgpu_flip_work, unpin_work);
	int r;

	/* unpin of the old buffer */
	r = amdgpu_bo_reserve(work->old_abo, true);
	if (likely(r == 0)) {
		amdgpu_bo_unpin(work->old_abo);
		amdgpu_bo_unreserve(work->old_abo);
	} else
		DRM_ERROR("failed to reserve buffer after flip\n");

	amdgpu_bo_unref(&work->old_abo);
	kfree(work->shared);
	kfree(work);
}

#ifdef HAVE_STRUCT_DRM_CRTC_FUNCS_PAGE_FLIP_TARGET
int amdgpu_display_crtc_page_flip_target(struct drm_crtc *crtc,
				struct drm_framebuffer *fb,
				struct drm_pending_vblank_event *event,
#ifdef HAVE_STRUCT_DRM_CRTC_FUNCS_PAGE_FLIP_TARGET_CTX
				uint32_t page_flip_flags, uint32_t target,
				struct drm_modeset_acquire_ctx *ctx)
#else
				uint32_t page_flip_flags, uint32_t target)
#endif
{
	struct drm_device *dev = crtc->dev;
	struct amdgpu_device *adev = drm_to_adev(dev);
	struct amdgpu_crtc *amdgpu_crtc = to_amdgpu_crtc(crtc);
	struct drm_gem_object *obj;
	struct amdgpu_flip_work *work;
	struct amdgpu_bo *new_abo;
	unsigned long flags;
	u64 tiling_flags;
	int i, r;

	work = kzalloc(sizeof *work, GFP_KERNEL);
	if (work == NULL)
		return -ENOMEM;

	INIT_DELAYED_WORK(&work->flip_work, amdgpu_display_flip_work_func);
	INIT_WORK(&work->unpin_work, amdgpu_display_unpin_work_func);

	work->event = event;
	work->adev = adev;
	work->crtc_id = amdgpu_crtc->crtc_id;
	work->async = (page_flip_flags & DRM_MODE_PAGE_FLIP_ASYNC) != 0;

	/* schedule unpin of the old buffer */

	obj = drm_gem_fb_get_obj(crtc->primary->fb, 0);
	/* take a reference to the old object */
	work->old_abo = gem_to_amdgpu_bo(obj);
	amdgpu_bo_ref(work->old_abo);

	obj = drm_gem_fb_get_obj(fb, 0);
	new_abo = gem_to_amdgpu_bo(obj);

	/* pin the new buffer */
	r = amdgpu_bo_reserve(new_abo, false);
	if (unlikely(r != 0)) {
		DRM_ERROR("failed to reserve new abo buffer before flip\n");
		goto cleanup;
	}

	if (!adev->enable_virtual_display) {
		r = amdgpu_bo_pin(new_abo,
				  amdgpu_display_supported_domains(adev, new_abo->flags));
		if (unlikely(r != 0)) {
			DRM_ERROR("failed to pin new abo buffer before flip\n");
			goto unreserve;
		}
	}

	r = amdgpu_ttm_alloc_gart(&new_abo->tbo);
	if (unlikely(r != 0)) {
		DRM_ERROR("%p bind failed\n", new_abo);
		goto unpin;
	}

	r = dma_resv_get_fences(amdkcl_ttm_resvp(&new_abo->tbo), &work->excl,
				&work->shared_count, &work->shared);
	if (unlikely(r != 0)) {
		DRM_ERROR("failed to get fences for buffer\n");
		goto unpin;
	}

	amdgpu_bo_get_tiling_flags(new_abo, &tiling_flags);
	amdgpu_bo_unreserve(new_abo);

	if (!adev->enable_virtual_display)
		work->base = amdgpu_bo_gpu_offset(new_abo);
	work->target_vblank = target - (uint32_t)drm_crtc_vblank_count(crtc) +
		amdgpu_get_vblank_counter_kms(crtc);

	/* we borrow the event spin lock for protecting flip_wrok */
	spin_lock_irqsave(&crtc->dev->event_lock, flags);
	if (amdgpu_crtc->pflip_status != AMDGPU_FLIP_NONE) {
		DRM_DEBUG_DRIVER("flip queue: crtc already busy\n");
		spin_unlock_irqrestore(&crtc->dev->event_lock, flags);
		r = -EBUSY;
		goto pflip_cleanup;
	}

	amdgpu_crtc->pflip_status = AMDGPU_FLIP_PENDING;
	amdgpu_crtc->pflip_works = work;


	DRM_DEBUG_DRIVER("crtc:%d[%p], pflip_stat:AMDGPU_FLIP_PENDING, work: %p,\n",
					 amdgpu_crtc->crtc_id, amdgpu_crtc, work);
	/* update crtc fb */
	crtc->primary->fb = fb;
	spin_unlock_irqrestore(&crtc->dev->event_lock, flags);
	amdgpu_display_flip_work_func(&work->flip_work.work);
	return 0;

pflip_cleanup:
	if (unlikely(amdgpu_bo_reserve(new_abo, false) != 0)) {
		DRM_ERROR("failed to reserve new abo in error path\n");
		goto cleanup;
	}
unpin:
	if (!adev->enable_virtual_display)
		amdgpu_bo_unpin(new_abo);

unreserve:
	amdgpu_bo_unreserve(new_abo);

cleanup:
	amdgpu_bo_unref(&work->old_abo);
	dma_fence_put(work->excl);
	for (i = 0; i < work->shared_count; ++i)
		dma_fence_put(work->shared[i]);
	kfree(work->shared);
	kfree(work);

	return r;
}
#else
int amdgpu_crtc_page_flip(struct drm_crtc *crtc,
			  struct drm_framebuffer *fb,
			  struct drm_pending_vblank_event *event,
			  uint32_t page_flip_flags)
{
	struct drm_device *dev = crtc->dev;
	struct amdgpu_device *adev = drm_to_adev(dev);
	struct amdgpu_crtc *amdgpu_crtc = to_amdgpu_crtc(crtc);
	struct amdgpu_framebuffer *old_amdgpu_fb;
	struct amdgpu_framebuffer *new_amdgpu_fb;
	struct drm_gem_object *obj;
	struct amdgpu_flip_work *work;
	struct amdgpu_bo *new_abo;
	unsigned long flags;
	u64 tiling_flags;
	u64 base;
	int i, r;

	work = kzalloc(sizeof *work, GFP_KERNEL);
	if (work == NULL)
		return -ENOMEM;

	INIT_WORK(&work->flip_work, amdgpu_flip_work_func);
	INIT_WORK(&work->unpin_work, amdgpu_display_unpin_work_func);

	work->event = event;
	work->adev = adev;
	work->crtc_id = amdgpu_crtc->crtc_id;
	work->async = (page_flip_flags & DRM_MODE_PAGE_FLIP_ASYNC) != 0;

	/* schedule unpin of the old buffer */
	old_amdgpu_fb = to_amdgpu_framebuffer(crtc->primary->fb);
	obj = old_amdgpu_fb->obj;

	/* take a reference to the old object */
	work->old_abo = gem_to_amdgpu_bo(obj);
	amdgpu_bo_ref(work->old_abo);

	new_amdgpu_fb = to_amdgpu_framebuffer(fb);
	obj = new_amdgpu_fb->obj;
	new_abo = gem_to_amdgpu_bo(obj);

	/* pin the new buffer */
	r = amdgpu_bo_reserve(new_abo, false);
	if (unlikely(r != 0)) {
		DRM_ERROR("failed to reserve new abo buffer before flip\n");
		goto cleanup;
	}

	r = amdgpu_bo_pin(new_abo, AMDGPU_GEM_DOMAIN_VRAM);
	if (unlikely(r != 0)) {
		r = -EINVAL;
		DRM_ERROR("failed to pin new abo buffer before flip\n");
		goto unreserve;
	}

	r = dma_resv_get_fences(amdkcl_ttm_resvp(&new_abo->tbo), &work->excl,
					      &work->shared_count,
					      &work->shared);
	if (unlikely(r != 0)) {
		DRM_ERROR("failed to get fences for buffer\n");
		goto unpin;
	}

	amdgpu_bo_get_tiling_flags(new_abo, &tiling_flags);
	amdgpu_bo_unreserve(new_abo);

	work->base = base;

	r = drm_crtc_vblank_get(crtc);
	if (r) {
		DRM_ERROR("failed to get vblank before flip\n");
		goto pflip_cleanup;
	}

	/* we borrow the event spin lock for protecting flip_wrok */
	spin_lock_irqsave(&crtc->dev->event_lock, flags);
	if (amdgpu_crtc->pflip_status != AMDGPU_FLIP_NONE) {
		DRM_DEBUG_DRIVER("flip queue: crtc already busy\n");
		spin_unlock_irqrestore(&crtc->dev->event_lock, flags);
		r = -EBUSY;
		goto vblank_cleanup;
	}

	amdgpu_crtc->pflip_status = AMDGPU_FLIP_PENDING;
	amdgpu_crtc->pflip_works = work;


	DRM_DEBUG_DRIVER("crtc:%d[%p], pflip_stat:AMDGPU_FLIP_PENDING, work: %p,\n",
					 amdgpu_crtc->crtc_id, amdgpu_crtc, work);
	/* update crtc fb */
	crtc->primary->fb = fb;
	spin_unlock_irqrestore(&crtc->dev->event_lock, flags);
	amdgpu_flip_work_func(&work->flip_work);
	return 0;

vblank_cleanup:
	drm_crtc_vblank_put(crtc);

pflip_cleanup:
	if (unlikely(amdgpu_bo_reserve(new_abo, false) != 0)) {
		DRM_ERROR("failed to reserve new abo in error path\n");
		goto cleanup;
	}
unpin:
	amdgpu_bo_unpin(new_abo);

unreserve:
	amdgpu_bo_unreserve(new_abo);

cleanup:
	amdgpu_bo_unref(&work->old_abo);
	fence_put(work->excl);
	for (i = 0; i < work->shared_count; ++i)
		fence_put(work->shared[i]);
	kfree(work->shared);
	kfree(work);

	return r;
}
#endif

#ifdef HAVE_STRUCT_DRM_CRTC_FUNCS_SET_CONFIG_CTX
int amdgpu_display_crtc_set_config(struct drm_mode_set *set,
				   struct drm_modeset_acquire_ctx *ctx)
#else
int amdgpu_display_crtc_set_config(struct drm_mode_set *set)
#endif
{
	struct drm_device *dev;
	struct amdgpu_device *adev;
	struct drm_crtc *crtc;
	bool active = false;
	int ret;

	if (!set || !set->crtc)
		return -EINVAL;

	dev = set->crtc->dev;

	ret = pm_runtime_get_sync(dev->dev);
	if (ret < 0)
		goto out;

#ifdef HAVE_STRUCT_DRM_CRTC_FUNCS_SET_CONFIG_CTX
	ret = drm_crtc_helper_set_config(set, ctx);
#else
	ret = drm_crtc_helper_set_config(set);
#endif

	list_for_each_entry(crtc, &dev->mode_config.crtc_list, head)
		if (crtc->enabled)
			active = true;

	pm_runtime_mark_last_busy(dev->dev);

	adev = drm_to_adev(dev);
	/* if we have active crtcs and we don't have a power ref,
	   take the current one */
	if (active && !adev->have_disp_power_ref) {
		adev->have_disp_power_ref = true;
		return ret;
	}
	/* if we have no active crtcs, then drop the power ref
	   we got before */
	if (!active && adev->have_disp_power_ref) {
		pm_runtime_put_autosuspend(dev->dev);
		adev->have_disp_power_ref = false;
	}

out:
	/* drop the power reference we got coming in here */
	pm_runtime_put_autosuspend(dev->dev);
	return ret;
}

static const char *encoder_names[41] = {
	"NONE",
	"INTERNAL_LVDS",
	"INTERNAL_TMDS1",
	"INTERNAL_TMDS2",
	"INTERNAL_DAC1",
	"INTERNAL_DAC2",
	"INTERNAL_SDVOA",
	"INTERNAL_SDVOB",
	"SI170B",
	"CH7303",
	"CH7301",
	"INTERNAL_DVO1",
	"EXTERNAL_SDVOA",
	"EXTERNAL_SDVOB",
	"TITFP513",
	"INTERNAL_LVTM1",
	"VT1623",
	"HDMI_SI1930",
	"HDMI_INTERNAL",
	"INTERNAL_KLDSCP_TMDS1",
	"INTERNAL_KLDSCP_DVO1",
	"INTERNAL_KLDSCP_DAC1",
	"INTERNAL_KLDSCP_DAC2",
	"SI178",
	"MVPU_FPGA",
	"INTERNAL_DDI",
	"VT1625",
	"HDMI_SI1932",
	"DP_AN9801",
	"DP_DP501",
	"INTERNAL_UNIPHY",
	"INTERNAL_KLDSCP_LVTMA",
	"INTERNAL_UNIPHY1",
	"INTERNAL_UNIPHY2",
	"NUTMEG",
	"TRAVIS",
	"INTERNAL_VCE",
	"INTERNAL_UNIPHY3",
	"HDMI_ANX9805",
	"INTERNAL_AMCLK",
	"VIRTUAL",
};

static const char *hpd_names[6] = {
	"HPD1",
	"HPD2",
	"HPD3",
	"HPD4",
	"HPD5",
	"HPD6",
};

void amdgpu_display_print_display_setup(struct drm_device *dev)
{
	struct drm_connector *connector;
	struct amdgpu_connector *amdgpu_connector;
	struct drm_encoder *encoder;
	struct amdgpu_encoder *amdgpu_encoder;
#ifdef HAVE_DRM_CONNECTOR_LIST_ITER_BEGIN
	struct drm_connector_list_iter iter;
#endif
	uint32_t devices;
	int i = 0;

	DRM_INFO("AMDGPU Display Connectors\n");
#ifdef HAVE_DRM_CONNECTOR_LIST_ITER_BEGIN
	drm_connector_list_iter_begin(dev, &iter);
	drm_for_each_connector_iter(connector, &iter) {
#else
	list_for_each_entry(connector, &(dev)->mode_config.connector_list, head) {
#endif
		amdgpu_connector = to_amdgpu_connector(connector);
		DRM_INFO("Connector %d:\n", i);
		DRM_INFO("  %s\n", connector->name);
		if (amdgpu_connector->hpd.hpd != AMDGPU_HPD_NONE)
			DRM_INFO("  %s\n", hpd_names[amdgpu_connector->hpd.hpd]);
		if (amdgpu_connector->ddc_bus) {
			DRM_INFO("  DDC: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
				 amdgpu_connector->ddc_bus->rec.mask_clk_reg,
				 amdgpu_connector->ddc_bus->rec.mask_data_reg,
				 amdgpu_connector->ddc_bus->rec.a_clk_reg,
				 amdgpu_connector->ddc_bus->rec.a_data_reg,
				 amdgpu_connector->ddc_bus->rec.en_clk_reg,
				 amdgpu_connector->ddc_bus->rec.en_data_reg,
				 amdgpu_connector->ddc_bus->rec.y_clk_reg,
				 amdgpu_connector->ddc_bus->rec.y_data_reg);
			if (amdgpu_connector->router.ddc_valid)
				DRM_INFO("  DDC Router 0x%x/0x%x\n",
					 amdgpu_connector->router.ddc_mux_control_pin,
					 amdgpu_connector->router.ddc_mux_state);
			if (amdgpu_connector->router.cd_valid)
				DRM_INFO("  Clock/Data Router 0x%x/0x%x\n",
					 amdgpu_connector->router.cd_mux_control_pin,
					 amdgpu_connector->router.cd_mux_state);
		} else {
			if (connector->connector_type == DRM_MODE_CONNECTOR_VGA ||
			    connector->connector_type == DRM_MODE_CONNECTOR_DVII ||
			    connector->connector_type == DRM_MODE_CONNECTOR_DVID ||
			    connector->connector_type == DRM_MODE_CONNECTOR_DVIA ||
			    connector->connector_type == DRM_MODE_CONNECTOR_HDMIA ||
			    connector->connector_type == DRM_MODE_CONNECTOR_HDMIB)
				DRM_INFO("  DDC: no ddc bus - possible BIOS bug - please report to xorg-driver-ati@lists.x.org\n");
		}
		DRM_INFO("  Encoders:\n");
		list_for_each_entry(encoder, &dev->mode_config.encoder_list, head) {
			amdgpu_encoder = to_amdgpu_encoder(encoder);
			devices = amdgpu_encoder->devices & amdgpu_connector->devices;
			if (devices) {
				if (devices & ATOM_DEVICE_CRT1_SUPPORT)
					DRM_INFO("    CRT1: %s\n", encoder_names[amdgpu_encoder->encoder_id]);
				if (devices & ATOM_DEVICE_CRT2_SUPPORT)
					DRM_INFO("    CRT2: %s\n", encoder_names[amdgpu_encoder->encoder_id]);
				if (devices & ATOM_DEVICE_LCD1_SUPPORT)
					DRM_INFO("    LCD1: %s\n", encoder_names[amdgpu_encoder->encoder_id]);
				if (devices & ATOM_DEVICE_DFP1_SUPPORT)
					DRM_INFO("    DFP1: %s\n", encoder_names[amdgpu_encoder->encoder_id]);
				if (devices & ATOM_DEVICE_DFP2_SUPPORT)
					DRM_INFO("    DFP2: %s\n", encoder_names[amdgpu_encoder->encoder_id]);
				if (devices & ATOM_DEVICE_DFP3_SUPPORT)
					DRM_INFO("    DFP3: %s\n", encoder_names[amdgpu_encoder->encoder_id]);
				if (devices & ATOM_DEVICE_DFP4_SUPPORT)
					DRM_INFO("    DFP4: %s\n", encoder_names[amdgpu_encoder->encoder_id]);
				if (devices & ATOM_DEVICE_DFP5_SUPPORT)
					DRM_INFO("    DFP5: %s\n", encoder_names[amdgpu_encoder->encoder_id]);
				if (devices & ATOM_DEVICE_DFP6_SUPPORT)
					DRM_INFO("    DFP6: %s\n", encoder_names[amdgpu_encoder->encoder_id]);
				if (devices & ATOM_DEVICE_TV1_SUPPORT)
					DRM_INFO("    TV1: %s\n", encoder_names[amdgpu_encoder->encoder_id]);
				if (devices & ATOM_DEVICE_CV_SUPPORT)
					DRM_INFO("    CV: %s\n", encoder_names[amdgpu_encoder->encoder_id]);
			}
		}
		i++;
	}
#ifdef HAVE_DRM_CONNECTOR_LIST_ITER_BEGIN
	drm_connector_list_iter_end(&iter);
#endif
}

bool amdgpu_display_ddc_probe(struct amdgpu_connector *amdgpu_connector,
			      bool use_aux)
{
	u8 out = 0x0;
	u8 buf[8];
	int ret;
	struct i2c_msg msgs[] = {
		{
			.addr = DDC_ADDR,
			.flags = 0,
			.len = 1,
			.buf = &out,
		},
		{
			.addr = DDC_ADDR,
			.flags = I2C_M_RD,
			.len = 8,
			.buf = buf,
		}
	};

	/* on hw with routers, select right port */
	if (amdgpu_connector->router.ddc_valid)
		amdgpu_i2c_router_select_ddc_port(amdgpu_connector);

	if (use_aux) {
		ret = i2c_transfer(&amdgpu_connector->ddc_bus->aux.ddc, msgs, 2);
	} else {
		ret = i2c_transfer(&amdgpu_connector->ddc_bus->adapter, msgs, 2);
	}

	if (ret != 2)
		/* Couldn't find an accessible DDC on this connector */
		return false;
	/* Probe also for valid EDID header
	 * EDID header starts with:
	 * 0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00.
	 * Only the first 6 bytes must be valid as
	 * drm_edid_block_valid() can fix the last 2 bytes */
	if (drm_edid_header_is_valid(buf) < 6) {
		/* Couldn't find an accessible EDID on this
		 * connector */
		return false;
	}
	return true;
}

static const struct drm_framebuffer_funcs amdgpu_fb_funcs = {
	.destroy = drm_gem_fb_destroy,
	.create_handle = drm_gem_fb_create_handle,
};

uint32_t amdgpu_display_supported_domains(struct amdgpu_device *adev,
					  uint64_t bo_flags)
{
	uint32_t domain = AMDGPU_GEM_DOMAIN_VRAM;

#if defined(CONFIG_DRM_AMD_DC)
	/*
	 * if amdgpu_bo_support_uswc returns false it means that USWC mappings
	 * is not supported for this board. But this mapping is required
	 * to avoid hang caused by placement of scanout BO in GTT on certain
	 * APUs. So force the BO placement to VRAM in case this architecture
	 * will not allow USWC mappings.
	 * Also, don't allow GTT domain if the BO doesn't have USWC flag set.
	 */
	if ((bo_flags & AMDGPU_GEM_CREATE_CPU_GTT_USWC) &&
	    amdgpu_bo_support_uswc(bo_flags) &&
	    amdgpu_device_asic_has_dc_support(adev->asic_type)) {
		switch (adev->asic_type) {
		case CHIP_CARRIZO:
		case CHIP_STONEY:
			domain |= AMDGPU_GEM_DOMAIN_GTT;
			break;
		default:
			switch (adev->ip_versions[DCE_HWIP][0]) {
			case IP_VERSION(1, 0, 0):
			case IP_VERSION(1, 0, 1):
				/* enable S/G on PCO and RV2 */
				if ((adev->apu_flags & AMD_APU_IS_RAVEN2) ||
				    (adev->apu_flags & AMD_APU_IS_PICASSO))
					domain |= AMDGPU_GEM_DOMAIN_GTT;
				break;
			case IP_VERSION(2, 1, 0):
			case IP_VERSION(3, 0, 1):
			case IP_VERSION(3, 1, 2):
			case IP_VERSION(3, 1, 3):
				domain |= AMDGPU_GEM_DOMAIN_GTT;
				break;
			default:
				break;
			}
			break;
		}
	}
#endif

	return domain;
}

/*
 * Tries to extract the renderable DCC offset from the opaque metadata attached
 * to the buffer.
 */
static int
extract_render_dcc_offset(struct amdgpu_device *adev,
			  struct drm_gem_object *obj,
			  uint64_t *offset)
{
	struct amdgpu_bo *rbo;
	int r = 0;
	uint32_t metadata[10]; /* Something that fits a descriptor + header. */
	uint32_t size;

	rbo = gem_to_amdgpu_bo(obj);
	r = amdgpu_bo_reserve(rbo, false);

	if (unlikely(r)) {
		/* Don't show error message when returning -ERESTARTSYS */
		if (r != -ERESTARTSYS)
			DRM_ERROR("Unable to reserve buffer: %d\n", r);
		return r;
	}

	r = amdgpu_bo_get_metadata(rbo, metadata, sizeof(metadata), &size, NULL);
	amdgpu_bo_unreserve(rbo);

	if (r)
		return r;

	/*
	 * The first word is the metadata version, and we need space for at least
	 * the version + pci vendor+device id + 8 words for a descriptor.
	 */
	if (size < 40  || metadata[0] != 1)
		return -EINVAL;

	if (adev->family >= AMDGPU_FAMILY_NV) {
		/* resource word 6/7 META_DATA_ADDRESS{_LO} */
		*offset = ((u64)metadata[9] << 16u) |
			  ((metadata[8] & 0xFF000000u) >> 16);
	} else {
		/* resource word 5/7 META_DATA_ADDRESS */
		*offset = ((u64)metadata[9] << 8u) |
			  ((u64)(metadata[7] & 0x1FE0000u) << 23);
	}

	return 0;
}

#ifdef HAVE_DRM_FORMAT_INFO_MODIFIER_SUPPORTED
static int convert_tiling_flags_to_modifier(struct amdgpu_framebuffer *afb)
{
	struct amdgpu_device *adev = drm_to_adev(afb->base.dev);
	uint64_t modifier = 0;

	if (!afb->tiling_flags || !AMDGPU_TILING_GET(afb->tiling_flags, SWIZZLE_MODE)) {
		modifier = DRM_FORMAT_MOD_LINEAR;
	} else {
		int swizzle = AMDGPU_TILING_GET(afb->tiling_flags, SWIZZLE_MODE);
		bool has_xor = swizzle >= 16;
		int block_size_bits;
		int version;
		int pipe_xor_bits = 0;
		int bank_xor_bits = 0;
		int packers = 0;
		int rb = 0;
		int pipes = ilog2(adev->gfx.config.gb_addr_config_fields.num_pipes);
		uint32_t dcc_offset = AMDGPU_TILING_GET(afb->tiling_flags, DCC_OFFSET_256B);

		switch (swizzle >> 2) {
		case 0: /* 256B */
			block_size_bits = 8;
			break;
		case 1: /* 4KiB */
		case 5: /* 4KiB _X */
			block_size_bits = 12;
			break;
		case 2: /* 64KiB */
		case 4: /* 64 KiB _T */
		case 6: /* 64 KiB _X */
			block_size_bits = 16;
			break;
		default:
			/* RESERVED or VAR */
			return -EINVAL;
		}

		if (adev->asic_type >= CHIP_SIENNA_CICHLID)
			version = AMD_FMT_MOD_TILE_VER_GFX10_RBPLUS;
		else if (adev->family == AMDGPU_FAMILY_NV)
			version = AMD_FMT_MOD_TILE_VER_GFX10;
		else
			version = AMD_FMT_MOD_TILE_VER_GFX9;

		switch (swizzle & 3) {
		case 0: /* Z microtiling */
			return -EINVAL;
		case 1: /* S microtiling */
			if (!has_xor)
				version = AMD_FMT_MOD_TILE_VER_GFX9;
			break;
		case 2:
			if (!has_xor && afb->base.format->cpp[0] != 4)
				version = AMD_FMT_MOD_TILE_VER_GFX9;
			break;
		case 3:
			break;
		}

		if (has_xor) {
			switch (version) {
			case AMD_FMT_MOD_TILE_VER_GFX10_RBPLUS:
				pipe_xor_bits = min(block_size_bits - 8, pipes);
				packers = min(block_size_bits - 8 - pipe_xor_bits,
					      ilog2(adev->gfx.config.gb_addr_config_fields.num_pkrs));
				break;
			case AMD_FMT_MOD_TILE_VER_GFX10:
				pipe_xor_bits = min(block_size_bits - 8, pipes);
				break;
			case AMD_FMT_MOD_TILE_VER_GFX9:
				rb = ilog2(adev->gfx.config.gb_addr_config_fields.num_se) +
				     ilog2(adev->gfx.config.gb_addr_config_fields.num_rb_per_se);
				pipe_xor_bits = min(block_size_bits - 8, pipes +
						    ilog2(adev->gfx.config.gb_addr_config_fields.num_se));
				bank_xor_bits = min(block_size_bits - 8 - pipe_xor_bits,
						    ilog2(adev->gfx.config.gb_addr_config_fields.num_banks));
				break;
			}
		}

		modifier = AMD_FMT_MOD |
			   AMD_FMT_MOD_SET(TILE, AMDGPU_TILING_GET(afb->tiling_flags, SWIZZLE_MODE)) |
			   AMD_FMT_MOD_SET(TILE_VERSION, version) |
			   AMD_FMT_MOD_SET(PIPE_XOR_BITS, pipe_xor_bits) |
			   AMD_FMT_MOD_SET(BANK_XOR_BITS, bank_xor_bits) |
			   AMD_FMT_MOD_SET(PACKERS, packers);

		if (dcc_offset != 0) {
			bool dcc_i64b = AMDGPU_TILING_GET(afb->tiling_flags, DCC_INDEPENDENT_64B) != 0;
			bool dcc_i128b = version >= AMD_FMT_MOD_TILE_VER_GFX10_RBPLUS;
			u64 render_dcc_offset;

			/* Enable constant encode on RAVEN2 and later. */
			bool dcc_constant_encode = adev->asic_type > CHIP_RAVEN ||
						   (adev->asic_type == CHIP_RAVEN &&
						    adev->external_rev_id >= 0x81);

			int max_cblock_size = dcc_i64b ? AMD_FMT_MOD_DCC_BLOCK_64B :
					      dcc_i128b ? AMD_FMT_MOD_DCC_BLOCK_128B :
					      AMD_FMT_MOD_DCC_BLOCK_256B;

			modifier |= AMD_FMT_MOD_SET(DCC, 1) |
				    AMD_FMT_MOD_SET(DCC_CONSTANT_ENCODE, dcc_constant_encode) |
				    AMD_FMT_MOD_SET(DCC_INDEPENDENT_64B, dcc_i64b) |
				    AMD_FMT_MOD_SET(DCC_INDEPENDENT_128B, dcc_i128b) |
				    AMD_FMT_MOD_SET(DCC_MAX_COMPRESSED_BLOCK, max_cblock_size);

			afb->base.offsets[1] = dcc_offset * 256 + afb->base.offsets[0];
			afb->base.pitches[1] =
				AMDGPU_TILING_GET(afb->tiling_flags, DCC_PITCH_MAX) + 1;

			/*
			 * If the userspace driver uses retiling the tiling flags do not contain
			 * info on the renderable DCC buffer. Luckily the opaque metadata contains
			 * the info so we can try to extract it. The kernel does not use this info
			 * but we should convert it to a modifier plane for getfb2, so the
			 * userspace driver that gets it doesn't have to juggle around another DCC
			 * plane internally.
			 */
			if (extract_render_dcc_offset(adev, afb->base.obj[0],
						      &render_dcc_offset) == 0 &&
			    render_dcc_offset != 0 &&
			    render_dcc_offset != afb->base.offsets[1] &&
			    render_dcc_offset < UINT_MAX) {
				uint32_t dcc_block_bits;  /* of base surface data */

				modifier |= AMD_FMT_MOD_SET(DCC_RETILE, 1);
				afb->base.offsets[2] = render_dcc_offset;

				if (adev->family >= AMDGPU_FAMILY_NV) {
					int extra_pipe = 0;

					if (adev->asic_type >= CHIP_SIENNA_CICHLID &&
					    pipes == packers && pipes > 1)
						extra_pipe = 1;

					dcc_block_bits = max(20, 16 + pipes + extra_pipe);
				} else {
					modifier |= AMD_FMT_MOD_SET(RB, rb) |
						    AMD_FMT_MOD_SET(PIPE, pipes);
					dcc_block_bits = max(20, 18 + rb);
				}

				dcc_block_bits -= ilog2(afb->base.format->cpp[0]);
				afb->base.pitches[2] = ALIGN(afb->base.width,
							     1u << ((dcc_block_bits + 1) / 2));
			}
		}
	}

	afb->base.modifier = modifier;
	afb->base.flags |= DRM_MODE_FB_MODIFIERS;
	return 0;
}
#endif

#ifdef HAVE_DRM_FORMAT_INFO_MODIFIER_SUPPORTED
static void get_block_dimensions(unsigned int block_log2, unsigned int cpp,
				 unsigned int *width, unsigned int *height)
{
	unsigned int cpp_log2 = ilog2(cpp);
	unsigned int pixel_log2 = block_log2 - cpp_log2;
	unsigned int width_log2 = (pixel_log2 + 1) / 2;
	unsigned int height_log2 = pixel_log2 - width_log2;

	*width = 1 << width_log2;
	*height = 1 << height_log2;
}

static unsigned int get_dcc_block_size(uint64_t modifier, bool rb_aligned,
				       bool pipe_aligned)
{
	unsigned int ver = AMD_FMT_MOD_GET(TILE_VERSION, modifier);

	switch (ver) {
	case AMD_FMT_MOD_TILE_VER_GFX9: {
		/*
		 * TODO: for pipe aligned we may need to check the alignment of the
		 * total size of the surface, which may need to be bigger than the
		 * natural alignment due to some HW workarounds
		 */
		return max(10 + (rb_aligned ? (int)AMD_FMT_MOD_GET(RB, modifier) : 0), 12);
	}
	case AMD_FMT_MOD_TILE_VER_GFX10:
	case AMD_FMT_MOD_TILE_VER_GFX10_RBPLUS: {
		int pipes_log2 = AMD_FMT_MOD_GET(PIPE_XOR_BITS, modifier);

		if (ver == AMD_FMT_MOD_TILE_VER_GFX10_RBPLUS && pipes_log2 > 1 &&
		    AMD_FMT_MOD_GET(PACKERS, modifier) == pipes_log2)
			++pipes_log2;

		return max(8 + (pipe_aligned ? pipes_log2 : 0), 12);
	}
	default:
		return 0;
	}
}

static int amdgpu_display_verify_plane(struct amdgpu_framebuffer *rfb, int plane,
				       const struct drm_format_info *format,
				       unsigned int block_width, unsigned int block_height,
				       unsigned int block_size_log2)
{
	unsigned int width = rfb->base.width /
		((plane && plane < format->num_planes) ? format->hsub : 1);
	unsigned int height = rfb->base.height /
		((plane && plane < format->num_planes) ? format->vsub : 1);
	unsigned int cpp = plane < format->num_planes ? format->cpp[plane] : 1;
	unsigned int block_pitch = block_width * cpp;
	unsigned int min_pitch = ALIGN(width * cpp, block_pitch);
	unsigned int block_size = 1 << block_size_log2;
	uint64_t size;

	if (rfb->base.pitches[plane] % block_pitch) {
		drm_dbg_kms(rfb->base.dev,
			    "pitch %d for plane %d is not a multiple of block pitch %d\n",
			    rfb->base.pitches[plane], plane, block_pitch);
		return -EINVAL;
	}
	if (rfb->base.pitches[plane] < min_pitch) {
		drm_dbg_kms(rfb->base.dev,
			    "pitch %d for plane %d is less than minimum pitch %d\n",
			    rfb->base.pitches[plane], plane, min_pitch);
		return -EINVAL;
	}

	/* Force at least natural alignment. */
	if (rfb->base.offsets[plane] % block_size) {
		drm_dbg_kms(rfb->base.dev,
			    "offset 0x%x for plane %d is not a multiple of block pitch 0x%x\n",
			    rfb->base.offsets[plane], plane, block_size);
		return -EINVAL;
	}

	size = rfb->base.offsets[plane] +
		(uint64_t)rfb->base.pitches[plane] / block_pitch *
		block_size * DIV_ROUND_UP(height, block_height);

	if (drm_gem_fb_get_obj(&rfb->base, 0)->size < size) {
		drm_dbg_kms(rfb->base.dev,
			    "BO size 0x%zx is less than 0x%llx required for plane %d\n",
			    drm_gem_fb_get_obj(&rfb->base, 0)->size, size, plane);
		return -EINVAL;
	}

	return 0;
}

static int amdgpu_display_verify_sizes(struct amdgpu_framebuffer *rfb)
{
	const struct drm_format_info *format_info = drm_format_info(rfb->base.format->format);
	uint64_t modifier = rfb->base.modifier;
	int ret;
	unsigned int i, block_width, block_height, block_size_log2;

	if (!rfb->base.dev->mode_config.allow_fb_modifiers)
		return 0;

	for (i = 0; i < format_info->num_planes; ++i) {
		if (modifier == DRM_FORMAT_MOD_LINEAR) {
			block_width = 256 / format_info->cpp[i];
			block_height = 1;
			block_size_log2 = 8;
		} else {
			int swizzle = AMD_FMT_MOD_GET(TILE, modifier);

			switch ((swizzle & ~3) + 1) {
			case DC_SW_256B_S:
				block_size_log2 = 8;
				break;
			case DC_SW_4KB_S:
			case DC_SW_4KB_S_X:
				block_size_log2 = 12;
				break;
			case DC_SW_64KB_S:
			case DC_SW_64KB_S_T:
			case DC_SW_64KB_S_X:
				block_size_log2 = 16;
				break;
			default:
				drm_dbg_kms(rfb->base.dev,
					    "Swizzle mode with unknown block size: %d\n", swizzle);
				return -EINVAL;
			}

			get_block_dimensions(block_size_log2, format_info->cpp[i],
					     &block_width, &block_height);
		}

		ret = amdgpu_display_verify_plane(rfb, i, format_info,
						  block_width, block_height, block_size_log2);
		if (ret)
			return ret;
	}

	if (AMD_FMT_MOD_GET(DCC, modifier)) {
		if (AMD_FMT_MOD_GET(DCC_RETILE, modifier)) {
			block_size_log2 = get_dcc_block_size(modifier, false, false);
			get_block_dimensions(block_size_log2 + 8, format_info->cpp[0],
					     &block_width, &block_height);
			ret = amdgpu_display_verify_plane(rfb, i, format_info,
							  block_width, block_height,
							  block_size_log2);
			if (ret)
				return ret;

			++i;
			block_size_log2 = get_dcc_block_size(modifier, true, true);
		} else {
			bool pipe_aligned = AMD_FMT_MOD_GET(DCC_PIPE_ALIGN, modifier);

			block_size_log2 = get_dcc_block_size(modifier, true, pipe_aligned);
		}
		get_block_dimensions(block_size_log2 + 8, format_info->cpp[0],
				     &block_width, &block_height);
		ret = amdgpu_display_verify_plane(rfb, i, format_info,
						  block_width, block_height, block_size_log2);
		if (ret)
			return ret;
	}

	return 0;
}
#endif

static int amdgpu_display_get_fb_info(const struct amdgpu_framebuffer *amdgpu_fb,
				      uint64_t *tiling_flags, bool *tmz_surface)
{
	struct amdgpu_bo *rbo;
	int r;

	if (!amdgpu_fb) {
		*tiling_flags = 0;
		*tmz_surface = false;
		return 0;
	}

	rbo = gem_to_amdgpu_bo(drm_gem_fb_get_obj(&amdgpu_fb->base, 0));
	r = amdgpu_bo_reserve(rbo, false);

	if (unlikely(r)) {
		/* Don't show error message when returning -ERESTARTSYS */
		if (r != -ERESTARTSYS)
			DRM_ERROR("Unable to reserve buffer: %d\n", r);
		return r;
	}

	if (tiling_flags)
		amdgpu_bo_get_tiling_flags(rbo, tiling_flags);

	if (tmz_surface)
		*tmz_surface = amdgpu_bo_encrypted(rbo);

	amdgpu_bo_unreserve(rbo);

	return r;
}

int amdgpu_display_gem_fb_init(struct drm_device *dev,
			       struct amdgpu_framebuffer *rfb,
			       const struct drm_mode_fb_cmd2 *mode_cmd,
			       struct drm_gem_object *obj)
{
	int ret;
	kcl_drm_gem_fb_set_obj(&rfb->base, 0, obj);
	drm_helper_mode_fill_fb_struct(dev, &rfb->base, mode_cmd);

	ret = amdgpu_display_framebuffer_init(dev, rfb, mode_cmd, obj);
	if (ret)
		goto err;

	ret = drm_framebuffer_init(dev, &rfb->base, &amdgpu_fb_funcs);
	if (ret)
		goto err;

	return 0;
err:
	drm_dbg_kms(dev, "Failed to init gem fb: %d\n", ret);
	kcl_drm_gem_fb_set_obj(&rfb->base, 0, NULL);
	return ret;
}

int amdgpu_display_gem_fb_verify_and_init(
	struct drm_device *dev, struct amdgpu_framebuffer *rfb,
	struct drm_file *file_priv, const struct drm_mode_fb_cmd2 *mode_cmd,
	struct drm_gem_object *obj)
{
	int ret;

	kcl_drm_gem_fb_set_obj(&rfb->base, 0, obj);
	drm_helper_mode_fill_fb_struct(dev, &rfb->base, mode_cmd);

#ifdef HAVE_DRM_FORMAT_INFO_MODIFIER_SUPPORTED
	/* Verify that the modifier is supported. */
	if (!drm_any_plane_has_format(dev, mode_cmd->pixel_format,
				      mode_cmd->modifier[0])) {
		drm_dbg_kms(dev,
			    "unsupported pixel format %p4cc / modifier 0x%llx\n",
			    &mode_cmd->pixel_format, mode_cmd->modifier[0]);

		ret = -EINVAL;
		goto err;
	}
#endif

	ret = amdgpu_display_framebuffer_init(dev, rfb, mode_cmd, obj);
	if (ret)
		goto err;

	ret = drm_framebuffer_init(dev, &rfb->base, &amdgpu_fb_funcs);
	if (ret)
		goto err;

	return 0;
err:
	drm_dbg_kms(dev, "Failed to verify and init gem fb: %d\n", ret);
	kcl_drm_gem_fb_set_obj(&rfb->base, 0, NULL);
	return ret;
}

int amdgpu_display_framebuffer_init(struct drm_device *dev,
				    struct amdgpu_framebuffer *rfb,
				    const struct drm_mode_fb_cmd2 *mode_cmd,
				    struct drm_gem_object *obj)
{
	int ret, i;

	/*
	 * This needs to happen before modifier conversion as that might change
	 * the number of planes.
	 */
#ifdef HAVE_DRM_FORMAT_INFO_MODIFIER_SUPPORTED
	for (i = 1; i < rfb->base.format->num_planes; ++i) {
		if (mode_cmd->handles[i] != mode_cmd->handles[0]) {
			drm_dbg_kms(dev, "Plane 0 and %d have different BOs: %u vs. %u\n",
				    i, mode_cmd->handles[0], mode_cmd->handles[i]);
			ret = -EINVAL;
			return ret;
		}
	}
#endif

	ret = amdgpu_display_get_fb_info(rfb, &rfb->tiling_flags, &rfb->tmz_surface);
	if (ret)
		return ret;

#ifdef HAVE_DRM_FORMAT_INFO_MODIFIER_SUPPORTED
	if (dev->mode_config.allow_fb_modifiers &&
	    !(rfb->base.flags & DRM_MODE_FB_MODIFIERS)) {
		ret = convert_tiling_flags_to_modifier(rfb);
		if (ret) {
			drm_dbg_kms(dev, "Failed to convert tiling flags 0x%llX to a modifier",
				    rfb->tiling_flags);
			return ret;
		}
	}

	ret = amdgpu_display_verify_sizes(rfb);
	if (ret)
		return ret;

	for (i = 0; i < rfb->base.format->num_planes; ++i) {
		drm_gem_object_get(rfb->base.obj[0]);
		rfb->base.obj[i] = rfb->base.obj[0];
	}
#endif

	return 0;
}

struct drm_framebuffer *
amdgpu_display_user_framebuffer_create(struct drm_device *dev,
				       struct drm_file *file_priv,
				       const struct drm_mode_fb_cmd2 *mode_cmd)
{
	struct amdgpu_framebuffer *amdgpu_fb;
	struct drm_gem_object *obj;
	struct amdgpu_bo *bo;
	uint32_t domains;
	int ret;

	obj = drm_gem_object_lookup(file_priv, mode_cmd->handles[0]);
	if (obj ==  NULL) {
		drm_dbg_kms(dev, "No GEM object associated to handle 0x%08X, "
			    "can't create framebuffer\n", mode_cmd->handles[0]);
		return ERR_PTR(-ENOENT);
	}

	/* Handle is imported dma-buf, so cannot be migrated to VRAM for scanout */
	bo = gem_to_amdgpu_bo(obj);
	domains = amdgpu_display_supported_domains(drm_to_adev(dev), bo->flags);
	if (obj->import_attach && !(domains & AMDGPU_GEM_DOMAIN_GTT)) {
		drm_dbg_kms(dev, "Cannot create framebuffer from imported dma_buf\n");
#ifdef HAVE_DRM_FORMAT_INFO_MODIFIER_SUPPORTED
		drm_gem_object_put(obj);
#endif
		return ERR_PTR(-EINVAL);
	}

	amdgpu_fb = kzalloc(sizeof(*amdgpu_fb), GFP_KERNEL);
	if (amdgpu_fb == NULL) {
#ifdef HAVE_DRM_FORMAT_INFO_MODIFIER_SUPPORTED
		drm_gem_object_put(obj);
#endif
		return ERR_PTR(-ENOMEM);
	}

	ret = amdgpu_display_gem_fb_verify_and_init(dev, amdgpu_fb, file_priv,
						    mode_cmd, obj);
	if (ret) {
		kfree(amdgpu_fb);
#ifdef HAVE_DRM_FORMAT_INFO_MODIFIER_SUPPORTED
		drm_gem_object_put(obj);
#endif
		return ERR_PTR(ret);
	}

#ifdef HAVE_DRM_FORMAT_INFO_MODIFIER_SUPPORTED
	drm_gem_object_put(obj);
#endif

	return &amdgpu_fb->base;
}

const struct drm_mode_config_funcs amdgpu_mode_funcs = {
	.fb_create = amdgpu_display_user_framebuffer_create,
	.output_poll_changed = drm_fb_helper_output_poll_changed,
};

static const struct drm_prop_enum_list amdgpu_underscan_enum_list[] =
{	{ UNDERSCAN_OFF, "off" },
	{ UNDERSCAN_ON, "on" },
	{ UNDERSCAN_AUTO, "auto" },
};

static const struct drm_prop_enum_list amdgpu_audio_enum_list[] =
{	{ AMDGPU_AUDIO_DISABLE, "off" },
	{ AMDGPU_AUDIO_ENABLE, "on" },
	{ AMDGPU_AUDIO_AUTO, "auto" },
};

/* XXX support different dither options? spatial, temporal, both, etc. */
static const struct drm_prop_enum_list amdgpu_dither_enum_list[] =
{	{ AMDGPU_FMT_DITHER_DISABLE, "off" },
	{ AMDGPU_FMT_DITHER_ENABLE, "on" },
};

int amdgpu_display_modeset_create_props(struct amdgpu_device *adev)
{
	int sz;

	adev->mode_info.coherent_mode_property =
		drm_property_create_range(adev_to_drm(adev), 0, "coherent", 0, 1);
	if (!adev->mode_info.coherent_mode_property)
		return -ENOMEM;

	adev->mode_info.load_detect_property =
		drm_property_create_range(adev_to_drm(adev), 0, "load detection", 0, 1);
	if (!adev->mode_info.load_detect_property)
		return -ENOMEM;

	drm_mode_create_scaling_mode_property(adev_to_drm(adev));

	sz = ARRAY_SIZE(amdgpu_underscan_enum_list);
	adev->mode_info.underscan_property =
		drm_property_create_enum(adev_to_drm(adev), 0,
					 "underscan",
					 amdgpu_underscan_enum_list, sz);

	adev->mode_info.underscan_hborder_property =
		drm_property_create_range(adev_to_drm(adev), 0,
					  "underscan hborder", 0, 128);
	if (!adev->mode_info.underscan_hborder_property)
		return -ENOMEM;

	adev->mode_info.underscan_vborder_property =
		drm_property_create_range(adev_to_drm(adev), 0,
					  "underscan vborder", 0, 128);
	if (!adev->mode_info.underscan_vborder_property)
		return -ENOMEM;

	sz = ARRAY_SIZE(amdgpu_audio_enum_list);
	adev->mode_info.audio_property =
		drm_property_create_enum(adev_to_drm(adev), 0,
					 "audio",
					 amdgpu_audio_enum_list, sz);

	sz = ARRAY_SIZE(amdgpu_dither_enum_list);
	adev->mode_info.dither_property =
		drm_property_create_enum(adev_to_drm(adev), 0,
					 "dither",
					 amdgpu_dither_enum_list, sz);

	if (amdgpu_device_has_dc_support(adev)) {
#ifndef HAVE_DRM_CONNECTOR_PROPERTY_MAX_BPC
		adev->mode_info.max_bpc_property =
			drm_property_create_range(adev_to_drm(adev), 0, "max bpc", 8, 16);
		if (!adev->mode_info.max_bpc_property)
			return -ENOMEM;
#endif
		adev->mode_info.abm_level_property =
			drm_property_create_range(adev_to_drm(adev), 0,
						  "abm level", 0, 4);
		if (!adev->mode_info.abm_level_property)
			return -ENOMEM;
		adev->mode_info.freesync_property =
			drm_property_create_bool(adev_to_drm(adev), 0, "freesync");
		if (!adev->mode_info.freesync_property)
			return -ENOMEM;
		adev->mode_info.freesync_capable_property =
			drm_property_create_bool(adev_to_drm(adev),
						 0,
						 "freesync_capable");
		if (!adev->mode_info.freesync_capable_property)
			return -ENOMEM;
#ifndef HAVE_DRM_VRR_SUPPORTED
		adev->mode_info.vrr_capable_property =
			drm_property_create_bool(adev_to_drm(adev),
						 DRM_MODE_PROP_IMMUTABLE,
						 "vrr_capable");
		if (!adev->mode_info.vrr_capable_property)
			return -ENOMEM;

		adev->mode_info.vrr_enabled_property =
			drm_property_create_bool(adev_to_drm(adev),
						 0,
						 "VRR_ENABLED");
		if (!adev->mode_info.vrr_enabled_property)
			return -ENOMEM;
#endif
	}

	return 0;
}

void amdgpu_display_update_priority(struct amdgpu_device *adev)
{
	/* adjustment options for the display watermarks */
	if ((amdgpu_disp_priority == 0) || (amdgpu_disp_priority > 2))
		adev->mode_info.disp_priority = 0;
	else
		adev->mode_info.disp_priority = amdgpu_disp_priority;

}

static bool amdgpu_display_is_hdtv_mode(const struct drm_display_mode *mode)
{
	/* try and guess if this is a tv or a monitor */
	if ((mode->vdisplay == 480 && mode->hdisplay == 720) || /* 480p */
	    (mode->vdisplay == 576) || /* 576p */
	    (mode->vdisplay == 720) || /* 720p */
	    (mode->vdisplay == 1080)) /* 1080p */
		return true;
	else
		return false;
}

bool amdgpu_display_crtc_scaling_mode_fixup(struct drm_crtc *crtc,
					const struct drm_display_mode *mode,
					struct drm_display_mode *adjusted_mode)
{
	struct drm_device *dev = crtc->dev;
	struct drm_encoder *encoder;
	struct amdgpu_crtc *amdgpu_crtc = to_amdgpu_crtc(crtc);
	struct amdgpu_encoder *amdgpu_encoder;
	struct drm_connector *connector;
	u32 src_v = 1, dst_v = 1;
	u32 src_h = 1, dst_h = 1;

	amdgpu_crtc->h_border = 0;
	amdgpu_crtc->v_border = 0;

	list_for_each_entry(encoder, &dev->mode_config.encoder_list, head) {
		if (encoder->crtc != crtc)
			continue;
		amdgpu_encoder = to_amdgpu_encoder(encoder);
		connector = amdgpu_get_connector_for_encoder(encoder);

		/* set scaling */
		if (amdgpu_encoder->rmx_type == RMX_OFF)
			amdgpu_crtc->rmx_type = RMX_OFF;
		else if (mode->hdisplay < amdgpu_encoder->native_mode.hdisplay ||
			 mode->vdisplay < amdgpu_encoder->native_mode.vdisplay)
			amdgpu_crtc->rmx_type = amdgpu_encoder->rmx_type;
		else
			amdgpu_crtc->rmx_type = RMX_OFF;
		/* copy native mode */
		memcpy(&amdgpu_crtc->native_mode,
		       &amdgpu_encoder->native_mode,
		       sizeof(struct drm_display_mode));
		src_v = crtc->mode.vdisplay;
		dst_v = amdgpu_crtc->native_mode.vdisplay;
		src_h = crtc->mode.hdisplay;
		dst_h = amdgpu_crtc->native_mode.hdisplay;

		/* fix up for overscan on hdmi */
		if ((!(mode->flags & DRM_MODE_FLAG_INTERLACE)) &&
		    ((amdgpu_encoder->underscan_type == UNDERSCAN_ON) ||
		     ((amdgpu_encoder->underscan_type == UNDERSCAN_AUTO) &&
#if defined(HAVE_DRM_DISPLAY_INFO_IS_HDMI)
		      connector->display_info.is_hdmi &&
#else
		      drm_detect_hdmi_monitor(amdgpu_connector_edid(connector)) &&
#endif
		      amdgpu_display_is_hdtv_mode(mode)))) {
			if (amdgpu_encoder->underscan_hborder != 0)
				amdgpu_crtc->h_border = amdgpu_encoder->underscan_hborder;
			else
				amdgpu_crtc->h_border = (mode->hdisplay >> 5) + 16;
			if (amdgpu_encoder->underscan_vborder != 0)
				amdgpu_crtc->v_border = amdgpu_encoder->underscan_vborder;
			else
				amdgpu_crtc->v_border = (mode->vdisplay >> 5) + 16;
			amdgpu_crtc->rmx_type = RMX_FULL;
			src_v = crtc->mode.vdisplay;
			dst_v = crtc->mode.vdisplay - (amdgpu_crtc->v_border * 2);
			src_h = crtc->mode.hdisplay;
			dst_h = crtc->mode.hdisplay - (amdgpu_crtc->h_border * 2);
		}
	}
	if (amdgpu_crtc->rmx_type != RMX_OFF) {
		fixed20_12 a, b;
		a.full = dfixed_const(src_v);
		b.full = dfixed_const(dst_v);
		amdgpu_crtc->vsc.full = dfixed_div(a, b);
		a.full = dfixed_const(src_h);
		b.full = dfixed_const(dst_h);
		amdgpu_crtc->hsc.full = dfixed_div(a, b);
	} else {
		amdgpu_crtc->vsc.full = dfixed_const(1);
		amdgpu_crtc->hsc.full = dfixed_const(1);
	}
	return true;
}

/*
 * Retrieve current video scanout position of crtc on a given gpu, and
 * an optional accurate timestamp of when query happened.
 *
 * \param dev Device to query.
 * \param pipe Crtc to query.
 * \param flags Flags from caller (DRM_CALLED_FROM_VBLIRQ or 0).
 *              For driver internal use only also supports these flags:
 *
 *              USE_REAL_VBLANKSTART to use the real start of vblank instead
 *              of a fudged earlier start of vblank.
 *
 *              GET_DISTANCE_TO_VBLANKSTART to return distance to the
 *              fudged earlier start of vblank in *vpos and the distance
 *              to true start of vblank in *hpos.
 *
 * \param *vpos Location where vertical scanout position should be stored.
 * \param *hpos Location where horizontal scanout position should go.
 * \param *stime Target location for timestamp taken immediately before
 *               scanout position query. Can be NULL to skip timestamp.
 * \param *etime Target location for timestamp taken immediately after
 *               scanout position query. Can be NULL to skip timestamp.
 *
 * Returns vpos as a positive number while in active scanout area.
 * Returns vpos as a negative number inside vblank, counting the number
 * of scanlines to go until end of vblank, e.g., -1 means "one scanline
 * until start of active scanout / end of vblank."
 *
 * \return Flags, or'ed together as follows:
 *
 * DRM_SCANOUTPOS_VALID = Query successful.
 * DRM_SCANOUTPOS_INVBL = Inside vblank.
 * DRM_SCANOUTPOS_ACCURATE = Returned position is accurate. A lack of
 * this flag means that returned position may be offset by a constant but
 * unknown small number of scanlines wrt. real scanout position.
 *
 */
int amdgpu_display_get_crtc_scanoutpos(struct drm_device *dev,
			unsigned int pipe, unsigned int flags, int *vpos,
			int *hpos, ktime_t *stime, ktime_t *etime,
			const struct drm_display_mode *mode)
{
	u32 vbl = 0, position = 0;
	int vbl_start, vbl_end, vtotal, ret = 0;
	bool in_vbl = true;

	struct amdgpu_device *adev = drm_to_adev(dev);

	/* preempt_disable_rt() should go right here in PREEMPT_RT patchset. */

	/* Get optional system timestamp before query. */
	if (stime)
		*stime = ktime_get();

	if (amdgpu_display_page_flip_get_scanoutpos(adev, pipe, &vbl, &position) == 0)
		ret |= DRM_SCANOUTPOS_VALID;

	/* Get optional system timestamp after query. */
	if (etime)
		*etime = ktime_get();

	/* preempt_enable_rt() should go right here in PREEMPT_RT patchset. */

	/* Decode into vertical and horizontal scanout position. */
	*vpos = position & 0x1fff;
	*hpos = (position >> 16) & 0x1fff;

	/* Valid vblank area boundaries from gpu retrieved? */
	if (vbl > 0) {
		/* Yes: Decode. */
		ret |= DRM_SCANOUTPOS_ACCURATE;
		vbl_start = vbl & 0x1fff;
		vbl_end = (vbl >> 16) & 0x1fff;
	}
	else {
		/* No: Fake something reasonable which gives at least ok results. */
		vbl_start = mode->crtc_vdisplay;
		vbl_end = 0;
	}

	/* Called from driver internal vblank counter query code? */
	if (flags & GET_DISTANCE_TO_VBLANKSTART) {
	    /* Caller wants distance from real vbl_start in *hpos */
	    *hpos = *vpos - vbl_start;
	}

	/* Fudge vblank to start a few scanlines earlier to handle the
	 * problem that vblank irqs fire a few scanlines before start
	 * of vblank. Some driver internal callers need the true vblank
	 * start to be used and signal this via the USE_REAL_VBLANKSTART flag.
	 *
	 * The cause of the "early" vblank irq is that the irq is triggered
	 * by the line buffer logic when the line buffer read position enters
	 * the vblank, whereas our crtc scanout position naturally lags the
	 * line buffer read position.
	 */
	if (!(flags & USE_REAL_VBLANKSTART))
		vbl_start -= adev->mode_info.crtcs[pipe]->lb_vblank_lead_lines;

	/* Test scanout position against vblank region. */
	if ((*vpos < vbl_start) && (*vpos >= vbl_end))
		in_vbl = false;

	/* In vblank? */
	if (in_vbl)
	    ret |= DRM_SCANOUTPOS_IN_VBLANK;

	/* Called from driver internal vblank counter query code? */
	if (flags & GET_DISTANCE_TO_VBLANKSTART) {
		/* Caller wants distance from fudged earlier vbl_start */
		*vpos -= vbl_start;
		return ret;
	}

	/* Check if inside vblank area and apply corrective offsets:
	 * vpos will then be >=0 in video scanout area, but negative
	 * within vblank area, counting down the number of lines until
	 * start of scanout.
	 */

	/* Inside "upper part" of vblank area? Apply corrective offset if so: */
	if (in_vbl && (*vpos >= vbl_start)) {
		vtotal = mode->crtc_vtotal;

		/* With variable refresh rate displays the vpos can exceed
		 * the vtotal value. Clamp to 0 to return -vbl_end instead
		 * of guessing the remaining number of lines until scanout.
		 */
		*vpos = (*vpos < vtotal) ? (*vpos - vtotal) : 0;
	}

	/* Correct for shifted end of vbl at vbl_end. */
	*vpos = *vpos - vbl_end;

	return ret;
}

int amdgpu_display_crtc_idx_to_irq_type(struct amdgpu_device *adev, int crtc)
{
	if (crtc < 0 || crtc >= adev->mode_info.num_crtc)
		return AMDGPU_CRTC_IRQ_NONE;

	switch (crtc) {
	case 0:
		return AMDGPU_CRTC_IRQ_VBLANK1;
	case 1:
		return AMDGPU_CRTC_IRQ_VBLANK2;
	case 2:
		return AMDGPU_CRTC_IRQ_VBLANK3;
	case 3:
		return AMDGPU_CRTC_IRQ_VBLANK4;
	case 4:
		return AMDGPU_CRTC_IRQ_VBLANK5;
	case 5:
		return AMDGPU_CRTC_IRQ_VBLANK6;
	default:
		return AMDGPU_CRTC_IRQ_NONE;
	}
}

bool amdgpu_crtc_get_scanout_position(struct drm_crtc *crtc,
			bool in_vblank_irq, int *vpos,
			int *hpos, ktime_t *stime, ktime_t *etime,
			const struct drm_display_mode *mode)
{
	struct drm_device *dev = crtc->dev;
	unsigned int pipe = drm_crtc_index(crtc);

	return amdgpu_display_get_crtc_scanoutpos(dev, pipe, 0, vpos, hpos,
						  stime, etime, mode);
}

int amdgpu_display_suspend_helper(struct amdgpu_device *adev)
{
	struct drm_device *dev = adev_to_drm(adev);
	struct drm_crtc *crtc;
	struct drm_connector *connector;

#ifdef HAVE_DRM_CONNECTOR_LIST_ITER_BEGIN
	struct drm_connector_list_iter iter;
#endif
	int r;

	/* turn off display hw */
	drm_modeset_lock_all(dev);
#ifdef HAVE_DRM_CONNECTOR_LIST_ITER_BEGIN
	drm_connector_list_iter_begin(dev, &iter);
	drm_for_each_connector_iter(connector, &iter)
#else
	list_for_each_entry(connector, &(dev)->mode_config.connector_list, head)
#endif
		drm_helper_connector_dpms(connector,
					  DRM_MODE_DPMS_OFF);

#ifdef HAVE_DRM_CONNECTOR_LIST_ITER_BEGIN
	drm_connector_list_iter_end(&iter);
#endif
	drm_modeset_unlock_all(dev);
	/* unpin the front buffers and cursors */
	list_for_each_entry(crtc, &dev->mode_config.crtc_list, head) {
		struct amdgpu_crtc *amdgpu_crtc = to_amdgpu_crtc(crtc);
		struct drm_framebuffer *fb = crtc->primary->fb;
		struct amdgpu_bo *robj;

		if (amdgpu_crtc->cursor_bo && !adev->enable_virtual_display) {
			struct amdgpu_bo *aobj = gem_to_amdgpu_bo(amdgpu_crtc->cursor_bo);
			r = amdgpu_bo_reserve(aobj, true);
			if (r == 0) {
				amdgpu_bo_unpin(aobj);
				amdgpu_bo_unreserve(aobj);
			}
		}

		if (fb == NULL || drm_gem_fb_get_obj(fb, 0) == NULL) {
			continue;
		}
		robj = gem_to_amdgpu_bo(drm_gem_fb_get_obj(fb, 0));
#ifndef HAVE_DRM_FBDEV_GENERIC_SETUP
		/* don't unpin kernel fb objects */
		if (!amdgpu_fbdev_robj_is_fb(adev, robj)) {
			r = amdgpu_bo_reserve(robj, true);
			if (r == 0) {
				amdgpu_bo_unpin(robj);
				amdgpu_bo_unreserve(robj);
			}
		}
#else
		r = amdgpu_bo_reserve(robj, true);
		if (r == 0) {
			amdgpu_bo_unpin(robj);
			amdgpu_bo_unreserve(robj);
		}
#endif
	}
	return 0;
}

int amdgpu_display_resume_helper(struct amdgpu_device *adev)
{
	struct drm_device *dev = adev_to_drm(adev);
	struct drm_connector *connector;
#ifdef HAVE_DRM_CONNECTOR_LIST_ITER_BEGIN
	struct drm_connector_list_iter iter;
#endif
	struct drm_crtc *crtc;
	int r;

	/* pin cursors */
	list_for_each_entry(crtc, &dev->mode_config.crtc_list, head) {
		struct amdgpu_crtc *amdgpu_crtc = to_amdgpu_crtc(crtc);

		if (amdgpu_crtc->cursor_bo && !adev->enable_virtual_display) {
			struct amdgpu_bo *aobj = gem_to_amdgpu_bo(amdgpu_crtc->cursor_bo);
			r = amdgpu_bo_reserve(aobj, true);
			if (r == 0) {
				r = amdgpu_bo_pin(aobj, AMDGPU_GEM_DOMAIN_VRAM);
				if (r != 0)
					dev_err(adev->dev, "Failed to pin cursor BO (%d)\n", r);
				amdgpu_crtc->cursor_addr = amdgpu_bo_gpu_offset(aobj);
				amdgpu_bo_unreserve(aobj);
			}
		}
	}

	drm_helper_resume_force_mode(dev);

	/* turn on display hw */
	drm_modeset_lock_all(dev);

#ifdef HAVE_DRM_CONNECTOR_LIST_ITER_BEGIN
	drm_connector_list_iter_begin(dev, &iter);
	drm_for_each_connector_iter(connector, &iter)
#else
	list_for_each_entry(connector, &(dev)->mode_config.connector_list, head)
#endif
		drm_helper_connector_dpms(connector,
					  DRM_MODE_DPMS_ON);
#ifdef HAVE_DRM_CONNECTOR_LIST_ITER_BEGIN
	drm_connector_list_iter_end(&iter);

#endif
	drm_modeset_unlock_all(dev);

	return 0;
}

int amdgpu_display_freesync_ioctl(struct drm_device *dev, void *data,
				  struct drm_file *filp)
{
	int ret = -EPERM;
	struct amdgpu_device *adev = drm_to_adev(dev);

	if (adev->mode_info.funcs->notify_freesync)
		ret = adev->mode_info.funcs->notify_freesync(dev,data,filp);
	else
		DRM_DEBUG("amdgpu no notify_freesync ioctl\n");

	return ret;
}
