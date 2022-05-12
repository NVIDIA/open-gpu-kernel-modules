// SPDX-License-Identifier: GPL-2.0
/*
 * zfcp device driver
 *
 * Implementation of FSF commands.
 *
 * Copyright IBM Corp. 2002, 2020
 */

#define KMSG_COMPONENT "zfcp"
#define pr_fmt(fmt) KMSG_COMPONENT ": " fmt

#include <linux/blktrace_api.h>
#include <linux/jiffies.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <scsi/fc/fc_els.h>
#include "zfcp_ext.h"
#include "zfcp_fc.h"
#include "zfcp_dbf.h"
#include "zfcp_qdio.h"
#include "zfcp_reqlist.h"
#include "zfcp_diag.h"

/* timeout for FSF requests sent during scsi_eh: abort or FCP TMF */
#define ZFCP_FSF_SCSI_ER_TIMEOUT (10*HZ)
/* timeout for: exchange config/port data outside ERP, or open/close WKA port */
#define ZFCP_FSF_REQUEST_TIMEOUT (60*HZ)

struct kmem_cache *zfcp_fsf_qtcb_cache;

static bool ber_stop = true;
module_param(ber_stop, bool, 0600);
MODULE_PARM_DESC(ber_stop,
		 "Shuts down FCP devices for FCP channels that report a bit-error count in excess of its threshold (default on)");

static void zfcp_fsf_request_timeout_handler(struct timer_list *t)
{
	struct zfcp_fsf_req *fsf_req = from_timer(fsf_req, t, timer);
	struct zfcp_adapter *adapter = fsf_req->adapter;

	zfcp_qdio_siosl(adapter);
	zfcp_erp_adapter_reopen(adapter, ZFCP_STATUS_COMMON_ERP_FAILED,
				"fsrth_1");
}

static void zfcp_fsf_start_timer(struct zfcp_fsf_req *fsf_req,
				 unsigned long timeout)
{
	fsf_req->timer.function = zfcp_fsf_request_timeout_handler;
	fsf_req->timer.expires = jiffies + timeout;
	add_timer(&fsf_req->timer);
}

static void zfcp_fsf_start_erp_timer(struct zfcp_fsf_req *fsf_req)
{
	BUG_ON(!fsf_req->erp_action);
	fsf_req->timer.function = zfcp_erp_timeout_handler;
	fsf_req->timer.expires = jiffies + 30 * HZ;
	add_timer(&fsf_req->timer);
}

/* association between FSF command and FSF QTCB type */
static u32 fsf_qtcb_type[] = {
	[FSF_QTCB_FCP_CMND] =             FSF_IO_COMMAND,
	[FSF_QTCB_ABORT_FCP_CMND] =       FSF_SUPPORT_COMMAND,
	[FSF_QTCB_OPEN_PORT_WITH_DID] =   FSF_SUPPORT_COMMAND,
	[FSF_QTCB_OPEN_LUN] =             FSF_SUPPORT_COMMAND,
	[FSF_QTCB_CLOSE_LUN] =            FSF_SUPPORT_COMMAND,
	[FSF_QTCB_CLOSE_PORT] =           FSF_SUPPORT_COMMAND,
	[FSF_QTCB_CLOSE_PHYSICAL_PORT] =  FSF_SUPPORT_COMMAND,
	[FSF_QTCB_SEND_ELS] =             FSF_SUPPORT_COMMAND,
	[FSF_QTCB_SEND_GENERIC] =         FSF_SUPPORT_COMMAND,
	[FSF_QTCB_EXCHANGE_CONFIG_DATA] = FSF_CONFIG_COMMAND,
	[FSF_QTCB_EXCHANGE_PORT_DATA] =   FSF_PORT_COMMAND,
	[FSF_QTCB_DOWNLOAD_CONTROL_FILE] = FSF_SUPPORT_COMMAND,
	[FSF_QTCB_UPLOAD_CONTROL_FILE] =  FSF_SUPPORT_COMMAND
};

static void zfcp_fsf_class_not_supp(struct zfcp_fsf_req *req)
{
	dev_err(&req->adapter->ccw_device->dev, "FCP device not "
		"operational because of an unsupported FC class\n");
	zfcp_erp_adapter_shutdown(req->adapter, 0, "fscns_1");
	req->status |= ZFCP_STATUS_FSFREQ_ERROR;
}

/**
 * zfcp_fsf_req_free - free memory used by fsf request
 * @req: pointer to struct zfcp_fsf_req
 */
void zfcp_fsf_req_free(struct zfcp_fsf_req *req)
{
	if (likely(req->pool)) {
		if (likely(!zfcp_fsf_req_is_status_read_buffer(req)))
			mempool_free(req->qtcb, req->adapter->pool.qtcb_pool);
		mempool_free(req, req->pool);
		return;
	}

	if (likely(!zfcp_fsf_req_is_status_read_buffer(req)))
		kmem_cache_free(zfcp_fsf_qtcb_cache, req->qtcb);
	kfree(req);
}

static void zfcp_fsf_status_read_port_closed(struct zfcp_fsf_req *req)
{
	unsigned long flags;
	struct fsf_status_read_buffer *sr_buf = req->data;
	struct zfcp_adapter *adapter = req->adapter;
	struct zfcp_port *port;
	int d_id = ntoh24(sr_buf->d_id);

	read_lock_irqsave(&adapter->port_list_lock, flags);
	list_for_each_entry(port, &adapter->port_list, list)
		if (port->d_id == d_id) {
			zfcp_erp_port_reopen(port, 0, "fssrpc1");
			break;
		}
	read_unlock_irqrestore(&adapter->port_list_lock, flags);
}

void zfcp_fsf_fc_host_link_down(struct zfcp_adapter *adapter)
{
	struct Scsi_Host *shost = adapter->scsi_host;

	adapter->hydra_version = 0;
	adapter->peer_wwpn = 0;
	adapter->peer_wwnn = 0;
	adapter->peer_d_id = 0;

	/* if there is no shost yet, we have nothing to zero-out */
	if (shost == NULL)
		return;

	fc_host_port_id(shost) = 0;
	fc_host_fabric_name(shost) = 0;
	fc_host_speed(shost) = FC_PORTSPEED_UNKNOWN;
	fc_host_port_type(shost) = FC_PORTTYPE_UNKNOWN;
	snprintf(fc_host_model(shost), FC_SYMBOLIC_NAME_SIZE, "0x%04x", 0);
	memset(fc_host_active_fc4s(shost), 0, FC_FC4_LIST_SIZE);
}

static void zfcp_fsf_link_down_info_eval(struct zfcp_fsf_req *req,
					 struct fsf_link_down_info *link_down)
{
	struct zfcp_adapter *adapter = req->adapter;

	if (atomic_read(&adapter->status) & ZFCP_STATUS_ADAPTER_LINK_UNPLUGGED)
		return;

	atomic_or(ZFCP_STATUS_ADAPTER_LINK_UNPLUGGED, &adapter->status);

	zfcp_scsi_schedule_rports_block(adapter);

	zfcp_fsf_fc_host_link_down(adapter);

	if (!link_down)
		goto out;

	switch (link_down->error_code) {
	case FSF_PSQ_LINK_NO_LIGHT:
		dev_warn(&req->adapter->ccw_device->dev,
			 "There is no light signal from the local "
			 "fibre channel cable\n");
		break;
	case FSF_PSQ_LINK_WRAP_PLUG:
		dev_warn(&req->adapter->ccw_device->dev,
			 "There is a wrap plug instead of a fibre "
			 "channel cable\n");
		break;
	case FSF_PSQ_LINK_NO_FCP:
		dev_warn(&req->adapter->ccw_device->dev,
			 "The adjacent fibre channel node does not "
			 "support FCP\n");
		break;
	case FSF_PSQ_LINK_FIRMWARE_UPDATE:
		dev_warn(&req->adapter->ccw_device->dev,
			 "The FCP device is suspended because of a "
			 "firmware update\n");
		break;
	case FSF_PSQ_LINK_INVALID_WWPN:
		dev_warn(&req->adapter->ccw_device->dev,
			 "The FCP device detected a WWPN that is "
			 "duplicate or not valid\n");
		break;
	case FSF_PSQ_LINK_NO_NPIV_SUPPORT:
		dev_warn(&req->adapter->ccw_device->dev,
			 "The fibre channel fabric does not support NPIV\n");
		break;
	case FSF_PSQ_LINK_NO_FCP_RESOURCES:
		dev_warn(&req->adapter->ccw_device->dev,
			 "The FCP adapter cannot support more NPIV ports\n");
		break;
	case FSF_PSQ_LINK_NO_FABRIC_RESOURCES:
		dev_warn(&req->adapter->ccw_device->dev,
			 "The adjacent switch cannot support "
			 "more NPIV ports\n");
		break;
	case FSF_PSQ_LINK_FABRIC_LOGIN_UNABLE:
		dev_warn(&req->adapter->ccw_device->dev,
			 "The FCP adapter could not log in to the "
			 "fibre channel fabric\n");
		break;
	case FSF_PSQ_LINK_WWPN_ASSIGNMENT_CORRUPTED:
		dev_warn(&req->adapter->ccw_device->dev,
			 "The WWPN assignment file on the FCP adapter "
			 "has been damaged\n");
		break;
	case FSF_PSQ_LINK_MODE_TABLE_CURRUPTED:
		dev_warn(&req->adapter->ccw_device->dev,
			 "The mode table on the FCP adapter "
			 "has been damaged\n");
		break;
	case FSF_PSQ_LINK_NO_WWPN_ASSIGNMENT:
		dev_warn(&req->adapter->ccw_device->dev,
			 "All NPIV ports on the FCP adapter have "
			 "been assigned\n");
		break;
	default:
		dev_warn(&req->adapter->ccw_device->dev,
			 "The link between the FCP adapter and "
			 "the FC fabric is down\n");
	}
out:
	zfcp_erp_set_adapter_status(adapter, ZFCP_STATUS_COMMON_ERP_FAILED);
}

static void zfcp_fsf_status_read_link_down(struct zfcp_fsf_req *req)
{
	struct fsf_status_read_buffer *sr_buf = req->data;
	struct fsf_link_down_info *ldi =
		(struct fsf_link_down_info *) &sr_buf->payload;

	switch (sr_buf->status_subtype) {
	case FSF_STATUS_READ_SUB_NO_PHYSICAL_LINK:
	case FSF_STATUS_READ_SUB_FDISC_FAILED:
		zfcp_fsf_link_down_info_eval(req, ldi);
		break;
	case FSF_STATUS_READ_SUB_FIRMWARE_UPDATE:
		zfcp_fsf_link_down_info_eval(req, NULL);
	}
}

static void
zfcp_fsf_status_read_version_change(struct zfcp_adapter *adapter,
				    struct fsf_status_read_buffer *sr_buf)
{
	if (sr_buf->status_subtype == FSF_STATUS_READ_SUB_LIC_CHANGE) {
		u32 version = sr_buf->payload.version_change.current_version;

		WRITE_ONCE(adapter->fsf_lic_version, version);
		snprintf(fc_host_firmware_version(adapter->scsi_host),
			 FC_VERSION_STRING_SIZE, "%#08x", version);
	}
}

static void zfcp_fsf_status_read_handler(struct zfcp_fsf_req *req)
{
	struct zfcp_adapter *adapter = req->adapter;
	struct fsf_status_read_buffer *sr_buf = req->data;

	if (req->status & ZFCP_STATUS_FSFREQ_DISMISSED) {
		zfcp_dbf_hba_fsf_uss("fssrh_1", req);
		mempool_free(virt_to_page(sr_buf), adapter->pool.sr_data);
		zfcp_fsf_req_free(req);
		return;
	}

	zfcp_dbf_hba_fsf_uss("fssrh_4", req);

	switch (sr_buf->status_type) {
	case FSF_STATUS_READ_PORT_CLOSED:
		zfcp_fsf_status_read_port_closed(req);
		break;
	case FSF_STATUS_READ_INCOMING_ELS:
		zfcp_fc_incoming_els(req);
		break;
	case FSF_STATUS_READ_SENSE_DATA_AVAIL:
		break;
	case FSF_STATUS_READ_BIT_ERROR_THRESHOLD:
		zfcp_dbf_hba_bit_err("fssrh_3", req);
		if (ber_stop) {
			dev_warn(&adapter->ccw_device->dev,
				 "All paths over this FCP device are disused because of excessive bit errors\n");
			zfcp_erp_adapter_shutdown(adapter, 0, "fssrh_b");
		} else {
			dev_warn(&adapter->ccw_device->dev,
				 "The error threshold for checksum statistics has been exceeded\n");
		}
		break;
	case FSF_STATUS_READ_LINK_DOWN:
		zfcp_fsf_status_read_link_down(req);
		zfcp_fc_enqueue_event(adapter, FCH_EVT_LINKDOWN, 0);
		break;
	case FSF_STATUS_READ_LINK_UP:
		dev_info(&adapter->ccw_device->dev,
			 "The local link has been restored\n");
		/* All ports should be marked as ready to run again */
		zfcp_erp_set_adapter_status(adapter,
					    ZFCP_STATUS_COMMON_RUNNING);
		zfcp_erp_adapter_reopen(adapter,
					ZFCP_STATUS_ADAPTER_LINK_UNPLUGGED |
					ZFCP_STATUS_COMMON_ERP_FAILED,
					"fssrh_2");
		zfcp_fc_enqueue_event(adapter, FCH_EVT_LINKUP, 0);

		break;
	case FSF_STATUS_READ_NOTIFICATION_LOST:
		if (sr_buf->status_subtype & FSF_STATUS_READ_SUB_INCOMING_ELS)
			zfcp_fc_conditional_port_scan(adapter);
		if (sr_buf->status_subtype & FSF_STATUS_READ_SUB_VERSION_CHANGE)
			queue_work(adapter->work_queue,
				   &adapter->version_change_lost_work);
		break;
	case FSF_STATUS_READ_FEATURE_UPDATE_ALERT:
		adapter->adapter_features = sr_buf->payload.word[0];
		break;
	case FSF_STATUS_READ_VERSION_CHANGE:
		zfcp_fsf_status_read_version_change(adapter, sr_buf);
		break;
	}

	mempool_free(virt_to_page(sr_buf), adapter->pool.sr_data);
	zfcp_fsf_req_free(req);

	atomic_inc(&adapter->stat_miss);
	queue_work(adapter->work_queue, &adapter->stat_work);
}

static void zfcp_fsf_fsfstatus_qual_eval(struct zfcp_fsf_req *req)
{
	switch (req->qtcb->header.fsf_status_qual.word[0]) {
	case FSF_SQ_FCP_RSP_AVAILABLE:
	case FSF_SQ_INVOKE_LINK_TEST_PROCEDURE:
	case FSF_SQ_NO_RETRY_POSSIBLE:
	case FSF_SQ_ULP_DEPENDENT_ERP_REQUIRED:
		return;
	case FSF_SQ_COMMAND_ABORTED:
		break;
	case FSF_SQ_NO_RECOM:
		dev_err(&req->adapter->ccw_device->dev,
			"The FCP adapter reported a problem "
			"that cannot be recovered\n");
		zfcp_qdio_siosl(req->adapter);
		zfcp_erp_adapter_shutdown(req->adapter, 0, "fsfsqe1");
		break;
	}
	/* all non-return stats set FSFREQ_ERROR*/
	req->status |= ZFCP_STATUS_FSFREQ_ERROR;
}

static void zfcp_fsf_fsfstatus_eval(struct zfcp_fsf_req *req)
{
	if (unlikely(req->status & ZFCP_STATUS_FSFREQ_ERROR))
		return;

	switch (req->qtcb->header.fsf_status) {
	case FSF_UNKNOWN_COMMAND:
		dev_err(&req->adapter->ccw_device->dev,
			"The FCP adapter does not recognize the command 0x%x\n",
			req->qtcb->header.fsf_command);
		zfcp_erp_adapter_shutdown(req->adapter, 0, "fsfse_1");
		req->status |= ZFCP_STATUS_FSFREQ_ERROR;
		break;
	case FSF_ADAPTER_STATUS_AVAILABLE:
		zfcp_fsf_fsfstatus_qual_eval(req);
		break;
	}
}

static void zfcp_fsf_protstatus_eval(struct zfcp_fsf_req *req)
{
	struct zfcp_adapter *adapter = req->adapter;
	struct fsf_qtcb *qtcb = req->qtcb;
	union fsf_prot_status_qual *psq = &qtcb->prefix.prot_status_qual;

	zfcp_dbf_hba_fsf_response(req);

	if (req->status & ZFCP_STATUS_FSFREQ_DISMISSED) {
		req->status |= ZFCP_STATUS_FSFREQ_ERROR;
		return;
	}

	switch (qtcb->prefix.prot_status) {
	case FSF_PROT_GOOD:
	case FSF_PROT_FSF_STATUS_PRESENTED:
		return;
	case FSF_PROT_QTCB_VERSION_ERROR:
		dev_err(&adapter->ccw_device->dev,
			"QTCB version 0x%x not supported by FCP adapter "
			"(0x%x to 0x%x)\n", FSF_QTCB_CURRENT_VERSION,
			psq->word[0], psq->word[1]);
		zfcp_erp_adapter_shutdown(adapter, 0, "fspse_1");
		break;
	case FSF_PROT_ERROR_STATE:
	case FSF_PROT_SEQ_NUMB_ERROR:
		zfcp_erp_adapter_reopen(adapter, 0, "fspse_2");
		req->status |= ZFCP_STATUS_FSFREQ_ERROR;
		break;
	case FSF_PROT_UNSUPP_QTCB_TYPE:
		dev_err(&adapter->ccw_device->dev,
			"The QTCB type is not supported by the FCP adapter\n");
		zfcp_erp_adapter_shutdown(adapter, 0, "fspse_3");
		break;
	case FSF_PROT_HOST_CONNECTION_INITIALIZING:
		atomic_or(ZFCP_STATUS_ADAPTER_HOST_CON_INIT,
				&adapter->status);
		break;
	case FSF_PROT_DUPLICATE_REQUEST_ID:
		dev_err(&adapter->ccw_device->dev,
			"0x%Lx is an ambiguous request identifier\n",
			(unsigned long long)qtcb->bottom.support.req_handle);
		zfcp_erp_adapter_shutdown(adapter, 0, "fspse_4");
		break;
	case FSF_PROT_LINK_DOWN:
		zfcp_fsf_link_down_info_eval(req, &psq->link_down_info);
		/* go through reopen to flush pending requests */
		zfcp_erp_adapter_reopen(adapter, 0, "fspse_6");
		break;
	case FSF_PROT_REEST_QUEUE:
		/* All ports should be marked as ready to run again */
		zfcp_erp_set_adapter_status(adapter,
					    ZFCP_STATUS_COMMON_RUNNING);
		zfcp_erp_adapter_reopen(adapter,
					ZFCP_STATUS_ADAPTER_LINK_UNPLUGGED |
					ZFCP_STATUS_COMMON_ERP_FAILED,
					"fspse_8");
		break;
	default:
		dev_err(&adapter->ccw_device->dev,
			"0x%x is not a valid transfer protocol status\n",
			qtcb->prefix.prot_status);
		zfcp_qdio_siosl(adapter);
		zfcp_erp_adapter_shutdown(adapter, 0, "fspse_9");
	}
	req->status |= ZFCP_STATUS_FSFREQ_ERROR;
}

/**
 * zfcp_fsf_req_complete - process completion of a FSF request
 * @req: The FSF request that has been completed.
 *
 * When a request has been completed either from the FCP adapter,
 * or it has been dismissed due to a queue shutdown, this function
 * is called to process the completion status and trigger further
 * events related to the FSF request.
 * Caller must ensure that the request has been removed from
 * adapter->req_list, to protect against concurrent modification
 * by zfcp_erp_strategy_check_fsfreq().
 */
static void zfcp_fsf_req_complete(struct zfcp_fsf_req *req)
{
	struct zfcp_erp_action *erp_action;

	if (unlikely(zfcp_fsf_req_is_status_read_buffer(req))) {
		zfcp_fsf_status_read_handler(req);
		return;
	}

	del_timer_sync(&req->timer);
	zfcp_fsf_protstatus_eval(req);
	zfcp_fsf_fsfstatus_eval(req);
	req->handler(req);

	erp_action = req->erp_action;
	if (erp_action)
		zfcp_erp_notify(erp_action, 0);

	if (likely(req->status & ZFCP_STATUS_FSFREQ_CLEANUP))
		zfcp_fsf_req_free(req);
	else
		complete(&req->completion);
}

/**
 * zfcp_fsf_req_dismiss_all - dismiss all fsf requests
 * @adapter: pointer to struct zfcp_adapter
 *
 * Never ever call this without shutting down the adapter first.
 * Otherwise the adapter would continue using and corrupting s390 storage.
 * Included BUG_ON() call to ensure this is done.
 * ERP is supposed to be the only user of this function.
 */
void zfcp_fsf_req_dismiss_all(struct zfcp_adapter *adapter)
{
	struct zfcp_fsf_req *req, *tmp;
	LIST_HEAD(remove_queue);

	BUG_ON(atomic_read(&adapter->status) & ZFCP_STATUS_ADAPTER_QDIOUP);
	zfcp_reqlist_move(adapter->req_list, &remove_queue);

	list_for_each_entry_safe(req, tmp, &remove_queue, list) {
		list_del(&req->list);
		req->status |= ZFCP_STATUS_FSFREQ_DISMISSED;
		zfcp_fsf_req_complete(req);
	}
}

#define ZFCP_FSF_PORTSPEED_1GBIT	(1 <<  0)
#define ZFCP_FSF_PORTSPEED_2GBIT	(1 <<  1)
#define ZFCP_FSF_PORTSPEED_4GBIT	(1 <<  2)
#define ZFCP_FSF_PORTSPEED_10GBIT	(1 <<  3)
#define ZFCP_FSF_PORTSPEED_8GBIT	(1 <<  4)
#define ZFCP_FSF_PORTSPEED_16GBIT	(1 <<  5)
#define ZFCP_FSF_PORTSPEED_32GBIT	(1 <<  6)
#define ZFCP_FSF_PORTSPEED_64GBIT	(1 <<  7)
#define ZFCP_FSF_PORTSPEED_128GBIT	(1 <<  8)
#define ZFCP_FSF_PORTSPEED_NOT_NEGOTIATED (1 << 15)

u32 zfcp_fsf_convert_portspeed(u32 fsf_speed)
{
	u32 fdmi_speed = 0;
	if (fsf_speed & ZFCP_FSF_PORTSPEED_1GBIT)
		fdmi_speed |= FC_PORTSPEED_1GBIT;
	if (fsf_speed & ZFCP_FSF_PORTSPEED_2GBIT)
		fdmi_speed |= FC_PORTSPEED_2GBIT;
	if (fsf_speed & ZFCP_FSF_PORTSPEED_4GBIT)
		fdmi_speed |= FC_PORTSPEED_4GBIT;
	if (fsf_speed & ZFCP_FSF_PORTSPEED_10GBIT)
		fdmi_speed |= FC_PORTSPEED_10GBIT;
	if (fsf_speed & ZFCP_FSF_PORTSPEED_8GBIT)
		fdmi_speed |= FC_PORTSPEED_8GBIT;
	if (fsf_speed & ZFCP_FSF_PORTSPEED_16GBIT)
		fdmi_speed |= FC_PORTSPEED_16GBIT;
	if (fsf_speed & ZFCP_FSF_PORTSPEED_32GBIT)
		fdmi_speed |= FC_PORTSPEED_32GBIT;
	if (fsf_speed & ZFCP_FSF_PORTSPEED_64GBIT)
		fdmi_speed |= FC_PORTSPEED_64GBIT;
	if (fsf_speed & ZFCP_FSF_PORTSPEED_128GBIT)
		fdmi_speed |= FC_PORTSPEED_128GBIT;
	if (fsf_speed & ZFCP_FSF_PORTSPEED_NOT_NEGOTIATED)
		fdmi_speed |= FC_PORTSPEED_NOT_NEGOTIATED;
	return fdmi_speed;
}

static int zfcp_fsf_exchange_config_evaluate(struct zfcp_fsf_req *req)
{
	struct fsf_qtcb_bottom_config *bottom = &req->qtcb->bottom.config;
	struct zfcp_adapter *adapter = req->adapter;
	struct fc_els_flogi *plogi;

	/* adjust pointers for missing command code */
	plogi = (struct fc_els_flogi *) ((u8 *)&bottom->plogi_payload
					- sizeof(u32));

	if (req->data)
		memcpy(req->data, bottom, sizeof(*bottom));

	adapter->timer_ticks = bottom->timer_interval & ZFCP_FSF_TIMER_INT_MASK;
	adapter->stat_read_buf_num = max(bottom->status_read_buf_num,
					 (u16)FSF_STATUS_READS_RECOM);

	/* no error return above here, otherwise must fix call chains */
	/* do not evaluate invalid fields */
	if (req->qtcb->header.fsf_status == FSF_EXCHANGE_CONFIG_DATA_INCOMPLETE)
		return 0;

	adapter->hydra_version = bottom->adapter_type;

	switch (bottom->fc_topology) {
	case FSF_TOPO_P2P:
		adapter->peer_d_id = ntoh24(bottom->peer_d_id);
		adapter->peer_wwpn = be64_to_cpu(plogi->fl_wwpn);
		adapter->peer_wwnn = be64_to_cpu(plogi->fl_wwnn);
		break;
	case FSF_TOPO_FABRIC:
		break;
	case FSF_TOPO_AL:
	default:
		dev_err(&adapter->ccw_device->dev,
			"Unknown or unsupported arbitrated loop "
			"fibre channel topology detected\n");
		zfcp_erp_adapter_shutdown(adapter, 0, "fsece_1");
		return -EIO;
	}

	return 0;
}

static void zfcp_fsf_exchange_config_data_handler(struct zfcp_fsf_req *req)
{
	struct zfcp_adapter *adapter = req->adapter;
	struct zfcp_diag_header *const diag_hdr =
		&adapter->diagnostics->config_data.header;
	struct fsf_qtcb *qtcb = req->qtcb;
	struct fsf_qtcb_bottom_config *bottom = &qtcb->bottom.config;

	if (req->status & ZFCP_STATUS_FSFREQ_ERROR)
		return;

	adapter->fsf_lic_version = bottom->lic_version;
	adapter->adapter_features = bottom->adapter_features;
	adapter->connection_features = bottom->connection_features;
	adapter->peer_wwpn = 0;
	adapter->peer_wwnn = 0;
	adapter->peer_d_id = 0;

	switch (qtcb->header.fsf_status) {
	case FSF_GOOD:
		/*
		 * usually we wait with an update till the cache is too old,
		 * but because we have the data available, update it anyway
		 */
		zfcp_diag_update_xdata(diag_hdr, bottom, false);

		zfcp_scsi_shost_update_config_data(adapter, bottom, false);
		if (zfcp_fsf_exchange_config_evaluate(req))
			return;

		if (bottom->max_qtcb_size < sizeof(struct fsf_qtcb)) {
			dev_err(&adapter->ccw_device->dev,
				"FCP adapter maximum QTCB size (%d bytes) "
				"is too small\n",
				bottom->max_qtcb_size);
			zfcp_erp_adapter_shutdown(adapter, 0, "fsecdh1");
			return;
		}
		atomic_or(ZFCP_STATUS_ADAPTER_XCONFIG_OK,
				&adapter->status);
		break;
	case FSF_EXCHANGE_CONFIG_DATA_INCOMPLETE:
		zfcp_diag_update_xdata(diag_hdr, bottom, true);
		req->status |= ZFCP_STATUS_FSFREQ_XDATAINCOMPLETE;

		/* avoids adapter shutdown to be able to recognize
		 * events such as LINK UP */
		atomic_or(ZFCP_STATUS_ADAPTER_XCONFIG_OK,
				&adapter->status);
		zfcp_fsf_link_down_info_eval(req,
			&qtcb->header.fsf_status_qual.link_down_info);

		zfcp_scsi_shost_update_config_data(adapter, bottom, true);
		if (zfcp_fsf_exchange_config_evaluate(req))
			return;
		break;
	default:
		zfcp_erp_adapter_shutdown(adapter, 0, "fsecdh3");
		return;
	}

	if (adapter->adapter_features & FSF_FEATURE_HBAAPI_MANAGEMENT)
		adapter->hardware_version = bottom->hardware_version;

	if (FSF_QTCB_CURRENT_VERSION < bottom->low_qtcb_version) {
		dev_err(&adapter->ccw_device->dev,
			"The FCP adapter only supports newer "
			"control block versions\n");
		zfcp_erp_adapter_shutdown(adapter, 0, "fsecdh4");
		return;
	}
	if (FSF_QTCB_CURRENT_VERSION > bottom->high_qtcb_version) {
		dev_err(&adapter->ccw_device->dev,
			"The FCP adapter only supports older "
			"control block versions\n");
		zfcp_erp_adapter_shutdown(adapter, 0, "fsecdh5");
	}
}

/*
 * Mapping of FC Endpoint Security flag masks to mnemonics
 *
 * NOTE: Update macro ZFCP_FSF_MAX_FC_SECURITY_MNEMONIC_LENGTH when making any
 *       changes.
 */
static const struct {
	u32	mask;
	char	*name;
} zfcp_fsf_fc_security_mnemonics[] = {
	{ FSF_FC_SECURITY_AUTH,		"Authentication" },
	{ FSF_FC_SECURITY_ENC_FCSP2 |
	  FSF_FC_SECURITY_ENC_ERAS,	"Encryption" },
};

/* maximum strlen(zfcp_fsf_fc_security_mnemonics[...].name) + 1 */
#define ZFCP_FSF_MAX_FC_SECURITY_MNEMONIC_LENGTH 15

/**
 * zfcp_fsf_scnprint_fc_security() - translate FC Endpoint Security flags into
 *                                   mnemonics and place in a buffer
 * @buf        : the buffer to place the translated FC Endpoint Security flag(s)
 *               into
 * @size       : the size of the buffer, including the trailing null space
 * @fc_security: one or more FC Endpoint Security flags, or zero
 * @fmt        : specifies whether a list or a single item is to be put into the
 *               buffer
 *
 * The Fibre Channel (FC) Endpoint Security flags are translated into mnemonics.
 * If the FC Endpoint Security flags are zero "none" is placed into the buffer.
 *
 * With ZFCP_FSF_PRINT_FMT_LIST the mnemonics are placed as a list separated by
 * a comma followed by a space into the buffer. If one or more FC Endpoint
 * Security flags cannot be translated into a mnemonic, as they are undefined
 * in zfcp_fsf_fc_security_mnemonics, their bitwise ORed value in hexadecimal
 * representation is placed into the buffer.
 *
 * With ZFCP_FSF_PRINT_FMT_SINGLEITEM only one single mnemonic is placed into
 * the buffer. If the FC Endpoint Security flag cannot be translated, as it is
 * undefined in zfcp_fsf_fc_security_mnemonics, its value in hexadecimal
 * representation is placed into the buffer. If more than one FC Endpoint
 * Security flag was specified, their value in hexadecimal representation is
 * placed into the buffer. The macro ZFCP_FSF_MAX_FC_SECURITY_MNEMONIC_LENGTH
 * can be used to define a buffer that is large enough to hold one mnemonic.
 *
 * Return: The number of characters written into buf not including the trailing
 *         '\0'. If size is == 0 the function returns 0.
 */
ssize_t zfcp_fsf_scnprint_fc_security(char *buf, size_t size, u32 fc_security,
				      enum zfcp_fsf_print_fmt fmt)
{
	const char *prefix = "";
	ssize_t len = 0;
	int i;

	if (fc_security == 0)
		return scnprintf(buf, size, "none");
	if (fmt == ZFCP_FSF_PRINT_FMT_SINGLEITEM && hweight32(fc_security) != 1)
		return scnprintf(buf, size, "0x%08x", fc_security);

	for (i = 0; i < ARRAY_SIZE(zfcp_fsf_fc_security_mnemonics); i++) {
		if (!(fc_security & zfcp_fsf_fc_security_mnemonics[i].mask))
			continue;

		len += scnprintf(buf + len, size - len, "%s%s", prefix,
				 zfcp_fsf_fc_security_mnemonics[i].name);
		prefix = ", ";
		fc_security &= ~zfcp_fsf_fc_security_mnemonics[i].mask;
	}

	if (fc_security != 0)
		len += scnprintf(buf + len, size - len, "%s0x%08x",
				 prefix, fc_security);

	return len;
}

static void zfcp_fsf_dbf_adapter_fc_security(struct zfcp_adapter *adapter,
					     struct zfcp_fsf_req *req)
{
	if (adapter->fc_security_algorithms ==
	    adapter->fc_security_algorithms_old) {
		/* no change, no trace */
		return;
	}

	zfcp_dbf_hba_fsf_fces("fsfcesa", req, ZFCP_DBF_INVALID_WWPN,
			      adapter->fc_security_algorithms_old,
			      adapter->fc_security_algorithms);

	adapter->fc_security_algorithms_old = adapter->fc_security_algorithms;
}

static void zfcp_fsf_exchange_port_evaluate(struct zfcp_fsf_req *req)
{
	struct zfcp_adapter *adapter = req->adapter;
	struct fsf_qtcb_bottom_port *bottom = &req->qtcb->bottom.port;

	if (req->data)
		memcpy(req->data, bottom, sizeof(*bottom));

	if (adapter->adapter_features & FSF_FEATURE_FC_SECURITY)
		adapter->fc_security_algorithms =
			bottom->fc_security_algorithms;
	else
		adapter->fc_security_algorithms = 0;
	zfcp_fsf_dbf_adapter_fc_security(adapter, req);
}

static void zfcp_fsf_exchange_port_data_handler(struct zfcp_fsf_req *req)
{
	struct zfcp_diag_header *const diag_hdr =
		&req->adapter->diagnostics->port_data.header;
	struct fsf_qtcb *qtcb = req->qtcb;
	struct fsf_qtcb_bottom_port *bottom = &qtcb->bottom.port;

	if (req->status & ZFCP_STATUS_FSFREQ_ERROR)
		return;

	switch (qtcb->header.fsf_status) {
	case FSF_GOOD:
		/*
		 * usually we wait with an update till the cache is too old,
		 * but because we have the data available, update it anyway
		 */
		zfcp_diag_update_xdata(diag_hdr, bottom, false);

		zfcp_scsi_shost_update_port_data(req->adapter, bottom);
		zfcp_fsf_exchange_port_evaluate(req);
		break;
	case FSF_EXCHANGE_CONFIG_DATA_INCOMPLETE:
		zfcp_diag_update_xdata(diag_hdr, bottom, true);
		req->status |= ZFCP_STATUS_FSFREQ_XDATAINCOMPLETE;

		zfcp_fsf_link_down_info_eval(req,
			&qtcb->header.fsf_status_qual.link_down_info);

		zfcp_scsi_shost_update_port_data(req->adapter, bottom);
		zfcp_fsf_exchange_port_evaluate(req);
		break;
	}
}

static struct zfcp_fsf_req *zfcp_fsf_alloc(mempool_t *pool)
{
	struct zfcp_fsf_req *req;

	if (likely(pool))
		req = mempool_alloc(pool, GFP_ATOMIC);
	else
		req = kmalloc(sizeof(*req), GFP_ATOMIC);

	if (unlikely(!req))
		return NULL;

	memset(req, 0, sizeof(*req));
	req->pool = pool;
	return req;
}

static struct fsf_qtcb *zfcp_fsf_qtcb_alloc(mempool_t *pool)
{
	struct fsf_qtcb *qtcb;

	if (likely(pool))
		qtcb = mempool_alloc(pool, GFP_ATOMIC);
	else
		qtcb = kmem_cache_alloc(zfcp_fsf_qtcb_cache, GFP_ATOMIC);

	if (unlikely(!qtcb))
		return NULL;

	memset(qtcb, 0, sizeof(*qtcb));
	return qtcb;
}

static struct zfcp_fsf_req *zfcp_fsf_req_create(struct zfcp_qdio *qdio,
						u32 fsf_cmd, u8 sbtype,
						mempool_t *pool)
{
	struct zfcp_adapter *adapter = qdio->adapter;
	struct zfcp_fsf_req *req = zfcp_fsf_alloc(pool);

	if (unlikely(!req))
		return ERR_PTR(-ENOMEM);

	if (adapter->req_no == 0)
		adapter->req_no++;

	timer_setup(&req->timer, NULL, 0);
	init_completion(&req->completion);

	req->adapter = adapter;
	req->req_id = adapter->req_no;

	if (likely(fsf_cmd != FSF_QTCB_UNSOLICITED_STATUS)) {
		if (likely(pool))
			req->qtcb = zfcp_fsf_qtcb_alloc(
				adapter->pool.qtcb_pool);
		else
			req->qtcb = zfcp_fsf_qtcb_alloc(NULL);

		if (unlikely(!req->qtcb)) {
			zfcp_fsf_req_free(req);
			return ERR_PTR(-ENOMEM);
		}

		req->qtcb->prefix.req_seq_no = adapter->fsf_req_seq_no;
		req->qtcb->prefix.req_id = req->req_id;
		req->qtcb->prefix.ulp_info = 26;
		req->qtcb->prefix.qtcb_type = fsf_qtcb_type[fsf_cmd];
		req->qtcb->prefix.qtcb_version = FSF_QTCB_CURRENT_VERSION;
		req->qtcb->header.req_handle = req->req_id;
		req->qtcb->header.fsf_command = fsf_cmd;
	}

	zfcp_qdio_req_init(adapter->qdio, &req->qdio_req, req->req_id, sbtype,
			   req->qtcb, sizeof(struct fsf_qtcb));

	return req;
}

static int zfcp_fsf_req_send(struct zfcp_fsf_req *req)
{
	const bool is_srb = zfcp_fsf_req_is_status_read_buffer(req);
	struct zfcp_adapter *adapter = req->adapter;
	struct zfcp_qdio *qdio = adapter->qdio;
	int req_id = req->req_id;

	zfcp_reqlist_add(adapter->req_list, req);

	req->qdio_req.qdio_outb_usage = atomic_read(&qdio->req_q_free);
	req->issued = get_tod_clock();
	if (zfcp_qdio_send(qdio, &req->qdio_req)) {
		del_timer_sync(&req->timer);
		/* lookup request again, list might have changed */
		zfcp_reqlist_find_rm(adapter->req_list, req_id);
		zfcp_erp_adapter_reopen(adapter, 0, "fsrs__1");
		return -EIO;
	}

	/*
	 * NOTE: DO NOT TOUCH ASYNC req PAST THIS POINT.
	 *	 ONLY TOUCH SYNC req AGAIN ON req->completion.
	 *
	 * The request might complete and be freed concurrently at any point
	 * now. This is not protected by the QDIO-lock (req_q_lock). So any
	 * uncontrolled access after this might result in an use-after-free bug.
	 * Only if the request doesn't have ZFCP_STATUS_FSFREQ_CLEANUP set, and
	 * when it is completed via req->completion, is it safe to use req
	 * again.
	 */

	/* Don't increase for unsolicited status */
	if (!is_srb)
		adapter->fsf_req_seq_no++;
	adapter->req_no++;

	return 0;
}

/**
 * zfcp_fsf_status_read - send status read request
 * @qdio: pointer to struct zfcp_qdio
 * Returns: 0 on success, ERROR otherwise
 */
int zfcp_fsf_status_read(struct zfcp_qdio *qdio)
{
	struct zfcp_adapter *adapter = qdio->adapter;
	struct zfcp_fsf_req *req;
	struct fsf_status_read_buffer *sr_buf;
	struct page *page;
	int retval = -EIO;

	spin_lock_irq(&qdio->req_q_lock);
	if (zfcp_qdio_sbal_get(qdio))
		goto out;

	req = zfcp_fsf_req_create(qdio, FSF_QTCB_UNSOLICITED_STATUS,
				  SBAL_SFLAGS0_TYPE_STATUS,
				  adapter->pool.status_read_req);
	if (IS_ERR(req)) {
		retval = PTR_ERR(req);
		goto out;
	}

	page = mempool_alloc(adapter->pool.sr_data, GFP_ATOMIC);
	if (!page) {
		retval = -ENOMEM;
		goto failed_buf;
	}
	sr_buf = page_address(page);
	memset(sr_buf, 0, sizeof(*sr_buf));
	req->data = sr_buf;

	zfcp_qdio_fill_next(qdio, &req->qdio_req, sr_buf, sizeof(*sr_buf));
	zfcp_qdio_set_sbale_last(qdio, &req->qdio_req);

	retval = zfcp_fsf_req_send(req);
	if (retval)
		goto failed_req_send;
	/* NOTE: DO NOT TOUCH req PAST THIS POINT! */

	goto out;

failed_req_send:
	req->data = NULL;
	mempool_free(virt_to_page(sr_buf), adapter->pool.sr_data);
failed_buf:
	zfcp_dbf_hba_fsf_uss("fssr__1", req);
	zfcp_fsf_req_free(req);
out:
	spin_unlock_irq(&qdio->req_q_lock);
	return retval;
}

static void zfcp_fsf_abort_fcp_command_handler(struct zfcp_fsf_req *req)
{
	struct scsi_device *sdev = req->data;
	struct zfcp_scsi_dev *zfcp_sdev;
	union fsf_status_qual *fsq = &req->qtcb->header.fsf_status_qual;

	if (req->status & ZFCP_STATUS_FSFREQ_ERROR)
		return;

	zfcp_sdev = sdev_to_zfcp(sdev);

	switch (req->qtcb->header.fsf_status) {
	case FSF_PORT_HANDLE_NOT_VALID:
		if (fsq->word[0] == fsq->word[1]) {
			zfcp_erp_adapter_reopen(zfcp_sdev->port->adapter, 0,
						"fsafch1");
			req->status |= ZFCP_STATUS_FSFREQ_ERROR;
		}
		break;
	case FSF_LUN_HANDLE_NOT_VALID:
		if (fsq->word[0] == fsq->word[1]) {
			zfcp_erp_port_reopen(zfcp_sdev->port, 0, "fsafch2");
			req->status |= ZFCP_STATUS_FSFREQ_ERROR;
		}
		break;
	case FSF_FCP_COMMAND_DOES_NOT_EXIST:
		req->status |= ZFCP_STATUS_FSFREQ_ABORTNOTNEEDED;
		break;
	case FSF_PORT_BOXED:
		zfcp_erp_set_port_status(zfcp_sdev->port,
					 ZFCP_STATUS_COMMON_ACCESS_BOXED);
		zfcp_erp_port_reopen(zfcp_sdev->port,
				     ZFCP_STATUS_COMMON_ERP_FAILED, "fsafch3");
		req->status |= ZFCP_STATUS_FSFREQ_ERROR;
		break;
	case FSF_LUN_BOXED:
		zfcp_erp_set_lun_status(sdev, ZFCP_STATUS_COMMON_ACCESS_BOXED);
		zfcp_erp_lun_reopen(sdev, ZFCP_STATUS_COMMON_ERP_FAILED,
				    "fsafch4");
		req->status |= ZFCP_STATUS_FSFREQ_ERROR;
                break;
	case FSF_ADAPTER_STATUS_AVAILABLE:
		switch (fsq->word[0]) {
		case FSF_SQ_INVOKE_LINK_TEST_PROCEDURE:
			zfcp_fc_test_link(zfcp_sdev->port);
			fallthrough;
		case FSF_SQ_ULP_DEPENDENT_ERP_REQUIRED:
			req->status |= ZFCP_STATUS_FSFREQ_ERROR;
			break;
		}
		break;
	case FSF_GOOD:
		req->status |= ZFCP_STATUS_FSFREQ_ABORTSUCCEEDED;
		break;
	}
}

/**
 * zfcp_fsf_abort_fcp_cmnd - abort running SCSI command
 * @scmnd: The SCSI command to abort
 * Returns: pointer to struct zfcp_fsf_req
 */

struct zfcp_fsf_req *zfcp_fsf_abort_fcp_cmnd(struct scsi_cmnd *scmnd)
{
	struct zfcp_fsf_req *req = NULL;
	struct scsi_device *sdev = scmnd->device;
	struct zfcp_scsi_dev *zfcp_sdev = sdev_to_zfcp(sdev);
	struct zfcp_qdio *qdio = zfcp_sdev->port->adapter->qdio;
	unsigned long old_req_id = (unsigned long) scmnd->host_scribble;

	spin_lock_irq(&qdio->req_q_lock);
	if (zfcp_qdio_sbal_get(qdio))
		goto out;
	req = zfcp_fsf_req_create(qdio, FSF_QTCB_ABORT_FCP_CMND,
				  SBAL_SFLAGS0_TYPE_READ,
				  qdio->adapter->pool.scsi_abort);
	if (IS_ERR(req)) {
		req = NULL;
		goto out;
	}

	if (unlikely(!(atomic_read(&zfcp_sdev->status) &
		       ZFCP_STATUS_COMMON_UNBLOCKED)))
		goto out_error_free;

	zfcp_qdio_set_sbale_last(qdio, &req->qdio_req);

	req->data = sdev;
	req->handler = zfcp_fsf_abort_fcp_command_handler;
	req->qtcb->header.lun_handle = zfcp_sdev->lun_handle;
	req->qtcb->header.port_handle = zfcp_sdev->port->handle;
	req->qtcb->bottom.support.req_handle = (u64) old_req_id;

	zfcp_fsf_start_timer(req, ZFCP_FSF_SCSI_ER_TIMEOUT);
	if (!zfcp_fsf_req_send(req)) {
		/* NOTE: DO NOT TOUCH req, UNTIL IT COMPLETES! */
		goto out;
	}

out_error_free:
	zfcp_fsf_req_free(req);
	req = NULL;
out:
	spin_unlock_irq(&qdio->req_q_lock);
	return req;
}

static void zfcp_fsf_send_ct_handler(struct zfcp_fsf_req *req)
{
	struct zfcp_adapter *adapter = req->adapter;
	struct zfcp_fsf_ct_els *ct = req->data;
	struct fsf_qtcb_header *header = &req->qtcb->header;

	ct->status = -EINVAL;

	if (req->status & ZFCP_STATUS_FSFREQ_ERROR)
		goto skip_fsfstatus;

	switch (header->fsf_status) {
        case FSF_GOOD:
		ct->status = 0;
		zfcp_dbf_san_res("fsscth2", req);
		break;
        case FSF_SERVICE_CLASS_NOT_SUPPORTED:
		zfcp_fsf_class_not_supp(req);
		break;
        case FSF_ADAPTER_STATUS_AVAILABLE:
                switch (header->fsf_status_qual.word[0]){
                case FSF_SQ_INVOKE_LINK_TEST_PROCEDURE:
                case FSF_SQ_ULP_DEPENDENT_ERP_REQUIRED:
			req->status |= ZFCP_STATUS_FSFREQ_ERROR;
			break;
                }
                break;
        case FSF_PORT_BOXED:
		req->status |= ZFCP_STATUS_FSFREQ_ERROR;
		break;
	case FSF_PORT_HANDLE_NOT_VALID:
		zfcp_erp_adapter_reopen(adapter, 0, "fsscth1");
		fallthrough;
	case FSF_GENERIC_COMMAND_REJECTED:
	case FSF_PAYLOAD_SIZE_MISMATCH:
	case FSF_REQUEST_SIZE_TOO_LARGE:
	case FSF_RESPONSE_SIZE_TOO_LARGE:
	case FSF_SBAL_MISMATCH:
		req->status |= ZFCP_STATUS_FSFREQ_ERROR;
		break;
	}

skip_fsfstatus:
	if (ct->handler)
		ct->handler(ct->handler_data);
}

static void zfcp_fsf_setup_ct_els_unchained(struct zfcp_qdio *qdio,
					    struct zfcp_qdio_req *q_req,
					    struct scatterlist *sg_req,
					    struct scatterlist *sg_resp)
{
	zfcp_qdio_fill_next(qdio, q_req, sg_virt(sg_req), sg_req->length);
	zfcp_qdio_fill_next(qdio, q_req, sg_virt(sg_resp), sg_resp->length);
	zfcp_qdio_set_sbale_last(qdio, q_req);
}

static int zfcp_fsf_setup_ct_els_sbals(struct zfcp_fsf_req *req,
				       struct scatterlist *sg_req,
				       struct scatterlist *sg_resp)
{
	struct zfcp_adapter *adapter = req->adapter;
	struct zfcp_qdio *qdio = adapter->qdio;
	struct fsf_qtcb *qtcb = req->qtcb;
	u32 feat = adapter->adapter_features;

	if (zfcp_adapter_multi_buffer_active(adapter)) {
		if (zfcp_qdio_sbals_from_sg(qdio, &req->qdio_req, sg_req))
			return -EIO;
		qtcb->bottom.support.req_buf_length =
			zfcp_qdio_real_bytes(sg_req);
		if (zfcp_qdio_sbals_from_sg(qdio, &req->qdio_req, sg_resp))
			return -EIO;
		qtcb->bottom.support.resp_buf_length =
			zfcp_qdio_real_bytes(sg_resp);

		zfcp_qdio_set_data_div(qdio, &req->qdio_req, sg_nents(sg_req));
		zfcp_qdio_set_sbale_last(qdio, &req->qdio_req);
		zfcp_qdio_set_scount(qdio, &req->qdio_req);
		return 0;
	}

	/* use single, unchained SBAL if it can hold the request */
	if (zfcp_qdio_sg_one_sbale(sg_req) && zfcp_qdio_sg_one_sbale(sg_resp)) {
		zfcp_fsf_setup_ct_els_unchained(qdio, &req->qdio_req,
						sg_req, sg_resp);
		return 0;
	}

	if (!(feat & FSF_FEATURE_ELS_CT_CHAINED_SBALS))
		return -EOPNOTSUPP;

	if (zfcp_qdio_sbals_from_sg(qdio, &req->qdio_req, sg_req))
		return -EIO;

	qtcb->bottom.support.req_buf_length = zfcp_qdio_real_bytes(sg_req);

	zfcp_qdio_set_sbale_last(qdio, &req->qdio_req);
	zfcp_qdio_skip_to_last_sbale(qdio, &req->qdio_req);

	if (zfcp_qdio_sbals_from_sg(qdio, &req->qdio_req, sg_resp))
		return -EIO;

	qtcb->bottom.support.resp_buf_length = zfcp_qdio_real_bytes(sg_resp);

	zfcp_qdio_set_sbale_last(qdio, &req->qdio_req);

	return 0;
}

static int zfcp_fsf_setup_ct_els(struct zfcp_fsf_req *req,
				 struct scatterlist *sg_req,
				 struct scatterlist *sg_resp,
				 unsigned int timeout)
{
	int ret;

	ret = zfcp_fsf_setup_ct_els_sbals(req, sg_req, sg_resp);
	if (ret)
		return ret;

	/* common settings for ct/gs and els requests */
	if (timeout > 255)
		timeout = 255; /* max value accepted by hardware */
	req->qtcb->bottom.support.service_class = FSF_CLASS_3;
	req->qtcb->bottom.support.timeout = timeout;
	zfcp_fsf_start_timer(req, (timeout + 10) * HZ);

	return 0;
}

/**
 * zfcp_fsf_send_ct - initiate a Generic Service request (FC-GS)
 * @wka_port: pointer to zfcp WKA port to send CT/GS to
 * @ct: pointer to struct zfcp_send_ct with data for request
 * @pool: if non-null this mempool is used to allocate struct zfcp_fsf_req
 * @timeout: timeout that hardware should use, and a later software timeout
 */
int zfcp_fsf_send_ct(struct zfcp_fc_wka_port *wka_port,
		     struct zfcp_fsf_ct_els *ct, mempool_t *pool,
		     unsigned int timeout)
{
	struct zfcp_qdio *qdio = wka_port->adapter->qdio;
	struct zfcp_fsf_req *req;
	int ret = -EIO;

	spin_lock_irq(&qdio->req_q_lock);
	if (zfcp_qdio_sbal_get(qdio))
		goto out;

	req = zfcp_fsf_req_create(qdio, FSF_QTCB_SEND_GENERIC,
				  SBAL_SFLAGS0_TYPE_WRITE_READ, pool);

	if (IS_ERR(req)) {
		ret = PTR_ERR(req);
		goto out;
	}

	req->status |= ZFCP_STATUS_FSFREQ_CLEANUP;
	ret = zfcp_fsf_setup_ct_els(req, ct->req, ct->resp, timeout);
	if (ret)
		goto failed_send;

	req->handler = zfcp_fsf_send_ct_handler;
	req->qtcb->header.port_handle = wka_port->handle;
	ct->d_id = wka_port->d_id;
	req->data = ct;

	zfcp_dbf_san_req("fssct_1", req, wka_port->d_id);

	ret = zfcp_fsf_req_send(req);
	if (ret)
		goto failed_send;
	/* NOTE: DO NOT TOUCH req PAST THIS POINT! */

	goto out;

failed_send:
	zfcp_fsf_req_free(req);
out:
	spin_unlock_irq(&qdio->req_q_lock);
	return ret;
}

static void zfcp_fsf_send_els_handler(struct zfcp_fsf_req *req)
{
	struct zfcp_fsf_ct_els *send_els = req->data;
	struct fsf_qtcb_header *header = &req->qtcb->header;

	send_els->status = -EINVAL;

	if (req->status & ZFCP_STATUS_FSFREQ_ERROR)
		goto skip_fsfstatus;

	switch (header->fsf_status) {
	case FSF_GOOD:
		send_els->status = 0;
		zfcp_dbf_san_res("fsselh1", req);
		break;
	case FSF_SERVICE_CLASS_NOT_SUPPORTED:
		zfcp_fsf_class_not_supp(req);
		break;
	case FSF_ADAPTER_STATUS_AVAILABLE:
		switch (header->fsf_status_qual.word[0]){
		case FSF_SQ_INVOKE_LINK_TEST_PROCEDURE:
		case FSF_SQ_ULP_DEPENDENT_ERP_REQUIRED:
		case FSF_SQ_RETRY_IF_POSSIBLE:
			req->status |= ZFCP_STATUS_FSFREQ_ERROR;
			break;
		}
		break;
	case FSF_ELS_COMMAND_REJECTED:
	case FSF_PAYLOAD_SIZE_MISMATCH:
	case FSF_REQUEST_SIZE_TOO_LARGE:
	case FSF_RESPONSE_SIZE_TOO_LARGE:
		break;
	case FSF_SBAL_MISMATCH:
		/* should never occur, avoided in zfcp_fsf_send_els */
		fallthrough;
	default:
		req->status |= ZFCP_STATUS_FSFREQ_ERROR;
		break;
	}
skip_fsfstatus:
	if (send_els->handler)
		send_els->handler(send_els->handler_data);
}

/**
 * zfcp_fsf_send_els - initiate an ELS command (FC-FS)
 * @adapter: pointer to zfcp adapter
 * @d_id: N_Port_ID to send ELS to
 * @els: pointer to struct zfcp_send_els with data for the command
 * @timeout: timeout that hardware should use, and a later software timeout
 */
int zfcp_fsf_send_els(struct zfcp_adapter *adapter, u32 d_id,
		      struct zfcp_fsf_ct_els *els, unsigned int timeout)
{
	struct zfcp_fsf_req *req;
	struct zfcp_qdio *qdio = adapter->qdio;
	int ret = -EIO;

	spin_lock_irq(&qdio->req_q_lock);
	if (zfcp_qdio_sbal_get(qdio))
		goto out;

	req = zfcp_fsf_req_create(qdio, FSF_QTCB_SEND_ELS,
				  SBAL_SFLAGS0_TYPE_WRITE_READ, NULL);

	if (IS_ERR(req)) {
		ret = PTR_ERR(req);
		goto out;
	}

	req->status |= ZFCP_STATUS_FSFREQ_CLEANUP;

	if (!zfcp_adapter_multi_buffer_active(adapter))
		zfcp_qdio_sbal_limit(qdio, &req->qdio_req, 2);

	ret = zfcp_fsf_setup_ct_els(req, els->req, els->resp, timeout);

	if (ret)
		goto failed_send;

	hton24(req->qtcb->bottom.support.d_id, d_id);
	req->handler = zfcp_fsf_send_els_handler;
	els->d_id = d_id;
	req->data = els;

	zfcp_dbf_san_req("fssels1", req, d_id);

	ret = zfcp_fsf_req_send(req);
	if (ret)
		goto failed_send;
	/* NOTE: DO NOT TOUCH req PAST THIS POINT! */

	goto out;

failed_send:
	zfcp_fsf_req_free(req);
out:
	spin_unlock_irq(&qdio->req_q_lock);
	return ret;
}

int zfcp_fsf_exchange_config_data(struct zfcp_erp_action *erp_action)
{
	struct zfcp_fsf_req *req;
	struct zfcp_qdio *qdio = erp_action->adapter->qdio;
	int retval = -EIO;

	spin_lock_irq(&qdio->req_q_lock);
	if (zfcp_qdio_sbal_get(qdio))
		goto out;

	req = zfcp_fsf_req_create(qdio, FSF_QTCB_EXCHANGE_CONFIG_DATA,
				  SBAL_SFLAGS0_TYPE_READ,
				  qdio->adapter->pool.erp_req);

	if (IS_ERR(req)) {
		retval = PTR_ERR(req);
		goto out;
	}

	req->status |= ZFCP_STATUS_FSFREQ_CLEANUP;
	zfcp_qdio_set_sbale_last(qdio, &req->qdio_req);

	req->qtcb->bottom.config.feature_selection =
			FSF_FEATURE_NOTIFICATION_LOST |
			FSF_FEATURE_UPDATE_ALERT |
			FSF_FEATURE_REQUEST_SFP_DATA |
			FSF_FEATURE_FC_SECURITY;
	req->erp_action = erp_action;
	req->handler = zfcp_fsf_exchange_config_data_handler;
	erp_action->fsf_req_id = req->req_id;

	zfcp_fsf_start_erp_timer(req);
	retval = zfcp_fsf_req_send(req);
	if (retval) {
		zfcp_fsf_req_free(req);
		erp_action->fsf_req_id = 0;
	}
	/* NOTE: DO NOT TOUCH req PAST THIS POINT! */
out:
	spin_unlock_irq(&qdio->req_q_lock);
	return retval;
}


/**
 * zfcp_fsf_exchange_config_data_sync() - Request information about FCP channel.
 * @qdio: pointer to the QDIO-Queue to use for sending the command.
 * @data: pointer to the QTCB-Bottom for storing the result of the command,
 *	  might be %NULL.
 *
 * Returns:
 * * 0		- Exchange Config Data was successful, @data is complete
 * * -EIO	- Exchange Config Data was not successful, @data is invalid
 * * -EAGAIN	- @data contains incomplete data
 * * -ENOMEM	- Some memory allocation failed along the way
 */
int zfcp_fsf_exchange_config_data_sync(struct zfcp_qdio *qdio,
				       struct fsf_qtcb_bottom_config *data)
{
	struct zfcp_fsf_req *req = NULL;
	int retval = -EIO;

	spin_lock_irq(&qdio->req_q_lock);
	if (zfcp_qdio_sbal_get(qdio))
		goto out_unlock;

	req = zfcp_fsf_req_create(qdio, FSF_QTCB_EXCHANGE_CONFIG_DATA,
				  SBAL_SFLAGS0_TYPE_READ, NULL);

	if (IS_ERR(req)) {
		retval = PTR_ERR(req);
		goto out_unlock;
	}

	zfcp_qdio_set_sbale_last(qdio, &req->qdio_req);
	req->handler = zfcp_fsf_exchange_config_data_handler;

	req->qtcb->bottom.config.feature_selection =
			FSF_FEATURE_NOTIFICATION_LOST |
			FSF_FEATURE_UPDATE_ALERT |
			FSF_FEATURE_REQUEST_SFP_DATA |
			FSF_FEATURE_FC_SECURITY;

	if (data)
		req->data = data;

	zfcp_fsf_start_timer(req, ZFCP_FSF_REQUEST_TIMEOUT);
	retval = zfcp_fsf_req_send(req);
	spin_unlock_irq(&qdio->req_q_lock);

	if (!retval) {
		/* NOTE: ONLY TOUCH SYNC req AGAIN ON req->completion. */
		wait_for_completion(&req->completion);

		if (req->status &
		    (ZFCP_STATUS_FSFREQ_ERROR | ZFCP_STATUS_FSFREQ_DISMISSED))
			retval = -EIO;
		else if (req->status & ZFCP_STATUS_FSFREQ_XDATAINCOMPLETE)
			retval = -EAGAIN;
	}

	zfcp_fsf_req_free(req);
	return retval;

out_unlock:
	spin_unlock_irq(&qdio->req_q_lock);
	return retval;
}

/**
 * zfcp_fsf_exchange_port_data - request information about local port
 * @erp_action: ERP action for the adapter for which port data is requested
 * Returns: 0 on success, error otherwise
 */
int zfcp_fsf_exchange_port_data(struct zfcp_erp_action *erp_action)
{
	struct zfcp_qdio *qdio = erp_action->adapter->qdio;
	struct zfcp_fsf_req *req;
	int retval = -EIO;

	if (!(qdio->adapter->adapter_features & FSF_FEATURE_HBAAPI_MANAGEMENT))
		return -EOPNOTSUPP;

	spin_lock_irq(&qdio->req_q_lock);
	if (zfcp_qdio_sbal_get(qdio))
		goto out;

	req = zfcp_fsf_req_create(qdio, FSF_QTCB_EXCHANGE_PORT_DATA,
				  SBAL_SFLAGS0_TYPE_READ,
				  qdio->adapter->pool.erp_req);

	if (IS_ERR(req)) {
		retval = PTR_ERR(req);
		goto out;
	}

	req->status |= ZFCP_STATUS_FSFREQ_CLEANUP;
	zfcp_qdio_set_sbale_last(qdio, &req->qdio_req);

	req->handler = zfcp_fsf_exchange_port_data_handler;
	req->erp_action = erp_action;
	erp_action->fsf_req_id = req->req_id;

	zfcp_fsf_start_erp_timer(req);
	retval = zfcp_fsf_req_send(req);
	if (retval) {
		zfcp_fsf_req_free(req);
		erp_action->fsf_req_id = 0;
	}
	/* NOTE: DO NOT TOUCH req PAST THIS POINT! */
out:
	spin_unlock_irq(&qdio->req_q_lock);
	return retval;
}

/**
 * zfcp_fsf_exchange_port_data_sync() - Request information about local port.
 * @qdio: pointer to the QDIO-Queue to use for sending the command.
 * @data: pointer to the QTCB-Bottom for storing the result of the command,
 *	  might be %NULL.
 *
 * Returns:
 * * 0		- Exchange Port Data was successful, @data is complete
 * * -EIO	- Exchange Port Data was not successful, @data is invalid
 * * -EAGAIN	- @data contains incomplete data
 * * -ENOMEM	- Some memory allocation failed along the way
 * * -EOPNOTSUPP	- This operation is not supported
 */
int zfcp_fsf_exchange_port_data_sync(struct zfcp_qdio *qdio,
				     struct fsf_qtcb_bottom_port *data)
{
	struct zfcp_fsf_req *req = NULL;
	int retval = -EIO;

	if (!(qdio->adapter->adapter_features & FSF_FEATURE_HBAAPI_MANAGEMENT))
		return -EOPNOTSUPP;

	spin_lock_irq(&qdio->req_q_lock);
	if (zfcp_qdio_sbal_get(qdio))
		goto out_unlock;

	req = zfcp_fsf_req_create(qdio, FSF_QTCB_EXCHANGE_PORT_DATA,
				  SBAL_SFLAGS0_TYPE_READ, NULL);

	if (IS_ERR(req)) {
		retval = PTR_ERR(req);
		goto out_unlock;
	}

	if (data)
		req->data = data;

	zfcp_qdio_set_sbale_last(qdio, &req->qdio_req);

	req->handler = zfcp_fsf_exchange_port_data_handler;
	zfcp_fsf_start_timer(req, ZFCP_FSF_REQUEST_TIMEOUT);
	retval = zfcp_fsf_req_send(req);
	spin_unlock_irq(&qdio->req_q_lock);

	if (!retval) {
		/* NOTE: ONLY TOUCH SYNC req AGAIN ON req->completion. */
		wait_for_completion(&req->completion);

		if (req->status &
		    (ZFCP_STATUS_FSFREQ_ERROR | ZFCP_STATUS_FSFREQ_DISMISSED))
			retval = -EIO;
		else if (req->status & ZFCP_STATUS_FSFREQ_XDATAINCOMPLETE)
			retval = -EAGAIN;
	}

	zfcp_fsf_req_free(req);
	return retval;

out_unlock:
	spin_unlock_irq(&qdio->req_q_lock);
	return retval;
}

static void zfcp_fsf_log_port_fc_security(struct zfcp_port *port,
					  struct zfcp_fsf_req *req)
{
	char mnemonic_old[ZFCP_FSF_MAX_FC_SECURITY_MNEMONIC_LENGTH];
	char mnemonic_new[ZFCP_FSF_MAX_FC_SECURITY_MNEMONIC_LENGTH];

	if (port->connection_info == port->connection_info_old) {
		/* no change, no log nor trace */
		return;
	}

	zfcp_dbf_hba_fsf_fces("fsfcesp", req, port->wwpn,
			      port->connection_info_old,
			      port->connection_info);

	zfcp_fsf_scnprint_fc_security(mnemonic_old, sizeof(mnemonic_old),
				      port->connection_info_old,
				      ZFCP_FSF_PRINT_FMT_SINGLEITEM);
	zfcp_fsf_scnprint_fc_security(mnemonic_new, sizeof(mnemonic_new),
				      port->connection_info,
				      ZFCP_FSF_PRINT_FMT_SINGLEITEM);

	if (strncmp(mnemonic_old, mnemonic_new,
		    ZFCP_FSF_MAX_FC_SECURITY_MNEMONIC_LENGTH) == 0) {
		/* no change in string representation, no log */
		goto out;
	}

	if (port->connection_info_old == 0) {
		/* activation */
		dev_info(&port->adapter->ccw_device->dev,
			 "FC Endpoint Security of connection to remote port 0x%16llx enabled: %s\n",
			 port->wwpn, mnemonic_new);
	} else if (port->connection_info == 0) {
		/* deactivation */
		dev_warn(&port->adapter->ccw_device->dev,
			 "FC Endpoint Security of connection to remote port 0x%16llx disabled: was %s\n",
			 port->wwpn, mnemonic_old);
	} else {
		/* change */
		dev_warn(&port->adapter->ccw_device->dev,
			 "FC Endpoint Security of connection to remote port 0x%16llx changed: from %s to %s\n",
			 port->wwpn, mnemonic_old, mnemonic_new);
	}

out:
	port->connection_info_old = port->connection_info;
}

static void zfcp_fsf_log_security_error(const struct device *dev, u32 fsf_sqw0,
					u64 wwpn)
{
	switch (fsf_sqw0) {

	/*
	 * Open Port command error codes
	 */

	case FSF_SQ_SECURITY_REQUIRED:
		dev_warn_ratelimited(dev,
				     "FC Endpoint Security error: FC security is required but not supported or configured on remote port 0x%016llx\n",
				     wwpn);
		break;
	case FSF_SQ_SECURITY_TIMEOUT:
		dev_warn_ratelimited(dev,
				     "FC Endpoint Security error: a timeout prevented opening remote port 0x%016llx\n",
				     wwpn);
		break;
	case FSF_SQ_SECURITY_KM_UNAVAILABLE:
		dev_warn_ratelimited(dev,
				     "FC Endpoint Security error: opening remote port 0x%016llx failed because local and external key manager cannot communicate\n",
				     wwpn);
		break;
	case FSF_SQ_SECURITY_RKM_UNAVAILABLE:
		dev_warn_ratelimited(dev,
				     "FC Endpoint Security error: opening remote port 0x%016llx failed because it cannot communicate with the external key manager\n",
				     wwpn);
		break;
	case FSF_SQ_SECURITY_AUTH_FAILURE:
		dev_warn_ratelimited(dev,
				     "FC Endpoint Security error: the device could not verify the identity of remote port 0x%016llx\n",
				     wwpn);
		break;

	/*
	 * Send FCP command error codes
	 */

	case FSF_SQ_SECURITY_ENC_FAILURE:
		dev_warn_ratelimited(dev,
				     "FC Endpoint Security error: FC connection to remote port 0x%016llx closed because encryption broke down\n",
				     wwpn);
		break;

	/*
	 * Unknown error codes
	 */

	default:
		dev_warn_ratelimited(dev,
				     "FC Endpoint Security error: the device issued an unknown error code 0x%08x related to the FC connection to remote port 0x%016llx\n",
				     fsf_sqw0, wwpn);
	}
}

static void zfcp_fsf_open_port_handler(struct zfcp_fsf_req *req)
{
	struct zfcp_adapter *adapter = req->adapter;
	struct zfcp_port *port = req->data;
	struct fsf_qtcb_header *header = &req->qtcb->header;
	struct fsf_qtcb_bottom_support *bottom = &req->qtcb->bottom.support;
	struct fc_els_flogi *plogi;

	if (req->status & ZFCP_STATUS_FSFREQ_ERROR)
		goto out;

	switch (header->fsf_status) {
	case FSF_PORT_ALREADY_OPEN:
		break;
	case FSF_MAXIMUM_NUMBER_OF_PORTS_EXCEEDED:
		dev_warn(&adapter->ccw_device->dev,
			 "Not enough FCP adapter resources to open "
			 "remote port 0x%016Lx\n",
			 (unsigned long long)port->wwpn);
		zfcp_erp_set_port_status(port,
					 ZFCP_STATUS_COMMON_ERP_FAILED);
		req->status |= ZFCP_STATUS_FSFREQ_ERROR;
		break;
	case FSF_SECURITY_ERROR:
		zfcp_fsf_log_security_error(&req->adapter->ccw_device->dev,
					    header->fsf_status_qual.word[0],
					    port->wwpn);
		req->status |= ZFCP_STATUS_FSFREQ_ERROR;
		break;
	case FSF_ADAPTER_STATUS_AVAILABLE:
		switch (header->fsf_status_qual.word[0]) {
		case FSF_SQ_INVOKE_LINK_TEST_PROCEDURE:
			/* no zfcp_fc_test_link() with failed open port */
			fallthrough;
		case FSF_SQ_ULP_DEPENDENT_ERP_REQUIRED:
		case FSF_SQ_NO_RETRY_POSSIBLE:
			req->status |= ZFCP_STATUS_FSFREQ_ERROR;
			break;
		}
		break;
	case FSF_GOOD:
		port->handle = header->port_handle;
		if (adapter->adapter_features & FSF_FEATURE_FC_SECURITY)
			port->connection_info = bottom->connection_info;
		else
			port->connection_info = 0;
		zfcp_fsf_log_port_fc_security(port, req);
		atomic_or(ZFCP_STATUS_COMMON_OPEN |
				ZFCP_STATUS_PORT_PHYS_OPEN, &port->status);
		atomic_andnot(ZFCP_STATUS_COMMON_ACCESS_BOXED,
		                  &port->status);
		/* check whether D_ID has changed during open */
		/*
		 * FIXME: This check is not airtight, as the FCP channel does
		 * not monitor closures of target port connections caused on
		 * the remote side. Thus, they might miss out on invalidating
		 * locally cached WWPNs (and other N_Port parameters) of gone
		 * target ports. So, our heroic attempt to make things safe
		 * could be undermined by 'open port' response data tagged with
		 * obsolete WWPNs. Another reason to monitor potential
		 * connection closures ourself at least (by interpreting
		 * incoming ELS' and unsolicited status). It just crosses my
		 * mind that one should be able to cross-check by means of
		 * another GID_PN straight after a port has been opened.
		 * Alternately, an ADISC/PDISC ELS should suffice, as well.
		 */
		plogi = (struct fc_els_flogi *) bottom->els;
		if (bottom->els1_length >= FSF_PLOGI_MIN_LEN)
			zfcp_fc_plogi_evaluate(port, plogi);
		break;
	case FSF_UNKNOWN_OP_SUBTYPE:
		req->status |= ZFCP_STATUS_FSFREQ_ERROR;
		break;
	}

out:
	put_device(&port->dev);
}

/**
 * zfcp_fsf_open_port - create and send open port request
 * @erp_action: pointer to struct zfcp_erp_action
 * Returns: 0 on success, error otherwise
 */
int zfcp_fsf_open_port(struct zfcp_erp_action *erp_action)
{
	struct zfcp_qdio *qdio = erp_action->adapter->qdio;
	struct zfcp_port *port = erp_action->port;
	struct zfcp_fsf_req *req;
	int retval = -EIO;

	spin_lock_irq(&qdio->req_q_lock);
	if (zfcp_qdio_sbal_get(qdio))
		goto out;

	req = zfcp_fsf_req_create(qdio, FSF_QTCB_OPEN_PORT_WITH_DID,
				  SBAL_SFLAGS0_TYPE_READ,
				  qdio->adapter->pool.erp_req);

	if (IS_ERR(req)) {
		retval = PTR_ERR(req);
		goto out;
	}

	req->status |= ZFCP_STATUS_FSFREQ_CLEANUP;
	zfcp_qdio_set_sbale_last(qdio, &req->qdio_req);

	req->handler = zfcp_fsf_open_port_handler;
	hton24(req->qtcb->bottom.support.d_id, port->d_id);
	req->data = port;
	req->erp_action = erp_action;
	erp_action->fsf_req_id = req->req_id;
	get_device(&port->dev);

	zfcp_fsf_start_erp_timer(req);
	retval = zfcp_fsf_req_send(req);
	if (retval) {
		zfcp_fsf_req_free(req);
		erp_action->fsf_req_id = 0;
		put_device(&port->dev);
	}
	/* NOTE: DO NOT TOUCH req PAST THIS POINT! */
out:
	spin_unlock_irq(&qdio->req_q_lock);
	return retval;
}

static void zfcp_fsf_close_port_handler(struct zfcp_fsf_req *req)
{
	struct zfcp_port *port = req->data;

	if (req->status & ZFCP_STATUS_FSFREQ_ERROR)
		return;

	switch (req->qtcb->header.fsf_status) {
	case FSF_PORT_HANDLE_NOT_VALID:
		zfcp_erp_adapter_reopen(port->adapter, 0, "fscph_1");
		req->status |= ZFCP_STATUS_FSFREQ_ERROR;
		break;
	case FSF_ADAPTER_STATUS_AVAILABLE:
		break;
	case FSF_GOOD:
		zfcp_erp_clear_port_status(port, ZFCP_STATUS_COMMON_OPEN);
		break;
	}
}

/**
 * zfcp_fsf_close_port - create and send close port request
 * @erp_action: pointer to struct zfcp_erp_action
 * Returns: 0 on success, error otherwise
 */
int zfcp_fsf_close_port(struct zfcp_erp_action *erp_action)
{
	struct zfcp_qdio *qdio = erp_action->adapter->qdio;
	struct zfcp_fsf_req *req;
	int retval = -EIO;

	spin_lock_irq(&qdio->req_q_lock);
	if (zfcp_qdio_sbal_get(qdio))
		goto out;

	req = zfcp_fsf_req_create(qdio, FSF_QTCB_CLOSE_PORT,
				  SBAL_SFLAGS0_TYPE_READ,
				  qdio->adapter->pool.erp_req);

	if (IS_ERR(req)) {
		retval = PTR_ERR(req);
		goto out;
	}

	req->status |= ZFCP_STATUS_FSFREQ_CLEANUP;
	zfcp_qdio_set_sbale_last(qdio, &req->qdio_req);

	req->handler = zfcp_fsf_close_port_handler;
	req->data = erp_action->port;
	req->erp_action = erp_action;
	req->qtcb->header.port_handle = erp_action->port->handle;
	erp_action->fsf_req_id = req->req_id;

	zfcp_fsf_start_erp_timer(req);
	retval = zfcp_fsf_req_send(req);
	if (retval) {
		zfcp_fsf_req_free(req);
		erp_action->fsf_req_id = 0;
	}
	/* NOTE: DO NOT TOUCH req PAST THIS POINT! */
out:
	spin_unlock_irq(&qdio->req_q_lock);
	return retval;
}

static void zfcp_fsf_open_wka_port_handler(struct zfcp_fsf_req *req)
{
	struct zfcp_fc_wka_port *wka_port = req->data;
	struct fsf_qtcb_header *header = &req->qtcb->header;

	if (req->status & ZFCP_STATUS_FSFREQ_ERROR) {
		wka_port->status = ZFCP_FC_WKA_PORT_OFFLINE;
		goto out;
	}

	switch (header->fsf_status) {
	case FSF_MAXIMUM_NUMBER_OF_PORTS_EXCEEDED:
		dev_warn(&req->adapter->ccw_device->dev,
			 "Opening WKA port 0x%x failed\n", wka_port->d_id);
		fallthrough;
	case FSF_ADAPTER_STATUS_AVAILABLE:
		req->status |= ZFCP_STATUS_FSFREQ_ERROR;
		wka_port->status = ZFCP_FC_WKA_PORT_OFFLINE;
		break;
	case FSF_GOOD:
		wka_port->handle = header->port_handle;
		fallthrough;
	case FSF_PORT_ALREADY_OPEN:
		wka_port->status = ZFCP_FC_WKA_PORT_ONLINE;
	}
out:
	wake_up(&wka_port->completion_wq);
}

/**
 * zfcp_fsf_open_wka_port - create and send open wka-port request
 * @wka_port: pointer to struct zfcp_fc_wka_port
 * Returns: 0 on success, error otherwise
 */
int zfcp_fsf_open_wka_port(struct zfcp_fc_wka_port *wka_port)
{
	struct zfcp_qdio *qdio = wka_port->adapter->qdio;
	struct zfcp_fsf_req *req;
	unsigned long req_id = 0;
	int retval = -EIO;

	spin_lock_irq(&qdio->req_q_lock);
	if (zfcp_qdio_sbal_get(qdio))
		goto out;

	req = zfcp_fsf_req_create(qdio, FSF_QTCB_OPEN_PORT_WITH_DID,
				  SBAL_SFLAGS0_TYPE_READ,
				  qdio->adapter->pool.erp_req);

	if (IS_ERR(req)) {
		retval = PTR_ERR(req);
		goto out;
	}

	req->status |= ZFCP_STATUS_FSFREQ_CLEANUP;
	zfcp_qdio_set_sbale_last(qdio, &req->qdio_req);

	req->handler = zfcp_fsf_open_wka_port_handler;
	hton24(req->qtcb->bottom.support.d_id, wka_port->d_id);
	req->data = wka_port;

	req_id = req->req_id;

	zfcp_fsf_start_timer(req, ZFCP_FSF_REQUEST_TIMEOUT);
	retval = zfcp_fsf_req_send(req);
	if (retval)
		zfcp_fsf_req_free(req);
	/* NOTE: DO NOT TOUCH req PAST THIS POINT! */
out:
	spin_unlock_irq(&qdio->req_q_lock);
	if (!retval)
		zfcp_dbf_rec_run_wka("fsowp_1", wka_port, req_id);
	return retval;
}

static void zfcp_fsf_close_wka_port_handler(struct zfcp_fsf_req *req)
{
	struct zfcp_fc_wka_port *wka_port = req->data;

	if (req->qtcb->header.fsf_status == FSF_PORT_HANDLE_NOT_VALID) {
		req->status |= ZFCP_STATUS_FSFREQ_ERROR;
		zfcp_erp_adapter_reopen(wka_port->adapter, 0, "fscwph1");
	}

	wka_port->status = ZFCP_FC_WKA_PORT_OFFLINE;
	wake_up(&wka_port->completion_wq);
}

/**
 * zfcp_fsf_close_wka_port - create and send close wka port request
 * @wka_port: WKA port to open
 * Returns: 0 on success, error otherwise
 */
int zfcp_fsf_close_wka_port(struct zfcp_fc_wka_port *wka_port)
{
	struct zfcp_qdio *qdio = wka_port->adapter->qdio;
	struct zfcp_fsf_req *req;
	unsigned long req_id = 0;
	int retval = -EIO;

	spin_lock_irq(&qdio->req_q_lock);
	if (zfcp_qdio_sbal_get(qdio))
		goto out;

	req = zfcp_fsf_req_create(qdio, FSF_QTCB_CLOSE_PORT,
				  SBAL_SFLAGS0_TYPE_READ,
				  qdio->adapter->pool.erp_req);

	if (IS_ERR(req)) {
		retval = PTR_ERR(req);
		goto out;
	}

	req->status |= ZFCP_STATUS_FSFREQ_CLEANUP;
	zfcp_qdio_set_sbale_last(qdio, &req->qdio_req);

	req->handler = zfcp_fsf_close_wka_port_handler;
	req->data = wka_port;
	req->qtcb->header.port_handle = wka_port->handle;

	req_id = req->req_id;

	zfcp_fsf_start_timer(req, ZFCP_FSF_REQUEST_TIMEOUT);
	retval = zfcp_fsf_req_send(req);
	if (retval)
		zfcp_fsf_req_free(req);
	/* NOTE: DO NOT TOUCH req PAST THIS POINT! */
out:
	spin_unlock_irq(&qdio->req_q_lock);
	if (!retval)
		zfcp_dbf_rec_run_wka("fscwp_1", wka_port, req_id);
	return retval;
}

static void zfcp_fsf_close_physical_port_handler(struct zfcp_fsf_req *req)
{
	struct zfcp_port *port = req->data;
	struct fsf_qtcb_header *header = &req->qtcb->header;
	struct scsi_device *sdev;

	if (req->status & ZFCP_STATUS_FSFREQ_ERROR)
		return;

	switch (header->fsf_status) {
	case FSF_PORT_HANDLE_NOT_VALID:
		zfcp_erp_adapter_reopen(port->adapter, 0, "fscpph1");
		req->status |= ZFCP_STATUS_FSFREQ_ERROR;
		break;
	case FSF_PORT_BOXED:
		/* can't use generic zfcp_erp_modify_port_status because
		 * ZFCP_STATUS_COMMON_OPEN must not be reset for the port */
		atomic_andnot(ZFCP_STATUS_PORT_PHYS_OPEN, &port->status);
		shost_for_each_device(sdev, port->adapter->scsi_host)
			if (sdev_to_zfcp(sdev)->port == port)
				atomic_andnot(ZFCP_STATUS_COMMON_OPEN,
						  &sdev_to_zfcp(sdev)->status);
		zfcp_erp_set_port_status(port, ZFCP_STATUS_COMMON_ACCESS_BOXED);
		zfcp_erp_port_reopen(port, ZFCP_STATUS_COMMON_ERP_FAILED,
				     "fscpph2");
		req->status |= ZFCP_STATUS_FSFREQ_ERROR;
		break;
	case FSF_ADAPTER_STATUS_AVAILABLE:
		switch (header->fsf_status_qual.word[0]) {
		case FSF_SQ_INVOKE_LINK_TEST_PROCEDURE:
		case FSF_SQ_ULP_DEPENDENT_ERP_REQUIRED:
			req->status |= ZFCP_STATUS_FSFREQ_ERROR;
			break;
		}
		break;
	case FSF_GOOD:
		/* can't use generic zfcp_erp_modify_port_status because
		 * ZFCP_STATUS_COMMON_OPEN must not be reset for the port
		 */
		atomic_andnot(ZFCP_STATUS_PORT_PHYS_OPEN, &port->status);
		shost_for_each_device(sdev, port->adapter->scsi_host)
			if (sdev_to_zfcp(sdev)->port == port)
				atomic_andnot(ZFCP_STATUS_COMMON_OPEN,
						  &sdev_to_zfcp(sdev)->status);
		break;
	}
}

/**
 * zfcp_fsf_close_physical_port - close physical port
 * @erp_action: pointer to struct zfcp_erp_action
 * Returns: 0 on success
 */
int zfcp_fsf_close_physical_port(struct zfcp_erp_action *erp_action)
{
	struct zfcp_qdio *qdio = erp_action->adapter->qdio;
	struct zfcp_fsf_req *req;
	int retval = -EIO;

	spin_lock_irq(&qdio->req_q_lock);
	if (zfcp_qdio_sbal_get(qdio))
		goto out;

	req = zfcp_fsf_req_create(qdio, FSF_QTCB_CLOSE_PHYSICAL_PORT,
				  SBAL_SFLAGS0_TYPE_READ,
				  qdio->adapter->pool.erp_req);

	if (IS_ERR(req)) {
		retval = PTR_ERR(req);
		goto out;
	}

	req->status |= ZFCP_STATUS_FSFREQ_CLEANUP;
	zfcp_qdio_set_sbale_last(qdio, &req->qdio_req);

	req->data = erp_action->port;
	req->qtcb->header.port_handle = erp_action->port->handle;
	req->erp_action = erp_action;
	req->handler = zfcp_fsf_close_physical_port_handler;
	erp_action->fsf_req_id = req->req_id;

	zfcp_fsf_start_erp_timer(req);
	retval = zfcp_fsf_req_send(req);
	if (retval) {
		zfcp_fsf_req_free(req);
		erp_action->fsf_req_id = 0;
	}
	/* NOTE: DO NOT TOUCH req PAST THIS POINT! */
out:
	spin_unlock_irq(&qdio->req_q_lock);
	return retval;
}

static void zfcp_fsf_open_lun_handler(struct zfcp_fsf_req *req)
{
	struct zfcp_adapter *adapter = req->adapter;
	struct scsi_device *sdev = req->data;
	struct zfcp_scsi_dev *zfcp_sdev;
	struct fsf_qtcb_header *header = &req->qtcb->header;
	union fsf_status_qual *qual = &header->fsf_status_qual;

	if (req->status & ZFCP_STATUS_FSFREQ_ERROR)
		return;

	zfcp_sdev = sdev_to_zfcp(sdev);

	atomic_andnot(ZFCP_STATUS_COMMON_ACCESS_DENIED |
			  ZFCP_STATUS_COMMON_ACCESS_BOXED,
			  &zfcp_sdev->status);

	switch (header->fsf_status) {

	case FSF_PORT_HANDLE_NOT_VALID:
		zfcp_erp_adapter_reopen(adapter, 0, "fsouh_1");
		fallthrough;
	case FSF_LUN_ALREADY_OPEN:
		break;
	case FSF_PORT_BOXED:
		zfcp_erp_set_port_status(zfcp_sdev->port,
					 ZFCP_STATUS_COMMON_ACCESS_BOXED);
		zfcp_erp_port_reopen(zfcp_sdev->port,
				     ZFCP_STATUS_COMMON_ERP_FAILED, "fsouh_2");
		req->status |= ZFCP_STATUS_FSFREQ_ERROR;
		break;
	case FSF_LUN_SHARING_VIOLATION:
		if (qual->word[0])
			dev_warn(&zfcp_sdev->port->adapter->ccw_device->dev,
				 "LUN 0x%016Lx on port 0x%016Lx is already in "
				 "use by CSS%d, MIF Image ID %x\n",
				 zfcp_scsi_dev_lun(sdev),
				 (unsigned long long)zfcp_sdev->port->wwpn,
				 qual->fsf_queue_designator.cssid,
				 qual->fsf_queue_designator.hla);
		zfcp_erp_set_lun_status(sdev,
					ZFCP_STATUS_COMMON_ERP_FAILED |
					ZFCP_STATUS_COMMON_ACCESS_DENIED);
		req->status |= ZFCP_STATUS_FSFREQ_ERROR;
		break;
	case FSF_MAXIMUM_NUMBER_OF_LUNS_EXCEEDED:
		dev_warn(&adapter->ccw_device->dev,
			 "No handle is available for LUN "
			 "0x%016Lx on port 0x%016Lx\n",
			 (unsigned long long)zfcp_scsi_dev_lun(sdev),
			 (unsigned long long)zfcp_sdev->port->wwpn);
		zfcp_erp_set_lun_status(sdev, ZFCP_STATUS_COMMON_ERP_FAILED);
		fallthrough;
	case FSF_INVALID_COMMAND_OPTION:
		req->status |= ZFCP_STATUS_FSFREQ_ERROR;
		break;
	case FSF_ADAPTER_STATUS_AVAILABLE:
		switch (header->fsf_status_qual.word[0]) {
		case FSF_SQ_INVOKE_LINK_TEST_PROCEDURE:
			zfcp_fc_test_link(zfcp_sdev->port);
			fallthrough;
		case FSF_SQ_ULP_DEPENDENT_ERP_REQUIRED:
			req->status |= ZFCP_STATUS_FSFREQ_ERROR;
			break;
		}
		break;

	case FSF_GOOD:
		zfcp_sdev->lun_handle = header->lun_handle;
		atomic_or(ZFCP_STATUS_COMMON_OPEN, &zfcp_sdev->status);
		break;
	}
}

/**
 * zfcp_fsf_open_lun - open LUN
 * @erp_action: pointer to struct zfcp_erp_action
 * Returns: 0 on success, error otherwise
 */
int zfcp_fsf_open_lun(struct zfcp_erp_action *erp_action)
{
	struct zfcp_adapter *adapter = erp_action->adapter;
	struct zfcp_qdio *qdio = adapter->qdio;
	struct zfcp_fsf_req *req;
	int retval = -EIO;

	spin_lock_irq(&qdio->req_q_lock);
	if (zfcp_qdio_sbal_get(qdio))
		goto out;

	req = zfcp_fsf_req_create(qdio, FSF_QTCB_OPEN_LUN,
				  SBAL_SFLAGS0_TYPE_READ,
				  adapter->pool.erp_req);

	if (IS_ERR(req)) {
		retval = PTR_ERR(req);
		goto out;
	}

	req->status |= ZFCP_STATUS_FSFREQ_CLEANUP;
	zfcp_qdio_set_sbale_last(qdio, &req->qdio_req);

	req->qtcb->header.port_handle = erp_action->port->handle;
	req->qtcb->bottom.support.fcp_lun = zfcp_scsi_dev_lun(erp_action->sdev);
	req->handler = zfcp_fsf_open_lun_handler;
	req->data = erp_action->sdev;
	req->erp_action = erp_action;
	erp_action->fsf_req_id = req->req_id;

	if (!(adapter->connection_features & FSF_FEATURE_NPIV_MODE))
		req->qtcb->bottom.support.option = FSF_OPEN_LUN_SUPPRESS_BOXING;

	zfcp_fsf_start_erp_timer(req);
	retval = zfcp_fsf_req_send(req);
	if (retval) {
		zfcp_fsf_req_free(req);
		erp_action->fsf_req_id = 0;
	}
	/* NOTE: DO NOT TOUCH req PAST THIS POINT! */
out:
	spin_unlock_irq(&qdio->req_q_lock);
	return retval;
}

static void zfcp_fsf_close_lun_handler(struct zfcp_fsf_req *req)
{
	struct scsi_device *sdev = req->data;
	struct zfcp_scsi_dev *zfcp_sdev;

	if (req->status & ZFCP_STATUS_FSFREQ_ERROR)
		return;

	zfcp_sdev = sdev_to_zfcp(sdev);

	switch (req->qtcb->header.fsf_status) {
	case FSF_PORT_HANDLE_NOT_VALID:
		zfcp_erp_adapter_reopen(zfcp_sdev->port->adapter, 0, "fscuh_1");
		req->status |= ZFCP_STATUS_FSFREQ_ERROR;
		break;
	case FSF_LUN_HANDLE_NOT_VALID:
		zfcp_erp_port_reopen(zfcp_sdev->port, 0, "fscuh_2");
		req->status |= ZFCP_STATUS_FSFREQ_ERROR;
		break;
	case FSF_PORT_BOXED:
		zfcp_erp_set_port_status(zfcp_sdev->port,
					 ZFCP_STATUS_COMMON_ACCESS_BOXED);
		zfcp_erp_port_reopen(zfcp_sdev->port,
				     ZFCP_STATUS_COMMON_ERP_FAILED, "fscuh_3");
		req->status |= ZFCP_STATUS_FSFREQ_ERROR;
		break;
	case FSF_ADAPTER_STATUS_AVAILABLE:
		switch (req->qtcb->header.fsf_status_qual.word[0]) {
		case FSF_SQ_INVOKE_LINK_TEST_PROCEDURE:
			zfcp_fc_test_link(zfcp_sdev->port);
			fallthrough;
		case FSF_SQ_ULP_DEPENDENT_ERP_REQUIRED:
			req->status |= ZFCP_STATUS_FSFREQ_ERROR;
			break;
		}
		break;
	case FSF_GOOD:
		atomic_andnot(ZFCP_STATUS_COMMON_OPEN, &zfcp_sdev->status);
		break;
	}
}

/**
 * zfcp_fsf_close_LUN - close LUN
 * @erp_action: pointer to erp_action triggering the "close LUN"
 * Returns: 0 on success, error otherwise
 */
int zfcp_fsf_close_lun(struct zfcp_erp_action *erp_action)
{
	struct zfcp_qdio *qdio = erp_action->adapter->qdio;
	struct zfcp_scsi_dev *zfcp_sdev = sdev_to_zfcp(erp_action->sdev);
	struct zfcp_fsf_req *req;
	int retval = -EIO;

	spin_lock_irq(&qdio->req_q_lock);
	if (zfcp_qdio_sbal_get(qdio))
		goto out;

	req = zfcp_fsf_req_create(qdio, FSF_QTCB_CLOSE_LUN,
				  SBAL_SFLAGS0_TYPE_READ,
				  qdio->adapter->pool.erp_req);

	if (IS_ERR(req)) {
		retval = PTR_ERR(req);
		goto out;
	}

	req->status |= ZFCP_STATUS_FSFREQ_CLEANUP;
	zfcp_qdio_set_sbale_last(qdio, &req->qdio_req);

	req->qtcb->header.port_handle = erp_action->port->handle;
	req->qtcb->header.lun_handle = zfcp_sdev->lun_handle;
	req->handler = zfcp_fsf_close_lun_handler;
	req->data = erp_action->sdev;
	req->erp_action = erp_action;
	erp_action->fsf_req_id = req->req_id;

	zfcp_fsf_start_erp_timer(req);
	retval = zfcp_fsf_req_send(req);
	if (retval) {
		zfcp_fsf_req_free(req);
		erp_action->fsf_req_id = 0;
	}
	/* NOTE: DO NOT TOUCH req PAST THIS POINT! */
out:
	spin_unlock_irq(&qdio->req_q_lock);
	return retval;
}

static void zfcp_fsf_update_lat(struct zfcp_latency_record *lat_rec, u32 lat)
{
	lat_rec->sum += lat;
	lat_rec->min = min(lat_rec->min, lat);
	lat_rec->max = max(lat_rec->max, lat);
}

static void zfcp_fsf_req_trace(struct zfcp_fsf_req *req, struct scsi_cmnd *scsi)
{
	struct fsf_qual_latency_info *lat_in;
	struct zfcp_latency_cont *lat = NULL;
	struct zfcp_scsi_dev *zfcp_sdev;
	struct zfcp_blk_drv_data blktrc;
	int ticks = req->adapter->timer_ticks;

	lat_in = &req->qtcb->prefix.prot_status_qual.latency_info;

	blktrc.flags = 0;
	blktrc.magic = ZFCP_BLK_DRV_DATA_MAGIC;
	if (req->status & ZFCP_STATUS_FSFREQ_ERROR)
		blktrc.flags |= ZFCP_BLK_REQ_ERROR;
	blktrc.inb_usage = 0;
	blktrc.outb_usage = req->qdio_req.qdio_outb_usage;

	if (req->adapter->adapter_features & FSF_FEATURE_MEASUREMENT_DATA &&
	    !(req->status & ZFCP_STATUS_FSFREQ_ERROR)) {
		zfcp_sdev = sdev_to_zfcp(scsi->device);
		blktrc.flags |= ZFCP_BLK_LAT_VALID;
		blktrc.channel_lat = lat_in->channel_lat * ticks;
		blktrc.fabric_lat = lat_in->fabric_lat * ticks;

		switch (req->qtcb->bottom.io.data_direction) {
		case FSF_DATADIR_DIF_READ_STRIP:
		case FSF_DATADIR_DIF_READ_CONVERT:
		case FSF_DATADIR_READ:
			lat = &zfcp_sdev->latencies.read;
			break;
		case FSF_DATADIR_DIF_WRITE_INSERT:
		case FSF_DATADIR_DIF_WRITE_CONVERT:
		case FSF_DATADIR_WRITE:
			lat = &zfcp_sdev->latencies.write;
			break;
		case FSF_DATADIR_CMND:
			lat = &zfcp_sdev->latencies.cmd;
			break;
		}

		if (lat) {
			spin_lock(&zfcp_sdev->latencies.lock);
			zfcp_fsf_update_lat(&lat->channel, lat_in->channel_lat);
			zfcp_fsf_update_lat(&lat->fabric, lat_in->fabric_lat);
			lat->counter++;
			spin_unlock(&zfcp_sdev->latencies.lock);
		}
	}

	blk_add_driver_data(scsi->request, &blktrc, sizeof(blktrc));
}

/**
 * zfcp_fsf_fcp_handler_common() - FCP response handler common to I/O and TMF.
 * @req: Pointer to FSF request.
 * @sdev: Pointer to SCSI device as request context.
 */
static void zfcp_fsf_fcp_handler_common(struct zfcp_fsf_req *req,
					struct scsi_device *sdev)
{
	struct zfcp_scsi_dev *zfcp_sdev;
	struct fsf_qtcb_header *header = &req->qtcb->header;

	if (unlikely(req->status & ZFCP_STATUS_FSFREQ_ERROR))
		return;

	zfcp_sdev = sdev_to_zfcp(sdev);

	switch (header->fsf_status) {
	case FSF_HANDLE_MISMATCH:
	case FSF_PORT_HANDLE_NOT_VALID:
		zfcp_erp_adapter_reopen(req->adapter, 0, "fssfch1");
		req->status |= ZFCP_STATUS_FSFREQ_ERROR;
		break;
	case FSF_FCPLUN_NOT_VALID:
	case FSF_LUN_HANDLE_NOT_VALID:
		zfcp_erp_port_reopen(zfcp_sdev->port, 0, "fssfch2");
		req->status |= ZFCP_STATUS_FSFREQ_ERROR;
		break;
	case FSF_SERVICE_CLASS_NOT_SUPPORTED:
		zfcp_fsf_class_not_supp(req);
		break;
	case FSF_DIRECTION_INDICATOR_NOT_VALID:
		dev_err(&req->adapter->ccw_device->dev,
			"Incorrect direction %d, LUN 0x%016Lx on port "
			"0x%016Lx closed\n",
			req->qtcb->bottom.io.data_direction,
			(unsigned long long)zfcp_scsi_dev_lun(sdev),
			(unsigned long long)zfcp_sdev->port->wwpn);
		zfcp_erp_adapter_shutdown(req->adapter, 0, "fssfch3");
		req->status |= ZFCP_STATUS_FSFREQ_ERROR;
		break;
	case FSF_CMND_LENGTH_NOT_VALID:
		dev_err(&req->adapter->ccw_device->dev,
			"Incorrect FCP_CMND length %d, FCP device closed\n",
			req->qtcb->bottom.io.fcp_cmnd_length);
		zfcp_erp_adapter_shutdown(req->adapter, 0, "fssfch4");
		req->status |= ZFCP_STATUS_FSFREQ_ERROR;
		break;
	case FSF_PORT_BOXED:
		zfcp_erp_set_port_status(zfcp_sdev->port,
					 ZFCP_STATUS_COMMON_ACCESS_BOXED);
		zfcp_erp_port_reopen(zfcp_sdev->port,
				     ZFCP_STATUS_COMMON_ERP_FAILED, "fssfch5");
		req->status |= ZFCP_STATUS_FSFREQ_ERROR;
		break;
	case FSF_LUN_BOXED:
		zfcp_erp_set_lun_status(sdev, ZFCP_STATUS_COMMON_ACCESS_BOXED);
		zfcp_erp_lun_reopen(sdev, ZFCP_STATUS_COMMON_ERP_FAILED,
				    "fssfch6");
		req->status |= ZFCP_STATUS_FSFREQ_ERROR;
		break;
	case FSF_ADAPTER_STATUS_AVAILABLE:
		if (header->fsf_status_qual.word[0] ==
		    FSF_SQ_INVOKE_LINK_TEST_PROCEDURE)
			zfcp_fc_test_link(zfcp_sdev->port);
		req->status |= ZFCP_STATUS_FSFREQ_ERROR;
		break;
	case FSF_SECURITY_ERROR:
		zfcp_fsf_log_security_error(&req->adapter->ccw_device->dev,
					    header->fsf_status_qual.word[0],
					    zfcp_sdev->port->wwpn);
		zfcp_erp_port_forced_reopen(zfcp_sdev->port, 0, "fssfch7");
		req->status |= ZFCP_STATUS_FSFREQ_ERROR;
		break;
	}
}

static void zfcp_fsf_fcp_cmnd_handler(struct zfcp_fsf_req *req)
{
	struct scsi_cmnd *scpnt;
	struct fcp_resp_with_ext *fcp_rsp;
	unsigned long flags;

	read_lock_irqsave(&req->adapter->abort_lock, flags);

	scpnt = req->data;
	if (unlikely(!scpnt)) {
		read_unlock_irqrestore(&req->adapter->abort_lock, flags);
		return;
	}

	zfcp_fsf_fcp_handler_common(req, scpnt->device);

	if (unlikely(req->status & ZFCP_STATUS_FSFREQ_ERROR)) {
		set_host_byte(scpnt, DID_TRANSPORT_DISRUPTED);
		goto skip_fsfstatus;
	}

	switch (req->qtcb->header.fsf_status) {
	case FSF_INCONSISTENT_PROT_DATA:
	case FSF_INVALID_PROT_PARM:
		set_host_byte(scpnt, DID_ERROR);
		goto skip_fsfstatus;
	case FSF_BLOCK_GUARD_CHECK_FAILURE:
		zfcp_scsi_dif_sense_error(scpnt, 0x1);
		goto skip_fsfstatus;
	case FSF_APP_TAG_CHECK_FAILURE:
		zfcp_scsi_dif_sense_error(scpnt, 0x2);
		goto skip_fsfstatus;
	case FSF_REF_TAG_CHECK_FAILURE:
		zfcp_scsi_dif_sense_error(scpnt, 0x3);
		goto skip_fsfstatus;
	}
	BUILD_BUG_ON(sizeof(struct fcp_resp_with_ext) > FSF_FCP_RSP_SIZE);
	fcp_rsp = &req->qtcb->bottom.io.fcp_rsp.iu;
	zfcp_fc_eval_fcp_rsp(fcp_rsp, scpnt);

skip_fsfstatus:
	zfcp_fsf_req_trace(req, scpnt);
	zfcp_dbf_scsi_result(scpnt, req);

	scpnt->host_scribble = NULL;
	(scpnt->scsi_done) (scpnt);
	/*
	 * We must hold this lock until scsi_done has been called.
	 * Otherwise we may call scsi_done after abort regarding this
	 * command has completed.
	 * Note: scsi_done must not block!
	 */
	read_unlock_irqrestore(&req->adapter->abort_lock, flags);
}

static int zfcp_fsf_set_data_dir(struct scsi_cmnd *scsi_cmnd, u32 *data_dir)
{
	switch (scsi_get_prot_op(scsi_cmnd)) {
	case SCSI_PROT_NORMAL:
		switch (scsi_cmnd->sc_data_direction) {
		case DMA_NONE:
			*data_dir = FSF_DATADIR_CMND;
			break;
		case DMA_FROM_DEVICE:
			*data_dir = FSF_DATADIR_READ;
			break;
		case DMA_TO_DEVICE:
			*data_dir = FSF_DATADIR_WRITE;
			break;
		case DMA_BIDIRECTIONAL:
			return -EINVAL;
		}
		break;

	case SCSI_PROT_READ_STRIP:
		*data_dir = FSF_DATADIR_DIF_READ_STRIP;
		break;
	case SCSI_PROT_WRITE_INSERT:
		*data_dir = FSF_DATADIR_DIF_WRITE_INSERT;
		break;
	case SCSI_PROT_READ_PASS:
		*data_dir = FSF_DATADIR_DIF_READ_CONVERT;
		break;
	case SCSI_PROT_WRITE_PASS:
		*data_dir = FSF_DATADIR_DIF_WRITE_CONVERT;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * zfcp_fsf_fcp_cmnd - initiate an FCP command (for a SCSI command)
 * @scsi_cmnd: scsi command to be sent
 */
int zfcp_fsf_fcp_cmnd(struct scsi_cmnd *scsi_cmnd)
{
	struct zfcp_fsf_req *req;
	struct fcp_cmnd *fcp_cmnd;
	u8 sbtype = SBAL_SFLAGS0_TYPE_READ;
	int retval = -EIO;
	struct scsi_device *sdev = scsi_cmnd->device;
	struct zfcp_scsi_dev *zfcp_sdev = sdev_to_zfcp(sdev);
	struct zfcp_adapter *adapter = zfcp_sdev->port->adapter;
	struct zfcp_qdio *qdio = adapter->qdio;
	struct fsf_qtcb_bottom_io *io;
	unsigned long flags;

	if (unlikely(!(atomic_read(&zfcp_sdev->status) &
		       ZFCP_STATUS_COMMON_UNBLOCKED)))
		return -EBUSY;

	spin_lock_irqsave(&qdio->req_q_lock, flags);
	if (atomic_read(&qdio->req_q_free) <= 0) {
		atomic_inc(&qdio->req_q_full);
		goto out;
	}

	if (scsi_cmnd->sc_data_direction == DMA_TO_DEVICE)
		sbtype = SBAL_SFLAGS0_TYPE_WRITE;

	req = zfcp_fsf_req_create(qdio, FSF_QTCB_FCP_CMND,
				  sbtype, adapter->pool.scsi_req);

	if (IS_ERR(req)) {
		retval = PTR_ERR(req);
		goto out;
	}

	scsi_cmnd->host_scribble = (unsigned char *) req->req_id;

	io = &req->qtcb->bottom.io;
	req->status |= ZFCP_STATUS_FSFREQ_CLEANUP;
	req->data = scsi_cmnd;
	req->handler = zfcp_fsf_fcp_cmnd_handler;
	req->qtcb->header.lun_handle = zfcp_sdev->lun_handle;
	req->qtcb->header.port_handle = zfcp_sdev->port->handle;
	io->service_class = FSF_CLASS_3;
	io->fcp_cmnd_length = FCP_CMND_LEN;

	if (scsi_get_prot_op(scsi_cmnd) != SCSI_PROT_NORMAL) {
		io->data_block_length = scsi_cmnd->device->sector_size;
		io->ref_tag_value = scsi_get_lba(scsi_cmnd) & 0xFFFFFFFF;
	}

	if (zfcp_fsf_set_data_dir(scsi_cmnd, &io->data_direction))
		goto failed_scsi_cmnd;

	BUILD_BUG_ON(sizeof(struct fcp_cmnd) > FSF_FCP_CMND_SIZE);
	fcp_cmnd = &req->qtcb->bottom.io.fcp_cmnd.iu;
	zfcp_fc_scsi_to_fcp(fcp_cmnd, scsi_cmnd);

	if ((scsi_get_prot_op(scsi_cmnd) != SCSI_PROT_NORMAL) &&
	    scsi_prot_sg_count(scsi_cmnd)) {
		zfcp_qdio_set_data_div(qdio, &req->qdio_req,
				       scsi_prot_sg_count(scsi_cmnd));
		retval = zfcp_qdio_sbals_from_sg(qdio, &req->qdio_req,
						 scsi_prot_sglist(scsi_cmnd));
		if (retval)
			goto failed_scsi_cmnd;
		io->prot_data_length = zfcp_qdio_real_bytes(
						scsi_prot_sglist(scsi_cmnd));
	}

	retval = zfcp_qdio_sbals_from_sg(qdio, &req->qdio_req,
					 scsi_sglist(scsi_cmnd));
	if (unlikely(retval))
		goto failed_scsi_cmnd;

	zfcp_qdio_set_sbale_last(adapter->qdio, &req->qdio_req);
	if (zfcp_adapter_multi_buffer_active(adapter))
		zfcp_qdio_set_scount(qdio, &req->qdio_req);

	retval = zfcp_fsf_req_send(req);
	if (unlikely(retval))
		goto failed_scsi_cmnd;
	/* NOTE: DO NOT TOUCH req PAST THIS POINT! */

	goto out;

failed_scsi_cmnd:
	zfcp_fsf_req_free(req);
	scsi_cmnd->host_scribble = NULL;
out:
	spin_unlock_irqrestore(&qdio->req_q_lock, flags);
	return retval;
}

static void zfcp_fsf_fcp_task_mgmt_handler(struct zfcp_fsf_req *req)
{
	struct scsi_device *sdev = req->data;
	struct fcp_resp_with_ext *fcp_rsp;
	struct fcp_resp_rsp_info *rsp_info;

	zfcp_fsf_fcp_handler_common(req, sdev);

	fcp_rsp = &req->qtcb->bottom.io.fcp_rsp.iu;
	rsp_info = (struct fcp_resp_rsp_info *) &fcp_rsp[1];

	if ((rsp_info->rsp_code != FCP_TMF_CMPL) ||
	     (req->status & ZFCP_STATUS_FSFREQ_ERROR))
		req->status |= ZFCP_STATUS_FSFREQ_TMFUNCFAILED;
}

/**
 * zfcp_fsf_fcp_task_mgmt() - Send SCSI task management command (TMF).
 * @sdev: Pointer to SCSI device to send the task management command to.
 * @tm_flags: Unsigned byte for task management flags.
 *
 * Return: On success pointer to struct zfcp_fsf_req, %NULL otherwise.
 */
struct zfcp_fsf_req *zfcp_fsf_fcp_task_mgmt(struct scsi_device *sdev,
					    u8 tm_flags)
{
	struct zfcp_fsf_req *req = NULL;
	struct fcp_cmnd *fcp_cmnd;
	struct zfcp_scsi_dev *zfcp_sdev = sdev_to_zfcp(sdev);
	struct zfcp_qdio *qdio = zfcp_sdev->port->adapter->qdio;

	if (unlikely(!(atomic_read(&zfcp_sdev->status) &
		       ZFCP_STATUS_COMMON_UNBLOCKED)))
		return NULL;

	spin_lock_irq(&qdio->req_q_lock);
	if (zfcp_qdio_sbal_get(qdio))
		goto out;

	req = zfcp_fsf_req_create(qdio, FSF_QTCB_FCP_CMND,
				  SBAL_SFLAGS0_TYPE_WRITE,
				  qdio->adapter->pool.scsi_req);

	if (IS_ERR(req)) {
		req = NULL;
		goto out;
	}

	req->data = sdev;

	req->handler = zfcp_fsf_fcp_task_mgmt_handler;
	req->qtcb->header.lun_handle = zfcp_sdev->lun_handle;
	req->qtcb->header.port_handle = zfcp_sdev->port->handle;
	req->qtcb->bottom.io.data_direction = FSF_DATADIR_CMND;
	req->qtcb->bottom.io.service_class = FSF_CLASS_3;
	req->qtcb->bottom.io.fcp_cmnd_length = FCP_CMND_LEN;

	zfcp_qdio_set_sbale_last(qdio, &req->qdio_req);

	fcp_cmnd = &req->qtcb->bottom.io.fcp_cmnd.iu;
	zfcp_fc_fcp_tm(fcp_cmnd, sdev, tm_flags);

	zfcp_fsf_start_timer(req, ZFCP_FSF_SCSI_ER_TIMEOUT);
	if (!zfcp_fsf_req_send(req)) {
		/* NOTE: DO NOT TOUCH req, UNTIL IT COMPLETES! */
		goto out;
	}

	zfcp_fsf_req_free(req);
	req = NULL;
out:
	spin_unlock_irq(&qdio->req_q_lock);
	return req;
}

/**
 * zfcp_fsf_reqid_check - validate req_id contained in SBAL returned by QDIO
 * @qdio: pointer to struct zfcp_qdio
 * @sbal_idx: response queue index of SBAL to be processed
 */
void zfcp_fsf_reqid_check(struct zfcp_qdio *qdio, int sbal_idx)
{
	struct zfcp_adapter *adapter = qdio->adapter;
	struct qdio_buffer *sbal = qdio->res_q[sbal_idx];
	struct qdio_buffer_element *sbale;
	struct zfcp_fsf_req *fsf_req;
	unsigned long req_id;
	int idx;

	for (idx = 0; idx < QDIO_MAX_ELEMENTS_PER_BUFFER; idx++) {

		sbale = &sbal->element[idx];
		req_id = sbale->addr;
		fsf_req = zfcp_reqlist_find_rm(adapter->req_list, req_id);

		if (!fsf_req) {
			/*
			 * Unknown request means that we have potentially memory
			 * corruption and must stop the machine immediately.
			 */
			zfcp_qdio_siosl(adapter);
			panic("error: unknown req_id (%lx) on adapter %s.\n",
			      req_id, dev_name(&adapter->ccw_device->dev));
		}

		zfcp_fsf_req_complete(fsf_req);

		if (likely(sbale->eflags & SBAL_EFLAGS_LAST_ENTRY))
			break;
	}
}
