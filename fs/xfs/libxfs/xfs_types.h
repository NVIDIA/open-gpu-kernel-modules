// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2000-2005 Silicon Graphics, Inc.
 * All Rights Reserved.
 */
#ifndef __XFS_TYPES_H__
#define	__XFS_TYPES_H__

typedef uint32_t	prid_t;		/* project ID */

typedef uint32_t	xfs_agblock_t;	/* blockno in alloc. group */
typedef uint32_t	xfs_agino_t;	/* inode # within allocation grp */
typedef uint32_t	xfs_extlen_t;	/* extent length in blocks */
typedef uint32_t	xfs_agnumber_t;	/* allocation group number */
typedef int32_t		xfs_extnum_t;	/* # of extents in a file */
typedef int16_t		xfs_aextnum_t;	/* # extents in an attribute fork */
typedef int64_t		xfs_fsize_t;	/* bytes in a file */
typedef uint64_t	xfs_ufsize_t;	/* unsigned bytes in a file */

typedef int32_t		xfs_suminfo_t;	/* type of bitmap summary info */
typedef uint32_t	xfs_rtword_t;	/* word type for bitmap manipulations */

typedef int64_t		xfs_lsn_t;	/* log sequence number */

typedef uint32_t	xfs_dablk_t;	/* dir/attr block number (in file) */
typedef uint32_t	xfs_dahash_t;	/* dir/attr hash value */

typedef uint64_t	xfs_fsblock_t;	/* blockno in filesystem (agno|agbno) */
typedef uint64_t	xfs_rfsblock_t;	/* blockno in filesystem (raw) */
typedef uint64_t	xfs_rtblock_t;	/* extent (block) in realtime area */
typedef uint64_t	xfs_fileoff_t;	/* block number in a file */
typedef uint64_t	xfs_filblks_t;	/* number of blocks in a file */

typedef int64_t		xfs_srtblock_t;	/* signed version of xfs_rtblock_t */

/*
 * New verifiers will return the instruction address of the failing check.
 * NULL means everything is ok.
 */
typedef void *		xfs_failaddr_t;

/*
 * Null values for the types.
 */
#define	NULLFSBLOCK	((xfs_fsblock_t)-1)
#define	NULLRFSBLOCK	((xfs_rfsblock_t)-1)
#define	NULLRTBLOCK	((xfs_rtblock_t)-1)
#define	NULLFILEOFF	((xfs_fileoff_t)-1)

#define	NULLAGBLOCK	((xfs_agblock_t)-1)
#define	NULLAGNUMBER	((xfs_agnumber_t)-1)

#define NULLCOMMITLSN	((xfs_lsn_t)-1)

#define	NULLFSINO	((xfs_ino_t)-1)
#define	NULLAGINO	((xfs_agino_t)-1)

/*
 * Max values for extlen, extnum, aextnum.
 */
#define	MAXEXTLEN	((xfs_extlen_t)0x001fffff)	/* 21 bits */
#define	MAXEXTNUM	((xfs_extnum_t)0x7fffffff)	/* signed int */
#define	MAXAEXTNUM	((xfs_aextnum_t)0x7fff)		/* signed short */

/*
 * Minimum and maximum blocksize and sectorsize.
 * The blocksize upper limit is pretty much arbitrary.
 * The sectorsize upper limit is due to sizeof(sb_sectsize).
 * CRC enable filesystems use 512 byte inodes, meaning 512 byte block sizes
 * cannot be used.
 */
#define XFS_MIN_BLOCKSIZE_LOG	9	/* i.e. 512 bytes */
#define XFS_MAX_BLOCKSIZE_LOG	16	/* i.e. 65536 bytes */
#define XFS_MIN_BLOCKSIZE	(1 << XFS_MIN_BLOCKSIZE_LOG)
#define XFS_MAX_BLOCKSIZE	(1 << XFS_MAX_BLOCKSIZE_LOG)
#define XFS_MIN_CRC_BLOCKSIZE	(1 << (XFS_MIN_BLOCKSIZE_LOG + 1))
#define XFS_MIN_SECTORSIZE_LOG	9	/* i.e. 512 bytes */
#define XFS_MAX_SECTORSIZE_LOG	15	/* i.e. 32768 bytes */
#define XFS_MIN_SECTORSIZE	(1 << XFS_MIN_SECTORSIZE_LOG)
#define XFS_MAX_SECTORSIZE	(1 << XFS_MAX_SECTORSIZE_LOG)

/*
 * Inode fork identifiers.
 */
#define	XFS_DATA_FORK	0
#define	XFS_ATTR_FORK	1
#define	XFS_COW_FORK	2

/*
 * Min numbers of data/attr fork btree root pointers.
 */
#define MINDBTPTRS	3
#define MINABTPTRS	2

/*
 * MAXNAMELEN is the length (including the terminating null) of
 * the longest permissible file (component) name.
 */
#define MAXNAMELEN	256

/*
 * This enum is used in string mapping in xfs_trace.h; please keep the
 * TRACE_DEFINE_ENUMs for it up to date.
 */
typedef enum {
	XFS_LOOKUP_EQi, XFS_LOOKUP_LEi, XFS_LOOKUP_GEi
} xfs_lookup_t;

#define XFS_AG_BTREE_CMP_FORMAT_STR \
	{ XFS_LOOKUP_EQi,	"eq" }, \
	{ XFS_LOOKUP_LEi,	"le" }, \
	{ XFS_LOOKUP_GEi,	"ge" }

/*
 * This enum is used in string mapping in xfs_trace.h and scrub/trace.h;
 * please keep the TRACE_DEFINE_ENUMs for it up to date.
 */
typedef enum {
	XFS_BTNUM_BNOi, XFS_BTNUM_CNTi, XFS_BTNUM_RMAPi, XFS_BTNUM_BMAPi,
	XFS_BTNUM_INOi, XFS_BTNUM_FINOi, XFS_BTNUM_REFCi, XFS_BTNUM_MAX
} xfs_btnum_t;

#define XFS_BTNUM_STRINGS \
	{ XFS_BTNUM_BNOi,	"bnobt" }, \
	{ XFS_BTNUM_CNTi,	"cntbt" }, \
	{ XFS_BTNUM_RMAPi,	"rmapbt" }, \
	{ XFS_BTNUM_BMAPi,	"bmbt" }, \
	{ XFS_BTNUM_INOi,	"inobt" }, \
	{ XFS_BTNUM_FINOi,	"finobt" }, \
	{ XFS_BTNUM_REFCi,	"refcbt" }

struct xfs_name {
	const unsigned char	*name;
	int			len;
	int			type;
};

/*
 * uid_t and gid_t are hard-coded to 32 bits in the inode.
 * Hence, an 'id' in a dquot is 32 bits..
 */
typedef uint32_t	xfs_dqid_t;

/*
 * Constants for bit manipulations.
 */
#define	XFS_NBBYLOG	3		/* log2(NBBY) */
#define	XFS_WORDLOG	2		/* log2(sizeof(xfs_rtword_t)) */
#define	XFS_NBWORDLOG	(XFS_NBBYLOG + XFS_WORDLOG)
#define	XFS_NBWORD	(1 << XFS_NBWORDLOG)
#define	XFS_WORDMASK	((1 << XFS_WORDLOG) - 1)

struct xfs_iext_cursor {
	struct xfs_iext_leaf	*leaf;
	int			pos;
};

typedef enum {
	XFS_EXT_NORM, XFS_EXT_UNWRITTEN,
} xfs_exntst_t;

typedef struct xfs_bmbt_irec
{
	xfs_fileoff_t	br_startoff;	/* starting file offset */
	xfs_fsblock_t	br_startblock;	/* starting block number */
	xfs_filblks_t	br_blockcount;	/* number of blocks */
	xfs_exntst_t	br_state;	/* extent state */
} xfs_bmbt_irec_t;

/* per-AG block reservation types */
enum xfs_ag_resv_type {
	XFS_AG_RESV_NONE = 0,
	XFS_AG_RESV_AGFL,
	XFS_AG_RESV_METADATA,
	XFS_AG_RESV_RMAPBT,
};

/*
 * Type verifier functions
 */
struct xfs_mount;

xfs_agblock_t xfs_ag_block_count(struct xfs_mount *mp, xfs_agnumber_t agno);
bool xfs_verify_agbno(struct xfs_mount *mp, xfs_agnumber_t agno,
		xfs_agblock_t agbno);
bool xfs_verify_fsbno(struct xfs_mount *mp, xfs_fsblock_t fsbno);
bool xfs_verify_fsbext(struct xfs_mount *mp, xfs_fsblock_t fsbno,
		xfs_fsblock_t len);

void xfs_agino_range(struct xfs_mount *mp, xfs_agnumber_t agno,
		xfs_agino_t *first, xfs_agino_t *last);
bool xfs_verify_agino(struct xfs_mount *mp, xfs_agnumber_t agno,
		xfs_agino_t agino);
bool xfs_verify_agino_or_null(struct xfs_mount *mp, xfs_agnumber_t agno,
		xfs_agino_t agino);
bool xfs_verify_ino(struct xfs_mount *mp, xfs_ino_t ino);
bool xfs_internal_inum(struct xfs_mount *mp, xfs_ino_t ino);
bool xfs_verify_dir_ino(struct xfs_mount *mp, xfs_ino_t ino);
bool xfs_verify_rtbno(struct xfs_mount *mp, xfs_rtblock_t rtbno);
bool xfs_verify_rtext(struct xfs_mount *mp, xfs_rtblock_t rtbno,
		xfs_rtblock_t len);
bool xfs_verify_icount(struct xfs_mount *mp, unsigned long long icount);
bool xfs_verify_dablk(struct xfs_mount *mp, xfs_fileoff_t off);
void xfs_icount_range(struct xfs_mount *mp, unsigned long long *min,
		unsigned long long *max);
bool xfs_verify_fileoff(struct xfs_mount *mp, xfs_fileoff_t off);
bool xfs_verify_fileext(struct xfs_mount *mp, xfs_fileoff_t off,
		xfs_fileoff_t len);

#endif	/* __XFS_TYPES_H__ */
