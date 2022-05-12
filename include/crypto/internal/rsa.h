/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * RSA internal helpers
 *
 * Copyright (c) 2015, Intel Corporation
 * Authors: Tadeusz Struk <tadeusz.struk@intel.com>
 */
#ifndef _RSA_HELPER_
#define _RSA_HELPER_
#include <linux/types.h>

/**
 * rsa_key - RSA key structure
 * @n           : RSA modulus raw byte stream
 * @e           : RSA public exponent raw byte stream
 * @d           : RSA private exponent raw byte stream
 * @p           : RSA prime factor p of n raw byte stream
 * @q           : RSA prime factor q of n raw byte stream
 * @dp          : RSA exponent d mod (p - 1) raw byte stream
 * @dq          : RSA exponent d mod (q - 1) raw byte stream
 * @qinv        : RSA CRT coefficient q^(-1) mod p raw byte stream
 * @n_sz        : length in bytes of RSA modulus n
 * @e_sz        : length in bytes of RSA public exponent
 * @d_sz        : length in bytes of RSA private exponent
 * @p_sz        : length in bytes of p field
 * @q_sz        : length in bytes of q field
 * @dp_sz       : length in bytes of dp field
 * @dq_sz       : length in bytes of dq field
 * @qinv_sz     : length in bytes of qinv field
 */
struct rsa_key {
	const u8 *n;
	const u8 *e;
	const u8 *d;
	const u8 *p;
	const u8 *q;
	const u8 *dp;
	const u8 *dq;
	const u8 *qinv;
	size_t n_sz;
	size_t e_sz;
	size_t d_sz;
	size_t p_sz;
	size_t q_sz;
	size_t dp_sz;
	size_t dq_sz;
	size_t qinv_sz;
};

int rsa_parse_pub_key(struct rsa_key *rsa_key, const void *key,
		      unsigned int key_len);

int rsa_parse_priv_key(struct rsa_key *rsa_key, const void *key,
		       unsigned int key_len);

extern struct crypto_template rsa_pkcs1pad_tmpl;
#endif
