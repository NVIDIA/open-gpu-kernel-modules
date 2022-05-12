// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Linux/arm64 port of the OpenSSL SHA512 implementation for AArch64
 *
 * Copyright (c) 2016 Linaro Ltd. <ard.biesheuvel@linaro.org>
 */

#include <crypto/internal/hash.h>
#include <linux/types.h>
#include <linux/string.h>
#include <crypto/sha2.h>
#include <crypto/sha512_base.h>
#include <asm/neon.h>

MODULE_DESCRIPTION("SHA-384/SHA-512 secure hash for arm64");
MODULE_AUTHOR("Andy Polyakov <appro@openssl.org>");
MODULE_AUTHOR("Ard Biesheuvel <ard.biesheuvel@linaro.org>");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS_CRYPTO("sha384");
MODULE_ALIAS_CRYPTO("sha512");

asmlinkage void sha512_block_data_order(u64 *digest, const void *data,
					unsigned int num_blks);
EXPORT_SYMBOL(sha512_block_data_order);

static void __sha512_block_data_order(struct sha512_state *sst, u8 const *src,
				      int blocks)
{
	sha512_block_data_order(sst->state, src, blocks);
}

static int sha512_update(struct shash_desc *desc, const u8 *data,
			 unsigned int len)
{
	return sha512_base_do_update(desc, data, len,
				     __sha512_block_data_order);
}

static int sha512_finup(struct shash_desc *desc, const u8 *data,
			unsigned int len, u8 *out)
{
	if (len)
		sha512_base_do_update(desc, data, len,
				      __sha512_block_data_order);
	sha512_base_do_finalize(desc, __sha512_block_data_order);

	return sha512_base_finish(desc, out);
}

static int sha512_final(struct shash_desc *desc, u8 *out)
{
	return sha512_finup(desc, NULL, 0, out);
}

static struct shash_alg algs[] = { {
	.digestsize		= SHA512_DIGEST_SIZE,
	.init			= sha512_base_init,
	.update			= sha512_update,
	.final			= sha512_final,
	.finup			= sha512_finup,
	.descsize		= sizeof(struct sha512_state),
	.base.cra_name		= "sha512",
	.base.cra_driver_name	= "sha512-arm64",
	.base.cra_priority	= 150,
	.base.cra_blocksize	= SHA512_BLOCK_SIZE,
	.base.cra_module	= THIS_MODULE,
}, {
	.digestsize		= SHA384_DIGEST_SIZE,
	.init			= sha384_base_init,
	.update			= sha512_update,
	.final			= sha512_final,
	.finup			= sha512_finup,
	.descsize		= sizeof(struct sha512_state),
	.base.cra_name		= "sha384",
	.base.cra_driver_name	= "sha384-arm64",
	.base.cra_priority	= 150,
	.base.cra_blocksize	= SHA384_BLOCK_SIZE,
	.base.cra_module	= THIS_MODULE,
} };

static int __init sha512_mod_init(void)
{
	return crypto_register_shashes(algs, ARRAY_SIZE(algs));
}

static void __exit sha512_mod_fini(void)
{
	crypto_unregister_shashes(algs, ARRAY_SIZE(algs));
}

module_init(sha512_mod_init);
module_exit(sha512_mod_fini);
