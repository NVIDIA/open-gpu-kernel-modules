// SPDX-License-Identifier: GPL-2.0-or-later
/*******************************************************************************
 * Filename:  target_core_tmr.c
 *
 * This file contains SPC-3 task management infrastructure
 *
 * (c) Copyright 2009-2013 Datera, Inc.
 *
 * Nicholas A. Bellinger <nab@kernel.org>
 *
 ******************************************************************************/

#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/list.h>
#include <linux/export.h>

#include <target/target_core_base.h>
#include <target/target_core_backend.h>
#include <target/target_core_fabric.h>

#include "target_core_internal.h"
#include "target_core_alua.h"
#include "target_core_pr.h"

int core_tmr_alloc_req(
	struct se_cmd *se_cmd,
	void *fabric_tmr_ptr,
	u8 function,
	gfp_t gfp_flags)
{
	struct se_tmr_req *tmr;

	tmr = kzalloc(sizeof(struct se_tmr_req), gfp_flags);
	if (!tmr) {
		pr_err("Unable to allocate struct se_tmr_req\n");
		return -ENOMEM;
	}

	se_cmd->se_cmd_flags |= SCF_SCSI_TMR_CDB;
	se_cmd->se_tmr_req = tmr;
	tmr->task_cmd = se_cmd;
	tmr->fabric_tmr_ptr = fabric_tmr_ptr;
	tmr->function = function;
	INIT_LIST_HEAD(&tmr->tmr_list);

	return 0;
}
EXPORT_SYMBOL(core_tmr_alloc_req);

void core_tmr_release_req(struct se_tmr_req *tmr)
{
	struct se_device *dev = tmr->tmr_dev;
	unsigned long flags;

	if (dev) {
		spin_lock_irqsave(&dev->se_tmr_lock, flags);
		list_del_init(&tmr->tmr_list);
		spin_unlock_irqrestore(&dev->se_tmr_lock, flags);
	}

	kfree(tmr);
}

static int target_check_cdb_and_preempt(struct list_head *list,
		struct se_cmd *cmd)
{
	struct t10_pr_registration *reg;

	if (!list)
		return 0;
	list_for_each_entry(reg, list, pr_reg_abort_list) {
		if (reg->pr_res_key == cmd->pr_res_key)
			return 0;
	}

	return 1;
}

static bool __target_check_io_state(struct se_cmd *se_cmd,
				    struct se_session *tmr_sess, bool tas)
{
	struct se_session *sess = se_cmd->se_sess;

	assert_spin_locked(&sess->sess_cmd_lock);
	WARN_ON_ONCE(!irqs_disabled());
	/*
	 * If command already reached CMD_T_COMPLETE state within
	 * target_complete_cmd() or CMD_T_FABRIC_STOP due to shutdown,
	 * this se_cmd has been passed to fabric driver and will
	 * not be aborted.
	 *
	 * Otherwise, obtain a local se_cmd->cmd_kref now for TMR
	 * ABORT_TASK + LUN_RESET for CMD_T_ABORTED processing as
	 * long as se_cmd->cmd_kref is still active unless zero.
	 */
	spin_lock(&se_cmd->t_state_lock);
	if (se_cmd->transport_state & (CMD_T_COMPLETE | CMD_T_FABRIC_STOP)) {
		pr_debug("Attempted to abort io tag: %llu already complete or"
			" fabric stop, skipping\n", se_cmd->tag);
		spin_unlock(&se_cmd->t_state_lock);
		return false;
	}
	se_cmd->transport_state |= CMD_T_ABORTED;

	if ((tmr_sess != se_cmd->se_sess) && tas)
		se_cmd->transport_state |= CMD_T_TAS;

	spin_unlock(&se_cmd->t_state_lock);

	return kref_get_unless_zero(&se_cmd->cmd_kref);
}

void core_tmr_abort_task(
	struct se_device *dev,
	struct se_tmr_req *tmr,
	struct se_session *se_sess)
{
	LIST_HEAD(aborted_list);
	struct se_cmd *se_cmd, *next;
	unsigned long flags;
	bool rc;
	u64 ref_tag;
	int i;

	for (i = 0; i < dev->queue_cnt; i++) {
		flush_work(&dev->queues[i].sq.work);

		spin_lock_irqsave(&dev->queues[i].lock, flags);
		list_for_each_entry_safe(se_cmd, next, &dev->queues[i].state_list,
					 state_list) {
			if (se_sess != se_cmd->se_sess)
				continue;

			/*
			 * skip task management functions, including
			 * tmr->task_cmd
			 */
			if (se_cmd->se_cmd_flags & SCF_SCSI_TMR_CDB)
				continue;

			ref_tag = se_cmd->tag;
			if (tmr->ref_task_tag != ref_tag)
				continue;

			pr_err("ABORT_TASK: Found referenced %s task_tag: %llu\n",
			       se_cmd->se_tfo->fabric_name, ref_tag);

			spin_lock(&se_sess->sess_cmd_lock);
			rc = __target_check_io_state(se_cmd, se_sess, 0);
			spin_unlock(&se_sess->sess_cmd_lock);
			if (!rc)
				continue;

			list_move_tail(&se_cmd->state_list, &aborted_list);
			se_cmd->state_active = false;
			spin_unlock_irqrestore(&dev->queues[i].lock, flags);

			/*
			 * Ensure that this ABORT request is visible to the LU
			 * RESET code.
			 */
			if (!tmr->tmr_dev)
				WARN_ON_ONCE(transport_lookup_tmr_lun(tmr->task_cmd) < 0);

			if (dev->transport->tmr_notify)
				dev->transport->tmr_notify(dev, TMR_ABORT_TASK,
							   &aborted_list);

			list_del_init(&se_cmd->state_list);
			target_put_cmd_and_wait(se_cmd);

			pr_err("ABORT_TASK: Sending TMR_FUNCTION_COMPLETE for ref_tag: %llu\n",
			       ref_tag);
			tmr->response = TMR_FUNCTION_COMPLETE;
			atomic_long_inc(&dev->aborts_complete);
			return;
		}
		spin_unlock_irqrestore(&dev->queues[i].lock, flags);
	}

	if (dev->transport->tmr_notify)
		dev->transport->tmr_notify(dev, TMR_ABORT_TASK, &aborted_list);

	printk("ABORT_TASK: Sending TMR_TASK_DOES_NOT_EXIST for ref_tag: %lld\n",
			tmr->ref_task_tag);
	tmr->response = TMR_TASK_DOES_NOT_EXIST;
	atomic_long_inc(&dev->aborts_no_task);
}

static void core_tmr_drain_tmr_list(
	struct se_device *dev,
	struct se_tmr_req *tmr,
	struct list_head *preempt_and_abort_list)
{
	LIST_HEAD(drain_tmr_list);
	struct se_session *sess;
	struct se_tmr_req *tmr_p, *tmr_pp;
	struct se_cmd *cmd;
	unsigned long flags;
	bool rc;
	/*
	 * Release all pending and outgoing TMRs aside from the received
	 * LUN_RESET tmr..
	 */
	spin_lock_irqsave(&dev->se_tmr_lock, flags);
	if (tmr)
		list_del_init(&tmr->tmr_list);
	list_for_each_entry_safe(tmr_p, tmr_pp, &dev->dev_tmr_list, tmr_list) {
		cmd = tmr_p->task_cmd;
		if (!cmd) {
			pr_err("Unable to locate struct se_cmd for TMR\n");
			continue;
		}
		/*
		 * If this function was called with a valid pr_res_key
		 * parameter (eg: for PROUT PREEMPT_AND_ABORT service action
		 * skip non registration key matching TMRs.
		 */
		if (target_check_cdb_and_preempt(preempt_and_abort_list, cmd))
			continue;

		sess = cmd->se_sess;
		if (WARN_ON_ONCE(!sess))
			continue;

		spin_lock(&sess->sess_cmd_lock);
		rc = __target_check_io_state(cmd, sess, 0);
		spin_unlock(&sess->sess_cmd_lock);

		if (!rc) {
			printk("LUN_RESET TMR: non-zero kref_get_unless_zero\n");
			continue;
		}

		list_move_tail(&tmr_p->tmr_list, &drain_tmr_list);
	}
	spin_unlock_irqrestore(&dev->se_tmr_lock, flags);

	list_for_each_entry_safe(tmr_p, tmr_pp, &drain_tmr_list, tmr_list) {
		list_del_init(&tmr_p->tmr_list);
		cmd = tmr_p->task_cmd;

		pr_debug("LUN_RESET: %s releasing TMR %p Function: 0x%02x,"
			" Response: 0x%02x, t_state: %d\n",
			(preempt_and_abort_list) ? "Preempt" : "", tmr_p,
			tmr_p->function, tmr_p->response, cmd->t_state);

		target_put_cmd_and_wait(cmd);
	}
}

/**
 * core_tmr_drain_state_list() - abort SCSI commands associated with a device
 *
 * @dev:       Device for which to abort outstanding SCSI commands.
 * @prout_cmd: Pointer to the SCSI PREEMPT AND ABORT if this function is called
 *             to realize the PREEMPT AND ABORT functionality.
 * @tmr_sess:  Session through which the LUN RESET has been received.
 * @tas:       Task Aborted Status (TAS) bit from the SCSI control mode page.
 *             A quote from SPC-4, paragraph "7.5.10 Control mode page":
 *             "A task aborted status (TAS) bit set to zero specifies that
 *             aborted commands shall be terminated by the device server
 *             without any response to the application client. A TAS bit set
 *             to one specifies that commands aborted by the actions of an I_T
 *             nexus other than the I_T nexus on which the command was
 *             received shall be completed with TASK ABORTED status."
 * @preempt_and_abort_list: For the PREEMPT AND ABORT functionality, a list
 *             with registrations that will be preempted.
 */
static void core_tmr_drain_state_list(
	struct se_device *dev,
	struct se_cmd *prout_cmd,
	struct se_session *tmr_sess,
	bool tas,
	struct list_head *preempt_and_abort_list)
{
	LIST_HEAD(drain_task_list);
	struct se_session *sess;
	struct se_cmd *cmd, *next;
	unsigned long flags;
	int rc, i;

	/*
	 * Complete outstanding commands with TASK_ABORTED SAM status.
	 *
	 * This is following sam4r17, section 5.6 Aborting commands, Table 38
	 * for TMR LUN_RESET:
	 *
	 * a) "Yes" indicates that each command that is aborted on an I_T nexus
	 * other than the one that caused the SCSI device condition is
	 * completed with TASK ABORTED status, if the TAS bit is set to one in
	 * the Control mode page (see SPC-4). "No" indicates that no status is
	 * returned for aborted commands.
	 *
	 * d) If the logical unit reset is caused by a particular I_T nexus
	 * (e.g., by a LOGICAL UNIT RESET task management function), then "yes"
	 * (TASK_ABORTED status) applies.
	 *
	 * Otherwise (e.g., if triggered by a hard reset), "no"
	 * (no TASK_ABORTED SAM status) applies.
	 *
	 * Note that this seems to be independent of TAS (Task Aborted Status)
	 * in the Control Mode Page.
	 */
	for (i = 0; i < dev->queue_cnt; i++) {
		flush_work(&dev->queues[i].sq.work);

		spin_lock_irqsave(&dev->queues[i].lock, flags);
		list_for_each_entry_safe(cmd, next, &dev->queues[i].state_list,
					 state_list) {
			/*
			 * For PREEMPT_AND_ABORT usage, only process commands
			 * with a matching reservation key.
			 */
			if (target_check_cdb_and_preempt(preempt_and_abort_list,
							 cmd))
				continue;

			/*
			 * Not aborting PROUT PREEMPT_AND_ABORT CDB..
			 */
			if (prout_cmd == cmd)
				continue;

			sess = cmd->se_sess;
			if (WARN_ON_ONCE(!sess))
				continue;

			spin_lock(&sess->sess_cmd_lock);
			rc = __target_check_io_state(cmd, tmr_sess, tas);
			spin_unlock(&sess->sess_cmd_lock);
			if (!rc)
				continue;

			list_move_tail(&cmd->state_list, &drain_task_list);
			cmd->state_active = false;
		}
		spin_unlock_irqrestore(&dev->queues[i].lock, flags);
	}

	if (dev->transport->tmr_notify)
		dev->transport->tmr_notify(dev, preempt_and_abort_list ?
					   TMR_LUN_RESET_PRO : TMR_LUN_RESET,
					   &drain_task_list);

	while (!list_empty(&drain_task_list)) {
		cmd = list_entry(drain_task_list.next, struct se_cmd, state_list);
		list_del_init(&cmd->state_list);

		target_show_cmd("LUN_RESET: ", cmd);
		pr_debug("LUN_RESET: ITT[0x%08llx] - %s pr_res_key: 0x%016Lx\n",
			 cmd->tag, (preempt_and_abort_list) ? "preempt" : "",
			 cmd->pr_res_key);

		target_put_cmd_and_wait(cmd);
	}
}

int core_tmr_lun_reset(
        struct se_device *dev,
        struct se_tmr_req *tmr,
        struct list_head *preempt_and_abort_list,
        struct se_cmd *prout_cmd)
{
	struct se_node_acl *tmr_nacl = NULL;
	struct se_portal_group *tmr_tpg = NULL;
	struct se_session *tmr_sess = NULL;
	bool tas;
        /*
	 * TASK_ABORTED status bit, this is configurable via ConfigFS
	 * struct se_device attributes.  spc4r17 section 7.4.6 Control mode page
	 *
	 * A task aborted status (TAS) bit set to zero specifies that aborted
	 * tasks shall be terminated by the device server without any response
	 * to the application client. A TAS bit set to one specifies that tasks
	 * aborted by the actions of an I_T nexus other than the I_T nexus on
	 * which the command was received shall be completed with TASK ABORTED
	 * status (see SAM-4).
	 */
	tas = dev->dev_attrib.emulate_tas;
	/*
	 * Determine if this se_tmr is coming from a $FABRIC_MOD
	 * or struct se_device passthrough..
	 */
	if (tmr && tmr->task_cmd && tmr->task_cmd->se_sess) {
		tmr_sess = tmr->task_cmd->se_sess;
		tmr_nacl = tmr_sess->se_node_acl;
		tmr_tpg = tmr_sess->se_tpg;
		if (tmr_nacl && tmr_tpg) {
			pr_debug("LUN_RESET: TMR caller fabric: %s"
				" initiator port %s\n",
				tmr_tpg->se_tpg_tfo->fabric_name,
				tmr_nacl->initiatorname);
		}
	}
	pr_debug("LUN_RESET: %s starting for [%s], tas: %d\n",
		(preempt_and_abort_list) ? "Preempt" : "TMR",
		dev->transport->name, tas);

	core_tmr_drain_tmr_list(dev, tmr, preempt_and_abort_list);
	core_tmr_drain_state_list(dev, prout_cmd, tmr_sess, tas,
				preempt_and_abort_list);

	/*
	 * Clear any legacy SPC-2 reservation when called during
	 * LOGICAL UNIT RESET
	 */
	if (!preempt_and_abort_list &&
	     (dev->dev_reservation_flags & DRF_SPC2_RESERVATIONS)) {
		spin_lock(&dev->dev_reservation_lock);
		dev->reservation_holder = NULL;
		dev->dev_reservation_flags &= ~DRF_SPC2_RESERVATIONS;
		spin_unlock(&dev->dev_reservation_lock);
		pr_debug("LUN_RESET: SCSI-2 Released reservation\n");
	}

	atomic_long_inc(&dev->num_resets);

	pr_debug("LUN_RESET: %s for [%s] Complete\n",
			(preempt_and_abort_list) ? "Preempt" : "TMR",
			dev->transport->name);
	return 0;
}

