// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * TTUSB DEC Frontend Driver
 *
 * Copyright (C) 2003-2004 Alex Woods <linux-dvb@giblets.org>
 */

#include <media/dvb_frontend.h>
#include "ttusbdecfe.h"


#define LOF_HI			10600000
#define LOF_LO			9750000

struct ttusbdecfe_state {

	/* configuration settings */
	const struct ttusbdecfe_config* config;

	struct dvb_frontend frontend;

	u8 hi_band;
	u8 voltage;
};


static int ttusbdecfe_dvbs_read_status(struct dvb_frontend *fe,
				       enum fe_status *status)
{
	*status = FE_HAS_SIGNAL | FE_HAS_VITERBI |
		FE_HAS_SYNC | FE_HAS_CARRIER | FE_HAS_LOCK;
	return 0;
}


static int ttusbdecfe_dvbt_read_status(struct dvb_frontend *fe,
				       enum fe_status *status)
{
	struct ttusbdecfe_state* state = fe->demodulator_priv;
	u8 b[] = { 0x00, 0x00, 0x00, 0x00,
		   0x00, 0x00, 0x00, 0x00 };
	u8 result[4];
	int len, ret;

	*status=0;

	ret=state->config->send_command(fe, 0x73, sizeof(b), b, &len, result);
	if(ret)
		return ret;

	if(len != 4) {
		printk(KERN_ERR "%s: unexpected reply\n", __func__);
		return -EIO;
	}

	switch(result[3]) {
		case 1:  /* not tuned yet */
		case 2:  /* no signal/no lock*/
			break;
		case 3:	 /* signal found and locked*/
			*status = FE_HAS_SIGNAL | FE_HAS_VITERBI |
			FE_HAS_SYNC | FE_HAS_CARRIER | FE_HAS_LOCK;
			break;
		case 4:
			*status = FE_TIMEDOUT;
			break;
		default:
			pr_info("%s: returned unknown value: %d\n",
				__func__, result[3]);
			return -EIO;
	}

	return 0;
}

static int ttusbdecfe_dvbt_set_frontend(struct dvb_frontend *fe)
{
	struct dtv_frontend_properties *p = &fe->dtv_property_cache;
	struct ttusbdecfe_state* state = (struct ttusbdecfe_state*) fe->demodulator_priv;
	u8 b[] = { 0x00, 0x00, 0x00, 0x03,
		   0x00, 0x00, 0x00, 0x00,
		   0x00, 0x00, 0x00, 0x01,
		   0x00, 0x00, 0x00, 0xff,
		   0x00, 0x00, 0x00, 0xff };

	__be32 freq = htonl(p->frequency / 1000);
	memcpy(&b[4], &freq, sizeof (u32));
	state->config->send_command(fe, 0x71, sizeof(b), b, NULL, NULL);

	return 0;
}

static int ttusbdecfe_dvbt_get_tune_settings(struct dvb_frontend* fe,
					struct dvb_frontend_tune_settings* fesettings)
{
		fesettings->min_delay_ms = 1500;
		/* Drift compensation makes no sense for DVB-T */
		fesettings->step_size = 0;
		fesettings->max_drift = 0;
		return 0;
}

static int ttusbdecfe_dvbs_set_frontend(struct dvb_frontend *fe)
{
	struct dtv_frontend_properties *p = &fe->dtv_property_cache;
	struct ttusbdecfe_state* state = (struct ttusbdecfe_state*) fe->demodulator_priv;

	u8 b[] = { 0x00, 0x00, 0x00, 0x01,
		   0x00, 0x00, 0x00, 0x00,
		   0x00, 0x00, 0x00, 0x01,
		   0x00, 0x00, 0x00, 0x00,
		   0x00, 0x00, 0x00, 0x00,
		   0x00, 0x00, 0x00, 0x00,
		   0x00, 0x00, 0x00, 0x00,
		   0x00, 0x00, 0x00, 0x00,
		   0x00, 0x00, 0x00, 0x00,
		   0x00, 0x00, 0x00, 0x00 };
	__be32 freq;
	__be32 sym_rate;
	__be32 band;
	__be32 lnb_voltage;

	freq = htonl(p->frequency +
	       (state->hi_band ? LOF_HI : LOF_LO));
	memcpy(&b[4], &freq, sizeof(u32));
	sym_rate = htonl(p->symbol_rate);
	memcpy(&b[12], &sym_rate, sizeof(u32));
	band = htonl(state->hi_band ? LOF_HI : LOF_LO);
	memcpy(&b[24], &band, sizeof(u32));
	lnb_voltage = htonl(state->voltage);
	memcpy(&b[28], &lnb_voltage, sizeof(u32));

	state->config->send_command(fe, 0x71, sizeof(b), b, NULL, NULL);

	return 0;
}

static int ttusbdecfe_dvbs_diseqc_send_master_cmd(struct dvb_frontend* fe, struct dvb_diseqc_master_cmd *cmd)
{
	struct ttusbdecfe_state* state = (struct ttusbdecfe_state*) fe->demodulator_priv;
	u8 b[] = { 0x00, 0xff, 0x00, 0x00,
		   0x00, 0x00, 0x00, 0x00,
		   0x00, 0x00 };

	if (cmd->msg_len > sizeof(b) - 4)
		return -EINVAL;

	memcpy(&b[4], cmd->msg, cmd->msg_len);

	state->config->send_command(fe, 0x72,
				    sizeof(b) - (6 - cmd->msg_len), b,
				    NULL, NULL);

	return 0;
}


static int ttusbdecfe_dvbs_set_tone(struct dvb_frontend *fe,
				    enum fe_sec_tone_mode tone)
{
	struct ttusbdecfe_state* state = (struct ttusbdecfe_state*) fe->demodulator_priv;

	state->hi_band = (SEC_TONE_ON == tone);

	return 0;
}


static int ttusbdecfe_dvbs_set_voltage(struct dvb_frontend *fe,
				       enum fe_sec_voltage voltage)
{
	struct ttusbdecfe_state* state = (struct ttusbdecfe_state*) fe->demodulator_priv;

	switch (voltage) {
	case SEC_VOLTAGE_13:
		state->voltage = 13;
		break;
	case SEC_VOLTAGE_18:
		state->voltage = 18;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static void ttusbdecfe_release(struct dvb_frontend* fe)
{
	struct ttusbdecfe_state* state = (struct ttusbdecfe_state*) fe->demodulator_priv;
	kfree(state);
}

static const struct dvb_frontend_ops ttusbdecfe_dvbt_ops;

struct dvb_frontend* ttusbdecfe_dvbt_attach(const struct ttusbdecfe_config* config)
{
	struct ttusbdecfe_state* state = NULL;

	/* allocate memory for the internal state */
	state = kmalloc(sizeof(struct ttusbdecfe_state), GFP_KERNEL);
	if (state == NULL)
		return NULL;

	/* setup the state */
	state->config = config;

	/* create dvb_frontend */
	memcpy(&state->frontend.ops, &ttusbdecfe_dvbt_ops, sizeof(struct dvb_frontend_ops));
	state->frontend.demodulator_priv = state;
	return &state->frontend;
}

static const struct dvb_frontend_ops ttusbdecfe_dvbs_ops;

struct dvb_frontend* ttusbdecfe_dvbs_attach(const struct ttusbdecfe_config* config)
{
	struct ttusbdecfe_state* state = NULL;

	/* allocate memory for the internal state */
	state = kmalloc(sizeof(struct ttusbdecfe_state), GFP_KERNEL);
	if (state == NULL)
		return NULL;

	/* setup the state */
	state->config = config;
	state->voltage = 0;
	state->hi_band = 0;

	/* create dvb_frontend */
	memcpy(&state->frontend.ops, &ttusbdecfe_dvbs_ops, sizeof(struct dvb_frontend_ops));
	state->frontend.demodulator_priv = state;
	return &state->frontend;
}

static const struct dvb_frontend_ops ttusbdecfe_dvbt_ops = {
	.delsys = { SYS_DVBT },
	.info = {
		.name			= "TechnoTrend/Hauppauge DEC2000-t Frontend",
		.frequency_min_hz	=  51 * MHz,
		.frequency_max_hz	= 858 * MHz,
		.frequency_stepsize_hz	= 62500,
		.caps =	FE_CAN_FEC_1_2 | FE_CAN_FEC_2_3 | FE_CAN_FEC_3_4 |
			FE_CAN_FEC_5_6 | FE_CAN_FEC_7_8 | FE_CAN_FEC_AUTO |
			FE_CAN_QAM_16 | FE_CAN_QAM_64 | FE_CAN_QAM_AUTO |
			FE_CAN_TRANSMISSION_MODE_AUTO | FE_CAN_GUARD_INTERVAL_AUTO |
			FE_CAN_HIERARCHY_AUTO,
	},

	.release = ttusbdecfe_release,

	.set_frontend = ttusbdecfe_dvbt_set_frontend,

	.get_tune_settings = ttusbdecfe_dvbt_get_tune_settings,

	.read_status = ttusbdecfe_dvbt_read_status,
};

static const struct dvb_frontend_ops ttusbdecfe_dvbs_ops = {
	.delsys = { SYS_DVBS },
	.info = {
		.name			= "TechnoTrend/Hauppauge DEC3000-s Frontend",
		.frequency_min_hz	=  950 * MHz,
		.frequency_max_hz	= 2150 * MHz,
		.frequency_stepsize_hz	=  125 * kHz,
		.symbol_rate_min        = 1000000,  /* guessed */
		.symbol_rate_max        = 45000000, /* guessed */
		.caps =	FE_CAN_FEC_1_2 | FE_CAN_FEC_2_3 | FE_CAN_FEC_3_4 |
			FE_CAN_FEC_5_6 | FE_CAN_FEC_7_8 | FE_CAN_FEC_AUTO |
			FE_CAN_QPSK
	},

	.release = ttusbdecfe_release,

	.set_frontend = ttusbdecfe_dvbs_set_frontend,

	.read_status = ttusbdecfe_dvbs_read_status,

	.diseqc_send_master_cmd = ttusbdecfe_dvbs_diseqc_send_master_cmd,
	.set_voltage = ttusbdecfe_dvbs_set_voltage,
	.set_tone = ttusbdecfe_dvbs_set_tone,
};

MODULE_DESCRIPTION("TTUSB DEC DVB-T/S Demodulator driver");
MODULE_AUTHOR("Alex Woods/Andrew de Quincey");
MODULE_LICENSE("GPL");

EXPORT_SYMBOL(ttusbdecfe_dvbt_attach);
EXPORT_SYMBOL(ttusbdecfe_dvbs_attach);
