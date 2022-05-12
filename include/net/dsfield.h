/* SPDX-License-Identifier: GPL-2.0 */
/* include/net/dsfield.h - Manipulation of the Differentiated Services field */

/* Written 1998-2000 by Werner Almesberger, EPFL ICA */


#ifndef __NET_DSFIELD_H
#define __NET_DSFIELD_H

#include <linux/types.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <asm/byteorder.h>


static inline __u8 ipv4_get_dsfield(const struct iphdr *iph)
{
	return iph->tos;
}


static inline __u8 ipv6_get_dsfield(const struct ipv6hdr *ipv6h)
{
	return ntohs(*(__force const __be16 *)ipv6h) >> 4;
}


static inline void ipv4_change_dsfield(struct iphdr *iph,__u8 mask,
    __u8 value)
{
        __u32 check = ntohs((__force __be16)iph->check);
	__u8 dsfield;

	dsfield = (iph->tos & mask) | value;
	check += iph->tos;
	if ((check+1) >> 16) check = (check+1) & 0xffff;
	check -= dsfield;
	check += check >> 16; /* adjust carry */
	iph->check = (__force __sum16)htons(check);
	iph->tos = dsfield;
}


static inline void ipv6_change_dsfield(struct ipv6hdr *ipv6h,__u8 mask,
    __u8 value)
{
	__be16 *p = (__force __be16 *)ipv6h;

	*p = (*p & htons((((u16)mask << 4) | 0xf00f))) | htons((u16)value << 4);
}


#endif
