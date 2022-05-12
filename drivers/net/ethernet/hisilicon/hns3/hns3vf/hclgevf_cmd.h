/* SPDX-License-Identifier: GPL-2.0+ */
/* Copyright (c) 2016-2017 Hisilicon Limited. */

#ifndef __HCLGEVF_CMD_H
#define __HCLGEVF_CMD_H
#include <linux/io.h>
#include <linux/types.h>
#include "hnae3.h"

#define HCLGEVF_CMDQ_TX_TIMEOUT		30000
#define HCLGEVF_CMDQ_RX_INVLD_B		0
#define HCLGEVF_CMDQ_RX_OUTVLD_B	1

struct hclgevf_hw;
struct hclgevf_dev;

struct hclgevf_desc {
	__le16 opcode;
	__le16 flag;
	__le16 retval;
	__le16 rsv;
	__le32 data[6];
};

struct hclgevf_desc_cb {
	dma_addr_t dma;
	void *va;
	u32 length;
};

struct hclgevf_cmq_ring {
	dma_addr_t desc_dma_addr;
	struct hclgevf_desc *desc;
	struct hclgevf_desc_cb *desc_cb;
	struct hclgevf_dev  *dev;
	u32 head;
	u32 tail;

	u16 buf_size;
	u16 desc_num;
	int next_to_use;
	int next_to_clean;
	u8 flag;
	spinlock_t lock; /* Command queue lock */
};

enum hclgevf_cmd_return_status {
	HCLGEVF_CMD_EXEC_SUCCESS	= 0,
	HCLGEVF_CMD_NO_AUTH		= 1,
	HCLGEVF_CMD_NOT_SUPPORTED	= 2,
	HCLGEVF_CMD_QUEUE_FULL		= 3,
	HCLGEVF_CMD_NEXT_ERR		= 4,
	HCLGEVF_CMD_UNEXE_ERR		= 5,
	HCLGEVF_CMD_PARA_ERR		= 6,
	HCLGEVF_CMD_RESULT_ERR		= 7,
	HCLGEVF_CMD_TIMEOUT		= 8,
	HCLGEVF_CMD_HILINK_ERR		= 9,
	HCLGEVF_CMD_QUEUE_ILLEGAL	= 10,
	HCLGEVF_CMD_INVALID		= 11,
};

enum hclgevf_cmd_status {
	HCLGEVF_STATUS_SUCCESS	= 0,
	HCLGEVF_ERR_CSQ_FULL	= -1,
	HCLGEVF_ERR_CSQ_TIMEOUT	= -2,
	HCLGEVF_ERR_CSQ_ERROR	= -3
};

struct hclgevf_cmq {
	struct hclgevf_cmq_ring csq;
	struct hclgevf_cmq_ring crq;
	u16 tx_timeout; /* Tx timeout */
	enum hclgevf_cmd_status last_status;
};

#define HCLGEVF_CMD_FLAG_IN_VALID_SHIFT		0
#define HCLGEVF_CMD_FLAG_OUT_VALID_SHIFT	1
#define HCLGEVF_CMD_FLAG_NEXT_SHIFT		2
#define HCLGEVF_CMD_FLAG_WR_OR_RD_SHIFT		3
#define HCLGEVF_CMD_FLAG_NO_INTR_SHIFT		4
#define HCLGEVF_CMD_FLAG_ERR_INTR_SHIFT		5

#define HCLGEVF_CMD_FLAG_IN		BIT(HCLGEVF_CMD_FLAG_IN_VALID_SHIFT)
#define HCLGEVF_CMD_FLAG_OUT		BIT(HCLGEVF_CMD_FLAG_OUT_VALID_SHIFT)
#define HCLGEVF_CMD_FLAG_NEXT		BIT(HCLGEVF_CMD_FLAG_NEXT_SHIFT)
#define HCLGEVF_CMD_FLAG_WR		BIT(HCLGEVF_CMD_FLAG_WR_OR_RD_SHIFT)
#define HCLGEVF_CMD_FLAG_NO_INTR	BIT(HCLGEVF_CMD_FLAG_NO_INTR_SHIFT)
#define HCLGEVF_CMD_FLAG_ERR_INTR	BIT(HCLGEVF_CMD_FLAG_ERR_INTR_SHIFT)

enum hclgevf_opcode_type {
	/* Generic command */
	HCLGEVF_OPC_QUERY_FW_VER	= 0x0001,
	HCLGEVF_OPC_QUERY_VF_RSRC	= 0x0024,
	HCLGEVF_OPC_QUERY_DEV_SPECS	= 0x0050,

	/* TQP command */
	HCLGEVF_OPC_QUERY_TX_STATUS	= 0x0B03,
	HCLGEVF_OPC_QUERY_RX_STATUS	= 0x0B13,
	HCLGEVF_OPC_CFG_COM_TQP_QUEUE	= 0x0B20,
	/* GRO command */
	HCLGEVF_OPC_GRO_GENERIC_CONFIG  = 0x0C10,
	/* RSS cmd */
	HCLGEVF_OPC_RSS_GENERIC_CONFIG	= 0x0D01,
	HCLGEVF_OPC_RSS_INPUT_TUPLE     = 0x0D02,
	HCLGEVF_OPC_RSS_INDIR_TABLE	= 0x0D07,
	HCLGEVF_OPC_RSS_TC_MODE		= 0x0D08,
	/* Mailbox cmd */
	HCLGEVF_OPC_MBX_VF_TO_PF	= 0x2001,
};

#define HCLGEVF_TQP_REG_OFFSET		0x80000
#define HCLGEVF_TQP_REG_SIZE		0x200

#define HCLGEVF_TQP_MAX_SIZE_DEV_V2	1024
#define HCLGEVF_TQP_EXT_REG_OFFSET	0x100

struct hclgevf_tqp_map {
	__le16 tqp_id;	/* Absolute tqp id for in this pf */
	u8 tqp_vf; /* VF id */
#define HCLGEVF_TQP_MAP_TYPE_PF		0
#define HCLGEVF_TQP_MAP_TYPE_VF		1
#define HCLGEVF_TQP_MAP_TYPE_B		0
#define HCLGEVF_TQP_MAP_EN_B		1
	u8 tqp_flag;	/* Indicate it's pf or vf tqp */
	__le16 tqp_vid; /* Virtual id in this pf/vf */
	u8 rsv[18];
};

#define HCLGEVF_VECTOR_ELEMENTS_PER_CMD	10

enum hclgevf_int_type {
	HCLGEVF_INT_TX = 0,
	HCLGEVF_INT_RX,
	HCLGEVF_INT_EVENT,
};

struct hclgevf_ctrl_vector_chain {
	u8 int_vector_id;
	u8 int_cause_num;
#define HCLGEVF_INT_TYPE_S	0
#define HCLGEVF_INT_TYPE_M	0x3
#define HCLGEVF_TQP_ID_S	2
#define HCLGEVF_TQP_ID_M	(0x3fff << HCLGEVF_TQP_ID_S)
	__le16 tqp_type_and_id[HCLGEVF_VECTOR_ELEMENTS_PER_CMD];
	u8 vfid;
	u8 resv;
};

enum HCLGEVF_CAP_BITS {
	HCLGEVF_CAP_UDP_GSO_B,
	HCLGEVF_CAP_QB_B,
	HCLGEVF_CAP_FD_FORWARD_TC_B,
	HCLGEVF_CAP_PTP_B,
	HCLGEVF_CAP_INT_QL_B,
	HCLGEVF_CAP_HW_TX_CSUM_B,
	HCLGEVF_CAP_TX_PUSH_B,
	HCLGEVF_CAP_PHY_IMP_B,
	HCLGEVF_CAP_TQP_TXRX_INDEP_B,
	HCLGEVF_CAP_HW_PAD_B,
	HCLGEVF_CAP_STASH_B,
	HCLGEVF_CAP_UDP_TUNNEL_CSUM_B,
};

enum HCLGEVF_API_CAP_BITS {
	HCLGEVF_API_CAP_FLEX_RSS_TBL_B,
};

#define HCLGEVF_QUERY_CAP_LENGTH		3
struct hclgevf_query_version_cmd {
	__le32 firmware;
	__le32 hardware;
	__le32 api_caps;
	__le32 caps[HCLGEVF_QUERY_CAP_LENGTH]; /* capabilities of device */
};

#define HCLGEVF_MSIX_OFT_ROCEE_S       0
#define HCLGEVF_MSIX_OFT_ROCEE_M       (0xffff << HCLGEVF_MSIX_OFT_ROCEE_S)
#define HCLGEVF_VEC_NUM_S              0
#define HCLGEVF_VEC_NUM_M              (0xff << HCLGEVF_VEC_NUM_S)
struct hclgevf_query_res_cmd {
	__le16 tqp_num;
	__le16 reserved;
	__le16 msixcap_localid_ba_nic;
	__le16 msixcap_localid_ba_rocee;
	__le16 vf_intr_vector_number;
	__le16 rsv[7];
};

#define HCLGEVF_GRO_EN_B               0
struct hclgevf_cfg_gro_status_cmd {
	u8 gro_en;
	u8 rsv[23];
};

#define HCLGEVF_RSS_DEFAULT_OUTPORT_B	4
#define HCLGEVF_RSS_HASH_KEY_OFFSET_B	4
#define HCLGEVF_RSS_HASH_KEY_NUM	16
struct hclgevf_rss_config_cmd {
	u8 hash_config;
	u8 rsv[7];
	u8 hash_key[HCLGEVF_RSS_HASH_KEY_NUM];
};

struct hclgevf_rss_input_tuple_cmd {
	u8 ipv4_tcp_en;
	u8 ipv4_udp_en;
	u8 ipv4_sctp_en;
	u8 ipv4_fragment_en;
	u8 ipv6_tcp_en;
	u8 ipv6_udp_en;
	u8 ipv6_sctp_en;
	u8 ipv6_fragment_en;
	u8 rsv[16];
};

#define HCLGEVF_RSS_CFG_TBL_SIZE	16

struct hclgevf_rss_indirection_table_cmd {
	__le16 start_table_index;
	__le16 rss_set_bitmap;
	u8 rsv[4];
	u8 rss_result[HCLGEVF_RSS_CFG_TBL_SIZE];
};

#define HCLGEVF_RSS_TC_OFFSET_S		0
#define HCLGEVF_RSS_TC_OFFSET_M		GENMASK(10, 0)
#define HCLGEVF_RSS_TC_SIZE_MSB_B	11
#define HCLGEVF_RSS_TC_SIZE_S		12
#define HCLGEVF_RSS_TC_SIZE_M		GENMASK(14, 12)
#define HCLGEVF_RSS_TC_VALID_B		15
#define HCLGEVF_MAX_TC_NUM		8
#define HCLGEVF_RSS_TC_SIZE_MSB_OFFSET	3

struct hclgevf_rss_tc_mode_cmd {
	__le16 rss_tc_mode[HCLGEVF_MAX_TC_NUM];
	u8 rsv[8];
};

#define HCLGEVF_LINK_STS_B	0
#define HCLGEVF_LINK_STATUS	BIT(HCLGEVF_LINK_STS_B)
struct hclgevf_link_status_cmd {
	u8 status;
	u8 rsv[23];
};

#define HCLGEVF_RING_ID_MASK	0x3ff
#define HCLGEVF_TQP_ENABLE_B	0

struct hclgevf_cfg_com_tqp_queue_cmd {
	__le16 tqp_id;
	__le16 stream_id;
	u8 enable;
	u8 rsv[19];
};

struct hclgevf_cfg_tx_queue_pointer_cmd {
	__le16 tqp_id;
	__le16 tx_tail;
	__le16 tx_head;
	__le16 fbd_num;
	__le16 ring_offset;
	u8 rsv[14];
};

#define HCLGEVF_TYPE_CRQ		0
#define HCLGEVF_TYPE_CSQ		1
#define HCLGEVF_NIC_CSQ_BASEADDR_L_REG	0x27000
#define HCLGEVF_NIC_CSQ_BASEADDR_H_REG	0x27004
#define HCLGEVF_NIC_CSQ_DEPTH_REG	0x27008
#define HCLGEVF_NIC_CSQ_TAIL_REG	0x27010
#define HCLGEVF_NIC_CSQ_HEAD_REG	0x27014
#define HCLGEVF_NIC_CRQ_BASEADDR_L_REG	0x27018
#define HCLGEVF_NIC_CRQ_BASEADDR_H_REG	0x2701c
#define HCLGEVF_NIC_CRQ_DEPTH_REG	0x27020
#define HCLGEVF_NIC_CRQ_TAIL_REG	0x27024
#define HCLGEVF_NIC_CRQ_HEAD_REG	0x27028

/* this bit indicates that the driver is ready for hardware reset */
#define HCLGEVF_NIC_SW_RST_RDY_B	16
#define HCLGEVF_NIC_SW_RST_RDY		BIT(HCLGEVF_NIC_SW_RST_RDY_B)

#define HCLGEVF_NIC_CMQ_DESC_NUM	1024
#define HCLGEVF_NIC_CMQ_DESC_NUM_S	3

#define HCLGEVF_QUERY_DEV_SPECS_BD_NUM		4

struct hclgevf_dev_specs_0_cmd {
	__le32 rsv0;
	__le32 mac_entry_num;
	__le32 mng_entry_num;
	__le16 rss_ind_tbl_size;
	__le16 rss_key_size;
	__le16 int_ql_max;
	u8 max_non_tso_bd_num;
	u8 rsv1[5];
};

#define HCLGEVF_DEF_MAX_INT_GL		0x1FE0U

struct hclgevf_dev_specs_1_cmd {
	__le16 max_frm_size;
	__le16 rsv0;
	__le16 max_int_gl;
	u8 rsv1[18];
};

static inline void hclgevf_write_reg(void __iomem *base, u32 reg, u32 value)
{
	writel(value, base + reg);
}

static inline u32 hclgevf_read_reg(u8 __iomem *base, u32 reg)
{
	u8 __iomem *reg_addr = READ_ONCE(base);

	return readl(reg_addr + reg);
}

#define hclgevf_write_dev(a, reg, value) \
	hclgevf_write_reg((a)->io_base, reg, value)
#define hclgevf_read_dev(a, reg) \
	hclgevf_read_reg((a)->io_base, reg)

#define HCLGEVF_SEND_SYNC(flag) \
	((flag) & HCLGEVF_CMD_FLAG_NO_INTR)

int hclgevf_cmd_init(struct hclgevf_dev *hdev);
void hclgevf_cmd_uninit(struct hclgevf_dev *hdev);
int hclgevf_cmd_queue_init(struct hclgevf_dev *hdev);

int hclgevf_cmd_send(struct hclgevf_hw *hw, struct hclgevf_desc *desc, int num);
void hclgevf_cmd_setup_basic_desc(struct hclgevf_desc *desc,
				  enum hclgevf_opcode_type opcode,
				  bool is_read);
#endif
