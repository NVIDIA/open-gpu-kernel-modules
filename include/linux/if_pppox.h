/* SPDX-License-Identifier: GPL-2.0-or-later */
/***************************************************************************
 * Linux PPP over X - Generic PPP transport layer sockets
 * Linux PPP over Ethernet (PPPoE) Socket Implementation (RFC 2516) 
 *
 * This file supplies definitions required by the PPP over Ethernet driver
 * (pppox.c).  All version information wrt this file is located in pppox.c
 *
 * License:
 */
#ifndef __LINUX_IF_PPPOX_H
#define __LINUX_IF_PPPOX_H

#include <linux/if.h>
#include <linux/netdevice.h>
#include <linux/ppp_channel.h>
#include <linux/skbuff.h>
#include <linux/workqueue.h>
#include <uapi/linux/if_pppox.h>

static inline struct pppoe_hdr *pppoe_hdr(const struct sk_buff *skb)
{
	return (struct pppoe_hdr *)skb_network_header(skb);
}

struct pppoe_opt {
	struct net_device      *dev;	  /* device associated with socket*/
	int			ifindex;  /* ifindex of device associated with socket */
	struct pppoe_addr	pa;	  /* what this socket is bound to*/
	struct sockaddr_pppox	relay;	  /* what socket data will be
					     relayed to (PPPoE relaying) */
	struct work_struct      padt_work;/* Work item for handling PADT */
};

struct pptp_opt {
	struct pptp_addr src_addr;
	struct pptp_addr dst_addr;
	u32 ack_sent, ack_recv;
	u32 seq_sent, seq_recv;
	int ppp_flags;
};
#include <net/sock.h>

struct pppox_sock {
	/* struct sock must be the first member of pppox_sock */
	struct sock sk;
	struct ppp_channel chan;
	struct pppox_sock	*next;	  /* for hash table */
	union {
		struct pppoe_opt pppoe;
		struct pptp_opt  pptp;
	} proto;
	__be16			num;
};
#define pppoe_dev	proto.pppoe.dev
#define pppoe_ifindex	proto.pppoe.ifindex
#define pppoe_pa	proto.pppoe.pa
#define pppoe_relay	proto.pppoe.relay

static inline struct pppox_sock *pppox_sk(struct sock *sk)
{
	return (struct pppox_sock *)sk;
}

static inline struct sock *sk_pppox(struct pppox_sock *po)
{
	return (struct sock *)po;
}

struct module;

struct pppox_proto {
	int		(*create)(struct net *net, struct socket *sock, int kern);
	int		(*ioctl)(struct socket *sock, unsigned int cmd,
				 unsigned long arg);
	struct module	*owner;
};

extern int register_pppox_proto(int proto_num, const struct pppox_proto *pp);
extern void unregister_pppox_proto(int proto_num);
extern void pppox_unbind_sock(struct sock *sk);/* delete ppp-channel binding */
extern int pppox_ioctl(struct socket *sock, unsigned int cmd, unsigned long arg);
extern int pppox_compat_ioctl(struct socket *sock, unsigned int cmd, unsigned long arg);

#define PPPOEIOCSFWD32    _IOW(0xB1 ,0, compat_size_t)

/* PPPoX socket states */
enum {
    PPPOX_NONE		= 0,  /* initial state */
    PPPOX_CONNECTED	= 1,  /* connection established ==TCP_ESTABLISHED */
    PPPOX_BOUND		= 2,  /* bound to ppp device */
    PPPOX_RELAY		= 4,  /* forwarding is enabled */
    PPPOX_DEAD		= 16  /* dead, useless, please clean me up!*/
};

#endif /* !(__LINUX_IF_PPPOX_H) */
