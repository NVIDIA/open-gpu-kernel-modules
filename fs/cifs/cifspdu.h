/*
 *   fs/cifs/cifspdu.h
 *
 *   Copyright (c) International Business Machines  Corp., 2002,2009
 *   Author(s): Steve French (sfrench@us.ibm.com)
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
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with this library; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef _CIFSPDU_H
#define _CIFSPDU_H

#include <net/sock.h>
#include <asm/unaligned.h>
#include "smbfsctl.h"

#ifdef CONFIG_CIFS_WEAK_PW_HASH
#define LANMAN_PROT 0
#define LANMAN2_PROT 1
#define CIFS_PROT   2
#else
#define CIFS_PROT   0
#endif
#define POSIX_PROT  (CIFS_PROT+1)
#define BAD_PROT 0xFFFF

/* SMB command codes:
 * Note some commands have minimal (wct=0,bcc=0), or uninteresting, responses
 * (ie which include no useful data other than the SMB error code itself).
 * This can allow us to avoid response buffer allocations and copy in some cases
 */
#define SMB_COM_CREATE_DIRECTORY      0x00 /* trivial response */
#define SMB_COM_DELETE_DIRECTORY      0x01 /* trivial response */
#define SMB_COM_CLOSE                 0x04 /* triv req/rsp, timestamp ignored */
#define SMB_COM_FLUSH                 0x05 /* triv req/rsp */
#define SMB_COM_DELETE                0x06 /* trivial response */
#define SMB_COM_RENAME                0x07 /* trivial response */
#define SMB_COM_QUERY_INFORMATION     0x08 /* aka getattr */
#define SMB_COM_SETATTR               0x09 /* trivial response */
#define SMB_COM_LOCKING_ANDX          0x24 /* trivial response */
#define SMB_COM_COPY                  0x29 /* trivial rsp, fail filename ignrd*/
#define SMB_COM_ECHO                  0x2B /* echo request */
#define SMB_COM_OPEN_ANDX             0x2D /* Legacy open for old servers */
#define SMB_COM_READ_ANDX             0x2E
#define SMB_COM_WRITE_ANDX            0x2F
#define SMB_COM_TRANSACTION2          0x32
#define SMB_COM_TRANSACTION2_SECONDARY 0x33
#define SMB_COM_FIND_CLOSE2           0x34 /* trivial response */
#define SMB_COM_TREE_DISCONNECT       0x71 /* trivial response */
#define SMB_COM_NEGOTIATE             0x72
#define SMB_COM_SESSION_SETUP_ANDX    0x73
#define SMB_COM_LOGOFF_ANDX           0x74 /* trivial response */
#define SMB_COM_TREE_CONNECT_ANDX     0x75
#define SMB_COM_NT_TRANSACT           0xA0
#define SMB_COM_NT_TRANSACT_SECONDARY 0xA1
#define SMB_COM_NT_CREATE_ANDX        0xA2
#define SMB_COM_NT_CANCEL             0xA4 /* no response */
#define SMB_COM_NT_RENAME             0xA5 /* trivial response */

/* Transact2 subcommand codes */
#define TRANS2_OPEN                   0x00
#define TRANS2_FIND_FIRST             0x01
#define TRANS2_FIND_NEXT              0x02
#define TRANS2_QUERY_FS_INFORMATION   0x03
#define TRANS2_SET_FS_INFORMATION     0x04
#define TRANS2_QUERY_PATH_INFORMATION 0x05
#define TRANS2_SET_PATH_INFORMATION   0x06
#define TRANS2_QUERY_FILE_INFORMATION 0x07
#define TRANS2_SET_FILE_INFORMATION   0x08
#define TRANS2_GET_DFS_REFERRAL       0x10
#define TRANS2_REPORT_DFS_INCOSISTENCY 0x11

/* SMB Transact (Named Pipe) subcommand codes */
#define TRANS_SET_NMPIPE_STATE      0x0001
#define TRANS_RAW_READ_NMPIPE       0x0011
#define TRANS_QUERY_NMPIPE_STATE    0x0021
#define TRANS_QUERY_NMPIPE_INFO     0x0022
#define TRANS_PEEK_NMPIPE           0x0023
#define TRANS_TRANSACT_NMPIPE       0x0026
#define TRANS_RAW_WRITE_NMPIPE      0x0031
#define TRANS_READ_NMPIPE           0x0036
#define TRANS_WRITE_NMPIPE          0x0037
#define TRANS_WAIT_NMPIPE           0x0053
#define TRANS_CALL_NMPIPE           0x0054

/* NT Transact subcommand codes */
#define NT_TRANSACT_CREATE            0x01
#define NT_TRANSACT_IOCTL             0x02
#define NT_TRANSACT_SET_SECURITY_DESC 0x03
#define NT_TRANSACT_NOTIFY_CHANGE     0x04
#define NT_TRANSACT_RENAME            0x05
#define NT_TRANSACT_QUERY_SECURITY_DESC 0x06
#define NT_TRANSACT_GET_USER_QUOTA    0x07
#define NT_TRANSACT_SET_USER_QUOTA    0x08

#define MAX_CIFS_SMALL_BUFFER_SIZE 448 /* big enough for most */
/* future chained NTCreateXReadX bigger, but for time being NTCreateX biggest */
/* among the requests (NTCreateX response is bigger with wct of 34) */
#define MAX_CIFS_HDR_SIZE 0x58 /* 4 len + 32 hdr + (2*24 wct) + 2 bct + 2 pad */
#define CIFS_SMALL_PATH 120 /* allows for (448-88)/3 */

/* internal cifs vfs structures */
/*****************************************************************
 * All constants go here
 *****************************************************************
 */

/*
 * Starting value for maximum SMB size negotiation
 */
#define CIFS_MAX_MSGSIZE (4*4096)

/*
 * Size of encrypted user password in bytes
 */
#define CIFS_ENCPWD_SIZE (16)

/*
 * Size of the crypto key returned on the negotiate SMB in bytes
 */
#define CIFS_CRYPTO_KEY_SIZE (8)

/*
 * Size of the ntlm client response
 */
#define CIFS_AUTH_RESP_SIZE (24)

/*
 * Size of the session key (crypto key encrypted with the password
 */
#define CIFS_SESS_KEY_SIZE (16)

/*
 * Size of the smb3 signing key
 */
#define SMB3_SIGN_KEY_SIZE (16)

/*
 * Size of the smb3 encryption/decryption key storage.
 * This size is big enough to store any cipher key types.
 */
#define SMB3_ENC_DEC_KEY_SIZE (32)

#define CIFS_CLIENT_CHALLENGE_SIZE (8)
#define CIFS_SERVER_CHALLENGE_SIZE (8)
#define CIFS_HMAC_MD5_HASH_SIZE (16)
#define CIFS_CPHTXT_SIZE (16)
#define CIFS_NTHASH_SIZE (16)

/*
 * Maximum user name length
 */
#define CIFS_UNLEN (20)

/*
 * Flags on SMB open
 */
#define SMBOPEN_WRITE_THROUGH 0x4000
#define SMBOPEN_DENY_ALL      0x0010
#define SMBOPEN_DENY_WRITE    0x0020
#define SMBOPEN_DENY_READ     0x0030
#define SMBOPEN_DENY_NONE     0x0040
#define SMBOPEN_READ          0x0000
#define SMBOPEN_WRITE         0x0001
#define SMBOPEN_READWRITE     0x0002
#define SMBOPEN_EXECUTE       0x0003

#define SMBOPEN_OCREATE       0x0010
#define SMBOPEN_OTRUNC        0x0002
#define SMBOPEN_OAPPEND       0x0001

/*
 * SMB flag definitions
 */
#define SMBFLG_EXTD_LOCK 0x01	/* server supports lock-read write-unlock smb */
#define SMBFLG_RCV_POSTED 0x02	/* obsolete */
#define SMBFLG_RSVD 0x04
#define SMBFLG_CASELESS 0x08	/* all pathnames treated as caseless (off
				implies case sensitive file handling request) */
#define SMBFLG_CANONICAL_PATH_FORMAT 0x10	/* obsolete */
#define SMBFLG_OLD_OPLOCK 0x20	/* obsolete */
#define SMBFLG_OLD_OPLOCK_NOTIFY 0x40	/* obsolete */
#define SMBFLG_RESPONSE 0x80	/* this PDU is a response from server */

/*
 * SMB flag2 definitions
 */
#define SMBFLG2_KNOWS_LONG_NAMES cpu_to_le16(1)	/* can send long (non-8.3)
						   path names in response */
#define SMBFLG2_KNOWS_EAS cpu_to_le16(2)
#define SMBFLG2_SECURITY_SIGNATURE cpu_to_le16(4)
#define SMBFLG2_COMPRESSED (8)
#define SMBFLG2_SECURITY_SIGNATURE_REQUIRED (0x10)
#define SMBFLG2_IS_LONG_NAME cpu_to_le16(0x40)
#define SMBFLG2_REPARSE_PATH (0x400)
#define SMBFLG2_EXT_SEC cpu_to_le16(0x800)
#define SMBFLG2_DFS cpu_to_le16(0x1000)
#define SMBFLG2_PAGING_IO cpu_to_le16(0x2000)
#define SMBFLG2_ERR_STATUS cpu_to_le16(0x4000)
#define SMBFLG2_UNICODE cpu_to_le16(0x8000)

/*
 * These are the file access permission bits defined in CIFS for the
 * NTCreateAndX as well as the level 0x107
 * TRANS2_QUERY_PATH_INFORMATION API.  The level 0x107, SMB_QUERY_FILE_ALL_INFO
 * responds with the AccessFlags.
 * The AccessFlags specifies the access permissions a caller has to the
 * file and can have any suitable combination of the following values:
 */

#define FILE_READ_DATA        0x00000001  /* Data can be read from the file   */
#define FILE_WRITE_DATA       0x00000002  /* Data can be written to the file  */
#define FILE_APPEND_DATA      0x00000004  /* Data can be appended to the file */
#define FILE_READ_EA          0x00000008  /* Extended attributes associated   */
					  /* with the file can be read        */
#define FILE_WRITE_EA         0x00000010  /* Extended attributes associated   */
					  /* with the file can be written     */
#define FILE_EXECUTE          0x00000020  /*Data can be read into memory from */
					  /* the file using system paging I/O */
#define FILE_DELETE_CHILD     0x00000040
#define FILE_READ_ATTRIBUTES  0x00000080  /* Attributes associated with the   */
					  /* file can be read                 */
#define FILE_WRITE_ATTRIBUTES 0x00000100  /* Attributes associated with the   */
					  /* file can be written              */
#define DELETE                0x00010000  /* The file can be deleted          */
#define READ_CONTROL          0x00020000  /* The access control list and      */
					  /* ownership associated with the    */
					  /* file can be read                 */
#define WRITE_DAC             0x00040000  /* The access control list and      */
					  /* ownership associated with the    */
					  /* file can be written.             */
#define WRITE_OWNER           0x00080000  /* Ownership information associated */
					  /* with the file can be written     */
#define SYNCHRONIZE           0x00100000  /* The file handle can waited on to */
					  /* synchronize with the completion  */
					  /* of an input/output request       */
#define SYSTEM_SECURITY       0x01000000  /* The system access control list   */
					  /* can be read and changed          */
#define GENERIC_ALL           0x10000000
#define GENERIC_EXECUTE       0x20000000
#define GENERIC_WRITE         0x40000000
#define GENERIC_READ          0x80000000
					 /* In summary - Relevant file       */
					 /* access flags from CIFS are       */
					 /* file_read_data, file_write_data  */
					 /* file_execute, file_read_attributes*/
					 /* write_dac, and delete.           */

#define FILE_READ_RIGHTS (FILE_READ_DATA | FILE_READ_EA | FILE_READ_ATTRIBUTES)
#define FILE_WRITE_RIGHTS (FILE_WRITE_DATA | FILE_APPEND_DATA \
				| FILE_WRITE_EA | FILE_WRITE_ATTRIBUTES)
#define FILE_EXEC_RIGHTS (FILE_EXECUTE)

#define SET_FILE_READ_RIGHTS (FILE_READ_DATA | FILE_READ_EA | FILE_WRITE_EA \
				| FILE_READ_ATTRIBUTES \
				| FILE_WRITE_ATTRIBUTES \
				| DELETE | READ_CONTROL | WRITE_DAC \
				| WRITE_OWNER | SYNCHRONIZE)
#define SET_FILE_WRITE_RIGHTS (FILE_WRITE_DATA | FILE_APPEND_DATA \
				| FILE_READ_EA | FILE_WRITE_EA \
				| FILE_READ_ATTRIBUTES \
				| FILE_WRITE_ATTRIBUTES \
				| DELETE | READ_CONTROL | WRITE_DAC \
				| WRITE_OWNER | SYNCHRONIZE)
#define SET_FILE_EXEC_RIGHTS (FILE_READ_EA | FILE_WRITE_EA | FILE_EXECUTE \
				| FILE_READ_ATTRIBUTES \
				| FILE_WRITE_ATTRIBUTES \
				| DELETE | READ_CONTROL | WRITE_DAC \
				| WRITE_OWNER | SYNCHRONIZE)

#define SET_MINIMUM_RIGHTS (FILE_READ_EA | FILE_READ_ATTRIBUTES \
				| READ_CONTROL | SYNCHRONIZE)


/*
 * Invalid readdir handle
 */
#define CIFS_NO_HANDLE        0xFFFF

#define NO_CHANGE_64          0xFFFFFFFFFFFFFFFFULL

/* IPC$ in ASCII */
#define CIFS_IPC_RESOURCE "\x49\x50\x43\x24"

/* IPC$ in Unicode */
#define CIFS_IPC_UNICODE_RESOURCE "\x00\x49\x00\x50\x00\x43\x00\x24\x00\x00"

/* Unicode Null terminate 2 bytes of 0 */
#define UNICODE_NULL "\x00\x00"
#define ASCII_NULL 0x00

/*
 * Server type values (returned on EnumServer API
 */
#define CIFS_SV_TYPE_DC     0x00000008
#define CIFS_SV_TYPE_BACKDC 0x00000010

/*
 * Alias type flags (From EnumAlias API call
 */
#define CIFS_ALIAS_TYPE_FILE 0x0001
#define CIFS_SHARE_TYPE_FILE 0x0000

/*
 * File Attribute flags
 */
#define ATTR_READONLY  0x0001
#define ATTR_HIDDEN    0x0002
#define ATTR_SYSTEM    0x0004
#define ATTR_VOLUME    0x0008
#define ATTR_DIRECTORY 0x0010
#define ATTR_ARCHIVE   0x0020
#define ATTR_DEVICE    0x0040
#define ATTR_NORMAL    0x0080
#define ATTR_TEMPORARY 0x0100
#define ATTR_SPARSE    0x0200
#define ATTR_REPARSE   0x0400
#define ATTR_COMPRESSED 0x0800
#define ATTR_OFFLINE    0x1000	/* ie file not immediately available -
					on offline storage */
#define ATTR_NOT_CONTENT_INDEXED 0x2000
#define ATTR_ENCRYPTED  0x4000
#define ATTR_POSIX_SEMANTICS 0x01000000
#define ATTR_BACKUP_SEMANTICS 0x02000000
#define ATTR_DELETE_ON_CLOSE 0x04000000
#define ATTR_SEQUENTIAL_SCAN 0x08000000
#define ATTR_RANDOM_ACCESS   0x10000000
#define ATTR_NO_BUFFERING    0x20000000
#define ATTR_WRITE_THROUGH   0x80000000

/* ShareAccess flags */
#define FILE_NO_SHARE     0x00000000
#define FILE_SHARE_READ   0x00000001
#define FILE_SHARE_WRITE  0x00000002
#define FILE_SHARE_DELETE 0x00000004
#define FILE_SHARE_ALL    0x00000007

/* CreateDisposition flags, similar to CreateAction as well */
#define FILE_SUPERSEDE    0x00000000
#define FILE_OPEN         0x00000001
#define FILE_CREATE       0x00000002
#define FILE_OPEN_IF      0x00000003
#define FILE_OVERWRITE    0x00000004
#define FILE_OVERWRITE_IF 0x00000005

/* CreateOptions */
#define CREATE_NOT_FILE		0x00000001	/* if set must not be file */
#define CREATE_WRITE_THROUGH	0x00000002
#define CREATE_SEQUENTIAL       0x00000004
#define CREATE_NO_BUFFER        0x00000008      /* should not buffer on srv */
#define CREATE_SYNC_ALERT       0x00000010	/* MBZ */
#define CREATE_ASYNC_ALERT      0x00000020	/* MBZ */
#define CREATE_NOT_DIR		0x00000040    /* if set must not be directory */
#define CREATE_TREE_CONNECTION  0x00000080	/* should be zero */
#define CREATE_COMPLETE_IF_OPLK 0x00000100	/* should be zero */
#define CREATE_NO_EA_KNOWLEDGE  0x00000200
#define CREATE_EIGHT_DOT_THREE  0x00000400	/* doc says this is obsolete
						 "open for recovery" flag should
						 be zero in any case */
#define CREATE_OPEN_FOR_RECOVERY 0x00000400
#define CREATE_RANDOM_ACCESS	0x00000800
#define CREATE_DELETE_ON_CLOSE	0x00001000
#define CREATE_OPEN_BY_ID       0x00002000
#define CREATE_OPEN_BACKUP_INTENT 0x00004000
#define CREATE_NO_COMPRESSION   0x00008000
#define CREATE_RESERVE_OPFILTER 0x00100000	/* should be zero */
#define OPEN_REPARSE_POINT	0x00200000
#define OPEN_NO_RECALL          0x00400000
#define OPEN_FREE_SPACE_QUERY   0x00800000	/* should be zero */
#define CREATE_OPTIONS_MASK     0x007FFFFF
#define CREATE_OPTION_READONLY	0x10000000
#define CREATE_OPTION_SPECIAL   0x20000000   /* system. NB not sent over wire */

/* ImpersonationLevel flags */
#define SECURITY_ANONYMOUS      0
#define SECURITY_IDENTIFICATION 1
#define SECURITY_IMPERSONATION  2
#define SECURITY_DELEGATION     3

/* SecurityFlags */
#define SECURITY_CONTEXT_TRACKING 0x01
#define SECURITY_EFFECTIVE_ONLY   0x02

/*
 * Default PID value, used in all SMBs where the PID is not important
 */
#define CIFS_DFT_PID  0x1234

/*
 * We use the same routine for Copy and Move SMBs.  This flag is used to
 * distinguish
 */
#define CIFS_COPY_OP 1
#define CIFS_RENAME_OP 2

#define GETU16(var)  (*((__u16 *)var))	/* BB check for endian issues */
#define GETU32(var)  (*((__u32 *)var))	/* BB check for endian issues */

struct smb_hdr {
	__be32 smb_buf_length;	/* BB length is only two (rarely three) bytes,
		with one or two byte "type" preceding it that will be
		zero - we could mask the type byte off */
	__u8 Protocol[4];
	__u8 Command;
	union {
		struct {
			__u8 ErrorClass;
			__u8 Reserved;
			__le16 Error;
		} __attribute__((packed)) DosError;
		__le32 CifsError;
	} __attribute__((packed)) Status;
	__u8 Flags;
	__le16 Flags2;		/* note: le */
	__le16 PidHigh;
	union {
		struct {
			__le32 SequenceNumber;  /* le */
			__u32 Reserved; /* zero */
		} __attribute__((packed)) Sequence;
		__u8 SecuritySignature[8];	/* le */
	} __attribute__((packed)) Signature;
	__u8 pad[2];
	__u16 Tid;
	__le16 Pid;
	__u16 Uid;
	__le16 Mid;
	__u8 WordCount;
} __attribute__((packed));

/* given a pointer to an smb_hdr, retrieve a void pointer to the ByteCount */
static inline void *
BCC(struct smb_hdr *smb)
{
	return (void *)smb + sizeof(*smb) + 2 * smb->WordCount;
}

/* given a pointer to an smb_hdr retrieve the pointer to the byte area */
#define pByteArea(smb_var) (BCC(smb_var) + 2)

/* get the unconverted ByteCount for a SMB packet and return it */
static inline __u16
get_bcc(struct smb_hdr *hdr)
{
	__le16 *bc_ptr = (__le16 *)BCC(hdr);

	return get_unaligned_le16(bc_ptr);
}

/* set the ByteCount for a SMB packet in little-endian */
static inline void
put_bcc(__u16 count, struct smb_hdr *hdr)
{
	__le16 *bc_ptr = (__le16 *)BCC(hdr);

	put_unaligned_le16(count, bc_ptr);
}

/*
 * Computer Name Length (since Netbios name was length 16 with last byte 0x20)
 * No longer as important, now that TCP names are more commonly used to
 * resolve hosts.
 */
#define CNLEN 15

/*
 * Share Name Length (SNLEN)
 * Note:  This length was limited by the SMB used to get
 *        the Share info.   NetShareEnum only returned 13
 *        chars, including the null termination.
 * This was removed because it no longer is limiting.
 */

/*
 * Comment Length
 */
#define MAXCOMMENTLEN 40

/*
 * The OS/2 maximum path name
 */
#define MAX_PATHCONF 256

/*
 *  SMB frame definitions  (following must be packed structs)
 *  See the SNIA CIFS Specification for details.
 *
 *  The Naming convention is the lower case version of the
 *  smb command code name for the struct and this is typedef to the
 *  uppercase version of the same name with the prefix SMB_ removed
 *  for brevity.  Although typedefs are not commonly used for
 *  structure definitions in the Linux kernel, their use in the
 *  CIFS standards document, which this code is based on, may
 *  make this one of the cases where typedefs for structures make
 *  sense to improve readability for readers of the standards doc.
 *  Typedefs can always be removed later if they are too distracting
 *  and they are only used for the CIFSs PDUs themselves, not
 *  internal cifs vfs structures
 *
 */

typedef struct negotiate_req {
	struct smb_hdr hdr;	/* wct = 0 */
	__le16 ByteCount;
	unsigned char DialectsArray[1];
} __attribute__((packed)) NEGOTIATE_REQ;

/* Dialect index is 13 for LANMAN */

#define MIN_TZ_ADJ (15 * 60) /* minimum grid for timezones in seconds */

typedef struct lanman_neg_rsp {
	struct smb_hdr hdr;	/* wct = 13 */
	__le16 DialectIndex;
	__le16 SecurityMode;
	__le16 MaxBufSize;
	__le16 MaxMpxCount;
	__le16 MaxNumberVcs;
	__le16 RawMode;
	__le32 SessionKey;
	struct {
		__le16 Time;
		__le16 Date;
	} __attribute__((packed)) SrvTime;
	__le16 ServerTimeZone;
	__le16 EncryptionKeyLength;
	__le16 Reserved;
	__u16  ByteCount;
	unsigned char EncryptionKey[1];
} __attribute__((packed)) LANMAN_NEG_RSP;

#define READ_RAW_ENABLE 1
#define WRITE_RAW_ENABLE 2
#define RAW_ENABLE (READ_RAW_ENABLE | WRITE_RAW_ENABLE)
#define SMB1_CLIENT_GUID_SIZE (16)
typedef struct negotiate_rsp {
	struct smb_hdr hdr;	/* wct = 17 */
	__le16 DialectIndex; /* 0xFFFF = no dialect acceptable */
	__u8 SecurityMode;
	__le16 MaxMpxCount;
	__le16 MaxNumberVcs;
	__le32 MaxBufferSize;
	__le32 MaxRawSize;
	__le32 SessionKey;
	__le32 Capabilities;	/* see below */
	__le32 SystemTimeLow;
	__le32 SystemTimeHigh;
	__le16 ServerTimeZone;
	__u8 EncryptionKeyLength;
	__u16 ByteCount;
	union {
		unsigned char EncryptionKey[1];	/* cap extended security off */
		/* followed by Domain name - if extended security is off */
		/* followed by 16 bytes of server GUID */
		/* then security blob if cap_extended_security negotiated */
		struct {
			unsigned char GUID[SMB1_CLIENT_GUID_SIZE];
			unsigned char SecurityBlob[1];
		} __attribute__((packed)) extended_response;
	} __attribute__((packed)) u;
} __attribute__((packed)) NEGOTIATE_RSP;

/* SecurityMode bits */
#define SECMODE_USER          0x01	/* off indicates share level security */
#define SECMODE_PW_ENCRYPT    0x02
#define SECMODE_SIGN_ENABLED  0x04	/* SMB security signatures enabled */
#define SECMODE_SIGN_REQUIRED 0x08	/* SMB security signatures required */

/* Negotiate response Capabilities */
#define CAP_RAW_MODE           0x00000001
#define CAP_MPX_MODE           0x00000002
#define CAP_UNICODE            0x00000004
#define CAP_LARGE_FILES        0x00000008
#define CAP_NT_SMBS            0x00000010	/* implies CAP_NT_FIND */
#define CAP_RPC_REMOTE_APIS    0x00000020
#define CAP_STATUS32           0x00000040
#define CAP_LEVEL_II_OPLOCKS   0x00000080
#define CAP_LOCK_AND_READ      0x00000100
#define CAP_NT_FIND            0x00000200
#define CAP_DFS                0x00001000
#define CAP_INFOLEVEL_PASSTHRU 0x00002000
#define CAP_LARGE_READ_X       0x00004000
#define CAP_LARGE_WRITE_X      0x00008000
#define CAP_LWIO               0x00010000 /* support fctl_srv_req_resume_key */
#define CAP_UNIX               0x00800000
#define CAP_COMPRESSED_DATA    0x02000000
#define CAP_DYNAMIC_REAUTH     0x20000000
#define CAP_PERSISTENT_HANDLES 0x40000000
#define CAP_EXTENDED_SECURITY  0x80000000

typedef union smb_com_session_setup_andx {
	struct {		/* request format */
		struct smb_hdr hdr;	/* wct = 12 */
		__u8 AndXCommand;
		__u8 AndXReserved;
		__le16 AndXOffset;
		__le16 MaxBufferSize;
		__le16 MaxMpxCount;
		__le16 VcNumber;
		__u32 SessionKey;
		__le16 SecurityBlobLength;
		__u32 Reserved;
		__le32 Capabilities;	/* see below */
		__le16 ByteCount;
		unsigned char SecurityBlob[1];	/* followed by */
		/* STRING NativeOS */
		/* STRING NativeLanMan */
	} __attribute__((packed)) req;	/* NTLM request format (with
					extended security */

	struct {		/* request format */
		struct smb_hdr hdr;	/* wct = 13 */
		__u8 AndXCommand;
		__u8 AndXReserved;
		__le16 AndXOffset;
		__le16 MaxBufferSize;
		__le16 MaxMpxCount;
		__le16 VcNumber;
		__u32 SessionKey;
		__le16 CaseInsensitivePasswordLength; /* ASCII password len */
		__le16 CaseSensitivePasswordLength; /* Unicode password length*/
		__u32 Reserved;	/* see below */
		__le32 Capabilities;
		__le16 ByteCount;
		unsigned char CaseInsensitivePassword[1];     /* followed by: */
		/* unsigned char * CaseSensitivePassword; */
		/* STRING AccountName */
		/* STRING PrimaryDomain */
		/* STRING NativeOS */
		/* STRING NativeLanMan */
	} __attribute__((packed)) req_no_secext; /* NTLM request format (without
							extended security */

	struct {		/* default (NTLM) response format */
		struct smb_hdr hdr;	/* wct = 4 */
		__u8 AndXCommand;
		__u8 AndXReserved;
		__le16 AndXOffset;
		__le16 Action;	/* see below */
		__le16 SecurityBlobLength;
		__u16 ByteCount;
		unsigned char SecurityBlob[1];	/* followed by */
/*      unsigned char  * NativeOS;      */
/*	unsigned char  * NativeLanMan;  */
/*      unsigned char  * PrimaryDomain; */
	} __attribute__((packed)) resp;	/* NTLM response
					   (with or without extended sec) */

	struct {		/* request format */
		struct smb_hdr hdr;	/* wct = 10 */
		__u8 AndXCommand;
		__u8 AndXReserved;
		__le16 AndXOffset;
		__le16 MaxBufferSize;
		__le16 MaxMpxCount;
		__le16 VcNumber;
		__u32 SessionKey;
		__le16 PasswordLength;
		__u32 Reserved; /* encrypt key len and offset */
		__le16 ByteCount;
		unsigned char AccountPassword[1];	/* followed by */
		/* STRING AccountName */
		/* STRING PrimaryDomain */
		/* STRING NativeOS */
		/* STRING NativeLanMan */
	} __attribute__((packed)) old_req; /* pre-NTLM (LANMAN2.1) req format */

	struct {		/* default (NTLM) response format */
		struct smb_hdr hdr;	/* wct = 3 */
		__u8 AndXCommand;
		__u8 AndXReserved;
		__le16 AndXOffset;
		__le16 Action;	/* see below */
		__u16 ByteCount;
		unsigned char NativeOS[1];	/* followed by */
/*	unsigned char * NativeLanMan; */
/*      unsigned char * PrimaryDomain; */
	} __attribute__((packed)) old_resp; /* pre-NTLM (LANMAN2.1) response */
} __attribute__((packed)) SESSION_SETUP_ANDX;

/* format of NLTMv2 Response ie "case sensitive password" hash when NTLMv2 */

#define NTLMSSP_SERVER_TYPE	1
#define NTLMSSP_DOMAIN_TYPE	2
#define NTLMSSP_FQ_DOMAIN_TYPE	3
#define NTLMSSP_DNS_DOMAIN_TYPE	4
#define NTLMSSP_DNS_PARENT_TYPE	5

struct ntlmssp2_name {
	__le16 type;
	__le16 length;
/*	char   name[length]; */
} __attribute__((packed));

struct ntlmv2_resp {
	union {
	    char ntlmv2_hash[CIFS_ENCPWD_SIZE];
	    struct {
		__u8 reserved[8];
		__u8 key[CIFS_SERVER_CHALLENGE_SIZE];
	    } __attribute__((packed)) challenge;
	} __attribute__((packed));
	__le32 blob_signature;
	__u32  reserved;
	__le64  time;
	__u64  client_chal; /* random */
	__u32  reserved2;
	/* array of name entries could follow ending in minimum 4 byte struct */
} __attribute__((packed));


#define CIFS_NETWORK_OPSYS "CIFS VFS Client for Linux"

/* Capabilities bits (for NTLM SessSetup request) */
#define CAP_UNICODE            0x00000004
#define CAP_LARGE_FILES        0x00000008
#define CAP_NT_SMBS            0x00000010
#define CAP_STATUS32           0x00000040
#define CAP_LEVEL_II_OPLOCKS   0x00000080
#define CAP_NT_FIND            0x00000200	/* reserved should be zero
				(because NT_SMBs implies the same thing?) */
#define CAP_BULK_TRANSFER      0x20000000
#define CAP_EXTENDED_SECURITY  0x80000000

/* Action bits */
#define GUEST_LOGIN 1

typedef struct smb_com_tconx_req {
	struct smb_hdr hdr;	/* wct = 4 */
	__u8 AndXCommand;
	__u8 AndXReserved;
	__le16 AndXOffset;
	__le16 Flags;		/* see below */
	__le16 PasswordLength;
	__le16 ByteCount;
	unsigned char Password[1];	/* followed by */
/* STRING Path    *//* \\server\share name */
	/* STRING Service */
} __attribute__((packed)) TCONX_REQ;

typedef struct smb_com_tconx_rsp {
	struct smb_hdr hdr;	/* wct = 3 , not extended response */
	__u8 AndXCommand;
	__u8 AndXReserved;
	__le16 AndXOffset;
	__le16 OptionalSupport;	/* see below */
	__u16 ByteCount;
	unsigned char Service[1];	/* always ASCII, not Unicode */
	/* STRING NativeFileSystem */
} __attribute__((packed)) TCONX_RSP;

typedef struct smb_com_tconx_rsp_ext {
	struct smb_hdr hdr;	/* wct = 7, extended response */
	__u8 AndXCommand;
	__u8 AndXReserved;
	__le16 AndXOffset;
	__le16 OptionalSupport;	/* see below */
	__le32 MaximalShareAccessRights;
	__le32 GuestMaximalShareAccessRights;
	__u16 ByteCount;
	unsigned char Service[1];	/* always ASCII, not Unicode */
	/* STRING NativeFileSystem */
} __attribute__((packed)) TCONX_RSP_EXT;


/* tree connect Flags */
#define DISCONNECT_TID          0x0001
#define TCON_EXTENDED_SIGNATURES 0x0004
#define TCON_EXTENDED_SECINFO   0x0008

/* OptionalSupport bits */
#define SMB_SUPPORT_SEARCH_BITS 0x0001	/* "must have" directory search bits
					 (exclusive searches supported) */
#define SMB_SHARE_IS_IN_DFS     0x0002
#define SMB_CSC_MASK               0x000C
/* CSC flags defined as follows */
#define SMB_CSC_CACHE_MANUAL_REINT 0x0000
#define SMB_CSC_CACHE_AUTO_REINT   0x0004
#define SMB_CSC_CACHE_VDO          0x0008
#define SMB_CSC_NO_CACHING         0x000C
#define SMB_UNIQUE_FILE_NAME    0x0010
#define SMB_EXTENDED_SIGNATURES 0x0020

/* services
 *
 * A:       ie disk
 * LPT1:    ie printer
 * IPC      ie named pipe
 * COMM
 * ?????    ie any type
 *
 */

typedef struct smb_com_echo_req {
	struct	smb_hdr hdr;
	__le16	EchoCount;
	__le16	ByteCount;
	char	Data[1];
} __attribute__((packed)) ECHO_REQ;

typedef struct smb_com_echo_rsp {
	struct	smb_hdr hdr;
	__le16	SequenceNumber;
	__le16	ByteCount;
	char	Data[1];
} __attribute__((packed)) ECHO_RSP;

typedef struct smb_com_logoff_andx_req {
	struct smb_hdr hdr;	/* wct = 2 */
	__u8 AndXCommand;
	__u8 AndXReserved;
	__u16 AndXOffset;
	__u16 ByteCount;
} __attribute__((packed)) LOGOFF_ANDX_REQ;

typedef struct smb_com_logoff_andx_rsp {
	struct smb_hdr hdr;	/* wct = 2 */
	__u8 AndXCommand;
	__u8 AndXReserved;
	__u16 AndXOffset;
	__u16 ByteCount;
} __attribute__((packed)) LOGOFF_ANDX_RSP;

typedef union smb_com_tree_disconnect {	/* as an altetnative can use flag on
					tree_connect PDU to effect disconnect */
					/* tdis is probably simplest SMB PDU */
	struct {
		struct smb_hdr hdr;	/* wct = 0 */
		__u16 ByteCount;	/* bcc = 0 */
	} __attribute__((packed)) req;
	struct {
		struct smb_hdr hdr;	/* wct = 0 */
		__u16 ByteCount;	/* bcc = 0 */
	} __attribute__((packed)) resp;
} __attribute__((packed)) TREE_DISCONNECT;

typedef struct smb_com_close_req {
	struct smb_hdr hdr;	/* wct = 3 */
	__u16 FileID;
	__u32 LastWriteTime;	/* should be zero or -1 */
	__u16 ByteCount;	/* 0 */
} __attribute__((packed)) CLOSE_REQ;

typedef struct smb_com_close_rsp {
	struct smb_hdr hdr;	/* wct = 0 */
	__u16 ByteCount;	/* bct = 0 */
} __attribute__((packed)) CLOSE_RSP;

typedef struct smb_com_flush_req {
	struct smb_hdr hdr;	/* wct = 1 */
	__u16 FileID;
	__u16 ByteCount;	/* 0 */
} __attribute__((packed)) FLUSH_REQ;

typedef struct smb_com_findclose_req {
	struct smb_hdr hdr; /* wct = 1 */
	__u16 FileID;
	__u16 ByteCount;    /* 0 */
} __attribute__((packed)) FINDCLOSE_REQ;

/* OpenFlags */
#define REQ_MORE_INFO      0x00000001  /* legacy (OPEN_AND_X) only */
#define REQ_OPLOCK         0x00000002
#define REQ_BATCHOPLOCK    0x00000004
#define REQ_OPENDIRONLY    0x00000008
#define REQ_EXTENDED_INFO  0x00000010

/* File type */
#define DISK_TYPE		0x0000
#define BYTE_PIPE_TYPE		0x0001
#define MESSAGE_PIPE_TYPE	0x0002
#define PRINTER_TYPE		0x0003
#define COMM_DEV_TYPE		0x0004
#define UNKNOWN_TYPE		0xFFFF

/* Device Type or File Status Flags */
#define NO_EAS			0x0001
#define NO_SUBSTREAMS		0x0002
#define NO_REPARSETAG		0x0004
/* following flags can apply if pipe */
#define ICOUNT_MASK		0x00FF
#define PIPE_READ_MODE		0x0100
#define NAMED_PIPE_TYPE		0x0400
#define PIPE_END_POINT		0x4000
#define BLOCKING_NAMED_PIPE	0x8000

typedef struct smb_com_open_req {	/* also handles create */
	struct smb_hdr hdr;	/* wct = 24 */
	__u8 AndXCommand;
	__u8 AndXReserved;
	__le16 AndXOffset;
	__u8 Reserved;		/* Must Be Zero */
	__le16 NameLength;
	__le32 OpenFlags;
	__u32  RootDirectoryFid;
	__le32 DesiredAccess;
	__le64 AllocationSize;
	__le32 FileAttributes;
	__le32 ShareAccess;
	__le32 CreateDisposition;
	__le32 CreateOptions;
	__le32 ImpersonationLevel;
	__u8 SecurityFlags;
	__le16 ByteCount;
	char fileName[1];
} __attribute__((packed)) OPEN_REQ;

/* open response: oplock levels */
#define OPLOCK_NONE  	 0
#define OPLOCK_EXCLUSIVE 1
#define OPLOCK_BATCH	 2
#define OPLOCK_READ	 3  /* level 2 oplock */

/* open response for CreateAction shifted left */
#define CIFS_CREATE_ACTION 0x20000 /* file created */

typedef struct smb_com_open_rsp {
	struct smb_hdr hdr;	/* wct = 34 BB */
	__u8 AndXCommand;
	__u8 AndXReserved;
	__le16 AndXOffset;
	__u8 OplockLevel;
	__u16 Fid;
	__le32 CreateAction;
	__le64 CreationTime;
	__le64 LastAccessTime;
	__le64 LastWriteTime;
	__le64 ChangeTime;
	__le32 FileAttributes;
	__le64 AllocationSize;
	__le64 EndOfFile;
	__le16 FileType;
	__le16 DeviceState;
	__u8 DirectoryFlag;
	__u16 ByteCount;	/* bct = 0 */
} __attribute__((packed)) OPEN_RSP;

typedef struct smb_com_open_rsp_ext {
	struct smb_hdr hdr;     /* wct = 42 but meaningless due to MS bug? */
	__u8 AndXCommand;
	__u8 AndXReserved;
	__le16 AndXOffset;
	__u8 OplockLevel;
	__u16 Fid;
	__le32 CreateAction;
	__le64 CreationTime;
	__le64 LastAccessTime;
	__le64 LastWriteTime;
	__le64 ChangeTime;
	__le32 FileAttributes;
	__le64 AllocationSize;
	__le64 EndOfFile;
	__le16 FileType;
	__le16 DeviceState;
	__u8 DirectoryFlag;
	__u8 VolumeGUID[16];
	__u64 FileId; /* note no endian conversion - is opaque UniqueID */
	__le32 MaximalAccessRights;
	__le32 GuestMaximalAccessRights;
	__u16 ByteCount;        /* bct = 0 */
} __attribute__((packed)) OPEN_RSP_EXT;


/* format of legacy open request */
typedef struct smb_com_openx_req {
	struct smb_hdr	hdr;	/* wct = 15 */
	__u8 AndXCommand;
	__u8 AndXReserved;
	__le16 AndXOffset;
	__le16 OpenFlags;
	__le16 Mode;
	__le16 Sattr; /* search attributes */
	__le16 FileAttributes;  /* dos attrs */
	__le32 CreateTime; /* os2 format */
	__le16 OpenFunction;
	__le32 EndOfFile;
	__le32 Timeout;
	__le32 Reserved;
	__le16  ByteCount;  /* file name follows */
	char   fileName[1];
} __attribute__((packed)) OPENX_REQ;

typedef struct smb_com_openx_rsp {
	struct smb_hdr	hdr;	/* wct = 15 */
	__u8 AndXCommand;
	__u8 AndXReserved;
	__le16 AndXOffset;
	__u16  Fid;
	__le16 FileAttributes;
	__le32 LastWriteTime; /* os2 format */
	__le32 EndOfFile;
	__le16 Access;
	__le16 FileType;
	__le16 IPCState;
	__le16 Action;
	__u32  FileId;
	__u16  Reserved;
	__u16  ByteCount;
} __attribute__((packed)) OPENX_RSP;

/* For encoding of POSIX Open Request - see trans2 function 0x209 data struct */

/* Legacy write request for older servers */
typedef struct smb_com_writex_req {
	struct smb_hdr hdr;     /* wct = 12 */
	__u8 AndXCommand;
	__u8 AndXReserved;
	__le16 AndXOffset;
	__u16 Fid;
	__le32 OffsetLow;
	__u32 Reserved; /* Timeout */
	__le16 WriteMode; /* 1 = write through */
	__le16 Remaining;
	__le16 Reserved2;
	__le16 DataLengthLow;
	__le16 DataOffset;
	__le16 ByteCount;
	__u8 Pad;		/* BB check for whether padded to DWORD
				   boundary and optimum performance here */
	char Data[];
} __attribute__((packed)) WRITEX_REQ;

typedef struct smb_com_write_req {
	struct smb_hdr hdr;	/* wct = 14 */
	__u8 AndXCommand;
	__u8 AndXReserved;
	__le16 AndXOffset;
	__u16 Fid;
	__le32 OffsetLow;
	__u32 Reserved;
	__le16 WriteMode;
	__le16 Remaining;
	__le16 DataLengthHigh;
	__le16 DataLengthLow;
	__le16 DataOffset;
	__le32 OffsetHigh;
	__le16 ByteCount;
	__u8 Pad;		/* BB check for whether padded to DWORD
				   boundary and optimum performance here */
	char Data[];
} __attribute__((packed)) WRITE_REQ;

typedef struct smb_com_write_rsp {
	struct smb_hdr hdr;	/* wct = 6 */
	__u8 AndXCommand;
	__u8 AndXReserved;
	__le16 AndXOffset;
	__le16 Count;
	__le16 Remaining;
	__le16 CountHigh;
	__u16  Reserved;
	__u16 ByteCount;
} __attribute__((packed)) WRITE_RSP;

/* legacy read request for older servers */
typedef struct smb_com_readx_req {
	struct smb_hdr hdr;	/* wct = 10 */
	__u8 AndXCommand;
	__u8 AndXReserved;
	__le16 AndXOffset;
	__u16 Fid;
	__le32 OffsetLow;
	__le16 MaxCount;
	__le16 MinCount;	/* obsolete */
	__le32 Reserved;
	__le16 Remaining;
	__le16 ByteCount;
} __attribute__((packed)) READX_REQ;

typedef struct smb_com_read_req {
	struct smb_hdr hdr;	/* wct = 12 */
	__u8 AndXCommand;
	__u8 AndXReserved;
	__le16 AndXOffset;
	__u16 Fid;
	__le32 OffsetLow;
	__le16 MaxCount;
	__le16 MinCount;		/* obsolete */
	__le32 MaxCountHigh;
	__le16 Remaining;
	__le32 OffsetHigh;
	__le16 ByteCount;
} __attribute__((packed)) READ_REQ;

typedef struct smb_com_read_rsp {
	struct smb_hdr hdr;	/* wct = 12 */
	__u8 AndXCommand;
	__u8 AndXReserved;
	__le16 AndXOffset;
	__le16 Remaining;
	__le16 DataCompactionMode;
	__le16 Reserved;
	__le16 DataLength;
	__le16 DataOffset;
	__le16 DataLengthHigh;
	__u64 Reserved2;
	__u16 ByteCount;
	/* read response data immediately follows */
} __attribute__((packed)) READ_RSP;

typedef struct locking_andx_range {
	__le16 Pid;
	__le16 Pad;
	__le32 OffsetHigh;
	__le32 OffsetLow;
	__le32 LengthHigh;
	__le32 LengthLow;
} __attribute__((packed)) LOCKING_ANDX_RANGE;

#define LOCKING_ANDX_SHARED_LOCK     0x01
#define LOCKING_ANDX_OPLOCK_RELEASE  0x02
#define LOCKING_ANDX_CHANGE_LOCKTYPE 0x04
#define LOCKING_ANDX_CANCEL_LOCK     0x08
#define LOCKING_ANDX_LARGE_FILES     0x10	/* always on for us */

typedef struct smb_com_lock_req {
	struct smb_hdr hdr;	/* wct = 8 */
	__u8 AndXCommand;
	__u8 AndXReserved;
	__le16 AndXOffset;
	__u16 Fid;
	__u8 LockType;
	__u8 OplockLevel;
	__le32 Timeout;
	__le16 NumberOfUnlocks;
	__le16 NumberOfLocks;
	__le16 ByteCount;
	LOCKING_ANDX_RANGE Locks[1];
} __attribute__((packed)) LOCK_REQ;

/* lock type */
#define CIFS_RDLCK	0
#define CIFS_WRLCK	1
#define CIFS_UNLCK      2
typedef struct cifs_posix_lock {
	__le16  lock_type;  /* 0 = Read, 1 = Write, 2 = Unlock */
	__le16  lock_flags; /* 1 = Wait (only valid for setlock) */
	__le32  pid;
	__le64	start;
	__le64	length;
	/* BB what about additional owner info to identify network client */
} __attribute__((packed)) CIFS_POSIX_LOCK;

typedef struct smb_com_lock_rsp {
	struct smb_hdr hdr;	/* wct = 2 */
	__u8 AndXCommand;
	__u8 AndXReserved;
	__le16 AndXOffset;
	__u16 ByteCount;
} __attribute__((packed)) LOCK_RSP;

typedef struct smb_com_rename_req {
	struct smb_hdr hdr;	/* wct = 1 */
	__le16 SearchAttributes;	/* target file attributes */
	__le16 ByteCount;
	__u8 BufferFormat;	/* 4 = ASCII or Unicode */
	unsigned char OldFileName[1];
	/* followed by __u8 BufferFormat2 */
	/* followed by NewFileName */
} __attribute__((packed)) RENAME_REQ;

	/* copy request flags */
#define COPY_MUST_BE_FILE      0x0001
#define COPY_MUST_BE_DIR       0x0002
#define COPY_TARGET_MODE_ASCII 0x0004 /* if not set, binary */
#define COPY_SOURCE_MODE_ASCII 0x0008 /* if not set, binary */
#define COPY_VERIFY_WRITES     0x0010
#define COPY_TREE              0x0020

typedef struct smb_com_copy_req {
	struct smb_hdr hdr;	/* wct = 3 */
	__u16 Tid2;
	__le16 OpenFunction;
	__le16 Flags;
	__le16 ByteCount;
	__u8 BufferFormat;	/* 4 = ASCII or Unicode */
	unsigned char OldFileName[1];
	/* followed by __u8 BufferFormat2 */
	/* followed by NewFileName string */
} __attribute__((packed)) COPY_REQ;

typedef struct smb_com_copy_rsp {
	struct smb_hdr hdr;     /* wct = 1 */
	__le16 CopyCount;    /* number of files copied */
	__u16 ByteCount;    /* may be zero */
	__u8 BufferFormat;  /* 0x04 - only present if errored file follows */
	unsigned char ErrorFileName[1]; /* only present if error in copy */
} __attribute__((packed)) COPY_RSP;

#define CREATE_HARD_LINK		0x103
#define MOVEFILE_COPY_ALLOWED		0x0002
#define MOVEFILE_REPLACE_EXISTING	0x0001

typedef struct smb_com_nt_rename_req {	/* A5 - also used for create hardlink */
	struct smb_hdr hdr;	/* wct = 4 */
	__le16 SearchAttributes;	/* target file attributes */
	__le16 Flags;		/* spec says Information Level */
	__le32 ClusterCount;
	__le16 ByteCount;
	__u8 BufferFormat;	/* 4 = ASCII or Unicode */
	unsigned char OldFileName[1];
	/* followed by __u8 BufferFormat2 */
	/* followed by NewFileName */
} __attribute__((packed)) NT_RENAME_REQ;

typedef struct smb_com_rename_rsp {
	struct smb_hdr hdr;	/* wct = 0 */
	__u16 ByteCount;	/* bct = 0 */
} __attribute__((packed)) RENAME_RSP;

typedef struct smb_com_delete_file_req {
	struct smb_hdr hdr;	/* wct = 1 */
	__le16 SearchAttributes;
	__le16 ByteCount;
	__u8 BufferFormat;	/* 4 = ASCII */
	unsigned char fileName[1];
} __attribute__((packed)) DELETE_FILE_REQ;

typedef struct smb_com_delete_file_rsp {
	struct smb_hdr hdr;	/* wct = 0 */
	__u16 ByteCount;	/* bct = 0 */
} __attribute__((packed)) DELETE_FILE_RSP;

typedef struct smb_com_delete_directory_req {
	struct smb_hdr hdr;	/* wct = 0 */
	__le16 ByteCount;
	__u8 BufferFormat;	/* 4 = ASCII */
	unsigned char DirName[1];
} __attribute__((packed)) DELETE_DIRECTORY_REQ;

typedef struct smb_com_delete_directory_rsp {
	struct smb_hdr hdr;	/* wct = 0 */
	__u16 ByteCount;	/* bct = 0 */
} __attribute__((packed)) DELETE_DIRECTORY_RSP;

typedef struct smb_com_create_directory_req {
	struct smb_hdr hdr;	/* wct = 0 */
	__le16 ByteCount;
	__u8 BufferFormat;	/* 4 = ASCII */
	unsigned char DirName[1];
} __attribute__((packed)) CREATE_DIRECTORY_REQ;

typedef struct smb_com_create_directory_rsp {
	struct smb_hdr hdr;	/* wct = 0 */
	__u16 ByteCount;	/* bct = 0 */
} __attribute__((packed)) CREATE_DIRECTORY_RSP;

typedef struct smb_com_query_information_req {
	struct smb_hdr hdr;     /* wct = 0 */
	__le16 ByteCount;	/* 1 + namelen + 1 */
	__u8 BufferFormat;      /* 4 = ASCII */
	unsigned char FileName[1];
} __attribute__((packed)) QUERY_INFORMATION_REQ;

typedef struct smb_com_query_information_rsp {
	struct smb_hdr hdr;     /* wct = 10 */
	__le16 attr;
	__le32  last_write_time;
	__le32 size;
	__u16  reserved[5];
	__le16 ByteCount;	/* bcc = 0 */
} __attribute__((packed)) QUERY_INFORMATION_RSP;

typedef struct smb_com_setattr_req {
	struct smb_hdr hdr; /* wct = 8 */
	__le16 attr;
	__le16 time_low;
	__le16 time_high;
	__le16 reserved[5]; /* must be zero */
	__u16  ByteCount;
	__u8   BufferFormat; /* 4 = ASCII */
	unsigned char fileName[1];
} __attribute__((packed)) SETATTR_REQ;

typedef struct smb_com_setattr_rsp {
	struct smb_hdr hdr;     /* wct = 0 */
	__u16 ByteCount;        /* bct = 0 */
} __attribute__((packed)) SETATTR_RSP;

/* empty wct response to setattr */

/*******************************************************/
/* NT Transact structure definitions follow            */
/* Currently only ioctl, acl (get security descriptor) */
/* and notify are implemented                          */
/*******************************************************/
typedef struct smb_com_ntransact_req {
	struct smb_hdr hdr; /* wct >= 19 */
	__u8 MaxSetupCount;
	__u16 Reserved;
	__le32 TotalParameterCount;
	__le32 TotalDataCount;
	__le32 MaxParameterCount;
	__le32 MaxDataCount;
	__le32 ParameterCount;
	__le32 ParameterOffset;
	__le32 DataCount;
	__le32 DataOffset;
	__u8 SetupCount; /* four setup words follow subcommand */
	/* SNIA spec incorrectly included spurious pad here */
	__le16 SubCommand; /* 2 = IOCTL/FSCTL */
	/* SetupCount words follow then */
	__le16 ByteCount;
	__u8 Pad[3];
	__u8 Parms[];
} __attribute__((packed)) NTRANSACT_REQ;

typedef struct smb_com_ntransact_rsp {
	struct smb_hdr hdr;     /* wct = 18 */
	__u8 Reserved[3];
	__le32 TotalParameterCount;
	__le32 TotalDataCount;
	__le32 ParameterCount;
	__le32 ParameterOffset;
	__le32 ParameterDisplacement;
	__le32 DataCount;
	__le32 DataOffset;
	__le32 DataDisplacement;
	__u8 SetupCount;   /* 0 */
	__u16 ByteCount;
	/* __u8 Pad[3]; */
	/* parms and data follow */
} __attribute__((packed)) NTRANSACT_RSP;

/* See MS-SMB 2.2.7.2.1.1 */
struct srv_copychunk {
	__le64 SourceOffset;
	__le64 DestinationOffset;
	__le32 CopyLength;
	__u32  Reserved;
} __packed;

typedef struct smb_com_transaction_ioctl_req {
	struct smb_hdr hdr;	/* wct = 23 */
	__u8 MaxSetupCount;
	__u16 Reserved;
	__le32 TotalParameterCount;
	__le32 TotalDataCount;
	__le32 MaxParameterCount;
	__le32 MaxDataCount;
	__le32 ParameterCount;
	__le32 ParameterOffset;
	__le32 DataCount;
	__le32 DataOffset;
	__u8 SetupCount; /* four setup words follow subcommand */
	/* SNIA spec incorrectly included spurious pad here */
	__le16 SubCommand; /* 2 = IOCTL/FSCTL */
	__le32 FunctionCode;
	__u16 Fid;
	__u8 IsFsctl;  /* 1 = File System Control 0 = device control (IOCTL) */
	__u8 IsRootFlag; /* 1 = apply command to root of share (must be DFS) */
	__le16 ByteCount;
	__u8 Pad[3];
	__u8 Data[1];
} __attribute__((packed)) TRANSACT_IOCTL_REQ;

typedef struct smb_com_transaction_compr_ioctl_req {
	struct smb_hdr hdr;	/* wct = 23 */
	__u8 MaxSetupCount;
	__u16 Reserved;
	__le32 TotalParameterCount;
	__le32 TotalDataCount;
	__le32 MaxParameterCount;
	__le32 MaxDataCount;
	__le32 ParameterCount;
	__le32 ParameterOffset;
	__le32 DataCount;
	__le32 DataOffset;
	__u8 SetupCount; /* four setup words follow subcommand */
	/* SNIA spec incorrectly included spurious pad here */
	__le16 SubCommand; /* 2 = IOCTL/FSCTL */
	__le32 FunctionCode;
	__u16 Fid;
	__u8 IsFsctl;  /* 1 = File System Control 0 = device control (IOCTL) */
	__u8 IsRootFlag; /* 1 = apply command to root of share (must be DFS) */
	__le16 ByteCount;
	__u8 Pad[3];
	__le16 compression_state;  /* See below for valid flags */
} __attribute__((packed)) TRANSACT_COMPR_IOCTL_REQ;

/* compression state flags */
#define COMPRESSION_FORMAT_NONE		0x0000
#define COMPRESSION_FORMAT_DEFAULT	0x0001
#define COMPRESSION_FORMAT_LZNT1	0x0002

typedef struct smb_com_transaction_ioctl_rsp {
	struct smb_hdr hdr;	/* wct = 19 */
	__u8 Reserved[3];
	__le32 TotalParameterCount;
	__le32 TotalDataCount;
	__le32 ParameterCount;
	__le32 ParameterOffset;
	__le32 ParameterDisplacement;
	__le32 DataCount;
	__le32 DataOffset;
	__le32 DataDisplacement;
	__u8 SetupCount;	/* 1 */
	__le16 ReturnedDataLen;
	__u16 ByteCount;
} __attribute__((packed)) TRANSACT_IOCTL_RSP;

#define CIFS_ACL_OWNER 1
#define CIFS_ACL_GROUP 2
#define CIFS_ACL_DACL  4
#define CIFS_ACL_SACL  8

typedef struct smb_com_transaction_qsec_req {
	struct smb_hdr hdr;     /* wct = 19 */
	__u8 MaxSetupCount;
	__u16 Reserved;
	__le32 TotalParameterCount;
	__le32 TotalDataCount;
	__le32 MaxParameterCount;
	__le32 MaxDataCount;
	__le32 ParameterCount;
	__le32 ParameterOffset;
	__le32 DataCount;
	__le32 DataOffset;
	__u8 SetupCount; /* no setup words follow subcommand */
	/* SNIA spec incorrectly included spurious pad here */
	__le16 SubCommand; /* 6 = QUERY_SECURITY_DESC */
	__le16 ByteCount; /* bcc = 3 + 8 */
	__u8 Pad[3];
	__u16 Fid;
	__u16 Reserved2;
	__le32 AclFlags;
} __attribute__((packed)) QUERY_SEC_DESC_REQ;


typedef struct smb_com_transaction_ssec_req {
	struct smb_hdr hdr;     /* wct = 19 */
	__u8 MaxSetupCount;
	__u16 Reserved;
	__le32 TotalParameterCount;
	__le32 TotalDataCount;
	__le32 MaxParameterCount;
	__le32 MaxDataCount;
	__le32 ParameterCount;
	__le32 ParameterOffset;
	__le32 DataCount;
	__le32 DataOffset;
	__u8 SetupCount; /* no setup words follow subcommand */
	/* SNIA spec incorrectly included spurious pad here */
	__le16 SubCommand; /* 3 = SET_SECURITY_DESC */
	__le16 ByteCount; /* bcc = 3 + 8 */
	__u8 Pad[3];
	__u16 Fid;
	__u16 Reserved2;
	__le32 AclFlags;
} __attribute__((packed)) SET_SEC_DESC_REQ;

typedef struct smb_com_transaction_change_notify_req {
	struct smb_hdr hdr;     /* wct = 23 */
	__u8 MaxSetupCount;
	__u16 Reserved;
	__le32 TotalParameterCount;
	__le32 TotalDataCount;
	__le32 MaxParameterCount;
	__le32 MaxDataCount;
	__le32 ParameterCount;
	__le32 ParameterOffset;
	__le32 DataCount;
	__le32 DataOffset;
	__u8 SetupCount; /* four setup words follow subcommand */
	/* SNIA spec incorrectly included spurious pad here */
	__le16 SubCommand;/* 4 = Change Notify */
	__le32 CompletionFilter;  /* operation to monitor */
	__u16 Fid;
	__u8 WatchTree;  /* 1 = Monitor subdirectories */
	__u8 Reserved2;
	__le16 ByteCount;
/* 	__u8 Pad[3];*/
/*	__u8 Data[1];*/
} __attribute__((packed)) TRANSACT_CHANGE_NOTIFY_REQ;

/* BB eventually change to use generic ntransact rsp struct
      and validation routine */
typedef struct smb_com_transaction_change_notify_rsp {
	struct smb_hdr hdr;	/* wct = 18 */
	__u8 Reserved[3];
	__le32 TotalParameterCount;
	__le32 TotalDataCount;
	__le32 ParameterCount;
	__le32 ParameterOffset;
	__le32 ParameterDisplacement;
	__le32 DataCount;
	__le32 DataOffset;
	__le32 DataDisplacement;
	__u8 SetupCount;   /* 0 */
	__u16 ByteCount;
	/* __u8 Pad[3]; */
} __attribute__((packed)) TRANSACT_CHANGE_NOTIFY_RSP;
/* Completion Filter flags for Notify */
#define FILE_NOTIFY_CHANGE_FILE_NAME    0x00000001
#define FILE_NOTIFY_CHANGE_DIR_NAME     0x00000002
#define FILE_NOTIFY_CHANGE_NAME         0x00000003
#define FILE_NOTIFY_CHANGE_ATTRIBUTES   0x00000004
#define FILE_NOTIFY_CHANGE_SIZE         0x00000008
#define FILE_NOTIFY_CHANGE_LAST_WRITE   0x00000010
#define FILE_NOTIFY_CHANGE_LAST_ACCESS  0x00000020
#define FILE_NOTIFY_CHANGE_CREATION     0x00000040
#define FILE_NOTIFY_CHANGE_EA           0x00000080
#define FILE_NOTIFY_CHANGE_SECURITY     0x00000100
#define FILE_NOTIFY_CHANGE_STREAM_NAME  0x00000200
#define FILE_NOTIFY_CHANGE_STREAM_SIZE  0x00000400
#define FILE_NOTIFY_CHANGE_STREAM_WRITE 0x00000800

#define FILE_ACTION_ADDED		0x00000001
#define FILE_ACTION_REMOVED		0x00000002
#define FILE_ACTION_MODIFIED		0x00000003
#define FILE_ACTION_RENAMED_OLD_NAME	0x00000004
#define FILE_ACTION_RENAMED_NEW_NAME	0x00000005
#define FILE_ACTION_ADDED_STREAM	0x00000006
#define FILE_ACTION_REMOVED_STREAM	0x00000007
#define FILE_ACTION_MODIFIED_STREAM	0x00000008

/* response contains array of the following structures */
struct file_notify_information {
	__le32 NextEntryOffset;
	__le32 Action;
	__le32 FileNameLength;
	__u8  FileName[];
} __attribute__((packed));

/* For IO_REPARSE_TAG_SYMLINK */
struct reparse_symlink_data {
	__le32	ReparseTag;
	__le16	ReparseDataLength;
	__u16	Reserved;
	__le16	SubstituteNameOffset;
	__le16	SubstituteNameLength;
	__le16	PrintNameOffset;
	__le16	PrintNameLength;
	__le32	Flags;
	char	PathBuffer[];
} __attribute__((packed));

/* Flag above */
#define SYMLINK_FLAG_RELATIVE 0x00000001

/* For IO_REPARSE_TAG_NFS */
#define NFS_SPECFILE_LNK	0x00000000014B4E4C
#define NFS_SPECFILE_CHR	0x0000000000524843
#define NFS_SPECFILE_BLK	0x00000000004B4C42
#define NFS_SPECFILE_FIFO	0x000000004F464946
#define NFS_SPECFILE_SOCK	0x000000004B434F53
struct reparse_posix_data {
	__le32	ReparseTag;
	__le16	ReparseDataLength;
	__u16	Reserved;
	__le64	InodeType; /* LNK, FIFO, CHR etc. */
	char	PathBuffer[];
} __attribute__((packed));

struct cifs_quota_data {
	__u32	rsrvd1;  /* 0 */
	__u32	sid_size;
	__u64	rsrvd2;  /* 0 */
	__u64	space_used;
	__u64	soft_limit;
	__u64	hard_limit;
	char	sid[1];  /* variable size? */
} __attribute__((packed));

/* quota sub commands */
#define QUOTA_LIST_CONTINUE	    0
#define QUOTA_LIST_START	0x100
#define QUOTA_FOR_SID		0x101

struct trans2_req {
	/* struct smb_hdr hdr precedes. Set wct = 14+ */
	__le16 TotalParameterCount;
	__le16 TotalDataCount;
	__le16 MaxParameterCount;
	__le16 MaxDataCount;
	__u8 MaxSetupCount;
	__u8 Reserved;
	__le16 Flags;
	__le32 Timeout;
	__u16 Reserved2;
	__le16 ParameterCount;
	__le16 ParameterOffset;
	__le16 DataCount;
	__le16 DataOffset;
	__u8 SetupCount;
	__u8 Reserved3;
	__le16 SubCommand; /* 1st setup word - SetupCount words follow */
	__le16 ByteCount;
} __attribute__((packed));

struct smb_t2_req {
	struct smb_hdr hdr;
	struct trans2_req t2_req;
} __attribute__((packed));

struct trans2_resp {
	/* struct smb_hdr hdr precedes. Note wct = 10 + setup count */
	__le16 TotalParameterCount;
	__le16 TotalDataCount;
	__u16 Reserved;
	__le16 ParameterCount;
	__le16 ParameterOffset;
	__le16 ParameterDisplacement;
	__le16 DataCount;
	__le16 DataOffset;
	__le16 DataDisplacement;
	__u8 SetupCount;
	__u8 Reserved1;
	/* SetupWords[SetupCount];
	__u16 ByteCount;
	__u16 Reserved2;*/
	/* data area follows */
} __attribute__((packed));

struct smb_t2_rsp {
	struct smb_hdr hdr;
	struct trans2_resp t2_rsp;
} __attribute__((packed));

/* PathInfo/FileInfo infolevels */
#define SMB_INFO_STANDARD                   1
#define SMB_SET_FILE_EA                     2
#define SMB_QUERY_FILE_EA_SIZE              2
#define SMB_INFO_QUERY_EAS_FROM_LIST        3
#define SMB_INFO_QUERY_ALL_EAS              4
#define SMB_INFO_IS_NAME_VALID              6
#define SMB_QUERY_FILE_BASIC_INFO       0x101
#define SMB_QUERY_FILE_STANDARD_INFO    0x102
#define SMB_QUERY_FILE_EA_INFO          0x103
#define SMB_QUERY_FILE_NAME_INFO        0x104
#define SMB_QUERY_FILE_ALLOCATION_INFO  0x105
#define SMB_QUERY_FILE_END_OF_FILEINFO  0x106
#define SMB_QUERY_FILE_ALL_INFO         0x107
#define SMB_QUERY_ALT_NAME_INFO         0x108
#define SMB_QUERY_FILE_STREAM_INFO      0x109
#define SMB_QUERY_FILE_COMPRESSION_INFO 0x10B
#define SMB_QUERY_FILE_UNIX_BASIC       0x200
#define SMB_QUERY_FILE_UNIX_LINK        0x201
#define SMB_QUERY_POSIX_ACL             0x204
#define SMB_QUERY_XATTR                 0x205  /* e.g. system EA name space */
#define SMB_QUERY_ATTR_FLAGS            0x206  /* append,immutable etc. */
#define SMB_QUERY_POSIX_PERMISSION      0x207
#define SMB_QUERY_POSIX_LOCK            0x208
/* #define SMB_POSIX_OPEN               0x209 */
/* #define SMB_POSIX_UNLINK             0x20a */
#define SMB_QUERY_FILE__UNIX_INFO2      0x20b
#define SMB_QUERY_FILE_INTERNAL_INFO    0x3ee
#define SMB_QUERY_FILE_ACCESS_INFO      0x3f0
#define SMB_QUERY_FILE_NAME_INFO2       0x3f1 /* 0x30 bytes */
#define SMB_QUERY_FILE_POSITION_INFO    0x3f6
#define SMB_QUERY_FILE_MODE_INFO        0x3f8
#define SMB_QUERY_FILE_ALGN_INFO        0x3f9


#define SMB_SET_FILE_BASIC_INFO	        0x101
#define SMB_SET_FILE_DISPOSITION_INFO   0x102
#define SMB_SET_FILE_ALLOCATION_INFO    0x103
#define SMB_SET_FILE_END_OF_FILE_INFO   0x104
#define SMB_SET_FILE_UNIX_BASIC         0x200
#define SMB_SET_FILE_UNIX_LINK          0x201
#define SMB_SET_FILE_UNIX_HLINK         0x203
#define SMB_SET_POSIX_ACL               0x204
#define SMB_SET_XATTR                   0x205
#define SMB_SET_ATTR_FLAGS              0x206  /* append, immutable etc. */
#define SMB_SET_POSIX_LOCK              0x208
#define SMB_POSIX_OPEN                  0x209
#define SMB_POSIX_UNLINK                0x20a
#define SMB_SET_FILE_UNIX_INFO2         0x20b
#define SMB_SET_FILE_BASIC_INFO2        0x3ec
#define SMB_SET_FILE_RENAME_INFORMATION 0x3f2 /* BB check if qpathinfo too */
#define SMB_FILE_ALL_INFO2              0x3fa
#define SMB_SET_FILE_ALLOCATION_INFO2   0x3fb
#define SMB_SET_FILE_END_OF_FILE_INFO2  0x3fc
#define SMB_FILE_MOVE_CLUSTER_INFO      0x407
#define SMB_FILE_QUOTA_INFO             0x408
#define SMB_FILE_REPARSEPOINT_INFO      0x409
#define SMB_FILE_MAXIMUM_INFO           0x40d

/* Find File infolevels */
#define SMB_FIND_FILE_INFO_STANDARD       0x001
#define SMB_FIND_FILE_QUERY_EA_SIZE       0x002
#define SMB_FIND_FILE_QUERY_EAS_FROM_LIST 0x003
#define SMB_FIND_FILE_DIRECTORY_INFO      0x101
#define SMB_FIND_FILE_FULL_DIRECTORY_INFO 0x102
#define SMB_FIND_FILE_NAMES_INFO          0x103
#define SMB_FIND_FILE_BOTH_DIRECTORY_INFO 0x104
#define SMB_FIND_FILE_ID_FULL_DIR_INFO    0x105
#define SMB_FIND_FILE_ID_BOTH_DIR_INFO    0x106
#define SMB_FIND_FILE_UNIX                0x202
#define SMB_FIND_FILE_POSIX_INFO          0x064

typedef struct smb_com_transaction2_qpi_req {
	struct smb_hdr hdr;	/* wct = 14+ */
	__le16 TotalParameterCount;
	__le16 TotalDataCount;
	__le16 MaxParameterCount;
	__le16 MaxDataCount;
	__u8 MaxSetupCount;
	__u8 Reserved;
	__le16 Flags;
	__le32 Timeout;
	__u16 Reserved2;
	__le16 ParameterCount;
	__le16 ParameterOffset;
	__le16 DataCount;
	__le16 DataOffset;
	__u8 SetupCount;
	__u8 Reserved3;
	__le16 SubCommand;	/* one setup word */
	__le16 ByteCount;
	__u8 Pad;
	__le16 InformationLevel;
	__u32 Reserved4;
	char FileName[1];
} __attribute__((packed)) TRANSACTION2_QPI_REQ;

typedef struct smb_com_transaction2_qpi_rsp {
	struct smb_hdr hdr;	/* wct = 10 + SetupCount */
	struct trans2_resp t2;
	__u16 ByteCount;
	__u16 Reserved2; /* parameter word is present for infolevels > 100 */
} __attribute__((packed)) TRANSACTION2_QPI_RSP;

typedef struct smb_com_transaction2_spi_req {
	struct smb_hdr hdr;	/* wct = 15 */
	__le16 TotalParameterCount;
	__le16 TotalDataCount;
	__le16 MaxParameterCount;
	__le16 MaxDataCount;
	__u8 MaxSetupCount;
	__u8 Reserved;
	__le16 Flags;
	__le32 Timeout;
	__u16 Reserved2;
	__le16 ParameterCount;
	__le16 ParameterOffset;
	__le16 DataCount;
	__le16 DataOffset;
	__u8 SetupCount;
	__u8 Reserved3;
	__le16 SubCommand;	/* one setup word */
	__le16 ByteCount;
	__u8 Pad;
	__u16 Pad1;
	__le16 InformationLevel;
	__u32 Reserved4;
	char FileName[1];
} __attribute__((packed)) TRANSACTION2_SPI_REQ;

typedef struct smb_com_transaction2_spi_rsp {
	struct smb_hdr hdr;	/* wct = 10 + SetupCount */
	struct trans2_resp t2;
	__u16 ByteCount;
	__u16 Reserved2; /* parameter word is present for infolevels > 100 */
} __attribute__((packed)) TRANSACTION2_SPI_RSP;

struct set_file_rename {
	__le32 overwrite;   /* 1 = overwrite dest */
	__u32 root_fid;   /* zero */
	__le32 target_name_len;
	char  target_name[];  /* Must be unicode */
} __attribute__((packed));

struct smb_com_transaction2_sfi_req {
	struct smb_hdr hdr;	/* wct = 15 */
	__le16 TotalParameterCount;
	__le16 TotalDataCount;
	__le16 MaxParameterCount;
	__le16 MaxDataCount;
	__u8 MaxSetupCount;
	__u8 Reserved;
	__le16 Flags;
	__le32 Timeout;
	__u16 Reserved2;
	__le16 ParameterCount;
	__le16 ParameterOffset;
	__le16 DataCount;
	__le16 DataOffset;
	__u8 SetupCount;
	__u8 Reserved3;
	__le16 SubCommand;	/* one setup word */
	__le16 ByteCount;
	__u8 Pad;
	__u16 Pad1;
	__u16 Fid;
	__le16 InformationLevel;
	__u16 Reserved4;
} __attribute__((packed));

struct smb_com_transaction2_sfi_rsp {
	struct smb_hdr hdr;	/* wct = 10 + SetupCount */
	struct trans2_resp t2;
	__u16 ByteCount;
	__u16 Reserved2;	/* parameter word reserved -
					present for infolevels > 100 */
} __attribute__((packed));

struct smb_t2_qfi_req {
	struct	smb_hdr hdr;
	struct	trans2_req t2;
	__u8	Pad;
	__u16	Fid;
	__le16	InformationLevel;
} __attribute__((packed));

struct smb_t2_qfi_rsp {
	struct smb_hdr hdr;     /* wct = 10 + SetupCount */
	struct trans2_resp t2;
	__u16 ByteCount;
	__u16 Reserved2;        /* parameter word reserved -
				   present for infolevels > 100 */
} __attribute__((packed));

/*
 * Flags on T2 FINDFIRST and FINDNEXT
 */
#define CIFS_SEARCH_CLOSE_ALWAYS  0x0001
#define CIFS_SEARCH_CLOSE_AT_END  0x0002
#define CIFS_SEARCH_RETURN_RESUME 0x0004
#define CIFS_SEARCH_CONTINUE_FROM_LAST 0x0008
#define CIFS_SEARCH_BACKUP_SEARCH 0x0010

/*
 * Size of the resume key on FINDFIRST and FINDNEXT calls
 */
#define CIFS_SMB_RESUME_KEY_SIZE 4

typedef struct smb_com_transaction2_ffirst_req {
	struct smb_hdr hdr;	/* wct = 15 */
	__le16 TotalParameterCount;
	__le16 TotalDataCount;
	__le16 MaxParameterCount;
	__le16 MaxDataCount;
	__u8 MaxSetupCount;
	__u8 Reserved;
	__le16 Flags;
	__le32 Timeout;
	__u16 Reserved2;
	__le16 ParameterCount;
	__le16 ParameterOffset;
	__le16 DataCount;
	__le16 DataOffset;
	__u8 SetupCount;	/* one */
	__u8 Reserved3;
	__le16 SubCommand;	/* TRANS2_FIND_FIRST */
	__le16 ByteCount;
	__u8 Pad;
	__le16 SearchAttributes;
	__le16 SearchCount;
	__le16 SearchFlags;
	__le16 InformationLevel;
	__le32 SearchStorageType;
	char FileName[1];
} __attribute__((packed)) TRANSACTION2_FFIRST_REQ;

typedef struct smb_com_transaction2_ffirst_rsp {
	struct smb_hdr hdr;	/* wct = 10 */
	struct trans2_resp t2;
	__u16 ByteCount;
} __attribute__((packed)) TRANSACTION2_FFIRST_RSP;

typedef struct smb_com_transaction2_ffirst_rsp_parms {
	__u16 SearchHandle;
	__le16 SearchCount;
	__le16 EndofSearch;
	__le16 EAErrorOffset;
	__le16 LastNameOffset;
} __attribute__((packed)) T2_FFIRST_RSP_PARMS;

typedef struct smb_com_transaction2_fnext_req {
	struct smb_hdr hdr;	/* wct = 15 */
	__le16 TotalParameterCount;
	__le16 TotalDataCount;
	__le16 MaxParameterCount;
	__le16 MaxDataCount;
	__u8 MaxSetupCount;
	__u8 Reserved;
	__le16 Flags;
	__le32 Timeout;
	__u16 Reserved2;
	__le16 ParameterCount;
	__le16 ParameterOffset;
	__le16 DataCount;
	__le16 DataOffset;
	__u8 SetupCount;	/* one */
	__u8 Reserved3;
	__le16 SubCommand;	/* TRANS2_FIND_NEXT */
	__le16 ByteCount;
	__u8 Pad;
	__u16 SearchHandle;
	__le16 SearchCount;
	__le16 InformationLevel;
	__u32 ResumeKey;
	__le16 SearchFlags;
	char ResumeFileName[];
} __attribute__((packed)) TRANSACTION2_FNEXT_REQ;

typedef struct smb_com_transaction2_fnext_rsp {
	struct smb_hdr hdr;	/* wct = 10 */
	struct trans2_resp t2;
	__u16 ByteCount;
} __attribute__((packed)) TRANSACTION2_FNEXT_RSP;

typedef struct smb_com_transaction2_fnext_rsp_parms {
	__le16 SearchCount;
	__le16 EndofSearch;
	__le16 EAErrorOffset;
	__le16 LastNameOffset;
} __attribute__((packed)) T2_FNEXT_RSP_PARMS;

/* QFSInfo Levels */
#define SMB_INFO_ALLOCATION         1
#define SMB_INFO_VOLUME             2
#define SMB_QUERY_FS_VOLUME_INFO    0x102
#define SMB_QUERY_FS_SIZE_INFO      0x103
#define SMB_QUERY_FS_DEVICE_INFO    0x104
#define SMB_QUERY_FS_ATTRIBUTE_INFO 0x105
#define SMB_QUERY_CIFS_UNIX_INFO    0x200
#define SMB_QUERY_POSIX_FS_INFO     0x201
#define SMB_QUERY_POSIX_WHO_AM_I    0x202
#define SMB_REQUEST_TRANSPORT_ENCRYPTION 0x203
#define SMB_QUERY_FS_PROXY          0x204 /* WAFS enabled. Returns structure
					    FILE_SYSTEM__UNIX_INFO to tell
					    whether new NTIOCTL available
					    (0xACE) for WAN friendly SMB
					    operations to be carried */
#define SMB_QUERY_LABEL_INFO        0x3ea
#define SMB_QUERY_FS_QUOTA_INFO     0x3ee
#define SMB_QUERY_FS_FULL_SIZE_INFO 0x3ef
#define SMB_QUERY_OBJECTID_INFO     0x3f0

typedef struct smb_com_transaction2_qfsi_req {
	struct smb_hdr hdr;	/* wct = 14+ */
	__le16 TotalParameterCount;
	__le16 TotalDataCount;
	__le16 MaxParameterCount;
	__le16 MaxDataCount;
	__u8 MaxSetupCount;
	__u8 Reserved;
	__le16 Flags;
	__le32 Timeout;
	__u16 Reserved2;
	__le16 ParameterCount;
	__le16 ParameterOffset;
	__le16 DataCount;
	__le16 DataOffset;
	__u8 SetupCount;
	__u8 Reserved3;
	__le16 SubCommand;	/* one setup word */
	__le16 ByteCount;
	__u8 Pad;
	__le16 InformationLevel;
} __attribute__((packed)) TRANSACTION2_QFSI_REQ;

typedef struct smb_com_transaction_qfsi_rsp {
	struct smb_hdr hdr;	/* wct = 10 + SetupCount */
	struct trans2_resp t2;
	__u16 ByteCount;
	__u8 Pad;	/* may be three bytes? *//* followed by data area */
} __attribute__((packed)) TRANSACTION2_QFSI_RSP;

typedef struct whoami_rsp_data { /* Query level 0x202 */
	__u32 flags; /* 0 = Authenticated user 1 = GUEST */
	__u32 mask; /* which flags bits server understands ie 0x0001 */
	__u64 unix_user_id;
	__u64 unix_user_gid;
	__u32 number_of_supplementary_gids; /* may be zero */
	__u32 number_of_sids; /* may be zero */
	__u32 length_of_sid_array; /* in bytes - may be zero */
	__u32 pad; /* reserved - MBZ */
	/* __u64 gid_array[0]; */  /* may be empty */
	/* __u8 * psid_list */  /* may be empty */
} __attribute__((packed)) WHOAMI_RSP_DATA;

/* SETFSInfo Levels */
#define SMB_SET_CIFS_UNIX_INFO    0x200
/* level 0x203 is defined above in list of QFS info levels */
/* #define SMB_REQUEST_TRANSPORT_ENCRYPTION 0x203 */

/* Level 0x200 request structure follows */
typedef struct smb_com_transaction2_setfsi_req {
	struct smb_hdr hdr;	/* wct = 15 */
	__le16 TotalParameterCount;
	__le16 TotalDataCount;
	__le16 MaxParameterCount;
	__le16 MaxDataCount;
	__u8 MaxSetupCount;
	__u8 Reserved;
	__le16 Flags;
	__le32 Timeout;
	__u16 Reserved2;
	__le16 ParameterCount;	/* 4 */
	__le16 ParameterOffset;
	__le16 DataCount;	/* 12 */
	__le16 DataOffset;
	__u8 SetupCount;	/* one */
	__u8 Reserved3;
	__le16 SubCommand;	/* TRANS2_SET_FS_INFORMATION */
	__le16 ByteCount;
	__u8 Pad;
	__u16 FileNum;		/* Parameters start. */
	__le16 InformationLevel;/* Parameters end. */
	__le16 ClientUnixMajor; /* Data start. */
	__le16 ClientUnixMinor;
	__le64 ClientUnixCap;   /* Data end */
} __attribute__((packed)) TRANSACTION2_SETFSI_REQ;

/* level 0x203 request structure follows */
typedef struct smb_com_transaction2_setfs_enc_req {
	struct smb_hdr hdr;	/* wct = 15 */
	__le16 TotalParameterCount;
	__le16 TotalDataCount;
	__le16 MaxParameterCount;
	__le16 MaxDataCount;
	__u8 MaxSetupCount;
	__u8 Reserved;
	__le16 Flags;
	__le32 Timeout;
	__u16 Reserved2;
	__le16 ParameterCount;	/* 4 */
	__le16 ParameterOffset;
	__le16 DataCount;	/* 12 */
	__le16 DataOffset;
	__u8 SetupCount;	/* one */
	__u8 Reserved3;
	__le16 SubCommand;	/* TRANS2_SET_FS_INFORMATION */
	__le16 ByteCount;
	__u8 Pad;
	__u16  Reserved4;	/* Parameters start. */
	__le16 InformationLevel;/* Parameters end. */
	/* NTLMSSP Blob, Data start. */
} __attribute__((packed)) TRANSACTION2_SETFSI_ENC_REQ;

/* response for setfsinfo levels 0x200 and 0x203 */
typedef struct smb_com_transaction2_setfsi_rsp {
	struct smb_hdr hdr;	/* wct = 10 */
	struct trans2_resp t2;
	__u16 ByteCount;
} __attribute__((packed)) TRANSACTION2_SETFSI_RSP;

typedef struct smb_com_transaction2_get_dfs_refer_req {
	struct smb_hdr hdr;	/* wct = 15 */
	__le16 TotalParameterCount;
	__le16 TotalDataCount;
	__le16 MaxParameterCount;
	__le16 MaxDataCount;
	__u8 MaxSetupCount;
	__u8 Reserved;
	__le16 Flags;
	__le32 Timeout;
	__u16 Reserved2;
	__le16 ParameterCount;
	__le16 ParameterOffset;
	__le16 DataCount;
	__le16 DataOffset;
	__u8 SetupCount;
	__u8 Reserved3;
	__le16 SubCommand;	/* one setup word */
	__le16 ByteCount;
	__u8 Pad[3];		/* Win2K has sent 0x0F01 (max response length
				   perhaps?) followed by one byte pad - doesn't
				   seem to matter though */
	__le16 MaxReferralLevel;
	char RequestFileName[1];
} __attribute__((packed)) TRANSACTION2_GET_DFS_REFER_REQ;

#define DFS_VERSION cpu_to_le16(0x0003)

/* DFS server target type */
#define DFS_TYPE_LINK 0x0000  /* also for sysvol targets */
#define DFS_TYPE_ROOT 0x0001

/* Referral Entry Flags */
#define DFS_NAME_LIST_REF 0x0200 /* set for domain or DC referral responses */
#define DFS_TARGET_SET_BOUNDARY 0x0400 /* only valid with version 4 dfs req */

typedef struct dfs_referral_level_3 { /* version 4 is same, + one flag bit */
	__le16 VersionNumber;  /* must be 3 or 4 */
	__le16 Size;
	__le16 ServerType; /* 0x0001 = root targets; 0x0000 = link targets */
	__le16 ReferralEntryFlags;
	__le32 TimeToLive;
	__le16 DfsPathOffset;
	__le16 DfsAlternatePathOffset;
	__le16 NetworkAddressOffset; /* offset of the link target */
	__u8   ServiceSiteGuid[16];  /* MBZ, ignored */
} __attribute__((packed)) REFERRAL3;

struct get_dfs_referral_rsp {
	__le16 PathConsumed;
	__le16 NumberOfReferrals;
	__le32 DFSFlags;
	REFERRAL3 referrals[1];	/* array of level 3 dfs_referral structures */
	/* followed by the strings pointed to by the referral structures */
} __packed;

typedef struct smb_com_transaction_get_dfs_refer_rsp {
	struct smb_hdr hdr;	/* wct = 10 */
	struct trans2_resp t2;
	__u16 ByteCount;
	__u8 Pad;
	struct get_dfs_referral_rsp dfs_data;
} __packed TRANSACTION2_GET_DFS_REFER_RSP;

/* DFS Flags */
#define DFSREF_REFERRAL_SERVER  0x00000001 /* all targets are DFS roots */
#define DFSREF_STORAGE_SERVER   0x00000002 /* no further ref requests needed */
#define DFSREF_TARGET_FAILBACK  0x00000004 /* only for DFS referral version 4 */

/*
 ************************************************************************
 * All structs for everything above the SMB PDUs themselves
 * (such as the T2 level specific data) go here
 ************************************************************************
 */

/*
 * Information on a server
 */

struct serverInfo {
	char name[16];
	unsigned char versionMajor;
	unsigned char versionMinor;
	unsigned long type;
	unsigned int commentOffset;
} __attribute__((packed));

/*
 * The following structure is the format of the data returned on a NetShareEnum
 * with level "90" (x5A)
 */

struct shareInfo {
	char shareName[13];
	char pad;
	unsigned short type;
	unsigned int commentOffset;
} __attribute__((packed));

struct aliasInfo {
	char aliasName[9];
	char pad;
	unsigned int commentOffset;
	unsigned char type[2];
} __attribute__((packed));

struct aliasInfo92 {
	int aliasNameOffset;
	int serverNameOffset;
	int shareNameOffset;
} __attribute__((packed));

typedef struct {
	__le64 TotalAllocationUnits;
	__le64 FreeAllocationUnits;
	__le32 SectorsPerAllocationUnit;
	__le32 BytesPerSector;
} __attribute__((packed)) FILE_SYSTEM_INFO;	/* size info, level 0x103 */

typedef struct {
	__le32 fsid;
	__le32 SectorsPerAllocationUnit;
	__le32 TotalAllocationUnits;
	__le32 FreeAllocationUnits;
	__le16  BytesPerSector;
} __attribute__((packed)) FILE_SYSTEM_ALLOC_INFO;

typedef struct {
	__le16 MajorVersionNumber;
	__le16 MinorVersionNumber;
	__le64 Capability;
} __attribute__((packed)) FILE_SYSTEM_UNIX_INFO; /* Unix extension level 0x200*/

/* Version numbers for CIFS UNIX major and minor. */
#define CIFS_UNIX_MAJOR_VERSION 1
#define CIFS_UNIX_MINOR_VERSION 0

/* Linux/Unix extensions capability flags */
#define CIFS_UNIX_FCNTL_CAP             0x00000001 /* support for fcntl locks */
#define CIFS_UNIX_POSIX_ACL_CAP         0x00000002 /* support getfacl/setfacl */
#define CIFS_UNIX_XATTR_CAP             0x00000004 /* support new namespace   */
#define CIFS_UNIX_EXTATTR_CAP           0x00000008 /* support chattr/chflag   */
#define CIFS_UNIX_POSIX_PATHNAMES_CAP   0x00000010 /* Allow POSIX path chars  */
#define CIFS_UNIX_POSIX_PATH_OPS_CAP    0x00000020 /* Allow new POSIX path based
						      calls including posix open
						      and posix unlink */
#define CIFS_UNIX_LARGE_READ_CAP        0x00000040 /* support reads >128K (up
						      to 0xFFFF00 */
#define CIFS_UNIX_LARGE_WRITE_CAP       0x00000080
#define CIFS_UNIX_TRANSPORT_ENCRYPTION_CAP 0x00000100 /* can do SPNEGO crypt */
#define CIFS_UNIX_TRANSPORT_ENCRYPTION_MANDATORY_CAP  0x00000200 /* must do  */
#define CIFS_UNIX_PROXY_CAP             0x00000400 /* Proxy cap: 0xACE ioctl and
						      QFS PROXY call */
#ifdef CONFIG_CIFS_POSIX
/* presumably don't need the 0x20 POSIX_PATH_OPS_CAP since we never send
   LockingX instead of posix locking call on unix sess (and we do not expect
   LockingX to use different (ie Windows) semantics than posix locking on
   the same session (if WINE needs to do this later, we can add this cap
   back in later */
/* #define CIFS_UNIX_CAP_MASK              0x000000fb */
#define CIFS_UNIX_CAP_MASK              0x000003db
#else
#define CIFS_UNIX_CAP_MASK              0x00000013
#endif /* CONFIG_CIFS_POSIX */


#define CIFS_POSIX_EXTENSIONS           0x00000010 /* support for new QFSInfo */

typedef struct {
	/* For undefined recommended transfer size return -1 in that field */
	__le32 OptimalTransferSize;  /* bsize on some os, iosize on other os */
	__le32 BlockSize;
    /* The next three fields are in terms of the block size.
	(above). If block size is unknown, 4096 would be a
	reasonable block size for a server to report.
	Note that returning the blocks/blocksavail removes need
	to make a second call (to QFSInfo level 0x103 to get this info.
	UserBlockAvail is typically less than or equal to BlocksAvail,
	if no distinction is made return the same value in each */
	__le64 TotalBlocks;
	__le64 BlocksAvail;       /* bfree */
	__le64 UserBlocksAvail;   /* bavail */
    /* For undefined Node fields or FSID return -1 */
	__le64 TotalFileNodes;
	__le64 FreeFileNodes;
	__le64 FileSysIdentifier;   /* fsid */
	/* NB Namelen comes from FILE_SYSTEM_ATTRIBUTE_INFO call */
	/* NB flags can come from FILE_SYSTEM_DEVICE_INFO call   */
} __attribute__((packed)) FILE_SYSTEM_POSIX_INFO;

/* DeviceType Flags */
#define FILE_DEVICE_CD_ROM              0x00000002
#define FILE_DEVICE_CD_ROM_FILE_SYSTEM  0x00000003
#define FILE_DEVICE_DFS                 0x00000006
#define FILE_DEVICE_DISK                0x00000007
#define FILE_DEVICE_DISK_FILE_SYSTEM    0x00000008
#define FILE_DEVICE_FILE_SYSTEM         0x00000009
#define FILE_DEVICE_NAMED_PIPE          0x00000011
#define FILE_DEVICE_NETWORK             0x00000012
#define FILE_DEVICE_NETWORK_FILE_SYSTEM 0x00000014
#define FILE_DEVICE_NULL                0x00000015
#define FILE_DEVICE_PARALLEL_PORT       0x00000016
#define FILE_DEVICE_PRINTER             0x00000018
#define FILE_DEVICE_SERIAL_PORT         0x0000001b
#define FILE_DEVICE_STREAMS             0x0000001e
#define FILE_DEVICE_TAPE                0x0000001f
#define FILE_DEVICE_TAPE_FILE_SYSTEM    0x00000020
#define FILE_DEVICE_VIRTUAL_DISK        0x00000024
#define FILE_DEVICE_NETWORK_REDIRECTOR  0x00000028

/* Device Characteristics */
#define FILE_REMOVABLE_MEDIA			0x00000001
#define FILE_READ_ONLY_DEVICE			0x00000002
#define FILE_FLOPPY_DISKETTE			0x00000004
#define FILE_WRITE_ONCE_MEDIA			0x00000008
#define FILE_REMOTE_DEVICE			0x00000010
#define FILE_DEVICE_IS_MOUNTED			0x00000020
#define FILE_VIRTUAL_VOLUME			0x00000040
#define FILE_DEVICE_SECURE_OPEN			0x00000100
#define FILE_CHARACTERISTIC_TS_DEVICE		0x00001000
#define FILE_CHARACTERISTIC_WEBDAV_DEVICE	0x00002000
#define FILE_PORTABLE_DEVICE			0x00004000
#define FILE_DEVICE_ALLOW_APPCONTAINER_TRAVERSAL 0x00020000

typedef struct {
	__le32 DeviceType;
	__le32 DeviceCharacteristics;
} __attribute__((packed)) FILE_SYSTEM_DEVICE_INFO; /* device info level 0x104 */

/* minimum includes first three fields, and empty FS Name */
#define MIN_FS_ATTR_INFO_SIZE 12


/* List of FileSystemAttributes - see 2.5.1 of MS-FSCC */
#define FILE_SUPPORTS_SPARSE_VDL	0x10000000 /* faster nonsparse extend */
#define FILE_SUPPORTS_BLOCK_REFCOUNTING	0x08000000 /* allow ioctl dup extents */
#define FILE_SUPPORT_INTEGRITY_STREAMS	0x04000000
#define FILE_SUPPORTS_USN_JOURNAL	0x02000000
#define FILE_SUPPORTS_OPEN_BY_FILE_ID	0x01000000
#define FILE_SUPPORTS_EXTENDED_ATTRIBUTES 0x00800000
#define FILE_SUPPORTS_HARD_LINKS	0x00400000
#define FILE_SUPPORTS_TRANSACTIONS	0x00200000
#define FILE_SEQUENTIAL_WRITE_ONCE	0x00100000
#define FILE_READ_ONLY_VOLUME		0x00080000
#define FILE_NAMED_STREAMS		0x00040000
#define FILE_SUPPORTS_ENCRYPTION	0x00020000
#define FILE_SUPPORTS_OBJECT_IDS	0x00010000
#define FILE_VOLUME_IS_COMPRESSED	0x00008000
#define FILE_SUPPORTS_REMOTE_STORAGE	0x00000100
#define FILE_SUPPORTS_REPARSE_POINTS	0x00000080
#define FILE_SUPPORTS_SPARSE_FILES	0x00000040
#define FILE_VOLUME_QUOTAS		0x00000020
#define FILE_FILE_COMPRESSION		0x00000010
#define FILE_PERSISTENT_ACLS		0x00000008
#define FILE_UNICODE_ON_DISK		0x00000004
#define FILE_CASE_PRESERVED_NAMES	0x00000002
#define FILE_CASE_SENSITIVE_SEARCH	0x00000001
typedef struct {
	__le32 Attributes;
	__le32 MaxPathNameComponentLength;
	__le32 FileSystemNameLen;
	char FileSystemName[52]; /* do not have to save this - get subset? */
} __attribute__((packed)) FILE_SYSTEM_ATTRIBUTE_INFO;

/******************************************************************************/
/* QueryFileInfo/QueryPathinfo (also for SetPath/SetFile) data buffer formats */
/******************************************************************************/
typedef struct { /* data block encoding of response to level 263 QPathInfo */
	__le64 CreationTime;
	__le64 LastAccessTime;
	__le64 LastWriteTime;
	__le64 ChangeTime;
	__le32 Attributes;
	__u32 Pad1;
	__le64 AllocationSize;
	__le64 EndOfFile;	/* size ie offset to first free byte in file */
	__le32 NumberOfLinks;	/* hard links */
	__u8 DeletePending;
	__u8 Directory;
	__u16 Pad2;
	__le64 IndexNumber;
	__le32 EASize;
	__le32 AccessFlags;
	__u64 IndexNumber1;
	__le64 CurrentByteOffset;
	__le32 Mode;
	__le32 AlignmentRequirement;
	__le32 FileNameLength;
	char FileName[1];
} __attribute__((packed)) FILE_ALL_INFO;	/* level 0x107 QPathInfo */

typedef struct {
	__le64 AllocationSize;
	__le64 EndOfFile;	/* size ie offset to first free byte in file */
	__le32 NumberOfLinks;	/* hard links */
	__u8 DeletePending;
	__u8 Directory;
	__u16 Pad;
} __attribute__((packed)) FILE_STANDARD_INFO;	/* level 0x102 QPathInfo */


/* defines for enumerating possible values of the Unix type field below */
#define UNIX_FILE      0
#define UNIX_DIR       1
#define UNIX_SYMLINK   2
#define UNIX_CHARDEV   3
#define UNIX_BLOCKDEV  4
#define UNIX_FIFO      5
#define UNIX_SOCKET    6
typedef struct {
	__le64 EndOfFile;
	__le64 NumOfBytes;
	__le64 LastStatusChange; /*SNIA specs DCE time for the 3 time fields */
	__le64 LastAccessTime;
	__le64 LastModificationTime;
	__le64 Uid;
	__le64 Gid;
	__le32 Type;
	__le64 DevMajor;
	__le64 DevMinor;
	__le64 UniqueId;
	__le64 Permissions;
	__le64 Nlinks;
} __attribute__((packed)) FILE_UNIX_BASIC_INFO;	/* level 0x200 QPathInfo */

typedef struct {
	char LinkDest[1];
} __attribute__((packed)) FILE_UNIX_LINK_INFO;	/* level 0x201 QPathInfo */

/* The following three structures are needed only for
	setting time to NT4 and some older servers via
	the primitive DOS time format */
typedef struct {
	__u16 Day:5;
	__u16 Month:4;
	__u16 Year:7;
} __attribute__((packed)) SMB_DATE;

typedef struct {
	__u16 TwoSeconds:5;
	__u16 Minutes:6;
	__u16 Hours:5;
} __attribute__((packed)) SMB_TIME;

typedef struct {
	__le16 CreationDate; /* SMB Date see above */
	__le16 CreationTime; /* SMB Time */
	__le16 LastAccessDate;
	__le16 LastAccessTime;
	__le16 LastWriteDate;
	__le16 LastWriteTime;
	__le32 DataSize; /* File Size (EOF) */
	__le32 AllocationSize;
	__le16 Attributes; /* verify not u32 */
	__le32 EASize;
} __attribute__((packed)) FILE_INFO_STANDARD;  /* level 1 SetPath/FileInfo */

typedef struct {
	__le64 CreationTime;
	__le64 LastAccessTime;
	__le64 LastWriteTime;
	__le64 ChangeTime;
	__le32 Attributes;
	__u32 Pad;
} __attribute__((packed)) FILE_BASIC_INFO;	/* size info, level 0x101 */

struct file_allocation_info {
	__le64 AllocationSize; /* Note old Samba srvr rounds this up too much */
} __attribute__((packed));	/* size used on disk, for level 0x103 for set,
				   0x105 for query */

struct file_end_of_file_info {
	__le64 FileSize;		/* offset to end of file */
} __attribute__((packed)); /* size info, level 0x104 for set, 0x106 for query */

struct file_alt_name_info {
	__u8   alt_name[1];
} __attribute__((packed));      /* level 0x0108 */

struct file_stream_info {
	__le32 number_of_streams;  /* BB check sizes and verify location */
	/* followed by info on streams themselves
		u64 size;
		u64 allocation_size
		stream info */
};      /* level 0x109 */

struct file_compression_info {
	__le64 compressed_size;
	__le16 format;
	__u8   unit_shift;
	__u8   ch_shift;
	__u8   cl_shift;
	__u8   pad[3];
} __attribute__((packed));      /* level 0x10b */

/* POSIX ACL set/query path info structures */
#define CIFS_ACL_VERSION 1
struct cifs_posix_ace { /* access control entry (ACE) */
	__u8  cifs_e_tag;
	__u8  cifs_e_perm;
	__le64 cifs_uid; /* or gid */
} __attribute__((packed));

struct cifs_posix_acl { /* access conrol list  (ACL) */
	__le16	version;
	__le16	access_entry_count;  /* access ACL - count of entries */
	__le16	default_entry_count; /* default ACL - count of entries */
	struct cifs_posix_ace ace_array[];
	/* followed by
	struct cifs_posix_ace default_ace_arraay[] */
} __attribute__((packed));  /* level 0x204 */

/* types of access control entries already defined in posix_acl.h */
/* #define CIFS_POSIX_ACL_USER_OBJ	 0x01
#define CIFS_POSIX_ACL_USER      0x02
#define CIFS_POSIX_ACL_GROUP_OBJ 0x04
#define CIFS_POSIX_ACL_GROUP     0x08
#define CIFS_POSIX_ACL_MASK      0x10
#define CIFS_POSIX_ACL_OTHER     0x20 */

/* types of perms */
/* #define CIFS_POSIX_ACL_EXECUTE   0x01
#define CIFS_POSIX_ACL_WRITE     0x02
#define CIFS_POSIX_ACL_READ	     0x04 */

/* end of POSIX ACL definitions */

/* POSIX Open Flags */
#define SMB_O_RDONLY 	 0x1
#define SMB_O_WRONLY 	0x2
#define SMB_O_RDWR 	0x4
#define SMB_O_CREAT 	0x10
#define SMB_O_EXCL 	0x20
#define SMB_O_TRUNC 	0x40
#define SMB_O_APPEND 	0x80
#define SMB_O_SYNC 	0x100
#define SMB_O_DIRECTORY 0x200
#define SMB_O_NOFOLLOW 	0x400
#define SMB_O_DIRECT 	0x800

typedef struct {
	__le32 OpenFlags; /* same as NT CreateX */
	__le32 PosixOpenFlags;
	__le64 Permissions;
	__le16 Level; /* reply level requested (see QPathInfo levels) */
} __attribute__((packed)) OPEN_PSX_REQ; /* level 0x209 SetPathInfo data */

typedef struct {
	__le16 OplockFlags;
	__u16 Fid;
	__le32 CreateAction;
	__le16 ReturnedLevel;
	__le16 Pad;
	/* struct following varies based on requested level */
} __attribute__((packed)) OPEN_PSX_RSP; /* level 0x209 SetPathInfo data */

#define SMB_POSIX_UNLINK_FILE_TARGET		0
#define SMB_POSIX_UNLINK_DIRECTORY_TARGET	1

struct unlink_psx_rq { /* level 0x20a SetPathInfo */
	__le16 type;
} __attribute__((packed));

struct file_internal_info {
	__le64  UniqueId; /* inode number */
} __attribute__((packed));      /* level 0x3ee */

struct file_mode_info {
	__le32	Mode;
} __attribute__((packed));      /* level 0x3f8 */

struct file_attrib_tag {
	__le32 Attribute;
	__le32 ReparseTag;
} __attribute__((packed));      /* level 0x40b */


/********************************************************/
/*  FindFirst/FindNext transact2 data buffer formats    */
/********************************************************/

typedef struct {
	__le32 NextEntryOffset;
	__u32 ResumeKey; /* as with FileIndex - no need to convert */
	FILE_UNIX_BASIC_INFO basic;
	char FileName[1];
} __attribute__((packed)) FILE_UNIX_INFO; /* level 0x202 */

typedef struct {
	__le32 NextEntryOffset;
	__u32 FileIndex;
	__le64 CreationTime;
	__le64 LastAccessTime;
	__le64 LastWriteTime;
	__le64 ChangeTime;
	__le64 EndOfFile;
	__le64 AllocationSize;
	__le32 ExtFileAttributes;
	__le32 FileNameLength;
	char FileName[1];
} __attribute__((packed)) FILE_DIRECTORY_INFO;   /* level 0x101 FF resp data */

typedef struct {
	__le32 NextEntryOffset;
	__u32 FileIndex;
	__le64 CreationTime;
	__le64 LastAccessTime;
	__le64 LastWriteTime;
	__le64 ChangeTime;
	__le64 EndOfFile;
	__le64 AllocationSize;
	__le32 ExtFileAttributes;
	__le32 FileNameLength;
	__le32 EaSize; /* length of the xattrs */
	char FileName[1];
} __attribute__((packed)) FILE_FULL_DIRECTORY_INFO; /* level 0x102 rsp data */

typedef struct {
	__le32 NextEntryOffset;
	__u32 FileIndex;
	__le64 CreationTime;
	__le64 LastAccessTime;
	__le64 LastWriteTime;
	__le64 ChangeTime;
	__le64 EndOfFile;
	__le64 AllocationSize;
	__le32 ExtFileAttributes;
	__le32 FileNameLength;
	__le32 EaSize; /* EA size */
	__le32 Reserved;
	__le64 UniqueId; /* inode num - le since Samba puts ino in low 32 bit*/
	char FileName[1];
} __attribute__((packed)) SEARCH_ID_FULL_DIR_INFO; /* level 0x105 FF rsp data */

typedef struct {
	__le32 NextEntryOffset;
	__u32 FileIndex;
	__le64 CreationTime;
	__le64 LastAccessTime;
	__le64 LastWriteTime;
	__le64 ChangeTime;
	__le64 EndOfFile;
	__le64 AllocationSize;
	__le32 ExtFileAttributes;
	__le32 FileNameLength;
	__le32 EaSize; /* length of the xattrs */
	__u8   ShortNameLength;
	__u8   Reserved;
	__u8   ShortName[12];
	char FileName[1];
} __attribute__((packed)) FILE_BOTH_DIRECTORY_INFO; /* level 0x104 FFrsp data */

typedef struct {
	__u32  ResumeKey;
	__le16 CreationDate; /* SMB Date */
	__le16 CreationTime; /* SMB Time */
	__le16 LastAccessDate;
	__le16 LastAccessTime;
	__le16 LastWriteDate;
	__le16 LastWriteTime;
	__le32 DataSize; /* File Size (EOF) */
	__le32 AllocationSize;
	__le16 Attributes; /* verify not u32 */
	__u8   FileNameLength;
	char FileName[1];
} __attribute__((packed)) FIND_FILE_STANDARD_INFO; /* level 0x1 FF resp data */


struct win_dev {
	unsigned char type[8]; /* IntxCHR or IntxBLK */
	__le64 major;
	__le64 minor;
} __attribute__((packed));

struct gea {
	unsigned char name_len;
	char name[1];
} __attribute__((packed));

struct gealist {
	unsigned long list_len;
	struct gea list[1];
} __attribute__((packed));

struct fea {
	unsigned char EA_flags;
	__u8 name_len;
	__le16 value_len;
	char name[1];
	/* optionally followed by value */
} __attribute__((packed));
/* flags for _FEA.fEA */
#define FEA_NEEDEA         0x80	/* need EA bit */

struct fealist {
	__le32 list_len;
	struct fea list[1];
} __attribute__((packed));

/* used to hold an arbitrary blob of data */
struct data_blob {
	__u8 *data;
	size_t length;
	void (*free) (struct data_blob *data_blob);
} __attribute__((packed));


#ifdef CONFIG_CIFS_POSIX
/*
	For better POSIX semantics from Linux client, (even better
	than the existing CIFS Unix Extensions) we need updated PDUs for:

	1) PosixCreateX - to set and return the mode, inode#, device info and
	perhaps add a CreateDevice - to create Pipes and other special .inodes
	Also note POSIX open flags
	2) Close - to return the last write time to do cache across close
		more safely
	3) FindFirst return unique inode number - what about resume key, two
	forms short (matches readdir) and full (enough info to cache inodes)
	4) Mkdir - set mode

	And under consideration:
	5) FindClose2 (return nanosecond timestamp ??)
	6) Use nanosecond timestamps throughout all time fields if
	   corresponding attribute flag is set
	7) sendfile - handle based copy

	what about fixing 64 bit alignment

	There are also various legacy SMB/CIFS requests used as is

	From existing Lanman and NTLM dialects:
	--------------------------------------
	NEGOTIATE
	SESSION_SETUP_ANDX (BB which?)
	TREE_CONNECT_ANDX (BB which wct?)
	TREE_DISCONNECT (BB add volume timestamp on response)
	LOGOFF_ANDX
	DELETE (note delete open file behavior)
	DELETE_DIRECTORY
	READ_AND_X
	WRITE_AND_X
	LOCKING_AND_X (note posix lock semantics)
	RENAME (note rename across dirs and open file rename posix behaviors)
	NT_RENAME (for hardlinks) Is this good enough for all features?
	FIND_CLOSE2
	TRANSACTION2 (18 cases)
		SMB_SET_FILE_END_OF_FILE_INFO2 SMB_SET_PATH_END_OF_FILE_INFO2
		(BB verify that never need to set allocation size)
		SMB_SET_FILE_BASIC_INFO2 (setting times - BB can it be done via
			 Unix ext?)

	COPY (note support for copy across directories) - FUTURE, OPTIONAL
	setting/getting OS/2 EAs - FUTURE (BB can this handle
	setting Linux xattrs perfectly)         - OPTIONAL
	dnotify                                 - FUTURE, OPTIONAL
	quota                                   - FUTURE, OPTIONAL

	Note that various requests implemented for NT interop such as
		NT_TRANSACT (IOCTL) QueryReparseInfo
	are unneeded to servers compliant with the CIFS POSIX extensions

	From CIFS Unix Extensions:
	-------------------------
	T2 SET_PATH_INFO (SMB_SET_FILE_UNIX_LINK) for symlinks
	T2 SET_PATH_INFO (SMB_SET_FILE_BASIC_INFO2)
	T2 QUERY_PATH_INFO (SMB_QUERY_FILE_UNIX_LINK)
	T2 QUERY_PATH_INFO (SMB_QUERY_FILE_UNIX_BASIC)	BB check for missing
							inode fields
				Actually a need QUERY_FILE_UNIX_INFO
				since has inode num
				BB what about a) blksize/blkbits/blocks
							  b) i_version
							  c) i_rdev
							  d) notify mask?
							  e) generation
							  f) size_seqcount
	T2 FIND_FIRST/FIND_NEXT FIND_FILE_UNIX
	TRANS2_GET_DFS_REFERRAL		      - OPTIONAL but recommended
	T2_QFS_INFO QueryDevice/AttributeInfo - OPTIONAL
 */

/* xsymlink is a symlink format (used by MacOS) that can be used
   to save symlink info in a regular file when
   mounted to operating systems that do not
   support the cifs Unix extensions or EAs (for xattr
   based symlinks).  For such a file to be recognized
   as containing symlink data:

   1) file size must be 1067,
   2) signature must begin file data,
   3) length field must be set to ASCII representation
	of a number which is less than or equal to 1024,
   4) md5 must match that of the path data */

struct xsymlink {
	/* 1067 bytes */
	char signature[4]; /* XSym */ /* not null terminated */
	char cr0;         /* \n */
/* ASCII representation of length (4 bytes decimal) terminated by \n not null */
	char length[4];
	char cr1;         /* \n */
/* md5 of valid subset of path ie path[0] through path[length-1] */
	__u8 md5[32];
	char cr2;        /* \n */
/* if room left, then end with \n then 0x20s by convention but not required */
	char path[1024];
} __attribute__((packed));

typedef struct file_xattr_info {
	/* BB do we need another field for flags? BB */
	__u32 xattr_name_len;
	__u32 xattr_value_len;
	char  xattr_name[];
	/* followed by xattr_value[xattr_value_len], no pad */
} __attribute__((packed)) FILE_XATTR_INFO; /* extended attribute info
					      level 0x205 */

/* flags for lsattr and chflags commands removed arein uapi/linux/fs.h */

typedef struct file_chattr_info {
	__le64	mask; /* list of all possible attribute bits */
	__le64	mode; /* list of actual attribute bits on this inode */
} __attribute__((packed)) FILE_CHATTR_INFO;  /* ext attributes
						(chattr, chflags) level 0x206 */
#endif 				/* POSIX */
#endif				/* _CIFSPDU_H */
