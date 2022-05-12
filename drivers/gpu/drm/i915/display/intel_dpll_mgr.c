/*
 * Copyright © 2006-2016 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "intel_de.h"
#include "intel_display_types.h"
#include "intel_dpio_phy.h"
#include "intel_dpll.h"
#include "intel_dpll_mgr.h"

/**
 * DOC: Display PLLs
 *
 * Display PLLs used for driving outputs vary by platform. While some have
 * per-pipe or per-encoder dedicated PLLs, others allow the use of any PLL
 * from a pool. In the latter scenario, it is possible that multiple pipes
 * share a PLL if their configurations match.
 *
 * This file provides an abstraction over display PLLs. The function
 * intel_shared_dpll_init() initializes the PLLs for the given platform.  The
 * users of a PLL are tracked and that tracking is integrated with the atomic
 * modset interface. During an atomic operation, required PLLs can be reserved
 * for a given CRTC and encoder configuration by calling
 * intel_reserve_shared_dplls() and previously reserved PLLs can be released
 * with intel_release_shared_dplls().
 * Changes to the users are first staged in the atomic state, and then made
 * effective by calling intel_shared_dpll_swap_state() during the atomic
 * commit phase.
 */

struct intel_dpll_mgr {
	const struct dpll_info *dpll_info;

	bool (*get_dplls)(struct intel_atomic_state *state,
			  struct intel_crtc *crtc,
			  struct intel_encoder *encoder);
	void (*put_dplls)(struct intel_atomic_state *state,
			  struct intel_crtc *crtc);
	void (*update_active_dpll)(struct intel_atomic_state *state,
				   struct intel_crtc *crtc,
				   struct intel_encoder *encoder);
	void (*update_ref_clks)(struct drm_i915_private *i915);
	void (*dump_hw_state)(struct drm_i915_private *dev_priv,
			      const struct intel_dpll_hw_state *hw_state);
};

static void
intel_atomic_duplicate_dpll_state(struct drm_i915_private *dev_priv,
				  struct intel_shared_dpll_state *shared_dpll)
{
	enum intel_dpll_id i;

	/* Copy shared dpll state */
	for (i = 0; i < dev_priv->dpll.num_shared_dpll; i++) {
		struct intel_shared_dpll *pll = &dev_priv->dpll.shared_dplls[i];

		shared_dpll[i] = pll->state;
	}
}

static struct intel_shared_dpll_state *
intel_atomic_get_shared_dpll_state(struct drm_atomic_state *s)
{
	struct intel_atomic_state *state = to_intel_atomic_state(s);

	drm_WARN_ON(s->dev, !drm_modeset_is_locked(&s->dev->mode_config.connection_mutex));

	if (!state->dpll_set) {
		state->dpll_set = true;

		intel_atomic_duplicate_dpll_state(to_i915(s->dev),
						  state->shared_dpll);
	}

	return state->shared_dpll;
}

/**
 * intel_get_shared_dpll_by_id - get a DPLL given its id
 * @dev_priv: i915 device instance
 * @id: pll id
 *
 * Returns:
 * A pointer to the DPLL with @id
 */
struct intel_shared_dpll *
intel_get_shared_dpll_by_id(struct drm_i915_private *dev_priv,
			    enum intel_dpll_id id)
{
	return &dev_priv->dpll.shared_dplls[id];
}

/**
 * intel_get_shared_dpll_id - get the id of a DPLL
 * @dev_priv: i915 device instance
 * @pll: the DPLL
 *
 * Returns:
 * The id of @pll
 */
enum intel_dpll_id
intel_get_shared_dpll_id(struct drm_i915_private *dev_priv,
			 struct intel_shared_dpll *pll)
{
	long pll_idx = pll - dev_priv->dpll.shared_dplls;

	if (drm_WARN_ON(&dev_priv->drm,
			pll_idx < 0 ||
			pll_idx >= dev_priv->dpll.num_shared_dpll))
		return -1;

	return pll_idx;
}

/* For ILK+ */
void assert_shared_dpll(struct drm_i915_private *dev_priv,
			struct intel_shared_dpll *pll,
			bool state)
{
	bool cur_state;
	struct intel_dpll_hw_state hw_state;

	if (drm_WARN(&dev_priv->drm, !pll,
		     "asserting DPLL %s with no DPLL\n", onoff(state)))
		return;

	cur_state = intel_dpll_get_hw_state(dev_priv, pll, &hw_state);
	I915_STATE_WARN(cur_state != state,
	     "%s assertion failure (expected %s, current %s)\n",
			pll->info->name, onoff(state), onoff(cur_state));
}

static enum tc_port icl_pll_id_to_tc_port(enum intel_dpll_id id)
{
	return TC_PORT_1 + id - DPLL_ID_ICL_MGPLL1;
}

enum intel_dpll_id icl_tc_port_to_pll_id(enum tc_port tc_port)
{
	return tc_port - TC_PORT_1 + DPLL_ID_ICL_MGPLL1;
}

static i915_reg_t
intel_combo_pll_enable_reg(struct drm_i915_private *i915,
			   struct intel_shared_dpll *pll)
{
	if (IS_DG1(i915))
		return DG1_DPLL_ENABLE(pll->info->id);
	else if (IS_JSL_EHL(i915) && (pll->info->id == DPLL_ID_EHL_DPLL4))
		return MG_PLL_ENABLE(0);

	return CNL_DPLL_ENABLE(pll->info->id);
}

static i915_reg_t
intel_tc_pll_enable_reg(struct drm_i915_private *i915,
			struct intel_shared_dpll *pll)
{
	const enum intel_dpll_id id = pll->info->id;
	enum tc_port tc_port = icl_pll_id_to_tc_port(id);

	if (IS_ALDERLAKE_P(i915))
		return ADLP_PORTTC_PLL_ENABLE(tc_port);

	return MG_PLL_ENABLE(tc_port);
}

/**
 * intel_prepare_shared_dpll - call a dpll's prepare hook
 * @crtc_state: CRTC, and its state, which has a shared dpll
 *
 * This calls the PLL's prepare hook if it has one and if the PLL is not
 * already enabled. The prepare hook is platform specific.
 */
void intel_prepare_shared_dpll(const struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	struct intel_shared_dpll *pll = crtc_state->shared_dpll;

	if (drm_WARN_ON(&dev_priv->drm, pll == NULL))
		return;

	mutex_lock(&dev_priv->dpll.lock);
	drm_WARN_ON(&dev_priv->drm, !pll->state.pipe_mask);
	if (!pll->active_mask) {
		drm_dbg(&dev_priv->drm, "setting up %s\n", pll->info->name);
		drm_WARN_ON(&dev_priv->drm, pll->on);
		assert_shared_dpll_disabled(dev_priv, pll);

		pll->info->funcs->prepare(dev_priv, pll);
	}
	mutex_unlock(&dev_priv->dpll.lock);
}

/**
 * intel_enable_shared_dpll - enable a CRTC's shared DPLL
 * @crtc_state: CRTC, and its state, which has a shared DPLL
 *
 * Enable the shared DPLL used by @crtc.
 */
void intel_enable_shared_dpll(const struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	struct intel_shared_dpll *pll = crtc_state->shared_dpll;
	unsigned int pipe_mask = BIT(crtc->pipe);
	unsigned int old_mask;

	if (drm_WARN_ON(&dev_priv->drm, pll == NULL))
		return;

	mutex_lock(&dev_priv->dpll.lock);
	old_mask = pll->active_mask;

	if (drm_WARN_ON(&dev_priv->drm, !(pll->state.pipe_mask & pipe_mask)) ||
	    drm_WARN_ON(&dev_priv->drm, pll->active_mask & pipe_mask))
		goto out;

	pll->active_mask |= pipe_mask;

	drm_dbg_kms(&dev_priv->drm,
		    "enable %s (active 0x%x, on? %d) for [CRTC:%d:%s]\n",
		    pll->info->name, pll->active_mask, pll->on,
		    crtc->base.base.id, crtc->base.name);

	if (old_mask) {
		drm_WARN_ON(&dev_priv->drm, !pll->on);
		assert_shared_dpll_enabled(dev_priv, pll);
		goto out;
	}
	drm_WARN_ON(&dev_priv->drm, pll->on);

	drm_dbg_kms(&dev_priv->drm, "enabling %s\n", pll->info->name);
	pll->info->funcs->enable(dev_priv, pll);
	pll->on = true;

out:
	mutex_unlock(&dev_priv->dpll.lock);
}

/**
 * intel_disable_shared_dpll - disable a CRTC's shared DPLL
 * @crtc_state: CRTC, and its state, which has a shared DPLL
 *
 * Disable the shared DPLL used by @crtc.
 */
void intel_disable_shared_dpll(const struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	struct intel_shared_dpll *pll = crtc_state->shared_dpll;
	unsigned int pipe_mask = BIT(crtc->pipe);

	/* PCH only available on ILK+ */
	if (DISPLAY_VER(dev_priv) < 5)
		return;

	if (pll == NULL)
		return;

	mutex_lock(&dev_priv->dpll.lock);
	if (drm_WARN(&dev_priv->drm, !(pll->active_mask & pipe_mask),
		     "%s not used by [CRTC:%d:%s]\n", pll->info->name,
		     crtc->base.base.id, crtc->base.name))
		goto out;

	drm_dbg_kms(&dev_priv->drm,
		    "disable %s (active 0x%x, on? %d) for [CRTC:%d:%s]\n",
		    pll->info->name, pll->active_mask, pll->on,
		    crtc->base.base.id, crtc->base.name);

	assert_shared_dpll_enabled(dev_priv, pll);
	drm_WARN_ON(&dev_priv->drm, !pll->on);

	pll->active_mask &= ~pipe_mask;
	if (pll->active_mask)
		goto out;

	drm_dbg_kms(&dev_priv->drm, "disabling %s\n", pll->info->name);
	pll->info->funcs->disable(dev_priv, pll);
	pll->on = false;

out:
	mutex_unlock(&dev_priv->dpll.lock);
}

static struct intel_shared_dpll *
intel_find_shared_dpll(struct intel_atomic_state *state,
		       const struct intel_crtc *crtc,
		       const struct intel_dpll_hw_state *pll_state,
		       unsigned long dpll_mask)
{
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	struct intel_shared_dpll *pll, *unused_pll = NULL;
	struct intel_shared_dpll_state *shared_dpll;
	enum intel_dpll_id i;

	shared_dpll = intel_atomic_get_shared_dpll_state(&state->base);

	drm_WARN_ON(&dev_priv->drm, dpll_mask & ~(BIT(I915_NUM_PLLS) - 1));

	for_each_set_bit(i, &dpll_mask, I915_NUM_PLLS) {
		pll = &dev_priv->dpll.shared_dplls[i];

		/* Only want to check enabled timings first */
		if (shared_dpll[i].pipe_mask == 0) {
			if (!unused_pll)
				unused_pll = pll;
			continue;
		}

		if (memcmp(pll_state,
			   &shared_dpll[i].hw_state,
			   sizeof(*pll_state)) == 0) {
			drm_dbg_kms(&dev_priv->drm,
				    "[CRTC:%d:%s] sharing existing %s (pipe mask 0x%x, active 0x%x)\n",
				    crtc->base.base.id, crtc->base.name,
				    pll->info->name,
				    shared_dpll[i].pipe_mask,
				    pll->active_mask);
			return pll;
		}
	}

	/* Ok no matching timings, maybe there's a free one? */
	if (unused_pll) {
		drm_dbg_kms(&dev_priv->drm, "[CRTC:%d:%s] allocated %s\n",
			    crtc->base.base.id, crtc->base.name,
			    unused_pll->info->name);
		return unused_pll;
	}

	return NULL;
}

static void
intel_reference_shared_dpll(struct intel_atomic_state *state,
			    const struct intel_crtc *crtc,
			    const struct intel_shared_dpll *pll,
			    const struct intel_dpll_hw_state *pll_state)
{
	struct drm_i915_private *i915 = to_i915(state->base.dev);
	struct intel_shared_dpll_state *shared_dpll;
	const enum intel_dpll_id id = pll->info->id;

	shared_dpll = intel_atomic_get_shared_dpll_state(&state->base);

	if (shared_dpll[id].pipe_mask == 0)
		shared_dpll[id].hw_state = *pll_state;

	drm_dbg(&i915->drm, "using %s for pipe %c\n", pll->info->name,
		pipe_name(crtc->pipe));

	shared_dpll[id].pipe_mask |= BIT(crtc->pipe);
}

static void intel_unreference_shared_dpll(struct intel_atomic_state *state,
					  const struct intel_crtc *crtc,
					  const struct intel_shared_dpll *pll)
{
	struct intel_shared_dpll_state *shared_dpll;

	shared_dpll = intel_atomic_get_shared_dpll_state(&state->base);
	shared_dpll[pll->info->id].pipe_mask &= ~BIT(crtc->pipe);
}

static void intel_put_dpll(struct intel_atomic_state *state,
			   struct intel_crtc *crtc)
{
	const struct intel_crtc_state *old_crtc_state =
		intel_atomic_get_old_crtc_state(state, crtc);
	struct intel_crtc_state *new_crtc_state =
		intel_atomic_get_new_crtc_state(state, crtc);

	new_crtc_state->shared_dpll = NULL;

	if (!old_crtc_state->shared_dpll)
		return;

	intel_unreference_shared_dpll(state, crtc, old_crtc_state->shared_dpll);
}

/**
 * intel_shared_dpll_swap_state - make atomic DPLL configuration effective
 * @state: atomic state
 *
 * This is the dpll version of drm_atomic_helper_swap_state() since the
 * helper does not handle driver-specific global state.
 *
 * For consistency with atomic helpers this function does a complete swap,
 * i.e. it also puts the current state into @state, even though there is no
 * need for that at this moment.
 */
void intel_shared_dpll_swap_state(struct intel_atomic_state *state)
{
	struct drm_i915_private *dev_priv = to_i915(state->base.dev);
	struct intel_shared_dpll_state *shared_dpll = state->shared_dpll;
	enum intel_dpll_id i;

	if (!state->dpll_set)
		return;

	for (i = 0; i < dev_priv->dpll.num_shared_dpll; i++) {
		struct intel_shared_dpll *pll =
			&dev_priv->dpll.shared_dplls[i];

		swap(pll->state, shared_dpll[i]);
	}
}

static bool ibx_pch_dpll_get_hw_state(struct drm_i915_private *dev_priv,
				      struct intel_shared_dpll *pll,
				      struct intel_dpll_hw_state *hw_state)
{
	const enum intel_dpll_id id = pll->info->id;
	intel_wakeref_t wakeref;
	u32 val;

	wakeref = intel_display_power_get_if_enabled(dev_priv,
						     POWER_DOMAIN_DISPLAY_CORE);
	if (!wakeref)
		return false;

	val = intel_de_read(dev_priv, PCH_DPLL(id));
	hw_state->dpll = val;
	hw_state->fp0 = intel_de_read(dev_priv, PCH_FP0(id));
	hw_state->fp1 = intel_de_read(dev_priv, PCH_FP1(id));

	intel_display_power_put(dev_priv, POWER_DOMAIN_DISPLAY_CORE, wakeref);

	return val & DPLL_VCO_ENABLE;
}

static void ibx_pch_dpll_prepare(struct drm_i915_private *dev_priv,
				 struct intel_shared_dpll *pll)
{
	const enum intel_dpll_id id = pll->info->id;

	intel_de_write(dev_priv, PCH_FP0(id), pll->state.hw_state.fp0);
	intel_de_write(dev_priv, PCH_FP1(id), pll->state.hw_state.fp1);
}

static void ibx_assert_pch_refclk_enabled(struct drm_i915_private *dev_priv)
{
	u32 val;
	bool enabled;

	I915_STATE_WARN_ON(!(HAS_PCH_IBX(dev_priv) || HAS_PCH_CPT(dev_priv)));

	val = intel_de_read(dev_priv, PCH_DREF_CONTROL);
	enabled = !!(val & (DREF_SSC_SOURCE_MASK | DREF_NONSPREAD_SOURCE_MASK |
			    DREF_SUPERSPREAD_SOURCE_MASK));
	I915_STATE_WARN(!enabled, "PCH refclk assertion failure, should be active but is disabled\n");
}

static void ibx_pch_dpll_enable(struct drm_i915_private *dev_priv,
				struct intel_shared_dpll *pll)
{
	const enum intel_dpll_id id = pll->info->id;

	/* PCH refclock must be enabled first */
	ibx_assert_pch_refclk_enabled(dev_priv);

	intel_de_write(dev_priv, PCH_DPLL(id), pll->state.hw_state.dpll);

	/* Wait for the clocks to stabilize. */
	intel_de_posting_read(dev_priv, PCH_DPLL(id));
	udelay(150);

	/* The pixel multiplier can only be updated once the
	 * DPLL is enabled and the clocks are stable.
	 *
	 * So write it again.
	 */
	intel_de_write(dev_priv, PCH_DPLL(id), pll->state.hw_state.dpll);
	intel_de_posting_read(dev_priv, PCH_DPLL(id));
	udelay(200);
}

static void ibx_pch_dpll_disable(struct drm_i915_private *dev_priv,
				 struct intel_shared_dpll *pll)
{
	const enum intel_dpll_id id = pll->info->id;

	intel_de_write(dev_priv, PCH_DPLL(id), 0);
	intel_de_posting_read(dev_priv, PCH_DPLL(id));
	udelay(200);
}

static bool ibx_get_dpll(struct intel_atomic_state *state,
			 struct intel_crtc *crtc,
			 struct intel_encoder *encoder)
{
	struct intel_crtc_state *crtc_state =
		intel_atomic_get_new_crtc_state(state, crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	struct intel_shared_dpll *pll;
	enum intel_dpll_id i;

	if (HAS_PCH_IBX(dev_priv)) {
		/* Ironlake PCH has a fixed PLL->PCH pipe mapping. */
		i = (enum intel_dpll_id) crtc->pipe;
		pll = &dev_priv->dpll.shared_dplls[i];

		drm_dbg_kms(&dev_priv->drm,
			    "[CRTC:%d:%s] using pre-allocated %s\n",
			    crtc->base.base.id, crtc->base.name,
			    pll->info->name);
	} else {
		pll = intel_find_shared_dpll(state, crtc,
					     &crtc_state->dpll_hw_state,
					     BIT(DPLL_ID_PCH_PLL_B) |
					     BIT(DPLL_ID_PCH_PLL_A));
	}

	if (!pll)
		return false;

	/* reference the pll */
	intel_reference_shared_dpll(state, crtc,
				    pll, &crtc_state->dpll_hw_state);

	crtc_state->shared_dpll = pll;

	return true;
}

static void ibx_dump_hw_state(struct drm_i915_private *dev_priv,
			      const struct intel_dpll_hw_state *hw_state)
{
	drm_dbg_kms(&dev_priv->drm,
		    "dpll_hw_state: dpll: 0x%x, dpll_md: 0x%x, "
		    "fp0: 0x%x, fp1: 0x%x\n",
		    hw_state->dpll,
		    hw_state->dpll_md,
		    hw_state->fp0,
		    hw_state->fp1);
}

static const struct intel_shared_dpll_funcs ibx_pch_dpll_funcs = {
	.prepare = ibx_pch_dpll_prepare,
	.enable = ibx_pch_dpll_enable,
	.disable = ibx_pch_dpll_disable,
	.get_hw_state = ibx_pch_dpll_get_hw_state,
};

static const struct dpll_info pch_plls[] = {
	{ "PCH DPLL A", &ibx_pch_dpll_funcs, DPLL_ID_PCH_PLL_A, 0 },
	{ "PCH DPLL B", &ibx_pch_dpll_funcs, DPLL_ID_PCH_PLL_B, 0 },
	{ },
};

static const struct intel_dpll_mgr pch_pll_mgr = {
	.dpll_info = pch_plls,
	.get_dplls = ibx_get_dpll,
	.put_dplls = intel_put_dpll,
	.dump_hw_state = ibx_dump_hw_state,
};

static void hsw_ddi_wrpll_enable(struct drm_i915_private *dev_priv,
			       struct intel_shared_dpll *pll)
{
	const enum intel_dpll_id id = pll->info->id;

	intel_de_write(dev_priv, WRPLL_CTL(id), pll->state.hw_state.wrpll);
	intel_de_posting_read(dev_priv, WRPLL_CTL(id));
	udelay(20);
}

static void hsw_ddi_spll_enable(struct drm_i915_private *dev_priv,
				struct intel_shared_dpll *pll)
{
	intel_de_write(dev_priv, SPLL_CTL, pll->state.hw_state.spll);
	intel_de_posting_read(dev_priv, SPLL_CTL);
	udelay(20);
}

static void hsw_ddi_wrpll_disable(struct drm_i915_private *dev_priv,
				  struct intel_shared_dpll *pll)
{
	const enum intel_dpll_id id = pll->info->id;
	u32 val;

	val = intel_de_read(dev_priv, WRPLL_CTL(id));
	intel_de_write(dev_priv, WRPLL_CTL(id), val & ~WRPLL_PLL_ENABLE);
	intel_de_posting_read(dev_priv, WRPLL_CTL(id));

	/*
	 * Try to set up the PCH reference clock once all DPLLs
	 * that depend on it have been shut down.
	 */
	if (dev_priv->pch_ssc_use & BIT(id))
		intel_init_pch_refclk(dev_priv);
}

static void hsw_ddi_spll_disable(struct drm_i915_private *dev_priv,
				 struct intel_shared_dpll *pll)
{
	enum intel_dpll_id id = pll->info->id;
	u32 val;

	val = intel_de_read(dev_priv, SPLL_CTL);
	intel_de_write(dev_priv, SPLL_CTL, val & ~SPLL_PLL_ENABLE);
	intel_de_posting_read(dev_priv, SPLL_CTL);

	/*
	 * Try to set up the PCH reference clock once all DPLLs
	 * that depend on it have been shut down.
	 */
	if (dev_priv->pch_ssc_use & BIT(id))
		intel_init_pch_refclk(dev_priv);
}

static bool hsw_ddi_wrpll_get_hw_state(struct drm_i915_private *dev_priv,
				       struct intel_shared_dpll *pll,
				       struct intel_dpll_hw_state *hw_state)
{
	const enum intel_dpll_id id = pll->info->id;
	intel_wakeref_t wakeref;
	u32 val;

	wakeref = intel_display_power_get_if_enabled(dev_priv,
						     POWER_DOMAIN_DISPLAY_CORE);
	if (!wakeref)
		return false;

	val = intel_de_read(dev_priv, WRPLL_CTL(id));
	hw_state->wrpll = val;

	intel_display_power_put(dev_priv, POWER_DOMAIN_DISPLAY_CORE, wakeref);

	return val & WRPLL_PLL_ENABLE;
}

static bool hsw_ddi_spll_get_hw_state(struct drm_i915_private *dev_priv,
				      struct intel_shared_dpll *pll,
				      struct intel_dpll_hw_state *hw_state)
{
	intel_wakeref_t wakeref;
	u32 val;

	wakeref = intel_display_power_get_if_enabled(dev_priv,
						     POWER_DOMAIN_DISPLAY_CORE);
	if (!wakeref)
		return false;

	val = intel_de_read(dev_priv, SPLL_CTL);
	hw_state->spll = val;

	intel_display_power_put(dev_priv, POWER_DOMAIN_DISPLAY_CORE, wakeref);

	return val & SPLL_PLL_ENABLE;
}

#define LC_FREQ 2700
#define LC_FREQ_2K U64_C(LC_FREQ * 2000)

#define P_MIN 2
#define P_MAX 64
#define P_INC 2

/* Constraints for PLL good behavior */
#define REF_MIN 48
#define REF_MAX 400
#define VCO_MIN 2400
#define VCO_MAX 4800

struct hsw_wrpll_rnp {
	unsigned p, n2, r2;
};

static unsigned hsw_wrpll_get_budget_for_freq(int clock)
{
	unsigned budget;

	switch (clock) {
	case 25175000:
	case 25200000:
	case 27000000:
	case 27027000:
	case 37762500:
	case 37800000:
	case 40500000:
	case 40541000:
	case 54000000:
	case 54054000:
	case 59341000:
	case 59400000:
	case 72000000:
	case 74176000:
	case 74250000:
	case 81000000:
	case 81081000:
	case 89012000:
	case 89100000:
	case 108000000:
	case 108108000:
	case 111264000:
	case 111375000:
	case 148352000:
	case 148500000:
	case 162000000:
	case 162162000:
	case 222525000:
	case 222750000:
	case 296703000:
	case 297000000:
		budget = 0;
		break;
	case 233500000:
	case 245250000:
	case 247750000:
	case 253250000:
	case 298000000:
		budget = 1500;
		break;
	case 169128000:
	case 169500000:
	case 179500000:
	case 202000000:
		budget = 2000;
		break;
	case 256250000:
	case 262500000:
	case 270000000:
	case 272500000:
	case 273750000:
	case 280750000:
	case 281250000:
	case 286000000:
	case 291750000:
		budget = 4000;
		break;
	case 267250000:
	case 268500000:
		budget = 5000;
		break;
	default:
		budget = 1000;
		break;
	}

	return budget;
}

static void hsw_wrpll_update_rnp(u64 freq2k, unsigned int budget,
				 unsigned int r2, unsigned int n2,
				 unsigned int p,
				 struct hsw_wrpll_rnp *best)
{
	u64 a, b, c, d, diff, diff_best;

	/* No best (r,n,p) yet */
	if (best->p == 0) {
		best->p = p;
		best->n2 = n2;
		best->r2 = r2;
		return;
	}

	/*
	 * Output clock is (LC_FREQ_2K / 2000) * N / (P * R), which compares to
	 * freq2k.
	 *
	 * delta = 1e6 *
	 *	   abs(freq2k - (LC_FREQ_2K * n2/(p * r2))) /
	 *	   freq2k;
	 *
	 * and we would like delta <= budget.
	 *
	 * If the discrepancy is above the PPM-based budget, always prefer to
	 * improve upon the previous solution.  However, if you're within the
	 * budget, try to maximize Ref * VCO, that is N / (P * R^2).
	 */
	a = freq2k * budget * p * r2;
	b = freq2k * budget * best->p * best->r2;
	diff = abs_diff(freq2k * p * r2, LC_FREQ_2K * n2);
	diff_best = abs_diff(freq2k * best->p * best->r2,
			     LC_FREQ_2K * best->n2);
	c = 1000000 * diff;
	d = 1000000 * diff_best;

	if (a < c && b < d) {
		/* If both are above the budget, pick the closer */
		if (best->p * best->r2 * diff < p * r2 * diff_best) {
			best->p = p;
			best->n2 = n2;
			best->r2 = r2;
		}
	} else if (a >= c && b < d) {
		/* If A is below the threshold but B is above it?  Update. */
		best->p = p;
		best->n2 = n2;
		best->r2 = r2;
	} else if (a >= c && b >= d) {
		/* Both are below the limit, so pick the higher n2/(r2*r2) */
		if (n2 * best->r2 * best->r2 > best->n2 * r2 * r2) {
			best->p = p;
			best->n2 = n2;
			best->r2 = r2;
		}
	}
	/* Otherwise a < c && b >= d, do nothing */
}

static void
hsw_ddi_calculate_wrpll(int clock /* in Hz */,
			unsigned *r2_out, unsigned *n2_out, unsigned *p_out)
{
	u64 freq2k;
	unsigned p, n2, r2;
	struct hsw_wrpll_rnp best = { 0, 0, 0 };
	unsigned budget;

	freq2k = clock / 100;

	budget = hsw_wrpll_get_budget_for_freq(clock);

	/* Special case handling for 540 pixel clock: bypass WR PLL entirely
	 * and directly pass the LC PLL to it. */
	if (freq2k == 5400000) {
		*n2_out = 2;
		*p_out = 1;
		*r2_out = 2;
		return;
	}

	/*
	 * Ref = LC_FREQ / R, where Ref is the actual reference input seen by
	 * the WR PLL.
	 *
	 * We want R so that REF_MIN <= Ref <= REF_MAX.
	 * Injecting R2 = 2 * R gives:
	 *   REF_MAX * r2 > LC_FREQ * 2 and
	 *   REF_MIN * r2 < LC_FREQ * 2
	 *
	 * Which means the desired boundaries for r2 are:
	 *  LC_FREQ * 2 / REF_MAX < r2 < LC_FREQ * 2 / REF_MIN
	 *
	 */
	for (r2 = LC_FREQ * 2 / REF_MAX + 1;
	     r2 <= LC_FREQ * 2 / REF_MIN;
	     r2++) {

		/*
		 * VCO = N * Ref, that is: VCO = N * LC_FREQ / R
		 *
		 * Once again we want VCO_MIN <= VCO <= VCO_MAX.
		 * Injecting R2 = 2 * R and N2 = 2 * N, we get:
		 *   VCO_MAX * r2 > n2 * LC_FREQ and
		 *   VCO_MIN * r2 < n2 * LC_FREQ)
		 *
		 * Which means the desired boundaries for n2 are:
		 * VCO_MIN * r2 / LC_FREQ < n2 < VCO_MAX * r2 / LC_FREQ
		 */
		for (n2 = VCO_MIN * r2 / LC_FREQ + 1;
		     n2 <= VCO_MAX * r2 / LC_FREQ;
		     n2++) {

			for (p = P_MIN; p <= P_MAX; p += P_INC)
				hsw_wrpll_update_rnp(freq2k, budget,
						     r2, n2, p, &best);
		}
	}

	*n2_out = best.n2;
	*p_out = best.p;
	*r2_out = best.r2;
}

static struct intel_shared_dpll *
hsw_ddi_wrpll_get_dpll(struct intel_atomic_state *state,
		       struct intel_crtc *crtc)
{
	struct intel_crtc_state *crtc_state =
		intel_atomic_get_new_crtc_state(state, crtc);
	struct intel_shared_dpll *pll;
	u32 val;
	unsigned int p, n2, r2;

	hsw_ddi_calculate_wrpll(crtc_state->port_clock * 1000, &r2, &n2, &p);

	val = WRPLL_PLL_ENABLE | WRPLL_REF_LCPLL |
	      WRPLL_DIVIDER_REFERENCE(r2) | WRPLL_DIVIDER_FEEDBACK(n2) |
	      WRPLL_DIVIDER_POST(p);

	crtc_state->dpll_hw_state.wrpll = val;

	pll = intel_find_shared_dpll(state, crtc,
				     &crtc_state->dpll_hw_state,
				     BIT(DPLL_ID_WRPLL2) |
				     BIT(DPLL_ID_WRPLL1));

	if (!pll)
		return NULL;

	return pll;
}

static int hsw_ddi_wrpll_get_freq(struct drm_i915_private *dev_priv,
				  const struct intel_shared_dpll *pll,
				  const struct intel_dpll_hw_state *pll_state)
{
	int refclk;
	int n, p, r;
	u32 wrpll = pll_state->wrpll;

	switch (wrpll & WRPLL_REF_MASK) {
	case WRPLL_REF_SPECIAL_HSW:
		/* Muxed-SSC for BDW, non-SSC for non-ULT HSW. */
		if (IS_HASWELL(dev_priv) && !IS_HSW_ULT(dev_priv)) {
			refclk = dev_priv->dpll.ref_clks.nssc;
			break;
		}
		fallthrough;
	case WRPLL_REF_PCH_SSC:
		/*
		 * We could calculate spread here, but our checking
		 * code only cares about 5% accuracy, and spread is a max of
		 * 0.5% downspread.
		 */
		refclk = dev_priv->dpll.ref_clks.ssc;
		break;
	case WRPLL_REF_LCPLL:
		refclk = 2700000;
		break;
	default:
		MISSING_CASE(wrpll);
		return 0;
	}

	r = wrpll & WRPLL_DIVIDER_REF_MASK;
	p = (wrpll & WRPLL_DIVIDER_POST_MASK) >> WRPLL_DIVIDER_POST_SHIFT;
	n = (wrpll & WRPLL_DIVIDER_FB_MASK) >> WRPLL_DIVIDER_FB_SHIFT;

	/* Convert to KHz, p & r have a fixed point portion */
	return (refclk * n / 10) / (p * r) * 2;
}

static struct intel_shared_dpll *
hsw_ddi_lcpll_get_dpll(struct intel_crtc_state *crtc_state)
{
	struct drm_i915_private *dev_priv = to_i915(crtc_state->uapi.crtc->dev);
	struct intel_shared_dpll *pll;
	enum intel_dpll_id pll_id;
	int clock = crtc_state->port_clock;

	switch (clock / 2) {
	case 81000:
		pll_id = DPLL_ID_LCPLL_810;
		break;
	case 135000:
		pll_id = DPLL_ID_LCPLL_1350;
		break;
	case 270000:
		pll_id = DPLL_ID_LCPLL_2700;
		break;
	default:
		drm_dbg_kms(&dev_priv->drm, "Invalid clock for DP: %d\n",
			    clock);
		return NULL;
	}

	pll = intel_get_shared_dpll_by_id(dev_priv, pll_id);

	if (!pll)
		return NULL;

	return pll;
}

static int hsw_ddi_lcpll_get_freq(struct drm_i915_private *i915,
				  const struct intel_shared_dpll *pll,
				  const struct intel_dpll_hw_state *pll_state)
{
	int link_clock = 0;

	switch (pll->info->id) {
	case DPLL_ID_LCPLL_810:
		link_clock = 81000;
		break;
	case DPLL_ID_LCPLL_1350:
		link_clock = 135000;
		break;
	case DPLL_ID_LCPLL_2700:
		link_clock = 270000;
		break;
	default:
		drm_WARN(&i915->drm, 1, "bad port clock sel\n");
		break;
	}

	return link_clock * 2;
}

static struct intel_shared_dpll *
hsw_ddi_spll_get_dpll(struct intel_atomic_state *state,
		      struct intel_crtc *crtc)
{
	struct intel_crtc_state *crtc_state =
		intel_atomic_get_new_crtc_state(state, crtc);

	if (drm_WARN_ON(crtc->base.dev, crtc_state->port_clock / 2 != 135000))
		return NULL;

	crtc_state->dpll_hw_state.spll = SPLL_PLL_ENABLE | SPLL_FREQ_1350MHz |
					 SPLL_REF_MUXED_SSC;

	return intel_find_shared_dpll(state, crtc, &crtc_state->dpll_hw_state,
				      BIT(DPLL_ID_SPLL));
}

static int hsw_ddi_spll_get_freq(struct drm_i915_private *i915,
				 const struct intel_shared_dpll *pll,
				 const struct intel_dpll_hw_state *pll_state)
{
	int link_clock = 0;

	switch (pll_state->spll & SPLL_FREQ_MASK) {
	case SPLL_FREQ_810MHz:
		link_clock = 81000;
		break;
	case SPLL_FREQ_1350MHz:
		link_clock = 135000;
		break;
	case SPLL_FREQ_2700MHz:
		link_clock = 270000;
		break;
	default:
		drm_WARN(&i915->drm, 1, "bad spll freq\n");
		break;
	}

	return link_clock * 2;
}

static bool hsw_get_dpll(struct intel_atomic_state *state,
			 struct intel_crtc *crtc,
			 struct intel_encoder *encoder)
{
	struct intel_crtc_state *crtc_state =
		intel_atomic_get_new_crtc_state(state, crtc);
	struct intel_shared_dpll *pll;

	memset(&crtc_state->dpll_hw_state, 0,
	       sizeof(crtc_state->dpll_hw_state));

	if (intel_crtc_has_type(crtc_state, INTEL_OUTPUT_HDMI))
		pll = hsw_ddi_wrpll_get_dpll(state, crtc);
	else if (intel_crtc_has_dp_encoder(crtc_state))
		pll = hsw_ddi_lcpll_get_dpll(crtc_state);
	else if (intel_crtc_has_type(crtc_state, INTEL_OUTPUT_ANALOG))
		pll = hsw_ddi_spll_get_dpll(state, crtc);
	else
		return false;

	if (!pll)
		return false;

	intel_reference_shared_dpll(state, crtc,
				    pll, &crtc_state->dpll_hw_state);

	crtc_state->shared_dpll = pll;

	return true;
}

static void hsw_update_dpll_ref_clks(struct drm_i915_private *i915)
{
	i915->dpll.ref_clks.ssc = 135000;
	/* Non-SSC is only used on non-ULT HSW. */
	if (intel_de_read(i915, FUSE_STRAP3) & HSW_REF_CLK_SELECT)
		i915->dpll.ref_clks.nssc = 24000;
	else
		i915->dpll.ref_clks.nssc = 135000;
}

static void hsw_dump_hw_state(struct drm_i915_private *dev_priv,
			      const struct intel_dpll_hw_state *hw_state)
{
	drm_dbg_kms(&dev_priv->drm, "dpll_hw_state: wrpll: 0x%x spll: 0x%x\n",
		    hw_state->wrpll, hw_state->spll);
}

static const struct intel_shared_dpll_funcs hsw_ddi_wrpll_funcs = {
	.enable = hsw_ddi_wrpll_enable,
	.disable = hsw_ddi_wrpll_disable,
	.get_hw_state = hsw_ddi_wrpll_get_hw_state,
	.get_freq = hsw_ddi_wrpll_get_freq,
};

static const struct intel_shared_dpll_funcs hsw_ddi_spll_funcs = {
	.enable = hsw_ddi_spll_enable,
	.disable = hsw_ddi_spll_disable,
	.get_hw_state = hsw_ddi_spll_get_hw_state,
	.get_freq = hsw_ddi_spll_get_freq,
};

static void hsw_ddi_lcpll_enable(struct drm_i915_private *dev_priv,
				 struct intel_shared_dpll *pll)
{
}

static void hsw_ddi_lcpll_disable(struct drm_i915_private *dev_priv,
				  struct intel_shared_dpll *pll)
{
}

static bool hsw_ddi_lcpll_get_hw_state(struct drm_i915_private *dev_priv,
				       struct intel_shared_dpll *pll,
				       struct intel_dpll_hw_state *hw_state)
{
	return true;
}

static const struct intel_shared_dpll_funcs hsw_ddi_lcpll_funcs = {
	.enable = hsw_ddi_lcpll_enable,
	.disable = hsw_ddi_lcpll_disable,
	.get_hw_state = hsw_ddi_lcpll_get_hw_state,
	.get_freq = hsw_ddi_lcpll_get_freq,
};

static const struct dpll_info hsw_plls[] = {
	{ "WRPLL 1",    &hsw_ddi_wrpll_funcs, DPLL_ID_WRPLL1,     0 },
	{ "WRPLL 2",    &hsw_ddi_wrpll_funcs, DPLL_ID_WRPLL2,     0 },
	{ "SPLL",       &hsw_ddi_spll_funcs,  DPLL_ID_SPLL,       0 },
	{ "LCPLL 810",  &hsw_ddi_lcpll_funcs, DPLL_ID_LCPLL_810,  INTEL_DPLL_ALWAYS_ON },
	{ "LCPLL 1350", &hsw_ddi_lcpll_funcs, DPLL_ID_LCPLL_1350, INTEL_DPLL_ALWAYS_ON },
	{ "LCPLL 2700", &hsw_ddi_lcpll_funcs, DPLL_ID_LCPLL_2700, INTEL_DPLL_ALWAYS_ON },
	{ },
};

static const struct intel_dpll_mgr hsw_pll_mgr = {
	.dpll_info = hsw_plls,
	.get_dplls = hsw_get_dpll,
	.put_dplls = intel_put_dpll,
	.update_ref_clks = hsw_update_dpll_ref_clks,
	.dump_hw_state = hsw_dump_hw_state,
};

struct skl_dpll_regs {
	i915_reg_t ctl, cfgcr1, cfgcr2;
};

/* this array is indexed by the *shared* pll id */
static const struct skl_dpll_regs skl_dpll_regs[4] = {
	{
		/* DPLL 0 */
		.ctl = LCPLL1_CTL,
		/* DPLL 0 doesn't support HDMI mode */
	},
	{
		/* DPLL 1 */
		.ctl = LCPLL2_CTL,
		.cfgcr1 = DPLL_CFGCR1(SKL_DPLL1),
		.cfgcr2 = DPLL_CFGCR2(SKL_DPLL1),
	},
	{
		/* DPLL 2 */
		.ctl = WRPLL_CTL(0),
		.cfgcr1 = DPLL_CFGCR1(SKL_DPLL2),
		.cfgcr2 = DPLL_CFGCR2(SKL_DPLL2),
	},
	{
		/* DPLL 3 */
		.ctl = WRPLL_CTL(1),
		.cfgcr1 = DPLL_CFGCR1(SKL_DPLL3),
		.cfgcr2 = DPLL_CFGCR2(SKL_DPLL3),
	},
};

static void skl_ddi_pll_write_ctrl1(struct drm_i915_private *dev_priv,
				    struct intel_shared_dpll *pll)
{
	const enum intel_dpll_id id = pll->info->id;
	u32 val;

	val = intel_de_read(dev_priv, DPLL_CTRL1);

	val &= ~(DPLL_CTRL1_HDMI_MODE(id) |
		 DPLL_CTRL1_SSC(id) |
		 DPLL_CTRL1_LINK_RATE_MASK(id));
	val |= pll->state.hw_state.ctrl1 << (id * 6);

	intel_de_write(dev_priv, DPLL_CTRL1, val);
	intel_de_posting_read(dev_priv, DPLL_CTRL1);
}

static void skl_ddi_pll_enable(struct drm_i915_private *dev_priv,
			       struct intel_shared_dpll *pll)
{
	const struct skl_dpll_regs *regs = skl_dpll_regs;
	const enum intel_dpll_id id = pll->info->id;

	skl_ddi_pll_write_ctrl1(dev_priv, pll);

	intel_de_write(dev_priv, regs[id].cfgcr1, pll->state.hw_state.cfgcr1);
	intel_de_write(dev_priv, regs[id].cfgcr2, pll->state.hw_state.cfgcr2);
	intel_de_posting_read(dev_priv, regs[id].cfgcr1);
	intel_de_posting_read(dev_priv, regs[id].cfgcr2);

	/* the enable bit is always bit 31 */
	intel_de_write(dev_priv, regs[id].ctl,
		       intel_de_read(dev_priv, regs[id].ctl) | LCPLL_PLL_ENABLE);

	if (intel_de_wait_for_set(dev_priv, DPLL_STATUS, DPLL_LOCK(id), 5))
		drm_err(&dev_priv->drm, "DPLL %d not locked\n", id);
}

static void skl_ddi_dpll0_enable(struct drm_i915_private *dev_priv,
				 struct intel_shared_dpll *pll)
{
	skl_ddi_pll_write_ctrl1(dev_priv, pll);
}

static void skl_ddi_pll_disable(struct drm_i915_private *dev_priv,
				struct intel_shared_dpll *pll)
{
	const struct skl_dpll_regs *regs = skl_dpll_regs;
	const enum intel_dpll_id id = pll->info->id;

	/* the enable bit is always bit 31 */
	intel_de_write(dev_priv, regs[id].ctl,
		       intel_de_read(dev_priv, regs[id].ctl) & ~LCPLL_PLL_ENABLE);
	intel_de_posting_read(dev_priv, regs[id].ctl);
}

static void skl_ddi_dpll0_disable(struct drm_i915_private *dev_priv,
				  struct intel_shared_dpll *pll)
{
}

static bool skl_ddi_pll_get_hw_state(struct drm_i915_private *dev_priv,
				     struct intel_shared_dpll *pll,
				     struct intel_dpll_hw_state *hw_state)
{
	u32 val;
	const struct skl_dpll_regs *regs = skl_dpll_regs;
	const enum intel_dpll_id id = pll->info->id;
	intel_wakeref_t wakeref;
	bool ret;

	wakeref = intel_display_power_get_if_enabled(dev_priv,
						     POWER_DOMAIN_DISPLAY_CORE);
	if (!wakeref)
		return false;

	ret = false;

	val = intel_de_read(dev_priv, regs[id].ctl);
	if (!(val & LCPLL_PLL_ENABLE))
		goto out;

	val = intel_de_read(dev_priv, DPLL_CTRL1);
	hw_state->ctrl1 = (val >> (id * 6)) & 0x3f;

	/* avoid reading back stale values if HDMI mode is not enabled */
	if (val & DPLL_CTRL1_HDMI_MODE(id)) {
		hw_state->cfgcr1 = intel_de_read(dev_priv, regs[id].cfgcr1);
		hw_state->cfgcr2 = intel_de_read(dev_priv, regs[id].cfgcr2);
	}
	ret = true;

out:
	intel_display_power_put(dev_priv, POWER_DOMAIN_DISPLAY_CORE, wakeref);

	return ret;
}

static bool skl_ddi_dpll0_get_hw_state(struct drm_i915_private *dev_priv,
				       struct intel_shared_dpll *pll,
				       struct intel_dpll_hw_state *hw_state)
{
	const struct skl_dpll_regs *regs = skl_dpll_regs;
	const enum intel_dpll_id id = pll->info->id;
	intel_wakeref_t wakeref;
	u32 val;
	bool ret;

	wakeref = intel_display_power_get_if_enabled(dev_priv,
						     POWER_DOMAIN_DISPLAY_CORE);
	if (!wakeref)
		return false;

	ret = false;

	/* DPLL0 is always enabled since it drives CDCLK */
	val = intel_de_read(dev_priv, regs[id].ctl);
	if (drm_WARN_ON(&dev_priv->drm, !(val & LCPLL_PLL_ENABLE)))
		goto out;

	val = intel_de_read(dev_priv, DPLL_CTRL1);
	hw_state->ctrl1 = (val >> (id * 6)) & 0x3f;

	ret = true;

out:
	intel_display_power_put(dev_priv, POWER_DOMAIN_DISPLAY_CORE, wakeref);

	return ret;
}

struct skl_wrpll_context {
	u64 min_deviation;		/* current minimal deviation */
	u64 central_freq;		/* chosen central freq */
	u64 dco_freq;			/* chosen dco freq */
	unsigned int p;			/* chosen divider */
};

static void skl_wrpll_context_init(struct skl_wrpll_context *ctx)
{
	memset(ctx, 0, sizeof(*ctx));

	ctx->min_deviation = U64_MAX;
}

/* DCO freq must be within +1%/-6%  of the DCO central freq */
#define SKL_DCO_MAX_PDEVIATION	100
#define SKL_DCO_MAX_NDEVIATION	600

static void skl_wrpll_try_divider(struct skl_wrpll_context *ctx,
				  u64 central_freq,
				  u64 dco_freq,
				  unsigned int divider)
{
	u64 deviation;

	deviation = div64_u64(10000 * abs_diff(dco_freq, central_freq),
			      central_freq);

	/* positive deviation */
	if (dco_freq >= central_freq) {
		if (deviation < SKL_DCO_MAX_PDEVIATION &&
		    deviation < ctx->min_deviation) {
			ctx->min_deviation = deviation;
			ctx->central_freq = central_freq;
			ctx->dco_freq = dco_freq;
			ctx->p = divider;
		}
	/* negative deviation */
	} else if (deviation < SKL_DCO_MAX_NDEVIATION &&
		   deviation < ctx->min_deviation) {
		ctx->min_deviation = deviation;
		ctx->central_freq = central_freq;
		ctx->dco_freq = dco_freq;
		ctx->p = divider;
	}
}

static void skl_wrpll_get_multipliers(unsigned int p,
				      unsigned int *p0 /* out */,
				      unsigned int *p1 /* out */,
				      unsigned int *p2 /* out */)
{
	/* even dividers */
	if (p % 2 == 0) {
		unsigned int half = p / 2;

		if (half == 1 || half == 2 || half == 3 || half == 5) {
			*p0 = 2;
			*p1 = 1;
			*p2 = half;
		} else if (half % 2 == 0) {
			*p0 = 2;
			*p1 = half / 2;
			*p2 = 2;
		} else if (half % 3 == 0) {
			*p0 = 3;
			*p1 = half / 3;
			*p2 = 2;
		} else if (half % 7 == 0) {
			*p0 = 7;
			*p1 = half / 7;
			*p2 = 2;
		}
	} else if (p == 3 || p == 9) {  /* 3, 5, 7, 9, 15, 21, 35 */
		*p0 = 3;
		*p1 = 1;
		*p2 = p / 3;
	} else if (p == 5 || p == 7) {
		*p0 = p;
		*p1 = 1;
		*p2 = 1;
	} else if (p == 15) {
		*p0 = 3;
		*p1 = 1;
		*p2 = 5;
	} else if (p == 21) {
		*p0 = 7;
		*p1 = 1;
		*p2 = 3;
	} else if (p == 35) {
		*p0 = 7;
		*p1 = 1;
		*p2 = 5;
	}
}

struct skl_wrpll_params {
	u32 dco_fraction;
	u32 dco_integer;
	u32 qdiv_ratio;
	u32 qdiv_mode;
	u32 kdiv;
	u32 pdiv;
	u32 central_freq;
};

static void skl_wrpll_params_populate(struct skl_wrpll_params *params,
				      u64 afe_clock,
				      int ref_clock,
				      u64 central_freq,
				      u32 p0, u32 p1, u32 p2)
{
	u64 dco_freq;

	switch (central_freq) {
	case 9600000000ULL:
		params->central_freq = 0;
		break;
	case 9000000000ULL:
		params->central_freq = 1;
		break;
	case 8400000000ULL:
		params->central_freq = 3;
	}

	switch (p0) {
	case 1:
		params->pdiv = 0;
		break;
	case 2:
		params->pdiv = 1;
		break;
	case 3:
		params->pdiv = 2;
		break;
	case 7:
		params->pdiv = 4;
		break;
	default:
		WARN(1, "Incorrect PDiv\n");
	}

	switch (p2) {
	case 5:
		params->kdiv = 0;
		break;
	case 2:
		params->kdiv = 1;
		break;
	case 3:
		params->kdiv = 2;
		break;
	case 1:
		params->kdiv = 3;
		break;
	default:
		WARN(1, "Incorrect KDiv\n");
	}

	params->qdiv_ratio = p1;
	params->qdiv_mode = (params->qdiv_ratio == 1) ? 0 : 1;

	dco_freq = p0 * p1 * p2 * afe_clock;

	/*
	 * Intermediate values are in Hz.
	 * Divide by MHz to match bsepc
	 */
	params->dco_integer = div_u64(dco_freq, ref_clock * KHz(1));
	params->dco_fraction =
		div_u64((div_u64(dco_freq, ref_clock / KHz(1)) -
			 params->dco_integer * MHz(1)) * 0x8000, MHz(1));
}

static bool
skl_ddi_calculate_wrpll(int clock /* in Hz */,
			int ref_clock,
			struct skl_wrpll_params *wrpll_params)
{
	u64 afe_clock = clock * 5; /* AFE Clock is 5x Pixel clock */
	u64 dco_central_freq[3] = { 8400000000ULL,
				    9000000000ULL,
				    9600000000ULL };
	static const int even_dividers[] = {  4,  6,  8, 10, 12, 14, 16, 18, 20,
					     24, 28, 30, 32, 36, 40, 42, 44,
					     48, 52, 54, 56, 60, 64, 66, 68,
					     70, 72, 76, 78, 80, 84, 88, 90,
					     92, 96, 98 };
	static const int odd_dividers[] = { 3, 5, 7, 9, 15, 21, 35 };
	static const struct {
		const int *list;
		int n_dividers;
	} dividers[] = {
		{ even_dividers, ARRAY_SIZE(even_dividers) },
		{ odd_dividers, ARRAY_SIZE(odd_dividers) },
	};
	struct skl_wrpll_context ctx;
	unsigned int dco, d, i;
	unsigned int p0, p1, p2;

	skl_wrpll_context_init(&ctx);

	for (d = 0; d < ARRAY_SIZE(dividers); d++) {
		for (dco = 0; dco < ARRAY_SIZE(dco_central_freq); dco++) {
			for (i = 0; i < dividers[d].n_dividers; i++) {
				unsigned int p = dividers[d].list[i];
				u64 dco_freq = p * afe_clock;

				skl_wrpll_try_divider(&ctx,
						      dco_central_freq[dco],
						      dco_freq,
						      p);
				/*
				 * Skip the remaining dividers if we're sure to
				 * have found the definitive divider, we can't
				 * improve a 0 deviation.
				 */
				if (ctx.min_deviation == 0)
					goto skip_remaining_dividers;
			}
		}

skip_remaining_dividers:
		/*
		 * If a solution is found with an even divider, prefer
		 * this one.
		 */
		if (d == 0 && ctx.p)
			break;
	}

	if (!ctx.p) {
		DRM_DEBUG_DRIVER("No valid divider found for %dHz\n", clock);
		return false;
	}

	/*
	 * gcc incorrectly analyses that these can be used without being
	 * initialized. To be fair, it's hard to guess.
	 */
	p0 = p1 = p2 = 0;
	skl_wrpll_get_multipliers(ctx.p, &p0, &p1, &p2);
	skl_wrpll_params_populate(wrpll_params, afe_clock, ref_clock,
				  ctx.central_freq, p0, p1, p2);

	return true;
}

static bool skl_ddi_hdmi_pll_dividers(struct intel_crtc_state *crtc_state)
{
	struct drm_i915_private *i915 = to_i915(crtc_state->uapi.crtc->dev);
	u32 ctrl1, cfgcr1, cfgcr2;
	struct skl_wrpll_params wrpll_params = { 0, };

	/*
	 * See comment in intel_dpll_hw_state to understand why we always use 0
	 * as the DPLL id in this function.
	 */
	ctrl1 = DPLL_CTRL1_OVERRIDE(0);

	ctrl1 |= DPLL_CTRL1_HDMI_MODE(0);

	if (!skl_ddi_calculate_wrpll(crtc_state->port_clock * 1000,
				     i915->dpll.ref_clks.nssc,
				     &wrpll_params))
		return false;

	cfgcr1 = DPLL_CFGCR1_FREQ_ENABLE |
		DPLL_CFGCR1_DCO_FRACTION(wrpll_params.dco_fraction) |
		wrpll_params.dco_integer;

	cfgcr2 = DPLL_CFGCR2_QDIV_RATIO(wrpll_params.qdiv_ratio) |
		DPLL_CFGCR2_QDIV_MODE(wrpll_params.qdiv_mode) |
		DPLL_CFGCR2_KDIV(wrpll_params.kdiv) |
		DPLL_CFGCR2_PDIV(wrpll_params.pdiv) |
		wrpll_params.central_freq;

	memset(&crtc_state->dpll_hw_state, 0,
	       sizeof(crtc_state->dpll_hw_state));

	crtc_state->dpll_hw_state.ctrl1 = ctrl1;
	crtc_state->dpll_hw_state.cfgcr1 = cfgcr1;
	crtc_state->dpll_hw_state.cfgcr2 = cfgcr2;
	return true;
}

static int skl_ddi_wrpll_get_freq(struct drm_i915_private *i915,
				  const struct intel_shared_dpll *pll,
				  const struct intel_dpll_hw_state *pll_state)
{
	int ref_clock = i915->dpll.ref_clks.nssc;
	u32 p0, p1, p2, dco_freq;

	p0 = pll_state->cfgcr2 & DPLL_CFGCR2_PDIV_MASK;
	p2 = pll_state->cfgcr2 & DPLL_CFGCR2_KDIV_MASK;

	if (pll_state->cfgcr2 &  DPLL_CFGCR2_QDIV_MODE(1))
		p1 = (pll_state->cfgcr2 & DPLL_CFGCR2_QDIV_RATIO_MASK) >> 8;
	else
		p1 = 1;


	switch (p0) {
	case DPLL_CFGCR2_PDIV_1:
		p0 = 1;
		break;
	case DPLL_CFGCR2_PDIV_2:
		p0 = 2;
		break;
	case DPLL_CFGCR2_PDIV_3:
		p0 = 3;
		break;
	case DPLL_CFGCR2_PDIV_7_INVALID:
		/*
		 * Incorrect ASUS-Z170M BIOS setting, the HW seems to ignore bit#0,
		 * handling it the same way as PDIV_7.
		 */
		drm_dbg_kms(&i915->drm, "Invalid WRPLL PDIV divider value, fixing it.\n");
		fallthrough;
	case DPLL_CFGCR2_PDIV_7:
		p0 = 7;
		break;
	default:
		MISSING_CASE(p0);
		return 0;
	}

	switch (p2) {
	case DPLL_CFGCR2_KDIV_5:
		p2 = 5;
		break;
	case DPLL_CFGCR2_KDIV_2:
		p2 = 2;
		break;
	case DPLL_CFGCR2_KDIV_3:
		p2 = 3;
		break;
	case DPLL_CFGCR2_KDIV_1:
		p2 = 1;
		break;
	default:
		MISSING_CASE(p2);
		return 0;
	}

	dco_freq = (pll_state->cfgcr1 & DPLL_CFGCR1_DCO_INTEGER_MASK) *
		   ref_clock;

	dco_freq += ((pll_state->cfgcr1 & DPLL_CFGCR1_DCO_FRACTION_MASK) >> 9) *
		    ref_clock / 0x8000;

	if (drm_WARN_ON(&i915->drm, p0 == 0 || p1 == 0 || p2 == 0))
		return 0;

	return dco_freq / (p0 * p1 * p2 * 5);
}

static bool
skl_ddi_dp_set_dpll_hw_state(struct intel_crtc_state *crtc_state)
{
	u32 ctrl1;

	/*
	 * See comment in intel_dpll_hw_state to understand why we always use 0
	 * as the DPLL id in this function.
	 */
	ctrl1 = DPLL_CTRL1_OVERRIDE(0);
	switch (crtc_state->port_clock / 2) {
	case 81000:
		ctrl1 |= DPLL_CTRL1_LINK_RATE(DPLL_CTRL1_LINK_RATE_810, 0);
		break;
	case 135000:
		ctrl1 |= DPLL_CTRL1_LINK_RATE(DPLL_CTRL1_LINK_RATE_1350, 0);
		break;
	case 270000:
		ctrl1 |= DPLL_CTRL1_LINK_RATE(DPLL_CTRL1_LINK_RATE_2700, 0);
		break;
		/* eDP 1.4 rates */
	case 162000:
		ctrl1 |= DPLL_CTRL1_LINK_RATE(DPLL_CTRL1_LINK_RATE_1620, 0);
		break;
	case 108000:
		ctrl1 |= DPLL_CTRL1_LINK_RATE(DPLL_CTRL1_LINK_RATE_1080, 0);
		break;
	case 216000:
		ctrl1 |= DPLL_CTRL1_LINK_RATE(DPLL_CTRL1_LINK_RATE_2160, 0);
		break;
	}

	memset(&crtc_state->dpll_hw_state, 0,
	       sizeof(crtc_state->dpll_hw_state));

	crtc_state->dpll_hw_state.ctrl1 = ctrl1;

	return true;
}

static int skl_ddi_lcpll_get_freq(struct drm_i915_private *i915,
				  const struct intel_shared_dpll *pll,
				  const struct intel_dpll_hw_state *pll_state)
{
	int link_clock = 0;

	switch ((pll_state->ctrl1 & DPLL_CTRL1_LINK_RATE_MASK(0)) >>
		DPLL_CTRL1_LINK_RATE_SHIFT(0)) {
	case DPLL_CTRL1_LINK_RATE_810:
		link_clock = 81000;
		break;
	case DPLL_CTRL1_LINK_RATE_1080:
		link_clock = 108000;
		break;
	case DPLL_CTRL1_LINK_RATE_1350:
		link_clock = 135000;
		break;
	case DPLL_CTRL1_LINK_RATE_1620:
		link_clock = 162000;
		break;
	case DPLL_CTRL1_LINK_RATE_2160:
		link_clock = 216000;
		break;
	case DPLL_CTRL1_LINK_RATE_2700:
		link_clock = 270000;
		break;
	default:
		drm_WARN(&i915->drm, 1, "Unsupported link rate\n");
		break;
	}

	return link_clock * 2;
}

static bool skl_get_dpll(struct intel_atomic_state *state,
			 struct intel_crtc *crtc,
			 struct intel_encoder *encoder)
{
	struct intel_crtc_state *crtc_state =
		intel_atomic_get_new_crtc_state(state, crtc);
	struct drm_i915_private *i915 = to_i915(crtc->base.dev);
	struct intel_shared_dpll *pll;
	bool bret;

	if (intel_crtc_has_type(crtc_state, INTEL_OUTPUT_HDMI)) {
		bret = skl_ddi_hdmi_pll_dividers(crtc_state);
		if (!bret) {
			drm_dbg_kms(&i915->drm,
				    "Could not get HDMI pll dividers.\n");
			return false;
		}
	} else if (intel_crtc_has_dp_encoder(crtc_state)) {
		bret = skl_ddi_dp_set_dpll_hw_state(crtc_state);
		if (!bret) {
			drm_dbg_kms(&i915->drm,
				    "Could not set DP dpll HW state.\n");
			return false;
		}
	} else {
		return false;
	}

	if (intel_crtc_has_type(crtc_state, INTEL_OUTPUT_EDP))
		pll = intel_find_shared_dpll(state, crtc,
					     &crtc_state->dpll_hw_state,
					     BIT(DPLL_ID_SKL_DPLL0));
	else
		pll = intel_find_shared_dpll(state, crtc,
					     &crtc_state->dpll_hw_state,
					     BIT(DPLL_ID_SKL_DPLL3) |
					     BIT(DPLL_ID_SKL_DPLL2) |
					     BIT(DPLL_ID_SKL_DPLL1));
	if (!pll)
		return false;

	intel_reference_shared_dpll(state, crtc,
				    pll, &crtc_state->dpll_hw_state);

	crtc_state->shared_dpll = pll;

	return true;
}

static int skl_ddi_pll_get_freq(struct drm_i915_private *i915,
				const struct intel_shared_dpll *pll,
				const struct intel_dpll_hw_state *pll_state)
{
	/*
	 * ctrl1 register is already shifted for each pll, just use 0 to get
	 * the internal shift for each field
	 */
	if (pll_state->ctrl1 & DPLL_CTRL1_HDMI_MODE(0))
		return skl_ddi_wrpll_get_freq(i915, pll, pll_state);
	else
		return skl_ddi_lcpll_get_freq(i915, pll, pll_state);
}

static void skl_update_dpll_ref_clks(struct drm_i915_private *i915)
{
	/* No SSC ref */
	i915->dpll.ref_clks.nssc = i915->cdclk.hw.ref;
}

static void skl_dump_hw_state(struct drm_i915_private *dev_priv,
			      const struct intel_dpll_hw_state *hw_state)
{
	drm_dbg_kms(&dev_priv->drm, "dpll_hw_state: "
		      "ctrl1: 0x%x, cfgcr1: 0x%x, cfgcr2: 0x%x\n",
		      hw_state->ctrl1,
		      hw_state->cfgcr1,
		      hw_state->cfgcr2);
}

static const struct intel_shared_dpll_funcs skl_ddi_pll_funcs = {
	.enable = skl_ddi_pll_enable,
	.disable = skl_ddi_pll_disable,
	.get_hw_state = skl_ddi_pll_get_hw_state,
	.get_freq = skl_ddi_pll_get_freq,
};

static const struct intel_shared_dpll_funcs skl_ddi_dpll0_funcs = {
	.enable = skl_ddi_dpll0_enable,
	.disable = skl_ddi_dpll0_disable,
	.get_hw_state = skl_ddi_dpll0_get_hw_state,
	.get_freq = skl_ddi_pll_get_freq,
};

static const struct dpll_info skl_plls[] = {
	{ "DPLL 0", &skl_ddi_dpll0_funcs, DPLL_ID_SKL_DPLL0, INTEL_DPLL_ALWAYS_ON },
	{ "DPLL 1", &skl_ddi_pll_funcs,   DPLL_ID_SKL_DPLL1, 0 },
	{ "DPLL 2", &skl_ddi_pll_funcs,   DPLL_ID_SKL_DPLL2, 0 },
	{ "DPLL 3", &skl_ddi_pll_funcs,   DPLL_ID_SKL_DPLL3, 0 },
	{ },
};

static const struct intel_dpll_mgr skl_pll_mgr = {
	.dpll_info = skl_plls,
	.get_dplls = skl_get_dpll,
	.put_dplls = intel_put_dpll,
	.update_ref_clks = skl_update_dpll_ref_clks,
	.dump_hw_state = skl_dump_hw_state,
};

static void bxt_ddi_pll_enable(struct drm_i915_private *dev_priv,
				struct intel_shared_dpll *pll)
{
	u32 temp;
	enum port port = (enum port)pll->info->id; /* 1:1 port->PLL mapping */
	enum dpio_phy phy;
	enum dpio_channel ch;

	bxt_port_to_phy_channel(dev_priv, port, &phy, &ch);

	/* Non-SSC reference */
	temp = intel_de_read(dev_priv, BXT_PORT_PLL_ENABLE(port));
	temp |= PORT_PLL_REF_SEL;
	intel_de_write(dev_priv, BXT_PORT_PLL_ENABLE(port), temp);

	if (IS_GEMINILAKE(dev_priv)) {
		temp = intel_de_read(dev_priv, BXT_PORT_PLL_ENABLE(port));
		temp |= PORT_PLL_POWER_ENABLE;
		intel_de_write(dev_priv, BXT_PORT_PLL_ENABLE(port), temp);

		if (wait_for_us((intel_de_read(dev_priv, BXT_PORT_PLL_ENABLE(port)) &
				 PORT_PLL_POWER_STATE), 200))
			drm_err(&dev_priv->drm,
				"Power state not set for PLL:%d\n", port);
	}

	/* Disable 10 bit clock */
	temp = intel_de_read(dev_priv, BXT_PORT_PLL_EBB_4(phy, ch));
	temp &= ~PORT_PLL_10BIT_CLK_ENABLE;
	intel_de_write(dev_priv, BXT_PORT_PLL_EBB_4(phy, ch), temp);

	/* Write P1 & P2 */
	temp = intel_de_read(dev_priv, BXT_PORT_PLL_EBB_0(phy, ch));
	temp &= ~(PORT_PLL_P1_MASK | PORT_PLL_P2_MASK);
	temp |= pll->state.hw_state.ebb0;
	intel_de_write(dev_priv, BXT_PORT_PLL_EBB_0(phy, ch), temp);

	/* Write M2 integer */
	temp = intel_de_read(dev_priv, BXT_PORT_PLL(phy, ch, 0));
	temp &= ~PORT_PLL_M2_MASK;
	temp |= pll->state.hw_state.pll0;
	intel_de_write(dev_priv, BXT_PORT_PLL(phy, ch, 0), temp);

	/* Write N */
	temp = intel_de_read(dev_priv, BXT_PORT_PLL(phy, ch, 1));
	temp &= ~PORT_PLL_N_MASK;
	temp |= pll->state.hw_state.pll1;
	intel_de_write(dev_priv, BXT_PORT_PLL(phy, ch, 1), temp);

	/* Write M2 fraction */
	temp = intel_de_read(dev_priv, BXT_PORT_PLL(phy, ch, 2));
	temp &= ~PORT_PLL_M2_FRAC_MASK;
	temp |= pll->state.hw_state.pll2;
	intel_de_write(dev_priv, BXT_PORT_PLL(phy, ch, 2), temp);

	/* Write M2 fraction enable */
	temp = intel_de_read(dev_priv, BXT_PORT_PLL(phy, ch, 3));
	temp &= ~PORT_PLL_M2_FRAC_ENABLE;
	temp |= pll->state.hw_state.pll3;
	intel_de_write(dev_priv, BXT_PORT_PLL(phy, ch, 3), temp);

	/* Write coeff */
	temp = intel_de_read(dev_priv, BXT_PORT_PLL(phy, ch, 6));
	temp &= ~PORT_PLL_PROP_COEFF_MASK;
	temp &= ~PORT_PLL_INT_COEFF_MASK;
	temp &= ~PORT_PLL_GAIN_CTL_MASK;
	temp |= pll->state.hw_state.pll6;
	intel_de_write(dev_priv, BXT_PORT_PLL(phy, ch, 6), temp);

	/* Write calibration val */
	temp = intel_de_read(dev_priv, BXT_PORT_PLL(phy, ch, 8));
	temp &= ~PORT_PLL_TARGET_CNT_MASK;
	temp |= pll->state.hw_state.pll8;
	intel_de_write(dev_priv, BXT_PORT_PLL(phy, ch, 8), temp);

	temp = intel_de_read(dev_priv, BXT_PORT_PLL(phy, ch, 9));
	temp &= ~PORT_PLL_LOCK_THRESHOLD_MASK;
	temp |= pll->state.hw_state.pll9;
	intel_de_write(dev_priv, BXT_PORT_PLL(phy, ch, 9), temp);

	temp = intel_de_read(dev_priv, BXT_PORT_PLL(phy, ch, 10));
	temp &= ~PORT_PLL_DCO_AMP_OVR_EN_H;
	temp &= ~PORT_PLL_DCO_AMP_MASK;
	temp |= pll->state.hw_state.pll10;
	intel_de_write(dev_priv, BXT_PORT_PLL(phy, ch, 10), temp);

	/* Recalibrate with new settings */
	temp = intel_de_read(dev_priv, BXT_PORT_PLL_EBB_4(phy, ch));
	temp |= PORT_PLL_RECALIBRATE;
	intel_de_write(dev_priv, BXT_PORT_PLL_EBB_4(phy, ch), temp);
	temp &= ~PORT_PLL_10BIT_CLK_ENABLE;
	temp |= pll->state.hw_state.ebb4;
	intel_de_write(dev_priv, BXT_PORT_PLL_EBB_4(phy, ch), temp);

	/* Enable PLL */
	temp = intel_de_read(dev_priv, BXT_PORT_PLL_ENABLE(port));
	temp |= PORT_PLL_ENABLE;
	intel_de_write(dev_priv, BXT_PORT_PLL_ENABLE(port), temp);
	intel_de_posting_read(dev_priv, BXT_PORT_PLL_ENABLE(port));

	if (wait_for_us((intel_de_read(dev_priv, BXT_PORT_PLL_ENABLE(port)) & PORT_PLL_LOCK),
			200))
		drm_err(&dev_priv->drm, "PLL %d not locked\n", port);

	if (IS_GEMINILAKE(dev_priv)) {
		temp = intel_de_read(dev_priv, BXT_PORT_TX_DW5_LN0(phy, ch));
		temp |= DCC_DELAY_RANGE_2;
		intel_de_write(dev_priv, BXT_PORT_TX_DW5_GRP(phy, ch), temp);
	}

	/*
	 * While we write to the group register to program all lanes at once we
	 * can read only lane registers and we pick lanes 0/1 for that.
	 */
	temp = intel_de_read(dev_priv, BXT_PORT_PCS_DW12_LN01(phy, ch));
	temp &= ~LANE_STAGGER_MASK;
	temp &= ~LANESTAGGER_STRAP_OVRD;
	temp |= pll->state.hw_state.pcsdw12;
	intel_de_write(dev_priv, BXT_PORT_PCS_DW12_GRP(phy, ch), temp);
}

static void bxt_ddi_pll_disable(struct drm_i915_private *dev_priv,
					struct intel_shared_dpll *pll)
{
	enum port port = (enum port)pll->info->id; /* 1:1 port->PLL mapping */
	u32 temp;

	temp = intel_de_read(dev_priv, BXT_PORT_PLL_ENABLE(port));
	temp &= ~PORT_PLL_ENABLE;
	intel_de_write(dev_priv, BXT_PORT_PLL_ENABLE(port), temp);
	intel_de_posting_read(dev_priv, BXT_PORT_PLL_ENABLE(port));

	if (IS_GEMINILAKE(dev_priv)) {
		temp = intel_de_read(dev_priv, BXT_PORT_PLL_ENABLE(port));
		temp &= ~PORT_PLL_POWER_ENABLE;
		intel_de_write(dev_priv, BXT_PORT_PLL_ENABLE(port), temp);

		if (wait_for_us(!(intel_de_read(dev_priv, BXT_PORT_PLL_ENABLE(port)) &
				  PORT_PLL_POWER_STATE), 200))
			drm_err(&dev_priv->drm,
				"Power state not reset for PLL:%d\n", port);
	}
}

static bool bxt_ddi_pll_get_hw_state(struct drm_i915_private *dev_priv,
					struct intel_shared_dpll *pll,
					struct intel_dpll_hw_state *hw_state)
{
	enum port port = (enum port)pll->info->id; /* 1:1 port->PLL mapping */
	intel_wakeref_t wakeref;
	enum dpio_phy phy;
	enum dpio_channel ch;
	u32 val;
	bool ret;

	bxt_port_to_phy_channel(dev_priv, port, &phy, &ch);

	wakeref = intel_display_power_get_if_enabled(dev_priv,
						     POWER_DOMAIN_DISPLAY_CORE);
	if (!wakeref)
		return false;

	ret = false;

	val = intel_de_read(dev_priv, BXT_PORT_PLL_ENABLE(port));
	if (!(val & PORT_PLL_ENABLE))
		goto out;

	hw_state->ebb0 = intel_de_read(dev_priv, BXT_PORT_PLL_EBB_0(phy, ch));
	hw_state->ebb0 &= PORT_PLL_P1_MASK | PORT_PLL_P2_MASK;

	hw_state->ebb4 = intel_de_read(dev_priv, BXT_PORT_PLL_EBB_4(phy, ch));
	hw_state->ebb4 &= PORT_PLL_10BIT_CLK_ENABLE;

	hw_state->pll0 = intel_de_read(dev_priv, BXT_PORT_PLL(phy, ch, 0));
	hw_state->pll0 &= PORT_PLL_M2_MASK;

	hw_state->pll1 = intel_de_read(dev_priv, BXT_PORT_PLL(phy, ch, 1));
	hw_state->pll1 &= PORT_PLL_N_MASK;

	hw_state->pll2 = intel_de_read(dev_priv, BXT_PORT_PLL(phy, ch, 2));
	hw_state->pll2 &= PORT_PLL_M2_FRAC_MASK;

	hw_state->pll3 = intel_de_read(dev_priv, BXT_PORT_PLL(phy, ch, 3));
	hw_state->pll3 &= PORT_PLL_M2_FRAC_ENABLE;

	hw_state->pll6 = intel_de_read(dev_priv, BXT_PORT_PLL(phy, ch, 6));
	hw_state->pll6 &= PORT_PLL_PROP_COEFF_MASK |
			  PORT_PLL_INT_COEFF_MASK |
			  PORT_PLL_GAIN_CTL_MASK;

	hw_state->pll8 = intel_de_read(dev_priv, BXT_PORT_PLL(phy, ch, 8));
	hw_state->pll8 &= PORT_PLL_TARGET_CNT_MASK;

	hw_state->pll9 = intel_de_read(dev_priv, BXT_PORT_PLL(phy, ch, 9));
	hw_state->pll9 &= PORT_PLL_LOCK_THRESHOLD_MASK;

	hw_state->pll10 = intel_de_read(dev_priv, BXT_PORT_PLL(phy, ch, 10));
	hw_state->pll10 &= PORT_PLL_DCO_AMP_OVR_EN_H |
			   PORT_PLL_DCO_AMP_MASK;

	/*
	 * While we write to the group register to program all lanes at once we
	 * can read only lane registers. We configure all lanes the same way, so
	 * here just read out lanes 0/1 and output a note if lanes 2/3 differ.
	 */
	hw_state->pcsdw12 = intel_de_read(dev_priv,
					  BXT_PORT_PCS_DW12_LN01(phy, ch));
	if (intel_de_read(dev_priv, BXT_PORT_PCS_DW12_LN23(phy, ch)) != hw_state->pcsdw12)
		drm_dbg(&dev_priv->drm,
			"lane stagger config different for lane 01 (%08x) and 23 (%08x)\n",
			hw_state->pcsdw12,
			intel_de_read(dev_priv,
				      BXT_PORT_PCS_DW12_LN23(phy, ch)));
	hw_state->pcsdw12 &= LANE_STAGGER_MASK | LANESTAGGER_STRAP_OVRD;

	ret = true;

out:
	intel_display_power_put(dev_priv, POWER_DOMAIN_DISPLAY_CORE, wakeref);

	return ret;
}

/* bxt clock parameters */
struct bxt_clk_div {
	int clock;
	u32 p1;
	u32 p2;
	u32 m2_int;
	u32 m2_frac;
	bool m2_frac_en;
	u32 n;

	int vco;
};

/* pre-calculated values for DP linkrates */
static const struct bxt_clk_div bxt_dp_clk_val[] = {
	{162000, 4, 2, 32, 1677722, 1, 1},
	{270000, 4, 1, 27,       0, 0, 1},
	{540000, 2, 1, 27,       0, 0, 1},
	{216000, 3, 2, 32, 1677722, 1, 1},
	{243000, 4, 1, 24, 1258291, 1, 1},
	{324000, 4, 1, 32, 1677722, 1, 1},
	{432000, 3, 1, 32, 1677722, 1, 1}
};

static bool
bxt_ddi_hdmi_pll_dividers(struct intel_crtc_state *crtc_state,
			  struct bxt_clk_div *clk_div)
{
	struct drm_i915_private *i915 = to_i915(crtc_state->uapi.crtc->dev);
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct dpll best_clock;

	/* Calculate HDMI div */
	/*
	 * FIXME: tie the following calculation into
	 * i9xx_crtc_compute_clock
	 */
	if (!bxt_find_best_dpll(crtc_state, &best_clock)) {
		drm_dbg(&i915->drm, "no PLL dividers found for clock %d pipe %c\n",
			crtc_state->port_clock,
			pipe_name(crtc->pipe));
		return false;
	}

	clk_div->p1 = best_clock.p1;
	clk_div->p2 = best_clock.p2;
	drm_WARN_ON(&i915->drm, best_clock.m1 != 2);
	clk_div->n = best_clock.n;
	clk_div->m2_int = best_clock.m2 >> 22;
	clk_div->m2_frac = best_clock.m2 & ((1 << 22) - 1);
	clk_div->m2_frac_en = clk_div->m2_frac != 0;

	clk_div->vco = best_clock.vco;

	return true;
}

static void bxt_ddi_dp_pll_dividers(struct intel_crtc_state *crtc_state,
				    struct bxt_clk_div *clk_div)
{
	int clock = crtc_state->port_clock;
	int i;

	*clk_div = bxt_dp_clk_val[0];
	for (i = 0; i < ARRAY_SIZE(bxt_dp_clk_val); ++i) {
		if (bxt_dp_clk_val[i].clock == clock) {
			*clk_div = bxt_dp_clk_val[i];
			break;
		}
	}

	clk_div->vco = clock * 10 / 2 * clk_div->p1 * clk_div->p2;
}

static bool bxt_ddi_set_dpll_hw_state(struct intel_crtc_state *crtc_state,
				      const struct bxt_clk_div *clk_div)
{
	struct drm_i915_private *i915 = to_i915(crtc_state->uapi.crtc->dev);
	struct intel_dpll_hw_state *dpll_hw_state = &crtc_state->dpll_hw_state;
	int clock = crtc_state->port_clock;
	int vco = clk_div->vco;
	u32 prop_coef, int_coef, gain_ctl, targ_cnt;
	u32 lanestagger;

	memset(dpll_hw_state, 0, sizeof(*dpll_hw_state));

	if (vco >= 6200000 && vco <= 6700000) {
		prop_coef = 4;
		int_coef = 9;
		gain_ctl = 3;
		targ_cnt = 8;
	} else if ((vco > 5400000 && vco < 6200000) ||
			(vco >= 4800000 && vco < 5400000)) {
		prop_coef = 5;
		int_coef = 11;
		gain_ctl = 3;
		targ_cnt = 9;
	} else if (vco == 5400000) {
		prop_coef = 3;
		int_coef = 8;
		gain_ctl = 1;
		targ_cnt = 9;
	} else {
		drm_err(&i915->drm, "Invalid VCO\n");
		return false;
	}

	if (clock > 270000)
		lanestagger = 0x18;
	else if (clock > 135000)
		lanestagger = 0x0d;
	else if (clock > 67000)
		lanestagger = 0x07;
	else if (clock > 33000)
		lanestagger = 0x04;
	else
		lanestagger = 0x02;

	dpll_hw_state->ebb0 = PORT_PLL_P1(clk_div->p1) | PORT_PLL_P2(clk_div->p2);
	dpll_hw_state->pll0 = clk_div->m2_int;
	dpll_hw_state->pll1 = PORT_PLL_N(clk_div->n);
	dpll_hw_state->pll2 = clk_div->m2_frac;

	if (clk_div->m2_frac_en)
		dpll_hw_state->pll3 = PORT_PLL_M2_FRAC_ENABLE;

	dpll_hw_state->pll6 = prop_coef | PORT_PLL_INT_COEFF(int_coef);
	dpll_hw_state->pll6 |= PORT_PLL_GAIN_CTL(gain_ctl);

	dpll_hw_state->pll8 = targ_cnt;

	dpll_hw_state->pll9 = 5 << PORT_PLL_LOCK_THRESHOLD_SHIFT;

	dpll_hw_state->pll10 =
		PORT_PLL_DCO_AMP(PORT_PLL_DCO_AMP_DEFAULT)
		| PORT_PLL_DCO_AMP_OVR_EN_H;

	dpll_hw_state->ebb4 = PORT_PLL_10BIT_CLK_ENABLE;

	dpll_hw_state->pcsdw12 = LANESTAGGER_STRAP_OVRD | lanestagger;

	return true;
}

static bool
bxt_ddi_dp_set_dpll_hw_state(struct intel_crtc_state *crtc_state)
{
	struct bxt_clk_div clk_div = {};

	bxt_ddi_dp_pll_dividers(crtc_state, &clk_div);

	return bxt_ddi_set_dpll_hw_state(crtc_state, &clk_div);
}

static bool
bxt_ddi_hdmi_set_dpll_hw_state(struct intel_crtc_state *crtc_state)
{
	struct bxt_clk_div clk_div = {};

	bxt_ddi_hdmi_pll_dividers(crtc_state, &clk_div);

	return bxt_ddi_set_dpll_hw_state(crtc_state, &clk_div);
}

static int bxt_ddi_pll_get_freq(struct drm_i915_private *i915,
				const struct intel_shared_dpll *pll,
				const struct intel_dpll_hw_state *pll_state)
{
	struct dpll clock;

	clock.m1 = 2;
	clock.m2 = (pll_state->pll0 & PORT_PLL_M2_MASK) << 22;
	if (pll_state->pll3 & PORT_PLL_M2_FRAC_ENABLE)
		clock.m2 |= pll_state->pll2 & PORT_PLL_M2_FRAC_MASK;
	clock.n = (pll_state->pll1 & PORT_PLL_N_MASK) >> PORT_PLL_N_SHIFT;
	clock.p1 = (pll_state->ebb0 & PORT_PLL_P1_MASK) >> PORT_PLL_P1_SHIFT;
	clock.p2 = (pll_state->ebb0 & PORT_PLL_P2_MASK) >> PORT_PLL_P2_SHIFT;

	return chv_calc_dpll_params(i915->dpll.ref_clks.nssc, &clock);
}

static bool bxt_get_dpll(struct intel_atomic_state *state,
			 struct intel_crtc *crtc,
			 struct intel_encoder *encoder)
{
	struct intel_crtc_state *crtc_state =
		intel_atomic_get_new_crtc_state(state, crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	struct intel_shared_dpll *pll;
	enum intel_dpll_id id;

	if (intel_crtc_has_type(crtc_state, INTEL_OUTPUT_HDMI) &&
	    !bxt_ddi_hdmi_set_dpll_hw_state(crtc_state))
		return false;

	if (intel_crtc_has_dp_encoder(crtc_state) &&
	    !bxt_ddi_dp_set_dpll_hw_state(crtc_state))
		return false;

	/* 1:1 mapping between ports and PLLs */
	id = (enum intel_dpll_id) encoder->port;
	pll = intel_get_shared_dpll_by_id(dev_priv, id);

	drm_dbg_kms(&dev_priv->drm, "[CRTC:%d:%s] using pre-allocated %s\n",
		    crtc->base.base.id, crtc->base.name, pll->info->name);

	intel_reference_shared_dpll(state, crtc,
				    pll, &crtc_state->dpll_hw_state);

	crtc_state->shared_dpll = pll;

	return true;
}

static void bxt_update_dpll_ref_clks(struct drm_i915_private *i915)
{
	i915->dpll.ref_clks.ssc = 100000;
	i915->dpll.ref_clks.nssc = 100000;
	/* DSI non-SSC ref 19.2MHz */
}

static void bxt_dump_hw_state(struct drm_i915_private *dev_priv,
			      const struct intel_dpll_hw_state *hw_state)
{
	drm_dbg_kms(&dev_priv->drm, "dpll_hw_state: ebb0: 0x%x, ebb4: 0x%x,"
		    "pll0: 0x%x, pll1: 0x%x, pll2: 0x%x, pll3: 0x%x, "
		    "pll6: 0x%x, pll8: 0x%x, pll9: 0x%x, pll10: 0x%x, pcsdw12: 0x%x\n",
		    hw_state->ebb0,
		    hw_state->ebb4,
		    hw_state->pll0,
		    hw_state->pll1,
		    hw_state->pll2,
		    hw_state->pll3,
		    hw_state->pll6,
		    hw_state->pll8,
		    hw_state->pll9,
		    hw_state->pll10,
		    hw_state->pcsdw12);
}

static const struct intel_shared_dpll_funcs bxt_ddi_pll_funcs = {
	.enable = bxt_ddi_pll_enable,
	.disable = bxt_ddi_pll_disable,
	.get_hw_state = bxt_ddi_pll_get_hw_state,
	.get_freq = bxt_ddi_pll_get_freq,
};

static const struct dpll_info bxt_plls[] = {
	{ "PORT PLL A", &bxt_ddi_pll_funcs, DPLL_ID_SKL_DPLL0, 0 },
	{ "PORT PLL B", &bxt_ddi_pll_funcs, DPLL_ID_SKL_DPLL1, 0 },
	{ "PORT PLL C", &bxt_ddi_pll_funcs, DPLL_ID_SKL_DPLL2, 0 },
	{ },
};

static const struct intel_dpll_mgr bxt_pll_mgr = {
	.dpll_info = bxt_plls,
	.get_dplls = bxt_get_dpll,
	.put_dplls = intel_put_dpll,
	.update_ref_clks = bxt_update_dpll_ref_clks,
	.dump_hw_state = bxt_dump_hw_state,
};

static void cnl_ddi_pll_enable(struct drm_i915_private *dev_priv,
			       struct intel_shared_dpll *pll)
{
	const enum intel_dpll_id id = pll->info->id;
	u32 val;

	/* 1. Enable DPLL power in DPLL_ENABLE. */
	val = intel_de_read(dev_priv, CNL_DPLL_ENABLE(id));
	val |= PLL_POWER_ENABLE;
	intel_de_write(dev_priv, CNL_DPLL_ENABLE(id), val);

	/* 2. Wait for DPLL power state enabled in DPLL_ENABLE. */
	if (intel_de_wait_for_set(dev_priv, CNL_DPLL_ENABLE(id),
				  PLL_POWER_STATE, 5))
		drm_err(&dev_priv->drm, "PLL %d Power not enabled\n", id);

	/*
	 * 3. Configure DPLL_CFGCR0 to set SSC enable/disable,
	 * select DP mode, and set DP link rate.
	 */
	val = pll->state.hw_state.cfgcr0;
	intel_de_write(dev_priv, CNL_DPLL_CFGCR0(id), val);

	/* 4. Reab back to ensure writes completed */
	intel_de_posting_read(dev_priv, CNL_DPLL_CFGCR0(id));

	/* 3. Configure DPLL_CFGCR0 */
	/* Avoid touch CFGCR1 if HDMI mode is not enabled */
	if (pll->state.hw_state.cfgcr0 & DPLL_CFGCR0_HDMI_MODE) {
		val = pll->state.hw_state.cfgcr1;
		intel_de_write(dev_priv, CNL_DPLL_CFGCR1(id), val);
		/* 4. Reab back to ensure writes completed */
		intel_de_posting_read(dev_priv, CNL_DPLL_CFGCR1(id));
	}

	/*
	 * 5. If the frequency will result in a change to the voltage
	 * requirement, follow the Display Voltage Frequency Switching
	 * Sequence Before Frequency Change
	 *
	 * Note: DVFS is actually handled via the cdclk code paths,
	 * hence we do nothing here.
	 */

	/* 6. Enable DPLL in DPLL_ENABLE. */
	val = intel_de_read(dev_priv, CNL_DPLL_ENABLE(id));
	val |= PLL_ENABLE;
	intel_de_write(dev_priv, CNL_DPLL_ENABLE(id), val);

	/* 7. Wait for PLL lock status in DPLL_ENABLE. */
	if (intel_de_wait_for_set(dev_priv, CNL_DPLL_ENABLE(id), PLL_LOCK, 5))
		drm_err(&dev_priv->drm, "PLL %d not locked\n", id);

	/*
	 * 8. If the frequency will result in a change to the voltage
	 * requirement, follow the Display Voltage Frequency Switching
	 * Sequence After Frequency Change
	 *
	 * Note: DVFS is actually handled via the cdclk code paths,
	 * hence we do nothing here.
	 */

	/*
	 * 9. turn on the clock for the DDI and map the DPLL to the DDI
	 * Done at intel_ddi_clk_select
	 */
}

static void cnl_ddi_pll_disable(struct drm_i915_private *dev_priv,
				struct intel_shared_dpll *pll)
{
	const enum intel_dpll_id id = pll->info->id;
	u32 val;

	/*
	 * 1. Configure DPCLKA_CFGCR0 to turn off the clock for the DDI.
	 * Done at intel_ddi_post_disable
	 */

	/*
	 * 2. If the frequency will result in a change to the voltage
	 * requirement, follow the Display Voltage Frequency Switching
	 * Sequence Before Frequency Change
	 *
	 * Note: DVFS is actually handled via the cdclk code paths,
	 * hence we do nothing here.
	 */

	/* 3. Disable DPLL through DPLL_ENABLE. */
	val = intel_de_read(dev_priv, CNL_DPLL_ENABLE(id));
	val &= ~PLL_ENABLE;
	intel_de_write(dev_priv, CNL_DPLL_ENABLE(id), val);

	/* 4. Wait for PLL not locked status in DPLL_ENABLE. */
	if (intel_de_wait_for_clear(dev_priv, CNL_DPLL_ENABLE(id), PLL_LOCK, 5))
		drm_err(&dev_priv->drm, "PLL %d locked\n", id);

	/*
	 * 5. If the frequency will result in a change to the voltage
	 * requirement, follow the Display Voltage Frequency Switching
	 * Sequence After Frequency Change
	 *
	 * Note: DVFS is actually handled via the cdclk code paths,
	 * hence we do nothing here.
	 */

	/* 6. Disable DPLL power in DPLL_ENABLE. */
	val = intel_de_read(dev_priv, CNL_DPLL_ENABLE(id));
	val &= ~PLL_POWER_ENABLE;
	intel_de_write(dev_priv, CNL_DPLL_ENABLE(id), val);

	/* 7. Wait for DPLL power state disabled in DPLL_ENABLE. */
	if (intel_de_wait_for_clear(dev_priv, CNL_DPLL_ENABLE(id),
				    PLL_POWER_STATE, 5))
		drm_err(&dev_priv->drm, "PLL %d Power not disabled\n", id);
}

static bool cnl_ddi_pll_get_hw_state(struct drm_i915_private *dev_priv,
				     struct intel_shared_dpll *pll,
				     struct intel_dpll_hw_state *hw_state)
{
	const enum intel_dpll_id id = pll->info->id;
	intel_wakeref_t wakeref;
	u32 val;
	bool ret;

	wakeref = intel_display_power_get_if_enabled(dev_priv,
						     POWER_DOMAIN_DISPLAY_CORE);
	if (!wakeref)
		return false;

	ret = false;

	val = intel_de_read(dev_priv, CNL_DPLL_ENABLE(id));
	if (!(val & PLL_ENABLE))
		goto out;

	val = intel_de_read(dev_priv, CNL_DPLL_CFGCR0(id));
	hw_state->cfgcr0 = val;

	/* avoid reading back stale values if HDMI mode is not enabled */
	if (val & DPLL_CFGCR0_HDMI_MODE) {
		hw_state->cfgcr1 = intel_de_read(dev_priv,
						 CNL_DPLL_CFGCR1(id));
	}
	ret = true;

out:
	intel_display_power_put(dev_priv, POWER_DOMAIN_DISPLAY_CORE, wakeref);

	return ret;
}

static void cnl_wrpll_get_multipliers(int bestdiv, int *pdiv,
				      int *qdiv, int *kdiv)
{
	/* even dividers */
	if (bestdiv % 2 == 0) {
		if (bestdiv == 2) {
			*pdiv = 2;
			*qdiv = 1;
			*kdiv = 1;
		} else if (bestdiv % 4 == 0) {
			*pdiv = 2;
			*qdiv = bestdiv / 4;
			*kdiv = 2;
		} else if (bestdiv % 6 == 0) {
			*pdiv = 3;
			*qdiv = bestdiv / 6;
			*kdiv = 2;
		} else if (bestdiv % 5 == 0) {
			*pdiv = 5;
			*qdiv = bestdiv / 10;
			*kdiv = 2;
		} else if (bestdiv % 14 == 0) {
			*pdiv = 7;
			*qdiv = bestdiv / 14;
			*kdiv = 2;
		}
	} else {
		if (bestdiv == 3 || bestdiv == 5 || bestdiv == 7) {
			*pdiv = bestdiv;
			*qdiv = 1;
			*kdiv = 1;
		} else { /* 9, 15, 21 */
			*pdiv = bestdiv / 3;
			*qdiv = 1;
			*kdiv = 3;
		}
	}
}

static void cnl_wrpll_params_populate(struct skl_wrpll_params *params,
				      u32 dco_freq, u32 ref_freq,
				      int pdiv, int qdiv, int kdiv)
{
	u32 dco;

	switch (kdiv) {
	case 1:
		params->kdiv = 1;
		break;
	case 2:
		params->kdiv = 2;
		break;
	case 3:
		params->kdiv = 4;
		break;
	default:
		WARN(1, "Incorrect KDiv\n");
	}

	switch (pdiv) {
	case 2:
		params->pdiv = 1;
		break;
	case 3:
		params->pdiv = 2;
		break;
	case 5:
		params->pdiv = 4;
		break;
	case 7:
		params->pdiv = 8;
		break;
	default:
		WARN(1, "Incorrect PDiv\n");
	}

	WARN_ON(kdiv != 2 && qdiv != 1);

	params->qdiv_ratio = qdiv;
	params->qdiv_mode = (qdiv == 1) ? 0 : 1;

	dco = div_u64((u64)dco_freq << 15, ref_freq);

	params->dco_integer = dco >> 15;
	params->dco_fraction = dco & 0x7fff;
}

static bool
__cnl_ddi_calculate_wrpll(struct intel_crtc_state *crtc_state,
			  struct skl_wrpll_params *wrpll_params,
			  int ref_clock)
{
	u32 afe_clock = crtc_state->port_clock * 5;
	u32 dco_min = 7998000;
	u32 dco_max = 10000000;
	u32 dco_mid = (dco_min + dco_max) / 2;
	static const int dividers[] = {  2,  4,  6,  8, 10, 12,  14,  16,
					 18, 20, 24, 28, 30, 32,  36,  40,
					 42, 44, 48, 50, 52, 54,  56,  60,
					 64, 66, 68, 70, 72, 76,  78,  80,
					 84, 88, 90, 92, 96, 98, 100, 102,
					  3,  5,  7,  9, 15, 21 };
	u32 dco, best_dco = 0, dco_centrality = 0;
	u32 best_dco_centrality = U32_MAX; /* Spec meaning of 999999 MHz */
	int d, best_div = 0, pdiv = 0, qdiv = 0, kdiv = 0;

	for (d = 0; d < ARRAY_SIZE(dividers); d++) {
		dco = afe_clock * dividers[d];

		if ((dco <= dco_max) && (dco >= dco_min)) {
			dco_centrality = abs(dco - dco_mid);

			if (dco_centrality < best_dco_centrality) {
				best_dco_centrality = dco_centrality;
				best_div = dividers[d];
				best_dco = dco;
			}
		}
	}

	if (best_div == 0)
		return false;

	cnl_wrpll_get_multipliers(best_div, &pdiv, &qdiv, &kdiv);
	cnl_wrpll_params_populate(wrpll_params, best_dco, ref_clock,
				  pdiv, qdiv, kdiv);

	return true;
}

static bool
cnl_ddi_calculate_wrpll(struct intel_crtc_state *crtc_state,
			struct skl_wrpll_params *wrpll_params)
{
	struct drm_i915_private *i915 = to_i915(crtc_state->uapi.crtc->dev);

	return __cnl_ddi_calculate_wrpll(crtc_state, wrpll_params,
					 i915->dpll.ref_clks.nssc);
}

static bool cnl_ddi_hdmi_pll_dividers(struct intel_crtc_state *crtc_state)
{
	u32 cfgcr0, cfgcr1;
	struct skl_wrpll_params wrpll_params = { 0, };

	cfgcr0 = DPLL_CFGCR0_HDMI_MODE;

	if (!cnl_ddi_calculate_wrpll(crtc_state, &wrpll_params))
		return false;

	cfgcr0 |= DPLL_CFGCR0_DCO_FRACTION(wrpll_params.dco_fraction) |
		wrpll_params.dco_integer;

	cfgcr1 = DPLL_CFGCR1_QDIV_RATIO(wrpll_params.qdiv_ratio) |
		DPLL_CFGCR1_QDIV_MODE(wrpll_params.qdiv_mode) |
		DPLL_CFGCR1_KDIV(wrpll_params.kdiv) |
		DPLL_CFGCR1_PDIV(wrpll_params.pdiv) |
		DPLL_CFGCR1_CENTRAL_FREQ;

	memset(&crtc_state->dpll_hw_state, 0,
	       sizeof(crtc_state->dpll_hw_state));

	crtc_state->dpll_hw_state.cfgcr0 = cfgcr0;
	crtc_state->dpll_hw_state.cfgcr1 = cfgcr1;
	return true;
}

/*
 * Display WA #22010492432: ehl, tgl
 * Program half of the nominal DCO divider fraction value.
 */
static bool
ehl_combo_pll_div_frac_wa_needed(struct drm_i915_private *i915)
{
	return ((IS_PLATFORM(i915, INTEL_ELKHARTLAKE) &&
		 IS_JSL_EHL_REVID(i915, EHL_REVID_B0, REVID_FOREVER)) ||
		 IS_TIGERLAKE(i915)) &&
		 i915->dpll.ref_clks.nssc == 38400;
}

static int __cnl_ddi_wrpll_get_freq(struct drm_i915_private *dev_priv,
				    const struct intel_shared_dpll *pll,
				    const struct intel_dpll_hw_state *pll_state,
				    int ref_clock)
{
	u32 dco_fraction;
	u32 p0, p1, p2, dco_freq;

	p0 = pll_state->cfgcr1 & DPLL_CFGCR1_PDIV_MASK;
	p2 = pll_state->cfgcr1 & DPLL_CFGCR1_KDIV_MASK;

	if (pll_state->cfgcr1 & DPLL_CFGCR1_QDIV_MODE(1))
		p1 = (pll_state->cfgcr1 & DPLL_CFGCR1_QDIV_RATIO_MASK) >>
			DPLL_CFGCR1_QDIV_RATIO_SHIFT;
	else
		p1 = 1;


	switch (p0) {
	case DPLL_CFGCR1_PDIV_2:
		p0 = 2;
		break;
	case DPLL_CFGCR1_PDIV_3:
		p0 = 3;
		break;
	case DPLL_CFGCR1_PDIV_5:
		p0 = 5;
		break;
	case DPLL_CFGCR1_PDIV_7:
		p0 = 7;
		break;
	}

	switch (p2) {
	case DPLL_CFGCR1_KDIV_1:
		p2 = 1;
		break;
	case DPLL_CFGCR1_KDIV_2:
		p2 = 2;
		break;
	case DPLL_CFGCR1_KDIV_3:
		p2 = 3;
		break;
	}

	dco_freq = (pll_state->cfgcr0 & DPLL_CFGCR0_DCO_INTEGER_MASK) *
		   ref_clock;

	dco_fraction = (pll_state->cfgcr0 & DPLL_CFGCR0_DCO_FRACTION_MASK) >>
		       DPLL_CFGCR0_DCO_FRACTION_SHIFT;

	if (ehl_combo_pll_div_frac_wa_needed(dev_priv))
		dco_fraction *= 2;

	dco_freq += (dco_fraction * ref_clock) / 0x8000;

	if (drm_WARN_ON(&dev_priv->drm, p0 == 0 || p1 == 0 || p2 == 0))
		return 0;

	return dco_freq / (p0 * p1 * p2 * 5);
}

static int cnl_ddi_wrpll_get_freq(struct drm_i915_private *i915,
				  const struct intel_shared_dpll *pll,
				  const struct intel_dpll_hw_state *pll_state)
{
	return __cnl_ddi_wrpll_get_freq(i915, pll, pll_state,
					i915->dpll.ref_clks.nssc);
}

static bool
cnl_ddi_dp_set_dpll_hw_state(struct intel_crtc_state *crtc_state)
{
	u32 cfgcr0;

	cfgcr0 = DPLL_CFGCR0_SSC_ENABLE;

	switch (crtc_state->port_clock / 2) {
	case 81000:
		cfgcr0 |= DPLL_CFGCR0_LINK_RATE_810;
		break;
	case 135000:
		cfgcr0 |= DPLL_CFGCR0_LINK_RATE_1350;
		break;
	case 270000:
		cfgcr0 |= DPLL_CFGCR0_LINK_RATE_2700;
		break;
		/* eDP 1.4 rates */
	case 162000:
		cfgcr0 |= DPLL_CFGCR0_LINK_RATE_1620;
		break;
	case 108000:
		cfgcr0 |= DPLL_CFGCR0_LINK_RATE_1080;
		break;
	case 216000:
		cfgcr0 |= DPLL_CFGCR0_LINK_RATE_2160;
		break;
	case 324000:
		/* Some SKUs may require elevated I/O voltage to support this */
		cfgcr0 |= DPLL_CFGCR0_LINK_RATE_3240;
		break;
	case 405000:
		/* Some SKUs may require elevated I/O voltage to support this */
		cfgcr0 |= DPLL_CFGCR0_LINK_RATE_4050;
		break;
	}

	memset(&crtc_state->dpll_hw_state, 0,
	       sizeof(crtc_state->dpll_hw_state));

	crtc_state->dpll_hw_state.cfgcr0 = cfgcr0;

	return true;
}

static int cnl_ddi_lcpll_get_freq(struct drm_i915_private *i915,
				  const struct intel_shared_dpll *pll,
				  const struct intel_dpll_hw_state *pll_state)
{
	int link_clock = 0;

	switch (pll_state->cfgcr0 & DPLL_CFGCR0_LINK_RATE_MASK) {
	case DPLL_CFGCR0_LINK_RATE_810:
		link_clock = 81000;
		break;
	case DPLL_CFGCR0_LINK_RATE_1080:
		link_clock = 108000;
		break;
	case DPLL_CFGCR0_LINK_RATE_1350:
		link_clock = 135000;
		break;
	case DPLL_CFGCR0_LINK_RATE_1620:
		link_clock = 162000;
		break;
	case DPLL_CFGCR0_LINK_RATE_2160:
		link_clock = 216000;
		break;
	case DPLL_CFGCR0_LINK_RATE_2700:
		link_clock = 270000;
		break;
	case DPLL_CFGCR0_LINK_RATE_3240:
		link_clock = 324000;
		break;
	case DPLL_CFGCR0_LINK_RATE_4050:
		link_clock = 405000;
		break;
	default:
		drm_WARN(&i915->drm, 1, "Unsupported link rate\n");
		break;
	}

	return link_clock * 2;
}

static bool cnl_get_dpll(struct intel_atomic_state *state,
			 struct intel_crtc *crtc,
			 struct intel_encoder *encoder)
{
	struct intel_crtc_state *crtc_state =
		intel_atomic_get_new_crtc_state(state, crtc);
	struct drm_i915_private *i915 = to_i915(crtc_state->uapi.crtc->dev);
	struct intel_shared_dpll *pll;
	bool bret;

	if (intel_crtc_has_type(crtc_state, INTEL_OUTPUT_HDMI)) {
		bret = cnl_ddi_hdmi_pll_dividers(crtc_state);
		if (!bret) {
			drm_dbg_kms(&i915->drm,
				    "Could not get HDMI pll dividers.\n");
			return false;
		}
	} else if (intel_crtc_has_dp_encoder(crtc_state)) {
		bret = cnl_ddi_dp_set_dpll_hw_state(crtc_state);
		if (!bret) {
			drm_dbg_kms(&i915->drm,
				    "Could not set DP dpll HW state.\n");
			return false;
		}
	} else {
		drm_dbg_kms(&i915->drm,
			    "Skip DPLL setup for output_types 0x%x\n",
			    crtc_state->output_types);
		return false;
	}

	pll = intel_find_shared_dpll(state, crtc,
				     &crtc_state->dpll_hw_state,
				     BIT(DPLL_ID_SKL_DPLL2) |
				     BIT(DPLL_ID_SKL_DPLL1) |
				     BIT(DPLL_ID_SKL_DPLL0));
	if (!pll) {
		drm_dbg_kms(&i915->drm, "No PLL selected\n");
		return false;
	}

	intel_reference_shared_dpll(state, crtc,
				    pll, &crtc_state->dpll_hw_state);

	crtc_state->shared_dpll = pll;

	return true;
}

static int cnl_ddi_pll_get_freq(struct drm_i915_private *i915,
				const struct intel_shared_dpll *pll,
				const struct intel_dpll_hw_state *pll_state)
{
	if (pll_state->cfgcr0 & DPLL_CFGCR0_HDMI_MODE)
		return cnl_ddi_wrpll_get_freq(i915, pll, pll_state);
	else
		return cnl_ddi_lcpll_get_freq(i915, pll, pll_state);
}

static void cnl_update_dpll_ref_clks(struct drm_i915_private *i915)
{
	/* No SSC reference */
	i915->dpll.ref_clks.nssc = i915->cdclk.hw.ref;
}

static void cnl_dump_hw_state(struct drm_i915_private *dev_priv,
			      const struct intel_dpll_hw_state *hw_state)
{
	drm_dbg_kms(&dev_priv->drm, "dpll_hw_state: "
		    "cfgcr0: 0x%x, cfgcr1: 0x%x\n",
		    hw_state->cfgcr0,
		    hw_state->cfgcr1);
}

static const struct intel_shared_dpll_funcs cnl_ddi_pll_funcs = {
	.enable = cnl_ddi_pll_enable,
	.disable = cnl_ddi_pll_disable,
	.get_hw_state = cnl_ddi_pll_get_hw_state,
	.get_freq = cnl_ddi_pll_get_freq,
};

static const struct dpll_info cnl_plls[] = {
	{ "DPLL 0", &cnl_ddi_pll_funcs, DPLL_ID_SKL_DPLL0, 0 },
	{ "DPLL 1", &cnl_ddi_pll_funcs, DPLL_ID_SKL_DPLL1, 0 },
	{ "DPLL 2", &cnl_ddi_pll_funcs, DPLL_ID_SKL_DPLL2, 0 },
	{ },
};

static const struct intel_dpll_mgr cnl_pll_mgr = {
	.dpll_info = cnl_plls,
	.get_dplls = cnl_get_dpll,
	.put_dplls = intel_put_dpll,
	.update_ref_clks = cnl_update_dpll_ref_clks,
	.dump_hw_state = cnl_dump_hw_state,
};

struct icl_combo_pll_params {
	int clock;
	struct skl_wrpll_params wrpll;
};

/*
 * These values alrea already adjusted: they're the bits we write to the
 * registers, not the logical values.
 */
static const struct icl_combo_pll_params icl_dp_combo_pll_24MHz_values[] = {
	{ 540000,
	  { .dco_integer = 0x151, .dco_fraction = 0x4000,		/* [0]: 5.4 */
	    .pdiv = 0x2 /* 3 */, .kdiv = 1, .qdiv_mode = 0, .qdiv_ratio = 0, }, },
	{ 270000,
	  { .dco_integer = 0x151, .dco_fraction = 0x4000,		/* [1]: 2.7 */
	    .pdiv = 0x2 /* 3 */, .kdiv = 2, .qdiv_mode = 0, .qdiv_ratio = 0, }, },
	{ 162000,
	  { .dco_integer = 0x151, .dco_fraction = 0x4000,		/* [2]: 1.62 */
	    .pdiv = 0x4 /* 5 */, .kdiv = 2, .qdiv_mode = 0, .qdiv_ratio = 0, }, },
	{ 324000,
	  { .dco_integer = 0x151, .dco_fraction = 0x4000,		/* [3]: 3.24 */
	    .pdiv = 0x4 /* 5 */, .kdiv = 1, .qdiv_mode = 0, .qdiv_ratio = 0, }, },
	{ 216000,
	  { .dco_integer = 0x168, .dco_fraction = 0x0000,		/* [4]: 2.16 */
	    .pdiv = 0x1 /* 2 */, .kdiv = 2, .qdiv_mode = 1, .qdiv_ratio = 2, }, },
	{ 432000,
	  { .dco_integer = 0x168, .dco_fraction = 0x0000,		/* [5]: 4.32 */
	    .pdiv = 0x1 /* 2 */, .kdiv = 2, .qdiv_mode = 0, .qdiv_ratio = 0, }, },
	{ 648000,
	  { .dco_integer = 0x195, .dco_fraction = 0x0000,		/* [6]: 6.48 */
	    .pdiv = 0x2 /* 3 */, .kdiv = 1, .qdiv_mode = 0, .qdiv_ratio = 0, }, },
	{ 810000,
	  { .dco_integer = 0x151, .dco_fraction = 0x4000,		/* [7]: 8.1 */
	    .pdiv = 0x1 /* 2 */, .kdiv = 1, .qdiv_mode = 0, .qdiv_ratio = 0, }, },
};


/* Also used for 38.4 MHz values. */
static const struct icl_combo_pll_params icl_dp_combo_pll_19_2MHz_values[] = {
	{ 540000,
	  { .dco_integer = 0x1A5, .dco_fraction = 0x7000,		/* [0]: 5.4 */
	    .pdiv = 0x2 /* 3 */, .kdiv = 1, .qdiv_mode = 0, .qdiv_ratio = 0, }, },
	{ 270000,
	  { .dco_integer = 0x1A5, .dco_fraction = 0x7000,		/* [1]: 2.7 */
	    .pdiv = 0x2 /* 3 */, .kdiv = 2, .qdiv_mode = 0, .qdiv_ratio = 0, }, },
	{ 162000,
	  { .dco_integer = 0x1A5, .dco_fraction = 0x7000,		/* [2]: 1.62 */
	    .pdiv = 0x4 /* 5 */, .kdiv = 2, .qdiv_mode = 0, .qdiv_ratio = 0, }, },
	{ 324000,
	  { .dco_integer = 0x1A5, .dco_fraction = 0x7000,		/* [3]: 3.24 */
	    .pdiv = 0x4 /* 5 */, .kdiv = 1, .qdiv_mode = 0, .qdiv_ratio = 0, }, },
	{ 216000,
	  { .dco_integer = 0x1C2, .dco_fraction = 0x0000,		/* [4]: 2.16 */
	    .pdiv = 0x1 /* 2 */, .kdiv = 2, .qdiv_mode = 1, .qdiv_ratio = 2, }, },
	{ 432000,
	  { .dco_integer = 0x1C2, .dco_fraction = 0x0000,		/* [5]: 4.32 */
	    .pdiv = 0x1 /* 2 */, .kdiv = 2, .qdiv_mode = 0, .qdiv_ratio = 0, }, },
	{ 648000,
	  { .dco_integer = 0x1FA, .dco_fraction = 0x2000,		/* [6]: 6.48 */
	    .pdiv = 0x2 /* 3 */, .kdiv = 1, .qdiv_mode = 0, .qdiv_ratio = 0, }, },
	{ 810000,
	  { .dco_integer = 0x1A5, .dco_fraction = 0x7000,		/* [7]: 8.1 */
	    .pdiv = 0x1 /* 2 */, .kdiv = 1, .qdiv_mode = 0, .qdiv_ratio = 0, }, },
};

static const struct skl_wrpll_params icl_tbt_pll_24MHz_values = {
	.dco_integer = 0x151, .dco_fraction = 0x4000,
	.pdiv = 0x4 /* 5 */, .kdiv = 1, .qdiv_mode = 0, .qdiv_ratio = 0,
};

static const struct skl_wrpll_params icl_tbt_pll_19_2MHz_values = {
	.dco_integer = 0x1A5, .dco_fraction = 0x7000,
	.pdiv = 0x4 /* 5 */, .kdiv = 1, .qdiv_mode = 0, .qdiv_ratio = 0,
};

static const struct skl_wrpll_params tgl_tbt_pll_19_2MHz_values = {
	.dco_integer = 0x54, .dco_fraction = 0x3000,
	/* the following params are unused */
	.pdiv = 0, .kdiv = 0, .qdiv_mode = 0, .qdiv_ratio = 0,
};

static const struct skl_wrpll_params tgl_tbt_pll_24MHz_values = {
	.dco_integer = 0x43, .dco_fraction = 0x4000,
	/* the following params are unused */
};

static bool icl_calc_dp_combo_pll(struct intel_crtc_state *crtc_state,
				  struct skl_wrpll_params *pll_params)
{
	struct drm_i915_private *dev_priv = to_i915(crtc_state->uapi.crtc->dev);
	const struct icl_combo_pll_params *params =
		dev_priv->dpll.ref_clks.nssc == 24000 ?
		icl_dp_combo_pll_24MHz_values :
		icl_dp_combo_pll_19_2MHz_values;
	int clock = crtc_state->port_clock;
	int i;

	for (i = 0; i < ARRAY_SIZE(icl_dp_combo_pll_24MHz_values); i++) {
		if (clock == params[i].clock) {
			*pll_params = params[i].wrpll;
			return true;
		}
	}

	MISSING_CASE(clock);
	return false;
}

static bool icl_calc_tbt_pll(struct intel_crtc_state *crtc_state,
			     struct skl_wrpll_params *pll_params)
{
	struct drm_i915_private *dev_priv = to_i915(crtc_state->uapi.crtc->dev);

	if (DISPLAY_VER(dev_priv) >= 12) {
		switch (dev_priv->dpll.ref_clks.nssc) {
		default:
			MISSING_CASE(dev_priv->dpll.ref_clks.nssc);
			fallthrough;
		case 19200:
		case 38400:
			*pll_params = tgl_tbt_pll_19_2MHz_values;
			break;
		case 24000:
			*pll_params = tgl_tbt_pll_24MHz_values;
			break;
		}
	} else {
		switch (dev_priv->dpll.ref_clks.nssc) {
		default:
			MISSING_CASE(dev_priv->dpll.ref_clks.nssc);
			fallthrough;
		case 19200:
		case 38400:
			*pll_params = icl_tbt_pll_19_2MHz_values;
			break;
		case 24000:
			*pll_params = icl_tbt_pll_24MHz_values;
			break;
		}
	}

	return true;
}

static int icl_ddi_tbt_pll_get_freq(struct drm_i915_private *i915,
				    const struct intel_shared_dpll *pll,
				    const struct intel_dpll_hw_state *pll_state)
{
	/*
	 * The PLL outputs multiple frequencies at the same time, selection is
	 * made at DDI clock mux level.
	 */
	drm_WARN_ON(&i915->drm, 1);

	return 0;
}

static int icl_wrpll_ref_clock(struct drm_i915_private *i915)
{
	int ref_clock = i915->dpll.ref_clks.nssc;

	/*
	 * For ICL+, the spec states: if reference frequency is 38.4,
	 * use 19.2 because the DPLL automatically divides that by 2.
	 */
	if (ref_clock == 38400)
		ref_clock = 19200;

	return ref_clock;
}

static bool
icl_calc_wrpll(struct intel_crtc_state *crtc_state,
	       struct skl_wrpll_params *wrpll_params)
{
	struct drm_i915_private *i915 = to_i915(crtc_state->uapi.crtc->dev);

	return __cnl_ddi_calculate_wrpll(crtc_state, wrpll_params,
					 icl_wrpll_ref_clock(i915));
}

static int icl_ddi_combo_pll_get_freq(struct drm_i915_private *i915,
				      const struct intel_shared_dpll *pll,
				      const struct intel_dpll_hw_state *pll_state)
{
	return __cnl_ddi_wrpll_get_freq(i915, pll, pll_state,
					icl_wrpll_ref_clock(i915));
}

static void icl_calc_dpll_state(struct drm_i915_private *i915,
				const struct skl_wrpll_params *pll_params,
				struct intel_dpll_hw_state *pll_state)
{
	u32 dco_fraction = pll_params->dco_fraction;

	memset(pll_state, 0, sizeof(*pll_state));

	if (ehl_combo_pll_div_frac_wa_needed(i915))
		dco_fraction = DIV_ROUND_CLOSEST(dco_fraction, 2);

	pll_state->cfgcr0 = DPLL_CFGCR0_DCO_FRACTION(dco_fraction) |
			    pll_params->dco_integer;

	pll_state->cfgcr1 = DPLL_CFGCR1_QDIV_RATIO(pll_params->qdiv_ratio) |
			    DPLL_CFGCR1_QDIV_MODE(pll_params->qdiv_mode) |
			    DPLL_CFGCR1_KDIV(pll_params->kdiv) |
			    DPLL_CFGCR1_PDIV(pll_params->pdiv);

	if (DISPLAY_VER(i915) >= 12)
		pll_state->cfgcr1 |= TGL_DPLL_CFGCR1_CFSELOVRD_NORMAL_XTAL;
	else
		pll_state->cfgcr1 |= DPLL_CFGCR1_CENTRAL_FREQ_8400;
}

static bool icl_mg_pll_find_divisors(int clock_khz, bool is_dp, bool use_ssc,
				     u32 *target_dco_khz,
				     struct intel_dpll_hw_state *state,
				     bool is_dkl)
{
	u32 dco_min_freq, dco_max_freq;
	int div1_vals[] = {7, 5, 3, 2};
	unsigned int i;
	int div2;

	dco_min_freq = is_dp ? 8100000 : use_ssc ? 8000000 : 7992000;
	dco_max_freq = is_dp ? 8100000 : 10000000;

	for (i = 0; i < ARRAY_SIZE(div1_vals); i++) {
		int div1 = div1_vals[i];

		for (div2 = 10; div2 > 0; div2--) {
			int dco = div1 * div2 * clock_khz * 5;
			int a_divratio, tlinedrv, inputsel;
			u32 hsdiv;

			if (dco < dco_min_freq || dco > dco_max_freq)
				continue;

			if (div2 >= 2) {
				/*
				 * Note: a_divratio not matching TGL BSpec
				 * algorithm but matching hardcoded values and
				 * working on HW for DP alt-mode at least
				 */
				a_divratio = is_dp ? 10 : 5;
				tlinedrv = is_dkl ? 1 : 2;
			} else {
				a_divratio = 5;
				tlinedrv = 0;
			}
			inputsel = is_dp ? 0 : 1;

			switch (div1) {
			default:
				MISSING_CASE(div1);
				fallthrough;
			case 2:
				hsdiv = MG_CLKTOP2_HSCLKCTL_HSDIV_RATIO_2;
				break;
			case 3:
				hsdiv = MG_CLKTOP2_HSCLKCTL_HSDIV_RATIO_3;
				break;
			case 5:
				hsdiv = MG_CLKTOP2_HSCLKCTL_HSDIV_RATIO_5;
				break;
			case 7:
				hsdiv = MG_CLKTOP2_HSCLKCTL_HSDIV_RATIO_7;
				break;
			}

			*target_dco_khz = dco;

			state->mg_refclkin_ctl = MG_REFCLKIN_CTL_OD_2_MUX(1);

			state->mg_clktop2_coreclkctl1 =
				MG_CLKTOP2_CORECLKCTL1_A_DIVRATIO(a_divratio);

			state->mg_clktop2_hsclkctl =
				MG_CLKTOP2_HSCLKCTL_TLINEDRV_CLKSEL(tlinedrv) |
				MG_CLKTOP2_HSCLKCTL_CORE_INPUTSEL(inputsel) |
				hsdiv |
				MG_CLKTOP2_HSCLKCTL_DSDIV_RATIO(div2);

			return true;
		}
	}

	return false;
}

/*
 * The specification for this function uses real numbers, so the math had to be
 * adapted to integer-only calculation, that's why it looks so different.
 */
static bool icl_calc_mg_pll_state(struct intel_crtc_state *crtc_state,
				  struct intel_dpll_hw_state *pll_state)
{
	struct drm_i915_private *dev_priv = to_i915(crtc_state->uapi.crtc->dev);
	int refclk_khz = dev_priv->dpll.ref_clks.nssc;
	int clock = crtc_state->port_clock;
	u32 dco_khz, m1div, m2div_int, m2div_rem, m2div_frac;
	u32 iref_ndiv, iref_trim, iref_pulse_w;
	u32 prop_coeff, int_coeff;
	u32 tdc_targetcnt, feedfwgain;
	u64 ssc_stepsize, ssc_steplen, ssc_steplog;
	u64 tmp;
	bool use_ssc = false;
	bool is_dp = !intel_crtc_has_type(crtc_state, INTEL_OUTPUT_HDMI);
	bool is_dkl = DISPLAY_VER(dev_priv) >= 12;

	memset(pll_state, 0, sizeof(*pll_state));

	if (!icl_mg_pll_find_divisors(clock, is_dp, use_ssc, &dco_khz,
				      pll_state, is_dkl)) {
		drm_dbg_kms(&dev_priv->drm,
			    "Failed to find divisors for clock %d\n", clock);
		return false;
	}

	m1div = 2;
	m2div_int = dco_khz / (refclk_khz * m1div);
	if (m2div_int > 255) {
		if (!is_dkl) {
			m1div = 4;
			m2div_int = dco_khz / (refclk_khz * m1div);
		}

		if (m2div_int > 255) {
			drm_dbg_kms(&dev_priv->drm,
				    "Failed to find mdiv for clock %d\n",
				    clock);
			return false;
		}
	}
	m2div_rem = dco_khz % (refclk_khz * m1div);

	tmp = (u64)m2div_rem * (1 << 22);
	do_div(tmp, refclk_khz * m1div);
	m2div_frac = tmp;

	switch (refclk_khz) {
	case 19200:
		iref_ndiv = 1;
		iref_trim = 28;
		iref_pulse_w = 1;
		break;
	case 24000:
		iref_ndiv = 1;
		iref_trim = 25;
		iref_pulse_w = 2;
		break;
	case 38400:
		iref_ndiv = 2;
		iref_trim = 28;
		iref_pulse_w = 1;
		break;
	default:
		MISSING_CASE(refclk_khz);
		return false;
	}

	/*
	 * tdc_res = 0.000003
	 * tdc_targetcnt = int(2 / (tdc_res * 8 * 50 * 1.1) / refclk_mhz + 0.5)
	 *
	 * The multiplication by 1000 is due to refclk MHz to KHz conversion. It
	 * was supposed to be a division, but we rearranged the operations of
	 * the formula to avoid early divisions so we don't multiply the
	 * rounding errors.
	 *
	 * 0.000003 * 8 * 50 * 1.1 = 0.00132, also known as 132 / 100000, which
	 * we also rearrange to work with integers.
	 *
	 * The 0.5 transformed to 5 results in a multiplication by 10 and the
	 * last division by 10.
	 */
	tdc_targetcnt = (2 * 1000 * 100000 * 10 / (132 * refclk_khz) + 5) / 10;

	/*
	 * Here we divide dco_khz by 10 in order to allow the dividend to fit in
	 * 32 bits. That's not a problem since we round the division down
	 * anyway.
	 */
	feedfwgain = (use_ssc || m2div_rem > 0) ?
		m1div * 1000000 * 100 / (dco_khz * 3 / 10) : 0;

	if (dco_khz >= 9000000) {
		prop_coeff = 5;
		int_coeff = 10;
	} else {
		prop_coeff = 4;
		int_coeff = 8;
	}

	if (use_ssc) {
		tmp = mul_u32_u32(dco_khz, 47 * 32);
		do_div(tmp, refclk_khz * m1div * 10000);
		ssc_stepsize = tmp;

		tmp = mul_u32_u32(dco_khz, 1000);
		ssc_steplen = DIV_ROUND_UP_ULL(tmp, 32 * 2 * 32);
	} else {
		ssc_stepsize = 0;
		ssc_steplen = 0;
	}
	ssc_steplog = 4;

	/* write pll_state calculations */
	if (is_dkl) {
		pll_state->mg_pll_div0 = DKL_PLL_DIV0_INTEG_COEFF(int_coeff) |
					 DKL_PLL_DIV0_PROP_COEFF(prop_coeff) |
					 DKL_PLL_DIV0_FBPREDIV(m1div) |
					 DKL_PLL_DIV0_FBDIV_INT(m2div_int);

		pll_state->mg_pll_div1 = DKL_PLL_DIV1_IREF_TRIM(iref_trim) |
					 DKL_PLL_DIV1_TDC_TARGET_CNT(tdc_targetcnt);

		pll_state->mg_pll_ssc = DKL_PLL_SSC_IREF_NDIV_RATIO(iref_ndiv) |
					DKL_PLL_SSC_STEP_LEN(ssc_steplen) |
					DKL_PLL_SSC_STEP_NUM(ssc_steplog) |
					(use_ssc ? DKL_PLL_SSC_EN : 0);

		pll_state->mg_pll_bias = (m2div_frac ? DKL_PLL_BIAS_FRAC_EN_H : 0) |
					  DKL_PLL_BIAS_FBDIV_FRAC(m2div_frac);

		pll_state->mg_pll_tdc_coldst_bias =
				DKL_PLL_TDC_SSC_STEP_SIZE(ssc_stepsize) |
				DKL_PLL_TDC_FEED_FWD_GAIN(feedfwgain);

	} else {
		pll_state->mg_pll_div0 =
			(m2div_rem > 0 ? MG_PLL_DIV0_FRACNEN_H : 0) |
			MG_PLL_DIV0_FBDIV_FRAC(m2div_frac) |
			MG_PLL_DIV0_FBDIV_INT(m2div_int);

		pll_state->mg_pll_div1 =
			MG_PLL_DIV1_IREF_NDIVRATIO(iref_ndiv) |
			MG_PLL_DIV1_DITHER_DIV_2 |
			MG_PLL_DIV1_NDIVRATIO(1) |
			MG_PLL_DIV1_FBPREDIV(m1div);

		pll_state->mg_pll_lf =
			MG_PLL_LF_TDCTARGETCNT(tdc_targetcnt) |
			MG_PLL_LF_AFCCNTSEL_512 |
			MG_PLL_LF_GAINCTRL(1) |
			MG_PLL_LF_INT_COEFF(int_coeff) |
			MG_PLL_LF_PROP_COEFF(prop_coeff);

		pll_state->mg_pll_frac_lock =
			MG_PLL_FRAC_LOCK_TRUELOCK_CRIT_32 |
			MG_PLL_FRAC_LOCK_EARLYLOCK_CRIT_32 |
			MG_PLL_FRAC_LOCK_LOCKTHRESH(10) |
			MG_PLL_FRAC_LOCK_DCODITHEREN |
			MG_PLL_FRAC_LOCK_FEEDFWRDGAIN(feedfwgain);
		if (use_ssc || m2div_rem > 0)
			pll_state->mg_pll_frac_lock |=
				MG_PLL_FRAC_LOCK_FEEDFWRDCAL_EN;

		pll_state->mg_pll_ssc =
			(use_ssc ? MG_PLL_SSC_EN : 0) |
			MG_PLL_SSC_TYPE(2) |
			MG_PLL_SSC_STEPLENGTH(ssc_steplen) |
			MG_PLL_SSC_STEPNUM(ssc_steplog) |
			MG_PLL_SSC_FLLEN |
			MG_PLL_SSC_STEPSIZE(ssc_stepsize);

		pll_state->mg_pll_tdc_coldst_bias =
			MG_PLL_TDC_COLDST_COLDSTART |
			MG_PLL_TDC_COLDST_IREFINT_EN |
			MG_PLL_TDC_COLDST_REFBIAS_START_PULSE_W(iref_pulse_w) |
			MG_PLL_TDC_TDCOVCCORR_EN |
			MG_PLL_TDC_TDCSEL(3);

		pll_state->mg_pll_bias =
			MG_PLL_BIAS_BIAS_GB_SEL(3) |
			MG_PLL_BIAS_INIT_DCOAMP(0x3F) |
			MG_PLL_BIAS_BIAS_BONUS(10) |
			MG_PLL_BIAS_BIASCAL_EN |
			MG_PLL_BIAS_CTRIM(12) |
			MG_PLL_BIAS_VREF_RDAC(4) |
			MG_PLL_BIAS_IREFTRIM(iref_trim);

		if (refclk_khz == 38400) {
			pll_state->mg_pll_tdc_coldst_bias_mask =
				MG_PLL_TDC_COLDST_COLDSTART;
			pll_state->mg_pll_bias_mask = 0;
		} else {
			pll_state->mg_pll_tdc_coldst_bias_mask = -1U;
			pll_state->mg_pll_bias_mask = -1U;
		}

		pll_state->mg_pll_tdc_coldst_bias &=
			pll_state->mg_pll_tdc_coldst_bias_mask;
		pll_state->mg_pll_bias &= pll_state->mg_pll_bias_mask;
	}

	return true;
}

static int icl_ddi_mg_pll_get_freq(struct drm_i915_private *dev_priv,
				   const struct intel_shared_dpll *pll,
				   const struct intel_dpll_hw_state *pll_state)
{
	u32 m1, m2_int, m2_frac, div1, div2, ref_clock;
	u64 tmp;

	ref_clock = dev_priv->dpll.ref_clks.nssc;

	if (DISPLAY_VER(dev_priv) >= 12) {
		m1 = pll_state->mg_pll_div0 & DKL_PLL_DIV0_FBPREDIV_MASK;
		m1 = m1 >> DKL_PLL_DIV0_FBPREDIV_SHIFT;
		m2_int = pll_state->mg_pll_div0 & DKL_PLL_DIV0_FBDIV_INT_MASK;

		if (pll_state->mg_pll_bias & DKL_PLL_BIAS_FRAC_EN_H) {
			m2_frac = pll_state->mg_pll_bias &
				  DKL_PLL_BIAS_FBDIV_FRAC_MASK;
			m2_frac = m2_frac >> DKL_PLL_BIAS_FBDIV_SHIFT;
		} else {
			m2_frac = 0;
		}
	} else {
		m1 = pll_state->mg_pll_div1 & MG_PLL_DIV1_FBPREDIV_MASK;
		m2_int = pll_state->mg_pll_div0 & MG_PLL_DIV0_FBDIV_INT_MASK;

		if (pll_state->mg_pll_div0 & MG_PLL_DIV0_FRACNEN_H) {
			m2_frac = pll_state->mg_pll_div0 &
				  MG_PLL_DIV0_FBDIV_FRAC_MASK;
			m2_frac = m2_frac >> MG_PLL_DIV0_FBDIV_FRAC_SHIFT;
		} else {
			m2_frac = 0;
		}
	}

	switch (pll_state->mg_clktop2_hsclkctl &
		MG_CLKTOP2_HSCLKCTL_HSDIV_RATIO_MASK) {
	case MG_CLKTOP2_HSCLKCTL_HSDIV_RATIO_2:
		div1 = 2;
		break;
	case MG_CLKTOP2_HSCLKCTL_HSDIV_RATIO_3:
		div1 = 3;
		break;
	case MG_CLKTOP2_HSCLKCTL_HSDIV_RATIO_5:
		div1 = 5;
		break;
	case MG_CLKTOP2_HSCLKCTL_HSDIV_RATIO_7:
		div1 = 7;
		break;
	default:
		MISSING_CASE(pll_state->mg_clktop2_hsclkctl);
		return 0;
	}

	div2 = (pll_state->mg_clktop2_hsclkctl &
		MG_CLKTOP2_HSCLKCTL_DSDIV_RATIO_MASK) >>
		MG_CLKTOP2_HSCLKCTL_DSDIV_RATIO_SHIFT;

	/* div2 value of 0 is same as 1 means no div */
	if (div2 == 0)
		div2 = 1;

	/*
	 * Adjust the original formula to delay the division by 2^22 in order to
	 * minimize possible rounding errors.
	 */
	tmp = (u64)m1 * m2_int * ref_clock +
	      (((u64)m1 * m2_frac * ref_clock) >> 22);
	tmp = div_u64(tmp, 5 * div1 * div2);

	return tmp;
}

/**
 * icl_set_active_port_dpll - select the active port DPLL for a given CRTC
 * @crtc_state: state for the CRTC to select the DPLL for
 * @port_dpll_id: the active @port_dpll_id to select
 *
 * Select the given @port_dpll_id instance from the DPLLs reserved for the
 * CRTC.
 */
void icl_set_active_port_dpll(struct intel_crtc_state *crtc_state,
			      enum icl_port_dpll_id port_dpll_id)
{
	struct icl_port_dpll *port_dpll =
		&crtc_state->icl_port_dplls[port_dpll_id];

	crtc_state->shared_dpll = port_dpll->pll;
	crtc_state->dpll_hw_state = port_dpll->hw_state;
}

static void icl_update_active_dpll(struct intel_atomic_state *state,
				   struct intel_crtc *crtc,
				   struct intel_encoder *encoder)
{
	struct intel_crtc_state *crtc_state =
		intel_atomic_get_new_crtc_state(state, crtc);
	struct intel_digital_port *primary_port;
	enum icl_port_dpll_id port_dpll_id = ICL_PORT_DPLL_DEFAULT;

	primary_port = encoder->type == INTEL_OUTPUT_DP_MST ?
		enc_to_mst(encoder)->primary :
		enc_to_dig_port(encoder);

	if (primary_port &&
	    (primary_port->tc_mode == TC_PORT_DP_ALT ||
	     primary_port->tc_mode == TC_PORT_LEGACY))
		port_dpll_id = ICL_PORT_DPLL_MG_PHY;

	icl_set_active_port_dpll(crtc_state, port_dpll_id);
}

static u32 intel_get_hti_plls(struct drm_i915_private *i915)
{
	if (!(i915->hti_state & HDPORT_ENABLED))
		return 0;

	return REG_FIELD_GET(HDPORT_DPLL_USED_MASK, i915->hti_state);
}

static bool icl_get_combo_phy_dpll(struct intel_atomic_state *state,
				   struct intel_crtc *crtc,
				   struct intel_encoder *encoder)
{
	struct intel_crtc_state *crtc_state =
		intel_atomic_get_new_crtc_state(state, crtc);
	struct skl_wrpll_params pll_params = { };
	struct icl_port_dpll *port_dpll =
		&crtc_state->icl_port_dplls[ICL_PORT_DPLL_DEFAULT];
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	enum port port = encoder->port;
	unsigned long dpll_mask;
	int ret;

	if (intel_crtc_has_type(crtc_state, INTEL_OUTPUT_HDMI) ||
	    intel_crtc_has_type(crtc_state, INTEL_OUTPUT_DSI))
		ret = icl_calc_wrpll(crtc_state, &pll_params);
	else
		ret = icl_calc_dp_combo_pll(crtc_state, &pll_params);

	if (!ret) {
		drm_dbg_kms(&dev_priv->drm,
			    "Could not calculate combo PHY PLL state.\n");

		return false;
	}

	icl_calc_dpll_state(dev_priv, &pll_params, &port_dpll->hw_state);

	if (IS_ALDERLAKE_S(dev_priv)) {
		dpll_mask =
			BIT(DPLL_ID_DG1_DPLL3) |
			BIT(DPLL_ID_DG1_DPLL2) |
			BIT(DPLL_ID_ICL_DPLL1) |
			BIT(DPLL_ID_ICL_DPLL0);
	} else if (IS_DG1(dev_priv)) {
		if (port == PORT_D || port == PORT_E) {
			dpll_mask =
				BIT(DPLL_ID_DG1_DPLL2) |
				BIT(DPLL_ID_DG1_DPLL3);
		} else {
			dpll_mask =
				BIT(DPLL_ID_DG1_DPLL0) |
				BIT(DPLL_ID_DG1_DPLL1);
		}
	} else if (IS_ROCKETLAKE(dev_priv)) {
		dpll_mask =
			BIT(DPLL_ID_EHL_DPLL4) |
			BIT(DPLL_ID_ICL_DPLL1) |
			BIT(DPLL_ID_ICL_DPLL0);
	} else if (IS_JSL_EHL(dev_priv) && port != PORT_A) {
		dpll_mask =
			BIT(DPLL_ID_EHL_DPLL4) |
			BIT(DPLL_ID_ICL_DPLL1) |
			BIT(DPLL_ID_ICL_DPLL0);
	} else {
		dpll_mask = BIT(DPLL_ID_ICL_DPLL1) | BIT(DPLL_ID_ICL_DPLL0);
	}

	/* Eliminate DPLLs from consideration if reserved by HTI */
	dpll_mask &= ~intel_get_hti_plls(dev_priv);

	port_dpll->pll = intel_find_shared_dpll(state, crtc,
						&port_dpll->hw_state,
						dpll_mask);
	if (!port_dpll->pll) {
		drm_dbg_kms(&dev_priv->drm,
			    "No combo PHY PLL found for [ENCODER:%d:%s]\n",
			    encoder->base.base.id, encoder->base.name);
		return false;
	}

	intel_reference_shared_dpll(state, crtc,
				    port_dpll->pll, &port_dpll->hw_state);

	icl_update_active_dpll(state, crtc, encoder);

	return true;
}

static bool icl_get_tc_phy_dplls(struct intel_atomic_state *state,
				 struct intel_crtc *crtc,
				 struct intel_encoder *encoder)
{
	struct drm_i915_private *dev_priv = to_i915(state->base.dev);
	struct intel_crtc_state *crtc_state =
		intel_atomic_get_new_crtc_state(state, crtc);
	struct skl_wrpll_params pll_params = { };
	struct icl_port_dpll *port_dpll;
	enum intel_dpll_id dpll_id;

	port_dpll = &crtc_state->icl_port_dplls[ICL_PORT_DPLL_DEFAULT];
	if (!icl_calc_tbt_pll(crtc_state, &pll_params)) {
		drm_dbg_kms(&dev_priv->drm,
			    "Could not calculate TBT PLL state.\n");
		return false;
	}

	icl_calc_dpll_state(dev_priv, &pll_params, &port_dpll->hw_state);

	port_dpll->pll = intel_find_shared_dpll(state, crtc,
						&port_dpll->hw_state,
						BIT(DPLL_ID_ICL_TBTPLL));
	if (!port_dpll->pll) {
		drm_dbg_kms(&dev_priv->drm, "No TBT-ALT PLL found\n");
		return false;
	}
	intel_reference_shared_dpll(state, crtc,
				    port_dpll->pll, &port_dpll->hw_state);


	port_dpll = &crtc_state->icl_port_dplls[ICL_PORT_DPLL_MG_PHY];
	if (!icl_calc_mg_pll_state(crtc_state, &port_dpll->hw_state)) {
		drm_dbg_kms(&dev_priv->drm,
			    "Could not calculate MG PHY PLL state.\n");
		goto err_unreference_tbt_pll;
	}

	dpll_id = icl_tc_port_to_pll_id(intel_port_to_tc(dev_priv,
							 encoder->port));
	port_dpll->pll = intel_find_shared_dpll(state, crtc,
						&port_dpll->hw_state,
						BIT(dpll_id));
	if (!port_dpll->pll) {
		drm_dbg_kms(&dev_priv->drm, "No MG PHY PLL found\n");
		goto err_unreference_tbt_pll;
	}
	intel_reference_shared_dpll(state, crtc,
				    port_dpll->pll, &port_dpll->hw_state);

	icl_update_active_dpll(state, crtc, encoder);

	return true;

err_unreference_tbt_pll:
	port_dpll = &crtc_state->icl_port_dplls[ICL_PORT_DPLL_DEFAULT];
	intel_unreference_shared_dpll(state, crtc, port_dpll->pll);

	return false;
}

static bool icl_get_dplls(struct intel_atomic_state *state,
			  struct intel_crtc *crtc,
			  struct intel_encoder *encoder)
{
	struct drm_i915_private *dev_priv = to_i915(state->base.dev);
	enum phy phy = intel_port_to_phy(dev_priv, encoder->port);

	if (intel_phy_is_combo(dev_priv, phy))
		return icl_get_combo_phy_dpll(state, crtc, encoder);
	else if (intel_phy_is_tc(dev_priv, phy))
		return icl_get_tc_phy_dplls(state, crtc, encoder);

	MISSING_CASE(phy);

	return false;
}

static void icl_put_dplls(struct intel_atomic_state *state,
			  struct intel_crtc *crtc)
{
	const struct intel_crtc_state *old_crtc_state =
		intel_atomic_get_old_crtc_state(state, crtc);
	struct intel_crtc_state *new_crtc_state =
		intel_atomic_get_new_crtc_state(state, crtc);
	enum icl_port_dpll_id id;

	new_crtc_state->shared_dpll = NULL;

	for (id = ICL_PORT_DPLL_DEFAULT; id < ICL_PORT_DPLL_COUNT; id++) {
		const struct icl_port_dpll *old_port_dpll =
			&old_crtc_state->icl_port_dplls[id];
		struct icl_port_dpll *new_port_dpll =
			&new_crtc_state->icl_port_dplls[id];

		new_port_dpll->pll = NULL;

		if (!old_port_dpll->pll)
			continue;

		intel_unreference_shared_dpll(state, crtc, old_port_dpll->pll);
	}
}

static bool mg_pll_get_hw_state(struct drm_i915_private *dev_priv,
				struct intel_shared_dpll *pll,
				struct intel_dpll_hw_state *hw_state)
{
	const enum intel_dpll_id id = pll->info->id;
	enum tc_port tc_port = icl_pll_id_to_tc_port(id);
	intel_wakeref_t wakeref;
	bool ret = false;
	u32 val;

	i915_reg_t enable_reg = intel_tc_pll_enable_reg(dev_priv, pll);

	wakeref = intel_display_power_get_if_enabled(dev_priv,
						     POWER_DOMAIN_DISPLAY_CORE);
	if (!wakeref)
		return false;

	val = intel_de_read(dev_priv, enable_reg);
	if (!(val & PLL_ENABLE))
		goto out;

	hw_state->mg_refclkin_ctl = intel_de_read(dev_priv,
						  MG_REFCLKIN_CTL(tc_port));
	hw_state->mg_refclkin_ctl &= MG_REFCLKIN_CTL_OD_2_MUX_MASK;

	hw_state->mg_clktop2_coreclkctl1 =
		intel_de_read(dev_priv, MG_CLKTOP2_CORECLKCTL1(tc_port));
	hw_state->mg_clktop2_coreclkctl1 &=
		MG_CLKTOP2_CORECLKCTL1_A_DIVRATIO_MASK;

	hw_state->mg_clktop2_hsclkctl =
		intel_de_read(dev_priv, MG_CLKTOP2_HSCLKCTL(tc_port));
	hw_state->mg_clktop2_hsclkctl &=
		MG_CLKTOP2_HSCLKCTL_TLINEDRV_CLKSEL_MASK |
		MG_CLKTOP2_HSCLKCTL_CORE_INPUTSEL_MASK |
		MG_CLKTOP2_HSCLKCTL_HSDIV_RATIO_MASK |
		MG_CLKTOP2_HSCLKCTL_DSDIV_RATIO_MASK;

	hw_state->mg_pll_div0 = intel_de_read(dev_priv, MG_PLL_DIV0(tc_port));
	hw_state->mg_pll_div1 = intel_de_read(dev_priv, MG_PLL_DIV1(tc_port));
	hw_state->mg_pll_lf = intel_de_read(dev_priv, MG_PLL_LF(tc_port));
	hw_state->mg_pll_frac_lock = intel_de_read(dev_priv,
						   MG_PLL_FRAC_LOCK(tc_port));
	hw_state->mg_pll_ssc = intel_de_read(dev_priv, MG_PLL_SSC(tc_port));

	hw_state->mg_pll_bias = intel_de_read(dev_priv, MG_PLL_BIAS(tc_port));
	hw_state->mg_pll_tdc_coldst_bias =
		intel_de_read(dev_priv, MG_PLL_TDC_COLDST_BIAS(tc_port));

	if (dev_priv->dpll.ref_clks.nssc == 38400) {
		hw_state->mg_pll_tdc_coldst_bias_mask = MG_PLL_TDC_COLDST_COLDSTART;
		hw_state->mg_pll_bias_mask = 0;
	} else {
		hw_state->mg_pll_tdc_coldst_bias_mask = -1U;
		hw_state->mg_pll_bias_mask = -1U;
	}

	hw_state->mg_pll_tdc_coldst_bias &= hw_state->mg_pll_tdc_coldst_bias_mask;
	hw_state->mg_pll_bias &= hw_state->mg_pll_bias_mask;

	ret = true;
out:
	intel_display_power_put(dev_priv, POWER_DOMAIN_DISPLAY_CORE, wakeref);
	return ret;
}

static bool dkl_pll_get_hw_state(struct drm_i915_private *dev_priv,
				 struct intel_shared_dpll *pll,
				 struct intel_dpll_hw_state *hw_state)
{
	const enum intel_dpll_id id = pll->info->id;
	enum tc_port tc_port = icl_pll_id_to_tc_port(id);
	intel_wakeref_t wakeref;
	bool ret = false;
	u32 val;

	wakeref = intel_display_power_get_if_enabled(dev_priv,
						     POWER_DOMAIN_DISPLAY_CORE);
	if (!wakeref)
		return false;

	val = intel_de_read(dev_priv, intel_tc_pll_enable_reg(dev_priv, pll));
	if (!(val & PLL_ENABLE))
		goto out;

	/*
	 * All registers read here have the same HIP_INDEX_REG even though
	 * they are on different building blocks
	 */
	intel_de_write(dev_priv, HIP_INDEX_REG(tc_port),
		       HIP_INDEX_VAL(tc_port, 0x2));

	hw_state->mg_refclkin_ctl = intel_de_read(dev_priv,
						  DKL_REFCLKIN_CTL(tc_port));
	hw_state->mg_refclkin_ctl &= MG_REFCLKIN_CTL_OD_2_MUX_MASK;

	hw_state->mg_clktop2_hsclkctl =
		intel_de_read(dev_priv, DKL_CLKTOP2_HSCLKCTL(tc_port));
	hw_state->mg_clktop2_hsclkctl &=
		MG_CLKTOP2_HSCLKCTL_TLINEDRV_CLKSEL_MASK |
		MG_CLKTOP2_HSCLKCTL_CORE_INPUTSEL_MASK |
		MG_CLKTOP2_HSCLKCTL_HSDIV_RATIO_MASK |
		MG_CLKTOP2_HSCLKCTL_DSDIV_RATIO_MASK;

	hw_state->mg_clktop2_coreclkctl1 =
		intel_de_read(dev_priv, DKL_CLKTOP2_CORECLKCTL1(tc_port));
	hw_state->mg_clktop2_coreclkctl1 &=
		MG_CLKTOP2_CORECLKCTL1_A_DIVRATIO_MASK;

	hw_state->mg_pll_div0 = intel_de_read(dev_priv, DKL_PLL_DIV0(tc_port));
	hw_state->mg_pll_div0 &= (DKL_PLL_DIV0_INTEG_COEFF_MASK |
				  DKL_PLL_DIV0_PROP_COEFF_MASK |
				  DKL_PLL_DIV0_FBPREDIV_MASK |
				  DKL_PLL_DIV0_FBDIV_INT_MASK);

	hw_state->mg_pll_div1 = intel_de_read(dev_priv, DKL_PLL_DIV1(tc_port));
	hw_state->mg_pll_div1 &= (DKL_PLL_DIV1_IREF_TRIM_MASK |
				  DKL_PLL_DIV1_TDC_TARGET_CNT_MASK);

	hw_state->mg_pll_ssc = intel_de_read(dev_priv, DKL_PLL_SSC(tc_port));
	hw_state->mg_pll_ssc &= (DKL_PLL_SSC_IREF_NDIV_RATIO_MASK |
				 DKL_PLL_SSC_STEP_LEN_MASK |
				 DKL_PLL_SSC_STEP_NUM_MASK |
				 DKL_PLL_SSC_EN);

	hw_state->mg_pll_bias = intel_de_read(dev_priv, DKL_PLL_BIAS(tc_port));
	hw_state->mg_pll_bias &= (DKL_PLL_BIAS_FRAC_EN_H |
				  DKL_PLL_BIAS_FBDIV_FRAC_MASK);

	hw_state->mg_pll_tdc_coldst_bias =
		intel_de_read(dev_priv, DKL_PLL_TDC_COLDST_BIAS(tc_port));
	hw_state->mg_pll_tdc_coldst_bias &= (DKL_PLL_TDC_SSC_STEP_SIZE_MASK |
					     DKL_PLL_TDC_FEED_FWD_GAIN_MASK);

	ret = true;
out:
	intel_display_power_put(dev_priv, POWER_DOMAIN_DISPLAY_CORE, wakeref);
	return ret;
}

static bool icl_pll_get_hw_state(struct drm_i915_private *dev_priv,
				 struct intel_shared_dpll *pll,
				 struct intel_dpll_hw_state *hw_state,
				 i915_reg_t enable_reg)
{
	const enum intel_dpll_id id = pll->info->id;
	intel_wakeref_t wakeref;
	bool ret = false;
	u32 val;

	wakeref = intel_display_power_get_if_enabled(dev_priv,
						     POWER_DOMAIN_DISPLAY_CORE);
	if (!wakeref)
		return false;

	val = intel_de_read(dev_priv, enable_reg);
	if (!(val & PLL_ENABLE))
		goto out;

	if (IS_ALDERLAKE_S(dev_priv)) {
		hw_state->cfgcr0 = intel_de_read(dev_priv, ADLS_DPLL_CFGCR0(id));
		hw_state->cfgcr1 = intel_de_read(dev_priv, ADLS_DPLL_CFGCR1(id));
	} else if (IS_DG1(dev_priv)) {
		hw_state->cfgcr0 = intel_de_read(dev_priv, DG1_DPLL_CFGCR0(id));
		hw_state->cfgcr1 = intel_de_read(dev_priv, DG1_DPLL_CFGCR1(id));
	} else if (IS_ROCKETLAKE(dev_priv)) {
		hw_state->cfgcr0 = intel_de_read(dev_priv,
						 RKL_DPLL_CFGCR0(id));
		hw_state->cfgcr1 = intel_de_read(dev_priv,
						 RKL_DPLL_CFGCR1(id));
	} else if (DISPLAY_VER(dev_priv) >= 12) {
		hw_state->cfgcr0 = intel_de_read(dev_priv,
						 TGL_DPLL_CFGCR0(id));
		hw_state->cfgcr1 = intel_de_read(dev_priv,
						 TGL_DPLL_CFGCR1(id));
	} else {
		if (IS_JSL_EHL(dev_priv) && id == DPLL_ID_EHL_DPLL4) {
			hw_state->cfgcr0 = intel_de_read(dev_priv,
							 ICL_DPLL_CFGCR0(4));
			hw_state->cfgcr1 = intel_de_read(dev_priv,
							 ICL_DPLL_CFGCR1(4));
		} else {
			hw_state->cfgcr0 = intel_de_read(dev_priv,
							 ICL_DPLL_CFGCR0(id));
			hw_state->cfgcr1 = intel_de_read(dev_priv,
							 ICL_DPLL_CFGCR1(id));
		}
	}

	ret = true;
out:
	intel_display_power_put(dev_priv, POWER_DOMAIN_DISPLAY_CORE, wakeref);
	return ret;
}

static bool combo_pll_get_hw_state(struct drm_i915_private *dev_priv,
				   struct intel_shared_dpll *pll,
				   struct intel_dpll_hw_state *hw_state)
{
	i915_reg_t enable_reg = intel_combo_pll_enable_reg(dev_priv, pll);

	return icl_pll_get_hw_state(dev_priv, pll, hw_state, enable_reg);
}

static bool tbt_pll_get_hw_state(struct drm_i915_private *dev_priv,
				 struct intel_shared_dpll *pll,
				 struct intel_dpll_hw_state *hw_state)
{
	return icl_pll_get_hw_state(dev_priv, pll, hw_state, TBT_PLL_ENABLE);
}

static void icl_dpll_write(struct drm_i915_private *dev_priv,
			   struct intel_shared_dpll *pll)
{
	struct intel_dpll_hw_state *hw_state = &pll->state.hw_state;
	const enum intel_dpll_id id = pll->info->id;
	i915_reg_t cfgcr0_reg, cfgcr1_reg;

	if (IS_ALDERLAKE_S(dev_priv)) {
		cfgcr0_reg = ADLS_DPLL_CFGCR0(id);
		cfgcr1_reg = ADLS_DPLL_CFGCR1(id);
	} else if (IS_DG1(dev_priv)) {
		cfgcr0_reg = DG1_DPLL_CFGCR0(id);
		cfgcr1_reg = DG1_DPLL_CFGCR1(id);
	} else if (IS_ROCKETLAKE(dev_priv)) {
		cfgcr0_reg = RKL_DPLL_CFGCR0(id);
		cfgcr1_reg = RKL_DPLL_CFGCR1(id);
	} else if (DISPLAY_VER(dev_priv) >= 12) {
		cfgcr0_reg = TGL_DPLL_CFGCR0(id);
		cfgcr1_reg = TGL_DPLL_CFGCR1(id);
	} else {
		if (IS_JSL_EHL(dev_priv) && id == DPLL_ID_EHL_DPLL4) {
			cfgcr0_reg = ICL_DPLL_CFGCR0(4);
			cfgcr1_reg = ICL_DPLL_CFGCR1(4);
		} else {
			cfgcr0_reg = ICL_DPLL_CFGCR0(id);
			cfgcr1_reg = ICL_DPLL_CFGCR1(id);
		}
	}

	intel_de_write(dev_priv, cfgcr0_reg, hw_state->cfgcr0);
	intel_de_write(dev_priv, cfgcr1_reg, hw_state->cfgcr1);
	intel_de_posting_read(dev_priv, cfgcr1_reg);
}

static void icl_mg_pll_write(struct drm_i915_private *dev_priv,
			     struct intel_shared_dpll *pll)
{
	struct intel_dpll_hw_state *hw_state = &pll->state.hw_state;
	enum tc_port tc_port = icl_pll_id_to_tc_port(pll->info->id);
	u32 val;

	/*
	 * Some of the following registers have reserved fields, so program
	 * these with RMW based on a mask. The mask can be fixed or generated
	 * during the calc/readout phase if the mask depends on some other HW
	 * state like refclk, see icl_calc_mg_pll_state().
	 */
	val = intel_de_read(dev_priv, MG_REFCLKIN_CTL(tc_port));
	val &= ~MG_REFCLKIN_CTL_OD_2_MUX_MASK;
	val |= hw_state->mg_refclkin_ctl;
	intel_de_write(dev_priv, MG_REFCLKIN_CTL(tc_port), val);

	val = intel_de_read(dev_priv, MG_CLKTOP2_CORECLKCTL1(tc_port));
	val &= ~MG_CLKTOP2_CORECLKCTL1_A_DIVRATIO_MASK;
	val |= hw_state->mg_clktop2_coreclkctl1;
	intel_de_write(dev_priv, MG_CLKTOP2_CORECLKCTL1(tc_port), val);

	val = intel_de_read(dev_priv, MG_CLKTOP2_HSCLKCTL(tc_port));
	val &= ~(MG_CLKTOP2_HSCLKCTL_TLINEDRV_CLKSEL_MASK |
		 MG_CLKTOP2_HSCLKCTL_CORE_INPUTSEL_MASK |
		 MG_CLKTOP2_HSCLKCTL_HSDIV_RATIO_MASK |
		 MG_CLKTOP2_HSCLKCTL_DSDIV_RATIO_MASK);
	val |= hw_state->mg_clktop2_hsclkctl;
	intel_de_write(dev_priv, MG_CLKTOP2_HSCLKCTL(tc_port), val);

	intel_de_write(dev_priv, MG_PLL_DIV0(tc_port), hw_state->mg_pll_div0);
	intel_de_write(dev_priv, MG_PLL_DIV1(tc_port), hw_state->mg_pll_div1);
	intel_de_write(dev_priv, MG_PLL_LF(tc_port), hw_state->mg_pll_lf);
	intel_de_write(dev_priv, MG_PLL_FRAC_LOCK(tc_port),
		       hw_state->mg_pll_frac_lock);
	intel_de_write(dev_priv, MG_PLL_SSC(tc_port), hw_state->mg_pll_ssc);

	val = intel_de_read(dev_priv, MG_PLL_BIAS(tc_port));
	val &= ~hw_state->mg_pll_bias_mask;
	val |= hw_state->mg_pll_bias;
	intel_de_write(dev_priv, MG_PLL_BIAS(tc_port), val);

	val = intel_de_read(dev_priv, MG_PLL_TDC_COLDST_BIAS(tc_port));
	val &= ~hw_state->mg_pll_tdc_coldst_bias_mask;
	val |= hw_state->mg_pll_tdc_coldst_bias;
	intel_de_write(dev_priv, MG_PLL_TDC_COLDST_BIAS(tc_port), val);

	intel_de_posting_read(dev_priv, MG_PLL_TDC_COLDST_BIAS(tc_port));
}

static void dkl_pll_write(struct drm_i915_private *dev_priv,
			  struct intel_shared_dpll *pll)
{
	struct intel_dpll_hw_state *hw_state = &pll->state.hw_state;
	enum tc_port tc_port = icl_pll_id_to_tc_port(pll->info->id);
	u32 val;

	/*
	 * All registers programmed here have the same HIP_INDEX_REG even
	 * though on different building block
	 */
	intel_de_write(dev_priv, HIP_INDEX_REG(tc_port),
		       HIP_INDEX_VAL(tc_port, 0x2));

	/* All the registers are RMW */
	val = intel_de_read(dev_priv, DKL_REFCLKIN_CTL(tc_port));
	val &= ~MG_REFCLKIN_CTL_OD_2_MUX_MASK;
	val |= hw_state->mg_refclkin_ctl;
	intel_de_write(dev_priv, DKL_REFCLKIN_CTL(tc_port), val);

	val = intel_de_read(dev_priv, DKL_CLKTOP2_CORECLKCTL1(tc_port));
	val &= ~MG_CLKTOP2_CORECLKCTL1_A_DIVRATIO_MASK;
	val |= hw_state->mg_clktop2_coreclkctl1;
	intel_de_write(dev_priv, DKL_CLKTOP2_CORECLKCTL1(tc_port), val);

	val = intel_de_read(dev_priv, DKL_CLKTOP2_HSCLKCTL(tc_port));
	val &= ~(MG_CLKTOP2_HSCLKCTL_TLINEDRV_CLKSEL_MASK |
		 MG_CLKTOP2_HSCLKCTL_CORE_INPUTSEL_MASK |
		 MG_CLKTOP2_HSCLKCTL_HSDIV_RATIO_MASK |
		 MG_CLKTOP2_HSCLKCTL_DSDIV_RATIO_MASK);
	val |= hw_state->mg_clktop2_hsclkctl;
	intel_de_write(dev_priv, DKL_CLKTOP2_HSCLKCTL(tc_port), val);

	val = intel_de_read(dev_priv, DKL_PLL_DIV0(tc_port));
	val &= ~(DKL_PLL_DIV0_INTEG_COEFF_MASK |
		 DKL_PLL_DIV0_PROP_COEFF_MASK |
		 DKL_PLL_DIV0_FBPREDIV_MASK |
		 DKL_PLL_DIV0_FBDIV_INT_MASK);
	val |= hw_state->mg_pll_div0;
	intel_de_write(dev_priv, DKL_PLL_DIV0(tc_port), val);

	val = intel_de_read(dev_priv, DKL_PLL_DIV1(tc_port));
	val &= ~(DKL_PLL_DIV1_IREF_TRIM_MASK |
		 DKL_PLL_DIV1_TDC_TARGET_CNT_MASK);
	val |= hw_state->mg_pll_div1;
	intel_de_write(dev_priv, DKL_PLL_DIV1(tc_port), val);

	val = intel_de_read(dev_priv, DKL_PLL_SSC(tc_port));
	val &= ~(DKL_PLL_SSC_IREF_NDIV_RATIO_MASK |
		 DKL_PLL_SSC_STEP_LEN_MASK |
		 DKL_PLL_SSC_STEP_NUM_MASK |
		 DKL_PLL_SSC_EN);
	val |= hw_state->mg_pll_ssc;
	intel_de_write(dev_priv, DKL_PLL_SSC(tc_port), val);

	val = intel_de_read(dev_priv, DKL_PLL_BIAS(tc_port));
	val &= ~(DKL_PLL_BIAS_FRAC_EN_H |
		 DKL_PLL_BIAS_FBDIV_FRAC_MASK);
	val |= hw_state->mg_pll_bias;
	intel_de_write(dev_priv, DKL_PLL_BIAS(tc_port), val);

	val = intel_de_read(dev_priv, DKL_PLL_TDC_COLDST_BIAS(tc_port));
	val &= ~(DKL_PLL_TDC_SSC_STEP_SIZE_MASK |
		 DKL_PLL_TDC_FEED_FWD_GAIN_MASK);
	val |= hw_state->mg_pll_tdc_coldst_bias;
	intel_de_write(dev_priv, DKL_PLL_TDC_COLDST_BIAS(tc_port), val);

	intel_de_posting_read(dev_priv, DKL_PLL_TDC_COLDST_BIAS(tc_port));
}

static void icl_pll_power_enable(struct drm_i915_private *dev_priv,
				 struct intel_shared_dpll *pll,
				 i915_reg_t enable_reg)
{
	u32 val;

	val = intel_de_read(dev_priv, enable_reg);
	val |= PLL_POWER_ENABLE;
	intel_de_write(dev_priv, enable_reg, val);

	/*
	 * The spec says we need to "wait" but it also says it should be
	 * immediate.
	 */
	if (intel_de_wait_for_set(dev_priv, enable_reg, PLL_POWER_STATE, 1))
		drm_err(&dev_priv->drm, "PLL %d Power not enabled\n",
			pll->info->id);
}

static void icl_pll_enable(struct drm_i915_private *dev_priv,
			   struct intel_shared_dpll *pll,
			   i915_reg_t enable_reg)
{
	u32 val;

	val = intel_de_read(dev_priv, enable_reg);
	val |= PLL_ENABLE;
	intel_de_write(dev_priv, enable_reg, val);

	/* Timeout is actually 600us. */
	if (intel_de_wait_for_set(dev_priv, enable_reg, PLL_LOCK, 1))
		drm_err(&dev_priv->drm, "PLL %d not locked\n", pll->info->id);
}

static void combo_pll_enable(struct drm_i915_private *dev_priv,
			     struct intel_shared_dpll *pll)
{
	i915_reg_t enable_reg = intel_combo_pll_enable_reg(dev_priv, pll);

	if (IS_JSL_EHL(dev_priv) &&
	    pll->info->id == DPLL_ID_EHL_DPLL4) {

		/*
		 * We need to disable DC states when this DPLL is enabled.
		 * This can be done by taking a reference on DPLL4 power
		 * domain.
		 */
		pll->wakeref = intel_display_power_get(dev_priv,
						       POWER_DOMAIN_DPLL_DC_OFF);
	}

	icl_pll_power_enable(dev_priv, pll, enable_reg);

	icl_dpll_write(dev_priv, pll);

	/*
	 * DVFS pre sequence would be here, but in our driver the cdclk code
	 * paths should already be setting the appropriate voltage, hence we do
	 * nothing here.
	 */

	icl_pll_enable(dev_priv, pll, enable_reg);

	/* DVFS post sequence would be here. See the comment above. */
}

static void tbt_pll_enable(struct drm_i915_private *dev_priv,
			   struct intel_shared_dpll *pll)
{
	icl_pll_power_enable(dev_priv, pll, TBT_PLL_ENABLE);

	icl_dpll_write(dev_priv, pll);

	/*
	 * DVFS pre sequence would be here, but in our driver the cdclk code
	 * paths should already be setting the appropriate voltage, hence we do
	 * nothing here.
	 */

	icl_pll_enable(dev_priv, pll, TBT_PLL_ENABLE);

	/* DVFS post sequence would be here. See the comment above. */
}

static void mg_pll_enable(struct drm_i915_private *dev_priv,
			  struct intel_shared_dpll *pll)
{
	i915_reg_t enable_reg = intel_tc_pll_enable_reg(dev_priv, pll);

	icl_pll_power_enable(dev_priv, pll, enable_reg);

	if (DISPLAY_VER(dev_priv) >= 12)
		dkl_pll_write(dev_priv, pll);
	else
		icl_mg_pll_write(dev_priv, pll);

	/*
	 * DVFS pre sequence would be here, but in our driver the cdclk code
	 * paths should already be setting the appropriate voltage, hence we do
	 * nothing here.
	 */

	icl_pll_enable(dev_priv, pll, enable_reg);

	/* DVFS post sequence would be here. See the comment above. */
}

static void icl_pll_disable(struct drm_i915_private *dev_priv,
			    struct intel_shared_dpll *pll,
			    i915_reg_t enable_reg)
{
	u32 val;

	/* The first steps are done by intel_ddi_post_disable(). */

	/*
	 * DVFS pre sequence would be here, but in our driver the cdclk code
	 * paths should already be setting the appropriate voltage, hence we do
	 * nothing here.
	 */

	val = intel_de_read(dev_priv, enable_reg);
	val &= ~PLL_ENABLE;
	intel_de_write(dev_priv, enable_reg, val);

	/* Timeout is actually 1us. */
	if (intel_de_wait_for_clear(dev_priv, enable_reg, PLL_LOCK, 1))
		drm_err(&dev_priv->drm, "PLL %d locked\n", pll->info->id);

	/* DVFS post sequence would be here. See the comment above. */

	val = intel_de_read(dev_priv, enable_reg);
	val &= ~PLL_POWER_ENABLE;
	intel_de_write(dev_priv, enable_reg, val);

	/*
	 * The spec says we need to "wait" but it also says it should be
	 * immediate.
	 */
	if (intel_de_wait_for_clear(dev_priv, enable_reg, PLL_POWER_STATE, 1))
		drm_err(&dev_priv->drm, "PLL %d Power not disabled\n",
			pll->info->id);
}

static void combo_pll_disable(struct drm_i915_private *dev_priv,
			      struct intel_shared_dpll *pll)
{
	i915_reg_t enable_reg = intel_combo_pll_enable_reg(dev_priv, pll);

	icl_pll_disable(dev_priv, pll, enable_reg);

	if (IS_JSL_EHL(dev_priv) &&
	    pll->info->id == DPLL_ID_EHL_DPLL4)
		intel_display_power_put(dev_priv, POWER_DOMAIN_DPLL_DC_OFF,
					pll->wakeref);
}

static void tbt_pll_disable(struct drm_i915_private *dev_priv,
			    struct intel_shared_dpll *pll)
{
	icl_pll_disable(dev_priv, pll, TBT_PLL_ENABLE);
}

static void mg_pll_disable(struct drm_i915_private *dev_priv,
			   struct intel_shared_dpll *pll)
{
	i915_reg_t enable_reg = intel_tc_pll_enable_reg(dev_priv, pll);

	icl_pll_disable(dev_priv, pll, enable_reg);
}

static void icl_update_dpll_ref_clks(struct drm_i915_private *i915)
{
	/* No SSC ref */
	i915->dpll.ref_clks.nssc = i915->cdclk.hw.ref;
}

static void icl_dump_hw_state(struct drm_i915_private *dev_priv,
			      const struct intel_dpll_hw_state *hw_state)
{
	drm_dbg_kms(&dev_priv->drm,
		    "dpll_hw_state: cfgcr0: 0x%x, cfgcr1: 0x%x, "
		    "mg_refclkin_ctl: 0x%x, hg_clktop2_coreclkctl1: 0x%x, "
		    "mg_clktop2_hsclkctl: 0x%x, mg_pll_div0: 0x%x, "
		    "mg_pll_div2: 0x%x, mg_pll_lf: 0x%x, "
		    "mg_pll_frac_lock: 0x%x, mg_pll_ssc: 0x%x, "
		    "mg_pll_bias: 0x%x, mg_pll_tdc_coldst_bias: 0x%x\n",
		    hw_state->cfgcr0, hw_state->cfgcr1,
		    hw_state->mg_refclkin_ctl,
		    hw_state->mg_clktop2_coreclkctl1,
		    hw_state->mg_clktop2_hsclkctl,
		    hw_state->mg_pll_div0,
		    hw_state->mg_pll_div1,
		    hw_state->mg_pll_lf,
		    hw_state->mg_pll_frac_lock,
		    hw_state->mg_pll_ssc,
		    hw_state->mg_pll_bias,
		    hw_state->mg_pll_tdc_coldst_bias);
}

static const struct intel_shared_dpll_funcs combo_pll_funcs = {
	.enable = combo_pll_enable,
	.disable = combo_pll_disable,
	.get_hw_state = combo_pll_get_hw_state,
	.get_freq = icl_ddi_combo_pll_get_freq,
};

static const struct intel_shared_dpll_funcs tbt_pll_funcs = {
	.enable = tbt_pll_enable,
	.disable = tbt_pll_disable,
	.get_hw_state = tbt_pll_get_hw_state,
	.get_freq = icl_ddi_tbt_pll_get_freq,
};

static const struct intel_shared_dpll_funcs mg_pll_funcs = {
	.enable = mg_pll_enable,
	.disable = mg_pll_disable,
	.get_hw_state = mg_pll_get_hw_state,
	.get_freq = icl_ddi_mg_pll_get_freq,
};

static const struct dpll_info icl_plls[] = {
	{ "DPLL 0",   &combo_pll_funcs, DPLL_ID_ICL_DPLL0,  0 },
	{ "DPLL 1",   &combo_pll_funcs, DPLL_ID_ICL_DPLL1,  0 },
	{ "TBT PLL",  &tbt_pll_funcs, DPLL_ID_ICL_TBTPLL, 0 },
	{ "MG PLL 1", &mg_pll_funcs, DPLL_ID_ICL_MGPLL1, 0 },
	{ "MG PLL 2", &mg_pll_funcs, DPLL_ID_ICL_MGPLL2, 0 },
	{ "MG PLL 3", &mg_pll_funcs, DPLL_ID_ICL_MGPLL3, 0 },
	{ "MG PLL 4", &mg_pll_funcs, DPLL_ID_ICL_MGPLL4, 0 },
	{ },
};

static const struct intel_dpll_mgr icl_pll_mgr = {
	.dpll_info = icl_plls,
	.get_dplls = icl_get_dplls,
	.put_dplls = icl_put_dplls,
	.update_active_dpll = icl_update_active_dpll,
	.update_ref_clks = icl_update_dpll_ref_clks,
	.dump_hw_state = icl_dump_hw_state,
};

static const struct dpll_info ehl_plls[] = {
	{ "DPLL 0", &combo_pll_funcs, DPLL_ID_ICL_DPLL0, 0 },
	{ "DPLL 1", &combo_pll_funcs, DPLL_ID_ICL_DPLL1, 0 },
	{ "DPLL 4", &combo_pll_funcs, DPLL_ID_EHL_DPLL4, 0 },
	{ },
};

static const struct intel_dpll_mgr ehl_pll_mgr = {
	.dpll_info = ehl_plls,
	.get_dplls = icl_get_dplls,
	.put_dplls = icl_put_dplls,
	.update_ref_clks = icl_update_dpll_ref_clks,
	.dump_hw_state = icl_dump_hw_state,
};

static const struct intel_shared_dpll_funcs dkl_pll_funcs = {
	.enable = mg_pll_enable,
	.disable = mg_pll_disable,
	.get_hw_state = dkl_pll_get_hw_state,
	.get_freq = icl_ddi_mg_pll_get_freq,
};

static const struct dpll_info tgl_plls[] = {
	{ "DPLL 0", &combo_pll_funcs, DPLL_ID_ICL_DPLL0,  0 },
	{ "DPLL 1", &combo_pll_funcs, DPLL_ID_ICL_DPLL1,  0 },
	{ "TBT PLL",  &tbt_pll_funcs, DPLL_ID_ICL_TBTPLL, 0 },
	{ "TC PLL 1", &dkl_pll_funcs, DPLL_ID_ICL_MGPLL1, 0 },
	{ "TC PLL 2", &dkl_pll_funcs, DPLL_ID_ICL_MGPLL2, 0 },
	{ "TC PLL 3", &dkl_pll_funcs, DPLL_ID_ICL_MGPLL3, 0 },
	{ "TC PLL 4", &dkl_pll_funcs, DPLL_ID_ICL_MGPLL4, 0 },
	{ "TC PLL 5", &dkl_pll_funcs, DPLL_ID_TGL_MGPLL5, 0 },
	{ "TC PLL 6", &dkl_pll_funcs, DPLL_ID_TGL_MGPLL6, 0 },
	{ },
};

static const struct intel_dpll_mgr tgl_pll_mgr = {
	.dpll_info = tgl_plls,
	.get_dplls = icl_get_dplls,
	.put_dplls = icl_put_dplls,
	.update_active_dpll = icl_update_active_dpll,
	.update_ref_clks = icl_update_dpll_ref_clks,
	.dump_hw_state = icl_dump_hw_state,
};

static const struct dpll_info rkl_plls[] = {
	{ "DPLL 0", &combo_pll_funcs, DPLL_ID_ICL_DPLL0, 0 },
	{ "DPLL 1", &combo_pll_funcs, DPLL_ID_ICL_DPLL1, 0 },
	{ "DPLL 4", &combo_pll_funcs, DPLL_ID_EHL_DPLL4, 0 },
	{ },
};

static const struct intel_dpll_mgr rkl_pll_mgr = {
	.dpll_info = rkl_plls,
	.get_dplls = icl_get_dplls,
	.put_dplls = icl_put_dplls,
	.update_ref_clks = icl_update_dpll_ref_clks,
	.dump_hw_state = icl_dump_hw_state,
};

static const struct dpll_info dg1_plls[] = {
	{ "DPLL 0", &combo_pll_funcs, DPLL_ID_DG1_DPLL0, 0 },
	{ "DPLL 1", &combo_pll_funcs, DPLL_ID_DG1_DPLL1, 0 },
	{ "DPLL 2", &combo_pll_funcs, DPLL_ID_DG1_DPLL2, 0 },
	{ "DPLL 3", &combo_pll_funcs, DPLL_ID_DG1_DPLL3, 0 },
	{ },
};

static const struct intel_dpll_mgr dg1_pll_mgr = {
	.dpll_info = dg1_plls,
	.get_dplls = icl_get_dplls,
	.put_dplls = icl_put_dplls,
	.update_ref_clks = icl_update_dpll_ref_clks,
	.dump_hw_state = icl_dump_hw_state,
};

static const struct dpll_info adls_plls[] = {
	{ "DPLL 0", &combo_pll_funcs, DPLL_ID_ICL_DPLL0, 0 },
	{ "DPLL 1", &combo_pll_funcs, DPLL_ID_ICL_DPLL1, 0 },
	{ "DPLL 2", &combo_pll_funcs, DPLL_ID_DG1_DPLL2, 0 },
	{ "DPLL 3", &combo_pll_funcs, DPLL_ID_DG1_DPLL3, 0 },
	{ },
};

static const struct intel_dpll_mgr adls_pll_mgr = {
	.dpll_info = adls_plls,
	.get_dplls = icl_get_dplls,
	.put_dplls = icl_put_dplls,
	.update_ref_clks = icl_update_dpll_ref_clks,
	.dump_hw_state = icl_dump_hw_state,
};

static const struct dpll_info adlp_plls[] = {
	{ "DPLL 0", &combo_pll_funcs, DPLL_ID_ICL_DPLL0,  0 },
	{ "DPLL 1", &combo_pll_funcs, DPLL_ID_ICL_DPLL1,  0 },
	{ "TBT PLL",  &tbt_pll_funcs, DPLL_ID_ICL_TBTPLL, 0 },
	{ "TC PLL 1", &dkl_pll_funcs, DPLL_ID_ICL_MGPLL1, 0 },
	{ "TC PLL 2", &dkl_pll_funcs, DPLL_ID_ICL_MGPLL2, 0 },
	{ "TC PLL 3", &dkl_pll_funcs, DPLL_ID_ICL_MGPLL3, 0 },
	{ "TC PLL 4", &dkl_pll_funcs, DPLL_ID_ICL_MGPLL4, 0 },
	{ },
};

static const struct intel_dpll_mgr adlp_pll_mgr = {
	.dpll_info = adlp_plls,
	.get_dplls = icl_get_dplls,
	.put_dplls = icl_put_dplls,
	.update_active_dpll = icl_update_active_dpll,
	.update_ref_clks = icl_update_dpll_ref_clks,
	.dump_hw_state = icl_dump_hw_state,
};

/**
 * intel_shared_dpll_init - Initialize shared DPLLs
 * @dev: drm device
 *
 * Initialize shared DPLLs for @dev.
 */
void intel_shared_dpll_init(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = to_i915(dev);
	const struct intel_dpll_mgr *dpll_mgr = NULL;
	const struct dpll_info *dpll_info;
	int i;

	if (IS_ALDERLAKE_P(dev_priv))
		dpll_mgr = &adlp_pll_mgr;
	else if (IS_ALDERLAKE_S(dev_priv))
		dpll_mgr = &adls_pll_mgr;
	else if (IS_DG1(dev_priv))
		dpll_mgr = &dg1_pll_mgr;
	else if (IS_ROCKETLAKE(dev_priv))
		dpll_mgr = &rkl_pll_mgr;
	else if (DISPLAY_VER(dev_priv) >= 12)
		dpll_mgr = &tgl_pll_mgr;
	else if (IS_JSL_EHL(dev_priv))
		dpll_mgr = &ehl_pll_mgr;
	else if (DISPLAY_VER(dev_priv) >= 11)
		dpll_mgr = &icl_pll_mgr;
	else if (IS_CANNONLAKE(dev_priv))
		dpll_mgr = &cnl_pll_mgr;
	else if (IS_GEMINILAKE(dev_priv) || IS_BROXTON(dev_priv))
		dpll_mgr = &bxt_pll_mgr;
	else if (DISPLAY_VER(dev_priv) == 9)
		dpll_mgr = &skl_pll_mgr;
	else if (HAS_DDI(dev_priv))
		dpll_mgr = &hsw_pll_mgr;
	else if (HAS_PCH_IBX(dev_priv) || HAS_PCH_CPT(dev_priv))
		dpll_mgr = &pch_pll_mgr;

	if (!dpll_mgr) {
		dev_priv->dpll.num_shared_dpll = 0;
		return;
	}

	dpll_info = dpll_mgr->dpll_info;

	for (i = 0; dpll_info[i].name; i++) {
		drm_WARN_ON(dev, i != dpll_info[i].id);
		dev_priv->dpll.shared_dplls[i].info = &dpll_info[i];
	}

	dev_priv->dpll.mgr = dpll_mgr;
	dev_priv->dpll.num_shared_dpll = i;
	mutex_init(&dev_priv->dpll.lock);

	BUG_ON(dev_priv->dpll.num_shared_dpll > I915_NUM_PLLS);
}

/**
 * intel_reserve_shared_dplls - reserve DPLLs for CRTC and encoder combination
 * @state: atomic state
 * @crtc: CRTC to reserve DPLLs for
 * @encoder: encoder
 *
 * This function reserves all required DPLLs for the given CRTC and encoder
 * combination in the current atomic commit @state and the new @crtc atomic
 * state.
 *
 * The new configuration in the atomic commit @state is made effective by
 * calling intel_shared_dpll_swap_state().
 *
 * The reserved DPLLs should be released by calling
 * intel_release_shared_dplls().
 *
 * Returns:
 * True if all required DPLLs were successfully reserved.
 */
bool intel_reserve_shared_dplls(struct intel_atomic_state *state,
				struct intel_crtc *crtc,
				struct intel_encoder *encoder)
{
	struct drm_i915_private *dev_priv = to_i915(state->base.dev);
	const struct intel_dpll_mgr *dpll_mgr = dev_priv->dpll.mgr;

	if (drm_WARN_ON(&dev_priv->drm, !dpll_mgr))
		return false;

	return dpll_mgr->get_dplls(state, crtc, encoder);
}

/**
 * intel_release_shared_dplls - end use of DPLLs by CRTC in atomic state
 * @state: atomic state
 * @crtc: crtc from which the DPLLs are to be released
 *
 * This function releases all DPLLs reserved by intel_reserve_shared_dplls()
 * from the current atomic commit @state and the old @crtc atomic state.
 *
 * The new configuration in the atomic commit @state is made effective by
 * calling intel_shared_dpll_swap_state().
 */
void intel_release_shared_dplls(struct intel_atomic_state *state,
				struct intel_crtc *crtc)
{
	struct drm_i915_private *dev_priv = to_i915(state->base.dev);
	const struct intel_dpll_mgr *dpll_mgr = dev_priv->dpll.mgr;

	/*
	 * FIXME: this function is called for every platform having a
	 * compute_clock hook, even though the platform doesn't yet support
	 * the shared DPLL framework and intel_reserve_shared_dplls() is not
	 * called on those.
	 */
	if (!dpll_mgr)
		return;

	dpll_mgr->put_dplls(state, crtc);
}

/**
 * intel_update_active_dpll - update the active DPLL for a CRTC/encoder
 * @state: atomic state
 * @crtc: the CRTC for which to update the active DPLL
 * @encoder: encoder determining the type of port DPLL
 *
 * Update the active DPLL for the given @crtc/@encoder in @crtc's atomic state,
 * from the port DPLLs reserved previously by intel_reserve_shared_dplls(). The
 * DPLL selected will be based on the current mode of the encoder's port.
 */
void intel_update_active_dpll(struct intel_atomic_state *state,
			      struct intel_crtc *crtc,
			      struct intel_encoder *encoder)
{
	struct drm_i915_private *dev_priv = to_i915(encoder->base.dev);
	const struct intel_dpll_mgr *dpll_mgr = dev_priv->dpll.mgr;

	if (drm_WARN_ON(&dev_priv->drm, !dpll_mgr))
		return;

	dpll_mgr->update_active_dpll(state, crtc, encoder);
}

/**
 * intel_dpll_get_freq - calculate the DPLL's output frequency
 * @i915: i915 device
 * @pll: DPLL for which to calculate the output frequency
 * @pll_state: DPLL state from which to calculate the output frequency
 *
 * Return the output frequency corresponding to @pll's passed in @pll_state.
 */
int intel_dpll_get_freq(struct drm_i915_private *i915,
			const struct intel_shared_dpll *pll,
			const struct intel_dpll_hw_state *pll_state)
{
	if (drm_WARN_ON(&i915->drm, !pll->info->funcs->get_freq))
		return 0;

	return pll->info->funcs->get_freq(i915, pll, pll_state);
}

/**
 * intel_dpll_get_hw_state - readout the DPLL's hardware state
 * @i915: i915 device
 * @pll: DPLL for which to calculate the output frequency
 * @hw_state: DPLL's hardware state
 *
 * Read out @pll's hardware state into @hw_state.
 */
bool intel_dpll_get_hw_state(struct drm_i915_private *i915,
			     struct intel_shared_dpll *pll,
			     struct intel_dpll_hw_state *hw_state)
{
	return pll->info->funcs->get_hw_state(i915, pll, hw_state);
}

static void readout_dpll_hw_state(struct drm_i915_private *i915,
				  struct intel_shared_dpll *pll)
{
	struct intel_crtc *crtc;

	pll->on = intel_dpll_get_hw_state(i915, pll, &pll->state.hw_state);

	if (IS_JSL_EHL(i915) && pll->on &&
	    pll->info->id == DPLL_ID_EHL_DPLL4) {
		pll->wakeref = intel_display_power_get(i915,
						       POWER_DOMAIN_DPLL_DC_OFF);
	}

	pll->state.pipe_mask = 0;
	for_each_intel_crtc(&i915->drm, crtc) {
		struct intel_crtc_state *crtc_state =
			to_intel_crtc_state(crtc->base.state);

		if (crtc_state->hw.active && crtc_state->shared_dpll == pll)
			pll->state.pipe_mask |= BIT(crtc->pipe);
	}
	pll->active_mask = pll->state.pipe_mask;

	drm_dbg_kms(&i915->drm,
		    "%s hw state readout: pipe_mask 0x%x, on %i\n",
		    pll->info->name, pll->state.pipe_mask, pll->on);
}

void intel_dpll_update_ref_clks(struct drm_i915_private *i915)
{
	if (i915->dpll.mgr && i915->dpll.mgr->update_ref_clks)
		i915->dpll.mgr->update_ref_clks(i915);
}

void intel_dpll_readout_hw_state(struct drm_i915_private *i915)
{
	int i;

	for (i = 0; i < i915->dpll.num_shared_dpll; i++)
		readout_dpll_hw_state(i915, &i915->dpll.shared_dplls[i]);
}

static void sanitize_dpll_state(struct drm_i915_private *i915,
				struct intel_shared_dpll *pll)
{
	if (!pll->on || pll->active_mask)
		return;

	drm_dbg_kms(&i915->drm,
		    "%s enabled but not in use, disabling\n",
		    pll->info->name);

	pll->info->funcs->disable(i915, pll);
	pll->on = false;
}

void intel_dpll_sanitize_state(struct drm_i915_private *i915)
{
	int i;

	for (i = 0; i < i915->dpll.num_shared_dpll; i++)
		sanitize_dpll_state(i915, &i915->dpll.shared_dplls[i]);
}

/**
 * intel_dpll_dump_hw_state - write hw_state to dmesg
 * @dev_priv: i915 drm device
 * @hw_state: hw state to be written to the log
 *
 * Write the relevant values in @hw_state to dmesg using drm_dbg_kms.
 */
void intel_dpll_dump_hw_state(struct drm_i915_private *dev_priv,
			      const struct intel_dpll_hw_state *hw_state)
{
	if (dev_priv->dpll.mgr) {
		dev_priv->dpll.mgr->dump_hw_state(dev_priv, hw_state);
	} else {
		/* fallback for platforms that don't use the shared dpll
		 * infrastructure
		 */
		drm_dbg_kms(&dev_priv->drm,
			    "dpll_hw_state: dpll: 0x%x, dpll_md: 0x%x, "
			    "fp0: 0x%x, fp1: 0x%x\n",
			    hw_state->dpll,
			    hw_state->dpll_md,
			    hw_state->fp0,
			    hw_state->fp1);
	}
}
