/* SPDX-License-Identifier: ((GPL-2.0 WITH Linux-syscall-note) OR Linux-OpenIB) */
/*
 * Copyright (c) 2016-2017, Mellanox Technologies. All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _UAPI_LINUX_TLS_H
#define _UAPI_LINUX_TLS_H

#include <linux/types.h>

/* TLS socket options */
#define TLS_TX			1	/* Set transmit parameters */
#define TLS_RX			2	/* Set receive parameters */

/* Supported versions */
#define TLS_VERSION_MINOR(ver)	((ver) & 0xFF)
#define TLS_VERSION_MAJOR(ver)	(((ver) >> 8) & 0xFF)

#define TLS_VERSION_NUMBER(id)	((((id##_VERSION_MAJOR) & 0xFF) << 8) |	\
				 ((id##_VERSION_MINOR) & 0xFF))

#define TLS_1_2_VERSION_MAJOR	0x3
#define TLS_1_2_VERSION_MINOR	0x3
#define TLS_1_2_VERSION		TLS_VERSION_NUMBER(TLS_1_2)

#define TLS_1_3_VERSION_MAJOR	0x3
#define TLS_1_3_VERSION_MINOR	0x4
#define TLS_1_3_VERSION		TLS_VERSION_NUMBER(TLS_1_3)

/* Supported ciphers */
#define TLS_CIPHER_AES_GCM_128				51
#define TLS_CIPHER_AES_GCM_128_IV_SIZE			8
#define TLS_CIPHER_AES_GCM_128_KEY_SIZE		16
#define TLS_CIPHER_AES_GCM_128_SALT_SIZE		4
#define TLS_CIPHER_AES_GCM_128_TAG_SIZE		16
#define TLS_CIPHER_AES_GCM_128_REC_SEQ_SIZE		8

#define TLS_CIPHER_AES_GCM_256				52
#define TLS_CIPHER_AES_GCM_256_IV_SIZE			8
#define TLS_CIPHER_AES_GCM_256_KEY_SIZE		32
#define TLS_CIPHER_AES_GCM_256_SALT_SIZE		4
#define TLS_CIPHER_AES_GCM_256_TAG_SIZE		16
#define TLS_CIPHER_AES_GCM_256_REC_SEQ_SIZE		8

#define TLS_CIPHER_AES_CCM_128				53
#define TLS_CIPHER_AES_CCM_128_IV_SIZE			8
#define TLS_CIPHER_AES_CCM_128_KEY_SIZE		16
#define TLS_CIPHER_AES_CCM_128_SALT_SIZE		4
#define TLS_CIPHER_AES_CCM_128_TAG_SIZE		16
#define TLS_CIPHER_AES_CCM_128_REC_SEQ_SIZE		8

#define TLS_CIPHER_CHACHA20_POLY1305			54
#define TLS_CIPHER_CHACHA20_POLY1305_IV_SIZE		12
#define TLS_CIPHER_CHACHA20_POLY1305_KEY_SIZE	32
#define TLS_CIPHER_CHACHA20_POLY1305_SALT_SIZE		0
#define TLS_CIPHER_CHACHA20_POLY1305_TAG_SIZE	16
#define TLS_CIPHER_CHACHA20_POLY1305_REC_SEQ_SIZE	8

#define TLS_SET_RECORD_TYPE	1
#define TLS_GET_RECORD_TYPE	2

struct tls_crypto_info {
	__u16 version;
	__u16 cipher_type;
};

struct tls12_crypto_info_aes_gcm_128 {
	struct tls_crypto_info info;
	unsigned char iv[TLS_CIPHER_AES_GCM_128_IV_SIZE];
	unsigned char key[TLS_CIPHER_AES_GCM_128_KEY_SIZE];
	unsigned char salt[TLS_CIPHER_AES_GCM_128_SALT_SIZE];
	unsigned char rec_seq[TLS_CIPHER_AES_GCM_128_REC_SEQ_SIZE];
};

struct tls12_crypto_info_aes_gcm_256 {
	struct tls_crypto_info info;
	unsigned char iv[TLS_CIPHER_AES_GCM_256_IV_SIZE];
	unsigned char key[TLS_CIPHER_AES_GCM_256_KEY_SIZE];
	unsigned char salt[TLS_CIPHER_AES_GCM_256_SALT_SIZE];
	unsigned char rec_seq[TLS_CIPHER_AES_GCM_256_REC_SEQ_SIZE];
};

struct tls12_crypto_info_aes_ccm_128 {
	struct tls_crypto_info info;
	unsigned char iv[TLS_CIPHER_AES_CCM_128_IV_SIZE];
	unsigned char key[TLS_CIPHER_AES_CCM_128_KEY_SIZE];
	unsigned char salt[TLS_CIPHER_AES_CCM_128_SALT_SIZE];
	unsigned char rec_seq[TLS_CIPHER_AES_CCM_128_REC_SEQ_SIZE];
};

struct tls12_crypto_info_chacha20_poly1305 {
	struct tls_crypto_info info;
	unsigned char iv[TLS_CIPHER_CHACHA20_POLY1305_IV_SIZE];
	unsigned char key[TLS_CIPHER_CHACHA20_POLY1305_KEY_SIZE];
	unsigned char salt[TLS_CIPHER_CHACHA20_POLY1305_SALT_SIZE];
	unsigned char rec_seq[TLS_CIPHER_CHACHA20_POLY1305_REC_SEQ_SIZE];
};

enum {
	TLS_INFO_UNSPEC,
	TLS_INFO_VERSION,
	TLS_INFO_CIPHER,
	TLS_INFO_TXCONF,
	TLS_INFO_RXCONF,
	__TLS_INFO_MAX,
};
#define TLS_INFO_MAX (__TLS_INFO_MAX - 1)

#define TLS_CONF_BASE 1
#define TLS_CONF_SW 2
#define TLS_CONF_HW 3
#define TLS_CONF_HW_RECORD 4

#endif /* _UAPI_LINUX_TLS_H */
