// SPDX-License-Identifier: GPL-2.0-only
/*
 * tef6862.c Philips TEF6862 Car Radio Enhanced Selectivity Tuner
 * Copyright (c) 2009 Intel Corporation
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-device.h>

#define DRIVER_NAME "tef6862"

#define FREQ_MUL 16000

#define TEF6862_LO_FREQ (875U * FREQ_MUL / 10)
#define TEF6862_HI_FREQ (108U * FREQ_MUL)

/* Write mode sub addresses */
#define WM_SUB_BANDWIDTH	0x0
#define WM_SUB_PLLM		0x1
#define WM_SUB_PLLL		0x2
#define WM_SUB_DAA		0x3
#define WM_SUB_AGC		0x4
#define WM_SUB_BAND		0x5
#define WM_SUB_CONTROL		0x6
#define WM_SUB_LEVEL		0x7
#define WM_SUB_IFCF		0x8
#define WM_SUB_IFCAP		0x9
#define WM_SUB_ACD		0xA
#define WM_SUB_TEST		0xF

/* Different modes of the MSA register */
#define MSA_MODE_BUFFER		0x0
#define MSA_MODE_PRESET		0x1
#define MSA_MODE_SEARCH		0x2
#define MSA_MODE_AF_UPDATE	0x3
#define MSA_MODE_JUMP		0x4
#define MSA_MODE_CHECK		0x5
#define MSA_MODE_LOAD		0x6
#define MSA_MODE_END		0x7
#define MSA_MODE_SHIFT		5

struct tef6862_state {
	struct v4l2_subdev sd;
	unsigned long freq;
};

static inline struct tef6862_state *to_state(struct v4l2_subdev *sd)
{
	return container_of(sd, struct tef6862_state, sd);
}

static u16 tef6862_sigstr(struct i2c_client *client)
{
	u8 buf[4];
	int err = i2c_master_recv(client, buf, sizeof(buf));
	if (err == sizeof(buf))
		return buf[3] << 8;
	return 0;
}

static int tef6862_g_tuner(struct v4l2_subdev *sd, struct v4l2_tuner *v)
{
	if (v->index > 0)
		return -EINVAL;

	/* only support FM for now */
	strscpy(v->name, "FM", sizeof(v->name));
	v->type = V4L2_TUNER_RADIO;
	v->rangelow = TEF6862_LO_FREQ;
	v->rangehigh = TEF6862_HI_FREQ;
	v->rxsubchans = V4L2_TUNER_SUB_MONO;
	v->capability = V4L2_TUNER_CAP_LOW;
	v->audmode = V4L2_TUNER_MODE_STEREO;
	v->signal = tef6862_sigstr(v4l2_get_subdevdata(sd));

	return 0;
}

static int tef6862_s_tuner(struct v4l2_subdev *sd, const struct v4l2_tuner *v)
{
	return v->index ? -EINVAL : 0;
}

static int tef6862_s_frequency(struct v4l2_subdev *sd, const struct v4l2_frequency *f)
{
	struct tef6862_state *state = to_state(sd);
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	unsigned freq = f->frequency;
	u16 pll;
	u8 i2cmsg[3];
	int err;

	if (f->tuner != 0)
		return -EINVAL;

	freq = clamp(freq, TEF6862_LO_FREQ, TEF6862_HI_FREQ);
	pll = 1964 + ((freq - TEF6862_LO_FREQ) * 20) / FREQ_MUL;
	i2cmsg[0] = (MSA_MODE_PRESET << MSA_MODE_SHIFT) | WM_SUB_PLLM;
	i2cmsg[1] = (pll >> 8) & 0xff;
	i2cmsg[2] = pll & 0xff;

	err = i2c_master_send(client, i2cmsg, sizeof(i2cmsg));
	if (err != sizeof(i2cmsg))
		return err < 0 ? err : -EIO;

	state->freq = freq;
	return 0;
}

static int tef6862_g_frequency(struct v4l2_subdev *sd, struct v4l2_frequency *f)
{
	struct tef6862_state *state = to_state(sd);

	if (f->tuner != 0)
		return -EINVAL;
	f->type = V4L2_TUNER_RADIO;
	f->frequency = state->freq;
	return 0;
}

static const struct v4l2_subdev_tuner_ops tef6862_tuner_ops = {
	.g_tuner = tef6862_g_tuner,
	.s_tuner = tef6862_s_tuner,
	.s_frequency = tef6862_s_frequency,
	.g_frequency = tef6862_g_frequency,
};

static const struct v4l2_subdev_ops tef6862_ops = {
	.tuner = &tef6862_tuner_ops,
};

/*
 * Generic i2c probe
 * concerning the addresses: i2c wants 7 bit (without the r/w bit), so '>>1'
 */

static int tef6862_probe(struct i2c_client *client,
			 const struct i2c_device_id *id)
{
	struct tef6862_state *state;
	struct v4l2_subdev *sd;

	/* Check if the adapter supports the needed features */
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_BYTE_DATA))
		return -EIO;

	v4l_info(client, "chip found @ 0x%02x (%s)\n",
			client->addr << 1, client->adapter->name);

	state = kzalloc(sizeof(struct tef6862_state), GFP_KERNEL);
	if (state == NULL)
		return -ENOMEM;
	state->freq = TEF6862_LO_FREQ;

	sd = &state->sd;
	v4l2_i2c_subdev_init(sd, client, &tef6862_ops);

	return 0;
}

static int tef6862_remove(struct i2c_client *client)
{
	struct v4l2_subdev *sd = i2c_get_clientdata(client);

	v4l2_device_unregister_subdev(sd);
	kfree(to_state(sd));
	return 0;
}

static const struct i2c_device_id tef6862_id[] = {
	{DRIVER_NAME, 0},
	{},
};

MODULE_DEVICE_TABLE(i2c, tef6862_id);

static struct i2c_driver tef6862_driver = {
	.driver = {
		.name	= DRIVER_NAME,
	},
	.probe		= tef6862_probe,
	.remove		= tef6862_remove,
	.id_table	= tef6862_id,
};

module_i2c_driver(tef6862_driver);

MODULE_DESCRIPTION("TEF6862 Car Radio Enhanced Selectivity Tuner");
MODULE_AUTHOR("Mocean Laboratories");
MODULE_LICENSE("GPL v2");
