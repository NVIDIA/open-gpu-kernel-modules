// SPDX-License-Identifier: GPL-2.0-only
/*
 * SHA-256 routines supporting the Power 7+ Nest Accelerators driver
 *
 * Copyright (C) 2011-2012 International Business Machines Inc.
 *
 * Author: Kent Yoder <yoder1@us.ibm.com>
 */

#include <crypto/internal/hash.h>
#include <crypto/sha2.h>
#include <linux/module.h>
#include <asm/vio.h>
#include <asm/byteorder.h>

#include "nx_csbcpb.h"
#include "nx.h"


static int nx_crypto_ctx_sha256_init(struct crypto_tfm *tfm)
{
	struct nx_crypto_ctx *nx_ctx = crypto_tfm_ctx(tfm);
	int err;

	err = nx_crypto_ctx_sha_init(tfm);
	if (err)
		return err;

	nx_ctx_init(nx_ctx, HCOP_FC_SHA);

	nx_ctx->ap = &nx_ctx->props[NX_PROPS_SHA256];

	NX_CPB_SET_DIGEST_SIZE(nx_ctx->csbcpb, NX_DS_SHA256);

	return 0;
}

static int nx_sha256_init(struct shash_desc *desc) {
	struct sha256_state *sctx = shash_desc_ctx(desc);

	memset(sctx, 0, sizeof *sctx);

	sctx->state[0] = __cpu_to_be32(SHA256_H0);
	sctx->state[1] = __cpu_to_be32(SHA256_H1);
	sctx->state[2] = __cpu_to_be32(SHA256_H2);
	sctx->state[3] = __cpu_to_be32(SHA256_H3);
	sctx->state[4] = __cpu_to_be32(SHA256_H4);
	sctx->state[5] = __cpu_to_be32(SHA256_H5);
	sctx->state[6] = __cpu_to_be32(SHA256_H6);
	sctx->state[7] = __cpu_to_be32(SHA256_H7);
	sctx->count = 0;

	return 0;
}

static int nx_sha256_update(struct shash_desc *desc, const u8 *data,
			    unsigned int len)
{
	struct sha256_state *sctx = shash_desc_ctx(desc);
	struct nx_crypto_ctx *nx_ctx = crypto_tfm_ctx(&desc->tfm->base);
	struct nx_csbcpb *csbcpb = (struct nx_csbcpb *)nx_ctx->csbcpb;
	struct nx_sg *out_sg;
	u64 to_process = 0, leftover, total;
	unsigned long irq_flags;
	int rc = 0;
	int data_len;
	u32 max_sg_len;
	u64 buf_len = (sctx->count % SHA256_BLOCK_SIZE);

	spin_lock_irqsave(&nx_ctx->lock, irq_flags);

	/* 2 cases for total data len:
	 *  1: < SHA256_BLOCK_SIZE: copy into state, return 0
	 *  2: >= SHA256_BLOCK_SIZE: process X blocks, copy in leftover
	 */
	total = (sctx->count % SHA256_BLOCK_SIZE) + len;
	if (total < SHA256_BLOCK_SIZE) {
		memcpy(sctx->buf + buf_len, data, len);
		sctx->count += len;
		goto out;
	}

	memcpy(csbcpb->cpb.sha256.message_digest, sctx->state, SHA256_DIGEST_SIZE);
	NX_CPB_FDM(csbcpb) |= NX_FDM_INTERMEDIATE;
	NX_CPB_FDM(csbcpb) |= NX_FDM_CONTINUATION;

	max_sg_len = min_t(u64, nx_ctx->ap->sglen,
			nx_driver.of.max_sg_len/sizeof(struct nx_sg));
	max_sg_len = min_t(u64, max_sg_len,
			nx_ctx->ap->databytelen/NX_PAGE_SIZE);

	data_len = SHA256_DIGEST_SIZE;
	out_sg = nx_build_sg_list(nx_ctx->out_sg, (u8 *)sctx->state,
				  &data_len, max_sg_len);
	nx_ctx->op.outlen = (nx_ctx->out_sg - out_sg) * sizeof(struct nx_sg);

	if (data_len != SHA256_DIGEST_SIZE) {
		rc = -EINVAL;
		goto out;
	}

	do {
		int used_sgs = 0;
		struct nx_sg *in_sg = nx_ctx->in_sg;

		if (buf_len) {
			data_len = buf_len;
			in_sg = nx_build_sg_list(in_sg,
						 (u8 *) sctx->buf,
						 &data_len,
						 max_sg_len);

			if (data_len != buf_len) {
				rc = -EINVAL;
				goto out;
			}
			used_sgs = in_sg - nx_ctx->in_sg;
		}

		/* to_process: SHA256_BLOCK_SIZE aligned chunk to be
		 * processed in this iteration. This value is restricted
		 * by sg list limits and number of sgs we already used
		 * for leftover data. (see above)
		 * In ideal case, we could allow NX_PAGE_SIZE * max_sg_len,
		 * but because data may not be aligned, we need to account
		 * for that too. */
		to_process = min_t(u64, total,
			(max_sg_len - 1 - used_sgs) * NX_PAGE_SIZE);
		to_process = to_process & ~(SHA256_BLOCK_SIZE - 1);

		data_len = to_process - buf_len;
		in_sg = nx_build_sg_list(in_sg, (u8 *) data,
					 &data_len, max_sg_len);

		nx_ctx->op.inlen = (nx_ctx->in_sg - in_sg) * sizeof(struct nx_sg);

		to_process = data_len + buf_len;
		leftover = total - to_process;

		/*
		 * we've hit the nx chip previously and we're updating
		 * again, so copy over the partial digest.
		 */
		memcpy(csbcpb->cpb.sha256.input_partial_digest,
			       csbcpb->cpb.sha256.message_digest,
			       SHA256_DIGEST_SIZE);

		if (!nx_ctx->op.inlen || !nx_ctx->op.outlen) {
			rc = -EINVAL;
			goto out;
		}

		rc = nx_hcall_sync(nx_ctx, &nx_ctx->op, 0);
		if (rc)
			goto out;

		atomic_inc(&(nx_ctx->stats->sha256_ops));

		total -= to_process;
		data += to_process - buf_len;
		buf_len = 0;

	} while (leftover >= SHA256_BLOCK_SIZE);

	/* copy the leftover back into the state struct */
	if (leftover)
		memcpy(sctx->buf, data, leftover);

	sctx->count += len;
	memcpy(sctx->state, csbcpb->cpb.sha256.message_digest, SHA256_DIGEST_SIZE);
out:
	spin_unlock_irqrestore(&nx_ctx->lock, irq_flags);
	return rc;
}

static int nx_sha256_final(struct shash_desc *desc, u8 *out)
{
	struct sha256_state *sctx = shash_desc_ctx(desc);
	struct nx_crypto_ctx *nx_ctx = crypto_tfm_ctx(&desc->tfm->base);
	struct nx_csbcpb *csbcpb = (struct nx_csbcpb *)nx_ctx->csbcpb;
	struct nx_sg *in_sg, *out_sg;
	unsigned long irq_flags;
	u32 max_sg_len;
	int rc = 0;
	int len;

	spin_lock_irqsave(&nx_ctx->lock, irq_flags);

	max_sg_len = min_t(u64, nx_ctx->ap->sglen,
			nx_driver.of.max_sg_len/sizeof(struct nx_sg));
	max_sg_len = min_t(u64, max_sg_len,
			nx_ctx->ap->databytelen/NX_PAGE_SIZE);

	/* final is represented by continuing the operation and indicating that
	 * this is not an intermediate operation */
	if (sctx->count >= SHA256_BLOCK_SIZE) {
		/* we've hit the nx chip previously, now we're finalizing,
		 * so copy over the partial digest */
		memcpy(csbcpb->cpb.sha256.input_partial_digest, sctx->state, SHA256_DIGEST_SIZE);
		NX_CPB_FDM(csbcpb) &= ~NX_FDM_INTERMEDIATE;
		NX_CPB_FDM(csbcpb) |= NX_FDM_CONTINUATION;
	} else {
		NX_CPB_FDM(csbcpb) &= ~NX_FDM_INTERMEDIATE;
		NX_CPB_FDM(csbcpb) &= ~NX_FDM_CONTINUATION;
	}

	csbcpb->cpb.sha256.message_bit_length = (u64) (sctx->count * 8);

	len = sctx->count & (SHA256_BLOCK_SIZE - 1);
	in_sg = nx_build_sg_list(nx_ctx->in_sg, (u8 *) sctx->buf,
				 &len, max_sg_len);

	if (len != (sctx->count & (SHA256_BLOCK_SIZE - 1))) {
		rc = -EINVAL;
		goto out;
	}

	len = SHA256_DIGEST_SIZE;
	out_sg = nx_build_sg_list(nx_ctx->out_sg, out, &len, max_sg_len);

	if (len != SHA256_DIGEST_SIZE) {
		rc = -EINVAL;
		goto out;
	}

	nx_ctx->op.inlen = (nx_ctx->in_sg - in_sg) * sizeof(struct nx_sg);
	nx_ctx->op.outlen = (nx_ctx->out_sg - out_sg) * sizeof(struct nx_sg);
	if (!nx_ctx->op.outlen) {
		rc = -EINVAL;
		goto out;
	}

	rc = nx_hcall_sync(nx_ctx, &nx_ctx->op, 0);
	if (rc)
		goto out;

	atomic_inc(&(nx_ctx->stats->sha256_ops));

	atomic64_add(sctx->count, &(nx_ctx->stats->sha256_bytes));
	memcpy(out, csbcpb->cpb.sha256.message_digest, SHA256_DIGEST_SIZE);
out:
	spin_unlock_irqrestore(&nx_ctx->lock, irq_flags);
	return rc;
}

static int nx_sha256_export(struct shash_desc *desc, void *out)
{
	struct sha256_state *sctx = shash_desc_ctx(desc);

	memcpy(out, sctx, sizeof(*sctx));

	return 0;
}

static int nx_sha256_import(struct shash_desc *desc, const void *in)
{
	struct sha256_state *sctx = shash_desc_ctx(desc);

	memcpy(sctx, in, sizeof(*sctx));

	return 0;
}

struct shash_alg nx_shash_sha256_alg = {
	.digestsize = SHA256_DIGEST_SIZE,
	.init       = nx_sha256_init,
	.update     = nx_sha256_update,
	.final      = nx_sha256_final,
	.export     = nx_sha256_export,
	.import     = nx_sha256_import,
	.descsize   = sizeof(struct sha256_state),
	.statesize  = sizeof(struct sha256_state),
	.base       = {
		.cra_name        = "sha256",
		.cra_driver_name = "sha256-nx",
		.cra_priority    = 300,
		.cra_blocksize   = SHA256_BLOCK_SIZE,
		.cra_module      = THIS_MODULE,
		.cra_ctxsize     = sizeof(struct nx_crypto_ctx),
		.cra_init        = nx_crypto_ctx_sha256_init,
		.cra_exit        = nx_crypto_ctx_exit,
	}
};
