/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Network device features.
 */
#ifndef _LINUX_NETDEV_FEATURES_H
#define _LINUX_NETDEV_FEATURES_H

#include <linux/types.h>
#include <linux/bitops.h>
#include <asm/byteorder.h>

typedef u64 netdev_features_t;

enum {
	NETIF_F_SG_BIT,			/* Scatter/gather IO. */
	NETIF_F_IP_CSUM_BIT,		/* Can checksum TCP/UDP over IPv4. */
	__UNUSED_NETIF_F_1,
	NETIF_F_HW_CSUM_BIT,		/* Can checksum all the packets. */
	NETIF_F_IPV6_CSUM_BIT,		/* Can checksum TCP/UDP over IPV6 */
	NETIF_F_HIGHDMA_BIT,		/* Can DMA to high memory. */
	NETIF_F_FRAGLIST_BIT,		/* Scatter/gather IO. */
	NETIF_F_HW_VLAN_CTAG_TX_BIT,	/* Transmit VLAN CTAG HW acceleration */
	NETIF_F_HW_VLAN_CTAG_RX_BIT,	/* Receive VLAN CTAG HW acceleration */
	NETIF_F_HW_VLAN_CTAG_FILTER_BIT,/* Receive filtering on VLAN CTAGs */
	NETIF_F_VLAN_CHALLENGED_BIT,	/* Device cannot handle VLAN packets */
	NETIF_F_GSO_BIT,		/* Enable software GSO. */
	NETIF_F_LLTX_BIT,		/* LockLess TX - deprecated. Please */
					/* do not use LLTX in new drivers */
	NETIF_F_NETNS_LOCAL_BIT,	/* Does not change network namespaces */
	NETIF_F_GRO_BIT,		/* Generic receive offload */
	NETIF_F_LRO_BIT,		/* large receive offload */

	/**/NETIF_F_GSO_SHIFT,		/* keep the order of SKB_GSO_* bits */
	NETIF_F_TSO_BIT			/* ... TCPv4 segmentation */
		= NETIF_F_GSO_SHIFT,
	NETIF_F_GSO_ROBUST_BIT,		/* ... ->SKB_GSO_DODGY */
	NETIF_F_TSO_ECN_BIT,		/* ... TCP ECN support */
	NETIF_F_TSO_MANGLEID_BIT,	/* ... IPV4 ID mangling allowed */
	NETIF_F_TSO6_BIT,		/* ... TCPv6 segmentation */
	NETIF_F_FSO_BIT,		/* ... FCoE segmentation */
	NETIF_F_GSO_GRE_BIT,		/* ... GRE with TSO */
	NETIF_F_GSO_GRE_CSUM_BIT,	/* ... GRE with csum with TSO */
	NETIF_F_GSO_IPXIP4_BIT,		/* ... IP4 or IP6 over IP4 with TSO */
	NETIF_F_GSO_IPXIP6_BIT,		/* ... IP4 or IP6 over IP6 with TSO */
	NETIF_F_GSO_UDP_TUNNEL_BIT,	/* ... UDP TUNNEL with TSO */
	NETIF_F_GSO_UDP_TUNNEL_CSUM_BIT,/* ... UDP TUNNEL with TSO & CSUM */
	NETIF_F_GSO_PARTIAL_BIT,	/* ... Only segment inner-most L4
					 *     in hardware and all other
					 *     headers in software.
					 */
	NETIF_F_GSO_TUNNEL_REMCSUM_BIT, /* ... TUNNEL with TSO & REMCSUM */
	NETIF_F_GSO_SCTP_BIT,		/* ... SCTP fragmentation */
	NETIF_F_GSO_ESP_BIT,		/* ... ESP with TSO */
	NETIF_F_GSO_UDP_BIT,		/* ... UFO, deprecated except tuntap */
	NETIF_F_GSO_UDP_L4_BIT,		/* ... UDP payload GSO (not UFO) */
	NETIF_F_GSO_FRAGLIST_BIT,		/* ... Fraglist GSO */
	/**/NETIF_F_GSO_LAST =		/* last bit, see GSO_MASK */
		NETIF_F_GSO_FRAGLIST_BIT,

	NETIF_F_FCOE_CRC_BIT,		/* FCoE CRC32 */
	NETIF_F_SCTP_CRC_BIT,		/* SCTP checksum offload */
	NETIF_F_FCOE_MTU_BIT,		/* Supports max FCoE MTU, 2158 bytes*/
	NETIF_F_NTUPLE_BIT,		/* N-tuple filters supported */
	NETIF_F_RXHASH_BIT,		/* Receive hashing offload */
	NETIF_F_RXCSUM_BIT,		/* Receive checksumming offload */
	NETIF_F_NOCACHE_COPY_BIT,	/* Use no-cache copyfromuser */
	NETIF_F_LOOPBACK_BIT,		/* Enable loopback */
	NETIF_F_RXFCS_BIT,		/* Append FCS to skb pkt data */
	NETIF_F_RXALL_BIT,		/* Receive errored frames too */
	NETIF_F_HW_VLAN_STAG_TX_BIT,	/* Transmit VLAN STAG HW acceleration */
	NETIF_F_HW_VLAN_STAG_RX_BIT,	/* Receive VLAN STAG HW acceleration */
	NETIF_F_HW_VLAN_STAG_FILTER_BIT,/* Receive filtering on VLAN STAGs */
	NETIF_F_HW_L2FW_DOFFLOAD_BIT,	/* Allow L2 Forwarding in Hardware */

	NETIF_F_HW_TC_BIT,		/* Offload TC infrastructure */
	NETIF_F_HW_ESP_BIT,		/* Hardware ESP transformation offload */
	NETIF_F_HW_ESP_TX_CSUM_BIT,	/* ESP with TX checksum offload */
	NETIF_F_RX_UDP_TUNNEL_PORT_BIT, /* Offload of RX port for UDP tunnels */
	NETIF_F_HW_TLS_TX_BIT,		/* Hardware TLS TX offload */
	NETIF_F_HW_TLS_RX_BIT,		/* Hardware TLS RX offload */

	NETIF_F_GRO_HW_BIT,		/* Hardware Generic receive offload */
	NETIF_F_HW_TLS_RECORD_BIT,	/* Offload TLS record */
	NETIF_F_GRO_FRAGLIST_BIT,	/* Fraglist GRO */

	NETIF_F_HW_MACSEC_BIT,		/* Offload MACsec operations */
	NETIF_F_GRO_UDP_FWD_BIT,	/* Allow UDP GRO for forwarding */

	NETIF_F_HW_HSR_TAG_INS_BIT,	/* Offload HSR tag insertion */
	NETIF_F_HW_HSR_TAG_RM_BIT,	/* Offload HSR tag removal */
	NETIF_F_HW_HSR_FWD_BIT,		/* Offload HSR forwarding */
	NETIF_F_HW_HSR_DUP_BIT,		/* Offload HSR duplication */

	/*
	 * Add your fresh new feature above and remember to update
	 * netdev_features_strings[] in net/core/ethtool.c and maybe
	 * some feature mask #defines below. Please also describe it
	 * in Documentation/networking/netdev-features.rst.
	 */

	/**/NETDEV_FEATURE_COUNT
};

/* copy'n'paste compression ;) */
#define __NETIF_F_BIT(bit)	((netdev_features_t)1 << (bit))
#define __NETIF_F(name)		__NETIF_F_BIT(NETIF_F_##name##_BIT)

#define NETIF_F_FCOE_CRC	__NETIF_F(FCOE_CRC)
#define NETIF_F_FCOE_MTU	__NETIF_F(FCOE_MTU)
#define NETIF_F_FRAGLIST	__NETIF_F(FRAGLIST)
#define NETIF_F_FSO		__NETIF_F(FSO)
#define NETIF_F_GRO		__NETIF_F(GRO)
#define NETIF_F_GRO_HW		__NETIF_F(GRO_HW)
#define NETIF_F_GSO		__NETIF_F(GSO)
#define NETIF_F_GSO_ROBUST	__NETIF_F(GSO_ROBUST)
#define NETIF_F_HIGHDMA		__NETIF_F(HIGHDMA)
#define NETIF_F_HW_CSUM		__NETIF_F(HW_CSUM)
#define NETIF_F_HW_VLAN_CTAG_FILTER __NETIF_F(HW_VLAN_CTAG_FILTER)
#define NETIF_F_HW_VLAN_CTAG_RX	__NETIF_F(HW_VLAN_CTAG_RX)
#define NETIF_F_HW_VLAN_CTAG_TX	__NETIF_F(HW_VLAN_CTAG_TX)
#define NETIF_F_IP_CSUM		__NETIF_F(IP_CSUM)
#define NETIF_F_IPV6_CSUM	__NETIF_F(IPV6_CSUM)
#define NETIF_F_LLTX		__NETIF_F(LLTX)
#define NETIF_F_LOOPBACK	__NETIF_F(LOOPBACK)
#define NETIF_F_LRO		__NETIF_F(LRO)
#define NETIF_F_NETNS_LOCAL	__NETIF_F(NETNS_LOCAL)
#define NETIF_F_NOCACHE_COPY	__NETIF_F(NOCACHE_COPY)
#define NETIF_F_NTUPLE		__NETIF_F(NTUPLE)
#define NETIF_F_RXCSUM		__NETIF_F(RXCSUM)
#define NETIF_F_RXHASH		__NETIF_F(RXHASH)
#define NETIF_F_SCTP_CRC	__NETIF_F(SCTP_CRC)
#define NETIF_F_SG		__NETIF_F(SG)
#define NETIF_F_TSO6		__NETIF_F(TSO6)
#define NETIF_F_TSO_ECN		__NETIF_F(TSO_ECN)
#define NETIF_F_TSO		__NETIF_F(TSO)
#define NETIF_F_VLAN_CHALLENGED	__NETIF_F(VLAN_CHALLENGED)
#define NETIF_F_RXFCS		__NETIF_F(RXFCS)
#define NETIF_F_RXALL		__NETIF_F(RXALL)
#define NETIF_F_GSO_GRE		__NETIF_F(GSO_GRE)
#define NETIF_F_GSO_GRE_CSUM	__NETIF_F(GSO_GRE_CSUM)
#define NETIF_F_GSO_IPXIP4	__NETIF_F(GSO_IPXIP4)
#define NETIF_F_GSO_IPXIP6	__NETIF_F(GSO_IPXIP6)
#define NETIF_F_GSO_UDP_TUNNEL	__NETIF_F(GSO_UDP_TUNNEL)
#define NETIF_F_GSO_UDP_TUNNEL_CSUM __NETIF_F(GSO_UDP_TUNNEL_CSUM)
#define NETIF_F_TSO_MANGLEID	__NETIF_F(TSO_MANGLEID)
#define NETIF_F_GSO_PARTIAL	 __NETIF_F(GSO_PARTIAL)
#define NETIF_F_GSO_TUNNEL_REMCSUM __NETIF_F(GSO_TUNNEL_REMCSUM)
#define NETIF_F_GSO_SCTP	__NETIF_F(GSO_SCTP)
#define NETIF_F_GSO_ESP		__NETIF_F(GSO_ESP)
#define NETIF_F_GSO_UDP		__NETIF_F(GSO_UDP)
#define NETIF_F_HW_VLAN_STAG_FILTER __NETIF_F(HW_VLAN_STAG_FILTER)
#define NETIF_F_HW_VLAN_STAG_RX	__NETIF_F(HW_VLAN_STAG_RX)
#define NETIF_F_HW_VLAN_STAG_TX	__NETIF_F(HW_VLAN_STAG_TX)
#define NETIF_F_HW_L2FW_DOFFLOAD	__NETIF_F(HW_L2FW_DOFFLOAD)
#define NETIF_F_HW_TC		__NETIF_F(HW_TC)
#define NETIF_F_HW_ESP		__NETIF_F(HW_ESP)
#define NETIF_F_HW_ESP_TX_CSUM	__NETIF_F(HW_ESP_TX_CSUM)
#define	NETIF_F_RX_UDP_TUNNEL_PORT  __NETIF_F(RX_UDP_TUNNEL_PORT)
#define NETIF_F_HW_TLS_RECORD	__NETIF_F(HW_TLS_RECORD)
#define NETIF_F_GSO_UDP_L4	__NETIF_F(GSO_UDP_L4)
#define NETIF_F_HW_TLS_TX	__NETIF_F(HW_TLS_TX)
#define NETIF_F_HW_TLS_RX	__NETIF_F(HW_TLS_RX)
#define NETIF_F_GRO_FRAGLIST	__NETIF_F(GRO_FRAGLIST)
#define NETIF_F_GSO_FRAGLIST	__NETIF_F(GSO_FRAGLIST)
#define NETIF_F_HW_MACSEC	__NETIF_F(HW_MACSEC)
#define NETIF_F_GRO_UDP_FWD	__NETIF_F(GRO_UDP_FWD)
#define NETIF_F_HW_HSR_TAG_INS	__NETIF_F(HW_HSR_TAG_INS)
#define NETIF_F_HW_HSR_TAG_RM	__NETIF_F(HW_HSR_TAG_RM)
#define NETIF_F_HW_HSR_FWD	__NETIF_F(HW_HSR_FWD)
#define NETIF_F_HW_HSR_DUP	__NETIF_F(HW_HSR_DUP)

/* Finds the next feature with the highest number of the range of start till 0.
 */
static inline int find_next_netdev_feature(u64 feature, unsigned long start)
{
	/* like BITMAP_LAST_WORD_MASK() for u64
	 * this sets the most significant 64 - start to 0.
	 */
	feature &= ~0ULL >> (-start & ((sizeof(feature) * 8) - 1));

	return fls64(feature) - 1;
}

/* This goes for the MSB to the LSB through the set feature bits,
 * mask_addr should be a u64 and bit an int
 */
#define for_each_netdev_feature(mask_addr, bit)				\
	for ((bit) = find_next_netdev_feature((mask_addr),		\
					      NETDEV_FEATURE_COUNT);	\
	     (bit) >= 0;						\
	     (bit) = find_next_netdev_feature((mask_addr), (bit) - 1))

/* Features valid for ethtool to change */
/* = all defined minus driver/device-class-related */
#define NETIF_F_NEVER_CHANGE	(NETIF_F_VLAN_CHALLENGED | \
				 NETIF_F_LLTX | NETIF_F_NETNS_LOCAL)

/* remember that ((t)1 << t_BITS) is undefined in C99 */
#define NETIF_F_ETHTOOL_BITS	((__NETIF_F_BIT(NETDEV_FEATURE_COUNT - 1) | \
		(__NETIF_F_BIT(NETDEV_FEATURE_COUNT - 1) - 1)) & \
		~NETIF_F_NEVER_CHANGE)

/* Segmentation offload feature mask */
#define NETIF_F_GSO_MASK	(__NETIF_F_BIT(NETIF_F_GSO_LAST + 1) - \
		__NETIF_F_BIT(NETIF_F_GSO_SHIFT))

/* List of IP checksum features. Note that NETIF_F_HW_CSUM should not be
 * set in features when NETIF_F_IP_CSUM or NETIF_F_IPV6_CSUM are set--
 * this would be contradictory
 */
#define NETIF_F_CSUM_MASK	(NETIF_F_IP_CSUM | NETIF_F_IPV6_CSUM | \
				 NETIF_F_HW_CSUM)

#define NETIF_F_ALL_TSO 	(NETIF_F_TSO | NETIF_F_TSO6 | \
				 NETIF_F_TSO_ECN | NETIF_F_TSO_MANGLEID)

#define NETIF_F_ALL_FCOE	(NETIF_F_FCOE_CRC | NETIF_F_FCOE_MTU | \
				 NETIF_F_FSO)

/* List of features with software fallbacks. */
#define NETIF_F_GSO_SOFTWARE	(NETIF_F_ALL_TSO | NETIF_F_GSO_SCTP |	     \
				 NETIF_F_GSO_UDP_L4 | NETIF_F_GSO_FRAGLIST)

/*
 * If one device supports one of these features, then enable them
 * for all in netdev_increment_features.
 */
#define NETIF_F_ONE_FOR_ALL	(NETIF_F_GSO_SOFTWARE | NETIF_F_GSO_ROBUST | \
				 NETIF_F_SG | NETIF_F_HIGHDMA |		\
				 NETIF_F_FRAGLIST | NETIF_F_VLAN_CHALLENGED)

/*
 * If one device doesn't support one of these features, then disable it
 * for all in netdev_increment_features.
 */
#define NETIF_F_ALL_FOR_ALL	(NETIF_F_NOCACHE_COPY | NETIF_F_FSO)

/*
 * If upper/master device has these features disabled, they must be disabled
 * on all lower/slave devices as well.
 */
#define NETIF_F_UPPER_DISABLES	NETIF_F_LRO

/* changeable features with no special hardware requirements */
#define NETIF_F_SOFT_FEATURES	(NETIF_F_GSO | NETIF_F_GRO)

/* Changeable features with no special hardware requirements that defaults to off. */
#define NETIF_F_SOFT_FEATURES_OFF	(NETIF_F_GRO_FRAGLIST | NETIF_F_GRO_UDP_FWD)

#define NETIF_F_VLAN_FEATURES	(NETIF_F_HW_VLAN_CTAG_FILTER | \
				 NETIF_F_HW_VLAN_CTAG_RX | \
				 NETIF_F_HW_VLAN_CTAG_TX | \
				 NETIF_F_HW_VLAN_STAG_FILTER | \
				 NETIF_F_HW_VLAN_STAG_RX | \
				 NETIF_F_HW_VLAN_STAG_TX)

#define NETIF_F_GSO_ENCAP_ALL	(NETIF_F_GSO_GRE |			\
				 NETIF_F_GSO_GRE_CSUM |			\
				 NETIF_F_GSO_IPXIP4 |			\
				 NETIF_F_GSO_IPXIP6 |			\
				 NETIF_F_GSO_UDP_TUNNEL |		\
				 NETIF_F_GSO_UDP_TUNNEL_CSUM)

#endif	/* _LINUX_NETDEV_FEATURES_H */
