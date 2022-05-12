/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _NF_CONNTRACK_TSTAMP_H
#define _NF_CONNTRACK_TSTAMP_H

#include <net/net_namespace.h>
#include <linux/netfilter/nf_conntrack_common.h>
#include <linux/netfilter/nf_conntrack_tuple_common.h>
#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_extend.h>

struct nf_conn_tstamp {
	u_int64_t start;
	u_int64_t stop;
};

static inline
struct nf_conn_tstamp *nf_conn_tstamp_find(const struct nf_conn *ct)
{
#ifdef CONFIG_NF_CONNTRACK_TIMESTAMP
	return nf_ct_ext_find(ct, NF_CT_EXT_TSTAMP);
#else
	return NULL;
#endif
}

static inline
struct nf_conn_tstamp *nf_ct_tstamp_ext_add(struct nf_conn *ct, gfp_t gfp)
{
#ifdef CONFIG_NF_CONNTRACK_TIMESTAMP
	struct net *net = nf_ct_net(ct);

	if (!net->ct.sysctl_tstamp)
		return NULL;

	return nf_ct_ext_add(ct, NF_CT_EXT_TSTAMP, gfp);
#else
	return NULL;
#endif
};

#ifdef CONFIG_NF_CONNTRACK_TIMESTAMP
void nf_conntrack_tstamp_pernet_init(struct net *net);

int nf_conntrack_tstamp_init(void);
void nf_conntrack_tstamp_fini(void);
#else
static inline void nf_conntrack_tstamp_pernet_init(struct net *net) {}

static inline int nf_conntrack_tstamp_init(void)
{
	return 0;
}

static inline void nf_conntrack_tstamp_fini(void)
{
	return;
}
#endif /* CONFIG_NF_CONNTRACK_TIMESTAMP */

#endif /* _NF_CONNTRACK_TSTAMP_H */
