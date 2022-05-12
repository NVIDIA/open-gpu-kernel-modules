/* SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause */
/*
 * Copyright (C) 2012-2014, 2018-2020 Intel Corporation
 * Copyright (C) 2013-2014 Intel Mobile Communications GmbH
 * Copyright (C) 2015-2016 Intel Deutschland GmbH
 */
#ifndef __sta_h__
#define __sta_h__

#include <linux/spinlock.h>
#include <net/mac80211.h>
#include <linux/wait.h>

#include "iwl-trans.h" /* for IWL_MAX_TID_COUNT */
#include "fw-api.h" /* IWL_MVM_STATION_COUNT_MAX */
#include "rs.h"

struct iwl_mvm;
struct iwl_mvm_vif;

/**
 * DOC: DQA - Dynamic Queue Allocation -introduction
 *
 * Dynamic Queue Allocation (AKA "DQA") is a feature implemented in iwlwifi
 * driver to allow dynamic allocation of queues on-demand, rather than allocate
 * them statically ahead of time. Ideally, we would like to allocate one queue
 * per RA/TID, thus allowing an AP - for example - to send BE traffic to STA2
 * even if it also needs to send traffic to a sleeping STA1, without being
 * blocked by the sleeping station.
 *
 * Although the queues in DQA mode are dynamically allocated, there are still
 * some queues that are statically allocated:
 *	TXQ #0 - command queue
 *	TXQ #1 - aux frames
 *	TXQ #2 - P2P device frames
 *	TXQ #3 - P2P GO/SoftAP GCAST/BCAST frames
 *	TXQ #4 - BSS DATA frames queue
 *	TXQ #5-8 - Non-QoS and MGMT frames queue pool
 *	TXQ #9 - P2P GO/SoftAP probe responses
 *	TXQ #10-31 - DATA frames queue pool
 * The queues are dynamically taken from either the MGMT frames queue pool or
 * the DATA frames one. See the %iwl_mvm_dqa_txq for more information on every
 * queue.
 *
 * When a frame for a previously unseen RA/TID comes in, it needs to be deferred
 * until a queue is allocated for it, and only then can be TXed. Therefore, it
 * is placed into %iwl_mvm_tid_data.deferred_tx_frames, and a worker called
 * %mvm->add_stream_wk later allocates the queues and TXes the deferred frames.
 *
 * For convenience, MGMT is considered as if it has TID=8, and go to the MGMT
 * queues in the pool. If there is no longer a free MGMT queue to allocate, a
 * queue will be allocated from the DATA pool instead. Since QoS NDPs can create
 * a problem for aggregations, they too will use a MGMT queue.
 *
 * When adding a STA, a DATA queue is reserved for it so that it can TX from
 * it. If no such free queue exists for reserving, the STA addition will fail.
 *
 * If the DATA queue pool gets exhausted, no new STA will be accepted, and if a
 * new RA/TID comes in for an existing STA, one of the STA's queues will become
 * shared and will serve more than the single TID (but always for the same RA!).
 *
 * When a RA/TID needs to become aggregated, no new queue is required to be
 * allocated, only mark the queue as aggregated via the ADD_STA command. Note,
 * however, that a shared queue cannot be aggregated, and only after the other
 * TIDs become inactive and are removed - only then can the queue be
 * reconfigured and become aggregated.
 *
 * When removing a station, its queues are returned to the pool for reuse. Here
 * we also need to make sure that we are synced with the worker thread that TXes
 * the deferred frames so we don't get into a situation where the queues are
 * removed and then the worker puts deferred frames onto the released queues or
 * tries to allocate new queues for a STA we don't need anymore.
 */

/**
 * DOC: station table - introduction
 *
 * The station table is a list of data structure that reprensent the stations.
 * In STA/P2P client mode, the driver will hold one station for the AP/ GO.
 * In GO/AP mode, the driver will have as many stations as associated clients.
 * All these stations are reflected in the fw's station table. The driver
 * keeps the fw's station table up to date with the ADD_STA command. Stations
 * can be removed by the REMOVE_STA command.
 *
 * All the data related to a station is held in the structure %iwl_mvm_sta
 * which is embed in the mac80211's %ieee80211_sta (in the drv_priv) area.
 * This data includes the index of the station in the fw, per tid information
 * (sequence numbers, Block-ack state machine, etc...). The stations are
 * created and deleted by the %sta_state callback from %ieee80211_ops.
 *
 * The driver holds a map: %fw_id_to_mac_id that allows to fetch a
 * %ieee80211_sta (and the %iwl_mvm_sta embedded into it) based on a fw
 * station index. That way, the driver is able to get the tid related data in
 * O(1) in time sensitive paths (Tx / Tx response / BA notification). These
 * paths are triggered by the fw, and the driver needs to get a pointer to the
 * %ieee80211 structure. This map helps to get that pointer quickly.
 */

/**
 * DOC: station table - locking
 *
 * As stated before, the station is created / deleted by mac80211's %sta_state
 * callback from %ieee80211_ops which can sleep. The next paragraph explains
 * the locking of a single stations, the next ones relates to the station
 * table.
 *
 * The station holds the sequence number per tid. So this data needs to be
 * accessed in the Tx path (which is softIRQ). It also holds the Block-Ack
 * information (the state machine / and the logic that checks if the queues
 * were drained), so it also needs to be accessible from the Tx response flow.
 * In short, the station needs to be access from sleepable context as well as
 * from tasklets, so the station itself needs a spinlock.
 *
 * The writers of %fw_id_to_mac_id map are serialized by the global mutex of
 * the mvm op_mode. This is possible since %sta_state can sleep.
 * The pointers in this map are RCU protected, hence we won't replace the
 * station while we have Tx / Tx response / BA notification running.
 *
 * If a station is deleted while it still has packets in its A-MPDU queues,
 * then the reclaim flow will notice that there is no station in the map for
 * sta_id and it will dump the responses.
 */

/**
 * DOC: station table - internal stations
 *
 * The FW needs a few internal stations that are not reflected in
 * mac80211, such as broadcast station in AP / GO mode, or AUX sta for
 * scanning and P2P device (during the GO negotiation).
 * For these kind of stations we have %iwl_mvm_int_sta struct which holds the
 * data relevant for them from both %iwl_mvm_sta and %ieee80211_sta.
 * Usually the data for these stations is static, so no locking is required,
 * and no TID data as this is also not needed.
 * One thing to note, is that these stations have an ID in the fw, but not
 * in mac80211. In order to "reserve" them a sta_id in %fw_id_to_mac_id
 * we fill ERR_PTR(EINVAL) in this mapping and all other dereferencing of
 * pointers from this mapping need to check that the value is not error
 * or NULL.
 *
 * Currently there is only one auxiliary station for scanning, initialized
 * on init.
 */

/**
 * DOC: station table - AP Station in STA mode
 *
 * %iwl_mvm_vif includes the index of the AP station in the fw's STA table:
 * %ap_sta_id. To get the point to the corresponding %ieee80211_sta,
 * &fw_id_to_mac_id can be used. Due to the way the fw works, we must not remove
 * the AP station from the fw before setting the MAC context as unassociated.
 * Hence, %fw_id_to_mac_id[%ap_sta_id] will be NULLed when the AP station is
 * removed by mac80211, but the station won't be removed in the fw until the
 * VIF is set as unassociated. Then, %ap_sta_id will be invalidated.
 */

/**
 * DOC: station table - Drain vs. Flush
 *
 * Flush means that all the frames in the SCD queue are dumped regardless the
 * station to which they were sent. We do that when we disassociate and before
 * we remove the STA of the AP. The flush can be done synchronously against the
 * fw.
 * Drain means that the fw will drop all the frames sent to a specific station.
 * This is useful when a client (if we are IBSS / GO or AP) disassociates.
 */

/**
 * DOC: station table - fw restart
 *
 * When the fw asserts, or we have any other issue that requires to reset the
 * driver, we require mac80211 to reconfigure the driver. Since the private
 * data of the stations is embed in mac80211's %ieee80211_sta, that data will
 * not be zeroed and needs to be reinitialized manually.
 * %IWL_MVM_STATUS_IN_HW_RESTART is set during restart and that will hint us
 * that we must not allocate a new sta_id but reuse the previous one. This
 * means that the stations being re-added after the reset will have the same
 * place in the fw as before the reset. We do need to zero the %fw_id_to_mac_id
 * map, since the stations aren't in the fw any more. Internal stations that
 * are not added by mac80211 will be re-added in the init flow that is called
 * after the restart: mac80211 call's %iwl_mvm_mac_start which calls to
 * %iwl_mvm_up.
 */

/**
 * DOC: AP mode - PS
 *
 * When a station is asleep, the fw will set it as "asleep". All frames on
 * shared queues (i.e. non-aggregation queues) to that station will be dropped
 * by the fw (%TX_STATUS_FAIL_DEST_PS failure code).
 *
 * AMPDUs are in a separate queue that is stopped by the fw. We just need to
 * let mac80211 know when there are frames in these queues so that it can
 * properly handle trigger frames.
 *
 * When a trigger frame is received, mac80211 tells the driver to send frames
 * from the AMPDU queues or sends frames to non-aggregation queues itself,
 * depending on which ACs are delivery-enabled and what TID has frames to
 * transmit. Note that mac80211 has all the knowledge since all the non-agg
 * frames are buffered / filtered, and the driver tells mac80211 about agg
 * frames). The driver needs to tell the fw to let frames out even if the
 * station is asleep. This is done by %iwl_mvm_sta_modify_sleep_tx_count.
 *
 * When we receive a frame from that station with PM bit unset, the driver
 * needs to let the fw know that this station isn't asleep any more. This is
 * done by %iwl_mvm_sta_modify_ps_wake in response to mac80211 signaling the
 * station's wakeup.
 *
 * For a GO, the Service Period might be cut short due to an absence period
 * of the GO. In this (and all other cases) the firmware notifies us with the
 * EOSP_NOTIFICATION, and we notify mac80211 of that. Further frames that we
 * already sent to the device will be rejected again.
 *
 * See also "AP support for powersaving clients" in mac80211.h.
 */

/**
 * enum iwl_mvm_agg_state
 *
 * The state machine of the BA agreement establishment / tear down.
 * These states relate to a specific RA / TID.
 *
 * @IWL_AGG_OFF: aggregation is not used
 * @IWL_AGG_QUEUED: aggregation start work has been queued
 * @IWL_AGG_STARTING: aggregation are starting (between start and oper)
 * @IWL_AGG_ON: aggregation session is up
 * @IWL_EMPTYING_HW_QUEUE_ADDBA: establishing a BA session - waiting for the
 *	HW queue to be empty from packets for this RA /TID.
 * @IWL_EMPTYING_HW_QUEUE_DELBA: tearing down a BA session - waiting for the
 *	HW queue to be empty from packets for this RA /TID.
 */
enum iwl_mvm_agg_state {
	IWL_AGG_OFF = 0,
	IWL_AGG_QUEUED,
	IWL_AGG_STARTING,
	IWL_AGG_ON,
	IWL_EMPTYING_HW_QUEUE_ADDBA,
	IWL_EMPTYING_HW_QUEUE_DELBA,
};

/**
 * struct iwl_mvm_tid_data - holds the states for each RA / TID
 * @seq_number: the next WiFi sequence number to use
 * @next_reclaimed: the WiFi sequence number of the next packet to be acked.
 *	This is basically (last acked packet++).
 * @rate_n_flags: Rate at which Tx was attempted. Holds the data between the
 *	Tx response (TX_CMD), and the block ack notification (COMPRESSED_BA).
 * @lq_color: the color of the LQ command as it appears in tx response.
 * @amsdu_in_ampdu_allowed: true if A-MSDU in A-MPDU is allowed.
 * @state: state of the BA agreement establishment / tear down.
 * @txq_id: Tx queue used by the BA session / DQA
 * @ssn: the first packet to be sent in AGG HW queue in Tx AGG start flow, or
 *	the first packet to be sent in legacy HW queue in Tx AGG stop flow.
 *	Basically when next_reclaimed reaches ssn, we can tell mac80211 that
 *	we are ready to finish the Tx AGG stop / start flow.
 * @tx_time: medium time consumed by this A-MPDU
 * @tpt_meas_start: time of the throughput measurements start, is reset every HZ
 * @tx_count_last: number of frames transmitted during the last second
 * @tx_count: counts the number of frames transmitted since the last reset of
 *	 tpt_meas_start
 */
struct iwl_mvm_tid_data {
	u16 seq_number;
	u16 next_reclaimed;
	/* The rest is Tx AGG related */
	u32 rate_n_flags;
	u8 lq_color;
	bool amsdu_in_ampdu_allowed;
	enum iwl_mvm_agg_state state;
	u16 txq_id;
	u16 ssn;
	u16 tx_time;
	unsigned long tpt_meas_start;
	u32 tx_count_last;
	u32 tx_count;
};

struct iwl_mvm_key_pn {
	struct rcu_head rcu_head;
	struct {
		u8 pn[IWL_MAX_TID_COUNT][IEEE80211_CCMP_PN_LEN];
	} ____cacheline_aligned_in_smp q[];
};

/**
 * enum iwl_mvm_rxq_notif_type - Internal message identifier
 *
 * @IWL_MVM_RXQ_EMPTY: empty sync notification
 * @IWL_MVM_RXQ_NOTIF_DEL_BA: notify RSS queues of delBA
 * @IWL_MVM_RXQ_NSSN_SYNC: notify all the RSS queues with the new NSSN
 */
enum iwl_mvm_rxq_notif_type {
	IWL_MVM_RXQ_EMPTY,
	IWL_MVM_RXQ_NOTIF_DEL_BA,
	IWL_MVM_RXQ_NSSN_SYNC,
};

/**
 * struct iwl_mvm_internal_rxq_notif - Internal representation of the data sent
 * in &iwl_rxq_sync_cmd. Should be DWORD aligned.
 * FW is agnostic to the payload, so there are no endianity requirements.
 *
 * @type: value from &iwl_mvm_rxq_notif_type
 * @sync: ctrl path is waiting for all notifications to be received
 * @cookie: internal cookie to identify old notifications
 * @data: payload
 */
struct iwl_mvm_internal_rxq_notif {
	u16 type;
	u16 sync;
	u32 cookie;
	u8 data[];
} __packed;

struct iwl_mvm_delba_data {
	u32 baid;
} __packed;

struct iwl_mvm_nssn_sync_data {
	u32 baid;
	u32 nssn;
} __packed;

/**
 * struct iwl_mvm_rxq_dup_data - per station per rx queue data
 * @last_seq: last sequence per tid for duplicate packet detection
 * @last_sub_frame: last subframe packet
 */
struct iwl_mvm_rxq_dup_data {
	__le16 last_seq[IWL_MAX_TID_COUNT + 1];
	u8 last_sub_frame[IWL_MAX_TID_COUNT + 1];
} ____cacheline_aligned_in_smp;

/**
 * struct iwl_mvm_sta - representation of a station in the driver
 * @sta_id: the index of the station in the fw (will be replaced by id_n_color)
 * @tfd_queue_msk: the tfd queues used by the station
 * @mac_id_n_color: the MAC context this station is linked to
 * @tid_disable_agg: bitmap: if bit(tid) is set, the fw won't send ampdus for
 *	tid.
 * @max_agg_bufsize: the maximal size of the AGG buffer for this station
 * @sta_type: station type
 * @sta_state: station state according to enum %ieee80211_sta_state
 * @bt_reduced_txpower: is reduced tx power enabled for this station
 * @next_status_eosp: the next reclaimed packet is a PS-Poll response and
 *	we need to signal the EOSP
 * @lock: lock to protect the whole struct. Since %tid_data is access from Tx
 * and from Tx response flow, it needs a spinlock.
 * @tid_data: per tid data + mgmt. Look at %iwl_mvm_tid_data.
 * @tid_to_baid: a simple map of TID to baid
 * @lq_sta: holds rate scaling data, either for the case when RS is done in
 *	the driver - %rs_drv or in the FW - %rs_fw.
 * @reserved_queue: the queue reserved for this STA for DQA purposes
 *	Every STA has is given one reserved queue to allow it to operate. If no
 *	such queue can be guaranteed, the STA addition will fail.
 * @tx_protection: reference counter for controlling the Tx protection.
 * @tt_tx_protection: is thermal throttling enable Tx protection?
 * @disable_tx: is tx to this STA disabled?
 * @amsdu_enabled: bitmap of TX AMSDU allowed TIDs.
 *	In case TLC offload is not active it is either 0xFFFF or 0.
 * @max_amsdu_len: max AMSDU length
 * @orig_amsdu_len: used to save the original amsdu_len when it is changed via
 *      debugfs.  If it's set to 0, it means that it is it's not set via
 *      debugfs.
 * @agg_tids: bitmap of tids whose status is operational aggregated (IWL_AGG_ON)
 * @sleep_tx_count: the number of frames that we told the firmware to let out
 *	even when that station is asleep. This is useful in case the queue
 *	gets empty before all the frames were sent, which can happen when
 *	we are sending frames from an AMPDU queue and there was a hole in
 *	the BA window. To be used for UAPSD only.
 * @ptk_pn: per-queue PTK PN data structures
 * @dup_data: per queue duplicate packet detection data
 * @deferred_traffic_tid_map: indication bitmap of deferred traffic per-TID
 * @tx_ant: the index of the antenna to use for data tx to this station. Only
 *	used during connection establishment (e.g. for the 4 way handshake
 *	exchange).
 *
 * When mac80211 creates a station it reserves some space (hw->sta_data_size)
 * in the structure for use by driver. This structure is placed in that
 * space.
 *
 */
struct iwl_mvm_sta {
	u32 sta_id;
	u32 tfd_queue_msk;
	u32 mac_id_n_color;
	u16 tid_disable_agg;
	u16 max_agg_bufsize;
	enum iwl_sta_type sta_type;
	enum ieee80211_sta_state sta_state;
	bool bt_reduced_txpower;
	bool next_status_eosp;
	spinlock_t lock;
	struct iwl_mvm_tid_data tid_data[IWL_MAX_TID_COUNT + 1];
	u8 tid_to_baid[IWL_MAX_TID_COUNT];
	union {
		struct iwl_lq_sta_rs_fw rs_fw;
		struct iwl_lq_sta rs_drv;
	} lq_sta;
	struct ieee80211_vif *vif;
	struct iwl_mvm_key_pn __rcu *ptk_pn[4];
	struct iwl_mvm_rxq_dup_data *dup_data;

	u8 reserved_queue;

	/* Temporary, until the new TLC will control the Tx protection */
	s8 tx_protection;
	bool tt_tx_protection;

	bool disable_tx;
	u16 amsdu_enabled;
	u16 max_amsdu_len;
	u16 orig_amsdu_len;
	bool sleeping;
	u8 agg_tids;
	u8 sleep_tx_count;
	u8 avg_energy;
	u8 tx_ant;
};

u16 iwl_mvm_tid_queued(struct iwl_mvm *mvm, struct iwl_mvm_tid_data *tid_data);

static inline struct iwl_mvm_sta *
iwl_mvm_sta_from_mac80211(struct ieee80211_sta *sta)
{
	return (void *)sta->drv_priv;
}

/**
 * struct iwl_mvm_int_sta - representation of an internal station (auxiliary or
 * broadcast)
 * @sta_id: the index of the station in the fw (will be replaced by id_n_color)
 * @type: station type
 * @tfd_queue_msk: the tfd queues used by the station
 */
struct iwl_mvm_int_sta {
	u32 sta_id;
	enum iwl_sta_type type;
	u32 tfd_queue_msk;
};

/**
 * Send the STA info to the FW.
 *
 * @mvm: the iwl_mvm* to use
 * @sta: the STA
 * @update: this is true if the FW is being updated about a STA it already knows
 *	about. Otherwise (if this is a new STA), this should be false.
 * @flags: if update==true, this marks what is being changed via ORs of values
 *	from enum iwl_sta_modify_flag. Otherwise, this is ignored.
 */
int iwl_mvm_sta_send_to_fw(struct iwl_mvm *mvm, struct ieee80211_sta *sta,
			   bool update, unsigned int flags);
int iwl_mvm_add_sta(struct iwl_mvm *mvm,
		    struct ieee80211_vif *vif,
		    struct ieee80211_sta *sta);

static inline int iwl_mvm_update_sta(struct iwl_mvm *mvm,
				     struct ieee80211_vif *vif,
				     struct ieee80211_sta *sta)
{
	return iwl_mvm_sta_send_to_fw(mvm, sta, true, 0);
}

int iwl_mvm_wait_sta_queues_empty(struct iwl_mvm *mvm,
				  struct iwl_mvm_sta *mvm_sta);
int iwl_mvm_rm_sta(struct iwl_mvm *mvm,
		   struct ieee80211_vif *vif,
		   struct ieee80211_sta *sta);
int iwl_mvm_rm_sta_id(struct iwl_mvm *mvm,
		      struct ieee80211_vif *vif,
		      u8 sta_id);
int iwl_mvm_set_sta_key(struct iwl_mvm *mvm,
			struct ieee80211_vif *vif,
			struct ieee80211_sta *sta,
			struct ieee80211_key_conf *keyconf,
			u8 key_offset);
int iwl_mvm_remove_sta_key(struct iwl_mvm *mvm,
			   struct ieee80211_vif *vif,
			   struct ieee80211_sta *sta,
			   struct ieee80211_key_conf *keyconf);

void iwl_mvm_update_tkip_key(struct iwl_mvm *mvm,
			     struct ieee80211_vif *vif,
			     struct ieee80211_key_conf *keyconf,
			     struct ieee80211_sta *sta, u32 iv32,
			     u16 *phase1key);

void iwl_mvm_rx_eosp_notif(struct iwl_mvm *mvm,
			   struct iwl_rx_cmd_buffer *rxb);

/* AMPDU */
int iwl_mvm_sta_rx_agg(struct iwl_mvm *mvm, struct ieee80211_sta *sta,
		       int tid, u16 ssn, bool start, u16 buf_size, u16 timeout);
int iwl_mvm_sta_tx_agg_start(struct iwl_mvm *mvm, struct ieee80211_vif *vif,
			struct ieee80211_sta *sta, u16 tid, u16 *ssn);
int iwl_mvm_sta_tx_agg_oper(struct iwl_mvm *mvm, struct ieee80211_vif *vif,
			    struct ieee80211_sta *sta, u16 tid, u16 buf_size,
			    bool amsdu);
int iwl_mvm_sta_tx_agg_stop(struct iwl_mvm *mvm, struct ieee80211_vif *vif,
			    struct ieee80211_sta *sta, u16 tid);
int iwl_mvm_sta_tx_agg_flush(struct iwl_mvm *mvm, struct ieee80211_vif *vif,
			    struct ieee80211_sta *sta, u16 tid);

int iwl_mvm_sta_tx_agg(struct iwl_mvm *mvm, struct ieee80211_sta *sta,
		       int tid, u8 queue, bool start);

int iwl_mvm_add_aux_sta(struct iwl_mvm *mvm, u32 lmac_id);
int iwl_mvm_rm_aux_sta(struct iwl_mvm *mvm);

int iwl_mvm_alloc_bcast_sta(struct iwl_mvm *mvm, struct ieee80211_vif *vif);
int iwl_mvm_send_add_bcast_sta(struct iwl_mvm *mvm, struct ieee80211_vif *vif);
int iwl_mvm_add_p2p_bcast_sta(struct iwl_mvm *mvm, struct ieee80211_vif *vif);
int iwl_mvm_send_rm_bcast_sta(struct iwl_mvm *mvm, struct ieee80211_vif *vif);
int iwl_mvm_rm_p2p_bcast_sta(struct iwl_mvm *mvm, struct ieee80211_vif *vif);
int iwl_mvm_add_mcast_sta(struct iwl_mvm *mvm, struct ieee80211_vif *vif);
int iwl_mvm_rm_mcast_sta(struct iwl_mvm *mvm, struct ieee80211_vif *vif);
int iwl_mvm_allocate_int_sta(struct iwl_mvm *mvm,
			     struct iwl_mvm_int_sta *sta,
				    u32 qmask, enum nl80211_iftype iftype,
				    enum iwl_sta_type type);
void iwl_mvm_dealloc_bcast_sta(struct iwl_mvm *mvm, struct ieee80211_vif *vif);
void iwl_mvm_dealloc_int_sta(struct iwl_mvm *mvm, struct iwl_mvm_int_sta *sta);
int iwl_mvm_add_snif_sta(struct iwl_mvm *mvm, struct ieee80211_vif *vif);
int iwl_mvm_rm_snif_sta(struct iwl_mvm *mvm, struct ieee80211_vif *vif);
void iwl_mvm_dealloc_snif_sta(struct iwl_mvm *mvm);

void iwl_mvm_sta_modify_ps_wake(struct iwl_mvm *mvm,
				struct ieee80211_sta *sta);
void iwl_mvm_sta_modify_sleep_tx_count(struct iwl_mvm *mvm,
				       struct ieee80211_sta *sta,
				       enum ieee80211_frame_release_type reason,
				       u16 cnt, u16 tids, bool more_data,
				       bool single_sta_queue);
int iwl_mvm_drain_sta(struct iwl_mvm *mvm, struct iwl_mvm_sta *mvmsta,
		      bool drain);
void iwl_mvm_sta_modify_disable_tx(struct iwl_mvm *mvm,
				   struct iwl_mvm_sta *mvmsta, bool disable);
void iwl_mvm_sta_modify_disable_tx_ap(struct iwl_mvm *mvm,
				      struct ieee80211_sta *sta,
				      bool disable);
void iwl_mvm_modify_all_sta_disable_tx(struct iwl_mvm *mvm,
				       struct iwl_mvm_vif *mvmvif,
				       bool disable);
void iwl_mvm_csa_client_absent(struct iwl_mvm *mvm, struct ieee80211_vif *vif);
void iwl_mvm_add_new_dqa_stream_wk(struct work_struct *wk);
int iwl_mvm_add_pasn_sta(struct iwl_mvm *mvm, struct ieee80211_vif *vif,
			 struct iwl_mvm_int_sta *sta, u8 *addr, u32 cipher,
			 u8 *key, u32 key_len);
#endif /* __sta_h__ */
