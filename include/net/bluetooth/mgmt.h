/*
   BlueZ - Bluetooth protocol stack for Linux

   Copyright (C) 2010  Nokia Corporation
   Copyright (C) 2011-2012  Intel Corporation

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2 as
   published by the Free Software Foundation;

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS.
   IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) AND AUTHOR(S) BE LIABLE FOR ANY
   CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES
   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

   ALL LIABILITY, INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PATENTS,
   COPYRIGHTS, TRADEMARKS OR OTHER RIGHTS, RELATING TO USE OF THIS
   SOFTWARE IS DISCLAIMED.
*/

#define MGMT_INDEX_NONE			0xFFFF

#define MGMT_STATUS_SUCCESS		0x00
#define MGMT_STATUS_UNKNOWN_COMMAND	0x01
#define MGMT_STATUS_NOT_CONNECTED	0x02
#define MGMT_STATUS_FAILED		0x03
#define MGMT_STATUS_CONNECT_FAILED	0x04
#define MGMT_STATUS_AUTH_FAILED		0x05
#define MGMT_STATUS_NOT_PAIRED		0x06
#define MGMT_STATUS_NO_RESOURCES	0x07
#define MGMT_STATUS_TIMEOUT		0x08
#define MGMT_STATUS_ALREADY_CONNECTED	0x09
#define MGMT_STATUS_BUSY		0x0a
#define MGMT_STATUS_REJECTED		0x0b
#define MGMT_STATUS_NOT_SUPPORTED	0x0c
#define MGMT_STATUS_INVALID_PARAMS	0x0d
#define MGMT_STATUS_DISCONNECTED	0x0e
#define MGMT_STATUS_NOT_POWERED		0x0f
#define MGMT_STATUS_CANCELLED		0x10
#define MGMT_STATUS_INVALID_INDEX	0x11
#define MGMT_STATUS_RFKILLED		0x12
#define MGMT_STATUS_ALREADY_PAIRED	0x13
#define MGMT_STATUS_PERMISSION_DENIED	0x14

struct mgmt_hdr {
	__le16	opcode;
	__le16	index;
	__le16	len;
} __packed;

struct mgmt_tlv {
	__le16 type;
	__u8   length;
	__u8   value[];
} __packed;

struct mgmt_addr_info {
	bdaddr_t	bdaddr;
	__u8		type;
} __packed;
#define MGMT_ADDR_INFO_SIZE		7

#define MGMT_OP_READ_VERSION		0x0001
#define MGMT_READ_VERSION_SIZE		0
struct mgmt_rp_read_version {
	__u8	version;
	__le16	revision;
} __packed;

#define MGMT_OP_READ_COMMANDS		0x0002
#define MGMT_READ_COMMANDS_SIZE		0
struct mgmt_rp_read_commands {
	__le16	num_commands;
	__le16	num_events;
	__le16	opcodes[];
} __packed;

#define MGMT_OP_READ_INDEX_LIST		0x0003
#define MGMT_READ_INDEX_LIST_SIZE	0
struct mgmt_rp_read_index_list {
	__le16	num_controllers;
	__le16	index[];
} __packed;

/* Reserve one extra byte for names in management messages so that they
 * are always guaranteed to be nul-terminated */
#define MGMT_MAX_NAME_LENGTH		(HCI_MAX_NAME_LENGTH + 1)
#define MGMT_MAX_SHORT_NAME_LENGTH	(HCI_MAX_SHORT_NAME_LENGTH + 1)

#define MGMT_SETTING_POWERED		0x00000001
#define MGMT_SETTING_CONNECTABLE	0x00000002
#define MGMT_SETTING_FAST_CONNECTABLE	0x00000004
#define MGMT_SETTING_DISCOVERABLE	0x00000008
#define MGMT_SETTING_BONDABLE		0x00000010
#define MGMT_SETTING_LINK_SECURITY	0x00000020
#define MGMT_SETTING_SSP		0x00000040
#define MGMT_SETTING_BREDR		0x00000080
#define MGMT_SETTING_HS			0x00000100
#define MGMT_SETTING_LE			0x00000200
#define MGMT_SETTING_ADVERTISING	0x00000400
#define MGMT_SETTING_SECURE_CONN	0x00000800
#define MGMT_SETTING_DEBUG_KEYS		0x00001000
#define MGMT_SETTING_PRIVACY		0x00002000
#define MGMT_SETTING_CONFIGURATION	0x00004000
#define MGMT_SETTING_STATIC_ADDRESS	0x00008000
#define MGMT_SETTING_PHY_CONFIGURATION	0x00010000
#define MGMT_SETTING_WIDEBAND_SPEECH	0x00020000

#define MGMT_OP_READ_INFO		0x0004
#define MGMT_READ_INFO_SIZE		0
struct mgmt_rp_read_info {
	bdaddr_t	bdaddr;
	__u8		version;
	__le16		manufacturer;
	__le32		supported_settings;
	__le32		current_settings;
	__u8		dev_class[3];
	__u8		name[MGMT_MAX_NAME_LENGTH];
	__u8		short_name[MGMT_MAX_SHORT_NAME_LENGTH];
} __packed;

struct mgmt_mode {
	__u8 val;
} __packed;

#define MGMT_SETTING_SIZE		1

#define MGMT_OP_SET_POWERED		0x0005

#define MGMT_OP_SET_DISCOVERABLE	0x0006
struct mgmt_cp_set_discoverable {
	__u8	val;
	__le16	timeout;
} __packed;
#define MGMT_SET_DISCOVERABLE_SIZE	3

#define MGMT_OP_SET_CONNECTABLE		0x0007

#define MGMT_OP_SET_FAST_CONNECTABLE	0x0008

#define MGMT_OP_SET_BONDABLE		0x0009

#define MGMT_OP_SET_LINK_SECURITY	0x000A

#define MGMT_OP_SET_SSP			0x000B

#define MGMT_OP_SET_HS			0x000C

#define MGMT_OP_SET_LE			0x000D
#define MGMT_OP_SET_DEV_CLASS		0x000E
struct mgmt_cp_set_dev_class {
	__u8	major;
	__u8	minor;
} __packed;
#define MGMT_SET_DEV_CLASS_SIZE		2

#define MGMT_OP_SET_LOCAL_NAME		0x000F
struct mgmt_cp_set_local_name {
	__u8	name[MGMT_MAX_NAME_LENGTH];
	__u8	short_name[MGMT_MAX_SHORT_NAME_LENGTH];
} __packed;
#define MGMT_SET_LOCAL_NAME_SIZE	260

#define MGMT_OP_ADD_UUID		0x0010
struct mgmt_cp_add_uuid {
	__u8	uuid[16];
	__u8	svc_hint;
} __packed;
#define MGMT_ADD_UUID_SIZE		17

#define MGMT_OP_REMOVE_UUID		0x0011
struct mgmt_cp_remove_uuid {
	__u8	uuid[16];
} __packed;
#define MGMT_REMOVE_UUID_SIZE		16

struct mgmt_link_key_info {
	struct mgmt_addr_info addr;
	__u8	type;
	__u8	val[16];
	__u8	pin_len;
} __packed;

#define MGMT_OP_LOAD_LINK_KEYS		0x0012
struct mgmt_cp_load_link_keys {
	__u8	debug_keys;
	__le16	key_count;
	struct	mgmt_link_key_info keys[];
} __packed;
#define MGMT_LOAD_LINK_KEYS_SIZE	3

#define MGMT_LTK_UNAUTHENTICATED	0x00
#define MGMT_LTK_AUTHENTICATED		0x01
#define MGMT_LTK_P256_UNAUTH		0x02
#define MGMT_LTK_P256_AUTH		0x03
#define MGMT_LTK_P256_DEBUG		0x04

struct mgmt_ltk_info {
	struct mgmt_addr_info addr;
	__u8	type;
	__u8	master;
	__u8	enc_size;
	__le16	ediv;
	__le64	rand;
	__u8	val[16];
} __packed;

#define MGMT_OP_LOAD_LONG_TERM_KEYS	0x0013
struct mgmt_cp_load_long_term_keys {
	__le16	key_count;
	struct	mgmt_ltk_info keys[];
} __packed;
#define MGMT_LOAD_LONG_TERM_KEYS_SIZE	2

#define MGMT_OP_DISCONNECT		0x0014
struct mgmt_cp_disconnect {
	struct mgmt_addr_info addr;
} __packed;
#define MGMT_DISCONNECT_SIZE		MGMT_ADDR_INFO_SIZE
struct mgmt_rp_disconnect {
	struct mgmt_addr_info addr;
} __packed;

#define MGMT_OP_GET_CONNECTIONS		0x0015
#define MGMT_GET_CONNECTIONS_SIZE	0
struct mgmt_rp_get_connections {
	__le16 conn_count;
	struct mgmt_addr_info addr[];
} __packed;

#define MGMT_OP_PIN_CODE_REPLY		0x0016
struct mgmt_cp_pin_code_reply {
	struct mgmt_addr_info addr;
	__u8	pin_len;
	__u8	pin_code[16];
} __packed;
#define MGMT_PIN_CODE_REPLY_SIZE	(MGMT_ADDR_INFO_SIZE + 17)
struct mgmt_rp_pin_code_reply {
	struct mgmt_addr_info addr;
} __packed;

#define MGMT_OP_PIN_CODE_NEG_REPLY	0x0017
struct mgmt_cp_pin_code_neg_reply {
	struct mgmt_addr_info addr;
} __packed;
#define MGMT_PIN_CODE_NEG_REPLY_SIZE	MGMT_ADDR_INFO_SIZE

#define MGMT_OP_SET_IO_CAPABILITY	0x0018
struct mgmt_cp_set_io_capability {
	__u8	io_capability;
} __packed;
#define MGMT_SET_IO_CAPABILITY_SIZE	1

#define MGMT_OP_PAIR_DEVICE		0x0019
struct mgmt_cp_pair_device {
	struct mgmt_addr_info addr;
	__u8	io_cap;
} __packed;
#define MGMT_PAIR_DEVICE_SIZE		(MGMT_ADDR_INFO_SIZE + 1)
struct mgmt_rp_pair_device {
	struct mgmt_addr_info addr;
} __packed;

#define MGMT_OP_CANCEL_PAIR_DEVICE	0x001A
#define MGMT_CANCEL_PAIR_DEVICE_SIZE	MGMT_ADDR_INFO_SIZE

#define MGMT_OP_UNPAIR_DEVICE		0x001B
struct mgmt_cp_unpair_device {
	struct mgmt_addr_info addr;
	__u8 disconnect;
} __packed;
#define MGMT_UNPAIR_DEVICE_SIZE		(MGMT_ADDR_INFO_SIZE + 1)
struct mgmt_rp_unpair_device {
	struct mgmt_addr_info addr;
};

#define MGMT_OP_USER_CONFIRM_REPLY	0x001C
struct mgmt_cp_user_confirm_reply {
	struct mgmt_addr_info addr;
} __packed;
#define MGMT_USER_CONFIRM_REPLY_SIZE	MGMT_ADDR_INFO_SIZE
struct mgmt_rp_user_confirm_reply {
	struct mgmt_addr_info addr;
} __packed;

#define MGMT_OP_USER_CONFIRM_NEG_REPLY	0x001D
struct mgmt_cp_user_confirm_neg_reply {
	struct mgmt_addr_info addr;
} __packed;
#define MGMT_USER_CONFIRM_NEG_REPLY_SIZE MGMT_ADDR_INFO_SIZE

#define MGMT_OP_USER_PASSKEY_REPLY	0x001E
struct mgmt_cp_user_passkey_reply {
	struct mgmt_addr_info addr;
	__le32	passkey;
} __packed;
#define MGMT_USER_PASSKEY_REPLY_SIZE	(MGMT_ADDR_INFO_SIZE + 4)
struct mgmt_rp_user_passkey_reply {
	struct mgmt_addr_info addr;
} __packed;

#define MGMT_OP_USER_PASSKEY_NEG_REPLY	0x001F
struct mgmt_cp_user_passkey_neg_reply {
	struct mgmt_addr_info addr;
} __packed;
#define MGMT_USER_PASSKEY_NEG_REPLY_SIZE MGMT_ADDR_INFO_SIZE

#define MGMT_OP_READ_LOCAL_OOB_DATA	0x0020
#define MGMT_READ_LOCAL_OOB_DATA_SIZE	0
struct mgmt_rp_read_local_oob_data {
	__u8	hash192[16];
	__u8	rand192[16];
	__u8	hash256[16];
	__u8	rand256[16];
} __packed;

#define MGMT_OP_ADD_REMOTE_OOB_DATA	0x0021
struct mgmt_cp_add_remote_oob_data {
	struct mgmt_addr_info addr;
	__u8	hash[16];
	__u8	rand[16];
} __packed;
#define MGMT_ADD_REMOTE_OOB_DATA_SIZE	(MGMT_ADDR_INFO_SIZE + 32)
struct mgmt_cp_add_remote_oob_ext_data {
	struct mgmt_addr_info addr;
	__u8	hash192[16];
	__u8	rand192[16];
	__u8	hash256[16];
	__u8	rand256[16];
} __packed;
#define MGMT_ADD_REMOTE_OOB_EXT_DATA_SIZE (MGMT_ADDR_INFO_SIZE + 64)

#define MGMT_OP_REMOVE_REMOTE_OOB_DATA	0x0022
struct mgmt_cp_remove_remote_oob_data {
	struct mgmt_addr_info addr;
} __packed;
#define MGMT_REMOVE_REMOTE_OOB_DATA_SIZE MGMT_ADDR_INFO_SIZE

#define MGMT_OP_START_DISCOVERY		0x0023
struct mgmt_cp_start_discovery {
	__u8 type;
} __packed;
#define MGMT_START_DISCOVERY_SIZE	1

#define MGMT_OP_STOP_DISCOVERY		0x0024
struct mgmt_cp_stop_discovery {
	__u8 type;
} __packed;
#define MGMT_STOP_DISCOVERY_SIZE	1

#define MGMT_OP_CONFIRM_NAME		0x0025
struct mgmt_cp_confirm_name {
	struct mgmt_addr_info addr;
	__u8	name_known;
} __packed;
#define MGMT_CONFIRM_NAME_SIZE		(MGMT_ADDR_INFO_SIZE + 1)
struct mgmt_rp_confirm_name {
	struct mgmt_addr_info addr;
} __packed;

#define MGMT_OP_BLOCK_DEVICE		0x0026
struct mgmt_cp_block_device {
	struct mgmt_addr_info addr;
} __packed;
#define MGMT_BLOCK_DEVICE_SIZE		MGMT_ADDR_INFO_SIZE

#define MGMT_OP_UNBLOCK_DEVICE		0x0027
struct mgmt_cp_unblock_device {
	struct mgmt_addr_info addr;
} __packed;
#define MGMT_UNBLOCK_DEVICE_SIZE	MGMT_ADDR_INFO_SIZE

#define MGMT_OP_SET_DEVICE_ID		0x0028
struct mgmt_cp_set_device_id {
	__le16	source;
	__le16	vendor;
	__le16	product;
	__le16	version;
} __packed;
#define MGMT_SET_DEVICE_ID_SIZE		8

#define MGMT_OP_SET_ADVERTISING		0x0029

#define MGMT_OP_SET_BREDR		0x002A

#define MGMT_OP_SET_STATIC_ADDRESS	0x002B
struct mgmt_cp_set_static_address {
	bdaddr_t bdaddr;
} __packed;
#define MGMT_SET_STATIC_ADDRESS_SIZE	6

#define MGMT_OP_SET_SCAN_PARAMS		0x002C
struct mgmt_cp_set_scan_params {
	__le16	interval;
	__le16	window;
} __packed;
#define MGMT_SET_SCAN_PARAMS_SIZE	4

#define MGMT_OP_SET_SECURE_CONN		0x002D

#define MGMT_OP_SET_DEBUG_KEYS		0x002E

#define MGMT_OP_SET_PRIVACY		0x002F
struct mgmt_cp_set_privacy {
	__u8 privacy;
	__u8 irk[16];
} __packed;
#define MGMT_SET_PRIVACY_SIZE		17

struct mgmt_irk_info {
	struct mgmt_addr_info addr;
	__u8 val[16];
} __packed;

#define MGMT_OP_LOAD_IRKS		0x0030
struct mgmt_cp_load_irks {
	__le16 irk_count;
	struct mgmt_irk_info irks[];
} __packed;
#define MGMT_LOAD_IRKS_SIZE		2

#define MGMT_OP_GET_CONN_INFO		0x0031
struct mgmt_cp_get_conn_info {
	struct mgmt_addr_info addr;
} __packed;
#define MGMT_GET_CONN_INFO_SIZE		MGMT_ADDR_INFO_SIZE
struct mgmt_rp_get_conn_info {
	struct mgmt_addr_info addr;
	__s8	rssi;
	__s8	tx_power;
	__s8	max_tx_power;
} __packed;

#define MGMT_OP_GET_CLOCK_INFO		0x0032
struct mgmt_cp_get_clock_info {
	struct mgmt_addr_info addr;
} __packed;
#define MGMT_GET_CLOCK_INFO_SIZE	MGMT_ADDR_INFO_SIZE
struct mgmt_rp_get_clock_info {
	struct mgmt_addr_info addr;
	__le32  local_clock;
	__le32  piconet_clock;
	__le16  accuracy;
} __packed;

#define MGMT_OP_ADD_DEVICE		0x0033
struct mgmt_cp_add_device {
	struct mgmt_addr_info addr;
	__u8	action;
} __packed;
#define MGMT_ADD_DEVICE_SIZE		(MGMT_ADDR_INFO_SIZE + 1)

#define MGMT_OP_REMOVE_DEVICE		0x0034
struct mgmt_cp_remove_device {
	struct mgmt_addr_info addr;
} __packed;
#define MGMT_REMOVE_DEVICE_SIZE		MGMT_ADDR_INFO_SIZE

struct mgmt_conn_param {
	struct mgmt_addr_info addr;
	__le16 min_interval;
	__le16 max_interval;
	__le16 latency;
	__le16 timeout;
} __packed;

#define MGMT_OP_LOAD_CONN_PARAM		0x0035
struct mgmt_cp_load_conn_param {
	__le16 param_count;
	struct mgmt_conn_param params[];
} __packed;
#define MGMT_LOAD_CONN_PARAM_SIZE	2

#define MGMT_OP_READ_UNCONF_INDEX_LIST	0x0036
#define MGMT_READ_UNCONF_INDEX_LIST_SIZE 0
struct mgmt_rp_read_unconf_index_list {
	__le16	num_controllers;
	__le16	index[];
} __packed;

#define MGMT_OPTION_EXTERNAL_CONFIG	0x00000001
#define MGMT_OPTION_PUBLIC_ADDRESS	0x00000002

#define MGMT_OP_READ_CONFIG_INFO	0x0037
#define MGMT_READ_CONFIG_INFO_SIZE	0
struct mgmt_rp_read_config_info {
	__le16	manufacturer;
	__le32	supported_options;
	__le32	missing_options;
} __packed;

#define MGMT_OP_SET_EXTERNAL_CONFIG	0x0038
struct mgmt_cp_set_external_config {
	__u8 config;
} __packed;
#define MGMT_SET_EXTERNAL_CONFIG_SIZE	1

#define MGMT_OP_SET_PUBLIC_ADDRESS	0x0039
struct mgmt_cp_set_public_address {
	bdaddr_t bdaddr;
} __packed;
#define MGMT_SET_PUBLIC_ADDRESS_SIZE	6

#define MGMT_OP_START_SERVICE_DISCOVERY	0x003A
struct mgmt_cp_start_service_discovery {
	__u8 type;
	__s8 rssi;
	__le16 uuid_count;
	__u8 uuids[][16];
} __packed;
#define MGMT_START_SERVICE_DISCOVERY_SIZE 4

#define MGMT_OP_READ_LOCAL_OOB_EXT_DATA	0x003B
struct mgmt_cp_read_local_oob_ext_data {
	__u8 type;
} __packed;
#define MGMT_READ_LOCAL_OOB_EXT_DATA_SIZE 1
struct mgmt_rp_read_local_oob_ext_data {
	__u8    type;
	__le16	eir_len;
	__u8	eir[];
} __packed;

#define MGMT_OP_READ_EXT_INDEX_LIST	0x003C
#define MGMT_READ_EXT_INDEX_LIST_SIZE	0
struct mgmt_rp_read_ext_index_list {
	__le16	num_controllers;
	struct {
		__le16 index;
		__u8   type;
		__u8   bus;
	} entry[];
} __packed;

#define MGMT_OP_READ_ADV_FEATURES	0x0003D
#define MGMT_READ_ADV_FEATURES_SIZE	0
struct mgmt_rp_read_adv_features {
	__le32 supported_flags;
	__u8   max_adv_data_len;
	__u8   max_scan_rsp_len;
	__u8   max_instances;
	__u8   num_instances;
	__u8   instance[];
} __packed;

#define MGMT_OP_ADD_ADVERTISING		0x003E
struct mgmt_cp_add_advertising {
	__u8	instance;
	__le32	flags;
	__le16	duration;
	__le16	timeout;
	__u8	adv_data_len;
	__u8	scan_rsp_len;
	__u8	data[];
} __packed;
#define MGMT_ADD_ADVERTISING_SIZE	11
struct mgmt_rp_add_advertising {
	__u8	instance;
} __packed;

#define MGMT_ADV_FLAG_CONNECTABLE	BIT(0)
#define MGMT_ADV_FLAG_DISCOV		BIT(1)
#define MGMT_ADV_FLAG_LIMITED_DISCOV	BIT(2)
#define MGMT_ADV_FLAG_MANAGED_FLAGS	BIT(3)
#define MGMT_ADV_FLAG_TX_POWER		BIT(4)
#define MGMT_ADV_FLAG_APPEARANCE	BIT(5)
#define MGMT_ADV_FLAG_LOCAL_NAME	BIT(6)
#define MGMT_ADV_FLAG_SEC_1M 		BIT(7)
#define MGMT_ADV_FLAG_SEC_2M 		BIT(8)
#define MGMT_ADV_FLAG_SEC_CODED 	BIT(9)
#define MGMT_ADV_FLAG_CAN_SET_TX_POWER	BIT(10)
#define MGMT_ADV_FLAG_HW_OFFLOAD	BIT(11)
#define MGMT_ADV_PARAM_DURATION		BIT(12)
#define MGMT_ADV_PARAM_TIMEOUT		BIT(13)
#define MGMT_ADV_PARAM_INTERVALS	BIT(14)
#define MGMT_ADV_PARAM_TX_POWER		BIT(15)
#define MGMT_ADV_PARAM_SCAN_RSP		BIT(16)

#define MGMT_ADV_FLAG_SEC_MASK	(MGMT_ADV_FLAG_SEC_1M | MGMT_ADV_FLAG_SEC_2M | \
				 MGMT_ADV_FLAG_SEC_CODED)

#define MGMT_OP_REMOVE_ADVERTISING	0x003F
struct mgmt_cp_remove_advertising {
	__u8	instance;
} __packed;
#define MGMT_REMOVE_ADVERTISING_SIZE	1
struct mgmt_rp_remove_advertising {
	__u8	instance;
} __packed;

#define MGMT_OP_GET_ADV_SIZE_INFO	0x0040
struct mgmt_cp_get_adv_size_info {
	__u8	instance;
	__le32	flags;
} __packed;
#define MGMT_GET_ADV_SIZE_INFO_SIZE	5
struct mgmt_rp_get_adv_size_info {
	__u8	instance;
	__le32	flags;
	__u8	max_adv_data_len;
	__u8	max_scan_rsp_len;
} __packed;

#define MGMT_OP_START_LIMITED_DISCOVERY	0x0041

#define MGMT_OP_READ_EXT_INFO		0x0042
#define MGMT_READ_EXT_INFO_SIZE		0
struct mgmt_rp_read_ext_info {
	bdaddr_t bdaddr;
	__u8     version;
	__le16   manufacturer;
	__le32   supported_settings;
	__le32   current_settings;
	__le16   eir_len;
	__u8     eir[];
} __packed;

#define MGMT_OP_SET_APPEARANCE		0x0043
struct mgmt_cp_set_appearance {
	__le16	appearance;
} __packed;
#define MGMT_SET_APPEARANCE_SIZE	2

#define MGMT_OP_GET_PHY_CONFIGURATION	0x0044
struct mgmt_rp_get_phy_configuration {
	__le32	supported_phys;
	__le32	configurable_phys;
	__le32	selected_phys;
} __packed;
#define MGMT_GET_PHY_CONFIGURATION_SIZE	0

#define MGMT_PHY_BR_1M_1SLOT	0x00000001
#define MGMT_PHY_BR_1M_3SLOT	0x00000002
#define MGMT_PHY_BR_1M_5SLOT	0x00000004
#define MGMT_PHY_EDR_2M_1SLOT	0x00000008
#define MGMT_PHY_EDR_2M_3SLOT	0x00000010
#define MGMT_PHY_EDR_2M_5SLOT	0x00000020
#define MGMT_PHY_EDR_3M_1SLOT	0x00000040
#define MGMT_PHY_EDR_3M_3SLOT	0x00000080
#define MGMT_PHY_EDR_3M_5SLOT	0x00000100
#define MGMT_PHY_LE_1M_TX		0x00000200
#define MGMT_PHY_LE_1M_RX		0x00000400
#define MGMT_PHY_LE_2M_TX		0x00000800
#define MGMT_PHY_LE_2M_RX		0x00001000
#define MGMT_PHY_LE_CODED_TX	0x00002000
#define MGMT_PHY_LE_CODED_RX	0x00004000

#define MGMT_PHY_BREDR_MASK (MGMT_PHY_BR_1M_1SLOT | MGMT_PHY_BR_1M_3SLOT | \
			     MGMT_PHY_BR_1M_5SLOT | MGMT_PHY_EDR_2M_1SLOT | \
			     MGMT_PHY_EDR_2M_3SLOT | MGMT_PHY_EDR_2M_5SLOT | \
			     MGMT_PHY_EDR_3M_1SLOT | MGMT_PHY_EDR_3M_3SLOT | \
			     MGMT_PHY_EDR_3M_5SLOT)
#define MGMT_PHY_LE_MASK (MGMT_PHY_LE_1M_TX | MGMT_PHY_LE_1M_RX | \
			  MGMT_PHY_LE_2M_TX | MGMT_PHY_LE_2M_RX | \
			  MGMT_PHY_LE_CODED_TX | MGMT_PHY_LE_CODED_RX)
#define MGMT_PHY_LE_TX_MASK (MGMT_PHY_LE_1M_TX | MGMT_PHY_LE_2M_TX | \
			     MGMT_PHY_LE_CODED_TX)
#define MGMT_PHY_LE_RX_MASK (MGMT_PHY_LE_1M_RX | MGMT_PHY_LE_2M_RX | \
			     MGMT_PHY_LE_CODED_RX)

#define MGMT_OP_SET_PHY_CONFIGURATION	0x0045
struct mgmt_cp_set_phy_configuration {
	__le32	selected_phys;
} __packed;
#define MGMT_SET_PHY_CONFIGURATION_SIZE	4

#define MGMT_OP_SET_BLOCKED_KEYS	0x0046

#define HCI_BLOCKED_KEY_TYPE_LINKKEY	0x00
#define HCI_BLOCKED_KEY_TYPE_LTK	0x01
#define HCI_BLOCKED_KEY_TYPE_IRK	0x02

struct mgmt_blocked_key_info {
	__u8 type;
	__u8 val[16];
} __packed;

struct mgmt_cp_set_blocked_keys {
	__le16 key_count;
	struct mgmt_blocked_key_info keys[];
} __packed;
#define MGMT_OP_SET_BLOCKED_KEYS_SIZE 2

#define MGMT_OP_SET_WIDEBAND_SPEECH	0x0047

#define MGMT_CAP_SEC_FLAGS		0x01
#define MGMT_CAP_MAX_ENC_KEY_SIZE	0x02
#define MGMT_CAP_SMP_MAX_ENC_KEY_SIZE	0x03
#define MGMT_CAP_LE_TX_PWR		0x04

#define MGMT_OP_READ_CONTROLLER_CAP	0x0048
#define MGMT_READ_CONTROLLER_CAP_SIZE	0
struct mgmt_rp_read_controller_cap {
	__le16   cap_len;
	__u8     cap[0];
} __packed;

#define MGMT_OP_READ_EXP_FEATURES_INFO	0x0049
#define MGMT_READ_EXP_FEATURES_INFO_SIZE 0
struct mgmt_rp_read_exp_features_info {
	__le16 feature_count;
	struct {
		__u8   uuid[16];
		__le32 flags;
	} features[];
} __packed;

#define MGMT_OP_SET_EXP_FEATURE		0x004a
struct mgmt_cp_set_exp_feature {
	__u8   uuid[16];
	__u8   param[];
} __packed;
#define MGMT_SET_EXP_FEATURE_SIZE	16
struct mgmt_rp_set_exp_feature {
	__u8   uuid[16];
	__le32 flags;
} __packed;

#define MGMT_OP_READ_DEF_SYSTEM_CONFIG	0x004b
#define MGMT_READ_DEF_SYSTEM_CONFIG_SIZE	0

#define MGMT_OP_SET_DEF_SYSTEM_CONFIG	0x004c
#define MGMT_SET_DEF_SYSTEM_CONFIG_SIZE		0

#define MGMT_OP_READ_DEF_RUNTIME_CONFIG	0x004d
#define MGMT_READ_DEF_RUNTIME_CONFIG_SIZE	0

#define MGMT_OP_SET_DEF_RUNTIME_CONFIG	0x004e
#define MGMT_SET_DEF_RUNTIME_CONFIG_SIZE	0

#define MGMT_OP_GET_DEVICE_FLAGS	0x004F
#define MGMT_GET_DEVICE_FLAGS_SIZE	7
struct mgmt_cp_get_device_flags {
	struct mgmt_addr_info addr;
} __packed;
struct mgmt_rp_get_device_flags {
	struct mgmt_addr_info addr;
	__le32 supported_flags;
	__le32 current_flags;
} __packed;

#define MGMT_OP_SET_DEVICE_FLAGS	0x0050
#define MGMT_SET_DEVICE_FLAGS_SIZE	11
struct mgmt_cp_set_device_flags {
	struct mgmt_addr_info addr;
	__le32 current_flags;
} __packed;
struct mgmt_rp_set_device_flags {
	struct mgmt_addr_info addr;
} __packed;

#define MGMT_ADV_MONITOR_FEATURE_MASK_OR_PATTERNS    BIT(0)

#define MGMT_OP_READ_ADV_MONITOR_FEATURES	0x0051
#define MGMT_READ_ADV_MONITOR_FEATURES_SIZE	0
struct mgmt_rp_read_adv_monitor_features {
	__le32 supported_features;
	__le32 enabled_features;
	__le16 max_num_handles;
	__u8 max_num_patterns;
	__le16 num_handles;
	__le16 handles[];
}  __packed;

struct mgmt_adv_pattern {
	__u8 ad_type;
	__u8 offset;
	__u8 length;
	__u8 value[31];
} __packed;

#define MGMT_OP_ADD_ADV_PATTERNS_MONITOR	0x0052
struct mgmt_cp_add_adv_patterns_monitor {
	__u8 pattern_count;
	struct mgmt_adv_pattern patterns[];
} __packed;
#define MGMT_ADD_ADV_PATTERNS_MONITOR_SIZE	1
struct mgmt_rp_add_adv_patterns_monitor {
	__le16 monitor_handle;
} __packed;

#define MGMT_OP_REMOVE_ADV_MONITOR		0x0053
struct mgmt_cp_remove_adv_monitor {
	__le16 monitor_handle;
} __packed;
#define MGMT_REMOVE_ADV_MONITOR_SIZE		2
struct mgmt_rp_remove_adv_monitor {
	__le16 monitor_handle;
} __packed;

#define MGMT_OP_ADD_EXT_ADV_PARAMS		0x0054
struct mgmt_cp_add_ext_adv_params {
	__u8	instance;
	__le32	flags;
	__le16	duration;
	__le16	timeout;
	__le32	min_interval;
	__le32	max_interval;
	__s8	tx_power;
} __packed;
#define MGMT_ADD_EXT_ADV_PARAMS_MIN_SIZE	18
struct mgmt_rp_add_ext_adv_params {
	__u8	instance;
	__s8	tx_power;
	__u8	max_adv_data_len;
	__u8	max_scan_rsp_len;
} __packed;

#define MGMT_OP_ADD_EXT_ADV_DATA		0x0055
struct mgmt_cp_add_ext_adv_data {
	__u8	instance;
	__u8	adv_data_len;
	__u8	scan_rsp_len;
	__u8	data[];
} __packed;
#define MGMT_ADD_EXT_ADV_DATA_SIZE	3
struct mgmt_rp_add_ext_adv_data {
	__u8	instance;
} __packed;

struct mgmt_adv_rssi_thresholds {
	__s8	high_threshold;
	__le16	high_threshold_timeout;
	__s8	low_threshold;
	__le16	low_threshold_timeout;
	__u8	sampling_period;
} __packed;

#define MGMT_OP_ADD_ADV_PATTERNS_MONITOR_RSSI	0x0056
struct mgmt_cp_add_adv_patterns_monitor_rssi {
	struct mgmt_adv_rssi_thresholds rssi;
	__u8	pattern_count;
	struct mgmt_adv_pattern patterns[];
} __packed;
#define MGMT_ADD_ADV_PATTERNS_MONITOR_RSSI_SIZE	8

#define MGMT_EV_CMD_COMPLETE		0x0001
struct mgmt_ev_cmd_complete {
	__le16	opcode;
	__u8	status;
	__u8	data[];
} __packed;

#define MGMT_EV_CMD_STATUS		0x0002
struct mgmt_ev_cmd_status {
	__le16	opcode;
	__u8	status;
} __packed;

#define MGMT_EV_CONTROLLER_ERROR	0x0003
struct mgmt_ev_controller_error {
	__u8	error_code;
} __packed;

#define MGMT_EV_INDEX_ADDED		0x0004

#define MGMT_EV_INDEX_REMOVED		0x0005

#define MGMT_EV_NEW_SETTINGS		0x0006

#define MGMT_EV_CLASS_OF_DEV_CHANGED	0x0007
struct mgmt_ev_class_of_dev_changed {
	__u8	dev_class[3];
};

#define MGMT_EV_LOCAL_NAME_CHANGED	0x0008
struct mgmt_ev_local_name_changed {
	__u8	name[MGMT_MAX_NAME_LENGTH];
	__u8	short_name[MGMT_MAX_SHORT_NAME_LENGTH];
} __packed;

#define MGMT_EV_NEW_LINK_KEY		0x0009
struct mgmt_ev_new_link_key {
	__u8	store_hint;
	struct mgmt_link_key_info key;
} __packed;

#define MGMT_EV_NEW_LONG_TERM_KEY	0x000A
struct mgmt_ev_new_long_term_key {
	__u8	store_hint;
	struct mgmt_ltk_info key;
} __packed;

#define MGMT_EV_DEVICE_CONNECTED	0x000B
struct mgmt_ev_device_connected {
	struct mgmt_addr_info addr;
	__le32	flags;
	__le16	eir_len;
	__u8	eir[];
} __packed;

#define MGMT_DEV_DISCONN_UNKNOWN	0x00
#define MGMT_DEV_DISCONN_TIMEOUT	0x01
#define MGMT_DEV_DISCONN_LOCAL_HOST	0x02
#define MGMT_DEV_DISCONN_REMOTE		0x03
#define MGMT_DEV_DISCONN_AUTH_FAILURE	0x04
#define MGMT_DEV_DISCONN_LOCAL_HOST_SUSPEND	0x05

#define MGMT_EV_DEVICE_DISCONNECTED	0x000C
struct mgmt_ev_device_disconnected {
	struct mgmt_addr_info addr;
	__u8	reason;
} __packed;

#define MGMT_EV_CONNECT_FAILED		0x000D
struct mgmt_ev_connect_failed {
	struct mgmt_addr_info addr;
	__u8	status;
} __packed;

#define MGMT_EV_PIN_CODE_REQUEST	0x000E
struct mgmt_ev_pin_code_request {
	struct mgmt_addr_info addr;
	__u8	secure;
} __packed;

#define MGMT_EV_USER_CONFIRM_REQUEST	0x000F
struct mgmt_ev_user_confirm_request {
	struct mgmt_addr_info addr;
	__u8	confirm_hint;
	__le32	value;
} __packed;

#define MGMT_EV_USER_PASSKEY_REQUEST	0x0010
struct mgmt_ev_user_passkey_request {
	struct mgmt_addr_info addr;
} __packed;

#define MGMT_EV_AUTH_FAILED		0x0011
struct mgmt_ev_auth_failed {
	struct mgmt_addr_info addr;
	__u8	status;
} __packed;

#define MGMT_DEV_FOUND_CONFIRM_NAME    0x01
#define MGMT_DEV_FOUND_LEGACY_PAIRING  0x02
#define MGMT_DEV_FOUND_NOT_CONNECTABLE 0x04

#define MGMT_EV_DEVICE_FOUND		0x0012
struct mgmt_ev_device_found {
	struct mgmt_addr_info addr;
	__s8	rssi;
	__le32	flags;
	__le16	eir_len;
	__u8	eir[];
} __packed;

#define MGMT_EV_DISCOVERING		0x0013
struct mgmt_ev_discovering {
	__u8	type;
	__u8	discovering;
} __packed;

#define MGMT_EV_DEVICE_BLOCKED		0x0014
struct mgmt_ev_device_blocked {
	struct mgmt_addr_info addr;
} __packed;

#define MGMT_EV_DEVICE_UNBLOCKED	0x0015
struct mgmt_ev_device_unblocked {
	struct mgmt_addr_info addr;
} __packed;

#define MGMT_EV_DEVICE_UNPAIRED		0x0016
struct mgmt_ev_device_unpaired {
	struct mgmt_addr_info addr;
} __packed;

#define MGMT_EV_PASSKEY_NOTIFY		0x0017
struct mgmt_ev_passkey_notify {
	struct mgmt_addr_info addr;
	__le32	passkey;
	__u8	entered;
} __packed;

#define MGMT_EV_NEW_IRK			0x0018
struct mgmt_ev_new_irk {
	__u8     store_hint;
	bdaddr_t rpa;
	struct mgmt_irk_info irk;
} __packed;

#define MGMT_CSRK_LOCAL_UNAUTHENTICATED		0x00
#define MGMT_CSRK_REMOTE_UNAUTHENTICATED	0x01
#define MGMT_CSRK_LOCAL_AUTHENTICATED		0x02
#define MGMT_CSRK_REMOTE_AUTHENTICATED		0x03

struct mgmt_csrk_info {
	struct mgmt_addr_info addr;
	__u8 type;
	__u8 val[16];
} __packed;

#define MGMT_EV_NEW_CSRK		0x0019
struct mgmt_ev_new_csrk {
	__u8 store_hint;
	struct mgmt_csrk_info key;
} __packed;

#define MGMT_EV_DEVICE_ADDED		0x001a
struct mgmt_ev_device_added {
	struct mgmt_addr_info addr;
	__u8 action;
} __packed;

#define MGMT_EV_DEVICE_REMOVED		0x001b
struct mgmt_ev_device_removed {
	struct mgmt_addr_info addr;
} __packed;

#define MGMT_EV_NEW_CONN_PARAM		0x001c
struct mgmt_ev_new_conn_param {
	struct mgmt_addr_info addr;
	__u8 store_hint;
	__le16 min_interval;
	__le16 max_interval;
	__le16 latency;
	__le16 timeout;
} __packed;

#define MGMT_EV_UNCONF_INDEX_ADDED	0x001d

#define MGMT_EV_UNCONF_INDEX_REMOVED	0x001e

#define MGMT_EV_NEW_CONFIG_OPTIONS	0x001f

struct mgmt_ev_ext_index {
	__u8 type;
	__u8 bus;
} __packed;

#define MGMT_EV_EXT_INDEX_ADDED		0x0020

#define MGMT_EV_EXT_INDEX_REMOVED	0x0021

#define MGMT_EV_LOCAL_OOB_DATA_UPDATED	0x0022
struct mgmt_ev_local_oob_data_updated {
	__u8    type;
	__le16	eir_len;
	__u8	eir[];
} __packed;

#define MGMT_EV_ADVERTISING_ADDED	0x0023
struct mgmt_ev_advertising_added {
	__u8    instance;
} __packed;

#define MGMT_EV_ADVERTISING_REMOVED	0x0024
struct mgmt_ev_advertising_removed {
	__u8    instance;
} __packed;

#define MGMT_EV_EXT_INFO_CHANGED	0x0025
struct mgmt_ev_ext_info_changed {
	__le16	eir_len;
	__u8	eir[];
} __packed;

#define MGMT_EV_PHY_CONFIGURATION_CHANGED	0x0026
struct mgmt_ev_phy_configuration_changed {
	__le32	selected_phys;
} __packed;

#define MGMT_EV_EXP_FEATURE_CHANGED	0x0027
struct mgmt_ev_exp_feature_changed {
	__u8	uuid[16];
	__le32	flags;
} __packed;

#define MGMT_EV_DEVICE_FLAGS_CHANGED		0x002a
struct mgmt_ev_device_flags_changed {
	struct mgmt_addr_info addr;
	__le32 supported_flags;
	__le32 current_flags;
} __packed;

#define MGMT_EV_ADV_MONITOR_ADDED	0x002b
struct mgmt_ev_adv_monitor_added {
	__le16 monitor_handle;
}  __packed;

#define MGMT_EV_ADV_MONITOR_REMOVED	0x002c
struct mgmt_ev_adv_monitor_removed {
	__le16 monitor_handle;
}  __packed;

#define MGMT_EV_CONTROLLER_SUSPEND		0x002d
struct mgmt_ev_controller_suspend {
	__u8	suspend_state;
} __packed;

#define MGMT_EV_CONTROLLER_RESUME		0x002e
struct mgmt_ev_controller_resume {
	__u8	wake_reason;
	struct mgmt_addr_info addr;
} __packed;

#define MGMT_WAKE_REASON_NON_BT_WAKE		0x0
#define MGMT_WAKE_REASON_UNEXPECTED		0x1
#define MGMT_WAKE_REASON_REMOTE_WAKE		0x2
