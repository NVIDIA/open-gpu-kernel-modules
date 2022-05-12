/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *	inet6 interface/address list definitions
 *	Linux INET6 implementation 
 *
 *	Authors:
 *	Pedro Roque		<roque@di.fc.ul.pt>	
 */

#ifndef _NET_IF_INET6_H
#define _NET_IF_INET6_H

#include <net/snmp.h>
#include <linux/ipv6.h>
#include <linux/refcount.h>

/* inet6_dev.if_flags */

#define IF_RA_OTHERCONF	0x80
#define IF_RA_MANAGED	0x40
#define IF_RA_RCVD	0x20
#define IF_RS_SENT	0x10
#define IF_READY	0x80000000

/* prefix flags */
#define IF_PREFIX_ONLINK	0x01
#define IF_PREFIX_AUTOCONF	0x02

enum {
	INET6_IFADDR_STATE_PREDAD,
	INET6_IFADDR_STATE_DAD,
	INET6_IFADDR_STATE_POSTDAD,
	INET6_IFADDR_STATE_ERRDAD,
	INET6_IFADDR_STATE_DEAD,
};

struct inet6_ifaddr {
	struct in6_addr		addr;
	__u32			prefix_len;
	__u32			rt_priority;

	/* In seconds, relative to tstamp. Expiry is at tstamp + HZ * lft. */
	__u32			valid_lft;
	__u32			prefered_lft;
	refcount_t		refcnt;
	spinlock_t		lock;

	int			state;

	__u32			flags;
	__u8			dad_probes;
	__u8			stable_privacy_retry;

	__u16			scope;
	__u64			dad_nonce;

	unsigned long		cstamp;	/* created timestamp */
	unsigned long		tstamp; /* updated timestamp */

	struct delayed_work	dad_work;

	struct inet6_dev	*idev;
	struct fib6_info	*rt;

	struct hlist_node	addr_lst;
	struct list_head	if_list;

	struct list_head	tmp_list;
	struct inet6_ifaddr	*ifpub;
	int			regen_count;

	bool			tokenized;

	struct rcu_head		rcu;
	struct in6_addr		peer_addr;
};

struct ip6_sf_socklist {
	unsigned int		sl_max;
	unsigned int		sl_count;
	struct rcu_head		rcu;
	struct in6_addr		sl_addr[];
};

#define IP6_SFLSIZE(count)	(sizeof(struct ip6_sf_socklist) + \
	(count) * sizeof(struct in6_addr))

#define IP6_SFBLOCK	10	/* allocate this many at once */

struct ipv6_mc_socklist {
	struct in6_addr		addr;
	int			ifindex;
	unsigned int		sfmode;		/* MCAST_{INCLUDE,EXCLUDE} */
	struct ipv6_mc_socklist __rcu *next;
	struct ip6_sf_socklist	__rcu *sflist;
	struct rcu_head		rcu;
};

struct ip6_sf_list {
	struct ip6_sf_list __rcu *sf_next;
	struct in6_addr		sf_addr;
	unsigned long		sf_count[2];	/* include/exclude counts */
	unsigned char		sf_gsresp;	/* include in g & s response? */
	unsigned char		sf_oldin;	/* change state */
	unsigned char		sf_crcount;	/* retrans. left to send */
	struct rcu_head		rcu;
};

#define MAF_TIMER_RUNNING	0x01
#define MAF_LAST_REPORTER	0x02
#define MAF_LOADED		0x04
#define MAF_NOREPORT		0x08
#define MAF_GSQUERY		0x10

struct ifmcaddr6 {
	struct in6_addr		mca_addr;
	struct inet6_dev	*idev;
	struct ifmcaddr6	__rcu *next;
	struct ip6_sf_list	__rcu *mca_sources;
	struct ip6_sf_list	__rcu *mca_tomb;
	unsigned int		mca_sfmode;
	unsigned char		mca_crcount;
	unsigned long		mca_sfcount[2];
	struct delayed_work	mca_work;
	unsigned int		mca_flags;
	int			mca_users;
	refcount_t		mca_refcnt;
	unsigned long		mca_cstamp;
	unsigned long		mca_tstamp;
	struct rcu_head		rcu;
};

/* Anycast stuff */

struct ipv6_ac_socklist {
	struct in6_addr		acl_addr;
	int			acl_ifindex;
	struct ipv6_ac_socklist *acl_next;
};

struct ifacaddr6 {
	struct in6_addr		aca_addr;
	struct fib6_info	*aca_rt;
	struct ifacaddr6	*aca_next;
	struct hlist_node	aca_addr_lst;
	int			aca_users;
	refcount_t		aca_refcnt;
	unsigned long		aca_cstamp;
	unsigned long		aca_tstamp;
	struct rcu_head		rcu;
};

#define	IFA_HOST	IPV6_ADDR_LOOPBACK
#define	IFA_LINK	IPV6_ADDR_LINKLOCAL
#define	IFA_SITE	IPV6_ADDR_SITELOCAL

struct ipv6_devstat {
	struct proc_dir_entry	*proc_dir_entry;
	DEFINE_SNMP_STAT(struct ipstats_mib, ipv6);
	DEFINE_SNMP_STAT_ATOMIC(struct icmpv6_mib_device, icmpv6dev);
	DEFINE_SNMP_STAT_ATOMIC(struct icmpv6msg_mib_device, icmpv6msgdev);
};

struct inet6_dev {
	struct net_device	*dev;

	struct list_head	addr_list;

	struct ifmcaddr6	__rcu *mc_list;
	struct ifmcaddr6	__rcu *mc_tomb;

	unsigned char		mc_qrv;		/* Query Robustness Variable */
	unsigned char		mc_gq_running;
	unsigned char		mc_ifc_count;
	unsigned char		mc_dad_count;

	unsigned long		mc_v1_seen;	/* Max time we stay in MLDv1 mode */
	unsigned long		mc_qi;		/* Query Interval */
	unsigned long		mc_qri;		/* Query Response Interval */
	unsigned long		mc_maxdelay;

	struct delayed_work	mc_gq_work;	/* general query work */
	struct delayed_work	mc_ifc_work;	/* interface change work */
	struct delayed_work	mc_dad_work;	/* dad complete mc work */
	struct delayed_work	mc_query_work;	/* mld query work */
	struct delayed_work	mc_report_work;	/* mld report work */

	struct sk_buff_head	mc_query_queue;		/* mld query queue */
	struct sk_buff_head	mc_report_queue;	/* mld report queue */

	spinlock_t		mc_query_lock;	/* mld query queue lock */
	spinlock_t		mc_report_lock;	/* mld query report lock */
	struct mutex		mc_lock;	/* mld global lock */

	struct ifacaddr6	*ac_list;
	rwlock_t		lock;
	refcount_t		refcnt;
	__u32			if_flags;
	int			dead;

	u32			desync_factor;
	struct list_head	tempaddr_list;

	struct in6_addr		token;

	struct neigh_parms	*nd_parms;
	struct ipv6_devconf	cnf;
	struct ipv6_devstat	stats;

	struct timer_list	rs_timer;
	__s32			rs_interval;	/* in jiffies */
	__u8			rs_probes;

	unsigned long		tstamp; /* ipv6InterfaceTable update timestamp */
	struct rcu_head		rcu;
};

static inline void ipv6_eth_mc_map(const struct in6_addr *addr, char *buf)
{
	/*
	 *	+-------+-------+-------+-------+-------+-------+
	 *      |   33  |   33  | DST13 | DST14 | DST15 | DST16 |
	 *      +-------+-------+-------+-------+-------+-------+
	 */

	buf[0]= 0x33;
	buf[1]= 0x33;

	memcpy(buf + 2, &addr->s6_addr32[3], sizeof(__u32));
}

static inline void ipv6_arcnet_mc_map(const struct in6_addr *addr, char *buf)
{
	buf[0] = 0x00;
}

static inline void ipv6_ib_mc_map(const struct in6_addr *addr,
				  const unsigned char *broadcast, char *buf)
{
	unsigned char scope = broadcast[5] & 0xF;

	buf[0]  = 0;		/* Reserved */
	buf[1]  = 0xff;		/* Multicast QPN */
	buf[2]  = 0xff;
	buf[3]  = 0xff;
	buf[4]  = 0xff;
	buf[5]  = 0x10 | scope;	/* scope from broadcast address */
	buf[6]  = 0x60;		/* IPv6 signature */
	buf[7]  = 0x1b;
	buf[8]  = broadcast[8];	/* P_Key */
	buf[9]  = broadcast[9];
	memcpy(buf + 10, addr->s6_addr + 6, 10);
}

static inline int ipv6_ipgre_mc_map(const struct in6_addr *addr,
				    const unsigned char *broadcast, char *buf)
{
	if ((broadcast[0] | broadcast[1] | broadcast[2] | broadcast[3]) != 0) {
		memcpy(buf, broadcast, 4);
	} else {
		/* v4mapped? */
		if ((addr->s6_addr32[0] | addr->s6_addr32[1] |
		     (addr->s6_addr32[2] ^ htonl(0x0000ffff))) != 0)
			return -EINVAL;
		memcpy(buf, &addr->s6_addr32[3], 4);
	}
	return 0;
}

#endif
