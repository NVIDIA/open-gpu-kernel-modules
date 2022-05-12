// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * budget-av.c: driver for the SAA7146 based Budget DVB cards
 *              with analog video in
 *
 * Compiled from various sources by Michael Hunold <michael@mihu.de>
 *
 * CI interface support (c) 2004 Olivier Gournet <ogournet@anevia.com> &
 *                               Andrew de Quincey <adq_dvb@lidskialf.net>
 *
 * Copyright (C) 2002 Ralph Metzler <rjkm@metzlerbros.de>
 *
 * Copyright (C) 1999-2002 Ralph  Metzler
 *                       & Marcus Metzler for convergence integrated media GmbH
 *
 * the project's page is at https://linuxtv.org
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include "budget.h"
#include "stv0299.h"
#include "stb0899_drv.h"
#include "stb0899_reg.h"
#include "stb0899_cfg.h"
#include "tda8261.h"
#include "tda8261_cfg.h"
#include "tda1002x.h"
#include "tda1004x.h"
#include "tua6100.h"
#include "dvb-pll.h"
#include <media/drv-intf/saa7146_vv.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/input.h>
#include <linux/spinlock.h>

#include <media/dvb_ca_en50221.h>

#define DEBICICAM		0x02420000

#define SLOTSTATUS_NONE         1
#define SLOTSTATUS_PRESENT      2
#define SLOTSTATUS_RESET        4
#define SLOTSTATUS_READY        8
#define SLOTSTATUS_OCCUPIED     (SLOTSTATUS_PRESENT|SLOTSTATUS_RESET|SLOTSTATUS_READY)

DVB_DEFINE_MOD_OPT_ADAPTER_NR(adapter_nr);

struct budget_av {
	struct budget budget;
	struct video_device vd;
	int cur_input;
	int has_saa7113;
	struct tasklet_struct ciintf_irq_tasklet;
	int slot_status;
	struct dvb_ca_en50221 ca;
	u8 reinitialise_demod:1;
};

static int ciintf_slot_shutdown(struct dvb_ca_en50221 *ca, int slot);


/* GPIO Connections:
 * 0 - Vcc/Reset (Reset is controlled by capacitor). Resets the frontend *AS WELL*!
 * 1 - CI memory select 0=>IO memory, 1=>Attribute Memory
 * 2 - CI Card Enable (Active Low)
 * 3 - CI Card Detect
 */

/****************************************************************************
 * INITIALIZATION
 ****************************************************************************/

static u8 i2c_readreg(struct i2c_adapter *i2c, u8 id, u8 reg)
{
	u8 mm1[] = { 0x00 };
	u8 mm2[] = { 0x00 };
	struct i2c_msg msgs[2];

	msgs[0].flags = 0;
	msgs[1].flags = I2C_M_RD;
	msgs[0].addr = msgs[1].addr = id / 2;
	mm1[0] = reg;
	msgs[0].len = 1;
	msgs[1].len = 1;
	msgs[0].buf = mm1;
	msgs[1].buf = mm2;

	i2c_transfer(i2c, msgs, 2);

	return mm2[0];
}

static int i2c_readregs(struct i2c_adapter *i2c, u8 id, u8 reg, u8 * buf, u8 len)
{
	u8 mm1[] = { reg };
	struct i2c_msg msgs[2] = {
		{.addr = id / 2,.flags = 0,.buf = mm1,.len = 1},
		{.addr = id / 2,.flags = I2C_M_RD,.buf = buf,.len = len}
	};

	if (i2c_transfer(i2c, msgs, 2) != 2)
		return -EIO;

	return 0;
}

static int i2c_writereg(struct i2c_adapter *i2c, u8 id, u8 reg, u8 val)
{
	u8 msg[2] = { reg, val };
	struct i2c_msg msgs;

	msgs.flags = 0;
	msgs.addr = id / 2;
	msgs.len = 2;
	msgs.buf = msg;
	return i2c_transfer(i2c, &msgs, 1);
}

static int ciintf_read_attribute_mem(struct dvb_ca_en50221 *ca, int slot, int address)
{
	struct budget_av *budget_av = (struct budget_av *) ca->data;
	int result;

	if (slot != 0)
		return -EINVAL;

	saa7146_setgpio(budget_av->budget.dev, 1, SAA7146_GPIO_OUTHI);
	udelay(1);

	result = ttpci_budget_debiread(&budget_av->budget, DEBICICAM, address & 0xfff, 1, 0, 1);
	if (result == -ETIMEDOUT) {
		ciintf_slot_shutdown(ca, slot);
		pr_info("cam ejected 1\n");
	}
	return result;
}

static int ciintf_write_attribute_mem(struct dvb_ca_en50221 *ca, int slot, int address, u8 value)
{
	struct budget_av *budget_av = (struct budget_av *) ca->data;
	int result;

	if (slot != 0)
		return -EINVAL;

	saa7146_setgpio(budget_av->budget.dev, 1, SAA7146_GPIO_OUTHI);
	udelay(1);

	result = ttpci_budget_debiwrite(&budget_av->budget, DEBICICAM, address & 0xfff, 1, value, 0, 1);
	if (result == -ETIMEDOUT) {
		ciintf_slot_shutdown(ca, slot);
		pr_info("cam ejected 2\n");
	}
	return result;
}

static int ciintf_read_cam_control(struct dvb_ca_en50221 *ca, int slot, u8 address)
{
	struct budget_av *budget_av = (struct budget_av *) ca->data;
	int result;

	if (slot != 0)
		return -EINVAL;

	saa7146_setgpio(budget_av->budget.dev, 1, SAA7146_GPIO_OUTLO);
	udelay(1);

	result = ttpci_budget_debiread(&budget_av->budget, DEBICICAM, address & 3, 1, 0, 0);
	if (result == -ETIMEDOUT) {
		ciintf_slot_shutdown(ca, slot);
		pr_info("cam ejected 3\n");
		return -ETIMEDOUT;
	}
	return result;
}

static int ciintf_write_cam_control(struct dvb_ca_en50221 *ca, int slot, u8 address, u8 value)
{
	struct budget_av *budget_av = (struct budget_av *) ca->data;
	int result;

	if (slot != 0)
		return -EINVAL;

	saa7146_setgpio(budget_av->budget.dev, 1, SAA7146_GPIO_OUTLO);
	udelay(1);

	result = ttpci_budget_debiwrite(&budget_av->budget, DEBICICAM, address & 3, 1, value, 0, 0);
	if (result == -ETIMEDOUT) {
		ciintf_slot_shutdown(ca, slot);
		pr_info("cam ejected 5\n");
	}
	return result;
}

static int ciintf_slot_reset(struct dvb_ca_en50221 *ca, int slot)
{
	struct budget_av *budget_av = (struct budget_av *) ca->data;
	struct saa7146_dev *saa = budget_av->budget.dev;

	if (slot != 0)
		return -EINVAL;

	dprintk(1, "ciintf_slot_reset\n");
	budget_av->slot_status = SLOTSTATUS_RESET;

	saa7146_setgpio(saa, 2, SAA7146_GPIO_OUTHI); /* disable card */

	saa7146_setgpio(saa, 0, SAA7146_GPIO_OUTHI); /* Vcc off */
	msleep(2);
	saa7146_setgpio(saa, 0, SAA7146_GPIO_OUTLO); /* Vcc on */
	msleep(20); /* 20 ms Vcc settling time */

	saa7146_setgpio(saa, 2, SAA7146_GPIO_OUTLO); /* enable card */
	ttpci_budget_set_video_port(saa, BUDGET_VIDEO_PORTB);
	msleep(20);

	/* reinitialise the frontend if necessary */
	if (budget_av->reinitialise_demod)
		dvb_frontend_reinitialise(budget_av->budget.dvb_frontend);

	return 0;
}

static int ciintf_slot_shutdown(struct dvb_ca_en50221 *ca, int slot)
{
	struct budget_av *budget_av = (struct budget_av *) ca->data;
	struct saa7146_dev *saa = budget_av->budget.dev;

	if (slot != 0)
		return -EINVAL;

	dprintk(1, "ciintf_slot_shutdown\n");

	ttpci_budget_set_video_port(saa, BUDGET_VIDEO_PORTB);
	budget_av->slot_status = SLOTSTATUS_NONE;

	return 0;
}

static int ciintf_slot_ts_enable(struct dvb_ca_en50221 *ca, int slot)
{
	struct budget_av *budget_av = (struct budget_av *) ca->data;
	struct saa7146_dev *saa = budget_av->budget.dev;

	if (slot != 0)
		return -EINVAL;

	dprintk(1, "ciintf_slot_ts_enable: %d\n", budget_av->slot_status);

	ttpci_budget_set_video_port(saa, BUDGET_VIDEO_PORTA);

	return 0;
}

static int ciintf_poll_slot_status(struct dvb_ca_en50221 *ca, int slot, int open)
{
	struct budget_av *budget_av = (struct budget_av *) ca->data;
	struct saa7146_dev *saa = budget_av->budget.dev;
	int result;

	if (slot != 0)
		return -EINVAL;

	/* test the card detect line - needs to be done carefully
	 * since it never goes high for some CAMs on this interface (e.g. topuptv) */
	if (budget_av->slot_status == SLOTSTATUS_NONE) {
		saa7146_setgpio(saa, 3, SAA7146_GPIO_INPUT);
		udelay(1);
		if (saa7146_read(saa, PSR) & MASK_06) {
			if (budget_av->slot_status == SLOTSTATUS_NONE) {
				budget_av->slot_status = SLOTSTATUS_PRESENT;
				pr_info("cam inserted A\n");
			}
		}
		saa7146_setgpio(saa, 3, SAA7146_GPIO_OUTLO);
	}

	/* We also try and read from IO memory to work round the above detection bug. If
	 * there is no CAM, we will get a timeout. Only done if there is no cam
	 * present, since this test actually breaks some cams :(
	 *
	 * if the CI interface is not open, we also do the above test since we
	 * don't care if the cam has problems - we'll be resetting it on open() anyway */
	if ((budget_av->slot_status == SLOTSTATUS_NONE) || (!open)) {
		saa7146_setgpio(budget_av->budget.dev, 1, SAA7146_GPIO_OUTLO);
		result = ttpci_budget_debiread(&budget_av->budget, DEBICICAM, 0, 1, 0, 1);
		if ((result >= 0) && (budget_av->slot_status == SLOTSTATUS_NONE)) {
			budget_av->slot_status = SLOTSTATUS_PRESENT;
			pr_info("cam inserted B\n");
		} else if (result < 0) {
			if (budget_av->slot_status != SLOTSTATUS_NONE) {
				ciintf_slot_shutdown(ca, slot);
				pr_info("cam ejected 5\n");
				return 0;
			}
		}
	}

	/* read from attribute memory in reset/ready state to know when the CAM is ready */
	if (budget_av->slot_status == SLOTSTATUS_RESET) {
		result = ciintf_read_attribute_mem(ca, slot, 0);
		if (result == 0x1d) {
			budget_av->slot_status = SLOTSTATUS_READY;
		}
	}

	/* work out correct return code */
	if (budget_av->slot_status != SLOTSTATUS_NONE) {
		if (budget_av->slot_status & SLOTSTATUS_READY) {
			return DVB_CA_EN50221_POLL_CAM_PRESENT | DVB_CA_EN50221_POLL_CAM_READY;
		}
		return DVB_CA_EN50221_POLL_CAM_PRESENT;
	}
	return 0;
}

static int ciintf_init(struct budget_av *budget_av)
{
	struct saa7146_dev *saa = budget_av->budget.dev;
	int result;

	memset(&budget_av->ca, 0, sizeof(struct dvb_ca_en50221));

	saa7146_setgpio(saa, 0, SAA7146_GPIO_OUTLO);
	saa7146_setgpio(saa, 1, SAA7146_GPIO_OUTLO);
	saa7146_setgpio(saa, 2, SAA7146_GPIO_OUTLO);
	saa7146_setgpio(saa, 3, SAA7146_GPIO_OUTLO);

	/* Enable DEBI pins */
	saa7146_write(saa, MC1, MASK_27 | MASK_11);

	/* register CI interface */
	budget_av->ca.owner = THIS_MODULE;
	budget_av->ca.read_attribute_mem = ciintf_read_attribute_mem;
	budget_av->ca.write_attribute_mem = ciintf_write_attribute_mem;
	budget_av->ca.read_cam_control = ciintf_read_cam_control;
	budget_av->ca.write_cam_control = ciintf_write_cam_control;
	budget_av->ca.slot_reset = ciintf_slot_reset;
	budget_av->ca.slot_shutdown = ciintf_slot_shutdown;
	budget_av->ca.slot_ts_enable = ciintf_slot_ts_enable;
	budget_av->ca.poll_slot_status = ciintf_poll_slot_status;
	budget_av->ca.data = budget_av;
	budget_av->budget.ci_present = 1;
	budget_av->slot_status = SLOTSTATUS_NONE;

	if ((result = dvb_ca_en50221_init(&budget_av->budget.dvb_adapter,
					  &budget_av->ca, 0, 1)) != 0) {
		pr_err("ci initialisation failed\n");
		goto error;
	}

	pr_info("ci interface initialised\n");
	return 0;

error:
	saa7146_write(saa, MC1, MASK_27);
	return result;
}

static void ciintf_deinit(struct budget_av *budget_av)
{
	struct saa7146_dev *saa = budget_av->budget.dev;

	saa7146_setgpio(saa, 0, SAA7146_GPIO_INPUT);
	saa7146_setgpio(saa, 1, SAA7146_GPIO_INPUT);
	saa7146_setgpio(saa, 2, SAA7146_GPIO_INPUT);
	saa7146_setgpio(saa, 3, SAA7146_GPIO_INPUT);

	/* release the CA device */
	dvb_ca_en50221_release(&budget_av->ca);

	/* disable DEBI pins */
	saa7146_write(saa, MC1, MASK_27);
}


static const u8 saa7113_tab[] = {
	0x01, 0x08,
	0x02, 0xc0,
	0x03, 0x33,
	0x04, 0x00,
	0x05, 0x00,
	0x06, 0xeb,
	0x07, 0xe0,
	0x08, 0x28,
	0x09, 0x00,
	0x0a, 0x80,
	0x0b, 0x47,
	0x0c, 0x40,
	0x0d, 0x00,
	0x0e, 0x01,
	0x0f, 0x44,

	0x10, 0x08,
	0x11, 0x0c,
	0x12, 0x7b,
	0x13, 0x00,
	0x15, 0x00, 0x16, 0x00, 0x17, 0x00,

	0x57, 0xff,
	0x40, 0x82, 0x58, 0x00, 0x59, 0x54, 0x5a, 0x07,
	0x5b, 0x83, 0x5e, 0x00,
	0xff
};

static int saa7113_init(struct budget_av *budget_av)
{
	struct budget *budget = &budget_av->budget;
	struct saa7146_dev *saa = budget->dev;
	const u8 *data = saa7113_tab;

	saa7146_setgpio(saa, 0, SAA7146_GPIO_OUTHI);
	msleep(200);

	if (i2c_writereg(&budget->i2c_adap, 0x4a, 0x01, 0x08) != 1) {
		dprintk(1, "saa7113 not found on KNC card\n");
		return -ENODEV;
	}

	dprintk(1, "saa7113 detected and initializing\n");

	while (*data != 0xff) {
		i2c_writereg(&budget->i2c_adap, 0x4a, *data, *(data + 1));
		data += 2;
	}

	dprintk(1, "saa7113  status=%02x\n", i2c_readreg(&budget->i2c_adap, 0x4a, 0x1f));

	return 0;
}

static int saa7113_setinput(struct budget_av *budget_av, int input)
{
	struct budget *budget = &budget_av->budget;

	if (1 != budget_av->has_saa7113)
		return -ENODEV;

	if (input == 1) {
		i2c_writereg(&budget->i2c_adap, 0x4a, 0x02, 0xc7);
		i2c_writereg(&budget->i2c_adap, 0x4a, 0x09, 0x80);
	} else if (input == 0) {
		i2c_writereg(&budget->i2c_adap, 0x4a, 0x02, 0xc0);
		i2c_writereg(&budget->i2c_adap, 0x4a, 0x09, 0x00);
	} else
		return -EINVAL;

	budget_av->cur_input = input;
	return 0;
}


static int philips_su1278_ty_ci_set_symbol_rate(struct dvb_frontend *fe, u32 srate, u32 ratio)
{
	u8 aclk = 0;
	u8 bclk = 0;
	u8 m1;

	aclk = 0xb5;
	if (srate < 2000000)
		bclk = 0x86;
	else if (srate < 5000000)
		bclk = 0x89;
	else if (srate < 15000000)
		bclk = 0x8f;
	else if (srate < 45000000)
		bclk = 0x95;

	m1 = 0x14;
	if (srate < 4000000)
		m1 = 0x10;

	stv0299_writereg(fe, 0x13, aclk);
	stv0299_writereg(fe, 0x14, bclk);
	stv0299_writereg(fe, 0x1f, (ratio >> 16) & 0xff);
	stv0299_writereg(fe, 0x20, (ratio >> 8) & 0xff);
	stv0299_writereg(fe, 0x21, (ratio) & 0xf0);
	stv0299_writereg(fe, 0x0f, 0x80 | m1);

	return 0;
}

static int philips_su1278_ty_ci_tuner_set_params(struct dvb_frontend *fe)
{
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	u32 div;
	u8 buf[4];
	struct budget *budget = (struct budget *) fe->dvb->priv;
	struct i2c_msg msg = {.addr = 0x61,.flags = 0,.buf = buf,.len = sizeof(buf) };

	if ((c->frequency < 950000) || (c->frequency > 2150000))
		return -EINVAL;

	div = (c->frequency + (125 - 1)) / 125;	/* round correctly */
	buf[0] = (div >> 8) & 0x7f;
	buf[1] = div & 0xff;
	buf[2] = 0x80 | ((div & 0x18000) >> 10) | 4;
	buf[3] = 0x20;

	if (c->symbol_rate < 4000000)
		buf[3] |= 1;

	if (c->frequency < 1250000)
		buf[3] |= 0;
	else if (c->frequency < 1550000)
		buf[3] |= 0x40;
	else if (c->frequency < 2050000)
		buf[3] |= 0x80;
	else if (c->frequency < 2150000)
		buf[3] |= 0xC0;

	if (fe->ops.i2c_gate_ctrl)
		fe->ops.i2c_gate_ctrl(fe, 1);
	if (i2c_transfer(&budget->i2c_adap, &msg, 1) != 1)
		return -EIO;
	return 0;
}

static u8 typhoon_cinergy1200s_inittab[] = {
	0x01, 0x15,
	0x02, 0x30,
	0x03, 0x00,
	0x04, 0x7d,		/* F22FR = 0x7d, F22 = f_VCO / 128 / 0x7d = 22 kHz */
	0x05, 0x35,		/* I2CT = 0, SCLT = 1, SDAT = 1 */
	0x06, 0x40,		/* DAC not used, set to high impendance mode */
	0x07, 0x00,		/* DAC LSB */
	0x08, 0x40,		/* DiSEqC off */
	0x09, 0x00,		/* FIFO */
	0x0c, 0x51,		/* OP1 ctl = Normal, OP1 val = 1 (LNB Power ON) */
	0x0d, 0x82,		/* DC offset compensation = ON, beta_agc1 = 2 */
	0x0e, 0x23,		/* alpha_tmg = 2, beta_tmg = 3 */
	0x10, 0x3f,		// AGC2  0x3d
	0x11, 0x84,
	0x12, 0xb9,
	0x15, 0xc9,		// lock detector threshold
	0x16, 0x00,
	0x17, 0x00,
	0x18, 0x00,
	0x19, 0x00,
	0x1a, 0x00,
	0x1f, 0x50,
	0x20, 0x00,
	0x21, 0x00,
	0x22, 0x00,
	0x23, 0x00,
	0x28, 0x00,		// out imp: normal  out type: parallel FEC mode:0
	0x29, 0x1e,		// 1/2 threshold
	0x2a, 0x14,		// 2/3 threshold
	0x2b, 0x0f,		// 3/4 threshold
	0x2c, 0x09,		// 5/6 threshold
	0x2d, 0x05,		// 7/8 threshold
	0x2e, 0x01,
	0x31, 0x1f,		// test all FECs
	0x32, 0x19,		// viterbi and synchro search
	0x33, 0xfc,		// rs control
	0x34, 0x93,		// error control
	0x0f, 0x92,
	0xff, 0xff
};

static const struct stv0299_config typhoon_config = {
	.demod_address = 0x68,
	.inittab = typhoon_cinergy1200s_inittab,
	.mclk = 88000000UL,
	.invert = 0,
	.skip_reinit = 0,
	.lock_output = STV0299_LOCKOUTPUT_1,
	.volt13_op0_op1 = STV0299_VOLT13_OP0,
	.min_delay_ms = 100,
	.set_symbol_rate = philips_su1278_ty_ci_set_symbol_rate,
};


static const struct stv0299_config cinergy_1200s_config = {
	.demod_address = 0x68,
	.inittab = typhoon_cinergy1200s_inittab,
	.mclk = 88000000UL,
	.invert = 0,
	.skip_reinit = 0,
	.lock_output = STV0299_LOCKOUTPUT_0,
	.volt13_op0_op1 = STV0299_VOLT13_OP0,
	.min_delay_ms = 100,
	.set_symbol_rate = philips_su1278_ty_ci_set_symbol_rate,
};

static const struct stv0299_config cinergy_1200s_1894_0010_config = {
	.demod_address = 0x68,
	.inittab = typhoon_cinergy1200s_inittab,
	.mclk = 88000000UL,
	.invert = 1,
	.skip_reinit = 0,
	.lock_output = STV0299_LOCKOUTPUT_1,
	.volt13_op0_op1 = STV0299_VOLT13_OP0,
	.min_delay_ms = 100,
	.set_symbol_rate = philips_su1278_ty_ci_set_symbol_rate,
};

static int philips_cu1216_tuner_set_params(struct dvb_frontend *fe)
{
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	struct budget *budget = (struct budget *) fe->dvb->priv;
	u8 buf[6];
	struct i2c_msg msg = {.addr = 0x60,.flags = 0,.buf = buf,.len = sizeof(buf) };
	int i;

#define CU1216_IF 36125000
#define TUNER_MUL 62500

	u32 div = (c->frequency + CU1216_IF + TUNER_MUL / 2) / TUNER_MUL;

	buf[0] = (div >> 8) & 0x7f;
	buf[1] = div & 0xff;
	buf[2] = 0xce;
	buf[3] = (c->frequency < 150000000 ? 0x01 :
		  c->frequency < 445000000 ? 0x02 : 0x04);
	buf[4] = 0xde;
	buf[5] = 0x20;

	if (fe->ops.i2c_gate_ctrl)
		fe->ops.i2c_gate_ctrl(fe, 1);
	if (i2c_transfer(&budget->i2c_adap, &msg, 1) != 1)
		return -EIO;

	/* wait for the pll lock */
	msg.flags = I2C_M_RD;
	msg.len = 1;
	for (i = 0; i < 20; i++) {
		if (fe->ops.i2c_gate_ctrl)
			fe->ops.i2c_gate_ctrl(fe, 1);
		if (i2c_transfer(&budget->i2c_adap, &msg, 1) == 1 && (buf[0] & 0x40))
			break;
		msleep(10);
	}

	/* switch the charge pump to the lower current */
	msg.flags = 0;
	msg.len = 2;
	msg.buf = &buf[2];
	buf[2] &= ~0x40;
	if (fe->ops.i2c_gate_ctrl)
		fe->ops.i2c_gate_ctrl(fe, 1);
	if (i2c_transfer(&budget->i2c_adap, &msg, 1) != 1)
		return -EIO;

	return 0;
}

static struct tda1002x_config philips_cu1216_config = {
	.demod_address = 0x0c,
	.invert = 1,
};

static struct tda1002x_config philips_cu1216_config_altaddress = {
	.demod_address = 0x0d,
	.invert = 0,
};

static struct tda10023_config philips_cu1216_tda10023_config = {
	.demod_address = 0x0c,
	.invert = 1,
};

static int philips_tu1216_tuner_init(struct dvb_frontend *fe)
{
	struct budget *budget = (struct budget *) fe->dvb->priv;
	static u8 tu1216_init[] = { 0x0b, 0xf5, 0x85, 0xab };
	struct i2c_msg tuner_msg = {.addr = 0x60,.flags = 0,.buf = tu1216_init,.len = sizeof(tu1216_init) };

	// setup PLL configuration
	if (fe->ops.i2c_gate_ctrl)
		fe->ops.i2c_gate_ctrl(fe, 1);
	if (i2c_transfer(&budget->i2c_adap, &tuner_msg, 1) != 1)
		return -EIO;
	msleep(1);

	return 0;
}

static int philips_tu1216_tuner_set_params(struct dvb_frontend *fe)
{
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	struct budget *budget = (struct budget *) fe->dvb->priv;
	u8 tuner_buf[4];
	struct i2c_msg tuner_msg = {.addr = 0x60,.flags = 0,.buf = tuner_buf,.len =
			sizeof(tuner_buf) };
	int tuner_frequency = 0;
	u8 band, cp, filter;

	// determine charge pump
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

	// determine band
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

	// setup PLL filter
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

	// calculate divisor
	// ((36166000+((1000000/6)/2)) + Finput)/(1000000/6)
	tuner_frequency = (((c->frequency / 1000) * 6) + 217496) / 1000;

	// setup tuner buffer
	tuner_buf[0] = (tuner_frequency >> 8) & 0x7f;
	tuner_buf[1] = tuner_frequency & 0xff;
	tuner_buf[2] = 0xca;
	tuner_buf[3] = (cp << 5) | (filter << 3) | band;

	if (fe->ops.i2c_gate_ctrl)
		fe->ops.i2c_gate_ctrl(fe, 1);
	if (i2c_transfer(&budget->i2c_adap, &tuner_msg, 1) != 1)
		return -EIO;

	msleep(1);
	return 0;
}

static int philips_tu1216_request_firmware(struct dvb_frontend *fe,
					   const struct firmware **fw, char *name)
{
	struct budget *budget = (struct budget *) fe->dvb->priv;

	return request_firmware(fw, name, &budget->dev->pci->dev);
}

static struct tda1004x_config philips_tu1216_config = {

	.demod_address = 0x8,
	.invert = 1,
	.invert_oclk = 1,
	.xtal_freq = TDA10046_XTAL_4M,
	.agc_config = TDA10046_AGC_DEFAULT,
	.if_freq = TDA10046_FREQ_3617,
	.request_firmware = philips_tu1216_request_firmware,
};

static u8 philips_sd1878_inittab[] = {
	0x01, 0x15,
	0x02, 0x30,
	0x03, 0x00,
	0x04, 0x7d,
	0x05, 0x35,
	0x06, 0x40,
	0x07, 0x00,
	0x08, 0x43,
	0x09, 0x02,
	0x0C, 0x51,
	0x0D, 0x82,
	0x0E, 0x23,
	0x10, 0x3f,
	0x11, 0x84,
	0x12, 0xb9,
	0x15, 0xc9,
	0x16, 0x19,
	0x17, 0x8c,
	0x18, 0x59,
	0x19, 0xf8,
	0x1a, 0xfe,
	0x1c, 0x7f,
	0x1d, 0x00,
	0x1e, 0x00,
	0x1f, 0x50,
	0x20, 0x00,
	0x21, 0x00,
	0x22, 0x00,
	0x23, 0x00,
	0x28, 0x00,
	0x29, 0x28,
	0x2a, 0x14,
	0x2b, 0x0f,
	0x2c, 0x09,
	0x2d, 0x09,
	0x31, 0x1f,
	0x32, 0x19,
	0x33, 0xfc,
	0x34, 0x93,
	0xff, 0xff
};

static int philips_sd1878_ci_set_symbol_rate(struct dvb_frontend *fe,
		u32 srate, u32 ratio)
{
	u8 aclk = 0;
	u8 bclk = 0;
	u8 m1;

	aclk = 0xb5;
	if (srate < 2000000)
		bclk = 0x86;
	else if (srate < 5000000)
		bclk = 0x89;
	else if (srate < 15000000)
		bclk = 0x8f;
	else if (srate < 45000000)
		bclk = 0x95;

	m1 = 0x14;
	if (srate < 4000000)
		m1 = 0x10;

	stv0299_writereg(fe, 0x0e, 0x23);
	stv0299_writereg(fe, 0x0f, 0x94);
	stv0299_writereg(fe, 0x10, 0x39);
	stv0299_writereg(fe, 0x13, aclk);
	stv0299_writereg(fe, 0x14, bclk);
	stv0299_writereg(fe, 0x15, 0xc9);
	stv0299_writereg(fe, 0x1f, (ratio >> 16) & 0xff);
	stv0299_writereg(fe, 0x20, (ratio >> 8) & 0xff);
	stv0299_writereg(fe, 0x21, (ratio) & 0xf0);
	stv0299_writereg(fe, 0x0f, 0x80 | m1);

	return 0;
}

static const struct stv0299_config philips_sd1878_config = {
	.demod_address = 0x68,
     .inittab = philips_sd1878_inittab,
	.mclk = 88000000UL,
	.invert = 0,
	.skip_reinit = 0,
	.lock_output = STV0299_LOCKOUTPUT_1,
	.volt13_op0_op1 = STV0299_VOLT13_OP0,
	.min_delay_ms = 100,
	.set_symbol_rate = philips_sd1878_ci_set_symbol_rate,
};

/* KNC1 DVB-S (STB0899) Inittab	*/
static const struct stb0899_s1_reg knc1_stb0899_s1_init_1[] = {

	{ STB0899_DEV_ID		, 0x81 },
	{ STB0899_DISCNTRL1		, 0x32 },
	{ STB0899_DISCNTRL2		, 0x80 },
	{ STB0899_DISRX_ST0		, 0x04 },
	{ STB0899_DISRX_ST1		, 0x00 },
	{ STB0899_DISPARITY		, 0x00 },
	{ STB0899_DISSTATUS		, 0x20 },
	{ STB0899_DISF22		, 0x8c },
	{ STB0899_DISF22RX		, 0x9a },
	{ STB0899_SYSREG		, 0x0b },
	{ STB0899_ACRPRESC		, 0x11 },
	{ STB0899_ACRDIV1		, 0x0a },
	{ STB0899_ACRDIV2		, 0x05 },
	{ STB0899_DACR1			, 0x00 },
	{ STB0899_DACR2			, 0x00 },
	{ STB0899_OUTCFG		, 0x00 },
	{ STB0899_MODECFG		, 0x00 },
	{ STB0899_IRQSTATUS_3		, 0x30 },
	{ STB0899_IRQSTATUS_2		, 0x00 },
	{ STB0899_IRQSTATUS_1		, 0x00 },
	{ STB0899_IRQSTATUS_0		, 0x00 },
	{ STB0899_IRQMSK_3		, 0xf3 },
	{ STB0899_IRQMSK_2		, 0xfc },
	{ STB0899_IRQMSK_1		, 0xff },
	{ STB0899_IRQMSK_0		, 0xff },
	{ STB0899_IRQCFG		, 0x00 },
	{ STB0899_I2CCFG		, 0x88 },
	{ STB0899_I2CRPT		, 0x58 }, /* Repeater=8, Stop=disabled */
	{ STB0899_IOPVALUE5		, 0x00 },
	{ STB0899_IOPVALUE4		, 0x20 },
	{ STB0899_IOPVALUE3		, 0xc9 },
	{ STB0899_IOPVALUE2		, 0x90 },
	{ STB0899_IOPVALUE1		, 0x40 },
	{ STB0899_IOPVALUE0		, 0x00 },
	{ STB0899_GPIO00CFG		, 0x82 },
	{ STB0899_GPIO01CFG		, 0x82 },
	{ STB0899_GPIO02CFG		, 0x82 },
	{ STB0899_GPIO03CFG		, 0x82 },
	{ STB0899_GPIO04CFG		, 0x82 },
	{ STB0899_GPIO05CFG		, 0x82 },
	{ STB0899_GPIO06CFG		, 0x82 },
	{ STB0899_GPIO07CFG		, 0x82 },
	{ STB0899_GPIO08CFG		, 0x82 },
	{ STB0899_GPIO09CFG		, 0x82 },
	{ STB0899_GPIO10CFG		, 0x82 },
	{ STB0899_GPIO11CFG		, 0x82 },
	{ STB0899_GPIO12CFG		, 0x82 },
	{ STB0899_GPIO13CFG		, 0x82 },
	{ STB0899_GPIO14CFG		, 0x82 },
	{ STB0899_GPIO15CFG		, 0x82 },
	{ STB0899_GPIO16CFG		, 0x82 },
	{ STB0899_GPIO17CFG		, 0x82 },
	{ STB0899_GPIO18CFG		, 0x82 },
	{ STB0899_GPIO19CFG		, 0x82 },
	{ STB0899_GPIO20CFG		, 0x82 },
	{ STB0899_SDATCFG		, 0xb8 },
	{ STB0899_SCLTCFG		, 0xba },
	{ STB0899_AGCRFCFG		, 0x08 }, /* 0x1c */
	{ STB0899_GPIO22		, 0x82 }, /* AGCBB2CFG */
	{ STB0899_GPIO21		, 0x91 }, /* AGCBB1CFG */
	{ STB0899_DIRCLKCFG		, 0x82 },
	{ STB0899_CLKOUT27CFG		, 0x7e },
	{ STB0899_STDBYCFG		, 0x82 },
	{ STB0899_CS0CFG		, 0x82 },
	{ STB0899_CS1CFG		, 0x82 },
	{ STB0899_DISEQCOCFG		, 0x20 },
	{ STB0899_GPIO32CFG		, 0x82 },
	{ STB0899_GPIO33CFG		, 0x82 },
	{ STB0899_GPIO34CFG		, 0x82 },
	{ STB0899_GPIO35CFG		, 0x82 },
	{ STB0899_GPIO36CFG		, 0x82 },
	{ STB0899_GPIO37CFG		, 0x82 },
	{ STB0899_GPIO38CFG		, 0x82 },
	{ STB0899_GPIO39CFG		, 0x82 },
	{ STB0899_NCOARSE		, 0x15 }, /* 0x15 = 27 Mhz Clock, F/3 = 198MHz, F/6 = 99MHz */
	{ STB0899_SYNTCTRL		, 0x02 }, /* 0x00 = CLK from CLKI, 0x02 = CLK from XTALI */
	{ STB0899_FILTCTRL		, 0x00 },
	{ STB0899_SYSCTRL		, 0x00 },
	{ STB0899_STOPCLK1		, 0x20 },
	{ STB0899_STOPCLK2		, 0x00 },
	{ STB0899_INTBUFSTATUS		, 0x00 },
	{ STB0899_INTBUFCTRL		, 0x0a },
	{ 0xffff			, 0xff },
};

static const struct stb0899_s1_reg knc1_stb0899_s1_init_3[] = {
	{ STB0899_DEMOD			, 0x00 },
	{ STB0899_RCOMPC		, 0xc9 },
	{ STB0899_AGC1CN		, 0x41 },
	{ STB0899_AGC1REF		, 0x08 },
	{ STB0899_RTC			, 0x7a },
	{ STB0899_TMGCFG		, 0x4e },
	{ STB0899_AGC2REF		, 0x33 },
	{ STB0899_TLSR			, 0x84 },
	{ STB0899_CFD			, 0xee },
	{ STB0899_ACLC			, 0x87 },
	{ STB0899_BCLC			, 0x94 },
	{ STB0899_EQON			, 0x41 },
	{ STB0899_LDT			, 0xdd },
	{ STB0899_LDT2			, 0xc9 },
	{ STB0899_EQUALREF		, 0xb4 },
	{ STB0899_TMGRAMP		, 0x10 },
	{ STB0899_TMGTHD		, 0x30 },
	{ STB0899_IDCCOMP		, 0xfb },
	{ STB0899_QDCCOMP		, 0x03 },
	{ STB0899_POWERI		, 0x3b },
	{ STB0899_POWERQ		, 0x3d },
	{ STB0899_RCOMP			, 0x81 },
	{ STB0899_AGCIQIN		, 0x80 },
	{ STB0899_AGC2I1		, 0x04 },
	{ STB0899_AGC2I2		, 0xf5 },
	{ STB0899_TLIR			, 0x25 },
	{ STB0899_RTF			, 0x80 },
	{ STB0899_DSTATUS		, 0x00 },
	{ STB0899_LDI			, 0xca },
	{ STB0899_CFRM			, 0xf1 },
	{ STB0899_CFRL			, 0xf3 },
	{ STB0899_NIRM			, 0x2a },
	{ STB0899_NIRL			, 0x05 },
	{ STB0899_ISYMB			, 0x17 },
	{ STB0899_QSYMB			, 0xfa },
	{ STB0899_SFRH			, 0x2f },
	{ STB0899_SFRM			, 0x68 },
	{ STB0899_SFRL			, 0x40 },
	{ STB0899_SFRUPH		, 0x2f },
	{ STB0899_SFRUPM		, 0x68 },
	{ STB0899_SFRUPL		, 0x40 },
	{ STB0899_EQUAI1		, 0xfd },
	{ STB0899_EQUAQ1		, 0x04 },
	{ STB0899_EQUAI2		, 0x0f },
	{ STB0899_EQUAQ2		, 0xff },
	{ STB0899_EQUAI3		, 0xdf },
	{ STB0899_EQUAQ3		, 0xfa },
	{ STB0899_EQUAI4		, 0x37 },
	{ STB0899_EQUAQ4		, 0x0d },
	{ STB0899_EQUAI5		, 0xbd },
	{ STB0899_EQUAQ5		, 0xf7 },
	{ STB0899_DSTATUS2		, 0x00 },
	{ STB0899_VSTATUS		, 0x00 },
	{ STB0899_VERROR		, 0xff },
	{ STB0899_IQSWAP		, 0x2a },
	{ STB0899_ECNT1M		, 0x00 },
	{ STB0899_ECNT1L		, 0x00 },
	{ STB0899_ECNT2M		, 0x00 },
	{ STB0899_ECNT2L		, 0x00 },
	{ STB0899_ECNT3M		, 0x00 },
	{ STB0899_ECNT3L		, 0x00 },
	{ STB0899_FECAUTO1		, 0x06 },
	{ STB0899_FECM			, 0x01 },
	{ STB0899_VTH12			, 0xf0 },
	{ STB0899_VTH23			, 0xa0 },
	{ STB0899_VTH34			, 0x78 },
	{ STB0899_VTH56			, 0x4e },
	{ STB0899_VTH67			, 0x48 },
	{ STB0899_VTH78			, 0x38 },
	{ STB0899_PRVIT			, 0xff },
	{ STB0899_VITSYNC		, 0x19 },
	{ STB0899_RSULC			, 0xb1 }, /* DVB = 0xb1, DSS = 0xa1 */
	{ STB0899_TSULC			, 0x42 },
	{ STB0899_RSLLC			, 0x40 },
	{ STB0899_TSLPL			, 0x12 },
	{ STB0899_TSCFGH		, 0x0c },
	{ STB0899_TSCFGM		, 0x00 },
	{ STB0899_TSCFGL		, 0x0c },
	{ STB0899_TSOUT			, 0x4d }, /* 0x0d for CAM */
	{ STB0899_RSSYNCDEL		, 0x00 },
	{ STB0899_TSINHDELH		, 0x02 },
	{ STB0899_TSINHDELM		, 0x00 },
	{ STB0899_TSINHDELL		, 0x00 },
	{ STB0899_TSLLSTKM		, 0x00 },
	{ STB0899_TSLLSTKL		, 0x00 },
	{ STB0899_TSULSTKM		, 0x00 },
	{ STB0899_TSULSTKL		, 0xab },
	{ STB0899_PCKLENUL		, 0x00 },
	{ STB0899_PCKLENLL		, 0xcc },
	{ STB0899_RSPCKLEN		, 0xcc },
	{ STB0899_TSSTATUS		, 0x80 },
	{ STB0899_ERRCTRL1		, 0xb6 },
	{ STB0899_ERRCTRL2		, 0x96 },
	{ STB0899_ERRCTRL3		, 0x89 },
	{ STB0899_DMONMSK1		, 0x27 },
	{ STB0899_DMONMSK0		, 0x03 },
	{ STB0899_DEMAPVIT		, 0x5c },
	{ STB0899_PLPARM		, 0x1f },
	{ STB0899_PDELCTRL		, 0x48 },
	{ STB0899_PDELCTRL2		, 0x00 },
	{ STB0899_BBHCTRL1		, 0x00 },
	{ STB0899_BBHCTRL2		, 0x00 },
	{ STB0899_HYSTTHRESH		, 0x77 },
	{ STB0899_MATCSTM		, 0x00 },
	{ STB0899_MATCSTL		, 0x00 },
	{ STB0899_UPLCSTM		, 0x00 },
	{ STB0899_UPLCSTL		, 0x00 },
	{ STB0899_DFLCSTM		, 0x00 },
	{ STB0899_DFLCSTL		, 0x00 },
	{ STB0899_SYNCCST		, 0x00 },
	{ STB0899_SYNCDCSTM		, 0x00 },
	{ STB0899_SYNCDCSTL		, 0x00 },
	{ STB0899_ISI_ENTRY		, 0x00 },
	{ STB0899_ISI_BIT_EN		, 0x00 },
	{ STB0899_MATSTRM		, 0x00 },
	{ STB0899_MATSTRL		, 0x00 },
	{ STB0899_UPLSTRM		, 0x00 },
	{ STB0899_UPLSTRL		, 0x00 },
	{ STB0899_DFLSTRM		, 0x00 },
	{ STB0899_DFLSTRL		, 0x00 },
	{ STB0899_SYNCSTR		, 0x00 },
	{ STB0899_SYNCDSTRM		, 0x00 },
	{ STB0899_SYNCDSTRL		, 0x00 },
	{ STB0899_CFGPDELSTATUS1	, 0x10 },
	{ STB0899_CFGPDELSTATUS2	, 0x00 },
	{ STB0899_BBFERRORM		, 0x00 },
	{ STB0899_BBFERRORL		, 0x00 },
	{ STB0899_UPKTERRORM		, 0x00 },
	{ STB0899_UPKTERRORL		, 0x00 },
	{ 0xffff			, 0xff },
};

/* STB0899 demodulator config for the KNC1 and clones */
static struct stb0899_config knc1_dvbs2_config = {
	.init_dev		= knc1_stb0899_s1_init_1,
	.init_s2_demod		= stb0899_s2_init_2,
	.init_s1_demod		= knc1_stb0899_s1_init_3,
	.init_s2_fec		= stb0899_s2_init_4,
	.init_tst		= stb0899_s1_init_5,

	.postproc		= NULL,

	.demod_address		= 0x68,
//	.ts_output_mode		= STB0899_OUT_PARALLEL,	/* types = SERIAL/PARALLEL	*/
	.block_sync_mode	= STB0899_SYNC_FORCED,	/* DSS, SYNC_FORCED/UNSYNCED	*/
//	.ts_pfbit_toggle	= STB0899_MPEG_NORMAL,	/* DirecTV, MPEG toggling seq	*/

	.xtal_freq		= 27000000,
	.inversion		= IQ_SWAP_OFF,

	.lo_clk			= 76500000,
	.hi_clk			= 90000000,

	.esno_ave		= STB0899_DVBS2_ESNO_AVE,
	.esno_quant		= STB0899_DVBS2_ESNO_QUANT,
	.avframes_coarse	= STB0899_DVBS2_AVFRAMES_COARSE,
	.avframes_fine		= STB0899_DVBS2_AVFRAMES_FINE,
	.miss_threshold		= STB0899_DVBS2_MISS_THRESHOLD,
	.uwp_threshold_acq	= STB0899_DVBS2_UWP_THRESHOLD_ACQ,
	.uwp_threshold_track	= STB0899_DVBS2_UWP_THRESHOLD_TRACK,
	.uwp_threshold_sof	= STB0899_DVBS2_UWP_THRESHOLD_SOF,
	.sof_search_timeout	= STB0899_DVBS2_SOF_SEARCH_TIMEOUT,

	.btr_nco_bits		= STB0899_DVBS2_BTR_NCO_BITS,
	.btr_gain_shift_offset	= STB0899_DVBS2_BTR_GAIN_SHIFT_OFFSET,
	.crl_nco_bits		= STB0899_DVBS2_CRL_NCO_BITS,
	.ldpc_max_iter		= STB0899_DVBS2_LDPC_MAX_ITER,

	.tuner_get_frequency	= tda8261_get_frequency,
	.tuner_set_frequency	= tda8261_set_frequency,
	.tuner_set_bandwidth	= NULL,
	.tuner_get_bandwidth	= tda8261_get_bandwidth,
	.tuner_set_rfsiggain	= NULL
};

/*
 * SD1878/SHA tuner config
 * 1F, Single I/P, Horizontal mount, High Sensitivity
 */
static const struct tda8261_config sd1878c_config = {
//	.name		= "SD1878/SHA",
	.addr		= 0x60,
	.step_size	= TDA8261_STEP_1000 /* kHz */
};

static u8 read_pwm(struct budget_av *budget_av)
{
	u8 b = 0xff;
	u8 pwm;
	struct i2c_msg msg[] = { {.addr = 0x50,.flags = 0,.buf = &b,.len = 1},
	{.addr = 0x50,.flags = I2C_M_RD,.buf = &pwm,.len = 1}
	};

	if ((i2c_transfer(&budget_av->budget.i2c_adap, msg, 2) != 2)
	    || (pwm == 0xff))
		pwm = 0x48;

	return pwm;
}

#define SUBID_DVBS_KNC1			0x0010
#define SUBID_DVBS_KNC1_PLUS		0x0011
#define SUBID_DVBS_TYPHOON		0x4f56
#define SUBID_DVBS_CINERGY1200		0x1154
#define SUBID_DVBS_CYNERGY1200N		0x1155
#define SUBID_DVBS_TV_STAR		0x0014
#define SUBID_DVBS_TV_STAR_PLUS_X4	0x0015
#define SUBID_DVBS_TV_STAR_CI		0x0016
#define SUBID_DVBS2_KNC1		0x0018
#define SUBID_DVBS2_KNC1_OEM		0x0019
#define SUBID_DVBS_EASYWATCH_1		0x001a
#define SUBID_DVBS_EASYWATCH_2		0x001b
#define SUBID_DVBS2_EASYWATCH		0x001d
#define SUBID_DVBS_EASYWATCH		0x001e

#define SUBID_DVBC_EASYWATCH		0x002a
#define SUBID_DVBC_EASYWATCH_MK3	0x002c
#define SUBID_DVBC_KNC1			0x0020
#define SUBID_DVBC_KNC1_PLUS		0x0021
#define SUBID_DVBC_KNC1_MK3		0x0022
#define SUBID_DVBC_KNC1_TDA10024	0x0028
#define SUBID_DVBC_KNC1_PLUS_MK3	0x0023
#define SUBID_DVBC_CINERGY1200		0x1156
#define SUBID_DVBC_CINERGY1200_MK3	0x1176

#define SUBID_DVBT_EASYWATCH		0x003a
#define SUBID_DVBT_KNC1_PLUS		0x0031
#define SUBID_DVBT_KNC1			0x0030
#define SUBID_DVBT_CINERGY1200		0x1157

static void frontend_init(struct budget_av *budget_av)
{
	struct saa7146_dev * saa = budget_av->budget.dev;
	struct dvb_frontend * fe = NULL;

	/* Enable / PowerON Frontend */
	saa7146_setgpio(saa, 0, SAA7146_GPIO_OUTLO);

	/* Wait for PowerON */
	msleep(100);

	/* additional setup necessary for the PLUS cards */
	switch (saa->pci->subsystem_device) {
		case SUBID_DVBS_KNC1_PLUS:
		case SUBID_DVBC_KNC1_PLUS:
		case SUBID_DVBT_KNC1_PLUS:
		case SUBID_DVBC_EASYWATCH:
		case SUBID_DVBC_KNC1_PLUS_MK3:
		case SUBID_DVBS2_KNC1:
		case SUBID_DVBS2_KNC1_OEM:
		case SUBID_DVBS2_EASYWATCH:
			saa7146_setgpio(saa, 3, SAA7146_GPIO_OUTHI);
			break;
	}

	switch (saa->pci->subsystem_device) {

	case SUBID_DVBS_KNC1:
		/*
		 * maybe that setting is needed for other dvb-s cards as well,
		 * but so far it has been only confirmed for this type
		 */
		budget_av->reinitialise_demod = 1;
		fallthrough;
	case SUBID_DVBS_KNC1_PLUS:
	case SUBID_DVBS_EASYWATCH_1:
		if (saa->pci->subsystem_vendor == 0x1894) {
			fe = dvb_attach(stv0299_attach, &cinergy_1200s_1894_0010_config,
					     &budget_av->budget.i2c_adap);
			if (fe) {
				dvb_attach(tua6100_attach, fe, 0x60, &budget_av->budget.i2c_adap);
			}
		} else {
			fe = dvb_attach(stv0299_attach, &typhoon_config,
					     &budget_av->budget.i2c_adap);
			if (fe) {
				fe->ops.tuner_ops.set_params = philips_su1278_ty_ci_tuner_set_params;
			}
		}
		break;

	case SUBID_DVBS_TV_STAR:
	case SUBID_DVBS_TV_STAR_PLUS_X4:
	case SUBID_DVBS_TV_STAR_CI:
	case SUBID_DVBS_CYNERGY1200N:
	case SUBID_DVBS_EASYWATCH:
	case SUBID_DVBS_EASYWATCH_2:
		fe = dvb_attach(stv0299_attach, &philips_sd1878_config,
				&budget_av->budget.i2c_adap);
		if (fe) {
			dvb_attach(dvb_pll_attach, fe, 0x60,
				   &budget_av->budget.i2c_adap,
				   DVB_PLL_PHILIPS_SD1878_TDA8261);
		}
		break;

	case SUBID_DVBS_TYPHOON:
		fe = dvb_attach(stv0299_attach, &typhoon_config,
				    &budget_av->budget.i2c_adap);
		if (fe) {
			fe->ops.tuner_ops.set_params = philips_su1278_ty_ci_tuner_set_params;
		}
		break;
	case SUBID_DVBS2_KNC1:
	case SUBID_DVBS2_KNC1_OEM:
	case SUBID_DVBS2_EASYWATCH:
		budget_av->reinitialise_demod = 1;
		if ((fe = dvb_attach(stb0899_attach, &knc1_dvbs2_config, &budget_av->budget.i2c_adap)))
			dvb_attach(tda8261_attach, fe, &sd1878c_config, &budget_av->budget.i2c_adap);

		break;
	case SUBID_DVBS_CINERGY1200:
		fe = dvb_attach(stv0299_attach, &cinergy_1200s_config,
				    &budget_av->budget.i2c_adap);
		if (fe) {
			fe->ops.tuner_ops.set_params = philips_su1278_ty_ci_tuner_set_params;
		}
		break;

	case SUBID_DVBC_KNC1:
	case SUBID_DVBC_KNC1_PLUS:
	case SUBID_DVBC_CINERGY1200:
	case SUBID_DVBC_EASYWATCH:
		budget_av->reinitialise_demod = 1;
		budget_av->budget.dev->i2c_bitrate = SAA7146_I2C_BUS_BIT_RATE_240;
		fe = dvb_attach(tda10021_attach, &philips_cu1216_config,
				     &budget_av->budget.i2c_adap,
				     read_pwm(budget_av));
		if (fe == NULL)
			fe = dvb_attach(tda10021_attach, &philips_cu1216_config_altaddress,
					     &budget_av->budget.i2c_adap,
					     read_pwm(budget_av));
		if (fe) {
			fe->ops.tuner_ops.set_params = philips_cu1216_tuner_set_params;
		}
		break;

	case SUBID_DVBC_EASYWATCH_MK3:
	case SUBID_DVBC_CINERGY1200_MK3:
	case SUBID_DVBC_KNC1_MK3:
	case SUBID_DVBC_KNC1_TDA10024:
	case SUBID_DVBC_KNC1_PLUS_MK3:
		budget_av->reinitialise_demod = 1;
		budget_av->budget.dev->i2c_bitrate = SAA7146_I2C_BUS_BIT_RATE_240;
		fe = dvb_attach(tda10023_attach,
			&philips_cu1216_tda10023_config,
			&budget_av->budget.i2c_adap,
			read_pwm(budget_av));
		if (fe) {
			fe->ops.tuner_ops.set_params = philips_cu1216_tuner_set_params;
		}
		break;

	case SUBID_DVBT_EASYWATCH:
	case SUBID_DVBT_KNC1:
	case SUBID_DVBT_KNC1_PLUS:
	case SUBID_DVBT_CINERGY1200:
		budget_av->reinitialise_demod = 1;
		fe = dvb_attach(tda10046_attach, &philips_tu1216_config,
				     &budget_av->budget.i2c_adap);
		if (fe) {
			fe->ops.tuner_ops.init = philips_tu1216_tuner_init;
			fe->ops.tuner_ops.set_params = philips_tu1216_tuner_set_params;
		}
		break;
	}

	if (fe == NULL) {
		pr_err("A frontend driver was not found for device [%04x:%04x] subsystem [%04x:%04x]\n",
		       saa->pci->vendor,
		       saa->pci->device,
		       saa->pci->subsystem_vendor,
		       saa->pci->subsystem_device);
		return;
	}

	budget_av->budget.dvb_frontend = fe;

	if (dvb_register_frontend(&budget_av->budget.dvb_adapter,
				  budget_av->budget.dvb_frontend)) {
		pr_err("Frontend registration failed!\n");
		dvb_frontend_detach(budget_av->budget.dvb_frontend);
		budget_av->budget.dvb_frontend = NULL;
	}
}


static void budget_av_irq(struct saa7146_dev *dev, u32 * isr)
{
	struct budget_av *budget_av = (struct budget_av *) dev->ext_priv;

	dprintk(8, "dev: %p, budget_av: %p\n", dev, budget_av);

	if (*isr & MASK_10)
		ttpci_budget_irq10_handler(dev, isr);
}

static int budget_av_detach(struct saa7146_dev *dev)
{
	struct budget_av *budget_av = (struct budget_av *) dev->ext_priv;
	int err;

	dprintk(2, "dev: %p\n", dev);

	if (1 == budget_av->has_saa7113) {
		saa7146_setgpio(dev, 0, SAA7146_GPIO_OUTLO);

		msleep(200);

		saa7146_unregister_device(&budget_av->vd, dev);

		saa7146_vv_release(dev);
	}

	if (budget_av->budget.ci_present)
		ciintf_deinit(budget_av);

	if (budget_av->budget.dvb_frontend != NULL) {
		dvb_unregister_frontend(budget_av->budget.dvb_frontend);
		dvb_frontend_detach(budget_av->budget.dvb_frontend);
	}
	err = ttpci_budget_deinit(&budget_av->budget);

	kfree(budget_av);

	return err;
}

#define KNC1_INPUTS 2
static struct v4l2_input knc1_inputs[KNC1_INPUTS] = {
	{ 0, "Composite", V4L2_INPUT_TYPE_TUNER, 1, 0,
		V4L2_STD_PAL_BG | V4L2_STD_NTSC_M, 0, V4L2_IN_CAP_STD },
	{ 1, "S-Video", V4L2_INPUT_TYPE_CAMERA, 2, 0,
		V4L2_STD_PAL_BG | V4L2_STD_NTSC_M, 0, V4L2_IN_CAP_STD },
};

static int vidioc_enum_input(struct file *file, void *fh, struct v4l2_input *i)
{
	dprintk(1, "VIDIOC_ENUMINPUT %d\n", i->index);
	if (i->index >= KNC1_INPUTS)
		return -EINVAL;
	memcpy(i, &knc1_inputs[i->index], sizeof(struct v4l2_input));
	return 0;
}

static int vidioc_g_input(struct file *file, void *fh, unsigned int *i)
{
	struct saa7146_dev *dev = ((struct saa7146_fh *)fh)->dev;
	struct budget_av *budget_av = (struct budget_av *)dev->ext_priv;

	*i = budget_av->cur_input;

	dprintk(1, "VIDIOC_G_INPUT %d\n", *i);
	return 0;
}

static int vidioc_s_input(struct file *file, void *fh, unsigned int input)
{
	struct saa7146_dev *dev = ((struct saa7146_fh *)fh)->dev;
	struct budget_av *budget_av = (struct budget_av *)dev->ext_priv;

	dprintk(1, "VIDIOC_S_INPUT %d\n", input);
	return saa7113_setinput(budget_av, input);
}

static struct saa7146_ext_vv vv_data;

static int budget_av_attach(struct saa7146_dev *dev, struct saa7146_pci_extension_data *info)
{
	struct budget_av *budget_av;
	u8 *mac;
	int err;

	dprintk(2, "dev: %p\n", dev);

	if (!(budget_av = kzalloc(sizeof(struct budget_av), GFP_KERNEL)))
		return -ENOMEM;

	budget_av->has_saa7113 = 0;
	budget_av->budget.ci_present = 0;

	dev->ext_priv = budget_av;

	err = ttpci_budget_init(&budget_av->budget, dev, info, THIS_MODULE,
				adapter_nr);
	if (err) {
		kfree(budget_av);
		return err;
	}

	/* knc1 initialization */
	saa7146_write(dev, DD1_STREAM_B, 0x04000000);
	saa7146_write(dev, DD1_INIT, 0x07000600);
	saa7146_write(dev, MC2, MASK_09 | MASK_25 | MASK_10 | MASK_26);

	if (saa7113_init(budget_av) == 0) {
		budget_av->has_saa7113 = 1;
		err = saa7146_vv_init(dev, &vv_data);
		if (err != 0) {
			/* fixme: proper cleanup here */
			ERR("cannot init vv subsystem\n");
			return err;
		}
		vv_data.vid_ops.vidioc_enum_input = vidioc_enum_input;
		vv_data.vid_ops.vidioc_g_input = vidioc_g_input;
		vv_data.vid_ops.vidioc_s_input = vidioc_s_input;

		if ((err = saa7146_register_device(&budget_av->vd, dev, "knc1", VFL_TYPE_VIDEO))) {
			/* fixme: proper cleanup here */
			ERR("cannot register capture v4l2 device\n");
			saa7146_vv_release(dev);
			return err;
		}

		/* beware: this modifies dev->vv ... */
		saa7146_set_hps_source_and_sync(dev, SAA7146_HPS_SOURCE_PORT_A,
						SAA7146_HPS_SYNC_PORT_A);

		saa7113_setinput(budget_av, 0);
	}

	/* fixme: find some sane values here... */
	saa7146_write(dev, PCI_BT_V1, 0x1c00101f);

	mac = budget_av->budget.dvb_adapter.proposed_mac;
	if (i2c_readregs(&budget_av->budget.i2c_adap, 0xa0, 0x30, mac, 6)) {
		pr_err("KNC1-%d: Could not read MAC from KNC1 card\n",
		       budget_av->budget.dvb_adapter.num);
		eth_zero_addr(mac);
	} else {
		pr_info("KNC1-%d: MAC addr = %pM\n",
			budget_av->budget.dvb_adapter.num, mac);
	}

	budget_av->budget.dvb_adapter.priv = budget_av;
	frontend_init(budget_av);
	ciintf_init(budget_av);

	ttpci_budget_init_hooks(&budget_av->budget);

	return 0;
}

static struct saa7146_standard standard[] = {
	{.name = "PAL",.id = V4L2_STD_PAL,
	 .v_offset = 0x17,.v_field = 288,
	 .h_offset = 0x14,.h_pixels = 680,
	 .v_max_out = 576,.h_max_out = 768 },

	{.name = "NTSC",.id = V4L2_STD_NTSC,
	 .v_offset = 0x16,.v_field = 240,
	 .h_offset = 0x06,.h_pixels = 708,
	 .v_max_out = 480,.h_max_out = 640, },
};

static struct saa7146_ext_vv vv_data = {
	.inputs = 2,
	.capabilities = 0,	// perhaps later: V4L2_CAP_VBI_CAPTURE, but that need tweaking with the saa7113
	.flags = 0,
	.stds = &standard[0],
	.num_stds = ARRAY_SIZE(standard),
};

static struct saa7146_extension budget_extension;

MAKE_BUDGET_INFO(knc1s, "KNC1 DVB-S", BUDGET_KNC1S);
MAKE_BUDGET_INFO(knc1s2,"KNC1 DVB-S2", BUDGET_KNC1S2);
MAKE_BUDGET_INFO(sates2,"Satelco EasyWatch DVB-S2", BUDGET_KNC1S2);
MAKE_BUDGET_INFO(knc1c, "KNC1 DVB-C", BUDGET_KNC1C);
MAKE_BUDGET_INFO(knc1t, "KNC1 DVB-T", BUDGET_KNC1T);
MAKE_BUDGET_INFO(kncxs, "KNC TV STAR DVB-S", BUDGET_TVSTAR);
MAKE_BUDGET_INFO(satewpls, "Satelco EasyWatch DVB-S light", BUDGET_TVSTAR);
MAKE_BUDGET_INFO(satewpls1, "Satelco EasyWatch DVB-S light", BUDGET_KNC1S);
MAKE_BUDGET_INFO(satewps, "Satelco EasyWatch DVB-S", BUDGET_KNC1S);
MAKE_BUDGET_INFO(satewplc, "Satelco EasyWatch DVB-C", BUDGET_KNC1CP);
MAKE_BUDGET_INFO(satewcmk3, "Satelco EasyWatch DVB-C MK3", BUDGET_KNC1C_MK3);
MAKE_BUDGET_INFO(satewt, "Satelco EasyWatch DVB-T", BUDGET_KNC1T);
MAKE_BUDGET_INFO(knc1sp, "KNC1 DVB-S Plus", BUDGET_KNC1SP);
MAKE_BUDGET_INFO(knc1spx4, "KNC1 DVB-S Plus X4", BUDGET_KNC1SP);
MAKE_BUDGET_INFO(knc1cp, "KNC1 DVB-C Plus", BUDGET_KNC1CP);
MAKE_BUDGET_INFO(knc1cmk3, "KNC1 DVB-C MK3", BUDGET_KNC1C_MK3);
MAKE_BUDGET_INFO(knc1ctda10024, "KNC1 DVB-C TDA10024", BUDGET_KNC1C_TDA10024);
MAKE_BUDGET_INFO(knc1cpmk3, "KNC1 DVB-C Plus MK3", BUDGET_KNC1CP_MK3);
MAKE_BUDGET_INFO(knc1tp, "KNC1 DVB-T Plus", BUDGET_KNC1TP);
MAKE_BUDGET_INFO(cin1200s, "TerraTec Cinergy 1200 DVB-S", BUDGET_CIN1200S);
MAKE_BUDGET_INFO(cin1200sn, "TerraTec Cinergy 1200 DVB-S", BUDGET_CIN1200S);
MAKE_BUDGET_INFO(cin1200c, "Terratec Cinergy 1200 DVB-C", BUDGET_CIN1200C);
MAKE_BUDGET_INFO(cin1200cmk3, "Terratec Cinergy 1200 DVB-C MK3", BUDGET_CIN1200C_MK3);
MAKE_BUDGET_INFO(cin1200t, "Terratec Cinergy 1200 DVB-T", BUDGET_CIN1200T);

static const struct pci_device_id pci_tbl[] = {
	MAKE_EXTENSION_PCI(knc1s, 0x1131, 0x4f56),
	MAKE_EXTENSION_PCI(knc1s, 0x1131, 0x0010),
	MAKE_EXTENSION_PCI(knc1s, 0x1894, 0x0010),
	MAKE_EXTENSION_PCI(knc1sp, 0x1131, 0x0011),
	MAKE_EXTENSION_PCI(knc1sp, 0x1894, 0x0011),
	MAKE_EXTENSION_PCI(kncxs, 0x1894, 0x0014),
	MAKE_EXTENSION_PCI(knc1spx4, 0x1894, 0x0015),
	MAKE_EXTENSION_PCI(kncxs, 0x1894, 0x0016),
	MAKE_EXTENSION_PCI(knc1s2, 0x1894, 0x0018),
	MAKE_EXTENSION_PCI(knc1s2, 0x1894, 0x0019),
	MAKE_EXTENSION_PCI(sates2, 0x1894, 0x001d),
	MAKE_EXTENSION_PCI(satewpls, 0x1894, 0x001e),
	MAKE_EXTENSION_PCI(satewpls1, 0x1894, 0x001a),
	MAKE_EXTENSION_PCI(satewps, 0x1894, 0x001b),
	MAKE_EXTENSION_PCI(satewplc, 0x1894, 0x002a),
	MAKE_EXTENSION_PCI(satewcmk3, 0x1894, 0x002c),
	MAKE_EXTENSION_PCI(satewt, 0x1894, 0x003a),
	MAKE_EXTENSION_PCI(knc1c, 0x1894, 0x0020),
	MAKE_EXTENSION_PCI(knc1cp, 0x1894, 0x0021),
	MAKE_EXTENSION_PCI(knc1cmk3, 0x1894, 0x0022),
	MAKE_EXTENSION_PCI(knc1ctda10024, 0x1894, 0x0028),
	MAKE_EXTENSION_PCI(knc1cpmk3, 0x1894, 0x0023),
	MAKE_EXTENSION_PCI(knc1t, 0x1894, 0x0030),
	MAKE_EXTENSION_PCI(knc1tp, 0x1894, 0x0031),
	MAKE_EXTENSION_PCI(cin1200s, 0x153b, 0x1154),
	MAKE_EXTENSION_PCI(cin1200sn, 0x153b, 0x1155),
	MAKE_EXTENSION_PCI(cin1200c, 0x153b, 0x1156),
	MAKE_EXTENSION_PCI(cin1200cmk3, 0x153b, 0x1176),
	MAKE_EXTENSION_PCI(cin1200t, 0x153b, 0x1157),
	{
	 .vendor = 0,
	}
};

MODULE_DEVICE_TABLE(pci, pci_tbl);

static struct saa7146_extension budget_extension = {
	.name = "budget_av",
	.flags = SAA7146_USE_I2C_IRQ,

	.pci_tbl = pci_tbl,

	.module = THIS_MODULE,
	.attach = budget_av_attach,
	.detach = budget_av_detach,

	.irq_mask = MASK_10,
	.irq_func = budget_av_irq,
};

static int __init budget_av_init(void)
{
	return saa7146_register_extension(&budget_extension);
}

static void __exit budget_av_exit(void)
{
	saa7146_unregister_extension(&budget_extension);
}

module_init(budget_av_init);
module_exit(budget_av_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ralph Metzler, Marcus Metzler, Michael Hunold, others");
MODULE_DESCRIPTION("driver for the SAA7146 based so-called budget PCI DVB w/ analog input and CI-module (e.g. the KNC cards)");
