// SPDX-License-Identifier: GPL-2.0-only
/* DVB frontend part of the Linux driver for TwinhanDTV Alpha/MagicBoxII USB2.0
 * DVB-T receiver.
 *
 * Copyright (C) 2004-5 Patrick Boettcher (patrick.boettcher@posteo.de)
 *
 * Thanks to Twinhan who kindly provided hardware and information.
 *
 * see Documentation/driver-api/media/drivers/dvb-usb.rst for more information
 */
#include "vp7045.h"

/* It is a Zarlink MT352 within a Samsung Tuner (DNOS404ZH102A) - 040929 - AAT
 *
 * Programming is hidden inside the firmware, so set_frontend is very easy.
 * Even though there is a Firmware command that one can use to access the demod
 * via its registers. This is used for status information.
 */

struct vp7045_fe_state {
	struct dvb_frontend fe;
	struct dvb_usb_device *d;
};

static int vp7045_fe_read_status(struct dvb_frontend *fe,
				 enum fe_status *status)
{
	struct vp7045_fe_state *state = fe->demodulator_priv;
	u8 s0 = vp7045_read_reg(state->d,0x00),
	   s1 = vp7045_read_reg(state->d,0x01),
	   s3 = vp7045_read_reg(state->d,0x03);

	*status = 0;
	if (s0 & (1 << 4))
		*status |= FE_HAS_CARRIER;
	if (s0 & (1 << 1))
		*status |= FE_HAS_VITERBI;
	if (s0 & (1 << 5))
		*status |= FE_HAS_LOCK;
	if (s1 & (1 << 1))
		*status |= FE_HAS_SYNC;
	if (s3 & (1 << 6))
		*status |= FE_HAS_SIGNAL;

	if ((*status & (FE_HAS_CARRIER | FE_HAS_VITERBI | FE_HAS_SYNC)) !=
			(FE_HAS_CARRIER | FE_HAS_VITERBI | FE_HAS_SYNC))
		*status &= ~FE_HAS_LOCK;

	return 0;
}

static int vp7045_fe_read_ber(struct dvb_frontend* fe, u32 *ber)
{
	struct vp7045_fe_state *state = fe->demodulator_priv;
	*ber = (vp7045_read_reg(state->d, 0x0D) << 16) |
	       (vp7045_read_reg(state->d, 0x0E) << 8) |
		vp7045_read_reg(state->d, 0x0F);
	return 0;
}

static int vp7045_fe_read_unc_blocks(struct dvb_frontend* fe, u32 *unc)
{
	struct vp7045_fe_state *state = fe->demodulator_priv;
	*unc = (vp7045_read_reg(state->d, 0x10) << 8) |
		    vp7045_read_reg(state->d, 0x11);
	return 0;
}

static int vp7045_fe_read_signal_strength(struct dvb_frontend* fe, u16 *strength)
{
	struct vp7045_fe_state *state = fe->demodulator_priv;
	u16 signal = (vp7045_read_reg(state->d, 0x14) << 8) |
		vp7045_read_reg(state->d, 0x15);

	*strength = ~signal;
	return 0;
}

static int vp7045_fe_read_snr(struct dvb_frontend* fe, u16 *snr)
{
	struct vp7045_fe_state *state = fe->demodulator_priv;
	u8 _snr = vp7045_read_reg(state->d, 0x09);
	*snr = (_snr << 8) | _snr;
	return 0;
}

static int vp7045_fe_init(struct dvb_frontend* fe)
{
	return 0;
}

static int vp7045_fe_sleep(struct dvb_frontend* fe)
{
	return 0;
}

static int vp7045_fe_get_tune_settings(struct dvb_frontend* fe, struct dvb_frontend_tune_settings *tune)
{
	tune->min_delay_ms = 800;
	return 0;
}

static int vp7045_fe_set_frontend(struct dvb_frontend *fe)
{
	struct dtv_frontend_properties *fep = &fe->dtv_property_cache;
	struct vp7045_fe_state *state = fe->demodulator_priv;
	u8 buf[5];
	u32 freq = fep->frequency / 1000;

	buf[0] = (freq >> 16) & 0xff;
	buf[1] = (freq >>  8) & 0xff;
	buf[2] =  freq        & 0xff;
	buf[3] = 0;

	switch (fep->bandwidth_hz) {
	case 8000000:
		buf[4] = 8;
		break;
	case 7000000:
		buf[4] = 7;
		break;
	case 6000000:
		buf[4] = 6;
		break;
	default:
		return -EINVAL;
	}

	vp7045_usb_op(state->d,LOCK_TUNER_COMMAND,buf,5,NULL,0,200);
	return 0;
}

static void vp7045_fe_release(struct dvb_frontend* fe)
{
	struct vp7045_fe_state *state = fe->demodulator_priv;
	kfree(state);
}

static const struct dvb_frontend_ops vp7045_fe_ops;

struct dvb_frontend * vp7045_fe_attach(struct dvb_usb_device *d)
{
	struct vp7045_fe_state *s = kzalloc(sizeof(struct vp7045_fe_state), GFP_KERNEL);
	if (s == NULL)
		goto error;

	s->d = d;
	memcpy(&s->fe.ops, &vp7045_fe_ops, sizeof(struct dvb_frontend_ops));
	s->fe.demodulator_priv = s;

	return &s->fe;
error:
	return NULL;
}


static const struct dvb_frontend_ops vp7045_fe_ops = {
	.delsys = { SYS_DVBT },
	.info = {
		.name			= "Twinhan VP7045/46 USB DVB-T",
		.frequency_min_hz	=  44250 * kHz,
		.frequency_max_hz	= 867250 * kHz,
		.frequency_stepsize_hz	=      1 * kHz,
		.caps = FE_CAN_INVERSION_AUTO |
				FE_CAN_FEC_1_2 | FE_CAN_FEC_2_3 | FE_CAN_FEC_3_4 |
				FE_CAN_FEC_5_6 | FE_CAN_FEC_7_8 | FE_CAN_FEC_AUTO |
				FE_CAN_QPSK | FE_CAN_QAM_16 | FE_CAN_QAM_64 | FE_CAN_QAM_AUTO |
				FE_CAN_TRANSMISSION_MODE_AUTO |
				FE_CAN_GUARD_INTERVAL_AUTO |
				FE_CAN_RECOVER |
				FE_CAN_HIERARCHY_AUTO,
	},

	.release = vp7045_fe_release,

	.init = vp7045_fe_init,
	.sleep = vp7045_fe_sleep,

	.set_frontend = vp7045_fe_set_frontend,
	.get_tune_settings = vp7045_fe_get_tune_settings,

	.read_status = vp7045_fe_read_status,
	.read_ber = vp7045_fe_read_ber,
	.read_signal_strength = vp7045_fe_read_signal_strength,
	.read_snr = vp7045_fe_read_snr,
	.read_ucblocks = vp7045_fe_read_unc_blocks,
};
