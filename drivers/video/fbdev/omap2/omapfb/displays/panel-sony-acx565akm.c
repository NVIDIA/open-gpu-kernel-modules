// SPDX-License-Identifier: GPL-2.0-only
/*
 * Sony ACX565AKM LCD Panel driver
 *
 * Copyright (C) 2010 Nokia Corporation
 *
 * Original Driver Author: Imre Deak <imre.deak@nokia.com>
 * Based on panel-generic.c by Tomi Valkeinen <tomi.valkeinen@nokia.com>
 * Adapted to new DSS2 framework: Roger Quadros <roger.quadros@nokia.com>
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/spi/spi.h>
#include <linux/jiffies.h>
#include <linux/sched.h>
#include <linux/backlight.h>
#include <linux/fb.h>
#include <linux/gpio.h>
#include <linux/of.h>
#include <linux/of_gpio.h>

#include <video/omapfb_dss.h>
#include <video/omap-panel-data.h>

#define MIPID_CMD_READ_DISP_ID		0x04
#define MIPID_CMD_READ_RED		0x06
#define MIPID_CMD_READ_GREEN		0x07
#define MIPID_CMD_READ_BLUE		0x08
#define MIPID_CMD_READ_DISP_STATUS	0x09
#define MIPID_CMD_RDDSDR		0x0F
#define MIPID_CMD_SLEEP_IN		0x10
#define MIPID_CMD_SLEEP_OUT		0x11
#define MIPID_CMD_DISP_OFF		0x28
#define MIPID_CMD_DISP_ON		0x29
#define MIPID_CMD_WRITE_DISP_BRIGHTNESS	0x51
#define MIPID_CMD_READ_DISP_BRIGHTNESS	0x52
#define MIPID_CMD_WRITE_CTRL_DISP	0x53

#define CTRL_DISP_BRIGHTNESS_CTRL_ON	(1 << 5)
#define CTRL_DISP_AMBIENT_LIGHT_CTRL_ON	(1 << 4)
#define CTRL_DISP_BACKLIGHT_ON		(1 << 2)
#define CTRL_DISP_AUTO_BRIGHTNESS_ON	(1 << 1)

#define MIPID_CMD_READ_CTRL_DISP	0x54
#define MIPID_CMD_WRITE_CABC		0x55
#define MIPID_CMD_READ_CABC		0x56

#define MIPID_VER_LPH8923		3
#define MIPID_VER_LS041Y3		4
#define MIPID_VER_L4F00311		8
#define MIPID_VER_ACX565AKM		9

struct panel_drv_data {
	struct omap_dss_device	dssdev;
	struct omap_dss_device *in;

	int reset_gpio;
	int datapairs;

	struct omap_video_timings videomode;

	char		*name;
	int		enabled;
	int		model;
	int		revision;
	u8		display_id[3];
	unsigned	has_bc:1;
	unsigned	has_cabc:1;
	unsigned	cabc_mode;
	unsigned long	hw_guard_end;		/* next value of jiffies
						   when we can issue the
						   next sleep in/out command */
	unsigned long	hw_guard_wait;		/* max guard time in jiffies */

	struct spi_device	*spi;
	struct mutex		mutex;

	struct backlight_device *bl_dev;
};

static const struct omap_video_timings acx565akm_panel_timings = {
	.x_res		= 800,
	.y_res		= 480,
	.pixelclock	= 24000000,
	.hfp		= 28,
	.hsw		= 4,
	.hbp		= 24,
	.vfp		= 3,
	.vsw		= 3,
	.vbp		= 4,

	.vsync_level	= OMAPDSS_SIG_ACTIVE_LOW,
	.hsync_level	= OMAPDSS_SIG_ACTIVE_LOW,

	.data_pclk_edge	= OMAPDSS_DRIVE_SIG_RISING_EDGE,
	.de_level	= OMAPDSS_SIG_ACTIVE_HIGH,
	.sync_pclk_edge	= OMAPDSS_DRIVE_SIG_FALLING_EDGE,
};

#define to_panel_data(p) container_of(p, struct panel_drv_data, dssdev)

static void acx565akm_transfer(struct panel_drv_data *ddata, int cmd,
			      const u8 *wbuf, int wlen, u8 *rbuf, int rlen)
{
	struct spi_message	m;
	struct spi_transfer	*x, xfer[5];
	int			r;

	BUG_ON(ddata->spi == NULL);

	spi_message_init(&m);

	memset(xfer, 0, sizeof(xfer));
	x = &xfer[0];

	cmd &=  0xff;
	x->tx_buf = &cmd;
	x->bits_per_word = 9;
	x->len = 2;

	if (rlen > 1 && wlen == 0) {
		/*
		 * Between the command and the response data there is a
		 * dummy clock cycle. Add an extra bit after the command
		 * word to account for this.
		 */
		x->bits_per_word = 10;
		cmd <<= 1;
	}
	spi_message_add_tail(x, &m);

	if (wlen) {
		x++;
		x->tx_buf = wbuf;
		x->len = wlen;
		x->bits_per_word = 9;
		spi_message_add_tail(x, &m);
	}

	if (rlen) {
		x++;
		x->rx_buf	= rbuf;
		x->len		= rlen;
		spi_message_add_tail(x, &m);
	}

	r = spi_sync(ddata->spi, &m);
	if (r < 0)
		dev_dbg(&ddata->spi->dev, "spi_sync %d\n", r);
}

static inline void acx565akm_cmd(struct panel_drv_data *ddata, int cmd)
{
	acx565akm_transfer(ddata, cmd, NULL, 0, NULL, 0);
}

static inline void acx565akm_write(struct panel_drv_data *ddata,
			       int reg, const u8 *buf, int len)
{
	acx565akm_transfer(ddata, reg, buf, len, NULL, 0);
}

static inline void acx565akm_read(struct panel_drv_data *ddata,
			      int reg, u8 *buf, int len)
{
	acx565akm_transfer(ddata, reg, NULL, 0, buf, len);
}

static void hw_guard_start(struct panel_drv_data *ddata, int guard_msec)
{
	ddata->hw_guard_wait = msecs_to_jiffies(guard_msec);
	ddata->hw_guard_end = jiffies + ddata->hw_guard_wait;
}

static void hw_guard_wait(struct panel_drv_data *ddata)
{
	unsigned long wait = ddata->hw_guard_end - jiffies;

	if ((long)wait > 0 && wait <= ddata->hw_guard_wait) {
		set_current_state(TASK_UNINTERRUPTIBLE);
		schedule_timeout(wait);
	}
}

static void set_sleep_mode(struct panel_drv_data *ddata, int on)
{
	int cmd;

	if (on)
		cmd = MIPID_CMD_SLEEP_IN;
	else
		cmd = MIPID_CMD_SLEEP_OUT;
	/*
	 * We have to keep 120msec between sleep in/out commands.
	 * (8.2.15, 8.2.16).
	 */
	hw_guard_wait(ddata);
	acx565akm_cmd(ddata, cmd);
	hw_guard_start(ddata, 120);
}

static void set_display_state(struct panel_drv_data *ddata, int enabled)
{
	int cmd = enabled ? MIPID_CMD_DISP_ON : MIPID_CMD_DISP_OFF;

	acx565akm_cmd(ddata, cmd);
}

static int panel_enabled(struct panel_drv_data *ddata)
{
	u32 disp_status;
	int enabled;

	acx565akm_read(ddata, MIPID_CMD_READ_DISP_STATUS,
			(u8 *)&disp_status, 4);
	disp_status = __be32_to_cpu(disp_status);
	enabled = (disp_status & (1 << 17)) && (disp_status & (1 << 10));
	dev_dbg(&ddata->spi->dev,
		"LCD panel %senabled by bootloader (status 0x%04x)\n",
		enabled ? "" : "not ", disp_status);
	return enabled;
}

static int panel_detect(struct panel_drv_data *ddata)
{
	acx565akm_read(ddata, MIPID_CMD_READ_DISP_ID, ddata->display_id, 3);
	dev_dbg(&ddata->spi->dev, "MIPI display ID: %02x%02x%02x\n",
		ddata->display_id[0],
		ddata->display_id[1],
		ddata->display_id[2]);

	switch (ddata->display_id[0]) {
	case 0x10:
		ddata->model = MIPID_VER_ACX565AKM;
		ddata->name = "acx565akm";
		ddata->has_bc = 1;
		ddata->has_cabc = 1;
		break;
	case 0x29:
		ddata->model = MIPID_VER_L4F00311;
		ddata->name = "l4f00311";
		break;
	case 0x45:
		ddata->model = MIPID_VER_LPH8923;
		ddata->name = "lph8923";
		break;
	case 0x83:
		ddata->model = MIPID_VER_LS041Y3;
		ddata->name = "ls041y3";
		break;
	default:
		ddata->name = "unknown";
		dev_err(&ddata->spi->dev, "invalid display ID\n");
		return -ENODEV;
	}

	ddata->revision = ddata->display_id[1];

	dev_info(&ddata->spi->dev, "omapfb: %s rev %02x LCD detected\n",
			ddata->name, ddata->revision);

	return 0;
}

/*----------------------Backlight Control-------------------------*/

static void enable_backlight_ctrl(struct panel_drv_data *ddata, int enable)
{
	u16 ctrl;

	acx565akm_read(ddata, MIPID_CMD_READ_CTRL_DISP, (u8 *)&ctrl, 1);
	if (enable) {
		ctrl |= CTRL_DISP_BRIGHTNESS_CTRL_ON |
			CTRL_DISP_BACKLIGHT_ON;
	} else {
		ctrl &= ~(CTRL_DISP_BRIGHTNESS_CTRL_ON |
			  CTRL_DISP_BACKLIGHT_ON);
	}

	ctrl |= 1 << 8;
	acx565akm_write(ddata, MIPID_CMD_WRITE_CTRL_DISP, (u8 *)&ctrl, 2);
}

static void set_cabc_mode(struct panel_drv_data *ddata, unsigned mode)
{
	u16 cabc_ctrl;

	ddata->cabc_mode = mode;
	if (!ddata->enabled)
		return;
	cabc_ctrl = 0;
	acx565akm_read(ddata, MIPID_CMD_READ_CABC, (u8 *)&cabc_ctrl, 1);
	cabc_ctrl &= ~3;
	cabc_ctrl |= (1 << 8) | (mode & 3);
	acx565akm_write(ddata, MIPID_CMD_WRITE_CABC, (u8 *)&cabc_ctrl, 2);
}

static unsigned get_cabc_mode(struct panel_drv_data *ddata)
{
	return ddata->cabc_mode;
}

static unsigned get_hw_cabc_mode(struct panel_drv_data *ddata)
{
	u8 cabc_ctrl;

	acx565akm_read(ddata, MIPID_CMD_READ_CABC, &cabc_ctrl, 1);
	return cabc_ctrl & 3;
}

static void acx565akm_set_brightness(struct panel_drv_data *ddata, int level)
{
	int bv;

	bv = level | (1 << 8);
	acx565akm_write(ddata, MIPID_CMD_WRITE_DISP_BRIGHTNESS, (u8 *)&bv, 2);

	if (level)
		enable_backlight_ctrl(ddata, 1);
	else
		enable_backlight_ctrl(ddata, 0);
}

static int acx565akm_get_actual_brightness(struct panel_drv_data *ddata)
{
	u8 bv;

	acx565akm_read(ddata, MIPID_CMD_READ_DISP_BRIGHTNESS, &bv, 1);

	return bv;
}


static int acx565akm_bl_update_status(struct backlight_device *dev)
{
	struct panel_drv_data *ddata = dev_get_drvdata(&dev->dev);
	int level;

	dev_dbg(&ddata->spi->dev, "%s\n", __func__);

	if (dev->props.fb_blank == FB_BLANK_UNBLANK &&
			dev->props.power == FB_BLANK_UNBLANK)
		level = dev->props.brightness;
	else
		level = 0;

	if (ddata->has_bc)
		acx565akm_set_brightness(ddata, level);
	else
		return -ENODEV;

	return 0;
}

static int acx565akm_bl_get_intensity(struct backlight_device *dev)
{
	struct panel_drv_data *ddata = dev_get_drvdata(&dev->dev);

	dev_dbg(&dev->dev, "%s\n", __func__);

	if (!ddata->has_bc)
		return -ENODEV;

	if (dev->props.fb_blank == FB_BLANK_UNBLANK &&
			dev->props.power == FB_BLANK_UNBLANK) {
		if (ddata->has_bc)
			return acx565akm_get_actual_brightness(ddata);
		else
			return dev->props.brightness;
	}

	return 0;
}

static int acx565akm_bl_update_status_locked(struct backlight_device *dev)
{
	struct panel_drv_data *ddata = dev_get_drvdata(&dev->dev);
	int r;

	mutex_lock(&ddata->mutex);
	r = acx565akm_bl_update_status(dev);
	mutex_unlock(&ddata->mutex);

	return r;
}

static int acx565akm_bl_get_intensity_locked(struct backlight_device *dev)
{
	struct panel_drv_data *ddata = dev_get_drvdata(&dev->dev);
	int r;

	mutex_lock(&ddata->mutex);
	r = acx565akm_bl_get_intensity(dev);
	mutex_unlock(&ddata->mutex);

	return r;
}

static const struct backlight_ops acx565akm_bl_ops = {
	.get_brightness = acx565akm_bl_get_intensity_locked,
	.update_status  = acx565akm_bl_update_status_locked,
};

/*--------------------Auto Brightness control via Sysfs---------------------*/

static const char * const cabc_modes[] = {
	"off",		/* always used when CABC is not supported */
	"ui",
	"still-image",
	"moving-image",
};

static ssize_t show_cabc_mode(struct device *dev,
		struct device_attribute *attr,
		char *buf)
{
	struct panel_drv_data *ddata = dev_get_drvdata(dev);
	const char *mode_str;
	int mode;
	int len;

	if (!ddata->has_cabc)
		mode = 0;
	else
		mode = get_cabc_mode(ddata);
	mode_str = "unknown";
	if (mode >= 0 && mode < ARRAY_SIZE(cabc_modes))
		mode_str = cabc_modes[mode];
	len = snprintf(buf, PAGE_SIZE, "%s\n", mode_str);

	return len < PAGE_SIZE - 1 ? len : PAGE_SIZE - 1;
}

static ssize_t store_cabc_mode(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	struct panel_drv_data *ddata = dev_get_drvdata(dev);
	int i;

	for (i = 0; i < ARRAY_SIZE(cabc_modes); i++) {
		const char *mode_str = cabc_modes[i];
		int cmp_len = strlen(mode_str);

		if (count > 0 && buf[count - 1] == '\n')
			count--;
		if (count != cmp_len)
			continue;

		if (strncmp(buf, mode_str, cmp_len) == 0)
			break;
	}

	if (i == ARRAY_SIZE(cabc_modes))
		return -EINVAL;

	if (!ddata->has_cabc && i != 0)
		return -EINVAL;

	mutex_lock(&ddata->mutex);
	set_cabc_mode(ddata, i);
	mutex_unlock(&ddata->mutex);

	return count;
}

static ssize_t show_cabc_available_modes(struct device *dev,
		struct device_attribute *attr,
		char *buf)
{
	struct panel_drv_data *ddata = dev_get_drvdata(dev);
	int len;
	int i;

	if (!ddata->has_cabc)
		return snprintf(buf, PAGE_SIZE, "%s\n", cabc_modes[0]);

	for (i = 0, len = 0;
	     len < PAGE_SIZE && i < ARRAY_SIZE(cabc_modes); i++)
		len += snprintf(&buf[len], PAGE_SIZE - len, "%s%s%s",
			i ? " " : "", cabc_modes[i],
			i == ARRAY_SIZE(cabc_modes) - 1 ? "\n" : "");

	return len < PAGE_SIZE ? len : PAGE_SIZE - 1;
}

static DEVICE_ATTR(cabc_mode, S_IRUGO | S_IWUSR,
		show_cabc_mode, store_cabc_mode);
static DEVICE_ATTR(cabc_available_modes, S_IRUGO,
		show_cabc_available_modes, NULL);

static struct attribute *bldev_attrs[] = {
	&dev_attr_cabc_mode.attr,
	&dev_attr_cabc_available_modes.attr,
	NULL,
};

static const struct attribute_group bldev_attr_group = {
	.attrs = bldev_attrs,
};

static int acx565akm_connect(struct omap_dss_device *dssdev)
{
	struct panel_drv_data *ddata = to_panel_data(dssdev);
	struct omap_dss_device *in = ddata->in;

	if (omapdss_device_is_connected(dssdev))
		return 0;

	return in->ops.sdi->connect(in, dssdev);
}

static void acx565akm_disconnect(struct omap_dss_device *dssdev)
{
	struct panel_drv_data *ddata = to_panel_data(dssdev);
	struct omap_dss_device *in = ddata->in;

	if (!omapdss_device_is_connected(dssdev))
		return;

	in->ops.sdi->disconnect(in, dssdev);
}

static int acx565akm_panel_power_on(struct omap_dss_device *dssdev)
{
	struct panel_drv_data *ddata = to_panel_data(dssdev);
	struct omap_dss_device *in = ddata->in;
	int r;

	dev_dbg(&ddata->spi->dev, "%s\n", __func__);

	in->ops.sdi->set_timings(in, &ddata->videomode);

	if (ddata->datapairs > 0)
		in->ops.sdi->set_datapairs(in, ddata->datapairs);

	r = in->ops.sdi->enable(in);
	if (r) {
		pr_err("%s sdi enable failed\n", __func__);
		return r;
	}

	/*FIXME tweak me */
	msleep(50);

	if (gpio_is_valid(ddata->reset_gpio))
		gpio_set_value(ddata->reset_gpio, 1);

	if (ddata->enabled) {
		dev_dbg(&ddata->spi->dev, "panel already enabled\n");
		return 0;
	}

	/*
	 * We have to meet all the following delay requirements:
	 * 1. tRW: reset pulse width 10usec (7.12.1)
	 * 2. tRT: reset cancel time 5msec (7.12.1)
	 * 3. Providing PCLK,HS,VS signals for 2 frames = ~50msec worst
	 *    case (7.6.2)
	 * 4. 120msec before the sleep out command (7.12.1)
	 */
	msleep(120);

	set_sleep_mode(ddata, 0);
	ddata->enabled = 1;

	/* 5msec between sleep out and the next command. (8.2.16) */
	usleep_range(5000, 10000);
	set_display_state(ddata, 1);
	set_cabc_mode(ddata, ddata->cabc_mode);

	return acx565akm_bl_update_status(ddata->bl_dev);
}

static void acx565akm_panel_power_off(struct omap_dss_device *dssdev)
{
	struct panel_drv_data *ddata = to_panel_data(dssdev);
	struct omap_dss_device *in = ddata->in;

	dev_dbg(dssdev->dev, "%s\n", __func__);

	if (!ddata->enabled)
		return;

	set_display_state(ddata, 0);
	set_sleep_mode(ddata, 1);
	ddata->enabled = 0;
	/*
	 * We have to provide PCLK,HS,VS signals for 2 frames (worst case
	 * ~50msec) after sending the sleep in command and asserting the
	 * reset signal. We probably could assert the reset w/o the delay
	 * but we still delay to avoid possible artifacts. (7.6.1)
	 */
	msleep(50);

	if (gpio_is_valid(ddata->reset_gpio))
		gpio_set_value(ddata->reset_gpio, 0);

	/* FIXME need to tweak this delay */
	msleep(100);

	in->ops.sdi->disable(in);
}

static int acx565akm_enable(struct omap_dss_device *dssdev)
{
	struct panel_drv_data *ddata = to_panel_data(dssdev);
	int r;

	dev_dbg(dssdev->dev, "%s\n", __func__);

	if (!omapdss_device_is_connected(dssdev))
		return -ENODEV;

	if (omapdss_device_is_enabled(dssdev))
		return 0;

	mutex_lock(&ddata->mutex);
	r = acx565akm_panel_power_on(dssdev);
	mutex_unlock(&ddata->mutex);
	if (r)
		return r;

	dssdev->state = OMAP_DSS_DISPLAY_ACTIVE;

	return 0;
}

static void acx565akm_disable(struct omap_dss_device *dssdev)
{
	struct panel_drv_data *ddata = to_panel_data(dssdev);

	dev_dbg(dssdev->dev, "%s\n", __func__);

	if (!omapdss_device_is_enabled(dssdev))
		return;

	mutex_lock(&ddata->mutex);
	acx565akm_panel_power_off(dssdev);
	mutex_unlock(&ddata->mutex);

	dssdev->state = OMAP_DSS_DISPLAY_DISABLED;
}

static void acx565akm_set_timings(struct omap_dss_device *dssdev,
		struct omap_video_timings *timings)
{
	struct panel_drv_data *ddata = to_panel_data(dssdev);
	struct omap_dss_device *in = ddata->in;

	ddata->videomode = *timings;
	dssdev->panel.timings = *timings;

	in->ops.sdi->set_timings(in, timings);
}

static void acx565akm_get_timings(struct omap_dss_device *dssdev,
		struct omap_video_timings *timings)
{
	struct panel_drv_data *ddata = to_panel_data(dssdev);

	*timings = ddata->videomode;
}

static int acx565akm_check_timings(struct omap_dss_device *dssdev,
		struct omap_video_timings *timings)
{
	struct panel_drv_data *ddata = to_panel_data(dssdev);
	struct omap_dss_device *in = ddata->in;

	return in->ops.sdi->check_timings(in, timings);
}

static struct omap_dss_driver acx565akm_ops = {
	.connect	= acx565akm_connect,
	.disconnect	= acx565akm_disconnect,

	.enable		= acx565akm_enable,
	.disable	= acx565akm_disable,

	.set_timings	= acx565akm_set_timings,
	.get_timings	= acx565akm_get_timings,
	.check_timings	= acx565akm_check_timings,

	.get_resolution	= omapdss_default_get_resolution,
};

static int acx565akm_probe_pdata(struct spi_device *spi)
{
	const struct panel_acx565akm_platform_data *pdata;
	struct panel_drv_data *ddata = dev_get_drvdata(&spi->dev);
	struct omap_dss_device *dssdev, *in;

	pdata = dev_get_platdata(&spi->dev);

	ddata->reset_gpio = pdata->reset_gpio;

	in = omap_dss_find_output(pdata->source);
	if (in == NULL) {
		dev_err(&spi->dev, "failed to find video source '%s'\n",
				pdata->source);
		return -EPROBE_DEFER;
	}
	ddata->in = in;

	ddata->datapairs = pdata->datapairs;

	dssdev = &ddata->dssdev;
	dssdev->name = pdata->name;

	return 0;
}

static int acx565akm_probe_of(struct spi_device *spi)
{
	struct panel_drv_data *ddata = dev_get_drvdata(&spi->dev);
	struct device_node *np = spi->dev.of_node;

	ddata->reset_gpio = of_get_named_gpio(np, "reset-gpios", 0);

	ddata->in = omapdss_of_find_source_for_first_ep(np);
	if (IS_ERR(ddata->in)) {
		dev_err(&spi->dev, "failed to find video source\n");
		return PTR_ERR(ddata->in);
	}

	return 0;
}

static int acx565akm_probe(struct spi_device *spi)
{
	struct panel_drv_data *ddata;
	struct omap_dss_device *dssdev;
	struct backlight_device *bldev;
	int max_brightness, brightness;
	struct backlight_properties props;
	int r;

	dev_dbg(&spi->dev, "%s\n", __func__);

	spi->mode = SPI_MODE_3;

	ddata = devm_kzalloc(&spi->dev, sizeof(*ddata), GFP_KERNEL);
	if (ddata == NULL)
		return -ENOMEM;

	dev_set_drvdata(&spi->dev, ddata);

	ddata->spi = spi;

	mutex_init(&ddata->mutex);

	if (dev_get_platdata(&spi->dev)) {
		r = acx565akm_probe_pdata(spi);
		if (r)
			return r;
	} else if (spi->dev.of_node) {
		r = acx565akm_probe_of(spi);
		if (r)
			return r;
	} else {
		dev_err(&spi->dev, "platform data missing!\n");
		return -ENODEV;
	}

	if (gpio_is_valid(ddata->reset_gpio)) {
		r = devm_gpio_request_one(&spi->dev, ddata->reset_gpio,
				GPIOF_OUT_INIT_LOW, "lcd reset");
		if (r)
			goto err_gpio;
	}

	if (gpio_is_valid(ddata->reset_gpio))
		gpio_set_value(ddata->reset_gpio, 1);

	/*
	 * After reset we have to wait 5 msec before the first
	 * command can be sent.
	 */
	usleep_range(5000, 10000);

	ddata->enabled = panel_enabled(ddata);

	r = panel_detect(ddata);

	if (!ddata->enabled && gpio_is_valid(ddata->reset_gpio))
		gpio_set_value(ddata->reset_gpio, 0);

	if (r) {
		dev_err(&spi->dev, "%s panel detect error\n", __func__);
		goto err_detect;
	}

	memset(&props, 0, sizeof(props));
	props.fb_blank = FB_BLANK_UNBLANK;
	props.power = FB_BLANK_UNBLANK;
	props.type = BACKLIGHT_RAW;

	bldev = backlight_device_register("acx565akm", &ddata->spi->dev,
			ddata, &acx565akm_bl_ops, &props);
	if (IS_ERR(bldev)) {
		r = PTR_ERR(bldev);
		goto err_reg_bl;
	}
	ddata->bl_dev = bldev;
	if (ddata->has_cabc) {
		r = sysfs_create_group(&bldev->dev.kobj, &bldev_attr_group);
		if (r) {
			dev_err(&bldev->dev,
				"%s failed to create sysfs files\n", __func__);
			goto err_sysfs;
		}
		ddata->cabc_mode = get_hw_cabc_mode(ddata);
	}

	max_brightness = 255;

	if (ddata->has_bc)
		brightness = acx565akm_get_actual_brightness(ddata);
	else
		brightness = 0;

	bldev->props.max_brightness = max_brightness;
	bldev->props.brightness = brightness;

	acx565akm_bl_update_status(bldev);


	ddata->videomode = acx565akm_panel_timings;

	dssdev = &ddata->dssdev;
	dssdev->dev = &spi->dev;
	dssdev->driver = &acx565akm_ops;
	dssdev->type = OMAP_DISPLAY_TYPE_SDI;
	dssdev->owner = THIS_MODULE;
	dssdev->panel.timings = ddata->videomode;

	r = omapdss_register_display(dssdev);
	if (r) {
		dev_err(&spi->dev, "Failed to register panel\n");
		goto err_reg;
	}

	return 0;

err_reg:
	sysfs_remove_group(&bldev->dev.kobj, &bldev_attr_group);
err_sysfs:
	backlight_device_unregister(bldev);
err_reg_bl:
err_detect:
err_gpio:
	omap_dss_put_device(ddata->in);
	return r;
}

static int acx565akm_remove(struct spi_device *spi)
{
	struct panel_drv_data *ddata = dev_get_drvdata(&spi->dev);
	struct omap_dss_device *dssdev = &ddata->dssdev;
	struct omap_dss_device *in = ddata->in;

	dev_dbg(&ddata->spi->dev, "%s\n", __func__);

	sysfs_remove_group(&ddata->bl_dev->dev.kobj, &bldev_attr_group);
	backlight_device_unregister(ddata->bl_dev);

	omapdss_unregister_display(dssdev);

	acx565akm_disable(dssdev);
	acx565akm_disconnect(dssdev);

	omap_dss_put_device(in);

	return 0;
}

static const struct of_device_id acx565akm_of_match[] = {
	{ .compatible = "omapdss,sony,acx565akm", },
	{},
};
MODULE_DEVICE_TABLE(of, acx565akm_of_match);

static struct spi_driver acx565akm_driver = {
	.driver = {
		.name	= "acx565akm",
		.of_match_table = acx565akm_of_match,
		.suppress_bind_attrs = true,
	},
	.probe	= acx565akm_probe,
	.remove	= acx565akm_remove,
};

module_spi_driver(acx565akm_driver);

MODULE_AUTHOR("Nokia Corporation");
MODULE_DESCRIPTION("acx565akm LCD Driver");
MODULE_LICENSE("GPL");
