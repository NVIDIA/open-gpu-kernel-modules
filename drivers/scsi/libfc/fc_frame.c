// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright(c) 2007 Intel Corporation. All rights reserved.
 *
 * Maintained at www.Open-FCoE.org
 */

/*
 * Frame allocation.
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/skbuff.h>
#include <linux/crc32.h>
#include <linux/gfp.h>

#include <scsi/fc_frame.h>

/*
 * Check the CRC in a frame.
 */
u32 fc_frame_crc_check(struct fc_frame *fp)
{
	u32 crc;
	u32 error;
	const u8 *bp;
	unsigned int len;

	WARN_ON(!fc_frame_is_linear(fp));
	fr_flags(fp) &= ~FCPHF_CRC_UNCHECKED;
	len = (fr_len(fp) + 3) & ~3;	/* round up length to include fill */
	bp = (const u8 *) fr_hdr(fp);
	crc = ~crc32(~0, bp, len);
	error = crc ^ fr_crc(fp);
	return error;
}
EXPORT_SYMBOL(fc_frame_crc_check);

/*
 * Allocate a frame intended to be sent.
 * Get an sk_buff for the frame and set the length.
 */
struct fc_frame *_fc_frame_alloc(size_t len)
{
	struct fc_frame *fp;
	struct sk_buff *skb;

	WARN_ON((len % sizeof(u32)) != 0);
	len += sizeof(struct fc_frame_header);
	skb = alloc_skb_fclone(len + FC_FRAME_HEADROOM + FC_FRAME_TAILROOM +
			       NET_SKB_PAD, GFP_ATOMIC);
	if (!skb)
		return NULL;
	skb_reserve(skb, NET_SKB_PAD + FC_FRAME_HEADROOM);
	fp = (struct fc_frame *) skb;
	fc_frame_init(fp);
	skb_put(skb, len);
	return fp;
}
EXPORT_SYMBOL(_fc_frame_alloc);

struct fc_frame *fc_frame_alloc_fill(struct fc_lport *lp, size_t payload_len)
{
	struct fc_frame *fp;
	size_t fill;

	fill = payload_len % 4;
	if (fill != 0)
		fill = 4 - fill;
	fp = _fc_frame_alloc(payload_len + fill);
	if (fp) {
		memset((char *) fr_hdr(fp) + payload_len, 0, fill);
		/* trim is OK, we just allocated it so there are no fragments */
		skb_trim(fp_skb(fp),
			 payload_len + sizeof(struct fc_frame_header));
	}
	return fp;
}
EXPORT_SYMBOL(fc_frame_alloc_fill);
