/* SPDX-License-Identifier: GPL-2.0 */
/******************************************************************************
 *
 * Copyright(c) 2009-2013  Realtek Corporation.
 *
 * Contact Information:
 * wlanfae <wlanfae@realtek.com>
 * Realtek Corporation, No. 2, Innovation Road II, Hsinchu Science Park,
 * Hsinchu 300, Taiwan.
 * Larry Finger <Larry.Finger@lwfinger.net>
 *
 *****************************************************************************/
#include "drv_types.h"
#include <linux/types.h>

#ifndef __RTL92C__FW__H__
#define __RTL92C__FW__H__

#define FW_8192C_START_ADDRESS		0x1000
#define FW_8192C_PAGE_SIZE			4096
#define FW_8192C_POLLING_DELAY		5

struct rtl92c_firmware_header {
	__le16 signature;
	u8 category;
	u8 function;
	u16 version;
	u8 subversion;
	u8 rsvd1;
	u8 month;
	u8 date;
	u8 hour;
	u8 minute;
	u16 ramcodesize;
	u16 rsvd2;
	u32 svnindex;
	u32 rsvd3;
	u32 rsvd4;
	u32 rsvd5;
};

int rtl88eu_download_fw(struct adapter *adapt);

#endif
