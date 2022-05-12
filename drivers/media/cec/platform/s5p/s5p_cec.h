/* SPDX-License-Identifier: GPL-2.0-or-later */
/* drivers/media/platform/s5p-cec/s5p_cec.h
 *
 * Samsung S5P HDMI CEC driver
 *
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 */

#ifndef _S5P_CEC_H_
#define _S5P_CEC_H_ __FILE__

#include <linux/clk.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/timer.h>
#include <linux/workqueue.h>
#include <media/cec.h>

#include "exynos_hdmi_cec.h"
#include "regs-cec.h"
#include "s5p_cec.h"

#define CEC_NAME	"s5p-cec"

#define CEC_STATUS_TX_RUNNING		(1 << 0)
#define CEC_STATUS_TX_TRANSFERRING	(1 << 1)
#define CEC_STATUS_TX_DONE		(1 << 2)
#define CEC_STATUS_TX_ERROR		(1 << 3)
#define CEC_STATUS_TX_NACK		(1 << 4)
#define CEC_STATUS_TX_BYTES		(0xFF << 8)
#define CEC_STATUS_RX_RUNNING		(1 << 16)
#define CEC_STATUS_RX_RECEIVING		(1 << 17)
#define CEC_STATUS_RX_DONE		(1 << 18)
#define CEC_STATUS_RX_ERROR		(1 << 19)
#define CEC_STATUS_RX_BCAST		(1 << 20)
#define CEC_STATUS_RX_BYTES		(0xFF << 24)

#define CEC_WORKER_TX_DONE		(1 << 0)
#define CEC_WORKER_RX_MSG		(1 << 1)

/* CEC Rx buffer size */
#define CEC_RX_BUFF_SIZE		16
/* CEC Tx buffer size */
#define CEC_TX_BUFF_SIZE		16

enum cec_state {
	STATE_IDLE,
	STATE_BUSY,
	STATE_DONE,
	STATE_NACK,
	STATE_ERROR
};

struct cec_notifier;

struct s5p_cec_dev {
	struct cec_adapter	*adap;
	struct clk		*clk;
	struct device		*dev;
	struct mutex		lock;
	struct regmap           *pmu;
	struct cec_notifier	*notifier;
	int			irq;
	void __iomem		*reg;

	enum cec_state		rx;
	enum cec_state		tx;
	struct cec_msg		msg;
};

#endif /* _S5P_CEC_H_ */
