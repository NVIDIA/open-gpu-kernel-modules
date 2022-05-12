// SPDX-License-Identifier: GPL-2.0-only
/*
 * Accelerated GHASH implementation with ARMv8 vmull.p64 instructions.
 *
 * Copyright (C) 2015 - 2018 Linaro Ltd. <ard.biesheuvel@linaro.org>
 */

#include <asm/hwcap.h>
#include <asm/neon.h>
#include <asm/simd.h>
#include <asm/unaligned.h>
#include <crypto/b128ops.h>
#include <crypto/cryptd.h>
#include <crypto/internal/hash.h>
#include <crypto/internal/simd.h>
#include <crypto/gf128mul.h>
#include <linux/cpufeature.h>
#include <linux/crypto.h>
#include <linux/jump_label.h>
#include <linux/module.h>

MODULE_DESCRIPTION("GHASH hash function using ARMv8 Crypto Extensions");
MODULE_AUTHOR("Ard Biesheuvel <ard.biesheuvel@linaro.org>");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS_CRYPTO("ghash");

#define GHASH_BLOCK_SIZE	16
#define GHASH_DIGEST_SIZE	16

struct ghash_key {
	be128	k;
	u64	h[][2];
};

struct ghash_desc_ctx {
	u64 digest[GHASH_DIGEST_SIZE/sizeof(u64)];
	u8 buf[GHASH_BLOCK_SIZE];
	u32 count;
};

struct ghash_async_ctx {
	struct cryptd_ahash *cryptd_tfm;
};

asmlinkage void pmull_ghash_update_p64(int blocks, u64 dg[], const char *src,
				       u64 const h[][2], const char *head);

asmlinkage void pmull_ghash_update_p8(int blocks, u64 dg[], const char *src,
				      u64 const h[][2], const char *head);

static __ro_after_init DEFINE_STATIC_KEY_FALSE(use_p64);

static int ghash_init(struct shash_desc *desc)
{
	struct ghash_desc_ctx *ctx = shash_desc_ctx(desc);

	*ctx = (struct ghash_desc_ctx){};
	return 0;
}

static void ghash_do_update(int blocks, u64 dg[], const char *src,
			    struct ghash_key *key, const char *head)
{
	if (likely(crypto_simd_usable())) {
		kernel_neon_begin();
		if (static_branch_likely(&use_p64))
			pmull_ghash_update_p64(blocks, dg, src, key->h, head);
		else
			pmull_ghash_update_p8(blocks, dg, src, key->h, head);
		kernel_neon_end();
	} else {
		be128 dst = { cpu_to_be64(dg[1]), cpu_to_be64(dg[0]) };

		do {
			const u8 *in = src;

			if (head) {
				in = head;
				blocks++;
				head = NULL;
			} else {
				src += GHASH_BLOCK_SIZE;
			}

			crypto_xor((u8 *)&dst, in, GHASH_BLOCK_SIZE);
			gf128mul_lle(&dst, &key->k);
		} while (--blocks);

		dg[0] = be64_to_cpu(dst.b);
		dg[1] = be64_to_cpu(dst.a);
	}
}

static int ghash_update(struct shash_desc *desc, const u8 *src,
			unsigned int len)
{
	struct ghash_desc_ctx *ctx = shash_desc_ctx(desc);
	unsigned int partial = ctx->count % GHASH_BLOCK_SIZE;

	ctx->count += len;

	if ((partial + len) >= GHASH_BLOCK_SIZE) {
		struct ghash_key *key = crypto_shash_ctx(desc->tfm);
		int blocks;

		if (partial) {
			int p = GHASH_BLOCK_SIZE - partial;

			memcpy(ctx->buf + partial, src, p);
			src += p;
			len -= p;
		}

		blocks = len / GHASH_BLOCK_SIZE;
		len %= GHASH_BLOCK_SIZE;

		ghash_do_update(blocks, ctx->digest, src, key,
				partial ? ctx->buf : NULL);
		src += blocks * GHASH_BLOCK_SIZE;
		partial = 0;
	}
	if (len)
		memcpy(ctx->buf + partial, src, len);
	return 0;
}

static int ghash_final(struct shash_desc *desc, u8 *dst)
{
	struct ghash_desc_ctx *ctx = shash_desc_ctx(desc);
	unsigned int partial = ctx->count % GHASH_BLOCK_SIZE;

	if (partial) {
		struct ghash_key *key = crypto_shash_ctx(desc->tfm);

		memset(ctx->buf + partial, 0, GHASH_BLOCK_SIZE - partial);
		ghash_do_update(1, ctx->digest, ctx->buf, key, NULL);
	}
	put_unaligned_be64(ctx->digest[1], dst);
	put_unaligned_be64(ctx->digest[0], dst + 8);

	*ctx = (struct ghash_desc_ctx){};
	return 0;
}

static void ghash_reflect(u64 h[], const be128 *k)
{
	u64 carry = be64_to_cpu(k->a) >> 63;

	h[0] = (be64_to_cpu(k->b) << 1) | carry;
	h[1] = (be64_to_cpu(k->a) << 1) | (be64_to_cpu(k->b) >> 63);

	if (carry)
		h[1] ^= 0xc200000000000000UL;
}

static int ghash_setkey(struct crypto_shash *tfm,
			const u8 *inkey, unsigned int keylen)
{
	struct ghash_key *key = crypto_shash_ctx(tfm);

	if (keylen != GHASH_BLOCK_SIZE)
		return -EINVAL;

	/* needed for the fallback */
	memcpy(&key->k, inkey, GHASH_BLOCK_SIZE);
	ghash_reflect(key->h[0], &key->k);

	if (static_branch_likely(&use_p64)) {
		be128 h = key->k;

		gf128mul_lle(&h, &key->k);
		ghash_reflect(key->h[1], &h);

		gf128mul_lle(&h, &key->k);
		ghash_reflect(key->h[2], &h);

		gf128mul_lle(&h, &key->k);
		ghash_reflect(key->h[3], &h);
	}
	return 0;
}

static struct shash_alg ghash_alg = {
	.digestsize		= GHASH_DIGEST_SIZE,
	.init			= ghash_init,
	.update			= ghash_update,
	.final			= ghash_final,
	.setkey			= ghash_setkey,
	.descsize		= sizeof(struct ghash_desc_ctx),

	.base.cra_name		= "ghash",
	.base.cra_driver_name	= "ghash-ce-sync",
	.base.cra_priority	= 300 - 1,
	.base.cra_blocksize	= GHASH_BLOCK_SIZE,
	.base.cra_ctxsize	= sizeof(struct ghash_key) + sizeof(u64[2]),
	.base.cra_module	= THIS_MODULE,
};

static int ghash_async_init(struct ahash_request *req)
{
	struct crypto_ahash *tfm = crypto_ahash_reqtfm(req);
	struct ghash_async_ctx *ctx = crypto_ahash_ctx(tfm);
	struct ahash_request *cryptd_req = ahash_request_ctx(req);
	struct cryptd_ahash *cryptd_tfm = ctx->cryptd_tfm;
	struct shash_desc *desc = cryptd_shash_desc(cryptd_req);
	struct crypto_shash *child = cryptd_ahash_child(cryptd_tfm);

	desc->tfm = child;
	return crypto_shash_init(desc);
}

static int ghash_async_update(struct ahash_request *req)
{
	struct ahash_request *cryptd_req = ahash_request_ctx(req);
	struct crypto_ahash *tfm = crypto_ahash_reqtfm(req);
	struct ghash_async_ctx *ctx = crypto_ahash_ctx(tfm);
	struct cryptd_ahash *cryptd_tfm = ctx->cryptd_tfm;

	if (!crypto_simd_usable() ||
	    (in_atomic() && cryptd_ahash_queued(cryptd_tfm))) {
		memcpy(cryptd_req, req, sizeof(*req));
		ahash_request_set_tfm(cryptd_req, &cryptd_tfm->base);
		return crypto_ahash_update(cryptd_req);
	} else {
		struct shash_desc *desc = cryptd_shash_desc(cryptd_req);
		return shash_ahash_update(req, desc);
	}
}

static int ghash_async_final(struct ahash_request *req)
{
	struct ahash_request *cryptd_req = ahash_request_ctx(req);
	struct crypto_ahash *tfm = crypto_ahash_reqtfm(req);
	struct ghash_async_ctx *ctx = crypto_ahash_ctx(tfm);
	struct cryptd_ahash *cryptd_tfm = ctx->cryptd_tfm;

	if (!crypto_simd_usable() ||
	    (in_atomic() && cryptd_ahash_queued(cryptd_tfm))) {
		memcpy(cryptd_req, req, sizeof(*req));
		ahash_request_set_tfm(cryptd_req, &cryptd_tfm->base);
		return crypto_ahash_final(cryptd_req);
	} else {
		struct shash_desc *desc = cryptd_shash_desc(cryptd_req);
		return crypto_shash_final(desc, req->result);
	}
}

static int ghash_async_digest(struct ahash_request *req)
{
	struct crypto_ahash *tfm = crypto_ahash_reqtfm(req);
	struct ghash_async_ctx *ctx = crypto_ahash_ctx(tfm);
	struct ahash_request *cryptd_req = ahash_request_ctx(req);
	struct cryptd_ahash *cryptd_tfm = ctx->cryptd_tfm;

	if (!crypto_simd_usable() ||
	    (in_atomic() && cryptd_ahash_queued(cryptd_tfm))) {
		memcpy(cryptd_req, req, sizeof(*req));
		ahash_request_set_tfm(cryptd_req, &cryptd_tfm->base);
		return crypto_ahash_digest(cryptd_req);
	} else {
		struct shash_desc *desc = cryptd_shash_desc(cryptd_req);
		struct crypto_shash *child = cryptd_ahash_child(cryptd_tfm);

		desc->tfm = child;
		return shash_ahash_digest(req, desc);
	}
}

static int ghash_async_import(struct ahash_request *req, const void *in)
{
	struct ahash_request *cryptd_req = ahash_request_ctx(req);
	struct crypto_ahash *tfm = crypto_ahash_reqtfm(req);
	struct ghash_async_ctx *ctx = crypto_ahash_ctx(tfm);
	struct shash_desc *desc = cryptd_shash_desc(cryptd_req);

	desc->tfm = cryptd_ahash_child(ctx->cryptd_tfm);

	return crypto_shash_import(desc, in);
}

static int ghash_async_export(struct ahash_request *req, void *out)
{
	struct ahash_request *cryptd_req = ahash_request_ctx(req);
	struct shash_desc *desc = cryptd_shash_desc(cryptd_req);

	return crypto_shash_export(desc, out);
}

static int ghash_async_setkey(struct crypto_ahash *tfm, const u8 *key,
			      unsigned int keylen)
{
	struct ghash_async_ctx *ctx = crypto_ahash_ctx(tfm);
	struct crypto_ahash *child = &ctx->cryptd_tfm->base;

	crypto_ahash_clear_flags(child, CRYPTO_TFM_REQ_MASK);
	crypto_ahash_set_flags(child, crypto_ahash_get_flags(tfm)
			       & CRYPTO_TFM_REQ_MASK);
	return crypto_ahash_setkey(child, key, keylen);
}

static int ghash_async_init_tfm(struct crypto_tfm *tfm)
{
	struct cryptd_ahash *cryptd_tfm;
	struct ghash_async_ctx *ctx = crypto_tfm_ctx(tfm);

	cryptd_tfm = cryptd_alloc_ahash("ghash-ce-sync", 0, 0);
	if (IS_ERR(cryptd_tfm))
		return PTR_ERR(cryptd_tfm);
	ctx->cryptd_tfm = cryptd_tfm;
	crypto_ahash_set_reqsize(__crypto_ahash_cast(tfm),
				 sizeof(struct ahash_request) +
				 crypto_ahash_reqsize(&cryptd_tfm->base));

	return 0;
}

static void ghash_async_exit_tfm(struct crypto_tfm *tfm)
{
	struct ghash_async_ctx *ctx = crypto_tfm_ctx(tfm);

	cryptd_free_ahash(ctx->cryptd_tfm);
}

static struct ahash_alg ghash_async_alg = {
	.init			= ghash_async_init,
	.update			= ghash_async_update,
	.final			= ghash_async_final,
	.setkey			= ghash_async_setkey,
	.digest			= ghash_async_digest,
	.import			= ghash_async_import,
	.export			= ghash_async_export,
	.halg.digestsize	= GHASH_DIGEST_SIZE,
	.halg.statesize		= sizeof(struct ghash_desc_ctx),
	.halg.base		= {
		.cra_name	= "ghash",
		.cra_driver_name = "ghash-ce",
		.cra_priority	= 300,
		.cra_flags	= CRYPTO_ALG_ASYNC,
		.cra_blocksize	= GHASH_BLOCK_SIZE,
		.cra_ctxsize	= sizeof(struct ghash_async_ctx),
		.cra_module	= THIS_MODULE,
		.cra_init	= ghash_async_init_tfm,
		.cra_exit	= ghash_async_exit_tfm,
	},
};

static int __init ghash_ce_mod_init(void)
{
	int err;

	if (!(elf_hwcap & HWCAP_NEON))
		return -ENODEV;

	if (elf_hwcap2 & HWCAP2_PMULL) {
		ghash_alg.base.cra_ctxsize += 3 * sizeof(u64[2]);
		static_branch_enable(&use_p64);
	}

	err = crypto_register_shash(&ghash_alg);
	if (err)
		return err;
	err = crypto_register_ahash(&ghash_async_alg);
	if (err)
		goto err_shash;

	return 0;

err_shash:
	crypto_unregister_shash(&ghash_alg);
	return err;
}

static void __exit ghash_ce_mod_exit(void)
{
	crypto_unregister_ahash(&ghash_async_alg);
	crypto_unregister_shash(&ghash_alg);
}

module_init(ghash_ce_mod_init);
module_exit(ghash_ce_mod_exit);
