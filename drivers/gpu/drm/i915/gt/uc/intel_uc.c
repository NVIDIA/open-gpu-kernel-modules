// SPDX-License-Identifier: MIT
/*
 * Copyright © 2016-2019 Intel Corporation
 */

#include "gt/intel_gt.h"
#include "gt/intel_reset.h"
#include "intel_guc.h"
#include "intel_guc_ads.h"
#include "intel_guc_submission.h"
#include "intel_uc.h"

#include "i915_drv.h"

static const struct intel_uc_ops uc_ops_off;
static const struct intel_uc_ops uc_ops_on;

static void uc_expand_default_options(struct intel_uc *uc)
{
	struct drm_i915_private *i915 = uc_to_gt(uc)->i915;

	if (i915->params.enable_guc != -1)
		return;

	/* Don't enable GuC/HuC on pre-Gen12 */
	if (GRAPHICS_VER(i915) < 12) {
		i915->params.enable_guc = 0;
		return;
	}

	/* Don't enable GuC/HuC on older Gen12 platforms */
	if (IS_TIGERLAKE(i915) || IS_ROCKETLAKE(i915)) {
		i915->params.enable_guc = 0;
		return;
	}

	/* Default: enable HuC authentication only */
	i915->params.enable_guc = ENABLE_GUC_LOAD_HUC;
}

/* Reset GuC providing us with fresh state for both GuC and HuC.
 */
static int __intel_uc_reset_hw(struct intel_uc *uc)
{
	struct intel_gt *gt = uc_to_gt(uc);
	int ret;
	u32 guc_status;

	ret = i915_inject_probe_error(gt->i915, -ENXIO);
	if (ret)
		return ret;

	ret = intel_reset_guc(gt);
	if (ret) {
		DRM_ERROR("Failed to reset GuC, ret = %d\n", ret);
		return ret;
	}

	guc_status = intel_uncore_read(gt->uncore, GUC_STATUS);
	WARN(!(guc_status & GS_MIA_IN_RESET),
	     "GuC status: 0x%x, MIA core expected to be in reset\n",
	     guc_status);

	return ret;
}

static void __confirm_options(struct intel_uc *uc)
{
	struct drm_i915_private *i915 = uc_to_gt(uc)->i915;

	drm_dbg(&i915->drm,
		"enable_guc=%d (guc:%s submission:%s huc:%s)\n",
		i915->params.enable_guc,
		yesno(intel_uc_wants_guc(uc)),
		yesno(intel_uc_wants_guc_submission(uc)),
		yesno(intel_uc_wants_huc(uc)));

	if (i915->params.enable_guc == 0) {
		GEM_BUG_ON(intel_uc_wants_guc(uc));
		GEM_BUG_ON(intel_uc_wants_guc_submission(uc));
		GEM_BUG_ON(intel_uc_wants_huc(uc));
		return;
	}

	if (!intel_uc_supports_guc(uc))
		drm_info(&i915->drm,
			 "Incompatible option enable_guc=%d - %s\n",
			 i915->params.enable_guc, "GuC is not supported!");

	if (i915->params.enable_guc & ENABLE_GUC_LOAD_HUC &&
	    !intel_uc_supports_huc(uc))
		drm_info(&i915->drm,
			 "Incompatible option enable_guc=%d - %s\n",
			 i915->params.enable_guc, "HuC is not supported!");

	if (i915->params.enable_guc & ENABLE_GUC_SUBMISSION &&
	    !intel_uc_supports_guc_submission(uc))
		drm_info(&i915->drm,
			 "Incompatible option enable_guc=%d - %s\n",
			 i915->params.enable_guc, "GuC submission is N/A");

	if (i915->params.enable_guc & ~ENABLE_GUC_MASK)
		drm_info(&i915->drm,
			 "Incompatible option enable_guc=%d - %s\n",
			 i915->params.enable_guc, "undocumented flag");
}

void intel_uc_init_early(struct intel_uc *uc)
{
	uc_expand_default_options(uc);

	intel_guc_init_early(&uc->guc);
	intel_huc_init_early(&uc->huc);

	__confirm_options(uc);

	if (intel_uc_wants_guc(uc))
		uc->ops = &uc_ops_on;
	else
		uc->ops = &uc_ops_off;
}

void intel_uc_driver_late_release(struct intel_uc *uc)
{
}

/**
 * intel_uc_init_mmio - setup uC MMIO access
 * @uc: the intel_uc structure
 *
 * Setup minimal state necessary for MMIO accesses later in the
 * initialization sequence.
 */
void intel_uc_init_mmio(struct intel_uc *uc)
{
	intel_guc_init_send_regs(&uc->guc);
}

static void __uc_capture_load_err_log(struct intel_uc *uc)
{
	struct intel_guc *guc = &uc->guc;

	if (guc->log.vma && !uc->load_err_log)
		uc->load_err_log = i915_gem_object_get(guc->log.vma->obj);
}

static void __uc_free_load_err_log(struct intel_uc *uc)
{
	struct drm_i915_gem_object *log = fetch_and_zero(&uc->load_err_log);

	if (log)
		i915_gem_object_put(log);
}

void intel_uc_driver_remove(struct intel_uc *uc)
{
	intel_uc_fini_hw(uc);
	intel_uc_fini(uc);
	__uc_free_load_err_log(uc);
}

static inline bool guc_communication_enabled(struct intel_guc *guc)
{
	return intel_guc_ct_enabled(&guc->ct);
}

/*
 * Events triggered while CT buffers are disabled are logged in the SCRATCH_15
 * register using the same bits used in the CT message payload. Since our
 * communication channel with guc is turned off at this point, we can save the
 * message and handle it after we turn it back on.
 */
static void guc_clear_mmio_msg(struct intel_guc *guc)
{
	intel_uncore_write(guc_to_gt(guc)->uncore, SOFT_SCRATCH(15), 0);
}

static void guc_get_mmio_msg(struct intel_guc *guc)
{
	u32 val;

	spin_lock_irq(&guc->irq_lock);

	val = intel_uncore_read(guc_to_gt(guc)->uncore, SOFT_SCRATCH(15));
	guc->mmio_msg |= val & guc->msg_enabled_mask;

	/*
	 * clear all events, including the ones we're not currently servicing,
	 * to make sure we don't try to process a stale message if we enable
	 * handling of more events later.
	 */
	guc_clear_mmio_msg(guc);

	spin_unlock_irq(&guc->irq_lock);
}

static void guc_handle_mmio_msg(struct intel_guc *guc)
{
	/* we need communication to be enabled to reply to GuC */
	GEM_BUG_ON(!guc_communication_enabled(guc));

	spin_lock_irq(&guc->irq_lock);
	if (guc->mmio_msg) {
		intel_guc_to_host_process_recv_msg(guc, &guc->mmio_msg, 1);
		guc->mmio_msg = 0;
	}
	spin_unlock_irq(&guc->irq_lock);
}

static void guc_reset_interrupts(struct intel_guc *guc)
{
	guc->interrupts.reset(guc);
}

static void guc_enable_interrupts(struct intel_guc *guc)
{
	guc->interrupts.enable(guc);
}

static void guc_disable_interrupts(struct intel_guc *guc)
{
	guc->interrupts.disable(guc);
}

static int guc_enable_communication(struct intel_guc *guc)
{
	struct intel_gt *gt = guc_to_gt(guc);
	struct drm_i915_private *i915 = gt->i915;
	int ret;

	GEM_BUG_ON(guc_communication_enabled(guc));

	ret = i915_inject_probe_error(i915, -ENXIO);
	if (ret)
		return ret;

	ret = intel_guc_ct_enable(&guc->ct);
	if (ret)
		return ret;

	/* check for mmio messages received before/during the CT enable */
	guc_get_mmio_msg(guc);
	guc_handle_mmio_msg(guc);

	guc_enable_interrupts(guc);

	/* check for CT messages received before we enabled interrupts */
	spin_lock_irq(&gt->irq_lock);
	intel_guc_ct_event_handler(&guc->ct);
	spin_unlock_irq(&gt->irq_lock);

	drm_dbg(&i915->drm, "GuC communication enabled\n");

	return 0;
}

static void guc_disable_communication(struct intel_guc *guc)
{
	struct drm_i915_private *i915 = guc_to_gt(guc)->i915;

	/*
	 * Events generated during or after CT disable are logged by guc in
	 * via mmio. Make sure the register is clear before disabling CT since
	 * all events we cared about have already been processed via CT.
	 */
	guc_clear_mmio_msg(guc);

	guc_disable_interrupts(guc);

	intel_guc_ct_disable(&guc->ct);

	/*
	 * Check for messages received during/after the CT disable. We do not
	 * expect any messages to have arrived via CT between the interrupt
	 * disable and the CT disable because GuC should've been idle until we
	 * triggered the CT disable protocol.
	 */
	guc_get_mmio_msg(guc);

	drm_dbg(&i915->drm, "GuC communication disabled\n");
}

static void __uc_fetch_firmwares(struct intel_uc *uc)
{
	int err;

	GEM_BUG_ON(!intel_uc_wants_guc(uc));

	err = intel_uc_fw_fetch(&uc->guc.fw);
	if (err) {
		/* Make sure we transition out of transient "SELECTED" state */
		if (intel_uc_wants_huc(uc)) {
			drm_dbg(&uc_to_gt(uc)->i915->drm,
				"Failed to fetch GuC: %d disabling HuC\n", err);
			intel_uc_fw_change_status(&uc->huc.fw,
						  INTEL_UC_FIRMWARE_ERROR);
		}

		return;
	}

	if (intel_uc_wants_huc(uc))
		intel_uc_fw_fetch(&uc->huc.fw);
}

static void __uc_cleanup_firmwares(struct intel_uc *uc)
{
	intel_uc_fw_cleanup_fetch(&uc->huc.fw);
	intel_uc_fw_cleanup_fetch(&uc->guc.fw);
}

static int __uc_init(struct intel_uc *uc)
{
	struct intel_guc *guc = &uc->guc;
	struct intel_huc *huc = &uc->huc;
	int ret;

	GEM_BUG_ON(!intel_uc_wants_guc(uc));

	if (!intel_uc_uses_guc(uc))
		return 0;

	if (i915_inject_probe_failure(uc_to_gt(uc)->i915))
		return -ENOMEM;

	/* XXX: GuC submission is unavailable for now */
	GEM_BUG_ON(intel_uc_uses_guc_submission(uc));

	ret = intel_guc_init(guc);
	if (ret)
		return ret;

	if (intel_uc_uses_huc(uc)) {
		ret = intel_huc_init(huc);
		if (ret)
			goto out_guc;
	}

	return 0;

out_guc:
	intel_guc_fini(guc);
	return ret;
}

static void __uc_fini(struct intel_uc *uc)
{
	intel_huc_fini(&uc->huc);
	intel_guc_fini(&uc->guc);
}

static int __uc_sanitize(struct intel_uc *uc)
{
	struct intel_guc *guc = &uc->guc;
	struct intel_huc *huc = &uc->huc;

	GEM_BUG_ON(!intel_uc_supports_guc(uc));

	intel_huc_sanitize(huc);
	intel_guc_sanitize(guc);

	return __intel_uc_reset_hw(uc);
}

/* Initialize and verify the uC regs related to uC positioning in WOPCM */
static int uc_init_wopcm(struct intel_uc *uc)
{
	struct intel_gt *gt = uc_to_gt(uc);
	struct intel_uncore *uncore = gt->uncore;
	u32 base = intel_wopcm_guc_base(&gt->i915->wopcm);
	u32 size = intel_wopcm_guc_size(&gt->i915->wopcm);
	u32 huc_agent = intel_uc_uses_huc(uc) ? HUC_LOADING_AGENT_GUC : 0;
	u32 mask;
	int err;

	if (unlikely(!base || !size)) {
		i915_probe_error(gt->i915, "Unsuccessful WOPCM partitioning\n");
		return -E2BIG;
	}

	GEM_BUG_ON(!intel_uc_supports_guc(uc));
	GEM_BUG_ON(!(base & GUC_WOPCM_OFFSET_MASK));
	GEM_BUG_ON(base & ~GUC_WOPCM_OFFSET_MASK);
	GEM_BUG_ON(!(size & GUC_WOPCM_SIZE_MASK));
	GEM_BUG_ON(size & ~GUC_WOPCM_SIZE_MASK);

	err = i915_inject_probe_error(gt->i915, -ENXIO);
	if (err)
		return err;

	mask = GUC_WOPCM_SIZE_MASK | GUC_WOPCM_SIZE_LOCKED;
	err = intel_uncore_write_and_verify(uncore, GUC_WOPCM_SIZE, size, mask,
					    size | GUC_WOPCM_SIZE_LOCKED);
	if (err)
		goto err_out;

	mask = GUC_WOPCM_OFFSET_MASK | GUC_WOPCM_OFFSET_VALID | huc_agent;
	err = intel_uncore_write_and_verify(uncore, DMA_GUC_WOPCM_OFFSET,
					    base | huc_agent, mask,
					    base | huc_agent |
					    GUC_WOPCM_OFFSET_VALID);
	if (err)
		goto err_out;

	return 0;

err_out:
	i915_probe_error(gt->i915, "Failed to init uC WOPCM registers!\n");
	i915_probe_error(gt->i915, "%s(%#x)=%#x\n", "DMA_GUC_WOPCM_OFFSET",
			 i915_mmio_reg_offset(DMA_GUC_WOPCM_OFFSET),
			 intel_uncore_read(uncore, DMA_GUC_WOPCM_OFFSET));
	i915_probe_error(gt->i915, "%s(%#x)=%#x\n", "GUC_WOPCM_SIZE",
			 i915_mmio_reg_offset(GUC_WOPCM_SIZE),
			 intel_uncore_read(uncore, GUC_WOPCM_SIZE));

	return err;
}

static bool uc_is_wopcm_locked(struct intel_uc *uc)
{
	struct intel_gt *gt = uc_to_gt(uc);
	struct intel_uncore *uncore = gt->uncore;

	return (intel_uncore_read(uncore, GUC_WOPCM_SIZE) & GUC_WOPCM_SIZE_LOCKED) ||
	       (intel_uncore_read(uncore, DMA_GUC_WOPCM_OFFSET) & GUC_WOPCM_OFFSET_VALID);
}

static int __uc_check_hw(struct intel_uc *uc)
{
	if (!intel_uc_supports_guc(uc))
		return 0;

	/*
	 * We can silently continue without GuC only if it was never enabled
	 * before on this system after reboot, otherwise we risk GPU hangs.
	 * To check if GuC was loaded before we look at WOPCM registers.
	 */
	if (uc_is_wopcm_locked(uc))
		return -EIO;

	return 0;
}

static int __uc_init_hw(struct intel_uc *uc)
{
	struct drm_i915_private *i915 = uc_to_gt(uc)->i915;
	struct intel_guc *guc = &uc->guc;
	struct intel_huc *huc = &uc->huc;
	int ret, attempts;

	GEM_BUG_ON(!intel_uc_supports_guc(uc));
	GEM_BUG_ON(!intel_uc_wants_guc(uc));

	if (!intel_uc_fw_is_loadable(&guc->fw)) {
		ret = __uc_check_hw(uc) ||
		      intel_uc_fw_is_overridden(&guc->fw) ||
		      intel_uc_wants_guc_submission(uc) ?
		      intel_uc_fw_status_to_error(guc->fw.status) : 0;
		goto err_out;
	}

	ret = uc_init_wopcm(uc);
	if (ret)
		goto err_out;

	guc_reset_interrupts(guc);

	/* WaEnableuKernelHeaderValidFix:skl */
	/* WaEnableGuCBootHashCheckNotSet:skl,bxt,kbl */
	if (GRAPHICS_VER(i915) == 9)
		attempts = 3;
	else
		attempts = 1;

	while (attempts--) {
		/*
		 * Always reset the GuC just before (re)loading, so
		 * that the state and timing are fairly predictable
		 */
		ret = __uc_sanitize(uc);
		if (ret)
			goto err_out;

		intel_huc_fw_upload(huc);
		intel_guc_ads_reset(guc);
		intel_guc_write_params(guc);
		ret = intel_guc_fw_upload(guc);
		if (ret == 0)
			break;

		DRM_DEBUG_DRIVER("GuC fw load failed: %d; will reset and "
				 "retry %d more time(s)\n", ret, attempts);
	}

	/* Did we succeded or run out of retries? */
	if (ret)
		goto err_log_capture;

	ret = guc_enable_communication(guc);
	if (ret)
		goto err_log_capture;

	intel_huc_auth(huc);

	if (intel_uc_uses_guc_submission(uc))
		intel_guc_submission_enable(guc);

	drm_info(&i915->drm, "%s firmware %s version %u.%u %s:%s\n",
		 intel_uc_fw_type_repr(INTEL_UC_FW_TYPE_GUC), guc->fw.path,
		 guc->fw.major_ver_found, guc->fw.minor_ver_found,
		 "submission",
		 enableddisabled(intel_uc_uses_guc_submission(uc)));

	if (intel_uc_uses_huc(uc)) {
		drm_info(&i915->drm, "%s firmware %s version %u.%u %s:%s\n",
			 intel_uc_fw_type_repr(INTEL_UC_FW_TYPE_HUC),
			 huc->fw.path,
			 huc->fw.major_ver_found, huc->fw.minor_ver_found,
			 "authenticated",
			 yesno(intel_huc_is_authenticated(huc)));
	}

	return 0;

	/*
	 * We've failed to load the firmware :(
	 */
err_log_capture:
	__uc_capture_load_err_log(uc);
err_out:
	__uc_sanitize(uc);

	if (!ret) {
		drm_notice(&i915->drm, "GuC is uninitialized\n");
		/* We want to run without GuC submission */
		return 0;
	}

	i915_probe_error(i915, "GuC initialization failed %d\n", ret);

	/* We want to keep KMS alive */
	return -EIO;
}

static void __uc_fini_hw(struct intel_uc *uc)
{
	struct intel_guc *guc = &uc->guc;

	if (!intel_guc_is_fw_running(guc))
		return;

	if (intel_uc_uses_guc_submission(uc))
		intel_guc_submission_disable(guc);

	__uc_sanitize(uc);
}

/**
 * intel_uc_reset_prepare - Prepare for reset
 * @uc: the intel_uc structure
 *
 * Preparing for full gpu reset.
 */
void intel_uc_reset_prepare(struct intel_uc *uc)
{
	struct intel_guc *guc = &uc->guc;

	if (!intel_guc_is_ready(guc))
		return;

	__uc_sanitize(uc);
}

void intel_uc_runtime_suspend(struct intel_uc *uc)
{
	struct intel_guc *guc = &uc->guc;
	int err;

	if (!intel_guc_is_ready(guc))
		return;

	err = intel_guc_suspend(guc);
	if (err)
		DRM_DEBUG_DRIVER("Failed to suspend GuC, err=%d", err);

	guc_disable_communication(guc);
}

void intel_uc_suspend(struct intel_uc *uc)
{
	struct intel_guc *guc = &uc->guc;
	intel_wakeref_t wakeref;

	if (!intel_guc_is_ready(guc))
		return;

	with_intel_runtime_pm(uc_to_gt(uc)->uncore->rpm, wakeref)
		intel_uc_runtime_suspend(uc);
}

static int __uc_resume(struct intel_uc *uc, bool enable_communication)
{
	struct intel_guc *guc = &uc->guc;
	int err;

	if (!intel_guc_is_fw_running(guc))
		return 0;

	/* Make sure we enable communication if and only if it's disabled */
	GEM_BUG_ON(enable_communication == guc_communication_enabled(guc));

	if (enable_communication)
		guc_enable_communication(guc);

	err = intel_guc_resume(guc);
	if (err) {
		DRM_DEBUG_DRIVER("Failed to resume GuC, err=%d", err);
		return err;
	}

	return 0;
}

int intel_uc_resume(struct intel_uc *uc)
{
	/*
	 * When coming out of S3/S4 we sanitize and re-init the HW, so
	 * communication is already re-enabled at this point.
	 */
	return __uc_resume(uc, false);
}

int intel_uc_runtime_resume(struct intel_uc *uc)
{
	/*
	 * During runtime resume we don't sanitize, so we need to re-init
	 * communication as well.
	 */
	return __uc_resume(uc, true);
}

static const struct intel_uc_ops uc_ops_off = {
	.init_hw = __uc_check_hw,
};

static const struct intel_uc_ops uc_ops_on = {
	.sanitize = __uc_sanitize,

	.init_fw = __uc_fetch_firmwares,
	.fini_fw = __uc_cleanup_firmwares,

	.init = __uc_init,
	.fini = __uc_fini,

	.init_hw = __uc_init_hw,
	.fini_hw = __uc_fini_hw,
};
