// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Glue code for the SHA256 Secure Hash Algorithm assembly implementation
 * using NEON instructions.
 *
 * Copyright © 2015 Google Inc.
 *
 * This file is based on sha512_neon_glue.c:
 *   Copyright © 2014 Jussi Kivilinna <jussi.kivilinna@iki.fi>
 */

#include <crypto/internal/hash.h>
#include <crypto/internal/simd.h>
#include <linux/types.h>
#include <linux/string.h>
#include <crypto/sha2.h>
#include <crypto/sha256_base.h>
#include <asm/byteorder.h>
#include <asm/simd.h>
#include <asm/neon.h>

#include "sha256_glue.h"

asmlinkage void sha256_block_data_order_neon(u32 *digest, const void *data,
					     unsigned int num_blks);

static int crypto_sha256_neon_update(struct shash_desc *desc, const u8 *data,
				     unsigned int len)
{
	struct sha256_state *sctx = shash_desc_ctx(desc);

	if (!crypto_simd_usable() ||
	    (sctx->count % SHA256_BLOCK_SIZE) + len < SHA256_BLOCK_SIZE)
		return crypto_sha256_arm_update(desc, data, len);

	kernel_neon_begin();
	sha256_base_do_update(desc, data, len,
			(sha256_block_fn *)sha256_block_data_order_neon);
	kernel_neon_end();

	return 0;
}

static int crypto_sha256_neon_finup(struct shash_desc *desc, const u8 *data,
				    unsigned int len, u8 *out)
{
	if (!crypto_simd_usable())
		return crypto_sha256_arm_finup(desc, data, len, out);

	kernel_neon_begin();
	if (len)
		sha256_base_do_update(desc, data, len,
			(sha256_block_fn *)sha256_block_data_order_neon);
	sha256_base_do_finalize(desc,
			(sha256_block_fn *)sha256_block_data_order_neon);
	kernel_neon_end();

	return sha256_base_finish(desc, out);
}

static int crypto_sha256_neon_final(struct shash_desc *desc, u8 *out)
{
	return crypto_sha256_neon_finup(desc, NULL, 0, out);
}

struct shash_alg sha256_neon_algs[] = { {
	.digestsize	=	SHA256_DIGEST_SIZE,
	.init		=	sha256_base_init,
	.update		=	crypto_sha256_neon_update,
	.final		=	crypto_sha256_neon_final,
	.finup		=	crypto_sha256_neon_finup,
	.descsize	=	sizeof(struct sha256_state),
	.base		=	{
		.cra_name	=	"sha256",
		.cra_driver_name =	"sha256-neon",
		.cra_priority	=	250,
		.cra_blocksize	=	SHA256_BLOCK_SIZE,
		.cra_module	=	THIS_MODULE,
	}
}, {
	.digestsize	=	SHA224_DIGEST_SIZE,
	.init		=	sha224_base_init,
	.update		=	crypto_sha256_neon_update,
	.final		=	crypto_sha256_neon_final,
	.finup		=	crypto_sha256_neon_finup,
	.descsize	=	sizeof(struct sha256_state),
	.base		=	{
		.cra_name	=	"sha224",
		.cra_driver_name =	"sha224-neon",
		.cra_priority	=	250,
		.cra_blocksize	=	SHA224_BLOCK_SIZE,
		.cra_module	=	THIS_MODULE,
	}
} };
