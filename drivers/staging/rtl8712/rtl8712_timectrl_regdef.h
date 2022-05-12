/* SPDX-License-Identifier: GPL-2.0 */
/******************************************************************************
 *
 * Copyright(c) 2007 - 2010 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#ifndef __RTL8712_TIMECTRL_REGDEF_H__
#define __RTL8712_TIMECTRL_REGDEF_H__

#define TSFTR			(RTL8712_TIMECTRL_ + 0x00)
#define USTIME			(RTL8712_TIMECTRL_ + 0x08)
#define SLOT			(RTL8712_TIMECTRL_ + 0x09)
#define TUBASE			(RTL8712_TIMECTRL_ + 0x0A)
#define SIFS_CCK		(RTL8712_TIMECTRL_ + 0x0C)
#define SIFS_OFDM		(RTL8712_TIMECTRL_ + 0x0E)
#define PIFS			(RTL8712_TIMECTRL_ + 0x10)
#define ACKTO			(RTL8712_TIMECTRL_ + 0x11)
#define EIFS			(RTL8712_TIMECTRL_ + 0x12)
#define BCNITV			(RTL8712_TIMECTRL_ + 0x14)
#define ATIMWND			(RTL8712_TIMECTRL_ + 0x16)
#define DRVERLYINT		(RTL8712_TIMECTRL_ + 0x18)
#define BCNDMATIM		(RTL8712_TIMECTRL_ + 0x1A)
#define BCNERRTH		(RTL8712_TIMECTRL_ + 0x1C)
#define MLT			(RTL8712_TIMECTRL_ + 0x1D)

#endif /* __RTL8712_TIMECTRL_REGDEF_H__ */
