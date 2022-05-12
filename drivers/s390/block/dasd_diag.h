/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Author(s)......: Holger Smolinski <Holger.Smolinski@de.ibm.com>
 * Based on.......: linux/drivers/s390/block/mdisk.h
 * ...............: by Hartmunt Penner <hpenner@de.ibm.com>
 * Bugreports.to..: <Linux390@de.ibm.com>
 * Copyright IBM Corp. 1999, 2000
 *
 */

#define MDSK_WRITE_REQ 0x01
#define MDSK_READ_REQ  0x02

#define INIT_BIO	0x00
#define RW_BIO		0x01
#define TERM_BIO	0x02

#define DEV_CLASS_FBA	0x01
#define DEV_CLASS_ECKD	0x04

#define DASD_DIAG_CODE_31BIT		0x03
#define DASD_DIAG_CODE_64BIT		0x07

#define DASD_DIAG_RWFLAG_ASYNC		0x02
#define DASD_DIAG_RWFLAG_NOCACHE	0x01

#define DASD_DIAG_FLAGA_FORMAT_64BIT	0x80

struct dasd_diag_characteristics {
	u16 dev_nr;
	u16 rdc_len;
	u8 vdev_class;
	u8 vdev_type;
	u8 vdev_status;
	u8 vdev_flags;
	u8 rdev_class;
	u8 rdev_type;
	u8 rdev_model;
	u8 rdev_features;
} __attribute__ ((packed, aligned(4)));

#define DASD_DIAG_FLAGA_DEFAULT		DASD_DIAG_FLAGA_FORMAT_64BIT

typedef u64 blocknum_t;
typedef s64 sblocknum_t;

struct dasd_diag_bio {
	u8 type;
	u8 status;
	u8 spare1[2];
	u32 alet;
	blocknum_t block_number;
	void *buffer;
} __attribute__ ((packed, aligned(8)));

struct dasd_diag_init_io {
	u16 dev_nr;
	u8 flaga;
	u8 spare1[21];
	u32 block_size;
	u8 spare2[4];
	blocknum_t offset;
	sblocknum_t start_block;
	blocknum_t end_block;
	u8  spare3[8];
} __attribute__ ((packed, aligned(8)));

struct dasd_diag_rw_io {
	u16 dev_nr;
	u8  flaga;
	u8  spare1[21];
	u8  key;
	u8  flags;
	u8  spare2[2];
	u32 block_count;
	u32 alet;
	u8  spare3[4];
	u64 interrupt_params;
	struct dasd_diag_bio *bio_list;
	u8  spare4[8];
} __attribute__ ((packed, aligned(8)));
