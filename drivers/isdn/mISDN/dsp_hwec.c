// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * dsp_hwec.c:
 * builtin mISDN dsp pipeline element for enabling the hw echocanceller
 *
 * Copyright (C) 2007, Nadi Sarrar
 *
 * Nadi Sarrar <nadi@beronet.com>
 */

#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/mISDNdsp.h>
#include <linux/mISDNif.h>
#include "core.h"
#include "dsp.h"
#include "dsp_hwec.h"

static struct mISDN_dsp_element_arg args[] = {
	{ "deftaps", "128", "Set the number of taps of cancellation." },
};

static struct mISDN_dsp_element dsp_hwec_p = {
	.name = "hwec",
	.new = NULL,
	.free = NULL,
	.process_tx = NULL,
	.process_rx = NULL,
	.num_args = ARRAY_SIZE(args),
	.args = args,
};
struct mISDN_dsp_element *dsp_hwec = &dsp_hwec_p;

void dsp_hwec_enable(struct dsp *dsp, const char *arg)
{
	int deftaps = 128,
		len;
	struct mISDN_ctrl_req	cq;

	if (!dsp) {
		printk(KERN_ERR "%s: failed to enable hwec: dsp is NULL\n",
		       __func__);
		return;
	}

	if (!arg)
		goto _do;

	len = strlen(arg);
	if (!len)
		goto _do;

	{
		char *dup, *tok, *name, *val;
		int tmp;

		dup = kstrdup(arg, GFP_ATOMIC);
		if (!dup)
			return;

		while ((tok = strsep(&dup, ","))) {
			if (!strlen(tok))
				continue;
			name = strsep(&tok, "=");
			val = tok;

			if (!val)
				continue;

			if (!strcmp(name, "deftaps")) {
				if (sscanf(val, "%d", &tmp) == 1)
					deftaps = tmp;
			}
		}

		kfree(dup);
	}

_do:
	printk(KERN_DEBUG "%s: enabling hwec with deftaps=%d\n",
	       __func__, deftaps);
	memset(&cq, 0, sizeof(cq));
	cq.op = MISDN_CTRL_HFC_ECHOCAN_ON;
	cq.p1 = deftaps;
	if (!dsp->ch.peer->ctrl(&dsp->ch, CONTROL_CHANNEL, &cq)) {
		printk(KERN_DEBUG "%s: CONTROL_CHANNEL failed\n",
		       __func__);
		return;
	}
}

void dsp_hwec_disable(struct dsp *dsp)
{
	struct mISDN_ctrl_req	cq;

	if (!dsp) {
		printk(KERN_ERR "%s: failed to disable hwec: dsp is NULL\n",
		       __func__);
		return;
	}

	printk(KERN_DEBUG "%s: disabling hwec\n", __func__);
	memset(&cq, 0, sizeof(cq));
	cq.op = MISDN_CTRL_HFC_ECHOCAN_OFF;
	if (!dsp->ch.peer->ctrl(&dsp->ch, CONTROL_CHANNEL, &cq)) {
		printk(KERN_DEBUG "%s: CONTROL_CHANNEL failed\n",
		       __func__);
		return;
	}
}

int dsp_hwec_init(void)
{
	mISDN_dsp_element_register(dsp_hwec);

	return 0;
}

void dsp_hwec_exit(void)
{
	mISDN_dsp_element_unregister(dsp_hwec);
}
