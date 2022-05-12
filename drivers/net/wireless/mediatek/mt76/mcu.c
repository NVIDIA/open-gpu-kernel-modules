// SPDX-License-Identifier: ISC
/*
 * Copyright (C) 2019 Lorenzo Bianconi <lorenzo.bianconi83@gmail.com>
 */

#include "mt76.h"

struct sk_buff *
mt76_mcu_msg_alloc(struct mt76_dev *dev, const void *data,
		   int data_len)
{
	const struct mt76_mcu_ops *ops = dev->mcu_ops;
	int length = ops->headroom + data_len + ops->tailroom;
	struct sk_buff *skb;

	skb = alloc_skb(length, GFP_KERNEL);
	if (!skb)
		return NULL;

	memset(skb->head, 0, length);
	skb_reserve(skb, ops->headroom);

	if (data && data_len)
		skb_put_data(skb, data, data_len);

	return skb;
}
EXPORT_SYMBOL_GPL(mt76_mcu_msg_alloc);

struct sk_buff *mt76_mcu_get_response(struct mt76_dev *dev,
				      unsigned long expires)
{
	unsigned long timeout;

	if (!time_is_after_jiffies(expires))
		return NULL;

	timeout = expires - jiffies;
	wait_event_timeout(dev->mcu.wait,
			   (!skb_queue_empty(&dev->mcu.res_q) ||
			    test_bit(MT76_MCU_RESET, &dev->phy.state)),
			   timeout);
	return skb_dequeue(&dev->mcu.res_q);
}
EXPORT_SYMBOL_GPL(mt76_mcu_get_response);

void mt76_mcu_rx_event(struct mt76_dev *dev, struct sk_buff *skb)
{
	skb_queue_tail(&dev->mcu.res_q, skb);
	wake_up(&dev->mcu.wait);
}
EXPORT_SYMBOL_GPL(mt76_mcu_rx_event);

int mt76_mcu_send_and_get_msg(struct mt76_dev *dev, int cmd, const void *data,
			      int len, bool wait_resp, struct sk_buff **ret_skb)
{
	struct sk_buff *skb;

	if (dev->mcu_ops->mcu_send_msg)
		return dev->mcu_ops->mcu_send_msg(dev, cmd, data, len, wait_resp);

	skb = mt76_mcu_msg_alloc(dev, data, len);
	if (!skb)
		return -ENOMEM;

	return mt76_mcu_skb_send_and_get_msg(dev, skb, cmd, wait_resp, ret_skb);
}
EXPORT_SYMBOL_GPL(mt76_mcu_send_and_get_msg);

int mt76_mcu_skb_send_and_get_msg(struct mt76_dev *dev, struct sk_buff *skb,
				  int cmd, bool wait_resp,
				  struct sk_buff **ret_skb)
{
	unsigned long expires;
	int ret, seq;

	if (ret_skb)
		*ret_skb = NULL;

	mutex_lock(&dev->mcu.mutex);

	ret = dev->mcu_ops->mcu_skb_send_msg(dev, skb, cmd, &seq);
	if (ret < 0)
		goto out;

	if (!wait_resp) {
		ret = 0;
		goto out;
	}

	expires = jiffies + dev->mcu.timeout;

	do {
		skb = mt76_mcu_get_response(dev, expires);
		ret = dev->mcu_ops->mcu_parse_response(dev, cmd, skb, seq);
		if (!ret && ret_skb)
			*ret_skb = skb;
		else
			dev_kfree_skb(skb);
	} while (ret == -EAGAIN);

out:
	mutex_unlock(&dev->mcu.mutex);

	return ret;
}
EXPORT_SYMBOL_GPL(mt76_mcu_skb_send_and_get_msg);

int mt76_mcu_send_firmware(struct mt76_dev *dev, int cmd, const void *data,
			   int len)
{
	int err, cur_len;

	while (len > 0) {
		cur_len = min_t(int, 4096 - dev->mcu_ops->headroom, len);

		err = mt76_mcu_send_msg(dev, cmd, data, cur_len, false);
		if (err)
			return err;

		data += cur_len;
		len -= cur_len;

		if (dev->queue_ops->tx_cleanup)
			dev->queue_ops->tx_cleanup(dev,
						   dev->q_mcu[MT_MCUQ_FWDL],
						   false);
	}

	return 0;
}
EXPORT_SYMBOL_GPL(mt76_mcu_send_firmware);
