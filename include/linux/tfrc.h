/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LINUX_TFRC_H_
#define _LINUX_TFRC_H_
/*
 *  TFRC - Data Structures for the TCP-Friendly Rate Control congestion
 *         control mechanism as specified in RFC 3448.
 *
 *  Copyright (c) 2005 The University of Waikato, Hamilton, New Zealand.
 *  Copyright (c) 2005 Ian McDonald <iam4@cs.waikato.ac.nz>
 *  Copyright (c) 2005 Arnaldo Carvalho de Melo <acme@conectiva.com.br>
 *  Copyright (c) 2003 Nils-Erik Mattsson, Joacim Haggmark, Magnus Erixzon
 */
#include <linux/types.h>

/** 	tfrc_rx_info    -    TFRC Receiver Data Structure
 *
 * 	@tfrcrx_x_recv:	receiver estimate of sending rate (3.2.2)
 * 	@tfrcrx_rtt:	round-trip-time (communicated by sender)
 * 	@tfrcrx_p:	current estimate of loss event rate (3.2.2)
 */
struct tfrc_rx_info {
  	__u32 tfrcrx_x_recv;
	__u32 tfrcrx_rtt;
  	__u32 tfrcrx_p;
};

/** 	tfrc_tx_info    -    TFRC Sender Data Structure
 *
 * 	@tfrctx_x:	computed transmit rate (4.3 (4))
 * 	@tfrctx_x_recv: receiver estimate of send rate (4.3)
 * 	@tfrctx_x_calc:	return value of throughput equation (3.1)
 * 	@tfrctx_rtt:	(moving average) estimate of RTT (4.3)
 * 	@tfrctx_p:	current loss event rate (5.4)
 * 	@tfrctx_rto:	estimate of RTO, equals 4*RTT (4.3)
 * 	@tfrctx_ipi:	inter-packet interval (4.6)
 *
 *  Note: X and X_recv are both maintained in units of 64 * bytes/second. This
 *        enables a finer resolution of sending rates and avoids problems with
 *        integer arithmetic; u32 is not sufficient as scaling consumes 6 bits.
 */
struct tfrc_tx_info {
	__u64 tfrctx_x;
	__u64 tfrctx_x_recv;
	__u32 tfrctx_x_calc;
	__u32 tfrctx_rtt;
	__u32 tfrctx_p;
	__u32 tfrctx_rto;
	__u32 tfrctx_ipi;
};

#endif /* _LINUX_TFRC_H_ */
