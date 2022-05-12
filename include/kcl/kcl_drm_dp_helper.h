/*
 * Copyright © 2008 Keith Packard
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the name of the copyright holders not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  The copyright holders make no representations
 * about the suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */


#ifndef _KCL_DRM_DP_HELPER_H_
#define _KCL_DRM_DP_HELPER_H_

#include <linux/types.h>
#include <linux/i2c.h>
#include <linux/delay.h>

#include <drm/drm_connector.h>
#include <drm/drm_device.h>
#include <drm/drm_dp_helper.h>
#include <kcl/kcl_drm_dp_cec.h>

/*
 * v4.13-rc5-840-gc673fe7f0cd5
 * drm/dp: DPCD register defines for link status within ESI field
 */
#ifndef DP_LANE0_1_STATUS_ESI
#define DP_LANE0_1_STATUS_ESI                  0x200c /* status same as 0x202 */
#define DP_LANE2_3_STATUS_ESI                  0x200d /* status same as 0x203 */
#define DP_LANE_ALIGN_STATUS_UPDATED_ESI       0x200e /* status same as 0x204 */
#define DP_SINK_STATUS_ESI                     0x200f /* status same as 0x205 */
#endif

/*
 * v4.13-rc5-1383-gac58fff15516
 * drm/dp-helper: add missing defines needed by AMD display core.
 */
#ifndef DP_ADJUST_REQUEST_POST_CURSOR2
#define DP_ADJUST_REQUEST_POST_CURSOR2      0x20c

#define DP_TEST_MISC0                       0x232

#define DP_TEST_PHY_PATTERN                 0x248
#define DP_TEST_80BIT_CUSTOM_PATTERN_7_0    0x250
#define	DP_TEST_80BIT_CUSTOM_PATTERN_15_8   0x251
#define	DP_TEST_80BIT_CUSTOM_PATTERN_23_16  0x252
#define	DP_TEST_80BIT_CUSTOM_PATTERN_31_24  0x253
#define	DP_TEST_80BIT_CUSTOM_PATTERN_39_32  0x254
#define	DP_TEST_80BIT_CUSTOM_PATTERN_47_40  0x255
#define	DP_TEST_80BIT_CUSTOM_PATTERN_55_48  0x256
#define	DP_TEST_80BIT_CUSTOM_PATTERN_63_56  0x257
#define	DP_TEST_80BIT_CUSTOM_PATTERN_71_64  0x258
#define	DP_TEST_80BIT_CUSTOM_PATTERN_79_72  0x259

#define DP_BRANCH_REVISION_START            0x509

#define DP_DP13_DPCD_REV                    0x2200
#define DP_DP13_MAX_LINK_RATE               0x2201
#endif


#if !defined(DP_DPRX_FEATURE_ENUMERATION_LIST)
#define DP_DPRX_FEATURE_ENUMERATION_LIST    0x2210  /* DP 1.3 */
#endif

#if !defined(DP_TRAINING_PATTERN_SET_PHY_REPEATER1)
#define DP_TRAINING_PATTERN_SET_PHY_REPEATER1              0xf0010 /* 1.3 */
#endif

#if !defined(DP_LANE0_1_STATUS_PHY_REPEATER1)
#define DP_LANE0_1_STATUS_PHY_REPEATER1                            0xf0030 /* 1.3 */
#endif

#if !defined(DP_ADJUST_REQUEST_LANE0_1_PHY_REPEATER1)
#define DP_ADJUST_REQUEST_LANE0_1_PHY_REPEATER1                    0xf0033 /* 1.3 */
#endif

#if !defined(DP_TRAINING_LANE0_SET_PHY_REPEATER1)
#define DP_TRAINING_LANE0_SET_PHY_REPEATER1                0xf0011 /* 1.3 */
#endif

#if !defined(DP_PHY_REPEATER_MODE_TRANSPARENT)
#define DP_PHY_REPEATER_MODE_TRANSPARENT                   0x55    /* 1.3 */
#endif

#if !defined(DP_PHY_REPEATER_MODE)
#define DP_PHY_REPEATER_MODE                               0xf0003 /* 1.3 */
#endif

#if !defined(DP_PHY_REPEATER_MODE_NON_TRANSPARENT)
#define DP_PHY_REPEATER_MODE_NON_TRANSPARENT               0xaa    /* 1.3 */
#endif

#if !defined(DP_TRAINING_AUX_RD_INTERVAL_PHY_REPEATER1)
#define DP_TRAINING_AUX_RD_INTERVAL_PHY_REPEATER1          0xf0020 /* 1.4a */
#endif

#if !defined(DP_PHY_REPEATER_EXTENDED_WAIT_TIMEOUT)
#define DP_PHY_REPEATER_EXTENDED_WAIT_TIMEOUT              0xf0005 /* 1.4a */
#endif

#if !defined(DP_LT_TUNABLE_PHY_REPEATER_FIELD_DATA_STRUCTURE_REV)
#define DP_LT_TUNABLE_PHY_REPEATER_FIELD_DATA_STRUCTURE_REV 0xf0000 /* 1.3 */
#endif

#if !defined(DP_MAX_LINK_RATE_PHY_REPEATER)
#define DP_MAX_LINK_RATE_PHY_REPEATER                      0xf0001 /* 1.4a */
#endif

#if !defined(DP_PHY_REPEATER_CNT)
#define DP_PHY_REPEATER_CNT                                0xf0002 /* 1.3 */
#endif

#if !defined(DP_MAX_LANE_COUNT_PHY_REPEATER)
#define DP_MAX_LANE_COUNT_PHY_REPEATER                     0xf0004 /* 1.4a */
#endif

#if !defined(DP_TEST_AUDIO_MODE)
#define DP_TEST_AUDIO_MODE                 0x271
#endif

#if !defined(DP_TEST_AUDIO_PATTERN_TYPE)
#define DP_TEST_AUDIO_PATTERN_TYPE         0x272
#endif

#if !defined(DP_TEST_AUDIO_PERIOD_CH1)
#define DP_TEST_AUDIO_PERIOD_CH1           0x273
#endif

#if !defined(DP_DSC_SUPPORT)
#define DP_DSC_SUPPORT                      0x060   /* DP 1.4 */
#endif

/*
 * v5.6-1624-g8811d9eb4dfa
 * drm/amd/display: Align macro name as per DP spec
 */
#ifdef DP_TEST_PHY_PATTERN
#define DP_PHY_TEST_PATTERN DP_TEST_PHY_PATTERN
#endif

/* commit fc1424c2ec813080aa1eaa2948070902b1a0e507
 * drm: Correct DP DSC macro typo */
#ifdef DP_DSC_THROUGHPUT_MODE_0_UPSUPPORTED
#define DP_DSC_THROUGHPUT_MODE_0_UNSUPPORTED DP_DSC_THROUGHPUT_MODE_0_UPSUPPORTED
#endif

/* v5.9-rc4-979-g9782f52ab5d6
 * drm/dp: Add LTTPR helpers
 */
#ifndef DP_TRAINING_PATTERN_SET_PHY_REPEATER

enum drm_dp_phy {
        DP_PHY_DPRX,

        DP_PHY_LTTPR1,
        DP_PHY_LTTPR2,
        DP_PHY_LTTPR3,
        DP_PHY_LTTPR4,
        DP_PHY_LTTPR5,
        DP_PHY_LTTPR6,
        DP_PHY_LTTPR7,
        DP_PHY_LTTPR8,

        DP_MAX_LTTPR_COUNT = DP_PHY_LTTPR8,
};

#define DP_PHY_LTTPR(i)                                     (DP_PHY_LTTPR1 + (i))
#define __DP_LTTPR1_BASE                                    0xf0010 /* 1.3 */
#define __DP_LTTPR2_BASE                                    0xf0060 /* 1.3 */
#define DP_LTTPR_BASE(dp_phy) \
        (__DP_LTTPR1_BASE + (__DP_LTTPR2_BASE - __DP_LTTPR1_BASE) * \
                ((dp_phy) - DP_PHY_LTTPR1))
#define DP_LTTPR_REG(dp_phy, lttpr1_reg) \
        (DP_LTTPR_BASE(dp_phy) - DP_LTTPR_BASE(DP_PHY_LTTPR1) + (lttpr1_reg))
#define DP_TRAINING_PATTERN_SET_PHY_REPEATER(dp_phy) \
        DP_LTTPR_REG(dp_phy, DP_TRAINING_PATTERN_SET_PHY_REPEATER1)
#endif

#ifndef DP_FEC_STATUS_PHY_REPEATER

#define __DP_FEC1_BASE                                      0xf0290 /* 1.4 */
#define __DP_FEC2_BASE                                      0xf0298 /* 1.4 */
#define DP_FEC_BASE(dp_phy) \
        (__DP_FEC1_BASE + ((__DP_FEC2_BASE - __DP_FEC1_BASE) * \
                           ((dp_phy) - DP_PHY_LTTPR1)))
#define DP_FEC_REG(dp_phy, fec1_reg) \
        (DP_FEC_BASE(dp_phy) - DP_FEC_BASE(DP_PHY_LTTPR1) + fec1_reg)
#define DP_FEC_STATUS_PHY_REPEATER1                         0xf0290 /* 1.4 */
#define DP_FEC_STATUS_PHY_REPEATER(dp_phy) \
        DP_FEC_REG(dp_phy, DP_FEC_STATUS_PHY_REPEATER1)
#define DP_LTTPR_MAX_ADD                                    0xf02ff /* 1.4 */
#define DP_DPCD_MAX_ADD                                     0xfffff /* 1.4 */

#endif

/*
 * v5.10-rc2-482-gce32a6239de6
 * drm/dp_helper: Add Helpers for FRL Link Training support for DP-HDMI2.1 PCON
 */
#ifndef DP_PCON_HDMI_POST_FRL_STATUS 

/* PCON CONFIGURE-1 FRL FOR HDMI SINK */
#define DP_PCON_HDMI_LINK_CONFIG_1             0x305A
# define DP_PCON_ENABLE_MAX_FRL_BW             (7 << 0)
# define DP_PCON_ENABLE_MAX_BW_0GBPS	       0
# define DP_PCON_ENABLE_MAX_BW_9GBPS	       1
# define DP_PCON_ENABLE_MAX_BW_18GBPS	       2
# define DP_PCON_ENABLE_MAX_BW_24GBPS	       3
# define DP_PCON_ENABLE_MAX_BW_32GBPS	       4
# define DP_PCON_ENABLE_MAX_BW_40GBPS	       5
# define DP_PCON_ENABLE_MAX_BW_48GBPS	       6
# define DP_PCON_ENABLE_SOURCE_CTL_MODE       (1 << 3)
# define DP_PCON_ENABLE_CONCURRENT_LINK       (1 << 4)
# define DP_PCON_ENABLE_SEQUENTIAL_LINK       (0 << 4)
# define DP_PCON_ENABLE_LINK_FRL_MODE         (1 << 5)
# define DP_PCON_ENABLE_HPD_READY	      (1 << 6)
# define DP_PCON_ENABLE_HDMI_LINK             (1 << 7)

/* PCON CONFIGURE-2 FRL FOR HDMI SINK */
#define DP_PCON_HDMI_LINK_CONFIG_2            0x305B
# define DP_PCON_MAX_LINK_BW_MASK             (0x3F << 0)
# define DP_PCON_FRL_BW_MASK_9GBPS            (1 << 0)
# define DP_PCON_FRL_BW_MASK_18GBPS           (1 << 1)
# define DP_PCON_FRL_BW_MASK_24GBPS           (1 << 2)
# define DP_PCON_FRL_BW_MASK_32GBPS           (1 << 3)
# define DP_PCON_FRL_BW_MASK_40GBPS           (1 << 4)
# define DP_PCON_FRL_BW_MASK_48GBPS           (1 << 5)
# define DP_PCON_FRL_LINK_TRAIN_EXTENDED      (1 << 6)
# define DP_PCON_FRL_LINK_TRAIN_NORMAL        (0 << 6)

/* PCON HDMI LINK STATUS */
#define DP_PCON_HDMI_TX_LINK_STATUS           0x303B
# define DP_PCON_HDMI_TX_LINK_ACTIVE          (1 << 0)
# define DP_PCON_FRL_READY		      (1 << 1)

/* PCON HDMI POST FRL STATUS */
#define DP_PCON_HDMI_POST_FRL_STATUS          0x3036
# define DP_PCON_HDMI_LINK_MODE               (1 << 0)
# define DP_PCON_HDMI_MODE_TMDS               0
# define DP_PCON_HDMI_MODE_FRL                1
# define DP_PCON_HDMI_FRL_TRAINED_BW          (0x3F << 1)
# define DP_PCON_FRL_TRAINED_BW_9GBPS	      (1 << 1)
# define DP_PCON_FRL_TRAINED_BW_18GBPS	      (1 << 2)
# define DP_PCON_FRL_TRAINED_BW_24GBPS	      (1 << 3)
# define DP_PCON_FRL_TRAINED_BW_32GBPS	      (1 << 4)
# define DP_PCON_FRL_TRAINED_BW_40GBPS	      (1 << 5)
# define DP_PCON_FRL_TRAINED_BW_48GBPS	      (1 << 6)
#endif
/*
 * v4.16-rc7-1860-g0597017cd18d
 * drm/dp: Add DP_DPCD_REV_XX to drm_dp_helper
 */

/* DPCD Field Address Mapping */

/* Receiver Capability */
#ifndef DP_DPCD_REV_14
# define DP_DPCD_REV_10                     0x10
# define DP_DPCD_REV_11                     0x11
# define DP_DPCD_REV_12                     0x12
# define DP_DPCD_REV_13                     0x13
# define DP_DPCD_REV_14                     0x14
#endif

#endif /* _KCL_DRM_DP_HELPER_H_ */
