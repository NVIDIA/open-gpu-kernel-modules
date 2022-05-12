// SPDX-License-Identifier: GPL-2.0-only
/*
 * radio-timb.c Timberdale FPGA Radio driver
 * Copyright (c) 2009 Intel Corporation
 */

#include <linux/io.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-event.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/platform_data/media/timb_radio.h>

#define DRIVER_NAME "timb-radio"

struct timbradio {
	struct timb_radio_platform_data	pdata;
	struct v4l2_subdev	*sd_tuner;
	struct v4l2_subdev	*sd_dsp;
	struct video_device	video_dev;
	struct v4l2_device	v4l2_dev;
	struct mutex		lock;
};


static int timbradio_vidioc_querycap(struct file *file, void  *priv,
	struct v4l2_capability *v)
{
	strscpy(v->driver, DRIVER_NAME, sizeof(v->driver));
	strscpy(v->card, "Timberdale Radio", sizeof(v->card));
	snprintf(v->bus_info, sizeof(v->bus_info), "platform:"DRIVER_NAME);
	return 0;
}

static int timbradio_vidioc_g_tuner(struct file *file, void *priv,
	struct v4l2_tuner *v)
{
	struct timbradio *tr = video_drvdata(file);
	return v4l2_subdev_call(tr->sd_tuner, tuner, g_tuner, v);
}

static int timbradio_vidioc_s_tuner(struct file *file, void *priv,
	const struct v4l2_tuner *v)
{
	struct timbradio *tr = video_drvdata(file);
	return v4l2_subdev_call(tr->sd_tuner, tuner, s_tuner, v);
}

static int timbradio_vidioc_s_frequency(struct file *file, void *priv,
	const struct v4l2_frequency *f)
{
	struct timbradio *tr = video_drvdata(file);
	return v4l2_subdev_call(tr->sd_tuner, tuner, s_frequency, f);
}

static int timbradio_vidioc_g_frequency(struct file *file, void *priv,
	struct v4l2_frequency *f)
{
	struct timbradio *tr = video_drvdata(file);
	return v4l2_subdev_call(tr->sd_tuner, tuner, g_frequency, f);
}

static const struct v4l2_ioctl_ops timbradio_ioctl_ops = {
	.vidioc_querycap	= timbradio_vidioc_querycap,
	.vidioc_g_tuner		= timbradio_vidioc_g_tuner,
	.vidioc_s_tuner		= timbradio_vidioc_s_tuner,
	.vidioc_g_frequency	= timbradio_vidioc_g_frequency,
	.vidioc_s_frequency	= timbradio_vidioc_s_frequency,
	.vidioc_log_status      = v4l2_ctrl_log_status,
	.vidioc_subscribe_event = v4l2_ctrl_subscribe_event,
	.vidioc_unsubscribe_event = v4l2_event_unsubscribe,
};

static const struct v4l2_file_operations timbradio_fops = {
	.owner		= THIS_MODULE,
	.open		= v4l2_fh_open,
	.release	= v4l2_fh_release,
	.poll		= v4l2_ctrl_poll,
	.unlocked_ioctl	= video_ioctl2,
};

static int timbradio_probe(struct platform_device *pdev)
{
	struct timb_radio_platform_data *pdata = pdev->dev.platform_data;
	struct timbradio *tr;
	int err;

	if (!pdata) {
		dev_err(&pdev->dev, "Platform data missing\n");
		err = -EINVAL;
		goto err;
	}

	tr = devm_kzalloc(&pdev->dev, sizeof(*tr), GFP_KERNEL);
	if (!tr) {
		err = -ENOMEM;
		goto err;
	}

	tr->pdata = *pdata;
	mutex_init(&tr->lock);

	strscpy(tr->video_dev.name, "Timberdale Radio",
		sizeof(tr->video_dev.name));
	tr->video_dev.fops = &timbradio_fops;
	tr->video_dev.ioctl_ops = &timbradio_ioctl_ops;
	tr->video_dev.release = video_device_release_empty;
	tr->video_dev.minor = -1;
	tr->video_dev.lock = &tr->lock;
	tr->video_dev.device_caps = V4L2_CAP_TUNER | V4L2_CAP_RADIO;

	strscpy(tr->v4l2_dev.name, DRIVER_NAME, sizeof(tr->v4l2_dev.name));
	err = v4l2_device_register(NULL, &tr->v4l2_dev);
	if (err)
		goto err;

	tr->video_dev.v4l2_dev = &tr->v4l2_dev;

	tr->sd_tuner = v4l2_i2c_new_subdev_board(&tr->v4l2_dev,
		i2c_get_adapter(pdata->i2c_adapter), pdata->tuner, NULL);
	tr->sd_dsp = v4l2_i2c_new_subdev_board(&tr->v4l2_dev,
		i2c_get_adapter(pdata->i2c_adapter), pdata->dsp, NULL);
	if (tr->sd_tuner == NULL || tr->sd_dsp == NULL) {
		err = -ENODEV;
		goto err_video_req;
	}

	tr->v4l2_dev.ctrl_handler = tr->sd_dsp->ctrl_handler;

	err = video_register_device(&tr->video_dev, VFL_TYPE_RADIO, -1);
	if (err) {
		dev_err(&pdev->dev, "Error reg video\n");
		goto err_video_req;
	}

	video_set_drvdata(&tr->video_dev, tr);

	platform_set_drvdata(pdev, tr);
	return 0;

err_video_req:
	v4l2_device_unregister(&tr->v4l2_dev);
err:
	dev_err(&pdev->dev, "Failed to register: %d\n", err);

	return err;
}

static int timbradio_remove(struct platform_device *pdev)
{
	struct timbradio *tr = platform_get_drvdata(pdev);

	video_unregister_device(&tr->video_dev);
	v4l2_device_unregister(&tr->v4l2_dev);
	return 0;
}

static struct platform_driver timbradio_platform_driver = {
	.driver = {
		.name	= DRIVER_NAME,
	},
	.probe		= timbradio_probe,
	.remove		= timbradio_remove,
};

module_platform_driver(timbradio_platform_driver);

MODULE_DESCRIPTION("Timberdale Radio driver");
MODULE_AUTHOR("Mocean Laboratories <info@mocean-labs.com>");
MODULE_LICENSE("GPL v2");
MODULE_VERSION("0.0.2");
MODULE_ALIAS("platform:"DRIVER_NAME);
