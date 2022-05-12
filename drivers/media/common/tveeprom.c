// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * tveeprom - eeprom decoder for tvcard configuration eeproms
 *
 * Data and decoding routines shamelessly borrowed from bttv-cards.c
 * eeprom access routine shamelessly borrowed from bttv-if.c
 * which are:

    Copyright (C) 1996,97,98 Ralph  Metzler (rjkm@thp.uni-koeln.de)
			   & Marcus Metzler (mocm@thp.uni-koeln.de)
    (c) 1999-2001 Gerd Knorr <kraxel@goldbach.in-berlin.de>

 * Adjustments to fit a more general model and all bugs:

	Copyright (C) 2003 John Klar <linpvr at projectplasma.com>

 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/videodev2.h>
#include <linux/i2c.h>

#include <media/tuner.h>
#include <media/tveeprom.h>
#include <media/v4l2-common.h>

MODULE_DESCRIPTION("i2c Hauppauge eeprom decoder driver");
MODULE_AUTHOR("John Klar");
MODULE_LICENSE("GPL");

#define STRM(array, i) \
	(i < sizeof(array) / sizeof(char *) ? array[i] : "unknown")


/*
 * The Hauppauge eeprom uses an 8bit field to determine which
 * tuner formats the tuner supports.
 */
static const struct {
	int	id;
	const char * const name;
} hauppauge_tuner_fmt[] = {
	{ V4L2_STD_UNKNOWN,                   " UNKNOWN" },
	{ V4L2_STD_UNKNOWN,                   " FM" },
	{ V4L2_STD_B|V4L2_STD_GH,             " PAL(B/G)" },
	{ V4L2_STD_MN,                        " NTSC(M)" },
	{ V4L2_STD_PAL_I,                     " PAL(I)" },
	{ V4L2_STD_SECAM_L|V4L2_STD_SECAM_LC, " SECAM(L/L')" },
	{ V4L2_STD_DK,                        " PAL(D/D1/K)" },
	{ V4L2_STD_ATSC,                      " ATSC/DVB Digital" },
};

/* This is the full list of possible tuners. Many thanks to Hauppauge for
   supplying this information. Note that many tuners where only used for
   testing and never made it to the outside world. So you will only see
   a subset in actual produced cards. */
static const struct {
	int  id;
	const char * const name;
} hauppauge_tuner[] = {
	/* 0-9 */
	{ TUNER_ABSENT,			"None" },
	{ TUNER_ABSENT,			"External" },
	{ TUNER_ABSENT,			"Unspecified" },
	{ TUNER_PHILIPS_PAL,		"Philips FI1216" },
	{ TUNER_PHILIPS_SECAM,		"Philips FI1216MF" },
	{ TUNER_PHILIPS_NTSC,		"Philips FI1236" },
	{ TUNER_PHILIPS_PAL_I,		"Philips FI1246" },
	{ TUNER_PHILIPS_PAL_DK,		"Philips FI1256" },
	{ TUNER_PHILIPS_PAL,		"Philips FI1216 MK2" },
	{ TUNER_PHILIPS_SECAM,		"Philips FI1216MF MK2" },
	/* 10-19 */
	{ TUNER_PHILIPS_NTSC,		"Philips FI1236 MK2" },
	{ TUNER_PHILIPS_PAL_I,		"Philips FI1246 MK2" },
	{ TUNER_PHILIPS_PAL_DK,		"Philips FI1256 MK2" },
	{ TUNER_TEMIC_NTSC,		"Temic 4032FY5" },
	{ TUNER_TEMIC_PAL,		"Temic 4002FH5" },
	{ TUNER_TEMIC_PAL_I,		"Temic 4062FY5" },
	{ TUNER_PHILIPS_PAL,		"Philips FR1216 MK2" },
	{ TUNER_PHILIPS_SECAM,		"Philips FR1216MF MK2" },
	{ TUNER_PHILIPS_NTSC,		"Philips FR1236 MK2" },
	{ TUNER_PHILIPS_PAL_I,		"Philips FR1246 MK2" },
	/* 20-29 */
	{ TUNER_PHILIPS_PAL_DK,		"Philips FR1256 MK2" },
	{ TUNER_PHILIPS_PAL,		"Philips FM1216" },
	{ TUNER_PHILIPS_SECAM,		"Philips FM1216MF" },
	{ TUNER_PHILIPS_NTSC,		"Philips FM1236" },
	{ TUNER_PHILIPS_PAL_I,		"Philips FM1246" },
	{ TUNER_PHILIPS_PAL_DK,		"Philips FM1256" },
	{ TUNER_TEMIC_4036FY5_NTSC,	"Temic 4036FY5" },
	{ TUNER_ABSENT,			"Samsung TCPN9082D" },
	{ TUNER_ABSENT,			"Samsung TCPM9092P" },
	{ TUNER_TEMIC_4006FH5_PAL,	"Temic 4006FH5" },
	/* 30-39 */
	{ TUNER_ABSENT,			"Samsung TCPN9085D" },
	{ TUNER_ABSENT,			"Samsung TCPB9085P" },
	{ TUNER_ABSENT,			"Samsung TCPL9091P" },
	{ TUNER_TEMIC_4039FR5_NTSC,	"Temic 4039FR5" },
	{ TUNER_PHILIPS_FQ1216ME,	"Philips FQ1216 ME" },
	{ TUNER_TEMIC_4066FY5_PAL_I,	"Temic 4066FY5" },
	{ TUNER_PHILIPS_NTSC,		"Philips TD1536" },
	{ TUNER_PHILIPS_NTSC,		"Philips TD1536D" },
	{ TUNER_PHILIPS_NTSC,		"Philips FMR1236" }, /* mono radio */
	{ TUNER_ABSENT,			"Philips FI1256MP" },
	/* 40-49 */
	{ TUNER_ABSENT,			"Samsung TCPQ9091P" },
	{ TUNER_TEMIC_4006FN5_MULTI_PAL,"Temic 4006FN5" },
	{ TUNER_TEMIC_4009FR5_PAL,	"Temic 4009FR5" },
	{ TUNER_TEMIC_4046FM5,		"Temic 4046FM5" },
	{ TUNER_TEMIC_4009FN5_MULTI_PAL_FM, "Temic 4009FN5" },
	{ TUNER_ABSENT,			"Philips TD1536D FH 44"},
	{ TUNER_LG_NTSC_FM,		"LG TP18NSR01F"},
	{ TUNER_LG_PAL_FM,		"LG TP18PSB01D"},
	{ TUNER_LG_PAL,		"LG TP18PSB11D"},
	{ TUNER_LG_PAL_I_FM,		"LG TAPC-I001D"},
	/* 50-59 */
	{ TUNER_LG_PAL_I,		"LG TAPC-I701D"},
	{ TUNER_ABSENT,			"Temic 4042FI5"},
	{ TUNER_MICROTUNE_4049FM5,	"Microtune 4049 FM5"},
	{ TUNER_ABSENT,			"LG TPI8NSR11F"},
	{ TUNER_ABSENT,			"Microtune 4049 FM5 Alt I2C"},
	{ TUNER_PHILIPS_FM1216ME_MK3,	"Philips FQ1216ME MK3"},
	{ TUNER_ABSENT,			"Philips FI1236 MK3"},
	{ TUNER_PHILIPS_FM1216ME_MK3,	"Philips FM1216 ME MK3"},
	{ TUNER_PHILIPS_FM1236_MK3,	"Philips FM1236 MK3"},
	{ TUNER_ABSENT,			"Philips FM1216MP MK3"},
	/* 60-69 */
	{ TUNER_PHILIPS_FM1216ME_MK3,	"LG S001D MK3"},
	{ TUNER_ABSENT,			"LG M001D MK3"},
	{ TUNER_PHILIPS_FM1216ME_MK3,	"LG S701D MK3"},
	{ TUNER_ABSENT,			"LG M701D MK3"},
	{ TUNER_ABSENT,			"Temic 4146FM5"},
	{ TUNER_ABSENT,			"Temic 4136FY5"},
	{ TUNER_ABSENT,			"Temic 4106FH5"},
	{ TUNER_ABSENT,			"Philips FQ1216LMP MK3"},
	{ TUNER_LG_NTSC_TAPE,		"LG TAPE H001F MK3"},
	{ TUNER_LG_NTSC_TAPE,		"LG TAPE H701F MK3"},
	/* 70-79 */
	{ TUNER_ABSENT,			"LG TALN H200T"},
	{ TUNER_ABSENT,			"LG TALN H250T"},
	{ TUNER_ABSENT,			"LG TALN M200T"},
	{ TUNER_ABSENT,			"LG TALN Z200T"},
	{ TUNER_ABSENT,			"LG TALN S200T"},
	{ TUNER_ABSENT,			"Thompson DTT7595"},
	{ TUNER_ABSENT,			"Thompson DTT7592"},
	{ TUNER_ABSENT,			"Silicon TDA8275C1 8290"},
	{ TUNER_ABSENT,			"Silicon TDA8275C1 8290 FM"},
	{ TUNER_ABSENT,			"Thompson DTT757"},
	/* 80-89 */
	{ TUNER_PHILIPS_FQ1216LME_MK3,	"Philips FQ1216LME MK3"},
	{ TUNER_LG_PAL_NEW_TAPC,	"LG TAPC G701D"},
	{ TUNER_LG_NTSC_NEW_TAPC,	"LG TAPC H791F"},
	{ TUNER_LG_PAL_NEW_TAPC,	"TCL 2002MB 3"},
	{ TUNER_LG_PAL_NEW_TAPC,	"TCL 2002MI 3"},
	{ TUNER_TCL_2002N,		"TCL 2002N 6A"},
	{ TUNER_PHILIPS_FM1236_MK3,	"Philips FQ1236 MK3"},
	{ TUNER_SAMSUNG_TCPN_2121P30A,	"Samsung TCPN 2121P30A"},
	{ TUNER_ABSENT,			"Samsung TCPE 4121P30A"},
	{ TUNER_PHILIPS_FM1216ME_MK3,	"TCL MFPE05 2"},
	/* 90-99 */
	{ TUNER_ABSENT,			"LG TALN H202T"},
	{ TUNER_PHILIPS_FQ1216AME_MK4,	"Philips FQ1216AME MK4"},
	{ TUNER_PHILIPS_FQ1236A_MK4,	"Philips FQ1236A MK4"},
	{ TUNER_ABSENT,			"Philips FQ1286A MK4"},
	{ TUNER_ABSENT,			"Philips FQ1216ME MK5"},
	{ TUNER_ABSENT,			"Philips FQ1236 MK5"},
	{ TUNER_SAMSUNG_TCPG_6121P30A,	"Samsung TCPG 6121P30A"},
	{ TUNER_TCL_2002MB,		"TCL 2002MB_3H"},
	{ TUNER_ABSENT,			"TCL 2002MI_3H"},
	{ TUNER_TCL_2002N,		"TCL 2002N 5H"},
	/* 100-109 */
	{ TUNER_PHILIPS_FMD1216ME_MK3,	"Philips FMD1216ME"},
	{ TUNER_TEA5767,		"Philips TEA5768HL FM Radio"},
	{ TUNER_ABSENT,			"Panasonic ENV57H12D5"},
	{ TUNER_PHILIPS_FM1236_MK3,	"TCL MFNM05-4"},
	{ TUNER_PHILIPS_FM1236_MK3,	"TCL MNM05-4"},
	{ TUNER_PHILIPS_FM1216ME_MK3,	"TCL MPE05-2"},
	{ TUNER_ABSENT,			"TCL MQNM05-4"},
	{ TUNER_ABSENT,			"LG TAPC-W701D"},
	{ TUNER_ABSENT,			"TCL 9886P-WM"},
	{ TUNER_ABSENT,			"TCL 1676NM-WM"},
	/* 110-119 */
	{ TUNER_ABSENT,			"Thompson DTT75105"},
	{ TUNER_ABSENT,			"Conexant_CX24109"},
	{ TUNER_TCL_2002N,		"TCL M2523_5N_E"},
	{ TUNER_TCL_2002MB,		"TCL M2523_3DB_E"},
	{ TUNER_ABSENT,			"Philips 8275A"},
	{ TUNER_ABSENT,			"Microtune MT2060"},
	{ TUNER_PHILIPS_FM1236_MK3,	"Philips FM1236 MK5"},
	{ TUNER_PHILIPS_FM1216ME_MK3,	"Philips FM1216ME MK5"},
	{ TUNER_ABSENT,			"TCL M2523_3DI_E"},
	{ TUNER_ABSENT,			"Samsung THPD5222FG30A"},
	/* 120-129 */
	{ TUNER_XC2028,			"Xceive XC3028"},
	{ TUNER_PHILIPS_FQ1216LME_MK3,	"Philips FQ1216LME MK5"},
	{ TUNER_ABSENT,			"Philips FQD1216LME"},
	{ TUNER_ABSENT,			"Conexant CX24118A"},
	{ TUNER_ABSENT,			"TCL DMF11WIP"},
	{ TUNER_ABSENT,			"TCL MFNM05_4H_E"},
	{ TUNER_ABSENT,			"TCL MNM05_4H_E"},
	{ TUNER_ABSENT,			"TCL MPE05_2H_E"},
	{ TUNER_ABSENT,			"TCL MQNM05_4_U"},
	{ TUNER_ABSENT,			"TCL M2523_5NH_E"},
	/* 130-139 */
	{ TUNER_ABSENT,			"TCL M2523_3DBH_E"},
	{ TUNER_ABSENT,			"TCL M2523_3DIH_E"},
	{ TUNER_ABSENT,			"TCL MFPE05_2_U"},
	{ TUNER_PHILIPS_FMD1216MEX_MK3,	"Philips FMD1216MEX"},
	{ TUNER_ABSENT,			"Philips FRH2036B"},
	{ TUNER_ABSENT,			"Panasonic ENGF75_01GF"},
	{ TUNER_ABSENT,			"MaxLinear MXL5005"},
	{ TUNER_ABSENT,			"MaxLinear MXL5003"},
	{ TUNER_ABSENT,			"Xceive XC2028"},
	{ TUNER_ABSENT,			"Microtune MT2131"},
	/* 140-149 */
	{ TUNER_ABSENT,			"Philips 8275A_8295"},
	{ TUNER_ABSENT,			"TCL MF02GIP_5N_E"},
	{ TUNER_ABSENT,			"TCL MF02GIP_3DB_E"},
	{ TUNER_ABSENT,			"TCL MF02GIP_3DI_E"},
	{ TUNER_ABSENT,			"Microtune MT2266"},
	{ TUNER_ABSENT,			"TCL MF10WPP_4N_E"},
	{ TUNER_ABSENT,			"LG TAPQ_H702F"},
	{ TUNER_ABSENT,			"TCL M09WPP_4N_E"},
	{ TUNER_ABSENT,			"MaxLinear MXL5005_v2"},
	{ TUNER_PHILIPS_TDA8290,	"Philips 18271_8295"},
	/* 150-159 */
	{ TUNER_XC5000,                 "Xceive XC5000"},
	{ TUNER_ABSENT,                 "Xceive XC3028L"},
	{ TUNER_ABSENT,                 "NXP 18271C2_716x"},
	{ TUNER_ABSENT,                 "Xceive XC4000"},
	{ TUNER_ABSENT,                 "Dibcom 7070"},
	{ TUNER_PHILIPS_TDA8290,        "NXP 18271C2"},
	{ TUNER_ABSENT,                 "Siano SMS1010"},
	{ TUNER_ABSENT,                 "Siano SMS1150"},
	{ TUNER_ABSENT,                 "MaxLinear 5007"},
	{ TUNER_ABSENT,                 "TCL M09WPP_2P_E"},
	/* 160-169 */
	{ TUNER_ABSENT,                 "Siano SMS1180"},
	{ TUNER_ABSENT,                 "Maxim_MAX2165"},
	{ TUNER_ABSENT,                 "Siano SMS1140"},
	{ TUNER_ABSENT,                 "Siano SMS1150 B1"},
	{ TUNER_ABSENT,                 "MaxLinear 111"},
	{ TUNER_ABSENT,                 "Dibcom 7770"},
	{ TUNER_ABSENT,                 "Siano SMS1180VNS"},
	{ TUNER_ABSENT,                 "Siano SMS1184"},
	{ TUNER_PHILIPS_FQ1236_MK5,	"TCL M30WTP-4N-E"},
	{ TUNER_ABSENT,                 "TCL_M11WPP_2PN_E"},
	/* 170-179 */
	{ TUNER_ABSENT,                 "MaxLinear 301"},
	{ TUNER_ABSENT,                 "Mirics MSi001"},
	{ TUNER_ABSENT,                 "MaxLinear MxL241SF"},
	{ TUNER_XC5000C,                "Xceive XC5000C"},
	{ TUNER_ABSENT,                 "Montage M68TS2020"},
	{ TUNER_ABSENT,                 "Siano SMS1530"},
	{ TUNER_ABSENT,                 "Dibcom 7090"},
	{ TUNER_ABSENT,                 "Xceive XC5200C"},
	{ TUNER_ABSENT,                 "NXP 18273"},
	{ TUNER_ABSENT,                 "Montage M88TS2022"},
	/* 180-188 */
	{ TUNER_ABSENT,                 "NXP 18272M"},
	{ TUNER_ABSENT,                 "NXP 18272S"},

	{ TUNER_ABSENT,                 "Mirics MSi003"},
	{ TUNER_ABSENT,                 "MaxLinear MxL256"},
	{ TUNER_ABSENT,                 "SiLabs Si2158"},
	{ TUNER_ABSENT,                 "SiLabs Si2178"},
	{ TUNER_ABSENT,                 "SiLabs Si2157"},
	{ TUNER_ABSENT,                 "SiLabs Si2177"},
	{ TUNER_ABSENT,                 "ITE IT9137FN"},
};

/* Use TVEEPROM_AUDPROC_INTERNAL for those audio 'chips' that are
 * internal to a video chip, i.e. not a separate audio chip. */
static const struct {
	u32   id;
	const char * const name;
} audio_ic[] = {
	/* 0-4 */
	{ TVEEPROM_AUDPROC_NONE,  "None"      },
	{ TVEEPROM_AUDPROC_OTHER, "TEA6300"   },
	{ TVEEPROM_AUDPROC_OTHER, "TEA6320"   },
	{ TVEEPROM_AUDPROC_OTHER, "TDA9850"   },
	{ TVEEPROM_AUDPROC_MSP,   "MSP3400C"  },
	/* 5-9 */
	{ TVEEPROM_AUDPROC_MSP,   "MSP3410D"  },
	{ TVEEPROM_AUDPROC_MSP,   "MSP3415"   },
	{ TVEEPROM_AUDPROC_MSP,   "MSP3430"   },
	{ TVEEPROM_AUDPROC_MSP,   "MSP3438"   },
	{ TVEEPROM_AUDPROC_OTHER, "CS5331"    },
	/* 10-14 */
	{ TVEEPROM_AUDPROC_MSP,   "MSP3435"   },
	{ TVEEPROM_AUDPROC_MSP,   "MSP3440"   },
	{ TVEEPROM_AUDPROC_MSP,   "MSP3445"   },
	{ TVEEPROM_AUDPROC_MSP,   "MSP3411"   },
	{ TVEEPROM_AUDPROC_MSP,   "MSP3416"   },
	/* 15-19 */
	{ TVEEPROM_AUDPROC_MSP,   "MSP3425"   },
	{ TVEEPROM_AUDPROC_MSP,   "MSP3451"   },
	{ TVEEPROM_AUDPROC_MSP,   "MSP3418"   },
	{ TVEEPROM_AUDPROC_OTHER, "Type 0x12" },
	{ TVEEPROM_AUDPROC_OTHER, "OKI7716"   },
	/* 20-24 */
	{ TVEEPROM_AUDPROC_MSP,   "MSP4410"   },
	{ TVEEPROM_AUDPROC_MSP,   "MSP4420"   },
	{ TVEEPROM_AUDPROC_MSP,   "MSP4440"   },
	{ TVEEPROM_AUDPROC_MSP,   "MSP4450"   },
	{ TVEEPROM_AUDPROC_MSP,   "MSP4408"   },
	/* 25-29 */
	{ TVEEPROM_AUDPROC_MSP,   "MSP4418"   },
	{ TVEEPROM_AUDPROC_MSP,   "MSP4428"   },
	{ TVEEPROM_AUDPROC_MSP,   "MSP4448"   },
	{ TVEEPROM_AUDPROC_MSP,   "MSP4458"   },
	{ TVEEPROM_AUDPROC_MSP,   "Type 0x1d" },
	/* 30-34 */
	{ TVEEPROM_AUDPROC_INTERNAL, "CX880"     },
	{ TVEEPROM_AUDPROC_INTERNAL, "CX881"     },
	{ TVEEPROM_AUDPROC_INTERNAL, "CX883"     },
	{ TVEEPROM_AUDPROC_INTERNAL, "CX882"     },
	{ TVEEPROM_AUDPROC_INTERNAL, "CX25840"   },
	/* 35-39 */
	{ TVEEPROM_AUDPROC_INTERNAL, "CX25841"   },
	{ TVEEPROM_AUDPROC_INTERNAL, "CX25842"   },
	{ TVEEPROM_AUDPROC_INTERNAL, "CX25843"   },
	{ TVEEPROM_AUDPROC_INTERNAL, "CX23418"   },
	{ TVEEPROM_AUDPROC_INTERNAL, "CX23885"   },
	/* 40-44 */
	{ TVEEPROM_AUDPROC_INTERNAL, "CX23888"   },
	{ TVEEPROM_AUDPROC_INTERNAL, "SAA7131"   },
	{ TVEEPROM_AUDPROC_INTERNAL, "CX23887"   },
	{ TVEEPROM_AUDPROC_INTERNAL, "SAA7164"   },
	{ TVEEPROM_AUDPROC_INTERNAL, "AU8522"    },
	/* 45-49 */
	{ TVEEPROM_AUDPROC_INTERNAL, "AVF4910B"  },
	{ TVEEPROM_AUDPROC_INTERNAL, "SAA7231"   },
	{ TVEEPROM_AUDPROC_INTERNAL, "CX23102"   },
	{ TVEEPROM_AUDPROC_INTERNAL, "SAA7163"   },
	{ TVEEPROM_AUDPROC_OTHER,    "AK4113"    },
	/* 50-52 */
	{ TVEEPROM_AUDPROC_OTHER,    "CS5340"    },
	{ TVEEPROM_AUDPROC_OTHER,    "CS8416"    },
	{ TVEEPROM_AUDPROC_OTHER,    "CX20810"   },
};

/* This list is supplied by Hauppauge. Thanks! */
static const char *decoderIC[] = {
	/* 0-4 */
	"None", "BT815", "BT817", "BT819", "BT815A",
	/* 5-9 */
	"BT817A", "BT819A", "BT827", "BT829", "BT848",
	/* 10-14 */
	"BT848A", "BT849A", "BT829A", "BT827A", "BT878",
	/* 15-19 */
	"BT879", "BT880", "VPX3226E", "SAA7114", "SAA7115",
	/* 20-24 */
	"CX880", "CX881", "CX883", "SAA7111", "SAA7113",
	/* 25-29 */
	"CX882", "TVP5150A", "CX25840", "CX25841", "CX25842",
	/* 30-34 */
	"CX25843", "CX23418", "NEC61153", "CX23885", "CX23888",
	/* 35-39 */
	"SAA7131", "CX25837", "CX23887", "CX23885A", "CX23887A",
	/* 40-44 */
	"SAA7164", "CX23885B", "AU8522", "ADV7401", "AVF4910B",
	/* 45-49 */
	"SAA7231", "CX23102", "SAA7163", "ADV7441A", "ADV7181C",
	/* 50-53 */
	"CX25836", "TDA9955", "TDA19977", "ADV7842"
};

static int hasRadioTuner(int tunerType)
{
	switch (tunerType) {
	case 18: /* PNPEnv_TUNER_FR1236_MK2 */
	case 23: /* PNPEnv_TUNER_FM1236 */
	case 38: /* PNPEnv_TUNER_FMR1236 */
	case 16: /* PNPEnv_TUNER_FR1216_MK2 */
	case 19: /* PNPEnv_TUNER_FR1246_MK2 */
	case 21: /* PNPEnv_TUNER_FM1216 */
	case 24: /* PNPEnv_TUNER_FM1246 */
	case 17: /* PNPEnv_TUNER_FR1216MF_MK2 */
	case 22: /* PNPEnv_TUNER_FM1216MF */
	case 20: /* PNPEnv_TUNER_FR1256_MK2 */
	case 25: /* PNPEnv_TUNER_FM1256 */
	case 33: /* PNPEnv_TUNER_4039FR5 */
	case 42: /* PNPEnv_TUNER_4009FR5 */
	case 52: /* PNPEnv_TUNER_4049FM5 */
	case 54: /* PNPEnv_TUNER_4049FM5_AltI2C */
	case 44: /* PNPEnv_TUNER_4009FN5 */
	case 31: /* PNPEnv_TUNER_TCPB9085P */
	case 30: /* PNPEnv_TUNER_TCPN9085D */
	case 46: /* PNPEnv_TUNER_TP18NSR01F */
	case 47: /* PNPEnv_TUNER_TP18PSB01D */
	case 49: /* PNPEnv_TUNER_TAPC_I001D */
	case 60: /* PNPEnv_TUNER_TAPE_S001D_MK3 */
	case 57: /* PNPEnv_TUNER_FM1216ME_MK3 */
	case 59: /* PNPEnv_TUNER_FM1216MP_MK3 */
	case 58: /* PNPEnv_TUNER_FM1236_MK3 */
	case 68: /* PNPEnv_TUNER_TAPE_H001F_MK3 */
	case 61: /* PNPEnv_TUNER_TAPE_M001D_MK3 */
	case 78: /* PNPEnv_TUNER_TDA8275C1_8290_FM */
	case 89: /* PNPEnv_TUNER_TCL_MFPE05_2 */
	case 92: /* PNPEnv_TUNER_PHILIPS_FQ1236A_MK4 */
	case 105:
		return 1;
	}
	return 0;
}

void tveeprom_hauppauge_analog(struct tveeprom *tvee,
			       unsigned char *eeprom_data)
{
	/* ----------------------------------------------
	** The hauppauge eeprom format is tagged
	**
	** if packet[0] == 0x84, then packet[0..1] == length
	** else length = packet[0] & 3f;
	** if packet[0] & f8 == f8, then EOD and packet[1] == checksum
	**
	** In our (ivtv) case we're interested in the following:
	** tuner type:   tag [00].05 or [0a].01 (index into hauppauge_tuner)
	** tuner fmts:   tag [00].04 or [0a].00 (bitmask index into
	**		 hauppauge_tuner_fmt)
	** radio:        tag [00].{last} or [0e].00  (bitmask.  bit2=FM)
	** audio proc:   tag [02].01 or [05].00 (mask with 0x7f)
	** decoder proc: tag [09].01)

	** Fun info:
	** model:      tag [00].07-08 or [06].00-01
	** revision:   tag [00].09-0b or [06].04-06
	** serial#:    tag [01].05-07 or [04].04-06

	** # of inputs/outputs ???
	*/

	int i, j, len, done, beenhere, tag, start;

	int tuner1 = 0, t_format1 = 0, audioic = -1;
	const char *t_name1 = NULL;
	const char *t_fmt_name1[8] = { " none", "", "", "", "", "", "", "" };

	int tuner2 = 0, t_format2 = 0;
	const char *t_name2 = NULL;
	const char *t_fmt_name2[8] = { " none", "", "", "", "", "", "", "" };

	memset(tvee, 0, sizeof(*tvee));
	tvee->tuner_type = TUNER_ABSENT;
	tvee->tuner2_type = TUNER_ABSENT;

	done = len = beenhere = 0;

	/* Different eeprom start offsets for em28xx, cx2388x and cx23418 */
	if (eeprom_data[0] == 0x1a &&
	    eeprom_data[1] == 0xeb &&
	    eeprom_data[2] == 0x67 &&
	    eeprom_data[3] == 0x95)
		start = 0xa0; /* Generic em28xx offset */
	else if ((eeprom_data[0] & 0xe1) == 0x01 &&
		 eeprom_data[1] == 0x00 &&
		 eeprom_data[2] == 0x00 &&
		 eeprom_data[8] == 0x84)
		start = 8; /* Generic cx2388x offset */
	else if (eeprom_data[1] == 0x70 &&
		 eeprom_data[2] == 0x00 &&
		 eeprom_data[4] == 0x74 &&
		 eeprom_data[8] == 0x84)
		start = 8; /* Generic cx23418 offset (models 74xxx) */
	else
		start = 0;

	for (i = start; !done && i < 256; i += len) {
		if (eeprom_data[i] == 0x84) {
			len = eeprom_data[i + 1] + (eeprom_data[i + 2] << 8);
			i += 3;
		} else if ((eeprom_data[i] & 0xf0) == 0x70) {
			if (eeprom_data[i] & 0x08) {
				/* verify checksum! */
				done = 1;
				break;
			}
			len = eeprom_data[i] & 0x07;
			++i;
		} else {
			pr_warn("Encountered bad packet header [%02x]. Corrupt or not a Hauppauge eeprom.\n",
				eeprom_data[i]);
			return;
		}

		pr_debug("Tag [%02x] + %d bytes: %*ph\n",
			eeprom_data[i], len - 1, len, &eeprom_data[i]);

		/* process by tag */
		tag = eeprom_data[i];
		switch (tag) {
		case 0x00:
			/* tag: 'Comprehensive' */
			tuner1 = eeprom_data[i+6];
			t_format1 = eeprom_data[i+5];
			tvee->has_radio = eeprom_data[i+len-1];
			/* old style tag, don't know how to detect
			IR presence, mark as unknown. */
			tvee->has_ir = 0;
			tvee->model =
				eeprom_data[i+8] +
				(eeprom_data[i+9] << 8);
			tvee->revision = eeprom_data[i+10] +
				(eeprom_data[i+11] << 8) +
				(eeprom_data[i+12] << 16);
			break;

		case 0x01:
			/* tag: 'SerialID' */
			tvee->serial_number =
				eeprom_data[i+6] +
				(eeprom_data[i+7] << 8) +
				(eeprom_data[i+8] << 16);
			break;

		case 0x02:
			/* tag 'AudioInfo'
			Note mask with 0x7F, high bit used on some older models
			to indicate 4052 mux was removed in favor of using MSP
			inputs directly. */
			audioic = eeprom_data[i+2] & 0x7f;
			if (audioic < ARRAY_SIZE(audio_ic))
				tvee->audio_processor = audio_ic[audioic].id;
			else
				tvee->audio_processor = TVEEPROM_AUDPROC_OTHER;
			break;

		/* case 0x03: tag 'EEInfo' */

		case 0x04:
			/* tag 'SerialID2' */
			tvee->serial_number =
				eeprom_data[i+5] +
				(eeprom_data[i+6] << 8) +
				(eeprom_data[i+7] << 16)+
				(eeprom_data[i+8] << 24);

			if (eeprom_data[i + 8] == 0xf0) {
				tvee->MAC_address[0] = 0x00;
				tvee->MAC_address[1] = 0x0D;
				tvee->MAC_address[2] = 0xFE;
				tvee->MAC_address[3] = eeprom_data[i + 7];
				tvee->MAC_address[4] = eeprom_data[i + 6];
				tvee->MAC_address[5] = eeprom_data[i + 5];
				tvee->has_MAC_address = 1;
			}
			break;

		case 0x05:
			/* tag 'Audio2'
			Note mask with 0x7F, high bit used on some older models
			to indicate 4052 mux was removed in favor of using MSP
			inputs directly. */
			audioic = eeprom_data[i+1] & 0x7f;
			if (audioic < ARRAY_SIZE(audio_ic))
				tvee->audio_processor = audio_ic[audioic].id;
			else
				tvee->audio_processor = TVEEPROM_AUDPROC_OTHER;

			break;

		case 0x06:
			/* tag 'ModelRev' */
			tvee->model =
				eeprom_data[i + 1] +
				(eeprom_data[i + 2] << 8) +
				(eeprom_data[i + 3] << 16) +
				(eeprom_data[i + 4] << 24);
			tvee->revision =
				eeprom_data[i + 5] +
				(eeprom_data[i + 6] << 8) +
				(eeprom_data[i + 7] << 16);
			break;

		case 0x07:
			/* tag 'Details': according to Hauppauge not interesting
			on any PCI-era or later boards. */
			break;

		/* there is no tag 0x08 defined */

		case 0x09:
			/* tag 'Video' */
			tvee->decoder_processor = eeprom_data[i + 1];
			break;

		case 0x0a:
			/* tag 'Tuner' */
			if (beenhere == 0) {
				tuner1 = eeprom_data[i + 2];
				t_format1 = eeprom_data[i + 1];
				beenhere = 1;
			} else {
				/* a second (radio) tuner may be present */
				tuner2 = eeprom_data[i + 2];
				t_format2 = eeprom_data[i + 1];
				/* not a TV tuner? */
				if (t_format2 == 0)
					tvee->has_radio = 1; /* must be radio */
			}
			break;

		case 0x0b:
			/* tag 'Inputs': according to Hauppauge this is specific
			to each driver family, so no good assumptions can be
			made. */
			break;

		/* case 0x0c: tag 'Balun' */
		/* case 0x0d: tag 'Teletext' */

		case 0x0e:
			/* tag: 'Radio' */
			tvee->has_radio = eeprom_data[i+1];
			break;

		case 0x0f:
			/* tag 'IRInfo' */
			tvee->has_ir = 1 | (eeprom_data[i+1] << 1);
			break;

		/* case 0x10: tag 'VBIInfo' */
		/* case 0x11: tag 'QCInfo' */
		/* case 0x12: tag 'InfoBits' */

		default:
			pr_debug("Not sure what to do with tag [%02x]\n",
					tag);
			/* dump the rest of the packet? */
		}
	}

	if (!done) {
		pr_warn("Ran out of data!\n");
		return;
	}

	if (tvee->revision != 0) {
		tvee->rev_str[0] = 32 + ((tvee->revision >> 18) & 0x3f);
		tvee->rev_str[1] = 32 + ((tvee->revision >> 12) & 0x3f);
		tvee->rev_str[2] = 32 + ((tvee->revision >>  6) & 0x3f);
		tvee->rev_str[3] = 32 + (tvee->revision & 0x3f);
		tvee->rev_str[4] = 0;
	}

	if (hasRadioTuner(tuner1) && !tvee->has_radio) {
		pr_info("The eeprom says no radio is present, but the tuner type\n");
		pr_info("indicates otherwise. I will assume that radio is present.\n");
		tvee->has_radio = 1;
	}

	if (tuner1 < ARRAY_SIZE(hauppauge_tuner)) {
		tvee->tuner_type = hauppauge_tuner[tuner1].id;
		t_name1 = hauppauge_tuner[tuner1].name;
	} else {
		t_name1 = "unknown";
	}

	if (tuner2 < ARRAY_SIZE(hauppauge_tuner)) {
		tvee->tuner2_type = hauppauge_tuner[tuner2].id;
		t_name2 = hauppauge_tuner[tuner2].name;
	} else {
		t_name2 = "unknown";
	}

	tvee->tuner_hauppauge_model = tuner1;
	tvee->tuner2_hauppauge_model = tuner2;
	tvee->tuner_formats = 0;
	tvee->tuner2_formats = 0;
	for (i = j = 0; i < 8; i++) {
		if (t_format1 & (1 << i)) {
			tvee->tuner_formats |= hauppauge_tuner_fmt[i].id;
			t_fmt_name1[j++] = hauppauge_tuner_fmt[i].name;
		}
	}
	for (i = j = 0; i < 8; i++) {
		if (t_format2 & (1 << i)) {
			tvee->tuner2_formats |= hauppauge_tuner_fmt[i].id;
			t_fmt_name2[j++] = hauppauge_tuner_fmt[i].name;
		}
	}

	pr_info("Hauppauge model %d, rev %s, serial# %u\n",
		tvee->model, tvee->rev_str, tvee->serial_number);
	if (tvee->has_MAC_address == 1)
		pr_info("MAC address is %pM\n", tvee->MAC_address);
	pr_info("tuner model is %s (idx %d, type %d)\n",
		t_name1, tuner1, tvee->tuner_type);
	pr_info("TV standards%s%s%s%s%s%s%s%s (eeprom 0x%02x)\n",
		t_fmt_name1[0], t_fmt_name1[1], t_fmt_name1[2],
		t_fmt_name1[3],	t_fmt_name1[4], t_fmt_name1[5],
		t_fmt_name1[6], t_fmt_name1[7],	t_format1);
	if (tuner2)
		pr_info("second tuner model is %s (idx %d, type %d)\n",
					t_name2, tuner2, tvee->tuner2_type);
	if (t_format2)
		pr_info("TV standards%s%s%s%s%s%s%s%s (eeprom 0x%02x)\n",
			t_fmt_name2[0], t_fmt_name2[1], t_fmt_name2[2],
			t_fmt_name2[3],	t_fmt_name2[4], t_fmt_name2[5],
			t_fmt_name2[6], t_fmt_name2[7], t_format2);
	if (audioic < 0) {
		pr_info("audio processor is unknown (no idx)\n");
		tvee->audio_processor = TVEEPROM_AUDPROC_OTHER;
	} else {
		if (audioic < ARRAY_SIZE(audio_ic))
			pr_info("audio processor is %s (idx %d)\n",
					audio_ic[audioic].name, audioic);
		else
			pr_info("audio processor is unknown (idx %d)\n",
								audioic);
	}
	if (tvee->decoder_processor)
		pr_info("decoder processor is %s (idx %d)\n",
			STRM(decoderIC, tvee->decoder_processor),
			tvee->decoder_processor);
	if (tvee->has_ir)
		pr_info("has %sradio, has %sIR receiver, has %sIR transmitter\n",
				tvee->has_radio ? "" : "no ",
				(tvee->has_ir & 2) ? "" : "no ",
				(tvee->has_ir & 4) ? "" : "no ");
	else
		pr_info("has %sradio\n",
				tvee->has_radio ? "" : "no ");
}
EXPORT_SYMBOL(tveeprom_hauppauge_analog);

/* ----------------------------------------------------------------------- */
/* generic helper functions                                                */

int tveeprom_read(struct i2c_client *c, unsigned char *eedata, int len)
{
	unsigned char buf;
	int err;

	buf = 0;
	err = i2c_master_send(c, &buf, 1);
	if (err != 1) {
		pr_info("Huh, no eeprom present (err=%d)?\n", err);
		return -1;
	}
	err = i2c_master_recv(c, eedata, len);
	if (err != len) {
		pr_warn("i2c eeprom read error (err=%d)\n", err);
		return -1;
	}

	print_hex_dump_debug("full 256-byte eeprom dump:", DUMP_PREFIX_NONE,
			     16, 1, eedata, len, true);
	return 0;
}
EXPORT_SYMBOL(tveeprom_read);
