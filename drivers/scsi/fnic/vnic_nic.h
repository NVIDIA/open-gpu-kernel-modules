/*
 * Copyright 2008 Cisco Systems, Inc.  All rights reserved.
 * Copyright 2007 Nuova Systems, Inc.  All rights reserved.
 *
 * This program is free software; you may redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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
#ifndef _VNIC_NIC_H_
#define _VNIC_NIC_H_

/*
 * These defines avoid symbol clash between fnic and enic (Cisco 10G Eth
 * Driver) when both are built with CONFIG options =y
 */
#define vnic_set_nic_cfg fnic_set_nic_cfg

#define NIC_CFG_RSS_DEFAULT_CPU_MASK_FIELD	0xffUL
#define NIC_CFG_RSS_DEFAULT_CPU_SHIFT		0
#define NIC_CFG_RSS_HASH_TYPE			(0xffUL << 8)
#define NIC_CFG_RSS_HASH_TYPE_MASK_FIELD	0xffUL
#define NIC_CFG_RSS_HASH_TYPE_SHIFT		8
#define NIC_CFG_RSS_HASH_BITS			(7UL << 16)
#define NIC_CFG_RSS_HASH_BITS_MASK_FIELD	7UL
#define NIC_CFG_RSS_HASH_BITS_SHIFT		16
#define NIC_CFG_RSS_BASE_CPU			(7UL << 19)
#define NIC_CFG_RSS_BASE_CPU_MASK_FIELD		7UL
#define NIC_CFG_RSS_BASE_CPU_SHIFT		19
#define NIC_CFG_RSS_ENABLE			(1UL << 22)
#define NIC_CFG_RSS_ENABLE_MASK_FIELD		1UL
#define NIC_CFG_RSS_ENABLE_SHIFT		22
#define NIC_CFG_TSO_IPID_SPLIT_EN		(1UL << 23)
#define NIC_CFG_TSO_IPID_SPLIT_EN_MASK_FIELD	1UL
#define NIC_CFG_TSO_IPID_SPLIT_EN_SHIFT		23
#define NIC_CFG_IG_VLAN_STRIP_EN		(1UL << 24)
#define NIC_CFG_IG_VLAN_STRIP_EN_MASK_FIELD	1UL
#define NIC_CFG_IG_VLAN_STRIP_EN_SHIFT		24

static inline void vnic_set_nic_cfg(u32 *nic_cfg,
	u8 rss_default_cpu, u8 rss_hash_type,
	u8 rss_hash_bits, u8 rss_base_cpu,
	u8 rss_enable, u8 tso_ipid_split_en,
	u8 ig_vlan_strip_en)
{
	*nic_cfg = (rss_default_cpu & NIC_CFG_RSS_DEFAULT_CPU_MASK_FIELD) |
		((rss_hash_type & NIC_CFG_RSS_HASH_TYPE_MASK_FIELD)
			<< NIC_CFG_RSS_HASH_TYPE_SHIFT) |
		((rss_hash_bits & NIC_CFG_RSS_HASH_BITS_MASK_FIELD)
			<< NIC_CFG_RSS_HASH_BITS_SHIFT) |
		((rss_base_cpu & NIC_CFG_RSS_BASE_CPU_MASK_FIELD)
			<< NIC_CFG_RSS_BASE_CPU_SHIFT) |
		((rss_enable & NIC_CFG_RSS_ENABLE_MASK_FIELD)
			<< NIC_CFG_RSS_ENABLE_SHIFT) |
		((tso_ipid_split_en & NIC_CFG_TSO_IPID_SPLIT_EN_MASK_FIELD)
			<< NIC_CFG_TSO_IPID_SPLIT_EN_SHIFT) |
		((ig_vlan_strip_en & NIC_CFG_IG_VLAN_STRIP_EN_MASK_FIELD)
			<< NIC_CFG_IG_VLAN_STRIP_EN_SHIFT);
}

#endif /* _VNIC_NIC_H_ */
