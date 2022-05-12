// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2015, The Linux Foundation. All rights reserved.
 */

#include "dsi.h"

struct drm_encoder *msm_dsi_get_encoder(struct msm_dsi *msm_dsi)
{
	if (!msm_dsi || !msm_dsi_device_connected(msm_dsi))
		return NULL;

	return msm_dsi->encoder;
}

static int dsi_get_phy(struct msm_dsi *msm_dsi)
{
	struct platform_device *pdev = msm_dsi->pdev;
	struct platform_device *phy_pdev;
	struct device_node *phy_node;

	phy_node = of_parse_phandle(pdev->dev.of_node, "phys", 0);
	if (!phy_node) {
		DRM_DEV_ERROR(&pdev->dev, "cannot find phy device\n");
		return -ENXIO;
	}

	phy_pdev = of_find_device_by_node(phy_node);
	if (phy_pdev)
		msm_dsi->phy = platform_get_drvdata(phy_pdev);

	of_node_put(phy_node);

	if (!phy_pdev || !msm_dsi->phy) {
		DRM_DEV_ERROR(&pdev->dev, "%s: phy driver is not ready\n", __func__);
		return -EPROBE_DEFER;
	}

	msm_dsi->phy_dev = get_device(&phy_pdev->dev);

	return 0;
}

static void dsi_destroy(struct msm_dsi *msm_dsi)
{
	if (!msm_dsi)
		return;

	msm_dsi_manager_unregister(msm_dsi);

	if (msm_dsi->phy_dev) {
		put_device(msm_dsi->phy_dev);
		msm_dsi->phy = NULL;
		msm_dsi->phy_dev = NULL;
	}

	if (msm_dsi->host) {
		msm_dsi_host_destroy(msm_dsi->host);
		msm_dsi->host = NULL;
	}

	platform_set_drvdata(msm_dsi->pdev, NULL);
}

static struct msm_dsi *dsi_init(struct platform_device *pdev)
{
	struct msm_dsi *msm_dsi;
	int ret;

	if (!pdev)
		return ERR_PTR(-ENXIO);

	msm_dsi = devm_kzalloc(&pdev->dev, sizeof(*msm_dsi), GFP_KERNEL);
	if (!msm_dsi)
		return ERR_PTR(-ENOMEM);
	DBG("dsi probed=%p", msm_dsi);

	msm_dsi->id = -1;
	msm_dsi->pdev = pdev;
	platform_set_drvdata(pdev, msm_dsi);

	/* Init dsi host */
	ret = msm_dsi_host_init(msm_dsi);
	if (ret)
		goto destroy_dsi;

	/* GET dsi PHY */
	ret = dsi_get_phy(msm_dsi);
	if (ret)
		goto destroy_dsi;

	/* Register to dsi manager */
	ret = msm_dsi_manager_register(msm_dsi);
	if (ret)
		goto destroy_dsi;

	return msm_dsi;

destroy_dsi:
	dsi_destroy(msm_dsi);
	return ERR_PTR(ret);
}

static int dsi_bind(struct device *dev, struct device *master, void *data)
{
	struct drm_device *drm = dev_get_drvdata(master);
	struct msm_drm_private *priv = drm->dev_private;
	struct platform_device *pdev = to_platform_device(dev);
	struct msm_dsi *msm_dsi;

	DBG("");
	msm_dsi = dsi_init(pdev);
	if (IS_ERR(msm_dsi)) {
		/* Don't fail the bind if the dsi port is not connected */
		if (PTR_ERR(msm_dsi) == -ENODEV)
			return 0;
		else
			return PTR_ERR(msm_dsi);
	}

	priv->dsi[msm_dsi->id] = msm_dsi;

	return 0;
}

static void dsi_unbind(struct device *dev, struct device *master,
		void *data)
{
	struct drm_device *drm = dev_get_drvdata(master);
	struct msm_drm_private *priv = drm->dev_private;
	struct msm_dsi *msm_dsi = dev_get_drvdata(dev);
	int id = msm_dsi->id;

	if (priv->dsi[id]) {
		dsi_destroy(msm_dsi);
		priv->dsi[id] = NULL;
	}
}

static const struct component_ops dsi_ops = {
	.bind   = dsi_bind,
	.unbind = dsi_unbind,
};

static int dsi_dev_probe(struct platform_device *pdev)
{
	return component_add(&pdev->dev, &dsi_ops);
}

static int dsi_dev_remove(struct platform_device *pdev)
{
	DBG("");
	component_del(&pdev->dev, &dsi_ops);
	return 0;
}

static const struct of_device_id dt_match[] = {
	{ .compatible = "qcom,mdss-dsi-ctrl" },
	{}
};

static const struct dev_pm_ops dsi_pm_ops = {
	SET_RUNTIME_PM_OPS(msm_dsi_runtime_suspend, msm_dsi_runtime_resume, NULL)
	SET_SYSTEM_SLEEP_PM_OPS(pm_runtime_force_suspend,
				pm_runtime_force_resume)
};

static struct platform_driver dsi_driver = {
	.probe = dsi_dev_probe,
	.remove = dsi_dev_remove,
	.driver = {
		.name = "msm_dsi",
		.of_match_table = dt_match,
		.pm = &dsi_pm_ops,
	},
};

void __init msm_dsi_register(void)
{
	DBG("");
	msm_dsi_phy_driver_register();
	platform_driver_register(&dsi_driver);
}

void __exit msm_dsi_unregister(void)
{
	DBG("");
	msm_dsi_phy_driver_unregister();
	platform_driver_unregister(&dsi_driver);
}

int msm_dsi_modeset_init(struct msm_dsi *msm_dsi, struct drm_device *dev,
			 struct drm_encoder *encoder)
{
	struct msm_drm_private *priv;
	struct drm_bridge *ext_bridge;
	int ret;

	if (WARN_ON(!encoder) || WARN_ON(!msm_dsi) || WARN_ON(!dev))
		return -EINVAL;

	priv = dev->dev_private;
	msm_dsi->dev = dev;

	ret = msm_dsi_host_modeset_init(msm_dsi->host, dev);
	if (ret) {
		DRM_DEV_ERROR(dev->dev, "failed to modeset init host: %d\n", ret);
		goto fail;
	}

	if (!msm_dsi_manager_validate_current_config(msm_dsi->id))
		goto fail;

	msm_dsi->encoder = encoder;

	msm_dsi->bridge = msm_dsi_manager_bridge_init(msm_dsi->id);
	if (IS_ERR(msm_dsi->bridge)) {
		ret = PTR_ERR(msm_dsi->bridge);
		DRM_DEV_ERROR(dev->dev, "failed to create dsi bridge: %d\n", ret);
		msm_dsi->bridge = NULL;
		goto fail;
	}

	/*
	 * check if the dsi encoder output is connected to a panel or an
	 * external bridge. We create a connector only if we're connected to a
	 * drm_panel device. When we're connected to an external bridge, we
	 * assume that the drm_bridge driver will create the connector itself.
	 */
	ext_bridge = msm_dsi_host_get_bridge(msm_dsi->host);

	if (ext_bridge)
		msm_dsi->connector =
			msm_dsi_manager_ext_bridge_init(msm_dsi->id);
	else
		msm_dsi->connector =
			msm_dsi_manager_connector_init(msm_dsi->id);

	if (IS_ERR(msm_dsi->connector)) {
		ret = PTR_ERR(msm_dsi->connector);
		DRM_DEV_ERROR(dev->dev,
			"failed to create dsi connector: %d\n", ret);
		msm_dsi->connector = NULL;
		goto fail;
	}

	msm_dsi_manager_setup_encoder(msm_dsi->id);

	priv->bridges[priv->num_bridges++]       = msm_dsi->bridge;
	priv->connectors[priv->num_connectors++] = msm_dsi->connector;

	return 0;
fail:
	/* bridge/connector are normally destroyed by drm: */
	if (msm_dsi->bridge) {
		msm_dsi_manager_bridge_destroy(msm_dsi->bridge);
		msm_dsi->bridge = NULL;
	}

	/* don't destroy connector if we didn't make it */
	if (msm_dsi->connector && !msm_dsi->external_bridge)
		msm_dsi->connector->funcs->destroy(msm_dsi->connector);

	msm_dsi->connector = NULL;

	return ret;
}

void msm_dsi_snapshot(struct msm_disp_state *disp_state, struct msm_dsi *msm_dsi)
{
	msm_dsi_host_snapshot(disp_state, msm_dsi->host);
	msm_dsi_phy_snapshot(disp_state, msm_dsi->phy);
}

