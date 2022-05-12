// SPDX-License-Identifier: GPL-2.0-or-later
/*
	Copyright (C) 2010 Willow Garage <http://www.willowgarage.com>
	Copyright (C) 2009 - 2010 Ivo van Doorn <IvDoorn@gmail.com>
	Copyright (C) 2009 Mattias Nissler <mattias.nissler@gmx.de>
	Copyright (C) 2009 Felix Fietkau <nbd@openwrt.org>
	Copyright (C) 2009 Xose Vazquez Perez <xose.vazquez@gmail.com>
	Copyright (C) 2009 Axel Kollhofer <rain_maker@root-forum.org>
	<http://rt2x00.serialmonkey.com>

 */

/*
	Module: rt2800usb
	Abstract: rt2800usb device specific routines.
	Supported chipsets: RT2800U.
 */

#include <linux/delay.h>
#include <linux/etherdevice.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/usb.h>

#include "rt2x00.h"
#include "rt2x00usb.h"
#include "rt2800lib.h"
#include "rt2800.h"
#include "rt2800usb.h"

/*
 * Allow hardware encryption to be disabled.
 */
static bool modparam_nohwcrypt;
module_param_named(nohwcrypt, modparam_nohwcrypt, bool, 0444);
MODULE_PARM_DESC(nohwcrypt, "Disable hardware encryption.");

static bool rt2800usb_hwcrypt_disabled(struct rt2x00_dev *rt2x00dev)
{
	return modparam_nohwcrypt;
}

/*
 * Queue handlers.
 */
static void rt2800usb_start_queue(struct data_queue *queue)
{
	struct rt2x00_dev *rt2x00dev = queue->rt2x00dev;
	u32 reg;

	switch (queue->qid) {
	case QID_RX:
		reg = rt2x00usb_register_read(rt2x00dev, MAC_SYS_CTRL);
		rt2x00_set_field32(&reg, MAC_SYS_CTRL_ENABLE_RX, 1);
		rt2x00usb_register_write(rt2x00dev, MAC_SYS_CTRL, reg);
		break;
	case QID_BEACON:
		reg = rt2x00usb_register_read(rt2x00dev, BCN_TIME_CFG);
		rt2x00_set_field32(&reg, BCN_TIME_CFG_TSF_TICKING, 1);
		rt2x00_set_field32(&reg, BCN_TIME_CFG_TBTT_ENABLE, 1);
		rt2x00_set_field32(&reg, BCN_TIME_CFG_BEACON_GEN, 1);
		rt2x00usb_register_write(rt2x00dev, BCN_TIME_CFG, reg);
		break;
	default:
		break;
	}
}

static void rt2800usb_stop_queue(struct data_queue *queue)
{
	struct rt2x00_dev *rt2x00dev = queue->rt2x00dev;
	u32 reg;

	switch (queue->qid) {
	case QID_RX:
		reg = rt2x00usb_register_read(rt2x00dev, MAC_SYS_CTRL);
		rt2x00_set_field32(&reg, MAC_SYS_CTRL_ENABLE_RX, 0);
		rt2x00usb_register_write(rt2x00dev, MAC_SYS_CTRL, reg);
		break;
	case QID_BEACON:
		reg = rt2x00usb_register_read(rt2x00dev, BCN_TIME_CFG);
		rt2x00_set_field32(&reg, BCN_TIME_CFG_TSF_TICKING, 0);
		rt2x00_set_field32(&reg, BCN_TIME_CFG_TBTT_ENABLE, 0);
		rt2x00_set_field32(&reg, BCN_TIME_CFG_BEACON_GEN, 0);
		rt2x00usb_register_write(rt2x00dev, BCN_TIME_CFG, reg);
		break;
	default:
		break;
	}
}

#define TXSTATUS_READ_INTERVAL 1000000

static bool rt2800usb_tx_sta_fifo_read_completed(struct rt2x00_dev *rt2x00dev,
						 int urb_status, u32 tx_status)
{
	bool valid;

	if (urb_status) {
		rt2x00_warn(rt2x00dev, "TX status read failed %d\n",
			    urb_status);

		goto stop_reading;
	}

	valid = rt2x00_get_field32(tx_status, TX_STA_FIFO_VALID);
	if (valid) {
		if (!kfifo_put(&rt2x00dev->txstatus_fifo, tx_status))
			rt2x00_warn(rt2x00dev, "TX status FIFO overrun\n");

		queue_work(rt2x00dev->workqueue, &rt2x00dev->txdone_work);

		/* Reschedule urb to read TX status again instantly */
		return true;
	}

	/* Check if there is any entry that timedout waiting on TX status */
	if (rt2800_txstatus_timeout(rt2x00dev))
		queue_work(rt2x00dev->workqueue, &rt2x00dev->txdone_work);

	if (rt2800_txstatus_pending(rt2x00dev)) {
		/* Read register after 1 ms */
		hrtimer_start(&rt2x00dev->txstatus_timer,
			      TXSTATUS_READ_INTERVAL,
			      HRTIMER_MODE_REL);
		return false;
	}

stop_reading:
	clear_bit(TX_STATUS_READING, &rt2x00dev->flags);
	/*
	 * There is small race window above, between txstatus pending check and
	 * clear_bit someone could do rt2x00usb_interrupt_txdone, so recheck
	 * here again if status reading is needed.
	 */
	if (rt2800_txstatus_pending(rt2x00dev) &&
	    !test_and_set_bit(TX_STATUS_READING, &rt2x00dev->flags))
		return true;
	else
		return false;
}

static void rt2800usb_async_read_tx_status(struct rt2x00_dev *rt2x00dev)
{

	if (test_and_set_bit(TX_STATUS_READING, &rt2x00dev->flags))
		return;

	/* Read TX_STA_FIFO register after 2 ms */
	hrtimer_start(&rt2x00dev->txstatus_timer,
		      2 * TXSTATUS_READ_INTERVAL,
		      HRTIMER_MODE_REL);
}

static void rt2800usb_tx_dma_done(struct queue_entry *entry)
{
	struct rt2x00_dev *rt2x00dev = entry->queue->rt2x00dev;

	rt2800usb_async_read_tx_status(rt2x00dev);
}

static enum hrtimer_restart rt2800usb_tx_sta_fifo_timeout(struct hrtimer *timer)
{
	struct rt2x00_dev *rt2x00dev =
	    container_of(timer, struct rt2x00_dev, txstatus_timer);

	rt2x00usb_register_read_async(rt2x00dev, TX_STA_FIFO,
				      rt2800usb_tx_sta_fifo_read_completed);

	return HRTIMER_NORESTART;
}

/*
 * Firmware functions
 */
static int rt2800usb_autorun_detect(struct rt2x00_dev *rt2x00dev)
{
	__le32 *reg;
	u32 fw_mode;
	int ret;

	reg = kmalloc(sizeof(*reg), GFP_KERNEL);
	if (reg == NULL)
		return -ENOMEM;
	/* cannot use rt2x00usb_register_read here as it uses different
	 * mode (MULTI_READ vs. DEVICE_MODE) and does not pass the
	 * magic value USB_MODE_AUTORUN (0x11) to the device, thus the
	 * returned value would be invalid.
	 */
	ret = rt2x00usb_vendor_request(rt2x00dev, USB_DEVICE_MODE,
				       USB_VENDOR_REQUEST_IN, 0,
				       USB_MODE_AUTORUN, reg, sizeof(*reg),
				       REGISTER_TIMEOUT_FIRMWARE);
	fw_mode = le32_to_cpu(*reg);
	kfree(reg);
	if (ret < 0)
		return ret;

	if ((fw_mode & 0x00000003) == 2)
		return 1;

	return 0;
}

static char *rt2800usb_get_firmware_name(struct rt2x00_dev *rt2x00dev)
{
	return FIRMWARE_RT2870;
}

static int rt2800usb_write_firmware(struct rt2x00_dev *rt2x00dev,
				    const u8 *data, const size_t len)
{
	int status;
	u32 offset;
	u32 length;
	int retval;

	/*
	 * Check which section of the firmware we need.
	 */
	if (rt2x00_rt(rt2x00dev, RT2860) ||
	    rt2x00_rt(rt2x00dev, RT2872) ||
	    rt2x00_rt(rt2x00dev, RT3070)) {
		offset = 0;
		length = 4096;
	} else {
		offset = 4096;
		length = 4096;
	}

	/*
	 * Write firmware to device.
	 */
	retval = rt2800usb_autorun_detect(rt2x00dev);
	if (retval < 0)
		return retval;
	if (retval) {
		rt2x00_info(rt2x00dev,
			    "Firmware loading not required - NIC in AutoRun mode\n");
		__clear_bit(REQUIRE_FIRMWARE, &rt2x00dev->cap_flags);
	} else {
		rt2x00usb_register_multiwrite(rt2x00dev, FIRMWARE_IMAGE_BASE,
					      data + offset, length);
	}

	rt2x00usb_register_write(rt2x00dev, H2M_MAILBOX_CID, ~0);
	rt2x00usb_register_write(rt2x00dev, H2M_MAILBOX_STATUS, ~0);

	/*
	 * Send firmware request to device to load firmware,
	 * we need to specify a long timeout time.
	 */
	status = rt2x00usb_vendor_request_sw(rt2x00dev, USB_DEVICE_MODE,
					     0, USB_MODE_FIRMWARE,
					     REGISTER_TIMEOUT_FIRMWARE);
	if (status < 0) {
		rt2x00_err(rt2x00dev, "Failed to write Firmware to device\n");
		return status;
	}

	msleep(10);
	rt2x00usb_register_write(rt2x00dev, H2M_MAILBOX_CSR, 0);

	return 0;
}

/*
 * Device state switch handlers.
 */
static int rt2800usb_init_registers(struct rt2x00_dev *rt2x00dev)
{
	u32 reg;

	/*
	 * Wait until BBP and RF are ready.
	 */
	if (rt2800_wait_csr_ready(rt2x00dev))
		return -EBUSY;

	reg = rt2x00usb_register_read(rt2x00dev, PBF_SYS_CTRL);
	rt2x00usb_register_write(rt2x00dev, PBF_SYS_CTRL, reg & ~0x00002000);

	reg = 0;
	rt2x00_set_field32(&reg, MAC_SYS_CTRL_RESET_CSR, 1);
	rt2x00_set_field32(&reg, MAC_SYS_CTRL_RESET_BBP, 1);
	rt2x00usb_register_write(rt2x00dev, MAC_SYS_CTRL, reg);

	rt2x00usb_vendor_request_sw(rt2x00dev, USB_DEVICE_MODE, 0,
				    USB_MODE_RESET, REGISTER_TIMEOUT);

	rt2x00usb_register_write(rt2x00dev, MAC_SYS_CTRL, 0x00000000);

	return 0;
}

static int rt2800usb_enable_radio(struct rt2x00_dev *rt2x00dev)
{
	u32 reg = 0;

	if (unlikely(rt2800_wait_wpdma_ready(rt2x00dev)))
		return -EIO;

	rt2x00_set_field32(&reg, USB_DMA_CFG_PHY_CLEAR, 0);
	rt2x00_set_field32(&reg, USB_DMA_CFG_RX_BULK_AGG_EN, 0);
	rt2x00_set_field32(&reg, USB_DMA_CFG_RX_BULK_AGG_TIMEOUT, 128);
	/*
	 * Total room for RX frames in kilobytes, PBF might still exceed
	 * this limit so reduce the number to prevent errors.
	 */
	rt2x00_set_field32(&reg, USB_DMA_CFG_RX_BULK_AGG_LIMIT,
			   ((rt2x00dev->rx->limit * DATA_FRAME_SIZE)
			    / 1024) - 3);
	rt2x00_set_field32(&reg, USB_DMA_CFG_RX_BULK_EN, 1);
	rt2x00_set_field32(&reg, USB_DMA_CFG_TX_BULK_EN, 1);
	rt2x00usb_register_write(rt2x00dev, USB_DMA_CFG, reg);

	return rt2800_enable_radio(rt2x00dev);
}

static void rt2800usb_disable_radio(struct rt2x00_dev *rt2x00dev)
{
	rt2800_disable_radio(rt2x00dev);
}

static int rt2800usb_set_state(struct rt2x00_dev *rt2x00dev,
			       enum dev_state state)
{
	if (state == STATE_AWAKE)
		rt2800_mcu_request(rt2x00dev, MCU_WAKEUP, 0xff, 0, 2);
	else
		rt2800_mcu_request(rt2x00dev, MCU_SLEEP, 0xff, 0xff, 2);

	return 0;
}

static int rt2800usb_set_device_state(struct rt2x00_dev *rt2x00dev,
				      enum dev_state state)
{
	int retval = 0;

	switch (state) {
	case STATE_RADIO_ON:
		/*
		 * Before the radio can be enabled, the device first has
		 * to be woken up. After that it needs a bit of time
		 * to be fully awake and then the radio can be enabled.
		 */
		rt2800usb_set_state(rt2x00dev, STATE_AWAKE);
		msleep(1);
		retval = rt2800usb_enable_radio(rt2x00dev);
		break;
	case STATE_RADIO_OFF:
		/*
		 * After the radio has been disabled, the device should
		 * be put to sleep for powersaving.
		 */
		rt2800usb_disable_radio(rt2x00dev);
		rt2800usb_set_state(rt2x00dev, STATE_SLEEP);
		break;
	case STATE_RADIO_IRQ_ON:
	case STATE_RADIO_IRQ_OFF:
		/* No support, but no error either */
		break;
	case STATE_DEEP_SLEEP:
	case STATE_SLEEP:
	case STATE_STANDBY:
	case STATE_AWAKE:
		retval = rt2800usb_set_state(rt2x00dev, state);
		break;
	default:
		retval = -ENOTSUPP;
		break;
	}

	if (unlikely(retval))
		rt2x00_err(rt2x00dev, "Device failed to enter state %d (%d)\n",
			   state, retval);

	return retval;
}

static unsigned int rt2800usb_get_dma_done(struct data_queue *queue)
{
	struct queue_entry *entry;

	entry = rt2x00queue_get_entry(queue, Q_INDEX_DMA_DONE);
	return entry->entry_idx;
}

/*
 * TX descriptor initialization
 */
static __le32 *rt2800usb_get_txwi(struct queue_entry *entry)
{
	if (entry->queue->qid == QID_BEACON)
		return (__le32 *) (entry->skb->data);
	else
		return (__le32 *) (entry->skb->data + TXINFO_DESC_SIZE);
}

static void rt2800usb_write_tx_desc(struct queue_entry *entry,
				    struct txentry_desc *txdesc)
{
	struct skb_frame_desc *skbdesc = get_skb_frame_desc(entry->skb);
	__le32 *txi = (__le32 *) entry->skb->data;
	u32 word;

	/*
	 * Initialize TXINFO descriptor
	 */
	word = rt2x00_desc_read(txi, 0);

	/*
	 * The size of TXINFO_W0_USB_DMA_TX_PKT_LEN is
	 * TXWI + 802.11 header + L2 pad + payload + pad,
	 * so need to decrease size of TXINFO.
	 */
	rt2x00_set_field32(&word, TXINFO_W0_USB_DMA_TX_PKT_LEN,
			   roundup(entry->skb->len, 4) - TXINFO_DESC_SIZE);
	rt2x00_set_field32(&word, TXINFO_W0_WIV,
			   !test_bit(ENTRY_TXD_ENCRYPT_IV, &txdesc->flags));
	rt2x00_set_field32(&word, TXINFO_W0_QSEL, 2);
	rt2x00_set_field32(&word, TXINFO_W0_SW_USE_LAST_ROUND, 0);
	rt2x00_set_field32(&word, TXINFO_W0_USB_DMA_NEXT_VALID, 0);
	rt2x00_set_field32(&word, TXINFO_W0_USB_DMA_TX_BURST,
			   test_bit(ENTRY_TXD_BURST, &txdesc->flags));
	rt2x00_desc_write(txi, 0, word);

	/*
	 * Register descriptor details in skb frame descriptor.
	 */
	skbdesc->flags |= SKBDESC_DESC_IN_SKB;
	skbdesc->desc = txi;
	skbdesc->desc_len = TXINFO_DESC_SIZE + entry->queue->winfo_size;
}

/*
 * TX data initialization
 */
static int rt2800usb_get_tx_data_len(struct queue_entry *entry)
{
	/*
	 * pad(1~3 bytes) is needed after each 802.11 payload.
	 * USB end pad(4 bytes) is needed at each USB bulk out packet end.
	 * TX frame format is :
	 * | TXINFO | TXWI | 802.11 header | L2 pad | payload | pad | USB end pad |
	 *                 |<------------- tx_pkt_len ------------->|
	 */

	return roundup(entry->skb->len, 4) + 4;
}

/*
 * TX control handlers
 */
static void rt2800usb_work_txdone(struct work_struct *work)
{
	struct rt2x00_dev *rt2x00dev =
	    container_of(work, struct rt2x00_dev, txdone_work);

	while (!kfifo_is_empty(&rt2x00dev->txstatus_fifo) ||
	       rt2800_txstatus_timeout(rt2x00dev)) {

		rt2800_txdone(rt2x00dev, UINT_MAX);

		rt2800_txdone_nostatus(rt2x00dev);

		/*
		 * The hw may delay sending the packet after DMA complete
		 * if the medium is busy, thus the TX_STA_FIFO entry is
		 * also delayed -> use a timer to retrieve it.
		 */
		if (rt2800_txstatus_pending(rt2x00dev))
			rt2800usb_async_read_tx_status(rt2x00dev);
	}
}

/*
 * RX control handlers
 */
static void rt2800usb_fill_rxdone(struct queue_entry *entry,
				  struct rxdone_entry_desc *rxdesc)
{
	struct skb_frame_desc *skbdesc = get_skb_frame_desc(entry->skb);
	__le32 *rxi = (__le32 *)entry->skb->data;
	__le32 *rxd;
	u32 word;
	int rx_pkt_len;

	/*
	 * Copy descriptor to the skbdesc->desc buffer, making it safe from
	 * moving of frame data in rt2x00usb.
	 */
	memcpy(skbdesc->desc, rxi, skbdesc->desc_len);

	/*
	 * RX frame format is :
	 * | RXINFO | RXWI | header | L2 pad | payload | pad | RXD | USB pad |
	 *          |<------------ rx_pkt_len -------------->|
	 */
	word = rt2x00_desc_read(rxi, 0);
	rx_pkt_len = rt2x00_get_field32(word, RXINFO_W0_USB_DMA_RX_PKT_LEN);

	/*
	 * Remove the RXINFO structure from the sbk.
	 */
	skb_pull(entry->skb, RXINFO_DESC_SIZE);

	/*
	 * Check for rx_pkt_len validity. Return if invalid, leaving
	 * rxdesc->size zeroed out by the upper level.
	 */
	if (unlikely(rx_pkt_len == 0 ||
			rx_pkt_len > entry->queue->data_size)) {
		rt2x00_err(entry->queue->rt2x00dev,
			   "Bad frame size %d, forcing to 0\n", rx_pkt_len);
		return;
	}

	rxd = (__le32 *)(entry->skb->data + rx_pkt_len);

	/*
	 * It is now safe to read the descriptor on all architectures.
	 */
	word = rt2x00_desc_read(rxd, 0);

	if (rt2x00_get_field32(word, RXD_W0_CRC_ERROR))
		rxdesc->flags |= RX_FLAG_FAILED_FCS_CRC;

	rxdesc->cipher_status = rt2x00_get_field32(word, RXD_W0_CIPHER_ERROR);

	if (rt2x00_get_field32(word, RXD_W0_DECRYPTED)) {
		/*
		 * Hardware has stripped IV/EIV data from 802.11 frame during
		 * decryption. Unfortunately the descriptor doesn't contain
		 * any fields with the EIV/IV data either, so they can't
		 * be restored by rt2x00lib.
		 */
		rxdesc->flags |= RX_FLAG_IV_STRIPPED;

		/*
		 * The hardware has already checked the Michael Mic and has
		 * stripped it from the frame. Signal this to mac80211.
		 */
		rxdesc->flags |= RX_FLAG_MMIC_STRIPPED;

		if (rxdesc->cipher_status == RX_CRYPTO_SUCCESS) {
			rxdesc->flags |= RX_FLAG_DECRYPTED;
		} else if (rxdesc->cipher_status == RX_CRYPTO_FAIL_MIC) {
			/*
			 * In order to check the Michael Mic, the packet must have
			 * been decrypted.  Mac80211 doesnt check the MMIC failure
			 * flag to initiate MMIC countermeasures if the decoded flag
			 * has not been set.
			 */
			rxdesc->flags |= RX_FLAG_DECRYPTED;

			rxdesc->flags |= RX_FLAG_MMIC_ERROR;
		}
	}

	if (rt2x00_get_field32(word, RXD_W0_MY_BSS))
		rxdesc->dev_flags |= RXDONE_MY_BSS;

	if (rt2x00_get_field32(word, RXD_W0_L2PAD))
		rxdesc->dev_flags |= RXDONE_L2PAD;

	/*
	 * Remove RXD descriptor from end of buffer.
	 */
	skb_trim(entry->skb, rx_pkt_len);

	/*
	 * Process the RXWI structure.
	 */
	rt2800_process_rxwi(entry, rxdesc);
}

/*
 * Device probe functions.
 */
static int rt2800usb_efuse_detect(struct rt2x00_dev *rt2x00dev)
{
	int retval;

	retval = rt2800usb_autorun_detect(rt2x00dev);
	if (retval < 0)
		return retval;
	if (retval)
		return 1;
	return rt2800_efuse_detect(rt2x00dev);
}

static int rt2800usb_read_eeprom(struct rt2x00_dev *rt2x00dev)
{
	int retval;

	retval = rt2800usb_efuse_detect(rt2x00dev);
	if (retval < 0)
		return retval;
	if (retval)
		retval = rt2800_read_eeprom_efuse(rt2x00dev);
	else
		retval = rt2x00usb_eeprom_read(rt2x00dev, rt2x00dev->eeprom,
					       EEPROM_SIZE);

	return retval;
}

static int rt2800usb_probe_hw(struct rt2x00_dev *rt2x00dev)
{
	int retval;

	retval = rt2800_probe_hw(rt2x00dev);
	if (retval)
		return retval;

	/*
	 * Set txstatus timer function.
	 */
	rt2x00dev->txstatus_timer.function = rt2800usb_tx_sta_fifo_timeout;

	/*
	 * Overwrite TX done handler
	 */
	INIT_WORK(&rt2x00dev->txdone_work, rt2800usb_work_txdone);

	return 0;
}

static const struct ieee80211_ops rt2800usb_mac80211_ops = {
	.tx			= rt2x00mac_tx,
	.start			= rt2x00mac_start,
	.stop			= rt2x00mac_stop,
	.add_interface		= rt2x00mac_add_interface,
	.remove_interface	= rt2x00mac_remove_interface,
	.config			= rt2x00mac_config,
	.configure_filter	= rt2x00mac_configure_filter,
	.set_tim		= rt2x00mac_set_tim,
	.set_key		= rt2x00mac_set_key,
	.sw_scan_start		= rt2x00mac_sw_scan_start,
	.sw_scan_complete	= rt2x00mac_sw_scan_complete,
	.get_stats		= rt2x00mac_get_stats,
	.get_key_seq		= rt2800_get_key_seq,
	.set_rts_threshold	= rt2800_set_rts_threshold,
	.sta_add		= rt2800_sta_add,
	.sta_remove		= rt2800_sta_remove,
	.bss_info_changed	= rt2x00mac_bss_info_changed,
	.conf_tx		= rt2800_conf_tx,
	.get_tsf		= rt2800_get_tsf,
	.rfkill_poll		= rt2x00mac_rfkill_poll,
	.ampdu_action		= rt2800_ampdu_action,
	.flush			= rt2x00mac_flush,
	.get_survey		= rt2800_get_survey,
	.get_ringparam		= rt2x00mac_get_ringparam,
	.tx_frames_pending	= rt2x00mac_tx_frames_pending,
	.reconfig_complete	= rt2x00mac_reconfig_complete,
};

static const struct rt2800_ops rt2800usb_rt2800_ops = {
	.register_read		= rt2x00usb_register_read,
	.register_read_lock	= rt2x00usb_register_read_lock,
	.register_write		= rt2x00usb_register_write,
	.register_write_lock	= rt2x00usb_register_write_lock,
	.register_multiread	= rt2x00usb_register_multiread,
	.register_multiwrite	= rt2x00usb_register_multiwrite,
	.regbusy_read		= rt2x00usb_regbusy_read,
	.read_eeprom		= rt2800usb_read_eeprom,
	.hwcrypt_disabled	= rt2800usb_hwcrypt_disabled,
	.drv_write_firmware	= rt2800usb_write_firmware,
	.drv_init_registers	= rt2800usb_init_registers,
	.drv_get_txwi		= rt2800usb_get_txwi,
	.drv_get_dma_done	= rt2800usb_get_dma_done,
};

static const struct rt2x00lib_ops rt2800usb_rt2x00_ops = {
	.probe_hw		= rt2800usb_probe_hw,
	.get_firmware_name	= rt2800usb_get_firmware_name,
	.check_firmware		= rt2800_check_firmware,
	.load_firmware		= rt2800_load_firmware,
	.initialize		= rt2x00usb_initialize,
	.uninitialize		= rt2x00usb_uninitialize,
	.clear_entry		= rt2x00usb_clear_entry,
	.set_device_state	= rt2800usb_set_device_state,
	.rfkill_poll		= rt2800_rfkill_poll,
	.link_stats		= rt2800_link_stats,
	.reset_tuner		= rt2800_reset_tuner,
	.link_tuner		= rt2800_link_tuner,
	.gain_calibration	= rt2800_gain_calibration,
	.vco_calibration	= rt2800_vco_calibration,
	.watchdog		= rt2800_watchdog,
	.start_queue		= rt2800usb_start_queue,
	.kick_queue		= rt2x00usb_kick_queue,
	.stop_queue		= rt2800usb_stop_queue,
	.flush_queue		= rt2x00usb_flush_queue,
	.tx_dma_done		= rt2800usb_tx_dma_done,
	.write_tx_desc		= rt2800usb_write_tx_desc,
	.write_tx_data		= rt2800_write_tx_data,
	.write_beacon		= rt2800_write_beacon,
	.clear_beacon		= rt2800_clear_beacon,
	.get_tx_data_len	= rt2800usb_get_tx_data_len,
	.fill_rxdone		= rt2800usb_fill_rxdone,
	.config_shared_key	= rt2800_config_shared_key,
	.config_pairwise_key	= rt2800_config_pairwise_key,
	.config_filter		= rt2800_config_filter,
	.config_intf		= rt2800_config_intf,
	.config_erp		= rt2800_config_erp,
	.config_ant		= rt2800_config_ant,
	.config			= rt2800_config,
	.pre_reset_hw		= rt2800_pre_reset_hw,
};

static void rt2800usb_queue_init(struct data_queue *queue)
{
	struct rt2x00_dev *rt2x00dev = queue->rt2x00dev;
	unsigned short txwi_size, rxwi_size;

	rt2800_get_txwi_rxwi_size(rt2x00dev, &txwi_size, &rxwi_size);

	switch (queue->qid) {
	case QID_RX:
		queue->limit = 128;
		queue->data_size = AGGREGATION_SIZE;
		queue->desc_size = RXINFO_DESC_SIZE;
		queue->winfo_size = rxwi_size;
		queue->priv_size = sizeof(struct queue_entry_priv_usb);
		break;

	case QID_AC_VO:
	case QID_AC_VI:
	case QID_AC_BE:
	case QID_AC_BK:
		queue->limit = 16;
		queue->data_size = AGGREGATION_SIZE;
		queue->desc_size = TXINFO_DESC_SIZE;
		queue->winfo_size = txwi_size;
		queue->priv_size = sizeof(struct queue_entry_priv_usb);
		break;

	case QID_BEACON:
		queue->limit = 8;
		queue->data_size = MGMT_FRAME_SIZE;
		queue->desc_size = TXINFO_DESC_SIZE;
		queue->winfo_size = txwi_size;
		queue->priv_size = sizeof(struct queue_entry_priv_usb);
		break;

	case QID_ATIM:
	default:
		BUG();
		break;
	}
}

static const struct rt2x00_ops rt2800usb_ops = {
	.name			= KBUILD_MODNAME,
	.drv_data_size		= sizeof(struct rt2800_drv_data),
	.max_ap_intf		= 8,
	.eeprom_size		= EEPROM_SIZE,
	.rf_size		= RF_SIZE,
	.tx_queues		= NUM_TX_QUEUES,
	.queue_init		= rt2800usb_queue_init,
	.lib			= &rt2800usb_rt2x00_ops,
	.drv			= &rt2800usb_rt2800_ops,
	.hw			= &rt2800usb_mac80211_ops,
#ifdef CONFIG_RT2X00_LIB_DEBUGFS
	.debugfs		= &rt2800_rt2x00debug,
#endif /* CONFIG_RT2X00_LIB_DEBUGFS */
};

/*
 * rt2800usb module information.
 */
static const struct usb_device_id rt2800usb_device_table[] = {
	/* Abocom */
	{ USB_DEVICE(0x07b8, 0x2870) },
	{ USB_DEVICE(0x07b8, 0x2770) },
	{ USB_DEVICE(0x07b8, 0x3070) },
	{ USB_DEVICE(0x07b8, 0x3071) },
	{ USB_DEVICE(0x07b8, 0x3072) },
	{ USB_DEVICE(0x1482, 0x3c09) },
	/* AirTies */
	{ USB_DEVICE(0x1eda, 0x2012) },
	{ USB_DEVICE(0x1eda, 0x2210) },
	{ USB_DEVICE(0x1eda, 0x2310) },
	/* Allwin */
	{ USB_DEVICE(0x8516, 0x2070) },
	{ USB_DEVICE(0x8516, 0x2770) },
	{ USB_DEVICE(0x8516, 0x2870) },
	{ USB_DEVICE(0x8516, 0x3070) },
	{ USB_DEVICE(0x8516, 0x3071) },
	{ USB_DEVICE(0x8516, 0x3072) },
	/* Alpha Networks */
	{ USB_DEVICE(0x14b2, 0x3c06) },
	{ USB_DEVICE(0x14b2, 0x3c07) },
	{ USB_DEVICE(0x14b2, 0x3c09) },
	{ USB_DEVICE(0x14b2, 0x3c12) },
	{ USB_DEVICE(0x14b2, 0x3c23) },
	{ USB_DEVICE(0x14b2, 0x3c25) },
	{ USB_DEVICE(0x14b2, 0x3c27) },
	{ USB_DEVICE(0x14b2, 0x3c28) },
	{ USB_DEVICE(0x14b2, 0x3c2c) },
	/* Amit */
	{ USB_DEVICE(0x15c5, 0x0008) },
	/* Askey */
	{ USB_DEVICE(0x1690, 0x0740) },
	/* ASUS */
	{ USB_DEVICE(0x0b05, 0x1731) },
	{ USB_DEVICE(0x0b05, 0x1732) },
	{ USB_DEVICE(0x0b05, 0x1742) },
	{ USB_DEVICE(0x0b05, 0x1784) },
	{ USB_DEVICE(0x1761, 0x0b05) },
	/* AzureWave */
	{ USB_DEVICE(0x13d3, 0x3247) },
	{ USB_DEVICE(0x13d3, 0x3273) },
	{ USB_DEVICE(0x13d3, 0x3305) },
	{ USB_DEVICE(0x13d3, 0x3307) },
	{ USB_DEVICE(0x13d3, 0x3321) },
	/* Belkin */
	{ USB_DEVICE(0x050d, 0x8053) },
	{ USB_DEVICE(0x050d, 0x805c) },
	{ USB_DEVICE(0x050d, 0x815c) },
	{ USB_DEVICE(0x050d, 0x825a) },
	{ USB_DEVICE(0x050d, 0x825b) },
	{ USB_DEVICE(0x050d, 0x935a) },
	{ USB_DEVICE(0x050d, 0x935b) },
	/* Buffalo */
	{ USB_DEVICE(0x0411, 0x00e8) },
	{ USB_DEVICE(0x0411, 0x0158) },
	{ USB_DEVICE(0x0411, 0x015d) },
	{ USB_DEVICE(0x0411, 0x016f) },
	{ USB_DEVICE(0x0411, 0x01a2) },
	{ USB_DEVICE(0x0411, 0x01ee) },
	{ USB_DEVICE(0x0411, 0x01a8) },
	{ USB_DEVICE(0x0411, 0x01fd) },
	/* Corega */
	{ USB_DEVICE(0x07aa, 0x002f) },
	{ USB_DEVICE(0x07aa, 0x003c) },
	{ USB_DEVICE(0x07aa, 0x003f) },
	{ USB_DEVICE(0x18c5, 0x0012) },
	/* D-Link */
	{ USB_DEVICE(0x07d1, 0x3c09) },
	{ USB_DEVICE(0x07d1, 0x3c0a) },
	{ USB_DEVICE(0x07d1, 0x3c0d) },
	{ USB_DEVICE(0x07d1, 0x3c0e) },
	{ USB_DEVICE(0x07d1, 0x3c0f) },
	{ USB_DEVICE(0x07d1, 0x3c11) },
	{ USB_DEVICE(0x07d1, 0x3c13) },
	{ USB_DEVICE(0x07d1, 0x3c15) },
	{ USB_DEVICE(0x07d1, 0x3c16) },
	{ USB_DEVICE(0x07d1, 0x3c17) },
	{ USB_DEVICE(0x2001, 0x3317) },
	{ USB_DEVICE(0x2001, 0x3c1b) },
	{ USB_DEVICE(0x2001, 0x3c25) },
	/* Draytek */
	{ USB_DEVICE(0x07fa, 0x7712) },
	/* DVICO */
	{ USB_DEVICE(0x0fe9, 0xb307) },
	/* Edimax */
	{ USB_DEVICE(0x7392, 0x4085) },
	{ USB_DEVICE(0x7392, 0x7711) },
	{ USB_DEVICE(0x7392, 0x7717) },
	{ USB_DEVICE(0x7392, 0x7718) },
	{ USB_DEVICE(0x7392, 0x7722) },
	/* Encore */
	{ USB_DEVICE(0x203d, 0x1480) },
	{ USB_DEVICE(0x203d, 0x14a9) },
	/* EnGenius */
	{ USB_DEVICE(0x1740, 0x9701) },
	{ USB_DEVICE(0x1740, 0x9702) },
	{ USB_DEVICE(0x1740, 0x9703) },
	{ USB_DEVICE(0x1740, 0x9705) },
	{ USB_DEVICE(0x1740, 0x9706) },
	{ USB_DEVICE(0x1740, 0x9707) },
	{ USB_DEVICE(0x1740, 0x9708) },
	{ USB_DEVICE(0x1740, 0x9709) },
	/* Gemtek */
	{ USB_DEVICE(0x15a9, 0x0012) },
	/* Gigabyte */
	{ USB_DEVICE(0x1044, 0x800b) },
	{ USB_DEVICE(0x1044, 0x800d) },
	/* Hawking */
	{ USB_DEVICE(0x0e66, 0x0001) },
	{ USB_DEVICE(0x0e66, 0x0003) },
	{ USB_DEVICE(0x0e66, 0x0009) },
	{ USB_DEVICE(0x0e66, 0x000b) },
	{ USB_DEVICE(0x0e66, 0x0013) },
	{ USB_DEVICE(0x0e66, 0x0017) },
	{ USB_DEVICE(0x0e66, 0x0018) },
	/* I-O DATA */
	{ USB_DEVICE(0x04bb, 0x0945) },
	{ USB_DEVICE(0x04bb, 0x0947) },
	{ USB_DEVICE(0x04bb, 0x0948) },
	/* Linksys */
	{ USB_DEVICE(0x13b1, 0x0031) },
	{ USB_DEVICE(0x1737, 0x0070) },
	{ USB_DEVICE(0x1737, 0x0071) },
	{ USB_DEVICE(0x1737, 0x0077) },
	{ USB_DEVICE(0x1737, 0x0078) },
	/* Logitec */
	{ USB_DEVICE(0x0789, 0x0162) },
	{ USB_DEVICE(0x0789, 0x0163) },
	{ USB_DEVICE(0x0789, 0x0164) },
	{ USB_DEVICE(0x0789, 0x0166) },
	/* Motorola */
	{ USB_DEVICE(0x100d, 0x9031) },
	/* MSI */
	{ USB_DEVICE(0x0db0, 0x3820) },
	{ USB_DEVICE(0x0db0, 0x3821) },
	{ USB_DEVICE(0x0db0, 0x3822) },
	{ USB_DEVICE(0x0db0, 0x3870) },
	{ USB_DEVICE(0x0db0, 0x3871) },
	{ USB_DEVICE(0x0db0, 0x6899) },
	{ USB_DEVICE(0x0db0, 0x821a) },
	{ USB_DEVICE(0x0db0, 0x822a) },
	{ USB_DEVICE(0x0db0, 0x822b) },
	{ USB_DEVICE(0x0db0, 0x822c) },
	{ USB_DEVICE(0x0db0, 0x870a) },
	{ USB_DEVICE(0x0db0, 0x871a) },
	{ USB_DEVICE(0x0db0, 0x871b) },
	{ USB_DEVICE(0x0db0, 0x871c) },
	{ USB_DEVICE(0x0db0, 0x899a) },
	/* Ovislink */
	{ USB_DEVICE(0x1b75, 0x3070) },
	{ USB_DEVICE(0x1b75, 0x3071) },
	{ USB_DEVICE(0x1b75, 0x3072) },
	{ USB_DEVICE(0x1b75, 0xa200) },
	/* Para */
	{ USB_DEVICE(0x20b8, 0x8888) },
	/* Pegatron */
	{ USB_DEVICE(0x1d4d, 0x0002) },
	{ USB_DEVICE(0x1d4d, 0x000c) },
	{ USB_DEVICE(0x1d4d, 0x000e) },
	{ USB_DEVICE(0x1d4d, 0x0011) },
	/* Philips */
	{ USB_DEVICE(0x0471, 0x200f) },
	/* Planex */
	{ USB_DEVICE(0x2019, 0x5201) },
	{ USB_DEVICE(0x2019, 0xab25) },
	{ USB_DEVICE(0x2019, 0xed06) },
	/* Quanta */
	{ USB_DEVICE(0x1a32, 0x0304) },
	/* Ralink */
	{ USB_DEVICE(0x148f, 0x2070) },
	{ USB_DEVICE(0x148f, 0x2770) },
	{ USB_DEVICE(0x148f, 0x2870) },
	{ USB_DEVICE(0x148f, 0x3070) },
	{ USB_DEVICE(0x148f, 0x3071) },
	{ USB_DEVICE(0x148f, 0x3072) },
	/* Samsung */
	{ USB_DEVICE(0x04e8, 0x2018) },
	/* Siemens */
	{ USB_DEVICE(0x129b, 0x1828) },
	/* Sitecom */
	{ USB_DEVICE(0x0df6, 0x0017) },
	{ USB_DEVICE(0x0df6, 0x002b) },
	{ USB_DEVICE(0x0df6, 0x002c) },
	{ USB_DEVICE(0x0df6, 0x002d) },
	{ USB_DEVICE(0x0df6, 0x0039) },
	{ USB_DEVICE(0x0df6, 0x003b) },
	{ USB_DEVICE(0x0df6, 0x003d) },
	{ USB_DEVICE(0x0df6, 0x003e) },
	{ USB_DEVICE(0x0df6, 0x003f) },
	{ USB_DEVICE(0x0df6, 0x0040) },
	{ USB_DEVICE(0x0df6, 0x0042) },
	{ USB_DEVICE(0x0df6, 0x0047) },
	{ USB_DEVICE(0x0df6, 0x0048) },
	{ USB_DEVICE(0x0df6, 0x0051) },
	{ USB_DEVICE(0x0df6, 0x005f) },
	{ USB_DEVICE(0x0df6, 0x0060) },
	/* SMC */
	{ USB_DEVICE(0x083a, 0x6618) },
	{ USB_DEVICE(0x083a, 0x7511) },
	{ USB_DEVICE(0x083a, 0x7512) },
	{ USB_DEVICE(0x083a, 0x7522) },
	{ USB_DEVICE(0x083a, 0x8522) },
	{ USB_DEVICE(0x083a, 0xa618) },
	{ USB_DEVICE(0x083a, 0xa701) },
	{ USB_DEVICE(0x083a, 0xa702) },
	{ USB_DEVICE(0x083a, 0xa703) },
	{ USB_DEVICE(0x083a, 0xb522) },
	/* Sparklan */
	{ USB_DEVICE(0x15a9, 0x0006) },
	/* Sweex */
	{ USB_DEVICE(0x177f, 0x0153) },
	{ USB_DEVICE(0x177f, 0x0164) },
	{ USB_DEVICE(0x177f, 0x0302) },
	{ USB_DEVICE(0x177f, 0x0313) },
	{ USB_DEVICE(0x177f, 0x0323) },
	{ USB_DEVICE(0x177f, 0x0324) },
	{ USB_DEVICE(0x177f, 0x1163) },
	/* U-Media */
	{ USB_DEVICE(0x157e, 0x300e) },
	{ USB_DEVICE(0x157e, 0x3013) },
	/* ZCOM */
	{ USB_DEVICE(0x0cde, 0x0022) },
	{ USB_DEVICE(0x0cde, 0x0025) },
	/* Zinwell */
	{ USB_DEVICE(0x5a57, 0x0280) },
	{ USB_DEVICE(0x5a57, 0x0282) },
	{ USB_DEVICE(0x5a57, 0x0283) },
	{ USB_DEVICE(0x5a57, 0x5257) },
	/* Zyxel */
	{ USB_DEVICE(0x0586, 0x3416) },
	{ USB_DEVICE(0x0586, 0x3418) },
	{ USB_DEVICE(0x0586, 0x341a) },
	{ USB_DEVICE(0x0586, 0x341e) },
	{ USB_DEVICE(0x0586, 0x343e) },
#ifdef CONFIG_RT2800USB_RT33XX
	/* Belkin */
	{ USB_DEVICE(0x050d, 0x945b) },
	/* D-Link */
	{ USB_DEVICE(0x2001, 0x3c17) },
	/* Panasonic */
	{ USB_DEVICE(0x083a, 0xb511) },
	/* Accton/Arcadyan/Epson */
	{ USB_DEVICE(0x083a, 0xb512) },
	/* Philips */
	{ USB_DEVICE(0x0471, 0x20dd) },
	/* Ralink */
	{ USB_DEVICE(0x148f, 0x3370) },
	{ USB_DEVICE(0x148f, 0x8070) },
	/* Sitecom */
	{ USB_DEVICE(0x0df6, 0x0050) },
	/* Sweex */
	{ USB_DEVICE(0x177f, 0x0163) },
	{ USB_DEVICE(0x177f, 0x0165) },
#endif
#ifdef CONFIG_RT2800USB_RT35XX
	/* Allwin */
	{ USB_DEVICE(0x8516, 0x3572) },
	/* Askey */
	{ USB_DEVICE(0x1690, 0x0744) },
	{ USB_DEVICE(0x1690, 0x0761) },
	{ USB_DEVICE(0x1690, 0x0764) },
	/* ASUS */
	{ USB_DEVICE(0x0b05, 0x179d) },
	/* Cisco */
	{ USB_DEVICE(0x167b, 0x4001) },
	/* EnGenius */
	{ USB_DEVICE(0x1740, 0x9801) },
	/* I-O DATA */
	{ USB_DEVICE(0x04bb, 0x0944) },
	/* Linksys */
	{ USB_DEVICE(0x13b1, 0x002f) },
	{ USB_DEVICE(0x1737, 0x0079) },
	/* Logitec */
	{ USB_DEVICE(0x0789, 0x0170) },
	/* Ralink */
	{ USB_DEVICE(0x148f, 0x3572) },
	/* Sitecom */
	{ USB_DEVICE(0x0df6, 0x0041) },
	{ USB_DEVICE(0x0df6, 0x0062) },
	{ USB_DEVICE(0x0df6, 0x0065) },
	{ USB_DEVICE(0x0df6, 0x0066) },
	{ USB_DEVICE(0x0df6, 0x0068) },
	/* Toshiba */
	{ USB_DEVICE(0x0930, 0x0a07) },
	/* Zinwell */
	{ USB_DEVICE(0x5a57, 0x0284) },
#endif
#ifdef CONFIG_RT2800USB_RT3573
	/* AirLive */
	{ USB_DEVICE(0x1b75, 0x7733) },
	/* ASUS */
	{ USB_DEVICE(0x0b05, 0x17bc) },
	{ USB_DEVICE(0x0b05, 0x17ad) },
	/* Belkin */
	{ USB_DEVICE(0x050d, 0x1103) },
	/* Cameo */
	{ USB_DEVICE(0x148f, 0xf301) },
	/* D-Link */
	{ USB_DEVICE(0x2001, 0x3c1f) },
	/* Edimax */
	{ USB_DEVICE(0x7392, 0x7733) },
	/* Hawking */
	{ USB_DEVICE(0x0e66, 0x0020) },
	{ USB_DEVICE(0x0e66, 0x0021) },
	/* I-O DATA */
	{ USB_DEVICE(0x04bb, 0x094e) },
	/* Linksys */
	{ USB_DEVICE(0x13b1, 0x003b) },
	/* Logitec */
	{ USB_DEVICE(0x0789, 0x016b) },
	/* NETGEAR */
	{ USB_DEVICE(0x0846, 0x9012) },
	{ USB_DEVICE(0x0846, 0x9013) },
	{ USB_DEVICE(0x0846, 0x9019) },
	/* Planex */
	{ USB_DEVICE(0x2019, 0xed14) },
	{ USB_DEVICE(0x2019, 0xed19) },
	/* Ralink */
	{ USB_DEVICE(0x148f, 0x3573) },
	/* Sitecom */
	{ USB_DEVICE(0x0df6, 0x0067) },
	{ USB_DEVICE(0x0df6, 0x006a) },
	{ USB_DEVICE(0x0df6, 0x006e) },
	/* ZyXEL */
	{ USB_DEVICE(0x0586, 0x3421) },
#endif
#ifdef CONFIG_RT2800USB_RT53XX
	/* Arcadyan */
	{ USB_DEVICE(0x043e, 0x7a12) },
	{ USB_DEVICE(0x043e, 0x7a32) },
	/* ASUS */
	{ USB_DEVICE(0x0b05, 0x17e8) },
	/* Azurewave */
	{ USB_DEVICE(0x13d3, 0x3329) },
	{ USB_DEVICE(0x13d3, 0x3365) },
	/* D-Link */
	{ USB_DEVICE(0x2001, 0x3c15) },
	{ USB_DEVICE(0x2001, 0x3c19) },
	{ USB_DEVICE(0x2001, 0x3c1c) },
	{ USB_DEVICE(0x2001, 0x3c1d) },
	{ USB_DEVICE(0x2001, 0x3c1e) },
	{ USB_DEVICE(0x2001, 0x3c20) },
	{ USB_DEVICE(0x2001, 0x3c22) },
	{ USB_DEVICE(0x2001, 0x3c23) },
	/* LG innotek */
	{ USB_DEVICE(0x043e, 0x7a22) },
	{ USB_DEVICE(0x043e, 0x7a42) },
	/* Panasonic */
	{ USB_DEVICE(0x04da, 0x1801) },
	{ USB_DEVICE(0x04da, 0x1800) },
	{ USB_DEVICE(0x04da, 0x23f6) },
	/* Philips */
	{ USB_DEVICE(0x0471, 0x2104) },
	{ USB_DEVICE(0x0471, 0x2126) },
	{ USB_DEVICE(0x0471, 0x2180) },
	{ USB_DEVICE(0x0471, 0x2181) },
	{ USB_DEVICE(0x0471, 0x2182) },
	/* Ralink */
	{ USB_DEVICE(0x148f, 0x5370) },
	{ USB_DEVICE(0x148f, 0x5372) },
#endif
#ifdef CONFIG_RT2800USB_RT55XX
	/* Arcadyan */
	{ USB_DEVICE(0x043e, 0x7a32) },
	/* AVM GmbH */
	{ USB_DEVICE(0x057c, 0x8501) },
	/* Buffalo */
	{ USB_DEVICE(0x0411, 0x0241) },
	{ USB_DEVICE(0x0411, 0x0253) },
	/* D-Link */
	{ USB_DEVICE(0x2001, 0x3c1a) },
	{ USB_DEVICE(0x2001, 0x3c21) },
	/* Proware */
	{ USB_DEVICE(0x043e, 0x7a13) },
	/* Ralink */
	{ USB_DEVICE(0x148f, 0x5572) },
	/* TRENDnet */
	{ USB_DEVICE(0x20f4, 0x724a) },
#endif
#ifdef CONFIG_RT2800USB_UNKNOWN
	/*
	 * Unclear what kind of devices these are (they aren't supported by the
	 * vendor linux driver).
	 */
	/* Abocom */
	{ USB_DEVICE(0x07b8, 0x3073) },
	{ USB_DEVICE(0x07b8, 0x3074) },
	/* Alpha Networks */
	{ USB_DEVICE(0x14b2, 0x3c08) },
	{ USB_DEVICE(0x14b2, 0x3c11) },
	/* Amigo */
	{ USB_DEVICE(0x0e0b, 0x9031) },
	{ USB_DEVICE(0x0e0b, 0x9041) },
	/* ASUS */
	{ USB_DEVICE(0x0b05, 0x166a) },
	{ USB_DEVICE(0x0b05, 0x1760) },
	{ USB_DEVICE(0x0b05, 0x1761) },
	{ USB_DEVICE(0x0b05, 0x1790) },
	{ USB_DEVICE(0x0b05, 0x17a7) },
	/* AzureWave */
	{ USB_DEVICE(0x13d3, 0x3262) },
	{ USB_DEVICE(0x13d3, 0x3284) },
	{ USB_DEVICE(0x13d3, 0x3322) },
	{ USB_DEVICE(0x13d3, 0x3340) },
	{ USB_DEVICE(0x13d3, 0x3399) },
	{ USB_DEVICE(0x13d3, 0x3400) },
	{ USB_DEVICE(0x13d3, 0x3401) },
	/* Belkin */
	{ USB_DEVICE(0x050d, 0x1003) },
	/* Buffalo */
	{ USB_DEVICE(0x0411, 0x012e) },
	{ USB_DEVICE(0x0411, 0x0148) },
	{ USB_DEVICE(0x0411, 0x0150) },
	/* Corega */
	{ USB_DEVICE(0x07aa, 0x0041) },
	{ USB_DEVICE(0x07aa, 0x0042) },
	{ USB_DEVICE(0x18c5, 0x0008) },
	/* D-Link */
	{ USB_DEVICE(0x07d1, 0x3c0b) },
	/* Encore */
	{ USB_DEVICE(0x203d, 0x14a1) },
	/* EnGenius */
	{ USB_DEVICE(0x1740, 0x0600) },
	{ USB_DEVICE(0x1740, 0x0602) },
	/* Gemtek */
	{ USB_DEVICE(0x15a9, 0x0010) },
	/* Gigabyte */
	{ USB_DEVICE(0x1044, 0x800c) },
	/* Hercules */
	{ USB_DEVICE(0x06f8, 0xe036) },
	/* Huawei */
	{ USB_DEVICE(0x148f, 0xf101) },
	/* I-O DATA */
	{ USB_DEVICE(0x04bb, 0x094b) },
	/* LevelOne */
	{ USB_DEVICE(0x1740, 0x0605) },
	{ USB_DEVICE(0x1740, 0x0615) },
	/* Logitec */
	{ USB_DEVICE(0x0789, 0x0168) },
	{ USB_DEVICE(0x0789, 0x0169) },
	/* Motorola */
	{ USB_DEVICE(0x100d, 0x9032) },
	/* Pegatron */
	{ USB_DEVICE(0x05a6, 0x0101) },
	{ USB_DEVICE(0x1d4d, 0x0010) },
	/* Planex */
	{ USB_DEVICE(0x2019, 0xab24) },
	{ USB_DEVICE(0x2019, 0xab29) },
	/* Qcom */
	{ USB_DEVICE(0x18e8, 0x6259) },
	/* RadioShack */
	{ USB_DEVICE(0x08b9, 0x1197) },
	/* Sitecom */
	{ USB_DEVICE(0x0df6, 0x003c) },
	{ USB_DEVICE(0x0df6, 0x004a) },
	{ USB_DEVICE(0x0df6, 0x004d) },
	{ USB_DEVICE(0x0df6, 0x0053) },
	{ USB_DEVICE(0x0df6, 0x0069) },
	{ USB_DEVICE(0x0df6, 0x006f) },
	{ USB_DEVICE(0x0df6, 0x0078) },
	/* SMC */
	{ USB_DEVICE(0x083a, 0xa512) },
	{ USB_DEVICE(0x083a, 0xc522) },
	{ USB_DEVICE(0x083a, 0xd522) },
	{ USB_DEVICE(0x083a, 0xf511) },
	/* Sweex */
	{ USB_DEVICE(0x177f, 0x0254) },
	/* TP-LINK */
	{ USB_DEVICE(0xf201, 0x5370) },
#endif
	{ 0, }
};

MODULE_AUTHOR(DRV_PROJECT);
MODULE_VERSION(DRV_VERSION);
MODULE_DESCRIPTION("Ralink RT2800 USB Wireless LAN driver.");
MODULE_DEVICE_TABLE(usb, rt2800usb_device_table);
MODULE_FIRMWARE(FIRMWARE_RT2870);
MODULE_LICENSE("GPL");

static int rt2800usb_probe(struct usb_interface *usb_intf,
			   const struct usb_device_id *id)
{
	return rt2x00usb_probe(usb_intf, &rt2800usb_ops);
}

static struct usb_driver rt2800usb_driver = {
	.name		= KBUILD_MODNAME,
	.id_table	= rt2800usb_device_table,
	.probe		= rt2800usb_probe,
	.disconnect	= rt2x00usb_disconnect,
	.suspend	= rt2x00usb_suspend,
	.resume		= rt2x00usb_resume,
	.reset_resume	= rt2x00usb_resume,
	.disable_hub_initiated_lpm = 1,
};

module_usb_driver(rt2800usb_driver);
