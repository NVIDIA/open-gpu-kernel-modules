/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * ibmvfc.h -- driver for IBM Power Virtual Fibre Channel Adapter
 *
 * Written By: Brian King <brking@linux.vnet.ibm.com>, IBM Corporation
 *
 * Copyright (C) IBM Corporation, 2008
 */

#ifndef _IBMVFC_H
#define _IBMVFC_H

#include <linux/list.h>
#include <linux/types.h>
#include <scsi/viosrp.h>

#define IBMVFC_NAME	"ibmvfc"
#define IBMVFC_DRIVER_VERSION		"1.0.11"
#define IBMVFC_DRIVER_DATE		"(April 12, 2013)"

#define IBMVFC_DEFAULT_TIMEOUT	60
#define IBMVFC_ADISC_CANCEL_TIMEOUT	45
#define IBMVFC_ADISC_TIMEOUT		15
#define IBMVFC_ADISC_PLUS_CANCEL_TIMEOUT	\
		(IBMVFC_ADISC_TIMEOUT + IBMVFC_ADISC_CANCEL_TIMEOUT)
#define IBMVFC_INIT_TIMEOUT		120
#define IBMVFC_ABORT_TIMEOUT		8
#define IBMVFC_ABORT_WAIT_TIMEOUT	40
#define IBMVFC_MAX_REQUESTS_DEFAULT	100

#define IBMVFC_DEBUG			0
#define IBMVFC_MAX_TARGETS		1024
#define IBMVFC_MAX_LUN			0xffffffff
#define IBMVFC_MAX_SECTORS		0xffffu
#define IBMVFC_MAX_DISC_THREADS	4
#define IBMVFC_TGT_MEMPOOL_SZ		64
#define IBMVFC_MAX_CMDS_PER_LUN	64
#define IBMVFC_MAX_HOST_INIT_RETRIES	6
#define IBMVFC_MAX_TGT_INIT_RETRIES		3
#define IBMVFC_DEV_LOSS_TMO		(5 * 60)
#define IBMVFC_DEFAULT_LOG_LEVEL	2
#define IBMVFC_MAX_CDB_LEN		16
#define IBMVFC_CLS3_ERROR		0
#define IBMVFC_MQ			1
#define IBMVFC_SCSI_CHANNELS		8
#define IBMVFC_MAX_SCSI_QUEUES		16
#define IBMVFC_SCSI_HW_QUEUES		8
#define IBMVFC_MIG_NO_SUB_TO_CRQ	0
#define IBMVFC_MIG_NO_N_TO_M		0

/*
 * Ensure we have resources for ERP and initialization:
 * 1 for ERP
 * 1 for initialization
 * 1 for NPIV Logout
 * 2 for BSG passthru
 * 2 for each discovery thread
 */
#define IBMVFC_NUM_INTERNAL_REQ	(1 + 1 + 1 + 2 + (disc_threads * 2))

#define IBMVFC_MAD_SUCCESS		0x00
#define IBMVFC_MAD_NOT_SUPPORTED	0xF1
#define IBMVFC_MAD_VERSION_NOT_SUPP	0xF2
#define IBMVFC_MAD_FAILED		0xF7
#define IBMVFC_MAD_DRIVER_FAILED	0xEE
#define IBMVFC_MAD_CRQ_ERROR		0xEF

enum ibmvfc_crq_valid {
	IBMVFC_CRQ_CMD_RSP		= 0x80,
	IBMVFC_CRQ_INIT_RSP		= 0xC0,
	IBMVFC_CRQ_XPORT_EVENT		= 0xFF,
};

enum ibmvfc_crq_init_msg {
	IBMVFC_CRQ_INIT			= 0x01,
	IBMVFC_CRQ_INIT_COMPLETE	= 0x02,
};

enum ibmvfc_crq_xport_evts {
	IBMVFC_PARTNER_FAILED		= 0x01,
	IBMVFC_PARTNER_DEREGISTER	= 0x02,
	IBMVFC_PARTITION_MIGRATED	= 0x06,
};

enum ibmvfc_cmd_status_flags {
	IBMVFC_FABRIC_MAPPED		= 0x0001,
	IBMVFC_VIOS_FAILURE		= 0x0002,
	IBMVFC_FC_FAILURE			= 0x0004,
	IBMVFC_FC_SCSI_ERROR		= 0x0008,
	IBMVFC_HW_EVENT_LOGGED		= 0x0010,
	IBMVFC_VIOS_LOGGED		= 0x0020,
};

enum ibmvfc_fabric_mapped_errors {
	IBMVFC_UNABLE_TO_ESTABLISH	= 0x0001,
	IBMVFC_XPORT_FAULT		= 0x0002,
	IBMVFC_CMD_TIMEOUT		= 0x0003,
	IBMVFC_ENETDOWN			= 0x0004,
	IBMVFC_HW_FAILURE			= 0x0005,
	IBMVFC_LINK_DOWN_ERR		= 0x0006,
	IBMVFC_LINK_DEAD_ERR		= 0x0007,
	IBMVFC_UNABLE_TO_REGISTER	= 0x0008,
	IBMVFC_XPORT_BUSY			= 0x000A,
	IBMVFC_XPORT_DEAD			= 0x000B,
	IBMVFC_CONFIG_ERROR		= 0x000C,
	IBMVFC_NAME_SERVER_FAIL		= 0x000D,
	IBMVFC_LINK_HALTED		= 0x000E,
	IBMVFC_XPORT_GENERAL		= 0x8000,
};

enum ibmvfc_vios_errors {
	IBMVFC_CRQ_FAILURE			= 0x0001,
	IBMVFC_SW_FAILURE				= 0x0002,
	IBMVFC_INVALID_PARAMETER		= 0x0003,
	IBMVFC_MISSING_PARAMETER		= 0x0004,
	IBMVFC_HOST_IO_BUS			= 0x0005,
	IBMVFC_TRANS_CANCELLED			= 0x0006,
	IBMVFC_TRANS_CANCELLED_IMPLICIT	= 0x0007,
	IBMVFC_INSUFFICIENT_RESOURCE		= 0x0008,
	IBMVFC_PLOGI_REQUIRED			= 0x0010,
	IBMVFC_COMMAND_FAILED			= 0x8000,
};

enum ibmvfc_mad_types {
	IBMVFC_NPIV_LOGIN		= 0x0001,
	IBMVFC_DISC_TARGETS	= 0x0002,
	IBMVFC_PORT_LOGIN		= 0x0004,
	IBMVFC_PROCESS_LOGIN	= 0x0008,
	IBMVFC_QUERY_TARGET	= 0x0010,
	IBMVFC_MOVE_LOGIN		= 0x0020,
	IBMVFC_IMPLICIT_LOGOUT	= 0x0040,
	IBMVFC_PASSTHRU		= 0x0200,
	IBMVFC_TMF_MAD		= 0x0100,
	IBMVFC_NPIV_LOGOUT	= 0x0800,
	IBMVFC_CHANNEL_ENQUIRY	= 0x1000,
	IBMVFC_CHANNEL_SETUP	= 0x2000,
	IBMVFC_CONNECTION_INFO	= 0x4000,
};

struct ibmvfc_mad_common {
	__be32 version;
	__be32 reserved;
	__be32 opcode;
	__be16 status;
	__be16 length;
	__be64 tag;
} __packed __aligned(8);

struct ibmvfc_npiv_login_mad {
	struct ibmvfc_mad_common common;
	struct srp_direct_buf buffer;
} __packed __aligned(8);

struct ibmvfc_npiv_logout_mad {
	struct ibmvfc_mad_common common;
} __packed __aligned(8);

#define IBMVFC_MAX_NAME 256

struct ibmvfc_npiv_login {
	__be32 ostype;
#define IBMVFC_OS_LINUX	0x02
	__be32 pad;
	__be64 max_dma_len;
	__be32 max_payload;
	__be32 max_response;
	__be32 partition_num;
	__be32 vfc_frame_version;
	__be16 fcp_version;
	__be16 flags;
#define IBMVFC_CLIENT_MIGRATED	0x01
#define IBMVFC_FLUSH_ON_HALT		0x02
	__be32 max_cmds;
	__be64 capabilities;
#define IBMVFC_CAN_MIGRATE		0x01
#define IBMVFC_CAN_USE_CHANNELS		0x02
#define IBMVFC_CAN_HANDLE_FPIN		0x04
#define IBMVFC_CAN_USE_MAD_VERSION	0x08
#define IBMVFC_CAN_SEND_VF_WWPN		0x10
	__be64 node_name;
	struct srp_direct_buf async;
	u8 partition_name[IBMVFC_MAX_NAME];
	u8 device_name[IBMVFC_MAX_NAME];
	u8 drc_name[IBMVFC_MAX_NAME];
	__be64 reserved2[2];
} __packed __aligned(8);

struct ibmvfc_common_svc_parms {
	__be16 fcph_version;
	__be16 b2b_credit;
	__be16 features;
	__be16 bb_rcv_sz; /* upper nibble is BB_SC_N */
	__be32 ratov;
	__be32 edtov;
} __packed __aligned(4);

struct ibmvfc_service_parms {
	struct ibmvfc_common_svc_parms common;
	u8 port_name[8];
	u8 node_name[8];
	__be32 class1_parms[4];
	__be32 class2_parms[4];
	__be32 class3_parms[4];
	__be32 obsolete[4];
	__be32 vendor_version[4];
	__be32 services_avail[2];
	__be32 ext_len;
	__be32 reserved[30];
	__be32 clk_sync_qos[2];
	__be32 reserved2;
} __packed __aligned(4);

struct ibmvfc_npiv_login_resp {
	__be32 version;
	__be16 status;
	__be16 error;
	__be32 flags;
#define IBMVFC_NATIVE_FC		0x01
	__be32 reserved;
	__be64 capabilities;
#define IBMVFC_CAN_FLUSH_ON_HALT	0x08
#define IBMVFC_CAN_SUPPRESS_ABTS	0x10
#define IBMVFC_MAD_VERSION_CAP		0x20
#define IBMVFC_HANDLE_VF_WWPN		0x40
#define IBMVFC_CAN_SUPPORT_CHANNELS	0x80
	__be32 max_cmds;
	__be32 scsi_id_sz;
	__be64 max_dma_len;
	__be64 scsi_id;
	__be64 port_name;
	__be64 node_name;
	__be64 link_speed;
	u8 partition_name[IBMVFC_MAX_NAME];
	u8 device_name[IBMVFC_MAX_NAME];
	u8 port_loc_code[IBMVFC_MAX_NAME];
	u8 drc_name[IBMVFC_MAX_NAME];
	struct ibmvfc_service_parms service_parms;
	__be64 reserved2;
} __packed __aligned(8);

union ibmvfc_npiv_login_data {
	struct ibmvfc_npiv_login login;
	struct ibmvfc_npiv_login_resp resp;
} __packed __aligned(8);

struct ibmvfc_discover_targets_entry {
	__be32 scsi_id;
	__be32 pad;
	__be64 wwpn;
#define IBMVFC_DISC_TGT_SCSI_ID_MASK	0x00ffffff
} __packed __aligned(8);

struct ibmvfc_discover_targets {
	struct ibmvfc_mad_common common;
	struct srp_direct_buf buffer;
	__be32 flags;
#define IBMVFC_DISC_TGT_PORT_ID_WWPN_LIST	0x02
	__be16 status;
	__be16 error;
	__be32 bufflen;
	__be32 num_avail;
	__be32 num_written;
	__be64 reserved[2];
} __packed __aligned(8);

enum ibmvfc_fc_reason {
	IBMVFC_INVALID_ELS_CMD_CODE	= 0x01,
	IBMVFC_INVALID_VERSION		= 0x02,
	IBMVFC_LOGICAL_ERROR		= 0x03,
	IBMVFC_INVALID_CT_IU_SIZE	= 0x04,
	IBMVFC_LOGICAL_BUSY		= 0x05,
	IBMVFC_PROTOCOL_ERROR		= 0x07,
	IBMVFC_UNABLE_TO_PERFORM_REQ	= 0x09,
	IBMVFC_CMD_NOT_SUPPORTED	= 0x0B,
	IBMVFC_SERVER_NOT_AVAIL		= 0x0D,
	IBMVFC_CMD_IN_PROGRESS		= 0x0E,
	IBMVFC_VENDOR_SPECIFIC		= 0xFF,
};

enum ibmvfc_fc_type {
	IBMVFC_FABRIC_REJECT	= 0x01,
	IBMVFC_PORT_REJECT	= 0x02,
	IBMVFC_LS_REJECT		= 0x03,
	IBMVFC_FABRIC_BUSY	= 0x04,
	IBMVFC_PORT_BUSY		= 0x05,
	IBMVFC_BASIC_REJECT	= 0x06,
};

enum ibmvfc_gs_explain {
	IBMVFC_PORT_NAME_NOT_REG	= 0x02,
};

struct ibmvfc_port_login {
	struct ibmvfc_mad_common common;
	__be64 scsi_id;
	__be16 reserved;
	__be16 fc_service_class;
	__be32 blksz;
	__be32 hdr_per_blk;
	__be16 status;
	__be16 error;		/* also fc_reason */
	__be16 fc_explain;
	__be16 fc_type;
	__be32 reserved2;
	struct ibmvfc_service_parms service_parms;
	struct ibmvfc_service_parms service_parms_change;
	__be64 target_wwpn;
	__be64 reserved3[2];
} __packed __aligned(8);

struct ibmvfc_move_login {
	struct ibmvfc_mad_common common;
	__be64 old_scsi_id;
	__be64 new_scsi_id;
	__be64 wwpn;
	__be64 node_name;
	__be32 flags;
#define IBMVFC_MOVE_LOGIN_IMPLICIT_OLD_FAILED	0x01
#define IBMVFC_MOVE_LOGIN_IMPLICIT_NEW_FAILED	0x02
#define IBMVFC_MOVE_LOGIN_PORT_LOGIN_FAILED	0x04
	__be32 reserved;
	struct ibmvfc_service_parms service_parms;
	struct ibmvfc_service_parms service_parms_change;
	__be32 reserved2;
	__be16 service_class;
	__be16 vios_flags;
#define IBMVFC_MOVE_LOGIN_VF_NOT_SENT_ADAPTER	0x01
	__be64 reserved3;
} __packed __aligned(8);

struct ibmvfc_prli_svc_parms {
	u8 type;
#define IBMVFC_SCSI_FCP_TYPE		0x08
	u8 type_ext;
	__be16 flags;
#define IBMVFC_PRLI_ORIG_PA_VALID			0x8000
#define IBMVFC_PRLI_RESP_PA_VALID			0x4000
#define IBMVFC_PRLI_EST_IMG_PAIR			0x2000
	__be32 orig_pa;
	__be32 resp_pa;
	__be32 service_parms;
#define IBMVFC_PRLI_TASK_RETRY			0x00000200
#define IBMVFC_PRLI_RETRY				0x00000100
#define IBMVFC_PRLI_DATA_OVERLAY			0x00000040
#define IBMVFC_PRLI_INITIATOR_FUNC			0x00000020
#define IBMVFC_PRLI_TARGET_FUNC			0x00000010
#define IBMVFC_PRLI_READ_FCP_XFER_RDY_DISABLED	0x00000002
#define IBMVFC_PRLI_WR_FCP_XFER_RDY_DISABLED	0x00000001
} __packed __aligned(4);

struct ibmvfc_process_login {
	struct ibmvfc_mad_common common;
	__be64 scsi_id;
	struct ibmvfc_prli_svc_parms parms;
	u8 reserved[48];
	__be16 status;
	__be16 error;			/* also fc_reason */
	__be32 reserved2;
	__be64 target_wwpn;
	__be64 reserved3[2];
} __packed __aligned(8);

struct ibmvfc_query_tgt {
	struct ibmvfc_mad_common common;
	__be64 wwpn;
	__be64 scsi_id;
	__be16 status;
	__be16 error;
	__be16 fc_explain;
	__be16 fc_type;
	__be64 reserved[2];
} __packed __aligned(8);

struct ibmvfc_implicit_logout {
	struct ibmvfc_mad_common common;
	__be64 old_scsi_id;
	__be64 reserved[2];
} __packed __aligned(8);

struct ibmvfc_tmf {
	struct ibmvfc_mad_common common;
	__be64 scsi_id;
	struct scsi_lun lun;
	__be32 flags;
#define IBMVFC_TMF_ABORT_TASK		0x02
#define IBMVFC_TMF_ABORT_TASK_SET	0x04
#define IBMVFC_TMF_LUN_RESET		0x10
#define IBMVFC_TMF_TGT_RESET		0x20
#define IBMVFC_TMF_LUA_VALID		0x40
#define IBMVFC_TMF_SUPPRESS_ABTS	0x80
	__be32 cancel_key;
	__be32 my_cancel_key;
	__be32 pad;
	__be64 target_wwpn;
	__be64 task_tag;
	__be64 reserved[2];
} __packed __aligned(8);

enum ibmvfc_fcp_rsp_info_codes {
	RSP_NO_FAILURE		= 0x00,
	RSP_TMF_REJECTED		= 0x04,
	RSP_TMF_FAILED		= 0x05,
	RSP_TMF_INVALID_LUN	= 0x09,
};

struct ibmvfc_fcp_rsp_info {
	u8 reserved[3];
	u8 rsp_code;
	u8 reserved2[4];
} __packed __aligned(2);

enum ibmvfc_fcp_rsp_flags {
	FCP_BIDI_RSP			= 0x80,
	FCP_BIDI_READ_RESID_UNDER	= 0x40,
	FCP_BIDI_READ_RESID_OVER	= 0x20,
	FCP_CONF_REQ			= 0x10,
	FCP_RESID_UNDER			= 0x08,
	FCP_RESID_OVER			= 0x04,
	FCP_SNS_LEN_VALID			= 0x02,
	FCP_RSP_LEN_VALID			= 0x01,
};

union ibmvfc_fcp_rsp_data {
	struct ibmvfc_fcp_rsp_info info;
	u8 sense[SCSI_SENSE_BUFFERSIZE + sizeof(struct ibmvfc_fcp_rsp_info)];
} __packed __aligned(8);

struct ibmvfc_fcp_rsp {
	__be64 reserved;
	__be16 retry_delay_timer;
	u8 flags;
	u8 scsi_status;
	__be32 fcp_resid;
	__be32 fcp_sense_len;
	__be32 fcp_rsp_len;
	union ibmvfc_fcp_rsp_data data;
} __packed __aligned(8);

enum ibmvfc_cmd_flags {
	IBMVFC_SCATTERLIST	= 0x0001,
	IBMVFC_NO_MEM_DESC	= 0x0002,
	IBMVFC_READ			= 0x0004,
	IBMVFC_WRITE		= 0x0008,
	IBMVFC_TMF			= 0x0080,
	IBMVFC_CLASS_3_ERR	= 0x0100,
};

enum ibmvfc_fc_task_attr {
	IBMVFC_SIMPLE_TASK	= 0x00,
	IBMVFC_HEAD_OF_QUEUE	= 0x01,
	IBMVFC_ORDERED_TASK	= 0x02,
	IBMVFC_ACA_TASK		= 0x04,
};

enum ibmvfc_fc_tmf_flags {
	IBMVFC_ABORT_TASK_SET	= 0x02,
	IBMVFC_LUN_RESET		= 0x10,
	IBMVFC_TARGET_RESET	= 0x20,
};

struct ibmvfc_fcp_cmd_iu {
	struct scsi_lun lun;
	u8 crn;
	u8 pri_task_attr;
	u8 tmf_flags;
	u8 add_cdb_len;
#define IBMVFC_RDDATA		0x02
#define IBMVFC_WRDATA		0x01
	u8 cdb[IBMVFC_MAX_CDB_LEN];
	__be32 xfer_len;
} __packed __aligned(4);

struct ibmvfc_cmd {
	__be64 task_tag;
	__be32 frame_type;
	__be32 payload_len;
	__be32 resp_len;
	__be32 adapter_resid;
	__be16 status;
	__be16 error;
	__be16 flags;
	__be16 response_flags;
#define IBMVFC_ADAPTER_RESID_VALID	0x01
	__be32 cancel_key;
	__be32 exchange_id;
	struct srp_direct_buf ext_func;
	struct srp_direct_buf ioba;
	struct srp_direct_buf resp;
	__be64 correlation;
	__be64 tgt_scsi_id;
	__be64 tag;
	__be64 target_wwpn;
	__be64 reserved3;
	union {
		struct {
			struct ibmvfc_fcp_cmd_iu iu;
			struct ibmvfc_fcp_rsp rsp;
		} v1;
		struct {
			__be64 reserved4;
			struct ibmvfc_fcp_cmd_iu iu;
			struct ibmvfc_fcp_rsp rsp;
		} v2;
	};
} __packed __aligned(8);

struct ibmvfc_passthru_fc_iu {
	__be32 payload[7];
#define IBMVFC_ADISC	0x52000000
	__be32 response[7];
};

struct ibmvfc_passthru_iu {
	__be64 task_tag;
	__be32 cmd_len;
	__be32 rsp_len;
	__be16 status;
	__be16 error;
	__be32 flags;
#define IBMVFC_FC_ELS		0x01
#define IBMVFC_FC_CT_IU		0x02
	__be32 cancel_key;
#define IBMVFC_PASSTHRU_CANCEL_KEY	0x80000000
#define IBMVFC_INTERNAL_CANCEL_KEY	0x80000001
	__be32 reserved;
	struct srp_direct_buf cmd;
	struct srp_direct_buf rsp;
	__be64 correlation;
	__be64 scsi_id;
	__be64 tag;
	__be64 target_wwpn;
	__be64 reserved2[2];
} __packed __aligned(8);

struct ibmvfc_passthru_mad {
	struct ibmvfc_mad_common common;
	struct srp_direct_buf cmd_ioba;
	struct ibmvfc_passthru_iu iu;
	struct ibmvfc_passthru_fc_iu fc_iu;
} __packed __aligned(8);

struct ibmvfc_channel_enquiry {
	struct ibmvfc_mad_common common;
	__be32 flags;
#define IBMVFC_NO_CHANNELS_TO_CRQ_SUPPORT	0x01
#define IBMVFC_SUPPORT_VARIABLE_SUBQ_MSG	0x02
#define IBMVFC_NO_N_TO_M_CHANNELS_SUPPORT	0x04
	__be32 num_scsi_subq_channels;
	__be32 num_nvmeof_subq_channels;
	__be32 num_scsi_vas_channels;
	__be32 num_nvmeof_vas_channels;
} __packed __aligned(8);

struct ibmvfc_channel_setup_mad {
	struct ibmvfc_mad_common common;
	struct srp_direct_buf buffer;
} __packed __aligned(8);

#define IBMVFC_MAX_CHANNELS	502

struct ibmvfc_channel_setup {
	__be32 flags;
#define IBMVFC_CANCEL_CHANNELS		0x01
#define IBMVFC_USE_BUFFER		0x02
#define IBMVFC_CHANNELS_CANCELED	0x04
	__be32 reserved;
	__be32 num_scsi_subq_channels;
	__be32 num_nvmeof_subq_channels;
	__be32 num_scsi_vas_channels;
	__be32 num_nvmeof_vas_channels;
	struct srp_direct_buf buffer;
	__be64 reserved2[5];
	__be64 channel_handles[IBMVFC_MAX_CHANNELS];
} __packed __aligned(8);

struct ibmvfc_connection_info {
	struct ibmvfc_mad_common common;
	__be64 information_bits;
#define IBMVFC_NO_FC_IO_CHANNEL		0x01
#define IBMVFC_NO_PHYP_VAS		0x02
#define IBMVFC_NO_PHYP_SUBQ		0x04
#define IBMVFC_PHYP_DEPRECATED_SUBQ	0x08
#define IBMVFC_PHYP_PRESERVED_SUBQ	0x10
#define IBMVFC_PHYP_FULL_SUBQ		0x20
	__be64 reserved[16];
} __packed __aligned(8);

struct ibmvfc_trace_start_entry {
	u32 xfer_len;
} __packed;

struct ibmvfc_trace_end_entry {
	u16 status;
	u16 error;
	u8 fcp_rsp_flags;
	u8 rsp_code;
	u8 scsi_status;
	u8 reserved;
} __packed;

struct ibmvfc_trace_entry {
	struct ibmvfc_event *evt;
	u32 time;
	u32 scsi_id;
	u32 lun;
	u8 fmt;
	u8 op_code;
	u8 tmf_flags;
	u8 type;
#define IBMVFC_TRC_START	0x00
#define IBMVFC_TRC_END		0xff
	union {
		struct ibmvfc_trace_start_entry start;
		struct ibmvfc_trace_end_entry end;
	} u;
} __packed __aligned(8);

enum ibmvfc_crq_formats {
	IBMVFC_CMD_FORMAT		= 0x01,
	IBMVFC_ASYNC_EVENT	= 0x02,
	IBMVFC_MAD_FORMAT		= 0x04,
};

enum ibmvfc_async_event {
	IBMVFC_AE_ELS_PLOGI		= 0x0001,
	IBMVFC_AE_ELS_LOGO		= 0x0002,
	IBMVFC_AE_ELS_PRLO		= 0x0004,
	IBMVFC_AE_SCN_NPORT		= 0x0008,
	IBMVFC_AE_SCN_GROUP		= 0x0010,
	IBMVFC_AE_SCN_DOMAIN		= 0x0020,
	IBMVFC_AE_SCN_FABRIC		= 0x0040,
	IBMVFC_AE_LINK_UP			= 0x0080,
	IBMVFC_AE_LINK_DOWN		= 0x0100,
	IBMVFC_AE_LINK_DEAD		= 0x0200,
	IBMVFC_AE_HALT			= 0x0400,
	IBMVFC_AE_RESUME			= 0x0800,
	IBMVFC_AE_ADAPTER_FAILED	= 0x1000,
	IBMVFC_AE_FPIN			= 0x2000,
};

struct ibmvfc_async_desc {
	const char *desc;
	enum ibmvfc_async_event ae;
	int log_level;
};

struct ibmvfc_crq {
	volatile u8 valid;
	volatile u8 format;
	u8 reserved[6];
	volatile __be64 ioba;
} __packed __aligned(8);

struct ibmvfc_sub_crq {
	struct ibmvfc_crq crq;
	__be64 reserved[2];
} __packed __aligned(8);

enum ibmvfc_ae_link_state {
	IBMVFC_AE_LS_LINK_UP		= 0x01,
	IBMVFC_AE_LS_LINK_BOUNCED	= 0x02,
	IBMVFC_AE_LS_LINK_DOWN		= 0x04,
	IBMVFC_AE_LS_LINK_DEAD		= 0x08,
};

enum ibmvfc_ae_fpin_status {
	IBMVFC_AE_FPIN_LINK_CONGESTED	= 0x1,
	IBMVFC_AE_FPIN_PORT_CONGESTED	= 0x2,
	IBMVFC_AE_FPIN_PORT_CLEARED	= 0x3,
	IBMVFC_AE_FPIN_PORT_DEGRADED	= 0x4,
};

struct ibmvfc_async_crq {
	volatile u8 valid;
	u8 link_state;
	u8 fpin_status;
	u8 pad;
	__be32 pad2;
	volatile __be64 event;
	volatile __be64 scsi_id;
	volatile __be64 wwpn;
	volatile __be64 node_name;
	__be64 reserved;
} __packed __aligned(8);

union ibmvfc_iu {
	struct ibmvfc_mad_common mad_common;
	struct ibmvfc_npiv_login_mad npiv_login;
	struct ibmvfc_npiv_logout_mad npiv_logout;
	struct ibmvfc_discover_targets discover_targets;
	struct ibmvfc_port_login plogi;
	struct ibmvfc_process_login prli;
	struct ibmvfc_move_login move_login;
	struct ibmvfc_query_tgt query_tgt;
	struct ibmvfc_implicit_logout implicit_logout;
	struct ibmvfc_tmf tmf;
	struct ibmvfc_cmd cmd;
	struct ibmvfc_passthru_mad passthru;
	struct ibmvfc_channel_enquiry channel_enquiry;
	struct ibmvfc_channel_setup_mad channel_setup;
	struct ibmvfc_connection_info connection_info;
} __packed __aligned(8);

enum ibmvfc_target_action {
	IBMVFC_TGT_ACTION_NONE = 0,
	IBMVFC_TGT_ACTION_INIT,
	IBMVFC_TGT_ACTION_INIT_WAIT,
	IBMVFC_TGT_ACTION_LOGOUT_RPORT,
	IBMVFC_TGT_ACTION_LOGOUT_RPORT_WAIT,
	IBMVFC_TGT_ACTION_DEL_RPORT,
	IBMVFC_TGT_ACTION_DELETED_RPORT,
	IBMVFC_TGT_ACTION_DEL_AND_LOGOUT_RPORT,
	IBMVFC_TGT_ACTION_LOGOUT_DELETED_RPORT,
};

struct ibmvfc_target {
	struct list_head queue;
	struct ibmvfc_host *vhost;
	u64 scsi_id;
	u64 wwpn;
	u64 old_scsi_id;
	struct fc_rport *rport;
	int target_id;
	enum ibmvfc_target_action action;
	int need_login;
	int add_rport;
	int init_retries;
	int logo_rcvd;
	u32 cancel_key;
	struct ibmvfc_service_parms service_parms;
	struct ibmvfc_service_parms service_parms_change;
	struct fc_rport_identifiers ids;
	void (*job_step) (struct ibmvfc_target *);
	struct timer_list timer;
	struct kref kref;
};

/* a unit of work for the hosting partition */
struct ibmvfc_event {
	struct list_head queue_list;
	struct list_head cancel;
	struct ibmvfc_host *vhost;
	struct ibmvfc_queue *queue;
	struct ibmvfc_target *tgt;
	struct scsi_cmnd *cmnd;
	atomic_t free;
	union ibmvfc_iu *xfer_iu;
	void (*done)(struct ibmvfc_event *evt);
	void (*_done)(struct ibmvfc_event *evt);
	struct ibmvfc_crq crq;
	union ibmvfc_iu iu;
	union ibmvfc_iu *sync_iu;
	struct srp_direct_buf *ext_list;
	dma_addr_t ext_list_token;
	struct completion comp;
	struct completion *eh_comp;
	struct timer_list timer;
	u16 hwq;
};

/* a pool of event structs for use */
struct ibmvfc_event_pool {
	struct ibmvfc_event *events;
	u32 size;
	union ibmvfc_iu *iu_storage;
	dma_addr_t iu_token;
};

enum ibmvfc_msg_fmt {
	IBMVFC_CRQ_FMT = 0,
	IBMVFC_ASYNC_FMT,
	IBMVFC_SUB_CRQ_FMT,
};

union ibmvfc_msgs {
	void *handle;
	struct ibmvfc_crq *crq;
	struct ibmvfc_async_crq *async;
	struct ibmvfc_sub_crq *scrq;
};

struct ibmvfc_queue {
	union ibmvfc_msgs msgs;
	dma_addr_t msg_token;
	enum ibmvfc_msg_fmt fmt;
	int size, cur;
	spinlock_t _lock;
	spinlock_t *q_lock;

	struct ibmvfc_event_pool evt_pool;
	struct list_head sent;
	struct list_head free;
	spinlock_t l_lock;

	union ibmvfc_iu cancel_rsp;

	/* Sub-CRQ fields */
	struct ibmvfc_host *vhost;
	unsigned long cookie;
	unsigned long vios_cookie;
	unsigned long hw_irq;
	unsigned long irq;
	unsigned long hwq_id;
	char name[32];
};

struct ibmvfc_scsi_channels {
	struct ibmvfc_queue *scrqs;
	unsigned int active_queues;
};

enum ibmvfc_host_action {
	IBMVFC_HOST_ACTION_NONE = 0,
	IBMVFC_HOST_ACTION_RESET,
	IBMVFC_HOST_ACTION_REENABLE,
	IBMVFC_HOST_ACTION_LOGO,
	IBMVFC_HOST_ACTION_LOGO_WAIT,
	IBMVFC_HOST_ACTION_INIT,
	IBMVFC_HOST_ACTION_INIT_WAIT,
	IBMVFC_HOST_ACTION_QUERY,
	IBMVFC_HOST_ACTION_QUERY_TGTS,
	IBMVFC_HOST_ACTION_TGT_DEL,
	IBMVFC_HOST_ACTION_ALLOC_TGTS,
	IBMVFC_HOST_ACTION_TGT_INIT,
	IBMVFC_HOST_ACTION_TGT_DEL_FAILED,
};

enum ibmvfc_host_state {
	IBMVFC_NO_CRQ = 0,
	IBMVFC_INITIALIZING,
	IBMVFC_ACTIVE,
	IBMVFC_HALTED,
	IBMVFC_LINK_DOWN,
	IBMVFC_LINK_DEAD,
	IBMVFC_HOST_OFFLINE,
};

struct ibmvfc_host {
	char name[8];
	struct list_head queue;
	struct Scsi_Host *host;
	enum ibmvfc_host_state state;
	enum ibmvfc_host_action action;
#define IBMVFC_NUM_TRACE_INDEX_BITS		8
#define IBMVFC_NUM_TRACE_ENTRIES		(1 << IBMVFC_NUM_TRACE_INDEX_BITS)
#define IBMVFC_TRACE_INDEX_MASK			(IBMVFC_NUM_TRACE_ENTRIES - 1)
#define IBMVFC_TRACE_SIZE	(sizeof(struct ibmvfc_trace_entry) * IBMVFC_NUM_TRACE_ENTRIES)
	struct ibmvfc_trace_entry *trace;
	atomic_t trace_index;
	int num_targets;
	struct list_head targets;
	struct list_head purge;
	struct device *dev;
	struct dma_pool *sg_pool;
	mempool_t *tgt_pool;
	struct ibmvfc_queue crq;
	struct ibmvfc_queue async_crq;
	struct ibmvfc_scsi_channels scsi_scrqs;
	struct ibmvfc_npiv_login login_info;
	union ibmvfc_npiv_login_data *login_buf;
	dma_addr_t login_buf_dma;
	struct ibmvfc_channel_setup *channel_setup_buf;
	dma_addr_t channel_setup_dma;
	int disc_buf_sz;
	int log_level;
	struct ibmvfc_discover_targets_entry *disc_buf;
	struct mutex passthru_mutex;
	int max_vios_scsi_channels;
	int task_set;
	int init_retries;
	int discovery_threads;
	int abort_threads;
	int client_migrated;
	int reinit;
	int delay_init;
	int scan_complete;
	int logged_in;
	int mq_enabled;
	int using_channels;
	int do_enquiry;
	int client_scsi_channels;
	int aborting_passthru;
	int events_to_log;
#define IBMVFC_AE_LINKUP	0x0001
#define IBMVFC_AE_LINKDOWN	0x0002
#define IBMVFC_AE_RSCN		0x0004
	dma_addr_t disc_buf_dma;
	unsigned int partition_number;
	char partition_name[97];
	void (*job_step) (struct ibmvfc_host *);
	struct task_struct *work_thread;
	struct tasklet_struct tasklet;
	struct work_struct rport_add_work_q;
	wait_queue_head_t init_wait_q;
	wait_queue_head_t work_wait_q;
};

#define DBG_CMD(CMD) do { if (ibmvfc_debug) CMD; } while (0)

#define tgt_dbg(t, fmt, ...)			\
	DBG_CMD(dev_info((t)->vhost->dev, "%llX: " fmt, (t)->scsi_id, ##__VA_ARGS__))

#define tgt_info(t, fmt, ...)		\
	dev_info((t)->vhost->dev, "%llX: " fmt, (t)->scsi_id, ##__VA_ARGS__)

#define tgt_err(t, fmt, ...)		\
	dev_err((t)->vhost->dev, "%llX: " fmt, (t)->scsi_id, ##__VA_ARGS__)

#define tgt_log(t, level, fmt, ...) \
	do { \
		if ((t)->vhost->log_level >= level) \
			tgt_err(t, fmt, ##__VA_ARGS__); \
	} while (0)

#define ibmvfc_dbg(vhost, ...) \
	DBG_CMD(dev_info((vhost)->dev, ##__VA_ARGS__))

#define ibmvfc_log(vhost, level, ...) \
	do { \
		if ((vhost)->log_level >= level) \
			dev_err((vhost)->dev, ##__VA_ARGS__); \
	} while (0)

#define ENTER DBG_CMD(printk(KERN_INFO IBMVFC_NAME": Entering %s\n", __func__))
#define LEAVE DBG_CMD(printk(KERN_INFO IBMVFC_NAME": Leaving %s\n", __func__))

#ifdef CONFIG_SCSI_IBMVFC_TRACE
#define ibmvfc_create_trace_file(kobj, attr) sysfs_create_bin_file(kobj, attr)
#define ibmvfc_remove_trace_file(kobj, attr) sysfs_remove_bin_file(kobj, attr)
#else
#define ibmvfc_create_trace_file(kobj, attr) 0
#define ibmvfc_remove_trace_file(kobj, attr) do { } while (0)
#endif

#endif
