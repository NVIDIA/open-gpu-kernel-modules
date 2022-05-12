/* SPDX-License-Identifier: GPL-2.0-or-later */
/*

  Broadcom B43 wireless driver

  Copyright (c) 2005 Martin Langer <martin-langer@gmx.de>,
                     Stefano Brivio <stefano.brivio@polimi.it>
                     Michael Buesch <m@bues.ch>
                     Danny van Dyk <kugelfang@gentoo.org>
                     Andreas Jaggi <andreas.jaggi@waterwave.ch>

  Some parts of the code in this file are derived from the ipw2200
  driver  Copyright(c) 2003 - 2004 Intel Corporation.


*/

#ifndef B43_MAIN_H_
#define B43_MAIN_H_

#include "b43.h"

#define P4D_BYT3S(magic, nr_bytes)	u8 __p4dding##magic[nr_bytes]
#define P4D_BYTES(line, nr_bytes)	P4D_BYT3S(line, nr_bytes)
/* Magic helper macro to pad structures. Ignore those above. It's magic. */
#define PAD_BYTES(nr_bytes)		P4D_BYTES( __LINE__ , (nr_bytes))


extern int b43_modparam_verbose;

/* Logmessage verbosity levels. Update the b43_modparam_verbose helptext, if
 * you add or remove levels. */
enum b43_verbosity {
	B43_VERBOSITY_ERROR,
	B43_VERBOSITY_WARN,
	B43_VERBOSITY_INFO,
	B43_VERBOSITY_DEBUG,
	__B43_VERBOSITY_AFTERLAST, /* keep last */

	B43_VERBOSITY_MAX = __B43_VERBOSITY_AFTERLAST - 1,
#if B43_DEBUG
	B43_VERBOSITY_DEFAULT = B43_VERBOSITY_DEBUG,
#else
	B43_VERBOSITY_DEFAULT = B43_VERBOSITY_INFO,
#endif
};

static inline int b43_is_cck_rate(int rate)
{
	return (rate == B43_CCK_RATE_1MB ||
		rate == B43_CCK_RATE_2MB ||
		rate == B43_CCK_RATE_5MB || rate == B43_CCK_RATE_11MB);
}

static inline int b43_is_ofdm_rate(int rate)
{
	return !b43_is_cck_rate(rate);
}

u8 b43_ieee80211_antenna_sanitize(struct b43_wldev *dev,
				  u8 antenna_nr);

void b43_tsf_read(struct b43_wldev *dev, u64 * tsf);
void b43_tsf_write(struct b43_wldev *dev, u64 tsf);

u32 b43_shm_read32(struct b43_wldev *dev, u16 routing, u16 offset);
u16 b43_shm_read16(struct b43_wldev *dev, u16 routing, u16 offset);
void b43_shm_write32(struct b43_wldev *dev, u16 routing, u16 offset, u32 value);
void b43_shm_write16(struct b43_wldev *dev, u16 routing, u16 offset, u16 value);

u64 b43_hf_read(struct b43_wldev *dev);
void b43_hf_write(struct b43_wldev *dev, u64 value);

void b43_dummy_transmission(struct b43_wldev *dev, bool ofdm, bool pa_on);

void b43_wireless_core_reset(struct b43_wldev *dev, bool gmode);

void b43_controller_restart(struct b43_wldev *dev, const char *reason);

#define B43_PS_ENABLED	(1 << 0)	/* Force enable hardware power saving */
#define B43_PS_DISABLED	(1 << 1)	/* Force disable hardware power saving */
#define B43_PS_AWAKE	(1 << 2)	/* Force device awake */
#define B43_PS_ASLEEP	(1 << 3)	/* Force device asleep */
void b43_power_saving_ctl_bits(struct b43_wldev *dev, unsigned int ps_flags);

void b43_wireless_core_phy_pll_reset(struct b43_wldev *dev);

void b43_mac_suspend(struct b43_wldev *dev);
void b43_mac_enable(struct b43_wldev *dev);
void b43_mac_phy_clock_set(struct b43_wldev *dev, bool on);
void b43_mac_switch_freq(struct b43_wldev *dev, u8 spurmode);


struct b43_request_fw_context;
int b43_do_request_fw(struct b43_request_fw_context *ctx, const char *name,
		      struct b43_firmware_file *fw, bool async);
void b43_do_release_fw(struct b43_firmware_file *fw);

#endif /* B43_MAIN_H_ */
