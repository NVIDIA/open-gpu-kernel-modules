/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * This file is part of wl1271
 *
 * Copyright (C) 2009 Nokia Corporation
 *
 * Contact: Luciano Coelho <luciano.coelho@nokia.com>
 */

#ifndef __INIT_H__
#define __INIT_H__

#include "wlcore.h"

int wl1271_hw_init_power_auth(struct wl1271 *wl);
int wl1271_init_templates_config(struct wl1271 *wl);
int wl1271_init_pta(struct wl1271 *wl);
int wl1271_init_energy_detection(struct wl1271 *wl);
int wl1271_chip_specific_init(struct wl1271 *wl);
int wl1271_hw_init(struct wl1271 *wl);
int wl1271_init_vif_specific(struct wl1271 *wl, struct ieee80211_vif *vif);
int wl1271_init_ap_rates(struct wl1271 *wl, struct wl12xx_vif *wlvif);
int wl1271_ap_init_templates(struct wl1271 *wl, struct ieee80211_vif *vif);
int wl1271_sta_hw_init(struct wl1271 *wl, struct wl12xx_vif *wlvif);

#endif
