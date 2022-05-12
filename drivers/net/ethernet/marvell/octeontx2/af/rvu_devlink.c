// SPDX-License-Identifier: GPL-2.0
/* Marvell OcteonTx2 RVU Devlink
 *
 * Copyright (C) 2020 Marvell.
 *
 */

#include<linux/bitfield.h>

#include "rvu.h"
#include "rvu_reg.h"
#include "rvu_struct.h"

#define DRV_NAME "octeontx2-af"

static int rvu_report_pair_start(struct devlink_fmsg *fmsg, const char *name)
{
	int err;

	err = devlink_fmsg_pair_nest_start(fmsg, name);
	if (err)
		return err;

	return  devlink_fmsg_obj_nest_start(fmsg);
}

static int rvu_report_pair_end(struct devlink_fmsg *fmsg)
{
	int err;

	err = devlink_fmsg_obj_nest_end(fmsg);
	if (err)
		return err;

	return devlink_fmsg_pair_nest_end(fmsg);
}

static bool rvu_common_request_irq(struct rvu *rvu, int offset,
				   const char *name, irq_handler_t fn)
{
	struct rvu_devlink *rvu_dl = rvu->rvu_dl;
	int rc;

	sprintf(&rvu->irq_name[offset * NAME_SIZE], name);
	rc = request_irq(pci_irq_vector(rvu->pdev, offset), fn, 0,
			 &rvu->irq_name[offset * NAME_SIZE], rvu_dl);
	if (rc)
		dev_warn(rvu->dev, "Failed to register %s irq\n", name);
	else
		rvu->irq_allocated[offset] = true;

	return rvu->irq_allocated[offset];
}

static void rvu_nix_intr_work(struct work_struct *work)
{
	struct rvu_nix_health_reporters *rvu_nix_health_reporter;

	rvu_nix_health_reporter = container_of(work, struct rvu_nix_health_reporters, intr_work);
	devlink_health_report(rvu_nix_health_reporter->rvu_hw_nix_intr_reporter,
			      "NIX_AF_RVU Error",
			      rvu_nix_health_reporter->nix_event_ctx);
}

static irqreturn_t rvu_nix_af_rvu_intr_handler(int irq, void *rvu_irq)
{
	struct rvu_nix_event_ctx *nix_event_context;
	struct rvu_devlink *rvu_dl = rvu_irq;
	struct rvu *rvu;
	int blkaddr;
	u64 intr;

	rvu = rvu_dl->rvu;
	blkaddr = rvu_get_blkaddr(rvu, BLKTYPE_NIX, 0);
	if (blkaddr < 0)
		return IRQ_NONE;

	nix_event_context = rvu_dl->rvu_nix_health_reporter->nix_event_ctx;
	intr = rvu_read64(rvu, blkaddr, NIX_AF_RVU_INT);
	nix_event_context->nix_af_rvu_int = intr;

	/* Clear interrupts */
	rvu_write64(rvu, blkaddr, NIX_AF_RVU_INT, intr);
	rvu_write64(rvu, blkaddr, NIX_AF_RVU_INT_ENA_W1C, ~0ULL);
	queue_work(rvu_dl->devlink_wq, &rvu_dl->rvu_nix_health_reporter->intr_work);

	return IRQ_HANDLED;
}

static void rvu_nix_gen_work(struct work_struct *work)
{
	struct rvu_nix_health_reporters *rvu_nix_health_reporter;

	rvu_nix_health_reporter = container_of(work, struct rvu_nix_health_reporters, gen_work);
	devlink_health_report(rvu_nix_health_reporter->rvu_hw_nix_gen_reporter,
			      "NIX_AF_GEN Error",
			      rvu_nix_health_reporter->nix_event_ctx);
}

static irqreturn_t rvu_nix_af_rvu_gen_handler(int irq, void *rvu_irq)
{
	struct rvu_nix_event_ctx *nix_event_context;
	struct rvu_devlink *rvu_dl = rvu_irq;
	struct rvu *rvu;
	int blkaddr;
	u64 intr;

	rvu = rvu_dl->rvu;
	blkaddr = rvu_get_blkaddr(rvu, BLKTYPE_NIX, 0);
	if (blkaddr < 0)
		return IRQ_NONE;

	nix_event_context = rvu_dl->rvu_nix_health_reporter->nix_event_ctx;
	intr = rvu_read64(rvu, blkaddr, NIX_AF_GEN_INT);
	nix_event_context->nix_af_rvu_gen = intr;

	/* Clear interrupts */
	rvu_write64(rvu, blkaddr, NIX_AF_GEN_INT, intr);
	rvu_write64(rvu, blkaddr, NIX_AF_GEN_INT_ENA_W1C, ~0ULL);
	queue_work(rvu_dl->devlink_wq, &rvu_dl->rvu_nix_health_reporter->gen_work);

	return IRQ_HANDLED;
}

static void rvu_nix_err_work(struct work_struct *work)
{
	struct rvu_nix_health_reporters *rvu_nix_health_reporter;

	rvu_nix_health_reporter = container_of(work, struct rvu_nix_health_reporters, err_work);
	devlink_health_report(rvu_nix_health_reporter->rvu_hw_nix_err_reporter,
			      "NIX_AF_ERR Error",
			      rvu_nix_health_reporter->nix_event_ctx);
}

static irqreturn_t rvu_nix_af_rvu_err_handler(int irq, void *rvu_irq)
{
	struct rvu_nix_event_ctx *nix_event_context;
	struct rvu_devlink *rvu_dl = rvu_irq;
	struct rvu *rvu;
	int blkaddr;
	u64 intr;

	rvu = rvu_dl->rvu;
	blkaddr = rvu_get_blkaddr(rvu, BLKTYPE_NIX, 0);
	if (blkaddr < 0)
		return IRQ_NONE;

	nix_event_context = rvu_dl->rvu_nix_health_reporter->nix_event_ctx;
	intr = rvu_read64(rvu, blkaddr, NIX_AF_ERR_INT);
	nix_event_context->nix_af_rvu_err = intr;

	/* Clear interrupts */
	rvu_write64(rvu, blkaddr, NIX_AF_ERR_INT, intr);
	rvu_write64(rvu, blkaddr, NIX_AF_ERR_INT_ENA_W1C, ~0ULL);
	queue_work(rvu_dl->devlink_wq, &rvu_dl->rvu_nix_health_reporter->err_work);

	return IRQ_HANDLED;
}

static void rvu_nix_ras_work(struct work_struct *work)
{
	struct rvu_nix_health_reporters *rvu_nix_health_reporter;

	rvu_nix_health_reporter = container_of(work, struct rvu_nix_health_reporters, ras_work);
	devlink_health_report(rvu_nix_health_reporter->rvu_hw_nix_ras_reporter,
			      "NIX_AF_RAS Error",
			      rvu_nix_health_reporter->nix_event_ctx);
}

static irqreturn_t rvu_nix_af_rvu_ras_handler(int irq, void *rvu_irq)
{
	struct rvu_nix_event_ctx *nix_event_context;
	struct rvu_devlink *rvu_dl = rvu_irq;
	struct rvu *rvu;
	int blkaddr;
	u64 intr;

	rvu = rvu_dl->rvu;
	blkaddr = rvu_get_blkaddr(rvu, BLKTYPE_NIX, 0);
	if (blkaddr < 0)
		return IRQ_NONE;

	nix_event_context = rvu_dl->rvu_nix_health_reporter->nix_event_ctx;
	intr = rvu_read64(rvu, blkaddr, NIX_AF_ERR_INT);
	nix_event_context->nix_af_rvu_ras = intr;

	/* Clear interrupts */
	rvu_write64(rvu, blkaddr, NIX_AF_RAS, intr);
	rvu_write64(rvu, blkaddr, NIX_AF_RAS_ENA_W1C, ~0ULL);
	queue_work(rvu_dl->devlink_wq, &rvu_dl->rvu_nix_health_reporter->ras_work);

	return IRQ_HANDLED;
}

static void rvu_nix_unregister_interrupts(struct rvu *rvu)
{
	struct rvu_devlink *rvu_dl = rvu->rvu_dl;
	int offs, i, blkaddr;

	blkaddr = rvu_get_blkaddr(rvu, BLKTYPE_NIX, 0);
	if (blkaddr < 0)
		return;

	offs = rvu_read64(rvu, blkaddr, NIX_PRIV_AF_INT_CFG) & 0x3ff;
	if (!offs)
		return;

	rvu_write64(rvu, blkaddr, NIX_AF_RVU_INT_ENA_W1C, ~0ULL);
	rvu_write64(rvu, blkaddr, NIX_AF_GEN_INT_ENA_W1C, ~0ULL);
	rvu_write64(rvu, blkaddr, NIX_AF_ERR_INT_ENA_W1C, ~0ULL);
	rvu_write64(rvu, blkaddr, NIX_AF_RAS_ENA_W1C, ~0ULL);

	if (rvu->irq_allocated[offs + NIX_AF_INT_VEC_RVU]) {
		free_irq(pci_irq_vector(rvu->pdev, offs + NIX_AF_INT_VEC_RVU),
			 rvu_dl);
		rvu->irq_allocated[offs + NIX_AF_INT_VEC_RVU] = false;
	}

	for (i = NIX_AF_INT_VEC_AF_ERR; i < NIX_AF_INT_VEC_CNT; i++)
		if (rvu->irq_allocated[offs + i]) {
			free_irq(pci_irq_vector(rvu->pdev, offs + i), rvu_dl);
			rvu->irq_allocated[offs + i] = false;
		}
}

static int rvu_nix_register_interrupts(struct rvu *rvu)
{
	int blkaddr, base;
	bool rc;

	blkaddr = rvu_get_blkaddr(rvu, BLKTYPE_NIX, 0);
	if (blkaddr < 0)
		return blkaddr;

	/* Get NIX AF MSIX vectors offset. */
	base = rvu_read64(rvu, blkaddr, NIX_PRIV_AF_INT_CFG) & 0x3ff;
	if (!base) {
		dev_warn(rvu->dev,
			 "Failed to get NIX%d NIX_AF_INT vector offsets\n",
			 blkaddr - BLKADDR_NIX0);
		return 0;
	}
	/* Register and enable NIX_AF_RVU_INT interrupt */
	rc = rvu_common_request_irq(rvu, base +  NIX_AF_INT_VEC_RVU,
				    "NIX_AF_RVU_INT",
				    rvu_nix_af_rvu_intr_handler);
	if (!rc)
		goto err;
	rvu_write64(rvu, blkaddr, NIX_AF_RVU_INT_ENA_W1S, ~0ULL);

	/* Register and enable NIX_AF_GEN_INT interrupt */
	rc = rvu_common_request_irq(rvu, base +  NIX_AF_INT_VEC_GEN,
				    "NIX_AF_GEN_INT",
				    rvu_nix_af_rvu_gen_handler);
	if (!rc)
		goto err;
	rvu_write64(rvu, blkaddr, NIX_AF_GEN_INT_ENA_W1S, ~0ULL);

	/* Register and enable NIX_AF_ERR_INT interrupt */
	rc = rvu_common_request_irq(rvu, base + NIX_AF_INT_VEC_AF_ERR,
				    "NIX_AF_ERR_INT",
				    rvu_nix_af_rvu_err_handler);
	if (!rc)
		goto err;
	rvu_write64(rvu, blkaddr, NIX_AF_ERR_INT_ENA_W1S, ~0ULL);

	/* Register and enable NIX_AF_RAS interrupt */
	rc = rvu_common_request_irq(rvu, base + NIX_AF_INT_VEC_POISON,
				    "NIX_AF_RAS",
				    rvu_nix_af_rvu_ras_handler);
	if (!rc)
		goto err;
	rvu_write64(rvu, blkaddr, NIX_AF_RAS_ENA_W1S, ~0ULL);

	return 0;
err:
	rvu_nix_unregister_interrupts(rvu);
	return rc;
}

static int rvu_nix_report_show(struct devlink_fmsg *fmsg, void *ctx,
			       enum nix_af_rvu_health health_reporter)
{
	struct rvu_nix_event_ctx *nix_event_context;
	u64 intr_val;
	int err;

	nix_event_context = ctx;
	switch (health_reporter) {
	case NIX_AF_RVU_INTR:
		intr_val = nix_event_context->nix_af_rvu_int;
		err = rvu_report_pair_start(fmsg, "NIX_AF_RVU");
		if (err)
			return err;
		err = devlink_fmsg_u64_pair_put(fmsg, "\tNIX RVU Interrupt Reg ",
						nix_event_context->nix_af_rvu_int);
		if (err)
			return err;
		if (intr_val & BIT_ULL(0)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tUnmap Slot Error");
			if (err)
				return err;
		}
		err = rvu_report_pair_end(fmsg);
		if (err)
			return err;
		break;
	case NIX_AF_RVU_GEN:
		intr_val = nix_event_context->nix_af_rvu_gen;
		err = rvu_report_pair_start(fmsg, "NIX_AF_GENERAL");
		if (err)
			return err;
		err = devlink_fmsg_u64_pair_put(fmsg, "\tNIX General Interrupt Reg ",
						nix_event_context->nix_af_rvu_gen);
		if (err)
			return err;
		if (intr_val & BIT_ULL(0)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tRx multicast pkt drop");
			if (err)
				return err;
		}
		if (intr_val & BIT_ULL(1)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tRx mirror pkt drop");
			if (err)
				return err;
		}
		if (intr_val & BIT_ULL(4)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tSMQ flush done");
			if (err)
				return err;
		}
		err = rvu_report_pair_end(fmsg);
		if (err)
			return err;
		break;
	case NIX_AF_RVU_ERR:
		intr_val = nix_event_context->nix_af_rvu_err;
		err = rvu_report_pair_start(fmsg, "NIX_AF_ERR");
		if (err)
			return err;
		err = devlink_fmsg_u64_pair_put(fmsg, "\tNIX Error Interrupt Reg ",
						nix_event_context->nix_af_rvu_err);
		if (err)
			return err;
		if (intr_val & BIT_ULL(14)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tFault on NIX_AQ_INST_S read");
			if (err)
				return err;
		}
		if (intr_val & BIT_ULL(13)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tFault on NIX_AQ_RES_S write");
			if (err)
				return err;
		}
		if (intr_val & BIT_ULL(12)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tAQ Doorbell Error");
			if (err)
				return err;
		}
		if (intr_val & BIT_ULL(6)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tRx on unmapped PF_FUNC");
			if (err)
				return err;
		}
		if (intr_val & BIT_ULL(5)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tRx multicast replication error");
			if (err)
				return err;
		}
		if (intr_val & BIT_ULL(4)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tFault on NIX_RX_MCE_S read");
			if (err)
				return err;
		}
		if (intr_val & BIT_ULL(3)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tFault on multicast WQE read");
			if (err)
				return err;
		}
		if (intr_val & BIT_ULL(2)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tFault on mirror WQE read");
			if (err)
				return err;
		}
		if (intr_val & BIT_ULL(1)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tFault on mirror pkt write");
			if (err)
				return err;
		}
		if (intr_val & BIT_ULL(0)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tFault on multicast pkt write");
			if (err)
				return err;
		}
		err = rvu_report_pair_end(fmsg);
		if (err)
			return err;
		break;
	case NIX_AF_RVU_RAS:
		intr_val = nix_event_context->nix_af_rvu_err;
		err = rvu_report_pair_start(fmsg, "NIX_AF_RAS");
		if (err)
			return err;
		err = devlink_fmsg_u64_pair_put(fmsg, "\tNIX RAS Interrupt Reg ",
						nix_event_context->nix_af_rvu_err);
		if (err)
			return err;
		err = devlink_fmsg_string_put(fmsg, "\n\tPoison Data on:");
		if (err)
			return err;
		if (intr_val & BIT_ULL(34)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tNIX_AQ_INST_S");
			if (err)
				return err;
		}
		if (intr_val & BIT_ULL(33)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tNIX_AQ_RES_S");
			if (err)
				return err;
		}
		if (intr_val & BIT_ULL(32)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tHW ctx");
			if (err)
				return err;
		}
		if (intr_val & BIT_ULL(4)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tPacket from mirror buffer");
			if (err)
				return err;
		}
		if (intr_val & BIT_ULL(3)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tPacket from multicast buffer");

			if (err)
				return err;
		}
		if (intr_val & BIT_ULL(2)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tWQE read from mirror buffer");
			if (err)
				return err;
		}
		if (intr_val & BIT_ULL(1)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tWQE read from multicast buffer");
			if (err)
				return err;
		}
		if (intr_val & BIT_ULL(0)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tNIX_RX_MCE_S read");
			if (err)
				return err;
		}
		err = rvu_report_pair_end(fmsg);
		if (err)
			return err;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int rvu_hw_nix_intr_dump(struct devlink_health_reporter *reporter,
				struct devlink_fmsg *fmsg, void *ctx,
				struct netlink_ext_ack *netlink_extack)
{
	struct rvu *rvu = devlink_health_reporter_priv(reporter);
	struct rvu_devlink *rvu_dl = rvu->rvu_dl;
	struct rvu_nix_event_ctx *nix_ctx;

	nix_ctx = rvu_dl->rvu_nix_health_reporter->nix_event_ctx;

	return ctx ? rvu_nix_report_show(fmsg, ctx, NIX_AF_RVU_INTR) :
		     rvu_nix_report_show(fmsg, nix_ctx, NIX_AF_RVU_INTR);
}

static int rvu_hw_nix_intr_recover(struct devlink_health_reporter *reporter,
				   void *ctx, struct netlink_ext_ack *netlink_extack)
{
	struct rvu *rvu = devlink_health_reporter_priv(reporter);
	struct rvu_nix_event_ctx *nix_event_ctx = ctx;
	int blkaddr;

	blkaddr = rvu_get_blkaddr(rvu, BLKTYPE_NIX, 0);
	if (blkaddr < 0)
		return blkaddr;

	if (nix_event_ctx->nix_af_rvu_int)
		rvu_write64(rvu, blkaddr, NIX_AF_RVU_INT_ENA_W1S, ~0ULL);

	return 0;
}

static int rvu_hw_nix_gen_dump(struct devlink_health_reporter *reporter,
			       struct devlink_fmsg *fmsg, void *ctx,
			       struct netlink_ext_ack *netlink_extack)
{
	struct rvu *rvu = devlink_health_reporter_priv(reporter);
	struct rvu_devlink *rvu_dl = rvu->rvu_dl;
	struct rvu_nix_event_ctx *nix_ctx;

	nix_ctx = rvu_dl->rvu_nix_health_reporter->nix_event_ctx;

	return ctx ? rvu_nix_report_show(fmsg, ctx, NIX_AF_RVU_GEN) :
		     rvu_nix_report_show(fmsg, nix_ctx, NIX_AF_RVU_GEN);
}

static int rvu_hw_nix_gen_recover(struct devlink_health_reporter *reporter,
				  void *ctx, struct netlink_ext_ack *netlink_extack)
{
	struct rvu *rvu = devlink_health_reporter_priv(reporter);
	struct rvu_nix_event_ctx *nix_event_ctx = ctx;
	int blkaddr;

	blkaddr = rvu_get_blkaddr(rvu, BLKTYPE_NIX, 0);
	if (blkaddr < 0)
		return blkaddr;

	if (nix_event_ctx->nix_af_rvu_gen)
		rvu_write64(rvu, blkaddr, NIX_AF_GEN_INT_ENA_W1S, ~0ULL);

	return 0;
}

static int rvu_hw_nix_err_dump(struct devlink_health_reporter *reporter,
			       struct devlink_fmsg *fmsg, void *ctx,
			       struct netlink_ext_ack *netlink_extack)
{
	struct rvu *rvu = devlink_health_reporter_priv(reporter);
	struct rvu_devlink *rvu_dl = rvu->rvu_dl;
	struct rvu_nix_event_ctx *nix_ctx;

	nix_ctx = rvu_dl->rvu_nix_health_reporter->nix_event_ctx;

	return ctx ? rvu_nix_report_show(fmsg, ctx, NIX_AF_RVU_ERR) :
		     rvu_nix_report_show(fmsg, nix_ctx, NIX_AF_RVU_ERR);
}

static int rvu_hw_nix_err_recover(struct devlink_health_reporter *reporter,
				  void *ctx, struct netlink_ext_ack *netlink_extack)
{
	struct rvu *rvu = devlink_health_reporter_priv(reporter);
	struct rvu_nix_event_ctx *nix_event_ctx = ctx;
	int blkaddr;

	blkaddr = rvu_get_blkaddr(rvu, BLKTYPE_NIX, 0);
	if (blkaddr < 0)
		return blkaddr;

	if (nix_event_ctx->nix_af_rvu_err)
		rvu_write64(rvu, blkaddr, NIX_AF_ERR_INT_ENA_W1S, ~0ULL);

	return 0;
}

static int rvu_hw_nix_ras_dump(struct devlink_health_reporter *reporter,
			       struct devlink_fmsg *fmsg, void *ctx,
			       struct netlink_ext_ack *netlink_extack)
{
	struct rvu *rvu = devlink_health_reporter_priv(reporter);
	struct rvu_devlink *rvu_dl = rvu->rvu_dl;
	struct rvu_nix_event_ctx *nix_ctx;

	nix_ctx = rvu_dl->rvu_nix_health_reporter->nix_event_ctx;

	return ctx ? rvu_nix_report_show(fmsg, ctx, NIX_AF_RVU_RAS) :
		     rvu_nix_report_show(fmsg, nix_ctx, NIX_AF_RVU_RAS);
}

static int rvu_hw_nix_ras_recover(struct devlink_health_reporter *reporter,
				  void *ctx, struct netlink_ext_ack *netlink_extack)
{
	struct rvu *rvu = devlink_health_reporter_priv(reporter);
	struct rvu_nix_event_ctx *nix_event_ctx = ctx;
	int blkaddr;

	blkaddr = rvu_get_blkaddr(rvu, BLKTYPE_NIX, 0);
	if (blkaddr < 0)
		return blkaddr;

	if (nix_event_ctx->nix_af_rvu_int)
		rvu_write64(rvu, blkaddr, NIX_AF_RAS_ENA_W1S, ~0ULL);

	return 0;
}

RVU_REPORTERS(hw_nix_intr);
RVU_REPORTERS(hw_nix_gen);
RVU_REPORTERS(hw_nix_err);
RVU_REPORTERS(hw_nix_ras);

static void rvu_nix_health_reporters_destroy(struct rvu_devlink *rvu_dl);

static int rvu_nix_register_reporters(struct rvu_devlink *rvu_dl)
{
	struct rvu_nix_health_reporters *rvu_reporters;
	struct rvu_nix_event_ctx *nix_event_context;
	struct rvu *rvu = rvu_dl->rvu;

	rvu_reporters = kzalloc(sizeof(*rvu_reporters), GFP_KERNEL);
	if (!rvu_reporters)
		return -ENOMEM;

	rvu_dl->rvu_nix_health_reporter = rvu_reporters;
	nix_event_context = kzalloc(sizeof(*nix_event_context), GFP_KERNEL);
	if (!nix_event_context)
		return -ENOMEM;

	rvu_reporters->nix_event_ctx = nix_event_context;
	rvu_reporters->rvu_hw_nix_intr_reporter =
		devlink_health_reporter_create(rvu_dl->dl, &rvu_hw_nix_intr_reporter_ops, 0, rvu);
	if (IS_ERR(rvu_reporters->rvu_hw_nix_intr_reporter)) {
		dev_warn(rvu->dev, "Failed to create hw_nix_intr reporter, err=%ld\n",
			 PTR_ERR(rvu_reporters->rvu_hw_nix_intr_reporter));
		return PTR_ERR(rvu_reporters->rvu_hw_nix_intr_reporter);
	}

	rvu_reporters->rvu_hw_nix_gen_reporter =
		devlink_health_reporter_create(rvu_dl->dl, &rvu_hw_nix_gen_reporter_ops, 0, rvu);
	if (IS_ERR(rvu_reporters->rvu_hw_nix_gen_reporter)) {
		dev_warn(rvu->dev, "Failed to create hw_nix_gen reporter, err=%ld\n",
			 PTR_ERR(rvu_reporters->rvu_hw_nix_gen_reporter));
		return PTR_ERR(rvu_reporters->rvu_hw_nix_gen_reporter);
	}

	rvu_reporters->rvu_hw_nix_err_reporter =
		devlink_health_reporter_create(rvu_dl->dl, &rvu_hw_nix_err_reporter_ops, 0, rvu);
	if (IS_ERR(rvu_reporters->rvu_hw_nix_err_reporter)) {
		dev_warn(rvu->dev, "Failed to create hw_nix_err reporter, err=%ld\n",
			 PTR_ERR(rvu_reporters->rvu_hw_nix_err_reporter));
		return PTR_ERR(rvu_reporters->rvu_hw_nix_err_reporter);
	}

	rvu_reporters->rvu_hw_nix_ras_reporter =
		devlink_health_reporter_create(rvu_dl->dl, &rvu_hw_nix_ras_reporter_ops, 0, rvu);
	if (IS_ERR(rvu_reporters->rvu_hw_nix_ras_reporter)) {
		dev_warn(rvu->dev, "Failed to create hw_nix_ras reporter, err=%ld\n",
			 PTR_ERR(rvu_reporters->rvu_hw_nix_ras_reporter));
		return PTR_ERR(rvu_reporters->rvu_hw_nix_ras_reporter);
	}

	rvu_dl->devlink_wq = create_workqueue("rvu_devlink_wq");
	if (!rvu_dl->devlink_wq)
		goto err;

	INIT_WORK(&rvu_reporters->intr_work, rvu_nix_intr_work);
	INIT_WORK(&rvu_reporters->gen_work, rvu_nix_gen_work);
	INIT_WORK(&rvu_reporters->err_work, rvu_nix_err_work);
	INIT_WORK(&rvu_reporters->ras_work, rvu_nix_ras_work);

	return 0;
err:
	rvu_nix_health_reporters_destroy(rvu_dl);
	return -ENOMEM;
}

static int rvu_nix_health_reporters_create(struct rvu_devlink *rvu_dl)
{
	struct rvu *rvu = rvu_dl->rvu;
	int err;

	err = rvu_nix_register_reporters(rvu_dl);
	if (err) {
		dev_warn(rvu->dev, "Failed to create nix reporter, err =%d\n",
			 err);
		return err;
	}
	rvu_nix_register_interrupts(rvu);

	return 0;
}

static void rvu_nix_health_reporters_destroy(struct rvu_devlink *rvu_dl)
{
	struct rvu_nix_health_reporters *nix_reporters;
	struct rvu *rvu = rvu_dl->rvu;

	nix_reporters = rvu_dl->rvu_nix_health_reporter;

	if (!nix_reporters->rvu_hw_nix_ras_reporter)
		return;
	if (!IS_ERR_OR_NULL(nix_reporters->rvu_hw_nix_intr_reporter))
		devlink_health_reporter_destroy(nix_reporters->rvu_hw_nix_intr_reporter);

	if (!IS_ERR_OR_NULL(nix_reporters->rvu_hw_nix_gen_reporter))
		devlink_health_reporter_destroy(nix_reporters->rvu_hw_nix_gen_reporter);

	if (!IS_ERR_OR_NULL(nix_reporters->rvu_hw_nix_err_reporter))
		devlink_health_reporter_destroy(nix_reporters->rvu_hw_nix_err_reporter);

	if (!IS_ERR_OR_NULL(nix_reporters->rvu_hw_nix_ras_reporter))
		devlink_health_reporter_destroy(nix_reporters->rvu_hw_nix_ras_reporter);

	rvu_nix_unregister_interrupts(rvu);
	kfree(rvu_dl->rvu_nix_health_reporter->nix_event_ctx);
	kfree(rvu_dl->rvu_nix_health_reporter);
}

static void rvu_npa_intr_work(struct work_struct *work)
{
	struct rvu_npa_health_reporters *rvu_npa_health_reporter;

	rvu_npa_health_reporter = container_of(work, struct rvu_npa_health_reporters, intr_work);
	devlink_health_report(rvu_npa_health_reporter->rvu_hw_npa_intr_reporter,
			      "NPA_AF_RVU Error",
			      rvu_npa_health_reporter->npa_event_ctx);
}

static irqreturn_t rvu_npa_af_rvu_intr_handler(int irq, void *rvu_irq)
{
	struct rvu_npa_event_ctx *npa_event_context;
	struct rvu_devlink *rvu_dl = rvu_irq;
	struct rvu *rvu;
	int blkaddr;
	u64 intr;

	rvu = rvu_dl->rvu;
	blkaddr = rvu_get_blkaddr(rvu, BLKTYPE_NPA, 0);
	if (blkaddr < 0)
		return IRQ_NONE;

	npa_event_context = rvu_dl->rvu_npa_health_reporter->npa_event_ctx;
	intr = rvu_read64(rvu, blkaddr, NPA_AF_RVU_INT);
	npa_event_context->npa_af_rvu_int = intr;

	/* Clear interrupts */
	rvu_write64(rvu, blkaddr, NPA_AF_RVU_INT, intr);
	rvu_write64(rvu, blkaddr, NPA_AF_RVU_INT_ENA_W1C, ~0ULL);
	queue_work(rvu_dl->devlink_wq, &rvu_dl->rvu_npa_health_reporter->intr_work);

	return IRQ_HANDLED;
}

static void rvu_npa_gen_work(struct work_struct *work)
{
	struct rvu_npa_health_reporters *rvu_npa_health_reporter;

	rvu_npa_health_reporter = container_of(work, struct rvu_npa_health_reporters, gen_work);
	devlink_health_report(rvu_npa_health_reporter->rvu_hw_npa_gen_reporter,
			      "NPA_AF_GEN Error",
			      rvu_npa_health_reporter->npa_event_ctx);
}

static irqreturn_t rvu_npa_af_gen_intr_handler(int irq, void *rvu_irq)
{
	struct rvu_npa_event_ctx *npa_event_context;
	struct rvu_devlink *rvu_dl = rvu_irq;
	struct rvu *rvu;
	int blkaddr;
	u64 intr;

	rvu = rvu_dl->rvu;
	blkaddr = rvu_get_blkaddr(rvu, BLKTYPE_NPA, 0);
	if (blkaddr < 0)
		return IRQ_NONE;

	npa_event_context = rvu_dl->rvu_npa_health_reporter->npa_event_ctx;
	intr = rvu_read64(rvu, blkaddr, NPA_AF_GEN_INT);
	npa_event_context->npa_af_rvu_gen = intr;

	/* Clear interrupts */
	rvu_write64(rvu, blkaddr, NPA_AF_GEN_INT, intr);
	rvu_write64(rvu, blkaddr, NPA_AF_GEN_INT_ENA_W1C, ~0ULL);
	queue_work(rvu_dl->devlink_wq, &rvu_dl->rvu_npa_health_reporter->gen_work);

	return IRQ_HANDLED;
}

static void rvu_npa_err_work(struct work_struct *work)
{
	struct rvu_npa_health_reporters *rvu_npa_health_reporter;

	rvu_npa_health_reporter = container_of(work, struct rvu_npa_health_reporters, err_work);
	devlink_health_report(rvu_npa_health_reporter->rvu_hw_npa_err_reporter,
			      "NPA_AF_ERR Error",
			      rvu_npa_health_reporter->npa_event_ctx);
}

static irqreturn_t rvu_npa_af_err_intr_handler(int irq, void *rvu_irq)
{
	struct rvu_npa_event_ctx *npa_event_context;
	struct rvu_devlink *rvu_dl = rvu_irq;
	struct rvu *rvu;
	int blkaddr;
	u64 intr;

	rvu = rvu_dl->rvu;
	blkaddr = rvu_get_blkaddr(rvu, BLKTYPE_NPA, 0);
	if (blkaddr < 0)
		return IRQ_NONE;
	npa_event_context = rvu_dl->rvu_npa_health_reporter->npa_event_ctx;
	intr = rvu_read64(rvu, blkaddr, NPA_AF_ERR_INT);
	npa_event_context->npa_af_rvu_err = intr;

	/* Clear interrupts */
	rvu_write64(rvu, blkaddr, NPA_AF_ERR_INT, intr);
	rvu_write64(rvu, blkaddr, NPA_AF_ERR_INT_ENA_W1C, ~0ULL);
	queue_work(rvu_dl->devlink_wq, &rvu_dl->rvu_npa_health_reporter->err_work);

	return IRQ_HANDLED;
}

static void rvu_npa_ras_work(struct work_struct *work)
{
	struct rvu_npa_health_reporters *rvu_npa_health_reporter;

	rvu_npa_health_reporter = container_of(work, struct rvu_npa_health_reporters, ras_work);
	devlink_health_report(rvu_npa_health_reporter->rvu_hw_npa_ras_reporter,
			      "HW NPA_AF_RAS Error reported",
			      rvu_npa_health_reporter->npa_event_ctx);
}

static irqreturn_t rvu_npa_af_ras_intr_handler(int irq, void *rvu_irq)
{
	struct rvu_npa_event_ctx *npa_event_context;
	struct rvu_devlink *rvu_dl = rvu_irq;
	struct rvu *rvu;
	int blkaddr;
	u64 intr;

	rvu = rvu_dl->rvu;
	blkaddr = rvu_get_blkaddr(rvu, BLKTYPE_NPA, 0);
	if (blkaddr < 0)
		return IRQ_NONE;

	npa_event_context = rvu_dl->rvu_npa_health_reporter->npa_event_ctx;
	intr = rvu_read64(rvu, blkaddr, NPA_AF_RAS);
	npa_event_context->npa_af_rvu_ras = intr;

	/* Clear interrupts */
	rvu_write64(rvu, blkaddr, NPA_AF_RAS, intr);
	rvu_write64(rvu, blkaddr, NPA_AF_RAS_ENA_W1C, ~0ULL);
	queue_work(rvu_dl->devlink_wq, &rvu_dl->rvu_npa_health_reporter->ras_work);

	return IRQ_HANDLED;
}

static void rvu_npa_unregister_interrupts(struct rvu *rvu)
{
	struct rvu_devlink *rvu_dl = rvu->rvu_dl;
	int i, offs, blkaddr;
	u64 reg;

	blkaddr = rvu_get_blkaddr(rvu, BLKTYPE_NPA, 0);
	if (blkaddr < 0)
		return;

	reg = rvu_read64(rvu, blkaddr, NPA_PRIV_AF_INT_CFG);
	offs = reg & 0x3FF;

	rvu_write64(rvu, blkaddr, NPA_AF_RVU_INT_ENA_W1C, ~0ULL);
	rvu_write64(rvu, blkaddr, NPA_AF_GEN_INT_ENA_W1C, ~0ULL);
	rvu_write64(rvu, blkaddr, NPA_AF_ERR_INT_ENA_W1C, ~0ULL);
	rvu_write64(rvu, blkaddr, NPA_AF_RAS_ENA_W1C, ~0ULL);

	for (i = 0; i < NPA_AF_INT_VEC_CNT; i++)
		if (rvu->irq_allocated[offs + i]) {
			free_irq(pci_irq_vector(rvu->pdev, offs + i), rvu_dl);
			rvu->irq_allocated[offs + i] = false;
		}
}

static int rvu_npa_register_interrupts(struct rvu *rvu)
{
	int blkaddr, base;
	bool rc;

	blkaddr = rvu_get_blkaddr(rvu, BLKTYPE_NPA, 0);
	if (blkaddr < 0)
		return blkaddr;

	/* Get NPA AF MSIX vectors offset. */
	base = rvu_read64(rvu, blkaddr, NPA_PRIV_AF_INT_CFG) & 0x3ff;
	if (!base) {
		dev_warn(rvu->dev,
			 "Failed to get NPA_AF_INT vector offsets\n");
		return 0;
	}

	/* Register and enable NPA_AF_RVU_INT interrupt */
	rc = rvu_common_request_irq(rvu, base +  NPA_AF_INT_VEC_RVU,
				    "NPA_AF_RVU_INT",
				    rvu_npa_af_rvu_intr_handler);
	if (!rc)
		goto err;
	rvu_write64(rvu, blkaddr, NPA_AF_RVU_INT_ENA_W1S, ~0ULL);

	/* Register and enable NPA_AF_GEN_INT interrupt */
	rc = rvu_common_request_irq(rvu, base + NPA_AF_INT_VEC_GEN,
				    "NPA_AF_RVU_GEN",
				    rvu_npa_af_gen_intr_handler);
	if (!rc)
		goto err;
	rvu_write64(rvu, blkaddr, NPA_AF_GEN_INT_ENA_W1S, ~0ULL);

	/* Register and enable NPA_AF_ERR_INT interrupt */
	rc = rvu_common_request_irq(rvu, base + NPA_AF_INT_VEC_AF_ERR,
				    "NPA_AF_ERR_INT",
				    rvu_npa_af_err_intr_handler);
	if (!rc)
		goto err;
	rvu_write64(rvu, blkaddr, NPA_AF_ERR_INT_ENA_W1S, ~0ULL);

	/* Register and enable NPA_AF_RAS interrupt */
	rc = rvu_common_request_irq(rvu, base + NPA_AF_INT_VEC_POISON,
				    "NPA_AF_RAS",
				    rvu_npa_af_ras_intr_handler);
	if (!rc)
		goto err;
	rvu_write64(rvu, blkaddr, NPA_AF_RAS_ENA_W1S, ~0ULL);

	return 0;
err:
	rvu_npa_unregister_interrupts(rvu);
	return rc;
}

static int rvu_npa_report_show(struct devlink_fmsg *fmsg, void *ctx,
			       enum npa_af_rvu_health health_reporter)
{
	struct rvu_npa_event_ctx *npa_event_context;
	unsigned int alloc_dis, free_dis;
	u64 intr_val;
	int err;

	npa_event_context = ctx;
	switch (health_reporter) {
	case NPA_AF_RVU_GEN:
		intr_val = npa_event_context->npa_af_rvu_gen;
		err = rvu_report_pair_start(fmsg, "NPA_AF_GENERAL");
		if (err)
			return err;
		err = devlink_fmsg_u64_pair_put(fmsg, "\tNPA General Interrupt Reg ",
						npa_event_context->npa_af_rvu_gen);
		if (err)
			return err;
		if (intr_val & BIT_ULL(32)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tUnmap PF Error");
			if (err)
				return err;
		}

		free_dis = FIELD_GET(GENMASK(15, 0), intr_val);
		if (free_dis & BIT(NPA_INPQ_NIX0_RX)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tNIX0: free disabled RX");
			if (err)
				return err;
		}
		if (free_dis & BIT(NPA_INPQ_NIX0_TX)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tNIX0:free disabled TX");
			if (err)
				return err;
		}
		if (free_dis & BIT(NPA_INPQ_NIX1_RX)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tNIX1: free disabled RX");
			if (err)
				return err;
		}
		if (free_dis & BIT(NPA_INPQ_NIX1_TX)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tNIX1:free disabled TX");
			if (err)
				return err;
		}
		if (free_dis & BIT(NPA_INPQ_SSO)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tFree Disabled for SSO");
			if (err)
				return err;
		}
		if (free_dis & BIT(NPA_INPQ_TIM)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tFree Disabled for TIM");
			if (err)
				return err;
		}
		if (free_dis & BIT(NPA_INPQ_DPI)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tFree Disabled for DPI");
			if (err)
				return err;
		}
		if (free_dis & BIT(NPA_INPQ_AURA_OP)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tFree Disabled for AURA");
			if (err)
				return err;
		}

		alloc_dis = FIELD_GET(GENMASK(31, 16), intr_val);
		if (alloc_dis & BIT(NPA_INPQ_NIX0_RX)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tNIX0: alloc disabled RX");
			if (err)
				return err;
		}
		if (alloc_dis & BIT(NPA_INPQ_NIX0_TX)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tNIX0:alloc disabled TX");
			if (err)
				return err;
		}
		if (alloc_dis & BIT(NPA_INPQ_NIX1_RX)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tNIX1: alloc disabled RX");
			if (err)
				return err;
		}
		if (alloc_dis & BIT(NPA_INPQ_NIX1_TX)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tNIX1:alloc disabled TX");
			if (err)
				return err;
		}
		if (alloc_dis & BIT(NPA_INPQ_SSO)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tAlloc Disabled for SSO");
			if (err)
				return err;
		}
		if (alloc_dis & BIT(NPA_INPQ_TIM)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tAlloc Disabled for TIM");
			if (err)
				return err;
		}
		if (alloc_dis & BIT(NPA_INPQ_DPI)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tAlloc Disabled for DPI");
			if (err)
				return err;
		}
		if (alloc_dis & BIT(NPA_INPQ_AURA_OP)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tAlloc Disabled for AURA");
			if (err)
				return err;
		}
		err = rvu_report_pair_end(fmsg);
		if (err)
			return err;
		break;
	case NPA_AF_RVU_ERR:
		err = rvu_report_pair_start(fmsg, "NPA_AF_ERR");
		if (err)
			return err;
		err = devlink_fmsg_u64_pair_put(fmsg, "\tNPA Error Interrupt Reg ",
						npa_event_context->npa_af_rvu_err);
		if (err)
			return err;

		if (npa_event_context->npa_af_rvu_err & BIT_ULL(14)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tFault on NPA_AQ_INST_S read");
			if (err)
				return err;
		}
		if (npa_event_context->npa_af_rvu_err & BIT_ULL(13)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tFault on NPA_AQ_RES_S write");
			if (err)
				return err;
		}
		if (npa_event_context->npa_af_rvu_err & BIT_ULL(12)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tAQ Doorbell Error");
			if (err)
				return err;
		}
		err = rvu_report_pair_end(fmsg);
		if (err)
			return err;
		break;
	case NPA_AF_RVU_RAS:
		err = rvu_report_pair_start(fmsg, "NPA_AF_RVU_RAS");
		if (err)
			return err;
		err = devlink_fmsg_u64_pair_put(fmsg, "\tNPA RAS Interrupt Reg ",
						npa_event_context->npa_af_rvu_ras);
		if (err)
			return err;
		if (npa_event_context->npa_af_rvu_ras & BIT_ULL(34)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tPoison data on NPA_AQ_INST_S");
			if (err)
				return err;
		}
		if (npa_event_context->npa_af_rvu_ras & BIT_ULL(33)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tPoison data on NPA_AQ_RES_S");
			if (err)
				return err;
		}
		if (npa_event_context->npa_af_rvu_ras & BIT_ULL(32)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tPoison data on HW context");
			if (err)
				return err;
		}
		err = rvu_report_pair_end(fmsg);
		if (err)
			return err;
		break;
	case NPA_AF_RVU_INTR:
		err = rvu_report_pair_start(fmsg, "NPA_AF_RVU");
		if (err)
			return err;
		err = devlink_fmsg_u64_pair_put(fmsg, "\tNPA RVU Interrupt Reg ",
						npa_event_context->npa_af_rvu_int);
		if (err)
			return err;
		if (npa_event_context->npa_af_rvu_int & BIT_ULL(0)) {
			err = devlink_fmsg_string_put(fmsg, "\n\tUnmap Slot Error");
			if (err)
				return err;
		}
		return rvu_report_pair_end(fmsg);
	default:
		return -EINVAL;
	}

	return 0;
}

static int rvu_hw_npa_intr_dump(struct devlink_health_reporter *reporter,
				struct devlink_fmsg *fmsg, void *ctx,
				struct netlink_ext_ack *netlink_extack)
{
	struct rvu *rvu = devlink_health_reporter_priv(reporter);
	struct rvu_devlink *rvu_dl = rvu->rvu_dl;
	struct rvu_npa_event_ctx *npa_ctx;

	npa_ctx = rvu_dl->rvu_npa_health_reporter->npa_event_ctx;

	return ctx ? rvu_npa_report_show(fmsg, ctx, NPA_AF_RVU_INTR) :
		     rvu_npa_report_show(fmsg, npa_ctx, NPA_AF_RVU_INTR);
}

static int rvu_hw_npa_intr_recover(struct devlink_health_reporter *reporter,
				   void *ctx, struct netlink_ext_ack *netlink_extack)
{
	struct rvu *rvu = devlink_health_reporter_priv(reporter);
	struct rvu_npa_event_ctx *npa_event_ctx = ctx;
	int blkaddr;

	blkaddr = rvu_get_blkaddr(rvu, BLKTYPE_NPA, 0);
	if (blkaddr < 0)
		return blkaddr;

	if (npa_event_ctx->npa_af_rvu_int)
		rvu_write64(rvu, blkaddr, NPA_AF_RVU_INT_ENA_W1S, ~0ULL);

	return 0;
}

static int rvu_hw_npa_gen_dump(struct devlink_health_reporter *reporter,
			       struct devlink_fmsg *fmsg, void *ctx,
			       struct netlink_ext_ack *netlink_extack)
{
	struct rvu *rvu = devlink_health_reporter_priv(reporter);
	struct rvu_devlink *rvu_dl = rvu->rvu_dl;
	struct rvu_npa_event_ctx *npa_ctx;

	npa_ctx = rvu_dl->rvu_npa_health_reporter->npa_event_ctx;

	return ctx ? rvu_npa_report_show(fmsg, ctx, NPA_AF_RVU_GEN) :
		     rvu_npa_report_show(fmsg, npa_ctx, NPA_AF_RVU_GEN);
}

static int rvu_hw_npa_gen_recover(struct devlink_health_reporter *reporter,
				  void *ctx, struct netlink_ext_ack *netlink_extack)
{
	struct rvu *rvu = devlink_health_reporter_priv(reporter);
	struct rvu_npa_event_ctx *npa_event_ctx = ctx;
	int blkaddr;

	blkaddr = rvu_get_blkaddr(rvu, BLKTYPE_NPA, 0);
	if (blkaddr < 0)
		return blkaddr;

	if (npa_event_ctx->npa_af_rvu_gen)
		rvu_write64(rvu, blkaddr, NPA_AF_GEN_INT_ENA_W1S, ~0ULL);

	return 0;
}

static int rvu_hw_npa_err_dump(struct devlink_health_reporter *reporter,
			       struct devlink_fmsg *fmsg, void *ctx,
			       struct netlink_ext_ack *netlink_extack)
{
	struct rvu *rvu = devlink_health_reporter_priv(reporter);
	struct rvu_devlink *rvu_dl = rvu->rvu_dl;
	struct rvu_npa_event_ctx *npa_ctx;

	npa_ctx = rvu_dl->rvu_npa_health_reporter->npa_event_ctx;

	return ctx ? rvu_npa_report_show(fmsg, ctx, NPA_AF_RVU_ERR) :
		     rvu_npa_report_show(fmsg, npa_ctx, NPA_AF_RVU_ERR);
}

static int rvu_hw_npa_err_recover(struct devlink_health_reporter *reporter,
				  void *ctx, struct netlink_ext_ack *netlink_extack)
{
	struct rvu *rvu = devlink_health_reporter_priv(reporter);
	struct rvu_npa_event_ctx *npa_event_ctx = ctx;
	int blkaddr;

	blkaddr = rvu_get_blkaddr(rvu, BLKTYPE_NPA, 0);
	if (blkaddr < 0)
		return blkaddr;

	if (npa_event_ctx->npa_af_rvu_err)
		rvu_write64(rvu, blkaddr, NPA_AF_ERR_INT_ENA_W1S, ~0ULL);

	return 0;
}

static int rvu_hw_npa_ras_dump(struct devlink_health_reporter *reporter,
			       struct devlink_fmsg *fmsg, void *ctx,
			       struct netlink_ext_ack *netlink_extack)
{
	struct rvu *rvu = devlink_health_reporter_priv(reporter);
	struct rvu_devlink *rvu_dl = rvu->rvu_dl;
	struct rvu_npa_event_ctx *npa_ctx;

	npa_ctx = rvu_dl->rvu_npa_health_reporter->npa_event_ctx;

	return ctx ? rvu_npa_report_show(fmsg, ctx, NPA_AF_RVU_RAS) :
		     rvu_npa_report_show(fmsg, npa_ctx, NPA_AF_RVU_RAS);
}

static int rvu_hw_npa_ras_recover(struct devlink_health_reporter *reporter,
				  void *ctx, struct netlink_ext_ack *netlink_extack)
{
	struct rvu *rvu = devlink_health_reporter_priv(reporter);
	struct rvu_npa_event_ctx *npa_event_ctx = ctx;
	int blkaddr;

	blkaddr = rvu_get_blkaddr(rvu, BLKTYPE_NPA, 0);
	if (blkaddr < 0)
		return blkaddr;

	if (npa_event_ctx->npa_af_rvu_ras)
		rvu_write64(rvu, blkaddr, NPA_AF_RAS_ENA_W1S, ~0ULL);

	return 0;
}

RVU_REPORTERS(hw_npa_intr);
RVU_REPORTERS(hw_npa_gen);
RVU_REPORTERS(hw_npa_err);
RVU_REPORTERS(hw_npa_ras);

static void rvu_npa_health_reporters_destroy(struct rvu_devlink *rvu_dl);

static int rvu_npa_register_reporters(struct rvu_devlink *rvu_dl)
{
	struct rvu_npa_health_reporters *rvu_reporters;
	struct rvu_npa_event_ctx *npa_event_context;
	struct rvu *rvu = rvu_dl->rvu;

	rvu_reporters = kzalloc(sizeof(*rvu_reporters), GFP_KERNEL);
	if (!rvu_reporters)
		return -ENOMEM;

	rvu_dl->rvu_npa_health_reporter = rvu_reporters;
	npa_event_context = kzalloc(sizeof(*npa_event_context), GFP_KERNEL);
	if (!npa_event_context)
		return -ENOMEM;

	rvu_reporters->npa_event_ctx = npa_event_context;
	rvu_reporters->rvu_hw_npa_intr_reporter =
		devlink_health_reporter_create(rvu_dl->dl, &rvu_hw_npa_intr_reporter_ops, 0, rvu);
	if (IS_ERR(rvu_reporters->rvu_hw_npa_intr_reporter)) {
		dev_warn(rvu->dev, "Failed to create hw_npa_intr reporter, err=%ld\n",
			 PTR_ERR(rvu_reporters->rvu_hw_npa_intr_reporter));
		return PTR_ERR(rvu_reporters->rvu_hw_npa_intr_reporter);
	}

	rvu_reporters->rvu_hw_npa_gen_reporter =
		devlink_health_reporter_create(rvu_dl->dl, &rvu_hw_npa_gen_reporter_ops, 0, rvu);
	if (IS_ERR(rvu_reporters->rvu_hw_npa_gen_reporter)) {
		dev_warn(rvu->dev, "Failed to create hw_npa_gen reporter, err=%ld\n",
			 PTR_ERR(rvu_reporters->rvu_hw_npa_gen_reporter));
		return PTR_ERR(rvu_reporters->rvu_hw_npa_gen_reporter);
	}

	rvu_reporters->rvu_hw_npa_err_reporter =
		devlink_health_reporter_create(rvu_dl->dl, &rvu_hw_npa_err_reporter_ops, 0, rvu);
	if (IS_ERR(rvu_reporters->rvu_hw_npa_err_reporter)) {
		dev_warn(rvu->dev, "Failed to create hw_npa_err reporter, err=%ld\n",
			 PTR_ERR(rvu_reporters->rvu_hw_npa_err_reporter));
		return PTR_ERR(rvu_reporters->rvu_hw_npa_err_reporter);
	}

	rvu_reporters->rvu_hw_npa_ras_reporter =
		devlink_health_reporter_create(rvu_dl->dl, &rvu_hw_npa_ras_reporter_ops, 0, rvu);
	if (IS_ERR(rvu_reporters->rvu_hw_npa_ras_reporter)) {
		dev_warn(rvu->dev, "Failed to create hw_npa_ras reporter, err=%ld\n",
			 PTR_ERR(rvu_reporters->rvu_hw_npa_ras_reporter));
		return PTR_ERR(rvu_reporters->rvu_hw_npa_ras_reporter);
	}

	rvu_dl->devlink_wq = create_workqueue("rvu_devlink_wq");
	if (!rvu_dl->devlink_wq)
		goto err;

	INIT_WORK(&rvu_reporters->intr_work, rvu_npa_intr_work);
	INIT_WORK(&rvu_reporters->err_work, rvu_npa_err_work);
	INIT_WORK(&rvu_reporters->gen_work, rvu_npa_gen_work);
	INIT_WORK(&rvu_reporters->ras_work, rvu_npa_ras_work);

	return 0;
err:
	rvu_npa_health_reporters_destroy(rvu_dl);
	return -ENOMEM;
}

static int rvu_npa_health_reporters_create(struct rvu_devlink *rvu_dl)
{
	struct rvu *rvu = rvu_dl->rvu;
	int err;

	err = rvu_npa_register_reporters(rvu_dl);
	if (err) {
		dev_warn(rvu->dev, "Failed to create npa reporter, err =%d\n",
			 err);
		return err;
	}
	rvu_npa_register_interrupts(rvu);

	return 0;
}

static void rvu_npa_health_reporters_destroy(struct rvu_devlink *rvu_dl)
{
	struct rvu_npa_health_reporters *npa_reporters;
	struct rvu *rvu = rvu_dl->rvu;

	npa_reporters = rvu_dl->rvu_npa_health_reporter;

	if (!npa_reporters->rvu_hw_npa_ras_reporter)
		return;
	if (!IS_ERR_OR_NULL(npa_reporters->rvu_hw_npa_intr_reporter))
		devlink_health_reporter_destroy(npa_reporters->rvu_hw_npa_intr_reporter);

	if (!IS_ERR_OR_NULL(npa_reporters->rvu_hw_npa_gen_reporter))
		devlink_health_reporter_destroy(npa_reporters->rvu_hw_npa_gen_reporter);

	if (!IS_ERR_OR_NULL(npa_reporters->rvu_hw_npa_err_reporter))
		devlink_health_reporter_destroy(npa_reporters->rvu_hw_npa_err_reporter);

	if (!IS_ERR_OR_NULL(npa_reporters->rvu_hw_npa_ras_reporter))
		devlink_health_reporter_destroy(npa_reporters->rvu_hw_npa_ras_reporter);

	rvu_npa_unregister_interrupts(rvu);
	kfree(rvu_dl->rvu_npa_health_reporter->npa_event_ctx);
	kfree(rvu_dl->rvu_npa_health_reporter);
}

static int rvu_health_reporters_create(struct rvu *rvu)
{
	struct rvu_devlink *rvu_dl;
	int err;

	rvu_dl = rvu->rvu_dl;
	err = rvu_npa_health_reporters_create(rvu_dl);
	if (err)
		return err;

	return rvu_nix_health_reporters_create(rvu_dl);
}

static void rvu_health_reporters_destroy(struct rvu *rvu)
{
	struct rvu_devlink *rvu_dl;

	if (!rvu->rvu_dl)
		return;

	rvu_dl = rvu->rvu_dl;
	rvu_npa_health_reporters_destroy(rvu_dl);
	rvu_nix_health_reporters_destroy(rvu_dl);
}

static int rvu_devlink_info_get(struct devlink *devlink, struct devlink_info_req *req,
				struct netlink_ext_ack *extack)
{
	return devlink_info_driver_name_put(req, DRV_NAME);
}

static const struct devlink_ops rvu_devlink_ops = {
	.info_get = rvu_devlink_info_get,
};

int rvu_register_dl(struct rvu *rvu)
{
	struct rvu_devlink *rvu_dl;
	struct devlink *dl;
	int err;

	rvu_dl = kzalloc(sizeof(*rvu_dl), GFP_KERNEL);
	if (!rvu_dl)
		return -ENOMEM;

	dl = devlink_alloc(&rvu_devlink_ops, sizeof(struct rvu_devlink));
	if (!dl) {
		dev_warn(rvu->dev, "devlink_alloc failed\n");
		kfree(rvu_dl);
		return -ENOMEM;
	}

	err = devlink_register(dl, rvu->dev);
	if (err) {
		dev_err(rvu->dev, "devlink register failed with error %d\n", err);
		devlink_free(dl);
		kfree(rvu_dl);
		return err;
	}

	rvu_dl->dl = dl;
	rvu_dl->rvu = rvu;
	rvu->rvu_dl = rvu_dl;

	return rvu_health_reporters_create(rvu);
}

void rvu_unregister_dl(struct rvu *rvu)
{
	struct rvu_devlink *rvu_dl = rvu->rvu_dl;
	struct devlink *dl = rvu_dl->dl;

	if (!dl)
		return;

	rvu_health_reporters_destroy(rvu);
	devlink_unregister(dl);
	devlink_free(dl);
	kfree(rvu_dl);
}
