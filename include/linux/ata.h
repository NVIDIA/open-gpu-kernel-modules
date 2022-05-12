/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 *  Copyright 2003-2004 Red Hat, Inc.  All rights reserved.
 *  Copyright 2003-2004 Jeff Garzik
 *
 *  libata documentation is available via 'make {ps|pdf}docs',
 *  as Documentation/driver-api/libata.rst
 *
 *  Hardware documentation available from http://www.t13.org/
 */

#ifndef __LINUX_ATA_H__
#define __LINUX_ATA_H__

#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/types.h>
#include <asm/byteorder.h>

/* defines only for the constants which don't work well as enums */
#define ATA_DMA_BOUNDARY	0xffffUL
#define ATA_DMA_MASK		0xffffffffULL

enum {
	/* various global constants */
	ATA_MAX_DEVICES		= 2,	/* per bus/port */
	ATA_MAX_PRD		= 256,	/* we could make these 256/256 */
	ATA_SECT_SIZE		= 512,
	ATA_MAX_SECTORS_128	= 128,
	ATA_MAX_SECTORS		= 256,
	ATA_MAX_SECTORS_1024    = 1024,
	ATA_MAX_SECTORS_LBA48	= 65535,/* avoid count to be 0000h */
	ATA_MAX_SECTORS_TAPE	= 65535,
	ATA_MAX_TRIM_RNUM	= 64,	/* 512-byte payload / (6-byte LBA + 2-byte range per entry) */

	ATA_ID_WORDS		= 256,
	ATA_ID_CONFIG		= 0,
	ATA_ID_CYLS		= 1,
	ATA_ID_HEADS		= 3,
	ATA_ID_SECTORS		= 6,
	ATA_ID_SERNO		= 10,
	ATA_ID_BUF_SIZE		= 21,
	ATA_ID_FW_REV		= 23,
	ATA_ID_PROD		= 27,
	ATA_ID_MAX_MULTSECT	= 47,
	ATA_ID_DWORD_IO		= 48,	/* before ATA-8 */
	ATA_ID_TRUSTED		= 48,	/* ATA-8 and later */
	ATA_ID_CAPABILITY	= 49,
	ATA_ID_OLD_PIO_MODES	= 51,
	ATA_ID_OLD_DMA_MODES	= 52,
	ATA_ID_FIELD_VALID	= 53,
	ATA_ID_CUR_CYLS		= 54,
	ATA_ID_CUR_HEADS	= 55,
	ATA_ID_CUR_SECTORS	= 56,
	ATA_ID_MULTSECT		= 59,
	ATA_ID_LBA_CAPACITY	= 60,
	ATA_ID_SWDMA_MODES	= 62,
	ATA_ID_MWDMA_MODES	= 63,
	ATA_ID_PIO_MODES	= 64,
	ATA_ID_EIDE_DMA_MIN	= 65,
	ATA_ID_EIDE_DMA_TIME	= 66,
	ATA_ID_EIDE_PIO		= 67,
	ATA_ID_EIDE_PIO_IORDY	= 68,
	ATA_ID_ADDITIONAL_SUPP	= 69,
	ATA_ID_QUEUE_DEPTH	= 75,
	ATA_ID_SATA_CAPABILITY	= 76,
	ATA_ID_SATA_CAPABILITY_2	= 77,
	ATA_ID_FEATURE_SUPP	= 78,
	ATA_ID_MAJOR_VER	= 80,
	ATA_ID_COMMAND_SET_1	= 82,
	ATA_ID_COMMAND_SET_2	= 83,
	ATA_ID_CFSSE		= 84,
	ATA_ID_CFS_ENABLE_1	= 85,
	ATA_ID_CFS_ENABLE_2	= 86,
	ATA_ID_CSF_DEFAULT	= 87,
	ATA_ID_UDMA_MODES	= 88,
	ATA_ID_HW_CONFIG	= 93,
	ATA_ID_SPG		= 98,
	ATA_ID_LBA_CAPACITY_2	= 100,
	ATA_ID_SECTOR_SIZE	= 106,
	ATA_ID_WWN		= 108,
	ATA_ID_LOGICAL_SECTOR_SIZE	= 117,	/* and 118 */
	ATA_ID_COMMAND_SET_3	= 119,
	ATA_ID_COMMAND_SET_4	= 120,
	ATA_ID_LAST_LUN		= 126,
	ATA_ID_DLF		= 128,
	ATA_ID_CSFO		= 129,
	ATA_ID_CFA_POWER	= 160,
	ATA_ID_CFA_KEY_MGMT	= 162,
	ATA_ID_CFA_MODES	= 163,
	ATA_ID_DATA_SET_MGMT	= 169,
	ATA_ID_SCT_CMD_XPORT	= 206,
	ATA_ID_ROT_SPEED	= 217,
	ATA_ID_PIO4		= (1 << 1),

	ATA_ID_SERNO_LEN	= 20,
	ATA_ID_FW_REV_LEN	= 8,
	ATA_ID_PROD_LEN		= 40,
	ATA_ID_WWN_LEN		= 8,

	ATA_PCI_CTL_OFS		= 2,

	ATA_PIO0		= (1 << 0),
	ATA_PIO1		= ATA_PIO0 | (1 << 1),
	ATA_PIO2		= ATA_PIO1 | (1 << 2),
	ATA_PIO3		= ATA_PIO2 | (1 << 3),
	ATA_PIO4		= ATA_PIO3 | (1 << 4),
	ATA_PIO5		= ATA_PIO4 | (1 << 5),
	ATA_PIO6		= ATA_PIO5 | (1 << 6),

	ATA_PIO4_ONLY		= (1 << 4),

	ATA_SWDMA0		= (1 << 0),
	ATA_SWDMA1		= ATA_SWDMA0 | (1 << 1),
	ATA_SWDMA2		= ATA_SWDMA1 | (1 << 2),

	ATA_SWDMA2_ONLY		= (1 << 2),

	ATA_MWDMA0		= (1 << 0),
	ATA_MWDMA1		= ATA_MWDMA0 | (1 << 1),
	ATA_MWDMA2		= ATA_MWDMA1 | (1 << 2),
	ATA_MWDMA3		= ATA_MWDMA2 | (1 << 3),
	ATA_MWDMA4		= ATA_MWDMA3 | (1 << 4),

	ATA_MWDMA12_ONLY	= (1 << 1) | (1 << 2),
	ATA_MWDMA2_ONLY		= (1 << 2),

	ATA_UDMA0		= (1 << 0),
	ATA_UDMA1		= ATA_UDMA0 | (1 << 1),
	ATA_UDMA2		= ATA_UDMA1 | (1 << 2),
	ATA_UDMA3		= ATA_UDMA2 | (1 << 3),
	ATA_UDMA4		= ATA_UDMA3 | (1 << 4),
	ATA_UDMA5		= ATA_UDMA4 | (1 << 5),
	ATA_UDMA6		= ATA_UDMA5 | (1 << 6),
	ATA_UDMA7		= ATA_UDMA6 | (1 << 7),
	/* ATA_UDMA7 is just for completeness... doesn't exist (yet?).  */

	ATA_UDMA24_ONLY		= (1 << 2) | (1 << 4),

	ATA_UDMA_MASK_40C	= ATA_UDMA2,	/* udma0-2 */

	/* DMA-related */
	ATA_PRD_SZ		= 8,
	ATA_PRD_TBL_SZ		= (ATA_MAX_PRD * ATA_PRD_SZ),
	ATA_PRD_EOT		= (1 << 31),	/* end-of-table flag */

	ATA_DMA_TABLE_OFS	= 4,
	ATA_DMA_STATUS		= 2,
	ATA_DMA_CMD		= 0,
	ATA_DMA_WR		= (1 << 3),
	ATA_DMA_START		= (1 << 0),
	ATA_DMA_INTR		= (1 << 2),
	ATA_DMA_ERR		= (1 << 1),
	ATA_DMA_ACTIVE		= (1 << 0),

	/* bits in ATA command block registers */
	ATA_HOB			= (1 << 7),	/* LBA48 selector */
	ATA_NIEN		= (1 << 1),	/* disable-irq flag */
	ATA_LBA			= (1 << 6),	/* LBA28 selector */
	ATA_DEV1		= (1 << 4),	/* Select Device 1 (slave) */
	ATA_DEVICE_OBS		= (1 << 7) | (1 << 5), /* obs bits in dev reg */
	ATA_DEVCTL_OBS		= (1 << 3),	/* obsolete bit in devctl reg */
	ATA_BUSY		= (1 << 7),	/* BSY status bit */
	ATA_DRDY		= (1 << 6),	/* device ready */
	ATA_DF			= (1 << 5),	/* device fault */
	ATA_DSC			= (1 << 4),	/* drive seek complete */
	ATA_DRQ			= (1 << 3),	/* data request i/o */
	ATA_CORR		= (1 << 2),	/* corrected data error */
	ATA_SENSE		= (1 << 1),	/* sense code available */
	ATA_ERR			= (1 << 0),	/* have an error */
	ATA_SRST		= (1 << 2),	/* software reset */
	ATA_ICRC		= (1 << 7),	/* interface CRC error */
	ATA_BBK			= ATA_ICRC,	/* pre-EIDE: block marked bad */
	ATA_UNC			= (1 << 6),	/* uncorrectable media error */
	ATA_MC			= (1 << 5),	/* media changed */
	ATA_IDNF		= (1 << 4),	/* ID not found */
	ATA_MCR			= (1 << 3),	/* media change requested */
	ATA_ABORTED		= (1 << 2),	/* command aborted */
	ATA_TRK0NF		= (1 << 1),	/* track 0 not found */
	ATA_AMNF		= (1 << 0),	/* address mark not found */
	ATAPI_LFS		= 0xF0,		/* last failed sense */
	ATAPI_EOM		= ATA_TRK0NF,	/* end of media */
	ATAPI_ILI		= ATA_AMNF,	/* illegal length indication */
	ATAPI_IO		= (1 << 1),
	ATAPI_COD		= (1 << 0),

	/* ATA command block registers */
	ATA_REG_DATA		= 0x00,
	ATA_REG_ERR		= 0x01,
	ATA_REG_NSECT		= 0x02,
	ATA_REG_LBAL		= 0x03,
	ATA_REG_LBAM		= 0x04,
	ATA_REG_LBAH		= 0x05,
	ATA_REG_DEVICE		= 0x06,
	ATA_REG_STATUS		= 0x07,

	ATA_REG_FEATURE		= ATA_REG_ERR, /* and their aliases */
	ATA_REG_CMD		= ATA_REG_STATUS,
	ATA_REG_BYTEL		= ATA_REG_LBAM,
	ATA_REG_BYTEH		= ATA_REG_LBAH,
	ATA_REG_DEVSEL		= ATA_REG_DEVICE,
	ATA_REG_IRQ		= ATA_REG_NSECT,

	/* ATA device commands */
	ATA_CMD_DEV_RESET	= 0x08, /* ATAPI device reset */
	ATA_CMD_CHK_POWER	= 0xE5, /* check power mode */
	ATA_CMD_STANDBY		= 0xE2, /* place in standby power mode */
	ATA_CMD_IDLE		= 0xE3, /* place in idle power mode */
	ATA_CMD_EDD		= 0x90,	/* execute device diagnostic */
	ATA_CMD_DOWNLOAD_MICRO  = 0x92,
	ATA_CMD_DOWNLOAD_MICRO_DMA = 0x93,
	ATA_CMD_NOP		= 0x00,
	ATA_CMD_FLUSH		= 0xE7,
	ATA_CMD_FLUSH_EXT	= 0xEA,
	ATA_CMD_ID_ATA		= 0xEC,
	ATA_CMD_ID_ATAPI	= 0xA1,
	ATA_CMD_SERVICE		= 0xA2,
	ATA_CMD_READ		= 0xC8,
	ATA_CMD_READ_EXT	= 0x25,
	ATA_CMD_READ_QUEUED	= 0x26,
	ATA_CMD_READ_STREAM_EXT	= 0x2B,
	ATA_CMD_READ_STREAM_DMA_EXT = 0x2A,
	ATA_CMD_WRITE		= 0xCA,
	ATA_CMD_WRITE_EXT	= 0x35,
	ATA_CMD_WRITE_QUEUED	= 0x36,
	ATA_CMD_WRITE_STREAM_EXT = 0x3B,
	ATA_CMD_WRITE_STREAM_DMA_EXT = 0x3A,
	ATA_CMD_WRITE_FUA_EXT	= 0x3D,
	ATA_CMD_WRITE_QUEUED_FUA_EXT = 0x3E,
	ATA_CMD_FPDMA_READ	= 0x60,
	ATA_CMD_FPDMA_WRITE	= 0x61,
	ATA_CMD_NCQ_NON_DATA	= 0x63,
	ATA_CMD_FPDMA_SEND	= 0x64,
	ATA_CMD_FPDMA_RECV	= 0x65,
	ATA_CMD_PIO_READ	= 0x20,
	ATA_CMD_PIO_READ_EXT	= 0x24,
	ATA_CMD_PIO_WRITE	= 0x30,
	ATA_CMD_PIO_WRITE_EXT	= 0x34,
	ATA_CMD_READ_MULTI	= 0xC4,
	ATA_CMD_READ_MULTI_EXT	= 0x29,
	ATA_CMD_WRITE_MULTI	= 0xC5,
	ATA_CMD_WRITE_MULTI_EXT	= 0x39,
	ATA_CMD_WRITE_MULTI_FUA_EXT = 0xCE,
	ATA_CMD_SET_FEATURES	= 0xEF,
	ATA_CMD_SET_MULTI	= 0xC6,
	ATA_CMD_PACKET		= 0xA0,
	ATA_CMD_VERIFY		= 0x40,
	ATA_CMD_VERIFY_EXT	= 0x42,
	ATA_CMD_WRITE_UNCORR_EXT = 0x45,
	ATA_CMD_STANDBYNOW1	= 0xE0,
	ATA_CMD_IDLEIMMEDIATE	= 0xE1,
	ATA_CMD_SLEEP		= 0xE6,
	ATA_CMD_INIT_DEV_PARAMS	= 0x91,
	ATA_CMD_READ_NATIVE_MAX	= 0xF8,
	ATA_CMD_READ_NATIVE_MAX_EXT = 0x27,
	ATA_CMD_SET_MAX		= 0xF9,
	ATA_CMD_SET_MAX_EXT	= 0x37,
	ATA_CMD_READ_LOG_EXT	= 0x2F,
	ATA_CMD_WRITE_LOG_EXT	= 0x3F,
	ATA_CMD_READ_LOG_DMA_EXT = 0x47,
	ATA_CMD_WRITE_LOG_DMA_EXT = 0x57,
	ATA_CMD_TRUSTED_NONDATA	= 0x5B,
	ATA_CMD_TRUSTED_RCV	= 0x5C,
	ATA_CMD_TRUSTED_RCV_DMA = 0x5D,
	ATA_CMD_TRUSTED_SND	= 0x5E,
	ATA_CMD_TRUSTED_SND_DMA = 0x5F,
	ATA_CMD_PMP_READ	= 0xE4,
	ATA_CMD_PMP_READ_DMA	= 0xE9,
	ATA_CMD_PMP_WRITE	= 0xE8,
	ATA_CMD_PMP_WRITE_DMA	= 0xEB,
	ATA_CMD_CONF_OVERLAY	= 0xB1,
	ATA_CMD_SEC_SET_PASS	= 0xF1,
	ATA_CMD_SEC_UNLOCK	= 0xF2,
	ATA_CMD_SEC_ERASE_PREP	= 0xF3,
	ATA_CMD_SEC_ERASE_UNIT	= 0xF4,
	ATA_CMD_SEC_FREEZE_LOCK	= 0xF5,
	ATA_CMD_SEC_DISABLE_PASS = 0xF6,
	ATA_CMD_CONFIG_STREAM	= 0x51,
	ATA_CMD_SMART		= 0xB0,
	ATA_CMD_MEDIA_LOCK	= 0xDE,
	ATA_CMD_MEDIA_UNLOCK	= 0xDF,
	ATA_CMD_DSM		= 0x06,
	ATA_CMD_CHK_MED_CRD_TYP = 0xD1,
	ATA_CMD_CFA_REQ_EXT_ERR = 0x03,
	ATA_CMD_CFA_WRITE_NE	= 0x38,
	ATA_CMD_CFA_TRANS_SECT	= 0x87,
	ATA_CMD_CFA_ERASE	= 0xC0,
	ATA_CMD_CFA_WRITE_MULT_NE = 0xCD,
	ATA_CMD_REQ_SENSE_DATA  = 0x0B,
	ATA_CMD_SANITIZE_DEVICE = 0xB4,
	ATA_CMD_ZAC_MGMT_IN	= 0x4A,
	ATA_CMD_ZAC_MGMT_OUT	= 0x9F,

	/* marked obsolete in the ATA/ATAPI-7 spec */
	ATA_CMD_RESTORE		= 0x10,

	/* Subcmds for ATA_CMD_FPDMA_RECV */
	ATA_SUBCMD_FPDMA_RECV_RD_LOG_DMA_EXT = 0x01,
	ATA_SUBCMD_FPDMA_RECV_ZAC_MGMT_IN    = 0x02,

	/* Subcmds for ATA_CMD_FPDMA_SEND */
	ATA_SUBCMD_FPDMA_SEND_DSM            = 0x00,
	ATA_SUBCMD_FPDMA_SEND_WR_LOG_DMA_EXT = 0x02,

	/* Subcmds for ATA_CMD_NCQ_NON_DATA */
	ATA_SUBCMD_NCQ_NON_DATA_ABORT_QUEUE  = 0x00,
	ATA_SUBCMD_NCQ_NON_DATA_SET_FEATURES = 0x05,
	ATA_SUBCMD_NCQ_NON_DATA_ZERO_EXT     = 0x06,
	ATA_SUBCMD_NCQ_NON_DATA_ZAC_MGMT_OUT = 0x07,

	/* Subcmds for ATA_CMD_ZAC_MGMT_IN */
	ATA_SUBCMD_ZAC_MGMT_IN_REPORT_ZONES = 0x00,

	/* Subcmds for ATA_CMD_ZAC_MGMT_OUT */
	ATA_SUBCMD_ZAC_MGMT_OUT_CLOSE_ZONE = 0x01,
	ATA_SUBCMD_ZAC_MGMT_OUT_FINISH_ZONE = 0x02,
	ATA_SUBCMD_ZAC_MGMT_OUT_OPEN_ZONE = 0x03,
	ATA_SUBCMD_ZAC_MGMT_OUT_RESET_WRITE_POINTER = 0x04,

	/* READ_LOG_EXT pages */
	ATA_LOG_DIRECTORY	= 0x0,
	ATA_LOG_SATA_NCQ	= 0x10,
	ATA_LOG_NCQ_NON_DATA	= 0x12,
	ATA_LOG_NCQ_SEND_RECV	= 0x13,
	ATA_LOG_IDENTIFY_DEVICE	= 0x30,

	/* Identify device log pages: */
	ATA_LOG_SECURITY	  = 0x06,
	ATA_LOG_SATA_SETTINGS	  = 0x08,
	ATA_LOG_ZONED_INFORMATION = 0x09,

	/* Identify device SATA settings log:*/
	ATA_LOG_DEVSLP_OFFSET	  = 0x30,
	ATA_LOG_DEVSLP_SIZE	  = 0x08,
	ATA_LOG_DEVSLP_MDAT	  = 0x00,
	ATA_LOG_DEVSLP_MDAT_MASK  = 0x1F,
	ATA_LOG_DEVSLP_DETO	  = 0x01,
	ATA_LOG_DEVSLP_VALID	  = 0x07,
	ATA_LOG_DEVSLP_VALID_MASK = 0x80,
	ATA_LOG_NCQ_PRIO_OFFSET   = 0x09,

	/* NCQ send and receive log */
	ATA_LOG_NCQ_SEND_RECV_SUBCMDS_OFFSET	= 0x00,
	ATA_LOG_NCQ_SEND_RECV_SUBCMDS_DSM	= (1 << 0),
	ATA_LOG_NCQ_SEND_RECV_DSM_OFFSET	= 0x04,
	ATA_LOG_NCQ_SEND_RECV_DSM_TRIM		= (1 << 0),
	ATA_LOG_NCQ_SEND_RECV_RD_LOG_OFFSET	= 0x08,
	ATA_LOG_NCQ_SEND_RECV_RD_LOG_SUPPORTED  = (1 << 0),
	ATA_LOG_NCQ_SEND_RECV_WR_LOG_OFFSET	= 0x0C,
	ATA_LOG_NCQ_SEND_RECV_WR_LOG_SUPPORTED  = (1 << 0),
	ATA_LOG_NCQ_SEND_RECV_ZAC_MGMT_OFFSET	= 0x10,
	ATA_LOG_NCQ_SEND_RECV_ZAC_MGMT_OUT_SUPPORTED = (1 << 0),
	ATA_LOG_NCQ_SEND_RECV_ZAC_MGMT_IN_SUPPORTED = (1 << 1),
	ATA_LOG_NCQ_SEND_RECV_SIZE		= 0x14,

	/* NCQ Non-Data log */
	ATA_LOG_NCQ_NON_DATA_SUBCMDS_OFFSET	= 0x00,
	ATA_LOG_NCQ_NON_DATA_ABORT_OFFSET	= 0x00,
	ATA_LOG_NCQ_NON_DATA_ABORT_NCQ		= (1 << 0),
	ATA_LOG_NCQ_NON_DATA_ABORT_ALL		= (1 << 1),
	ATA_LOG_NCQ_NON_DATA_ABORT_STREAMING	= (1 << 2),
	ATA_LOG_NCQ_NON_DATA_ABORT_NON_STREAMING = (1 << 3),
	ATA_LOG_NCQ_NON_DATA_ABORT_SELECTED	= (1 << 4),
	ATA_LOG_NCQ_NON_DATA_ZAC_MGMT_OFFSET	= 0x1C,
	ATA_LOG_NCQ_NON_DATA_ZAC_MGMT_OUT	= (1 << 0),
	ATA_LOG_NCQ_NON_DATA_SIZE		= 0x40,

	/* READ/WRITE LONG (obsolete) */
	ATA_CMD_READ_LONG	= 0x22,
	ATA_CMD_READ_LONG_ONCE	= 0x23,
	ATA_CMD_WRITE_LONG	= 0x32,
	ATA_CMD_WRITE_LONG_ONCE	= 0x33,

	/* SETFEATURES stuff */
	SETFEATURES_XFER	= 0x03,
	XFER_UDMA_7		= 0x47,
	XFER_UDMA_6		= 0x46,
	XFER_UDMA_5		= 0x45,
	XFER_UDMA_4		= 0x44,
	XFER_UDMA_3		= 0x43,
	XFER_UDMA_2		= 0x42,
	XFER_UDMA_1		= 0x41,
	XFER_UDMA_0		= 0x40,
	XFER_MW_DMA_4		= 0x24,	/* CFA only */
	XFER_MW_DMA_3		= 0x23,	/* CFA only */
	XFER_MW_DMA_2		= 0x22,
	XFER_MW_DMA_1		= 0x21,
	XFER_MW_DMA_0		= 0x20,
	XFER_SW_DMA_2		= 0x12,
	XFER_SW_DMA_1		= 0x11,
	XFER_SW_DMA_0		= 0x10,
	XFER_PIO_6		= 0x0E,	/* CFA only */
	XFER_PIO_5		= 0x0D,	/* CFA only */
	XFER_PIO_4		= 0x0C,
	XFER_PIO_3		= 0x0B,
	XFER_PIO_2		= 0x0A,
	XFER_PIO_1		= 0x09,
	XFER_PIO_0		= 0x08,
	XFER_PIO_SLOW		= 0x00,

	SETFEATURES_WC_ON	= 0x02, /* Enable write cache */
	SETFEATURES_WC_OFF	= 0x82, /* Disable write cache */

	SETFEATURES_RA_ON	= 0xaa, /* Enable read look-ahead */
	SETFEATURES_RA_OFF	= 0x55, /* Disable read look-ahead */

	/* Enable/Disable Automatic Acoustic Management */
	SETFEATURES_AAM_ON	= 0x42,
	SETFEATURES_AAM_OFF	= 0xC2,

	SETFEATURES_SPINUP		= 0x07, /* Spin-up drive */
	SETFEATURES_SPINUP_TIMEOUT	= 30000, /* 30s timeout for drive spin-up from PUIS */

	SETFEATURES_SATA_ENABLE = 0x10, /* Enable use of SATA feature */
	SETFEATURES_SATA_DISABLE = 0x90, /* Disable use of SATA feature */

	/* SETFEATURE Sector counts for SATA features */
	SATA_FPDMA_OFFSET	= 0x01,	/* FPDMA non-zero buffer offsets */
	SATA_FPDMA_AA		= 0x02, /* FPDMA Setup FIS Auto-Activate */
	SATA_DIPM		= 0x03,	/* Device Initiated Power Management */
	SATA_FPDMA_IN_ORDER	= 0x04,	/* FPDMA in-order data delivery */
	SATA_AN			= 0x05,	/* Asynchronous Notification */
	SATA_SSP		= 0x06,	/* Software Settings Preservation */
	SATA_DEVSLP		= 0x09,	/* Device Sleep */

	SETFEATURE_SENSE_DATA	= 0xC3, /* Sense Data Reporting feature */

	/* feature values for SET_MAX */
	ATA_SET_MAX_ADDR	= 0x00,
	ATA_SET_MAX_PASSWD	= 0x01,
	ATA_SET_MAX_LOCK	= 0x02,
	ATA_SET_MAX_UNLOCK	= 0x03,
	ATA_SET_MAX_FREEZE_LOCK	= 0x04,
	ATA_SET_MAX_PASSWD_DMA	= 0x05,
	ATA_SET_MAX_UNLOCK_DMA	= 0x06,

	/* feature values for DEVICE CONFIGURATION OVERLAY */
	ATA_DCO_RESTORE		= 0xC0,
	ATA_DCO_FREEZE_LOCK	= 0xC1,
	ATA_DCO_IDENTIFY	= 0xC2,
	ATA_DCO_SET		= 0xC3,

	/* feature values for SMART */
	ATA_SMART_ENABLE	= 0xD8,
	ATA_SMART_READ_VALUES	= 0xD0,
	ATA_SMART_READ_THRESHOLDS = 0xD1,

	/* feature values for Data Set Management */
	ATA_DSM_TRIM		= 0x01,

	/* password used in LBA Mid / LBA High for executing SMART commands */
	ATA_SMART_LBAM_PASS	= 0x4F,
	ATA_SMART_LBAH_PASS	= 0xC2,

	/* ATAPI stuff */
	ATAPI_PKT_DMA		= (1 << 0),
	ATAPI_DMADIR		= (1 << 2),	/* ATAPI data dir:
						   0=to device, 1=to host */
	ATAPI_CDB_LEN		= 16,

	/* PMP stuff */
	SATA_PMP_MAX_PORTS	= 15,
	SATA_PMP_CTRL_PORT	= 15,

	SATA_PMP_GSCR_DWORDS	= 128,
	SATA_PMP_GSCR_PROD_ID	= 0,
	SATA_PMP_GSCR_REV	= 1,
	SATA_PMP_GSCR_PORT_INFO	= 2,
	SATA_PMP_GSCR_ERROR	= 32,
	SATA_PMP_GSCR_ERROR_EN	= 33,
	SATA_PMP_GSCR_FEAT	= 64,
	SATA_PMP_GSCR_FEAT_EN	= 96,

	SATA_PMP_PSCR_STATUS	= 0,
	SATA_PMP_PSCR_ERROR	= 1,
	SATA_PMP_PSCR_CONTROL	= 2,

	SATA_PMP_FEAT_BIST	= (1 << 0),
	SATA_PMP_FEAT_PMREQ	= (1 << 1),
	SATA_PMP_FEAT_DYNSSC	= (1 << 2),
	SATA_PMP_FEAT_NOTIFY	= (1 << 3),

	/* cable types */
	ATA_CBL_NONE		= 0,
	ATA_CBL_PATA40		= 1,
	ATA_CBL_PATA80		= 2,
	ATA_CBL_PATA40_SHORT	= 3,	/* 40 wire cable to high UDMA spec */
	ATA_CBL_PATA_UNK	= 4,	/* don't know, maybe 80c? */
	ATA_CBL_PATA_IGN	= 5,	/* don't know, ignore cable handling */
	ATA_CBL_SATA		= 6,

	/* SATA Status and Control Registers */
	SCR_STATUS		= 0,
	SCR_ERROR		= 1,
	SCR_CONTROL		= 2,
	SCR_ACTIVE		= 3,
	SCR_NOTIFICATION	= 4,

	/* SError bits */
	SERR_DATA_RECOVERED	= (1 << 0), /* recovered data error */
	SERR_COMM_RECOVERED	= (1 << 1), /* recovered comm failure */
	SERR_DATA		= (1 << 8), /* unrecovered data error */
	SERR_PERSISTENT		= (1 << 9), /* persistent data/comm error */
	SERR_PROTOCOL		= (1 << 10), /* protocol violation */
	SERR_INTERNAL		= (1 << 11), /* host internal error */
	SERR_PHYRDY_CHG		= (1 << 16), /* PHY RDY changed */
	SERR_PHY_INT_ERR	= (1 << 17), /* PHY internal error */
	SERR_COMM_WAKE		= (1 << 18), /* Comm wake */
	SERR_10B_8B_ERR		= (1 << 19), /* 10b to 8b decode error */
	SERR_DISPARITY		= (1 << 20), /* Disparity */
	SERR_CRC		= (1 << 21), /* CRC error */
	SERR_HANDSHAKE		= (1 << 22), /* Handshake error */
	SERR_LINK_SEQ_ERR	= (1 << 23), /* Link sequence error */
	SERR_TRANS_ST_ERROR	= (1 << 24), /* Transport state trans. error */
	SERR_UNRECOG_FIS	= (1 << 25), /* Unrecognized FIS */
	SERR_DEV_XCHG		= (1 << 26), /* device exchanged */
};

enum ata_prot_flags {
	/* protocol flags */
	ATA_PROT_FLAG_PIO	= (1 << 0), /* is PIO */
	ATA_PROT_FLAG_DMA	= (1 << 1), /* is DMA */
	ATA_PROT_FLAG_NCQ	= (1 << 2), /* is NCQ */
	ATA_PROT_FLAG_ATAPI	= (1 << 3), /* is ATAPI */

	/* taskfile protocols */
	ATA_PROT_UNKNOWN	= (u8)-1,
	ATA_PROT_NODATA		= 0,
	ATA_PROT_PIO		= ATA_PROT_FLAG_PIO,
	ATA_PROT_DMA		= ATA_PROT_FLAG_DMA,
	ATA_PROT_NCQ_NODATA	= ATA_PROT_FLAG_NCQ,
	ATA_PROT_NCQ		= ATA_PROT_FLAG_DMA | ATA_PROT_FLAG_NCQ,
	ATAPI_PROT_NODATA	= ATA_PROT_FLAG_ATAPI,
	ATAPI_PROT_PIO		= ATA_PROT_FLAG_ATAPI | ATA_PROT_FLAG_PIO,
	ATAPI_PROT_DMA		= ATA_PROT_FLAG_ATAPI | ATA_PROT_FLAG_DMA,
};

enum ata_ioctls {
	ATA_IOC_GET_IO32	= 0x309, /* HDIO_GET_32BIT */
	ATA_IOC_SET_IO32	= 0x324, /* HDIO_SET_32BIT */
};

/* core structures */

struct ata_bmdma_prd {
	__le32			addr;
	__le32			flags_len;
};

/*
 * id tests
 */
#define ata_id_is_ata(id)	(((id)[ATA_ID_CONFIG] & (1 << 15)) == 0)
#define ata_id_has_lba(id)	((id)[ATA_ID_CAPABILITY] & (1 << 9))
#define ata_id_has_dma(id)	((id)[ATA_ID_CAPABILITY] & (1 << 8))
#define ata_id_has_ncq(id)	((id)[ATA_ID_SATA_CAPABILITY] & (1 << 8))
#define ata_id_queue_depth(id)	(((id)[ATA_ID_QUEUE_DEPTH] & 0x1f) + 1)
#define ata_id_removable(id)	((id)[ATA_ID_CONFIG] & (1 << 7))
#define ata_id_has_atapi_AN(id)	\
	((((id)[ATA_ID_SATA_CAPABILITY] != 0x0000) && \
	  ((id)[ATA_ID_SATA_CAPABILITY] != 0xffff)) && \
	 ((id)[ATA_ID_FEATURE_SUPP] & (1 << 5)))
#define ata_id_has_fpdma_aa(id)	\
	((((id)[ATA_ID_SATA_CAPABILITY] != 0x0000) && \
	  ((id)[ATA_ID_SATA_CAPABILITY] != 0xffff)) && \
	 ((id)[ATA_ID_FEATURE_SUPP] & (1 << 2)))
#define ata_id_iordy_disable(id) ((id)[ATA_ID_CAPABILITY] & (1 << 10))
#define ata_id_has_iordy(id) ((id)[ATA_ID_CAPABILITY] & (1 << 11))
#define ata_id_u32(id,n)	\
	(((u32) (id)[(n) + 1] << 16) | ((u32) (id)[(n)]))
#define ata_id_u64(id,n)	\
	( ((u64) (id)[(n) + 3] << 48) |	\
	  ((u64) (id)[(n) + 2] << 32) |	\
	  ((u64) (id)[(n) + 1] << 16) |	\
	  ((u64) (id)[(n) + 0]) )

#define ata_id_cdb_intr(id)	(((id)[ATA_ID_CONFIG] & 0x60) == 0x20)
#define ata_id_has_da(id)	((id)[ATA_ID_SATA_CAPABILITY_2] & (1 << 4))
#define ata_id_has_devslp(id)	((id)[ATA_ID_FEATURE_SUPP] & (1 << 8))
#define ata_id_has_ncq_autosense(id) \
				((id)[ATA_ID_FEATURE_SUPP] & (1 << 7))

static inline bool ata_id_has_hipm(const u16 *id)
{
	u16 val = id[ATA_ID_SATA_CAPABILITY];

	if (val == 0 || val == 0xffff)
		return false;

	return val & (1 << 9);
}

static inline bool ata_id_has_dipm(const u16 *id)
{
	u16 val = id[ATA_ID_FEATURE_SUPP];

	if (val == 0 || val == 0xffff)
		return false;

	return val & (1 << 3);
}


static inline bool ata_id_has_fua(const u16 *id)
{
	if ((id[ATA_ID_CFSSE] & 0xC000) != 0x4000)
		return false;
	return id[ATA_ID_CFSSE] & (1 << 6);
}

static inline bool ata_id_has_flush(const u16 *id)
{
	if ((id[ATA_ID_COMMAND_SET_2] & 0xC000) != 0x4000)
		return false;
	return id[ATA_ID_COMMAND_SET_2] & (1 << 12);
}

static inline bool ata_id_flush_enabled(const u16 *id)
{
	if (ata_id_has_flush(id) == 0)
		return false;
	if ((id[ATA_ID_CSF_DEFAULT] & 0xC000) != 0x4000)
		return false;
	return id[ATA_ID_CFS_ENABLE_2] & (1 << 12);
}

static inline bool ata_id_has_flush_ext(const u16 *id)
{
	if ((id[ATA_ID_COMMAND_SET_2] & 0xC000) != 0x4000)
		return false;
	return id[ATA_ID_COMMAND_SET_2] & (1 << 13);
}

static inline bool ata_id_flush_ext_enabled(const u16 *id)
{
	if (ata_id_has_flush_ext(id) == 0)
		return false;
	if ((id[ATA_ID_CSF_DEFAULT] & 0xC000) != 0x4000)
		return false;
	/*
	 * some Maxtor disks have bit 13 defined incorrectly
	 * so check bit 10 too
	 */
	return (id[ATA_ID_CFS_ENABLE_2] & 0x2400) == 0x2400;
}

static inline u32 ata_id_logical_sector_size(const u16 *id)
{
	/* T13/1699-D Revision 6a, Sep 6, 2008. Page 128.
	 * IDENTIFY DEVICE data, word 117-118.
	 * 0xd000 ignores bit 13 (logical:physical > 1)
	 */
	if ((id[ATA_ID_SECTOR_SIZE] & 0xd000) == 0x5000)
		return (((id[ATA_ID_LOGICAL_SECTOR_SIZE+1] << 16)
			 + id[ATA_ID_LOGICAL_SECTOR_SIZE]) * sizeof(u16)) ;
	return ATA_SECT_SIZE;
}

static inline u8 ata_id_log2_per_physical_sector(const u16 *id)
{
	/* T13/1699-D Revision 6a, Sep 6, 2008. Page 128.
	 * IDENTIFY DEVICE data, word 106.
	 * 0xe000 ignores bit 12 (logical sector > 512 bytes)
	 */
	if ((id[ATA_ID_SECTOR_SIZE] & 0xe000) == 0x6000)
		return (id[ATA_ID_SECTOR_SIZE] & 0xf);
	return 0;
}

/* Offset of logical sectors relative to physical sectors.
 *
 * If device has more than one logical sector per physical sector
 * (aka 512 byte emulation), vendors might offset the "sector 0" address
 * so sector 63 is "naturally aligned" - e.g. FAT partition table.
 * This avoids Read/Mod/Write penalties when using FAT partition table
 * and updating "well aligned" (FS perspective) physical sectors on every
 * transaction.
 */
static inline u16 ata_id_logical_sector_offset(const u16 *id,
	 u8 log2_per_phys)
{
	u16 word_209 = id[209];

	if ((log2_per_phys > 1) && (word_209 & 0xc000) == 0x4000) {
		u16 first = word_209 & 0x3fff;
		if (first > 0)
			return (1 << log2_per_phys) - first;
	}
	return 0;
}

static inline bool ata_id_has_lba48(const u16 *id)
{
	if ((id[ATA_ID_COMMAND_SET_2] & 0xC000) != 0x4000)
		return false;
	if (!ata_id_u64(id, ATA_ID_LBA_CAPACITY_2))
		return false;
	return id[ATA_ID_COMMAND_SET_2] & (1 << 10);
}

static inline bool ata_id_lba48_enabled(const u16 *id)
{
	if (ata_id_has_lba48(id) == 0)
		return false;
	if ((id[ATA_ID_CSF_DEFAULT] & 0xC000) != 0x4000)
		return false;
	return id[ATA_ID_CFS_ENABLE_2] & (1 << 10);
}

static inline bool ata_id_hpa_enabled(const u16 *id)
{
	/* Yes children, word 83 valid bits cover word 82 data */
	if ((id[ATA_ID_COMMAND_SET_2] & 0xC000) != 0x4000)
		return false;
	/* And 87 covers 85-87 */
	if ((id[ATA_ID_CSF_DEFAULT] & 0xC000) != 0x4000)
		return false;
	/* Check command sets enabled as well as supported */
	if ((id[ATA_ID_CFS_ENABLE_1] & (1 << 10)) == 0)
		return false;
	return id[ATA_ID_COMMAND_SET_1] & (1 << 10);
}

static inline bool ata_id_has_wcache(const u16 *id)
{
	/* Yes children, word 83 valid bits cover word 82 data */
	if ((id[ATA_ID_COMMAND_SET_2] & 0xC000) != 0x4000)
		return false;
	return id[ATA_ID_COMMAND_SET_1] & (1 << 5);
}

static inline bool ata_id_has_pm(const u16 *id)
{
	if ((id[ATA_ID_COMMAND_SET_2] & 0xC000) != 0x4000)
		return false;
	return id[ATA_ID_COMMAND_SET_1] & (1 << 3);
}

static inline bool ata_id_rahead_enabled(const u16 *id)
{
	if ((id[ATA_ID_CSF_DEFAULT] & 0xC000) != 0x4000)
		return false;
	return id[ATA_ID_CFS_ENABLE_1] & (1 << 6);
}

static inline bool ata_id_wcache_enabled(const u16 *id)
{
	if ((id[ATA_ID_CSF_DEFAULT] & 0xC000) != 0x4000)
		return false;
	return id[ATA_ID_CFS_ENABLE_1] & (1 << 5);
}

static inline bool ata_id_has_read_log_dma_ext(const u16 *id)
{
	/* Word 86 must have bit 15 set */
	if (!(id[ATA_ID_CFS_ENABLE_2] & (1 << 15)))
		return false;

	/* READ LOG DMA EXT support can be signaled either from word 119
	 * or from word 120. The format is the same for both words: Bit
	 * 15 must be cleared, bit 14 set and bit 3 set.
	 */
	if ((id[ATA_ID_COMMAND_SET_3] & 0xC008) == 0x4008 ||
	    (id[ATA_ID_COMMAND_SET_4] & 0xC008) == 0x4008)
		return true;

	return false;
}

static inline bool ata_id_has_sense_reporting(const u16 *id)
{
	if (!(id[ATA_ID_CFS_ENABLE_2] & (1 << 15)))
		return false;
	return id[ATA_ID_COMMAND_SET_3] & (1 << 6);
}

static inline bool ata_id_sense_reporting_enabled(const u16 *id)
{
	if (!(id[ATA_ID_CFS_ENABLE_2] & (1 << 15)))
		return false;
	return id[ATA_ID_COMMAND_SET_4] & (1 << 6);
}

/**
 *
 * Word: 206 - SCT Command Transport
 *    15:12 - Vendor Specific
 *     11:6 - Reserved
 *        5 - SCT Command Transport Data Tables supported
 *        4 - SCT Command Transport Features Control supported
 *        3 - SCT Command Transport Error Recovery Control supported
 *        2 - SCT Command Transport Write Same supported
 *        1 - SCT Command Transport Long Sector Access supported
 *        0 - SCT Command Transport supported
 */
static inline bool ata_id_sct_data_tables(const u16 *id)
{
	return id[ATA_ID_SCT_CMD_XPORT] & (1 << 5) ? true : false;
}

static inline bool ata_id_sct_features_ctrl(const u16 *id)
{
	return id[ATA_ID_SCT_CMD_XPORT] & (1 << 4) ? true : false;
}

static inline bool ata_id_sct_error_recovery_ctrl(const u16 *id)
{
	return id[ATA_ID_SCT_CMD_XPORT] & (1 << 3) ? true : false;
}

static inline bool ata_id_sct_long_sector_access(const u16 *id)
{
	return id[ATA_ID_SCT_CMD_XPORT] & (1 << 1) ? true : false;
}

static inline bool ata_id_sct_supported(const u16 *id)
{
	return id[ATA_ID_SCT_CMD_XPORT] & (1 << 0) ? true : false;
}

/**
 *	ata_id_major_version	-	get ATA level of drive
 *	@id: Identify data
 *
 *	Caveats:
 *		ATA-1 considers identify optional
 *		ATA-2 introduces mandatory identify
 *		ATA-3 introduces word 80 and accurate reporting
 *
 *	The practical impact of this is that ata_id_major_version cannot
 *	reliably report on drives below ATA3.
 */

static inline unsigned int ata_id_major_version(const u16 *id)
{
	unsigned int mver;

	if (id[ATA_ID_MAJOR_VER] == 0xFFFF)
		return 0;

	for (mver = 14; mver >= 1; mver--)
		if (id[ATA_ID_MAJOR_VER] & (1 << mver))
			break;
	return mver;
}

static inline bool ata_id_is_sata(const u16 *id)
{
	/*
	 * See if word 93 is 0 AND drive is at least ATA-5 compatible
	 * verifying that word 80 by casting it to a signed type --
	 * this trick allows us to filter out the reserved values of
	 * 0x0000 and 0xffff along with the earlier ATA revisions...
	 */
	if (id[ATA_ID_HW_CONFIG] == 0 && (short)id[ATA_ID_MAJOR_VER] >= 0x0020)
		return true;
	return false;
}

static inline bool ata_id_has_tpm(const u16 *id)
{
	/* The TPM bits are only valid on ATA8 */
	if (ata_id_major_version(id) < 8)
		return false;
	if ((id[48] & 0xC000) != 0x4000)
		return false;
	return id[48] & (1 << 0);
}

static inline bool ata_id_has_dword_io(const u16 *id)
{
	/* ATA 8 reuses this flag for "trusted" computing */
	if (ata_id_major_version(id) > 7)
		return false;
	return id[ATA_ID_DWORD_IO] & (1 << 0);
}

static inline bool ata_id_has_trusted(const u16 *id)
{
	if (ata_id_major_version(id) <= 7)
		return false;
	return id[ATA_ID_TRUSTED] & (1 << 0);
}

static inline bool ata_id_has_unload(const u16 *id)
{
	if (ata_id_major_version(id) >= 7 &&
	    (id[ATA_ID_CFSSE] & 0xC000) == 0x4000 &&
	    id[ATA_ID_CFSSE] & (1 << 13))
		return true;
	return false;
}

static inline bool ata_id_has_wwn(const u16 *id)
{
	return (id[ATA_ID_CSF_DEFAULT] & 0xC100) == 0x4100;
}

static inline int ata_id_form_factor(const u16 *id)
{
	u16 val = id[168];

	if (ata_id_major_version(id) < 7 || val == 0 || val == 0xffff)
		return 0;

	val &= 0xf;

	if (val > 5)
		return 0;

	return val;
}

static inline int ata_id_rotation_rate(const u16 *id)
{
	u16 val = id[217];

	if (ata_id_major_version(id) < 7 || val == 0 || val == 0xffff)
		return 0;

	if (val > 1 && val < 0x401)
		return 0;

	return val;
}

static inline bool ata_id_has_ncq_send_and_recv(const u16 *id)
{
	return id[ATA_ID_SATA_CAPABILITY_2] & BIT(6);
}

static inline bool ata_id_has_ncq_non_data(const u16 *id)
{
	return id[ATA_ID_SATA_CAPABILITY_2] & BIT(5);
}

static inline bool ata_id_has_ncq_prio(const u16 *id)
{
	return id[ATA_ID_SATA_CAPABILITY] & BIT(12);
}

static inline bool ata_id_has_trim(const u16 *id)
{
	if (ata_id_major_version(id) >= 7 &&
	    (id[ATA_ID_DATA_SET_MGMT] & 1))
		return true;
	return false;
}

static inline bool ata_id_has_zero_after_trim(const u16 *id)
{
	/* DSM supported, deterministic read, and read zero after trim set */
	if (ata_id_has_trim(id) &&
	    (id[ATA_ID_ADDITIONAL_SUPP] & 0x4020) == 0x4020)
		return true;

	return false;
}

static inline bool ata_id_current_chs_valid(const u16 *id)
{
	/* For ATA-1 devices, if the INITIALIZE DEVICE PARAMETERS command
	   has not been issued to the device then the values of
	   id[ATA_ID_CUR_CYLS] to id[ATA_ID_CUR_SECTORS] are vendor specific. */
	return (id[ATA_ID_FIELD_VALID] & 1) && /* Current translation valid */
		id[ATA_ID_CUR_CYLS] &&  /* cylinders in current translation */
		id[ATA_ID_CUR_HEADS] &&  /* heads in current translation */
		id[ATA_ID_CUR_HEADS] <= 16 &&
		id[ATA_ID_CUR_SECTORS];    /* sectors in current translation */
}

static inline bool ata_id_is_cfa(const u16 *id)
{
	if ((id[ATA_ID_CONFIG] == 0x848A) ||	/* Traditional CF */
	    (id[ATA_ID_CONFIG] == 0x844A))	/* Delkin Devices CF */
		return true;
	/*
	 * CF specs don't require specific value in the word 0 anymore and yet
	 * they forbid to report the ATA version in the word 80 and require the
	 * CFA feature set support to be indicated in the word 83 in this case.
	 * Unfortunately, some cards only follow either of this requirements,
	 * and while those that don't indicate CFA feature support need some
	 * sort of quirk list, it seems impractical for the ones that do...
	 */
	return (id[ATA_ID_COMMAND_SET_2] & 0xC004) == 0x4004;
}

static inline bool ata_id_is_ssd(const u16 *id)
{
	return id[ATA_ID_ROT_SPEED] == 0x01;
}

static inline u8 ata_id_zoned_cap(const u16 *id)
{
	return (id[ATA_ID_ADDITIONAL_SUPP] & 0x3);
}

static inline bool ata_id_pio_need_iordy(const u16 *id, const u8 pio)
{
	/* CF spec. r4.1 Table 22 says no IORDY on PIO5 and PIO6. */
	if (pio > 4 && ata_id_is_cfa(id))
		return false;
	/* For PIO3 and higher it is mandatory. */
	if (pio > 2)
		return true;
	/* Turn it on when possible. */
	return ata_id_has_iordy(id);
}

static inline bool ata_drive_40wire(const u16 *dev_id)
{
	if (ata_id_is_sata(dev_id))
		return false;	/* SATA */
	if ((dev_id[ATA_ID_HW_CONFIG] & 0xE000) == 0x6000)
		return false;	/* 80 wire */
	return true;
}

static inline bool ata_drive_40wire_relaxed(const u16 *dev_id)
{
	if ((dev_id[ATA_ID_HW_CONFIG] & 0x2000) == 0x2000)
		return false;	/* 80 wire */
	return true;
}

static inline int atapi_cdb_len(const u16 *dev_id)
{
	u16 tmp = dev_id[ATA_ID_CONFIG] & 0x3;
	switch (tmp) {
	case 0:		return 12;
	case 1:		return 16;
	default:	return -1;
	}
}

static inline int atapi_command_packet_set(const u16 *dev_id)
{
	return (dev_id[ATA_ID_CONFIG] >> 8) & 0x1f;
}

static inline bool atapi_id_dmadir(const u16 *dev_id)
{
	return ata_id_major_version(dev_id) >= 7 && (dev_id[62] & 0x8000);
}

/*
 * ata_id_is_lba_capacity_ok() performs a sanity check on
 * the claimed LBA capacity value for the device.
 *
 * Returns 1 if LBA capacity looks sensible, 0 otherwise.
 *
 * It is called only once for each device.
 */
static inline bool ata_id_is_lba_capacity_ok(u16 *id)
{
	unsigned long lba_sects, chs_sects, head, tail;

	/* No non-LBA info .. so valid! */
	if (id[ATA_ID_CYLS] == 0)
		return true;

	lba_sects = ata_id_u32(id, ATA_ID_LBA_CAPACITY);

	/*
	 * The ATA spec tells large drives to return
	 * C/H/S = 16383/16/63 independent of their size.
	 * Some drives can be jumpered to use 15 heads instead of 16.
	 * Some drives can be jumpered to use 4092 cyls instead of 16383.
	 */
	if ((id[ATA_ID_CYLS] == 16383 ||
	     (id[ATA_ID_CYLS] == 4092 && id[ATA_ID_CUR_CYLS] == 16383)) &&
	    id[ATA_ID_SECTORS] == 63 &&
	    (id[ATA_ID_HEADS] == 15 || id[ATA_ID_HEADS] == 16) &&
	    (lba_sects >= 16383 * 63 * id[ATA_ID_HEADS]))
		return true;

	chs_sects = id[ATA_ID_CYLS] * id[ATA_ID_HEADS] * id[ATA_ID_SECTORS];

	/* perform a rough sanity check on lba_sects: within 10% is OK */
	if (lba_sects - chs_sects < chs_sects/10)
		return true;

	/* some drives have the word order reversed */
	head = (lba_sects >> 16) & 0xffff;
	tail = lba_sects & 0xffff;
	lba_sects = head | (tail << 16);

	if (lba_sects - chs_sects < chs_sects/10) {
		*(__le32 *)&id[ATA_ID_LBA_CAPACITY] = __cpu_to_le32(lba_sects);
		return true;	/* LBA capacity is (now) good */
	}

	return false;	/* LBA capacity value may be bad */
}

static inline void ata_id_to_hd_driveid(u16 *id)
{
#ifdef __BIG_ENDIAN
	/* accessed in struct hd_driveid as 8-bit values */
	id[ATA_ID_MAX_MULTSECT]	 = __cpu_to_le16(id[ATA_ID_MAX_MULTSECT]);
	id[ATA_ID_CAPABILITY]	 = __cpu_to_le16(id[ATA_ID_CAPABILITY]);
	id[ATA_ID_OLD_PIO_MODES] = __cpu_to_le16(id[ATA_ID_OLD_PIO_MODES]);
	id[ATA_ID_OLD_DMA_MODES] = __cpu_to_le16(id[ATA_ID_OLD_DMA_MODES]);
	id[ATA_ID_MULTSECT]	 = __cpu_to_le16(id[ATA_ID_MULTSECT]);

	/* as 32-bit values */
	*(u32 *)&id[ATA_ID_LBA_CAPACITY] = ata_id_u32(id, ATA_ID_LBA_CAPACITY);
	*(u32 *)&id[ATA_ID_SPG]		 = ata_id_u32(id, ATA_ID_SPG);

	/* as 64-bit value */
	*(u64 *)&id[ATA_ID_LBA_CAPACITY_2] =
		ata_id_u64(id, ATA_ID_LBA_CAPACITY_2);
#endif
}

static inline bool ata_ok(u8 status)
{
	return ((status & (ATA_BUSY | ATA_DRDY | ATA_DF | ATA_DRQ | ATA_ERR))
			== ATA_DRDY);
}

static inline bool lba_28_ok(u64 block, u32 n_block)
{
	/* check the ending block number: must be LESS THAN 0x0fffffff */
	return ((block + n_block) < ((1 << 28) - 1)) && (n_block <= ATA_MAX_SECTORS);
}

static inline bool lba_48_ok(u64 block, u32 n_block)
{
	/* check the ending block number */
	return ((block + n_block - 1) < ((u64)1 << 48)) && (n_block <= ATA_MAX_SECTORS_LBA48);
}

#define sata_pmp_gscr_vendor(gscr)	((gscr)[SATA_PMP_GSCR_PROD_ID] & 0xffff)
#define sata_pmp_gscr_devid(gscr)	((gscr)[SATA_PMP_GSCR_PROD_ID] >> 16)
#define sata_pmp_gscr_rev(gscr)		(((gscr)[SATA_PMP_GSCR_REV] >> 8) & 0xff)
#define sata_pmp_gscr_ports(gscr)	((gscr)[SATA_PMP_GSCR_PORT_INFO] & 0xf)

#endif /* __LINUX_ATA_H__ */
