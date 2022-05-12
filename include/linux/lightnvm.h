/* SPDX-License-Identifier: GPL-2.0 */
#ifndef NVM_H
#define NVM_H

#include <linux/blkdev.h>
#include <linux/types.h>
#include <uapi/linux/lightnvm.h>

enum {
	NVM_IO_OK = 0,
	NVM_IO_REQUEUE = 1,
	NVM_IO_DONE = 2,
	NVM_IO_ERR = 3,

	NVM_IOTYPE_NONE = 0,
	NVM_IOTYPE_GC = 1,
};

/* common format */
#define NVM_GEN_CH_BITS  (8)
#define NVM_GEN_LUN_BITS (8)
#define NVM_GEN_BLK_BITS (16)
#define NVM_GEN_RESERVED (32)

/* 1.2 format */
#define NVM_12_PG_BITS  (16)
#define NVM_12_PL_BITS  (4)
#define NVM_12_SEC_BITS (4)
#define NVM_12_RESERVED (8)

/* 2.0 format */
#define NVM_20_SEC_BITS (24)
#define NVM_20_RESERVED (8)

enum {
	NVM_OCSSD_SPEC_12 = 12,
	NVM_OCSSD_SPEC_20 = 20,
};

struct ppa_addr {
	/* Generic structure for all addresses */
	union {
		/* generic device format */
		struct {
			u64 ch		: NVM_GEN_CH_BITS;
			u64 lun		: NVM_GEN_LUN_BITS;
			u64 blk		: NVM_GEN_BLK_BITS;
			u64 reserved	: NVM_GEN_RESERVED;
		} a;

		/* 1.2 device format */
		struct {
			u64 ch		: NVM_GEN_CH_BITS;
			u64 lun		: NVM_GEN_LUN_BITS;
			u64 blk		: NVM_GEN_BLK_BITS;
			u64 pg		: NVM_12_PG_BITS;
			u64 pl		: NVM_12_PL_BITS;
			u64 sec		: NVM_12_SEC_BITS;
			u64 reserved	: NVM_12_RESERVED;
		} g;

		/* 2.0 device format */
		struct {
			u64 grp		: NVM_GEN_CH_BITS;
			u64 pu		: NVM_GEN_LUN_BITS;
			u64 chk		: NVM_GEN_BLK_BITS;
			u64 sec		: NVM_20_SEC_BITS;
			u64 reserved	: NVM_20_RESERVED;
		} m;

		struct {
			u64 line	: 63;
			u64 is_cached	: 1;
		} c;

		u64 ppa;
	};
};

struct nvm_rq;
struct nvm_id;
struct nvm_dev;
struct nvm_tgt_dev;
struct nvm_chk_meta;

typedef int (nvm_id_fn)(struct nvm_dev *);
typedef int (nvm_op_bb_tbl_fn)(struct nvm_dev *, struct ppa_addr, u8 *);
typedef int (nvm_op_set_bb_fn)(struct nvm_dev *, struct ppa_addr *, int, int);
typedef int (nvm_get_chk_meta_fn)(struct nvm_dev *, sector_t, int,
							struct nvm_chk_meta *);
typedef int (nvm_submit_io_fn)(struct nvm_dev *, struct nvm_rq *, void *);
typedef void *(nvm_create_dma_pool_fn)(struct nvm_dev *, char *, int);
typedef void (nvm_destroy_dma_pool_fn)(void *);
typedef void *(nvm_dev_dma_alloc_fn)(struct nvm_dev *, void *, gfp_t,
								dma_addr_t *);
typedef void (nvm_dev_dma_free_fn)(void *, void*, dma_addr_t);

struct nvm_dev_ops {
	nvm_id_fn		*identity;
	nvm_op_bb_tbl_fn	*get_bb_tbl;
	nvm_op_set_bb_fn	*set_bb_tbl;

	nvm_get_chk_meta_fn	*get_chk_meta;

	nvm_submit_io_fn	*submit_io;

	nvm_create_dma_pool_fn	*create_dma_pool;
	nvm_destroy_dma_pool_fn	*destroy_dma_pool;
	nvm_dev_dma_alloc_fn	*dev_dma_alloc;
	nvm_dev_dma_free_fn	*dev_dma_free;
};

#ifdef CONFIG_NVM

#include <linux/file.h>
#include <linux/dmapool.h>

enum {
	/* HW Responsibilities */
	NVM_RSP_L2P	= 1 << 0,
	NVM_RSP_ECC	= 1 << 1,

	/* Physical Adressing Mode */
	NVM_ADDRMODE_LINEAR	= 0,
	NVM_ADDRMODE_CHANNEL	= 1,

	/* Plane programming mode for LUN */
	NVM_PLANE_SINGLE	= 1,
	NVM_PLANE_DOUBLE	= 2,
	NVM_PLANE_QUAD		= 4,

	/* Status codes */
	NVM_RSP_SUCCESS		= 0x0,
	NVM_RSP_NOT_CHANGEABLE	= 0x1,
	NVM_RSP_ERR_FAILWRITE	= 0x40ff,
	NVM_RSP_ERR_EMPTYPAGE	= 0x42ff,
	NVM_RSP_ERR_FAILECC	= 0x4281,
	NVM_RSP_ERR_FAILCRC	= 0x4004,
	NVM_RSP_WARN_HIGHECC	= 0x4700,

	/* Device opcodes */
	NVM_OP_PWRITE		= 0x91,
	NVM_OP_PREAD		= 0x92,
	NVM_OP_ERASE		= 0x90,

	/* PPA Command Flags */
	NVM_IO_SNGL_ACCESS	= 0x0,
	NVM_IO_DUAL_ACCESS	= 0x1,
	NVM_IO_QUAD_ACCESS	= 0x2,

	/* NAND Access Modes */
	NVM_IO_SUSPEND		= 0x80,
	NVM_IO_SLC_MODE		= 0x100,
	NVM_IO_SCRAMBLE_ENABLE	= 0x200,

	/* Block Types */
	NVM_BLK_T_FREE		= 0x0,
	NVM_BLK_T_BAD		= 0x1,
	NVM_BLK_T_GRWN_BAD	= 0x2,
	NVM_BLK_T_DEV		= 0x4,
	NVM_BLK_T_HOST		= 0x8,

	/* Memory capabilities */
	NVM_ID_CAP_SLC		= 0x1,
	NVM_ID_CAP_CMD_SUSPEND	= 0x2,
	NVM_ID_CAP_SCRAMBLE	= 0x4,
	NVM_ID_CAP_ENCRYPT	= 0x8,

	/* Memory types */
	NVM_ID_FMTYPE_SLC	= 0,
	NVM_ID_FMTYPE_MLC	= 1,

	/* Device capabilities */
	NVM_ID_DCAP_BBLKMGMT	= 0x1,
	NVM_UD_DCAP_ECC		= 0x2,
};

struct nvm_id_lp_mlc {
	u16	num_pairs;
	u8	pairs[886];
};

struct nvm_id_lp_tbl {
	__u8	id[8];
	struct nvm_id_lp_mlc mlc;
};

struct nvm_addrf_12 {
	u8	ch_len;
	u8	lun_len;
	u8	blk_len;
	u8	pg_len;
	u8	pln_len;
	u8	sec_len;

	u8	ch_offset;
	u8	lun_offset;
	u8	blk_offset;
	u8	pg_offset;
	u8	pln_offset;
	u8	sec_offset;

	u64	ch_mask;
	u64	lun_mask;
	u64	blk_mask;
	u64	pg_mask;
	u64	pln_mask;
	u64	sec_mask;
};

struct nvm_addrf {
	u8	ch_len;
	u8	lun_len;
	u8	chk_len;
	u8	sec_len;
	u8	rsv_len[2];

	u8	ch_offset;
	u8	lun_offset;
	u8	chk_offset;
	u8	sec_offset;
	u8	rsv_off[2];

	u64	ch_mask;
	u64	lun_mask;
	u64	chk_mask;
	u64	sec_mask;
	u64	rsv_mask[2];
};

enum {
	/* Chunk states */
	NVM_CHK_ST_FREE =	1 << 0,
	NVM_CHK_ST_CLOSED =	1 << 1,
	NVM_CHK_ST_OPEN =	1 << 2,
	NVM_CHK_ST_OFFLINE =	1 << 3,

	/* Chunk types */
	NVM_CHK_TP_W_SEQ =	1 << 0,
	NVM_CHK_TP_W_RAN =	1 << 1,
	NVM_CHK_TP_SZ_SPEC =	1 << 4,
};

/*
 * Note: The structure size is linked to nvme_nvm_chk_meta such that the same
 * buffer can be used when converting from little endian to cpu addressing.
 */
struct nvm_chk_meta {
	u8	state;
	u8	type;
	u8	wi;
	u8	rsvd[5];
	u64	slba;
	u64	cnlb;
	u64	wp;
};

struct nvm_target {
	struct list_head list;
	struct nvm_tgt_dev *dev;
	struct nvm_tgt_type *type;
	struct gendisk *disk;
};

#define ADDR_EMPTY (~0ULL)

#define NVM_TARGET_DEFAULT_OP (101)
#define NVM_TARGET_MIN_OP (3)
#define NVM_TARGET_MAX_OP (80)

#define NVM_VERSION_MAJOR 1
#define NVM_VERSION_MINOR 0
#define NVM_VERSION_PATCH 0

#define NVM_MAX_VLBA (64) /* max logical blocks in a vector command */

struct nvm_rq;
typedef void (nvm_end_io_fn)(struct nvm_rq *);

struct nvm_rq {
	struct nvm_tgt_dev *dev;

	struct bio *bio;

	union {
		struct ppa_addr ppa_addr;
		dma_addr_t dma_ppa_list;
	};

	struct ppa_addr *ppa_list;

	void *meta_list;
	dma_addr_t dma_meta_list;

	nvm_end_io_fn *end_io;

	uint8_t opcode;
	uint16_t nr_ppas;
	uint16_t flags;

	u64 ppa_status; /* ppa media status */
	int error;

	int is_seq; /* Sequential hint flag. 1.2 only */

	void *private;
};

static inline struct nvm_rq *nvm_rq_from_pdu(void *pdu)
{
	return pdu - sizeof(struct nvm_rq);
}

static inline void *nvm_rq_to_pdu(struct nvm_rq *rqdata)
{
	return rqdata + 1;
}

static inline struct ppa_addr *nvm_rq_to_ppa_list(struct nvm_rq *rqd)
{
	return (rqd->nr_ppas > 1) ? rqd->ppa_list : &rqd->ppa_addr;
}

enum {
	NVM_BLK_ST_FREE =	0x1,	/* Free block */
	NVM_BLK_ST_TGT =	0x2,	/* Block in use by target */
	NVM_BLK_ST_BAD =	0x8,	/* Bad block */
};

/* Instance geometry */
struct nvm_geo {
	/* device reported version */
	u8	major_ver_id;
	u8	minor_ver_id;

	/* kernel short version */
	u8	version;

	/* instance specific geometry */
	int num_ch;
	int num_lun;		/* per channel */

	/* calculated values */
	int all_luns;		/* across channels */
	int all_chunks;		/* across channels */

	int op;			/* over-provision in instance */

	sector_t total_secs;	/* across channels */

	/* chunk geometry */
	u32	num_chk;	/* chunks per lun */
	u32	clba;		/* sectors per chunk */
	u16	csecs;		/* sector size */
	u16	sos;		/* out-of-band area size */
	bool	ext;		/* metadata in extended data buffer */
	u32	mdts;		/* Max data transfer size*/

	/* device write constrains */
	u32	ws_min;		/* minimum write size */
	u32	ws_opt;		/* optimal write size */
	u32	mw_cunits;	/* distance required for successful read */
	u32	maxoc;		/* maximum open chunks */
	u32	maxocpu;	/* maximum open chunks per parallel unit */

	/* device capabilities */
	u32	mccap;

	/* device timings */
	u32	trdt;		/* Avg. Tread (ns) */
	u32	trdm;		/* Max Tread (ns) */
	u32	tprt;		/* Avg. Tprog (ns) */
	u32	tprm;		/* Max Tprog (ns) */
	u32	tbet;		/* Avg. Terase (ns) */
	u32	tbem;		/* Max Terase (ns) */

	/* generic address format */
	struct nvm_addrf addrf;

	/* 1.2 compatibility */
	u8	vmnt;
	u32	cap;
	u32	dom;

	u8	mtype;
	u8	fmtype;

	u16	cpar;
	u32	mpos;

	u8	num_pln;
	u8	pln_mode;
	u16	num_pg;
	u16	fpg_sz;
};

/* sub-device structure */
struct nvm_tgt_dev {
	/* Device information */
	struct nvm_geo geo;

	/* Base ppas for target LUNs */
	struct ppa_addr *luns;

	struct request_queue *q;

	struct nvm_dev *parent;
	void *map;
};

struct nvm_dev {
	struct nvm_dev_ops *ops;

	struct list_head devices;

	/* Device information */
	struct nvm_geo geo;

	unsigned long *lun_map;
	void *dma_pool;

	/* Backend device */
	struct request_queue *q;
	char name[DISK_NAME_LEN];
	void *private_data;

	struct kref ref;
	void *rmap;

	struct mutex mlock;
	spinlock_t lock;

	/* target management */
	struct list_head area_list;
	struct list_head targets;
};

static inline struct ppa_addr generic_to_dev_addr(struct nvm_dev *dev,
						  struct ppa_addr r)
{
	struct nvm_geo *geo = &dev->geo;
	struct ppa_addr l;

	if (geo->version == NVM_OCSSD_SPEC_12) {
		struct nvm_addrf_12 *ppaf = (struct nvm_addrf_12 *)&geo->addrf;

		l.ppa = ((u64)r.g.ch) << ppaf->ch_offset;
		l.ppa |= ((u64)r.g.lun) << ppaf->lun_offset;
		l.ppa |= ((u64)r.g.blk) << ppaf->blk_offset;
		l.ppa |= ((u64)r.g.pg) << ppaf->pg_offset;
		l.ppa |= ((u64)r.g.pl) << ppaf->pln_offset;
		l.ppa |= ((u64)r.g.sec) << ppaf->sec_offset;
	} else {
		struct nvm_addrf *lbaf = &geo->addrf;

		l.ppa = ((u64)r.m.grp) << lbaf->ch_offset;
		l.ppa |= ((u64)r.m.pu) << lbaf->lun_offset;
		l.ppa |= ((u64)r.m.chk) << lbaf->chk_offset;
		l.ppa |= ((u64)r.m.sec) << lbaf->sec_offset;
	}

	return l;
}

static inline struct ppa_addr dev_to_generic_addr(struct nvm_dev *dev,
						  struct ppa_addr r)
{
	struct nvm_geo *geo = &dev->geo;
	struct ppa_addr l;

	l.ppa = 0;

	if (geo->version == NVM_OCSSD_SPEC_12) {
		struct nvm_addrf_12 *ppaf = (struct nvm_addrf_12 *)&geo->addrf;

		l.g.ch = (r.ppa & ppaf->ch_mask) >> ppaf->ch_offset;
		l.g.lun = (r.ppa & ppaf->lun_mask) >> ppaf->lun_offset;
		l.g.blk = (r.ppa & ppaf->blk_mask) >> ppaf->blk_offset;
		l.g.pg = (r.ppa & ppaf->pg_mask) >> ppaf->pg_offset;
		l.g.pl = (r.ppa & ppaf->pln_mask) >> ppaf->pln_offset;
		l.g.sec = (r.ppa & ppaf->sec_mask) >> ppaf->sec_offset;
	} else {
		struct nvm_addrf *lbaf = &geo->addrf;

		l.m.grp = (r.ppa & lbaf->ch_mask) >> lbaf->ch_offset;
		l.m.pu = (r.ppa & lbaf->lun_mask) >> lbaf->lun_offset;
		l.m.chk = (r.ppa & lbaf->chk_mask) >> lbaf->chk_offset;
		l.m.sec = (r.ppa & lbaf->sec_mask) >> lbaf->sec_offset;
	}

	return l;
}

static inline u64 dev_to_chunk_addr(struct nvm_dev *dev, void *addrf,
				    struct ppa_addr p)
{
	struct nvm_geo *geo = &dev->geo;
	u64 caddr;

	if (geo->version == NVM_OCSSD_SPEC_12) {
		struct nvm_addrf_12 *ppaf = (struct nvm_addrf_12 *)addrf;

		caddr = (u64)p.g.pg << ppaf->pg_offset;
		caddr |= (u64)p.g.pl << ppaf->pln_offset;
		caddr |= (u64)p.g.sec << ppaf->sec_offset;
	} else {
		caddr = p.m.sec;
	}

	return caddr;
}

static inline struct ppa_addr nvm_ppa32_to_ppa64(struct nvm_dev *dev,
						 void *addrf, u32 ppa32)
{
	struct ppa_addr ppa64;

	ppa64.ppa = 0;

	if (ppa32 == -1) {
		ppa64.ppa = ADDR_EMPTY;
	} else if (ppa32 & (1U << 31)) {
		ppa64.c.line = ppa32 & ((~0U) >> 1);
		ppa64.c.is_cached = 1;
	} else {
		struct nvm_geo *geo = &dev->geo;

		if (geo->version == NVM_OCSSD_SPEC_12) {
			struct nvm_addrf_12 *ppaf = addrf;

			ppa64.g.ch = (ppa32 & ppaf->ch_mask) >>
							ppaf->ch_offset;
			ppa64.g.lun = (ppa32 & ppaf->lun_mask) >>
							ppaf->lun_offset;
			ppa64.g.blk = (ppa32 & ppaf->blk_mask) >>
							ppaf->blk_offset;
			ppa64.g.pg = (ppa32 & ppaf->pg_mask) >>
							ppaf->pg_offset;
			ppa64.g.pl = (ppa32 & ppaf->pln_mask) >>
							ppaf->pln_offset;
			ppa64.g.sec = (ppa32 & ppaf->sec_mask) >>
							ppaf->sec_offset;
		} else {
			struct nvm_addrf *lbaf = addrf;

			ppa64.m.grp = (ppa32 & lbaf->ch_mask) >>
							lbaf->ch_offset;
			ppa64.m.pu = (ppa32 & lbaf->lun_mask) >>
							lbaf->lun_offset;
			ppa64.m.chk = (ppa32 & lbaf->chk_mask) >>
							lbaf->chk_offset;
			ppa64.m.sec = (ppa32 & lbaf->sec_mask) >>
							lbaf->sec_offset;
		}
	}

	return ppa64;
}

static inline u32 nvm_ppa64_to_ppa32(struct nvm_dev *dev,
				     void *addrf, struct ppa_addr ppa64)
{
	u32 ppa32 = 0;

	if (ppa64.ppa == ADDR_EMPTY) {
		ppa32 = ~0U;
	} else if (ppa64.c.is_cached) {
		ppa32 |= ppa64.c.line;
		ppa32 |= 1U << 31;
	} else {
		struct nvm_geo *geo = &dev->geo;

		if (geo->version == NVM_OCSSD_SPEC_12) {
			struct nvm_addrf_12 *ppaf = addrf;

			ppa32 |= ppa64.g.ch << ppaf->ch_offset;
			ppa32 |= ppa64.g.lun << ppaf->lun_offset;
			ppa32 |= ppa64.g.blk << ppaf->blk_offset;
			ppa32 |= ppa64.g.pg << ppaf->pg_offset;
			ppa32 |= ppa64.g.pl << ppaf->pln_offset;
			ppa32 |= ppa64.g.sec << ppaf->sec_offset;
		} else {
			struct nvm_addrf *lbaf = addrf;

			ppa32 |= ppa64.m.grp << lbaf->ch_offset;
			ppa32 |= ppa64.m.pu << lbaf->lun_offset;
			ppa32 |= ppa64.m.chk << lbaf->chk_offset;
			ppa32 |= ppa64.m.sec << lbaf->sec_offset;
		}
	}

	return ppa32;
}

static inline int nvm_next_ppa_in_chk(struct nvm_tgt_dev *dev,
				      struct ppa_addr *ppa)
{
	struct nvm_geo *geo = &dev->geo;
	int last = 0;

	if (geo->version == NVM_OCSSD_SPEC_12) {
		int sec = ppa->g.sec;

		sec++;
		if (sec == geo->ws_min) {
			int pg = ppa->g.pg;

			sec = 0;
			pg++;
			if (pg == geo->num_pg) {
				int pl = ppa->g.pl;

				pg = 0;
				pl++;
				if (pl == geo->num_pln)
					last = 1;

				ppa->g.pl = pl;
			}
			ppa->g.pg = pg;
		}
		ppa->g.sec = sec;
	} else {
		ppa->m.sec++;
		if (ppa->m.sec == geo->clba)
			last = 1;
	}

	return last;
}

typedef sector_t (nvm_tgt_capacity_fn)(void *);
typedef void *(nvm_tgt_init_fn)(struct nvm_tgt_dev *, struct gendisk *,
				int flags);
typedef void (nvm_tgt_exit_fn)(void *, bool);
typedef int (nvm_tgt_sysfs_init_fn)(struct gendisk *);
typedef void (nvm_tgt_sysfs_exit_fn)(struct gendisk *);

enum {
	NVM_TGT_F_DEV_L2P = 0,
	NVM_TGT_F_HOST_L2P = 1 << 0,
};

struct nvm_tgt_type {
	const char *name;
	unsigned int version[3];
	int flags;

	/* target entry points */
	const struct block_device_operations *bops;
	nvm_tgt_capacity_fn *capacity;

	/* module-specific init/teardown */
	nvm_tgt_init_fn *init;
	nvm_tgt_exit_fn *exit;

	/* sysfs */
	nvm_tgt_sysfs_init_fn *sysfs_init;
	nvm_tgt_sysfs_exit_fn *sysfs_exit;

	/* For internal use */
	struct list_head list;
	struct module *owner;
};

extern int nvm_register_tgt_type(struct nvm_tgt_type *);
extern void nvm_unregister_tgt_type(struct nvm_tgt_type *);

extern void *nvm_dev_dma_alloc(struct nvm_dev *, gfp_t, dma_addr_t *);
extern void nvm_dev_dma_free(struct nvm_dev *, void *, dma_addr_t);

extern struct nvm_dev *nvm_alloc_dev(int);
extern int nvm_register(struct nvm_dev *);
extern void nvm_unregister(struct nvm_dev *);

extern int nvm_get_chunk_meta(struct nvm_tgt_dev *, struct ppa_addr,
			      int, struct nvm_chk_meta *);
extern int nvm_set_chunk_meta(struct nvm_tgt_dev *, struct ppa_addr *,
			      int, int);
extern int nvm_submit_io(struct nvm_tgt_dev *, struct nvm_rq *, void *);
extern int nvm_submit_io_sync(struct nvm_tgt_dev *, struct nvm_rq *, void *);
extern void nvm_end_io(struct nvm_rq *);

#else /* CONFIG_NVM */
struct nvm_dev_ops;

static inline struct nvm_dev *nvm_alloc_dev(int node)
{
	return ERR_PTR(-EINVAL);
}
static inline int nvm_register(struct nvm_dev *dev)
{
	return -EINVAL;
}
static inline void nvm_unregister(struct nvm_dev *dev) {}
#endif /* CONFIG_NVM */
#endif /* LIGHTNVM.H */
