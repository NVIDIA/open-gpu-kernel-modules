/* SPDX-License-Identifier: (LGPL-2.1 OR BSD-2-Clause) */
#ifndef __BPF_TRACING_NET_H__
#define __BPF_TRACING_NET_H__

#define AF_INET			2
#define AF_INET6		10

#define ICSK_TIME_RETRANS	1
#define ICSK_TIME_PROBE0	3
#define ICSK_TIME_LOSS_PROBE	5
#define ICSK_TIME_REO_TIMEOUT	6

#define IFNAMSIZ		16

#define RTF_GATEWAY		0x0002

#define TCP_INFINITE_SSTHRESH	0x7fffffff
#define TCP_PINGPONG_THRESH	3

#define fib_nh_dev		nh_common.nhc_dev
#define fib_nh_gw_family	nh_common.nhc_gw_family
#define fib_nh_gw6		nh_common.nhc_gw.ipv6

#define inet_daddr		sk.__sk_common.skc_daddr
#define inet_rcv_saddr		sk.__sk_common.skc_rcv_saddr
#define inet_dport		sk.__sk_common.skc_dport

#define ir_loc_addr		req.__req_common.skc_rcv_saddr
#define ir_num			req.__req_common.skc_num
#define ir_rmt_addr		req.__req_common.skc_daddr
#define ir_rmt_port		req.__req_common.skc_dport
#define ir_v6_rmt_addr		req.__req_common.skc_v6_daddr
#define ir_v6_loc_addr		req.__req_common.skc_v6_rcv_saddr

#define sk_family		__sk_common.skc_family
#define sk_rmem_alloc		sk_backlog.rmem_alloc
#define sk_refcnt		__sk_common.skc_refcnt
#define sk_state		__sk_common.skc_state
#define sk_v6_daddr		__sk_common.skc_v6_daddr
#define sk_v6_rcv_saddr		__sk_common.skc_v6_rcv_saddr

#define s6_addr32		in6_u.u6_addr32

#define tw_daddr		__tw_common.skc_daddr
#define tw_rcv_saddr		__tw_common.skc_rcv_saddr
#define tw_dport		__tw_common.skc_dport
#define tw_refcnt		__tw_common.skc_refcnt
#define tw_v6_daddr		__tw_common.skc_v6_daddr
#define tw_v6_rcv_saddr		__tw_common.skc_v6_rcv_saddr

#endif
