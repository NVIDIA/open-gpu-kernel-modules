/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
	Copyright (C) 2004 - 2009 Ivo van Doorn <IvDoorn@gmail.com>
	Copyright (C) 2004 - 2009 Gertjan van Wingerde <gwingerde@gmail.com>
	<http://rt2x00.serialmonkey.com>

 */

/*
	Module: rt2x00lib
	Abstract: Data structures and definitions for the rt2x00lib module.
 */

#ifndef RT2X00LIB_H
#define RT2X00LIB_H

/*
 * Interval defines
 */
#define WATCHDOG_INTERVAL	round_jiffies_relative(HZ)
#define LINK_TUNE_SECONDS	1
#define LINK_TUNE_INTERVAL	round_jiffies_relative(LINK_TUNE_SECONDS * HZ)
#define AGC_SECONDS		4
#define VCO_SECONDS		10

/*
 * rt2x00_rate: Per rate device information
 */
struct rt2x00_rate {
	unsigned short flags;
#define DEV_RATE_CCK			0x0001
#define DEV_RATE_OFDM			0x0002
#define DEV_RATE_SHORT_PREAMBLE		0x0004

	unsigned short bitrate; /* In 100kbit/s */
	unsigned short ratemask;

	unsigned short plcp;
	unsigned short mcs;
};

extern const struct rt2x00_rate rt2x00_supported_rates[12];

static inline const struct rt2x00_rate *rt2x00_get_rate(const u16 hw_value)
{
	return &rt2x00_supported_rates[hw_value & 0xff];
}

#define RATE_MCS(__mode, __mcs) \
	((((__mode) & 0x00ff) << 8) | ((__mcs) & 0x00ff))

static inline int rt2x00_get_rate_mcs(const u16 mcs_value)
{
	return (mcs_value & 0x00ff);
}

/*
 * Radio control handlers.
 */
int rt2x00lib_enable_radio(struct rt2x00_dev *rt2x00dev);
void rt2x00lib_disable_radio(struct rt2x00_dev *rt2x00dev);

/*
 * Initialization handlers.
 */
int rt2x00lib_start(struct rt2x00_dev *rt2x00dev);
void rt2x00lib_stop(struct rt2x00_dev *rt2x00dev);

/*
 * Configuration handlers.
 */
void rt2x00lib_config_intf(struct rt2x00_dev *rt2x00dev,
			   struct rt2x00_intf *intf,
			   enum nl80211_iftype type,
			   const u8 *mac, const u8 *bssid);
void rt2x00lib_config_erp(struct rt2x00_dev *rt2x00dev,
			  struct rt2x00_intf *intf,
			  struct ieee80211_bss_conf *conf,
			  u32 changed);
void rt2x00lib_config_antenna(struct rt2x00_dev *rt2x00dev,
			      struct antenna_setup ant);
void rt2x00lib_config(struct rt2x00_dev *rt2x00dev,
		      struct ieee80211_conf *conf,
		      const unsigned int changed_flags);

/**
 * DOC: Queue handlers
 */

/**
 * rt2x00queue_alloc_rxskb - allocate a skb for RX purposes.
 * @entry: The entry for which the skb will be applicable.
 */
struct sk_buff *rt2x00queue_alloc_rxskb(struct queue_entry *entry, gfp_t gfp);

/**
 * rt2x00queue_free_skb - free a skb
 * @entry: The entry for which the skb will be applicable.
 */
void rt2x00queue_free_skb(struct queue_entry *entry);

/**
 * rt2x00queue_align_frame - Align 802.11 frame to 4-byte boundary
 * @skb: The skb to align
 *
 * Align the start of the 802.11 frame to a 4-byte boundary, this could
 * mean the payload is not aligned properly though.
 */
void rt2x00queue_align_frame(struct sk_buff *skb);

/**
 * rt2x00queue_insert_l2pad - Align 802.11 header & payload to 4-byte boundary
 * @skb: The skb to align
 * @header_length: Length of 802.11 header
 *
 * Apply L2 padding to align both header and payload to 4-byte boundary
 */
void rt2x00queue_insert_l2pad(struct sk_buff *skb, unsigned int header_length);

/**
 * rt2x00queue_insert_l2pad - Remove L2 padding from 802.11 frame
 * @skb: The skb to align
 * @header_length: Length of 802.11 header
 *
 * Remove L2 padding used to align both header and payload to 4-byte boundary,
 * by removing the L2 padding the header will no longer be 4-byte aligned.
 */
void rt2x00queue_remove_l2pad(struct sk_buff *skb, unsigned int header_length);

/**
 * rt2x00queue_write_tx_frame - Write TX frame to hardware
 * @queue: Queue over which the frame should be send
 * @skb: The skb to send
 * @local: frame is not from mac80211
 */
int rt2x00queue_write_tx_frame(struct data_queue *queue, struct sk_buff *skb,
			       struct ieee80211_sta *sta, bool local);

/**
 * rt2x00queue_update_beacon - Send new beacon from mac80211
 *	to hardware. Handles locking by itself (mutex).
 * @rt2x00dev: Pointer to &struct rt2x00_dev.
 * @vif: Interface for which the beacon should be updated.
 */
int rt2x00queue_update_beacon(struct rt2x00_dev *rt2x00dev,
			      struct ieee80211_vif *vif);

/**
 * rt2x00queue_update_beacon_locked - Send new beacon from mac80211
 *	to hardware. Caller needs to ensure locking.
 * @rt2x00dev: Pointer to &struct rt2x00_dev.
 * @vif: Interface for which the beacon should be updated.
 */
int rt2x00queue_update_beacon_locked(struct rt2x00_dev *rt2x00dev,
				     struct ieee80211_vif *vif);

/**
 * rt2x00queue_clear_beacon - Clear beacon in hardware
 * @rt2x00dev: Pointer to &struct rt2x00_dev.
 * @vif: Interface for which the beacon should be updated.
 */
int rt2x00queue_clear_beacon(struct rt2x00_dev *rt2x00dev,
			     struct ieee80211_vif *vif);

/**
 * rt2x00queue_index_inc - Index incrementation function
 * @entry: Queue entry (&struct queue_entry) to perform the action on.
 * @index: Index type (&enum queue_index) to perform the action on.
 *
 * This function will increase the requested index on the entry's queue,
 * it will grab the appropriate locks and handle queue overflow events by
 * resetting the index to the start of the queue.
 */
void rt2x00queue_index_inc(struct queue_entry *entry, enum queue_index index);

/**
 * rt2x00queue_init_queues - Initialize all data queues
 * @rt2x00dev: Pointer to &struct rt2x00_dev.
 *
 * This function will loop through all available queues to clear all
 * index numbers and set the queue entry to the correct initialization
 * state.
 */
void rt2x00queue_init_queues(struct rt2x00_dev *rt2x00dev);

int rt2x00queue_initialize(struct rt2x00_dev *rt2x00dev);
void rt2x00queue_uninitialize(struct rt2x00_dev *rt2x00dev);
int rt2x00queue_allocate(struct rt2x00_dev *rt2x00dev);
void rt2x00queue_free(struct rt2x00_dev *rt2x00dev);

/**
 * rt2x00link_update_stats - Update link statistics from RX frame
 * @rt2x00dev: Pointer to &struct rt2x00_dev.
 * @skb: Received frame
 * @rxdesc: Received frame descriptor
 *
 * Update link statistics based on the information from the
 * received frame descriptor.
 */
void rt2x00link_update_stats(struct rt2x00_dev *rt2x00dev,
			     struct sk_buff *skb,
			     struct rxdone_entry_desc *rxdesc);

/**
 * rt2x00link_start_tuner - Start periodic link tuner work
 * @rt2x00dev: Pointer to &struct rt2x00_dev.
 *
 * This start the link tuner periodic work, this work will
 * be executed periodically until &rt2x00link_stop_tuner has
 * been called.
 */
void rt2x00link_start_tuner(struct rt2x00_dev *rt2x00dev);

/**
 * rt2x00link_stop_tuner - Stop periodic link tuner work
 * @rt2x00dev: Pointer to &struct rt2x00_dev.
 *
 * After this function completed the link tuner will not
 * be running until &rt2x00link_start_tuner is called.
 */
void rt2x00link_stop_tuner(struct rt2x00_dev *rt2x00dev);

/**
 * rt2x00link_reset_tuner - Reset periodic link tuner work
 * @rt2x00dev: Pointer to &struct rt2x00_dev.
 * @antenna: Should the antenna tuning also be reset
 *
 * The VGC limit configured in the hardware will be reset to 0
 * which forces the driver to rediscover the correct value for
 * the current association. This is needed when configuration
 * options have changed which could drastically change the
 * SNR level or link quality (i.e. changing the antenna setting).
 *
 * Resetting the link tuner will also cause the periodic work counter
 * to be reset. Any driver which has a fixed limit on the number
 * of rounds the link tuner is supposed to work will accept the
 * tuner actions again if this limit was previously reached.
 *
 * If @antenna is set to true a the software antenna diversity
 * tuning will also be reset.
 */
void rt2x00link_reset_tuner(struct rt2x00_dev *rt2x00dev, bool antenna);

/**
 * rt2x00link_start_watchdog - Start periodic watchdog monitoring
 * @rt2x00dev: Pointer to &struct rt2x00_dev.
 *
 * This start the watchdog periodic work, this work will
 *be executed periodically until &rt2x00link_stop_watchdog has
 * been called.
 */
void rt2x00link_start_watchdog(struct rt2x00_dev *rt2x00dev);

/**
 * rt2x00link_stop_watchdog - Stop periodic watchdog monitoring
 * @rt2x00dev: Pointer to &struct rt2x00_dev.
 *
 * After this function completed the watchdog monitoring will not
 * be running until &rt2x00link_start_watchdog is called.
 */
void rt2x00link_stop_watchdog(struct rt2x00_dev *rt2x00dev);

/**
 * rt2x00link_register - Initialize link tuning & watchdog functionality
 * @rt2x00dev: Pointer to &struct rt2x00_dev.
 *
 * Initialize work structure and all link tuning and watchdog related
 * parameters. This will not start the periodic work itself.
 */
void rt2x00link_register(struct rt2x00_dev *rt2x00dev);

/*
 * Firmware handlers.
 */
#ifdef CONFIG_RT2X00_LIB_FIRMWARE
int rt2x00lib_load_firmware(struct rt2x00_dev *rt2x00dev);
void rt2x00lib_free_firmware(struct rt2x00_dev *rt2x00dev);
#else
static inline int rt2x00lib_load_firmware(struct rt2x00_dev *rt2x00dev)
{
	return 0;
}
static inline void rt2x00lib_free_firmware(struct rt2x00_dev *rt2x00dev)
{
}
#endif /* CONFIG_RT2X00_LIB_FIRMWARE */

/*
 * Debugfs handlers.
 */
#ifdef CONFIG_RT2X00_LIB_DEBUGFS
void rt2x00debug_register(struct rt2x00_dev *rt2x00dev);
void rt2x00debug_deregister(struct rt2x00_dev *rt2x00dev);
void rt2x00debug_update_crypto(struct rt2x00_dev *rt2x00dev,
			       struct rxdone_entry_desc *rxdesc);
#else
static inline void rt2x00debug_register(struct rt2x00_dev *rt2x00dev)
{
}

static inline void rt2x00debug_deregister(struct rt2x00_dev *rt2x00dev)
{
}

static inline void rt2x00debug_update_crypto(struct rt2x00_dev *rt2x00dev,
					     struct rxdone_entry_desc *rxdesc)
{
}
#endif /* CONFIG_RT2X00_LIB_DEBUGFS */

/*
 * Crypto handlers.
 */
#ifdef CONFIG_RT2X00_LIB_CRYPTO
enum cipher rt2x00crypto_key_to_cipher(struct ieee80211_key_conf *key);
void rt2x00crypto_create_tx_descriptor(struct rt2x00_dev *rt2x00dev,
				       struct sk_buff *skb,
				       struct txentry_desc *txdesc);
unsigned int rt2x00crypto_tx_overhead(struct rt2x00_dev *rt2x00dev,
				      struct sk_buff *skb);
void rt2x00crypto_tx_copy_iv(struct sk_buff *skb,
			     struct txentry_desc *txdesc);
void rt2x00crypto_tx_remove_iv(struct sk_buff *skb,
			       struct txentry_desc *txdesc);
void rt2x00crypto_tx_insert_iv(struct sk_buff *skb, unsigned int header_length);
void rt2x00crypto_rx_insert_iv(struct sk_buff *skb,
			       unsigned int header_length,
			       struct rxdone_entry_desc *rxdesc);
#else
static inline enum cipher rt2x00crypto_key_to_cipher(struct ieee80211_key_conf *key)
{
	return CIPHER_NONE;
}

static inline void rt2x00crypto_create_tx_descriptor(struct rt2x00_dev *rt2x00dev,
						     struct sk_buff *skb,
						     struct txentry_desc *txdesc)
{
}

static inline unsigned int rt2x00crypto_tx_overhead(struct rt2x00_dev *rt2x00dev,
						    struct sk_buff *skb)
{
	return 0;
}

static inline void rt2x00crypto_tx_copy_iv(struct sk_buff *skb,
					   struct txentry_desc *txdesc)
{
}

static inline void rt2x00crypto_tx_remove_iv(struct sk_buff *skb,
					     struct txentry_desc *txdesc)
{
}

static inline void rt2x00crypto_tx_insert_iv(struct sk_buff *skb,
					     unsigned int header_length)
{
}

static inline void rt2x00crypto_rx_insert_iv(struct sk_buff *skb,
					     unsigned int header_length,
					     struct rxdone_entry_desc *rxdesc)
{
}
#endif /* CONFIG_RT2X00_LIB_CRYPTO */

/*
 * RFkill handlers.
 */
static inline void rt2x00rfkill_register(struct rt2x00_dev *rt2x00dev)
{
	if (test_bit(CAPABILITY_HW_BUTTON, &rt2x00dev->cap_flags))
		wiphy_rfkill_start_polling(rt2x00dev->hw->wiphy);
}

static inline void rt2x00rfkill_unregister(struct rt2x00_dev *rt2x00dev)
{
	if (test_bit(CAPABILITY_HW_BUTTON, &rt2x00dev->cap_flags))
		wiphy_rfkill_stop_polling(rt2x00dev->hw->wiphy);
}

/*
 * LED handlers
 */
#ifdef CONFIG_RT2X00_LIB_LEDS
void rt2x00leds_led_quality(struct rt2x00_dev *rt2x00dev, int rssi);
void rt2x00led_led_activity(struct rt2x00_dev *rt2x00dev, bool enabled);
void rt2x00leds_led_assoc(struct rt2x00_dev *rt2x00dev, bool enabled);
void rt2x00leds_led_radio(struct rt2x00_dev *rt2x00dev, bool enabled);
void rt2x00leds_register(struct rt2x00_dev *rt2x00dev);
void rt2x00leds_unregister(struct rt2x00_dev *rt2x00dev);
void rt2x00leds_suspend(struct rt2x00_dev *rt2x00dev);
void rt2x00leds_resume(struct rt2x00_dev *rt2x00dev);
#else
static inline void rt2x00leds_led_quality(struct rt2x00_dev *rt2x00dev,
					  int rssi)
{
}

static inline void rt2x00led_led_activity(struct rt2x00_dev *rt2x00dev,
					  bool enabled)
{
}

static inline void rt2x00leds_led_assoc(struct rt2x00_dev *rt2x00dev,
					bool enabled)
{
}

static inline void rt2x00leds_led_radio(struct rt2x00_dev *rt2x00dev,
					bool enabled)
{
}

static inline void rt2x00leds_register(struct rt2x00_dev *rt2x00dev)
{
}

static inline void rt2x00leds_unregister(struct rt2x00_dev *rt2x00dev)
{
}

static inline void rt2x00leds_suspend(struct rt2x00_dev *rt2x00dev)
{
}

static inline void rt2x00leds_resume(struct rt2x00_dev *rt2x00dev)
{
}
#endif /* CONFIG_RT2X00_LIB_LEDS */

#endif /* RT2X00LIB_H */
