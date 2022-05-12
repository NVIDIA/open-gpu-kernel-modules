// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Programming the mspx4xx sound processor family
 *
 * (c) 1997-2001 Gerd Knorr <kraxel@bytesex.org>
 */


#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/freezer.h>
#include <linux/videodev2.h>
#include <media/v4l2-common.h>
#include <media/drv-intf/msp3400.h>
#include <linux/kthread.h>
#include <linux/suspend.h>
#include "msp3400-driver.h"

/* this one uses the automatic sound standard detection of newer msp34xx
   chip versions */
static struct {
	int retval;
	int main, second;
	char *name;
	v4l2_std_id std;
} msp_stdlist[] = {
	{ 0x0000, 0, 0, "could not detect sound standard", V4L2_STD_ALL },
	{ 0x0001, 0, 0, "autodetect start", V4L2_STD_ALL },
	{ 0x0002, MSP_CARRIER(4.5), MSP_CARRIER(4.72),
	  "4.5/4.72  M Dual FM-Stereo", V4L2_STD_MN },
	{ 0x0003, MSP_CARRIER(5.5), MSP_CARRIER(5.7421875),
	  "5.5/5.74  B/G Dual FM-Stereo", V4L2_STD_BG },
	{ 0x0004, MSP_CARRIER(6.5), MSP_CARRIER(6.2578125),
	  "6.5/6.25  D/K1 Dual FM-Stereo", V4L2_STD_DK },
	{ 0x0005, MSP_CARRIER(6.5), MSP_CARRIER(6.7421875),
	  "6.5/6.74  D/K2 Dual FM-Stereo", V4L2_STD_DK },
	{ 0x0006, MSP_CARRIER(6.5), MSP_CARRIER(6.5),
	  "6.5  D/K FM-Mono (HDEV3)", V4L2_STD_DK },
	{ 0x0007, MSP_CARRIER(6.5), MSP_CARRIER(5.7421875),
	  "6.5/5.74  D/K3 Dual FM-Stereo", V4L2_STD_DK },
	{ 0x0008, MSP_CARRIER(5.5), MSP_CARRIER(5.85),
	  "5.5/5.85  B/G NICAM FM", V4L2_STD_BG },
	{ 0x0009, MSP_CARRIER(6.5), MSP_CARRIER(5.85),
	  "6.5/5.85  L NICAM AM", V4L2_STD_L },
	{ 0x000a, MSP_CARRIER(6.0), MSP_CARRIER(6.55),
	  "6.0/6.55  I NICAM FM", V4L2_STD_PAL_I },
	{ 0x000b, MSP_CARRIER(6.5), MSP_CARRIER(5.85),
	  "6.5/5.85  D/K NICAM FM", V4L2_STD_DK },
	{ 0x000c, MSP_CARRIER(6.5), MSP_CARRIER(5.85),
	  "6.5/5.85  D/K NICAM FM (HDEV2)", V4L2_STD_DK },
	{ 0x000d, MSP_CARRIER(6.5), MSP_CARRIER(5.85),
	  "6.5/5.85  D/K NICAM FM (HDEV3)", V4L2_STD_DK },
	{ 0x0020, MSP_CARRIER(4.5), MSP_CARRIER(4.5),
	  "4.5  M BTSC-Stereo", V4L2_STD_MTS },
	{ 0x0021, MSP_CARRIER(4.5), MSP_CARRIER(4.5),
	  "4.5  M BTSC-Mono + SAP", V4L2_STD_MTS },
	{ 0x0030, MSP_CARRIER(4.5), MSP_CARRIER(4.5),
	  "4.5  M EIA-J Japan Stereo", V4L2_STD_NTSC_M_JP },
	{ 0x0040, MSP_CARRIER(10.7), MSP_CARRIER(10.7),
	  "10.7  FM-Stereo Radio", V4L2_STD_ALL },
	{ 0x0050, MSP_CARRIER(6.5), MSP_CARRIER(6.5),
	  "6.5  SAT-Mono", V4L2_STD_ALL },
	{ 0x0051, MSP_CARRIER(7.02), MSP_CARRIER(7.20),
	  "7.02/7.20  SAT-Stereo", V4L2_STD_ALL },
	{ 0x0060, MSP_CARRIER(7.2), MSP_CARRIER(7.2),
	  "7.2  SAT ADR", V4L2_STD_ALL },
	{     -1, 0, 0, NULL, 0 }, /* EOF */
};

static struct msp3400c_init_data_dem {
	int fir1[6];
	int fir2[6];
	int cdo1;
	int cdo2;
	int ad_cv;
	int mode_reg;
	int dsp_src;
	int dsp_matrix;
} msp3400c_init_data[] = {
	{	/* AM (for carrier detect / msp3400) */
		{75, 19, 36, 35, 39, 40},
		{75, 19, 36, 35, 39, 40},
		MSP_CARRIER(5.5), MSP_CARRIER(5.5),
		0x00d0, 0x0500, 0x0020, 0x3000
	}, {	/* AM (for carrier detect / msp3410) */
		{-1, -1, -8, 2, 59, 126},
		{-1, -1, -8, 2, 59, 126},
		MSP_CARRIER(5.5), MSP_CARRIER(5.5),
		0x00d0, 0x0100, 0x0020, 0x3000
	}, {	/* FM Radio */
		{-8, -8, 4, 6, 78, 107},
		{-8, -8, 4, 6, 78, 107},
		MSP_CARRIER(10.7), MSP_CARRIER(10.7),
		0x00d0, 0x0480, 0x0020, 0x3000
	}, {	/* Terrestrial FM-mono + FM-stereo */
		{3, 18, 27, 48, 66, 72},
		{3, 18, 27, 48, 66, 72},
		MSP_CARRIER(5.5), MSP_CARRIER(5.5),
		0x00d0, 0x0480, 0x0030, 0x3000
	}, {	/* Sat FM-mono */
		{ 1, 9, 14, 24, 33, 37},
		{ 3, 18, 27, 48, 66, 72},
		MSP_CARRIER(6.5), MSP_CARRIER(6.5),
		0x00c6, 0x0480, 0x0000, 0x3000
	}, {	/* NICAM/FM --  B/G (5.5/5.85), D/K (6.5/5.85) */
		{-2, -8, -10, 10, 50, 86},
		{3, 18, 27, 48, 66, 72},
		MSP_CARRIER(5.5), MSP_CARRIER(5.5),
		0x00d0, 0x0040, 0x0120, 0x3000
	}, {	/* NICAM/FM -- I (6.0/6.552) */
		{2, 4, -6, -4, 40, 94},
		{3, 18, 27, 48, 66, 72},
		MSP_CARRIER(6.0), MSP_CARRIER(6.0),
		0x00d0, 0x0040, 0x0120, 0x3000
	}, {	/* NICAM/AM -- L (6.5/5.85) */
		{-2, -8, -10, 10, 50, 86},
		{-4, -12, -9, 23, 79, 126},
		MSP_CARRIER(6.5), MSP_CARRIER(6.5),
		0x00c6, 0x0140, 0x0120, 0x7c00
	},
};

struct msp3400c_carrier_detect {
	int   cdo;
	char *name;
};

static struct msp3400c_carrier_detect msp3400c_carrier_detect_main[] = {
	/* main carrier */
	{ MSP_CARRIER(4.5),        "4.5   NTSC"                   },
	{ MSP_CARRIER(5.5),        "5.5   PAL B/G"                },
	{ MSP_CARRIER(6.0),        "6.0   PAL I"                  },
	{ MSP_CARRIER(6.5),        "6.5   PAL D/K + SAT + SECAM"  }
};

static struct msp3400c_carrier_detect msp3400c_carrier_detect_55[] = {
	/* PAL B/G */
	{ MSP_CARRIER(5.7421875),  "5.742 PAL B/G FM-stereo"     },
	{ MSP_CARRIER(5.85),       "5.85  PAL B/G NICAM"         }
};

static struct msp3400c_carrier_detect msp3400c_carrier_detect_65[] = {
	/* PAL SAT / SECAM */
	{ MSP_CARRIER(5.85),       "5.85  PAL D/K + SECAM NICAM" },
	{ MSP_CARRIER(6.2578125),  "6.25  PAL D/K1 FM-stereo" },
	{ MSP_CARRIER(6.7421875),  "6.74  PAL D/K2 FM-stereo" },
	{ MSP_CARRIER(7.02),       "7.02  PAL SAT FM-stereo s/b" },
	{ MSP_CARRIER(7.20),       "7.20  PAL SAT FM-stereo s"   },
	{ MSP_CARRIER(7.38),       "7.38  PAL SAT FM-stereo b"   },
};

/* ------------------------------------------------------------------------ */

const char *msp_standard_std_name(int std)
{
	int i;

	for (i = 0; msp_stdlist[i].name != NULL; i++)
		if (msp_stdlist[i].retval == std)
			return msp_stdlist[i].name;
	return "unknown";
}

static v4l2_std_id msp_standard_std(int std)
{
	int i;

	for (i = 0; msp_stdlist[i].name != NULL; i++)
		if (msp_stdlist[i].retval == std)
			return msp_stdlist[i].std;
	return V4L2_STD_ALL;
}

static void msp_set_source(struct i2c_client *client, u16 src)
{
	struct msp_state *state = to_state(i2c_get_clientdata(client));

	if (msp_dolby) {
		msp_write_dsp(client, 0x0008, 0x0520); /* I2S1 */
		msp_write_dsp(client, 0x0009, 0x0620); /* I2S2 */
	} else {
		msp_write_dsp(client, 0x0008, src);
		msp_write_dsp(client, 0x0009, src);
	}
	msp_write_dsp(client, 0x000a, src);
	msp_write_dsp(client, 0x000b, src);
	msp_write_dsp(client, 0x000c, src);
	if (state->has_scart2_out)
		msp_write_dsp(client, 0x0041, src);
}

void msp3400c_set_carrier(struct i2c_client *client, int cdo1, int cdo2)
{
	msp_write_dem(client, 0x0093, cdo1 & 0xfff);
	msp_write_dem(client, 0x009b, cdo1 >> 12);
	msp_write_dem(client, 0x00a3, cdo2 & 0xfff);
	msp_write_dem(client, 0x00ab, cdo2 >> 12);
	msp_write_dem(client, 0x0056, 0); /* LOAD_REG_1/2 */
}

void msp3400c_set_mode(struct i2c_client *client, int mode)
{
	struct msp_state *state = to_state(i2c_get_clientdata(client));
	struct msp3400c_init_data_dem *data = &msp3400c_init_data[mode];
	int tuner = (state->route_in >> 3) & 1;
	int i;

	dev_dbg_lvl(&client->dev, 1, msp_debug, "set_mode: %d\n", mode);
	state->mode = mode;
	state->rxsubchans = V4L2_TUNER_SUB_MONO;

	msp_write_dem(client, 0x00bb, data->ad_cv | (tuner ? 0x100 : 0));

	for (i = 5; i >= 0; i--)               /* fir 1 */
		msp_write_dem(client, 0x0001, data->fir1[i]);

	msp_write_dem(client, 0x0005, 0x0004); /* fir 2 */
	msp_write_dem(client, 0x0005, 0x0040);
	msp_write_dem(client, 0x0005, 0x0000);
	for (i = 5; i >= 0; i--)
		msp_write_dem(client, 0x0005, data->fir2[i]);

	msp_write_dem(client, 0x0083, data->mode_reg);

	msp3400c_set_carrier(client, data->cdo1, data->cdo2);

	msp_set_source(client, data->dsp_src);
	/* set prescales */

	/* volume prescale for SCART (AM mono input) */
	msp_write_dsp(client, 0x000d, 0x1900);
	msp_write_dsp(client, 0x000e, data->dsp_matrix);
	if (state->has_nicam) /* nicam prescale */
		msp_write_dsp(client, 0x0010, 0x5a00);
}

/* Set audio mode. Note that the pre-'G' models do not support BTSC+SAP,
   nor do they support stereo BTSC. */
static void msp3400c_set_audmode(struct i2c_client *client)
{
	static char *strmode[] = {
		"mono", "stereo", "lang2", "lang1", "lang1+lang2"
	};
	struct msp_state *state = to_state(i2c_get_clientdata(client));
	char *modestr = (state->audmode >= 0 && state->audmode < 5) ?
		strmode[state->audmode] : "unknown";
	int src = 0;	/* channel source: FM/AM, nicam or SCART */
	int audmode = state->audmode;

	if (state->opmode == OPMODE_AUTOSELECT) {
		/* this method would break everything, let's make sure
		 * it's never called
		 */
		dev_dbg_lvl(&client->dev, 1, msp_debug,
			"set_audmode called with mode=%d instead of set_source (ignored)\n",
			state->audmode);
		return;
	}

	/* Note: for the C and D revs no NTSC stereo + SAP is possible as
	   the hardware does not support SAP. So the rxsubchans combination
	   of STEREO | LANG2 does not occur. */

	if (state->mode != MSP_MODE_EXTERN) {
		/* switch to mono if only mono is available */
		if (state->rxsubchans == V4L2_TUNER_SUB_MONO)
			audmode = V4L2_TUNER_MODE_MONO;
		/* if bilingual */
		else if (state->rxsubchans & V4L2_TUNER_SUB_LANG2) {
			/* and mono or stereo, then fallback to lang1 */
			if (audmode == V4L2_TUNER_MODE_MONO ||
			    audmode == V4L2_TUNER_MODE_STEREO)
				audmode = V4L2_TUNER_MODE_LANG1;
		}
		/* if stereo, and audmode is not mono, then switch to stereo */
		else if (audmode != V4L2_TUNER_MODE_MONO)
			audmode = V4L2_TUNER_MODE_STEREO;
	}

	/* switch demodulator */
	switch (state->mode) {
	case MSP_MODE_FM_TERRA:
		dev_dbg_lvl(&client->dev, 1, msp_debug, "FM set_audmode: %s\n", modestr);
		switch (audmode) {
		case V4L2_TUNER_MODE_STEREO:
			msp_write_dsp(client, 0x000e, 0x3001);
			break;
		case V4L2_TUNER_MODE_MONO:
		case V4L2_TUNER_MODE_LANG1:
		case V4L2_TUNER_MODE_LANG2:
		case V4L2_TUNER_MODE_LANG1_LANG2:
			msp_write_dsp(client, 0x000e, 0x3000);
			break;
		}
		break;
	case MSP_MODE_FM_SAT:
		dev_dbg_lvl(&client->dev, 1, msp_debug, "SAT set_audmode: %s\n", modestr);
		switch (audmode) {
		case V4L2_TUNER_MODE_MONO:
			msp3400c_set_carrier(client, MSP_CARRIER(6.5), MSP_CARRIER(6.5));
			break;
		case V4L2_TUNER_MODE_STEREO:
		case V4L2_TUNER_MODE_LANG1_LANG2:
			msp3400c_set_carrier(client, MSP_CARRIER(7.2), MSP_CARRIER(7.02));
			break;
		case V4L2_TUNER_MODE_LANG1:
			msp3400c_set_carrier(client, MSP_CARRIER(7.38), MSP_CARRIER(7.02));
			break;
		case V4L2_TUNER_MODE_LANG2:
			msp3400c_set_carrier(client, MSP_CARRIER(7.38), MSP_CARRIER(7.02));
			break;
		}
		break;
	case MSP_MODE_FM_NICAM1:
	case MSP_MODE_FM_NICAM2:
	case MSP_MODE_AM_NICAM:
		dev_dbg_lvl(&client->dev, 1, msp_debug,
			"NICAM set_audmode: %s\n", modestr);
		if (state->nicam_on)
			src = 0x0100;  /* NICAM */
		break;
	case MSP_MODE_BTSC:
		dev_dbg_lvl(&client->dev, 1, msp_debug,
			"BTSC set_audmode: %s\n", modestr);
		break;
	case MSP_MODE_EXTERN:
		dev_dbg_lvl(&client->dev, 1, msp_debug,
			"extern set_audmode: %s\n", modestr);
		src = 0x0200;  /* SCART */
		break;
	case MSP_MODE_FM_RADIO:
		dev_dbg_lvl(&client->dev, 1, msp_debug,
			"FM-Radio set_audmode: %s\n", modestr);
		break;
	default:
		dev_dbg_lvl(&client->dev, 1, msp_debug, "mono set_audmode\n");
		return;
	}

	/* switch audio */
	dev_dbg_lvl(&client->dev, 1, msp_debug, "set audmode %d\n", audmode);
	switch (audmode) {
	case V4L2_TUNER_MODE_STEREO:
	case V4L2_TUNER_MODE_LANG1_LANG2:
		src |= 0x0020;
		break;
	case V4L2_TUNER_MODE_MONO:
		if (state->mode == MSP_MODE_AM_NICAM) {
			dev_dbg_lvl(&client->dev, 1, msp_debug, "switching to AM mono\n");
			/* AM mono decoding is handled by tuner, not MSP chip */
			/* SCART switching control register */
			msp_set_scart(client, SCART_MONO, 0);
			src = 0x0200;
			break;
		}
		if (state->rxsubchans & V4L2_TUNER_SUB_STEREO)
			src = 0x0030;
		break;
	case V4L2_TUNER_MODE_LANG1:
		break;
	case V4L2_TUNER_MODE_LANG2:
		src |= 0x0010;
		break;
	}
	dev_dbg_lvl(&client->dev, 1, msp_debug,
		"set_audmode final source/matrix = 0x%x\n", src);

	msp_set_source(client, src);
}

static void msp3400c_print_mode(struct i2c_client *client)
{
	struct msp_state *state = to_state(i2c_get_clientdata(client));

	if (state->main == state->second)
		dev_dbg_lvl(&client->dev, 1, msp_debug,
			"mono sound carrier: %d.%03d MHz\n",
			state->main / 910000, (state->main / 910) % 1000);
	else
		dev_dbg_lvl(&client->dev, 1, msp_debug,
			"main sound carrier: %d.%03d MHz\n",
			state->main / 910000, (state->main / 910) % 1000);
	if (state->mode == MSP_MODE_FM_NICAM1 || state->mode == MSP_MODE_FM_NICAM2)
		dev_dbg_lvl(&client->dev, 1, msp_debug,
			"NICAM/FM carrier  : %d.%03d MHz\n",
			state->second / 910000, (state->second/910) % 1000);
	if (state->mode == MSP_MODE_AM_NICAM)
		dev_dbg_lvl(&client->dev, 1, msp_debug,
			"NICAM/AM carrier  : %d.%03d MHz\n",
			state->second / 910000, (state->second / 910) % 1000);
	if (state->mode == MSP_MODE_FM_TERRA && state->main != state->second) {
		dev_dbg_lvl(&client->dev, 1, msp_debug,
			"FM-stereo carrier : %d.%03d MHz\n",
			state->second / 910000, (state->second / 910) % 1000);
	}
}

/* ----------------------------------------------------------------------- */

static int msp3400c_detect_stereo(struct i2c_client *client)
{
	struct msp_state *state = to_state(i2c_get_clientdata(client));
	int val;
	int rxsubchans = state->rxsubchans;
	int newnicam = state->nicam_on;
	int update = 0;

	switch (state->mode) {
	case MSP_MODE_FM_TERRA:
		val = msp_read_dsp(client, 0x18);
		if (val > 32767)
			val -= 65536;
		dev_dbg_lvl(&client->dev, 2, msp_debug,
			"stereo detect register: %d\n", val);
		if (val > 8192) {
			rxsubchans = V4L2_TUNER_SUB_STEREO;
		} else if (val < -4096) {
			rxsubchans = V4L2_TUNER_SUB_LANG1 | V4L2_TUNER_SUB_LANG2;
		} else {
			rxsubchans = V4L2_TUNER_SUB_MONO;
		}
		newnicam = 0;
		break;
	case MSP_MODE_FM_NICAM1:
	case MSP_MODE_FM_NICAM2:
	case MSP_MODE_AM_NICAM:
		val = msp_read_dem(client, 0x23);
		dev_dbg_lvl(&client->dev, 2, msp_debug, "nicam sync=%d, mode=%d\n",
			val & 1, (val & 0x1e) >> 1);

		if (val & 1) {
			/* nicam synced */
			switch ((val & 0x1e) >> 1)  {
			case 0:
			case 8:
				rxsubchans = V4L2_TUNER_SUB_STEREO;
				break;
			case 1:
			case 9:
				rxsubchans = V4L2_TUNER_SUB_MONO;
				break;
			case 2:
			case 10:
				rxsubchans = V4L2_TUNER_SUB_LANG1 | V4L2_TUNER_SUB_LANG2;
				break;
			default:
				rxsubchans = V4L2_TUNER_SUB_MONO;
				break;
			}
			newnicam = 1;
		} else {
			newnicam = 0;
			rxsubchans = V4L2_TUNER_SUB_MONO;
		}
		break;
	}
	if (rxsubchans != state->rxsubchans) {
		update = 1;
		dev_dbg_lvl(&client->dev, 1, msp_debug,
			"watch: rxsubchans %02x => %02x\n",
			state->rxsubchans, rxsubchans);
		state->rxsubchans = rxsubchans;
	}
	if (newnicam != state->nicam_on) {
		update = 1;
		dev_dbg_lvl(&client->dev, 1, msp_debug, "watch: nicam %d => %d\n",
			state->nicam_on, newnicam);
		state->nicam_on = newnicam;
	}
	return update;
}

/*
 * A kernel thread for msp3400 control -- we don't want to block the
 * in the ioctl while doing the sound carrier & stereo detect
 */
/* stereo/multilang monitoring */
static void watch_stereo(struct i2c_client *client)
{
	struct msp_state *state = to_state(i2c_get_clientdata(client));

	if (msp_detect_stereo(client))
		msp_set_audmode(client);

	if (msp_once)
		state->watch_stereo = 0;
}

int msp3400c_thread(void *data)
{
	struct i2c_client *client = data;
	struct msp_state *state = to_state(i2c_get_clientdata(client));
	struct msp3400c_carrier_detect *cd;
	int count, max1, max2, val1, val2, val, i;

	dev_dbg_lvl(&client->dev, 1, msp_debug, "msp3400 daemon started\n");
	state->detected_std = V4L2_STD_ALL;
	set_freezable();
	for (;;) {
		dev_dbg_lvl(&client->dev, 2, msp_debug, "msp3400 thread: sleep\n");
		msp_sleep(state, -1);
		dev_dbg_lvl(&client->dev, 2, msp_debug, "msp3400 thread: wakeup\n");

restart:
		dev_dbg_lvl(&client->dev, 2, msp_debug, "thread: restart scan\n");
		state->restart = 0;
		if (kthread_should_stop())
			break;

		if (state->radio || MSP_MODE_EXTERN == state->mode) {
			/* no carrier scan, just unmute */
			dev_dbg_lvl(&client->dev, 1, msp_debug,
				"thread: no carrier scan\n");
			state->scan_in_progress = 0;
			msp_update_volume(state);
			continue;
		}

		/* mute audio */
		state->scan_in_progress = 1;
		msp_update_volume(state);

		msp3400c_set_mode(client, MSP_MODE_AM_DETECT);
		val1 = val2 = 0;
		max1 = max2 = -1;
		state->watch_stereo = 0;
		state->nicam_on = 0;

		/* wait for tuner to settle down after a channel change */
		if (msp_sleep(state, 200))
			goto restart;

		/* carrier detect pass #1 -- main carrier */
		cd = msp3400c_carrier_detect_main;
		count = ARRAY_SIZE(msp3400c_carrier_detect_main);

		if (msp_amsound && (state->v4l2_std & V4L2_STD_SECAM)) {
			/* autodetect doesn't work well with AM ... */
			max1 = 3;
			count = 0;
			dev_dbg_lvl(&client->dev, 1, msp_debug, "AM sound override\n");
		}

		for (i = 0; i < count; i++) {
			msp3400c_set_carrier(client, cd[i].cdo, cd[i].cdo);
			if (msp_sleep(state, 100))
				goto restart;
			val = msp_read_dsp(client, 0x1b);
			if (val > 32767)
				val -= 65536;
			if (val1 < val) {
				val1 = val;
				max1 = i;
			}
			dev_dbg_lvl(&client->dev, 1, msp_debug,
				"carrier1 val: %5d / %s\n", val, cd[i].name);
		}

		/* carrier detect pass #2 -- second (stereo) carrier */
		switch (max1) {
		case 1: /* 5.5 */
			cd = msp3400c_carrier_detect_55;
			count = ARRAY_SIZE(msp3400c_carrier_detect_55);
			break;
		case 3: /* 6.5 */
			cd = msp3400c_carrier_detect_65;
			count = ARRAY_SIZE(msp3400c_carrier_detect_65);
			break;
		case 0: /* 4.5 */
		case 2: /* 6.0 */
		default:
			cd = NULL;
			count = 0;
			break;
		}

		if (msp_amsound && (state->v4l2_std & V4L2_STD_SECAM)) {
			/* autodetect doesn't work well with AM ... */
			cd = NULL;
			count = 0;
			max2 = 0;
		}
		for (i = 0; i < count; i++) {
			msp3400c_set_carrier(client, cd[i].cdo, cd[i].cdo);
			if (msp_sleep(state, 100))
				goto restart;
			val = msp_read_dsp(client, 0x1b);
			if (val > 32767)
				val -= 65536;
			if (val2 < val) {
				val2 = val;
				max2 = i;
			}
			dev_dbg_lvl(&client->dev, 1, msp_debug,
				"carrier2 val: %5d / %s\n", val, cd[i].name);
		}

		/* program the msp3400 according to the results */
		state->main = msp3400c_carrier_detect_main[max1].cdo;
		switch (max1) {
		case 1: /* 5.5 */
			state->detected_std = V4L2_STD_BG | V4L2_STD_PAL_H;
			if (max2 == 0) {
				/* B/G FM-stereo */
				state->second = msp3400c_carrier_detect_55[max2].cdo;
				msp3400c_set_mode(client, MSP_MODE_FM_TERRA);
				state->watch_stereo = 1;
			} else if (max2 == 1 && state->has_nicam) {
				/* B/G NICAM */
				state->second = msp3400c_carrier_detect_55[max2].cdo;
				msp3400c_set_mode(client, MSP_MODE_FM_NICAM1);
				state->nicam_on = 1;
				state->watch_stereo = 1;
			} else {
				goto no_second;
			}
			break;
		case 2: /* 6.0 */
			/* PAL I NICAM */
			state->detected_std = V4L2_STD_PAL_I;
			state->second = MSP_CARRIER(6.552);
			msp3400c_set_mode(client, MSP_MODE_FM_NICAM2);
			state->nicam_on = 1;
			state->watch_stereo = 1;
			break;
		case 3: /* 6.5 */
			if (max2 == 1 || max2 == 2) {
				/* D/K FM-stereo */
				state->second = msp3400c_carrier_detect_65[max2].cdo;
				msp3400c_set_mode(client, MSP_MODE_FM_TERRA);
				state->watch_stereo = 1;
				state->detected_std = V4L2_STD_DK;
			} else if (max2 == 0 && (state->v4l2_std & V4L2_STD_SECAM)) {
				/* L NICAM or AM-mono */
				state->second = msp3400c_carrier_detect_65[max2].cdo;
				msp3400c_set_mode(client, MSP_MODE_AM_NICAM);
				state->watch_stereo = 1;
				state->detected_std = V4L2_STD_L;
			} else if (max2 == 0 && state->has_nicam) {
				/* D/K NICAM */
				state->second = msp3400c_carrier_detect_65[max2].cdo;
				msp3400c_set_mode(client, MSP_MODE_FM_NICAM1);
				state->nicam_on = 1;
				state->watch_stereo = 1;
				state->detected_std = V4L2_STD_DK;
			} else {
				goto no_second;
			}
			break;
		case 0: /* 4.5 */
			state->detected_std = V4L2_STD_MN;
			fallthrough;
		default:
no_second:
			state->second = msp3400c_carrier_detect_main[max1].cdo;
			msp3400c_set_mode(client, MSP_MODE_FM_TERRA);
			break;
		}
		msp3400c_set_carrier(client, state->second, state->main);

		/* unmute */
		state->scan_in_progress = 0;
		msp3400c_set_audmode(client);
		msp_update_volume(state);

		if (msp_debug)
			msp3400c_print_mode(client);

		/* monitor tv audio mode, the first time don't wait
		   so long to get a quick stereo/bilingual result */
		count = 3;
		while (state->watch_stereo) {
			if (msp_sleep(state, count ? 1000 : 5000))
				goto restart;
			if (count)
				count--;
			watch_stereo(client);
		}
	}
	dev_dbg_lvl(&client->dev, 1, msp_debug, "thread: exit\n");
	return 0;
}


int msp3410d_thread(void *data)
{
	struct i2c_client *client = data;
	struct msp_state *state = to_state(i2c_get_clientdata(client));
	int val, i, std, count;

	dev_dbg_lvl(&client->dev, 1, msp_debug, "msp3410 daemon started\n");
	state->detected_std = V4L2_STD_ALL;
	set_freezable();
	for (;;) {
		dev_dbg_lvl(&client->dev, 2, msp_debug, "msp3410 thread: sleep\n");
		msp_sleep(state, -1);
		dev_dbg_lvl(&client->dev, 2, msp_debug, "msp3410 thread: wakeup\n");

restart:
		dev_dbg_lvl(&client->dev, 2, msp_debug, "thread: restart scan\n");
		state->restart = 0;
		if (kthread_should_stop())
			break;

		if (state->mode == MSP_MODE_EXTERN) {
			/* no carrier scan needed, just unmute */
			dev_dbg_lvl(&client->dev, 1, msp_debug,
				"thread: no carrier scan\n");
			state->scan_in_progress = 0;
			msp_update_volume(state);
			continue;
		}

		/* mute audio */
		state->scan_in_progress = 1;
		msp_update_volume(state);

		/* start autodetect. Note: autodetect is not supported for
		   NTSC-M and radio, hence we force the standard in those
		   cases. */
		if (state->radio)
			std = 0x40;
		else
			std = (state->v4l2_std & V4L2_STD_NTSC) ? 0x20 : 1;
		state->watch_stereo = 0;
		state->nicam_on = 0;

		/* wait for tuner to settle down after a channel change */
		if (msp_sleep(state, 200))
			goto restart;

		if (msp_debug)
			dev_dbg_lvl(&client->dev, 2, msp_debug,
				"setting standard: %s (0x%04x)\n",
				msp_standard_std_name(std), std);

		if (std != 1) {
			/* programmed some specific mode */
			val = std;
		} else {
			/* triggered autodetect */
			msp_write_dem(client, 0x20, std);
			for (;;) {
				if (msp_sleep(state, 100))
					goto restart;

				/* check results */
				val = msp_read_dem(client, 0x7e);
				if (val < 0x07ff)
					break;
				dev_dbg_lvl(&client->dev, 2, msp_debug,
					"detection still in progress\n");
			}
		}
		for (i = 0; msp_stdlist[i].name != NULL; i++)
			if (msp_stdlist[i].retval == val)
				break;
		dev_dbg_lvl(&client->dev, 1, msp_debug, "current standard: %s (0x%04x)\n",
			msp_standard_std_name(val), val);
		state->main   = msp_stdlist[i].main;
		state->second = msp_stdlist[i].second;
		state->std = val;
		state->rxsubchans = V4L2_TUNER_SUB_MONO;

		if (msp_amsound && !state->radio &&
		    (state->v4l2_std & V4L2_STD_SECAM) && (val != 0x0009)) {
			/* autodetection has failed, let backup */
			dev_dbg_lvl(&client->dev, 1, msp_debug, "autodetection failed, switching to backup standard: %s (0x%04x)\n",
				msp_stdlist[8].name ?
					msp_stdlist[8].name : "unknown", val);
			state->std = val = 0x0009;
			msp_write_dem(client, 0x20, val);
		} else {
			state->detected_std = msp_standard_std(state->std);
		}

		/* set stereo */
		switch (val) {
		case 0x0008: /* B/G NICAM */
		case 0x000a: /* I NICAM */
		case 0x000b: /* D/K NICAM */
			if (val == 0x000a)
				state->mode = MSP_MODE_FM_NICAM2;
			else
				state->mode = MSP_MODE_FM_NICAM1;
			/* just turn on stereo */
			state->nicam_on = 1;
			state->watch_stereo = 1;
			break;
		case 0x0009:
			state->mode = MSP_MODE_AM_NICAM;
			state->nicam_on = 1;
			state->watch_stereo = 1;
			break;
		case 0x0020: /* BTSC */
			/* The pre-'G' models only have BTSC-mono */
			state->mode = MSP_MODE_BTSC;
			break;
		case 0x0040: /* FM radio */
			state->mode = MSP_MODE_FM_RADIO;
			state->rxsubchans = V4L2_TUNER_SUB_STEREO;
			/* not needed in theory if we have radio, but
			   short programming enables carrier mute */
			msp3400c_set_mode(client, MSP_MODE_FM_RADIO);
			msp3400c_set_carrier(client, MSP_CARRIER(10.7),
					    MSP_CARRIER(10.7));
			break;
		case 0x0002:
		case 0x0003:
		case 0x0004:
		case 0x0005:
			state->mode = MSP_MODE_FM_TERRA;
			state->watch_stereo = 1;
			break;
		}

		/* set various prescales */
		msp_write_dsp(client, 0x0d, 0x1900); /* scart */
		msp_write_dsp(client, 0x0e, 0x3000); /* FM */
		if (state->has_nicam)
			msp_write_dsp(client, 0x10, 0x5a00); /* nicam */

		if (state->has_i2s_conf)
			msp_write_dem(client, 0x40, state->i2s_mode);

		/* unmute */
		msp3400c_set_audmode(client);
		state->scan_in_progress = 0;
		msp_update_volume(state);

		/* monitor tv audio mode, the first time don't wait
		   so long to get a quick stereo/bilingual result */
		count = 3;
		while (state->watch_stereo) {
			if (msp_sleep(state, count ? 1000 : 5000))
				goto restart;
			if (count)
				count--;
			watch_stereo(client);
		}
	}
	dev_dbg_lvl(&client->dev, 1, msp_debug, "thread: exit\n");
	return 0;
}

/* ----------------------------------------------------------------------- */

/* msp34xxG + (autoselect no-thread)
 * this one uses both automatic standard detection and automatic sound
 * select which are available in the newer G versions
 * struct msp: only norm, acb and source are really used in this mode
 */

static int msp34xxg_modus(struct i2c_client *client)
{
	struct msp_state *state = to_state(i2c_get_clientdata(client));

	if (state->radio) {
		dev_dbg_lvl(&client->dev, 1, msp_debug, "selected radio modus\n");
		return 0x0001;
	}
	if (state->v4l2_std == V4L2_STD_NTSC_M_JP) {
		dev_dbg_lvl(&client->dev, 1, msp_debug, "selected M (EIA-J) modus\n");
		return 0x4001;
	}
	if (state->v4l2_std == V4L2_STD_NTSC_M_KR) {
		dev_dbg_lvl(&client->dev, 1, msp_debug, "selected M (A2) modus\n");
		return 0x0001;
	}
	if (state->v4l2_std == V4L2_STD_SECAM_L) {
		dev_dbg_lvl(&client->dev, 1, msp_debug, "selected SECAM-L modus\n");
		return 0x6001;
	}
	if (state->v4l2_std & V4L2_STD_MN) {
		dev_dbg_lvl(&client->dev, 1, msp_debug, "selected M (BTSC) modus\n");
		return 0x2001;
	}
	return 0x7001;
}

static void msp34xxg_set_source(struct i2c_client *client, u16 reg, int in)
{
	struct msp_state *state = to_state(i2c_get_clientdata(client));
	int source, matrix;

	switch (state->audmode) {
	case V4L2_TUNER_MODE_MONO:
		source = 0; /* mono only */
		matrix = 0x30;
		break;
	case V4L2_TUNER_MODE_LANG2:
		source = 4; /* stereo or B */
		matrix = 0x10;
		break;
	case V4L2_TUNER_MODE_LANG1_LANG2:
		source = 1; /* stereo or A|B */
		matrix = 0x20;
		break;
	case V4L2_TUNER_MODE_LANG1:
		source = 3; /* stereo or A */
		matrix = 0x00;
		break;
	case V4L2_TUNER_MODE_STEREO:
	default:
		source = 3; /* stereo or A */
		matrix = 0x20;
		break;
	}

	if (in == MSP_DSP_IN_TUNER)
		source = (source << 8) | 0x20;
	/* the msp34x2g puts the MAIN_AVC, MAIN and AUX sources in 12, 13, 14
	   instead of 11, 12, 13. So we add one for that msp version. */
	else if (in >= MSP_DSP_IN_MAIN_AVC && state->has_dolby_pro_logic)
		source = ((in + 1) << 8) | matrix;
	else
		source = (in << 8) | matrix;

	dev_dbg_lvl(&client->dev, 1, msp_debug,
		"set source to %d (0x%x) for output %02x\n", in, source, reg);
	msp_write_dsp(client, reg, source);
}

static void msp34xxg_set_sources(struct i2c_client *client)
{
	struct msp_state *state = to_state(i2c_get_clientdata(client));
	u32 in = state->route_in;

	msp34xxg_set_source(client, 0x0008, (in >> 4) & 0xf);
	/* quasi-peak detector is set to same input as the loudspeaker (MAIN) */
	msp34xxg_set_source(client, 0x000c, (in >> 4) & 0xf);
	msp34xxg_set_source(client, 0x0009, (in >> 8) & 0xf);
	msp34xxg_set_source(client, 0x000a, (in >> 12) & 0xf);
	if (state->has_scart2_out)
		msp34xxg_set_source(client, 0x0041, (in >> 16) & 0xf);
	msp34xxg_set_source(client, 0x000b, (in >> 20) & 0xf);
}

/* (re-)initialize the msp34xxg */
static void msp34xxg_reset(struct i2c_client *client)
{
	struct msp_state *state = to_state(i2c_get_clientdata(client));
	int tuner = (state->route_in >> 3) & 1;
	int modus;

	/* initialize std to 1 (autodetect) to signal that no standard is
	   selected yet. */
	state->std = 1;

	msp_reset(client);

	if (state->has_i2s_conf)
		msp_write_dem(client, 0x40, state->i2s_mode);

	/* step-by-step initialisation, as described in the manual */
	modus = msp34xxg_modus(client);
	modus |= tuner ? 0x100 : 0;
	msp_write_dem(client, 0x30, modus);

	/* write the dsps that may have an influence on
	   standard/audio autodetection right now */
	msp34xxg_set_sources(client);

	msp_write_dsp(client, 0x0d, 0x1900); /* scart */
	msp_write_dsp(client, 0x0e, 0x3000); /* FM */
	if (state->has_nicam)
		msp_write_dsp(client, 0x10, 0x5a00); /* nicam */

	/* set identification threshold. Personally, I
	 * I set it to a higher value than the default
	 * of 0x190 to ignore noisy stereo signals.
	 * this needs tuning. (recommended range 0x00a0-0x03c0)
	 * 0x7f0 = forced mono mode
	 *
	 * a2 threshold for stereo/bilingual.
	 * Note: this register is part of the Manual/Compatibility mode.
	 * It is supported by all 'G'-family chips.
	 */
	msp_write_dem(client, 0x22, msp_stereo_thresh);
}

int msp34xxg_thread(void *data)
{
	struct i2c_client *client = data;
	struct msp_state *state = to_state(i2c_get_clientdata(client));
	int val, i;

	dev_dbg_lvl(&client->dev, 1, msp_debug, "msp34xxg daemon started\n");
	state->detected_std = V4L2_STD_ALL;
	set_freezable();
	for (;;) {
		dev_dbg_lvl(&client->dev, 2, msp_debug, "msp34xxg thread: sleep\n");
		msp_sleep(state, -1);
		dev_dbg_lvl(&client->dev, 2, msp_debug, "msp34xxg thread: wakeup\n");

restart:
		dev_dbg_lvl(&client->dev, 1, msp_debug, "thread: restart scan\n");
		state->restart = 0;
		if (kthread_should_stop())
			break;

		if (state->mode == MSP_MODE_EXTERN) {
			/* no carrier scan needed, just unmute */
			dev_dbg_lvl(&client->dev, 1, msp_debug,
				"thread: no carrier scan\n");
			state->scan_in_progress = 0;
			msp_update_volume(state);
			continue;
		}

		/* setup the chip*/
		msp34xxg_reset(client);
		state->std = state->radio ? 0x40 :
			(state->force_btsc && msp_standard == 1) ? 32 : msp_standard;
		msp_write_dem(client, 0x20, state->std);
		/* start autodetect */
		if (state->std != 1)
			goto unmute;

		/* watch autodetect */
		dev_dbg_lvl(&client->dev, 1, msp_debug,
			"started autodetect, waiting for result\n");
		for (i = 0; i < 10; i++) {
			if (msp_sleep(state, 100))
				goto restart;

			/* check results */
			val = msp_read_dem(client, 0x7e);
			if (val < 0x07ff) {
				state->std = val;
				break;
			}
			dev_dbg_lvl(&client->dev, 2, msp_debug,
				"detection still in progress\n");
		}
		if (state->std == 1) {
			dev_dbg_lvl(&client->dev, 1, msp_debug,
				"detection still in progress after 10 tries. giving up.\n");
			continue;
		}

unmute:
		dev_dbg_lvl(&client->dev, 1, msp_debug,
			"detected standard: %s (0x%04x)\n",
			msp_standard_std_name(state->std), state->std);
		state->detected_std = msp_standard_std(state->std);

		if (state->std == 9) {
			/* AM NICAM mode */
			msp_write_dsp(client, 0x0e, 0x7c00);
		}

		/* unmute: dispatch sound to scart output, set scart volume */
		msp_update_volume(state);

		/* restore ACB */
		if (msp_write_dsp(client, 0x13, state->acb))
			return -1;

		/* the periodic stereo/SAP check is only relevant for
		   the 0x20 standard (BTSC) */
		if (state->std != 0x20)
			continue;

		state->watch_stereo = 1;

		/* monitor tv audio mode, the first time don't wait
		   in order to get a quick stereo/SAP update */
		watch_stereo(client);
		while (state->watch_stereo) {
			watch_stereo(client);
			if (msp_sleep(state, 5000))
				goto restart;
		}
	}
	dev_dbg_lvl(&client->dev, 1, msp_debug, "thread: exit\n");
	return 0;
}

static int msp34xxg_detect_stereo(struct i2c_client *client)
{
	struct msp_state *state = to_state(i2c_get_clientdata(client));
	int status = msp_read_dem(client, 0x0200);
	int is_bilingual = status & 0x100;
	int is_stereo = status & 0x40;
	int oldrx = state->rxsubchans;

	if (state->mode == MSP_MODE_EXTERN)
		return 0;

	state->rxsubchans = 0;
	if (is_stereo)
		state->rxsubchans = V4L2_TUNER_SUB_STEREO;
	else
		state->rxsubchans = V4L2_TUNER_SUB_MONO;
	if (is_bilingual) {
		if (state->std == 0x20)
			state->rxsubchans |= V4L2_TUNER_SUB_SAP;
		else
			state->rxsubchans =
				V4L2_TUNER_SUB_LANG1 | V4L2_TUNER_SUB_LANG2;
	}
	dev_dbg_lvl(&client->dev, 1, msp_debug,
		"status=0x%x, stereo=%d, bilingual=%d -> rxsubchans=%d\n",
		status, is_stereo, is_bilingual, state->rxsubchans);
	return (oldrx != state->rxsubchans);
}

static void msp34xxg_set_audmode(struct i2c_client *client)
{
	struct msp_state *state = to_state(i2c_get_clientdata(client));

	if (state->std == 0x20) {
	       if ((state->rxsubchans & V4L2_TUNER_SUB_SAP) &&
		   (state->audmode == V4L2_TUNER_MODE_LANG1_LANG2 ||
		    state->audmode == V4L2_TUNER_MODE_LANG2)) {
			msp_write_dem(client, 0x20, 0x21);
	       } else {
			msp_write_dem(client, 0x20, 0x20);
	       }
	}

	msp34xxg_set_sources(client);
}

void msp_set_audmode(struct i2c_client *client)
{
	struct msp_state *state = to_state(i2c_get_clientdata(client));

	switch (state->opmode) {
	case OPMODE_MANUAL:
	case OPMODE_AUTODETECT:
		msp3400c_set_audmode(client);
		break;
	case OPMODE_AUTOSELECT:
		msp34xxg_set_audmode(client);
		break;
	}
}

int msp_detect_stereo(struct i2c_client *client)
{
	struct msp_state *state  = to_state(i2c_get_clientdata(client));

	switch (state->opmode) {
	case OPMODE_MANUAL:
	case OPMODE_AUTODETECT:
		return msp3400c_detect_stereo(client);
	case OPMODE_AUTOSELECT:
		return msp34xxg_detect_stereo(client);
	}
	return 0;
}

