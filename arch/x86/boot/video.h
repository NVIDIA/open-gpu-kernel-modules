/* SPDX-License-Identifier: GPL-2.0-only */
/* -*- linux-c -*- ------------------------------------------------------- *
 *
 *   Copyright (C) 1991, 1992 Linus Torvalds
 *   Copyright 2007 rPath, Inc. - All Rights Reserved
 *
 * ----------------------------------------------------------------------- */

/*
 * Header file for the real-mode video probing code
 */

#ifndef BOOT_VIDEO_H
#define BOOT_VIDEO_H

#include <linux/types.h>

/*
 * This code uses an extended set of video mode numbers. These include:
 * Aliases for standard modes
 *      NORMAL_VGA (-1)
 *      EXTENDED_VGA (-2)
 *      ASK_VGA (-3)
 * Video modes numbered by menu position -- NOT RECOMMENDED because of lack
 * of compatibility when extending the table. These are between 0x00 and 0xff.
 */
#define VIDEO_FIRST_MENU 0x0000

/* Standard BIOS video modes (BIOS number + 0x0100) */
#define VIDEO_FIRST_BIOS 0x0100

/* VESA BIOS video modes (VESA number + 0x0200) */
#define VIDEO_FIRST_VESA 0x0200

/* Video7 special modes (BIOS number + 0x0900) */
#define VIDEO_FIRST_V7 0x0900

/* Special video modes */
#define VIDEO_FIRST_SPECIAL 0x0f00
#define VIDEO_80x25 0x0f00
#define VIDEO_8POINT 0x0f01
#define VIDEO_80x43 0x0f02
#define VIDEO_80x28 0x0f03
#define VIDEO_CURRENT_MODE 0x0f04
#define VIDEO_80x30 0x0f05
#define VIDEO_80x34 0x0f06
#define VIDEO_80x60 0x0f07
#define VIDEO_GFX_HACK 0x0f08
#define VIDEO_LAST_SPECIAL 0x0f09

/* Video modes given by resolution */
#define VIDEO_FIRST_RESOLUTION 0x1000

/* The "recalculate timings" flag */
#define VIDEO_RECALC 0x8000

void store_screen(void);
#define DO_STORE() store_screen()

/*
 * Mode table structures
 */

struct mode_info {
	u16 mode;		/* Mode number (vga= style) */
	u16 x, y;		/* Width, height */
	u16 depth;		/* Bits per pixel, 0 for text mode */
};

struct card_info {
	const char *card_name;
	int (*set_mode)(struct mode_info *mode);
	int (*probe)(void);
	struct mode_info *modes;
	int nmodes;		/* Number of probed modes so far */
	int unsafe;		/* Probing is unsafe, only do after "scan" */
	u16 xmode_first;	/* Unprobed modes to try to call anyway */
	u16 xmode_n;		/* Size of unprobed mode range */
};

#define __videocard struct card_info __section(".videocards") __attribute__((used))
extern struct card_info video_cards[], video_cards_end[];

int mode_defined(u16 mode);	/* video.c */

/* Basic video information */
#define ADAPTER_CGA	0	/* CGA/MDA/HGC */
#define ADAPTER_EGA	1
#define ADAPTER_VGA	2

extern int adapter;
extern int force_x, force_y;	/* Don't query the BIOS for cols/rows */
extern int do_restore;		/* Restore screen contents */
extern int graphic_mode;	/* Graphics mode with linear frame buffer */

/* Accessing VGA indexed registers */
static inline u8 in_idx(u16 port, u8 index)
{
	outb(index, port);
	return inb(port+1);
}

static inline void out_idx(u8 v, u16 port, u8 index)
{
	outw(index+(v << 8), port);
}

/* Writes a value to an indexed port and then reads the port again */
static inline u8 tst_idx(u8 v, u16 port, u8 index)
{
	out_idx(port, index, v);
	return in_idx(port, index);
}

/* Get the I/O port of the VGA CRTC */
u16 vga_crtc(void);		/* video-vga.c */

#endif /* BOOT_VIDEO_H */
