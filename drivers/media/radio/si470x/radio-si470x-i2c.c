// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * drivers/media/radio/si470x/radio-si470x-i2c.c
 *
 * I2C driver for radios with Silicon Labs Si470x FM Radio Receivers
 *
 * Copyright (c) 2009 Samsung Electronics Co.Ltd
 * Author: Joonyoung Shim <jy0922.shim@samsung.com>
 */


/* driver definitions */
#define DRIVER_AUTHOR "Joonyoung Shim <jy0922.shim@samsung.com>";
#define DRIVER_CARD "Silicon Labs Si470x FM Radio Receiver"
#define DRIVER_DESC "I2C radio driver for Si470x FM Radio Receivers"
#define DRIVER_VERSION "1.0.2"

/* kernel includes */
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/interrupt.h>

#include "radio-si470x.h"


/* I2C Device ID List */
static const struct i2c_device_id si470x_i2c_id[] = {
	/* Generic Entry */
	{ "si470x", 0 },
	/* Terminating entry */
	{ }
};
MODULE_DEVICE_TABLE(i2c, si470x_i2c_id);


/**************************************************************************
 * Module Parameters
 **************************************************************************/

/* Radio Nr */
static int radio_nr = -1;
module_param(radio_nr, int, 0444);
MODULE_PARM_DESC(radio_nr, "Radio Nr");

/* RDS buffer blocks */
static unsigned int rds_buf = 100;
module_param(rds_buf, uint, 0444);
MODULE_PARM_DESC(rds_buf, "RDS buffer entries: *100*");

/* RDS maximum block errors */
static unsigned short max_rds_errors = 1;
/* 0 means   0  errors requiring correction */
/* 1 means 1-2  errors requiring correction (used by original USBRadio.exe) */
/* 2 means 3-5  errors requiring correction */
/* 3 means   6+ errors or errors in checkword, correction not possible */
module_param(max_rds_errors, ushort, 0644);
MODULE_PARM_DESC(max_rds_errors, "RDS maximum block errors: *1*");



/**************************************************************************
 * I2C Definitions
 **************************************************************************/

/* Write starts with the upper byte of register 0x02 */
#define WRITE_REG_NUM		8
#define WRITE_INDEX(i)		(i + 0x02)

/* Read starts with the upper byte of register 0x0a */
#define READ_REG_NUM		RADIO_REGISTER_NUM
#define READ_INDEX(i)		((i + RADIO_REGISTER_NUM - 0x0a) % READ_REG_NUM)



/**************************************************************************
 * General Driver Functions - REGISTERs
 **************************************************************************/

/*
 * si470x_get_register - read register
 */
static int si470x_get_register(struct si470x_device *radio, int regnr)
{
	__be16 buf[READ_REG_NUM];
	struct i2c_msg msgs[1] = {
		{
			.addr = radio->client->addr,
			.flags = I2C_M_RD,
			.len = sizeof(u16) * READ_REG_NUM,
			.buf = (void *)buf
		},
	};

	if (i2c_transfer(radio->client->adapter, msgs, 1) != 1)
		return -EIO;

	radio->registers[regnr] = __be16_to_cpu(buf[READ_INDEX(regnr)]);

	return 0;
}


/*
 * si470x_set_register - write register
 */
static int si470x_set_register(struct si470x_device *radio, int regnr)
{
	int i;
	__be16 buf[WRITE_REG_NUM];
	struct i2c_msg msgs[1] = {
		{
			.addr = radio->client->addr,
			.len = sizeof(u16) * WRITE_REG_NUM,
			.buf = (void *)buf
		},
	};

	for (i = 0; i < WRITE_REG_NUM; i++)
		buf[i] = __cpu_to_be16(radio->registers[WRITE_INDEX(i)]);

	if (i2c_transfer(radio->client->adapter, msgs, 1) != 1)
		return -EIO;

	return 0;
}



/**************************************************************************
 * General Driver Functions - ENTIRE REGISTERS
 **************************************************************************/

/*
 * si470x_get_all_registers - read entire registers
 */
static int si470x_get_all_registers(struct si470x_device *radio)
{
	int i;
	__be16 buf[READ_REG_NUM];
	struct i2c_msg msgs[1] = {
		{
			.addr = radio->client->addr,
			.flags = I2C_M_RD,
			.len = sizeof(u16) * READ_REG_NUM,
			.buf = (void *)buf
		},
	};

	if (i2c_transfer(radio->client->adapter, msgs, 1) != 1)
		return -EIO;

	for (i = 0; i < READ_REG_NUM; i++)
		radio->registers[i] = __be16_to_cpu(buf[READ_INDEX(i)]);

	return 0;
}



/**************************************************************************
 * File Operations Interface
 **************************************************************************/

/*
 * si470x_fops_open - file open
 */
static int si470x_fops_open(struct file *file)
{
	struct si470x_device *radio = video_drvdata(file);
	int retval = v4l2_fh_open(file);

	if (retval)
		return retval;

	if (v4l2_fh_is_singular_file(file)) {
		/* start radio */
		retval = si470x_start(radio);
		if (retval < 0)
			goto done;

		/* enable RDS / STC interrupt */
		radio->registers[SYSCONFIG1] |= SYSCONFIG1_RDSIEN;
		radio->registers[SYSCONFIG1] |= SYSCONFIG1_STCIEN;
		radio->registers[SYSCONFIG1] &= ~SYSCONFIG1_GPIO2;
		radio->registers[SYSCONFIG1] |= 0x1 << 2;
		retval = si470x_set_register(radio, SYSCONFIG1);
	}

done:
	if (retval)
		v4l2_fh_release(file);
	return retval;
}


/*
 * si470x_fops_release - file release
 */
static int si470x_fops_release(struct file *file)
{
	struct si470x_device *radio = video_drvdata(file);

	if (v4l2_fh_is_singular_file(file))
		/* stop radio */
		si470x_stop(radio);

	return v4l2_fh_release(file);
}



/**************************************************************************
 * Video4Linux Interface
 **************************************************************************/

/*
 * si470x_vidioc_querycap - query device capabilities
 */
static int si470x_vidioc_querycap(struct file *file, void *priv,
				  struct v4l2_capability *capability)
{
	strscpy(capability->driver, DRIVER_NAME, sizeof(capability->driver));
	strscpy(capability->card, DRIVER_CARD, sizeof(capability->card));
	return 0;
}



/**************************************************************************
 * I2C Interface
 **************************************************************************/

/*
 * si470x_i2c_interrupt - interrupt handler
 */
static irqreturn_t si470x_i2c_interrupt(int irq, void *dev_id)
{
	struct si470x_device *radio = dev_id;
	unsigned char regnr;
	unsigned char blocknum;
	unsigned short bler; /* rds block errors */
	unsigned short rds;
	unsigned char tmpbuf[3];
	int retval = 0;

	/* check Seek/Tune Complete */
	retval = si470x_get_register(radio, STATUSRSSI);
	if (retval < 0)
		goto end;

	if (radio->registers[STATUSRSSI] & STATUSRSSI_STC)
		complete(&radio->completion);

	/* safety checks */
	if ((radio->registers[SYSCONFIG1] & SYSCONFIG1_RDS) == 0)
		goto end;

	/* Update RDS registers */
	for (regnr = 1; regnr < RDS_REGISTER_NUM; regnr++) {
		retval = si470x_get_register(radio, STATUSRSSI + regnr);
		if (retval < 0)
			goto end;
	}

	/* get rds blocks */
	if ((radio->registers[STATUSRSSI] & STATUSRSSI_RDSR) == 0)
		/* No RDS group ready, better luck next time */
		goto end;

	for (blocknum = 0; blocknum < 4; blocknum++) {
		switch (blocknum) {
		default:
			bler = (radio->registers[STATUSRSSI] &
					STATUSRSSI_BLERA) >> 9;
			rds = radio->registers[RDSA];
			break;
		case 1:
			bler = (radio->registers[READCHAN] &
					READCHAN_BLERB) >> 14;
			rds = radio->registers[RDSB];
			break;
		case 2:
			bler = (radio->registers[READCHAN] &
					READCHAN_BLERC) >> 12;
			rds = radio->registers[RDSC];
			break;
		case 3:
			bler = (radio->registers[READCHAN] &
					READCHAN_BLERD) >> 10;
			rds = radio->registers[RDSD];
			break;
		}

		/* Fill the V4L2 RDS buffer */
		put_unaligned_le16(rds, &tmpbuf);
		tmpbuf[2] = blocknum;		/* offset name */
		tmpbuf[2] |= blocknum << 3;	/* received offset */
		if (bler > max_rds_errors)
			tmpbuf[2] |= 0x80;	/* uncorrectable errors */
		else if (bler > 0)
			tmpbuf[2] |= 0x40;	/* corrected error(s) */

		/* copy RDS block to internal buffer */
		memcpy(&radio->buffer[radio->wr_index], &tmpbuf, 3);
		radio->wr_index += 3;

		/* wrap write pointer */
		if (radio->wr_index >= radio->buf_size)
			radio->wr_index = 0;

		/* check for overflow */
		if (radio->wr_index == radio->rd_index) {
			/* increment and wrap read pointer */
			radio->rd_index += 3;
			if (radio->rd_index >= radio->buf_size)
				radio->rd_index = 0;
		}
	}

	if (radio->wr_index != radio->rd_index)
		wake_up_interruptible(&radio->read_queue);

end:
	return IRQ_HANDLED;
}


/*
 * si470x_i2c_probe - probe for the device
 */
static int si470x_i2c_probe(struct i2c_client *client)
{
	struct si470x_device *radio;
	int retval = 0;
	unsigned char version_warning = 0;

	/* private data allocation and initialization */
	radio = devm_kzalloc(&client->dev, sizeof(*radio), GFP_KERNEL);
	if (!radio) {
		retval = -ENOMEM;
		goto err_initial;
	}

	radio->client = client;
	radio->band = 1; /* Default to 76 - 108 MHz */
	mutex_init(&radio->lock);
	init_completion(&radio->completion);

	radio->get_register = si470x_get_register;
	radio->set_register = si470x_set_register;
	radio->fops_open = si470x_fops_open;
	radio->fops_release = si470x_fops_release;
	radio->vidioc_querycap = si470x_vidioc_querycap;

	retval = v4l2_device_register(&client->dev, &radio->v4l2_dev);
	if (retval < 0) {
		dev_err(&client->dev, "couldn't register v4l2_device\n");
		goto err_initial;
	}

	v4l2_ctrl_handler_init(&radio->hdl, 2);
	v4l2_ctrl_new_std(&radio->hdl, &si470x_ctrl_ops,
			V4L2_CID_AUDIO_MUTE, 0, 1, 1, 1);
	v4l2_ctrl_new_std(&radio->hdl, &si470x_ctrl_ops,
			V4L2_CID_AUDIO_VOLUME, 0, 15, 1, 15);
	if (radio->hdl.error) {
		retval = radio->hdl.error;
		dev_err(&client->dev, "couldn't register control\n");
		goto err_dev;
	}

	/* video device initialization */
	radio->videodev = si470x_viddev_template;
	radio->videodev.ctrl_handler = &radio->hdl;
	radio->videodev.lock = &radio->lock;
	radio->videodev.v4l2_dev = &radio->v4l2_dev;
	radio->videodev.release = video_device_release_empty;
	radio->videodev.device_caps =
		V4L2_CAP_HW_FREQ_SEEK | V4L2_CAP_READWRITE | V4L2_CAP_TUNER |
		V4L2_CAP_RADIO | V4L2_CAP_RDS_CAPTURE;
	video_set_drvdata(&radio->videodev, radio);

	radio->gpio_reset = devm_gpiod_get_optional(&client->dev, "reset",
						    GPIOD_OUT_LOW);
	if (IS_ERR(radio->gpio_reset)) {
		retval = PTR_ERR(radio->gpio_reset);
		dev_err(&client->dev, "Failed to request gpio: %d\n", retval);
		goto err_all;
	}

	if (radio->gpio_reset)
		gpiod_set_value(radio->gpio_reset, 1);

	/* power up : need 110ms */
	radio->registers[POWERCFG] = POWERCFG_ENABLE;
	if (si470x_set_register(radio, POWERCFG) < 0) {
		retval = -EIO;
		goto err_all;
	}
	msleep(110);

	/* get device and chip versions */
	if (si470x_get_all_registers(radio) < 0) {
		retval = -EIO;
		goto err_all;
	}
	dev_info(&client->dev, "DeviceID=0x%4.4hx ChipID=0x%4.4hx\n",
			radio->registers[DEVICEID], radio->registers[SI_CHIPID]);
	if ((radio->registers[SI_CHIPID] & SI_CHIPID_FIRMWARE) < RADIO_FW_VERSION) {
		dev_warn(&client->dev,
			"This driver is known to work with firmware version %hu,\n",
			RADIO_FW_VERSION);
		dev_warn(&client->dev,
			"but the device has firmware version %hu.\n",
			radio->registers[SI_CHIPID] & SI_CHIPID_FIRMWARE);
		version_warning = 1;
	}

	/* give out version warning */
	if (version_warning == 1) {
		dev_warn(&client->dev,
			"If you have some trouble using this driver,\n");
		dev_warn(&client->dev,
			"please report to V4L ML at linux-media@vger.kernel.org\n");
	}

	/* set initial frequency */
	si470x_set_freq(radio, 87.5 * FREQ_MUL); /* available in all regions */

	/* rds buffer allocation */
	radio->buf_size = rds_buf * 3;
	radio->buffer = devm_kmalloc(&client->dev, radio->buf_size, GFP_KERNEL);
	if (!radio->buffer) {
		retval = -EIO;
		goto err_all;
	}

	/* rds buffer configuration */
	radio->wr_index = 0;
	radio->rd_index = 0;
	init_waitqueue_head(&radio->read_queue);

	retval = devm_request_threaded_irq(&client->dev, client->irq, NULL,
					   si470x_i2c_interrupt,
					   IRQF_TRIGGER_FALLING | IRQF_ONESHOT,
					   DRIVER_NAME, radio);
	if (retval) {
		dev_err(&client->dev, "Failed to register interrupt\n");
		goto err_all;
	}

	/* register video device */
	retval = video_register_device(&radio->videodev, VFL_TYPE_RADIO,
			radio_nr);
	if (retval) {
		dev_warn(&client->dev, "Could not register video device\n");
		goto err_all;
	}
	i2c_set_clientdata(client, radio);

	return 0;
err_all:
	v4l2_ctrl_handler_free(&radio->hdl);
err_dev:
	v4l2_device_unregister(&radio->v4l2_dev);
err_initial:
	return retval;
}


/*
 * si470x_i2c_remove - remove the device
 */
static int si470x_i2c_remove(struct i2c_client *client)
{
	struct si470x_device *radio = i2c_get_clientdata(client);

	video_unregister_device(&radio->videodev);

	if (radio->gpio_reset)
		gpiod_set_value(radio->gpio_reset, 0);

	v4l2_ctrl_handler_free(&radio->hdl);
	v4l2_device_unregister(&radio->v4l2_dev);
	return 0;
}


#ifdef CONFIG_PM_SLEEP
/*
 * si470x_i2c_suspend - suspend the device
 */
static int si470x_i2c_suspend(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct si470x_device *radio = i2c_get_clientdata(client);

	/* power down */
	radio->registers[POWERCFG] |= POWERCFG_DISABLE;
	if (si470x_set_register(radio, POWERCFG) < 0)
		return -EIO;

	return 0;
}


/*
 * si470x_i2c_resume - resume the device
 */
static int si470x_i2c_resume(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct si470x_device *radio = i2c_get_clientdata(client);

	/* power up : need 110ms */
	radio->registers[POWERCFG] |= POWERCFG_ENABLE;
	if (si470x_set_register(radio, POWERCFG) < 0)
		return -EIO;
	msleep(110);

	return 0;
}

static SIMPLE_DEV_PM_OPS(si470x_i2c_pm, si470x_i2c_suspend, si470x_i2c_resume);
#endif

#if IS_ENABLED(CONFIG_OF)
static const struct of_device_id si470x_of_match[] = {
	{ .compatible = "silabs,si470x" },
	{ },
};
MODULE_DEVICE_TABLE(of, si470x_of_match);
#endif

/*
 * si470x_i2c_driver - i2c driver interface
 */
static struct i2c_driver si470x_i2c_driver = {
	.driver = {
		.name		= "si470x",
		.of_match_table = of_match_ptr(si470x_of_match),
#ifdef CONFIG_PM_SLEEP
		.pm		= &si470x_i2c_pm,
#endif
	},
	.probe_new		= si470x_i2c_probe,
	.remove			= si470x_i2c_remove,
	.id_table		= si470x_i2c_id,
};

module_i2c_driver(si470x_i2c_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION(DRIVER_VERSION);
