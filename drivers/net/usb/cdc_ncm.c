/*
 * cdc_ncm.c
 *
 * Copyright (C) ST-Ericsson 2010-2012
 * Contact: Alexey Orishko <alexey.orishko@stericsson.com>
 * Original author: Hans Petter Selasky <hans.petter.selasky@stericsson.com>
 *
 * USB Host Driver for Network Control Model (NCM)
 * http://www.usb.org/developers/docs/devclass_docs/NCM10_012011.zip
 *
 * The NCM encoding, decoding and initialization logic
 * derives from FreeBSD 8.x. if_cdce.c and if_cdcereg.h
 *
 * This software is available to you under a choice of one of two
 * licenses. You may choose this file to be licensed under the terms
 * of the GNU General Public License (GPL) Version 2 or the 2-clause
 * BSD license listed below:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/ctype.h>
#include <linux/etherdevice.h>
#include <linux/ethtool.h>
#include <linux/workqueue.h>
#include <linux/mii.h>
#include <linux/crc32.h>
#include <linux/usb.h>
#include <linux/hrtimer.h>
#include <linux/atomic.h>
#include <linux/usb/usbnet.h>
#include <linux/usb/cdc.h>
#include <linux/usb/cdc_ncm.h>

#if IS_ENABLED(CONFIG_USB_NET_CDC_MBIM)
static bool prefer_mbim = true;
#else
static bool prefer_mbim;
#endif
module_param(prefer_mbim, bool, 0644);
MODULE_PARM_DESC(prefer_mbim, "Prefer MBIM setting on dual NCM/MBIM functions");

static void cdc_ncm_txpath_bh(struct tasklet_struct *t);
static void cdc_ncm_tx_timeout_start(struct cdc_ncm_ctx *ctx);
static enum hrtimer_restart cdc_ncm_tx_timer_cb(struct hrtimer *hr_timer);
static struct usb_driver cdc_ncm_driver;

struct cdc_ncm_stats {
	char stat_string[ETH_GSTRING_LEN];
	int sizeof_stat;
	int stat_offset;
};

#define CDC_NCM_STAT(str, m) { \
		.stat_string = str, \
		.sizeof_stat = sizeof(((struct cdc_ncm_ctx *)0)->m), \
		.stat_offset = offsetof(struct cdc_ncm_ctx, m) }
#define CDC_NCM_SIMPLE_STAT(m)	CDC_NCM_STAT(__stringify(m), m)

static const struct cdc_ncm_stats cdc_ncm_gstrings_stats[] = {
	CDC_NCM_SIMPLE_STAT(tx_reason_ntb_full),
	CDC_NCM_SIMPLE_STAT(tx_reason_ndp_full),
	CDC_NCM_SIMPLE_STAT(tx_reason_timeout),
	CDC_NCM_SIMPLE_STAT(tx_reason_max_datagram),
	CDC_NCM_SIMPLE_STAT(tx_overhead),
	CDC_NCM_SIMPLE_STAT(tx_ntbs),
	CDC_NCM_SIMPLE_STAT(rx_overhead),
	CDC_NCM_SIMPLE_STAT(rx_ntbs),
};

#define CDC_NCM_LOW_MEM_MAX_CNT 10

static int cdc_ncm_get_sset_count(struct net_device __always_unused *netdev, int sset)
{
	switch (sset) {
	case ETH_SS_STATS:
		return ARRAY_SIZE(cdc_ncm_gstrings_stats);
	default:
		return -EOPNOTSUPP;
	}
}

static void cdc_ncm_get_ethtool_stats(struct net_device *netdev,
				    struct ethtool_stats __always_unused *stats,
				    u64 *data)
{
	struct usbnet *dev = netdev_priv(netdev);
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];
	int i;
	char *p = NULL;

	for (i = 0; i < ARRAY_SIZE(cdc_ncm_gstrings_stats); i++) {
		p = (char *)ctx + cdc_ncm_gstrings_stats[i].stat_offset;
		data[i] = (cdc_ncm_gstrings_stats[i].sizeof_stat == sizeof(u64)) ? *(u64 *)p : *(u32 *)p;
	}
}

static void cdc_ncm_get_strings(struct net_device __always_unused *netdev, u32 stringset, u8 *data)
{
	u8 *p = data;
	int i;

	switch (stringset) {
	case ETH_SS_STATS:
		for (i = 0; i < ARRAY_SIZE(cdc_ncm_gstrings_stats); i++) {
			memcpy(p, cdc_ncm_gstrings_stats[i].stat_string, ETH_GSTRING_LEN);
			p += ETH_GSTRING_LEN;
		}
	}
}

static void cdc_ncm_update_rxtx_max(struct usbnet *dev, u32 new_rx, u32 new_tx);

static const struct ethtool_ops cdc_ncm_ethtool_ops = {
	.get_link		= usbnet_get_link,
	.nway_reset		= usbnet_nway_reset,
	.get_drvinfo		= usbnet_get_drvinfo,
	.get_msglevel		= usbnet_get_msglevel,
	.set_msglevel		= usbnet_set_msglevel,
	.get_ts_info		= ethtool_op_get_ts_info,
	.get_sset_count		= cdc_ncm_get_sset_count,
	.get_strings		= cdc_ncm_get_strings,
	.get_ethtool_stats	= cdc_ncm_get_ethtool_stats,
	.get_link_ksettings	= usbnet_get_link_ksettings_internal,
	.set_link_ksettings	= NULL,
};

static u32 cdc_ncm_check_rx_max(struct usbnet *dev, u32 new_rx)
{
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];
	u32 val, max, min;

	/* clamp new_rx to sane values */
	min = USB_CDC_NCM_NTB_MIN_IN_SIZE;
	max = min_t(u32, CDC_NCM_NTB_MAX_SIZE_RX, le32_to_cpu(ctx->ncm_parm.dwNtbInMaxSize));

	/* dwNtbInMaxSize spec violation? Use MIN size for both limits */
	if (max < min) {
		dev_warn(&dev->intf->dev, "dwNtbInMaxSize=%u is too small. Using %u\n",
			 le32_to_cpu(ctx->ncm_parm.dwNtbInMaxSize), min);
		max = min;
	}

	val = clamp_t(u32, new_rx, min, max);
	if (val != new_rx)
		dev_dbg(&dev->intf->dev, "rx_max must be in the [%u, %u] range\n", min, max);

	return val;
}

static u32 cdc_ncm_check_tx_max(struct usbnet *dev, u32 new_tx)
{
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];
	u32 val, max, min;

	/* clamp new_tx to sane values */
	if (ctx->is_ndp16)
		min = ctx->max_datagram_size + ctx->max_ndp_size + sizeof(struct usb_cdc_ncm_nth16);
	else
		min = ctx->max_datagram_size + ctx->max_ndp_size + sizeof(struct usb_cdc_ncm_nth32);

	max = min_t(u32, CDC_NCM_NTB_MAX_SIZE_TX, le32_to_cpu(ctx->ncm_parm.dwNtbOutMaxSize));

	/* some devices set dwNtbOutMaxSize too low for the above default */
	min = min(min, max);

	val = clamp_t(u32, new_tx, min, max);
	if (val != new_tx)
		dev_dbg(&dev->intf->dev, "tx_max must be in the [%u, %u] range\n", min, max);

	return val;
}

static ssize_t cdc_ncm_show_min_tx_pkt(struct device *d, struct device_attribute *attr, char *buf)
{
	struct usbnet *dev = netdev_priv(to_net_dev(d));
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];

	return sprintf(buf, "%u\n", ctx->min_tx_pkt);
}

static ssize_t cdc_ncm_show_rx_max(struct device *d, struct device_attribute *attr, char *buf)
{
	struct usbnet *dev = netdev_priv(to_net_dev(d));
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];

	return sprintf(buf, "%u\n", ctx->rx_max);
}

static ssize_t cdc_ncm_show_tx_max(struct device *d, struct device_attribute *attr, char *buf)
{
	struct usbnet *dev = netdev_priv(to_net_dev(d));
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];

	return sprintf(buf, "%u\n", ctx->tx_max);
}

static ssize_t cdc_ncm_show_tx_timer_usecs(struct device *d, struct device_attribute *attr, char *buf)
{
	struct usbnet *dev = netdev_priv(to_net_dev(d));
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];

	return sprintf(buf, "%u\n", ctx->timer_interval / (u32)NSEC_PER_USEC);
}

static ssize_t cdc_ncm_store_min_tx_pkt(struct device *d,  struct device_attribute *attr, const char *buf, size_t len)
{
	struct usbnet *dev = netdev_priv(to_net_dev(d));
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];
	unsigned long val;

	/* no need to restrict values - anything from 0 to infinity is OK */
	if (kstrtoul(buf, 0, &val))
		return -EINVAL;

	ctx->min_tx_pkt = val;
	return len;
}

static ssize_t cdc_ncm_store_rx_max(struct device *d,  struct device_attribute *attr, const char *buf, size_t len)
{
	struct usbnet *dev = netdev_priv(to_net_dev(d));
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];
	unsigned long val;

	if (kstrtoul(buf, 0, &val) || cdc_ncm_check_rx_max(dev, val) != val)
		return -EINVAL;

	cdc_ncm_update_rxtx_max(dev, val, ctx->tx_max);
	return len;
}

static ssize_t cdc_ncm_store_tx_max(struct device *d,  struct device_attribute *attr, const char *buf, size_t len)
{
	struct usbnet *dev = netdev_priv(to_net_dev(d));
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];
	unsigned long val;

	if (kstrtoul(buf, 0, &val) || cdc_ncm_check_tx_max(dev, val) != val)
		return -EINVAL;

	cdc_ncm_update_rxtx_max(dev, ctx->rx_max, val);
	return len;
}

static ssize_t cdc_ncm_store_tx_timer_usecs(struct device *d,  struct device_attribute *attr, const char *buf, size_t len)
{
	struct usbnet *dev = netdev_priv(to_net_dev(d));
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];
	ssize_t ret;
	unsigned long val;

	ret = kstrtoul(buf, 0, &val);
	if (ret)
		return ret;
	if (val && (val < CDC_NCM_TIMER_INTERVAL_MIN || val > CDC_NCM_TIMER_INTERVAL_MAX))
		return -EINVAL;

	spin_lock_bh(&ctx->mtx);
	ctx->timer_interval = val * NSEC_PER_USEC;
	if (!ctx->timer_interval)
		ctx->tx_timer_pending = 0;
	spin_unlock_bh(&ctx->mtx);
	return len;
}

static DEVICE_ATTR(min_tx_pkt, 0644, cdc_ncm_show_min_tx_pkt, cdc_ncm_store_min_tx_pkt);
static DEVICE_ATTR(rx_max, 0644, cdc_ncm_show_rx_max, cdc_ncm_store_rx_max);
static DEVICE_ATTR(tx_max, 0644, cdc_ncm_show_tx_max, cdc_ncm_store_tx_max);
static DEVICE_ATTR(tx_timer_usecs, 0644, cdc_ncm_show_tx_timer_usecs, cdc_ncm_store_tx_timer_usecs);

static ssize_t ndp_to_end_show(struct device *d, struct device_attribute *attr, char *buf)
{
	struct usbnet *dev = netdev_priv(to_net_dev(d));
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];

	return sprintf(buf, "%c\n", ctx->drvflags & CDC_NCM_FLAG_NDP_TO_END ? 'Y' : 'N');
}

static ssize_t ndp_to_end_store(struct device *d,  struct device_attribute *attr, const char *buf, size_t len)
{
	struct usbnet *dev = netdev_priv(to_net_dev(d));
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];
	bool enable;

	if (strtobool(buf, &enable))
		return -EINVAL;

	/* no change? */
	if (enable == (ctx->drvflags & CDC_NCM_FLAG_NDP_TO_END))
		return len;

	if (enable) {
		if (ctx->is_ndp16 && !ctx->delayed_ndp16) {
			ctx->delayed_ndp16 = kzalloc(ctx->max_ndp_size, GFP_KERNEL);
			if (!ctx->delayed_ndp16)
				return -ENOMEM;
		}
		if (!ctx->is_ndp16 && !ctx->delayed_ndp32) {
			ctx->delayed_ndp32 = kzalloc(ctx->max_ndp_size, GFP_KERNEL);
			if (!ctx->delayed_ndp32)
				return -ENOMEM;
		}
	}

	/* flush pending data before changing flag */
	netif_tx_lock_bh(dev->net);
	usbnet_start_xmit(NULL, dev->net);
	spin_lock_bh(&ctx->mtx);
	if (enable)
		ctx->drvflags |= CDC_NCM_FLAG_NDP_TO_END;
	else
		ctx->drvflags &= ~CDC_NCM_FLAG_NDP_TO_END;
	spin_unlock_bh(&ctx->mtx);
	netif_tx_unlock_bh(dev->net);

	return len;
}
static DEVICE_ATTR_RW(ndp_to_end);

#define NCM_PARM_ATTR(name, format, tocpu)				\
static ssize_t cdc_ncm_show_##name(struct device *d, struct device_attribute *attr, char *buf) \
{ \
	struct usbnet *dev = netdev_priv(to_net_dev(d)); \
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0]; \
	return sprintf(buf, format "\n", tocpu(ctx->ncm_parm.name));	\
} \
static DEVICE_ATTR(name, 0444, cdc_ncm_show_##name, NULL)

NCM_PARM_ATTR(bmNtbFormatsSupported, "0x%04x", le16_to_cpu);
NCM_PARM_ATTR(dwNtbInMaxSize, "%u", le32_to_cpu);
NCM_PARM_ATTR(wNdpInDivisor, "%u", le16_to_cpu);
NCM_PARM_ATTR(wNdpInPayloadRemainder, "%u", le16_to_cpu);
NCM_PARM_ATTR(wNdpInAlignment, "%u", le16_to_cpu);
NCM_PARM_ATTR(dwNtbOutMaxSize, "%u", le32_to_cpu);
NCM_PARM_ATTR(wNdpOutDivisor, "%u", le16_to_cpu);
NCM_PARM_ATTR(wNdpOutPayloadRemainder, "%u", le16_to_cpu);
NCM_PARM_ATTR(wNdpOutAlignment, "%u", le16_to_cpu);
NCM_PARM_ATTR(wNtbOutMaxDatagrams, "%u", le16_to_cpu);

static struct attribute *cdc_ncm_sysfs_attrs[] = {
	&dev_attr_min_tx_pkt.attr,
	&dev_attr_ndp_to_end.attr,
	&dev_attr_rx_max.attr,
	&dev_attr_tx_max.attr,
	&dev_attr_tx_timer_usecs.attr,
	&dev_attr_bmNtbFormatsSupported.attr,
	&dev_attr_dwNtbInMaxSize.attr,
	&dev_attr_wNdpInDivisor.attr,
	&dev_attr_wNdpInPayloadRemainder.attr,
	&dev_attr_wNdpInAlignment.attr,
	&dev_attr_dwNtbOutMaxSize.attr,
	&dev_attr_wNdpOutDivisor.attr,
	&dev_attr_wNdpOutPayloadRemainder.attr,
	&dev_attr_wNdpOutAlignment.attr,
	&dev_attr_wNtbOutMaxDatagrams.attr,
	NULL,
};

static const struct attribute_group cdc_ncm_sysfs_attr_group = {
	.name = "cdc_ncm",
	.attrs = cdc_ncm_sysfs_attrs,
};

/* handle rx_max and tx_max changes */
static void cdc_ncm_update_rxtx_max(struct usbnet *dev, u32 new_rx, u32 new_tx)
{
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];
	u8 iface_no = ctx->control->cur_altsetting->desc.bInterfaceNumber;
	u32 val;

	val = cdc_ncm_check_rx_max(dev, new_rx);

	/* inform device about NTB input size changes */
	if (val != ctx->rx_max) {
		__le32 dwNtbInMaxSize = cpu_to_le32(val);

		dev_info(&dev->intf->dev, "setting rx_max = %u\n", val);

		/* tell device to use new size */
		if (usbnet_write_cmd(dev, USB_CDC_SET_NTB_INPUT_SIZE,
				     USB_TYPE_CLASS | USB_DIR_OUT
				     | USB_RECIP_INTERFACE,
				     0, iface_no, &dwNtbInMaxSize, 4) < 0)
			dev_dbg(&dev->intf->dev, "Setting NTB Input Size failed\n");
		else
			ctx->rx_max = val;
	}

	/* usbnet use these values for sizing rx queues */
	if (dev->rx_urb_size != ctx->rx_max) {
		dev->rx_urb_size = ctx->rx_max;
		if (netif_running(dev->net))
			usbnet_unlink_rx_urbs(dev);
	}

	val = cdc_ncm_check_tx_max(dev, new_tx);
	if (val != ctx->tx_max)
		dev_info(&dev->intf->dev, "setting tx_max = %u\n", val);

	/* Adding a pad byte here if necessary simplifies the handling
	 * in cdc_ncm_fill_tx_frame, making tx_max always represent
	 * the real skb max size.
	 *
	 * We cannot use dev->maxpacket here because this is called from
	 * .bind which is called before usbnet sets up dev->maxpacket
	 */
	if (val != le32_to_cpu(ctx->ncm_parm.dwNtbOutMaxSize) &&
	    val % usb_maxpacket(dev->udev, dev->out, 1) == 0)
		val++;

	/* we might need to flush any pending tx buffers if running */
	if (netif_running(dev->net) && val > ctx->tx_max) {
		netif_tx_lock_bh(dev->net);
		usbnet_start_xmit(NULL, dev->net);
		/* make sure tx_curr_skb is reallocated if it was empty */
		if (ctx->tx_curr_skb) {
			dev_kfree_skb_any(ctx->tx_curr_skb);
			ctx->tx_curr_skb = NULL;
		}
		ctx->tx_max = val;
		netif_tx_unlock_bh(dev->net);
	} else {
		ctx->tx_max = val;
	}

	dev->hard_mtu = ctx->tx_max;

	/* max qlen depend on hard_mtu and rx_urb_size */
	usbnet_update_max_qlen(dev);

	/* never pad more than 3 full USB packets per transfer */
	ctx->min_tx_pkt = clamp_t(u16, ctx->tx_max - 3 * usb_maxpacket(dev->udev, dev->out, 1),
				  CDC_NCM_MIN_TX_PKT, ctx->tx_max);
}

/* helpers for NCM and MBIM differences */
static u8 cdc_ncm_flags(struct usbnet *dev)
{
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];

	if (cdc_ncm_comm_intf_is_mbim(dev->intf->cur_altsetting) && ctx->mbim_desc)
		return ctx->mbim_desc->bmNetworkCapabilities;
	if (ctx->func_desc)
		return ctx->func_desc->bmNetworkCapabilities;
	return 0;
}

static int cdc_ncm_eth_hlen(struct usbnet *dev)
{
	if (cdc_ncm_comm_intf_is_mbim(dev->intf->cur_altsetting))
		return 0;
	return ETH_HLEN;
}

static u32 cdc_ncm_min_dgram_size(struct usbnet *dev)
{
	if (cdc_ncm_comm_intf_is_mbim(dev->intf->cur_altsetting))
		return CDC_MBIM_MIN_DATAGRAM_SIZE;
	return CDC_NCM_MIN_DATAGRAM_SIZE;
}

static u32 cdc_ncm_max_dgram_size(struct usbnet *dev)
{
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];

	if (cdc_ncm_comm_intf_is_mbim(dev->intf->cur_altsetting) && ctx->mbim_desc)
		return le16_to_cpu(ctx->mbim_desc->wMaxSegmentSize);
	if (ctx->ether_desc)
		return le16_to_cpu(ctx->ether_desc->wMaxSegmentSize);
	return CDC_NCM_MAX_DATAGRAM_SIZE;
}

/* initial one-time device setup.  MUST be called with the data interface
 * in altsetting 0
 */
static int cdc_ncm_init(struct usbnet *dev)
{
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];
	u8 iface_no = ctx->control->cur_altsetting->desc.bInterfaceNumber;
	int err;

	err = usbnet_read_cmd(dev, USB_CDC_GET_NTB_PARAMETERS,
			      USB_TYPE_CLASS | USB_DIR_IN
			      |USB_RECIP_INTERFACE,
			      0, iface_no, &ctx->ncm_parm,
			      sizeof(ctx->ncm_parm));
	if (err < 0) {
		dev_err(&dev->intf->dev, "failed GET_NTB_PARAMETERS\n");
		return err; /* GET_NTB_PARAMETERS is required */
	}

	/* set CRC Mode */
	if (cdc_ncm_flags(dev) & USB_CDC_NCM_NCAP_CRC_MODE) {
		dev_dbg(&dev->intf->dev, "Setting CRC mode off\n");
		err = usbnet_write_cmd(dev, USB_CDC_SET_CRC_MODE,
				       USB_TYPE_CLASS | USB_DIR_OUT
				       | USB_RECIP_INTERFACE,
				       USB_CDC_NCM_CRC_NOT_APPENDED,
				       iface_no, NULL, 0);
		if (err < 0)
			dev_err(&dev->intf->dev, "SET_CRC_MODE failed\n");
	}

	/* use ndp16 by default */
	ctx->is_ndp16 = 1;

	/* set NTB format, if both formats are supported.
	 *
	 * "The host shall only send this command while the NCM Data
	 *  Interface is in alternate setting 0."
	 */
	if (le16_to_cpu(ctx->ncm_parm.bmNtbFormatsSupported) &
						USB_CDC_NCM_NTB32_SUPPORTED) {
		if (ctx->drvflags & CDC_NCM_FLAG_PREFER_NTB32) {
			ctx->is_ndp16 = 0;
			dev_dbg(&dev->intf->dev, "Setting NTB format to 32-bit\n");
			err = usbnet_write_cmd(dev, USB_CDC_SET_NTB_FORMAT,
					       USB_TYPE_CLASS | USB_DIR_OUT
					       | USB_RECIP_INTERFACE,
					       USB_CDC_NCM_NTB32_FORMAT,
					       iface_no, NULL, 0);
		} else {
			ctx->is_ndp16 = 1;
			dev_dbg(&dev->intf->dev, "Setting NTB format to 16-bit\n");
			err = usbnet_write_cmd(dev, USB_CDC_SET_NTB_FORMAT,
					       USB_TYPE_CLASS | USB_DIR_OUT
					       | USB_RECIP_INTERFACE,
					       USB_CDC_NCM_NTB16_FORMAT,
					       iface_no, NULL, 0);
		}
		if (err < 0) {
			ctx->is_ndp16 = 1;
			dev_err(&dev->intf->dev, "SET_NTB_FORMAT failed\n");
		}
	}

	/* set initial device values */
	ctx->rx_max = le32_to_cpu(ctx->ncm_parm.dwNtbInMaxSize);
	ctx->tx_max = le32_to_cpu(ctx->ncm_parm.dwNtbOutMaxSize);
	ctx->tx_remainder = le16_to_cpu(ctx->ncm_parm.wNdpOutPayloadRemainder);
	ctx->tx_modulus = le16_to_cpu(ctx->ncm_parm.wNdpOutDivisor);
	ctx->tx_ndp_modulus = le16_to_cpu(ctx->ncm_parm.wNdpOutAlignment);
	/* devices prior to NCM Errata shall set this field to zero */
	ctx->tx_max_datagrams = le16_to_cpu(ctx->ncm_parm.wNtbOutMaxDatagrams);

	dev_dbg(&dev->intf->dev,
		"dwNtbInMaxSize=%u dwNtbOutMaxSize=%u wNdpOutPayloadRemainder=%u wNdpOutDivisor=%u wNdpOutAlignment=%u wNtbOutMaxDatagrams=%u flags=0x%x\n",
		ctx->rx_max, ctx->tx_max, ctx->tx_remainder, ctx->tx_modulus,
		ctx->tx_ndp_modulus, ctx->tx_max_datagrams, cdc_ncm_flags(dev));

	/* max count of tx datagrams */
	if ((ctx->tx_max_datagrams == 0) ||
			(ctx->tx_max_datagrams > CDC_NCM_DPT_DATAGRAMS_MAX))
		ctx->tx_max_datagrams = CDC_NCM_DPT_DATAGRAMS_MAX;

	/* set up maximum NDP size */
	if (ctx->is_ndp16)
		ctx->max_ndp_size = sizeof(struct usb_cdc_ncm_ndp16) + (ctx->tx_max_datagrams + 1) * sizeof(struct usb_cdc_ncm_dpe16);
	else
		ctx->max_ndp_size = sizeof(struct usb_cdc_ncm_ndp32) + (ctx->tx_max_datagrams + 1) * sizeof(struct usb_cdc_ncm_dpe32);

	/* initial coalescing timer interval */
	ctx->timer_interval = CDC_NCM_TIMER_INTERVAL_USEC * NSEC_PER_USEC;

	return 0;
}

/* set a new max datagram size */
static void cdc_ncm_set_dgram_size(struct usbnet *dev, int new_size)
{
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];
	u8 iface_no = ctx->control->cur_altsetting->desc.bInterfaceNumber;
	__le16 max_datagram_size;
	u16 mbim_mtu;
	int err;

	/* set default based on descriptors */
	ctx->max_datagram_size = clamp_t(u32, new_size,
					 cdc_ncm_min_dgram_size(dev),
					 CDC_NCM_MAX_DATAGRAM_SIZE);

	/* inform the device about the selected Max Datagram Size? */
	if (!(cdc_ncm_flags(dev) & USB_CDC_NCM_NCAP_MAX_DATAGRAM_SIZE))
		goto out;

	/* read current mtu value from device */
	err = usbnet_read_cmd(dev, USB_CDC_GET_MAX_DATAGRAM_SIZE,
			      USB_TYPE_CLASS | USB_DIR_IN | USB_RECIP_INTERFACE,
			      0, iface_no, &max_datagram_size, sizeof(max_datagram_size));
	if (err != sizeof(max_datagram_size)) {
		dev_dbg(&dev->intf->dev, "GET_MAX_DATAGRAM_SIZE failed\n");
		goto out;
	}

	if (le16_to_cpu(max_datagram_size) == ctx->max_datagram_size)
		goto out;

	max_datagram_size = cpu_to_le16(ctx->max_datagram_size);
	err = usbnet_write_cmd(dev, USB_CDC_SET_MAX_DATAGRAM_SIZE,
			       USB_TYPE_CLASS | USB_DIR_OUT | USB_RECIP_INTERFACE,
			       0, iface_no, &max_datagram_size, sizeof(max_datagram_size));
	if (err < 0)
		dev_dbg(&dev->intf->dev, "SET_MAX_DATAGRAM_SIZE failed\n");

out:
	/* set MTU to max supported by the device if necessary */
	dev->net->mtu = min_t(int, dev->net->mtu, ctx->max_datagram_size - cdc_ncm_eth_hlen(dev));

	/* do not exceed operater preferred MTU */
	if (ctx->mbim_extended_desc) {
		mbim_mtu = le16_to_cpu(ctx->mbim_extended_desc->wMTU);
		if (mbim_mtu != 0 && mbim_mtu < dev->net->mtu)
			dev->net->mtu = mbim_mtu;
	}
}

static void cdc_ncm_fix_modulus(struct usbnet *dev)
{
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];
	u32 val;

	/*
	 * verify that the structure alignment is:
	 * - power of two
	 * - not greater than the maximum transmit length
	 * - not less than four bytes
	 */
	val = ctx->tx_ndp_modulus;

	if ((val < USB_CDC_NCM_NDP_ALIGN_MIN_SIZE) ||
	    (val != ((-val) & val)) || (val >= ctx->tx_max)) {
		dev_dbg(&dev->intf->dev, "Using default alignment: 4 bytes\n");
		ctx->tx_ndp_modulus = USB_CDC_NCM_NDP_ALIGN_MIN_SIZE;
	}

	/*
	 * verify that the payload alignment is:
	 * - power of two
	 * - not greater than the maximum transmit length
	 * - not less than four bytes
	 */
	val = ctx->tx_modulus;

	if ((val < USB_CDC_NCM_NDP_ALIGN_MIN_SIZE) ||
	    (val != ((-val) & val)) || (val >= ctx->tx_max)) {
		dev_dbg(&dev->intf->dev, "Using default transmit modulus: 4 bytes\n");
		ctx->tx_modulus = USB_CDC_NCM_NDP_ALIGN_MIN_SIZE;
	}

	/* verify the payload remainder */
	if (ctx->tx_remainder >= ctx->tx_modulus) {
		dev_dbg(&dev->intf->dev, "Using default transmit remainder: 0 bytes\n");
		ctx->tx_remainder = 0;
	}

	/* adjust TX-remainder according to NCM specification. */
	ctx->tx_remainder = ((ctx->tx_remainder - cdc_ncm_eth_hlen(dev)) &
			     (ctx->tx_modulus - 1));
}

static int cdc_ncm_setup(struct usbnet *dev)
{
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];
	u32 def_rx, def_tx;

	/* be conservative when selecting intial buffer size to
	 * increase the number of hosts this will work for
	 */
	def_rx = min_t(u32, CDC_NCM_NTB_DEF_SIZE_RX,
		       le32_to_cpu(ctx->ncm_parm.dwNtbInMaxSize));
	def_tx = min_t(u32, CDC_NCM_NTB_DEF_SIZE_TX,
		       le32_to_cpu(ctx->ncm_parm.dwNtbOutMaxSize));

	/* clamp rx_max and tx_max and inform device */
	cdc_ncm_update_rxtx_max(dev, def_rx, def_tx);

	/* sanitize the modulus and remainder values */
	cdc_ncm_fix_modulus(dev);

	/* set max datagram size */
	cdc_ncm_set_dgram_size(dev, cdc_ncm_max_dgram_size(dev));
	return 0;
}

static void
cdc_ncm_find_endpoints(struct usbnet *dev, struct usb_interface *intf)
{
	struct usb_host_endpoint *e, *in = NULL, *out = NULL;
	u8 ep;

	for (ep = 0; ep < intf->cur_altsetting->desc.bNumEndpoints; ep++) {
		e = intf->cur_altsetting->endpoint + ep;

		/* ignore endpoints which cannot transfer data */
		if (!usb_endpoint_maxp(&e->desc))
			continue;

		switch (e->desc.bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) {
		case USB_ENDPOINT_XFER_INT:
			if (usb_endpoint_dir_in(&e->desc)) {
				if (!dev->status)
					dev->status = e;
			}
			break;

		case USB_ENDPOINT_XFER_BULK:
			if (usb_endpoint_dir_in(&e->desc)) {
				if (!in)
					in = e;
			} else {
				if (!out)
					out = e;
			}
			break;

		default:
			break;
		}
	}
	if (in && !dev->in)
		dev->in = usb_rcvbulkpipe(dev->udev,
					  in->desc.bEndpointAddress &
					  USB_ENDPOINT_NUMBER_MASK);
	if (out && !dev->out)
		dev->out = usb_sndbulkpipe(dev->udev,
					   out->desc.bEndpointAddress &
					   USB_ENDPOINT_NUMBER_MASK);
}

static void cdc_ncm_free(struct cdc_ncm_ctx *ctx)
{
	if (ctx == NULL)
		return;

	if (ctx->tx_rem_skb != NULL) {
		dev_kfree_skb_any(ctx->tx_rem_skb);
		ctx->tx_rem_skb = NULL;
	}

	if (ctx->tx_curr_skb != NULL) {
		dev_kfree_skb_any(ctx->tx_curr_skb);
		ctx->tx_curr_skb = NULL;
	}

	if (ctx->is_ndp16)
		kfree(ctx->delayed_ndp16);
	else
		kfree(ctx->delayed_ndp32);

	kfree(ctx);
}

/* we need to override the usbnet change_mtu ndo for two reasons:
 *  - respect the negotiated maximum datagram size
 *  - avoid unwanted changes to rx and tx buffers
 */
int cdc_ncm_change_mtu(struct net_device *net, int new_mtu)
{
	struct usbnet *dev = netdev_priv(net);

	net->mtu = new_mtu;
	cdc_ncm_set_dgram_size(dev, new_mtu + cdc_ncm_eth_hlen(dev));

	return 0;
}
EXPORT_SYMBOL_GPL(cdc_ncm_change_mtu);

static const struct net_device_ops cdc_ncm_netdev_ops = {
	.ndo_open	     = usbnet_open,
	.ndo_stop	     = usbnet_stop,
	.ndo_start_xmit	     = usbnet_start_xmit,
	.ndo_tx_timeout	     = usbnet_tx_timeout,
	.ndo_set_rx_mode     = usbnet_set_rx_mode,
	.ndo_get_stats64     = dev_get_tstats64,
	.ndo_change_mtu	     = cdc_ncm_change_mtu,
	.ndo_set_mac_address = eth_mac_addr,
	.ndo_validate_addr   = eth_validate_addr,
};

int cdc_ncm_bind_common(struct usbnet *dev, struct usb_interface *intf, u8 data_altsetting, int drvflags)
{
	struct cdc_ncm_ctx *ctx;
	struct usb_driver *driver;
	u8 *buf;
	int len;
	int temp;
	u8 iface_no;
	struct usb_cdc_parsed_header hdr;

	ctx = kzalloc(sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	ctx->dev = dev;

	hrtimer_init(&ctx->tx_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	ctx->tx_timer.function = &cdc_ncm_tx_timer_cb;
	tasklet_setup(&ctx->bh, cdc_ncm_txpath_bh);
	atomic_set(&ctx->stop, 0);
	spin_lock_init(&ctx->mtx);

	/* store ctx pointer in device data field */
	dev->data[0] = (unsigned long)ctx;

	/* only the control interface can be successfully probed */
	ctx->control = intf;

	/* get some pointers */
	driver = driver_of(intf);
	buf = intf->cur_altsetting->extra;
	len = intf->cur_altsetting->extralen;

	/* parse through descriptors associated with control interface */
	cdc_parse_cdc_header(&hdr, intf, buf, len);

	if (hdr.usb_cdc_union_desc)
		ctx->data = usb_ifnum_to_if(dev->udev,
					    hdr.usb_cdc_union_desc->bSlaveInterface0);
	ctx->ether_desc = hdr.usb_cdc_ether_desc;
	ctx->func_desc = hdr.usb_cdc_ncm_desc;
	ctx->mbim_desc = hdr.usb_cdc_mbim_desc;
	ctx->mbim_extended_desc = hdr.usb_cdc_mbim_extended_desc;

	/* some buggy devices have an IAD but no CDC Union */
	if (!hdr.usb_cdc_union_desc && intf->intf_assoc && intf->intf_assoc->bInterfaceCount == 2) {
		ctx->data = usb_ifnum_to_if(dev->udev, intf->cur_altsetting->desc.bInterfaceNumber + 1);
		dev_dbg(&intf->dev, "CDC Union missing - got slave from IAD\n");
	}

	/* check if we got everything */
	if (!ctx->data) {
		dev_err(&intf->dev, "CDC Union missing and no IAD found\n");
		goto error;
	}
	if (cdc_ncm_comm_intf_is_mbim(intf->cur_altsetting)) {
		if (!ctx->mbim_desc) {
			dev_err(&intf->dev, "MBIM functional descriptor missing\n");
			goto error;
		}
	} else {
		if (!ctx->ether_desc || !ctx->func_desc) {
			dev_err(&intf->dev, "NCM or ECM functional descriptors missing\n");
			goto error;
		}
	}

	/* claim data interface, if different from control */
	if (ctx->data != ctx->control) {
		temp = usb_driver_claim_interface(driver, ctx->data, dev);
		if (temp) {
			dev_err(&intf->dev, "failed to claim data intf\n");
			goto error;
		}
	}

	iface_no = ctx->data->cur_altsetting->desc.bInterfaceNumber;

	/* Device-specific flags */
	ctx->drvflags = drvflags;

	/* Reset data interface. Some devices will not reset properly
	 * unless they are configured first.  Toggle the altsetting to
	 * force a reset.
	 * Some other devices do not work properly with this procedure
	 * that can be avoided using quirk CDC_MBIM_FLAG_AVOID_ALTSETTING_TOGGLE
	 */
	if (!(ctx->drvflags & CDC_MBIM_FLAG_AVOID_ALTSETTING_TOGGLE))
		usb_set_interface(dev->udev, iface_no, data_altsetting);

	temp = usb_set_interface(dev->udev, iface_no, 0);
	if (temp) {
		dev_dbg(&intf->dev, "set interface failed\n");
		goto error2;
	}

	/* initialize basic device settings */
	if (cdc_ncm_init(dev))
		goto error2;

	/* Some firmwares need a pause here or they will silently fail
	 * to set up the interface properly.  This value was decided
	 * empirically on a Sierra Wireless MC7455 running 02.08.02.00
	 * firmware.
	 */
	usleep_range(10000, 20000);

	/* configure data interface */
	temp = usb_set_interface(dev->udev, iface_no, data_altsetting);
	if (temp) {
		dev_dbg(&intf->dev, "set interface failed\n");
		goto error2;
	}

	cdc_ncm_find_endpoints(dev, ctx->data);
	cdc_ncm_find_endpoints(dev, ctx->control);
	if (!dev->in || !dev->out || !dev->status) {
		dev_dbg(&intf->dev, "failed to collect endpoints\n");
		goto error2;
	}

	usb_set_intfdata(ctx->control, dev);

	if (ctx->ether_desc) {
		temp = usbnet_get_ethernet_addr(dev, ctx->ether_desc->iMACAddress);
		if (temp) {
			dev_err(&intf->dev, "failed to get mac address\n");
			goto error2;
		}
		dev_info(&intf->dev, "MAC-Address: %pM\n", dev->net->dev_addr);
	}

	/* finish setting up the device specific data */
	cdc_ncm_setup(dev);

	/* Allocate the delayed NDP if needed. */
	if (ctx->drvflags & CDC_NCM_FLAG_NDP_TO_END) {
		if (ctx->is_ndp16) {
			ctx->delayed_ndp16 = kzalloc(ctx->max_ndp_size, GFP_KERNEL);
			if (!ctx->delayed_ndp16)
				goto error2;
		} else {
			ctx->delayed_ndp32 = kzalloc(ctx->max_ndp_size, GFP_KERNEL);
			if (!ctx->delayed_ndp32)
				goto error2;
		}
		dev_info(&intf->dev, "NDP will be placed at end of frame for this device.");
	}

	/* override ethtool_ops */
	dev->net->ethtool_ops = &cdc_ncm_ethtool_ops;

	/* add our sysfs attrs */
	dev->net->sysfs_groups[0] = &cdc_ncm_sysfs_attr_group;

	/* must handle MTU changes */
	dev->net->netdev_ops = &cdc_ncm_netdev_ops;
	dev->net->max_mtu = cdc_ncm_max_dgram_size(dev) - cdc_ncm_eth_hlen(dev);

	return 0;

error2:
	usb_set_intfdata(ctx->control, NULL);
	usb_set_intfdata(ctx->data, NULL);
	if (ctx->data != ctx->control)
		usb_driver_release_interface(driver, ctx->data);
error:
	cdc_ncm_free((struct cdc_ncm_ctx *)dev->data[0]);
	dev->data[0] = 0;
	dev_info(&intf->dev, "bind() failure\n");
	return -ENODEV;
}
EXPORT_SYMBOL_GPL(cdc_ncm_bind_common);

void cdc_ncm_unbind(struct usbnet *dev, struct usb_interface *intf)
{
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];
	struct usb_driver *driver = driver_of(intf);

	if (ctx == NULL)
		return;		/* no setup */

	atomic_set(&ctx->stop, 1);

	hrtimer_cancel(&ctx->tx_timer);

	tasklet_kill(&ctx->bh);

	/* handle devices with combined control and data interface */
	if (ctx->control == ctx->data)
		ctx->data = NULL;

	/* disconnect master --> disconnect slave */
	if (intf == ctx->control && ctx->data) {
		usb_set_intfdata(ctx->data, NULL);
		usb_driver_release_interface(driver, ctx->data);
		ctx->data = NULL;

	} else if (intf == ctx->data && ctx->control) {
		usb_set_intfdata(ctx->control, NULL);
		usb_driver_release_interface(driver, ctx->control);
		ctx->control = NULL;
	}

	usb_set_intfdata(intf, NULL);
	cdc_ncm_free(ctx);
}
EXPORT_SYMBOL_GPL(cdc_ncm_unbind);

/* Return the number of the MBIM control interface altsetting iff it
 * is preferred and available,
 */
u8 cdc_ncm_select_altsetting(struct usb_interface *intf)
{
	struct usb_host_interface *alt;

	/* The MBIM spec defines a NCM compatible default altsetting,
	 * which we may have matched:
	 *
	 *  "Functions that implement both NCM 1.0 and MBIM (an
	 *   “NCM/MBIM function”) according to this recommendation
	 *   shall provide two alternate settings for the
	 *   Communication Interface.  Alternate setting 0, and the
	 *   associated class and endpoint descriptors, shall be
	 *   constructed according to the rules given for the
	 *   Communication Interface in section 5 of [USBNCM10].
	 *   Alternate setting 1, and the associated class and
	 *   endpoint descriptors, shall be constructed according to
	 *   the rules given in section 6 (USB Device Model) of this
	 *   specification."
	 */
	if (intf->num_altsetting < 2)
		return intf->cur_altsetting->desc.bAlternateSetting;

	if (prefer_mbim) {
		alt = usb_altnum_to_altsetting(intf, CDC_NCM_COMM_ALTSETTING_MBIM);
		if (alt && cdc_ncm_comm_intf_is_mbim(alt))
			return CDC_NCM_COMM_ALTSETTING_MBIM;
	}
	return CDC_NCM_COMM_ALTSETTING_NCM;
}
EXPORT_SYMBOL_GPL(cdc_ncm_select_altsetting);

static int cdc_ncm_bind(struct usbnet *dev, struct usb_interface *intf)
{
	/* MBIM backwards compatible function? */
	if (cdc_ncm_select_altsetting(intf) != CDC_NCM_COMM_ALTSETTING_NCM)
		return -ENODEV;

	/* The NCM data altsetting is fixed, so we hard-coded it.
	 * Additionally, generic NCM devices are assumed to accept arbitrarily
	 * placed NDP.
	 */
	return cdc_ncm_bind_common(dev, intf, CDC_NCM_DATA_ALTSETTING_NCM, 0);
}

static void cdc_ncm_align_tail(struct sk_buff *skb, size_t modulus, size_t remainder, size_t max)
{
	size_t align = ALIGN(skb->len, modulus) - skb->len + remainder;

	if (skb->len + align > max)
		align = max - skb->len;
	if (align && skb_tailroom(skb) >= align)
		skb_put_zero(skb, align);
}

/* return a pointer to a valid struct usb_cdc_ncm_ndp16 of type sign, possibly
 * allocating a new one within skb
 */
static struct usb_cdc_ncm_ndp16 *cdc_ncm_ndp16(struct cdc_ncm_ctx *ctx, struct sk_buff *skb, __le32 sign, size_t reserve)
{
	struct usb_cdc_ncm_ndp16 *ndp16 = NULL;
	struct usb_cdc_ncm_nth16 *nth16 = (void *)skb->data;
	size_t ndpoffset = le16_to_cpu(nth16->wNdpIndex);

	/* If NDP should be moved to the end of the NCM package, we can't follow the
	* NTH16 header as we would normally do. NDP isn't written to the SKB yet, and
	* the wNdpIndex field in the header is actually not consistent with reality. It will be later.
	*/
	if (ctx->drvflags & CDC_NCM_FLAG_NDP_TO_END) {
		if (ctx->delayed_ndp16->dwSignature == sign)
			return ctx->delayed_ndp16;

		/* We can only push a single NDP to the end. Return
		 * NULL to send what we've already got and queue this
		 * skb for later.
		 */
		else if (ctx->delayed_ndp16->dwSignature)
			return NULL;
	}

	/* follow the chain of NDPs, looking for a match */
	while (ndpoffset) {
		ndp16 = (struct usb_cdc_ncm_ndp16 *)(skb->data + ndpoffset);
		if  (ndp16->dwSignature == sign)
			return ndp16;
		ndpoffset = le16_to_cpu(ndp16->wNextNdpIndex);
	}

	/* align new NDP */
	if (!(ctx->drvflags & CDC_NCM_FLAG_NDP_TO_END))
		cdc_ncm_align_tail(skb, ctx->tx_ndp_modulus, 0, ctx->tx_curr_size);

	/* verify that there is room for the NDP and the datagram (reserve) */
	if ((ctx->tx_curr_size - skb->len - reserve) < ctx->max_ndp_size)
		return NULL;

	/* link to it */
	if (ndp16)
		ndp16->wNextNdpIndex = cpu_to_le16(skb->len);
	else
		nth16->wNdpIndex = cpu_to_le16(skb->len);

	/* push a new empty NDP */
	if (!(ctx->drvflags & CDC_NCM_FLAG_NDP_TO_END))
		ndp16 = skb_put_zero(skb, ctx->max_ndp_size);
	else
		ndp16 = ctx->delayed_ndp16;

	ndp16->dwSignature = sign;
	ndp16->wLength = cpu_to_le16(sizeof(struct usb_cdc_ncm_ndp16) + sizeof(struct usb_cdc_ncm_dpe16));
	return ndp16;
}

static struct usb_cdc_ncm_ndp32 *cdc_ncm_ndp32(struct cdc_ncm_ctx *ctx, struct sk_buff *skb, __le32 sign, size_t reserve)
{
	struct usb_cdc_ncm_ndp32 *ndp32 = NULL;
	struct usb_cdc_ncm_nth32 *nth32 = (void *)skb->data;
	size_t ndpoffset = le32_to_cpu(nth32->dwNdpIndex);

	/* If NDP should be moved to the end of the NCM package, we can't follow the
	 * NTH32 header as we would normally do. NDP isn't written to the SKB yet, and
	 * the wNdpIndex field in the header is actually not consistent with reality. It will be later.
	 */
	if (ctx->drvflags & CDC_NCM_FLAG_NDP_TO_END) {
		if (ctx->delayed_ndp32->dwSignature == sign)
			return ctx->delayed_ndp32;

		/* We can only push a single NDP to the end. Return
		 * NULL to send what we've already got and queue this
		 * skb for later.
		 */
		else if (ctx->delayed_ndp32->dwSignature)
			return NULL;
	}

	/* follow the chain of NDPs, looking for a match */
	while (ndpoffset) {
		ndp32 = (struct usb_cdc_ncm_ndp32 *)(skb->data + ndpoffset);
		if  (ndp32->dwSignature == sign)
			return ndp32;
		ndpoffset = le32_to_cpu(ndp32->dwNextNdpIndex);
	}

	/* align new NDP */
	if (!(ctx->drvflags & CDC_NCM_FLAG_NDP_TO_END))
		cdc_ncm_align_tail(skb, ctx->tx_ndp_modulus, 0, ctx->tx_curr_size);

	/* verify that there is room for the NDP and the datagram (reserve) */
	if ((ctx->tx_curr_size - skb->len - reserve) < ctx->max_ndp_size)
		return NULL;

	/* link to it */
	if (ndp32)
		ndp32->dwNextNdpIndex = cpu_to_le32(skb->len);
	else
		nth32->dwNdpIndex = cpu_to_le32(skb->len);

	/* push a new empty NDP */
	if (!(ctx->drvflags & CDC_NCM_FLAG_NDP_TO_END))
		ndp32 = skb_put_zero(skb, ctx->max_ndp_size);
	else
		ndp32 = ctx->delayed_ndp32;

	ndp32->dwSignature = sign;
	ndp32->wLength = cpu_to_le16(sizeof(struct usb_cdc_ncm_ndp32) + sizeof(struct usb_cdc_ncm_dpe32));
	return ndp32;
}

struct sk_buff *
cdc_ncm_fill_tx_frame(struct usbnet *dev, struct sk_buff *skb, __le32 sign)
{
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];
	union {
		struct usb_cdc_ncm_nth16 *nth16;
		struct usb_cdc_ncm_nth32 *nth32;
	} nth;
	union {
		struct usb_cdc_ncm_ndp16 *ndp16;
		struct usb_cdc_ncm_ndp32 *ndp32;
	} ndp;
	struct sk_buff *skb_out;
	u16 n = 0, index, ndplen;
	u8 ready2send = 0;
	u32 delayed_ndp_size;
	size_t padding_count;

	/* When our NDP gets written in cdc_ncm_ndp(), then skb_out->len gets updated
	 * accordingly. Otherwise, we should check here.
	 */
	if (ctx->drvflags & CDC_NCM_FLAG_NDP_TO_END)
		delayed_ndp_size = ctx->max_ndp_size +
			max_t(u32,
			      ctx->tx_ndp_modulus,
			      ctx->tx_modulus + ctx->tx_remainder) - 1;
	else
		delayed_ndp_size = 0;

	/* if there is a remaining skb, it gets priority */
	if (skb != NULL) {
		swap(skb, ctx->tx_rem_skb);
		swap(sign, ctx->tx_rem_sign);
	} else {
		ready2send = 1;
	}

	/* check if we are resuming an OUT skb */
	skb_out = ctx->tx_curr_skb;

	/* allocate a new OUT skb */
	if (!skb_out) {
		if (ctx->tx_low_mem_val == 0) {
			ctx->tx_curr_size = ctx->tx_max;
			skb_out = alloc_skb(ctx->tx_curr_size, GFP_ATOMIC);
			/* If the memory allocation fails we will wait longer
			 * each time before attempting another full size
			 * allocation again to not overload the system
			 * further.
			 */
			if (skb_out == NULL) {
				ctx->tx_low_mem_max_cnt = min(ctx->tx_low_mem_max_cnt + 1,
							      (unsigned)CDC_NCM_LOW_MEM_MAX_CNT);
				ctx->tx_low_mem_val = ctx->tx_low_mem_max_cnt;
			}
		}
		if (skb_out == NULL) {
			/* See if a very small allocation is possible.
			 * We will send this packet immediately and hope
			 * that there is more memory available later.
			 */
			if (skb)
				ctx->tx_curr_size = max(skb->len,
					(u32)USB_CDC_NCM_NTB_MIN_OUT_SIZE);
			else
				ctx->tx_curr_size = USB_CDC_NCM_NTB_MIN_OUT_SIZE;
			skb_out = alloc_skb(ctx->tx_curr_size, GFP_ATOMIC);

			/* No allocation possible so we will abort */
			if (skb_out == NULL) {
				if (skb != NULL) {
					dev_kfree_skb_any(skb);
					dev->net->stats.tx_dropped++;
				}
				goto exit_no_skb;
			}
			ctx->tx_low_mem_val--;
		}
		if (ctx->is_ndp16) {
			/* fill out the initial 16-bit NTB header */
			nth.nth16 = skb_put_zero(skb_out, sizeof(struct usb_cdc_ncm_nth16));
			nth.nth16->dwSignature = cpu_to_le32(USB_CDC_NCM_NTH16_SIGN);
			nth.nth16->wHeaderLength = cpu_to_le16(sizeof(struct usb_cdc_ncm_nth16));
			nth.nth16->wSequence = cpu_to_le16(ctx->tx_seq++);
		} else {
			/* fill out the initial 32-bit NTB header */
			nth.nth32 = skb_put_zero(skb_out, sizeof(struct usb_cdc_ncm_nth32));
			nth.nth32->dwSignature = cpu_to_le32(USB_CDC_NCM_NTH32_SIGN);
			nth.nth32->wHeaderLength = cpu_to_le16(sizeof(struct usb_cdc_ncm_nth32));
			nth.nth32->wSequence = cpu_to_le16(ctx->tx_seq++);
		}

		/* count total number of frames in this NTB */
		ctx->tx_curr_frame_num = 0;

		/* recent payload counter for this skb_out */
		ctx->tx_curr_frame_payload = 0;
	}

	for (n = ctx->tx_curr_frame_num; n < ctx->tx_max_datagrams; n++) {
		/* send any remaining skb first */
		if (skb == NULL) {
			skb = ctx->tx_rem_skb;
			sign = ctx->tx_rem_sign;
			ctx->tx_rem_skb = NULL;

			/* check for end of skb */
			if (skb == NULL)
				break;
		}

		/* get the appropriate NDP for this skb */
		if (ctx->is_ndp16)
			ndp.ndp16 = cdc_ncm_ndp16(ctx, skb_out, sign, skb->len + ctx->tx_modulus + ctx->tx_remainder);
		else
			ndp.ndp32 = cdc_ncm_ndp32(ctx, skb_out, sign, skb->len + ctx->tx_modulus + ctx->tx_remainder);

		/* align beginning of next frame */
		cdc_ncm_align_tail(skb_out,  ctx->tx_modulus, ctx->tx_remainder, ctx->tx_curr_size);

		/* check if we had enough room left for both NDP and frame */
		if ((ctx->is_ndp16 && !ndp.ndp16) || (!ctx->is_ndp16 && !ndp.ndp32) ||
		    skb_out->len + skb->len + delayed_ndp_size > ctx->tx_curr_size) {
			if (n == 0) {
				/* won't fit, MTU problem? */
				dev_kfree_skb_any(skb);
				skb = NULL;
				dev->net->stats.tx_dropped++;
			} else {
				/* no room for skb - store for later */
				if (ctx->tx_rem_skb != NULL) {
					dev_kfree_skb_any(ctx->tx_rem_skb);
					dev->net->stats.tx_dropped++;
				}
				ctx->tx_rem_skb = skb;
				ctx->tx_rem_sign = sign;
				skb = NULL;
				ready2send = 1;
				ctx->tx_reason_ntb_full++;	/* count reason for transmitting */
			}
			break;
		}

		/* calculate frame number within this NDP */
		if (ctx->is_ndp16) {
			ndplen = le16_to_cpu(ndp.ndp16->wLength);
			index = (ndplen - sizeof(struct usb_cdc_ncm_ndp16)) / sizeof(struct usb_cdc_ncm_dpe16) - 1;

			/* OK, add this skb */
			ndp.ndp16->dpe16[index].wDatagramLength = cpu_to_le16(skb->len);
			ndp.ndp16->dpe16[index].wDatagramIndex = cpu_to_le16(skb_out->len);
			ndp.ndp16->wLength = cpu_to_le16(ndplen + sizeof(struct usb_cdc_ncm_dpe16));
		} else {
			ndplen = le16_to_cpu(ndp.ndp32->wLength);
			index = (ndplen - sizeof(struct usb_cdc_ncm_ndp32)) / sizeof(struct usb_cdc_ncm_dpe32) - 1;

			ndp.ndp32->dpe32[index].dwDatagramLength = cpu_to_le32(skb->len);
			ndp.ndp32->dpe32[index].dwDatagramIndex = cpu_to_le32(skb_out->len);
			ndp.ndp32->wLength = cpu_to_le16(ndplen + sizeof(struct usb_cdc_ncm_dpe32));
		}
		skb_put_data(skb_out, skb->data, skb->len);
		ctx->tx_curr_frame_payload += skb->len;	/* count real tx payload data */
		dev_kfree_skb_any(skb);
		skb = NULL;

		/* send now if this NDP is full */
		if (index >= CDC_NCM_DPT_DATAGRAMS_MAX) {
			ready2send = 1;
			ctx->tx_reason_ndp_full++;	/* count reason for transmitting */
			break;
		}
	}

	/* free up any dangling skb */
	if (skb != NULL) {
		dev_kfree_skb_any(skb);
		skb = NULL;
		dev->net->stats.tx_dropped++;
	}

	ctx->tx_curr_frame_num = n;

	if (n == 0) {
		/* wait for more frames */
		/* push variables */
		ctx->tx_curr_skb = skb_out;
		goto exit_no_skb;

	} else if ((n < ctx->tx_max_datagrams) && (ready2send == 0) && (ctx->timer_interval > 0)) {
		/* wait for more frames */
		/* push variables */
		ctx->tx_curr_skb = skb_out;
		/* set the pending count */
		if (n < CDC_NCM_RESTART_TIMER_DATAGRAM_CNT)
			ctx->tx_timer_pending = CDC_NCM_TIMER_PENDING_CNT;
		goto exit_no_skb;

	} else {
		if (n == ctx->tx_max_datagrams)
			ctx->tx_reason_max_datagram++;	/* count reason for transmitting */
		/* frame goes out */
		/* variables will be reset at next call */
	}

	/* If requested, put NDP at end of frame. */
	if (ctx->drvflags & CDC_NCM_FLAG_NDP_TO_END) {
		if (ctx->is_ndp16) {
			nth.nth16 = (struct usb_cdc_ncm_nth16 *)skb_out->data;
			cdc_ncm_align_tail(skb_out, ctx->tx_ndp_modulus, 0, ctx->tx_curr_size - ctx->max_ndp_size);
			nth.nth16->wNdpIndex = cpu_to_le16(skb_out->len);
			skb_put_data(skb_out, ctx->delayed_ndp16, ctx->max_ndp_size);

			/* Zero out delayed NDP - signature checking will naturally fail. */
			ndp.ndp16 = memset(ctx->delayed_ndp16, 0, ctx->max_ndp_size);
		} else {
			nth.nth32 = (struct usb_cdc_ncm_nth32 *)skb_out->data;
			cdc_ncm_align_tail(skb_out, ctx->tx_ndp_modulus, 0, ctx->tx_curr_size - ctx->max_ndp_size);
			nth.nth32->dwNdpIndex = cpu_to_le32(skb_out->len);
			skb_put_data(skb_out, ctx->delayed_ndp32, ctx->max_ndp_size);

			ndp.ndp32 = memset(ctx->delayed_ndp32, 0, ctx->max_ndp_size);
		}
	}

	/* If collected data size is less or equal ctx->min_tx_pkt
	 * bytes, we send buffers as it is. If we get more data, it
	 * would be more efficient for USB HS mobile device with DMA
	 * engine to receive a full size NTB, than canceling DMA
	 * transfer and receiving a short packet.
	 *
	 * This optimization support is pointless if we end up sending
	 * a ZLP after full sized NTBs.
	 */
	if (!(dev->driver_info->flags & FLAG_SEND_ZLP) &&
	    skb_out->len > ctx->min_tx_pkt) {
		padding_count = ctx->tx_curr_size - skb_out->len;
		if (!WARN_ON(padding_count > ctx->tx_curr_size))
			skb_put_zero(skb_out, padding_count);
	} else if (skb_out->len < ctx->tx_curr_size &&
		   (skb_out->len % dev->maxpacket) == 0) {
		skb_put_u8(skb_out, 0);	/* force short packet */
	}

	/* set final frame length */
	if (ctx->is_ndp16) {
		nth.nth16 = (struct usb_cdc_ncm_nth16 *)skb_out->data;
		nth.nth16->wBlockLength = cpu_to_le16(skb_out->len);
	} else {
		nth.nth32 = (struct usb_cdc_ncm_nth32 *)skb_out->data;
		nth.nth32->dwBlockLength = cpu_to_le32(skb_out->len);
	}

	/* return skb */
	ctx->tx_curr_skb = NULL;

	/* keep private stats: framing overhead and number of NTBs */
	ctx->tx_overhead += skb_out->len - ctx->tx_curr_frame_payload;
	ctx->tx_ntbs++;

	/* usbnet will count all the framing overhead by default.
	 * Adjust the stats so that the tx_bytes counter show real
	 * payload data instead.
	 */
	usbnet_set_skb_tx_stats(skb_out, n,
				(long)ctx->tx_curr_frame_payload - skb_out->len);

	return skb_out;

exit_no_skb:
	/* Start timer, if there is a remaining non-empty skb */
	if (ctx->tx_curr_skb != NULL && n > 0)
		cdc_ncm_tx_timeout_start(ctx);
	return NULL;
}
EXPORT_SYMBOL_GPL(cdc_ncm_fill_tx_frame);

static void cdc_ncm_tx_timeout_start(struct cdc_ncm_ctx *ctx)
{
	/* start timer, if not already started */
	if (!(hrtimer_active(&ctx->tx_timer) || atomic_read(&ctx->stop)))
		hrtimer_start(&ctx->tx_timer,
				ctx->timer_interval,
				HRTIMER_MODE_REL);
}

static enum hrtimer_restart cdc_ncm_tx_timer_cb(struct hrtimer *timer)
{
	struct cdc_ncm_ctx *ctx =
			container_of(timer, struct cdc_ncm_ctx, tx_timer);

	if (!atomic_read(&ctx->stop))
		tasklet_schedule(&ctx->bh);
	return HRTIMER_NORESTART;
}

static void cdc_ncm_txpath_bh(struct tasklet_struct *t)
{
	struct cdc_ncm_ctx *ctx = from_tasklet(ctx, t, bh);
	struct usbnet *dev = ctx->dev;

	spin_lock_bh(&ctx->mtx);
	if (ctx->tx_timer_pending != 0) {
		ctx->tx_timer_pending--;
		cdc_ncm_tx_timeout_start(ctx);
		spin_unlock_bh(&ctx->mtx);
	} else if (dev->net != NULL) {
		ctx->tx_reason_timeout++;	/* count reason for transmitting */
		spin_unlock_bh(&ctx->mtx);
		netif_tx_lock_bh(dev->net);
		usbnet_start_xmit(NULL, dev->net);
		netif_tx_unlock_bh(dev->net);
	} else {
		spin_unlock_bh(&ctx->mtx);
	}
}

struct sk_buff *
cdc_ncm_tx_fixup(struct usbnet *dev, struct sk_buff *skb, gfp_t flags)
{
	struct sk_buff *skb_out;
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];

	/*
	 * The Ethernet API we are using does not support transmitting
	 * multiple Ethernet frames in a single call. This driver will
	 * accumulate multiple Ethernet frames and send out a larger
	 * USB frame when the USB buffer is full or when a single jiffies
	 * timeout happens.
	 */
	if (ctx == NULL)
		goto error;

	spin_lock_bh(&ctx->mtx);

	if (ctx->is_ndp16)
		skb_out = cdc_ncm_fill_tx_frame(dev, skb, cpu_to_le32(USB_CDC_NCM_NDP16_NOCRC_SIGN));
	else
		skb_out = cdc_ncm_fill_tx_frame(dev, skb, cpu_to_le32(USB_CDC_NCM_NDP32_NOCRC_SIGN));

	spin_unlock_bh(&ctx->mtx);
	return skb_out;

error:
	if (skb != NULL)
		dev_kfree_skb_any(skb);

	return NULL;
}
EXPORT_SYMBOL_GPL(cdc_ncm_tx_fixup);

/* verify NTB header and return offset of first NDP, or negative error */
int cdc_ncm_rx_verify_nth16(struct cdc_ncm_ctx *ctx, struct sk_buff *skb_in)
{
	struct usbnet *dev = netdev_priv(skb_in->dev);
	struct usb_cdc_ncm_nth16 *nth16;
	int len;
	int ret = -EINVAL;

	if (ctx == NULL)
		goto error;

	if (skb_in->len < (sizeof(struct usb_cdc_ncm_nth16) +
					sizeof(struct usb_cdc_ncm_ndp16))) {
		netif_dbg(dev, rx_err, dev->net, "frame too short\n");
		goto error;
	}

	nth16 = (struct usb_cdc_ncm_nth16 *)skb_in->data;

	if (nth16->dwSignature != cpu_to_le32(USB_CDC_NCM_NTH16_SIGN)) {
		netif_dbg(dev, rx_err, dev->net,
			  "invalid NTH16 signature <%#010x>\n",
			  le32_to_cpu(nth16->dwSignature));
		goto error;
	}

	len = le16_to_cpu(nth16->wBlockLength);
	if (len > ctx->rx_max) {
		netif_dbg(dev, rx_err, dev->net,
			  "unsupported NTB block length %u/%u\n", len,
			  ctx->rx_max);
		goto error;
	}

	if ((ctx->rx_seq + 1) != le16_to_cpu(nth16->wSequence) &&
	    (ctx->rx_seq || le16_to_cpu(nth16->wSequence)) &&
	    !((ctx->rx_seq == 0xffff) && !le16_to_cpu(nth16->wSequence))) {
		netif_dbg(dev, rx_err, dev->net,
			  "sequence number glitch prev=%d curr=%d\n",
			  ctx->rx_seq, le16_to_cpu(nth16->wSequence));
	}
	ctx->rx_seq = le16_to_cpu(nth16->wSequence);

	ret = le16_to_cpu(nth16->wNdpIndex);
error:
	return ret;
}
EXPORT_SYMBOL_GPL(cdc_ncm_rx_verify_nth16);

int cdc_ncm_rx_verify_nth32(struct cdc_ncm_ctx *ctx, struct sk_buff *skb_in)
{
	struct usbnet *dev = netdev_priv(skb_in->dev);
	struct usb_cdc_ncm_nth32 *nth32;
	int len;
	int ret = -EINVAL;

	if (ctx == NULL)
		goto error;

	if (skb_in->len < (sizeof(struct usb_cdc_ncm_nth32) +
					sizeof(struct usb_cdc_ncm_ndp32))) {
		netif_dbg(dev, rx_err, dev->net, "frame too short\n");
		goto error;
	}

	nth32 = (struct usb_cdc_ncm_nth32 *)skb_in->data;

	if (nth32->dwSignature != cpu_to_le32(USB_CDC_NCM_NTH32_SIGN)) {
		netif_dbg(dev, rx_err, dev->net,
			  "invalid NTH32 signature <%#010x>\n",
			  le32_to_cpu(nth32->dwSignature));
		goto error;
	}

	len = le32_to_cpu(nth32->dwBlockLength);
	if (len > ctx->rx_max) {
		netif_dbg(dev, rx_err, dev->net,
			  "unsupported NTB block length %u/%u\n", len,
			  ctx->rx_max);
		goto error;
	}

	if ((ctx->rx_seq + 1) != le16_to_cpu(nth32->wSequence) &&
	    (ctx->rx_seq || le16_to_cpu(nth32->wSequence)) &&
	    !((ctx->rx_seq == 0xffff) && !le16_to_cpu(nth32->wSequence))) {
		netif_dbg(dev, rx_err, dev->net,
			  "sequence number glitch prev=%d curr=%d\n",
			  ctx->rx_seq, le16_to_cpu(nth32->wSequence));
	}
	ctx->rx_seq = le16_to_cpu(nth32->wSequence);

	ret = le32_to_cpu(nth32->dwNdpIndex);
error:
	return ret;
}
EXPORT_SYMBOL_GPL(cdc_ncm_rx_verify_nth32);

/* verify NDP header and return number of datagrams, or negative error */
int cdc_ncm_rx_verify_ndp16(struct sk_buff *skb_in, int ndpoffset)
{
	struct usbnet *dev = netdev_priv(skb_in->dev);
	struct usb_cdc_ncm_ndp16 *ndp16;
	int ret = -EINVAL;

	if ((ndpoffset + sizeof(struct usb_cdc_ncm_ndp16)) > skb_in->len) {
		netif_dbg(dev, rx_err, dev->net, "invalid NDP offset  <%u>\n",
			  ndpoffset);
		goto error;
	}
	ndp16 = (struct usb_cdc_ncm_ndp16 *)(skb_in->data + ndpoffset);

	if (le16_to_cpu(ndp16->wLength) < USB_CDC_NCM_NDP16_LENGTH_MIN) {
		netif_dbg(dev, rx_err, dev->net, "invalid DPT16 length <%u>\n",
			  le16_to_cpu(ndp16->wLength));
		goto error;
	}

	ret = ((le16_to_cpu(ndp16->wLength) -
					sizeof(struct usb_cdc_ncm_ndp16)) /
					sizeof(struct usb_cdc_ncm_dpe16));
	ret--; /* we process NDP entries except for the last one */

	if ((sizeof(struct usb_cdc_ncm_ndp16) +
	     ret * (sizeof(struct usb_cdc_ncm_dpe16))) > skb_in->len) {
		netif_dbg(dev, rx_err, dev->net, "Invalid nframes = %d\n", ret);
		ret = -EINVAL;
	}

error:
	return ret;
}
EXPORT_SYMBOL_GPL(cdc_ncm_rx_verify_ndp16);

/* verify NDP header and return number of datagrams, or negative error */
int cdc_ncm_rx_verify_ndp32(struct sk_buff *skb_in, int ndpoffset)
{
	struct usbnet *dev = netdev_priv(skb_in->dev);
	struct usb_cdc_ncm_ndp32 *ndp32;
	int ret = -EINVAL;

	if ((ndpoffset + sizeof(struct usb_cdc_ncm_ndp32)) > skb_in->len) {
		netif_dbg(dev, rx_err, dev->net, "invalid NDP offset  <%u>\n",
			  ndpoffset);
		goto error;
	}
	ndp32 = (struct usb_cdc_ncm_ndp32 *)(skb_in->data + ndpoffset);

	if (le16_to_cpu(ndp32->wLength) < USB_CDC_NCM_NDP32_LENGTH_MIN) {
		netif_dbg(dev, rx_err, dev->net, "invalid DPT32 length <%u>\n",
			  le16_to_cpu(ndp32->wLength));
		goto error;
	}

	ret = ((le16_to_cpu(ndp32->wLength) -
					sizeof(struct usb_cdc_ncm_ndp32)) /
					sizeof(struct usb_cdc_ncm_dpe32));
	ret--; /* we process NDP entries except for the last one */

	if ((sizeof(struct usb_cdc_ncm_ndp32) +
	     ret * (sizeof(struct usb_cdc_ncm_dpe32))) > skb_in->len) {
		netif_dbg(dev, rx_err, dev->net, "Invalid nframes = %d\n", ret);
		ret = -EINVAL;
	}

error:
	return ret;
}
EXPORT_SYMBOL_GPL(cdc_ncm_rx_verify_ndp32);

int cdc_ncm_rx_fixup(struct usbnet *dev, struct sk_buff *skb_in)
{
	struct sk_buff *skb;
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];
	int len;
	int nframes;
	int x;
	int offset;
	union {
		struct usb_cdc_ncm_ndp16 *ndp16;
		struct usb_cdc_ncm_ndp32 *ndp32;
	} ndp;
	union {
		struct usb_cdc_ncm_dpe16 *dpe16;
		struct usb_cdc_ncm_dpe32 *dpe32;
	} dpe;

	int ndpoffset;
	int loopcount = 50; /* arbitrary max preventing infinite loop */
	u32 payload = 0;

	if (ctx->is_ndp16)
		ndpoffset = cdc_ncm_rx_verify_nth16(ctx, skb_in);
	else
		ndpoffset = cdc_ncm_rx_verify_nth32(ctx, skb_in);

	if (ndpoffset < 0)
		goto error;

next_ndp:
	if (ctx->is_ndp16) {
		nframes = cdc_ncm_rx_verify_ndp16(skb_in, ndpoffset);
		if (nframes < 0)
			goto error;

		ndp.ndp16 = (struct usb_cdc_ncm_ndp16 *)(skb_in->data + ndpoffset);

		if (ndp.ndp16->dwSignature != cpu_to_le32(USB_CDC_NCM_NDP16_NOCRC_SIGN)) {
			netif_dbg(dev, rx_err, dev->net,
				  "invalid DPT16 signature <%#010x>\n",
				  le32_to_cpu(ndp.ndp16->dwSignature));
			goto err_ndp;
		}
		dpe.dpe16 = ndp.ndp16->dpe16;
	} else {
		nframes = cdc_ncm_rx_verify_ndp32(skb_in, ndpoffset);
		if (nframes < 0)
			goto error;

		ndp.ndp32 = (struct usb_cdc_ncm_ndp32 *)(skb_in->data + ndpoffset);

		if (ndp.ndp32->dwSignature != cpu_to_le32(USB_CDC_NCM_NDP32_NOCRC_SIGN)) {
			netif_dbg(dev, rx_err, dev->net,
				  "invalid DPT32 signature <%#010x>\n",
				  le32_to_cpu(ndp.ndp32->dwSignature));
			goto err_ndp;
		}
		dpe.dpe32 = ndp.ndp32->dpe32;
	}

	for (x = 0; x < nframes; x++) {
		if (ctx->is_ndp16) {
			offset = le16_to_cpu(dpe.dpe16->wDatagramIndex);
			len = le16_to_cpu(dpe.dpe16->wDatagramLength);
		} else {
			offset = le32_to_cpu(dpe.dpe32->dwDatagramIndex);
			len = le32_to_cpu(dpe.dpe32->dwDatagramLength);
		}

		/*
		 * CDC NCM ch. 3.7
		 * All entries after first NULL entry are to be ignored
		 */
		if ((offset == 0) || (len == 0)) {
			if (!x)
				goto err_ndp; /* empty NTB */
			break;
		}

		/* sanity checking */
		if (((offset + len) > skb_in->len) ||
				(len > ctx->rx_max) || (len < ETH_HLEN)) {
			netif_dbg(dev, rx_err, dev->net,
				  "invalid frame detected (ignored) offset[%u]=%u, length=%u, skb=%p\n",
				  x, offset, len, skb_in);
			if (!x)
				goto err_ndp;
			break;

		} else {
			/* create a fresh copy to reduce truesize */
			skb = netdev_alloc_skb_ip_align(dev->net,  len);
			if (!skb)
				goto error;
			skb_put_data(skb, skb_in->data + offset, len);
			usbnet_skb_return(dev, skb);
			payload += len;	/* count payload bytes in this NTB */
		}

		if (ctx->is_ndp16)
			dpe.dpe16++;
		else
			dpe.dpe32++;
	}
err_ndp:
	/* are there more NDPs to process? */
	if (ctx->is_ndp16)
		ndpoffset = le16_to_cpu(ndp.ndp16->wNextNdpIndex);
	else
		ndpoffset = le32_to_cpu(ndp.ndp32->dwNextNdpIndex);

	if (ndpoffset && loopcount--)
		goto next_ndp;

	/* update stats */
	ctx->rx_overhead += skb_in->len - payload;
	ctx->rx_ntbs++;

	return 1;
error:
	return 0;
}
EXPORT_SYMBOL_GPL(cdc_ncm_rx_fixup);

static void
cdc_ncm_speed_change(struct usbnet *dev,
		     struct usb_cdc_speed_change *data)
{
	/* RTL8156 shipped before 2021 sends notification about every 32ms. */
	dev->rx_speed = le32_to_cpu(data->DLBitRRate);
	dev->tx_speed = le32_to_cpu(data->ULBitRate);
}

static void cdc_ncm_status(struct usbnet *dev, struct urb *urb)
{
	struct usb_cdc_notification *event;

	if (urb->actual_length < sizeof(*event))
		return;

	/* test for split data in 8-byte chunks */
	if (test_and_clear_bit(EVENT_STS_SPLIT, &dev->flags)) {
		cdc_ncm_speed_change(dev,
		      (struct usb_cdc_speed_change *)urb->transfer_buffer);
		return;
	}

	event = urb->transfer_buffer;

	switch (event->bNotificationType) {
	case USB_CDC_NOTIFY_NETWORK_CONNECTION:
		/*
		 * According to the CDC NCM specification ch.7.1
		 * USB_CDC_NOTIFY_NETWORK_CONNECTION notification shall be
		 * sent by device after USB_CDC_NOTIFY_SPEED_CHANGE.
		 */
		/* RTL8156 shipped before 2021 sends notification about
		 * every 32ms. Don't forward notification if state is same.
		 */
		if (netif_carrier_ok(dev->net) != !!event->wValue)
			usbnet_link_change(dev, !!event->wValue, 0);
		break;

	case USB_CDC_NOTIFY_SPEED_CHANGE:
		if (urb->actual_length < (sizeof(*event) +
					sizeof(struct usb_cdc_speed_change)))
			set_bit(EVENT_STS_SPLIT, &dev->flags);
		else
			cdc_ncm_speed_change(dev,
					     (struct usb_cdc_speed_change *)&event[1]);
		break;

	default:
		dev_dbg(&dev->udev->dev,
			"NCM: unexpected notification 0x%02x!\n",
			event->bNotificationType);
		break;
	}
}

static const struct driver_info cdc_ncm_info = {
	.description = "CDC NCM",
	.flags = FLAG_POINTTOPOINT | FLAG_NO_SETINT | FLAG_MULTI_PACKET
			| FLAG_LINK_INTR | FLAG_ETHER,
	.bind = cdc_ncm_bind,
	.unbind = cdc_ncm_unbind,
	.manage_power = usbnet_manage_power,
	.status = cdc_ncm_status,
	.rx_fixup = cdc_ncm_rx_fixup,
	.tx_fixup = cdc_ncm_tx_fixup,
	.set_rx_mode = usbnet_cdc_update_filter,
};

/* Same as cdc_ncm_info, but with FLAG_WWAN */
static const struct driver_info wwan_info = {
	.description = "Mobile Broadband Network Device",
	.flags = FLAG_POINTTOPOINT | FLAG_NO_SETINT | FLAG_MULTI_PACKET
			| FLAG_LINK_INTR | FLAG_WWAN,
	.bind = cdc_ncm_bind,
	.unbind = cdc_ncm_unbind,
	.manage_power = usbnet_manage_power,
	.status = cdc_ncm_status,
	.rx_fixup = cdc_ncm_rx_fixup,
	.tx_fixup = cdc_ncm_tx_fixup,
	.set_rx_mode = usbnet_cdc_update_filter,
};

/* Same as wwan_info, but with FLAG_NOARP  */
static const struct driver_info wwan_noarp_info = {
	.description = "Mobile Broadband Network Device (NO ARP)",
	.flags = FLAG_POINTTOPOINT | FLAG_NO_SETINT | FLAG_MULTI_PACKET
			| FLAG_LINK_INTR | FLAG_WWAN | FLAG_NOARP,
	.bind = cdc_ncm_bind,
	.unbind = cdc_ncm_unbind,
	.manage_power = usbnet_manage_power,
	.status = cdc_ncm_status,
	.rx_fixup = cdc_ncm_rx_fixup,
	.tx_fixup = cdc_ncm_tx_fixup,
	.set_rx_mode = usbnet_cdc_update_filter,
};

static const struct usb_device_id cdc_devs[] = {
	/* Ericsson MBM devices like F5521gw */
	{ .match_flags = USB_DEVICE_ID_MATCH_INT_INFO
		| USB_DEVICE_ID_MATCH_VENDOR,
	  .idVendor = 0x0bdb,
	  .bInterfaceClass = USB_CLASS_COMM,
	  .bInterfaceSubClass = USB_CDC_SUBCLASS_NCM,
	  .bInterfaceProtocol = USB_CDC_PROTO_NONE,
	  .driver_info = (unsigned long) &wwan_info,
	},

	/* Telit LE910 V2 */
	{ USB_DEVICE_AND_INTERFACE_INFO(0x1bc7, 0x0036,
		USB_CLASS_COMM,
		USB_CDC_SUBCLASS_NCM, USB_CDC_PROTO_NONE),
	  .driver_info = (unsigned long)&wwan_noarp_info,
	},

	/* DW5812 LTE Verizon Mobile Broadband Card
	 * Unlike DW5550 this device requires FLAG_NOARP
	 */
	{ USB_DEVICE_AND_INTERFACE_INFO(0x413c, 0x81bb,
		USB_CLASS_COMM,
		USB_CDC_SUBCLASS_NCM, USB_CDC_PROTO_NONE),
	  .driver_info = (unsigned long)&wwan_noarp_info,
	},

	/* DW5813 LTE AT&T Mobile Broadband Card
	 * Unlike DW5550 this device requires FLAG_NOARP
	 */
	{ USB_DEVICE_AND_INTERFACE_INFO(0x413c, 0x81bc,
		USB_CLASS_COMM,
		USB_CDC_SUBCLASS_NCM, USB_CDC_PROTO_NONE),
	  .driver_info = (unsigned long)&wwan_noarp_info,
	},

	/* Dell branded MBM devices like DW5550 */
	{ .match_flags = USB_DEVICE_ID_MATCH_INT_INFO
		| USB_DEVICE_ID_MATCH_VENDOR,
	  .idVendor = 0x413c,
	  .bInterfaceClass = USB_CLASS_COMM,
	  .bInterfaceSubClass = USB_CDC_SUBCLASS_NCM,
	  .bInterfaceProtocol = USB_CDC_PROTO_NONE,
	  .driver_info = (unsigned long) &wwan_info,
	},

	/* Toshiba branded MBM devices */
	{ .match_flags = USB_DEVICE_ID_MATCH_INT_INFO
		| USB_DEVICE_ID_MATCH_VENDOR,
	  .idVendor = 0x0930,
	  .bInterfaceClass = USB_CLASS_COMM,
	  .bInterfaceSubClass = USB_CDC_SUBCLASS_NCM,
	  .bInterfaceProtocol = USB_CDC_PROTO_NONE,
	  .driver_info = (unsigned long) &wwan_info,
	},

	/* tag Huawei devices as wwan */
	{ USB_VENDOR_AND_INTERFACE_INFO(0x12d1,
					USB_CLASS_COMM,
					USB_CDC_SUBCLASS_NCM,
					USB_CDC_PROTO_NONE),
	  .driver_info = (unsigned long)&wwan_info,
	},

	/* Infineon(now Intel) HSPA Modem platform */
	{ USB_DEVICE_AND_INTERFACE_INFO(0x1519, 0x0443,
		USB_CLASS_COMM,
		USB_CDC_SUBCLASS_NCM, USB_CDC_PROTO_NONE),
	  .driver_info = (unsigned long)&wwan_noarp_info,
	},

	/* u-blox TOBY-L4 */
	{ USB_DEVICE_AND_INTERFACE_INFO(0x1546, 0x1010,
		USB_CLASS_COMM,
		USB_CDC_SUBCLASS_NCM, USB_CDC_PROTO_NONE),
	  .driver_info = (unsigned long)&wwan_info,
	},

	/* Generic CDC-NCM devices */
	{ USB_INTERFACE_INFO(USB_CLASS_COMM,
		USB_CDC_SUBCLASS_NCM, USB_CDC_PROTO_NONE),
		.driver_info = (unsigned long)&cdc_ncm_info,
	},
	{
	},
};
MODULE_DEVICE_TABLE(usb, cdc_devs);

static struct usb_driver cdc_ncm_driver = {
	.name = "cdc_ncm",
	.id_table = cdc_devs,
	.probe = usbnet_probe,
	.disconnect = usbnet_disconnect,
	.suspend = usbnet_suspend,
	.resume = usbnet_resume,
	.reset_resume =	usbnet_resume,
	.supports_autosuspend = 1,
	.disable_hub_initiated_lpm = 1,
};

module_usb_driver(cdc_ncm_driver);

MODULE_AUTHOR("Hans Petter Selasky");
MODULE_DESCRIPTION("USB CDC NCM host driver");
MODULE_LICENSE("Dual BSD/GPL");
