// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause
/*
 * Copyright (C) 2012-2014, 2018-2020 Intel Corporation
 * Copyright (C) 2013-2014 Intel Mobile Communications GmbH
 * Copyright (C) 2017 Intel Deutschland GmbH
 */
#include <net/mac80211.h>
#include "fw-api.h"
#include "mvm.h"

/* Maps the driver specific channel width definition to the fw values */
u8 iwl_mvm_get_channel_width(struct cfg80211_chan_def *chandef)
{
	switch (chandef->width) {
	case NL80211_CHAN_WIDTH_20_NOHT:
	case NL80211_CHAN_WIDTH_20:
		return PHY_VHT_CHANNEL_MODE20;
	case NL80211_CHAN_WIDTH_40:
		return PHY_VHT_CHANNEL_MODE40;
	case NL80211_CHAN_WIDTH_80:
		return PHY_VHT_CHANNEL_MODE80;
	case NL80211_CHAN_WIDTH_160:
		return PHY_VHT_CHANNEL_MODE160;
	default:
		WARN(1, "Invalid channel width=%u", chandef->width);
		return PHY_VHT_CHANNEL_MODE20;
	}
}

/*
 * Maps the driver specific control channel position (relative to the center
 * freq) definitions to the the fw values
 */
u8 iwl_mvm_get_ctrl_pos(struct cfg80211_chan_def *chandef)
{
	switch (chandef->chan->center_freq - chandef->center_freq1) {
	case -70:
		return PHY_VHT_CTRL_POS_4_BELOW;
	case -50:
		return PHY_VHT_CTRL_POS_3_BELOW;
	case -30:
		return PHY_VHT_CTRL_POS_2_BELOW;
	case -10:
		return PHY_VHT_CTRL_POS_1_BELOW;
	case  10:
		return PHY_VHT_CTRL_POS_1_ABOVE;
	case  30:
		return PHY_VHT_CTRL_POS_2_ABOVE;
	case  50:
		return PHY_VHT_CTRL_POS_3_ABOVE;
	case  70:
		return PHY_VHT_CTRL_POS_4_ABOVE;
	default:
		WARN(1, "Invalid channel definition");
		fallthrough;
	case 0:
		/*
		 * The FW is expected to check the control channel position only
		 * when in HT/VHT and the channel width is not 20MHz. Return
		 * this value as the default one.
		 */
		return PHY_VHT_CTRL_POS_1_BELOW;
	}
}

/*
 * Construct the generic fields of the PHY context command
 */
static void iwl_mvm_phy_ctxt_cmd_hdr(struct iwl_mvm_phy_ctxt *ctxt,
				     struct iwl_phy_context_cmd *cmd,
				     u32 action)
{
	cmd->id_and_color = cpu_to_le32(FW_CMD_ID_AND_COLOR(ctxt->id,
							    ctxt->color));
	cmd->action = cpu_to_le32(action);
}

static void iwl_mvm_phy_ctxt_set_rxchain(struct iwl_mvm *mvm,
					 __le32 *rxchain_info,
					 u8 chains_static,
					 u8 chains_dynamic)
{
	u8 active_cnt, idle_cnt;

	/* Set rx the chains */
	idle_cnt = chains_static;
	active_cnt = chains_dynamic;

	/* In scenarios where we only ever use a single-stream rates,
	 * i.e. legacy 11b/g/a associations, single-stream APs or even
	 * static SMPS, enable both chains to get diversity, improving
	 * the case where we're far enough from the AP that attenuation
	 * between the two antennas is sufficiently different to impact
	 * performance.
	 */
	if (active_cnt == 1 && iwl_mvm_rx_diversity_allowed(mvm)) {
		idle_cnt = 2;
		active_cnt = 2;
	}

	*rxchain_info = cpu_to_le32(iwl_mvm_get_valid_rx_ant(mvm) <<
					PHY_RX_CHAIN_VALID_POS);
	*rxchain_info |= cpu_to_le32(idle_cnt << PHY_RX_CHAIN_CNT_POS);
	*rxchain_info |= cpu_to_le32(active_cnt <<
					 PHY_RX_CHAIN_MIMO_CNT_POS);
#ifdef CONFIG_IWLWIFI_DEBUGFS
	if (unlikely(mvm->dbgfs_rx_phyinfo))
		*rxchain_info = cpu_to_le32(mvm->dbgfs_rx_phyinfo);
#endif
}

/*
 * Add the phy configuration to the PHY context command
 */
static void iwl_mvm_phy_ctxt_cmd_data_v1(struct iwl_mvm *mvm,
					 struct iwl_phy_context_cmd_v1 *cmd,
					 struct cfg80211_chan_def *chandef,
					 u8 chains_static, u8 chains_dynamic)
{
	struct iwl_phy_context_cmd_tail *tail =
		iwl_mvm_chan_info_cmd_tail(mvm, &cmd->ci);

	/* Set the channel info data */
	iwl_mvm_set_chan_info_chandef(mvm, &cmd->ci, chandef);

	iwl_mvm_phy_ctxt_set_rxchain(mvm, &tail->rxchain_info,
				     chains_static, chains_dynamic);

	tail->txchain_info = cpu_to_le32(iwl_mvm_get_valid_tx_ant(mvm));
}

/*
 * Add the phy configuration to the PHY context command
 */
static void iwl_mvm_phy_ctxt_cmd_data(struct iwl_mvm *mvm,
				      struct iwl_phy_context_cmd *cmd,
				      struct cfg80211_chan_def *chandef,
				      u8 chains_static, u8 chains_dynamic)
{
	cmd->lmac_id = cpu_to_le32(iwl_mvm_get_lmac_id(mvm->fw,
						       chandef->chan->band));

	/* Set the channel info data */
	iwl_mvm_set_chan_info_chandef(mvm, &cmd->ci, chandef);

	iwl_mvm_phy_ctxt_set_rxchain(mvm, &cmd->rxchain_info,
				     chains_static, chains_dynamic);
}

/*
 * Send a command to apply the current phy configuration. The command is send
 * only if something in the configuration changed: in case that this is the
 * first time that the phy configuration is applied or in case that the phy
 * configuration changed from the previous apply.
 */
static int iwl_mvm_phy_ctxt_apply(struct iwl_mvm *mvm,
				  struct iwl_mvm_phy_ctxt *ctxt,
				  struct cfg80211_chan_def *chandef,
				  u8 chains_static, u8 chains_dynamic,
				  u32 action)
{
	int ret;
	int ver = iwl_fw_lookup_cmd_ver(mvm->fw, IWL_ALWAYS_LONG_GROUP,
					PHY_CONTEXT_CMD, 1);

	if (ver == 3) {
		struct iwl_phy_context_cmd cmd = {};

		/* Set the command header fields */
		iwl_mvm_phy_ctxt_cmd_hdr(ctxt, &cmd, action);

		/* Set the command data */
		iwl_mvm_phy_ctxt_cmd_data(mvm, &cmd, chandef,
					  chains_static,
					  chains_dynamic);

		ret = iwl_mvm_send_cmd_pdu(mvm, PHY_CONTEXT_CMD,
					   0, sizeof(cmd), &cmd);
	} else if (ver < 3) {
		struct iwl_phy_context_cmd_v1 cmd = {};
		u16 len = sizeof(cmd) - iwl_mvm_chan_info_padding(mvm);

		/* Set the command header fields */
		iwl_mvm_phy_ctxt_cmd_hdr(ctxt,
					 (struct iwl_phy_context_cmd *)&cmd,
					 action);

		/* Set the command data */
		iwl_mvm_phy_ctxt_cmd_data_v1(mvm, &cmd, chandef,
					     chains_static,
					     chains_dynamic);
		ret = iwl_mvm_send_cmd_pdu(mvm, PHY_CONTEXT_CMD,
					   0, len, &cmd);
	} else {
		IWL_ERR(mvm, "PHY ctxt cmd error ver %d not supported\n", ver);
		return -EOPNOTSUPP;
	}


	if (ret)
		IWL_ERR(mvm, "PHY ctxt cmd error. ret=%d\n", ret);
	return ret;
}

/*
 * Send a command to add a PHY context based on the current HW configuration.
 */
int iwl_mvm_phy_ctxt_add(struct iwl_mvm *mvm, struct iwl_mvm_phy_ctxt *ctxt,
			 struct cfg80211_chan_def *chandef,
			 u8 chains_static, u8 chains_dynamic)
{
	WARN_ON(!test_bit(IWL_MVM_STATUS_IN_HW_RESTART, &mvm->status) &&
		ctxt->ref);
	lockdep_assert_held(&mvm->mutex);

	ctxt->channel = chandef->chan;

	return iwl_mvm_phy_ctxt_apply(mvm, ctxt, chandef,
				      chains_static, chains_dynamic,
				      FW_CTXT_ACTION_ADD);
}

/*
 * Update the number of references to the given PHY context. This is valid only
 * in case the PHY context was already created, i.e., its reference count > 0.
 */
void iwl_mvm_phy_ctxt_ref(struct iwl_mvm *mvm, struct iwl_mvm_phy_ctxt *ctxt)
{
	lockdep_assert_held(&mvm->mutex);
	ctxt->ref++;
}

/*
 * Send a command to modify the PHY context based on the current HW
 * configuration. Note that the function does not check that the configuration
 * changed.
 */
int iwl_mvm_phy_ctxt_changed(struct iwl_mvm *mvm, struct iwl_mvm_phy_ctxt *ctxt,
			     struct cfg80211_chan_def *chandef,
			     u8 chains_static, u8 chains_dynamic)
{
	enum iwl_ctxt_action action = FW_CTXT_ACTION_MODIFY;

	lockdep_assert_held(&mvm->mutex);

	if (fw_has_capa(&mvm->fw->ucode_capa,
			IWL_UCODE_TLV_CAPA_BINDING_CDB_SUPPORT) &&
	    ctxt->channel->band != chandef->chan->band) {
		int ret;

		/* ... remove it here ...*/
		ret = iwl_mvm_phy_ctxt_apply(mvm, ctxt, chandef,
					     chains_static, chains_dynamic,
					     FW_CTXT_ACTION_REMOVE);
		if (ret)
			return ret;

		/* ... and proceed to add it again */
		action = FW_CTXT_ACTION_ADD;
	}

	ctxt->channel = chandef->chan;
	ctxt->width = chandef->width;
	return iwl_mvm_phy_ctxt_apply(mvm, ctxt, chandef,
				      chains_static, chains_dynamic,
				      action);
}

void iwl_mvm_phy_ctxt_unref(struct iwl_mvm *mvm, struct iwl_mvm_phy_ctxt *ctxt)
{
	lockdep_assert_held(&mvm->mutex);

	if (WARN_ON_ONCE(!ctxt))
		return;

	ctxt->ref--;

	/*
	 * Move unused phy's to a default channel. When the phy is moved the,
	 * fw will cleanup immediate quiet bit if it was previously set,
	 * otherwise we might not be able to reuse this phy.
	 */
	if (ctxt->ref == 0) {
		struct ieee80211_channel *chan;
		struct cfg80211_chan_def chandef;
		struct ieee80211_supported_band *sband = NULL;
		enum nl80211_band band = NL80211_BAND_2GHZ;

		while (!sband && band < NUM_NL80211_BANDS)
			sband = mvm->hw->wiphy->bands[band++];

		if (WARN_ON(!sband))
			return;

		chan = &sband->channels[0];

		cfg80211_chandef_create(&chandef, chan, NL80211_CHAN_NO_HT);
		iwl_mvm_phy_ctxt_changed(mvm, ctxt, &chandef, 1, 1);
	}
}

static void iwl_mvm_binding_iterator(void *_data, u8 *mac,
				     struct ieee80211_vif *vif)
{
	unsigned long *data = _data;
	struct iwl_mvm_vif *mvmvif = iwl_mvm_vif_from_mac80211(vif);

	if (!mvmvif->phy_ctxt)
		return;

	if (vif->type == NL80211_IFTYPE_STATION ||
	    vif->type == NL80211_IFTYPE_AP)
		__set_bit(mvmvif->phy_ctxt->id, data);
}

int iwl_mvm_phy_ctx_count(struct iwl_mvm *mvm)
{
	unsigned long phy_ctxt_counter = 0;

	ieee80211_iterate_active_interfaces_atomic(mvm->hw,
						   IEEE80211_IFACE_ITER_NORMAL,
						   iwl_mvm_binding_iterator,
						   &phy_ctxt_counter);

	return hweight8(phy_ctxt_counter);
}
