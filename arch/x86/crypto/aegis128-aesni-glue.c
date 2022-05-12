// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * The AEGIS-128 Authenticated-Encryption Algorithm
 *   Glue for AES-NI + SSE2 implementation
 *
 * Copyright (c) 2017-2018 Ondrej Mosnacek <omosnacek@gmail.com>
 * Copyright (C) 2017-2018 Red Hat, Inc. All rights reserved.
 */

#include <crypto/internal/aead.h>
#include <crypto/internal/simd.h>
#include <crypto/internal/skcipher.h>
#include <crypto/scatterwalk.h>
#include <linux/module.h>
#include <asm/fpu/api.h>
#include <asm/cpu_device_id.h>

#define AEGIS128_BLOCK_ALIGN 16
#define AEGIS128_BLOCK_SIZE 16
#define AEGIS128_NONCE_SIZE 16
#define AEGIS128_STATE_BLOCKS 5
#define AEGIS128_KEY_SIZE 16
#define AEGIS128_MIN_AUTH_SIZE 8
#define AEGIS128_MAX_AUTH_SIZE 16

asmlinkage void crypto_aegis128_aesni_init(void *state, void *key, void *iv);

asmlinkage void crypto_aegis128_aesni_ad(
		void *state, unsigned int length, const void *data);

asmlinkage void crypto_aegis128_aesni_enc(
		void *state, unsigned int length, const void *src, void *dst);

asmlinkage void crypto_aegis128_aesni_dec(
		void *state, unsigned int length, const void *src, void *dst);

asmlinkage void crypto_aegis128_aesni_enc_tail(
		void *state, unsigned int length, const void *src, void *dst);

asmlinkage void crypto_aegis128_aesni_dec_tail(
		void *state, unsigned int length, const void *src, void *dst);

asmlinkage void crypto_aegis128_aesni_final(
		void *state, void *tag_xor, unsigned int cryptlen,
		unsigned int assoclen);

struct aegis_block {
	u8 bytes[AEGIS128_BLOCK_SIZE] __aligned(AEGIS128_BLOCK_ALIGN);
};

struct aegis_state {
	struct aegis_block blocks[AEGIS128_STATE_BLOCKS];
};

struct aegis_ctx {
	struct aegis_block key;
};

struct aegis_crypt_ops {
	int (*skcipher_walk_init)(struct skcipher_walk *walk,
				  struct aead_request *req, bool atomic);

	void (*crypt_blocks)(void *state, unsigned int length, const void *src,
			     void *dst);
	void (*crypt_tail)(void *state, unsigned int length, const void *src,
			   void *dst);
};

static void crypto_aegis128_aesni_process_ad(
		struct aegis_state *state, struct scatterlist *sg_src,
		unsigned int assoclen)
{
	struct scatter_walk walk;
	struct aegis_block buf;
	unsigned int pos = 0;

	scatterwalk_start(&walk, sg_src);
	while (assoclen != 0) {
		unsigned int size = scatterwalk_clamp(&walk, assoclen);
		unsigned int left = size;
		void *mapped = scatterwalk_map(&walk);
		const u8 *src = (const u8 *)mapped;

		if (pos + size >= AEGIS128_BLOCK_SIZE) {
			if (pos > 0) {
				unsigned int fill = AEGIS128_BLOCK_SIZE - pos;
				memcpy(buf.bytes + pos, src, fill);
				crypto_aegis128_aesni_ad(state,
							 AEGIS128_BLOCK_SIZE,
							 buf.bytes);
				pos = 0;
				left -= fill;
				src += fill;
			}

			crypto_aegis128_aesni_ad(state, left, src);

			src += left & ~(AEGIS128_BLOCK_SIZE - 1);
			left &= AEGIS128_BLOCK_SIZE - 1;
		}

		memcpy(buf.bytes + pos, src, left);
		pos += left;
		assoclen -= size;

		scatterwalk_unmap(mapped);
		scatterwalk_advance(&walk, size);
		scatterwalk_done(&walk, 0, assoclen);
	}

	if (pos > 0) {
		memset(buf.bytes + pos, 0, AEGIS128_BLOCK_SIZE - pos);
		crypto_aegis128_aesni_ad(state, AEGIS128_BLOCK_SIZE, buf.bytes);
	}
}

static void crypto_aegis128_aesni_process_crypt(
		struct aegis_state *state, struct skcipher_walk *walk,
		const struct aegis_crypt_ops *ops)
{
	while (walk->nbytes >= AEGIS128_BLOCK_SIZE) {
		ops->crypt_blocks(state,
				  round_down(walk->nbytes, AEGIS128_BLOCK_SIZE),
				  walk->src.virt.addr, walk->dst.virt.addr);
		skcipher_walk_done(walk, walk->nbytes % AEGIS128_BLOCK_SIZE);
	}

	if (walk->nbytes) {
		ops->crypt_tail(state, walk->nbytes, walk->src.virt.addr,
				walk->dst.virt.addr);
		skcipher_walk_done(walk, 0);
	}
}

static struct aegis_ctx *crypto_aegis128_aesni_ctx(struct crypto_aead *aead)
{
	u8 *ctx = crypto_aead_ctx(aead);
	ctx = PTR_ALIGN(ctx, __alignof__(struct aegis_ctx));
	return (void *)ctx;
}

static int crypto_aegis128_aesni_setkey(struct crypto_aead *aead, const u8 *key,
					unsigned int keylen)
{
	struct aegis_ctx *ctx = crypto_aegis128_aesni_ctx(aead);

	if (keylen != AEGIS128_KEY_SIZE)
		return -EINVAL;

	memcpy(ctx->key.bytes, key, AEGIS128_KEY_SIZE);

	return 0;
}

static int crypto_aegis128_aesni_setauthsize(struct crypto_aead *tfm,
						unsigned int authsize)
{
	if (authsize > AEGIS128_MAX_AUTH_SIZE)
		return -EINVAL;
	if (authsize < AEGIS128_MIN_AUTH_SIZE)
		return -EINVAL;
	return 0;
}

static void crypto_aegis128_aesni_crypt(struct aead_request *req,
					struct aegis_block *tag_xor,
					unsigned int cryptlen,
					const struct aegis_crypt_ops *ops)
{
	struct crypto_aead *tfm = crypto_aead_reqtfm(req);
	struct aegis_ctx *ctx = crypto_aegis128_aesni_ctx(tfm);
	struct skcipher_walk walk;
	struct aegis_state state;

	ops->skcipher_walk_init(&walk, req, true);

	kernel_fpu_begin();

	crypto_aegis128_aesni_init(&state, ctx->key.bytes, req->iv);
	crypto_aegis128_aesni_process_ad(&state, req->src, req->assoclen);
	crypto_aegis128_aesni_process_crypt(&state, &walk, ops);
	crypto_aegis128_aesni_final(&state, tag_xor, req->assoclen, cryptlen);

	kernel_fpu_end();
}

static int crypto_aegis128_aesni_encrypt(struct aead_request *req)
{
	static const struct aegis_crypt_ops OPS = {
		.skcipher_walk_init = skcipher_walk_aead_encrypt,
		.crypt_blocks = crypto_aegis128_aesni_enc,
		.crypt_tail = crypto_aegis128_aesni_enc_tail,
	};

	struct crypto_aead *tfm = crypto_aead_reqtfm(req);
	struct aegis_block tag = {};
	unsigned int authsize = crypto_aead_authsize(tfm);
	unsigned int cryptlen = req->cryptlen;

	crypto_aegis128_aesni_crypt(req, &tag, cryptlen, &OPS);

	scatterwalk_map_and_copy(tag.bytes, req->dst,
				 req->assoclen + cryptlen, authsize, 1);
	return 0;
}

static int crypto_aegis128_aesni_decrypt(struct aead_request *req)
{
	static const struct aegis_block zeros = {};

	static const struct aegis_crypt_ops OPS = {
		.skcipher_walk_init = skcipher_walk_aead_decrypt,
		.crypt_blocks = crypto_aegis128_aesni_dec,
		.crypt_tail = crypto_aegis128_aesni_dec_tail,
	};

	struct crypto_aead *tfm = crypto_aead_reqtfm(req);
	struct aegis_block tag;
	unsigned int authsize = crypto_aead_authsize(tfm);
	unsigned int cryptlen = req->cryptlen - authsize;

	scatterwalk_map_and_copy(tag.bytes, req->src,
				 req->assoclen + cryptlen, authsize, 0);

	crypto_aegis128_aesni_crypt(req, &tag, cryptlen, &OPS);

	return crypto_memneq(tag.bytes, zeros.bytes, authsize) ? -EBADMSG : 0;
}

static int crypto_aegis128_aesni_init_tfm(struct crypto_aead *aead)
{
	return 0;
}

static void crypto_aegis128_aesni_exit_tfm(struct crypto_aead *aead)
{
}

static struct aead_alg crypto_aegis128_aesni_alg = {
	.setkey = crypto_aegis128_aesni_setkey,
	.setauthsize = crypto_aegis128_aesni_setauthsize,
	.encrypt = crypto_aegis128_aesni_encrypt,
	.decrypt = crypto_aegis128_aesni_decrypt,
	.init = crypto_aegis128_aesni_init_tfm,
	.exit = crypto_aegis128_aesni_exit_tfm,

	.ivsize = AEGIS128_NONCE_SIZE,
	.maxauthsize = AEGIS128_MAX_AUTH_SIZE,
	.chunksize = AEGIS128_BLOCK_SIZE,

	.base = {
		.cra_flags = CRYPTO_ALG_INTERNAL,
		.cra_blocksize = 1,
		.cra_ctxsize = sizeof(struct aegis_ctx) +
			       __alignof__(struct aegis_ctx),
		.cra_alignmask = 0,
		.cra_priority = 400,

		.cra_name = "__aegis128",
		.cra_driver_name = "__aegis128-aesni",

		.cra_module = THIS_MODULE,
	}
};

static struct simd_aead_alg *simd_alg;

static int __init crypto_aegis128_aesni_module_init(void)
{
	if (!boot_cpu_has(X86_FEATURE_XMM2) ||
	    !boot_cpu_has(X86_FEATURE_AES) ||
	    !cpu_has_xfeatures(XFEATURE_MASK_SSE, NULL))
		return -ENODEV;

	return simd_register_aeads_compat(&crypto_aegis128_aesni_alg, 1,
					  &simd_alg);
}

static void __exit crypto_aegis128_aesni_module_exit(void)
{
	simd_unregister_aeads(&crypto_aegis128_aesni_alg, 1, &simd_alg);
}

module_init(crypto_aegis128_aesni_module_init);
module_exit(crypto_aegis128_aesni_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ondrej Mosnacek <omosnacek@gmail.com>");
MODULE_DESCRIPTION("AEGIS-128 AEAD algorithm -- AESNI+SSE2 implementation");
MODULE_ALIAS_CRYPTO("aegis128");
MODULE_ALIAS_CRYPTO("aegis128-aesni");
