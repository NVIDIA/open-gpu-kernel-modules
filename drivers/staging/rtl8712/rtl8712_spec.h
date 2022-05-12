/* SPDX-License-Identifier: GPL-2.0 */
/******************************************************************************
 *
 * Copyright(c) 2007 - 2010 Realtek Corporation. All rights reserved.
 *
 * Modifications for inclusion into the Linux staging tree are
 * Copyright(c) 2010 Larry Finger. All rights reserved.
 *
 * Contact information:
 * WLAN FAE <wlanfae@realtek.com>
 * Larry Finger <Larry.Finger@lwfinger.net>
 *
 ******************************************************************************/
#ifndef __RTL8712_SPEC_H__
#define __RTL8712_SPEC_H__

#define RTL8712_IOBASE_TXPKT		0x10200000	/*IOBASE_TXPKT*/
#define RTL8712_IOBASE_RXPKT		0x10210000	/*IOBASE_RXPKT*/
#define RTL8712_IOBASE_RXCMD		0x10220000	/*IOBASE_RXCMD*/
#define RTL8712_IOBASE_TXSTATUS		0x10230000	/*IOBASE_TXSTATUS*/
#define RTL8712_IOBASE_RXSTATUS		0x10240000	/*IOBASE_RXSTATUS*/
#define RTL8712_IOBASE_IOREG		0x10250000	/*IOBASE_IOREG ADDR*/
#define RTL8712_IOBASE_SCHEDULER	0x10260000	/*IOBASE_SCHEDULE*/

#define RTL8712_IOBASE_TRXDMA		0x10270000	/*IOBASE_TRXDMA*/
#define RTL8712_IOBASE_TXLLT		0x10280000	/*IOBASE_TXLLT*/
#define RTL8712_IOBASE_WMAC		0x10290000	/*IOBASE_WMAC*/
#define RTL8712_IOBASE_FW2HW		0x102A0000	/*IOBASE_FW2HW*/
#define RTL8712_IOBASE_ACCESS_PHYREG	0x102B0000	/*IOBASE_ACCESS_PHYREG*/

#define RTL8712_IOBASE_FF	0x10300000 /*IOBASE_FIFO 0x1031000~0x103AFFFF*/


/*IOREG Offset for 8712*/
#define RTL8712_SYSCFG_		RTL8712_IOBASE_IOREG
#define RTL8712_CMDCTRL_	(RTL8712_IOBASE_IOREG + 0x40)
#define RTL8712_MACIDSETTING_	(RTL8712_IOBASE_IOREG + 0x50)
#define RTL8712_TIMECTRL_	(RTL8712_IOBASE_IOREG + 0x80)
#define RTL8712_FIFOCTRL_	(RTL8712_IOBASE_IOREG + 0xA0)
#define RTL8712_RATECTRL_	(RTL8712_IOBASE_IOREG + 0x160)
#define RTL8712_EDCASETTING_	(RTL8712_IOBASE_IOREG + 0x1D0)
#define RTL8712_WMAC_		(RTL8712_IOBASE_IOREG + 0x200)
#define RTL8712_SECURITY_	(RTL8712_IOBASE_IOREG + 0x240)
#define RTL8712_POWERSAVE_	(RTL8712_IOBASE_IOREG + 0x260)
#define RTL8712_GP_		(RTL8712_IOBASE_IOREG + 0x2E0)
#define RTL8712_INTERRUPT_	(RTL8712_IOBASE_IOREG + 0x300)
#define RTL8712_DEBUGCTRL_	(RTL8712_IOBASE_IOREG + 0x310)
#define RTL8712_OFFLOAD_	(RTL8712_IOBASE_IOREG + 0x2D0)


/*FIFO for 8712*/
#define RTL8712_DMA_BCNQ	(RTL8712_IOBASE_FF + 0x10000)
#define RTL8712_DMA_MGTQ	(RTL8712_IOBASE_FF + 0x20000)
#define RTL8712_DMA_BMCQ	(RTL8712_IOBASE_FF + 0x30000)
#define RTL8712_DMA_VOQ		(RTL8712_IOBASE_FF + 0x40000)
#define RTL8712_DMA_VIQ		(RTL8712_IOBASE_FF + 0x50000)
#define RTL8712_DMA_BEQ		(RTL8712_IOBASE_FF + 0x60000)
#define RTL8712_DMA_BKQ		(RTL8712_IOBASE_FF + 0x70000)
#define RTL8712_DMA_RX0FF	(RTL8712_IOBASE_FF + 0x80000)
#define RTL8712_DMA_H2CCMD	(RTL8712_IOBASE_FF + 0x90000)
#define RTL8712_DMA_C2HCMD	(RTL8712_IOBASE_FF + 0xA0000)


/*------------------------------*/

/*BIT 16 15*/
#define	DID_SDIO_LOCAL			0	/* 0 0*/
#define	DID_WLAN_IOREG			1	/* 0 1*/
#define	DID_WLAN_FIFO			3	/* 1 1*/
#define   DID_UNDEFINE				(-1)

#define CMD_ADDR_MAPPING_SHIFT		2	/*SDIO CMD ADDR MAPPING,
						 *shift 2 bit for match
						 * offset[14:2]
						 */

/*Offset for SDIO LOCAL*/
#define	OFFSET_SDIO_LOCAL				0x0FFF

/*Offset for WLAN IOREG*/
#define OFFSET_WLAN_IOREG				0x0FFF

/*Offset for WLAN FIFO*/
#define	OFFSET_TX_BCNQ				0x0300
#define	OFFSET_TX_HIQ					0x0310
#define	OFFSET_TX_CMDQ				0x0320
#define	OFFSET_TX_MGTQ				0x0330
#define	OFFSET_TX_HCCAQ				0x0340
#define	OFFSET_TX_VOQ					0x0350
#define	OFFSET_TX_VIQ					0x0360
#define	OFFSET_TX_BEQ					0x0370
#define	OFFSET_TX_BKQ					0x0380
#define	OFFSET_RX_RX0FFQ				0x0390
#define	OFFSET_RX_C2HFFQ				0x03A0

#define	BK_QID_01	1
#define	BK_QID_02	2
#define	BE_QID_01	0
#define	BE_QID_02	3
#define	VI_QID_01	4
#define	VI_QID_02	5
#define	VO_QID_01	6
#define	VO_QID_02	7
#define	HCCA_QID_01	8
#define	HCCA_QID_02	9
#define	HCCA_QID_03	10
#define	HCCA_QID_04	11
#define	HCCA_QID_05	12
#define	HCCA_QID_06	13
#define	HCCA_QID_07	14
#define	HCCA_QID_08	15
#define	HI_QID		17
#define	CMD_QID	19
#define	MGT_QID	18
#define	BCN_QID	16

#include "rtl8712_regdef.h"

#include "rtl8712_bitdef.h"

#include "basic_types.h"

#endif /* __RTL8712_SPEC_H__ */

