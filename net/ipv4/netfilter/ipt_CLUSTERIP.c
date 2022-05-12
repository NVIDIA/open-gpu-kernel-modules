// SPDX-License-Identifier: GPL-2.0-only
/* Cluster IP hashmark target
 * (C) 2003-2004 by Harald Welte <laforge@netfilter.org>
 * based on ideas of Fabio Olive Leite <olive@unixforge.org>
 *
 * Development of this code funded by SuSE Linux AG, https://www.suse.com/
 */
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/jhash.h>
#include <linux/bitops.h>
#include <linux/skbuff.h>
#include <linux/slab.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/icmp.h>
#include <linux/if_arp.h>
#include <linux/seq_file.h>
#include <linux/refcount.h>
#include <linux/netfilter_arp.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netfilter_ipv4/ipt_CLUSTERIP.h>
#include <net/netfilter/nf_conntrack.h>
#include <net/net_namespace.h>
#include <net/netns/generic.h>
#include <net/checksum.h>
#include <net/ip.h>

#define CLUSTERIP_VERSION "0.8"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Harald Welte <laforge@netfilter.org>");
MODULE_DESCRIPTION("Xtables: CLUSTERIP target");

struct clusterip_config {
	struct list_head list;			/* list of all configs */
	refcount_t refcount;			/* reference count */
	refcount_t entries;			/* number of entries/rules
						 * referencing us */

	__be32 clusterip;			/* the IP address */
	u_int8_t clustermac[ETH_ALEN];		/* the MAC address */
	int ifindex;				/* device ifindex */
	u_int16_t num_total_nodes;		/* total number of nodes */
	unsigned long local_nodes;		/* node number array */

#ifdef CONFIG_PROC_FS
	struct proc_dir_entry *pde;		/* proc dir entry */
#endif
	enum clusterip_hashmode hash_mode;	/* which hashing mode */
	u_int32_t hash_initval;			/* hash initialization */
	struct rcu_head rcu;			/* for call_rcu */
	struct net *net;			/* netns for pernet list */
	char ifname[IFNAMSIZ];			/* device ifname */
};

#ifdef CONFIG_PROC_FS
static const struct proc_ops clusterip_proc_ops;
#endif

struct clusterip_net {
	struct list_head configs;
	/* lock protects the configs list */
	spinlock_t lock;

#ifdef CONFIG_PROC_FS
	struct proc_dir_entry *procdir;
	/* mutex protects the config->pde*/
	struct mutex mutex;
#endif
};

static unsigned int clusterip_net_id __read_mostly;
static inline struct clusterip_net *clusterip_pernet(struct net *net)
{
	return net_generic(net, clusterip_net_id);
}

static inline void
clusterip_config_get(struct clusterip_config *c)
{
	refcount_inc(&c->refcount);
}

static void clusterip_config_rcu_free(struct rcu_head *head)
{
	struct clusterip_config *config;
	struct net_device *dev;

	config = container_of(head, struct clusterip_config, rcu);
	dev = dev_get_by_name(config->net, config->ifname);
	if (dev) {
		dev_mc_del(dev, config->clustermac);
		dev_put(dev);
	}
	kfree(config);
}

static inline void
clusterip_config_put(struct clusterip_config *c)
{
	if (refcount_dec_and_test(&c->refcount))
		call_rcu(&c->rcu, clusterip_config_rcu_free);
}

/* decrease the count of entries using/referencing this config.  If last
 * entry(rule) is removed, remove the config from lists, but don't free it
 * yet, since proc-files could still be holding references */
static inline void
clusterip_config_entry_put(struct clusterip_config *c)
{
	struct clusterip_net *cn = clusterip_pernet(c->net);

	local_bh_disable();
	if (refcount_dec_and_lock(&c->entries, &cn->lock)) {
		list_del_rcu(&c->list);
		spin_unlock(&cn->lock);
		local_bh_enable();
		/* In case anyone still accesses the file, the open/close
		 * functions are also incrementing the refcount on their own,
		 * so it's safe to remove the entry even if it's in use. */
#ifdef CONFIG_PROC_FS
		mutex_lock(&cn->mutex);
		if (cn->procdir)
			proc_remove(c->pde);
		mutex_unlock(&cn->mutex);
#endif
		return;
	}
	local_bh_enable();
}

static struct clusterip_config *
__clusterip_config_find(struct net *net, __be32 clusterip)
{
	struct clusterip_config *c;
	struct clusterip_net *cn = clusterip_pernet(net);

	list_for_each_entry_rcu(c, &cn->configs, list) {
		if (c->clusterip == clusterip)
			return c;
	}

	return NULL;
}

static inline struct clusterip_config *
clusterip_config_find_get(struct net *net, __be32 clusterip, int entry)
{
	struct clusterip_config *c;

	rcu_read_lock_bh();
	c = __clusterip_config_find(net, clusterip);
	if (c) {
#ifdef CONFIG_PROC_FS
		if (!c->pde)
			c = NULL;
		else
#endif
		if (unlikely(!refcount_inc_not_zero(&c->refcount)))
			c = NULL;
		else if (entry) {
			if (unlikely(!refcount_inc_not_zero(&c->entries))) {
				clusterip_config_put(c);
				c = NULL;
			}
		}
	}
	rcu_read_unlock_bh();

	return c;
}

static void
clusterip_config_init_nodelist(struct clusterip_config *c,
			       const struct ipt_clusterip_tgt_info *i)
{
	int n;

	for (n = 0; n < i->num_local_nodes; n++)
		set_bit(i->local_nodes[n] - 1, &c->local_nodes);
}

static int
clusterip_netdev_event(struct notifier_block *this, unsigned long event,
		       void *ptr)
{
	struct net_device *dev = netdev_notifier_info_to_dev(ptr);
	struct net *net = dev_net(dev);
	struct clusterip_net *cn = clusterip_pernet(net);
	struct clusterip_config *c;

	spin_lock_bh(&cn->lock);
	list_for_each_entry_rcu(c, &cn->configs, list) {
		switch (event) {
		case NETDEV_REGISTER:
			if (!strcmp(dev->name, c->ifname)) {
				c->ifindex = dev->ifindex;
				dev_mc_add(dev, c->clustermac);
			}
			break;
		case NETDEV_UNREGISTER:
			if (dev->ifindex == c->ifindex) {
				dev_mc_del(dev, c->clustermac);
				c->ifindex = -1;
			}
			break;
		case NETDEV_CHANGENAME:
			if (!strcmp(dev->name, c->ifname)) {
				c->ifindex = dev->ifindex;
				dev_mc_add(dev, c->clustermac);
			} else if (dev->ifindex == c->ifindex) {
				dev_mc_del(dev, c->clustermac);
				c->ifindex = -1;
			}
			break;
		}
	}
	spin_unlock_bh(&cn->lock);

	return NOTIFY_DONE;
}

static struct clusterip_config *
clusterip_config_init(struct net *net, const struct ipt_clusterip_tgt_info *i,
		      __be32 ip, const char *iniface)
{
	struct clusterip_net *cn = clusterip_pernet(net);
	struct clusterip_config *c;
	struct net_device *dev;
	int err;

	if (iniface[0] == '\0') {
		pr_info("Please specify an interface name\n");
		return ERR_PTR(-EINVAL);
	}

	c = kzalloc(sizeof(*c), GFP_ATOMIC);
	if (!c)
		return ERR_PTR(-ENOMEM);

	dev = dev_get_by_name(net, iniface);
	if (!dev) {
		pr_info("no such interface %s\n", iniface);
		kfree(c);
		return ERR_PTR(-ENOENT);
	}
	c->ifindex = dev->ifindex;
	strcpy(c->ifname, dev->name);
	memcpy(&c->clustermac, &i->clustermac, ETH_ALEN);
	dev_mc_add(dev, c->clustermac);
	dev_put(dev);

	c->clusterip = ip;
	c->num_total_nodes = i->num_total_nodes;
	clusterip_config_init_nodelist(c, i);
	c->hash_mode = i->hash_mode;
	c->hash_initval = i->hash_initval;
	c->net = net;
	refcount_set(&c->refcount, 1);

	spin_lock_bh(&cn->lock);
	if (__clusterip_config_find(net, ip)) {
		err = -EBUSY;
		goto out_config_put;
	}

	list_add_rcu(&c->list, &cn->configs);
	spin_unlock_bh(&cn->lock);

#ifdef CONFIG_PROC_FS
	{
		char buffer[16];

		/* create proc dir entry */
		sprintf(buffer, "%pI4", &ip);
		mutex_lock(&cn->mutex);
		c->pde = proc_create_data(buffer, 0600,
					  cn->procdir,
					  &clusterip_proc_ops, c);
		mutex_unlock(&cn->mutex);
		if (!c->pde) {
			err = -ENOMEM;
			goto err;
		}
	}
#endif

	refcount_set(&c->entries, 1);
	return c;

#ifdef CONFIG_PROC_FS
err:
#endif
	spin_lock_bh(&cn->lock);
	list_del_rcu(&c->list);
out_config_put:
	spin_unlock_bh(&cn->lock);
	clusterip_config_put(c);
	return ERR_PTR(err);
}

#ifdef CONFIG_PROC_FS
static int
clusterip_add_node(struct clusterip_config *c, u_int16_t nodenum)
{

	if (nodenum == 0 ||
	    nodenum > c->num_total_nodes)
		return 1;

	/* check if we already have this number in our bitfield */
	if (test_and_set_bit(nodenum - 1, &c->local_nodes))
		return 1;

	return 0;
}

static bool
clusterip_del_node(struct clusterip_config *c, u_int16_t nodenum)
{
	if (nodenum == 0 ||
	    nodenum > c->num_total_nodes)
		return true;

	if (test_and_clear_bit(nodenum - 1, &c->local_nodes))
		return false;

	return true;
}
#endif

static inline u_int32_t
clusterip_hashfn(const struct sk_buff *skb,
		 const struct clusterip_config *config)
{
	const struct iphdr *iph = ip_hdr(skb);
	unsigned long hashval;
	u_int16_t sport = 0, dport = 0;
	int poff;

	poff = proto_ports_offset(iph->protocol);
	if (poff >= 0) {
		const u_int16_t *ports;
		u16 _ports[2];

		ports = skb_header_pointer(skb, iph->ihl * 4 + poff, 4, _ports);
		if (ports) {
			sport = ports[0];
			dport = ports[1];
		}
	} else {
		net_info_ratelimited("unknown protocol %u\n", iph->protocol);
	}

	switch (config->hash_mode) {
	case CLUSTERIP_HASHMODE_SIP:
		hashval = jhash_1word(ntohl(iph->saddr),
				      config->hash_initval);
		break;
	case CLUSTERIP_HASHMODE_SIP_SPT:
		hashval = jhash_2words(ntohl(iph->saddr), sport,
				       config->hash_initval);
		break;
	case CLUSTERIP_HASHMODE_SIP_SPT_DPT:
		hashval = jhash_3words(ntohl(iph->saddr), sport, dport,
				       config->hash_initval);
		break;
	default:
		/* to make gcc happy */
		hashval = 0;
		/* This cannot happen, unless the check function wasn't called
		 * at rule load time */
		pr_info("unknown mode %u\n", config->hash_mode);
		BUG();
		break;
	}

	/* node numbers are 1..n, not 0..n */
	return reciprocal_scale(hashval, config->num_total_nodes) + 1;
}

static inline int
clusterip_responsible(const struct clusterip_config *config, u_int32_t hash)
{
	return test_bit(hash - 1, &config->local_nodes);
}

/***********************************************************************
 * IPTABLES TARGET
 ***********************************************************************/

static unsigned int
clusterip_tg(struct sk_buff *skb, const struct xt_action_param *par)
{
	const struct ipt_clusterip_tgt_info *cipinfo = par->targinfo;
	struct nf_conn *ct;
	enum ip_conntrack_info ctinfo;
	u_int32_t hash;

	/* don't need to clusterip_config_get() here, since refcount
	 * is only decremented by destroy() - and ip_tables guarantees
	 * that the ->target() function isn't called after ->destroy() */

	ct = nf_ct_get(skb, &ctinfo);
	if (ct == NULL)
		return NF_DROP;

	/* special case: ICMP error handling. conntrack distinguishes between
	 * error messages (RELATED) and information requests (see below) */
	if (ip_hdr(skb)->protocol == IPPROTO_ICMP &&
	    (ctinfo == IP_CT_RELATED ||
	     ctinfo == IP_CT_RELATED_REPLY))
		return XT_CONTINUE;

	/* nf_conntrack_proto_icmp guarantees us that we only have ICMP_ECHO,
	 * TIMESTAMP, INFO_REQUEST or ICMP_ADDRESS type icmp packets from here
	 * on, which all have an ID field [relevant for hashing]. */

	hash = clusterip_hashfn(skb, cipinfo->config);

	switch (ctinfo) {
	case IP_CT_NEW:
		ct->mark = hash;
		break;
	case IP_CT_RELATED:
	case IP_CT_RELATED_REPLY:
		/* FIXME: we don't handle expectations at the moment.
		 * They can arrive on a different node than
		 * the master connection (e.g. FTP passive mode) */
	case IP_CT_ESTABLISHED:
	case IP_CT_ESTABLISHED_REPLY:
		break;
	default:			/* Prevent gcc warnings */
		break;
	}

#ifdef DEBUG
	nf_ct_dump_tuple_ip(&ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple);
#endif
	pr_debug("hash=%u ct_hash=%u ", hash, ct->mark);
	if (!clusterip_responsible(cipinfo->config, hash)) {
		pr_debug("not responsible\n");
		return NF_DROP;
	}
	pr_debug("responsible\n");

	/* despite being received via linklayer multicast, this is
	 * actually a unicast IP packet. TCP doesn't like PACKET_MULTICAST */
	skb->pkt_type = PACKET_HOST;

	return XT_CONTINUE;
}

static int clusterip_tg_check(const struct xt_tgchk_param *par)
{
	struct ipt_clusterip_tgt_info *cipinfo = par->targinfo;
	const struct ipt_entry *e = par->entryinfo;
	struct clusterip_config *config;
	int ret, i;

	if (par->nft_compat) {
		pr_err("cannot use CLUSTERIP target from nftables compat\n");
		return -EOPNOTSUPP;
	}

	if (cipinfo->hash_mode != CLUSTERIP_HASHMODE_SIP &&
	    cipinfo->hash_mode != CLUSTERIP_HASHMODE_SIP_SPT &&
	    cipinfo->hash_mode != CLUSTERIP_HASHMODE_SIP_SPT_DPT) {
		pr_info("unknown mode %u\n", cipinfo->hash_mode);
		return -EINVAL;

	}
	if (e->ip.dmsk.s_addr != htonl(0xffffffff) ||
	    e->ip.dst.s_addr == 0) {
		pr_info("Please specify destination IP\n");
		return -EINVAL;
	}
	if (cipinfo->num_local_nodes > ARRAY_SIZE(cipinfo->local_nodes)) {
		pr_info("bad num_local_nodes %u\n", cipinfo->num_local_nodes);
		return -EINVAL;
	}
	for (i = 0; i < cipinfo->num_local_nodes; i++) {
		if (cipinfo->local_nodes[i] - 1 >=
		    sizeof(config->local_nodes) * 8) {
			pr_info("bad local_nodes[%d] %u\n",
				i, cipinfo->local_nodes[i]);
			return -EINVAL;
		}
	}

	config = clusterip_config_find_get(par->net, e->ip.dst.s_addr, 1);
	if (!config) {
		if (!(cipinfo->flags & CLUSTERIP_FLAG_NEW)) {
			pr_info("no config found for %pI4, need 'new'\n",
				&e->ip.dst.s_addr);
			return -EINVAL;
		} else {
			config = clusterip_config_init(par->net, cipinfo,
						       e->ip.dst.s_addr,
						       e->ip.iniface);
			if (IS_ERR(config))
				return PTR_ERR(config);
		}
	} else if (memcmp(&config->clustermac, &cipinfo->clustermac, ETH_ALEN))
		return -EINVAL;

	ret = nf_ct_netns_get(par->net, par->family);
	if (ret < 0) {
		pr_info("cannot load conntrack support for proto=%u\n",
			par->family);
		clusterip_config_entry_put(config);
		clusterip_config_put(config);
		return ret;
	}

	if (!par->net->xt.clusterip_deprecated_warning) {
		pr_info("ipt_CLUSTERIP is deprecated and it will removed soon, "
			"use xt_cluster instead\n");
		par->net->xt.clusterip_deprecated_warning = true;
	}

	cipinfo->config = config;
	return ret;
}

/* drop reference count of cluster config when rule is deleted */
static void clusterip_tg_destroy(const struct xt_tgdtor_param *par)
{
	const struct ipt_clusterip_tgt_info *cipinfo = par->targinfo;

	/* if no more entries are referencing the config, remove it
	 * from the list and destroy the proc entry */
	clusterip_config_entry_put(cipinfo->config);

	clusterip_config_put(cipinfo->config);

	nf_ct_netns_put(par->net, par->family);
}

#ifdef CONFIG_NETFILTER_XTABLES_COMPAT
struct compat_ipt_clusterip_tgt_info
{
	u_int32_t	flags;
	u_int8_t	clustermac[6];
	u_int16_t	num_total_nodes;
	u_int16_t	num_local_nodes;
	u_int16_t	local_nodes[CLUSTERIP_MAX_NODES];
	u_int32_t	hash_mode;
	u_int32_t	hash_initval;
	compat_uptr_t	config;
};
#endif /* CONFIG_NETFILTER_XTABLES_COMPAT */

static struct xt_target clusterip_tg_reg __read_mostly = {
	.name		= "CLUSTERIP",
	.family		= NFPROTO_IPV4,
	.target		= clusterip_tg,
	.checkentry	= clusterip_tg_check,
	.destroy	= clusterip_tg_destroy,
	.targetsize	= sizeof(struct ipt_clusterip_tgt_info),
	.usersize	= offsetof(struct ipt_clusterip_tgt_info, config),
#ifdef CONFIG_NETFILTER_XTABLES_COMPAT
	.compatsize	= sizeof(struct compat_ipt_clusterip_tgt_info),
#endif /* CONFIG_NETFILTER_XTABLES_COMPAT */
	.me		= THIS_MODULE
};


/***********************************************************************
 * ARP MANGLING CODE
 ***********************************************************************/

/* hardcoded for 48bit ethernet and 32bit ipv4 addresses */
struct arp_payload {
	u_int8_t src_hw[ETH_ALEN];
	__be32 src_ip;
	u_int8_t dst_hw[ETH_ALEN];
	__be32 dst_ip;
} __packed;

#ifdef DEBUG
static void arp_print(struct arp_payload *payload)
{
#define HBUFFERLEN 30
	char hbuffer[HBUFFERLEN];
	int j, k;

	for (k = 0, j = 0; k < HBUFFERLEN - 3 && j < ETH_ALEN; j++) {
		hbuffer[k++] = hex_asc_hi(payload->src_hw[j]);
		hbuffer[k++] = hex_asc_lo(payload->src_hw[j]);
		hbuffer[k++] = ':';
	}
	hbuffer[--k] = '\0';

	pr_debug("src %pI4@%s, dst %pI4\n",
		 &payload->src_ip, hbuffer, &payload->dst_ip);
}
#endif

static unsigned int
arp_mangle(void *priv,
	   struct sk_buff *skb,
	   const struct nf_hook_state *state)
{
	struct arphdr *arp = arp_hdr(skb);
	struct arp_payload *payload;
	struct clusterip_config *c;
	struct net *net = state->net;

	/* we don't care about non-ethernet and non-ipv4 ARP */
	if (arp->ar_hrd != htons(ARPHRD_ETHER) ||
	    arp->ar_pro != htons(ETH_P_IP) ||
	    arp->ar_pln != 4 || arp->ar_hln != ETH_ALEN)
		return NF_ACCEPT;

	/* we only want to mangle arp requests and replies */
	if (arp->ar_op != htons(ARPOP_REPLY) &&
	    arp->ar_op != htons(ARPOP_REQUEST))
		return NF_ACCEPT;

	payload = (void *)(arp+1);

	/* if there is no clusterip configuration for the arp reply's
	 * source ip, we don't want to mangle it */
	c = clusterip_config_find_get(net, payload->src_ip, 0);
	if (!c)
		return NF_ACCEPT;

	/* normally the linux kernel always replies to arp queries of
	 * addresses on different interfacs.  However, in the CLUSTERIP case
	 * this wouldn't work, since we didn't subscribe the mcast group on
	 * other interfaces */
	if (c->ifindex != state->out->ifindex) {
		pr_debug("not mangling arp reply on different interface: cip'%d'-skb'%d'\n",
			 c->ifindex, state->out->ifindex);
		clusterip_config_put(c);
		return NF_ACCEPT;
	}

	/* mangle reply hardware address */
	memcpy(payload->src_hw, c->clustermac, arp->ar_hln);

#ifdef DEBUG
	pr_debug("mangled arp reply: ");
	arp_print(payload);
#endif

	clusterip_config_put(c);

	return NF_ACCEPT;
}

static const struct nf_hook_ops cip_arp_ops = {
	.hook = arp_mangle,
	.pf = NFPROTO_ARP,
	.hooknum = NF_ARP_OUT,
	.priority = -1
};

/***********************************************************************
 * PROC DIR HANDLING
 ***********************************************************************/

#ifdef CONFIG_PROC_FS

struct clusterip_seq_position {
	unsigned int pos;	/* position */
	unsigned int weight;	/* number of bits set == size */
	unsigned int bit;	/* current bit */
	unsigned long val;	/* current value */
};

static void *clusterip_seq_start(struct seq_file *s, loff_t *pos)
{
	struct clusterip_config *c = s->private;
	unsigned int weight;
	u_int32_t local_nodes;
	struct clusterip_seq_position *idx;

	/* FIXME: possible race */
	local_nodes = c->local_nodes;
	weight = hweight32(local_nodes);
	if (*pos >= weight)
		return NULL;

	idx = kmalloc(sizeof(struct clusterip_seq_position), GFP_KERNEL);
	if (!idx)
		return ERR_PTR(-ENOMEM);

	idx->pos = *pos;
	idx->weight = weight;
	idx->bit = ffs(local_nodes);
	idx->val = local_nodes;
	clear_bit(idx->bit - 1, &idx->val);

	return idx;
}

static void *clusterip_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	struct clusterip_seq_position *idx = v;

	*pos = ++idx->pos;
	if (*pos >= idx->weight) {
		kfree(v);
		return NULL;
	}
	idx->bit = ffs(idx->val);
	clear_bit(idx->bit - 1, &idx->val);
	return idx;
}

static void clusterip_seq_stop(struct seq_file *s, void *v)
{
	if (!IS_ERR(v))
		kfree(v);
}

static int clusterip_seq_show(struct seq_file *s, void *v)
{
	struct clusterip_seq_position *idx = v;

	if (idx->pos != 0)
		seq_putc(s, ',');

	seq_printf(s, "%u", idx->bit);

	if (idx->pos == idx->weight - 1)
		seq_putc(s, '\n');

	return 0;
}

static const struct seq_operations clusterip_seq_ops = {
	.start	= clusterip_seq_start,
	.next	= clusterip_seq_next,
	.stop	= clusterip_seq_stop,
	.show	= clusterip_seq_show,
};

static int clusterip_proc_open(struct inode *inode, struct file *file)
{
	int ret = seq_open(file, &clusterip_seq_ops);

	if (!ret) {
		struct seq_file *sf = file->private_data;
		struct clusterip_config *c = PDE_DATA(inode);

		sf->private = c;

		clusterip_config_get(c);
	}

	return ret;
}

static int clusterip_proc_release(struct inode *inode, struct file *file)
{
	struct clusterip_config *c = PDE_DATA(inode);
	int ret;

	ret = seq_release(inode, file);

	if (!ret)
		clusterip_config_put(c);

	return ret;
}

static ssize_t clusterip_proc_write(struct file *file, const char __user *input,
				size_t size, loff_t *ofs)
{
	struct clusterip_config *c = PDE_DATA(file_inode(file));
#define PROC_WRITELEN	10
	char buffer[PROC_WRITELEN+1];
	unsigned long nodenum;
	int rc;

	if (size > PROC_WRITELEN)
		return -EIO;
	if (copy_from_user(buffer, input, size))
		return -EFAULT;
	buffer[size] = 0;

	if (*buffer == '+') {
		rc = kstrtoul(buffer+1, 10, &nodenum);
		if (rc)
			return rc;
		if (clusterip_add_node(c, nodenum))
			return -ENOMEM;
	} else if (*buffer == '-') {
		rc = kstrtoul(buffer+1, 10, &nodenum);
		if (rc)
			return rc;
		if (clusterip_del_node(c, nodenum))
			return -ENOENT;
	} else
		return -EIO;

	return size;
}

static const struct proc_ops clusterip_proc_ops = {
	.proc_open	= clusterip_proc_open,
	.proc_read	= seq_read,
	.proc_write	= clusterip_proc_write,
	.proc_lseek	= seq_lseek,
	.proc_release	= clusterip_proc_release,
};

#endif /* CONFIG_PROC_FS */

static int clusterip_net_init(struct net *net)
{
	struct clusterip_net *cn = clusterip_pernet(net);
	int ret;

	INIT_LIST_HEAD(&cn->configs);

	spin_lock_init(&cn->lock);

	ret = nf_register_net_hook(net, &cip_arp_ops);
	if (ret < 0)
		return ret;

#ifdef CONFIG_PROC_FS
	cn->procdir = proc_mkdir("ipt_CLUSTERIP", net->proc_net);
	if (!cn->procdir) {
		nf_unregister_net_hook(net, &cip_arp_ops);
		pr_err("Unable to proc dir entry\n");
		return -ENOMEM;
	}
	mutex_init(&cn->mutex);
#endif /* CONFIG_PROC_FS */

	return 0;
}

static void clusterip_net_exit(struct net *net)
{
#ifdef CONFIG_PROC_FS
	struct clusterip_net *cn = clusterip_pernet(net);

	mutex_lock(&cn->mutex);
	proc_remove(cn->procdir);
	cn->procdir = NULL;
	mutex_unlock(&cn->mutex);
#endif
	nf_unregister_net_hook(net, &cip_arp_ops);
}

static struct pernet_operations clusterip_net_ops = {
	.init = clusterip_net_init,
	.exit = clusterip_net_exit,
	.id   = &clusterip_net_id,
	.size = sizeof(struct clusterip_net),
};

static struct notifier_block cip_netdev_notifier = {
	.notifier_call = clusterip_netdev_event
};

static int __init clusterip_tg_init(void)
{
	int ret;

	ret = register_pernet_subsys(&clusterip_net_ops);
	if (ret < 0)
		return ret;

	ret = xt_register_target(&clusterip_tg_reg);
	if (ret < 0)
		goto cleanup_subsys;

	ret = register_netdevice_notifier(&cip_netdev_notifier);
	if (ret < 0)
		goto unregister_target;

	pr_info("ClusterIP Version %s loaded successfully\n",
		CLUSTERIP_VERSION);

	return 0;

unregister_target:
	xt_unregister_target(&clusterip_tg_reg);
cleanup_subsys:
	unregister_pernet_subsys(&clusterip_net_ops);
	return ret;
}

static void __exit clusterip_tg_exit(void)
{
	pr_info("ClusterIP Version %s unloading\n", CLUSTERIP_VERSION);

	unregister_netdevice_notifier(&cip_netdev_notifier);
	xt_unregister_target(&clusterip_tg_reg);
	unregister_pernet_subsys(&clusterip_net_ops);

	/* Wait for completion of call_rcu()'s (clusterip_config_rcu_free) */
	rcu_barrier();
}

module_init(clusterip_tg_init);
module_exit(clusterip_tg_exit);
