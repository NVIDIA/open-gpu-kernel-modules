/*
 * Copyright © 2006-2007 Intel Corporation
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
 *
 * Authors:
 *	Eric Anholt <eric@anholt.net>
 */

#include <acpi/video.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/intel-iommu.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/dma-resv.h>
#include <linux/slab.h>

#include <drm/drm_atomic.h>
#include <drm/drm_atomic_helper.h>
#include <drm/drm_atomic_uapi.h>
#include <drm/drm_damage_helper.h>
#include <drm/drm_dp_helper.h>
#include <drm/drm_edid.h>
#include <drm/drm_fourcc.h>
#include <drm/drm_plane_helper.h>
#include <drm/drm_probe_helper.h>
#include <drm/drm_rect.h>

#include "display/intel_audio.h"
#include "display/intel_crt.h"
#include "display/intel_ddi.h"
#include "display/intel_display_debugfs.h"
#include "display/intel_dp.h"
#include "display/intel_dp_mst.h"
#include "display/intel_dpll.h"
#include "display/intel_dpll_mgr.h"
#include "display/intel_dsi.h"
#include "display/intel_dvo.h"
#include "display/intel_fb.h"
#include "display/intel_gmbus.h"
#include "display/intel_hdmi.h"
#include "display/intel_lvds.h"
#include "display/intel_sdvo.h"
#include "display/intel_tv.h"
#include "display/intel_vdsc.h"
#include "display/intel_vrr.h"

#include "gem/i915_gem_lmem.h"
#include "gem/i915_gem_object.h"

#include "gt/intel_rps.h"
#include "gt/gen8_ppgtt.h"

#include "g4x_dp.h"
#include "g4x_hdmi.h"
#include "i915_drv.h"
#include "intel_acpi.h"
#include "intel_atomic.h"
#include "intel_atomic_plane.h"
#include "intel_bw.h"
#include "intel_cdclk.h"
#include "intel_color.h"
#include "intel_crtc.h"
#include "intel_de.h"
#include "intel_display_types.h"
#include "intel_dmc.h"
#include "intel_dp_link_training.h"
#include "intel_fbc.h"
#include "intel_fdi.h"
#include "intel_fbdev.h"
#include "intel_fifo_underrun.h"
#include "intel_frontbuffer.h"
#include "intel_hdcp.h"
#include "intel_hotplug.h"
#include "intel_overlay.h"
#include "intel_pipe_crc.h"
#include "intel_pm.h"
#include "intel_pps.h"
#include "intel_psr.h"
#include "intel_quirks.h"
#include "intel_sideband.h"
#include "intel_sprite.h"
#include "intel_tc.h"
#include "intel_vga.h"
#include "i9xx_plane.h"
#include "skl_scaler.h"
#include "skl_universal_plane.h"

static void i9xx_crtc_clock_get(struct intel_crtc *crtc,
				struct intel_crtc_state *pipe_config);
static void ilk_pch_clock_get(struct intel_crtc *crtc,
			      struct intel_crtc_state *pipe_config);

static int intel_framebuffer_init(struct intel_framebuffer *ifb,
				  struct drm_i915_gem_object *obj,
				  struct drm_mode_fb_cmd2 *mode_cmd);
static void intel_set_transcoder_timings(const struct intel_crtc_state *crtc_state);
static void intel_set_pipe_src_size(const struct intel_crtc_state *crtc_state);
static void intel_cpu_transcoder_set_m_n(const struct intel_crtc_state *crtc_state,
					 const struct intel_link_m_n *m_n,
					 const struct intel_link_m_n *m2_n2);
static void i9xx_set_pipeconf(const struct intel_crtc_state *crtc_state);
static void ilk_set_pipeconf(const struct intel_crtc_state *crtc_state);
static void hsw_set_pipeconf(const struct intel_crtc_state *crtc_state);
static void bdw_set_pipemisc(const struct intel_crtc_state *crtc_state);
static void ilk_pfit_enable(const struct intel_crtc_state *crtc_state);
static void intel_modeset_setup_hw_state(struct drm_device *dev,
					 struct drm_modeset_acquire_ctx *ctx);

struct i915_dpt {
	struct i915_address_space vm;

	struct drm_i915_gem_object *obj;
	struct i915_vma *vma;
	void __iomem *iomem;
};

#define i915_is_dpt(vm) ((vm)->is_dpt)

static inline struct i915_dpt *
i915_vm_to_dpt(struct i915_address_space *vm)
{
	BUILD_BUG_ON(offsetof(struct i915_dpt, vm));
	GEM_BUG_ON(!i915_is_dpt(vm));
	return container_of(vm, struct i915_dpt, vm);
}

#define dpt_total_entries(dpt) ((dpt)->vm.total >> PAGE_SHIFT)

static void gen8_set_pte(void __iomem *addr, gen8_pte_t pte)
{
	writeq(pte, addr);
}

static void dpt_insert_page(struct i915_address_space *vm,
			    dma_addr_t addr,
			    u64 offset,
			    enum i915_cache_level level,
			    u32 flags)
{
	struct i915_dpt *dpt = i915_vm_to_dpt(vm);
	gen8_pte_t __iomem *base = dpt->iomem;

	gen8_set_pte(base + offset / I915_GTT_PAGE_SIZE,
		     vm->pte_encode(addr, level, flags));
}

static void dpt_insert_entries(struct i915_address_space *vm,
			       struct i915_vma *vma,
			       enum i915_cache_level level,
			       u32 flags)
{
	struct i915_dpt *dpt = i915_vm_to_dpt(vm);
	gen8_pte_t __iomem *base = dpt->iomem;
	const gen8_pte_t pte_encode = vm->pte_encode(0, level, flags);
	struct sgt_iter sgt_iter;
	dma_addr_t addr;
	int i;

	/*
	 * Note that we ignore PTE_READ_ONLY here. The caller must be careful
	 * not to allow the user to override access to a read only page.
	 */

	i = vma->node.start / I915_GTT_PAGE_SIZE;
	for_each_sgt_daddr(addr, sgt_iter, vma->pages)
		gen8_set_pte(&base[i++], pte_encode | addr);
}

static void dpt_clear_range(struct i915_address_space *vm,
			    u64 start, u64 length)
{
}

static void dpt_bind_vma(struct i915_address_space *vm,
			 struct i915_vm_pt_stash *stash,
			 struct i915_vma *vma,
			 enum i915_cache_level cache_level,
			 u32 flags)
{
	struct drm_i915_gem_object *obj = vma->obj;
	u32 pte_flags;

	/* Applicable to VLV (gen8+ do not support RO in the GGTT) */
	pte_flags = 0;
	if (vma->vm->has_read_only && i915_gem_object_is_readonly(obj))
		pte_flags |= PTE_READ_ONLY;
	if (i915_gem_object_is_lmem(obj))
		pte_flags |= PTE_LM;

	vma->vm->insert_entries(vma->vm, vma, cache_level, pte_flags);

	vma->page_sizes.gtt = I915_GTT_PAGE_SIZE;

	/*
	 * Without aliasing PPGTT there's no difference between
	 * GLOBAL/LOCAL_BIND, it's all the same ptes. Hence unconditionally
	 * upgrade to both bound if we bind either to avoid double-binding.
	 */
	atomic_or(I915_VMA_GLOBAL_BIND | I915_VMA_LOCAL_BIND, &vma->flags);
}

static void dpt_unbind_vma(struct i915_address_space *vm, struct i915_vma *vma)
{
	vm->clear_range(vm, vma->node.start, vma->size);
}

static void dpt_cleanup(struct i915_address_space *vm)
{
	struct i915_dpt *dpt = i915_vm_to_dpt(vm);

	i915_gem_object_put(dpt->obj);
}

static struct i915_address_space *
intel_dpt_create(struct intel_framebuffer *fb)
{
	struct drm_gem_object *obj = &intel_fb_obj(&fb->base)->base;
	struct drm_i915_private *i915 = to_i915(obj->dev);
	struct drm_i915_gem_object *dpt_obj;
	struct i915_address_space *vm;
	struct i915_dpt *dpt;
	size_t size;
	int ret;

	if (intel_fb_needs_pot_stride_remap(fb))
		size = intel_remapped_info_size(&fb->remapped_view.gtt.remapped);
	else
		size = DIV_ROUND_UP_ULL(obj->size, I915_GTT_PAGE_SIZE);

	size = round_up(size * sizeof(gen8_pte_t), I915_GTT_PAGE_SIZE);

	if (HAS_LMEM(i915))
		dpt_obj = i915_gem_object_create_lmem(i915, size, 0);
	else
		dpt_obj = i915_gem_object_create_stolen(i915, size);
	if (IS_ERR(dpt_obj))
		return ERR_CAST(dpt_obj);

	ret = i915_gem_object_set_cache_level(dpt_obj, I915_CACHE_NONE);
	if (ret) {
		i915_gem_object_put(dpt_obj);
		return ERR_PTR(ret);
	}

	dpt = kzalloc(sizeof(*dpt), GFP_KERNEL);
	if (!dpt) {
		i915_gem_object_put(dpt_obj);
		return ERR_PTR(-ENOMEM);
	}

	vm = &dpt->vm;

	vm->gt = &i915->gt;
	vm->i915 = i915;
	vm->dma = i915->drm.dev;
	vm->total = (size / sizeof(gen8_pte_t)) * I915_GTT_PAGE_SIZE;
	vm->is_dpt = true;

	i915_address_space_init(vm, VM_CLASS_DPT);

	vm->insert_page = dpt_insert_page;
	vm->clear_range = dpt_clear_range;
	vm->insert_entries = dpt_insert_entries;
	vm->cleanup = dpt_cleanup;

	vm->vma_ops.bind_vma    = dpt_bind_vma;
	vm->vma_ops.unbind_vma  = dpt_unbind_vma;
	vm->vma_ops.set_pages   = ggtt_set_pages;
	vm->vma_ops.clear_pages = clear_pages;

	vm->pte_encode = gen8_ggtt_pte_encode;

	dpt->obj = dpt_obj;

	return &dpt->vm;
}

static void intel_dpt_destroy(struct i915_address_space *vm)
{
	struct i915_dpt *dpt = i915_vm_to_dpt(vm);

	i915_vm_close(&dpt->vm);
}

/* returns HPLL frequency in kHz */
int vlv_get_hpll_vco(struct drm_i915_private *dev_priv)
{
	int hpll_freq, vco_freq[] = { 800, 1600, 2000, 2400 };

	/* Obtain SKU information */
	hpll_freq = vlv_cck_read(dev_priv, CCK_FUSE_REG) &
		CCK_FUSE_HPLL_FREQ_MASK;

	return vco_freq[hpll_freq] * 1000;
}

int vlv_get_cck_clock(struct drm_i915_private *dev_priv,
		      const char *name, u32 reg, int ref_freq)
{
	u32 val;
	int divider;

	val = vlv_cck_read(dev_priv, reg);
	divider = val & CCK_FREQUENCY_VALUES;

	drm_WARN(&dev_priv->drm, (val & CCK_FREQUENCY_STATUS) !=
		 (divider << CCK_FREQUENCY_STATUS_SHIFT),
		 "%s change in progress\n", name);

	return DIV_ROUND_CLOSEST(ref_freq << 1, divider + 1);
}

int vlv_get_cck_clock_hpll(struct drm_i915_private *dev_priv,
			   const char *name, u32 reg)
{
	int hpll;

	vlv_cck_get(dev_priv);

	if (dev_priv->hpll_freq == 0)
		dev_priv->hpll_freq = vlv_get_hpll_vco(dev_priv);

	hpll = vlv_get_cck_clock(dev_priv, name, reg, dev_priv->hpll_freq);

	vlv_cck_put(dev_priv);

	return hpll;
}

static void intel_update_czclk(struct drm_i915_private *dev_priv)
{
	if (!(IS_VALLEYVIEW(dev_priv) || IS_CHERRYVIEW(dev_priv)))
		return;

	dev_priv->czclk_freq = vlv_get_cck_clock_hpll(dev_priv, "czclk",
						      CCK_CZ_CLOCK_CONTROL);

	drm_dbg(&dev_priv->drm, "CZ clock rate: %d kHz\n",
		dev_priv->czclk_freq);
}

/* WA Display #0827: Gen9:all */
static void
skl_wa_827(struct drm_i915_private *dev_priv, enum pipe pipe, bool enable)
{
	if (enable)
		intel_de_write(dev_priv, CLKGATE_DIS_PSL(pipe),
		               intel_de_read(dev_priv, CLKGATE_DIS_PSL(pipe)) | DUPS1_GATING_DIS | DUPS2_GATING_DIS);
	else
		intel_de_write(dev_priv, CLKGATE_DIS_PSL(pipe),
		               intel_de_read(dev_priv, CLKGATE_DIS_PSL(pipe)) & ~(DUPS1_GATING_DIS | DUPS2_GATING_DIS));
}

/* Wa_2006604312:icl,ehl */
static void
icl_wa_scalerclkgating(struct drm_i915_private *dev_priv, enum pipe pipe,
		       bool enable)
{
	if (enable)
		intel_de_write(dev_priv, CLKGATE_DIS_PSL(pipe),
		               intel_de_read(dev_priv, CLKGATE_DIS_PSL(pipe)) | DPFR_GATING_DIS);
	else
		intel_de_write(dev_priv, CLKGATE_DIS_PSL(pipe),
		               intel_de_read(dev_priv, CLKGATE_DIS_PSL(pipe)) & ~DPFR_GATING_DIS);
}

static bool
is_trans_port_sync_slave(const struct intel_crtc_state *crtc_state)
{
	return crtc_state->master_transcoder != INVALID_TRANSCODER;
}

static bool
is_trans_port_sync_master(const struct intel_crtc_state *crtc_state)
{
	return crtc_state->sync_mode_slaves_mask != 0;
}

bool
is_trans_port_sync_mode(const struct intel_crtc_state *crtc_state)
{
	return is_trans_port_sync_master(crtc_state) ||
		is_trans_port_sync_slave(crtc_state);
}

static bool pipe_scanline_is_moving(struct drm_i915_private *dev_priv,
				    enum pipe pipe)
{
	i915_reg_t reg = PIPEDSL(pipe);
	u32 line1, line2;
	u32 line_mask;

	if (DISPLAY_VER(dev_priv) == 2)
		line_mask = DSL_LINEMASK_GEN2;
	else
		line_mask = DSL_LINEMASK_GEN3;

	line1 = intel_de_read(dev_priv, reg) & line_mask;
	msleep(5);
	line2 = intel_de_read(dev_priv, reg) & line_mask;

	return line1 != line2;
}

static void wait_for_pipe_scanline_moving(struct intel_crtc *crtc, bool state)
{
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	enum pipe pipe = crtc->pipe;

	/* Wait for the display line to settle/start moving */
	if (wait_for(pipe_scanline_is_moving(dev_priv, pipe) == state, 100))
		drm_err(&dev_priv->drm,
			"pipe %c scanline %s wait timed out\n",
			pipe_name(pipe), onoff(state));
}

static void intel_wait_for_pipe_scanline_stopped(struct intel_crtc *crtc)
{
	wait_for_pipe_scanline_moving(crtc, false);
}

static void intel_wait_for_pipe_scanline_moving(struct intel_crtc *crtc)
{
	wait_for_pipe_scanline_moving(crtc, true);
}

static void
intel_wait_for_pipe_off(const struct intel_crtc_state *old_crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(old_crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);

	if (DISPLAY_VER(dev_priv) >= 4) {
		enum transcoder cpu_transcoder = old_crtc_state->cpu_transcoder;
		i915_reg_t reg = PIPECONF(cpu_transcoder);

		/* Wait for the Pipe State to go off */
		if (intel_de_wait_for_clear(dev_priv, reg,
					    I965_PIPECONF_ACTIVE, 100))
			drm_WARN(&dev_priv->drm, 1,
				 "pipe_off wait timed out\n");
	} else {
		intel_wait_for_pipe_scanline_stopped(crtc);
	}
}

/* Only for pre-ILK configs */
void assert_pll(struct drm_i915_private *dev_priv,
		enum pipe pipe, bool state)
{
	u32 val;
	bool cur_state;

	val = intel_de_read(dev_priv, DPLL(pipe));
	cur_state = !!(val & DPLL_VCO_ENABLE);
	I915_STATE_WARN(cur_state != state,
	     "PLL state assertion failure (expected %s, current %s)\n",
			onoff(state), onoff(cur_state));
}

/* XXX: the dsi pll is shared between MIPI DSI ports */
void assert_dsi_pll(struct drm_i915_private *dev_priv, bool state)
{
	u32 val;
	bool cur_state;

	vlv_cck_get(dev_priv);
	val = vlv_cck_read(dev_priv, CCK_REG_DSI_PLL_CONTROL);
	vlv_cck_put(dev_priv);

	cur_state = val & DSI_PLL_VCO_EN;
	I915_STATE_WARN(cur_state != state,
	     "DSI PLL state assertion failure (expected %s, current %s)\n",
			onoff(state), onoff(cur_state));
}

static void assert_fdi_tx(struct drm_i915_private *dev_priv,
			  enum pipe pipe, bool state)
{
	bool cur_state;

	if (HAS_DDI(dev_priv)) {
		/*
		 * DDI does not have a specific FDI_TX register.
		 *
		 * FDI is never fed from EDP transcoder
		 * so pipe->transcoder cast is fine here.
		 */
		enum transcoder cpu_transcoder = (enum transcoder)pipe;
		u32 val = intel_de_read(dev_priv,
					TRANS_DDI_FUNC_CTL(cpu_transcoder));
		cur_state = !!(val & TRANS_DDI_FUNC_ENABLE);
	} else {
		u32 val = intel_de_read(dev_priv, FDI_TX_CTL(pipe));
		cur_state = !!(val & FDI_TX_ENABLE);
	}
	I915_STATE_WARN(cur_state != state,
	     "FDI TX state assertion failure (expected %s, current %s)\n",
			onoff(state), onoff(cur_state));
}
#define assert_fdi_tx_enabled(d, p) assert_fdi_tx(d, p, true)
#define assert_fdi_tx_disabled(d, p) assert_fdi_tx(d, p, false)

static void assert_fdi_rx(struct drm_i915_private *dev_priv,
			  enum pipe pipe, bool state)
{
	u32 val;
	bool cur_state;

	val = intel_de_read(dev_priv, FDI_RX_CTL(pipe));
	cur_state = !!(val & FDI_RX_ENABLE);
	I915_STATE_WARN(cur_state != state,
	     "FDI RX state assertion failure (expected %s, current %s)\n",
			onoff(state), onoff(cur_state));
}
#define assert_fdi_rx_enabled(d, p) assert_fdi_rx(d, p, true)
#define assert_fdi_rx_disabled(d, p) assert_fdi_rx(d, p, false)

static void assert_fdi_tx_pll_enabled(struct drm_i915_private *dev_priv,
				      enum pipe pipe)
{
	u32 val;

	/* ILK FDI PLL is always enabled */
	if (IS_IRONLAKE(dev_priv))
		return;

	/* On Haswell, DDI ports are responsible for the FDI PLL setup */
	if (HAS_DDI(dev_priv))
		return;

	val = intel_de_read(dev_priv, FDI_TX_CTL(pipe));
	I915_STATE_WARN(!(val & FDI_TX_PLL_ENABLE), "FDI TX PLL assertion failure, should be active but is disabled\n");
}

void assert_fdi_rx_pll(struct drm_i915_private *dev_priv,
		       enum pipe pipe, bool state)
{
	u32 val;
	bool cur_state;

	val = intel_de_read(dev_priv, FDI_RX_CTL(pipe));
	cur_state = !!(val & FDI_RX_PLL_ENABLE);
	I915_STATE_WARN(cur_state != state,
	     "FDI RX PLL assertion failure (expected %s, current %s)\n",
			onoff(state), onoff(cur_state));
}

void assert_panel_unlocked(struct drm_i915_private *dev_priv, enum pipe pipe)
{
	i915_reg_t pp_reg;
	u32 val;
	enum pipe panel_pipe = INVALID_PIPE;
	bool locked = true;

	if (drm_WARN_ON(&dev_priv->drm, HAS_DDI(dev_priv)))
		return;

	if (HAS_PCH_SPLIT(dev_priv)) {
		u32 port_sel;

		pp_reg = PP_CONTROL(0);
		port_sel = intel_de_read(dev_priv, PP_ON_DELAYS(0)) & PANEL_PORT_SELECT_MASK;

		switch (port_sel) {
		case PANEL_PORT_SELECT_LVDS:
			intel_lvds_port_enabled(dev_priv, PCH_LVDS, &panel_pipe);
			break;
		case PANEL_PORT_SELECT_DPA:
			g4x_dp_port_enabled(dev_priv, DP_A, PORT_A, &panel_pipe);
			break;
		case PANEL_PORT_SELECT_DPC:
			g4x_dp_port_enabled(dev_priv, PCH_DP_C, PORT_C, &panel_pipe);
			break;
		case PANEL_PORT_SELECT_DPD:
			g4x_dp_port_enabled(dev_priv, PCH_DP_D, PORT_D, &panel_pipe);
			break;
		default:
			MISSING_CASE(port_sel);
			break;
		}
	} else if (IS_VALLEYVIEW(dev_priv) || IS_CHERRYVIEW(dev_priv)) {
		/* presumably write lock depends on pipe, not port select */
		pp_reg = PP_CONTROL(pipe);
		panel_pipe = pipe;
	} else {
		u32 port_sel;

		pp_reg = PP_CONTROL(0);
		port_sel = intel_de_read(dev_priv, PP_ON_DELAYS(0)) & PANEL_PORT_SELECT_MASK;

		drm_WARN_ON(&dev_priv->drm,
			    port_sel != PANEL_PORT_SELECT_LVDS);
		intel_lvds_port_enabled(dev_priv, LVDS, &panel_pipe);
	}

	val = intel_de_read(dev_priv, pp_reg);
	if (!(val & PANEL_POWER_ON) ||
	    ((val & PANEL_UNLOCK_MASK) == PANEL_UNLOCK_REGS))
		locked = false;

	I915_STATE_WARN(panel_pipe == pipe && locked,
	     "panel assertion failure, pipe %c regs locked\n",
	     pipe_name(pipe));
}

void assert_pipe(struct drm_i915_private *dev_priv,
		 enum transcoder cpu_transcoder, bool state)
{
	bool cur_state;
	enum intel_display_power_domain power_domain;
	intel_wakeref_t wakeref;

	/* we keep both pipes enabled on 830 */
	if (IS_I830(dev_priv))
		state = true;

	power_domain = POWER_DOMAIN_TRANSCODER(cpu_transcoder);
	wakeref = intel_display_power_get_if_enabled(dev_priv, power_domain);
	if (wakeref) {
		u32 val = intel_de_read(dev_priv, PIPECONF(cpu_transcoder));
		cur_state = !!(val & PIPECONF_ENABLE);

		intel_display_power_put(dev_priv, power_domain, wakeref);
	} else {
		cur_state = false;
	}

	I915_STATE_WARN(cur_state != state,
			"transcoder %s assertion failure (expected %s, current %s)\n",
			transcoder_name(cpu_transcoder),
			onoff(state), onoff(cur_state));
}

static void assert_plane(struct intel_plane *plane, bool state)
{
	enum pipe pipe;
	bool cur_state;

	cur_state = plane->get_hw_state(plane, &pipe);

	I915_STATE_WARN(cur_state != state,
			"%s assertion failure (expected %s, current %s)\n",
			plane->base.name, onoff(state), onoff(cur_state));
}

#define assert_plane_enabled(p) assert_plane(p, true)
#define assert_plane_disabled(p) assert_plane(p, false)

static void assert_planes_disabled(struct intel_crtc *crtc)
{
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	struct intel_plane *plane;

	for_each_intel_plane_on_crtc(&dev_priv->drm, crtc, plane)
		assert_plane_disabled(plane);
}

void assert_pch_transcoder_disabled(struct drm_i915_private *dev_priv,
				    enum pipe pipe)
{
	u32 val;
	bool enabled;

	val = intel_de_read(dev_priv, PCH_TRANSCONF(pipe));
	enabled = !!(val & TRANS_ENABLE);
	I915_STATE_WARN(enabled,
	     "transcoder assertion failed, should be off on pipe %c but is still active\n",
	     pipe_name(pipe));
}

static void assert_pch_dp_disabled(struct drm_i915_private *dev_priv,
				   enum pipe pipe, enum port port,
				   i915_reg_t dp_reg)
{
	enum pipe port_pipe;
	bool state;

	state = g4x_dp_port_enabled(dev_priv, dp_reg, port, &port_pipe);

	I915_STATE_WARN(state && port_pipe == pipe,
			"PCH DP %c enabled on transcoder %c, should be disabled\n",
			port_name(port), pipe_name(pipe));

	I915_STATE_WARN(HAS_PCH_IBX(dev_priv) && !state && port_pipe == PIPE_B,
			"IBX PCH DP %c still using transcoder B\n",
			port_name(port));
}

static void assert_pch_hdmi_disabled(struct drm_i915_private *dev_priv,
				     enum pipe pipe, enum port port,
				     i915_reg_t hdmi_reg)
{
	enum pipe port_pipe;
	bool state;

	state = intel_sdvo_port_enabled(dev_priv, hdmi_reg, &port_pipe);

	I915_STATE_WARN(state && port_pipe == pipe,
			"PCH HDMI %c enabled on transcoder %c, should be disabled\n",
			port_name(port), pipe_name(pipe));

	I915_STATE_WARN(HAS_PCH_IBX(dev_priv) && !state && port_pipe == PIPE_B,
			"IBX PCH HDMI %c still using transcoder B\n",
			port_name(port));
}

static void assert_pch_ports_disabled(struct drm_i915_private *dev_priv,
				      enum pipe pipe)
{
	enum pipe port_pipe;

	assert_pch_dp_disabled(dev_priv, pipe, PORT_B, PCH_DP_B);
	assert_pch_dp_disabled(dev_priv, pipe, PORT_C, PCH_DP_C);
	assert_pch_dp_disabled(dev_priv, pipe, PORT_D, PCH_DP_D);

	I915_STATE_WARN(intel_crt_port_enabled(dev_priv, PCH_ADPA, &port_pipe) &&
			port_pipe == pipe,
			"PCH VGA enabled on transcoder %c, should be disabled\n",
			pipe_name(pipe));

	I915_STATE_WARN(intel_lvds_port_enabled(dev_priv, PCH_LVDS, &port_pipe) &&
			port_pipe == pipe,
			"PCH LVDS enabled on transcoder %c, should be disabled\n",
			pipe_name(pipe));

	/* PCH SDVOB multiplex with HDMIB */
	assert_pch_hdmi_disabled(dev_priv, pipe, PORT_B, PCH_HDMIB);
	assert_pch_hdmi_disabled(dev_priv, pipe, PORT_C, PCH_HDMIC);
	assert_pch_hdmi_disabled(dev_priv, pipe, PORT_D, PCH_HDMID);
}

void vlv_wait_port_ready(struct drm_i915_private *dev_priv,
			 struct intel_digital_port *dig_port,
			 unsigned int expected_mask)
{
	u32 port_mask;
	i915_reg_t dpll_reg;

	switch (dig_port->base.port) {
	case PORT_B:
		port_mask = DPLL_PORTB_READY_MASK;
		dpll_reg = DPLL(0);
		break;
	case PORT_C:
		port_mask = DPLL_PORTC_READY_MASK;
		dpll_reg = DPLL(0);
		expected_mask <<= 4;
		break;
	case PORT_D:
		port_mask = DPLL_PORTD_READY_MASK;
		dpll_reg = DPIO_PHY_STATUS;
		break;
	default:
		BUG();
	}

	if (intel_de_wait_for_register(dev_priv, dpll_reg,
				       port_mask, expected_mask, 1000))
		drm_WARN(&dev_priv->drm, 1,
			 "timed out waiting for [ENCODER:%d:%s] port ready: got 0x%x, expected 0x%x\n",
			 dig_port->base.base.base.id, dig_port->base.base.name,
			 intel_de_read(dev_priv, dpll_reg) & port_mask,
			 expected_mask);
}

static void ilk_enable_pch_transcoder(const struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	enum pipe pipe = crtc->pipe;
	i915_reg_t reg;
	u32 val, pipeconf_val;

	/* Make sure PCH DPLL is enabled */
	assert_shared_dpll_enabled(dev_priv, crtc_state->shared_dpll);

	/* FDI must be feeding us bits for PCH ports */
	assert_fdi_tx_enabled(dev_priv, pipe);
	assert_fdi_rx_enabled(dev_priv, pipe);

	if (HAS_PCH_CPT(dev_priv)) {
		reg = TRANS_CHICKEN2(pipe);
		val = intel_de_read(dev_priv, reg);
		/*
		 * Workaround: Set the timing override bit
		 * before enabling the pch transcoder.
		 */
		val |= TRANS_CHICKEN2_TIMING_OVERRIDE;
		/* Configure frame start delay to match the CPU */
		val &= ~TRANS_CHICKEN2_FRAME_START_DELAY_MASK;
		val |= TRANS_CHICKEN2_FRAME_START_DELAY(dev_priv->framestart_delay - 1);
		intel_de_write(dev_priv, reg, val);
	}

	reg = PCH_TRANSCONF(pipe);
	val = intel_de_read(dev_priv, reg);
	pipeconf_val = intel_de_read(dev_priv, PIPECONF(pipe));

	if (HAS_PCH_IBX(dev_priv)) {
		/* Configure frame start delay to match the CPU */
		val &= ~TRANS_FRAME_START_DELAY_MASK;
		val |= TRANS_FRAME_START_DELAY(dev_priv->framestart_delay - 1);

		/*
		 * Make the BPC in transcoder be consistent with
		 * that in pipeconf reg. For HDMI we must use 8bpc
		 * here for both 8bpc and 12bpc.
		 */
		val &= ~PIPECONF_BPC_MASK;
		if (intel_crtc_has_type(crtc_state, INTEL_OUTPUT_HDMI))
			val |= PIPECONF_8BPC;
		else
			val |= pipeconf_val & PIPECONF_BPC_MASK;
	}

	val &= ~TRANS_INTERLACE_MASK;
	if ((pipeconf_val & PIPECONF_INTERLACE_MASK) == PIPECONF_INTERLACED_ILK) {
		if (HAS_PCH_IBX(dev_priv) &&
		    intel_crtc_has_type(crtc_state, INTEL_OUTPUT_SDVO))
			val |= TRANS_LEGACY_INTERLACED_ILK;
		else
			val |= TRANS_INTERLACED;
	} else {
		val |= TRANS_PROGRESSIVE;
	}

	intel_de_write(dev_priv, reg, val | TRANS_ENABLE);
	if (intel_de_wait_for_set(dev_priv, reg, TRANS_STATE_ENABLE, 100))
		drm_err(&dev_priv->drm, "failed to enable transcoder %c\n",
			pipe_name(pipe));
}

static void lpt_enable_pch_transcoder(struct drm_i915_private *dev_priv,
				      enum transcoder cpu_transcoder)
{
	u32 val, pipeconf_val;

	/* FDI must be feeding us bits for PCH ports */
	assert_fdi_tx_enabled(dev_priv, (enum pipe) cpu_transcoder);
	assert_fdi_rx_enabled(dev_priv, PIPE_A);

	val = intel_de_read(dev_priv, TRANS_CHICKEN2(PIPE_A));
	/* Workaround: set timing override bit. */
	val |= TRANS_CHICKEN2_TIMING_OVERRIDE;
	/* Configure frame start delay to match the CPU */
	val &= ~TRANS_CHICKEN2_FRAME_START_DELAY_MASK;
	val |= TRANS_CHICKEN2_FRAME_START_DELAY(dev_priv->framestart_delay - 1);
	intel_de_write(dev_priv, TRANS_CHICKEN2(PIPE_A), val);

	val = TRANS_ENABLE;
	pipeconf_val = intel_de_read(dev_priv, PIPECONF(cpu_transcoder));

	if ((pipeconf_val & PIPECONF_INTERLACE_MASK_HSW) ==
	    PIPECONF_INTERLACED_ILK)
		val |= TRANS_INTERLACED;
	else
		val |= TRANS_PROGRESSIVE;

	intel_de_write(dev_priv, LPT_TRANSCONF, val);
	if (intel_de_wait_for_set(dev_priv, LPT_TRANSCONF,
				  TRANS_STATE_ENABLE, 100))
		drm_err(&dev_priv->drm, "Failed to enable PCH transcoder\n");
}

static void ilk_disable_pch_transcoder(struct drm_i915_private *dev_priv,
				       enum pipe pipe)
{
	i915_reg_t reg;
	u32 val;

	/* FDI relies on the transcoder */
	assert_fdi_tx_disabled(dev_priv, pipe);
	assert_fdi_rx_disabled(dev_priv, pipe);

	/* Ports must be off as well */
	assert_pch_ports_disabled(dev_priv, pipe);

	reg = PCH_TRANSCONF(pipe);
	val = intel_de_read(dev_priv, reg);
	val &= ~TRANS_ENABLE;
	intel_de_write(dev_priv, reg, val);
	/* wait for PCH transcoder off, transcoder state */
	if (intel_de_wait_for_clear(dev_priv, reg, TRANS_STATE_ENABLE, 50))
		drm_err(&dev_priv->drm, "failed to disable transcoder %c\n",
			pipe_name(pipe));

	if (HAS_PCH_CPT(dev_priv)) {
		/* Workaround: Clear the timing override chicken bit again. */
		reg = TRANS_CHICKEN2(pipe);
		val = intel_de_read(dev_priv, reg);
		val &= ~TRANS_CHICKEN2_TIMING_OVERRIDE;
		intel_de_write(dev_priv, reg, val);
	}
}

void lpt_disable_pch_transcoder(struct drm_i915_private *dev_priv)
{
	u32 val;

	val = intel_de_read(dev_priv, LPT_TRANSCONF);
	val &= ~TRANS_ENABLE;
	intel_de_write(dev_priv, LPT_TRANSCONF, val);
	/* wait for PCH transcoder off, transcoder state */
	if (intel_de_wait_for_clear(dev_priv, LPT_TRANSCONF,
				    TRANS_STATE_ENABLE, 50))
		drm_err(&dev_priv->drm, "Failed to disable PCH transcoder\n");

	/* Workaround: clear timing override bit. */
	val = intel_de_read(dev_priv, TRANS_CHICKEN2(PIPE_A));
	val &= ~TRANS_CHICKEN2_TIMING_OVERRIDE;
	intel_de_write(dev_priv, TRANS_CHICKEN2(PIPE_A), val);
}

enum pipe intel_crtc_pch_transcoder(struct intel_crtc *crtc)
{
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);

	if (HAS_PCH_LPT(dev_priv))
		return PIPE_A;
	else
		return crtc->pipe;
}

void intel_enable_pipe(const struct intel_crtc_state *new_crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(new_crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	enum transcoder cpu_transcoder = new_crtc_state->cpu_transcoder;
	enum pipe pipe = crtc->pipe;
	i915_reg_t reg;
	u32 val;

	drm_dbg_kms(&dev_priv->drm, "enabling pipe %c\n", pipe_name(pipe));

	assert_planes_disabled(crtc);

	/*
	 * A pipe without a PLL won't actually be able to drive bits from
	 * a plane.  On ILK+ the pipe PLLs are integrated, so we don't
	 * need the check.
	 */
	if (HAS_GMCH(dev_priv)) {
		if (intel_crtc_has_type(new_crtc_state, INTEL_OUTPUT_DSI))
			assert_dsi_pll_enabled(dev_priv);
		else
			assert_pll_enabled(dev_priv, pipe);
	} else {
		if (new_crtc_state->has_pch_encoder) {
			/* if driving the PCH, we need FDI enabled */
			assert_fdi_rx_pll_enabled(dev_priv,
						  intel_crtc_pch_transcoder(crtc));
			assert_fdi_tx_pll_enabled(dev_priv,
						  (enum pipe) cpu_transcoder);
		}
		/* FIXME: assert CPU port conditions for SNB+ */
	}

	/* Wa_22012358565:adlp */
	if (DISPLAY_VER(dev_priv) == 13)
		intel_de_rmw(dev_priv, PIPE_ARB_CTL(pipe),
			     0, PIPE_ARB_USE_PROG_SLOTS);

	reg = PIPECONF(cpu_transcoder);
	val = intel_de_read(dev_priv, reg);
	if (val & PIPECONF_ENABLE) {
		/* we keep both pipes enabled on 830 */
		drm_WARN_ON(&dev_priv->drm, !IS_I830(dev_priv));
		return;
	}

	intel_de_write(dev_priv, reg, val | PIPECONF_ENABLE);
	intel_de_posting_read(dev_priv, reg);

	/*
	 * Until the pipe starts PIPEDSL reads will return a stale value,
	 * which causes an apparent vblank timestamp jump when PIPEDSL
	 * resets to its proper value. That also messes up the frame count
	 * when it's derived from the timestamps. So let's wait for the
	 * pipe to start properly before we call drm_crtc_vblank_on()
	 */
	if (intel_crtc_max_vblank_count(new_crtc_state) == 0)
		intel_wait_for_pipe_scanline_moving(crtc);
}

void intel_disable_pipe(const struct intel_crtc_state *old_crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(old_crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	enum transcoder cpu_transcoder = old_crtc_state->cpu_transcoder;
	enum pipe pipe = crtc->pipe;
	i915_reg_t reg;
	u32 val;

	drm_dbg_kms(&dev_priv->drm, "disabling pipe %c\n", pipe_name(pipe));

	/*
	 * Make sure planes won't keep trying to pump pixels to us,
	 * or we might hang the display.
	 */
	assert_planes_disabled(crtc);

	reg = PIPECONF(cpu_transcoder);
	val = intel_de_read(dev_priv, reg);
	if ((val & PIPECONF_ENABLE) == 0)
		return;

	/*
	 * Double wide has implications for planes
	 * so best keep it disabled when not needed.
	 */
	if (old_crtc_state->double_wide)
		val &= ~PIPECONF_DOUBLE_WIDE;

	/* Don't disable pipe or pipe PLLs if needed */
	if (!IS_I830(dev_priv))
		val &= ~PIPECONF_ENABLE;

	intel_de_write(dev_priv, reg, val);
	if ((val & PIPECONF_ENABLE) == 0)
		intel_wait_for_pipe_off(old_crtc_state);
}

bool
intel_format_info_is_yuv_semiplanar(const struct drm_format_info *info,
				    u64 modifier)
{
	return info->is_yuv &&
	       info->num_planes == (is_ccs_modifier(modifier) ? 4 : 2);
}

unsigned int
intel_tile_width_bytes(const struct drm_framebuffer *fb, int color_plane)
{
	struct drm_i915_private *dev_priv = to_i915(fb->dev);
	unsigned int cpp = fb->format->cpp[color_plane];

	switch (fb->modifier) {
	case DRM_FORMAT_MOD_LINEAR:
		return intel_tile_size(dev_priv);
	case I915_FORMAT_MOD_X_TILED:
		if (DISPLAY_VER(dev_priv) == 2)
			return 128;
		else
			return 512;
	case I915_FORMAT_MOD_Y_TILED_CCS:
		if (is_ccs_plane(fb, color_plane))
			return 128;
		fallthrough;
	case I915_FORMAT_MOD_Y_TILED_GEN12_RC_CCS:
	case I915_FORMAT_MOD_Y_TILED_GEN12_RC_CCS_CC:
	case I915_FORMAT_MOD_Y_TILED_GEN12_MC_CCS:
		if (is_ccs_plane(fb, color_plane))
			return 64;
		fallthrough;
	case I915_FORMAT_MOD_Y_TILED:
		if (DISPLAY_VER(dev_priv) == 2 || HAS_128_BYTE_Y_TILING(dev_priv))
			return 128;
		else
			return 512;
	case I915_FORMAT_MOD_Yf_TILED_CCS:
		if (is_ccs_plane(fb, color_plane))
			return 128;
		fallthrough;
	case I915_FORMAT_MOD_Yf_TILED:
		switch (cpp) {
		case 1:
			return 64;
		case 2:
		case 4:
			return 128;
		case 8:
		case 16:
			return 256;
		default:
			MISSING_CASE(cpp);
			return cpp;
		}
		break;
	default:
		MISSING_CASE(fb->modifier);
		return cpp;
	}
}

unsigned int
intel_fb_align_height(const struct drm_framebuffer *fb,
		      int color_plane, unsigned int height)
{
	unsigned int tile_height = intel_tile_height(fb, color_plane);

	return ALIGN(height, tile_height);
}

unsigned int intel_rotation_info_size(const struct intel_rotation_info *rot_info)
{
	unsigned int size = 0;
	int i;

	for (i = 0 ; i < ARRAY_SIZE(rot_info->plane); i++)
		size += rot_info->plane[i].dst_stride * rot_info->plane[i].width;

	return size;
}

unsigned int intel_remapped_info_size(const struct intel_remapped_info *rem_info)
{
	unsigned int size = 0;
	int i;

	for (i = 0 ; i < ARRAY_SIZE(rem_info->plane); i++)
		size += rem_info->plane[i].dst_stride * rem_info->plane[i].height;

	return size;
}

static unsigned int intel_linear_alignment(const struct drm_i915_private *dev_priv)
{
	if (DISPLAY_VER(dev_priv) >= 9)
		return 256 * 1024;
	else if (IS_I965G(dev_priv) || IS_I965GM(dev_priv) ||
		 IS_VALLEYVIEW(dev_priv) || IS_CHERRYVIEW(dev_priv))
		return 128 * 1024;
	else if (DISPLAY_VER(dev_priv) >= 4)
		return 4 * 1024;
	else
		return 0;
}

static bool has_async_flips(struct drm_i915_private *i915)
{
	return DISPLAY_VER(i915) >= 5;
}

unsigned int intel_surf_alignment(const struct drm_framebuffer *fb,
				  int color_plane)
{
	struct drm_i915_private *dev_priv = to_i915(fb->dev);

	if (intel_fb_uses_dpt(fb))
		return 512 * 4096;

	/* AUX_DIST needs only 4K alignment */
	if (is_ccs_plane(fb, color_plane))
		return 4096;

	if (is_semiplanar_uv_plane(fb, color_plane)) {
		/*
		 * TODO: cross-check wrt. the bspec stride in bytes * 64 bytes
		 * alignment for linear UV planes on all platforms.
		 */
		if (DISPLAY_VER(dev_priv) >= 12) {
			if (fb->modifier == DRM_FORMAT_MOD_LINEAR)
				return intel_linear_alignment(dev_priv);

			return intel_tile_row_size(fb, color_plane);
		}

		return 4096;
	}

	drm_WARN_ON(&dev_priv->drm, color_plane != 0);

	switch (fb->modifier) {
	case DRM_FORMAT_MOD_LINEAR:
		return intel_linear_alignment(dev_priv);
	case I915_FORMAT_MOD_X_TILED:
		if (has_async_flips(dev_priv))
			return 256 * 1024;
		return 0;
	case I915_FORMAT_MOD_Y_TILED_GEN12_MC_CCS:
	case I915_FORMAT_MOD_Y_TILED_GEN12_RC_CCS:
	case I915_FORMAT_MOD_Y_TILED_GEN12_RC_CCS_CC:
		return 16 * 1024;
	case I915_FORMAT_MOD_Y_TILED_CCS:
	case I915_FORMAT_MOD_Yf_TILED_CCS:
	case I915_FORMAT_MOD_Y_TILED:
	case I915_FORMAT_MOD_Yf_TILED:
		return 1 * 1024 * 1024;
	default:
		MISSING_CASE(fb->modifier);
		return 0;
	}
}

static bool intel_plane_uses_fence(const struct intel_plane_state *plane_state)
{
	struct intel_plane *plane = to_intel_plane(plane_state->uapi.plane);
	struct drm_i915_private *dev_priv = to_i915(plane->base.dev);

	return DISPLAY_VER(dev_priv) < 4 ||
		(plane->has_fbc &&
		 plane_state->view.gtt.type == I915_GGTT_VIEW_NORMAL);
}

static struct i915_vma *
intel_pin_fb_obj_dpt(struct drm_framebuffer *fb,
		     const struct i915_ggtt_view *view,
		     bool uses_fence,
		     unsigned long *out_flags,
		     struct i915_address_space *vm)
{
	struct drm_device *dev = fb->dev;
	struct drm_i915_private *dev_priv = to_i915(dev);
	struct drm_i915_gem_object *obj = intel_fb_obj(fb);
	struct i915_vma *vma;
	u32 alignment;
	int ret;

	if (WARN_ON(!i915_gem_object_is_framebuffer(obj)))
		return ERR_PTR(-EINVAL);

	alignment = 4096 * 512;

	atomic_inc(&dev_priv->gpu_error.pending_fb_pin);

	ret = i915_gem_object_set_cache_level(obj, I915_CACHE_NONE);
	if (ret) {
		vma = ERR_PTR(ret);
		goto err;
	}

	vma = i915_vma_instance(obj, vm, view);
	if (IS_ERR(vma))
		goto err;

	if (i915_vma_misplaced(vma, 0, alignment, 0)) {
		ret = i915_vma_unbind(vma);
		if (ret) {
			vma = ERR_PTR(ret);
			goto err;
		}
	}

	ret = i915_vma_pin(vma, 0, alignment, PIN_GLOBAL);
	if (ret) {
		vma = ERR_PTR(ret);
		goto err;
	}

	vma->display_alignment = max_t(u64, vma->display_alignment, alignment);

	i915_gem_object_flush_if_display(obj);

	i915_vma_get(vma);
err:
	atomic_dec(&dev_priv->gpu_error.pending_fb_pin);

	return vma;
}

struct i915_vma *
intel_pin_and_fence_fb_obj(struct drm_framebuffer *fb,
			   bool phys_cursor,
			   const struct i915_ggtt_view *view,
			   bool uses_fence,
			   unsigned long *out_flags)
{
	struct drm_device *dev = fb->dev;
	struct drm_i915_private *dev_priv = to_i915(dev);
	struct drm_i915_gem_object *obj = intel_fb_obj(fb);
	intel_wakeref_t wakeref;
	struct i915_gem_ww_ctx ww;
	struct i915_vma *vma;
	unsigned int pinctl;
	u32 alignment;
	int ret;

	if (drm_WARN_ON(dev, !i915_gem_object_is_framebuffer(obj)))
		return ERR_PTR(-EINVAL);

	if (phys_cursor)
		alignment = intel_cursor_alignment(dev_priv);
	else
		alignment = intel_surf_alignment(fb, 0);
	if (drm_WARN_ON(dev, alignment && !is_power_of_2(alignment)))
		return ERR_PTR(-EINVAL);

	/* Note that the w/a also requires 64 PTE of padding following the
	 * bo. We currently fill all unused PTE with the shadow page and so
	 * we should always have valid PTE following the scanout preventing
	 * the VT-d warning.
	 */
	if (intel_scanout_needs_vtd_wa(dev_priv) && alignment < 256 * 1024)
		alignment = 256 * 1024;

	/*
	 * Global gtt pte registers are special registers which actually forward
	 * writes to a chunk of system memory. Which means that there is no risk
	 * that the register values disappear as soon as we call
	 * intel_runtime_pm_put(), so it is correct to wrap only the
	 * pin/unpin/fence and not more.
	 */
	wakeref = intel_runtime_pm_get(&dev_priv->runtime_pm);

	atomic_inc(&dev_priv->gpu_error.pending_fb_pin);

	/*
	 * Valleyview is definitely limited to scanning out the first
	 * 512MiB. Lets presume this behaviour was inherited from the
	 * g4x display engine and that all earlier gen are similarly
	 * limited. Testing suggests that it is a little more
	 * complicated than this. For example, Cherryview appears quite
	 * happy to scanout from anywhere within its global aperture.
	 */
	pinctl = 0;
	if (HAS_GMCH(dev_priv))
		pinctl |= PIN_MAPPABLE;

	i915_gem_ww_ctx_init(&ww, true);
retry:
	ret = i915_gem_object_lock(obj, &ww);
	if (!ret && phys_cursor)
		ret = i915_gem_object_attach_phys(obj, alignment);
	if (!ret)
		ret = i915_gem_object_pin_pages(obj);
	if (ret)
		goto err;

	if (!ret) {
		vma = i915_gem_object_pin_to_display_plane(obj, &ww, alignment,
							   view, pinctl);
		if (IS_ERR(vma)) {
			ret = PTR_ERR(vma);
			goto err_unpin;
		}
	}

	if (uses_fence && i915_vma_is_map_and_fenceable(vma)) {
		/*
		 * Install a fence for tiled scan-out. Pre-i965 always needs a
		 * fence, whereas 965+ only requires a fence if using
		 * framebuffer compression.  For simplicity, we always, when
		 * possible, install a fence as the cost is not that onerous.
		 *
		 * If we fail to fence the tiled scanout, then either the
		 * modeset will reject the change (which is highly unlikely as
		 * the affected systems, all but one, do not have unmappable
		 * space) or we will not be able to enable full powersaving
		 * techniques (also likely not to apply due to various limits
		 * FBC and the like impose on the size of the buffer, which
		 * presumably we violated anyway with this unmappable buffer).
		 * Anyway, it is presumably better to stumble onwards with
		 * something and try to run the system in a "less than optimal"
		 * mode that matches the user configuration.
		 */
		ret = i915_vma_pin_fence(vma);
		if (ret != 0 && DISPLAY_VER(dev_priv) < 4) {
			i915_vma_unpin(vma);
			goto err_unpin;
		}
		ret = 0;

		if (vma->fence)
			*out_flags |= PLANE_HAS_FENCE;
	}

	i915_vma_get(vma);

err_unpin:
	i915_gem_object_unpin_pages(obj);
err:
	if (ret == -EDEADLK) {
		ret = i915_gem_ww_ctx_backoff(&ww);
		if (!ret)
			goto retry;
	}
	i915_gem_ww_ctx_fini(&ww);
	if (ret)
		vma = ERR_PTR(ret);

	atomic_dec(&dev_priv->gpu_error.pending_fb_pin);
	intel_runtime_pm_put(&dev_priv->runtime_pm, wakeref);
	return vma;
}

void intel_unpin_fb_vma(struct i915_vma *vma, unsigned long flags)
{
	if (flags & PLANE_HAS_FENCE)
		i915_vma_unpin_fence(vma);
	i915_vma_unpin(vma);
	i915_vma_put(vma);
}

/*
 * Convert the x/y offsets into a linear offset.
 * Only valid with 0/180 degree rotation, which is fine since linear
 * offset is only used with linear buffers on pre-hsw and tiled buffers
 * with gen2/3, and 90/270 degree rotations isn't supported on any of them.
 */
u32 intel_fb_xy_to_linear(int x, int y,
			  const struct intel_plane_state *state,
			  int color_plane)
{
	const struct drm_framebuffer *fb = state->hw.fb;
	unsigned int cpp = fb->format->cpp[color_plane];
	unsigned int pitch = state->view.color_plane[color_plane].stride;

	return y * pitch + x * cpp;
}

/*
 * Add the x/y offsets derived from fb->offsets[] to the user
 * specified plane src x/y offsets. The resulting x/y offsets
 * specify the start of scanout from the beginning of the gtt mapping.
 */
void intel_add_fb_offsets(int *x, int *y,
			  const struct intel_plane_state *state,
			  int color_plane)

{
	*x += state->view.color_plane[color_plane].x;
	*y += state->view.color_plane[color_plane].y;
}

static unsigned int intel_fb_modifier_to_tiling(u64 fb_modifier)
{
	switch (fb_modifier) {
	case I915_FORMAT_MOD_X_TILED:
		return I915_TILING_X;
	case I915_FORMAT_MOD_Y_TILED:
	case I915_FORMAT_MOD_Y_TILED_CCS:
	case I915_FORMAT_MOD_Y_TILED_GEN12_RC_CCS:
	case I915_FORMAT_MOD_Y_TILED_GEN12_RC_CCS_CC:
	case I915_FORMAT_MOD_Y_TILED_GEN12_MC_CCS:
		return I915_TILING_Y;
	default:
		return I915_TILING_NONE;
	}
}

/*
 * From the Sky Lake PRM:
 * "The Color Control Surface (CCS) contains the compression status of
 *  the cache-line pairs. The compression state of the cache-line pair
 *  is specified by 2 bits in the CCS. Each CCS cache-line represents
 *  an area on the main surface of 16 x16 sets of 128 byte Y-tiled
 *  cache-line-pairs. CCS is always Y tiled."
 *
 * Since cache line pairs refers to horizontally adjacent cache lines,
 * each cache line in the CCS corresponds to an area of 32x16 cache
 * lines on the main surface. Since each pixel is 4 bytes, this gives
 * us a ratio of one byte in the CCS for each 8x16 pixels in the
 * main surface.
 */
static const struct drm_format_info skl_ccs_formats[] = {
	{ .format = DRM_FORMAT_XRGB8888, .depth = 24, .num_planes = 2,
	  .cpp = { 4, 1, }, .hsub = 8, .vsub = 16, },
	{ .format = DRM_FORMAT_XBGR8888, .depth = 24, .num_planes = 2,
	  .cpp = { 4, 1, }, .hsub = 8, .vsub = 16, },
	{ .format = DRM_FORMAT_ARGB8888, .depth = 32, .num_planes = 2,
	  .cpp = { 4, 1, }, .hsub = 8, .vsub = 16, .has_alpha = true, },
	{ .format = DRM_FORMAT_ABGR8888, .depth = 32, .num_planes = 2,
	  .cpp = { 4, 1, }, .hsub = 8, .vsub = 16, .has_alpha = true, },
};

/*
 * Gen-12 compression uses 4 bits of CCS data for each cache line pair in the
 * main surface. And each 64B CCS cache line represents an area of 4x1 Y-tiles
 * in the main surface. With 4 byte pixels and each Y-tile having dimensions of
 * 32x32 pixels, the ratio turns out to 1B in the CCS for every 2x32 pixels in
 * the main surface.
 */
static const struct drm_format_info gen12_ccs_formats[] = {
	{ .format = DRM_FORMAT_XRGB8888, .depth = 24, .num_planes = 2,
	  .char_per_block = { 4, 1 }, .block_w = { 1, 2 }, .block_h = { 1, 1 },
	  .hsub = 1, .vsub = 1, },
	{ .format = DRM_FORMAT_XBGR8888, .depth = 24, .num_planes = 2,
	  .char_per_block = { 4, 1 }, .block_w = { 1, 2 }, .block_h = { 1, 1 },
	  .hsub = 1, .vsub = 1, },
	{ .format = DRM_FORMAT_ARGB8888, .depth = 32, .num_planes = 2,
	  .char_per_block = { 4, 1 }, .block_w = { 1, 2 }, .block_h = { 1, 1 },
	  .hsub = 1, .vsub = 1, .has_alpha = true },
	{ .format = DRM_FORMAT_ABGR8888, .depth = 32, .num_planes = 2,
	  .char_per_block = { 4, 1 }, .block_w = { 1, 2 }, .block_h = { 1, 1 },
	  .hsub = 1, .vsub = 1, .has_alpha = true },
	{ .format = DRM_FORMAT_YUYV, .num_planes = 2,
	  .char_per_block = { 2, 1 }, .block_w = { 1, 2 }, .block_h = { 1, 1 },
	  .hsub = 2, .vsub = 1, .is_yuv = true },
	{ .format = DRM_FORMAT_YVYU, .num_planes = 2,
	  .char_per_block = { 2, 1 }, .block_w = { 1, 2 }, .block_h = { 1, 1 },
	  .hsub = 2, .vsub = 1, .is_yuv = true },
	{ .format = DRM_FORMAT_UYVY, .num_planes = 2,
	  .char_per_block = { 2, 1 }, .block_w = { 1, 2 }, .block_h = { 1, 1 },
	  .hsub = 2, .vsub = 1, .is_yuv = true },
	{ .format = DRM_FORMAT_VYUY, .num_planes = 2,
	  .char_per_block = { 2, 1 }, .block_w = { 1, 2 }, .block_h = { 1, 1 },
	  .hsub = 2, .vsub = 1, .is_yuv = true },
	{ .format = DRM_FORMAT_XYUV8888, .num_planes = 2,
	  .char_per_block = { 4, 1 }, .block_w = { 1, 2 }, .block_h = { 1, 1 },
	  .hsub = 1, .vsub = 1, .is_yuv = true },
	{ .format = DRM_FORMAT_NV12, .num_planes = 4,
	  .char_per_block = { 1, 2, 1, 1 }, .block_w = { 1, 1, 4, 4 }, .block_h = { 1, 1, 1, 1 },
	  .hsub = 2, .vsub = 2, .is_yuv = true },
	{ .format = DRM_FORMAT_P010, .num_planes = 4,
	  .char_per_block = { 2, 4, 1, 1 }, .block_w = { 1, 1, 2, 2 }, .block_h = { 1, 1, 1, 1 },
	  .hsub = 2, .vsub = 2, .is_yuv = true },
	{ .format = DRM_FORMAT_P012, .num_planes = 4,
	  .char_per_block = { 2, 4, 1, 1 }, .block_w = { 1, 1, 2, 2 }, .block_h = { 1, 1, 1, 1 },
	  .hsub = 2, .vsub = 2, .is_yuv = true },
	{ .format = DRM_FORMAT_P016, .num_planes = 4,
	  .char_per_block = { 2, 4, 1, 1 }, .block_w = { 1, 1, 2, 2 }, .block_h = { 1, 1, 1, 1 },
	  .hsub = 2, .vsub = 2, .is_yuv = true },
};

/*
 * Same as gen12_ccs_formats[] above, but with additional surface used
 * to pass Clear Color information in plane 2 with 64 bits of data.
 */
static const struct drm_format_info gen12_ccs_cc_formats[] = {
	{ .format = DRM_FORMAT_XRGB8888, .depth = 24, .num_planes = 3,
	  .char_per_block = { 4, 1, 0 }, .block_w = { 1, 2, 2 }, .block_h = { 1, 1, 1 },
	  .hsub = 1, .vsub = 1, },
	{ .format = DRM_FORMAT_XBGR8888, .depth = 24, .num_planes = 3,
	  .char_per_block = { 4, 1, 0 }, .block_w = { 1, 2, 2 }, .block_h = { 1, 1, 1 },
	  .hsub = 1, .vsub = 1, },
	{ .format = DRM_FORMAT_ARGB8888, .depth = 32, .num_planes = 3,
	  .char_per_block = { 4, 1, 0 }, .block_w = { 1, 2, 2 }, .block_h = { 1, 1, 1 },
	  .hsub = 1, .vsub = 1, .has_alpha = true },
	{ .format = DRM_FORMAT_ABGR8888, .depth = 32, .num_planes = 3,
	  .char_per_block = { 4, 1, 0 }, .block_w = { 1, 2, 2 }, .block_h = { 1, 1, 1 },
	  .hsub = 1, .vsub = 1, .has_alpha = true },
};

static const struct drm_format_info *
lookup_format_info(const struct drm_format_info formats[],
		   int num_formats, u32 format)
{
	int i;

	for (i = 0; i < num_formats; i++) {
		if (formats[i].format == format)
			return &formats[i];
	}

	return NULL;
}

static const struct drm_format_info *
intel_get_format_info(const struct drm_mode_fb_cmd2 *cmd)
{
	switch (cmd->modifier[0]) {
	case I915_FORMAT_MOD_Y_TILED_CCS:
	case I915_FORMAT_MOD_Yf_TILED_CCS:
		return lookup_format_info(skl_ccs_formats,
					  ARRAY_SIZE(skl_ccs_formats),
					  cmd->pixel_format);
	case I915_FORMAT_MOD_Y_TILED_GEN12_RC_CCS:
	case I915_FORMAT_MOD_Y_TILED_GEN12_MC_CCS:
		return lookup_format_info(gen12_ccs_formats,
					  ARRAY_SIZE(gen12_ccs_formats),
					  cmd->pixel_format);
	case I915_FORMAT_MOD_Y_TILED_GEN12_RC_CCS_CC:
		return lookup_format_info(gen12_ccs_cc_formats,
					  ARRAY_SIZE(gen12_ccs_cc_formats),
					  cmd->pixel_format);
	default:
		return NULL;
	}
}

static int gen12_ccs_aux_stride(struct drm_framebuffer *fb, int ccs_plane)
{
	return DIV_ROUND_UP(fb->pitches[skl_ccs_to_main_plane(fb, ccs_plane)],
			    512) * 64;
}

u32 intel_plane_fb_max_stride(struct drm_i915_private *dev_priv,
			      u32 pixel_format, u64 modifier)
{
	struct intel_crtc *crtc;
	struct intel_plane *plane;

	if (!HAS_DISPLAY(dev_priv))
		return 0;

	/*
	 * We assume the primary plane for pipe A has
	 * the highest stride limits of them all,
	 * if in case pipe A is disabled, use the first pipe from pipe_mask.
	 */
	crtc = intel_get_first_crtc(dev_priv);
	if (!crtc)
		return 0;

	plane = to_intel_plane(crtc->base.primary);

	return plane->max_stride(plane, pixel_format, modifier,
				 DRM_MODE_ROTATE_0);
}

static
u32 intel_fb_max_stride(struct drm_i915_private *dev_priv,
			u32 pixel_format, u64 modifier)
{
	/*
	 * Arbitrary limit for gen4+ chosen to match the
	 * render engine max stride.
	 *
	 * The new CCS hash mode makes remapping impossible
	 */
	if (DISPLAY_VER(dev_priv) < 4 || is_ccs_modifier(modifier) ||
	    intel_modifier_uses_dpt(dev_priv, modifier))
		return intel_plane_fb_max_stride(dev_priv, pixel_format, modifier);
	else if (DISPLAY_VER(dev_priv) >= 7)
		return 256 * 1024;
	else
		return 128 * 1024;
}

static u32
intel_fb_stride_alignment(const struct drm_framebuffer *fb, int color_plane)
{
	struct drm_i915_private *dev_priv = to_i915(fb->dev);
	u32 tile_width;

	if (is_surface_linear(fb, color_plane)) {
		u32 max_stride = intel_plane_fb_max_stride(dev_priv,
							   fb->format->format,
							   fb->modifier);

		/*
		 * To make remapping with linear generally feasible
		 * we need the stride to be page aligned.
		 */
		if (fb->pitches[color_plane] > max_stride &&
		    !is_ccs_modifier(fb->modifier))
			return intel_tile_size(dev_priv);
		else
			return 64;
	}

	tile_width = intel_tile_width_bytes(fb, color_plane);
	if (is_ccs_modifier(fb->modifier)) {
		/*
		 * Display WA #0531: skl,bxt,kbl,glk
		 *
		 * Render decompression and plane width > 3840
		 * combined with horizontal panning requires the
		 * plane stride to be a multiple of 4. We'll just
		 * require the entire fb to accommodate that to avoid
		 * potential runtime errors at plane configuration time.
		 */
		if ((DISPLAY_VER(dev_priv) == 9 || IS_GEMINILAKE(dev_priv)) &&
		    color_plane == 0 && fb->width > 3840)
			tile_width *= 4;
		/*
		 * The main surface pitch must be padded to a multiple of four
		 * tile widths.
		 */
		else if (DISPLAY_VER(dev_priv) >= 12)
			tile_width *= 4;
	}
	return tile_width;
}

static struct i915_vma *
initial_plane_vma(struct drm_i915_private *i915,
		  struct intel_initial_plane_config *plane_config)
{
	struct drm_i915_gem_object *obj;
	struct i915_vma *vma;
	u32 base, size;

	if (plane_config->size == 0)
		return NULL;

	base = round_down(plane_config->base,
			  I915_GTT_MIN_ALIGNMENT);
	size = round_up(plane_config->base + plane_config->size,
			I915_GTT_MIN_ALIGNMENT);
	size -= base;

	/*
	 * If the FB is too big, just don't use it since fbdev is not very
	 * important and we should probably use that space with FBC or other
	 * features.
	 */
	if (IS_ENABLED(CONFIG_FRAMEBUFFER_CONSOLE) &&
	    size * 2 > i915->stolen_usable_size)
		return NULL;

	obj = i915_gem_object_create_stolen_for_preallocated(i915, base, size);
	if (IS_ERR(obj))
		return NULL;

	/*
	 * Mark it WT ahead of time to avoid changing the
	 * cache_level during fbdev initialization. The
	 * unbind there would get stuck waiting for rcu.
	 */
	i915_gem_object_set_cache_coherency(obj, HAS_WT(i915) ?
					    I915_CACHE_WT : I915_CACHE_NONE);

	switch (plane_config->tiling) {
	case I915_TILING_NONE:
		break;
	case I915_TILING_X:
	case I915_TILING_Y:
		obj->tiling_and_stride =
			plane_config->fb->base.pitches[0] |
			plane_config->tiling;
		break;
	default:
		MISSING_CASE(plane_config->tiling);
		goto err_obj;
	}

	vma = i915_vma_instance(obj, &i915->ggtt.vm, NULL);
	if (IS_ERR(vma))
		goto err_obj;

	if (i915_ggtt_pin(vma, NULL, 0, PIN_MAPPABLE | PIN_OFFSET_FIXED | base))
		goto err_obj;

	if (i915_gem_object_is_tiled(obj) &&
	    !i915_vma_is_map_and_fenceable(vma))
		goto err_obj;

	return vma;

err_obj:
	i915_gem_object_put(obj);
	return NULL;
}

static bool
intel_alloc_initial_plane_obj(struct intel_crtc *crtc,
			      struct intel_initial_plane_config *plane_config)
{
	struct drm_device *dev = crtc->base.dev;
	struct drm_i915_private *dev_priv = to_i915(dev);
	struct drm_mode_fb_cmd2 mode_cmd = { 0 };
	struct drm_framebuffer *fb = &plane_config->fb->base;
	struct i915_vma *vma;

	switch (fb->modifier) {
	case DRM_FORMAT_MOD_LINEAR:
	case I915_FORMAT_MOD_X_TILED:
	case I915_FORMAT_MOD_Y_TILED:
		break;
	default:
		drm_dbg(&dev_priv->drm,
			"Unsupported modifier for initial FB: 0x%llx\n",
			fb->modifier);
		return false;
	}

	vma = initial_plane_vma(dev_priv, plane_config);
	if (!vma)
		return false;

	mode_cmd.pixel_format = fb->format->format;
	mode_cmd.width = fb->width;
	mode_cmd.height = fb->height;
	mode_cmd.pitches[0] = fb->pitches[0];
	mode_cmd.modifier[0] = fb->modifier;
	mode_cmd.flags = DRM_MODE_FB_MODIFIERS;

	if (intel_framebuffer_init(to_intel_framebuffer(fb),
				   vma->obj, &mode_cmd)) {
		drm_dbg_kms(&dev_priv->drm, "intel fb init failed\n");
		goto err_vma;
	}

	plane_config->vma = vma;
	return true;

err_vma:
	i915_vma_put(vma);
	return false;
}

static void
intel_set_plane_visible(struct intel_crtc_state *crtc_state,
			struct intel_plane_state *plane_state,
			bool visible)
{
	struct intel_plane *plane = to_intel_plane(plane_state->uapi.plane);

	plane_state->uapi.visible = visible;

	if (visible)
		crtc_state->uapi.plane_mask |= drm_plane_mask(&plane->base);
	else
		crtc_state->uapi.plane_mask &= ~drm_plane_mask(&plane->base);
}

static void fixup_plane_bitmasks(struct intel_crtc_state *crtc_state)
{
	struct drm_i915_private *dev_priv = to_i915(crtc_state->uapi.crtc->dev);
	struct drm_plane *plane;

	/*
	 * Active_planes aliases if multiple "primary" or cursor planes
	 * have been used on the same (or wrong) pipe. plane_mask uses
	 * unique ids, hence we can use that to reconstruct active_planes.
	 */
	crtc_state->enabled_planes = 0;
	crtc_state->active_planes = 0;

	drm_for_each_plane_mask(plane, &dev_priv->drm,
				crtc_state->uapi.plane_mask) {
		crtc_state->enabled_planes |= BIT(to_intel_plane(plane)->id);
		crtc_state->active_planes |= BIT(to_intel_plane(plane)->id);
	}
}

static void intel_plane_disable_noatomic(struct intel_crtc *crtc,
					 struct intel_plane *plane)
{
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	struct intel_crtc_state *crtc_state =
		to_intel_crtc_state(crtc->base.state);
	struct intel_plane_state *plane_state =
		to_intel_plane_state(plane->base.state);

	drm_dbg_kms(&dev_priv->drm,
		    "Disabling [PLANE:%d:%s] on [CRTC:%d:%s]\n",
		    plane->base.base.id, plane->base.name,
		    crtc->base.base.id, crtc->base.name);

	intel_set_plane_visible(crtc_state, plane_state, false);
	fixup_plane_bitmasks(crtc_state);
	crtc_state->data_rate[plane->id] = 0;
	crtc_state->min_cdclk[plane->id] = 0;

	if (plane->id == PLANE_PRIMARY)
		hsw_disable_ips(crtc_state);

	/*
	 * Vblank time updates from the shadow to live plane control register
	 * are blocked if the memory self-refresh mode is active at that
	 * moment. So to make sure the plane gets truly disabled, disable
	 * first the self-refresh mode. The self-refresh enable bit in turn
	 * will be checked/applied by the HW only at the next frame start
	 * event which is after the vblank start event, so we need to have a
	 * wait-for-vblank between disabling the plane and the pipe.
	 */
	if (HAS_GMCH(dev_priv) &&
	    intel_set_memory_cxsr(dev_priv, false))
		intel_wait_for_vblank(dev_priv, crtc->pipe);

	/*
	 * Gen2 reports pipe underruns whenever all planes are disabled.
	 * So disable underrun reporting before all the planes get disabled.
	 */
	if (DISPLAY_VER(dev_priv) == 2 && !crtc_state->active_planes)
		intel_set_cpu_fifo_underrun_reporting(dev_priv, crtc->pipe, false);

	intel_disable_plane(plane, crtc_state);
	intel_wait_for_vblank(dev_priv, crtc->pipe);
}

static struct i915_vma *intel_dpt_pin(struct i915_address_space *vm)
{
	struct drm_i915_private *i915 = vm->i915;
	struct i915_dpt *dpt = i915_vm_to_dpt(vm);
	intel_wakeref_t wakeref;
	struct i915_vma *vma;
	void __iomem *iomem;

	wakeref = intel_runtime_pm_get(&i915->runtime_pm);
	atomic_inc(&i915->gpu_error.pending_fb_pin);

	vma = i915_gem_object_ggtt_pin(dpt->obj, NULL, 0, 4096,
				       HAS_LMEM(i915) ? 0 : PIN_MAPPABLE);
	if (IS_ERR(vma))
		goto err;

	iomem = i915_vma_pin_iomap(vma);
	i915_vma_unpin(vma);
	if (IS_ERR(iomem)) {
		vma = iomem;
		goto err;
	}

	dpt->vma = vma;
	dpt->iomem = iomem;

	i915_vma_get(vma);

err:
	atomic_dec(&i915->gpu_error.pending_fb_pin);
	intel_runtime_pm_put(&i915->runtime_pm, wakeref);

	return vma;
}

static void intel_dpt_unpin(struct i915_address_space *vm)
{
	struct i915_dpt *dpt = i915_vm_to_dpt(vm);

	i915_vma_unpin_iomap(dpt->vma);
	i915_vma_put(dpt->vma);
}

static void
intel_find_initial_plane_obj(struct intel_crtc *intel_crtc,
			     struct intel_initial_plane_config *plane_config)
{
	struct drm_device *dev = intel_crtc->base.dev;
	struct drm_i915_private *dev_priv = to_i915(dev);
	struct drm_crtc *c;
	struct drm_plane *primary = intel_crtc->base.primary;
	struct drm_plane_state *plane_state = primary->state;
	struct intel_plane *intel_plane = to_intel_plane(primary);
	struct intel_plane_state *intel_state =
		to_intel_plane_state(plane_state);
	struct intel_crtc_state *crtc_state =
		to_intel_crtc_state(intel_crtc->base.state);
	struct drm_framebuffer *fb;
	struct i915_vma *vma;

	/*
	 * TODO:
	 *   Disable planes if get_initial_plane_config() failed.
	 *   Make sure things work if the surface base is not page aligned.
	 */
	if (!plane_config->fb)
		return;

	if (intel_alloc_initial_plane_obj(intel_crtc, plane_config)) {
		fb = &plane_config->fb->base;
		vma = plane_config->vma;
		goto valid_fb;
	}

	/*
	 * Failed to alloc the obj, check to see if we should share
	 * an fb with another CRTC instead
	 */
	for_each_crtc(dev, c) {
		struct intel_plane_state *state;

		if (c == &intel_crtc->base)
			continue;

		if (!to_intel_crtc_state(c->state)->uapi.active)
			continue;

		state = to_intel_plane_state(c->primary->state);
		if (!state->ggtt_vma)
			continue;

		if (intel_plane_ggtt_offset(state) == plane_config->base) {
			fb = state->hw.fb;
			vma = state->ggtt_vma;
			goto valid_fb;
		}
	}

	/*
	 * We've failed to reconstruct the BIOS FB.  Current display state
	 * indicates that the primary plane is visible, but has a NULL FB,
	 * which will lead to problems later if we don't fix it up.  The
	 * simplest solution is to just disable the primary plane now and
	 * pretend the BIOS never had it enabled.
	 */
	intel_plane_disable_noatomic(intel_crtc, intel_plane);
	if (crtc_state->bigjoiner) {
		struct intel_crtc *slave =
			crtc_state->bigjoiner_linked_crtc;
		intel_plane_disable_noatomic(slave, to_intel_plane(slave->base.primary));
	}

	return;

valid_fb:
	plane_state->rotation = plane_config->rotation;
	intel_fb_fill_view(to_intel_framebuffer(fb), plane_state->rotation,
			   &intel_state->view);

	__i915_vma_pin(vma);
	intel_state->ggtt_vma = i915_vma_get(vma);
	if (intel_plane_uses_fence(intel_state) && i915_vma_pin_fence(vma) == 0)
		if (vma->fence)
			intel_state->flags |= PLANE_HAS_FENCE;

	plane_state->src_x = 0;
	plane_state->src_y = 0;
	plane_state->src_w = fb->width << 16;
	plane_state->src_h = fb->height << 16;

	plane_state->crtc_x = 0;
	plane_state->crtc_y = 0;
	plane_state->crtc_w = fb->width;
	plane_state->crtc_h = fb->height;

	if (plane_config->tiling)
		dev_priv->preserve_bios_swizzle = true;

	plane_state->fb = fb;
	drm_framebuffer_get(fb);

	plane_state->crtc = &intel_crtc->base;
	intel_plane_copy_uapi_to_hw_state(intel_state, intel_state,
					  intel_crtc);

	intel_frontbuffer_flush(to_intel_frontbuffer(fb), ORIGIN_DIRTYFB);

	atomic_or(to_intel_plane(primary)->frontbuffer_bit,
		  &to_intel_frontbuffer(fb)->bits);
}

unsigned int
intel_plane_fence_y_offset(const struct intel_plane_state *plane_state)
{
	int x = 0, y = 0;

	intel_plane_adjust_aligned_offset(&x, &y, plane_state, 0,
					  plane_state->view.color_plane[0].offset, 0);

	return y;
}

static int
__intel_display_resume(struct drm_device *dev,
		       struct drm_atomic_state *state,
		       struct drm_modeset_acquire_ctx *ctx)
{
	struct drm_crtc_state *crtc_state;
	struct drm_crtc *crtc;
	int i, ret;

	intel_modeset_setup_hw_state(dev, ctx);
	intel_vga_redisable(to_i915(dev));

	if (!state)
		return 0;

	/*
	 * We've duplicated the state, pointers to the old state are invalid.
	 *
	 * Don't attempt to use the old state until we commit the duplicated state.
	 */
	for_each_new_crtc_in_state(state, crtc, crtc_state, i) {
		/*
		 * Force recalculation even if we restore
		 * current state. With fast modeset this may not result
		 * in a modeset when the state is compatible.
		 */
		crtc_state->mode_changed = true;
	}

	/* ignore any reset values/BIOS leftovers in the WM registers */
	if (!HAS_GMCH(to_i915(dev)))
		to_intel_atomic_state(state)->skip_intermediate_wm = true;

	ret = drm_atomic_helper_commit_duplicated_state(state, ctx);

	drm_WARN_ON(dev, ret == -EDEADLK);
	return ret;
}

static bool gpu_reset_clobbers_display(struct drm_i915_private *dev_priv)
{
	return (INTEL_INFO(dev_priv)->gpu_reset_clobbers_display &&
		intel_has_gpu_reset(&dev_priv->gt));
}

void intel_display_prepare_reset(struct drm_i915_private *dev_priv)
{
	struct drm_device *dev = &dev_priv->drm;
	struct drm_modeset_acquire_ctx *ctx = &dev_priv->reset_ctx;
	struct drm_atomic_state *state;
	int ret;

	if (!HAS_DISPLAY(dev_priv))
		return;

	/* reset doesn't touch the display */
	if (!dev_priv->params.force_reset_modeset_test &&
	    !gpu_reset_clobbers_display(dev_priv))
		return;

	/* We have a modeset vs reset deadlock, defensively unbreak it. */
	set_bit(I915_RESET_MODESET, &dev_priv->gt.reset.flags);
	smp_mb__after_atomic();
	wake_up_bit(&dev_priv->gt.reset.flags, I915_RESET_MODESET);

	if (atomic_read(&dev_priv->gpu_error.pending_fb_pin)) {
		drm_dbg_kms(&dev_priv->drm,
			    "Modeset potentially stuck, unbreaking through wedging\n");
		intel_gt_set_wedged(&dev_priv->gt);
	}

	/*
	 * Need mode_config.mutex so that we don't
	 * trample ongoing ->detect() and whatnot.
	 */
	mutex_lock(&dev->mode_config.mutex);
	drm_modeset_acquire_init(ctx, 0);
	while (1) {
		ret = drm_modeset_lock_all_ctx(dev, ctx);
		if (ret != -EDEADLK)
			break;

		drm_modeset_backoff(ctx);
	}
	/*
	 * Disabling the crtcs gracefully seems nicer. Also the
	 * g33 docs say we should at least disable all the planes.
	 */
	state = drm_atomic_helper_duplicate_state(dev, ctx);
	if (IS_ERR(state)) {
		ret = PTR_ERR(state);
		drm_err(&dev_priv->drm, "Duplicating state failed with %i\n",
			ret);
		return;
	}

	ret = drm_atomic_helper_disable_all(dev, ctx);
	if (ret) {
		drm_err(&dev_priv->drm, "Suspending crtc's failed with %i\n",
			ret);
		drm_atomic_state_put(state);
		return;
	}

	dev_priv->modeset_restore_state = state;
	state->acquire_ctx = ctx;
}

void intel_display_finish_reset(struct drm_i915_private *dev_priv)
{
	struct drm_device *dev = &dev_priv->drm;
	struct drm_modeset_acquire_ctx *ctx = &dev_priv->reset_ctx;
	struct drm_atomic_state *state;
	int ret;

	if (!HAS_DISPLAY(dev_priv))
		return;

	/* reset doesn't touch the display */
	if (!test_bit(I915_RESET_MODESET, &dev_priv->gt.reset.flags))
		return;

	state = fetch_and_zero(&dev_priv->modeset_restore_state);
	if (!state)
		goto unlock;

	/* reset doesn't touch the display */
	if (!gpu_reset_clobbers_display(dev_priv)) {
		/* for testing only restore the display */
		ret = __intel_display_resume(dev, state, ctx);
		if (ret)
			drm_err(&dev_priv->drm,
				"Restoring old state failed with %i\n", ret);
	} else {
		/*
		 * The display has been reset as well,
		 * so need a full re-initialization.
		 */
		intel_pps_unlock_regs_wa(dev_priv);
		intel_modeset_init_hw(dev_priv);
		intel_init_clock_gating(dev_priv);
		intel_hpd_init(dev_priv);

		ret = __intel_display_resume(dev, state, ctx);
		if (ret)
			drm_err(&dev_priv->drm,
				"Restoring old state failed with %i\n", ret);

		intel_hpd_poll_disable(dev_priv);
	}

	drm_atomic_state_put(state);
unlock:
	drm_modeset_drop_locks(ctx);
	drm_modeset_acquire_fini(ctx);
	mutex_unlock(&dev->mode_config.mutex);

	clear_bit_unlock(I915_RESET_MODESET, &dev_priv->gt.reset.flags);
}

static void icl_set_pipe_chicken(struct intel_crtc *crtc)
{
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	enum pipe pipe = crtc->pipe;
	u32 tmp;

	tmp = intel_de_read(dev_priv, PIPE_CHICKEN(pipe));

	/*
	 * Display WA #1153: icl
	 * enable hardware to bypass the alpha math
	 * and rounding for per-pixel values 00 and 0xff
	 */
	tmp |= PER_PIXEL_ALPHA_BYPASS_EN;
	/*
	 * Display WA # 1605353570: icl
	 * Set the pixel rounding bit to 1 for allowing
	 * passthrough of Frame buffer pixels unmodified
	 * across pipe
	 */
	tmp |= PIXEL_ROUNDING_TRUNC_FB_PASSTHRU;

	/*
	 * "The underrun recovery mechanism should be disabled
	 *  when the following is enabled for this pipe:
	 *  WiDi
	 *  Downscaling (this includes YUV420 fullblend)
	 *  COG
	 *  DSC
	 *  PSR2"
	 *
	 * FIXME: enable whenever possible...
	 */
	if (IS_ALDERLAKE_P(dev_priv))
		tmp |= UNDERRUN_RECOVERY_DISABLE;

	intel_de_write(dev_priv, PIPE_CHICKEN(pipe), tmp);
}

bool intel_has_pending_fb_unpin(struct drm_i915_private *dev_priv)
{
	struct drm_crtc *crtc;
	bool cleanup_done;

	drm_for_each_crtc(crtc, &dev_priv->drm) {
		struct drm_crtc_commit *commit;
		spin_lock(&crtc->commit_lock);
		commit = list_first_entry_or_null(&crtc->commit_list,
						  struct drm_crtc_commit, commit_entry);
		cleanup_done = commit ?
			try_wait_for_completion(&commit->cleanup_done) : true;
		spin_unlock(&crtc->commit_lock);

		if (cleanup_done)
			continue;

		drm_crtc_wait_one_vblank(crtc);

		return true;
	}

	return false;
}

void lpt_disable_iclkip(struct drm_i915_private *dev_priv)
{
	u32 temp;

	intel_de_write(dev_priv, PIXCLK_GATE, PIXCLK_GATE_GATE);

	mutex_lock(&dev_priv->sb_lock);

	temp = intel_sbi_read(dev_priv, SBI_SSCCTL6, SBI_ICLK);
	temp |= SBI_SSCCTL_DISABLE;
	intel_sbi_write(dev_priv, SBI_SSCCTL6, temp, SBI_ICLK);

	mutex_unlock(&dev_priv->sb_lock);
}

/* Program iCLKIP clock to the desired frequency */
static void lpt_program_iclkip(const struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	int clock = crtc_state->hw.adjusted_mode.crtc_clock;
	u32 divsel, phaseinc, auxdiv, phasedir = 0;
	u32 temp;

	lpt_disable_iclkip(dev_priv);

	/* The iCLK virtual clock root frequency is in MHz,
	 * but the adjusted_mode->crtc_clock in in KHz. To get the
	 * divisors, it is necessary to divide one by another, so we
	 * convert the virtual clock precision to KHz here for higher
	 * precision.
	 */
	for (auxdiv = 0; auxdiv < 2; auxdiv++) {
		u32 iclk_virtual_root_freq = 172800 * 1000;
		u32 iclk_pi_range = 64;
		u32 desired_divisor;

		desired_divisor = DIV_ROUND_CLOSEST(iclk_virtual_root_freq,
						    clock << auxdiv);
		divsel = (desired_divisor / iclk_pi_range) - 2;
		phaseinc = desired_divisor % iclk_pi_range;

		/*
		 * Near 20MHz is a corner case which is
		 * out of range for the 7-bit divisor
		 */
		if (divsel <= 0x7f)
			break;
	}

	/* This should not happen with any sane values */
	drm_WARN_ON(&dev_priv->drm, SBI_SSCDIVINTPHASE_DIVSEL(divsel) &
		    ~SBI_SSCDIVINTPHASE_DIVSEL_MASK);
	drm_WARN_ON(&dev_priv->drm, SBI_SSCDIVINTPHASE_DIR(phasedir) &
		    ~SBI_SSCDIVINTPHASE_INCVAL_MASK);

	drm_dbg_kms(&dev_priv->drm,
		    "iCLKIP clock: found settings for %dKHz refresh rate: auxdiv=%x, divsel=%x, phasedir=%x, phaseinc=%x\n",
		    clock, auxdiv, divsel, phasedir, phaseinc);

	mutex_lock(&dev_priv->sb_lock);

	/* Program SSCDIVINTPHASE6 */
	temp = intel_sbi_read(dev_priv, SBI_SSCDIVINTPHASE6, SBI_ICLK);
	temp &= ~SBI_SSCDIVINTPHASE_DIVSEL_MASK;
	temp |= SBI_SSCDIVINTPHASE_DIVSEL(divsel);
	temp &= ~SBI_SSCDIVINTPHASE_INCVAL_MASK;
	temp |= SBI_SSCDIVINTPHASE_INCVAL(phaseinc);
	temp |= SBI_SSCDIVINTPHASE_DIR(phasedir);
	temp |= SBI_SSCDIVINTPHASE_PROPAGATE;
	intel_sbi_write(dev_priv, SBI_SSCDIVINTPHASE6, temp, SBI_ICLK);

	/* Program SSCAUXDIV */
	temp = intel_sbi_read(dev_priv, SBI_SSCAUXDIV6, SBI_ICLK);
	temp &= ~SBI_SSCAUXDIV_FINALDIV2SEL(1);
	temp |= SBI_SSCAUXDIV_FINALDIV2SEL(auxdiv);
	intel_sbi_write(dev_priv, SBI_SSCAUXDIV6, temp, SBI_ICLK);

	/* Enable modulator and associated divider */
	temp = intel_sbi_read(dev_priv, SBI_SSCCTL6, SBI_ICLK);
	temp &= ~SBI_SSCCTL_DISABLE;
	intel_sbi_write(dev_priv, SBI_SSCCTL6, temp, SBI_ICLK);

	mutex_unlock(&dev_priv->sb_lock);

	/* Wait for initialization time */
	udelay(24);

	intel_de_write(dev_priv, PIXCLK_GATE, PIXCLK_GATE_UNGATE);
}

int lpt_get_iclkip(struct drm_i915_private *dev_priv)
{
	u32 divsel, phaseinc, auxdiv;
	u32 iclk_virtual_root_freq = 172800 * 1000;
	u32 iclk_pi_range = 64;
	u32 desired_divisor;
	u32 temp;

	if ((intel_de_read(dev_priv, PIXCLK_GATE) & PIXCLK_GATE_UNGATE) == 0)
		return 0;

	mutex_lock(&dev_priv->sb_lock);

	temp = intel_sbi_read(dev_priv, SBI_SSCCTL6, SBI_ICLK);
	if (temp & SBI_SSCCTL_DISABLE) {
		mutex_unlock(&dev_priv->sb_lock);
		return 0;
	}

	temp = intel_sbi_read(dev_priv, SBI_SSCDIVINTPHASE6, SBI_ICLK);
	divsel = (temp & SBI_SSCDIVINTPHASE_DIVSEL_MASK) >>
		SBI_SSCDIVINTPHASE_DIVSEL_SHIFT;
	phaseinc = (temp & SBI_SSCDIVINTPHASE_INCVAL_MASK) >>
		SBI_SSCDIVINTPHASE_INCVAL_SHIFT;

	temp = intel_sbi_read(dev_priv, SBI_SSCAUXDIV6, SBI_ICLK);
	auxdiv = (temp & SBI_SSCAUXDIV_FINALDIV2SEL_MASK) >>
		SBI_SSCAUXDIV_FINALDIV2SEL_SHIFT;

	mutex_unlock(&dev_priv->sb_lock);

	desired_divisor = (divsel + 2) * iclk_pi_range + phaseinc;

	return DIV_ROUND_CLOSEST(iclk_virtual_root_freq,
				 desired_divisor << auxdiv);
}

static void ilk_pch_transcoder_set_timings(const struct intel_crtc_state *crtc_state,
					   enum pipe pch_transcoder)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	enum transcoder cpu_transcoder = crtc_state->cpu_transcoder;

	intel_de_write(dev_priv, PCH_TRANS_HTOTAL(pch_transcoder),
		       intel_de_read(dev_priv, HTOTAL(cpu_transcoder)));
	intel_de_write(dev_priv, PCH_TRANS_HBLANK(pch_transcoder),
		       intel_de_read(dev_priv, HBLANK(cpu_transcoder)));
	intel_de_write(dev_priv, PCH_TRANS_HSYNC(pch_transcoder),
		       intel_de_read(dev_priv, HSYNC(cpu_transcoder)));

	intel_de_write(dev_priv, PCH_TRANS_VTOTAL(pch_transcoder),
		       intel_de_read(dev_priv, VTOTAL(cpu_transcoder)));
	intel_de_write(dev_priv, PCH_TRANS_VBLANK(pch_transcoder),
		       intel_de_read(dev_priv, VBLANK(cpu_transcoder)));
	intel_de_write(dev_priv, PCH_TRANS_VSYNC(pch_transcoder),
		       intel_de_read(dev_priv, VSYNC(cpu_transcoder)));
	intel_de_write(dev_priv, PCH_TRANS_VSYNCSHIFT(pch_transcoder),
		       intel_de_read(dev_priv, VSYNCSHIFT(cpu_transcoder)));
}

static void cpt_set_fdi_bc_bifurcation(struct drm_i915_private *dev_priv, bool enable)
{
	u32 temp;

	temp = intel_de_read(dev_priv, SOUTH_CHICKEN1);
	if (!!(temp & FDI_BC_BIFURCATION_SELECT) == enable)
		return;

	drm_WARN_ON(&dev_priv->drm,
		    intel_de_read(dev_priv, FDI_RX_CTL(PIPE_B)) &
		    FDI_RX_ENABLE);
	drm_WARN_ON(&dev_priv->drm,
		    intel_de_read(dev_priv, FDI_RX_CTL(PIPE_C)) &
		    FDI_RX_ENABLE);

	temp &= ~FDI_BC_BIFURCATION_SELECT;
	if (enable)
		temp |= FDI_BC_BIFURCATION_SELECT;

	drm_dbg_kms(&dev_priv->drm, "%sabling fdi C rx\n",
		    enable ? "en" : "dis");
	intel_de_write(dev_priv, SOUTH_CHICKEN1, temp);
	intel_de_posting_read(dev_priv, SOUTH_CHICKEN1);
}

static void ivb_update_fdi_bc_bifurcation(const struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);

	switch (crtc->pipe) {
	case PIPE_A:
		break;
	case PIPE_B:
		if (crtc_state->fdi_lanes > 2)
			cpt_set_fdi_bc_bifurcation(dev_priv, false);
		else
			cpt_set_fdi_bc_bifurcation(dev_priv, true);

		break;
	case PIPE_C:
		cpt_set_fdi_bc_bifurcation(dev_priv, true);

		break;
	default:
		BUG();
	}
}

/*
 * Finds the encoder associated with the given CRTC. This can only be
 * used when we know that the CRTC isn't feeding multiple encoders!
 */
struct intel_encoder *
intel_get_crtc_new_encoder(const struct intel_atomic_state *state,
			   const struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	const struct drm_connector_state *connector_state;
	const struct drm_connector *connector;
	struct intel_encoder *encoder = NULL;
	int num_encoders = 0;
	int i;

	for_each_new_connector_in_state(&state->base, connector, connector_state, i) {
		if (connector_state->crtc != &crtc->base)
			continue;

		encoder = to_intel_encoder(connector_state->best_encoder);
		num_encoders++;
	}

	drm_WARN(encoder->base.dev, num_encoders != 1,
		 "%d encoders for pipe %c\n",
		 num_encoders, pipe_name(crtc->pipe));

	return encoder;
}

/*
 * Enable PCH resources required for PCH ports:
 *   - PCH PLLs
 *   - FDI training & RX/TX
 *   - update transcoder timings
 *   - DP transcoding bits
 *   - transcoder
 */
static void ilk_pch_enable(const struct intel_atomic_state *state,
			   const struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_device *dev = crtc->base.dev;
	struct drm_i915_private *dev_priv = to_i915(dev);
	enum pipe pipe = crtc->pipe;
	u32 temp;

	assert_pch_transcoder_disabled(dev_priv, pipe);

	if (IS_IVYBRIDGE(dev_priv))
		ivb_update_fdi_bc_bifurcation(crtc_state);

	/* Write the TU size bits before fdi link training, so that error
	 * detection works. */
	intel_de_write(dev_priv, FDI_RX_TUSIZE1(pipe),
		       intel_de_read(dev_priv, PIPE_DATA_M1(pipe)) & TU_SIZE_MASK);

	/* For PCH output, training FDI link */
	dev_priv->display.fdi_link_train(crtc, crtc_state);

	/* We need to program the right clock selection before writing the pixel
	 * mutliplier into the DPLL. */
	if (HAS_PCH_CPT(dev_priv)) {
		u32 sel;

		temp = intel_de_read(dev_priv, PCH_DPLL_SEL);
		temp |= TRANS_DPLL_ENABLE(pipe);
		sel = TRANS_DPLLB_SEL(pipe);
		if (crtc_state->shared_dpll ==
		    intel_get_shared_dpll_by_id(dev_priv, DPLL_ID_PCH_PLL_B))
			temp |= sel;
		else
			temp &= ~sel;
		intel_de_write(dev_priv, PCH_DPLL_SEL, temp);
	}

	/* XXX: pch pll's can be enabled any time before we enable the PCH
	 * transcoder, and we actually should do this to not upset any PCH
	 * transcoder that already use the clock when we share it.
	 *
	 * Note that enable_shared_dpll tries to do the right thing, but
	 * get_shared_dpll unconditionally resets the pll - we need that to have
	 * the right LVDS enable sequence. */
	intel_enable_shared_dpll(crtc_state);

	/* set transcoder timing, panel must allow it */
	assert_panel_unlocked(dev_priv, pipe);
	ilk_pch_transcoder_set_timings(crtc_state, pipe);

	intel_fdi_normal_train(crtc);

	/* For PCH DP, enable TRANS_DP_CTL */
	if (HAS_PCH_CPT(dev_priv) &&
	    intel_crtc_has_dp_encoder(crtc_state)) {
		const struct drm_display_mode *adjusted_mode =
			&crtc_state->hw.adjusted_mode;
		u32 bpc = (intel_de_read(dev_priv, PIPECONF(pipe)) & PIPECONF_BPC_MASK) >> 5;
		i915_reg_t reg = TRANS_DP_CTL(pipe);
		enum port port;

		temp = intel_de_read(dev_priv, reg);
		temp &= ~(TRANS_DP_PORT_SEL_MASK |
			  TRANS_DP_SYNC_MASK |
			  TRANS_DP_BPC_MASK);
		temp |= TRANS_DP_OUTPUT_ENABLE;
		temp |= bpc << 9; /* same format but at 11:9 */

		if (adjusted_mode->flags & DRM_MODE_FLAG_PHSYNC)
			temp |= TRANS_DP_HSYNC_ACTIVE_HIGH;
		if (adjusted_mode->flags & DRM_MODE_FLAG_PVSYNC)
			temp |= TRANS_DP_VSYNC_ACTIVE_HIGH;

		port = intel_get_crtc_new_encoder(state, crtc_state)->port;
		drm_WARN_ON(dev, port < PORT_B || port > PORT_D);
		temp |= TRANS_DP_PORT_SEL(port);

		intel_de_write(dev_priv, reg, temp);
	}

	ilk_enable_pch_transcoder(crtc_state);
}

void lpt_pch_enable(const struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	enum transcoder cpu_transcoder = crtc_state->cpu_transcoder;

	assert_pch_transcoder_disabled(dev_priv, PIPE_A);

	lpt_program_iclkip(crtc_state);

	/* Set transcoder timing. */
	ilk_pch_transcoder_set_timings(crtc_state, PIPE_A);

	lpt_enable_pch_transcoder(dev_priv, cpu_transcoder);
}

static void cpt_verify_modeset(struct drm_i915_private *dev_priv,
			       enum pipe pipe)
{
	i915_reg_t dslreg = PIPEDSL(pipe);
	u32 temp;

	temp = intel_de_read(dev_priv, dslreg);
	udelay(500);
	if (wait_for(intel_de_read(dev_priv, dslreg) != temp, 5)) {
		if (wait_for(intel_de_read(dev_priv, dslreg) != temp, 5))
			drm_err(&dev_priv->drm,
				"mode set failed: pipe %c stuck\n",
				pipe_name(pipe));
	}
}

static void ilk_pfit_enable(const struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	const struct drm_rect *dst = &crtc_state->pch_pfit.dst;
	enum pipe pipe = crtc->pipe;
	int width = drm_rect_width(dst);
	int height = drm_rect_height(dst);
	int x = dst->x1;
	int y = dst->y1;

	if (!crtc_state->pch_pfit.enabled)
		return;

	/* Force use of hard-coded filter coefficients
	 * as some pre-programmed values are broken,
	 * e.g. x201.
	 */
	if (IS_IVYBRIDGE(dev_priv) || IS_HASWELL(dev_priv))
		intel_de_write(dev_priv, PF_CTL(pipe), PF_ENABLE |
			       PF_FILTER_MED_3x3 | PF_PIPE_SEL_IVB(pipe));
	else
		intel_de_write(dev_priv, PF_CTL(pipe), PF_ENABLE |
			       PF_FILTER_MED_3x3);
	intel_de_write(dev_priv, PF_WIN_POS(pipe), x << 16 | y);
	intel_de_write(dev_priv, PF_WIN_SZ(pipe), width << 16 | height);
}

void hsw_enable_ips(const struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_device *dev = crtc->base.dev;
	struct drm_i915_private *dev_priv = to_i915(dev);

	if (!crtc_state->ips_enabled)
		return;

	/*
	 * We can only enable IPS after we enable a plane and wait for a vblank
	 * This function is called from post_plane_update, which is run after
	 * a vblank wait.
	 */
	drm_WARN_ON(dev, !(crtc_state->active_planes & ~BIT(PLANE_CURSOR)));

	if (IS_BROADWELL(dev_priv)) {
		drm_WARN_ON(dev, sandybridge_pcode_write(dev_priv, DISPLAY_IPS_CONTROL,
							 IPS_ENABLE | IPS_PCODE_CONTROL));
		/* Quoting Art Runyan: "its not safe to expect any particular
		 * value in IPS_CTL bit 31 after enabling IPS through the
		 * mailbox." Moreover, the mailbox may return a bogus state,
		 * so we need to just enable it and continue on.
		 */
	} else {
		intel_de_write(dev_priv, IPS_CTL, IPS_ENABLE);
		/* The bit only becomes 1 in the next vblank, so this wait here
		 * is essentially intel_wait_for_vblank. If we don't have this
		 * and don't wait for vblanks until the end of crtc_enable, then
		 * the HW state readout code will complain that the expected
		 * IPS_CTL value is not the one we read. */
		if (intel_de_wait_for_set(dev_priv, IPS_CTL, IPS_ENABLE, 50))
			drm_err(&dev_priv->drm,
				"Timed out waiting for IPS enable\n");
	}
}

void hsw_disable_ips(const struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_device *dev = crtc->base.dev;
	struct drm_i915_private *dev_priv = to_i915(dev);

	if (!crtc_state->ips_enabled)
		return;

	if (IS_BROADWELL(dev_priv)) {
		drm_WARN_ON(dev,
			    sandybridge_pcode_write(dev_priv, DISPLAY_IPS_CONTROL, 0));
		/*
		 * Wait for PCODE to finish disabling IPS. The BSpec specified
		 * 42ms timeout value leads to occasional timeouts so use 100ms
		 * instead.
		 */
		if (intel_de_wait_for_clear(dev_priv, IPS_CTL, IPS_ENABLE, 100))
			drm_err(&dev_priv->drm,
				"Timed out waiting for IPS disable\n");
	} else {
		intel_de_write(dev_priv, IPS_CTL, 0);
		intel_de_posting_read(dev_priv, IPS_CTL);
	}

	/* We need to wait for a vblank before we can disable the plane. */
	intel_wait_for_vblank(dev_priv, crtc->pipe);
}

static void intel_crtc_dpms_overlay_disable(struct intel_crtc *intel_crtc)
{
	if (intel_crtc->overlay)
		(void) intel_overlay_switch_off(intel_crtc->overlay);

	/* Let userspace switch the overlay on again. In most cases userspace
	 * has to recompute where to put it anyway.
	 */
}

static bool hsw_pre_update_disable_ips(const struct intel_crtc_state *old_crtc_state,
				       const struct intel_crtc_state *new_crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(new_crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);

	if (!old_crtc_state->ips_enabled)
		return false;

	if (intel_crtc_needs_modeset(new_crtc_state))
		return true;

	/*
	 * Workaround : Do not read or write the pipe palette/gamma data while
	 * GAMMA_MODE is configured for split gamma and IPS_CTL has IPS enabled.
	 *
	 * Disable IPS before we program the LUT.
	 */
	if (IS_HASWELL(dev_priv) &&
	    (new_crtc_state->uapi.color_mgmt_changed ||
	     new_crtc_state->update_pipe) &&
	    new_crtc_state->gamma_mode == GAMMA_MODE_MODE_SPLIT)
		return true;

	return !new_crtc_state->ips_enabled;
}

static bool hsw_post_update_enable_ips(const struct intel_crtc_state *old_crtc_state,
				       const struct intel_crtc_state *new_crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(new_crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);

	if (!new_crtc_state->ips_enabled)
		return false;

	if (intel_crtc_needs_modeset(new_crtc_state))
		return true;

	/*
	 * Workaround : Do not read or write the pipe palette/gamma data while
	 * GAMMA_MODE is configured for split gamma and IPS_CTL has IPS enabled.
	 *
	 * Re-enable IPS after the LUT has been programmed.
	 */
	if (IS_HASWELL(dev_priv) &&
	    (new_crtc_state->uapi.color_mgmt_changed ||
	     new_crtc_state->update_pipe) &&
	    new_crtc_state->gamma_mode == GAMMA_MODE_MODE_SPLIT)
		return true;

	/*
	 * We can't read out IPS on broadwell, assume the worst and
	 * forcibly enable IPS on the first fastset.
	 */
	if (new_crtc_state->update_pipe && old_crtc_state->inherited)
		return true;

	return !old_crtc_state->ips_enabled;
}

static bool needs_nv12_wa(const struct intel_crtc_state *crtc_state)
{
	struct drm_i915_private *dev_priv = to_i915(crtc_state->uapi.crtc->dev);

	if (!crtc_state->nv12_planes)
		return false;

	/* WA Display #0827: Gen9:all */
	if (DISPLAY_VER(dev_priv) == 9)
		return true;

	return false;
}

static bool needs_scalerclk_wa(const struct intel_crtc_state *crtc_state)
{
	struct drm_i915_private *dev_priv = to_i915(crtc_state->uapi.crtc->dev);

	/* Wa_2006604312:icl,ehl */
	if (crtc_state->scaler_state.scaler_users > 0 && DISPLAY_VER(dev_priv) == 11)
		return true;

	return false;
}

static bool planes_enabling(const struct intel_crtc_state *old_crtc_state,
			    const struct intel_crtc_state *new_crtc_state)
{
	return (!old_crtc_state->active_planes || intel_crtc_needs_modeset(new_crtc_state)) &&
		new_crtc_state->active_planes;
}

static bool planes_disabling(const struct intel_crtc_state *old_crtc_state,
			     const struct intel_crtc_state *new_crtc_state)
{
	return old_crtc_state->active_planes &&
		(!new_crtc_state->active_planes || intel_crtc_needs_modeset(new_crtc_state));
}

static void intel_post_plane_update(struct intel_atomic_state *state,
				    struct intel_crtc *crtc)
{
	struct drm_i915_private *dev_priv = to_i915(state->base.dev);
	const struct intel_crtc_state *old_crtc_state =
		intel_atomic_get_old_crtc_state(state, crtc);
	const struct intel_crtc_state *new_crtc_state =
		intel_atomic_get_new_crtc_state(state, crtc);
	enum pipe pipe = crtc->pipe;

	intel_frontbuffer_flip(dev_priv, new_crtc_state->fb_bits);

	if (new_crtc_state->update_wm_post && new_crtc_state->hw.active)
		intel_update_watermarks(crtc);

	if (hsw_post_update_enable_ips(old_crtc_state, new_crtc_state))
		hsw_enable_ips(new_crtc_state);

	intel_fbc_post_update(state, crtc);

	if (needs_nv12_wa(old_crtc_state) &&
	    !needs_nv12_wa(new_crtc_state))
		skl_wa_827(dev_priv, pipe, false);

	if (needs_scalerclk_wa(old_crtc_state) &&
	    !needs_scalerclk_wa(new_crtc_state))
		icl_wa_scalerclkgating(dev_priv, pipe, false);
}

static void intel_crtc_enable_flip_done(struct intel_atomic_state *state,
					struct intel_crtc *crtc)
{
	const struct intel_crtc_state *crtc_state =
		intel_atomic_get_new_crtc_state(state, crtc);
	u8 update_planes = crtc_state->update_planes;
	const struct intel_plane_state *plane_state;
	struct intel_plane *plane;
	int i;

	for_each_new_intel_plane_in_state(state, plane, plane_state, i) {
		if (plane->enable_flip_done &&
		    plane->pipe == crtc->pipe &&
		    update_planes & BIT(plane->id))
			plane->enable_flip_done(plane);
	}
}

static void intel_crtc_disable_flip_done(struct intel_atomic_state *state,
					 struct intel_crtc *crtc)
{
	const struct intel_crtc_state *crtc_state =
		intel_atomic_get_new_crtc_state(state, crtc);
	u8 update_planes = crtc_state->update_planes;
	const struct intel_plane_state *plane_state;
	struct intel_plane *plane;
	int i;

	for_each_new_intel_plane_in_state(state, plane, plane_state, i) {
		if (plane->disable_flip_done &&
		    plane->pipe == crtc->pipe &&
		    update_planes & BIT(plane->id))
			plane->disable_flip_done(plane);
	}
}

static void intel_crtc_async_flip_disable_wa(struct intel_atomic_state *state,
					     struct intel_crtc *crtc)
{
	struct drm_i915_private *i915 = to_i915(state->base.dev);
	const struct intel_crtc_state *old_crtc_state =
		intel_atomic_get_old_crtc_state(state, crtc);
	const struct intel_crtc_state *new_crtc_state =
		intel_atomic_get_new_crtc_state(state, crtc);
	u8 update_planes = new_crtc_state->update_planes;
	const struct intel_plane_state *old_plane_state;
	struct intel_plane *plane;
	bool need_vbl_wait = false;
	int i;

	for_each_old_intel_plane_in_state(state, plane, old_plane_state, i) {
		if (plane->need_async_flip_disable_wa &&
		    plane->pipe == crtc->pipe &&
		    update_planes & BIT(plane->id)) {
			/*
			 * Apart from the async flip bit we want to
			 * preserve the old state for the plane.
			 */
			plane->async_flip(plane, old_crtc_state,
					  old_plane_state, false);
			need_vbl_wait = true;
		}
	}

	if (need_vbl_wait)
		intel_wait_for_vblank(i915, crtc->pipe);
}

static void intel_pre_plane_update(struct intel_atomic_state *state,
				   struct intel_crtc *crtc)
{
	struct drm_i915_private *dev_priv = to_i915(state->base.dev);
	const struct intel_crtc_state *old_crtc_state =
		intel_atomic_get_old_crtc_state(state, crtc);
	const struct intel_crtc_state *new_crtc_state =
		intel_atomic_get_new_crtc_state(state, crtc);
	enum pipe pipe = crtc->pipe;

	if (hsw_pre_update_disable_ips(old_crtc_state, new_crtc_state))
		hsw_disable_ips(old_crtc_state);

	if (intel_fbc_pre_update(state, crtc))
		intel_wait_for_vblank(dev_priv, pipe);

	/* Display WA 827 */
	if (!needs_nv12_wa(old_crtc_state) &&
	    needs_nv12_wa(new_crtc_state))
		skl_wa_827(dev_priv, pipe, true);

	/* Wa_2006604312:icl,ehl */
	if (!needs_scalerclk_wa(old_crtc_state) &&
	    needs_scalerclk_wa(new_crtc_state))
		icl_wa_scalerclkgating(dev_priv, pipe, true);

	/*
	 * Vblank time updates from the shadow to live plane control register
	 * are blocked if the memory self-refresh mode is active at that
	 * moment. So to make sure the plane gets truly disabled, disable
	 * first the self-refresh mode. The self-refresh enable bit in turn
	 * will be checked/applied by the HW only at the next frame start
	 * event which is after the vblank start event, so we need to have a
	 * wait-for-vblank between disabling the plane and the pipe.
	 */
	if (HAS_GMCH(dev_priv) && old_crtc_state->hw.active &&
	    new_crtc_state->disable_cxsr && intel_set_memory_cxsr(dev_priv, false))
		intel_wait_for_vblank(dev_priv, pipe);

	/*
	 * IVB workaround: must disable low power watermarks for at least
	 * one frame before enabling scaling.  LP watermarks can be re-enabled
	 * when scaling is disabled.
	 *
	 * WaCxSRDisabledForSpriteScaling:ivb
	 */
	if (old_crtc_state->hw.active &&
	    new_crtc_state->disable_lp_wm && ilk_disable_lp_wm(dev_priv))
		intel_wait_for_vblank(dev_priv, pipe);

	/*
	 * If we're doing a modeset we don't need to do any
	 * pre-vblank watermark programming here.
	 */
	if (!intel_crtc_needs_modeset(new_crtc_state)) {
		/*
		 * For platforms that support atomic watermarks, program the
		 * 'intermediate' watermarks immediately.  On pre-gen9 platforms, these
		 * will be the intermediate values that are safe for both pre- and
		 * post- vblank; when vblank happens, the 'active' values will be set
		 * to the final 'target' values and we'll do this again to get the
		 * optimal watermarks.  For gen9+ platforms, the values we program here
		 * will be the final target values which will get automatically latched
		 * at vblank time; no further programming will be necessary.
		 *
		 * If a platform hasn't been transitioned to atomic watermarks yet,
		 * we'll continue to update watermarks the old way, if flags tell
		 * us to.
		 */
		if (dev_priv->display.initial_watermarks)
			dev_priv->display.initial_watermarks(state, crtc);
		else if (new_crtc_state->update_wm_pre)
			intel_update_watermarks(crtc);
	}

	/*
	 * Gen2 reports pipe underruns whenever all planes are disabled.
	 * So disable underrun reporting before all the planes get disabled.
	 *
	 * We do this after .initial_watermarks() so that we have a
	 * chance of catching underruns with the intermediate watermarks
	 * vs. the old plane configuration.
	 */
	if (DISPLAY_VER(dev_priv) == 2 && planes_disabling(old_crtc_state, new_crtc_state))
		intel_set_cpu_fifo_underrun_reporting(dev_priv, pipe, false);

	/*
	 * WA for platforms where async address update enable bit
	 * is double buffered and only latched at start of vblank.
	 */
	if (old_crtc_state->uapi.async_flip && !new_crtc_state->uapi.async_flip)
		intel_crtc_async_flip_disable_wa(state, crtc);
}

static void intel_crtc_disable_planes(struct intel_atomic_state *state,
				      struct intel_crtc *crtc)
{
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	const struct intel_crtc_state *new_crtc_state =
		intel_atomic_get_new_crtc_state(state, crtc);
	unsigned int update_mask = new_crtc_state->update_planes;
	const struct intel_plane_state *old_plane_state;
	struct intel_plane *plane;
	unsigned fb_bits = 0;
	int i;

	intel_crtc_dpms_overlay_disable(crtc);

	for_each_old_intel_plane_in_state(state, plane, old_plane_state, i) {
		if (crtc->pipe != plane->pipe ||
		    !(update_mask & BIT(plane->id)))
			continue;

		intel_disable_plane(plane, new_crtc_state);

		if (old_plane_state->uapi.visible)
			fb_bits |= plane->frontbuffer_bit;
	}

	intel_frontbuffer_flip(dev_priv, fb_bits);
}

/*
 * intel_connector_primary_encoder - get the primary encoder for a connector
 * @connector: connector for which to return the encoder
 *
 * Returns the primary encoder for a connector. There is a 1:1 mapping from
 * all connectors to their encoder, except for DP-MST connectors which have
 * both a virtual and a primary encoder. These DP-MST primary encoders can be
 * pointed to by as many DP-MST connectors as there are pipes.
 */
static struct intel_encoder *
intel_connector_primary_encoder(struct intel_connector *connector)
{
	struct intel_encoder *encoder;

	if (connector->mst_port)
		return &dp_to_dig_port(connector->mst_port)->base;

	encoder = intel_attached_encoder(connector);
	drm_WARN_ON(connector->base.dev, !encoder);

	return encoder;
}

static void intel_encoders_update_prepare(struct intel_atomic_state *state)
{
	struct drm_connector_state *new_conn_state;
	struct drm_connector *connector;
	int i;

	for_each_new_connector_in_state(&state->base, connector, new_conn_state,
					i) {
		struct intel_connector *intel_connector;
		struct intel_encoder *encoder;
		struct intel_crtc *crtc;

		if (!intel_connector_needs_modeset(state, connector))
			continue;

		intel_connector = to_intel_connector(connector);
		encoder = intel_connector_primary_encoder(intel_connector);
		if (!encoder->update_prepare)
			continue;

		crtc = new_conn_state->crtc ?
			to_intel_crtc(new_conn_state->crtc) : NULL;
		encoder->update_prepare(state, encoder, crtc);
	}
}

static void intel_encoders_update_complete(struct intel_atomic_state *state)
{
	struct drm_connector_state *new_conn_state;
	struct drm_connector *connector;
	int i;

	for_each_new_connector_in_state(&state->base, connector, new_conn_state,
					i) {
		struct intel_connector *intel_connector;
		struct intel_encoder *encoder;
		struct intel_crtc *crtc;

		if (!intel_connector_needs_modeset(state, connector))
			continue;

		intel_connector = to_intel_connector(connector);
		encoder = intel_connector_primary_encoder(intel_connector);
		if (!encoder->update_complete)
			continue;

		crtc = new_conn_state->crtc ?
			to_intel_crtc(new_conn_state->crtc) : NULL;
		encoder->update_complete(state, encoder, crtc);
	}
}

static void intel_encoders_pre_pll_enable(struct intel_atomic_state *state,
					  struct intel_crtc *crtc)
{
	const struct intel_crtc_state *crtc_state =
		intel_atomic_get_new_crtc_state(state, crtc);
	const struct drm_connector_state *conn_state;
	struct drm_connector *conn;
	int i;

	for_each_new_connector_in_state(&state->base, conn, conn_state, i) {
		struct intel_encoder *encoder =
			to_intel_encoder(conn_state->best_encoder);

		if (conn_state->crtc != &crtc->base)
			continue;

		if (encoder->pre_pll_enable)
			encoder->pre_pll_enable(state, encoder,
						crtc_state, conn_state);
	}
}

static void intel_encoders_pre_enable(struct intel_atomic_state *state,
				      struct intel_crtc *crtc)
{
	const struct intel_crtc_state *crtc_state =
		intel_atomic_get_new_crtc_state(state, crtc);
	const struct drm_connector_state *conn_state;
	struct drm_connector *conn;
	int i;

	for_each_new_connector_in_state(&state->base, conn, conn_state, i) {
		struct intel_encoder *encoder =
			to_intel_encoder(conn_state->best_encoder);

		if (conn_state->crtc != &crtc->base)
			continue;

		if (encoder->pre_enable)
			encoder->pre_enable(state, encoder,
					    crtc_state, conn_state);
	}
}

static void intel_encoders_enable(struct intel_atomic_state *state,
				  struct intel_crtc *crtc)
{
	const struct intel_crtc_state *crtc_state =
		intel_atomic_get_new_crtc_state(state, crtc);
	const struct drm_connector_state *conn_state;
	struct drm_connector *conn;
	int i;

	for_each_new_connector_in_state(&state->base, conn, conn_state, i) {
		struct intel_encoder *encoder =
			to_intel_encoder(conn_state->best_encoder);

		if (conn_state->crtc != &crtc->base)
			continue;

		if (encoder->enable)
			encoder->enable(state, encoder,
					crtc_state, conn_state);
		intel_opregion_notify_encoder(encoder, true);
	}
}

static void intel_encoders_disable(struct intel_atomic_state *state,
				   struct intel_crtc *crtc)
{
	const struct intel_crtc_state *old_crtc_state =
		intel_atomic_get_old_crtc_state(state, crtc);
	const struct drm_connector_state *old_conn_state;
	struct drm_connector *conn;
	int i;

	for_each_old_connector_in_state(&state->base, conn, old_conn_state, i) {
		struct intel_encoder *encoder =
			to_intel_encoder(old_conn_state->best_encoder);

		if (old_conn_state->crtc != &crtc->base)
			continue;

		intel_opregion_notify_encoder(encoder, false);
		if (encoder->disable)
			encoder->disable(state, encoder,
					 old_crtc_state, old_conn_state);
	}
}

static void intel_encoders_post_disable(struct intel_atomic_state *state,
					struct intel_crtc *crtc)
{
	const struct intel_crtc_state *old_crtc_state =
		intel_atomic_get_old_crtc_state(state, crtc);
	const struct drm_connector_state *old_conn_state;
	struct drm_connector *conn;
	int i;

	for_each_old_connector_in_state(&state->base, conn, old_conn_state, i) {
		struct intel_encoder *encoder =
			to_intel_encoder(old_conn_state->best_encoder);

		if (old_conn_state->crtc != &crtc->base)
			continue;

		if (encoder->post_disable)
			encoder->post_disable(state, encoder,
					      old_crtc_state, old_conn_state);
	}
}

static void intel_encoders_post_pll_disable(struct intel_atomic_state *state,
					    struct intel_crtc *crtc)
{
	const struct intel_crtc_state *old_crtc_state =
		intel_atomic_get_old_crtc_state(state, crtc);
	const struct drm_connector_state *old_conn_state;
	struct drm_connector *conn;
	int i;

	for_each_old_connector_in_state(&state->base, conn, old_conn_state, i) {
		struct intel_encoder *encoder =
			to_intel_encoder(old_conn_state->best_encoder);

		if (old_conn_state->crtc != &crtc->base)
			continue;

		if (encoder->post_pll_disable)
			encoder->post_pll_disable(state, encoder,
						  old_crtc_state, old_conn_state);
	}
}

static void intel_encoders_update_pipe(struct intel_atomic_state *state,
				       struct intel_crtc *crtc)
{
	const struct intel_crtc_state *crtc_state =
		intel_atomic_get_new_crtc_state(state, crtc);
	const struct drm_connector_state *conn_state;
	struct drm_connector *conn;
	int i;

	for_each_new_connector_in_state(&state->base, conn, conn_state, i) {
		struct intel_encoder *encoder =
			to_intel_encoder(conn_state->best_encoder);

		if (conn_state->crtc != &crtc->base)
			continue;

		if (encoder->update_pipe)
			encoder->update_pipe(state, encoder,
					     crtc_state, conn_state);
	}
}

static void intel_disable_primary_plane(const struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct intel_plane *plane = to_intel_plane(crtc->base.primary);

	plane->disable_plane(plane, crtc_state);
}

static void ilk_crtc_enable(struct intel_atomic_state *state,
			    struct intel_crtc *crtc)
{
	const struct intel_crtc_state *new_crtc_state =
		intel_atomic_get_new_crtc_state(state, crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	enum pipe pipe = crtc->pipe;

	if (drm_WARN_ON(&dev_priv->drm, crtc->active))
		return;

	/*
	 * Sometimes spurious CPU pipe underruns happen during FDI
	 * training, at least with VGA+HDMI cloning. Suppress them.
	 *
	 * On ILK we get an occasional spurious CPU pipe underruns
	 * between eDP port A enable and vdd enable. Also PCH port
	 * enable seems to result in the occasional CPU pipe underrun.
	 *
	 * Spurious PCH underruns also occur during PCH enabling.
	 */
	intel_set_cpu_fifo_underrun_reporting(dev_priv, pipe, false);
	intel_set_pch_fifo_underrun_reporting(dev_priv, pipe, false);

	if (new_crtc_state->has_pch_encoder)
		intel_prepare_shared_dpll(new_crtc_state);

	if (intel_crtc_has_dp_encoder(new_crtc_state))
		intel_dp_set_m_n(new_crtc_state, M1_N1);

	intel_set_transcoder_timings(new_crtc_state);
	intel_set_pipe_src_size(new_crtc_state);

	if (new_crtc_state->has_pch_encoder)
		intel_cpu_transcoder_set_m_n(new_crtc_state,
					     &new_crtc_state->fdi_m_n, NULL);

	ilk_set_pipeconf(new_crtc_state);

	crtc->active = true;

	intel_encoders_pre_enable(state, crtc);

	if (new_crtc_state->has_pch_encoder) {
		/* Note: FDI PLL enabling _must_ be done before we enable the
		 * cpu pipes, hence this is separate from all the other fdi/pch
		 * enabling. */
		ilk_fdi_pll_enable(new_crtc_state);
	} else {
		assert_fdi_tx_disabled(dev_priv, pipe);
		assert_fdi_rx_disabled(dev_priv, pipe);
	}

	ilk_pfit_enable(new_crtc_state);

	/*
	 * On ILK+ LUT must be loaded before the pipe is running but with
	 * clocks enabled
	 */
	intel_color_load_luts(new_crtc_state);
	intel_color_commit(new_crtc_state);
	/* update DSPCNTR to configure gamma for pipe bottom color */
	intel_disable_primary_plane(new_crtc_state);

	if (dev_priv->display.initial_watermarks)
		dev_priv->display.initial_watermarks(state, crtc);
	intel_enable_pipe(new_crtc_state);

	if (new_crtc_state->has_pch_encoder)
		ilk_pch_enable(state, new_crtc_state);

	intel_crtc_vblank_on(new_crtc_state);

	intel_encoders_enable(state, crtc);

	if (HAS_PCH_CPT(dev_priv))
		cpt_verify_modeset(dev_priv, pipe);

	/*
	 * Must wait for vblank to avoid spurious PCH FIFO underruns.
	 * And a second vblank wait is needed at least on ILK with
	 * some interlaced HDMI modes. Let's do the double wait always
	 * in case there are more corner cases we don't know about.
	 */
	if (new_crtc_state->has_pch_encoder) {
		intel_wait_for_vblank(dev_priv, pipe);
		intel_wait_for_vblank(dev_priv, pipe);
	}
	intel_set_cpu_fifo_underrun_reporting(dev_priv, pipe, true);
	intel_set_pch_fifo_underrun_reporting(dev_priv, pipe, true);
}

/* IPS only exists on ULT machines and is tied to pipe A. */
static bool hsw_crtc_supports_ips(struct intel_crtc *crtc)
{
	return HAS_IPS(to_i915(crtc->base.dev)) && crtc->pipe == PIPE_A;
}

static void glk_pipe_scaler_clock_gating_wa(struct drm_i915_private *dev_priv,
					    enum pipe pipe, bool apply)
{
	u32 val = intel_de_read(dev_priv, CLKGATE_DIS_PSL(pipe));
	u32 mask = DPF_GATING_DIS | DPF_RAM_GATING_DIS | DPFR_GATING_DIS;

	if (apply)
		val |= mask;
	else
		val &= ~mask;

	intel_de_write(dev_priv, CLKGATE_DIS_PSL(pipe), val);
}

static void icl_pipe_mbus_enable(struct intel_crtc *crtc)
{
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	enum pipe pipe = crtc->pipe;
	u32 val;

	val = MBUS_DBOX_A_CREDIT(2);

	if (DISPLAY_VER(dev_priv) >= 12) {
		val |= MBUS_DBOX_BW_CREDIT(2);
		val |= MBUS_DBOX_B_CREDIT(12);
	} else {
		val |= MBUS_DBOX_BW_CREDIT(1);
		val |= MBUS_DBOX_B_CREDIT(8);
	}

	intel_de_write(dev_priv, PIPE_MBUS_DBOX_CTL(pipe), val);
}

static void hsw_set_linetime_wm(const struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);

	intel_de_write(dev_priv, WM_LINETIME(crtc->pipe),
		       HSW_LINETIME(crtc_state->linetime) |
		       HSW_IPS_LINETIME(crtc_state->ips_linetime));
}

static void hsw_set_frame_start_delay(const struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	i915_reg_t reg = CHICKEN_TRANS(crtc_state->cpu_transcoder);
	u32 val;

	val = intel_de_read(dev_priv, reg);
	val &= ~HSW_FRAME_START_DELAY_MASK;
	val |= HSW_FRAME_START_DELAY(dev_priv->framestart_delay - 1);
	intel_de_write(dev_priv, reg, val);
}

static void icl_ddi_bigjoiner_pre_enable(struct intel_atomic_state *state,
					 const struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *master = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(master->base.dev);
	struct intel_crtc_state *master_crtc_state;
	struct drm_connector_state *conn_state;
	struct drm_connector *conn;
	struct intel_encoder *encoder = NULL;
	int i;

	if (crtc_state->bigjoiner_slave)
		master = crtc_state->bigjoiner_linked_crtc;

	master_crtc_state = intel_atomic_get_new_crtc_state(state, master);

	for_each_new_connector_in_state(&state->base, conn, conn_state, i) {
		if (conn_state->crtc != &master->base)
			continue;

		encoder = to_intel_encoder(conn_state->best_encoder);
		break;
	}

	if (!crtc_state->bigjoiner_slave) {
		/* need to enable VDSC, which we skipped in pre-enable */
		intel_dsc_enable(encoder, crtc_state);
	} else {
		/*
		 * Enable sequence steps 1-7 on bigjoiner master
		 */
		intel_encoders_pre_pll_enable(state, master);
		intel_enable_shared_dpll(master_crtc_state);
		intel_encoders_pre_enable(state, master);

		/* and DSC on slave */
		intel_dsc_enable(NULL, crtc_state);
	}

	if (DISPLAY_VER(dev_priv) >= 13)
		intel_uncompressed_joiner_enable(crtc_state);
}

static void hsw_crtc_enable(struct intel_atomic_state *state,
			    struct intel_crtc *crtc)
{
	const struct intel_crtc_state *new_crtc_state =
		intel_atomic_get_new_crtc_state(state, crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	enum pipe pipe = crtc->pipe, hsw_workaround_pipe;
	enum transcoder cpu_transcoder = new_crtc_state->cpu_transcoder;
	bool psl_clkgate_wa;

	if (drm_WARN_ON(&dev_priv->drm, crtc->active))
		return;

	if (!new_crtc_state->bigjoiner) {
		intel_encoders_pre_pll_enable(state, crtc);

		if (new_crtc_state->shared_dpll)
			intel_enable_shared_dpll(new_crtc_state);

		intel_encoders_pre_enable(state, crtc);
	} else {
		icl_ddi_bigjoiner_pre_enable(state, new_crtc_state);
	}

	intel_set_pipe_src_size(new_crtc_state);
	if (DISPLAY_VER(dev_priv) >= 9 || IS_BROADWELL(dev_priv))
		bdw_set_pipemisc(new_crtc_state);

	if (!new_crtc_state->bigjoiner_slave && !transcoder_is_dsi(cpu_transcoder)) {
		intel_set_transcoder_timings(new_crtc_state);

		if (cpu_transcoder != TRANSCODER_EDP)
			intel_de_write(dev_priv, PIPE_MULT(cpu_transcoder),
				       new_crtc_state->pixel_multiplier - 1);

		if (new_crtc_state->has_pch_encoder)
			intel_cpu_transcoder_set_m_n(new_crtc_state,
						     &new_crtc_state->fdi_m_n, NULL);

		hsw_set_frame_start_delay(new_crtc_state);
	}

	if (!transcoder_is_dsi(cpu_transcoder))
		hsw_set_pipeconf(new_crtc_state);

	crtc->active = true;

	/* Display WA #1180: WaDisableScalarClockGating: glk, cnl */
	psl_clkgate_wa = DISPLAY_VER(dev_priv) == 10 &&
		new_crtc_state->pch_pfit.enabled;
	if (psl_clkgate_wa)
		glk_pipe_scaler_clock_gating_wa(dev_priv, pipe, true);

	if (DISPLAY_VER(dev_priv) >= 9)
		skl_pfit_enable(new_crtc_state);
	else
		ilk_pfit_enable(new_crtc_state);

	/*
	 * On ILK+ LUT must be loaded before the pipe is running but with
	 * clocks enabled
	 */
	intel_color_load_luts(new_crtc_state);
	intel_color_commit(new_crtc_state);
	/* update DSPCNTR to configure gamma/csc for pipe bottom color */
	if (DISPLAY_VER(dev_priv) < 9)
		intel_disable_primary_plane(new_crtc_state);

	hsw_set_linetime_wm(new_crtc_state);

	if (DISPLAY_VER(dev_priv) >= 11)
		icl_set_pipe_chicken(crtc);

	if (dev_priv->display.initial_watermarks)
		dev_priv->display.initial_watermarks(state, crtc);

	if (DISPLAY_VER(dev_priv) >= 11)
		icl_pipe_mbus_enable(crtc);

	if (new_crtc_state->bigjoiner_slave)
		intel_crtc_vblank_on(new_crtc_state);

	intel_encoders_enable(state, crtc);

	if (psl_clkgate_wa) {
		intel_wait_for_vblank(dev_priv, pipe);
		glk_pipe_scaler_clock_gating_wa(dev_priv, pipe, false);
	}

	/* If we change the relative order between pipe/planes enabling, we need
	 * to change the workaround. */
	hsw_workaround_pipe = new_crtc_state->hsw_workaround_pipe;
	if (IS_HASWELL(dev_priv) && hsw_workaround_pipe != INVALID_PIPE) {
		intel_wait_for_vblank(dev_priv, hsw_workaround_pipe);
		intel_wait_for_vblank(dev_priv, hsw_workaround_pipe);
	}
}

void ilk_pfit_disable(const struct intel_crtc_state *old_crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(old_crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	enum pipe pipe = crtc->pipe;

	/* To avoid upsetting the power well on haswell only disable the pfit if
	 * it's in use. The hw state code will make sure we get this right. */
	if (!old_crtc_state->pch_pfit.enabled)
		return;

	intel_de_write(dev_priv, PF_CTL(pipe), 0);
	intel_de_write(dev_priv, PF_WIN_POS(pipe), 0);
	intel_de_write(dev_priv, PF_WIN_SZ(pipe), 0);
}

static void ilk_crtc_disable(struct intel_atomic_state *state,
			     struct intel_crtc *crtc)
{
	const struct intel_crtc_state *old_crtc_state =
		intel_atomic_get_old_crtc_state(state, crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	enum pipe pipe = crtc->pipe;

	/*
	 * Sometimes spurious CPU pipe underruns happen when the
	 * pipe is already disabled, but FDI RX/TX is still enabled.
	 * Happens at least with VGA+HDMI cloning. Suppress them.
	 */
	intel_set_cpu_fifo_underrun_reporting(dev_priv, pipe, false);
	intel_set_pch_fifo_underrun_reporting(dev_priv, pipe, false);

	intel_encoders_disable(state, crtc);

	intel_crtc_vblank_off(old_crtc_state);

	intel_disable_pipe(old_crtc_state);

	ilk_pfit_disable(old_crtc_state);

	if (old_crtc_state->has_pch_encoder)
		ilk_fdi_disable(crtc);

	intel_encoders_post_disable(state, crtc);

	if (old_crtc_state->has_pch_encoder) {
		ilk_disable_pch_transcoder(dev_priv, pipe);

		if (HAS_PCH_CPT(dev_priv)) {
			i915_reg_t reg;
			u32 temp;

			/* disable TRANS_DP_CTL */
			reg = TRANS_DP_CTL(pipe);
			temp = intel_de_read(dev_priv, reg);
			temp &= ~(TRANS_DP_OUTPUT_ENABLE |
				  TRANS_DP_PORT_SEL_MASK);
			temp |= TRANS_DP_PORT_SEL_NONE;
			intel_de_write(dev_priv, reg, temp);

			/* disable DPLL_SEL */
			temp = intel_de_read(dev_priv, PCH_DPLL_SEL);
			temp &= ~(TRANS_DPLL_ENABLE(pipe) | TRANS_DPLLB_SEL(pipe));
			intel_de_write(dev_priv, PCH_DPLL_SEL, temp);
		}

		ilk_fdi_pll_disable(crtc);
	}

	intel_set_cpu_fifo_underrun_reporting(dev_priv, pipe, true);
	intel_set_pch_fifo_underrun_reporting(dev_priv, pipe, true);
}

static void hsw_crtc_disable(struct intel_atomic_state *state,
			     struct intel_crtc *crtc)
{
	/*
	 * FIXME collapse everything to one hook.
	 * Need care with mst->ddi interactions.
	 */
	intel_encoders_disable(state, crtc);
	intel_encoders_post_disable(state, crtc);
}

static void i9xx_pfit_enable(const struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);

	if (!crtc_state->gmch_pfit.control)
		return;

	/*
	 * The panel fitter should only be adjusted whilst the pipe is disabled,
	 * according to register description and PRM.
	 */
	drm_WARN_ON(&dev_priv->drm,
		    intel_de_read(dev_priv, PFIT_CONTROL) & PFIT_ENABLE);
	assert_pipe_disabled(dev_priv, crtc_state->cpu_transcoder);

	intel_de_write(dev_priv, PFIT_PGM_RATIOS,
		       crtc_state->gmch_pfit.pgm_ratios);
	intel_de_write(dev_priv, PFIT_CONTROL, crtc_state->gmch_pfit.control);

	/* Border color in case we don't scale up to the full screen. Black by
	 * default, change to something else for debugging. */
	intel_de_write(dev_priv, BCLRPAT(crtc->pipe), 0);
}

bool intel_phy_is_combo(struct drm_i915_private *dev_priv, enum phy phy)
{
	if (phy == PHY_NONE)
		return false;
	else if (IS_ALDERLAKE_S(dev_priv))
		return phy <= PHY_E;
	else if (IS_DG1(dev_priv) || IS_ROCKETLAKE(dev_priv))
		return phy <= PHY_D;
	else if (IS_JSL_EHL(dev_priv))
		return phy <= PHY_C;
	else if (DISPLAY_VER(dev_priv) >= 11)
		return phy <= PHY_B;
	else
		return false;
}

bool intel_phy_is_tc(struct drm_i915_private *dev_priv, enum phy phy)
{
	if (IS_ALDERLAKE_P(dev_priv))
		return phy >= PHY_F && phy <= PHY_I;
	else if (IS_TIGERLAKE(dev_priv))
		return phy >= PHY_D && phy <= PHY_I;
	else if (IS_ICELAKE(dev_priv))
		return phy >= PHY_C && phy <= PHY_F;
	else
		return false;
}

enum phy intel_port_to_phy(struct drm_i915_private *i915, enum port port)
{
	if (DISPLAY_VER(i915) >= 13 && port >= PORT_D_XELPD)
		return PHY_D + port - PORT_D_XELPD;
	else if (DISPLAY_VER(i915) >= 13 && port >= PORT_TC1)
		return PHY_F + port - PORT_TC1;
	else if (IS_ALDERLAKE_S(i915) && port >= PORT_TC1)
		return PHY_B + port - PORT_TC1;
	else if ((IS_DG1(i915) || IS_ROCKETLAKE(i915)) && port >= PORT_TC1)
		return PHY_C + port - PORT_TC1;
	else if (IS_JSL_EHL(i915) && port == PORT_D)
		return PHY_A;

	return PHY_A + port - PORT_A;
}

enum tc_port intel_port_to_tc(struct drm_i915_private *dev_priv, enum port port)
{
	if (!intel_phy_is_tc(dev_priv, intel_port_to_phy(dev_priv, port)))
		return TC_PORT_NONE;

	if (DISPLAY_VER(dev_priv) >= 12)
		return TC_PORT_1 + port - PORT_TC1;
	else
		return TC_PORT_1 + port - PORT_C;
}

enum intel_display_power_domain intel_port_to_power_domain(enum port port)
{
	switch (port) {
	case PORT_A:
		return POWER_DOMAIN_PORT_DDI_A_LANES;
	case PORT_B:
		return POWER_DOMAIN_PORT_DDI_B_LANES;
	case PORT_C:
		return POWER_DOMAIN_PORT_DDI_C_LANES;
	case PORT_D:
		return POWER_DOMAIN_PORT_DDI_D_LANES;
	case PORT_E:
		return POWER_DOMAIN_PORT_DDI_E_LANES;
	case PORT_F:
		return POWER_DOMAIN_PORT_DDI_F_LANES;
	case PORT_G:
		return POWER_DOMAIN_PORT_DDI_G_LANES;
	case PORT_H:
		return POWER_DOMAIN_PORT_DDI_H_LANES;
	case PORT_I:
		return POWER_DOMAIN_PORT_DDI_I_LANES;
	default:
		MISSING_CASE(port);
		return POWER_DOMAIN_PORT_OTHER;
	}
}

enum intel_display_power_domain
intel_aux_power_domain(struct intel_digital_port *dig_port)
{
	struct drm_i915_private *dev_priv = to_i915(dig_port->base.base.dev);
	enum phy phy = intel_port_to_phy(dev_priv, dig_port->base.port);

	if (intel_phy_is_tc(dev_priv, phy) &&
	    dig_port->tc_mode == TC_PORT_TBT_ALT) {
		switch (dig_port->aux_ch) {
		case AUX_CH_C:
			return POWER_DOMAIN_AUX_C_TBT;
		case AUX_CH_D:
			return POWER_DOMAIN_AUX_D_TBT;
		case AUX_CH_E:
			return POWER_DOMAIN_AUX_E_TBT;
		case AUX_CH_F:
			return POWER_DOMAIN_AUX_F_TBT;
		case AUX_CH_G:
			return POWER_DOMAIN_AUX_G_TBT;
		case AUX_CH_H:
			return POWER_DOMAIN_AUX_H_TBT;
		case AUX_CH_I:
			return POWER_DOMAIN_AUX_I_TBT;
		default:
			MISSING_CASE(dig_port->aux_ch);
			return POWER_DOMAIN_AUX_C_TBT;
		}
	}

	return intel_legacy_aux_to_power_domain(dig_port->aux_ch);
}

/*
 * Converts aux_ch to power_domain without caring about TBT ports for that use
 * intel_aux_power_domain()
 */
enum intel_display_power_domain
intel_legacy_aux_to_power_domain(enum aux_ch aux_ch)
{
	switch (aux_ch) {
	case AUX_CH_A:
		return POWER_DOMAIN_AUX_A;
	case AUX_CH_B:
		return POWER_DOMAIN_AUX_B;
	case AUX_CH_C:
		return POWER_DOMAIN_AUX_C;
	case AUX_CH_D:
		return POWER_DOMAIN_AUX_D;
	case AUX_CH_E:
		return POWER_DOMAIN_AUX_E;
	case AUX_CH_F:
		return POWER_DOMAIN_AUX_F;
	case AUX_CH_G:
		return POWER_DOMAIN_AUX_G;
	case AUX_CH_H:
		return POWER_DOMAIN_AUX_H;
	case AUX_CH_I:
		return POWER_DOMAIN_AUX_I;
	default:
		MISSING_CASE(aux_ch);
		return POWER_DOMAIN_AUX_A;
	}
}

static u64 get_crtc_power_domains(struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	struct drm_encoder *encoder;
	enum pipe pipe = crtc->pipe;
	u64 mask;
	enum transcoder transcoder = crtc_state->cpu_transcoder;

	if (!crtc_state->hw.active)
		return 0;

	mask = BIT_ULL(POWER_DOMAIN_PIPE(pipe));
	mask |= BIT_ULL(POWER_DOMAIN_TRANSCODER(transcoder));
	if (crtc_state->pch_pfit.enabled ||
	    crtc_state->pch_pfit.force_thru)
		mask |= BIT_ULL(POWER_DOMAIN_PIPE_PANEL_FITTER(pipe));

	drm_for_each_encoder_mask(encoder, &dev_priv->drm,
				  crtc_state->uapi.encoder_mask) {
		struct intel_encoder *intel_encoder = to_intel_encoder(encoder);

		mask |= BIT_ULL(intel_encoder->power_domain);
	}

	if (HAS_DDI(dev_priv) && crtc_state->has_audio)
		mask |= BIT_ULL(POWER_DOMAIN_AUDIO);

	if (crtc_state->shared_dpll)
		mask |= BIT_ULL(POWER_DOMAIN_DISPLAY_CORE);

	if (crtc_state->dsc.compression_enable)
		mask |= BIT_ULL(intel_dsc_power_domain(crtc_state));

	return mask;
}

static u64
modeset_get_crtc_power_domains(struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	enum intel_display_power_domain domain;
	u64 domains, new_domains, old_domains;

	domains = get_crtc_power_domains(crtc_state);

	new_domains = domains & ~crtc->enabled_power_domains.mask;
	old_domains = crtc->enabled_power_domains.mask & ~domains;

	for_each_power_domain(domain, new_domains)
		intel_display_power_get_in_set(dev_priv,
					       &crtc->enabled_power_domains,
					       domain);

	return old_domains;
}

static void modeset_put_crtc_power_domains(struct intel_crtc *crtc,
					   u64 domains)
{
	intel_display_power_put_mask_in_set(to_i915(crtc->base.dev),
					    &crtc->enabled_power_domains,
					    domains);
}

static void valleyview_crtc_enable(struct intel_atomic_state *state,
				   struct intel_crtc *crtc)
{
	const struct intel_crtc_state *new_crtc_state =
		intel_atomic_get_new_crtc_state(state, crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	enum pipe pipe = crtc->pipe;

	if (drm_WARN_ON(&dev_priv->drm, crtc->active))
		return;

	if (intel_crtc_has_dp_encoder(new_crtc_state))
		intel_dp_set_m_n(new_crtc_state, M1_N1);

	intel_set_transcoder_timings(new_crtc_state);
	intel_set_pipe_src_size(new_crtc_state);

	if (IS_CHERRYVIEW(dev_priv) && pipe == PIPE_B) {
		intel_de_write(dev_priv, CHV_BLEND(pipe), CHV_BLEND_LEGACY);
		intel_de_write(dev_priv, CHV_CANVAS(pipe), 0);
	}

	i9xx_set_pipeconf(new_crtc_state);

	crtc->active = true;

	intel_set_cpu_fifo_underrun_reporting(dev_priv, pipe, true);

	intel_encoders_pre_pll_enable(state, crtc);

	if (IS_CHERRYVIEW(dev_priv)) {
		chv_prepare_pll(crtc, new_crtc_state);
		chv_enable_pll(crtc, new_crtc_state);
	} else {
		vlv_prepare_pll(crtc, new_crtc_state);
		vlv_enable_pll(crtc, new_crtc_state);
	}

	intel_encoders_pre_enable(state, crtc);

	i9xx_pfit_enable(new_crtc_state);

	intel_color_load_luts(new_crtc_state);
	intel_color_commit(new_crtc_state);
	/* update DSPCNTR to configure gamma for pipe bottom color */
	intel_disable_primary_plane(new_crtc_state);

	dev_priv->display.initial_watermarks(state, crtc);
	intel_enable_pipe(new_crtc_state);

	intel_crtc_vblank_on(new_crtc_state);

	intel_encoders_enable(state, crtc);
}

static void i9xx_set_pll_dividers(const struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);

	intel_de_write(dev_priv, FP0(crtc->pipe),
		       crtc_state->dpll_hw_state.fp0);
	intel_de_write(dev_priv, FP1(crtc->pipe),
		       crtc_state->dpll_hw_state.fp1);
}

static void i9xx_crtc_enable(struct intel_atomic_state *state,
			     struct intel_crtc *crtc)
{
	const struct intel_crtc_state *new_crtc_state =
		intel_atomic_get_new_crtc_state(state, crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	enum pipe pipe = crtc->pipe;

	if (drm_WARN_ON(&dev_priv->drm, crtc->active))
		return;

	i9xx_set_pll_dividers(new_crtc_state);

	if (intel_crtc_has_dp_encoder(new_crtc_state))
		intel_dp_set_m_n(new_crtc_state, M1_N1);

	intel_set_transcoder_timings(new_crtc_state);
	intel_set_pipe_src_size(new_crtc_state);

	i9xx_set_pipeconf(new_crtc_state);

	crtc->active = true;

	if (DISPLAY_VER(dev_priv) != 2)
		intel_set_cpu_fifo_underrun_reporting(dev_priv, pipe, true);

	intel_encoders_pre_enable(state, crtc);

	i9xx_enable_pll(crtc, new_crtc_state);

	i9xx_pfit_enable(new_crtc_state);

	intel_color_load_luts(new_crtc_state);
	intel_color_commit(new_crtc_state);
	/* update DSPCNTR to configure gamma for pipe bottom color */
	intel_disable_primary_plane(new_crtc_state);

	if (dev_priv->display.initial_watermarks)
		dev_priv->display.initial_watermarks(state, crtc);
	else
		intel_update_watermarks(crtc);
	intel_enable_pipe(new_crtc_state);

	intel_crtc_vblank_on(new_crtc_state);

	intel_encoders_enable(state, crtc);

	/* prevents spurious underruns */
	if (DISPLAY_VER(dev_priv) == 2)
		intel_wait_for_vblank(dev_priv, pipe);
}

static void i9xx_pfit_disable(const struct intel_crtc_state *old_crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(old_crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);

	if (!old_crtc_state->gmch_pfit.control)
		return;

	assert_pipe_disabled(dev_priv, old_crtc_state->cpu_transcoder);

	drm_dbg_kms(&dev_priv->drm, "disabling pfit, current: 0x%08x\n",
		    intel_de_read(dev_priv, PFIT_CONTROL));
	intel_de_write(dev_priv, PFIT_CONTROL, 0);
}

static void i9xx_crtc_disable(struct intel_atomic_state *state,
			      struct intel_crtc *crtc)
{
	struct intel_crtc_state *old_crtc_state =
		intel_atomic_get_old_crtc_state(state, crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	enum pipe pipe = crtc->pipe;

	/*
	 * On gen2 planes are double buffered but the pipe isn't, so we must
	 * wait for planes to fully turn off before disabling the pipe.
	 */
	if (DISPLAY_VER(dev_priv) == 2)
		intel_wait_for_vblank(dev_priv, pipe);

	intel_encoders_disable(state, crtc);

	intel_crtc_vblank_off(old_crtc_state);

	intel_disable_pipe(old_crtc_state);

	i9xx_pfit_disable(old_crtc_state);

	intel_encoders_post_disable(state, crtc);

	if (!intel_crtc_has_type(old_crtc_state, INTEL_OUTPUT_DSI)) {
		if (IS_CHERRYVIEW(dev_priv))
			chv_disable_pll(dev_priv, pipe);
		else if (IS_VALLEYVIEW(dev_priv))
			vlv_disable_pll(dev_priv, pipe);
		else
			i9xx_disable_pll(old_crtc_state);
	}

	intel_encoders_post_pll_disable(state, crtc);

	if (DISPLAY_VER(dev_priv) != 2)
		intel_set_cpu_fifo_underrun_reporting(dev_priv, pipe, false);

	if (!dev_priv->display.initial_watermarks)
		intel_update_watermarks(crtc);

	/* clock the pipe down to 640x480@60 to potentially save power */
	if (IS_I830(dev_priv))
		i830_enable_pipe(dev_priv, pipe);
}

static void intel_crtc_disable_noatomic(struct intel_crtc *crtc,
					struct drm_modeset_acquire_ctx *ctx)
{
	struct intel_encoder *encoder;
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	struct intel_bw_state *bw_state =
		to_intel_bw_state(dev_priv->bw_obj.state);
	struct intel_cdclk_state *cdclk_state =
		to_intel_cdclk_state(dev_priv->cdclk.obj.state);
	struct intel_dbuf_state *dbuf_state =
		to_intel_dbuf_state(dev_priv->dbuf.obj.state);
	struct intel_crtc_state *crtc_state =
		to_intel_crtc_state(crtc->base.state);
	struct intel_plane *plane;
	struct drm_atomic_state *state;
	struct intel_crtc_state *temp_crtc_state;
	enum pipe pipe = crtc->pipe;
	int ret;

	if (!crtc_state->hw.active)
		return;

	for_each_intel_plane_on_crtc(&dev_priv->drm, crtc, plane) {
		const struct intel_plane_state *plane_state =
			to_intel_plane_state(plane->base.state);

		if (plane_state->uapi.visible)
			intel_plane_disable_noatomic(crtc, plane);
	}

	state = drm_atomic_state_alloc(&dev_priv->drm);
	if (!state) {
		drm_dbg_kms(&dev_priv->drm,
			    "failed to disable [CRTC:%d:%s], out of memory",
			    crtc->base.base.id, crtc->base.name);
		return;
	}

	state->acquire_ctx = ctx;

	/* Everything's already locked, -EDEADLK can't happen. */
	temp_crtc_state = intel_atomic_get_crtc_state(state, crtc);
	ret = drm_atomic_add_affected_connectors(state, &crtc->base);

	drm_WARN_ON(&dev_priv->drm, IS_ERR(temp_crtc_state) || ret);

	dev_priv->display.crtc_disable(to_intel_atomic_state(state), crtc);

	drm_atomic_state_put(state);

	drm_dbg_kms(&dev_priv->drm,
		    "[CRTC:%d:%s] hw state adjusted, was enabled, now disabled\n",
		    crtc->base.base.id, crtc->base.name);

	crtc->active = false;
	crtc->base.enabled = false;

	drm_WARN_ON(&dev_priv->drm,
		    drm_atomic_set_mode_for_crtc(&crtc_state->uapi, NULL) < 0);
	crtc_state->uapi.active = false;
	crtc_state->uapi.connector_mask = 0;
	crtc_state->uapi.encoder_mask = 0;
	intel_crtc_free_hw_state(crtc_state);
	memset(&crtc_state->hw, 0, sizeof(crtc_state->hw));

	for_each_encoder_on_crtc(&dev_priv->drm, &crtc->base, encoder)
		encoder->base.crtc = NULL;

	intel_fbc_disable(crtc);
	intel_update_watermarks(crtc);
	intel_disable_shared_dpll(crtc_state);

	intel_display_power_put_all_in_set(dev_priv, &crtc->enabled_power_domains);

	dev_priv->active_pipes &= ~BIT(pipe);
	cdclk_state->min_cdclk[pipe] = 0;
	cdclk_state->min_voltage_level[pipe] = 0;
	cdclk_state->active_pipes &= ~BIT(pipe);

	dbuf_state->active_pipes &= ~BIT(pipe);

	bw_state->data_rate[pipe] = 0;
	bw_state->num_active_planes[pipe] = 0;
}

/*
 * turn all crtc's off, but do not adjust state
 * This has to be paired with a call to intel_modeset_setup_hw_state.
 */
int intel_display_suspend(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = to_i915(dev);
	struct drm_atomic_state *state;
	int ret;

	if (!HAS_DISPLAY(dev_priv))
		return 0;

	state = drm_atomic_helper_suspend(dev);
	ret = PTR_ERR_OR_ZERO(state);
	if (ret)
		drm_err(&dev_priv->drm, "Suspending crtc's failed with %i\n",
			ret);
	else
		dev_priv->modeset_restore_state = state;
	return ret;
}

void intel_encoder_destroy(struct drm_encoder *encoder)
{
	struct intel_encoder *intel_encoder = to_intel_encoder(encoder);

	drm_encoder_cleanup(encoder);
	kfree(intel_encoder);
}

/* Cross check the actual hw state with our own modeset state tracking (and it's
 * internal consistency). */
static void intel_connector_verify_state(struct intel_crtc_state *crtc_state,
					 struct drm_connector_state *conn_state)
{
	struct intel_connector *connector = to_intel_connector(conn_state->connector);
	struct drm_i915_private *i915 = to_i915(connector->base.dev);

	drm_dbg_kms(&i915->drm, "[CONNECTOR:%d:%s]\n",
		    connector->base.base.id, connector->base.name);

	if (connector->get_hw_state(connector)) {
		struct intel_encoder *encoder = intel_attached_encoder(connector);

		I915_STATE_WARN(!crtc_state,
			 "connector enabled without attached crtc\n");

		if (!crtc_state)
			return;

		I915_STATE_WARN(!crtc_state->hw.active,
				"connector is active, but attached crtc isn't\n");

		if (!encoder || encoder->type == INTEL_OUTPUT_DP_MST)
			return;

		I915_STATE_WARN(conn_state->best_encoder != &encoder->base,
			"atomic encoder doesn't match attached encoder\n");

		I915_STATE_WARN(conn_state->crtc != encoder->base.crtc,
			"attached encoder crtc differs from connector crtc\n");
	} else {
		I915_STATE_WARN(crtc_state && crtc_state->hw.active,
				"attached crtc is active, but connector isn't\n");
		I915_STATE_WARN(!crtc_state && conn_state->best_encoder,
			"best encoder set without crtc!\n");
	}
}

bool hsw_crtc_state_ips_capable(const struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);

	/* IPS only exists on ULT machines and is tied to pipe A. */
	if (!hsw_crtc_supports_ips(crtc))
		return false;

	if (!dev_priv->params.enable_ips)
		return false;

	if (crtc_state->pipe_bpp > 24)
		return false;

	/*
	 * We compare against max which means we must take
	 * the increased cdclk requirement into account when
	 * calculating the new cdclk.
	 *
	 * Should measure whether using a lower cdclk w/o IPS
	 */
	if (IS_BROADWELL(dev_priv) &&
	    crtc_state->pixel_rate > dev_priv->max_cdclk_freq * 95 / 100)
		return false;

	return true;
}

static int hsw_compute_ips_config(struct intel_crtc_state *crtc_state)
{
	struct drm_i915_private *dev_priv =
		to_i915(crtc_state->uapi.crtc->dev);
	struct intel_atomic_state *state =
		to_intel_atomic_state(crtc_state->uapi.state);

	crtc_state->ips_enabled = false;

	if (!hsw_crtc_state_ips_capable(crtc_state))
		return 0;

	/*
	 * When IPS gets enabled, the pipe CRC changes. Since IPS gets
	 * enabled and disabled dynamically based on package C states,
	 * user space can't make reliable use of the CRCs, so let's just
	 * completely disable it.
	 */
	if (crtc_state->crc_enabled)
		return 0;

	/* IPS should be fine as long as at least one plane is enabled. */
	if (!(crtc_state->active_planes & ~BIT(PLANE_CURSOR)))
		return 0;

	if (IS_BROADWELL(dev_priv)) {
		const struct intel_cdclk_state *cdclk_state;

		cdclk_state = intel_atomic_get_cdclk_state(state);
		if (IS_ERR(cdclk_state))
			return PTR_ERR(cdclk_state);

		/* pixel rate mustn't exceed 95% of cdclk with IPS on BDW */
		if (crtc_state->pixel_rate > cdclk_state->logical.cdclk * 95 / 100)
			return 0;
	}

	crtc_state->ips_enabled = true;

	return 0;
}

static bool intel_crtc_supports_double_wide(const struct intel_crtc *crtc)
{
	const struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);

	/* GDG double wide on either pipe, otherwise pipe A only */
	return DISPLAY_VER(dev_priv) < 4 &&
		(crtc->pipe == PIPE_A || IS_I915G(dev_priv));
}

static u32 ilk_pipe_pixel_rate(const struct intel_crtc_state *crtc_state)
{
	u32 pixel_rate = crtc_state->hw.pipe_mode.crtc_clock;
	struct drm_rect src;

	/*
	 * We only use IF-ID interlacing. If we ever use
	 * PF-ID we'll need to adjust the pixel_rate here.
	 */

	if (!crtc_state->pch_pfit.enabled)
		return pixel_rate;

	drm_rect_init(&src, 0, 0,
		      crtc_state->pipe_src_w << 16,
		      crtc_state->pipe_src_h << 16);

	return intel_adjusted_rate(&src, &crtc_state->pch_pfit.dst,
				   pixel_rate);
}

static void intel_mode_from_crtc_timings(struct drm_display_mode *mode,
					 const struct drm_display_mode *timings)
{
	mode->hdisplay = timings->crtc_hdisplay;
	mode->htotal = timings->crtc_htotal;
	mode->hsync_start = timings->crtc_hsync_start;
	mode->hsync_end = timings->crtc_hsync_end;

	mode->vdisplay = timings->crtc_vdisplay;
	mode->vtotal = timings->crtc_vtotal;
	mode->vsync_start = timings->crtc_vsync_start;
	mode->vsync_end = timings->crtc_vsync_end;

	mode->flags = timings->flags;
	mode->type = DRM_MODE_TYPE_DRIVER;

	mode->clock = timings->crtc_clock;

	drm_mode_set_name(mode);
}

static void intel_crtc_compute_pixel_rate(struct intel_crtc_state *crtc_state)
{
	struct drm_i915_private *dev_priv = to_i915(crtc_state->uapi.crtc->dev);

	if (HAS_GMCH(dev_priv))
		/* FIXME calculate proper pipe pixel rate for GMCH pfit */
		crtc_state->pixel_rate =
			crtc_state->hw.pipe_mode.crtc_clock;
	else
		crtc_state->pixel_rate =
			ilk_pipe_pixel_rate(crtc_state);
}

static void intel_crtc_readout_derived_state(struct intel_crtc_state *crtc_state)
{
	struct drm_display_mode *mode = &crtc_state->hw.mode;
	struct drm_display_mode *pipe_mode = &crtc_state->hw.pipe_mode;
	struct drm_display_mode *adjusted_mode = &crtc_state->hw.adjusted_mode;

	drm_mode_copy(pipe_mode, adjusted_mode);

	if (crtc_state->bigjoiner) {
		/*
		 * transcoder is programmed to the full mode,
		 * but pipe timings are half of the transcoder mode
		 */
		pipe_mode->crtc_hdisplay /= 2;
		pipe_mode->crtc_hblank_start /= 2;
		pipe_mode->crtc_hblank_end /= 2;
		pipe_mode->crtc_hsync_start /= 2;
		pipe_mode->crtc_hsync_end /= 2;
		pipe_mode->crtc_htotal /= 2;
		pipe_mode->crtc_clock /= 2;
	}

	if (crtc_state->splitter.enable) {
		int n = crtc_state->splitter.link_count;
		int overlap = crtc_state->splitter.pixel_overlap;

		/*
		 * eDP MSO uses segment timings from EDID for transcoder
		 * timings, but full mode for everything else.
		 *
		 * h_full = (h_segment - pixel_overlap) * link_count
		 */
		pipe_mode->crtc_hdisplay = (pipe_mode->crtc_hdisplay - overlap) * n;
		pipe_mode->crtc_hblank_start = (pipe_mode->crtc_hblank_start - overlap) * n;
		pipe_mode->crtc_hblank_end = (pipe_mode->crtc_hblank_end - overlap) * n;
		pipe_mode->crtc_hsync_start = (pipe_mode->crtc_hsync_start - overlap) * n;
		pipe_mode->crtc_hsync_end = (pipe_mode->crtc_hsync_end - overlap) * n;
		pipe_mode->crtc_htotal = (pipe_mode->crtc_htotal - overlap) * n;
		pipe_mode->crtc_clock *= n;

		intel_mode_from_crtc_timings(pipe_mode, pipe_mode);
		intel_mode_from_crtc_timings(adjusted_mode, pipe_mode);
	} else {
		intel_mode_from_crtc_timings(pipe_mode, pipe_mode);
		intel_mode_from_crtc_timings(adjusted_mode, adjusted_mode);
	}

	intel_crtc_compute_pixel_rate(crtc_state);

	drm_mode_copy(mode, adjusted_mode);
	mode->hdisplay = crtc_state->pipe_src_w << crtc_state->bigjoiner;
	mode->vdisplay = crtc_state->pipe_src_h;
}

static void intel_encoder_get_config(struct intel_encoder *encoder,
				     struct intel_crtc_state *crtc_state)
{
	encoder->get_config(encoder, crtc_state);

	intel_crtc_readout_derived_state(crtc_state);
}

static int intel_crtc_compute_config(struct intel_crtc *crtc,
				     struct intel_crtc_state *pipe_config)
{
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	struct drm_display_mode *pipe_mode = &pipe_config->hw.pipe_mode;
	int clock_limit = dev_priv->max_dotclk_freq;

	drm_mode_copy(pipe_mode, &pipe_config->hw.adjusted_mode);

	/* Adjust pipe_mode for bigjoiner, with half the horizontal mode */
	if (pipe_config->bigjoiner) {
		pipe_mode->crtc_clock /= 2;
		pipe_mode->crtc_hdisplay /= 2;
		pipe_mode->crtc_hblank_start /= 2;
		pipe_mode->crtc_hblank_end /= 2;
		pipe_mode->crtc_hsync_start /= 2;
		pipe_mode->crtc_hsync_end /= 2;
		pipe_mode->crtc_htotal /= 2;
		pipe_config->pipe_src_w /= 2;
	}

	if (pipe_config->splitter.enable) {
		int n = pipe_config->splitter.link_count;
		int overlap = pipe_config->splitter.pixel_overlap;

		pipe_mode->crtc_hdisplay = (pipe_mode->crtc_hdisplay - overlap) * n;
		pipe_mode->crtc_hblank_start = (pipe_mode->crtc_hblank_start - overlap) * n;
		pipe_mode->crtc_hblank_end = (pipe_mode->crtc_hblank_end - overlap) * n;
		pipe_mode->crtc_hsync_start = (pipe_mode->crtc_hsync_start - overlap) * n;
		pipe_mode->crtc_hsync_end = (pipe_mode->crtc_hsync_end - overlap) * n;
		pipe_mode->crtc_htotal = (pipe_mode->crtc_htotal - overlap) * n;
		pipe_mode->crtc_clock *= n;
	}

	intel_mode_from_crtc_timings(pipe_mode, pipe_mode);

	if (DISPLAY_VER(dev_priv) < 4) {
		clock_limit = dev_priv->max_cdclk_freq * 9 / 10;

		/*
		 * Enable double wide mode when the dot clock
		 * is > 90% of the (display) core speed.
		 */
		if (intel_crtc_supports_double_wide(crtc) &&
		    pipe_mode->crtc_clock > clock_limit) {
			clock_limit = dev_priv->max_dotclk_freq;
			pipe_config->double_wide = true;
		}
	}

	if (pipe_mode->crtc_clock > clock_limit) {
		drm_dbg_kms(&dev_priv->drm,
			    "requested pixel clock (%d kHz) too high (max: %d kHz, double wide: %s)\n",
			    pipe_mode->crtc_clock, clock_limit,
			    yesno(pipe_config->double_wide));
		return -EINVAL;
	}

	/*
	 * Pipe horizontal size must be even in:
	 * - DVO ganged mode
	 * - LVDS dual channel mode
	 * - Double wide pipe
	 */
	if (pipe_config->pipe_src_w & 1) {
		if (pipe_config->double_wide) {
			drm_dbg_kms(&dev_priv->drm,
				    "Odd pipe source width not supported with double wide pipe\n");
			return -EINVAL;
		}

		if (intel_crtc_has_type(pipe_config, INTEL_OUTPUT_LVDS) &&
		    intel_is_dual_link_lvds(dev_priv)) {
			drm_dbg_kms(&dev_priv->drm,
				    "Odd pipe source width not supported with dual link LVDS\n");
			return -EINVAL;
		}
	}

	/* Cantiga+ cannot handle modes with a hsync front porch of 0.
	 * WaPruneModeWithIncorrectHsyncOffset:ctg,elk,ilk,snb,ivb,vlv,hsw.
	 */
	if ((DISPLAY_VER(dev_priv) > 4 || IS_G4X(dev_priv)) &&
	    pipe_mode->crtc_hsync_start == pipe_mode->crtc_hdisplay)
		return -EINVAL;

	intel_crtc_compute_pixel_rate(pipe_config);

	if (pipe_config->has_pch_encoder)
		return ilk_fdi_compute_config(crtc, pipe_config);

	return 0;
}

static void
intel_reduce_m_n_ratio(u32 *num, u32 *den)
{
	while (*num > DATA_LINK_M_N_MASK ||
	       *den > DATA_LINK_M_N_MASK) {
		*num >>= 1;
		*den >>= 1;
	}
}

static void compute_m_n(unsigned int m, unsigned int n,
			u32 *ret_m, u32 *ret_n,
			bool constant_n)
{
	/*
	 * Several DP dongles in particular seem to be fussy about
	 * too large link M/N values. Give N value as 0x8000 that
	 * should be acceptable by specific devices. 0x8000 is the
	 * specified fixed N value for asynchronous clock mode,
	 * which the devices expect also in synchronous clock mode.
	 */
	if (constant_n)
		*ret_n = DP_LINK_CONSTANT_N_VALUE;
	else
		*ret_n = min_t(unsigned int, roundup_pow_of_two(n), DATA_LINK_N_MAX);

	*ret_m = div_u64(mul_u32_u32(m, *ret_n), n);
	intel_reduce_m_n_ratio(ret_m, ret_n);
}

void
intel_link_compute_m_n(u16 bits_per_pixel, int nlanes,
		       int pixel_clock, int link_clock,
		       struct intel_link_m_n *m_n,
		       bool constant_n, bool fec_enable)
{
	u32 data_clock = bits_per_pixel * pixel_clock;

	if (fec_enable)
		data_clock = intel_dp_mode_to_fec_clock(data_clock);

	m_n->tu = 64;
	compute_m_n(data_clock,
		    link_clock * nlanes * 8,
		    &m_n->gmch_m, &m_n->gmch_n,
		    constant_n);

	compute_m_n(pixel_clock, link_clock,
		    &m_n->link_m, &m_n->link_n,
		    constant_n);
}

static void intel_panel_sanitize_ssc(struct drm_i915_private *dev_priv)
{
	/*
	 * There may be no VBT; and if the BIOS enabled SSC we can
	 * just keep using it to avoid unnecessary flicker.  Whereas if the
	 * BIOS isn't using it, don't assume it will work even if the VBT
	 * indicates as much.
	 */
	if (HAS_PCH_IBX(dev_priv) || HAS_PCH_CPT(dev_priv)) {
		bool bios_lvds_use_ssc = intel_de_read(dev_priv,
						       PCH_DREF_CONTROL) &
			DREF_SSC1_ENABLE;

		if (dev_priv->vbt.lvds_use_ssc != bios_lvds_use_ssc) {
			drm_dbg_kms(&dev_priv->drm,
				    "SSC %s by BIOS, overriding VBT which says %s\n",
				    enableddisabled(bios_lvds_use_ssc),
				    enableddisabled(dev_priv->vbt.lvds_use_ssc));
			dev_priv->vbt.lvds_use_ssc = bios_lvds_use_ssc;
		}
	}
}

static void intel_pch_transcoder_set_m_n(const struct intel_crtc_state *crtc_state,
					 const struct intel_link_m_n *m_n)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	enum pipe pipe = crtc->pipe;

	intel_de_write(dev_priv, PCH_TRANS_DATA_M1(pipe),
		       TU_SIZE(m_n->tu) | m_n->gmch_m);
	intel_de_write(dev_priv, PCH_TRANS_DATA_N1(pipe), m_n->gmch_n);
	intel_de_write(dev_priv, PCH_TRANS_LINK_M1(pipe), m_n->link_m);
	intel_de_write(dev_priv, PCH_TRANS_LINK_N1(pipe), m_n->link_n);
}

static bool transcoder_has_m2_n2(struct drm_i915_private *dev_priv,
				 enum transcoder transcoder)
{
	if (IS_HASWELL(dev_priv))
		return transcoder == TRANSCODER_EDP;

	/*
	 * Strictly speaking some registers are available before
	 * gen7, but we only support DRRS on gen7+
	 */
	return DISPLAY_VER(dev_priv) == 7 || IS_CHERRYVIEW(dev_priv);
}

static void intel_cpu_transcoder_set_m_n(const struct intel_crtc_state *crtc_state,
					 const struct intel_link_m_n *m_n,
					 const struct intel_link_m_n *m2_n2)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	enum pipe pipe = crtc->pipe;
	enum transcoder transcoder = crtc_state->cpu_transcoder;

	if (DISPLAY_VER(dev_priv) >= 5) {
		intel_de_write(dev_priv, PIPE_DATA_M1(transcoder),
			       TU_SIZE(m_n->tu) | m_n->gmch_m);
		intel_de_write(dev_priv, PIPE_DATA_N1(transcoder),
			       m_n->gmch_n);
		intel_de_write(dev_priv, PIPE_LINK_M1(transcoder),
			       m_n->link_m);
		intel_de_write(dev_priv, PIPE_LINK_N1(transcoder),
			       m_n->link_n);
		/*
		 *  M2_N2 registers are set only if DRRS is supported
		 * (to make sure the registers are not unnecessarily accessed).
		 */
		if (m2_n2 && crtc_state->has_drrs &&
		    transcoder_has_m2_n2(dev_priv, transcoder)) {
			intel_de_write(dev_priv, PIPE_DATA_M2(transcoder),
				       TU_SIZE(m2_n2->tu) | m2_n2->gmch_m);
			intel_de_write(dev_priv, PIPE_DATA_N2(transcoder),
				       m2_n2->gmch_n);
			intel_de_write(dev_priv, PIPE_LINK_M2(transcoder),
				       m2_n2->link_m);
			intel_de_write(dev_priv, PIPE_LINK_N2(transcoder),
				       m2_n2->link_n);
		}
	} else {
		intel_de_write(dev_priv, PIPE_DATA_M_G4X(pipe),
			       TU_SIZE(m_n->tu) | m_n->gmch_m);
		intel_de_write(dev_priv, PIPE_DATA_N_G4X(pipe), m_n->gmch_n);
		intel_de_write(dev_priv, PIPE_LINK_M_G4X(pipe), m_n->link_m);
		intel_de_write(dev_priv, PIPE_LINK_N_G4X(pipe), m_n->link_n);
	}
}

void intel_dp_set_m_n(const struct intel_crtc_state *crtc_state, enum link_m_n_set m_n)
{
	const struct intel_link_m_n *dp_m_n, *dp_m2_n2 = NULL;
	struct drm_i915_private *i915 = to_i915(crtc_state->uapi.crtc->dev);

	if (m_n == M1_N1) {
		dp_m_n = &crtc_state->dp_m_n;
		dp_m2_n2 = &crtc_state->dp_m2_n2;
	} else if (m_n == M2_N2) {

		/*
		 * M2_N2 registers are not supported. Hence m2_n2 divider value
		 * needs to be programmed into M1_N1.
		 */
		dp_m_n = &crtc_state->dp_m2_n2;
	} else {
		drm_err(&i915->drm, "Unsupported divider value\n");
		return;
	}

	if (crtc_state->has_pch_encoder)
		intel_pch_transcoder_set_m_n(crtc_state, &crtc_state->dp_m_n);
	else
		intel_cpu_transcoder_set_m_n(crtc_state, dp_m_n, dp_m2_n2);
}

static void intel_set_transcoder_timings(const struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	enum pipe pipe = crtc->pipe;
	enum transcoder cpu_transcoder = crtc_state->cpu_transcoder;
	const struct drm_display_mode *adjusted_mode = &crtc_state->hw.adjusted_mode;
	u32 crtc_vtotal, crtc_vblank_end;
	int vsyncshift = 0;

	/* We need to be careful not to changed the adjusted mode, for otherwise
	 * the hw state checker will get angry at the mismatch. */
	crtc_vtotal = adjusted_mode->crtc_vtotal;
	crtc_vblank_end = adjusted_mode->crtc_vblank_end;

	if (adjusted_mode->flags & DRM_MODE_FLAG_INTERLACE) {
		/* the chip adds 2 halflines automatically */
		crtc_vtotal -= 1;
		crtc_vblank_end -= 1;

		if (intel_crtc_has_type(crtc_state, INTEL_OUTPUT_SDVO))
			vsyncshift = (adjusted_mode->crtc_htotal - 1) / 2;
		else
			vsyncshift = adjusted_mode->crtc_hsync_start -
				adjusted_mode->crtc_htotal / 2;
		if (vsyncshift < 0)
			vsyncshift += adjusted_mode->crtc_htotal;
	}

	if (DISPLAY_VER(dev_priv) > 3)
		intel_de_write(dev_priv, VSYNCSHIFT(cpu_transcoder),
		               vsyncshift);

	intel_de_write(dev_priv, HTOTAL(cpu_transcoder),
		       (adjusted_mode->crtc_hdisplay - 1) | ((adjusted_mode->crtc_htotal - 1) << 16));
	intel_de_write(dev_priv, HBLANK(cpu_transcoder),
		       (adjusted_mode->crtc_hblank_start - 1) | ((adjusted_mode->crtc_hblank_end - 1) << 16));
	intel_de_write(dev_priv, HSYNC(cpu_transcoder),
		       (adjusted_mode->crtc_hsync_start - 1) | ((adjusted_mode->crtc_hsync_end - 1) << 16));

	intel_de_write(dev_priv, VTOTAL(cpu_transcoder),
		       (adjusted_mode->crtc_vdisplay - 1) | ((crtc_vtotal - 1) << 16));
	intel_de_write(dev_priv, VBLANK(cpu_transcoder),
		       (adjusted_mode->crtc_vblank_start - 1) | ((crtc_vblank_end - 1) << 16));
	intel_de_write(dev_priv, VSYNC(cpu_transcoder),
		       (adjusted_mode->crtc_vsync_start - 1) | ((adjusted_mode->crtc_vsync_end - 1) << 16));

	/* Workaround: when the EDP input selection is B, the VTOTAL_B must be
	 * programmed with the VTOTAL_EDP value. Same for VTOTAL_C. This is
	 * documented on the DDI_FUNC_CTL register description, EDP Input Select
	 * bits. */
	if (IS_HASWELL(dev_priv) && cpu_transcoder == TRANSCODER_EDP &&
	    (pipe == PIPE_B || pipe == PIPE_C))
		intel_de_write(dev_priv, VTOTAL(pipe),
		               intel_de_read(dev_priv, VTOTAL(cpu_transcoder)));

}

static void intel_set_pipe_src_size(const struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	enum pipe pipe = crtc->pipe;

	/* pipesrc controls the size that is scaled from, which should
	 * always be the user's requested size.
	 */
	intel_de_write(dev_priv, PIPESRC(pipe),
		       ((crtc_state->pipe_src_w - 1) << 16) | (crtc_state->pipe_src_h - 1));
}

static bool intel_pipe_is_interlaced(const struct intel_crtc_state *crtc_state)
{
	struct drm_i915_private *dev_priv = to_i915(crtc_state->uapi.crtc->dev);
	enum transcoder cpu_transcoder = crtc_state->cpu_transcoder;

	if (DISPLAY_VER(dev_priv) == 2)
		return false;

	if (DISPLAY_VER(dev_priv) >= 9 ||
	    IS_BROADWELL(dev_priv) || IS_HASWELL(dev_priv))
		return intel_de_read(dev_priv, PIPECONF(cpu_transcoder)) & PIPECONF_INTERLACE_MASK_HSW;
	else
		return intel_de_read(dev_priv, PIPECONF(cpu_transcoder)) & PIPECONF_INTERLACE_MASK;
}

static void intel_get_transcoder_timings(struct intel_crtc *crtc,
					 struct intel_crtc_state *pipe_config)
{
	struct drm_device *dev = crtc->base.dev;
	struct drm_i915_private *dev_priv = to_i915(dev);
	enum transcoder cpu_transcoder = pipe_config->cpu_transcoder;
	u32 tmp;

	tmp = intel_de_read(dev_priv, HTOTAL(cpu_transcoder));
	pipe_config->hw.adjusted_mode.crtc_hdisplay = (tmp & 0xffff) + 1;
	pipe_config->hw.adjusted_mode.crtc_htotal = ((tmp >> 16) & 0xffff) + 1;

	if (!transcoder_is_dsi(cpu_transcoder)) {
		tmp = intel_de_read(dev_priv, HBLANK(cpu_transcoder));
		pipe_config->hw.adjusted_mode.crtc_hblank_start =
							(tmp & 0xffff) + 1;
		pipe_config->hw.adjusted_mode.crtc_hblank_end =
						((tmp >> 16) & 0xffff) + 1;
	}
	tmp = intel_de_read(dev_priv, HSYNC(cpu_transcoder));
	pipe_config->hw.adjusted_mode.crtc_hsync_start = (tmp & 0xffff) + 1;
	pipe_config->hw.adjusted_mode.crtc_hsync_end = ((tmp >> 16) & 0xffff) + 1;

	tmp = intel_de_read(dev_priv, VTOTAL(cpu_transcoder));
	pipe_config->hw.adjusted_mode.crtc_vdisplay = (tmp & 0xffff) + 1;
	pipe_config->hw.adjusted_mode.crtc_vtotal = ((tmp >> 16) & 0xffff) + 1;

	if (!transcoder_is_dsi(cpu_transcoder)) {
		tmp = intel_de_read(dev_priv, VBLANK(cpu_transcoder));
		pipe_config->hw.adjusted_mode.crtc_vblank_start =
							(tmp & 0xffff) + 1;
		pipe_config->hw.adjusted_mode.crtc_vblank_end =
						((tmp >> 16) & 0xffff) + 1;
	}
	tmp = intel_de_read(dev_priv, VSYNC(cpu_transcoder));
	pipe_config->hw.adjusted_mode.crtc_vsync_start = (tmp & 0xffff) + 1;
	pipe_config->hw.adjusted_mode.crtc_vsync_end = ((tmp >> 16) & 0xffff) + 1;

	if (intel_pipe_is_interlaced(pipe_config)) {
		pipe_config->hw.adjusted_mode.flags |= DRM_MODE_FLAG_INTERLACE;
		pipe_config->hw.adjusted_mode.crtc_vtotal += 1;
		pipe_config->hw.adjusted_mode.crtc_vblank_end += 1;
	}
}

static void intel_get_pipe_src_size(struct intel_crtc *crtc,
				    struct intel_crtc_state *pipe_config)
{
	struct drm_device *dev = crtc->base.dev;
	struct drm_i915_private *dev_priv = to_i915(dev);
	u32 tmp;

	tmp = intel_de_read(dev_priv, PIPESRC(crtc->pipe));
	pipe_config->pipe_src_h = (tmp & 0xffff) + 1;
	pipe_config->pipe_src_w = ((tmp >> 16) & 0xffff) + 1;
}

static void i9xx_set_pipeconf(const struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	u32 pipeconf;

	pipeconf = 0;

	/* we keep both pipes enabled on 830 */
	if (IS_I830(dev_priv))
		pipeconf |= intel_de_read(dev_priv, PIPECONF(crtc->pipe)) & PIPECONF_ENABLE;

	if (crtc_state->double_wide)
		pipeconf |= PIPECONF_DOUBLE_WIDE;

	/* only g4x and later have fancy bpc/dither controls */
	if (IS_G4X(dev_priv) || IS_VALLEYVIEW(dev_priv) ||
	    IS_CHERRYVIEW(dev_priv)) {
		/* Bspec claims that we can't use dithering for 30bpp pipes. */
		if (crtc_state->dither && crtc_state->pipe_bpp != 30)
			pipeconf |= PIPECONF_DITHER_EN |
				    PIPECONF_DITHER_TYPE_SP;

		switch (crtc_state->pipe_bpp) {
		case 18:
			pipeconf |= PIPECONF_6BPC;
			break;
		case 24:
			pipeconf |= PIPECONF_8BPC;
			break;
		case 30:
			pipeconf |= PIPECONF_10BPC;
			break;
		default:
			/* Case prevented by intel_choose_pipe_bpp_dither. */
			BUG();
		}
	}

	if (crtc_state->hw.adjusted_mode.flags & DRM_MODE_FLAG_INTERLACE) {
		if (DISPLAY_VER(dev_priv) < 4 ||
		    intel_crtc_has_type(crtc_state, INTEL_OUTPUT_SDVO))
			pipeconf |= PIPECONF_INTERLACE_W_FIELD_INDICATION;
		else
			pipeconf |= PIPECONF_INTERLACE_W_SYNC_SHIFT;
	} else {
		pipeconf |= PIPECONF_PROGRESSIVE;
	}

	if ((IS_VALLEYVIEW(dev_priv) || IS_CHERRYVIEW(dev_priv)) &&
	     crtc_state->limited_color_range)
		pipeconf |= PIPECONF_COLOR_RANGE_SELECT;

	pipeconf |= PIPECONF_GAMMA_MODE(crtc_state->gamma_mode);

	pipeconf |= PIPECONF_FRAME_START_DELAY(dev_priv->framestart_delay - 1);

	intel_de_write(dev_priv, PIPECONF(crtc->pipe), pipeconf);
	intel_de_posting_read(dev_priv, PIPECONF(crtc->pipe));
}

static bool i9xx_has_pfit(struct drm_i915_private *dev_priv)
{
	if (IS_I830(dev_priv))
		return false;

	return DISPLAY_VER(dev_priv) >= 4 ||
		IS_PINEVIEW(dev_priv) || IS_MOBILE(dev_priv);
}

static void i9xx_get_pfit_config(struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	u32 tmp;

	if (!i9xx_has_pfit(dev_priv))
		return;

	tmp = intel_de_read(dev_priv, PFIT_CONTROL);
	if (!(tmp & PFIT_ENABLE))
		return;

	/* Check whether the pfit is attached to our pipe. */
	if (DISPLAY_VER(dev_priv) < 4) {
		if (crtc->pipe != PIPE_B)
			return;
	} else {
		if ((tmp & PFIT_PIPE_MASK) != (crtc->pipe << PFIT_PIPE_SHIFT))
			return;
	}

	crtc_state->gmch_pfit.control = tmp;
	crtc_state->gmch_pfit.pgm_ratios =
		intel_de_read(dev_priv, PFIT_PGM_RATIOS);
}

static void vlv_crtc_clock_get(struct intel_crtc *crtc,
			       struct intel_crtc_state *pipe_config)
{
	struct drm_device *dev = crtc->base.dev;
	struct drm_i915_private *dev_priv = to_i915(dev);
	enum pipe pipe = crtc->pipe;
	struct dpll clock;
	u32 mdiv;
	int refclk = 100000;

	/* In case of DSI, DPLL will not be used */
	if ((pipe_config->dpll_hw_state.dpll & DPLL_VCO_ENABLE) == 0)
		return;

	vlv_dpio_get(dev_priv);
	mdiv = vlv_dpio_read(dev_priv, pipe, VLV_PLL_DW3(pipe));
	vlv_dpio_put(dev_priv);

	clock.m1 = (mdiv >> DPIO_M1DIV_SHIFT) & 7;
	clock.m2 = mdiv & DPIO_M2DIV_MASK;
	clock.n = (mdiv >> DPIO_N_SHIFT) & 0xf;
	clock.p1 = (mdiv >> DPIO_P1_SHIFT) & 7;
	clock.p2 = (mdiv >> DPIO_P2_SHIFT) & 0x1f;

	pipe_config->port_clock = vlv_calc_dpll_params(refclk, &clock);
}

static void chv_crtc_clock_get(struct intel_crtc *crtc,
			       struct intel_crtc_state *pipe_config)
{
	struct drm_device *dev = crtc->base.dev;
	struct drm_i915_private *dev_priv = to_i915(dev);
	enum pipe pipe = crtc->pipe;
	enum dpio_channel port = vlv_pipe_to_channel(pipe);
	struct dpll clock;
	u32 cmn_dw13, pll_dw0, pll_dw1, pll_dw2, pll_dw3;
	int refclk = 100000;

	/* In case of DSI, DPLL will not be used */
	if ((pipe_config->dpll_hw_state.dpll & DPLL_VCO_ENABLE) == 0)
		return;

	vlv_dpio_get(dev_priv);
	cmn_dw13 = vlv_dpio_read(dev_priv, pipe, CHV_CMN_DW13(port));
	pll_dw0 = vlv_dpio_read(dev_priv, pipe, CHV_PLL_DW0(port));
	pll_dw1 = vlv_dpio_read(dev_priv, pipe, CHV_PLL_DW1(port));
	pll_dw2 = vlv_dpio_read(dev_priv, pipe, CHV_PLL_DW2(port));
	pll_dw3 = vlv_dpio_read(dev_priv, pipe, CHV_PLL_DW3(port));
	vlv_dpio_put(dev_priv);

	clock.m1 = (pll_dw1 & 0x7) == DPIO_CHV_M1_DIV_BY_2 ? 2 : 0;
	clock.m2 = (pll_dw0 & 0xff) << 22;
	if (pll_dw3 & DPIO_CHV_FRAC_DIV_EN)
		clock.m2 |= pll_dw2 & 0x3fffff;
	clock.n = (pll_dw1 >> DPIO_CHV_N_DIV_SHIFT) & 0xf;
	clock.p1 = (cmn_dw13 >> DPIO_CHV_P1_DIV_SHIFT) & 0x7;
	clock.p2 = (cmn_dw13 >> DPIO_CHV_P2_DIV_SHIFT) & 0x1f;

	pipe_config->port_clock = chv_calc_dpll_params(refclk, &clock);
}

static enum intel_output_format
bdw_get_pipemisc_output_format(struct intel_crtc *crtc)
{
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	u32 tmp;

	tmp = intel_de_read(dev_priv, PIPEMISC(crtc->pipe));

	if (tmp & PIPEMISC_YUV420_ENABLE) {
		/* We support 4:2:0 in full blend mode only */
		drm_WARN_ON(&dev_priv->drm,
			    (tmp & PIPEMISC_YUV420_MODE_FULL_BLEND) == 0);

		return INTEL_OUTPUT_FORMAT_YCBCR420;
	} else if (tmp & PIPEMISC_OUTPUT_COLORSPACE_YUV) {
		return INTEL_OUTPUT_FORMAT_YCBCR444;
	} else {
		return INTEL_OUTPUT_FORMAT_RGB;
	}
}

static void i9xx_get_pipe_color_config(struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct intel_plane *plane = to_intel_plane(crtc->base.primary);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	enum i9xx_plane_id i9xx_plane = plane->i9xx_plane;
	u32 tmp;

	tmp = intel_de_read(dev_priv, DSPCNTR(i9xx_plane));

	if (tmp & DISPPLANE_GAMMA_ENABLE)
		crtc_state->gamma_enable = true;

	if (!HAS_GMCH(dev_priv) &&
	    tmp & DISPPLANE_PIPE_CSC_ENABLE)
		crtc_state->csc_enable = true;
}

static bool i9xx_get_pipe_config(struct intel_crtc *crtc,
				 struct intel_crtc_state *pipe_config)
{
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	enum intel_display_power_domain power_domain;
	intel_wakeref_t wakeref;
	u32 tmp;
	bool ret;

	power_domain = POWER_DOMAIN_PIPE(crtc->pipe);
	wakeref = intel_display_power_get_if_enabled(dev_priv, power_domain);
	if (!wakeref)
		return false;

	pipe_config->output_format = INTEL_OUTPUT_FORMAT_RGB;
	pipe_config->cpu_transcoder = (enum transcoder) crtc->pipe;
	pipe_config->shared_dpll = NULL;

	ret = false;

	tmp = intel_de_read(dev_priv, PIPECONF(crtc->pipe));
	if (!(tmp & PIPECONF_ENABLE))
		goto out;

	if (IS_G4X(dev_priv) || IS_VALLEYVIEW(dev_priv) ||
	    IS_CHERRYVIEW(dev_priv)) {
		switch (tmp & PIPECONF_BPC_MASK) {
		case PIPECONF_6BPC:
			pipe_config->pipe_bpp = 18;
			break;
		case PIPECONF_8BPC:
			pipe_config->pipe_bpp = 24;
			break;
		case PIPECONF_10BPC:
			pipe_config->pipe_bpp = 30;
			break;
		default:
			break;
		}
	}

	if ((IS_VALLEYVIEW(dev_priv) || IS_CHERRYVIEW(dev_priv)) &&
	    (tmp & PIPECONF_COLOR_RANGE_SELECT))
		pipe_config->limited_color_range = true;

	pipe_config->gamma_mode = (tmp & PIPECONF_GAMMA_MODE_MASK_I9XX) >>
		PIPECONF_GAMMA_MODE_SHIFT;

	if (IS_CHERRYVIEW(dev_priv))
		pipe_config->cgm_mode = intel_de_read(dev_priv,
						      CGM_PIPE_MODE(crtc->pipe));

	i9xx_get_pipe_color_config(pipe_config);
	intel_color_get_config(pipe_config);

	if (DISPLAY_VER(dev_priv) < 4)
		pipe_config->double_wide = tmp & PIPECONF_DOUBLE_WIDE;

	intel_get_transcoder_timings(crtc, pipe_config);
	intel_get_pipe_src_size(crtc, pipe_config);

	i9xx_get_pfit_config(pipe_config);

	if (DISPLAY_VER(dev_priv) >= 4) {
		/* No way to read it out on pipes B and C */
		if (IS_CHERRYVIEW(dev_priv) && crtc->pipe != PIPE_A)
			tmp = dev_priv->chv_dpll_md[crtc->pipe];
		else
			tmp = intel_de_read(dev_priv, DPLL_MD(crtc->pipe));
		pipe_config->pixel_multiplier =
			((tmp & DPLL_MD_UDI_MULTIPLIER_MASK)
			 >> DPLL_MD_UDI_MULTIPLIER_SHIFT) + 1;
		pipe_config->dpll_hw_state.dpll_md = tmp;
	} else if (IS_I945G(dev_priv) || IS_I945GM(dev_priv) ||
		   IS_G33(dev_priv) || IS_PINEVIEW(dev_priv)) {
		tmp = intel_de_read(dev_priv, DPLL(crtc->pipe));
		pipe_config->pixel_multiplier =
			((tmp & SDVO_MULTIPLIER_MASK)
			 >> SDVO_MULTIPLIER_SHIFT_HIRES) + 1;
	} else {
		/* Note that on i915G/GM the pixel multiplier is in the sdvo
		 * port and will be fixed up in the encoder->get_config
		 * function. */
		pipe_config->pixel_multiplier = 1;
	}
	pipe_config->dpll_hw_state.dpll = intel_de_read(dev_priv,
							DPLL(crtc->pipe));
	if (!IS_VALLEYVIEW(dev_priv) && !IS_CHERRYVIEW(dev_priv)) {
		pipe_config->dpll_hw_state.fp0 = intel_de_read(dev_priv,
							       FP0(crtc->pipe));
		pipe_config->dpll_hw_state.fp1 = intel_de_read(dev_priv,
							       FP1(crtc->pipe));
	} else {
		/* Mask out read-only status bits. */
		pipe_config->dpll_hw_state.dpll &= ~(DPLL_LOCK_VLV |
						     DPLL_PORTC_READY_MASK |
						     DPLL_PORTB_READY_MASK);
	}

	if (IS_CHERRYVIEW(dev_priv))
		chv_crtc_clock_get(crtc, pipe_config);
	else if (IS_VALLEYVIEW(dev_priv))
		vlv_crtc_clock_get(crtc, pipe_config);
	else
		i9xx_crtc_clock_get(crtc, pipe_config);

	/*
	 * Normally the dotclock is filled in by the encoder .get_config()
	 * but in case the pipe is enabled w/o any ports we need a sane
	 * default.
	 */
	pipe_config->hw.adjusted_mode.crtc_clock =
		pipe_config->port_clock / pipe_config->pixel_multiplier;

	ret = true;

out:
	intel_display_power_put(dev_priv, power_domain, wakeref);

	return ret;
}

static void ilk_init_pch_refclk(struct drm_i915_private *dev_priv)
{
	struct intel_encoder *encoder;
	int i;
	u32 val, final;
	bool has_lvds = false;
	bool has_cpu_edp = false;
	bool has_panel = false;
	bool has_ck505 = false;
	bool can_ssc = false;
	bool using_ssc_source = false;

	/* We need to take the global config into account */
	for_each_intel_encoder(&dev_priv->drm, encoder) {
		switch (encoder->type) {
		case INTEL_OUTPUT_LVDS:
			has_panel = true;
			has_lvds = true;
			break;
		case INTEL_OUTPUT_EDP:
			has_panel = true;
			if (encoder->port == PORT_A)
				has_cpu_edp = true;
			break;
		default:
			break;
		}
	}

	if (HAS_PCH_IBX(dev_priv)) {
		has_ck505 = dev_priv->vbt.display_clock_mode;
		can_ssc = has_ck505;
	} else {
		has_ck505 = false;
		can_ssc = true;
	}

	/* Check if any DPLLs are using the SSC source */
	for (i = 0; i < dev_priv->dpll.num_shared_dpll; i++) {
		u32 temp = intel_de_read(dev_priv, PCH_DPLL(i));

		if (!(temp & DPLL_VCO_ENABLE))
			continue;

		if ((temp & PLL_REF_INPUT_MASK) ==
		    PLLB_REF_INPUT_SPREADSPECTRUMIN) {
			using_ssc_source = true;
			break;
		}
	}

	drm_dbg_kms(&dev_priv->drm,
		    "has_panel %d has_lvds %d has_ck505 %d using_ssc_source %d\n",
		    has_panel, has_lvds, has_ck505, using_ssc_source);

	/* Ironlake: try to setup display ref clock before DPLL
	 * enabling. This is only under driver's control after
	 * PCH B stepping, previous chipset stepping should be
	 * ignoring this setting.
	 */
	val = intel_de_read(dev_priv, PCH_DREF_CONTROL);

	/* As we must carefully and slowly disable/enable each source in turn,
	 * compute the final state we want first and check if we need to
	 * make any changes at all.
	 */
	final = val;
	final &= ~DREF_NONSPREAD_SOURCE_MASK;
	if (has_ck505)
		final |= DREF_NONSPREAD_CK505_ENABLE;
	else
		final |= DREF_NONSPREAD_SOURCE_ENABLE;

	final &= ~DREF_SSC_SOURCE_MASK;
	final &= ~DREF_CPU_SOURCE_OUTPUT_MASK;
	final &= ~DREF_SSC1_ENABLE;

	if (has_panel) {
		final |= DREF_SSC_SOURCE_ENABLE;

		if (intel_panel_use_ssc(dev_priv) && can_ssc)
			final |= DREF_SSC1_ENABLE;

		if (has_cpu_edp) {
			if (intel_panel_use_ssc(dev_priv) && can_ssc)
				final |= DREF_CPU_SOURCE_OUTPUT_DOWNSPREAD;
			else
				final |= DREF_CPU_SOURCE_OUTPUT_NONSPREAD;
		} else
			final |= DREF_CPU_SOURCE_OUTPUT_DISABLE;
	} else if (using_ssc_source) {
		final |= DREF_SSC_SOURCE_ENABLE;
		final |= DREF_SSC1_ENABLE;
	}

	if (final == val)
		return;

	/* Always enable nonspread source */
	val &= ~DREF_NONSPREAD_SOURCE_MASK;

	if (has_ck505)
		val |= DREF_NONSPREAD_CK505_ENABLE;
	else
		val |= DREF_NONSPREAD_SOURCE_ENABLE;

	if (has_panel) {
		val &= ~DREF_SSC_SOURCE_MASK;
		val |= DREF_SSC_SOURCE_ENABLE;

		/* SSC must be turned on before enabling the CPU output  */
		if (intel_panel_use_ssc(dev_priv) && can_ssc) {
			drm_dbg_kms(&dev_priv->drm, "Using SSC on panel\n");
			val |= DREF_SSC1_ENABLE;
		} else
			val &= ~DREF_SSC1_ENABLE;

		/* Get SSC going before enabling the outputs */
		intel_de_write(dev_priv, PCH_DREF_CONTROL, val);
		intel_de_posting_read(dev_priv, PCH_DREF_CONTROL);
		udelay(200);

		val &= ~DREF_CPU_SOURCE_OUTPUT_MASK;

		/* Enable CPU source on CPU attached eDP */
		if (has_cpu_edp) {
			if (intel_panel_use_ssc(dev_priv) && can_ssc) {
				drm_dbg_kms(&dev_priv->drm,
					    "Using SSC on eDP\n");
				val |= DREF_CPU_SOURCE_OUTPUT_DOWNSPREAD;
			} else
				val |= DREF_CPU_SOURCE_OUTPUT_NONSPREAD;
		} else
			val |= DREF_CPU_SOURCE_OUTPUT_DISABLE;

		intel_de_write(dev_priv, PCH_DREF_CONTROL, val);
		intel_de_posting_read(dev_priv, PCH_DREF_CONTROL);
		udelay(200);
	} else {
		drm_dbg_kms(&dev_priv->drm, "Disabling CPU source output\n");

		val &= ~DREF_CPU_SOURCE_OUTPUT_MASK;

		/* Turn off CPU output */
		val |= DREF_CPU_SOURCE_OUTPUT_DISABLE;

		intel_de_write(dev_priv, PCH_DREF_CONTROL, val);
		intel_de_posting_read(dev_priv, PCH_DREF_CONTROL);
		udelay(200);

		if (!using_ssc_source) {
			drm_dbg_kms(&dev_priv->drm, "Disabling SSC source\n");

			/* Turn off the SSC source */
			val &= ~DREF_SSC_SOURCE_MASK;
			val |= DREF_SSC_SOURCE_DISABLE;

			/* Turn off SSC1 */
			val &= ~DREF_SSC1_ENABLE;

			intel_de_write(dev_priv, PCH_DREF_CONTROL, val);
			intel_de_posting_read(dev_priv, PCH_DREF_CONTROL);
			udelay(200);
		}
	}

	BUG_ON(val != final);
}

static void lpt_reset_fdi_mphy(struct drm_i915_private *dev_priv)
{
	u32 tmp;

	tmp = intel_de_read(dev_priv, SOUTH_CHICKEN2);
	tmp |= FDI_MPHY_IOSFSB_RESET_CTL;
	intel_de_write(dev_priv, SOUTH_CHICKEN2, tmp);

	if (wait_for_us(intel_de_read(dev_priv, SOUTH_CHICKEN2) &
			FDI_MPHY_IOSFSB_RESET_STATUS, 100))
		drm_err(&dev_priv->drm, "FDI mPHY reset assert timeout\n");

	tmp = intel_de_read(dev_priv, SOUTH_CHICKEN2);
	tmp &= ~FDI_MPHY_IOSFSB_RESET_CTL;
	intel_de_write(dev_priv, SOUTH_CHICKEN2, tmp);

	if (wait_for_us((intel_de_read(dev_priv, SOUTH_CHICKEN2) &
			 FDI_MPHY_IOSFSB_RESET_STATUS) == 0, 100))
		drm_err(&dev_priv->drm, "FDI mPHY reset de-assert timeout\n");
}

/* WaMPhyProgramming:hsw */
static void lpt_program_fdi_mphy(struct drm_i915_private *dev_priv)
{
	u32 tmp;

	tmp = intel_sbi_read(dev_priv, 0x8008, SBI_MPHY);
	tmp &= ~(0xFF << 24);
	tmp |= (0x12 << 24);
	intel_sbi_write(dev_priv, 0x8008, tmp, SBI_MPHY);

	tmp = intel_sbi_read(dev_priv, 0x2008, SBI_MPHY);
	tmp |= (1 << 11);
	intel_sbi_write(dev_priv, 0x2008, tmp, SBI_MPHY);

	tmp = intel_sbi_read(dev_priv, 0x2108, SBI_MPHY);
	tmp |= (1 << 11);
	intel_sbi_write(dev_priv, 0x2108, tmp, SBI_MPHY);

	tmp = intel_sbi_read(dev_priv, 0x206C, SBI_MPHY);
	tmp |= (1 << 24) | (1 << 21) | (1 << 18);
	intel_sbi_write(dev_priv, 0x206C, tmp, SBI_MPHY);

	tmp = intel_sbi_read(dev_priv, 0x216C, SBI_MPHY);
	tmp |= (1 << 24) | (1 << 21) | (1 << 18);
	intel_sbi_write(dev_priv, 0x216C, tmp, SBI_MPHY);

	tmp = intel_sbi_read(dev_priv, 0x2080, SBI_MPHY);
	tmp &= ~(7 << 13);
	tmp |= (5 << 13);
	intel_sbi_write(dev_priv, 0x2080, tmp, SBI_MPHY);

	tmp = intel_sbi_read(dev_priv, 0x2180, SBI_MPHY);
	tmp &= ~(7 << 13);
	tmp |= (5 << 13);
	intel_sbi_write(dev_priv, 0x2180, tmp, SBI_MPHY);

	tmp = intel_sbi_read(dev_priv, 0x208C, SBI_MPHY);
	tmp &= ~0xFF;
	tmp |= 0x1C;
	intel_sbi_write(dev_priv, 0x208C, tmp, SBI_MPHY);

	tmp = intel_sbi_read(dev_priv, 0x218C, SBI_MPHY);
	tmp &= ~0xFF;
	tmp |= 0x1C;
	intel_sbi_write(dev_priv, 0x218C, tmp, SBI_MPHY);

	tmp = intel_sbi_read(dev_priv, 0x2098, SBI_MPHY);
	tmp &= ~(0xFF << 16);
	tmp |= (0x1C << 16);
	intel_sbi_write(dev_priv, 0x2098, tmp, SBI_MPHY);

	tmp = intel_sbi_read(dev_priv, 0x2198, SBI_MPHY);
	tmp &= ~(0xFF << 16);
	tmp |= (0x1C << 16);
	intel_sbi_write(dev_priv, 0x2198, tmp, SBI_MPHY);

	tmp = intel_sbi_read(dev_priv, 0x20C4, SBI_MPHY);
	tmp |= (1 << 27);
	intel_sbi_write(dev_priv, 0x20C4, tmp, SBI_MPHY);

	tmp = intel_sbi_read(dev_priv, 0x21C4, SBI_MPHY);
	tmp |= (1 << 27);
	intel_sbi_write(dev_priv, 0x21C4, tmp, SBI_MPHY);

	tmp = intel_sbi_read(dev_priv, 0x20EC, SBI_MPHY);
	tmp &= ~(0xF << 28);
	tmp |= (4 << 28);
	intel_sbi_write(dev_priv, 0x20EC, tmp, SBI_MPHY);

	tmp = intel_sbi_read(dev_priv, 0x21EC, SBI_MPHY);
	tmp &= ~(0xF << 28);
	tmp |= (4 << 28);
	intel_sbi_write(dev_priv, 0x21EC, tmp, SBI_MPHY);
}

/* Implements 3 different sequences from BSpec chapter "Display iCLK
 * Programming" based on the parameters passed:
 * - Sequence to enable CLKOUT_DP
 * - Sequence to enable CLKOUT_DP without spread
 * - Sequence to enable CLKOUT_DP for FDI usage and configure PCH FDI I/O
 */
static void lpt_enable_clkout_dp(struct drm_i915_private *dev_priv,
				 bool with_spread, bool with_fdi)
{
	u32 reg, tmp;

	if (drm_WARN(&dev_priv->drm, with_fdi && !with_spread,
		     "FDI requires downspread\n"))
		with_spread = true;
	if (drm_WARN(&dev_priv->drm, HAS_PCH_LPT_LP(dev_priv) &&
		     with_fdi, "LP PCH doesn't have FDI\n"))
		with_fdi = false;

	mutex_lock(&dev_priv->sb_lock);

	tmp = intel_sbi_read(dev_priv, SBI_SSCCTL, SBI_ICLK);
	tmp &= ~SBI_SSCCTL_DISABLE;
	tmp |= SBI_SSCCTL_PATHALT;
	intel_sbi_write(dev_priv, SBI_SSCCTL, tmp, SBI_ICLK);

	udelay(24);

	if (with_spread) {
		tmp = intel_sbi_read(dev_priv, SBI_SSCCTL, SBI_ICLK);
		tmp &= ~SBI_SSCCTL_PATHALT;
		intel_sbi_write(dev_priv, SBI_SSCCTL, tmp, SBI_ICLK);

		if (with_fdi) {
			lpt_reset_fdi_mphy(dev_priv);
			lpt_program_fdi_mphy(dev_priv);
		}
	}

	reg = HAS_PCH_LPT_LP(dev_priv) ? SBI_GEN0 : SBI_DBUFF0;
	tmp = intel_sbi_read(dev_priv, reg, SBI_ICLK);
	tmp |= SBI_GEN0_CFG_BUFFENABLE_DISABLE;
	intel_sbi_write(dev_priv, reg, tmp, SBI_ICLK);

	mutex_unlock(&dev_priv->sb_lock);
}

/* Sequence to disable CLKOUT_DP */
void lpt_disable_clkout_dp(struct drm_i915_private *dev_priv)
{
	u32 reg, tmp;

	mutex_lock(&dev_priv->sb_lock);

	reg = HAS_PCH_LPT_LP(dev_priv) ? SBI_GEN0 : SBI_DBUFF0;
	tmp = intel_sbi_read(dev_priv, reg, SBI_ICLK);
	tmp &= ~SBI_GEN0_CFG_BUFFENABLE_DISABLE;
	intel_sbi_write(dev_priv, reg, tmp, SBI_ICLK);

	tmp = intel_sbi_read(dev_priv, SBI_SSCCTL, SBI_ICLK);
	if (!(tmp & SBI_SSCCTL_DISABLE)) {
		if (!(tmp & SBI_SSCCTL_PATHALT)) {
			tmp |= SBI_SSCCTL_PATHALT;
			intel_sbi_write(dev_priv, SBI_SSCCTL, tmp, SBI_ICLK);
			udelay(32);
		}
		tmp |= SBI_SSCCTL_DISABLE;
		intel_sbi_write(dev_priv, SBI_SSCCTL, tmp, SBI_ICLK);
	}

	mutex_unlock(&dev_priv->sb_lock);
}

#define BEND_IDX(steps) ((50 + (steps)) / 5)

static const u16 sscdivintphase[] = {
	[BEND_IDX( 50)] = 0x3B23,
	[BEND_IDX( 45)] = 0x3B23,
	[BEND_IDX( 40)] = 0x3C23,
	[BEND_IDX( 35)] = 0x3C23,
	[BEND_IDX( 30)] = 0x3D23,
	[BEND_IDX( 25)] = 0x3D23,
	[BEND_IDX( 20)] = 0x3E23,
	[BEND_IDX( 15)] = 0x3E23,
	[BEND_IDX( 10)] = 0x3F23,
	[BEND_IDX(  5)] = 0x3F23,
	[BEND_IDX(  0)] = 0x0025,
	[BEND_IDX( -5)] = 0x0025,
	[BEND_IDX(-10)] = 0x0125,
	[BEND_IDX(-15)] = 0x0125,
	[BEND_IDX(-20)] = 0x0225,
	[BEND_IDX(-25)] = 0x0225,
	[BEND_IDX(-30)] = 0x0325,
	[BEND_IDX(-35)] = 0x0325,
	[BEND_IDX(-40)] = 0x0425,
	[BEND_IDX(-45)] = 0x0425,
	[BEND_IDX(-50)] = 0x0525,
};

/*
 * Bend CLKOUT_DP
 * steps -50 to 50 inclusive, in steps of 5
 * < 0 slow down the clock, > 0 speed up the clock, 0 == no bend (135MHz)
 * change in clock period = -(steps / 10) * 5.787 ps
 */
static void lpt_bend_clkout_dp(struct drm_i915_private *dev_priv, int steps)
{
	u32 tmp;
	int idx = BEND_IDX(steps);

	if (drm_WARN_ON(&dev_priv->drm, steps % 5 != 0))
		return;

	if (drm_WARN_ON(&dev_priv->drm, idx >= ARRAY_SIZE(sscdivintphase)))
		return;

	mutex_lock(&dev_priv->sb_lock);

	if (steps % 10 != 0)
		tmp = 0xAAAAAAAB;
	else
		tmp = 0x00000000;
	intel_sbi_write(dev_priv, SBI_SSCDITHPHASE, tmp, SBI_ICLK);

	tmp = intel_sbi_read(dev_priv, SBI_SSCDIVINTPHASE, SBI_ICLK);
	tmp &= 0xffff0000;
	tmp |= sscdivintphase[idx];
	intel_sbi_write(dev_priv, SBI_SSCDIVINTPHASE, tmp, SBI_ICLK);

	mutex_unlock(&dev_priv->sb_lock);
}

#undef BEND_IDX

static bool spll_uses_pch_ssc(struct drm_i915_private *dev_priv)
{
	u32 fuse_strap = intel_de_read(dev_priv, FUSE_STRAP);
	u32 ctl = intel_de_read(dev_priv, SPLL_CTL);

	if ((ctl & SPLL_PLL_ENABLE) == 0)
		return false;

	if ((ctl & SPLL_REF_MASK) == SPLL_REF_MUXED_SSC &&
	    (fuse_strap & HSW_CPU_SSC_ENABLE) == 0)
		return true;

	if (IS_BROADWELL(dev_priv) &&
	    (ctl & SPLL_REF_MASK) == SPLL_REF_PCH_SSC_BDW)
		return true;

	return false;
}

static bool wrpll_uses_pch_ssc(struct drm_i915_private *dev_priv,
			       enum intel_dpll_id id)
{
	u32 fuse_strap = intel_de_read(dev_priv, FUSE_STRAP);
	u32 ctl = intel_de_read(dev_priv, WRPLL_CTL(id));

	if ((ctl & WRPLL_PLL_ENABLE) == 0)
		return false;

	if ((ctl & WRPLL_REF_MASK) == WRPLL_REF_PCH_SSC)
		return true;

	if ((IS_BROADWELL(dev_priv) || IS_HSW_ULT(dev_priv)) &&
	    (ctl & WRPLL_REF_MASK) == WRPLL_REF_MUXED_SSC_BDW &&
	    (fuse_strap & HSW_CPU_SSC_ENABLE) == 0)
		return true;

	return false;
}

static void lpt_init_pch_refclk(struct drm_i915_private *dev_priv)
{
	struct intel_encoder *encoder;
	bool has_fdi = false;

	for_each_intel_encoder(&dev_priv->drm, encoder) {
		switch (encoder->type) {
		case INTEL_OUTPUT_ANALOG:
			has_fdi = true;
			break;
		default:
			break;
		}
	}

	/*
	 * The BIOS may have decided to use the PCH SSC
	 * reference so we must not disable it until the
	 * relevant PLLs have stopped relying on it. We'll
	 * just leave the PCH SSC reference enabled in case
	 * any active PLL is using it. It will get disabled
	 * after runtime suspend if we don't have FDI.
	 *
	 * TODO: Move the whole reference clock handling
	 * to the modeset sequence proper so that we can
	 * actually enable/disable/reconfigure these things
	 * safely. To do that we need to introduce a real
	 * clock hierarchy. That would also allow us to do
	 * clock bending finally.
	 */
	dev_priv->pch_ssc_use = 0;

	if (spll_uses_pch_ssc(dev_priv)) {
		drm_dbg_kms(&dev_priv->drm, "SPLL using PCH SSC\n");
		dev_priv->pch_ssc_use |= BIT(DPLL_ID_SPLL);
	}

	if (wrpll_uses_pch_ssc(dev_priv, DPLL_ID_WRPLL1)) {
		drm_dbg_kms(&dev_priv->drm, "WRPLL1 using PCH SSC\n");
		dev_priv->pch_ssc_use |= BIT(DPLL_ID_WRPLL1);
	}

	if (wrpll_uses_pch_ssc(dev_priv, DPLL_ID_WRPLL2)) {
		drm_dbg_kms(&dev_priv->drm, "WRPLL2 using PCH SSC\n");
		dev_priv->pch_ssc_use |= BIT(DPLL_ID_WRPLL2);
	}

	if (dev_priv->pch_ssc_use)
		return;

	if (has_fdi) {
		lpt_bend_clkout_dp(dev_priv, 0);
		lpt_enable_clkout_dp(dev_priv, true, true);
	} else {
		lpt_disable_clkout_dp(dev_priv);
	}
}

/*
 * Initialize reference clocks when the driver loads
 */
void intel_init_pch_refclk(struct drm_i915_private *dev_priv)
{
	if (HAS_PCH_IBX(dev_priv) || HAS_PCH_CPT(dev_priv))
		ilk_init_pch_refclk(dev_priv);
	else if (HAS_PCH_LPT(dev_priv))
		lpt_init_pch_refclk(dev_priv);
}

static void ilk_set_pipeconf(const struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	enum pipe pipe = crtc->pipe;
	u32 val;

	val = 0;

	switch (crtc_state->pipe_bpp) {
	case 18:
		val |= PIPECONF_6BPC;
		break;
	case 24:
		val |= PIPECONF_8BPC;
		break;
	case 30:
		val |= PIPECONF_10BPC;
		break;
	case 36:
		val |= PIPECONF_12BPC;
		break;
	default:
		/* Case prevented by intel_choose_pipe_bpp_dither. */
		BUG();
	}

	if (crtc_state->dither)
		val |= (PIPECONF_DITHER_EN | PIPECONF_DITHER_TYPE_SP);

	if (crtc_state->hw.adjusted_mode.flags & DRM_MODE_FLAG_INTERLACE)
		val |= PIPECONF_INTERLACED_ILK;
	else
		val |= PIPECONF_PROGRESSIVE;

	/*
	 * This would end up with an odd purple hue over
	 * the entire display. Make sure we don't do it.
	 */
	drm_WARN_ON(&dev_priv->drm, crtc_state->limited_color_range &&
		    crtc_state->output_format != INTEL_OUTPUT_FORMAT_RGB);

	if (crtc_state->limited_color_range &&
	    !intel_crtc_has_type(crtc_state, INTEL_OUTPUT_SDVO))
		val |= PIPECONF_COLOR_RANGE_SELECT;

	if (crtc_state->output_format != INTEL_OUTPUT_FORMAT_RGB)
		val |= PIPECONF_OUTPUT_COLORSPACE_YUV709;

	val |= PIPECONF_GAMMA_MODE(crtc_state->gamma_mode);

	val |= PIPECONF_FRAME_START_DELAY(dev_priv->framestart_delay - 1);

	intel_de_write(dev_priv, PIPECONF(pipe), val);
	intel_de_posting_read(dev_priv, PIPECONF(pipe));
}

static void hsw_set_pipeconf(const struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	enum transcoder cpu_transcoder = crtc_state->cpu_transcoder;
	u32 val = 0;

	if (IS_HASWELL(dev_priv) && crtc_state->dither)
		val |= (PIPECONF_DITHER_EN | PIPECONF_DITHER_TYPE_SP);

	if (crtc_state->hw.adjusted_mode.flags & DRM_MODE_FLAG_INTERLACE)
		val |= PIPECONF_INTERLACED_ILK;
	else
		val |= PIPECONF_PROGRESSIVE;

	if (IS_HASWELL(dev_priv) &&
	    crtc_state->output_format != INTEL_OUTPUT_FORMAT_RGB)
		val |= PIPECONF_OUTPUT_COLORSPACE_YUV_HSW;

	intel_de_write(dev_priv, PIPECONF(cpu_transcoder), val);
	intel_de_posting_read(dev_priv, PIPECONF(cpu_transcoder));
}

static void bdw_set_pipemisc(const struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	const struct intel_crtc_scaler_state *scaler_state =
		&crtc_state->scaler_state;

	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	u32 val = 0;
	int i;

	switch (crtc_state->pipe_bpp) {
	case 18:
		val |= PIPEMISC_DITHER_6_BPC;
		break;
	case 24:
		val |= PIPEMISC_DITHER_8_BPC;
		break;
	case 30:
		val |= PIPEMISC_DITHER_10_BPC;
		break;
	case 36:
		val |= PIPEMISC_DITHER_12_BPC;
		break;
	default:
		MISSING_CASE(crtc_state->pipe_bpp);
		break;
	}

	if (crtc_state->dither)
		val |= PIPEMISC_DITHER_ENABLE | PIPEMISC_DITHER_TYPE_SP;

	if (crtc_state->output_format == INTEL_OUTPUT_FORMAT_YCBCR420 ||
	    crtc_state->output_format == INTEL_OUTPUT_FORMAT_YCBCR444)
		val |= PIPEMISC_OUTPUT_COLORSPACE_YUV;

	if (crtc_state->output_format == INTEL_OUTPUT_FORMAT_YCBCR420)
		val |= PIPEMISC_YUV420_ENABLE |
			PIPEMISC_YUV420_MODE_FULL_BLEND;

	if (DISPLAY_VER(dev_priv) >= 11 &&
	    (crtc_state->active_planes & ~(icl_hdr_plane_mask() |
					   BIT(PLANE_CURSOR))) == 0)
		val |= PIPEMISC_HDR_MODE_PRECISION;

	if (DISPLAY_VER(dev_priv) >= 12)
		val |= PIPEMISC_PIXEL_ROUNDING_TRUNC;

	if (IS_ALDERLAKE_P(dev_priv)) {
		bool scaler_in_use = false;

		for (i = 0; i < crtc->num_scalers; i++) {
			if (!scaler_state->scalers[i].in_use)
				continue;

			scaler_in_use = true;
			break;
		}

		intel_de_rmw(dev_priv, PIPE_MISC2(crtc->pipe),
			     PIPE_MISC2_UNDERRUN_BUBBLE_COUNTER_MASK,
			     scaler_in_use ? PIPE_MISC2_BUBBLE_COUNTER_SCALER_EN :
			     PIPE_MISC2_BUBBLE_COUNTER_SCALER_DIS);
	}

	intel_de_write(dev_priv, PIPEMISC(crtc->pipe), val);
}

int bdw_get_pipemisc_bpp(struct intel_crtc *crtc)
{
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	u32 tmp;

	tmp = intel_de_read(dev_priv, PIPEMISC(crtc->pipe));

	switch (tmp & PIPEMISC_DITHER_BPC_MASK) {
	case PIPEMISC_DITHER_6_BPC:
		return 18;
	case PIPEMISC_DITHER_8_BPC:
		return 24;
	case PIPEMISC_DITHER_10_BPC:
		return 30;
	case PIPEMISC_DITHER_12_BPC:
		return 36;
	default:
		MISSING_CASE(tmp);
		return 0;
	}
}

int ilk_get_lanes_required(int target_clock, int link_bw, int bpp)
{
	/*
	 * Account for spread spectrum to avoid
	 * oversubscribing the link. Max center spread
	 * is 2.5%; use 5% for safety's sake.
	 */
	u32 bps = target_clock * bpp * 21 / 20;
	return DIV_ROUND_UP(bps, link_bw * 8);
}

static void intel_pch_transcoder_get_m_n(struct intel_crtc *crtc,
					 struct intel_link_m_n *m_n)
{
	struct drm_device *dev = crtc->base.dev;
	struct drm_i915_private *dev_priv = to_i915(dev);
	enum pipe pipe = crtc->pipe;

	m_n->link_m = intel_de_read(dev_priv, PCH_TRANS_LINK_M1(pipe));
	m_n->link_n = intel_de_read(dev_priv, PCH_TRANS_LINK_N1(pipe));
	m_n->gmch_m = intel_de_read(dev_priv, PCH_TRANS_DATA_M1(pipe))
		& ~TU_SIZE_MASK;
	m_n->gmch_n = intel_de_read(dev_priv, PCH_TRANS_DATA_N1(pipe));
	m_n->tu = ((intel_de_read(dev_priv, PCH_TRANS_DATA_M1(pipe))
		    & TU_SIZE_MASK) >> TU_SIZE_SHIFT) + 1;
}

static void intel_cpu_transcoder_get_m_n(struct intel_crtc *crtc,
					 enum transcoder transcoder,
					 struct intel_link_m_n *m_n,
					 struct intel_link_m_n *m2_n2)
{
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	enum pipe pipe = crtc->pipe;

	if (DISPLAY_VER(dev_priv) >= 5) {
		m_n->link_m = intel_de_read(dev_priv,
					    PIPE_LINK_M1(transcoder));
		m_n->link_n = intel_de_read(dev_priv,
					    PIPE_LINK_N1(transcoder));
		m_n->gmch_m = intel_de_read(dev_priv,
					    PIPE_DATA_M1(transcoder))
			& ~TU_SIZE_MASK;
		m_n->gmch_n = intel_de_read(dev_priv,
					    PIPE_DATA_N1(transcoder));
		m_n->tu = ((intel_de_read(dev_priv, PIPE_DATA_M1(transcoder))
			    & TU_SIZE_MASK) >> TU_SIZE_SHIFT) + 1;

		if (m2_n2 && transcoder_has_m2_n2(dev_priv, transcoder)) {
			m2_n2->link_m = intel_de_read(dev_priv,
						      PIPE_LINK_M2(transcoder));
			m2_n2->link_n =	intel_de_read(dev_priv,
							     PIPE_LINK_N2(transcoder));
			m2_n2->gmch_m =	intel_de_read(dev_priv,
							     PIPE_DATA_M2(transcoder))
					& ~TU_SIZE_MASK;
			m2_n2->gmch_n =	intel_de_read(dev_priv,
							     PIPE_DATA_N2(transcoder));
			m2_n2->tu = ((intel_de_read(dev_priv, PIPE_DATA_M2(transcoder))
					& TU_SIZE_MASK) >> TU_SIZE_SHIFT) + 1;
		}
	} else {
		m_n->link_m = intel_de_read(dev_priv, PIPE_LINK_M_G4X(pipe));
		m_n->link_n = intel_de_read(dev_priv, PIPE_LINK_N_G4X(pipe));
		m_n->gmch_m = intel_de_read(dev_priv, PIPE_DATA_M_G4X(pipe))
			& ~TU_SIZE_MASK;
		m_n->gmch_n = intel_de_read(dev_priv, PIPE_DATA_N_G4X(pipe));
		m_n->tu = ((intel_de_read(dev_priv, PIPE_DATA_M_G4X(pipe))
			    & TU_SIZE_MASK) >> TU_SIZE_SHIFT) + 1;
	}
}

void intel_dp_get_m_n(struct intel_crtc *crtc,
		      struct intel_crtc_state *pipe_config)
{
	if (pipe_config->has_pch_encoder)
		intel_pch_transcoder_get_m_n(crtc, &pipe_config->dp_m_n);
	else
		intel_cpu_transcoder_get_m_n(crtc, pipe_config->cpu_transcoder,
					     &pipe_config->dp_m_n,
					     &pipe_config->dp_m2_n2);
}

static void ilk_get_fdi_m_n_config(struct intel_crtc *crtc,
				   struct intel_crtc_state *pipe_config)
{
	intel_cpu_transcoder_get_m_n(crtc, pipe_config->cpu_transcoder,
				     &pipe_config->fdi_m_n, NULL);
}

static void ilk_get_pfit_pos_size(struct intel_crtc_state *crtc_state,
				  u32 pos, u32 size)
{
	drm_rect_init(&crtc_state->pch_pfit.dst,
		      pos >> 16, pos & 0xffff,
		      size >> 16, size & 0xffff);
}

static void skl_get_pfit_config(struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	struct intel_crtc_scaler_state *scaler_state = &crtc_state->scaler_state;
	int id = -1;
	int i;

	/* find scaler attached to this pipe */
	for (i = 0; i < crtc->num_scalers; i++) {
		u32 ctl, pos, size;

		ctl = intel_de_read(dev_priv, SKL_PS_CTRL(crtc->pipe, i));
		if ((ctl & (PS_SCALER_EN | PS_PLANE_SEL_MASK)) != PS_SCALER_EN)
			continue;

		id = i;
		crtc_state->pch_pfit.enabled = true;

		pos = intel_de_read(dev_priv, SKL_PS_WIN_POS(crtc->pipe, i));
		size = intel_de_read(dev_priv, SKL_PS_WIN_SZ(crtc->pipe, i));

		ilk_get_pfit_pos_size(crtc_state, pos, size);

		scaler_state->scalers[i].in_use = true;
		break;
	}

	scaler_state->scaler_id = id;
	if (id >= 0)
		scaler_state->scaler_users |= (1 << SKL_CRTC_INDEX);
	else
		scaler_state->scaler_users &= ~(1 << SKL_CRTC_INDEX);
}

static void ilk_get_pfit_config(struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	u32 ctl, pos, size;

	ctl = intel_de_read(dev_priv, PF_CTL(crtc->pipe));
	if ((ctl & PF_ENABLE) == 0)
		return;

	crtc_state->pch_pfit.enabled = true;

	pos = intel_de_read(dev_priv, PF_WIN_POS(crtc->pipe));
	size = intel_de_read(dev_priv, PF_WIN_SZ(crtc->pipe));

	ilk_get_pfit_pos_size(crtc_state, pos, size);

	/*
	 * We currently do not free assignements of panel fitters on
	 * ivb/hsw (since we don't use the higher upscaling modes which
	 * differentiates them) so just WARN about this case for now.
	 */
	drm_WARN_ON(&dev_priv->drm, DISPLAY_VER(dev_priv) == 7 &&
		    (ctl & PF_PIPE_SEL_MASK_IVB) != PF_PIPE_SEL_IVB(crtc->pipe));
}

static bool ilk_get_pipe_config(struct intel_crtc *crtc,
				struct intel_crtc_state *pipe_config)
{
	struct drm_device *dev = crtc->base.dev;
	struct drm_i915_private *dev_priv = to_i915(dev);
	enum intel_display_power_domain power_domain;
	intel_wakeref_t wakeref;
	u32 tmp;
	bool ret;

	power_domain = POWER_DOMAIN_PIPE(crtc->pipe);
	wakeref = intel_display_power_get_if_enabled(dev_priv, power_domain);
	if (!wakeref)
		return false;

	pipe_config->cpu_transcoder = (enum transcoder) crtc->pipe;
	pipe_config->shared_dpll = NULL;

	ret = false;
	tmp = intel_de_read(dev_priv, PIPECONF(crtc->pipe));
	if (!(tmp & PIPECONF_ENABLE))
		goto out;

	switch (tmp & PIPECONF_BPC_MASK) {
	case PIPECONF_6BPC:
		pipe_config->pipe_bpp = 18;
		break;
	case PIPECONF_8BPC:
		pipe_config->pipe_bpp = 24;
		break;
	case PIPECONF_10BPC:
		pipe_config->pipe_bpp = 30;
		break;
	case PIPECONF_12BPC:
		pipe_config->pipe_bpp = 36;
		break;
	default:
		break;
	}

	if (tmp & PIPECONF_COLOR_RANGE_SELECT)
		pipe_config->limited_color_range = true;

	switch (tmp & PIPECONF_OUTPUT_COLORSPACE_MASK) {
	case PIPECONF_OUTPUT_COLORSPACE_YUV601:
	case PIPECONF_OUTPUT_COLORSPACE_YUV709:
		pipe_config->output_format = INTEL_OUTPUT_FORMAT_YCBCR444;
		break;
	default:
		pipe_config->output_format = INTEL_OUTPUT_FORMAT_RGB;
		break;
	}

	pipe_config->gamma_mode = (tmp & PIPECONF_GAMMA_MODE_MASK_ILK) >>
		PIPECONF_GAMMA_MODE_SHIFT;

	pipe_config->csc_mode = intel_de_read(dev_priv,
					      PIPE_CSC_MODE(crtc->pipe));

	i9xx_get_pipe_color_config(pipe_config);
	intel_color_get_config(pipe_config);

	if (intel_de_read(dev_priv, PCH_TRANSCONF(crtc->pipe)) & TRANS_ENABLE) {
		struct intel_shared_dpll *pll;
		enum intel_dpll_id pll_id;
		bool pll_active;

		pipe_config->has_pch_encoder = true;

		tmp = intel_de_read(dev_priv, FDI_RX_CTL(crtc->pipe));
		pipe_config->fdi_lanes = ((FDI_DP_PORT_WIDTH_MASK & tmp) >>
					  FDI_DP_PORT_WIDTH_SHIFT) + 1;

		ilk_get_fdi_m_n_config(crtc, pipe_config);

		if (HAS_PCH_IBX(dev_priv)) {
			/*
			 * The pipe->pch transcoder and pch transcoder->pll
			 * mapping is fixed.
			 */
			pll_id = (enum intel_dpll_id) crtc->pipe;
		} else {
			tmp = intel_de_read(dev_priv, PCH_DPLL_SEL);
			if (tmp & TRANS_DPLLB_SEL(crtc->pipe))
				pll_id = DPLL_ID_PCH_PLL_B;
			else
				pll_id= DPLL_ID_PCH_PLL_A;
		}

		pipe_config->shared_dpll =
			intel_get_shared_dpll_by_id(dev_priv, pll_id);
		pll = pipe_config->shared_dpll;

		pll_active = intel_dpll_get_hw_state(dev_priv, pll,
						     &pipe_config->dpll_hw_state);
		drm_WARN_ON(dev, !pll_active);

		tmp = pipe_config->dpll_hw_state.dpll;
		pipe_config->pixel_multiplier =
			((tmp & PLL_REF_SDVO_HDMI_MULTIPLIER_MASK)
			 >> PLL_REF_SDVO_HDMI_MULTIPLIER_SHIFT) + 1;

		ilk_pch_clock_get(crtc, pipe_config);
	} else {
		pipe_config->pixel_multiplier = 1;
	}

	intel_get_transcoder_timings(crtc, pipe_config);
	intel_get_pipe_src_size(crtc, pipe_config);

	ilk_get_pfit_config(pipe_config);

	ret = true;

out:
	intel_display_power_put(dev_priv, power_domain, wakeref);

	return ret;
}

static bool hsw_get_transcoder_state(struct intel_crtc *crtc,
				     struct intel_crtc_state *pipe_config,
				     struct intel_display_power_domain_set *power_domain_set)
{
	struct drm_device *dev = crtc->base.dev;
	struct drm_i915_private *dev_priv = to_i915(dev);
	unsigned long panel_transcoder_mask = BIT(TRANSCODER_EDP);
	unsigned long enabled_panel_transcoders = 0;
	enum transcoder panel_transcoder;
	u32 tmp;

	if (DISPLAY_VER(dev_priv) >= 11)
		panel_transcoder_mask |=
			BIT(TRANSCODER_DSI_0) | BIT(TRANSCODER_DSI_1);

	/*
	 * The pipe->transcoder mapping is fixed with the exception of the eDP
	 * and DSI transcoders handled below.
	 */
	pipe_config->cpu_transcoder = (enum transcoder) crtc->pipe;

	/*
	 * XXX: Do intel_display_power_get_if_enabled before reading this (for
	 * consistency and less surprising code; it's in always on power).
	 */
	for_each_cpu_transcoder_masked(dev_priv, panel_transcoder,
				       panel_transcoder_mask) {
		bool force_thru = false;
		enum pipe trans_pipe;

		tmp = intel_de_read(dev_priv,
				    TRANS_DDI_FUNC_CTL(panel_transcoder));
		if (!(tmp & TRANS_DDI_FUNC_ENABLE))
			continue;

		/*
		 * Log all enabled ones, only use the first one.
		 *
		 * FIXME: This won't work for two separate DSI displays.
		 */
		enabled_panel_transcoders |= BIT(panel_transcoder);
		if (enabled_panel_transcoders != BIT(panel_transcoder))
			continue;

		switch (tmp & TRANS_DDI_EDP_INPUT_MASK) {
		default:
			drm_WARN(dev, 1,
				 "unknown pipe linked to transcoder %s\n",
				 transcoder_name(panel_transcoder));
			fallthrough;
		case TRANS_DDI_EDP_INPUT_A_ONOFF:
			force_thru = true;
			fallthrough;
		case TRANS_DDI_EDP_INPUT_A_ON:
			trans_pipe = PIPE_A;
			break;
		case TRANS_DDI_EDP_INPUT_B_ONOFF:
			trans_pipe = PIPE_B;
			break;
		case TRANS_DDI_EDP_INPUT_C_ONOFF:
			trans_pipe = PIPE_C;
			break;
		case TRANS_DDI_EDP_INPUT_D_ONOFF:
			trans_pipe = PIPE_D;
			break;
		}

		if (trans_pipe == crtc->pipe) {
			pipe_config->cpu_transcoder = panel_transcoder;
			pipe_config->pch_pfit.force_thru = force_thru;
		}
	}

	/*
	 * Valid combos: none, eDP, DSI0, DSI1, DSI0+DSI1
	 */
	drm_WARN_ON(dev, (enabled_panel_transcoders & BIT(TRANSCODER_EDP)) &&
		    enabled_panel_transcoders != BIT(TRANSCODER_EDP));

	if (!intel_display_power_get_in_set_if_enabled(dev_priv, power_domain_set,
						       POWER_DOMAIN_TRANSCODER(pipe_config->cpu_transcoder)))
		return false;

	tmp = intel_de_read(dev_priv, PIPECONF(pipe_config->cpu_transcoder));

	return tmp & PIPECONF_ENABLE;
}

static bool bxt_get_dsi_transcoder_state(struct intel_crtc *crtc,
					 struct intel_crtc_state *pipe_config,
					 struct intel_display_power_domain_set *power_domain_set)
{
	struct drm_device *dev = crtc->base.dev;
	struct drm_i915_private *dev_priv = to_i915(dev);
	enum transcoder cpu_transcoder;
	enum port port;
	u32 tmp;

	for_each_port_masked(port, BIT(PORT_A) | BIT(PORT_C)) {
		if (port == PORT_A)
			cpu_transcoder = TRANSCODER_DSI_A;
		else
			cpu_transcoder = TRANSCODER_DSI_C;

		if (!intel_display_power_get_in_set_if_enabled(dev_priv, power_domain_set,
							       POWER_DOMAIN_TRANSCODER(cpu_transcoder)))
			continue;

		/*
		 * The PLL needs to be enabled with a valid divider
		 * configuration, otherwise accessing DSI registers will hang
		 * the machine. See BSpec North Display Engine
		 * registers/MIPI[BXT]. We can break out here early, since we
		 * need the same DSI PLL to be enabled for both DSI ports.
		 */
		if (!bxt_dsi_pll_is_enabled(dev_priv))
			break;

		/* XXX: this works for video mode only */
		tmp = intel_de_read(dev_priv, BXT_MIPI_PORT_CTRL(port));
		if (!(tmp & DPI_ENABLE))
			continue;

		tmp = intel_de_read(dev_priv, MIPI_CTRL(port));
		if ((tmp & BXT_PIPE_SELECT_MASK) != BXT_PIPE_SELECT(crtc->pipe))
			continue;

		pipe_config->cpu_transcoder = cpu_transcoder;
		break;
	}

	return transcoder_is_dsi(pipe_config->cpu_transcoder);
}

static void hsw_get_ddi_port_state(struct intel_crtc *crtc,
				   struct intel_crtc_state *pipe_config)
{
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	enum transcoder cpu_transcoder = pipe_config->cpu_transcoder;
	enum port port;
	u32 tmp;

	if (transcoder_is_dsi(cpu_transcoder)) {
		port = (cpu_transcoder == TRANSCODER_DSI_A) ?
						PORT_A : PORT_B;
	} else {
		tmp = intel_de_read(dev_priv,
				    TRANS_DDI_FUNC_CTL(cpu_transcoder));
		if (!(tmp & TRANS_DDI_FUNC_ENABLE))
			return;
		if (DISPLAY_VER(dev_priv) >= 12)
			port = TGL_TRANS_DDI_FUNC_CTL_VAL_TO_PORT(tmp);
		else
			port = TRANS_DDI_FUNC_CTL_VAL_TO_PORT(tmp);
	}

	/*
	 * Haswell has only FDI/PCH transcoder A. It is which is connected to
	 * DDI E. So just check whether this pipe is wired to DDI E and whether
	 * the PCH transcoder is on.
	 */
	if (DISPLAY_VER(dev_priv) < 9 &&
	    (port == PORT_E) && intel_de_read(dev_priv, LPT_TRANSCONF) & TRANS_ENABLE) {
		pipe_config->has_pch_encoder = true;

		tmp = intel_de_read(dev_priv, FDI_RX_CTL(PIPE_A));
		pipe_config->fdi_lanes = ((FDI_DP_PORT_WIDTH_MASK & tmp) >>
					  FDI_DP_PORT_WIDTH_SHIFT) + 1;

		ilk_get_fdi_m_n_config(crtc, pipe_config);
	}
}

static bool hsw_get_pipe_config(struct intel_crtc *crtc,
				struct intel_crtc_state *pipe_config)
{
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	struct intel_display_power_domain_set power_domain_set = { };
	bool active;
	u32 tmp;

	if (!intel_display_power_get_in_set_if_enabled(dev_priv, &power_domain_set,
						       POWER_DOMAIN_PIPE(crtc->pipe)))
		return false;

	pipe_config->shared_dpll = NULL;

	active = hsw_get_transcoder_state(crtc, pipe_config, &power_domain_set);

	if ((IS_GEMINILAKE(dev_priv) || IS_BROXTON(dev_priv)) &&
	    bxt_get_dsi_transcoder_state(crtc, pipe_config, &power_domain_set)) {
		drm_WARN_ON(&dev_priv->drm, active);
		active = true;
	}

	intel_dsc_get_config(pipe_config);
	if (DISPLAY_VER(dev_priv) >= 13 && !pipe_config->dsc.compression_enable)
		intel_uncompressed_joiner_get_config(pipe_config);

	if (!active) {
		/* bigjoiner slave doesn't enable transcoder */
		if (!pipe_config->bigjoiner_slave)
			goto out;

		active = true;
		pipe_config->pixel_multiplier = 1;

		/* we cannot read out most state, so don't bother.. */
		pipe_config->quirks |= PIPE_CONFIG_QUIRK_BIGJOINER_SLAVE;
	} else if (!transcoder_is_dsi(pipe_config->cpu_transcoder) ||
	    DISPLAY_VER(dev_priv) >= 11) {
		hsw_get_ddi_port_state(crtc, pipe_config);
		intel_get_transcoder_timings(crtc, pipe_config);
	}

	if (HAS_VRR(dev_priv) && !transcoder_is_dsi(pipe_config->cpu_transcoder))
		intel_vrr_get_config(crtc, pipe_config);

	intel_get_pipe_src_size(crtc, pipe_config);

	if (IS_HASWELL(dev_priv)) {
		u32 tmp = intel_de_read(dev_priv,
					PIPECONF(pipe_config->cpu_transcoder));

		if (tmp & PIPECONF_OUTPUT_COLORSPACE_YUV_HSW)
			pipe_config->output_format = INTEL_OUTPUT_FORMAT_YCBCR444;
		else
			pipe_config->output_format = INTEL_OUTPUT_FORMAT_RGB;
	} else {
		pipe_config->output_format =
			bdw_get_pipemisc_output_format(crtc);
	}

	pipe_config->gamma_mode = intel_de_read(dev_priv,
						GAMMA_MODE(crtc->pipe));

	pipe_config->csc_mode = intel_de_read(dev_priv,
					      PIPE_CSC_MODE(crtc->pipe));

	if (DISPLAY_VER(dev_priv) >= 9) {
		tmp = intel_de_read(dev_priv, SKL_BOTTOM_COLOR(crtc->pipe));

		if (tmp & SKL_BOTTOM_COLOR_GAMMA_ENABLE)
			pipe_config->gamma_enable = true;

		if (tmp & SKL_BOTTOM_COLOR_CSC_ENABLE)
			pipe_config->csc_enable = true;
	} else {
		i9xx_get_pipe_color_config(pipe_config);
	}

	intel_color_get_config(pipe_config);

	tmp = intel_de_read(dev_priv, WM_LINETIME(crtc->pipe));
	pipe_config->linetime = REG_FIELD_GET(HSW_LINETIME_MASK, tmp);
	if (IS_BROADWELL(dev_priv) || IS_HASWELL(dev_priv))
		pipe_config->ips_linetime =
			REG_FIELD_GET(HSW_IPS_LINETIME_MASK, tmp);

	if (intel_display_power_get_in_set_if_enabled(dev_priv, &power_domain_set,
						      POWER_DOMAIN_PIPE_PANEL_FITTER(crtc->pipe))) {
		if (DISPLAY_VER(dev_priv) >= 9)
			skl_get_pfit_config(pipe_config);
		else
			ilk_get_pfit_config(pipe_config);
	}

	if (hsw_crtc_supports_ips(crtc)) {
		if (IS_HASWELL(dev_priv))
			pipe_config->ips_enabled = intel_de_read(dev_priv,
								 IPS_CTL) & IPS_ENABLE;
		else {
			/*
			 * We cannot readout IPS state on broadwell, set to
			 * true so we can set it to a defined state on first
			 * commit.
			 */
			pipe_config->ips_enabled = true;
		}
	}

	if (pipe_config->bigjoiner_slave) {
		/* Cannot be read out as a slave, set to 0. */
		pipe_config->pixel_multiplier = 0;
	} else if (pipe_config->cpu_transcoder != TRANSCODER_EDP &&
	    !transcoder_is_dsi(pipe_config->cpu_transcoder)) {
		pipe_config->pixel_multiplier =
			intel_de_read(dev_priv,
				      PIPE_MULT(pipe_config->cpu_transcoder)) + 1;
	} else {
		pipe_config->pixel_multiplier = 1;
	}

out:
	intel_display_power_put_all_in_set(dev_priv, &power_domain_set);

	return active;
}

static bool intel_crtc_get_pipe_config(struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_i915_private *i915 = to_i915(crtc->base.dev);

	if (!i915->display.get_pipe_config(crtc, crtc_state))
		return false;

	crtc_state->hw.active = true;

	intel_crtc_readout_derived_state(crtc_state);

	return true;
}

/* VESA 640x480x72Hz mode to set on the pipe */
static const struct drm_display_mode load_detect_mode = {
	DRM_MODE("640x480", DRM_MODE_TYPE_DEFAULT, 31500, 640, 664,
		 704, 832, 0, 480, 489, 491, 520, 0, DRM_MODE_FLAG_NHSYNC | DRM_MODE_FLAG_NVSYNC),
};

struct drm_framebuffer *
intel_framebuffer_create(struct drm_i915_gem_object *obj,
			 struct drm_mode_fb_cmd2 *mode_cmd)
{
	struct intel_framebuffer *intel_fb;
	int ret;

	intel_fb = kzalloc(sizeof(*intel_fb), GFP_KERNEL);
	if (!intel_fb)
		return ERR_PTR(-ENOMEM);

	ret = intel_framebuffer_init(intel_fb, obj, mode_cmd);
	if (ret)
		goto err;

	return &intel_fb->base;

err:
	kfree(intel_fb);
	return ERR_PTR(ret);
}

static int intel_modeset_disable_planes(struct drm_atomic_state *state,
					struct drm_crtc *crtc)
{
	struct drm_plane *plane;
	struct drm_plane_state *plane_state;
	int ret, i;

	ret = drm_atomic_add_affected_planes(state, crtc);
	if (ret)
		return ret;

	for_each_new_plane_in_state(state, plane, plane_state, i) {
		if (plane_state->crtc != crtc)
			continue;

		ret = drm_atomic_set_crtc_for_plane(plane_state, NULL);
		if (ret)
			return ret;

		drm_atomic_set_fb_for_plane(plane_state, NULL);
	}

	return 0;
}

int intel_get_load_detect_pipe(struct drm_connector *connector,
			       struct intel_load_detect_pipe *old,
			       struct drm_modeset_acquire_ctx *ctx)
{
	struct intel_crtc *intel_crtc;
	struct intel_encoder *intel_encoder =
		intel_attached_encoder(to_intel_connector(connector));
	struct drm_crtc *possible_crtc;
	struct drm_encoder *encoder = &intel_encoder->base;
	struct drm_crtc *crtc = NULL;
	struct drm_device *dev = encoder->dev;
	struct drm_i915_private *dev_priv = to_i915(dev);
	struct drm_mode_config *config = &dev->mode_config;
	struct drm_atomic_state *state = NULL, *restore_state = NULL;
	struct drm_connector_state *connector_state;
	struct intel_crtc_state *crtc_state;
	int ret, i = -1;

	drm_dbg_kms(&dev_priv->drm, "[CONNECTOR:%d:%s], [ENCODER:%d:%s]\n",
		    connector->base.id, connector->name,
		    encoder->base.id, encoder->name);

	old->restore_state = NULL;

	drm_WARN_ON(dev, !drm_modeset_is_locked(&config->connection_mutex));

	/*
	 * Algorithm gets a little messy:
	 *
	 *   - if the connector already has an assigned crtc, use it (but make
	 *     sure it's on first)
	 *
	 *   - try to find the first unused crtc that can drive this connector,
	 *     and use that if we find one
	 */

	/* See if we already have a CRTC for this connector */
	if (connector->state->crtc) {
		crtc = connector->state->crtc;

		ret = drm_modeset_lock(&crtc->mutex, ctx);
		if (ret)
			goto fail;

		/* Make sure the crtc and connector are running */
		goto found;
	}

	/* Find an unused one (if possible) */
	for_each_crtc(dev, possible_crtc) {
		i++;
		if (!(encoder->possible_crtcs & (1 << i)))
			continue;

		ret = drm_modeset_lock(&possible_crtc->mutex, ctx);
		if (ret)
			goto fail;

		if (possible_crtc->state->enable) {
			drm_modeset_unlock(&possible_crtc->mutex);
			continue;
		}

		crtc = possible_crtc;
		break;
	}

	/*
	 * If we didn't find an unused CRTC, don't use any.
	 */
	if (!crtc) {
		drm_dbg_kms(&dev_priv->drm,
			    "no pipe available for load-detect\n");
		ret = -ENODEV;
		goto fail;
	}

found:
	intel_crtc = to_intel_crtc(crtc);

	state = drm_atomic_state_alloc(dev);
	restore_state = drm_atomic_state_alloc(dev);
	if (!state || !restore_state) {
		ret = -ENOMEM;
		goto fail;
	}

	state->acquire_ctx = ctx;
	restore_state->acquire_ctx = ctx;

	connector_state = drm_atomic_get_connector_state(state, connector);
	if (IS_ERR(connector_state)) {
		ret = PTR_ERR(connector_state);
		goto fail;
	}

	ret = drm_atomic_set_crtc_for_connector(connector_state, crtc);
	if (ret)
		goto fail;

	crtc_state = intel_atomic_get_crtc_state(state, intel_crtc);
	if (IS_ERR(crtc_state)) {
		ret = PTR_ERR(crtc_state);
		goto fail;
	}

	crtc_state->uapi.active = true;

	ret = drm_atomic_set_mode_for_crtc(&crtc_state->uapi,
					   &load_detect_mode);
	if (ret)
		goto fail;

	ret = intel_modeset_disable_planes(state, crtc);
	if (ret)
		goto fail;

	ret = PTR_ERR_OR_ZERO(drm_atomic_get_connector_state(restore_state, connector));
	if (!ret)
		ret = PTR_ERR_OR_ZERO(drm_atomic_get_crtc_state(restore_state, crtc));
	if (!ret)
		ret = drm_atomic_add_affected_planes(restore_state, crtc);
	if (ret) {
		drm_dbg_kms(&dev_priv->drm,
			    "Failed to create a copy of old state to restore: %i\n",
			    ret);
		goto fail;
	}

	ret = drm_atomic_commit(state);
	if (ret) {
		drm_dbg_kms(&dev_priv->drm,
			    "failed to set mode on load-detect pipe\n");
		goto fail;
	}

	old->restore_state = restore_state;
	drm_atomic_state_put(state);

	/* let the connector get through one full cycle before testing */
	intel_wait_for_vblank(dev_priv, intel_crtc->pipe);
	return true;

fail:
	if (state) {
		drm_atomic_state_put(state);
		state = NULL;
	}
	if (restore_state) {
		drm_atomic_state_put(restore_state);
		restore_state = NULL;
	}

	if (ret == -EDEADLK)
		return ret;

	return false;
}

void intel_release_load_detect_pipe(struct drm_connector *connector,
				    struct intel_load_detect_pipe *old,
				    struct drm_modeset_acquire_ctx *ctx)
{
	struct intel_encoder *intel_encoder =
		intel_attached_encoder(to_intel_connector(connector));
	struct drm_i915_private *i915 = to_i915(intel_encoder->base.dev);
	struct drm_encoder *encoder = &intel_encoder->base;
	struct drm_atomic_state *state = old->restore_state;
	int ret;

	drm_dbg_kms(&i915->drm, "[CONNECTOR:%d:%s], [ENCODER:%d:%s]\n",
		    connector->base.id, connector->name,
		    encoder->base.id, encoder->name);

	if (!state)
		return;

	ret = drm_atomic_helper_commit_duplicated_state(state, ctx);
	if (ret)
		drm_dbg_kms(&i915->drm,
			    "Couldn't release load detect pipe: %i\n", ret);
	drm_atomic_state_put(state);
}

static int i9xx_pll_refclk(struct drm_device *dev,
			   const struct intel_crtc_state *pipe_config)
{
	struct drm_i915_private *dev_priv = to_i915(dev);
	u32 dpll = pipe_config->dpll_hw_state.dpll;

	if ((dpll & PLL_REF_INPUT_MASK) == PLLB_REF_INPUT_SPREADSPECTRUMIN)
		return dev_priv->vbt.lvds_ssc_freq;
	else if (HAS_PCH_SPLIT(dev_priv))
		return 120000;
	else if (DISPLAY_VER(dev_priv) != 2)
		return 96000;
	else
		return 48000;
}

/* Returns the clock of the currently programmed mode of the given pipe. */
static void i9xx_crtc_clock_get(struct intel_crtc *crtc,
				struct intel_crtc_state *pipe_config)
{
	struct drm_device *dev = crtc->base.dev;
	struct drm_i915_private *dev_priv = to_i915(dev);
	enum pipe pipe = crtc->pipe;
	u32 dpll = pipe_config->dpll_hw_state.dpll;
	u32 fp;
	struct dpll clock;
	int port_clock;
	int refclk = i9xx_pll_refclk(dev, pipe_config);

	if ((dpll & DISPLAY_RATE_SELECT_FPA1) == 0)
		fp = pipe_config->dpll_hw_state.fp0;
	else
		fp = pipe_config->dpll_hw_state.fp1;

	clock.m1 = (fp & FP_M1_DIV_MASK) >> FP_M1_DIV_SHIFT;
	if (IS_PINEVIEW(dev_priv)) {
		clock.n = ffs((fp & FP_N_PINEVIEW_DIV_MASK) >> FP_N_DIV_SHIFT) - 1;
		clock.m2 = (fp & FP_M2_PINEVIEW_DIV_MASK) >> FP_M2_DIV_SHIFT;
	} else {
		clock.n = (fp & FP_N_DIV_MASK) >> FP_N_DIV_SHIFT;
		clock.m2 = (fp & FP_M2_DIV_MASK) >> FP_M2_DIV_SHIFT;
	}

	if (DISPLAY_VER(dev_priv) != 2) {
		if (IS_PINEVIEW(dev_priv))
			clock.p1 = ffs((dpll & DPLL_FPA01_P1_POST_DIV_MASK_PINEVIEW) >>
				DPLL_FPA01_P1_POST_DIV_SHIFT_PINEVIEW);
		else
			clock.p1 = ffs((dpll & DPLL_FPA01_P1_POST_DIV_MASK) >>
			       DPLL_FPA01_P1_POST_DIV_SHIFT);

		switch (dpll & DPLL_MODE_MASK) {
		case DPLLB_MODE_DAC_SERIAL:
			clock.p2 = dpll & DPLL_DAC_SERIAL_P2_CLOCK_DIV_5 ?
				5 : 10;
			break;
		case DPLLB_MODE_LVDS:
			clock.p2 = dpll & DPLLB_LVDS_P2_CLOCK_DIV_7 ?
				7 : 14;
			break;
		default:
			drm_dbg_kms(&dev_priv->drm,
				    "Unknown DPLL mode %08x in programmed "
				    "mode\n", (int)(dpll & DPLL_MODE_MASK));
			return;
		}

		if (IS_PINEVIEW(dev_priv))
			port_clock = pnv_calc_dpll_params(refclk, &clock);
		else
			port_clock = i9xx_calc_dpll_params(refclk, &clock);
	} else {
		u32 lvds = IS_I830(dev_priv) ? 0 : intel_de_read(dev_priv,
								 LVDS);
		bool is_lvds = (pipe == 1) && (lvds & LVDS_PORT_EN);

		if (is_lvds) {
			clock.p1 = ffs((dpll & DPLL_FPA01_P1_POST_DIV_MASK_I830_LVDS) >>
				       DPLL_FPA01_P1_POST_DIV_SHIFT);

			if (lvds & LVDS_CLKB_POWER_UP)
				clock.p2 = 7;
			else
				clock.p2 = 14;
		} else {
			if (dpll & PLL_P1_DIVIDE_BY_TWO)
				clock.p1 = 2;
			else {
				clock.p1 = ((dpll & DPLL_FPA01_P1_POST_DIV_MASK_I830) >>
					    DPLL_FPA01_P1_POST_DIV_SHIFT) + 2;
			}
			if (dpll & PLL_P2_DIVIDE_BY_4)
				clock.p2 = 4;
			else
				clock.p2 = 2;
		}

		port_clock = i9xx_calc_dpll_params(refclk, &clock);
	}

	/*
	 * This value includes pixel_multiplier. We will use
	 * port_clock to compute adjusted_mode.crtc_clock in the
	 * encoder's get_config() function.
	 */
	pipe_config->port_clock = port_clock;
}

int intel_dotclock_calculate(int link_freq,
			     const struct intel_link_m_n *m_n)
{
	/*
	 * The calculation for the data clock is:
	 * pixel_clock = ((m/n)*(link_clock * nr_lanes))/bpp
	 * But we want to avoid losing precison if possible, so:
	 * pixel_clock = ((m * link_clock * nr_lanes)/(n*bpp))
	 *
	 * and the link clock is simpler:
	 * link_clock = (m * link_clock) / n
	 */

	if (!m_n->link_n)
		return 0;

	return div_u64(mul_u32_u32(m_n->link_m, link_freq), m_n->link_n);
}

static void ilk_pch_clock_get(struct intel_crtc *crtc,
			      struct intel_crtc_state *pipe_config)
{
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);

	/* read out port_clock from the DPLL */
	i9xx_crtc_clock_get(crtc, pipe_config);

	/*
	 * In case there is an active pipe without active ports,
	 * we may need some idea for the dotclock anyway.
	 * Calculate one based on the FDI configuration.
	 */
	pipe_config->hw.adjusted_mode.crtc_clock =
		intel_dotclock_calculate(intel_fdi_link_freq(dev_priv, pipe_config),
					 &pipe_config->fdi_m_n);
}

/* Returns the currently programmed mode of the given encoder. */
struct drm_display_mode *
intel_encoder_current_mode(struct intel_encoder *encoder)
{
	struct drm_i915_private *dev_priv = to_i915(encoder->base.dev);
	struct intel_crtc_state *crtc_state;
	struct drm_display_mode *mode;
	struct intel_crtc *crtc;
	enum pipe pipe;

	if (!encoder->get_hw_state(encoder, &pipe))
		return NULL;

	crtc = intel_get_crtc_for_pipe(dev_priv, pipe);

	mode = kzalloc(sizeof(*mode), GFP_KERNEL);
	if (!mode)
		return NULL;

	crtc_state = intel_crtc_state_alloc(crtc);
	if (!crtc_state) {
		kfree(mode);
		return NULL;
	}

	if (!intel_crtc_get_pipe_config(crtc_state)) {
		kfree(crtc_state);
		kfree(mode);
		return NULL;
	}

	intel_encoder_get_config(encoder, crtc_state);

	intel_mode_from_crtc_timings(mode, &crtc_state->hw.adjusted_mode);

	kfree(crtc_state);

	return mode;
}

/**
 * intel_wm_need_update - Check whether watermarks need updating
 * @cur: current plane state
 * @new: new plane state
 *
 * Check current plane state versus the new one to determine whether
 * watermarks need to be recalculated.
 *
 * Returns true or false.
 */
static bool intel_wm_need_update(const struct intel_plane_state *cur,
				 struct intel_plane_state *new)
{
	/* Update watermarks on tiling or size changes. */
	if (new->uapi.visible != cur->uapi.visible)
		return true;

	if (!cur->hw.fb || !new->hw.fb)
		return false;

	if (cur->hw.fb->modifier != new->hw.fb->modifier ||
	    cur->hw.rotation != new->hw.rotation ||
	    drm_rect_width(&new->uapi.src) != drm_rect_width(&cur->uapi.src) ||
	    drm_rect_height(&new->uapi.src) != drm_rect_height(&cur->uapi.src) ||
	    drm_rect_width(&new->uapi.dst) != drm_rect_width(&cur->uapi.dst) ||
	    drm_rect_height(&new->uapi.dst) != drm_rect_height(&cur->uapi.dst))
		return true;

	return false;
}

static bool needs_scaling(const struct intel_plane_state *state)
{
	int src_w = drm_rect_width(&state->uapi.src) >> 16;
	int src_h = drm_rect_height(&state->uapi.src) >> 16;
	int dst_w = drm_rect_width(&state->uapi.dst);
	int dst_h = drm_rect_height(&state->uapi.dst);

	return (src_w != dst_w || src_h != dst_h);
}

int intel_plane_atomic_calc_changes(const struct intel_crtc_state *old_crtc_state,
				    struct intel_crtc_state *crtc_state,
				    const struct intel_plane_state *old_plane_state,
				    struct intel_plane_state *plane_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct intel_plane *plane = to_intel_plane(plane_state->uapi.plane);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	bool mode_changed = intel_crtc_needs_modeset(crtc_state);
	bool was_crtc_enabled = old_crtc_state->hw.active;
	bool is_crtc_enabled = crtc_state->hw.active;
	bool turn_off, turn_on, visible, was_visible;
	int ret;

	if (DISPLAY_VER(dev_priv) >= 9 && plane->id != PLANE_CURSOR) {
		ret = skl_update_scaler_plane(crtc_state, plane_state);
		if (ret)
			return ret;
	}

	was_visible = old_plane_state->uapi.visible;
	visible = plane_state->uapi.visible;

	if (!was_crtc_enabled && drm_WARN_ON(&dev_priv->drm, was_visible))
		was_visible = false;

	/*
	 * Visibility is calculated as if the crtc was on, but
	 * after scaler setup everything depends on it being off
	 * when the crtc isn't active.
	 *
	 * FIXME this is wrong for watermarks. Watermarks should also
	 * be computed as if the pipe would be active. Perhaps move
	 * per-plane wm computation to the .check_plane() hook, and
	 * only combine the results from all planes in the current place?
	 */
	if (!is_crtc_enabled) {
		intel_plane_set_invisible(crtc_state, plane_state);
		visible = false;
	}

	if (!was_visible && !visible)
		return 0;

	turn_off = was_visible && (!visible || mode_changed);
	turn_on = visible && (!was_visible || mode_changed);

	drm_dbg_atomic(&dev_priv->drm,
		       "[CRTC:%d:%s] with [PLANE:%d:%s] visible %i -> %i, off %i, on %i, ms %i\n",
		       crtc->base.base.id, crtc->base.name,
		       plane->base.base.id, plane->base.name,
		       was_visible, visible,
		       turn_off, turn_on, mode_changed);

	if (turn_on) {
		if (DISPLAY_VER(dev_priv) < 5 && !IS_G4X(dev_priv))
			crtc_state->update_wm_pre = true;

		/* must disable cxsr around plane enable/disable */
		if (plane->id != PLANE_CURSOR)
			crtc_state->disable_cxsr = true;
	} else if (turn_off) {
		if (DISPLAY_VER(dev_priv) < 5 && !IS_G4X(dev_priv))
			crtc_state->update_wm_post = true;

		/* must disable cxsr around plane enable/disable */
		if (plane->id != PLANE_CURSOR)
			crtc_state->disable_cxsr = true;
	} else if (intel_wm_need_update(old_plane_state, plane_state)) {
		if (DISPLAY_VER(dev_priv) < 5 && !IS_G4X(dev_priv)) {
			/* FIXME bollocks */
			crtc_state->update_wm_pre = true;
			crtc_state->update_wm_post = true;
		}
	}

	if (visible || was_visible)
		crtc_state->fb_bits |= plane->frontbuffer_bit;

	/*
	 * ILK/SNB DVSACNTR/Sprite Enable
	 * IVB SPR_CTL/Sprite Enable
	 * "When in Self Refresh Big FIFO mode, a write to enable the
	 *  plane will be internally buffered and delayed while Big FIFO
	 *  mode is exiting."
	 *
	 * Which means that enabling the sprite can take an extra frame
	 * when we start in big FIFO mode (LP1+). Thus we need to drop
	 * down to LP0 and wait for vblank in order to make sure the
	 * sprite gets enabled on the next vblank after the register write.
	 * Doing otherwise would risk enabling the sprite one frame after
	 * we've already signalled flip completion. We can resume LP1+
	 * once the sprite has been enabled.
	 *
	 *
	 * WaCxSRDisabledForSpriteScaling:ivb
	 * IVB SPR_SCALE/Scaling Enable
	 * "Low Power watermarks must be disabled for at least one
	 *  frame before enabling sprite scaling, and kept disabled
	 *  until sprite scaling is disabled."
	 *
	 * ILK/SNB DVSASCALE/Scaling Enable
	 * "When in Self Refresh Big FIFO mode, scaling enable will be
	 *  masked off while Big FIFO mode is exiting."
	 *
	 * Despite the w/a only being listed for IVB we assume that
	 * the ILK/SNB note has similar ramifications, hence we apply
	 * the w/a on all three platforms.
	 *
	 * With experimental results seems this is needed also for primary
	 * plane, not only sprite plane.
	 */
	if (plane->id != PLANE_CURSOR &&
	    (IS_IRONLAKE(dev_priv) || IS_SANDYBRIDGE(dev_priv) ||
	     IS_IVYBRIDGE(dev_priv)) &&
	    (turn_on || (!needs_scaling(old_plane_state) &&
			 needs_scaling(plane_state))))
		crtc_state->disable_lp_wm = true;

	return 0;
}

static bool encoders_cloneable(const struct intel_encoder *a,
			       const struct intel_encoder *b)
{
	/* masks could be asymmetric, so check both ways */
	return a == b || (a->cloneable & (1 << b->type) &&
			  b->cloneable & (1 << a->type));
}

static bool check_single_encoder_cloning(struct intel_atomic_state *state,
					 struct intel_crtc *crtc,
					 struct intel_encoder *encoder)
{
	struct intel_encoder *source_encoder;
	struct drm_connector *connector;
	struct drm_connector_state *connector_state;
	int i;

	for_each_new_connector_in_state(&state->base, connector, connector_state, i) {
		if (connector_state->crtc != &crtc->base)
			continue;

		source_encoder =
			to_intel_encoder(connector_state->best_encoder);
		if (!encoders_cloneable(encoder, source_encoder))
			return false;
	}

	return true;
}

static int icl_add_linked_planes(struct intel_atomic_state *state)
{
	struct intel_plane *plane, *linked;
	struct intel_plane_state *plane_state, *linked_plane_state;
	int i;

	for_each_new_intel_plane_in_state(state, plane, plane_state, i) {
		linked = plane_state->planar_linked_plane;

		if (!linked)
			continue;

		linked_plane_state = intel_atomic_get_plane_state(state, linked);
		if (IS_ERR(linked_plane_state))
			return PTR_ERR(linked_plane_state);

		drm_WARN_ON(state->base.dev,
			    linked_plane_state->planar_linked_plane != plane);
		drm_WARN_ON(state->base.dev,
			    linked_plane_state->planar_slave == plane_state->planar_slave);
	}

	return 0;
}

static int icl_check_nv12_planes(struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	struct intel_atomic_state *state = to_intel_atomic_state(crtc_state->uapi.state);
	struct intel_plane *plane, *linked;
	struct intel_plane_state *plane_state;
	int i;

	if (DISPLAY_VER(dev_priv) < 11)
		return 0;

	/*
	 * Destroy all old plane links and make the slave plane invisible
	 * in the crtc_state->active_planes mask.
	 */
	for_each_new_intel_plane_in_state(state, plane, plane_state, i) {
		if (plane->pipe != crtc->pipe || !plane_state->planar_linked_plane)
			continue;

		plane_state->planar_linked_plane = NULL;
		if (plane_state->planar_slave && !plane_state->uapi.visible) {
			crtc_state->enabled_planes &= ~BIT(plane->id);
			crtc_state->active_planes &= ~BIT(plane->id);
			crtc_state->update_planes |= BIT(plane->id);
		}

		plane_state->planar_slave = false;
	}

	if (!crtc_state->nv12_planes)
		return 0;

	for_each_new_intel_plane_in_state(state, plane, plane_state, i) {
		struct intel_plane_state *linked_state = NULL;

		if (plane->pipe != crtc->pipe ||
		    !(crtc_state->nv12_planes & BIT(plane->id)))
			continue;

		for_each_intel_plane_on_crtc(&dev_priv->drm, crtc, linked) {
			if (!icl_is_nv12_y_plane(dev_priv, linked->id))
				continue;

			if (crtc_state->active_planes & BIT(linked->id))
				continue;

			linked_state = intel_atomic_get_plane_state(state, linked);
			if (IS_ERR(linked_state))
				return PTR_ERR(linked_state);

			break;
		}

		if (!linked_state) {
			drm_dbg_kms(&dev_priv->drm,
				    "Need %d free Y planes for planar YUV\n",
				    hweight8(crtc_state->nv12_planes));

			return -EINVAL;
		}

		plane_state->planar_linked_plane = linked;

		linked_state->planar_slave = true;
		linked_state->planar_linked_plane = plane;
		crtc_state->enabled_planes |= BIT(linked->id);
		crtc_state->active_planes |= BIT(linked->id);
		crtc_state->update_planes |= BIT(linked->id);
		drm_dbg_kms(&dev_priv->drm, "Using %s as Y plane for %s\n",
			    linked->base.name, plane->base.name);

		/* Copy parameters to slave plane */
		linked_state->ctl = plane_state->ctl | PLANE_CTL_YUV420_Y_PLANE;
		linked_state->color_ctl = plane_state->color_ctl;
		linked_state->view = plane_state->view;

		intel_plane_copy_hw_state(linked_state, plane_state);
		linked_state->uapi.src = plane_state->uapi.src;
		linked_state->uapi.dst = plane_state->uapi.dst;

		if (icl_is_hdr_plane(dev_priv, plane->id)) {
			if (linked->id == PLANE_SPRITE5)
				plane_state->cus_ctl |= PLANE_CUS_PLANE_7;
			else if (linked->id == PLANE_SPRITE4)
				plane_state->cus_ctl |= PLANE_CUS_PLANE_6;
			else if (linked->id == PLANE_SPRITE3)
				plane_state->cus_ctl |= PLANE_CUS_PLANE_5_RKL;
			else if (linked->id == PLANE_SPRITE2)
				plane_state->cus_ctl |= PLANE_CUS_PLANE_4_RKL;
			else
				MISSING_CASE(linked->id);
		}
	}

	return 0;
}

static bool c8_planes_changed(const struct intel_crtc_state *new_crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(new_crtc_state->uapi.crtc);
	struct intel_atomic_state *state =
		to_intel_atomic_state(new_crtc_state->uapi.state);
	const struct intel_crtc_state *old_crtc_state =
		intel_atomic_get_old_crtc_state(state, crtc);

	return !old_crtc_state->c8_planes != !new_crtc_state->c8_planes;
}

static u16 hsw_linetime_wm(const struct intel_crtc_state *crtc_state)
{
	const struct drm_display_mode *pipe_mode =
		&crtc_state->hw.pipe_mode;
	int linetime_wm;

	if (!crtc_state->hw.enable)
		return 0;

	linetime_wm = DIV_ROUND_CLOSEST(pipe_mode->crtc_htotal * 1000 * 8,
					pipe_mode->crtc_clock);

	return min(linetime_wm, 0x1ff);
}

static u16 hsw_ips_linetime_wm(const struct intel_crtc_state *crtc_state,
			       const struct intel_cdclk_state *cdclk_state)
{
	const struct drm_display_mode *pipe_mode =
		&crtc_state->hw.pipe_mode;
	int linetime_wm;

	if (!crtc_state->hw.enable)
		return 0;

	linetime_wm = DIV_ROUND_CLOSEST(pipe_mode->crtc_htotal * 1000 * 8,
					cdclk_state->logical.cdclk);

	return min(linetime_wm, 0x1ff);
}

static u16 skl_linetime_wm(const struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	const struct drm_display_mode *pipe_mode =
		&crtc_state->hw.pipe_mode;
	int linetime_wm;

	if (!crtc_state->hw.enable)
		return 0;

	linetime_wm = DIV_ROUND_UP(pipe_mode->crtc_htotal * 1000 * 8,
				   crtc_state->pixel_rate);

	/* Display WA #1135: BXT:ALL GLK:ALL */
	if ((IS_GEMINILAKE(dev_priv) || IS_BROXTON(dev_priv)) &&
	    dev_priv->ipc_enabled)
		linetime_wm /= 2;

	return min(linetime_wm, 0x1ff);
}

static int hsw_compute_linetime_wm(struct intel_atomic_state *state,
				   struct intel_crtc *crtc)
{
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	struct intel_crtc_state *crtc_state =
		intel_atomic_get_new_crtc_state(state, crtc);
	const struct intel_cdclk_state *cdclk_state;

	if (DISPLAY_VER(dev_priv) >= 9)
		crtc_state->linetime = skl_linetime_wm(crtc_state);
	else
		crtc_state->linetime = hsw_linetime_wm(crtc_state);

	if (!hsw_crtc_supports_ips(crtc))
		return 0;

	cdclk_state = intel_atomic_get_cdclk_state(state);
	if (IS_ERR(cdclk_state))
		return PTR_ERR(cdclk_state);

	crtc_state->ips_linetime = hsw_ips_linetime_wm(crtc_state,
						       cdclk_state);

	return 0;
}

static int intel_crtc_atomic_check(struct intel_atomic_state *state,
				   struct intel_crtc *crtc)
{
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	struct intel_crtc_state *crtc_state =
		intel_atomic_get_new_crtc_state(state, crtc);
	bool mode_changed = intel_crtc_needs_modeset(crtc_state);
	int ret;

	if (DISPLAY_VER(dev_priv) < 5 && !IS_G4X(dev_priv) &&
	    mode_changed && !crtc_state->hw.active)
		crtc_state->update_wm_post = true;

	if (mode_changed && crtc_state->hw.enable &&
	    dev_priv->display.crtc_compute_clock &&
	    !crtc_state->bigjoiner_slave &&
	    !drm_WARN_ON(&dev_priv->drm, crtc_state->shared_dpll)) {
		ret = dev_priv->display.crtc_compute_clock(crtc, crtc_state);
		if (ret)
			return ret;
	}

	/*
	 * May need to update pipe gamma enable bits
	 * when C8 planes are getting enabled/disabled.
	 */
	if (c8_planes_changed(crtc_state))
		crtc_state->uapi.color_mgmt_changed = true;

	if (mode_changed || crtc_state->update_pipe ||
	    crtc_state->uapi.color_mgmt_changed) {
		ret = intel_color_check(crtc_state);
		if (ret)
			return ret;
	}

	if (dev_priv->display.compute_pipe_wm) {
		ret = dev_priv->display.compute_pipe_wm(crtc_state);
		if (ret) {
			drm_dbg_kms(&dev_priv->drm,
				    "Target pipe watermarks are invalid\n");
			return ret;
		}
	}

	if (dev_priv->display.compute_intermediate_wm) {
		if (drm_WARN_ON(&dev_priv->drm,
				!dev_priv->display.compute_pipe_wm))
			return 0;

		/*
		 * Calculate 'intermediate' watermarks that satisfy both the
		 * old state and the new state.  We can program these
		 * immediately.
		 */
		ret = dev_priv->display.compute_intermediate_wm(crtc_state);
		if (ret) {
			drm_dbg_kms(&dev_priv->drm,
				    "No valid intermediate pipe watermarks are possible\n");
			return ret;
		}
	}

	if (DISPLAY_VER(dev_priv) >= 9) {
		if (mode_changed || crtc_state->update_pipe) {
			ret = skl_update_scaler_crtc(crtc_state);
			if (ret)
				return ret;
		}

		ret = intel_atomic_setup_scalers(dev_priv, crtc, crtc_state);
		if (ret)
			return ret;
	}

	if (HAS_IPS(dev_priv)) {
		ret = hsw_compute_ips_config(crtc_state);
		if (ret)
			return ret;
	}

	if (DISPLAY_VER(dev_priv) >= 9 ||
	    IS_BROADWELL(dev_priv) || IS_HASWELL(dev_priv)) {
		ret = hsw_compute_linetime_wm(state, crtc);
		if (ret)
			return ret;

	}

	if (!mode_changed) {
		ret = intel_psr2_sel_fetch_update(state, crtc);
		if (ret)
			return ret;
	}

	return 0;
}

static void intel_modeset_update_connector_atomic_state(struct drm_device *dev)
{
	struct intel_connector *connector;
	struct drm_connector_list_iter conn_iter;

	drm_connector_list_iter_begin(dev, &conn_iter);
	for_each_intel_connector_iter(connector, &conn_iter) {
		struct drm_connector_state *conn_state = connector->base.state;
		struct intel_encoder *encoder =
			to_intel_encoder(connector->base.encoder);

		if (conn_state->crtc)
			drm_connector_put(&connector->base);

		if (encoder) {
			struct intel_crtc *crtc =
				to_intel_crtc(encoder->base.crtc);
			const struct intel_crtc_state *crtc_state =
				to_intel_crtc_state(crtc->base.state);

			conn_state->best_encoder = &encoder->base;
			conn_state->crtc = &crtc->base;
			conn_state->max_bpc = (crtc_state->pipe_bpp ?: 24) / 3;

			drm_connector_get(&connector->base);
		} else {
			conn_state->best_encoder = NULL;
			conn_state->crtc = NULL;
		}
	}
	drm_connector_list_iter_end(&conn_iter);
}

static int
compute_sink_pipe_bpp(const struct drm_connector_state *conn_state,
		      struct intel_crtc_state *pipe_config)
{
	struct drm_connector *connector = conn_state->connector;
	struct drm_i915_private *i915 = to_i915(pipe_config->uapi.crtc->dev);
	const struct drm_display_info *info = &connector->display_info;
	int bpp;

	switch (conn_state->max_bpc) {
	case 6 ... 7:
		bpp = 6 * 3;
		break;
	case 8 ... 9:
		bpp = 8 * 3;
		break;
	case 10 ... 11:
		bpp = 10 * 3;
		break;
	case 12 ... 16:
		bpp = 12 * 3;
		break;
	default:
		MISSING_CASE(conn_state->max_bpc);
		return -EINVAL;
	}

	if (bpp < pipe_config->pipe_bpp) {
		drm_dbg_kms(&i915->drm,
			    "[CONNECTOR:%d:%s] Limiting display bpp to %d instead of "
			    "EDID bpp %d, requested bpp %d, max platform bpp %d\n",
			    connector->base.id, connector->name,
			    bpp, 3 * info->bpc,
			    3 * conn_state->max_requested_bpc,
			    pipe_config->pipe_bpp);

		pipe_config->pipe_bpp = bpp;
	}

	return 0;
}

static int
compute_baseline_pipe_bpp(struct intel_crtc *crtc,
			  struct intel_crtc_state *pipe_config)
{
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	struct drm_atomic_state *state = pipe_config->uapi.state;
	struct drm_connector *connector;
	struct drm_connector_state *connector_state;
	int bpp, i;

	if ((IS_G4X(dev_priv) || IS_VALLEYVIEW(dev_priv) ||
	    IS_CHERRYVIEW(dev_priv)))
		bpp = 10*3;
	else if (DISPLAY_VER(dev_priv) >= 5)
		bpp = 12*3;
	else
		bpp = 8*3;

	pipe_config->pipe_bpp = bpp;

	/* Clamp display bpp to connector max bpp */
	for_each_new_connector_in_state(state, connector, connector_state, i) {
		int ret;

		if (connector_state->crtc != &crtc->base)
			continue;

		ret = compute_sink_pipe_bpp(connector_state, pipe_config);
		if (ret)
			return ret;
	}

	return 0;
}

static void intel_dump_crtc_timings(struct drm_i915_private *i915,
				    const struct drm_display_mode *mode)
{
	drm_dbg_kms(&i915->drm, "crtc timings: %d %d %d %d %d %d %d %d %d, "
		    "type: 0x%x flags: 0x%x\n",
		    mode->crtc_clock,
		    mode->crtc_hdisplay, mode->crtc_hsync_start,
		    mode->crtc_hsync_end, mode->crtc_htotal,
		    mode->crtc_vdisplay, mode->crtc_vsync_start,
		    mode->crtc_vsync_end, mode->crtc_vtotal,
		    mode->type, mode->flags);
}

static void
intel_dump_m_n_config(const struct intel_crtc_state *pipe_config,
		      const char *id, unsigned int lane_count,
		      const struct intel_link_m_n *m_n)
{
	struct drm_i915_private *i915 = to_i915(pipe_config->uapi.crtc->dev);

	drm_dbg_kms(&i915->drm,
		    "%s: lanes: %i; gmch_m: %u, gmch_n: %u, link_m: %u, link_n: %u, tu: %u\n",
		    id, lane_count,
		    m_n->gmch_m, m_n->gmch_n,
		    m_n->link_m, m_n->link_n, m_n->tu);
}

static void
intel_dump_infoframe(struct drm_i915_private *dev_priv,
		     const union hdmi_infoframe *frame)
{
	if (!drm_debug_enabled(DRM_UT_KMS))
		return;

	hdmi_infoframe_log(KERN_DEBUG, dev_priv->drm.dev, frame);
}

static void
intel_dump_dp_vsc_sdp(struct drm_i915_private *dev_priv,
		      const struct drm_dp_vsc_sdp *vsc)
{
	if (!drm_debug_enabled(DRM_UT_KMS))
		return;

	drm_dp_vsc_sdp_log(KERN_DEBUG, dev_priv->drm.dev, vsc);
}

#define OUTPUT_TYPE(x) [INTEL_OUTPUT_ ## x] = #x

static const char * const output_type_str[] = {
	OUTPUT_TYPE(UNUSED),
	OUTPUT_TYPE(ANALOG),
	OUTPUT_TYPE(DVO),
	OUTPUT_TYPE(SDVO),
	OUTPUT_TYPE(LVDS),
	OUTPUT_TYPE(TVOUT),
	OUTPUT_TYPE(HDMI),
	OUTPUT_TYPE(DP),
	OUTPUT_TYPE(EDP),
	OUTPUT_TYPE(DSI),
	OUTPUT_TYPE(DDI),
	OUTPUT_TYPE(DP_MST),
};

#undef OUTPUT_TYPE

static void snprintf_output_types(char *buf, size_t len,
				  unsigned int output_types)
{
	char *str = buf;
	int i;

	str[0] = '\0';

	for (i = 0; i < ARRAY_SIZE(output_type_str); i++) {
		int r;

		if ((output_types & BIT(i)) == 0)
			continue;

		r = snprintf(str, len, "%s%s",
			     str != buf ? "," : "", output_type_str[i]);
		if (r >= len)
			break;
		str += r;
		len -= r;

		output_types &= ~BIT(i);
	}

	WARN_ON_ONCE(output_types != 0);
}

static const char * const output_format_str[] = {
	[INTEL_OUTPUT_FORMAT_RGB] = "RGB",
	[INTEL_OUTPUT_FORMAT_YCBCR420] = "YCBCR4:2:0",
	[INTEL_OUTPUT_FORMAT_YCBCR444] = "YCBCR4:4:4",
};

static const char *output_formats(enum intel_output_format format)
{
	if (format >= ARRAY_SIZE(output_format_str))
		return "invalid";
	return output_format_str[format];
}

static void intel_dump_plane_state(const struct intel_plane_state *plane_state)
{
	struct intel_plane *plane = to_intel_plane(plane_state->uapi.plane);
	struct drm_i915_private *i915 = to_i915(plane->base.dev);
	const struct drm_framebuffer *fb = plane_state->hw.fb;

	if (!fb) {
		drm_dbg_kms(&i915->drm,
			    "[PLANE:%d:%s] fb: [NOFB], visible: %s\n",
			    plane->base.base.id, plane->base.name,
			    yesno(plane_state->uapi.visible));
		return;
	}

	drm_dbg_kms(&i915->drm,
		    "[PLANE:%d:%s] fb: [FB:%d] %ux%u format = %p4cc modifier = 0x%llx, visible: %s\n",
		    plane->base.base.id, plane->base.name,
		    fb->base.id, fb->width, fb->height, &fb->format->format,
		    fb->modifier, yesno(plane_state->uapi.visible));
	drm_dbg_kms(&i915->drm, "\trotation: 0x%x, scaler: %d\n",
		    plane_state->hw.rotation, plane_state->scaler_id);
	if (plane_state->uapi.visible)
		drm_dbg_kms(&i915->drm,
			    "\tsrc: " DRM_RECT_FP_FMT " dst: " DRM_RECT_FMT "\n",
			    DRM_RECT_FP_ARG(&plane_state->uapi.src),
			    DRM_RECT_ARG(&plane_state->uapi.dst));
}

static void intel_dump_pipe_config(const struct intel_crtc_state *pipe_config,
				   struct intel_atomic_state *state,
				   const char *context)
{
	struct intel_crtc *crtc = to_intel_crtc(pipe_config->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	const struct intel_plane_state *plane_state;
	struct intel_plane *plane;
	char buf[64];
	int i;

	drm_dbg_kms(&dev_priv->drm, "[CRTC:%d:%s] enable: %s %s\n",
		    crtc->base.base.id, crtc->base.name,
		    yesno(pipe_config->hw.enable), context);

	if (!pipe_config->hw.enable)
		goto dump_planes;

	snprintf_output_types(buf, sizeof(buf), pipe_config->output_types);
	drm_dbg_kms(&dev_priv->drm,
		    "active: %s, output_types: %s (0x%x), output format: %s\n",
		    yesno(pipe_config->hw.active),
		    buf, pipe_config->output_types,
		    output_formats(pipe_config->output_format));

	drm_dbg_kms(&dev_priv->drm,
		    "cpu_transcoder: %s, pipe bpp: %i, dithering: %i\n",
		    transcoder_name(pipe_config->cpu_transcoder),
		    pipe_config->pipe_bpp, pipe_config->dither);

	drm_dbg_kms(&dev_priv->drm, "MST master transcoder: %s\n",
		    transcoder_name(pipe_config->mst_master_transcoder));

	drm_dbg_kms(&dev_priv->drm,
		    "port sync: master transcoder: %s, slave transcoder bitmask = 0x%x\n",
		    transcoder_name(pipe_config->master_transcoder),
		    pipe_config->sync_mode_slaves_mask);

	drm_dbg_kms(&dev_priv->drm, "bigjoiner: %s\n",
		    pipe_config->bigjoiner_slave ? "slave" :
		    pipe_config->bigjoiner ? "master" : "no");

	drm_dbg_kms(&dev_priv->drm, "splitter: %s, link count %d, overlap %d\n",
		    enableddisabled(pipe_config->splitter.enable),
		    pipe_config->splitter.link_count,
		    pipe_config->splitter.pixel_overlap);

	if (pipe_config->has_pch_encoder)
		intel_dump_m_n_config(pipe_config, "fdi",
				      pipe_config->fdi_lanes,
				      &pipe_config->fdi_m_n);

	if (intel_crtc_has_dp_encoder(pipe_config)) {
		intel_dump_m_n_config(pipe_config, "dp m_n",
				pipe_config->lane_count, &pipe_config->dp_m_n);
		if (pipe_config->has_drrs)
			intel_dump_m_n_config(pipe_config, "dp m2_n2",
					      pipe_config->lane_count,
					      &pipe_config->dp_m2_n2);
	}

	drm_dbg_kms(&dev_priv->drm,
		    "audio: %i, infoframes: %i, infoframes enabled: 0x%x\n",
		    pipe_config->has_audio, pipe_config->has_infoframe,
		    pipe_config->infoframes.enable);

	if (pipe_config->infoframes.enable &
	    intel_hdmi_infoframe_enable(HDMI_PACKET_TYPE_GENERAL_CONTROL))
		drm_dbg_kms(&dev_priv->drm, "GCP: 0x%x\n",
			    pipe_config->infoframes.gcp);
	if (pipe_config->infoframes.enable &
	    intel_hdmi_infoframe_enable(HDMI_INFOFRAME_TYPE_AVI))
		intel_dump_infoframe(dev_priv, &pipe_config->infoframes.avi);
	if (pipe_config->infoframes.enable &
	    intel_hdmi_infoframe_enable(HDMI_INFOFRAME_TYPE_SPD))
		intel_dump_infoframe(dev_priv, &pipe_config->infoframes.spd);
	if (pipe_config->infoframes.enable &
	    intel_hdmi_infoframe_enable(HDMI_INFOFRAME_TYPE_VENDOR))
		intel_dump_infoframe(dev_priv, &pipe_config->infoframes.hdmi);
	if (pipe_config->infoframes.enable &
	    intel_hdmi_infoframe_enable(HDMI_INFOFRAME_TYPE_DRM))
		intel_dump_infoframe(dev_priv, &pipe_config->infoframes.drm);
	if (pipe_config->infoframes.enable &
	    intel_hdmi_infoframe_enable(HDMI_PACKET_TYPE_GAMUT_METADATA))
		intel_dump_infoframe(dev_priv, &pipe_config->infoframes.drm);
	if (pipe_config->infoframes.enable &
	    intel_hdmi_infoframe_enable(DP_SDP_VSC))
		intel_dump_dp_vsc_sdp(dev_priv, &pipe_config->infoframes.vsc);

	drm_dbg_kms(&dev_priv->drm, "vrr: %s, vmin: %d, vmax: %d, pipeline full: %d, guardband: %d flipline: %d, vmin vblank: %d, vmax vblank: %d\n",
		    yesno(pipe_config->vrr.enable),
		    pipe_config->vrr.vmin, pipe_config->vrr.vmax,
		    pipe_config->vrr.pipeline_full, pipe_config->vrr.guardband,
		    pipe_config->vrr.flipline,
		    intel_vrr_vmin_vblank_start(pipe_config),
		    intel_vrr_vmax_vblank_start(pipe_config));

	drm_dbg_kms(&dev_priv->drm, "requested mode:\n");
	drm_mode_debug_printmodeline(&pipe_config->hw.mode);
	drm_dbg_kms(&dev_priv->drm, "adjusted mode:\n");
	drm_mode_debug_printmodeline(&pipe_config->hw.adjusted_mode);
	intel_dump_crtc_timings(dev_priv, &pipe_config->hw.adjusted_mode);
	drm_dbg_kms(&dev_priv->drm, "pipe mode:\n");
	drm_mode_debug_printmodeline(&pipe_config->hw.pipe_mode);
	intel_dump_crtc_timings(dev_priv, &pipe_config->hw.pipe_mode);
	drm_dbg_kms(&dev_priv->drm,
		    "port clock: %d, pipe src size: %dx%d, pixel rate %d\n",
		    pipe_config->port_clock,
		    pipe_config->pipe_src_w, pipe_config->pipe_src_h,
		    pipe_config->pixel_rate);

	drm_dbg_kms(&dev_priv->drm, "linetime: %d, ips linetime: %d\n",
		    pipe_config->linetime, pipe_config->ips_linetime);

	if (DISPLAY_VER(dev_priv) >= 9)
		drm_dbg_kms(&dev_priv->drm,
			    "num_scalers: %d, scaler_users: 0x%x, scaler_id: %d\n",
			    crtc->num_scalers,
			    pipe_config->scaler_state.scaler_users,
			    pipe_config->scaler_state.scaler_id);

	if (HAS_GMCH(dev_priv))
		drm_dbg_kms(&dev_priv->drm,
			    "gmch pfit: control: 0x%08x, ratios: 0x%08x, lvds border: 0x%08x\n",
			    pipe_config->gmch_pfit.control,
			    pipe_config->gmch_pfit.pgm_ratios,
			    pipe_config->gmch_pfit.lvds_border_bits);
	else
		drm_dbg_kms(&dev_priv->drm,
			    "pch pfit: " DRM_RECT_FMT ", %s, force thru: %s\n",
			    DRM_RECT_ARG(&pipe_config->pch_pfit.dst),
			    enableddisabled(pipe_config->pch_pfit.enabled),
			    yesno(pipe_config->pch_pfit.force_thru));

	drm_dbg_kms(&dev_priv->drm, "ips: %i, double wide: %i\n",
		    pipe_config->ips_enabled, pipe_config->double_wide);

	intel_dpll_dump_hw_state(dev_priv, &pipe_config->dpll_hw_state);

	if (IS_CHERRYVIEW(dev_priv))
		drm_dbg_kms(&dev_priv->drm,
			    "cgm_mode: 0x%x gamma_mode: 0x%x gamma_enable: %d csc_enable: %d\n",
			    pipe_config->cgm_mode, pipe_config->gamma_mode,
			    pipe_config->gamma_enable, pipe_config->csc_enable);
	else
		drm_dbg_kms(&dev_priv->drm,
			    "csc_mode: 0x%x gamma_mode: 0x%x gamma_enable: %d csc_enable: %d\n",
			    pipe_config->csc_mode, pipe_config->gamma_mode,
			    pipe_config->gamma_enable, pipe_config->csc_enable);

	drm_dbg_kms(&dev_priv->drm, "degamma lut: %d entries, gamma lut: %d entries\n",
		    pipe_config->hw.degamma_lut ?
		    drm_color_lut_size(pipe_config->hw.degamma_lut) : 0,
		    pipe_config->hw.gamma_lut ?
		    drm_color_lut_size(pipe_config->hw.gamma_lut) : 0);

dump_planes:
	if (!state)
		return;

	for_each_new_intel_plane_in_state(state, plane, plane_state, i) {
		if (plane->pipe == crtc->pipe)
			intel_dump_plane_state(plane_state);
	}
}

static bool check_digital_port_conflicts(struct intel_atomic_state *state)
{
	struct drm_device *dev = state->base.dev;
	struct drm_connector *connector;
	struct drm_connector_list_iter conn_iter;
	unsigned int used_ports = 0;
	unsigned int used_mst_ports = 0;
	bool ret = true;

	/*
	 * We're going to peek into connector->state,
	 * hence connection_mutex must be held.
	 */
	drm_modeset_lock_assert_held(&dev->mode_config.connection_mutex);

	/*
	 * Walk the connector list instead of the encoder
	 * list to detect the problem on ddi platforms
	 * where there's just one encoder per digital port.
	 */
	drm_connector_list_iter_begin(dev, &conn_iter);
	drm_for_each_connector_iter(connector, &conn_iter) {
		struct drm_connector_state *connector_state;
		struct intel_encoder *encoder;

		connector_state =
			drm_atomic_get_new_connector_state(&state->base,
							   connector);
		if (!connector_state)
			connector_state = connector->state;

		if (!connector_state->best_encoder)
			continue;

		encoder = to_intel_encoder(connector_state->best_encoder);

		drm_WARN_ON(dev, !connector_state->crtc);

		switch (encoder->type) {
		case INTEL_OUTPUT_DDI:
			if (drm_WARN_ON(dev, !HAS_DDI(to_i915(dev))))
				break;
			fallthrough;
		case INTEL_OUTPUT_DP:
		case INTEL_OUTPUT_HDMI:
		case INTEL_OUTPUT_EDP:
			/* the same port mustn't appear more than once */
			if (used_ports & BIT(encoder->port))
				ret = false;

			used_ports |= BIT(encoder->port);
			break;
		case INTEL_OUTPUT_DP_MST:
			used_mst_ports |=
				1 << encoder->port;
			break;
		default:
			break;
		}
	}
	drm_connector_list_iter_end(&conn_iter);

	/* can't mix MST and SST/HDMI on the same port */
	if (used_ports & used_mst_ports)
		return false;

	return ret;
}

static void
intel_crtc_copy_uapi_to_hw_state_nomodeset(struct intel_atomic_state *state,
					   struct intel_crtc_state *crtc_state)
{
	const struct intel_crtc_state *from_crtc_state = crtc_state;

	if (crtc_state->bigjoiner_slave) {
		from_crtc_state = intel_atomic_get_new_crtc_state(state,
								  crtc_state->bigjoiner_linked_crtc);

		/* No need to copy state if the master state is unchanged */
		if (!from_crtc_state)
			return;
	}

	intel_crtc_copy_color_blobs(crtc_state, from_crtc_state);
}

static void
intel_crtc_copy_uapi_to_hw_state(struct intel_atomic_state *state,
				 struct intel_crtc_state *crtc_state)
{
	crtc_state->hw.enable = crtc_state->uapi.enable;
	crtc_state->hw.active = crtc_state->uapi.active;
	crtc_state->hw.mode = crtc_state->uapi.mode;
	crtc_state->hw.adjusted_mode = crtc_state->uapi.adjusted_mode;
	crtc_state->hw.scaling_filter = crtc_state->uapi.scaling_filter;

	intel_crtc_copy_uapi_to_hw_state_nomodeset(state, crtc_state);
}

static void intel_crtc_copy_hw_to_uapi_state(struct intel_crtc_state *crtc_state)
{
	if (crtc_state->bigjoiner_slave)
		return;

	crtc_state->uapi.enable = crtc_state->hw.enable;
	crtc_state->uapi.active = crtc_state->hw.active;
	drm_WARN_ON(crtc_state->uapi.crtc->dev,
		    drm_atomic_set_mode_for_crtc(&crtc_state->uapi, &crtc_state->hw.mode) < 0);

	crtc_state->uapi.adjusted_mode = crtc_state->hw.adjusted_mode;
	crtc_state->uapi.scaling_filter = crtc_state->hw.scaling_filter;

	/* copy color blobs to uapi */
	drm_property_replace_blob(&crtc_state->uapi.degamma_lut,
				  crtc_state->hw.degamma_lut);
	drm_property_replace_blob(&crtc_state->uapi.gamma_lut,
				  crtc_state->hw.gamma_lut);
	drm_property_replace_blob(&crtc_state->uapi.ctm,
				  crtc_state->hw.ctm);
}

static int
copy_bigjoiner_crtc_state(struct intel_crtc_state *crtc_state,
			  const struct intel_crtc_state *from_crtc_state)
{
	struct intel_crtc_state *saved_state;
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);

	saved_state = kmemdup(from_crtc_state, sizeof(*saved_state), GFP_KERNEL);
	if (!saved_state)
		return -ENOMEM;

	saved_state->uapi = crtc_state->uapi;
	saved_state->scaler_state = crtc_state->scaler_state;
	saved_state->shared_dpll = crtc_state->shared_dpll;
	saved_state->dpll_hw_state = crtc_state->dpll_hw_state;
	saved_state->crc_enabled = crtc_state->crc_enabled;

	intel_crtc_free_hw_state(crtc_state);
	memcpy(crtc_state, saved_state, sizeof(*crtc_state));
	kfree(saved_state);

	/* Re-init hw state */
	memset(&crtc_state->hw, 0, sizeof(saved_state->hw));
	crtc_state->hw.enable = from_crtc_state->hw.enable;
	crtc_state->hw.active = from_crtc_state->hw.active;
	crtc_state->hw.pipe_mode = from_crtc_state->hw.pipe_mode;
	crtc_state->hw.adjusted_mode = from_crtc_state->hw.adjusted_mode;

	/* Some fixups */
	crtc_state->uapi.mode_changed = from_crtc_state->uapi.mode_changed;
	crtc_state->uapi.connectors_changed = from_crtc_state->uapi.connectors_changed;
	crtc_state->uapi.active_changed = from_crtc_state->uapi.active_changed;
	crtc_state->nv12_planes = crtc_state->c8_planes = crtc_state->update_planes = 0;
	crtc_state->bigjoiner_linked_crtc = to_intel_crtc(from_crtc_state->uapi.crtc);
	crtc_state->bigjoiner_slave = true;
	crtc_state->cpu_transcoder = (enum transcoder)crtc->pipe;
	crtc_state->has_audio = false;

	return 0;
}

static int
intel_crtc_prepare_cleared_state(struct intel_atomic_state *state,
				 struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	struct intel_crtc_state *saved_state;

	saved_state = intel_crtc_state_alloc(crtc);
	if (!saved_state)
		return -ENOMEM;

	/* free the old crtc_state->hw members */
	intel_crtc_free_hw_state(crtc_state);

	/* FIXME: before the switch to atomic started, a new pipe_config was
	 * kzalloc'd. Code that depends on any field being zero should be
	 * fixed, so that the crtc_state can be safely duplicated. For now,
	 * only fields that are know to not cause problems are preserved. */

	saved_state->uapi = crtc_state->uapi;
	saved_state->scaler_state = crtc_state->scaler_state;
	saved_state->shared_dpll = crtc_state->shared_dpll;
	saved_state->dpll_hw_state = crtc_state->dpll_hw_state;
	memcpy(saved_state->icl_port_dplls, crtc_state->icl_port_dplls,
	       sizeof(saved_state->icl_port_dplls));
	saved_state->crc_enabled = crtc_state->crc_enabled;
	if (IS_G4X(dev_priv) ||
	    IS_VALLEYVIEW(dev_priv) || IS_CHERRYVIEW(dev_priv))
		saved_state->wm = crtc_state->wm;

	memcpy(crtc_state, saved_state, sizeof(*crtc_state));
	kfree(saved_state);

	intel_crtc_copy_uapi_to_hw_state(state, crtc_state);

	return 0;
}

static int
intel_modeset_pipe_config(struct intel_atomic_state *state,
			  struct intel_crtc_state *pipe_config)
{
	struct drm_crtc *crtc = pipe_config->uapi.crtc;
	struct drm_i915_private *i915 = to_i915(pipe_config->uapi.crtc->dev);
	struct drm_connector *connector;
	struct drm_connector_state *connector_state;
	int base_bpp, ret, i;
	bool retry = true;

	pipe_config->cpu_transcoder =
		(enum transcoder) to_intel_crtc(crtc)->pipe;

	/*
	 * Sanitize sync polarity flags based on requested ones. If neither
	 * positive or negative polarity is requested, treat this as meaning
	 * negative polarity.
	 */
	if (!(pipe_config->hw.adjusted_mode.flags &
	      (DRM_MODE_FLAG_PHSYNC | DRM_MODE_FLAG_NHSYNC)))
		pipe_config->hw.adjusted_mode.flags |= DRM_MODE_FLAG_NHSYNC;

	if (!(pipe_config->hw.adjusted_mode.flags &
	      (DRM_MODE_FLAG_PVSYNC | DRM_MODE_FLAG_NVSYNC)))
		pipe_config->hw.adjusted_mode.flags |= DRM_MODE_FLAG_NVSYNC;

	ret = compute_baseline_pipe_bpp(to_intel_crtc(crtc),
					pipe_config);
	if (ret)
		return ret;

	base_bpp = pipe_config->pipe_bpp;

	/*
	 * Determine the real pipe dimensions. Note that stereo modes can
	 * increase the actual pipe size due to the frame doubling and
	 * insertion of additional space for blanks between the frame. This
	 * is stored in the crtc timings. We use the requested mode to do this
	 * computation to clearly distinguish it from the adjusted mode, which
	 * can be changed by the connectors in the below retry loop.
	 */
	drm_mode_get_hv_timing(&pipe_config->hw.mode,
			       &pipe_config->pipe_src_w,
			       &pipe_config->pipe_src_h);

	for_each_new_connector_in_state(&state->base, connector, connector_state, i) {
		struct intel_encoder *encoder =
			to_intel_encoder(connector_state->best_encoder);

		if (connector_state->crtc != crtc)
			continue;

		if (!check_single_encoder_cloning(state, to_intel_crtc(crtc), encoder)) {
			drm_dbg_kms(&i915->drm,
				    "rejecting invalid cloning configuration\n");
			return -EINVAL;
		}

		/*
		 * Determine output_types before calling the .compute_config()
		 * hooks so that the hooks can use this information safely.
		 */
		if (encoder->compute_output_type)
			pipe_config->output_types |=
				BIT(encoder->compute_output_type(encoder, pipe_config,
								 connector_state));
		else
			pipe_config->output_types |= BIT(encoder->type);
	}

encoder_retry:
	/* Ensure the port clock defaults are reset when retrying. */
	pipe_config->port_clock = 0;
	pipe_config->pixel_multiplier = 1;

	/* Fill in default crtc timings, allow encoders to overwrite them. */
	drm_mode_set_crtcinfo(&pipe_config->hw.adjusted_mode,
			      CRTC_STEREO_DOUBLE);

	/* Pass our mode to the connectors and the CRTC to give them a chance to
	 * adjust it according to limitations or connector properties, and also
	 * a chance to reject the mode entirely.
	 */
	for_each_new_connector_in_state(&state->base, connector, connector_state, i) {
		struct intel_encoder *encoder =
			to_intel_encoder(connector_state->best_encoder);

		if (connector_state->crtc != crtc)
			continue;

		ret = encoder->compute_config(encoder, pipe_config,
					      connector_state);
		if (ret < 0) {
			if (ret != -EDEADLK)
				drm_dbg_kms(&i915->drm,
					    "Encoder config failure: %d\n",
					    ret);
			return ret;
		}
	}

	/* Set default port clock if not overwritten by the encoder. Needs to be
	 * done afterwards in case the encoder adjusts the mode. */
	if (!pipe_config->port_clock)
		pipe_config->port_clock = pipe_config->hw.adjusted_mode.crtc_clock
			* pipe_config->pixel_multiplier;

	ret = intel_crtc_compute_config(to_intel_crtc(crtc), pipe_config);
	if (ret == -EDEADLK)
		return ret;
	if (ret < 0) {
		drm_dbg_kms(&i915->drm, "CRTC fixup failed\n");
		return ret;
	}

	if (ret == I915_DISPLAY_CONFIG_RETRY) {
		if (drm_WARN(&i915->drm, !retry,
			     "loop in pipe configuration computation\n"))
			return -EINVAL;

		drm_dbg_kms(&i915->drm, "CRTC bw constrained, retrying\n");
		retry = false;
		goto encoder_retry;
	}

	/* Dithering seems to not pass-through bits correctly when it should, so
	 * only enable it on 6bpc panels and when its not a compliance
	 * test requesting 6bpc video pattern.
	 */
	pipe_config->dither = (pipe_config->pipe_bpp == 6*3) &&
		!pipe_config->dither_force_disable;
	drm_dbg_kms(&i915->drm,
		    "hw max bpp: %i, pipe bpp: %i, dithering: %i\n",
		    base_bpp, pipe_config->pipe_bpp, pipe_config->dither);

	return 0;
}

static int
intel_modeset_pipe_config_late(struct intel_crtc_state *crtc_state)
{
	struct intel_atomic_state *state =
		to_intel_atomic_state(crtc_state->uapi.state);
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_connector_state *conn_state;
	struct drm_connector *connector;
	int i;

	for_each_new_connector_in_state(&state->base, connector,
					conn_state, i) {
		struct intel_encoder *encoder =
			to_intel_encoder(conn_state->best_encoder);
		int ret;

		if (conn_state->crtc != &crtc->base ||
		    !encoder->compute_config_late)
			continue;

		ret = encoder->compute_config_late(encoder, crtc_state,
						   conn_state);
		if (ret)
			return ret;
	}

	return 0;
}

bool intel_fuzzy_clock_check(int clock1, int clock2)
{
	int diff;

	if (clock1 == clock2)
		return true;

	if (!clock1 || !clock2)
		return false;

	diff = abs(clock1 - clock2);

	if (((((diff + clock1 + clock2) * 100)) / (clock1 + clock2)) < 105)
		return true;

	return false;
}

static bool
intel_compare_m_n(unsigned int m, unsigned int n,
		  unsigned int m2, unsigned int n2,
		  bool exact)
{
	if (m == m2 && n == n2)
		return true;

	if (exact || !m || !n || !m2 || !n2)
		return false;

	BUILD_BUG_ON(DATA_LINK_M_N_MASK > INT_MAX);

	if (n > n2) {
		while (n > n2) {
			m2 <<= 1;
			n2 <<= 1;
		}
	} else if (n < n2) {
		while (n < n2) {
			m <<= 1;
			n <<= 1;
		}
	}

	if (n != n2)
		return false;

	return intel_fuzzy_clock_check(m, m2);
}

static bool
intel_compare_link_m_n(const struct intel_link_m_n *m_n,
		       const struct intel_link_m_n *m2_n2,
		       bool exact)
{
	return m_n->tu == m2_n2->tu &&
		intel_compare_m_n(m_n->gmch_m, m_n->gmch_n,
				  m2_n2->gmch_m, m2_n2->gmch_n, exact) &&
		intel_compare_m_n(m_n->link_m, m_n->link_n,
				  m2_n2->link_m, m2_n2->link_n, exact);
}

static bool
intel_compare_infoframe(const union hdmi_infoframe *a,
			const union hdmi_infoframe *b)
{
	return memcmp(a, b, sizeof(*a)) == 0;
}

static bool
intel_compare_dp_vsc_sdp(const struct drm_dp_vsc_sdp *a,
			 const struct drm_dp_vsc_sdp *b)
{
	return memcmp(a, b, sizeof(*a)) == 0;
}

static void
pipe_config_infoframe_mismatch(struct drm_i915_private *dev_priv,
			       bool fastset, const char *name,
			       const union hdmi_infoframe *a,
			       const union hdmi_infoframe *b)
{
	if (fastset) {
		if (!drm_debug_enabled(DRM_UT_KMS))
			return;

		drm_dbg_kms(&dev_priv->drm,
			    "fastset mismatch in %s infoframe\n", name);
		drm_dbg_kms(&dev_priv->drm, "expected:\n");
		hdmi_infoframe_log(KERN_DEBUG, dev_priv->drm.dev, a);
		drm_dbg_kms(&dev_priv->drm, "found:\n");
		hdmi_infoframe_log(KERN_DEBUG, dev_priv->drm.dev, b);
	} else {
		drm_err(&dev_priv->drm, "mismatch in %s infoframe\n", name);
		drm_err(&dev_priv->drm, "expected:\n");
		hdmi_infoframe_log(KERN_ERR, dev_priv->drm.dev, a);
		drm_err(&dev_priv->drm, "found:\n");
		hdmi_infoframe_log(KERN_ERR, dev_priv->drm.dev, b);
	}
}

static void
pipe_config_dp_vsc_sdp_mismatch(struct drm_i915_private *dev_priv,
				bool fastset, const char *name,
				const struct drm_dp_vsc_sdp *a,
				const struct drm_dp_vsc_sdp *b)
{
	if (fastset) {
		if (!drm_debug_enabled(DRM_UT_KMS))
			return;

		drm_dbg_kms(&dev_priv->drm,
			    "fastset mismatch in %s dp sdp\n", name);
		drm_dbg_kms(&dev_priv->drm, "expected:\n");
		drm_dp_vsc_sdp_log(KERN_DEBUG, dev_priv->drm.dev, a);
		drm_dbg_kms(&dev_priv->drm, "found:\n");
		drm_dp_vsc_sdp_log(KERN_DEBUG, dev_priv->drm.dev, b);
	} else {
		drm_err(&dev_priv->drm, "mismatch in %s dp sdp\n", name);
		drm_err(&dev_priv->drm, "expected:\n");
		drm_dp_vsc_sdp_log(KERN_ERR, dev_priv->drm.dev, a);
		drm_err(&dev_priv->drm, "found:\n");
		drm_dp_vsc_sdp_log(KERN_ERR, dev_priv->drm.dev, b);
	}
}

static void __printf(4, 5)
pipe_config_mismatch(bool fastset, const struct intel_crtc *crtc,
		     const char *name, const char *format, ...)
{
	struct drm_i915_private *i915 = to_i915(crtc->base.dev);
	struct va_format vaf;
	va_list args;

	va_start(args, format);
	vaf.fmt = format;
	vaf.va = &args;

	if (fastset)
		drm_dbg_kms(&i915->drm,
			    "[CRTC:%d:%s] fastset mismatch in %s %pV\n",
			    crtc->base.base.id, crtc->base.name, name, &vaf);
	else
		drm_err(&i915->drm, "[CRTC:%d:%s] mismatch in %s %pV\n",
			crtc->base.base.id, crtc->base.name, name, &vaf);

	va_end(args);
}

static bool fastboot_enabled(struct drm_i915_private *dev_priv)
{
	if (dev_priv->params.fastboot != -1)
		return dev_priv->params.fastboot;

	/* Enable fastboot by default on Skylake and newer */
	if (DISPLAY_VER(dev_priv) >= 9)
		return true;

	/* Enable fastboot by default on VLV and CHV */
	if (IS_VALLEYVIEW(dev_priv) || IS_CHERRYVIEW(dev_priv))
		return true;

	/* Disabled by default on all others */
	return false;
}

static bool
intel_pipe_config_compare(const struct intel_crtc_state *current_config,
			  const struct intel_crtc_state *pipe_config,
			  bool fastset)
{
	struct drm_i915_private *dev_priv = to_i915(current_config->uapi.crtc->dev);
	struct intel_crtc *crtc = to_intel_crtc(pipe_config->uapi.crtc);
	bool ret = true;
	u32 bp_gamma = 0;
	bool fixup_inherited = fastset &&
		current_config->inherited && !pipe_config->inherited;

	if (fixup_inherited && !fastboot_enabled(dev_priv)) {
		drm_dbg_kms(&dev_priv->drm,
			    "initial modeset and fastboot not set\n");
		ret = false;
	}

#define PIPE_CONF_CHECK_X(name) do { \
	if (current_config->name != pipe_config->name) { \
		pipe_config_mismatch(fastset, crtc, __stringify(name), \
				     "(expected 0x%08x, found 0x%08x)", \
				     current_config->name, \
				     pipe_config->name); \
		ret = false; \
	} \
} while (0)

#define PIPE_CONF_CHECK_X_WITH_MASK(name, mask) do { \
	if ((current_config->name & (mask)) != (pipe_config->name & (mask))) { \
		pipe_config_mismatch(fastset, crtc, __stringify(name), \
				     "(expected 0x%08x, found 0x%08x)", \
				     current_config->name & (mask), \
				     pipe_config->name & (mask)); \
		ret = false; \
	} \
} while (0)

#define PIPE_CONF_CHECK_I(name) do { \
	if (current_config->name != pipe_config->name) { \
		pipe_config_mismatch(fastset, crtc, __stringify(name), \
				     "(expected %i, found %i)", \
				     current_config->name, \
				     pipe_config->name); \
		ret = false; \
	} \
} while (0)

#define PIPE_CONF_CHECK_BOOL(name) do { \
	if (current_config->name != pipe_config->name) { \
		pipe_config_mismatch(fastset, crtc,  __stringify(name), \
				     "(expected %s, found %s)", \
				     yesno(current_config->name), \
				     yesno(pipe_config->name)); \
		ret = false; \
	} \
} while (0)

/*
 * Checks state where we only read out the enabling, but not the entire
 * state itself (like full infoframes or ELD for audio). These states
 * require a full modeset on bootup to fix up.
 */
#define PIPE_CONF_CHECK_BOOL_INCOMPLETE(name) do { \
	if (!fixup_inherited || (!current_config->name && !pipe_config->name)) { \
		PIPE_CONF_CHECK_BOOL(name); \
	} else { \
		pipe_config_mismatch(fastset, crtc, __stringify(name), \
				     "unable to verify whether state matches exactly, forcing modeset (expected %s, found %s)", \
				     yesno(current_config->name), \
				     yesno(pipe_config->name)); \
		ret = false; \
	} \
} while (0)

#define PIPE_CONF_CHECK_P(name) do { \
	if (current_config->name != pipe_config->name) { \
		pipe_config_mismatch(fastset, crtc, __stringify(name), \
				     "(expected %p, found %p)", \
				     current_config->name, \
				     pipe_config->name); \
		ret = false; \
	} \
} while (0)

#define PIPE_CONF_CHECK_M_N(name) do { \
	if (!intel_compare_link_m_n(&current_config->name, \
				    &pipe_config->name,\
				    !fastset)) { \
		pipe_config_mismatch(fastset, crtc, __stringify(name), \
				     "(expected tu %i gmch %i/%i link %i/%i, " \
				     "found tu %i, gmch %i/%i link %i/%i)", \
				     current_config->name.tu, \
				     current_config->name.gmch_m, \
				     current_config->name.gmch_n, \
				     current_config->name.link_m, \
				     current_config->name.link_n, \
				     pipe_config->name.tu, \
				     pipe_config->name.gmch_m, \
				     pipe_config->name.gmch_n, \
				     pipe_config->name.link_m, \
				     pipe_config->name.link_n); \
		ret = false; \
	} \
} while (0)

/* This is required for BDW+ where there is only one set of registers for
 * switching between high and low RR.
 * This macro can be used whenever a comparison has to be made between one
 * hw state and multiple sw state variables.
 */
#define PIPE_CONF_CHECK_M_N_ALT(name, alt_name) do { \
	if (!intel_compare_link_m_n(&current_config->name, \
				    &pipe_config->name, !fastset) && \
	    !intel_compare_link_m_n(&current_config->alt_name, \
				    &pipe_config->name, !fastset)) { \
		pipe_config_mismatch(fastset, crtc, __stringify(name), \
				     "(expected tu %i gmch %i/%i link %i/%i, " \
				     "or tu %i gmch %i/%i link %i/%i, " \
				     "found tu %i, gmch %i/%i link %i/%i)", \
				     current_config->name.tu, \
				     current_config->name.gmch_m, \
				     current_config->name.gmch_n, \
				     current_config->name.link_m, \
				     current_config->name.link_n, \
				     current_config->alt_name.tu, \
				     current_config->alt_name.gmch_m, \
				     current_config->alt_name.gmch_n, \
				     current_config->alt_name.link_m, \
				     current_config->alt_name.link_n, \
				     pipe_config->name.tu, \
				     pipe_config->name.gmch_m, \
				     pipe_config->name.gmch_n, \
				     pipe_config->name.link_m, \
				     pipe_config->name.link_n); \
		ret = false; \
	} \
} while (0)

#define PIPE_CONF_CHECK_FLAGS(name, mask) do { \
	if ((current_config->name ^ pipe_config->name) & (mask)) { \
		pipe_config_mismatch(fastset, crtc, __stringify(name), \
				     "(%x) (expected %i, found %i)", \
				     (mask), \
				     current_config->name & (mask), \
				     pipe_config->name & (mask)); \
		ret = false; \
	} \
} while (0)

#define PIPE_CONF_CHECK_CLOCK_FUZZY(name) do { \
	if (!intel_fuzzy_clock_check(current_config->name, pipe_config->name)) { \
		pipe_config_mismatch(fastset, crtc, __stringify(name), \
				     "(expected %i, found %i)", \
				     current_config->name, \
				     pipe_config->name); \
		ret = false; \
	} \
} while (0)

#define PIPE_CONF_CHECK_INFOFRAME(name) do { \
	if (!intel_compare_infoframe(&current_config->infoframes.name, \
				     &pipe_config->infoframes.name)) { \
		pipe_config_infoframe_mismatch(dev_priv, fastset, __stringify(name), \
					       &current_config->infoframes.name, \
					       &pipe_config->infoframes.name); \
		ret = false; \
	} \
} while (0)

#define PIPE_CONF_CHECK_DP_VSC_SDP(name) do { \
	if (!current_config->has_psr && !pipe_config->has_psr && \
	    !intel_compare_dp_vsc_sdp(&current_config->infoframes.name, \
				      &pipe_config->infoframes.name)) { \
		pipe_config_dp_vsc_sdp_mismatch(dev_priv, fastset, __stringify(name), \
						&current_config->infoframes.name, \
						&pipe_config->infoframes.name); \
		ret = false; \
	} \
} while (0)

#define PIPE_CONF_CHECK_COLOR_LUT(name1, name2, bit_precision) do { \
	if (current_config->name1 != pipe_config->name1) { \
		pipe_config_mismatch(fastset, crtc, __stringify(name1), \
				"(expected %i, found %i, won't compare lut values)", \
				current_config->name1, \
				pipe_config->name1); \
		ret = false;\
	} else { \
		if (!intel_color_lut_equal(current_config->name2, \
					pipe_config->name2, pipe_config->name1, \
					bit_precision)) { \
			pipe_config_mismatch(fastset, crtc, __stringify(name2), \
					"hw_state doesn't match sw_state"); \
			ret = false; \
		} \
	} \
} while (0)

#define PIPE_CONF_QUIRK(quirk) \
	((current_config->quirks | pipe_config->quirks) & (quirk))

	PIPE_CONF_CHECK_I(cpu_transcoder);

	PIPE_CONF_CHECK_BOOL(has_pch_encoder);
	PIPE_CONF_CHECK_I(fdi_lanes);
	PIPE_CONF_CHECK_M_N(fdi_m_n);

	PIPE_CONF_CHECK_I(lane_count);
	PIPE_CONF_CHECK_X(lane_lat_optim_mask);

	if (DISPLAY_VER(dev_priv) < 8) {
		PIPE_CONF_CHECK_M_N(dp_m_n);

		if (current_config->has_drrs)
			PIPE_CONF_CHECK_M_N(dp_m2_n2);
	} else
		PIPE_CONF_CHECK_M_N_ALT(dp_m_n, dp_m2_n2);

	PIPE_CONF_CHECK_X(output_types);

	/* FIXME do the readout properly and get rid of this quirk */
	if (!PIPE_CONF_QUIRK(PIPE_CONFIG_QUIRK_BIGJOINER_SLAVE)) {
		PIPE_CONF_CHECK_I(hw.pipe_mode.crtc_hdisplay);
		PIPE_CONF_CHECK_I(hw.pipe_mode.crtc_htotal);
		PIPE_CONF_CHECK_I(hw.pipe_mode.crtc_hblank_start);
		PIPE_CONF_CHECK_I(hw.pipe_mode.crtc_hblank_end);
		PIPE_CONF_CHECK_I(hw.pipe_mode.crtc_hsync_start);
		PIPE_CONF_CHECK_I(hw.pipe_mode.crtc_hsync_end);

		PIPE_CONF_CHECK_I(hw.pipe_mode.crtc_vdisplay);
		PIPE_CONF_CHECK_I(hw.pipe_mode.crtc_vtotal);
		PIPE_CONF_CHECK_I(hw.pipe_mode.crtc_vblank_start);
		PIPE_CONF_CHECK_I(hw.pipe_mode.crtc_vblank_end);
		PIPE_CONF_CHECK_I(hw.pipe_mode.crtc_vsync_start);
		PIPE_CONF_CHECK_I(hw.pipe_mode.crtc_vsync_end);

		PIPE_CONF_CHECK_I(hw.adjusted_mode.crtc_hdisplay);
		PIPE_CONF_CHECK_I(hw.adjusted_mode.crtc_htotal);
		PIPE_CONF_CHECK_I(hw.adjusted_mode.crtc_hblank_start);
		PIPE_CONF_CHECK_I(hw.adjusted_mode.crtc_hblank_end);
		PIPE_CONF_CHECK_I(hw.adjusted_mode.crtc_hsync_start);
		PIPE_CONF_CHECK_I(hw.adjusted_mode.crtc_hsync_end);

		PIPE_CONF_CHECK_I(hw.adjusted_mode.crtc_vdisplay);
		PIPE_CONF_CHECK_I(hw.adjusted_mode.crtc_vtotal);
		PIPE_CONF_CHECK_I(hw.adjusted_mode.crtc_vblank_start);
		PIPE_CONF_CHECK_I(hw.adjusted_mode.crtc_vblank_end);
		PIPE_CONF_CHECK_I(hw.adjusted_mode.crtc_vsync_start);
		PIPE_CONF_CHECK_I(hw.adjusted_mode.crtc_vsync_end);

		PIPE_CONF_CHECK_I(pixel_multiplier);

		PIPE_CONF_CHECK_FLAGS(hw.adjusted_mode.flags,
				      DRM_MODE_FLAG_INTERLACE);

		if (!PIPE_CONF_QUIRK(PIPE_CONFIG_QUIRK_MODE_SYNC_FLAGS)) {
			PIPE_CONF_CHECK_FLAGS(hw.adjusted_mode.flags,
					      DRM_MODE_FLAG_PHSYNC);
			PIPE_CONF_CHECK_FLAGS(hw.adjusted_mode.flags,
					      DRM_MODE_FLAG_NHSYNC);
			PIPE_CONF_CHECK_FLAGS(hw.adjusted_mode.flags,
					      DRM_MODE_FLAG_PVSYNC);
			PIPE_CONF_CHECK_FLAGS(hw.adjusted_mode.flags,
					      DRM_MODE_FLAG_NVSYNC);
		}
	}

	PIPE_CONF_CHECK_I(output_format);
	PIPE_CONF_CHECK_BOOL(has_hdmi_sink);
	if ((DISPLAY_VER(dev_priv) < 8 && !IS_HASWELL(dev_priv)) ||
	    IS_VALLEYVIEW(dev_priv) || IS_CHERRYVIEW(dev_priv))
		PIPE_CONF_CHECK_BOOL(limited_color_range);

	PIPE_CONF_CHECK_BOOL(hdmi_scrambling);
	PIPE_CONF_CHECK_BOOL(hdmi_high_tmds_clock_ratio);
	PIPE_CONF_CHECK_BOOL(has_infoframe);
	/* FIXME do the readout properly and get rid of this quirk */
	if (!PIPE_CONF_QUIRK(PIPE_CONFIG_QUIRK_BIGJOINER_SLAVE))
		PIPE_CONF_CHECK_BOOL(fec_enable);

	PIPE_CONF_CHECK_BOOL_INCOMPLETE(has_audio);

	PIPE_CONF_CHECK_X(gmch_pfit.control);
	/* pfit ratios are autocomputed by the hw on gen4+ */
	if (DISPLAY_VER(dev_priv) < 4)
		PIPE_CONF_CHECK_X(gmch_pfit.pgm_ratios);
	PIPE_CONF_CHECK_X(gmch_pfit.lvds_border_bits);

	/*
	 * Changing the EDP transcoder input mux
	 * (A_ONOFF vs. A_ON) requires a full modeset.
	 */
	PIPE_CONF_CHECK_BOOL(pch_pfit.force_thru);

	if (!fastset) {
		PIPE_CONF_CHECK_I(pipe_src_w);
		PIPE_CONF_CHECK_I(pipe_src_h);

		PIPE_CONF_CHECK_BOOL(pch_pfit.enabled);
		if (current_config->pch_pfit.enabled) {
			PIPE_CONF_CHECK_I(pch_pfit.dst.x1);
			PIPE_CONF_CHECK_I(pch_pfit.dst.y1);
			PIPE_CONF_CHECK_I(pch_pfit.dst.x2);
			PIPE_CONF_CHECK_I(pch_pfit.dst.y2);
		}

		PIPE_CONF_CHECK_I(scaler_state.scaler_id);
		/* FIXME do the readout properly and get rid of this quirk */
		if (!PIPE_CONF_QUIRK(PIPE_CONFIG_QUIRK_BIGJOINER_SLAVE))
			PIPE_CONF_CHECK_CLOCK_FUZZY(pixel_rate);

		PIPE_CONF_CHECK_X(gamma_mode);
		if (IS_CHERRYVIEW(dev_priv))
			PIPE_CONF_CHECK_X(cgm_mode);
		else
			PIPE_CONF_CHECK_X(csc_mode);
		PIPE_CONF_CHECK_BOOL(gamma_enable);
		PIPE_CONF_CHECK_BOOL(csc_enable);

		PIPE_CONF_CHECK_I(linetime);
		PIPE_CONF_CHECK_I(ips_linetime);

		bp_gamma = intel_color_get_gamma_bit_precision(pipe_config);
		if (bp_gamma)
			PIPE_CONF_CHECK_COLOR_LUT(gamma_mode, hw.gamma_lut, bp_gamma);

		PIPE_CONF_CHECK_BOOL(has_psr);
		PIPE_CONF_CHECK_BOOL(has_psr2);
		PIPE_CONF_CHECK_BOOL(enable_psr2_sel_fetch);
		PIPE_CONF_CHECK_I(dc3co_exitline);
	}

	PIPE_CONF_CHECK_BOOL(double_wide);

	PIPE_CONF_CHECK_P(shared_dpll);

	/* FIXME do the readout properly and get rid of this quirk */
	if (!PIPE_CONF_QUIRK(PIPE_CONFIG_QUIRK_BIGJOINER_SLAVE)) {
		PIPE_CONF_CHECK_X(dpll_hw_state.dpll);
		PIPE_CONF_CHECK_X(dpll_hw_state.dpll_md);
		PIPE_CONF_CHECK_X(dpll_hw_state.fp0);
		PIPE_CONF_CHECK_X(dpll_hw_state.fp1);
		PIPE_CONF_CHECK_X(dpll_hw_state.wrpll);
		PIPE_CONF_CHECK_X(dpll_hw_state.spll);
		PIPE_CONF_CHECK_X(dpll_hw_state.ctrl1);
		PIPE_CONF_CHECK_X(dpll_hw_state.cfgcr1);
		PIPE_CONF_CHECK_X(dpll_hw_state.cfgcr2);
		PIPE_CONF_CHECK_X(dpll_hw_state.cfgcr0);
		PIPE_CONF_CHECK_X(dpll_hw_state.ebb0);
		PIPE_CONF_CHECK_X(dpll_hw_state.ebb4);
		PIPE_CONF_CHECK_X(dpll_hw_state.pll0);
		PIPE_CONF_CHECK_X(dpll_hw_state.pll1);
		PIPE_CONF_CHECK_X(dpll_hw_state.pll2);
		PIPE_CONF_CHECK_X(dpll_hw_state.pll3);
		PIPE_CONF_CHECK_X(dpll_hw_state.pll6);
		PIPE_CONF_CHECK_X(dpll_hw_state.pll8);
		PIPE_CONF_CHECK_X(dpll_hw_state.pll9);
		PIPE_CONF_CHECK_X(dpll_hw_state.pll10);
		PIPE_CONF_CHECK_X(dpll_hw_state.pcsdw12);
		PIPE_CONF_CHECK_X(dpll_hw_state.mg_refclkin_ctl);
		PIPE_CONF_CHECK_X(dpll_hw_state.mg_clktop2_coreclkctl1);
		PIPE_CONF_CHECK_X(dpll_hw_state.mg_clktop2_hsclkctl);
		PIPE_CONF_CHECK_X(dpll_hw_state.mg_pll_div0);
		PIPE_CONF_CHECK_X(dpll_hw_state.mg_pll_div1);
		PIPE_CONF_CHECK_X(dpll_hw_state.mg_pll_lf);
		PIPE_CONF_CHECK_X(dpll_hw_state.mg_pll_frac_lock);
		PIPE_CONF_CHECK_X(dpll_hw_state.mg_pll_ssc);
		PIPE_CONF_CHECK_X(dpll_hw_state.mg_pll_bias);
		PIPE_CONF_CHECK_X(dpll_hw_state.mg_pll_tdc_coldst_bias);

		PIPE_CONF_CHECK_X(dsi_pll.ctrl);
		PIPE_CONF_CHECK_X(dsi_pll.div);

		if (IS_G4X(dev_priv) || DISPLAY_VER(dev_priv) >= 5)
			PIPE_CONF_CHECK_I(pipe_bpp);

		PIPE_CONF_CHECK_CLOCK_FUZZY(hw.pipe_mode.crtc_clock);
		PIPE_CONF_CHECK_CLOCK_FUZZY(hw.adjusted_mode.crtc_clock);
		PIPE_CONF_CHECK_CLOCK_FUZZY(port_clock);

		PIPE_CONF_CHECK_I(min_voltage_level);
	}

	if (fastset && (current_config->has_psr || pipe_config->has_psr))
		PIPE_CONF_CHECK_X_WITH_MASK(infoframes.enable,
					    ~intel_hdmi_infoframe_enable(DP_SDP_VSC));
	else
		PIPE_CONF_CHECK_X(infoframes.enable);

	PIPE_CONF_CHECK_X(infoframes.gcp);
	PIPE_CONF_CHECK_INFOFRAME(avi);
	PIPE_CONF_CHECK_INFOFRAME(spd);
	PIPE_CONF_CHECK_INFOFRAME(hdmi);
	PIPE_CONF_CHECK_INFOFRAME(drm);
	PIPE_CONF_CHECK_DP_VSC_SDP(vsc);

	PIPE_CONF_CHECK_X(sync_mode_slaves_mask);
	PIPE_CONF_CHECK_I(master_transcoder);
	PIPE_CONF_CHECK_BOOL(bigjoiner);
	PIPE_CONF_CHECK_BOOL(bigjoiner_slave);
	PIPE_CONF_CHECK_P(bigjoiner_linked_crtc);

	PIPE_CONF_CHECK_I(dsc.compression_enable);
	PIPE_CONF_CHECK_I(dsc.dsc_split);
	PIPE_CONF_CHECK_I(dsc.compressed_bpp);

	PIPE_CONF_CHECK_BOOL(splitter.enable);
	PIPE_CONF_CHECK_I(splitter.link_count);
	PIPE_CONF_CHECK_I(splitter.pixel_overlap);

	PIPE_CONF_CHECK_I(mst_master_transcoder);

	PIPE_CONF_CHECK_BOOL(vrr.enable);
	PIPE_CONF_CHECK_I(vrr.vmin);
	PIPE_CONF_CHECK_I(vrr.vmax);
	PIPE_CONF_CHECK_I(vrr.flipline);
	PIPE_CONF_CHECK_I(vrr.pipeline_full);
	PIPE_CONF_CHECK_I(vrr.guardband);

#undef PIPE_CONF_CHECK_X
#undef PIPE_CONF_CHECK_I
#undef PIPE_CONF_CHECK_BOOL
#undef PIPE_CONF_CHECK_BOOL_INCOMPLETE
#undef PIPE_CONF_CHECK_P
#undef PIPE_CONF_CHECK_FLAGS
#undef PIPE_CONF_CHECK_CLOCK_FUZZY
#undef PIPE_CONF_CHECK_COLOR_LUT
#undef PIPE_CONF_QUIRK

	return ret;
}

static void intel_pipe_config_sanity_check(struct drm_i915_private *dev_priv,
					   const struct intel_crtc_state *pipe_config)
{
	if (pipe_config->has_pch_encoder) {
		int fdi_dotclock = intel_dotclock_calculate(intel_fdi_link_freq(dev_priv, pipe_config),
							    &pipe_config->fdi_m_n);
		int dotclock = pipe_config->hw.adjusted_mode.crtc_clock;

		/*
		 * FDI already provided one idea for the dotclock.
		 * Yell if the encoder disagrees.
		 */
		drm_WARN(&dev_priv->drm,
			 !intel_fuzzy_clock_check(fdi_dotclock, dotclock),
			 "FDI dotclock and encoder dotclock mismatch, fdi: %i, encoder: %i\n",
			 fdi_dotclock, dotclock);
	}
}

static void verify_wm_state(struct intel_crtc *crtc,
			    struct intel_crtc_state *new_crtc_state)
{
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	struct skl_hw_state {
		struct skl_ddb_entry ddb_y[I915_MAX_PLANES];
		struct skl_ddb_entry ddb_uv[I915_MAX_PLANES];
		struct skl_pipe_wm wm;
	} *hw;
	const struct skl_pipe_wm *sw_wm = &new_crtc_state->wm.skl.optimal;
	int level, max_level = ilk_wm_max_level(dev_priv);
	struct intel_plane *plane;
	u8 hw_enabled_slices;

	if (DISPLAY_VER(dev_priv) < 9 || !new_crtc_state->hw.active)
		return;

	hw = kzalloc(sizeof(*hw), GFP_KERNEL);
	if (!hw)
		return;

	skl_pipe_wm_get_hw_state(crtc, &hw->wm);

	skl_pipe_ddb_get_hw_state(crtc, hw->ddb_y, hw->ddb_uv);

	hw_enabled_slices = intel_enabled_dbuf_slices_mask(dev_priv);

	if (DISPLAY_VER(dev_priv) >= 11 &&
	    hw_enabled_slices != dev_priv->dbuf.enabled_slices)
		drm_err(&dev_priv->drm,
			"mismatch in DBUF Slices (expected 0x%x, got 0x%x)\n",
			dev_priv->dbuf.enabled_slices,
			hw_enabled_slices);

	for_each_intel_plane_on_crtc(&dev_priv->drm, crtc, plane) {
		const struct skl_ddb_entry *hw_ddb_entry, *sw_ddb_entry;
		const struct skl_wm_level *hw_wm_level, *sw_wm_level;

		/* Watermarks */
		for (level = 0; level <= max_level; level++) {
			hw_wm_level = &hw->wm.planes[plane->id].wm[level];
			sw_wm_level = skl_plane_wm_level(sw_wm, plane->id, level);

			if (skl_wm_level_equals(hw_wm_level, sw_wm_level))
				continue;

			drm_err(&dev_priv->drm,
				"[PLANE:%d:%s] mismatch in WM%d (expected e=%d b=%u l=%u, got e=%d b=%u l=%u)\n",
				plane->base.base.id, plane->base.name, level,
				sw_wm_level->enable,
				sw_wm_level->blocks,
				sw_wm_level->lines,
				hw_wm_level->enable,
				hw_wm_level->blocks,
				hw_wm_level->lines);
		}

		hw_wm_level = &hw->wm.planes[plane->id].trans_wm;
		sw_wm_level = skl_plane_trans_wm(sw_wm, plane->id);

		if (!skl_wm_level_equals(hw_wm_level, sw_wm_level)) {
			drm_err(&dev_priv->drm,
				"[PLANE:%d:%s] mismatch in trans WM (expected e=%d b=%u l=%u, got e=%d b=%u l=%u)\n",
				plane->base.base.id, plane->base.name,
				sw_wm_level->enable,
				sw_wm_level->blocks,
				sw_wm_level->lines,
				hw_wm_level->enable,
				hw_wm_level->blocks,
				hw_wm_level->lines);
		}

		hw_wm_level = &hw->wm.planes[plane->id].sagv.wm0;
		sw_wm_level = &sw_wm->planes[plane->id].sagv.wm0;

		if (HAS_HW_SAGV_WM(dev_priv) &&
		    !skl_wm_level_equals(hw_wm_level, sw_wm_level)) {
			drm_err(&dev_priv->drm,
				"[PLANE:%d:%s] mismatch in SAGV WM (expected e=%d b=%u l=%u, got e=%d b=%u l=%u)\n",
				plane->base.base.id, plane->base.name,
				sw_wm_level->enable,
				sw_wm_level->blocks,
				sw_wm_level->lines,
				hw_wm_level->enable,
				hw_wm_level->blocks,
				hw_wm_level->lines);
		}

		hw_wm_level = &hw->wm.planes[plane->id].sagv.trans_wm;
		sw_wm_level = &sw_wm->planes[plane->id].sagv.trans_wm;

		if (HAS_HW_SAGV_WM(dev_priv) &&
		    !skl_wm_level_equals(hw_wm_level, sw_wm_level)) {
			drm_err(&dev_priv->drm,
				"[PLANE:%d:%s] mismatch in SAGV trans WM (expected e=%d b=%u l=%u, got e=%d b=%u l=%u)\n",
				plane->base.base.id, plane->base.name,
				sw_wm_level->enable,
				sw_wm_level->blocks,
				sw_wm_level->lines,
				hw_wm_level->enable,
				hw_wm_level->blocks,
				hw_wm_level->lines);
		}

		/* DDB */
		hw_ddb_entry = &hw->ddb_y[plane->id];
		sw_ddb_entry = &new_crtc_state->wm.skl.plane_ddb_y[plane->id];

		if (!skl_ddb_entry_equal(hw_ddb_entry, sw_ddb_entry)) {
			drm_err(&dev_priv->drm,
				"[PLANE:%d:%s] mismatch in DDB (expected (%u,%u), found (%u,%u))\n",
				plane->base.base.id, plane->base.name,
				sw_ddb_entry->start, sw_ddb_entry->end,
				hw_ddb_entry->start, hw_ddb_entry->end);
		}
	}

	kfree(hw);
}

static void
verify_connector_state(struct intel_atomic_state *state,
		       struct intel_crtc *crtc)
{
	struct drm_connector *connector;
	struct drm_connector_state *new_conn_state;
	int i;

	for_each_new_connector_in_state(&state->base, connector, new_conn_state, i) {
		struct drm_encoder *encoder = connector->encoder;
		struct intel_crtc_state *crtc_state = NULL;

		if (new_conn_state->crtc != &crtc->base)
			continue;

		if (crtc)
			crtc_state = intel_atomic_get_new_crtc_state(state, crtc);

		intel_connector_verify_state(crtc_state, new_conn_state);

		I915_STATE_WARN(new_conn_state->best_encoder != encoder,
		     "connector's atomic encoder doesn't match legacy encoder\n");
	}
}

static void
verify_encoder_state(struct drm_i915_private *dev_priv, struct intel_atomic_state *state)
{
	struct intel_encoder *encoder;
	struct drm_connector *connector;
	struct drm_connector_state *old_conn_state, *new_conn_state;
	int i;

	for_each_intel_encoder(&dev_priv->drm, encoder) {
		bool enabled = false, found = false;
		enum pipe pipe;

		drm_dbg_kms(&dev_priv->drm, "[ENCODER:%d:%s]\n",
			    encoder->base.base.id,
			    encoder->base.name);

		for_each_oldnew_connector_in_state(&state->base, connector, old_conn_state,
						   new_conn_state, i) {
			if (old_conn_state->best_encoder == &encoder->base)
				found = true;

			if (new_conn_state->best_encoder != &encoder->base)
				continue;
			found = enabled = true;

			I915_STATE_WARN(new_conn_state->crtc !=
					encoder->base.crtc,
			     "connector's crtc doesn't match encoder crtc\n");
		}

		if (!found)
			continue;

		I915_STATE_WARN(!!encoder->base.crtc != enabled,
		     "encoder's enabled state mismatch "
		     "(expected %i, found %i)\n",
		     !!encoder->base.crtc, enabled);

		if (!encoder->base.crtc) {
			bool active;

			active = encoder->get_hw_state(encoder, &pipe);
			I915_STATE_WARN(active,
			     "encoder detached but still enabled on pipe %c.\n",
			     pipe_name(pipe));
		}
	}
}

static void
verify_crtc_state(struct intel_crtc *crtc,
		  struct intel_crtc_state *old_crtc_state,
		  struct intel_crtc_state *new_crtc_state)
{
	struct drm_device *dev = crtc->base.dev;
	struct drm_i915_private *dev_priv = to_i915(dev);
	struct intel_encoder *encoder;
	struct intel_crtc_state *pipe_config = old_crtc_state;
	struct drm_atomic_state *state = old_crtc_state->uapi.state;
	struct intel_crtc *master = crtc;

	__drm_atomic_helper_crtc_destroy_state(&old_crtc_state->uapi);
	intel_crtc_free_hw_state(old_crtc_state);
	intel_crtc_state_reset(old_crtc_state, crtc);
	old_crtc_state->uapi.state = state;

	drm_dbg_kms(&dev_priv->drm, "[CRTC:%d:%s]\n", crtc->base.base.id,
		    crtc->base.name);

	pipe_config->hw.enable = new_crtc_state->hw.enable;

	intel_crtc_get_pipe_config(pipe_config);

	/* we keep both pipes enabled on 830 */
	if (IS_I830(dev_priv) && pipe_config->hw.active)
		pipe_config->hw.active = new_crtc_state->hw.active;

	I915_STATE_WARN(new_crtc_state->hw.active != pipe_config->hw.active,
			"crtc active state doesn't match with hw state "
			"(expected %i, found %i)\n",
			new_crtc_state->hw.active, pipe_config->hw.active);

	I915_STATE_WARN(crtc->active != new_crtc_state->hw.active,
			"transitional active state does not match atomic hw state "
			"(expected %i, found %i)\n",
			new_crtc_state->hw.active, crtc->active);

	if (new_crtc_state->bigjoiner_slave)
		master = new_crtc_state->bigjoiner_linked_crtc;

	for_each_encoder_on_crtc(dev, &master->base, encoder) {
		enum pipe pipe;
		bool active;

		active = encoder->get_hw_state(encoder, &pipe);
		I915_STATE_WARN(active != new_crtc_state->hw.active,
				"[ENCODER:%i] active %i with crtc active %i\n",
				encoder->base.base.id, active,
				new_crtc_state->hw.active);

		I915_STATE_WARN(active && master->pipe != pipe,
				"Encoder connected to wrong pipe %c\n",
				pipe_name(pipe));

		if (active)
			intel_encoder_get_config(encoder, pipe_config);
	}

	if (!new_crtc_state->hw.active)
		return;

	intel_pipe_config_sanity_check(dev_priv, pipe_config);

	if (!intel_pipe_config_compare(new_crtc_state,
				       pipe_config, false)) {
		I915_STATE_WARN(1, "pipe state doesn't match!\n");
		intel_dump_pipe_config(pipe_config, NULL, "[hw state]");
		intel_dump_pipe_config(new_crtc_state, NULL, "[sw state]");
	}
}

static void
intel_verify_planes(struct intel_atomic_state *state)
{
	struct intel_plane *plane;
	const struct intel_plane_state *plane_state;
	int i;

	for_each_new_intel_plane_in_state(state, plane,
					  plane_state, i)
		assert_plane(plane, plane_state->planar_slave ||
			     plane_state->uapi.visible);
}

static void
verify_single_dpll_state(struct drm_i915_private *dev_priv,
			 struct intel_shared_dpll *pll,
			 struct intel_crtc *crtc,
			 struct intel_crtc_state *new_crtc_state)
{
	struct intel_dpll_hw_state dpll_hw_state;
	u8 pipe_mask;
	bool active;

	memset(&dpll_hw_state, 0, sizeof(dpll_hw_state));

	drm_dbg_kms(&dev_priv->drm, "%s\n", pll->info->name);

	active = intel_dpll_get_hw_state(dev_priv, pll, &dpll_hw_state);

	if (!(pll->info->flags & INTEL_DPLL_ALWAYS_ON)) {
		I915_STATE_WARN(!pll->on && pll->active_mask,
		     "pll in active use but not on in sw tracking\n");
		I915_STATE_WARN(pll->on && !pll->active_mask,
		     "pll is on but not used by any active pipe\n");
		I915_STATE_WARN(pll->on != active,
		     "pll on state mismatch (expected %i, found %i)\n",
		     pll->on, active);
	}

	if (!crtc) {
		I915_STATE_WARN(pll->active_mask & ~pll->state.pipe_mask,
				"more active pll users than references: 0x%x vs 0x%x\n",
				pll->active_mask, pll->state.pipe_mask);

		return;
	}

	pipe_mask = BIT(crtc->pipe);

	if (new_crtc_state->hw.active)
		I915_STATE_WARN(!(pll->active_mask & pipe_mask),
				"pll active mismatch (expected pipe %c in active mask 0x%x)\n",
				pipe_name(crtc->pipe), pll->active_mask);
	else
		I915_STATE_WARN(pll->active_mask & pipe_mask,
				"pll active mismatch (didn't expect pipe %c in active mask 0x%x)\n",
				pipe_name(crtc->pipe), pll->active_mask);

	I915_STATE_WARN(!(pll->state.pipe_mask & pipe_mask),
			"pll enabled crtcs mismatch (expected 0x%x in 0x%x)\n",
			pipe_mask, pll->state.pipe_mask);

	I915_STATE_WARN(pll->on && memcmp(&pll->state.hw_state,
					  &dpll_hw_state,
					  sizeof(dpll_hw_state)),
			"pll hw state mismatch\n");
}

static void
verify_shared_dpll_state(struct intel_crtc *crtc,
			 struct intel_crtc_state *old_crtc_state,
			 struct intel_crtc_state *new_crtc_state)
{
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);

	if (new_crtc_state->shared_dpll)
		verify_single_dpll_state(dev_priv, new_crtc_state->shared_dpll, crtc, new_crtc_state);

	if (old_crtc_state->shared_dpll &&
	    old_crtc_state->shared_dpll != new_crtc_state->shared_dpll) {
		u8 pipe_mask = BIT(crtc->pipe);
		struct intel_shared_dpll *pll = old_crtc_state->shared_dpll;

		I915_STATE_WARN(pll->active_mask & pipe_mask,
				"pll active mismatch (didn't expect pipe %c in active mask (0x%x))\n",
				pipe_name(crtc->pipe), pll->active_mask);
		I915_STATE_WARN(pll->state.pipe_mask & pipe_mask,
				"pll enabled crtcs mismatch (found %x in enabled mask (0x%x))\n",
				pipe_name(crtc->pipe), pll->state.pipe_mask);
	}
}

static void
intel_modeset_verify_crtc(struct intel_crtc *crtc,
			  struct intel_atomic_state *state,
			  struct intel_crtc_state *old_crtc_state,
			  struct intel_crtc_state *new_crtc_state)
{
	if (!intel_crtc_needs_modeset(new_crtc_state) && !new_crtc_state->update_pipe)
		return;

	verify_wm_state(crtc, new_crtc_state);
	verify_connector_state(state, crtc);
	verify_crtc_state(crtc, old_crtc_state, new_crtc_state);
	verify_shared_dpll_state(crtc, old_crtc_state, new_crtc_state);
}

static void
verify_disabled_dpll_state(struct drm_i915_private *dev_priv)
{
	int i;

	for (i = 0; i < dev_priv->dpll.num_shared_dpll; i++)
		verify_single_dpll_state(dev_priv,
					 &dev_priv->dpll.shared_dplls[i],
					 NULL, NULL);
}

static void
intel_modeset_verify_disabled(struct drm_i915_private *dev_priv,
			      struct intel_atomic_state *state)
{
	verify_encoder_state(dev_priv, state);
	verify_connector_state(state, NULL);
	verify_disabled_dpll_state(dev_priv);
}

int intel_modeset_all_pipes(struct intel_atomic_state *state)
{
	struct drm_i915_private *dev_priv = to_i915(state->base.dev);
	struct intel_crtc *crtc;

	/*
	 * Add all pipes to the state, and force
	 * a modeset on all the active ones.
	 */
	for_each_intel_crtc(&dev_priv->drm, crtc) {
		struct intel_crtc_state *crtc_state;
		int ret;

		crtc_state = intel_atomic_get_crtc_state(&state->base, crtc);
		if (IS_ERR(crtc_state))
			return PTR_ERR(crtc_state);

		if (!crtc_state->hw.active ||
		    drm_atomic_crtc_needs_modeset(&crtc_state->uapi))
			continue;

		crtc_state->uapi.mode_changed = true;

		ret = drm_atomic_add_affected_connectors(&state->base,
							 &crtc->base);
		if (ret)
			return ret;

		ret = intel_atomic_add_affected_planes(state, crtc);
		if (ret)
			return ret;

		crtc_state->update_planes |= crtc_state->active_planes;
	}

	return 0;
}

static void
intel_crtc_update_active_timings(const struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	struct drm_display_mode adjusted_mode =
		crtc_state->hw.adjusted_mode;

	if (crtc_state->vrr.enable) {
		adjusted_mode.crtc_vtotal = crtc_state->vrr.vmax;
		adjusted_mode.crtc_vblank_end = crtc_state->vrr.vmax;
		adjusted_mode.crtc_vblank_start = intel_vrr_vmin_vblank_start(crtc_state);
		crtc->vmax_vblank_start = intel_vrr_vmax_vblank_start(crtc_state);
	}

	drm_calc_timestamping_constants(&crtc->base, &adjusted_mode);

	crtc->mode_flags = crtc_state->mode_flags;

	/*
	 * The scanline counter increments at the leading edge of hsync.
	 *
	 * On most platforms it starts counting from vtotal-1 on the
	 * first active line. That means the scanline counter value is
	 * always one less than what we would expect. Ie. just after
	 * start of vblank, which also occurs at start of hsync (on the
	 * last active line), the scanline counter will read vblank_start-1.
	 *
	 * On gen2 the scanline counter starts counting from 1 instead
	 * of vtotal-1, so we have to subtract one (or rather add vtotal-1
	 * to keep the value positive), instead of adding one.
	 *
	 * On HSW+ the behaviour of the scanline counter depends on the output
	 * type. For DP ports it behaves like most other platforms, but on HDMI
	 * there's an extra 1 line difference. So we need to add two instead of
	 * one to the value.
	 *
	 * On VLV/CHV DSI the scanline counter would appear to increment
	 * approx. 1/3 of a scanline before start of vblank. Unfortunately
	 * that means we can't tell whether we're in vblank or not while
	 * we're on that particular line. We must still set scanline_offset
	 * to 1 so that the vblank timestamps come out correct when we query
	 * the scanline counter from within the vblank interrupt handler.
	 * However if queried just before the start of vblank we'll get an
	 * answer that's slightly in the future.
	 */
	if (DISPLAY_VER(dev_priv) == 2) {
		int vtotal;

		vtotal = adjusted_mode.crtc_vtotal;
		if (adjusted_mode.flags & DRM_MODE_FLAG_INTERLACE)
			vtotal /= 2;

		crtc->scanline_offset = vtotal - 1;
	} else if (HAS_DDI(dev_priv) &&
		   intel_crtc_has_type(crtc_state, INTEL_OUTPUT_HDMI)) {
		crtc->scanline_offset = 2;
	} else {
		crtc->scanline_offset = 1;
	}
}

static void intel_modeset_clear_plls(struct intel_atomic_state *state)
{
	struct drm_i915_private *dev_priv = to_i915(state->base.dev);
	struct intel_crtc_state *new_crtc_state;
	struct intel_crtc *crtc;
	int i;

	if (!dev_priv->display.crtc_compute_clock)
		return;

	for_each_new_intel_crtc_in_state(state, crtc, new_crtc_state, i) {
		if (!intel_crtc_needs_modeset(new_crtc_state))
			continue;

		intel_release_shared_dplls(state, crtc);
	}
}

/*
 * This implements the workaround described in the "notes" section of the mode
 * set sequence documentation. When going from no pipes or single pipe to
 * multiple pipes, and planes are enabled after the pipe, we need to wait at
 * least 2 vblanks on the first pipe before enabling planes on the second pipe.
 */
static int hsw_mode_set_planes_workaround(struct intel_atomic_state *state)
{
	struct intel_crtc_state *crtc_state;
	struct intel_crtc *crtc;
	struct intel_crtc_state *first_crtc_state = NULL;
	struct intel_crtc_state *other_crtc_state = NULL;
	enum pipe first_pipe = INVALID_PIPE, enabled_pipe = INVALID_PIPE;
	int i;

	/* look at all crtc's that are going to be enabled in during modeset */
	for_each_new_intel_crtc_in_state(state, crtc, crtc_state, i) {
		if (!crtc_state->hw.active ||
		    !intel_crtc_needs_modeset(crtc_state))
			continue;

		if (first_crtc_state) {
			other_crtc_state = crtc_state;
			break;
		} else {
			first_crtc_state = crtc_state;
			first_pipe = crtc->pipe;
		}
	}

	/* No workaround needed? */
	if (!first_crtc_state)
		return 0;

	/* w/a possibly needed, check how many crtc's are already enabled. */
	for_each_intel_crtc(state->base.dev, crtc) {
		crtc_state = intel_atomic_get_crtc_state(&state->base, crtc);
		if (IS_ERR(crtc_state))
			return PTR_ERR(crtc_state);

		crtc_state->hsw_workaround_pipe = INVALID_PIPE;

		if (!crtc_state->hw.active ||
		    intel_crtc_needs_modeset(crtc_state))
			continue;

		/* 2 or more enabled crtcs means no need for w/a */
		if (enabled_pipe != INVALID_PIPE)
			return 0;

		enabled_pipe = crtc->pipe;
	}

	if (enabled_pipe != INVALID_PIPE)
		first_crtc_state->hsw_workaround_pipe = enabled_pipe;
	else if (other_crtc_state)
		other_crtc_state->hsw_workaround_pipe = first_pipe;

	return 0;
}

u8 intel_calc_active_pipes(struct intel_atomic_state *state,
			   u8 active_pipes)
{
	const struct intel_crtc_state *crtc_state;
	struct intel_crtc *crtc;
	int i;

	for_each_new_intel_crtc_in_state(state, crtc, crtc_state, i) {
		if (crtc_state->hw.active)
			active_pipes |= BIT(crtc->pipe);
		else
			active_pipes &= ~BIT(crtc->pipe);
	}

	return active_pipes;
}

static int intel_modeset_checks(struct intel_atomic_state *state)
{
	struct drm_i915_private *dev_priv = to_i915(state->base.dev);

	state->modeset = true;

	if (IS_HASWELL(dev_priv))
		return hsw_mode_set_planes_workaround(state);

	return 0;
}

/*
 * Handle calculation of various watermark data at the end of the atomic check
 * phase.  The code here should be run after the per-crtc and per-plane 'check'
 * handlers to ensure that all derived state has been updated.
 */
static int calc_watermark_data(struct intel_atomic_state *state)
{
	struct drm_device *dev = state->base.dev;
	struct drm_i915_private *dev_priv = to_i915(dev);

	/* Is there platform-specific watermark information to calculate? */
	if (dev_priv->display.compute_global_watermarks)
		return dev_priv->display.compute_global_watermarks(state);

	return 0;
}

static void intel_crtc_check_fastset(const struct intel_crtc_state *old_crtc_state,
				     struct intel_crtc_state *new_crtc_state)
{
	if (!intel_pipe_config_compare(old_crtc_state, new_crtc_state, true))
		return;

	new_crtc_state->uapi.mode_changed = false;
	new_crtc_state->update_pipe = true;
}

static void intel_crtc_copy_fastset(const struct intel_crtc_state *old_crtc_state,
				    struct intel_crtc_state *new_crtc_state)
{
	/*
	 * If we're not doing the full modeset we want to
	 * keep the current M/N values as they may be
	 * sufficiently different to the computed values
	 * to cause problems.
	 *
	 * FIXME: should really copy more fuzzy state here
	 */
	new_crtc_state->fdi_m_n = old_crtc_state->fdi_m_n;
	new_crtc_state->dp_m_n = old_crtc_state->dp_m_n;
	new_crtc_state->dp_m2_n2 = old_crtc_state->dp_m2_n2;
	new_crtc_state->has_drrs = old_crtc_state->has_drrs;
}

static int intel_crtc_add_planes_to_state(struct intel_atomic_state *state,
					  struct intel_crtc *crtc,
					  u8 plane_ids_mask)
{
	struct drm_i915_private *dev_priv = to_i915(state->base.dev);
	struct intel_plane *plane;

	for_each_intel_plane_on_crtc(&dev_priv->drm, crtc, plane) {
		struct intel_plane_state *plane_state;

		if ((plane_ids_mask & BIT(plane->id)) == 0)
			continue;

		plane_state = intel_atomic_get_plane_state(state, plane);
		if (IS_ERR(plane_state))
			return PTR_ERR(plane_state);
	}

	return 0;
}

int intel_atomic_add_affected_planes(struct intel_atomic_state *state,
				     struct intel_crtc *crtc)
{
	const struct intel_crtc_state *old_crtc_state =
		intel_atomic_get_old_crtc_state(state, crtc);
	const struct intel_crtc_state *new_crtc_state =
		intel_atomic_get_new_crtc_state(state, crtc);

	return intel_crtc_add_planes_to_state(state, crtc,
					      old_crtc_state->enabled_planes |
					      new_crtc_state->enabled_planes);
}

static bool active_planes_affects_min_cdclk(struct drm_i915_private *dev_priv)
{
	/* See {hsw,vlv,ivb}_plane_ratio() */
	return IS_BROADWELL(dev_priv) || IS_HASWELL(dev_priv) ||
		IS_CHERRYVIEW(dev_priv) || IS_VALLEYVIEW(dev_priv) ||
		IS_IVYBRIDGE(dev_priv);
}

static int intel_crtc_add_bigjoiner_planes(struct intel_atomic_state *state,
					   struct intel_crtc *crtc,
					   struct intel_crtc *other)
{
	const struct intel_plane_state *plane_state;
	struct intel_plane *plane;
	u8 plane_ids = 0;
	int i;

	for_each_new_intel_plane_in_state(state, plane, plane_state, i) {
		if (plane->pipe == crtc->pipe)
			plane_ids |= BIT(plane->id);
	}

	return intel_crtc_add_planes_to_state(state, other, plane_ids);
}

static int intel_bigjoiner_add_affected_planes(struct intel_atomic_state *state)
{
	const struct intel_crtc_state *crtc_state;
	struct intel_crtc *crtc;
	int i;

	for_each_new_intel_crtc_in_state(state, crtc, crtc_state, i) {
		int ret;

		if (!crtc_state->bigjoiner)
			continue;

		ret = intel_crtc_add_bigjoiner_planes(state, crtc,
						      crtc_state->bigjoiner_linked_crtc);
		if (ret)
			return ret;
	}

	return 0;
}

static int intel_atomic_check_planes(struct intel_atomic_state *state)
{
	struct drm_i915_private *dev_priv = to_i915(state->base.dev);
	struct intel_crtc_state *old_crtc_state, *new_crtc_state;
	struct intel_plane_state *plane_state;
	struct intel_plane *plane;
	struct intel_crtc *crtc;
	int i, ret;

	ret = icl_add_linked_planes(state);
	if (ret)
		return ret;

	ret = intel_bigjoiner_add_affected_planes(state);
	if (ret)
		return ret;

	for_each_new_intel_plane_in_state(state, plane, plane_state, i) {
		ret = intel_plane_atomic_check(state, plane);
		if (ret) {
			drm_dbg_atomic(&dev_priv->drm,
				       "[PLANE:%d:%s] atomic driver check failed\n",
				       plane->base.base.id, plane->base.name);
			return ret;
		}
	}

	for_each_oldnew_intel_crtc_in_state(state, crtc, old_crtc_state,
					    new_crtc_state, i) {
		u8 old_active_planes, new_active_planes;

		ret = icl_check_nv12_planes(new_crtc_state);
		if (ret)
			return ret;

		/*
		 * On some platforms the number of active planes affects
		 * the planes' minimum cdclk calculation. Add such planes
		 * to the state before we compute the minimum cdclk.
		 */
		if (!active_planes_affects_min_cdclk(dev_priv))
			continue;

		old_active_planes = old_crtc_state->active_planes & ~BIT(PLANE_CURSOR);
		new_active_planes = new_crtc_state->active_planes & ~BIT(PLANE_CURSOR);

		if (hweight8(old_active_planes) == hweight8(new_active_planes))
			continue;

		ret = intel_crtc_add_planes_to_state(state, crtc, new_active_planes);
		if (ret)
			return ret;
	}

	return 0;
}

static int intel_atomic_check_cdclk(struct intel_atomic_state *state,
				    bool *need_cdclk_calc)
{
	struct drm_i915_private *dev_priv = to_i915(state->base.dev);
	const struct intel_cdclk_state *old_cdclk_state;
	const struct intel_cdclk_state *new_cdclk_state;
	struct intel_plane_state *plane_state;
	struct intel_bw_state *new_bw_state;
	struct intel_plane *plane;
	int min_cdclk = 0;
	enum pipe pipe;
	int ret;
	int i;
	/*
	 * active_planes bitmask has been updated, and potentially
	 * affected planes are part of the state. We can now
	 * compute the minimum cdclk for each plane.
	 */
	for_each_new_intel_plane_in_state(state, plane, plane_state, i) {
		ret = intel_plane_calc_min_cdclk(state, plane, need_cdclk_calc);
		if (ret)
			return ret;
	}

	old_cdclk_state = intel_atomic_get_old_cdclk_state(state);
	new_cdclk_state = intel_atomic_get_new_cdclk_state(state);

	if (new_cdclk_state &&
	    old_cdclk_state->force_min_cdclk != new_cdclk_state->force_min_cdclk)
		*need_cdclk_calc = true;

	ret = dev_priv->display.bw_calc_min_cdclk(state);
	if (ret)
		return ret;

	new_bw_state = intel_atomic_get_new_bw_state(state);

	if (!new_cdclk_state || !new_bw_state)
		return 0;

	for_each_pipe(dev_priv, pipe) {
		min_cdclk = max(new_cdclk_state->min_cdclk[pipe], min_cdclk);

		/*
		 * Currently do this change only if we need to increase
		 */
		if (new_bw_state->min_cdclk > min_cdclk)
			*need_cdclk_calc = true;
	}

	return 0;
}

static int intel_atomic_check_crtcs(struct intel_atomic_state *state)
{
	struct intel_crtc_state *crtc_state;
	struct intel_crtc *crtc;
	int i;

	for_each_new_intel_crtc_in_state(state, crtc, crtc_state, i) {
		struct drm_i915_private *i915 = to_i915(crtc->base.dev);
		int ret;

		ret = intel_crtc_atomic_check(state, crtc);
		if (ret) {
			drm_dbg_atomic(&i915->drm,
				       "[CRTC:%d:%s] atomic driver check failed\n",
				       crtc->base.base.id, crtc->base.name);
			return ret;
		}
	}

	return 0;
}

static bool intel_cpu_transcoders_need_modeset(struct intel_atomic_state *state,
					       u8 transcoders)
{
	const struct intel_crtc_state *new_crtc_state;
	struct intel_crtc *crtc;
	int i;

	for_each_new_intel_crtc_in_state(state, crtc, new_crtc_state, i) {
		if (new_crtc_state->hw.enable &&
		    transcoders & BIT(new_crtc_state->cpu_transcoder) &&
		    intel_crtc_needs_modeset(new_crtc_state))
			return true;
	}

	return false;
}

static int intel_atomic_check_bigjoiner(struct intel_atomic_state *state,
					struct intel_crtc *crtc,
					struct intel_crtc_state *old_crtc_state,
					struct intel_crtc_state *new_crtc_state)
{
	struct intel_crtc_state *slave_crtc_state, *master_crtc_state;
	struct intel_crtc *slave, *master;

	/* slave being enabled, is master is still claiming this crtc? */
	if (old_crtc_state->bigjoiner_slave) {
		slave = crtc;
		master = old_crtc_state->bigjoiner_linked_crtc;
		master_crtc_state = intel_atomic_get_new_crtc_state(state, master);
		if (!master_crtc_state || !intel_crtc_needs_modeset(master_crtc_state))
			goto claimed;
	}

	if (!new_crtc_state->bigjoiner)
		return 0;

	slave = intel_dsc_get_bigjoiner_secondary(crtc);
	if (!slave) {
		DRM_DEBUG_KMS("[CRTC:%d:%s] Big joiner configuration requires "
			      "CRTC + 1 to be used, doesn't exist\n",
			      crtc->base.base.id, crtc->base.name);
		return -EINVAL;
	}

	new_crtc_state->bigjoiner_linked_crtc = slave;
	slave_crtc_state = intel_atomic_get_crtc_state(&state->base, slave);
	master = crtc;
	if (IS_ERR(slave_crtc_state))
		return PTR_ERR(slave_crtc_state);

	/* master being enabled, slave was already configured? */
	if (slave_crtc_state->uapi.enable)
		goto claimed;

	DRM_DEBUG_KMS("[CRTC:%d:%s] Used as slave for big joiner\n",
		      slave->base.base.id, slave->base.name);

	return copy_bigjoiner_crtc_state(slave_crtc_state, new_crtc_state);

claimed:
	DRM_DEBUG_KMS("[CRTC:%d:%s] Slave is enabled as normal CRTC, but "
		      "[CRTC:%d:%s] claiming this CRTC for bigjoiner.\n",
		      slave->base.base.id, slave->base.name,
		      master->base.base.id, master->base.name);
	return -EINVAL;
}

static void kill_bigjoiner_slave(struct intel_atomic_state *state,
				 struct intel_crtc_state *master_crtc_state)
{
	struct intel_crtc_state *slave_crtc_state =
		intel_atomic_get_new_crtc_state(state, master_crtc_state->bigjoiner_linked_crtc);

	slave_crtc_state->bigjoiner = master_crtc_state->bigjoiner = false;
	slave_crtc_state->bigjoiner_slave = master_crtc_state->bigjoiner_slave = false;
	slave_crtc_state->bigjoiner_linked_crtc = master_crtc_state->bigjoiner_linked_crtc = NULL;
	intel_crtc_copy_uapi_to_hw_state(state, slave_crtc_state);
}

/**
 * DOC: asynchronous flip implementation
 *
 * Asynchronous page flip is the implementation for the DRM_MODE_PAGE_FLIP_ASYNC
 * flag. Currently async flip is only supported via the drmModePageFlip IOCTL.
 * Correspondingly, support is currently added for primary plane only.
 *
 * Async flip can only change the plane surface address, so anything else
 * changing is rejected from the intel_atomic_check_async() function.
 * Once this check is cleared, flip done interrupt is enabled using
 * the intel_crtc_enable_flip_done() function.
 *
 * As soon as the surface address register is written, flip done interrupt is
 * generated and the requested events are sent to the usersapce in the interrupt
 * handler itself. The timestamp and sequence sent during the flip done event
 * correspond to the last vblank and have no relation to the actual time when
 * the flip done event was sent.
 */
static int intel_atomic_check_async(struct intel_atomic_state *state)
{
	struct drm_i915_private *i915 = to_i915(state->base.dev);
	const struct intel_crtc_state *old_crtc_state, *new_crtc_state;
	const struct intel_plane_state *new_plane_state, *old_plane_state;
	struct intel_crtc *crtc;
	struct intel_plane *plane;
	int i;

	for_each_oldnew_intel_crtc_in_state(state, crtc, old_crtc_state,
					    new_crtc_state, i) {
		if (intel_crtc_needs_modeset(new_crtc_state)) {
			drm_dbg_kms(&i915->drm, "Modeset Required. Async flip not supported\n");
			return -EINVAL;
		}

		if (!new_crtc_state->hw.active) {
			drm_dbg_kms(&i915->drm, "CRTC inactive\n");
			return -EINVAL;
		}
		if (old_crtc_state->active_planes != new_crtc_state->active_planes) {
			drm_dbg_kms(&i915->drm,
				    "Active planes cannot be changed during async flip\n");
			return -EINVAL;
		}
	}

	for_each_oldnew_intel_plane_in_state(state, plane, old_plane_state,
					     new_plane_state, i) {
		/*
		 * TODO: Async flip is only supported through the page flip IOCTL
		 * as of now. So support currently added for primary plane only.
		 * Support for other planes on platforms on which supports
		 * this(vlv/chv and icl+) should be added when async flip is
		 * enabled in the atomic IOCTL path.
		 */
		if (!plane->async_flip)
			return -EINVAL;

		/*
		 * FIXME: This check is kept generic for all platforms.
		 * Need to verify this for all gen9 and gen10 platforms to enable
		 * this selectively if required.
		 */
		switch (new_plane_state->hw.fb->modifier) {
		case I915_FORMAT_MOD_X_TILED:
		case I915_FORMAT_MOD_Y_TILED:
		case I915_FORMAT_MOD_Yf_TILED:
			break;
		default:
			drm_dbg_kms(&i915->drm,
				    "Linear memory/CCS does not support async flips\n");
			return -EINVAL;
		}

		if (old_plane_state->view.color_plane[0].stride !=
		    new_plane_state->view.color_plane[0].stride) {
			drm_dbg_kms(&i915->drm, "Stride cannot be changed in async flip\n");
			return -EINVAL;
		}

		if (old_plane_state->hw.fb->modifier !=
		    new_plane_state->hw.fb->modifier) {
			drm_dbg_kms(&i915->drm,
				    "Framebuffer modifiers cannot be changed in async flip\n");
			return -EINVAL;
		}

		if (old_plane_state->hw.fb->format !=
		    new_plane_state->hw.fb->format) {
			drm_dbg_kms(&i915->drm,
				    "Framebuffer format cannot be changed in async flip\n");
			return -EINVAL;
		}

		if (old_plane_state->hw.rotation !=
		    new_plane_state->hw.rotation) {
			drm_dbg_kms(&i915->drm, "Rotation cannot be changed in async flip\n");
			return -EINVAL;
		}

		if (!drm_rect_equals(&old_plane_state->uapi.src, &new_plane_state->uapi.src) ||
		    !drm_rect_equals(&old_plane_state->uapi.dst, &new_plane_state->uapi.dst)) {
			drm_dbg_kms(&i915->drm,
				    "Plane size/co-ordinates cannot be changed in async flip\n");
			return -EINVAL;
		}

		if (old_plane_state->hw.alpha != new_plane_state->hw.alpha) {
			drm_dbg_kms(&i915->drm, "Alpha value cannot be changed in async flip\n");
			return -EINVAL;
		}

		if (old_plane_state->hw.pixel_blend_mode !=
		    new_plane_state->hw.pixel_blend_mode) {
			drm_dbg_kms(&i915->drm,
				    "Pixel blend mode cannot be changed in async flip\n");
			return -EINVAL;
		}

		if (old_plane_state->hw.color_encoding != new_plane_state->hw.color_encoding) {
			drm_dbg_kms(&i915->drm,
				    "Color encoding cannot be changed in async flip\n");
			return -EINVAL;
		}

		if (old_plane_state->hw.color_range != new_plane_state->hw.color_range) {
			drm_dbg_kms(&i915->drm, "Color range cannot be changed in async flip\n");
			return -EINVAL;
		}
	}

	return 0;
}

static int intel_bigjoiner_add_affected_crtcs(struct intel_atomic_state *state)
{
	struct intel_crtc_state *crtc_state;
	struct intel_crtc *crtc;
	int i;

	for_each_new_intel_crtc_in_state(state, crtc, crtc_state, i) {
		struct intel_crtc_state *linked_crtc_state;
		struct intel_crtc *linked_crtc;
		int ret;

		if (!crtc_state->bigjoiner)
			continue;

		linked_crtc = crtc_state->bigjoiner_linked_crtc;
		linked_crtc_state = intel_atomic_get_crtc_state(&state->base, linked_crtc);
		if (IS_ERR(linked_crtc_state))
			return PTR_ERR(linked_crtc_state);

		if (!intel_crtc_needs_modeset(crtc_state))
			continue;

		linked_crtc_state->uapi.mode_changed = true;

		ret = drm_atomic_add_affected_connectors(&state->base,
							 &linked_crtc->base);
		if (ret)
			return ret;

		ret = intel_atomic_add_affected_planes(state, linked_crtc);
		if (ret)
			return ret;
	}

	for_each_new_intel_crtc_in_state(state, crtc, crtc_state, i) {
		/* Kill old bigjoiner link, we may re-establish afterwards */
		if (intel_crtc_needs_modeset(crtc_state) &&
		    crtc_state->bigjoiner && !crtc_state->bigjoiner_slave)
			kill_bigjoiner_slave(state, crtc_state);
	}

	return 0;
}

/**
 * intel_atomic_check - validate state object
 * @dev: drm device
 * @_state: state to validate
 */
static int intel_atomic_check(struct drm_device *dev,
			      struct drm_atomic_state *_state)
{
	struct drm_i915_private *dev_priv = to_i915(dev);
	struct intel_atomic_state *state = to_intel_atomic_state(_state);
	struct intel_crtc_state *old_crtc_state, *new_crtc_state;
	struct intel_crtc *crtc;
	int ret, i;
	bool any_ms = false;

	for_each_oldnew_intel_crtc_in_state(state, crtc, old_crtc_state,
					    new_crtc_state, i) {
		if (new_crtc_state->inherited != old_crtc_state->inherited)
			new_crtc_state->uapi.mode_changed = true;
	}

	intel_vrr_check_modeset(state);

	ret = drm_atomic_helper_check_modeset(dev, &state->base);
	if (ret)
		goto fail;

	ret = intel_bigjoiner_add_affected_crtcs(state);
	if (ret)
		goto fail;

	for_each_oldnew_intel_crtc_in_state(state, crtc, old_crtc_state,
					    new_crtc_state, i) {
		if (!intel_crtc_needs_modeset(new_crtc_state)) {
			/* Light copy */
			intel_crtc_copy_uapi_to_hw_state_nomodeset(state, new_crtc_state);

			continue;
		}

		if (!new_crtc_state->uapi.enable) {
			if (!new_crtc_state->bigjoiner_slave) {
				intel_crtc_copy_uapi_to_hw_state(state, new_crtc_state);
				any_ms = true;
			}
			continue;
		}

		ret = intel_crtc_prepare_cleared_state(state, new_crtc_state);
		if (ret)
			goto fail;

		ret = intel_modeset_pipe_config(state, new_crtc_state);
		if (ret)
			goto fail;

		ret = intel_atomic_check_bigjoiner(state, crtc, old_crtc_state,
						   new_crtc_state);
		if (ret)
			goto fail;
	}

	for_each_oldnew_intel_crtc_in_state(state, crtc, old_crtc_state,
					    new_crtc_state, i) {
		if (!intel_crtc_needs_modeset(new_crtc_state))
			continue;

		ret = intel_modeset_pipe_config_late(new_crtc_state);
		if (ret)
			goto fail;

		intel_crtc_check_fastset(old_crtc_state, new_crtc_state);
	}

	/**
	 * Check if fastset is allowed by external dependencies like other
	 * pipes and transcoders.
	 *
	 * Right now it only forces a fullmodeset when the MST master
	 * transcoder did not changed but the pipe of the master transcoder
	 * needs a fullmodeset so all slaves also needs to do a fullmodeset or
	 * in case of port synced crtcs, if one of the synced crtcs
	 * needs a full modeset, all other synced crtcs should be
	 * forced a full modeset.
	 */
	for_each_new_intel_crtc_in_state(state, crtc, new_crtc_state, i) {
		if (!new_crtc_state->hw.enable || intel_crtc_needs_modeset(new_crtc_state))
			continue;

		if (intel_dp_mst_is_slave_trans(new_crtc_state)) {
			enum transcoder master = new_crtc_state->mst_master_transcoder;

			if (intel_cpu_transcoders_need_modeset(state, BIT(master))) {
				new_crtc_state->uapi.mode_changed = true;
				new_crtc_state->update_pipe = false;
			}
		}

		if (is_trans_port_sync_mode(new_crtc_state)) {
			u8 trans = new_crtc_state->sync_mode_slaves_mask;

			if (new_crtc_state->master_transcoder != INVALID_TRANSCODER)
				trans |= BIT(new_crtc_state->master_transcoder);

			if (intel_cpu_transcoders_need_modeset(state, trans)) {
				new_crtc_state->uapi.mode_changed = true;
				new_crtc_state->update_pipe = false;
			}
		}

		if (new_crtc_state->bigjoiner) {
			struct intel_crtc_state *linked_crtc_state =
				intel_atomic_get_new_crtc_state(state, new_crtc_state->bigjoiner_linked_crtc);

			if (intel_crtc_needs_modeset(linked_crtc_state)) {
				new_crtc_state->uapi.mode_changed = true;
				new_crtc_state->update_pipe = false;
			}
		}
	}

	for_each_oldnew_intel_crtc_in_state(state, crtc, old_crtc_state,
					    new_crtc_state, i) {
		if (intel_crtc_needs_modeset(new_crtc_state)) {
			any_ms = true;
			continue;
		}

		if (!new_crtc_state->update_pipe)
			continue;

		intel_crtc_copy_fastset(old_crtc_state, new_crtc_state);
	}

	if (any_ms && !check_digital_port_conflicts(state)) {
		drm_dbg_kms(&dev_priv->drm,
			    "rejecting conflicting digital port configuration\n");
		ret = -EINVAL;
		goto fail;
	}

	ret = drm_dp_mst_atomic_check(&state->base);
	if (ret)
		goto fail;

	ret = intel_atomic_check_planes(state);
	if (ret)
		goto fail;

	intel_fbc_choose_crtc(dev_priv, state);
	ret = calc_watermark_data(state);
	if (ret)
		goto fail;

	ret = intel_bw_atomic_check(state);
	if (ret)
		goto fail;

	ret = intel_atomic_check_cdclk(state, &any_ms);
	if (ret)
		goto fail;

	if (intel_any_crtc_needs_modeset(state))
		any_ms = true;

	if (any_ms) {
		ret = intel_modeset_checks(state);
		if (ret)
			goto fail;

		ret = intel_modeset_calc_cdclk(state);
		if (ret)
			return ret;

		intel_modeset_clear_plls(state);
	}

	ret = intel_atomic_check_crtcs(state);
	if (ret)
		goto fail;

	for_each_oldnew_intel_crtc_in_state(state, crtc, old_crtc_state,
					    new_crtc_state, i) {
		if (new_crtc_state->uapi.async_flip) {
			ret = intel_atomic_check_async(state);
			if (ret)
				goto fail;
		}

		if (!intel_crtc_needs_modeset(new_crtc_state) &&
		    !new_crtc_state->update_pipe)
			continue;

		intel_dump_pipe_config(new_crtc_state, state,
				       intel_crtc_needs_modeset(new_crtc_state) ?
				       "[modeset]" : "[fastset]");
	}

	return 0;

 fail:
	if (ret == -EDEADLK)
		return ret;

	/*
	 * FIXME would probably be nice to know which crtc specifically
	 * caused the failure, in cases where we can pinpoint it.
	 */
	for_each_oldnew_intel_crtc_in_state(state, crtc, old_crtc_state,
					    new_crtc_state, i)
		intel_dump_pipe_config(new_crtc_state, state, "[failed]");

	return ret;
}

static int intel_atomic_prepare_commit(struct intel_atomic_state *state)
{
	struct intel_crtc_state *crtc_state;
	struct intel_crtc *crtc;
	int i, ret;

	ret = drm_atomic_helper_prepare_planes(state->base.dev, &state->base);
	if (ret < 0)
		return ret;

	for_each_new_intel_crtc_in_state(state, crtc, crtc_state, i) {
		bool mode_changed = intel_crtc_needs_modeset(crtc_state);

		if (mode_changed || crtc_state->update_pipe ||
		    crtc_state->uapi.color_mgmt_changed) {
			intel_dsb_prepare(crtc_state);
		}
	}

	return 0;
}

void intel_crtc_arm_fifo_underrun(struct intel_crtc *crtc,
				  struct intel_crtc_state *crtc_state)
{
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);

	if (DISPLAY_VER(dev_priv) != 2 || crtc_state->active_planes)
		intel_set_cpu_fifo_underrun_reporting(dev_priv, crtc->pipe, true);

	if (crtc_state->has_pch_encoder) {
		enum pipe pch_transcoder =
			intel_crtc_pch_transcoder(crtc);

		intel_set_pch_fifo_underrun_reporting(dev_priv, pch_transcoder, true);
	}
}

static void intel_pipe_fastset(const struct intel_crtc_state *old_crtc_state,
			       const struct intel_crtc_state *new_crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(new_crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);

	/*
	 * Update pipe size and adjust fitter if needed: the reason for this is
	 * that in compute_mode_changes we check the native mode (not the pfit
	 * mode) to see if we can flip rather than do a full mode set. In the
	 * fastboot case, we'll flip, but if we don't update the pipesrc and
	 * pfit state, we'll end up with a big fb scanned out into the wrong
	 * sized surface.
	 */
	intel_set_pipe_src_size(new_crtc_state);

	/* on skylake this is done by detaching scalers */
	if (DISPLAY_VER(dev_priv) >= 9) {
		if (new_crtc_state->pch_pfit.enabled)
			skl_pfit_enable(new_crtc_state);
	} else if (HAS_PCH_SPLIT(dev_priv)) {
		if (new_crtc_state->pch_pfit.enabled)
			ilk_pfit_enable(new_crtc_state);
		else if (old_crtc_state->pch_pfit.enabled)
			ilk_pfit_disable(old_crtc_state);
	}

	/*
	 * The register is supposedly single buffered so perhaps
	 * not 100% correct to do this here. But SKL+ calculate
	 * this based on the adjust pixel rate so pfit changes do
	 * affect it and so it must be updated for fastsets.
	 * HSW/BDW only really need this here for fastboot, after
	 * that the value should not change without a full modeset.
	 */
	if (DISPLAY_VER(dev_priv) >= 9 ||
	    IS_BROADWELL(dev_priv) || IS_HASWELL(dev_priv))
		hsw_set_linetime_wm(new_crtc_state);

	if (DISPLAY_VER(dev_priv) >= 11)
		icl_set_pipe_chicken(crtc);
}

static void commit_pipe_pre_planes(struct intel_atomic_state *state,
				   struct intel_crtc *crtc)
{
	struct drm_i915_private *dev_priv = to_i915(state->base.dev);
	const struct intel_crtc_state *old_crtc_state =
		intel_atomic_get_old_crtc_state(state, crtc);
	const struct intel_crtc_state *new_crtc_state =
		intel_atomic_get_new_crtc_state(state, crtc);
	bool modeset = intel_crtc_needs_modeset(new_crtc_state);

	/*
	 * During modesets pipe configuration was programmed as the
	 * CRTC was enabled.
	 */
	if (!modeset) {
		if (new_crtc_state->uapi.color_mgmt_changed ||
		    new_crtc_state->update_pipe)
			intel_color_commit(new_crtc_state);

		if (DISPLAY_VER(dev_priv) >= 9 || IS_BROADWELL(dev_priv))
			bdw_set_pipemisc(new_crtc_state);

		if (new_crtc_state->update_pipe)
			intel_pipe_fastset(old_crtc_state, new_crtc_state);

		intel_psr2_program_trans_man_trk_ctl(new_crtc_state);
	}

	if (dev_priv->display.atomic_update_watermarks)
		dev_priv->display.atomic_update_watermarks(state, crtc);
}

static void commit_pipe_post_planes(struct intel_atomic_state *state,
				    struct intel_crtc *crtc)
{
	struct drm_i915_private *dev_priv = to_i915(state->base.dev);
	const struct intel_crtc_state *new_crtc_state =
		intel_atomic_get_new_crtc_state(state, crtc);

	/*
	 * Disable the scaler(s) after the plane(s) so that we don't
	 * get a catastrophic underrun even if the two operations
	 * end up happening in two different frames.
	 */
	if (DISPLAY_VER(dev_priv) >= 9 &&
	    !intel_crtc_needs_modeset(new_crtc_state))
		skl_detach_scalers(new_crtc_state);
}

static void intel_enable_crtc(struct intel_atomic_state *state,
			      struct intel_crtc *crtc)
{
	struct drm_i915_private *dev_priv = to_i915(state->base.dev);
	const struct intel_crtc_state *new_crtc_state =
		intel_atomic_get_new_crtc_state(state, crtc);

	if (!intel_crtc_needs_modeset(new_crtc_state))
		return;

	intel_crtc_update_active_timings(new_crtc_state);

	dev_priv->display.crtc_enable(state, crtc);

	if (new_crtc_state->bigjoiner_slave)
		return;

	/* vblanks work again, re-enable pipe CRC. */
	intel_crtc_enable_pipe_crc(crtc);
}

static void intel_update_crtc(struct intel_atomic_state *state,
			      struct intel_crtc *crtc)
{
	struct drm_i915_private *dev_priv = to_i915(state->base.dev);
	const struct intel_crtc_state *old_crtc_state =
		intel_atomic_get_old_crtc_state(state, crtc);
	struct intel_crtc_state *new_crtc_state =
		intel_atomic_get_new_crtc_state(state, crtc);
	bool modeset = intel_crtc_needs_modeset(new_crtc_state);

	if (!modeset) {
		if (new_crtc_state->preload_luts &&
		    (new_crtc_state->uapi.color_mgmt_changed ||
		     new_crtc_state->update_pipe))
			intel_color_load_luts(new_crtc_state);

		intel_pre_plane_update(state, crtc);

		if (new_crtc_state->update_pipe)
			intel_encoders_update_pipe(state, crtc);
	}

	if (new_crtc_state->update_pipe && !new_crtc_state->enable_fbc)
		intel_fbc_disable(crtc);
	else
		intel_fbc_enable(state, crtc);

	/* Perform vblank evasion around commit operation */
	intel_pipe_update_start(new_crtc_state);

	commit_pipe_pre_planes(state, crtc);

	if (DISPLAY_VER(dev_priv) >= 9)
		skl_update_planes_on_crtc(state, crtc);
	else
		i9xx_update_planes_on_crtc(state, crtc);

	commit_pipe_post_planes(state, crtc);

	intel_pipe_update_end(new_crtc_state);

	/*
	 * We usually enable FIFO underrun interrupts as part of the
	 * CRTC enable sequence during modesets.  But when we inherit a
	 * valid pipe configuration from the BIOS we need to take care
	 * of enabling them on the CRTC's first fastset.
	 */
	if (new_crtc_state->update_pipe && !modeset &&
	    old_crtc_state->inherited)
		intel_crtc_arm_fifo_underrun(crtc, new_crtc_state);
}

static void intel_old_crtc_state_disables(struct intel_atomic_state *state,
					  struct intel_crtc_state *old_crtc_state,
					  struct intel_crtc_state *new_crtc_state,
					  struct intel_crtc *crtc)
{
	struct drm_i915_private *dev_priv = to_i915(state->base.dev);

	drm_WARN_ON(&dev_priv->drm, old_crtc_state->bigjoiner_slave);

	intel_crtc_disable_planes(state, crtc);

	/*
	 * We still need special handling for disabling bigjoiner master
	 * and slaves since for slave we do not have encoder or plls
	 * so we dont need to disable those.
	 */
	if (old_crtc_state->bigjoiner) {
		intel_crtc_disable_planes(state,
					  old_crtc_state->bigjoiner_linked_crtc);
		old_crtc_state->bigjoiner_linked_crtc->active = false;
	}

	/*
	 * We need to disable pipe CRC before disabling the pipe,
	 * or we race against vblank off.
	 */
	intel_crtc_disable_pipe_crc(crtc);

	dev_priv->display.crtc_disable(state, crtc);
	crtc->active = false;
	intel_fbc_disable(crtc);
	intel_disable_shared_dpll(old_crtc_state);

	/* FIXME unify this for all platforms */
	if (!new_crtc_state->hw.active &&
	    !HAS_GMCH(dev_priv) &&
	    dev_priv->display.initial_watermarks)
		dev_priv->display.initial_watermarks(state, crtc);
}

static void intel_commit_modeset_disables(struct intel_atomic_state *state)
{
	struct intel_crtc_state *new_crtc_state, *old_crtc_state;
	struct intel_crtc *crtc;
	u32 handled = 0;
	int i;

	/* Only disable port sync and MST slaves */
	for_each_oldnew_intel_crtc_in_state(state, crtc, old_crtc_state,
					    new_crtc_state, i) {
		if (!intel_crtc_needs_modeset(new_crtc_state) || old_crtc_state->bigjoiner)
			continue;

		if (!old_crtc_state->hw.active)
			continue;

		/* In case of Transcoder port Sync master slave CRTCs can be
		 * assigned in any order and we need to make sure that
		 * slave CRTCs are disabled first and then master CRTC since
		 * Slave vblanks are masked till Master Vblanks.
		 */
		if (!is_trans_port_sync_slave(old_crtc_state) &&
		    !intel_dp_mst_is_slave_trans(old_crtc_state))
			continue;

		intel_pre_plane_update(state, crtc);
		intel_old_crtc_state_disables(state, old_crtc_state,
					      new_crtc_state, crtc);
		handled |= BIT(crtc->pipe);
	}

	/* Disable everything else left on */
	for_each_oldnew_intel_crtc_in_state(state, crtc, old_crtc_state,
					    new_crtc_state, i) {
		if (!intel_crtc_needs_modeset(new_crtc_state) ||
		    (handled & BIT(crtc->pipe)) ||
		    old_crtc_state->bigjoiner_slave)
			continue;

		intel_pre_plane_update(state, crtc);
		if (old_crtc_state->bigjoiner) {
			struct intel_crtc *slave =
				old_crtc_state->bigjoiner_linked_crtc;

			intel_pre_plane_update(state, slave);
		}

		if (old_crtc_state->hw.active)
			intel_old_crtc_state_disables(state, old_crtc_state,
						      new_crtc_state, crtc);
	}
}

static void intel_commit_modeset_enables(struct intel_atomic_state *state)
{
	struct intel_crtc_state *new_crtc_state;
	struct intel_crtc *crtc;
	int i;

	for_each_new_intel_crtc_in_state(state, crtc, new_crtc_state, i) {
		if (!new_crtc_state->hw.active)
			continue;

		intel_enable_crtc(state, crtc);
		intel_update_crtc(state, crtc);
	}
}

static void skl_commit_modeset_enables(struct intel_atomic_state *state)
{
	struct drm_i915_private *dev_priv = to_i915(state->base.dev);
	struct intel_crtc *crtc;
	struct intel_crtc_state *old_crtc_state, *new_crtc_state;
	struct skl_ddb_entry entries[I915_MAX_PIPES] = {};
	u8 update_pipes = 0, modeset_pipes = 0;
	int i;

	for_each_oldnew_intel_crtc_in_state(state, crtc, old_crtc_state, new_crtc_state, i) {
		enum pipe pipe = crtc->pipe;

		if (!new_crtc_state->hw.active)
			continue;

		/* ignore allocations for crtc's that have been turned off. */
		if (!intel_crtc_needs_modeset(new_crtc_state)) {
			entries[pipe] = old_crtc_state->wm.skl.ddb;
			update_pipes |= BIT(pipe);
		} else {
			modeset_pipes |= BIT(pipe);
		}
	}

	/*
	 * Whenever the number of active pipes changes, we need to make sure we
	 * update the pipes in the right order so that their ddb allocations
	 * never overlap with each other between CRTC updates. Otherwise we'll
	 * cause pipe underruns and other bad stuff.
	 *
	 * So first lets enable all pipes that do not need a fullmodeset as
	 * those don't have any external dependency.
	 */
	while (update_pipes) {
		for_each_oldnew_intel_crtc_in_state(state, crtc, old_crtc_state,
						    new_crtc_state, i) {
			enum pipe pipe = crtc->pipe;

			if ((update_pipes & BIT(pipe)) == 0)
				continue;

			if (skl_ddb_allocation_overlaps(&new_crtc_state->wm.skl.ddb,
							entries, I915_MAX_PIPES, pipe))
				continue;

			entries[pipe] = new_crtc_state->wm.skl.ddb;
			update_pipes &= ~BIT(pipe);

			intel_update_crtc(state, crtc);

			/*
			 * If this is an already active pipe, it's DDB changed,
			 * and this isn't the last pipe that needs updating
			 * then we need to wait for a vblank to pass for the
			 * new ddb allocation to take effect.
			 */
			if (!skl_ddb_entry_equal(&new_crtc_state->wm.skl.ddb,
						 &old_crtc_state->wm.skl.ddb) &&
			    (update_pipes | modeset_pipes))
				intel_wait_for_vblank(dev_priv, pipe);
		}
	}

	update_pipes = modeset_pipes;

	/*
	 * Enable all pipes that needs a modeset and do not depends on other
	 * pipes
	 */
	for_each_new_intel_crtc_in_state(state, crtc, new_crtc_state, i) {
		enum pipe pipe = crtc->pipe;

		if ((modeset_pipes & BIT(pipe)) == 0)
			continue;

		if (intel_dp_mst_is_slave_trans(new_crtc_state) ||
		    is_trans_port_sync_master(new_crtc_state) ||
		    (new_crtc_state->bigjoiner && !new_crtc_state->bigjoiner_slave))
			continue;

		modeset_pipes &= ~BIT(pipe);

		intel_enable_crtc(state, crtc);
	}

	/*
	 * Then we enable all remaining pipes that depend on other
	 * pipes: MST slaves and port sync masters, big joiner master
	 */
	for_each_new_intel_crtc_in_state(state, crtc, new_crtc_state, i) {
		enum pipe pipe = crtc->pipe;

		if ((modeset_pipes & BIT(pipe)) == 0)
			continue;

		modeset_pipes &= ~BIT(pipe);

		intel_enable_crtc(state, crtc);
	}

	/*
	 * Finally we do the plane updates/etc. for all pipes that got enabled.
	 */
	for_each_new_intel_crtc_in_state(state, crtc, new_crtc_state, i) {
		enum pipe pipe = crtc->pipe;

		if ((update_pipes & BIT(pipe)) == 0)
			continue;

		drm_WARN_ON(&dev_priv->drm, skl_ddb_allocation_overlaps(&new_crtc_state->wm.skl.ddb,
									entries, I915_MAX_PIPES, pipe));

		entries[pipe] = new_crtc_state->wm.skl.ddb;
		update_pipes &= ~BIT(pipe);

		intel_update_crtc(state, crtc);
	}

	drm_WARN_ON(&dev_priv->drm, modeset_pipes);
	drm_WARN_ON(&dev_priv->drm, update_pipes);
}

static void intel_atomic_helper_free_state(struct drm_i915_private *dev_priv)
{
	struct intel_atomic_state *state, *next;
	struct llist_node *freed;

	freed = llist_del_all(&dev_priv->atomic_helper.free_list);
	llist_for_each_entry_safe(state, next, freed, freed)
		drm_atomic_state_put(&state->base);
}

static void intel_atomic_helper_free_state_worker(struct work_struct *work)
{
	struct drm_i915_private *dev_priv =
		container_of(work, typeof(*dev_priv), atomic_helper.free_work);

	intel_atomic_helper_free_state(dev_priv);
}

static void intel_atomic_commit_fence_wait(struct intel_atomic_state *intel_state)
{
	struct wait_queue_entry wait_fence, wait_reset;
	struct drm_i915_private *dev_priv = to_i915(intel_state->base.dev);

	init_wait_entry(&wait_fence, 0);
	init_wait_entry(&wait_reset, 0);
	for (;;) {
		prepare_to_wait(&intel_state->commit_ready.wait,
				&wait_fence, TASK_UNINTERRUPTIBLE);
		prepare_to_wait(bit_waitqueue(&dev_priv->gt.reset.flags,
					      I915_RESET_MODESET),
				&wait_reset, TASK_UNINTERRUPTIBLE);


		if (i915_sw_fence_done(&intel_state->commit_ready) ||
		    test_bit(I915_RESET_MODESET, &dev_priv->gt.reset.flags))
			break;

		schedule();
	}
	finish_wait(&intel_state->commit_ready.wait, &wait_fence);
	finish_wait(bit_waitqueue(&dev_priv->gt.reset.flags,
				  I915_RESET_MODESET),
		    &wait_reset);
}

static void intel_cleanup_dsbs(struct intel_atomic_state *state)
{
	struct intel_crtc_state *old_crtc_state, *new_crtc_state;
	struct intel_crtc *crtc;
	int i;

	for_each_oldnew_intel_crtc_in_state(state, crtc, old_crtc_state,
					    new_crtc_state, i)
		intel_dsb_cleanup(old_crtc_state);
}

static void intel_atomic_cleanup_work(struct work_struct *work)
{
	struct intel_atomic_state *state =
		container_of(work, struct intel_atomic_state, base.commit_work);
	struct drm_i915_private *i915 = to_i915(state->base.dev);

	intel_cleanup_dsbs(state);
	drm_atomic_helper_cleanup_planes(&i915->drm, &state->base);
	drm_atomic_helper_commit_cleanup_done(&state->base);
	drm_atomic_state_put(&state->base);

	intel_atomic_helper_free_state(i915);
}

static void intel_atomic_prepare_plane_clear_colors(struct intel_atomic_state *state)
{
	struct drm_i915_private *i915 = to_i915(state->base.dev);
	struct intel_plane *plane;
	struct intel_plane_state *plane_state;
	int i;

	for_each_new_intel_plane_in_state(state, plane, plane_state, i) {
		struct drm_framebuffer *fb = plane_state->hw.fb;
		int ret;

		if (!fb ||
		    fb->modifier != I915_FORMAT_MOD_Y_TILED_GEN12_RC_CCS_CC)
			continue;

		/*
		 * The layout of the fast clear color value expected by HW
		 * (the DRM ABI requiring this value to be located in fb at offset 0 of plane#2):
		 * - 4 x 4 bytes per-channel value
		 *   (in surface type specific float/int format provided by the fb user)
		 * - 8 bytes native color value used by the display
		 *   (converted/written by GPU during a fast clear operation using the
		 *    above per-channel values)
		 *
		 * The commit's FB prepare hook already ensured that FB obj is pinned and the
		 * caller made sure that the object is synced wrt. the related color clear value
		 * GPU write on it.
		 */
		ret = i915_gem_object_read_from_page(intel_fb_obj(fb),
						     fb->offsets[2] + 16,
						     &plane_state->ccval,
						     sizeof(plane_state->ccval));
		/* The above could only fail if the FB obj has an unexpected backing store type. */
		drm_WARN_ON(&i915->drm, ret);
	}
}

static void intel_atomic_commit_tail(struct intel_atomic_state *state)
{
	struct drm_device *dev = state->base.dev;
	struct drm_i915_private *dev_priv = to_i915(dev);
	struct intel_crtc_state *new_crtc_state, *old_crtc_state;
	struct intel_crtc *crtc;
	u64 put_domains[I915_MAX_PIPES] = {};
	intel_wakeref_t wakeref = 0;
	int i;

	intel_atomic_commit_fence_wait(state);

	drm_atomic_helper_wait_for_dependencies(&state->base);

	if (state->modeset)
		wakeref = intel_display_power_get(dev_priv, POWER_DOMAIN_MODESET);

	intel_atomic_prepare_plane_clear_colors(state);

	for_each_oldnew_intel_crtc_in_state(state, crtc, old_crtc_state,
					    new_crtc_state, i) {
		if (intel_crtc_needs_modeset(new_crtc_state) ||
		    new_crtc_state->update_pipe) {

			put_domains[crtc->pipe] =
				modeset_get_crtc_power_domains(new_crtc_state);
		}
	}

	intel_commit_modeset_disables(state);

	/* FIXME: Eventually get rid of our crtc->config pointer */
	for_each_new_intel_crtc_in_state(state, crtc, new_crtc_state, i)
		crtc->config = new_crtc_state;

	if (state->modeset) {
		drm_atomic_helper_update_legacy_modeset_state(dev, &state->base);

		intel_set_cdclk_pre_plane_update(state);

		intel_modeset_verify_disabled(dev_priv, state);
	}

	intel_sagv_pre_plane_update(state);

	/* Complete the events for pipes that have now been disabled */
	for_each_new_intel_crtc_in_state(state, crtc, new_crtc_state, i) {
		bool modeset = intel_crtc_needs_modeset(new_crtc_state);

		/* Complete events for now disable pipes here. */
		if (modeset && !new_crtc_state->hw.active && new_crtc_state->uapi.event) {
			spin_lock_irq(&dev->event_lock);
			drm_crtc_send_vblank_event(&crtc->base,
						   new_crtc_state->uapi.event);
			spin_unlock_irq(&dev->event_lock);

			new_crtc_state->uapi.event = NULL;
		}
	}

	if (state->modeset)
		intel_encoders_update_prepare(state);

	intel_dbuf_pre_plane_update(state);

	for_each_new_intel_crtc_in_state(state, crtc, new_crtc_state, i) {
		if (new_crtc_state->uapi.async_flip)
			intel_crtc_enable_flip_done(state, crtc);
	}

	/* Now enable the clocks, plane, pipe, and connectors that we set up. */
	dev_priv->display.commit_modeset_enables(state);

	if (state->modeset) {
		intel_encoders_update_complete(state);

		intel_set_cdclk_post_plane_update(state);
	}

	/* FIXME: We should call drm_atomic_helper_commit_hw_done() here
	 * already, but still need the state for the delayed optimization. To
	 * fix this:
	 * - wrap the optimization/post_plane_update stuff into a per-crtc work.
	 * - schedule that vblank worker _before_ calling hw_done
	 * - at the start of commit_tail, cancel it _synchrously
	 * - switch over to the vblank wait helper in the core after that since
	 *   we don't need out special handling any more.
	 */
	drm_atomic_helper_wait_for_flip_done(dev, &state->base);

	for_each_new_intel_crtc_in_state(state, crtc, new_crtc_state, i) {
		if (new_crtc_state->uapi.async_flip)
			intel_crtc_disable_flip_done(state, crtc);

		if (new_crtc_state->hw.active &&
		    !intel_crtc_needs_modeset(new_crtc_state) &&
		    !new_crtc_state->preload_luts &&
		    (new_crtc_state->uapi.color_mgmt_changed ||
		     new_crtc_state->update_pipe))
			intel_color_load_luts(new_crtc_state);
	}

	/*
	 * Now that the vblank has passed, we can go ahead and program the
	 * optimal watermarks on platforms that need two-step watermark
	 * programming.
	 *
	 * TODO: Move this (and other cleanup) to an async worker eventually.
	 */
	for_each_oldnew_intel_crtc_in_state(state, crtc, old_crtc_state,
					    new_crtc_state, i) {
		/*
		 * Gen2 reports pipe underruns whenever all planes are disabled.
		 * So re-enable underrun reporting after some planes get enabled.
		 *
		 * We do this before .optimize_watermarks() so that we have a
		 * chance of catching underruns with the intermediate watermarks
		 * vs. the new plane configuration.
		 */
		if (DISPLAY_VER(dev_priv) == 2 && planes_enabling(old_crtc_state, new_crtc_state))
			intel_set_cpu_fifo_underrun_reporting(dev_priv, crtc->pipe, true);

		if (dev_priv->display.optimize_watermarks)
			dev_priv->display.optimize_watermarks(state, crtc);
	}

	intel_dbuf_post_plane_update(state);

	for_each_oldnew_intel_crtc_in_state(state, crtc, old_crtc_state, new_crtc_state, i) {
		intel_post_plane_update(state, crtc);

		modeset_put_crtc_power_domains(crtc, put_domains[crtc->pipe]);

		intel_modeset_verify_crtc(crtc, state, old_crtc_state, new_crtc_state);

		/*
		 * DSB cleanup is done in cleanup_work aligning with framebuffer
		 * cleanup. So copy and reset the dsb structure to sync with
		 * commit_done and later do dsb cleanup in cleanup_work.
		 */
		old_crtc_state->dsb = fetch_and_zero(&new_crtc_state->dsb);
	}

	/* Underruns don't always raise interrupts, so check manually */
	intel_check_cpu_fifo_underruns(dev_priv);
	intel_check_pch_fifo_underruns(dev_priv);

	if (state->modeset)
		intel_verify_planes(state);

	intel_sagv_post_plane_update(state);

	drm_atomic_helper_commit_hw_done(&state->base);

	if (state->modeset) {
		/* As one of the primary mmio accessors, KMS has a high
		 * likelihood of triggering bugs in unclaimed access. After we
		 * finish modesetting, see if an error has been flagged, and if
		 * so enable debugging for the next modeset - and hope we catch
		 * the culprit.
		 */
		intel_uncore_arm_unclaimed_mmio_detection(&dev_priv->uncore);
		intel_display_power_put(dev_priv, POWER_DOMAIN_MODESET, wakeref);
	}
	intel_runtime_pm_put(&dev_priv->runtime_pm, state->wakeref);

	/*
	 * Defer the cleanup of the old state to a separate worker to not
	 * impede the current task (userspace for blocking modesets) that
	 * are executed inline. For out-of-line asynchronous modesets/flips,
	 * deferring to a new worker seems overkill, but we would place a
	 * schedule point (cond_resched()) here anyway to keep latencies
	 * down.
	 */
	INIT_WORK(&state->base.commit_work, intel_atomic_cleanup_work);
	queue_work(system_highpri_wq, &state->base.commit_work);
}

static void intel_atomic_commit_work(struct work_struct *work)
{
	struct intel_atomic_state *state =
		container_of(work, struct intel_atomic_state, base.commit_work);

	intel_atomic_commit_tail(state);
}

static int __i915_sw_fence_call
intel_atomic_commit_ready(struct i915_sw_fence *fence,
			  enum i915_sw_fence_notify notify)
{
	struct intel_atomic_state *state =
		container_of(fence, struct intel_atomic_state, commit_ready);

	switch (notify) {
	case FENCE_COMPLETE:
		/* we do blocking waits in the worker, nothing to do here */
		break;
	case FENCE_FREE:
		{
			struct intel_atomic_helper *helper =
				&to_i915(state->base.dev)->atomic_helper;

			if (llist_add(&state->freed, &helper->free_list))
				schedule_work(&helper->free_work);
			break;
		}
	}

	return NOTIFY_DONE;
}

static void intel_atomic_track_fbs(struct intel_atomic_state *state)
{
	struct intel_plane_state *old_plane_state, *new_plane_state;
	struct intel_plane *plane;
	int i;

	for_each_oldnew_intel_plane_in_state(state, plane, old_plane_state,
					     new_plane_state, i)
		intel_frontbuffer_track(to_intel_frontbuffer(old_plane_state->hw.fb),
					to_intel_frontbuffer(new_plane_state->hw.fb),
					plane->frontbuffer_bit);
}

static int intel_atomic_commit(struct drm_device *dev,
			       struct drm_atomic_state *_state,
			       bool nonblock)
{
	struct intel_atomic_state *state = to_intel_atomic_state(_state);
	struct drm_i915_private *dev_priv = to_i915(dev);
	int ret = 0;

	state->wakeref = intel_runtime_pm_get(&dev_priv->runtime_pm);

	drm_atomic_state_get(&state->base);
	i915_sw_fence_init(&state->commit_ready,
			   intel_atomic_commit_ready);

	/*
	 * The intel_legacy_cursor_update() fast path takes care
	 * of avoiding the vblank waits for simple cursor
	 * movement and flips. For cursor on/off and size changes,
	 * we want to perform the vblank waits so that watermark
	 * updates happen during the correct frames. Gen9+ have
	 * double buffered watermarks and so shouldn't need this.
	 *
	 * Unset state->legacy_cursor_update before the call to
	 * drm_atomic_helper_setup_commit() because otherwise
	 * drm_atomic_helper_wait_for_flip_done() is a noop and
	 * we get FIFO underruns because we didn't wait
	 * for vblank.
	 *
	 * FIXME doing watermarks and fb cleanup from a vblank worker
	 * (assuming we had any) would solve these problems.
	 */
	if (DISPLAY_VER(dev_priv) < 9 && state->base.legacy_cursor_update) {
		struct intel_crtc_state *new_crtc_state;
		struct intel_crtc *crtc;
		int i;

		for_each_new_intel_crtc_in_state(state, crtc, new_crtc_state, i)
			if (new_crtc_state->wm.need_postvbl_update ||
			    new_crtc_state->update_wm_post)
				state->base.legacy_cursor_update = false;
	}

	ret = intel_atomic_prepare_commit(state);
	if (ret) {
		drm_dbg_atomic(&dev_priv->drm,
			       "Preparing state failed with %i\n", ret);
		i915_sw_fence_commit(&state->commit_ready);
		intel_runtime_pm_put(&dev_priv->runtime_pm, state->wakeref);
		return ret;
	}

	ret = drm_atomic_helper_setup_commit(&state->base, nonblock);
	if (!ret)
		ret = drm_atomic_helper_swap_state(&state->base, true);
	if (!ret)
		intel_atomic_swap_global_state(state);

	if (ret) {
		struct intel_crtc_state *new_crtc_state;
		struct intel_crtc *crtc;
		int i;

		i915_sw_fence_commit(&state->commit_ready);

		for_each_new_intel_crtc_in_state(state, crtc, new_crtc_state, i)
			intel_dsb_cleanup(new_crtc_state);

		drm_atomic_helper_cleanup_planes(dev, &state->base);
		intel_runtime_pm_put(&dev_priv->runtime_pm, state->wakeref);
		return ret;
	}
	intel_shared_dpll_swap_state(state);
	intel_atomic_track_fbs(state);

	drm_atomic_state_get(&state->base);
	INIT_WORK(&state->base.commit_work, intel_atomic_commit_work);

	i915_sw_fence_commit(&state->commit_ready);
	if (nonblock && state->modeset) {
		queue_work(dev_priv->modeset_wq, &state->base.commit_work);
	} else if (nonblock) {
		queue_work(dev_priv->flip_wq, &state->base.commit_work);
	} else {
		if (state->modeset)
			flush_workqueue(dev_priv->modeset_wq);
		intel_atomic_commit_tail(state);
	}

	return 0;
}

struct wait_rps_boost {
	struct wait_queue_entry wait;

	struct drm_crtc *crtc;
	struct i915_request *request;
};

static int do_rps_boost(struct wait_queue_entry *_wait,
			unsigned mode, int sync, void *key)
{
	struct wait_rps_boost *wait = container_of(_wait, typeof(*wait), wait);
	struct i915_request *rq = wait->request;

	/*
	 * If we missed the vblank, but the request is already running it
	 * is reasonable to assume that it will complete before the next
	 * vblank without our intervention, so leave RPS alone.
	 */
	if (!i915_request_started(rq))
		intel_rps_boost(rq);
	i915_request_put(rq);

	drm_crtc_vblank_put(wait->crtc);

	list_del(&wait->wait.entry);
	kfree(wait);
	return 1;
}

static void add_rps_boost_after_vblank(struct drm_crtc *crtc,
				       struct dma_fence *fence)
{
	struct wait_rps_boost *wait;

	if (!dma_fence_is_i915(fence))
		return;

	if (DISPLAY_VER(to_i915(crtc->dev)) < 6)
		return;

	if (drm_crtc_vblank_get(crtc))
		return;

	wait = kmalloc(sizeof(*wait), GFP_KERNEL);
	if (!wait) {
		drm_crtc_vblank_put(crtc);
		return;
	}

	wait->request = to_request(dma_fence_get(fence));
	wait->crtc = crtc;

	wait->wait.func = do_rps_boost;
	wait->wait.flags = 0;

	add_wait_queue(drm_crtc_vblank_waitqueue(crtc), &wait->wait);
}

int intel_plane_pin_fb(struct intel_plane_state *plane_state)
{
	struct intel_plane *plane = to_intel_plane(plane_state->uapi.plane);
	struct drm_i915_private *dev_priv = to_i915(plane->base.dev);
	struct drm_framebuffer *fb = plane_state->hw.fb;
	struct i915_vma *vma;
	bool phys_cursor =
		plane->id == PLANE_CURSOR &&
		INTEL_INFO(dev_priv)->display.cursor_needs_physical;

	if (!intel_fb_uses_dpt(fb)) {
		vma = intel_pin_and_fence_fb_obj(fb, phys_cursor,
						 &plane_state->view.gtt,
						 intel_plane_uses_fence(plane_state),
						 &plane_state->flags);
		if (IS_ERR(vma))
			return PTR_ERR(vma);

		plane_state->ggtt_vma = vma;
	} else {
		struct intel_framebuffer *intel_fb = to_intel_framebuffer(fb);

		vma = intel_dpt_pin(intel_fb->dpt_vm);
		if (IS_ERR(vma))
			return PTR_ERR(vma);

		plane_state->ggtt_vma = vma;

		vma = intel_pin_fb_obj_dpt(fb, &plane_state->view.gtt, false,
					   &plane_state->flags, intel_fb->dpt_vm);
		if (IS_ERR(vma)) {
			intel_dpt_unpin(intel_fb->dpt_vm);
			plane_state->ggtt_vma = NULL;
			return PTR_ERR(vma);
		}

		plane_state->dpt_vma = vma;

		WARN_ON(plane_state->ggtt_vma == plane_state->dpt_vma);
	}

	return 0;
}

void intel_plane_unpin_fb(struct intel_plane_state *old_plane_state)
{
	struct drm_framebuffer *fb = old_plane_state->hw.fb;
	struct i915_vma *vma;

	if (!intel_fb_uses_dpt(fb)) {
		vma = fetch_and_zero(&old_plane_state->ggtt_vma);
		if (vma)
			intel_unpin_fb_vma(vma, old_plane_state->flags);
	} else {
		struct intel_framebuffer *intel_fb = to_intel_framebuffer(fb);

		vma = fetch_and_zero(&old_plane_state->dpt_vma);
		if (vma)
			intel_unpin_fb_vma(vma, old_plane_state->flags);

		vma = fetch_and_zero(&old_plane_state->ggtt_vma);
		if (vma)
			intel_dpt_unpin(intel_fb->dpt_vm);
	}
}

/**
 * intel_prepare_plane_fb - Prepare fb for usage on plane
 * @_plane: drm plane to prepare for
 * @_new_plane_state: the plane state being prepared
 *
 * Prepares a framebuffer for usage on a display plane.  Generally this
 * involves pinning the underlying object and updating the frontbuffer tracking
 * bits.  Some older platforms need special physical address handling for
 * cursor planes.
 *
 * Returns 0 on success, negative error code on failure.
 */
int
intel_prepare_plane_fb(struct drm_plane *_plane,
		       struct drm_plane_state *_new_plane_state)
{
	struct i915_sched_attr attr = { .priority = I915_PRIORITY_DISPLAY };
	struct intel_plane *plane = to_intel_plane(_plane);
	struct intel_plane_state *new_plane_state =
		to_intel_plane_state(_new_plane_state);
	struct intel_atomic_state *state =
		to_intel_atomic_state(new_plane_state->uapi.state);
	struct drm_i915_private *dev_priv = to_i915(plane->base.dev);
	const struct intel_plane_state *old_plane_state =
		intel_atomic_get_old_plane_state(state, plane);
	struct drm_i915_gem_object *obj = intel_fb_obj(new_plane_state->hw.fb);
	struct drm_i915_gem_object *old_obj = intel_fb_obj(old_plane_state->hw.fb);
	int ret;

	if (old_obj) {
		const struct intel_crtc_state *crtc_state =
			intel_atomic_get_new_crtc_state(state,
							to_intel_crtc(old_plane_state->hw.crtc));

		/* Big Hammer, we also need to ensure that any pending
		 * MI_WAIT_FOR_EVENT inside a user batch buffer on the
		 * current scanout is retired before unpinning the old
		 * framebuffer. Note that we rely on userspace rendering
		 * into the buffer attached to the pipe they are waiting
		 * on. If not, userspace generates a GPU hang with IPEHR
		 * point to the MI_WAIT_FOR_EVENT.
		 *
		 * This should only fail upon a hung GPU, in which case we
		 * can safely continue.
		 */
		if (intel_crtc_needs_modeset(crtc_state)) {
			ret = i915_sw_fence_await_reservation(&state->commit_ready,
							      old_obj->base.resv, NULL,
							      false, 0,
							      GFP_KERNEL);
			if (ret < 0)
				return ret;
		}
	}

	if (new_plane_state->uapi.fence) { /* explicit fencing */
		i915_gem_fence_wait_priority(new_plane_state->uapi.fence,
					     &attr);
		ret = i915_sw_fence_await_dma_fence(&state->commit_ready,
						    new_plane_state->uapi.fence,
						    i915_fence_timeout(dev_priv),
						    GFP_KERNEL);
		if (ret < 0)
			return ret;
	}

	if (!obj)
		return 0;


	ret = intel_plane_pin_fb(new_plane_state);
	if (ret)
		return ret;

	i915_gem_object_wait_priority(obj, 0, &attr);
	i915_gem_object_flush_frontbuffer(obj, ORIGIN_DIRTYFB);

	if (!new_plane_state->uapi.fence) { /* implicit fencing */
		struct dma_fence *fence;

		ret = i915_sw_fence_await_reservation(&state->commit_ready,
						      obj->base.resv, NULL,
						      false,
						      i915_fence_timeout(dev_priv),
						      GFP_KERNEL);
		if (ret < 0)
			goto unpin_fb;

		fence = dma_resv_get_excl_unlocked(obj->base.resv);
		if (fence) {
			add_rps_boost_after_vblank(new_plane_state->hw.crtc,
						   fence);
			dma_fence_put(fence);
		}
	} else {
		add_rps_boost_after_vblank(new_plane_state->hw.crtc,
					   new_plane_state->uapi.fence);
	}

	/*
	 * We declare pageflips to be interactive and so merit a small bias
	 * towards upclocking to deliver the frame on time. By only changing
	 * the RPS thresholds to sample more regularly and aim for higher
	 * clocks we can hopefully deliver low power workloads (like kodi)
	 * that are not quite steady state without resorting to forcing
	 * maximum clocks following a vblank miss (see do_rps_boost()).
	 */
	if (!state->rps_interactive) {
		intel_rps_mark_interactive(&dev_priv->gt.rps, true);
		state->rps_interactive = true;
	}

	return 0;

unpin_fb:
	intel_plane_unpin_fb(new_plane_state);

	return ret;
}

/**
 * intel_cleanup_plane_fb - Cleans up an fb after plane use
 * @plane: drm plane to clean up for
 * @_old_plane_state: the state from the previous modeset
 *
 * Cleans up a framebuffer that has just been removed from a plane.
 */
void
intel_cleanup_plane_fb(struct drm_plane *plane,
		       struct drm_plane_state *_old_plane_state)
{
	struct intel_plane_state *old_plane_state =
		to_intel_plane_state(_old_plane_state);
	struct intel_atomic_state *state =
		to_intel_atomic_state(old_plane_state->uapi.state);
	struct drm_i915_private *dev_priv = to_i915(plane->dev);
	struct drm_i915_gem_object *obj = intel_fb_obj(old_plane_state->hw.fb);

	if (!obj)
		return;

	if (state->rps_interactive) {
		intel_rps_mark_interactive(&dev_priv->gt.rps, false);
		state->rps_interactive = false;
	}

	/* Should only be called after a successful intel_prepare_plane_fb()! */
	intel_plane_unpin_fb(old_plane_state);
}

/**
 * intel_plane_destroy - destroy a plane
 * @plane: plane to destroy
 *
 * Common destruction function for all types of planes (primary, cursor,
 * sprite).
 */
void intel_plane_destroy(struct drm_plane *plane)
{
	drm_plane_cleanup(plane);
	kfree(to_intel_plane(plane));
}

static void intel_plane_possible_crtcs_init(struct drm_i915_private *dev_priv)
{
	struct intel_plane *plane;

	for_each_intel_plane(&dev_priv->drm, plane) {
		struct intel_crtc *crtc = intel_get_crtc_for_pipe(dev_priv,
								  plane->pipe);

		plane->base.possible_crtcs = drm_crtc_mask(&crtc->base);
	}
}


int intel_get_pipe_from_crtc_id_ioctl(struct drm_device *dev, void *data,
				      struct drm_file *file)
{
	struct drm_i915_get_pipe_from_crtc_id *pipe_from_crtc_id = data;
	struct drm_crtc *drmmode_crtc;
	struct intel_crtc *crtc;

	drmmode_crtc = drm_crtc_find(dev, file, pipe_from_crtc_id->crtc_id);
	if (!drmmode_crtc)
		return -ENOENT;

	crtc = to_intel_crtc(drmmode_crtc);
	pipe_from_crtc_id->pipe = crtc->pipe;

	return 0;
}

static u32 intel_encoder_possible_clones(struct intel_encoder *encoder)
{
	struct drm_device *dev = encoder->base.dev;
	struct intel_encoder *source_encoder;
	u32 possible_clones = 0;

	for_each_intel_encoder(dev, source_encoder) {
		if (encoders_cloneable(encoder, source_encoder))
			possible_clones |= drm_encoder_mask(&source_encoder->base);
	}

	return possible_clones;
}

static u32 intel_encoder_possible_crtcs(struct intel_encoder *encoder)
{
	struct drm_device *dev = encoder->base.dev;
	struct intel_crtc *crtc;
	u32 possible_crtcs = 0;

	for_each_intel_crtc(dev, crtc) {
		if (encoder->pipe_mask & BIT(crtc->pipe))
			possible_crtcs |= drm_crtc_mask(&crtc->base);
	}

	return possible_crtcs;
}

static bool ilk_has_edp_a(struct drm_i915_private *dev_priv)
{
	if (!IS_MOBILE(dev_priv))
		return false;

	if ((intel_de_read(dev_priv, DP_A) & DP_DETECTED) == 0)
		return false;

	if (IS_IRONLAKE(dev_priv) && (intel_de_read(dev_priv, FUSE_STRAP) & ILK_eDP_A_DISABLE))
		return false;

	return true;
}

static bool intel_ddi_crt_present(struct drm_i915_private *dev_priv)
{
	if (DISPLAY_VER(dev_priv) >= 9)
		return false;

	if (IS_HSW_ULT(dev_priv) || IS_BDW_ULT(dev_priv))
		return false;

	if (HAS_PCH_LPT_H(dev_priv) &&
	    intel_de_read(dev_priv, SFUSE_STRAP) & SFUSE_STRAP_CRT_DISABLED)
		return false;

	/* DDI E can't be used if DDI A requires 4 lanes */
	if (intel_de_read(dev_priv, DDI_BUF_CTL(PORT_A)) & DDI_A_4_LANES)
		return false;

	if (!dev_priv->vbt.int_crt_support)
		return false;

	return true;
}

static void intel_setup_outputs(struct drm_i915_private *dev_priv)
{
	struct intel_encoder *encoder;
	bool dpd_is_edp = false;

	intel_pps_unlock_regs_wa(dev_priv);

	if (!HAS_DISPLAY(dev_priv))
		return;

	if (IS_ALDERLAKE_P(dev_priv)) {
		intel_ddi_init(dev_priv, PORT_A);
		intel_ddi_init(dev_priv, PORT_B);
		intel_ddi_init(dev_priv, PORT_TC1);
		intel_ddi_init(dev_priv, PORT_TC2);
		intel_ddi_init(dev_priv, PORT_TC3);
		intel_ddi_init(dev_priv, PORT_TC4);
	} else if (IS_ALDERLAKE_S(dev_priv)) {
		intel_ddi_init(dev_priv, PORT_A);
		intel_ddi_init(dev_priv, PORT_TC1);
		intel_ddi_init(dev_priv, PORT_TC2);
		intel_ddi_init(dev_priv, PORT_TC3);
		intel_ddi_init(dev_priv, PORT_TC4);
	} else if (IS_DG1(dev_priv) || IS_ROCKETLAKE(dev_priv)) {
		intel_ddi_init(dev_priv, PORT_A);
		intel_ddi_init(dev_priv, PORT_B);
		intel_ddi_init(dev_priv, PORT_TC1);
		intel_ddi_init(dev_priv, PORT_TC2);
	} else if (DISPLAY_VER(dev_priv) >= 12) {
		intel_ddi_init(dev_priv, PORT_A);
		intel_ddi_init(dev_priv, PORT_B);
		intel_ddi_init(dev_priv, PORT_TC1);
		intel_ddi_init(dev_priv, PORT_TC2);
		intel_ddi_init(dev_priv, PORT_TC3);
		intel_ddi_init(dev_priv, PORT_TC4);
		intel_ddi_init(dev_priv, PORT_TC5);
		intel_ddi_init(dev_priv, PORT_TC6);
		icl_dsi_init(dev_priv);
	} else if (IS_JSL_EHL(dev_priv)) {
		intel_ddi_init(dev_priv, PORT_A);
		intel_ddi_init(dev_priv, PORT_B);
		intel_ddi_init(dev_priv, PORT_C);
		intel_ddi_init(dev_priv, PORT_D);
		icl_dsi_init(dev_priv);
	} else if (DISPLAY_VER(dev_priv) == 11) {
		intel_ddi_init(dev_priv, PORT_A);
		intel_ddi_init(dev_priv, PORT_B);
		intel_ddi_init(dev_priv, PORT_C);
		intel_ddi_init(dev_priv, PORT_D);
		intel_ddi_init(dev_priv, PORT_E);
		intel_ddi_init(dev_priv, PORT_F);
		icl_dsi_init(dev_priv);
	} else if (IS_GEMINILAKE(dev_priv) || IS_BROXTON(dev_priv)) {
		intel_ddi_init(dev_priv, PORT_A);
		intel_ddi_init(dev_priv, PORT_B);
		intel_ddi_init(dev_priv, PORT_C);
		vlv_dsi_init(dev_priv);
	} else if (DISPLAY_VER(dev_priv) >= 9) {
		intel_ddi_init(dev_priv, PORT_A);
		intel_ddi_init(dev_priv, PORT_B);
		intel_ddi_init(dev_priv, PORT_C);
		intel_ddi_init(dev_priv, PORT_D);
		intel_ddi_init(dev_priv, PORT_E);
		intel_ddi_init(dev_priv, PORT_F);
	} else if (HAS_DDI(dev_priv)) {
		u32 found;

		if (intel_ddi_crt_present(dev_priv))
			intel_crt_init(dev_priv);

		/* Haswell uses DDI functions to detect digital outputs. */
		found = intel_de_read(dev_priv, DDI_BUF_CTL(PORT_A)) & DDI_INIT_DISPLAY_DETECTED;
		if (found)
			intel_ddi_init(dev_priv, PORT_A);

		found = intel_de_read(dev_priv, SFUSE_STRAP);
		if (found & SFUSE_STRAP_DDIB_DETECTED)
			intel_ddi_init(dev_priv, PORT_B);
		if (found & SFUSE_STRAP_DDIC_DETECTED)
			intel_ddi_init(dev_priv, PORT_C);
		if (found & SFUSE_STRAP_DDID_DETECTED)
			intel_ddi_init(dev_priv, PORT_D);
		if (found & SFUSE_STRAP_DDIF_DETECTED)
			intel_ddi_init(dev_priv, PORT_F);
	} else if (HAS_PCH_SPLIT(dev_priv)) {
		int found;

		/*
		 * intel_edp_init_connector() depends on this completing first,
		 * to prevent the registration of both eDP and LVDS and the
		 * incorrect sharing of the PPS.
		 */
		intel_lvds_init(dev_priv);
		intel_crt_init(dev_priv);

		dpd_is_edp = intel_dp_is_port_edp(dev_priv, PORT_D);

		if (ilk_has_edp_a(dev_priv))
			g4x_dp_init(dev_priv, DP_A, PORT_A);

		if (intel_de_read(dev_priv, PCH_HDMIB) & SDVO_DETECTED) {
			/* PCH SDVOB multiplex with HDMIB */
			found = intel_sdvo_init(dev_priv, PCH_SDVOB, PORT_B);
			if (!found)
				g4x_hdmi_init(dev_priv, PCH_HDMIB, PORT_B);
			if (!found && (intel_de_read(dev_priv, PCH_DP_B) & DP_DETECTED))
				g4x_dp_init(dev_priv, PCH_DP_B, PORT_B);
		}

		if (intel_de_read(dev_priv, PCH_HDMIC) & SDVO_DETECTED)
			g4x_hdmi_init(dev_priv, PCH_HDMIC, PORT_C);

		if (!dpd_is_edp && intel_de_read(dev_priv, PCH_HDMID) & SDVO_DETECTED)
			g4x_hdmi_init(dev_priv, PCH_HDMID, PORT_D);

		if (intel_de_read(dev_priv, PCH_DP_C) & DP_DETECTED)
			g4x_dp_init(dev_priv, PCH_DP_C, PORT_C);

		if (intel_de_read(dev_priv, PCH_DP_D) & DP_DETECTED)
			g4x_dp_init(dev_priv, PCH_DP_D, PORT_D);
	} else if (IS_VALLEYVIEW(dev_priv) || IS_CHERRYVIEW(dev_priv)) {
		bool has_edp, has_port;

		if (IS_VALLEYVIEW(dev_priv) && dev_priv->vbt.int_crt_support)
			intel_crt_init(dev_priv);

		/*
		 * The DP_DETECTED bit is the latched state of the DDC
		 * SDA pin at boot. However since eDP doesn't require DDC
		 * (no way to plug in a DP->HDMI dongle) the DDC pins for
		 * eDP ports may have been muxed to an alternate function.
		 * Thus we can't rely on the DP_DETECTED bit alone to detect
		 * eDP ports. Consult the VBT as well as DP_DETECTED to
		 * detect eDP ports.
		 *
		 * Sadly the straps seem to be missing sometimes even for HDMI
		 * ports (eg. on Voyo V3 - CHT x7-Z8700), so check both strap
		 * and VBT for the presence of the port. Additionally we can't
		 * trust the port type the VBT declares as we've seen at least
		 * HDMI ports that the VBT claim are DP or eDP.
		 */
		has_edp = intel_dp_is_port_edp(dev_priv, PORT_B);
		has_port = intel_bios_is_port_present(dev_priv, PORT_B);
		if (intel_de_read(dev_priv, VLV_DP_B) & DP_DETECTED || has_port)
			has_edp &= g4x_dp_init(dev_priv, VLV_DP_B, PORT_B);
		if ((intel_de_read(dev_priv, VLV_HDMIB) & SDVO_DETECTED || has_port) && !has_edp)
			g4x_hdmi_init(dev_priv, VLV_HDMIB, PORT_B);

		has_edp = intel_dp_is_port_edp(dev_priv, PORT_C);
		has_port = intel_bios_is_port_present(dev_priv, PORT_C);
		if (intel_de_read(dev_priv, VLV_DP_C) & DP_DETECTED || has_port)
			has_edp &= g4x_dp_init(dev_priv, VLV_DP_C, PORT_C);
		if ((intel_de_read(dev_priv, VLV_HDMIC) & SDVO_DETECTED || has_port) && !has_edp)
			g4x_hdmi_init(dev_priv, VLV_HDMIC, PORT_C);

		if (IS_CHERRYVIEW(dev_priv)) {
			/*
			 * eDP not supported on port D,
			 * so no need to worry about it
			 */
			has_port = intel_bios_is_port_present(dev_priv, PORT_D);
			if (intel_de_read(dev_priv, CHV_DP_D) & DP_DETECTED || has_port)
				g4x_dp_init(dev_priv, CHV_DP_D, PORT_D);
			if (intel_de_read(dev_priv, CHV_HDMID) & SDVO_DETECTED || has_port)
				g4x_hdmi_init(dev_priv, CHV_HDMID, PORT_D);
		}

		vlv_dsi_init(dev_priv);
	} else if (IS_PINEVIEW(dev_priv)) {
		intel_lvds_init(dev_priv);
		intel_crt_init(dev_priv);
	} else if (IS_DISPLAY_VER(dev_priv, 3, 4)) {
		bool found = false;

		if (IS_MOBILE(dev_priv))
			intel_lvds_init(dev_priv);

		intel_crt_init(dev_priv);

		if (intel_de_read(dev_priv, GEN3_SDVOB) & SDVO_DETECTED) {
			drm_dbg_kms(&dev_priv->drm, "probing SDVOB\n");
			found = intel_sdvo_init(dev_priv, GEN3_SDVOB, PORT_B);
			if (!found && IS_G4X(dev_priv)) {
				drm_dbg_kms(&dev_priv->drm,
					    "probing HDMI on SDVOB\n");
				g4x_hdmi_init(dev_priv, GEN4_HDMIB, PORT_B);
			}

			if (!found && IS_G4X(dev_priv))
				g4x_dp_init(dev_priv, DP_B, PORT_B);
		}

		/* Before G4X SDVOC doesn't have its own detect register */

		if (intel_de_read(dev_priv, GEN3_SDVOB) & SDVO_DETECTED) {
			drm_dbg_kms(&dev_priv->drm, "probing SDVOC\n");
			found = intel_sdvo_init(dev_priv, GEN3_SDVOC, PORT_C);
		}

		if (!found && (intel_de_read(dev_priv, GEN3_SDVOC) & SDVO_DETECTED)) {

			if (IS_G4X(dev_priv)) {
				drm_dbg_kms(&dev_priv->drm,
					    "probing HDMI on SDVOC\n");
				g4x_hdmi_init(dev_priv, GEN4_HDMIC, PORT_C);
			}
			if (IS_G4X(dev_priv))
				g4x_dp_init(dev_priv, DP_C, PORT_C);
		}

		if (IS_G4X(dev_priv) && (intel_de_read(dev_priv, DP_D) & DP_DETECTED))
			g4x_dp_init(dev_priv, DP_D, PORT_D);

		if (SUPPORTS_TV(dev_priv))
			intel_tv_init(dev_priv);
	} else if (DISPLAY_VER(dev_priv) == 2) {
		if (IS_I85X(dev_priv))
			intel_lvds_init(dev_priv);

		intel_crt_init(dev_priv);
		intel_dvo_init(dev_priv);
	}

	for_each_intel_encoder(&dev_priv->drm, encoder) {
		encoder->base.possible_crtcs =
			intel_encoder_possible_crtcs(encoder);
		encoder->base.possible_clones =
			intel_encoder_possible_clones(encoder);
	}

	intel_init_pch_refclk(dev_priv);

	drm_helper_move_panel_connectors_to_head(&dev_priv->drm);
}

static void intel_user_framebuffer_destroy(struct drm_framebuffer *fb)
{
	struct intel_framebuffer *intel_fb = to_intel_framebuffer(fb);

	drm_framebuffer_cleanup(fb);

	if (intel_fb_uses_dpt(fb))
		intel_dpt_destroy(intel_fb->dpt_vm);

	intel_frontbuffer_put(intel_fb->frontbuffer);

	kfree(intel_fb);
}

static int intel_user_framebuffer_create_handle(struct drm_framebuffer *fb,
						struct drm_file *file,
						unsigned int *handle)
{
	struct drm_i915_gem_object *obj = intel_fb_obj(fb);
	struct drm_i915_private *i915 = to_i915(obj->base.dev);

	if (i915_gem_object_is_userptr(obj)) {
		drm_dbg(&i915->drm,
			"attempting to use a userptr for a framebuffer, denied\n");
		return -EINVAL;
	}

	return drm_gem_handle_create(file, &obj->base, handle);
}

static int intel_user_framebuffer_dirty(struct drm_framebuffer *fb,
					struct drm_file *file,
					unsigned flags, unsigned color,
					struct drm_clip_rect *clips,
					unsigned num_clips)
{
	struct drm_i915_gem_object *obj = intel_fb_obj(fb);

	i915_gem_object_flush_if_display(obj);
	intel_frontbuffer_flush(to_intel_frontbuffer(fb), ORIGIN_DIRTYFB);

	return 0;
}

static const struct drm_framebuffer_funcs intel_fb_funcs = {
	.destroy = intel_user_framebuffer_destroy,
	.create_handle = intel_user_framebuffer_create_handle,
	.dirty = intel_user_framebuffer_dirty,
};

static int intel_framebuffer_init(struct intel_framebuffer *intel_fb,
				  struct drm_i915_gem_object *obj,
				  struct drm_mode_fb_cmd2 *mode_cmd)
{
	struct drm_i915_private *dev_priv = to_i915(obj->base.dev);
	struct drm_framebuffer *fb = &intel_fb->base;
	u32 max_stride;
	unsigned int tiling, stride;
	int ret = -EINVAL;
	int i;

	intel_fb->frontbuffer = intel_frontbuffer_get(obj);
	if (!intel_fb->frontbuffer)
		return -ENOMEM;

	i915_gem_object_lock(obj, NULL);
	tiling = i915_gem_object_get_tiling(obj);
	stride = i915_gem_object_get_stride(obj);
	i915_gem_object_unlock(obj);

	if (mode_cmd->flags & DRM_MODE_FB_MODIFIERS) {
		/*
		 * If there's a fence, enforce that
		 * the fb modifier and tiling mode match.
		 */
		if (tiling != I915_TILING_NONE &&
		    tiling != intel_fb_modifier_to_tiling(mode_cmd->modifier[0])) {
			drm_dbg_kms(&dev_priv->drm,
				    "tiling_mode doesn't match fb modifier\n");
			goto err;
		}
	} else {
		if (tiling == I915_TILING_X) {
			mode_cmd->modifier[0] = I915_FORMAT_MOD_X_TILED;
		} else if (tiling == I915_TILING_Y) {
			drm_dbg_kms(&dev_priv->drm,
				    "No Y tiling for legacy addfb\n");
			goto err;
		}
	}

	if (!drm_any_plane_has_format(&dev_priv->drm,
				      mode_cmd->pixel_format,
				      mode_cmd->modifier[0])) {
		drm_dbg_kms(&dev_priv->drm,
			    "unsupported pixel format %p4cc / modifier 0x%llx\n",
			    &mode_cmd->pixel_format, mode_cmd->modifier[0]);
		goto err;
	}

	/*
	 * gen2/3 display engine uses the fence if present,
	 * so the tiling mode must match the fb modifier exactly.
	 */
	if (DISPLAY_VER(dev_priv) < 4 &&
	    tiling != intel_fb_modifier_to_tiling(mode_cmd->modifier[0])) {
		drm_dbg_kms(&dev_priv->drm,
			    "tiling_mode must match fb modifier exactly on gen2/3\n");
		goto err;
	}

	max_stride = intel_fb_max_stride(dev_priv, mode_cmd->pixel_format,
					 mode_cmd->modifier[0]);
	if (mode_cmd->pitches[0] > max_stride) {
		drm_dbg_kms(&dev_priv->drm,
			    "%s pitch (%u) must be at most %d\n",
			    mode_cmd->modifier[0] != DRM_FORMAT_MOD_LINEAR ?
			    "tiled" : "linear",
			    mode_cmd->pitches[0], max_stride);
		goto err;
	}

	/*
	 * If there's a fence, enforce that
	 * the fb pitch and fence stride match.
	 */
	if (tiling != I915_TILING_NONE && mode_cmd->pitches[0] != stride) {
		drm_dbg_kms(&dev_priv->drm,
			    "pitch (%d) must match tiling stride (%d)\n",
			    mode_cmd->pitches[0], stride);
		goto err;
	}

	/* FIXME need to adjust LINOFF/TILEOFF accordingly. */
	if (mode_cmd->offsets[0] != 0) {
		drm_dbg_kms(&dev_priv->drm,
			    "plane 0 offset (0x%08x) must be 0\n",
			    mode_cmd->offsets[0]);
		goto err;
	}

	drm_helper_mode_fill_fb_struct(&dev_priv->drm, fb, mode_cmd);

	for (i = 0; i < fb->format->num_planes; i++) {
		u32 stride_alignment;

		if (mode_cmd->handles[i] != mode_cmd->handles[0]) {
			drm_dbg_kms(&dev_priv->drm, "bad plane %d handle\n",
				    i);
			goto err;
		}

		stride_alignment = intel_fb_stride_alignment(fb, i);
		if (fb->pitches[i] & (stride_alignment - 1)) {
			drm_dbg_kms(&dev_priv->drm,
				    "plane %d pitch (%d) must be at least %u byte aligned\n",
				    i, fb->pitches[i], stride_alignment);
			goto err;
		}

		if (is_gen12_ccs_plane(fb, i) && !is_gen12_ccs_cc_plane(fb, i)) {
			int ccs_aux_stride = gen12_ccs_aux_stride(fb, i);

			if (fb->pitches[i] != ccs_aux_stride) {
				drm_dbg_kms(&dev_priv->drm,
					    "ccs aux plane %d pitch (%d) must be %d\n",
					    i,
					    fb->pitches[i], ccs_aux_stride);
				goto err;
			}
		}

		/* TODO: Add POT stride remapping support for CCS formats as well. */
		if (IS_ALDERLAKE_P(dev_priv) &&
		    mode_cmd->modifier[i] != DRM_FORMAT_MOD_LINEAR &&
		    !intel_fb_needs_pot_stride_remap(intel_fb) &&
		    !is_power_of_2(mode_cmd->pitches[i])) {
			drm_dbg_kms(&dev_priv->drm,
				    "plane %d pitch (%d) must be power of two for tiled buffers\n",
				    i, mode_cmd->pitches[i]);
			goto err;
		}

		fb->obj[i] = &obj->base;
	}

	ret = intel_fill_fb_info(dev_priv, intel_fb);
	if (ret)
		goto err;

	if (intel_fb_uses_dpt(fb)) {
		struct i915_address_space *vm;

		vm = intel_dpt_create(intel_fb);
		if (IS_ERR(vm)) {
			ret = PTR_ERR(vm);
			goto err;
		}

		intel_fb->dpt_vm = vm;
	}

	ret = drm_framebuffer_init(&dev_priv->drm, fb, &intel_fb_funcs);
	if (ret) {
		drm_err(&dev_priv->drm, "framebuffer init failed %d\n", ret);
		goto err;
	}

	return 0;

err:
	intel_frontbuffer_put(intel_fb->frontbuffer);
	return ret;
}

static struct drm_framebuffer *
intel_user_framebuffer_create(struct drm_device *dev,
			      struct drm_file *filp,
			      const struct drm_mode_fb_cmd2 *user_mode_cmd)
{
	struct drm_framebuffer *fb;
	struct drm_i915_gem_object *obj;
	struct drm_mode_fb_cmd2 mode_cmd = *user_mode_cmd;
	struct drm_i915_private *i915;

	obj = i915_gem_object_lookup(filp, mode_cmd.handles[0]);
	if (!obj)
		return ERR_PTR(-ENOENT);

	/* object is backed with LMEM for discrete */
	i915 = to_i915(obj->base.dev);
	if (HAS_LMEM(i915) && !i915_gem_object_is_lmem(obj)) {
		/* object is "remote", not in local memory */
		i915_gem_object_put(obj);
		return ERR_PTR(-EREMOTE);
	}

	fb = intel_framebuffer_create(obj, &mode_cmd);
	i915_gem_object_put(obj);

	return fb;
}

static enum drm_mode_status
intel_mode_valid(struct drm_device *dev,
		 const struct drm_display_mode *mode)
{
	struct drm_i915_private *dev_priv = to_i915(dev);
	int hdisplay_max, htotal_max;
	int vdisplay_max, vtotal_max;

	/*
	 * Can't reject DBLSCAN here because Xorg ddxen can add piles
	 * of DBLSCAN modes to the output's mode list when they detect
	 * the scaling mode property on the connector. And they don't
	 * ask the kernel to validate those modes in any way until
	 * modeset time at which point the client gets a protocol error.
	 * So in order to not upset those clients we silently ignore the
	 * DBLSCAN flag on such connectors. For other connectors we will
	 * reject modes with the DBLSCAN flag in encoder->compute_config().
	 * And we always reject DBLSCAN modes in connector->mode_valid()
	 * as we never want such modes on the connector's mode list.
	 */

	if (mode->vscan > 1)
		return MODE_NO_VSCAN;

	if (mode->flags & DRM_MODE_FLAG_HSKEW)
		return MODE_H_ILLEGAL;

	if (mode->flags & (DRM_MODE_FLAG_CSYNC |
			   DRM_MODE_FLAG_NCSYNC |
			   DRM_MODE_FLAG_PCSYNC))
		return MODE_HSYNC;

	if (mode->flags & (DRM_MODE_FLAG_BCAST |
			   DRM_MODE_FLAG_PIXMUX |
			   DRM_MODE_FLAG_CLKDIV2))
		return MODE_BAD;

	/* Transcoder timing limits */
	if (DISPLAY_VER(dev_priv) >= 11) {
		hdisplay_max = 16384;
		vdisplay_max = 8192;
		htotal_max = 16384;
		vtotal_max = 8192;
	} else if (DISPLAY_VER(dev_priv) >= 9 ||
		   IS_BROADWELL(dev_priv) || IS_HASWELL(dev_priv)) {
		hdisplay_max = 8192; /* FDI max 4096 handled elsewhere */
		vdisplay_max = 4096;
		htotal_max = 8192;
		vtotal_max = 8192;
	} else if (DISPLAY_VER(dev_priv) >= 3) {
		hdisplay_max = 4096;
		vdisplay_max = 4096;
		htotal_max = 8192;
		vtotal_max = 8192;
	} else {
		hdisplay_max = 2048;
		vdisplay_max = 2048;
		htotal_max = 4096;
		vtotal_max = 4096;
	}

	if (mode->hdisplay > hdisplay_max ||
	    mode->hsync_start > htotal_max ||
	    mode->hsync_end > htotal_max ||
	    mode->htotal > htotal_max)
		return MODE_H_ILLEGAL;

	if (mode->vdisplay > vdisplay_max ||
	    mode->vsync_start > vtotal_max ||
	    mode->vsync_end > vtotal_max ||
	    mode->vtotal > vtotal_max)
		return MODE_V_ILLEGAL;

	if (DISPLAY_VER(dev_priv) >= 5) {
		if (mode->hdisplay < 64 ||
		    mode->htotal - mode->hdisplay < 32)
			return MODE_H_ILLEGAL;

		if (mode->vtotal - mode->vdisplay < 5)
			return MODE_V_ILLEGAL;
	} else {
		if (mode->htotal - mode->hdisplay < 32)
			return MODE_H_ILLEGAL;

		if (mode->vtotal - mode->vdisplay < 3)
			return MODE_V_ILLEGAL;
	}

	return MODE_OK;
}

enum drm_mode_status
intel_mode_valid_max_plane_size(struct drm_i915_private *dev_priv,
				const struct drm_display_mode *mode,
				bool bigjoiner)
{
	int plane_width_max, plane_height_max;

	/*
	 * intel_mode_valid() should be
	 * sufficient on older platforms.
	 */
	if (DISPLAY_VER(dev_priv) < 9)
		return MODE_OK;

	/*
	 * Most people will probably want a fullscreen
	 * plane so let's not advertize modes that are
	 * too big for that.
	 */
	if (DISPLAY_VER(dev_priv) >= 11) {
		plane_width_max = 5120 << bigjoiner;
		plane_height_max = 4320;
	} else {
		plane_width_max = 5120;
		plane_height_max = 4096;
	}

	if (mode->hdisplay > plane_width_max)
		return MODE_H_ILLEGAL;

	if (mode->vdisplay > plane_height_max)
		return MODE_V_ILLEGAL;

	return MODE_OK;
}

static const struct drm_mode_config_funcs intel_mode_funcs = {
	.fb_create = intel_user_framebuffer_create,
	.get_format_info = intel_get_format_info,
	.output_poll_changed = intel_fbdev_output_poll_changed,
	.mode_valid = intel_mode_valid,
	.atomic_check = intel_atomic_check,
	.atomic_commit = intel_atomic_commit,
	.atomic_state_alloc = intel_atomic_state_alloc,
	.atomic_state_clear = intel_atomic_state_clear,
	.atomic_state_free = intel_atomic_state_free,
};

/**
 * intel_init_display_hooks - initialize the display modesetting hooks
 * @dev_priv: device private
 */
void intel_init_display_hooks(struct drm_i915_private *dev_priv)
{
	if (!HAS_DISPLAY(dev_priv))
		return;

	intel_init_cdclk_hooks(dev_priv);
	intel_init_audio_hooks(dev_priv);

	intel_dpll_init_clock_hook(dev_priv);

	if (DISPLAY_VER(dev_priv) >= 9) {
		dev_priv->display.get_pipe_config = hsw_get_pipe_config;
		dev_priv->display.crtc_enable = hsw_crtc_enable;
		dev_priv->display.crtc_disable = hsw_crtc_disable;
	} else if (HAS_DDI(dev_priv)) {
		dev_priv->display.get_pipe_config = hsw_get_pipe_config;
		dev_priv->display.crtc_enable = hsw_crtc_enable;
		dev_priv->display.crtc_disable = hsw_crtc_disable;
	} else if (HAS_PCH_SPLIT(dev_priv)) {
		dev_priv->display.get_pipe_config = ilk_get_pipe_config;
		dev_priv->display.crtc_enable = ilk_crtc_enable;
		dev_priv->display.crtc_disable = ilk_crtc_disable;
	} else if (IS_CHERRYVIEW(dev_priv) ||
		   IS_VALLEYVIEW(dev_priv)) {
		dev_priv->display.get_pipe_config = i9xx_get_pipe_config;
		dev_priv->display.crtc_enable = valleyview_crtc_enable;
		dev_priv->display.crtc_disable = i9xx_crtc_disable;
	} else {
		dev_priv->display.get_pipe_config = i9xx_get_pipe_config;
		dev_priv->display.crtc_enable = i9xx_crtc_enable;
		dev_priv->display.crtc_disable = i9xx_crtc_disable;
	}

	intel_fdi_init_hook(dev_priv);

	if (DISPLAY_VER(dev_priv) >= 9) {
		dev_priv->display.commit_modeset_enables = skl_commit_modeset_enables;
		dev_priv->display.get_initial_plane_config = skl_get_initial_plane_config;
	} else {
		dev_priv->display.commit_modeset_enables = intel_commit_modeset_enables;
		dev_priv->display.get_initial_plane_config = i9xx_get_initial_plane_config;
	}

}

void intel_modeset_init_hw(struct drm_i915_private *i915)
{
	struct intel_cdclk_state *cdclk_state;

	if (!HAS_DISPLAY(i915))
		return;

	cdclk_state = to_intel_cdclk_state(i915->cdclk.obj.state);

	intel_update_cdclk(i915);
	intel_dump_cdclk_config(&i915->cdclk.hw, "Current CDCLK");
	cdclk_state->logical = cdclk_state->actual = i915->cdclk.hw;
}

static int sanitize_watermarks_add_affected(struct drm_atomic_state *state)
{
	struct drm_plane *plane;
	struct intel_crtc *crtc;

	for_each_intel_crtc(state->dev, crtc) {
		struct intel_crtc_state *crtc_state;

		crtc_state = intel_atomic_get_crtc_state(state, crtc);
		if (IS_ERR(crtc_state))
			return PTR_ERR(crtc_state);

		if (crtc_state->hw.active) {
			/*
			 * Preserve the inherited flag to avoid
			 * taking the full modeset path.
			 */
			crtc_state->inherited = true;
		}
	}

	drm_for_each_plane(plane, state->dev) {
		struct drm_plane_state *plane_state;

		plane_state = drm_atomic_get_plane_state(state, plane);
		if (IS_ERR(plane_state))
			return PTR_ERR(plane_state);
	}

	return 0;
}

/*
 * Calculate what we think the watermarks should be for the state we've read
 * out of the hardware and then immediately program those watermarks so that
 * we ensure the hardware settings match our internal state.
 *
 * We can calculate what we think WM's should be by creating a duplicate of the
 * current state (which was constructed during hardware readout) and running it
 * through the atomic check code to calculate new watermark values in the
 * state object.
 */
static void sanitize_watermarks(struct drm_i915_private *dev_priv)
{
	struct drm_atomic_state *state;
	struct intel_atomic_state *intel_state;
	struct intel_crtc *crtc;
	struct intel_crtc_state *crtc_state;
	struct drm_modeset_acquire_ctx ctx;
	int ret;
	int i;

	/* Only supported on platforms that use atomic watermark design */
	if (!dev_priv->display.optimize_watermarks)
		return;

	state = drm_atomic_state_alloc(&dev_priv->drm);
	if (drm_WARN_ON(&dev_priv->drm, !state))
		return;

	intel_state = to_intel_atomic_state(state);

	drm_modeset_acquire_init(&ctx, 0);

retry:
	state->acquire_ctx = &ctx;

	/*
	 * Hardware readout is the only time we don't want to calculate
	 * intermediate watermarks (since we don't trust the current
	 * watermarks).
	 */
	if (!HAS_GMCH(dev_priv))
		intel_state->skip_intermediate_wm = true;

	ret = sanitize_watermarks_add_affected(state);
	if (ret)
		goto fail;

	ret = intel_atomic_check(&dev_priv->drm, state);
	if (ret)
		goto fail;

	/* Write calculated watermark values back */
	for_each_new_intel_crtc_in_state(intel_state, crtc, crtc_state, i) {
		crtc_state->wm.need_postvbl_update = true;
		dev_priv->display.optimize_watermarks(intel_state, crtc);

		to_intel_crtc_state(crtc->base.state)->wm = crtc_state->wm;
	}

fail:
	if (ret == -EDEADLK) {
		drm_atomic_state_clear(state);
		drm_modeset_backoff(&ctx);
		goto retry;
	}

	/*
	 * If we fail here, it means that the hardware appears to be
	 * programmed in a way that shouldn't be possible, given our
	 * understanding of watermark requirements.  This might mean a
	 * mistake in the hardware readout code or a mistake in the
	 * watermark calculations for a given platform.  Raise a WARN
	 * so that this is noticeable.
	 *
	 * If this actually happens, we'll have to just leave the
	 * BIOS-programmed watermarks untouched and hope for the best.
	 */
	drm_WARN(&dev_priv->drm, ret,
		 "Could not determine valid watermarks for inherited state\n");

	drm_atomic_state_put(state);

	drm_modeset_drop_locks(&ctx);
	drm_modeset_acquire_fini(&ctx);
}

static void intel_update_fdi_pll_freq(struct drm_i915_private *dev_priv)
{
	if (IS_IRONLAKE(dev_priv)) {
		u32 fdi_pll_clk =
			intel_de_read(dev_priv, FDI_PLL_BIOS_0) & FDI_PLL_FB_CLOCK_MASK;

		dev_priv->fdi_pll_freq = (fdi_pll_clk + 2) * 10000;
	} else if (IS_SANDYBRIDGE(dev_priv) || IS_IVYBRIDGE(dev_priv)) {
		dev_priv->fdi_pll_freq = 270000;
	} else {
		return;
	}

	drm_dbg(&dev_priv->drm, "FDI PLL freq=%d\n", dev_priv->fdi_pll_freq);
}

static int intel_initial_commit(struct drm_device *dev)
{
	struct drm_atomic_state *state = NULL;
	struct drm_modeset_acquire_ctx ctx;
	struct intel_crtc *crtc;
	int ret = 0;

	state = drm_atomic_state_alloc(dev);
	if (!state)
		return -ENOMEM;

	drm_modeset_acquire_init(&ctx, 0);

retry:
	state->acquire_ctx = &ctx;

	for_each_intel_crtc(dev, crtc) {
		struct intel_crtc_state *crtc_state =
			intel_atomic_get_crtc_state(state, crtc);

		if (IS_ERR(crtc_state)) {
			ret = PTR_ERR(crtc_state);
			goto out;
		}

		if (crtc_state->hw.active) {
			struct intel_encoder *encoder;

			/*
			 * We've not yet detected sink capabilities
			 * (audio,infoframes,etc.) and thus we don't want to
			 * force a full state recomputation yet. We want that to
			 * happen only for the first real commit from userspace.
			 * So preserve the inherited flag for the time being.
			 */
			crtc_state->inherited = true;

			ret = drm_atomic_add_affected_planes(state, &crtc->base);
			if (ret)
				goto out;

			/*
			 * FIXME hack to force a LUT update to avoid the
			 * plane update forcing the pipe gamma on without
			 * having a proper LUT loaded. Remove once we
			 * have readout for pipe gamma enable.
			 */
			crtc_state->uapi.color_mgmt_changed = true;

			for_each_intel_encoder_mask(dev, encoder,
						    crtc_state->uapi.encoder_mask) {
				if (encoder->initial_fastset_check &&
				    !encoder->initial_fastset_check(encoder, crtc_state)) {
					ret = drm_atomic_add_affected_connectors(state,
										 &crtc->base);
					if (ret)
						goto out;
				}
			}
		}
	}

	ret = drm_atomic_commit(state);

out:
	if (ret == -EDEADLK) {
		drm_atomic_state_clear(state);
		drm_modeset_backoff(&ctx);
		goto retry;
	}

	drm_atomic_state_put(state);

	drm_modeset_drop_locks(&ctx);
	drm_modeset_acquire_fini(&ctx);

	return ret;
}

static void intel_mode_config_init(struct drm_i915_private *i915)
{
	struct drm_mode_config *mode_config = &i915->drm.mode_config;

	drm_mode_config_init(&i915->drm);
	INIT_LIST_HEAD(&i915->global_obj_list);

	mode_config->min_width = 0;
	mode_config->min_height = 0;

	mode_config->preferred_depth = 24;
	mode_config->prefer_shadow = 1;

	mode_config->funcs = &intel_mode_funcs;

	mode_config->async_page_flip = has_async_flips(i915);

	/*
	 * Maximum framebuffer dimensions, chosen to match
	 * the maximum render engine surface size on gen4+.
	 */
	if (DISPLAY_VER(i915) >= 7) {
		mode_config->max_width = 16384;
		mode_config->max_height = 16384;
	} else if (DISPLAY_VER(i915) >= 4) {
		mode_config->max_width = 8192;
		mode_config->max_height = 8192;
	} else if (DISPLAY_VER(i915) == 3) {
		mode_config->max_width = 4096;
		mode_config->max_height = 4096;
	} else {
		mode_config->max_width = 2048;
		mode_config->max_height = 2048;
	}

	if (IS_I845G(i915) || IS_I865G(i915)) {
		mode_config->cursor_width = IS_I845G(i915) ? 64 : 512;
		mode_config->cursor_height = 1023;
	} else if (IS_I830(i915) || IS_I85X(i915) ||
		   IS_I915G(i915) || IS_I915GM(i915)) {
		mode_config->cursor_width = 64;
		mode_config->cursor_height = 64;
	} else {
		mode_config->cursor_width = 256;
		mode_config->cursor_height = 256;
	}
}

static void intel_mode_config_cleanup(struct drm_i915_private *i915)
{
	intel_atomic_global_obj_cleanup(i915);
	drm_mode_config_cleanup(&i915->drm);
}

static void plane_config_fini(struct intel_initial_plane_config *plane_config)
{
	if (plane_config->fb) {
		struct drm_framebuffer *fb = &plane_config->fb->base;

		/* We may only have the stub and not a full framebuffer */
		if (drm_framebuffer_read_refcount(fb))
			drm_framebuffer_put(fb);
		else
			kfree(fb);
	}

	if (plane_config->vma)
		i915_vma_put(plane_config->vma);
}

/* part #1: call before irq install */
int intel_modeset_init_noirq(struct drm_i915_private *i915)
{
	int ret;

	if (i915_inject_probe_failure(i915))
		return -ENODEV;

	if (HAS_DISPLAY(i915)) {
		ret = drm_vblank_init(&i915->drm,
				      INTEL_NUM_PIPES(i915));
		if (ret)
			return ret;
	}

	intel_bios_init(i915);

	ret = intel_vga_register(i915);
	if (ret)
		goto cleanup_bios;

	/* FIXME: completely on the wrong abstraction layer */
	intel_power_domains_init_hw(i915, false);

	if (!HAS_DISPLAY(i915))
		return 0;

	intel_dmc_ucode_init(i915);

	i915->modeset_wq = alloc_ordered_workqueue("i915_modeset", 0);
	i915->flip_wq = alloc_workqueue("i915_flip", WQ_HIGHPRI |
					WQ_UNBOUND, WQ_UNBOUND_MAX_ACTIVE);

	i915->framestart_delay = 1; /* 1-4 */

	i915->window2_delay = 0; /* No DSB so no window2 delay */

	intel_mode_config_init(i915);

	ret = intel_cdclk_init(i915);
	if (ret)
		goto cleanup_vga_client_pw_domain_dmc;

	ret = intel_dbuf_init(i915);
	if (ret)
		goto cleanup_vga_client_pw_domain_dmc;

	ret = intel_bw_init(i915);
	if (ret)
		goto cleanup_vga_client_pw_domain_dmc;

	init_llist_head(&i915->atomic_helper.free_list);
	INIT_WORK(&i915->atomic_helper.free_work,
		  intel_atomic_helper_free_state_worker);

	intel_init_quirks(i915);

	intel_fbc_init(i915);

	return 0;

cleanup_vga_client_pw_domain_dmc:
	intel_dmc_ucode_fini(i915);
	intel_power_domains_driver_remove(i915);
	intel_vga_unregister(i915);
cleanup_bios:
	intel_bios_driver_remove(i915);

	return ret;
}

/* part #2: call after irq install, but before gem init */
int intel_modeset_init_nogem(struct drm_i915_private *i915)
{
	struct drm_device *dev = &i915->drm;
	enum pipe pipe;
	struct intel_crtc *crtc;
	int ret;

	if (!HAS_DISPLAY(i915))
		return 0;

	intel_init_pm(i915);

	intel_panel_sanitize_ssc(i915);

	intel_pps_setup(i915);

	intel_gmbus_setup(i915);

	drm_dbg_kms(&i915->drm, "%d display pipe%s available.\n",
		    INTEL_NUM_PIPES(i915),
		    INTEL_NUM_PIPES(i915) > 1 ? "s" : "");

	for_each_pipe(i915, pipe) {
		ret = intel_crtc_init(i915, pipe);
		if (ret) {
			intel_mode_config_cleanup(i915);
			return ret;
		}
	}

	intel_plane_possible_crtcs_init(i915);
	intel_shared_dpll_init(dev);
	intel_update_fdi_pll_freq(i915);

	intel_update_czclk(i915);
	intel_modeset_init_hw(i915);
	intel_dpll_update_ref_clks(i915);

	intel_hdcp_component_init(i915);

	if (i915->max_cdclk_freq == 0)
		intel_update_max_cdclk(i915);

	/*
	 * If the platform has HTI, we need to find out whether it has reserved
	 * any display resources before we create our display outputs.
	 */
	if (INTEL_INFO(i915)->display.has_hti)
		i915->hti_state = intel_de_read(i915, HDPORT_STATE);

	/* Just disable it once at startup */
	intel_vga_disable(i915);
	intel_setup_outputs(i915);

	drm_modeset_lock_all(dev);
	intel_modeset_setup_hw_state(dev, dev->mode_config.acquire_ctx);
	drm_modeset_unlock_all(dev);

	for_each_intel_crtc(dev, crtc) {
		struct intel_initial_plane_config plane_config = {};

		if (!to_intel_crtc_state(crtc->base.state)->uapi.active)
			continue;

		/*
		 * Note that reserving the BIOS fb up front prevents us
		 * from stuffing other stolen allocations like the ring
		 * on top.  This prevents some ugliness at boot time, and
		 * can even allow for smooth boot transitions if the BIOS
		 * fb is large enough for the active pipe configuration.
		 */
		i915->display.get_initial_plane_config(crtc, &plane_config);

		/*
		 * If the fb is shared between multiple heads, we'll
		 * just get the first one.
		 */
		intel_find_initial_plane_obj(crtc, &plane_config);

		plane_config_fini(&plane_config);
	}

	/*
	 * Make sure hardware watermarks really match the state we read out.
	 * Note that we need to do this after reconstructing the BIOS fb's
	 * since the watermark calculation done here will use pstate->fb.
	 */
	if (!HAS_GMCH(i915))
		sanitize_watermarks(i915);

	return 0;
}

/* part #3: call after gem init */
int intel_modeset_init(struct drm_i915_private *i915)
{
	int ret;

	if (!HAS_DISPLAY(i915))
		return 0;

	/*
	 * Force all active planes to recompute their states. So that on
	 * mode_setcrtc after probe, all the intel_plane_state variables
	 * are already calculated and there is no assert_plane warnings
	 * during bootup.
	 */
	ret = intel_initial_commit(&i915->drm);
	if (ret)
		drm_dbg_kms(&i915->drm, "Initial modeset failed, %d\n", ret);

	intel_overlay_setup(i915);

	ret = intel_fbdev_init(&i915->drm);
	if (ret)
		return ret;

	/* Only enable hotplug handling once the fbdev is fully set up. */
	intel_hpd_init(i915);
	intel_hpd_poll_disable(i915);

	intel_init_ipc(i915);

	return 0;
}

void i830_enable_pipe(struct drm_i915_private *dev_priv, enum pipe pipe)
{
	struct intel_crtc *crtc = intel_get_crtc_for_pipe(dev_priv, pipe);
	/* 640x480@60Hz, ~25175 kHz */
	struct dpll clock = {
		.m1 = 18,
		.m2 = 7,
		.p1 = 13,
		.p2 = 4,
		.n = 2,
	};
	u32 dpll, fp;
	int i;

	drm_WARN_ON(&dev_priv->drm,
		    i9xx_calc_dpll_params(48000, &clock) != 25154);

	drm_dbg_kms(&dev_priv->drm,
		    "enabling pipe %c due to force quirk (vco=%d dot=%d)\n",
		    pipe_name(pipe), clock.vco, clock.dot);

	fp = i9xx_dpll_compute_fp(&clock);
	dpll = DPLL_DVO_2X_MODE |
		DPLL_VGA_MODE_DIS |
		((clock.p1 - 2) << DPLL_FPA01_P1_POST_DIV_SHIFT) |
		PLL_P2_DIVIDE_BY_4 |
		PLL_REF_INPUT_DREFCLK |
		DPLL_VCO_ENABLE;

	intel_de_write(dev_priv, FP0(pipe), fp);
	intel_de_write(dev_priv, FP1(pipe), fp);

	intel_de_write(dev_priv, HTOTAL(pipe), (640 - 1) | ((800 - 1) << 16));
	intel_de_write(dev_priv, HBLANK(pipe), (640 - 1) | ((800 - 1) << 16));
	intel_de_write(dev_priv, HSYNC(pipe), (656 - 1) | ((752 - 1) << 16));
	intel_de_write(dev_priv, VTOTAL(pipe), (480 - 1) | ((525 - 1) << 16));
	intel_de_write(dev_priv, VBLANK(pipe), (480 - 1) | ((525 - 1) << 16));
	intel_de_write(dev_priv, VSYNC(pipe), (490 - 1) | ((492 - 1) << 16));
	intel_de_write(dev_priv, PIPESRC(pipe), ((640 - 1) << 16) | (480 - 1));

	/*
	 * Apparently we need to have VGA mode enabled prior to changing
	 * the P1/P2 dividers. Otherwise the DPLL will keep using the old
	 * dividers, even though the register value does change.
	 */
	intel_de_write(dev_priv, DPLL(pipe), dpll & ~DPLL_VGA_MODE_DIS);
	intel_de_write(dev_priv, DPLL(pipe), dpll);

	/* Wait for the clocks to stabilize. */
	intel_de_posting_read(dev_priv, DPLL(pipe));
	udelay(150);

	/* The pixel multiplier can only be updated once the
	 * DPLL is enabled and the clocks are stable.
	 *
	 * So write it again.
	 */
	intel_de_write(dev_priv, DPLL(pipe), dpll);

	/* We do this three times for luck */
	for (i = 0; i < 3 ; i++) {
		intel_de_write(dev_priv, DPLL(pipe), dpll);
		intel_de_posting_read(dev_priv, DPLL(pipe));
		udelay(150); /* wait for warmup */
	}

	intel_de_write(dev_priv, PIPECONF(pipe),
		       PIPECONF_ENABLE | PIPECONF_PROGRESSIVE);
	intel_de_posting_read(dev_priv, PIPECONF(pipe));

	intel_wait_for_pipe_scanline_moving(crtc);
}

void i830_disable_pipe(struct drm_i915_private *dev_priv, enum pipe pipe)
{
	struct intel_crtc *crtc = intel_get_crtc_for_pipe(dev_priv, pipe);

	drm_dbg_kms(&dev_priv->drm, "disabling pipe %c due to force quirk\n",
		    pipe_name(pipe));

	drm_WARN_ON(&dev_priv->drm,
		    intel_de_read(dev_priv, DSPCNTR(PLANE_A)) &
		    DISPLAY_PLANE_ENABLE);
	drm_WARN_ON(&dev_priv->drm,
		    intel_de_read(dev_priv, DSPCNTR(PLANE_B)) &
		    DISPLAY_PLANE_ENABLE);
	drm_WARN_ON(&dev_priv->drm,
		    intel_de_read(dev_priv, DSPCNTR(PLANE_C)) &
		    DISPLAY_PLANE_ENABLE);
	drm_WARN_ON(&dev_priv->drm,
		    intel_de_read(dev_priv, CURCNTR(PIPE_A)) & MCURSOR_MODE);
	drm_WARN_ON(&dev_priv->drm,
		    intel_de_read(dev_priv, CURCNTR(PIPE_B)) & MCURSOR_MODE);

	intel_de_write(dev_priv, PIPECONF(pipe), 0);
	intel_de_posting_read(dev_priv, PIPECONF(pipe));

	intel_wait_for_pipe_scanline_stopped(crtc);

	intel_de_write(dev_priv, DPLL(pipe), DPLL_VGA_MODE_DIS);
	intel_de_posting_read(dev_priv, DPLL(pipe));
}

static void
intel_sanitize_plane_mapping(struct drm_i915_private *dev_priv)
{
	struct intel_crtc *crtc;

	if (DISPLAY_VER(dev_priv) >= 4)
		return;

	for_each_intel_crtc(&dev_priv->drm, crtc) {
		struct intel_plane *plane =
			to_intel_plane(crtc->base.primary);
		struct intel_crtc *plane_crtc;
		enum pipe pipe;

		if (!plane->get_hw_state(plane, &pipe))
			continue;

		if (pipe == crtc->pipe)
			continue;

		drm_dbg_kms(&dev_priv->drm,
			    "[PLANE:%d:%s] attached to the wrong pipe, disabling plane\n",
			    plane->base.base.id, plane->base.name);

		plane_crtc = intel_get_crtc_for_pipe(dev_priv, pipe);
		intel_plane_disable_noatomic(plane_crtc, plane);
	}
}

static bool intel_crtc_has_encoders(struct intel_crtc *crtc)
{
	struct drm_device *dev = crtc->base.dev;
	struct intel_encoder *encoder;

	for_each_encoder_on_crtc(dev, &crtc->base, encoder)
		return true;

	return false;
}

static struct intel_connector *intel_encoder_find_connector(struct intel_encoder *encoder)
{
	struct drm_device *dev = encoder->base.dev;
	struct intel_connector *connector;

	for_each_connector_on_encoder(dev, &encoder->base, connector)
		return connector;

	return NULL;
}

static bool has_pch_trancoder(struct drm_i915_private *dev_priv,
			      enum pipe pch_transcoder)
{
	return HAS_PCH_IBX(dev_priv) || HAS_PCH_CPT(dev_priv) ||
		(HAS_PCH_LPT_H(dev_priv) && pch_transcoder == PIPE_A);
}

static void intel_sanitize_frame_start_delay(const struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->uapi.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	enum transcoder cpu_transcoder = crtc_state->cpu_transcoder;

	if (DISPLAY_VER(dev_priv) >= 9 ||
	    IS_BROADWELL(dev_priv) || IS_HASWELL(dev_priv)) {
		i915_reg_t reg = CHICKEN_TRANS(cpu_transcoder);
		u32 val;

		if (transcoder_is_dsi(cpu_transcoder))
			return;

		val = intel_de_read(dev_priv, reg);
		val &= ~HSW_FRAME_START_DELAY_MASK;
		val |= HSW_FRAME_START_DELAY(dev_priv->framestart_delay - 1);
		intel_de_write(dev_priv, reg, val);
	} else {
		i915_reg_t reg = PIPECONF(cpu_transcoder);
		u32 val;

		val = intel_de_read(dev_priv, reg);
		val &= ~PIPECONF_FRAME_START_DELAY_MASK;
		val |= PIPECONF_FRAME_START_DELAY(dev_priv->framestart_delay - 1);
		intel_de_write(dev_priv, reg, val);
	}

	if (!crtc_state->has_pch_encoder)
		return;

	if (HAS_PCH_IBX(dev_priv)) {
		i915_reg_t reg = PCH_TRANSCONF(crtc->pipe);
		u32 val;

		val = intel_de_read(dev_priv, reg);
		val &= ~TRANS_FRAME_START_DELAY_MASK;
		val |= TRANS_FRAME_START_DELAY(dev_priv->framestart_delay - 1);
		intel_de_write(dev_priv, reg, val);
	} else {
		enum pipe pch_transcoder = intel_crtc_pch_transcoder(crtc);
		i915_reg_t reg = TRANS_CHICKEN2(pch_transcoder);
		u32 val;

		val = intel_de_read(dev_priv, reg);
		val &= ~TRANS_CHICKEN2_FRAME_START_DELAY_MASK;
		val |= TRANS_CHICKEN2_FRAME_START_DELAY(dev_priv->framestart_delay - 1);
		intel_de_write(dev_priv, reg, val);
	}
}

static void intel_sanitize_crtc(struct intel_crtc *crtc,
				struct drm_modeset_acquire_ctx *ctx)
{
	struct drm_device *dev = crtc->base.dev;
	struct drm_i915_private *dev_priv = to_i915(dev);
	struct intel_crtc_state *crtc_state = to_intel_crtc_state(crtc->base.state);

	if (crtc_state->hw.active) {
		struct intel_plane *plane;

		/* Clear any frame start delays used for debugging left by the BIOS */
		intel_sanitize_frame_start_delay(crtc_state);

		/* Disable everything but the primary plane */
		for_each_intel_plane_on_crtc(dev, crtc, plane) {
			const struct intel_plane_state *plane_state =
				to_intel_plane_state(plane->base.state);

			if (plane_state->uapi.visible &&
			    plane->base.type != DRM_PLANE_TYPE_PRIMARY)
				intel_plane_disable_noatomic(crtc, plane);
		}

		/*
		 * Disable any background color set by the BIOS, but enable the
		 * gamma and CSC to match how we program our planes.
		 */
		if (DISPLAY_VER(dev_priv) >= 9)
			intel_de_write(dev_priv, SKL_BOTTOM_COLOR(crtc->pipe),
				       SKL_BOTTOM_COLOR_GAMMA_ENABLE | SKL_BOTTOM_COLOR_CSC_ENABLE);
	}

	/* Adjust the state of the output pipe according to whether we
	 * have active connectors/encoders. */
	if (crtc_state->hw.active && !intel_crtc_has_encoders(crtc) &&
	    !crtc_state->bigjoiner_slave)
		intel_crtc_disable_noatomic(crtc, ctx);

	if (crtc_state->hw.active || HAS_GMCH(dev_priv)) {
		/*
		 * We start out with underrun reporting disabled to avoid races.
		 * For correct bookkeeping mark this on active crtcs.
		 *
		 * Also on gmch platforms we dont have any hardware bits to
		 * disable the underrun reporting. Which means we need to start
		 * out with underrun reporting disabled also on inactive pipes,
		 * since otherwise we'll complain about the garbage we read when
		 * e.g. coming up after runtime pm.
		 *
		 * No protection against concurrent access is required - at
		 * worst a fifo underrun happens which also sets this to false.
		 */
		crtc->cpu_fifo_underrun_disabled = true;
		/*
		 * We track the PCH trancoder underrun reporting state
		 * within the crtc. With crtc for pipe A housing the underrun
		 * reporting state for PCH transcoder A, crtc for pipe B housing
		 * it for PCH transcoder B, etc. LPT-H has only PCH transcoder A,
		 * and marking underrun reporting as disabled for the non-existing
		 * PCH transcoders B and C would prevent enabling the south
		 * error interrupt (see cpt_can_enable_serr_int()).
		 */
		if (has_pch_trancoder(dev_priv, crtc->pipe))
			crtc->pch_fifo_underrun_disabled = true;
	}
}

static bool has_bogus_dpll_config(const struct intel_crtc_state *crtc_state)
{
	struct drm_i915_private *dev_priv = to_i915(crtc_state->uapi.crtc->dev);

	/*
	 * Some SNB BIOSen (eg. ASUS K53SV) are known to misprogram
	 * the hardware when a high res displays plugged in. DPLL P
	 * divider is zero, and the pipe timings are bonkers. We'll
	 * try to disable everything in that case.
	 *
	 * FIXME would be nice to be able to sanitize this state
	 * without several WARNs, but for now let's take the easy
	 * road.
	 */
	return IS_SANDYBRIDGE(dev_priv) &&
		crtc_state->hw.active &&
		crtc_state->shared_dpll &&
		crtc_state->port_clock == 0;
}

static void intel_sanitize_encoder(struct intel_encoder *encoder)
{
	struct drm_i915_private *dev_priv = to_i915(encoder->base.dev);
	struct intel_connector *connector;
	struct intel_crtc *crtc = to_intel_crtc(encoder->base.crtc);
	struct intel_crtc_state *crtc_state = crtc ?
		to_intel_crtc_state(crtc->base.state) : NULL;

	/* We need to check both for a crtc link (meaning that the
	 * encoder is active and trying to read from a pipe) and the
	 * pipe itself being active. */
	bool has_active_crtc = crtc_state &&
		crtc_state->hw.active;

	if (crtc_state && has_bogus_dpll_config(crtc_state)) {
		drm_dbg_kms(&dev_priv->drm,
			    "BIOS has misprogrammed the hardware. Disabling pipe %c\n",
			    pipe_name(crtc->pipe));
		has_active_crtc = false;
	}

	connector = intel_encoder_find_connector(encoder);
	if (connector && !has_active_crtc) {
		drm_dbg_kms(&dev_priv->drm,
			    "[ENCODER:%d:%s] has active connectors but no active pipe!\n",
			    encoder->base.base.id,
			    encoder->base.name);

		/* Connector is active, but has no active pipe. This is
		 * fallout from our resume register restoring. Disable
		 * the encoder manually again. */
		if (crtc_state) {
			struct drm_encoder *best_encoder;

			drm_dbg_kms(&dev_priv->drm,
				    "[ENCODER:%d:%s] manually disabled\n",
				    encoder->base.base.id,
				    encoder->base.name);

			/* avoid oopsing in case the hooks consult best_encoder */
			best_encoder = connector->base.state->best_encoder;
			connector->base.state->best_encoder = &encoder->base;

			/* FIXME NULL atomic state passed! */
			if (encoder->disable)
				encoder->disable(NULL, encoder, crtc_state,
						 connector->base.state);
			if (encoder->post_disable)
				encoder->post_disable(NULL, encoder, crtc_state,
						      connector->base.state);

			connector->base.state->best_encoder = best_encoder;
		}
		encoder->base.crtc = NULL;

		/* Inconsistent output/port/pipe state happens presumably due to
		 * a bug in one of the get_hw_state functions. Or someplace else
		 * in our code, like the register restore mess on resume. Clamp
		 * things to off as a safer default. */

		connector->base.dpms = DRM_MODE_DPMS_OFF;
		connector->base.encoder = NULL;
	}

	/* notify opregion of the sanitized encoder state */
	intel_opregion_notify_encoder(encoder, connector && has_active_crtc);

	if (HAS_DDI(dev_priv))
		intel_ddi_sanitize_encoder_pll_mapping(encoder);
}

/* FIXME read out full plane state for all planes */
static void readout_plane_state(struct drm_i915_private *dev_priv)
{
	struct intel_plane *plane;
	struct intel_crtc *crtc;

	for_each_intel_plane(&dev_priv->drm, plane) {
		struct intel_plane_state *plane_state =
			to_intel_plane_state(plane->base.state);
		struct intel_crtc_state *crtc_state;
		enum pipe pipe = PIPE_A;
		bool visible;

		visible = plane->get_hw_state(plane, &pipe);

		crtc = intel_get_crtc_for_pipe(dev_priv, pipe);
		crtc_state = to_intel_crtc_state(crtc->base.state);

		intel_set_plane_visible(crtc_state, plane_state, visible);

		drm_dbg_kms(&dev_priv->drm,
			    "[PLANE:%d:%s] hw state readout: %s, pipe %c\n",
			    plane->base.base.id, plane->base.name,
			    enableddisabled(visible), pipe_name(pipe));
	}

	for_each_intel_crtc(&dev_priv->drm, crtc) {
		struct intel_crtc_state *crtc_state =
			to_intel_crtc_state(crtc->base.state);

		fixup_plane_bitmasks(crtc_state);
	}
}

static void intel_modeset_readout_hw_state(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = to_i915(dev);
	struct intel_cdclk_state *cdclk_state =
		to_intel_cdclk_state(dev_priv->cdclk.obj.state);
	struct intel_dbuf_state *dbuf_state =
		to_intel_dbuf_state(dev_priv->dbuf.obj.state);
	enum pipe pipe;
	struct intel_crtc *crtc;
	struct intel_encoder *encoder;
	struct intel_connector *connector;
	struct drm_connector_list_iter conn_iter;
	u8 active_pipes = 0;

	for_each_intel_crtc(dev, crtc) {
		struct intel_crtc_state *crtc_state =
			to_intel_crtc_state(crtc->base.state);

		__drm_atomic_helper_crtc_destroy_state(&crtc_state->uapi);
		intel_crtc_free_hw_state(crtc_state);
		intel_crtc_state_reset(crtc_state, crtc);

		intel_crtc_get_pipe_config(crtc_state);

		crtc_state->hw.enable = crtc_state->hw.active;

		crtc->base.enabled = crtc_state->hw.enable;
		crtc->active = crtc_state->hw.active;

		if (crtc_state->hw.active)
			active_pipes |= BIT(crtc->pipe);

		drm_dbg_kms(&dev_priv->drm,
			    "[CRTC:%d:%s] hw state readout: %s\n",
			    crtc->base.base.id, crtc->base.name,
			    enableddisabled(crtc_state->hw.active));
	}

	dev_priv->active_pipes = cdclk_state->active_pipes =
		dbuf_state->active_pipes = active_pipes;

	readout_plane_state(dev_priv);

	for_each_intel_encoder(dev, encoder) {
		pipe = 0;

		if (encoder->get_hw_state(encoder, &pipe)) {
			struct intel_crtc_state *crtc_state;

			crtc = intel_get_crtc_for_pipe(dev_priv, pipe);
			crtc_state = to_intel_crtc_state(crtc->base.state);

			encoder->base.crtc = &crtc->base;
			intel_encoder_get_config(encoder, crtc_state);
			if (encoder->sync_state)
				encoder->sync_state(encoder, crtc_state);

			/* read out to slave crtc as well for bigjoiner */
			if (crtc_state->bigjoiner) {
				/* encoder should read be linked to bigjoiner master */
				WARN_ON(crtc_state->bigjoiner_slave);

				crtc = crtc_state->bigjoiner_linked_crtc;
				crtc_state = to_intel_crtc_state(crtc->base.state);
				intel_encoder_get_config(encoder, crtc_state);
			}
		} else {
			encoder->base.crtc = NULL;
		}

		drm_dbg_kms(&dev_priv->drm,
			    "[ENCODER:%d:%s] hw state readout: %s, pipe %c\n",
			    encoder->base.base.id, encoder->base.name,
			    enableddisabled(encoder->base.crtc),
			    pipe_name(pipe));
	}

	intel_dpll_readout_hw_state(dev_priv);

	drm_connector_list_iter_begin(dev, &conn_iter);
	for_each_intel_connector_iter(connector, &conn_iter) {
		if (connector->get_hw_state(connector)) {
			struct intel_crtc_state *crtc_state;
			struct intel_crtc *crtc;

			connector->base.dpms = DRM_MODE_DPMS_ON;

			encoder = intel_attached_encoder(connector);
			connector->base.encoder = &encoder->base;

			crtc = to_intel_crtc(encoder->base.crtc);
			crtc_state = crtc ? to_intel_crtc_state(crtc->base.state) : NULL;

			if (crtc_state && crtc_state->hw.active) {
				/*
				 * This has to be done during hardware readout
				 * because anything calling .crtc_disable may
				 * rely on the connector_mask being accurate.
				 */
				crtc_state->uapi.connector_mask |=
					drm_connector_mask(&connector->base);
				crtc_state->uapi.encoder_mask |=
					drm_encoder_mask(&encoder->base);
			}
		} else {
			connector->base.dpms = DRM_MODE_DPMS_OFF;
			connector->base.encoder = NULL;
		}
		drm_dbg_kms(&dev_priv->drm,
			    "[CONNECTOR:%d:%s] hw state readout: %s\n",
			    connector->base.base.id, connector->base.name,
			    enableddisabled(connector->base.encoder));
	}
	drm_connector_list_iter_end(&conn_iter);

	for_each_intel_crtc(dev, crtc) {
		struct intel_bw_state *bw_state =
			to_intel_bw_state(dev_priv->bw_obj.state);
		struct intel_crtc_state *crtc_state =
			to_intel_crtc_state(crtc->base.state);
		struct intel_plane *plane;
		int min_cdclk = 0;

		if (crtc_state->bigjoiner_slave)
			continue;

		if (crtc_state->hw.active) {
			/*
			 * The initial mode needs to be set in order to keep
			 * the atomic core happy. It wants a valid mode if the
			 * crtc's enabled, so we do the above call.
			 *
			 * But we don't set all the derived state fully, hence
			 * set a flag to indicate that a full recalculation is
			 * needed on the next commit.
			 */
			crtc_state->inherited = true;

			intel_crtc_update_active_timings(crtc_state);

			intel_crtc_copy_hw_to_uapi_state(crtc_state);
		}

		for_each_intel_plane_on_crtc(&dev_priv->drm, crtc, plane) {
			const struct intel_plane_state *plane_state =
				to_intel_plane_state(plane->base.state);

			/*
			 * FIXME don't have the fb yet, so can't
			 * use intel_plane_data_rate() :(
			 */
			if (plane_state->uapi.visible)
				crtc_state->data_rate[plane->id] =
					4 * crtc_state->pixel_rate;
			/*
			 * FIXME don't have the fb yet, so can't
			 * use plane->min_cdclk() :(
			 */
			if (plane_state->uapi.visible && plane->min_cdclk) {
				if (crtc_state->double_wide || DISPLAY_VER(dev_priv) >= 10)
					crtc_state->min_cdclk[plane->id] =
						DIV_ROUND_UP(crtc_state->pixel_rate, 2);
				else
					crtc_state->min_cdclk[plane->id] =
						crtc_state->pixel_rate;
			}
			drm_dbg_kms(&dev_priv->drm,
				    "[PLANE:%d:%s] min_cdclk %d kHz\n",
				    plane->base.base.id, plane->base.name,
				    crtc_state->min_cdclk[plane->id]);
		}

		if (crtc_state->hw.active) {
			min_cdclk = intel_crtc_compute_min_cdclk(crtc_state);
			if (drm_WARN_ON(dev, min_cdclk < 0))
				min_cdclk = 0;
		}

		cdclk_state->min_cdclk[crtc->pipe] = min_cdclk;
		cdclk_state->min_voltage_level[crtc->pipe] =
			crtc_state->min_voltage_level;

		intel_bw_crtc_update(bw_state, crtc_state);

		intel_pipe_config_sanity_check(dev_priv, crtc_state);

		/* discard our incomplete slave state, copy it from master */
		if (crtc_state->bigjoiner && crtc_state->hw.active) {
			struct intel_crtc *slave = crtc_state->bigjoiner_linked_crtc;
			struct intel_crtc_state *slave_crtc_state =
				to_intel_crtc_state(slave->base.state);

			copy_bigjoiner_crtc_state(slave_crtc_state, crtc_state);
			slave->base.mode = crtc->base.mode;

			cdclk_state->min_cdclk[slave->pipe] = min_cdclk;
			cdclk_state->min_voltage_level[slave->pipe] =
				crtc_state->min_voltage_level;

			for_each_intel_plane_on_crtc(&dev_priv->drm, slave, plane) {
				const struct intel_plane_state *plane_state =
					to_intel_plane_state(plane->base.state);

				/*
				 * FIXME don't have the fb yet, so can't
				 * use intel_plane_data_rate() :(
				 */
				if (plane_state->uapi.visible)
					crtc_state->data_rate[plane->id] =
						4 * crtc_state->pixel_rate;
				else
					crtc_state->data_rate[plane->id] = 0;
			}

			intel_bw_crtc_update(bw_state, slave_crtc_state);
			drm_calc_timestamping_constants(&slave->base,
							&slave_crtc_state->hw.adjusted_mode);
		}
	}
}

static void
get_encoder_power_domains(struct drm_i915_private *dev_priv)
{
	struct intel_encoder *encoder;

	for_each_intel_encoder(&dev_priv->drm, encoder) {
		struct intel_crtc_state *crtc_state;

		if (!encoder->get_power_domains)
			continue;

		/*
		 * MST-primary and inactive encoders don't have a crtc state
		 * and neither of these require any power domain references.
		 */
		if (!encoder->base.crtc)
			continue;

		crtc_state = to_intel_crtc_state(encoder->base.crtc->state);
		encoder->get_power_domains(encoder, crtc_state);
	}
}

static void intel_early_display_was(struct drm_i915_private *dev_priv)
{
	/*
	 * Display WA #1185 WaDisableDARBFClkGating:cnl,glk,icl,ehl,tgl
	 * Also known as Wa_14010480278.
	 */
	if (IS_DISPLAY_VER(dev_priv, 10, 12))
		intel_de_write(dev_priv, GEN9_CLKGATE_DIS_0,
			       intel_de_read(dev_priv, GEN9_CLKGATE_DIS_0) | DARBF_GATING_DIS);

	if (IS_HASWELL(dev_priv)) {
		/*
		 * WaRsPkgCStateDisplayPMReq:hsw
		 * System hang if this isn't done before disabling all planes!
		 */
		intel_de_write(dev_priv, CHICKEN_PAR1_1,
			       intel_de_read(dev_priv, CHICKEN_PAR1_1) | FORCE_ARB_IDLE_PLANES);
	}

	if (IS_KABYLAKE(dev_priv) || IS_COFFEELAKE(dev_priv) || IS_COMETLAKE(dev_priv)) {
		/* Display WA #1142:kbl,cfl,cml */
		intel_de_rmw(dev_priv, CHICKEN_PAR1_1,
			     KBL_ARB_FILL_SPARE_22, KBL_ARB_FILL_SPARE_22);
		intel_de_rmw(dev_priv, CHICKEN_MISC_2,
			     KBL_ARB_FILL_SPARE_13 | KBL_ARB_FILL_SPARE_14,
			     KBL_ARB_FILL_SPARE_14);
	}
}

static void ibx_sanitize_pch_hdmi_port(struct drm_i915_private *dev_priv,
				       enum port port, i915_reg_t hdmi_reg)
{
	u32 val = intel_de_read(dev_priv, hdmi_reg);

	if (val & SDVO_ENABLE ||
	    (val & SDVO_PIPE_SEL_MASK) == SDVO_PIPE_SEL(PIPE_A))
		return;

	drm_dbg_kms(&dev_priv->drm,
		    "Sanitizing transcoder select for HDMI %c\n",
		    port_name(port));

	val &= ~SDVO_PIPE_SEL_MASK;
	val |= SDVO_PIPE_SEL(PIPE_A);

	intel_de_write(dev_priv, hdmi_reg, val);
}

static void ibx_sanitize_pch_dp_port(struct drm_i915_private *dev_priv,
				     enum port port, i915_reg_t dp_reg)
{
	u32 val = intel_de_read(dev_priv, dp_reg);

	if (val & DP_PORT_EN ||
	    (val & DP_PIPE_SEL_MASK) == DP_PIPE_SEL(PIPE_A))
		return;

	drm_dbg_kms(&dev_priv->drm,
		    "Sanitizing transcoder select for DP %c\n",
		    port_name(port));

	val &= ~DP_PIPE_SEL_MASK;
	val |= DP_PIPE_SEL(PIPE_A);

	intel_de_write(dev_priv, dp_reg, val);
}

static void ibx_sanitize_pch_ports(struct drm_i915_private *dev_priv)
{
	/*
	 * The BIOS may select transcoder B on some of the PCH
	 * ports even it doesn't enable the port. This would trip
	 * assert_pch_dp_disabled() and assert_pch_hdmi_disabled().
	 * Sanitize the transcoder select bits to prevent that. We
	 * assume that the BIOS never actually enabled the port,
	 * because if it did we'd actually have to toggle the port
	 * on and back off to make the transcoder A select stick
	 * (see. intel_dp_link_down(), intel_disable_hdmi(),
	 * intel_disable_sdvo()).
	 */
	ibx_sanitize_pch_dp_port(dev_priv, PORT_B, PCH_DP_B);
	ibx_sanitize_pch_dp_port(dev_priv, PORT_C, PCH_DP_C);
	ibx_sanitize_pch_dp_port(dev_priv, PORT_D, PCH_DP_D);

	/* PCH SDVOB multiplex with HDMIB */
	ibx_sanitize_pch_hdmi_port(dev_priv, PORT_B, PCH_HDMIB);
	ibx_sanitize_pch_hdmi_port(dev_priv, PORT_C, PCH_HDMIC);
	ibx_sanitize_pch_hdmi_port(dev_priv, PORT_D, PCH_HDMID);
}

/* Scan out the current hw modeset state,
 * and sanitizes it to the current state
 */
static void
intel_modeset_setup_hw_state(struct drm_device *dev,
			     struct drm_modeset_acquire_ctx *ctx)
{
	struct drm_i915_private *dev_priv = to_i915(dev);
	struct intel_encoder *encoder;
	struct intel_crtc *crtc;
	intel_wakeref_t wakeref;

	wakeref = intel_display_power_get(dev_priv, POWER_DOMAIN_INIT);

	intel_early_display_was(dev_priv);
	intel_modeset_readout_hw_state(dev);

	/* HW state is read out, now we need to sanitize this mess. */

	/* Sanitize the TypeC port mode upfront, encoders depend on this */
	for_each_intel_encoder(dev, encoder) {
		enum phy phy = intel_port_to_phy(dev_priv, encoder->port);

		/* We need to sanitize only the MST primary port. */
		if (encoder->type != INTEL_OUTPUT_DP_MST &&
		    intel_phy_is_tc(dev_priv, phy))
			intel_tc_port_sanitize(enc_to_dig_port(encoder));
	}

	get_encoder_power_domains(dev_priv);

	if (HAS_PCH_IBX(dev_priv))
		ibx_sanitize_pch_ports(dev_priv);

	/*
	 * intel_sanitize_plane_mapping() may need to do vblank
	 * waits, so we need vblank interrupts restored beforehand.
	 */
	for_each_intel_crtc(&dev_priv->drm, crtc) {
		struct intel_crtc_state *crtc_state =
			to_intel_crtc_state(crtc->base.state);

		drm_crtc_vblank_reset(&crtc->base);

		if (crtc_state->hw.active)
			intel_crtc_vblank_on(crtc_state);
	}

	intel_sanitize_plane_mapping(dev_priv);

	for_each_intel_encoder(dev, encoder)
		intel_sanitize_encoder(encoder);

	for_each_intel_crtc(&dev_priv->drm, crtc) {
		struct intel_crtc_state *crtc_state =
			to_intel_crtc_state(crtc->base.state);

		intel_sanitize_crtc(crtc, ctx);
		intel_dump_pipe_config(crtc_state, NULL, "[setup_hw_state]");
	}

	intel_modeset_update_connector_atomic_state(dev);

	intel_dpll_sanitize_state(dev_priv);

	if (IS_G4X(dev_priv)) {
		g4x_wm_get_hw_state(dev_priv);
		g4x_wm_sanitize(dev_priv);
	} else if (IS_VALLEYVIEW(dev_priv) || IS_CHERRYVIEW(dev_priv)) {
		vlv_wm_get_hw_state(dev_priv);
		vlv_wm_sanitize(dev_priv);
	} else if (DISPLAY_VER(dev_priv) >= 9) {
		skl_wm_get_hw_state(dev_priv);
	} else if (HAS_PCH_SPLIT(dev_priv)) {
		ilk_wm_get_hw_state(dev_priv);
	}

	for_each_intel_crtc(dev, crtc) {
		struct intel_crtc_state *crtc_state =
			to_intel_crtc_state(crtc->base.state);
		u64 put_domains;

		put_domains = modeset_get_crtc_power_domains(crtc_state);
		if (drm_WARN_ON(dev, put_domains))
			modeset_put_crtc_power_domains(crtc, put_domains);
	}

	intel_display_power_put(dev_priv, POWER_DOMAIN_INIT, wakeref);
}

void intel_display_resume(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = to_i915(dev);
	struct drm_atomic_state *state = dev_priv->modeset_restore_state;
	struct drm_modeset_acquire_ctx ctx;
	int ret;

	if (!HAS_DISPLAY(dev_priv))
		return;

	dev_priv->modeset_restore_state = NULL;
	if (state)
		state->acquire_ctx = &ctx;

	drm_modeset_acquire_init(&ctx, 0);

	while (1) {
		ret = drm_modeset_lock_all_ctx(dev, &ctx);
		if (ret != -EDEADLK)
			break;

		drm_modeset_backoff(&ctx);
	}

	if (!ret)
		ret = __intel_display_resume(dev, state, &ctx);

	intel_enable_ipc(dev_priv);
	drm_modeset_drop_locks(&ctx);
	drm_modeset_acquire_fini(&ctx);

	if (ret)
		drm_err(&dev_priv->drm,
			"Restoring old state failed with %i\n", ret);
	if (state)
		drm_atomic_state_put(state);
}

static void intel_hpd_poll_fini(struct drm_i915_private *i915)
{
	struct intel_connector *connector;
	struct drm_connector_list_iter conn_iter;

	/* Kill all the work that may have been queued by hpd. */
	drm_connector_list_iter_begin(&i915->drm, &conn_iter);
	for_each_intel_connector_iter(connector, &conn_iter) {
		if (connector->modeset_retry_work.func)
			cancel_work_sync(&connector->modeset_retry_work);
		if (connector->hdcp.shim) {
			cancel_delayed_work_sync(&connector->hdcp.check_work);
			cancel_work_sync(&connector->hdcp.prop_work);
		}
	}
	drm_connector_list_iter_end(&conn_iter);
}

/* part #1: call before irq uninstall */
void intel_modeset_driver_remove(struct drm_i915_private *i915)
{
	if (!HAS_DISPLAY(i915))
		return;

	flush_workqueue(i915->flip_wq);
	flush_workqueue(i915->modeset_wq);

	flush_work(&i915->atomic_helper.free_work);
	drm_WARN_ON(&i915->drm, !llist_empty(&i915->atomic_helper.free_list));
}

/* part #2: call after irq uninstall */
void intel_modeset_driver_remove_noirq(struct drm_i915_private *i915)
{
	if (!HAS_DISPLAY(i915))
		return;

	/*
	 * Due to the hpd irq storm handling the hotplug work can re-arm the
	 * poll handlers. Hence disable polling after hpd handling is shut down.
	 */
	intel_hpd_poll_fini(i915);

	/*
	 * MST topology needs to be suspended so we don't have any calls to
	 * fbdev after it's finalized. MST will be destroyed later as part of
	 * drm_mode_config_cleanup()
	 */
	intel_dp_mst_suspend(i915);

	/* poll work can call into fbdev, hence clean that up afterwards */
	intel_fbdev_fini(i915);

	intel_unregister_dsm_handler();

	intel_fbc_global_disable(i915);

	/* flush any delayed tasks or pending work */
	flush_scheduled_work();

	intel_hdcp_component_fini(i915);

	intel_mode_config_cleanup(i915);

	intel_overlay_cleanup(i915);

	intel_gmbus_teardown(i915);

	destroy_workqueue(i915->flip_wq);
	destroy_workqueue(i915->modeset_wq);

	intel_fbc_cleanup_cfb(i915);
}

/* part #3: call after gem init */
void intel_modeset_driver_remove_nogem(struct drm_i915_private *i915)
{
	intel_dmc_ucode_fini(i915);

	intel_power_domains_driver_remove(i915);

	intel_vga_unregister(i915);

	intel_bios_driver_remove(i915);
}

void intel_display_driver_register(struct drm_i915_private *i915)
{
	if (!HAS_DISPLAY(i915))
		return;

	intel_display_debugfs_register(i915);

	/* Must be done after probing outputs */
	intel_opregion_register(i915);
	acpi_video_register();

	intel_audio_init(i915);

	/*
	 * Some ports require correctly set-up hpd registers for
	 * detection to work properly (leading to ghost connected
	 * connector status), e.g. VGA on gm45.  Hence we can only set
	 * up the initial fbdev config after hpd irqs are fully
	 * enabled. We do it last so that the async config cannot run
	 * before the connectors are registered.
	 */
	intel_fbdev_initial_config_async(&i915->drm);

	/*
	 * We need to coordinate the hotplugs with the asynchronous
	 * fbdev configuration, for which we use the
	 * fbdev->async_cookie.
	 */
	drm_kms_helper_poll_init(&i915->drm);
}

void intel_display_driver_unregister(struct drm_i915_private *i915)
{
	if (!HAS_DISPLAY(i915))
		return;

	intel_fbdev_unregister(i915);
	intel_audio_deinit(i915);

	/*
	 * After flushing the fbdev (incl. a late async config which
	 * will have delayed queuing of a hotplug event), then flush
	 * the hotplug events.
	 */
	drm_kms_helper_poll_fini(&i915->drm);
	drm_atomic_helper_shutdown(&i915->drm);

	acpi_video_unregister();
	intel_opregion_unregister(i915);
}
