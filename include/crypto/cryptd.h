/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Software async crypto daemon
 *
 * Added AEAD support to cryptd.
 *    Authors: Tadeusz Struk (tadeusz.struk@intel.com)
 *             Adrian Hoban <adrian.hoban@intel.com>
 *             Gabriele Paoloni <gabriele.paoloni@intel.com>
 *             Aidan O'Mahony (aidan.o.mahony@intel.com)
 *    Copyright (c) 2010, Intel Corporation.
 */

#ifndef _CRYPTO_CRYPT_H
#define _CRYPTO_CRYPT_H

#include <linux/kernel.h>
#include <crypto/aead.h>
#include <crypto/hash.h>
#include <crypto/skcipher.h>

struct cryptd_skcipher {
	struct crypto_skcipher base;
};

/* alg_name should be algorithm to be cryptd-ed */
struct cryptd_skcipher *cryptd_alloc_skcipher(const char *alg_name,
					      u32 type, u32 mask);
struct crypto_skcipher *cryptd_skcipher_child(struct cryptd_skcipher *tfm);
/* Must be called without moving CPUs. */
bool cryptd_skcipher_queued(struct cryptd_skcipher *tfm);
void cryptd_free_skcipher(struct cryptd_skcipher *tfm);

struct cryptd_ahash {
	struct crypto_ahash base;
};

static inline struct cryptd_ahash *__cryptd_ahash_cast(
	struct crypto_ahash *tfm)
{
	return (struct cryptd_ahash *)tfm;
}

/* alg_name should be algorithm to be cryptd-ed */
struct cryptd_ahash *cryptd_alloc_ahash(const char *alg_name,
					u32 type, u32 mask);
struct crypto_shash *cryptd_ahash_child(struct cryptd_ahash *tfm);
struct shash_desc *cryptd_shash_desc(struct ahash_request *req);
/* Must be called without moving CPUs. */
bool cryptd_ahash_queued(struct cryptd_ahash *tfm);
void cryptd_free_ahash(struct cryptd_ahash *tfm);

struct cryptd_aead {
	struct crypto_aead base;
};

static inline struct cryptd_aead *__cryptd_aead_cast(
	struct crypto_aead *tfm)
{
	return (struct cryptd_aead *)tfm;
}

struct cryptd_aead *cryptd_alloc_aead(const char *alg_name,
					  u32 type, u32 mask);

struct crypto_aead *cryptd_aead_child(struct cryptd_aead *tfm);
/* Must be called without moving CPUs. */
bool cryptd_aead_queued(struct cryptd_aead *tfm);

void cryptd_free_aead(struct cryptd_aead *tfm);

#endif
