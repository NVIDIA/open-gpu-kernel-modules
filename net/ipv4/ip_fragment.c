// SPDX-License-Identifier: GPL-2.0
/*
 * INET		An implementation of the TCP/IP protocol suite for the LINUX
 *		operating system.  INET is implemented using the  BSD Socket
 *		interface as the means of communication with the user level.
 *
 *		The IP fragmentation functionality.
 *
 * Authors:	Fred N. van Kempen <waltje@uWalt.NL.Mugnet.ORG>
 *		Alan Cox <alan@lxorguk.ukuu.org.uk>
 *
 * Fixes:
 *		Alan Cox	:	Split from ip.c , see ip_input.c for history.
 *		David S. Miller :	Begin massive cleanup...
 *		Andi Kleen	:	Add sysctls.
 *		xxxx		:	Overlapfrag bug.
 *		Ultima          :       ip_expire() kernel panic.
 *		Bill Hawes	:	Frag accounting and evictor fixes.
 *		John McDonald	:	0 length frag bug.
 *		Alexey Kuznetsov:	SMP races, threading, cleanup.
 *		Patrick McHardy :	LRU queue of frag heads for evictor.
 */

#define pr_fmt(fmt) "IPv4: " fmt

#include <linux/compiler.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/mm.h>
#include <linux/jiffies.h>
#include <linux/skbuff.h>
#include <linux/list.h>
#include <linux/ip.h>
#include <linux/icmp.h>
#include <linux/netdevice.h>
#include <linux/jhash.h>
#include <linux/random.h>
#include <linux/slab.h>
#include <net/route.h>
#include <net/dst.h>
#include <net/sock.h>
#include <net/ip.h>
#include <net/icmp.h>
#include <net/checksum.h>
#include <net/inetpeer.h>
#include <net/inet_frag.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/inet.h>
#include <linux/netfilter_ipv4.h>
#include <net/inet_ecn.h>
#include <net/l3mdev.h>

/* NOTE. Logic of IP defragmentation is parallel to corresponding IPv6
 * code now. If you change something here, _PLEASE_ update ipv6/reassembly.c
 * as well. Or notify me, at least. --ANK
 */
static const char ip_frag_cache_name[] = "ip4-frags";

/* Describe an entry in the "incomplete datagrams" queue. */
struct ipq {
	struct inet_frag_queue q;

	u8		ecn; /* RFC3168 support */
	u16		max_df_size; /* largest frag with DF set seen */
	int             iif;
	unsigned int    rid;
	struct inet_peer *peer;
};

static u8 ip4_frag_ecn(u8 tos)
{
	return 1 << (tos & INET_ECN_MASK);
}

static struct inet_frags ip4_frags;

static int ip_frag_reasm(struct ipq *qp, struct sk_buff *skb,
			 struct sk_buff *prev_tail, struct net_device *dev);


static void ip4_frag_init(struct inet_frag_queue *q, const void *a)
{
	struct ipq *qp = container_of(q, struct ipq, q);
	struct net *net = q->fqdir->net;

	const struct frag_v4_compare_key *key = a;

	q->key.v4 = *key;
	qp->ecn = 0;
	qp->peer = q->fqdir->max_dist ?
		inet_getpeer_v4(net->ipv4.peers, key->saddr, key->vif, 1) :
		NULL;
}

static void ip4_frag_free(struct inet_frag_queue *q)
{
	struct ipq *qp;

	qp = container_of(q, struct ipq, q);
	if (qp->peer)
		inet_putpeer(qp->peer);
}


/* Destruction primitives. */

static void ipq_put(struct ipq *ipq)
{
	inet_frag_put(&ipq->q);
}

/* Kill ipq entry. It is not destroyed immediately,
 * because caller (and someone more) holds reference count.
 */
static void ipq_kill(struct ipq *ipq)
{
	inet_frag_kill(&ipq->q);
}

static bool frag_expire_skip_icmp(u32 user)
{
	return user == IP_DEFRAG_AF_PACKET ||
	       ip_defrag_user_in_between(user, IP_DEFRAG_CONNTRACK_IN,
					 __IP_DEFRAG_CONNTRACK_IN_END) ||
	       ip_defrag_user_in_between(user, IP_DEFRAG_CONNTRACK_BRIDGE_IN,
					 __IP_DEFRAG_CONNTRACK_BRIDGE_IN);
}

/*
 * Oops, a fragment queue timed out.  Kill it and send an ICMP reply.
 */
static void ip_expire(struct timer_list *t)
{
	struct inet_frag_queue *frag = from_timer(frag, t, timer);
	const struct iphdr *iph;
	struct sk_buff *head = NULL;
	struct net *net;
	struct ipq *qp;
	int err;

	qp = container_of(frag, struct ipq, q);
	net = qp->q.fqdir->net;

	rcu_read_lock();

	if (qp->q.fqdir->dead)
		goto out_rcu_unlock;

	spin_lock(&qp->q.lock);

	if (qp->q.flags & INET_FRAG_COMPLETE)
		goto out;

	ipq_kill(qp);
	__IP_INC_STATS(net, IPSTATS_MIB_REASMFAILS);
	__IP_INC_STATS(net, IPSTATS_MIB_REASMTIMEOUT);

	if (!(qp->q.flags & INET_FRAG_FIRST_IN))
		goto out;

	/* sk_buff::dev and sk_buff::rbnode are unionized. So we
	 * pull the head out of the tree in order to be able to
	 * deal with head->dev.
	 */
	head = inet_frag_pull_head(&qp->q);
	if (!head)
		goto out;
	head->dev = dev_get_by_index_rcu(net, qp->iif);
	if (!head->dev)
		goto out;


	/* skb has no dst, perform route lookup again */
	iph = ip_hdr(head);
	err = ip_route_input_noref(head, iph->daddr, iph->saddr,
					   iph->tos, head->dev);
	if (err)
		goto out;

	/* Only an end host needs to send an ICMP
	 * "Fragment Reassembly Timeout" message, per RFC792.
	 */
	if (frag_expire_skip_icmp(qp->q.key.v4.user) &&
	    (skb_rtable(head)->rt_type != RTN_LOCAL))
		goto out;

	spin_unlock(&qp->q.lock);
	icmp_send(head, ICMP_TIME_EXCEEDED, ICMP_EXC_FRAGTIME, 0);
	goto out_rcu_unlock;

out:
	spin_unlock(&qp->q.lock);
out_rcu_unlock:
	rcu_read_unlock();
	kfree_skb(head);
	ipq_put(qp);
}

/* Find the correct entry in the "incomplete datagrams" queue for
 * this IP datagram, and create new one, if nothing is found.
 */
static struct ipq *ip_find(struct net *net, struct iphdr *iph,
			   u32 user, int vif)
{
	struct frag_v4_compare_key key = {
		.saddr = iph->saddr,
		.daddr = iph->daddr,
		.user = user,
		.vif = vif,
		.id = iph->id,
		.protocol = iph->protocol,
	};
	struct inet_frag_queue *q;

	q = inet_frag_find(net->ipv4.fqdir, &key);
	if (!q)
		return NULL;

	return container_of(q, struct ipq, q);
}

/* Is the fragment too far ahead to be part of ipq? */
static int ip_frag_too_far(struct ipq *qp)
{
	struct inet_peer *peer = qp->peer;
	unsigned int max = qp->q.fqdir->max_dist;
	unsigned int start, end;

	int rc;

	if (!peer || !max)
		return 0;

	start = qp->rid;
	end = atomic_inc_return(&peer->rid);
	qp->rid = end;

	rc = qp->q.fragments_tail && (end - start) > max;

	if (rc)
		__IP_INC_STATS(qp->q.fqdir->net, IPSTATS_MIB_REASMFAILS);

	return rc;
}

static int ip_frag_reinit(struct ipq *qp)
{
	unsigned int sum_truesize = 0;

	if (!mod_timer(&qp->q.timer, jiffies + qp->q.fqdir->timeout)) {
		refcount_inc(&qp->q.refcnt);
		return -ETIMEDOUT;
	}

	sum_truesize = inet_frag_rbtree_purge(&qp->q.rb_fragments);
	sub_frag_mem_limit(qp->q.fqdir, sum_truesize);

	qp->q.flags = 0;
	qp->q.len = 0;
	qp->q.meat = 0;
	qp->q.rb_fragments = RB_ROOT;
	qp->q.fragments_tail = NULL;
	qp->q.last_run_head = NULL;
	qp->iif = 0;
	qp->ecn = 0;

	return 0;
}

/* Add new segment to existing queue. */
static int ip_frag_queue(struct ipq *qp, struct sk_buff *skb)
{
	struct net *net = qp->q.fqdir->net;
	int ihl, end, flags, offset;
	struct sk_buff *prev_tail;
	struct net_device *dev;
	unsigned int fragsize;
	int err = -ENOENT;
	u8 ecn;

	if (qp->q.flags & INET_FRAG_COMPLETE)
		goto err;

	if (!(IPCB(skb)->flags & IPSKB_FRAG_COMPLETE) &&
	    unlikely(ip_frag_too_far(qp)) &&
	    unlikely(err = ip_frag_reinit(qp))) {
		ipq_kill(qp);
		goto err;
	}

	ecn = ip4_frag_ecn(ip_hdr(skb)->tos);
	offset = ntohs(ip_hdr(skb)->frag_off);
	flags = offset & ~IP_OFFSET;
	offset &= IP_OFFSET;
	offset <<= 3;		/* offset is in 8-byte chunks */
	ihl = ip_hdrlen(skb);

	/* Determine the position of this fragment. */
	end = offset + skb->len - skb_network_offset(skb) - ihl;
	err = -EINVAL;

	/* Is this the final fragment? */
	if ((flags & IP_MF) == 0) {
		/* If we already have some bits beyond end
		 * or have different end, the segment is corrupted.
		 */
		if (end < qp->q.len ||
		    ((qp->q.flags & INET_FRAG_LAST_IN) && end != qp->q.len))
			goto discard_qp;
		qp->q.flags |= INET_FRAG_LAST_IN;
		qp->q.len = end;
	} else {
		if (end&7) {
			end &= ~7;
			if (skb->ip_summed != CHECKSUM_UNNECESSARY)
				skb->ip_summed = CHECKSUM_NONE;
		}
		if (end > qp->q.len) {
			/* Some bits beyond end -> corruption. */
			if (qp->q.flags & INET_FRAG_LAST_IN)
				goto discard_qp;
			qp->q.len = end;
		}
	}
	if (end == offset)
		goto discard_qp;

	err = -ENOMEM;
	if (!pskb_pull(skb, skb_network_offset(skb) + ihl))
		goto discard_qp;

	err = pskb_trim_rcsum(skb, end - offset);
	if (err)
		goto discard_qp;

	/* Note : skb->rbnode and skb->dev share the same location. */
	dev = skb->dev;
	/* Makes sure compiler wont do silly aliasing games */
	barrier();

	prev_tail = qp->q.fragments_tail;
	err = inet_frag_queue_insert(&qp->q, skb, offset, end);
	if (err)
		goto insert_error;

	if (dev)
		qp->iif = dev->ifindex;

	qp->q.stamp = skb->tstamp;
	qp->q.meat += skb->len;
	qp->ecn |= ecn;
	add_frag_mem_limit(qp->q.fqdir, skb->truesize);
	if (offset == 0)
		qp->q.flags |= INET_FRAG_FIRST_IN;

	fragsize = skb->len + ihl;

	if (fragsize > qp->q.max_size)
		qp->q.max_size = fragsize;

	if (ip_hdr(skb)->frag_off & htons(IP_DF) &&
	    fragsize > qp->max_df_size)
		qp->max_df_size = fragsize;

	if (qp->q.flags == (INET_FRAG_FIRST_IN | INET_FRAG_LAST_IN) &&
	    qp->q.meat == qp->q.len) {
		unsigned long orefdst = skb->_skb_refdst;

		skb->_skb_refdst = 0UL;
		err = ip_frag_reasm(qp, skb, prev_tail, dev);
		skb->_skb_refdst = orefdst;
		if (err)
			inet_frag_kill(&qp->q);
		return err;
	}

	skb_dst_drop(skb);
	return -EINPROGRESS;

insert_error:
	if (err == IPFRAG_DUP) {
		kfree_skb(skb);
		return -EINVAL;
	}
	err = -EINVAL;
	__IP_INC_STATS(net, IPSTATS_MIB_REASM_OVERLAPS);
discard_qp:
	inet_frag_kill(&qp->q);
	__IP_INC_STATS(net, IPSTATS_MIB_REASMFAILS);
err:
	kfree_skb(skb);
	return err;
}

static bool ip_frag_coalesce_ok(const struct ipq *qp)
{
	return qp->q.key.v4.user == IP_DEFRAG_LOCAL_DELIVER;
}

/* Build a new IP datagram from all its fragments. */
static int ip_frag_reasm(struct ipq *qp, struct sk_buff *skb,
			 struct sk_buff *prev_tail, struct net_device *dev)
{
	struct net *net = qp->q.fqdir->net;
	struct iphdr *iph;
	void *reasm_data;
	int len, err;
	u8 ecn;

	ipq_kill(qp);

	ecn = ip_frag_ecn_table[qp->ecn];
	if (unlikely(ecn == 0xff)) {
		err = -EINVAL;
		goto out_fail;
	}

	/* Make the one we just received the head. */
	reasm_data = inet_frag_reasm_prepare(&qp->q, skb, prev_tail);
	if (!reasm_data)
		goto out_nomem;

	len = ip_hdrlen(skb) + qp->q.len;
	err = -E2BIG;
	if (len > 65535)
		goto out_oversize;

	inet_frag_reasm_finish(&qp->q, skb, reasm_data,
			       ip_frag_coalesce_ok(qp));

	skb->dev = dev;
	IPCB(skb)->frag_max_size = max(qp->max_df_size, qp->q.max_size);

	iph = ip_hdr(skb);
	iph->tot_len = htons(len);
	iph->tos |= ecn;

	/* When we set IP_DF on a refragmented skb we must also force a
	 * call to ip_fragment to avoid forwarding a DF-skb of size s while
	 * original sender only sent fragments of size f (where f < s).
	 *
	 * We only set DF/IPSKB_FRAG_PMTU if such DF fragment was the largest
	 * frag seen to avoid sending tiny DF-fragments in case skb was built
	 * from one very small df-fragment and one large non-df frag.
	 */
	if (qp->max_df_size == qp->q.max_size) {
		IPCB(skb)->flags |= IPSKB_FRAG_PMTU;
		iph->frag_off = htons(IP_DF);
	} else {
		iph->frag_off = 0;
	}

	ip_send_check(iph);

	__IP_INC_STATS(net, IPSTATS_MIB_REASMOKS);
	qp->q.rb_fragments = RB_ROOT;
	qp->q.fragments_tail = NULL;
	qp->q.last_run_head = NULL;
	return 0;

out_nomem:
	net_dbg_ratelimited("queue_glue: no memory for gluing queue %p\n", qp);
	err = -ENOMEM;
	goto out_fail;
out_oversize:
	net_info_ratelimited("Oversized IP packet from %pI4\n", &qp->q.key.v4.saddr);
out_fail:
	__IP_INC_STATS(net, IPSTATS_MIB_REASMFAILS);
	return err;
}

/* Process an incoming IP datagram fragment. */
int ip_defrag(struct net *net, struct sk_buff *skb, u32 user)
{
	struct net_device *dev = skb->dev ? : skb_dst(skb)->dev;
	int vif = l3mdev_master_ifindex_rcu(dev);
	struct ipq *qp;

	__IP_INC_STATS(net, IPSTATS_MIB_REASMREQDS);
	skb_orphan(skb);

	/* Lookup (or create) queue header */
	qp = ip_find(net, ip_hdr(skb), user, vif);
	if (qp) {
		int ret;

		spin_lock(&qp->q.lock);

		ret = ip_frag_queue(qp, skb);

		spin_unlock(&qp->q.lock);
		ipq_put(qp);
		return ret;
	}

	__IP_INC_STATS(net, IPSTATS_MIB_REASMFAILS);
	kfree_skb(skb);
	return -ENOMEM;
}
EXPORT_SYMBOL(ip_defrag);

struct sk_buff *ip_check_defrag(struct net *net, struct sk_buff *skb, u32 user)
{
	struct iphdr iph;
	int netoff;
	u32 len;

	if (skb->protocol != htons(ETH_P_IP))
		return skb;

	netoff = skb_network_offset(skb);

	if (skb_copy_bits(skb, netoff, &iph, sizeof(iph)) < 0)
		return skb;

	if (iph.ihl < 5 || iph.version != 4)
		return skb;

	len = ntohs(iph.tot_len);
	if (skb->len < netoff + len || len < (iph.ihl * 4))
		return skb;

	if (ip_is_fragment(&iph)) {
		skb = skb_share_check(skb, GFP_ATOMIC);
		if (skb) {
			if (!pskb_may_pull(skb, netoff + iph.ihl * 4)) {
				kfree_skb(skb);
				return NULL;
			}
			if (pskb_trim_rcsum(skb, netoff + len)) {
				kfree_skb(skb);
				return NULL;
			}
			memset(IPCB(skb), 0, sizeof(struct inet_skb_parm));
			if (ip_defrag(net, skb, user))
				return NULL;
			skb_clear_hash(skb);
		}
	}
	return skb;
}
EXPORT_SYMBOL(ip_check_defrag);

#ifdef CONFIG_SYSCTL
static int dist_min;

static struct ctl_table ip4_frags_ns_ctl_table[] = {
	{
		.procname	= "ipfrag_high_thresh",
		.maxlen		= sizeof(unsigned long),
		.mode		= 0644,
		.proc_handler	= proc_doulongvec_minmax,
	},
	{
		.procname	= "ipfrag_low_thresh",
		.maxlen		= sizeof(unsigned long),
		.mode		= 0644,
		.proc_handler	= proc_doulongvec_minmax,
	},
	{
		.procname	= "ipfrag_time",
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec_jiffies,
	},
	{
		.procname	= "ipfrag_max_dist",
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec_minmax,
		.extra1		= &dist_min,
	},
	{ }
};

/* secret interval has been deprecated */
static int ip4_frags_secret_interval_unused;
static struct ctl_table ip4_frags_ctl_table[] = {
	{
		.procname	= "ipfrag_secret_interval",
		.data		= &ip4_frags_secret_interval_unused,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec_jiffies,
	},
	{ }
};

static int __net_init ip4_frags_ns_ctl_register(struct net *net)
{
	struct ctl_table *table;
	struct ctl_table_header *hdr;

	table = ip4_frags_ns_ctl_table;
	if (!net_eq(net, &init_net)) {
		table = kmemdup(table, sizeof(ip4_frags_ns_ctl_table), GFP_KERNEL);
		if (!table)
			goto err_alloc;

	}
	table[0].data	= &net->ipv4.fqdir->high_thresh;
	table[0].extra1	= &net->ipv4.fqdir->low_thresh;
	table[1].data	= &net->ipv4.fqdir->low_thresh;
	table[1].extra2	= &net->ipv4.fqdir->high_thresh;
	table[2].data	= &net->ipv4.fqdir->timeout;
	table[3].data	= &net->ipv4.fqdir->max_dist;

	hdr = register_net_sysctl(net, "net/ipv4", table);
	if (!hdr)
		goto err_reg;

	net->ipv4.frags_hdr = hdr;
	return 0;

err_reg:
	if (!net_eq(net, &init_net))
		kfree(table);
err_alloc:
	return -ENOMEM;
}

static void __net_exit ip4_frags_ns_ctl_unregister(struct net *net)
{
	struct ctl_table *table;

	table = net->ipv4.frags_hdr->ctl_table_arg;
	unregister_net_sysctl_table(net->ipv4.frags_hdr);
	kfree(table);
}

static void __init ip4_frags_ctl_register(void)
{
	register_net_sysctl(&init_net, "net/ipv4", ip4_frags_ctl_table);
}
#else
static int ip4_frags_ns_ctl_register(struct net *net)
{
	return 0;
}

static void ip4_frags_ns_ctl_unregister(struct net *net)
{
}

static void __init ip4_frags_ctl_register(void)
{
}
#endif

static int __net_init ipv4_frags_init_net(struct net *net)
{
	int res;

	res = fqdir_init(&net->ipv4.fqdir, &ip4_frags, net);
	if (res < 0)
		return res;
	/* Fragment cache limits.
	 *
	 * The fragment memory accounting code, (tries to) account for
	 * the real memory usage, by measuring both the size of frag
	 * queue struct (inet_frag_queue (ipv4:ipq/ipv6:frag_queue))
	 * and the SKB's truesize.
	 *
	 * A 64K fragment consumes 129736 bytes (44*2944)+200
	 * (1500 truesize == 2944, sizeof(struct ipq) == 200)
	 *
	 * We will commit 4MB at one time. Should we cross that limit
	 * we will prune down to 3MB, making room for approx 8 big 64K
	 * fragments 8x128k.
	 */
	net->ipv4.fqdir->high_thresh = 4 * 1024 * 1024;
	net->ipv4.fqdir->low_thresh  = 3 * 1024 * 1024;
	/*
	 * Important NOTE! Fragment queue must be destroyed before MSL expires.
	 * RFC791 is wrong proposing to prolongate timer each fragment arrival
	 * by TTL.
	 */
	net->ipv4.fqdir->timeout = IP_FRAG_TIME;

	net->ipv4.fqdir->max_dist = 64;

	res = ip4_frags_ns_ctl_register(net);
	if (res < 0)
		fqdir_exit(net->ipv4.fqdir);
	return res;
}

static void __net_exit ipv4_frags_pre_exit_net(struct net *net)
{
	fqdir_pre_exit(net->ipv4.fqdir);
}

static void __net_exit ipv4_frags_exit_net(struct net *net)
{
	ip4_frags_ns_ctl_unregister(net);
	fqdir_exit(net->ipv4.fqdir);
}

static struct pernet_operations ip4_frags_ops = {
	.init		= ipv4_frags_init_net,
	.pre_exit	= ipv4_frags_pre_exit_net,
	.exit		= ipv4_frags_exit_net,
};


static u32 ip4_key_hashfn(const void *data, u32 len, u32 seed)
{
	return jhash2(data,
		      sizeof(struct frag_v4_compare_key) / sizeof(u32), seed);
}

static u32 ip4_obj_hashfn(const void *data, u32 len, u32 seed)
{
	const struct inet_frag_queue *fq = data;

	return jhash2((const u32 *)&fq->key.v4,
		      sizeof(struct frag_v4_compare_key) / sizeof(u32), seed);
}

static int ip4_obj_cmpfn(struct rhashtable_compare_arg *arg, const void *ptr)
{
	const struct frag_v4_compare_key *key = arg->key;
	const struct inet_frag_queue *fq = ptr;

	return !!memcmp(&fq->key, key, sizeof(*key));
}

static const struct rhashtable_params ip4_rhash_params = {
	.head_offset		= offsetof(struct inet_frag_queue, node),
	.key_offset		= offsetof(struct inet_frag_queue, key),
	.key_len		= sizeof(struct frag_v4_compare_key),
	.hashfn			= ip4_key_hashfn,
	.obj_hashfn		= ip4_obj_hashfn,
	.obj_cmpfn		= ip4_obj_cmpfn,
	.automatic_shrinking	= true,
};

void __init ipfrag_init(void)
{
	ip4_frags.constructor = ip4_frag_init;
	ip4_frags.destructor = ip4_frag_free;
	ip4_frags.qsize = sizeof(struct ipq);
	ip4_frags.frag_expire = ip_expire;
	ip4_frags.frags_cache_name = ip_frag_cache_name;
	ip4_frags.rhash_params = ip4_rhash_params;
	if (inet_frags_init(&ip4_frags))
		panic("IP: failed to allocate ip4_frags cache\n");
	ip4_frags_ctl_register();
	register_pernet_subsys(&ip4_frags_ops);
}
