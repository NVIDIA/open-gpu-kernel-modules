// SPDX-License-Identifier: GPL-2.0
/*
 *  Shared Memory Communications over RDMA (SMC-R) and RoCE
 *
 *  Generic netlink support functions to interact with SMC module
 *
 *  Copyright IBM Corp. 2020
 *
 *  Author(s):	Guvenc Gulce <guvenc@linux.ibm.com>
 */

#include <linux/module.h>
#include <linux/list.h>
#include <linux/ctype.h>
#include <linux/mutex.h>
#include <linux/if.h>
#include <linux/smc.h>

#include "smc_core.h"
#include "smc_ism.h"
#include "smc_ib.h"
#include "smc_netlink.h"

#define SMC_CMD_MAX_ATTR 1

/* SMC_GENL generic netlink operation definition */
static const struct genl_ops smc_gen_nl_ops[] = {
	{
		.cmd = SMC_NETLINK_GET_SYS_INFO,
		/* can be retrieved by unprivileged users */
		.dumpit = smc_nl_get_sys_info,
	},
	{
		.cmd = SMC_NETLINK_GET_LGR_SMCR,
		/* can be retrieved by unprivileged users */
		.dumpit = smcr_nl_get_lgr,
	},
	{
		.cmd = SMC_NETLINK_GET_LINK_SMCR,
		/* can be retrieved by unprivileged users */
		.dumpit = smcr_nl_get_link,
	},
	{
		.cmd = SMC_NETLINK_GET_LGR_SMCD,
		/* can be retrieved by unprivileged users */
		.dumpit = smcd_nl_get_lgr,
	},
	{
		.cmd = SMC_NETLINK_GET_DEV_SMCD,
		/* can be retrieved by unprivileged users */
		.dumpit = smcd_nl_get_device,
	},
	{
		.cmd = SMC_NETLINK_GET_DEV_SMCR,
		/* can be retrieved by unprivileged users */
		.dumpit = smcr_nl_get_device,
	},
};

static const struct nla_policy smc_gen_nl_policy[2] = {
	[SMC_CMD_MAX_ATTR]	= { .type = NLA_REJECT, },
};

/* SMC_GENL family definition */
struct genl_family smc_gen_nl_family __ro_after_init = {
	.hdrsize =	0,
	.name =		SMC_GENL_FAMILY_NAME,
	.version =	SMC_GENL_FAMILY_VERSION,
	.maxattr =	SMC_CMD_MAX_ATTR,
	.policy =	smc_gen_nl_policy,
	.netnsok =	true,
	.module =	THIS_MODULE,
	.ops =		smc_gen_nl_ops,
	.n_ops =	ARRAY_SIZE(smc_gen_nl_ops)
};

int __init smc_nl_init(void)
{
	return genl_register_family(&smc_gen_nl_family);
}

void smc_nl_exit(void)
{
	genl_unregister_family(&smc_gen_nl_family);
}
