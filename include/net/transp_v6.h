/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _TRANSP_V6_H
#define _TRANSP_V6_H

#include <net/checksum.h>

/* IPv6 transport protocols */
extern struct proto rawv6_prot;
extern struct proto udpv6_prot;
extern struct proto udplitev6_prot;
extern struct proto tcpv6_prot;
extern struct proto pingv6_prot;

struct flowi6;

/* extension headers */
int ipv6_exthdrs_init(void);
void ipv6_exthdrs_exit(void);
int ipv6_frag_init(void);
void ipv6_frag_exit(void);

/* transport protocols */
int pingv6_init(void);
void pingv6_exit(void);
int rawv6_init(void);
void rawv6_exit(void);
int udpv6_init(void);
void udpv6_exit(void);
int udplitev6_init(void);
void udplitev6_exit(void);
int tcpv6_init(void);
void tcpv6_exit(void);

int udpv6_connect(struct sock *sk, struct sockaddr *uaddr, int addr_len);

/* this does all the common and the specific ctl work */
void ip6_datagram_recv_ctl(struct sock *sk, struct msghdr *msg,
			   struct sk_buff *skb);
void ip6_datagram_recv_common_ctl(struct sock *sk, struct msghdr *msg,
				  struct sk_buff *skb);
void ip6_datagram_recv_specific_ctl(struct sock *sk, struct msghdr *msg,
				    struct sk_buff *skb);

int ip6_datagram_send_ctl(struct net *net, struct sock *sk, struct msghdr *msg,
			  struct flowi6 *fl6, struct ipcm6_cookie *ipc6);

void __ip6_dgram_sock_seq_show(struct seq_file *seq, struct sock *sp,
			       __u16 srcp, __u16 destp, int rqueue, int bucket);
static inline void
ip6_dgram_sock_seq_show(struct seq_file *seq, struct sock *sp, __u16 srcp,
			__u16 destp, int bucket)
{
	__ip6_dgram_sock_seq_show(seq, sp, srcp, destp, sk_rmem_alloc_get(sp),
				  bucket);
}

#define LOOPBACK4_IPV6 cpu_to_be32(0x7f000006)

void inet6_destroy_sock(struct sock *sk);

#define IPV6_SEQ_DGRAM_HEADER					       \
	"  sl  "						       \
	"local_address                         "		       \
	"remote_address                        "		       \
	"st tx_queue rx_queue tr tm->when retrnsmt"		       \
	"   uid  timeout inode ref pointer drops\n"

#endif
