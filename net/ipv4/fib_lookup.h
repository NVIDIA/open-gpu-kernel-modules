/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _FIB_LOOKUP_H
#define _FIB_LOOKUP_H

#include <linux/types.h>
#include <linux/list.h>
#include <net/ip_fib.h>
#include <net/nexthop.h>

struct fib_alias {
	struct hlist_node	fa_list;
	struct fib_info		*fa_info;
	u8			fa_tos;
	u8			fa_type;
	u8			fa_state;
	u8			fa_slen;
	u32			tb_id;
	s16			fa_default;
	u8			offload:1,
				trap:1,
				offload_failed:1,
				unused:5;
	struct rcu_head		rcu;
};

#define FA_S_ACCESSED	0x01

/* Dont write on fa_state unless needed, to keep it shared on all cpus */
static inline void fib_alias_accessed(struct fib_alias *fa)
{
	if (!(fa->fa_state & FA_S_ACCESSED))
		fa->fa_state |= FA_S_ACCESSED;
}

/* Exported by fib_semantics.c */
void fib_release_info(struct fib_info *);
struct fib_info *fib_create_info(struct fib_config *cfg,
				 struct netlink_ext_ack *extack);
int fib_nh_match(struct net *net, struct fib_config *cfg, struct fib_info *fi,
		 struct netlink_ext_ack *extack);
bool fib_metrics_match(struct fib_config *cfg, struct fib_info *fi);
int fib_dump_info(struct sk_buff *skb, u32 pid, u32 seq, int event,
		  const struct fib_rt_info *fri, unsigned int flags);
void rtmsg_fib(int event, __be32 key, struct fib_alias *fa, int dst_len,
	       u32 tb_id, const struct nl_info *info, unsigned int nlm_flags);
size_t fib_nlmsg_size(struct fib_info *fi);

static inline void fib_result_assign(struct fib_result *res,
				     struct fib_info *fi)
{
	/* we used to play games with refcounts, but we now use RCU */
	res->fi = fi;
	res->nhc = fib_info_nhc(fi, 0);
}

struct fib_prop {
	int	error;
	u8	scope;
};

extern const struct fib_prop fib_props[RTN_MAX + 1];

#endif /* _FIB_LOOKUP_H */
