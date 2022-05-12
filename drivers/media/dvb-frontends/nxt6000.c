// SPDX-License-Identifier: GPL-2.0-or-later
/*
	NxtWave Communications - NXT6000 demodulator driver

    Copyright (C) 2002-2003 Florian Schirmer <jolt@tuxbox.org>
    Copyright (C) 2003 Paul Andreassen <paul@andreassen.com.au>

*/

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/slab.h>

#include <media/dvb_frontend.h>
#include "nxt6000_priv.h"
#include "nxt6000.h"



struct nxt6000_state {
	struct i2c_adapter* i2c;
	/* configuration settings */
	const struct nxt6000_config* config;
	struct dvb_frontend frontend;
};

static int debug;
#define dprintk(fmt, arg...) do {					\
	if (debug)							\
		printk(KERN_DEBUG pr_fmt("%s: " fmt),			\
		       __func__, ##arg);				\
} while (0)

static int nxt6000_writereg(struct nxt6000_state* state, u8 reg, u8 data)
{
	u8 buf[] = { reg, data };
	struct i2c_msg msg = {.addr = state->config->demod_address,.flags = 0,.buf = buf,.len = 2 };
	int ret;

	if ((ret = i2c_transfer(state->i2c, &msg, 1)) != 1)
		dprintk("nxt6000: nxt6000_write error (reg: 0x%02X, data: 0x%02X, ret: %d)\n", reg, data, ret);

	return (ret != 1) ? -EIO : 0;
}

static u8 nxt6000_readreg(struct nxt6000_state* state, u8 reg)
{
	int ret;
	u8 b0[] = { reg };
	u8 b1[] = { 0 };
	struct i2c_msg msgs[] = {
		{.addr = state->config->demod_address,.flags = 0,.buf = b0,.len = 1},
		{.addr = state->config->demod_address,.flags = I2C_M_RD,.buf = b1,.len = 1}
	};

	ret = i2c_transfer(state->i2c, msgs, 2);

	if (ret != 2)
		dprintk("nxt6000: nxt6000_read error (reg: 0x%02X, ret: %d)\n", reg, ret);

	return b1[0];
}

static void nxt6000_reset(struct nxt6000_state* state)
{
	u8 val;

	val = nxt6000_readreg(state, OFDM_COR_CTL);

	nxt6000_writereg(state, OFDM_COR_CTL, val & ~COREACT);
	nxt6000_writereg(state, OFDM_COR_CTL, val | COREACT);
}

static int nxt6000_set_bandwidth(struct nxt6000_state *state, u32 bandwidth)
{
	u16 nominal_rate;
	int result;

	switch (bandwidth) {
	case 6000000:
		nominal_rate = 0x55B7;
		break;

	case 7000000:
		nominal_rate = 0x6400;
		break;

	case 8000000:
		nominal_rate = 0x7249;
		break;

	default:
		return -EINVAL;
	}

	if ((result = nxt6000_writereg(state, OFDM_TRL_NOMINALRATE_1, nominal_rate & 0xFF)) < 0)
		return result;

	return nxt6000_writereg(state, OFDM_TRL_NOMINALRATE_2, (nominal_rate >> 8) & 0xFF);
}

static int nxt6000_set_guard_interval(struct nxt6000_state *state,
				      enum fe_guard_interval guard_interval)
{
	switch (guard_interval) {

	case GUARD_INTERVAL_1_32:
		return nxt6000_writereg(state, OFDM_COR_MODEGUARD, 0x00 | (nxt6000_readreg(state, OFDM_COR_MODEGUARD) & ~0x03));

	case GUARD_INTERVAL_1_16:
		return nxt6000_writereg(state, OFDM_COR_MODEGUARD, 0x01 | (nxt6000_readreg(state, OFDM_COR_MODEGUARD) & ~0x03));

	case GUARD_INTERVAL_AUTO:
	case GUARD_INTERVAL_1_8:
		return nxt6000_writereg(state, OFDM_COR_MODEGUARD, 0x02 | (nxt6000_readreg(state, OFDM_COR_MODEGUARD) & ~0x03));

	case GUARD_INTERVAL_1_4:
		return nxt6000_writereg(state, OFDM_COR_MODEGUARD, 0x03 | (nxt6000_readreg(state, OFDM_COR_MODEGUARD) & ~0x03));

	default:
		return -EINVAL;
	}
}

static int nxt6000_set_inversion(struct nxt6000_state *state,
				 enum fe_spectral_inversion inversion)
{
	switch (inversion) {

	case INVERSION_OFF:
		return nxt6000_writereg(state, OFDM_ITB_CTL, 0x00);

	case INVERSION_ON:
		return nxt6000_writereg(state, OFDM_ITB_CTL, ITBINV);

	default:
		return -EINVAL;

	}
}

static int
nxt6000_set_transmission_mode(struct nxt6000_state *state,
			      enum fe_transmit_mode transmission_mode)
{
	int result;

	switch (transmission_mode) {

	case TRANSMISSION_MODE_2K:
		if ((result = nxt6000_writereg(state, EN_DMD_RACQ, 0x00 | (nxt6000_readreg(state, EN_DMD_RACQ) & ~0x03))) < 0)
			return result;

		return nxt6000_writereg(state, OFDM_COR_MODEGUARD, (0x00 << 2) | (nxt6000_readreg(state, OFDM_COR_MODEGUARD) & ~0x04));

	case TRANSMISSION_MODE_8K:
	case TRANSMISSION_MODE_AUTO:
		if ((result = nxt6000_writereg(state, EN_DMD_RACQ, 0x02 | (nxt6000_readreg(state, EN_DMD_RACQ) & ~0x03))) < 0)
			return result;

		return nxt6000_writereg(state, OFDM_COR_MODEGUARD, (0x01 << 2) | (nxt6000_readreg(state, OFDM_COR_MODEGUARD) & ~0x04));

	default:
		return -EINVAL;

	}
}

static void nxt6000_setup(struct dvb_frontend* fe)
{
	struct nxt6000_state* state = fe->demodulator_priv;

	nxt6000_writereg(state, RS_COR_SYNC_PARAM, SYNC_PARAM);
	nxt6000_writereg(state, BER_CTRL, /*(1 << 2) | */ (0x01 << 1) | 0x01);
	nxt6000_writereg(state, VIT_BERTIME_2, 0x00);  // BER Timer = 0x000200 * 256 = 131072 bits
	nxt6000_writereg(state, VIT_BERTIME_1, 0x02);  //
	nxt6000_writereg(state, VIT_BERTIME_0, 0x00);  //
	nxt6000_writereg(state, VIT_COR_INTEN, 0x98); // Enable BER interrupts
	nxt6000_writereg(state, VIT_COR_CTL, 0x82);   // Enable BER measurement
	nxt6000_writereg(state, VIT_COR_CTL, VIT_COR_RESYNC | 0x02 );
	nxt6000_writereg(state, OFDM_COR_CTL, (0x01 << 5) | (nxt6000_readreg(state, OFDM_COR_CTL) & 0x0F));
	nxt6000_writereg(state, OFDM_COR_MODEGUARD, FORCEMODE8K | 0x02);
	nxt6000_writereg(state, OFDM_AGC_CTL, AGCLAST | INITIAL_AGC_BW);
	nxt6000_writereg(state, OFDM_ITB_FREQ_1, 0x06);
	nxt6000_writereg(state, OFDM_ITB_FREQ_2, 0x31);
	nxt6000_writereg(state, OFDM_CAS_CTL, (0x01 << 7) | (0x02 << 3) | 0x04);
	nxt6000_writereg(state, CAS_FREQ, 0xBB);	/* CHECKME */
	nxt6000_writereg(state, OFDM_SYR_CTL, 1 << 2);
	nxt6000_writereg(state, OFDM_PPM_CTL_1, PPM256);
	nxt6000_writereg(state, OFDM_TRL_NOMINALRATE_1, 0x49);
	nxt6000_writereg(state, OFDM_TRL_NOMINALRATE_2, 0x72);
	nxt6000_writereg(state, ANALOG_CONTROL_0, 1 << 5);
	nxt6000_writereg(state, EN_DMD_RACQ, (1 << 7) | (3 << 4) | 2);
	nxt6000_writereg(state, DIAG_CONFIG, TB_SET);

	if (state->config->clock_inversion)
		nxt6000_writereg(state, SUB_DIAG_MODE_SEL, CLKINVERSION);
	else
		nxt6000_writereg(state, SUB_DIAG_MODE_SEL, 0);

	nxt6000_writereg(state, TS_FORMAT, 0);
}

static void nxt6000_dump_status(struct nxt6000_state *state)
{
	u8 val;

#if 0
	pr_info("RS_COR_STAT: 0x%02X\n",
		nxt6000_readreg(fe, RS_COR_STAT));
	pr_info("VIT_SYNC_STATUS: 0x%02X\n",
		nxt6000_readreg(fe, VIT_SYNC_STATUS));
	pr_info("OFDM_COR_STAT: 0x%02X\n",
		nxt6000_readreg(fe, OFDM_COR_STAT));
	pr_info("OFDM_SYR_STAT: 0x%02X\n",
		nxt6000_readreg(fe, OFDM_SYR_STAT));
	pr_info("OFDM_TPS_RCVD_1: 0x%02X\n",
		nxt6000_readreg(fe, OFDM_TPS_RCVD_1));
	pr_info("OFDM_TPS_RCVD_2: 0x%02X\n",
		nxt6000_readreg(fe, OFDM_TPS_RCVD_2));
	pr_info("OFDM_TPS_RCVD_3: 0x%02X\n",
		nxt6000_readreg(fe, OFDM_TPS_RCVD_3));
	pr_info("OFDM_TPS_RCVD_4: 0x%02X\n",
		nxt6000_readreg(fe, OFDM_TPS_RCVD_4));
	pr_info("OFDM_TPS_RESERVED_1: 0x%02X\n",
		nxt6000_readreg(fe, OFDM_TPS_RESERVED_1));
	pr_info("OFDM_TPS_RESERVED_2: 0x%02X\n",
		nxt6000_readreg(fe, OFDM_TPS_RESERVED_2));
#endif
	pr_info("NXT6000 status:");

	val = nxt6000_readreg(state, RS_COR_STAT);

	pr_cont(" DATA DESCR LOCK: %d,", val & 0x01);
	pr_cont(" DATA SYNC LOCK: %d,", (val >> 1) & 0x01);

	val = nxt6000_readreg(state, VIT_SYNC_STATUS);

	pr_cont(" VITERBI LOCK: %d,", (val >> 7) & 0x01);

	switch ((val >> 4) & 0x07) {

	case 0x00:
		pr_cont(" VITERBI CODERATE: 1/2,");
		break;

	case 0x01:
		pr_cont(" VITERBI CODERATE: 2/3,");
		break;

	case 0x02:
		pr_cont(" VITERBI CODERATE: 3/4,");
		break;

	case 0x03:
		pr_cont(" VITERBI CODERATE: 5/6,");
		break;

	case 0x04:
		pr_cont(" VITERBI CODERATE: 7/8,");
		break;

	default:
		pr_cont(" VITERBI CODERATE: Reserved,");

	}

	val = nxt6000_readreg(state, OFDM_COR_STAT);

	pr_cont(" CHCTrack: %d,", (val >> 7) & 0x01);
	pr_cont(" TPSLock: %d,", (val >> 6) & 0x01);
	pr_cont(" SYRLock: %d,", (val >> 5) & 0x01);
	pr_cont(" AGCLock: %d,", (val >> 4) & 0x01);

	switch (val & 0x0F) {

	case 0x00:
		pr_cont(" CoreState: IDLE,");
		break;

	case 0x02:
		pr_cont(" CoreState: WAIT_AGC,");
		break;

	case 0x03:
		pr_cont(" CoreState: WAIT_SYR,");
		break;

	case 0x04:
		pr_cont(" CoreState: WAIT_PPM,");
		break;

	case 0x01:
		pr_cont(" CoreState: WAIT_TRL,");
		break;

	case 0x05:
		pr_cont(" CoreState: WAIT_TPS,");
		break;

	case 0x06:
		pr_cont(" CoreState: MONITOR_TPS,");
		break;

	default:
		pr_cont(" CoreState: Reserved,");

	}

	val = nxt6000_readreg(state, OFDM_SYR_STAT);

	pr_cont(" SYRLock: %d,", (val >> 4) & 0x01);
	pr_cont(" SYRMode: %s,", (val >> 2) & 0x01 ? "8K" : "2K");

	switch ((val >> 4) & 0x03) {

	case 0x00:
		pr_cont(" SYRGuard: 1/32,");
		break;

	case 0x01:
		pr_cont(" SYRGuard: 1/16,");
		break;

	case 0x02:
		pr_cont(" SYRGuard: 1/8,");
		break;

	case 0x03:
		pr_cont(" SYRGuard: 1/4,");
		break;
	}

	val = nxt6000_readreg(state, OFDM_TPS_RCVD_3);

	switch ((val >> 4) & 0x07) {

	case 0x00:
		pr_cont(" TPSLP: 1/2,");
		break;

	case 0x01:
		pr_cont(" TPSLP: 2/3,");
		break;

	case 0x02:
		pr_cont(" TPSLP: 3/4,");
		break;

	case 0x03:
		pr_cont(" TPSLP: 5/6,");
		break;

	case 0x04:
		pr_cont(" TPSLP: 7/8,");
		break;

	default:
		pr_cont(" TPSLP: Reserved,");

	}

	switch (val & 0x07) {

	case 0x00:
		pr_cont(" TPSHP: 1/2,");
		break;

	case 0x01:
		pr_cont(" TPSHP: 2/3,");
		break;

	case 0x02:
		pr_cont(" TPSHP: 3/4,");
		break;

	case 0x03:
		pr_cont(" TPSHP: 5/6,");
		break;

	case 0x04:
		pr_cont(" TPSHP: 7/8,");
		break;

	default:
		pr_cont(" TPSHP: Reserved,");

	}

	val = nxt6000_readreg(state, OFDM_TPS_RCVD_4);

	pr_cont(" TPSMode: %s,", val & 0x01 ? "8K" : "2K");

	switch ((val >> 4) & 0x03) {

	case 0x00:
		pr_cont(" TPSGuard: 1/32,");
		break;

	case 0x01:
		pr_cont(" TPSGuard: 1/16,");
		break;

	case 0x02:
		pr_cont(" TPSGuard: 1/8,");
		break;

	case 0x03:
		pr_cont(" TPSGuard: 1/4,");
		break;

	}

	/* Strange magic required to gain access to RF_AGC_STATUS */
	nxt6000_readreg(state, RF_AGC_VAL_1);
	val = nxt6000_readreg(state, RF_AGC_STATUS);
	val = nxt6000_readreg(state, RF_AGC_STATUS);

	pr_cont(" RF AGC LOCK: %d,", (val >> 4) & 0x01);
	pr_cont("\n");
}

static int nxt6000_read_status(struct dvb_frontend *fe, enum fe_status *status)
{
	u8 core_status;
	struct nxt6000_state* state = fe->demodulator_priv;

	*status = 0;

	core_status = nxt6000_readreg(state, OFDM_COR_STAT);

	if (core_status & AGCLOCKED)
		*status |= FE_HAS_SIGNAL;

	if (nxt6000_readreg(state, OFDM_SYR_STAT) & GI14_SYR_LOCK)
		*status |= FE_HAS_CARRIER;

	if (nxt6000_readreg(state, VIT_SYNC_STATUS) & VITINSYNC)
		*status |= FE_HAS_VITERBI;

	if (nxt6000_readreg(state, RS_COR_STAT) & RSCORESTATUS)
		*status |= FE_HAS_SYNC;

	if ((core_status & TPSLOCKED) && (*status == (FE_HAS_SIGNAL | FE_HAS_CARRIER | FE_HAS_VITERBI | FE_HAS_SYNC)))
		*status |= FE_HAS_LOCK;

	if (debug)
		nxt6000_dump_status(state);

	return 0;
}

static int nxt6000_init(struct dvb_frontend* fe)
{
	struct nxt6000_state* state = fe->demodulator_priv;

	nxt6000_reset(state);
	nxt6000_setup(fe);

	return 0;
}

static int nxt6000_set_frontend(struct dvb_frontend *fe)
{
	struct dtv_frontend_properties *p = &fe->dtv_property_cache;
	struct nxt6000_state* state = fe->demodulator_priv;
	int result;

	if (fe->ops.tuner_ops.set_params) {
		fe->ops.tuner_ops.set_params(fe);
		if (fe->ops.i2c_gate_ctrl) fe->ops.i2c_gate_ctrl(fe, 0);
	}

	result = nxt6000_set_bandwidth(state, p->bandwidth_hz);
	if (result < 0)
		return result;

	result = nxt6000_set_guard_interval(state, p->guard_interval);
	if (result < 0)
		return result;

	result = nxt6000_set_transmission_mode(state, p->transmission_mode);
	if (result < 0)
		return result;

	result = nxt6000_set_inversion(state, p->inversion);
	if (result < 0)
		return result;

	msleep(500);
	return 0;
}

static void nxt6000_release(struct dvb_frontend* fe)
{
	struct nxt6000_state* state = fe->demodulator_priv;
	kfree(state);
}

static int nxt6000_read_snr(struct dvb_frontend* fe, u16* snr)
{
	struct nxt6000_state* state = fe->demodulator_priv;

	*snr = nxt6000_readreg( state, OFDM_CHC_SNR) / 8;

	return 0;
}

static int nxt6000_read_ber(struct dvb_frontend* fe, u32* ber)
{
	struct nxt6000_state* state = fe->demodulator_priv;

	nxt6000_writereg( state, VIT_COR_INTSTAT, 0x18 );

	*ber = (nxt6000_readreg( state, VIT_BER_1 ) << 8 ) |
		nxt6000_readreg( state, VIT_BER_0 );

	nxt6000_writereg( state, VIT_COR_INTSTAT, 0x18); // Clear BER Done interrupts

	return 0;
}

static int nxt6000_read_signal_strength(struct dvb_frontend* fe, u16* signal_strength)
{
	struct nxt6000_state* state = fe->demodulator_priv;

	*signal_strength = (short) (511 -
		(nxt6000_readreg(state, AGC_GAIN_1) +
		((nxt6000_readreg(state, AGC_GAIN_2) & 0x03) << 8)));

	return 0;
}

static int nxt6000_fe_get_tune_settings(struct dvb_frontend* fe, struct dvb_frontend_tune_settings *tune)
{
	tune->min_delay_ms = 500;
	return 0;
}

static int nxt6000_i2c_gate_ctrl(struct dvb_frontend* fe, int enable)
{
	struct nxt6000_state* state = fe->demodulator_priv;

	if (enable) {
		return nxt6000_writereg(state, ENABLE_TUNER_IIC, 0x01);
	} else {
		return nxt6000_writereg(state, ENABLE_TUNER_IIC, 0x00);
	}
}

static const struct dvb_frontend_ops nxt6000_ops;

struct dvb_frontend* nxt6000_attach(const struct nxt6000_config* config,
				    struct i2c_adapter* i2c)
{
	struct nxt6000_state* state = NULL;

	/* allocate memory for the internal state */
	state = kzalloc(sizeof(struct nxt6000_state), GFP_KERNEL);
	if (state == NULL) goto error;

	/* setup the state */
	state->config = config;
	state->i2c = i2c;

	/* check if the demod is there */
	if (nxt6000_readreg(state, OFDM_MSC_REV) != NXT6000ASICDEVICE) goto error;

	/* create dvb_frontend */
	memcpy(&state->frontend.ops, &nxt6000_ops, sizeof(struct dvb_frontend_ops));
	state->frontend.demodulator_priv = state;
	return &state->frontend;

error:
	kfree(state);
	return NULL;
}

static const struct dvb_frontend_ops nxt6000_ops = {
	.delsys = { SYS_DVBT },
	.info = {
		.name = "NxtWave NXT6000 DVB-T",
		.frequency_min_hz = 0,
		.frequency_max_hz = 863250 * kHz,
		.frequency_stepsize_hz = 62500,
		/*.frequency_tolerance = *//* FIXME: 12% of SR */
		.symbol_rate_min = 0,	/* FIXME */
		.symbol_rate_max = 9360000,	/* FIXME */
		.symbol_rate_tolerance = 4000,
		.caps = FE_CAN_FEC_1_2 | FE_CAN_FEC_2_3 | FE_CAN_FEC_3_4 |
			FE_CAN_FEC_4_5 | FE_CAN_FEC_5_6 | FE_CAN_FEC_6_7 |
			FE_CAN_FEC_7_8 | FE_CAN_FEC_8_9 | FE_CAN_FEC_AUTO |
			FE_CAN_QAM_16 | FE_CAN_QAM_64 | FE_CAN_QAM_AUTO |
			FE_CAN_TRANSMISSION_MODE_AUTO | FE_CAN_GUARD_INTERVAL_AUTO |
			FE_CAN_HIERARCHY_AUTO,
	},

	.release = nxt6000_release,

	.init = nxt6000_init,
	.i2c_gate_ctrl = nxt6000_i2c_gate_ctrl,

	.get_tune_settings = nxt6000_fe_get_tune_settings,

	.set_frontend = nxt6000_set_frontend,

	.read_status = nxt6000_read_status,
	.read_ber = nxt6000_read_ber,
	.read_signal_strength = nxt6000_read_signal_strength,
	.read_snr = nxt6000_read_snr,
};

module_param(debug, int, 0644);
MODULE_PARM_DESC(debug, "Turn on/off frontend debugging (default:off).");

MODULE_DESCRIPTION("NxtWave NXT6000 DVB-T demodulator driver");
MODULE_AUTHOR("Florian Schirmer");
MODULE_LICENSE("GPL");

EXPORT_SYMBOL(nxt6000_attach);
