// SPDX-License-Identifier: GPL-2.0-only
/*
 * An implementation of file copy service.
 *
 * Copyright (C) 2014, Microsoft, Inc.
 *
 * Author : K. Y. Srinivasan <ksrinivasan@novell.com>
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/nls.h>
#include <linux/workqueue.h>
#include <linux/hyperv.h>
#include <linux/sched.h>
#include <asm/hyperv-tlfs.h>

#include "hyperv_vmbus.h"
#include "hv_utils_transport.h"

#define WIN8_SRV_MAJOR		1
#define WIN8_SRV_MINOR		1
#define WIN8_SRV_VERSION	(WIN8_SRV_MAJOR << 16 | WIN8_SRV_MINOR)

#define FCOPY_VER_COUNT 1
static const int fcopy_versions[] = {
	WIN8_SRV_VERSION
};

#define FW_VER_COUNT 1
static const int fw_versions[] = {
	UTIL_FW_VERSION
};

/*
 * Global state maintained for transaction that is being processed.
 * For a class of integration services, including the "file copy service",
 * the specified protocol is a "request/response" protocol which means that
 * there can only be single outstanding transaction from the host at any
 * given point in time. We use this to simplify memory management in this
 * driver - we cache and process only one message at a time.
 *
 * While the request/response protocol is guaranteed by the host, we further
 * ensure this by serializing packet processing in this driver - we do not
 * read additional packets from the VMBUs until the current packet is fully
 * handled.
 */

static struct {
	int state;   /* hvutil_device_state */
	int recv_len; /* number of bytes received. */
	struct hv_fcopy_hdr  *fcopy_msg; /* current message */
	struct vmbus_channel *recv_channel; /* chn we got the request */
	u64 recv_req_id; /* request ID. */
} fcopy_transaction;

static void fcopy_respond_to_host(int error);
static void fcopy_send_data(struct work_struct *dummy);
static void fcopy_timeout_func(struct work_struct *dummy);
static DECLARE_DELAYED_WORK(fcopy_timeout_work, fcopy_timeout_func);
static DECLARE_WORK(fcopy_send_work, fcopy_send_data);
static const char fcopy_devname[] = "vmbus/hv_fcopy";
static u8 *recv_buffer;
static struct hvutil_transport *hvt;
/*
 * This state maintains the version number registered by the daemon.
 */
static int dm_reg_value;

static void fcopy_poll_wrapper(void *channel)
{
	/* Transaction is finished, reset the state here to avoid races. */
	fcopy_transaction.state = HVUTIL_READY;
	tasklet_schedule(&((struct vmbus_channel *)channel)->callback_event);
}

static void fcopy_timeout_func(struct work_struct *dummy)
{
	/*
	 * If the timer fires, the user-mode component has not responded;
	 * process the pending transaction.
	 */
	fcopy_respond_to_host(HV_E_FAIL);
	hv_poll_channel(fcopy_transaction.recv_channel, fcopy_poll_wrapper);
}

static void fcopy_register_done(void)
{
	pr_debug("FCP: userspace daemon registered\n");
	hv_poll_channel(fcopy_transaction.recv_channel, fcopy_poll_wrapper);
}

static int fcopy_handle_handshake(u32 version)
{
	u32 our_ver = FCOPY_CURRENT_VERSION;

	switch (version) {
	case FCOPY_VERSION_0:
		/* Daemon doesn't expect us to reply */
		dm_reg_value = version;
		break;
	case FCOPY_VERSION_1:
		/* Daemon expects us to reply with our own version */
		if (hvutil_transport_send(hvt, &our_ver, sizeof(our_ver),
		    fcopy_register_done))
			return -EFAULT;
		dm_reg_value = version;
		break;
	default:
		/*
		 * For now we will fail the registration.
		 * If and when we have multiple versions to
		 * deal with, we will be backward compatible.
		 * We will add this code when needed.
		 */
		return -EINVAL;
	}
	pr_debug("FCP: userspace daemon ver. %d connected\n", version);
	return 0;
}

static void fcopy_send_data(struct work_struct *dummy)
{
	struct hv_start_fcopy *smsg_out = NULL;
	int operation = fcopy_transaction.fcopy_msg->operation;
	struct hv_start_fcopy *smsg_in;
	void *out_src;
	int rc, out_len;

	/*
	 * The  strings sent from the host are encoded in
	 * in utf16; convert it to utf8 strings.
	 * The host assures us that the utf16 strings will not exceed
	 * the max lengths specified. We will however, reserve room
	 * for the string terminating character - in the utf16s_utf8s()
	 * function we limit the size of the buffer where the converted
	 * string is placed to W_MAX_PATH -1 to guarantee
	 * that the strings can be properly terminated!
	 */

	switch (operation) {
	case START_FILE_COPY:
		out_len = sizeof(struct hv_start_fcopy);
		smsg_out = kzalloc(sizeof(*smsg_out), GFP_KERNEL);
		if (!smsg_out)
			return;

		smsg_out->hdr.operation = operation;
		smsg_in = (struct hv_start_fcopy *)fcopy_transaction.fcopy_msg;

		utf16s_to_utf8s((wchar_t *)smsg_in->file_name, W_MAX_PATH,
				UTF16_LITTLE_ENDIAN,
				(__u8 *)&smsg_out->file_name, W_MAX_PATH - 1);

		utf16s_to_utf8s((wchar_t *)smsg_in->path_name, W_MAX_PATH,
				UTF16_LITTLE_ENDIAN,
				(__u8 *)&smsg_out->path_name, W_MAX_PATH - 1);

		smsg_out->copy_flags = smsg_in->copy_flags;
		smsg_out->file_size = smsg_in->file_size;
		out_src = smsg_out;
		break;

	case WRITE_TO_FILE:
		out_src = fcopy_transaction.fcopy_msg;
		out_len = sizeof(struct hv_do_fcopy);
		break;
	default:
		out_src = fcopy_transaction.fcopy_msg;
		out_len = fcopy_transaction.recv_len;
		break;
	}

	fcopy_transaction.state = HVUTIL_USERSPACE_REQ;
	rc = hvutil_transport_send(hvt, out_src, out_len, NULL);
	if (rc) {
		pr_debug("FCP: failed to communicate to the daemon: %d\n", rc);
		if (cancel_delayed_work_sync(&fcopy_timeout_work)) {
			fcopy_respond_to_host(HV_E_FAIL);
			fcopy_transaction.state = HVUTIL_READY;
		}
	}
	kfree(smsg_out);
}

/*
 * Send a response back to the host.
 */

static void
fcopy_respond_to_host(int error)
{
	struct icmsg_hdr *icmsghdr;
	u32 buf_len;
	struct vmbus_channel *channel;
	u64 req_id;

	/*
	 * Copy the global state for completing the transaction. Note that
	 * only one transaction can be active at a time. This is guaranteed
	 * by the file copy protocol implemented by the host. Furthermore,
	 * the "transaction active" state we maintain ensures that there can
	 * only be one active transaction at a time.
	 */

	buf_len = fcopy_transaction.recv_len;
	channel = fcopy_transaction.recv_channel;
	req_id = fcopy_transaction.recv_req_id;

	icmsghdr = (struct icmsg_hdr *)
			&recv_buffer[sizeof(struct vmbuspipe_hdr)];

	if (channel->onchannel_callback == NULL)
		/*
		 * We have raced with util driver being unloaded;
		 * silently return.
		 */
		return;

	icmsghdr->status = error;
	icmsghdr->icflags = ICMSGHDRFLAG_TRANSACTION | ICMSGHDRFLAG_RESPONSE;
	vmbus_sendpacket(channel, recv_buffer, buf_len, req_id,
				VM_PKT_DATA_INBAND, 0);
}

void hv_fcopy_onchannelcallback(void *context)
{
	struct vmbus_channel *channel = context;
	u32 recvlen;
	u64 requestid;
	struct hv_fcopy_hdr *fcopy_msg;
	struct icmsg_hdr *icmsghdr;
	int fcopy_srv_version;

	if (fcopy_transaction.state > HVUTIL_READY)
		return;

	if (vmbus_recvpacket(channel, recv_buffer, HV_HYP_PAGE_SIZE * 2, &recvlen, &requestid)) {
		pr_err_ratelimited("Fcopy request received. Could not read into recv buf\n");
		return;
	}

	if (!recvlen)
		return;

	/* Ensure recvlen is big enough to read header data */
	if (recvlen < ICMSG_HDR) {
		pr_err_ratelimited("Fcopy request received. Packet length too small: %d\n",
				   recvlen);
		return;
	}

	icmsghdr = (struct icmsg_hdr *)&recv_buffer[
			sizeof(struct vmbuspipe_hdr)];

	if (icmsghdr->icmsgtype == ICMSGTYPE_NEGOTIATE) {
		if (vmbus_prep_negotiate_resp(icmsghdr,
				recv_buffer, recvlen,
				fw_versions, FW_VER_COUNT,
				fcopy_versions, FCOPY_VER_COUNT,
				NULL, &fcopy_srv_version)) {

			pr_info("FCopy IC version %d.%d\n",
				fcopy_srv_version >> 16,
				fcopy_srv_version & 0xFFFF);
		}
	} else if (icmsghdr->icmsgtype == ICMSGTYPE_FCOPY) {
		/* Ensure recvlen is big enough to contain hv_fcopy_hdr */
		if (recvlen < ICMSG_HDR + sizeof(struct hv_fcopy_hdr)) {
			pr_err_ratelimited("Invalid Fcopy hdr. Packet length too small: %u\n",
					   recvlen);
			return;
		}
		fcopy_msg = (struct hv_fcopy_hdr *)&recv_buffer[ICMSG_HDR];

		/*
		 * Stash away this global state for completing the
		 * transaction; note transactions are serialized.
		 */

		fcopy_transaction.recv_len = recvlen;
		fcopy_transaction.recv_req_id = requestid;
		fcopy_transaction.fcopy_msg = fcopy_msg;

		if (fcopy_transaction.state < HVUTIL_READY) {
			/* Userspace is not registered yet */
			fcopy_respond_to_host(HV_E_FAIL);
			return;
		}
		fcopy_transaction.state = HVUTIL_HOSTMSG_RECEIVED;

		/*
		 * Send the information to the user-level daemon.
		 */
		schedule_work(&fcopy_send_work);
		schedule_delayed_work(&fcopy_timeout_work,
				      HV_UTIL_TIMEOUT * HZ);
		return;
	} else {
		pr_err_ratelimited("Fcopy request received. Invalid msg type: %d\n",
				   icmsghdr->icmsgtype);
		return;
	}
	icmsghdr->icflags = ICMSGHDRFLAG_TRANSACTION | ICMSGHDRFLAG_RESPONSE;
	vmbus_sendpacket(channel, recv_buffer, recvlen, requestid,
			VM_PKT_DATA_INBAND, 0);
}

/* Callback when data is received from userspace */
static int fcopy_on_msg(void *msg, int len)
{
	int *val = (int *)msg;

	if (len != sizeof(int))
		return -EINVAL;

	if (fcopy_transaction.state == HVUTIL_DEVICE_INIT)
		return fcopy_handle_handshake(*val);

	if (fcopy_transaction.state != HVUTIL_USERSPACE_REQ)
		return -EINVAL;

	/*
	 * Complete the transaction by forwarding the result
	 * to the host. But first, cancel the timeout.
	 */
	if (cancel_delayed_work_sync(&fcopy_timeout_work)) {
		fcopy_transaction.state = HVUTIL_USERSPACE_RECV;
		fcopy_respond_to_host(*val);
		hv_poll_channel(fcopy_transaction.recv_channel,
				fcopy_poll_wrapper);
	}

	return 0;
}

static void fcopy_on_reset(void)
{
	/*
	 * The daemon has exited; reset the state.
	 */
	fcopy_transaction.state = HVUTIL_DEVICE_INIT;

	if (cancel_delayed_work_sync(&fcopy_timeout_work))
		fcopy_respond_to_host(HV_E_FAIL);
}

int hv_fcopy_init(struct hv_util_service *srv)
{
	recv_buffer = srv->recv_buffer;
	fcopy_transaction.recv_channel = srv->channel;

	/*
	 * When this driver loads, the user level daemon that
	 * processes the host requests may not yet be running.
	 * Defer processing channel callbacks until the daemon
	 * has registered.
	 */
	fcopy_transaction.state = HVUTIL_DEVICE_INIT;

	hvt = hvutil_transport_init(fcopy_devname, 0, 0,
				    fcopy_on_msg, fcopy_on_reset);
	if (!hvt)
		return -EFAULT;

	return 0;
}

static void hv_fcopy_cancel_work(void)
{
	cancel_delayed_work_sync(&fcopy_timeout_work);
	cancel_work_sync(&fcopy_send_work);
}

int hv_fcopy_pre_suspend(void)
{
	struct vmbus_channel *channel = fcopy_transaction.recv_channel;
	struct hv_fcopy_hdr *fcopy_msg;

	/*
	 * Fake a CANCEL_FCOPY message for the user space daemon in case the
	 * daemon is in the middle of copying some file. It doesn't matter if
	 * there is already a message pending to be delivered to the user
	 * space since we force fcopy_transaction.state to be HVUTIL_READY, so
	 * the user space daemon's write() will fail with EINVAL (see
	 * fcopy_on_msg()), and the daemon will reset the device by closing
	 * and re-opening it.
	 */
	fcopy_msg = kzalloc(sizeof(*fcopy_msg), GFP_KERNEL);
	if (!fcopy_msg)
		return -ENOMEM;

	tasklet_disable(&channel->callback_event);

	fcopy_msg->operation = CANCEL_FCOPY;

	hv_fcopy_cancel_work();

	/* We don't care about the return value. */
	hvutil_transport_send(hvt, fcopy_msg, sizeof(*fcopy_msg), NULL);

	kfree(fcopy_msg);

	fcopy_transaction.state = HVUTIL_READY;

	/* tasklet_enable() will be called in hv_fcopy_pre_resume(). */
	return 0;
}

int hv_fcopy_pre_resume(void)
{
	struct vmbus_channel *channel = fcopy_transaction.recv_channel;

	tasklet_enable(&channel->callback_event);

	return 0;
}

void hv_fcopy_deinit(void)
{
	fcopy_transaction.state = HVUTIL_DEVICE_DYING;

	hv_fcopy_cancel_work();

	hvutil_transport_destroy(hvt);
}
