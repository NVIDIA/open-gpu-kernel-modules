/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef __SOUND_AUREON_H
#define __SOUND_AUREON_H

/*
 *   ALSA driver for VIA VT1724 (Envy24HT)
 *
 *   Lowlevel functions for Terratec Aureon cards
 *
 *	Copyright (c) 2003 Takashi Iwai <tiwai@suse.de>
 */      

#define  AUREON_DEVICE_DESC 	       "{Terratec,Aureon 5.1 Sky},"\
				       "{Terratec,Aureon 7.1 Space},"\
				       "{Terratec,Aureon 7.1 Universe}," \
					"{AudioTrak,Prodigy 7.1}," \
					"{AudioTrak,Prodigy 7.1 LT},"\
					"{AudioTrak,Prodigy 7.1 XT},"

#define VT1724_SUBDEVICE_AUREON51_SKY	0x3b154711	/* Aureon 5.1 Sky */
#define VT1724_SUBDEVICE_AUREON71_SPACE	0x3b154511	/* Aureon 7.1 Space */
#define VT1724_SUBDEVICE_AUREON71_UNIVERSE	0x3b155311	/* Aureon 7.1 Universe */
#define VT1724_SUBDEVICE_PRODIGY71	0x33495345	/* PRODIGY 7.1 */
#define VT1724_SUBDEVICE_PRODIGY71LT	0x32315441	/* PRODIGY 7.1 LT */
#define VT1724_SUBDEVICE_PRODIGY71XT	0x36315441	/* PRODIGY 7.1 XT*/

extern struct snd_ice1712_card_info  snd_vt1724_aureon_cards[];

/* GPIO bits */
#define AUREON_CS8415_CS	(1 << 22)
#define AUREON_SPI_MISO		(1 << 21)
#define AUREON_WM_RESET		(1 << 20)
#define AUREON_SPI_CLK		(1 << 19)
#define AUREON_SPI_MOSI		(1 << 18)
#define AUREON_WM_RW		(1 << 17)
#define AUREON_AC97_RESET	(1 << 16)
#define AUREON_DIGITAL_SEL1	(1 << 15)
#define AUREON_HP_SEL		(1 << 14)
#define AUREON_WM_CS		(1 << 12)
#define AUREON_AC97_COMMIT	(1 << 11)
#define AUREON_AC97_ADDR	(1 << 10)
#define AUREON_AC97_DATA_LOW	(1 << 9)
#define AUREON_AC97_DATA_HIGH	(1 << 8)
#define AUREON_AC97_DATA_MASK	0xFF

#define PRODIGY_WM_CS		(1 << 8)
#define PRODIGY_SPI_MOSI	(1 << 10)
#define PRODIGY_SPI_CLK		(1 << 9)
#define PRODIGY_HP_SEL		(1 << 5)

#endif /* __SOUND_AUREON_H */
