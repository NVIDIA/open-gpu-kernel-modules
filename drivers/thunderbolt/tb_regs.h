/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Thunderbolt driver - Port/Switch config area registers
 *
 * Every thunderbolt device consists (logically) of a switch with multiple
 * ports. Every port contains up to four config regions (HOPS, PORT, SWITCH,
 * COUNTERS) which are used to configure the device.
 *
 * Copyright (c) 2014 Andreas Noever <andreas.noever@gmail.com>
 * Copyright (C) 2018, Intel Corporation
 */

#ifndef _TB_REGS
#define _TB_REGS

#include <linux/types.h>


#define TB_ROUTE_SHIFT 8  /* number of bits in a port entry of a route */


/*
 * TODO: should be 63? But we do not know how to receive frames larger than 256
 * bytes at the frame level. (header + checksum = 16, 60*4 = 240)
 */
#define TB_MAX_CONFIG_RW_LENGTH 60

enum tb_switch_cap {
	TB_SWITCH_CAP_TMU		= 0x03,
	TB_SWITCH_CAP_VSE		= 0x05,
};

enum tb_switch_vse_cap {
	TB_VSE_CAP_PLUG_EVENTS		= 0x01, /* also EEPROM */
	TB_VSE_CAP_TIME2		= 0x03,
	TB_VSE_CAP_IECS			= 0x04,
	TB_VSE_CAP_LINK_CONTROLLER	= 0x06, /* also IECS */
};

enum tb_port_cap {
	TB_PORT_CAP_PHY			= 0x01,
	TB_PORT_CAP_POWER		= 0x02,
	TB_PORT_CAP_TIME1		= 0x03,
	TB_PORT_CAP_ADAP		= 0x04,
	TB_PORT_CAP_VSE			= 0x05,
	TB_PORT_CAP_USB4		= 0x06,
};

enum tb_port_state {
	TB_PORT_DISABLED	= 0, /* tb_cap_phy.disable == 1 */
	TB_PORT_CONNECTING	= 1, /* retry */
	TB_PORT_UP		= 2,
	TB_PORT_UNPLUGGED	= 7,
};

/* capability headers */

struct tb_cap_basic {
	u8 next;
	/* enum tb_cap cap:8; prevent "narrower than values of its type" */
	u8 cap; /* if cap == 0x05 then we have a extended capability */
} __packed;

/**
 * struct tb_cap_extended_short - Switch extended short capability
 * @next: Pointer to the next capability. If @next and @length are zero
 *	  then we have a long cap.
 * @cap: Base capability ID (see &enum tb_switch_cap)
 * @vsec_id: Vendor specific capability ID (see &enum switch_vse_cap)
 * @length: Length of this capability
 */
struct tb_cap_extended_short {
	u8 next;
	u8 cap;
	u8 vsec_id;
	u8 length;
} __packed;

/**
 * struct tb_cap_extended_long - Switch extended long capability
 * @zero1: This field should be zero
 * @cap: Base capability ID (see &enum tb_switch_cap)
 * @vsec_id: Vendor specific capability ID (see &enum switch_vse_cap)
 * @zero2: This field should be zero
 * @next: Pointer to the next capability
 * @length: Length of this capability
 */
struct tb_cap_extended_long {
	u8 zero1;
	u8 cap;
	u8 vsec_id;
	u8 zero2;
	u16 next;
	u16 length;
} __packed;

/**
 * struct tb_cap_any - Structure capable of hold every capability
 * @basic: Basic capability
 * @extended_short: Vendor specific capability
 * @extended_long: Vendor specific extended capability
 */
struct tb_cap_any {
	union {
		struct tb_cap_basic basic;
		struct tb_cap_extended_short extended_short;
		struct tb_cap_extended_long extended_long;
	};
} __packed;

/* capabilities */

struct tb_cap_link_controller {
	struct tb_cap_extended_long cap_header;
	u32 count:4; /* number of link controllers */
	u32 unknown1:4;
	u32 base_offset:8; /*
			    * offset (into this capability) of the configuration
			    * area of the first link controller
			    */
	u32 length:12; /* link controller configuration area length */
	u32 unknown2:4; /* TODO check that length is correct */
} __packed;

struct tb_cap_phy {
	struct tb_cap_basic cap_header;
	u32 unknown1:16;
	u32 unknown2:14;
	bool disable:1;
	u32 unknown3:11;
	enum tb_port_state state:4;
	u32 unknown4:2;
} __packed;

struct tb_eeprom_ctl {
	bool clock:1; /* send pulse to transfer one bit */
	bool access_low:1; /* set to 0 before access */
	bool data_out:1; /* to eeprom */
	bool data_in:1; /* from eeprom */
	bool access_high:1; /* set to 1 before access */
	bool not_present:1; /* should be 0 */
	bool unknown1:1;
	bool present:1; /* should be 1 */
	u32 unknown2:24;
} __packed;

struct tb_cap_plug_events {
	struct tb_cap_extended_short cap_header;
	u32 __unknown1:2;
	u32 plug_events:5;
	u32 __unknown2:25;
	u32 __unknown3;
	u32 __unknown4;
	struct tb_eeprom_ctl eeprom_ctl;
	u32 __unknown5[7];
	u32 drom_offset; /* 32 bit register, but eeprom addresses are 16 bit */
} __packed;

/* device headers */

/* Present on port 0 in TB_CFG_SWITCH at address zero. */
struct tb_regs_switch_header {
	/* DWORD 0 */
	u16 vendor_id;
	u16 device_id;
	/* DWORD 1 */
	u32 first_cap_offset:8;
	u32 upstream_port_number:6;
	u32 max_port_number:6;
	u32 depth:3;
	u32 __unknown1:1;
	u32 revision:8;
	/* DWORD 2 */
	u32 route_lo;
	/* DWORD 3 */
	u32 route_hi:31;
	bool enabled:1;
	/* DWORD 4 */
	u32 plug_events_delay:8; /*
				  * RW, pause between plug events in
				  * milliseconds. Writing 0x00 is interpreted
				  * as 255ms.
				  */
	u32 cmuv:8;
	u32 __unknown4:8;
	u32 thunderbolt_version:8;
} __packed;

/* USB4 version 1.0 */
#define USB4_VERSION_1_0			0x20

#define ROUTER_CS_1				0x01
#define ROUTER_CS_4				0x04
#define ROUTER_CS_5				0x05
#define ROUTER_CS_5_SLP				BIT(0)
#define ROUTER_CS_5_WOP				BIT(1)
#define ROUTER_CS_5_WOU				BIT(2)
#define ROUTER_CS_5_C3S				BIT(23)
#define ROUTER_CS_5_PTO				BIT(24)
#define ROUTER_CS_5_UTO				BIT(25)
#define ROUTER_CS_5_HCO				BIT(26)
#define ROUTER_CS_5_CV				BIT(31)
#define ROUTER_CS_6				0x06
#define ROUTER_CS_6_SLPR			BIT(0)
#define ROUTER_CS_6_TNS				BIT(1)
#define ROUTER_CS_6_WOPS			BIT(2)
#define ROUTER_CS_6_WOUS			BIT(3)
#define ROUTER_CS_6_HCI				BIT(18)
#define ROUTER_CS_6_CR				BIT(25)
#define ROUTER_CS_7				0x07
#define ROUTER_CS_9				0x09
#define ROUTER_CS_25				0x19
#define ROUTER_CS_26				0x1a
#define ROUTER_CS_26_OPCODE_MASK		GENMASK(15, 0)
#define ROUTER_CS_26_STATUS_MASK		GENMASK(29, 24)
#define ROUTER_CS_26_STATUS_SHIFT		24
#define ROUTER_CS_26_ONS			BIT(30)
#define ROUTER_CS_26_OV				BIT(31)

/* USB4 router operations opcodes */
enum usb4_switch_op {
	USB4_SWITCH_OP_QUERY_DP_RESOURCE = 0x10,
	USB4_SWITCH_OP_ALLOC_DP_RESOURCE = 0x11,
	USB4_SWITCH_OP_DEALLOC_DP_RESOURCE = 0x12,
	USB4_SWITCH_OP_NVM_WRITE = 0x20,
	USB4_SWITCH_OP_NVM_AUTH = 0x21,
	USB4_SWITCH_OP_NVM_READ = 0x22,
	USB4_SWITCH_OP_NVM_SET_OFFSET = 0x23,
	USB4_SWITCH_OP_DROM_READ = 0x24,
	USB4_SWITCH_OP_NVM_SECTOR_SIZE = 0x25,
};

/* Router TMU configuration */
#define TMU_RTR_CS_0				0x00
#define TMU_RTR_CS_0_TD				BIT(27)
#define TMU_RTR_CS_0_UCAP			BIT(30)
#define TMU_RTR_CS_1				0x01
#define TMU_RTR_CS_1_LOCAL_TIME_NS_MASK		GENMASK(31, 16)
#define TMU_RTR_CS_1_LOCAL_TIME_NS_SHIFT	16
#define TMU_RTR_CS_2				0x02
#define TMU_RTR_CS_3				0x03
#define TMU_RTR_CS_3_LOCAL_TIME_NS_MASK		GENMASK(15, 0)
#define TMU_RTR_CS_3_TS_PACKET_INTERVAL_MASK	GENMASK(31, 16)
#define TMU_RTR_CS_3_TS_PACKET_INTERVAL_SHIFT	16
#define TMU_RTR_CS_22				0x16
#define TMU_RTR_CS_24				0x18

enum tb_port_type {
	TB_TYPE_INACTIVE	= 0x000000,
	TB_TYPE_PORT		= 0x000001,
	TB_TYPE_NHI		= 0x000002,
	/* TB_TYPE_ETHERNET	= 0x020000, lower order bits are not known */
	/* TB_TYPE_SATA		= 0x080000, lower order bits are not known */
	TB_TYPE_DP_HDMI_IN	= 0x0e0101,
	TB_TYPE_DP_HDMI_OUT	= 0x0e0102,
	TB_TYPE_PCIE_DOWN	= 0x100101,
	TB_TYPE_PCIE_UP		= 0x100102,
	TB_TYPE_USB3_DOWN	= 0x200101,
	TB_TYPE_USB3_UP		= 0x200102,
};

/* Present on every port in TB_CF_PORT at address zero. */
struct tb_regs_port_header {
	/* DWORD 0 */
	u16 vendor_id;
	u16 device_id;
	/* DWORD 1 */
	u32 first_cap_offset:8;
	u32 max_counters:11;
	u32 counters_support:1;
	u32 __unknown1:4;
	u32 revision:8;
	/* DWORD 2 */
	enum tb_port_type type:24;
	u32 thunderbolt_version:8;
	/* DWORD 3 */
	u32 __unknown2:20;
	u32 port_number:6;
	u32 __unknown3:6;
	/* DWORD 4 */
	u32 nfc_credits;
	/* DWORD 5 */
	u32 max_in_hop_id:11;
	u32 max_out_hop_id:11;
	u32 __unknown4:10;
	/* DWORD 6 */
	u32 __unknown5;
	/* DWORD 7 */
	u32 __unknown6;

} __packed;

/* Basic adapter configuration registers */
#define ADP_CS_4				0x04
#define ADP_CS_4_NFC_BUFFERS_MASK		GENMASK(9, 0)
#define ADP_CS_4_TOTAL_BUFFERS_MASK		GENMASK(29, 20)
#define ADP_CS_4_TOTAL_BUFFERS_SHIFT		20
#define ADP_CS_4_LCK				BIT(31)
#define ADP_CS_5				0x05
#define ADP_CS_5_LCA_MASK			GENMASK(28, 22)
#define ADP_CS_5_LCA_SHIFT			22

/* TMU adapter registers */
#define TMU_ADP_CS_3				0x03
#define TMU_ADP_CS_3_UDM			BIT(29)

/* Lane adapter registers */
#define LANE_ADP_CS_0				0x00
#define LANE_ADP_CS_0_SUPPORTED_WIDTH_MASK	GENMASK(25, 20)
#define LANE_ADP_CS_0_SUPPORTED_WIDTH_SHIFT	20
#define LANE_ADP_CS_1				0x01
#define LANE_ADP_CS_1_TARGET_WIDTH_MASK		GENMASK(9, 4)
#define LANE_ADP_CS_1_TARGET_WIDTH_SHIFT	4
#define LANE_ADP_CS_1_TARGET_WIDTH_SINGLE	0x1
#define LANE_ADP_CS_1_TARGET_WIDTH_DUAL		0x3
#define LANE_ADP_CS_1_LD			BIT(14)
#define LANE_ADP_CS_1_LB			BIT(15)
#define LANE_ADP_CS_1_CURRENT_SPEED_MASK	GENMASK(19, 16)
#define LANE_ADP_CS_1_CURRENT_SPEED_SHIFT	16
#define LANE_ADP_CS_1_CURRENT_SPEED_GEN2	0x8
#define LANE_ADP_CS_1_CURRENT_SPEED_GEN3	0x4
#define LANE_ADP_CS_1_CURRENT_WIDTH_MASK	GENMASK(25, 20)
#define LANE_ADP_CS_1_CURRENT_WIDTH_SHIFT	20

/* USB4 port registers */
#define PORT_CS_1				0x01
#define PORT_CS_1_LENGTH_SHIFT			8
#define PORT_CS_1_TARGET_MASK			GENMASK(18, 16)
#define PORT_CS_1_TARGET_SHIFT			16
#define PORT_CS_1_RETIMER_INDEX_SHIFT		20
#define PORT_CS_1_WNR_WRITE			BIT(24)
#define PORT_CS_1_NR				BIT(25)
#define PORT_CS_1_RC				BIT(26)
#define PORT_CS_1_PND				BIT(31)
#define PORT_CS_2				0x02
#define PORT_CS_18				0x12
#define PORT_CS_18_BE				BIT(8)
#define PORT_CS_18_TCM				BIT(9)
#define PORT_CS_18_WOU4S			BIT(18)
#define PORT_CS_19				0x13
#define PORT_CS_19_PC				BIT(3)
#define PORT_CS_19_PID				BIT(4)
#define PORT_CS_19_WOC				BIT(16)
#define PORT_CS_19_WOD				BIT(17)
#define PORT_CS_19_WOU4				BIT(18)

/* Display Port adapter registers */
#define ADP_DP_CS_0				0x00
#define ADP_DP_CS_0_VIDEO_HOPID_MASK		GENMASK(26, 16)
#define ADP_DP_CS_0_VIDEO_HOPID_SHIFT		16
#define ADP_DP_CS_0_AE				BIT(30)
#define ADP_DP_CS_0_VE				BIT(31)
#define ADP_DP_CS_1_AUX_TX_HOPID_MASK		GENMASK(10, 0)
#define ADP_DP_CS_1_AUX_RX_HOPID_MASK		GENMASK(21, 11)
#define ADP_DP_CS_1_AUX_RX_HOPID_SHIFT		11
#define ADP_DP_CS_2				0x02
#define ADP_DP_CS_2_HDP				BIT(6)
#define ADP_DP_CS_3				0x03
#define ADP_DP_CS_3_HDPC			BIT(9)
#define DP_LOCAL_CAP				0x04
#define DP_REMOTE_CAP				0x05
#define DP_STATUS_CTRL				0x06
#define DP_STATUS_CTRL_CMHS			BIT(25)
#define DP_STATUS_CTRL_UF			BIT(26)
#define DP_COMMON_CAP				0x07
/*
 * DP_COMMON_CAP offsets work also for DP_LOCAL_CAP and DP_REMOTE_CAP
 * with exception of DPRX done.
 */
#define DP_COMMON_CAP_RATE_MASK			GENMASK(11, 8)
#define DP_COMMON_CAP_RATE_SHIFT		8
#define DP_COMMON_CAP_RATE_RBR			0x0
#define DP_COMMON_CAP_RATE_HBR			0x1
#define DP_COMMON_CAP_RATE_HBR2			0x2
#define DP_COMMON_CAP_RATE_HBR3			0x3
#define DP_COMMON_CAP_LANES_MASK		GENMASK(14, 12)
#define DP_COMMON_CAP_LANES_SHIFT		12
#define DP_COMMON_CAP_1_LANE			0x0
#define DP_COMMON_CAP_2_LANES			0x1
#define DP_COMMON_CAP_4_LANES			0x2
#define DP_COMMON_CAP_DPRX_DONE			BIT(31)

/* PCIe adapter registers */
#define ADP_PCIE_CS_0				0x00
#define ADP_PCIE_CS_0_PE			BIT(31)

/* USB adapter registers */
#define ADP_USB3_CS_0				0x00
#define ADP_USB3_CS_0_V				BIT(30)
#define ADP_USB3_CS_0_PE			BIT(31)
#define ADP_USB3_CS_1				0x01
#define ADP_USB3_CS_1_CUBW_MASK			GENMASK(11, 0)
#define ADP_USB3_CS_1_CDBW_MASK			GENMASK(23, 12)
#define ADP_USB3_CS_1_CDBW_SHIFT		12
#define ADP_USB3_CS_1_HCA			BIT(31)
#define ADP_USB3_CS_2				0x02
#define ADP_USB3_CS_2_AUBW_MASK			GENMASK(11, 0)
#define ADP_USB3_CS_2_ADBW_MASK			GENMASK(23, 12)
#define ADP_USB3_CS_2_ADBW_SHIFT		12
#define ADP_USB3_CS_2_CMR			BIT(31)
#define ADP_USB3_CS_3				0x03
#define ADP_USB3_CS_3_SCALE_MASK		GENMASK(5, 0)
#define ADP_USB3_CS_4				0x04
#define ADP_USB3_CS_4_ALR_MASK			GENMASK(6, 0)
#define ADP_USB3_CS_4_ALR_20G			0x1
#define ADP_USB3_CS_4_ULV			BIT(7)
#define ADP_USB3_CS_4_MSLR_MASK			GENMASK(18, 12)
#define ADP_USB3_CS_4_MSLR_SHIFT		12
#define ADP_USB3_CS_4_MSLR_20G			0x1

/* Hop register from TB_CFG_HOPS. 8 byte per entry. */
struct tb_regs_hop {
	/* DWORD 0 */
	u32 next_hop:11; /*
			  * hop to take after sending the packet through
			  * out_port (on the incoming port of the next switch)
			  */
	u32 out_port:6; /* next port of the path (on the same switch) */
	u32 initial_credits:8;
	u32 unknown1:6; /* set to zero */
	bool enable:1;

	/* DWORD 1 */
	u32 weight:4;
	u32 unknown2:4; /* set to zero */
	u32 priority:3;
	bool drop_packages:1;
	u32 counter:11; /* index into TB_CFG_COUNTERS on this port */
	bool counter_enable:1;
	bool ingress_fc:1;
	bool egress_fc:1;
	bool ingress_shared_buffer:1;
	bool egress_shared_buffer:1;
	bool pending:1;
	u32 unknown3:3; /* set to zero */
} __packed;

/* Common link controller registers */
#define TB_LC_DESC			0x02
#define TB_LC_DESC_NLC_MASK		GENMASK(3, 0)
#define TB_LC_DESC_SIZE_SHIFT		8
#define TB_LC_DESC_SIZE_MASK		GENMASK(15, 8)
#define TB_LC_DESC_PORT_SIZE_SHIFT	16
#define TB_LC_DESC_PORT_SIZE_MASK	GENMASK(27, 16)
#define TB_LC_FUSE			0x03
#define TB_LC_SNK_ALLOCATION		0x10
#define TB_LC_SNK_ALLOCATION_SNK0_MASK	GENMASK(3, 0)
#define TB_LC_SNK_ALLOCATION_SNK0_CM	0x1
#define TB_LC_SNK_ALLOCATION_SNK1_SHIFT	4
#define TB_LC_SNK_ALLOCATION_SNK1_MASK	GENMASK(7, 4)
#define TB_LC_SNK_ALLOCATION_SNK1_CM	0x1
#define TB_LC_POWER			0x740

/* Link controller registers */
#define TB_LC_PORT_ATTR			0x8d
#define TB_LC_PORT_ATTR_BE		BIT(12)

#define TB_LC_SX_CTRL			0x96
#define TB_LC_SX_CTRL_WOC		BIT(1)
#define TB_LC_SX_CTRL_WOD		BIT(2)
#define TB_LC_SX_CTRL_WOU4		BIT(5)
#define TB_LC_SX_CTRL_WOP		BIT(6)
#define TB_LC_SX_CTRL_L1C		BIT(16)
#define TB_LC_SX_CTRL_L1D		BIT(17)
#define TB_LC_SX_CTRL_L2C		BIT(20)
#define TB_LC_SX_CTRL_L2D		BIT(21)
#define TB_LC_SX_CTRL_SLI		BIT(29)
#define TB_LC_SX_CTRL_UPSTREAM		BIT(30)
#define TB_LC_SX_CTRL_SLP		BIT(31)

#endif
