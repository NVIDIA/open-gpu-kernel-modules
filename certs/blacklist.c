// SPDX-License-Identifier: GPL-2.0-or-later
/* System hash blacklist.
 *
 * Copyright (C) 2016 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 */

#define pr_fmt(fmt) "blacklist: "fmt
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/key.h>
#include <linux/key-type.h>
#include <linux/sched.h>
#include <linux/ctype.h>
#include <linux/err.h>
#include <linux/seq_file.h>
#include <linux/uidgid.h>
#include <keys/system_keyring.h>
#include "blacklist.h"
#include "common.h"

static struct key *blacklist_keyring;

#ifdef CONFIG_SYSTEM_REVOCATION_LIST
extern __initconst const u8 revocation_certificate_list[];
extern __initconst const unsigned long revocation_certificate_list_size;
#endif

/*
 * The description must be a type prefix, a colon and then an even number of
 * hex digits.  The hash is kept in the description.
 */
static int blacklist_vet_description(const char *desc)
{
	int n = 0;

	if (*desc == ':')
		return -EINVAL;
	for (; *desc; desc++)
		if (*desc == ':')
			goto found_colon;
	return -EINVAL;

found_colon:
	desc++;
	for (; *desc; desc++) {
		if (!isxdigit(*desc) || isupper(*desc))
			return -EINVAL;
		n++;
	}

	if (n == 0 || n & 1)
		return -EINVAL;
	return 0;
}

/*
 * The hash to be blacklisted is expected to be in the description.  There will
 * be no payload.
 */
static int blacklist_preparse(struct key_preparsed_payload *prep)
{
	if (prep->datalen > 0)
		return -EINVAL;
	return 0;
}

static void blacklist_free_preparse(struct key_preparsed_payload *prep)
{
}

static void blacklist_describe(const struct key *key, struct seq_file *m)
{
	seq_puts(m, key->description);
}

static struct key_type key_type_blacklist = {
	.name			= "blacklist",
	.vet_description	= blacklist_vet_description,
	.preparse		= blacklist_preparse,
	.free_preparse		= blacklist_free_preparse,
	.instantiate		= generic_key_instantiate,
	.describe		= blacklist_describe,
};

/**
 * mark_hash_blacklisted - Add a hash to the system blacklist
 * @hash: The hash as a hex string with a type prefix (eg. "tbs:23aa429783")
 */
int mark_hash_blacklisted(const char *hash)
{
	key_ref_t key;

	key = key_create_or_update(make_key_ref(blacklist_keyring, true),
				   "blacklist",
				   hash,
				   NULL,
				   0,
				   ((KEY_POS_ALL & ~KEY_POS_SETATTR) |
				    KEY_USR_VIEW),
				   KEY_ALLOC_NOT_IN_QUOTA |
				   KEY_ALLOC_BUILT_IN);
	if (IS_ERR(key)) {
		pr_err("Problem blacklisting hash (%ld)\n", PTR_ERR(key));
		return PTR_ERR(key);
	}
	return 0;
}

/**
 * is_hash_blacklisted - Determine if a hash is blacklisted
 * @hash: The hash to be checked as a binary blob
 * @hash_len: The length of the binary hash
 * @type: Type of hash
 */
int is_hash_blacklisted(const u8 *hash, size_t hash_len, const char *type)
{
	key_ref_t kref;
	size_t type_len = strlen(type);
	char *buffer, *p;
	int ret = 0;

	buffer = kmalloc(type_len + 1 + hash_len * 2 + 1, GFP_KERNEL);
	if (!buffer)
		return -ENOMEM;
	p = memcpy(buffer, type, type_len);
	p += type_len;
	*p++ = ':';
	bin2hex(p, hash, hash_len);
	p += hash_len * 2;
	*p = 0;

	kref = keyring_search(make_key_ref(blacklist_keyring, true),
			      &key_type_blacklist, buffer, false);
	if (!IS_ERR(kref)) {
		key_ref_put(kref);
		ret = -EKEYREJECTED;
	}

	kfree(buffer);
	return ret;
}
EXPORT_SYMBOL_GPL(is_hash_blacklisted);

int is_binary_blacklisted(const u8 *hash, size_t hash_len)
{
	if (is_hash_blacklisted(hash, hash_len, "bin") == -EKEYREJECTED)
		return -EPERM;

	return 0;
}
EXPORT_SYMBOL_GPL(is_binary_blacklisted);

#ifdef CONFIG_SYSTEM_REVOCATION_LIST
/**
 * add_key_to_revocation_list - Add a revocation certificate to the blacklist
 * @data: The data blob containing the certificate
 * @size: The size of data blob
 */
int add_key_to_revocation_list(const char *data, size_t size)
{
	key_ref_t key;

	key = key_create_or_update(make_key_ref(blacklist_keyring, true),
				   "asymmetric",
				   NULL,
				   data,
				   size,
				   ((KEY_POS_ALL & ~KEY_POS_SETATTR) | KEY_USR_VIEW),
				   KEY_ALLOC_NOT_IN_QUOTA | KEY_ALLOC_BUILT_IN);

	if (IS_ERR(key)) {
		pr_err("Problem with revocation key (%ld)\n", PTR_ERR(key));
		return PTR_ERR(key);
	}

	return 0;
}

/**
 * is_key_on_revocation_list - Determine if the key for a PKCS#7 message is revoked
 * @pkcs7: The PKCS#7 message to check
 */
int is_key_on_revocation_list(struct pkcs7_message *pkcs7)
{
	int ret;

	ret = pkcs7_validate_trust(pkcs7, blacklist_keyring);

	if (ret == 0)
		return -EKEYREJECTED;

	return -ENOKEY;
}
#endif

/*
 * Initialise the blacklist
 */
static int __init blacklist_init(void)
{
	const char *const *bl;

	if (register_key_type(&key_type_blacklist) < 0)
		panic("Can't allocate system blacklist key type\n");

	blacklist_keyring =
		keyring_alloc(".blacklist",
			      GLOBAL_ROOT_UID, GLOBAL_ROOT_GID, current_cred(),
			      (KEY_POS_ALL & ~KEY_POS_SETATTR) |
			      KEY_USR_VIEW | KEY_USR_READ |
			      KEY_USR_SEARCH,
			      KEY_ALLOC_NOT_IN_QUOTA |
			      KEY_ALLOC_SET_KEEP,
			      NULL, NULL);
	if (IS_ERR(blacklist_keyring))
		panic("Can't allocate system blacklist keyring\n");

	for (bl = blacklist_hashes; *bl; bl++)
		if (mark_hash_blacklisted(*bl) < 0)
			pr_err("- blacklisting failed\n");
	return 0;
}

/*
 * Must be initialised before we try and load the keys into the keyring.
 */
device_initcall(blacklist_init);

#ifdef CONFIG_SYSTEM_REVOCATION_LIST
/*
 * Load the compiled-in list of revocation X.509 certificates.
 */
static __init int load_revocation_certificate_list(void)
{
	if (revocation_certificate_list_size)
		pr_notice("Loading compiled-in revocation X.509 certificates\n");

	return load_certificate_list(revocation_certificate_list, revocation_certificate_list_size,
				     blacklist_keyring);
}
late_initcall(load_revocation_certificate_list);
#endif
