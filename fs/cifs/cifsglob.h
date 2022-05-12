/*
 *   fs/cifs/cifsglob.h
 *
 *   Copyright (C) International Business Machines  Corp., 2002,2008
 *   Author(s): Steve French (sfrench@us.ibm.com)
 *              Jeremy Allison (jra@samba.org)
 *
 *   This library is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published
 *   by the Free Software Foundation; either version 2.1 of the License, or
 *   (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 *   the GNU Lesser General Public License for more details.
 *
 */
#ifndef _CIFS_GLOB_H
#define _CIFS_GLOB_H

#include <linux/in.h>
#include <linux/in6.h>
#include <linux/inet.h>
#include <linux/slab.h>
#include <linux/mempool.h>
#include <linux/workqueue.h>
#include "cifs_fs_sb.h"
#include "cifsacl.h"
#include <crypto/internal/hash.h>
#include <linux/scatterlist.h>
#include <uapi/linux/cifs/cifs_mount.h>
#include "smb2pdu.h"

#define CIFS_MAGIC_NUMBER 0xFF534D42      /* the first four bytes of SMB PDUs */

#define SMB_PATH_MAX 260
#define CIFS_PORT 445
#define RFC1001_PORT 139

/*
 * The sizes of various internal tables and strings
 */
#define MAX_UID_INFO 16
#define MAX_SES_INFO 2
#define MAX_TCON_INFO 4

#define MAX_TREE_SIZE (2 + CIFS_NI_MAXHOST + 1 + CIFS_MAX_SHARE_LEN + 1)

#define CIFS_MIN_RCV_POOL 4

#define MAX_REOPEN_ATT	5 /* these many maximum attempts to reopen a file */
/*
 * default attribute cache timeout (jiffies)
 */
#define CIFS_DEF_ACTIMEO (1 * HZ)

/*
 * max attribute cache timeout (jiffies) - 2^30
 */
#define CIFS_MAX_ACTIMEO (1 << 30)

/*
 * Max persistent and resilient handle timeout (milliseconds).
 * Windows durable max was 960000 (16 minutes)
 */
#define SMB3_MAX_HANDLE_TIMEOUT 960000

/*
 * MAX_REQ is the maximum number of requests that WE will send
 * on one socket concurrently.
 */
#define CIFS_MAX_REQ 32767

#define RFC1001_NAME_LEN 15
#define RFC1001_NAME_LEN_WITH_NULL (RFC1001_NAME_LEN + 1)

/* maximum length of ip addr as a string (including ipv6 and sctp) */
#define SERVER_NAME_LENGTH 80
#define SERVER_NAME_LEN_WITH_NULL     (SERVER_NAME_LENGTH + 1)

/* echo interval in seconds */
#define SMB_ECHO_INTERVAL_MIN 1
#define SMB_ECHO_INTERVAL_MAX 600
#define SMB_ECHO_INTERVAL_DEFAULT 60

/* maximum number of PDUs in one compound */
#define MAX_COMPOUND 5

/*
 * Default number of credits to keep available for SMB3.
 * This value is chosen somewhat arbitrarily. The Windows client
 * defaults to 128 credits, the Windows server allows clients up to
 * 512 credits (or 8K for later versions), and the NetApp server
 * does not limit clients at all.  Choose a high enough default value
 * such that the client shouldn't limit performance, but allow mount
 * to override (until you approach 64K, where we limit credits to 65000
 * to reduce possibility of seeing more server credit overflow bugs.
 */
#define SMB2_MAX_CREDITS_AVAILABLE 32000

#include "cifspdu.h"

#ifndef XATTR_DOS_ATTRIB
#define XATTR_DOS_ATTRIB "user.DOSATTRIB"
#endif

/*
 * CIFS vfs client Status information (based on what we know.)
 */

/* associated with each tcp and smb session */
enum statusEnum {
	CifsNew = 0,
	CifsGood,
	CifsExiting,
	CifsNeedReconnect,
	CifsNeedNegotiate
};

enum securityEnum {
	Unspecified = 0,	/* not specified */
	LANMAN,			/* Legacy LANMAN auth */
	NTLM,			/* Legacy NTLM012 auth with NTLM hash */
	NTLMv2,			/* Legacy NTLM auth with NTLMv2 hash */
	RawNTLMSSP,		/* NTLMSSP without SPNEGO, NTLMv2 hash */
	Kerberos,		/* Kerberos via SPNEGO */
};

struct session_key {
	unsigned int len;
	char *response;
};

/* crypto security descriptor definition */
struct sdesc {
	struct shash_desc shash;
	char ctx[];
};

/* crypto hashing related structure/fields, not specific to a sec mech */
struct cifs_secmech {
	struct crypto_shash *hmacmd5; /* hmac-md5 hash function */
	struct crypto_shash *md5; /* md5 hash function */
	struct crypto_shash *hmacsha256; /* hmac-sha256 hash function */
	struct crypto_shash *cmacaes; /* block-cipher based MAC function */
	struct crypto_shash *sha512; /* sha512 hash function */
	struct sdesc *sdeschmacmd5;  /* ctxt to generate ntlmv2 hash, CR1 */
	struct sdesc *sdescmd5; /* ctxt to generate cifs/smb signature */
	struct sdesc *sdeschmacsha256;  /* ctxt to generate smb2 signature */
	struct sdesc *sdesccmacaes;  /* ctxt to generate smb3 signature */
	struct sdesc *sdescsha512; /* ctxt to generate smb3.11 signing key */
	struct crypto_aead *ccmaesencrypt; /* smb3 encryption aead */
	struct crypto_aead *ccmaesdecrypt; /* smb3 decryption aead */
};

/* per smb session structure/fields */
struct ntlmssp_auth {
	bool sesskey_per_smbsess; /* whether session key is per smb session */
	__u32 client_flags; /* sent by client in type 1 ntlmsssp exchange */
	__u32 server_flags; /* sent by server in type 2 ntlmssp exchange */
	unsigned char ciphertext[CIFS_CPHTXT_SIZE]; /* sent to server */
	char cryptkey[CIFS_CRYPTO_KEY_SIZE]; /* used by ntlmssp */
};

struct cifs_cred {
	int uid;
	int gid;
	int mode;
	int cecount;
	struct cifs_sid osid;
	struct cifs_sid gsid;
	struct cifs_ntace *ntaces;
	struct cifs_ace *aces;
};

/*
 *****************************************************************
 * Except the CIFS PDUs themselves all the
 * globally interesting structs should go here
 *****************************************************************
 */

/*
 * A smb_rqst represents a complete request to be issued to a server. It's
 * formed by a kvec array, followed by an array of pages. Page data is assumed
 * to start at the beginning of the first page.
 */
struct smb_rqst {
	struct kvec	*rq_iov;	/* array of kvecs */
	unsigned int	rq_nvec;	/* number of kvecs in array */
	struct page	**rq_pages;	/* pointer to array of page ptrs */
	unsigned int	rq_offset;	/* the offset to the 1st page */
	unsigned int	rq_npages;	/* number pages in array */
	unsigned int	rq_pagesz;	/* page size to use */
	unsigned int	rq_tailsz;	/* length of last page */
};

struct mid_q_entry;
struct TCP_Server_Info;
struct cifsFileInfo;
struct cifs_ses;
struct cifs_tcon;
struct dfs_info3_param;
struct cifs_fattr;
struct smb3_fs_context;
struct cifs_fid;
struct cifs_readdata;
struct cifs_writedata;
struct cifs_io_parms;
struct cifs_search_info;
struct cifsInodeInfo;
struct cifs_open_parms;
struct cifs_credits;

struct smb_version_operations {
	int (*send_cancel)(struct TCP_Server_Info *, struct smb_rqst *,
			   struct mid_q_entry *);
	bool (*compare_fids)(struct cifsFileInfo *, struct cifsFileInfo *);
	/* setup request: allocate mid, sign message */
	struct mid_q_entry *(*setup_request)(struct cifs_ses *,
					     struct TCP_Server_Info *,
					     struct smb_rqst *);
	/* setup async request: allocate mid, sign message */
	struct mid_q_entry *(*setup_async_request)(struct TCP_Server_Info *,
						struct smb_rqst *);
	/* check response: verify signature, map error */
	int (*check_receive)(struct mid_q_entry *, struct TCP_Server_Info *,
			     bool);
	void (*add_credits)(struct TCP_Server_Info *server,
			    const struct cifs_credits *credits,
			    const int optype);
	void (*set_credits)(struct TCP_Server_Info *, const int);
	int * (*get_credits_field)(struct TCP_Server_Info *, const int);
	unsigned int (*get_credits)(struct mid_q_entry *);
	__u64 (*get_next_mid)(struct TCP_Server_Info *);
	void (*revert_current_mid)(struct TCP_Server_Info *server,
				   const unsigned int val);
	/* data offset from read response message */
	unsigned int (*read_data_offset)(char *);
	/*
	 * Data length from read response message
	 * When in_remaining is true, the returned data length is in
	 * message field DataRemaining for out-of-band data read (e.g through
	 * Memory Registration RDMA write in SMBD).
	 * Otherwise, the returned data length is in message field DataLength.
	 */
	unsigned int (*read_data_length)(char *, bool in_remaining);
	/* map smb to linux error */
	int (*map_error)(char *, bool);
	/* find mid corresponding to the response message */
	struct mid_q_entry * (*find_mid)(struct TCP_Server_Info *, char *);
	void (*dump_detail)(void *buf, struct TCP_Server_Info *ptcp_info);
	void (*clear_stats)(struct cifs_tcon *);
	void (*print_stats)(struct seq_file *m, struct cifs_tcon *);
	void (*dump_share_caps)(struct seq_file *, struct cifs_tcon *);
	/* verify the message */
	int (*check_message)(char *, unsigned int, struct TCP_Server_Info *);
	bool (*is_oplock_break)(char *, struct TCP_Server_Info *);
	int (*handle_cancelled_mid)(struct mid_q_entry *, struct TCP_Server_Info *);
	void (*downgrade_oplock)(struct TCP_Server_Info *server,
				 struct cifsInodeInfo *cinode, __u32 oplock,
				 unsigned int epoch, bool *purge_cache);
	/* process transaction2 response */
	bool (*check_trans2)(struct mid_q_entry *, struct TCP_Server_Info *,
			     char *, int);
	/* check if we need to negotiate */
	bool (*need_neg)(struct TCP_Server_Info *);
	/* negotiate to the server */
	int (*negotiate)(const unsigned int, struct cifs_ses *);
	/* set negotiated write size */
	unsigned int (*negotiate_wsize)(struct cifs_tcon *tcon, struct smb3_fs_context *ctx);
	/* set negotiated read size */
	unsigned int (*negotiate_rsize)(struct cifs_tcon *tcon, struct smb3_fs_context *ctx);
	/* setup smb sessionn */
	int (*sess_setup)(const unsigned int, struct cifs_ses *,
			  const struct nls_table *);
	/* close smb session */
	int (*logoff)(const unsigned int, struct cifs_ses *);
	/* connect to a server share */
	int (*tree_connect)(const unsigned int, struct cifs_ses *, const char *,
			    struct cifs_tcon *, const struct nls_table *);
	/* close tree connecion */
	int (*tree_disconnect)(const unsigned int, struct cifs_tcon *);
	/* get DFS referrals */
	int (*get_dfs_refer)(const unsigned int, struct cifs_ses *,
			     const char *, struct dfs_info3_param **,
			     unsigned int *, const struct nls_table *, int);
	/* informational QFS call */
	void (*qfs_tcon)(const unsigned int, struct cifs_tcon *,
			 struct cifs_sb_info *);
	/* check if a path is accessible or not */
	int (*is_path_accessible)(const unsigned int, struct cifs_tcon *,
				  struct cifs_sb_info *, const char *);
	/* query path data from the server */
	int (*query_path_info)(const unsigned int, struct cifs_tcon *,
			       struct cifs_sb_info *, const char *,
			       FILE_ALL_INFO *, bool *, bool *);
	/* query file data from the server */
	int (*query_file_info)(const unsigned int, struct cifs_tcon *,
			       struct cifs_fid *, FILE_ALL_INFO *);
	/* query reparse tag from srv to determine which type of special file */
	int (*query_reparse_tag)(const unsigned int xid, struct cifs_tcon *tcon,
				struct cifs_sb_info *cifs_sb, const char *path,
				__u32 *reparse_tag);
	/* get server index number */
	int (*get_srv_inum)(const unsigned int, struct cifs_tcon *,
			    struct cifs_sb_info *, const char *,
			    u64 *uniqueid, FILE_ALL_INFO *);
	/* set size by path */
	int (*set_path_size)(const unsigned int, struct cifs_tcon *,
			     const char *, __u64, struct cifs_sb_info *, bool);
	/* set size by file handle */
	int (*set_file_size)(const unsigned int, struct cifs_tcon *,
			     struct cifsFileInfo *, __u64, bool);
	/* set attributes */
	int (*set_file_info)(struct inode *, const char *, FILE_BASIC_INFO *,
			     const unsigned int);
	int (*set_compression)(const unsigned int, struct cifs_tcon *,
			       struct cifsFileInfo *);
	/* check if we can send an echo or nor */
	bool (*can_echo)(struct TCP_Server_Info *);
	/* send echo request */
	int (*echo)(struct TCP_Server_Info *);
	/* create directory */
	int (*posix_mkdir)(const unsigned int xid, struct inode *inode,
			umode_t mode, struct cifs_tcon *tcon,
			const char *full_path,
			struct cifs_sb_info *cifs_sb);
	int (*mkdir)(const unsigned int xid, struct inode *inode, umode_t mode,
		     struct cifs_tcon *tcon, const char *name,
		     struct cifs_sb_info *sb);
	/* set info on created directory */
	void (*mkdir_setinfo)(struct inode *, const char *,
			      struct cifs_sb_info *, struct cifs_tcon *,
			      const unsigned int);
	/* remove directory */
	int (*rmdir)(const unsigned int, struct cifs_tcon *, const char *,
		     struct cifs_sb_info *);
	/* unlink file */
	int (*unlink)(const unsigned int, struct cifs_tcon *, const char *,
		      struct cifs_sb_info *);
	/* open, rename and delete file */
	int (*rename_pending_delete)(const char *, struct dentry *,
				     const unsigned int);
	/* send rename request */
	int (*rename)(const unsigned int, struct cifs_tcon *, const char *,
		      const char *, struct cifs_sb_info *);
	/* send create hardlink request */
	int (*create_hardlink)(const unsigned int, struct cifs_tcon *,
			       const char *, const char *,
			       struct cifs_sb_info *);
	/* query symlink target */
	int (*query_symlink)(const unsigned int, struct cifs_tcon *,
			     struct cifs_sb_info *, const char *,
			     char **, bool);
	/* open a file for non-posix mounts */
	int (*open)(const unsigned int, struct cifs_open_parms *,
		    __u32 *, FILE_ALL_INFO *);
	/* set fid protocol-specific info */
	void (*set_fid)(struct cifsFileInfo *, struct cifs_fid *, __u32);
	/* close a file */
	void (*close)(const unsigned int, struct cifs_tcon *,
		      struct cifs_fid *);
	/* close a file, returning file attributes and timestamps */
	void (*close_getattr)(const unsigned int xid, struct cifs_tcon *tcon,
		      struct cifsFileInfo *pfile_info);
	/* send a flush request to the server */
	int (*flush)(const unsigned int, struct cifs_tcon *, struct cifs_fid *);
	/* async read from the server */
	int (*async_readv)(struct cifs_readdata *);
	/* async write to the server */
	int (*async_writev)(struct cifs_writedata *,
			    void (*release)(struct kref *));
	/* sync read from the server */
	int (*sync_read)(const unsigned int, struct cifs_fid *,
			 struct cifs_io_parms *, unsigned int *, char **,
			 int *);
	/* sync write to the server */
	int (*sync_write)(const unsigned int, struct cifs_fid *,
			  struct cifs_io_parms *, unsigned int *, struct kvec *,
			  unsigned long);
	/* open dir, start readdir */
	int (*query_dir_first)(const unsigned int, struct cifs_tcon *,
			       const char *, struct cifs_sb_info *,
			       struct cifs_fid *, __u16,
			       struct cifs_search_info *);
	/* continue readdir */
	int (*query_dir_next)(const unsigned int, struct cifs_tcon *,
			      struct cifs_fid *,
			      __u16, struct cifs_search_info *srch_inf);
	/* close dir */
	int (*close_dir)(const unsigned int, struct cifs_tcon *,
			 struct cifs_fid *);
	/* calculate a size of SMB message */
	unsigned int (*calc_smb_size)(void *buf, struct TCP_Server_Info *ptcpi);
	/* check for STATUS_PENDING and process the response if yes */
	bool (*is_status_pending)(char *buf, struct TCP_Server_Info *server);
	/* check for STATUS_NETWORK_SESSION_EXPIRED */
	bool (*is_session_expired)(char *);
	/* send oplock break response */
	int (*oplock_response)(struct cifs_tcon *, struct cifs_fid *,
			       struct cifsInodeInfo *);
	/* query remote filesystem */
	int (*queryfs)(const unsigned int, struct cifs_tcon *,
		       struct cifs_sb_info *, struct kstatfs *);
	/* send mandatory brlock to the server */
	int (*mand_lock)(const unsigned int, struct cifsFileInfo *, __u64,
			 __u64, __u32, int, int, bool);
	/* unlock range of mandatory locks */
	int (*mand_unlock_range)(struct cifsFileInfo *, struct file_lock *,
				 const unsigned int);
	/* push brlocks from the cache to the server */
	int (*push_mand_locks)(struct cifsFileInfo *);
	/* get lease key of the inode */
	void (*get_lease_key)(struct inode *, struct cifs_fid *);
	/* set lease key of the inode */
	void (*set_lease_key)(struct inode *, struct cifs_fid *);
	/* generate new lease key */
	void (*new_lease_key)(struct cifs_fid *);
	int (*generate_signingkey)(struct cifs_ses *);
	int (*calc_signature)(struct smb_rqst *, struct TCP_Server_Info *,
				bool allocate_crypto);
	int (*set_integrity)(const unsigned int, struct cifs_tcon *tcon,
			     struct cifsFileInfo *src_file);
	int (*enum_snapshots)(const unsigned int xid, struct cifs_tcon *tcon,
			     struct cifsFileInfo *src_file, void __user *);
	int (*notify)(const unsigned int xid, struct file *pfile,
			     void __user *pbuf);
	int (*query_mf_symlink)(unsigned int, struct cifs_tcon *,
				struct cifs_sb_info *, const unsigned char *,
				char *, unsigned int *);
	int (*create_mf_symlink)(unsigned int, struct cifs_tcon *,
				 struct cifs_sb_info *, const unsigned char *,
				 char *, unsigned int *);
	/* if we can do cache read operations */
	bool (*is_read_op)(__u32);
	/* set oplock level for the inode */
	void (*set_oplock_level)(struct cifsInodeInfo *, __u32, unsigned int,
				 bool *);
	/* create lease context buffer for CREATE request */
	char * (*create_lease_buf)(u8 *lease_key, u8 oplock);
	/* parse lease context buffer and return oplock/epoch info */
	__u8 (*parse_lease_buf)(void *buf, unsigned int *epoch, char *lkey);
	ssize_t (*copychunk_range)(const unsigned int,
			struct cifsFileInfo *src_file,
			struct cifsFileInfo *target_file,
			u64 src_off, u64 len, u64 dest_off);
	int (*duplicate_extents)(const unsigned int, struct cifsFileInfo *src,
			struct cifsFileInfo *target_file, u64 src_off, u64 len,
			u64 dest_off);
	int (*validate_negotiate)(const unsigned int, struct cifs_tcon *);
	ssize_t (*query_all_EAs)(const unsigned int, struct cifs_tcon *,
			const unsigned char *, const unsigned char *, char *,
			size_t, struct cifs_sb_info *);
	int (*set_EA)(const unsigned int, struct cifs_tcon *, const char *,
			const char *, const void *, const __u16,
			const struct nls_table *, struct cifs_sb_info *);
	struct cifs_ntsd * (*get_acl)(struct cifs_sb_info *, struct inode *,
			const char *, u32 *, u32);
	struct cifs_ntsd * (*get_acl_by_fid)(struct cifs_sb_info *,
			const struct cifs_fid *, u32 *, u32);
	int (*set_acl)(struct cifs_ntsd *, __u32, struct inode *, const char *,
			int);
	/* writepages retry size */
	unsigned int (*wp_retry_size)(struct inode *);
	/* get mtu credits */
	int (*wait_mtu_credits)(struct TCP_Server_Info *, unsigned int,
				unsigned int *, struct cifs_credits *);
	/* adjust previously taken mtu credits to request size */
	int (*adjust_credits)(struct TCP_Server_Info *server,
			      struct cifs_credits *credits,
			      const unsigned int payload_size);
	/* check if we need to issue closedir */
	bool (*dir_needs_close)(struct cifsFileInfo *);
	long (*fallocate)(struct file *, struct cifs_tcon *, int, loff_t,
			  loff_t);
	/* init transform request - used for encryption for now */
	int (*init_transform_rq)(struct TCP_Server_Info *, int num_rqst,
				 struct smb_rqst *, struct smb_rqst *);
	int (*is_transform_hdr)(void *buf);
	int (*receive_transform)(struct TCP_Server_Info *,
				 struct mid_q_entry **, char **, int *);
	enum securityEnum (*select_sectype)(struct TCP_Server_Info *,
			    enum securityEnum);
	int (*next_header)(char *);
	/* ioctl passthrough for query_info */
	int (*ioctl_query_info)(const unsigned int xid,
				struct cifs_tcon *tcon,
				struct cifs_sb_info *cifs_sb,
				__le16 *path, int is_dir,
				unsigned long p);
	/* make unix special files (block, char, fifo, socket) */
	int (*make_node)(unsigned int xid,
			 struct inode *inode,
			 struct dentry *dentry,
			 struct cifs_tcon *tcon,
			 const char *full_path,
			 umode_t mode,
			 dev_t device_number);
	/* version specific fiemap implementation */
	int (*fiemap)(struct cifs_tcon *tcon, struct cifsFileInfo *,
		      struct fiemap_extent_info *, u64, u64);
	/* version specific llseek implementation */
	loff_t (*llseek)(struct file *, struct cifs_tcon *, loff_t, int);
	/* Check for STATUS_IO_TIMEOUT */
	bool (*is_status_io_timeout)(char *buf);
	/* Check for STATUS_NETWORK_NAME_DELETED */
	void (*is_network_name_deleted)(char *buf, struct TCP_Server_Info *srv);
};

struct smb_version_values {
	char		*version_string;
	__u16		protocol_id;
	__u32		req_capabilities;
	__u32		large_lock_type;
	__u32		exclusive_lock_type;
	__u32		shared_lock_type;
	__u32		unlock_lock_type;
	size_t		header_preamble_size;
	size_t		header_size;
	size_t		max_header_size;
	size_t		read_rsp_size;
	__le16		lock_cmd;
	unsigned int	cap_unix;
	unsigned int	cap_nt_find;
	unsigned int	cap_large_files;
	__u16		signing_enabled;
	__u16		signing_required;
	size_t		create_lease_size;
};

#define HEADER_SIZE(server) (server->vals->header_size)
#define MAX_HEADER_SIZE(server) (server->vals->max_header_size)

/**
 * CIFS superblock mount flags (mnt_cifs_flags) to consider when
 * trying to reuse existing superblock for a new mount
 */
#define CIFS_MOUNT_MASK (CIFS_MOUNT_NO_PERM | CIFS_MOUNT_SET_UID | \
			 CIFS_MOUNT_SERVER_INUM | CIFS_MOUNT_DIRECT_IO | \
			 CIFS_MOUNT_NO_XATTR | CIFS_MOUNT_MAP_SPECIAL_CHR | \
			 CIFS_MOUNT_MAP_SFM_CHR | \
			 CIFS_MOUNT_UNX_EMUL | CIFS_MOUNT_NO_BRL | \
			 CIFS_MOUNT_CIFS_ACL | CIFS_MOUNT_OVERR_UID | \
			 CIFS_MOUNT_OVERR_GID | CIFS_MOUNT_DYNPERM | \
			 CIFS_MOUNT_NOPOSIXBRL | CIFS_MOUNT_NOSSYNC | \
			 CIFS_MOUNT_FSCACHE | CIFS_MOUNT_MF_SYMLINKS | \
			 CIFS_MOUNT_MULTIUSER | CIFS_MOUNT_STRICT_IO | \
			 CIFS_MOUNT_CIFS_BACKUPUID | CIFS_MOUNT_CIFS_BACKUPGID | \
			 CIFS_MOUNT_UID_FROM_ACL | CIFS_MOUNT_NO_HANDLE_CACHE | \
			 CIFS_MOUNT_NO_DFS | CIFS_MOUNT_MODE_FROM_SID | \
			 CIFS_MOUNT_RO_CACHE | CIFS_MOUNT_RW_CACHE)

/**
 * Generic VFS superblock mount flags (s_flags) to consider when
 * trying to reuse existing superblock for a new mount
 */
#define CIFS_MS_MASK (SB_RDONLY | SB_MANDLOCK | SB_NOEXEC | SB_NOSUID | \
		      SB_NODEV | SB_SYNCHRONOUS)

struct cifs_mnt_data {
	struct cifs_sb_info *cifs_sb;
	struct smb3_fs_context *ctx;
	int flags;
};

static inline unsigned int
get_rfc1002_length(void *buf)
{
	return be32_to_cpu(*((__be32 *)buf)) & 0xffffff;
}

static inline void
inc_rfc1001_len(void *buf, int count)
{
	be32_add_cpu((__be32 *)buf, count);
}

struct TCP_Server_Info {
	struct list_head tcp_ses_list;
	struct list_head smb_ses_list;
	__u64 conn_id; /* connection identifier (useful for debugging) */
	int srv_count; /* reference counter */
	/* 15 character server name + 0x20 16th byte indicating type = srv */
	char server_RFC1001_name[RFC1001_NAME_LEN_WITH_NULL];
	struct smb_version_operations	*ops;
	struct smb_version_values	*vals;
	enum statusEnum tcpStatus; /* what we think the status is */
	char *hostname; /* hostname portion of UNC string */
	struct socket *ssocket;
	struct sockaddr_storage dstaddr;
	struct sockaddr_storage srcaddr; /* locally bind to this IP */
#ifdef CONFIG_NET_NS
	struct net *net;
#endif
	wait_queue_head_t response_q;
	wait_queue_head_t request_q; /* if more than maxmpx to srvr must block*/
	struct list_head pending_mid_q;
	bool noblocksnd;		/* use blocking sendmsg */
	bool noautotune;		/* do not autotune send buf sizes */
	bool tcp_nodelay;
	unsigned int credits;  /* send no more requests at once */
	unsigned int max_credits; /* can override large 32000 default at mnt */
	unsigned int in_flight;  /* number of requests on the wire to server */
	unsigned int max_in_flight; /* max number of requests that were on wire */
	spinlock_t req_lock;  /* protect the two values above */
	struct mutex srv_mutex;
	struct task_struct *tsk;
	char server_GUID[16];
	__u16 sec_mode;
	bool sign; /* is signing enabled on this connection? */
	bool ignore_signature:1; /* skip validation of signatures in SMB2/3 rsp */
	bool session_estab; /* mark when very first sess is established */
	int echo_credits;  /* echo reserved slots */
	int oplock_credits;  /* oplock break reserved slots */
	bool echoes:1; /* enable echoes */
	__u8 client_guid[SMB2_CLIENT_GUID_SIZE]; /* Client GUID */
	u16 dialect; /* dialect index that server chose */
	bool oplocks:1; /* enable oplocks */
	unsigned int maxReq;	/* Clients should submit no more */
	/* than maxReq distinct unanswered SMBs to the server when using  */
	/* multiplexed reads or writes (for SMB1/CIFS only, not SMB2/SMB3) */
	unsigned int maxBuf;	/* maxBuf specifies the maximum */
	/* message size the server can send or receive for non-raw SMBs */
	/* maxBuf is returned by SMB NegotiateProtocol so maxBuf is only 0 */
	/* when socket is setup (and during reconnect) before NegProt sent */
	unsigned int max_rw;	/* maxRw specifies the maximum */
	/* message size the server can send or receive for */
	/* SMB_COM_WRITE_RAW or SMB_COM_READ_RAW. */
	unsigned int capabilities; /* selective disabling of caps by smb sess */
	int timeAdj;  /* Adjust for difference in server time zone in sec */
	__u64 CurrentMid;         /* multiplex id - rotating counter */
	char cryptkey[CIFS_CRYPTO_KEY_SIZE]; /* used by ntlm, ntlmv2 etc */
	/* 16th byte of RFC1001 workstation name is always null */
	char workstation_RFC1001_name[RFC1001_NAME_LEN_WITH_NULL];
	__u32 sequence_number; /* for signing, protected by srv_mutex */
	__u32 reconnect_instance; /* incremented on each reconnect */
	struct session_key session_key;
	unsigned long lstrp; /* when we got last response from this server */
	struct cifs_secmech secmech; /* crypto sec mech functs, descriptors */
#define	CIFS_NEGFLAVOR_LANMAN	0	/* wct == 13, LANMAN */
#define	CIFS_NEGFLAVOR_UNENCAP	1	/* wct == 17, but no ext_sec */
#define	CIFS_NEGFLAVOR_EXTENDED	2	/* wct == 17, ext_sec bit set */
	char	negflavor;	/* NEGOTIATE response flavor */
	/* extended security flavors that server supports */
	bool	sec_ntlmssp;		/* supports NTLMSSP */
	bool	sec_kerberosu2u;	/* supports U2U Kerberos */
	bool	sec_kerberos;		/* supports plain Kerberos */
	bool	sec_mskerberos;		/* supports legacy MS Kerberos */
	bool	large_buf;		/* is current buffer large? */
	/* use SMBD connection instead of socket */
	bool	rdma;
	/* point to the SMBD connection if RDMA is used instead of socket */
	struct smbd_connection *smbd_conn;
	struct delayed_work	echo; /* echo ping workqueue job */
	char	*smallbuf;	/* pointer to current "small" buffer */
	char	*bigbuf;	/* pointer to current "big" buffer */
	/* Total size of this PDU. Only valid from cifs_demultiplex_thread */
	unsigned int pdu_size;
	unsigned int total_read; /* total amount of data read in this pass */
	atomic_t in_send; /* requests trying to send */
	atomic_t num_waiters;   /* blocked waiting to get in sendrecv */
#ifdef CONFIG_CIFS_FSCACHE
	struct fscache_cookie   *fscache; /* client index cache cookie */
#endif
#ifdef CONFIG_CIFS_STATS2
	atomic_t num_cmds[NUMBER_OF_SMB2_COMMANDS]; /* total requests by cmd */
	atomic_t smb2slowcmd[NUMBER_OF_SMB2_COMMANDS]; /* count resps > 1 sec */
	__u64 time_per_cmd[NUMBER_OF_SMB2_COMMANDS]; /* total time per cmd */
	__u32 slowest_cmd[NUMBER_OF_SMB2_COMMANDS];
	__u32 fastest_cmd[NUMBER_OF_SMB2_COMMANDS];
#endif /* STATS2 */
	unsigned int	max_read;
	unsigned int	max_write;
	unsigned int	min_offload;
	__le16	compress_algorithm;
	__le16	cipher_type;
	 /* save initital negprot hash */
	__u8	preauth_sha_hash[SMB2_PREAUTH_HASH_SIZE];
	bool	posix_ext_supported;
	struct delayed_work reconnect; /* reconnect workqueue job */
	struct mutex reconnect_mutex; /* prevent simultaneous reconnects */
	unsigned long echo_interval;

	/*
	 * Number of targets available for reconnect. The more targets
	 * the more tasks have to wait to let the demultiplex thread
	 * reconnect.
	 */
	int nr_targets;
	bool noblockcnt; /* use non-blocking connect() */
	bool is_channel; /* if a session channel */
#ifdef CONFIG_CIFS_SWN_UPCALL
	bool use_swn_dstaddr;
	struct sockaddr_storage swn_dstaddr;
#endif
};

struct cifs_credits {
	unsigned int value;
	unsigned int instance;
};

static inline unsigned int
in_flight(struct TCP_Server_Info *server)
{
	unsigned int num;
	spin_lock(&server->req_lock);
	num = server->in_flight;
	spin_unlock(&server->req_lock);
	return num;
}

static inline bool
has_credits(struct TCP_Server_Info *server, int *credits, int num_credits)
{
	int num;
	spin_lock(&server->req_lock);
	num = *credits;
	spin_unlock(&server->req_lock);
	return num >= num_credits;
}

static inline void
add_credits(struct TCP_Server_Info *server, const struct cifs_credits *credits,
	    const int optype)
{
	server->ops->add_credits(server, credits, optype);
}

static inline void
add_credits_and_wake_if(struct TCP_Server_Info *server,
			const struct cifs_credits *credits, const int optype)
{
	if (credits->value) {
		server->ops->add_credits(server, credits, optype);
		wake_up(&server->request_q);
	}
}

static inline void
set_credits(struct TCP_Server_Info *server, const int val)
{
	server->ops->set_credits(server, val);
}

static inline int
adjust_credits(struct TCP_Server_Info *server, struct cifs_credits *credits,
	       const unsigned int payload_size)
{
	return server->ops->adjust_credits ?
		server->ops->adjust_credits(server, credits, payload_size) : 0;
}

static inline __le64
get_next_mid64(struct TCP_Server_Info *server)
{
	return cpu_to_le64(server->ops->get_next_mid(server));
}

static inline __le16
get_next_mid(struct TCP_Server_Info *server)
{
	__u16 mid = server->ops->get_next_mid(server);
	/*
	 * The value in the SMB header should be little endian for easy
	 * on-the-wire decoding.
	 */
	return cpu_to_le16(mid);
}

static inline void
revert_current_mid(struct TCP_Server_Info *server, const unsigned int val)
{
	if (server->ops->revert_current_mid)
		server->ops->revert_current_mid(server, val);
}

static inline void
revert_current_mid_from_hdr(struct TCP_Server_Info *server,
			    const struct smb2_sync_hdr *shdr)
{
	unsigned int num = le16_to_cpu(shdr->CreditCharge);

	return revert_current_mid(server, num > 0 ? num : 1);
}

static inline __u16
get_mid(const struct smb_hdr *smb)
{
	return le16_to_cpu(smb->Mid);
}

static inline bool
compare_mid(__u16 mid, const struct smb_hdr *smb)
{
	return mid == le16_to_cpu(smb->Mid);
}

/*
 * When the server supports very large reads and writes via POSIX extensions,
 * we can allow up to 2^24-1, minus the size of a READ/WRITE_AND_X header, not
 * including the RFC1001 length.
 *
 * Note that this might make for "interesting" allocation problems during
 * writeback however as we have to allocate an array of pointers for the
 * pages. A 16M write means ~32kb page array with PAGE_SIZE == 4096.
 *
 * For reads, there is a similar problem as we need to allocate an array
 * of kvecs to handle the receive, though that should only need to be done
 * once.
 */
#define CIFS_MAX_WSIZE ((1<<24) - 1 - sizeof(WRITE_REQ) + 4)
#define CIFS_MAX_RSIZE ((1<<24) - sizeof(READ_RSP) + 4)

/*
 * When the server doesn't allow large posix writes, only allow a rsize/wsize
 * of 2^17-1 minus the size of the call header. That allows for a read or
 * write up to the maximum size described by RFC1002.
 */
#define CIFS_MAX_RFC1002_WSIZE ((1<<17) - 1 - sizeof(WRITE_REQ) + 4)
#define CIFS_MAX_RFC1002_RSIZE ((1<<17) - 1 - sizeof(READ_RSP) + 4)

/*
 * The default wsize is 1M. find_get_pages seems to return a maximum of 256
 * pages in a single call. With PAGE_SIZE == 4k, this means we can fill
 * a single wsize request with a single call.
 */
#define CIFS_DEFAULT_IOSIZE (1024 * 1024)
#define SMB3_DEFAULT_IOSIZE (4 * 1024 * 1024)

/*
 * Windows only supports a max of 60kb reads and 65535 byte writes. Default to
 * those values when posix extensions aren't in force. In actuality here, we
 * use 65536 to allow for a write that is a multiple of 4k. Most servers seem
 * to be ok with the extra byte even though Windows doesn't send writes that
 * are that large.
 *
 * Citation:
 *
 * https://blogs.msdn.com/b/openspecification/archive/2009/04/10/smb-maximum-transmit-buffer-size-and-performance-tuning.aspx
 */
#define CIFS_DEFAULT_NON_POSIX_RSIZE (60 * 1024)
#define CIFS_DEFAULT_NON_POSIX_WSIZE (65536)

/*
 * Macros to allow the TCP_Server_Info->net field and related code to drop out
 * when CONFIG_NET_NS isn't set.
 */

#ifdef CONFIG_NET_NS

static inline struct net *cifs_net_ns(struct TCP_Server_Info *srv)
{
	return srv->net;
}

static inline void cifs_set_net_ns(struct TCP_Server_Info *srv, struct net *net)
{
	srv->net = net;
}

#else

static inline struct net *cifs_net_ns(struct TCP_Server_Info *srv)
{
	return &init_net;
}

static inline void cifs_set_net_ns(struct TCP_Server_Info *srv, struct net *net)
{
}

#endif

struct cifs_server_iface {
	size_t speed;
	unsigned int rdma_capable : 1;
	unsigned int rss_capable : 1;
	struct sockaddr_storage sockaddr;
};

struct cifs_chan {
	struct TCP_Server_Info *server;
	__u8 signkey[SMB3_SIGN_KEY_SIZE];
};

/*
 * Session structure.  One of these for each uid session with a particular host
 */
struct cifs_ses {
	struct list_head smb_ses_list;
	struct list_head tcon_list;
	struct cifs_tcon *tcon_ipc;
	struct mutex session_mutex;
	struct TCP_Server_Info *server;	/* pointer to server info */
	int ses_count;		/* reference counter */
	enum statusEnum status;
	unsigned overrideSecFlg;  /* if non-zero override global sec flags */
	char *serverOS;		/* name of operating system underlying server */
	char *serverNOS;	/* name of network operating system of server */
	char *serverDomain;	/* security realm of server */
	__u64 Suid;		/* remote smb uid  */
	kuid_t linux_uid;	/* overriding owner of files on the mount */
	kuid_t cred_uid;	/* owner of credentials */
	unsigned int capabilities;
	char ip_addr[INET6_ADDRSTRLEN + 1]; /* Max ipv6 (or v4) addr string len */
	char *user_name;	/* must not be null except during init of sess
				   and after mount option parsing we fill it */
	char *domainName;
	char *password;
	struct session_key auth_key;
	struct ntlmssp_auth *ntlmssp; /* ciphertext, flags, server challenge */
	enum securityEnum sectype; /* what security flavor was specified? */
	bool sign;		/* is signing required? */
	bool need_reconnect:1; /* connection reset, uid now invalid */
	bool domainAuto:1;
	bool binding:1; /* are we binding the session? */
	__u16 session_flags;
	__u8 smb3signingkey[SMB3_SIGN_KEY_SIZE];
	__u8 smb3encryptionkey[SMB3_ENC_DEC_KEY_SIZE];
	__u8 smb3decryptionkey[SMB3_ENC_DEC_KEY_SIZE];
	__u8 preauth_sha_hash[SMB2_PREAUTH_HASH_SIZE];

	__u8 binding_preauth_sha_hash[SMB2_PREAUTH_HASH_SIZE];

	/*
	 * Network interfaces available on the server this session is
	 * connected to.
	 *
	 * Other channels can be opened by connecting and binding this
	 * session to interfaces from this list.
	 *
	 * iface_lock should be taken when accessing any of these fields
	 */
	spinlock_t iface_lock;
	struct cifs_server_iface *iface_list;
	size_t iface_count;
	unsigned long iface_last_update; /* jiffies */

#define CIFS_MAX_CHANNELS 16
	struct cifs_chan chans[CIFS_MAX_CHANNELS];
	struct cifs_chan *binding_chan;
	size_t chan_count;
	size_t chan_max;
	atomic_t chan_seq; /* round robin state */
};

/*
 * When binding a new channel, we need to access the channel which isn't fully
 * established yet.
 */

static inline
struct cifs_chan *cifs_ses_binding_channel(struct cifs_ses *ses)
{
	if (ses->binding)
		return ses->binding_chan;
	else
		return NULL;
}

/*
 * Returns the server pointer of the session. When binding a new
 * channel this returns the last channel which isn't fully established
 * yet.
 *
 * This function should be use for negprot/sess.setup codepaths. For
 * the other requests see cifs_pick_channel().
 */
static inline
struct TCP_Server_Info *cifs_ses_server(struct cifs_ses *ses)
{
	if (ses->binding)
		return ses->binding_chan->server;
	else
		return ses->server;
}

static inline bool
cap_unix(struct cifs_ses *ses)
{
	return ses->server->vals->cap_unix & ses->capabilities;
}

struct cached_fid {
	bool is_valid:1;	/* Do we have a useable root fid */
	bool file_all_info_is_valid:1;
	bool has_lease:1;
	unsigned long time; /* jiffies of when lease was taken */
	struct kref refcount;
	struct cifs_fid *fid;
	struct mutex fid_mutex;
	struct cifs_tcon *tcon;
	struct dentry *dentry;
	struct work_struct lease_break;
	struct smb2_file_all_info file_all_info;
};

/*
 * there is one of these for each connection to a resource on a particular
 * session
 */
struct cifs_tcon {
	struct list_head tcon_list;
	int tc_count;
	struct list_head rlist; /* reconnect list */
	atomic_t num_local_opens;  /* num of all opens including disconnected */
	atomic_t num_remote_opens; /* num of all network opens on server */
	struct list_head openFileList;
	spinlock_t open_file_lock; /* protects list above */
	struct cifs_ses *ses;	/* pointer to session associated with */
	char treeName[MAX_TREE_SIZE + 1]; /* UNC name of resource in ASCII */
	char *nativeFileSystem;
	char *password;		/* for share-level security */
	__u32 tid;		/* The 4 byte tree id */
	__u16 Flags;		/* optional support bits */
	enum statusEnum tidStatus;
	atomic_t num_smbs_sent;
	union {
		struct {
			atomic_t num_writes;
			atomic_t num_reads;
			atomic_t num_flushes;
			atomic_t num_oplock_brks;
			atomic_t num_opens;
			atomic_t num_closes;
			atomic_t num_deletes;
			atomic_t num_mkdirs;
			atomic_t num_posixopens;
			atomic_t num_posixmkdirs;
			atomic_t num_rmdirs;
			atomic_t num_renames;
			atomic_t num_t2renames;
			atomic_t num_ffirst;
			atomic_t num_fnext;
			atomic_t num_fclose;
			atomic_t num_hardlinks;
			atomic_t num_symlinks;
			atomic_t num_locks;
			atomic_t num_acl_get;
			atomic_t num_acl_set;
		} cifs_stats;
		struct {
			atomic_t smb2_com_sent[NUMBER_OF_SMB2_COMMANDS];
			atomic_t smb2_com_failed[NUMBER_OF_SMB2_COMMANDS];
		} smb2_stats;
	} stats;
	__u64    bytes_read;
	__u64    bytes_written;
	spinlock_t stat_lock;  /* protects the two fields above */
	FILE_SYSTEM_DEVICE_INFO fsDevInfo;
	FILE_SYSTEM_ATTRIBUTE_INFO fsAttrInfo; /* ok if fs name truncated */
	FILE_SYSTEM_UNIX_INFO fsUnixInfo;
	bool ipc:1;   /* set if connection to IPC$ share (always also pipe) */
	bool pipe:1;  /* set if connection to pipe share */
	bool print:1; /* set if connection to printer share */
	bool retry:1;
	bool nocase:1;
	bool nohandlecache:1; /* if strange server resource prob can turn off */
	bool nodelete:1;
	bool seal:1;      /* transport encryption for this mounted share */
	bool unix_ext:1;  /* if false disable Linux extensions to CIFS protocol
				for this mount even if server would support */
	bool posix_extensions; /* if true SMB3.11 posix extensions enabled */
	bool local_lease:1; /* check leases (only) on local system not remote */
	bool broken_posix_open; /* e.g. Samba server versions < 3.3.2, 3.2.9 */
	bool broken_sparse_sup; /* if server or share does not support sparse */
	bool need_reconnect:1; /* connection reset, tid now invalid */
	bool need_reopen_files:1; /* need to reopen tcon file handles */
	bool use_resilient:1; /* use resilient instead of durable handles */
	bool use_persistent:1; /* use persistent instead of durable handles */
	bool no_lease:1;    /* Do not request leases on files or directories */
	bool use_witness:1; /* use witness protocol */
	__le32 capabilities;
	__u32 share_flags;
	__u32 maximal_access;
	__u32 vol_serial_number;
	__le64 vol_create_time;
	__u64 snapshot_time; /* for timewarp tokens - timestamp of snapshot */
	__u32 handle_timeout; /* persistent and durable handle timeout in ms */
	__u32 ss_flags;		/* sector size flags */
	__u32 perf_sector_size; /* best sector size for perf */
	__u32 max_chunks;
	__u32 max_bytes_chunk;
	__u32 max_bytes_copy;
#ifdef CONFIG_CIFS_FSCACHE
	u64 resource_id;		/* server resource id */
	struct fscache_cookie *fscache;	/* cookie for share */
#endif
	struct list_head pending_opens;	/* list of incomplete opens */
	struct cached_fid crfid; /* Cached root fid */
	/* BB add field for back pointer to sb struct(s)? */
#ifdef CONFIG_CIFS_DFS_UPCALL
	char *dfs_path;
	int remap:2;
	struct list_head ulist; /* cache update list */
#endif
};

/*
 * This is a refcounted and timestamped container for a tcon pointer. The
 * container holds a tcon reference. It is considered safe to free one of
 * these when the tl_count goes to 0. The tl_time is the time of the last
 * "get" on the container.
 */
struct tcon_link {
	struct rb_node		tl_rbnode;
	kuid_t			tl_uid;
	unsigned long		tl_flags;
#define TCON_LINK_MASTER	0
#define TCON_LINK_PENDING	1
#define TCON_LINK_IN_TREE	2
	unsigned long		tl_time;
	atomic_t		tl_count;
	struct cifs_tcon	*tl_tcon;
};

extern struct tcon_link *cifs_sb_tlink(struct cifs_sb_info *cifs_sb);
extern void smb3_free_compound_rqst(int num_rqst, struct smb_rqst *rqst);

static inline struct cifs_tcon *
tlink_tcon(struct tcon_link *tlink)
{
	return tlink->tl_tcon;
}

static inline struct tcon_link *
cifs_sb_master_tlink(struct cifs_sb_info *cifs_sb)
{
	return cifs_sb->master_tlink;
}

extern void cifs_put_tlink(struct tcon_link *tlink);

static inline struct tcon_link *
cifs_get_tlink(struct tcon_link *tlink)
{
	if (tlink && !IS_ERR(tlink))
		atomic_inc(&tlink->tl_count);
	return tlink;
}

/* This function is always expected to succeed */
extern struct cifs_tcon *cifs_sb_master_tcon(struct cifs_sb_info *cifs_sb);

#define CIFS_OPLOCK_NO_CHANGE 0xfe

struct cifs_pending_open {
	struct list_head olist;
	struct tcon_link *tlink;
	__u8 lease_key[16];
	__u32 oplock;
};

struct cifs_deferred_close {
	struct list_head dlist;
	struct tcon_link *tlink;
	__u16  netfid;
	__u64  persistent_fid;
	__u64  volatile_fid;
};

/*
 * This info hangs off the cifsFileInfo structure, pointed to by llist.
 * This is used to track byte stream locks on the file
 */
struct cifsLockInfo {
	struct list_head llist;	/* pointer to next cifsLockInfo */
	struct list_head blist; /* pointer to locks blocked on this */
	wait_queue_head_t block_q;
	__u64 offset;
	__u64 length;
	__u32 pid;
	__u16 type;
	__u16 flags;
};

/*
 * One of these for each open instance of a file
 */
struct cifs_search_info {
	loff_t index_of_last_entry;
	__u16 entries_in_buffer;
	__u16 info_level;
	__u32 resume_key;
	char *ntwrk_buf_start;
	char *srch_entries_start;
	char *last_entry;
	const char *presume_name;
	unsigned int resume_name_len;
	bool endOfSearch:1;
	bool emptyDir:1;
	bool unicode:1;
	bool smallBuf:1; /* so we know which buf_release function to call */
};

#define ACL_NO_MODE	((umode_t)(-1))
struct cifs_open_parms {
	struct cifs_tcon *tcon;
	struct cifs_sb_info *cifs_sb;
	int disposition;
	int desired_access;
	int create_options;
	const char *path;
	struct cifs_fid *fid;
	umode_t mode;
	bool reconnect:1;
};

struct cifs_fid {
	__u16 netfid;
	__u64 persistent_fid;	/* persist file id for smb2 */
	__u64 volatile_fid;	/* volatile file id for smb2 */
	__u8 lease_key[SMB2_LEASE_KEY_SIZE];	/* lease key for smb2 */
	__u8 create_guid[16];
	__u32 access;
	struct cifs_pending_open *pending_open;
	unsigned int epoch;
#ifdef CONFIG_CIFS_DEBUG2
	__u64 mid;
#endif /* CIFS_DEBUG2 */
	bool purge_cache;
};

struct cifs_fid_locks {
	struct list_head llist;
	struct cifsFileInfo *cfile;	/* fid that owns locks */
	struct list_head locks;		/* locks held by fid above */
};

struct cifsFileInfo {
	/* following two lists are protected by tcon->open_file_lock */
	struct list_head tlist;	/* pointer to next fid owned by tcon */
	struct list_head flist;	/* next fid (file instance) for this inode */
	/* lock list below protected by cifsi->lock_sem */
	struct cifs_fid_locks *llist;	/* brlocks held by this fid */
	kuid_t uid;		/* allows finding which FileInfo structure */
	__u32 pid;		/* process id who opened file */
	struct cifs_fid fid;	/* file id from remote */
	struct list_head rlist; /* reconnect list */
	/* BB add lock scope info here if needed */ ;
	/* lock scope id (0 if none) */
	struct dentry *dentry;
	struct tcon_link *tlink;
	unsigned int f_flags;
	bool invalidHandle:1;	/* file closed via session abend */
	bool swapfile:1;
	bool oplock_break_cancelled:1;
	unsigned int oplock_epoch; /* epoch from the lease break */
	__u32 oplock_level; /* oplock/lease level from the lease break */
	int count;
	spinlock_t file_info_lock; /* protects four flag/count fields above */
	struct mutex fh_mutex; /* prevents reopen race after dead ses*/
	struct cifs_search_info srch_inf;
	struct work_struct oplock_break; /* work for oplock breaks */
	struct work_struct put; /* work for the final part of _put */
	struct delayed_work deferred;
	bool deferred_close_scheduled; /* Flag to indicate close is scheduled */
};

struct cifs_io_parms {
	__u16 netfid;
	__u64 persistent_fid;	/* persist file id for smb2 */
	__u64 volatile_fid;	/* volatile file id for smb2 */
	__u32 pid;
	__u64 offset;
	unsigned int length;
	struct cifs_tcon *tcon;
	struct TCP_Server_Info *server;
};

struct cifs_aio_ctx {
	struct kref		refcount;
	struct list_head	list;
	struct mutex		aio_mutex;
	struct completion	done;
	struct iov_iter		iter;
	struct kiocb		*iocb;
	struct cifsFileInfo	*cfile;
	struct bio_vec		*bv;
	loff_t			pos;
	unsigned int		npages;
	ssize_t			rc;
	unsigned int		len;
	unsigned int		total_len;
	bool			should_dirty;
	/*
	 * Indicates if this aio_ctx is for direct_io,
	 * If yes, iter is a copy of the user passed iov_iter
	 */
	bool			direct_io;
};

/* asynchronous read support */
struct cifs_readdata {
	struct kref			refcount;
	struct list_head		list;
	struct completion		done;
	struct cifsFileInfo		*cfile;
	struct address_space		*mapping;
	struct cifs_aio_ctx		*ctx;
	__u64				offset;
	unsigned int			bytes;
	unsigned int			got_bytes;
	pid_t				pid;
	int				result;
	struct work_struct		work;
	int (*read_into_pages)(struct TCP_Server_Info *server,
				struct cifs_readdata *rdata,
				unsigned int len);
	int (*copy_into_pages)(struct TCP_Server_Info *server,
				struct cifs_readdata *rdata,
				struct iov_iter *iter);
	struct kvec			iov[2];
	struct TCP_Server_Info		*server;
#ifdef CONFIG_CIFS_SMB_DIRECT
	struct smbd_mr			*mr;
#endif
	unsigned int			pagesz;
	unsigned int			page_offset;
	unsigned int			tailsz;
	struct cifs_credits		credits;
	unsigned int			nr_pages;
	struct page			**pages;
};

/* asynchronous write support */
struct cifs_writedata {
	struct kref			refcount;
	struct list_head		list;
	struct completion		done;
	enum writeback_sync_modes	sync_mode;
	struct work_struct		work;
	struct cifsFileInfo		*cfile;
	struct cifs_aio_ctx		*ctx;
	__u64				offset;
	pid_t				pid;
	unsigned int			bytes;
	int				result;
	struct TCP_Server_Info		*server;
#ifdef CONFIG_CIFS_SMB_DIRECT
	struct smbd_mr			*mr;
#endif
	unsigned int			pagesz;
	unsigned int			page_offset;
	unsigned int			tailsz;
	struct cifs_credits		credits;
	unsigned int			nr_pages;
	struct page			**pages;
};

/*
 * Take a reference on the file private data. Must be called with
 * cfile->file_info_lock held.
 */
static inline void
cifsFileInfo_get_locked(struct cifsFileInfo *cifs_file)
{
	++cifs_file->count;
}

struct cifsFileInfo *cifsFileInfo_get(struct cifsFileInfo *cifs_file);
void _cifsFileInfo_put(struct cifsFileInfo *cifs_file, bool wait_oplock_hdlr,
		       bool offload);
void cifsFileInfo_put(struct cifsFileInfo *cifs_file);

#define CIFS_CACHE_READ_FLG	1
#define CIFS_CACHE_HANDLE_FLG	2
#define CIFS_CACHE_RH_FLG	(CIFS_CACHE_READ_FLG | CIFS_CACHE_HANDLE_FLG)
#define CIFS_CACHE_WRITE_FLG	4
#define CIFS_CACHE_RW_FLG	(CIFS_CACHE_READ_FLG | CIFS_CACHE_WRITE_FLG)
#define CIFS_CACHE_RHW_FLG	(CIFS_CACHE_RW_FLG | CIFS_CACHE_HANDLE_FLG)

#define CIFS_CACHE_READ(cinode) ((cinode->oplock & CIFS_CACHE_READ_FLG) || (CIFS_SB(cinode->vfs_inode.i_sb)->mnt_cifs_flags & CIFS_MOUNT_RO_CACHE))
#define CIFS_CACHE_HANDLE(cinode) (cinode->oplock & CIFS_CACHE_HANDLE_FLG)
#define CIFS_CACHE_WRITE(cinode) ((cinode->oplock & CIFS_CACHE_WRITE_FLG) || (CIFS_SB(cinode->vfs_inode.i_sb)->mnt_cifs_flags & CIFS_MOUNT_RW_CACHE))

/*
 * One of these for each file inode
 */

struct cifsInodeInfo {
	bool can_cache_brlcks;
	struct list_head llist;	/* locks helb by this inode */
	/*
	 * NOTE: Some code paths call down_read(lock_sem) twice, so
	 * we must always use cifs_down_write() instead of down_write()
	 * for this semaphore to avoid deadlocks.
	 */
	struct rw_semaphore lock_sem;	/* protect the fields above */
	/* BB add in lists for dirty pages i.e. write caching info for oplock */
	struct list_head openFileList;
	spinlock_t	open_file_lock;	/* protects openFileList */
	__u32 cifsAttrs; /* e.g. DOS archive bit, sparse, compressed, system */
	unsigned int oplock;		/* oplock/lease level we have */
	unsigned int epoch;		/* used to track lease state changes */
#define CIFS_INODE_PENDING_OPLOCK_BREAK   (0) /* oplock break in progress */
#define CIFS_INODE_PENDING_WRITERS	  (1) /* Writes in progress */
#define CIFS_INODE_FLAG_UNUSED		  (2) /* Unused flag */
#define CIFS_INO_DELETE_PENDING		  (3) /* delete pending on server */
#define CIFS_INO_INVALID_MAPPING	  (4) /* pagecache is invalid */
#define CIFS_INO_LOCK			  (5) /* lock bit for synchronization */
#define CIFS_INO_MODIFIED_ATTR            (6) /* Indicate change in mtime/ctime */
	unsigned long flags;
	spinlock_t writers_lock;
	unsigned int writers;		/* Number of writers on this inode */
	unsigned long time;		/* jiffies of last update of inode */
	u64  server_eof;		/* current file size on server -- protected by i_lock */
	u64  uniqueid;			/* server inode number */
	u64  createtime;		/* creation time on server */
	__u8 lease_key[SMB2_LEASE_KEY_SIZE];	/* lease key for this inode */
#ifdef CONFIG_CIFS_FSCACHE
	struct fscache_cookie *fscache;
#endif
	struct inode vfs_inode;
	struct list_head deferred_closes; /* list of deferred closes */
	spinlock_t deferred_lock; /* protection on deferred list */
	bool lease_granted; /* Flag to indicate whether lease or oplock is granted. */
};

static inline struct cifsInodeInfo *
CIFS_I(struct inode *inode)
{
	return container_of(inode, struct cifsInodeInfo, vfs_inode);
}

static inline struct cifs_sb_info *
CIFS_SB(struct super_block *sb)
{
	return sb->s_fs_info;
}

static inline struct cifs_sb_info *
CIFS_FILE_SB(struct file *file)
{
	return CIFS_SB(file_inode(file)->i_sb);
}

static inline char CIFS_DIR_SEP(const struct cifs_sb_info *cifs_sb)
{
	if (cifs_sb->mnt_cifs_flags & CIFS_MOUNT_POSIX_PATHS)
		return '/';
	else
		return '\\';
}

static inline void
convert_delimiter(char *path, char delim)
{
	char old_delim, *pos;

	if (delim == '/')
		old_delim = '\\';
	else
		old_delim = '/';

	pos = path;
	while ((pos = strchr(pos, old_delim)))
		*pos = delim;
}

#define cifs_stats_inc atomic_inc

static inline void cifs_stats_bytes_written(struct cifs_tcon *tcon,
					    unsigned int bytes)
{
	if (bytes) {
		spin_lock(&tcon->stat_lock);
		tcon->bytes_written += bytes;
		spin_unlock(&tcon->stat_lock);
	}
}

static inline void cifs_stats_bytes_read(struct cifs_tcon *tcon,
					 unsigned int bytes)
{
	spin_lock(&tcon->stat_lock);
	tcon->bytes_read += bytes;
	spin_unlock(&tcon->stat_lock);
}


/*
 * This is the prototype for the mid receive function. This function is for
 * receiving the rest of the SMB frame, starting with the WordCount (which is
 * just after the MID in struct smb_hdr). Note:
 *
 * - This will be called by cifsd, with no locks held.
 * - The mid will still be on the pending_mid_q.
 * - mid->resp_buf will point to the current buffer.
 *
 * Returns zero on a successful receive, or an error. The receive state in
 * the TCP_Server_Info will also be updated.
 */
typedef int (mid_receive_t)(struct TCP_Server_Info *server,
			    struct mid_q_entry *mid);

/*
 * This is the prototype for the mid callback function. This is called once the
 * mid has been received off of the socket. When creating one, take special
 * care to avoid deadlocks. Things to bear in mind:
 *
 * - it will be called by cifsd, with no locks held
 * - the mid will be removed from any lists
 */
typedef void (mid_callback_t)(struct mid_q_entry *mid);

/*
 * This is the protopyte for mid handle function. This is called once the mid
 * has been recognized after decryption of the message.
 */
typedef int (mid_handle_t)(struct TCP_Server_Info *server,
			    struct mid_q_entry *mid);

/* one of these for every pending CIFS request to the server */
struct mid_q_entry {
	struct list_head qhead;	/* mids waiting on reply from this server */
	struct kref refcount;
	struct TCP_Server_Info *server;	/* server corresponding to this mid */
	__u64 mid;		/* multiplex id */
	__u16 credits;		/* number of credits consumed by this mid */
	__u16 credits_received;	/* number of credits from the response */
	__u32 pid;		/* process id */
	__u32 sequence_number;  /* for CIFS signing */
	unsigned long when_alloc;  /* when mid was created */
#ifdef CONFIG_CIFS_STATS2
	unsigned long when_sent; /* time when smb send finished */
	unsigned long when_received; /* when demux complete (taken off wire) */
#endif
	mid_receive_t *receive; /* call receive callback */
	mid_callback_t *callback; /* call completion callback */
	mid_handle_t *handle; /* call handle mid callback */
	void *callback_data;	  /* general purpose pointer for callback */
	struct task_struct *creator;
	void *resp_buf;		/* pointer to received SMB header */
	unsigned int resp_buf_size;
	int mid_state;	/* wish this were enum but can not pass to wait_event */
	unsigned int mid_flags;
	__le16 command;		/* smb command code */
	unsigned int optype;	/* operation type */
	bool large_buf:1;	/* if valid response, is pointer to large buf */
	bool multiRsp:1;	/* multiple trans2 responses for one request  */
	bool multiEnd:1;	/* both received */
	bool decrypted:1;	/* decrypted entry */
};

struct close_cancelled_open {
	struct cifs_fid         fid;
	struct cifs_tcon        *tcon;
	struct work_struct      work;
	__u64 mid;
	__u16 cmd;
};

/*	Make code in transport.c a little cleaner by moving
	update of optional stats into function below */
static inline void cifs_in_send_inc(struct TCP_Server_Info *server)
{
	atomic_inc(&server->in_send);
}

static inline void cifs_in_send_dec(struct TCP_Server_Info *server)
{
	atomic_dec(&server->in_send);
}

static inline void cifs_num_waiters_inc(struct TCP_Server_Info *server)
{
	atomic_inc(&server->num_waiters);
}

static inline void cifs_num_waiters_dec(struct TCP_Server_Info *server)
{
	atomic_dec(&server->num_waiters);
}

#ifdef CONFIG_CIFS_STATS2
static inline void cifs_save_when_sent(struct mid_q_entry *mid)
{
	mid->when_sent = jiffies;
}
#else
static inline void cifs_save_when_sent(struct mid_q_entry *mid)
{
}
#endif

/* for pending dnotify requests */
struct dir_notify_req {
	struct list_head lhead;
	__le16 Pid;
	__le16 PidHigh;
	__u16 Mid;
	__u16 Tid;
	__u16 Uid;
	__u16 netfid;
	__u32 filter; /* CompletionFilter (for multishot) */
	int multishot;
	struct file *pfile;
};

struct dfs_info3_param {
	int flags; /* DFSREF_REFERRAL_SERVER, DFSREF_STORAGE_SERVER*/
	int path_consumed;
	int server_type;
	int ref_flag;
	char *path_name;
	char *node_name;
	int ttl;
};

/*
 * common struct for holding inode info when searching for or updating an
 * inode with new info
 */

#define CIFS_FATTR_DFS_REFERRAL		0x1
#define CIFS_FATTR_DELETE_PENDING	0x2
#define CIFS_FATTR_NEED_REVAL		0x4
#define CIFS_FATTR_INO_COLLISION	0x8
#define CIFS_FATTR_UNKNOWN_NLINK	0x10
#define CIFS_FATTR_FAKE_ROOT_INO	0x20

struct cifs_fattr {
	u32		cf_flags;
	u32		cf_cifsattrs;
	u64		cf_uniqueid;
	u64		cf_eof;
	u64		cf_bytes;
	u64		cf_createtime;
	kuid_t		cf_uid;
	kgid_t		cf_gid;
	umode_t		cf_mode;
	dev_t		cf_rdev;
	unsigned int	cf_nlink;
	unsigned int	cf_dtype;
	struct timespec64 cf_atime;
	struct timespec64 cf_mtime;
	struct timespec64 cf_ctime;
	u32             cf_cifstag;
};

static inline void free_dfs_info_param(struct dfs_info3_param *param)
{
	if (param) {
		kfree(param->path_name);
		kfree(param->node_name);
	}
}

static inline void free_dfs_info_array(struct dfs_info3_param *param,
				       int number_of_items)
{
	int i;
	if ((number_of_items == 0) || (param == NULL))
		return;
	for (i = 0; i < number_of_items; i++) {
		kfree(param[i].path_name);
		kfree(param[i].node_name);
	}
	kfree(param);
}

static inline bool is_interrupt_error(int error)
{
	switch (error) {
	case -EINTR:
	case -ERESTARTSYS:
	case -ERESTARTNOHAND:
	case -ERESTARTNOINTR:
		return true;
	}
	return false;
}

static inline bool is_retryable_error(int error)
{
	if (is_interrupt_error(error) || error == -EAGAIN)
		return true;
	return false;
}


/* cifs_get_writable_file() flags */
#define FIND_WR_ANY         0
#define FIND_WR_FSUID_ONLY  1
#define FIND_WR_WITH_DELETE 2

#define   MID_FREE 0
#define   MID_REQUEST_ALLOCATED 1
#define   MID_REQUEST_SUBMITTED 2
#define   MID_RESPONSE_RECEIVED 4
#define   MID_RETRY_NEEDED      8 /* session closed while this request out */
#define   MID_RESPONSE_MALFORMED 0x10
#define   MID_SHUTDOWN		 0x20

/* Flags */
#define   MID_WAIT_CANCELLED	 1 /* Cancelled while waiting for response */
#define   MID_DELETED            2 /* Mid has been dequeued/deleted */

/* Types of response buffer returned from SendReceive2 */
#define   CIFS_NO_BUFFER        0    /* Response buffer not returned */
#define   CIFS_SMALL_BUFFER     1
#define   CIFS_LARGE_BUFFER     2
#define   CIFS_IOVEC            4    /* array of response buffers */

/* Type of Request to SendReceive2 */
#define   CIFS_BLOCKING_OP      1    /* operation can block */
#define   CIFS_NON_BLOCKING     2    /* do not block waiting for credits */
#define   CIFS_TIMEOUT_MASK 0x003    /* only one of above set in req */
#define   CIFS_LOG_ERROR    0x010    /* log NT STATUS if non-zero */
#define   CIFS_LARGE_BUF_OP 0x020    /* large request buffer */
#define   CIFS_NO_RSP_BUF   0x040    /* no response buffer required */

/* Type of request operation */
#define   CIFS_ECHO_OP            0x080  /* echo request */
#define   CIFS_OBREAK_OP          0x0100 /* oplock break request */
#define   CIFS_NEG_OP             0x0200 /* negotiate request */
#define   CIFS_CP_CREATE_CLOSE_OP 0x0400 /* compound create+close request */
/* Lower bitmask values are reserved by others below. */
#define   CIFS_SESS_OP            0x2000 /* session setup request */
#define   CIFS_OP_MASK            0x2780 /* mask request type */

#define   CIFS_HAS_CREDITS        0x0400 /* already has credits */
#define   CIFS_TRANSFORM_REQ      0x0800 /* transform request before sending */
#define   CIFS_NO_SRV_RSP         0x1000 /* there is no server response */

/* Security Flags: indicate type of session setup needed */
#define   CIFSSEC_MAY_SIGN	0x00001
#define   CIFSSEC_MAY_NTLM	0x00002
#define   CIFSSEC_MAY_NTLMV2	0x00004
#define   CIFSSEC_MAY_KRB5	0x00008
#ifdef CONFIG_CIFS_WEAK_PW_HASH
#define   CIFSSEC_MAY_LANMAN	0x00010
#define   CIFSSEC_MAY_PLNTXT	0x00020
#else
#define   CIFSSEC_MAY_LANMAN    0
#define   CIFSSEC_MAY_PLNTXT    0
#endif /* weak passwords */
#define   CIFSSEC_MAY_SEAL	0x00040 /* not supported yet */
#define   CIFSSEC_MAY_NTLMSSP	0x00080 /* raw ntlmssp with ntlmv2 */

#define   CIFSSEC_MUST_SIGN	0x01001
/* note that only one of the following can be set so the
result of setting MUST flags more than once will be to
require use of the stronger protocol */
#define   CIFSSEC_MUST_NTLM	0x02002
#define   CIFSSEC_MUST_NTLMV2	0x04004
#define   CIFSSEC_MUST_KRB5	0x08008
#ifdef CONFIG_CIFS_WEAK_PW_HASH
#define   CIFSSEC_MUST_LANMAN	0x10010
#define   CIFSSEC_MUST_PLNTXT	0x20020
#ifdef CONFIG_CIFS_UPCALL
#define   CIFSSEC_MASK          0xBF0BF /* allows weak security but also krb5 */
#else
#define   CIFSSEC_MASK          0xB70B7 /* current flags supported if weak */
#endif /* UPCALL */
#else /* do not allow weak pw hash */
#define   CIFSSEC_MUST_LANMAN	0
#define   CIFSSEC_MUST_PLNTXT	0
#ifdef CONFIG_CIFS_UPCALL
#define   CIFSSEC_MASK          0x8F08F /* flags supported if no weak allowed */
#else
#define	  CIFSSEC_MASK          0x87087 /* flags supported if no weak allowed */
#endif /* UPCALL */
#endif /* WEAK_PW_HASH */
#define   CIFSSEC_MUST_SEAL	0x40040 /* not supported yet */
#define   CIFSSEC_MUST_NTLMSSP	0x80080 /* raw ntlmssp with ntlmv2 */

#define   CIFSSEC_DEF (CIFSSEC_MAY_SIGN | CIFSSEC_MAY_NTLMV2 | CIFSSEC_MAY_NTLMSSP)
#define   CIFSSEC_MAX (CIFSSEC_MUST_SIGN | CIFSSEC_MUST_NTLMV2)
#define   CIFSSEC_AUTH_MASK (CIFSSEC_MAY_NTLM | CIFSSEC_MAY_NTLMV2 | CIFSSEC_MAY_LANMAN | CIFSSEC_MAY_PLNTXT | CIFSSEC_MAY_KRB5 | CIFSSEC_MAY_NTLMSSP)
/*
 *****************************************************************
 * All constants go here
 *****************************************************************
 */

#define UID_HASH (16)

/*
 * Note that ONE module should define _DECLARE_GLOBALS_HERE to cause the
 * following to be declared.
 */

/****************************************************************************
 *  Locking notes.  All updates to global variables and lists should be
 *                  protected by spinlocks or semaphores.
 *
 *  Spinlocks
 *  ---------
 *  GlobalMid_Lock protects:
 *	list operations on pending_mid_q and oplockQ
 *      updates to XID counters, multiplex id  and SMB sequence numbers
 *      list operations on global DnotifyReqList
 *  tcp_ses_lock protects:
 *	list operations on tcp and SMB session lists
 *  tcon->open_file_lock protects the list of open files hanging off the tcon
 *  inode->open_file_lock protects the openFileList hanging off the inode
 *  cfile->file_info_lock protects counters and fields in cifs file struct
 *  f_owner.lock protects certain per file struct operations
 *  mapping->page_lock protects certain per page operations
 *
 *  Note that the cifs_tcon.open_file_lock should be taken before
 *  not after the cifsInodeInfo.open_file_lock
 *
 *  Semaphores
 *  ----------
 *  cifsInodeInfo->lock_sem protects:
 *	the list of locks held by the inode
 *
 ****************************************************************************/

#ifdef DECLARE_GLOBALS_HERE
#define GLOBAL_EXTERN
#else
#define GLOBAL_EXTERN extern
#endif

/*
 * the list of TCP_Server_Info structures, ie each of the sockets
 * connecting our client to a distinct server (ip address), is
 * chained together by cifs_tcp_ses_list. The list of all our SMB
 * sessions (and from that the tree connections) can be found
 * by iterating over cifs_tcp_ses_list
 */
GLOBAL_EXTERN struct list_head		cifs_tcp_ses_list;

/*
 * This lock protects the cifs_tcp_ses_list, the list of smb sessions per
 * tcp session, and the list of tcon's per smb session. It also protects
 * the reference counters for the server, smb session, and tcon. It also
 * protects some fields in the TCP_Server_Info struct such as dstaddr. Finally,
 * changes to the tcon->tidStatus should be done while holding this lock.
 * generally the locks should be taken in order tcp_ses_lock before
 * tcon->open_file_lock and that before file->file_info_lock since the
 * structure order is cifs_socket-->cifs_ses-->cifs_tcon-->cifs_file
 */
GLOBAL_EXTERN spinlock_t		cifs_tcp_ses_lock;

/*
 * Global transaction id (XID) information
 */
GLOBAL_EXTERN unsigned int GlobalCurrentXid;	/* protected by GlobalMid_Sem */
GLOBAL_EXTERN unsigned int GlobalTotalActiveXid; /* prot by GlobalMid_Sem */
GLOBAL_EXTERN unsigned int GlobalMaxActiveXid;	/* prot by GlobalMid_Sem */
GLOBAL_EXTERN spinlock_t GlobalMid_Lock;  /* protects above & list operations */
					  /* on midQ entries */
/*
 *  Global counters, updated atomically
 */
GLOBAL_EXTERN atomic_t sesInfoAllocCount;
GLOBAL_EXTERN atomic_t tconInfoAllocCount;
GLOBAL_EXTERN atomic_t tcpSesNextId;
GLOBAL_EXTERN atomic_t tcpSesAllocCount;
GLOBAL_EXTERN atomic_t tcpSesReconnectCount;
GLOBAL_EXTERN atomic_t tconInfoReconnectCount;

/* Various Debug counters */
GLOBAL_EXTERN atomic_t bufAllocCount;    /* current number allocated  */
#ifdef CONFIG_CIFS_STATS2
GLOBAL_EXTERN atomic_t totBufAllocCount; /* total allocated over all time */
GLOBAL_EXTERN atomic_t totSmBufAllocCount;
extern unsigned int slow_rsp_threshold; /* number of secs before logging */
#endif
GLOBAL_EXTERN atomic_t smBufAllocCount;
GLOBAL_EXTERN atomic_t midCount;

/* Misc globals */
extern bool enable_oplocks; /* enable or disable oplocks */
extern bool lookupCacheEnabled;
extern unsigned int global_secflags;	/* if on, session setup sent
				with more secure ntlmssp2 challenge/resp */
extern unsigned int sign_CIFS_PDUs;  /* enable smb packet signing */
extern bool enable_gcm_256; /* allow optional negotiate of strongest signing (aes-gcm-256) */
extern bool require_gcm_256; /* require use of strongest signing (aes-gcm-256) */
extern bool linuxExtEnabled;/*enable Linux/Unix CIFS extensions*/
extern unsigned int CIFSMaxBufSize;  /* max size not including hdr */
extern unsigned int cifs_min_rcv;    /* min size of big ntwrk buf pool */
extern unsigned int cifs_min_small;  /* min size of small buf pool */
extern unsigned int cifs_max_pending; /* MAX requests at once to server*/
extern bool disable_legacy_dialects;  /* forbid vers=1.0 and vers=2.0 mounts */

void cifs_oplock_break(struct work_struct *work);
void cifs_queue_oplock_break(struct cifsFileInfo *cfile);
void smb2_deferred_work_close(struct work_struct *work);

extern const struct slow_work_ops cifs_oplock_break_ops;
extern struct workqueue_struct *cifsiod_wq;
extern struct workqueue_struct *decrypt_wq;
extern struct workqueue_struct *fileinfo_put_wq;
extern struct workqueue_struct *cifsoplockd_wq;
extern struct workqueue_struct *deferredclose_wq;
extern __u32 cifs_lock_secret;

extern mempool_t *cifs_mid_poolp;

/* Operations for different SMB versions */
#define SMB1_VERSION_STRING	"1.0"
extern struct smb_version_operations smb1_operations;
extern struct smb_version_values smb1_values;
#define SMB20_VERSION_STRING	"2.0"
extern struct smb_version_operations smb20_operations;
extern struct smb_version_values smb20_values;
#define SMB21_VERSION_STRING	"2.1"
extern struct smb_version_operations smb21_operations;
extern struct smb_version_values smb21_values;
#define SMBDEFAULT_VERSION_STRING "default"
extern struct smb_version_values smbdefault_values;
#define SMB3ANY_VERSION_STRING "3"
extern struct smb_version_values smb3any_values;
#define SMB30_VERSION_STRING	"3.0"
extern struct smb_version_operations smb30_operations;
extern struct smb_version_values smb30_values;
#define SMB302_VERSION_STRING	"3.02"
#define ALT_SMB302_VERSION_STRING "3.0.2"
/*extern struct smb_version_operations smb302_operations;*/ /* not needed yet */
extern struct smb_version_values smb302_values;
#define SMB311_VERSION_STRING	"3.1.1"
#define ALT_SMB311_VERSION_STRING "3.11"
extern struct smb_version_operations smb311_operations;
extern struct smb_version_values smb311_values;

static inline char *get_security_type_str(enum securityEnum sectype)
{
	switch (sectype) {
	case RawNTLMSSP:
		return "RawNTLMSSP";
	case Kerberos:
		return "Kerberos";
	case NTLMv2:
		return "NTLMv2";
	case NTLM:
		return "NTLM";
	case LANMAN:
		return "LANMAN";
	default:
		return "Unknown";
	}
}

static inline bool is_smb1_server(struct TCP_Server_Info *server)
{
	return strcmp(server->vals->version_string, SMB1_VERSION_STRING) == 0;
}

static inline bool is_tcon_dfs(struct cifs_tcon *tcon)
{
	/*
	 * For SMB1, see MS-CIFS 2.4.55 SMB_COM_TREE_CONNECT_ANDX (0x75) and MS-CIFS 3.3.4.4 DFS
	 * Subsystem Notifies That a Share Is a DFS Share.
	 *
	 * For SMB2+, see MS-SMB2 2.2.10 SMB2 TREE_CONNECT Response and MS-SMB2 3.3.4.14 Server
	 * Application Updates a Share.
	 */
	if (!tcon || !tcon->ses || !tcon->ses->server)
		return false;
	return is_smb1_server(tcon->ses->server) ? tcon->Flags & SMB_SHARE_IS_IN_DFS :
		tcon->share_flags & (SHI1005_FLAGS_DFS | SHI1005_FLAGS_DFS_ROOT);
}

#endif	/* _CIFS_GLOB_H */
