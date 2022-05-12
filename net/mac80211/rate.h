/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright 2002-2005, Instant802 Networks, Inc.
 * Copyright 2005, Devicescape Software, Inc.
 * Copyright (c) 2006 Jiri Benc <jbenc@suse.cz>
 */

#ifndef IEEE80211_RATE_H
#define IEEE80211_RATE_H

#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/types.h>
#include <net/mac80211.h>
#include "ieee80211_i.h"
#include "sta_info.h"
#include "driver-ops.h"

struct rate_control_ref {
	const struct rate_control_ops *ops;
	void *priv;
};

void rate_control_get_rate(struct ieee80211_sub_if_data *sdata,
			   struct sta_info *sta,
			   struct ieee80211_tx_rate_control *txrc);

void rate_control_tx_status(struct ieee80211_local *local,
			    struct ieee80211_supported_band *sband,
			    struct ieee80211_tx_status *st);

void rate_control_rate_init(struct sta_info *sta);
void rate_control_rate_update(struct ieee80211_local *local,
				    struct ieee80211_supported_band *sband,
				    struct sta_info *sta, u32 changed);

static inline void *rate_control_alloc_sta(struct rate_control_ref *ref,
					   struct sta_info *sta, gfp_t gfp)
{
	spin_lock_init(&sta->rate_ctrl_lock);
	return ref->ops->alloc_sta(ref->priv, &sta->sta, gfp);
}

static inline void rate_control_free_sta(struct sta_info *sta)
{
	struct rate_control_ref *ref = sta->rate_ctrl;
	struct ieee80211_sta *ista = &sta->sta;
	void *priv_sta = sta->rate_ctrl_priv;

	ref->ops->free_sta(ref->priv, ista, priv_sta);
}

static inline void rate_control_add_sta_debugfs(struct sta_info *sta)
{
#ifdef CONFIG_MAC80211_DEBUGFS
	struct rate_control_ref *ref = sta->rate_ctrl;
	if (ref && sta->debugfs_dir && ref->ops->add_sta_debugfs)
		ref->ops->add_sta_debugfs(ref->priv, sta->rate_ctrl_priv,
					  sta->debugfs_dir);
#endif
}

extern const struct file_operations rcname_ops;

static inline void rate_control_add_debugfs(struct ieee80211_local *local)
{
#ifdef CONFIG_MAC80211_DEBUGFS
	struct dentry *debugfsdir;

	if (!local->rate_ctrl)
		return;

	if (!local->rate_ctrl->ops->add_debugfs)
		return;

	debugfsdir = debugfs_create_dir("rc", local->hw.wiphy->debugfsdir);
	local->debugfs.rcdir = debugfsdir;
	debugfs_create_file("name", 0400, debugfsdir,
			    local->rate_ctrl, &rcname_ops);

	local->rate_ctrl->ops->add_debugfs(&local->hw, local->rate_ctrl->priv,
					   debugfsdir);
#endif
}

void ieee80211_check_rate_mask(struct ieee80211_sub_if_data *sdata);

/* Get a reference to the rate control algorithm. If `name' is NULL, get the
 * first available algorithm. */
int ieee80211_init_rate_ctrl_alg(struct ieee80211_local *local,
				 const char *name);
void rate_control_deinitialize(struct ieee80211_local *local);


/* Rate control algorithms */
#ifdef CONFIG_MAC80211_RC_MINSTREL
int rc80211_minstrel_init(void);
void rc80211_minstrel_exit(void);
#else
static inline int rc80211_minstrel_init(void)
{
	return 0;
}
static inline void rc80211_minstrel_exit(void)
{
}
#endif


#endif /* IEEE80211_RATE_H */
