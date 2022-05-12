/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2005-2010 IBM Corporation
 *
 * Authors:
 * Mimi Zohar <zohar@us.ibm.com>
 * Kylene Hall <kjhall@us.ibm.com>
 *
 * File: evm.h
 */

#ifndef __INTEGRITY_EVM_H
#define __INTEGRITY_EVM_H

#include <linux/xattr.h>
#include <linux/security.h>

#include "../integrity.h"

#define EVM_INIT_HMAC	0x0001
#define EVM_INIT_X509	0x0002
#define EVM_ALLOW_METADATA_WRITES	0x0004
#define EVM_SETUP_COMPLETE 0x80000000 /* userland has signaled key load */

#define EVM_KEY_MASK (EVM_INIT_HMAC | EVM_INIT_X509)
#define EVM_INIT_MASK (EVM_INIT_HMAC | EVM_INIT_X509 | EVM_SETUP_COMPLETE | \
		       EVM_ALLOW_METADATA_WRITES)

struct xattr_list {
	struct list_head list;
	char *name;
};

extern int evm_initialized;

#define EVM_ATTR_FSUUID		0x0001

extern int evm_hmac_attrs;

extern struct crypto_shash *hmac_tfm;
extern struct crypto_shash *hash_tfm;

/* List of EVM protected security xattrs */
extern struct list_head evm_config_xattrnames;

struct evm_digest {
	struct ima_digest_data hdr;
	char digest[IMA_MAX_DIGEST_SIZE];
} __packed;

int evm_init_key(void);
int evm_update_evmxattr(struct dentry *dentry,
			const char *req_xattr_name,
			const char *req_xattr_value,
			size_t req_xattr_value_len);
int evm_calc_hmac(struct dentry *dentry, const char *req_xattr_name,
		  const char *req_xattr_value,
		  size_t req_xattr_value_len, struct evm_digest *data);
int evm_calc_hash(struct dentry *dentry, const char *req_xattr_name,
		  const char *req_xattr_value,
		  size_t req_xattr_value_len, char type,
		  struct evm_digest *data);
int evm_init_hmac(struct inode *inode, const struct xattr *xattr,
		  char *hmac_val);
int evm_init_secfs(void);

#endif
