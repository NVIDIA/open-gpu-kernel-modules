/*
 * Copyright (c) 2004 Topspin Corporation.  All rights reserved.
 * Copyright (c) 2005 Sun Microsystems, Inc. All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <linux/errno.h>
#include <linux/string.h>
#include <linux/export.h>
#include <linux/if_ether.h>
#include <linux/ip.h>

#include <rdma/ib_pack.h>

#define STRUCT_FIELD(header, field) \
	.struct_offset_bytes = offsetof(struct ib_unpacked_ ## header, field),      \
	.struct_size_bytes   = sizeof_field(struct ib_unpacked_ ## header, field), \
	.field_name          = #header ":" #field

static const struct ib_field lrh_table[]  = {
	{ STRUCT_FIELD(lrh, virtual_lane),
	  .offset_words = 0,
	  .offset_bits  = 0,
	  .size_bits    = 4 },
	{ STRUCT_FIELD(lrh, link_version),
	  .offset_words = 0,
	  .offset_bits  = 4,
	  .size_bits    = 4 },
	{ STRUCT_FIELD(lrh, service_level),
	  .offset_words = 0,
	  .offset_bits  = 8,
	  .size_bits    = 4 },
	{ RESERVED,
	  .offset_words = 0,
	  .offset_bits  = 12,
	  .size_bits    = 2 },
	{ STRUCT_FIELD(lrh, link_next_header),
	  .offset_words = 0,
	  .offset_bits  = 14,
	  .size_bits    = 2 },
	{ STRUCT_FIELD(lrh, destination_lid),
	  .offset_words = 0,
	  .offset_bits  = 16,
	  .size_bits    = 16 },
	{ RESERVED,
	  .offset_words = 1,
	  .offset_bits  = 0,
	  .size_bits    = 5 },
	{ STRUCT_FIELD(lrh, packet_length),
	  .offset_words = 1,
	  .offset_bits  = 5,
	  .size_bits    = 11 },
	{ STRUCT_FIELD(lrh, source_lid),
	  .offset_words = 1,
	  .offset_bits  = 16,
	  .size_bits    = 16 }
};

static const struct ib_field eth_table[]  = {
	{ STRUCT_FIELD(eth, dmac_h),
	  .offset_words = 0,
	  .offset_bits  = 0,
	  .size_bits    = 32 },
	{ STRUCT_FIELD(eth, dmac_l),
	  .offset_words = 1,
	  .offset_bits  = 0,
	  .size_bits    = 16 },
	{ STRUCT_FIELD(eth, smac_h),
	  .offset_words = 1,
	  .offset_bits  = 16,
	  .size_bits    = 16 },
	{ STRUCT_FIELD(eth, smac_l),
	  .offset_words = 2,
	  .offset_bits  = 0,
	  .size_bits    = 32 },
	{ STRUCT_FIELD(eth, type),
	  .offset_words = 3,
	  .offset_bits  = 0,
	  .size_bits    = 16 }
};

static const struct ib_field vlan_table[]  = {
	{ STRUCT_FIELD(vlan, tag),
	  .offset_words = 0,
	  .offset_bits  = 0,
	  .size_bits    = 16 },
	{ STRUCT_FIELD(vlan, type),
	  .offset_words = 0,
	  .offset_bits  = 16,
	  .size_bits    = 16 }
};

static const struct ib_field ip4_table[]  = {
	{ STRUCT_FIELD(ip4, ver),
	  .offset_words = 0,
	  .offset_bits  = 0,
	  .size_bits    = 4 },
	{ STRUCT_FIELD(ip4, hdr_len),
	  .offset_words = 0,
	  .offset_bits  = 4,
	  .size_bits    = 4 },
	{ STRUCT_FIELD(ip4, tos),
	  .offset_words = 0,
	  .offset_bits  = 8,
	  .size_bits    = 8 },
	{ STRUCT_FIELD(ip4, tot_len),
	  .offset_words = 0,
	  .offset_bits  = 16,
	  .size_bits    = 16 },
	{ STRUCT_FIELD(ip4, id),
	  .offset_words = 1,
	  .offset_bits  = 0,
	  .size_bits    = 16 },
	{ STRUCT_FIELD(ip4, frag_off),
	  .offset_words = 1,
	  .offset_bits  = 16,
	  .size_bits    = 16 },
	{ STRUCT_FIELD(ip4, ttl),
	  .offset_words = 2,
	  .offset_bits  = 0,
	  .size_bits    = 8 },
	{ STRUCT_FIELD(ip4, protocol),
	  .offset_words = 2,
	  .offset_bits  = 8,
	  .size_bits    = 8 },
	{ STRUCT_FIELD(ip4, check),
	  .offset_words = 2,
	  .offset_bits  = 16,
	  .size_bits    = 16 },
	{ STRUCT_FIELD(ip4, saddr),
	  .offset_words = 3,
	  .offset_bits  = 0,
	  .size_bits    = 32 },
	{ STRUCT_FIELD(ip4, daddr),
	  .offset_words = 4,
	  .offset_bits  = 0,
	  .size_bits    = 32 }
};

static const struct ib_field udp_table[]  = {
	{ STRUCT_FIELD(udp, sport),
	  .offset_words = 0,
	  .offset_bits  = 0,
	  .size_bits    = 16 },
	{ STRUCT_FIELD(udp, dport),
	  .offset_words = 0,
	  .offset_bits  = 16,
	  .size_bits    = 16 },
	{ STRUCT_FIELD(udp, length),
	  .offset_words = 1,
	  .offset_bits  = 0,
	  .size_bits    = 16 },
	{ STRUCT_FIELD(udp, csum),
	  .offset_words = 1,
	  .offset_bits  = 16,
	  .size_bits    = 16 }
};

static const struct ib_field grh_table[]  = {
	{ STRUCT_FIELD(grh, ip_version),
	  .offset_words = 0,
	  .offset_bits  = 0,
	  .size_bits    = 4 },
	{ STRUCT_FIELD(grh, traffic_class),
	  .offset_words = 0,
	  .offset_bits  = 4,
	  .size_bits    = 8 },
	{ STRUCT_FIELD(grh, flow_label),
	  .offset_words = 0,
	  .offset_bits  = 12,
	  .size_bits    = 20 },
	{ STRUCT_FIELD(grh, payload_length),
	  .offset_words = 1,
	  .offset_bits  = 0,
	  .size_bits    = 16 },
	{ STRUCT_FIELD(grh, next_header),
	  .offset_words = 1,
	  .offset_bits  = 16,
	  .size_bits    = 8 },
	{ STRUCT_FIELD(grh, hop_limit),
	  .offset_words = 1,
	  .offset_bits  = 24,
	  .size_bits    = 8 },
	{ STRUCT_FIELD(grh, source_gid),
	  .offset_words = 2,
	  .offset_bits  = 0,
	  .size_bits    = 128 },
	{ STRUCT_FIELD(grh, destination_gid),
	  .offset_words = 6,
	  .offset_bits  = 0,
	  .size_bits    = 128 }
};

static const struct ib_field bth_table[]  = {
	{ STRUCT_FIELD(bth, opcode),
	  .offset_words = 0,
	  .offset_bits  = 0,
	  .size_bits    = 8 },
	{ STRUCT_FIELD(bth, solicited_event),
	  .offset_words = 0,
	  .offset_bits  = 8,
	  .size_bits    = 1 },
	{ STRUCT_FIELD(bth, mig_req),
	  .offset_words = 0,
	  .offset_bits  = 9,
	  .size_bits    = 1 },
	{ STRUCT_FIELD(bth, pad_count),
	  .offset_words = 0,
	  .offset_bits  = 10,
	  .size_bits    = 2 },
	{ STRUCT_FIELD(bth, transport_header_version),
	  .offset_words = 0,
	  .offset_bits  = 12,
	  .size_bits    = 4 },
	{ STRUCT_FIELD(bth, pkey),
	  .offset_words = 0,
	  .offset_bits  = 16,
	  .size_bits    = 16 },
	{ RESERVED,
	  .offset_words = 1,
	  .offset_bits  = 0,
	  .size_bits    = 8 },
	{ STRUCT_FIELD(bth, destination_qpn),
	  .offset_words = 1,
	  .offset_bits  = 8,
	  .size_bits    = 24 },
	{ STRUCT_FIELD(bth, ack_req),
	  .offset_words = 2,
	  .offset_bits  = 0,
	  .size_bits    = 1 },
	{ RESERVED,
	  .offset_words = 2,
	  .offset_bits  = 1,
	  .size_bits    = 7 },
	{ STRUCT_FIELD(bth, psn),
	  .offset_words = 2,
	  .offset_bits  = 8,
	  .size_bits    = 24 }
};

static const struct ib_field deth_table[] = {
	{ STRUCT_FIELD(deth, qkey),
	  .offset_words = 0,
	  .offset_bits  = 0,
	  .size_bits    = 32 },
	{ RESERVED,
	  .offset_words = 1,
	  .offset_bits  = 0,
	  .size_bits    = 8 },
	{ STRUCT_FIELD(deth, source_qpn),
	  .offset_words = 1,
	  .offset_bits  = 8,
	  .size_bits    = 24 }
};

__sum16 ib_ud_ip4_csum(struct ib_ud_header *header)
{
	struct iphdr iph;

	iph.ihl		= 5;
	iph.version	= 4;
	iph.tos		= header->ip4.tos;
	iph.tot_len	= header->ip4.tot_len;
	iph.id		= header->ip4.id;
	iph.frag_off	= header->ip4.frag_off;
	iph.ttl		= header->ip4.ttl;
	iph.protocol	= header->ip4.protocol;
	iph.check	= 0;
	iph.saddr	= header->ip4.saddr;
	iph.daddr	= header->ip4.daddr;

	return ip_fast_csum((u8 *)&iph, iph.ihl);
}
EXPORT_SYMBOL(ib_ud_ip4_csum);

/**
 * ib_ud_header_init - Initialize UD header structure
 * @payload_bytes:Length of packet payload
 * @lrh_present: specify if LRH is present
 * @eth_present: specify if Eth header is present
 * @vlan_present: packet is tagged vlan
 * @grh_present: GRH flag (if non-zero, GRH will be included)
 * @ip_version: if non-zero, IP header, V4 or V6, will be included
 * @udp_present :if non-zero, UDP header will be included
 * @immediate_present: specify if immediate data is present
 * @header:Structure to initialize
 */
int ib_ud_header_init(int     payload_bytes,
		      int    lrh_present,
		      int    eth_present,
		      int    vlan_present,
		      int    grh_present,
		      int    ip_version,
		      int    udp_present,
		      int    immediate_present,
		      struct ib_ud_header *header)
{
	size_t udp_bytes = udp_present ? IB_UDP_BYTES : 0;

	grh_present = grh_present && !ip_version;
	memset(header, 0, sizeof *header);

	/*
	 * UDP header without IP header doesn't make sense
	 */
	if (udp_present && ip_version != 4 && ip_version != 6)
		return -EINVAL;

	if (lrh_present) {
		u16 packet_length;

		header->lrh.link_version     = 0;
		header->lrh.link_next_header =
			grh_present ? IB_LNH_IBA_GLOBAL : IB_LNH_IBA_LOCAL;
		packet_length = (IB_LRH_BYTES	+
				 IB_BTH_BYTES	+
				 IB_DETH_BYTES	+
				 (grh_present ? IB_GRH_BYTES : 0) +
				 payload_bytes	+
				 4		+ /* ICRC     */
				 3) / 4;	  /* round up */
		header->lrh.packet_length = cpu_to_be16(packet_length);
	}

	if (vlan_present)
		header->eth.type = cpu_to_be16(ETH_P_8021Q);

	if (ip_version == 6 || grh_present) {
		header->grh.ip_version      = 6;
		header->grh.payload_length  =
			cpu_to_be16((udp_bytes        +
				     IB_BTH_BYTES     +
				     IB_DETH_BYTES    +
				     payload_bytes    +
				     4                + /* ICRC     */
				     3) & ~3);          /* round up */
		header->grh.next_header     = udp_present ? IPPROTO_UDP : 0x1b;
	}

	if (ip_version == 4) {
		header->ip4.ver = 4; /* version 4 */
		header->ip4.hdr_len = 5; /* 5 words */
		header->ip4.tot_len =
			cpu_to_be16(IB_IP4_BYTES   +
				     udp_bytes     +
				     IB_BTH_BYTES  +
				     IB_DETH_BYTES +
				     payload_bytes +
				     4);     /* ICRC     */
		header->ip4.protocol = IPPROTO_UDP;
	}
	if (udp_present && ip_version)
		header->udp.length =
			cpu_to_be16(IB_UDP_BYTES   +
				     IB_BTH_BYTES  +
				     IB_DETH_BYTES +
				     payload_bytes +
				     4);     /* ICRC     */

	if (immediate_present)
		header->bth.opcode           = IB_OPCODE_UD_SEND_ONLY_WITH_IMMEDIATE;
	else
		header->bth.opcode           = IB_OPCODE_UD_SEND_ONLY;
	header->bth.pad_count                = (4 - payload_bytes) & 3;
	header->bth.transport_header_version = 0;

	header->lrh_present = lrh_present;
	header->eth_present = eth_present;
	header->vlan_present = vlan_present;
	header->grh_present = grh_present || (ip_version == 6);
	header->ipv4_present = ip_version == 4;
	header->udp_present = udp_present;
	header->immediate_present = immediate_present;
	return 0;
}
EXPORT_SYMBOL(ib_ud_header_init);

/**
 * ib_ud_header_pack - Pack UD header struct into wire format
 * @header:UD header struct
 * @buf:Buffer to pack into
 *
 * ib_ud_header_pack() packs the UD header structure @header into wire
 * format in the buffer @buf.
 */
int ib_ud_header_pack(struct ib_ud_header *header,
		      void                *buf)
{
	int len = 0;

	if (header->lrh_present) {
		ib_pack(lrh_table, ARRAY_SIZE(lrh_table),
			&header->lrh, buf + len);
		len += IB_LRH_BYTES;
	}
	if (header->eth_present) {
		ib_pack(eth_table, ARRAY_SIZE(eth_table),
			&header->eth, buf + len);
		len += IB_ETH_BYTES;
	}
	if (header->vlan_present) {
		ib_pack(vlan_table, ARRAY_SIZE(vlan_table),
			&header->vlan, buf + len);
		len += IB_VLAN_BYTES;
	}
	if (header->grh_present) {
		ib_pack(grh_table, ARRAY_SIZE(grh_table),
			&header->grh, buf + len);
		len += IB_GRH_BYTES;
	}
	if (header->ipv4_present) {
		ib_pack(ip4_table, ARRAY_SIZE(ip4_table),
			&header->ip4, buf + len);
		len += IB_IP4_BYTES;
	}
	if (header->udp_present) {
		ib_pack(udp_table, ARRAY_SIZE(udp_table),
			&header->udp, buf + len);
		len += IB_UDP_BYTES;
	}

	ib_pack(bth_table, ARRAY_SIZE(bth_table),
		&header->bth, buf + len);
	len += IB_BTH_BYTES;

	ib_pack(deth_table, ARRAY_SIZE(deth_table),
		&header->deth, buf + len);
	len += IB_DETH_BYTES;

	if (header->immediate_present) {
		memcpy(buf + len, &header->immediate_data, sizeof header->immediate_data);
		len += sizeof header->immediate_data;
	}

	return len;
}
EXPORT_SYMBOL(ib_ud_header_pack);

/**
 * ib_ud_header_unpack - Unpack UD header struct from wire format
 * @header:UD header struct
 * @buf:Buffer to pack into
 *
 * ib_ud_header_pack() unpacks the UD header structure @header from wire
 * format in the buffer @buf.
 */
int ib_ud_header_unpack(void                *buf,
			struct ib_ud_header *header)
{
	ib_unpack(lrh_table, ARRAY_SIZE(lrh_table),
		  buf, &header->lrh);
	buf += IB_LRH_BYTES;

	if (header->lrh.link_version != 0) {
		pr_warn("Invalid LRH.link_version %d\n",
			header->lrh.link_version);
		return -EINVAL;
	}

	switch (header->lrh.link_next_header) {
	case IB_LNH_IBA_LOCAL:
		header->grh_present = 0;
		break;

	case IB_LNH_IBA_GLOBAL:
		header->grh_present = 1;
		ib_unpack(grh_table, ARRAY_SIZE(grh_table),
			  buf, &header->grh);
		buf += IB_GRH_BYTES;

		if (header->grh.ip_version != 6) {
			pr_warn("Invalid GRH.ip_version %d\n",
				header->grh.ip_version);
			return -EINVAL;
		}
		if (header->grh.next_header != 0x1b) {
			pr_warn("Invalid GRH.next_header 0x%02x\n",
				header->grh.next_header);
			return -EINVAL;
		}
		break;

	default:
		pr_warn("Invalid LRH.link_next_header %d\n",
			header->lrh.link_next_header);
		return -EINVAL;
	}

	ib_unpack(bth_table, ARRAY_SIZE(bth_table),
		  buf, &header->bth);
	buf += IB_BTH_BYTES;

	switch (header->bth.opcode) {
	case IB_OPCODE_UD_SEND_ONLY:
		header->immediate_present = 0;
		break;
	case IB_OPCODE_UD_SEND_ONLY_WITH_IMMEDIATE:
		header->immediate_present = 1;
		break;
	default:
		pr_warn("Invalid BTH.opcode 0x%02x\n", header->bth.opcode);
		return -EINVAL;
	}

	if (header->bth.transport_header_version != 0) {
		pr_warn("Invalid BTH.transport_header_version %d\n",
			header->bth.transport_header_version);
		return -EINVAL;
	}

	ib_unpack(deth_table, ARRAY_SIZE(deth_table),
		  buf, &header->deth);
	buf += IB_DETH_BYTES;

	if (header->immediate_present)
		memcpy(&header->immediate_data, buf, sizeof header->immediate_data);

	return 0;
}
EXPORT_SYMBOL(ib_ud_header_unpack);
