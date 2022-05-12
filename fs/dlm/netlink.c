// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2007 Red Hat, Inc.  All rights reserved.
 */

#include <net/genetlink.h>
#include <linux/dlm.h>
#include <linux/dlm_netlink.h>
#include <linux/gfp.h>

#include "dlm_internal.h"

static uint32_t dlm_nl_seqnum;
static uint32_t listener_nlportid;

static struct genl_family family;

static int prepare_data(u8 cmd, struct sk_buff **skbp, size_t size)
{
	struct sk_buff *skb;
	void *data;

	skb = genlmsg_new(size, GFP_NOFS);
	if (!skb)
		return -ENOMEM;

	/* add the message headers */
	data = genlmsg_put(skb, 0, dlm_nl_seqnum++, &family, 0, cmd);
	if (!data) {
		nlmsg_free(skb);
		return -EINVAL;
	}

	*skbp = skb;
	return 0;
}

static struct dlm_lock_data *mk_data(struct sk_buff *skb)
{
	struct nlattr *ret;

	ret = nla_reserve(skb, DLM_TYPE_LOCK, sizeof(struct dlm_lock_data));
	if (!ret)
		return NULL;
	return nla_data(ret);
}

static int send_data(struct sk_buff *skb)
{
	struct genlmsghdr *genlhdr = nlmsg_data((struct nlmsghdr *)skb->data);
	void *data = genlmsg_data(genlhdr);

	genlmsg_end(skb, data);

	return genlmsg_unicast(&init_net, skb, listener_nlportid);
}

static int user_cmd(struct sk_buff *skb, struct genl_info *info)
{
	listener_nlportid = info->snd_portid;
	printk("user_cmd nlpid %u\n", listener_nlportid);
	return 0;
}

static const struct genl_small_ops dlm_nl_ops[] = {
	{
		.cmd	= DLM_CMD_HELLO,
		.validate = GENL_DONT_VALIDATE_STRICT | GENL_DONT_VALIDATE_DUMP,
		.doit	= user_cmd,
	},
};

static struct genl_family family __ro_after_init = {
	.name		= DLM_GENL_NAME,
	.version	= DLM_GENL_VERSION,
	.small_ops	= dlm_nl_ops,
	.n_small_ops	= ARRAY_SIZE(dlm_nl_ops),
	.module		= THIS_MODULE,
};

int __init dlm_netlink_init(void)
{
	return genl_register_family(&family);
}

void dlm_netlink_exit(void)
{
	genl_unregister_family(&family);
}

static void fill_data(struct dlm_lock_data *data, struct dlm_lkb *lkb)
{
	struct dlm_rsb *r = lkb->lkb_resource;

	memset(data, 0, sizeof(struct dlm_lock_data));

	data->version = DLM_LOCK_DATA_VERSION;
	data->nodeid = lkb->lkb_nodeid;
	data->ownpid = lkb->lkb_ownpid;
	data->id = lkb->lkb_id;
	data->remid = lkb->lkb_remid;
	data->status = lkb->lkb_status;
	data->grmode = lkb->lkb_grmode;
	data->rqmode = lkb->lkb_rqmode;
	if (lkb->lkb_ua)
		data->xid = lkb->lkb_ua->xid;
	if (r) {
		data->lockspace_id = r->res_ls->ls_global_id;
		data->resource_namelen = r->res_length;
		memcpy(data->resource_name, r->res_name, r->res_length);
	}
}

void dlm_timeout_warn(struct dlm_lkb *lkb)
{
	struct sk_buff *send_skb;
	struct dlm_lock_data *data;
	size_t size;
	int rv;

	size = nla_total_size(sizeof(struct dlm_lock_data)) +
	       nla_total_size(0); /* why this? */

	rv = prepare_data(DLM_CMD_TIMEOUT, &send_skb, size);
	if (rv < 0)
		return;

	data = mk_data(send_skb);
	if (!data) {
		nlmsg_free(send_skb);
		return;
	}

	fill_data(data, lkb);

	send_data(send_skb);
}

