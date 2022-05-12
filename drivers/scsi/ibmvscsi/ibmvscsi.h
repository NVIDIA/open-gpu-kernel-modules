/* SPDX-License-Identifier: GPL-2.0-or-later */
/* ------------------------------------------------------------
 * ibmvscsi.h
 * (C) Copyright IBM Corporation 1994, 2003
 * Authors: Colin DeVilbiss (devilbis@us.ibm.com)
 *          Santiago Leon (santil@us.ibm.com)
 *          Dave Boutcher (sleddog@us.ibm.com)
 *
 * ------------------------------------------------------------
 * Emulation of a SCSI host adapter for Virtual I/O devices
 *
 * This driver allows the Linux SCSI peripheral drivers to directly
 * access devices in the hosting partition, either on an iSeries
 * hypervisor system or a converged hypervisor system.
 */
#ifndef IBMVSCSI_H
#define IBMVSCSI_H
#include <linux/types.h>
#include <linux/list.h>
#include <linux/completion.h>
#include <linux/interrupt.h>
#include <scsi/viosrp.h>

struct scsi_cmnd;
struct Scsi_Host;

/* Number of indirect bufs...the list of these has to fit in the
 * additional data of the srp_cmd struct along with the indirect
 * descriptor
 */
#define MAX_INDIRECT_BUFS 10

#define IBMVSCSI_MAX_REQUESTS_DEFAULT 100
#define IBMVSCSI_CMDS_PER_LUN_DEFAULT 16
#define IBMVSCSI_MAX_SECTORS_DEFAULT 256 /* 32 * 8 = default max I/O 32 pages */
#define IBMVSCSI_MAX_CMDS_PER_LUN 64
#define IBMVSCSI_MAX_LUN 32

/* ------------------------------------------------------------
 * Data Structures
 */
/* an RPA command/response transport queue */
struct crq_queue {
	struct viosrp_crq *msgs;
	int size, cur;
	dma_addr_t msg_token;
	spinlock_t lock;
};

/* a unit of work for the hosting partition */
struct srp_event_struct {
	union viosrp_iu *xfer_iu;
	struct scsi_cmnd *cmnd;
	struct list_head list;
	void (*done) (struct srp_event_struct *);
	struct viosrp_crq crq;
	struct ibmvscsi_host_data *hostdata;
	atomic_t free;
	union viosrp_iu iu;
	void (*cmnd_done) (struct scsi_cmnd *);
	struct completion comp;
	struct timer_list timer;
	union viosrp_iu *sync_srp;
	struct srp_direct_buf *ext_list;
	dma_addr_t ext_list_token;
};

/* a pool of event structs for use */
struct event_pool {
	struct srp_event_struct *events;
	u32 size;
	int next;
	union viosrp_iu *iu_storage;
	dma_addr_t iu_token;
};

enum ibmvscsi_host_action {
	IBMVSCSI_HOST_ACTION_NONE = 0,
	IBMVSCSI_HOST_ACTION_RESET,
	IBMVSCSI_HOST_ACTION_REENABLE,
	IBMVSCSI_HOST_ACTION_UNBLOCK,
};

/* all driver data associated with a host adapter */
struct ibmvscsi_host_data {
	struct list_head host_list;
	atomic_t request_limit;
	int client_migrated;
	enum ibmvscsi_host_action action;
	struct device *dev;
	struct event_pool pool;
	struct crq_queue queue;
	struct tasklet_struct srp_task;
	struct list_head sent;
	struct Scsi_Host *host;
	struct task_struct *work_thread;
	wait_queue_head_t work_wait_q;
	struct mad_adapter_info_data madapter_info;
	struct capabilities caps;
	dma_addr_t caps_addr;
	dma_addr_t adapter_info_addr;
};

#endif				/* IBMVSCSI_H */
