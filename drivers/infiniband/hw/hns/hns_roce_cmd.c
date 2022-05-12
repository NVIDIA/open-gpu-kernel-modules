/*
 * Copyright (c) 2016 Hisilicon Limited.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <linux/dmapool.h>
#include <linux/platform_device.h>
#include "hns_roce_common.h"
#include "hns_roce_device.h"
#include "hns_roce_cmd.h"

#define CMD_POLL_TOKEN 0xffff
#define CMD_MAX_NUM 32

static int hns_roce_cmd_mbox_post_hw(struct hns_roce_dev *hr_dev, u64 in_param,
				     u64 out_param, u32 in_modifier,
				     u8 op_modifier, u16 op, u16 token,
				     int event)
{
	return hr_dev->hw->post_mbox(hr_dev, in_param, out_param, in_modifier,
				     op_modifier, op, token, event);
}

/* this should be called with "poll_sem" */
static int __hns_roce_cmd_mbox_poll(struct hns_roce_dev *hr_dev, u64 in_param,
				    u64 out_param, unsigned long in_modifier,
				    u8 op_modifier, u16 op,
				    unsigned int timeout)
{
	int ret;

	ret = hns_roce_cmd_mbox_post_hw(hr_dev, in_param, out_param,
					in_modifier, op_modifier, op,
					CMD_POLL_TOKEN, 0);
	if (ret) {
		dev_err_ratelimited(hr_dev->dev,
				    "failed to post mailbox %x in poll mode, ret = %d.\n",
				    op, ret);
		return ret;
	}

	return hr_dev->hw->poll_mbox_done(hr_dev, timeout);
}

static int hns_roce_cmd_mbox_poll(struct hns_roce_dev *hr_dev, u64 in_param,
				  u64 out_param, unsigned long in_modifier,
				  u8 op_modifier, u16 op, unsigned int timeout)
{
	int ret;

	down(&hr_dev->cmd.poll_sem);
	ret = __hns_roce_cmd_mbox_poll(hr_dev, in_param, out_param, in_modifier,
				       op_modifier, op, timeout);
	up(&hr_dev->cmd.poll_sem);

	return ret;
}

void hns_roce_cmd_event(struct hns_roce_dev *hr_dev, u16 token, u8 status,
			u64 out_param)
{
	struct hns_roce_cmd_context *context =
		&hr_dev->cmd.context[token % hr_dev->cmd.max_cmds];

	if (unlikely(token != context->token)) {
		dev_err_ratelimited(hr_dev->dev,
				    "[cmd] invalid ae token %x,context token is %x!\n",
				    token, context->token);
		return;
	}

	context->result = (status == HNS_ROCE_CMD_SUCCESS) ? 0 : (-EIO);
	context->out_param = out_param;
	complete(&context->done);
}

static int __hns_roce_cmd_mbox_wait(struct hns_roce_dev *hr_dev, u64 in_param,
				    u64 out_param, unsigned long in_modifier,
				    u8 op_modifier, u16 op,
				    unsigned int timeout)
{
	struct hns_roce_cmdq *cmd = &hr_dev->cmd;
	struct hns_roce_cmd_context *context;
	struct device *dev = hr_dev->dev;
	int ret;

	spin_lock(&cmd->context_lock);

	do {
		context = &cmd->context[cmd->free_head];
		cmd->free_head = context->next;
	} while (context->busy);

	context->busy = 1;
	context->token += cmd->max_cmds;

	spin_unlock(&cmd->context_lock);

	reinit_completion(&context->done);

	ret = hns_roce_cmd_mbox_post_hw(hr_dev, in_param, out_param,
					in_modifier, op_modifier, op,
					context->token, 1);
	if (ret) {
		dev_err_ratelimited(dev,
				    "failed to post mailbox %x in event mode, ret = %d.\n",
				    op, ret);
		goto out;
	}

	if (!wait_for_completion_timeout(&context->done,
					 msecs_to_jiffies(timeout))) {
		dev_err_ratelimited(dev, "[cmd] token %x mailbox %x timeout.\n",
				    context->token, op);
		ret = -EBUSY;
		goto out;
	}

	ret = context->result;
	if (ret)
		dev_err_ratelimited(dev, "[cmd] token %x mailbox %x error %d\n",
				    context->token, op, ret);

out:
	context->busy = 0;
	return ret;
}

static int hns_roce_cmd_mbox_wait(struct hns_roce_dev *hr_dev, u64 in_param,
				  u64 out_param, unsigned long in_modifier,
				  u8 op_modifier, u16 op, unsigned int timeout)
{
	int ret;

	down(&hr_dev->cmd.event_sem);
	ret = __hns_roce_cmd_mbox_wait(hr_dev, in_param, out_param, in_modifier,
				       op_modifier, op, timeout);
	up(&hr_dev->cmd.event_sem);

	return ret;
}

int hns_roce_cmd_mbox(struct hns_roce_dev *hr_dev, u64 in_param, u64 out_param,
		      unsigned long in_modifier, u8 op_modifier, u16 op,
		      unsigned int timeout)
{
	bool is_busy;

	if (hr_dev->hw->chk_mbox_avail)
		if (!hr_dev->hw->chk_mbox_avail(hr_dev, &is_busy))
			return is_busy ? -EBUSY : 0;

	if (hr_dev->cmd.use_events)
		return hns_roce_cmd_mbox_wait(hr_dev, in_param, out_param,
					      in_modifier, op_modifier, op,
					      timeout);
	else
		return hns_roce_cmd_mbox_poll(hr_dev, in_param, out_param,
					      in_modifier, op_modifier, op,
					      timeout);
}

int hns_roce_cmd_init(struct hns_roce_dev *hr_dev)
{
	sema_init(&hr_dev->cmd.poll_sem, 1);
	hr_dev->cmd.use_events = 0;
	hr_dev->cmd.max_cmds = CMD_MAX_NUM;
	hr_dev->cmd.pool = dma_pool_create("hns_roce_cmd", hr_dev->dev,
					   HNS_ROCE_MAILBOX_SIZE,
					   HNS_ROCE_MAILBOX_SIZE, 0);
	if (!hr_dev->cmd.pool)
		return -ENOMEM;

	return 0;
}

void hns_roce_cmd_cleanup(struct hns_roce_dev *hr_dev)
{
	dma_pool_destroy(hr_dev->cmd.pool);
}

int hns_roce_cmd_use_events(struct hns_roce_dev *hr_dev)
{
	struct hns_roce_cmdq *hr_cmd = &hr_dev->cmd;
	int i;

	hr_cmd->context =
		kcalloc(hr_cmd->max_cmds, sizeof(*hr_cmd->context), GFP_KERNEL);
	if (!hr_cmd->context)
		return -ENOMEM;

	for (i = 0; i < hr_cmd->max_cmds; ++i) {
		hr_cmd->context[i].token = i;
		hr_cmd->context[i].next = i + 1;
		init_completion(&hr_cmd->context[i].done);
	}
	hr_cmd->context[hr_cmd->max_cmds - 1].next = 0;
	hr_cmd->free_head = 0;

	sema_init(&hr_cmd->event_sem, hr_cmd->max_cmds);
	spin_lock_init(&hr_cmd->context_lock);

	hr_cmd->use_events = 1;
	down(&hr_cmd->poll_sem);

	return 0;
}

void hns_roce_cmd_use_polling(struct hns_roce_dev *hr_dev)
{
	struct hns_roce_cmdq *hr_cmd = &hr_dev->cmd;

	kfree(hr_cmd->context);
	hr_cmd->use_events = 0;

	up(&hr_cmd->poll_sem);
}

struct hns_roce_cmd_mailbox *
hns_roce_alloc_cmd_mailbox(struct hns_roce_dev *hr_dev)
{
	struct hns_roce_cmd_mailbox *mailbox;

	mailbox = kmalloc(sizeof(*mailbox), GFP_KERNEL);
	if (!mailbox)
		return ERR_PTR(-ENOMEM);

	mailbox->buf =
		dma_pool_alloc(hr_dev->cmd.pool, GFP_KERNEL, &mailbox->dma);
	if (!mailbox->buf) {
		kfree(mailbox);
		return ERR_PTR(-ENOMEM);
	}

	return mailbox;
}

void hns_roce_free_cmd_mailbox(struct hns_roce_dev *hr_dev,
			       struct hns_roce_cmd_mailbox *mailbox)
{
	if (!mailbox)
		return;

	dma_pool_free(hr_dev->cmd.pool, mailbox->buf, mailbox->dma);
	kfree(mailbox);
}
