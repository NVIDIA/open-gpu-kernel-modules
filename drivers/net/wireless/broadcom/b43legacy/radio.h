/* SPDX-License-Identifier: GPL-2.0-or-later */
/*

  Broadcom B43legacy wireless driver

  Copyright (c) 2005 Martin Langer <martin-langer@gmx.de>,
		     Stefano Brivio <stefano.brivio@polimi.it>
		     Michael Buesch <m@bues.ch>
		     Danny van Dyk <kugelfang@gentoo.org>
		     Andreas Jaggi <andreas.jaggi@waterwave.ch>

  Some parts of the code in this file are derived from the ipw2200
  driver  Copyright(c) 2003 - 2004 Intel Corporation.


*/

#ifndef B43legacy_RADIO_H_
#define B43legacy_RADIO_H_

#include "b43legacy.h"


#define B43legacy_RADIO_DEFAULT_CHANNEL_BG	6

/* Force antenna 0. */
#define B43legacy_RADIO_TXANTENNA_0		0
/* Force antenna 1. */
#define B43legacy_RADIO_TXANTENNA_1		1
/* Use the RX antenna, that was selected for the most recently
 * received good PLCP header.
 */
#define B43legacy_RADIO_TXANTENNA_LASTPLCP	3
#define B43legacy_RADIO_TXANTENNA_DEFAULT	B43legacy_RADIO_TXANTENNA_LASTPLCP

#define B43legacy_RADIO_INTERFMODE_NONE		0
#define B43legacy_RADIO_INTERFMODE_NONWLAN	1
#define B43legacy_RADIO_INTERFMODE_MANUALWLAN	2
#define B43legacy_RADIO_INTERFMODE_AUTOWLAN	3


void b43legacy_radio_lock(struct b43legacy_wldev *dev);
void b43legacy_radio_unlock(struct b43legacy_wldev *dev);

u16 b43legacy_radio_read16(struct b43legacy_wldev *dev, u16 offset);
void b43legacy_radio_write16(struct b43legacy_wldev *dev, u16 offset, u16 val);

u16 b43legacy_radio_init2050(struct b43legacy_wldev *dev);

void b43legacy_radio_turn_on(struct b43legacy_wldev *dev);
void b43legacy_radio_turn_off(struct b43legacy_wldev *dev, bool force);

int b43legacy_radio_selectchannel(struct b43legacy_wldev *dev, u8 channel,
				  int synthetic_pu_workaround);

void b43legacy_radio_set_txpower_a(struct b43legacy_wldev *dev, u16 txpower);
void b43legacy_radio_set_txpower_bg(struct b43legacy_wldev *dev,
				    u16 baseband_attenuation, u16 attenuation,
				    u16 txpower);

u16 b43legacy_default_baseband_attenuation(struct b43legacy_wldev *dev);
u16 b43legacy_default_radio_attenuation(struct b43legacy_wldev *dev);
u16 b43legacy_default_txctl1(struct b43legacy_wldev *dev);

void b43legacy_radio_set_txantenna(struct b43legacy_wldev *dev, u32 val);

void b43legacy_radio_clear_tssi(struct b43legacy_wldev *dev);

u8 b43legacy_radio_aci_detect(struct b43legacy_wldev *dev, u8 channel);
u8 b43legacy_radio_aci_scan(struct b43legacy_wldev *dev);

int b43legacy_radio_set_interference_mitigation(struct b43legacy_wldev *dev,
						int mode);

void b43legacy_calc_nrssi_slope(struct b43legacy_wldev *dev);
void b43legacy_calc_nrssi_threshold(struct b43legacy_wldev *dev);
s16 b43legacy_nrssi_hw_read(struct b43legacy_wldev *dev, u16 offset);
void b43legacy_nrssi_hw_write(struct b43legacy_wldev *dev, u16 offset, s16 val);
void b43legacy_nrssi_hw_update(struct b43legacy_wldev *dev, u16 val);
void b43legacy_nrssi_mem_update(struct b43legacy_wldev *dev);

u16 b43legacy_radio_calibrationvalue(struct b43legacy_wldev *dev);

#endif /* B43legacy_RADIO_H_ */
