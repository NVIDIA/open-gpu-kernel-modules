/* SPDX-License-Identifier: GPL-2.0 */
/*
 * ethtool.h: Defines for Linux ethtool.
 *
 * Copyright (C) 1998 David S. Miller (davem@redhat.com)
 * Copyright 2001 Jeff Garzik <jgarzik@pobox.com>
 * Portions Copyright 2001 Sun Microsystems (thockin@sun.com)
 * Portions Copyright 2002 Intel (eli.kupermann@intel.com,
 *                                christopher.leech@intel.com,
 *                                scott.feldman@intel.com)
 * Portions Copyright (C) Sun Microsystems 2008
 */
#ifndef _LINUX_ETHTOOL_H
#define _LINUX_ETHTOOL_H

#include <linux/bitmap.h>
#include <linux/compat.h>
#include <uapi/linux/ethtool.h>

#ifdef CONFIG_COMPAT

struct compat_ethtool_rx_flow_spec {
	u32		flow_type;
	union ethtool_flow_union h_u;
	struct ethtool_flow_ext h_ext;
	union ethtool_flow_union m_u;
	struct ethtool_flow_ext m_ext;
	compat_u64	ring_cookie;
	u32		location;
};

struct compat_ethtool_rxnfc {
	u32				cmd;
	u32				flow_type;
	compat_u64			data;
	struct compat_ethtool_rx_flow_spec fs;
	u32				rule_cnt;
	u32				rule_locs[];
};

#endif /* CONFIG_COMPAT */

#include <linux/rculist.h>

/**
 * enum ethtool_phys_id_state - indicator state for physical identification
 * @ETHTOOL_ID_INACTIVE: Physical ID indicator should be deactivated
 * @ETHTOOL_ID_ACTIVE: Physical ID indicator should be activated
 * @ETHTOOL_ID_ON: LED should be turned on (used iff %ETHTOOL_ID_ACTIVE
 *	is not supported)
 * @ETHTOOL_ID_OFF: LED should be turned off (used iff %ETHTOOL_ID_ACTIVE
 *	is not supported)
 */
enum ethtool_phys_id_state {
	ETHTOOL_ID_INACTIVE,
	ETHTOOL_ID_ACTIVE,
	ETHTOOL_ID_ON,
	ETHTOOL_ID_OFF
};

enum {
	ETH_RSS_HASH_TOP_BIT, /* Configurable RSS hash function - Toeplitz */
	ETH_RSS_HASH_XOR_BIT, /* Configurable RSS hash function - Xor */
	ETH_RSS_HASH_CRC32_BIT, /* Configurable RSS hash function - Crc32 */

	/*
	 * Add your fresh new hash function bits above and remember to update
	 * rss_hash_func_strings[] in ethtool.c
	 */
	ETH_RSS_HASH_FUNCS_COUNT
};

#define __ETH_RSS_HASH_BIT(bit)	((u32)1 << (bit))
#define __ETH_RSS_HASH(name)	__ETH_RSS_HASH_BIT(ETH_RSS_HASH_##name##_BIT)

#define ETH_RSS_HASH_TOP	__ETH_RSS_HASH(TOP)
#define ETH_RSS_HASH_XOR	__ETH_RSS_HASH(XOR)
#define ETH_RSS_HASH_CRC32	__ETH_RSS_HASH(CRC32)

#define ETH_RSS_HASH_UNKNOWN	0
#define ETH_RSS_HASH_NO_CHANGE	0

struct net_device;
struct netlink_ext_ack;

/* Some generic methods drivers may use in their ethtool_ops */
u32 ethtool_op_get_link(struct net_device *dev);
int ethtool_op_get_ts_info(struct net_device *dev, struct ethtool_ts_info *eti);


/* Link extended state and substate. */
struct ethtool_link_ext_state_info {
	enum ethtool_link_ext_state link_ext_state;
	union {
		enum ethtool_link_ext_substate_autoneg autoneg;
		enum ethtool_link_ext_substate_link_training link_training;
		enum ethtool_link_ext_substate_link_logical_mismatch link_logical_mismatch;
		enum ethtool_link_ext_substate_bad_signal_integrity bad_signal_integrity;
		enum ethtool_link_ext_substate_cable_issue cable_issue;
		u8 __link_ext_substate;
	};
};

/**
 * ethtool_rxfh_indir_default - get default value for RX flow hash indirection
 * @index: Index in RX flow hash indirection table
 * @n_rx_rings: Number of RX rings to use
 *
 * This function provides the default policy for RX flow hash indirection.
 */
static inline u32 ethtool_rxfh_indir_default(u32 index, u32 n_rx_rings)
{
	return index % n_rx_rings;
}

/* declare a link mode bitmap */
#define __ETHTOOL_DECLARE_LINK_MODE_MASK(name)		\
	DECLARE_BITMAP(name, __ETHTOOL_LINK_MODE_MASK_NBITS)

/* drivers must ignore base.cmd and base.link_mode_masks_nwords
 * fields, but they are allowed to overwrite them (will be ignored).
 */
struct ethtool_link_ksettings {
	struct ethtool_link_settings base;
	struct {
		__ETHTOOL_DECLARE_LINK_MODE_MASK(supported);
		__ETHTOOL_DECLARE_LINK_MODE_MASK(advertising);
		__ETHTOOL_DECLARE_LINK_MODE_MASK(lp_advertising);
	} link_modes;
	u32	lanes;
};

/**
 * ethtool_link_ksettings_zero_link_mode - clear link_ksettings link mode mask
 *   @ptr : pointer to struct ethtool_link_ksettings
 *   @name : one of supported/advertising/lp_advertising
 */
#define ethtool_link_ksettings_zero_link_mode(ptr, name)		\
	bitmap_zero((ptr)->link_modes.name, __ETHTOOL_LINK_MODE_MASK_NBITS)

/**
 * ethtool_link_ksettings_add_link_mode - set bit in link_ksettings
 * link mode mask
 *   @ptr : pointer to struct ethtool_link_ksettings
 *   @name : one of supported/advertising/lp_advertising
 *   @mode : one of the ETHTOOL_LINK_MODE_*_BIT
 * (not atomic, no bound checking)
 */
#define ethtool_link_ksettings_add_link_mode(ptr, name, mode)		\
	__set_bit(ETHTOOL_LINK_MODE_ ## mode ## _BIT, (ptr)->link_modes.name)

/**
 * ethtool_link_ksettings_del_link_mode - clear bit in link_ksettings
 * link mode mask
 *   @ptr : pointer to struct ethtool_link_ksettings
 *   @name : one of supported/advertising/lp_advertising
 *   @mode : one of the ETHTOOL_LINK_MODE_*_BIT
 * (not atomic, no bound checking)
 */
#define ethtool_link_ksettings_del_link_mode(ptr, name, mode)		\
	__clear_bit(ETHTOOL_LINK_MODE_ ## mode ## _BIT, (ptr)->link_modes.name)

/**
 * ethtool_link_ksettings_test_link_mode - test bit in ksettings link mode mask
 *   @ptr : pointer to struct ethtool_link_ksettings
 *   @name : one of supported/advertising/lp_advertising
 *   @mode : one of the ETHTOOL_LINK_MODE_*_BIT
 * (not atomic, no bound checking)
 *
 * Returns true/false.
 */
#define ethtool_link_ksettings_test_link_mode(ptr, name, mode)		\
	test_bit(ETHTOOL_LINK_MODE_ ## mode ## _BIT, (ptr)->link_modes.name)

extern int
__ethtool_get_link_ksettings(struct net_device *dev,
			     struct ethtool_link_ksettings *link_ksettings);

/**
 * ethtool_intersect_link_masks - Given two link masks, AND them together
 * @dst: first mask and where result is stored
 * @src: second mask to intersect with
 *
 * Given two link mode masks, AND them together and save the result in dst.
 */
void ethtool_intersect_link_masks(struct ethtool_link_ksettings *dst,
				  struct ethtool_link_ksettings *src);

void ethtool_convert_legacy_u32_to_link_mode(unsigned long *dst,
					     u32 legacy_u32);

/* return false if src had higher bits set. lower bits always updated. */
bool ethtool_convert_link_mode_to_legacy_u32(u32 *legacy_u32,
				     const unsigned long *src);

#define ETHTOOL_COALESCE_RX_USECS		BIT(0)
#define ETHTOOL_COALESCE_RX_MAX_FRAMES		BIT(1)
#define ETHTOOL_COALESCE_RX_USECS_IRQ		BIT(2)
#define ETHTOOL_COALESCE_RX_MAX_FRAMES_IRQ	BIT(3)
#define ETHTOOL_COALESCE_TX_USECS		BIT(4)
#define ETHTOOL_COALESCE_TX_MAX_FRAMES		BIT(5)
#define ETHTOOL_COALESCE_TX_USECS_IRQ		BIT(6)
#define ETHTOOL_COALESCE_TX_MAX_FRAMES_IRQ	BIT(7)
#define ETHTOOL_COALESCE_STATS_BLOCK_USECS	BIT(8)
#define ETHTOOL_COALESCE_USE_ADAPTIVE_RX	BIT(9)
#define ETHTOOL_COALESCE_USE_ADAPTIVE_TX	BIT(10)
#define ETHTOOL_COALESCE_PKT_RATE_LOW		BIT(11)
#define ETHTOOL_COALESCE_RX_USECS_LOW		BIT(12)
#define ETHTOOL_COALESCE_RX_MAX_FRAMES_LOW	BIT(13)
#define ETHTOOL_COALESCE_TX_USECS_LOW		BIT(14)
#define ETHTOOL_COALESCE_TX_MAX_FRAMES_LOW	BIT(15)
#define ETHTOOL_COALESCE_PKT_RATE_HIGH		BIT(16)
#define ETHTOOL_COALESCE_RX_USECS_HIGH		BIT(17)
#define ETHTOOL_COALESCE_RX_MAX_FRAMES_HIGH	BIT(18)
#define ETHTOOL_COALESCE_TX_USECS_HIGH		BIT(19)
#define ETHTOOL_COALESCE_TX_MAX_FRAMES_HIGH	BIT(20)
#define ETHTOOL_COALESCE_RATE_SAMPLE_INTERVAL	BIT(21)
#define ETHTOOL_COALESCE_ALL_PARAMS		GENMASK(21, 0)

#define ETHTOOL_COALESCE_USECS						\
	(ETHTOOL_COALESCE_RX_USECS | ETHTOOL_COALESCE_TX_USECS)
#define ETHTOOL_COALESCE_MAX_FRAMES					\
	(ETHTOOL_COALESCE_RX_MAX_FRAMES | ETHTOOL_COALESCE_TX_MAX_FRAMES)
#define ETHTOOL_COALESCE_USECS_IRQ					\
	(ETHTOOL_COALESCE_RX_USECS_IRQ | ETHTOOL_COALESCE_TX_USECS_IRQ)
#define ETHTOOL_COALESCE_MAX_FRAMES_IRQ		\
	(ETHTOOL_COALESCE_RX_MAX_FRAMES_IRQ |	\
	 ETHTOOL_COALESCE_TX_MAX_FRAMES_IRQ)
#define ETHTOOL_COALESCE_USE_ADAPTIVE					\
	(ETHTOOL_COALESCE_USE_ADAPTIVE_RX | ETHTOOL_COALESCE_USE_ADAPTIVE_TX)
#define ETHTOOL_COALESCE_USECS_LOW_HIGH					\
	(ETHTOOL_COALESCE_RX_USECS_LOW | ETHTOOL_COALESCE_TX_USECS_LOW | \
	 ETHTOOL_COALESCE_RX_USECS_HIGH | ETHTOOL_COALESCE_TX_USECS_HIGH)
#define ETHTOOL_COALESCE_MAX_FRAMES_LOW_HIGH	\
	(ETHTOOL_COALESCE_RX_MAX_FRAMES_LOW |	\
	 ETHTOOL_COALESCE_TX_MAX_FRAMES_LOW |	\
	 ETHTOOL_COALESCE_RX_MAX_FRAMES_HIGH |	\
	 ETHTOOL_COALESCE_TX_MAX_FRAMES_HIGH)
#define ETHTOOL_COALESCE_PKT_RATE_RX_USECS				\
	(ETHTOOL_COALESCE_USE_ADAPTIVE_RX |				\
	 ETHTOOL_COALESCE_RX_USECS_LOW | ETHTOOL_COALESCE_RX_USECS_HIGH | \
	 ETHTOOL_COALESCE_PKT_RATE_LOW | ETHTOOL_COALESCE_PKT_RATE_HIGH | \
	 ETHTOOL_COALESCE_RATE_SAMPLE_INTERVAL)

#define ETHTOOL_STAT_NOT_SET	(~0ULL)

static inline void ethtool_stats_init(u64 *stats, unsigned int n)
{
	while (n--)
		stats[n] = ETHTOOL_STAT_NOT_SET;
}

/* Basic IEEE 802.3 MAC statistics (30.3.1.1.*), not otherwise exposed
 * via a more targeted API.
 */
struct ethtool_eth_mac_stats {
	u64 FramesTransmittedOK;
	u64 SingleCollisionFrames;
	u64 MultipleCollisionFrames;
	u64 FramesReceivedOK;
	u64 FrameCheckSequenceErrors;
	u64 AlignmentErrors;
	u64 OctetsTransmittedOK;
	u64 FramesWithDeferredXmissions;
	u64 LateCollisions;
	u64 FramesAbortedDueToXSColls;
	u64 FramesLostDueToIntMACXmitError;
	u64 CarrierSenseErrors;
	u64 OctetsReceivedOK;
	u64 FramesLostDueToIntMACRcvError;
	u64 MulticastFramesXmittedOK;
	u64 BroadcastFramesXmittedOK;
	u64 FramesWithExcessiveDeferral;
	u64 MulticastFramesReceivedOK;
	u64 BroadcastFramesReceivedOK;
	u64 InRangeLengthErrors;
	u64 OutOfRangeLengthField;
	u64 FrameTooLongErrors;
};

/* Basic IEEE 802.3 PHY statistics (30.3.2.1.*), not otherwise exposed
 * via a more targeted API.
 */
struct ethtool_eth_phy_stats {
	u64 SymbolErrorDuringCarrier;
};

/* Basic IEEE 802.3 MAC Ctrl statistics (30.3.3.*), not otherwise exposed
 * via a more targeted API.
 */
struct ethtool_eth_ctrl_stats {
	u64 MACControlFramesTransmitted;
	u64 MACControlFramesReceived;
	u64 UnsupportedOpcodesReceived;
};

/**
 * struct ethtool_pause_stats - statistics for IEEE 802.3x pause frames
 * @tx_pause_frames: transmitted pause frame count. Reported to user space
 *	as %ETHTOOL_A_PAUSE_STAT_TX_FRAMES.
 *
 *	Equivalent to `30.3.4.2 aPAUSEMACCtrlFramesTransmitted`
 *	from the standard.
 *
 * @rx_pause_frames: received pause frame count. Reported to user space
 *	as %ETHTOOL_A_PAUSE_STAT_RX_FRAMES. Equivalent to:
 *
 *	Equivalent to `30.3.4.3 aPAUSEMACCtrlFramesReceived`
 *	from the standard.
 */
struct ethtool_pause_stats {
	u64 tx_pause_frames;
	u64 rx_pause_frames;
};

#define ETHTOOL_MAX_LANES	8

/**
 * struct ethtool_fec_stats - statistics for IEEE 802.3 FEC
 * @corrected_blocks: number of received blocks corrected by FEC
 *	Reported to user space as %ETHTOOL_A_FEC_STAT_CORRECTED.
 *
 *	Equivalent to `30.5.1.1.17 aFECCorrectedBlocks` from the standard.
 *
 * @uncorrectable_blocks: number of received blocks FEC was not able to correct
 *	Reported to user space as %ETHTOOL_A_FEC_STAT_UNCORR.
 *
 *	Equivalent to `30.5.1.1.18 aFECUncorrectableBlocks` from the standard.
 *
 * @corrected_bits: number of bits corrected by FEC
 *	Similar to @corrected_blocks but counts individual bit changes,
 *	not entire FEC data blocks. This is a non-standard statistic.
 *	Reported to user space as %ETHTOOL_A_FEC_STAT_CORR_BITS.
 *
 * @lane: per-lane/PCS-instance counts as defined by the standard
 * @total: error counts for the entire port, for drivers incapable of reporting
 *	per-lane stats
 *
 * Drivers should fill in either only total or per-lane statistics, core
 * will take care of adding lane values up to produce the total.
 */
struct ethtool_fec_stats {
	struct ethtool_fec_stat {
		u64 total;
		u64 lanes[ETHTOOL_MAX_LANES];
	} corrected_blocks, uncorrectable_blocks, corrected_bits;
};

/**
 * struct ethtool_rmon_hist_range - byte range for histogram statistics
 * @low: low bound of the bucket (inclusive)
 * @high: high bound of the bucket (inclusive)
 */
struct ethtool_rmon_hist_range {
	u16 low;
	u16 high;
};

#define ETHTOOL_RMON_HIST_MAX	10

/**
 * struct ethtool_rmon_stats - selected RMON (RFC 2819) statistics
 * @undersize_pkts: Equivalent to `etherStatsUndersizePkts` from the RFC.
 * @oversize_pkts: Equivalent to `etherStatsOversizePkts` from the RFC.
 * @fragments: Equivalent to `etherStatsFragments` from the RFC.
 * @jabbers: Equivalent to `etherStatsJabbers` from the RFC.
 * @hist: Packet counter for packet length buckets (e.g.
 *	`etherStatsPkts128to255Octets` from the RFC).
 * @hist_tx: Tx counters in similar form to @hist, not defined in the RFC.
 *
 * Selection of RMON (RFC 2819) statistics which are not exposed via different
 * APIs, primarily the packet-length-based counters.
 * Unfortunately different designs choose different buckets beyond
 * the 1024B mark (jumbo frame teritory), so the definition of the bucket
 * ranges is left to the driver.
 */
struct ethtool_rmon_stats {
	u64 undersize_pkts;
	u64 oversize_pkts;
	u64 fragments;
	u64 jabbers;

	u64 hist[ETHTOOL_RMON_HIST_MAX];
	u64 hist_tx[ETHTOOL_RMON_HIST_MAX];
};

#define ETH_MODULE_EEPROM_PAGE_LEN	128
#define ETH_MODULE_MAX_I2C_ADDRESS	0x7f

/**
 * struct ethtool_module_eeprom - EEPROM dump from specified page
 * @offset: Offset within the specified EEPROM page to begin read, in bytes.
 * @length: Number of bytes to read.
 * @page: Page number to read from.
 * @bank: Page bank number to read from, if applicable by EEPROM spec.
 * @i2c_address: I2C address of a page. Value less than 0x7f expected. Most
 *	EEPROMs use 0x50 or 0x51.
 * @data: Pointer to buffer with EEPROM data of @length size.
 *
 * This can be used to manage pages during EEPROM dump in ethtool and pass
 * required information to the driver.
 */
struct ethtool_module_eeprom {
	__u32	offset;
	__u32	length;
	__u8	page;
	__u8	bank;
	__u8	i2c_address;
	__u8	*data;
};

/**
 * struct ethtool_ops - optional netdev operations
 * @cap_link_lanes_supported: indicates if the driver supports lanes
 *	parameter.
 * @supported_coalesce_params: supported types of interrupt coalescing.
 * @get_drvinfo: Report driver/device information.  Should only set the
 *	@driver, @version, @fw_version and @bus_info fields.  If not
 *	implemented, the @driver and @bus_info fields will be filled in
 *	according to the netdev's parent device.
 * @get_regs_len: Get buffer length required for @get_regs
 * @get_regs: Get device registers
 * @get_wol: Report whether Wake-on-Lan is enabled
 * @set_wol: Turn Wake-on-Lan on or off.  Returns a negative error code
 *	or zero.
 * @get_msglevel: Report driver message level.  This should be the value
 *	of the @msg_enable field used by netif logging functions.
 * @set_msglevel: Set driver message level
 * @nway_reset: Restart autonegotiation.  Returns a negative error code
 *	or zero.
 * @get_link: Report whether physical link is up.  Will only be called if
 *	the netdev is up.  Should usually be set to ethtool_op_get_link(),
 *	which uses netif_carrier_ok().
 * @get_link_ext_state: Report link extended state. Should set link_ext_state and
 *	link_ext_substate (link_ext_substate of 0 means link_ext_substate is unknown,
 *	do not attach ext_substate attribute to netlink message). If link_ext_state
 *	and link_ext_substate are unknown, return -ENODATA. If not implemented,
 *	link_ext_state and link_ext_substate will not be sent to userspace.
 * @get_eeprom_len: Read range of EEPROM addresses for validation of
 *	@get_eeprom and @set_eeprom requests.
 *	Returns 0 if device does not support EEPROM access.
 * @get_eeprom: Read data from the device EEPROM.
 *	Should fill in the magic field.  Don't need to check len for zero
 *	or wraparound.  Fill in the data argument with the eeprom values
 *	from offset to offset + len.  Update len to the amount read.
 *	Returns an error or zero.
 * @set_eeprom: Write data to the device EEPROM.
 *	Should validate the magic field.  Don't need to check len for zero
 *	or wraparound.  Update len to the amount written.  Returns an error
 *	or zero.
 * @get_coalesce: Get interrupt coalescing parameters.  Returns a negative
 *	error code or zero.
 * @set_coalesce: Set interrupt coalescing parameters.  Supported coalescing
 *	types should be set in @supported_coalesce_params.
 *	Returns a negative error code or zero.
 * @get_ringparam: Report ring sizes
 * @set_ringparam: Set ring sizes.  Returns a negative error code or zero.
 * @get_pause_stats: Report pause frame statistics. Drivers must not zero
 *	statistics which they don't report. The stats structure is initialized
 *	to ETHTOOL_STAT_NOT_SET indicating driver does not report statistics.
 * @get_pauseparam: Report pause parameters
 * @set_pauseparam: Set pause parameters.  Returns a negative error code
 *	or zero.
 * @self_test: Run specified self-tests
 * @get_strings: Return a set of strings that describe the requested objects
 * @set_phys_id: Identify the physical devices, e.g. by flashing an LED
 *	attached to it.  The implementation may update the indicator
 *	asynchronously or synchronously, but in either case it must return
 *	quickly.  It is initially called with the argument %ETHTOOL_ID_ACTIVE,
 *	and must either activate asynchronous updates and return zero, return
 *	a negative error or return a positive frequency for synchronous
 *	indication (e.g. 1 for one on/off cycle per second).  If it returns
 *	a frequency then it will be called again at intervals with the
 *	argument %ETHTOOL_ID_ON or %ETHTOOL_ID_OFF and should set the state of
 *	the indicator accordingly.  Finally, it is called with the argument
 *	%ETHTOOL_ID_INACTIVE and must deactivate the indicator.  Returns a
 *	negative error code or zero.
 * @get_ethtool_stats: Return extended statistics about the device.
 *	This is only useful if the device maintains statistics not
 *	included in &struct rtnl_link_stats64.
 * @begin: Function to be called before any other operation.  Returns a
 *	negative error code or zero.
 * @complete: Function to be called after any other operation except
 *	@begin.  Will be called even if the other operation failed.
 * @get_priv_flags: Report driver-specific feature flags.
 * @set_priv_flags: Set driver-specific feature flags.  Returns a negative
 *	error code or zero.
 * @get_sset_count: Get number of strings that @get_strings will write.
 * @get_rxnfc: Get RX flow classification rules.  Returns a negative
 *	error code or zero.
 * @set_rxnfc: Set RX flow classification rules.  Returns a negative
 *	error code or zero.
 * @flash_device: Write a firmware image to device's flash memory.
 *	Returns a negative error code or zero.
 * @reset: Reset (part of) the device, as specified by a bitmask of
 *	flags from &enum ethtool_reset_flags.  Returns a negative
 *	error code or zero.
 * @get_rxfh_key_size: Get the size of the RX flow hash key.
 *	Returns zero if not supported for this specific device.
 * @get_rxfh_indir_size: Get the size of the RX flow hash indirection table.
 *	Returns zero if not supported for this specific device.
 * @get_rxfh: Get the contents of the RX flow hash indirection table, hash key
 *	and/or hash function.
 *	Returns a negative error code or zero.
 * @set_rxfh: Set the contents of the RX flow hash indirection table, hash
 *	key, and/or hash function.  Arguments which are set to %NULL or zero
 *	will remain unchanged.
 *	Returns a negative error code or zero. An error code must be returned
 *	if at least one unsupported change was requested.
 * @get_rxfh_context: Get the contents of the RX flow hash indirection table,
 *	hash key, and/or hash function assiciated to the given rss context.
 *	Returns a negative error code or zero.
 * @set_rxfh_context: Create, remove and configure RSS contexts. Allows setting
 *	the contents of the RX flow hash indirection table, hash key, and/or
 *	hash function associated to the given context. Arguments which are set
 *	to %NULL or zero will remain unchanged.
 *	Returns a negative error code or zero. An error code must be returned
 *	if at least one unsupported change was requested.
 * @get_channels: Get number of channels.
 * @set_channels: Set number of channels.  Returns a negative error code or
 *	zero.
 * @get_dump_flag: Get dump flag indicating current dump length, version,
 * 		   and flag of the device.
 * @get_dump_data: Get dump data.
 * @set_dump: Set dump specific flags to the device.
 * @get_ts_info: Get the time stamping and PTP hardware clock capabilities.
 *	Drivers supporting transmit time stamps in software should set this to
 *	ethtool_op_get_ts_info().
 * @get_module_info: Get the size and type of the eeprom contained within
 *	a plug-in module.
 * @get_module_eeprom: Get the eeprom information from the plug-in module
 * @get_eee: Get Energy-Efficient (EEE) supported and status.
 * @set_eee: Set EEE status (enable/disable) as well as LPI timers.
 * @get_tunable: Read the value of a driver / device tunable.
 * @set_tunable: Set the value of a driver / device tunable.
 * @get_per_queue_coalesce: Get interrupt coalescing parameters per queue.
 *	It must check that the given queue number is valid. If neither a RX nor
 *	a TX queue has this number, return -EINVAL. If only a RX queue or a TX
 *	queue has this number, set the inapplicable fields to ~0 and return 0.
 *	Returns a negative error code or zero.
 * @set_per_queue_coalesce: Set interrupt coalescing parameters per queue.
 *	It must check that the given queue number is valid. If neither a RX nor
 *	a TX queue has this number, return -EINVAL. If only a RX queue or a TX
 *	queue has this number, ignore the inapplicable fields. Supported
 *	coalescing types should be set in @supported_coalesce_params.
 *	Returns a negative error code or zero.
 * @get_link_ksettings: Get various device settings including Ethernet link
 *	settings. The %cmd and %link_mode_masks_nwords fields should be
 *	ignored (use %__ETHTOOL_LINK_MODE_MASK_NBITS instead of the latter),
 *	any change to them will be overwritten by kernel. Returns a negative
 *	error code or zero.
 * @set_link_ksettings: Set various device settings including Ethernet link
 *	settings. The %cmd and %link_mode_masks_nwords fields should be
 *	ignored (use %__ETHTOOL_LINK_MODE_MASK_NBITS instead of the latter),
 *	any change to them will be overwritten by kernel. Returns a negative
 *	error code or zero.
 * @get_fec_stats: Report FEC statistics.
 *	Core will sum up per-lane stats to get the total.
 *	Drivers must not zero statistics which they don't report. The stats
 *	structure is initialized to ETHTOOL_STAT_NOT_SET indicating driver does
 *	not report statistics.
 * @get_fecparam: Get the network device Forward Error Correction parameters.
 * @set_fecparam: Set the network device Forward Error Correction parameters.
 * @get_ethtool_phy_stats: Return extended statistics about the PHY device.
 *	This is only useful if the device maintains PHY statistics and
 *	cannot use the standard PHY library helpers.
 * @get_phy_tunable: Read the value of a PHY tunable.
 * @set_phy_tunable: Set the value of a PHY tunable.
 * @get_module_eeprom_by_page: Get a region of plug-in module EEPROM data from
 *	specified page. Returns a negative error code or the amount of bytes
 *	read.
 * @get_eth_phy_stats: Query some of the IEEE 802.3 PHY statistics.
 * @get_eth_mac_stats: Query some of the IEEE 802.3 MAC statistics.
 * @get_eth_ctrl_stats: Query some of the IEEE 802.3 MAC Ctrl statistics.
 * @get_rmon_stats: Query some of the RMON (RFC 2819) statistics.
 *	Set %ranges to a pointer to zero-terminated array of byte ranges.
 *
 * All operations are optional (i.e. the function pointer may be set
 * to %NULL) and callers must take this into account.  Callers must
 * hold the RTNL lock.
 *
 * See the structures used by these operations for further documentation.
 * Note that for all operations using a structure ending with a zero-
 * length array, the array is allocated separately in the kernel and
 * is passed to the driver as an additional parameter.
 *
 * See &struct net_device and &struct net_device_ops for documentation
 * of the generic netdev features interface.
 */
struct ethtool_ops {
	u32     cap_link_lanes_supported:1;
	u32	supported_coalesce_params;
	void	(*get_drvinfo)(struct net_device *, struct ethtool_drvinfo *);
	int	(*get_regs_len)(struct net_device *);
	void	(*get_regs)(struct net_device *, struct ethtool_regs *, void *);
	void	(*get_wol)(struct net_device *, struct ethtool_wolinfo *);
	int	(*set_wol)(struct net_device *, struct ethtool_wolinfo *);
	u32	(*get_msglevel)(struct net_device *);
	void	(*set_msglevel)(struct net_device *, u32);
	int	(*nway_reset)(struct net_device *);
	u32	(*get_link)(struct net_device *);
	int	(*get_link_ext_state)(struct net_device *,
				      struct ethtool_link_ext_state_info *);
	int	(*get_eeprom_len)(struct net_device *);
	int	(*get_eeprom)(struct net_device *,
			      struct ethtool_eeprom *, u8 *);
	int	(*set_eeprom)(struct net_device *,
			      struct ethtool_eeprom *, u8 *);
	int	(*get_coalesce)(struct net_device *, struct ethtool_coalesce *);
	int	(*set_coalesce)(struct net_device *, struct ethtool_coalesce *);
	void	(*get_ringparam)(struct net_device *,
				 struct ethtool_ringparam *);
	int	(*set_ringparam)(struct net_device *,
				 struct ethtool_ringparam *);
	void	(*get_pause_stats)(struct net_device *dev,
				   struct ethtool_pause_stats *pause_stats);
	void	(*get_pauseparam)(struct net_device *,
				  struct ethtool_pauseparam*);
	int	(*set_pauseparam)(struct net_device *,
				  struct ethtool_pauseparam*);
	void	(*self_test)(struct net_device *, struct ethtool_test *, u64 *);
	void	(*get_strings)(struct net_device *, u32 stringset, u8 *);
	int	(*set_phys_id)(struct net_device *, enum ethtool_phys_id_state);
	void	(*get_ethtool_stats)(struct net_device *,
				     struct ethtool_stats *, u64 *);
	int	(*begin)(struct net_device *);
	void	(*complete)(struct net_device *);
	u32	(*get_priv_flags)(struct net_device *);
	int	(*set_priv_flags)(struct net_device *, u32);
	int	(*get_sset_count)(struct net_device *, int);
	int	(*get_rxnfc)(struct net_device *,
			     struct ethtool_rxnfc *, u32 *rule_locs);
	int	(*set_rxnfc)(struct net_device *, struct ethtool_rxnfc *);
	int	(*flash_device)(struct net_device *, struct ethtool_flash *);
	int	(*reset)(struct net_device *, u32 *);
	u32	(*get_rxfh_key_size)(struct net_device *);
	u32	(*get_rxfh_indir_size)(struct net_device *);
	int	(*get_rxfh)(struct net_device *, u32 *indir, u8 *key,
			    u8 *hfunc);
	int	(*set_rxfh)(struct net_device *, const u32 *indir,
			    const u8 *key, const u8 hfunc);
	int	(*get_rxfh_context)(struct net_device *, u32 *indir, u8 *key,
				    u8 *hfunc, u32 rss_context);
	int	(*set_rxfh_context)(struct net_device *, const u32 *indir,
				    const u8 *key, const u8 hfunc,
				    u32 *rss_context, bool delete);
	void	(*get_channels)(struct net_device *, struct ethtool_channels *);
	int	(*set_channels)(struct net_device *, struct ethtool_channels *);
	int	(*get_dump_flag)(struct net_device *, struct ethtool_dump *);
	int	(*get_dump_data)(struct net_device *,
				 struct ethtool_dump *, void *);
	int	(*set_dump)(struct net_device *, struct ethtool_dump *);
	int	(*get_ts_info)(struct net_device *, struct ethtool_ts_info *);
	int     (*get_module_info)(struct net_device *,
				   struct ethtool_modinfo *);
	int     (*get_module_eeprom)(struct net_device *,
				     struct ethtool_eeprom *, u8 *);
	int	(*get_eee)(struct net_device *, struct ethtool_eee *);
	int	(*set_eee)(struct net_device *, struct ethtool_eee *);
	int	(*get_tunable)(struct net_device *,
			       const struct ethtool_tunable *, void *);
	int	(*set_tunable)(struct net_device *,
			       const struct ethtool_tunable *, const void *);
	int	(*get_per_queue_coalesce)(struct net_device *, u32,
					  struct ethtool_coalesce *);
	int	(*set_per_queue_coalesce)(struct net_device *, u32,
					  struct ethtool_coalesce *);
	int	(*get_link_ksettings)(struct net_device *,
				      struct ethtool_link_ksettings *);
	int	(*set_link_ksettings)(struct net_device *,
				      const struct ethtool_link_ksettings *);
	void	(*get_fec_stats)(struct net_device *dev,
				 struct ethtool_fec_stats *fec_stats);
	int	(*get_fecparam)(struct net_device *,
				      struct ethtool_fecparam *);
	int	(*set_fecparam)(struct net_device *,
				      struct ethtool_fecparam *);
	void	(*get_ethtool_phy_stats)(struct net_device *,
					 struct ethtool_stats *, u64 *);
	int	(*get_phy_tunable)(struct net_device *,
				   const struct ethtool_tunable *, void *);
	int	(*set_phy_tunable)(struct net_device *,
				   const struct ethtool_tunable *, const void *);
	int	(*get_module_eeprom_by_page)(struct net_device *dev,
					     const struct ethtool_module_eeprom *page,
					     struct netlink_ext_ack *extack);
	void	(*get_eth_phy_stats)(struct net_device *dev,
				     struct ethtool_eth_phy_stats *phy_stats);
	void	(*get_eth_mac_stats)(struct net_device *dev,
				     struct ethtool_eth_mac_stats *mac_stats);
	void	(*get_eth_ctrl_stats)(struct net_device *dev,
				      struct ethtool_eth_ctrl_stats *ctrl_stats);
	void	(*get_rmon_stats)(struct net_device *dev,
				  struct ethtool_rmon_stats *rmon_stats,
				  const struct ethtool_rmon_hist_range **ranges);
};

int ethtool_check_ops(const struct ethtool_ops *ops);

struct ethtool_rx_flow_rule {
	struct flow_rule	*rule;
	unsigned long		priv[];
};

struct ethtool_rx_flow_spec_input {
	const struct ethtool_rx_flow_spec	*fs;
	u32					rss_ctx;
};

struct ethtool_rx_flow_rule *
ethtool_rx_flow_rule_create(const struct ethtool_rx_flow_spec_input *input);
void ethtool_rx_flow_rule_destroy(struct ethtool_rx_flow_rule *rule);

bool ethtool_virtdev_validate_cmd(const struct ethtool_link_ksettings *cmd);
int ethtool_virtdev_set_link_ksettings(struct net_device *dev,
				       const struct ethtool_link_ksettings *cmd,
				       u32 *dev_speed, u8 *dev_duplex);

struct phy_device;
struct phy_tdr_config;

/**
 * struct ethtool_phy_ops - Optional PHY device options
 * @get_sset_count: Get number of strings that @get_strings will write.
 * @get_strings: Return a set of strings that describe the requested objects
 * @get_stats: Return extended statistics about the PHY device.
 * @start_cable_test: Start a cable test
 * @start_cable_test_tdr: Start a Time Domain Reflectometry cable test
 *
 * All operations are optional (i.e. the function pointer may be set to %NULL)
 * and callers must take this into account. Callers must hold the RTNL lock.
 */
struct ethtool_phy_ops {
	int (*get_sset_count)(struct phy_device *dev);
	int (*get_strings)(struct phy_device *dev, u8 *data);
	int (*get_stats)(struct phy_device *dev,
			 struct ethtool_stats *stats, u64 *data);
	int (*start_cable_test)(struct phy_device *phydev,
				struct netlink_ext_ack *extack);
	int (*start_cable_test_tdr)(struct phy_device *phydev,
				    struct netlink_ext_ack *extack,
				    const struct phy_tdr_config *config);
};

/**
 * ethtool_set_ethtool_phy_ops - Set the ethtool_phy_ops singleton
 * @ops: Ethtool PHY operations to set
 */
void ethtool_set_ethtool_phy_ops(const struct ethtool_phy_ops *ops);

/**
 * ethtool_params_from_link_mode - Derive link parameters from a given link mode
 * @link_ksettings: Link parameters to be derived from the link mode
 * @link_mode: Link mode
 */
void
ethtool_params_from_link_mode(struct ethtool_link_ksettings *link_ksettings,
			      enum ethtool_link_mode_bit_indices link_mode);

/**
 * ethtool_sprintf - Write formatted string to ethtool string data
 * @data: Pointer to start of string to update
 * @fmt: Format of string to write
 *
 * Write formatted string to data. Update data to point at start of
 * next string.
 */
extern __printf(2, 3) void ethtool_sprintf(u8 **data, const char *fmt, ...);
#endif /* _LINUX_ETHTOOL_H */
