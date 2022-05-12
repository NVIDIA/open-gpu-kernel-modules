/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright(c) 2007 - 2018 Intel Corporation. */

#ifndef _E1000_IGB_HW_H_
#define _E1000_IGB_HW_H_

#include <linux/types.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/netdevice.h>

#include "e1000_regs.h"
#include "e1000_defines.h"

struct e1000_hw;

#define E1000_DEV_ID_82576			0x10C9
#define E1000_DEV_ID_82576_FIBER		0x10E6
#define E1000_DEV_ID_82576_SERDES		0x10E7
#define E1000_DEV_ID_82576_QUAD_COPPER		0x10E8
#define E1000_DEV_ID_82576_QUAD_COPPER_ET2	0x1526
#define E1000_DEV_ID_82576_NS			0x150A
#define E1000_DEV_ID_82576_NS_SERDES		0x1518
#define E1000_DEV_ID_82576_SERDES_QUAD		0x150D
#define E1000_DEV_ID_82575EB_COPPER		0x10A7
#define E1000_DEV_ID_82575EB_FIBER_SERDES	0x10A9
#define E1000_DEV_ID_82575GB_QUAD_COPPER	0x10D6
#define E1000_DEV_ID_82580_COPPER		0x150E
#define E1000_DEV_ID_82580_FIBER		0x150F
#define E1000_DEV_ID_82580_SERDES		0x1510
#define E1000_DEV_ID_82580_SGMII		0x1511
#define E1000_DEV_ID_82580_COPPER_DUAL		0x1516
#define E1000_DEV_ID_82580_QUAD_FIBER		0x1527
#define E1000_DEV_ID_DH89XXCC_SGMII		0x0438
#define E1000_DEV_ID_DH89XXCC_SERDES		0x043A
#define E1000_DEV_ID_DH89XXCC_BACKPLANE		0x043C
#define E1000_DEV_ID_DH89XXCC_SFP		0x0440
#define E1000_DEV_ID_I350_COPPER		0x1521
#define E1000_DEV_ID_I350_FIBER			0x1522
#define E1000_DEV_ID_I350_SERDES		0x1523
#define E1000_DEV_ID_I350_SGMII			0x1524
#define E1000_DEV_ID_I210_COPPER		0x1533
#define E1000_DEV_ID_I210_FIBER			0x1536
#define E1000_DEV_ID_I210_SERDES		0x1537
#define E1000_DEV_ID_I210_SGMII			0x1538
#define E1000_DEV_ID_I210_COPPER_FLASHLESS	0x157B
#define E1000_DEV_ID_I210_SERDES_FLASHLESS	0x157C
#define E1000_DEV_ID_I211_COPPER		0x1539
#define E1000_DEV_ID_I354_BACKPLANE_1GBPS	0x1F40
#define E1000_DEV_ID_I354_SGMII			0x1F41
#define E1000_DEV_ID_I354_BACKPLANE_2_5GBPS	0x1F45

#define E1000_REVISION_2 2
#define E1000_REVISION_4 4

#define E1000_FUNC_0     0
#define E1000_FUNC_1     1
#define E1000_FUNC_2     2
#define E1000_FUNC_3     3

#define E1000_ALT_MAC_ADDRESS_OFFSET_LAN0   0
#define E1000_ALT_MAC_ADDRESS_OFFSET_LAN1   3
#define E1000_ALT_MAC_ADDRESS_OFFSET_LAN2   6
#define E1000_ALT_MAC_ADDRESS_OFFSET_LAN3   9

enum e1000_mac_type {
	e1000_undefined = 0,
	e1000_82575,
	e1000_82576,
	e1000_82580,
	e1000_i350,
	e1000_i354,
	e1000_i210,
	e1000_i211,
	e1000_num_macs  /* List is 1-based, so subtract 1 for true count. */
};

enum e1000_media_type {
	e1000_media_type_unknown = 0,
	e1000_media_type_copper = 1,
	e1000_media_type_fiber = 2,
	e1000_media_type_internal_serdes = 3,
	e1000_num_media_types
};

enum e1000_nvm_type {
	e1000_nvm_unknown = 0,
	e1000_nvm_none,
	e1000_nvm_eeprom_spi,
	e1000_nvm_flash_hw,
	e1000_nvm_invm,
	e1000_nvm_flash_sw
};

enum e1000_nvm_override {
	e1000_nvm_override_none = 0,
	e1000_nvm_override_spi_small,
	e1000_nvm_override_spi_large,
};

enum e1000_phy_type {
	e1000_phy_unknown = 0,
	e1000_phy_none,
	e1000_phy_m88,
	e1000_phy_igp,
	e1000_phy_igp_2,
	e1000_phy_gg82563,
	e1000_phy_igp_3,
	e1000_phy_ife,
	e1000_phy_82580,
	e1000_phy_i210,
	e1000_phy_bcm54616,
};

enum e1000_bus_type {
	e1000_bus_type_unknown = 0,
	e1000_bus_type_pci,
	e1000_bus_type_pcix,
	e1000_bus_type_pci_express,
	e1000_bus_type_reserved
};

enum e1000_bus_speed {
	e1000_bus_speed_unknown = 0,
	e1000_bus_speed_33,
	e1000_bus_speed_66,
	e1000_bus_speed_100,
	e1000_bus_speed_120,
	e1000_bus_speed_133,
	e1000_bus_speed_2500,
	e1000_bus_speed_5000,
	e1000_bus_speed_reserved
};

enum e1000_bus_width {
	e1000_bus_width_unknown = 0,
	e1000_bus_width_pcie_x1,
	e1000_bus_width_pcie_x2,
	e1000_bus_width_pcie_x4 = 4,
	e1000_bus_width_pcie_x8 = 8,
	e1000_bus_width_32,
	e1000_bus_width_64,
	e1000_bus_width_reserved
};

enum e1000_1000t_rx_status {
	e1000_1000t_rx_status_not_ok = 0,
	e1000_1000t_rx_status_ok,
	e1000_1000t_rx_status_undefined = 0xFF
};

enum e1000_rev_polarity {
	e1000_rev_polarity_normal = 0,
	e1000_rev_polarity_reversed,
	e1000_rev_polarity_undefined = 0xFF
};

enum e1000_fc_mode {
	e1000_fc_none = 0,
	e1000_fc_rx_pause,
	e1000_fc_tx_pause,
	e1000_fc_full,
	e1000_fc_default = 0xFF
};

/* Statistics counters collected by the MAC */
struct e1000_hw_stats {
	u64 crcerrs;
	u64 algnerrc;
	u64 symerrs;
	u64 rxerrc;
	u64 mpc;
	u64 scc;
	u64 ecol;
	u64 mcc;
	u64 latecol;
	u64 colc;
	u64 dc;
	u64 tncrs;
	u64 sec;
	u64 cexterr;
	u64 rlec;
	u64 xonrxc;
	u64 xontxc;
	u64 xoffrxc;
	u64 xofftxc;
	u64 fcruc;
	u64 prc64;
	u64 prc127;
	u64 prc255;
	u64 prc511;
	u64 prc1023;
	u64 prc1522;
	u64 gprc;
	u64 bprc;
	u64 mprc;
	u64 gptc;
	u64 gorc;
	u64 gotc;
	u64 rnbc;
	u64 ruc;
	u64 rfc;
	u64 roc;
	u64 rjc;
	u64 mgprc;
	u64 mgpdc;
	u64 mgptc;
	u64 tor;
	u64 tot;
	u64 tpr;
	u64 tpt;
	u64 ptc64;
	u64 ptc127;
	u64 ptc255;
	u64 ptc511;
	u64 ptc1023;
	u64 ptc1522;
	u64 mptc;
	u64 bptc;
	u64 tsctc;
	u64 tsctfc;
	u64 iac;
	u64 icrxptc;
	u64 icrxatc;
	u64 ictxptc;
	u64 ictxatc;
	u64 ictxqec;
	u64 ictxqmtc;
	u64 icrxdmtc;
	u64 icrxoc;
	u64 cbtmpc;
	u64 htdpmc;
	u64 cbrdpc;
	u64 cbrmpc;
	u64 rpthc;
	u64 hgptc;
	u64 htcbdpc;
	u64 hgorc;
	u64 hgotc;
	u64 lenerrs;
	u64 scvpc;
	u64 hrmpc;
	u64 doosync;
	u64 o2bgptc;
	u64 o2bspc;
	u64 b2ospc;
	u64 b2ogprc;
};

struct e1000_host_mng_dhcp_cookie {
	u32 signature;
	u8  status;
	u8  reserved0;
	u16 vlan_id;
	u32 reserved1;
	u16 reserved2;
	u8  reserved3;
	u8  checksum;
};

/* Host Interface "Rev 1" */
struct e1000_host_command_header {
	u8 command_id;
	u8 command_length;
	u8 command_options;
	u8 checksum;
};

#define E1000_HI_MAX_DATA_LENGTH     252
struct e1000_host_command_info {
	struct e1000_host_command_header command_header;
	u8 command_data[E1000_HI_MAX_DATA_LENGTH];
};

/* Host Interface "Rev 2" */
struct e1000_host_mng_command_header {
	u8  command_id;
	u8  checksum;
	u16 reserved1;
	u16 reserved2;
	u16 command_length;
};

#define E1000_HI_MAX_MNG_DATA_LENGTH 0x6F8
struct e1000_host_mng_command_info {
	struct e1000_host_mng_command_header command_header;
	u8 command_data[E1000_HI_MAX_MNG_DATA_LENGTH];
};

#include "e1000_mac.h"
#include "e1000_phy.h"
#include "e1000_nvm.h"
#include "e1000_mbx.h"

struct e1000_mac_operations {
	s32 (*check_for_link)(struct e1000_hw *);
	s32 (*reset_hw)(struct e1000_hw *);
	s32 (*init_hw)(struct e1000_hw *);
	bool (*check_mng_mode)(struct e1000_hw *);
	s32 (*setup_physical_interface)(struct e1000_hw *);
	void (*rar_set)(struct e1000_hw *, u8 *, u32);
	s32 (*read_mac_addr)(struct e1000_hw *);
	s32 (*get_speed_and_duplex)(struct e1000_hw *, u16 *, u16 *);
	s32 (*acquire_swfw_sync)(struct e1000_hw *, u16);
	void (*release_swfw_sync)(struct e1000_hw *, u16);
#ifdef CONFIG_IGB_HWMON
	s32 (*get_thermal_sensor_data)(struct e1000_hw *);
	s32 (*init_thermal_sensor_thresh)(struct e1000_hw *);
#endif
	void (*write_vfta)(struct e1000_hw *, u32, u32);
};

struct e1000_phy_operations {
	s32 (*acquire)(struct e1000_hw *);
	s32 (*check_polarity)(struct e1000_hw *);
	s32 (*check_reset_block)(struct e1000_hw *);
	s32 (*force_speed_duplex)(struct e1000_hw *);
	s32 (*get_cfg_done)(struct e1000_hw *hw);
	s32 (*get_cable_length)(struct e1000_hw *);
	s32 (*get_phy_info)(struct e1000_hw *);
	s32 (*read_reg)(struct e1000_hw *, u32, u16 *);
	void (*release)(struct e1000_hw *);
	s32 (*reset)(struct e1000_hw *);
	s32 (*set_d0_lplu_state)(struct e1000_hw *, bool);
	s32 (*set_d3_lplu_state)(struct e1000_hw *, bool);
	s32 (*write_reg)(struct e1000_hw *, u32, u16);
	s32 (*read_i2c_byte)(struct e1000_hw *, u8, u8, u8 *);
	s32 (*write_i2c_byte)(struct e1000_hw *, u8, u8, u8);
};

struct e1000_nvm_operations {
	s32 (*acquire)(struct e1000_hw *);
	s32 (*read)(struct e1000_hw *, u16, u16, u16 *);
	void (*release)(struct e1000_hw *);
	s32 (*write)(struct e1000_hw *, u16, u16, u16 *);
	s32 (*update)(struct e1000_hw *);
	s32 (*validate)(struct e1000_hw *);
	s32 (*valid_led_default)(struct e1000_hw *, u16 *);
};

#define E1000_MAX_SENSORS		3

struct e1000_thermal_diode_data {
	u8 location;
	u8 temp;
	u8 caution_thresh;
	u8 max_op_thresh;
};

struct e1000_thermal_sensor_data {
	struct e1000_thermal_diode_data sensor[E1000_MAX_SENSORS];
};

struct e1000_info {
	s32 (*get_invariants)(struct e1000_hw *);
	struct e1000_mac_operations *mac_ops;
	const struct e1000_phy_operations *phy_ops;
	struct e1000_nvm_operations *nvm_ops;
};

extern const struct e1000_info e1000_82575_info;

struct e1000_mac_info {
	struct e1000_mac_operations ops;

	u8 addr[6];
	u8 perm_addr[6];

	enum e1000_mac_type type;

	u32 ledctl_default;
	u32 ledctl_mode1;
	u32 ledctl_mode2;
	u32 mc_filter_type;
	u32 txcw;

	u16 mta_reg_count;
	u16 uta_reg_count;

	/* Maximum size of the MTA register table in all supported adapters */
	#define MAX_MTA_REG 128
	u32 mta_shadow[MAX_MTA_REG];
	u16 rar_entry_count;

	u8  forced_speed_duplex;

	bool adaptive_ifs;
	bool arc_subsystem_valid;
	bool asf_firmware_present;
	bool autoneg;
	bool autoneg_failed;
	bool disable_hw_init_bits;
	bool get_link_status;
	bool ifs_params_forced;
	bool in_ifs_mode;
	bool report_tx_early;
	bool serdes_has_link;
	bool tx_pkt_filtering;
	struct e1000_thermal_sensor_data thermal_sensor_data;
};

struct e1000_phy_info {
	struct e1000_phy_operations ops;

	enum e1000_phy_type type;

	enum e1000_1000t_rx_status local_rx;
	enum e1000_1000t_rx_status remote_rx;
	enum e1000_ms_type ms_type;
	enum e1000_ms_type original_ms_type;
	enum e1000_rev_polarity cable_polarity;
	enum e1000_smart_speed smart_speed;

	u32 addr;
	u32 id;
	u32 reset_delay_us; /* in usec */
	u32 revision;

	enum e1000_media_type media_type;

	u16 autoneg_advertised;
	u16 autoneg_mask;
	u16 cable_length;
	u16 max_cable_length;
	u16 min_cable_length;
	u16 pair_length[4];

	u8 mdix;

	bool disable_polarity_correction;
	bool is_mdix;
	bool polarity_correction;
	bool reset_disable;
	bool speed_downgraded;
	bool autoneg_wait_to_complete;
};

struct e1000_nvm_info {
	struct e1000_nvm_operations ops;
	enum e1000_nvm_type type;
	enum e1000_nvm_override override;

	u32 flash_bank_size;
	u32 flash_base_addr;

	u16 word_size;
	u16 delay_usec;
	u16 address_bits;
	u16 opcode_bits;
	u16 page_size;
};

struct e1000_bus_info {
	enum e1000_bus_type type;
	enum e1000_bus_speed speed;
	enum e1000_bus_width width;

	u32 snoop;

	u16 func;
	u16 pci_cmd_word;
};

struct e1000_fc_info {
	u32 high_water;     /* Flow control high-water mark */
	u32 low_water;      /* Flow control low-water mark */
	u16 pause_time;     /* Flow control pause timer */
	bool send_xon;      /* Flow control send XON */
	bool strict_ieee;   /* Strict IEEE mode */
	enum e1000_fc_mode current_mode; /* Type of flow control */
	enum e1000_fc_mode requested_mode;
};

struct e1000_mbx_operations {
	s32 (*init_params)(struct e1000_hw *hw);
	s32 (*read)(struct e1000_hw *hw, u32 *msg, u16 size, u16 mbx_id,
		    bool unlock);
	s32 (*write)(struct e1000_hw *hw, u32 *msg, u16 size, u16 mbx_id);
	s32 (*read_posted)(struct e1000_hw *hw, u32 *msg, u16 size, u16 mbx_id);
	s32 (*write_posted)(struct e1000_hw *hw, u32 *msg, u16 size,
			    u16 mbx_id);
	s32 (*check_for_msg)(struct e1000_hw *hw, u16 mbx_id);
	s32 (*check_for_ack)(struct e1000_hw *hw, u16 mbx_id);
	s32 (*check_for_rst)(struct e1000_hw *hw, u16 mbx_id);
	s32 (*unlock)(struct e1000_hw *hw, u16 mbx_id);
};

struct e1000_mbx_stats {
	u32 msgs_tx;
	u32 msgs_rx;

	u32 acks;
	u32 reqs;
	u32 rsts;
};

struct e1000_mbx_info {
	struct e1000_mbx_operations ops;
	struct e1000_mbx_stats stats;
	u32 timeout;
	u32 usec_delay;
	u16 size;
};

struct e1000_dev_spec_82575 {
	bool sgmii_active;
	bool global_device_reset;
	bool eee_disable;
	bool clear_semaphore_once;
	struct e1000_sfp_flags eth_flags;
	bool module_plugged;
	u8 media_port;
	bool media_changed;
	bool mas_capable;
};

struct e1000_hw {
	void *back;

	u8 __iomem *hw_addr;
	u8 __iomem *flash_address;
	unsigned long io_base;

	struct e1000_mac_info  mac;
	struct e1000_fc_info   fc;
	struct e1000_phy_info  phy;
	struct e1000_nvm_info  nvm;
	struct e1000_bus_info  bus;
	struct e1000_mbx_info mbx;
	struct e1000_host_mng_dhcp_cookie mng_cookie;

	union {
		struct e1000_dev_spec_82575	_82575;
	} dev_spec;

	u16 device_id;
	u16 subsystem_vendor_id;
	u16 subsystem_device_id;
	u16 vendor_id;

	u8  revision_id;
};

struct net_device *igb_get_hw_dev(struct e1000_hw *hw);
#define hw_dbg(format, arg...) \
	netdev_dbg(igb_get_hw_dev(hw), format, ##arg)

/* These functions must be implemented by drivers */
s32 igb_read_pcie_cap_reg(struct e1000_hw *hw, u32 reg, u16 *value);
s32 igb_write_pcie_cap_reg(struct e1000_hw *hw, u32 reg, u16 *value);

void igb_read_pci_cfg(struct e1000_hw *hw, u32 reg, u16 *value);
void igb_write_pci_cfg(struct e1000_hw *hw, u32 reg, u16 *value);
#endif /* _E1000_IGB_HW_H_ */
