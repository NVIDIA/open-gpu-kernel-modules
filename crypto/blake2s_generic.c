// SPDX-License-Identifier: GPL-2.0 OR MIT
/*
 * shash interface to the generic implementation of BLAKE2s
 *
 * Copyright (C) 2015-2019 Jason A. Donenfeld <Jason@zx2c4.com>. All Rights Reserved.
 */

#include <crypto/internal/blake2s.h>
#include <crypto/internal/hash.h>

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/module.h>

static int crypto_blake2s_update_generic(struct shash_desc *desc,
					 const u8 *in, unsigned int inlen)
{
	return crypto_blake2s_update(desc, in, inlen, blake2s_compress_generic);
}

static int crypto_blake2s_final_generic(struct shash_desc *desc, u8 *out)
{
	return crypto_blake2s_final(desc, out, blake2s_compress_generic);
}

#define BLAKE2S_ALG(name, driver_name, digest_size)			\
	{								\
		.base.cra_name		= name,				\
		.base.cra_driver_name	= driver_name,			\
		.base.cra_priority	= 100,				\
		.base.cra_flags		= CRYPTO_ALG_OPTIONAL_KEY,	\
		.base.cra_blocksize	= BLAKE2S_BLOCK_SIZE,		\
		.base.cra_ctxsize	= sizeof(struct blake2s_tfm_ctx), \
		.base.cra_module	= THIS_MODULE,			\
		.digestsize		= digest_size,			\
		.setkey			= crypto_blake2s_setkey,	\
		.init			= crypto_blake2s_init,		\
		.update			= crypto_blake2s_update_generic, \
		.final			= crypto_blake2s_final_generic,	\
		.descsize		= sizeof(struct blake2s_state),	\
	}

static struct shash_alg blake2s_algs[] = {
	BLAKE2S_ALG("blake2s-128", "blake2s-128-generic",
		    BLAKE2S_128_HASH_SIZE),
	BLAKE2S_ALG("blake2s-160", "blake2s-160-generic",
		    BLAKE2S_160_HASH_SIZE),
	BLAKE2S_ALG("blake2s-224", "blake2s-224-generic",
		    BLAKE2S_224_HASH_SIZE),
	BLAKE2S_ALG("blake2s-256", "blake2s-256-generic",
		    BLAKE2S_256_HASH_SIZE),
};

static int __init blake2s_mod_init(void)
{
	return crypto_register_shashes(blake2s_algs, ARRAY_SIZE(blake2s_algs));
}

static void __exit blake2s_mod_exit(void)
{
	crypto_unregister_shashes(blake2s_algs, ARRAY_SIZE(blake2s_algs));
}

subsys_initcall(blake2s_mod_init);
module_exit(blake2s_mod_exit);

MODULE_ALIAS_CRYPTO("blake2s-128");
MODULE_ALIAS_CRYPTO("blake2s-128-generic");
MODULE_ALIAS_CRYPTO("blake2s-160");
MODULE_ALIAS_CRYPTO("blake2s-160-generic");
MODULE_ALIAS_CRYPTO("blake2s-224");
MODULE_ALIAS_CRYPTO("blake2s-224-generic");
MODULE_ALIAS_CRYPTO("blake2s-256");
MODULE_ALIAS_CRYPTO("blake2s-256-generic");
MODULE_LICENSE("GPL v2");
