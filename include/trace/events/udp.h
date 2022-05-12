/* SPDX-License-Identifier: GPL-2.0 */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM udp

#if !defined(_TRACE_UDP_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_UDP_H

#include <linux/udp.h>
#include <linux/tracepoint.h>

TRACE_EVENT(udp_fail_queue_rcv_skb,

	TP_PROTO(int rc, struct sock *sk),

	TP_ARGS(rc, sk),

	TP_STRUCT__entry(
		__field(int, rc)
		__field(__u16, lport)
	),

	TP_fast_assign(
		__entry->rc = rc;
		__entry->lport = inet_sk(sk)->inet_num;
	),

	TP_printk("rc=%d port=%hu", __entry->rc, __entry->lport)
);

#endif /* _TRACE_UDP_H */

/* This part must be outside protection */
#include <trace/define_trace.h>
