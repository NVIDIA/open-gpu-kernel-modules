// SPDX-License-Identifier: GPL-2.0-only
/*
 * Toppoly TD028TTEC1 panel support
 *
 * Copyright (C) 2008 Nokia Corporation
 * Author: Tomi Valkeinen <tomi.valkeinen@nokia.com>
 *
 * Neo 1973 code (jbt6k74.c):
 * Copyright (C) 2006-2007 by OpenMoko, Inc.
 * Author: Harald Welte <laforge@openmoko.org>
 *
 * Ported and adapted from Neo 1973 U-Boot by:
 * H. Nikolaus Schaller <hns@goldelico.com>
 */

#include <linux/module.h>
#include <linux/delay.h>
#include <linux/spi/spi.h>
#include <linux/gpio.h>
#include <video/omapfb_dss.h>

struct panel_drv_data {
	struct omap_dss_device dssdev;
	struct omap_dss_device *in;

	int data_lines;

	struct omap_video_timings videomode;

	struct spi_device *spi_dev;
};

static const struct omap_video_timings td028ttec1_panel_timings = {
	.x_res		= 480,
	.y_res		= 640,
	.pixelclock	= 22153000,
	.hfp		= 24,
	.hsw		= 8,
	.hbp		= 8,
	.vfp		= 4,
	.vsw		= 2,
	.vbp		= 2,

	.vsync_level	= OMAPDSS_SIG_ACTIVE_LOW,
	.hsync_level	= OMAPDSS_SIG_ACTIVE_LOW,

	.data_pclk_edge	= OMAPDSS_DRIVE_SIG_FALLING_EDGE,
	.de_level	= OMAPDSS_SIG_ACTIVE_HIGH,
	.sync_pclk_edge	= OMAPDSS_DRIVE_SIG_RISING_EDGE,
};

#define JBT_COMMAND	0x000
#define JBT_DATA	0x100

static int jbt_ret_write_0(struct panel_drv_data *ddata, u8 reg)
{
	int rc;
	u16 tx_buf = JBT_COMMAND | reg;

	rc = spi_write(ddata->spi_dev, (u8 *)&tx_buf,
			1*sizeof(u16));
	if (rc != 0)
		dev_err(&ddata->spi_dev->dev,
			"jbt_ret_write_0 spi_write ret %d\n", rc);

	return rc;
}

static int jbt_reg_write_1(struct panel_drv_data *ddata, u8 reg, u8 data)
{
	int rc;
	u16 tx_buf[2];

	tx_buf[0] = JBT_COMMAND | reg;
	tx_buf[1] = JBT_DATA | data;
	rc = spi_write(ddata->spi_dev, (u8 *)tx_buf,
			2*sizeof(u16));
	if (rc != 0)
		dev_err(&ddata->spi_dev->dev,
			"jbt_reg_write_1 spi_write ret %d\n", rc);

	return rc;
}

static int jbt_reg_write_2(struct panel_drv_data *ddata, u8 reg, u16 data)
{
	int rc;
	u16 tx_buf[3];

	tx_buf[0] = JBT_COMMAND | reg;
	tx_buf[1] = JBT_DATA | (data >> 8);
	tx_buf[2] = JBT_DATA | (data & 0xff);

	rc = spi_write(ddata->spi_dev, (u8 *)tx_buf,
			3*sizeof(u16));

	if (rc != 0)
		dev_err(&ddata->spi_dev->dev,
			"jbt_reg_write_2 spi_write ret %d\n", rc);

	return rc;
}

enum jbt_register {
	JBT_REG_SLEEP_IN		= 0x10,
	JBT_REG_SLEEP_OUT		= 0x11,

	JBT_REG_DISPLAY_OFF		= 0x28,
	JBT_REG_DISPLAY_ON		= 0x29,

	JBT_REG_RGB_FORMAT		= 0x3a,
	JBT_REG_QUAD_RATE		= 0x3b,

	JBT_REG_POWER_ON_OFF		= 0xb0,
	JBT_REG_BOOSTER_OP		= 0xb1,
	JBT_REG_BOOSTER_MODE		= 0xb2,
	JBT_REG_BOOSTER_FREQ		= 0xb3,
	JBT_REG_OPAMP_SYSCLK		= 0xb4,
	JBT_REG_VSC_VOLTAGE		= 0xb5,
	JBT_REG_VCOM_VOLTAGE		= 0xb6,
	JBT_REG_EXT_DISPL		= 0xb7,
	JBT_REG_OUTPUT_CONTROL		= 0xb8,
	JBT_REG_DCCLK_DCEV		= 0xb9,
	JBT_REG_DISPLAY_MODE1		= 0xba,
	JBT_REG_DISPLAY_MODE2		= 0xbb,
	JBT_REG_DISPLAY_MODE		= 0xbc,
	JBT_REG_ASW_SLEW		= 0xbd,
	JBT_REG_DUMMY_DISPLAY		= 0xbe,
	JBT_REG_DRIVE_SYSTEM		= 0xbf,

	JBT_REG_SLEEP_OUT_FR_A		= 0xc0,
	JBT_REG_SLEEP_OUT_FR_B		= 0xc1,
	JBT_REG_SLEEP_OUT_FR_C		= 0xc2,
	JBT_REG_SLEEP_IN_LCCNT_D	= 0xc3,
	JBT_REG_SLEEP_IN_LCCNT_E	= 0xc4,
	JBT_REG_SLEEP_IN_LCCNT_F	= 0xc5,
	JBT_REG_SLEEP_IN_LCCNT_G	= 0xc6,

	JBT_REG_GAMMA1_FINE_1		= 0xc7,
	JBT_REG_GAMMA1_FINE_2		= 0xc8,
	JBT_REG_GAMMA1_INCLINATION	= 0xc9,
	JBT_REG_GAMMA1_BLUE_OFFSET	= 0xca,

	JBT_REG_BLANK_CONTROL		= 0xcf,
	JBT_REG_BLANK_TH_TV		= 0xd0,
	JBT_REG_CKV_ON_OFF		= 0xd1,
	JBT_REG_CKV_1_2			= 0xd2,
	JBT_REG_OEV_TIMING		= 0xd3,
	JBT_REG_ASW_TIMING_1		= 0xd4,
	JBT_REG_ASW_TIMING_2		= 0xd5,

	JBT_REG_HCLOCK_VGA		= 0xec,
	JBT_REG_HCLOCK_QVGA		= 0xed,
};

#define to_panel_data(p) container_of(p, struct panel_drv_data, dssdev)

static int td028ttec1_panel_connect(struct omap_dss_device *dssdev)
{
	struct panel_drv_data *ddata = to_panel_data(dssdev);
	struct omap_dss_device *in = ddata->in;
	int r;

	if (omapdss_device_is_connected(dssdev))
		return 0;

	r = in->ops.dpi->connect(in, dssdev);
	if (r)
		return r;

	return 0;
}

static void td028ttec1_panel_disconnect(struct omap_dss_device *dssdev)
{
	struct panel_drv_data *ddata = to_panel_data(dssdev);
	struct omap_dss_device *in = ddata->in;

	if (!omapdss_device_is_connected(dssdev))
		return;

	in->ops.dpi->disconnect(in, dssdev);
}

static int td028ttec1_panel_enable(struct omap_dss_device *dssdev)
{
	struct panel_drv_data *ddata = to_panel_data(dssdev);
	struct omap_dss_device *in = ddata->in;
	int r;

	if (!omapdss_device_is_connected(dssdev))
		return -ENODEV;

	if (omapdss_device_is_enabled(dssdev))
		return 0;

	if (ddata->data_lines)
		in->ops.dpi->set_data_lines(in, ddata->data_lines);
	in->ops.dpi->set_timings(in, &ddata->videomode);

	r = in->ops.dpi->enable(in);
	if (r)
		return r;

	dev_dbg(dssdev->dev, "td028ttec1_panel_enable() - state %d\n",
		dssdev->state);

	/* three times command zero */
	r |= jbt_ret_write_0(ddata, 0x00);
	usleep_range(1000, 2000);
	r |= jbt_ret_write_0(ddata, 0x00);
	usleep_range(1000, 2000);
	r |= jbt_ret_write_0(ddata, 0x00);
	usleep_range(1000, 2000);

	if (r) {
		dev_warn(dssdev->dev, "transfer error\n");
		goto transfer_err;
	}

	/* deep standby out */
	r |= jbt_reg_write_1(ddata, JBT_REG_POWER_ON_OFF, 0x17);

	/* RGB I/F on, RAM write off, QVGA through, SIGCON enable */
	r |= jbt_reg_write_1(ddata, JBT_REG_DISPLAY_MODE, 0x80);

	/* Quad mode off */
	r |= jbt_reg_write_1(ddata, JBT_REG_QUAD_RATE, 0x00);

	/* AVDD on, XVDD on */
	r |= jbt_reg_write_1(ddata, JBT_REG_POWER_ON_OFF, 0x16);

	/* Output control */
	r |= jbt_reg_write_2(ddata, JBT_REG_OUTPUT_CONTROL, 0xfff9);

	/* Sleep mode off */
	r |= jbt_ret_write_0(ddata, JBT_REG_SLEEP_OUT);

	/* at this point we have like 50% grey */

	/* initialize register set */
	r |= jbt_reg_write_1(ddata, JBT_REG_DISPLAY_MODE1, 0x01);
	r |= jbt_reg_write_1(ddata, JBT_REG_DISPLAY_MODE2, 0x00);
	r |= jbt_reg_write_1(ddata, JBT_REG_RGB_FORMAT, 0x60);
	r |= jbt_reg_write_1(ddata, JBT_REG_DRIVE_SYSTEM, 0x10);
	r |= jbt_reg_write_1(ddata, JBT_REG_BOOSTER_OP, 0x56);
	r |= jbt_reg_write_1(ddata, JBT_REG_BOOSTER_MODE, 0x33);
	r |= jbt_reg_write_1(ddata, JBT_REG_BOOSTER_FREQ, 0x11);
	r |= jbt_reg_write_1(ddata, JBT_REG_BOOSTER_FREQ, 0x11);
	r |= jbt_reg_write_1(ddata, JBT_REG_OPAMP_SYSCLK, 0x02);
	r |= jbt_reg_write_1(ddata, JBT_REG_VSC_VOLTAGE, 0x2b);
	r |= jbt_reg_write_1(ddata, JBT_REG_VCOM_VOLTAGE, 0x40);
	r |= jbt_reg_write_1(ddata, JBT_REG_EXT_DISPL, 0x03);
	r |= jbt_reg_write_1(ddata, JBT_REG_DCCLK_DCEV, 0x04);
	/*
	 * default of 0x02 in JBT_REG_ASW_SLEW responsible for 72Hz requirement
	 * to avoid red / blue flicker
	 */
	r |= jbt_reg_write_1(ddata, JBT_REG_ASW_SLEW, 0x04);
	r |= jbt_reg_write_1(ddata, JBT_REG_DUMMY_DISPLAY, 0x00);

	r |= jbt_reg_write_1(ddata, JBT_REG_SLEEP_OUT_FR_A, 0x11);
	r |= jbt_reg_write_1(ddata, JBT_REG_SLEEP_OUT_FR_B, 0x11);
	r |= jbt_reg_write_1(ddata, JBT_REG_SLEEP_OUT_FR_C, 0x11);
	r |= jbt_reg_write_2(ddata, JBT_REG_SLEEP_IN_LCCNT_D, 0x2040);
	r |= jbt_reg_write_2(ddata, JBT_REG_SLEEP_IN_LCCNT_E, 0x60c0);
	r |= jbt_reg_write_2(ddata, JBT_REG_SLEEP_IN_LCCNT_F, 0x1020);
	r |= jbt_reg_write_2(ddata, JBT_REG_SLEEP_IN_LCCNT_G, 0x60c0);

	r |= jbt_reg_write_2(ddata, JBT_REG_GAMMA1_FINE_1, 0x5533);
	r |= jbt_reg_write_1(ddata, JBT_REG_GAMMA1_FINE_2, 0x00);
	r |= jbt_reg_write_1(ddata, JBT_REG_GAMMA1_INCLINATION, 0x00);
	r |= jbt_reg_write_1(ddata, JBT_REG_GAMMA1_BLUE_OFFSET, 0x00);

	r |= jbt_reg_write_2(ddata, JBT_REG_HCLOCK_VGA, 0x1f0);
	r |= jbt_reg_write_1(ddata, JBT_REG_BLANK_CONTROL, 0x02);
	r |= jbt_reg_write_2(ddata, JBT_REG_BLANK_TH_TV, 0x0804);

	r |= jbt_reg_write_1(ddata, JBT_REG_CKV_ON_OFF, 0x01);
	r |= jbt_reg_write_2(ddata, JBT_REG_CKV_1_2, 0x0000);

	r |= jbt_reg_write_2(ddata, JBT_REG_OEV_TIMING, 0x0d0e);
	r |= jbt_reg_write_2(ddata, JBT_REG_ASW_TIMING_1, 0x11a4);
	r |= jbt_reg_write_1(ddata, JBT_REG_ASW_TIMING_2, 0x0e);

	r |= jbt_ret_write_0(ddata, JBT_REG_DISPLAY_ON);

	dssdev->state = OMAP_DSS_DISPLAY_ACTIVE;

transfer_err:

	return r ? -EIO : 0;
}

static void td028ttec1_panel_disable(struct omap_dss_device *dssdev)
{
	struct panel_drv_data *ddata = to_panel_data(dssdev);
	struct omap_dss_device *in = ddata->in;

	if (!omapdss_device_is_enabled(dssdev))
		return;

	dev_dbg(dssdev->dev, "td028ttec1_panel_disable()\n");

	jbt_ret_write_0(ddata, JBT_REG_DISPLAY_OFF);
	jbt_reg_write_2(ddata, JBT_REG_OUTPUT_CONTROL, 0x8002);
	jbt_ret_write_0(ddata, JBT_REG_SLEEP_IN);
	jbt_reg_write_1(ddata, JBT_REG_POWER_ON_OFF, 0x00);

	in->ops.dpi->disable(in);

	dssdev->state = OMAP_DSS_DISPLAY_DISABLED;
}

static void td028ttec1_panel_set_timings(struct omap_dss_device *dssdev,
		struct omap_video_timings *timings)
{
	struct panel_drv_data *ddata = to_panel_data(dssdev);
	struct omap_dss_device *in = ddata->in;

	ddata->videomode = *timings;
	dssdev->panel.timings = *timings;

	in->ops.dpi->set_timings(in, timings);
}

static void td028ttec1_panel_get_timings(struct omap_dss_device *dssdev,
		struct omap_video_timings *timings)
{
	struct panel_drv_data *ddata = to_panel_data(dssdev);

	*timings = ddata->videomode;
}

static int td028ttec1_panel_check_timings(struct omap_dss_device *dssdev,
		struct omap_video_timings *timings)
{
	struct panel_drv_data *ddata = to_panel_data(dssdev);
	struct omap_dss_device *in = ddata->in;

	return in->ops.dpi->check_timings(in, timings);
}

static struct omap_dss_driver td028ttec1_ops = {
	.connect	= td028ttec1_panel_connect,
	.disconnect	= td028ttec1_panel_disconnect,

	.enable		= td028ttec1_panel_enable,
	.disable	= td028ttec1_panel_disable,

	.set_timings	= td028ttec1_panel_set_timings,
	.get_timings	= td028ttec1_panel_get_timings,
	.check_timings	= td028ttec1_panel_check_timings,
};

static int td028ttec1_probe_of(struct spi_device *spi)
{
	struct device_node *node = spi->dev.of_node;
	struct panel_drv_data *ddata = dev_get_drvdata(&spi->dev);
	struct omap_dss_device *in;

	in = omapdss_of_find_source_for_first_ep(node);
	if (IS_ERR(in)) {
		dev_err(&spi->dev, "failed to find video source\n");
		return PTR_ERR(in);
	}

	ddata->in = in;

	return 0;
}

static int td028ttec1_panel_probe(struct spi_device *spi)
{
	struct panel_drv_data *ddata;
	struct omap_dss_device *dssdev;
	int r;

	dev_dbg(&spi->dev, "%s\n", __func__);

	if (!spi->dev.of_node)
		return -ENODEV;

	spi->bits_per_word = 9;
	spi->mode = SPI_MODE_3;

	r = spi_setup(spi);
	if (r < 0) {
		dev_err(&spi->dev, "spi_setup failed: %d\n", r);
		return r;
	}

	ddata = devm_kzalloc(&spi->dev, sizeof(*ddata), GFP_KERNEL);
	if (ddata == NULL)
		return -ENOMEM;

	dev_set_drvdata(&spi->dev, ddata);

	ddata->spi_dev = spi;

	r = td028ttec1_probe_of(spi);
	if (r)
		return r;

	ddata->videomode = td028ttec1_panel_timings;

	dssdev = &ddata->dssdev;
	dssdev->dev = &spi->dev;
	dssdev->driver = &td028ttec1_ops;
	dssdev->type = OMAP_DISPLAY_TYPE_DPI;
	dssdev->owner = THIS_MODULE;
	dssdev->panel.timings = ddata->videomode;
	dssdev->phy.dpi.data_lines = ddata->data_lines;

	r = omapdss_register_display(dssdev);
	if (r) {
		dev_err(&spi->dev, "Failed to register panel\n");
		goto err_reg;
	}

	return 0;

err_reg:
	omap_dss_put_device(ddata->in);
	return r;
}

static int td028ttec1_panel_remove(struct spi_device *spi)
{
	struct panel_drv_data *ddata = dev_get_drvdata(&spi->dev);
	struct omap_dss_device *dssdev = &ddata->dssdev;
	struct omap_dss_device *in = ddata->in;

	dev_dbg(&ddata->spi_dev->dev, "%s\n", __func__);

	omapdss_unregister_display(dssdev);

	td028ttec1_panel_disable(dssdev);
	td028ttec1_panel_disconnect(dssdev);

	omap_dss_put_device(in);

	return 0;
}

static const struct of_device_id td028ttec1_of_match[] = {
	{ .compatible = "omapdss,tpo,td028ttec1", },
	/* keep to not break older DTB */
	{ .compatible = "omapdss,toppoly,td028ttec1", },
	{},
};

MODULE_DEVICE_TABLE(of, td028ttec1_of_match);

static const struct spi_device_id td028ttec1_ids[] = {
	{ "toppoly,td028ttec1", 0 },
	{ "tpo,td028ttec1", 0},
	{ /* sentinel */ }
};

MODULE_DEVICE_TABLE(spi, td028ttec1_ids);

static struct spi_driver td028ttec1_spi_driver = {
	.probe		= td028ttec1_panel_probe,
	.remove		= td028ttec1_panel_remove,
	.id_table	= td028ttec1_ids,

	.driver         = {
		.name   = "panel-tpo-td028ttec1",
		.of_match_table = td028ttec1_of_match,
		.suppress_bind_attrs = true,
	},
};

module_spi_driver(td028ttec1_spi_driver);

MODULE_AUTHOR("H. Nikolaus Schaller <hns@goldelico.com>");
MODULE_DESCRIPTION("Toppoly TD028TTEC1 panel driver");
MODULE_LICENSE("GPL");
