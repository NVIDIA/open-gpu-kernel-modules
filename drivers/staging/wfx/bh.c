// SPDX-License-Identifier: GPL-2.0-only
/*
 * Interrupt bottom half (BH).
 *
 * Copyright (c) 2017-2020, Silicon Laboratories, Inc.
 * Copyright (c) 2010, ST-Ericsson
 */
#include <linux/gpio/consumer.h>
#include <net/mac80211.h>

#include "bh.h"
#include "wfx.h"
#include "hwio.h"
#include "traces.h"
#include "hif_rx.h"
#include "hif_api_cmd.h"

static void device_wakeup(struct wfx_dev *wdev)
{
	int max_retry = 3;

	if (!wdev->pdata.gpio_wakeup)
		return;
	if (gpiod_get_value_cansleep(wdev->pdata.gpio_wakeup) > 0)
		return;

	if (wfx_api_older_than(wdev, 1, 4)) {
		gpiod_set_value_cansleep(wdev->pdata.gpio_wakeup, 1);
		if (!completion_done(&wdev->hif.ctrl_ready))
			usleep_range(2000, 2500);
		return;
	}
	for (;;) {
		gpiod_set_value_cansleep(wdev->pdata.gpio_wakeup, 1);
		// completion.h does not provide any function to wait
		// completion without consume it (a kind of
		// wait_for_completion_done_timeout()). So we have to emulate
		// it.
		if (wait_for_completion_timeout(&wdev->hif.ctrl_ready,
						msecs_to_jiffies(2))) {
			complete(&wdev->hif.ctrl_ready);
			return;
		} else if (max_retry-- > 0) {
			// Older firmwares have a race in sleep/wake-up process.
			// Redo the process is sufficient to unfreeze the
			// chip.
			dev_err(wdev->dev, "timeout while wake up chip\n");
			gpiod_set_value_cansleep(wdev->pdata.gpio_wakeup, 0);
			usleep_range(2000, 2500);
		} else {
			dev_err(wdev->dev, "max wake-up retries reached\n");
			return;
		}
	}
}

static void device_release(struct wfx_dev *wdev)
{
	if (!wdev->pdata.gpio_wakeup)
		return;

	gpiod_set_value_cansleep(wdev->pdata.gpio_wakeup, 0);
}

static int rx_helper(struct wfx_dev *wdev, size_t read_len, int *is_cnf)
{
	struct sk_buff *skb;
	struct hif_msg *hif;
	size_t alloc_len;
	size_t computed_len;
	int release_count;
	int piggyback = 0;

	WARN(read_len > round_down(0xFFF, 2) * sizeof(u16),
	     "%s: request exceed WFx capability", __func__);

	// Add 2 to take into account piggyback size
	alloc_len = wdev->hwbus_ops->align_size(wdev->hwbus_priv, read_len + 2);
	skb = dev_alloc_skb(alloc_len);
	if (!skb)
		return -ENOMEM;

	if (wfx_data_read(wdev, skb->data, alloc_len))
		goto err;

	piggyback = le16_to_cpup((__le16 *)(skb->data + alloc_len - 2));
	_trace_piggyback(piggyback, false);

	hif = (struct hif_msg *)skb->data;
	WARN(hif->encrypted & 0x3, "encryption is unsupported");
	if (WARN(read_len < sizeof(struct hif_msg), "corrupted read"))
		goto err;
	computed_len = le16_to_cpu(hif->len);
	computed_len = round_up(computed_len, 2);
	if (computed_len != read_len) {
		dev_err(wdev->dev, "inconsistent message length: %zu != %zu\n",
			computed_len, read_len);
		print_hex_dump(KERN_INFO, "hif: ", DUMP_PREFIX_OFFSET, 16, 1,
			       hif, read_len, true);
		goto err;
	}

	if (!(hif->id & HIF_ID_IS_INDICATION)) {
		(*is_cnf)++;
		if (hif->id == HIF_CNF_ID_MULTI_TRANSMIT)
			release_count = ((struct hif_cnf_multi_transmit *)hif->body)->num_tx_confs;
		else
			release_count = 1;
		WARN(wdev->hif.tx_buffers_used < release_count, "corrupted buffer counter");
		wdev->hif.tx_buffers_used -= release_count;
	}
	_trace_hif_recv(hif, wdev->hif.tx_buffers_used);

	if (hif->id != HIF_IND_ID_EXCEPTION && hif->id != HIF_IND_ID_ERROR) {
		if (hif->seqnum != wdev->hif.rx_seqnum)
			dev_warn(wdev->dev, "wrong message sequence: %d != %d\n",
				 hif->seqnum, wdev->hif.rx_seqnum);
		wdev->hif.rx_seqnum = (hif->seqnum + 1) % (HIF_COUNTER_MAX + 1);
	}

	skb_put(skb, le16_to_cpu(hif->len));
	// wfx_handle_rx takes care on SKB livetime
	wfx_handle_rx(wdev, skb);
	if (!wdev->hif.tx_buffers_used)
		wake_up(&wdev->hif.tx_buffers_empty);

	return piggyback;

err:
	if (skb)
		dev_kfree_skb(skb);
	return -EIO;
}

static int bh_work_rx(struct wfx_dev *wdev, int max_msg, int *num_cnf)
{
	size_t len;
	int i;
	int ctrl_reg, piggyback;

	piggyback = 0;
	for (i = 0; i < max_msg; i++) {
		if (piggyback & CTRL_NEXT_LEN_MASK)
			ctrl_reg = piggyback;
		else if (try_wait_for_completion(&wdev->hif.ctrl_ready))
			ctrl_reg = atomic_xchg(&wdev->hif.ctrl_reg, 0);
		else
			ctrl_reg = 0;
		if (!(ctrl_reg & CTRL_NEXT_LEN_MASK))
			return i;
		// ctrl_reg units are 16bits words
		len = (ctrl_reg & CTRL_NEXT_LEN_MASK) * 2;
		piggyback = rx_helper(wdev, len, num_cnf);
		if (piggyback < 0)
			return i;
		if (!(piggyback & CTRL_WLAN_READY))
			dev_err(wdev->dev, "unexpected piggyback value: ready bit not set: %04x\n",
				piggyback);
	}
	if (piggyback & CTRL_NEXT_LEN_MASK) {
		ctrl_reg = atomic_xchg(&wdev->hif.ctrl_reg, piggyback);
		complete(&wdev->hif.ctrl_ready);
		if (ctrl_reg)
			dev_err(wdev->dev, "unexpected IRQ happened: %04x/%04x\n",
				ctrl_reg, piggyback);
	}
	return i;
}

static void tx_helper(struct wfx_dev *wdev, struct hif_msg *hif)
{
	int ret;
	void *data;
	bool is_encrypted = false;
	size_t len = le16_to_cpu(hif->len);

	WARN(len < sizeof(*hif), "try to send corrupted data");

	hif->seqnum = wdev->hif.tx_seqnum;
	wdev->hif.tx_seqnum = (wdev->hif.tx_seqnum + 1) % (HIF_COUNTER_MAX + 1);

	data = hif;
	WARN(len > wdev->hw_caps.size_inp_ch_buf,
	     "%s: request exceed WFx capability: %zu > %d\n", __func__,
	     len, wdev->hw_caps.size_inp_ch_buf);
	len = wdev->hwbus_ops->align_size(wdev->hwbus_priv, len);
	ret = wfx_data_write(wdev, data, len);
	if (ret)
		goto end;

	wdev->hif.tx_buffers_used++;
	_trace_hif_send(hif, wdev->hif.tx_buffers_used);
end:
	if (is_encrypted)
		kfree(data);
}

static int bh_work_tx(struct wfx_dev *wdev, int max_msg)
{
	struct hif_msg *hif;
	int i;

	for (i = 0; i < max_msg; i++) {
		hif = NULL;
		if (wdev->hif.tx_buffers_used < wdev->hw_caps.num_inp_ch_bufs) {
			if (try_wait_for_completion(&wdev->hif_cmd.ready)) {
				WARN(!mutex_is_locked(&wdev->hif_cmd.lock), "data locking error");
				hif = wdev->hif_cmd.buf_send;
			} else {
				hif = wfx_tx_queues_get(wdev);
			}
		}
		if (!hif)
			return i;
		tx_helper(wdev, hif);
	}
	return i;
}

/* In SDIO mode, it is necessary to make an access to a register to acknowledge
 * last received message. It could be possible to restrict this acknowledge to
 * SDIO mode and only if last operation was rx.
 */
static void ack_sdio_data(struct wfx_dev *wdev)
{
	u32 cfg_reg;

	config_reg_read(wdev, &cfg_reg);
	if (cfg_reg & 0xFF) {
		dev_warn(wdev->dev, "chip reports errors: %02x\n",
			 cfg_reg & 0xFF);
		config_reg_write_bits(wdev, 0xFF, 0x00);
	}
}

static void bh_work(struct work_struct *work)
{
	struct wfx_dev *wdev = container_of(work, struct wfx_dev, hif.bh);
	int stats_req = 0, stats_cnf = 0, stats_ind = 0;
	bool release_chip = false, last_op_is_rx = false;
	int num_tx, num_rx;

	device_wakeup(wdev);
	do {
		num_tx = bh_work_tx(wdev, 32);
		stats_req += num_tx;
		if (num_tx)
			last_op_is_rx = false;
		num_rx = bh_work_rx(wdev, 32, &stats_cnf);
		stats_ind += num_rx;
		if (num_rx)
			last_op_is_rx = true;
	} while (num_rx || num_tx);
	stats_ind -= stats_cnf;

	if (last_op_is_rx)
		ack_sdio_data(wdev);
	if (!wdev->hif.tx_buffers_used && !work_pending(work)) {
		device_release(wdev);
		release_chip = true;
	}
	_trace_bh_stats(stats_ind, stats_req, stats_cnf,
			wdev->hif.tx_buffers_used, release_chip);
}

/*
 * An IRQ from chip did occur
 */
void wfx_bh_request_rx(struct wfx_dev *wdev)
{
	u32 cur, prev;

	control_reg_read(wdev, &cur);
	prev = atomic_xchg(&wdev->hif.ctrl_reg, cur);
	complete(&wdev->hif.ctrl_ready);
	queue_work(system_highpri_wq, &wdev->hif.bh);

	if (!(cur & CTRL_NEXT_LEN_MASK))
		dev_err(wdev->dev, "unexpected control register value: length field is 0: %04x\n",
			cur);
	if (prev != 0)
		dev_err(wdev->dev, "received IRQ but previous data was not (yet) read: %04x/%04x\n",
			prev, cur);
}

/*
 * Driver want to send data
 */
void wfx_bh_request_tx(struct wfx_dev *wdev)
{
	queue_work(system_highpri_wq, &wdev->hif.bh);
}

/*
 * If IRQ is not available, this function allow to manually poll the control
 * register and simulate an IRQ ahen an event happened.
 *
 * Note that the device has a bug: If an IRQ raise while host read control
 * register, the IRQ is lost. So, use this function carefully (only duing
 * device initialisation).
 */
void wfx_bh_poll_irq(struct wfx_dev *wdev)
{
	ktime_t now, start;
	u32 reg;

	WARN(!wdev->poll_irq, "unexpected IRQ polling can mask IRQ");
	start = ktime_get();
	for (;;) {
		control_reg_read(wdev, &reg);
		now = ktime_get();
		if (reg & 0xFFF)
			break;
		if (ktime_after(now, ktime_add_ms(start, 1000))) {
			dev_err(wdev->dev, "time out while polling control register\n");
			return;
		}
		udelay(200);
	}
	wfx_bh_request_rx(wdev);
}

void wfx_bh_register(struct wfx_dev *wdev)
{
	INIT_WORK(&wdev->hif.bh, bh_work);
	init_completion(&wdev->hif.ctrl_ready);
	init_waitqueue_head(&wdev->hif.tx_buffers_empty);
}

void wfx_bh_unregister(struct wfx_dev *wdev)
{
	flush_work(&wdev->hif.bh);
}
