/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef __SOUND_MPU401_H
#define __SOUND_MPU401_H

/*
 *  Header file for MPU-401 and compatible cards
 *  Copyright (c) by Jaroslav Kysela <perex@perex.cz>
 */

#include <sound/rawmidi.h>
#include <linux/interrupt.h>

#define MPU401_HW_MPU401		1	/* native MPU401 */
#define MPU401_HW_SB			2	/* SoundBlaster MPU-401 UART */
#define MPU401_HW_ES1688		3	/* AudioDrive ES1688 MPU-401 UART */
#define MPU401_HW_OPL3SA2		4	/* Yamaha OPL3-SA2 */
#define MPU401_HW_SONICVIBES		5	/* S3 SonicVibes */
#define MPU401_HW_CS4232		6	/* CS4232 */
#define MPU401_HW_ES18XX		7	/* AudioDrive ES18XX MPU-401 UART */
#define MPU401_HW_FM801			8	/* ForteMedia FM801 */
#define MPU401_HW_TRID4DWAVE		9	/* Trident 4DWave */
#define MPU401_HW_AZT2320		10	/* Aztech AZT2320 */
#define MPU401_HW_ALS100		11	/* Avance Logic ALS100 */
#define MPU401_HW_ICE1712		12	/* Envy24 */
#define MPU401_HW_VIA686A		13	/* VIA 82C686A */
#define MPU401_HW_YMFPCI		14	/* YMF DS-XG PCI */
#define MPU401_HW_CMIPCI		15	/* CMIPCI MPU-401 UART */
#define MPU401_HW_ALS4000		16	/* Avance Logic ALS4000 */
#define MPU401_HW_INTEL8X0		17	/* Intel8x0 driver */
#define MPU401_HW_PC98II		18	/* Roland PC98II */
#define MPU401_HW_AUREAL		19	/* Aureal Vortex */

#define MPU401_INFO_INPUT	(1 << 0)	/* input stream */
#define MPU401_INFO_OUTPUT	(1 << 1)	/* output stream */
#define MPU401_INFO_INTEGRATED	(1 << 2)	/* integrated h/w port */
#define MPU401_INFO_MMIO	(1 << 3)	/* MMIO access */
#define MPU401_INFO_TX_IRQ	(1 << 4)	/* independent TX irq */
#define MPU401_INFO_IRQ_HOOK	(1 << 5)	/* mpu401 irq handler is called
						   from driver irq handler */
#define MPU401_INFO_NO_ACK	(1 << 6)	/* No ACK cmd needed */
#define MPU401_INFO_USE_TIMER	(1 << 15)	/* internal */

#define MPU401_MODE_BIT_INPUT		0
#define MPU401_MODE_BIT_OUTPUT		1
#define MPU401_MODE_BIT_INPUT_TRIGGER	2
#define MPU401_MODE_BIT_OUTPUT_TRIGGER	3

#define MPU401_MODE_INPUT		(1<<MPU401_MODE_BIT_INPUT)
#define MPU401_MODE_OUTPUT		(1<<MPU401_MODE_BIT_OUTPUT)
#define MPU401_MODE_INPUT_TRIGGER	(1<<MPU401_MODE_BIT_INPUT_TRIGGER)
#define MPU401_MODE_OUTPUT_TRIGGER	(1<<MPU401_MODE_BIT_OUTPUT_TRIGGER)

#define MPU401_MODE_INPUT_TIMER		(1<<0)
#define MPU401_MODE_OUTPUT_TIMER	(1<<1)

struct snd_mpu401 {
	struct snd_rawmidi *rmidi;

	unsigned short hardware;	/* MPU401_HW_XXXX */
	unsigned int info_flags;	/* MPU401_INFO_XXX */
	unsigned long port;		/* base port of MPU-401 chip */
	unsigned long cport;		/* port + 1 (usually) */
	struct resource *res;		/* port resource */
	int irq;			/* IRQ number of MPU-401 chip */

	unsigned long mode;		/* MPU401_MODE_XXXX */
	int timer_invoked;

	int (*open_input) (struct snd_mpu401 * mpu);
	void (*close_input) (struct snd_mpu401 * mpu);
	int (*open_output) (struct snd_mpu401 * mpu);
	void (*close_output) (struct snd_mpu401 * mpu);
	void *private_data;

	struct snd_rawmidi_substream *substream_input;
	struct snd_rawmidi_substream *substream_output;

	spinlock_t input_lock;
	spinlock_t output_lock;
	spinlock_t timer_lock;
	
	struct timer_list timer;

	void (*write) (struct snd_mpu401 * mpu, unsigned char data, unsigned long addr);
	unsigned char (*read) (struct snd_mpu401 *mpu, unsigned long addr);
};

/* I/O ports */

#define MPU401C(mpu) (mpu)->cport
#define MPU401D(mpu) (mpu)->port

/*
 * control register bits
 */
/* read MPU401C() */
#define MPU401_RX_EMPTY		0x80
#define MPU401_TX_FULL		0x40

/* write MPU401C() */
#define MPU401_RESET		0xff
#define MPU401_ENTER_UART	0x3f

/* read MPU401D() */
#define MPU401_ACK		0xfe


/*

 */

irqreturn_t snd_mpu401_uart_interrupt(int irq, void *dev_id);
irqreturn_t snd_mpu401_uart_interrupt_tx(int irq, void *dev_id);

int snd_mpu401_uart_new(struct snd_card *card,
			int device,
			unsigned short hardware,
			unsigned long port,
			unsigned int info_flags,
			int irq,
			struct snd_rawmidi ** rrawmidi);

#endif /* __SOUND_MPU401_H */
