// SPDX-License-Identifier: GPL-2.0-or-later
/*******************************************************************************
 * This file contains the iSCSI Virtual Device and Disk Transport
 * agnostic related functions.
 *
 * (c) Copyright 2007-2013 Datera, Inc.
 *
 * Author: Nicholas A. Bellinger <nab@linux-iscsi.org>
 *
 ******************************************************************************/

#include <target/target_core_base.h>
#include <target/target_core_fabric.h>

#include <target/iscsi/iscsi_target_core.h>
#include "iscsi_target_device.h"
#include "iscsi_target_tpg.h"
#include "iscsi_target_util.h"

void iscsit_determine_maxcmdsn(struct iscsi_session *sess)
{
	struct se_node_acl *se_nacl;

	/*
	 * This is a discovery session, the single queue slot was already
	 * assigned in iscsi_login_zero_tsih().  Since only Logout and
	 * Text Opcodes are allowed during discovery we do not have to worry
	 * about the HBA's queue depth here.
	 */
	if (sess->sess_ops->SessionType)
		return;

	se_nacl = sess->se_sess->se_node_acl;

	/*
	 * This is a normal session, set the Session's CmdSN window to the
	 * struct se_node_acl->queue_depth.  The value in struct se_node_acl->queue_depth
	 * has already been validated as a legal value in
	 * core_set_queue_depth_for_node().
	 */
	sess->cmdsn_window = se_nacl->queue_depth;
	atomic_add(se_nacl->queue_depth - 1, &sess->max_cmd_sn);
}

void iscsit_increment_maxcmdsn(struct iscsi_cmd *cmd, struct iscsi_session *sess)
{
	u32 max_cmd_sn;

	if (cmd->immediate_cmd || cmd->maxcmdsn_inc)
		return;

	cmd->maxcmdsn_inc = 1;

	max_cmd_sn = atomic_inc_return(&sess->max_cmd_sn);
	pr_debug("Updated MaxCmdSN to 0x%08x\n", max_cmd_sn);
}
EXPORT_SYMBOL(iscsit_increment_maxcmdsn);
