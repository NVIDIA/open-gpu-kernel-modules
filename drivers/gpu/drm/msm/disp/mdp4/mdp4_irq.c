// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2013 Red Hat
 * Author: Rob Clark <robdclark@gmail.com>
 */

#include <drm/drm_print.h>
#include <drm/drm_vblank.h>

#include "msm_drv.h"
#include "mdp4_kms.h"

void mdp4_set_irqmask(struct mdp_kms *mdp_kms, uint32_t irqmask,
		uint32_t old_irqmask)
{
	mdp4_write(to_mdp4_kms(mdp_kms), REG_MDP4_INTR_CLEAR,
		irqmask ^ (irqmask & old_irqmask));
	mdp4_write(to_mdp4_kms(mdp_kms), REG_MDP4_INTR_ENABLE, irqmask);
}

static void mdp4_irq_error_handler(struct mdp_irq *irq, uint32_t irqstatus)
{
	struct mdp4_kms *mdp4_kms = container_of(irq, struct mdp4_kms, error_handler);
	static DEFINE_RATELIMIT_STATE(rs, 5*HZ, 1);
	extern bool dumpstate;

	DRM_ERROR_RATELIMITED("errors: %08x\n", irqstatus);

	if (dumpstate && __ratelimit(&rs)) {
		struct drm_printer p = drm_info_printer(mdp4_kms->dev->dev);
		drm_state_dump(mdp4_kms->dev, &p);
	}
}

void mdp4_irq_preinstall(struct msm_kms *kms)
{
	struct mdp4_kms *mdp4_kms = to_mdp4_kms(to_mdp_kms(kms));
	mdp4_enable(mdp4_kms);
	mdp4_write(mdp4_kms, REG_MDP4_INTR_CLEAR, 0xffffffff);
	mdp4_write(mdp4_kms, REG_MDP4_INTR_ENABLE, 0x00000000);
	mdp4_disable(mdp4_kms);
}

int mdp4_irq_postinstall(struct msm_kms *kms)
{
	struct mdp_kms *mdp_kms = to_mdp_kms(kms);
	struct mdp4_kms *mdp4_kms = to_mdp4_kms(mdp_kms);
	struct mdp_irq *error_handler = &mdp4_kms->error_handler;

	error_handler->irq = mdp4_irq_error_handler;
	error_handler->irqmask = MDP4_IRQ_PRIMARY_INTF_UDERRUN |
			MDP4_IRQ_EXTERNAL_INTF_UDERRUN;

	mdp_irq_register(mdp_kms, error_handler);

	return 0;
}

void mdp4_irq_uninstall(struct msm_kms *kms)
{
	struct mdp4_kms *mdp4_kms = to_mdp4_kms(to_mdp_kms(kms));
	mdp4_enable(mdp4_kms);
	mdp4_write(mdp4_kms, REG_MDP4_INTR_ENABLE, 0x00000000);
	mdp4_disable(mdp4_kms);
}

irqreturn_t mdp4_irq(struct msm_kms *kms)
{
	struct mdp_kms *mdp_kms = to_mdp_kms(kms);
	struct mdp4_kms *mdp4_kms = to_mdp4_kms(mdp_kms);
	struct drm_device *dev = mdp4_kms->dev;
	struct msm_drm_private *priv = dev->dev_private;
	unsigned int id;
	uint32_t status, enable;

	enable = mdp4_read(mdp4_kms, REG_MDP4_INTR_ENABLE);
	status = mdp4_read(mdp4_kms, REG_MDP4_INTR_STATUS) & enable;
	mdp4_write(mdp4_kms, REG_MDP4_INTR_CLEAR, status);

	VERB("status=%08x", status);

	mdp_dispatch_irqs(mdp_kms, status);

	for (id = 0; id < priv->num_crtcs; id++)
		if (status & mdp4_crtc_vblank(priv->crtcs[id]))
			drm_handle_vblank(dev, id);

	return IRQ_HANDLED;
}

int mdp4_enable_vblank(struct msm_kms *kms, struct drm_crtc *crtc)
{
	struct mdp4_kms *mdp4_kms = to_mdp4_kms(to_mdp_kms(kms));

	mdp4_enable(mdp4_kms);
	mdp_update_vblank_mask(to_mdp_kms(kms),
			mdp4_crtc_vblank(crtc), true);
	mdp4_disable(mdp4_kms);

	return 0;
}

void mdp4_disable_vblank(struct msm_kms *kms, struct drm_crtc *crtc)
{
	struct mdp4_kms *mdp4_kms = to_mdp4_kms(to_mdp_kms(kms));

	mdp4_enable(mdp4_kms);
	mdp_update_vblank_mask(to_mdp_kms(kms),
			mdp4_crtc_vblank(crtc), false);
	mdp4_disable(mdp4_kms);
}
