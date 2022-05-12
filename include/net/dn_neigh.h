/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _NET_DN_NEIGH_H
#define _NET_DN_NEIGH_H

/*
 * The position of the first two fields of
 * this structure are critical - SJW
 */
struct dn_neigh {
        struct neighbour n;
	__le16 addr;
        unsigned long flags;
#define DN_NDFLAG_R1    0x0001 /* Router L1      */
#define DN_NDFLAG_R2    0x0002 /* Router L2      */
#define DN_NDFLAG_P3    0x0004 /* Phase III Node */
        unsigned long blksize;
	__u8 priority;
};

void dn_neigh_init(void);
void dn_neigh_cleanup(void);
int dn_neigh_router_hello(struct net *net, struct sock *sk, struct sk_buff *skb);
int dn_neigh_endnode_hello(struct net *net, struct sock *sk, struct sk_buff *skb);
void dn_neigh_pointopoint_hello(struct sk_buff *skb);
int dn_neigh_elist(struct net_device *dev, unsigned char *ptr, int n);
int dn_to_neigh_output(struct net *net, struct sock *sk, struct sk_buff *skb);

extern struct neigh_table dn_neigh_table;

#endif /* _NET_DN_NEIGH_H */
