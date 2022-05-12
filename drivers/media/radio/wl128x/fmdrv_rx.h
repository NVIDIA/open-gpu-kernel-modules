/* SPDX-License-Identifier: GPL-2.0-only */
/*
 *  FM Driver for Connectivity chip of Texas Instruments.
 *  FM RX module header.
 *
 *  Copyright (C) 2011 Texas Instruments
 */

#ifndef _FMDRV_RX_H
#define _FMDRV_RX_H

int fm_rx_set_freq(struct fmdev *, u32);
int fm_rx_set_mute_mode(struct fmdev *, u8);
int fm_rx_set_stereo_mono(struct fmdev *, u16);
int fm_rx_set_rds_mode(struct fmdev *, u8);
int fm_rx_set_rds_system(struct fmdev *, u8);
int fm_rx_set_volume(struct fmdev *, u16);
int fm_rx_set_rssi_threshold(struct fmdev *, short);
int fm_rx_set_region(struct fmdev *, u8);
int fm_rx_set_rfdepend_softmute(struct fmdev *, u8);
int fm_rx_set_deemphasis_mode(struct fmdev *, u16);
int fm_rx_set_af_switch(struct fmdev *, u8);

void fm_rx_reset_rds_cache(struct fmdev *);
void fm_rx_reset_station_info(struct fmdev *);

int fm_rx_seek(struct fmdev *, u32, u32, u32);

int fm_rx_get_rds_mode(struct fmdev *, u8 *);
int fm_rx_get_mute_mode(struct fmdev *, u8 *);
int fm_rx_get_volume(struct fmdev *, u16 *);
int fm_rx_get_band_freq_range(struct fmdev *,
					u32 *, u32 *);
int fm_rx_get_stereo_mono(struct fmdev *, u16 *);
int fm_rx_get_rssi_level(struct fmdev *, u16 *);
int fm_rx_get_rssi_threshold(struct fmdev *, short *);
int fm_rx_get_rfdepend_softmute(struct fmdev *, u8 *);
int fm_rx_get_deemph_mode(struct fmdev *, u16 *);
int fm_rx_get_af_switch(struct fmdev *, u8 *);
void fm_rx_get_region(struct fmdev *, u8 *);

int fm_rx_set_chanl_spacing(struct fmdev *, u8);
int fm_rx_get_chanl_spacing(struct fmdev *, u8 *);
#endif

