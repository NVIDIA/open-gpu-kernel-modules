/* SPDX-License-Identifier: GPL-2.0 */
#ifndef ASM_X86_CAMELLIA_H
#define ASM_X86_CAMELLIA_H

#include <crypto/b128ops.h>
#include <linux/crypto.h>
#include <linux/kernel.h>

#define CAMELLIA_MIN_KEY_SIZE	16
#define CAMELLIA_MAX_KEY_SIZE	32
#define CAMELLIA_BLOCK_SIZE	16
#define CAMELLIA_TABLE_BYTE_LEN	272
#define CAMELLIA_PARALLEL_BLOCKS 2

struct crypto_skcipher;

struct camellia_ctx {
	u64 key_table[CAMELLIA_TABLE_BYTE_LEN / sizeof(u64)];
	u32 key_length;
};

extern int __camellia_setkey(struct camellia_ctx *cctx,
			     const unsigned char *key,
			     unsigned int key_len);

/* regular block cipher functions */
asmlinkage void __camellia_enc_blk(const void *ctx, u8 *dst, const u8 *src,
				   bool xor);
asmlinkage void camellia_dec_blk(const void *ctx, u8 *dst, const u8 *src);

/* 2-way parallel cipher functions */
asmlinkage void __camellia_enc_blk_2way(const void *ctx, u8 *dst, const u8 *src,
					bool xor);
asmlinkage void camellia_dec_blk_2way(const void *ctx, u8 *dst, const u8 *src);

/* 16-way parallel cipher functions (avx/aes-ni) */
asmlinkage void camellia_ecb_enc_16way(const void *ctx, u8 *dst, const u8 *src);
asmlinkage void camellia_ecb_dec_16way(const void *ctx, u8 *dst, const u8 *src);

asmlinkage void camellia_cbc_dec_16way(const void *ctx, u8 *dst, const u8 *src);

static inline void camellia_enc_blk(const void *ctx, u8 *dst, const u8 *src)
{
	__camellia_enc_blk(ctx, dst, src, false);
}

static inline void camellia_enc_blk_xor(const void *ctx, u8 *dst, const u8 *src)
{
	__camellia_enc_blk(ctx, dst, src, true);
}

static inline void camellia_enc_blk_2way(const void *ctx, u8 *dst,
					 const u8 *src)
{
	__camellia_enc_blk_2way(ctx, dst, src, false);
}

static inline void camellia_enc_blk_xor_2way(const void *ctx, u8 *dst,
					     const u8 *src)
{
	__camellia_enc_blk_2way(ctx, dst, src, true);
}

/* glue helpers */
extern void camellia_decrypt_cbc_2way(const void *ctx, u8 *dst, const u8 *src);

#endif /* ASM_X86_CAMELLIA_H */
