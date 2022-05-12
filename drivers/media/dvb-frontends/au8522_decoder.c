// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Auvitek AU8522 QAM/8VSB demodulator driver and video decoder
 *
 * Copyright (C) 2009 Devin Heitmueller <dheitmueller@linuxtv.org>
 * Copyright (C) 2005-2008 Auvitek International, Ltd.
 */

/* Developer notes:
 *
 * Enough is implemented here for CVBS and S-Video inputs, but the actual
 *  analog demodulator code isn't implemented (not needed for xc5000 since it
 *  has its own demodulator and outputs CVBS)
 *
 */

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <media/v4l2-common.h>
#include <media/v4l2-device.h>
#include "au8522.h"
#include "au8522_priv.h"

MODULE_AUTHOR("Devin Heitmueller");
MODULE_LICENSE("GPL");

static int au8522_analog_debug;


module_param_named(analog_debug, au8522_analog_debug, int, 0644);

MODULE_PARM_DESC(analog_debug,
		 "Analog debugging messages [0=Off (default) 1=On]");

struct au8522_register_config {
	u16 reg_name;
	u8 reg_val[8];
};


/* Video Decoder Filter Coefficients
   The values are as follows from left to right
   0="ATV RF" 1="ATV RF13" 2="CVBS" 3="S-Video" 4="PAL" 5=CVBS13" 6="SVideo13"
*/
static const struct au8522_register_config filter_coef[] = {
	{AU8522_FILTER_COEF_R410, {0x25, 0x00, 0x25, 0x25, 0x00, 0x00, 0x00} },
	{AU8522_FILTER_COEF_R411, {0x20, 0x00, 0x20, 0x20, 0x00, 0x00, 0x00} },
	{AU8522_FILTER_COEF_R412, {0x03, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00} },
	{AU8522_FILTER_COEF_R413, {0xe6, 0x00, 0xe6, 0xe6, 0x00, 0x00, 0x00} },
	{AU8522_FILTER_COEF_R414, {0x40, 0x00, 0x40, 0x40, 0x00, 0x00, 0x00} },
	{AU8522_FILTER_COEF_R415, {0x1b, 0x00, 0x1b, 0x1b, 0x00, 0x00, 0x00} },
	{AU8522_FILTER_COEF_R416, {0xc0, 0x00, 0xc0, 0x04, 0x00, 0x00, 0x00} },
	{AU8522_FILTER_COEF_R417, {0x04, 0x00, 0x04, 0x04, 0x00, 0x00, 0x00} },
	{AU8522_FILTER_COEF_R418, {0x8c, 0x00, 0x8c, 0x8c, 0x00, 0x00, 0x00} },
	{AU8522_FILTER_COEF_R419, {0xa0, 0x40, 0xa0, 0xa0, 0x40, 0x40, 0x40} },
	{AU8522_FILTER_COEF_R41A, {0x21, 0x09, 0x21, 0x21, 0x09, 0x09, 0x09} },
	{AU8522_FILTER_COEF_R41B, {0x6c, 0x38, 0x6c, 0x6c, 0x38, 0x38, 0x38} },
	{AU8522_FILTER_COEF_R41C, {0x03, 0xff, 0x03, 0x03, 0xff, 0xff, 0xff} },
	{AU8522_FILTER_COEF_R41D, {0xbf, 0xc7, 0xbf, 0xbf, 0xc7, 0xc7, 0xc7} },
	{AU8522_FILTER_COEF_R41E, {0xa0, 0xdf, 0xa0, 0xa0, 0xdf, 0xdf, 0xdf} },
	{AU8522_FILTER_COEF_R41F, {0x10, 0x06, 0x10, 0x10, 0x06, 0x06, 0x06} },
	{AU8522_FILTER_COEF_R420, {0xae, 0x30, 0xae, 0xae, 0x30, 0x30, 0x30} },
	{AU8522_FILTER_COEF_R421, {0xc4, 0x01, 0xc4, 0xc4, 0x01, 0x01, 0x01} },
	{AU8522_FILTER_COEF_R422, {0x54, 0xdd, 0x54, 0x54, 0xdd, 0xdd, 0xdd} },
	{AU8522_FILTER_COEF_R423, {0xd0, 0xaf, 0xd0, 0xd0, 0xaf, 0xaf, 0xaf} },
	{AU8522_FILTER_COEF_R424, {0x1c, 0xf7, 0x1c, 0x1c, 0xf7, 0xf7, 0xf7} },
	{AU8522_FILTER_COEF_R425, {0x76, 0xdb, 0x76, 0x76, 0xdb, 0xdb, 0xdb} },
	{AU8522_FILTER_COEF_R426, {0x61, 0xc0, 0x61, 0x61, 0xc0, 0xc0, 0xc0} },
	{AU8522_FILTER_COEF_R427, {0xd1, 0x2f, 0xd1, 0xd1, 0x2f, 0x2f, 0x2f} },
	{AU8522_FILTER_COEF_R428, {0x84, 0xd8, 0x84, 0x84, 0xd8, 0xd8, 0xd8} },
	{AU8522_FILTER_COEF_R429, {0x06, 0xfb, 0x06, 0x06, 0xfb, 0xfb, 0xfb} },
	{AU8522_FILTER_COEF_R42A, {0x21, 0xd5, 0x21, 0x21, 0xd5, 0xd5, 0xd5} },
	{AU8522_FILTER_COEF_R42B, {0x0a, 0x3e, 0x0a, 0x0a, 0x3e, 0x3e, 0x3e} },
	{AU8522_FILTER_COEF_R42C, {0xe6, 0x15, 0xe6, 0xe6, 0x15, 0x15, 0x15} },
	{AU8522_FILTER_COEF_R42D, {0x01, 0x34, 0x01, 0x01, 0x34, 0x34, 0x34} },

};
#define NUM_FILTER_COEF (sizeof(filter_coef)\
			 / sizeof(struct au8522_register_config))


/* Registers 0x060b through 0x0652 are the LP Filter coefficients
   The values are as follows from left to right
   0="SIF" 1="ATVRF/ATVRF13"
   Note: the "ATVRF/ATVRF13" mode has never been tested
*/
static const struct au8522_register_config lpfilter_coef[] = {
	{0x060b, {0x21, 0x0b} },
	{0x060c, {0xad, 0xad} },
	{0x060d, {0x70, 0xf0} },
	{0x060e, {0xea, 0xe9} },
	{0x060f, {0xdd, 0xdd} },
	{0x0610, {0x08, 0x64} },
	{0x0611, {0x60, 0x60} },
	{0x0612, {0xf8, 0xb2} },
	{0x0613, {0x01, 0x02} },
	{0x0614, {0xe4, 0xb4} },
	{0x0615, {0x19, 0x02} },
	{0x0616, {0xae, 0x2e} },
	{0x0617, {0xee, 0xc5} },
	{0x0618, {0x56, 0x56} },
	{0x0619, {0x30, 0x58} },
	{0x061a, {0xf9, 0xf8} },
	{0x061b, {0x24, 0x64} },
	{0x061c, {0x07, 0x07} },
	{0x061d, {0x30, 0x30} },
	{0x061e, {0xa9, 0xed} },
	{0x061f, {0x09, 0x0b} },
	{0x0620, {0x42, 0xc2} },
	{0x0621, {0x1d, 0x2a} },
	{0x0622, {0xd6, 0x56} },
	{0x0623, {0x95, 0x8b} },
	{0x0624, {0x2b, 0x2b} },
	{0x0625, {0x30, 0x24} },
	{0x0626, {0x3e, 0x3e} },
	{0x0627, {0x62, 0xe2} },
	{0x0628, {0xe9, 0xf5} },
	{0x0629, {0x99, 0x19} },
	{0x062a, {0xd4, 0x11} },
	{0x062b, {0x03, 0x04} },
	{0x062c, {0xb5, 0x85} },
	{0x062d, {0x1e, 0x20} },
	{0x062e, {0x2a, 0xea} },
	{0x062f, {0xd7, 0xd2} },
	{0x0630, {0x15, 0x15} },
	{0x0631, {0xa3, 0xa9} },
	{0x0632, {0x1f, 0x1f} },
	{0x0633, {0xf9, 0xd1} },
	{0x0634, {0xc0, 0xc3} },
	{0x0635, {0x4d, 0x8d} },
	{0x0636, {0x21, 0x31} },
	{0x0637, {0x83, 0x83} },
	{0x0638, {0x08, 0x8c} },
	{0x0639, {0x19, 0x19} },
	{0x063a, {0x45, 0xa5} },
	{0x063b, {0xef, 0xec} },
	{0x063c, {0x8a, 0x8a} },
	{0x063d, {0xf4, 0xf6} },
	{0x063e, {0x8f, 0x8f} },
	{0x063f, {0x44, 0x0c} },
	{0x0640, {0xef, 0xf0} },
	{0x0641, {0x66, 0x66} },
	{0x0642, {0xcc, 0xd2} },
	{0x0643, {0x41, 0x41} },
	{0x0644, {0x63, 0x93} },
	{0x0645, {0x8e, 0x8e} },
	{0x0646, {0xa2, 0x42} },
	{0x0647, {0x7b, 0x7b} },
	{0x0648, {0x04, 0x04} },
	{0x0649, {0x00, 0x00} },
	{0x064a, {0x40, 0x40} },
	{0x064b, {0x8c, 0x98} },
	{0x064c, {0x00, 0x00} },
	{0x064d, {0x63, 0xc3} },
	{0x064e, {0x04, 0x04} },
	{0x064f, {0x20, 0x20} },
	{0x0650, {0x00, 0x00} },
	{0x0651, {0x40, 0x40} },
	{0x0652, {0x01, 0x01} },
};
#define NUM_LPFILTER_COEF (sizeof(lpfilter_coef)\
			   / sizeof(struct au8522_register_config))

static inline struct au8522_state *to_state(struct v4l2_subdev *sd)
{
	return container_of(sd, struct au8522_state, sd);
}

static void setup_decoder_defaults(struct au8522_state *state, bool is_svideo)
{
	int i;
	int filter_coef_type;

	/* Provide reasonable defaults for picture tuning values */
	au8522_writereg(state, AU8522_TVDEC_SHARPNESSREG009H, 0x07);
	au8522_writereg(state, AU8522_TVDEC_BRIGHTNESS_REG00AH, 0xed);
	au8522_writereg(state, AU8522_TVDEC_CONTRAST_REG00BH, 0x79);
	au8522_writereg(state, AU8522_TVDEC_SATURATION_CB_REG00CH, 0x80);
	au8522_writereg(state, AU8522_TVDEC_SATURATION_CR_REG00DH, 0x80);
	au8522_writereg(state, AU8522_TVDEC_HUE_H_REG00EH, 0x00);
	au8522_writereg(state, AU8522_TVDEC_HUE_L_REG00FH, 0x00);

	/* Other decoder registers */
	au8522_writereg(state, AU8522_TVDEC_INT_MASK_REG010H, 0x00);

	if (is_svideo)
		au8522_writereg(state, AU8522_VIDEO_MODE_REG011H, 0x04);
	else
		au8522_writereg(state, AU8522_VIDEO_MODE_REG011H, 0x00);

	au8522_writereg(state, AU8522_TVDEC_PGA_REG012H,
			AU8522_TVDEC_PGA_REG012H_CVBS);
	au8522_writereg(state, AU8522_TVDEC_COMB_MODE_REG015H,
			AU8522_TVDEC_COMB_MODE_REG015H_CVBS);
	au8522_writereg(state, AU8522_TVDED_DBG_MODE_REG060H,
			AU8522_TVDED_DBG_MODE_REG060H_CVBS);

	if (state->std == V4L2_STD_PAL_M) {
		au8522_writereg(state, AU8522_TVDEC_FORMAT_CTRL1_REG061H,
				AU8522_TVDEC_FORMAT_CTRL1_REG061H_FIELD_LEN_525 |
				AU8522_TVDEC_FORMAT_CTRL1_REG061H_LINE_LEN_63_492 |
				AU8522_TVDEC_FORMAT_CTRL1_REG061H_SUBCARRIER_NTSC_AUTO);
		au8522_writereg(state, AU8522_TVDEC_FORMAT_CTRL2_REG062H,
				AU8522_TVDEC_FORMAT_CTRL2_REG062H_STD_PAL_M);
	} else {
		/* NTSC */
		au8522_writereg(state, AU8522_TVDEC_FORMAT_CTRL1_REG061H,
				AU8522_TVDEC_FORMAT_CTRL1_REG061H_FIELD_LEN_525 |
				AU8522_TVDEC_FORMAT_CTRL1_REG061H_LINE_LEN_63_492 |
				AU8522_TVDEC_FORMAT_CTRL1_REG061H_SUBCARRIER_NTSC_MN);
		au8522_writereg(state, AU8522_TVDEC_FORMAT_CTRL2_REG062H,
				AU8522_TVDEC_FORMAT_CTRL2_REG062H_STD_NTSC);
	}
	au8522_writereg(state, AU8522_TVDEC_VCR_DET_LLIM_REG063H,
			AU8522_TVDEC_VCR_DET_LLIM_REG063H_CVBS);
	au8522_writereg(state, AU8522_TVDEC_VCR_DET_HLIM_REG064H,
			AU8522_TVDEC_VCR_DET_HLIM_REG064H_CVBS);
	au8522_writereg(state, AU8522_TVDEC_COMB_VDIF_THR1_REG065H,
			AU8522_TVDEC_COMB_VDIF_THR1_REG065H_CVBS);
	au8522_writereg(state, AU8522_TVDEC_COMB_VDIF_THR2_REG066H,
			AU8522_TVDEC_COMB_VDIF_THR2_REG066H_CVBS);
	au8522_writereg(state, AU8522_TVDEC_COMB_VDIF_THR3_REG067H,
			AU8522_TVDEC_COMB_VDIF_THR3_REG067H_CVBS);
	au8522_writereg(state, AU8522_TVDEC_COMB_NOTCH_THR_REG068H,
			AU8522_TVDEC_COMB_NOTCH_THR_REG068H_CVBS);
	au8522_writereg(state, AU8522_TVDEC_COMB_HDIF_THR1_REG069H,
			AU8522_TVDEC_COMB_HDIF_THR1_REG069H_CVBS);
	au8522_writereg(state, AU8522_TVDEC_COMB_HDIF_THR2_REG06AH,
			AU8522_TVDEC_COMB_HDIF_THR2_REG06AH_CVBS);
	au8522_writereg(state, AU8522_TVDEC_COMB_HDIF_THR3_REG06BH,
			AU8522_TVDEC_COMB_HDIF_THR3_REG06BH_CVBS);
	if (is_svideo) {
		au8522_writereg(state, AU8522_TVDEC_COMB_DCDIF_THR1_REG06CH,
				AU8522_TVDEC_COMB_DCDIF_THR1_REG06CH_SVIDEO);
		au8522_writereg(state, AU8522_TVDEC_COMB_DCDIF_THR2_REG06DH,
				AU8522_TVDEC_COMB_DCDIF_THR2_REG06DH_SVIDEO);
	} else {
		au8522_writereg(state, AU8522_TVDEC_COMB_DCDIF_THR1_REG06CH,
				AU8522_TVDEC_COMB_DCDIF_THR1_REG06CH_CVBS);
		au8522_writereg(state, AU8522_TVDEC_COMB_DCDIF_THR2_REG06DH,
				AU8522_TVDEC_COMB_DCDIF_THR2_REG06DH_CVBS);
	}
	au8522_writereg(state, AU8522_TVDEC_COMB_DCDIF_THR3_REG06EH,
			AU8522_TVDEC_COMB_DCDIF_THR3_REG06EH_CVBS);
	au8522_writereg(state, AU8522_TVDEC_UV_SEP_THR_REG06FH,
			AU8522_TVDEC_UV_SEP_THR_REG06FH_CVBS);
	au8522_writereg(state, AU8522_TVDEC_COMB_DC_THR1_NTSC_REG070H,
			AU8522_TVDEC_COMB_DC_THR1_NTSC_REG070H_CVBS);
	au8522_writereg(state, AU8522_REG071H, AU8522_REG071H_CVBS);
	au8522_writereg(state, AU8522_REG072H, AU8522_REG072H_CVBS);
	au8522_writereg(state, AU8522_TVDEC_COMB_DC_THR2_NTSC_REG073H,
			AU8522_TVDEC_COMB_DC_THR2_NTSC_REG073H_CVBS);
	au8522_writereg(state, AU8522_REG074H, AU8522_REG074H_CVBS);
	au8522_writereg(state, AU8522_REG075H, AU8522_REG075H_CVBS);
	au8522_writereg(state, AU8522_TVDEC_DCAGC_CTRL_REG077H,
			AU8522_TVDEC_DCAGC_CTRL_REG077H_CVBS);
	au8522_writereg(state, AU8522_TVDEC_PIC_START_ADJ_REG078H,
			AU8522_TVDEC_PIC_START_ADJ_REG078H_CVBS);
	au8522_writereg(state, AU8522_TVDEC_AGC_HIGH_LIMIT_REG079H,
			AU8522_TVDEC_AGC_HIGH_LIMIT_REG079H_CVBS);
	au8522_writereg(state, AU8522_TVDEC_MACROVISION_SYNC_THR_REG07AH,
			AU8522_TVDEC_MACROVISION_SYNC_THR_REG07AH_CVBS);
	au8522_writereg(state, AU8522_TVDEC_INTRP_CTRL_REG07BH,
			AU8522_TVDEC_INTRP_CTRL_REG07BH_CVBS);
	au8522_writereg(state, AU8522_TVDEC_AGC_LOW_LIMIT_REG0E4H,
			AU8522_TVDEC_AGC_LOW_LIMIT_REG0E4H_CVBS);
	au8522_writereg(state, AU8522_TOREGAAGC_REG0E5H,
			AU8522_TOREGAAGC_REG0E5H_CVBS);
	au8522_writereg(state, AU8522_REG016H, AU8522_REG016H_CVBS);

	/*
	 * Despite what the table says, for the HVR-950q we still need
	 * to be in CVBS mode for the S-Video input (reason unknown).
	 */
	/* filter_coef_type = 3; */
	filter_coef_type = 5;

	/* Load the Video Decoder Filter Coefficients */
	for (i = 0; i < NUM_FILTER_COEF; i++) {
		au8522_writereg(state, filter_coef[i].reg_name,
				filter_coef[i].reg_val[filter_coef_type]);
	}

	/* It's not clear what these registers are for, but they are always
	   set to the same value regardless of what mode we're in */
	au8522_writereg(state, AU8522_REG42EH, 0x87);
	au8522_writereg(state, AU8522_REG42FH, 0xa2);
	au8522_writereg(state, AU8522_REG430H, 0xbf);
	au8522_writereg(state, AU8522_REG431H, 0xcb);
	au8522_writereg(state, AU8522_REG432H, 0xa1);
	au8522_writereg(state, AU8522_REG433H, 0x41);
	au8522_writereg(state, AU8522_REG434H, 0x88);
	au8522_writereg(state, AU8522_REG435H, 0xc2);
	au8522_writereg(state, AU8522_REG436H, 0x3c);
}

static void au8522_setup_cvbs_mode(struct au8522_state *state, u8 input_mode)
{
	/* here we're going to try the pre-programmed route */
	au8522_writereg(state, AU8522_MODULE_CLOCK_CONTROL_REG0A3H,
			AU8522_MODULE_CLOCK_CONTROL_REG0A3H_CVBS);

	/* PGA in automatic mode */
	au8522_writereg(state, AU8522_PGA_CONTROL_REG082H, 0x00);

	/* Enable clamping control */
	au8522_writereg(state, AU8522_CLAMPING_CONTROL_REG083H, 0x00);

	au8522_writereg(state, AU8522_INPUT_CONTROL_REG081H, input_mode);

	setup_decoder_defaults(state, false);

	au8522_writereg(state, AU8522_SYSTEM_MODULE_CONTROL_0_REG0A4H,
			AU8522_SYSTEM_MODULE_CONTROL_0_REG0A4H_CVBS);
}

static void au8522_setup_cvbs_tuner_mode(struct au8522_state *state,
					 u8 input_mode)
{
	/* here we're going to try the pre-programmed route */
	au8522_writereg(state, AU8522_MODULE_CLOCK_CONTROL_REG0A3H,
			AU8522_MODULE_CLOCK_CONTROL_REG0A3H_CVBS);

	/* It's not clear why we have to have the PGA in automatic mode while
	   enabling clamp control, but it's what Windows does */
	au8522_writereg(state, AU8522_PGA_CONTROL_REG082H, 0x00);

	/* Enable clamping control */
	au8522_writereg(state, AU8522_CLAMPING_CONTROL_REG083H, 0x0e);

	/* Disable automatic PGA (since the CVBS is coming from the tuner) */
	au8522_writereg(state, AU8522_PGA_CONTROL_REG082H, 0x10);

	/* Set input mode to CVBS on channel 4 with SIF audio input enabled */
	au8522_writereg(state, AU8522_INPUT_CONTROL_REG081H, input_mode);

	setup_decoder_defaults(state, false);

	au8522_writereg(state, AU8522_SYSTEM_MODULE_CONTROL_0_REG0A4H,
			AU8522_SYSTEM_MODULE_CONTROL_0_REG0A4H_CVBS);
}

static void au8522_setup_svideo_mode(struct au8522_state *state,
				     u8 input_mode)
{
	au8522_writereg(state, AU8522_MODULE_CLOCK_CONTROL_REG0A3H,
			AU8522_MODULE_CLOCK_CONTROL_REG0A3H_SVIDEO);

	/* Set input to Y on Channe1, C on Channel 3 */
	au8522_writereg(state, AU8522_INPUT_CONTROL_REG081H, input_mode);

	/* PGA in automatic mode */
	au8522_writereg(state, AU8522_PGA_CONTROL_REG082H, 0x00);

	/* Enable clamping control */
	au8522_writereg(state, AU8522_CLAMPING_CONTROL_REG083H, 0x00);

	setup_decoder_defaults(state, true);

	au8522_writereg(state, AU8522_SYSTEM_MODULE_CONTROL_0_REG0A4H,
			AU8522_SYSTEM_MODULE_CONTROL_0_REG0A4H_CVBS);
}

/* ----------------------------------------------------------------------- */

static void disable_audio_input(struct au8522_state *state)
{
	au8522_writereg(state, AU8522_AUDIO_VOLUME_L_REG0F2H, 0x00);
	au8522_writereg(state, AU8522_AUDIO_VOLUME_R_REG0F3H, 0x00);
	au8522_writereg(state, AU8522_AUDIO_VOLUME_REG0F4H, 0x00);

	au8522_writereg(state, AU8522_SYSTEM_MODULE_CONTROL_1_REG0A5H, 0x04);
	au8522_writereg(state, AU8522_I2S_CTRL_2_REG112H, 0x02);

	au8522_writereg(state, AU8522_SYSTEM_MODULE_CONTROL_0_REG0A4H,
			AU8522_SYSTEM_MODULE_CONTROL_0_REG0A4H_SVIDEO);
}

/* 0=disable, 1=SIF */
static void set_audio_input(struct au8522_state *state)
{
	int aud_input = state->aud_input;
	int i;

	/* Note that this function needs to be used in conjunction with setting
	   the input routing via register 0x81 */

	if (aud_input == AU8522_AUDIO_NONE) {
		disable_audio_input(state);
		return;
	}

	if (aud_input != AU8522_AUDIO_SIF) {
		/* The caller asked for a mode we don't currently support */
		printk(KERN_ERR "Unsupported audio mode requested! mode=%d\n",
		       aud_input);
		return;
	}

	/* Load the Audio Decoder Filter Coefficients */
	for (i = 0; i < NUM_LPFILTER_COEF; i++) {
		au8522_writereg(state, lpfilter_coef[i].reg_name,
				lpfilter_coef[i].reg_val[0]);
	}

	/* Set the volume */
	au8522_writereg(state, AU8522_AUDIO_VOLUME_L_REG0F2H, 0x7F);
	au8522_writereg(state, AU8522_AUDIO_VOLUME_R_REG0F3H, 0x7F);
	au8522_writereg(state, AU8522_AUDIO_VOLUME_REG0F4H, 0xff);

	/* Not sure what this does */
	au8522_writereg(state, AU8522_REG0F9H, AU8522_REG0F9H_AUDIO);

	/* Setup the audio mode to stereo DBX */
	au8522_writereg(state, AU8522_AUDIO_MODE_REG0F1H, 0x82);
	msleep(70);

	/* Start the audio processing module */
	au8522_writereg(state, AU8522_SYSTEM_MODULE_CONTROL_0_REG0A4H, 0x9d);

	/* Set the audio frequency to 48 KHz */
	au8522_writereg(state, AU8522_AUDIOFREQ_REG606H, 0x03);

	/* Set the I2S parameters (WS, LSB, mode, sample rate */
	au8522_writereg(state, AU8522_I2S_CTRL_2_REG112H, 0xc2);

	/* Enable the I2S output */
	au8522_writereg(state, AU8522_SYSTEM_MODULE_CONTROL_1_REG0A5H, 0x09);
}

/* ----------------------------------------------------------------------- */

static int au8522_s_ctrl(struct v4l2_ctrl *ctrl)
{
	struct au8522_state *state =
		container_of(ctrl->handler, struct au8522_state, hdl);

	switch (ctrl->id) {
	case V4L2_CID_BRIGHTNESS:
		au8522_writereg(state, AU8522_TVDEC_BRIGHTNESS_REG00AH,
				ctrl->val - 128);
		break;
	case V4L2_CID_CONTRAST:
		au8522_writereg(state, AU8522_TVDEC_CONTRAST_REG00BH,
				ctrl->val);
		break;
	case V4L2_CID_SATURATION:
		au8522_writereg(state, AU8522_TVDEC_SATURATION_CB_REG00CH,
				ctrl->val);
		au8522_writereg(state, AU8522_TVDEC_SATURATION_CR_REG00DH,
				ctrl->val);
		break;
	case V4L2_CID_HUE:
		au8522_writereg(state, AU8522_TVDEC_HUE_H_REG00EH,
				ctrl->val >> 8);
		au8522_writereg(state, AU8522_TVDEC_HUE_L_REG00FH,
				ctrl->val & 0xFF);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/* ----------------------------------------------------------------------- */

#ifdef CONFIG_VIDEO_ADV_DEBUG
static int au8522_g_register(struct v4l2_subdev *sd,
			     struct v4l2_dbg_register *reg)
{
	struct au8522_state *state = to_state(sd);

	reg->val = au8522_readreg(state, reg->reg & 0xffff);
	return 0;
}

static int au8522_s_register(struct v4l2_subdev *sd,
			     const struct v4l2_dbg_register *reg)
{
	struct au8522_state *state = to_state(sd);

	au8522_writereg(state, reg->reg, reg->val & 0xff);
	return 0;
}
#endif

static void au8522_video_set(struct au8522_state *state)
{
	u8 input_mode;

	au8522_writereg(state, 0xa4, 1 << 5);

	switch (state->vid_input) {
	case AU8522_COMPOSITE_CH1:
		input_mode = AU8522_INPUT_CONTROL_REG081H_CVBS_CH1;
		au8522_setup_cvbs_mode(state, input_mode);
		break;
	case AU8522_COMPOSITE_CH2:
		input_mode = AU8522_INPUT_CONTROL_REG081H_CVBS_CH2;
		au8522_setup_cvbs_mode(state, input_mode);
		break;
	case AU8522_COMPOSITE_CH3:
		input_mode = AU8522_INPUT_CONTROL_REG081H_CVBS_CH3;
		au8522_setup_cvbs_mode(state, input_mode);
		break;
	case AU8522_COMPOSITE_CH4:
		input_mode = AU8522_INPUT_CONTROL_REG081H_CVBS_CH4;
		au8522_setup_cvbs_mode(state, input_mode);
		break;
	case AU8522_SVIDEO_CH13:
		input_mode = AU8522_INPUT_CONTROL_REG081H_SVIDEO_CH13;
		au8522_setup_svideo_mode(state, input_mode);
		break;
	case AU8522_SVIDEO_CH24:
		input_mode = AU8522_INPUT_CONTROL_REG081H_SVIDEO_CH24;
		au8522_setup_svideo_mode(state, input_mode);
		break;
	default:
	case AU8522_COMPOSITE_CH4_SIF:
		input_mode = AU8522_INPUT_CONTROL_REG081H_CVBS_CH4_SIF;
		au8522_setup_cvbs_tuner_mode(state, input_mode);
		break;
	}
}

static int au8522_s_stream(struct v4l2_subdev *sd, int enable)
{
	struct au8522_state *state = to_state(sd);

	if (enable) {
		/*
		 * Clear out any state associated with the digital side of the
		 * chip, so that when it gets powered back up it won't think
		 * that it is already tuned
		 */
		state->current_frequency = 0;

		au8522_writereg(state, AU8522_SYSTEM_MODULE_CONTROL_0_REG0A4H,
				0x01);
		msleep(10);

		au8522_video_set(state);
		set_audio_input(state);

		state->operational_mode = AU8522_ANALOG_MODE;
	} else {
		/* This does not completely power down the device
		   (it only reduces it from around 140ma to 80ma) */
		au8522_writereg(state, AU8522_SYSTEM_MODULE_CONTROL_0_REG0A4H,
				1 << 5);
		state->operational_mode = AU8522_SUSPEND_MODE;
	}
	return 0;
}

static int au8522_s_video_routing(struct v4l2_subdev *sd,
					u32 input, u32 output, u32 config)
{
	struct au8522_state *state = to_state(sd);

	switch (input) {
	case AU8522_COMPOSITE_CH1:
	case AU8522_SVIDEO_CH13:
	case AU8522_COMPOSITE_CH4_SIF:
		state->vid_input = input;
		break;
	default:
		printk(KERN_ERR "au8522 mode not currently supported\n");
		return -EINVAL;
	}

	if (state->operational_mode == AU8522_ANALOG_MODE)
		au8522_video_set(state);

	return 0;
}

static int au8522_s_std(struct v4l2_subdev *sd, v4l2_std_id std)
{
	struct au8522_state *state = to_state(sd);

	if ((std & (V4L2_STD_PAL_M | V4L2_STD_NTSC_M)) == 0)
		return -EINVAL;

	state->std = std;

	if (state->operational_mode == AU8522_ANALOG_MODE)
		au8522_video_set(state);

	return 0;
}

static int au8522_s_audio_routing(struct v4l2_subdev *sd,
					u32 input, u32 output, u32 config)
{
	struct au8522_state *state = to_state(sd);

	state->aud_input = input;

	if (state->operational_mode == AU8522_ANALOG_MODE)
		set_audio_input(state);

	return 0;
}

static int au8522_g_tuner(struct v4l2_subdev *sd, struct v4l2_tuner *vt)
{
	int val = 0;
	struct au8522_state *state = to_state(sd);
	u8 lock_status;
	u8 pll_status;

	/* Interrogate the decoder to see if we are getting a real signal */
	lock_status = au8522_readreg(state, 0x00);
	pll_status = au8522_readreg(state, 0x7e);
	if ((lock_status == 0xa2) && (pll_status & 0x10))
		vt->signal = 0xffff;
	else
		vt->signal = 0x00;

	vt->capability |=
		V4L2_TUNER_CAP_STEREO | V4L2_TUNER_CAP_LANG1 |
		V4L2_TUNER_CAP_LANG2 | V4L2_TUNER_CAP_SAP;

	val = V4L2_TUNER_SUB_MONO;
	vt->rxsubchans = val;
	vt->audmode = V4L2_TUNER_MODE_STEREO;
	return 0;
}

/* ----------------------------------------------------------------------- */

static const struct v4l2_subdev_core_ops au8522_core_ops = {
	.log_status = v4l2_ctrl_subdev_log_status,
#ifdef CONFIG_VIDEO_ADV_DEBUG
	.g_register = au8522_g_register,
	.s_register = au8522_s_register,
#endif
};

static const struct v4l2_subdev_tuner_ops au8522_tuner_ops = {
	.g_tuner = au8522_g_tuner,
};

static const struct v4l2_subdev_audio_ops au8522_audio_ops = {
	.s_routing = au8522_s_audio_routing,
};

static const struct v4l2_subdev_video_ops au8522_video_ops = {
	.s_routing = au8522_s_video_routing,
	.s_stream = au8522_s_stream,
	.s_std = au8522_s_std,
};

static const struct v4l2_subdev_ops au8522_ops = {
	.core = &au8522_core_ops,
	.tuner = &au8522_tuner_ops,
	.audio = &au8522_audio_ops,
	.video = &au8522_video_ops,
};

static const struct v4l2_ctrl_ops au8522_ctrl_ops = {
	.s_ctrl = au8522_s_ctrl,
};

/* ----------------------------------------------------------------------- */

static int au8522_probe(struct i2c_client *client,
			const struct i2c_device_id *did)
{
	struct au8522_state *state;
	struct v4l2_ctrl_handler *hdl;
	struct v4l2_subdev *sd;
	int instance;
#ifdef CONFIG_MEDIA_CONTROLLER
	int ret;
#endif

	/* Check if the adapter supports the needed features */
	if (!i2c_check_functionality(client->adapter,
				     I2C_FUNC_SMBUS_BYTE_DATA)) {
		return -EIO;
	}

	/* allocate memory for the internal state */
	instance = au8522_get_state(&state, client->adapter, client->addr);
	switch (instance) {
	case 0:
		printk(KERN_ERR "au8522_decoder allocation failed\n");
		return -EIO;
	case 1:
		/* new demod instance */
		printk(KERN_INFO "au8522_decoder creating new instance...\n");
		break;
	default:
		/* existing demod instance */
		printk(KERN_INFO "au8522_decoder attach existing instance.\n");
		break;
	}

	state->config.demod_address = 0x8e >> 1;
	state->i2c = client->adapter;

	sd = &state->sd;
	v4l2_i2c_subdev_init(sd, client, &au8522_ops);
#if defined(CONFIG_MEDIA_CONTROLLER)

	state->pads[AU8522_PAD_IF_INPUT].flags = MEDIA_PAD_FL_SINK;
	state->pads[AU8522_PAD_IF_INPUT].sig_type = PAD_SIGNAL_ANALOG;
	state->pads[AU8522_PAD_VID_OUT].flags = MEDIA_PAD_FL_SOURCE;
	state->pads[AU8522_PAD_VID_OUT].sig_type = PAD_SIGNAL_DV;
	state->pads[AU8522_PAD_AUDIO_OUT].flags = MEDIA_PAD_FL_SOURCE;
	state->pads[AU8522_PAD_AUDIO_OUT].sig_type = PAD_SIGNAL_AUDIO;
	sd->entity.function = MEDIA_ENT_F_ATV_DECODER;

	ret = media_entity_pads_init(&sd->entity, ARRAY_SIZE(state->pads),
				state->pads);
	if (ret < 0) {
		v4l_info(client, "failed to initialize media entity!\n");
		return ret;
	}
#endif

	hdl = &state->hdl;
	v4l2_ctrl_handler_init(hdl, 4);
	v4l2_ctrl_new_std(hdl, &au8522_ctrl_ops,
			V4L2_CID_BRIGHTNESS, 0, 255, 1, 109);
	v4l2_ctrl_new_std(hdl, &au8522_ctrl_ops,
			V4L2_CID_CONTRAST, 0, 255, 1,
			AU8522_TVDEC_CONTRAST_REG00BH_CVBS);
	v4l2_ctrl_new_std(hdl, &au8522_ctrl_ops,
			V4L2_CID_SATURATION, 0, 255, 1, 128);
	v4l2_ctrl_new_std(hdl, &au8522_ctrl_ops,
			V4L2_CID_HUE, -32768, 32767, 1, 0);
	sd->ctrl_handler = hdl;
	if (hdl->error) {
		int err = hdl->error;

		v4l2_ctrl_handler_free(hdl);
		au8522_release_state(state);
		return err;
	}

	state->c = client;
	state->std = V4L2_STD_NTSC_M;
	state->vid_input = AU8522_COMPOSITE_CH1;
	state->aud_input = AU8522_AUDIO_NONE;
	state->id = 8522;
	state->rev = 0;

	/* Jam open the i2c gate to the tuner */
	au8522_writereg(state, 0x106, 1);

	return 0;
}

static int au8522_remove(struct i2c_client *client)
{
	struct v4l2_subdev *sd = i2c_get_clientdata(client);
	v4l2_device_unregister_subdev(sd);
	v4l2_ctrl_handler_free(sd->ctrl_handler);
	au8522_release_state(to_state(sd));
	return 0;
}

static const struct i2c_device_id au8522_id[] = {
	{"au8522", 0},
	{}
};

MODULE_DEVICE_TABLE(i2c, au8522_id);

static struct i2c_driver au8522_driver = {
	.driver = {
		.name	= "au8522",
	},
	.probe		= au8522_probe,
	.remove		= au8522_remove,
	.id_table	= au8522_id,
};

module_i2c_driver(au8522_driver);
