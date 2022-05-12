/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * ocfs2_ioctl.h
 *
 * Defines OCFS2 ioctls.
 *
 * Copyright (C) 2010 Oracle.  All rights reserved.
 */

#ifndef OCFS2_IOCTL_H
#define OCFS2_IOCTL_H

/*
 * Space reservation / allocation / free ioctls and argument structure
 * are designed to be compatible with XFS.
 *
 * ALLOCSP* and FREESP* are not and will never be supported, but are
 * included here for completeness.
 */
struct ocfs2_space_resv {
	__s16		l_type;
	__s16		l_whence;
	__s64		l_start;
	__s64		l_len;		/* len == 0 means until end of file */
	__s32		l_sysid;
	__u32		l_pid;
	__s32		l_pad[4];	/* reserve area			    */
};

#define OCFS2_IOC_ALLOCSP		_IOW ('X', 10, struct ocfs2_space_resv)
#define OCFS2_IOC_FREESP		_IOW ('X', 11, struct ocfs2_space_resv)
#define OCFS2_IOC_RESVSP		_IOW ('X', 40, struct ocfs2_space_resv)
#define OCFS2_IOC_UNRESVSP	_IOW ('X', 41, struct ocfs2_space_resv)
#define OCFS2_IOC_ALLOCSP64	_IOW ('X', 36, struct ocfs2_space_resv)
#define OCFS2_IOC_FREESP64	_IOW ('X', 37, struct ocfs2_space_resv)
#define OCFS2_IOC_RESVSP64	_IOW ('X', 42, struct ocfs2_space_resv)
#define OCFS2_IOC_UNRESVSP64	_IOW ('X', 43, struct ocfs2_space_resv)

/* Used to pass group descriptor data when online resize is done */
struct ocfs2_new_group_input {
	__u64 group;		/* Group descriptor's blkno. */
	__u32 clusters;		/* Total number of clusters in this group */
	__u32 frees;		/* Total free clusters in this group */
	__u16 chain;		/* Chain for this group */
	__u16 reserved1;
	__u32 reserved2;
};

#define OCFS2_IOC_GROUP_EXTEND	_IOW('o', 1, int)
#define OCFS2_IOC_GROUP_ADD	_IOW('o', 2,struct ocfs2_new_group_input)
#define OCFS2_IOC_GROUP_ADD64	_IOW('o', 3,struct ocfs2_new_group_input)

/* Used to pass 2 file names to reflink. */
struct reflink_arguments {
	__u64 old_path;
	__u64 new_path;
	__u64 preserve;
};
#define OCFS2_IOC_REFLINK	_IOW('o', 4, struct reflink_arguments)

/* Following definitions dedicated for ocfs2_info_request ioctls. */
#define OCFS2_INFO_MAX_REQUEST		(50)
#define OCFS2_TEXT_UUID_LEN		(OCFS2_VOL_UUID_LEN * 2)

/* Magic number of all requests */
#define OCFS2_INFO_MAGIC		(0x4F32494E)

/*
 * Always try to separate info request into small pieces to
 * guarantee the backward&forward compatibility.
 */
struct ocfs2_info {
	__u64 oi_requests;	/* Array of __u64 pointers to requests */
	__u32 oi_count;		/* Number of requests in info_requests */
	__u32 oi_pad;
};

struct ocfs2_info_request {
/*00*/	__u32 ir_magic;	/* Magic number */
	__u32 ir_code;	/* Info request code */
	__u32 ir_size;	/* Size of request */
	__u32 ir_flags;	/* Request flags */
/*10*/			/* Request specific fields */
};

struct ocfs2_info_clustersize {
	struct ocfs2_info_request ic_req;
	__u32 ic_clustersize;
	__u32 ic_pad;
};

struct ocfs2_info_blocksize {
	struct ocfs2_info_request ib_req;
	__u32 ib_blocksize;
	__u32 ib_pad;
};

struct ocfs2_info_maxslots {
	struct ocfs2_info_request im_req;
	__u32 im_max_slots;
	__u32 im_pad;
};

struct ocfs2_info_label {
	struct ocfs2_info_request il_req;
	__u8	il_label[OCFS2_MAX_VOL_LABEL_LEN];
} __attribute__ ((packed));

struct ocfs2_info_uuid {
	struct ocfs2_info_request iu_req;
	__u8	iu_uuid_str[OCFS2_TEXT_UUID_LEN + 1];
} __attribute__ ((packed));

struct ocfs2_info_fs_features {
	struct ocfs2_info_request if_req;
	__u32 if_compat_features;
	__u32 if_incompat_features;
	__u32 if_ro_compat_features;
	__u32 if_pad;
};

struct ocfs2_info_journal_size {
	struct ocfs2_info_request ij_req;
	__u64 ij_journal_size;
};

struct ocfs2_info_freeinode {
	struct ocfs2_info_request ifi_req;
	struct ocfs2_info_local_freeinode {
		__u64 lfi_total;
		__u64 lfi_free;
	} ifi_stat[OCFS2_MAX_SLOTS];
	__u32 ifi_slotnum; /* out */
	__u32 ifi_pad;
};

#define OCFS2_INFO_MAX_HIST     (32)

struct ocfs2_info_freefrag {
	struct ocfs2_info_request iff_req;
	struct ocfs2_info_freefrag_stats { /* (out) */
		struct ocfs2_info_free_chunk_list {
			__u32 fc_chunks[OCFS2_INFO_MAX_HIST];
			__u32 fc_clusters[OCFS2_INFO_MAX_HIST];
		} ffs_fc_hist;
		__u32 ffs_clusters;
		__u32 ffs_free_clusters;
		__u32 ffs_free_chunks;
		__u32 ffs_free_chunks_real;
		__u32 ffs_min; /* Minimum free chunksize in clusters */
		__u32 ffs_max;
		__u32 ffs_avg;
		__u32 ffs_pad;
	} iff_ffs;
	__u32 iff_chunksize; /* chunksize in clusters(in) */
	__u32 iff_pad;
};

/* Codes for ocfs2_info_request */
enum ocfs2_info_type {
	OCFS2_INFO_CLUSTERSIZE = 1,
	OCFS2_INFO_BLOCKSIZE,
	OCFS2_INFO_MAXSLOTS,
	OCFS2_INFO_LABEL,
	OCFS2_INFO_UUID,
	OCFS2_INFO_FS_FEATURES,
	OCFS2_INFO_JOURNAL_SIZE,
	OCFS2_INFO_FREEINODE,
	OCFS2_INFO_FREEFRAG,
	OCFS2_INFO_NUM_TYPES
};

/* Flags for struct ocfs2_info_request */
/* Filled by the caller */
#define OCFS2_INFO_FL_NON_COHERENT	(0x00000001)	/* Cluster coherency not
							   required. This is a hint.
							   It is up to ocfs2 whether
							   the request can be fulfilled
							   without locking. */
/* Filled by ocfs2 */
#define OCFS2_INFO_FL_FILLED		(0x40000000)	/* Filesystem understood
							   this request and
							   filled in the answer */

#define OCFS2_INFO_FL_ERROR		(0x80000000)	/* Error happened during
							   request handling. */

#define OCFS2_IOC_INFO		_IOR('o', 5, struct ocfs2_info)

struct ocfs2_move_extents {
/* All values are in bytes */
	/* in */
	__u64 me_start;		/* Virtual start in the file to move */
	__u64 me_len;		/* Length of the extents to be moved */
	__u64 me_goal;		/* Physical offset of the goal,
				   it's in block unit */
	__u64 me_threshold;	/* Maximum distance from goal or threshold
				   for auto defragmentation */
	__u64 me_flags;		/* Flags for the operation:
				 * - auto defragmentation.
				 * - refcount,xattr cases.
				 */
	/* out */
	__u64 me_moved_len;	/* Moved/defraged length */
	__u64 me_new_offset;	/* Resulting physical location */
	__u32 me_reserved[2];	/* Reserved for futhure */
};

#define OCFS2_MOVE_EXT_FL_AUTO_DEFRAG	(0x00000001)	/* Kernel manages to
							   claim new clusters
							   as the goal place
							   for extents moving */
#define OCFS2_MOVE_EXT_FL_PART_DEFRAG	(0x00000002)	/* Allow partial extent
							   moving, is to make
							   movement less likely
							   to fail, may make fs
							   even more fragmented */
#define OCFS2_MOVE_EXT_FL_COMPLETE	(0x00000004)	/* Move or defragmenation
							   completely gets done.
							 */

#define OCFS2_IOC_MOVE_EXT	_IOW('o', 6, struct ocfs2_move_extents)

#endif /* OCFS2_IOCTL_H */
