// SPDX-License-Identifier: GPL-2.0
/*
 * zfcp device driver
 *
 * Setup and helper functions to access QDIO.
 *
 * Copyright IBM Corp. 2002, 2020
 */

#define KMSG_COMPONENT "zfcp"
#define pr_fmt(fmt) KMSG_COMPONENT ": " fmt

#include <linux/lockdep.h>
#include <linux/slab.h>
#include <linux/module.h>
#include "zfcp_ext.h"
#include "zfcp_qdio.h"

static bool enable_multibuffer = true;
module_param_named(datarouter, enable_multibuffer, bool, 0400);
MODULE_PARM_DESC(datarouter, "Enable hardware data router support (default on)");

#define ZFCP_QDIO_REQUEST_RESCAN_MSECS	(MSEC_PER_SEC * 10)
#define ZFCP_QDIO_REQUEST_SCAN_MSECS	MSEC_PER_SEC

static void zfcp_qdio_handler_error(struct zfcp_qdio *qdio, char *dbftag,
				    unsigned int qdio_err)
{
	struct zfcp_adapter *adapter = qdio->adapter;

	dev_warn(&adapter->ccw_device->dev, "A QDIO problem occurred\n");

	if (qdio_err & QDIO_ERROR_SLSB_STATE) {
		zfcp_qdio_siosl(adapter);
		zfcp_erp_adapter_shutdown(adapter, 0, dbftag);
		return;
	}
	zfcp_erp_adapter_reopen(adapter,
				ZFCP_STATUS_ADAPTER_LINK_UNPLUGGED |
				ZFCP_STATUS_COMMON_ERP_FAILED, dbftag);
}

static void zfcp_qdio_zero_sbals(struct qdio_buffer *sbal[], int first, int cnt)
{
	int i, sbal_idx;

	for (i = first; i < first + cnt; i++) {
		sbal_idx = i % QDIO_MAX_BUFFERS_PER_Q;
		memset(sbal[sbal_idx], 0, sizeof(struct qdio_buffer));
	}
}

/* this needs to be called prior to updating the queue fill level */
static inline void zfcp_qdio_account(struct zfcp_qdio *qdio)
{
	unsigned long long now, span;
	int used;

	now = get_tod_clock_monotonic();
	span = (now - qdio->req_q_time) >> 12;
	used = QDIO_MAX_BUFFERS_PER_Q - atomic_read(&qdio->req_q_free);
	qdio->req_q_util += used * span;
	qdio->req_q_time = now;
}

static void zfcp_qdio_int_req(struct ccw_device *cdev, unsigned int qdio_err,
			      int queue_no, int idx, int count,
			      unsigned long parm)
{
	struct zfcp_qdio *qdio = (struct zfcp_qdio *) parm;

	if (unlikely(qdio_err)) {
		zfcp_qdio_handler_error(qdio, "qdireq1", qdio_err);
		return;
	}
}

static void zfcp_qdio_request_tasklet(struct tasklet_struct *tasklet)
{
	struct zfcp_qdio *qdio = from_tasklet(qdio, tasklet, request_tasklet);
	struct ccw_device *cdev = qdio->adapter->ccw_device;
	unsigned int start, error;
	int completed;

	completed = qdio_inspect_queue(cdev, 0, false, &start, &error);
	if (completed > 0) {
		if (error) {
			zfcp_qdio_handler_error(qdio, "qdreqt1", error);
		} else {
			/* cleanup all SBALs being program-owned now */
			zfcp_qdio_zero_sbals(qdio->req_q, start, completed);

			spin_lock_irq(&qdio->stat_lock);
			zfcp_qdio_account(qdio);
			spin_unlock_irq(&qdio->stat_lock);
			atomic_add(completed, &qdio->req_q_free);
			wake_up(&qdio->req_q_wq);
		}
	}

	if (atomic_read(&qdio->req_q_free) < QDIO_MAX_BUFFERS_PER_Q)
		timer_reduce(&qdio->request_timer,
			     jiffies + msecs_to_jiffies(ZFCP_QDIO_REQUEST_RESCAN_MSECS));
}

static void zfcp_qdio_request_timer(struct timer_list *timer)
{
	struct zfcp_qdio *qdio = from_timer(qdio, timer, request_timer);

	tasklet_schedule(&qdio->request_tasklet);
}

static void zfcp_qdio_int_resp(struct ccw_device *cdev, unsigned int qdio_err,
			       int queue_no, int idx, int count,
			       unsigned long parm)
{
	struct zfcp_qdio *qdio = (struct zfcp_qdio *) parm;
	struct zfcp_adapter *adapter = qdio->adapter;
	int sbal_no, sbal_idx;

	if (unlikely(qdio_err)) {
		if (zfcp_adapter_multi_buffer_active(adapter)) {
			void *pl[ZFCP_QDIO_MAX_SBALS_PER_REQ + 1];
			struct qdio_buffer_element *sbale;
			u64 req_id;
			u8 scount;

			memset(pl, 0,
			       ZFCP_QDIO_MAX_SBALS_PER_REQ * sizeof(void *));
			sbale = qdio->res_q[idx]->element;
			req_id = sbale->addr;
			scount = min(sbale->scount + 1,
				     ZFCP_QDIO_MAX_SBALS_PER_REQ + 1);
				     /* incl. signaling SBAL */

			for (sbal_no = 0; sbal_no < scount; sbal_no++) {
				sbal_idx = (idx + sbal_no) %
					QDIO_MAX_BUFFERS_PER_Q;
				pl[sbal_no] = qdio->res_q[sbal_idx];
			}
			zfcp_dbf_hba_def_err(adapter, req_id, scount, pl);
		}
		zfcp_qdio_handler_error(qdio, "qdires1", qdio_err);
		return;
	}

	/*
	 * go through all SBALs from input queue currently
	 * returned by QDIO layer
	 */
	for (sbal_no = 0; sbal_no < count; sbal_no++) {
		sbal_idx = (idx + sbal_no) % QDIO_MAX_BUFFERS_PER_Q;
		/* go through all SBALEs of SBAL */
		zfcp_fsf_reqid_check(qdio, sbal_idx);
	}

	/*
	 * put SBALs back to response queue
	 */
	if (do_QDIO(cdev, QDIO_FLAG_SYNC_INPUT, 0, idx, count, NULL))
		zfcp_erp_adapter_reopen(qdio->adapter, 0, "qdires2");
}

static void zfcp_qdio_irq_tasklet(struct tasklet_struct *tasklet)
{
	struct zfcp_qdio *qdio = from_tasklet(qdio, tasklet, irq_tasklet);
	struct ccw_device *cdev = qdio->adapter->ccw_device;
	unsigned int start, error;
	int completed;

	if (atomic_read(&qdio->req_q_free) < QDIO_MAX_BUFFERS_PER_Q)
		tasklet_schedule(&qdio->request_tasklet);

	/* Check the Response Queue: */
	completed = qdio_inspect_queue(cdev, 0, true, &start, &error);
	if (completed < 0)
		return;
	if (completed > 0)
		zfcp_qdio_int_resp(cdev, error, 0, start, completed,
				   (unsigned long) qdio);

	if (qdio_start_irq(cdev))
		/* More work pending: */
		tasklet_schedule(&qdio->irq_tasklet);
}

static void zfcp_qdio_poll(struct ccw_device *cdev, unsigned long data)
{
	struct zfcp_qdio *qdio = (struct zfcp_qdio *) data;

	tasklet_schedule(&qdio->irq_tasklet);
}

static struct qdio_buffer_element *
zfcp_qdio_sbal_chain(struct zfcp_qdio *qdio, struct zfcp_qdio_req *q_req)
{
	struct qdio_buffer_element *sbale;

	/* set last entry flag in current SBALE of current SBAL */
	sbale = zfcp_qdio_sbale_curr(qdio, q_req);
	sbale->eflags |= SBAL_EFLAGS_LAST_ENTRY;

	/* don't exceed last allowed SBAL */
	if (q_req->sbal_last == q_req->sbal_limit)
		return NULL;

	/* set chaining flag in first SBALE of current SBAL */
	sbale = zfcp_qdio_sbale_req(qdio, q_req);
	sbale->sflags |= SBAL_SFLAGS0_MORE_SBALS;

	/* calculate index of next SBAL */
	q_req->sbal_last++;
	q_req->sbal_last %= QDIO_MAX_BUFFERS_PER_Q;

	/* keep this requests number of SBALs up-to-date */
	q_req->sbal_number++;
	BUG_ON(q_req->sbal_number > ZFCP_QDIO_MAX_SBALS_PER_REQ);

	/* start at first SBALE of new SBAL */
	q_req->sbale_curr = 0;

	/* set storage-block type for new SBAL */
	sbale = zfcp_qdio_sbale_curr(qdio, q_req);
	sbale->sflags |= q_req->sbtype;

	return sbale;
}

static struct qdio_buffer_element *
zfcp_qdio_sbale_next(struct zfcp_qdio *qdio, struct zfcp_qdio_req *q_req)
{
	if (q_req->sbale_curr == qdio->max_sbale_per_sbal - 1)
		return zfcp_qdio_sbal_chain(qdio, q_req);
	q_req->sbale_curr++;
	return zfcp_qdio_sbale_curr(qdio, q_req);
}

/**
 * zfcp_qdio_sbals_from_sg - fill SBALs from scatter-gather list
 * @qdio: pointer to struct zfcp_qdio
 * @q_req: pointer to struct zfcp_qdio_req
 * @sg: scatter-gather list
 * Returns: zero or -EINVAL on error
 */
int zfcp_qdio_sbals_from_sg(struct zfcp_qdio *qdio, struct zfcp_qdio_req *q_req,
			    struct scatterlist *sg)
{
	struct qdio_buffer_element *sbale;

	/* set storage-block type for this request */
	sbale = zfcp_qdio_sbale_req(qdio, q_req);
	sbale->sflags |= q_req->sbtype;

	for (; sg; sg = sg_next(sg)) {
		sbale = zfcp_qdio_sbale_next(qdio, q_req);
		if (!sbale) {
			atomic_inc(&qdio->req_q_full);
			zfcp_qdio_zero_sbals(qdio->req_q, q_req->sbal_first,
					     q_req->sbal_number);
			return -EINVAL;
		}
		sbale->addr = sg_phys(sg);
		sbale->length = sg->length;
	}
	return 0;
}

static int zfcp_qdio_sbal_check(struct zfcp_qdio *qdio)
{
	if (atomic_read(&qdio->req_q_free) ||
	    !(atomic_read(&qdio->adapter->status) & ZFCP_STATUS_ADAPTER_QDIOUP))
		return 1;
	return 0;
}

/**
 * zfcp_qdio_sbal_get - get free sbal in request queue, wait if necessary
 * @qdio: pointer to struct zfcp_qdio
 *
 * The req_q_lock must be held by the caller of this function, and
 * this function may only be called from process context; it will
 * sleep when waiting for a free sbal.
 *
 * Returns: 0 on success, -EIO if there is no free sbal after waiting.
 */
int zfcp_qdio_sbal_get(struct zfcp_qdio *qdio)
{
	long ret;

	ret = wait_event_interruptible_lock_irq_timeout(qdio->req_q_wq,
		       zfcp_qdio_sbal_check(qdio), qdio->req_q_lock, 5 * HZ);

	if (!(atomic_read(&qdio->adapter->status) & ZFCP_STATUS_ADAPTER_QDIOUP))
		return -EIO;

	if (ret > 0)
		return 0;

	if (!ret) {
		atomic_inc(&qdio->req_q_full);
		/* assume hanging outbound queue, try queue recovery */
		zfcp_erp_adapter_reopen(qdio->adapter, 0, "qdsbg_1");
	}

	return -EIO;
}

/**
 * zfcp_qdio_send - send req to QDIO
 * @qdio: pointer to struct zfcp_qdio
 * @q_req: pointer to struct zfcp_qdio_req
 * Returns: 0 on success, error otherwise
 */
int zfcp_qdio_send(struct zfcp_qdio *qdio, struct zfcp_qdio_req *q_req)
{
	int retval;
	u8 sbal_number = q_req->sbal_number;

	/*
	 * This should actually be a spin_lock_bh(stat_lock), to protect against
	 * Request Queue completion processing in tasklet context.
	 * But we can't do so (and are safe), as we always get called with IRQs
	 * disabled by spin_lock_irq[save](req_q_lock).
	 */
	lockdep_assert_irqs_disabled();
	spin_lock(&qdio->stat_lock);
	zfcp_qdio_account(qdio);
	spin_unlock(&qdio->stat_lock);

	atomic_sub(sbal_number, &qdio->req_q_free);

	retval = do_QDIO(qdio->adapter->ccw_device, QDIO_FLAG_SYNC_OUTPUT, 0,
			 q_req->sbal_first, sbal_number, NULL);

	if (unlikely(retval)) {
		/* Failed to submit the IO, roll back our modifications. */
		atomic_add(sbal_number, &qdio->req_q_free);
		zfcp_qdio_zero_sbals(qdio->req_q, q_req->sbal_first,
				     sbal_number);
		return retval;
	}

	if (atomic_read(&qdio->req_q_free) <= 2 * ZFCP_QDIO_MAX_SBALS_PER_REQ)
		tasklet_schedule(&qdio->request_tasklet);
	else
		timer_reduce(&qdio->request_timer,
			     jiffies + msecs_to_jiffies(ZFCP_QDIO_REQUEST_SCAN_MSECS));

	/* account for transferred buffers */
	qdio->req_q_idx += sbal_number;
	qdio->req_q_idx %= QDIO_MAX_BUFFERS_PER_Q;

	return 0;
}

/**
 * zfcp_qdio_allocate - allocate queue memory and initialize QDIO data
 * @qdio: pointer to struct zfcp_qdio
 * Returns: -ENOMEM on memory allocation error or return value from
 *          qdio_allocate
 */
static int zfcp_qdio_allocate(struct zfcp_qdio *qdio)
{
	int ret;

	ret = qdio_alloc_buffers(qdio->req_q, QDIO_MAX_BUFFERS_PER_Q);
	if (ret)
		return -ENOMEM;

	ret = qdio_alloc_buffers(qdio->res_q, QDIO_MAX_BUFFERS_PER_Q);
	if (ret)
		goto free_req_q;

	init_waitqueue_head(&qdio->req_q_wq);

	ret = qdio_allocate(qdio->adapter->ccw_device, 1, 1);
	if (ret)
		goto free_res_q;

	return 0;

free_res_q:
	qdio_free_buffers(qdio->res_q, QDIO_MAX_BUFFERS_PER_Q);
free_req_q:
	qdio_free_buffers(qdio->req_q, QDIO_MAX_BUFFERS_PER_Q);
	return ret;
}

/**
 * zfcp_close_qdio - close qdio queues for an adapter
 * @qdio: pointer to structure zfcp_qdio
 */
void zfcp_qdio_close(struct zfcp_qdio *qdio)
{
	struct zfcp_adapter *adapter = qdio->adapter;
	int idx, count;

	if (!(atomic_read(&adapter->status) & ZFCP_STATUS_ADAPTER_QDIOUP))
		return;

	/* clear QDIOUP flag, thus do_QDIO is not called during qdio_shutdown */
	spin_lock_irq(&qdio->req_q_lock);
	atomic_andnot(ZFCP_STATUS_ADAPTER_QDIOUP, &adapter->status);
	spin_unlock_irq(&qdio->req_q_lock);

	wake_up(&qdio->req_q_wq);

	tasklet_disable(&qdio->irq_tasklet);
	tasklet_disable(&qdio->request_tasklet);
	del_timer_sync(&qdio->request_timer);
	qdio_stop_irq(adapter->ccw_device);
	qdio_shutdown(adapter->ccw_device, QDIO_FLAG_CLEANUP_USING_CLEAR);

	/* cleanup used outbound sbals */
	count = atomic_read(&qdio->req_q_free);
	if (count < QDIO_MAX_BUFFERS_PER_Q) {
		idx = (qdio->req_q_idx + count) % QDIO_MAX_BUFFERS_PER_Q;
		count = QDIO_MAX_BUFFERS_PER_Q - count;
		zfcp_qdio_zero_sbals(qdio->req_q, idx, count);
	}
	qdio->req_q_idx = 0;
	atomic_set(&qdio->req_q_free, 0);
}

void zfcp_qdio_shost_update(struct zfcp_adapter *const adapter,
			    const struct zfcp_qdio *const qdio)
{
	struct Scsi_Host *const shost = adapter->scsi_host;

	if (shost == NULL)
		return;

	shost->sg_tablesize = qdio->max_sbale_per_req;
	shost->max_sectors = qdio->max_sbale_per_req * 8;
}

/**
 * zfcp_qdio_open - prepare and initialize response queue
 * @qdio: pointer to struct zfcp_qdio
 * Returns: 0 on success, otherwise -EIO
 */
int zfcp_qdio_open(struct zfcp_qdio *qdio)
{
	struct qdio_buffer **input_sbals[1] = {qdio->res_q};
	struct qdio_buffer **output_sbals[1] = {qdio->req_q};
	struct qdio_buffer_element *sbale;
	struct qdio_initialize init_data = {0};
	struct zfcp_adapter *adapter = qdio->adapter;
	struct ccw_device *cdev = adapter->ccw_device;
	struct qdio_ssqd_desc ssqd;
	int cc;

	if (atomic_read(&adapter->status) & ZFCP_STATUS_ADAPTER_QDIOUP)
		return -EIO;

	atomic_andnot(ZFCP_STATUS_ADAPTER_SIOSL_ISSUED,
			  &qdio->adapter->status);

	init_data.q_format = QDIO_ZFCP_QFMT;
	init_data.qib_rflags = QIB_RFLAGS_ENABLE_DATA_DIV;
	if (enable_multibuffer)
		init_data.qdr_ac |= QDR_AC_MULTI_BUFFER_ENABLE;
	init_data.no_input_qs = 1;
	init_data.no_output_qs = 1;
	init_data.input_handler = zfcp_qdio_int_resp;
	init_data.output_handler = zfcp_qdio_int_req;
	init_data.irq_poll = zfcp_qdio_poll;
	init_data.int_parm = (unsigned long) qdio;
	init_data.input_sbal_addr_array = input_sbals;
	init_data.output_sbal_addr_array = output_sbals;

	if (qdio_establish(cdev, &init_data))
		goto failed_establish;

	if (qdio_get_ssqd_desc(cdev, &ssqd))
		goto failed_qdio;

	if (ssqd.qdioac2 & CHSC_AC2_DATA_DIV_ENABLED)
		atomic_or(ZFCP_STATUS_ADAPTER_DATA_DIV_ENABLED,
				&qdio->adapter->status);

	if (ssqd.qdioac2 & CHSC_AC2_MULTI_BUFFER_ENABLED) {
		atomic_or(ZFCP_STATUS_ADAPTER_MB_ACT, &adapter->status);
		qdio->max_sbale_per_sbal = QDIO_MAX_ELEMENTS_PER_BUFFER;
	} else {
		atomic_andnot(ZFCP_STATUS_ADAPTER_MB_ACT, &adapter->status);
		qdio->max_sbale_per_sbal = QDIO_MAX_ELEMENTS_PER_BUFFER - 1;
	}

	qdio->max_sbale_per_req =
		ZFCP_QDIO_MAX_SBALS_PER_REQ * qdio->max_sbale_per_sbal
		- 2;
	if (qdio_activate(cdev))
		goto failed_qdio;

	for (cc = 0; cc < QDIO_MAX_BUFFERS_PER_Q; cc++) {
		sbale = &(qdio->res_q[cc]->element[0]);
		sbale->length = 0;
		sbale->eflags = SBAL_EFLAGS_LAST_ENTRY;
		sbale->sflags = 0;
		sbale->addr = 0;
	}

	if (do_QDIO(cdev, QDIO_FLAG_SYNC_INPUT, 0, 0, QDIO_MAX_BUFFERS_PER_Q,
		    NULL))
		goto failed_qdio;

	/* set index of first available SBALS / number of available SBALS */
	qdio->req_q_idx = 0;
	atomic_set(&qdio->req_q_free, QDIO_MAX_BUFFERS_PER_Q);
	atomic_or(ZFCP_STATUS_ADAPTER_QDIOUP, &qdio->adapter->status);

	/* Enable processing for Request Queue completions: */
	tasklet_enable(&qdio->request_tasklet);
	/* Enable processing for QDIO interrupts: */
	tasklet_enable(&qdio->irq_tasklet);
	/* This results in a qdio_start_irq(): */
	tasklet_schedule(&qdio->irq_tasklet);

	zfcp_qdio_shost_update(adapter, qdio);

	return 0;

failed_qdio:
	qdio_shutdown(cdev, QDIO_FLAG_CLEANUP_USING_CLEAR);
failed_establish:
	dev_err(&cdev->dev,
		"Setting up the QDIO connection to the FCP adapter failed\n");
	return -EIO;
}

void zfcp_qdio_destroy(struct zfcp_qdio *qdio)
{
	if (!qdio)
		return;

	tasklet_kill(&qdio->irq_tasklet);
	tasklet_kill(&qdio->request_tasklet);

	if (qdio->adapter->ccw_device)
		qdio_free(qdio->adapter->ccw_device);

	qdio_free_buffers(qdio->req_q, QDIO_MAX_BUFFERS_PER_Q);
	qdio_free_buffers(qdio->res_q, QDIO_MAX_BUFFERS_PER_Q);
	kfree(qdio);
}

int zfcp_qdio_setup(struct zfcp_adapter *adapter)
{
	struct zfcp_qdio *qdio;

	qdio = kzalloc(sizeof(struct zfcp_qdio), GFP_KERNEL);
	if (!qdio)
		return -ENOMEM;

	qdio->adapter = adapter;

	if (zfcp_qdio_allocate(qdio)) {
		kfree(qdio);
		return -ENOMEM;
	}

	spin_lock_init(&qdio->req_q_lock);
	spin_lock_init(&qdio->stat_lock);
	timer_setup(&qdio->request_timer, zfcp_qdio_request_timer, 0);
	tasklet_setup(&qdio->irq_tasklet, zfcp_qdio_irq_tasklet);
	tasklet_setup(&qdio->request_tasklet, zfcp_qdio_request_tasklet);
	tasklet_disable(&qdio->irq_tasklet);
	tasklet_disable(&qdio->request_tasklet);

	adapter->qdio = qdio;
	return 0;
}

/**
 * zfcp_qdio_siosl - Trigger logging in FCP channel
 * @adapter: The zfcp_adapter where to trigger logging
 *
 * Call the cio siosl function to trigger hardware logging.  This
 * wrapper function sets a flag to ensure hardware logging is only
 * triggered once before going through qdio shutdown.
 *
 * The triggers are always run from qdio tasklet context, so no
 * additional synchronization is necessary.
 */
void zfcp_qdio_siosl(struct zfcp_adapter *adapter)
{
	int rc;

	if (atomic_read(&adapter->status) & ZFCP_STATUS_ADAPTER_SIOSL_ISSUED)
		return;

	rc = ccw_device_siosl(adapter->ccw_device);
	if (!rc)
		atomic_or(ZFCP_STATUS_ADAPTER_SIOSL_ISSUED,
				&adapter->status);
}
