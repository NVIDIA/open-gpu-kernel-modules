// SPDX-License-Identifier: GPL-2.0
#ifndef __LINUX_USB_STORAGE_H
#define __LINUX_USB_STORAGE_H

/*
 * linux/usb/storage.h
 *
 * Copyright Matthew Wilcox for Intel Corp, 2010
 *
 * This file contains definitions taken from the
 * USB Mass Storage Class Specification Overview
 *
 * Distributed under the terms of the GNU GPL, version two.
 */

/* Storage subclass codes */

#define USB_SC_RBC	0x01		/* Typically, flash devices */
#define USB_SC_8020	0x02		/* CD-ROM */
#define USB_SC_QIC	0x03		/* QIC-157 Tapes */
#define USB_SC_UFI	0x04		/* Floppy */
#define USB_SC_8070	0x05		/* Removable media */
#define USB_SC_SCSI	0x06		/* Transparent */
#define USB_SC_LOCKABLE	0x07		/* Password-protected */

#define USB_SC_ISD200	0xf0		/* ISD200 ATA */
#define USB_SC_CYP_ATACB	0xf1	/* Cypress ATACB */
#define USB_SC_DEVICE	0xff		/* Use device's value */

/* Storage protocol codes */

#define USB_PR_CBI	0x00		/* Control/Bulk/Interrupt */
#define USB_PR_CB	0x01		/* Control/Bulk w/o interrupt */
#define USB_PR_BULK	0x50		/* bulk only */
#define USB_PR_UAS	0x62		/* USB Attached SCSI */

#define USB_PR_USBAT	0x80		/* SCM-ATAPI bridge */
#define USB_PR_EUSB_SDDR09	0x81	/* SCM-SCSI bridge for SDDR-09 */
#define USB_PR_SDDR55	0x82		/* SDDR-55 (made up) */
#define USB_PR_DPCM_USB	0xf0		/* Combination CB/SDDR09 */
#define USB_PR_FREECOM	0xf1		/* Freecom */
#define USB_PR_DATAFAB	0xf2		/* Datafab chipsets */
#define USB_PR_JUMPSHOT	0xf3		/* Lexar Jumpshot */
#define USB_PR_ALAUDA	0xf4		/* Alauda chipsets */
#define USB_PR_KARMA	0xf5		/* Rio Karma */

#define USB_PR_DEVICE	0xff		/* Use device's value */

/*
 * Bulk only data structures
 */

/* command block wrapper */
struct bulk_cb_wrap {
	__le32	Signature;		/* contains 'USBC' */
	__u32	Tag;			/* unique per command id */
	__le32	DataTransferLength;	/* size of data */
	__u8	Flags;			/* direction in bit 0 */
	__u8	Lun;			/* LUN normally 0 */
	__u8	Length;			/* length of the CDB */
	__u8	CDB[16];		/* max command */
};

#define US_BULK_CB_WRAP_LEN	31
#define US_BULK_CB_SIGN		0x43425355	/* spells out 'USBC' */
#define US_BULK_FLAG_IN		(1 << 7)
#define US_BULK_FLAG_OUT	0

/* command status wrapper */
struct bulk_cs_wrap {
	__le32	Signature;	/* contains 'USBS' */
	__u32	Tag;		/* same as original command */
	__le32	Residue;	/* amount not transferred */
	__u8	Status;		/* see below */
};

#define US_BULK_CS_WRAP_LEN	13
#define US_BULK_CS_SIGN		0x53425355      /* spells out 'USBS' */
#define US_BULK_STAT_OK		0
#define US_BULK_STAT_FAIL	1
#define US_BULK_STAT_PHASE	2

/* bulk-only class specific requests */
#define US_BULK_RESET_REQUEST   0xff
#define US_BULK_GET_MAX_LUN     0xfe

#endif
