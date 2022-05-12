// SPDX-License-Identifier: GPL-2.0-only
/*
 * drivers/media/i2c/adp1653.c
 *
 * Copyright (C) 2008--2011 Nokia Corporation
 *
 * Contact: Sakari Ailus <sakari.ailus@iki.fi>
 *
 * Contributors:
 *	Sakari Ailus <sakari.ailus@iki.fi>
 *	Tuukka Toivonen <tuukkat76@gmail.com>
 *	Pavel Machek <pavel@ucw.cz>
 *
 * TODO:
 * - fault interrupt handling
 * - hardware strobe
 * - power doesn't need to be ON if all lights are off
 */

#include <linux/delay.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/gpio/consumer.h>
#include <media/i2c/adp1653.h>
#include <media/v4l2-device.h>

#define TIMEOUT_MAX		820000
#define TIMEOUT_STEP		54600
#define TIMEOUT_MIN		(TIMEOUT_MAX - ADP1653_REG_CONFIG_TMR_SET_MAX \
				 * TIMEOUT_STEP)
#define TIMEOUT_US_TO_CODE(t)	((TIMEOUT_MAX + (TIMEOUT_STEP / 2) - (t)) \
				 / TIMEOUT_STEP)
#define TIMEOUT_CODE_TO_US(c)	(TIMEOUT_MAX - (c) * TIMEOUT_STEP)

/* Write values into ADP1653 registers. */
static int adp1653_update_hw(struct adp1653_flash *flash)
{
	struct i2c_client *client = v4l2_get_subdevdata(&flash->subdev);
	u8 out_sel;
	u8 config = 0;
	int rval;

	out_sel = ADP1653_INDICATOR_INTENSITY_uA_TO_REG(
		flash->indicator_intensity->val)
		<< ADP1653_REG_OUT_SEL_ILED_SHIFT;

	switch (flash->led_mode->val) {
	case V4L2_FLASH_LED_MODE_NONE:
		break;
	case V4L2_FLASH_LED_MODE_FLASH:
		/* Flash mode, light on with strobe, duration from timer */
		config = ADP1653_REG_CONFIG_TMR_CFG;
		config |= TIMEOUT_US_TO_CODE(flash->flash_timeout->val)
			  << ADP1653_REG_CONFIG_TMR_SET_SHIFT;
		break;
	case V4L2_FLASH_LED_MODE_TORCH:
		/* Torch mode, light immediately on, duration indefinite */
		out_sel |= ADP1653_FLASH_INTENSITY_mA_TO_REG(
			flash->torch_intensity->val)
			<< ADP1653_REG_OUT_SEL_HPLED_SHIFT;
		break;
	}

	rval = i2c_smbus_write_byte_data(client, ADP1653_REG_OUT_SEL, out_sel);
	if (rval < 0)
		return rval;

	rval = i2c_smbus_write_byte_data(client, ADP1653_REG_CONFIG, config);
	if (rval < 0)
		return rval;

	return 0;
}

static int adp1653_get_fault(struct adp1653_flash *flash)
{
	struct i2c_client *client = v4l2_get_subdevdata(&flash->subdev);
	int fault;
	int rval;

	fault = i2c_smbus_read_byte_data(client, ADP1653_REG_FAULT);
	if (fault < 0)
		return fault;

	flash->fault |= fault;

	if (!flash->fault)
		return 0;

	/* Clear faults. */
	rval = i2c_smbus_write_byte_data(client, ADP1653_REG_OUT_SEL, 0);
	if (rval < 0)
		return rval;

	flash->led_mode->val = V4L2_FLASH_LED_MODE_NONE;

	rval = adp1653_update_hw(flash);
	if (rval)
		return rval;

	return flash->fault;
}

static int adp1653_strobe(struct adp1653_flash *flash, int enable)
{
	struct i2c_client *client = v4l2_get_subdevdata(&flash->subdev);
	u8 out_sel = ADP1653_INDICATOR_INTENSITY_uA_TO_REG(
		flash->indicator_intensity->val)
		<< ADP1653_REG_OUT_SEL_ILED_SHIFT;
	int rval;

	if (flash->led_mode->val != V4L2_FLASH_LED_MODE_FLASH)
		return -EBUSY;

	if (!enable)
		return i2c_smbus_write_byte_data(client, ADP1653_REG_OUT_SEL,
						 out_sel);

	out_sel |= ADP1653_FLASH_INTENSITY_mA_TO_REG(
		flash->flash_intensity->val)
		<< ADP1653_REG_OUT_SEL_HPLED_SHIFT;
	rval = i2c_smbus_write_byte_data(client, ADP1653_REG_OUT_SEL, out_sel);
	if (rval)
		return rval;

	/* Software strobe using i2c */
	rval = i2c_smbus_write_byte_data(client, ADP1653_REG_SW_STROBE,
		ADP1653_REG_SW_STROBE_SW_STROBE);
	if (rval)
		return rval;
	return i2c_smbus_write_byte_data(client, ADP1653_REG_SW_STROBE, 0);
}

/* --------------------------------------------------------------------------
 * V4L2 controls
 */

static int adp1653_get_ctrl(struct v4l2_ctrl *ctrl)
{
	struct adp1653_flash *flash =
		container_of(ctrl->handler, struct adp1653_flash, ctrls);
	int rval;

	rval = adp1653_get_fault(flash);
	if (rval)
		return rval;

	ctrl->cur.val = 0;

	if (flash->fault & ADP1653_REG_FAULT_FLT_SCP)
		ctrl->cur.val |= V4L2_FLASH_FAULT_SHORT_CIRCUIT;
	if (flash->fault & ADP1653_REG_FAULT_FLT_OT)
		ctrl->cur.val |= V4L2_FLASH_FAULT_OVER_TEMPERATURE;
	if (flash->fault & ADP1653_REG_FAULT_FLT_TMR)
		ctrl->cur.val |= V4L2_FLASH_FAULT_TIMEOUT;
	if (flash->fault & ADP1653_REG_FAULT_FLT_OV)
		ctrl->cur.val |= V4L2_FLASH_FAULT_OVER_VOLTAGE;

	flash->fault = 0;

	return 0;
}

static int adp1653_set_ctrl(struct v4l2_ctrl *ctrl)
{
	struct adp1653_flash *flash =
		container_of(ctrl->handler, struct adp1653_flash, ctrls);
	int rval;

	rval = adp1653_get_fault(flash);
	if (rval)
		return rval;
	if ((rval & (ADP1653_REG_FAULT_FLT_SCP |
		     ADP1653_REG_FAULT_FLT_OT |
		     ADP1653_REG_FAULT_FLT_OV)) &&
	    (ctrl->id == V4L2_CID_FLASH_STROBE ||
	     ctrl->id == V4L2_CID_FLASH_TORCH_INTENSITY ||
	     ctrl->id == V4L2_CID_FLASH_LED_MODE))
		return -EBUSY;

	switch (ctrl->id) {
	case V4L2_CID_FLASH_STROBE:
		return adp1653_strobe(flash, 1);
	case V4L2_CID_FLASH_STROBE_STOP:
		return adp1653_strobe(flash, 0);
	}

	return adp1653_update_hw(flash);
}

static const struct v4l2_ctrl_ops adp1653_ctrl_ops = {
	.g_volatile_ctrl = adp1653_get_ctrl,
	.s_ctrl = adp1653_set_ctrl,
};

static int adp1653_init_controls(struct adp1653_flash *flash)
{
	struct v4l2_ctrl *fault;

	v4l2_ctrl_handler_init(&flash->ctrls, 9);

	flash->led_mode =
		v4l2_ctrl_new_std_menu(&flash->ctrls, &adp1653_ctrl_ops,
				       V4L2_CID_FLASH_LED_MODE,
				       V4L2_FLASH_LED_MODE_TORCH, ~0x7, 0);
	v4l2_ctrl_new_std_menu(&flash->ctrls, &adp1653_ctrl_ops,
			       V4L2_CID_FLASH_STROBE_SOURCE,
			       V4L2_FLASH_STROBE_SOURCE_SOFTWARE, ~0x1, 0);
	v4l2_ctrl_new_std(&flash->ctrls, &adp1653_ctrl_ops,
			  V4L2_CID_FLASH_STROBE, 0, 0, 0, 0);
	v4l2_ctrl_new_std(&flash->ctrls, &adp1653_ctrl_ops,
			  V4L2_CID_FLASH_STROBE_STOP, 0, 0, 0, 0);
	flash->flash_timeout =
		v4l2_ctrl_new_std(&flash->ctrls, &adp1653_ctrl_ops,
				  V4L2_CID_FLASH_TIMEOUT, TIMEOUT_MIN,
				  flash->platform_data->max_flash_timeout,
				  TIMEOUT_STEP,
				  flash->platform_data->max_flash_timeout);
	flash->flash_intensity =
		v4l2_ctrl_new_std(&flash->ctrls, &adp1653_ctrl_ops,
				  V4L2_CID_FLASH_INTENSITY,
				  ADP1653_FLASH_INTENSITY_MIN,
				  flash->platform_data->max_flash_intensity,
				  1, flash->platform_data->max_flash_intensity);
	flash->torch_intensity =
		v4l2_ctrl_new_std(&flash->ctrls, &adp1653_ctrl_ops,
				  V4L2_CID_FLASH_TORCH_INTENSITY,
				  ADP1653_TORCH_INTENSITY_MIN,
				  flash->platform_data->max_torch_intensity,
				  ADP1653_FLASH_INTENSITY_STEP,
				  flash->platform_data->max_torch_intensity);
	flash->indicator_intensity =
		v4l2_ctrl_new_std(&flash->ctrls, &adp1653_ctrl_ops,
				  V4L2_CID_FLASH_INDICATOR_INTENSITY,
				  ADP1653_INDICATOR_INTENSITY_MIN,
				  flash->platform_data->max_indicator_intensity,
				  ADP1653_INDICATOR_INTENSITY_STEP,
				  ADP1653_INDICATOR_INTENSITY_MIN);
	fault = v4l2_ctrl_new_std(&flash->ctrls, &adp1653_ctrl_ops,
				  V4L2_CID_FLASH_FAULT, 0,
				  V4L2_FLASH_FAULT_OVER_VOLTAGE
				  | V4L2_FLASH_FAULT_OVER_TEMPERATURE
				  | V4L2_FLASH_FAULT_SHORT_CIRCUIT, 0, 0);

	if (flash->ctrls.error)
		return flash->ctrls.error;

	fault->flags |= V4L2_CTRL_FLAG_VOLATILE;

	flash->subdev.ctrl_handler = &flash->ctrls;
	return 0;
}

/* --------------------------------------------------------------------------
 * V4L2 subdev operations
 */

static int
adp1653_init_device(struct adp1653_flash *flash)
{
	struct i2c_client *client = v4l2_get_subdevdata(&flash->subdev);
	int rval;

	/* Clear FAULT register by writing zero to OUT_SEL */
	rval = i2c_smbus_write_byte_data(client, ADP1653_REG_OUT_SEL, 0);
	if (rval < 0) {
		dev_err(&client->dev, "failed writing fault register\n");
		return -EIO;
	}

	mutex_lock(flash->ctrls.lock);
	/* Reset faults before reading new ones. */
	flash->fault = 0;
	rval = adp1653_get_fault(flash);
	mutex_unlock(flash->ctrls.lock);
	if (rval > 0) {
		dev_err(&client->dev, "faults detected: 0x%1.1x\n", rval);
		return -EIO;
	}

	mutex_lock(flash->ctrls.lock);
	rval = adp1653_update_hw(flash);
	mutex_unlock(flash->ctrls.lock);
	if (rval) {
		dev_err(&client->dev,
			"adp1653_update_hw failed at %s\n", __func__);
		return -EIO;
	}

	return 0;
}

static int
__adp1653_set_power(struct adp1653_flash *flash, int on)
{
	int ret;

	if (flash->platform_data->power) {
		ret = flash->platform_data->power(&flash->subdev, on);
		if (ret < 0)
			return ret;
	} else {
		gpiod_set_value(flash->platform_data->enable_gpio, on);
		if (on)
			/* Some delay is apparently required. */
			udelay(20);
	}

	if (!on)
		return 0;

	ret = adp1653_init_device(flash);
	if (ret >= 0)
		return ret;

	if (flash->platform_data->power)
		flash->platform_data->power(&flash->subdev, 0);
	else
		gpiod_set_value(flash->platform_data->enable_gpio, 0);

	return ret;
}

static int
adp1653_set_power(struct v4l2_subdev *subdev, int on)
{
	struct adp1653_flash *flash = to_adp1653_flash(subdev);
	int ret = 0;

	mutex_lock(&flash->power_lock);

	/* If the power count is modified from 0 to != 0 or from != 0 to 0,
	 * update the power state.
	 */
	if (flash->power_count == !on) {
		ret = __adp1653_set_power(flash, !!on);
		if (ret < 0)
			goto done;
	}

	/* Update the power count. */
	flash->power_count += on ? 1 : -1;
	WARN_ON(flash->power_count < 0);

done:
	mutex_unlock(&flash->power_lock);
	return ret;
}

static int adp1653_open(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh)
{
	return adp1653_set_power(sd, 1);
}

static int adp1653_close(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh)
{
	return adp1653_set_power(sd, 0);
}

static const struct v4l2_subdev_core_ops adp1653_core_ops = {
	.s_power = adp1653_set_power,
};

static const struct v4l2_subdev_ops adp1653_ops = {
	.core = &adp1653_core_ops,
};

static const struct v4l2_subdev_internal_ops adp1653_internal_ops = {
	.open = adp1653_open,
	.close = adp1653_close,
};

/* --------------------------------------------------------------------------
 * I2C driver
 */
#ifdef CONFIG_PM

static int adp1653_suspend(struct device *dev)
{
	struct v4l2_subdev *subdev = dev_get_drvdata(dev);
	struct adp1653_flash *flash = to_adp1653_flash(subdev);

	if (!flash->power_count)
		return 0;

	return __adp1653_set_power(flash, 0);
}

static int adp1653_resume(struct device *dev)
{
	struct v4l2_subdev *subdev = dev_get_drvdata(dev);
	struct adp1653_flash *flash = to_adp1653_flash(subdev);

	if (!flash->power_count)
		return 0;

	return __adp1653_set_power(flash, 1);
}

#else

#define adp1653_suspend	NULL
#define adp1653_resume	NULL

#endif /* CONFIG_PM */

static int adp1653_of_init(struct i2c_client *client,
			   struct adp1653_flash *flash,
			   struct device_node *node)
{
	struct adp1653_platform_data *pd;
	struct device_node *child;

	pd = devm_kzalloc(&client->dev, sizeof(*pd), GFP_KERNEL);
	if (!pd)
		return -ENOMEM;
	flash->platform_data = pd;

	child = of_get_child_by_name(node, "flash");
	if (!child)
		return -EINVAL;

	if (of_property_read_u32(child, "flash-timeout-us",
				 &pd->max_flash_timeout))
		goto err;

	if (of_property_read_u32(child, "flash-max-microamp",
				 &pd->max_flash_intensity))
		goto err;

	pd->max_flash_intensity /= 1000;

	if (of_property_read_u32(child, "led-max-microamp",
				 &pd->max_torch_intensity))
		goto err;

	pd->max_torch_intensity /= 1000;
	of_node_put(child);

	child = of_get_child_by_name(node, "indicator");
	if (!child)
		return -EINVAL;

	if (of_property_read_u32(child, "led-max-microamp",
				 &pd->max_indicator_intensity))
		goto err;

	of_node_put(child);

	pd->enable_gpio = devm_gpiod_get(&client->dev, "enable", GPIOD_OUT_LOW);
	if (IS_ERR(pd->enable_gpio)) {
		dev_err(&client->dev, "Error getting GPIO\n");
		return PTR_ERR(pd->enable_gpio);
	}

	return 0;
err:
	dev_err(&client->dev, "Required property not found\n");
	of_node_put(child);
	return -EINVAL;
}


static int adp1653_probe(struct i2c_client *client,
			 const struct i2c_device_id *devid)
{
	struct adp1653_flash *flash;
	int ret;

	flash = devm_kzalloc(&client->dev, sizeof(*flash), GFP_KERNEL);
	if (flash == NULL)
		return -ENOMEM;

	if (client->dev.of_node) {
		ret = adp1653_of_init(client, flash, client->dev.of_node);
		if (ret)
			return ret;
	} else {
		if (!client->dev.platform_data) {
			dev_err(&client->dev,
				"Neither DT not platform data provided\n");
			return -EINVAL;
		}
		flash->platform_data = client->dev.platform_data;
	}

	mutex_init(&flash->power_lock);

	v4l2_i2c_subdev_init(&flash->subdev, client, &adp1653_ops);
	flash->subdev.internal_ops = &adp1653_internal_ops;
	flash->subdev.flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;

	ret = adp1653_init_controls(flash);
	if (ret)
		goto free_and_quit;

	ret = media_entity_pads_init(&flash->subdev.entity, 0, NULL);
	if (ret < 0)
		goto free_and_quit;

	flash->subdev.entity.function = MEDIA_ENT_F_FLASH;

	return 0;

free_and_quit:
	dev_err(&client->dev, "adp1653: failed to register device\n");
	v4l2_ctrl_handler_free(&flash->ctrls);
	return ret;
}

static int adp1653_remove(struct i2c_client *client)
{
	struct v4l2_subdev *subdev = i2c_get_clientdata(client);
	struct adp1653_flash *flash = to_adp1653_flash(subdev);

	v4l2_device_unregister_subdev(&flash->subdev);
	v4l2_ctrl_handler_free(&flash->ctrls);
	media_entity_cleanup(&flash->subdev.entity);

	return 0;
}

static const struct i2c_device_id adp1653_id_table[] = {
	{ ADP1653_NAME, 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, adp1653_id_table);

static const struct dev_pm_ops adp1653_pm_ops = {
	.suspend	= adp1653_suspend,
	.resume		= adp1653_resume,
};

static struct i2c_driver adp1653_i2c_driver = {
	.driver		= {
		.name	= ADP1653_NAME,
		.pm	= &adp1653_pm_ops,
	},
	.probe		= adp1653_probe,
	.remove		= adp1653_remove,
	.id_table	= adp1653_id_table,
};

module_i2c_driver(adp1653_i2c_driver);

MODULE_AUTHOR("Sakari Ailus <sakari.ailus@nokia.com>");
MODULE_DESCRIPTION("Analog Devices ADP1653 LED flash driver");
MODULE_LICENSE("GPL");
