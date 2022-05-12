// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2008-2009 Patrick McHardy <kaber@trash.net>
 *
 * Development of this code funded by Astaro AG (http://www.astaro.com/)
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/netlink.h>
#include <linux/netfilter.h>
#include <linux/netfilter/nf_tables.h>
#include <net/netfilter/nf_tables_core.h>
#include <net/netfilter/nf_tables.h>
#include <net/netfilter/nf_tables_offload.h>

void nft_immediate_eval(const struct nft_expr *expr,
			struct nft_regs *regs,
			const struct nft_pktinfo *pkt)
{
	const struct nft_immediate_expr *priv = nft_expr_priv(expr);

	nft_data_copy(&regs->data[priv->dreg], &priv->data, priv->dlen);
}

static const struct nla_policy nft_immediate_policy[NFTA_IMMEDIATE_MAX + 1] = {
	[NFTA_IMMEDIATE_DREG]	= { .type = NLA_U32 },
	[NFTA_IMMEDIATE_DATA]	= { .type = NLA_NESTED },
};

static int nft_immediate_init(const struct nft_ctx *ctx,
			      const struct nft_expr *expr,
			      const struct nlattr * const tb[])
{
	struct nft_immediate_expr *priv = nft_expr_priv(expr);
	struct nft_data_desc desc;
	int err;

	if (tb[NFTA_IMMEDIATE_DREG] == NULL ||
	    tb[NFTA_IMMEDIATE_DATA] == NULL)
		return -EINVAL;

	err = nft_data_init(ctx, &priv->data, sizeof(priv->data), &desc,
			    tb[NFTA_IMMEDIATE_DATA]);
	if (err < 0)
		return err;

	priv->dlen = desc.len;

	err = nft_parse_register_store(ctx, tb[NFTA_IMMEDIATE_DREG],
				       &priv->dreg, &priv->data, desc.type,
				       desc.len);
	if (err < 0)
		goto err1;

	if (priv->dreg == NFT_REG_VERDICT) {
		struct nft_chain *chain = priv->data.verdict.chain;

		switch (priv->data.verdict.code) {
		case NFT_JUMP:
		case NFT_GOTO:
			if (nft_chain_is_bound(chain)) {
				err = -EBUSY;
				goto err1;
			}
			chain->bound = true;
			break;
		default:
			break;
		}
	}

	return 0;

err1:
	nft_data_release(&priv->data, desc.type);
	return err;
}

static void nft_immediate_activate(const struct nft_ctx *ctx,
				   const struct nft_expr *expr)
{
	const struct nft_immediate_expr *priv = nft_expr_priv(expr);

	return nft_data_hold(&priv->data, nft_dreg_to_type(priv->dreg));
}

static void nft_immediate_deactivate(const struct nft_ctx *ctx,
				     const struct nft_expr *expr,
				     enum nft_trans_phase phase)
{
	const struct nft_immediate_expr *priv = nft_expr_priv(expr);

	if (phase == NFT_TRANS_COMMIT)
		return;

	return nft_data_release(&priv->data, nft_dreg_to_type(priv->dreg));
}

static void nft_immediate_destroy(const struct nft_ctx *ctx,
				  const struct nft_expr *expr)
{
	const struct nft_immediate_expr *priv = nft_expr_priv(expr);
	const struct nft_data *data = &priv->data;
	struct nft_rule *rule, *n;
	struct nft_ctx chain_ctx;
	struct nft_chain *chain;

	if (priv->dreg != NFT_REG_VERDICT)
		return;

	switch (data->verdict.code) {
	case NFT_JUMP:
	case NFT_GOTO:
		chain = data->verdict.chain;

		if (!nft_chain_is_bound(chain))
			break;

		chain_ctx = *ctx;
		chain_ctx.chain = chain;

		list_for_each_entry_safe(rule, n, &chain->rules, list)
			nf_tables_rule_release(&chain_ctx, rule);

		nf_tables_chain_destroy(&chain_ctx);
		break;
	default:
		break;
	}
}

static int nft_immediate_dump(struct sk_buff *skb, const struct nft_expr *expr)
{
	const struct nft_immediate_expr *priv = nft_expr_priv(expr);

	if (nft_dump_register(skb, NFTA_IMMEDIATE_DREG, priv->dreg))
		goto nla_put_failure;

	return nft_data_dump(skb, NFTA_IMMEDIATE_DATA, &priv->data,
			     nft_dreg_to_type(priv->dreg), priv->dlen);

nla_put_failure:
	return -1;
}

static int nft_immediate_validate(const struct nft_ctx *ctx,
				  const struct nft_expr *expr,
				  const struct nft_data **d)
{
	const struct nft_immediate_expr *priv = nft_expr_priv(expr);
	struct nft_ctx *pctx = (struct nft_ctx *)ctx;
	const struct nft_data *data;
	int err;

	if (priv->dreg != NFT_REG_VERDICT)
		return 0;

	data = &priv->data;

	switch (data->verdict.code) {
	case NFT_JUMP:
	case NFT_GOTO:
		pctx->level++;
		err = nft_chain_validate(ctx, data->verdict.chain);
		if (err < 0)
			return err;
		pctx->level--;
		break;
	default:
		break;
	}

	return 0;
}

static int nft_immediate_offload_verdict(struct nft_offload_ctx *ctx,
					 struct nft_flow_rule *flow,
					 const struct nft_immediate_expr *priv)
{
	struct flow_action_entry *entry;
	const struct nft_data *data;

	entry = &flow->rule->action.entries[ctx->num_actions++];

	data = &priv->data;
	switch (data->verdict.code) {
	case NF_ACCEPT:
		entry->id = FLOW_ACTION_ACCEPT;
		break;
	case NF_DROP:
		entry->id = FLOW_ACTION_DROP;
		break;
	default:
		return -EOPNOTSUPP;
	}

	return 0;
}

static int nft_immediate_offload(struct nft_offload_ctx *ctx,
				 struct nft_flow_rule *flow,
				 const struct nft_expr *expr)
{
	const struct nft_immediate_expr *priv = nft_expr_priv(expr);

	if (priv->dreg == NFT_REG_VERDICT)
		return nft_immediate_offload_verdict(ctx, flow, priv);

	memcpy(&ctx->regs[priv->dreg].data, &priv->data, sizeof(priv->data));

	return 0;
}

static const struct nft_expr_ops nft_imm_ops = {
	.type		= &nft_imm_type,
	.size		= NFT_EXPR_SIZE(sizeof(struct nft_immediate_expr)),
	.eval		= nft_immediate_eval,
	.init		= nft_immediate_init,
	.activate	= nft_immediate_activate,
	.deactivate	= nft_immediate_deactivate,
	.destroy	= nft_immediate_destroy,
	.dump		= nft_immediate_dump,
	.validate	= nft_immediate_validate,
	.offload	= nft_immediate_offload,
	.offload_flags	= NFT_OFFLOAD_F_ACTION,
};

struct nft_expr_type nft_imm_type __read_mostly = {
	.name		= "immediate",
	.ops		= &nft_imm_ops,
	.policy		= nft_immediate_policy,
	.maxattr	= NFTA_IMMEDIATE_MAX,
	.owner		= THIS_MODULE,
};
