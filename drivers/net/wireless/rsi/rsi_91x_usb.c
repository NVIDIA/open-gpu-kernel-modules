/*
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
 *
 */

#include <linux/module.h>
#include <linux/types.h>
#include <net/rsi_91x.h>
#include "rsi_usb.h"
#include "rsi_hal.h"
#include "rsi_coex.h"

/* Default operating mode is wlan STA + BT */
static u16 dev_oper_mode = DEV_OPMODE_STA_BT_DUAL;
module_param(dev_oper_mode, ushort, 0444);
MODULE_PARM_DESC(dev_oper_mode,
		 "1[Wi-Fi], 4[BT], 8[BT LE], 5[Wi-Fi STA + BT classic]\n"
		 "9[Wi-Fi STA + BT LE], 13[Wi-Fi STA + BT classic + BT LE]\n"
		 "6[AP + BT classic], 14[AP + BT classic + BT LE]");

static int rsi_rx_urb_submit(struct rsi_hw *adapter, u8 ep_num, gfp_t flags);

/**
 * rsi_usb_card_write() - This function writes to the USB Card.
 * @adapter: Pointer to the adapter structure.
 * @buf: Pointer to the buffer from where the data has to be taken.
 * @len: Length to be written.
 * @endpoint: Type of endpoint.
 *
 * Return: status: 0 on success, a negative error code on failure.
 */
static int rsi_usb_card_write(struct rsi_hw *adapter,
			      u8 *buf,
			      u16 len,
			      u8 endpoint)
{
	struct rsi_91x_usbdev *dev = (struct rsi_91x_usbdev *)adapter->rsi_dev;
	int status;
	u8 *seg = dev->tx_buffer;
	int transfer;
	int ep = dev->bulkout_endpoint_addr[endpoint - 1];

	memset(seg, 0, len + RSI_USB_TX_HEAD_ROOM);
	memcpy(seg + RSI_USB_TX_HEAD_ROOM, buf, len);
	len += RSI_USB_TX_HEAD_ROOM;
	transfer = len;
	status = usb_bulk_msg(dev->usbdev,
			      usb_sndbulkpipe(dev->usbdev, ep),
			      (void *)seg,
			      (int)len,
			      &transfer,
			      HZ * 5);

	if (status < 0) {
		rsi_dbg(ERR_ZONE,
			"Card write failed with error code :%10d\n", status);
		dev->write_fail = 1;
	}
	return status;
}

/**
 * rsi_write_multiple() - This function writes multiple bytes of information
 *			  to the USB card.
 * @adapter: Pointer to the adapter structure.
 * @endpoint: Type of endpoint.
 * @data: Pointer to the data that has to be written.
 * @count: Number of multiple bytes to be written.
 *
 * Return: 0 on success, a negative error code on failure.
 */
static int rsi_write_multiple(struct rsi_hw *adapter,
			      u8 endpoint,
			      u8 *data,
			      u32 count)
{
	struct rsi_91x_usbdev *dev;

	if (!adapter)
		return -ENODEV;

	if (endpoint == 0)
		return -EINVAL;

	dev = (struct rsi_91x_usbdev *)adapter->rsi_dev;
	if (dev->write_fail)
		return -ENETDOWN;

	return rsi_usb_card_write(adapter, data, count, endpoint);
}

/**
 * rsi_find_bulk_in_and_out_endpoints() - This function initializes the bulk
 *					  endpoints to the device.
 * @interface: Pointer to the USB interface structure.
 * @adapter: Pointer to the adapter structure.
 *
 * Return: ret_val: 0 on success, -ENOMEM on failure.
 */
static int rsi_find_bulk_in_and_out_endpoints(struct usb_interface *interface,
					      struct rsi_hw *adapter)
{
	struct rsi_91x_usbdev *dev = (struct rsi_91x_usbdev *)adapter->rsi_dev;
	struct usb_host_interface *iface_desc;
	struct usb_endpoint_descriptor *endpoint;
	__le16 buffer_size;
	int ii, bin_found = 0, bout_found = 0;

	iface_desc = interface->cur_altsetting;

	for (ii = 0; ii < iface_desc->desc.bNumEndpoints; ++ii) {
		endpoint = &(iface_desc->endpoint[ii].desc);

		if (!dev->bulkin_endpoint_addr[bin_found] &&
		    (endpoint->bEndpointAddress & USB_DIR_IN) &&
		    ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) ==
		    USB_ENDPOINT_XFER_BULK)) {
			buffer_size = endpoint->wMaxPacketSize;
			dev->bulkin_size[bin_found] = buffer_size;
			dev->bulkin_endpoint_addr[bin_found] =
				endpoint->bEndpointAddress;
			bin_found++;
		}

		if (!dev->bulkout_endpoint_addr[bout_found] &&
		    !(endpoint->bEndpointAddress & USB_DIR_IN) &&
		    ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) ==
		    USB_ENDPOINT_XFER_BULK)) {
			buffer_size = endpoint->wMaxPacketSize;
			dev->bulkout_endpoint_addr[bout_found] =
				endpoint->bEndpointAddress;
			dev->bulkout_size[bout_found] = buffer_size;
			bout_found++;
		}

		if (bin_found >= MAX_BULK_EP || bout_found >= MAX_BULK_EP)
			break;
	}

	if (!(dev->bulkin_endpoint_addr[0] && dev->bulkout_endpoint_addr[0])) {
		dev_err(&interface->dev, "missing wlan bulk endpoints\n");
		return -EINVAL;
	}

	if (adapter->priv->coex_mode > 1) {
		if (!dev->bulkin_endpoint_addr[1]) {
			dev_err(&interface->dev, "missing bt bulk-in endpoint\n");
			return -EINVAL;
		}
	}

	return 0;
}

#define RSI_USB_REQ_OUT	(USB_TYPE_VENDOR | USB_DIR_OUT | USB_RECIP_DEVICE)
#define RSI_USB_REQ_IN	(USB_TYPE_VENDOR | USB_DIR_IN | USB_RECIP_DEVICE)

/* rsi_usb_reg_read() - This function reads data from given register address.
 * @usbdev: Pointer to the usb_device structure.
 * @reg: Address of the register to be read.
 * @value: Value to be read.
 * @len: length of data to be read.
 *
 * Return: status: 0 on success, a negative error code on failure.
 */
static int rsi_usb_reg_read(struct usb_device *usbdev,
			    u32 reg,
			    u16 *value,
			    u16 len)
{
	u8 *buf;
	int status = -ENOMEM;

	if (len > RSI_USB_CTRL_BUF_SIZE)
		return -EINVAL;

	buf  = kmalloc(RSI_USB_CTRL_BUF_SIZE, GFP_KERNEL);
	if (!buf)
		return status;

	status = usb_control_msg(usbdev,
				 usb_rcvctrlpipe(usbdev, 0),
				 USB_VENDOR_REGISTER_READ,
				 RSI_USB_REQ_IN,
				 ((reg & 0xffff0000) >> 16), (reg & 0xffff),
				 (void *)buf,
				 len,
				 USB_CTRL_GET_TIMEOUT);

	*value = (buf[0] | (buf[1] << 8));
	if (status < 0) {
		rsi_dbg(ERR_ZONE,
			"%s: Reg read failed with error code :%d\n",
			__func__, status);
	}
	kfree(buf);

	return status;
}

/**
 * rsi_usb_reg_write() - This function writes the given data into the given
 *			 register address.
 * @usbdev: Pointer to the usb_device structure.
 * @reg: Address of the register.
 * @value: Value to write.
 * @len: Length of data to be written.
 *
 * Return: status: 0 on success, a negative error code on failure.
 */
static int rsi_usb_reg_write(struct usb_device *usbdev,
			     u32 reg,
			     u32 value,
			     u16 len)
{
	u8 *usb_reg_buf;
	int status = -ENOMEM;

	if (len > RSI_USB_CTRL_BUF_SIZE)
		return -EINVAL;

	usb_reg_buf  = kmalloc(RSI_USB_CTRL_BUF_SIZE, GFP_KERNEL);
	if (!usb_reg_buf)
		return status;

	usb_reg_buf[0] = (cpu_to_le32(value) & 0x00ff);
	usb_reg_buf[1] = (cpu_to_le32(value) & 0xff00) >> 8;
	usb_reg_buf[2] = (cpu_to_le32(value) & 0x00ff0000) >> 16;
	usb_reg_buf[3] = (cpu_to_le32(value) & 0xff000000) >> 24;

	status = usb_control_msg(usbdev,
				 usb_sndctrlpipe(usbdev, 0),
				 USB_VENDOR_REGISTER_WRITE,
				 RSI_USB_REQ_OUT,
				 ((cpu_to_le32(reg) & 0xffff0000) >> 16),
				 (cpu_to_le32(reg) & 0xffff),
				 (void *)usb_reg_buf,
				 len,
				 USB_CTRL_SET_TIMEOUT);
	if (status < 0) {
		rsi_dbg(ERR_ZONE,
			"%s: Reg write failed with error code :%d\n",
			__func__, status);
	}
	kfree(usb_reg_buf);

	return status;
}

/**
 * rsi_rx_done_handler() - This function is called when a packet is received
 *			   from USB stack. This is callback to receive done.
 * @urb: Received URB.
 *
 * Return: None.
 */
static void rsi_rx_done_handler(struct urb *urb)
{
	struct rx_usb_ctrl_block *rx_cb = urb->context;
	struct rsi_91x_usbdev *dev = (struct rsi_91x_usbdev *)rx_cb->data;
	int status = -EINVAL;

	if (urb->status) {
		dev_kfree_skb(rx_cb->rx_skb);
		return;
	}

	if (urb->actual_length <= 0 ||
	    urb->actual_length > rx_cb->rx_skb->len) {
		rsi_dbg(INFO_ZONE, "%s: Invalid packet length = %d\n",
			__func__, urb->actual_length);
		goto out;
	}
	if (skb_queue_len(&dev->rx_q) >= RSI_MAX_RX_PKTS) {
		rsi_dbg(INFO_ZONE, "Max RX packets reached\n");
		goto out;
	}
	skb_trim(rx_cb->rx_skb, urb->actual_length);
	skb_queue_tail(&dev->rx_q, rx_cb->rx_skb);

	rsi_set_event(&dev->rx_thread.event);
	status = 0;

out:
	if (rsi_rx_urb_submit(dev->priv, rx_cb->ep_num, GFP_ATOMIC))
		rsi_dbg(ERR_ZONE, "%s: Failed in urb submission", __func__);

	if (status)
		dev_kfree_skb(rx_cb->rx_skb);
}

static void rsi_rx_urb_kill(struct rsi_hw *adapter, u8 ep_num)
{
	struct rsi_91x_usbdev *dev = (struct rsi_91x_usbdev *)adapter->rsi_dev;
	struct rx_usb_ctrl_block *rx_cb = &dev->rx_cb[ep_num - 1];
	struct urb *urb = rx_cb->rx_urb;

	usb_kill_urb(urb);
}

/**
 * rsi_rx_urb_submit() - This function submits the given URB to the USB stack.
 * @adapter: Pointer to the adapter structure.
 * @ep_num: Endpoint number.
 * @mem_flags: The type of memory to allocate.
 *
 * Return: 0 on success, a negative error code on failure.
 */
static int rsi_rx_urb_submit(struct rsi_hw *adapter, u8 ep_num, gfp_t mem_flags)
{
	struct rsi_91x_usbdev *dev = (struct rsi_91x_usbdev *)adapter->rsi_dev;
	struct rx_usb_ctrl_block *rx_cb = &dev->rx_cb[ep_num - 1];
	struct urb *urb = rx_cb->rx_urb;
	int status;
	struct sk_buff *skb;
	u8 dword_align_bytes = 0;

#define RSI_MAX_RX_USB_PKT_SIZE	3000
	skb = dev_alloc_skb(RSI_MAX_RX_USB_PKT_SIZE);
	if (!skb)
		return -ENOMEM;
	skb_reserve(skb, MAX_DWORD_ALIGN_BYTES);
	skb_put(skb, RSI_MAX_RX_USB_PKT_SIZE - MAX_DWORD_ALIGN_BYTES);
	dword_align_bytes = (unsigned long)skb->data & 0x3f;
	if (dword_align_bytes > 0)
		skb_push(skb, dword_align_bytes);
	urb->transfer_buffer = skb->data;
	rx_cb->rx_skb = skb;

	usb_fill_bulk_urb(urb,
			  dev->usbdev,
			  usb_rcvbulkpipe(dev->usbdev,
			  dev->bulkin_endpoint_addr[ep_num - 1]),
			  urb->transfer_buffer,
			  skb->len,
			  rsi_rx_done_handler,
			  rx_cb);

	status = usb_submit_urb(urb, mem_flags);
	if (status) {
		rsi_dbg(ERR_ZONE, "%s: Failed in urb submission\n", __func__);
		dev_kfree_skb(skb);
	}

	return status;
}

static int rsi_usb_read_register_multiple(struct rsi_hw *adapter, u32 addr,
					  u8 *data, u16 count)
{
	struct rsi_91x_usbdev *dev = (struct rsi_91x_usbdev *)adapter->rsi_dev;
	u8 *buf;
	u16 transfer;
	int status;

	if (!addr)
		return -EINVAL;

	buf = kzalloc(RSI_USB_BUF_SIZE, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	while (count) {
		transfer = min_t(u16, count, RSI_USB_BUF_SIZE);
		status = usb_control_msg(dev->usbdev,
					 usb_rcvctrlpipe(dev->usbdev, 0),
					 USB_VENDOR_REGISTER_READ,
					 RSI_USB_REQ_IN,
					 ((addr & 0xffff0000) >> 16),
					 (addr & 0xffff), (void *)buf,
					 transfer, USB_CTRL_GET_TIMEOUT);
		if (status < 0) {
			rsi_dbg(ERR_ZONE,
				"Reg read failed with error code :%d\n",
				 status);
			kfree(buf);
			return status;
		}
		memcpy(data, buf, transfer);
		count -= transfer;
		data += transfer;
		addr += transfer;
	}
	kfree(buf);
	return 0;
}

/**
 * rsi_usb_write_register_multiple() - This function writes multiple bytes of
 *				       information to multiple registers.
 * @adapter: Pointer to the adapter structure.
 * @addr: Address of the register.
 * @data: Pointer to the data that has to be written.
 * @count: Number of multiple bytes to be written on to the registers.
 *
 * Return: status: 0 on success, a negative error code on failure.
 */
static int rsi_usb_write_register_multiple(struct rsi_hw *adapter, u32 addr,
					   u8 *data, u16 count)
{
	struct rsi_91x_usbdev *dev = (struct rsi_91x_usbdev *)adapter->rsi_dev;
	u8 *buf;
	u16 transfer;
	int status = 0;

	buf = kzalloc(RSI_USB_BUF_SIZE, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	while (count) {
		transfer = min_t(u16, count, RSI_USB_BUF_SIZE);
		memcpy(buf, data, transfer);
		status = usb_control_msg(dev->usbdev,
					 usb_sndctrlpipe(dev->usbdev, 0),
					 USB_VENDOR_REGISTER_WRITE,
					 RSI_USB_REQ_OUT,
					 ((addr & 0xffff0000) >> 16),
					 (addr & 0xffff),
					 (void *)buf,
					 transfer,
					 USB_CTRL_SET_TIMEOUT);
		if (status < 0) {
			rsi_dbg(ERR_ZONE,
				"Reg write failed with error code :%d\n",
				status);
			kfree(buf);
			return status;
		}
		count -= transfer;
		data += transfer;
		addr += transfer;
	}

	kfree(buf);
	return 0;
}

/**
 *rsi_usb_host_intf_write_pkt() - This function writes the packet to the
 *				   USB card.
 * @adapter: Pointer to the adapter structure.
 * @pkt: Pointer to the data to be written on to the card.
 * @len: Length of the data to be written on to the card.
 *
 * Return: 0 on success, a negative error code on failure.
 */
static int rsi_usb_host_intf_write_pkt(struct rsi_hw *adapter,
				       u8 *pkt,
				       u32 len)
{
	u32 queueno = ((pkt[1] >> 4) & 0x7);
	u8 endpoint;

	endpoint = ((queueno == RSI_WIFI_MGMT_Q || queueno == RSI_WIFI_DATA_Q ||
		     queueno == RSI_COEX_Q) ? WLAN_EP : BT_EP);

	return rsi_write_multiple(adapter,
				  endpoint,
				  (u8 *)pkt,
				  len);
}

static int rsi_usb_master_reg_read(struct rsi_hw *adapter, u32 reg,
				   u32 *value, u16 len)
{
	struct usb_device *usbdev =
		((struct rsi_91x_usbdev *)adapter->rsi_dev)->usbdev;
	u16 temp;
	int ret;

	ret = rsi_usb_reg_read(usbdev, reg, &temp, len);
	if (ret < 0)
		return ret;
	*value = temp;

	return 0;
}

static int rsi_usb_master_reg_write(struct rsi_hw *adapter,
				    unsigned long reg,
				    unsigned long value, u16 len)
{
	struct usb_device *usbdev =
		((struct rsi_91x_usbdev *)adapter->rsi_dev)->usbdev;

	return rsi_usb_reg_write(usbdev, reg, value, len);
}

static int rsi_usb_load_data_master_write(struct rsi_hw *adapter,
					  u32 base_address,
					  u32 instructions_sz, u16 block_size,
					  u8 *ta_firmware)
{
	u16 num_blocks;
	u32 cur_indx, i;
	u8 temp_buf[256];
	int status;

	num_blocks = instructions_sz / block_size;
	rsi_dbg(INFO_ZONE, "num_blocks: %d\n", num_blocks);

	for (cur_indx = 0, i = 0; i < num_blocks; i++, cur_indx += block_size) {
		memcpy(temp_buf, ta_firmware + cur_indx, block_size);
		status = rsi_usb_write_register_multiple(adapter, base_address,
							 (u8 *)(temp_buf),
							 block_size);
		if (status < 0)
			return status;

		rsi_dbg(INFO_ZONE, "%s: loading block: %d\n", __func__, i);
		base_address += block_size;
	}

	if (instructions_sz % block_size) {
		memset(temp_buf, 0, block_size);
		memcpy(temp_buf, ta_firmware + cur_indx,
		       instructions_sz % block_size);
		status = rsi_usb_write_register_multiple
						(adapter, base_address,
						 (u8 *)temp_buf,
						 instructions_sz % block_size);
		if (status < 0)
			return status;
		rsi_dbg(INFO_ZONE,
			"Written Last Block in Address 0x%x Successfully\n",
			cur_indx);
	}
	return 0;
}

static struct rsi_host_intf_ops usb_host_intf_ops = {
	.write_pkt		= rsi_usb_host_intf_write_pkt,
	.read_reg_multiple	= rsi_usb_read_register_multiple,
	.write_reg_multiple	= rsi_usb_write_register_multiple,
	.master_reg_read	= rsi_usb_master_reg_read,
	.master_reg_write	= rsi_usb_master_reg_write,
	.load_data_master_write	= rsi_usb_load_data_master_write,
};

/**
 * rsi_deinit_usb_interface() - This function deinitializes the usb interface.
 * @adapter: Pointer to the adapter structure.
 *
 * Return: None.
 */
static void rsi_deinit_usb_interface(struct rsi_hw *adapter)
{
	struct rsi_91x_usbdev *dev = (struct rsi_91x_usbdev *)adapter->rsi_dev;

	rsi_kill_thread(&dev->rx_thread);

	usb_free_urb(dev->rx_cb[0].rx_urb);
	if (adapter->priv->coex_mode > 1)
		usb_free_urb(dev->rx_cb[1].rx_urb);

	kfree(dev->tx_buffer);
}

static int rsi_usb_init_rx(struct rsi_hw *adapter)
{
	struct rsi_91x_usbdev *dev = (struct rsi_91x_usbdev *)adapter->rsi_dev;
	struct rx_usb_ctrl_block *rx_cb;
	u8 idx, num_rx_cb;

	num_rx_cb = (adapter->priv->coex_mode > 1 ? 2 : 1);

	for (idx = 0; idx < num_rx_cb; idx++) {
		rx_cb = &dev->rx_cb[idx];

		rx_cb->rx_urb = usb_alloc_urb(0, GFP_KERNEL);
		if (!rx_cb->rx_urb) {
			rsi_dbg(ERR_ZONE, "Failed alloc rx urb[%d]\n", idx);
			goto err;
		}
		rx_cb->ep_num = idx + 1;
		rx_cb->data = (void *)dev;
	}
	skb_queue_head_init(&dev->rx_q);
	rsi_init_event(&dev->rx_thread.event);
	if (rsi_create_kthread(adapter->priv, &dev->rx_thread,
			       rsi_usb_rx_thread, "RX-Thread")) {
		rsi_dbg(ERR_ZONE, "%s: Unable to init rx thrd\n", __func__);
		goto err;
	}

	return 0;

err:
	usb_free_urb(dev->rx_cb[0].rx_urb);
	if (adapter->priv->coex_mode > 1)
		usb_free_urb(dev->rx_cb[1].rx_urb);

	return -1;
}

/**
 * rsi_init_usb_interface() - This function initializes the usb interface.
 * @adapter: Pointer to the adapter structure.
 * @pfunction: Pointer to USB interface structure.
 *
 * Return: 0 on success, a negative error code on failure.
 */
static int rsi_init_usb_interface(struct rsi_hw *adapter,
				  struct usb_interface *pfunction)
{
	struct rsi_91x_usbdev *rsi_dev;
	int status;

	rsi_dev = kzalloc(sizeof(*rsi_dev), GFP_KERNEL);
	if (!rsi_dev)
		return -ENOMEM;

	adapter->rsi_dev = rsi_dev;
	rsi_dev->usbdev = interface_to_usbdev(pfunction);
	rsi_dev->priv = (void *)adapter;

	if (rsi_find_bulk_in_and_out_endpoints(pfunction, adapter)) {
		status = -EINVAL;
		goto fail_eps;
	}

	adapter->device = &pfunction->dev;
	usb_set_intfdata(pfunction, adapter);

	rsi_dev->tx_buffer = kmalloc(2048, GFP_KERNEL);
	if (!rsi_dev->tx_buffer) {
		status = -ENOMEM;
		goto fail_eps;
	}

	if (rsi_usb_init_rx(adapter)) {
		rsi_dbg(ERR_ZONE, "Failed to init RX handle\n");
		status = -ENOMEM;
		goto fail_rx;
	}

	rsi_dev->tx_blk_size = 252;
	adapter->block_size = rsi_dev->tx_blk_size;

	/* Initializing function callbacks */
	adapter->check_hw_queue_status = rsi_usb_check_queue_status;
	adapter->determine_event_timeout = rsi_usb_event_timeout;
	adapter->rsi_host_intf = RSI_HOST_INTF_USB;
	adapter->host_intf_ops = &usb_host_intf_ops;

#ifdef CONFIG_RSI_DEBUGFS
	/* In USB, one less than the MAX_DEBUGFS_ENTRIES entries is required */
	adapter->num_debugfs_entries = (MAX_DEBUGFS_ENTRIES - 1);
#endif

	rsi_dbg(INIT_ZONE, "%s: Enabled the interface\n", __func__);
	return 0;

fail_rx:
	kfree(rsi_dev->tx_buffer);

fail_eps:

	return status;
}

static int usb_ulp_read_write(struct rsi_hw *adapter, u16 addr, u32 data,
			      u16 len_in_bits)
{
	int ret;

	ret = rsi_usb_master_reg_write
			(adapter, RSI_GSPI_DATA_REG1,
			 ((addr << 6) | ((data >> 16) & 0xffff)), 2);
	if (ret < 0)
		return ret;

	ret = rsi_usb_master_reg_write(adapter, RSI_GSPI_DATA_REG0,
				       (data & 0xffff), 2);
	if (ret < 0)
		return ret;

	/* Initializing GSPI for ULP read/writes */
	rsi_usb_master_reg_write(adapter, RSI_GSPI_CTRL_REG0,
				 RSI_GSPI_CTRL_REG0_VALUE, 2);

	ret = rsi_usb_master_reg_write(adapter, RSI_GSPI_CTRL_REG1,
				       ((len_in_bits - 1) | RSI_GSPI_TRIG), 2);
	if (ret < 0)
		return ret;

	msleep(20);

	return 0;
}

static int rsi_reset_card(struct rsi_hw *adapter)
{
	int ret;

	rsi_dbg(INFO_ZONE, "Resetting Card...\n");
	rsi_usb_master_reg_write(adapter, RSI_TA_HOLD_REG, 0xE, 4);

	/* This msleep will ensure Thread-Arch processor to go to hold
	 * and any pending dma transfers to rf in device to finish.
	 */
	msleep(100);

	ret = rsi_usb_master_reg_write(adapter, SWBL_REGOUT,
				       RSI_FW_WDT_DISABLE_REQ,
				       RSI_COMMON_REG_SIZE);
	if (ret < 0) {
		rsi_dbg(ERR_ZONE, "Disabling firmware watchdog timer failed\n");
		goto fail;
	}

	if (adapter->device_model != RSI_DEV_9116) {
		ret = usb_ulp_read_write(adapter, RSI_WATCH_DOG_TIMER_1,
					 RSI_ULP_WRITE_2, 32);
		if (ret < 0)
			goto fail;
		ret = usb_ulp_read_write(adapter, RSI_WATCH_DOG_TIMER_2,
					 RSI_ULP_WRITE_0, 32);
		if (ret < 0)
			goto fail;
		ret = usb_ulp_read_write(adapter, RSI_WATCH_DOG_DELAY_TIMER_1,
					 RSI_ULP_WRITE_50, 32);
		if (ret < 0)
			goto fail;
		ret = usb_ulp_read_write(adapter, RSI_WATCH_DOG_DELAY_TIMER_2,
					 RSI_ULP_WRITE_0, 32);
		if (ret < 0)
			goto fail;
		ret = usb_ulp_read_write(adapter, RSI_WATCH_DOG_TIMER_ENABLE,
					 RSI_ULP_TIMER_ENABLE, 32);
		if (ret < 0)
			goto fail;
	} else {
		ret = rsi_usb_master_reg_write(adapter,
					       NWP_WWD_INTERRUPT_TIMER,
					       NWP_WWD_INT_TIMER_CLKS,
					       RSI_9116_REG_SIZE);
		if (ret < 0)
			goto fail;
		ret = rsi_usb_master_reg_write(adapter,
					       NWP_WWD_SYSTEM_RESET_TIMER,
					       NWP_WWD_SYS_RESET_TIMER_CLKS,
					       RSI_9116_REG_SIZE);
		if (ret < 0)
			goto fail;
		ret = rsi_usb_master_reg_write(adapter,
					       NWP_WWD_MODE_AND_RSTART,
					       NWP_WWD_TIMER_DISABLE,
					       RSI_9116_REG_SIZE);
		if (ret < 0)
			goto fail;
	}

	rsi_dbg(INFO_ZONE, "Reset card done\n");
	return ret;

fail:
	rsi_dbg(ERR_ZONE, "Reset card failed\n");
	return ret;
}

/**
 * rsi_probe() - This function is called by kernel when the driver provided
 *		 Vendor and device IDs are matched. All the initialization
 *		 work is done here.
 * @pfunction: Pointer to the USB interface structure.
 * @id: Pointer to the usb_device_id structure.
 *
 * Return: 0 on success, a negative error code on failure.
 */
static int rsi_probe(struct usb_interface *pfunction,
		     const struct usb_device_id *id)
{
	struct rsi_hw *adapter;
	struct rsi_91x_usbdev *dev;
	u16 fw_status;
	int status;

	rsi_dbg(INIT_ZONE, "%s: Init function called\n", __func__);

	adapter = rsi_91x_init(dev_oper_mode);
	if (!adapter) {
		rsi_dbg(ERR_ZONE, "%s: Failed to init os intf ops\n",
			__func__);
		return -ENOMEM;
	}
	adapter->rsi_host_intf = RSI_HOST_INTF_USB;

	status = rsi_init_usb_interface(adapter, pfunction);
	if (status) {
		rsi_dbg(ERR_ZONE, "%s: Failed to init usb interface\n",
			__func__);
		goto err;
	}

	rsi_dbg(ERR_ZONE, "%s: Initialized os intf ops\n", __func__);

	if (id->idProduct == RSI_USB_PID_9113) {
		rsi_dbg(INIT_ZONE, "%s: 9113 module detected\n", __func__);
		adapter->device_model = RSI_DEV_9113;
	} else if (id->idProduct == RSI_USB_PID_9116) {
		rsi_dbg(INIT_ZONE, "%s: 9116 module detected\n", __func__);
		adapter->device_model = RSI_DEV_9116;
	} else {
		rsi_dbg(ERR_ZONE, "%s: Unsupported RSI device id 0x%x\n",
			__func__, id->idProduct);
		goto err1;
	}

	dev = (struct rsi_91x_usbdev *)adapter->rsi_dev;

	status = rsi_usb_reg_read(dev->usbdev, FW_STATUS_REG, &fw_status, 2);
	if (status < 0)
		goto err1;
	else
		fw_status &= 1;

	if (!fw_status) {
		rsi_dbg(INIT_ZONE, "Loading firmware...\n");
		status = rsi_hal_device_init(adapter);
		if (status) {
			rsi_dbg(ERR_ZONE, "%s: Failed in device init\n",
				__func__);
			goto err1;
		}
		rsi_dbg(INIT_ZONE, "%s: Device Init Done\n", __func__);
	}

	status = rsi_rx_urb_submit(adapter, WLAN_EP, GFP_KERNEL);
	if (status)
		goto err1;

	if (adapter->priv->coex_mode > 1) {
		status = rsi_rx_urb_submit(adapter, BT_EP, GFP_KERNEL);
		if (status)
			goto err_kill_wlan_urb;
	}

	return 0;

err_kill_wlan_urb:
	rsi_rx_urb_kill(adapter, WLAN_EP);
err1:
	rsi_deinit_usb_interface(adapter);
err:
	rsi_91x_deinit(adapter);
	rsi_dbg(ERR_ZONE, "%s: Failed in probe...Exiting\n", __func__);
	return status;
}

/**
 * rsi_disconnect() - This function performs the reverse of the probe function,
 *		      it deinitialize the driver structure.
 * @pfunction: Pointer to the USB interface structure.
 *
 * Return: None.
 */
static void rsi_disconnect(struct usb_interface *pfunction)
{
	struct rsi_hw *adapter = usb_get_intfdata(pfunction);

	if (!adapter)
		return;

	rsi_mac80211_detach(adapter);

	if (IS_ENABLED(CONFIG_RSI_COEX) && adapter->priv->coex_mode > 1 &&
	    adapter->priv->bt_adapter) {
		rsi_bt_ops.detach(adapter->priv->bt_adapter);
		adapter->priv->bt_adapter = NULL;
	}

	if (adapter->priv->coex_mode > 1)
		rsi_rx_urb_kill(adapter, BT_EP);
	rsi_rx_urb_kill(adapter, WLAN_EP);

	rsi_reset_card(adapter);
	rsi_deinit_usb_interface(adapter);
	rsi_91x_deinit(adapter);

	rsi_dbg(INFO_ZONE, "%s: Deinitialization completed\n", __func__);
}

#ifdef CONFIG_PM
static int rsi_suspend(struct usb_interface *intf, pm_message_t message)
{
	/* Not yet implemented */
	return -ENOSYS;
}

static int rsi_resume(struct usb_interface *intf)
{
	/* Not yet implemented */
	return -ENOSYS;
}
#endif

static const struct usb_device_id rsi_dev_table[] = {
	{ USB_DEVICE(RSI_USB_VENDOR_ID, RSI_USB_PID_9113) },
	{ USB_DEVICE(RSI_USB_VENDOR_ID, RSI_USB_PID_9116) },
	{ /* Blank */},
};

static struct usb_driver rsi_driver = {
	.name       = "RSI-USB WLAN",
	.probe      = rsi_probe,
	.disconnect = rsi_disconnect,
	.id_table   = rsi_dev_table,
#ifdef CONFIG_PM
	.suspend    = rsi_suspend,
	.resume     = rsi_resume,
#endif
};

module_usb_driver(rsi_driver);

MODULE_AUTHOR("Redpine Signals Inc");
MODULE_DESCRIPTION("Common USB layer for RSI drivers");
MODULE_DEVICE_TABLE(usb, rsi_dev_table);
MODULE_FIRMWARE(FIRMWARE_RSI9113);
MODULE_VERSION("0.1");
MODULE_LICENSE("Dual BSD/GPL");
