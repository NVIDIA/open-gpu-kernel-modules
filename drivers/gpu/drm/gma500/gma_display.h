/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright © 2006-2011 Intel Corporation
 *
 * Authors:
 *	Eric Anholt <eric@anholt.net>
 *	Patrik Jakobsson <patrik.r.jakobsson@gmail.com>
 */

#ifndef _GMA_DISPLAY_H_
#define _GMA_DISPLAY_H_

#include <linux/pm_runtime.h>
#include <drm/drm_vblank.h>

struct drm_encoder;
struct drm_mode_set;

struct gma_clock_t {
	/* given values */
	int n;
	int m1, m2;
	int p1, p2;
	/* derived values */
	int dot;
	int vco;
	int m;
	int p;
};

struct gma_range_t {
	int min, max;
};

struct gma_p2_t {
	int dot_limit;
	int p2_slow, p2_fast;
};

struct gma_limit_t {
	struct gma_range_t dot, vco, n, m, m1, m2, p, p1;
	struct gma_p2_t p2;
	bool (*find_pll)(const struct gma_limit_t *, struct drm_crtc *,
			 int target, int refclk,
			 struct gma_clock_t *best_clock);
};

struct gma_clock_funcs {
	void (*clock)(int refclk, struct gma_clock_t *clock);
	const struct gma_limit_t *(*limit)(struct drm_crtc *crtc, int refclk);
	bool (*pll_is_valid)(struct drm_crtc *crtc,
			     const struct gma_limit_t *limit,
			     struct gma_clock_t *clock);
};

/* Common pipe related functions */
extern bool gma_pipe_has_type(struct drm_crtc *crtc, int type);
extern void gma_wait_for_vblank(struct drm_device *dev);
extern int gma_pipe_set_base(struct drm_crtc *crtc, int x, int y,
			     struct drm_framebuffer *old_fb);
extern int gma_crtc_cursor_set(struct drm_crtc *crtc,
			       struct drm_file *file_priv,
			       uint32_t handle,
			       uint32_t width, uint32_t height);
extern int gma_crtc_cursor_move(struct drm_crtc *crtc, int x, int y);
extern void gma_crtc_load_lut(struct drm_crtc *crtc);
extern int gma_crtc_gamma_set(struct drm_crtc *crtc, u16 *red, u16 *green,
			      u16 *blue, u32 size,
			      struct drm_modeset_acquire_ctx *ctx);
extern void gma_crtc_dpms(struct drm_crtc *crtc, int mode);
extern void gma_crtc_prepare(struct drm_crtc *crtc);
extern void gma_crtc_commit(struct drm_crtc *crtc);
extern void gma_crtc_disable(struct drm_crtc *crtc);
extern void gma_crtc_destroy(struct drm_crtc *crtc);
extern int gma_crtc_page_flip(struct drm_crtc *crtc,
			      struct drm_framebuffer *fb,
			      struct drm_pending_vblank_event *event,
			      uint32_t page_flip_flags,
			      struct drm_modeset_acquire_ctx *ctx);
extern int gma_crtc_set_config(struct drm_mode_set *set,
			       struct drm_modeset_acquire_ctx *ctx);

extern void gma_crtc_save(struct drm_crtc *crtc);
extern void gma_crtc_restore(struct drm_crtc *crtc);

extern void gma_encoder_prepare(struct drm_encoder *encoder);
extern void gma_encoder_commit(struct drm_encoder *encoder);
extern void gma_encoder_destroy(struct drm_encoder *encoder);

/* Common clock related functions */
extern const struct gma_limit_t *gma_limit(struct drm_crtc *crtc, int refclk);
extern void gma_clock(int refclk, struct gma_clock_t *clock);
extern bool gma_pll_is_valid(struct drm_crtc *crtc,
			     const struct gma_limit_t *limit,
			     struct gma_clock_t *clock);
extern bool gma_find_best_pll(const struct gma_limit_t *limit,
			      struct drm_crtc *crtc, int target, int refclk,
			      struct gma_clock_t *best_clock);
#endif
