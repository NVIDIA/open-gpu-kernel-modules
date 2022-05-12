/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright(c) 1999 - 2008 Intel Corporation. */

#ifndef _IXGB_EE_H_
#define _IXGB_EE_H_

#define IXGB_EEPROM_SIZE    64	/* Size in words */

/* EEPROM Commands */
#define EEPROM_READ_OPCODE  0x6	/* EEPROM read opcode */
#define EEPROM_WRITE_OPCODE 0x5	/* EEPROM write opcode */
#define EEPROM_ERASE_OPCODE 0x7	/* EEPROM erase opcode */
#define EEPROM_EWEN_OPCODE  0x13	/* EEPROM erase/write enable */
#define EEPROM_EWDS_OPCODE  0x10	/* EEPROM erase/write disable */

/* EEPROM MAP (Word Offsets) */
#define EEPROM_IA_1_2_REG        0x0000
#define EEPROM_IA_3_4_REG        0x0001
#define EEPROM_IA_5_6_REG        0x0002
#define EEPROM_COMPATIBILITY_REG 0x0003
#define EEPROM_PBA_1_2_REG       0x0008
#define EEPROM_PBA_3_4_REG       0x0009
#define EEPROM_INIT_CONTROL1_REG 0x000A
#define EEPROM_SUBSYS_ID_REG     0x000B
#define EEPROM_SUBVEND_ID_REG    0x000C
#define EEPROM_DEVICE_ID_REG     0x000D
#define EEPROM_VENDOR_ID_REG     0x000E
#define EEPROM_INIT_CONTROL2_REG 0x000F
#define EEPROM_SWDPINS_REG       0x0020
#define EEPROM_CIRCUIT_CTRL_REG  0x0021
#define EEPROM_D0_D3_POWER_REG   0x0022
#define EEPROM_FLASH_VERSION     0x0032
#define EEPROM_CHECKSUM_REG      0x003F

/* Mask bits for fields in Word 0x0a of the EEPROM */

#define EEPROM_ICW1_SIGNATURE_MASK  0xC000
#define EEPROM_ICW1_SIGNATURE_VALID 0x4000
#define EEPROM_ICW1_SIGNATURE_CLEAR 0x0000

/* For checksumming, the sum of all words in the EEPROM should equal 0xBABA. */
#define EEPROM_SUM 0xBABA

/* EEPROM Map Sizes (Byte Counts) */
#define PBA_SIZE 4

/* EEPROM Map defines (WORD OFFSETS)*/

/* EEPROM structure */
struct ixgb_ee_map_type {
	u8 mac_addr[ETH_ALEN];
	__le16 compatibility;
	__le16 reserved1[4];
	__le32 pba_number;
	__le16 init_ctrl_reg_1;
	__le16 subsystem_id;
	__le16 subvendor_id;
	__le16 device_id;
	__le16 vendor_id;
	__le16 init_ctrl_reg_2;
	__le16 oem_reserved[16];
	__le16 swdpins_reg;
	__le16 circuit_ctrl_reg;
	u8 d3_power;
	u8 d0_power;
	__le16 reserved2[28];
	__le16 checksum;
};

/* EEPROM Functions */
u16 ixgb_read_eeprom(struct ixgb_hw *hw, u16 reg);

bool ixgb_validate_eeprom_checksum(struct ixgb_hw *hw);

void ixgb_update_eeprom_checksum(struct ixgb_hw *hw);

void ixgb_write_eeprom(struct ixgb_hw *hw, u16 reg, u16 data);

#endif				/* IXGB_EE_H */
