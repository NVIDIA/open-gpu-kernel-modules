// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 * (c) 2004 Gerd Knorr <kraxel@bytesex.org> [SuSE Labs]
 *
 *  Extended 3 / 2005 by Hartmut Hackmann to support various
 *  cards with the tda10046 DVB-T channel decoder
 */

#include "saa7134.h"
#include "saa7134-reg.h"

#include <linux/init.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/suspend.h>

#include <media/v4l2-common.h>
#include "dvb-pll.h"
#include <media/dvb_frontend.h>

#include "mt352.h"
#include "mt352_priv.h" /* FIXME */
#include "tda1004x.h"
#include "nxt200x.h"
#include "tuner-xc2028.h"
#include "xc5000.h"

#include "tda10086.h"
#include "tda826x.h"
#include "tda827x.h"
#include "isl6421.h"
#include "isl6405.h"
#include "lnbp21.h"
#include "tuner-simple.h"
#include "tda10048.h"
#include "tda18271.h"
#include "lgdt3305.h"
#include "tda8290.h"
#include "mb86a20s.h"
#include "lgs8gxx.h"

#include "zl10353.h"
#include "qt1010.h"

#include "zl10036.h"
#include "zl10039.h"
#include "mt312.h"
#include "s5h1411.h"

MODULE_AUTHOR("Gerd Knorr <kraxel@bytesex.org> [SuSE Labs]");
MODULE_LICENSE("GPL");

static unsigned int antenna_pwr;

module_param(antenna_pwr, int, 0444);
MODULE_PARM_DESC(antenna_pwr,"enable antenna power (Pinnacle 300i)");

static int use_frontend;
module_param(use_frontend, int, 0644);
MODULE_PARM_DESC(use_frontend,"for cards with multiple frontends (0: terrestrial, 1: satellite)");

DVB_DEFINE_MOD_OPT_ADAPTER_NR(adapter_nr);

/* ------------------------------------------------------------------
 * mt352 based DVB-T cards
 */

static int pinnacle_antenna_pwr(struct saa7134_dev *dev, int on)
{
	u32 ok;

	if (!on) {
		saa_setl(SAA7134_GPIO_GPMODE0 >> 2,     (1 << 26));
		saa_clearl(SAA7134_GPIO_GPSTATUS0 >> 2, (1 << 26));
		return 0;
	}

	saa_setl(SAA7134_GPIO_GPMODE0 >> 2,     (1 << 26));
	saa_setl(SAA7134_GPIO_GPSTATUS0 >> 2,   (1 << 26));
	udelay(10);

	saa_setl(SAA7134_GPIO_GPMODE0 >> 2,     (1 << 28));
	saa_clearl(SAA7134_GPIO_GPSTATUS0 >> 2, (1 << 28));
	udelay(10);
	saa_setl(SAA7134_GPIO_GPSTATUS0 >> 2,   (1 << 28));
	udelay(10);
	ok = saa_readl(SAA7134_GPIO_GPSTATUS0) & (1 << 27);
	pr_debug("%s %s\n", __func__, ok ? "on" : "off");

	if (!ok)
		saa_clearl(SAA7134_GPIO_GPSTATUS0 >> 2,   (1 << 26));
	return ok;
}

static int mt352_pinnacle_init(struct dvb_frontend* fe)
{
	static u8 clock_config []  = { CLOCK_CTL,  0x3d, 0x28 };
	static u8 reset []         = { RESET,      0x80 };
	static u8 adc_ctl_1_cfg [] = { ADC_CTL_1,  0x40 };
	static u8 agc_cfg []       = { AGC_TARGET, 0x28, 0xa0 };
	static u8 capt_range_cfg[] = { CAPT_RANGE, 0x31 };
	static u8 fsm_ctl_cfg[]    = { 0x7b,       0x04 };
	static u8 gpp_ctl_cfg []   = { GPP_CTL,    0x0f };
	static u8 scan_ctl_cfg []  = { SCAN_CTL,   0x0d };
	static u8 irq_cfg []       = { INTERRUPT_EN_0, 0x00, 0x00, 0x00, 0x00 };

	pr_debug("%s called\n", __func__);

	mt352_write(fe, clock_config,   sizeof(clock_config));
	udelay(200);
	mt352_write(fe, reset,          sizeof(reset));
	mt352_write(fe, adc_ctl_1_cfg,  sizeof(adc_ctl_1_cfg));
	mt352_write(fe, agc_cfg,        sizeof(agc_cfg));
	mt352_write(fe, capt_range_cfg, sizeof(capt_range_cfg));
	mt352_write(fe, gpp_ctl_cfg,    sizeof(gpp_ctl_cfg));

	mt352_write(fe, fsm_ctl_cfg,    sizeof(fsm_ctl_cfg));
	mt352_write(fe, scan_ctl_cfg,   sizeof(scan_ctl_cfg));
	mt352_write(fe, irq_cfg,        sizeof(irq_cfg));

	return 0;
}

static int mt352_aver777_init(struct dvb_frontend* fe)
{
	static u8 clock_config []  = { CLOCK_CTL,  0x38, 0x2d };
	static u8 reset []         = { RESET,      0x80 };
	static u8 adc_ctl_1_cfg [] = { ADC_CTL_1,  0x40 };
	static u8 agc_cfg []       = { AGC_TARGET, 0x28, 0xa0 };
	static u8 capt_range_cfg[] = { CAPT_RANGE, 0x33 };

	mt352_write(fe, clock_config,   sizeof(clock_config));
	udelay(200);
	mt352_write(fe, reset,          sizeof(reset));
	mt352_write(fe, adc_ctl_1_cfg,  sizeof(adc_ctl_1_cfg));
	mt352_write(fe, agc_cfg,        sizeof(agc_cfg));
	mt352_write(fe, capt_range_cfg, sizeof(capt_range_cfg));

	return 0;
}

static int mt352_avermedia_xc3028_init(struct dvb_frontend *fe)
{
	static u8 clock_config []  = { CLOCK_CTL, 0x38, 0x2d };
	static u8 reset []         = { RESET, 0x80 };
	static u8 adc_ctl_1_cfg [] = { ADC_CTL_1, 0x40 };
	static u8 agc_cfg []       = { AGC_TARGET, 0xe };
	static u8 capt_range_cfg[] = { CAPT_RANGE, 0x33 };

	mt352_write(fe, clock_config,   sizeof(clock_config));
	udelay(200);
	mt352_write(fe, reset,          sizeof(reset));
	mt352_write(fe, adc_ctl_1_cfg,  sizeof(adc_ctl_1_cfg));
	mt352_write(fe, agc_cfg,        sizeof(agc_cfg));
	mt352_write(fe, capt_range_cfg, sizeof(capt_range_cfg));
	return 0;
}

static int mt352_pinnacle_tuner_set_params(struct dvb_frontend *fe)
{
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	u8 off[] = { 0x00, 0xf1};
	u8 on[]  = { 0x00, 0x71};
	struct i2c_msg msg = {.addr=0x43, .flags=0, .buf=off, .len = sizeof(off)};

	struct saa7134_dev *dev = fe->dvb->priv;
	struct v4l2_frequency f;

	/* set frequency (mt2050) */
	f.tuner     = 0;
	f.type      = V4L2_TUNER_DIGITAL_TV;
	f.frequency = c->frequency / 1000 * 16 / 1000;
	if (fe->ops.i2c_gate_ctrl)
		fe->ops.i2c_gate_ctrl(fe, 1);
	i2c_transfer(&dev->i2c_adap, &msg, 1);
	saa_call_all(dev, tuner, s_frequency, &f);
	msg.buf = on;
	if (fe->ops.i2c_gate_ctrl)
		fe->ops.i2c_gate_ctrl(fe, 1);
	i2c_transfer(&dev->i2c_adap, &msg, 1);

	pinnacle_antenna_pwr(dev, antenna_pwr);

	/* mt352 setup */
	return mt352_pinnacle_init(fe);
}

static struct mt352_config pinnacle_300i = {
	.demod_address = 0x3c >> 1,
	.adc_clock     = 20333,
	.if2           = 36150,
	.no_tuner      = 1,
	.demod_init    = mt352_pinnacle_init,
};

static struct mt352_config avermedia_777 = {
	.demod_address = 0xf,
	.demod_init    = mt352_aver777_init,
};

static struct mt352_config avermedia_xc3028_mt352_dev = {
	.demod_address   = (0x1e >> 1),
	.no_tuner        = 1,
	.demod_init      = mt352_avermedia_xc3028_init,
};

static struct tda18271_std_map mb86a20s_tda18271_std_map = {
	.dvbt_6   = { .if_freq = 3300, .agc_mode = 3, .std = 4,
		      .if_lvl = 7, .rfagc_top = 0x37, },
};

static struct tda18271_config kworld_tda18271_config = {
	.std_map = &mb86a20s_tda18271_std_map,
	.gate    = TDA18271_GATE_DIGITAL,
	.config  = 3,	/* Use tuner callback for AGC */

};

static const struct mb86a20s_config kworld_mb86a20s_config = {
	.demod_address = 0x10,
};

static int kworld_sbtvd_gate_ctrl(struct dvb_frontend* fe, int enable)
{
	struct saa7134_dev *dev = fe->dvb->priv;

	unsigned char initmsg[] = {0x45, 0x97};
	unsigned char msg_enable[] = {0x45, 0xc1};
	unsigned char msg_disable[] = {0x45, 0x81};
	struct i2c_msg msg = {.addr = 0x4b, .flags = 0, .buf = initmsg, .len = 2};

	if (i2c_transfer(&dev->i2c_adap, &msg, 1) != 1) {
		pr_warn("could not access the I2C gate\n");
		return -EIO;
	}
	if (enable)
		msg.buf = msg_enable;
	else
		msg.buf = msg_disable;
	if (i2c_transfer(&dev->i2c_adap, &msg, 1) != 1) {
		pr_warn("could not access the I2C gate\n");
		return -EIO;
	}
	msleep(20);
	return 0;
}

/* ==================================================================
 * tda1004x based DVB-T cards, helper functions
 */

static int philips_tda1004x_request_firmware(struct dvb_frontend *fe,
					   const struct firmware **fw, char *name)
{
	struct saa7134_dev *dev = fe->dvb->priv;
	return request_firmware(fw, name, &dev->pci->dev);
}

/* ------------------------------------------------------------------
 * these tuners are tu1216, td1316(a)
 */

static int philips_tda6651_pll_set(struct dvb_frontend *fe)
{
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	struct saa7134_dev *dev = fe->dvb->priv;
	struct tda1004x_state *state = fe->demodulator_priv;
	u8 addr = state->config->tuner_address;
	u8 tuner_buf[4];
	struct i2c_msg tuner_msg = {.addr = addr,.flags = 0,.buf = tuner_buf,.len =
			sizeof(tuner_buf) };
	int tuner_frequency = 0;
	u8 band, cp, filter;

	/* determine charge pump */
	tuner_frequency = c->frequency + 36166000;
	if (tuner_frequency < 87000000)
		return -EINVAL;
	else if (tuner_frequency < 130000000)
		cp = 3;
	else if (tuner_frequency < 160000000)
		cp = 5;
	else if (tuner_frequency < 200000000)
		cp = 6;
	else if (tuner_frequency < 290000000)
		cp = 3;
	else if (tuner_frequency < 420000000)
		cp = 5;
	else if (tuner_frequency < 480000000)
		cp = 6;
	else if (tuner_frequency < 620000000)
		cp = 3;
	else if (tuner_frequency < 830000000)
		cp = 5;
	else if (tuner_frequency < 895000000)
		cp = 7;
	else
		return -EINVAL;

	/* determine band */
	if (c->frequency < 49000000)
		return -EINVAL;
	else if (c->frequency < 161000000)
		band = 1;
	else if (c->frequency < 444000000)
		band = 2;
	else if (c->frequency < 861000000)
		band = 4;
	else
		return -EINVAL;

	/* setup PLL filter */
	switch (c->bandwidth_hz) {
	case 6000000:
		filter = 0;
		break;

	case 7000000:
		filter = 0;
		break;

	case 8000000:
		filter = 1;
		break;

	default:
		return -EINVAL;
	}

	/* calculate divisor
	 * ((36166000+((1000000/6)/2)) + Finput)/(1000000/6)
	 */
	tuner_frequency = (((c->frequency / 1000) * 6) + 217496) / 1000;

	/* setup tuner buffer */
	tuner_buf[0] = (tuner_frequency >> 8) & 0x7f;
	tuner_buf[1] = tuner_frequency & 0xff;
	tuner_buf[2] = 0xca;
	tuner_buf[3] = (cp << 5) | (filter << 3) | band;

	if (fe->ops.i2c_gate_ctrl)
		fe->ops.i2c_gate_ctrl(fe, 1);
	if (i2c_transfer(&dev->i2c_adap, &tuner_msg, 1) != 1) {
		pr_warn("could not write to tuner at addr: 0x%02x\n",
			addr << 1);
		return -EIO;
	}
	msleep(1);
	return 0;
}

static int philips_tu1216_init(struct dvb_frontend *fe)
{
	struct saa7134_dev *dev = fe->dvb->priv;
	struct tda1004x_state *state = fe->demodulator_priv;
	u8 addr = state->config->tuner_address;
	static u8 tu1216_init[] = { 0x0b, 0xf5, 0x85, 0xab };
	struct i2c_msg tuner_msg = {.addr = addr,.flags = 0,.buf = tu1216_init,.len = sizeof(tu1216_init) };

	/* setup PLL configuration */
	if (fe->ops.i2c_gate_ctrl)
		fe->ops.i2c_gate_ctrl(fe, 1);
	if (i2c_transfer(&dev->i2c_adap, &tuner_msg, 1) != 1)
		return -EIO;
	msleep(1);

	return 0;
}

/* ------------------------------------------------------------------ */

static struct tda1004x_config philips_tu1216_60_config = {
	.demod_address = 0x8,
	.invert        = 1,
	.invert_oclk   = 0,
	.xtal_freq     = TDA10046_XTAL_4M,
	.agc_config    = TDA10046_AGC_DEFAULT,
	.if_freq       = TDA10046_FREQ_3617,
	.tuner_address = 0x60,
	.request_firmware = philips_tda1004x_request_firmware
};

static struct tda1004x_config philips_tu1216_61_config = {

	.demod_address = 0x8,
	.invert        = 1,
	.invert_oclk   = 0,
	.xtal_freq     = TDA10046_XTAL_4M,
	.agc_config    = TDA10046_AGC_DEFAULT,
	.if_freq       = TDA10046_FREQ_3617,
	.tuner_address = 0x61,
	.request_firmware = philips_tda1004x_request_firmware
};

/* ------------------------------------------------------------------ */

static int philips_td1316_tuner_init(struct dvb_frontend *fe)
{
	struct saa7134_dev *dev = fe->dvb->priv;
	struct tda1004x_state *state = fe->demodulator_priv;
	u8 addr = state->config->tuner_address;
	static u8 msg[] = { 0x0b, 0xf5, 0x86, 0xab };
	struct i2c_msg init_msg = {.addr = addr,.flags = 0,.buf = msg,.len = sizeof(msg) };

	/* setup PLL configuration */
	if (fe->ops.i2c_gate_ctrl)
		fe->ops.i2c_gate_ctrl(fe, 1);
	if (i2c_transfer(&dev->i2c_adap, &init_msg, 1) != 1)
		return -EIO;
	return 0;
}

static int philips_td1316_tuner_set_params(struct dvb_frontend *fe)
{
	return philips_tda6651_pll_set(fe);
}

static int philips_td1316_tuner_sleep(struct dvb_frontend *fe)
{
	struct saa7134_dev *dev = fe->dvb->priv;
	struct tda1004x_state *state = fe->demodulator_priv;
	u8 addr = state->config->tuner_address;
	static u8 msg[] = { 0x0b, 0xdc, 0x86, 0xa4 };
	struct i2c_msg analog_msg = {.addr = addr,.flags = 0,.buf = msg,.len = sizeof(msg) };

	/* switch the tuner to analog mode */
	if (fe->ops.i2c_gate_ctrl)
		fe->ops.i2c_gate_ctrl(fe, 1);
	if (i2c_transfer(&dev->i2c_adap, &analog_msg, 1) != 1)
		return -EIO;
	return 0;
}

/* ------------------------------------------------------------------ */

static int philips_europa_tuner_init(struct dvb_frontend *fe)
{
	struct saa7134_dev *dev = fe->dvb->priv;
	static u8 msg[] = { 0x00, 0x40};
	struct i2c_msg init_msg = {.addr = 0x43,.flags = 0,.buf = msg,.len = sizeof(msg) };


	if (philips_td1316_tuner_init(fe))
		return -EIO;
	msleep(1);
	if (i2c_transfer(&dev->i2c_adap, &init_msg, 1) != 1)
		return -EIO;

	return 0;
}

static int philips_europa_tuner_sleep(struct dvb_frontend *fe)
{
	struct saa7134_dev *dev = fe->dvb->priv;

	static u8 msg[] = { 0x00, 0x14 };
	struct i2c_msg analog_msg = {.addr = 0x43,.flags = 0,.buf = msg,.len = sizeof(msg) };

	if (philips_td1316_tuner_sleep(fe))
		return -EIO;

	/* switch the board to analog mode */
	if (fe->ops.i2c_gate_ctrl)
		fe->ops.i2c_gate_ctrl(fe, 1);
	i2c_transfer(&dev->i2c_adap, &analog_msg, 1);
	return 0;
}

static int philips_europa_demod_sleep(struct dvb_frontend *fe)
{
	struct saa7134_dev *dev = fe->dvb->priv;

	if (dev->original_demod_sleep)
		dev->original_demod_sleep(fe);
	fe->ops.i2c_gate_ctrl(fe, 1);
	return 0;
}

static struct tda1004x_config philips_europa_config = {

	.demod_address = 0x8,
	.invert        = 0,
	.invert_oclk   = 0,
	.xtal_freq     = TDA10046_XTAL_4M,
	.agc_config    = TDA10046_AGC_IFO_AUTO_POS,
	.if_freq       = TDA10046_FREQ_052,
	.tuner_address = 0x61,
	.request_firmware = philips_tda1004x_request_firmware
};

static struct tda1004x_config medion_cardbus = {
	.demod_address = 0x08,
	.invert        = 1,
	.invert_oclk   = 0,
	.xtal_freq     = TDA10046_XTAL_16M,
	.agc_config    = TDA10046_AGC_IFO_AUTO_NEG,
	.if_freq       = TDA10046_FREQ_3613,
	.tuner_address = 0x61,
	.request_firmware = philips_tda1004x_request_firmware
};

static struct tda1004x_config technotrend_budget_t3000_config = {
	.demod_address = 0x8,
	.invert        = 1,
	.invert_oclk   = 0,
	.xtal_freq     = TDA10046_XTAL_4M,
	.agc_config    = TDA10046_AGC_DEFAULT,
	.if_freq       = TDA10046_FREQ_3617,
	.tuner_address = 0x63,
	.request_firmware = philips_tda1004x_request_firmware
};

/* ------------------------------------------------------------------
 * tda 1004x based cards with philips silicon tuner
 */

static int tda8290_i2c_gate_ctrl( struct dvb_frontend* fe, int enable)
{
	struct tda1004x_state *state = fe->demodulator_priv;

	u8 addr = state->config->i2c_gate;
	static u8 tda8290_close[] = { 0x21, 0xc0};
	static u8 tda8290_open[]  = { 0x21, 0x80};
	struct i2c_msg tda8290_msg = {.addr = addr,.flags = 0, .len = 2};
	if (enable) {
		tda8290_msg.buf = tda8290_close;
	} else {
		tda8290_msg.buf = tda8290_open;
	}
	if (i2c_transfer(state->i2c, &tda8290_msg, 1) != 1) {
		pr_warn("could not access tda8290 I2C gate\n");
		return -EIO;
	}
	msleep(20);
	return 0;
}

static int philips_tda827x_tuner_init(struct dvb_frontend *fe)
{
	struct saa7134_dev *dev = fe->dvb->priv;
	struct tda1004x_state *state = fe->demodulator_priv;

	switch (state->config->antenna_switch) {
	case 0:
		break;
	case 1:
		pr_debug("setting GPIO21 to 0 (TV antenna?)\n");
		saa7134_set_gpio(dev, 21, 0);
		break;
	case 2:
		pr_debug("setting GPIO21 to 1 (Radio antenna?)\n");
		saa7134_set_gpio(dev, 21, 1);
		break;
	}
	return 0;
}

static int philips_tda827x_tuner_sleep(struct dvb_frontend *fe)
{
	struct saa7134_dev *dev = fe->dvb->priv;
	struct tda1004x_state *state = fe->demodulator_priv;

	switch (state->config->antenna_switch) {
	case 0:
		break;
	case 1:
		pr_debug("setting GPIO21 to 1 (Radio antenna?)\n");
		saa7134_set_gpio(dev, 21, 1);
		break;
	case 2:
		pr_debug("setting GPIO21 to 0 (TV antenna?)\n");
		saa7134_set_gpio(dev, 21, 0);
		break;
	}
	return 0;
}

static int configure_tda827x_fe(struct saa7134_dev *dev,
				struct tda1004x_config *cdec_conf,
				struct tda827x_config *tuner_conf)
{
	struct vb2_dvb_frontend *fe0;

	/* Get the first frontend */
	fe0 = vb2_dvb_get_frontend(&dev->frontends, 1);

	if (!fe0)
		return -EINVAL;

	fe0->dvb.frontend = dvb_attach(tda10046_attach, cdec_conf, &dev->i2c_adap);
	if (fe0->dvb.frontend) {
		if (cdec_conf->i2c_gate)
			fe0->dvb.frontend->ops.i2c_gate_ctrl = tda8290_i2c_gate_ctrl;
		if (dvb_attach(tda827x_attach, fe0->dvb.frontend,
			       cdec_conf->tuner_address,
			       &dev->i2c_adap, tuner_conf))
			return 0;

		pr_warn("no tda827x tuner found at addr: %02x\n",
				cdec_conf->tuner_address);
	}
	return -EINVAL;
}

/* ------------------------------------------------------------------ */

static struct tda827x_config tda827x_cfg_0 = {
	.init = philips_tda827x_tuner_init,
	.sleep = philips_tda827x_tuner_sleep,
	.config = 0,
	.switch_addr = 0
};

static struct tda827x_config tda827x_cfg_1 = {
	.init = philips_tda827x_tuner_init,
	.sleep = philips_tda827x_tuner_sleep,
	.config = 1,
	.switch_addr = 0x4b
};

static struct tda827x_config tda827x_cfg_2 = {
	.init = philips_tda827x_tuner_init,
	.sleep = philips_tda827x_tuner_sleep,
	.config = 2,
	.switch_addr = 0x4b
};

static struct tda827x_config tda827x_cfg_2_sw42 = {
	.init = philips_tda827x_tuner_init,
	.sleep = philips_tda827x_tuner_sleep,
	.config = 2,
	.switch_addr = 0x42
};

/* ------------------------------------------------------------------ */

static struct tda1004x_config tda827x_lifeview_config = {
	.demod_address = 0x08,
	.invert        = 1,
	.invert_oclk   = 0,
	.xtal_freq     = TDA10046_XTAL_16M,
	.agc_config    = TDA10046_AGC_TDA827X,
	.gpio_config   = TDA10046_GP11_I,
	.if_freq       = TDA10046_FREQ_045,
	.tuner_address = 0x60,
	.request_firmware = philips_tda1004x_request_firmware
};

static struct tda1004x_config philips_tiger_config = {
	.demod_address = 0x08,
	.invert        = 1,
	.invert_oclk   = 0,
	.xtal_freq     = TDA10046_XTAL_16M,
	.agc_config    = TDA10046_AGC_TDA827X,
	.gpio_config   = TDA10046_GP11_I,
	.if_freq       = TDA10046_FREQ_045,
	.i2c_gate      = 0x4b,
	.tuner_address = 0x61,
	.antenna_switch= 1,
	.request_firmware = philips_tda1004x_request_firmware
};

static struct tda1004x_config cinergy_ht_config = {
	.demod_address = 0x08,
	.invert        = 1,
	.invert_oclk   = 0,
	.xtal_freq     = TDA10046_XTAL_16M,
	.agc_config    = TDA10046_AGC_TDA827X,
	.gpio_config   = TDA10046_GP01_I,
	.if_freq       = TDA10046_FREQ_045,
	.i2c_gate      = 0x4b,
	.tuner_address = 0x61,
	.request_firmware = philips_tda1004x_request_firmware
};

static struct tda1004x_config cinergy_ht_pci_config = {
	.demod_address = 0x08,
	.invert        = 1,
	.invert_oclk   = 0,
	.xtal_freq     = TDA10046_XTAL_16M,
	.agc_config    = TDA10046_AGC_TDA827X,
	.gpio_config   = TDA10046_GP01_I,
	.if_freq       = TDA10046_FREQ_045,
	.i2c_gate      = 0x4b,
	.tuner_address = 0x60,
	.request_firmware = philips_tda1004x_request_firmware
};

static struct tda1004x_config philips_tiger_s_config = {
	.demod_address = 0x08,
	.invert        = 1,
	.invert_oclk   = 0,
	.xtal_freq     = TDA10046_XTAL_16M,
	.agc_config    = TDA10046_AGC_TDA827X,
	.gpio_config   = TDA10046_GP01_I,
	.if_freq       = TDA10046_FREQ_045,
	.i2c_gate      = 0x4b,
	.tuner_address = 0x61,
	.antenna_switch= 1,
	.request_firmware = philips_tda1004x_request_firmware
};

static struct tda1004x_config pinnacle_pctv_310i_config = {
	.demod_address = 0x08,
	.invert        = 1,
	.invert_oclk   = 0,
	.xtal_freq     = TDA10046_XTAL_16M,
	.agc_config    = TDA10046_AGC_TDA827X,
	.gpio_config   = TDA10046_GP11_I,
	.if_freq       = TDA10046_FREQ_045,
	.i2c_gate      = 0x4b,
	.tuner_address = 0x61,
	.request_firmware = philips_tda1004x_request_firmware
};

static struct tda1004x_config hauppauge_hvr_1110_config = {
	.demod_address = 0x08,
	.invert        = 1,
	.invert_oclk   = 0,
	.xtal_freq     = TDA10046_XTAL_16M,
	.agc_config    = TDA10046_AGC_TDA827X,
	.gpio_config   = TDA10046_GP11_I,
	.if_freq       = TDA10046_FREQ_045,
	.i2c_gate      = 0x4b,
	.tuner_address = 0x61,
	.request_firmware = philips_tda1004x_request_firmware
};

static struct tda1004x_config asus_p7131_dual_config = {
	.demod_address = 0x08,
	.invert        = 1,
	.invert_oclk   = 0,
	.xtal_freq     = TDA10046_XTAL_16M,
	.agc_config    = TDA10046_AGC_TDA827X,
	.gpio_config   = TDA10046_GP11_I,
	.if_freq       = TDA10046_FREQ_045,
	.i2c_gate      = 0x4b,
	.tuner_address = 0x61,
	.antenna_switch= 2,
	.request_firmware = philips_tda1004x_request_firmware
};

static struct tda1004x_config lifeview_trio_config = {
	.demod_address = 0x09,
	.invert        = 1,
	.invert_oclk   = 0,
	.xtal_freq     = TDA10046_XTAL_16M,
	.agc_config    = TDA10046_AGC_TDA827X,
	.gpio_config   = TDA10046_GP00_I,
	.if_freq       = TDA10046_FREQ_045,
	.tuner_address = 0x60,
	.request_firmware = philips_tda1004x_request_firmware
};

static struct tda1004x_config tevion_dvbt220rf_config = {
	.demod_address = 0x08,
	.invert        = 1,
	.invert_oclk   = 0,
	.xtal_freq     = TDA10046_XTAL_16M,
	.agc_config    = TDA10046_AGC_TDA827X,
	.gpio_config   = TDA10046_GP11_I,
	.if_freq       = TDA10046_FREQ_045,
	.tuner_address = 0x60,
	.request_firmware = philips_tda1004x_request_firmware
};

static struct tda1004x_config md8800_dvbt_config = {
	.demod_address = 0x08,
	.invert        = 1,
	.invert_oclk   = 0,
	.xtal_freq     = TDA10046_XTAL_16M,
	.agc_config    = TDA10046_AGC_TDA827X,
	.gpio_config   = TDA10046_GP01_I,
	.if_freq       = TDA10046_FREQ_045,
	.i2c_gate      = 0x4b,
	.tuner_address = 0x60,
	.request_firmware = philips_tda1004x_request_firmware
};

static struct tda1004x_config asus_p7131_4871_config = {
	.demod_address = 0x08,
	.invert        = 1,
	.invert_oclk   = 0,
	.xtal_freq     = TDA10046_XTAL_16M,
	.agc_config    = TDA10046_AGC_TDA827X,
	.gpio_config   = TDA10046_GP01_I,
	.if_freq       = TDA10046_FREQ_045,
	.i2c_gate      = 0x4b,
	.tuner_address = 0x61,
	.antenna_switch= 2,
	.request_firmware = philips_tda1004x_request_firmware
};

static struct tda1004x_config asus_p7131_hybrid_lna_config = {
	.demod_address = 0x08,
	.invert        = 1,
	.invert_oclk   = 0,
	.xtal_freq     = TDA10046_XTAL_16M,
	.agc_config    = TDA10046_AGC_TDA827X,
	.gpio_config   = TDA10046_GP11_I,
	.if_freq       = TDA10046_FREQ_045,
	.i2c_gate      = 0x4b,
	.tuner_address = 0x61,
	.antenna_switch= 2,
	.request_firmware = philips_tda1004x_request_firmware
};

static struct tda1004x_config kworld_dvb_t_210_config = {
	.demod_address = 0x08,
	.invert        = 1,
	.invert_oclk   = 0,
	.xtal_freq     = TDA10046_XTAL_16M,
	.agc_config    = TDA10046_AGC_TDA827X,
	.gpio_config   = TDA10046_GP11_I,
	.if_freq       = TDA10046_FREQ_045,
	.i2c_gate      = 0x4b,
	.tuner_address = 0x61,
	.antenna_switch= 1,
	.request_firmware = philips_tda1004x_request_firmware
};

static struct tda1004x_config avermedia_super_007_config = {
	.demod_address = 0x08,
	.invert        = 1,
	.invert_oclk   = 0,
	.xtal_freq     = TDA10046_XTAL_16M,
	.agc_config    = TDA10046_AGC_TDA827X,
	.gpio_config   = TDA10046_GP01_I,
	.if_freq       = TDA10046_FREQ_045,
	.i2c_gate      = 0x4b,
	.tuner_address = 0x60,
	.antenna_switch= 1,
	.request_firmware = philips_tda1004x_request_firmware
};

static struct tda1004x_config twinhan_dtv_dvb_3056_config = {
	.demod_address = 0x08,
	.invert        = 1,
	.invert_oclk   = 0,
	.xtal_freq     = TDA10046_XTAL_16M,
	.agc_config    = TDA10046_AGC_TDA827X,
	.gpio_config   = TDA10046_GP01_I,
	.if_freq       = TDA10046_FREQ_045,
	.i2c_gate      = 0x42,
	.tuner_address = 0x61,
	.antenna_switch = 1,
	.request_firmware = philips_tda1004x_request_firmware
};

static struct tda1004x_config asus_tiger_3in1_config = {
	.demod_address = 0x0b,
	.invert        = 1,
	.invert_oclk   = 0,
	.xtal_freq     = TDA10046_XTAL_16M,
	.agc_config    = TDA10046_AGC_TDA827X,
	.gpio_config   = TDA10046_GP11_I,
	.if_freq       = TDA10046_FREQ_045,
	.i2c_gate      = 0x4b,
	.tuner_address = 0x61,
	.antenna_switch = 1,
	.request_firmware = philips_tda1004x_request_firmware
};

static struct tda1004x_config asus_ps3_100_config = {
	.demod_address = 0x0b,
	.invert        = 1,
	.invert_oclk   = 0,
	.xtal_freq     = TDA10046_XTAL_16M,
	.agc_config    = TDA10046_AGC_TDA827X,
	.gpio_config   = TDA10046_GP11_I,
	.if_freq       = TDA10046_FREQ_045,
	.i2c_gate      = 0x4b,
	.tuner_address = 0x61,
	.antenna_switch = 1,
	.request_firmware = philips_tda1004x_request_firmware
};

/* ------------------------------------------------------------------
 * special case: this card uses saa713x GPIO22 for the mode switch
 */

static int ads_duo_tuner_init(struct dvb_frontend *fe)
{
	struct saa7134_dev *dev = fe->dvb->priv;
	philips_tda827x_tuner_init(fe);
	/* route TDA8275a AGC input to the channel decoder */
	saa7134_set_gpio(dev, 22, 1);
	return 0;
}

static int ads_duo_tuner_sleep(struct dvb_frontend *fe)
{
	struct saa7134_dev *dev = fe->dvb->priv;
	/* route TDA8275a AGC input to the analog IF chip*/
	saa7134_set_gpio(dev, 22, 0);
	philips_tda827x_tuner_sleep(fe);
	return 0;
}

static struct tda827x_config ads_duo_cfg = {
	.init = ads_duo_tuner_init,
	.sleep = ads_duo_tuner_sleep,
	.config = 0
};

static struct tda1004x_config ads_tech_duo_config = {
	.demod_address = 0x08,
	.invert        = 1,
	.invert_oclk   = 0,
	.xtal_freq     = TDA10046_XTAL_16M,
	.agc_config    = TDA10046_AGC_TDA827X,
	.gpio_config   = TDA10046_GP00_I,
	.if_freq       = TDA10046_FREQ_045,
	.tuner_address = 0x61,
	.request_firmware = philips_tda1004x_request_firmware
};

static struct zl10353_config behold_h6_config = {
	.demod_address = 0x1e>>1,
	.no_tuner      = 1,
	.parallel_ts   = 1,
	.disable_i2c_gate_ctrl = 1,
};

static struct xc5000_config behold_x7_tunerconfig = {
	.i2c_address      = 0xc2>>1,
	.if_khz           = 4560,
	.radio_input      = XC5000_RADIO_FM1,
};

static struct zl10353_config behold_x7_config = {
	.demod_address = 0x1e>>1,
	.if2           = 45600,
	.no_tuner      = 1,
	.parallel_ts   = 1,
	.disable_i2c_gate_ctrl = 1,
};

static struct zl10353_config videomate_t750_zl10353_config = {
	.demod_address         = 0x0f,
	.no_tuner              = 1,
	.parallel_ts           = 1,
	.disable_i2c_gate_ctrl = 1,
};

static struct qt1010_config videomate_t750_qt1010_config = {
	.i2c_address = 0x62
};


/* ==================================================================
 * tda10086 based DVB-S cards, helper functions
 */

static struct tda10086_config flydvbs = {
	.demod_address = 0x0e,
	.invert = 0,
	.diseqc_tone = 0,
	.xtal_freq = TDA10086_XTAL_16M,
};

static struct tda10086_config sd1878_4m = {
	.demod_address = 0x0e,
	.invert = 0,
	.diseqc_tone = 0,
	.xtal_freq = TDA10086_XTAL_4M,
};

/* ------------------------------------------------------------------
 * special case: lnb supply is connected to the gated i2c
 */

static int md8800_set_voltage(struct dvb_frontend *fe,
			      enum fe_sec_voltage voltage)
{
	int res = -EIO;
	struct saa7134_dev *dev = fe->dvb->priv;
	if (fe->ops.i2c_gate_ctrl) {
		fe->ops.i2c_gate_ctrl(fe, 1);
		if (dev->original_set_voltage)
			res = dev->original_set_voltage(fe, voltage);
		fe->ops.i2c_gate_ctrl(fe, 0);
	}
	return res;
};

static int md8800_set_high_voltage(struct dvb_frontend *fe, long arg)
{
	int res = -EIO;
	struct saa7134_dev *dev = fe->dvb->priv;
	if (fe->ops.i2c_gate_ctrl) {
		fe->ops.i2c_gate_ctrl(fe, 1);
		if (dev->original_set_high_voltage)
			res = dev->original_set_high_voltage(fe, arg);
		fe->ops.i2c_gate_ctrl(fe, 0);
	}
	return res;
};

static int md8800_set_voltage2(struct dvb_frontend *fe,
			       enum fe_sec_voltage voltage)
{
	struct saa7134_dev *dev = fe->dvb->priv;
	u8 wbuf[2] = { 0x1f, 00 };
	u8 rbuf;
	struct i2c_msg msg[] = { { .addr = 0x08, .flags = 0, .buf = wbuf, .len = 1 },
				 { .addr = 0x08, .flags = I2C_M_RD, .buf = &rbuf, .len = 1 } };

	if (i2c_transfer(&dev->i2c_adap, msg, 2) != 2)
		return -EIO;
	/* NOTE: this assumes that gpo1 is used, it might be bit 5 (gpo2) */
	if (voltage == SEC_VOLTAGE_18)
		wbuf[1] = rbuf | 0x10;
	else
		wbuf[1] = rbuf & 0xef;
	msg[0].len = 2;
	i2c_transfer(&dev->i2c_adap, msg, 1);
	return 0;
}

static int md8800_set_high_voltage2(struct dvb_frontend *fe, long arg)
{
	pr_warn("%s: sorry can't set high LNB supply voltage from here\n",
		__func__);
	return -EIO;
}

/* ==================================================================
 * nxt200x based ATSC cards, helper functions
 */

static const struct nxt200x_config avertvhda180 = {
	.demod_address    = 0x0a,
};

static const struct nxt200x_config kworldatsc110 = {
	.demod_address    = 0x0a,
};

/* ------------------------------------------------------------------ */

static struct mt312_config avertv_a700_mt312 = {
	.demod_address = 0x0e,
	.voltage_inverted = 1,
};

static struct zl10036_config avertv_a700_tuner = {
	.tuner_address = 0x60,
};

static struct mt312_config zl10313_compro_s350_config = {
	.demod_address = 0x0e,
};

static struct mt312_config zl10313_avermedia_a706_config = {
	.demod_address = 0x0e,
};

static struct lgdt3305_config hcw_lgdt3305_config = {
	.i2c_addr           = 0x0e,
	.mpeg_mode          = LGDT3305_MPEG_SERIAL,
	.tpclk_edge         = LGDT3305_TPCLK_RISING_EDGE,
	.tpvalid_polarity   = LGDT3305_TP_VALID_HIGH,
	.deny_i2c_rptr      = 1,
	.spectral_inversion = 1,
	.qam_if_khz         = 4000,
	.vsb_if_khz         = 3250,
};

static struct tda10048_config hcw_tda10048_config = {
	.demod_address    = 0x10 >> 1,
	.output_mode      = TDA10048_SERIAL_OUTPUT,
	.fwbulkwritelen   = TDA10048_BULKWRITE_200,
	.inversion        = TDA10048_INVERSION_ON,
	.dtv6_if_freq_khz = TDA10048_IF_3300,
	.dtv7_if_freq_khz = TDA10048_IF_3500,
	.dtv8_if_freq_khz = TDA10048_IF_4000,
	.clk_freq_khz     = TDA10048_CLK_16000,
	.disable_gate_access = 1,
};

static struct tda18271_std_map hauppauge_tda18271_std_map = {
	.atsc_6   = { .if_freq = 3250, .agc_mode = 3, .std = 4,
		      .if_lvl = 1, .rfagc_top = 0x58, },
	.qam_6    = { .if_freq = 4000, .agc_mode = 3, .std = 5,
		      .if_lvl = 1, .rfagc_top = 0x58, },
};

static struct tda18271_config hcw_tda18271_config = {
	.std_map = &hauppauge_tda18271_std_map,
	.gate    = TDA18271_GATE_ANALOG,
	.config  = 3,
	.output_opt = TDA18271_OUTPUT_LT_OFF,
};

static struct tda829x_config tda829x_no_probe = {
	.probe_tuner = TDA829X_DONT_PROBE,
};

static struct tda10048_config zolid_tda10048_config = {
	.demod_address    = 0x10 >> 1,
	.output_mode      = TDA10048_PARALLEL_OUTPUT,
	.fwbulkwritelen   = TDA10048_BULKWRITE_200,
	.inversion        = TDA10048_INVERSION_ON,
	.dtv6_if_freq_khz = TDA10048_IF_3300,
	.dtv7_if_freq_khz = TDA10048_IF_3500,
	.dtv8_if_freq_khz = TDA10048_IF_4000,
	.clk_freq_khz     = TDA10048_CLK_16000,
	.disable_gate_access = 1,
};

static struct tda18271_config zolid_tda18271_config = {
	.gate    = TDA18271_GATE_ANALOG,
};

static struct tda10048_config dtv1000s_tda10048_config = {
	.demod_address    = 0x10 >> 1,
	.output_mode      = TDA10048_PARALLEL_OUTPUT,
	.fwbulkwritelen   = TDA10048_BULKWRITE_200,
	.inversion        = TDA10048_INVERSION_ON,
	.dtv6_if_freq_khz = TDA10048_IF_3300,
	.dtv7_if_freq_khz = TDA10048_IF_3800,
	.dtv8_if_freq_khz = TDA10048_IF_4300,
	.clk_freq_khz     = TDA10048_CLK_16000,
	.disable_gate_access = 1,
};

static struct tda18271_std_map dtv1000s_tda18271_std_map = {
	.dvbt_6   = { .if_freq = 3300, .agc_mode = 3, .std = 4,
		      .if_lvl = 1, .rfagc_top = 0x37, },
	.dvbt_7   = { .if_freq = 3800, .agc_mode = 3, .std = 5,
		      .if_lvl = 1, .rfagc_top = 0x37, },
	.dvbt_8   = { .if_freq = 4300, .agc_mode = 3, .std = 6,
		      .if_lvl = 1, .rfagc_top = 0x37, },
};

static struct tda18271_config dtv1000s_tda18271_config = {
	.std_map = &dtv1000s_tda18271_std_map,
	.gate    = TDA18271_GATE_ANALOG,
};

static struct lgs8gxx_config prohdtv_pro2_lgs8g75_config = {
	.prod = LGS8GXX_PROD_LGS8G75,
	.demod_address = 0x1d,
	.serial_ts = 0,
	.ts_clk_pol = 1,
	.ts_clk_gated = 0,
	.if_clk_freq = 30400, /* 30.4 MHz */
	.if_freq = 4000, /* 4.00 MHz */
	.if_neg_center = 0,
	.ext_adc = 0,
	.adc_signed = 1,
	.adc_vpp = 3, /* 2.0 Vpp */
	.if_neg_edge = 1,
};

static struct tda18271_config prohdtv_pro2_tda18271_config = {
	.gate = TDA18271_GATE_ANALOG,
	.output_opt = TDA18271_OUTPUT_LT_OFF,
};

static struct tda18271_std_map kworld_tda18271_std_map = {
	.atsc_6   = { .if_freq = 3250, .agc_mode = 3, .std = 3,
		      .if_lvl = 6, .rfagc_top = 0x37 },
	.qam_6    = { .if_freq = 4000, .agc_mode = 3, .std = 0,
		      .if_lvl = 6, .rfagc_top = 0x37 },
};

static struct tda18271_config kworld_pc150u_tda18271_config = {
	.std_map = &kworld_tda18271_std_map,
	.gate    = TDA18271_GATE_ANALOG,
	.output_opt = TDA18271_OUTPUT_LT_OFF,
	.config  = 3,	/* Use tuner callback for AGC */
	.rf_cal_on_startup = 1
};

static struct s5h1411_config kworld_s5h1411_config = {
	.output_mode   = S5H1411_PARALLEL_OUTPUT,
	.gpio          = S5H1411_GPIO_OFF,
	.qam_if        = S5H1411_IF_4000,
	.vsb_if        = S5H1411_IF_3250,
	.inversion     = S5H1411_INVERSION_ON,
	.status_mode   = S5H1411_DEMODLOCKING,
	.mpeg_timing   =
		S5H1411_MPEGTIMING_CONTINUOUS_NONINVERTING_CLOCK,
};


/* ==================================================================
 * Core code
 */

static int dvb_init(struct saa7134_dev *dev)
{
	int ret;
	int attach_xc3028 = 0;
	struct vb2_dvb_frontend *fe0;
	struct vb2_queue *q;

	/* FIXME: add support for multi-frontend */
	mutex_init(&dev->frontends.lock);
	INIT_LIST_HEAD(&dev->frontends.felist);

	pr_info("%s() allocating 1 frontend\n", __func__);
	fe0 = vb2_dvb_alloc_frontend(&dev->frontends, 1);
	if (!fe0) {
		pr_err("%s() failed to alloc\n", __func__);
		return -ENOMEM;
	}

	/* init struct vb2_dvb */
	dev->ts.nr_bufs    = 32;
	dev->ts.nr_packets = 32*4;
	fe0->dvb.name = dev->name;
	q = &fe0->dvb.dvbq;
	q->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	q->io_modes = VB2_MMAP | VB2_READ;
	q->drv_priv = &dev->ts_q;
	q->ops = &saa7134_ts_qops;
	q->mem_ops = &vb2_dma_sg_memops;
	q->buf_struct_size = sizeof(struct saa7134_buf);
	q->timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	q->lock = &dev->lock;
	q->dev = &dev->pci->dev;
	ret = vb2_queue_init(q);
	if (ret) {
		vb2_dvb_dealloc_frontends(&dev->frontends);
		return ret;
	}

	switch (dev->board) {
	case SAA7134_BOARD_PINNACLE_300I_DVBT_PAL:
		pr_debug("pinnacle 300i dvb setup\n");
		fe0->dvb.frontend = dvb_attach(mt352_attach, &pinnacle_300i,
					       &dev->i2c_adap);
		if (fe0->dvb.frontend) {
			fe0->dvb.frontend->ops.tuner_ops.set_params = mt352_pinnacle_tuner_set_params;
		}
		break;
	case SAA7134_BOARD_AVERMEDIA_777:
	case SAA7134_BOARD_AVERMEDIA_A16AR:
		pr_debug("avertv 777 dvb setup\n");
		fe0->dvb.frontend = dvb_attach(mt352_attach, &avermedia_777,
					       &dev->i2c_adap);
		if (fe0->dvb.frontend) {
			dvb_attach(simple_tuner_attach, fe0->dvb.frontend,
				   &dev->i2c_adap, 0x61,
				   TUNER_PHILIPS_TD1316);
		}
		break;
	case SAA7134_BOARD_AVERMEDIA_A16D:
		pr_debug("AverMedia A16D dvb setup\n");
		fe0->dvb.frontend = dvb_attach(mt352_attach,
						&avermedia_xc3028_mt352_dev,
						&dev->i2c_adap);
		attach_xc3028 = 1;
		break;
	case SAA7134_BOARD_MD7134:
		fe0->dvb.frontend = dvb_attach(tda10046_attach,
					       &medion_cardbus,
					       &dev->i2c_adap);
		if (fe0->dvb.frontend) {
			/*
			 * The TV tuner on this board is actually NOT
			 * behind the demod i2c gate.
			 * However, the demod EEPROM is indeed there and it
			 * conflicts with the SAA7134 chip config EEPROM
			 * if the i2c gate is open (since they have same
			 * bus addresses) resulting in card PCI SVID / SSID
			 * being garbage after a reboot from time to time.
			 *
			 * Let's just leave the gate permanently closed -
			 * saa7134_i2c_eeprom_md7134_gate() will close it for
			 * us at probe time if it was open for some reason.
			 */
			fe0->dvb.frontend->ops.i2c_gate_ctrl = NULL;
			dvb_attach(simple_tuner_attach, fe0->dvb.frontend,
				   &dev->i2c_adap, medion_cardbus.tuner_address,
				   TUNER_PHILIPS_FMD1216ME_MK3);
		}
		break;
	case SAA7134_BOARD_PHILIPS_TOUGH:
		fe0->dvb.frontend = dvb_attach(tda10046_attach,
					       &philips_tu1216_60_config,
					       &dev->i2c_adap);
		if (fe0->dvb.frontend) {
			fe0->dvb.frontend->ops.tuner_ops.init = philips_tu1216_init;
			fe0->dvb.frontend->ops.tuner_ops.set_params = philips_tda6651_pll_set;
		}
		break;
	case SAA7134_BOARD_FLYDVBTDUO:
	case SAA7134_BOARD_FLYDVBT_DUO_CARDBUS:
		if (configure_tda827x_fe(dev, &tda827x_lifeview_config,
					 &tda827x_cfg_0) < 0)
			goto detach_frontend;
		break;
	case SAA7134_BOARD_PHILIPS_EUROPA:
	case SAA7134_BOARD_VIDEOMATE_DVBT_300:
	case SAA7134_BOARD_ASUS_EUROPA_HYBRID:
		fe0->dvb.frontend = dvb_attach(tda10046_attach,
					       &philips_europa_config,
					       &dev->i2c_adap);
		if (fe0->dvb.frontend) {
			dev->original_demod_sleep = fe0->dvb.frontend->ops.sleep;
			fe0->dvb.frontend->ops.sleep = philips_europa_demod_sleep;
			fe0->dvb.frontend->ops.tuner_ops.init = philips_europa_tuner_init;
			fe0->dvb.frontend->ops.tuner_ops.sleep = philips_europa_tuner_sleep;
			fe0->dvb.frontend->ops.tuner_ops.set_params = philips_td1316_tuner_set_params;
		}
		break;
	case SAA7134_BOARD_TECHNOTREND_BUDGET_T3000:
		fe0->dvb.frontend = dvb_attach(tda10046_attach,
					       &technotrend_budget_t3000_config,
					       &dev->i2c_adap);
		if (fe0->dvb.frontend) {
			dev->original_demod_sleep = fe0->dvb.frontend->ops.sleep;
			fe0->dvb.frontend->ops.sleep = philips_europa_demod_sleep;
			fe0->dvb.frontend->ops.tuner_ops.init = philips_europa_tuner_init;
			fe0->dvb.frontend->ops.tuner_ops.sleep = philips_europa_tuner_sleep;
			fe0->dvb.frontend->ops.tuner_ops.set_params = philips_td1316_tuner_set_params;
		}
		break;
	case SAA7134_BOARD_VIDEOMATE_DVBT_200:
		fe0->dvb.frontend = dvb_attach(tda10046_attach,
					       &philips_tu1216_61_config,
					       &dev->i2c_adap);
		if (fe0->dvb.frontend) {
			fe0->dvb.frontend->ops.tuner_ops.init = philips_tu1216_init;
			fe0->dvb.frontend->ops.tuner_ops.set_params = philips_tda6651_pll_set;
		}
		break;
	case SAA7134_BOARD_KWORLD_DVBT_210:
		if (configure_tda827x_fe(dev, &kworld_dvb_t_210_config,
					 &tda827x_cfg_2) < 0)
			goto detach_frontend;
		break;
	case SAA7134_BOARD_HAUPPAUGE_HVR1120:
		fe0->dvb.frontend = dvb_attach(tda10048_attach,
					       &hcw_tda10048_config,
					       &dev->i2c_adap);
		if (fe0->dvb.frontend != NULL) {
			dvb_attach(tda829x_attach, fe0->dvb.frontend,
				   &dev->i2c_adap, 0x4b,
				   &tda829x_no_probe);
			dvb_attach(tda18271_attach, fe0->dvb.frontend,
				   0x60, &dev->i2c_adap,
				   &hcw_tda18271_config);
		}
		break;
	case SAA7134_BOARD_PHILIPS_TIGER:
		if (configure_tda827x_fe(dev, &philips_tiger_config,
					 &tda827x_cfg_0) < 0)
			goto detach_frontend;
		break;
	case SAA7134_BOARD_PINNACLE_PCTV_310i:
		if (configure_tda827x_fe(dev, &pinnacle_pctv_310i_config,
					 &tda827x_cfg_1) < 0)
			goto detach_frontend;
		break;
	case SAA7134_BOARD_HAUPPAUGE_HVR1110:
		if (configure_tda827x_fe(dev, &hauppauge_hvr_1110_config,
					 &tda827x_cfg_1) < 0)
			goto detach_frontend;
		break;
	case SAA7134_BOARD_HAUPPAUGE_HVR1150:
		fe0->dvb.frontend = dvb_attach(lgdt3305_attach,
					       &hcw_lgdt3305_config,
					       &dev->i2c_adap);
		if (fe0->dvb.frontend) {
			dvb_attach(tda829x_attach, fe0->dvb.frontend,
				   &dev->i2c_adap, 0x4b,
				   &tda829x_no_probe);
			dvb_attach(tda18271_attach, fe0->dvb.frontend,
				   0x60, &dev->i2c_adap,
				   &hcw_tda18271_config);
		}
		break;
	case SAA7134_BOARD_ASUSTeK_P7131_DUAL:
		if (configure_tda827x_fe(dev, &asus_p7131_dual_config,
					 &tda827x_cfg_0) < 0)
			goto detach_frontend;
		break;
	case SAA7134_BOARD_FLYDVBT_LR301:
		if (configure_tda827x_fe(dev, &tda827x_lifeview_config,
					 &tda827x_cfg_0) < 0)
			goto detach_frontend;
		break;
	case SAA7134_BOARD_FLYDVB_TRIO:
		if (!use_frontend) {	/* terrestrial */
			if (configure_tda827x_fe(dev, &lifeview_trio_config,
						 &tda827x_cfg_0) < 0)
				goto detach_frontend;
		} else {		/* satellite */
			fe0->dvb.frontend = dvb_attach(tda10086_attach, &flydvbs, &dev->i2c_adap);
			if (fe0->dvb.frontend) {
				if (dvb_attach(tda826x_attach, fe0->dvb.frontend, 0x63,
									&dev->i2c_adap, 0) == NULL) {
					pr_warn("%s: Lifeview Trio, No tda826x found!\n",
						__func__);
					goto detach_frontend;
				}
				if (dvb_attach(isl6421_attach, fe0->dvb.frontend,
					       &dev->i2c_adap,
					       0x08, 0, 0, false) == NULL) {
					pr_warn("%s: Lifeview Trio, No ISL6421 found!\n",
						__func__);
					goto detach_frontend;
				}
			}
		}
		break;
	case SAA7134_BOARD_ADS_DUO_CARDBUS_PTV331:
	case SAA7134_BOARD_FLYDVBT_HYBRID_CARDBUS:
		fe0->dvb.frontend = dvb_attach(tda10046_attach,
					       &ads_tech_duo_config,
					       &dev->i2c_adap);
		if (fe0->dvb.frontend) {
			if (dvb_attach(tda827x_attach,fe0->dvb.frontend,
				   ads_tech_duo_config.tuner_address, &dev->i2c_adap,
								&ads_duo_cfg) == NULL) {
				pr_warn("no tda827x tuner found at addr: %02x\n",
					ads_tech_duo_config.tuner_address);
				goto detach_frontend;
			}
		} else
			pr_warn("failed to attach tda10046\n");
		break;
	case SAA7134_BOARD_TEVION_DVBT_220RF:
		if (configure_tda827x_fe(dev, &tevion_dvbt220rf_config,
					 &tda827x_cfg_0) < 0)
			goto detach_frontend;
		break;
	case SAA7134_BOARD_MEDION_MD8800_QUADRO:
		if (!use_frontend) {     /* terrestrial */
			if (configure_tda827x_fe(dev, &md8800_dvbt_config,
						 &tda827x_cfg_0) < 0)
				goto detach_frontend;
		} else {        /* satellite */
			fe0->dvb.frontend = dvb_attach(tda10086_attach,
							&flydvbs, &dev->i2c_adap);
			if (fe0->dvb.frontend) {
				struct dvb_frontend *fe = fe0->dvb.frontend;
				u8 dev_id = dev->eedata[2];
				u8 data = 0xc4;
				struct i2c_msg msg = {.addr = 0x08, .flags = 0, .len = 1};

				if (dvb_attach(tda826x_attach, fe0->dvb.frontend,
						0x60, &dev->i2c_adap, 0) == NULL) {
					pr_warn("%s: Medion Quadro, no tda826x found !\n",
						__func__);
					goto detach_frontend;
				}
				if (dev_id != 0x08) {
					/* we need to open the i2c gate (we know it exists) */
					fe->ops.i2c_gate_ctrl(fe, 1);
					if (dvb_attach(isl6405_attach, fe,
							&dev->i2c_adap, 0x08, 0, 0) == NULL) {
						pr_warn("%s: Medion Quadro, no ISL6405 found !\n",
							__func__);
						goto detach_frontend;
					}
					if (dev_id == 0x07) {
						/* fire up the 2nd section of the LNB supply since
						   we can't do this from the other section */
						msg.buf = &data;
						i2c_transfer(&dev->i2c_adap, &msg, 1);
					}
					fe->ops.i2c_gate_ctrl(fe, 0);
					dev->original_set_voltage = fe->ops.set_voltage;
					fe->ops.set_voltage = md8800_set_voltage;
					dev->original_set_high_voltage = fe->ops.enable_high_lnb_voltage;
					fe->ops.enable_high_lnb_voltage = md8800_set_high_voltage;
				} else {
					fe->ops.set_voltage = md8800_set_voltage2;
					fe->ops.enable_high_lnb_voltage = md8800_set_high_voltage2;
				}
			}
		}
		break;
	case SAA7134_BOARD_AVERMEDIA_AVERTVHD_A180:
		fe0->dvb.frontend = dvb_attach(nxt200x_attach, &avertvhda180,
					       &dev->i2c_adap);
		if (fe0->dvb.frontend)
			dvb_attach(dvb_pll_attach, fe0->dvb.frontend, 0x61,
				   NULL, DVB_PLL_TDHU2);
		break;
	case SAA7134_BOARD_ADS_INSTANT_HDTV_PCI:
	case SAA7134_BOARD_KWORLD_ATSC110:
		fe0->dvb.frontend = dvb_attach(nxt200x_attach, &kworldatsc110,
					       &dev->i2c_adap);
		if (fe0->dvb.frontend)
			dvb_attach(simple_tuner_attach, fe0->dvb.frontend,
				   &dev->i2c_adap, 0x61,
				   TUNER_PHILIPS_TUV1236D);
		break;
	case SAA7134_BOARD_KWORLD_PC150U:
		saa7134_set_gpio(dev, 18, 1); /* Switch to digital mode */
		saa7134_tuner_callback(dev, 0,
				       TDA18271_CALLBACK_CMD_AGC_ENABLE, 1);
		fe0->dvb.frontend = dvb_attach(s5h1411_attach,
					       &kworld_s5h1411_config,
					       &dev->i2c_adap);
		if (fe0->dvb.frontend != NULL) {
			dvb_attach(tda829x_attach, fe0->dvb.frontend,
				   &dev->i2c_adap, 0x4b,
				   &tda829x_no_probe);
			dvb_attach(tda18271_attach, fe0->dvb.frontend,
				   0x60, &dev->i2c_adap,
				   &kworld_pc150u_tda18271_config);
		}
		break;
	case SAA7134_BOARD_FLYDVBS_LR300:
		fe0->dvb.frontend = dvb_attach(tda10086_attach, &flydvbs,
					       &dev->i2c_adap);
		if (fe0->dvb.frontend) {
			if (dvb_attach(tda826x_attach, fe0->dvb.frontend, 0x60,
				       &dev->i2c_adap, 0) == NULL) {
				pr_warn("%s: No tda826x found!\n", __func__);
				goto detach_frontend;
			}
			if (dvb_attach(isl6421_attach, fe0->dvb.frontend,
				       &dev->i2c_adap,
				       0x08, 0, 0, false) == NULL) {
				pr_warn("%s: No ISL6421 found!\n", __func__);
				goto detach_frontend;
			}
		}
		break;
	case SAA7134_BOARD_ASUS_EUROPA2_HYBRID:
		fe0->dvb.frontend = dvb_attach(tda10046_attach,
					       &medion_cardbus,
					       &dev->i2c_adap);
		if (fe0->dvb.frontend) {
			dev->original_demod_sleep = fe0->dvb.frontend->ops.sleep;
			fe0->dvb.frontend->ops.sleep = philips_europa_demod_sleep;

			dvb_attach(simple_tuner_attach, fe0->dvb.frontend,
				   &dev->i2c_adap, medion_cardbus.tuner_address,
				   TUNER_PHILIPS_FMD1216ME_MK3);
		}
		break;
	case SAA7134_BOARD_VIDEOMATE_DVBT_200A:
		fe0->dvb.frontend = dvb_attach(tda10046_attach,
				&philips_europa_config,
				&dev->i2c_adap);
		if (fe0->dvb.frontend) {
			fe0->dvb.frontend->ops.tuner_ops.init = philips_td1316_tuner_init;
			fe0->dvb.frontend->ops.tuner_ops.set_params = philips_td1316_tuner_set_params;
		}
		break;
	case SAA7134_BOARD_CINERGY_HT_PCMCIA:
		if (configure_tda827x_fe(dev, &cinergy_ht_config,
					 &tda827x_cfg_0) < 0)
			goto detach_frontend;
		break;
	case SAA7134_BOARD_CINERGY_HT_PCI:
		if (configure_tda827x_fe(dev, &cinergy_ht_pci_config,
					 &tda827x_cfg_0) < 0)
			goto detach_frontend;
		break;
	case SAA7134_BOARD_PHILIPS_TIGER_S:
		if (configure_tda827x_fe(dev, &philips_tiger_s_config,
					 &tda827x_cfg_2) < 0)
			goto detach_frontend;
		break;
	case SAA7134_BOARD_ASUS_P7131_4871:
		if (configure_tda827x_fe(dev, &asus_p7131_4871_config,
					 &tda827x_cfg_2) < 0)
			goto detach_frontend;
		break;
	case SAA7134_BOARD_ASUSTeK_P7131_HYBRID_LNA:
		if (configure_tda827x_fe(dev, &asus_p7131_hybrid_lna_config,
					 &tda827x_cfg_2) < 0)
			goto detach_frontend;
		break;
	case SAA7134_BOARD_AVERMEDIA_SUPER_007:
		if (configure_tda827x_fe(dev, &avermedia_super_007_config,
					 &tda827x_cfg_0) < 0)
			goto detach_frontend;
		break;
	case SAA7134_BOARD_TWINHAN_DTV_DVB_3056:
		if (configure_tda827x_fe(dev, &twinhan_dtv_dvb_3056_config,
					 &tda827x_cfg_2_sw42) < 0)
			goto detach_frontend;
		break;
	case SAA7134_BOARD_PHILIPS_SNAKE:
		fe0->dvb.frontend = dvb_attach(tda10086_attach, &flydvbs,
						&dev->i2c_adap);
		if (fe0->dvb.frontend) {
			if (dvb_attach(tda826x_attach, fe0->dvb.frontend, 0x60,
					&dev->i2c_adap, 0) == NULL) {
				pr_warn("%s: No tda826x found!\n", __func__);
				goto detach_frontend;
			}
			if (dvb_attach(lnbp21_attach, fe0->dvb.frontend,
					&dev->i2c_adap, 0, 0) == NULL) {
				pr_warn("%s: No lnbp21 found!\n", __func__);
				goto detach_frontend;
			}
		}
		break;
	case SAA7134_BOARD_CREATIX_CTX953:
		if (configure_tda827x_fe(dev, &md8800_dvbt_config,
					 &tda827x_cfg_0) < 0)
			goto detach_frontend;
		break;
	case SAA7134_BOARD_MSI_TVANYWHERE_AD11:
		if (configure_tda827x_fe(dev, &philips_tiger_s_config,
					 &tda827x_cfg_2) < 0)
			goto detach_frontend;
		break;
	case SAA7134_BOARD_AVERMEDIA_CARDBUS_506:
		pr_debug("AverMedia E506R dvb setup\n");
		saa7134_set_gpio(dev, 25, 0);
		msleep(10);
		saa7134_set_gpio(dev, 25, 1);
		fe0->dvb.frontend = dvb_attach(mt352_attach,
						&avermedia_xc3028_mt352_dev,
						&dev->i2c_adap);
		attach_xc3028 = 1;
		break;
	case SAA7134_BOARD_MD7134_BRIDGE_2:
		fe0->dvb.frontend = dvb_attach(tda10086_attach,
						&sd1878_4m, &dev->i2c_adap);
		if (fe0->dvb.frontend) {
			struct dvb_frontend *fe;
			if (dvb_attach(dvb_pll_attach, fe0->dvb.frontend, 0x60,
				  &dev->i2c_adap, DVB_PLL_PHILIPS_SD1878_TDA8261) == NULL) {
				pr_warn("%s: MD7134 DVB-S, no SD1878 found !\n",
					__func__);
				goto detach_frontend;
			}
			/* we need to open the i2c gate (we know it exists) */
			fe = fe0->dvb.frontend;
			fe->ops.i2c_gate_ctrl(fe, 1);
			if (dvb_attach(isl6405_attach, fe,
					&dev->i2c_adap, 0x08, 0, 0) == NULL) {
				pr_warn("%s: MD7134 DVB-S, no ISL6405 found !\n",
					__func__);
				goto detach_frontend;
			}
			fe->ops.i2c_gate_ctrl(fe, 0);
			dev->original_set_voltage = fe->ops.set_voltage;
			fe->ops.set_voltage = md8800_set_voltage;
			dev->original_set_high_voltage = fe->ops.enable_high_lnb_voltage;
			fe->ops.enable_high_lnb_voltage = md8800_set_high_voltage;
		}
		break;
	case SAA7134_BOARD_AVERMEDIA_M103:
		saa7134_set_gpio(dev, 25, 0);
		msleep(10);
		saa7134_set_gpio(dev, 25, 1);
		fe0->dvb.frontend = dvb_attach(mt352_attach,
						&avermedia_xc3028_mt352_dev,
						&dev->i2c_adap);
		attach_xc3028 = 1;
		break;
	case SAA7134_BOARD_ASUSTeK_TIGER_3IN1:
		if (!use_frontend) {     /* terrestrial */
			if (configure_tda827x_fe(dev, &asus_tiger_3in1_config,
							&tda827x_cfg_2) < 0)
				goto detach_frontend;
		} else {		/* satellite */
			fe0->dvb.frontend = dvb_attach(tda10086_attach,
						&flydvbs, &dev->i2c_adap);
			if (fe0->dvb.frontend) {
				if (dvb_attach(tda826x_attach,
						fe0->dvb.frontend, 0x60,
						&dev->i2c_adap, 0) == NULL) {
					pr_warn("%s: Asus Tiger 3in1, no tda826x found!\n",
						__func__);
					goto detach_frontend;
				}
				if (dvb_attach(lnbp21_attach, fe0->dvb.frontend,
						&dev->i2c_adap, 0, 0) == NULL) {
					pr_warn("%s: Asus Tiger 3in1, no lnbp21 found!\n",
						__func__);
					goto detach_frontend;
			       }
		       }
	       }
	       break;
	case SAA7134_BOARD_ASUSTeK_PS3_100:
		if (!use_frontend) {     /* terrestrial */
			if (configure_tda827x_fe(dev, &asus_ps3_100_config,
						 &tda827x_cfg_2) < 0)
				goto detach_frontend;
	       } else {                /* satellite */
			fe0->dvb.frontend = dvb_attach(tda10086_attach,
						       &flydvbs, &dev->i2c_adap);
			if (fe0->dvb.frontend) {
				if (dvb_attach(tda826x_attach,
					       fe0->dvb.frontend, 0x60,
					       &dev->i2c_adap, 0) == NULL) {
					pr_warn("%s: Asus My Cinema PS3-100, no tda826x found!\n",
						__func__);
					goto detach_frontend;
				}
				if (dvb_attach(lnbp21_attach, fe0->dvb.frontend,
					       &dev->i2c_adap, 0, 0) == NULL) {
					pr_warn("%s: Asus My Cinema PS3-100, no lnbp21 found!\n",
						__func__);
					goto detach_frontend;
				}
			}
		}
		break;
	case SAA7134_BOARD_ASUSTeK_TIGER:
		if (configure_tda827x_fe(dev, &philips_tiger_config,
					 &tda827x_cfg_0) < 0)
			goto detach_frontend;
		break;
	case SAA7134_BOARD_BEHOLD_H6:
		fe0->dvb.frontend = dvb_attach(zl10353_attach,
						&behold_h6_config,
						&dev->i2c_adap);
		if (fe0->dvb.frontend) {
			dvb_attach(simple_tuner_attach, fe0->dvb.frontend,
				   &dev->i2c_adap, 0x61,
				   TUNER_PHILIPS_FMD1216MEX_MK3);
		}
		break;
	case SAA7134_BOARD_BEHOLD_X7:
		fe0->dvb.frontend = dvb_attach(zl10353_attach,
						&behold_x7_config,
						&dev->i2c_adap);
		if (fe0->dvb.frontend) {
			dvb_attach(xc5000_attach, fe0->dvb.frontend,
				   &dev->i2c_adap, &behold_x7_tunerconfig);
		}
		break;
	case SAA7134_BOARD_BEHOLD_H7:
		fe0->dvb.frontend = dvb_attach(zl10353_attach,
						&behold_x7_config,
						&dev->i2c_adap);
		if (fe0->dvb.frontend) {
			dvb_attach(xc5000_attach, fe0->dvb.frontend,
				   &dev->i2c_adap, &behold_x7_tunerconfig);
		}
		break;
	case SAA7134_BOARD_AVERMEDIA_A700_PRO:
	case SAA7134_BOARD_AVERMEDIA_A700_HYBRID:
		/* Zarlink ZL10313 */
		fe0->dvb.frontend = dvb_attach(mt312_attach,
			&avertv_a700_mt312, &dev->i2c_adap);
		if (fe0->dvb.frontend) {
			if (dvb_attach(zl10036_attach, fe0->dvb.frontend,
					&avertv_a700_tuner, &dev->i2c_adap) == NULL) {
				pr_warn("%s: No zl10036 found!\n",
					__func__);
			}
		}
		break;
	case SAA7134_BOARD_VIDEOMATE_S350:
		fe0->dvb.frontend = dvb_attach(mt312_attach,
				&zl10313_compro_s350_config, &dev->i2c_adap);
		if (fe0->dvb.frontend)
			if (dvb_attach(zl10039_attach, fe0->dvb.frontend,
					0x60, &dev->i2c_adap) == NULL)
				pr_warn("%s: No zl10039 found!\n",
					__func__);

		break;
	case SAA7134_BOARD_VIDEOMATE_T750:
		fe0->dvb.frontend = dvb_attach(zl10353_attach,
						&videomate_t750_zl10353_config,
						&dev->i2c_adap);
		if (fe0->dvb.frontend != NULL) {
			if (dvb_attach(qt1010_attach,
					fe0->dvb.frontend,
					&dev->i2c_adap,
					&videomate_t750_qt1010_config) == NULL)
				pr_warn("error attaching QT1010\n");
		}
		break;
	case SAA7134_BOARD_ZOLID_HYBRID_PCI:
		fe0->dvb.frontend = dvb_attach(tda10048_attach,
					       &zolid_tda10048_config,
					       &dev->i2c_adap);
		if (fe0->dvb.frontend != NULL) {
			dvb_attach(tda829x_attach, fe0->dvb.frontend,
				   &dev->i2c_adap, 0x4b,
				   &tda829x_no_probe);
			dvb_attach(tda18271_attach, fe0->dvb.frontend,
				   0x60, &dev->i2c_adap,
				   &zolid_tda18271_config);
		}
		break;
	case SAA7134_BOARD_LEADTEK_WINFAST_DTV1000S:
		fe0->dvb.frontend = dvb_attach(tda10048_attach,
					       &dtv1000s_tda10048_config,
					       &dev->i2c_adap);
		if (fe0->dvb.frontend != NULL) {
			dvb_attach(tda829x_attach, fe0->dvb.frontend,
				   &dev->i2c_adap, 0x4b,
				   &tda829x_no_probe);
			dvb_attach(tda18271_attach, fe0->dvb.frontend,
				   0x60, &dev->i2c_adap,
				   &dtv1000s_tda18271_config);
		}
		break;
	case SAA7134_BOARD_KWORLD_PCI_SBTVD_FULLSEG:
		/* Switch to digital mode */
		saa7134_tuner_callback(dev, 0,
				       TDA18271_CALLBACK_CMD_AGC_ENABLE, 1);
		fe0->dvb.frontend = dvb_attach(mb86a20s_attach,
					       &kworld_mb86a20s_config,
					       &dev->i2c_adap);
		if (fe0->dvb.frontend != NULL) {
			dvb_attach(tda829x_attach, fe0->dvb.frontend,
				   &dev->i2c_adap, 0x4b,
				   &tda829x_no_probe);
			fe0->dvb.frontend->ops.i2c_gate_ctrl = kworld_sbtvd_gate_ctrl;
			dvb_attach(tda18271_attach, fe0->dvb.frontend,
				   0x60, &dev->i2c_adap,
				   &kworld_tda18271_config);
		}

		/* mb86a20s need to use the I2C gateway */
		break;
	case SAA7134_BOARD_MAGICPRO_PROHDTV_PRO2:
		fe0->dvb.frontend = dvb_attach(lgs8gxx_attach,
					       &prohdtv_pro2_lgs8g75_config,
					       &dev->i2c_adap);
		if (fe0->dvb.frontend != NULL) {
			dvb_attach(tda829x_attach, fe0->dvb.frontend,
				   &dev->i2c_adap, 0x4b,
				   &tda829x_no_probe);
			dvb_attach(tda18271_attach, fe0->dvb.frontend,
				   0x60, &dev->i2c_adap,
				   &prohdtv_pro2_tda18271_config);
		}
		break;
	case SAA7134_BOARD_AVERMEDIA_A706:
		/* Enable all DVB-S devices now */
		/* CE5039 DVB-S tuner SLEEP pin low */
		saa7134_set_gpio(dev, 23, 0);
		/* CE6313 DVB-S demod SLEEP pin low */
		saa7134_set_gpio(dev, 9, 0);
		/* CE6313 DVB-S demod RESET# pin high */
		saa7134_set_gpio(dev, 25, 1);
		msleep(1);
		fe0->dvb.frontend = dvb_attach(mt312_attach,
				&zl10313_avermedia_a706_config, &dev->i2c_adap);
		if (fe0->dvb.frontend) {
			fe0->dvb.frontend->ops.i2c_gate_ctrl = NULL;
			if (dvb_attach(zl10039_attach, fe0->dvb.frontend,
					0x60, &dev->i2c_adap) == NULL)
				pr_warn("%s: No zl10039 found!\n",
					__func__);
		}
		break;
	default:
		pr_warn("Huh? unknown DVB card?\n");
		break;
	}

	if (attach_xc3028) {
		struct dvb_frontend *fe;
		struct xc2028_config cfg = {
			.i2c_adap  = &dev->i2c_adap,
			.i2c_addr  = 0x61,
		};

		if (!fe0->dvb.frontend)
			goto detach_frontend;

		fe = dvb_attach(xc2028_attach, fe0->dvb.frontend, &cfg);
		if (!fe) {
			pr_err("%s/2: xc3028 attach failed\n",
			       dev->name);
			goto detach_frontend;
		}
	}

	if (NULL == fe0->dvb.frontend) {
		pr_err("%s/dvb: frontend initialization failed\n", dev->name);
		goto detach_frontend;
	}
	/* define general-purpose callback pointer */
	fe0->dvb.frontend->callback = saa7134_tuner_callback;

	/* register everything else */
#ifndef CONFIG_MEDIA_CONTROLLER_DVB
	ret = vb2_dvb_register_bus(&dev->frontends, THIS_MODULE, dev,
				   &dev->pci->dev, NULL,
				   adapter_nr, 0);
#else
	ret = vb2_dvb_register_bus(&dev->frontends, THIS_MODULE, dev,
				   &dev->pci->dev, dev->media_dev,
				   adapter_nr, 0);
#endif

	/* this sequence is necessary to make the tda1004x load its firmware
	 * and to enter analog mode of hybrid boards
	 */
	if (!ret) {
		if (fe0->dvb.frontend->ops.init)
			fe0->dvb.frontend->ops.init(fe0->dvb.frontend);
		if (fe0->dvb.frontend->ops.sleep)
			fe0->dvb.frontend->ops.sleep(fe0->dvb.frontend);
		if (fe0->dvb.frontend->ops.tuner_ops.sleep)
			fe0->dvb.frontend->ops.tuner_ops.sleep(fe0->dvb.frontend);
	}
	return ret;

detach_frontend:
	vb2_dvb_dealloc_frontends(&dev->frontends);
	vb2_queue_release(&fe0->dvb.dvbq);
	return -EINVAL;
}

static int dvb_fini(struct saa7134_dev *dev)
{
	struct vb2_dvb_frontend *fe0;

	/* Get the first frontend */
	fe0 = vb2_dvb_get_frontend(&dev->frontends, 1);
	if (!fe0)
		return -EINVAL;

	/* FIXME: I suspect that this code is bogus, since the entry for
	   Pinnacle 300I DVB-T PAL already defines the proper init to allow
	   the detection of mt2032 (TDA9887_PORT2_INACTIVE)
	 */
	if (dev->board == SAA7134_BOARD_PINNACLE_300I_DVBT_PAL) {
		struct v4l2_priv_tun_config tda9887_cfg;
		static int on  = TDA9887_PRESENT | TDA9887_PORT2_INACTIVE;

		tda9887_cfg.tuner = TUNER_TDA9887;
		tda9887_cfg.priv  = &on;

		/* otherwise we don't detect the tuner on next insmod */
		saa_call_all(dev, tuner, s_config, &tda9887_cfg);
	} else if (dev->board == SAA7134_BOARD_MEDION_MD8800_QUADRO) {
		if ((dev->eedata[2] == 0x07) && use_frontend) {
			/* turn off the 2nd lnb supply */
			u8 data = 0x80;
			struct i2c_msg msg = {.addr = 0x08, .buf = &data, .flags = 0, .len = 1};
			struct dvb_frontend *fe;
			fe = fe0->dvb.frontend;
			if (fe->ops.i2c_gate_ctrl) {
				fe->ops.i2c_gate_ctrl(fe, 1);
				i2c_transfer(&dev->i2c_adap, &msg, 1);
				fe->ops.i2c_gate_ctrl(fe, 0);
			}
		}
	}
	vb2_dvb_unregister_bus(&dev->frontends);
	vb2_queue_release(&fe0->dvb.dvbq);
	return 0;
}

static struct saa7134_mpeg_ops dvb_ops = {
	.type          = SAA7134_MPEG_DVB,
	.init          = dvb_init,
	.fini          = dvb_fini,
};

static int __init dvb_register(void)
{
	return saa7134_ts_register(&dvb_ops);
}

static void __exit dvb_unregister(void)
{
	saa7134_ts_unregister(&dvb_ops);
}

module_init(dvb_register);
module_exit(dvb_unregister);
