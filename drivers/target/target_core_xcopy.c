// SPDX-License-Identifier: GPL-2.0-or-later
/*******************************************************************************
 * Filename: target_core_xcopy.c
 *
 * This file contains support for SPC-4 Extended-Copy offload with generic
 * TCM backends.
 *
 * Copyright (c) 2011-2013 Datera, Inc. All rights reserved.
 *
 * Author:
 * Nicholas A. Bellinger <nab@daterainc.com>
 *
 ******************************************************************************/

#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/list.h>
#include <linux/configfs.h>
#include <linux/ratelimit.h>
#include <scsi/scsi_proto.h>
#include <asm/unaligned.h>

#include <target/target_core_base.h>
#include <target/target_core_backend.h>
#include <target/target_core_fabric.h>

#include "target_core_internal.h"
#include "target_core_pr.h"
#include "target_core_ua.h"
#include "target_core_xcopy.h"

static struct workqueue_struct *xcopy_wq = NULL;

static sense_reason_t target_parse_xcopy_cmd(struct xcopy_op *xop);

static int target_xcopy_gen_naa_ieee(struct se_device *dev, unsigned char *buf)
{
	int off = 0;

	buf[off++] = (0x6 << 4);
	buf[off++] = 0x01;
	buf[off++] = 0x40;
	buf[off] = (0x5 << 4);

	spc_parse_naa_6h_vendor_specific(dev, &buf[off]);
	return 0;
}

/**
 * target_xcopy_locate_se_dev_e4_iter - compare XCOPY NAA device identifiers
 *
 * @se_dev: device being considered for match
 * @dev_wwn: XCOPY requested NAA dev_wwn
 * @return: 1 on match, 0 on no-match
 */
static int target_xcopy_locate_se_dev_e4_iter(struct se_device *se_dev,
					      const unsigned char *dev_wwn)
{
	unsigned char tmp_dev_wwn[XCOPY_NAA_IEEE_REGEX_LEN];
	int rc;

	if (!se_dev->dev_attrib.emulate_3pc) {
		pr_debug("XCOPY: emulate_3pc disabled on se_dev %p\n", se_dev);
		return 0;
	}

	memset(&tmp_dev_wwn[0], 0, XCOPY_NAA_IEEE_REGEX_LEN);
	target_xcopy_gen_naa_ieee(se_dev, &tmp_dev_wwn[0]);

	rc = memcmp(&tmp_dev_wwn[0], dev_wwn, XCOPY_NAA_IEEE_REGEX_LEN);
	if (rc != 0) {
		pr_debug("XCOPY: skip non-matching: %*ph\n",
			 XCOPY_NAA_IEEE_REGEX_LEN, tmp_dev_wwn);
		return 0;
	}
	pr_debug("XCOPY 0xe4: located se_dev: %p\n", se_dev);

	return 1;
}

static int target_xcopy_locate_se_dev_e4(struct se_session *sess,
					const unsigned char *dev_wwn,
					struct se_device **_found_dev,
					struct percpu_ref **_found_lun_ref)
{
	struct se_dev_entry *deve;
	struct se_node_acl *nacl;
	struct se_lun *this_lun = NULL;
	struct se_device *found_dev = NULL;

	/* cmd with NULL sess indicates no associated $FABRIC_MOD */
	if (!sess)
		goto err_out;

	pr_debug("XCOPY 0xe4: searching for: %*ph\n",
		 XCOPY_NAA_IEEE_REGEX_LEN, dev_wwn);

	nacl = sess->se_node_acl;
	rcu_read_lock();
	hlist_for_each_entry_rcu(deve, &nacl->lun_entry_hlist, link) {
		struct se_device *this_dev;
		int rc;

		this_lun = rcu_dereference(deve->se_lun);
		this_dev = rcu_dereference_raw(this_lun->lun_se_dev);

		rc = target_xcopy_locate_se_dev_e4_iter(this_dev, dev_wwn);
		if (rc) {
			if (percpu_ref_tryget_live(&this_lun->lun_ref))
				found_dev = this_dev;
			break;
		}
	}
	rcu_read_unlock();
	if (found_dev == NULL)
		goto err_out;

	pr_debug("lun_ref held for se_dev: %p se_dev->se_dev_group: %p\n",
		 found_dev, &found_dev->dev_group);
	*_found_dev = found_dev;
	*_found_lun_ref = &this_lun->lun_ref;
	return 0;
err_out:
	pr_debug_ratelimited("Unable to locate 0xe4 descriptor for EXTENDED_COPY\n");
	return -EINVAL;
}

static int target_xcopy_parse_tiddesc_e4(struct se_cmd *se_cmd, struct xcopy_op *xop,
				unsigned char *p, unsigned short cscd_index)
{
	unsigned char *desc = p;
	unsigned short ript;
	u8 desig_len;
	/*
	 * Extract RELATIVE INITIATOR PORT IDENTIFIER
	 */
	ript = get_unaligned_be16(&desc[2]);
	pr_debug("XCOPY 0xe4: RELATIVE INITIATOR PORT IDENTIFIER: %hu\n", ript);
	/*
	 * Check for supported code set, association, and designator type
	 */
	if ((desc[4] & 0x0f) != 0x1) {
		pr_err("XCOPY 0xe4: code set of non binary type not supported\n");
		return -EINVAL;
	}
	if ((desc[5] & 0x30) != 0x00) {
		pr_err("XCOPY 0xe4: association other than LUN not supported\n");
		return -EINVAL;
	}
	if ((desc[5] & 0x0f) != 0x3) {
		pr_err("XCOPY 0xe4: designator type unsupported: 0x%02x\n",
				(desc[5] & 0x0f));
		return -EINVAL;
	}
	/*
	 * Check for matching 16 byte length for NAA IEEE Registered Extended
	 * Assigned designator
	 */
	desig_len = desc[7];
	if (desig_len != XCOPY_NAA_IEEE_REGEX_LEN) {
		pr_err("XCOPY 0xe4: invalid desig_len: %d\n", (int)desig_len);
		return -EINVAL;
	}
	pr_debug("XCOPY 0xe4: desig_len: %d\n", (int)desig_len);
	/*
	 * Check for NAA IEEE Registered Extended Assigned header..
	 */
	if ((desc[8] & 0xf0) != 0x60) {
		pr_err("XCOPY 0xe4: Unsupported DESIGNATOR TYPE: 0x%02x\n",
					(desc[8] & 0xf0));
		return -EINVAL;
	}

	if (cscd_index != xop->stdi && cscd_index != xop->dtdi) {
		pr_debug("XCOPY 0xe4: ignoring CSCD entry %d - neither src nor "
			 "dest\n", cscd_index);
		return 0;
	}

	if (cscd_index == xop->stdi) {
		memcpy(&xop->src_tid_wwn[0], &desc[8], XCOPY_NAA_IEEE_REGEX_LEN);
		/*
		 * Determine if the source designator matches the local device
		 */
		if (!memcmp(&xop->local_dev_wwn[0], &xop->src_tid_wwn[0],
				XCOPY_NAA_IEEE_REGEX_LEN)) {
			xop->op_origin = XCOL_SOURCE_RECV_OP;
			xop->src_dev = se_cmd->se_dev;
			pr_debug("XCOPY 0xe4: Set xop->src_dev %p from source"
					" received xop\n", xop->src_dev);
		}
	}

	if (cscd_index == xop->dtdi) {
		memcpy(&xop->dst_tid_wwn[0], &desc[8], XCOPY_NAA_IEEE_REGEX_LEN);
		/*
		 * Determine if the destination designator matches the local
		 * device. If @cscd_index corresponds to both source (stdi) and
		 * destination (dtdi), or dtdi comes after stdi, then
		 * XCOL_DEST_RECV_OP wins.
		 */
		if (!memcmp(&xop->local_dev_wwn[0], &xop->dst_tid_wwn[0],
				XCOPY_NAA_IEEE_REGEX_LEN)) {
			xop->op_origin = XCOL_DEST_RECV_OP;
			xop->dst_dev = se_cmd->se_dev;
			pr_debug("XCOPY 0xe4: Set xop->dst_dev: %p from destination"
				" received xop\n", xop->dst_dev);
		}
	}

	return 0;
}

static int target_xcopy_parse_target_descriptors(struct se_cmd *se_cmd,
				struct xcopy_op *xop, unsigned char *p,
				unsigned short tdll, sense_reason_t *sense_ret)
{
	struct se_device *local_dev = se_cmd->se_dev;
	unsigned char *desc = p;
	int offset = tdll % XCOPY_TARGET_DESC_LEN, rc;
	unsigned short cscd_index = 0;
	unsigned short start = 0;

	*sense_ret = TCM_INVALID_PARAMETER_LIST;

	if (offset != 0) {
		pr_err("XCOPY target descriptor list length is not"
			" multiple of %d\n", XCOPY_TARGET_DESC_LEN);
		*sense_ret = TCM_UNSUPPORTED_TARGET_DESC_TYPE_CODE;
		return -EINVAL;
	}
	if (tdll > RCR_OP_MAX_TARGET_DESC_COUNT * XCOPY_TARGET_DESC_LEN) {
		pr_err("XCOPY target descriptor supports a maximum"
			" two src/dest descriptors, tdll: %hu too large..\n", tdll);
		/* spc4r37 6.4.3.4 CSCD DESCRIPTOR LIST LENGTH field */
		*sense_ret = TCM_TOO_MANY_TARGET_DESCS;
		return -EINVAL;
	}
	/*
	 * Generate an IEEE Registered Extended designator based upon the
	 * se_device the XCOPY was received upon..
	 */
	memset(&xop->local_dev_wwn[0], 0, XCOPY_NAA_IEEE_REGEX_LEN);
	target_xcopy_gen_naa_ieee(local_dev, &xop->local_dev_wwn[0]);

	while (start < tdll) {
		/*
		 * Check target descriptor identification with 0xE4 type, and
		 * compare the current index with the CSCD descriptor IDs in
		 * the segment descriptor. Use VPD 0x83 WWPN matching ..
		 */
		switch (desc[0]) {
		case 0xe4:
			rc = target_xcopy_parse_tiddesc_e4(se_cmd, xop,
							&desc[0], cscd_index);
			if (rc != 0)
				goto out;
			start += XCOPY_TARGET_DESC_LEN;
			desc += XCOPY_TARGET_DESC_LEN;
			cscd_index++;
			break;
		default:
			pr_err("XCOPY unsupported descriptor type code:"
					" 0x%02x\n", desc[0]);
			*sense_ret = TCM_UNSUPPORTED_TARGET_DESC_TYPE_CODE;
			goto out;
		}
	}

	switch (xop->op_origin) {
	case XCOL_SOURCE_RECV_OP:
		rc = target_xcopy_locate_se_dev_e4(se_cmd->se_sess,
						xop->dst_tid_wwn,
						&xop->dst_dev,
						&xop->remote_lun_ref);
		break;
	case XCOL_DEST_RECV_OP:
		rc = target_xcopy_locate_se_dev_e4(se_cmd->se_sess,
						xop->src_tid_wwn,
						&xop->src_dev,
						&xop->remote_lun_ref);
		break;
	default:
		pr_err("XCOPY CSCD descriptor IDs not found in CSCD list - "
			"stdi: %hu dtdi: %hu\n", xop->stdi, xop->dtdi);
		rc = -EINVAL;
		break;
	}
	/*
	 * If a matching IEEE NAA 0x83 descriptor for the requested device
	 * is not located on this node, return COPY_ABORTED with ASQ/ASQC
	 * 0x0d/0x02 - COPY_TARGET_DEVICE_NOT_REACHABLE to request the
	 * initiator to fall back to normal copy method.
	 */
	if (rc < 0) {
		*sense_ret = TCM_COPY_TARGET_DEVICE_NOT_REACHABLE;
		goto out;
	}

	pr_debug("XCOPY TGT desc: Source dev: %p NAA IEEE WWN: 0x%16phN\n",
		 xop->src_dev, &xop->src_tid_wwn[0]);
	pr_debug("XCOPY TGT desc: Dest dev: %p NAA IEEE WWN: 0x%16phN\n",
		 xop->dst_dev, &xop->dst_tid_wwn[0]);

	return cscd_index;

out:
	return -EINVAL;
}

static int target_xcopy_parse_segdesc_02(struct se_cmd *se_cmd, struct xcopy_op *xop,
					unsigned char *p)
{
	unsigned char *desc = p;
	int dc = (desc[1] & 0x02);
	unsigned short desc_len;

	desc_len = get_unaligned_be16(&desc[2]);
	if (desc_len != 0x18) {
		pr_err("XCOPY segment desc 0x02: Illegal desc_len:"
				" %hu\n", desc_len);
		return -EINVAL;
	}

	xop->stdi = get_unaligned_be16(&desc[4]);
	xop->dtdi = get_unaligned_be16(&desc[6]);

	if (xop->stdi > XCOPY_CSCD_DESC_ID_LIST_OFF_MAX ||
	    xop->dtdi > XCOPY_CSCD_DESC_ID_LIST_OFF_MAX) {
		pr_err("XCOPY segment desc 0x02: unsupported CSCD ID > 0x%x; stdi: %hu dtdi: %hu\n",
			XCOPY_CSCD_DESC_ID_LIST_OFF_MAX, xop->stdi, xop->dtdi);
		return -EINVAL;
	}

	pr_debug("XCOPY seg desc 0x02: desc_len: %hu stdi: %hu dtdi: %hu, DC: %d\n",
		desc_len, xop->stdi, xop->dtdi, dc);

	xop->nolb = get_unaligned_be16(&desc[10]);
	xop->src_lba = get_unaligned_be64(&desc[12]);
	xop->dst_lba = get_unaligned_be64(&desc[20]);
	pr_debug("XCOPY seg desc 0x02: nolb: %hu src_lba: %llu dst_lba: %llu\n",
		xop->nolb, (unsigned long long)xop->src_lba,
		(unsigned long long)xop->dst_lba);

	return 0;
}

static int target_xcopy_parse_segment_descriptors(struct se_cmd *se_cmd,
				struct xcopy_op *xop, unsigned char *p,
				unsigned int sdll, sense_reason_t *sense_ret)
{
	unsigned char *desc = p;
	unsigned int start = 0;
	int offset = sdll % XCOPY_SEGMENT_DESC_LEN, rc, ret = 0;

	*sense_ret = TCM_INVALID_PARAMETER_LIST;

	if (offset != 0) {
		pr_err("XCOPY segment descriptor list length is not"
			" multiple of %d\n", XCOPY_SEGMENT_DESC_LEN);
		*sense_ret = TCM_UNSUPPORTED_SEGMENT_DESC_TYPE_CODE;
		return -EINVAL;
	}
	if (sdll > RCR_OP_MAX_SG_DESC_COUNT * XCOPY_SEGMENT_DESC_LEN) {
		pr_err("XCOPY supports %u segment descriptor(s), sdll: %u too"
			" large..\n", RCR_OP_MAX_SG_DESC_COUNT, sdll);
		/* spc4r37 6.4.3.5 SEGMENT DESCRIPTOR LIST LENGTH field */
		*sense_ret = TCM_TOO_MANY_SEGMENT_DESCS;
		return -EINVAL;
	}

	while (start < sdll) {
		/*
		 * Check segment descriptor type code for block -> block
		 */
		switch (desc[0]) {
		case 0x02:
			rc = target_xcopy_parse_segdesc_02(se_cmd, xop, desc);
			if (rc < 0)
				goto out;

			ret++;
			start += XCOPY_SEGMENT_DESC_LEN;
			desc += XCOPY_SEGMENT_DESC_LEN;
			break;
		default:
			pr_err("XCOPY unsupported segment descriptor"
				"type: 0x%02x\n", desc[0]);
			*sense_ret = TCM_UNSUPPORTED_SEGMENT_DESC_TYPE_CODE;
			goto out;
		}
	}

	return ret;

out:
	return -EINVAL;
}

/*
 * Start xcopy_pt ops
 */

struct xcopy_pt_cmd {
	struct se_cmd se_cmd;
	struct completion xpt_passthrough_sem;
	unsigned char sense_buffer[TRANSPORT_SENSE_BUFFER];
};

struct se_portal_group xcopy_pt_tpg;
static struct se_session xcopy_pt_sess;
static struct se_node_acl xcopy_pt_nacl;

static int xcopy_pt_get_cmd_state(struct se_cmd *se_cmd)
{
        return 0;
}

static void xcopy_pt_undepend_remotedev(struct xcopy_op *xop)
{
	if (xop->op_origin == XCOL_SOURCE_RECV_OP)
		pr_debug("putting dst lun_ref for %p\n", xop->dst_dev);
	else
		pr_debug("putting src lun_ref for %p\n", xop->src_dev);

	percpu_ref_put(xop->remote_lun_ref);
}

static void xcopy_pt_release_cmd(struct se_cmd *se_cmd)
{
	struct xcopy_pt_cmd *xpt_cmd = container_of(se_cmd,
				struct xcopy_pt_cmd, se_cmd);

	/* xpt_cmd is on the stack, nothing to free here */
	pr_debug("xpt_cmd done: %p\n", xpt_cmd);
}

static int xcopy_pt_check_stop_free(struct se_cmd *se_cmd)
{
	struct xcopy_pt_cmd *xpt_cmd = container_of(se_cmd,
				struct xcopy_pt_cmd, se_cmd);

	complete(&xpt_cmd->xpt_passthrough_sem);
	return 0;
}

static int xcopy_pt_write_pending(struct se_cmd *se_cmd)
{
	return 0;
}

static int xcopy_pt_queue_data_in(struct se_cmd *se_cmd)
{
	return 0;
}

static int xcopy_pt_queue_status(struct se_cmd *se_cmd)
{
	return 0;
}

static const struct target_core_fabric_ops xcopy_pt_tfo = {
	.fabric_name		= "xcopy-pt",
	.get_cmd_state		= xcopy_pt_get_cmd_state,
	.release_cmd		= xcopy_pt_release_cmd,
	.check_stop_free	= xcopy_pt_check_stop_free,
	.write_pending		= xcopy_pt_write_pending,
	.queue_data_in		= xcopy_pt_queue_data_in,
	.queue_status		= xcopy_pt_queue_status,
};

/*
 * End xcopy_pt_ops
 */

int target_xcopy_setup_pt(void)
{
	int ret;

	xcopy_wq = alloc_workqueue("xcopy_wq", WQ_MEM_RECLAIM, 0);
	if (!xcopy_wq) {
		pr_err("Unable to allocate xcopy_wq\n");
		return -ENOMEM;
	}

	memset(&xcopy_pt_tpg, 0, sizeof(struct se_portal_group));
	INIT_LIST_HEAD(&xcopy_pt_tpg.acl_node_list);
	INIT_LIST_HEAD(&xcopy_pt_tpg.tpg_sess_list);

	xcopy_pt_tpg.se_tpg_tfo = &xcopy_pt_tfo;

	memset(&xcopy_pt_nacl, 0, sizeof(struct se_node_acl));
	INIT_LIST_HEAD(&xcopy_pt_nacl.acl_list);
	INIT_LIST_HEAD(&xcopy_pt_nacl.acl_sess_list);
	memset(&xcopy_pt_sess, 0, sizeof(struct se_session));
	ret = transport_init_session(&xcopy_pt_sess);
	if (ret < 0)
		goto destroy_wq;

	xcopy_pt_nacl.se_tpg = &xcopy_pt_tpg;
	xcopy_pt_nacl.nacl_sess = &xcopy_pt_sess;

	xcopy_pt_sess.se_tpg = &xcopy_pt_tpg;
	xcopy_pt_sess.se_node_acl = &xcopy_pt_nacl;

	return 0;

destroy_wq:
	destroy_workqueue(xcopy_wq);
	xcopy_wq = NULL;
	return ret;
}

void target_xcopy_release_pt(void)
{
	if (xcopy_wq) {
		destroy_workqueue(xcopy_wq);
		transport_uninit_session(&xcopy_pt_sess);
	}
}

/*
 * target_xcopy_setup_pt_cmd - set up a pass-through command
 * @xpt_cmd:	 Data structure to initialize.
 * @xop:	 Describes the XCOPY operation received from an initiator.
 * @se_dev:	 Backend device to associate with @xpt_cmd if
 *		 @remote_port == true.
 * @cdb:	 SCSI CDB to be copied into @xpt_cmd.
 * @remote_port: If false, use the LUN through which the XCOPY command has
 *		 been received. If true, use @se_dev->xcopy_lun.
 *
 * Set up a SCSI command (READ or WRITE) that will be used to execute an
 * XCOPY command.
 */
static int target_xcopy_setup_pt_cmd(
	struct xcopy_pt_cmd *xpt_cmd,
	struct xcopy_op *xop,
	struct se_device *se_dev,
	unsigned char *cdb,
	bool remote_port)
{
	struct se_cmd *cmd = &xpt_cmd->se_cmd;

	/*
	 * Setup LUN+port to honor reservations based upon xop->op_origin for
	 * X-COPY PUSH or X-COPY PULL based upon where the CDB was received.
	 */
	if (remote_port) {
		cmd->se_lun = &se_dev->xcopy_lun;
		cmd->se_dev = se_dev;
	} else {
		cmd->se_lun = xop->xop_se_cmd->se_lun;
		cmd->se_dev = xop->xop_se_cmd->se_dev;
	}
	cmd->se_cmd_flags |= SCF_SE_LUN_CMD;

	if (target_cmd_init_cdb(cmd, cdb, GFP_KERNEL))
		return -EINVAL;

	cmd->tag = 0;
	if (target_cmd_parse_cdb(cmd))
		return -EINVAL;

	if (transport_generic_map_mem_to_cmd(cmd, xop->xop_data_sg,
					xop->xop_data_nents, NULL, 0))
		return -EINVAL;

	pr_debug("Setup PASSTHROUGH_NOALLOC t_data_sg: %p t_data_nents:"
		 " %u\n", cmd->t_data_sg, cmd->t_data_nents);

	return 0;
}

static int target_xcopy_issue_pt_cmd(struct xcopy_pt_cmd *xpt_cmd)
{
	struct se_cmd *se_cmd = &xpt_cmd->se_cmd;
	sense_reason_t sense_rc;

	sense_rc = transport_generic_new_cmd(se_cmd);
	if (sense_rc)
		return -EINVAL;

	if (se_cmd->data_direction == DMA_TO_DEVICE)
		target_execute_cmd(se_cmd);

	wait_for_completion_interruptible(&xpt_cmd->xpt_passthrough_sem);

	pr_debug("target_xcopy_issue_pt_cmd(): SCSI status: 0x%02x\n",
			se_cmd->scsi_status);

	return (se_cmd->scsi_status) ? -EINVAL : 0;
}

static int target_xcopy_read_source(
	struct se_cmd *ec_cmd,
	struct xcopy_op *xop,
	struct se_device *src_dev,
	sector_t src_lba,
	u32 src_sectors)
{
	struct xcopy_pt_cmd xpt_cmd;
	struct se_cmd *se_cmd = &xpt_cmd.se_cmd;
	u32 length = (src_sectors * src_dev->dev_attrib.block_size);
	int rc;
	unsigned char cdb[16];
	bool remote_port = (xop->op_origin == XCOL_DEST_RECV_OP);

	memset(&xpt_cmd, 0, sizeof(xpt_cmd));
	init_completion(&xpt_cmd.xpt_passthrough_sem);

	memset(&cdb[0], 0, 16);
	cdb[0] = READ_16;
	put_unaligned_be64(src_lba, &cdb[2]);
	put_unaligned_be32(src_sectors, &cdb[10]);
	pr_debug("XCOPY: Built READ_16: LBA: %llu Sectors: %u Length: %u\n",
		(unsigned long long)src_lba, src_sectors, length);

	__target_init_cmd(se_cmd, &xcopy_pt_tfo, &xcopy_pt_sess, length,
			  DMA_FROM_DEVICE, 0, &xpt_cmd.sense_buffer[0], 0);

	rc = target_xcopy_setup_pt_cmd(&xpt_cmd, xop, src_dev, &cdb[0],
				remote_port);
	if (rc < 0) {
		ec_cmd->scsi_status = se_cmd->scsi_status;
		goto out;
	}

	pr_debug("XCOPY-READ: Saved xop->xop_data_sg: %p, num: %u for READ"
		" memory\n", xop->xop_data_sg, xop->xop_data_nents);

	rc = target_xcopy_issue_pt_cmd(&xpt_cmd);
	if (rc < 0)
		ec_cmd->scsi_status = se_cmd->scsi_status;
out:
	transport_generic_free_cmd(se_cmd, 0);
	return rc;
}

static int target_xcopy_write_destination(
	struct se_cmd *ec_cmd,
	struct xcopy_op *xop,
	struct se_device *dst_dev,
	sector_t dst_lba,
	u32 dst_sectors)
{
	struct xcopy_pt_cmd xpt_cmd;
	struct se_cmd *se_cmd = &xpt_cmd.se_cmd;
	u32 length = (dst_sectors * dst_dev->dev_attrib.block_size);
	int rc;
	unsigned char cdb[16];
	bool remote_port = (xop->op_origin == XCOL_SOURCE_RECV_OP);

	memset(&xpt_cmd, 0, sizeof(xpt_cmd));
	init_completion(&xpt_cmd.xpt_passthrough_sem);

	memset(&cdb[0], 0, 16);
	cdb[0] = WRITE_16;
	put_unaligned_be64(dst_lba, &cdb[2]);
	put_unaligned_be32(dst_sectors, &cdb[10]);
	pr_debug("XCOPY: Built WRITE_16: LBA: %llu Sectors: %u Length: %u\n",
		(unsigned long long)dst_lba, dst_sectors, length);

	__target_init_cmd(se_cmd, &xcopy_pt_tfo, &xcopy_pt_sess, length,
			  DMA_TO_DEVICE, 0, &xpt_cmd.sense_buffer[0], 0);

	rc = target_xcopy_setup_pt_cmd(&xpt_cmd, xop, dst_dev, &cdb[0],
				remote_port);
	if (rc < 0) {
		ec_cmd->scsi_status = se_cmd->scsi_status;
		goto out;
	}

	rc = target_xcopy_issue_pt_cmd(&xpt_cmd);
	if (rc < 0)
		ec_cmd->scsi_status = se_cmd->scsi_status;
out:
	transport_generic_free_cmd(se_cmd, 0);
	return rc;
}

static void target_xcopy_do_work(struct work_struct *work)
{
	struct xcopy_op *xop = container_of(work, struct xcopy_op, xop_work);
	struct se_cmd *ec_cmd = xop->xop_se_cmd;
	struct se_device *src_dev, *dst_dev;
	sector_t src_lba, dst_lba, end_lba;
	unsigned int max_sectors;
	int rc = 0;
	unsigned short nolb, max_nolb, copied_nolb = 0;

	if (target_parse_xcopy_cmd(xop) != TCM_NO_SENSE)
		goto err_free;

	if (WARN_ON_ONCE(!xop->src_dev) || WARN_ON_ONCE(!xop->dst_dev))
		goto err_free;

	src_dev = xop->src_dev;
	dst_dev = xop->dst_dev;
	src_lba = xop->src_lba;
	dst_lba = xop->dst_lba;
	nolb = xop->nolb;
	end_lba = src_lba + nolb;
	/*
	 * Break up XCOPY I/O into hw_max_sectors sized I/O based on the
	 * smallest max_sectors between src_dev + dev_dev, or
	 */
	max_sectors = min(src_dev->dev_attrib.hw_max_sectors,
			  dst_dev->dev_attrib.hw_max_sectors);
	max_sectors = min_t(u32, max_sectors, XCOPY_MAX_SECTORS);

	max_nolb = min_t(u16, max_sectors, ((u16)(~0U)));

	pr_debug("target_xcopy_do_work: nolb: %hu, max_nolb: %hu end_lba: %llu\n",
			nolb, max_nolb, (unsigned long long)end_lba);
	pr_debug("target_xcopy_do_work: Starting src_lba: %llu, dst_lba: %llu\n",
			(unsigned long long)src_lba, (unsigned long long)dst_lba);

	while (src_lba < end_lba) {
		unsigned short cur_nolb = min(nolb, max_nolb);
		u32 cur_bytes = cur_nolb * src_dev->dev_attrib.block_size;

		if (cur_bytes != xop->xop_data_bytes) {
			/*
			 * (Re)allocate a buffer large enough to hold the XCOPY
			 * I/O size, which can be reused each read / write loop.
			 */
			target_free_sgl(xop->xop_data_sg, xop->xop_data_nents);
			rc = target_alloc_sgl(&xop->xop_data_sg,
					      &xop->xop_data_nents,
					      cur_bytes,
					      false, false);
			if (rc < 0)
				goto out;
			xop->xop_data_bytes = cur_bytes;
		}

		pr_debug("target_xcopy_do_work: Calling read src_dev: %p src_lba: %llu,"
			" cur_nolb: %hu\n", src_dev, (unsigned long long)src_lba, cur_nolb);

		rc = target_xcopy_read_source(ec_cmd, xop, src_dev, src_lba, cur_nolb);
		if (rc < 0)
			goto out;

		src_lba += cur_nolb;
		pr_debug("target_xcopy_do_work: Incremented READ src_lba to %llu\n",
				(unsigned long long)src_lba);

		pr_debug("target_xcopy_do_work: Calling write dst_dev: %p dst_lba: %llu,"
			" cur_nolb: %hu\n", dst_dev, (unsigned long long)dst_lba, cur_nolb);

		rc = target_xcopy_write_destination(ec_cmd, xop, dst_dev,
						dst_lba, cur_nolb);
		if (rc < 0)
			goto out;

		dst_lba += cur_nolb;
		pr_debug("target_xcopy_do_work: Incremented WRITE dst_lba to %llu\n",
				(unsigned long long)dst_lba);

		copied_nolb += cur_nolb;
		nolb -= cur_nolb;
	}

	xcopy_pt_undepend_remotedev(xop);
	target_free_sgl(xop->xop_data_sg, xop->xop_data_nents);
	kfree(xop);

	pr_debug("target_xcopy_do_work: Final src_lba: %llu, dst_lba: %llu\n",
		(unsigned long long)src_lba, (unsigned long long)dst_lba);
	pr_debug("target_xcopy_do_work: Blocks copied: %hu, Bytes Copied: %u\n",
		copied_nolb, copied_nolb * dst_dev->dev_attrib.block_size);

	pr_debug("target_xcopy_do_work: Setting X-COPY GOOD status -> sending response\n");
	target_complete_cmd(ec_cmd, SAM_STAT_GOOD);
	return;

out:
	xcopy_pt_undepend_remotedev(xop);
	target_free_sgl(xop->xop_data_sg, xop->xop_data_nents);

err_free:
	kfree(xop);
	/*
	 * Don't override an error scsi status if it has already been set
	 */
	if (ec_cmd->scsi_status == SAM_STAT_GOOD) {
		pr_warn_ratelimited("target_xcopy_do_work: rc: %d, Setting X-COPY"
			" CHECK_CONDITION -> sending response\n", rc);
		ec_cmd->scsi_status = SAM_STAT_CHECK_CONDITION;
	}
	target_complete_cmd(ec_cmd, ec_cmd->scsi_status);
}

/*
 * Returns TCM_NO_SENSE upon success or a sense code != TCM_NO_SENSE if parsing
 * fails.
 */
static sense_reason_t target_parse_xcopy_cmd(struct xcopy_op *xop)
{
	struct se_cmd *se_cmd = xop->xop_se_cmd;
	unsigned char *p = NULL, *seg_desc;
	unsigned int list_id, list_id_usage, sdll, inline_dl;
	sense_reason_t ret = TCM_INVALID_PARAMETER_LIST;
	int rc;
	unsigned short tdll;

	p = transport_kmap_data_sg(se_cmd);
	if (!p) {
		pr_err("transport_kmap_data_sg() failed in target_do_xcopy\n");
		return TCM_OUT_OF_RESOURCES;
	}

	list_id = p[0];
	list_id_usage = (p[1] & 0x18) >> 3;

	/*
	 * Determine TARGET DESCRIPTOR LIST LENGTH + SEGMENT DESCRIPTOR LIST LENGTH
	 */
	tdll = get_unaligned_be16(&p[2]);
	sdll = get_unaligned_be32(&p[8]);
	if (tdll + sdll > RCR_OP_MAX_DESC_LIST_LEN) {
		pr_err("XCOPY descriptor list length %u exceeds maximum %u\n",
		       tdll + sdll, RCR_OP_MAX_DESC_LIST_LEN);
		ret = TCM_PARAMETER_LIST_LENGTH_ERROR;
		goto out;
	}

	inline_dl = get_unaligned_be32(&p[12]);
	if (inline_dl != 0) {
		pr_err("XCOPY with non zero inline data length\n");
		goto out;
	}

	if (se_cmd->data_length < (XCOPY_HDR_LEN + tdll + sdll + inline_dl)) {
		pr_err("XCOPY parameter truncation: data length %u too small "
			"for tdll: %hu sdll: %u inline_dl: %u\n",
			se_cmd->data_length, tdll, sdll, inline_dl);
		ret = TCM_PARAMETER_LIST_LENGTH_ERROR;
		goto out;
	}

	pr_debug("Processing XCOPY with list_id: 0x%02x list_id_usage: 0x%02x"
		" tdll: %hu sdll: %u inline_dl: %u\n", list_id, list_id_usage,
		tdll, sdll, inline_dl);

	/*
	 * skip over the target descriptors until segment descriptors
	 * have been passed - CSCD ids are needed to determine src and dest.
	 */
	seg_desc = &p[16] + tdll;

	rc = target_xcopy_parse_segment_descriptors(se_cmd, xop, seg_desc,
						    sdll, &ret);
	if (rc <= 0)
		goto out;

	pr_debug("XCOPY: Processed %d segment descriptors, length: %u\n", rc,
				rc * XCOPY_SEGMENT_DESC_LEN);

	rc = target_xcopy_parse_target_descriptors(se_cmd, xop, &p[16], tdll, &ret);
	if (rc <= 0)
		goto out;

	if (xop->src_dev->dev_attrib.block_size !=
	    xop->dst_dev->dev_attrib.block_size) {
		pr_err("XCOPY: Non matching src_dev block_size: %u + dst_dev"
		       " block_size: %u currently unsupported\n",
			xop->src_dev->dev_attrib.block_size,
			xop->dst_dev->dev_attrib.block_size);
		xcopy_pt_undepend_remotedev(xop);
		ret = TCM_LOGICAL_UNIT_COMMUNICATION_FAILURE;
		goto out;
	}

	pr_debug("XCOPY: Processed %d target descriptors, length: %u\n", rc,
				rc * XCOPY_TARGET_DESC_LEN);
	transport_kunmap_data_sg(se_cmd);
	return TCM_NO_SENSE;

out:
	if (p)
		transport_kunmap_data_sg(se_cmd);
	return ret;
}

sense_reason_t target_do_xcopy(struct se_cmd *se_cmd)
{
	struct se_device *dev = se_cmd->se_dev;
	struct xcopy_op *xop;
	unsigned int sa;

	if (!dev->dev_attrib.emulate_3pc) {
		pr_err("EXTENDED_COPY operation explicitly disabled\n");
		return TCM_UNSUPPORTED_SCSI_OPCODE;
	}

	sa = se_cmd->t_task_cdb[1] & 0x1f;
	if (sa != 0x00) {
		pr_err("EXTENDED_COPY(LID4) not supported\n");
		return TCM_UNSUPPORTED_SCSI_OPCODE;
	}

	if (se_cmd->data_length == 0) {
		target_complete_cmd(se_cmd, SAM_STAT_GOOD);
		return TCM_NO_SENSE;
	}
	if (se_cmd->data_length < XCOPY_HDR_LEN) {
		pr_err("XCOPY parameter truncation: length %u < hdr_len %u\n",
				se_cmd->data_length, XCOPY_HDR_LEN);
		return TCM_PARAMETER_LIST_LENGTH_ERROR;
	}

	xop = kzalloc(sizeof(struct xcopy_op), GFP_KERNEL);
	if (!xop)
		goto err;
	xop->xop_se_cmd = se_cmd;
	INIT_WORK(&xop->xop_work, target_xcopy_do_work);
	if (WARN_ON_ONCE(!queue_work(xcopy_wq, &xop->xop_work)))
		goto free;
	return TCM_NO_SENSE;

free:
	kfree(xop);

err:
	return TCM_OUT_OF_RESOURCES;
}

static sense_reason_t target_rcr_operating_parameters(struct se_cmd *se_cmd)
{
	unsigned char *p;

	p = transport_kmap_data_sg(se_cmd);
	if (!p) {
		pr_err("transport_kmap_data_sg failed in"
		       " target_rcr_operating_parameters\n");
		return TCM_OUT_OF_RESOURCES;
	}

	if (se_cmd->data_length < 54) {
		pr_err("Receive Copy Results Op Parameters length"
		       " too small: %u\n", se_cmd->data_length);
		transport_kunmap_data_sg(se_cmd);
		return TCM_INVALID_CDB_FIELD;
	}
	/*
	 * Set SNLID=1 (Supports no List ID)
	 */
	p[4] = 0x1;
	/*
	 * MAXIMUM TARGET DESCRIPTOR COUNT
	 */
	put_unaligned_be16(RCR_OP_MAX_TARGET_DESC_COUNT, &p[8]);
	/*
	 * MAXIMUM SEGMENT DESCRIPTOR COUNT
	 */
	put_unaligned_be16(RCR_OP_MAX_SG_DESC_COUNT, &p[10]);
	/*
	 * MAXIMUM DESCRIPTOR LIST LENGTH
	 */
	put_unaligned_be32(RCR_OP_MAX_DESC_LIST_LEN, &p[12]);
	/*
	 * MAXIMUM SEGMENT LENGTH
	 */
	put_unaligned_be32(RCR_OP_MAX_SEGMENT_LEN, &p[16]);
	/*
	 * MAXIMUM INLINE DATA LENGTH for SA 0x04 (NOT SUPPORTED)
	 */
	put_unaligned_be32(0x0, &p[20]);
	/*
	 * HELD DATA LIMIT
	 */
	put_unaligned_be32(0x0, &p[24]);
	/*
	 * MAXIMUM STREAM DEVICE TRANSFER SIZE
	 */
	put_unaligned_be32(0x0, &p[28]);
	/*
	 * TOTAL CONCURRENT COPIES
	 */
	put_unaligned_be16(RCR_OP_TOTAL_CONCURR_COPIES, &p[34]);
	/*
	 * MAXIMUM CONCURRENT COPIES
	 */
	p[36] = RCR_OP_MAX_CONCURR_COPIES;
	/*
	 * DATA SEGMENT GRANULARITY (log 2)
	 */
	p[37] = RCR_OP_DATA_SEG_GRAN_LOG2;
	/*
	 * INLINE DATA GRANULARITY log 2)
	 */
	p[38] = RCR_OP_INLINE_DATA_GRAN_LOG2;
	/*
	 * HELD DATA GRANULARITY
	 */
	p[39] = RCR_OP_HELD_DATA_GRAN_LOG2;
	/*
	 * IMPLEMENTED DESCRIPTOR LIST LENGTH
	 */
	p[43] = 0x2;
	/*
	 * List of implemented descriptor type codes (ordered)
	 */
	p[44] = 0x02; /* Copy Block to Block device */
	p[45] = 0xe4; /* Identification descriptor target descriptor */

	/*
	 * AVAILABLE DATA (n-3)
	 */
	put_unaligned_be32(42, &p[0]);

	transport_kunmap_data_sg(se_cmd);
	target_complete_cmd(se_cmd, GOOD);

	return TCM_NO_SENSE;
}

sense_reason_t target_do_receive_copy_results(struct se_cmd *se_cmd)
{
	unsigned char *cdb = &se_cmd->t_task_cdb[0];
	int sa = (cdb[1] & 0x1f), list_id = cdb[2];
	sense_reason_t rc = TCM_NO_SENSE;

	pr_debug("Entering target_do_receive_copy_results: SA: 0x%02x, List ID:"
		" 0x%02x, AL: %u\n", sa, list_id, se_cmd->data_length);

	if (list_id != 0) {
		pr_err("Receive Copy Results with non zero list identifier"
		       " not supported\n");
		return TCM_INVALID_CDB_FIELD;
	}

	switch (sa) {
	case RCR_SA_OPERATING_PARAMETERS:
		rc = target_rcr_operating_parameters(se_cmd);
		break;
	case RCR_SA_COPY_STATUS:
	case RCR_SA_RECEIVE_DATA:
	case RCR_SA_FAILED_SEGMENT_DETAILS:
	default:
		pr_err("Unsupported SA for receive copy results: 0x%02x\n", sa);
		return TCM_INVALID_CDB_FIELD;
	}

	return rc;
}
