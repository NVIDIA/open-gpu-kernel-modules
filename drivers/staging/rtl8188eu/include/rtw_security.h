/* SPDX-License-Identifier: GPL-2.0 */
/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#ifndef __RTW_SECURITY_H_
#define __RTW_SECURITY_H_

#include <osdep_service.h>
#include <drv_types.h>

#define _NO_PRIVACY_			0x0
#define _WEP40_				0x1
#define _TKIP_				0x2
#define _TKIP_WTMIC_			0x3
#define _AES_				0x4
#define _WEP104_			0x5
#define _WEP_WPA_MIXED_			0x07  /*  WEP + WPA */
#define _SMS4_				0x06

#define is_wep_enc(alg) (((alg) == _WEP40_) || ((alg) == _WEP104_))

#define SHA256_MAC_LEN 32
#define AES_BLOCK_SIZE 16
#define AES_PRIV_SIZE (4 * 44)

enum {
	ENCRYP_PROTOCOL_OPENSYS,   /* open system */
	ENCRYP_PROTOCOL_WEP,       /* WEP */
	ENCRYP_PROTOCOL_WPA,       /* WPA */
	ENCRYP_PROTOCOL_WPA2,      /* WPA2 */
	ENCRYP_PROTOCOL_WAPI,      /* WAPI: Not support in this version */
	ENCRYP_PROTOCOL_MAX
};

#ifndef Ndis802_11AuthModeWPA2
#define Ndis802_11AuthModeWPA2 (Ndis802_11AuthModeWPANone + 1)
#endif

#ifndef Ndis802_11AuthModeWPA2PSK
#define Ndis802_11AuthModeWPA2PSK (Ndis802_11AuthModeWPANone + 2)
#endif

union pn48	{
	u64	val;

#ifdef __LITTLE_ENDIAN
	struct {
		u8 TSC0;
		u8 TSC1;
		u8 TSC2;
		u8 TSC3;
		u8 TSC4;
		u8 TSC5;
		u8 TSC6;
		u8 TSC7;
	} _byte_;

#elif defined(__BIG_ENDIAN)

	struct {
		u8 TSC7;
		u8 TSC6;
		u8 TSC5;
		u8 TSC4;
		u8 TSC3;
		u8 TSC2;
		u8 TSC1;
		u8 TSC0;
	} _byte_;
#endif
};

union Keytype {
	u8   skey[16];
	u32    lkey[4];
};

struct rt_pmkid_list {
	u8	used;
	u8	bssid[ETH_ALEN];
	u8	PMKID[16];
	u8	SsidBuf[33];
	u8	*ssid_octet;
	u16	ssid_length;
};

struct security_priv {
	u32	  dot11AuthAlgrthm;	/*  802.11 auth, could be open,
					 * shared, 8021x and authswitch
					 */
	u32	  dot11PrivacyAlgrthm;	/*  This specify the privacy for
					 * shared auth. algorithm.
					 */
	/* WEP */
	u32	  dot11PrivacyKeyIndex;	/*  this is only valid for legendary
					 * wep, 0~3 for key id.(tx key index)
					 */
	union Keytype dot11DefKey[4];	/*  this is only valid for def. key */
	u32	dot11DefKeylen[4];
	u32 dot118021XGrpPrivacy;	/*  This specify the privacy algthm.
					 * used for Grp key
					 */
	u32	dot118021XGrpKeyid;	/*  key id used for Grp Key
					 * ( tx key index)
					 */
	union Keytype	dot118021XGrpKey[4];	/*  802.1x Group Key,
						 * for inx0 and inx1
						 */
	union Keytype	dot118021XGrptxmickey[4];
	union Keytype	dot118021XGrprxmickey[4];
	union pn48	dot11Grptxpn;		/* PN48 used for Grp Key xmit.*/
	union pn48	dot11Grprxpn;		/* PN48 used for Grp Key recv.*/
#ifdef CONFIG_88EU_AP_MODE
	/* extend security capabilities for AP_MODE */
	unsigned int dot8021xalg;/* 0:disable, 1:psk, 2:802.1x */
	unsigned int wpa_psk;/* 0:disable, bit(0): WPA, bit(1):WPA2 */
	unsigned int wpa_group_cipher;
	unsigned int wpa2_group_cipher;
	unsigned int wpa_pairwise_cipher;
	unsigned int wpa2_pairwise_cipher;
#endif
	u8 wps_ie[MAX_WPS_IE_LEN];/* added in assoc req */
	int wps_ie_len;
	u8	binstallGrpkey;
	u8	busetkipkey;
	u8	bcheck_grpkey;
	u8	bgrpkey_handshake;
	s32	hw_decrypted;/* if the rx packets is hw_decrypted==false,i
			      * it means the hw has not been ready.
			      */

	/* keeps the auth_type & enc_status from upper layer
	 * ioctl(wpa_supplicant or wzc)
	 */
	u32 ndisauthtype;	/*  NDIS_802_11_AUTHENTICATION_MODE */
	u32 ndisencryptstatus;	/*  NDIS_802_11_ENCRYPTION_STATUS */
	struct wlan_bssid_ex sec_bss;  /* for joinbss (h2c buffer) usage */
	struct ndis_802_11_wep ndiswep;
	u8 assoc_info[600];
	u8 szofcapability[256]; /* for wpa2 usage */
	u8 oidassociation[512]; /* for wpa/wpa2 usage */
	u8 authenticator_ie[256];  /* store ap security information element */
	u8 supplicant_ie[256];  /* store sta security information element */

	/* for tkip countermeasure */
	u32 last_mic_err_time;
	u8	btkip_countermeasure;
	u8	btkip_wait_report;
	u32 btkip_countermeasure_time;

	/*  */
	/*  For WPA2 Pre-Authentication. */
	/*  */
	struct rt_pmkid_list PMKIDList[NUM_PMKID_CACHE];
	u8	PMKIDIndex;
	u8 bWepDefaultKeyIdxSet;
};

#define GET_ENCRY_ALGO(psecuritypriv, psta, encry_algo, bmcst)		\
do {									\
	switch (psecuritypriv->dot11AuthAlgrthm) {			\
	case dot11AuthAlgrthm_Open:					\
	case dot11AuthAlgrthm_Shared:					\
	case dot11AuthAlgrthm_Auto:					\
		encry_algo = (u8)psecuritypriv->dot11PrivacyAlgrthm;	\
		break;							\
	case dot11AuthAlgrthm_8021X:					\
		if (bmcst)						\
			encry_algo = (u8)psecuritypriv->dot118021XGrpPrivacy;\
		else							\
			encry_algo = (u8)psta->dot118021XPrivacy;	\
		break;							\
	case dot11AuthAlgrthm_WAPI:					\
		encry_algo = (u8)psecuritypriv->dot11PrivacyAlgrthm;	\
		break;							\
	}								\
} while (0)

#define SET_ICE_IV_LEN(iv_len, icv_len, encrypt)			\
do {									\
	switch (encrypt) {						\
	case _WEP40_:							\
	case _WEP104_:							\
		iv_len = 4;						\
		icv_len = 4;						\
		break;							\
	case _TKIP_:							\
		iv_len = 8;						\
		icv_len = 4;						\
		break;							\
	case _AES_:							\
		iv_len = 8;						\
		icv_len = 8;						\
		break;							\
	case _SMS4_:							\
		iv_len = 18;						\
		icv_len = 16;						\
		break;							\
	default:							\
		iv_len = 0;						\
		icv_len = 0;						\
		break;							\
	}								\
} while (0)

#define GET_TKIP_PN(iv, dot11txpn)					\
do {									\
	dot11txpn._byte_.TSC0 = iv[2];					\
	dot11txpn._byte_.TSC1 = iv[0];					\
	dot11txpn._byte_.TSC2 = iv[4];					\
	dot11txpn._byte_.TSC3 = iv[5];					\
	dot11txpn._byte_.TSC4 = iv[6];					\
	dot11txpn._byte_.TSC5 = iv[7];					\
} while (0)

#define ROL32(A, n)	(((A) << (n)) | (((A) >> (32 - (n)))  & ((1UL << (n)) - 1)))
#define ROR32(A, n)	ROL32((A), 32 - (n))

struct mic_data {
	u32  K0, K1;         /*  Key */
	u32  L, R;           /*  Current state */
	u32  M;              /*  Message accumulator (single word) */
	u32  nBytesInM;      /*  # bytes in M */
};

void rtw_secmicsetkey(struct mic_data *pmicdata, u8 *key);
void rtw_secmicappendbyte(struct mic_data *pmicdata, u8 b);
void rtw_secmicappend(struct mic_data *pmicdata, u8 *src, u32 nBytes);
void rtw_secgetmic(struct mic_data *pmicdata, u8 *dst);
void rtw_seccalctkipmic(u8 *key, u8 *header, u8 *data, u32 data_len,
			u8 *Miccode, u8   priority);
u32 rtw_aes_encrypt(struct adapter *padapter, struct xmit_frame *pxmitframe);
u32 rtw_tkip_encrypt(struct adapter *padapter, struct xmit_frame *pxmitframe);
void rtw_wep_encrypt(struct adapter *padapter, struct xmit_frame *pxmitframe);
u32 rtw_aes_decrypt(struct adapter *padapter, struct recv_frame *precvframe);
u32 rtw_tkip_decrypt(struct adapter *padapter, struct recv_frame *precvframe);
int rtw_wep_decrypt(struct adapter *padapter, struct recv_frame *precvframe);

#endif	/* __RTL871X_SECURITY_H_ */
