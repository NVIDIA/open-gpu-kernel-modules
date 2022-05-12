/* cnic.h: QLogic CNIC core network driver.
 *
 * Copyright (c) 2006-2014 Broadcom Corporation
 * Copyright (c) 2014 QLogic Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation.
 *
 */


#ifndef CNIC_H
#define CNIC_H

#define HC_INDEX_ISCSI_EQ_CONS			6

#define HC_INDEX_FCOE_EQ_CONS			3

#define HC_SP_INDEX_ETH_ISCSI_CQ_CONS		5
#define HC_SP_INDEX_ETH_ISCSI_RX_CQ_CONS	1

#define KWQ_PAGE_CNT	4
#define KCQ_PAGE_CNT	16

#define KWQ_CID 		24
#define KCQ_CID 		25

/*
 *	krnlq_context definition
 */
#define L5_KRNLQ_FLAGS	0x00000000
#define L5_KRNLQ_SIZE	0x00000000
#define L5_KRNLQ_TYPE	0x00000000
#define KRNLQ_FLAGS_PG_SZ					(0xf<<0)
#define KRNLQ_FLAGS_PG_SZ_256					(0<<0)
#define KRNLQ_FLAGS_PG_SZ_512					(1<<0)
#define KRNLQ_FLAGS_PG_SZ_1K					(2<<0)
#define KRNLQ_FLAGS_PG_SZ_2K					(3<<0)
#define KRNLQ_FLAGS_PG_SZ_4K					(4<<0)
#define KRNLQ_FLAGS_PG_SZ_8K					(5<<0)
#define KRNLQ_FLAGS_PG_SZ_16K					(6<<0)
#define KRNLQ_FLAGS_PG_SZ_32K					(7<<0)
#define KRNLQ_FLAGS_PG_SZ_64K					(8<<0)
#define KRNLQ_FLAGS_PG_SZ_128K					(9<<0)
#define KRNLQ_FLAGS_PG_SZ_256K					(10<<0)
#define KRNLQ_FLAGS_PG_SZ_512K					(11<<0)
#define KRNLQ_FLAGS_PG_SZ_1M					(12<<0)
#define KRNLQ_FLAGS_PG_SZ_2M					(13<<0)
#define KRNLQ_FLAGS_QE_SELF_SEQ					(1<<15)
#define KRNLQ_SIZE_TYPE_SIZE	((((0x28 + 0x1f) & ~0x1f) / 0x20) << 16)
#define KRNLQ_TYPE_TYPE						(0xf<<28)
#define KRNLQ_TYPE_TYPE_EMPTY					(0<<28)
#define KRNLQ_TYPE_TYPE_KRNLQ					(6<<28)

#define L5_KRNLQ_HOST_QIDX		0x00000004
#define L5_KRNLQ_HOST_FW_QIDX		0x00000008
#define L5_KRNLQ_NX_QE_SELF_SEQ 	0x0000000c
#define L5_KRNLQ_QE_SELF_SEQ_MAX	0x0000000c
#define L5_KRNLQ_NX_QE_HADDR_HI 	0x00000010
#define L5_KRNLQ_NX_QE_HADDR_LO 	0x00000014
#define L5_KRNLQ_PGTBL_PGIDX		0x00000018
#define L5_KRNLQ_NX_PG_QIDX 		0x00000018
#define L5_KRNLQ_PGTBL_NPAGES		0x0000001c
#define L5_KRNLQ_QIDX_INCR		0x0000001c
#define L5_KRNLQ_PGTBL_HADDR_HI 	0x00000020
#define L5_KRNLQ_PGTBL_HADDR_LO 	0x00000024

#define BNX2_PG_CTX_MAP			0x1a0034
#define BNX2_ISCSI_CTX_MAP		0x1a0074

#define MAX_COMPLETED_KCQE	64

#define MAX_CNIC_L5_CONTEXT	256

#define MAX_CM_SK_TBL_SZ	MAX_CNIC_L5_CONTEXT

#define MAX_ISCSI_TBL_SZ	256

#define CNIC_LOCAL_PORT_MIN	60000
#define CNIC_LOCAL_PORT_MAX	61024
#define CNIC_LOCAL_PORT_RANGE	(CNIC_LOCAL_PORT_MAX - CNIC_LOCAL_PORT_MIN)

#define KWQE_CNT (BNX2_PAGE_SIZE / sizeof(struct kwqe))
#define KCQE_CNT (BNX2_PAGE_SIZE / sizeof(struct kcqe))
#define MAX_KWQE_CNT (KWQE_CNT - 1)
#define MAX_KCQE_CNT (KCQE_CNT - 1)

#define MAX_KWQ_IDX	((KWQ_PAGE_CNT * KWQE_CNT) - 1)
#define MAX_KCQ_IDX	((KCQ_PAGE_CNT * KCQE_CNT) - 1)

#define KWQ_PG(x) (((x) & ~MAX_KWQE_CNT) >> (BNX2_PAGE_BITS - 5))
#define KWQ_IDX(x) ((x) & MAX_KWQE_CNT)

#define KCQ_PG(x) (((x) & ~MAX_KCQE_CNT) >> (BNX2_PAGE_BITS - 5))
#define KCQ_IDX(x) ((x) & MAX_KCQE_CNT)

#define BNX2X_NEXT_KCQE(x) (((x) & (MAX_KCQE_CNT - 1)) ==		\
		(MAX_KCQE_CNT - 1)) ?					\
		(x) + 2 : (x) + 1

#define BNX2X_KWQ_DATA_PG(cp, x) ((x) / (cp)->kwq_16_data_pp)
#define BNX2X_KWQ_DATA_IDX(cp, x) ((x) % (cp)->kwq_16_data_pp)
#define BNX2X_KWQ_DATA(cp, x)						\
	&(cp)->kwq_16_data[BNX2X_KWQ_DATA_PG(cp, x)][BNX2X_KWQ_DATA_IDX(cp, x)]

#define DEF_IPID_START		0x8000

#define DEF_KA_TIMEOUT		10000
#define DEF_KA_INTERVAL		300000
#define DEF_KA_MAX_PROBE_COUNT	3
#define DEF_TOS			0
#define DEF_TTL			0xfe
#define DEF_SND_SEQ_SCALE	0
#define DEF_RCV_BUF		0xffff
#define DEF_SND_BUF		0xffff
#define DEF_SEED		0
#define DEF_MAX_RT_TIME		500
#define DEF_MAX_DA_COUNT	2
#define DEF_SWS_TIMER		1000
#define DEF_MAX_CWND		0xffff

struct cnic_ctx {
	u32		cid;
	void		*ctx;
	dma_addr_t	mapping;
};

#define BNX2_MAX_CID		0x2000

struct cnic_dma {
	int		num_pages;
	void		**pg_arr;
	dma_addr_t	*pg_map_arr;
	int		pgtbl_size;
	u32		*pgtbl;
	dma_addr_t	pgtbl_map;
};

struct cnic_id_tbl {
	spinlock_t	lock;
	u32		start;
	u32		max;
	u32		next;
	unsigned long	*table;
};

#define CNIC_KWQ16_DATA_SIZE	128

struct kwqe_16_data {
	u8	data[CNIC_KWQ16_DATA_SIZE];
};

struct cnic_iscsi {
	struct cnic_dma		task_array_info;
	struct cnic_dma		r2tq_info;
	struct cnic_dma		hq_info;
};

struct cnic_context {
	u32			cid;
	struct kwqe_16_data	*kwqe_data;
	dma_addr_t		kwqe_data_mapping;
	wait_queue_head_t	waitq;
	int			wait_cond;
	unsigned long		timestamp;
	unsigned long		ctx_flags;
#define	CTX_FL_OFFLD_START	0
#define	CTX_FL_DELETE_WAIT	1
#define	CTX_FL_CID_ERROR	2
	u8			ulp_proto_id;
	union {
		struct cnic_iscsi	*iscsi;
	} proto;
};

struct kcq_info {
	struct cnic_dma	dma;
	struct kcqe	**kcq;

	u16		*hw_prod_idx_ptr;
	u16		sw_prod_idx;
	u16		*status_idx_ptr;
	u32		io_addr;

	u16		(*next_idx)(u16);
	u16		(*hw_idx)(u16);
};

#define UIO_USE_TX_DOORBELL 0x017855DB

struct cnic_uio_dev {
	struct uio_info		cnic_uinfo;
	u32			uio_dev;

	int			l2_ring_size;
	void			*l2_ring;
	dma_addr_t		l2_ring_map;

	int			l2_buf_size;
	void			*l2_buf;
	dma_addr_t		l2_buf_map;

	struct cnic_dev		*dev;
	struct pci_dev		*pdev;
	struct list_head	list;
};

struct cnic_local {

	spinlock_t cnic_ulp_lock;
	void *ulp_handle[MAX_CNIC_ULP_TYPE];
	unsigned long ulp_flags[MAX_CNIC_ULP_TYPE];
#define ULP_F_INIT	0
#define ULP_F_START	1
#define ULP_F_CALL_PENDING	2
	struct cnic_ulp_ops __rcu *ulp_ops[MAX_CNIC_ULP_TYPE];

	unsigned long cnic_local_flags;
#define	CNIC_LCL_FL_KWQ_INIT		0x0
#define	CNIC_LCL_FL_L2_WAIT		0x1
#define	CNIC_LCL_FL_RINGS_INITED	0x2
#define	CNIC_LCL_FL_STOP_ISCSI		0x4

	struct cnic_dev *dev;

	struct cnic_eth_dev *ethdev;

	struct cnic_uio_dev *udev;

	int		l2_rx_ring_size;
	int		l2_single_buf_size;

	u16		*rx_cons_ptr;
	u16		*tx_cons_ptr;
	u16		rx_cons;
	u16		tx_cons;

	struct cnic_dma		kwq_info;
	struct kwqe		**kwq;

	struct cnic_dma		kwq_16_data_info;

	u16		max_kwq_idx;

	u16		kwq_prod_idx;
	u32		kwq_io_addr;

	u16		*kwq_con_idx_ptr;
	u16		kwq_con_idx;

	struct kcq_info	kcq1;
	struct kcq_info	kcq2;

	union {
		void				*gen;
		struct status_block_msix	*bnx2;
		struct host_hc_status_block_e1x	*bnx2x_e1x;
		/* index values - which counter to update */
		#define SM_RX_ID		0
		#define SM_TX_ID		1
	} status_blk;

	struct host_sp_status_block	*bnx2x_def_status_blk;

	u32				status_blk_num;
	u32				bnx2x_igu_sb_id;
	u32				int_num;
	u32				last_status_idx;
	struct tasklet_struct		cnic_irq_task;

	struct kcqe		*completed_kcq[MAX_COMPLETED_KCQE];

	struct cnic_sock	*csk_tbl;
	struct cnic_id_tbl	csk_port_tbl;

	struct cnic_dma		gbl_buf_info;

	struct cnic_iscsi	*iscsi_tbl;
	struct cnic_context	*ctx_tbl;
	struct cnic_id_tbl	cid_tbl;
	atomic_t		iscsi_conn;
	u32			iscsi_start_cid;

	u32			fcoe_init_cid;
	u32			fcoe_start_cid;
	struct cnic_id_tbl	fcoe_cid_tbl;

	u32			max_cid_space;

	/* per connection parameters */
	int			num_iscsi_tasks;
	int			num_ccells;
	int			task_array_size;
	int			r2tq_size;
	int			hq_size;
	int			num_cqs;

	struct delayed_work	delete_task;

	struct cnic_ctx		*ctx_arr;
	int			ctx_blks;
	int			ctx_blk_size;
	unsigned long		ctx_align;
	int			cids_per_blk;

	u32			chip_id;
	int			func;

	u32			shmem_base;

	struct cnic_ops		*cnic_ops;
	int			(*start_hw)(struct cnic_dev *);
	void			(*stop_hw)(struct cnic_dev *);
	void			(*setup_pgtbl)(struct cnic_dev *,
					       struct cnic_dma *);
	int			(*alloc_resc)(struct cnic_dev *);
	void			(*free_resc)(struct cnic_dev *);
	int			(*start_cm)(struct cnic_dev *);
	void			(*stop_cm)(struct cnic_dev *);
	void			(*enable_int)(struct cnic_dev *);
	void			(*disable_int_sync)(struct cnic_dev *);
	void			(*ack_int)(struct cnic_dev *);
	void			(*arm_int)(struct cnic_dev *, u32 index);
	void			(*close_conn)(struct cnic_sock *, u32 opcode);
};

struct bnx2x_bd_chain_next {
	u32	addr_lo;
	u32	addr_hi;
	u8	reserved[8];
};

#define ISCSI_DEFAULT_MAX_OUTSTANDING_R2T 	(1)

#define ISCSI_RAMROD_CMD_ID_UPDATE_CONN		(ISCSI_KCQE_OPCODE_UPDATE_CONN)
#define ISCSI_RAMROD_CMD_ID_INIT		(ISCSI_KCQE_OPCODE_INIT)

#define CDU_REGION_NUMBER_XCM_AG 2
#define CDU_REGION_NUMBER_UCM_AG 4

#define CDU_VALID_DATA(_cid, _region, _type)	\
	(((_cid) << 8) | (((_region)&0xf)<<4) | (((_type)&0xf)))

#define CDU_CRC8(_cid, _region, _type)	\
	(calc_crc8(CDU_VALID_DATA(_cid, _region, _type), 0xff))

#define CDU_RSRVD_VALUE_TYPE_A(_cid, _region, _type)	\
	(0x80 | ((CDU_CRC8(_cid, _region, _type)) & 0x7f))

#define BNX2X_CONTEXT_MEM_SIZE		1024
#define BNX2X_FCOE_CID			16

#define BNX2X_ISCSI_START_CID		18
#define BNX2X_ISCSI_NUM_CONNECTIONS	128
#define BNX2X_ISCSI_TASK_CONTEXT_SIZE	128
#define BNX2X_ISCSI_MAX_PENDING_R2TS	4
#define BNX2X_ISCSI_R2TQE_SIZE		8
#define BNX2X_ISCSI_HQ_BD_SIZE		64
#define BNX2X_ISCSI_GLB_BUF_SIZE	64
#define BNX2X_ISCSI_PBL_NOT_CACHED	0xff
#define BNX2X_ISCSI_PDU_HEADER_NOT_CACHED	0xff

#define BNX2X_FCOE_NUM_CONNECTIONS	1024

#define BNX2X_FCOE_L5_CID_BASE		MAX_ISCSI_TBL_SZ

#define BNX2X_CHIP_IS_E2_PLUS(bp) (CHIP_IS_E2(bp) || CHIP_IS_E3(bp))

#define BNX2X_RX_DESC_CNT		(BNX2_PAGE_SIZE / \
					 sizeof(struct eth_rx_bd))
#define BNX2X_MAX_RX_DESC_CNT		(BNX2X_RX_DESC_CNT - 2)
#define BNX2X_RCQ_DESC_CNT		(BNX2_PAGE_SIZE / \
					 sizeof(union eth_rx_cqe))
#define BNX2X_MAX_RCQ_DESC_CNT		(BNX2X_RCQ_DESC_CNT - 1)

#define BNX2X_NEXT_RCQE(x) (((x) & BNX2X_MAX_RCQ_DESC_CNT) ==		\
		(BNX2X_MAX_RCQ_DESC_CNT - 1)) ?				\
		((x) + 2) : ((x) + 1)

#define BNX2X_DEF_SB_ID			HC_SP_SB_ID

#define BNX2X_SHMEM_MF_BLK_OFFSET	0x7e4

#define BNX2X_SHMEM_ADDR(base, field)	(base + \
					 offsetof(struct shmem_region, field))

#define BNX2X_SHMEM2_ADDR(base, field)	(base + \
					 offsetof(struct shmem2_region, field))

#define BNX2X_SHMEM2_HAS(base, field)				\
		((base) &&					\
		 (CNIC_RD(dev, BNX2X_SHMEM2_ADDR(base, size)) >	\
		  offsetof(struct shmem2_region, field)))

#define BNX2X_MF_CFG_ADDR(base, field)				\
			((base) + offsetof(struct mf_cfg, field))

#ifndef ETH_MAX_RX_CLIENTS_E2
#define ETH_MAX_RX_CLIENTS_E2 		ETH_MAX_RX_CLIENTS_E1H
#endif

#define CNIC_FUNC(cp)			((cp)->func)

#define BNX2X_HW_CID(bp, x)		((BP_PORT(bp) << 23) | \
					 (BP_VN(bp) << 17) | (x))

#define BNX2X_SW_CID(x)			(x & 0x1ffff)

#define BNX2X_CL_QZONE_ID(bp, cli)					\
		(BNX2X_CHIP_IS_E2_PLUS(bp) ? cli :			\
		 cli + (BP_PORT(bp) * ETH_MAX_RX_CLIENTS_E1H))

#ifndef MAX_STAT_COUNTER_ID
#define MAX_STAT_COUNTER_ID						\
	(CHIP_IS_E1H(bp) ? MAX_STAT_COUNTER_ID_E1H :			\
	 ((BNX2X_CHIP_IS_E2_PLUS(bp)) ? MAX_STAT_COUNTER_ID_E2 :	\
	  MAX_STAT_COUNTER_ID_E1))
#endif

#define CNIC_SUPPORTS_FCOE(cp)						\
	(BNX2X_CHIP_IS_E2_PLUS(bp) && !NO_FCOE(bp))

#define CNIC_RAMROD_TMO			(HZ / 4)

#endif

