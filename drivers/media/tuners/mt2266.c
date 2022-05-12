// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for Microtune MT2266 "Direct conversion low power broadband tuner"
 *
 *  Copyright (c) 2007 Olivier DANET <odanet@caramail.com>
 */

#include <linux/module.h>
#include <linux/delay.h>
#include <linux/dvb/frontend.h>
#include <linux/i2c.h>
#include <linux/slab.h>

#include <media/dvb_frontend.h>
#include "mt2266.h"

#define I2C_ADDRESS 0x60

#define REG_PART_REV   0
#define REG_TUNE       1
#define REG_BAND       6
#define REG_BANDWIDTH  8
#define REG_LOCK       0x12

#define PART_REV 0x85

struct mt2266_priv {
	struct mt2266_config *cfg;
	struct i2c_adapter   *i2c;

	u32 frequency;
	u32 bandwidth;
	u8 band;
};

#define MT2266_VHF 1
#define MT2266_UHF 0

/* Here, frequencies are expressed in kiloHertz to avoid 32 bits overflows */

static int debug;
module_param(debug, int, 0644);
MODULE_PARM_DESC(debug, "Turn on/off debugging (default:off).");

#define dprintk(args...) do { if (debug) {printk(KERN_DEBUG "MT2266: " args); printk("\n"); }} while (0)

// Reads a single register
static int mt2266_readreg(struct mt2266_priv *priv, u8 reg, u8 *val)
{
	struct i2c_msg msg[2] = {
		{ .addr = priv->cfg->i2c_address, .flags = 0,        .buf = &reg, .len = 1 },
		{ .addr = priv->cfg->i2c_address, .flags = I2C_M_RD, .buf = val,  .len = 1 },
	};
	if (i2c_transfer(priv->i2c, msg, 2) != 2) {
		printk(KERN_WARNING "MT2266 I2C read failed\n");
		return -EREMOTEIO;
	}
	return 0;
}

// Writes a single register
static int mt2266_writereg(struct mt2266_priv *priv, u8 reg, u8 val)
{
	u8 buf[2] = { reg, val };
	struct i2c_msg msg = {
		.addr = priv->cfg->i2c_address, .flags = 0, .buf = buf, .len = 2
	};
	if (i2c_transfer(priv->i2c, &msg, 1) != 1) {
		printk(KERN_WARNING "MT2266 I2C write failed\n");
		return -EREMOTEIO;
	}
	return 0;
}

// Writes a set of consecutive registers
static int mt2266_writeregs(struct mt2266_priv *priv,u8 *buf, u8 len)
{
	struct i2c_msg msg = {
		.addr = priv->cfg->i2c_address, .flags = 0, .buf = buf, .len = len
	};
	if (i2c_transfer(priv->i2c, &msg, 1) != 1) {
		printk(KERN_WARNING "MT2266 I2C write failed (len=%i)\n",(int)len);
		return -EREMOTEIO;
	}
	return 0;
}

// Initialisation sequences
static u8 mt2266_init1[] = { REG_TUNE, 0x00, 0x00, 0x28,
				 0x00, 0x52, 0x99, 0x3f };

static u8 mt2266_init2[] = {
    0x17, 0x6d, 0x71, 0x61, 0xc0, 0xbf, 0xff, 0xdc, 0x00, 0x0a, 0xd4,
    0x03, 0x64, 0x64, 0x64, 0x64, 0x22, 0xaa, 0xf2, 0x1e, 0x80, 0x14,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x7f, 0x5e, 0x3f, 0xff, 0xff,
    0xff, 0x00, 0x77, 0x0f, 0x2d
};

static u8 mt2266_init_8mhz[] = { REG_BANDWIDTH, 0x22, 0x22, 0x22, 0x22,
						0x22, 0x22, 0x22, 0x22 };

static u8 mt2266_init_7mhz[] = { REG_BANDWIDTH, 0x32, 0x32, 0x32, 0x32,
						0x32, 0x32, 0x32, 0x32 };

static u8 mt2266_init_6mhz[] = { REG_BANDWIDTH, 0xa7, 0xa7, 0xa7, 0xa7,
						0xa7, 0xa7, 0xa7, 0xa7 };

static u8 mt2266_uhf[] = { 0x1d, 0xdc, 0x00, 0x0a, 0xd4, 0x03, 0x64, 0x64,
			   0x64, 0x64, 0x22, 0xaa, 0xf2, 0x1e, 0x80, 0x14 };

static u8 mt2266_vhf[] = { 0x1d, 0xfe, 0x00, 0x00, 0xb4, 0x03, 0xa5, 0xa5,
			   0xa5, 0xa5, 0x82, 0xaa, 0xf1, 0x17, 0x80, 0x1f };

#define FREF 30000       // Quartz oscillator 30 MHz

static int mt2266_set_params(struct dvb_frontend *fe)
{
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	struct mt2266_priv *priv;
	int ret=0;
	u32 freq;
	u32 tune;
	u8  lnaband;
	u8  b[10];
	int i;
	u8 band;

	priv = fe->tuner_priv;

	freq = priv->frequency / 1000; /* Hz -> kHz */
	if (freq < 470000 && freq > 230000)
		return -EINVAL; /* Gap between VHF and UHF bands */

	priv->frequency = c->frequency;
	tune = 2 * freq * (8192/16) / (FREF/16);
	band = (freq < 300000) ? MT2266_VHF : MT2266_UHF;
	if (band == MT2266_VHF)
		tune *= 2;

	switch (c->bandwidth_hz) {
	case 6000000:
		mt2266_writeregs(priv, mt2266_init_6mhz,
				 sizeof(mt2266_init_6mhz));
		break;
	case 8000000:
		mt2266_writeregs(priv, mt2266_init_8mhz,
				 sizeof(mt2266_init_8mhz));
		break;
	case 7000000:
	default:
		mt2266_writeregs(priv, mt2266_init_7mhz,
				 sizeof(mt2266_init_7mhz));
		break;
	}
	priv->bandwidth = c->bandwidth_hz;

	if (band == MT2266_VHF && priv->band == MT2266_UHF) {
		dprintk("Switch from UHF to VHF");
		mt2266_writereg(priv, 0x05, 0x04);
		mt2266_writereg(priv, 0x19, 0x61);
		mt2266_writeregs(priv, mt2266_vhf, sizeof(mt2266_vhf));
	} else if (band == MT2266_UHF && priv->band == MT2266_VHF) {
		dprintk("Switch from VHF to UHF");
		mt2266_writereg(priv, 0x05, 0x52);
		mt2266_writereg(priv, 0x19, 0x61);
		mt2266_writeregs(priv, mt2266_uhf, sizeof(mt2266_uhf));
	}
	msleep(10);

	if (freq <= 495000)
		lnaband = 0xEE;
	else if (freq <= 525000)
		lnaband = 0xDD;
	else if (freq <= 550000)
		lnaband = 0xCC;
	else if (freq <= 580000)
		lnaband = 0xBB;
	else if (freq <= 605000)
		lnaband = 0xAA;
	else if (freq <= 630000)
		lnaband = 0x99;
	else if (freq <= 655000)
		lnaband = 0x88;
	else if (freq <= 685000)
		lnaband = 0x77;
	else if (freq <= 710000)
		lnaband = 0x66;
	else if (freq <= 735000)
		lnaband = 0x55;
	else if (freq <= 765000)
		lnaband = 0x44;
	else if (freq <= 802000)
		lnaband = 0x33;
	else if (freq <= 840000)
		lnaband = 0x22;
	else
		lnaband = 0x11;

	b[0] = REG_TUNE;
	b[1] = (tune >> 8) & 0x1F;
	b[2] = tune & 0xFF;
	b[3] = tune >> 13;
	mt2266_writeregs(priv,b,4);

	dprintk("set_parms: tune=%d band=%d %s",
		(int) tune, (int) lnaband,
		(band == MT2266_UHF) ? "UHF" : "VHF");
	dprintk("set_parms: [1..3]: %2x %2x %2x",
		(int) b[1], (int) b[2], (int)b[3]);

	if (band == MT2266_UHF) {
		b[0] = 0x05;
		b[1] = (priv->band == MT2266_VHF) ? 0x52 : 0x62;
		b[2] = lnaband;
		mt2266_writeregs(priv, b, 3);
	}

	/* Wait for pll lock or timeout */
	i = 0;
	do {
		mt2266_readreg(priv,REG_LOCK,b);
		if (b[0] & 0x40)
			break;
		msleep(10);
		i++;
	} while (i<10);
	dprintk("Lock when i=%i",(int)i);

	if (band == MT2266_UHF && priv->band == MT2266_VHF)
		mt2266_writereg(priv, 0x05, 0x62);

	priv->band = band;

	return ret;
}

static void mt2266_calibrate(struct mt2266_priv *priv)
{
	mt2266_writereg(priv, 0x11, 0x03);
	mt2266_writereg(priv, 0x11, 0x01);
	mt2266_writeregs(priv, mt2266_init1, sizeof(mt2266_init1));
	mt2266_writeregs(priv, mt2266_init2, sizeof(mt2266_init2));
	mt2266_writereg(priv, 0x33, 0x5e);
	mt2266_writereg(priv, 0x10, 0x10);
	mt2266_writereg(priv, 0x10, 0x00);
	mt2266_writeregs(priv, mt2266_init_8mhz, sizeof(mt2266_init_8mhz));
	msleep(25);
	mt2266_writereg(priv, 0x17, 0x6d);
	mt2266_writereg(priv, 0x1c, 0x00);
	msleep(75);
	mt2266_writereg(priv, 0x17, 0x6d);
	mt2266_writereg(priv, 0x1c, 0xff);
}

static int mt2266_get_frequency(struct dvb_frontend *fe, u32 *frequency)
{
	struct mt2266_priv *priv = fe->tuner_priv;
	*frequency = priv->frequency;
	return 0;
}

static int mt2266_get_bandwidth(struct dvb_frontend *fe, u32 *bandwidth)
{
	struct mt2266_priv *priv = fe->tuner_priv;
	*bandwidth = priv->bandwidth;
	return 0;
}

static int mt2266_init(struct dvb_frontend *fe)
{
	int ret;
	struct mt2266_priv *priv = fe->tuner_priv;
	ret = mt2266_writereg(priv, 0x17, 0x6d);
	if (ret < 0)
		return ret;
	ret = mt2266_writereg(priv, 0x1c, 0xff);
	if (ret < 0)
		return ret;
	return 0;
}

static int mt2266_sleep(struct dvb_frontend *fe)
{
	struct mt2266_priv *priv = fe->tuner_priv;
	mt2266_writereg(priv, 0x17, 0x6d);
	mt2266_writereg(priv, 0x1c, 0x00);
	return 0;
}

static void mt2266_release(struct dvb_frontend *fe)
{
	kfree(fe->tuner_priv);
	fe->tuner_priv = NULL;
}

static const struct dvb_tuner_ops mt2266_tuner_ops = {
	.info = {
		.name              = "Microtune MT2266",
		.frequency_min_hz  = 174 * MHz,
		.frequency_max_hz  = 862 * MHz,
		.frequency_step_hz =  50 * kHz,
	},
	.release       = mt2266_release,
	.init          = mt2266_init,
	.sleep         = mt2266_sleep,
	.set_params    = mt2266_set_params,
	.get_frequency = mt2266_get_frequency,
	.get_bandwidth = mt2266_get_bandwidth
};

struct dvb_frontend * mt2266_attach(struct dvb_frontend *fe, struct i2c_adapter *i2c, struct mt2266_config *cfg)
{
	struct mt2266_priv *priv = NULL;
	u8 id = 0;

	priv = kzalloc(sizeof(struct mt2266_priv), GFP_KERNEL);
	if (priv == NULL)
		return NULL;

	priv->cfg      = cfg;
	priv->i2c      = i2c;
	priv->band     = MT2266_UHF;

	if (mt2266_readreg(priv, 0, &id)) {
		kfree(priv);
		return NULL;
	}
	if (id != PART_REV) {
		kfree(priv);
		return NULL;
	}
	printk(KERN_INFO "MT2266: successfully identified\n");
	memcpy(&fe->ops.tuner_ops, &mt2266_tuner_ops, sizeof(struct dvb_tuner_ops));

	fe->tuner_priv = priv;
	mt2266_calibrate(priv);
	return fe;
}
EXPORT_SYMBOL(mt2266_attach);

MODULE_AUTHOR("Olivier DANET");
MODULE_DESCRIPTION("Microtune MT2266 silicon tuner driver");
MODULE_LICENSE("GPL");
