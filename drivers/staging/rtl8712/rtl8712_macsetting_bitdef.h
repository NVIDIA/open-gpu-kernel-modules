/* SPDX-License-Identifier: GPL-2.0 */
/******************************************************************************
 *
 * Copyright(c) 2007 - 2010 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#ifndef __RTL8712_MACSETTING_BITDEF_H__
#define __RTL8712_MACSETTING_BITDEF_H__


/*MACID*/
/*BSSID*/

/*HWVID*/
#define	_HWVID_MSK				0x0F

/*MAR*/
/*MBIDCANCONTENT*/

/*MBIDCANCFG*/
#define	_POOLING				BIT(31)
#define	_WRITE_EN				BIT(16)
#define	_CAM_ADDR_MSK			0x001F
#define	_CAM_ADDR_SHT			0

/*BUILDTIME*/
#define _BUILDTIME_MSK			0x3FFFFFFF

/*BUILDUSER*/



#endif /* __RTL8712_MACSETTING_BITDEF_H__*/

