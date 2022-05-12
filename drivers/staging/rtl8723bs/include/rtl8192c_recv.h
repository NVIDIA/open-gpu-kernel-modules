/* SPDX-License-Identifier: GPL-2.0 */
/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#ifndef _RTL8192C_RECV_H_
#define _RTL8192C_RECV_H_

#define RECV_BLK_SZ 512
#define RECV_BLK_CNT 16
#define RECV_BLK_TH RECV_BLK_CNT

#define MAX_RECVBUF_SZ (10240)

struct phy_stat {
	unsigned int phydw0;

	unsigned int phydw1;

	unsigned int phydw2;

	unsigned int phydw3;

	unsigned int phydw4;

	unsigned int phydw5;

	unsigned int phydw6;

	unsigned int phydw7;
};

/*  Rx smooth factor */
#define	Rx_Smooth_Factor (20)


void rtl8192c_translate_rx_signal_stuff(union recv_frame *precvframe, struct phy_stat *pphy_status);
void rtl8192c_query_rx_desc_status(union recv_frame *precvframe, struct recv_stat *pdesc);

#endif
