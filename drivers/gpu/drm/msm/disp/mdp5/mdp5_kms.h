/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2013 Red Hat
 * Author: Rob Clark <robdclark@gmail.com>
 */

#ifndef __MDP5_KMS_H__
#define __MDP5_KMS_H__

#include "msm_drv.h"
#include "msm_kms.h"
#include "disp/mdp_kms.h"
#include "mdp5_cfg.h"	/* must be included before mdp5.xml.h */
#include "mdp5.xml.h"
#include "mdp5_pipe.h"
#include "mdp5_mixer.h"
#include "mdp5_ctl.h"
#include "mdp5_smp.h"

struct mdp5_kms {
	struct mdp_kms base;

	struct drm_device *dev;

	struct platform_device *pdev;

	unsigned num_hwpipes;
	struct mdp5_hw_pipe *hwpipes[SSPP_MAX];

	unsigned num_hwmixers;
	struct mdp5_hw_mixer *hwmixers[8];

	unsigned num_intfs;
	struct mdp5_interface *intfs[5];

	struct mdp5_cfg_handler *cfg;
	uint32_t caps;	/* MDP capabilities (MDP_CAP_XXX bits) */

	/*
	 * Global private object state, Do not access directly, use
	 * mdp5_global_get_state()
	 */
	struct drm_modeset_lock glob_state_lock;
	struct drm_private_obj glob_state;

	struct mdp5_smp *smp;
	struct mdp5_ctl_manager *ctlm;

	/* io/register spaces: */
	void __iomem *mmio;

	struct clk *axi_clk;
	struct clk *ahb_clk;
	struct clk *core_clk;
	struct clk *lut_clk;
	struct clk *tbu_clk;
	struct clk *tbu_rt_clk;
	struct clk *vsync_clk;

	/*
	 * lock to protect access to global resources: ie., following register:
	 *	- REG_MDP5_DISP_INTF_SEL
	 */
	spinlock_t resource_lock;

	bool rpm_enabled;

	struct mdp_irq error_handler;

	int enable_count;
};
#define to_mdp5_kms(x) container_of(x, struct mdp5_kms, base)

/* Global private object state for tracking resources that are shared across
 * multiple kms objects (planes/crtcs/etc).
 */
#define to_mdp5_global_state(x) container_of(x, struct mdp5_global_state, base)
struct mdp5_global_state {
	struct drm_private_state base;

	struct drm_atomic_state *state;
	struct mdp5_kms *mdp5_kms;

	struct mdp5_hw_pipe_state hwpipe;
	struct mdp5_hw_mixer_state hwmixer;
	struct mdp5_smp_state smp;
};

struct mdp5_global_state * mdp5_get_existing_global_state(struct mdp5_kms *mdp5_kms);
struct mdp5_global_state *__must_check mdp5_get_global_state(struct drm_atomic_state *s);

/* Atomic plane state.  Subclasses the base drm_plane_state in order to
 * track assigned hwpipe and hw specific state.
 */
struct mdp5_plane_state {
	struct drm_plane_state base;

	struct mdp5_hw_pipe *hwpipe;
	struct mdp5_hw_pipe *r_hwpipe;	/* right hwpipe */

	/* assigned by crtc blender */
	enum mdp_mixer_stage_id stage;
};
#define to_mdp5_plane_state(x) \
		container_of(x, struct mdp5_plane_state, base)

struct mdp5_pipeline {
	struct mdp5_interface *intf;
	struct mdp5_hw_mixer *mixer;
	struct mdp5_hw_mixer *r_mixer;	/* right mixer */
};

struct mdp5_crtc_state {
	struct drm_crtc_state base;

	struct mdp5_ctl *ctl;
	struct mdp5_pipeline pipeline;

	/* these are derivatives of intf/mixer state in mdp5_pipeline */
	u32 vblank_irqmask;
	u32 err_irqmask;
	u32 pp_done_irqmask;

	bool cmd_mode;

	/* should we not write CTL[n].START register on flush?  If the
	 * encoder has changed this is set to true, since encoder->enable()
	 * is called after crtc state is committed, but we only want to
	 * write the CTL[n].START register once.  This lets us defer
	 * writing CTL[n].START until encoder->enable()
	 */
	bool defer_start;
};
#define to_mdp5_crtc_state(x) \
		container_of(x, struct mdp5_crtc_state, base)

enum mdp5_intf_mode {
	MDP5_INTF_MODE_NONE = 0,

	/* Modes used for DSI interface (INTF_DSI type): */
	MDP5_INTF_DSI_MODE_VIDEO,
	MDP5_INTF_DSI_MODE_COMMAND,

	/* Modes used for WB interface (INTF_WB type):  */
	MDP5_INTF_WB_MODE_BLOCK,
	MDP5_INTF_WB_MODE_LINE,
};

struct mdp5_interface {
	int idx;
	int num; /* display interface number */
	enum mdp5_intf_type type;
	enum mdp5_intf_mode mode;
};

struct mdp5_encoder {
	struct drm_encoder base;
	spinlock_t intf_lock;	/* protect REG_MDP5_INTF_* registers */
	bool enabled;
	uint32_t bsc;

	struct mdp5_interface *intf;
	struct mdp5_ctl *ctl;
};
#define to_mdp5_encoder(x) container_of(x, struct mdp5_encoder, base)

static inline void mdp5_write(struct mdp5_kms *mdp5_kms, u32 reg, u32 data)
{
	WARN_ON(mdp5_kms->enable_count <= 0);
	msm_writel(data, mdp5_kms->mmio + reg);
}

static inline u32 mdp5_read(struct mdp5_kms *mdp5_kms, u32 reg)
{
	WARN_ON(mdp5_kms->enable_count <= 0);
	return msm_readl(mdp5_kms->mmio + reg);
}

static inline const char *stage2name(enum mdp_mixer_stage_id stage)
{
	static const char *names[] = {
#define NAME(n) [n] = #n
		NAME(STAGE_UNUSED), NAME(STAGE_BASE),
		NAME(STAGE0), NAME(STAGE1), NAME(STAGE2),
		NAME(STAGE3), NAME(STAGE4), NAME(STAGE6),
#undef NAME
	};
	return names[stage];
}

static inline const char *pipe2name(enum mdp5_pipe pipe)
{
	static const char *names[] = {
#define NAME(n) [SSPP_ ## n] = #n
		NAME(VIG0), NAME(VIG1), NAME(VIG2),
		NAME(RGB0), NAME(RGB1), NAME(RGB2),
		NAME(DMA0), NAME(DMA1),
		NAME(VIG3), NAME(RGB3),
		NAME(CURSOR0), NAME(CURSOR1),
#undef NAME
	};
	return names[pipe];
}

static inline int pipe2nclients(enum mdp5_pipe pipe)
{
	switch (pipe) {
	case SSPP_RGB0:
	case SSPP_RGB1:
	case SSPP_RGB2:
	case SSPP_RGB3:
		return 1;
	default:
		return 3;
	}
}

static inline uint32_t intf2err(int intf_num)
{
	switch (intf_num) {
	case 0:  return MDP5_IRQ_INTF0_UNDER_RUN;
	case 1:  return MDP5_IRQ_INTF1_UNDER_RUN;
	case 2:  return MDP5_IRQ_INTF2_UNDER_RUN;
	case 3:  return MDP5_IRQ_INTF3_UNDER_RUN;
	default: return 0;
	}
}

static inline uint32_t intf2vblank(struct mdp5_hw_mixer *mixer,
				   struct mdp5_interface *intf)
{
	/*
	 * In case of DSI Command Mode, the Ping Pong's read pointer IRQ
	 * acts as a Vblank signal. The Ping Pong buffer used is bound to
	 * layer mixer.
	 */

	if ((intf->type == INTF_DSI) &&
			(intf->mode == MDP5_INTF_DSI_MODE_COMMAND))
		return MDP5_IRQ_PING_PONG_0_RD_PTR << mixer->pp;

	if (intf->type == INTF_WB)
		return MDP5_IRQ_WB_2_DONE;

	switch (intf->num) {
	case 0:  return MDP5_IRQ_INTF0_VSYNC;
	case 1:  return MDP5_IRQ_INTF1_VSYNC;
	case 2:  return MDP5_IRQ_INTF2_VSYNC;
	case 3:  return MDP5_IRQ_INTF3_VSYNC;
	default: return 0;
	}
}

static inline uint32_t lm2ppdone(struct mdp5_hw_mixer *mixer)
{
	return MDP5_IRQ_PING_PONG_0_DONE << mixer->pp;
}

void mdp5_set_irqmask(struct mdp_kms *mdp_kms, uint32_t irqmask,
		uint32_t old_irqmask);
void mdp5_irq_preinstall(struct msm_kms *kms);
int mdp5_irq_postinstall(struct msm_kms *kms);
void mdp5_irq_uninstall(struct msm_kms *kms);
irqreturn_t mdp5_irq(struct msm_kms *kms);
int mdp5_enable_vblank(struct msm_kms *kms, struct drm_crtc *crtc);
void mdp5_disable_vblank(struct msm_kms *kms, struct drm_crtc *crtc);
int mdp5_irq_domain_init(struct mdp5_kms *mdp5_kms);
void mdp5_irq_domain_fini(struct mdp5_kms *mdp5_kms);

uint32_t mdp5_plane_get_flush(struct drm_plane *plane);
enum mdp5_pipe mdp5_plane_pipe(struct drm_plane *plane);
enum mdp5_pipe mdp5_plane_right_pipe(struct drm_plane *plane);
struct drm_plane *mdp5_plane_init(struct drm_device *dev,
				  enum drm_plane_type type);

struct mdp5_ctl *mdp5_crtc_get_ctl(struct drm_crtc *crtc);
uint32_t mdp5_crtc_vblank(struct drm_crtc *crtc);

struct mdp5_hw_mixer *mdp5_crtc_get_mixer(struct drm_crtc *crtc);
struct mdp5_pipeline *mdp5_crtc_get_pipeline(struct drm_crtc *crtc);
void mdp5_crtc_set_pipeline(struct drm_crtc *crtc);
void mdp5_crtc_wait_for_commit_done(struct drm_crtc *crtc);
struct drm_crtc *mdp5_crtc_init(struct drm_device *dev,
				struct drm_plane *plane,
				struct drm_plane *cursor_plane, int id);

struct drm_encoder *mdp5_encoder_init(struct drm_device *dev,
		struct mdp5_interface *intf, struct mdp5_ctl *ctl);
int mdp5_vid_encoder_set_split_display(struct drm_encoder *encoder,
				       struct drm_encoder *slave_encoder);
void mdp5_encoder_set_intf_mode(struct drm_encoder *encoder, bool cmd_mode);
int mdp5_encoder_get_linecount(struct drm_encoder *encoder);
u32 mdp5_encoder_get_framecount(struct drm_encoder *encoder);

#ifdef CONFIG_DRM_MSM_DSI
void mdp5_cmd_encoder_mode_set(struct drm_encoder *encoder,
			       struct drm_display_mode *mode,
			       struct drm_display_mode *adjusted_mode);
void mdp5_cmd_encoder_disable(struct drm_encoder *encoder);
void mdp5_cmd_encoder_enable(struct drm_encoder *encoder);
int mdp5_cmd_encoder_set_split_display(struct drm_encoder *encoder,
				       struct drm_encoder *slave_encoder);
#else
static inline void mdp5_cmd_encoder_mode_set(struct drm_encoder *encoder,
					     struct drm_display_mode *mode,
					     struct drm_display_mode *adjusted_mode)
{
}
static inline void mdp5_cmd_encoder_disable(struct drm_encoder *encoder)
{
}
static inline void mdp5_cmd_encoder_enable(struct drm_encoder *encoder)
{
}
static inline int mdp5_cmd_encoder_set_split_display(
	struct drm_encoder *encoder, struct drm_encoder *slave_encoder)
{
	return -EINVAL;
}
#endif

#endif /* __MDP5_KMS_H__ */
