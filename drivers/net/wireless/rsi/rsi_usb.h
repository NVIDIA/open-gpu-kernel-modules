/*
 * @section LICENSE
 * Copyright (c) 2014 Redpine Signals Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef __RSI_USB_INTF__
#define __RSI_USB_INTF__

#include <linux/usb.h>
#include "rsi_main.h"
#include "rsi_common.h"

#define RSI_USB_VENDOR_ID	0x1618
#define RSI_USB_PID_9113	0x9113
#define RSI_USB_PID_9116	0x9116

#define USB_INTERNAL_REG_1           0x25000
#define RSI_USB_READY_MAGIC_NUM      0xab
#define FW_STATUS_REG                0x41050012
#define RSI_TA_HOLD_REG              0x22000844
#define RSI_FW_WDT_DISABLE_REQ	     0x69

#define USB_VENDOR_REGISTER_READ     0x15
#define USB_VENDOR_REGISTER_WRITE    0x16
#define RSI_USB_TX_HEAD_ROOM         128

#define MAX_RX_URBS                  2
#define MAX_BULK_EP                  8
#define WLAN_EP                      1
#define BT_EP                        2

#define RSI_USB_BUF_SIZE	     4096
#define RSI_USB_CTRL_BUF_SIZE	     0x04

struct rx_usb_ctrl_block {
	u8 *data;
	struct urb *rx_urb;
	struct sk_buff *rx_skb;
	u8 ep_num;
};

struct rsi_91x_usbdev {
	void *priv;
	struct rsi_thread rx_thread;
	u8 endpoint;
	struct usb_device *usbdev;
	struct usb_interface *pfunction;
	struct rx_usb_ctrl_block rx_cb[MAX_RX_URBS];
	u8 *tx_buffer;
	__le16 bulkin_size[MAX_BULK_EP];
	u8 bulkin_endpoint_addr[MAX_BULK_EP];
	__le16 bulkout_size[MAX_BULK_EP];
	u8 bulkout_endpoint_addr[MAX_BULK_EP];
	u32 tx_blk_size;
	u8 write_fail;
	struct sk_buff_head rx_q;
};

static inline int rsi_usb_check_queue_status(struct rsi_hw *adapter, u8 q_num)
{
	/* In USB, there isn't any need to check the queue status */
	return QUEUE_NOT_FULL;
}

static inline int rsi_usb_event_timeout(struct rsi_hw *adapter)
{
	return EVENT_WAIT_FOREVER;
}

void rsi_usb_rx_thread(struct rsi_common *common);
#endif
