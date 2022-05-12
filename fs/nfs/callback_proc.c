// SPDX-License-Identifier: GPL-2.0
/*
 * linux/fs/nfs/callback_proc.c
 *
 * Copyright (C) 2004 Trond Myklebust
 *
 * NFSv4 callback procedures
 */

#include <linux/errno.h>
#include <linux/math.h>
#include <linux/nfs4.h>
#include <linux/nfs_fs.h>
#include <linux/slab.h>
#include <linux/rcupdate.h>
#include <linux/types.h>

#include "nfs4_fs.h"
#include "callback.h"
#include "delegation.h"
#include "internal.h"
#include "pnfs.h"
#include "nfs4session.h"
#include "nfs4trace.h"

#define NFSDBG_FACILITY NFSDBG_CALLBACK

__be32 nfs4_callback_getattr(void *argp, void *resp,
			     struct cb_process_state *cps)
{
	struct cb_getattrargs *args = argp;
	struct cb_getattrres *res = resp;
	struct nfs_delegation *delegation;
	struct inode *inode;

	res->status = htonl(NFS4ERR_OP_NOT_IN_SESSION);
	if (!cps->clp) /* Always set for v4.0. Set in cb_sequence for v4.1 */
		goto out;

	res->bitmap[0] = res->bitmap[1] = 0;
	res->status = htonl(NFS4ERR_BADHANDLE);

	dprintk_rcu("NFS: GETATTR callback request from %s\n",
		rpc_peeraddr2str(cps->clp->cl_rpcclient, RPC_DISPLAY_ADDR));

	inode = nfs_delegation_find_inode(cps->clp, &args->fh);
	if (IS_ERR(inode)) {
		if (inode == ERR_PTR(-EAGAIN))
			res->status = htonl(NFS4ERR_DELAY);
		trace_nfs4_cb_getattr(cps->clp, &args->fh, NULL,
				-ntohl(res->status));
		goto out;
	}
	rcu_read_lock();
	delegation = nfs4_get_valid_delegation(inode);
	if (delegation == NULL || (delegation->type & FMODE_WRITE) == 0)
		goto out_iput;
	res->size = i_size_read(inode);
	res->change_attr = delegation->change_attr;
	if (nfs_have_writebacks(inode))
		res->change_attr++;
	res->ctime = inode->i_ctime;
	res->mtime = inode->i_mtime;
	res->bitmap[0] = (FATTR4_WORD0_CHANGE|FATTR4_WORD0_SIZE) &
		args->bitmap[0];
	res->bitmap[1] = (FATTR4_WORD1_TIME_METADATA|FATTR4_WORD1_TIME_MODIFY) &
		args->bitmap[1];
	res->status = 0;
out_iput:
	rcu_read_unlock();
	trace_nfs4_cb_getattr(cps->clp, &args->fh, inode, -ntohl(res->status));
	nfs_iput_and_deactive(inode);
out:
	dprintk("%s: exit with status = %d\n", __func__, ntohl(res->status));
	return res->status;
}

__be32 nfs4_callback_recall(void *argp, void *resp,
			    struct cb_process_state *cps)
{
	struct cb_recallargs *args = argp;
	struct inode *inode;
	__be32 res;
	
	res = htonl(NFS4ERR_OP_NOT_IN_SESSION);
	if (!cps->clp) /* Always set for v4.0. Set in cb_sequence for v4.1 */
		goto out;

	dprintk_rcu("NFS: RECALL callback request from %s\n",
		rpc_peeraddr2str(cps->clp->cl_rpcclient, RPC_DISPLAY_ADDR));

	res = htonl(NFS4ERR_BADHANDLE);
	inode = nfs_delegation_find_inode(cps->clp, &args->fh);
	if (IS_ERR(inode)) {
		if (inode == ERR_PTR(-EAGAIN))
			res = htonl(NFS4ERR_DELAY);
		trace_nfs4_cb_recall(cps->clp, &args->fh, NULL,
				&args->stateid, -ntohl(res));
		goto out;
	}
	/* Set up a helper thread to actually return the delegation */
	switch (nfs_async_inode_return_delegation(inode, &args->stateid)) {
	case 0:
		res = 0;
		break;
	case -ENOENT:
		res = htonl(NFS4ERR_BAD_STATEID);
		break;
	default:
		res = htonl(NFS4ERR_RESOURCE);
	}
	trace_nfs4_cb_recall(cps->clp, &args->fh, inode,
			&args->stateid, -ntohl(res));
	nfs_iput_and_deactive(inode);
out:
	dprintk("%s: exit with status = %d\n", __func__, ntohl(res));
	return res;
}

#if defined(CONFIG_NFS_V4_1)

/*
 * Lookup a layout inode by stateid
 *
 * Note: returns a refcount on the inode and superblock
 */
static struct inode *nfs_layout_find_inode_by_stateid(struct nfs_client *clp,
		const nfs4_stateid *stateid)
	__must_hold(RCU)
{
	struct nfs_server *server;
	struct inode *inode;
	struct pnfs_layout_hdr *lo;

	rcu_read_lock();
	list_for_each_entry_rcu(server, &clp->cl_superblocks, client_link) {
		list_for_each_entry_rcu(lo, &server->layouts, plh_layouts) {
			if (!pnfs_layout_is_valid(lo))
				continue;
			if (!nfs4_stateid_match_other(stateid, &lo->plh_stateid))
				continue;
			if (nfs_sb_active(server->super))
				inode = igrab(lo->plh_inode);
			else
				inode = ERR_PTR(-EAGAIN);
			rcu_read_unlock();
			if (inode)
				return inode;
			nfs_sb_deactive(server->super);
			return ERR_PTR(-EAGAIN);
		}
	}
	rcu_read_unlock();
	return ERR_PTR(-ENOENT);
}

/*
 * Lookup a layout inode by filehandle.
 *
 * Note: returns a refcount on the inode and superblock
 *
 */
static struct inode *nfs_layout_find_inode_by_fh(struct nfs_client *clp,
		const struct nfs_fh *fh)
{
	struct nfs_server *server;
	struct nfs_inode *nfsi;
	struct inode *inode;
	struct pnfs_layout_hdr *lo;

	rcu_read_lock();
	list_for_each_entry_rcu(server, &clp->cl_superblocks, client_link) {
		list_for_each_entry_rcu(lo, &server->layouts, plh_layouts) {
			nfsi = NFS_I(lo->plh_inode);
			if (nfs_compare_fh(fh, &nfsi->fh))
				continue;
			if (nfsi->layout != lo)
				continue;
			if (nfs_sb_active(server->super))
				inode = igrab(lo->plh_inode);
			else
				inode = ERR_PTR(-EAGAIN);
			rcu_read_unlock();
			if (inode)
				return inode;
			nfs_sb_deactive(server->super);
			return ERR_PTR(-EAGAIN);
		}
	}
	rcu_read_unlock();
	return ERR_PTR(-ENOENT);
}

static struct inode *nfs_layout_find_inode(struct nfs_client *clp,
		const struct nfs_fh *fh,
		const nfs4_stateid *stateid)
{
	struct inode *inode;

	inode = nfs_layout_find_inode_by_stateid(clp, stateid);
	if (inode == ERR_PTR(-ENOENT))
		inode = nfs_layout_find_inode_by_fh(clp, fh);
	return inode;
}

/*
 * Enforce RFC5661 section 12.5.5.2.1. (Layout Recall and Return Sequencing)
 */
static u32 pnfs_check_callback_stateid(struct pnfs_layout_hdr *lo,
					const nfs4_stateid *new)
{
	u32 oldseq, newseq;

	/* Is the stateid not initialised? */
	if (!pnfs_layout_is_valid(lo))
		return NFS4ERR_NOMATCHING_LAYOUT;

	/* Mismatched stateid? */
	if (!nfs4_stateid_match_other(&lo->plh_stateid, new))
		return NFS4ERR_BAD_STATEID;

	newseq = be32_to_cpu(new->seqid);
	/* Are we already in a layout recall situation? */
	if (test_bit(NFS_LAYOUT_RETURN_REQUESTED, &lo->plh_flags) &&
	    lo->plh_return_seq != 0) {
		if (newseq < lo->plh_return_seq)
			return NFS4ERR_OLD_STATEID;
		if (newseq > lo->plh_return_seq)
			return NFS4ERR_DELAY;
		goto out;
	}

	/* Check that the stateid matches what we think it should be. */
	oldseq = be32_to_cpu(lo->plh_stateid.seqid);
	if (newseq > oldseq + 1)
		return NFS4ERR_DELAY;
	/* Crazy server! */
	if (newseq <= oldseq)
		return NFS4ERR_OLD_STATEID;
out:
	return NFS_OK;
}

static u32 initiate_file_draining(struct nfs_client *clp,
				  struct cb_layoutrecallargs *args)
{
	struct inode *ino;
	struct pnfs_layout_hdr *lo;
	u32 rv = NFS4ERR_NOMATCHING_LAYOUT;
	LIST_HEAD(free_me_list);

	ino = nfs_layout_find_inode(clp, &args->cbl_fh, &args->cbl_stateid);
	if (IS_ERR(ino)) {
		if (ino == ERR_PTR(-EAGAIN))
			rv = NFS4ERR_DELAY;
		goto out_noput;
	}

	pnfs_layoutcommit_inode(ino, false);


	spin_lock(&ino->i_lock);
	lo = NFS_I(ino)->layout;
	if (!lo) {
		spin_unlock(&ino->i_lock);
		goto out;
	}
	pnfs_get_layout_hdr(lo);
	rv = pnfs_check_callback_stateid(lo, &args->cbl_stateid);
	if (rv != NFS_OK)
		goto unlock;

	/*
	 * Enforce RFC5661 Section 12.5.5.2.1.5 (Bulk Recall and Return)
	 */
	if (test_bit(NFS_LAYOUT_BULK_RECALL, &lo->plh_flags)) {
		rv = NFS4ERR_DELAY;
		goto unlock;
	}

	pnfs_set_layout_stateid(lo, &args->cbl_stateid, NULL, true);
	switch (pnfs_mark_matching_lsegs_return(lo, &free_me_list,
				&args->cbl_range,
				be32_to_cpu(args->cbl_stateid.seqid))) {
	case 0:
	case -EBUSY:
		/* There are layout segments that need to be returned */
		rv = NFS4_OK;
		break;
	case -ENOENT:
		/* Embrace your forgetfulness! */
		rv = NFS4ERR_NOMATCHING_LAYOUT;

		if (NFS_SERVER(ino)->pnfs_curr_ld->return_range) {
			NFS_SERVER(ino)->pnfs_curr_ld->return_range(lo,
				&args->cbl_range);
		}
	}
unlock:
	spin_unlock(&ino->i_lock);
	pnfs_free_lseg_list(&free_me_list);
	/* Free all lsegs that are attached to commit buckets */
	nfs_commit_inode(ino, 0);
	pnfs_put_layout_hdr(lo);
out:
	nfs_iput_and_deactive(ino);
out_noput:
	trace_nfs4_cb_layoutrecall_file(clp, &args->cbl_fh, ino,
			&args->cbl_stateid, -rv);
	return rv;
}

static u32 initiate_bulk_draining(struct nfs_client *clp,
				  struct cb_layoutrecallargs *args)
{
	int stat;

	if (args->cbl_recall_type == RETURN_FSID)
		stat = pnfs_destroy_layouts_byfsid(clp, &args->cbl_fsid, true);
	else
		stat = pnfs_destroy_layouts_byclid(clp, true);
	if (stat != 0)
		return NFS4ERR_DELAY;
	return NFS4ERR_NOMATCHING_LAYOUT;
}

static u32 do_callback_layoutrecall(struct nfs_client *clp,
				    struct cb_layoutrecallargs *args)
{
	if (args->cbl_recall_type == RETURN_FILE)
		return initiate_file_draining(clp, args);
	return initiate_bulk_draining(clp, args);
}

__be32 nfs4_callback_layoutrecall(void *argp, void *resp,
				  struct cb_process_state *cps)
{
	struct cb_layoutrecallargs *args = argp;
	u32 res = NFS4ERR_OP_NOT_IN_SESSION;

	if (cps->clp)
		res = do_callback_layoutrecall(cps->clp, args);
	return cpu_to_be32(res);
}

static void pnfs_recall_all_layouts(struct nfs_client *clp)
{
	struct cb_layoutrecallargs args;

	/* Pretend we got a CB_LAYOUTRECALL(ALL) */
	memset(&args, 0, sizeof(args));
	args.cbl_recall_type = RETURN_ALL;
	/* FIXME we ignore errors, what should we do? */
	do_callback_layoutrecall(clp, &args);
}

__be32 nfs4_callback_devicenotify(void *argp, void *resp,
				  struct cb_process_state *cps)
{
	struct cb_devicenotifyargs *args = argp;
	int i;
	__be32 res = 0;
	struct nfs_client *clp = cps->clp;
	struct nfs_server *server = NULL;

	if (!clp) {
		res = cpu_to_be32(NFS4ERR_OP_NOT_IN_SESSION);
		goto out;
	}

	for (i = 0; i < args->ndevs; i++) {
		struct cb_devicenotifyitem *dev = &args->devs[i];

		if (!server ||
		    server->pnfs_curr_ld->id != dev->cbd_layout_type) {
			rcu_read_lock();
			list_for_each_entry_rcu(server, &clp->cl_superblocks, client_link)
				if (server->pnfs_curr_ld &&
				    server->pnfs_curr_ld->id == dev->cbd_layout_type) {
					rcu_read_unlock();
					goto found;
				}
			rcu_read_unlock();
			continue;
		}

	found:
		nfs4_delete_deviceid(server->pnfs_curr_ld, clp, &dev->cbd_dev_id);
	}

out:
	kfree(args->devs);
	return res;
}

/*
 * Validate the sequenceID sent by the server.
 * Return success if the sequenceID is one more than what we last saw on
 * this slot, accounting for wraparound.  Increments the slot's sequence.
 *
 * We don't yet implement a duplicate request cache, instead we set the
 * back channel ca_maxresponsesize_cached to zero. This is OK for now
 * since we only currently implement idempotent callbacks anyway.
 *
 * We have a single slot backchannel at this time, so we don't bother
 * checking the used_slots bit array on the table.  The lower layer guarantees
 * a single outstanding callback request at a time.
 */
static __be32
validate_seqid(const struct nfs4_slot_table *tbl, const struct nfs4_slot *slot,
		const struct cb_sequenceargs * args)
{
	__be32 ret;

	ret = cpu_to_be32(NFS4ERR_BADSLOT);
	if (args->csa_slotid > tbl->server_highest_slotid)
		goto out_err;

	/* Replay */
	if (args->csa_sequenceid == slot->seq_nr) {
		ret = cpu_to_be32(NFS4ERR_DELAY);
		if (nfs4_test_locked_slot(tbl, slot->slot_nr))
			goto out_err;

		/* Signal process_op to set this error on next op */
		ret = cpu_to_be32(NFS4ERR_RETRY_UNCACHED_REP);
		if (args->csa_cachethis == 0)
			goto out_err;

		/* Liar! We never allowed you to set csa_cachethis != 0 */
		ret = cpu_to_be32(NFS4ERR_SEQ_FALSE_RETRY);
		goto out_err;
	}

	/* Note: wraparound relies on seq_nr being of type u32 */
	/* Misordered request */
	ret = cpu_to_be32(NFS4ERR_SEQ_MISORDERED);
	if (args->csa_sequenceid != slot->seq_nr + 1)
		goto out_err;

	return cpu_to_be32(NFS4_OK);

out_err:
	trace_nfs4_cb_seqid_err(args, ret);
	return ret;
}

/*
 * For each referring call triple, check the session's slot table for
 * a match.  If the slot is in use and the sequence numbers match, the
 * client is still waiting for a response to the original request.
 */
static int referring_call_exists(struct nfs_client *clp,
				  uint32_t nrclists,
				  struct referring_call_list *rclists,
				  spinlock_t *lock)
	__releases(lock)
	__acquires(lock)
{
	int status = 0;
	int i, j;
	struct nfs4_session *session;
	struct nfs4_slot_table *tbl;
	struct referring_call_list *rclist;
	struct referring_call *ref;

	/*
	 * XXX When client trunking is implemented, this becomes
	 * a session lookup from within the loop
	 */
	session = clp->cl_session;
	tbl = &session->fc_slot_table;

	for (i = 0; i < nrclists; i++) {
		rclist = &rclists[i];
		if (memcmp(session->sess_id.data,
			   rclist->rcl_sessionid.data,
			   NFS4_MAX_SESSIONID_LEN) != 0)
			continue;

		for (j = 0; j < rclist->rcl_nrefcalls; j++) {
			ref = &rclist->rcl_refcalls[j];
			spin_unlock(lock);
			status = nfs4_slot_wait_on_seqid(tbl, ref->rc_slotid,
					ref->rc_sequenceid, HZ >> 1) < 0;
			spin_lock(lock);
			if (status)
				goto out;
		}
	}

out:
	return status;
}

__be32 nfs4_callback_sequence(void *argp, void *resp,
			      struct cb_process_state *cps)
{
	struct cb_sequenceargs *args = argp;
	struct cb_sequenceres *res = resp;
	struct nfs4_slot_table *tbl;
	struct nfs4_slot *slot;
	struct nfs_client *clp;
	int i;
	__be32 status = htonl(NFS4ERR_BADSESSION);

	clp = nfs4_find_client_sessionid(cps->net, args->csa_addr,
					 &args->csa_sessionid, cps->minorversion);
	if (clp == NULL)
		goto out;

	if (!(clp->cl_session->flags & SESSION4_BACK_CHAN))
		goto out;

	tbl = &clp->cl_session->bc_slot_table;

	/* Set up res before grabbing the spinlock */
	memcpy(&res->csr_sessionid, &args->csa_sessionid,
	       sizeof(res->csr_sessionid));
	res->csr_sequenceid = args->csa_sequenceid;
	res->csr_slotid = args->csa_slotid;

	spin_lock(&tbl->slot_tbl_lock);
	/* state manager is resetting the session */
	if (test_bit(NFS4_SLOT_TBL_DRAINING, &tbl->slot_tbl_state)) {
		status = htonl(NFS4ERR_DELAY);
		/* Return NFS4ERR_BADSESSION if we're draining the session
		 * in order to reset it.
		 */
		if (test_bit(NFS4CLNT_SESSION_RESET, &clp->cl_state))
			status = htonl(NFS4ERR_BADSESSION);
		goto out_unlock;
	}

	status = htonl(NFS4ERR_BADSLOT);
	slot = nfs4_lookup_slot(tbl, args->csa_slotid);
	if (IS_ERR(slot))
		goto out_unlock;

	res->csr_highestslotid = tbl->server_highest_slotid;
	res->csr_target_highestslotid = tbl->target_highest_slotid;

	status = validate_seqid(tbl, slot, args);
	if (status)
		goto out_unlock;
	if (!nfs4_try_to_lock_slot(tbl, slot)) {
		status = htonl(NFS4ERR_DELAY);
		goto out_unlock;
	}
	cps->slot = slot;

	/* The ca_maxresponsesize_cached is 0 with no DRC */
	if (args->csa_cachethis != 0) {
		status = htonl(NFS4ERR_REP_TOO_BIG_TO_CACHE);
		goto out_unlock;
	}

	/*
	 * Check for pending referring calls.  If a match is found, a
	 * related callback was received before the response to the original
	 * call.
	 */
	if (referring_call_exists(clp, args->csa_nrclists, args->csa_rclists,
				&tbl->slot_tbl_lock) < 0) {
		status = htonl(NFS4ERR_DELAY);
		goto out_unlock;
	}

	/*
	 * RFC5661 20.9.3
	 * If CB_SEQUENCE returns an error, then the state of the slot
	 * (sequence ID, cached reply) MUST NOT change.
	 */
	slot->seq_nr = args->csa_sequenceid;
out_unlock:
	spin_unlock(&tbl->slot_tbl_lock);

out:
	cps->clp = clp; /* put in nfs4_callback_compound */
	for (i = 0; i < args->csa_nrclists; i++)
		kfree(args->csa_rclists[i].rcl_refcalls);
	kfree(args->csa_rclists);

	if (status == htonl(NFS4ERR_RETRY_UNCACHED_REP)) {
		cps->drc_status = status;
		status = 0;
	} else
		res->csr_status = status;

	trace_nfs4_cb_sequence(args, res, status);
	return status;
}

static bool
validate_bitmap_values(unsigned int mask)
{
	return (mask & ~RCA4_TYPE_MASK_ALL) == 0;
}

__be32 nfs4_callback_recallany(void *argp, void *resp,
			       struct cb_process_state *cps)
{
	struct cb_recallanyargs *args = argp;
	__be32 status;
	fmode_t flags = 0;
	bool schedule_manager = false;

	status = cpu_to_be32(NFS4ERR_OP_NOT_IN_SESSION);
	if (!cps->clp) /* set in cb_sequence */
		goto out;

	dprintk_rcu("NFS: RECALL_ANY callback request from %s\n",
		rpc_peeraddr2str(cps->clp->cl_rpcclient, RPC_DISPLAY_ADDR));

	status = cpu_to_be32(NFS4ERR_INVAL);
	if (!validate_bitmap_values(args->craa_type_mask))
		goto out;

	status = cpu_to_be32(NFS4_OK);
	if (args->craa_type_mask & BIT(RCA4_TYPE_MASK_RDATA_DLG))
		flags = FMODE_READ;
	if (args->craa_type_mask & BIT(RCA4_TYPE_MASK_WDATA_DLG))
		flags |= FMODE_WRITE;
	if (flags)
		nfs_expire_unused_delegation_types(cps->clp, flags);

	if (args->craa_type_mask & BIT(RCA4_TYPE_MASK_FILE_LAYOUT))
		pnfs_recall_all_layouts(cps->clp);

	if (args->craa_type_mask & BIT(PNFS_FF_RCA4_TYPE_MASK_READ)) {
		set_bit(NFS4CLNT_RECALL_ANY_LAYOUT_READ, &cps->clp->cl_state);
		schedule_manager = true;
	}
	if (args->craa_type_mask & BIT(PNFS_FF_RCA4_TYPE_MASK_RW)) {
		set_bit(NFS4CLNT_RECALL_ANY_LAYOUT_RW, &cps->clp->cl_state);
		schedule_manager = true;
	}
	if (schedule_manager)
		nfs4_schedule_state_manager(cps->clp);

out:
	dprintk("%s: exit with status = %d\n", __func__, ntohl(status));
	return status;
}

/* Reduce the fore channel's max_slots to the target value */
__be32 nfs4_callback_recallslot(void *argp, void *resp,
				struct cb_process_state *cps)
{
	struct cb_recallslotargs *args = argp;
	struct nfs4_slot_table *fc_tbl;
	__be32 status;

	status = htonl(NFS4ERR_OP_NOT_IN_SESSION);
	if (!cps->clp) /* set in cb_sequence */
		goto out;

	dprintk_rcu("NFS: CB_RECALL_SLOT request from %s target highest slotid %u\n",
		rpc_peeraddr2str(cps->clp->cl_rpcclient, RPC_DISPLAY_ADDR),
		args->crsa_target_highest_slotid);

	fc_tbl = &cps->clp->cl_session->fc_slot_table;

	status = htonl(NFS4_OK);

	nfs41_set_target_slotid(fc_tbl, args->crsa_target_highest_slotid);
	nfs41_notify_server(cps->clp);
out:
	dprintk("%s: exit with status = %d\n", __func__, ntohl(status));
	return status;
}

__be32 nfs4_callback_notify_lock(void *argp, void *resp,
				 struct cb_process_state *cps)
{
	struct cb_notify_lock_args *args = argp;

	if (!cps->clp) /* set in cb_sequence */
		return htonl(NFS4ERR_OP_NOT_IN_SESSION);

	dprintk_rcu("NFS: CB_NOTIFY_LOCK request from %s\n",
		rpc_peeraddr2str(cps->clp->cl_rpcclient, RPC_DISPLAY_ADDR));

	/* Don't wake anybody if the string looked bogus */
	if (args->cbnl_valid)
		__wake_up(&cps->clp->cl_lock_waitq, TASK_NORMAL, 0, args);

	return htonl(NFS4_OK);
}
#endif /* CONFIG_NFS_V4_1 */
#ifdef CONFIG_NFS_V4_2
static void nfs4_copy_cb_args(struct nfs4_copy_state *cp_state,
				struct cb_offloadargs *args)
{
	cp_state->count = args->wr_count;
	cp_state->error = args->error;
	if (!args->error) {
		cp_state->verf.committed = args->wr_writeverf.committed;
		memcpy(&cp_state->verf.verifier.data[0],
			&args->wr_writeverf.verifier.data[0],
			NFS4_VERIFIER_SIZE);
	}
}

__be32 nfs4_callback_offload(void *data, void *dummy,
			     struct cb_process_state *cps)
{
	struct cb_offloadargs *args = data;
	struct nfs_server *server;
	struct nfs4_copy_state *copy, *tmp_copy;
	bool found = false;

	copy = kzalloc(sizeof(struct nfs4_copy_state), GFP_NOFS);
	if (!copy)
		return htonl(NFS4ERR_SERVERFAULT);

	spin_lock(&cps->clp->cl_lock);
	rcu_read_lock();
	list_for_each_entry_rcu(server, &cps->clp->cl_superblocks,
				client_link) {
		list_for_each_entry(tmp_copy, &server->ss_copies, copies) {
			if (memcmp(args->coa_stateid.other,
					tmp_copy->stateid.other,
					sizeof(args->coa_stateid.other)))
				continue;
			nfs4_copy_cb_args(tmp_copy, args);
			complete(&tmp_copy->completion);
			found = true;
			goto out;
		}
	}
out:
	rcu_read_unlock();
	if (!found) {
		memcpy(&copy->stateid, &args->coa_stateid, NFS4_STATEID_SIZE);
		nfs4_copy_cb_args(copy, args);
		list_add_tail(&copy->copies, &cps->clp->pending_cb_stateids);
	} else
		kfree(copy);
	spin_unlock(&cps->clp->cl_lock);

	return 0;
}
#endif /* CONFIG_NFS_V4_2 */
