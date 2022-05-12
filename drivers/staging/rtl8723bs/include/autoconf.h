/* SPDX-License-Identifier: GPL-2.0 */
/******************************************************************************
 *
 * Copyright(c) 2007 - 2012 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/

/*
 * Automatically generated C config: don't edit
 */

/*
 * Functions Config
 */

#ifndef CONFIG_WIRELESS_EXT
#error CONFIG_WIRELESS_EXT needs to be enabled for this driver to work
#endif

/*
 * Auto Config Section
 */
#define LPS_RPWM_WAIT_MS 300
#ifndef DISABLE_BB_RF
#define DISABLE_BB_RF	0
#endif

#if DISABLE_BB_RF
	#define HAL_MAC_ENABLE	0
	#define HAL_BB_ENABLE		0
	#define HAL_RF_ENABLE		0
#else
	#define HAL_MAC_ENABLE	1
	#define HAL_BB_ENABLE		1
	#define HAL_RF_ENABLE		1
#endif

/*
 * Platform dependent
 */
#define WAKEUP_GPIO_IDX	12	/* WIFI Chip Side */

/*
 * Debug Related Config
 */

#define DBG	0	/*  for ODM & BTCOEX debug */

/* define DBG_XMIT_BUF */
/* define DBG_XMIT_BUF_EXT */
