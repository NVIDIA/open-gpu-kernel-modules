// SPDX-License-Identifier: GPL-2.0
/*
 * tw9910 Video Driver
 *
 * Copyright (C) 2017 Jacopo Mondi <jacopo+renesas@jmondi.org>
 *
 * Copyright (C) 2008 Renesas Solutions Corp.
 * Kuninori Morimoto <morimoto.kuninori@renesas.com>
 *
 * Based on ov772x driver,
 *
 * Copyright (C) 2008 Kuninori Morimoto <morimoto.kuninori@renesas.com>
 * Copyright 2006-7 Jonathan Corbet <corbet@lwn.net>
 * Copyright (C) 2008 Magnus Damm
 * Copyright (C) 2008, Guennadi Liakhovetski <kernel@pengutronix.de>
 */

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/v4l2-mediabus.h>
#include <linux/videodev2.h>

#include <media/i2c/tw9910.h>
#include <media/v4l2-subdev.h>

#define GET_ID(val)  ((val & 0xF8) >> 3)
#define GET_REV(val) (val & 0x07)

/*
 * register offset
 */
#define ID		0x00 /* Product ID Code Register */
#define STATUS1		0x01 /* Chip Status Register I */
#define INFORM		0x02 /* Input Format */
#define OPFORM		0x03 /* Output Format Control Register */
#define DLYCTR		0x04 /* Hysteresis and HSYNC Delay Control */
#define OUTCTR1		0x05 /* Output Control I */
#define ACNTL1		0x06 /* Analog Control Register 1 */
#define CROP_HI		0x07 /* Cropping Register, High */
#define VDELAY_LO	0x08 /* Vertical Delay Register, Low */
#define VACTIVE_LO	0x09 /* Vertical Active Register, Low */
#define HDELAY_LO	0x0A /* Horizontal Delay Register, Low */
#define HACTIVE_LO	0x0B /* Horizontal Active Register, Low */
#define CNTRL1		0x0C /* Control Register I */
#define VSCALE_LO	0x0D /* Vertical Scaling Register, Low */
#define SCALE_HI	0x0E /* Scaling Register, High */
#define HSCALE_LO	0x0F /* Horizontal Scaling Register, Low */
#define BRIGHT		0x10 /* BRIGHTNESS Control Register */
#define CONTRAST	0x11 /* CONTRAST Control Register */
#define SHARPNESS	0x12 /* SHARPNESS Control Register I */
#define SAT_U		0x13 /* Chroma (U) Gain Register */
#define SAT_V		0x14 /* Chroma (V) Gain Register */
#define HUE		0x15 /* Hue Control Register */
#define CORING1		0x17
#define CORING2		0x18 /* Coring and IF compensation */
#define VBICNTL		0x19 /* VBI Control Register */
#define ACNTL2		0x1A /* Analog Control 2 */
#define OUTCTR2		0x1B /* Output Control 2 */
#define SDT		0x1C /* Standard Selection */
#define SDTR		0x1D /* Standard Recognition */
#define TEST		0x1F /* Test Control Register */
#define CLMPG		0x20 /* Clamping Gain */
#define IAGC		0x21 /* Individual AGC Gain */
#define AGCGAIN		0x22 /* AGC Gain */
#define PEAKWT		0x23 /* White Peak Threshold */
#define CLMPL		0x24 /* Clamp level */
#define SYNCT		0x25 /* Sync Amplitude */
#define MISSCNT		0x26 /* Sync Miss Count Register */
#define PCLAMP		0x27 /* Clamp Position Register */
#define VCNTL1		0x28 /* Vertical Control I */
#define VCNTL2		0x29 /* Vertical Control II */
#define CKILL		0x2A /* Color Killer Level Control */
#define COMB		0x2B /* Comb Filter Control */
#define LDLY		0x2C /* Luma Delay and H Filter Control */
#define MISC1		0x2D /* Miscellaneous Control I */
#define LOOP		0x2E /* LOOP Control Register */
#define MISC2		0x2F /* Miscellaneous Control II */
#define MVSN		0x30 /* Macrovision Detection */
#define STATUS2		0x31 /* Chip STATUS II */
#define HFREF		0x32 /* H monitor */
#define CLMD		0x33 /* CLAMP MODE */
#define IDCNTL		0x34 /* ID Detection Control */
#define CLCNTL1		0x35 /* Clamp Control I */
#define ANAPLLCTL	0x4C
#define VBIMIN		0x4D
#define HSLOWCTL	0x4E
#define WSS3		0x4F
#define FILLDATA	0x50
#define SDID		0x51
#define DID		0x52
#define WSS1		0x53
#define WSS2		0x54
#define VVBI		0x55
#define LCTL6		0x56
#define LCTL7		0x57
#define LCTL8		0x58
#define LCTL9		0x59
#define LCTL10		0x5A
#define LCTL11		0x5B
#define LCTL12		0x5C
#define LCTL13		0x5D
#define LCTL14		0x5E
#define LCTL15		0x5F
#define LCTL16		0x60
#define LCTL17		0x61
#define LCTL18		0x62
#define LCTL19		0x63
#define LCTL20		0x64
#define LCTL21		0x65
#define LCTL22		0x66
#define LCTL23		0x67
#define LCTL24		0x68
#define LCTL25		0x69
#define LCTL26		0x6A
#define HSBEGIN		0x6B
#define HSEND		0x6C
#define OVSDLY		0x6D
#define OVSEND		0x6E
#define VBIDELAY	0x6F

/*
 * register detail
 */

/* INFORM */
#define FC27_ON     0x40 /* 1 : Input crystal clock frequency is 27MHz */
#define FC27_FF     0x00 /* 0 : Square pixel mode. */
			 /*     Must use 24.54MHz for 60Hz field rate */
			 /*     source or 29.5MHz for 50Hz field rate */
#define IFSEL_S     0x10 /* 01 : S-video decoding */
#define IFSEL_C     0x00 /* 00 : Composite video decoding */
			 /* Y input video selection */
#define YSEL_M0     0x00 /*  00 : Mux0 selected */
#define YSEL_M1     0x04 /*  01 : Mux1 selected */
#define YSEL_M2     0x08 /*  10 : Mux2 selected */
#define YSEL_M3     0x10 /*  11 : Mux3 selected */

/* OPFORM */
#define MODE        0x80 /* 0 : CCIR601 compatible YCrCb 4:2:2 format */
			 /* 1 : ITU-R-656 compatible data sequence format */
#define LEN         0x40 /* 0 : 8-bit YCrCb 4:2:2 output format */
			 /* 1 : 16-bit YCrCb 4:2:2 output format.*/
#define LLCMODE     0x20 /* 1 : LLC output mode. */
			 /* 0 : free-run output mode */
#define AINC        0x10 /* Serial interface auto-indexing control */
			 /* 0 : auto-increment */
			 /* 1 : non-auto */
#define VSCTL       0x08 /* 1 : Vertical out ctrl by DVALID */
			 /* 0 : Vertical out ctrl by HACTIVE and DVALID */
#define OEN_TRI_SEL_MASK	0x07
#define OEN_TRI_SEL_ALL_ON	0x00 /* Enable output for Rev0/Rev1 */
#define OEN_TRI_SEL_ALL_OFF_r0	0x06 /* All tri-stated for Rev0 */
#define OEN_TRI_SEL_ALL_OFF_r1	0x07 /* All tri-stated for Rev1 */

/* OUTCTR1 */
#define VSP_LO      0x00 /* 0 : VS pin output polarity is active low */
#define VSP_HI      0x80 /* 1 : VS pin output polarity is active high. */
			 /* VS pin output control */
#define VSSL_VSYNC  0x00 /*   0 : VSYNC  */
#define VSSL_VACT   0x10 /*   1 : VACT   */
#define VSSL_FIELD  0x20 /*   2 : FIELD  */
#define VSSL_VVALID 0x30 /*   3 : VVALID */
#define VSSL_ZERO   0x70 /*   7 : 0      */
#define HSP_LOW     0x00 /* 0 : HS pin output polarity is active low */
#define HSP_HI      0x08 /* 1 : HS pin output polarity is active high.*/
			 /* HS pin output control */
#define HSSL_HACT   0x00 /*   0 : HACT   */
#define HSSL_HSYNC  0x01 /*   1 : HSYNC  */
#define HSSL_DVALID 0x02 /*   2 : DVALID */
#define HSSL_HLOCK  0x03 /*   3 : HLOCK  */
#define HSSL_ASYNCW 0x04 /*   4 : ASYNCW */
#define HSSL_ZERO   0x07 /*   7 : 0      */

/* ACNTL1 */
#define SRESET      0x80 /* resets the device to its default state
			  * but all register content remain unchanged.
			  * This bit is self-resetting.
			  */
#define ACNTL1_PDN_MASK	0x0e
#define CLK_PDN		0x08 /* system clock power down */
#define Y_PDN		0x04 /* Luma ADC power down */
#define C_PDN		0x02 /* Chroma ADC power down */

/* ACNTL2 */
#define ACNTL2_PDN_MASK	0x40
#define PLL_PDN		0x40 /* PLL power down */

/* VBICNTL */

/* RTSEL : control the real time signal output from the MPOUT pin */
#define RTSEL_MASK  0x07
#define RTSEL_VLOSS 0x00 /* 0000 = Video loss */
#define RTSEL_HLOCK 0x01 /* 0001 = H-lock */
#define RTSEL_SLOCK 0x02 /* 0010 = S-lock */
#define RTSEL_VLOCK 0x03 /* 0011 = V-lock */
#define RTSEL_MONO  0x04 /* 0100 = MONO */
#define RTSEL_DET50 0x05 /* 0101 = DET50 */
#define RTSEL_FIELD 0x06 /* 0110 = FIELD */
#define RTSEL_RTCO  0x07 /* 0111 = RTCO ( Real Time Control ) */

/* HSYNC start and end are constant for now */
#define HSYNC_START	0x0260
#define HSYNC_END	0x0300

/*
 * structure
 */

struct regval_list {
	unsigned char reg_num;
	unsigned char value;
};

struct tw9910_scale_ctrl {
	char           *name;
	unsigned short  width;
	unsigned short  height;
	u16             hscale;
	u16             vscale;
};

struct tw9910_priv {
	struct v4l2_subdev		subdev;
	struct clk			*clk;
	struct tw9910_video_info	*info;
	struct gpio_desc		*pdn_gpio;
	struct gpio_desc		*rstb_gpio;
	const struct tw9910_scale_ctrl	*scale;
	v4l2_std_id			norm;
	u32				revision;
};

static const struct tw9910_scale_ctrl tw9910_ntsc_scales[] = {
	{
		.name   = "NTSC SQ",
		.width  = 640,
		.height = 480,
		.hscale = 0x0100,
		.vscale = 0x0100,
	},
	{
		.name   = "NTSC CCIR601",
		.width  = 720,
		.height = 480,
		.hscale = 0x0100,
		.vscale = 0x0100,
	},
	{
		.name   = "NTSC SQ (CIF)",
		.width  = 320,
		.height = 240,
		.hscale = 0x0200,
		.vscale = 0x0200,
	},
	{
		.name   = "NTSC CCIR601 (CIF)",
		.width  = 360,
		.height = 240,
		.hscale = 0x0200,
		.vscale = 0x0200,
	},
	{
		.name   = "NTSC SQ (QCIF)",
		.width  = 160,
		.height = 120,
		.hscale = 0x0400,
		.vscale = 0x0400,
	},
	{
		.name   = "NTSC CCIR601 (QCIF)",
		.width  = 180,
		.height = 120,
		.hscale = 0x0400,
		.vscale = 0x0400,
	},
};

static const struct tw9910_scale_ctrl tw9910_pal_scales[] = {
	{
		.name   = "PAL SQ",
		.width  = 768,
		.height = 576,
		.hscale = 0x0100,
		.vscale = 0x0100,
	},
	{
		.name   = "PAL CCIR601",
		.width  = 720,
		.height = 576,
		.hscale = 0x0100,
		.vscale = 0x0100,
	},
	{
		.name   = "PAL SQ (CIF)",
		.width  = 384,
		.height = 288,
		.hscale = 0x0200,
		.vscale = 0x0200,
	},
	{
		.name   = "PAL CCIR601 (CIF)",
		.width  = 360,
		.height = 288,
		.hscale = 0x0200,
		.vscale = 0x0200,
	},
	{
		.name   = "PAL SQ (QCIF)",
		.width  = 192,
		.height = 144,
		.hscale = 0x0400,
		.vscale = 0x0400,
	},
	{
		.name   = "PAL CCIR601 (QCIF)",
		.width  = 180,
		.height = 144,
		.hscale = 0x0400,
		.vscale = 0x0400,
	},
};

/*
 * general function
 */
static struct tw9910_priv *to_tw9910(const struct i2c_client *client)
{
	return container_of(i2c_get_clientdata(client), struct tw9910_priv,
			    subdev);
}

static int tw9910_mask_set(struct i2c_client *client, u8 command,
			   u8 mask, u8 set)
{
	s32 val = i2c_smbus_read_byte_data(client, command);

	if (val < 0)
		return val;

	val &= ~mask;
	val |= set & mask;

	return i2c_smbus_write_byte_data(client, command, val);
}

static int tw9910_set_scale(struct i2c_client *client,
			    const struct tw9910_scale_ctrl *scale)
{
	int ret;

	ret = i2c_smbus_write_byte_data(client, SCALE_HI,
					(scale->vscale & 0x0F00) >> 4 |
					(scale->hscale & 0x0F00) >> 8);
	if (ret < 0)
		return ret;

	ret = i2c_smbus_write_byte_data(client, HSCALE_LO,
					scale->hscale & 0x00FF);
	if (ret < 0)
		return ret;

	ret = i2c_smbus_write_byte_data(client, VSCALE_LO,
					scale->vscale & 0x00FF);

	return ret;
}

static int tw9910_set_hsync(struct i2c_client *client)
{
	struct tw9910_priv *priv = to_tw9910(client);
	int ret;

	/* bit 10 - 3 */
	ret = i2c_smbus_write_byte_data(client, HSBEGIN,
					(HSYNC_START & 0x07F8) >> 3);
	if (ret < 0)
		return ret;

	/* bit 10 - 3 */
	ret = i2c_smbus_write_byte_data(client, HSEND,
					(HSYNC_END & 0x07F8) >> 3);
	if (ret < 0)
		return ret;

	/* So far only revisions 0 and 1 have been seen. */
	/* bit 2 - 0 */
	if (priv->revision == 1)
		ret = tw9910_mask_set(client, HSLOWCTL, 0x77,
				      (HSYNC_START & 0x0007) << 4 |
				      (HSYNC_END   & 0x0007));

	return ret;
}

static void tw9910_reset(struct i2c_client *client)
{
	tw9910_mask_set(client, ACNTL1, SRESET, SRESET);
	usleep_range(1000, 5000);
}

static int tw9910_power(struct i2c_client *client, int enable)
{
	int ret;
	u8 acntl1;
	u8 acntl2;

	if (enable) {
		acntl1 = 0;
		acntl2 = 0;
	} else {
		acntl1 = CLK_PDN | Y_PDN | C_PDN;
		acntl2 = PLL_PDN;
	}

	ret = tw9910_mask_set(client, ACNTL1, ACNTL1_PDN_MASK, acntl1);
	if (ret < 0)
		return ret;

	return tw9910_mask_set(client, ACNTL2, ACNTL2_PDN_MASK, acntl2);
}

static const struct tw9910_scale_ctrl *tw9910_select_norm(v4l2_std_id norm,
							  u32 width, u32 height)
{
	const struct tw9910_scale_ctrl *scale;
	const struct tw9910_scale_ctrl *ret = NULL;
	__u32 diff = 0xffffffff, tmp;
	int size, i;

	if (norm & V4L2_STD_NTSC) {
		scale = tw9910_ntsc_scales;
		size = ARRAY_SIZE(tw9910_ntsc_scales);
	} else if (norm & V4L2_STD_PAL) {
		scale = tw9910_pal_scales;
		size = ARRAY_SIZE(tw9910_pal_scales);
	} else {
		return NULL;
	}

	for (i = 0; i < size; i++) {
		tmp = abs(width - scale[i].width) +
		      abs(height - scale[i].height);
		if (tmp < diff) {
			diff = tmp;
			ret = scale + i;
		}
	}

	return ret;
}

/*
 * subdevice operations
 */
static int tw9910_s_stream(struct v4l2_subdev *sd, int enable)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct tw9910_priv *priv = to_tw9910(client);
	u8 val;
	int ret;

	if (!enable) {
		switch (priv->revision) {
		case 0:
			val = OEN_TRI_SEL_ALL_OFF_r0;
			break;
		case 1:
			val = OEN_TRI_SEL_ALL_OFF_r1;
			break;
		default:
			dev_err(&client->dev, "un-supported revision\n");
			return -EINVAL;
		}
	} else {
		val = OEN_TRI_SEL_ALL_ON;

		if (!priv->scale) {
			dev_err(&client->dev, "norm select error\n");
			return -EPERM;
		}

		dev_dbg(&client->dev, "%s %dx%d\n",
			priv->scale->name,
			priv->scale->width,
			priv->scale->height);
	}

	ret = tw9910_mask_set(client, OPFORM, OEN_TRI_SEL_MASK, val);
	if (ret < 0)
		return ret;

	return tw9910_power(client, enable);
}

static int tw9910_g_std(struct v4l2_subdev *sd, v4l2_std_id *norm)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct tw9910_priv *priv = to_tw9910(client);

	*norm = priv->norm;

	return 0;
}

static int tw9910_s_std(struct v4l2_subdev *sd, v4l2_std_id norm)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct tw9910_priv *priv = to_tw9910(client);
	const unsigned int hact = 720;
	const unsigned int hdelay = 15;
	unsigned int vact;
	unsigned int vdelay;
	int ret;

	if (!(norm & (V4L2_STD_NTSC | V4L2_STD_PAL)))
		return -EINVAL;

	priv->norm = norm;
	if (norm & V4L2_STD_525_60) {
		vact = 240;
		vdelay = 18;
		ret = tw9910_mask_set(client, VVBI, 0x10, 0x10);
	} else {
		vact = 288;
		vdelay = 24;
		ret = tw9910_mask_set(client, VVBI, 0x10, 0x00);
	}
	if (!ret)
		ret = i2c_smbus_write_byte_data(client, CROP_HI,
						((vdelay >> 2) & 0xc0)	|
						((vact >> 4) & 0x30)	|
						((hdelay >> 6) & 0x0c)	|
						((hact >> 8) & 0x03));
	if (!ret)
		ret = i2c_smbus_write_byte_data(client, VDELAY_LO,
						vdelay & 0xff);
	if (!ret)
		ret = i2c_smbus_write_byte_data(client, VACTIVE_LO,
						vact & 0xff);

	return ret;
}

#ifdef CONFIG_VIDEO_ADV_DEBUG
static int tw9910_g_register(struct v4l2_subdev *sd,
			     struct v4l2_dbg_register *reg)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int ret;

	if (reg->reg > 0xff)
		return -EINVAL;

	reg->size = 1;
	ret = i2c_smbus_read_byte_data(client, reg->reg);
	if (ret < 0)
		return ret;

	/*
	 * ret      = int
	 * reg->val = __u64
	 */
	reg->val = (__u64)ret;

	return 0;
}

static int tw9910_s_register(struct v4l2_subdev *sd,
			     const struct v4l2_dbg_register *reg)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);

	if (reg->reg > 0xff ||
	    reg->val > 0xff)
		return -EINVAL;

	return i2c_smbus_write_byte_data(client, reg->reg, reg->val);
}
#endif

static void tw9910_set_gpio_value(struct gpio_desc *desc, int value)
{
	if (desc) {
		gpiod_set_value(desc, value);
		usleep_range(500, 1000);
	}
}

static int tw9910_power_on(struct tw9910_priv *priv)
{
	struct i2c_client *client = v4l2_get_subdevdata(&priv->subdev);
	int ret;

	if (priv->clk) {
		ret = clk_prepare_enable(priv->clk);
		if (ret)
			return ret;
	}

	tw9910_set_gpio_value(priv->pdn_gpio, 0);

	/*
	 * FIXME: The reset signal is connected to a shared GPIO on some
	 * platforms (namely the SuperH Migo-R). Until a framework becomes
	 * available to handle this cleanly, request the GPIO temporarily
	 * to avoid conflicts.
	 */
	priv->rstb_gpio = gpiod_get_optional(&client->dev, "rstb",
					     GPIOD_OUT_LOW);
	if (IS_ERR(priv->rstb_gpio)) {
		dev_info(&client->dev, "Unable to get GPIO \"rstb\"");
		clk_disable_unprepare(priv->clk);
		tw9910_set_gpio_value(priv->pdn_gpio, 1);
		return PTR_ERR(priv->rstb_gpio);
	}

	if (priv->rstb_gpio) {
		tw9910_set_gpio_value(priv->rstb_gpio, 1);
		tw9910_set_gpio_value(priv->rstb_gpio, 0);

		gpiod_put(priv->rstb_gpio);
	}

	return 0;
}

static int tw9910_power_off(struct tw9910_priv *priv)
{
	clk_disable_unprepare(priv->clk);
	tw9910_set_gpio_value(priv->pdn_gpio, 1);

	return 0;
}

static int tw9910_s_power(struct v4l2_subdev *sd, int on)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct tw9910_priv *priv = to_tw9910(client);

	return on ? tw9910_power_on(priv) : tw9910_power_off(priv);
}

static int tw9910_set_frame(struct v4l2_subdev *sd, u32 *width, u32 *height)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct tw9910_priv *priv = to_tw9910(client);
	int ret = -EINVAL;
	u8 val;

	/* Select suitable norm. */
	priv->scale = tw9910_select_norm(priv->norm, *width, *height);
	if (!priv->scale)
		goto tw9910_set_fmt_error;

	/* Reset hardware. */
	tw9910_reset(client);

	/* Set bus width. */
	val = 0x00;
	if (priv->info->buswidth == 16)
		val = LEN;

	ret = tw9910_mask_set(client, OPFORM, LEN, val);
	if (ret < 0)
		goto tw9910_set_fmt_error;

	/* Select MPOUT behavior. */
	switch (priv->info->mpout) {
	case TW9910_MPO_VLOSS:
		val = RTSEL_VLOSS; break;
	case TW9910_MPO_HLOCK:
		val = RTSEL_HLOCK; break;
	case TW9910_MPO_SLOCK:
		val = RTSEL_SLOCK; break;
	case TW9910_MPO_VLOCK:
		val = RTSEL_VLOCK; break;
	case TW9910_MPO_MONO:
		val = RTSEL_MONO;  break;
	case TW9910_MPO_DET50:
		val = RTSEL_DET50; break;
	case TW9910_MPO_FIELD:
		val = RTSEL_FIELD; break;
	case TW9910_MPO_RTCO:
		val = RTSEL_RTCO;  break;
	default:
		val = 0;
	}

	ret = tw9910_mask_set(client, VBICNTL, RTSEL_MASK, val);
	if (ret < 0)
		goto tw9910_set_fmt_error;

	/* Set scale. */
	ret = tw9910_set_scale(client, priv->scale);
	if (ret < 0)
		goto tw9910_set_fmt_error;

	/* Set hsync. */
	ret = tw9910_set_hsync(client);
	if (ret < 0)
		goto tw9910_set_fmt_error;

	*width = priv->scale->width;
	*height = priv->scale->height;

	return ret;

tw9910_set_fmt_error:

	tw9910_reset(client);
	priv->scale = NULL;

	return ret;
}

static int tw9910_get_selection(struct v4l2_subdev *sd,
				struct v4l2_subdev_pad_config *cfg,
				struct v4l2_subdev_selection *sel)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct tw9910_priv *priv = to_tw9910(client);

	if (sel->which != V4L2_SUBDEV_FORMAT_ACTIVE)
		return -EINVAL;
	/* Only CROP, CROP_DEFAULT and CROP_BOUNDS are supported. */
	if (sel->target > V4L2_SEL_TGT_CROP_BOUNDS)
		return -EINVAL;

	sel->r.left	= 0;
	sel->r.top	= 0;
	if (priv->norm & V4L2_STD_NTSC) {
		sel->r.width	= 640;
		sel->r.height	= 480;
	} else {
		sel->r.width	= 768;
		sel->r.height	= 576;
	}

	return 0;
}

static int tw9910_get_fmt(struct v4l2_subdev *sd,
			  struct v4l2_subdev_pad_config *cfg,
			  struct v4l2_subdev_format *format)
{
	struct v4l2_mbus_framefmt *mf = &format->format;
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct tw9910_priv *priv = to_tw9910(client);

	if (format->pad)
		return -EINVAL;

	if (!priv->scale) {
		priv->scale = tw9910_select_norm(priv->norm, 640, 480);
		if (!priv->scale)
			return -EINVAL;
	}

	mf->width	= priv->scale->width;
	mf->height	= priv->scale->height;
	mf->code	= MEDIA_BUS_FMT_UYVY8_2X8;
	mf->colorspace	= V4L2_COLORSPACE_SMPTE170M;
	mf->field	= V4L2_FIELD_INTERLACED_BT;

	return 0;
}

static int tw9910_s_fmt(struct v4l2_subdev *sd,
			struct v4l2_mbus_framefmt *mf)
{
	u32 width = mf->width, height = mf->height;
	int ret;

	WARN_ON(mf->field != V4L2_FIELD_ANY &&
		mf->field != V4L2_FIELD_INTERLACED_BT);

	/* Check color format. */
	if (mf->code != MEDIA_BUS_FMT_UYVY8_2X8)
		return -EINVAL;

	mf->colorspace = V4L2_COLORSPACE_SMPTE170M;

	ret = tw9910_set_frame(sd, &width, &height);
	if (ret)
		return ret;

	mf->width	= width;
	mf->height	= height;

	return 0;
}

static int tw9910_set_fmt(struct v4l2_subdev *sd,
			  struct v4l2_subdev_pad_config *cfg,
			  struct v4l2_subdev_format *format)
{
	struct v4l2_mbus_framefmt *mf = &format->format;
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct tw9910_priv *priv = to_tw9910(client);
	const struct tw9910_scale_ctrl *scale;

	if (format->pad)
		return -EINVAL;

	if (mf->field == V4L2_FIELD_ANY) {
		mf->field = V4L2_FIELD_INTERLACED_BT;
	} else if (mf->field != V4L2_FIELD_INTERLACED_BT) {
		dev_err(&client->dev, "Field type %d invalid\n", mf->field);
		return -EINVAL;
	}

	mf->code = MEDIA_BUS_FMT_UYVY8_2X8;
	mf->colorspace = V4L2_COLORSPACE_SMPTE170M;

	/* Select suitable norm. */
	scale = tw9910_select_norm(priv->norm, mf->width, mf->height);
	if (!scale)
		return -EINVAL;

	mf->width	= scale->width;
	mf->height	= scale->height;

	if (format->which == V4L2_SUBDEV_FORMAT_ACTIVE)
		return tw9910_s_fmt(sd, mf);

	cfg->try_fmt = *mf;

	return 0;
}

static int tw9910_video_probe(struct i2c_client *client)
{
	struct tw9910_priv *priv = to_tw9910(client);
	s32 id;
	int ret;

	/* TW9910 only use 8 or 16 bit bus width. */
	if (priv->info->buswidth != 16 && priv->info->buswidth != 8) {
		dev_err(&client->dev, "bus width error\n");
		return -ENODEV;
	}

	ret = tw9910_s_power(&priv->subdev, 1);
	if (ret < 0)
		return ret;

	/*
	 * Check and show Product ID.
	 * So far only revisions 0 and 1 have been seen.
	 */
	id = i2c_smbus_read_byte_data(client, ID);
	priv->revision = GET_REV(id);
	id = GET_ID(id);

	if (id != 0x0b || priv->revision > 0x01) {
		dev_err(&client->dev, "Product ID error %x:%x\n",
			id, priv->revision);
		ret = -ENODEV;
		goto done;
	}

	dev_info(&client->dev, "tw9910 Product ID %0x:%0x\n",
		 id, priv->revision);

	priv->norm = V4L2_STD_NTSC;
	priv->scale = &tw9910_ntsc_scales[0];

done:
	tw9910_s_power(&priv->subdev, 0);

	return ret;
}

static const struct v4l2_subdev_core_ops tw9910_subdev_core_ops = {
#ifdef CONFIG_VIDEO_ADV_DEBUG
	.g_register	= tw9910_g_register,
	.s_register	= tw9910_s_register,
#endif
	.s_power	= tw9910_s_power,
};

static int tw9910_enum_mbus_code(struct v4l2_subdev *sd,
				 struct v4l2_subdev_pad_config *cfg,
				 struct v4l2_subdev_mbus_code_enum *code)
{
	if (code->pad || code->index)
		return -EINVAL;

	code->code = MEDIA_BUS_FMT_UYVY8_2X8;

	return 0;
}

static int tw9910_g_tvnorms(struct v4l2_subdev *sd, v4l2_std_id *norm)
{
	*norm = V4L2_STD_NTSC | V4L2_STD_PAL;

	return 0;
}

static const struct v4l2_subdev_video_ops tw9910_subdev_video_ops = {
	.s_std		= tw9910_s_std,
	.g_std		= tw9910_g_std,
	.s_stream	= tw9910_s_stream,
	.g_tvnorms	= tw9910_g_tvnorms,
};

static const struct v4l2_subdev_pad_ops tw9910_subdev_pad_ops = {
	.enum_mbus_code = tw9910_enum_mbus_code,
	.get_selection	= tw9910_get_selection,
	.get_fmt	= tw9910_get_fmt,
	.set_fmt	= tw9910_set_fmt,
};

static const struct v4l2_subdev_ops tw9910_subdev_ops = {
	.core	= &tw9910_subdev_core_ops,
	.video	= &tw9910_subdev_video_ops,
	.pad	= &tw9910_subdev_pad_ops,
};

/*
 * i2c_driver function
 */

static int tw9910_probe(struct i2c_client *client,
			const struct i2c_device_id *did)

{
	struct tw9910_priv		*priv;
	struct tw9910_video_info	*info;
	struct i2c_adapter		*adapter = client->adapter;
	int ret;

	if (!client->dev.platform_data) {
		dev_err(&client->dev, "TW9910: missing platform data!\n");
		return -EINVAL;
	}

	info = client->dev.platform_data;

	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA)) {
		dev_err(&client->dev,
			"I2C-Adapter doesn't support I2C_FUNC_SMBUS_BYTE_DATA\n");
		return -EIO;
	}

	priv = devm_kzalloc(&client->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->info = info;

	v4l2_i2c_subdev_init(&priv->subdev, client, &tw9910_subdev_ops);

	priv->clk = clk_get(&client->dev, "xti");
	if (PTR_ERR(priv->clk) == -ENOENT) {
		priv->clk = NULL;
	} else if (IS_ERR(priv->clk)) {
		dev_err(&client->dev, "Unable to get xti clock\n");
		return PTR_ERR(priv->clk);
	}

	priv->pdn_gpio = gpiod_get_optional(&client->dev, "pdn",
					    GPIOD_OUT_HIGH);
	if (IS_ERR(priv->pdn_gpio)) {
		dev_info(&client->dev, "Unable to get GPIO \"pdn\"");
		ret = PTR_ERR(priv->pdn_gpio);
		goto error_clk_put;
	}

	ret = tw9910_video_probe(client);
	if (ret < 0)
		goto error_gpio_put;

	ret = v4l2_async_register_subdev(&priv->subdev);
	if (ret)
		goto error_gpio_put;

	return ret;

error_gpio_put:
	if (priv->pdn_gpio)
		gpiod_put(priv->pdn_gpio);
error_clk_put:
	clk_put(priv->clk);

	return ret;
}

static int tw9910_remove(struct i2c_client *client)
{
	struct tw9910_priv *priv = to_tw9910(client);

	if (priv->pdn_gpio)
		gpiod_put(priv->pdn_gpio);
	clk_put(priv->clk);
	v4l2_async_unregister_subdev(&priv->subdev);

	return 0;
}

static const struct i2c_device_id tw9910_id[] = {
	{ "tw9910", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, tw9910_id);

static struct i2c_driver tw9910_i2c_driver = {
	.driver = {
		.name = "tw9910",
	},
	.probe    = tw9910_probe,
	.remove   = tw9910_remove,
	.id_table = tw9910_id,
};

module_i2c_driver(tw9910_i2c_driver);

MODULE_DESCRIPTION("V4L2 driver for TW9910 video decoder");
MODULE_AUTHOR("Kuninori Morimoto");
MODULE_LICENSE("GPL v2");
