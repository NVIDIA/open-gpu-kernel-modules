/* SPDX-License-Identifier: GPL-2.0+ */
/* Copyright (c) 2015-2016 Quantenna Communications. All rights reserved. */

#ifndef _QTN_FMAC_SHM_IPC_H_
#define _QTN_FMAC_SHM_IPC_H_

#include <linux/workqueue.h>
#include <linux/completion.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>

#include "shm_ipc_defs.h"

#define QTN_SHM_IPC_ACK_TIMEOUT		(2 * HZ)

struct qtnf_shm_ipc_int {
	void (*fn)(void *arg);
	void *arg;
};

struct qtnf_shm_ipc_rx_callback {
	void (*fn)(void *arg, const u8 __iomem *buf, size_t len);
	void *arg;
};

enum qtnf_shm_ipc_direction {
	QTNF_SHM_IPC_OUTBOUND		= BIT(0),
	QTNF_SHM_IPC_INBOUND		= BIT(1),
};

struct qtnf_shm_ipc {
	struct qtnf_shm_ipc_region __iomem *shm_region;
	enum qtnf_shm_ipc_direction direction;
	size_t tx_packet_count;
	size_t rx_packet_count;

	size_t tx_timeout_count;

	u8 waiting_for_ack;

	struct qtnf_shm_ipc_int interrupt;
	struct qtnf_shm_ipc_rx_callback rx_callback;

	void (*irq_handler)(struct qtnf_shm_ipc *ipc);

	struct workqueue_struct *workqueue;
	struct work_struct irq_work;
	struct completion tx_completion;
};

int qtnf_shm_ipc_init(struct qtnf_shm_ipc *ipc,
		      enum qtnf_shm_ipc_direction direction,
		      struct qtnf_shm_ipc_region __iomem *shm_region,
		      struct workqueue_struct *workqueue,
		      const struct qtnf_shm_ipc_int *interrupt,
		      const struct qtnf_shm_ipc_rx_callback *rx_callback);
void qtnf_shm_ipc_free(struct qtnf_shm_ipc *ipc);
int qtnf_shm_ipc_send(struct qtnf_shm_ipc *ipc, const u8 *buf, size_t size);

static inline void qtnf_shm_ipc_irq_handler(struct qtnf_shm_ipc *ipc)
{
	ipc->irq_handler(ipc);
}

#endif /* _QTN_FMAC_SHM_IPC_H_ */
