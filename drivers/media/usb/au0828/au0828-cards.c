// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for the Auvitek USB bridge
 *
 *  Copyright (c) 2008 Steven Toth <stoth@linuxtv.org>
 */

#include "au0828.h"
#include "au0828-cards.h"
#include "au8522.h"
#include "media/tuner.h"
#include "media/v4l2-common.h"

static void hvr950q_cs5340_audio(void *priv, int enable)
{
	/* Because the HVR-950q shares an i2s bus between the cs5340 and the
	   au8522, we need to hold cs5340 in reset when using the au8522 */
	struct au0828_dev *dev = priv;
	if (enable == 1)
		au0828_set(dev, REG_000, 0x10);
	else
		au0828_clear(dev, REG_000, 0x10);
}

/*
 * WARNING: There's a quirks table at sound/usb/quirks-table.h
 * that should also be updated every time a new device with V4L2 support
 * is added here.
 */
struct au0828_board au0828_boards[] = {
	[AU0828_BOARD_UNKNOWN] = {
		.name	= "Unknown board",
		.tuner_type = -1U,
		.tuner_addr = ADDR_UNSET,
	},
	[AU0828_BOARD_HAUPPAUGE_HVR850] = {
		.name	= "Hauppauge HVR850",
		.tuner_type = TUNER_XC5000,
		.tuner_addr = 0x61,
		.has_ir_i2c = 1,
		.has_analog = 1,
		.i2c_clk_divider = AU0828_I2C_CLK_250KHZ,
		.input = {
			{
				.type = AU0828_VMUX_TELEVISION,
				.vmux = AU8522_COMPOSITE_CH4_SIF,
				.amux = AU8522_AUDIO_SIF,
			},
			{
				.type = AU0828_VMUX_COMPOSITE,
				.vmux = AU8522_COMPOSITE_CH1,
				.amux = AU8522_AUDIO_NONE,
				.audio_setup = hvr950q_cs5340_audio,
			},
			{
				.type = AU0828_VMUX_SVIDEO,
				.vmux = AU8522_SVIDEO_CH13,
				.amux = AU8522_AUDIO_NONE,
				.audio_setup = hvr950q_cs5340_audio,
			},
		},
	},
	[AU0828_BOARD_HAUPPAUGE_HVR950Q] = {
		.name	= "Hauppauge HVR950Q",
		.tuner_type = TUNER_XC5000,
		.tuner_addr = 0x61,
		.has_ir_i2c = 1,
		.has_analog = 1,
		.i2c_clk_divider = AU0828_I2C_CLK_250KHZ,
		.input = {
			{
				.type = AU0828_VMUX_TELEVISION,
				.vmux = AU8522_COMPOSITE_CH4_SIF,
				.amux = AU8522_AUDIO_SIF,
			},
			{
				.type = AU0828_VMUX_COMPOSITE,
				.vmux = AU8522_COMPOSITE_CH1,
				.amux = AU8522_AUDIO_NONE,
				.audio_setup = hvr950q_cs5340_audio,
			},
			{
				.type = AU0828_VMUX_SVIDEO,
				.vmux = AU8522_SVIDEO_CH13,
				.amux = AU8522_AUDIO_NONE,
				.audio_setup = hvr950q_cs5340_audio,
			},
		},
	},
	[AU0828_BOARD_HAUPPAUGE_HVR950Q_MXL] = {
		.name	= "Hauppauge HVR950Q rev xxF8",
		.tuner_type = TUNER_XC5000,
		.tuner_addr = 0x61,
		.i2c_clk_divider = AU0828_I2C_CLK_250KHZ,
	},
	[AU0828_BOARD_DVICO_FUSIONHDTV7] = {
		.name	= "DViCO FusionHDTV USB",
		.tuner_type = TUNER_XC5000,
		.tuner_addr = 0x61,
		.i2c_clk_divider = AU0828_I2C_CLK_250KHZ,
	},
	[AU0828_BOARD_HAUPPAUGE_WOODBURY] = {
		.name = "Hauppauge Woodbury",
		.tuner_type = TUNER_NXP_TDA18271,
		.tuner_addr = 0x60,
		.i2c_clk_divider = AU0828_I2C_CLK_250KHZ,
	},
};

/* Tuner callback function for au0828 boards. Currently only needed
 * for HVR1500Q, which has an xc5000 tuner.
 */
int au0828_tuner_callback(void *priv, int component, int command, int arg)
{
	struct au0828_dev *dev = priv;

	dprintk(1, "%s()\n", __func__);

	switch (dev->boardnr) {
	case AU0828_BOARD_HAUPPAUGE_HVR850:
	case AU0828_BOARD_HAUPPAUGE_HVR950Q:
	case AU0828_BOARD_HAUPPAUGE_HVR950Q_MXL:
	case AU0828_BOARD_DVICO_FUSIONHDTV7:
		if (command == 0) {
			/* Tuner Reset Command from xc5000 */
			/* Drive the tuner into reset and out */
			au0828_clear(dev, REG_001, 2);
			mdelay(10);
			au0828_set(dev, REG_001, 2);
			mdelay(10);
			return 0;
		} else {
			pr_err("%s(): Unknown command.\n", __func__);
			return -EINVAL;
		}
		break;
	}

	return 0; /* Should never be here */
}

static void hauppauge_eeprom(struct au0828_dev *dev, u8 *eeprom_data)
{
	struct tveeprom tv;

	tveeprom_hauppauge_analog(&tv, eeprom_data);
	dev->board.tuner_type = tv.tuner_type;

	/* Make sure we support the board model */
	switch (tv.model) {
	case 72000: /* WinTV-HVR950q (Retail, IR, ATSC/QAM */
	case 72001: /* WinTV-HVR950q (Retail, IR, ATSC/QAM and analog video */
	case 72101: /* WinTV-HVR950q (Retail, IR, ATSC/QAM and analog video */
	case 72201: /* WinTV-HVR950q (OEM, IR, ATSC/QAM and analog video */
	case 72211: /* WinTV-HVR950q (OEM, IR, ATSC/QAM and analog video */
	case 72221: /* WinTV-HVR950q (OEM, IR, ATSC/QAM and analog video */
	case 72231: /* WinTV-HVR950q (OEM, IR, ATSC/QAM and analog video */
	case 72241: /* WinTV-HVR950q (OEM, No IR, ATSC/QAM and analog video */
	case 72251: /* WinTV-HVR950q (Retail, IR, ATSC/QAM and analog video */
	case 72261: /* WinTV-HVR950q (OEM, No IR, ATSC/QAM and analog video */
	case 72271: /* WinTV-HVR950q (OEM, No IR, ATSC/QAM and analog video */
	case 72281: /* WinTV-HVR950q (OEM, No IR, ATSC/QAM and analog video */
	case 72301: /* WinTV-HVR850 (Retail, IR, ATSC and analog video */
	case 72500: /* WinTV-HVR950q (OEM, No IR, ATSC/QAM */
		break;
	default:
		pr_warn("%s: warning: unknown hauppauge model #%d\n",
			__func__, tv.model);
		break;
	}

	pr_info("%s: hauppauge eeprom: model=%d\n",
	       __func__, tv.model);
}

void au0828_card_analog_fe_setup(struct au0828_dev *dev);

void au0828_card_setup(struct au0828_dev *dev)
{
	static u8 eeprom[256];

	dprintk(1, "%s()\n", __func__);

	if (dev->i2c_rc == 0) {
		dev->i2c_client.addr = 0xa0 >> 1;
		tveeprom_read(&dev->i2c_client, eeprom, sizeof(eeprom));
	}

	switch (dev->boardnr) {
	case AU0828_BOARD_HAUPPAUGE_HVR850:
	case AU0828_BOARD_HAUPPAUGE_HVR950Q:
	case AU0828_BOARD_HAUPPAUGE_HVR950Q_MXL:
	case AU0828_BOARD_HAUPPAUGE_WOODBURY:
		if (dev->i2c_rc == 0)
			hauppauge_eeprom(dev, eeprom+0xa0);
		break;
	}

	au0828_card_analog_fe_setup(dev);
}

void au0828_card_analog_fe_setup(struct au0828_dev *dev)
{
#ifdef CONFIG_VIDEO_AU0828_V4L2
	struct tuner_setup tun_setup;
	struct v4l2_subdev *sd;
	unsigned int mode_mask = T_ANALOG_TV;

	if (AUVI_INPUT(0).type != AU0828_VMUX_UNDEFINED) {
		/* Load the analog demodulator driver (note this would need to
		   be abstracted out if we ever need to support a different
		   demod) */
		sd = v4l2_i2c_new_subdev(&dev->v4l2_dev, &dev->i2c_adap,
				"au8522", 0x8e >> 1, NULL);
		if (sd == NULL)
			pr_err("analog subdev registration failed\n");
	}

	/* Setup tuners */
	if (dev->board.tuner_type != TUNER_ABSENT && dev->board.has_analog) {
		/* Load the tuner module, which does the attach */
		sd = v4l2_i2c_new_subdev(&dev->v4l2_dev, &dev->i2c_adap,
				"tuner", dev->board.tuner_addr, NULL);
		if (sd == NULL)
			pr_err("tuner subdev registration fail\n");

		tun_setup.mode_mask      = mode_mask;
		tun_setup.type           = dev->board.tuner_type;
		tun_setup.addr           = dev->board.tuner_addr;
		tun_setup.tuner_callback = au0828_tuner_callback;
		v4l2_device_call_all(&dev->v4l2_dev, 0, tuner, s_type_addr,
				     &tun_setup);
	}
#endif
}

/*
 * The bridge has between 8 and 12 gpios.
 * Regs 1 and 0 deal with output enables.
 * Regs 3 and 2 deal with direction.
 */
void au0828_gpio_setup(struct au0828_dev *dev)
{
	dprintk(1, "%s()\n", __func__);

	switch (dev->boardnr) {
	case AU0828_BOARD_HAUPPAUGE_HVR850:
	case AU0828_BOARD_HAUPPAUGE_HVR950Q:
	case AU0828_BOARD_HAUPPAUGE_HVR950Q_MXL:
	case AU0828_BOARD_HAUPPAUGE_WOODBURY:
		/* GPIO's
		 * 4 - CS5340
		 * 5 - AU8522 Demodulator
		 * 6 - eeprom W/P
		 * 7 - power supply
		 * 9 - XC5000 Tuner
		 */

		/* Set relevant GPIOs as outputs (leave the EEPROM W/P
		   as an input since we will never touch it and it has
		   a pullup) */
		au0828_write(dev, REG_003, 0x02);
		au0828_write(dev, REG_002, 0x80 | 0x20 | 0x10);

		/* Into reset */
		au0828_write(dev, REG_001, 0x0);
		au0828_write(dev, REG_000, 0x0);
		msleep(50);

		/* Bring power supply out of reset */
		au0828_write(dev, REG_000, 0x80);
		msleep(50);

		/* Bring xc5000 and au8522 out of reset (leave the
		   cs5340 in reset until needed) */
		au0828_write(dev, REG_001, 0x02); /* xc5000 */
		au0828_write(dev, REG_000, 0x80 | 0x20); /* PS + au8522 */

		msleep(250);
		break;
	case AU0828_BOARD_DVICO_FUSIONHDTV7:
		/* GPIO's
		 * 6 - ?
		 * 8 - AU8522 Demodulator
		 * 9 - XC5000 Tuner
		 */

		/* Into reset */
		au0828_write(dev, REG_003, 0x02);
		au0828_write(dev, REG_002, 0xa0);
		au0828_write(dev, REG_001, 0x0);
		au0828_write(dev, REG_000, 0x0);
		msleep(100);

		/* Out of reset */
		au0828_write(dev, REG_003, 0x02);
		au0828_write(dev, REG_002, 0xa0);
		au0828_write(dev, REG_001, 0x02);
		au0828_write(dev, REG_000, 0xa0);
		msleep(250);
		break;
	}
}

/* table of devices that work with this driver */
struct usb_device_id au0828_usb_id_table[] = {
	{ USB_DEVICE(0x2040, 0x7200),
		.driver_info = AU0828_BOARD_HAUPPAUGE_HVR950Q },
	{ USB_DEVICE(0x2040, 0x7240),
		.driver_info = AU0828_BOARD_HAUPPAUGE_HVR850 },
	{ USB_DEVICE(0x0fe9, 0xd620),
		.driver_info = AU0828_BOARD_DVICO_FUSIONHDTV7 },
	{ USB_DEVICE(0x2040, 0x7210),
		.driver_info = AU0828_BOARD_HAUPPAUGE_HVR950Q },
	{ USB_DEVICE(0x2040, 0x7217),
		.driver_info = AU0828_BOARD_HAUPPAUGE_HVR950Q },
	{ USB_DEVICE(0x2040, 0x721b),
		.driver_info = AU0828_BOARD_HAUPPAUGE_HVR950Q },
	{ USB_DEVICE(0x2040, 0x721e),
		.driver_info = AU0828_BOARD_HAUPPAUGE_HVR950Q },
	{ USB_DEVICE(0x2040, 0x721f),
		.driver_info = AU0828_BOARD_HAUPPAUGE_HVR950Q },
	{ USB_DEVICE(0x2040, 0x7280),
		.driver_info = AU0828_BOARD_HAUPPAUGE_HVR950Q },
	{ USB_DEVICE(0x0fd9, 0x0008),
		.driver_info = AU0828_BOARD_HAUPPAUGE_HVR950Q },
	{ USB_DEVICE(0x2040, 0x7201),
		.driver_info = AU0828_BOARD_HAUPPAUGE_HVR950Q_MXL },
	{ USB_DEVICE(0x2040, 0x7211),
		.driver_info = AU0828_BOARD_HAUPPAUGE_HVR950Q_MXL },
	{ USB_DEVICE(0x2040, 0x7281),
		.driver_info = AU0828_BOARD_HAUPPAUGE_HVR950Q_MXL },
	{ USB_DEVICE(0x05e1, 0x0480),
		.driver_info = AU0828_BOARD_HAUPPAUGE_WOODBURY },
	{ USB_DEVICE(0x2040, 0x8200),
		.driver_info = AU0828_BOARD_HAUPPAUGE_WOODBURY },
	{ USB_DEVICE(0x2040, 0x7260),
		.driver_info = AU0828_BOARD_HAUPPAUGE_HVR950Q },
	{ USB_DEVICE(0x2040, 0x7213),
		.driver_info = AU0828_BOARD_HAUPPAUGE_HVR950Q },
	{ USB_DEVICE(0x2040, 0x7270),
		.driver_info = AU0828_BOARD_HAUPPAUGE_HVR950Q },
	{ },
};

MODULE_DEVICE_TABLE(usb, au0828_usb_id_table);
