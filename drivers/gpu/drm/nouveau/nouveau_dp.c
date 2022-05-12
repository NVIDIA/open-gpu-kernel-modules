/*
 * Copyright 2009 Red Hat Inc.
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
 * Authors: Ben Skeggs
 */

#include <drm/drm_dp_helper.h>

#include "nouveau_drv.h"
#include "nouveau_connector.h"
#include "nouveau_encoder.h"
#include "nouveau_crtc.h"

#include <nvif/class.h>
#include <nvif/cl5070.h>

MODULE_PARM_DESC(mst, "Enable DisplayPort multi-stream (default: enabled)");
static int nouveau_mst = 1;
module_param_named(mst, nouveau_mst, int, 0400);

static bool
nouveau_dp_has_sink_count(struct drm_connector *connector,
			  struct nouveau_encoder *outp)
{
	return drm_dp_read_sink_count_cap(connector, outp->dp.dpcd, &outp->dp.desc);
}

static enum drm_connector_status
nouveau_dp_probe_dpcd(struct nouveau_connector *nv_connector,
		      struct nouveau_encoder *outp)
{
	struct drm_connector *connector = &nv_connector->base;
	struct drm_dp_aux *aux = &nv_connector->aux;
	struct nv50_mstm *mstm = NULL;
	enum drm_connector_status status = connector_status_disconnected;
	int ret;
	u8 *dpcd = outp->dp.dpcd;

	ret = drm_dp_read_dpcd_caps(aux, dpcd);
	if (ret < 0)
		goto out;

	ret = drm_dp_read_desc(aux, &outp->dp.desc, drm_dp_is_branch(dpcd));
	if (ret < 0)
		goto out;

	if (nouveau_mst) {
		mstm = outp->dp.mstm;
		if (mstm)
			mstm->can_mst = drm_dp_read_mst_cap(aux, dpcd);
	}

	if (nouveau_dp_has_sink_count(connector, outp)) {
		ret = drm_dp_read_sink_count(aux);
		if (ret < 0)
			goto out;

		outp->dp.sink_count = ret;

		/*
		 * Dongle connected, but no display. Don't bother reading
		 * downstream port info
		 */
		if (!outp->dp.sink_count)
			return connector_status_disconnected;
	}

	ret = drm_dp_read_downstream_info(aux, dpcd,
					  outp->dp.downstream_ports);
	if (ret < 0)
		goto out;

	status = connector_status_connected;
out:
	if (status != connector_status_connected) {
		/* Clear any cached info */
		outp->dp.sink_count = 0;
	}
	return status;
}

int
nouveau_dp_detect(struct nouveau_connector *nv_connector,
		  struct nouveau_encoder *nv_encoder)
{
	struct drm_device *dev = nv_encoder->base.base.dev;
	struct nouveau_drm *drm = nouveau_drm(dev);
	struct drm_connector *connector = &nv_connector->base;
	struct nv50_mstm *mstm = nv_encoder->dp.mstm;
	enum drm_connector_status status;
	u8 *dpcd = nv_encoder->dp.dpcd;
	int ret = NOUVEAU_DP_NONE;

	/* If we've already read the DPCD on an eDP device, we don't need to
	 * reread it as it won't change
	 */
	if (connector->connector_type == DRM_MODE_CONNECTOR_eDP &&
	    dpcd[DP_DPCD_REV] != 0)
		return NOUVEAU_DP_SST;

	mutex_lock(&nv_encoder->dp.hpd_irq_lock);
	if (mstm) {
		/* If we're not ready to handle MST state changes yet, just
		 * report the last status of the connector. We'll reprobe it
		 * once we've resumed.
		 */
		if (mstm->suspended) {
			if (mstm->is_mst)
				ret = NOUVEAU_DP_MST;
			else if (connector->status ==
				 connector_status_connected)
				ret = NOUVEAU_DP_SST;

			goto out;
		}
	}

	status = nouveau_dp_probe_dpcd(nv_connector, nv_encoder);
	if (status == connector_status_disconnected)
		goto out;

	/* If we're in MST mode, we're done here */
	if (mstm && mstm->can_mst && mstm->is_mst) {
		ret = NOUVEAU_DP_MST;
		goto out;
	}

	nv_encoder->dp.link_bw = 27000 * dpcd[DP_MAX_LINK_RATE];
	nv_encoder->dp.link_nr =
		dpcd[DP_MAX_LANE_COUNT] & DP_MAX_LANE_COUNT_MASK;

	NV_DEBUG(drm, "display: %dx%d dpcd 0x%02x\n",
		 nv_encoder->dp.link_nr, nv_encoder->dp.link_bw,
		 dpcd[DP_DPCD_REV]);
	NV_DEBUG(drm, "encoder: %dx%d\n",
		 nv_encoder->dcb->dpconf.link_nr,
		 nv_encoder->dcb->dpconf.link_bw);

	if (nv_encoder->dcb->dpconf.link_nr < nv_encoder->dp.link_nr)
		nv_encoder->dp.link_nr = nv_encoder->dcb->dpconf.link_nr;
	if (nv_encoder->dcb->dpconf.link_bw < nv_encoder->dp.link_bw)
		nv_encoder->dp.link_bw = nv_encoder->dcb->dpconf.link_bw;

	NV_DEBUG(drm, "maximum: %dx%d\n",
		 nv_encoder->dp.link_nr, nv_encoder->dp.link_bw);

	if (mstm && mstm->can_mst) {
		ret = nv50_mstm_detect(nv_encoder);
		if (ret == 1) {
			ret = NOUVEAU_DP_MST;
			goto out;
		} else if (ret != 0) {
			goto out;
		}
	}
	ret = NOUVEAU_DP_SST;

out:
	if (mstm && !mstm->suspended && ret != NOUVEAU_DP_MST)
		nv50_mstm_remove(mstm);

	mutex_unlock(&nv_encoder->dp.hpd_irq_lock);
	return ret;
}

void nouveau_dp_irq(struct nouveau_drm *drm,
		    struct nouveau_connector *nv_connector)
{
	struct drm_connector *connector = &nv_connector->base;
	struct nouveau_encoder *outp = find_encoder(connector, DCB_OUTPUT_DP);
	struct nv50_mstm *mstm;
	int ret;
	bool send_hpd = false;

	if (!outp)
		return;

	mstm = outp->dp.mstm;
	NV_DEBUG(drm, "service %s\n", connector->name);

	mutex_lock(&outp->dp.hpd_irq_lock);

	if (mstm && mstm->is_mst) {
		if (!nv50_mstm_service(drm, nv_connector, mstm))
			send_hpd = true;
	} else {
		drm_dp_cec_irq(&nv_connector->aux);

		if (nouveau_dp_has_sink_count(connector, outp)) {
			ret = drm_dp_read_sink_count(&nv_connector->aux);
			if (ret != outp->dp.sink_count)
				send_hpd = true;
			if (ret >= 0)
				outp->dp.sink_count = ret;
		}
	}

	mutex_unlock(&outp->dp.hpd_irq_lock);

	if (send_hpd)
		nouveau_connector_hpd(connector);
}

/* TODO:
 * - Use the minimum possible BPC here, once we add support for the max bpc
 *   property.
 * - Validate against the DP caps advertised by the GPU (we don't check these
 *   yet)
 */
enum drm_mode_status
nv50_dp_mode_valid(struct drm_connector *connector,
		   struct nouveau_encoder *outp,
		   const struct drm_display_mode *mode,
		   unsigned *out_clock)
{
	const unsigned int min_clock = 25000;
	unsigned int max_rate, mode_rate, ds_max_dotclock, clock = mode->clock;
	const u8 bpp = connector->display_info.bpc * 3;

	if (mode->flags & DRM_MODE_FLAG_INTERLACE && !outp->caps.dp_interlace)
		return MODE_NO_INTERLACE;

	if ((mode->flags & DRM_MODE_FLAG_3D_MASK) == DRM_MODE_FLAG_3D_FRAME_PACKING)
		clock *= 2;

	max_rate = outp->dp.link_nr * outp->dp.link_bw;
	mode_rate = DIV_ROUND_UP(clock * bpp, 8);
	if (mode_rate > max_rate)
		return MODE_CLOCK_HIGH;

	ds_max_dotclock = drm_dp_downstream_max_dotclock(outp->dp.dpcd, outp->dp.downstream_ports);
	if (ds_max_dotclock && clock > ds_max_dotclock)
		return MODE_CLOCK_HIGH;

	if (clock < min_clock)
		return MODE_CLOCK_LOW;

	if (out_clock)
		*out_clock = clock;

	return MODE_OK;
}
