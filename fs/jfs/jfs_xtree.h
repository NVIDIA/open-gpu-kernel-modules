/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *   Copyright (C) International Business Machines Corp., 2000-2002
 */
#ifndef _H_JFS_XTREE
#define _H_JFS_XTREE

/*
 *	jfs_xtree.h: extent allocation descriptor B+-tree manager
 */

#include "jfs_btree.h"


/*
 *	extent allocation descriptor (xad)
 */
typedef struct xad {
	__u8 flag;	/* 1: flag */
	__u8 rsvrd[2];	/* 2: reserved */
	__u8 off1;	/* 1: offset in unit of fsblksize */
	__le32 off2;	/* 4: offset in unit of fsblksize */
	pxd_t loc;	/* 8: length and address in unit of fsblksize */
} xad_t;			/* (16) */

#define MAXXLEN		((1 << 24) - 1)

#define XTSLOTSIZE	16
#define L2XTSLOTSIZE	4

/* xad_t field construction */
#define XADoffset(xad, offset64)\
{\
	(xad)->off1 = ((u64)offset64) >> 32;\
	(xad)->off2 = __cpu_to_le32((offset64) & 0xffffffff);\
}
#define XADaddress(xad, address64) PXDaddress(&(xad)->loc, address64)
#define XADlength(xad, length32) PXDlength(&(xad)->loc, length32)

/* xad_t field extraction */
#define offsetXAD(xad)\
	( ((s64)((xad)->off1)) << 32 | __le32_to_cpu((xad)->off2))
#define addressXAD(xad) addressPXD(&(xad)->loc)
#define lengthXAD(xad) lengthPXD(&(xad)->loc)

/* xad list */
struct xadlist {
	s16 maxnxad;
	s16 nxad;
	xad_t *xad;
};

/* xad_t flags */
#define XAD_NEW		0x01	/* new */
#define XAD_EXTENDED	0x02	/* extended */
#define XAD_COMPRESSED	0x04	/* compressed with recorded length */
#define XAD_NOTRECORDED 0x08	/* allocated but not recorded */
#define XAD_COW		0x10	/* copy-on-write */


/* possible values for maxentry */
#define XTROOTINITSLOT_DIR 6
#define XTROOTINITSLOT	10
#define XTROOTMAXSLOT	18
#define XTPAGEMAXSLOT	256
#define XTENTRYSTART	2

/*
 *	xtree page:
 */
typedef union {
	struct xtheader {
		__le64 next;	/* 8: */
		__le64 prev;	/* 8: */

		u8 flag;	/* 1: */
		u8 rsrvd1;	/* 1: */
		__le16 nextindex;	/* 2: next index = number of entries */
		__le16 maxentry;	/* 2: max number of entries */
		__le16 rsrvd2;	/* 2: */

		pxd_t self;	/* 8: self */
	} header;		/* (32) */

	xad_t xad[XTROOTMAXSLOT];	/* 16 * maxentry: xad array */
} xtpage_t;

/*
 *	external declaration
 */
extern int xtLookup(struct inode *ip, s64 lstart, s64 llen,
		    int *pflag, s64 * paddr, int *plen, int flag);
extern void xtInitRoot(tid_t tid, struct inode *ip);
extern int xtInsert(tid_t tid, struct inode *ip,
		    int xflag, s64 xoff, int xlen, s64 * xaddrp, int flag);
extern int xtExtend(tid_t tid, struct inode *ip, s64 xoff, int xlen,
		    int flag);
#ifdef _NOTYET
extern int xtTailgate(tid_t tid, struct inode *ip,
		      s64 xoff, int xlen, s64 xaddr, int flag);
#endif
extern int xtUpdate(tid_t tid, struct inode *ip, struct xad *nxad);
extern int xtDelete(tid_t tid, struct inode *ip, s64 xoff, int xlen,
		    int flag);
extern s64 xtTruncate(tid_t tid, struct inode *ip, s64 newsize, int type);
extern s64 xtTruncate_pmap(tid_t tid, struct inode *ip, s64 committed_size);
extern int xtRelocate(tid_t tid, struct inode *ip,
		      xad_t * oxad, s64 nxaddr, int xtype);
extern int xtAppend(tid_t tid,
		    struct inode *ip, int xflag, s64 xoff, int maxblocks,
		    int *xlenp, s64 * xaddrp, int flag);
#endif				/* !_H_JFS_XTREE */
