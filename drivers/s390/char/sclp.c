// SPDX-License-Identifier: GPL-2.0
/*
 * core function to access sclp interface
 *
 * Copyright IBM Corp. 1999, 2009
 *
 * Author(s): Martin Peschke <mpeschke@de.ibm.com>
 *	      Martin Schwidefsky <schwidefsky@de.ibm.com>
 */

#include <linux/kernel_stat.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/timer.h>
#include <linux/reboot.h>
#include <linux/jiffies.h>
#include <linux/init.h>
#include <linux/suspend.h>
#include <linux/completion.h>
#include <linux/platform_device.h>
#include <asm/types.h>
#include <asm/irq.h>

#include "sclp.h"

#define SCLP_HEADER		"sclp: "

/* Lock to protect internal data consistency. */
static DEFINE_SPINLOCK(sclp_lock);

/* Mask of events that we can send to the sclp interface. */
static sccb_mask_t sclp_receive_mask;

/* Mask of events that we can receive from the sclp interface. */
static sccb_mask_t sclp_send_mask;

/* List of registered event listeners and senders. */
static LIST_HEAD(sclp_reg_list);

/* List of queued requests. */
static LIST_HEAD(sclp_req_queue);

/* Data for read and and init requests. */
static struct sclp_req sclp_read_req;
static struct sclp_req sclp_init_req;
static void *sclp_read_sccb;
static struct init_sccb *sclp_init_sccb;

/* Suspend request */
static DECLARE_COMPLETION(sclp_request_queue_flushed);

/* Number of console pages to allocate, used by sclp_con.c and sclp_vt220.c */
int sclp_console_pages = SCLP_CONSOLE_PAGES;
/* Flag to indicate if buffer pages are dropped on buffer full condition */
int sclp_console_drop = 1;
/* Number of times the console dropped buffer pages */
unsigned long sclp_console_full;

static void sclp_suspend_req_cb(struct sclp_req *req, void *data)
{
	complete(&sclp_request_queue_flushed);
}

static int __init sclp_setup_console_pages(char *str)
{
	int pages, rc;

	rc = kstrtoint(str, 0, &pages);
	if (!rc && pages >= SCLP_CONSOLE_PAGES)
		sclp_console_pages = pages;
	return 1;
}

__setup("sclp_con_pages=", sclp_setup_console_pages);

static int __init sclp_setup_console_drop(char *str)
{
	int drop, rc;

	rc = kstrtoint(str, 0, &drop);
	if (!rc)
		sclp_console_drop = drop;
	return 1;
}

__setup("sclp_con_drop=", sclp_setup_console_drop);

static struct sclp_req sclp_suspend_req;

/* Timer for request retries. */
static struct timer_list sclp_request_timer;

/* Timer for queued requests. */
static struct timer_list sclp_queue_timer;

/* Internal state: is a request active at the sclp? */
static volatile enum sclp_running_state_t {
	sclp_running_state_idle,
	sclp_running_state_running,
	sclp_running_state_reset_pending
} sclp_running_state = sclp_running_state_idle;

/* Internal state: is a read request pending? */
static volatile enum sclp_reading_state_t {
	sclp_reading_state_idle,
	sclp_reading_state_reading
} sclp_reading_state = sclp_reading_state_idle;

/* Internal state: is the driver currently serving requests? */
static volatile enum sclp_activation_state_t {
	sclp_activation_state_active,
	sclp_activation_state_deactivating,
	sclp_activation_state_inactive,
	sclp_activation_state_activating
} sclp_activation_state = sclp_activation_state_active;

/* Internal state: is an init mask request pending? */
static volatile enum sclp_mask_state_t {
	sclp_mask_state_idle,
	sclp_mask_state_initializing
} sclp_mask_state = sclp_mask_state_idle;

/* Internal state: is the driver suspended? */
static enum sclp_suspend_state_t {
	sclp_suspend_state_running,
	sclp_suspend_state_suspended,
} sclp_suspend_state = sclp_suspend_state_running;

/* Maximum retry counts */
#define SCLP_INIT_RETRY		3
#define SCLP_MASK_RETRY		3

/* Timeout intervals in seconds.*/
#define SCLP_BUSY_INTERVAL	10
#define SCLP_RETRY_INTERVAL	30

static void sclp_request_timeout(bool force_restart);
static void sclp_process_queue(void);
static void __sclp_make_read_req(void);
static int sclp_init_mask(int calculate);
static int sclp_init(void);

static void
__sclp_queue_read_req(void)
{
	if (sclp_reading_state == sclp_reading_state_idle) {
		sclp_reading_state = sclp_reading_state_reading;
		__sclp_make_read_req();
		/* Add request to head of queue */
		list_add(&sclp_read_req.list, &sclp_req_queue);
	}
}

/* Set up request retry timer. Called while sclp_lock is locked. */
static inline void
__sclp_set_request_timer(unsigned long time, void (*cb)(struct timer_list *))
{
	del_timer(&sclp_request_timer);
	sclp_request_timer.function = cb;
	sclp_request_timer.expires = jiffies + time;
	add_timer(&sclp_request_timer);
}

static void sclp_request_timeout_restart(struct timer_list *unused)
{
	sclp_request_timeout(true);
}

static void sclp_request_timeout_normal(struct timer_list *unused)
{
	sclp_request_timeout(false);
}

/* Request timeout handler. Restart the request queue. If force_restart,
 * force restart of running request. */
static void sclp_request_timeout(bool force_restart)
{
	unsigned long flags;

	spin_lock_irqsave(&sclp_lock, flags);
	if (force_restart) {
		if (sclp_running_state == sclp_running_state_running) {
			/* Break running state and queue NOP read event request
			 * to get a defined interface state. */
			__sclp_queue_read_req();
			sclp_running_state = sclp_running_state_idle;
		}
	} else {
		__sclp_set_request_timer(SCLP_BUSY_INTERVAL * HZ,
					 sclp_request_timeout_normal);
	}
	spin_unlock_irqrestore(&sclp_lock, flags);
	sclp_process_queue();
}

/*
 * Returns the expire value in jiffies of the next pending request timeout,
 * if any. Needs to be called with sclp_lock.
 */
static unsigned long __sclp_req_queue_find_next_timeout(void)
{
	unsigned long expires_next = 0;
	struct sclp_req *req;

	list_for_each_entry(req, &sclp_req_queue, list) {
		if (!req->queue_expires)
			continue;
		if (!expires_next ||
		   (time_before(req->queue_expires, expires_next)))
				expires_next = req->queue_expires;
	}
	return expires_next;
}

/*
 * Returns expired request, if any, and removes it from the list.
 */
static struct sclp_req *__sclp_req_queue_remove_expired_req(void)
{
	unsigned long flags, now;
	struct sclp_req *req;

	spin_lock_irqsave(&sclp_lock, flags);
	now = jiffies;
	/* Don't need list_for_each_safe because we break out after list_del */
	list_for_each_entry(req, &sclp_req_queue, list) {
		if (!req->queue_expires)
			continue;
		if (time_before_eq(req->queue_expires, now)) {
			if (req->status == SCLP_REQ_QUEUED) {
				req->status = SCLP_REQ_QUEUED_TIMEOUT;
				list_del(&req->list);
				goto out;
			}
		}
	}
	req = NULL;
out:
	spin_unlock_irqrestore(&sclp_lock, flags);
	return req;
}

/*
 * Timeout handler for queued requests. Removes request from list and
 * invokes callback. This timer can be set per request in situations where
 * waiting too long would be harmful to the system, e.g. during SE reboot.
 */
static void sclp_req_queue_timeout(struct timer_list *unused)
{
	unsigned long flags, expires_next;
	struct sclp_req *req;

	do {
		req = __sclp_req_queue_remove_expired_req();
		if (req && req->callback)
			req->callback(req, req->callback_data);
	} while (req);

	spin_lock_irqsave(&sclp_lock, flags);
	expires_next = __sclp_req_queue_find_next_timeout();
	if (expires_next)
		mod_timer(&sclp_queue_timer, expires_next);
	spin_unlock_irqrestore(&sclp_lock, flags);
}

/* Try to start a request. Return zero if the request was successfully
 * started or if it will be started at a later time. Return non-zero otherwise.
 * Called while sclp_lock is locked. */
static int
__sclp_start_request(struct sclp_req *req)
{
	int rc;

	if (sclp_running_state != sclp_running_state_idle)
		return 0;
	del_timer(&sclp_request_timer);
	rc = sclp_service_call(req->command, req->sccb);
	req->start_count++;

	if (rc == 0) {
		/* Successfully started request */
		req->status = SCLP_REQ_RUNNING;
		sclp_running_state = sclp_running_state_running;
		__sclp_set_request_timer(SCLP_RETRY_INTERVAL * HZ,
					 sclp_request_timeout_restart);
		return 0;
	} else if (rc == -EBUSY) {
		/* Try again later */
		__sclp_set_request_timer(SCLP_BUSY_INTERVAL * HZ,
					 sclp_request_timeout_normal);
		return 0;
	}
	/* Request failed */
	req->status = SCLP_REQ_FAILED;
	return rc;
}

/* Try to start queued requests. */
static void
sclp_process_queue(void)
{
	struct sclp_req *req;
	int rc;
	unsigned long flags;

	spin_lock_irqsave(&sclp_lock, flags);
	if (sclp_running_state != sclp_running_state_idle) {
		spin_unlock_irqrestore(&sclp_lock, flags);
		return;
	}
	del_timer(&sclp_request_timer);
	while (!list_empty(&sclp_req_queue)) {
		req = list_entry(sclp_req_queue.next, struct sclp_req, list);
		if (!req->sccb)
			goto do_post;
		rc = __sclp_start_request(req);
		if (rc == 0)
			break;
		/* Request failed */
		if (req->start_count > 1) {
			/* Cannot abort already submitted request - could still
			 * be active at the SCLP */
			__sclp_set_request_timer(SCLP_BUSY_INTERVAL * HZ,
						 sclp_request_timeout_normal);
			break;
		}
do_post:
		/* Post-processing for aborted request */
		list_del(&req->list);
		if (req->callback) {
			spin_unlock_irqrestore(&sclp_lock, flags);
			req->callback(req, req->callback_data);
			spin_lock_irqsave(&sclp_lock, flags);
		}
	}
	spin_unlock_irqrestore(&sclp_lock, flags);
}

static int __sclp_can_add_request(struct sclp_req *req)
{
	if (req == &sclp_suspend_req || req == &sclp_init_req)
		return 1;
	if (sclp_suspend_state != sclp_suspend_state_running)
		return 0;
	if (sclp_init_state != sclp_init_state_initialized)
		return 0;
	if (sclp_activation_state != sclp_activation_state_active)
		return 0;
	return 1;
}

/* Queue a new request. Return zero on success, non-zero otherwise. */
int
sclp_add_request(struct sclp_req *req)
{
	unsigned long flags;
	int rc;

	spin_lock_irqsave(&sclp_lock, flags);
	if (!__sclp_can_add_request(req)) {
		spin_unlock_irqrestore(&sclp_lock, flags);
		return -EIO;
	}
	req->status = SCLP_REQ_QUEUED;
	req->start_count = 0;
	list_add_tail(&req->list, &sclp_req_queue);
	rc = 0;
	if (req->queue_timeout) {
		req->queue_expires = jiffies + req->queue_timeout * HZ;
		if (!timer_pending(&sclp_queue_timer) ||
		    time_after(sclp_queue_timer.expires, req->queue_expires))
			mod_timer(&sclp_queue_timer, req->queue_expires);
	} else
		req->queue_expires = 0;
	/* Start if request is first in list */
	if (sclp_running_state == sclp_running_state_idle &&
	    req->list.prev == &sclp_req_queue) {
		if (!req->sccb) {
			list_del(&req->list);
			rc = -ENODATA;
			goto out;
		}
		rc = __sclp_start_request(req);
		if (rc)
			list_del(&req->list);
	}
out:
	spin_unlock_irqrestore(&sclp_lock, flags);
	return rc;
}

EXPORT_SYMBOL(sclp_add_request);

/* Dispatch events found in request buffer to registered listeners. Return 0
 * if all events were dispatched, non-zero otherwise. */
static int
sclp_dispatch_evbufs(struct sccb_header *sccb)
{
	unsigned long flags;
	struct evbuf_header *evbuf;
	struct list_head *l;
	struct sclp_register *reg;
	int offset;
	int rc;

	spin_lock_irqsave(&sclp_lock, flags);
	rc = 0;
	for (offset = sizeof(struct sccb_header); offset < sccb->length;
	     offset += evbuf->length) {
		evbuf = (struct evbuf_header *) ((addr_t) sccb + offset);
		/* Check for malformed hardware response */
		if (evbuf->length == 0)
			break;
		/* Search for event handler */
		reg = NULL;
		list_for_each(l, &sclp_reg_list) {
			reg = list_entry(l, struct sclp_register, list);
			if (reg->receive_mask & SCLP_EVTYP_MASK(evbuf->type))
				break;
			else
				reg = NULL;
		}
		if (reg && reg->receiver_fn) {
			spin_unlock_irqrestore(&sclp_lock, flags);
			reg->receiver_fn(evbuf);
			spin_lock_irqsave(&sclp_lock, flags);
		} else if (reg == NULL)
			rc = -EOPNOTSUPP;
	}
	spin_unlock_irqrestore(&sclp_lock, flags);
	return rc;
}

/* Read event data request callback. */
static void
sclp_read_cb(struct sclp_req *req, void *data)
{
	unsigned long flags;
	struct sccb_header *sccb;

	sccb = (struct sccb_header *) req->sccb;
	if (req->status == SCLP_REQ_DONE && (sccb->response_code == 0x20 ||
	    sccb->response_code == 0x220))
		sclp_dispatch_evbufs(sccb);
	spin_lock_irqsave(&sclp_lock, flags);
	sclp_reading_state = sclp_reading_state_idle;
	spin_unlock_irqrestore(&sclp_lock, flags);
}

/* Prepare read event data request. Called while sclp_lock is locked. */
static void __sclp_make_read_req(void)
{
	struct sccb_header *sccb;

	sccb = (struct sccb_header *) sclp_read_sccb;
	clear_page(sccb);
	memset(&sclp_read_req, 0, sizeof(struct sclp_req));
	sclp_read_req.command = SCLP_CMDW_READ_EVENT_DATA;
	sclp_read_req.status = SCLP_REQ_QUEUED;
	sclp_read_req.start_count = 0;
	sclp_read_req.callback = sclp_read_cb;
	sclp_read_req.sccb = sccb;
	sccb->length = PAGE_SIZE;
	sccb->function_code = 0;
	sccb->control_mask[2] = 0x80;
}

/* Search request list for request with matching sccb. Return request if found,
 * NULL otherwise. Called while sclp_lock is locked. */
static inline struct sclp_req *
__sclp_find_req(u32 sccb)
{
	struct list_head *l;
	struct sclp_req *req;

	list_for_each(l, &sclp_req_queue) {
		req = list_entry(l, struct sclp_req, list);
		if (sccb == (u32) (addr_t) req->sccb)
				return req;
	}
	return NULL;
}

/* Handler for external interruption. Perform request post-processing.
 * Prepare read event data request if necessary. Start processing of next
 * request on queue. */
static void sclp_interrupt_handler(struct ext_code ext_code,
				   unsigned int param32, unsigned long param64)
{
	struct sclp_req *req;
	u32 finished_sccb;
	u32 evbuf_pending;

	inc_irq_stat(IRQEXT_SCP);
	spin_lock(&sclp_lock);
	finished_sccb = param32 & 0xfffffff8;
	evbuf_pending = param32 & 0x3;
	if (finished_sccb) {
		del_timer(&sclp_request_timer);
		sclp_running_state = sclp_running_state_reset_pending;
		req = __sclp_find_req(finished_sccb);
		if (req) {
			/* Request post-processing */
			list_del(&req->list);
			req->status = SCLP_REQ_DONE;
			if (req->callback) {
				spin_unlock(&sclp_lock);
				req->callback(req, req->callback_data);
				spin_lock(&sclp_lock);
			}
		}
		sclp_running_state = sclp_running_state_idle;
	}
	if (evbuf_pending &&
	    sclp_activation_state == sclp_activation_state_active)
		__sclp_queue_read_req();
	spin_unlock(&sclp_lock);
	sclp_process_queue();
}

/* Convert interval in jiffies to TOD ticks. */
static inline u64
sclp_tod_from_jiffies(unsigned long jiffies)
{
	return (u64) (jiffies / HZ) << 32;
}

/* Wait until a currently running request finished. Note: while this function
 * is running, no timers are served on the calling CPU. */
void
sclp_sync_wait(void)
{
	unsigned long long old_tick;
	unsigned long flags;
	unsigned long cr0, cr0_sync;
	u64 timeout;
	int irq_context;

	/* We'll be disabling timer interrupts, so we need a custom timeout
	 * mechanism */
	timeout = 0;
	if (timer_pending(&sclp_request_timer)) {
		/* Get timeout TOD value */
		timeout = get_tod_clock_fast() +
			  sclp_tod_from_jiffies(sclp_request_timer.expires -
						jiffies);
	}
	local_irq_save(flags);
	/* Prevent bottom half from executing once we force interrupts open */
	irq_context = in_interrupt();
	if (!irq_context)
		local_bh_disable();
	/* Enable service-signal interruption, disable timer interrupts */
	old_tick = local_tick_disable();
	trace_hardirqs_on();
	__ctl_store(cr0, 0, 0);
	cr0_sync = cr0 & ~CR0_IRQ_SUBCLASS_MASK;
	cr0_sync |= 1UL << (63 - 54);
	__ctl_load(cr0_sync, 0, 0);
	__arch_local_irq_stosm(0x01);
	/* Loop until driver state indicates finished request */
	while (sclp_running_state != sclp_running_state_idle) {
		/* Check for expired request timer */
		if (timer_pending(&sclp_request_timer) &&
		    get_tod_clock_fast() > timeout &&
		    del_timer(&sclp_request_timer))
			sclp_request_timer.function(&sclp_request_timer);
		cpu_relax();
	}
	local_irq_disable();
	__ctl_load(cr0, 0, 0);
	if (!irq_context)
		_local_bh_enable();
	local_tick_enable(old_tick);
	local_irq_restore(flags);
}
EXPORT_SYMBOL(sclp_sync_wait);

/* Dispatch changes in send and receive mask to registered listeners. */
static void
sclp_dispatch_state_change(void)
{
	struct list_head *l;
	struct sclp_register *reg;
	unsigned long flags;
	sccb_mask_t receive_mask;
	sccb_mask_t send_mask;

	do {
		spin_lock_irqsave(&sclp_lock, flags);
		reg = NULL;
		list_for_each(l, &sclp_reg_list) {
			reg = list_entry(l, struct sclp_register, list);
			receive_mask = reg->send_mask & sclp_receive_mask;
			send_mask = reg->receive_mask & sclp_send_mask;
			if (reg->sclp_receive_mask != receive_mask ||
			    reg->sclp_send_mask != send_mask) {
				reg->sclp_receive_mask = receive_mask;
				reg->sclp_send_mask = send_mask;
				break;
			} else
				reg = NULL;
		}
		spin_unlock_irqrestore(&sclp_lock, flags);
		if (reg && reg->state_change_fn)
			reg->state_change_fn(reg);
	} while (reg);
}

struct sclp_statechangebuf {
	struct evbuf_header	header;
	u8		validity_sclp_active_facility_mask : 1;
	u8		validity_sclp_receive_mask : 1;
	u8		validity_sclp_send_mask : 1;
	u8		validity_read_data_function_mask : 1;
	u16		_zeros : 12;
	u16		mask_length;
	u64		sclp_active_facility_mask;
	u8		masks[2 * 1021 + 4];	/* variable length */
	/*
	 * u8		sclp_receive_mask[mask_length];
	 * u8		sclp_send_mask[mask_length];
	 * u32		read_data_function_mask;
	 */
} __attribute__((packed));


/* State change event callback. Inform listeners of changes. */
static void
sclp_state_change_cb(struct evbuf_header *evbuf)
{
	unsigned long flags;
	struct sclp_statechangebuf *scbuf;

	BUILD_BUG_ON(sizeof(struct sclp_statechangebuf) > PAGE_SIZE);

	scbuf = (struct sclp_statechangebuf *) evbuf;
	spin_lock_irqsave(&sclp_lock, flags);
	if (scbuf->validity_sclp_receive_mask)
		sclp_receive_mask = sccb_get_recv_mask(scbuf);
	if (scbuf->validity_sclp_send_mask)
		sclp_send_mask = sccb_get_send_mask(scbuf);
	spin_unlock_irqrestore(&sclp_lock, flags);
	if (scbuf->validity_sclp_active_facility_mask)
		sclp.facilities = scbuf->sclp_active_facility_mask;
	sclp_dispatch_state_change();
}

static struct sclp_register sclp_state_change_event = {
	.receive_mask = EVTYP_STATECHANGE_MASK,
	.receiver_fn = sclp_state_change_cb
};

/* Calculate receive and send mask of currently registered listeners.
 * Called while sclp_lock is locked. */
static inline void
__sclp_get_mask(sccb_mask_t *receive_mask, sccb_mask_t *send_mask)
{
	struct list_head *l;
	struct sclp_register *t;

	*receive_mask = 0;
	*send_mask = 0;
	list_for_each(l, &sclp_reg_list) {
		t = list_entry(l, struct sclp_register, list);
		*receive_mask |= t->receive_mask;
		*send_mask |= t->send_mask;
	}
}

/* Register event listener. Return 0 on success, non-zero otherwise. */
int
sclp_register(struct sclp_register *reg)
{
	unsigned long flags;
	sccb_mask_t receive_mask;
	sccb_mask_t send_mask;
	int rc;

	rc = sclp_init();
	if (rc)
		return rc;
	spin_lock_irqsave(&sclp_lock, flags);
	/* Check event mask for collisions */
	__sclp_get_mask(&receive_mask, &send_mask);
	if (reg->receive_mask & receive_mask || reg->send_mask & send_mask) {
		spin_unlock_irqrestore(&sclp_lock, flags);
		return -EBUSY;
	}
	/* Trigger initial state change callback */
	reg->sclp_receive_mask = 0;
	reg->sclp_send_mask = 0;
	reg->pm_event_posted = 0;
	list_add(&reg->list, &sclp_reg_list);
	spin_unlock_irqrestore(&sclp_lock, flags);
	rc = sclp_init_mask(1);
	if (rc) {
		spin_lock_irqsave(&sclp_lock, flags);
		list_del(&reg->list);
		spin_unlock_irqrestore(&sclp_lock, flags);
	}
	return rc;
}

EXPORT_SYMBOL(sclp_register);

/* Unregister event listener. */
void
sclp_unregister(struct sclp_register *reg)
{
	unsigned long flags;

	spin_lock_irqsave(&sclp_lock, flags);
	list_del(&reg->list);
	spin_unlock_irqrestore(&sclp_lock, flags);
	sclp_init_mask(1);
}

EXPORT_SYMBOL(sclp_unregister);

/* Remove event buffers which are marked processed. Return the number of
 * remaining event buffers. */
int
sclp_remove_processed(struct sccb_header *sccb)
{
	struct evbuf_header *evbuf;
	int unprocessed;
	u16 remaining;

	evbuf = (struct evbuf_header *) (sccb + 1);
	unprocessed = 0;
	remaining = sccb->length - sizeof(struct sccb_header);
	while (remaining > 0) {
		remaining -= evbuf->length;
		if (evbuf->flags & 0x80) {
			sccb->length -= evbuf->length;
			memcpy(evbuf, (void *) ((addr_t) evbuf + evbuf->length),
			       remaining);
		} else {
			unprocessed++;
			evbuf = (struct evbuf_header *)
					((addr_t) evbuf + evbuf->length);
		}
	}
	return unprocessed;
}

EXPORT_SYMBOL(sclp_remove_processed);

/* Prepare init mask request. Called while sclp_lock is locked. */
static inline void
__sclp_make_init_req(sccb_mask_t receive_mask, sccb_mask_t send_mask)
{
	struct init_sccb *sccb = sclp_init_sccb;

	clear_page(sccb);
	memset(&sclp_init_req, 0, sizeof(struct sclp_req));
	sclp_init_req.command = SCLP_CMDW_WRITE_EVENT_MASK;
	sclp_init_req.status = SCLP_REQ_FILLED;
	sclp_init_req.start_count = 0;
	sclp_init_req.callback = NULL;
	sclp_init_req.callback_data = NULL;
	sclp_init_req.sccb = sccb;
	sccb->header.length = sizeof(*sccb);
	if (sclp_mask_compat_mode)
		sccb->mask_length = SCLP_MASK_SIZE_COMPAT;
	else
		sccb->mask_length = sizeof(sccb_mask_t);
	sccb_set_recv_mask(sccb, receive_mask);
	sccb_set_send_mask(sccb, send_mask);
	sccb_set_sclp_recv_mask(sccb, 0);
	sccb_set_sclp_send_mask(sccb, 0);
}

/* Start init mask request. If calculate is non-zero, calculate the mask as
 * requested by registered listeners. Use zero mask otherwise. Return 0 on
 * success, non-zero otherwise. */
static int
sclp_init_mask(int calculate)
{
	unsigned long flags;
	struct init_sccb *sccb = sclp_init_sccb;
	sccb_mask_t receive_mask;
	sccb_mask_t send_mask;
	int retry;
	int rc;
	unsigned long wait;

	spin_lock_irqsave(&sclp_lock, flags);
	/* Check if interface is in appropriate state */
	if (sclp_mask_state != sclp_mask_state_idle) {
		spin_unlock_irqrestore(&sclp_lock, flags);
		return -EBUSY;
	}
	if (sclp_activation_state == sclp_activation_state_inactive) {
		spin_unlock_irqrestore(&sclp_lock, flags);
		return -EINVAL;
	}
	sclp_mask_state = sclp_mask_state_initializing;
	/* Determine mask */
	if (calculate)
		__sclp_get_mask(&receive_mask, &send_mask);
	else {
		receive_mask = 0;
		send_mask = 0;
	}
	rc = -EIO;
	for (retry = 0; retry <= SCLP_MASK_RETRY; retry++) {
		/* Prepare request */
		__sclp_make_init_req(receive_mask, send_mask);
		spin_unlock_irqrestore(&sclp_lock, flags);
		if (sclp_add_request(&sclp_init_req)) {
			/* Try again later */
			wait = jiffies + SCLP_BUSY_INTERVAL * HZ;
			while (time_before(jiffies, wait))
				sclp_sync_wait();
			spin_lock_irqsave(&sclp_lock, flags);
			continue;
		}
		while (sclp_init_req.status != SCLP_REQ_DONE &&
		       sclp_init_req.status != SCLP_REQ_FAILED)
			sclp_sync_wait();
		spin_lock_irqsave(&sclp_lock, flags);
		if (sclp_init_req.status == SCLP_REQ_DONE &&
		    sccb->header.response_code == 0x20) {
			/* Successful request */
			if (calculate) {
				sclp_receive_mask = sccb_get_sclp_recv_mask(sccb);
				sclp_send_mask = sccb_get_sclp_send_mask(sccb);
			} else {
				sclp_receive_mask = 0;
				sclp_send_mask = 0;
			}
			spin_unlock_irqrestore(&sclp_lock, flags);
			sclp_dispatch_state_change();
			spin_lock_irqsave(&sclp_lock, flags);
			rc = 0;
			break;
		}
	}
	sclp_mask_state = sclp_mask_state_idle;
	spin_unlock_irqrestore(&sclp_lock, flags);
	return rc;
}

/* Deactivate SCLP interface. On success, new requests will be rejected,
 * events will no longer be dispatched. Return 0 on success, non-zero
 * otherwise. */
int
sclp_deactivate(void)
{
	unsigned long flags;
	int rc;

	spin_lock_irqsave(&sclp_lock, flags);
	/* Deactivate can only be called when active */
	if (sclp_activation_state != sclp_activation_state_active) {
		spin_unlock_irqrestore(&sclp_lock, flags);
		return -EINVAL;
	}
	sclp_activation_state = sclp_activation_state_deactivating;
	spin_unlock_irqrestore(&sclp_lock, flags);
	rc = sclp_init_mask(0);
	spin_lock_irqsave(&sclp_lock, flags);
	if (rc == 0)
		sclp_activation_state = sclp_activation_state_inactive;
	else
		sclp_activation_state = sclp_activation_state_active;
	spin_unlock_irqrestore(&sclp_lock, flags);
	return rc;
}

EXPORT_SYMBOL(sclp_deactivate);

/* Reactivate SCLP interface after sclp_deactivate. On success, new
 * requests will be accepted, events will be dispatched again. Return 0 on
 * success, non-zero otherwise. */
int
sclp_reactivate(void)
{
	unsigned long flags;
	int rc;

	spin_lock_irqsave(&sclp_lock, flags);
	/* Reactivate can only be called when inactive */
	if (sclp_activation_state != sclp_activation_state_inactive) {
		spin_unlock_irqrestore(&sclp_lock, flags);
		return -EINVAL;
	}
	sclp_activation_state = sclp_activation_state_activating;
	spin_unlock_irqrestore(&sclp_lock, flags);
	rc = sclp_init_mask(1);
	spin_lock_irqsave(&sclp_lock, flags);
	if (rc == 0)
		sclp_activation_state = sclp_activation_state_active;
	else
		sclp_activation_state = sclp_activation_state_inactive;
	spin_unlock_irqrestore(&sclp_lock, flags);
	return rc;
}

EXPORT_SYMBOL(sclp_reactivate);

/* Handler for external interruption used during initialization. Modify
 * request state to done. */
static void sclp_check_handler(struct ext_code ext_code,
			       unsigned int param32, unsigned long param64)
{
	u32 finished_sccb;

	inc_irq_stat(IRQEXT_SCP);
	finished_sccb = param32 & 0xfffffff8;
	/* Is this the interrupt we are waiting for? */
	if (finished_sccb == 0)
		return;
	if (finished_sccb != (u32) (addr_t) sclp_init_sccb)
		panic("sclp: unsolicited interrupt for buffer at 0x%x\n",
		      finished_sccb);
	spin_lock(&sclp_lock);
	if (sclp_running_state == sclp_running_state_running) {
		sclp_init_req.status = SCLP_REQ_DONE;
		sclp_running_state = sclp_running_state_idle;
	}
	spin_unlock(&sclp_lock);
}

/* Initial init mask request timed out. Modify request state to failed. */
static void
sclp_check_timeout(struct timer_list *unused)
{
	unsigned long flags;

	spin_lock_irqsave(&sclp_lock, flags);
	if (sclp_running_state == sclp_running_state_running) {
		sclp_init_req.status = SCLP_REQ_FAILED;
		sclp_running_state = sclp_running_state_idle;
	}
	spin_unlock_irqrestore(&sclp_lock, flags);
}

/* Perform a check of the SCLP interface. Return zero if the interface is
 * available and there are no pending requests from a previous instance.
 * Return non-zero otherwise. */
static int
sclp_check_interface(void)
{
	struct init_sccb *sccb;
	unsigned long flags;
	int retry;
	int rc;

	spin_lock_irqsave(&sclp_lock, flags);
	/* Prepare init mask command */
	rc = register_external_irq(EXT_IRQ_SERVICE_SIG, sclp_check_handler);
	if (rc) {
		spin_unlock_irqrestore(&sclp_lock, flags);
		return rc;
	}
	for (retry = 0; retry <= SCLP_INIT_RETRY; retry++) {
		__sclp_make_init_req(0, 0);
		sccb = (struct init_sccb *) sclp_init_req.sccb;
		rc = sclp_service_call(sclp_init_req.command, sccb);
		if (rc == -EIO)
			break;
		sclp_init_req.status = SCLP_REQ_RUNNING;
		sclp_running_state = sclp_running_state_running;
		__sclp_set_request_timer(SCLP_RETRY_INTERVAL * HZ,
					 sclp_check_timeout);
		spin_unlock_irqrestore(&sclp_lock, flags);
		/* Enable service-signal interruption - needs to happen
		 * with IRQs enabled. */
		irq_subclass_register(IRQ_SUBCLASS_SERVICE_SIGNAL);
		/* Wait for signal from interrupt or timeout */
		sclp_sync_wait();
		/* Disable service-signal interruption - needs to happen
		 * with IRQs enabled. */
		irq_subclass_unregister(IRQ_SUBCLASS_SERVICE_SIGNAL);
		spin_lock_irqsave(&sclp_lock, flags);
		del_timer(&sclp_request_timer);
		rc = -EBUSY;
		if (sclp_init_req.status == SCLP_REQ_DONE) {
			if (sccb->header.response_code == 0x20) {
				rc = 0;
				break;
			} else if (sccb->header.response_code == 0x74f0) {
				if (!sclp_mask_compat_mode) {
					sclp_mask_compat_mode = true;
					retry = 0;
				}
			}
		}
	}
	unregister_external_irq(EXT_IRQ_SERVICE_SIG, sclp_check_handler);
	spin_unlock_irqrestore(&sclp_lock, flags);
	return rc;
}

/* Reboot event handler. Reset send and receive mask to prevent pending SCLP
 * events from interfering with rebooted system. */
static int
sclp_reboot_event(struct notifier_block *this, unsigned long event, void *ptr)
{
	sclp_deactivate();
	return NOTIFY_DONE;
}

static struct notifier_block sclp_reboot_notifier = {
	.notifier_call = sclp_reboot_event
};

/*
 * Suspend/resume SCLP notifier implementation
 */

static void sclp_pm_event(enum sclp_pm_event sclp_pm_event, int rollback)
{
	struct sclp_register *reg;
	unsigned long flags;

	if (!rollback) {
		spin_lock_irqsave(&sclp_lock, flags);
		list_for_each_entry(reg, &sclp_reg_list, list)
			reg->pm_event_posted = 0;
		spin_unlock_irqrestore(&sclp_lock, flags);
	}
	do {
		spin_lock_irqsave(&sclp_lock, flags);
		list_for_each_entry(reg, &sclp_reg_list, list) {
			if (rollback && reg->pm_event_posted)
				goto found;
			if (!rollback && !reg->pm_event_posted)
				goto found;
		}
		spin_unlock_irqrestore(&sclp_lock, flags);
		return;
found:
		spin_unlock_irqrestore(&sclp_lock, flags);
		if (reg->pm_event_fn)
			reg->pm_event_fn(reg, sclp_pm_event);
		reg->pm_event_posted = rollback ? 0 : 1;
	} while (1);
}

/*
 * Susend/resume callbacks for platform device
 */

static int sclp_freeze(struct device *dev)
{
	unsigned long flags;
	int rc;

	sclp_pm_event(SCLP_PM_EVENT_FREEZE, 0);

	spin_lock_irqsave(&sclp_lock, flags);
	sclp_suspend_state = sclp_suspend_state_suspended;
	spin_unlock_irqrestore(&sclp_lock, flags);

	/* Init supend data */
	memset(&sclp_suspend_req, 0, sizeof(sclp_suspend_req));
	sclp_suspend_req.callback = sclp_suspend_req_cb;
	sclp_suspend_req.status = SCLP_REQ_FILLED;
	init_completion(&sclp_request_queue_flushed);

	rc = sclp_add_request(&sclp_suspend_req);
	if (rc == 0)
		wait_for_completion(&sclp_request_queue_flushed);
	else if (rc != -ENODATA)
		goto fail_thaw;

	rc = sclp_deactivate();
	if (rc)
		goto fail_thaw;
	return 0;

fail_thaw:
	spin_lock_irqsave(&sclp_lock, flags);
	sclp_suspend_state = sclp_suspend_state_running;
	spin_unlock_irqrestore(&sclp_lock, flags);
	sclp_pm_event(SCLP_PM_EVENT_THAW, 1);
	return rc;
}

static int sclp_undo_suspend(enum sclp_pm_event event)
{
	unsigned long flags;
	int rc;

	rc = sclp_reactivate();
	if (rc)
		return rc;

	spin_lock_irqsave(&sclp_lock, flags);
	sclp_suspend_state = sclp_suspend_state_running;
	spin_unlock_irqrestore(&sclp_lock, flags);

	sclp_pm_event(event, 0);
	return 0;
}

static int sclp_thaw(struct device *dev)
{
	return sclp_undo_suspend(SCLP_PM_EVENT_THAW);
}

static int sclp_restore(struct device *dev)
{
	return sclp_undo_suspend(SCLP_PM_EVENT_RESTORE);
}

static const struct dev_pm_ops sclp_pm_ops = {
	.freeze		= sclp_freeze,
	.thaw		= sclp_thaw,
	.restore	= sclp_restore,
};

static ssize_t con_pages_show(struct device_driver *dev, char *buf)
{
	return sprintf(buf, "%i\n", sclp_console_pages);
}

static DRIVER_ATTR_RO(con_pages);

static ssize_t con_drop_show(struct device_driver *dev, char *buf)
{
	return sprintf(buf, "%i\n", sclp_console_drop);
}

static DRIVER_ATTR_RO(con_drop);

static ssize_t con_full_show(struct device_driver *dev, char *buf)
{
	return sprintf(buf, "%lu\n", sclp_console_full);
}

static DRIVER_ATTR_RO(con_full);

static struct attribute *sclp_drv_attrs[] = {
	&driver_attr_con_pages.attr,
	&driver_attr_con_drop.attr,
	&driver_attr_con_full.attr,
	NULL,
};
static struct attribute_group sclp_drv_attr_group = {
	.attrs = sclp_drv_attrs,
};
static const struct attribute_group *sclp_drv_attr_groups[] = {
	&sclp_drv_attr_group,
	NULL,
};

static struct platform_driver sclp_pdrv = {
	.driver = {
		.name	= "sclp",
		.pm	= &sclp_pm_ops,
		.groups = sclp_drv_attr_groups,
	},
};

static struct platform_device *sclp_pdev;

/* Initialize SCLP driver. Return zero if driver is operational, non-zero
 * otherwise. */
static int
sclp_init(void)
{
	unsigned long flags;
	int rc = 0;

	spin_lock_irqsave(&sclp_lock, flags);
	/* Check for previous or running initialization */
	if (sclp_init_state != sclp_init_state_uninitialized)
		goto fail_unlock;
	sclp_init_state = sclp_init_state_initializing;
	sclp_read_sccb = (void *) __get_free_page(GFP_ATOMIC | GFP_DMA);
	sclp_init_sccb = (void *) __get_free_page(GFP_ATOMIC | GFP_DMA);
	BUG_ON(!sclp_read_sccb || !sclp_init_sccb);
	/* Set up variables */
	list_add(&sclp_state_change_event.list, &sclp_reg_list);
	timer_setup(&sclp_request_timer, NULL, 0);
	timer_setup(&sclp_queue_timer, sclp_req_queue_timeout, 0);
	/* Check interface */
	spin_unlock_irqrestore(&sclp_lock, flags);
	rc = sclp_check_interface();
	spin_lock_irqsave(&sclp_lock, flags);
	if (rc)
		goto fail_init_state_uninitialized;
	/* Register reboot handler */
	rc = register_reboot_notifier(&sclp_reboot_notifier);
	if (rc)
		goto fail_init_state_uninitialized;
	/* Register interrupt handler */
	rc = register_external_irq(EXT_IRQ_SERVICE_SIG, sclp_interrupt_handler);
	if (rc)
		goto fail_unregister_reboot_notifier;
	sclp_init_state = sclp_init_state_initialized;
	spin_unlock_irqrestore(&sclp_lock, flags);
	/* Enable service-signal external interruption - needs to happen with
	 * IRQs enabled. */
	irq_subclass_register(IRQ_SUBCLASS_SERVICE_SIGNAL);
	sclp_init_mask(1);
	return 0;

fail_unregister_reboot_notifier:
	unregister_reboot_notifier(&sclp_reboot_notifier);
fail_init_state_uninitialized:
	sclp_init_state = sclp_init_state_uninitialized;
	free_page((unsigned long) sclp_read_sccb);
	free_page((unsigned long) sclp_init_sccb);
fail_unlock:
	spin_unlock_irqrestore(&sclp_lock, flags);
	return rc;
}

/*
 * SCLP panic notifier: If we are suspended, we thaw SCLP in order to be able
 * to print the panic message.
 */
static int sclp_panic_notify(struct notifier_block *self,
			     unsigned long event, void *data)
{
	if (sclp_suspend_state == sclp_suspend_state_suspended)
		sclp_undo_suspend(SCLP_PM_EVENT_THAW);
	return NOTIFY_OK;
}

static struct notifier_block sclp_on_panic_nb = {
	.notifier_call = sclp_panic_notify,
	.priority = SCLP_PANIC_PRIO,
};

static __init int sclp_initcall(void)
{
	int rc;

	rc = platform_driver_register(&sclp_pdrv);
	if (rc)
		return rc;

	sclp_pdev = platform_device_register_simple("sclp", -1, NULL, 0);
	rc = PTR_ERR_OR_ZERO(sclp_pdev);
	if (rc)
		goto fail_platform_driver_unregister;

	rc = atomic_notifier_chain_register(&panic_notifier_list,
					    &sclp_on_panic_nb);
	if (rc)
		goto fail_platform_device_unregister;

	return sclp_init();

fail_platform_device_unregister:
	platform_device_unregister(sclp_pdev);
fail_platform_driver_unregister:
	platform_driver_unregister(&sclp_pdrv);
	return rc;
}

arch_initcall(sclp_initcall);
