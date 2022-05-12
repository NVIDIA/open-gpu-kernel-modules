/*
 *  Server-side types for NFSv4.
 *
 *  Copyright (c) 2002 The Regents of the University of Michigan.
 *  All rights reserved.
 *
 *  Kendrick Smith <kmsmith@umich.edu>
 *  Andy Adamson   <andros@umich.edu>
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the University nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 *  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 *  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef _LINUX_NFSD_XDR4_H
#define _LINUX_NFSD_XDR4_H

#include "state.h"
#include "nfsd.h"

#define NFSD4_MAX_TAGLEN	128
#define XDR_LEN(n)                     (((n) + 3) & ~3)

#define CURRENT_STATE_ID_FLAG (1<<0)
#define SAVED_STATE_ID_FLAG (1<<1)

#define SET_CSTATE_FLAG(c, f) ((c)->sid_flags |= (f))
#define HAS_CSTATE_FLAG(c, f) ((c)->sid_flags & (f))
#define CLEAR_CSTATE_FLAG(c, f) ((c)->sid_flags &= ~(f))

struct nfsd4_compound_state {
	struct svc_fh		current_fh;
	struct svc_fh		save_fh;
	struct nfs4_stateowner	*replay_owner;
	struct nfs4_client	*clp;
	/* For sessions DRC */
	struct nfsd4_session	*session;
	struct nfsd4_slot	*slot;
	int			data_offset;
	bool                    spo_must_allowed;
	size_t			iovlen;
	u32			minorversion;
	__be32			status;
	stateid_t	current_stateid;
	stateid_t	save_stateid;
	/* to indicate current and saved state id presents */
	u32		sid_flags;
};

static inline bool nfsd4_has_session(struct nfsd4_compound_state *cs)
{
	return cs->slot != NULL;
}

struct nfsd4_change_info {
	u32		atomic;
	u64		before_change;
	u64		after_change;
};

struct nfsd4_access {
	u32		ac_req_access;      /* request */
	u32		ac_supported;       /* response */
	u32		ac_resp_access;     /* response */
};

struct nfsd4_close {
	u32		cl_seqid;           /* request */
	stateid_t	cl_stateid;         /* request+response */
};

struct nfsd4_commit {
	u64		co_offset;          /* request */
	u32		co_count;           /* request */
	nfs4_verifier	co_verf;            /* response */
};

struct nfsd4_create {
	u32		cr_namelen;         /* request */
	char *		cr_name;            /* request */
	u32		cr_type;            /* request */
	union {                             /* request */
		struct {
			u32 datalen;
			char *data;
			struct kvec first;
		} link;   /* NF4LNK */
		struct {
			u32 specdata1;
			u32 specdata2;
		} dev;    /* NF4BLK, NF4CHR */
	} u;
	u32		cr_bmval[3];        /* request */
	struct iattr	cr_iattr;           /* request */
	int		cr_umask;           /* request */
	struct nfsd4_change_info  cr_cinfo; /* response */
	struct nfs4_acl *cr_acl;
	struct xdr_netobj cr_label;
};
#define cr_datalen	u.link.datalen
#define cr_data		u.link.data
#define cr_first	u.link.first
#define cr_specdata1	u.dev.specdata1
#define cr_specdata2	u.dev.specdata2

struct nfsd4_delegreturn {
	stateid_t	dr_stateid;
};

struct nfsd4_getattr {
	u32		ga_bmval[3];        /* request */
	struct svc_fh	*ga_fhp;            /* response */
};

struct nfsd4_link {
	u32		li_namelen;         /* request */
	char *		li_name;            /* request */
	struct nfsd4_change_info  li_cinfo; /* response */
};

struct nfsd4_lock_denied {
	clientid_t	ld_clientid;
	struct xdr_netobj	ld_owner;
	u64             ld_start;
	u64             ld_length;
	u32             ld_type;
};

struct nfsd4_lock {
	/* request */
	u32             lk_type;
	u32             lk_reclaim;         /* boolean */
	u64             lk_offset;
	u64             lk_length;
	u32             lk_is_new;
	union {
		struct {
			u32             open_seqid;
			stateid_t       open_stateid;
			u32             lock_seqid;
			clientid_t      clientid;
			struct xdr_netobj owner;
		} new;
		struct {
			stateid_t       lock_stateid;
			u32             lock_seqid;
		} old;
	} v;

	/* response */
	union {
		struct {
			stateid_t               stateid;
		} ok;
		struct nfsd4_lock_denied        denied;
	} u;
};
#define lk_new_open_seqid       v.new.open_seqid
#define lk_new_open_stateid     v.new.open_stateid
#define lk_new_lock_seqid       v.new.lock_seqid
#define lk_new_clientid         v.new.clientid
#define lk_new_owner            v.new.owner
#define lk_old_lock_stateid     v.old.lock_stateid
#define lk_old_lock_seqid       v.old.lock_seqid

#define lk_resp_stateid u.ok.stateid
#define lk_denied       u.denied


struct nfsd4_lockt {
	u32				lt_type;
	clientid_t			lt_clientid;
	struct xdr_netobj		lt_owner;
	u64				lt_offset;
	u64				lt_length;
	struct nfsd4_lock_denied  	lt_denied;
};

 
struct nfsd4_locku {
	u32             lu_type;
	u32             lu_seqid;
	stateid_t       lu_stateid;
	u64             lu_offset;
	u64             lu_length;
};


struct nfsd4_lookup {
	u32		lo_len;             /* request */
	char *		lo_name;            /* request */
};

struct nfsd4_putfh {
	u32		pf_fhlen;           /* request */
	char		*pf_fhval;          /* request */
	bool		no_verify;	    /* represents foreigh fh */
};

struct nfsd4_getxattr {
	char		*getxa_name;		/* request */
	u32		getxa_len;		/* request */
	void		*getxa_buf;
};

struct nfsd4_setxattr {
	u32		setxa_flags;		/* request */
	char		*setxa_name;		/* request */
	char		*setxa_buf;		/* request */
	u32		setxa_len;		/* request */
	struct nfsd4_change_info  setxa_cinfo;	/* response */
};

struct nfsd4_removexattr {
	char		*rmxa_name;		/* request */
	struct nfsd4_change_info  rmxa_cinfo;	/* response */
};

struct nfsd4_listxattrs {
	u64		lsxa_cookie;		/* request */
	u32		lsxa_maxcount;		/* request */
	char		*lsxa_buf;		/* unfiltered buffer (reply) */
	u32		lsxa_len;		/* unfiltered len (reply) */
};

struct nfsd4_open {
	u32		op_claim_type;      /* request */
	u32		op_fnamelen;
	char *		op_fname;	    /* request - everything but CLAIM_PREV */
	u32		op_delegate_type;   /* request - CLAIM_PREV only */
	stateid_t       op_delegate_stateid; /* request - response */
	u32		op_why_no_deleg;    /* response - DELEG_NONE_EXT only */
	u32		op_create;     	    /* request */
	u32		op_createmode;      /* request */
	int		op_umask;           /* request */
	u32		op_bmval[3];        /* request */
	struct iattr	op_iattr;           /* UNCHECKED4, GUARDED4, EXCLUSIVE4_1 */
	nfs4_verifier	op_verf __attribute__((aligned(32)));
					    /* EXCLUSIVE4 */
	clientid_t	op_clientid;        /* request */
	struct xdr_netobj op_owner;           /* request */
	u32		op_seqid;           /* request */
	u32		op_share_access;    /* request */
	u32		op_share_deny;      /* request */
	u32		op_deleg_want;      /* request */
	stateid_t	op_stateid;         /* response */
	__be32		op_xdr_error;       /* see nfsd4_open_omfg() */
	u32		op_recall;          /* recall */
	struct nfsd4_change_info  op_cinfo; /* response */
	u32		op_rflags;          /* response */
	bool		op_truncate;        /* used during processing */
	bool		op_created;         /* used during processing */
	struct nfs4_openowner *op_openowner; /* used during processing */
	struct nfs4_file *op_file;          /* used during processing */
	struct nfs4_ol_stateid *op_stp;	    /* used during processing */
	struct nfs4_clnt_odstate *op_odstate; /* used during processing */
	struct nfs4_acl *op_acl;
	struct xdr_netobj op_label;
};

struct nfsd4_open_confirm {
	stateid_t	oc_req_stateid		/* request */;
	u32		oc_seqid    		/* request */;
	stateid_t	oc_resp_stateid		/* response */;
};

struct nfsd4_open_downgrade {
	stateid_t       od_stateid;
	u32             od_seqid;
	u32             od_share_access;	/* request */
	u32		od_deleg_want;		/* request */
	u32             od_share_deny;		/* request */
};


struct nfsd4_read {
	stateid_t		rd_stateid;         /* request */
	u64			rd_offset;          /* request */
	u32			rd_length;          /* request */
	int			rd_vlen;
	struct nfsd_file	*rd_nf;
	
	struct svc_rqst		*rd_rqstp;          /* response */
	struct svc_fh		*rd_fhp;             /* response */
};

struct nfsd4_readdir {
	u64		rd_cookie;          /* request */
	nfs4_verifier	rd_verf;            /* request */
	u32		rd_dircount;        /* request */
	u32		rd_maxcount;        /* request */
	u32		rd_bmval[3];        /* request */
	struct svc_rqst *rd_rqstp;          /* response */
	struct svc_fh * rd_fhp;             /* response */

	struct readdir_cd	common;
	struct xdr_stream	*xdr;
	int			cookie_offset;
};

struct nfsd4_release_lockowner {
	clientid_t        rl_clientid;
	struct xdr_netobj rl_owner;
};
struct nfsd4_readlink {
	struct svc_rqst *rl_rqstp;          /* request */
	struct svc_fh *	rl_fhp;             /* request */
};

struct nfsd4_remove {
	u32		rm_namelen;         /* request */
	char *		rm_name;            /* request */
	struct nfsd4_change_info  rm_cinfo; /* response */
};

struct nfsd4_rename {
	u32		rn_snamelen;        /* request */
	char *		rn_sname;           /* request */
	u32		rn_tnamelen;        /* request */
	char *		rn_tname;           /* request */
	struct nfsd4_change_info  rn_sinfo; /* response */
	struct nfsd4_change_info  rn_tinfo; /* response */
};

struct nfsd4_secinfo {
	u32 si_namelen;					/* request */
	char *si_name;					/* request */
	struct svc_export *si_exp;			/* response */
};

struct nfsd4_secinfo_no_name {
	u32 sin_style;					/* request */
	struct svc_export *sin_exp;			/* response */
};

struct nfsd4_setattr {
	stateid_t	sa_stateid;         /* request */
	u32		sa_bmval[3];        /* request */
	struct iattr	sa_iattr;           /* request */
	struct nfs4_acl *sa_acl;
	struct xdr_netobj sa_label;
};

struct nfsd4_setclientid {
	nfs4_verifier	se_verf;            /* request */
	struct xdr_netobj se_name;
	u32		se_callback_prog;   /* request */
	u32		se_callback_netid_len;  /* request */
	char *		se_callback_netid_val;  /* request */
	u32		se_callback_addr_len;   /* request */
	char *		se_callback_addr_val;   /* request */
	u32		se_callback_ident;  /* request */
	clientid_t	se_clientid;        /* response */
	nfs4_verifier	se_confirm;         /* response */
};

struct nfsd4_setclientid_confirm {
	clientid_t	sc_clientid;
	nfs4_verifier	sc_confirm;
};

struct nfsd4_test_stateid_id {
	__be32			ts_id_status;
	stateid_t		ts_id_stateid;
	struct list_head	ts_id_list;
};

struct nfsd4_test_stateid {
	u32		ts_num_ids;
	struct list_head ts_stateid_list;
};

struct nfsd4_free_stateid {
	stateid_t	fr_stateid;         /* request */
};

/* also used for NVERIFY */
struct nfsd4_verify {
	u32		ve_bmval[3];        /* request */
	u32		ve_attrlen;         /* request */
	char *		ve_attrval;         /* request */
};

struct nfsd4_write {
	stateid_t	wr_stateid;         /* request */
	u64		wr_offset;          /* request */
	u32		wr_stable_how;      /* request */
	u32		wr_buflen;          /* request */
	struct xdr_buf	wr_payload;         /* request */

	u32		wr_bytes_written;   /* response */
	u32		wr_how_written;     /* response */
	nfs4_verifier	wr_verifier;        /* response */
};

struct nfsd4_exchange_id {
	nfs4_verifier	verifier;
	struct xdr_netobj clname;
	u32		flags;
	clientid_t	clientid;
	u32		seqid;
	u32		spa_how;
	u32             spo_must_enforce[3];
	u32             spo_must_allow[3];
	struct xdr_netobj nii_domain;
	struct xdr_netobj nii_name;
	struct timespec64 nii_time;
};

struct nfsd4_sequence {
	struct nfs4_sessionid	sessionid;		/* request/response */
	u32			seqid;			/* request/response */
	u32			slotid;			/* request/response */
	u32			maxslots;		/* request/response */
	u32			cachethis;		/* request */
#if 0
	u32			target_maxslots;	/* response */
#endif /* not yet */
	u32			status_flags;		/* response */
};

struct nfsd4_destroy_session {
	struct nfs4_sessionid	sessionid;
};

struct nfsd4_destroy_clientid {
	clientid_t clientid;
};

struct nfsd4_reclaim_complete {
	u32 rca_one_fs;
};

struct nfsd4_deviceid {
	u64			fsid_idx;
	u32			generation;
	u32			pad;
};

struct nfsd4_layout_seg {
	u32			iomode;
	u64			offset;
	u64			length;
};

struct nfsd4_getdeviceinfo {
	struct nfsd4_deviceid	gd_devid;	/* request */
	u32			gd_layout_type;	/* request */
	u32			gd_maxcount;	/* request */
	u32			gd_notify_types;/* request - response */
	void			*gd_device;	/* response */
};

struct nfsd4_layoutget {
	u64			lg_minlength;	/* request */
	u32			lg_signal;	/* request */
	u32			lg_layout_type;	/* request */
	u32			lg_maxcount;	/* request */
	stateid_t		lg_sid;		/* request/response */
	struct nfsd4_layout_seg	lg_seg;		/* request/response */
	void			*lg_content;	/* response */
};

struct nfsd4_layoutcommit {
	stateid_t		lc_sid;		/* request */
	struct nfsd4_layout_seg	lc_seg;		/* request */
	u32			lc_reclaim;	/* request */
	u32			lc_newoffset;	/* request */
	u64			lc_last_wr;	/* request */
	struct timespec64	lc_mtime;	/* request */
	u32			lc_layout_type;	/* request */
	u32			lc_up_len;	/* layout length */
	void			*lc_up_layout;	/* decoded by callback */
	u32			lc_size_chg;	/* boolean for response */
	u64			lc_newsize;	/* response */
};

struct nfsd4_layoutreturn {
	u32			lr_return_type;	/* request */
	u32			lr_layout_type;	/* request */
	struct nfsd4_layout_seg	lr_seg;		/* request */
	u32			lr_reclaim;	/* request */
	u32			lrf_body_len;	/* request */
	void			*lrf_body;	/* request */
	stateid_t		lr_sid;		/* request/response */
	u32			lrs_present;	/* response */
};

struct nfsd4_fallocate {
	/* request */
	stateid_t	falloc_stateid;
	loff_t		falloc_offset;
	u64		falloc_length;
};

struct nfsd4_clone {
	/* request */
	stateid_t	cl_src_stateid;
	stateid_t	cl_dst_stateid;
	u64		cl_src_pos;
	u64		cl_dst_pos;
	u64		cl_count;
};

struct nfsd42_write_res {
	u64			wr_bytes_written;
	u32			wr_stable_how;
	nfs4_verifier		wr_verifier;
	stateid_t		cb_stateid;
};

struct nfsd4_copy {
	/* request */
	stateid_t		cp_src_stateid;
	stateid_t		cp_dst_stateid;
	u64			cp_src_pos;
	u64			cp_dst_pos;
	u64			cp_count;
	struct nl4_server	cp_src;
	bool			cp_intra;

	/* both */
	u32			cp_synchronous;

	/* response */
	struct nfsd42_write_res	cp_res;

	/* for cb_offload */
	struct nfsd4_callback	cp_cb;
	__be32			nfserr;
	struct knfsd_fh		fh;

	struct nfs4_client      *cp_clp;

	struct nfsd_file        *nf_src;
	struct nfsd_file        *nf_dst;

	copy_stateid_t		cp_stateid;

	struct list_head	copies;
	struct task_struct	*copy_task;
	refcount_t		refcount;
	bool			stopped;

	struct vfsmount		*ss_mnt;
	struct nfs_fh		c_fh;
	nfs4_stateid		stateid;
};

struct nfsd4_seek {
	/* request */
	stateid_t	seek_stateid;
	loff_t		seek_offset;
	u32		seek_whence;

	/* response */
	u32		seek_eof;
	loff_t		seek_pos;
};

struct nfsd4_offload_status {
	/* request */
	stateid_t	stateid;

	/* response */
	u64		count;
	u32		status;
};

struct nfsd4_copy_notify {
	/* request */
	stateid_t		cpn_src_stateid;
	struct nl4_server	cpn_dst;

	/* response */
	stateid_t		cpn_cnr_stateid;
	u64			cpn_sec;
	u32			cpn_nsec;
	struct nl4_server	cpn_src;
};

struct nfsd4_op {
	u32					opnum;
	const struct nfsd4_operation *		opdesc;
	__be32					status;
	union nfsd4_op_u {
		struct nfsd4_access		access;
		struct nfsd4_close		close;
		struct nfsd4_commit		commit;
		struct nfsd4_create		create;
		struct nfsd4_delegreturn	delegreturn;
		struct nfsd4_getattr		getattr;
		struct svc_fh *			getfh;
		struct nfsd4_link		link;
		struct nfsd4_lock		lock;
		struct nfsd4_lockt		lockt;
		struct nfsd4_locku		locku;
		struct nfsd4_lookup		lookup;
		struct nfsd4_verify		nverify;
		struct nfsd4_open		open;
		struct nfsd4_open_confirm	open_confirm;
		struct nfsd4_open_downgrade	open_downgrade;
		struct nfsd4_putfh		putfh;
		struct nfsd4_read		read;
		struct nfsd4_readdir		readdir;
		struct nfsd4_readlink		readlink;
		struct nfsd4_remove		remove;
		struct nfsd4_rename		rename;
		clientid_t			renew;
		struct nfsd4_secinfo		secinfo;
		struct nfsd4_setattr		setattr;
		struct nfsd4_setclientid	setclientid;
		struct nfsd4_setclientid_confirm setclientid_confirm;
		struct nfsd4_verify		verify;
		struct nfsd4_write		write;
		struct nfsd4_release_lockowner	release_lockowner;

		/* NFSv4.1 */
		struct nfsd4_exchange_id	exchange_id;
		struct nfsd4_backchannel_ctl	backchannel_ctl;
		struct nfsd4_bind_conn_to_session bind_conn_to_session;
		struct nfsd4_create_session	create_session;
		struct nfsd4_destroy_session	destroy_session;
		struct nfsd4_destroy_clientid	destroy_clientid;
		struct nfsd4_sequence		sequence;
		struct nfsd4_reclaim_complete	reclaim_complete;
		struct nfsd4_test_stateid	test_stateid;
		struct nfsd4_free_stateid	free_stateid;
		struct nfsd4_getdeviceinfo	getdeviceinfo;
		struct nfsd4_layoutget		layoutget;
		struct nfsd4_layoutcommit	layoutcommit;
		struct nfsd4_layoutreturn	layoutreturn;
		struct nfsd4_secinfo_no_name	secinfo_no_name;

		/* NFSv4.2 */
		struct nfsd4_fallocate		allocate;
		struct nfsd4_fallocate		deallocate;
		struct nfsd4_clone		clone;
		struct nfsd4_copy		copy;
		struct nfsd4_offload_status	offload_status;
		struct nfsd4_copy_notify	copy_notify;
		struct nfsd4_seek		seek;

		struct nfsd4_getxattr		getxattr;
		struct nfsd4_setxattr		setxattr;
		struct nfsd4_listxattrs		listxattrs;
		struct nfsd4_removexattr	removexattr;
	} u;
	struct nfs4_replay *			replay;
};

bool nfsd4_cache_this_op(struct nfsd4_op *);

/*
 * Memory needed just for the duration of processing one compound:
 */
struct svcxdr_tmpbuf {
	struct svcxdr_tmpbuf *next;
	char buf[];
};

struct nfsd4_compoundargs {
	/* scratch variables for XDR decode */
	struct xdr_stream		*xdr;
	struct svcxdr_tmpbuf		*to_free;
	struct svc_rqst			*rqstp;

	u32				taglen;
	char *				tag;
	u32				minorversion;
	u32				opcnt;
	struct nfsd4_op			*ops;
	struct nfsd4_op			iops[8];
	int				cachetype;
};

struct nfsd4_compoundres {
	/* scratch variables for XDR encode */
	struct xdr_stream		*xdr;
	struct svc_rqst *		rqstp;

	u32				taglen;
	char *				tag;
	u32				opcnt;
	__be32 *			tagp; /* tag, opcount encode location */
	struct nfsd4_compound_state	cstate;
};

static inline bool nfsd4_is_solo_sequence(struct nfsd4_compoundres *resp)
{
	struct nfsd4_compoundargs *args = resp->rqstp->rq_argp;
	return resp->opcnt == 1 && args->ops[0].opnum == OP_SEQUENCE;
}

/*
 * The session reply cache only needs to cache replies that the client
 * actually asked us to.  But it's almost free for us to cache compounds
 * consisting of only a SEQUENCE op, so we may as well cache those too.
 * Also, the protocol doesn't give us a convenient response in the case
 * of a replay of a solo SEQUENCE op that wasn't cached
 * (RETRY_UNCACHED_REP can only be returned in the second op of a
 * compound).
 */
static inline bool nfsd4_cache_this(struct nfsd4_compoundres *resp)
{
	return (resp->cstate.slot->sl_flags & NFSD4_SLOT_CACHETHIS)
		|| nfsd4_is_solo_sequence(resp);
}

static inline bool nfsd4_last_compound_op(struct svc_rqst *rqstp)
{
	struct nfsd4_compoundres *resp = rqstp->rq_resp;
	struct nfsd4_compoundargs *argp = rqstp->rq_argp;

	return argp->opcnt == resp->opcnt;
}

const struct nfsd4_operation *OPDESC(struct nfsd4_op *op);
int nfsd4_max_reply(struct svc_rqst *rqstp, struct nfsd4_op *op);
void warn_on_nonidempotent_op(struct nfsd4_op *op);

#define NFS4_SVC_XDRSIZE		sizeof(struct nfsd4_compoundargs)

static inline void
set_change_info(struct nfsd4_change_info *cinfo, struct svc_fh *fhp)
{
	BUG_ON(!fhp->fh_pre_saved);
	cinfo->atomic = (u32)(fhp->fh_post_saved && !fhp->fh_no_atomic_attr);

	cinfo->before_change = fhp->fh_pre_change;
	cinfo->after_change = fhp->fh_post_change;
}


bool nfsd4_mach_creds_match(struct nfs4_client *cl, struct svc_rqst *rqstp);
int nfs4svc_decode_compoundargs(struct svc_rqst *, __be32 *);
int nfs4svc_encode_compoundres(struct svc_rqst *, __be32 *);
__be32 nfsd4_check_resp_size(struct nfsd4_compoundres *, u32);
void nfsd4_encode_operation(struct nfsd4_compoundres *, struct nfsd4_op *);
void nfsd4_encode_replay(struct xdr_stream *xdr, struct nfsd4_op *op);
__be32 nfsd4_encode_fattr_to_buf(__be32 **p, int words,
		struct svc_fh *fhp, struct svc_export *exp,
		struct dentry *dentry,
		u32 *bmval, struct svc_rqst *, int ignore_crossmnt);
extern __be32 nfsd4_setclientid(struct svc_rqst *rqstp,
		struct nfsd4_compound_state *, union nfsd4_op_u *u);
extern __be32 nfsd4_setclientid_confirm(struct svc_rqst *rqstp,
		struct nfsd4_compound_state *, union nfsd4_op_u *u);
extern __be32 nfsd4_exchange_id(struct svc_rqst *rqstp,
		struct nfsd4_compound_state *, union nfsd4_op_u *u);
extern __be32 nfsd4_backchannel_ctl(struct svc_rqst *,
		struct nfsd4_compound_state *, union nfsd4_op_u *u);
extern __be32 nfsd4_bind_conn_to_session(struct svc_rqst *,
		struct nfsd4_compound_state *, union nfsd4_op_u *u);
extern __be32 nfsd4_create_session(struct svc_rqst *,
		struct nfsd4_compound_state *, union nfsd4_op_u *u);
extern __be32 nfsd4_sequence(struct svc_rqst *,
		struct nfsd4_compound_state *, union nfsd4_op_u *u);
extern void nfsd4_sequence_done(struct nfsd4_compoundres *resp);
extern __be32 nfsd4_destroy_session(struct svc_rqst *,
		struct nfsd4_compound_state *, union nfsd4_op_u *u);
extern __be32 nfsd4_destroy_clientid(struct svc_rqst *, struct nfsd4_compound_state *,
		union nfsd4_op_u *u);
__be32 nfsd4_reclaim_complete(struct svc_rqst *, struct nfsd4_compound_state *,
		union nfsd4_op_u *u);
extern __be32 nfsd4_process_open1(struct nfsd4_compound_state *,
		struct nfsd4_open *open, struct nfsd_net *nn);
extern __be32 nfsd4_process_open2(struct svc_rqst *rqstp,
		struct svc_fh *current_fh, struct nfsd4_open *open);
extern void nfsd4_cstate_clear_replay(struct nfsd4_compound_state *cstate);
extern void nfsd4_cleanup_open_state(struct nfsd4_compound_state *cstate,
		struct nfsd4_open *open);
extern __be32 nfsd4_open_confirm(struct svc_rqst *rqstp,
		struct nfsd4_compound_state *, union nfsd4_op_u *u);
extern __be32 nfsd4_close(struct svc_rqst *rqstp, struct nfsd4_compound_state *,
		union nfsd4_op_u *u);
extern __be32 nfsd4_open_downgrade(struct svc_rqst *rqstp,
		struct nfsd4_compound_state *, union nfsd4_op_u *u);
extern __be32 nfsd4_lock(struct svc_rqst *rqstp, struct nfsd4_compound_state *,
		union nfsd4_op_u *u);
extern __be32 nfsd4_lockt(struct svc_rqst *rqstp, struct nfsd4_compound_state *,
		union nfsd4_op_u *u);
extern __be32 nfsd4_locku(struct svc_rqst *rqstp, struct nfsd4_compound_state *,
		union nfsd4_op_u *u);
extern __be32
nfsd4_release_lockowner(struct svc_rqst *rqstp,
		struct nfsd4_compound_state *, union nfsd4_op_u *u);
extern void nfsd4_release_compoundargs(struct svc_rqst *rqstp);
extern __be32 nfsd4_delegreturn(struct svc_rqst *rqstp,
		struct nfsd4_compound_state *, union nfsd4_op_u *u);
extern __be32 nfsd4_renew(struct svc_rqst *rqstp, struct nfsd4_compound_state *,
		union nfsd4_op_u *u);
extern __be32 nfsd4_test_stateid(struct svc_rqst *rqstp,
		struct nfsd4_compound_state *, union nfsd4_op_u *);
extern __be32 nfsd4_free_stateid(struct svc_rqst *rqstp,
		struct nfsd4_compound_state *, union nfsd4_op_u *);
extern void nfsd4_bump_seqid(struct nfsd4_compound_state *, __be32 nfserr);

enum nfsd4_op_flags {
	ALLOWED_WITHOUT_FH = 1 << 0,    /* No current filehandle required */
	ALLOWED_ON_ABSENT_FS = 1 << 1,  /* ops processed on absent fs */
	ALLOWED_AS_FIRST_OP = 1 << 2,   /* ops reqired first in compound */
	/* For rfc 5661 section 2.6.3.1.1: */
	OP_HANDLES_WRONGSEC = 1 << 3,
	OP_IS_PUTFH_LIKE = 1 << 4,
	/*
	 * These are the ops whose result size we estimate before
	 * encoding, to avoid performing an op then not being able to
	 * respond or cache a response.  This includes writes and setattrs
	 * as well as the operations usually called "nonidempotent":
	 */
	OP_MODIFIES_SOMETHING = 1 << 5,
	/*
	 * Cache compounds containing these ops in the xid-based drc:
	 * We use the DRC for compounds containing non-idempotent
	 * operations, *except* those that are 4.1-specific (since
	 * sessions provide their own EOS), and except for stateful
	 * operations other than setclientid and setclientid_confirm
	 * (since sequence numbers provide EOS for open, lock, etc in
	 * the v4.0 case).
	 */
	OP_CACHEME = 1 << 6,
	/*
	 * These are ops which clear current state id.
	 */
	OP_CLEAR_STATEID = 1 << 7,
	/* Most ops return only an error on failure; some may do more: */
	OP_NONTRIVIAL_ERROR_ENCODE = 1 << 8,
};

struct nfsd4_operation {
	__be32 (*op_func)(struct svc_rqst *, struct nfsd4_compound_state *,
			union nfsd4_op_u *);
	void (*op_release)(union nfsd4_op_u *);
	u32 op_flags;
	char *op_name;
	/* Try to get response size before operation */
	u32 (*op_rsize_bop)(struct svc_rqst *, struct nfsd4_op *);
	void (*op_get_currentstateid)(struct nfsd4_compound_state *,
			union nfsd4_op_u *);
	void (*op_set_currentstateid)(struct nfsd4_compound_state *,
			union nfsd4_op_u *);
};


#endif
