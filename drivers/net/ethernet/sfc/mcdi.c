// SPDX-License-Identifier: GPL-2.0-only
/****************************************************************************
 * Driver for Solarflare network controllers and boards
 * Copyright 2008-2013 Solarflare Communications Inc.
 */

#include <linux/delay.h>
#include <linux/moduleparam.h>
#include <linux/atomic.h>
#include "net_driver.h"
#include "nic.h"
#include "io.h"
#include "farch_regs.h"
#include "mcdi_pcol.h"

/**************************************************************************
 *
 * Management-Controller-to-Driver Interface
 *
 **************************************************************************
 */

#define MCDI_RPC_TIMEOUT       (10 * HZ)

/* A reboot/assertion causes the MCDI status word to be set after the
 * command word is set or a REBOOT event is sent. If we notice a reboot
 * via these mechanisms then wait 250ms for the status word to be set.
 */
#define MCDI_STATUS_DELAY_US		100
#define MCDI_STATUS_DELAY_COUNT		2500
#define MCDI_STATUS_SLEEP_MS						\
	(MCDI_STATUS_DELAY_US * MCDI_STATUS_DELAY_COUNT / 1000)

#define SEQ_MASK							\
	EFX_MASK32(EFX_WIDTH(MCDI_HEADER_SEQ))

struct efx_mcdi_async_param {
	struct list_head list;
	unsigned int cmd;
	size_t inlen;
	size_t outlen;
	bool quiet;
	efx_mcdi_async_completer *complete;
	unsigned long cookie;
	/* followed by request/response buffer */
};

static void efx_mcdi_timeout_async(struct timer_list *t);
static int efx_mcdi_drv_attach(struct efx_nic *efx, bool driver_operating,
			       bool *was_attached_out);
static bool efx_mcdi_poll_once(struct efx_nic *efx);
static void efx_mcdi_abandon(struct efx_nic *efx);

#ifdef CONFIG_SFC_MCDI_LOGGING
static bool mcdi_logging_default;
module_param(mcdi_logging_default, bool, 0644);
MODULE_PARM_DESC(mcdi_logging_default,
		 "Enable MCDI logging on newly-probed functions");
#endif

int efx_mcdi_init(struct efx_nic *efx)
{
	struct efx_mcdi_iface *mcdi;
	bool already_attached;
	int rc = -ENOMEM;

	efx->mcdi = kzalloc(sizeof(*efx->mcdi), GFP_KERNEL);
	if (!efx->mcdi)
		goto fail;

	mcdi = efx_mcdi(efx);
	mcdi->efx = efx;
#ifdef CONFIG_SFC_MCDI_LOGGING
	/* consuming code assumes buffer is page-sized */
	mcdi->logging_buffer = (char *)__get_free_page(GFP_KERNEL);
	if (!mcdi->logging_buffer)
		goto fail1;
	mcdi->logging_enabled = mcdi_logging_default;
#endif
	init_waitqueue_head(&mcdi->wq);
	init_waitqueue_head(&mcdi->proxy_rx_wq);
	spin_lock_init(&mcdi->iface_lock);
	mcdi->state = MCDI_STATE_QUIESCENT;
	mcdi->mode = MCDI_MODE_POLL;
	spin_lock_init(&mcdi->async_lock);
	INIT_LIST_HEAD(&mcdi->async_list);
	timer_setup(&mcdi->async_timer, efx_mcdi_timeout_async, 0);

	(void) efx_mcdi_poll_reboot(efx);
	mcdi->new_epoch = true;

	/* Recover from a failed assertion before probing */
	rc = efx_mcdi_handle_assertion(efx);
	if (rc)
		goto fail2;

	/* Let the MC (and BMC, if this is a LOM) know that the driver
	 * is loaded. We should do this before we reset the NIC.
	 */
	rc = efx_mcdi_drv_attach(efx, true, &already_attached);
	if (rc) {
		netif_err(efx, probe, efx->net_dev,
			  "Unable to register driver with MCPU\n");
		goto fail2;
	}
	if (already_attached)
		/* Not a fatal error */
		netif_err(efx, probe, efx->net_dev,
			  "Host already registered with MCPU\n");

	if (efx->mcdi->fn_flags &
	    (1 << MC_CMD_DRV_ATTACH_EXT_OUT_FLAG_PRIMARY))
		efx->primary = efx;

	return 0;
fail2:
#ifdef CONFIG_SFC_MCDI_LOGGING
	free_page((unsigned long)mcdi->logging_buffer);
fail1:
#endif
	kfree(efx->mcdi);
	efx->mcdi = NULL;
fail:
	return rc;
}

void efx_mcdi_detach(struct efx_nic *efx)
{
	if (!efx->mcdi)
		return;

	BUG_ON(efx->mcdi->iface.state != MCDI_STATE_QUIESCENT);

	/* Relinquish the device (back to the BMC, if this is a LOM) */
	efx_mcdi_drv_attach(efx, false, NULL);
}

void efx_mcdi_fini(struct efx_nic *efx)
{
	if (!efx->mcdi)
		return;

#ifdef CONFIG_SFC_MCDI_LOGGING
	free_page((unsigned long)efx->mcdi->iface.logging_buffer);
#endif

	kfree(efx->mcdi);
}

static void efx_mcdi_send_request(struct efx_nic *efx, unsigned cmd,
				  const efx_dword_t *inbuf, size_t inlen)
{
	struct efx_mcdi_iface *mcdi = efx_mcdi(efx);
#ifdef CONFIG_SFC_MCDI_LOGGING
	char *buf = mcdi->logging_buffer; /* page-sized */
#endif
	efx_dword_t hdr[2];
	size_t hdr_len;
	u32 xflags, seqno;

	BUG_ON(mcdi->state == MCDI_STATE_QUIESCENT);

	/* Serialise with efx_mcdi_ev_cpl() and efx_mcdi_ev_death() */
	spin_lock_bh(&mcdi->iface_lock);
	++mcdi->seqno;
	spin_unlock_bh(&mcdi->iface_lock);

	seqno = mcdi->seqno & SEQ_MASK;
	xflags = 0;
	if (mcdi->mode == MCDI_MODE_EVENTS)
		xflags |= MCDI_HEADER_XFLAGS_EVREQ;

	if (efx->type->mcdi_max_ver == 1) {
		/* MCDI v1 */
		EFX_POPULATE_DWORD_7(hdr[0],
				     MCDI_HEADER_RESPONSE, 0,
				     MCDI_HEADER_RESYNC, 1,
				     MCDI_HEADER_CODE, cmd,
				     MCDI_HEADER_DATALEN, inlen,
				     MCDI_HEADER_SEQ, seqno,
				     MCDI_HEADER_XFLAGS, xflags,
				     MCDI_HEADER_NOT_EPOCH, !mcdi->new_epoch);
		hdr_len = 4;
	} else {
		/* MCDI v2 */
		BUG_ON(inlen > MCDI_CTL_SDU_LEN_MAX_V2);
		EFX_POPULATE_DWORD_7(hdr[0],
				     MCDI_HEADER_RESPONSE, 0,
				     MCDI_HEADER_RESYNC, 1,
				     MCDI_HEADER_CODE, MC_CMD_V2_EXTN,
				     MCDI_HEADER_DATALEN, 0,
				     MCDI_HEADER_SEQ, seqno,
				     MCDI_HEADER_XFLAGS, xflags,
				     MCDI_HEADER_NOT_EPOCH, !mcdi->new_epoch);
		EFX_POPULATE_DWORD_2(hdr[1],
				     MC_CMD_V2_EXTN_IN_EXTENDED_CMD, cmd,
				     MC_CMD_V2_EXTN_IN_ACTUAL_LEN, inlen);
		hdr_len = 8;
	}

#ifdef CONFIG_SFC_MCDI_LOGGING
	if (mcdi->logging_enabled && !WARN_ON_ONCE(!buf)) {
		int bytes = 0;
		int i;
		/* Lengths should always be a whole number of dwords, so scream
		 * if they're not.
		 */
		WARN_ON_ONCE(hdr_len % 4);
		WARN_ON_ONCE(inlen % 4);

		/* We own the logging buffer, as only one MCDI can be in
		 * progress on a NIC at any one time.  So no need for locking.
		 */
		for (i = 0; i < hdr_len / 4 && bytes < PAGE_SIZE; i++)
			bytes += scnprintf(buf + bytes, PAGE_SIZE - bytes,
					   " %08x",
					   le32_to_cpu(hdr[i].u32[0]));

		for (i = 0; i < inlen / 4 && bytes < PAGE_SIZE; i++)
			bytes += scnprintf(buf + bytes, PAGE_SIZE - bytes,
					   " %08x",
					   le32_to_cpu(inbuf[i].u32[0]));

		netif_info(efx, hw, efx->net_dev, "MCDI RPC REQ:%s\n", buf);
	}
#endif

	efx->type->mcdi_request(efx, hdr, hdr_len, inbuf, inlen);

	mcdi->new_epoch = false;
}

static int efx_mcdi_errno(unsigned int mcdi_err)
{
	switch (mcdi_err) {
	case 0:
		return 0;
#define TRANSLATE_ERROR(name)					\
	case MC_CMD_ERR_ ## name:				\
		return -name;
	TRANSLATE_ERROR(EPERM);
	TRANSLATE_ERROR(ENOENT);
	TRANSLATE_ERROR(EINTR);
	TRANSLATE_ERROR(EAGAIN);
	TRANSLATE_ERROR(EACCES);
	TRANSLATE_ERROR(EBUSY);
	TRANSLATE_ERROR(EINVAL);
	TRANSLATE_ERROR(EDEADLK);
	TRANSLATE_ERROR(ENOSYS);
	TRANSLATE_ERROR(ETIME);
	TRANSLATE_ERROR(EALREADY);
	TRANSLATE_ERROR(ENOSPC);
#undef TRANSLATE_ERROR
	case MC_CMD_ERR_ENOTSUP:
		return -EOPNOTSUPP;
	case MC_CMD_ERR_ALLOC_FAIL:
		return -ENOBUFS;
	case MC_CMD_ERR_MAC_EXIST:
		return -EADDRINUSE;
	default:
		return -EPROTO;
	}
}

static void efx_mcdi_read_response_header(struct efx_nic *efx)
{
	struct efx_mcdi_iface *mcdi = efx_mcdi(efx);
	unsigned int respseq, respcmd, error;
#ifdef CONFIG_SFC_MCDI_LOGGING
	char *buf = mcdi->logging_buffer; /* page-sized */
#endif
	efx_dword_t hdr;

	efx->type->mcdi_read_response(efx, &hdr, 0, 4);
	respseq = EFX_DWORD_FIELD(hdr, MCDI_HEADER_SEQ);
	respcmd = EFX_DWORD_FIELD(hdr, MCDI_HEADER_CODE);
	error = EFX_DWORD_FIELD(hdr, MCDI_HEADER_ERROR);

	if (respcmd != MC_CMD_V2_EXTN) {
		mcdi->resp_hdr_len = 4;
		mcdi->resp_data_len = EFX_DWORD_FIELD(hdr, MCDI_HEADER_DATALEN);
	} else {
		efx->type->mcdi_read_response(efx, &hdr, 4, 4);
		mcdi->resp_hdr_len = 8;
		mcdi->resp_data_len =
			EFX_DWORD_FIELD(hdr, MC_CMD_V2_EXTN_IN_ACTUAL_LEN);
	}

#ifdef CONFIG_SFC_MCDI_LOGGING
	if (mcdi->logging_enabled && !WARN_ON_ONCE(!buf)) {
		size_t hdr_len, data_len;
		int bytes = 0;
		int i;

		WARN_ON_ONCE(mcdi->resp_hdr_len % 4);
		hdr_len = mcdi->resp_hdr_len / 4;
		/* MCDI_DECLARE_BUF ensures that underlying buffer is padded
		 * to dword size, and the MCDI buffer is always dword size
		 */
		data_len = DIV_ROUND_UP(mcdi->resp_data_len, 4);

		/* We own the logging buffer, as only one MCDI can be in
		 * progress on a NIC at any one time.  So no need for locking.
		 */
		for (i = 0; i < hdr_len && bytes < PAGE_SIZE; i++) {
			efx->type->mcdi_read_response(efx, &hdr, (i * 4), 4);
			bytes += scnprintf(buf + bytes, PAGE_SIZE - bytes,
					   " %08x", le32_to_cpu(hdr.u32[0]));
		}

		for (i = 0; i < data_len && bytes < PAGE_SIZE; i++) {
			efx->type->mcdi_read_response(efx, &hdr,
					mcdi->resp_hdr_len + (i * 4), 4);
			bytes += scnprintf(buf + bytes, PAGE_SIZE - bytes,
					   " %08x", le32_to_cpu(hdr.u32[0]));
		}

		netif_info(efx, hw, efx->net_dev, "MCDI RPC RESP:%s\n", buf);
	}
#endif

	mcdi->resprc_raw = 0;
	if (error && mcdi->resp_data_len == 0) {
		netif_err(efx, hw, efx->net_dev, "MC rebooted\n");
		mcdi->resprc = -EIO;
	} else if ((respseq ^ mcdi->seqno) & SEQ_MASK) {
		netif_err(efx, hw, efx->net_dev,
			  "MC response mismatch tx seq 0x%x rx seq 0x%x\n",
			  respseq, mcdi->seqno);
		mcdi->resprc = -EIO;
	} else if (error) {
		efx->type->mcdi_read_response(efx, &hdr, mcdi->resp_hdr_len, 4);
		mcdi->resprc_raw = EFX_DWORD_FIELD(hdr, EFX_DWORD_0);
		mcdi->resprc = efx_mcdi_errno(mcdi->resprc_raw);
	} else {
		mcdi->resprc = 0;
	}
}

static bool efx_mcdi_poll_once(struct efx_nic *efx)
{
	struct efx_mcdi_iface *mcdi = efx_mcdi(efx);

	rmb();
	if (!efx->type->mcdi_poll_response(efx))
		return false;

	spin_lock_bh(&mcdi->iface_lock);
	efx_mcdi_read_response_header(efx);
	spin_unlock_bh(&mcdi->iface_lock);

	return true;
}

static int efx_mcdi_poll(struct efx_nic *efx)
{
	struct efx_mcdi_iface *mcdi = efx_mcdi(efx);
	unsigned long time, finish;
	unsigned int spins;
	int rc;

	/* Check for a reboot atomically with respect to efx_mcdi_copyout() */
	rc = efx_mcdi_poll_reboot(efx);
	if (rc) {
		spin_lock_bh(&mcdi->iface_lock);
		mcdi->resprc = rc;
		mcdi->resp_hdr_len = 0;
		mcdi->resp_data_len = 0;
		spin_unlock_bh(&mcdi->iface_lock);
		return 0;
	}

	/* Poll for completion. Poll quickly (once a us) for the 1st jiffy,
	 * because generally mcdi responses are fast. After that, back off
	 * and poll once a jiffy (approximately)
	 */
	spins = USER_TICK_USEC;
	finish = jiffies + MCDI_RPC_TIMEOUT;

	while (1) {
		if (spins != 0) {
			--spins;
			udelay(1);
		} else {
			schedule_timeout_uninterruptible(1);
		}

		time = jiffies;

		if (efx_mcdi_poll_once(efx))
			break;

		if (time_after(time, finish))
			return -ETIMEDOUT;
	}

	/* Return rc=0 like wait_event_timeout() */
	return 0;
}

/* Test and clear MC-rebooted flag for this port/function; reset
 * software state as necessary.
 */
int efx_mcdi_poll_reboot(struct efx_nic *efx)
{
	if (!efx->mcdi)
		return 0;

	return efx->type->mcdi_poll_reboot(efx);
}

static bool efx_mcdi_acquire_async(struct efx_mcdi_iface *mcdi)
{
	return cmpxchg(&mcdi->state,
		       MCDI_STATE_QUIESCENT, MCDI_STATE_RUNNING_ASYNC) ==
		MCDI_STATE_QUIESCENT;
}

static void efx_mcdi_acquire_sync(struct efx_mcdi_iface *mcdi)
{
	/* Wait until the interface becomes QUIESCENT and we win the race
	 * to mark it RUNNING_SYNC.
	 */
	wait_event(mcdi->wq,
		   cmpxchg(&mcdi->state,
			   MCDI_STATE_QUIESCENT, MCDI_STATE_RUNNING_SYNC) ==
		   MCDI_STATE_QUIESCENT);
}

static int efx_mcdi_await_completion(struct efx_nic *efx)
{
	struct efx_mcdi_iface *mcdi = efx_mcdi(efx);

	if (wait_event_timeout(mcdi->wq, mcdi->state == MCDI_STATE_COMPLETED,
			       MCDI_RPC_TIMEOUT) == 0)
		return -ETIMEDOUT;

	/* Check if efx_mcdi_set_mode() switched us back to polled completions.
	 * In which case, poll for completions directly. If efx_mcdi_ev_cpl()
	 * completed the request first, then we'll just end up completing the
	 * request again, which is safe.
	 *
	 * We need an smp_rmb() to synchronise with efx_mcdi_mode_poll(), which
	 * wait_event_timeout() implicitly provides.
	 */
	if (mcdi->mode == MCDI_MODE_POLL)
		return efx_mcdi_poll(efx);

	return 0;
}

/* If the interface is RUNNING_SYNC, switch to COMPLETED and wake the
 * requester.  Return whether this was done.  Does not take any locks.
 */
static bool efx_mcdi_complete_sync(struct efx_mcdi_iface *mcdi)
{
	if (cmpxchg(&mcdi->state,
		    MCDI_STATE_RUNNING_SYNC, MCDI_STATE_COMPLETED) ==
	    MCDI_STATE_RUNNING_SYNC) {
		wake_up(&mcdi->wq);
		return true;
	}

	return false;
}

static void efx_mcdi_release(struct efx_mcdi_iface *mcdi)
{
	if (mcdi->mode == MCDI_MODE_EVENTS) {
		struct efx_mcdi_async_param *async;
		struct efx_nic *efx = mcdi->efx;

		/* Process the asynchronous request queue */
		spin_lock_bh(&mcdi->async_lock);
		async = list_first_entry_or_null(
			&mcdi->async_list, struct efx_mcdi_async_param, list);
		if (async) {
			mcdi->state = MCDI_STATE_RUNNING_ASYNC;
			efx_mcdi_send_request(efx, async->cmd,
					      (const efx_dword_t *)(async + 1),
					      async->inlen);
			mod_timer(&mcdi->async_timer,
				  jiffies + MCDI_RPC_TIMEOUT);
		}
		spin_unlock_bh(&mcdi->async_lock);

		if (async)
			return;
	}

	mcdi->state = MCDI_STATE_QUIESCENT;
	wake_up(&mcdi->wq);
}

/* If the interface is RUNNING_ASYNC, switch to COMPLETED, call the
 * asynchronous completion function, and release the interface.
 * Return whether this was done.  Must be called in bh-disabled
 * context.  Will take iface_lock and async_lock.
 */
static bool efx_mcdi_complete_async(struct efx_mcdi_iface *mcdi, bool timeout)
{
	struct efx_nic *efx = mcdi->efx;
	struct efx_mcdi_async_param *async;
	size_t hdr_len, data_len, err_len;
	efx_dword_t *outbuf;
	MCDI_DECLARE_BUF_ERR(errbuf);
	int rc;

	if (cmpxchg(&mcdi->state,
		    MCDI_STATE_RUNNING_ASYNC, MCDI_STATE_COMPLETED) !=
	    MCDI_STATE_RUNNING_ASYNC)
		return false;

	spin_lock(&mcdi->iface_lock);
	if (timeout) {
		/* Ensure that if the completion event arrives later,
		 * the seqno check in efx_mcdi_ev_cpl() will fail
		 */
		++mcdi->seqno;
		++mcdi->credits;
		rc = -ETIMEDOUT;
		hdr_len = 0;
		data_len = 0;
	} else {
		rc = mcdi->resprc;
		hdr_len = mcdi->resp_hdr_len;
		data_len = mcdi->resp_data_len;
	}
	spin_unlock(&mcdi->iface_lock);

	/* Stop the timer.  In case the timer function is running, we
	 * must wait for it to return so that there is no possibility
	 * of it aborting the next request.
	 */
	if (!timeout)
		del_timer_sync(&mcdi->async_timer);

	spin_lock(&mcdi->async_lock);
	async = list_first_entry(&mcdi->async_list,
				 struct efx_mcdi_async_param, list);
	list_del(&async->list);
	spin_unlock(&mcdi->async_lock);

	outbuf = (efx_dword_t *)(async + 1);
	efx->type->mcdi_read_response(efx, outbuf, hdr_len,
				      min(async->outlen, data_len));
	if (!timeout && rc && !async->quiet) {
		err_len = min(sizeof(errbuf), data_len);
		efx->type->mcdi_read_response(efx, errbuf, hdr_len,
					      sizeof(errbuf));
		efx_mcdi_display_error(efx, async->cmd, async->inlen, errbuf,
				       err_len, rc);
	}

	if (async->complete)
		async->complete(efx, async->cookie, rc, outbuf,
				min(async->outlen, data_len));
	kfree(async);

	efx_mcdi_release(mcdi);

	return true;
}

static void efx_mcdi_ev_cpl(struct efx_nic *efx, unsigned int seqno,
			    unsigned int datalen, unsigned int mcdi_err)
{
	struct efx_mcdi_iface *mcdi = efx_mcdi(efx);
	bool wake = false;

	spin_lock(&mcdi->iface_lock);

	if ((seqno ^ mcdi->seqno) & SEQ_MASK) {
		if (mcdi->credits)
			/* The request has been cancelled */
			--mcdi->credits;
		else
			netif_err(efx, hw, efx->net_dev,
				  "MC response mismatch tx seq 0x%x rx "
				  "seq 0x%x\n", seqno, mcdi->seqno);
	} else {
		if (efx->type->mcdi_max_ver >= 2) {
			/* MCDI v2 responses don't fit in an event */
			efx_mcdi_read_response_header(efx);
		} else {
			mcdi->resprc = efx_mcdi_errno(mcdi_err);
			mcdi->resp_hdr_len = 4;
			mcdi->resp_data_len = datalen;
		}

		wake = true;
	}

	spin_unlock(&mcdi->iface_lock);

	if (wake) {
		if (!efx_mcdi_complete_async(mcdi, false))
			(void) efx_mcdi_complete_sync(mcdi);

		/* If the interface isn't RUNNING_ASYNC or
		 * RUNNING_SYNC then we've received a duplicate
		 * completion after we've already transitioned back to
		 * QUIESCENT. [A subsequent invocation would increment
		 * seqno, so would have failed the seqno check].
		 */
	}
}

static void efx_mcdi_timeout_async(struct timer_list *t)
{
	struct efx_mcdi_iface *mcdi = from_timer(mcdi, t, async_timer);

	efx_mcdi_complete_async(mcdi, true);
}

static int
efx_mcdi_check_supported(struct efx_nic *efx, unsigned int cmd, size_t inlen)
{
	if (efx->type->mcdi_max_ver < 0 ||
	     (efx->type->mcdi_max_ver < 2 &&
	      cmd > MC_CMD_CMD_SPACE_ESCAPE_7))
		return -EINVAL;

	if (inlen > MCDI_CTL_SDU_LEN_MAX_V2 ||
	    (efx->type->mcdi_max_ver < 2 &&
	     inlen > MCDI_CTL_SDU_LEN_MAX_V1))
		return -EMSGSIZE;

	return 0;
}

static bool efx_mcdi_get_proxy_handle(struct efx_nic *efx,
				      size_t hdr_len, size_t data_len,
				      u32 *proxy_handle)
{
	MCDI_DECLARE_BUF_ERR(testbuf);
	const size_t buflen = sizeof(testbuf);

	if (!proxy_handle || data_len < buflen)
		return false;

	efx->type->mcdi_read_response(efx, testbuf, hdr_len, buflen);
	if (MCDI_DWORD(testbuf, ERR_CODE) == MC_CMD_ERR_PROXY_PENDING) {
		*proxy_handle = MCDI_DWORD(testbuf, ERR_PROXY_PENDING_HANDLE);
		return true;
	}

	return false;
}

static int _efx_mcdi_rpc_finish(struct efx_nic *efx, unsigned int cmd,
				size_t inlen,
				efx_dword_t *outbuf, size_t outlen,
				size_t *outlen_actual, bool quiet,
				u32 *proxy_handle, int *raw_rc)
{
	struct efx_mcdi_iface *mcdi = efx_mcdi(efx);
	MCDI_DECLARE_BUF_ERR(errbuf);
	int rc;

	if (mcdi->mode == MCDI_MODE_POLL)
		rc = efx_mcdi_poll(efx);
	else
		rc = efx_mcdi_await_completion(efx);

	if (rc != 0) {
		netif_err(efx, hw, efx->net_dev,
			  "MC command 0x%x inlen %d mode %d timed out\n",
			  cmd, (int)inlen, mcdi->mode);

		if (mcdi->mode == MCDI_MODE_EVENTS && efx_mcdi_poll_once(efx)) {
			netif_err(efx, hw, efx->net_dev,
				  "MCDI request was completed without an event\n");
			rc = 0;
		}

		efx_mcdi_abandon(efx);

		/* Close the race with efx_mcdi_ev_cpl() executing just too late
		 * and completing a request we've just cancelled, by ensuring
		 * that the seqno check therein fails.
		 */
		spin_lock_bh(&mcdi->iface_lock);
		++mcdi->seqno;
		++mcdi->credits;
		spin_unlock_bh(&mcdi->iface_lock);
	}

	if (proxy_handle)
		*proxy_handle = 0;

	if (rc != 0) {
		if (outlen_actual)
			*outlen_actual = 0;
	} else {
		size_t hdr_len, data_len, err_len;

		/* At the very least we need a memory barrier here to ensure
		 * we pick up changes from efx_mcdi_ev_cpl(). Protect against
		 * a spurious efx_mcdi_ev_cpl() running concurrently by
		 * acquiring the iface_lock. */
		spin_lock_bh(&mcdi->iface_lock);
		rc = mcdi->resprc;
		if (raw_rc)
			*raw_rc = mcdi->resprc_raw;
		hdr_len = mcdi->resp_hdr_len;
		data_len = mcdi->resp_data_len;
		err_len = min(sizeof(errbuf), data_len);
		spin_unlock_bh(&mcdi->iface_lock);

		BUG_ON(rc > 0);

		efx->type->mcdi_read_response(efx, outbuf, hdr_len,
					      min(outlen, data_len));
		if (outlen_actual)
			*outlen_actual = data_len;

		efx->type->mcdi_read_response(efx, errbuf, hdr_len, err_len);

		if (cmd == MC_CMD_REBOOT && rc == -EIO) {
			/* Don't reset if MC_CMD_REBOOT returns EIO */
		} else if (rc == -EIO || rc == -EINTR) {
			netif_err(efx, hw, efx->net_dev, "MC reboot detected\n");
			netif_dbg(efx, hw, efx->net_dev, "MC rebooted during command %d rc %d\n",
				  cmd, -rc);
			if (efx->type->mcdi_reboot_detected)
				efx->type->mcdi_reboot_detected(efx);
			efx_schedule_reset(efx, RESET_TYPE_MC_FAILURE);
		} else if (proxy_handle && (rc == -EPROTO) &&
			   efx_mcdi_get_proxy_handle(efx, hdr_len, data_len,
						     proxy_handle)) {
			mcdi->proxy_rx_status = 0;
			mcdi->proxy_rx_handle = 0;
			mcdi->state = MCDI_STATE_PROXY_WAIT;
		} else if (rc && !quiet) {
			efx_mcdi_display_error(efx, cmd, inlen, errbuf, err_len,
					       rc);
		}

		if (rc == -EIO || rc == -EINTR) {
			msleep(MCDI_STATUS_SLEEP_MS);
			efx_mcdi_poll_reboot(efx);
			mcdi->new_epoch = true;
		}
	}

	if (!proxy_handle || !*proxy_handle)
		efx_mcdi_release(mcdi);
	return rc;
}

static void efx_mcdi_proxy_abort(struct efx_mcdi_iface *mcdi)
{
	if (mcdi->state == MCDI_STATE_PROXY_WAIT) {
		/* Interrupt the proxy wait. */
		mcdi->proxy_rx_status = -EINTR;
		wake_up(&mcdi->proxy_rx_wq);
	}
}

static void efx_mcdi_ev_proxy_response(struct efx_nic *efx,
				       u32 handle, int status)
{
	struct efx_mcdi_iface *mcdi = efx_mcdi(efx);

	WARN_ON(mcdi->state != MCDI_STATE_PROXY_WAIT);

	mcdi->proxy_rx_status = efx_mcdi_errno(status);
	/* Ensure the status is written before we update the handle, since the
	 * latter is used to check if we've finished.
	 */
	wmb();
	mcdi->proxy_rx_handle = handle;
	wake_up(&mcdi->proxy_rx_wq);
}

static int efx_mcdi_proxy_wait(struct efx_nic *efx, u32 handle, bool quiet)
{
	struct efx_mcdi_iface *mcdi = efx_mcdi(efx);
	int rc;

	/* Wait for a proxy event, or timeout. */
	rc = wait_event_timeout(mcdi->proxy_rx_wq,
				mcdi->proxy_rx_handle != 0 ||
				mcdi->proxy_rx_status == -EINTR,
				MCDI_RPC_TIMEOUT);

	if (rc <= 0) {
		netif_dbg(efx, hw, efx->net_dev,
			  "MCDI proxy timeout %d\n", handle);
		return -ETIMEDOUT;
	} else if (mcdi->proxy_rx_handle != handle) {
		netif_warn(efx, hw, efx->net_dev,
			   "MCDI proxy unexpected handle %d (expected %d)\n",
			   mcdi->proxy_rx_handle, handle);
		return -EINVAL;
	}

	return mcdi->proxy_rx_status;
}

static int _efx_mcdi_rpc(struct efx_nic *efx, unsigned int cmd,
			 const efx_dword_t *inbuf, size_t inlen,
			 efx_dword_t *outbuf, size_t outlen,
			 size_t *outlen_actual, bool quiet, int *raw_rc)
{
	u32 proxy_handle = 0; /* Zero is an invalid proxy handle. */
	int rc;

	if (inbuf && inlen && (inbuf == outbuf)) {
		/* The input buffer can't be aliased with the output. */
		WARN_ON(1);
		return -EINVAL;
	}

	rc = efx_mcdi_rpc_start(efx, cmd, inbuf, inlen);
	if (rc)
		return rc;

	rc = _efx_mcdi_rpc_finish(efx, cmd, inlen, outbuf, outlen,
				  outlen_actual, quiet, &proxy_handle, raw_rc);

	if (proxy_handle) {
		/* Handle proxy authorisation. This allows approval of MCDI
		 * operations to be delegated to the admin function, allowing
		 * fine control over (eg) multicast subscriptions.
		 */
		struct efx_mcdi_iface *mcdi = efx_mcdi(efx);

		netif_dbg(efx, hw, efx->net_dev,
			  "MCDI waiting for proxy auth %d\n",
			  proxy_handle);
		rc = efx_mcdi_proxy_wait(efx, proxy_handle, quiet);

		if (rc == 0) {
			netif_dbg(efx, hw, efx->net_dev,
				  "MCDI proxy retry %d\n", proxy_handle);

			/* We now retry the original request. */
			mcdi->state = MCDI_STATE_RUNNING_SYNC;
			efx_mcdi_send_request(efx, cmd, inbuf, inlen);

			rc = _efx_mcdi_rpc_finish(efx, cmd, inlen,
						  outbuf, outlen, outlen_actual,
						  quiet, NULL, raw_rc);
		} else {
			netif_cond_dbg(efx, hw, efx->net_dev, rc == -EPERM, err,
				       "MC command 0x%x failed after proxy auth rc=%d\n",
				       cmd, rc);

			if (rc == -EINTR || rc == -EIO)
				efx_schedule_reset(efx, RESET_TYPE_MC_FAILURE);
			efx_mcdi_release(mcdi);
		}
	}

	return rc;
}

static int _efx_mcdi_rpc_evb_retry(struct efx_nic *efx, unsigned cmd,
				   const efx_dword_t *inbuf, size_t inlen,
				   efx_dword_t *outbuf, size_t outlen,
				   size_t *outlen_actual, bool quiet)
{
	int raw_rc = 0;
	int rc;

	rc = _efx_mcdi_rpc(efx, cmd, inbuf, inlen,
			   outbuf, outlen, outlen_actual, true, &raw_rc);

	if ((rc == -EPROTO) && (raw_rc == MC_CMD_ERR_NO_EVB_PORT) &&
	    efx->type->is_vf) {
		/* If the EVB port isn't available within a VF this may
		 * mean the PF is still bringing the switch up. We should
		 * retry our request shortly.
		 */
		unsigned long abort_time = jiffies + MCDI_RPC_TIMEOUT;
		unsigned int delay_us = 10000;

		netif_dbg(efx, hw, efx->net_dev,
			  "%s: NO_EVB_PORT; will retry request\n",
			  __func__);

		do {
			usleep_range(delay_us, delay_us + 10000);
			rc = _efx_mcdi_rpc(efx, cmd, inbuf, inlen,
					   outbuf, outlen, outlen_actual,
					   true, &raw_rc);
			if (delay_us < 100000)
				delay_us <<= 1;
		} while ((rc == -EPROTO) &&
			 (raw_rc == MC_CMD_ERR_NO_EVB_PORT) &&
			 time_before(jiffies, abort_time));
	}

	if (rc && !quiet && !(cmd == MC_CMD_REBOOT && rc == -EIO))
		efx_mcdi_display_error(efx, cmd, inlen,
				       outbuf, outlen, rc);

	return rc;
}

/**
 * efx_mcdi_rpc - Issue an MCDI command and wait for completion
 * @efx: NIC through which to issue the command
 * @cmd: Command type number
 * @inbuf: Command parameters
 * @inlen: Length of command parameters, in bytes.  Must be a multiple
 *	of 4 and no greater than %MCDI_CTL_SDU_LEN_MAX_V1.
 * @outbuf: Response buffer.  May be %NULL if @outlen is 0.
 * @outlen: Length of response buffer, in bytes.  If the actual
 *	response is longer than @outlen & ~3, it will be truncated
 *	to that length.
 * @outlen_actual: Pointer through which to return the actual response
 *	length.  May be %NULL if this is not needed.
 *
 * This function may sleep and therefore must be called in an appropriate
 * context.
 *
 * Return: A negative error code, or zero if successful.  The error
 *	code may come from the MCDI response or may indicate a failure
 *	to communicate with the MC.  In the former case, the response
 *	will still be copied to @outbuf and *@outlen_actual will be
 *	set accordingly.  In the latter case, *@outlen_actual will be
 *	set to zero.
 */
int efx_mcdi_rpc(struct efx_nic *efx, unsigned cmd,
		 const efx_dword_t *inbuf, size_t inlen,
		 efx_dword_t *outbuf, size_t outlen,
		 size_t *outlen_actual)
{
	return _efx_mcdi_rpc_evb_retry(efx, cmd, inbuf, inlen, outbuf, outlen,
				       outlen_actual, false);
}

/* Normally, on receiving an error code in the MCDI response,
 * efx_mcdi_rpc will log an error message containing (among other
 * things) the raw error code, by means of efx_mcdi_display_error.
 * This _quiet version suppresses that; if the caller wishes to log
 * the error conditionally on the return code, it should call this
 * function and is then responsible for calling efx_mcdi_display_error
 * as needed.
 */
int efx_mcdi_rpc_quiet(struct efx_nic *efx, unsigned cmd,
		       const efx_dword_t *inbuf, size_t inlen,
		       efx_dword_t *outbuf, size_t outlen,
		       size_t *outlen_actual)
{
	return _efx_mcdi_rpc_evb_retry(efx, cmd, inbuf, inlen, outbuf, outlen,
				       outlen_actual, true);
}

int efx_mcdi_rpc_start(struct efx_nic *efx, unsigned cmd,
		       const efx_dword_t *inbuf, size_t inlen)
{
	struct efx_mcdi_iface *mcdi = efx_mcdi(efx);
	int rc;

	rc = efx_mcdi_check_supported(efx, cmd, inlen);
	if (rc)
		return rc;

	if (efx->mc_bist_for_other_fn)
		return -ENETDOWN;

	if (mcdi->mode == MCDI_MODE_FAIL)
		return -ENETDOWN;

	efx_mcdi_acquire_sync(mcdi);
	efx_mcdi_send_request(efx, cmd, inbuf, inlen);
	return 0;
}

static int _efx_mcdi_rpc_async(struct efx_nic *efx, unsigned int cmd,
			       const efx_dword_t *inbuf, size_t inlen,
			       size_t outlen,
			       efx_mcdi_async_completer *complete,
			       unsigned long cookie, bool quiet)
{
	struct efx_mcdi_iface *mcdi = efx_mcdi(efx);
	struct efx_mcdi_async_param *async;
	int rc;

	rc = efx_mcdi_check_supported(efx, cmd, inlen);
	if (rc)
		return rc;

	if (efx->mc_bist_for_other_fn)
		return -ENETDOWN;

	async = kmalloc(sizeof(*async) + ALIGN(max(inlen, outlen), 4),
			GFP_ATOMIC);
	if (!async)
		return -ENOMEM;

	async->cmd = cmd;
	async->inlen = inlen;
	async->outlen = outlen;
	async->quiet = quiet;
	async->complete = complete;
	async->cookie = cookie;
	memcpy(async + 1, inbuf, inlen);

	spin_lock_bh(&mcdi->async_lock);

	if (mcdi->mode == MCDI_MODE_EVENTS) {
		list_add_tail(&async->list, &mcdi->async_list);

		/* If this is at the front of the queue, try to start it
		 * immediately
		 */
		if (mcdi->async_list.next == &async->list &&
		    efx_mcdi_acquire_async(mcdi)) {
			efx_mcdi_send_request(efx, cmd, inbuf, inlen);
			mod_timer(&mcdi->async_timer,
				  jiffies + MCDI_RPC_TIMEOUT);
		}
	} else {
		kfree(async);
		rc = -ENETDOWN;
	}

	spin_unlock_bh(&mcdi->async_lock);

	return rc;
}

/**
 * efx_mcdi_rpc_async - Schedule an MCDI command to run asynchronously
 * @efx: NIC through which to issue the command
 * @cmd: Command type number
 * @inbuf: Command parameters
 * @inlen: Length of command parameters, in bytes
 * @outlen: Length to allocate for response buffer, in bytes
 * @complete: Function to be called on completion or cancellation.
 * @cookie: Arbitrary value to be passed to @complete.
 *
 * This function does not sleep and therefore may be called in atomic
 * context.  It will fail if event queues are disabled or if MCDI
 * event completions have been disabled due to an error.
 *
 * If it succeeds, the @complete function will be called exactly once
 * in atomic context, when one of the following occurs:
 * (a) the completion event is received (in NAPI context)
 * (b) event queues are disabled (in the process that disables them)
 * (c) the request times-out (in timer context)
 */
int
efx_mcdi_rpc_async(struct efx_nic *efx, unsigned int cmd,
		   const efx_dword_t *inbuf, size_t inlen, size_t outlen,
		   efx_mcdi_async_completer *complete, unsigned long cookie)
{
	return _efx_mcdi_rpc_async(efx, cmd, inbuf, inlen, outlen, complete,
				   cookie, false);
}

int efx_mcdi_rpc_async_quiet(struct efx_nic *efx, unsigned int cmd,
			     const efx_dword_t *inbuf, size_t inlen,
			     size_t outlen, efx_mcdi_async_completer *complete,
			     unsigned long cookie)
{
	return _efx_mcdi_rpc_async(efx, cmd, inbuf, inlen, outlen, complete,
				   cookie, true);
}

int efx_mcdi_rpc_finish(struct efx_nic *efx, unsigned cmd, size_t inlen,
			efx_dword_t *outbuf, size_t outlen,
			size_t *outlen_actual)
{
	return _efx_mcdi_rpc_finish(efx, cmd, inlen, outbuf, outlen,
				    outlen_actual, false, NULL, NULL);
}

int efx_mcdi_rpc_finish_quiet(struct efx_nic *efx, unsigned cmd, size_t inlen,
			      efx_dword_t *outbuf, size_t outlen,
			      size_t *outlen_actual)
{
	return _efx_mcdi_rpc_finish(efx, cmd, inlen, outbuf, outlen,
				    outlen_actual, true, NULL, NULL);
}

void efx_mcdi_display_error(struct efx_nic *efx, unsigned cmd,
			    size_t inlen, efx_dword_t *outbuf,
			    size_t outlen, int rc)
{
	int code = 0, err_arg = 0;

	if (outlen >= MC_CMD_ERR_CODE_OFST + 4)
		code = MCDI_DWORD(outbuf, ERR_CODE);
	if (outlen >= MC_CMD_ERR_ARG_OFST + 4)
		err_arg = MCDI_DWORD(outbuf, ERR_ARG);
	netif_cond_dbg(efx, hw, efx->net_dev, rc == -EPERM, err,
		       "MC command 0x%x inlen %zu failed rc=%d (raw=%d) arg=%d\n",
		       cmd, inlen, rc, code, err_arg);
}

/* Switch to polled MCDI completions.  This can be called in various
 * error conditions with various locks held, so it must be lockless.
 * Caller is responsible for flushing asynchronous requests later.
 */
void efx_mcdi_mode_poll(struct efx_nic *efx)
{
	struct efx_mcdi_iface *mcdi;

	if (!efx->mcdi)
		return;

	mcdi = efx_mcdi(efx);
	/* If already in polling mode, nothing to do.
	 * If in fail-fast state, don't switch to polled completion.
	 * FLR recovery will do that later.
	 */
	if (mcdi->mode == MCDI_MODE_POLL || mcdi->mode == MCDI_MODE_FAIL)
		return;

	/* We can switch from event completion to polled completion, because
	 * mcdi requests are always completed in shared memory. We do this by
	 * switching the mode to POLL'd then completing the request.
	 * efx_mcdi_await_completion() will then call efx_mcdi_poll().
	 *
	 * We need an smp_wmb() to synchronise with efx_mcdi_await_completion(),
	 * which efx_mcdi_complete_sync() provides for us.
	 */
	mcdi->mode = MCDI_MODE_POLL;

	efx_mcdi_complete_sync(mcdi);
}

/* Flush any running or queued asynchronous requests, after event processing
 * is stopped
 */
void efx_mcdi_flush_async(struct efx_nic *efx)
{
	struct efx_mcdi_async_param *async, *next;
	struct efx_mcdi_iface *mcdi;

	if (!efx->mcdi)
		return;

	mcdi = efx_mcdi(efx);

	/* We must be in poll or fail mode so no more requests can be queued */
	BUG_ON(mcdi->mode == MCDI_MODE_EVENTS);

	del_timer_sync(&mcdi->async_timer);

	/* If a request is still running, make sure we give the MC
	 * time to complete it so that the response won't overwrite our
	 * next request.
	 */
	if (mcdi->state == MCDI_STATE_RUNNING_ASYNC) {
		efx_mcdi_poll(efx);
		mcdi->state = MCDI_STATE_QUIESCENT;
	}

	/* Nothing else will access the async list now, so it is safe
	 * to walk it without holding async_lock.  If we hold it while
	 * calling a completer then lockdep may warn that we have
	 * acquired locks in the wrong order.
	 */
	list_for_each_entry_safe(async, next, &mcdi->async_list, list) {
		if (async->complete)
			async->complete(efx, async->cookie, -ENETDOWN, NULL, 0);
		list_del(&async->list);
		kfree(async);
	}
}

void efx_mcdi_mode_event(struct efx_nic *efx)
{
	struct efx_mcdi_iface *mcdi;

	if (!efx->mcdi)
		return;

	mcdi = efx_mcdi(efx);
	/* If already in event completion mode, nothing to do.
	 * If in fail-fast state, don't switch to event completion.  FLR
	 * recovery will do that later.
	 */
	if (mcdi->mode == MCDI_MODE_EVENTS || mcdi->mode == MCDI_MODE_FAIL)
		return;

	/* We can't switch from polled to event completion in the middle of a
	 * request, because the completion method is specified in the request.
	 * So acquire the interface to serialise the requestors. We don't need
	 * to acquire the iface_lock to change the mode here, but we do need a
	 * write memory barrier ensure that efx_mcdi_rpc() sees it, which
	 * efx_mcdi_acquire() provides.
	 */
	efx_mcdi_acquire_sync(mcdi);
	mcdi->mode = MCDI_MODE_EVENTS;
	efx_mcdi_release(mcdi);
}

static void efx_mcdi_ev_death(struct efx_nic *efx, int rc)
{
	struct efx_mcdi_iface *mcdi = efx_mcdi(efx);

	/* If there is an outstanding MCDI request, it has been terminated
	 * either by a BADASSERT or REBOOT event. If the mcdi interface is
	 * in polled mode, then do nothing because the MC reboot handler will
	 * set the header correctly. However, if the mcdi interface is waiting
	 * for a CMDDONE event it won't receive it [and since all MCDI events
	 * are sent to the same queue, we can't be racing with
	 * efx_mcdi_ev_cpl()]
	 *
	 * If there is an outstanding asynchronous request, we can't
	 * complete it now (efx_mcdi_complete() would deadlock).  The
	 * reset process will take care of this.
	 *
	 * There's a race here with efx_mcdi_send_request(), because
	 * we might receive a REBOOT event *before* the request has
	 * been copied out. In polled mode (during startup) this is
	 * irrelevant, because efx_mcdi_complete_sync() is ignored. In
	 * event mode, this condition is just an edge-case of
	 * receiving a REBOOT event after posting the MCDI
	 * request. Did the mc reboot before or after the copyout? The
	 * best we can do always is just return failure.
	 *
	 * If there is an outstanding proxy response expected it is not going
	 * to arrive. We should thus abort it.
	 */
	spin_lock(&mcdi->iface_lock);
	efx_mcdi_proxy_abort(mcdi);

	if (efx_mcdi_complete_sync(mcdi)) {
		if (mcdi->mode == MCDI_MODE_EVENTS) {
			mcdi->resprc = rc;
			mcdi->resp_hdr_len = 0;
			mcdi->resp_data_len = 0;
			++mcdi->credits;
		}
	} else {
		int count;

		/* Consume the status word since efx_mcdi_rpc_finish() won't */
		for (count = 0; count < MCDI_STATUS_DELAY_COUNT; ++count) {
			rc = efx_mcdi_poll_reboot(efx);
			if (rc)
				break;
			udelay(MCDI_STATUS_DELAY_US);
		}

		/* On EF10, a CODE_MC_REBOOT event can be received without the
		 * reboot detection in efx_mcdi_poll_reboot() being triggered.
		 * If zero was returned from the final call to
		 * efx_mcdi_poll_reboot(), the MC reboot wasn't noticed but the
		 * MC has definitely rebooted so prepare for the reset.
		 */
		if (!rc && efx->type->mcdi_reboot_detected)
			efx->type->mcdi_reboot_detected(efx);

		mcdi->new_epoch = true;

		/* Nobody was waiting for an MCDI request, so trigger a reset */
		efx_schedule_reset(efx, RESET_TYPE_MC_FAILURE);
	}

	spin_unlock(&mcdi->iface_lock);
}

/* The MC is going down in to BIST mode. set the BIST flag to block
 * new MCDI, cancel any outstanding MCDI and and schedule a BIST-type reset
 * (which doesn't actually execute a reset, it waits for the controlling
 * function to reset it).
 */
static void efx_mcdi_ev_bist(struct efx_nic *efx)
{
	struct efx_mcdi_iface *mcdi = efx_mcdi(efx);

	spin_lock(&mcdi->iface_lock);
	efx->mc_bist_for_other_fn = true;
	efx_mcdi_proxy_abort(mcdi);

	if (efx_mcdi_complete_sync(mcdi)) {
		if (mcdi->mode == MCDI_MODE_EVENTS) {
			mcdi->resprc = -EIO;
			mcdi->resp_hdr_len = 0;
			mcdi->resp_data_len = 0;
			++mcdi->credits;
		}
	}
	mcdi->new_epoch = true;
	efx_schedule_reset(efx, RESET_TYPE_MC_BIST);
	spin_unlock(&mcdi->iface_lock);
}

/* MCDI timeouts seen, so make all MCDI calls fail-fast and issue an FLR to try
 * to recover.
 */
static void efx_mcdi_abandon(struct efx_nic *efx)
{
	struct efx_mcdi_iface *mcdi = efx_mcdi(efx);

	if (xchg(&mcdi->mode, MCDI_MODE_FAIL) == MCDI_MODE_FAIL)
		return; /* it had already been done */
	netif_dbg(efx, hw, efx->net_dev, "MCDI is timing out; trying to recover\n");
	efx_schedule_reset(efx, RESET_TYPE_MCDI_TIMEOUT);
}

static void efx_handle_drain_event(struct efx_nic *efx)
{
	if (atomic_dec_and_test(&efx->active_queues))
		wake_up(&efx->flush_wq);

	WARN_ON(atomic_read(&efx->active_queues) < 0);
}

/* Called from efx_farch_ev_process and efx_ef10_ev_process for MCDI events */
void efx_mcdi_process_event(struct efx_channel *channel,
			    efx_qword_t *event)
{
	struct efx_nic *efx = channel->efx;
	int code = EFX_QWORD_FIELD(*event, MCDI_EVENT_CODE);
	u32 data = EFX_QWORD_FIELD(*event, MCDI_EVENT_DATA);

	switch (code) {
	case MCDI_EVENT_CODE_BADSSERT:
		netif_err(efx, hw, efx->net_dev,
			  "MC watchdog or assertion failure at 0x%x\n", data);
		efx_mcdi_ev_death(efx, -EINTR);
		break;

	case MCDI_EVENT_CODE_PMNOTICE:
		netif_info(efx, wol, efx->net_dev, "MCDI PM event.\n");
		break;

	case MCDI_EVENT_CODE_CMDDONE:
		efx_mcdi_ev_cpl(efx,
				MCDI_EVENT_FIELD(*event, CMDDONE_SEQ),
				MCDI_EVENT_FIELD(*event, CMDDONE_DATALEN),
				MCDI_EVENT_FIELD(*event, CMDDONE_ERRNO));
		break;

	case MCDI_EVENT_CODE_LINKCHANGE:
		efx_mcdi_process_link_change(efx, event);
		break;
	case MCDI_EVENT_CODE_SENSOREVT:
		efx_sensor_event(efx, event);
		break;
	case MCDI_EVENT_CODE_SCHEDERR:
		netif_dbg(efx, hw, efx->net_dev,
			  "MC Scheduler alert (0x%x)\n", data);
		break;
	case MCDI_EVENT_CODE_REBOOT:
	case MCDI_EVENT_CODE_MC_REBOOT:
		netif_info(efx, hw, efx->net_dev, "MC Reboot\n");
		efx_mcdi_ev_death(efx, -EIO);
		break;
	case MCDI_EVENT_CODE_MC_BIST:
		netif_info(efx, hw, efx->net_dev, "MC entered BIST mode\n");
		efx_mcdi_ev_bist(efx);
		break;
	case MCDI_EVENT_CODE_MAC_STATS_DMA:
		/* MAC stats are gather lazily.  We can ignore this. */
		break;
	case MCDI_EVENT_CODE_FLR:
		if (efx->type->sriov_flr)
			efx->type->sriov_flr(efx,
					     MCDI_EVENT_FIELD(*event, FLR_VF));
		break;
	case MCDI_EVENT_CODE_PTP_RX:
	case MCDI_EVENT_CODE_PTP_FAULT:
	case MCDI_EVENT_CODE_PTP_PPS:
		efx_ptp_event(efx, event);
		break;
	case MCDI_EVENT_CODE_PTP_TIME:
		efx_time_sync_event(channel, event);
		break;
	case MCDI_EVENT_CODE_TX_FLUSH:
	case MCDI_EVENT_CODE_RX_FLUSH:
		/* Two flush events will be sent: one to the same event
		 * queue as completions, and one to event queue 0.
		 * In the latter case the {RX,TX}_FLUSH_TO_DRIVER
		 * flag will be set, and we should ignore the event
		 * because we want to wait for all completions.
		 */
		BUILD_BUG_ON(MCDI_EVENT_TX_FLUSH_TO_DRIVER_LBN !=
			     MCDI_EVENT_RX_FLUSH_TO_DRIVER_LBN);
		if (!MCDI_EVENT_FIELD(*event, TX_FLUSH_TO_DRIVER))
			efx_handle_drain_event(efx);
		break;
	case MCDI_EVENT_CODE_TX_ERR:
	case MCDI_EVENT_CODE_RX_ERR:
		netif_err(efx, hw, efx->net_dev,
			  "%s DMA error (event: "EFX_QWORD_FMT")\n",
			  code == MCDI_EVENT_CODE_TX_ERR ? "TX" : "RX",
			  EFX_QWORD_VAL(*event));
		efx_schedule_reset(efx, RESET_TYPE_DMA_ERROR);
		break;
	case MCDI_EVENT_CODE_PROXY_RESPONSE:
		efx_mcdi_ev_proxy_response(efx,
				MCDI_EVENT_FIELD(*event, PROXY_RESPONSE_HANDLE),
				MCDI_EVENT_FIELD(*event, PROXY_RESPONSE_RC));
		break;
	default:
		netif_err(efx, hw, efx->net_dev,
			  "Unknown MCDI event " EFX_QWORD_FMT "\n",
			  EFX_QWORD_VAL(*event));
	}
}

/**************************************************************************
 *
 * Specific request functions
 *
 **************************************************************************
 */

void efx_mcdi_print_fwver(struct efx_nic *efx, char *buf, size_t len)
{
	MCDI_DECLARE_BUF(outbuf, MC_CMD_GET_VERSION_OUT_LEN);
	size_t outlength;
	const __le16 *ver_words;
	size_t offset;
	int rc;

	BUILD_BUG_ON(MC_CMD_GET_VERSION_IN_LEN != 0);
	rc = efx_mcdi_rpc(efx, MC_CMD_GET_VERSION, NULL, 0,
			  outbuf, sizeof(outbuf), &outlength);
	if (rc)
		goto fail;
	if (outlength < MC_CMD_GET_VERSION_OUT_LEN) {
		rc = -EIO;
		goto fail;
	}

	ver_words = (__le16 *)MCDI_PTR(outbuf, GET_VERSION_OUT_VERSION);
	offset = scnprintf(buf, len, "%u.%u.%u.%u",
			   le16_to_cpu(ver_words[0]),
			   le16_to_cpu(ver_words[1]),
			   le16_to_cpu(ver_words[2]),
			   le16_to_cpu(ver_words[3]));

	if (efx->type->print_additional_fwver)
		offset += efx->type->print_additional_fwver(efx, buf + offset,
							    len - offset);

	/* It's theoretically possible for the string to exceed 31
	 * characters, though in practice the first three version
	 * components are short enough that this doesn't happen.
	 */
	if (WARN_ON(offset >= len))
		buf[0] = 0;

	return;

fail:
	netif_err(efx, probe, efx->net_dev, "%s: failed rc=%d\n", __func__, rc);
	buf[0] = 0;
}

static int efx_mcdi_drv_attach(struct efx_nic *efx, bool driver_operating,
			       bool *was_attached)
{
	MCDI_DECLARE_BUF(inbuf, MC_CMD_DRV_ATTACH_IN_LEN);
	MCDI_DECLARE_BUF(outbuf, MC_CMD_DRV_ATTACH_EXT_OUT_LEN);
	size_t outlen;
	int rc;

	MCDI_SET_DWORD(inbuf, DRV_ATTACH_IN_NEW_STATE,
		       driver_operating ? 1 : 0);
	MCDI_SET_DWORD(inbuf, DRV_ATTACH_IN_UPDATE, 1);
	MCDI_SET_DWORD(inbuf, DRV_ATTACH_IN_FIRMWARE_ID, MC_CMD_FW_LOW_LATENCY);

	rc = efx_mcdi_rpc_quiet(efx, MC_CMD_DRV_ATTACH, inbuf, sizeof(inbuf),
				outbuf, sizeof(outbuf), &outlen);
	/* If we're not the primary PF, trying to ATTACH with a FIRMWARE_ID
	 * specified will fail with EPERM, and we have to tell the MC we don't
	 * care what firmware we get.
	 */
	if (rc == -EPERM) {
		netif_dbg(efx, probe, efx->net_dev,
			  "efx_mcdi_drv_attach with fw-variant setting failed EPERM, trying without it\n");
		MCDI_SET_DWORD(inbuf, DRV_ATTACH_IN_FIRMWARE_ID,
			       MC_CMD_FW_DONT_CARE);
		rc = efx_mcdi_rpc_quiet(efx, MC_CMD_DRV_ATTACH, inbuf,
					sizeof(inbuf), outbuf, sizeof(outbuf),
					&outlen);
	}
	if (rc) {
		efx_mcdi_display_error(efx, MC_CMD_DRV_ATTACH, sizeof(inbuf),
				       outbuf, outlen, rc);
		goto fail;
	}
	if (outlen < MC_CMD_DRV_ATTACH_OUT_LEN) {
		rc = -EIO;
		goto fail;
	}

	if (driver_operating) {
		if (outlen >= MC_CMD_DRV_ATTACH_EXT_OUT_LEN) {
			efx->mcdi->fn_flags =
				MCDI_DWORD(outbuf,
					   DRV_ATTACH_EXT_OUT_FUNC_FLAGS);
		} else {
			/* Synthesise flags for Siena */
			efx->mcdi->fn_flags =
				1 << MC_CMD_DRV_ATTACH_EXT_OUT_FLAG_LINKCTRL |
				1 << MC_CMD_DRV_ATTACH_EXT_OUT_FLAG_TRUSTED |
				(efx_port_num(efx) == 0) <<
				MC_CMD_DRV_ATTACH_EXT_OUT_FLAG_PRIMARY;
		}
	}

	/* We currently assume we have control of the external link
	 * and are completely trusted by firmware.  Abort probing
	 * if that's not true for this function.
	 */

	if (was_attached != NULL)
		*was_attached = MCDI_DWORD(outbuf, DRV_ATTACH_OUT_OLD_STATE);
	return 0;

fail:
	netif_err(efx, probe, efx->net_dev, "%s: failed rc=%d\n", __func__, rc);
	return rc;
}

int efx_mcdi_get_board_cfg(struct efx_nic *efx, u8 *mac_address,
			   u16 *fw_subtype_list, u32 *capabilities)
{
	MCDI_DECLARE_BUF(outbuf, MC_CMD_GET_BOARD_CFG_OUT_LENMAX);
	size_t outlen, i;
	int port_num = efx_port_num(efx);
	int rc;

	BUILD_BUG_ON(MC_CMD_GET_BOARD_CFG_IN_LEN != 0);
	/* we need __aligned(2) for ether_addr_copy */
	BUILD_BUG_ON(MC_CMD_GET_BOARD_CFG_OUT_MAC_ADDR_BASE_PORT0_OFST & 1);
	BUILD_BUG_ON(MC_CMD_GET_BOARD_CFG_OUT_MAC_ADDR_BASE_PORT1_OFST & 1);

	rc = efx_mcdi_rpc(efx, MC_CMD_GET_BOARD_CFG, NULL, 0,
			  outbuf, sizeof(outbuf), &outlen);
	if (rc)
		goto fail;

	if (outlen < MC_CMD_GET_BOARD_CFG_OUT_LENMIN) {
		rc = -EIO;
		goto fail;
	}

	if (mac_address)
		ether_addr_copy(mac_address,
				port_num ?
				MCDI_PTR(outbuf, GET_BOARD_CFG_OUT_MAC_ADDR_BASE_PORT1) :
				MCDI_PTR(outbuf, GET_BOARD_CFG_OUT_MAC_ADDR_BASE_PORT0));
	if (fw_subtype_list) {
		for (i = 0;
		     i < MCDI_VAR_ARRAY_LEN(outlen,
					    GET_BOARD_CFG_OUT_FW_SUBTYPE_LIST);
		     i++)
			fw_subtype_list[i] = MCDI_ARRAY_WORD(
				outbuf, GET_BOARD_CFG_OUT_FW_SUBTYPE_LIST, i);
		for (; i < MC_CMD_GET_BOARD_CFG_OUT_FW_SUBTYPE_LIST_MAXNUM; i++)
			fw_subtype_list[i] = 0;
	}
	if (capabilities) {
		if (port_num)
			*capabilities = MCDI_DWORD(outbuf,
					GET_BOARD_CFG_OUT_CAPABILITIES_PORT1);
		else
			*capabilities = MCDI_DWORD(outbuf,
					GET_BOARD_CFG_OUT_CAPABILITIES_PORT0);
	}

	return 0;

fail:
	netif_err(efx, hw, efx->net_dev, "%s: failed rc=%d len=%d\n",
		  __func__, rc, (int)outlen);

	return rc;
}

int efx_mcdi_log_ctrl(struct efx_nic *efx, bool evq, bool uart, u32 dest_evq)
{
	MCDI_DECLARE_BUF(inbuf, MC_CMD_LOG_CTRL_IN_LEN);
	u32 dest = 0;
	int rc;

	if (uart)
		dest |= MC_CMD_LOG_CTRL_IN_LOG_DEST_UART;
	if (evq)
		dest |= MC_CMD_LOG_CTRL_IN_LOG_DEST_EVQ;

	MCDI_SET_DWORD(inbuf, LOG_CTRL_IN_LOG_DEST, dest);
	MCDI_SET_DWORD(inbuf, LOG_CTRL_IN_LOG_DEST_EVQ, dest_evq);

	BUILD_BUG_ON(MC_CMD_LOG_CTRL_OUT_LEN != 0);

	rc = efx_mcdi_rpc(efx, MC_CMD_LOG_CTRL, inbuf, sizeof(inbuf),
			  NULL, 0, NULL);
	return rc;
}

int efx_mcdi_nvram_types(struct efx_nic *efx, u32 *nvram_types_out)
{
	MCDI_DECLARE_BUF(outbuf, MC_CMD_NVRAM_TYPES_OUT_LEN);
	size_t outlen;
	int rc;

	BUILD_BUG_ON(MC_CMD_NVRAM_TYPES_IN_LEN != 0);

	rc = efx_mcdi_rpc(efx, MC_CMD_NVRAM_TYPES, NULL, 0,
			  outbuf, sizeof(outbuf), &outlen);
	if (rc)
		goto fail;
	if (outlen < MC_CMD_NVRAM_TYPES_OUT_LEN) {
		rc = -EIO;
		goto fail;
	}

	*nvram_types_out = MCDI_DWORD(outbuf, NVRAM_TYPES_OUT_TYPES);
	return 0;

fail:
	netif_err(efx, hw, efx->net_dev, "%s: failed rc=%d\n",
		  __func__, rc);
	return rc;
}

/* This function finds types using the new NVRAM_PARTITIONS mcdi. */
static int efx_new_mcdi_nvram_types(struct efx_nic *efx, u32 *number,
				    u32 *nvram_types)
{
	efx_dword_t *outbuf = kzalloc(MC_CMD_NVRAM_PARTITIONS_OUT_LENMAX_MCDI2,
				      GFP_KERNEL);
	size_t outlen;
	int rc;

	if (!outbuf)
		return -ENOMEM;

	BUILD_BUG_ON(MC_CMD_NVRAM_PARTITIONS_IN_LEN != 0);

	rc = efx_mcdi_rpc(efx, MC_CMD_NVRAM_PARTITIONS, NULL, 0,
			  outbuf, MC_CMD_NVRAM_PARTITIONS_OUT_LENMAX_MCDI2, &outlen);
	if (rc)
		goto fail;

	*number = MCDI_DWORD(outbuf, NVRAM_PARTITIONS_OUT_NUM_PARTITIONS);

	memcpy(nvram_types, MCDI_PTR(outbuf, NVRAM_PARTITIONS_OUT_TYPE_ID),
	       *number * sizeof(u32));

fail:
	kfree(outbuf);
	return rc;
}

int efx_mcdi_nvram_info(struct efx_nic *efx, unsigned int type,
			size_t *size_out, size_t *erase_size_out,
			bool *protected_out)
{
	MCDI_DECLARE_BUF(inbuf, MC_CMD_NVRAM_INFO_IN_LEN);
	MCDI_DECLARE_BUF(outbuf, MC_CMD_NVRAM_INFO_OUT_LEN);
	size_t outlen;
	int rc;

	MCDI_SET_DWORD(inbuf, NVRAM_INFO_IN_TYPE, type);

	rc = efx_mcdi_rpc(efx, MC_CMD_NVRAM_INFO, inbuf, sizeof(inbuf),
			  outbuf, sizeof(outbuf), &outlen);
	if (rc)
		goto fail;
	if (outlen < MC_CMD_NVRAM_INFO_OUT_LEN) {
		rc = -EIO;
		goto fail;
	}

	*size_out = MCDI_DWORD(outbuf, NVRAM_INFO_OUT_SIZE);
	*erase_size_out = MCDI_DWORD(outbuf, NVRAM_INFO_OUT_ERASESIZE);
	*protected_out = !!(MCDI_DWORD(outbuf, NVRAM_INFO_OUT_FLAGS) &
				(1 << MC_CMD_NVRAM_INFO_OUT_PROTECTED_LBN));
	return 0;

fail:
	netif_err(efx, hw, efx->net_dev, "%s: failed rc=%d\n", __func__, rc);
	return rc;
}

static int efx_mcdi_nvram_test(struct efx_nic *efx, unsigned int type)
{
	MCDI_DECLARE_BUF(inbuf, MC_CMD_NVRAM_TEST_IN_LEN);
	MCDI_DECLARE_BUF(outbuf, MC_CMD_NVRAM_TEST_OUT_LEN);
	int rc;

	MCDI_SET_DWORD(inbuf, NVRAM_TEST_IN_TYPE, type);

	rc = efx_mcdi_rpc(efx, MC_CMD_NVRAM_TEST, inbuf, sizeof(inbuf),
			  outbuf, sizeof(outbuf), NULL);
	if (rc)
		return rc;

	switch (MCDI_DWORD(outbuf, NVRAM_TEST_OUT_RESULT)) {
	case MC_CMD_NVRAM_TEST_PASS:
	case MC_CMD_NVRAM_TEST_NOTSUPP:
		return 0;
	default:
		return -EIO;
	}
}

/* This function tests nvram partitions using the new mcdi partition lookup scheme */
int efx_new_mcdi_nvram_test_all(struct efx_nic *efx)
{
	u32 *nvram_types = kzalloc(MC_CMD_NVRAM_PARTITIONS_OUT_LENMAX_MCDI2,
				   GFP_KERNEL);
	unsigned int number;
	int rc, i;

	if (!nvram_types)
		return -ENOMEM;

	rc = efx_new_mcdi_nvram_types(efx, &number, nvram_types);
	if (rc)
		goto fail;

	/* Require at least one check */
	rc = -EAGAIN;

	for (i = 0; i < number; i++) {
		if (nvram_types[i] == NVRAM_PARTITION_TYPE_PARTITION_MAP ||
		    nvram_types[i] == NVRAM_PARTITION_TYPE_DYNAMIC_CONFIG)
			continue;

		rc = efx_mcdi_nvram_test(efx, nvram_types[i]);
		if (rc)
			goto fail;
	}

fail:
	kfree(nvram_types);
	return rc;
}

int efx_mcdi_nvram_test_all(struct efx_nic *efx)
{
	u32 nvram_types;
	unsigned int type;
	int rc;

	rc = efx_mcdi_nvram_types(efx, &nvram_types);
	if (rc)
		goto fail1;

	type = 0;
	while (nvram_types != 0) {
		if (nvram_types & 1) {
			rc = efx_mcdi_nvram_test(efx, type);
			if (rc)
				goto fail2;
		}
		type++;
		nvram_types >>= 1;
	}

	return 0;

fail2:
	netif_err(efx, hw, efx->net_dev, "%s: failed type=%u\n",
		  __func__, type);
fail1:
	netif_err(efx, hw, efx->net_dev, "%s: failed rc=%d\n", __func__, rc);
	return rc;
}

/* Returns 1 if an assertion was read, 0 if no assertion had fired,
 * negative on error.
 */
static int efx_mcdi_read_assertion(struct efx_nic *efx)
{
	MCDI_DECLARE_BUF(inbuf, MC_CMD_GET_ASSERTS_IN_LEN);
	MCDI_DECLARE_BUF(outbuf, MC_CMD_GET_ASSERTS_OUT_LEN);
	unsigned int flags, index;
	const char *reason;
	size_t outlen;
	int retry;
	int rc;

	/* Attempt to read any stored assertion state before we reboot
	 * the mcfw out of the assertion handler. Retry twice, once
	 * because a boot-time assertion might cause this command to fail
	 * with EINTR. And once again because GET_ASSERTS can race with
	 * MC_CMD_REBOOT running on the other port. */
	retry = 2;
	do {
		MCDI_SET_DWORD(inbuf, GET_ASSERTS_IN_CLEAR, 1);
		rc = efx_mcdi_rpc_quiet(efx, MC_CMD_GET_ASSERTS,
					inbuf, MC_CMD_GET_ASSERTS_IN_LEN,
					outbuf, sizeof(outbuf), &outlen);
		if (rc == -EPERM)
			return 0;
	} while ((rc == -EINTR || rc == -EIO) && retry-- > 0);

	if (rc) {
		efx_mcdi_display_error(efx, MC_CMD_GET_ASSERTS,
				       MC_CMD_GET_ASSERTS_IN_LEN, outbuf,
				       outlen, rc);
		return rc;
	}
	if (outlen < MC_CMD_GET_ASSERTS_OUT_LEN)
		return -EIO;

	/* Print out any recorded assertion state */
	flags = MCDI_DWORD(outbuf, GET_ASSERTS_OUT_GLOBAL_FLAGS);
	if (flags == MC_CMD_GET_ASSERTS_FLAGS_NO_FAILS)
		return 0;

	reason = (flags == MC_CMD_GET_ASSERTS_FLAGS_SYS_FAIL)
		? "system-level assertion"
		: (flags == MC_CMD_GET_ASSERTS_FLAGS_THR_FAIL)
		? "thread-level assertion"
		: (flags == MC_CMD_GET_ASSERTS_FLAGS_WDOG_FIRED)
		? "watchdog reset"
		: "unknown assertion";
	netif_err(efx, hw, efx->net_dev,
		  "MCPU %s at PC = 0x%.8x in thread 0x%.8x\n", reason,
		  MCDI_DWORD(outbuf, GET_ASSERTS_OUT_SAVED_PC_OFFS),
		  MCDI_DWORD(outbuf, GET_ASSERTS_OUT_THREAD_OFFS));

	/* Print out the registers */
	for (index = 0;
	     index < MC_CMD_GET_ASSERTS_OUT_GP_REGS_OFFS_NUM;
	     index++)
		netif_err(efx, hw, efx->net_dev, "R%.2d (?): 0x%.8x\n",
			  1 + index,
			  MCDI_ARRAY_DWORD(outbuf, GET_ASSERTS_OUT_GP_REGS_OFFS,
					   index));

	return 1;
}

static int efx_mcdi_exit_assertion(struct efx_nic *efx)
{
	MCDI_DECLARE_BUF(inbuf, MC_CMD_REBOOT_IN_LEN);
	int rc;

	/* If the MC is running debug firmware, it might now be
	 * waiting for a debugger to attach, but we just want it to
	 * reboot.  We set a flag that makes the command a no-op if it
	 * has already done so.
	 * The MCDI will thus return either 0 or -EIO.
	 */
	BUILD_BUG_ON(MC_CMD_REBOOT_OUT_LEN != 0);
	MCDI_SET_DWORD(inbuf, REBOOT_IN_FLAGS,
		       MC_CMD_REBOOT_FLAGS_AFTER_ASSERTION);
	rc = efx_mcdi_rpc_quiet(efx, MC_CMD_REBOOT, inbuf, MC_CMD_REBOOT_IN_LEN,
				NULL, 0, NULL);
	if (rc == -EIO)
		rc = 0;
	if (rc)
		efx_mcdi_display_error(efx, MC_CMD_REBOOT, MC_CMD_REBOOT_IN_LEN,
				       NULL, 0, rc);
	return rc;
}

int efx_mcdi_handle_assertion(struct efx_nic *efx)
{
	int rc;

	rc = efx_mcdi_read_assertion(efx);
	if (rc <= 0)
		return rc;

	return efx_mcdi_exit_assertion(efx);
}

int efx_mcdi_set_id_led(struct efx_nic *efx, enum efx_led_mode mode)
{
	MCDI_DECLARE_BUF(inbuf, MC_CMD_SET_ID_LED_IN_LEN);

	BUILD_BUG_ON(EFX_LED_OFF != MC_CMD_LED_OFF);
	BUILD_BUG_ON(EFX_LED_ON != MC_CMD_LED_ON);
	BUILD_BUG_ON(EFX_LED_DEFAULT != MC_CMD_LED_DEFAULT);

	BUILD_BUG_ON(MC_CMD_SET_ID_LED_OUT_LEN != 0);

	MCDI_SET_DWORD(inbuf, SET_ID_LED_IN_STATE, mode);

	return efx_mcdi_rpc(efx, MC_CMD_SET_ID_LED, inbuf, sizeof(inbuf), NULL, 0, NULL);
}

static int efx_mcdi_reset_func(struct efx_nic *efx)
{
	MCDI_DECLARE_BUF(inbuf, MC_CMD_ENTITY_RESET_IN_LEN);
	int rc;

	BUILD_BUG_ON(MC_CMD_ENTITY_RESET_OUT_LEN != 0);
	MCDI_POPULATE_DWORD_1(inbuf, ENTITY_RESET_IN_FLAG,
			      ENTITY_RESET_IN_FUNCTION_RESOURCE_RESET, 1);
	rc = efx_mcdi_rpc(efx, MC_CMD_ENTITY_RESET, inbuf, sizeof(inbuf),
			  NULL, 0, NULL);
	return rc;
}

static int efx_mcdi_reset_mc(struct efx_nic *efx)
{
	MCDI_DECLARE_BUF(inbuf, MC_CMD_REBOOT_IN_LEN);
	int rc;

	BUILD_BUG_ON(MC_CMD_REBOOT_OUT_LEN != 0);
	MCDI_SET_DWORD(inbuf, REBOOT_IN_FLAGS, 0);
	rc = efx_mcdi_rpc(efx, MC_CMD_REBOOT, inbuf, sizeof(inbuf),
			  NULL, 0, NULL);
	/* White is black, and up is down */
	if (rc == -EIO)
		return 0;
	if (rc == 0)
		rc = -EIO;
	return rc;
}

enum reset_type efx_mcdi_map_reset_reason(enum reset_type reason)
{
	return RESET_TYPE_RECOVER_OR_ALL;
}

int efx_mcdi_reset(struct efx_nic *efx, enum reset_type method)
{
	int rc;

	/* If MCDI is down, we can't handle_assertion */
	if (method == RESET_TYPE_MCDI_TIMEOUT) {
		rc = pci_reset_function(efx->pci_dev);
		if (rc)
			return rc;
		/* Re-enable polled MCDI completion */
		if (efx->mcdi) {
			struct efx_mcdi_iface *mcdi = efx_mcdi(efx);
			mcdi->mode = MCDI_MODE_POLL;
		}
		return 0;
	}

	/* Recover from a failed assertion pre-reset */
	rc = efx_mcdi_handle_assertion(efx);
	if (rc)
		return rc;

	if (method == RESET_TYPE_DATAPATH)
		return 0;
	else if (method == RESET_TYPE_WORLD)
		return efx_mcdi_reset_mc(efx);
	else
		return efx_mcdi_reset_func(efx);
}

static int efx_mcdi_wol_filter_set(struct efx_nic *efx, u32 type,
				   const u8 *mac, int *id_out)
{
	MCDI_DECLARE_BUF(inbuf, MC_CMD_WOL_FILTER_SET_IN_LEN);
	MCDI_DECLARE_BUF(outbuf, MC_CMD_WOL_FILTER_SET_OUT_LEN);
	size_t outlen;
	int rc;

	MCDI_SET_DWORD(inbuf, WOL_FILTER_SET_IN_WOL_TYPE, type);
	MCDI_SET_DWORD(inbuf, WOL_FILTER_SET_IN_FILTER_MODE,
		       MC_CMD_FILTER_MODE_SIMPLE);
	ether_addr_copy(MCDI_PTR(inbuf, WOL_FILTER_SET_IN_MAGIC_MAC), mac);

	rc = efx_mcdi_rpc(efx, MC_CMD_WOL_FILTER_SET, inbuf, sizeof(inbuf),
			  outbuf, sizeof(outbuf), &outlen);
	if (rc)
		goto fail;

	if (outlen < MC_CMD_WOL_FILTER_SET_OUT_LEN) {
		rc = -EIO;
		goto fail;
	}

	*id_out = (int)MCDI_DWORD(outbuf, WOL_FILTER_SET_OUT_FILTER_ID);

	return 0;

fail:
	*id_out = -1;
	netif_err(efx, hw, efx->net_dev, "%s: failed rc=%d\n", __func__, rc);
	return rc;

}


int
efx_mcdi_wol_filter_set_magic(struct efx_nic *efx,  const u8 *mac, int *id_out)
{
	return efx_mcdi_wol_filter_set(efx, MC_CMD_WOL_TYPE_MAGIC, mac, id_out);
}


int efx_mcdi_wol_filter_get_magic(struct efx_nic *efx, int *id_out)
{
	MCDI_DECLARE_BUF(outbuf, MC_CMD_WOL_FILTER_GET_OUT_LEN);
	size_t outlen;
	int rc;

	rc = efx_mcdi_rpc(efx, MC_CMD_WOL_FILTER_GET, NULL, 0,
			  outbuf, sizeof(outbuf), &outlen);
	if (rc)
		goto fail;

	if (outlen < MC_CMD_WOL_FILTER_GET_OUT_LEN) {
		rc = -EIO;
		goto fail;
	}

	*id_out = (int)MCDI_DWORD(outbuf, WOL_FILTER_GET_OUT_FILTER_ID);

	return 0;

fail:
	*id_out = -1;
	netif_err(efx, hw, efx->net_dev, "%s: failed rc=%d\n", __func__, rc);
	return rc;
}


int efx_mcdi_wol_filter_remove(struct efx_nic *efx, int id)
{
	MCDI_DECLARE_BUF(inbuf, MC_CMD_WOL_FILTER_REMOVE_IN_LEN);
	int rc;

	MCDI_SET_DWORD(inbuf, WOL_FILTER_REMOVE_IN_FILTER_ID, (u32)id);

	rc = efx_mcdi_rpc(efx, MC_CMD_WOL_FILTER_REMOVE, inbuf, sizeof(inbuf),
			  NULL, 0, NULL);
	return rc;
}

int efx_mcdi_flush_rxqs(struct efx_nic *efx)
{
	struct efx_channel *channel;
	struct efx_rx_queue *rx_queue;
	MCDI_DECLARE_BUF(inbuf,
			 MC_CMD_FLUSH_RX_QUEUES_IN_LEN(EFX_MAX_CHANNELS));
	int rc, count;

	BUILD_BUG_ON(EFX_MAX_CHANNELS >
		     MC_CMD_FLUSH_RX_QUEUES_IN_QID_OFST_MAXNUM);

	count = 0;
	efx_for_each_channel(channel, efx) {
		efx_for_each_channel_rx_queue(rx_queue, channel) {
			if (rx_queue->flush_pending) {
				rx_queue->flush_pending = false;
				atomic_dec(&efx->rxq_flush_pending);
				MCDI_SET_ARRAY_DWORD(
					inbuf, FLUSH_RX_QUEUES_IN_QID_OFST,
					count, efx_rx_queue_index(rx_queue));
				count++;
			}
		}
	}

	rc = efx_mcdi_rpc(efx, MC_CMD_FLUSH_RX_QUEUES, inbuf,
			  MC_CMD_FLUSH_RX_QUEUES_IN_LEN(count), NULL, 0, NULL);
	WARN_ON(rc < 0);

	return rc;
}

int efx_mcdi_wol_filter_reset(struct efx_nic *efx)
{
	int rc;

	rc = efx_mcdi_rpc(efx, MC_CMD_WOL_FILTER_RESET, NULL, 0, NULL, 0, NULL);
	return rc;
}

int efx_mcdi_set_workaround(struct efx_nic *efx, u32 type, bool enabled,
			    unsigned int *flags)
{
	MCDI_DECLARE_BUF(inbuf, MC_CMD_WORKAROUND_IN_LEN);
	MCDI_DECLARE_BUF(outbuf, MC_CMD_WORKAROUND_EXT_OUT_LEN);
	size_t outlen;
	int rc;

	BUILD_BUG_ON(MC_CMD_WORKAROUND_OUT_LEN != 0);
	MCDI_SET_DWORD(inbuf, WORKAROUND_IN_TYPE, type);
	MCDI_SET_DWORD(inbuf, WORKAROUND_IN_ENABLED, enabled);
	rc = efx_mcdi_rpc(efx, MC_CMD_WORKAROUND, inbuf, sizeof(inbuf),
			  outbuf, sizeof(outbuf), &outlen);
	if (rc)
		return rc;

	if (!flags)
		return 0;

	if (outlen >= MC_CMD_WORKAROUND_EXT_OUT_LEN)
		*flags = MCDI_DWORD(outbuf, WORKAROUND_EXT_OUT_FLAGS);
	else
		*flags = 0;

	return 0;
}

int efx_mcdi_get_workarounds(struct efx_nic *efx, unsigned int *impl_out,
			     unsigned int *enabled_out)
{
	MCDI_DECLARE_BUF(outbuf, MC_CMD_GET_WORKAROUNDS_OUT_LEN);
	size_t outlen;
	int rc;

	rc = efx_mcdi_rpc(efx, MC_CMD_GET_WORKAROUNDS, NULL, 0,
			  outbuf, sizeof(outbuf), &outlen);
	if (rc)
		goto fail;

	if (outlen < MC_CMD_GET_WORKAROUNDS_OUT_LEN) {
		rc = -EIO;
		goto fail;
	}

	if (impl_out)
		*impl_out = MCDI_DWORD(outbuf, GET_WORKAROUNDS_OUT_IMPLEMENTED);

	if (enabled_out)
		*enabled_out = MCDI_DWORD(outbuf, GET_WORKAROUNDS_OUT_ENABLED);

	return 0;

fail:
	/* Older firmware lacks GET_WORKAROUNDS and this isn't especially
	 * terrifying.  The call site will have to deal with it though.
	 */
	netif_cond_dbg(efx, hw, efx->net_dev, rc == -ENOSYS, err,
		       "%s: failed rc=%d\n", __func__, rc);
	return rc;
}

#ifdef CONFIG_SFC_MTD

#define EFX_MCDI_NVRAM_LEN_MAX 128

static int efx_mcdi_nvram_update_start(struct efx_nic *efx, unsigned int type)
{
	MCDI_DECLARE_BUF(inbuf, MC_CMD_NVRAM_UPDATE_START_V2_IN_LEN);
	int rc;

	MCDI_SET_DWORD(inbuf, NVRAM_UPDATE_START_IN_TYPE, type);
	MCDI_POPULATE_DWORD_1(inbuf, NVRAM_UPDATE_START_V2_IN_FLAGS,
			      NVRAM_UPDATE_START_V2_IN_FLAG_REPORT_VERIFY_RESULT,
			      1);

	BUILD_BUG_ON(MC_CMD_NVRAM_UPDATE_START_OUT_LEN != 0);

	rc = efx_mcdi_rpc(efx, MC_CMD_NVRAM_UPDATE_START, inbuf, sizeof(inbuf),
			  NULL, 0, NULL);

	return rc;
}

static int efx_mcdi_nvram_read(struct efx_nic *efx, unsigned int type,
			       loff_t offset, u8 *buffer, size_t length)
{
	MCDI_DECLARE_BUF(inbuf, MC_CMD_NVRAM_READ_IN_V2_LEN);
	MCDI_DECLARE_BUF(outbuf,
			 MC_CMD_NVRAM_READ_OUT_LEN(EFX_MCDI_NVRAM_LEN_MAX));
	size_t outlen;
	int rc;

	MCDI_SET_DWORD(inbuf, NVRAM_READ_IN_TYPE, type);
	MCDI_SET_DWORD(inbuf, NVRAM_READ_IN_OFFSET, offset);
	MCDI_SET_DWORD(inbuf, NVRAM_READ_IN_LENGTH, length);
	MCDI_SET_DWORD(inbuf, NVRAM_READ_IN_V2_MODE,
		       MC_CMD_NVRAM_READ_IN_V2_DEFAULT);

	rc = efx_mcdi_rpc(efx, MC_CMD_NVRAM_READ, inbuf, sizeof(inbuf),
			  outbuf, sizeof(outbuf), &outlen);
	if (rc)
		return rc;

	memcpy(buffer, MCDI_PTR(outbuf, NVRAM_READ_OUT_READ_BUFFER), length);
	return 0;
}

static int efx_mcdi_nvram_write(struct efx_nic *efx, unsigned int type,
				loff_t offset, const u8 *buffer, size_t length)
{
	MCDI_DECLARE_BUF(inbuf,
			 MC_CMD_NVRAM_WRITE_IN_LEN(EFX_MCDI_NVRAM_LEN_MAX));
	int rc;

	MCDI_SET_DWORD(inbuf, NVRAM_WRITE_IN_TYPE, type);
	MCDI_SET_DWORD(inbuf, NVRAM_WRITE_IN_OFFSET, offset);
	MCDI_SET_DWORD(inbuf, NVRAM_WRITE_IN_LENGTH, length);
	memcpy(MCDI_PTR(inbuf, NVRAM_WRITE_IN_WRITE_BUFFER), buffer, length);

	BUILD_BUG_ON(MC_CMD_NVRAM_WRITE_OUT_LEN != 0);

	rc = efx_mcdi_rpc(efx, MC_CMD_NVRAM_WRITE, inbuf,
			  ALIGN(MC_CMD_NVRAM_WRITE_IN_LEN(length), 4),
			  NULL, 0, NULL);
	return rc;
}

static int efx_mcdi_nvram_erase(struct efx_nic *efx, unsigned int type,
				loff_t offset, size_t length)
{
	MCDI_DECLARE_BUF(inbuf, MC_CMD_NVRAM_ERASE_IN_LEN);
	int rc;

	MCDI_SET_DWORD(inbuf, NVRAM_ERASE_IN_TYPE, type);
	MCDI_SET_DWORD(inbuf, NVRAM_ERASE_IN_OFFSET, offset);
	MCDI_SET_DWORD(inbuf, NVRAM_ERASE_IN_LENGTH, length);

	BUILD_BUG_ON(MC_CMD_NVRAM_ERASE_OUT_LEN != 0);

	rc = efx_mcdi_rpc(efx, MC_CMD_NVRAM_ERASE, inbuf, sizeof(inbuf),
			  NULL, 0, NULL);
	return rc;
}

static int efx_mcdi_nvram_update_finish(struct efx_nic *efx, unsigned int type)
{
	MCDI_DECLARE_BUF(inbuf, MC_CMD_NVRAM_UPDATE_FINISH_V2_IN_LEN);
	MCDI_DECLARE_BUF(outbuf, MC_CMD_NVRAM_UPDATE_FINISH_V2_OUT_LEN);
	size_t outlen;
	int rc, rc2;

	MCDI_SET_DWORD(inbuf, NVRAM_UPDATE_FINISH_IN_TYPE, type);
	/* Always set this flag. Old firmware ignores it */
	MCDI_POPULATE_DWORD_1(inbuf, NVRAM_UPDATE_FINISH_V2_IN_FLAGS,
			      NVRAM_UPDATE_FINISH_V2_IN_FLAG_REPORT_VERIFY_RESULT,
			      1);

	rc = efx_mcdi_rpc(efx, MC_CMD_NVRAM_UPDATE_FINISH, inbuf, sizeof(inbuf),
			  outbuf, sizeof(outbuf), &outlen);
	if (!rc && outlen >= MC_CMD_NVRAM_UPDATE_FINISH_V2_OUT_LEN) {
		rc2 = MCDI_DWORD(outbuf, NVRAM_UPDATE_FINISH_V2_OUT_RESULT_CODE);
		if (rc2 != MC_CMD_NVRAM_VERIFY_RC_SUCCESS)
			netif_err(efx, drv, efx->net_dev,
				  "NVRAM update failed verification with code 0x%x\n",
				  rc2);
		switch (rc2) {
		case MC_CMD_NVRAM_VERIFY_RC_SUCCESS:
			break;
		case MC_CMD_NVRAM_VERIFY_RC_CMS_CHECK_FAILED:
		case MC_CMD_NVRAM_VERIFY_RC_MESSAGE_DIGEST_CHECK_FAILED:
		case MC_CMD_NVRAM_VERIFY_RC_SIGNATURE_CHECK_FAILED:
		case MC_CMD_NVRAM_VERIFY_RC_TRUSTED_APPROVERS_CHECK_FAILED:
		case MC_CMD_NVRAM_VERIFY_RC_SIGNATURE_CHAIN_CHECK_FAILED:
			rc = -EIO;
			break;
		case MC_CMD_NVRAM_VERIFY_RC_INVALID_CMS_FORMAT:
		case MC_CMD_NVRAM_VERIFY_RC_BAD_MESSAGE_DIGEST:
			rc = -EINVAL;
			break;
		case MC_CMD_NVRAM_VERIFY_RC_NO_VALID_SIGNATURES:
		case MC_CMD_NVRAM_VERIFY_RC_NO_TRUSTED_APPROVERS:
		case MC_CMD_NVRAM_VERIFY_RC_NO_SIGNATURE_MATCH:
			rc = -EPERM;
			break;
		default:
			netif_err(efx, drv, efx->net_dev,
				  "Unknown response to NVRAM_UPDATE_FINISH\n");
			rc = -EIO;
		}
	}

	return rc;
}

int efx_mcdi_mtd_read(struct mtd_info *mtd, loff_t start,
		      size_t len, size_t *retlen, u8 *buffer)
{
	struct efx_mcdi_mtd_partition *part = to_efx_mcdi_mtd_partition(mtd);
	struct efx_nic *efx = mtd->priv;
	loff_t offset = start;
	loff_t end = min_t(loff_t, start + len, mtd->size);
	size_t chunk;
	int rc = 0;

	while (offset < end) {
		chunk = min_t(size_t, end - offset, EFX_MCDI_NVRAM_LEN_MAX);
		rc = efx_mcdi_nvram_read(efx, part->nvram_type, offset,
					 buffer, chunk);
		if (rc)
			goto out;
		offset += chunk;
		buffer += chunk;
	}
out:
	*retlen = offset - start;
	return rc;
}

int efx_mcdi_mtd_erase(struct mtd_info *mtd, loff_t start, size_t len)
{
	struct efx_mcdi_mtd_partition *part = to_efx_mcdi_mtd_partition(mtd);
	struct efx_nic *efx = mtd->priv;
	loff_t offset = start & ~((loff_t)(mtd->erasesize - 1));
	loff_t end = min_t(loff_t, start + len, mtd->size);
	size_t chunk = part->common.mtd.erasesize;
	int rc = 0;

	if (!part->updating) {
		rc = efx_mcdi_nvram_update_start(efx, part->nvram_type);
		if (rc)
			goto out;
		part->updating = true;
	}

	/* The MCDI interface can in fact do multiple erase blocks at once;
	 * but erasing may be slow, so we make multiple calls here to avoid
	 * tripping the MCDI RPC timeout. */
	while (offset < end) {
		rc = efx_mcdi_nvram_erase(efx, part->nvram_type, offset,
					  chunk);
		if (rc)
			goto out;
		offset += chunk;
	}
out:
	return rc;
}

int efx_mcdi_mtd_write(struct mtd_info *mtd, loff_t start,
		       size_t len, size_t *retlen, const u8 *buffer)
{
	struct efx_mcdi_mtd_partition *part = to_efx_mcdi_mtd_partition(mtd);
	struct efx_nic *efx = mtd->priv;
	loff_t offset = start;
	loff_t end = min_t(loff_t, start + len, mtd->size);
	size_t chunk;
	int rc = 0;

	if (!part->updating) {
		rc = efx_mcdi_nvram_update_start(efx, part->nvram_type);
		if (rc)
			goto out;
		part->updating = true;
	}

	while (offset < end) {
		chunk = min_t(size_t, end - offset, EFX_MCDI_NVRAM_LEN_MAX);
		rc = efx_mcdi_nvram_write(efx, part->nvram_type, offset,
					  buffer, chunk);
		if (rc)
			goto out;
		offset += chunk;
		buffer += chunk;
	}
out:
	*retlen = offset - start;
	return rc;
}

int efx_mcdi_mtd_sync(struct mtd_info *mtd)
{
	struct efx_mcdi_mtd_partition *part = to_efx_mcdi_mtd_partition(mtd);
	struct efx_nic *efx = mtd->priv;
	int rc = 0;

	if (part->updating) {
		part->updating = false;
		rc = efx_mcdi_nvram_update_finish(efx, part->nvram_type);
	}

	return rc;
}

void efx_mcdi_mtd_rename(struct efx_mtd_partition *part)
{
	struct efx_mcdi_mtd_partition *mcdi_part =
		container_of(part, struct efx_mcdi_mtd_partition, common);
	struct efx_nic *efx = part->mtd.priv;

	snprintf(part->name, sizeof(part->name), "%s %s:%02x",
		 efx->name, part->type_name, mcdi_part->fw_subtype);
}

#endif /* CONFIG_SFC_MTD */
