/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
	Copyright (C) 2004 - 2009 Ivo van Doorn <IvDoorn@gmail.com>
	<http://rt2x00.serialmonkey.com>

 */

/*
	Module: rt73usb
	Abstract: Data structures and registers for the rt73usb module.
	Supported chipsets: rt2571W & rt2671.
 */

#ifndef RT73USB_H
#define RT73USB_H

/*
 * RF chip defines.
 */
#define RF5226				0x0001
#define RF2528				0x0002
#define RF5225				0x0003
#define RF2527				0x0004

/*
 * Signal information.
 * Default offset is required for RSSI <-> dBm conversion.
 */
#define DEFAULT_RSSI_OFFSET		120

/*
 * Register layout information.
 */
#define CSR_REG_BASE			0x3000
#define CSR_REG_SIZE			0x04b0
#define EEPROM_BASE			0x0000
#define EEPROM_SIZE			0x0100
#define BBP_BASE			0x0000
#define BBP_SIZE			0x0080
#define RF_BASE				0x0004
#define RF_SIZE				0x0010

/*
 * Number of TX queues.
 */
#define NUM_TX_QUEUES			4

/*
 * USB registers.
 */

/*
 * MCU_LEDCS: LED control for MCU Mailbox.
 */
#define MCU_LEDCS_LED_MODE		FIELD16(0x001f)
#define MCU_LEDCS_RADIO_STATUS		FIELD16(0x0020)
#define MCU_LEDCS_LINK_BG_STATUS	FIELD16(0x0040)
#define MCU_LEDCS_LINK_A_STATUS		FIELD16(0x0080)
#define MCU_LEDCS_POLARITY_GPIO_0	FIELD16(0x0100)
#define MCU_LEDCS_POLARITY_GPIO_1	FIELD16(0x0200)
#define MCU_LEDCS_POLARITY_GPIO_2	FIELD16(0x0400)
#define MCU_LEDCS_POLARITY_GPIO_3	FIELD16(0x0800)
#define MCU_LEDCS_POLARITY_GPIO_4	FIELD16(0x1000)
#define MCU_LEDCS_POLARITY_ACT		FIELD16(0x2000)
#define MCU_LEDCS_POLARITY_READY_BG	FIELD16(0x4000)
#define MCU_LEDCS_POLARITY_READY_A	FIELD16(0x8000)

/*
 * 8051 firmware image.
 */
#define FIRMWARE_RT2571			"rt73.bin"
#define FIRMWARE_IMAGE_BASE		0x0800

/*
 * Security key table memory.
 * 16 entries 32-byte for shared key table
 * 64 entries 32-byte for pairwise key table
 * 64 entries 8-byte for pairwise ta key table
 */
#define SHARED_KEY_TABLE_BASE		0x1000
#define PAIRWISE_KEY_TABLE_BASE		0x1200
#define PAIRWISE_TA_TABLE_BASE		0x1a00

#define SHARED_KEY_ENTRY(__idx) \
	( SHARED_KEY_TABLE_BASE + \
		((__idx) * sizeof(struct hw_key_entry)) )
#define PAIRWISE_KEY_ENTRY(__idx) \
	( PAIRWISE_KEY_TABLE_BASE + \
		((__idx) * sizeof(struct hw_key_entry)) )
#define PAIRWISE_TA_ENTRY(__idx) \
	( PAIRWISE_TA_TABLE_BASE + \
		((__idx) * sizeof(struct hw_pairwise_ta_entry)) )

struct hw_key_entry {
	u8 key[16];
	u8 tx_mic[8];
	u8 rx_mic[8];
} __packed;

struct hw_pairwise_ta_entry {
	u8 address[6];
	u8 cipher;
	u8 reserved;
} __packed;

/*
 * Since NULL frame won't be that long (256 byte),
 * We steal 16 tail bytes to save debugging settings.
 */
#define HW_DEBUG_SETTING_BASE		0x2bf0

/*
 * On-chip BEACON frame space.
 */
#define HW_BEACON_BASE0			0x2400
#define HW_BEACON_BASE1			0x2500
#define HW_BEACON_BASE2			0x2600
#define HW_BEACON_BASE3			0x2700

#define HW_BEACON_OFFSET(__index) \
	( HW_BEACON_BASE0 + (__index * 0x0100) )

/*
 * MAC Control/Status Registers(CSR).
 * Some values are set in TU, whereas 1 TU == 1024 us.
 */

/*
 * MAC_CSR0: ASIC revision number.
 */
#define MAC_CSR0			0x3000
#define MAC_CSR0_REVISION		FIELD32(0x0000000f)
#define MAC_CSR0_CHIPSET		FIELD32(0x000ffff0)

/*
 * MAC_CSR1: System control register.
 * SOFT_RESET: Software reset bit, 1: reset, 0: normal.
 * BBP_RESET: Hardware reset BBP.
 * HOST_READY: Host is ready after initialization, 1: ready.
 */
#define MAC_CSR1			0x3004
#define MAC_CSR1_SOFT_RESET		FIELD32(0x00000001)
#define MAC_CSR1_BBP_RESET		FIELD32(0x00000002)
#define MAC_CSR1_HOST_READY		FIELD32(0x00000004)

/*
 * MAC_CSR2: STA MAC register 0.
 */
#define MAC_CSR2			0x3008
#define MAC_CSR2_BYTE0			FIELD32(0x000000ff)
#define MAC_CSR2_BYTE1			FIELD32(0x0000ff00)
#define MAC_CSR2_BYTE2			FIELD32(0x00ff0000)
#define MAC_CSR2_BYTE3			FIELD32(0xff000000)

/*
 * MAC_CSR3: STA MAC register 1.
 * UNICAST_TO_ME_MASK:
 *	Used to mask off bits from byte 5 of the MAC address
 *	to determine the UNICAST_TO_ME bit for RX frames.
 *	The full mask is complemented by BSS_ID_MASK:
 *		MASK = BSS_ID_MASK & UNICAST_TO_ME_MASK
 */
#define MAC_CSR3			0x300c
#define MAC_CSR3_BYTE4			FIELD32(0x000000ff)
#define MAC_CSR3_BYTE5			FIELD32(0x0000ff00)
#define MAC_CSR3_UNICAST_TO_ME_MASK	FIELD32(0x00ff0000)

/*
 * MAC_CSR4: BSSID register 0.
 */
#define MAC_CSR4			0x3010
#define MAC_CSR4_BYTE0			FIELD32(0x000000ff)
#define MAC_CSR4_BYTE1			FIELD32(0x0000ff00)
#define MAC_CSR4_BYTE2			FIELD32(0x00ff0000)
#define MAC_CSR4_BYTE3			FIELD32(0xff000000)

/*
 * MAC_CSR5: BSSID register 1.
 * BSS_ID_MASK:
 *	This mask is used to mask off bits 0 and 1 of byte 5 of the
 *	BSSID. This will make sure that those bits will be ignored
 *	when determining the MY_BSS of RX frames.
 *		0: 1-BSSID mode (BSS index = 0)
 *		1: 2-BSSID mode (BSS index: Byte5, bit 0)
 *		2: 2-BSSID mode (BSS index: byte5, bit 1)
 *		3: 4-BSSID mode (BSS index: byte5, bit 0 - 1)
 */
#define MAC_CSR5			0x3014
#define MAC_CSR5_BYTE4			FIELD32(0x000000ff)
#define MAC_CSR5_BYTE5			FIELD32(0x0000ff00)
#define MAC_CSR5_BSS_ID_MASK		FIELD32(0x00ff0000)

/*
 * MAC_CSR6: Maximum frame length register.
 */
#define MAC_CSR6			0x3018
#define MAC_CSR6_MAX_FRAME_UNIT		FIELD32(0x00000fff)

/*
 * MAC_CSR7: Reserved
 */
#define MAC_CSR7			0x301c

/*
 * MAC_CSR8: SIFS/EIFS register.
 * All units are in US.
 */
#define MAC_CSR8			0x3020
#define MAC_CSR8_SIFS			FIELD32(0x000000ff)
#define MAC_CSR8_SIFS_AFTER_RX_OFDM	FIELD32(0x0000ff00)
#define MAC_CSR8_EIFS			FIELD32(0xffff0000)

/*
 * MAC_CSR9: Back-Off control register.
 * SLOT_TIME: Slot time, default is 20us for 802.11BG.
 * CWMIN: Bit for Cwmin. default Cwmin is 31 (2^5 - 1).
 * CWMAX: Bit for Cwmax, default Cwmax is 1023 (2^10 - 1).
 * CW_SELECT: 1: CWmin/Cwmax select from register, 0:select from TxD.
 */
#define MAC_CSR9			0x3024
#define MAC_CSR9_SLOT_TIME		FIELD32(0x000000ff)
#define MAC_CSR9_CWMIN			FIELD32(0x00000f00)
#define MAC_CSR9_CWMAX			FIELD32(0x0000f000)
#define MAC_CSR9_CW_SELECT		FIELD32(0x00010000)

/*
 * MAC_CSR10: Power state configuration.
 */
#define MAC_CSR10			0x3028

/*
 * MAC_CSR11: Power saving transition time register.
 * DELAY_AFTER_TBCN: Delay after Tbcn expired in units of TU.
 * TBCN_BEFORE_WAKEUP: Number of beacon before wakeup.
 * WAKEUP_LATENCY: In unit of TU.
 */
#define MAC_CSR11			0x302c
#define MAC_CSR11_DELAY_AFTER_TBCN	FIELD32(0x000000ff)
#define MAC_CSR11_TBCN_BEFORE_WAKEUP	FIELD32(0x00007f00)
#define MAC_CSR11_AUTOWAKE		FIELD32(0x00008000)
#define MAC_CSR11_WAKEUP_LATENCY	FIELD32(0x000f0000)

/*
 * MAC_CSR12: Manual power control / status register (merge CSR20 & PWRCSR1).
 * CURRENT_STATE: 0:sleep, 1:awake.
 * FORCE_WAKEUP: This has higher priority than PUT_TO_SLEEP.
 * BBP_CURRENT_STATE: 0: BBP sleep, 1: BBP awake.
 */
#define MAC_CSR12			0x3030
#define MAC_CSR12_CURRENT_STATE		FIELD32(0x00000001)
#define MAC_CSR12_PUT_TO_SLEEP		FIELD32(0x00000002)
#define MAC_CSR12_FORCE_WAKEUP		FIELD32(0x00000004)
#define MAC_CSR12_BBP_CURRENT_STATE	FIELD32(0x00000008)

/*
 * MAC_CSR13: GPIO.
 *	MAC_CSR13_VALx: GPIO value
 *	MAC_CSR13_DIRx: GPIO direction: 0 = input; 1 = output
 */
#define MAC_CSR13			0x3034
#define MAC_CSR13_VAL0			FIELD32(0x00000001)
#define MAC_CSR13_VAL1			FIELD32(0x00000002)
#define MAC_CSR13_VAL2			FIELD32(0x00000004)
#define MAC_CSR13_VAL3			FIELD32(0x00000008)
#define MAC_CSR13_VAL4			FIELD32(0x00000010)
#define MAC_CSR13_VAL5			FIELD32(0x00000020)
#define MAC_CSR13_VAL6			FIELD32(0x00000040)
#define MAC_CSR13_VAL7			FIELD32(0x00000080)
#define MAC_CSR13_DIR0			FIELD32(0x00000100)
#define MAC_CSR13_DIR1			FIELD32(0x00000200)
#define MAC_CSR13_DIR2			FIELD32(0x00000400)
#define MAC_CSR13_DIR3			FIELD32(0x00000800)
#define MAC_CSR13_DIR4			FIELD32(0x00001000)
#define MAC_CSR13_DIR5			FIELD32(0x00002000)
#define MAC_CSR13_DIR6			FIELD32(0x00004000)
#define MAC_CSR13_DIR7			FIELD32(0x00008000)

/*
 * MAC_CSR14: LED control register.
 * ON_PERIOD: On period, default 70ms.
 * OFF_PERIOD: Off period, default 30ms.
 * HW_LED: HW TX activity, 1: normal OFF, 0: normal ON.
 * SW_LED: s/w LED, 1: ON, 0: OFF.
 * HW_LED_POLARITY: 0: active low, 1: active high.
 */
#define MAC_CSR14			0x3038
#define MAC_CSR14_ON_PERIOD		FIELD32(0x000000ff)
#define MAC_CSR14_OFF_PERIOD		FIELD32(0x0000ff00)
#define MAC_CSR14_HW_LED		FIELD32(0x00010000)
#define MAC_CSR14_SW_LED		FIELD32(0x00020000)
#define MAC_CSR14_HW_LED_POLARITY	FIELD32(0x00040000)
#define MAC_CSR14_SW_LED2		FIELD32(0x00080000)

/*
 * MAC_CSR15: NAV control.
 */
#define MAC_CSR15			0x303c

/*
 * TXRX control registers.
 * Some values are set in TU, whereas 1 TU == 1024 us.
 */

/*
 * TXRX_CSR0: TX/RX configuration register.
 * TSF_OFFSET: Default is 24.
 * AUTO_TX_SEQ: 1: ASIC auto replace sequence nr in outgoing frame.
 * DISABLE_RX: Disable Rx engine.
 * DROP_CRC: Drop CRC error.
 * DROP_PHYSICAL: Drop physical error.
 * DROP_CONTROL: Drop control frame.
 * DROP_NOT_TO_ME: Drop not to me unicast frame.
 * DROP_TO_DS: Drop fram ToDs bit is true.
 * DROP_VERSION_ERROR: Drop version error frame.
 * DROP_MULTICAST: Drop multicast frames.
 * DROP_BORADCAST: Drop broadcast frames.
 * DROP_ACK_CTS: Drop received ACK and CTS.
 */
#define TXRX_CSR0			0x3040
#define TXRX_CSR0_RX_ACK_TIMEOUT	FIELD32(0x000001ff)
#define TXRX_CSR0_TSF_OFFSET		FIELD32(0x00007e00)
#define TXRX_CSR0_AUTO_TX_SEQ		FIELD32(0x00008000)
#define TXRX_CSR0_DISABLE_RX		FIELD32(0x00010000)
#define TXRX_CSR0_DROP_CRC		FIELD32(0x00020000)
#define TXRX_CSR0_DROP_PHYSICAL		FIELD32(0x00040000)
#define TXRX_CSR0_DROP_CONTROL		FIELD32(0x00080000)
#define TXRX_CSR0_DROP_NOT_TO_ME	FIELD32(0x00100000)
#define TXRX_CSR0_DROP_TO_DS		FIELD32(0x00200000)
#define TXRX_CSR0_DROP_VERSION_ERROR	FIELD32(0x00400000)
#define TXRX_CSR0_DROP_MULTICAST	FIELD32(0x00800000)
#define TXRX_CSR0_DROP_BROADCAST	FIELD32(0x01000000)
#define TXRX_CSR0_DROP_ACK_CTS		FIELD32(0x02000000)
#define TXRX_CSR0_TX_WITHOUT_WAITING	FIELD32(0x04000000)

/*
 * TXRX_CSR1
 */
#define TXRX_CSR1			0x3044
#define TXRX_CSR1_BBP_ID0		FIELD32(0x0000007f)
#define TXRX_CSR1_BBP_ID0_VALID		FIELD32(0x00000080)
#define TXRX_CSR1_BBP_ID1		FIELD32(0x00007f00)
#define TXRX_CSR1_BBP_ID1_VALID		FIELD32(0x00008000)
#define TXRX_CSR1_BBP_ID2		FIELD32(0x007f0000)
#define TXRX_CSR1_BBP_ID2_VALID		FIELD32(0x00800000)
#define TXRX_CSR1_BBP_ID3		FIELD32(0x7f000000)
#define TXRX_CSR1_BBP_ID3_VALID		FIELD32(0x80000000)

/*
 * TXRX_CSR2
 */
#define TXRX_CSR2			0x3048
#define TXRX_CSR2_BBP_ID0		FIELD32(0x0000007f)
#define TXRX_CSR2_BBP_ID0_VALID		FIELD32(0x00000080)
#define TXRX_CSR2_BBP_ID1		FIELD32(0x00007f00)
#define TXRX_CSR2_BBP_ID1_VALID		FIELD32(0x00008000)
#define TXRX_CSR2_BBP_ID2		FIELD32(0x007f0000)
#define TXRX_CSR2_BBP_ID2_VALID		FIELD32(0x00800000)
#define TXRX_CSR2_BBP_ID3		FIELD32(0x7f000000)
#define TXRX_CSR2_BBP_ID3_VALID		FIELD32(0x80000000)

/*
 * TXRX_CSR3
 */
#define TXRX_CSR3			0x304c
#define TXRX_CSR3_BBP_ID0		FIELD32(0x0000007f)
#define TXRX_CSR3_BBP_ID0_VALID		FIELD32(0x00000080)
#define TXRX_CSR3_BBP_ID1		FIELD32(0x00007f00)
#define TXRX_CSR3_BBP_ID1_VALID		FIELD32(0x00008000)
#define TXRX_CSR3_BBP_ID2		FIELD32(0x007f0000)
#define TXRX_CSR3_BBP_ID2_VALID		FIELD32(0x00800000)
#define TXRX_CSR3_BBP_ID3		FIELD32(0x7f000000)
#define TXRX_CSR3_BBP_ID3_VALID		FIELD32(0x80000000)

/*
 * TXRX_CSR4: Auto-Responder/Tx-retry register.
 * AUTORESPOND_PREAMBLE: 0:long, 1:short preamble.
 * OFDM_TX_RATE_DOWN: 1:enable.
 * OFDM_TX_RATE_STEP: 0:1-step, 1: 2-step, 2:3-step, 3:4-step.
 * OFDM_TX_FALLBACK_CCK: 0: Fallback to OFDM 6M only, 1: Fallback to CCK 1M,2M.
 */
#define TXRX_CSR4			0x3050
#define TXRX_CSR4_TX_ACK_TIMEOUT	FIELD32(0x000000ff)
#define TXRX_CSR4_CNTL_ACK_POLICY	FIELD32(0x00000700)
#define TXRX_CSR4_ACK_CTS_PSM		FIELD32(0x00010000)
#define TXRX_CSR4_AUTORESPOND_ENABLE	FIELD32(0x00020000)
#define TXRX_CSR4_AUTORESPOND_PREAMBLE	FIELD32(0x00040000)
#define TXRX_CSR4_OFDM_TX_RATE_DOWN	FIELD32(0x00080000)
#define TXRX_CSR4_OFDM_TX_RATE_STEP	FIELD32(0x00300000)
#define TXRX_CSR4_OFDM_TX_FALLBACK_CCK	FIELD32(0x00400000)
#define TXRX_CSR4_LONG_RETRY_LIMIT	FIELD32(0x0f000000)
#define TXRX_CSR4_SHORT_RETRY_LIMIT	FIELD32(0xf0000000)

/*
 * TXRX_CSR5
 */
#define TXRX_CSR5			0x3054

/*
 * TXRX_CSR6: ACK/CTS payload consumed time
 */
#define TXRX_CSR6			0x3058

/*
 * TXRX_CSR7: OFDM ACK/CTS payload consumed time for 6/9/12/18 mbps.
 */
#define TXRX_CSR7			0x305c
#define TXRX_CSR7_ACK_CTS_6MBS		FIELD32(0x000000ff)
#define TXRX_CSR7_ACK_CTS_9MBS		FIELD32(0x0000ff00)
#define TXRX_CSR7_ACK_CTS_12MBS		FIELD32(0x00ff0000)
#define TXRX_CSR7_ACK_CTS_18MBS		FIELD32(0xff000000)

/*
 * TXRX_CSR8: OFDM ACK/CTS payload consumed time for 24/36/48/54 mbps.
 */
#define TXRX_CSR8			0x3060
#define TXRX_CSR8_ACK_CTS_24MBS		FIELD32(0x000000ff)
#define TXRX_CSR8_ACK_CTS_36MBS		FIELD32(0x0000ff00)
#define TXRX_CSR8_ACK_CTS_48MBS		FIELD32(0x00ff0000)
#define TXRX_CSR8_ACK_CTS_54MBS		FIELD32(0xff000000)

/*
 * TXRX_CSR9: Synchronization control register.
 * BEACON_INTERVAL: In unit of 1/16 TU.
 * TSF_TICKING: Enable TSF auto counting.
 * TSF_SYNC: Tsf sync, 0: disable, 1: infra, 2: ad-hoc/master mode.
 * BEACON_GEN: Enable beacon generator.
 */
#define TXRX_CSR9			0x3064
#define TXRX_CSR9_BEACON_INTERVAL	FIELD32(0x0000ffff)
#define TXRX_CSR9_TSF_TICKING		FIELD32(0x00010000)
#define TXRX_CSR9_TSF_SYNC		FIELD32(0x00060000)
#define TXRX_CSR9_TBTT_ENABLE		FIELD32(0x00080000)
#define TXRX_CSR9_BEACON_GEN		FIELD32(0x00100000)
#define TXRX_CSR9_TIMESTAMP_COMPENSATE	FIELD32(0xff000000)

/*
 * TXRX_CSR10: BEACON alignment.
 */
#define TXRX_CSR10			0x3068

/*
 * TXRX_CSR11: AES mask.
 */
#define TXRX_CSR11			0x306c

/*
 * TXRX_CSR12: TSF low 32.
 */
#define TXRX_CSR12			0x3070
#define TXRX_CSR12_LOW_TSFTIMER		FIELD32(0xffffffff)

/*
 * TXRX_CSR13: TSF high 32.
 */
#define TXRX_CSR13			0x3074
#define TXRX_CSR13_HIGH_TSFTIMER	FIELD32(0xffffffff)

/*
 * TXRX_CSR14: TBTT timer.
 */
#define TXRX_CSR14			0x3078

/*
 * TXRX_CSR15: TKIP MIC priority byte "AND" mask.
 */
#define TXRX_CSR15			0x307c

/*
 * PHY control registers.
 * Some values are set in TU, whereas 1 TU == 1024 us.
 */

/*
 * PHY_CSR0: RF/PS control.
 */
#define PHY_CSR0			0x3080
#define PHY_CSR0_PA_PE_BG		FIELD32(0x00010000)
#define PHY_CSR0_PA_PE_A		FIELD32(0x00020000)

/*
 * PHY_CSR1
 */
#define PHY_CSR1			0x3084
#define PHY_CSR1_RF_RPI			FIELD32(0x00010000)

/*
 * PHY_CSR2: Pre-TX BBP control.
 */
#define PHY_CSR2			0x3088

/*
 * PHY_CSR3: BBP serial control register.
 * VALUE: Register value to program into BBP.
 * REG_NUM: Selected BBP register.
 * READ_CONTROL: 0: Write BBP, 1: Read BBP.
 * BUSY: 1: ASIC is busy execute BBP programming.
 */
#define PHY_CSR3			0x308c
#define PHY_CSR3_VALUE			FIELD32(0x000000ff)
#define PHY_CSR3_REGNUM			FIELD32(0x00007f00)
#define PHY_CSR3_READ_CONTROL		FIELD32(0x00008000)
#define PHY_CSR3_BUSY			FIELD32(0x00010000)

/*
 * PHY_CSR4: RF serial control register
 * VALUE: Register value (include register id) serial out to RF/IF chip.
 * NUMBER_OF_BITS: Number of bits used in RFRegValue (I:20, RFMD:22).
 * IF_SELECT: 1: select IF to program, 0: select RF to program.
 * PLL_LD: RF PLL_LD status.
 * BUSY: 1: ASIC is busy execute RF programming.
 */
#define PHY_CSR4			0x3090
#define PHY_CSR4_VALUE			FIELD32(0x00ffffff)
#define PHY_CSR4_NUMBER_OF_BITS		FIELD32(0x1f000000)
#define PHY_CSR4_IF_SELECT		FIELD32(0x20000000)
#define PHY_CSR4_PLL_LD			FIELD32(0x40000000)
#define PHY_CSR4_BUSY			FIELD32(0x80000000)

/*
 * PHY_CSR5: RX to TX signal switch timing control.
 */
#define PHY_CSR5			0x3094
#define PHY_CSR5_IQ_FLIP		FIELD32(0x00000004)

/*
 * PHY_CSR6: TX to RX signal timing control.
 */
#define PHY_CSR6			0x3098
#define PHY_CSR6_IQ_FLIP		FIELD32(0x00000004)

/*
 * PHY_CSR7: TX DAC switching timing control.
 */
#define PHY_CSR7			0x309c

/*
 * Security control register.
 */

/*
 * SEC_CSR0: Shared key table control.
 */
#define SEC_CSR0			0x30a0
#define SEC_CSR0_BSS0_KEY0_VALID	FIELD32(0x00000001)
#define SEC_CSR0_BSS0_KEY1_VALID	FIELD32(0x00000002)
#define SEC_CSR0_BSS0_KEY2_VALID	FIELD32(0x00000004)
#define SEC_CSR0_BSS0_KEY3_VALID	FIELD32(0x00000008)
#define SEC_CSR0_BSS1_KEY0_VALID	FIELD32(0x00000010)
#define SEC_CSR0_BSS1_KEY1_VALID	FIELD32(0x00000020)
#define SEC_CSR0_BSS1_KEY2_VALID	FIELD32(0x00000040)
#define SEC_CSR0_BSS1_KEY3_VALID	FIELD32(0x00000080)
#define SEC_CSR0_BSS2_KEY0_VALID	FIELD32(0x00000100)
#define SEC_CSR0_BSS2_KEY1_VALID	FIELD32(0x00000200)
#define SEC_CSR0_BSS2_KEY2_VALID	FIELD32(0x00000400)
#define SEC_CSR0_BSS2_KEY3_VALID	FIELD32(0x00000800)
#define SEC_CSR0_BSS3_KEY0_VALID	FIELD32(0x00001000)
#define SEC_CSR0_BSS3_KEY1_VALID	FIELD32(0x00002000)
#define SEC_CSR0_BSS3_KEY2_VALID	FIELD32(0x00004000)
#define SEC_CSR0_BSS3_KEY3_VALID	FIELD32(0x00008000)

/*
 * SEC_CSR1: Shared key table security mode register.
 */
#define SEC_CSR1			0x30a4
#define SEC_CSR1_BSS0_KEY0_CIPHER_ALG	FIELD32(0x00000007)
#define SEC_CSR1_BSS0_KEY1_CIPHER_ALG	FIELD32(0x00000070)
#define SEC_CSR1_BSS0_KEY2_CIPHER_ALG	FIELD32(0x00000700)
#define SEC_CSR1_BSS0_KEY3_CIPHER_ALG	FIELD32(0x00007000)
#define SEC_CSR1_BSS1_KEY0_CIPHER_ALG	FIELD32(0x00070000)
#define SEC_CSR1_BSS1_KEY1_CIPHER_ALG	FIELD32(0x00700000)
#define SEC_CSR1_BSS1_KEY2_CIPHER_ALG	FIELD32(0x07000000)
#define SEC_CSR1_BSS1_KEY3_CIPHER_ALG	FIELD32(0x70000000)

/*
 * Pairwise key table valid bitmap registers.
 * SEC_CSR2: pairwise key table valid bitmap 0.
 * SEC_CSR3: pairwise key table valid bitmap 1.
 */
#define SEC_CSR2			0x30a8
#define SEC_CSR3			0x30ac

/*
 * SEC_CSR4: Pairwise key table lookup control.
 */
#define SEC_CSR4			0x30b0
#define SEC_CSR4_ENABLE_BSS0		FIELD32(0x00000001)
#define SEC_CSR4_ENABLE_BSS1		FIELD32(0x00000002)
#define SEC_CSR4_ENABLE_BSS2		FIELD32(0x00000004)
#define SEC_CSR4_ENABLE_BSS3		FIELD32(0x00000008)

/*
 * SEC_CSR5: shared key table security mode register.
 */
#define SEC_CSR5			0x30b4
#define SEC_CSR5_BSS2_KEY0_CIPHER_ALG	FIELD32(0x00000007)
#define SEC_CSR5_BSS2_KEY1_CIPHER_ALG	FIELD32(0x00000070)
#define SEC_CSR5_BSS2_KEY2_CIPHER_ALG	FIELD32(0x00000700)
#define SEC_CSR5_BSS2_KEY3_CIPHER_ALG	FIELD32(0x00007000)
#define SEC_CSR5_BSS3_KEY0_CIPHER_ALG	FIELD32(0x00070000)
#define SEC_CSR5_BSS3_KEY1_CIPHER_ALG	FIELD32(0x00700000)
#define SEC_CSR5_BSS3_KEY2_CIPHER_ALG	FIELD32(0x07000000)
#define SEC_CSR5_BSS3_KEY3_CIPHER_ALG	FIELD32(0x70000000)

/*
 * STA control registers.
 */

/*
 * STA_CSR0: RX PLCP error count & RX FCS error count.
 */
#define STA_CSR0			0x30c0
#define STA_CSR0_FCS_ERROR		FIELD32(0x0000ffff)
#define STA_CSR0_PLCP_ERROR		FIELD32(0xffff0000)

/*
 * STA_CSR1: RX False CCA count & RX LONG frame count.
 */
#define STA_CSR1			0x30c4
#define STA_CSR1_PHYSICAL_ERROR		FIELD32(0x0000ffff)
#define STA_CSR1_FALSE_CCA_ERROR	FIELD32(0xffff0000)

/*
 * STA_CSR2: TX Beacon count and RX FIFO overflow count.
 */
#define STA_CSR2			0x30c8
#define STA_CSR2_RX_FIFO_OVERFLOW_COUNT	FIELD32(0x0000ffff)
#define STA_CSR2_RX_OVERFLOW_COUNT	FIELD32(0xffff0000)

/*
 * STA_CSR3: TX Beacon count.
 */
#define STA_CSR3			0x30cc
#define STA_CSR3_TX_BEACON_COUNT	FIELD32(0x0000ffff)

/*
 * STA_CSR4: TX Retry count.
 */
#define STA_CSR4			0x30d0
#define STA_CSR4_TX_NO_RETRY_COUNT	FIELD32(0x0000ffff)
#define STA_CSR4_TX_ONE_RETRY_COUNT	FIELD32(0xffff0000)

/*
 * STA_CSR5: TX Retry count.
 */
#define STA_CSR5			0x30d4
#define STA_CSR4_TX_MULTI_RETRY_COUNT	FIELD32(0x0000ffff)
#define STA_CSR4_TX_RETRY_FAIL_COUNT	FIELD32(0xffff0000)

/*
 * QOS control registers.
 */

/*
 * QOS_CSR1: TXOP holder MAC address register.
 */
#define QOS_CSR1			0x30e4
#define QOS_CSR1_BYTE4			FIELD32(0x000000ff)
#define QOS_CSR1_BYTE5			FIELD32(0x0000ff00)

/*
 * QOS_CSR2: TXOP holder timeout register.
 */
#define QOS_CSR2			0x30e8

/*
 * RX QOS-CFPOLL MAC address register.
 * QOS_CSR3: RX QOS-CFPOLL MAC address 0.
 * QOS_CSR4: RX QOS-CFPOLL MAC address 1.
 */
#define QOS_CSR3			0x30ec
#define QOS_CSR4			0x30f0

/*
 * QOS_CSR5: "QosControl" field of the RX QOS-CFPOLL.
 */
#define QOS_CSR5			0x30f4

/*
 * WMM Scheduler Register
 */

/*
 * AIFSN_CSR: AIFSN for each EDCA AC.
 * AIFSN0: For AC_VO.
 * AIFSN1: For AC_VI.
 * AIFSN2: For AC_BE.
 * AIFSN3: For AC_BK.
 */
#define AIFSN_CSR			0x0400
#define AIFSN_CSR_AIFSN0		FIELD32(0x0000000f)
#define AIFSN_CSR_AIFSN1		FIELD32(0x000000f0)
#define AIFSN_CSR_AIFSN2		FIELD32(0x00000f00)
#define AIFSN_CSR_AIFSN3		FIELD32(0x0000f000)

/*
 * CWMIN_CSR: CWmin for each EDCA AC.
 * CWMIN0: For AC_VO.
 * CWMIN1: For AC_VI.
 * CWMIN2: For AC_BE.
 * CWMIN3: For AC_BK.
 */
#define CWMIN_CSR			0x0404
#define CWMIN_CSR_CWMIN0		FIELD32(0x0000000f)
#define CWMIN_CSR_CWMIN1		FIELD32(0x000000f0)
#define CWMIN_CSR_CWMIN2		FIELD32(0x00000f00)
#define CWMIN_CSR_CWMIN3		FIELD32(0x0000f000)

/*
 * CWMAX_CSR: CWmax for each EDCA AC.
 * CWMAX0: For AC_VO.
 * CWMAX1: For AC_VI.
 * CWMAX2: For AC_BE.
 * CWMAX3: For AC_BK.
 */
#define CWMAX_CSR			0x0408
#define CWMAX_CSR_CWMAX0		FIELD32(0x0000000f)
#define CWMAX_CSR_CWMAX1		FIELD32(0x000000f0)
#define CWMAX_CSR_CWMAX2		FIELD32(0x00000f00)
#define CWMAX_CSR_CWMAX3		FIELD32(0x0000f000)

/*
 * AC_TXOP_CSR0: AC_VO/AC_VI TXOP register.
 * AC0_TX_OP: For AC_VO, in unit of 32us.
 * AC1_TX_OP: For AC_VI, in unit of 32us.
 */
#define AC_TXOP_CSR0			0x040c
#define AC_TXOP_CSR0_AC0_TX_OP		FIELD32(0x0000ffff)
#define AC_TXOP_CSR0_AC1_TX_OP		FIELD32(0xffff0000)

/*
 * AC_TXOP_CSR1: AC_BE/AC_BK TXOP register.
 * AC2_TX_OP: For AC_BE, in unit of 32us.
 * AC3_TX_OP: For AC_BK, in unit of 32us.
 */
#define AC_TXOP_CSR1			0x0410
#define AC_TXOP_CSR1_AC2_TX_OP		FIELD32(0x0000ffff)
#define AC_TXOP_CSR1_AC3_TX_OP		FIELD32(0xffff0000)

/*
 * BBP registers.
 * The wordsize of the BBP is 8 bits.
 */

/*
 * R2
 */
#define BBP_R2_BG_MODE			FIELD8(0x20)

/*
 * R3
 */
#define BBP_R3_SMART_MODE		FIELD8(0x01)

/*
 * R4: RX antenna control
 * FRAME_END: 1 - DPDT, 0 - SPDT (Only valid for 802.11G, RF2527 & RF2529)
 */

/*
 * ANTENNA_CONTROL semantics (guessed):
 * 0x1: Software controlled antenna switching (fixed or SW diversity)
 * 0x2: Hardware diversity.
 */
#define BBP_R4_RX_ANTENNA_CONTROL	FIELD8(0x03)
#define BBP_R4_RX_FRAME_END		FIELD8(0x20)

/*
 * R77
 */
#define BBP_R77_RX_ANTENNA		FIELD8(0x03)

/*
 * RF registers
 */

/*
 * RF 3
 */
#define RF3_TXPOWER			FIELD32(0x00003e00)

/*
 * RF 4
 */
#define RF4_FREQ_OFFSET			FIELD32(0x0003f000)

/*
 * EEPROM content.
 * The wordsize of the EEPROM is 16 bits.
 */

/*
 * HW MAC address.
 */
#define EEPROM_MAC_ADDR_0		0x0002
#define EEPROM_MAC_ADDR_BYTE0		FIELD16(0x00ff)
#define EEPROM_MAC_ADDR_BYTE1		FIELD16(0xff00)
#define EEPROM_MAC_ADDR1		0x0003
#define EEPROM_MAC_ADDR_BYTE2		FIELD16(0x00ff)
#define EEPROM_MAC_ADDR_BYTE3		FIELD16(0xff00)
#define EEPROM_MAC_ADDR_2		0x0004
#define EEPROM_MAC_ADDR_BYTE4		FIELD16(0x00ff)
#define EEPROM_MAC_ADDR_BYTE5		FIELD16(0xff00)

/*
 * EEPROM antenna.
 * ANTENNA_NUM: Number of antennas.
 * TX_DEFAULT: Default antenna 0: diversity, 1: A, 2: B.
 * RX_DEFAULT: Default antenna 0: diversity, 1: A, 2: B.
 * FRAME_TYPE: 0: DPDT , 1: SPDT , noted this bit is valid for g only.
 * DYN_TXAGC: Dynamic TX AGC control.
 * HARDWARE_RADIO: 1: Hardware controlled radio. Read GPIO0.
 * RF_TYPE: Rf_type of this adapter.
 */
#define EEPROM_ANTENNA			0x0010
#define EEPROM_ANTENNA_NUM		FIELD16(0x0003)
#define EEPROM_ANTENNA_TX_DEFAULT	FIELD16(0x000c)
#define EEPROM_ANTENNA_RX_DEFAULT	FIELD16(0x0030)
#define EEPROM_ANTENNA_FRAME_TYPE	FIELD16(0x0040)
#define EEPROM_ANTENNA_DYN_TXAGC	FIELD16(0x0200)
#define EEPROM_ANTENNA_HARDWARE_RADIO	FIELD16(0x0400)
#define EEPROM_ANTENNA_RF_TYPE		FIELD16(0xf800)

/*
 * EEPROM NIC config.
 * EXTERNAL_LNA: External LNA.
 */
#define EEPROM_NIC			0x0011
#define EEPROM_NIC_EXTERNAL_LNA		FIELD16(0x0010)

/*
 * EEPROM geography.
 * GEO_A: Default geographical setting for 5GHz band
 * GEO: Default geographical setting.
 */
#define EEPROM_GEOGRAPHY		0x0012
#define EEPROM_GEOGRAPHY_GEO_A		FIELD16(0x00ff)
#define EEPROM_GEOGRAPHY_GEO		FIELD16(0xff00)

/*
 * EEPROM BBP.
 */
#define EEPROM_BBP_START		0x0013
#define EEPROM_BBP_SIZE			16
#define EEPROM_BBP_VALUE		FIELD16(0x00ff)
#define EEPROM_BBP_REG_ID		FIELD16(0xff00)

/*
 * EEPROM TXPOWER 802.11G
 */
#define EEPROM_TXPOWER_G_START		0x0023
#define EEPROM_TXPOWER_G_SIZE		7
#define EEPROM_TXPOWER_G_1		FIELD16(0x00ff)
#define EEPROM_TXPOWER_G_2		FIELD16(0xff00)

/*
 * EEPROM Frequency
 */
#define EEPROM_FREQ			0x002f
#define EEPROM_FREQ_OFFSET		FIELD16(0x00ff)
#define EEPROM_FREQ_SEQ_MASK		FIELD16(0xff00)
#define EEPROM_FREQ_SEQ			FIELD16(0x0300)

/*
 * EEPROM LED.
 * POLARITY_RDY_G: Polarity RDY_G setting.
 * POLARITY_RDY_A: Polarity RDY_A setting.
 * POLARITY_ACT: Polarity ACT setting.
 * POLARITY_GPIO_0: Polarity GPIO0 setting.
 * POLARITY_GPIO_1: Polarity GPIO1 setting.
 * POLARITY_GPIO_2: Polarity GPIO2 setting.
 * POLARITY_GPIO_3: Polarity GPIO3 setting.
 * POLARITY_GPIO_4: Polarity GPIO4 setting.
 * LED_MODE: Led mode.
 */
#define EEPROM_LED			0x0030
#define EEPROM_LED_POLARITY_RDY_G	FIELD16(0x0001)
#define EEPROM_LED_POLARITY_RDY_A	FIELD16(0x0002)
#define EEPROM_LED_POLARITY_ACT		FIELD16(0x0004)
#define EEPROM_LED_POLARITY_GPIO_0	FIELD16(0x0008)
#define EEPROM_LED_POLARITY_GPIO_1	FIELD16(0x0010)
#define EEPROM_LED_POLARITY_GPIO_2	FIELD16(0x0020)
#define EEPROM_LED_POLARITY_GPIO_3	FIELD16(0x0040)
#define EEPROM_LED_POLARITY_GPIO_4	FIELD16(0x0080)
#define EEPROM_LED_LED_MODE		FIELD16(0x1f00)

/*
 * EEPROM TXPOWER 802.11A
 */
#define EEPROM_TXPOWER_A_START		0x0031
#define EEPROM_TXPOWER_A_SIZE		12
#define EEPROM_TXPOWER_A_1		FIELD16(0x00ff)
#define EEPROM_TXPOWER_A_2		FIELD16(0xff00)

/*
 * EEPROM RSSI offset 802.11BG
 */
#define EEPROM_RSSI_OFFSET_BG		0x004d
#define EEPROM_RSSI_OFFSET_BG_1		FIELD16(0x00ff)
#define EEPROM_RSSI_OFFSET_BG_2		FIELD16(0xff00)

/*
 * EEPROM RSSI offset 802.11A
 */
#define EEPROM_RSSI_OFFSET_A		0x004e
#define EEPROM_RSSI_OFFSET_A_1		FIELD16(0x00ff)
#define EEPROM_RSSI_OFFSET_A_2		FIELD16(0xff00)

/*
 * DMA descriptor defines.
 */
#define TXD_DESC_SIZE			( 6 * sizeof(__le32) )
#define TXINFO_SIZE			( 6 * sizeof(__le32) )
#define RXD_DESC_SIZE			( 6 * sizeof(__le32) )

/*
 * TX descriptor format for TX, PRIO and Beacon Ring.
 */

/*
 * Word0
 * BURST: Next frame belongs to same "burst" event.
 * TKIP_MIC: ASIC appends TKIP MIC if TKIP is used.
 * KEY_TABLE: Use per-client pairwise KEY table.
 * KEY_INDEX:
 * Key index (0~31) to the pairwise KEY table.
 * 0~3 to shared KEY table 0 (BSS0).
 * 4~7 to shared KEY table 1 (BSS1).
 * 8~11 to shared KEY table 2 (BSS2).
 * 12~15 to shared KEY table 3 (BSS3).
 * BURST2: For backward compatibility, set to same value as BURST.
 */
#define TXD_W0_BURST			FIELD32(0x00000001)
#define TXD_W0_VALID			FIELD32(0x00000002)
#define TXD_W0_MORE_FRAG		FIELD32(0x00000004)
#define TXD_W0_ACK			FIELD32(0x00000008)
#define TXD_W0_TIMESTAMP		FIELD32(0x00000010)
#define TXD_W0_OFDM			FIELD32(0x00000020)
#define TXD_W0_IFS			FIELD32(0x00000040)
#define TXD_W0_RETRY_MODE		FIELD32(0x00000080)
#define TXD_W0_TKIP_MIC			FIELD32(0x00000100)
#define TXD_W0_KEY_TABLE		FIELD32(0x00000200)
#define TXD_W0_KEY_INDEX		FIELD32(0x0000fc00)
#define TXD_W0_DATABYTE_COUNT		FIELD32(0x0fff0000)
#define TXD_W0_BURST2			FIELD32(0x10000000)
#define TXD_W0_CIPHER_ALG		FIELD32(0xe0000000)

/*
 * Word1
 * HOST_Q_ID: EDCA/HCCA queue ID.
 * HW_SEQUENCE: MAC overwrites the frame sequence number.
 * BUFFER_COUNT: Number of buffers in this TXD.
 */
#define TXD_W1_HOST_Q_ID		FIELD32(0x0000000f)
#define TXD_W1_AIFSN			FIELD32(0x000000f0)
#define TXD_W1_CWMIN			FIELD32(0x00000f00)
#define TXD_W1_CWMAX			FIELD32(0x0000f000)
#define TXD_W1_IV_OFFSET		FIELD32(0x003f0000)
#define TXD_W1_HW_SEQUENCE		FIELD32(0x10000000)
#define TXD_W1_BUFFER_COUNT		FIELD32(0xe0000000)

/*
 * Word2: PLCP information
 */
#define TXD_W2_PLCP_SIGNAL		FIELD32(0x000000ff)
#define TXD_W2_PLCP_SERVICE		FIELD32(0x0000ff00)
#define TXD_W2_PLCP_LENGTH_LOW		FIELD32(0x00ff0000)
#define TXD_W2_PLCP_LENGTH_HIGH		FIELD32(0xff000000)

/*
 * Word3
 */
#define TXD_W3_IV			FIELD32(0xffffffff)

/*
 * Word4
 */
#define TXD_W4_EIV			FIELD32(0xffffffff)

/*
 * Word5
 * FRAME_OFFSET: Frame start offset inside ASIC TXFIFO (after TXINFO field).
 * PACKET_ID: Driver assigned packet ID to categorize TXResult in interrupt.
 * WAITING_DMA_DONE_INT: TXD been filled with data
 * and waiting for TxDoneISR housekeeping.
 */
#define TXD_W5_FRAME_OFFSET		FIELD32(0x000000ff)
#define TXD_W5_PACKET_ID		FIELD32(0x0000ff00)
#define TXD_W5_TX_POWER			FIELD32(0x00ff0000)
#define TXD_W5_WAITING_DMA_DONE_INT	FIELD32(0x01000000)

/*
 * RX descriptor format for RX Ring.
 */

/*
 * Word0
 * CIPHER_ERROR: 1:ICV error, 2:MIC error, 3:invalid key.
 * KEY_INDEX: Decryption key actually used.
 */
#define RXD_W0_OWNER_NIC		FIELD32(0x00000001)
#define RXD_W0_DROP			FIELD32(0x00000002)
#define RXD_W0_UNICAST_TO_ME		FIELD32(0x00000004)
#define RXD_W0_MULTICAST		FIELD32(0x00000008)
#define RXD_W0_BROADCAST		FIELD32(0x00000010)
#define RXD_W0_MY_BSS			FIELD32(0x00000020)
#define RXD_W0_CRC_ERROR		FIELD32(0x00000040)
#define RXD_W0_OFDM			FIELD32(0x00000080)
#define RXD_W0_CIPHER_ERROR		FIELD32(0x00000300)
#define RXD_W0_KEY_INDEX		FIELD32(0x0000fc00)
#define RXD_W0_DATABYTE_COUNT		FIELD32(0x0fff0000)
#define RXD_W0_CIPHER_ALG		FIELD32(0xe0000000)

/*
 * WORD1
 * SIGNAL: RX raw data rate reported by BBP.
 * RSSI: RSSI reported by BBP.
 */
#define RXD_W1_SIGNAL			FIELD32(0x000000ff)
#define RXD_W1_RSSI_AGC			FIELD32(0x00001f00)
#define RXD_W1_RSSI_LNA			FIELD32(0x00006000)
#define RXD_W1_FRAME_OFFSET		FIELD32(0x7f000000)

/*
 * Word2
 * IV: Received IV of originally encrypted.
 */
#define RXD_W2_IV			FIELD32(0xffffffff)

/*
 * Word3
 * EIV: Received EIV of originally encrypted.
 */
#define RXD_W3_EIV			FIELD32(0xffffffff)

/*
 * Word4
 * ICV: Received ICV of originally encrypted.
 * NOTE: This is a guess, the official definition is "reserved"
 */
#define RXD_W4_ICV			FIELD32(0xffffffff)

/*
 * the above 20-byte is called RXINFO and will be DMAed to MAC RX block
 * and passed to the HOST driver.
 * The following fields are for DMA block and HOST usage only.
 * Can't be touched by ASIC MAC block.
 */

/*
 * Word5
 */
#define RXD_W5_RESERVED			FIELD32(0xffffffff)

/*
 * Macros for converting txpower from EEPROM to mac80211 value
 * and from mac80211 value to register value.
 */
#define MIN_TXPOWER	0
#define MAX_TXPOWER	31
#define DEFAULT_TXPOWER	24

#define TXPOWER_FROM_DEV(__txpower) \
	(((u8)(__txpower)) > MAX_TXPOWER) ? DEFAULT_TXPOWER : (__txpower)

#define TXPOWER_TO_DEV(__txpower) \
	clamp_t(char, __txpower, MIN_TXPOWER, MAX_TXPOWER)

#endif /* RT73USB_H */
