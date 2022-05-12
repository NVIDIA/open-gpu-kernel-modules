/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _RTL819XU_HTTYPE_H_
#define _RTL819XU_HTTYPE_H_

/*
 * The HT Capability element is present in beacons, association request,
 * reassociation request and probe response frames
 */

/*
 * MIMO Power Save Settings
 */
#define MIMO_PS_STATIC				0

/*
 * There should be 128 bits to cover all of the MCS rates. However, since
 * 8190 does not support too much rates, one integer is quite enough.
 */
#define HTCLNG	4

/*
 * Represent Channel Width in HT Capabilities
 */
enum ht_channel_width {
	HT_CHANNEL_WIDTH_20 = 0,
	HT_CHANNEL_WIDTH_20_40 = 1,
};

/*
 * Represent Extension Channel Offset in HT Capabilities
 * This is available only in 40Mhz mode.
 */
enum ht_extension_chan_offset {
	HT_EXTCHNL_OFFSET_NO_EXT = 0,
	HT_EXTCHNL_OFFSET_UPPER = 1,
	HT_EXTCHNL_OFFSET_NO_DEF = 2,
	HT_EXTCHNL_OFFSET_LOWER = 3,
};

struct ht_capability_ele {
	//HT capability info
	u8	AdvCoding:1;
	u8	ChlWidth:1;
	u8	MimoPwrSave:2;
	u8	GreenField:1;
	u8	ShortGI20Mhz:1;
	u8	ShortGI40Mhz:1;
	u8	TxSTBC:1;
	u8	RxSTBC:2;
	u8	DelayBA:1;
	u8	MaxAMSDUSize:1;
	u8	DssCCk:1;
	u8	PSMP:1;
	u8	Rsvd1:1;
	u8	LSigTxopProtect:1;

	//MAC HT parameters info
	u8	MaxRxAMPDUFactor:2;
	u8	MPDUDensity:3;
	u8	Rsvd2:3;

	//Supported MCS set
	u8	MCS[16];

	//Extended HT Capability Info
	u16	ExtHTCapInfo;

	//TXBF Capabilities
	u8	TxBFCap[4];

	//Antenna Selection Capabilities
	u8	ASCap;

} __packed;

/*
 * The HT Information element is present in beacons
 * Only AP is required to include this element
 */
typedef struct _HT_INFORMATION_ELE {
	u8	ControlChl;

	u8	ExtChlOffset:2;
	u8	RecommemdedTxWidth:1;
	u8	RIFS:1;
	u8	PSMPAccessOnly:1;
	u8	SrvIntGranularity:3;

	u8	OptMode:2;
	u8	NonGFDevPresent:1;
	u8	Revd1:5;
	u8	Revd2:8;

	u8	Rsvd3:6;
	u8	DualBeacon:1;
	u8	DualCTSProtect:1;

	u8	SecondaryBeacon:1;
	u8	LSigTxopProtectFull:1;
	u8	PcoActive:1;
	u8	PcoPhase:1;
	u8	Rsvd4:4;

	u8	BasicMSC[16];
} __attribute__ ((packed)) HT_INFORMATION_ELE, *PHT_INFORMATION_ELE;

typedef enum _HT_SPEC_VER {
	HT_SPEC_VER_IEEE = 0,
	HT_SPEC_VER_EWC = 1,
} HT_SPEC_VER, *PHT_SPEC_VER;

typedef enum _HT_AGGRE_MODE_E {
	HT_AGG_AUTO = 0,
	HT_AGG_FORCE_ENABLE = 1,
	HT_AGG_FORCE_DISABLE = 2,
} HT_AGGRE_MODE_E, *PHT_AGGRE_MODE_E;

/*
 *  The Data structure is used to keep HT related variables when card is
 *  configured as non-AP STA mode.  **Note**  Current_xxx should be set
 *  to default value in HTInitializeHTInfo()
 */
typedef struct _RT_HIGH_THROUGHPUT {
	u8				bEnableHT;
	u8				bCurrentHTSupport;

	u8				bRegBW40MHz;				// Tx 40MHz channel capability
	u8				bCurBW40MHz;				// Tx 40MHz channel capability

	u8				bRegShortGI40MHz;			// Tx Short GI for 40Mhz
	u8				bCurShortGI40MHz;			// Tx Short GI for 40MHz

	u8				bRegShortGI20MHz;			// Tx Short GI for 20MHz
	u8				bCurShortGI20MHz;			// Tx Short GI for 20MHz

	u8				bRegSuppCCK;				// Tx CCK rate capability
	u8				bCurSuppCCK;				// Tx CCK rate capability

	// 802.11n spec version for "peer"
	HT_SPEC_VER			ePeerHTSpecVer;

	// HT related information for "Self"
	struct ht_capability_ele	SelfHTCap;		// This is HT cap element sent to peer STA, which also indicate HT Rx capabilities.
	HT_INFORMATION_ELE	SelfHTInfo;		// This is HT info element sent to peer STA, which also indicate HT Rx capabilities.

	// HT related information for "Peer"
	u8				PeerHTCapBuf[32];
	u8				PeerHTInfoBuf[32];

	// A-MSDU related
	u8				bAMSDU_Support;			// This indicates Tx A-MSDU capability
	u16				nAMSDU_MaxSize;			// This indicates Tx A-MSDU capability
	u8				bCurrent_AMSDU_Support;	// This indicates Tx A-MSDU capability
	u16				nCurrent_AMSDU_MaxSize;	// This indicates Tx A-MSDU capability

	// AMPDU  related <2006.08.10 Emily>
	u8				bAMPDUEnable;				// This indicate Tx A-MPDU capability
	u8				bCurrentAMPDUEnable;		// This indicate Tx A-MPDU capability
	u8				AMPDU_Factor;				// This indicate Tx A-MPDU capability
	u8				CurrentAMPDUFactor;		// This indicate Tx A-MPDU capability
	u8				MPDU_Density;				// This indicate Tx A-MPDU capability
	u8				CurrentMPDUDensity;			// This indicate Tx A-MPDU capability

	// Forced A-MPDU enable
	HT_AGGRE_MODE_E	ForcedAMPDUMode;
	u8				ForcedAMPDUFactor;
	u8				ForcedMPDUDensity;

	// Forced A-MSDU enable
	HT_AGGRE_MODE_E	ForcedAMSDUMode;
	u16				ForcedAMSDUMaxSize;

	u8				bForcedShortGI;

	u8				CurrentOpMode;

	// MIMO PS related
	u8				SelfMimoPs;
	u8				PeerMimoPs;

	// 40MHz Channel Offset settings.
	enum ht_extension_chan_offset	CurSTAExtChnlOffset;
	u8				bCurTxBW40MHz;	// If we use 40 MHz to Tx
	u8				PeerBandwidth;

	// For Bandwidth Switching
	u8				bSwBwInProgress;
	u8				SwBwStep;
	//struct timer_list		SwBwTimer;  //moved to ieee80211_device. as timer_list need include some header file here.

	// For Realtek proprietary A-MPDU factor for aggregation
	u8				bRegRT2RTAggregation;
	u8				bCurrentRT2RTAggregation;
	u8				bCurrentRT2RTLongSlotTime;
	u8				szRT2RTAggBuffer[10];

	// Rx Reorder control
	u8				bRegRxReorderEnable;
	u8				bCurRxReorderEnable;
	u8				RxReorderWinSize;
	u8				RxReorderPendingTime;
	u16				RxReorderDropCounter;

#ifdef USB_TX_DRIVER_AGGREGATION_ENABLE
	u8				UsbTxAggrNum;
#endif
#ifdef USB_RX_AGGREGATION_SUPPORT
	u8				UsbRxFwAggrEn;
	u8				UsbRxFwAggrPageNum;
	u8				UsbRxFwAggrPacketNum;
	u8				UsbRxFwAggrTimeout;
#endif

	// Add for Broadcom(Linksys) IOT. Joseph
	u8				bIsPeerBcm;

	// For IOT issue.
	u8				IOTPeer;
	u32				IOTAction;
} __attribute__ ((packed)) RT_HIGH_THROUGHPUT, *PRT_HIGH_THROUGHPUT;

/*
 * The Data structure is used to keep HT related variable for "each AP"
 * when card is configured as "STA mode"
 */
typedef struct _BSS_HT {
	u8				bdSupportHT;

	// HT related elements
	u8					bdHTCapBuf[32];
	u16					bdHTCapLen;
	u8					bdHTInfoBuf[32];
	u16					bdHTInfoLen;

	HT_SPEC_VER				bdHTSpecVer;
	//struct ht_capability_ele              bdHTCapEle;
	//HT_INFORMATION_ELE		bdHTInfoEle;

	u8					bdRT2RTAggregation;
	u8					bdRT2RTLongSlotTime;
} __attribute__ ((packed)) BSS_HT, *PBSS_HT;

extern u8 MCS_FILTER_ALL[16];
extern u8 MCS_FILTER_1SS[16];

/*
 * 2007/07/11 MH Modify the macro. Becaus STA may link with a N-AP. If we set
 * STA in A/B/G mode and AP is still in N mode. The macro will be wrong. We have
 * to add a macro to judge wireless mode.
 */
#define PICK_RATE(_nLegacyRate, _nMcsRate)	\
		(_nMcsRate == 0) ? (_nLegacyRate & 0x7f) : (_nMcsRate)
/* 2007/07/12 MH We only define legacy and HT wireless mode now. */
#define	LEGACY_WIRELESS_MODE	IEEE_MODE_MASK

#define CURRENT_RATE(WirelessMode, LegacyRate, HTRate)           \
		((WirelessMode & (LEGACY_WIRELESS_MODE)) != 0) ? \
			(LegacyRate) :                           \
			(PICK_RATE(LegacyRate, HTRate))

// MCS Bw 40 {1~7, 12~15,32}
#define	RATE_ADPT_1SS_MASK		0xFF
#define	RATE_ADPT_2SS_MASK		0xF0 //Skip MCS8~11 because mcs7 > mcs6, 9, 10, 11. 2007.01.16 by Emily
#define	RATE_ADPT_MCS32_MASK		0x01

#define		IS_11N_MCS_RATE(rate)		(rate & 0x80)

typedef enum _HT_AGGRE_SIZE {
	HT_AGG_SIZE_8K = 0,
	HT_AGG_SIZE_16K = 1,
	HT_AGG_SIZE_32K = 2,
	HT_AGG_SIZE_64K = 3,
} HT_AGGRE_SIZE_E, *PHT_AGGRE_SIZE_E;

/* Indicate different AP vendor for IOT issue */
typedef enum _HT_IOT_PEER {
	HT_IOT_PEER_UNKNOWN = 0,
	HT_IOT_PEER_REALTEK = 1,
	HT_IOT_PEER_BROADCOM = 2,
	HT_IOT_PEER_RALINK = 3,
	HT_IOT_PEER_ATHEROS = 4,
	HT_IOT_PEER_CISCO = 5,
	HT_IOT_PEER_MAX = 6
} HT_IOT_PEER_E, *PHTIOT_PEER_E;

/*
 * IOT Action for different AP
 */
typedef enum _HT_IOT_ACTION {
	HT_IOT_ACT_TX_USE_AMSDU_4K = 0x00000001,
	HT_IOT_ACT_TX_USE_AMSDU_8K = 0x00000002,
	HT_IOT_ACT_DISABLE_MCS14 = 0x00000004,
	HT_IOT_ACT_DISABLE_MCS15 = 0x00000008,
	HT_IOT_ACT_DISABLE_ALL_2SS = 0x00000010,
	HT_IOT_ACT_DISABLE_EDCA_TURBO = 0x00000020,
	HT_IOT_ACT_MGNT_USE_CCK_6M = 0x00000040,
	HT_IOT_ACT_CDD_FSYNC = 0x00000080,
	HT_IOT_ACT_PURE_N_MODE = 0x00000100,
	HT_IOT_ACT_FORCED_CTS2SELF = 0x00000200,
} HT_IOT_ACTION_E, *PHT_IOT_ACTION_E;

#endif //_RTL819XU_HTTYPE_H_
