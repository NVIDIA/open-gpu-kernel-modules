// SPDX-License-Identifier: GPL-2.0-only
/*
 * NXP PTN3460 DP/LVDS bridge driver
 *
 * Copyright (C) 2013 Google, Inc.
 */

#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/of.h>
#include <drm/drm_atomic_helper.h>
#include <drm/drm_bridge.h>
#include <drm/drm_crtc.h>
#include <drm/drm_edid.h>
#include <drm/drm_of.h>
#include <drm/drm_panel.h>
#include <drm/drm_print.h>
#include <drm/drm_probe_helper.h>

#define PTN3460_EDID_ADDR			0x0
#define PTN3460_EDID_EMULATION_ADDR		0x84
#define PTN3460_EDID_ENABLE_EMULATION		0
#define PTN3460_EDID_EMULATION_SELECTION	1
#define PTN3460_EDID_SRAM_LOAD_ADDR		0x85

struct ptn3460_bridge {
	struct drm_connector connector;
	struct i2c_client *client;
	struct drm_bridge bridge;
	struct drm_bridge *panel_bridge;
	struct gpio_desc *gpio_pd_n;
	struct gpio_desc *gpio_rst_n;
	u32 edid_emulation;
	bool enabled;
};

static inline struct ptn3460_bridge *
		bridge_to_ptn3460(struct drm_bridge *bridge)
{
	return container_of(bridge, struct ptn3460_bridge, bridge);
}

static inline struct ptn3460_bridge *
		connector_to_ptn3460(struct drm_connector *connector)
{
	return container_of(connector, struct ptn3460_bridge, connector);
}

static int ptn3460_read_bytes(struct ptn3460_bridge *ptn_bridge, char addr,
		u8 *buf, int len)
{
	int ret;

	ret = i2c_master_send(ptn_bridge->client, &addr, 1);
	if (ret <= 0) {
		DRM_ERROR("Failed to send i2c command, ret=%d\n", ret);
		return ret;
	}

	ret = i2c_master_recv(ptn_bridge->client, buf, len);
	if (ret <= 0) {
		DRM_ERROR("Failed to recv i2c data, ret=%d\n", ret);
		return ret;
	}

	return 0;
}

static int ptn3460_write_byte(struct ptn3460_bridge *ptn_bridge, char addr,
		char val)
{
	int ret;
	char buf[2];

	buf[0] = addr;
	buf[1] = val;

	ret = i2c_master_send(ptn_bridge->client, buf, ARRAY_SIZE(buf));
	if (ret <= 0) {
		DRM_ERROR("Failed to send i2c command, ret=%d\n", ret);
		return ret;
	}

	return 0;
}

static int ptn3460_select_edid(struct ptn3460_bridge *ptn_bridge)
{
	int ret;
	char val;

	/* Load the selected edid into SRAM (accessed at PTN3460_EDID_ADDR) */
	ret = ptn3460_write_byte(ptn_bridge, PTN3460_EDID_SRAM_LOAD_ADDR,
			ptn_bridge->edid_emulation);
	if (ret) {
		DRM_ERROR("Failed to transfer EDID to sram, ret=%d\n", ret);
		return ret;
	}

	/* Enable EDID emulation and select the desired EDID */
	val = 1 << PTN3460_EDID_ENABLE_EMULATION |
		ptn_bridge->edid_emulation << PTN3460_EDID_EMULATION_SELECTION;

	ret = ptn3460_write_byte(ptn_bridge, PTN3460_EDID_EMULATION_ADDR, val);
	if (ret) {
		DRM_ERROR("Failed to write EDID value, ret=%d\n", ret);
		return ret;
	}

	return 0;
}

static void ptn3460_pre_enable(struct drm_bridge *bridge)
{
	struct ptn3460_bridge *ptn_bridge = bridge_to_ptn3460(bridge);
	int ret;

	if (ptn_bridge->enabled)
		return;

	gpiod_set_value(ptn_bridge->gpio_pd_n, 1);

	gpiod_set_value(ptn_bridge->gpio_rst_n, 0);
	usleep_range(10, 20);
	gpiod_set_value(ptn_bridge->gpio_rst_n, 1);

	/*
	 * There's a bug in the PTN chip where it falsely asserts hotplug before
	 * it is fully functional. We're forced to wait for the maximum start up
	 * time specified in the chip's datasheet to make sure we're really up.
	 */
	msleep(90);

	ret = ptn3460_select_edid(ptn_bridge);
	if (ret)
		DRM_ERROR("Select EDID failed ret=%d\n", ret);

	ptn_bridge->enabled = true;
}

static void ptn3460_disable(struct drm_bridge *bridge)
{
	struct ptn3460_bridge *ptn_bridge = bridge_to_ptn3460(bridge);

	if (!ptn_bridge->enabled)
		return;

	ptn_bridge->enabled = false;

	gpiod_set_value(ptn_bridge->gpio_rst_n, 1);
	gpiod_set_value(ptn_bridge->gpio_pd_n, 0);
}


static struct edid *ptn3460_get_edid(struct drm_bridge *bridge,
				     struct drm_connector *connector)
{
	struct ptn3460_bridge *ptn_bridge = bridge_to_ptn3460(bridge);
	bool power_off;
	u8 *edid;
	int ret;

	power_off = !ptn_bridge->enabled;
	ptn3460_pre_enable(&ptn_bridge->bridge);

	edid = kmalloc(EDID_LENGTH, GFP_KERNEL);
	if (!edid) {
		DRM_ERROR("Failed to allocate EDID\n");
		goto out;
	}

	ret = ptn3460_read_bytes(ptn_bridge, PTN3460_EDID_ADDR, edid,
				 EDID_LENGTH);
	if (ret) {
		kfree(edid);
		edid = NULL;
		goto out;
	}

out:
	if (power_off)
		ptn3460_disable(&ptn_bridge->bridge);

	return (struct edid *)edid;
}

static int ptn3460_connector_get_modes(struct drm_connector *connector)
{
	struct ptn3460_bridge *ptn_bridge = connector_to_ptn3460(connector);
	struct edid *edid;
	int num_modes;

	edid = ptn3460_get_edid(&ptn_bridge->bridge, connector);
	drm_connector_update_edid_property(connector, edid);
	num_modes = drm_add_edid_modes(connector, edid);
	kfree(edid);

	return num_modes;
}

static const struct drm_connector_helper_funcs ptn3460_connector_helper_funcs = {
	.get_modes = ptn3460_connector_get_modes,
};

static const struct drm_connector_funcs ptn3460_connector_funcs = {
	.fill_modes = drm_helper_probe_single_connector_modes,
	.destroy = drm_connector_cleanup,
	.reset = drm_atomic_helper_connector_reset,
	.atomic_duplicate_state = drm_atomic_helper_connector_duplicate_state,
	.atomic_destroy_state = drm_atomic_helper_connector_destroy_state,
};

static int ptn3460_bridge_attach(struct drm_bridge *bridge,
				 enum drm_bridge_attach_flags flags)
{
	struct ptn3460_bridge *ptn_bridge = bridge_to_ptn3460(bridge);
	int ret;

	/* Let this driver create connector if requested */
	ret = drm_bridge_attach(bridge->encoder, ptn_bridge->panel_bridge,
				bridge, flags | DRM_BRIDGE_ATTACH_NO_CONNECTOR);
	if (ret < 0)
		return ret;

	if (flags & DRM_BRIDGE_ATTACH_NO_CONNECTOR)
		return 0;

	if (!bridge->encoder) {
		DRM_ERROR("Parent encoder object not found");
		return -ENODEV;
	}

	ptn_bridge->connector.polled = DRM_CONNECTOR_POLL_HPD;
	ret = drm_connector_init(bridge->dev, &ptn_bridge->connector,
			&ptn3460_connector_funcs, DRM_MODE_CONNECTOR_LVDS);
	if (ret) {
		DRM_ERROR("Failed to initialize connector with drm\n");
		return ret;
	}
	drm_connector_helper_add(&ptn_bridge->connector,
					&ptn3460_connector_helper_funcs);
	drm_connector_register(&ptn_bridge->connector);
	drm_connector_attach_encoder(&ptn_bridge->connector,
							bridge->encoder);

	drm_helper_hpd_irq_event(ptn_bridge->connector.dev);

	return ret;
}

static const struct drm_bridge_funcs ptn3460_bridge_funcs = {
	.pre_enable = ptn3460_pre_enable,
	.disable = ptn3460_disable,
	.attach = ptn3460_bridge_attach,
	.get_edid = ptn3460_get_edid,
};

static int ptn3460_probe(struct i2c_client *client,
				const struct i2c_device_id *id)
{
	struct device *dev = &client->dev;
	struct ptn3460_bridge *ptn_bridge;
	struct drm_bridge *panel_bridge;
	struct drm_panel *panel;
	int ret;

	ptn_bridge = devm_kzalloc(dev, sizeof(*ptn_bridge), GFP_KERNEL);
	if (!ptn_bridge) {
		return -ENOMEM;
	}

	ret = drm_of_find_panel_or_bridge(dev->of_node, 0, 0, &panel, NULL);
	if (ret)
		return ret;

	panel_bridge = devm_drm_panel_bridge_add(dev, panel);
	if (IS_ERR(panel_bridge))
		return PTR_ERR(panel_bridge);

	ptn_bridge->panel_bridge = panel_bridge;
	ptn_bridge->client = client;

	ptn_bridge->gpio_pd_n = devm_gpiod_get(&client->dev, "powerdown",
					       GPIOD_OUT_HIGH);
	if (IS_ERR(ptn_bridge->gpio_pd_n)) {
		ret = PTR_ERR(ptn_bridge->gpio_pd_n);
		dev_err(dev, "cannot get gpio_pd_n %d\n", ret);
		return ret;
	}

	/*
	 * Request the reset pin low to avoid the bridge being
	 * initialized prematurely
	 */
	ptn_bridge->gpio_rst_n = devm_gpiod_get(&client->dev, "reset",
						GPIOD_OUT_LOW);
	if (IS_ERR(ptn_bridge->gpio_rst_n)) {
		ret = PTR_ERR(ptn_bridge->gpio_rst_n);
		DRM_ERROR("cannot get gpio_rst_n %d\n", ret);
		return ret;
	}

	ret = of_property_read_u32(dev->of_node, "edid-emulation",
			&ptn_bridge->edid_emulation);
	if (ret) {
		dev_err(dev, "Can't read EDID emulation value\n");
		return ret;
	}

	ptn_bridge->bridge.funcs = &ptn3460_bridge_funcs;
	ptn_bridge->bridge.ops = DRM_BRIDGE_OP_EDID;
	ptn_bridge->bridge.type = DRM_MODE_CONNECTOR_LVDS;
	ptn_bridge->bridge.of_node = dev->of_node;
	drm_bridge_add(&ptn_bridge->bridge);

	i2c_set_clientdata(client, ptn_bridge);

	return 0;
}

static int ptn3460_remove(struct i2c_client *client)
{
	struct ptn3460_bridge *ptn_bridge = i2c_get_clientdata(client);

	drm_bridge_remove(&ptn_bridge->bridge);

	return 0;
}

static const struct i2c_device_id ptn3460_i2c_table[] = {
	{"ptn3460", 0},
	{},
};
MODULE_DEVICE_TABLE(i2c, ptn3460_i2c_table);

static const struct of_device_id ptn3460_match[] = {
	{ .compatible = "nxp,ptn3460" },
	{},
};
MODULE_DEVICE_TABLE(of, ptn3460_match);

static struct i2c_driver ptn3460_driver = {
	.id_table	= ptn3460_i2c_table,
	.probe		= ptn3460_probe,
	.remove		= ptn3460_remove,
	.driver		= {
		.name	= "nxp,ptn3460",
		.of_match_table = ptn3460_match,
	},
};
module_i2c_driver(ptn3460_driver);

MODULE_AUTHOR("Sean Paul <seanpaul@chromium.org>");
MODULE_DESCRIPTION("NXP ptn3460 eDP-LVDS converter driver");
MODULE_LICENSE("GPL v2");
