/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __NET_GENEVE_H
#define __NET_GENEVE_H  1

#include <net/udp_tunnel.h>

#define GENEVE_UDP_PORT		6081

/* Geneve Header:
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |Ver|  Opt Len  |O|C|    Rsvd.  |          Protocol Type        |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |        Virtual Network Identifier (VNI)       |    Reserved   |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |                    Variable Length Options                    |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * Option Header:
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |          Option Class         |      Type     |R|R|R| Length  |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |                      Variable Option Data                     |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

struct geneve_opt {
	__be16	opt_class;
	u8	type;
#ifdef __LITTLE_ENDIAN_BITFIELD
	u8	length:5;
	u8	r3:1;
	u8	r2:1;
	u8	r1:1;
#else
	u8	r1:1;
	u8	r2:1;
	u8	r3:1;
	u8	length:5;
#endif
	u8	opt_data[];
};

#define GENEVE_CRIT_OPT_TYPE (1 << 7)

struct genevehdr {
#ifdef __LITTLE_ENDIAN_BITFIELD
	u8 opt_len:6;
	u8 ver:2;
	u8 rsvd1:6;
	u8 critical:1;
	u8 oam:1;
#else
	u8 ver:2;
	u8 opt_len:6;
	u8 oam:1;
	u8 critical:1;
	u8 rsvd1:6;
#endif
	__be16 proto_type;
	u8 vni[3];
	u8 rsvd2;
	struct geneve_opt options[];
};

static inline bool netif_is_geneve(const struct net_device *dev)
{
	return dev->rtnl_link_ops &&
	       !strcmp(dev->rtnl_link_ops->kind, "geneve");
}

#ifdef CONFIG_INET
struct net_device *geneve_dev_create_fb(struct net *net, const char *name,
					u8 name_assign_type, u16 dst_port);
#endif /*ifdef CONFIG_INET */

#endif /*ifdef__NET_GENEVE_H */
