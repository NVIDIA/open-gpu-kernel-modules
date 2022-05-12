/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *
 * Copyright (c) 2003 by Karsten Wiese <annabellesgarden@yahoo.de>
 */

enum E_In84{
	eFader0 = 0,
	eFader1,
	eFader2,
	eFader3,
	eFader4,
	eFader5,
	eFader6,
	eFader7,
	eFaderM,
	eTransport,
	eModifier = 10,
	eFilterSelect,
	eSelect,
	eMute,

	eSwitch   = 15,
	eWheelGain,
	eWheelFreq,
	eWheelQ,
	eWheelPan,
	eWheel    = 20
};

#define T_RECORD   1
#define T_PLAY     2
#define T_STOP     4
#define T_F_FWD    8
#define T_REW   0x10
#define T_SOLO  0x20
#define T_REC   0x40
#define T_NULL  0x80


struct us428_ctls {
	unsigned char   Fader[9];
	unsigned char 	Transport;
	unsigned char 	Modifier;
	unsigned char 	FilterSelect;
	unsigned char 	Select;
	unsigned char   Mute;
	unsigned char   UNKNOWN;
	unsigned char   Switch;	     
	unsigned char   Wheel[5];
};

struct us428_setByte {
	unsigned char Offset,
		Value;
};

enum {
	eLT_Volume = 0,
	eLT_Light
};

struct usX2Y_volume {
	unsigned char Channel,
		LH,
		LL,
		RH,
		RL;
};

struct us428_lights {
	struct us428_setByte Light[7];
};

struct us428_p4out {
	char type;
	union {
		struct usX2Y_volume vol;
		struct us428_lights lights;
	} val;
};

#define N_us428_ctl_BUFS 16
#define N_us428_p4out_BUFS 16
struct us428ctls_sharedmem{
	struct us428_ctls	CtlSnapShot[N_us428_ctl_BUFS];
	int			CtlSnapShotDiffersAt[N_us428_ctl_BUFS];
	int			CtlSnapShotLast, CtlSnapShotRed;
	struct us428_p4out	p4out[N_us428_p4out_BUFS];
	int			p4outLast, p4outSent;
};
