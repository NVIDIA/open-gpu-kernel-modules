/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2003 Simtec Electronics <linux@simtec.co.uk>
 *		      http://www.simtec.co.uk/products/SWLINUX/
 */

#ifndef ___ASM_ARCH_REGS_LCD_H
#define ___ASM_ARCH_REGS_LCD_H

/*
 * a couple of values are used as platform data in
 * include/linux/platform_data/fb-s3c2410.h and not
 * duplicated here.
 */
#include <linux/platform_data/fb-s3c2410.h>

#define S3C2410_LCDREG(x)	(x)

/* LCD control registers */
#define S3C2410_LCDCON1	    S3C2410_LCDREG(0x00)
#define S3C2410_LCDCON2	    S3C2410_LCDREG(0x04)
#define S3C2410_LCDCON3	    S3C2410_LCDREG(0x08)
#define S3C2410_LCDCON4	    S3C2410_LCDREG(0x0C)
#define S3C2410_LCDCON5	    S3C2410_LCDREG(0x10)

#define S3C2410_LCDCON1_CLKVAL(x)  ((x) << 8)
#define S3C2410_LCDCON1_MMODE	   (1<<7)
#define S3C2410_LCDCON1_DSCAN4	   (0<<5)
#define S3C2410_LCDCON1_STN4	   (1<<5)
#define S3C2410_LCDCON1_STN8	   (2<<5)
#define S3C2410_LCDCON1_TFT	   (3<<5)

#define S3C2410_LCDCON1_STN1BPP	   (0<<1)
#define S3C2410_LCDCON1_STN2GREY   (1<<1)
#define S3C2410_LCDCON1_STN4GREY   (2<<1)
#define S3C2410_LCDCON1_STN8BPP	   (3<<1)
#define S3C2410_LCDCON1_STN12BPP   (4<<1)

#define S3C2410_LCDCON1_ENVID	   (1)

#define S3C2410_LCDCON1_MODEMASK    0x1E

#define S3C2410_LCDCON2_VBPD(x)	    ((x) << 24)
#define S3C2410_LCDCON2_LINEVAL(x)  ((x) << 14)
#define S3C2410_LCDCON2_VFPD(x)	    ((x) << 6)
#define S3C2410_LCDCON2_VSPW(x)	    ((x) << 0)

#define S3C2410_LCDCON2_GET_VBPD(x) ( ((x) >> 24) & 0xFF)
#define S3C2410_LCDCON2_GET_VFPD(x) ( ((x) >>  6) & 0xFF)
#define S3C2410_LCDCON2_GET_VSPW(x) ( ((x) >>  0) & 0x3F)

#define S3C2410_LCDCON3_HBPD(x)	    ((x) << 19)
#define S3C2410_LCDCON3_WDLY(x)	    ((x) << 19)
#define S3C2410_LCDCON3_HOZVAL(x)   ((x) << 8)
#define S3C2410_LCDCON3_HFPD(x)	    ((x) << 0)
#define S3C2410_LCDCON3_LINEBLANK(x)((x) << 0)

#define S3C2410_LCDCON3_GET_HBPD(x) ( ((x) >> 19) & 0x7F)
#define S3C2410_LCDCON3_GET_HFPD(x) ( ((x) >>  0) & 0xFF)

/* LDCCON4 changes for STN mode on the S3C2412 */

#define S3C2410_LCDCON4_MVAL(x)	    ((x) << 8)
#define S3C2410_LCDCON4_HSPW(x)	    ((x) << 0)
#define S3C2410_LCDCON4_WLH(x)	    ((x) << 0)

#define S3C2410_LCDCON4_GET_HSPW(x) ( ((x) >>  0) & 0xFF)

/* framebuffer start addressed */
#define S3C2410_LCDSADDR1   S3C2410_LCDREG(0x14)
#define S3C2410_LCDSADDR2   S3C2410_LCDREG(0x18)
#define S3C2410_LCDSADDR3   S3C2410_LCDREG(0x1C)

#define S3C2410_LCDBANK(x)	((x) << 21)
#define S3C2410_LCDBASEU(x)	(x)

#define S3C2410_OFFSIZE(x)	((x) << 11)
#define S3C2410_PAGEWIDTH(x)	(x)

/* colour lookup and miscellaneous controls */

#define S3C2410_REDLUT	   S3C2410_LCDREG(0x20)
#define S3C2410_GREENLUT   S3C2410_LCDREG(0x24)
#define S3C2410_BLUELUT	   S3C2410_LCDREG(0x28)

#define S3C2410_DITHMODE   S3C2410_LCDREG(0x4C)
#define S3C2410_TPAL	   S3C2410_LCDREG(0x50)

#define S3C2410_TPAL_EN		(1<<24)

/* interrupt info */
#define S3C2410_LCDINTPND  S3C2410_LCDREG(0x54)
#define S3C2410_LCDSRCPND  S3C2410_LCDREG(0x58)
#define S3C2410_LCDINTMSK  S3C2410_LCDREG(0x5C)
#define S3C2410_LCDINT_FIWSEL	(1<<2)
#define	S3C2410_LCDINT_FRSYNC	(1<<1)
#define S3C2410_LCDINT_FICNT	(1<<0)

/* s3c2442 extra stn registers */

#define S3C2442_REDLUT		S3C2410_LCDREG(0x20)
#define S3C2442_GREENLUT	S3C2410_LCDREG(0x24)
#define S3C2442_BLUELUT		S3C2410_LCDREG(0x28)
#define S3C2442_DITHMODE	S3C2410_LCDREG(0x20)

#define S3C2410_LPCSEL	   S3C2410_LCDREG(0x60)

#define S3C2410_TFTPAL(x)  S3C2410_LCDREG((0x400 + (x)*4))

/* S3C2412 registers */

#define S3C2412_TPAL		S3C2410_LCDREG(0x20)

#define S3C2412_LCDINTPND	S3C2410_LCDREG(0x24)
#define S3C2412_LCDSRCPND	S3C2410_LCDREG(0x28)
#define S3C2412_LCDINTMSK	S3C2410_LCDREG(0x2C)

#define S3C2412_TCONSEL		S3C2410_LCDREG(0x30)

#define S3C2412_LCDCON6		S3C2410_LCDREG(0x34)
#define S3C2412_LCDCON7		S3C2410_LCDREG(0x38)
#define S3C2412_LCDCON8		S3C2410_LCDREG(0x3C)
#define S3C2412_LCDCON9		S3C2410_LCDREG(0x40)

#define S3C2412_REDLUT(x)	S3C2410_LCDREG(0x44 + ((x)*4))
#define S3C2412_GREENLUT(x)	S3C2410_LCDREG(0x60 + ((x)*4))
#define S3C2412_BLUELUT(x)	S3C2410_LCDREG(0x98 + ((x)*4))

#define S3C2412_FRCPAT(x)	S3C2410_LCDREG(0xB4 + ((x)*4))

/* general registers */

/* base of the LCD registers, where INTPND, INTSRC and then INTMSK
 * are available. */

#define S3C2410_LCDINTBASE	S3C2410_LCDREG(0x54)
#define S3C2412_LCDINTBASE	S3C2410_LCDREG(0x24)

#define S3C24XX_LCDINTPND	(0x00)
#define S3C24XX_LCDSRCPND	(0x04)
#define S3C24XX_LCDINTMSK	(0x08)

#endif /* ___ASM_ARCH_REGS_LCD_H */
