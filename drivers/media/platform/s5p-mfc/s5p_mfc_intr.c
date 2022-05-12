// SPDX-License-Identifier: GPL-2.0-only
/*
 * drivers/media/platform/samsung/mfc5/s5p_mfc_intr.c
 *
 * C file for Samsung MFC (Multi Function Codec - FIMV) driver
 * This file contains functions used to wait for command completion.
 *
 * Kamil Debski, Copyright (C) 2011 Samsung Electronics Co., Ltd.
 * http://www.samsung.com/
 */

#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/io.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include "s5p_mfc_common.h"
#include "s5p_mfc_debug.h"
#include "s5p_mfc_intr.h"

int s5p_mfc_wait_for_done_dev(struct s5p_mfc_dev *dev, int command)
{
	int ret;

	ret = wait_event_interruptible_timeout(dev->queue,
		(dev->int_cond && (dev->int_type == command
		|| dev->int_type == S5P_MFC_R2H_CMD_ERR_RET)),
		msecs_to_jiffies(MFC_INT_TIMEOUT));
	if (ret == 0) {
		mfc_err("Interrupt (dev->int_type:%d, command:%d) timed out\n",
							dev->int_type, command);
		return 1;
	} else if (ret == -ERESTARTSYS) {
		mfc_err("Interrupted by a signal\n");
		return 1;
	}
	mfc_debug(1, "Finished waiting (dev->int_type:%d, command: %d)\n",
							dev->int_type, command);
	if (dev->int_type == S5P_MFC_R2H_CMD_ERR_RET)
		return 1;
	return 0;
}

void s5p_mfc_clean_dev_int_flags(struct s5p_mfc_dev *dev)
{
	dev->int_cond = 0;
	dev->int_type = 0;
	dev->int_err = 0;
}

int s5p_mfc_wait_for_done_ctx(struct s5p_mfc_ctx *ctx,
				    int command, int interrupt)
{
	int ret;

	if (interrupt) {
		ret = wait_event_interruptible_timeout(ctx->queue,
				(ctx->int_cond && (ctx->int_type == command
			|| ctx->int_type == S5P_MFC_R2H_CMD_ERR_RET)),
					msecs_to_jiffies(MFC_INT_TIMEOUT));
	} else {
		ret = wait_event_timeout(ctx->queue,
				(ctx->int_cond && (ctx->int_type == command
			|| ctx->int_type == S5P_MFC_R2H_CMD_ERR_RET)),
					msecs_to_jiffies(MFC_INT_TIMEOUT));
	}
	if (ret == 0) {
		mfc_err("Interrupt (ctx->int_type:%d, command:%d) timed out\n",
							ctx->int_type, command);
		return 1;
	} else if (ret == -ERESTARTSYS) {
		mfc_err("Interrupted by a signal\n");
		return 1;
	}
	mfc_debug(1, "Finished waiting (ctx->int_type:%d, command: %d)\n",
							ctx->int_type, command);
	if (ctx->int_type == S5P_MFC_R2H_CMD_ERR_RET)
		return 1;
	return 0;
}

void s5p_mfc_clean_ctx_int_flags(struct s5p_mfc_ctx *ctx)
{
	ctx->int_cond = 0;
	ctx->int_type = 0;
	ctx->int_err = 0;
}

