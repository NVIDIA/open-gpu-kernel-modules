// SPDX-License-Identifier: GPL-2.0-only
/* Kernel module to match Hop-by-Hop and Destination parameters. */

/* (C) 2001-2002 Andras Kis-Szabo <kisza@sch.bme.hu>
 */
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/ipv6.h>
#include <linux/types.h>
#include <net/checksum.h>
#include <net/ipv6.h>

#include <asm/byteorder.h>

#include <linux/netfilter/x_tables.h>
#include <linux/netfilter_ipv6/ip6_tables.h>
#include <linux/netfilter_ipv6/ip6t_opts.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Xtables: IPv6 Hop-By-Hop and Destination Header match");
MODULE_AUTHOR("Andras Kis-Szabo <kisza@sch.bme.hu>");
MODULE_ALIAS("ip6t_dst");

/*
 *  (Type & 0xC0) >> 6
 *	0	-> ignorable
 *	1	-> must drop the packet
 *	2	-> send ICMP PARM PROB regardless and drop packet
 *	3	-> Send ICMP if not a multicast address and drop packet
 *  (Type & 0x20) >> 5
 *	0	-> invariant
 *	1	-> can change the routing
 *  (Type & 0x1F) Type
 *	0	-> Pad1 (only 1 byte!)
 *	1	-> PadN LENGTH info (total length = length + 2)
 *	C0 | 2	-> JUMBO 4 x x x x ( xxxx > 64k )
 *	5	-> RTALERT 2 x x
 */

static struct xt_match hbh_mt6_reg[] __read_mostly;

static bool
hbh_mt6(const struct sk_buff *skb, struct xt_action_param *par)
{
	struct ipv6_opt_hdr _optsh;
	const struct ipv6_opt_hdr *oh;
	const struct ip6t_opts *optinfo = par->matchinfo;
	unsigned int temp;
	unsigned int ptr = 0;
	unsigned int hdrlen = 0;
	bool ret = false;
	u8 _opttype;
	u8 _optlen;
	const u_int8_t *tp = NULL;
	const u_int8_t *lp = NULL;
	unsigned int optlen;
	int err;

	err = ipv6_find_hdr(skb, &ptr,
			    (par->match == &hbh_mt6_reg[0]) ?
			    NEXTHDR_HOP : NEXTHDR_DEST, NULL, NULL);
	if (err < 0) {
		if (err != -ENOENT)
			par->hotdrop = true;
		return false;
	}

	oh = skb_header_pointer(skb, ptr, sizeof(_optsh), &_optsh);
	if (oh == NULL) {
		par->hotdrop = true;
		return false;
	}

	hdrlen = ipv6_optlen(oh);
	if (skb->len - ptr < hdrlen) {
		/* Packet smaller than it's length field */
		return false;
	}

	pr_debug("IPv6 OPTS LEN %u %u ", hdrlen, oh->hdrlen);

	pr_debug("len %02X %04X %02X ",
		 optinfo->hdrlen, hdrlen,
		 (!(optinfo->flags & IP6T_OPTS_LEN) ||
		  ((optinfo->hdrlen == hdrlen) ^
		   !!(optinfo->invflags & IP6T_OPTS_INV_LEN))));

	ret = (!(optinfo->flags & IP6T_OPTS_LEN) ||
	       ((optinfo->hdrlen == hdrlen) ^
		!!(optinfo->invflags & IP6T_OPTS_INV_LEN)));

	ptr += 2;
	hdrlen -= 2;
	if (!(optinfo->flags & IP6T_OPTS_OPTS)) {
		return ret;
	} else {
		pr_debug("Strict ");
		pr_debug("#%d ", optinfo->optsnr);
		for (temp = 0; temp < optinfo->optsnr; temp++) {
			/* type field exists ? */
			if (hdrlen < 1)
				break;
			tp = skb_header_pointer(skb, ptr, sizeof(_opttype),
						&_opttype);
			if (tp == NULL)
				break;

			/* Type check */
			if (*tp != (optinfo->opts[temp] & 0xFF00) >> 8) {
				pr_debug("Tbad %02X %02X\n", *tp,
					 (optinfo->opts[temp] & 0xFF00) >> 8);
				return false;
			} else {
				pr_debug("Tok ");
			}
			/* Length check */
			if (*tp) {
				u16 spec_len;

				/* length field exists ? */
				if (hdrlen < 2)
					break;
				lp = skb_header_pointer(skb, ptr + 1,
							sizeof(_optlen),
							&_optlen);
				if (lp == NULL)
					break;
				spec_len = optinfo->opts[temp] & 0x00FF;

				if (spec_len != 0x00FF && spec_len != *lp) {
					pr_debug("Lbad %02X %04X\n", *lp,
						 spec_len);
					return false;
				}
				pr_debug("Lok ");
				optlen = *lp + 2;
			} else {
				pr_debug("Pad1\n");
				optlen = 1;
			}

			/* Step to the next */
			pr_debug("len%04X\n", optlen);

			if ((ptr > skb->len - optlen || hdrlen < optlen) &&
			    temp < optinfo->optsnr - 1) {
				pr_debug("new pointer is too large!\n");
				break;
			}
			ptr += optlen;
			hdrlen -= optlen;
		}
		if (temp == optinfo->optsnr)
			return ret;
		else
			return false;
	}

	return false;
}

static int hbh_mt6_check(const struct xt_mtchk_param *par)
{
	const struct ip6t_opts *optsinfo = par->matchinfo;

	if (optsinfo->invflags & ~IP6T_OPTS_INV_MASK) {
		pr_debug("unknown flags %X\n", optsinfo->invflags);
		return -EINVAL;
	}

	if (optsinfo->flags & IP6T_OPTS_NSTRICT) {
		pr_debug("Not strict - not implemented");
		return -EINVAL;
	}

	return 0;
}

static struct xt_match hbh_mt6_reg[] __read_mostly = {
	{
		/* Note, hbh_mt6 relies on the order of hbh_mt6_reg */
		.name		= "hbh",
		.family		= NFPROTO_IPV6,
		.match		= hbh_mt6,
		.matchsize	= sizeof(struct ip6t_opts),
		.checkentry	= hbh_mt6_check,
		.me		= THIS_MODULE,
	},
	{
		.name		= "dst",
		.family		= NFPROTO_IPV6,
		.match		= hbh_mt6,
		.matchsize	= sizeof(struct ip6t_opts),
		.checkentry	= hbh_mt6_check,
		.me		= THIS_MODULE,
	},
};

static int __init hbh_mt6_init(void)
{
	return xt_register_matches(hbh_mt6_reg, ARRAY_SIZE(hbh_mt6_reg));
}

static void __exit hbh_mt6_exit(void)
{
	xt_unregister_matches(hbh_mt6_reg, ARRAY_SIZE(hbh_mt6_reg));
}

module_init(hbh_mt6_init);
module_exit(hbh_mt6_exit);
