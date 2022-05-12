// SPDX-License-Identifier: (GPL-2.0-only OR BSD-2-Clause)
/* Copyright (C) 2018 Netronome Systems, Inc. */

#include <linux/bitfield.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/types.h>

#include "nfp_net_ctrl.h"
#include "nfp_net.h"

static void nfp_net_tlv_caps_reset(struct nfp_net_tlv_caps *caps)
{
	memset(caps, 0, sizeof(*caps));
	caps->me_freq_mhz = 1200;
	caps->mbox_off = NFP_NET_CFG_MBOX_BASE;
	caps->mbox_len = NFP_NET_CFG_MBOX_VAL_MAX_SZ;
}

static bool
nfp_net_tls_parse_crypto_ops(struct device *dev, struct nfp_net_tlv_caps *caps,
			     u8 __iomem *ctrl_mem, u8 __iomem *data,
			     unsigned int length, unsigned int offset,
			     bool rx_stream_scan)
{
	/* Ignore the legacy TLV if new one was already parsed */
	if (caps->tls_resync_ss && !rx_stream_scan)
		return true;

	if (length < 32) {
		dev_err(dev,
			"CRYPTO OPS TLV should be at least 32B, is %dB offset:%u\n",
			length, offset);
		return false;
	}

	caps->crypto_ops = readl(data);
	caps->crypto_enable_off = data - ctrl_mem + 16;
	caps->tls_resync_ss = rx_stream_scan;

	return true;
}

int nfp_net_tlv_caps_parse(struct device *dev, u8 __iomem *ctrl_mem,
			   struct nfp_net_tlv_caps *caps)
{
	u8 __iomem *data = ctrl_mem + NFP_NET_CFG_TLV_BASE;
	u8 __iomem *end = ctrl_mem + NFP_NET_CFG_BAR_SZ;
	u32 hdr;

	nfp_net_tlv_caps_reset(caps);

	hdr = readl(data);
	if (!hdr)
		return 0;

	while (true) {
		unsigned int length, offset;
		u32 hdr = readl(data);

		length = FIELD_GET(NFP_NET_CFG_TLV_HEADER_LENGTH, hdr);
		offset = data - ctrl_mem;

		/* Advance past the header */
		data += 4;

		if (length % NFP_NET_CFG_TLV_LENGTH_INC) {
			dev_err(dev, "TLV size not multiple of %u offset:%u len:%u\n",
				NFP_NET_CFG_TLV_LENGTH_INC, offset, length);
			return -EINVAL;
		}
		if (data + length > end) {
			dev_err(dev, "oversized TLV offset:%u len:%u\n",
				offset, length);
			return -EINVAL;
		}

		switch (FIELD_GET(NFP_NET_CFG_TLV_HEADER_TYPE, hdr)) {
		case NFP_NET_CFG_TLV_TYPE_UNKNOWN:
			dev_err(dev, "NULL TLV at offset:%u\n", offset);
			return -EINVAL;
		case NFP_NET_CFG_TLV_TYPE_RESERVED:
			break;
		case NFP_NET_CFG_TLV_TYPE_END:
			if (!length)
				return 0;

			dev_err(dev, "END TLV should be empty, has offset:%u len:%d\n",
				offset, length);
			return -EINVAL;
		case NFP_NET_CFG_TLV_TYPE_ME_FREQ:
			if (length != 4) {
				dev_err(dev,
					"ME FREQ TLV should be 4B, is %dB offset:%u\n",
					length, offset);
				return -EINVAL;
			}

			caps->me_freq_mhz = readl(data);
			break;
		case NFP_NET_CFG_TLV_TYPE_MBOX:
			if (!length) {
				caps->mbox_off = 0;
				caps->mbox_len = 0;
			} else {
				caps->mbox_off = data - ctrl_mem;
				caps->mbox_len = length;
			}
			break;
		case NFP_NET_CFG_TLV_TYPE_EXPERIMENTAL0:
		case NFP_NET_CFG_TLV_TYPE_EXPERIMENTAL1:
			dev_warn(dev,
				 "experimental TLV type:%u offset:%u len:%u\n",
				 FIELD_GET(NFP_NET_CFG_TLV_HEADER_TYPE, hdr),
				 offset, length);
			break;
		case NFP_NET_CFG_TLV_TYPE_REPR_CAP:
			if (length < 4) {
				dev_err(dev, "REPR CAP TLV short %dB < 4B offset:%u\n",
					length, offset);
				return -EINVAL;
			}

			caps->repr_cap = readl(data);
			break;
		case NFP_NET_CFG_TLV_TYPE_MBOX_CMSG_TYPES:
			if (length >= 4)
				caps->mbox_cmsg_types = readl(data);
			break;
		case NFP_NET_CFG_TLV_TYPE_CRYPTO_OPS:
			if (!nfp_net_tls_parse_crypto_ops(dev, caps, ctrl_mem,
							  data, length, offset,
							  false))
				return -EINVAL;
			break;
		case NFP_NET_CFG_TLV_TYPE_VNIC_STATS:
			if ((data - ctrl_mem) % 8) {
				dev_warn(dev, "VNIC STATS TLV misaligned, ignoring offset:%u len:%u\n",
					 offset, length);
				break;
			}
			caps->vnic_stats_off = data - ctrl_mem;
			caps->vnic_stats_cnt = length / 10;
			break;
		case NFP_NET_CFG_TLV_TYPE_CRYPTO_OPS_RX_SCAN:
			if (!nfp_net_tls_parse_crypto_ops(dev, caps, ctrl_mem,
							  data, length, offset,
							  true))
				return -EINVAL;
			break;
		default:
			if (!FIELD_GET(NFP_NET_CFG_TLV_HEADER_REQUIRED, hdr))
				break;

			dev_err(dev, "unknown TLV type:%u offset:%u len:%u\n",
				FIELD_GET(NFP_NET_CFG_TLV_HEADER_TYPE, hdr),
				offset, length);
			return -EINVAL;
		}

		data += length;
		if (data + 4 > end) {
			dev_err(dev, "reached end of BAR without END TLV\n");
			return -EINVAL;
		}
	}

	/* Not reached */
	return -EINVAL;
}
