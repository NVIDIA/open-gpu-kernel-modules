/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * AMD Cryptographic Coprocessor (CCP) driver
 *
 * Copyright (C) 2013,2017 Advanced Micro Devices, Inc.
 *
 * Author: Tom Lendacky <thomas.lendacky@amd.com>
 * Author: Gary R Hook <gary.hook@amd.com>
 */

#ifndef __CCP_DEV_H__
#define __CCP_DEV_H__

#include <linux/device.h>
#include <linux/spinlock.h>
#include <linux/mutex.h>
#include <linux/list.h>
#include <linux/wait.h>
#include <linux/dma-direction.h>
#include <linux/dmapool.h>
#include <linux/hw_random.h>
#include <linux/bitops.h>
#include <linux/interrupt.h>
#include <linux/irqreturn.h>
#include <linux/dmaengine.h>

#include "sp-dev.h"

#define MAX_CCP_NAME_LEN		16
#define MAX_DMAPOOL_NAME_LEN		32

#define MAX_HW_QUEUES			5
#define MAX_CMD_QLEN			100

#define TRNG_RETRIES			10

#define CACHE_NONE			0x00
#define CACHE_WB_NO_ALLOC		0xb7

/****** Register Mappings ******/
#define Q_MASK_REG			0x000
#define TRNG_OUT_REG			0x00c
#define IRQ_MASK_REG			0x040
#define IRQ_STATUS_REG			0x200

#define DEL_CMD_Q_JOB			0x124
#define DEL_Q_ACTIVE			0x00000200
#define DEL_Q_ID_SHIFT			6

#define CMD_REQ0			0x180
#define CMD_REQ_INCR			0x04

#define CMD_Q_STATUS_BASE		0x210
#define CMD_Q_INT_STATUS_BASE		0x214
#define CMD_Q_STATUS_INCR		0x20

#define CMD_Q_CACHE_BASE		0x228
#define CMD_Q_CACHE_INC			0x20

#define CMD_Q_ERROR(__qs)		((__qs) & 0x0000003f)
#define CMD_Q_DEPTH(__qs)		(((__qs) >> 12) & 0x0000000f)

/* ------------------------ CCP Version 5 Specifics ------------------------ */
#define CMD5_QUEUE_MASK_OFFSET		0x00
#define	CMD5_QUEUE_PRIO_OFFSET		0x04
#define CMD5_REQID_CONFIG_OFFSET	0x08
#define	CMD5_CMD_TIMEOUT_OFFSET		0x10
#define LSB_PUBLIC_MASK_LO_OFFSET	0x18
#define LSB_PUBLIC_MASK_HI_OFFSET	0x1C
#define LSB_PRIVATE_MASK_LO_OFFSET	0x20
#define LSB_PRIVATE_MASK_HI_OFFSET	0x24
#define CMD5_PSP_CCP_VERSION		0x100

#define CMD5_Q_CONTROL_BASE		0x0000
#define CMD5_Q_TAIL_LO_BASE		0x0004
#define CMD5_Q_HEAD_LO_BASE		0x0008
#define CMD5_Q_INT_ENABLE_BASE		0x000C
#define CMD5_Q_INTERRUPT_STATUS_BASE	0x0010

#define CMD5_Q_STATUS_BASE		0x0100
#define CMD5_Q_INT_STATUS_BASE		0x0104
#define CMD5_Q_DMA_STATUS_BASE		0x0108
#define CMD5_Q_DMA_READ_STATUS_BASE	0x010C
#define CMD5_Q_DMA_WRITE_STATUS_BASE	0x0110
#define CMD5_Q_ABORT_BASE		0x0114
#define CMD5_Q_AX_CACHE_BASE		0x0118

#define	CMD5_CONFIG_0_OFFSET		0x6000
#define	CMD5_TRNG_CTL_OFFSET		0x6008
#define	CMD5_AES_MASK_OFFSET		0x6010
#define	CMD5_CLK_GATE_CTL_OFFSET	0x603C

/* Address offset between two virtual queue registers */
#define CMD5_Q_STATUS_INCR		0x1000

/* Bit masks */
#define CMD5_Q_RUN			0x1
#define CMD5_Q_HALT			0x2
#define CMD5_Q_MEM_LOCATION		0x4
#define CMD5_Q_SIZE			0x1F
#define CMD5_Q_SHIFT			3
#define COMMANDS_PER_QUEUE		16
#define QUEUE_SIZE_VAL			((ffs(COMMANDS_PER_QUEUE) - 2) & \
					  CMD5_Q_SIZE)
#define Q_PTR_MASK			(2 << (QUEUE_SIZE_VAL + 5) - 1)
#define Q_DESC_SIZE			sizeof(struct ccp5_desc)
#define Q_SIZE(n)			(COMMANDS_PER_QUEUE*(n))

#define INT_COMPLETION			0x1
#define INT_ERROR			0x2
#define INT_QUEUE_STOPPED		0x4
#define	INT_EMPTY_QUEUE			0x8
#define SUPPORTED_INTERRUPTS		(INT_COMPLETION | INT_ERROR)

#define LSB_REGION_WIDTH		5
#define MAX_LSB_CNT			8

#define LSB_SIZE			16
#define LSB_ITEM_SIZE			32
#define PLSB_MAP_SIZE			(LSB_SIZE)
#define SLSB_MAP_SIZE			(MAX_LSB_CNT * LSB_SIZE)

#define LSB_ENTRY_NUMBER(LSB_ADDR)	(LSB_ADDR / LSB_ITEM_SIZE)

/* ------------------------ CCP Version 3 Specifics ------------------------ */
#define REQ0_WAIT_FOR_WRITE		0x00000004
#define REQ0_INT_ON_COMPLETE		0x00000002
#define REQ0_STOP_ON_COMPLETE		0x00000001

#define REQ0_CMD_Q_SHIFT		9
#define REQ0_JOBID_SHIFT		3

/****** REQ1 Related Values ******/
#define REQ1_PROTECT_SHIFT		27
#define REQ1_ENGINE_SHIFT		23
#define REQ1_KEY_KSB_SHIFT		2

#define REQ1_EOM			0x00000002
#define REQ1_INIT			0x00000001

/* AES Related Values */
#define REQ1_AES_TYPE_SHIFT		21
#define REQ1_AES_MODE_SHIFT		18
#define REQ1_AES_ACTION_SHIFT		17
#define REQ1_AES_CFB_SIZE_SHIFT		10

/* XTS-AES Related Values */
#define REQ1_XTS_AES_SIZE_SHIFT		10

/* SHA Related Values */
#define REQ1_SHA_TYPE_SHIFT		21

/* RSA Related Values */
#define REQ1_RSA_MOD_SIZE_SHIFT		10

/* Pass-Through Related Values */
#define REQ1_PT_BW_SHIFT		12
#define REQ1_PT_BS_SHIFT		10

/* ECC Related Values */
#define REQ1_ECC_AFFINE_CONVERT		0x00200000
#define REQ1_ECC_FUNCTION_SHIFT		18

/****** REQ4 Related Values ******/
#define REQ4_KSB_SHIFT			18
#define REQ4_MEMTYPE_SHIFT		16

/****** REQ6 Related Values ******/
#define REQ6_MEMTYPE_SHIFT		16

/****** Key Storage Block ******/
#define KSB_START			77
#define KSB_END				127
#define KSB_COUNT			(KSB_END - KSB_START + 1)
#define CCP_SB_BITS			256

#define CCP_JOBID_MASK			0x0000003f

/* ------------------------ General CCP Defines ------------------------ */

#define	CCP_DMA_DFLT			0x0
#define	CCP_DMA_PRIV			0x1
#define	CCP_DMA_PUB			0x2

#define CCP_DMAPOOL_MAX_SIZE		64
#define CCP_DMAPOOL_ALIGN		BIT(5)

#define CCP_REVERSE_BUF_SIZE		64

#define CCP_AES_KEY_SB_COUNT		1
#define CCP_AES_CTX_SB_COUNT		1

#define CCP_XTS_AES_KEY_SB_COUNT	1
#define CCP5_XTS_AES_KEY_SB_COUNT	2
#define CCP_XTS_AES_CTX_SB_COUNT	1

#define CCP_DES3_KEY_SB_COUNT		1
#define CCP_DES3_CTX_SB_COUNT		1

#define CCP_SHA_SB_COUNT		1

#define CCP_RSA_MAX_WIDTH		4096
#define CCP5_RSA_MAX_WIDTH		16384

#define CCP_PASSTHRU_BLOCKSIZE		256
#define CCP_PASSTHRU_MASKSIZE		32
#define CCP_PASSTHRU_SB_COUNT		1

#define CCP_ECC_MODULUS_BYTES		48      /* 384-bits */
#define CCP_ECC_MAX_OPERANDS		6
#define CCP_ECC_MAX_OUTPUTS		3
#define CCP_ECC_SRC_BUF_SIZE		448
#define CCP_ECC_DST_BUF_SIZE		192
#define CCP_ECC_OPERAND_SIZE		64
#define CCP_ECC_OUTPUT_SIZE		64
#define CCP_ECC_RESULT_OFFSET		60
#define CCP_ECC_RESULT_SUCCESS		0x0001

#define CCP_SB_BYTES			32

struct ccp_op;
struct ccp_device;
struct ccp_cmd;
struct ccp_fns;

struct ccp_dma_cmd {
	struct list_head entry;

	struct ccp_cmd ccp_cmd;
};

struct ccp_dma_desc {
	struct list_head entry;

	struct ccp_device *ccp;

	struct list_head pending;
	struct list_head active;

	enum dma_status status;
	struct dma_async_tx_descriptor tx_desc;
	size_t len;
};

struct ccp_dma_chan {
	struct ccp_device *ccp;

	spinlock_t lock;
	struct list_head created;
	struct list_head pending;
	struct list_head active;
	struct list_head complete;

	struct tasklet_struct cleanup_tasklet;

	enum dma_status status;
	struct dma_chan dma_chan;
};

struct ccp_cmd_queue {
	struct ccp_device *ccp;

	/* Queue identifier */
	u32 id;

	/* Queue dma pool */
	struct dma_pool *dma_pool;

	/* Queue base address (not neccessarily aligned)*/
	struct ccp5_desc *qbase;

	/* Aligned queue start address (per requirement) */
	struct mutex q_mutex ____cacheline_aligned;
	unsigned int qidx;

	/* Version 5 has different requirements for queue memory */
	unsigned int qsize;
	dma_addr_t qbase_dma;
	dma_addr_t qdma_tail;

	/* Per-queue reserved storage block(s) */
	u32 sb_key;
	u32 sb_ctx;

	/* Bitmap of LSBs that can be accessed by this queue */
	DECLARE_BITMAP(lsbmask, MAX_LSB_CNT);
	/* Private LSB that is assigned to this queue, or -1 if none.
	 * Bitmap for my private LSB, unused otherwise
	 */
	int lsb;
	DECLARE_BITMAP(lsbmap, PLSB_MAP_SIZE);

	/* Queue processing thread */
	struct task_struct *kthread;
	unsigned int active;
	unsigned int suspended;

	/* Number of free command slots available */
	unsigned int free_slots;

	/* Interrupt masks */
	u32 int_ok;
	u32 int_err;

	/* Register addresses for queue */
	void __iomem *reg_control;
	void __iomem *reg_tail_lo;
	void __iomem *reg_head_lo;
	void __iomem *reg_int_enable;
	void __iomem *reg_interrupt_status;
	void __iomem *reg_status;
	void __iomem *reg_int_status;
	void __iomem *reg_dma_status;
	void __iomem *reg_dma_read_status;
	void __iomem *reg_dma_write_status;
	u32 qcontrol; /* Cached control register */

	/* Status values from job */
	u32 int_status;
	u32 q_status;
	u32 q_int_status;
	u32 cmd_error;

	/* Interrupt wait queue */
	wait_queue_head_t int_queue;
	unsigned int int_rcvd;

	/* Per-queue Statistics */
	unsigned long total_ops;
	unsigned long total_aes_ops;
	unsigned long total_xts_aes_ops;
	unsigned long total_3des_ops;
	unsigned long total_sha_ops;
	unsigned long total_rsa_ops;
	unsigned long total_pt_ops;
	unsigned long total_ecc_ops;
} ____cacheline_aligned;

struct ccp_device {
	struct list_head entry;

	struct ccp_vdata *vdata;
	unsigned int ord;
	char name[MAX_CCP_NAME_LEN];
	char rngname[MAX_CCP_NAME_LEN];

	struct device *dev;
	struct sp_device *sp;

	/* Bus specific device information
	 */
	void *dev_specific;
	unsigned int qim;
	unsigned int irq;
	bool use_tasklet;
	struct tasklet_struct irq_tasklet;

	/* I/O area used for device communication. The register mapping
	 * starts at an offset into the mapped bar.
	 *   The CMD_REQx registers and the Delete_Cmd_Queue_Job register
	 *   need to be protected while a command queue thread is accessing
	 *   them.
	 */
	struct mutex req_mutex ____cacheline_aligned;
	void __iomem *io_regs;

	/* Master lists that all cmds are queued on. Because there can be
	 * more than one CCP command queue that can process a cmd a separate
	 * backlog list is neeeded so that the backlog completion call
	 * completes before the cmd is available for execution.
	 */
	spinlock_t cmd_lock ____cacheline_aligned;
	unsigned int cmd_count;
	struct list_head cmd;
	struct list_head backlog;

	/* The command queues. These represent the queues available on the
	 * CCP that are available for processing cmds
	 */
	struct ccp_cmd_queue cmd_q[MAX_HW_QUEUES];
	unsigned int cmd_q_count;
	unsigned int max_q_count;

	/* Support for the CCP True RNG
	 */
	struct hwrng hwrng;
	unsigned int hwrng_retries;

	/* Support for the CCP DMA capabilities
	 */
	struct dma_device dma_dev;
	struct ccp_dma_chan *ccp_dma_chan;
	struct kmem_cache *dma_cmd_cache;
	struct kmem_cache *dma_desc_cache;

	/* A counter used to generate job-ids for cmds submitted to the CCP
	 */
	atomic_t current_id ____cacheline_aligned;

	/* The v3 CCP uses key storage blocks (SB) to maintain context for
	 * certain operations. To prevent multiple cmds from using the same
	 * SB range a command queue reserves an SB range for the duration of
	 * the cmd. Each queue, will however, reserve 2 SB blocks for
	 * operations that only require single SB entries (eg. AES context/iv
	 * and key) in order to avoid allocation contention.  This will reserve
	 * at most 10 SB entries, leaving 40 SB entries available for dynamic
	 * allocation.
	 *
	 * The v5 CCP Local Storage Block (LSB) is broken up into 8
	 * memrory ranges, each of which can be enabled for access by one
	 * or more queues. Device initialization takes this into account,
	 * and attempts to assign one region for exclusive use by each
	 * available queue; the rest are then aggregated as "public" use.
	 * If there are fewer regions than queues, all regions are shared
	 * amongst all queues.
	 */
	struct mutex sb_mutex ____cacheline_aligned;
	DECLARE_BITMAP(sb, KSB_COUNT);
	wait_queue_head_t sb_queue;
	unsigned int sb_avail;
	unsigned int sb_count;
	u32 sb_start;

	/* Bitmap of shared LSBs, if any */
	DECLARE_BITMAP(lsbmap, SLSB_MAP_SIZE);

	/* Suspend support */
	unsigned int suspending;
	wait_queue_head_t suspend_queue;

	/* DMA caching attribute support */
	unsigned int axcache;

	/* Device Statistics */
	unsigned long total_interrupts;

	/* DebugFS info */
	struct dentry *debugfs_instance;
};

enum ccp_memtype {
	CCP_MEMTYPE_SYSTEM = 0,
	CCP_MEMTYPE_SB,
	CCP_MEMTYPE_LOCAL,
	CCP_MEMTYPE__LAST,
};
#define	CCP_MEMTYPE_LSB	CCP_MEMTYPE_KSB


struct ccp_dma_info {
	dma_addr_t address;
	unsigned int offset;
	unsigned int length;
	enum dma_data_direction dir;
} __packed __aligned(4);

struct ccp_dm_workarea {
	struct device *dev;
	struct dma_pool *dma_pool;

	u8 *address;
	struct ccp_dma_info dma;
	unsigned int length;
};

struct ccp_sg_workarea {
	struct scatterlist *sg;
	int nents;
	unsigned int sg_used;

	struct scatterlist *dma_sg;
	struct scatterlist *dma_sg_head;
	struct device *dma_dev;
	unsigned int dma_count;
	enum dma_data_direction dma_dir;

	u64 bytes_left;
};

struct ccp_data {
	struct ccp_sg_workarea sg_wa;
	struct ccp_dm_workarea dm_wa;
};

struct ccp_mem {
	enum ccp_memtype type;
	union {
		struct ccp_dma_info dma;
		u32 sb;
	} u;
};

struct ccp_aes_op {
	enum ccp_aes_type type;
	enum ccp_aes_mode mode;
	enum ccp_aes_action action;
	unsigned int size;
};

struct ccp_xts_aes_op {
	enum ccp_aes_type type;
	enum ccp_aes_action action;
	enum ccp_xts_aes_unit_size unit_size;
};

struct ccp_des3_op {
	enum ccp_des3_type type;
	enum ccp_des3_mode mode;
	enum ccp_des3_action action;
};

struct ccp_sha_op {
	enum ccp_sha_type type;
	u64 msg_bits;
};

struct ccp_rsa_op {
	u32 mod_size;
	u32 input_len;
};

struct ccp_passthru_op {
	enum ccp_passthru_bitwise bit_mod;
	enum ccp_passthru_byteswap byte_swap;
};

struct ccp_ecc_op {
	enum ccp_ecc_function function;
};

struct ccp_op {
	struct ccp_cmd_queue *cmd_q;

	u32 jobid;
	u32 ioc;
	u32 soc;
	u32 sb_key;
	u32 sb_ctx;
	u32 init;
	u32 eom;

	struct ccp_mem src;
	struct ccp_mem dst;
	struct ccp_mem exp;

	union {
		struct ccp_aes_op aes;
		struct ccp_xts_aes_op xts;
		struct ccp_des3_op des3;
		struct ccp_sha_op sha;
		struct ccp_rsa_op rsa;
		struct ccp_passthru_op passthru;
		struct ccp_ecc_op ecc;
	} u;
};

static inline u32 ccp_addr_lo(struct ccp_dma_info *info)
{
	return lower_32_bits(info->address + info->offset);
}

static inline u32 ccp_addr_hi(struct ccp_dma_info *info)
{
	return upper_32_bits(info->address + info->offset) & 0x0000ffff;
}

/**
 * descriptor for version 5 CPP commands
 * 8 32-bit words:
 * word 0: function; engine; control bits
 * word 1: length of source data
 * word 2: low 32 bits of source pointer
 * word 3: upper 16 bits of source pointer; source memory type
 * word 4: low 32 bits of destination pointer
 * word 5: upper 16 bits of destination pointer; destination memory type
 * word 6: low 32 bits of key pointer
 * word 7: upper 16 bits of key pointer; key memory type
 */
struct dword0 {
	unsigned int soc:1;
	unsigned int ioc:1;
	unsigned int rsvd1:1;
	unsigned int init:1;
	unsigned int eom:1;		/* AES/SHA only */
	unsigned int function:15;
	unsigned int engine:4;
	unsigned int prot:1;
	unsigned int rsvd2:7;
};

struct dword3 {
	unsigned int  src_hi:16;
	unsigned int  src_mem:2;
	unsigned int  lsb_cxt_id:8;
	unsigned int  rsvd1:5;
	unsigned int  fixed:1;
};

union dword4 {
	u32 dst_lo;		/* NON-SHA	*/
	u32 sha_len_lo;		/* SHA		*/
};

union dword5 {
	struct {
		unsigned int  dst_hi:16;
		unsigned int  dst_mem:2;
		unsigned int  rsvd1:13;
		unsigned int  fixed:1;
	} fields;
	u32 sha_len_hi;
};

struct dword7 {
	unsigned int  key_hi:16;
	unsigned int  key_mem:2;
	unsigned int  rsvd1:14;
};

struct ccp5_desc {
	struct dword0 dw0;
	u32 length;
	u32 src_lo;
	struct dword3 dw3;
	union dword4 dw4;
	union dword5 dw5;
	u32 key_lo;
	struct dword7 dw7;
};

void ccp_add_device(struct ccp_device *ccp);
void ccp_del_device(struct ccp_device *ccp);

extern void ccp_log_error(struct ccp_device *, unsigned int);

struct ccp_device *ccp_alloc_struct(struct sp_device *sp);
bool ccp_queues_suspended(struct ccp_device *ccp);
int ccp_cmd_queue_thread(void *data);
int ccp_trng_read(struct hwrng *rng, void *data, size_t max, bool wait);

int ccp_run_cmd(struct ccp_cmd_queue *cmd_q, struct ccp_cmd *cmd);

int ccp_register_rng(struct ccp_device *ccp);
void ccp_unregister_rng(struct ccp_device *ccp);
int ccp_dmaengine_register(struct ccp_device *ccp);
void ccp_dmaengine_unregister(struct ccp_device *ccp);

void ccp5_debugfs_setup(struct ccp_device *ccp);
void ccp5_debugfs_destroy(void);

/* Structure for computation functions that are device-specific */
struct ccp_actions {
	int (*aes)(struct ccp_op *);
	int (*xts_aes)(struct ccp_op *);
	int (*des3)(struct ccp_op *);
	int (*sha)(struct ccp_op *);
	int (*rsa)(struct ccp_op *);
	int (*passthru)(struct ccp_op *);
	int (*ecc)(struct ccp_op *);
	u32 (*sballoc)(struct ccp_cmd_queue *, unsigned int);
	void (*sbfree)(struct ccp_cmd_queue *, unsigned int, unsigned int);
	unsigned int (*get_free_slots)(struct ccp_cmd_queue *);
	int (*init)(struct ccp_device *);
	void (*destroy)(struct ccp_device *);
	irqreturn_t (*irqhandler)(int, void *);
};

extern const struct ccp_vdata ccpv3_platform;
extern const struct ccp_vdata ccpv3;
extern const struct ccp_vdata ccpv5a;
extern const struct ccp_vdata ccpv5b;

#endif
