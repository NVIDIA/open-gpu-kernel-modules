// SPDX-License-Identifier: GPL-2.0-or-later
/* LRW: as defined by Cyril Guyot in
 *	http://grouper.ieee.org/groups/1619/email/pdf00017.pdf
 *
 * Copyright (c) 2006 Rik Snel <rsnel@cube.dyndns.org>
 *
 * Based on ecb.c
 * Copyright (c) 2006 Herbert Xu <herbert@gondor.apana.org.au>
 */
/* This implementation is checked against the test vectors in the above
 * document and by a test vector provided by Ken Buchanan at
 * https://www.mail-archive.com/stds-p1619@listserv.ieee.org/msg00173.html
 *
 * The test vectors are included in the testing module tcrypt.[ch] */

#include <crypto/internal/skcipher.h>
#include <crypto/scatterwalk.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/scatterlist.h>
#include <linux/slab.h>

#include <crypto/b128ops.h>
#include <crypto/gf128mul.h>

#define LRW_BLOCK_SIZE 16

struct lrw_tfm_ctx {
	struct crypto_skcipher *child;

	/*
	 * optimizes multiplying a random (non incrementing, as at the
	 * start of a new sector) value with key2, we could also have
	 * used 4k optimization tables or no optimization at all. In the
	 * latter case we would have to store key2 here
	 */
	struct gf128mul_64k *table;

	/*
	 * stores:
	 *  key2*{ 0,0,...0,0,0,0,1 }, key2*{ 0,0,...0,0,0,1,1 },
	 *  key2*{ 0,0,...0,0,1,1,1 }, key2*{ 0,0,...0,1,1,1,1 }
	 *  key2*{ 0,0,...1,1,1,1,1 }, etc
	 * needed for optimized multiplication of incrementing values
	 * with key2
	 */
	be128 mulinc[128];
};

struct lrw_request_ctx {
	be128 t;
	struct skcipher_request subreq;
};

static inline void lrw_setbit128_bbe(void *b, int bit)
{
	__set_bit(bit ^ (0x80 -
#ifdef __BIG_ENDIAN
			 BITS_PER_LONG
#else
			 BITS_PER_BYTE
#endif
			), b);
}

static int lrw_setkey(struct crypto_skcipher *parent, const u8 *key,
		      unsigned int keylen)
{
	struct lrw_tfm_ctx *ctx = crypto_skcipher_ctx(parent);
	struct crypto_skcipher *child = ctx->child;
	int err, bsize = LRW_BLOCK_SIZE;
	const u8 *tweak = key + keylen - bsize;
	be128 tmp = { 0 };
	int i;

	crypto_skcipher_clear_flags(child, CRYPTO_TFM_REQ_MASK);
	crypto_skcipher_set_flags(child, crypto_skcipher_get_flags(parent) &
					 CRYPTO_TFM_REQ_MASK);
	err = crypto_skcipher_setkey(child, key, keylen - bsize);
	if (err)
		return err;

	if (ctx->table)
		gf128mul_free_64k(ctx->table);

	/* initialize multiplication table for Key2 */
	ctx->table = gf128mul_init_64k_bbe((be128 *)tweak);
	if (!ctx->table)
		return -ENOMEM;

	/* initialize optimization table */
	for (i = 0; i < 128; i++) {
		lrw_setbit128_bbe(&tmp, i);
		ctx->mulinc[i] = tmp;
		gf128mul_64k_bbe(&ctx->mulinc[i], ctx->table);
	}

	return 0;
}

/*
 * Returns the number of trailing '1' bits in the words of the counter, which is
 * represented by 4 32-bit words, arranged from least to most significant.
 * At the same time, increments the counter by one.
 *
 * For example:
 *
 * u32 counter[4] = { 0xFFFFFFFF, 0x1, 0x0, 0x0 };
 * int i = lrw_next_index(&counter);
 * // i == 33, counter == { 0x0, 0x2, 0x0, 0x0 }
 */
static int lrw_next_index(u32 *counter)
{
	int i, res = 0;

	for (i = 0; i < 4; i++) {
		if (counter[i] + 1 != 0)
			return res + ffz(counter[i]++);

		counter[i] = 0;
		res += 32;
	}

	/*
	 * If we get here, then x == 128 and we are incrementing the counter
	 * from all ones to all zeros. This means we must return index 127, i.e.
	 * the one corresponding to key2*{ 1,...,1 }.
	 */
	return 127;
}

/*
 * We compute the tweak masks twice (both before and after the ECB encryption or
 * decryption) to avoid having to allocate a temporary buffer and/or make
 * mutliple calls to the 'ecb(..)' instance, which usually would be slower than
 * just doing the lrw_next_index() calls again.
 */
static int lrw_xor_tweak(struct skcipher_request *req, bool second_pass)
{
	const int bs = LRW_BLOCK_SIZE;
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);
	const struct lrw_tfm_ctx *ctx = crypto_skcipher_ctx(tfm);
	struct lrw_request_ctx *rctx = skcipher_request_ctx(req);
	be128 t = rctx->t;
	struct skcipher_walk w;
	__be32 *iv;
	u32 counter[4];
	int err;

	if (second_pass) {
		req = &rctx->subreq;
		/* set to our TFM to enforce correct alignment: */
		skcipher_request_set_tfm(req, tfm);
	}

	err = skcipher_walk_virt(&w, req, false);
	if (err)
		return err;

	iv = (__be32 *)w.iv;
	counter[0] = be32_to_cpu(iv[3]);
	counter[1] = be32_to_cpu(iv[2]);
	counter[2] = be32_to_cpu(iv[1]);
	counter[3] = be32_to_cpu(iv[0]);

	while (w.nbytes) {
		unsigned int avail = w.nbytes;
		be128 *wsrc;
		be128 *wdst;

		wsrc = w.src.virt.addr;
		wdst = w.dst.virt.addr;

		do {
			be128_xor(wdst++, &t, wsrc++);

			/* T <- I*Key2, using the optimization
			 * discussed in the specification */
			be128_xor(&t, &t,
				  &ctx->mulinc[lrw_next_index(counter)]);
		} while ((avail -= bs) >= bs);

		if (second_pass && w.nbytes == w.total) {
			iv[0] = cpu_to_be32(counter[3]);
			iv[1] = cpu_to_be32(counter[2]);
			iv[2] = cpu_to_be32(counter[1]);
			iv[3] = cpu_to_be32(counter[0]);
		}

		err = skcipher_walk_done(&w, avail);
	}

	return err;
}

static int lrw_xor_tweak_pre(struct skcipher_request *req)
{
	return lrw_xor_tweak(req, false);
}

static int lrw_xor_tweak_post(struct skcipher_request *req)
{
	return lrw_xor_tweak(req, true);
}

static void lrw_crypt_done(struct crypto_async_request *areq, int err)
{
	struct skcipher_request *req = areq->data;

	if (!err) {
		struct lrw_request_ctx *rctx = skcipher_request_ctx(req);

		rctx->subreq.base.flags &= ~CRYPTO_TFM_REQ_MAY_SLEEP;
		err = lrw_xor_tweak_post(req);
	}

	skcipher_request_complete(req, err);
}

static void lrw_init_crypt(struct skcipher_request *req)
{
	const struct lrw_tfm_ctx *ctx =
		crypto_skcipher_ctx(crypto_skcipher_reqtfm(req));
	struct lrw_request_ctx *rctx = skcipher_request_ctx(req);
	struct skcipher_request *subreq = &rctx->subreq;

	skcipher_request_set_tfm(subreq, ctx->child);
	skcipher_request_set_callback(subreq, req->base.flags, lrw_crypt_done,
				      req);
	/* pass req->iv as IV (will be used by xor_tweak, ECB will ignore it) */
	skcipher_request_set_crypt(subreq, req->dst, req->dst,
				   req->cryptlen, req->iv);

	/* calculate first value of T */
	memcpy(&rctx->t, req->iv, sizeof(rctx->t));

	/* T <- I*Key2 */
	gf128mul_64k_bbe(&rctx->t, ctx->table);
}

static int lrw_encrypt(struct skcipher_request *req)
{
	struct lrw_request_ctx *rctx = skcipher_request_ctx(req);
	struct skcipher_request *subreq = &rctx->subreq;

	lrw_init_crypt(req);
	return lrw_xor_tweak_pre(req) ?:
		crypto_skcipher_encrypt(subreq) ?:
		lrw_xor_tweak_post(req);
}

static int lrw_decrypt(struct skcipher_request *req)
{
	struct lrw_request_ctx *rctx = skcipher_request_ctx(req);
	struct skcipher_request *subreq = &rctx->subreq;

	lrw_init_crypt(req);
	return lrw_xor_tweak_pre(req) ?:
		crypto_skcipher_decrypt(subreq) ?:
		lrw_xor_tweak_post(req);
}

static int lrw_init_tfm(struct crypto_skcipher *tfm)
{
	struct skcipher_instance *inst = skcipher_alg_instance(tfm);
	struct crypto_skcipher_spawn *spawn = skcipher_instance_ctx(inst);
	struct lrw_tfm_ctx *ctx = crypto_skcipher_ctx(tfm);
	struct crypto_skcipher *cipher;

	cipher = crypto_spawn_skcipher(spawn);
	if (IS_ERR(cipher))
		return PTR_ERR(cipher);

	ctx->child = cipher;

	crypto_skcipher_set_reqsize(tfm, crypto_skcipher_reqsize(cipher) +
					 sizeof(struct lrw_request_ctx));

	return 0;
}

static void lrw_exit_tfm(struct crypto_skcipher *tfm)
{
	struct lrw_tfm_ctx *ctx = crypto_skcipher_ctx(tfm);

	if (ctx->table)
		gf128mul_free_64k(ctx->table);
	crypto_free_skcipher(ctx->child);
}

static void lrw_free_instance(struct skcipher_instance *inst)
{
	crypto_drop_skcipher(skcipher_instance_ctx(inst));
	kfree(inst);
}

static int lrw_create(struct crypto_template *tmpl, struct rtattr **tb)
{
	struct crypto_skcipher_spawn *spawn;
	struct skcipher_instance *inst;
	struct skcipher_alg *alg;
	const char *cipher_name;
	char ecb_name[CRYPTO_MAX_ALG_NAME];
	u32 mask;
	int err;

	err = crypto_check_attr_type(tb, CRYPTO_ALG_TYPE_SKCIPHER, &mask);
	if (err)
		return err;

	cipher_name = crypto_attr_alg_name(tb[1]);
	if (IS_ERR(cipher_name))
		return PTR_ERR(cipher_name);

	inst = kzalloc(sizeof(*inst) + sizeof(*spawn), GFP_KERNEL);
	if (!inst)
		return -ENOMEM;

	spawn = skcipher_instance_ctx(inst);

	err = crypto_grab_skcipher(spawn, skcipher_crypto_instance(inst),
				   cipher_name, 0, mask);
	if (err == -ENOENT) {
		err = -ENAMETOOLONG;
		if (snprintf(ecb_name, CRYPTO_MAX_ALG_NAME, "ecb(%s)",
			     cipher_name) >= CRYPTO_MAX_ALG_NAME)
			goto err_free_inst;

		err = crypto_grab_skcipher(spawn,
					   skcipher_crypto_instance(inst),
					   ecb_name, 0, mask);
	}

	if (err)
		goto err_free_inst;

	alg = crypto_skcipher_spawn_alg(spawn);

	err = -EINVAL;
	if (alg->base.cra_blocksize != LRW_BLOCK_SIZE)
		goto err_free_inst;

	if (crypto_skcipher_alg_ivsize(alg))
		goto err_free_inst;

	err = crypto_inst_setname(skcipher_crypto_instance(inst), "lrw",
				  &alg->base);
	if (err)
		goto err_free_inst;

	err = -EINVAL;
	cipher_name = alg->base.cra_name;

	/* Alas we screwed up the naming so we have to mangle the
	 * cipher name.
	 */
	if (!strncmp(cipher_name, "ecb(", 4)) {
		unsigned len;

		len = strlcpy(ecb_name, cipher_name + 4, sizeof(ecb_name));
		if (len < 2 || len >= sizeof(ecb_name))
			goto err_free_inst;

		if (ecb_name[len - 1] != ')')
			goto err_free_inst;

		ecb_name[len - 1] = 0;

		if (snprintf(inst->alg.base.cra_name, CRYPTO_MAX_ALG_NAME,
			     "lrw(%s)", ecb_name) >= CRYPTO_MAX_ALG_NAME) {
			err = -ENAMETOOLONG;
			goto err_free_inst;
		}
	} else
		goto err_free_inst;

	inst->alg.base.cra_priority = alg->base.cra_priority;
	inst->alg.base.cra_blocksize = LRW_BLOCK_SIZE;
	inst->alg.base.cra_alignmask = alg->base.cra_alignmask |
				       (__alignof__(be128) - 1);

	inst->alg.ivsize = LRW_BLOCK_SIZE;
	inst->alg.min_keysize = crypto_skcipher_alg_min_keysize(alg) +
				LRW_BLOCK_SIZE;
	inst->alg.max_keysize = crypto_skcipher_alg_max_keysize(alg) +
				LRW_BLOCK_SIZE;

	inst->alg.base.cra_ctxsize = sizeof(struct lrw_tfm_ctx);

	inst->alg.init = lrw_init_tfm;
	inst->alg.exit = lrw_exit_tfm;

	inst->alg.setkey = lrw_setkey;
	inst->alg.encrypt = lrw_encrypt;
	inst->alg.decrypt = lrw_decrypt;

	inst->free = lrw_free_instance;

	err = skcipher_register_instance(tmpl, inst);
	if (err) {
err_free_inst:
		lrw_free_instance(inst);
	}
	return err;
}

static struct crypto_template lrw_tmpl = {
	.name = "lrw",
	.create = lrw_create,
	.module = THIS_MODULE,
};

static int __init lrw_module_init(void)
{
	return crypto_register_template(&lrw_tmpl);
}

static void __exit lrw_module_exit(void)
{
	crypto_unregister_template(&lrw_tmpl);
}

subsys_initcall(lrw_module_init);
module_exit(lrw_module_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("LRW block cipher mode");
MODULE_ALIAS_CRYPTO("lrw");
