// SPDX-License-Identifier: GPL-2.0
/*
 * zfcp device driver
 *
 * Interface to Linux SCSI midlayer.
 *
 * Copyright IBM Corp. 2002, 2020
 */

#define KMSG_COMPONENT "zfcp"
#define pr_fmt(fmt) KMSG_COMPONENT ": " fmt

#include <linux/module.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <scsi/fc/fc_fcp.h>
#include <scsi/scsi_eh.h>
#include <linux/atomic.h>
#include "zfcp_ext.h"
#include "zfcp_dbf.h"
#include "zfcp_fc.h"
#include "zfcp_reqlist.h"

static unsigned int default_depth = 32;
module_param_named(queue_depth, default_depth, uint, 0600);
MODULE_PARM_DESC(queue_depth, "Default queue depth for new SCSI devices");

static bool enable_dif;
module_param_named(dif, enable_dif, bool, 0400);
MODULE_PARM_DESC(dif, "Enable DIF data integrity support (default off)");

bool zfcp_experimental_dix;
module_param_named(dix, zfcp_experimental_dix, bool, 0400);
MODULE_PARM_DESC(dix, "Enable experimental DIX (data integrity extension) support which implies DIF support (default off)");

static bool allow_lun_scan = true;
module_param(allow_lun_scan, bool, 0600);
MODULE_PARM_DESC(allow_lun_scan, "For NPIV, scan and attach all storage LUNs");

static void zfcp_scsi_slave_destroy(struct scsi_device *sdev)
{
	struct zfcp_scsi_dev *zfcp_sdev = sdev_to_zfcp(sdev);

	/* if previous slave_alloc returned early, there is nothing to do */
	if (!zfcp_sdev->port)
		return;

	zfcp_erp_lun_shutdown_wait(sdev, "scssd_1");
	put_device(&zfcp_sdev->port->dev);
}

static int zfcp_scsi_slave_configure(struct scsi_device *sdp)
{
	if (sdp->tagged_supported)
		scsi_change_queue_depth(sdp, default_depth);
	return 0;
}

static void zfcp_scsi_command_fail(struct scsi_cmnd *scpnt, int result)
{
	set_host_byte(scpnt, result);
	zfcp_dbf_scsi_fail_send(scpnt);
	scpnt->scsi_done(scpnt);
}

static
int zfcp_scsi_queuecommand(struct Scsi_Host *shost, struct scsi_cmnd *scpnt)
{
	struct zfcp_scsi_dev *zfcp_sdev = sdev_to_zfcp(scpnt->device);
	struct fc_rport *rport = starget_to_rport(scsi_target(scpnt->device));
	int    status, scsi_result, ret;

	/* reset the status for this request */
	scpnt->result = 0;
	scpnt->host_scribble = NULL;

	scsi_result = fc_remote_port_chkready(rport);
	if (unlikely(scsi_result)) {
		scpnt->result = scsi_result;
		zfcp_dbf_scsi_fail_send(scpnt);
		scpnt->scsi_done(scpnt);
		return 0;
	}

	status = atomic_read(&zfcp_sdev->status);
	if (unlikely(status & ZFCP_STATUS_COMMON_ERP_FAILED) &&
		     !(atomic_read(&zfcp_sdev->port->status) &
		       ZFCP_STATUS_COMMON_ERP_FAILED)) {
		/* only LUN access denied, but port is good
		 * not covered by FC transport, have to fail here */
		zfcp_scsi_command_fail(scpnt, DID_ERROR);
		return 0;
	}

	if (unlikely(!(status & ZFCP_STATUS_COMMON_UNBLOCKED))) {
		/* This could be
		 * call to rport_delete pending: mimic retry from
		 * 	fc_remote_port_chkready until rport is BLOCKED
		 */
		zfcp_scsi_command_fail(scpnt, DID_IMM_RETRY);
		return 0;
	}

	ret = zfcp_fsf_fcp_cmnd(scpnt);
	if (unlikely(ret == -EBUSY))
		return SCSI_MLQUEUE_DEVICE_BUSY;
	else if (unlikely(ret < 0))
		return SCSI_MLQUEUE_HOST_BUSY;

	return ret;
}

static int zfcp_scsi_slave_alloc(struct scsi_device *sdev)
{
	struct fc_rport *rport = starget_to_rport(scsi_target(sdev));
	struct zfcp_adapter *adapter =
		(struct zfcp_adapter *) sdev->host->hostdata[0];
	struct zfcp_scsi_dev *zfcp_sdev = sdev_to_zfcp(sdev);
	struct zfcp_port *port;
	struct zfcp_unit *unit;
	int npiv = adapter->connection_features & FSF_FEATURE_NPIV_MODE;

	zfcp_sdev->erp_action.adapter = adapter;
	zfcp_sdev->erp_action.sdev = sdev;

	port = zfcp_get_port_by_wwpn(adapter, rport->port_name);
	if (!port)
		return -ENXIO;

	zfcp_sdev->erp_action.port = port;

	mutex_lock(&zfcp_sysfs_port_units_mutex);
	if (zfcp_sysfs_port_is_removing(port)) {
		/* port is already gone */
		mutex_unlock(&zfcp_sysfs_port_units_mutex);
		put_device(&port->dev); /* undo zfcp_get_port_by_wwpn() */
		return -ENXIO;
	}
	mutex_unlock(&zfcp_sysfs_port_units_mutex);

	unit = zfcp_unit_find(port, zfcp_scsi_dev_lun(sdev));
	if (unit)
		put_device(&unit->dev);

	if (!unit && !(allow_lun_scan && npiv)) {
		put_device(&port->dev);
		return -ENXIO;
	}

	zfcp_sdev->port = port;
	zfcp_sdev->latencies.write.channel.min = 0xFFFFFFFF;
	zfcp_sdev->latencies.write.fabric.min = 0xFFFFFFFF;
	zfcp_sdev->latencies.read.channel.min = 0xFFFFFFFF;
	zfcp_sdev->latencies.read.fabric.min = 0xFFFFFFFF;
	zfcp_sdev->latencies.cmd.channel.min = 0xFFFFFFFF;
	zfcp_sdev->latencies.cmd.fabric.min = 0xFFFFFFFF;
	spin_lock_init(&zfcp_sdev->latencies.lock);

	zfcp_erp_set_lun_status(sdev, ZFCP_STATUS_COMMON_RUNNING);
	zfcp_erp_lun_reopen(sdev, 0, "scsla_1");
	zfcp_erp_wait(port->adapter);

	return 0;
}

static int zfcp_scsi_eh_abort_handler(struct scsi_cmnd *scpnt)
{
	struct Scsi_Host *scsi_host = scpnt->device->host;
	struct zfcp_adapter *adapter =
		(struct zfcp_adapter *) scsi_host->hostdata[0];
	struct zfcp_fsf_req *old_req, *abrt_req;
	unsigned long flags;
	unsigned long old_reqid = (unsigned long) scpnt->host_scribble;
	int retval = SUCCESS, ret;
	int retry = 3;
	char *dbf_tag;

	/* avoid race condition between late normal completion and abort */
	write_lock_irqsave(&adapter->abort_lock, flags);

	old_req = zfcp_reqlist_find(adapter->req_list, old_reqid);
	if (!old_req) {
		write_unlock_irqrestore(&adapter->abort_lock, flags);
		zfcp_dbf_scsi_abort("abrt_or", scpnt, NULL);
		return FAILED; /* completion could be in progress */
	}
	old_req->data = NULL;

	/* don't access old fsf_req after releasing the abort_lock */
	write_unlock_irqrestore(&adapter->abort_lock, flags);

	while (retry--) {
		abrt_req = zfcp_fsf_abort_fcp_cmnd(scpnt);
		if (abrt_req)
			break;

		zfcp_dbf_scsi_abort("abrt_wt", scpnt, NULL);
		zfcp_erp_wait(adapter);
		ret = fc_block_scsi_eh(scpnt);
		if (ret) {
			zfcp_dbf_scsi_abort("abrt_bl", scpnt, NULL);
			return ret;
		}
		if (!(atomic_read(&adapter->status) &
		      ZFCP_STATUS_COMMON_RUNNING)) {
			zfcp_dbf_scsi_abort("abrt_ru", scpnt, NULL);
			return SUCCESS;
		}
	}
	if (!abrt_req) {
		zfcp_dbf_scsi_abort("abrt_ar", scpnt, NULL);
		return FAILED;
	}

	wait_for_completion(&abrt_req->completion);

	if (abrt_req->status & ZFCP_STATUS_FSFREQ_ABORTSUCCEEDED)
		dbf_tag = "abrt_ok";
	else if (abrt_req->status & ZFCP_STATUS_FSFREQ_ABORTNOTNEEDED)
		dbf_tag = "abrt_nn";
	else {
		dbf_tag = "abrt_fa";
		retval = FAILED;
	}
	zfcp_dbf_scsi_abort(dbf_tag, scpnt, abrt_req);
	zfcp_fsf_req_free(abrt_req);
	return retval;
}

struct zfcp_scsi_req_filter {
	u8 tmf_scope;
	u32 lun_handle;
	u32 port_handle;
};

static void zfcp_scsi_forget_cmnd(struct zfcp_fsf_req *old_req, void *data)
{
	struct zfcp_scsi_req_filter *filter =
		(struct zfcp_scsi_req_filter *)data;

	/* already aborted - prevent side-effects - or not a SCSI command */
	if (old_req->data == NULL ||
	    zfcp_fsf_req_is_status_read_buffer(old_req) ||
	    old_req->qtcb->header.fsf_command != FSF_QTCB_FCP_CMND)
		return;

	/* (tmf_scope == FCP_TMF_TGT_RESET || tmf_scope == FCP_TMF_LUN_RESET) */
	if (old_req->qtcb->header.port_handle != filter->port_handle)
		return;

	if (filter->tmf_scope == FCP_TMF_LUN_RESET &&
	    old_req->qtcb->header.lun_handle != filter->lun_handle)
		return;

	zfcp_dbf_scsi_nullcmnd((struct scsi_cmnd *)old_req->data, old_req);
	old_req->data = NULL;
}

static void zfcp_scsi_forget_cmnds(struct zfcp_scsi_dev *zsdev, u8 tm_flags)
{
	struct zfcp_adapter *adapter = zsdev->port->adapter;
	struct zfcp_scsi_req_filter filter = {
		.tmf_scope = FCP_TMF_TGT_RESET,
		.port_handle = zsdev->port->handle,
	};
	unsigned long flags;

	if (tm_flags == FCP_TMF_LUN_RESET) {
		filter.tmf_scope = FCP_TMF_LUN_RESET;
		filter.lun_handle = zsdev->lun_handle;
	}

	/*
	 * abort_lock secures against other processings - in the abort-function
	 * and normal cmnd-handler - of (struct zfcp_fsf_req *)->data
	 */
	write_lock_irqsave(&adapter->abort_lock, flags);
	zfcp_reqlist_apply_for_all(adapter->req_list, zfcp_scsi_forget_cmnd,
				   &filter);
	write_unlock_irqrestore(&adapter->abort_lock, flags);
}

/**
 * zfcp_scsi_task_mgmt_function() - Send a task management function (sync).
 * @sdev: Pointer to SCSI device to send the task management command to.
 * @tm_flags: Task management flags,
 *	      here we only handle %FCP_TMF_TGT_RESET or %FCP_TMF_LUN_RESET.
 */
static int zfcp_scsi_task_mgmt_function(struct scsi_device *sdev, u8 tm_flags)
{
	struct zfcp_scsi_dev *zfcp_sdev = sdev_to_zfcp(sdev);
	struct zfcp_adapter *adapter = zfcp_sdev->port->adapter;
	struct fc_rport *rport = starget_to_rport(scsi_target(sdev));
	struct zfcp_fsf_req *fsf_req = NULL;
	int retval = SUCCESS, ret;
	int retry = 3;

	while (retry--) {
		fsf_req = zfcp_fsf_fcp_task_mgmt(sdev, tm_flags);
		if (fsf_req)
			break;

		zfcp_dbf_scsi_devreset("wait", sdev, tm_flags, NULL);
		zfcp_erp_wait(adapter);
		ret = fc_block_rport(rport);
		if (ret) {
			zfcp_dbf_scsi_devreset("fiof", sdev, tm_flags, NULL);
			return ret;
		}

		if (!(atomic_read(&adapter->status) &
		      ZFCP_STATUS_COMMON_RUNNING)) {
			zfcp_dbf_scsi_devreset("nres", sdev, tm_flags, NULL);
			return SUCCESS;
		}
	}
	if (!fsf_req) {
		zfcp_dbf_scsi_devreset("reqf", sdev, tm_flags, NULL);
		return FAILED;
	}

	wait_for_completion(&fsf_req->completion);

	if (fsf_req->status & ZFCP_STATUS_FSFREQ_TMFUNCFAILED) {
		zfcp_dbf_scsi_devreset("fail", sdev, tm_flags, fsf_req);
		retval = FAILED;
	} else {
		zfcp_dbf_scsi_devreset("okay", sdev, tm_flags, fsf_req);
		zfcp_scsi_forget_cmnds(zfcp_sdev, tm_flags);
	}

	zfcp_fsf_req_free(fsf_req);
	return retval;
}

static int zfcp_scsi_eh_device_reset_handler(struct scsi_cmnd *scpnt)
{
	struct scsi_device *sdev = scpnt->device;

	return zfcp_scsi_task_mgmt_function(sdev, FCP_TMF_LUN_RESET);
}

static int zfcp_scsi_eh_target_reset_handler(struct scsi_cmnd *scpnt)
{
	struct scsi_target *starget = scsi_target(scpnt->device);
	struct fc_rport *rport = starget_to_rport(starget);
	struct Scsi_Host *shost = rport_to_shost(rport);
	struct scsi_device *sdev = NULL, *tmp_sdev;
	struct zfcp_adapter *adapter =
		(struct zfcp_adapter *)shost->hostdata[0];
	int ret;

	shost_for_each_device(tmp_sdev, shost) {
		if (tmp_sdev->id == starget->id) {
			sdev = tmp_sdev;
			break;
		}
	}
	if (!sdev) {
		ret = FAILED;
		zfcp_dbf_scsi_eh("tr_nosd", adapter, starget->id, ret);
		return ret;
	}

	ret = zfcp_scsi_task_mgmt_function(sdev, FCP_TMF_TGT_RESET);

	/* release reference from above shost_for_each_device */
	if (sdev)
		scsi_device_put(tmp_sdev);

	return ret;
}

static int zfcp_scsi_eh_host_reset_handler(struct scsi_cmnd *scpnt)
{
	struct zfcp_scsi_dev *zfcp_sdev = sdev_to_zfcp(scpnt->device);
	struct zfcp_adapter *adapter = zfcp_sdev->port->adapter;
	int ret = SUCCESS, fc_ret;

	if (!(adapter->connection_features & FSF_FEATURE_NPIV_MODE)) {
		zfcp_erp_port_forced_reopen_all(adapter, 0, "schrh_p");
		zfcp_erp_wait(adapter);
	}
	zfcp_erp_adapter_reopen(adapter, 0, "schrh_1");
	zfcp_erp_wait(adapter);
	fc_ret = fc_block_scsi_eh(scpnt);
	if (fc_ret)
		ret = fc_ret;

	zfcp_dbf_scsi_eh("schrh_r", adapter, ~0, ret);
	return ret;
}

/**
 * zfcp_scsi_sysfs_host_reset() - Support scsi_host sysfs attribute host_reset.
 * @shost: Pointer to Scsi_Host to perform action on.
 * @reset_type: We support %SCSI_ADAPTER_RESET but not %SCSI_FIRMWARE_RESET.
 *
 * Return: 0 on %SCSI_ADAPTER_RESET, -%EOPNOTSUPP otherwise.
 *
 * This is similar to zfcp_sysfs_adapter_failed_store().
 */
static int zfcp_scsi_sysfs_host_reset(struct Scsi_Host *shost, int reset_type)
{
	struct zfcp_adapter *adapter =
		(struct zfcp_adapter *)shost->hostdata[0];
	int ret = 0;

	if (reset_type != SCSI_ADAPTER_RESET) {
		ret = -EOPNOTSUPP;
		zfcp_dbf_scsi_eh("scshr_n", adapter, ~0, ret);
		return ret;
	}

	zfcp_erp_adapter_reset_sync(adapter, "scshr_y");
	return ret;
}

struct scsi_transport_template *zfcp_scsi_transport_template;

static struct scsi_host_template zfcp_scsi_host_template = {
	.module			 = THIS_MODULE,
	.name			 = "zfcp",
	.queuecommand		 = zfcp_scsi_queuecommand,
	.eh_timed_out		 = fc_eh_timed_out,
	.eh_abort_handler	 = zfcp_scsi_eh_abort_handler,
	.eh_device_reset_handler = zfcp_scsi_eh_device_reset_handler,
	.eh_target_reset_handler = zfcp_scsi_eh_target_reset_handler,
	.eh_host_reset_handler	 = zfcp_scsi_eh_host_reset_handler,
	.slave_alloc		 = zfcp_scsi_slave_alloc,
	.slave_configure	 = zfcp_scsi_slave_configure,
	.slave_destroy		 = zfcp_scsi_slave_destroy,
	.change_queue_depth	 = scsi_change_queue_depth,
	.host_reset		 = zfcp_scsi_sysfs_host_reset,
	.proc_name		 = "zfcp",
	.can_queue		 = 4096,
	.this_id		 = -1,
	.sg_tablesize		 = (((QDIO_MAX_ELEMENTS_PER_BUFFER - 1)
				     * ZFCP_QDIO_MAX_SBALS_PER_REQ) - 2),
				   /* GCD, adjusted later */
	.max_sectors		 = (((QDIO_MAX_ELEMENTS_PER_BUFFER - 1)
				     * ZFCP_QDIO_MAX_SBALS_PER_REQ) - 2) * 8,
				   /* GCD, adjusted later */
	/* report size limit per scatter-gather segment */
	.max_segment_size	 = ZFCP_QDIO_SBALE_LEN,
	.dma_boundary		 = ZFCP_QDIO_SBALE_LEN - 1,
	.shost_attrs		 = zfcp_sysfs_shost_attrs,
	.sdev_attrs		 = zfcp_sysfs_sdev_attrs,
	.track_queue_depth	 = 1,
	.supported_mode		 = MODE_INITIATOR,
};

/**
 * zfcp_scsi_adapter_register() - Allocate and register SCSI and FC host with
 *				  SCSI midlayer
 * @adapter: The zfcp adapter to register with the SCSI midlayer
 *
 * Allocates the SCSI host object for the given adapter, sets basic properties
 * (such as the transport template, QDIO limits, ...), and registers it with
 * the midlayer.
 *
 * During registration with the midlayer the corresponding FC host object for
 * the referenced transport class is also implicitely allocated.
 *
 * Upon success adapter->scsi_host is set, and upon failure it remains NULL. If
 * adapter->scsi_host is already set, nothing is done.
 *
 * Return:
 * * 0	     - Allocation and registration was successful
 * * -EEXIST - SCSI and FC host did already exist, nothing was done, nothing
 *	       was changed
 * * -EIO    - Allocation or registration failed
 */
int zfcp_scsi_adapter_register(struct zfcp_adapter *adapter)
{
	struct ccw_dev_id dev_id;

	if (adapter->scsi_host)
		return -EEXIST;

	ccw_device_get_id(adapter->ccw_device, &dev_id);
	/* register adapter as SCSI host with mid layer of SCSI stack */
	adapter->scsi_host = scsi_host_alloc(&zfcp_scsi_host_template,
					     sizeof (struct zfcp_adapter *));
	if (!adapter->scsi_host)
		goto err_out;

	/* tell the SCSI stack some characteristics of this adapter */
	adapter->scsi_host->max_id = 511;
	adapter->scsi_host->max_lun = 0xFFFFFFFF;
	adapter->scsi_host->max_channel = 0;
	adapter->scsi_host->unique_id = dev_id.devno;
	adapter->scsi_host->max_cmd_len = 16; /* in struct fcp_cmnd */
	adapter->scsi_host->transportt = zfcp_scsi_transport_template;

	/* make all basic properties known at registration time */
	zfcp_qdio_shost_update(adapter, adapter->qdio);
	zfcp_scsi_set_prot(adapter);

	adapter->scsi_host->hostdata[0] = (unsigned long) adapter;

	if (scsi_add_host(adapter->scsi_host, &adapter->ccw_device->dev)) {
		scsi_host_put(adapter->scsi_host);
		goto err_out;
	}

	return 0;
err_out:
	adapter->scsi_host = NULL;
	dev_err(&adapter->ccw_device->dev,
		"Registering the FCP device with the SCSI stack failed\n");
	return -EIO;
}

/**
 * zfcp_scsi_adapter_unregister - Unregister SCSI and FC host from SCSI midlayer
 * @adapter: The zfcp adapter to unregister.
 */
void zfcp_scsi_adapter_unregister(struct zfcp_adapter *adapter)
{
	struct Scsi_Host *shost;
	struct zfcp_port *port;

	shost = adapter->scsi_host;
	if (!shost)
		return;

	read_lock_irq(&adapter->port_list_lock);
	list_for_each_entry(port, &adapter->port_list, list)
		port->rport = NULL;
	read_unlock_irq(&adapter->port_list_lock);

	fc_remove_host(shost);
	scsi_remove_host(shost);
	scsi_host_put(shost);
	adapter->scsi_host = NULL;
}

static struct fc_host_statistics*
zfcp_scsi_init_fc_host_stats(struct zfcp_adapter *adapter)
{
	struct fc_host_statistics *fc_stats;

	if (!adapter->fc_stats) {
		fc_stats = kmalloc(sizeof(*fc_stats), GFP_KERNEL);
		if (!fc_stats)
			return NULL;
		adapter->fc_stats = fc_stats; /* freed in adapter_release */
	}
	memset(adapter->fc_stats, 0, sizeof(*adapter->fc_stats));
	return adapter->fc_stats;
}

static void zfcp_scsi_adjust_fc_host_stats(struct fc_host_statistics *fc_stats,
					   struct fsf_qtcb_bottom_port *data,
					   struct fsf_qtcb_bottom_port *old)
{
	fc_stats->seconds_since_last_reset =
		data->seconds_since_last_reset - old->seconds_since_last_reset;
	fc_stats->tx_frames = data->tx_frames - old->tx_frames;
	fc_stats->tx_words = data->tx_words - old->tx_words;
	fc_stats->rx_frames = data->rx_frames - old->rx_frames;
	fc_stats->rx_words = data->rx_words - old->rx_words;
	fc_stats->lip_count = data->lip - old->lip;
	fc_stats->nos_count = data->nos - old->nos;
	fc_stats->error_frames = data->error_frames - old->error_frames;
	fc_stats->dumped_frames = data->dumped_frames - old->dumped_frames;
	fc_stats->link_failure_count = data->link_failure - old->link_failure;
	fc_stats->loss_of_sync_count = data->loss_of_sync - old->loss_of_sync;
	fc_stats->loss_of_signal_count =
		data->loss_of_signal - old->loss_of_signal;
	fc_stats->prim_seq_protocol_err_count =
		data->psp_error_counts - old->psp_error_counts;
	fc_stats->invalid_tx_word_count =
		data->invalid_tx_words - old->invalid_tx_words;
	fc_stats->invalid_crc_count = data->invalid_crcs - old->invalid_crcs;
	fc_stats->fcp_input_requests =
		data->input_requests - old->input_requests;
	fc_stats->fcp_output_requests =
		data->output_requests - old->output_requests;
	fc_stats->fcp_control_requests =
		data->control_requests - old->control_requests;
	fc_stats->fcp_input_megabytes = data->input_mb - old->input_mb;
	fc_stats->fcp_output_megabytes = data->output_mb - old->output_mb;
}

static void zfcp_scsi_set_fc_host_stats(struct fc_host_statistics *fc_stats,
					struct fsf_qtcb_bottom_port *data)
{
	fc_stats->seconds_since_last_reset = data->seconds_since_last_reset;
	fc_stats->tx_frames = data->tx_frames;
	fc_stats->tx_words = data->tx_words;
	fc_stats->rx_frames = data->rx_frames;
	fc_stats->rx_words = data->rx_words;
	fc_stats->lip_count = data->lip;
	fc_stats->nos_count = data->nos;
	fc_stats->error_frames = data->error_frames;
	fc_stats->dumped_frames = data->dumped_frames;
	fc_stats->link_failure_count = data->link_failure;
	fc_stats->loss_of_sync_count = data->loss_of_sync;
	fc_stats->loss_of_signal_count = data->loss_of_signal;
	fc_stats->prim_seq_protocol_err_count = data->psp_error_counts;
	fc_stats->invalid_tx_word_count = data->invalid_tx_words;
	fc_stats->invalid_crc_count = data->invalid_crcs;
	fc_stats->fcp_input_requests = data->input_requests;
	fc_stats->fcp_output_requests = data->output_requests;
	fc_stats->fcp_control_requests = data->control_requests;
	fc_stats->fcp_input_megabytes = data->input_mb;
	fc_stats->fcp_output_megabytes = data->output_mb;
}

static struct fc_host_statistics *
zfcp_scsi_get_fc_host_stats(struct Scsi_Host *host)
{
	struct zfcp_adapter *adapter;
	struct fc_host_statistics *fc_stats;
	struct fsf_qtcb_bottom_port *data;
	int ret;

	adapter = (struct zfcp_adapter *)host->hostdata[0];
	fc_stats = zfcp_scsi_init_fc_host_stats(adapter);
	if (!fc_stats)
		return NULL;

	data = kzalloc(sizeof(*data), GFP_KERNEL);
	if (!data)
		return NULL;

	ret = zfcp_fsf_exchange_port_data_sync(adapter->qdio, data);
	if (ret != 0 && ret != -EAGAIN) {
		kfree(data);
		return NULL;
	}

	if (adapter->stats_reset &&
	    ((jiffies/HZ - adapter->stats_reset) <
	     data->seconds_since_last_reset))
		zfcp_scsi_adjust_fc_host_stats(fc_stats, data,
					       adapter->stats_reset_data);
	else
		zfcp_scsi_set_fc_host_stats(fc_stats, data);

	kfree(data);
	return fc_stats;
}

static void zfcp_scsi_reset_fc_host_stats(struct Scsi_Host *shost)
{
	struct zfcp_adapter *adapter;
	struct fsf_qtcb_bottom_port *data;
	int ret;

	adapter = (struct zfcp_adapter *)shost->hostdata[0];
	data = kzalloc(sizeof(*data), GFP_KERNEL);
	if (!data)
		return;

	ret = zfcp_fsf_exchange_port_data_sync(adapter->qdio, data);
	if (ret != 0 && ret != -EAGAIN)
		kfree(data);
	else {
		adapter->stats_reset = jiffies/HZ;
		kfree(adapter->stats_reset_data);
		adapter->stats_reset_data = data; /* finally freed in
						     adapter_release */
	}
}

static void zfcp_scsi_get_host_port_state(struct Scsi_Host *shost)
{
	struct zfcp_adapter *adapter =
		(struct zfcp_adapter *)shost->hostdata[0];
	int status = atomic_read(&adapter->status);

	if ((status & ZFCP_STATUS_COMMON_RUNNING) &&
	    !(status & ZFCP_STATUS_ADAPTER_LINK_UNPLUGGED))
		fc_host_port_state(shost) = FC_PORTSTATE_ONLINE;
	else if (status & ZFCP_STATUS_ADAPTER_LINK_UNPLUGGED)
		fc_host_port_state(shost) = FC_PORTSTATE_LINKDOWN;
	else if (status & ZFCP_STATUS_COMMON_ERP_FAILED)
		fc_host_port_state(shost) = FC_PORTSTATE_ERROR;
	else
		fc_host_port_state(shost) = FC_PORTSTATE_UNKNOWN;
}

static void zfcp_scsi_set_rport_dev_loss_tmo(struct fc_rport *rport,
					     u32 timeout)
{
	rport->dev_loss_tmo = timeout;
}

/**
 * zfcp_scsi_terminate_rport_io - Terminate all I/O on a rport
 * @rport: The FC rport where to teminate I/O
 *
 * Abort all pending SCSI commands for a port by closing the
 * port. Using a reopen avoids a conflict with a shutdown
 * overwriting a reopen. The "forced" ensures that a disappeared port
 * is not opened again as valid due to the cached plogi data in
 * non-NPIV mode.
 */
static void zfcp_scsi_terminate_rport_io(struct fc_rport *rport)
{
	struct zfcp_port *port;
	struct Scsi_Host *shost = rport_to_shost(rport);
	struct zfcp_adapter *adapter =
		(struct zfcp_adapter *)shost->hostdata[0];

	port = zfcp_get_port_by_wwpn(adapter, rport->port_name);

	if (port) {
		zfcp_erp_port_forced_reopen(port, 0, "sctrpi1");
		put_device(&port->dev);
	} else {
		zfcp_erp_port_forced_no_port_dbf(
			"sctrpin", adapter,
			rport->port_name /* zfcp_scsi_rport_register */,
			rport->port_id /* zfcp_scsi_rport_register */);
	}
}

static void zfcp_scsi_rport_register(struct zfcp_port *port)
{
	struct fc_rport_identifiers ids;
	struct fc_rport *rport;

	if (port->rport)
		return;

	ids.node_name = port->wwnn;
	ids.port_name = port->wwpn;
	ids.port_id = port->d_id;
	ids.roles = FC_RPORT_ROLE_FCP_TARGET;

	zfcp_dbf_rec_trig_lock("scpaddy", port->adapter, port, NULL,
			       ZFCP_PSEUDO_ERP_ACTION_RPORT_ADD,
			       ZFCP_PSEUDO_ERP_ACTION_RPORT_ADD);
	rport = fc_remote_port_add(port->adapter->scsi_host, 0, &ids);
	if (!rport) {
		dev_err(&port->adapter->ccw_device->dev,
			"Registering port 0x%016Lx failed\n",
			(unsigned long long)port->wwpn);
		return;
	}

	rport->maxframe_size = port->maxframe_size;
	rport->supported_classes = port->supported_classes;
	port->rport = rport;
	port->starget_id = rport->scsi_target_id;

	zfcp_unit_queue_scsi_scan(port);
}

static void zfcp_scsi_rport_block(struct zfcp_port *port)
{
	struct fc_rport *rport = port->rport;

	if (rport) {
		zfcp_dbf_rec_trig_lock("scpdely", port->adapter, port, NULL,
				       ZFCP_PSEUDO_ERP_ACTION_RPORT_DEL,
				       ZFCP_PSEUDO_ERP_ACTION_RPORT_DEL);
		fc_remote_port_delete(rport);
		port->rport = NULL;
	}
}

void zfcp_scsi_schedule_rport_register(struct zfcp_port *port)
{
	get_device(&port->dev);
	port->rport_task = RPORT_ADD;

	if (!queue_work(port->adapter->work_queue, &port->rport_work))
		put_device(&port->dev);
}

void zfcp_scsi_schedule_rport_block(struct zfcp_port *port)
{
	get_device(&port->dev);
	port->rport_task = RPORT_DEL;

	if (port->rport && queue_work(port->adapter->work_queue,
				      &port->rport_work))
		return;

	put_device(&port->dev);
}

void zfcp_scsi_schedule_rports_block(struct zfcp_adapter *adapter)
{
	unsigned long flags;
	struct zfcp_port *port;

	read_lock_irqsave(&adapter->port_list_lock, flags);
	list_for_each_entry(port, &adapter->port_list, list)
		zfcp_scsi_schedule_rport_block(port);
	read_unlock_irqrestore(&adapter->port_list_lock, flags);
}

void zfcp_scsi_rport_work(struct work_struct *work)
{
	struct zfcp_port *port = container_of(work, struct zfcp_port,
					      rport_work);

	set_worker_desc("zrp%c-%16llx",
			(port->rport_task == RPORT_ADD) ? 'a' : 'd',
			port->wwpn); /* < WORKER_DESC_LEN=24 */
	while (port->rport_task) {
		if (port->rport_task == RPORT_ADD) {
			port->rport_task = RPORT_NONE;
			zfcp_scsi_rport_register(port);
		} else {
			port->rport_task = RPORT_NONE;
			zfcp_scsi_rport_block(port);
		}
	}

	put_device(&port->dev);
}

/**
 * zfcp_scsi_set_prot - Configure DIF/DIX support in scsi_host
 * @adapter: The adapter where to configure DIF/DIX for the SCSI host
 */
void zfcp_scsi_set_prot(struct zfcp_adapter *adapter)
{
	unsigned int mask = 0;
	unsigned int data_div;
	struct Scsi_Host *shost = adapter->scsi_host;

	data_div = atomic_read(&adapter->status) &
		   ZFCP_STATUS_ADAPTER_DATA_DIV_ENABLED;

	if ((enable_dif || zfcp_experimental_dix) &&
	    adapter->adapter_features & FSF_FEATURE_DIF_PROT_TYPE1)
		mask |= SHOST_DIF_TYPE1_PROTECTION;

	if (zfcp_experimental_dix && data_div &&
	    adapter->adapter_features & FSF_FEATURE_DIX_PROT_TCPIP) {
		mask |= SHOST_DIX_TYPE1_PROTECTION;
		scsi_host_set_guard(shost, SHOST_DIX_GUARD_IP);
		shost->sg_prot_tablesize = adapter->qdio->max_sbale_per_req / 2;
		shost->sg_tablesize = adapter->qdio->max_sbale_per_req / 2;
		shost->max_sectors = shost->sg_tablesize * 8;
	}

	scsi_host_set_prot(shost, mask);
}

/**
 * zfcp_scsi_dif_sense_error - Report DIF/DIX error as driver sense error
 * @scmd: The SCSI command to report the error for
 * @ascq: The ASCQ to put in the sense buffer
 *
 * See the error handling in sd_done for the sense codes used here.
 * Set DID_SOFT_ERROR to retry the request, if possible.
 */
void zfcp_scsi_dif_sense_error(struct scsi_cmnd *scmd, int ascq)
{
	scsi_build_sense_buffer(1, scmd->sense_buffer,
				ILLEGAL_REQUEST, 0x10, ascq);
	set_driver_byte(scmd, DRIVER_SENSE);
	scmd->result |= SAM_STAT_CHECK_CONDITION;
	set_host_byte(scmd, DID_SOFT_ERROR);
}

void zfcp_scsi_shost_update_config_data(
	struct zfcp_adapter *const adapter,
	const struct fsf_qtcb_bottom_config *const bottom,
	const bool bottom_incomplete)
{
	struct Scsi_Host *const shost = adapter->scsi_host;
	const struct fc_els_flogi *nsp, *plogi;

	if (shost == NULL)
		return;

	snprintf(fc_host_firmware_version(shost), FC_VERSION_STRING_SIZE,
		 "0x%08x", bottom->lic_version);

	if (adapter->adapter_features & FSF_FEATURE_HBAAPI_MANAGEMENT) {
		snprintf(fc_host_hardware_version(shost),
			 FC_VERSION_STRING_SIZE,
			 "0x%08x", bottom->hardware_version);
		memcpy(fc_host_serial_number(shost), bottom->serial_number,
		       min(FC_SERIAL_NUMBER_SIZE, 17));
		EBCASC(fc_host_serial_number(shost),
		       min(FC_SERIAL_NUMBER_SIZE, 17));
	}

	/* adjust pointers for missing command code */
	nsp = (struct fc_els_flogi *) ((u8 *)&bottom->nport_serv_param
					- sizeof(u32));
	plogi = (struct fc_els_flogi *) ((u8 *)&bottom->plogi_payload
					- sizeof(u32));

	snprintf(fc_host_manufacturer(shost), FC_SERIAL_NUMBER_SIZE, "%s",
		 "IBM");
	fc_host_port_name(shost) = be64_to_cpu(nsp->fl_wwpn);
	fc_host_node_name(shost) = be64_to_cpu(nsp->fl_wwnn);
	fc_host_supported_classes(shost) = FC_COS_CLASS2 | FC_COS_CLASS3;

	zfcp_scsi_set_prot(adapter);

	/* do not evaluate invalid fields */
	if (bottom_incomplete)
		return;

	fc_host_port_id(shost) = ntoh24(bottom->s_id);
	fc_host_speed(shost) =
		zfcp_fsf_convert_portspeed(bottom->fc_link_speed);

	snprintf(fc_host_model(shost), FC_SYMBOLIC_NAME_SIZE, "0x%04x",
		 bottom->adapter_type);

	switch (bottom->fc_topology) {
	case FSF_TOPO_P2P:
		fc_host_port_type(shost) = FC_PORTTYPE_PTP;
		fc_host_fabric_name(shost) = 0;
		break;
	case FSF_TOPO_FABRIC:
		fc_host_fabric_name(shost) = be64_to_cpu(plogi->fl_wwnn);
		if (bottom->connection_features & FSF_FEATURE_NPIV_MODE)
			fc_host_port_type(shost) = FC_PORTTYPE_NPIV;
		else
			fc_host_port_type(shost) = FC_PORTTYPE_NPORT;
		break;
	case FSF_TOPO_AL:
		fc_host_port_type(shost) = FC_PORTTYPE_NLPORT;
		fallthrough;
	default:
		fc_host_fabric_name(shost) = 0;
		break;
	}
}

void zfcp_scsi_shost_update_port_data(
	struct zfcp_adapter *const adapter,
	const struct fsf_qtcb_bottom_port *const bottom)
{
	struct Scsi_Host *const shost = adapter->scsi_host;

	if (shost == NULL)
		return;

	fc_host_permanent_port_name(shost) = bottom->wwpn;
	fc_host_maxframe_size(shost) = bottom->maximum_frame_size;
	fc_host_supported_speeds(shost) =
		zfcp_fsf_convert_portspeed(bottom->supported_speed);
	memcpy(fc_host_supported_fc4s(shost), bottom->supported_fc4_types,
	       FC_FC4_LIST_SIZE);
	memcpy(fc_host_active_fc4s(shost), bottom->active_fc4_types,
	       FC_FC4_LIST_SIZE);
}

struct fc_function_template zfcp_transport_functions = {
	.show_starget_port_id = 1,
	.show_starget_port_name = 1,
	.show_starget_node_name = 1,
	.show_rport_supported_classes = 1,
	.show_rport_maxframe_size = 1,
	.show_rport_dev_loss_tmo = 1,
	.show_host_node_name = 1,
	.show_host_port_name = 1,
	.show_host_permanent_port_name = 1,
	.show_host_supported_classes = 1,
	.show_host_supported_fc4s = 1,
	.show_host_supported_speeds = 1,
	.show_host_maxframe_size = 1,
	.show_host_serial_number = 1,
	.show_host_manufacturer = 1,
	.show_host_model = 1,
	.show_host_hardware_version = 1,
	.show_host_firmware_version = 1,
	.get_fc_host_stats = zfcp_scsi_get_fc_host_stats,
	.reset_fc_host_stats = zfcp_scsi_reset_fc_host_stats,
	.set_rport_dev_loss_tmo = zfcp_scsi_set_rport_dev_loss_tmo,
	.get_host_port_state = zfcp_scsi_get_host_port_state,
	.terminate_rport_io = zfcp_scsi_terminate_rport_io,
	.show_host_port_state = 1,
	.show_host_active_fc4s = 1,
	.bsg_request = zfcp_fc_exec_bsg_job,
	.bsg_timeout = zfcp_fc_timeout_bsg_job,
	/* no functions registered for following dynamic attributes but
	   directly set by LLDD */
	.show_host_port_type = 1,
	.show_host_symbolic_name = 1,
	.show_host_speed = 1,
	.show_host_port_id = 1,
	.show_host_fabric_name = 1,
	.dd_bsg_size = sizeof(struct zfcp_fsf_ct_els),
};
