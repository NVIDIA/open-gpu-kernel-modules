/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _NET_SECURE_SEQ
#define _NET_SECURE_SEQ

#include <linux/types.h>

u32 secure_ipv4_port_ephemeral(__be32 saddr, __be32 daddr, __be16 dport);
u32 secure_ipv6_port_ephemeral(const __be32 *saddr, const __be32 *daddr,
			       __be16 dport);
u32 secure_tcp_seq(__be32 saddr, __be32 daddr,
		   __be16 sport, __be16 dport);
u32 secure_tcp_ts_off(const struct net *net, __be32 saddr, __be32 daddr);
u32 secure_tcpv6_seq(const __be32 *saddr, const __be32 *daddr,
		     __be16 sport, __be16 dport);
u32 secure_tcpv6_ts_off(const struct net *net,
			const __be32 *saddr, const __be32 *daddr);
u64 secure_dccp_sequence_number(__be32 saddr, __be32 daddr,
				__be16 sport, __be16 dport);
u64 secure_dccpv6_sequence_number(__be32 *saddr, __be32 *daddr,
				  __be16 sport, __be16 dport);

#endif /* _NET_SECURE_SEQ */
