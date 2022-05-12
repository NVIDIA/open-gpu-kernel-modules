// SPDX-License-Identifier: GPL-2.0-only
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt
#include <linux/module.h>
#include <linux/skbuff.h>
#include <net/ip.h>
#include <net/ipv6.h>
#include <net/sctp/sctp.h>
#include <linux/sctp.h>

#include <linux/netfilter/x_tables.h>
#include <linux/netfilter/xt_sctp.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netfilter_ipv6/ip6_tables.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kiran Kumar Immidi");
MODULE_DESCRIPTION("Xtables: SCTP protocol packet match");
MODULE_ALIAS("ipt_sctp");
MODULE_ALIAS("ip6t_sctp");

#define SCCHECK(cond, option, flag, invflag) (!((flag) & (option)) \
					      || (!!((invflag) & (option)) ^ (cond)))

static bool
match_flags(const struct xt_sctp_flag_info *flag_info,
	    const int flag_count,
	    u_int8_t chunktype,
	    u_int8_t chunkflags)
{
	int i;

	for (i = 0; i < flag_count; i++)
		if (flag_info[i].chunktype == chunktype)
			return (chunkflags & flag_info[i].flag_mask) == flag_info[i].flag;

	return true;
}

static inline bool
match_packet(const struct sk_buff *skb,
	     unsigned int offset,
	     const struct xt_sctp_info *info,
	     bool *hotdrop)
{
	u_int32_t chunkmapcopy[256 / sizeof (u_int32_t)];
	const struct sctp_chunkhdr *sch;
	struct sctp_chunkhdr _sch;
	int chunk_match_type = info->chunk_match_type;
	const struct xt_sctp_flag_info *flag_info = info->flag_info;
	int flag_count = info->flag_count;

#ifdef DEBUG
	int i = 0;
#endif

	if (chunk_match_type == SCTP_CHUNK_MATCH_ALL)
		SCTP_CHUNKMAP_COPY(chunkmapcopy, info->chunkmap);

	do {
		sch = skb_header_pointer(skb, offset, sizeof(_sch), &_sch);
		if (sch == NULL || sch->length == 0) {
			pr_debug("Dropping invalid SCTP packet.\n");
			*hotdrop = true;
			return false;
		}
#ifdef DEBUG
		pr_debug("Chunk num: %d\toffset: %d\ttype: %d\tlength: %d"
			 "\tflags: %x\n",
			 ++i, offset, sch->type, htons(sch->length),
			 sch->flags);
#endif
		offset += SCTP_PAD4(ntohs(sch->length));

		pr_debug("skb->len: %d\toffset: %d\n", skb->len, offset);

		if (SCTP_CHUNKMAP_IS_SET(info->chunkmap, sch->type)) {
			switch (chunk_match_type) {
			case SCTP_CHUNK_MATCH_ANY:
				if (match_flags(flag_info, flag_count,
					sch->type, sch->flags)) {
					return true;
				}
				break;

			case SCTP_CHUNK_MATCH_ALL:
				if (match_flags(flag_info, flag_count,
				    sch->type, sch->flags))
					SCTP_CHUNKMAP_CLEAR(chunkmapcopy, sch->type);
				break;

			case SCTP_CHUNK_MATCH_ONLY:
				if (!match_flags(flag_info, flag_count,
				    sch->type, sch->flags))
					return false;
				break;
			}
		} else {
			switch (chunk_match_type) {
			case SCTP_CHUNK_MATCH_ONLY:
				return false;
			}
		}
	} while (offset < skb->len);

	switch (chunk_match_type) {
	case SCTP_CHUNK_MATCH_ALL:
		return SCTP_CHUNKMAP_IS_CLEAR(chunkmapcopy);
	case SCTP_CHUNK_MATCH_ANY:
		return false;
	case SCTP_CHUNK_MATCH_ONLY:
		return true;
	}

	/* This will never be reached, but required to stop compiler whine */
	return false;
}

static bool
sctp_mt(const struct sk_buff *skb, struct xt_action_param *par)
{
	const struct xt_sctp_info *info = par->matchinfo;
	const struct sctphdr *sh;
	struct sctphdr _sh;

	if (par->fragoff != 0) {
		pr_debug("Dropping non-first fragment.. FIXME\n");
		return false;
	}

	sh = skb_header_pointer(skb, par->thoff, sizeof(_sh), &_sh);
	if (sh == NULL) {
		pr_debug("Dropping evil TCP offset=0 tinygram.\n");
		par->hotdrop = true;
		return false;
	}
	pr_debug("spt: %d\tdpt: %d\n", ntohs(sh->source), ntohs(sh->dest));

	return  SCCHECK(ntohs(sh->source) >= info->spts[0]
			&& ntohs(sh->source) <= info->spts[1],
			XT_SCTP_SRC_PORTS, info->flags, info->invflags) &&
		SCCHECK(ntohs(sh->dest) >= info->dpts[0]
			&& ntohs(sh->dest) <= info->dpts[1],
			XT_SCTP_DEST_PORTS, info->flags, info->invflags) &&
		SCCHECK(match_packet(skb, par->thoff + sizeof(_sh),
				     info, &par->hotdrop),
			XT_SCTP_CHUNK_TYPES, info->flags, info->invflags);
}

static int sctp_mt_check(const struct xt_mtchk_param *par)
{
	const struct xt_sctp_info *info = par->matchinfo;

	if (info->flags & ~XT_SCTP_VALID_FLAGS)
		return -EINVAL;
	if (info->invflags & ~XT_SCTP_VALID_FLAGS)
		return -EINVAL;
	if (info->invflags & ~info->flags)
		return -EINVAL;
	if (!(info->flags & XT_SCTP_CHUNK_TYPES))
		return 0;
	if (info->chunk_match_type & (SCTP_CHUNK_MATCH_ALL |
	    SCTP_CHUNK_MATCH_ANY | SCTP_CHUNK_MATCH_ONLY))
		return 0;
	return -EINVAL;
}

static struct xt_match sctp_mt_reg[] __read_mostly = {
	{
		.name		= "sctp",
		.family		= NFPROTO_IPV4,
		.checkentry	= sctp_mt_check,
		.match		= sctp_mt,
		.matchsize	= sizeof(struct xt_sctp_info),
		.proto		= IPPROTO_SCTP,
		.me		= THIS_MODULE
	},
	{
		.name		= "sctp",
		.family		= NFPROTO_IPV6,
		.checkentry	= sctp_mt_check,
		.match		= sctp_mt,
		.matchsize	= sizeof(struct xt_sctp_info),
		.proto		= IPPROTO_SCTP,
		.me		= THIS_MODULE
	},
};

static int __init sctp_mt_init(void)
{
	return xt_register_matches(sctp_mt_reg, ARRAY_SIZE(sctp_mt_reg));
}

static void __exit sctp_mt_exit(void)
{
	xt_unregister_matches(sctp_mt_reg, ARRAY_SIZE(sctp_mt_reg));
}

module_init(sctp_mt_init);
module_exit(sctp_mt_exit);
