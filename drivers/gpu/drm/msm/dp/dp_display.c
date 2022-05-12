// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2017-2020, The Linux Foundation. All rights reserved.
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/debugfs.h>
#include <linux/component.h>
#include <linux/of_irq.h>
#include <linux/delay.h>

#include "msm_drv.h"
#include "msm_kms.h"
#include "dp_hpd.h"
#include "dp_parser.h"
#include "dp_power.h"
#include "dp_catalog.h"
#include "dp_aux.h"
#include "dp_reg.h"
#include "dp_link.h"
#include "dp_panel.h"
#include "dp_ctrl.h"
#include "dp_display.h"
#include "dp_drm.h"
#include "dp_audio.h"
#include "dp_debug.h"

static struct msm_dp *g_dp_display;
#define HPD_STRING_SIZE 30

enum {
	ISR_DISCONNECTED,
	ISR_CONNECT_PENDING,
	ISR_CONNECTED,
	ISR_HPD_REPLUG_COUNT,
	ISR_IRQ_HPD_PULSE_COUNT,
	ISR_HPD_LO_GLITH_COUNT,
};

/* event thread connection state */
enum {
	ST_DISCONNECTED,
	ST_CONNECT_PENDING,
	ST_CONNECTED,
	ST_DISCONNECT_PENDING,
	ST_DISPLAY_OFF,
	ST_SUSPENDED,
};

enum {
	EV_NO_EVENT,
	/* hpd events */
	EV_HPD_INIT_SETUP,
	EV_HPD_PLUG_INT,
	EV_IRQ_HPD_INT,
	EV_HPD_REPLUG_INT,
	EV_HPD_UNPLUG_INT,
	EV_USER_NOTIFICATION,
	EV_CONNECT_PENDING_TIMEOUT,
	EV_DISCONNECT_PENDING_TIMEOUT,
};

#define EVENT_TIMEOUT	(HZ/10)	/* 100ms */
#define DP_EVENT_Q_MAX	8

#define DP_TIMEOUT_5_SECOND	(5000/EVENT_TIMEOUT)
#define DP_TIMEOUT_NONE		0

#define WAIT_FOR_RESUME_TIMEOUT_JIFFIES (HZ / 2)

struct dp_event {
	u32 event_id;
	u32 data;
	u32 delay;
};

struct dp_display_private {
	char *name;
	int irq;

	/* state variables */
	bool core_initialized;
	bool hpd_irq_on;
	bool audio_supported;

	struct platform_device *pdev;
	struct dentry *root;

	struct dp_usbpd   *usbpd;
	struct dp_parser  *parser;
	struct dp_power   *power;
	struct dp_catalog *catalog;
	struct drm_dp_aux *aux;
	struct dp_link    *link;
	struct dp_panel   *panel;
	struct dp_ctrl    *ctrl;
	struct dp_debug   *debug;

	struct dp_usbpd_cb usbpd_cb;
	struct dp_display_mode dp_mode;
	struct msm_dp dp_display;

	bool encoder_mode_set;

	/* wait for audio signaling */
	struct completion audio_comp;

	/* event related only access by event thread */
	struct mutex event_mutex;
	wait_queue_head_t event_q;
	u32 hpd_state;
	u32 event_pndx;
	u32 event_gndx;
	struct dp_event event_list[DP_EVENT_Q_MAX];
	spinlock_t event_lock;

	struct dp_audio *audio;
};

static const struct of_device_id dp_dt_match[] = {
	{.compatible = "qcom,sc7180-dp"},
	{}
};

static int dp_add_event(struct dp_display_private *dp_priv, u32 event,
						u32 data, u32 delay)
{
	unsigned long flag;
	struct dp_event *todo;
	int pndx;

	spin_lock_irqsave(&dp_priv->event_lock, flag);
	pndx = dp_priv->event_pndx + 1;
	pndx %= DP_EVENT_Q_MAX;
	if (pndx == dp_priv->event_gndx) {
		pr_err("event_q is full: pndx=%d gndx=%d\n",
			dp_priv->event_pndx, dp_priv->event_gndx);
		spin_unlock_irqrestore(&dp_priv->event_lock, flag);
		return -EPERM;
	}
	todo = &dp_priv->event_list[dp_priv->event_pndx++];
	dp_priv->event_pndx %= DP_EVENT_Q_MAX;
	todo->event_id = event;
	todo->data = data;
	todo->delay = delay;
	wake_up(&dp_priv->event_q);
	spin_unlock_irqrestore(&dp_priv->event_lock, flag);

	return 0;
}

static int dp_del_event(struct dp_display_private *dp_priv, u32 event)
{
	unsigned long flag;
	struct dp_event *todo;
	u32	gndx;

	spin_lock_irqsave(&dp_priv->event_lock, flag);
	if (dp_priv->event_pndx == dp_priv->event_gndx) {
		spin_unlock_irqrestore(&dp_priv->event_lock, flag);
		return -ENOENT;
	}

	gndx = dp_priv->event_gndx;
	while (dp_priv->event_pndx != gndx) {
		todo = &dp_priv->event_list[gndx];
		if (todo->event_id == event) {
			todo->event_id = EV_NO_EVENT;	/* deleted */
			todo->delay = 0;
		}
		gndx++;
		gndx %= DP_EVENT_Q_MAX;
	}
	spin_unlock_irqrestore(&dp_priv->event_lock, flag);

	return 0;
}

void dp_display_signal_audio_start(struct msm_dp *dp_display)
{
	struct dp_display_private *dp;

	dp = container_of(dp_display, struct dp_display_private, dp_display);

	reinit_completion(&dp->audio_comp);
}

void dp_display_signal_audio_complete(struct msm_dp *dp_display)
{
	struct dp_display_private *dp;

	dp = container_of(dp_display, struct dp_display_private, dp_display);

	complete_all(&dp->audio_comp);
}

static int dp_display_bind(struct device *dev, struct device *master,
			   void *data)
{
	int rc = 0;
	struct dp_display_private *dp;
	struct drm_device *drm;
	struct msm_drm_private *priv;

	drm = dev_get_drvdata(master);

	dp = container_of(g_dp_display,
			struct dp_display_private, dp_display);

	dp->dp_display.drm_dev = drm;
	priv = drm->dev_private;
	priv->dp = &(dp->dp_display);

	rc = dp->parser->parse(dp->parser);
	if (rc) {
		DRM_ERROR("device tree parsing failed\n");
		goto end;
	}

	rc = dp_aux_register(dp->aux);
	if (rc) {
		DRM_ERROR("DRM DP AUX register failed\n");
		goto end;
	}

	rc = dp_power_client_init(dp->power);
	if (rc) {
		DRM_ERROR("Power client create failed\n");
		goto end;
	}

	rc = dp_register_audio_driver(dev, dp->audio);
	if (rc)
		DRM_ERROR("Audio registration Dp failed\n");

end:
	return rc;
}

static void dp_display_unbind(struct device *dev, struct device *master,
			      void *data)
{
	struct dp_display_private *dp;
	struct drm_device *drm = dev_get_drvdata(master);
	struct msm_drm_private *priv = drm->dev_private;

	dp = container_of(g_dp_display,
			struct dp_display_private, dp_display);

	dp_power_client_deinit(dp->power);
	dp_aux_unregister(dp->aux);
	priv->dp = NULL;
}

static const struct component_ops dp_display_comp_ops = {
	.bind = dp_display_bind,
	.unbind = dp_display_unbind,
};

static bool dp_display_is_ds_bridge(struct dp_panel *panel)
{
	return (panel->dpcd[DP_DOWNSTREAMPORT_PRESENT] &
		DP_DWN_STRM_PORT_PRESENT);
}

static bool dp_display_is_sink_count_zero(struct dp_display_private *dp)
{
	return dp_display_is_ds_bridge(dp->panel) &&
		(dp->link->sink_count == 0);
}

static void dp_display_send_hpd_event(struct msm_dp *dp_display)
{
	struct dp_display_private *dp;
	struct drm_connector *connector;

	dp = container_of(dp_display, struct dp_display_private, dp_display);

	connector = dp->dp_display.connector;
	drm_helper_hpd_irq_event(connector->dev);
}


static void dp_display_set_encoder_mode(struct dp_display_private *dp)
{
	struct msm_drm_private *priv = dp->dp_display.drm_dev->dev_private;
	struct msm_kms *kms = priv->kms;

	if (!dp->encoder_mode_set && dp->dp_display.encoder &&
				kms->funcs->set_encoder_mode) {
		kms->funcs->set_encoder_mode(kms,
				dp->dp_display.encoder, false);

		dp->encoder_mode_set = true;
	}
}

static int dp_display_send_hpd_notification(struct dp_display_private *dp,
					    bool hpd)
{
	if ((hpd && dp->dp_display.is_connected) ||
			(!hpd && !dp->dp_display.is_connected)) {
		DRM_DEBUG_DP("HPD already %s\n", (hpd ? "on" : "off"));
		return 0;
	}

	/* reset video pattern flag on disconnect */
	if (!hpd)
		dp->panel->video_test = false;

	dp->dp_display.is_connected = hpd;

	dp_display_send_hpd_event(&dp->dp_display);

	return 0;
}

static int dp_display_process_hpd_high(struct dp_display_private *dp)
{
	int rc = 0;
	struct edid *edid;

	dp->panel->max_dp_lanes = dp->parser->max_dp_lanes;

	rc = dp_panel_read_sink_caps(dp->panel, dp->dp_display.connector);
	if (rc)
		goto end;

	dp_link_process_request(dp->link);

	edid = dp->panel->edid;

	dp->audio_supported = drm_detect_monitor_audio(edid);
	dp_panel_handle_sink_request(dp->panel);

	dp->dp_display.max_pclk_khz = DP_MAX_PIXEL_CLK_KHZ;
	dp->dp_display.max_dp_lanes = dp->parser->max_dp_lanes;

	/*
	 * set sink to normal operation mode -- D0
	 * before dpcd read
	 */
	dp_link_psm_config(dp->link, &dp->panel->link_info, false);

	dp_link_reset_phy_params_vx_px(dp->link);
	rc = dp_ctrl_on_link(dp->ctrl);
	if (rc) {
		DRM_ERROR("failed to complete DP link training\n");
		goto end;
	}

	dp_add_event(dp, EV_USER_NOTIFICATION, true, 0);

end:
	return rc;
}

static void dp_display_host_init(struct dp_display_private *dp, int reset)
{
	bool flip = false;

	if (dp->core_initialized) {
		DRM_DEBUG_DP("DP core already initialized\n");
		return;
	}

	if (dp->usbpd->orientation == ORIENTATION_CC2)
		flip = true;

	dp_display_set_encoder_mode(dp);

	dp_power_init(dp->power, flip);
	dp_ctrl_host_init(dp->ctrl, flip, reset);
	dp_aux_init(dp->aux);
	dp->core_initialized = true;
}

static void dp_display_host_deinit(struct dp_display_private *dp)
{
	if (!dp->core_initialized) {
		DRM_DEBUG_DP("DP core not initialized\n");
		return;
	}

	dp_ctrl_host_deinit(dp->ctrl);
	dp_aux_deinit(dp->aux);
	dp_power_deinit(dp->power);

	dp->core_initialized = false;
}

static int dp_display_usbpd_configure_cb(struct device *dev)
{
	int rc = 0;
	struct dp_display_private *dp;

	if (!dev) {
		DRM_ERROR("invalid dev\n");
		rc = -EINVAL;
		goto end;
	}

	dp = container_of(g_dp_display,
			struct dp_display_private, dp_display);

	dp_display_host_init(dp, false);

	rc = dp_display_process_hpd_high(dp);
end:
	return rc;
}

static int dp_display_usbpd_disconnect_cb(struct device *dev)
{
	int rc = 0;
	struct dp_display_private *dp;

	if (!dev) {
		DRM_ERROR("invalid dev\n");
		rc = -EINVAL;
		return rc;
	}

	dp = container_of(g_dp_display,
			struct dp_display_private, dp_display);

	dp_add_event(dp, EV_USER_NOTIFICATION, false, 0);

	return rc;
}

static void dp_display_handle_video_request(struct dp_display_private *dp)
{
	if (dp->link->sink_request & DP_TEST_LINK_VIDEO_PATTERN) {
		dp->panel->video_test = true;
		dp_link_send_test_response(dp->link);
	}
}

static int dp_display_handle_port_ststus_changed(struct dp_display_private *dp)
{
	int rc = 0;

	if (dp_display_is_sink_count_zero(dp)) {
		DRM_DEBUG_DP("sink count is zero, nothing to do\n");
		if (dp->hpd_state != ST_DISCONNECTED) {
			dp->hpd_state = ST_DISCONNECT_PENDING;
			dp_add_event(dp, EV_USER_NOTIFICATION, false, 0);
		}
	} else {
		if (dp->hpd_state == ST_DISCONNECTED) {
			dp->hpd_state = ST_CONNECT_PENDING;
			rc = dp_display_process_hpd_high(dp);
			if (rc)
				dp->hpd_state = ST_DISCONNECTED;
		}
	}

	return rc;
}

static int dp_display_handle_irq_hpd(struct dp_display_private *dp)
{
	u32 sink_request = dp->link->sink_request;

	if (dp->hpd_state == ST_DISCONNECTED) {
		if (sink_request & DP_LINK_STATUS_UPDATED) {
			DRM_ERROR("Disconnected, no DP_LINK_STATUS_UPDATED\n");
			return -EINVAL;
		}
	}

	dp_ctrl_handle_sink_request(dp->ctrl);

	if (sink_request & DP_TEST_LINK_VIDEO_PATTERN)
		dp_display_handle_video_request(dp);

	return 0;
}

static int dp_display_usbpd_attention_cb(struct device *dev)
{
	int rc = 0;
	u32 sink_request;
	struct dp_display_private *dp;

	if (!dev) {
		DRM_ERROR("invalid dev\n");
		return -EINVAL;
	}

	dp = container_of(g_dp_display,
			struct dp_display_private, dp_display);

	/* check for any test request issued by sink */
	rc = dp_link_process_request(dp->link);
	if (!rc) {
		sink_request = dp->link->sink_request;
		if (sink_request & DS_PORT_STATUS_CHANGED)
			rc = dp_display_handle_port_ststus_changed(dp);
		else
			rc = dp_display_handle_irq_hpd(dp);
	}

	return rc;
}

static int dp_hpd_plug_handle(struct dp_display_private *dp, u32 data)
{
	struct dp_usbpd *hpd = dp->usbpd;
	u32 state;
	u32 tout = DP_TIMEOUT_5_SECOND;
	int ret;

	if (!hpd)
		return 0;

	mutex_lock(&dp->event_mutex);

	state =  dp->hpd_state;
	if (state == ST_DISPLAY_OFF || state == ST_SUSPENDED) {
		mutex_unlock(&dp->event_mutex);
		return 0;
	}

	if (state == ST_CONNECT_PENDING || state == ST_CONNECTED) {
		mutex_unlock(&dp->event_mutex);
		return 0;
	}

	if (state == ST_DISCONNECT_PENDING) {
		/* wait until ST_DISCONNECTED */
		dp_add_event(dp, EV_HPD_PLUG_INT, 0, 1); /* delay = 1 */
		mutex_unlock(&dp->event_mutex);
		return 0;
	}

	dp->hpd_state = ST_CONNECT_PENDING;

	hpd->hpd_high = 1;

	ret = dp_display_usbpd_configure_cb(&dp->pdev->dev);
	if (ret) {	/* link train failed */
		hpd->hpd_high = 0;
		dp->hpd_state = ST_DISCONNECTED;

		if (ret == -ECONNRESET) { /* cable unplugged */
			dp->core_initialized = false;
		}

	} else {
		/* start sentinel checking in case of missing uevent */
		dp_add_event(dp, EV_CONNECT_PENDING_TIMEOUT, 0, tout);
	}

	/* enable HDP irq_hpd/replug interrupt */
	dp_catalog_hpd_config_intr(dp->catalog,
		DP_DP_IRQ_HPD_INT_MASK | DP_DP_HPD_REPLUG_INT_MASK, true);

	mutex_unlock(&dp->event_mutex);

	/* uevent will complete connection part */
	return 0;
};

static int dp_display_enable(struct dp_display_private *dp, u32 data);
static int dp_display_disable(struct dp_display_private *dp, u32 data);

static int dp_connect_pending_timeout(struct dp_display_private *dp, u32 data)
{
	u32 state;

	mutex_lock(&dp->event_mutex);

	state = dp->hpd_state;
	if (state == ST_CONNECT_PENDING)
		dp->hpd_state = ST_CONNECTED;

	mutex_unlock(&dp->event_mutex);

	return 0;
}

static void dp_display_handle_plugged_change(struct msm_dp *dp_display,
		bool plugged)
{
	struct dp_display_private *dp;

	dp = container_of(dp_display,
			struct dp_display_private, dp_display);

	/* notify audio subsystem only if sink supports audio */
	if (dp_display->plugged_cb && dp_display->codec_dev &&
			dp->audio_supported)
		dp_display->plugged_cb(dp_display->codec_dev, plugged);
}

static int dp_hpd_unplug_handle(struct dp_display_private *dp, u32 data)
{
	struct dp_usbpd *hpd = dp->usbpd;
	u32 state;

	if (!hpd)
		return 0;

	mutex_lock(&dp->event_mutex);

	state = dp->hpd_state;

	/* disable irq_hpd/replug interrupts */
	dp_catalog_hpd_config_intr(dp->catalog,
		DP_DP_IRQ_HPD_INT_MASK | DP_DP_HPD_REPLUG_INT_MASK, false);

	/* unplugged, no more irq_hpd handle */
	dp_del_event(dp, EV_IRQ_HPD_INT);

	if (state == ST_DISCONNECTED) {
		/* triggered by irq_hdp with sink_count = 0 */
		if (dp->link->sink_count == 0) {
			dp_ctrl_off_phy(dp->ctrl);
			hpd->hpd_high = 0;
			dp->core_initialized = false;
		}
		mutex_unlock(&dp->event_mutex);
		return 0;
	}

	if (state == ST_DISCONNECT_PENDING) {
		mutex_unlock(&dp->event_mutex);
		return 0;
	}

	if (state == ST_CONNECT_PENDING) {
		/* wait until CONNECTED */
		dp_add_event(dp, EV_HPD_UNPLUG_INT, 0, 1); /* delay = 1 */
		mutex_unlock(&dp->event_mutex);
		return 0;
	}

	dp->hpd_state = ST_DISCONNECT_PENDING;

	/* disable HPD plug interrupts */
	dp_catalog_hpd_config_intr(dp->catalog, DP_DP_HPD_PLUG_INT_MASK, false);

	hpd->hpd_high = 0;

	/*
	 * We don't need separate work for disconnect as
	 * connect/attention interrupts are disabled
	 */
	dp_display_usbpd_disconnect_cb(&dp->pdev->dev);

	/* start sentinel checking in case of missing uevent */
	dp_add_event(dp, EV_DISCONNECT_PENDING_TIMEOUT, 0, DP_TIMEOUT_5_SECOND);

	/* signal the disconnect event early to ensure proper teardown */
	dp_display_handle_plugged_change(g_dp_display, false);

	/* enable HDP plug interrupt to prepare for next plugin */
	dp_catalog_hpd_config_intr(dp->catalog, DP_DP_HPD_PLUG_INT_MASK, true);

	/* uevent will complete disconnection part */
	mutex_unlock(&dp->event_mutex);
	return 0;
}

static int dp_disconnect_pending_timeout(struct dp_display_private *dp, u32 data)
{
	u32 state;

	mutex_lock(&dp->event_mutex);

	state =  dp->hpd_state;
	if (state == ST_DISCONNECT_PENDING)
		dp->hpd_state = ST_DISCONNECTED;

	mutex_unlock(&dp->event_mutex);

	return 0;
}

static int dp_irq_hpd_handle(struct dp_display_private *dp, u32 data)
{
	u32 state;
	int ret;

	mutex_lock(&dp->event_mutex);

	/* irq_hpd can happen at either connected or disconnected state */
	state =  dp->hpd_state;
	if (state == ST_DISPLAY_OFF || state == ST_SUSPENDED) {
		mutex_unlock(&dp->event_mutex);
		return 0;
	}

	if (state == ST_CONNECT_PENDING) {
		/* wait until ST_CONNECTED */
		dp_add_event(dp, EV_IRQ_HPD_INT, 0, 1); /* delay = 1 */
		mutex_unlock(&dp->event_mutex);
		return 0;
	}

	if (state == ST_CONNECT_PENDING || state == ST_DISCONNECT_PENDING) {
		/* wait until ST_CONNECTED */
		dp_add_event(dp, EV_IRQ_HPD_INT, 0, 1); /* delay = 1 */
		mutex_unlock(&dp->event_mutex);
		return 0;
	}

	ret = dp_display_usbpd_attention_cb(&dp->pdev->dev);
	if (ret == -ECONNRESET) { /* cable unplugged */
		dp->core_initialized = false;
	}

	mutex_unlock(&dp->event_mutex);

	return 0;
}

static void dp_display_deinit_sub_modules(struct dp_display_private *dp)
{
	dp_debug_put(dp->debug);
	dp_panel_put(dp->panel);
	dp_aux_put(dp->aux);
	dp_audio_put(dp->audio);
}

static int dp_init_sub_modules(struct dp_display_private *dp)
{
	int rc = 0;
	struct device *dev = &dp->pdev->dev;
	struct dp_usbpd_cb *cb = &dp->usbpd_cb;
	struct dp_panel_in panel_in = {
		.dev = dev,
	};

	/* Callback APIs used for cable status change event */
	cb->configure  = dp_display_usbpd_configure_cb;
	cb->disconnect = dp_display_usbpd_disconnect_cb;
	cb->attention  = dp_display_usbpd_attention_cb;

	dp->usbpd = dp_hpd_get(dev, cb);
	if (IS_ERR(dp->usbpd)) {
		rc = PTR_ERR(dp->usbpd);
		DRM_ERROR("failed to initialize hpd, rc = %d\n", rc);
		dp->usbpd = NULL;
		goto error;
	}

	dp->parser = dp_parser_get(dp->pdev);
	if (IS_ERR(dp->parser)) {
		rc = PTR_ERR(dp->parser);
		DRM_ERROR("failed to initialize parser, rc = %d\n", rc);
		dp->parser = NULL;
		goto error;
	}

	dp->catalog = dp_catalog_get(dev, &dp->parser->io);
	if (IS_ERR(dp->catalog)) {
		rc = PTR_ERR(dp->catalog);
		DRM_ERROR("failed to initialize catalog, rc = %d\n", rc);
		dp->catalog = NULL;
		goto error;
	}

	dp->power = dp_power_get(dev, dp->parser);
	if (IS_ERR(dp->power)) {
		rc = PTR_ERR(dp->power);
		DRM_ERROR("failed to initialize power, rc = %d\n", rc);
		dp->power = NULL;
		goto error;
	}

	dp->aux = dp_aux_get(dev, dp->catalog);
	if (IS_ERR(dp->aux)) {
		rc = PTR_ERR(dp->aux);
		DRM_ERROR("failed to initialize aux, rc = %d\n", rc);
		dp->aux = NULL;
		goto error;
	}

	dp->link = dp_link_get(dev, dp->aux);
	if (IS_ERR(dp->link)) {
		rc = PTR_ERR(dp->link);
		DRM_ERROR("failed to initialize link, rc = %d\n", rc);
		dp->link = NULL;
		goto error_link;
	}

	panel_in.aux = dp->aux;
	panel_in.catalog = dp->catalog;
	panel_in.link = dp->link;

	dp->panel = dp_panel_get(&panel_in);
	if (IS_ERR(dp->panel)) {
		rc = PTR_ERR(dp->panel);
		DRM_ERROR("failed to initialize panel, rc = %d\n", rc);
		dp->panel = NULL;
		goto error_link;
	}

	dp->ctrl = dp_ctrl_get(dev, dp->link, dp->panel, dp->aux,
			       dp->power, dp->catalog, dp->parser);
	if (IS_ERR(dp->ctrl)) {
		rc = PTR_ERR(dp->ctrl);
		DRM_ERROR("failed to initialize ctrl, rc = %d\n", rc);
		dp->ctrl = NULL;
		goto error_ctrl;
	}

	dp->audio = dp_audio_get(dp->pdev, dp->panel, dp->catalog);
	if (IS_ERR(dp->audio)) {
		rc = PTR_ERR(dp->audio);
		pr_err("failed to initialize audio, rc = %d\n", rc);
		dp->audio = NULL;
		goto error_ctrl;
	}

	return rc;

error_ctrl:
	dp_panel_put(dp->panel);
error_link:
	dp_aux_put(dp->aux);
error:
	return rc;
}

static int dp_display_set_mode(struct msm_dp *dp_display,
			       struct dp_display_mode *mode)
{
	struct dp_display_private *dp;

	dp = container_of(dp_display, struct dp_display_private, dp_display);

	dp->panel->dp_mode.drm_mode = mode->drm_mode;
	dp->panel->dp_mode.bpp = mode->bpp;
	dp->panel->dp_mode.capabilities = mode->capabilities;
	dp_panel_init_panel_info(dp->panel);
	return 0;
}

static int dp_display_prepare(struct msm_dp *dp)
{
	return 0;
}

static int dp_display_enable(struct dp_display_private *dp, u32 data)
{
	int rc = 0;
	struct msm_dp *dp_display;

	dp_display = g_dp_display;

	if (dp_display->power_on) {
		DRM_DEBUG_DP("Link already setup, return\n");
		return 0;
	}

	rc = dp_ctrl_on_stream(dp->ctrl);
	if (!rc)
		dp_display->power_on = true;

	return rc;
}

static int dp_display_post_enable(struct msm_dp *dp_display)
{
	struct dp_display_private *dp;
	u32 rate;

	dp = container_of(dp_display, struct dp_display_private, dp_display);

	rate = dp->link->link_params.rate;

	if (dp->audio_supported) {
		dp->audio->bw_code = drm_dp_link_rate_to_bw_code(rate);
		dp->audio->lane_count = dp->link->link_params.num_lanes;
	}

	/* signal the connect event late to synchronize video and display */
	dp_display_handle_plugged_change(dp_display, true);
	return 0;
}

static int dp_display_disable(struct dp_display_private *dp, u32 data)
{
	struct msm_dp *dp_display;

	dp_display = g_dp_display;

	if (!dp_display->power_on)
		return 0;

	/* wait only if audio was enabled */
	if (dp_display->audio_enabled) {
		/* signal the disconnect event */
		dp_display_handle_plugged_change(dp_display, false);
		if (!wait_for_completion_timeout(&dp->audio_comp,
				HZ * 5))
			DRM_ERROR("audio comp timeout\n");
	}

	dp_display->audio_enabled = false;

	/* triggered by irq_hpd with sink_count = 0 */
	if (dp->link->sink_count == 0) {
		dp_ctrl_off_link_stream(dp->ctrl);
	} else {
		dp_ctrl_off(dp->ctrl);
		dp->core_initialized = false;
	}

	dp_display->power_on = false;

	return 0;
}

static int dp_display_unprepare(struct msm_dp *dp)
{
	return 0;
}

int dp_display_set_plugged_cb(struct msm_dp *dp_display,
		hdmi_codec_plugged_cb fn, struct device *codec_dev)
{
	bool plugged;

	dp_display->plugged_cb = fn;
	dp_display->codec_dev = codec_dev;
	plugged = dp_display->is_connected;
	dp_display_handle_plugged_change(dp_display, plugged);

	return 0;
}

int dp_display_validate_mode(struct msm_dp *dp, u32 mode_pclk_khz)
{
	const u32 num_components = 3, default_bpp = 24;
	struct dp_display_private *dp_display;
	struct dp_link_info *link_info;
	u32 mode_rate_khz = 0, supported_rate_khz = 0, mode_bpp = 0;

	if (!dp || !mode_pclk_khz || !dp->connector) {
		DRM_ERROR("invalid params\n");
		return -EINVAL;
	}

	dp_display = container_of(dp, struct dp_display_private, dp_display);
	link_info = &dp_display->panel->link_info;

	mode_bpp = dp->connector->display_info.bpc * num_components;
	if (!mode_bpp)
		mode_bpp = default_bpp;

	mode_bpp = dp_panel_get_mode_bpp(dp_display->panel,
			mode_bpp, mode_pclk_khz);

	mode_rate_khz = mode_pclk_khz * mode_bpp;
	supported_rate_khz = link_info->num_lanes * link_info->rate * 8;

	if (mode_rate_khz > supported_rate_khz)
		return MODE_BAD;

	return MODE_OK;
}

int dp_display_get_modes(struct msm_dp *dp,
				struct dp_display_mode *dp_mode)
{
	struct dp_display_private *dp_display;
	int ret = 0;

	if (!dp) {
		DRM_ERROR("invalid params\n");
		return 0;
	}

	dp_display = container_of(dp, struct dp_display_private, dp_display);

	ret = dp_panel_get_modes(dp_display->panel,
		dp->connector, dp_mode);
	if (dp_mode->drm_mode.clock)
		dp->max_pclk_khz = dp_mode->drm_mode.clock;
	return ret;
}

bool dp_display_check_video_test(struct msm_dp *dp)
{
	struct dp_display_private *dp_display;

	dp_display = container_of(dp, struct dp_display_private, dp_display);

	return dp_display->panel->video_test;
}

int dp_display_get_test_bpp(struct msm_dp *dp)
{
	struct dp_display_private *dp_display;

	if (!dp) {
		DRM_ERROR("invalid params\n");
		return 0;
	}

	dp_display = container_of(dp, struct dp_display_private, dp_display);

	return dp_link_bit_depth_to_bpp(
		dp_display->link->test_video.test_bit_depth);
}

void msm_dp_snapshot(struct msm_disp_state *disp_state, struct msm_dp *dp)
{
	struct dp_display_private *dp_display;
	struct drm_device *drm;

	dp_display = container_of(dp, struct dp_display_private, dp_display);
	drm = dp->drm_dev;

	/*
	 * if we are reading registers we need the link clocks to be on
	 * however till DP cable is connected this will not happen as we
	 * do not know the resolution to power up with. Hence check the
	 * power_on status before dumping DP registers to avoid crash due
	 * to unclocked access
	 */
	mutex_lock(&dp_display->event_mutex);

	if (!dp->power_on) {
		mutex_unlock(&dp_display->event_mutex);
		return;
	}

	dp_catalog_snapshot(dp_display->catalog, disp_state);

	mutex_unlock(&dp_display->event_mutex);
}

static void dp_display_config_hpd(struct dp_display_private *dp)
{

	dp_display_host_init(dp, true);
	dp_catalog_ctrl_hpd_config(dp->catalog);

	/* Enable interrupt first time
	 * we are leaving dp clocks on during disconnect
	 * and never disable interrupt
	 */
	enable_irq(dp->irq);
}

static int hpd_event_thread(void *data)
{
	struct dp_display_private *dp_priv;
	unsigned long flag;
	struct dp_event *todo;
	int timeout_mode = 0;

	dp_priv = (struct dp_display_private *)data;

	while (1) {
		if (timeout_mode) {
			wait_event_timeout(dp_priv->event_q,
				(dp_priv->event_pndx == dp_priv->event_gndx),
						EVENT_TIMEOUT);
		} else {
			wait_event_interruptible(dp_priv->event_q,
				(dp_priv->event_pndx != dp_priv->event_gndx));
		}
		spin_lock_irqsave(&dp_priv->event_lock, flag);
		todo = &dp_priv->event_list[dp_priv->event_gndx];
		if (todo->delay) {
			struct dp_event *todo_next;

			dp_priv->event_gndx++;
			dp_priv->event_gndx %= DP_EVENT_Q_MAX;

			/* re enter delay event into q */
			todo_next = &dp_priv->event_list[dp_priv->event_pndx++];
			dp_priv->event_pndx %= DP_EVENT_Q_MAX;
			todo_next->event_id = todo->event_id;
			todo_next->data = todo->data;
			todo_next->delay = todo->delay - 1;

			/* clean up older event */
			todo->event_id = EV_NO_EVENT;
			todo->delay = 0;

			/* switch to timeout mode */
			timeout_mode = 1;
			spin_unlock_irqrestore(&dp_priv->event_lock, flag);
			continue;
		}

		/* timeout with no events in q */
		if (dp_priv->event_pndx == dp_priv->event_gndx) {
			spin_unlock_irqrestore(&dp_priv->event_lock, flag);
			continue;
		}

		dp_priv->event_gndx++;
		dp_priv->event_gndx %= DP_EVENT_Q_MAX;
		timeout_mode = 0;
		spin_unlock_irqrestore(&dp_priv->event_lock, flag);

		switch (todo->event_id) {
		case EV_HPD_INIT_SETUP:
			dp_display_config_hpd(dp_priv);
			break;
		case EV_HPD_PLUG_INT:
			dp_hpd_plug_handle(dp_priv, todo->data);
			break;
		case EV_HPD_UNPLUG_INT:
			dp_hpd_unplug_handle(dp_priv, todo->data);
			break;
		case EV_IRQ_HPD_INT:
			dp_irq_hpd_handle(dp_priv, todo->data);
			break;
		case EV_HPD_REPLUG_INT:
			/* do nothing */
			break;
		case EV_USER_NOTIFICATION:
			dp_display_send_hpd_notification(dp_priv,
						todo->data);
			break;
		case EV_CONNECT_PENDING_TIMEOUT:
			dp_connect_pending_timeout(dp_priv,
						todo->data);
			break;
		case EV_DISCONNECT_PENDING_TIMEOUT:
			dp_disconnect_pending_timeout(dp_priv,
						todo->data);
			break;
		default:
			break;
		}
	}

	return 0;
}

static void dp_hpd_event_setup(struct dp_display_private *dp_priv)
{
	init_waitqueue_head(&dp_priv->event_q);
	spin_lock_init(&dp_priv->event_lock);

	kthread_run(hpd_event_thread, dp_priv, "dp_hpd_handler");
}

static irqreturn_t dp_display_irq_handler(int irq, void *dev_id)
{
	struct dp_display_private *dp = dev_id;
	irqreturn_t ret = IRQ_HANDLED;
	u32 hpd_isr_status;

	if (!dp) {
		DRM_ERROR("invalid data\n");
		return IRQ_NONE;
	}

	hpd_isr_status = dp_catalog_hpd_get_intr_status(dp->catalog);

	if (hpd_isr_status & 0x0F) {
		/* hpd related interrupts */
		if (hpd_isr_status & DP_DP_HPD_PLUG_INT_MASK ||
			hpd_isr_status & DP_DP_HPD_REPLUG_INT_MASK) {
			dp_add_event(dp, EV_HPD_PLUG_INT, 0, 0);
		}

		if (hpd_isr_status & DP_DP_IRQ_HPD_INT_MASK) {
			/* stop sentinel connect pending checking */
			dp_del_event(dp, EV_CONNECT_PENDING_TIMEOUT);
			dp_add_event(dp, EV_IRQ_HPD_INT, 0, 0);
		}

		if (hpd_isr_status & DP_DP_HPD_REPLUG_INT_MASK)
			dp_add_event(dp, EV_HPD_REPLUG_INT, 0, 0);

		if (hpd_isr_status & DP_DP_HPD_UNPLUG_INT_MASK)
			dp_add_event(dp, EV_HPD_UNPLUG_INT, 0, 0);
	}

	/* DP controller isr */
	dp_ctrl_isr(dp->ctrl);

	/* DP aux isr */
	dp_aux_isr(dp->aux);

	return ret;
}

int dp_display_request_irq(struct msm_dp *dp_display)
{
	int rc = 0;
	struct dp_display_private *dp;

	if (!dp_display) {
		DRM_ERROR("invalid input\n");
		return -EINVAL;
	}

	dp = container_of(dp_display, struct dp_display_private, dp_display);

	dp->irq = irq_of_parse_and_map(dp->pdev->dev.of_node, 0);
	if (dp->irq < 0) {
		rc = dp->irq;
		DRM_ERROR("failed to get irq: %d\n", rc);
		return rc;
	}

	rc = devm_request_irq(&dp->pdev->dev, dp->irq,
			dp_display_irq_handler,
			IRQF_TRIGGER_HIGH, "dp_display_isr", dp);
	if (rc < 0) {
		DRM_ERROR("failed to request IRQ%u: %d\n",
				dp->irq, rc);
		return rc;
	}
	disable_irq(dp->irq);

	return 0;
}

static int dp_display_probe(struct platform_device *pdev)
{
	int rc = 0;
	struct dp_display_private *dp;

	if (!pdev || !pdev->dev.of_node) {
		DRM_ERROR("pdev not found\n");
		return -ENODEV;
	}

	dp = devm_kzalloc(&pdev->dev, sizeof(*dp), GFP_KERNEL);
	if (!dp)
		return -ENOMEM;

	dp->pdev = pdev;
	dp->name = "drm_dp";

	rc = dp_init_sub_modules(dp);
	if (rc) {
		DRM_ERROR("init sub module failed\n");
		return -EPROBE_DEFER;
	}

	mutex_init(&dp->event_mutex);
	g_dp_display = &dp->dp_display;

	/* Store DP audio handle inside DP display */
	g_dp_display->dp_audio = dp->audio;

	init_completion(&dp->audio_comp);

	platform_set_drvdata(pdev, g_dp_display);

	rc = component_add(&pdev->dev, &dp_display_comp_ops);
	if (rc) {
		DRM_ERROR("component add failed, rc=%d\n", rc);
		dp_display_deinit_sub_modules(dp);
	}

	return rc;
}

static int dp_display_remove(struct platform_device *pdev)
{
	struct dp_display_private *dp;

	dp = container_of(g_dp_display,
			struct dp_display_private, dp_display);

	dp_display_deinit_sub_modules(dp);

	component_del(&pdev->dev, &dp_display_comp_ops);
	platform_set_drvdata(pdev, NULL);

	return 0;
}

static int dp_pm_resume(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct msm_dp *dp_display = platform_get_drvdata(pdev);
	struct dp_display_private *dp;
	u32 status;

	dp = container_of(dp_display, struct dp_display_private, dp_display);

	mutex_lock(&dp->event_mutex);

	/* start from disconnected state */
	dp->hpd_state = ST_DISCONNECTED;

	/* turn on dp ctrl/phy */
	dp_display_host_init(dp, true);

	dp_catalog_ctrl_hpd_config(dp->catalog);

	status = dp_catalog_link_is_connected(dp->catalog);

	/*
	 * can not declared display is connected unless
	 * HDMI cable is plugged in and sink_count of
	 * dongle become 1
	 */
	if (status && dp->link->sink_count)
		dp->dp_display.is_connected = true;
	else
		dp->dp_display.is_connected = false;

	mutex_unlock(&dp->event_mutex);

	return 0;
}

static int dp_pm_suspend(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct msm_dp *dp_display = platform_get_drvdata(pdev);
	struct dp_display_private *dp;

	dp = container_of(dp_display, struct dp_display_private, dp_display);

	mutex_lock(&dp->event_mutex);

	if (dp->core_initialized == true) {
		/* mainlink enabled */
		if (dp_power_clk_status(dp->power, DP_CTRL_PM))
			dp_ctrl_off_link_stream(dp->ctrl);

		dp_display_host_deinit(dp);
	}

	dp->hpd_state = ST_SUSPENDED;

	/* host_init will be called at pm_resume */
	dp->core_initialized = false;

	mutex_unlock(&dp->event_mutex);

	return 0;
}

static int dp_pm_prepare(struct device *dev)
{
	return 0;
}

static void dp_pm_complete(struct device *dev)
{

}

static const struct dev_pm_ops dp_pm_ops = {
	.suspend = dp_pm_suspend,
	.resume =  dp_pm_resume,
	.prepare = dp_pm_prepare,
	.complete = dp_pm_complete,
};

static struct platform_driver dp_display_driver = {
	.probe  = dp_display_probe,
	.remove = dp_display_remove,
	.driver = {
		.name = "msm-dp-display",
		.of_match_table = dp_dt_match,
		.suppress_bind_attrs = true,
		.pm = &dp_pm_ops,
	},
};

int __init msm_dp_register(void)
{
	int ret;

	ret = platform_driver_register(&dp_display_driver);
	if (ret)
		DRM_ERROR("Dp display driver register failed");

	return ret;
}

void __exit msm_dp_unregister(void)
{
	platform_driver_unregister(&dp_display_driver);
}

void msm_dp_irq_postinstall(struct msm_dp *dp_display)
{
	struct dp_display_private *dp;

	if (!dp_display)
		return;

	dp = container_of(dp_display, struct dp_display_private, dp_display);

	dp_hpd_event_setup(dp);

	dp_add_event(dp, EV_HPD_INIT_SETUP, 0, 100);
}

void msm_dp_debugfs_init(struct msm_dp *dp_display, struct drm_minor *minor)
{
	struct dp_display_private *dp;
	struct device *dev;
	int rc;

	dp = container_of(dp_display, struct dp_display_private, dp_display);
	dev = &dp->pdev->dev;

	dp->debug = dp_debug_get(dev, dp->panel, dp->usbpd,
					dp->link, &dp->dp_display.connector,
					minor);
	if (IS_ERR(dp->debug)) {
		rc = PTR_ERR(dp->debug);
		DRM_ERROR("failed to initialize debug, rc = %d\n", rc);
		dp->debug = NULL;
	}
}

int msm_dp_modeset_init(struct msm_dp *dp_display, struct drm_device *dev,
			struct drm_encoder *encoder)
{
	struct msm_drm_private *priv;
	int ret;

	if (WARN_ON(!encoder) || WARN_ON(!dp_display) || WARN_ON(!dev))
		return -EINVAL;

	priv = dev->dev_private;
	dp_display->drm_dev = dev;

	ret = dp_display_request_irq(dp_display);
	if (ret) {
		DRM_ERROR("request_irq failed, ret=%d\n", ret);
		return ret;
	}

	dp_display->encoder = encoder;

	dp_display->connector = dp_drm_connector_init(dp_display);
	if (IS_ERR(dp_display->connector)) {
		ret = PTR_ERR(dp_display->connector);
		DRM_DEV_ERROR(dev->dev,
			"failed to create dp connector: %d\n", ret);
		dp_display->connector = NULL;
		return ret;
	}

	priv->connectors[priv->num_connectors++] = dp_display->connector;
	return 0;
}

int msm_dp_display_enable(struct msm_dp *dp, struct drm_encoder *encoder)
{
	int rc = 0;
	struct dp_display_private *dp_display;
	u32 state;

	dp_display = container_of(dp, struct dp_display_private, dp_display);
	if (!dp_display->dp_mode.drm_mode.clock) {
		DRM_ERROR("invalid params\n");
		return -EINVAL;
	}

	mutex_lock(&dp_display->event_mutex);

	/* stop sentinel checking */
	dp_del_event(dp_display, EV_CONNECT_PENDING_TIMEOUT);

	rc = dp_display_set_mode(dp, &dp_display->dp_mode);
	if (rc) {
		DRM_ERROR("Failed to perform a mode set, rc=%d\n", rc);
		mutex_unlock(&dp_display->event_mutex);
		return rc;
	}

	rc = dp_display_prepare(dp);
	if (rc) {
		DRM_ERROR("DP display prepare failed, rc=%d\n", rc);
		mutex_unlock(&dp_display->event_mutex);
		return rc;
	}

	state =  dp_display->hpd_state;

	if (state == ST_DISPLAY_OFF)
		dp_display_host_init(dp_display, true);

	dp_display_enable(dp_display, 0);

	rc = dp_display_post_enable(dp);
	if (rc) {
		DRM_ERROR("DP display post enable failed, rc=%d\n", rc);
		dp_display_disable(dp_display, 0);
		dp_display_unprepare(dp);
	}

	/* manual kick off plug event to train link */
	if (state == ST_DISPLAY_OFF)
		dp_add_event(dp_display, EV_IRQ_HPD_INT, 0, 0);

	/* completed connection */
	dp_display->hpd_state = ST_CONNECTED;

	mutex_unlock(&dp_display->event_mutex);

	return rc;
}

int msm_dp_display_pre_disable(struct msm_dp *dp, struct drm_encoder *encoder)
{
	struct dp_display_private *dp_display;

	dp_display = container_of(dp, struct dp_display_private, dp_display);

	dp_ctrl_push_idle(dp_display->ctrl);

	return 0;
}

int msm_dp_display_disable(struct msm_dp *dp, struct drm_encoder *encoder)
{
	int rc = 0;
	u32 state;
	struct dp_display_private *dp_display;

	dp_display = container_of(dp, struct dp_display_private, dp_display);

	mutex_lock(&dp_display->event_mutex);

	/* stop sentinel checking */
	dp_del_event(dp_display, EV_DISCONNECT_PENDING_TIMEOUT);

	dp_display_disable(dp_display, 0);

	rc = dp_display_unprepare(dp);
	if (rc)
		DRM_ERROR("DP display unprepare failed, rc=%d\n", rc);

	state =  dp_display->hpd_state;
	if (state == ST_DISCONNECT_PENDING) {
		/* completed disconnection */
		dp_display->hpd_state = ST_DISCONNECTED;
	} else {
		dp_display->hpd_state = ST_DISPLAY_OFF;
	}

	mutex_unlock(&dp_display->event_mutex);
	return rc;
}

void msm_dp_display_mode_set(struct msm_dp *dp, struct drm_encoder *encoder,
				struct drm_display_mode *mode,
				struct drm_display_mode *adjusted_mode)
{
	struct dp_display_private *dp_display;

	dp_display = container_of(dp, struct dp_display_private, dp_display);

	memset(&dp_display->dp_mode, 0x0, sizeof(struct dp_display_mode));

	if (dp_display_check_video_test(dp))
		dp_display->dp_mode.bpp = dp_display_get_test_bpp(dp);
	else /* Default num_components per pixel = 3 */
		dp_display->dp_mode.bpp = dp->connector->display_info.bpc * 3;

	if (!dp_display->dp_mode.bpp)
		dp_display->dp_mode.bpp = 24; /* Default bpp */

	drm_mode_copy(&dp_display->dp_mode.drm_mode, adjusted_mode);

	dp_display->dp_mode.v_active_low =
		!!(dp_display->dp_mode.drm_mode.flags & DRM_MODE_FLAG_NVSYNC);

	dp_display->dp_mode.h_active_low =
		!!(dp_display->dp_mode.drm_mode.flags & DRM_MODE_FLAG_NHSYNC);
}
