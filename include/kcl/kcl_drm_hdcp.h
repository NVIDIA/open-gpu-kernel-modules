/* SPDX-License-Identifier: MIT */
/*
 * Copyright (C) 2017 Google, Inc.
 *
 * Authors:
 * Sean Paul <seanpaul@chromium.org>
 */
#ifndef AMDKCL_DRM_HDCP_H
#define AMDKCL_DRM_HDCP_H

#ifdef CONFIG_DRM_AMD_DC_HDCP
#include <drm/drm_hdcp.h>
#include <kcl/kcl_drm_connector.h>

/* changed in v4.16-rc7-1717-gb8e47d87be65
 * drm: Fix HDCP downstream dev count read
 */
#ifdef DRM_HDCP_NUM_DOWNSTREAM
#undef DRM_HDCP_NUM_DOWNSTREAM
#define DRM_HDCP_NUM_DOWNSTREAM(x)             (x & 0x7f)
#endif

/* introduced in v5.3-rc1-377-g7672dbba85d3
 * drm: Add Content protection type property
 */
#ifndef DRM_MODE_HDCP_CONTENT_TYPE0
#define DRM_MODE_HDCP_CONTENT_TYPE0		0
#define DRM_MODE_HDCP_CONTENT_TYPE1		1
#endif

/* introduced in v4.19-rc2-1221-gaf5aad059885
 * drm: hdcp2.2 authentication msg definitions
 */
#ifndef DRM_HDCP_1_4_SRM_ID
#define DRM_HDCP_1_4_SRM_ID			0x8
#define DRM_HDCP_1_4_VRL_LENGTH_SIZE		3
#define DRM_HDCP_1_4_DCP_SIG_SIZE		40

/* Protocol message definition for HDCP2.2 specification */
/*
 * Protected content streams are classified into 2 types:
 * - Type0: Can be transmitted with HDCP 1.4+
 * - Type1: Can be transmitted with HDCP 2.2+
 */
#define HDCP_STREAM_TYPE0			0x00
#define HDCP_STREAM_TYPE1			0x01

/* introduced in v4.15-rc4-1351-g495eb7f877ab
 * drm: Add some HDCP related #defines
 */
#ifndef DRM_HDCP_KSV_LEN
#define DRM_HDCP_KSV_LEN			5
#endif

/* HDCP2.2 Msg IDs */
#define HDCP_2_2_NULL_MSG			1
#define HDCP_2_2_AKE_INIT			2
#define HDCP_2_2_AKE_SEND_CERT			3
#define HDCP_2_2_AKE_NO_STORED_KM		4
#define HDCP_2_2_AKE_STORED_KM			5
#define HDCP_2_2_AKE_SEND_HPRIME		7
#define HDCP_2_2_AKE_SEND_PAIRING_INFO		8
#define HDCP_2_2_LC_INIT			9
#define HDCP_2_2_LC_SEND_LPRIME			10
#define HDCP_2_2_SKE_SEND_EKS			11
#define HDCP_2_2_REP_SEND_RECVID_LIST		12
#define HDCP_2_2_REP_SEND_ACK			15
#define HDCP_2_2_REP_STREAM_MANAGE		16
#define HDCP_2_2_REP_STREAM_READY		17
#define HDCP_2_2_ERRATA_DP_STREAM_TYPE		50

#define HDCP_2_2_RTX_LEN			8
#define HDCP_2_2_RRX_LEN			8

#define HDCP_2_2_K_PUB_RX_MOD_N_LEN		128
#define HDCP_2_2_K_PUB_RX_EXP_E_LEN		3
#define HDCP_2_2_K_PUB_RX_LEN			(HDCP_2_2_K_PUB_RX_MOD_N_LEN + \
						 HDCP_2_2_K_PUB_RX_EXP_E_LEN)

#define HDCP_2_2_DCP_LLC_SIG_LEN		384

#define HDCP_2_2_E_KPUB_KM_LEN			128
#define HDCP_2_2_E_KH_KM_M_LEN			(16 + 16)
#define HDCP_2_2_H_PRIME_LEN			32
#define HDCP_2_2_E_KH_KM_LEN			16
#define HDCP_2_2_RN_LEN				8
#define HDCP_2_2_L_PRIME_LEN			32
#define HDCP_2_2_E_DKEY_KS_LEN			16
#define HDCP_2_2_RIV_LEN			8
#define HDCP_2_2_SEQ_NUM_LEN			3
#define HDCP_2_2_V_PRIME_HALF_LEN		(HDCP_2_2_L_PRIME_LEN / 2)
#define HDCP_2_2_RECEIVER_ID_LEN		DRM_HDCP_KSV_LEN
#define HDCP_2_2_MAX_DEVICE_COUNT		31
#define HDCP_2_2_RECEIVER_IDS_MAX_LEN		(HDCP_2_2_RECEIVER_ID_LEN * \
						 HDCP_2_2_MAX_DEVICE_COUNT)
#define HDCP_2_2_MPRIME_LEN			32

/* Following Macros take a byte at a time for bit(s) masking */
/*
 * TODO: This has to be changed for DP MST, as multiple stream on
 * same port is possible.
 * For HDCP2.2 on HDMI and DP SST this value is always 1.
 */
#define HDCP_2_2_MAX_CONTENT_STREAMS_CNT	1
#define HDCP_2_2_TXCAP_MASK_LEN			2
#define HDCP_2_2_RXCAPS_LEN			3
#define HDCP_2_2_RX_REPEATER(x)			((x) & BIT(0))
#define HDCP_2_2_DP_HDCP_CAPABLE(x)		((x) & BIT(1))
#define HDCP_2_2_RXINFO_LEN			2

/* HDCP1.x compliant device in downstream */
#define HDCP_2_2_HDCP1_DEVICE_CONNECTED(x)	((x) & BIT(0))

/* HDCP2.0 Compliant repeater in downstream */
#define HDCP_2_2_HDCP_2_0_REP_CONNECTED(x)	((x) & BIT(1))
#define HDCP_2_2_MAX_CASCADE_EXCEEDED(x)	((x) & BIT(2))
#define HDCP_2_2_MAX_DEVS_EXCEEDED(x)		((x) & BIT(3))
#define HDCP_2_2_DEV_COUNT_LO(x)		(((x) & (0xF << 4)) >> 4)
#define HDCP_2_2_DEV_COUNT_HI(x)		((x) & BIT(0))
#define HDCP_2_2_DEPTH(x)			(((x) & (0x7 << 1)) >> 1)

struct hdcp2_cert_rx {
	u8	receiver_id[HDCP_2_2_RECEIVER_ID_LEN];
	u8	kpub_rx[HDCP_2_2_K_PUB_RX_LEN];
	u8	reserved[2];
	u8	dcp_signature[HDCP_2_2_DCP_LLC_SIG_LEN];
} __packed;

struct hdcp2_streamid_type {
	u8	stream_id;
	u8	stream_type;
} __packed;

/*
 * The TxCaps field specified in the HDCP HDMI, DP specs
 * This field is big endian as specified in the errata.
 */
struct hdcp2_tx_caps {
	/* Transmitter must set this to 0x2 */
	u8	version;

	/* Reserved for HDCP and DP Spec. Read as Zero */
	u8	tx_cap_mask[HDCP_2_2_TXCAP_MASK_LEN];
} __packed;

/* Main structures for HDCP2.2 protocol communication */
struct hdcp2_ake_init {
	u8			msg_id;
	u8			r_tx[HDCP_2_2_RTX_LEN];
	struct hdcp2_tx_caps	tx_caps;
} __packed;

struct hdcp2_ake_send_cert {
	u8			msg_id;
	struct hdcp2_cert_rx	cert_rx;
	u8			r_rx[HDCP_2_2_RRX_LEN];
	u8			rx_caps[HDCP_2_2_RXCAPS_LEN];
} __packed;

struct hdcp2_ake_no_stored_km {
	u8	msg_id;
	u8	e_kpub_km[HDCP_2_2_E_KPUB_KM_LEN];
} __packed;

struct hdcp2_ake_stored_km {
	u8	msg_id;
	u8	e_kh_km_m[HDCP_2_2_E_KH_KM_M_LEN];
} __packed;

struct hdcp2_ake_send_hprime {
	u8	msg_id;
	u8	h_prime[HDCP_2_2_H_PRIME_LEN];
} __packed;

struct hdcp2_ake_send_pairing_info {
	u8	msg_id;
	u8	e_kh_km[HDCP_2_2_E_KH_KM_LEN];
} __packed;

struct hdcp2_lc_init {
	u8	msg_id;
	u8	r_n[HDCP_2_2_RN_LEN];
} __packed;

struct hdcp2_lc_send_lprime {
	u8	msg_id;
	u8	l_prime[HDCP_2_2_L_PRIME_LEN];
} __packed;

struct hdcp2_ske_send_eks {
	u8	msg_id;
	u8	e_dkey_ks[HDCP_2_2_E_DKEY_KS_LEN];
	u8	riv[HDCP_2_2_RIV_LEN];
} __packed;

struct hdcp2_rep_send_receiverid_list {
	u8	msg_id;
	u8	rx_info[HDCP_2_2_RXINFO_LEN];
	u8	seq_num_v[HDCP_2_2_SEQ_NUM_LEN];
	u8	v_prime[HDCP_2_2_V_PRIME_HALF_LEN];
	u8	receiver_ids[HDCP_2_2_RECEIVER_IDS_MAX_LEN];
} __packed;

struct hdcp2_rep_send_ack {
	u8	msg_id;
	u8	v[HDCP_2_2_V_PRIME_HALF_LEN];
} __packed;

struct hdcp2_rep_stream_manage {
	u8			msg_id;
	u8			seq_num_m[HDCP_2_2_SEQ_NUM_LEN];
	__be16			k;
	struct hdcp2_streamid_type streams[HDCP_2_2_MAX_CONTENT_STREAMS_CNT];
} __packed;

struct hdcp2_rep_stream_ready {
	u8	msg_id;
	u8	m_prime[HDCP_2_2_MPRIME_LEN];
} __packed;

struct hdcp2_dp_errata_stream_type {
	u8	msg_id;
	u8	stream_type;
} __packed;
#endif /* DRM_HDCP_1_4_SRM_ID */

/* introduced in v4.19-rc2-1222-g8b44fefee694
 * drm: HDMI and DP specific HDCP2.2 defines
 */
#ifndef HDCP_2_2_CERT_TIMEOUT_MS
/* HDCP2.2 TIMEOUTs in mSec */
#define HDCP_2_2_CERT_TIMEOUT_MS               100
#define HDCP_2_2_HPRIME_NO_PAIRED_TIMEOUT_MS   1000
#define HDCP_2_2_HPRIME_PAIRED_TIMEOUT_MS      200
#define HDCP_2_2_PAIRING_TIMEOUT_MS            200
#define        HDCP_2_2_HDMI_LPRIME_TIMEOUT_MS         20
#define HDCP_2_2_DP_LPRIME_TIMEOUT_MS          7
#define HDCP_2_2_RECVID_LIST_TIMEOUT_MS                3000
#define HDCP_2_2_STREAM_READY_TIMEOUT_MS       100

/* HDMI HDCP2.2 Register Offsets */
#define HDCP_2_2_HDMI_REG_VER_OFFSET           0x50
#define HDCP_2_2_HDMI_REG_WR_MSG_OFFSET                0x60
#define HDCP_2_2_HDMI_REG_RXSTATUS_OFFSET      0x70
#define HDCP_2_2_HDMI_REG_RD_MSG_OFFSET                0x80
#define HDCP_2_2_HDMI_REG_DBG_OFFSET           0xC0

#define HDCP_2_2_HDMI_SUPPORT_MASK             BIT(2)
#define HDCP_2_2_RX_CAPS_VERSION_VAL           0x02
#define HDCP_2_2_SEQ_NUM_MAX                   0xFFFFFF
#define        HDCP_2_2_DELAY_BEFORE_ENCRYPTION_EN     200

/* Below macros take a byte at a time and mask the bit(s) */
#define HDCP_2_2_HDMI_RXSTATUS_LEN             2
#define HDCP_2_2_HDMI_RXSTATUS_MSG_SZ_HI(x)    ((x) & 0x3)
#define HDCP_2_2_HDMI_RXSTATUS_READY(x)                ((x) & BIT(2))
#define HDCP_2_2_HDMI_RXSTATUS_REAUTH_REQ(x)   ((x) & BIT(3))
/* DP HDCP2.2 parameter offsets in DPCD address space */
#define DP_HDCP_2_2_REG_RTX_OFFSET             0x69000
#define DP_HDCP_2_2_REG_TXCAPS_OFFSET          0x69008
#define DP_HDCP_2_2_REG_CERT_RX_OFFSET         0x6900B
#define DP_HDCP_2_2_REG_RRX_OFFSET             0x69215
#define DP_HDCP_2_2_REG_RX_CAPS_OFFSET         0x6921D
#define DP_HDCP_2_2_REG_EKPUB_KM_OFFSET                0x69220
#define DP_HDCP_2_2_REG_EKH_KM_WR_OFFSET       0x692A0
#define DP_HDCP_2_2_REG_M_OFFSET               0x692B0
#define DP_HDCP_2_2_REG_HPRIME_OFFSET          0x692C0
#define DP_HDCP_2_2_REG_EKH_KM_RD_OFFSET       0x692E0
#define DP_HDCP_2_2_REG_RN_OFFSET              0x692F0
#define DP_HDCP_2_2_REG_LPRIME_OFFSET          0x692F8
#define DP_HDCP_2_2_REG_EDKEY_KS_OFFSET                0x69318
#define        DP_HDCP_2_2_REG_RIV_OFFSET              0x69328
#define DP_HDCP_2_2_REG_RXINFO_OFFSET          0x69330
#define DP_HDCP_2_2_REG_SEQ_NUM_V_OFFSET       0x69332
#define DP_HDCP_2_2_REG_VPRIME_OFFSET          0x69335
#define DP_HDCP_2_2_REG_RECV_ID_LIST_OFFSET    0x69345
#define DP_HDCP_2_2_REG_V_OFFSET               0x693E0
#define DP_HDCP_2_2_REG_SEQ_NUM_M_OFFSET       0x693F0
#define DP_HDCP_2_2_REG_K_OFFSET               0x693F3
#define DP_HDCP_2_2_REG_STREAM_ID_TYPE_OFFSET  0x693F5
#define DP_HDCP_2_2_REG_MPRIME_OFFSET          0x69473
#define DP_HDCP_2_2_REG_RXSTATUS_OFFSET                0x69493
#define DP_HDCP_2_2_REG_STREAM_TYPE_OFFSET     0x69494
#define DP_HDCP_2_2_REG_DBG_OFFSET             0x69518

/* DP HDCP message start offsets in DPCD address space */
#define DP_HDCP_2_2_AKE_INIT_OFFSET            DP_HDCP_2_2_REG_RTX_OFFSET
#define DP_HDCP_2_2_AKE_SEND_CERT_OFFSET       DP_HDCP_2_2_REG_CERT_RX_OFFSET
#define DP_HDCP_2_2_AKE_NO_STORED_KM_OFFSET    DP_HDCP_2_2_REG_EKPUB_KM_OFFSET
#define DP_HDCP_2_2_AKE_STORED_KM_OFFSET       DP_HDCP_2_2_REG_EKH_KM_WR_OFFSET
#define DP_HDCP_2_2_AKE_SEND_HPRIME_OFFSET     DP_HDCP_2_2_REG_HPRIME_OFFSET
#define DP_HDCP_2_2_AKE_SEND_PAIRING_INFO_OFFSET \
                                               DP_HDCP_2_2_REG_EKH_KM_RD_OFFSET
#define DP_HDCP_2_2_LC_INIT_OFFSET             DP_HDCP_2_2_REG_RN_OFFSET
#define DP_HDCP_2_2_LC_SEND_LPRIME_OFFSET      DP_HDCP_2_2_REG_LPRIME_OFFSET
#define DP_HDCP_2_2_SKE_SEND_EKS_OFFSET                DP_HDCP_2_2_REG_EDKEY_KS_OFFSET
#define DP_HDCP_2_2_REP_SEND_RECVID_LIST_OFFSET        DP_HDCP_2_2_REG_RXINFO_OFFSET
#define DP_HDCP_2_2_REP_SEND_ACK_OFFSET                DP_HDCP_2_2_REG_V_OFFSET
#define DP_HDCP_2_2_REP_STREAM_MANAGE_OFFSET   DP_HDCP_2_2_REG_SEQ_NUM_M_OFFSET
#define DP_HDCP_2_2_REP_STREAM_READY_OFFSET    DP_HDCP_2_2_REG_MPRIME_OFFSET

#define HDCP_2_2_DP_RXSTATUS_LEN               1
#define HDCP_2_2_DP_RXSTATUS_READY(x)          ((x) & BIT(0))
#define HDCP_2_2_DP_RXSTATUS_H_PRIME(x)                ((x) & BIT(1))
#define HDCP_2_2_DP_RXSTATUS_PAIRING(x)                ((x) & BIT(2))
#define HDCP_2_2_DP_RXSTATUS_REAUTH_REQ(x)     ((x) & BIT(3))
#define HDCP_2_2_DP_RXSTATUS_LINK_FAILED(x)    ((x) & BIT(4))
#endif /* HDCP_2_2_CERT_TIMEOUT_MS */

#ifndef HAVE_DRM_HDCP_UPDATE_CONTENT_PROTECTION
void _kcl_drm_hdcp_update_content_protection(struct drm_connector *connector,
							      u64 val);
static inline
void drm_hdcp_update_content_protection(struct drm_connector *connector,
					u64 val)
{
	_kcl_drm_hdcp_update_content_protection(connector, val);
}
#endif /* HAVE_DRM_HDCP_UPDATE_CONTENT_PROTECTION */

#endif /* CONFIG_DRM_AMD_DC_HDCP */

#endif /* AMDKCL_DRM_HDCP_H */
