/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Symmetric key ciphers.
 * 
 * Copyright (c) 2007 Herbert Xu <herbert@gondor.apana.org.au>
 */

#ifndef _CRYPTO_INTERNAL_SKCIPHER_H
#define _CRYPTO_INTERNAL_SKCIPHER_H

#include <crypto/algapi.h>
#include <crypto/internal/cipher.h>
#include <crypto/skcipher.h>
#include <linux/list.h>
#include <linux/types.h>

struct aead_request;
struct rtattr;

struct skcipher_instance {
	void (*free)(struct skcipher_instance *inst);
	union {
		struct {
			char head[offsetof(struct skcipher_alg, base)];
			struct crypto_instance base;
		} s;
		struct skcipher_alg alg;
	};
};

struct crypto_skcipher_spawn {
	struct crypto_spawn base;
};

struct skcipher_walk {
	union {
		struct {
			struct page *page;
			unsigned long offset;
		} phys;

		struct {
			u8 *page;
			void *addr;
		} virt;
	} src, dst;

	struct scatter_walk in;
	unsigned int nbytes;

	struct scatter_walk out;
	unsigned int total;

	struct list_head buffers;

	u8 *page;
	u8 *buffer;
	u8 *oiv;
	void *iv;

	unsigned int ivsize;

	int flags;
	unsigned int blocksize;
	unsigned int stride;
	unsigned int alignmask;
};

static inline struct crypto_instance *skcipher_crypto_instance(
	struct skcipher_instance *inst)
{
	return &inst->s.base;
}

static inline struct skcipher_instance *skcipher_alg_instance(
	struct crypto_skcipher *skcipher)
{
	return container_of(crypto_skcipher_alg(skcipher),
			    struct skcipher_instance, alg);
}

static inline void *skcipher_instance_ctx(struct skcipher_instance *inst)
{
	return crypto_instance_ctx(skcipher_crypto_instance(inst));
}

static inline void skcipher_request_complete(struct skcipher_request *req, int err)
{
	req->base.complete(&req->base, err);
}

int crypto_grab_skcipher(struct crypto_skcipher_spawn *spawn,
			 struct crypto_instance *inst,
			 const char *name, u32 type, u32 mask);

static inline void crypto_drop_skcipher(struct crypto_skcipher_spawn *spawn)
{
	crypto_drop_spawn(&spawn->base);
}

static inline struct skcipher_alg *crypto_skcipher_spawn_alg(
	struct crypto_skcipher_spawn *spawn)
{
	return container_of(spawn->base.alg, struct skcipher_alg, base);
}

static inline struct skcipher_alg *crypto_spawn_skcipher_alg(
	struct crypto_skcipher_spawn *spawn)
{
	return crypto_skcipher_spawn_alg(spawn);
}

static inline struct crypto_skcipher *crypto_spawn_skcipher(
	struct crypto_skcipher_spawn *spawn)
{
	return crypto_spawn_tfm2(&spawn->base);
}

static inline void crypto_skcipher_set_reqsize(
	struct crypto_skcipher *skcipher, unsigned int reqsize)
{
	skcipher->reqsize = reqsize;
}

int crypto_register_skcipher(struct skcipher_alg *alg);
void crypto_unregister_skcipher(struct skcipher_alg *alg);
int crypto_register_skciphers(struct skcipher_alg *algs, int count);
void crypto_unregister_skciphers(struct skcipher_alg *algs, int count);
int skcipher_register_instance(struct crypto_template *tmpl,
			       struct skcipher_instance *inst);

int skcipher_walk_done(struct skcipher_walk *walk, int err);
int skcipher_walk_virt(struct skcipher_walk *walk,
		       struct skcipher_request *req,
		       bool atomic);
int skcipher_walk_async(struct skcipher_walk *walk,
			struct skcipher_request *req);
int skcipher_walk_aead_encrypt(struct skcipher_walk *walk,
			       struct aead_request *req, bool atomic);
int skcipher_walk_aead_decrypt(struct skcipher_walk *walk,
			       struct aead_request *req, bool atomic);
void skcipher_walk_complete(struct skcipher_walk *walk, int err);

static inline void skcipher_walk_abort(struct skcipher_walk *walk)
{
	skcipher_walk_done(walk, -ECANCELED);
}

static inline void *crypto_skcipher_ctx(struct crypto_skcipher *tfm)
{
	return crypto_tfm_ctx(&tfm->base);
}

static inline void *skcipher_request_ctx(struct skcipher_request *req)
{
	return req->__ctx;
}

static inline u32 skcipher_request_flags(struct skcipher_request *req)
{
	return req->base.flags;
}

static inline unsigned int crypto_skcipher_alg_min_keysize(
	struct skcipher_alg *alg)
{
	return alg->min_keysize;
}

static inline unsigned int crypto_skcipher_alg_max_keysize(
	struct skcipher_alg *alg)
{
	return alg->max_keysize;
}

static inline unsigned int crypto_skcipher_alg_walksize(
	struct skcipher_alg *alg)
{
	return alg->walksize;
}

/**
 * crypto_skcipher_walksize() - obtain walk size
 * @tfm: cipher handle
 *
 * In some cases, algorithms can only perform optimally when operating on
 * multiple blocks in parallel. This is reflected by the walksize, which
 * must be a multiple of the chunksize (or equal if the concern does not
 * apply)
 *
 * Return: walk size in bytes
 */
static inline unsigned int crypto_skcipher_walksize(
	struct crypto_skcipher *tfm)
{
	return crypto_skcipher_alg_walksize(crypto_skcipher_alg(tfm));
}

/* Helpers for simple block cipher modes of operation */
struct skcipher_ctx_simple {
	struct crypto_cipher *cipher;	/* underlying block cipher */
};
static inline struct crypto_cipher *
skcipher_cipher_simple(struct crypto_skcipher *tfm)
{
	struct skcipher_ctx_simple *ctx = crypto_skcipher_ctx(tfm);

	return ctx->cipher;
}

struct skcipher_instance *skcipher_alloc_instance_simple(
	struct crypto_template *tmpl, struct rtattr **tb);

static inline struct crypto_alg *skcipher_ialg_simple(
	struct skcipher_instance *inst)
{
	struct crypto_cipher_spawn *spawn = skcipher_instance_ctx(inst);

	return crypto_spawn_cipher_alg(spawn);
}

#endif	/* _CRYPTO_INTERNAL_SKCIPHER_H */

