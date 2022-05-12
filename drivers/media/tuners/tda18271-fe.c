// SPDX-License-Identifier: GPL-2.0-or-later
/*
    tda18271-fe.c - driver for the Philips / NXP TDA18271 silicon tuner

    Copyright (C) 2007, 2008 Michael Krufky <mkrufky@linuxtv.org>

*/

#include "tda18271-priv.h"
#include "tda8290.h"

#include <linux/delay.h>
#include <linux/videodev2.h>

int tda18271_debug;
module_param_named(debug, tda18271_debug, int, 0644);
MODULE_PARM_DESC(debug, "set debug level (info=1, map=2, reg=4, adv=8, cal=16 (or-able))");

static int tda18271_cal_on_startup = -1;
module_param_named(cal, tda18271_cal_on_startup, int, 0644);
MODULE_PARM_DESC(cal, "perform RF tracking filter calibration on startup");

static DEFINE_MUTEX(tda18271_list_mutex);
static LIST_HEAD(hybrid_tuner_instance_list);

/*---------------------------------------------------------------------*/

static int tda18271_toggle_output(struct dvb_frontend *fe, int standby)
{
	struct tda18271_priv *priv = fe->tuner_priv;

	int ret = tda18271_set_standby_mode(fe, standby ? 1 : 0,
			priv->output_opt & TDA18271_OUTPUT_LT_OFF ? 1 : 0,
			priv->output_opt & TDA18271_OUTPUT_XT_OFF ? 1 : 0);

	if (tda_fail(ret))
		goto fail;

	tda_dbg("%s mode: xtal oscillator %s, slave tuner loop through %s\n",
		standby ? "standby" : "active",
		priv->output_opt & TDA18271_OUTPUT_XT_OFF ? "off" : "on",
		priv->output_opt & TDA18271_OUTPUT_LT_OFF ? "off" : "on");
fail:
	return ret;
}

/*---------------------------------------------------------------------*/

static inline int charge_pump_source(struct dvb_frontend *fe, int force)
{
	struct tda18271_priv *priv = fe->tuner_priv;
	return tda18271_charge_pump_source(fe,
					   (priv->role == TDA18271_SLAVE) ?
					   TDA18271_CAL_PLL :
					   TDA18271_MAIN_PLL, force);
}

static inline void tda18271_set_if_notch(struct dvb_frontend *fe)
{
	struct tda18271_priv *priv = fe->tuner_priv;
	unsigned char *regs = priv->tda18271_regs;

	switch (priv->mode) {
	case TDA18271_ANALOG:
		regs[R_MPD]  &= ~0x80; /* IF notch = 0 */
		break;
	case TDA18271_DIGITAL:
		regs[R_MPD]  |= 0x80; /* IF notch = 1 */
		break;
	}
}

static int tda18271_channel_configuration(struct dvb_frontend *fe,
					  struct tda18271_std_map_item *map,
					  u32 freq, u32 bw)
{
	struct tda18271_priv *priv = fe->tuner_priv;
	unsigned char *regs = priv->tda18271_regs;
	int ret;
	u32 N;

	/* update TV broadcast parameters */

	/* set standard */
	regs[R_EP3]  &= ~0x1f; /* clear std bits */
	regs[R_EP3]  |= (map->agc_mode << 3) | map->std;

	if (priv->id == TDA18271HDC2) {
		/* set rfagc to high speed mode */
		regs[R_EP3] &= ~0x04;
	}

	/* set cal mode to normal */
	regs[R_EP4]  &= ~0x03;

	/* update IF output level */
	regs[R_EP4]  &= ~0x1c; /* clear if level bits */
	regs[R_EP4]  |= (map->if_lvl << 2);

	/* update FM_RFn */
	regs[R_EP4]  &= ~0x80;
	regs[R_EP4]  |= map->fm_rfn << 7;

	/* update rf top / if top */
	regs[R_EB22]  = 0x00;
	regs[R_EB22] |= map->rfagc_top;
	ret = tda18271_write_regs(fe, R_EB22, 1);
	if (tda_fail(ret))
		goto fail;

	/* --------------------------------------------------------------- */

	/* disable Power Level Indicator */
	regs[R_EP1]  |= 0x40;

	/* make sure thermometer is off */
	regs[R_TM]   &= ~0x10;

	/* frequency dependent parameters */

	tda18271_calc_ir_measure(fe, &freq);

	tda18271_calc_bp_filter(fe, &freq);

	tda18271_calc_rf_band(fe, &freq);

	tda18271_calc_gain_taper(fe, &freq);

	/* --------------------------------------------------------------- */

	/* dual tuner and agc1 extra configuration */

	switch (priv->role) {
	case TDA18271_MASTER:
		regs[R_EB1]  |= 0x04; /* main vco */
		break;
	case TDA18271_SLAVE:
		regs[R_EB1]  &= ~0x04; /* cal vco */
		break;
	}

	/* agc1 always active */
	regs[R_EB1]  &= ~0x02;

	/* agc1 has priority on agc2 */
	regs[R_EB1]  &= ~0x01;

	ret = tda18271_write_regs(fe, R_EB1, 1);
	if (tda_fail(ret))
		goto fail;

	/* --------------------------------------------------------------- */

	N = map->if_freq * 1000 + freq;

	switch (priv->role) {
	case TDA18271_MASTER:
		tda18271_calc_main_pll(fe, N);
		tda18271_set_if_notch(fe);
		tda18271_write_regs(fe, R_MPD, 4);
		break;
	case TDA18271_SLAVE:
		tda18271_calc_cal_pll(fe, N);
		tda18271_write_regs(fe, R_CPD, 4);

		regs[R_MPD] = regs[R_CPD] & 0x7f;
		tda18271_set_if_notch(fe);
		tda18271_write_regs(fe, R_MPD, 1);
		break;
	}

	ret = tda18271_write_regs(fe, R_TM, 7);
	if (tda_fail(ret))
		goto fail;

	/* force charge pump source */
	charge_pump_source(fe, 1);

	msleep(1);

	/* return pll to normal operation */
	charge_pump_source(fe, 0);

	msleep(20);

	if (priv->id == TDA18271HDC2) {
		/* set rfagc to normal speed mode */
		if (map->fm_rfn)
			regs[R_EP3] &= ~0x04;
		else
			regs[R_EP3] |= 0x04;
		ret = tda18271_write_regs(fe, R_EP3, 1);
	}
fail:
	return ret;
}

static int tda18271_read_thermometer(struct dvb_frontend *fe)
{
	struct tda18271_priv *priv = fe->tuner_priv;
	unsigned char *regs = priv->tda18271_regs;
	int tm;

	/* switch thermometer on */
	regs[R_TM]   |= 0x10;
	tda18271_write_regs(fe, R_TM, 1);

	/* read thermometer info */
	tda18271_read_regs(fe);

	if ((((regs[R_TM] & 0x0f) == 0x00) && ((regs[R_TM] & 0x20) == 0x20)) ||
	    (((regs[R_TM] & 0x0f) == 0x08) && ((regs[R_TM] & 0x20) == 0x00))) {

		if ((regs[R_TM] & 0x20) == 0x20)
			regs[R_TM] &= ~0x20;
		else
			regs[R_TM] |= 0x20;

		tda18271_write_regs(fe, R_TM, 1);

		msleep(10); /* temperature sensing */

		/* read thermometer info */
		tda18271_read_regs(fe);
	}

	tm = tda18271_lookup_thermometer(fe);

	/* switch thermometer off */
	regs[R_TM]   &= ~0x10;
	tda18271_write_regs(fe, R_TM, 1);

	/* set CAL mode to normal */
	regs[R_EP4]  &= ~0x03;
	tda18271_write_regs(fe, R_EP4, 1);

	return tm;
}

/* ------------------------------------------------------------------ */

static int tda18271c2_rf_tracking_filters_correction(struct dvb_frontend *fe,
						     u32 freq)
{
	struct tda18271_priv *priv = fe->tuner_priv;
	struct tda18271_rf_tracking_filter_cal *map = priv->rf_cal_state;
	unsigned char *regs = priv->tda18271_regs;
	int i, ret;
	u8 tm_current, dc_over_dt, rf_tab;
	s32 rfcal_comp, approx;

	/* power up */
	ret = tda18271_set_standby_mode(fe, 0, 0, 0);
	if (tda_fail(ret))
		goto fail;

	/* read die current temperature */
	tm_current = tda18271_read_thermometer(fe);

	/* frequency dependent parameters */

	tda18271_calc_rf_cal(fe, &freq);
	rf_tab = regs[R_EB14];

	i = tda18271_lookup_rf_band(fe, &freq, NULL);
	if (tda_fail(i))
		return i;

	if ((0 == map[i].rf3) || (freq / 1000 < map[i].rf2)) {
		approx = map[i].rf_a1 * (s32)(freq / 1000 - map[i].rf1) +
			map[i].rf_b1 + rf_tab;
	} else {
		approx = map[i].rf_a2 * (s32)(freq / 1000 - map[i].rf2) +
			map[i].rf_b2 + rf_tab;
	}

	if (approx < 0)
		approx = 0;
	if (approx > 255)
		approx = 255;

	tda18271_lookup_map(fe, RF_CAL_DC_OVER_DT, &freq, &dc_over_dt);

	/* calculate temperature compensation */
	rfcal_comp = dc_over_dt * (s32)(tm_current - priv->tm_rfcal) / 1000;

	regs[R_EB14] = (unsigned char)(approx + rfcal_comp);
	ret = tda18271_write_regs(fe, R_EB14, 1);
fail:
	return ret;
}

static int tda18271_por(struct dvb_frontend *fe)
{
	struct tda18271_priv *priv = fe->tuner_priv;
	unsigned char *regs = priv->tda18271_regs;
	int ret;

	/* power up detector 1 */
	regs[R_EB12] &= ~0x20;
	ret = tda18271_write_regs(fe, R_EB12, 1);
	if (tda_fail(ret))
		goto fail;

	regs[R_EB18] &= ~0x80; /* turn agc1 loop on */
	regs[R_EB18] &= ~0x03; /* set agc1_gain to  6 dB */
	ret = tda18271_write_regs(fe, R_EB18, 1);
	if (tda_fail(ret))
		goto fail;

	regs[R_EB21] |= 0x03; /* set agc2_gain to -6 dB */

	/* POR mode */
	ret = tda18271_set_standby_mode(fe, 1, 0, 0);
	if (tda_fail(ret))
		goto fail;

	/* disable 1.5 MHz low pass filter */
	regs[R_EB23] &= ~0x04; /* forcelp_fc2_en = 0 */
	regs[R_EB23] &= ~0x02; /* XXX: lp_fc[2] = 0 */
	ret = tda18271_write_regs(fe, R_EB21, 3);
fail:
	return ret;
}

static int tda18271_calibrate_rf(struct dvb_frontend *fe, u32 freq)
{
	struct tda18271_priv *priv = fe->tuner_priv;
	unsigned char *regs = priv->tda18271_regs;
	u32 N;

	/* set CAL mode to normal */
	regs[R_EP4]  &= ~0x03;
	tda18271_write_regs(fe, R_EP4, 1);

	/* switch off agc1 */
	regs[R_EP3]  |= 0x40; /* sm_lt = 1 */

	regs[R_EB18] |= 0x03; /* set agc1_gain to 15 dB */
	tda18271_write_regs(fe, R_EB18, 1);

	/* frequency dependent parameters */

	tda18271_calc_bp_filter(fe, &freq);
	tda18271_calc_gain_taper(fe, &freq);
	tda18271_calc_rf_band(fe, &freq);
	tda18271_calc_km(fe, &freq);

	tda18271_write_regs(fe, R_EP1, 3);
	tda18271_write_regs(fe, R_EB13, 1);

	/* main pll charge pump source */
	tda18271_charge_pump_source(fe, TDA18271_MAIN_PLL, 1);

	/* cal pll charge pump source */
	tda18271_charge_pump_source(fe, TDA18271_CAL_PLL, 1);

	/* force dcdc converter to 0 V */
	regs[R_EB14] = 0x00;
	tda18271_write_regs(fe, R_EB14, 1);

	/* disable plls lock */
	regs[R_EB20] &= ~0x20;
	tda18271_write_regs(fe, R_EB20, 1);

	/* set CAL mode to RF tracking filter calibration */
	regs[R_EP4]  |= 0x03;
	tda18271_write_regs(fe, R_EP4, 2);

	/* --------------------------------------------------------------- */

	/* set the internal calibration signal */
	N = freq;

	tda18271_calc_cal_pll(fe, N);
	tda18271_write_regs(fe, R_CPD, 4);

	/* downconvert internal calibration */
	N += 1000000;

	tda18271_calc_main_pll(fe, N);
	tda18271_write_regs(fe, R_MPD, 4);

	msleep(5);

	tda18271_write_regs(fe, R_EP2, 1);
	tda18271_write_regs(fe, R_EP1, 1);
	tda18271_write_regs(fe, R_EP2, 1);
	tda18271_write_regs(fe, R_EP1, 1);

	/* --------------------------------------------------------------- */

	/* normal operation for the main pll */
	tda18271_charge_pump_source(fe, TDA18271_MAIN_PLL, 0);

	/* normal operation for the cal pll  */
	tda18271_charge_pump_source(fe, TDA18271_CAL_PLL, 0);

	msleep(10); /* plls locking */

	/* launch the rf tracking filters calibration */
	regs[R_EB20]  |= 0x20;
	tda18271_write_regs(fe, R_EB20, 1);

	msleep(60); /* calibration */

	/* --------------------------------------------------------------- */

	/* set CAL mode to normal */
	regs[R_EP4]  &= ~0x03;

	/* switch on agc1 */
	regs[R_EP3]  &= ~0x40; /* sm_lt = 0 */

	regs[R_EB18] &= ~0x03; /* set agc1_gain to  6 dB */
	tda18271_write_regs(fe, R_EB18, 1);

	tda18271_write_regs(fe, R_EP3, 2);

	/* synchronization */
	tda18271_write_regs(fe, R_EP1, 1);

	/* get calibration result */
	tda18271_read_extended(fe);

	return regs[R_EB14];
}

static int tda18271_powerscan(struct dvb_frontend *fe,
			      u32 *freq_in, u32 *freq_out)
{
	struct tda18271_priv *priv = fe->tuner_priv;
	unsigned char *regs = priv->tda18271_regs;
	int sgn, bcal, count, wait, ret;
	u8 cid_target;
	u16 count_limit;
	u32 freq;

	freq = *freq_in;

	tda18271_calc_rf_band(fe, &freq);
	tda18271_calc_rf_cal(fe, &freq);
	tda18271_calc_gain_taper(fe, &freq);
	tda18271_lookup_cid_target(fe, &freq, &cid_target, &count_limit);

	tda18271_write_regs(fe, R_EP2, 1);
	tda18271_write_regs(fe, R_EB14, 1);

	/* downconvert frequency */
	freq += 1000000;

	tda18271_calc_main_pll(fe, freq);
	tda18271_write_regs(fe, R_MPD, 4);

	msleep(5); /* pll locking */

	/* detection mode */
	regs[R_EP4]  &= ~0x03;
	regs[R_EP4]  |= 0x01;
	tda18271_write_regs(fe, R_EP4, 1);

	/* launch power detection measurement */
	tda18271_write_regs(fe, R_EP2, 1);

	/* read power detection info, stored in EB10 */
	ret = tda18271_read_extended(fe);
	if (tda_fail(ret))
		return ret;

	/* algorithm initialization */
	sgn = 1;
	*freq_out = *freq_in;
	bcal = 0;
	count = 0;
	wait = false;

	while ((regs[R_EB10] & 0x3f) < cid_target) {
		/* downconvert updated freq to 1 MHz */
		freq = *freq_in + (sgn * count) + 1000000;

		tda18271_calc_main_pll(fe, freq);
		tda18271_write_regs(fe, R_MPD, 4);

		if (wait) {
			msleep(5); /* pll locking */
			wait = false;
		} else
			udelay(100); /* pll locking */

		/* launch power detection measurement */
		tda18271_write_regs(fe, R_EP2, 1);

		/* read power detection info, stored in EB10 */
		ret = tda18271_read_extended(fe);
		if (tda_fail(ret))
			return ret;

		count += 200;

		if (count <= count_limit)
			continue;

		if (sgn <= 0)
			break;

		sgn = -1 * sgn;
		count = 200;
		wait = true;
	}

	if ((regs[R_EB10] & 0x3f) >= cid_target) {
		bcal = 1;
		*freq_out = freq - 1000000;
	} else
		bcal = 0;

	tda_cal("bcal = %d, freq_in = %d, freq_out = %d (freq = %d)\n",
		bcal, *freq_in, *freq_out, freq);

	return bcal;
}

static int tda18271_powerscan_init(struct dvb_frontend *fe)
{
	struct tda18271_priv *priv = fe->tuner_priv;
	unsigned char *regs = priv->tda18271_regs;
	int ret;

	/* set standard to digital */
	regs[R_EP3]  &= ~0x1f; /* clear std bits */
	regs[R_EP3]  |= 0x12;

	/* set cal mode to normal */
	regs[R_EP4]  &= ~0x03;

	/* update IF output level */
	regs[R_EP4]  &= ~0x1c; /* clear if level bits */

	ret = tda18271_write_regs(fe, R_EP3, 2);
	if (tda_fail(ret))
		goto fail;

	regs[R_EB18] &= ~0x03; /* set agc1_gain to   6 dB */
	ret = tda18271_write_regs(fe, R_EB18, 1);
	if (tda_fail(ret))
		goto fail;

	regs[R_EB21] &= ~0x03; /* set agc2_gain to -15 dB */

	/* 1.5 MHz low pass filter */
	regs[R_EB23] |= 0x04; /* forcelp_fc2_en = 1 */
	regs[R_EB23] |= 0x02; /* lp_fc[2] = 1 */

	ret = tda18271_write_regs(fe, R_EB21, 3);
fail:
	return ret;
}

static int tda18271_rf_tracking_filters_init(struct dvb_frontend *fe, u32 freq)
{
	struct tda18271_priv *priv = fe->tuner_priv;
	struct tda18271_rf_tracking_filter_cal *map = priv->rf_cal_state;
	unsigned char *regs = priv->tda18271_regs;
	int bcal, rf, i;
	s32 divisor, dividend;
#define RF1 0
#define RF2 1
#define RF3 2
	u32 rf_default[3];
	u32 rf_freq[3];
	s32 prog_cal[3];
	s32 prog_tab[3];

	i = tda18271_lookup_rf_band(fe, &freq, NULL);

	if (tda_fail(i))
		return i;

	rf_default[RF1] = 1000 * map[i].rf1_def;
	rf_default[RF2] = 1000 * map[i].rf2_def;
	rf_default[RF3] = 1000 * map[i].rf3_def;

	for (rf = RF1; rf <= RF3; rf++) {
		if (0 == rf_default[rf])
			return 0;
		tda_cal("freq = %d, rf = %d\n", freq, rf);

		/* look for optimized calibration frequency */
		bcal = tda18271_powerscan(fe, &rf_default[rf], &rf_freq[rf]);
		if (tda_fail(bcal))
			return bcal;

		tda18271_calc_rf_cal(fe, &rf_freq[rf]);
		prog_tab[rf] = (s32)regs[R_EB14];

		if (1 == bcal)
			prog_cal[rf] =
				(s32)tda18271_calibrate_rf(fe, rf_freq[rf]);
		else
			prog_cal[rf] = prog_tab[rf];

		switch (rf) {
		case RF1:
			map[i].rf_a1 = 0;
			map[i].rf_b1 = (prog_cal[RF1] - prog_tab[RF1]);
			map[i].rf1   = rf_freq[RF1] / 1000;
			break;
		case RF2:
			dividend = (prog_cal[RF2] - prog_tab[RF2] -
				    prog_cal[RF1] + prog_tab[RF1]);
			divisor = (s32)(rf_freq[RF2] - rf_freq[RF1]) / 1000;
			map[i].rf_a1 = (dividend / divisor);
			map[i].rf2   = rf_freq[RF2] / 1000;
			break;
		case RF3:
			dividend = (prog_cal[RF3] - prog_tab[RF3] -
				    prog_cal[RF2] + prog_tab[RF2]);
			divisor = (s32)(rf_freq[RF3] - rf_freq[RF2]) / 1000;
			map[i].rf_a2 = (dividend / divisor);
			map[i].rf_b2 = (prog_cal[RF2] - prog_tab[RF2]);
			map[i].rf3   = rf_freq[RF3] / 1000;
			break;
		default:
			BUG();
		}
	}

	return 0;
}

static int tda18271_calc_rf_filter_curve(struct dvb_frontend *fe)
{
	struct tda18271_priv *priv = fe->tuner_priv;
	unsigned int i;
	int ret;

	tda_info("performing RF tracking filter calibration\n");

	/* wait for die temperature stabilization */
	msleep(200);

	ret = tda18271_powerscan_init(fe);
	if (tda_fail(ret))
		goto fail;

	/* rf band calibration */
	for (i = 0; priv->rf_cal_state[i].rfmax != 0; i++) {
		ret =
		tda18271_rf_tracking_filters_init(fe, 1000 *
						  priv->rf_cal_state[i].rfmax);
		if (tda_fail(ret))
			goto fail;
	}

	priv->tm_rfcal = tda18271_read_thermometer(fe);
fail:
	return ret;
}

/* ------------------------------------------------------------------ */

static int tda18271c2_rf_cal_init(struct dvb_frontend *fe)
{
	struct tda18271_priv *priv = fe->tuner_priv;
	unsigned char *regs = priv->tda18271_regs;
	int ret;

	/* test RF_CAL_OK to see if we need init */
	if ((regs[R_EP1] & 0x10) == 0)
		priv->cal_initialized = false;

	if (priv->cal_initialized)
		return 0;

	ret = tda18271_calc_rf_filter_curve(fe);
	if (tda_fail(ret))
		goto fail;

	ret = tda18271_por(fe);
	if (tda_fail(ret))
		goto fail;

	tda_info("RF tracking filter calibration complete\n");

	priv->cal_initialized = true;
	goto end;
fail:
	tda_info("RF tracking filter calibration failed!\n");
end:
	return ret;
}

static int tda18271c1_rf_tracking_filter_calibration(struct dvb_frontend *fe,
						     u32 freq, u32 bw)
{
	struct tda18271_priv *priv = fe->tuner_priv;
	unsigned char *regs = priv->tda18271_regs;
	int ret;
	u32 N = 0;

	/* calculate bp filter */
	tda18271_calc_bp_filter(fe, &freq);
	tda18271_write_regs(fe, R_EP1, 1);

	regs[R_EB4]  &= 0x07;
	regs[R_EB4]  |= 0x60;
	tda18271_write_regs(fe, R_EB4, 1);

	regs[R_EB7]   = 0x60;
	tda18271_write_regs(fe, R_EB7, 1);

	regs[R_EB14]  = 0x00;
	tda18271_write_regs(fe, R_EB14, 1);

	regs[R_EB20]  = 0xcc;
	tda18271_write_regs(fe, R_EB20, 1);

	/* set cal mode to RF tracking filter calibration */
	regs[R_EP4]  |= 0x03;

	/* calculate cal pll */

	switch (priv->mode) {
	case TDA18271_ANALOG:
		N = freq - 1250000;
		break;
	case TDA18271_DIGITAL:
		N = freq + bw / 2;
		break;
	}

	tda18271_calc_cal_pll(fe, N);

	/* calculate main pll */

	switch (priv->mode) {
	case TDA18271_ANALOG:
		N = freq - 250000;
		break;
	case TDA18271_DIGITAL:
		N = freq + bw / 2 + 1000000;
		break;
	}

	tda18271_calc_main_pll(fe, N);

	ret = tda18271_write_regs(fe, R_EP3, 11);
	if (tda_fail(ret))
		return ret;

	msleep(5); /* RF tracking filter calibration initialization */

	/* search for K,M,CO for RF calibration */
	tda18271_calc_km(fe, &freq);
	tda18271_write_regs(fe, R_EB13, 1);

	/* search for rf band */
	tda18271_calc_rf_band(fe, &freq);

	/* search for gain taper */
	tda18271_calc_gain_taper(fe, &freq);

	tda18271_write_regs(fe, R_EP2, 1);
	tda18271_write_regs(fe, R_EP1, 1);
	tda18271_write_regs(fe, R_EP2, 1);
	tda18271_write_regs(fe, R_EP1, 1);

	regs[R_EB4]  &= 0x07;
	regs[R_EB4]  |= 0x40;
	tda18271_write_regs(fe, R_EB4, 1);

	regs[R_EB7]   = 0x40;
	tda18271_write_regs(fe, R_EB7, 1);
	msleep(10); /* pll locking */

	regs[R_EB20]  = 0xec;
	tda18271_write_regs(fe, R_EB20, 1);
	msleep(60); /* RF tracking filter calibration completion */

	regs[R_EP4]  &= ~0x03; /* set cal mode to normal */
	tda18271_write_regs(fe, R_EP4, 1);

	tda18271_write_regs(fe, R_EP1, 1);

	/* RF tracking filter correction for VHF_Low band */
	if (0 == tda18271_calc_rf_cal(fe, &freq))
		tda18271_write_regs(fe, R_EB14, 1);

	return 0;
}

/* ------------------------------------------------------------------ */

static int tda18271_ir_cal_init(struct dvb_frontend *fe)
{
	struct tda18271_priv *priv = fe->tuner_priv;
	unsigned char *regs = priv->tda18271_regs;
	int ret;

	ret = tda18271_read_regs(fe);
	if (tda_fail(ret))
		goto fail;

	/* test IR_CAL_OK to see if we need init */
	if ((regs[R_EP1] & 0x08) == 0)
		ret = tda18271_init_regs(fe);
fail:
	return ret;
}

static int tda18271_init(struct dvb_frontend *fe)
{
	struct tda18271_priv *priv = fe->tuner_priv;
	int ret;

	mutex_lock(&priv->lock);

	/* full power up */
	ret = tda18271_set_standby_mode(fe, 0, 0, 0);
	if (tda_fail(ret))
		goto fail;

	/* initialization */
	ret = tda18271_ir_cal_init(fe);
	if (tda_fail(ret))
		goto fail;

	if (priv->id == TDA18271HDC2)
		tda18271c2_rf_cal_init(fe);
fail:
	mutex_unlock(&priv->lock);

	return ret;
}

static int tda18271_sleep(struct dvb_frontend *fe)
{
	struct tda18271_priv *priv = fe->tuner_priv;
	int ret;

	mutex_lock(&priv->lock);

	/* enter standby mode, with required output features enabled */
	ret = tda18271_toggle_output(fe, 1);

	mutex_unlock(&priv->lock);

	return ret;
}

/* ------------------------------------------------------------------ */

static int tda18271_agc(struct dvb_frontend *fe)
{
	struct tda18271_priv *priv = fe->tuner_priv;
	int ret = 0;

	switch (priv->config) {
	case TDA8290_LNA_OFF:
		/* no external agc configuration required */
		if (tda18271_debug & DBG_ADV)
			tda_dbg("no agc configuration provided\n");
		break;
	case TDA8290_LNA_ON_BRIDGE:
		/* switch with GPIO of saa713x */
		tda_dbg("invoking callback\n");
		if (fe->callback)
			ret = fe->callback(priv->i2c_props.adap->algo_data,
					   DVB_FRONTEND_COMPONENT_TUNER,
					   TDA18271_CALLBACK_CMD_AGC_ENABLE,
					   priv->mode);
		break;
	case TDA8290_LNA_GP0_HIGH_ON:
	case TDA8290_LNA_GP0_HIGH_OFF:
	default:
		/* n/a - currently not supported */
		tda_err("unsupported configuration: %d\n", priv->config);
		ret = -EINVAL;
		break;
	}
	return ret;
}

static int tda18271_tune(struct dvb_frontend *fe,
			 struct tda18271_std_map_item *map, u32 freq, u32 bw)
{
	struct tda18271_priv *priv = fe->tuner_priv;
	int ret;

	tda_dbg("freq = %d, ifc = %d, bw = %d, agc_mode = %d, std = %d\n",
		freq, map->if_freq, bw, map->agc_mode, map->std);

	ret = tda18271_agc(fe);
	if (tda_fail(ret))
		tda_warn("failed to configure agc\n");

	ret = tda18271_init(fe);
	if (tda_fail(ret))
		goto fail;

	mutex_lock(&priv->lock);

	switch (priv->id) {
	case TDA18271HDC1:
		tda18271c1_rf_tracking_filter_calibration(fe, freq, bw);
		break;
	case TDA18271HDC2:
		tda18271c2_rf_tracking_filters_correction(fe, freq);
		break;
	}
	ret = tda18271_channel_configuration(fe, map, freq, bw);

	mutex_unlock(&priv->lock);
fail:
	return ret;
}

/* ------------------------------------------------------------------ */

static int tda18271_set_params(struct dvb_frontend *fe)
{
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	u32 delsys = c->delivery_system;
	u32 bw = c->bandwidth_hz;
	u32 freq = c->frequency;
	struct tda18271_priv *priv = fe->tuner_priv;
	struct tda18271_std_map *std_map = &priv->std;
	struct tda18271_std_map_item *map;
	int ret;

	priv->mode = TDA18271_DIGITAL;

	switch (delsys) {
	case SYS_ATSC:
		map = &std_map->atsc_6;
		bw = 6000000;
		break;
	case SYS_ISDBT:
	case SYS_DVBT:
	case SYS_DVBT2:
		if (bw <= 6000000) {
			map = &std_map->dvbt_6;
		} else if (bw <= 7000000) {
			map = &std_map->dvbt_7;
		} else {
			map = &std_map->dvbt_8;
		}
		break;
	case SYS_DVBC_ANNEX_B:
		bw = 6000000;
		fallthrough;
	case SYS_DVBC_ANNEX_A:
	case SYS_DVBC_ANNEX_C:
		if (bw <= 6000000) {
			map = &std_map->qam_6;
		} else if (bw <= 7000000) {
			map = &std_map->qam_7;
		} else {
			map = &std_map->qam_8;
		}
		break;
	default:
		tda_warn("modulation type not supported!\n");
		return -EINVAL;
	}

	/* When tuning digital, the analog demod must be tri-stated */
	if (fe->ops.analog_ops.standby)
		fe->ops.analog_ops.standby(fe);

	ret = tda18271_tune(fe, map, freq, bw);

	if (tda_fail(ret))
		goto fail;

	priv->if_freq   = map->if_freq;
	priv->frequency = freq;
	priv->bandwidth = bw;
fail:
	return ret;
}

static int tda18271_set_analog_params(struct dvb_frontend *fe,
				      struct analog_parameters *params)
{
	struct tda18271_priv *priv = fe->tuner_priv;
	struct tda18271_std_map *std_map = &priv->std;
	struct tda18271_std_map_item *map;
	char *mode;
	int ret;
	u32 freq = params->frequency * 125 *
		((params->mode == V4L2_TUNER_RADIO) ? 1 : 1000) / 2;

	priv->mode = TDA18271_ANALOG;

	if (params->mode == V4L2_TUNER_RADIO) {
		map = &std_map->fm_radio;
		mode = "fm";
	} else if (params->std & V4L2_STD_MN) {
		map = &std_map->atv_mn;
		mode = "MN";
	} else if (params->std & V4L2_STD_B) {
		map = &std_map->atv_b;
		mode = "B";
	} else if (params->std & V4L2_STD_GH) {
		map = &std_map->atv_gh;
		mode = "GH";
	} else if (params->std & V4L2_STD_PAL_I) {
		map = &std_map->atv_i;
		mode = "I";
	} else if (params->std & V4L2_STD_DK) {
		map = &std_map->atv_dk;
		mode = "DK";
	} else if (params->std & V4L2_STD_SECAM_L) {
		map = &std_map->atv_l;
		mode = "L";
	} else if (params->std & V4L2_STD_SECAM_LC) {
		map = &std_map->atv_lc;
		mode = "L'";
	} else {
		map = &std_map->atv_i;
		mode = "xx";
	}

	tda_dbg("setting tda18271 to system %s\n", mode);

	ret = tda18271_tune(fe, map, freq, 0);

	if (tda_fail(ret))
		goto fail;

	priv->if_freq   = map->if_freq;
	priv->frequency = freq;
	priv->bandwidth = 0;
fail:
	return ret;
}

static void tda18271_release(struct dvb_frontend *fe)
{
	struct tda18271_priv *priv = fe->tuner_priv;

	mutex_lock(&tda18271_list_mutex);

	if (priv)
		hybrid_tuner_release_state(priv);

	mutex_unlock(&tda18271_list_mutex);

	fe->tuner_priv = NULL;
}

static int tda18271_get_frequency(struct dvb_frontend *fe, u32 *frequency)
{
	struct tda18271_priv *priv = fe->tuner_priv;
	*frequency = priv->frequency;
	return 0;
}

static int tda18271_get_bandwidth(struct dvb_frontend *fe, u32 *bandwidth)
{
	struct tda18271_priv *priv = fe->tuner_priv;
	*bandwidth = priv->bandwidth;
	return 0;
}

static int tda18271_get_if_frequency(struct dvb_frontend *fe, u32 *frequency)
{
	struct tda18271_priv *priv = fe->tuner_priv;
	*frequency = (u32)priv->if_freq * 1000;
	return 0;
}

/* ------------------------------------------------------------------ */

#define tda18271_update_std(std_cfg, name) do {				\
	if (map->std_cfg.if_freq +					\
		map->std_cfg.agc_mode + map->std_cfg.std +		\
		map->std_cfg.if_lvl + map->std_cfg.rfagc_top > 0) {	\
		tda_dbg("Using custom std config for %s\n", name);	\
		memcpy(&std->std_cfg, &map->std_cfg,			\
			sizeof(struct tda18271_std_map_item));		\
	} } while (0)

#define tda18271_dump_std_item(std_cfg, name) do {			\
	tda_dbg("(%s) if_freq = %d, agc_mode = %d, std = %d, "		\
		"if_lvl = %d, rfagc_top = 0x%02x\n",			\
		name, std->std_cfg.if_freq,				\
		std->std_cfg.agc_mode, std->std_cfg.std,		\
		std->std_cfg.if_lvl, std->std_cfg.rfagc_top);		\
	} while (0)

static int tda18271_dump_std_map(struct dvb_frontend *fe)
{
	struct tda18271_priv *priv = fe->tuner_priv;
	struct tda18271_std_map *std = &priv->std;

	tda_dbg("========== STANDARD MAP SETTINGS ==========\n");
	tda18271_dump_std_item(fm_radio, "  fm  ");
	tda18271_dump_std_item(atv_b,  "atv b ");
	tda18271_dump_std_item(atv_dk, "atv dk");
	tda18271_dump_std_item(atv_gh, "atv gh");
	tda18271_dump_std_item(atv_i,  "atv i ");
	tda18271_dump_std_item(atv_l,  "atv l ");
	tda18271_dump_std_item(atv_lc, "atv l'");
	tda18271_dump_std_item(atv_mn, "atv mn");
	tda18271_dump_std_item(atsc_6, "atsc 6");
	tda18271_dump_std_item(dvbt_6, "dvbt 6");
	tda18271_dump_std_item(dvbt_7, "dvbt 7");
	tda18271_dump_std_item(dvbt_8, "dvbt 8");
	tda18271_dump_std_item(qam_6,  "qam 6 ");
	tda18271_dump_std_item(qam_7,  "qam 7 ");
	tda18271_dump_std_item(qam_8,  "qam 8 ");

	return 0;
}

static int tda18271_update_std_map(struct dvb_frontend *fe,
				   struct tda18271_std_map *map)
{
	struct tda18271_priv *priv = fe->tuner_priv;
	struct tda18271_std_map *std = &priv->std;

	if (!map)
		return -EINVAL;

	tda18271_update_std(fm_radio, "fm");
	tda18271_update_std(atv_b,  "atv b");
	tda18271_update_std(atv_dk, "atv dk");
	tda18271_update_std(atv_gh, "atv gh");
	tda18271_update_std(atv_i,  "atv i");
	tda18271_update_std(atv_l,  "atv l");
	tda18271_update_std(atv_lc, "atv l'");
	tda18271_update_std(atv_mn, "atv mn");
	tda18271_update_std(atsc_6, "atsc 6");
	tda18271_update_std(dvbt_6, "dvbt 6");
	tda18271_update_std(dvbt_7, "dvbt 7");
	tda18271_update_std(dvbt_8, "dvbt 8");
	tda18271_update_std(qam_6,  "qam 6");
	tda18271_update_std(qam_7,  "qam 7");
	tda18271_update_std(qam_8,  "qam 8");

	return 0;
}

static int tda18271_get_id(struct dvb_frontend *fe)
{
	struct tda18271_priv *priv = fe->tuner_priv;
	unsigned char *regs = priv->tda18271_regs;
	char *name;
	int ret;

	mutex_lock(&priv->lock);
	ret = tda18271_read_regs(fe);
	mutex_unlock(&priv->lock);

	if (ret) {
		tda_info("Error reading device ID @ %d-%04x, bailing out.\n",
			 i2c_adapter_id(priv->i2c_props.adap),
			 priv->i2c_props.addr);
		return -EIO;
	}

	switch (regs[R_ID] & 0x7f) {
	case 3:
		name = "TDA18271HD/C1";
		priv->id = TDA18271HDC1;
		break;
	case 4:
		name = "TDA18271HD/C2";
		priv->id = TDA18271HDC2;
		break;
	default:
		tda_info("Unknown device (%i) detected @ %d-%04x, device not supported.\n",
			 regs[R_ID], i2c_adapter_id(priv->i2c_props.adap),
			 priv->i2c_props.addr);
		return -EINVAL;
	}

	tda_info("%s detected @ %d-%04x\n", name,
		 i2c_adapter_id(priv->i2c_props.adap), priv->i2c_props.addr);

	return 0;
}

static int tda18271_setup_configuration(struct dvb_frontend *fe,
					struct tda18271_config *cfg)
{
	struct tda18271_priv *priv = fe->tuner_priv;

	priv->gate = (cfg) ? cfg->gate : TDA18271_GATE_AUTO;
	priv->role = (cfg) ? cfg->role : TDA18271_MASTER;
	priv->config = (cfg) ? cfg->config : 0;
	priv->small_i2c = (cfg) ?
		cfg->small_i2c : TDA18271_39_BYTE_CHUNK_INIT;
	priv->output_opt = (cfg) ?
		cfg->output_opt : TDA18271_OUTPUT_LT_XT_ON;

	return 0;
}

static inline int tda18271_need_cal_on_startup(struct tda18271_config *cfg)
{
	/* tda18271_cal_on_startup == -1 when cal module option is unset */
	return ((tda18271_cal_on_startup == -1) ?
		/* honor configuration setting */
		((cfg) && (cfg->rf_cal_on_startup)) :
		/* module option overrides configuration setting */
		(tda18271_cal_on_startup)) ? 1 : 0;
}

static int tda18271_set_config(struct dvb_frontend *fe, void *priv_cfg)
{
	struct tda18271_config *cfg = (struct tda18271_config *) priv_cfg;

	tda18271_setup_configuration(fe, cfg);

	if (tda18271_need_cal_on_startup(cfg))
		tda18271_init(fe);

	/* override default std map with values in config struct */
	if ((cfg) && (cfg->std_map))
		tda18271_update_std_map(fe, cfg->std_map);

	return 0;
}

static const struct dvb_tuner_ops tda18271_tuner_ops = {
	.info = {
		.name = "NXP TDA18271HD",
		.frequency_min_hz  =  45 * MHz,
		.frequency_max_hz  = 864 * MHz,
		.frequency_step_hz = 62500
	},
	.init              = tda18271_init,
	.sleep             = tda18271_sleep,
	.set_params        = tda18271_set_params,
	.set_analog_params = tda18271_set_analog_params,
	.release           = tda18271_release,
	.set_config        = tda18271_set_config,
	.get_frequency     = tda18271_get_frequency,
	.get_bandwidth     = tda18271_get_bandwidth,
	.get_if_frequency  = tda18271_get_if_frequency,
};

struct dvb_frontend *tda18271_attach(struct dvb_frontend *fe, u8 addr,
				     struct i2c_adapter *i2c,
				     struct tda18271_config *cfg)
{
	struct tda18271_priv *priv = NULL;
	int instance, ret;

	mutex_lock(&tda18271_list_mutex);

	instance = hybrid_tuner_request_state(struct tda18271_priv, priv,
					      hybrid_tuner_instance_list,
					      i2c, addr, "tda18271");
	switch (instance) {
	case 0:
		goto fail;
	case 1:
		/* new tuner instance */
		fe->tuner_priv = priv;

		tda18271_setup_configuration(fe, cfg);

		priv->cal_initialized = false;
		mutex_init(&priv->lock);

		ret = tda18271_get_id(fe);
		if (tda_fail(ret))
			goto fail;

		ret = tda18271_assign_map_layout(fe);
		if (tda_fail(ret))
			goto fail;

		/* if delay_cal is set, delay IR & RF calibration until init()
		 * module option 'cal' overrides this delay */
		if ((cfg->delay_cal) && (!tda18271_need_cal_on_startup(cfg)))
			break;

		mutex_lock(&priv->lock);
		tda18271_init_regs(fe);

		if ((tda18271_need_cal_on_startup(cfg)) &&
		    (priv->id == TDA18271HDC2))
			tda18271c2_rf_cal_init(fe);

		/* enter standby mode, with required output features enabled */
		ret = tda18271_toggle_output(fe, 1);
		tda_fail(ret);

		mutex_unlock(&priv->lock);
		break;
	default:
		/* existing tuner instance */
		fe->tuner_priv = priv;

		/* allow dvb driver to override configuration settings */
		if (cfg) {
			if (cfg->gate != TDA18271_GATE_ANALOG)
				priv->gate = cfg->gate;
			if (cfg->role)
				priv->role = cfg->role;
			if (cfg->config)
				priv->config = cfg->config;
			if (cfg->small_i2c)
				priv->small_i2c = cfg->small_i2c;
			if (cfg->output_opt)
				priv->output_opt = cfg->output_opt;
			if (cfg->std_map)
				tda18271_update_std_map(fe, cfg->std_map);
		}
		if (tda18271_need_cal_on_startup(cfg))
			tda18271_init(fe);
		break;
	}

	/* override default std map with values in config struct */
	if ((cfg) && (cfg->std_map))
		tda18271_update_std_map(fe, cfg->std_map);

	mutex_unlock(&tda18271_list_mutex);

	memcpy(&fe->ops.tuner_ops, &tda18271_tuner_ops,
	       sizeof(struct dvb_tuner_ops));

	if (tda18271_debug & (DBG_MAP | DBG_ADV))
		tda18271_dump_std_map(fe);

	return fe;
fail:
	mutex_unlock(&tda18271_list_mutex);

	tda18271_release(fe);
	return NULL;
}
EXPORT_SYMBOL_GPL(tda18271_attach);
MODULE_DESCRIPTION("NXP TDA18271HD analog / digital tuner driver");
MODULE_AUTHOR("Michael Krufky <mkrufky@linuxtv.org>");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.4");
