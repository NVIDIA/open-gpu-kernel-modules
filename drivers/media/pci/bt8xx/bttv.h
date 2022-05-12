/* SPDX-License-Identifier: GPL-2.0 */
/*
 *
 *  bttv - Bt848 frame grabber driver
 *
 *  card ID's and external interfaces of the bttv driver
 *  basically stuff needed by other drivers (i2c, lirc, ...)
 *  and is supported not to change much over time.
 *
 *  Copyright (C) 1996,97 Ralph Metzler (rjkm@thp.uni-koeln.de)
 *  (c) 1999,2000 Gerd Knorr <kraxel@goldbach.in-berlin.de>
 *
 */

#ifndef _BTTV_H_
#define _BTTV_H_

#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <media/v4l2-device.h>
#include <media/tuner.h>

/* ---------------------------------------------------------- */
/* exported by bttv-cards.c                                   */

#define BTTV_BOARD_UNKNOWN                 0x00
#define BTTV_BOARD_MIRO                    0x01
#define BTTV_BOARD_HAUPPAUGE               0x02
#define BTTV_BOARD_STB                     0x03
#define BTTV_BOARD_INTEL                   0x04
#define BTTV_BOARD_DIAMOND                 0x05
#define BTTV_BOARD_AVERMEDIA               0x06
#define BTTV_BOARD_MATRIX_VISION           0x07
#define BTTV_BOARD_FLYVIDEO                0x08
#define BTTV_BOARD_TURBOTV                 0x09
#define BTTV_BOARD_HAUPPAUGE878            0x0a
#define BTTV_BOARD_MIROPRO                 0x0b
#define BTTV_BOARD_ADSTECH_TV              0x0c
#define BTTV_BOARD_AVERMEDIA98             0x0d
#define BTTV_BOARD_VHX                     0x0e
#define BTTV_BOARD_ZOLTRIX                 0x0f
#define BTTV_BOARD_PIXVIEWPLAYTV           0x10
#define BTTV_BOARD_WINVIEW_601             0x11
#define BTTV_BOARD_AVEC_INTERCAP           0x12
#define BTTV_BOARD_LIFE_FLYKIT             0x13
#define BTTV_BOARD_CEI_RAFFLES             0x14
#define BTTV_BOARD_CONFERENCETV            0x15
#define BTTV_BOARD_PHOEBE_TVMAS            0x16
#define BTTV_BOARD_MODTEC_205              0x17
#define BTTV_BOARD_MAGICTVIEW061           0x18
#define BTTV_BOARD_VOBIS_BOOSTAR           0x19
#define BTTV_BOARD_HAUPPAUG_WCAM           0x1a
#define BTTV_BOARD_MAXI                    0x1b
#define BTTV_BOARD_TERRATV                 0x1c
#define BTTV_BOARD_PXC200                  0x1d
#define BTTV_BOARD_FLYVIDEO_98             0x1e
#define BTTV_BOARD_IPROTV                  0x1f
#define BTTV_BOARD_INTEL_C_S_PCI           0x20
#define BTTV_BOARD_TERRATVALUE             0x21
#define BTTV_BOARD_WINFAST2000             0x22
#define BTTV_BOARD_CHRONOS_VS2             0x23
#define BTTV_BOARD_TYPHOON_TVIEW           0x24
#define BTTV_BOARD_PXELVWPLTVPRO           0x25
#define BTTV_BOARD_MAGICTVIEW063           0x26
#define BTTV_BOARD_PINNACLE                0x27
#define BTTV_BOARD_STB2                    0x28
#define BTTV_BOARD_AVPHONE98               0x29
#define BTTV_BOARD_PV951                   0x2a
#define BTTV_BOARD_ONAIR_TV                0x2b
#define BTTV_BOARD_SIGMA_TVII_FM           0x2c
#define BTTV_BOARD_MATRIX_VISION2          0x2d
#define BTTV_BOARD_ZOLTRIX_GENIE           0x2e
#define BTTV_BOARD_TERRATVRADIO            0x2f
#define BTTV_BOARD_DYNALINK                0x30
#define BTTV_BOARD_GVBCTV3PCI              0x31
#define BTTV_BOARD_PXELVWPLTVPAK           0x32
#define BTTV_BOARD_EAGLE                   0x33
#define BTTV_BOARD_PINNACLEPRO             0x34
#define BTTV_BOARD_TVIEW_RDS_FM            0x35
#define BTTV_BOARD_LIFETEC_9415            0x36
#define BTTV_BOARD_BESTBUY_EASYTV          0x37
#define BTTV_BOARD_FLYVIDEO_98FM           0x38
#define BTTV_BOARD_GRANDTEC                0x39
#define BTTV_BOARD_ASKEY_CPH060            0x3a
#define BTTV_BOARD_ASKEY_CPH03X            0x3b
#define BTTV_BOARD_MM100PCTV               0x3c
#define BTTV_BOARD_GMV1                    0x3d
#define BTTV_BOARD_BESTBUY_EASYTV2         0x3e
#define BTTV_BOARD_ATI_TVWONDER            0x3f
#define BTTV_BOARD_ATI_TVWONDERVE          0x40
#define BTTV_BOARD_FLYVIDEO2000            0x41
#define BTTV_BOARD_TERRATVALUER            0x42
#define BTTV_BOARD_GVBCTV4PCI              0x43
#define BTTV_BOARD_VOODOOTV_FM             0x44
#define BTTV_BOARD_AIMMS                   0x45
#define BTTV_BOARD_PV_BT878P_PLUS          0x46
#define BTTV_BOARD_FLYVIDEO98EZ            0x47
#define BTTV_BOARD_PV_BT878P_9B            0x48
#define BTTV_BOARD_SENSORAY311_611         0x49
#define BTTV_BOARD_RV605                   0x4a
#define BTTV_BOARD_POWERCLR_MTV878         0x4b
#define BTTV_BOARD_WINDVR                  0x4c
#define BTTV_BOARD_GRANDTEC_MULTI          0x4d
#define BTTV_BOARD_KWORLD                  0x4e
#define BTTV_BOARD_DSP_TCVIDEO             0x4f
#define BTTV_BOARD_HAUPPAUGEPVR            0x50
#define BTTV_BOARD_GVBCTV5PCI              0x51
#define BTTV_BOARD_OSPREY1x0               0x52
#define BTTV_BOARD_OSPREY1x0_848           0x53
#define BTTV_BOARD_OSPREY101_848           0x54
#define BTTV_BOARD_OSPREY1x1               0x55
#define BTTV_BOARD_OSPREY1x1_SVID          0x56
#define BTTV_BOARD_OSPREY2xx               0x57
#define BTTV_BOARD_OSPREY2x0_SVID          0x58
#define BTTV_BOARD_OSPREY2x0               0x59
#define BTTV_BOARD_OSPREY500               0x5a
#define BTTV_BOARD_OSPREY540               0x5b
#define BTTV_BOARD_OSPREY2000              0x5c
#define BTTV_BOARD_IDS_EAGLE               0x5d
#define BTTV_BOARD_PINNACLESAT             0x5e
#define BTTV_BOARD_FORMAC_PROTV            0x5f
#define BTTV_BOARD_MACHTV                  0x60
#define BTTV_BOARD_EURESYS_PICOLO          0x61
#define BTTV_BOARD_PV150                   0x62
#define BTTV_BOARD_AD_TVK503               0x63
#define BTTV_BOARD_HERCULES_SM_TV          0x64
#define BTTV_BOARD_PACETV                  0x65
#define BTTV_BOARD_IVC200                  0x66
#define BTTV_BOARD_XGUARD                  0x67
#define BTTV_BOARD_NEBULA_DIGITV           0x68
#define BTTV_BOARD_PV143                   0x69
#define BTTV_BOARD_VD009X1_VD011_MINIDIN   0x6a
#define BTTV_BOARD_VD009X1_VD011_COMBI     0x6b
#define BTTV_BOARD_VD009_MINIDIN           0x6c
#define BTTV_BOARD_VD009_COMBI             0x6d
#define BTTV_BOARD_IVC100                  0x6e
#define BTTV_BOARD_IVC120                  0x6f
#define BTTV_BOARD_PC_HDTV                 0x70
#define BTTV_BOARD_TWINHAN_DST             0x71
#define BTTV_BOARD_WINFASTVC100            0x72
#define BTTV_BOARD_TEV560                  0x73
#define BTTV_BOARD_SIMUS_GVC1100           0x74
#define BTTV_BOARD_NGSTV_PLUS              0x75
#define BTTV_BOARD_LMLBT4                  0x76
#define BTTV_BOARD_TEKRAM_M205             0x77
#define BTTV_BOARD_CONTVFMI                0x78
#define BTTV_BOARD_PICOLO_TETRA_CHIP       0x79
#define BTTV_BOARD_SPIRIT_TV               0x7a
#define BTTV_BOARD_AVDVBT_771              0x7b
#define BTTV_BOARD_AVDVBT_761              0x7c
#define BTTV_BOARD_MATRIX_VISIONSQ         0x7d
#define BTTV_BOARD_MATRIX_VISIONSLC        0x7e
#define BTTV_BOARD_APAC_VIEWCOMP           0x7f
#define BTTV_BOARD_DVICO_DVBT_LITE         0x80
#define BTTV_BOARD_VGEAR_MYVCD             0x81
#define BTTV_BOARD_SUPER_TV                0x82
#define BTTV_BOARD_TIBET_CS16              0x83
#define BTTV_BOARD_KODICOM_4400R           0x84
#define BTTV_BOARD_KODICOM_4400R_SL        0x85
#define BTTV_BOARD_ADLINK_RTV24            0x86
#define BTTV_BOARD_DVICO_FUSIONHDTV_5_LITE 0x87
#define BTTV_BOARD_ACORP_Y878F             0x88
#define BTTV_BOARD_CONCEPTRONIC_CTVFMI2    0x89
#define BTTV_BOARD_PV_BT878P_2E            0x8a
#define BTTV_BOARD_PV_M4900                0x8b
#define BTTV_BOARD_OSPREY440               0x8c
#define BTTV_BOARD_ASOUND_SKYEYE	   0x8d
#define BTTV_BOARD_SABRENT_TVFM		   0x8e
#define BTTV_BOARD_HAUPPAUGE_IMPACTVCB     0x8f
#define BTTV_BOARD_MACHTV_MAGICTV          0x90
#define BTTV_BOARD_SSAI_SECURITY	   0x91
#define BTTV_BOARD_SSAI_ULTRASOUND	   0x92
#define BTTV_BOARD_VOODOOTV_200		   0x93
#define BTTV_BOARD_DVICO_FUSIONHDTV_2	   0x94
#define BTTV_BOARD_TYPHOON_TVTUNERPCI	   0x95
#define BTTV_BOARD_GEOVISION_GV600	   0x96
#define BTTV_BOARD_KOZUMI_KTV_01C          0x97
#define BTTV_BOARD_ENLTV_FM_2		   0x98
#define BTTV_BOARD_VD012		   0x99
#define BTTV_BOARD_VD012_X1		   0x9a
#define BTTV_BOARD_VD012_X2		   0x9b
#define BTTV_BOARD_IVCE8784		   0x9c
#define BTTV_BOARD_GEOVISION_GV800S	   0x9d
#define BTTV_BOARD_GEOVISION_GV800S_SL	   0x9e
#define BTTV_BOARD_PV183                   0x9f
#define BTTV_BOARD_TVT_TD3116		   0xa0
#define BTTV_BOARD_APOSONIC_WDVR           0xa1
#define BTTV_BOARD_ADLINK_MPG24            0xa2
#define BTTV_BOARD_BT848_CAP_14            0xa3
#define BTTV_BOARD_CYBERVISION_CV06        0xa4
#define BTTV_BOARD_KWORLD_VSTREAM_XPERT    0xa5
#define BTTV_BOARD_PCI_8604PW              0xa6

/* more card-specific defines */
#define PT2254_L_CHANNEL 0x10
#define PT2254_R_CHANNEL 0x08
#define PT2254_DBS_IN_2 0x400
#define PT2254_DBS_IN_10 0x20000
#define WINVIEW_PT2254_CLK  0x40
#define WINVIEW_PT2254_DATA 0x20
#define WINVIEW_PT2254_STROBE 0x80

struct bttv_core {
	/* device structs */
	struct v4l2_device   v4l2_dev;
	struct pci_dev       *pci;
	struct i2c_adapter   i2c_adap;
	struct list_head     subs;     /* struct bttv_sub_device */

	/* device config */
	unsigned int         nr;       /* dev nr (for printk("bttv%d: ...");  */
	unsigned int         type;     /* card type (pointer into tvcards[])  */
};

struct bttv;

struct tvcard {
	char *name;
	void (*volume_gpio)(struct bttv *btv, __u16 volume);
	void (*audio_mode_gpio)(struct bttv *btv, struct v4l2_tuner *tuner, int set);
	void (*muxsel_hook)(struct bttv *btv, unsigned int input);

	/* MUX bits for each input, two bits per input starting with the LSB */
	u32 muxsel; /* Use MUXSEL() to set */

	u32 gpiomask;
	u32 gpiomux[4];  /* Tuner, Radio, external, internal */
	u32 gpiomute;    /* GPIO mute setting */
	u32 gpiomask2;   /* GPIO MUX mask */

	unsigned int tuner_type;
	u8 tuner_addr;
	u8 video_inputs;	/* Number of inputs */
	unsigned int svhs:4;	/* Which input is s-video */
#define NO_SVHS	15
	unsigned int pll:2;
#define PLL_NONE 0
#define PLL_28   1
#define PLL_35   2
#define PLL_14   3

	/* i2c audio flags */
	unsigned int no_msp34xx:1;
	unsigned int no_tda7432:1;
	unsigned int msp34xx_alt:1;
	/* Note: currently no card definition needs to mark the presence
	   of a RDS saa6588 chip. If this is ever needed, then add a new
	   'has_saa6588' bit here. */

	unsigned int no_video:1; /* video pci function is unused */
	unsigned int has_dvb:1;
	unsigned int has_remote:1;
	unsigned int has_radio:1;
	unsigned int has_dig_in:1; /* Has digital input (always last input) */
	unsigned int no_gpioirq:1;
};

extern struct tvcard bttv_tvcards[];

/*
 * This bit of cpp voodoo is used to create a macro with a variable number of
 * arguments (1 to 16).  It will pack each argument into a word two bits at a
 * time.  It can't be a function because it needs to be compile time constant to
 * initialize structures.  Since each argument must fit in two bits, it's ok
 * that they are changed to octal.  One should not use hex number, macros, or
 * anything else with this macro.  Just use plain integers from 0 to 3.
 */
#define _MUXSELf(a)		0##a << 30
#define _MUXSELe(a, b...)	0##a << 28 | _MUXSELf(b)
#define _MUXSELd(a, b...)	0##a << 26 | _MUXSELe(b)
#define _MUXSELc(a, b...)	0##a << 24 | _MUXSELd(b)
#define _MUXSELb(a, b...)	0##a << 22 | _MUXSELc(b)
#define _MUXSELa(a, b...)	0##a << 20 | _MUXSELb(b)
#define _MUXSEL9(a, b...)	0##a << 18 | _MUXSELa(b)
#define _MUXSEL8(a, b...)	0##a << 16 | _MUXSEL9(b)
#define _MUXSEL7(a, b...)	0##a << 14 | _MUXSEL8(b)
#define _MUXSEL6(a, b...)	0##a << 12 | _MUXSEL7(b)
#define _MUXSEL5(a, b...)	0##a << 10 | _MUXSEL6(b)
#define _MUXSEL4(a, b...)	0##a << 8  | _MUXSEL5(b)
#define _MUXSEL3(a, b...)	0##a << 6  | _MUXSEL4(b)
#define _MUXSEL2(a, b...)	0##a << 4  | _MUXSEL3(b)
#define _MUXSEL1(a, b...)	0##a << 2  | _MUXSEL2(b)
#define MUXSEL(a, b...)		(a | _MUXSEL1(b))

/* identification / initialization of the card */
extern void bttv_idcard(struct bttv *btv);
extern void bttv_init_card1(struct bttv *btv);
extern void bttv_init_card2(struct bttv *btv);
extern void bttv_init_tuner(struct bttv *btv);

/* card-specific functions */
extern void tea5757_set_freq(struct bttv *btv, unsigned short freq);
extern u32 bttv_tda9880_setnorm(struct bttv *btv, u32 gpiobits);

/* extra tweaks for some chipsets */
extern void bttv_check_chipset(void);
extern int bttv_handle_chipset(struct bttv *btv);

/* ---------------------------------------------------------- */
/* exported by bttv-if.c                                      */

/* this obsolete -- please use the sysfs-based
   interface below for new code */

extern struct pci_dev* bttv_get_pcidev(unsigned int card);

/* sets GPOE register (BT848_GPIO_OUT_EN) to new value:
   data | (current_GPOE_value & ~mask)
   returns negative value if error occurred
*/
extern int bttv_gpio_enable(unsigned int card,
			    unsigned long mask, unsigned long data);

/* fills data with GPDATA register contents
   returns negative value if error occurred
*/
extern int bttv_read_gpio(unsigned int card, unsigned long *data);

/* sets GPDATA register to new value:
  (data & mask) | (current_GPDATA_value & ~mask)
  returns negative value if error occurred
*/
extern int bttv_write_gpio(unsigned int card,
			   unsigned long mask, unsigned long data);




/* ---------------------------------------------------------- */
/* sysfs/driver-moded based gpio access interface             */

struct bttv_sub_device {
	struct device    dev;
	struct bttv_core *core;
	struct list_head list;
};
#define to_bttv_sub_dev(x) container_of((x), struct bttv_sub_device, dev)

struct bttv_sub_driver {
	struct device_driver   drv;
	char                   wanted[20];
	int                    (*probe)(struct bttv_sub_device *sub);
	void                   (*remove)(struct bttv_sub_device *sub);
};
#define to_bttv_sub_drv(x) container_of((x), struct bttv_sub_driver, drv)

int bttv_sub_register(struct bttv_sub_driver *drv, char *wanted);
int bttv_sub_unregister(struct bttv_sub_driver *drv);

/* gpio access functions */
void bttv_gpio_inout(struct bttv_core *core, u32 mask, u32 outbits);
u32 bttv_gpio_read(struct bttv_core *core);
void bttv_gpio_write(struct bttv_core *core, u32 value);
void bttv_gpio_bits(struct bttv_core *core, u32 mask, u32 bits);

#define gpio_inout(mask,bits)  bttv_gpio_inout(&btv->c, mask, bits)
#define gpio_read()            bttv_gpio_read(&btv->c)
#define gpio_write(value)      bttv_gpio_write(&btv->c, value)
#define gpio_bits(mask,bits)   bttv_gpio_bits(&btv->c, mask, bits)


/* ---------------------------------------------------------- */
/* i2c                                                        */

#define bttv_call_all(btv, o, f, args...) \
	v4l2_device_call_all(&btv->c.v4l2_dev, 0, o, f, ##args)

#define bttv_call_all_err(btv, o, f, args...) \
	v4l2_device_call_until_err(&btv->c.v4l2_dev, 0, o, f, ##args)

extern int bttv_I2CRead(struct bttv *btv, unsigned char addr, char *probe_for);
extern int bttv_I2CWrite(struct bttv *btv, unsigned char addr, unsigned char b1,
			 unsigned char b2, int both);
extern void bttv_readee(struct bttv *btv, unsigned char *eedata, int addr);

extern int bttv_input_init(struct bttv *dev);
extern void bttv_input_fini(struct bttv *dev);
extern void bttv_input_irq(struct bttv *dev);

#endif /* _BTTV_H_ */
