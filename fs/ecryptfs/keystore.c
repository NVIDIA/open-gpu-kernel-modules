// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * eCryptfs: Linux filesystem encryption layer
 * In-kernel key management code.  Includes functions to parse and
 * write authentication token-related packets with the underlying
 * file.
 *
 * Copyright (C) 2004-2006 International Business Machines Corp.
 *   Author(s): Michael A. Halcrow <mhalcrow@us.ibm.com>
 *              Michael C. Thompson <mcthomps@us.ibm.com>
 *              Trevor S. Highland <trevor.highland@gmail.com>
 */

#include <crypto/hash.h>
#include <crypto/skcipher.h>
#include <linux/string.h>
#include <linux/pagemap.h>
#include <linux/key.h>
#include <linux/random.h>
#include <linux/scatterlist.h>
#include <linux/slab.h>
#include "ecryptfs_kernel.h"

/*
 * request_key returned an error instead of a valid key address;
 * determine the type of error, make appropriate log entries, and
 * return an error code.
 */
static int process_request_key_err(long err_code)
{
	int rc = 0;

	switch (err_code) {
	case -ENOKEY:
		ecryptfs_printk(KERN_WARNING, "No key\n");
		rc = -ENOENT;
		break;
	case -EKEYEXPIRED:
		ecryptfs_printk(KERN_WARNING, "Key expired\n");
		rc = -ETIME;
		break;
	case -EKEYREVOKED:
		ecryptfs_printk(KERN_WARNING, "Key revoked\n");
		rc = -EINVAL;
		break;
	default:
		ecryptfs_printk(KERN_WARNING, "Unknown error code: "
				"[0x%.16lx]\n", err_code);
		rc = -EINVAL;
	}
	return rc;
}

static int process_find_global_auth_tok_for_sig_err(int err_code)
{
	int rc = err_code;

	switch (err_code) {
	case -ENOENT:
		ecryptfs_printk(KERN_WARNING, "Missing auth tok\n");
		break;
	case -EINVAL:
		ecryptfs_printk(KERN_WARNING, "Invalid auth tok\n");
		break;
	default:
		rc = process_request_key_err(err_code);
		break;
	}
	return rc;
}

/**
 * ecryptfs_parse_packet_length
 * @data: Pointer to memory containing length at offset
 * @size: This function writes the decoded size to this memory
 *        address; zero on error
 * @length_size: The number of bytes occupied by the encoded length
 *
 * Returns zero on success; non-zero on error
 */
int ecryptfs_parse_packet_length(unsigned char *data, size_t *size,
				 size_t *length_size)
{
	int rc = 0;

	(*length_size) = 0;
	(*size) = 0;
	if (data[0] < 192) {
		/* One-byte length */
		(*size) = data[0];
		(*length_size) = 1;
	} else if (data[0] < 224) {
		/* Two-byte length */
		(*size) = (data[0] - 192) * 256;
		(*size) += data[1] + 192;
		(*length_size) = 2;
	} else if (data[0] == 255) {
		/* If support is added, adjust ECRYPTFS_MAX_PKT_LEN_SIZE */
		ecryptfs_printk(KERN_ERR, "Five-byte packet length not "
				"supported\n");
		rc = -EINVAL;
		goto out;
	} else {
		ecryptfs_printk(KERN_ERR, "Error parsing packet length\n");
		rc = -EINVAL;
		goto out;
	}
out:
	return rc;
}

/**
 * ecryptfs_write_packet_length
 * @dest: The byte array target into which to write the length. Must
 *        have at least ECRYPTFS_MAX_PKT_LEN_SIZE bytes allocated.
 * @size: The length to write.
 * @packet_size_length: The number of bytes used to encode the packet
 *                      length is written to this address.
 *
 * Returns zero on success; non-zero on error.
 */
int ecryptfs_write_packet_length(char *dest, size_t size,
				 size_t *packet_size_length)
{
	int rc = 0;

	if (size < 192) {
		dest[0] = size;
		(*packet_size_length) = 1;
	} else if (size < 65536) {
		dest[0] = (((size - 192) / 256) + 192);
		dest[1] = ((size - 192) % 256);
		(*packet_size_length) = 2;
	} else {
		/* If support is added, adjust ECRYPTFS_MAX_PKT_LEN_SIZE */
		rc = -EINVAL;
		ecryptfs_printk(KERN_WARNING,
				"Unsupported packet size: [%zd]\n", size);
	}
	return rc;
}

static int
write_tag_64_packet(char *signature, struct ecryptfs_session_key *session_key,
		    char **packet, size_t *packet_len)
{
	size_t i = 0;
	size_t data_len;
	size_t packet_size_len;
	char *message;
	int rc;

	/*
	 *              ***** TAG 64 Packet Format *****
	 *    | Content Type                       | 1 byte       |
	 *    | Key Identifier Size                | 1 or 2 bytes |
	 *    | Key Identifier                     | arbitrary    |
	 *    | Encrypted File Encryption Key Size | 1 or 2 bytes |
	 *    | Encrypted File Encryption Key      | arbitrary    |
	 */
	data_len = (5 + ECRYPTFS_SIG_SIZE_HEX
		    + session_key->encrypted_key_size);
	*packet = kmalloc(data_len, GFP_KERNEL);
	message = *packet;
	if (!message) {
		ecryptfs_printk(KERN_ERR, "Unable to allocate memory\n");
		rc = -ENOMEM;
		goto out;
	}
	message[i++] = ECRYPTFS_TAG_64_PACKET_TYPE;
	rc = ecryptfs_write_packet_length(&message[i], ECRYPTFS_SIG_SIZE_HEX,
					  &packet_size_len);
	if (rc) {
		ecryptfs_printk(KERN_ERR, "Error generating tag 64 packet "
				"header; cannot generate packet length\n");
		goto out;
	}
	i += packet_size_len;
	memcpy(&message[i], signature, ECRYPTFS_SIG_SIZE_HEX);
	i += ECRYPTFS_SIG_SIZE_HEX;
	rc = ecryptfs_write_packet_length(&message[i],
					  session_key->encrypted_key_size,
					  &packet_size_len);
	if (rc) {
		ecryptfs_printk(KERN_ERR, "Error generating tag 64 packet "
				"header; cannot generate packet length\n");
		goto out;
	}
	i += packet_size_len;
	memcpy(&message[i], session_key->encrypted_key,
	       session_key->encrypted_key_size);
	i += session_key->encrypted_key_size;
	*packet_len = i;
out:
	return rc;
}

static int
parse_tag_65_packet(struct ecryptfs_session_key *session_key, u8 *cipher_code,
		    struct ecryptfs_message *msg)
{
	size_t i = 0;
	char *data;
	size_t data_len;
	size_t m_size;
	size_t message_len;
	u16 checksum = 0;
	u16 expected_checksum = 0;
	int rc;

	/*
	 *              ***** TAG 65 Packet Format *****
	 *         | Content Type             | 1 byte       |
	 *         | Status Indicator         | 1 byte       |
	 *         | File Encryption Key Size | 1 or 2 bytes |
	 *         | File Encryption Key      | arbitrary    |
	 */
	message_len = msg->data_len;
	data = msg->data;
	if (message_len < 4) {
		rc = -EIO;
		goto out;
	}
	if (data[i++] != ECRYPTFS_TAG_65_PACKET_TYPE) {
		ecryptfs_printk(KERN_ERR, "Type should be ECRYPTFS_TAG_65\n");
		rc = -EIO;
		goto out;
	}
	if (data[i++]) {
		ecryptfs_printk(KERN_ERR, "Status indicator has non-zero value "
				"[%d]\n", data[i-1]);
		rc = -EIO;
		goto out;
	}
	rc = ecryptfs_parse_packet_length(&data[i], &m_size, &data_len);
	if (rc) {
		ecryptfs_printk(KERN_WARNING, "Error parsing packet length; "
				"rc = [%d]\n", rc);
		goto out;
	}
	i += data_len;
	if (message_len < (i + m_size)) {
		ecryptfs_printk(KERN_ERR, "The message received from ecryptfsd "
				"is shorter than expected\n");
		rc = -EIO;
		goto out;
	}
	if (m_size < 3) {
		ecryptfs_printk(KERN_ERR,
				"The decrypted key is not long enough to "
				"include a cipher code and checksum\n");
		rc = -EIO;
		goto out;
	}
	*cipher_code = data[i++];
	/* The decrypted key includes 1 byte cipher code and 2 byte checksum */
	session_key->decrypted_key_size = m_size - 3;
	if (session_key->decrypted_key_size > ECRYPTFS_MAX_KEY_BYTES) {
		ecryptfs_printk(KERN_ERR, "key_size [%d] larger than "
				"the maximum key size [%d]\n",
				session_key->decrypted_key_size,
				ECRYPTFS_MAX_ENCRYPTED_KEY_BYTES);
		rc = -EIO;
		goto out;
	}
	memcpy(session_key->decrypted_key, &data[i],
	       session_key->decrypted_key_size);
	i += session_key->decrypted_key_size;
	expected_checksum += (unsigned char)(data[i++]) << 8;
	expected_checksum += (unsigned char)(data[i++]);
	for (i = 0; i < session_key->decrypted_key_size; i++)
		checksum += session_key->decrypted_key[i];
	if (expected_checksum != checksum) {
		ecryptfs_printk(KERN_ERR, "Invalid checksum for file "
				"encryption  key; expected [%x]; calculated "
				"[%x]\n", expected_checksum, checksum);
		rc = -EIO;
	}
out:
	return rc;
}


static int
write_tag_66_packet(char *signature, u8 cipher_code,
		    struct ecryptfs_crypt_stat *crypt_stat, char **packet,
		    size_t *packet_len)
{
	size_t i = 0;
	size_t j;
	size_t data_len;
	size_t checksum = 0;
	size_t packet_size_len;
	char *message;
	int rc;

	/*
	 *              ***** TAG 66 Packet Format *****
	 *         | Content Type             | 1 byte       |
	 *         | Key Identifier Size      | 1 or 2 bytes |
	 *         | Key Identifier           | arbitrary    |
	 *         | File Encryption Key Size | 1 or 2 bytes |
	 *         | File Encryption Key      | arbitrary    |
	 */
	data_len = (5 + ECRYPTFS_SIG_SIZE_HEX + crypt_stat->key_size);
	*packet = kmalloc(data_len, GFP_KERNEL);
	message = *packet;
	if (!message) {
		ecryptfs_printk(KERN_ERR, "Unable to allocate memory\n");
		rc = -ENOMEM;
		goto out;
	}
	message[i++] = ECRYPTFS_TAG_66_PACKET_TYPE;
	rc = ecryptfs_write_packet_length(&message[i], ECRYPTFS_SIG_SIZE_HEX,
					  &packet_size_len);
	if (rc) {
		ecryptfs_printk(KERN_ERR, "Error generating tag 66 packet "
				"header; cannot generate packet length\n");
		goto out;
	}
	i += packet_size_len;
	memcpy(&message[i], signature, ECRYPTFS_SIG_SIZE_HEX);
	i += ECRYPTFS_SIG_SIZE_HEX;
	/* The encrypted key includes 1 byte cipher code and 2 byte checksum */
	rc = ecryptfs_write_packet_length(&message[i], crypt_stat->key_size + 3,
					  &packet_size_len);
	if (rc) {
		ecryptfs_printk(KERN_ERR, "Error generating tag 66 packet "
				"header; cannot generate packet length\n");
		goto out;
	}
	i += packet_size_len;
	message[i++] = cipher_code;
	memcpy(&message[i], crypt_stat->key, crypt_stat->key_size);
	i += crypt_stat->key_size;
	for (j = 0; j < crypt_stat->key_size; j++)
		checksum += crypt_stat->key[j];
	message[i++] = (checksum / 256) % 256;
	message[i++] = (checksum % 256);
	*packet_len = i;
out:
	return rc;
}

static int
parse_tag_67_packet(struct ecryptfs_key_record *key_rec,
		    struct ecryptfs_message *msg)
{
	size_t i = 0;
	char *data;
	size_t data_len;
	size_t message_len;
	int rc;

	/*
	 *              ***** TAG 65 Packet Format *****
	 *    | Content Type                       | 1 byte       |
	 *    | Status Indicator                   | 1 byte       |
	 *    | Encrypted File Encryption Key Size | 1 or 2 bytes |
	 *    | Encrypted File Encryption Key      | arbitrary    |
	 */
	message_len = msg->data_len;
	data = msg->data;
	/* verify that everything through the encrypted FEK size is present */
	if (message_len < 4) {
		rc = -EIO;
		printk(KERN_ERR "%s: message_len is [%zd]; minimum acceptable "
		       "message length is [%d]\n", __func__, message_len, 4);
		goto out;
	}
	if (data[i++] != ECRYPTFS_TAG_67_PACKET_TYPE) {
		rc = -EIO;
		printk(KERN_ERR "%s: Type should be ECRYPTFS_TAG_67\n",
		       __func__);
		goto out;
	}
	if (data[i++]) {
		rc = -EIO;
		printk(KERN_ERR "%s: Status indicator has non zero "
		       "value [%d]\n", __func__, data[i-1]);

		goto out;
	}
	rc = ecryptfs_parse_packet_length(&data[i], &key_rec->enc_key_size,
					  &data_len);
	if (rc) {
		ecryptfs_printk(KERN_WARNING, "Error parsing packet length; "
				"rc = [%d]\n", rc);
		goto out;
	}
	i += data_len;
	if (message_len < (i + key_rec->enc_key_size)) {
		rc = -EIO;
		printk(KERN_ERR "%s: message_len [%zd]; max len is [%zd]\n",
		       __func__, message_len, (i + key_rec->enc_key_size));
		goto out;
	}
	if (key_rec->enc_key_size > ECRYPTFS_MAX_ENCRYPTED_KEY_BYTES) {
		rc = -EIO;
		printk(KERN_ERR "%s: Encrypted key_size [%zd] larger than "
		       "the maximum key size [%d]\n", __func__,
		       key_rec->enc_key_size,
		       ECRYPTFS_MAX_ENCRYPTED_KEY_BYTES);
		goto out;
	}
	memcpy(key_rec->enc_key, &data[i], key_rec->enc_key_size);
out:
	return rc;
}

/**
 * ecryptfs_verify_version
 * @version: The version number to confirm
 *
 * Returns zero on good version; non-zero otherwise
 */
static int ecryptfs_verify_version(u16 version)
{
	int rc = 0;
	unsigned char major;
	unsigned char minor;

	major = ((version >> 8) & 0xFF);
	minor = (version & 0xFF);
	if (major != ECRYPTFS_VERSION_MAJOR) {
		ecryptfs_printk(KERN_ERR, "Major version number mismatch. "
				"Expected [%d]; got [%d]\n",
				ECRYPTFS_VERSION_MAJOR, major);
		rc = -EINVAL;
		goto out;
	}
	if (minor != ECRYPTFS_VERSION_MINOR) {
		ecryptfs_printk(KERN_ERR, "Minor version number mismatch. "
				"Expected [%d]; got [%d]\n",
				ECRYPTFS_VERSION_MINOR, minor);
		rc = -EINVAL;
		goto out;
	}
out:
	return rc;
}

/**
 * ecryptfs_verify_auth_tok_from_key
 * @auth_tok_key: key containing the authentication token
 * @auth_tok: authentication token
 *
 * Returns zero on valid auth tok; -EINVAL if the payload is invalid; or
 * -EKEYREVOKED if the key was revoked before we acquired its semaphore.
 */
static int
ecryptfs_verify_auth_tok_from_key(struct key *auth_tok_key,
				  struct ecryptfs_auth_tok **auth_tok)
{
	int rc = 0;

	(*auth_tok) = ecryptfs_get_key_payload_data(auth_tok_key);
	if (IS_ERR(*auth_tok)) {
		rc = PTR_ERR(*auth_tok);
		*auth_tok = NULL;
		goto out;
	}

	if (ecryptfs_verify_version((*auth_tok)->version)) {
		printk(KERN_ERR "Data structure version mismatch. Userspace "
		       "tools must match eCryptfs kernel module with major "
		       "version [%d] and minor version [%d]\n",
		       ECRYPTFS_VERSION_MAJOR, ECRYPTFS_VERSION_MINOR);
		rc = -EINVAL;
		goto out;
	}
	if ((*auth_tok)->token_type != ECRYPTFS_PASSWORD
	    && (*auth_tok)->token_type != ECRYPTFS_PRIVATE_KEY) {
		printk(KERN_ERR "Invalid auth_tok structure "
		       "returned from key query\n");
		rc = -EINVAL;
		goto out;
	}
out:
	return rc;
}

static int
ecryptfs_find_global_auth_tok_for_sig(
	struct key **auth_tok_key,
	struct ecryptfs_auth_tok **auth_tok,
	struct ecryptfs_mount_crypt_stat *mount_crypt_stat, char *sig)
{
	struct ecryptfs_global_auth_tok *walker;
	int rc = 0;

	(*auth_tok_key) = NULL;
	(*auth_tok) = NULL;
	mutex_lock(&mount_crypt_stat->global_auth_tok_list_mutex);
	list_for_each_entry(walker,
			    &mount_crypt_stat->global_auth_tok_list,
			    mount_crypt_stat_list) {
		if (memcmp(walker->sig, sig, ECRYPTFS_SIG_SIZE_HEX))
			continue;

		if (walker->flags & ECRYPTFS_AUTH_TOK_INVALID) {
			rc = -EINVAL;
			goto out;
		}

		rc = key_validate(walker->global_auth_tok_key);
		if (rc) {
			if (rc == -EKEYEXPIRED)
				goto out;
			goto out_invalid_auth_tok;
		}

		down_write(&(walker->global_auth_tok_key->sem));
		rc = ecryptfs_verify_auth_tok_from_key(
				walker->global_auth_tok_key, auth_tok);
		if (rc)
			goto out_invalid_auth_tok_unlock;

		(*auth_tok_key) = walker->global_auth_tok_key;
		key_get(*auth_tok_key);
		goto out;
	}
	rc = -ENOENT;
	goto out;
out_invalid_auth_tok_unlock:
	up_write(&(walker->global_auth_tok_key->sem));
out_invalid_auth_tok:
	printk(KERN_WARNING "Invalidating auth tok with sig = [%s]\n", sig);
	walker->flags |= ECRYPTFS_AUTH_TOK_INVALID;
	key_put(walker->global_auth_tok_key);
	walker->global_auth_tok_key = NULL;
out:
	mutex_unlock(&mount_crypt_stat->global_auth_tok_list_mutex);
	return rc;
}

/**
 * ecryptfs_find_auth_tok_for_sig
 * @auth_tok_key: key containing the authentication token
 * @auth_tok: Set to the matching auth_tok; NULL if not found
 * @mount_crypt_stat: inode crypt_stat crypto context
 * @sig: Sig of auth_tok to find
 *
 * For now, this function simply looks at the registered auth_tok's
 * linked off the mount_crypt_stat, so all the auth_toks that can be
 * used must be registered at mount time. This function could
 * potentially try a lot harder to find auth_tok's (e.g., by calling
 * out to ecryptfsd to dynamically retrieve an auth_tok object) so
 * that static registration of auth_tok's will no longer be necessary.
 *
 * Returns zero on no error; non-zero on error
 */
static int
ecryptfs_find_auth_tok_for_sig(
	struct key **auth_tok_key,
	struct ecryptfs_auth_tok **auth_tok,
	struct ecryptfs_mount_crypt_stat *mount_crypt_stat,
	char *sig)
{
	int rc = 0;

	rc = ecryptfs_find_global_auth_tok_for_sig(auth_tok_key, auth_tok,
						   mount_crypt_stat, sig);
	if (rc == -ENOENT) {
		/* if the flag ECRYPTFS_GLOBAL_MOUNT_AUTH_TOK_ONLY is set in the
		 * mount_crypt_stat structure, we prevent to use auth toks that
		 * are not inserted through the ecryptfs_add_global_auth_tok
		 * function.
		 */
		if (mount_crypt_stat->flags
				& ECRYPTFS_GLOBAL_MOUNT_AUTH_TOK_ONLY)
			return -EINVAL;

		rc = ecryptfs_keyring_auth_tok_for_sig(auth_tok_key, auth_tok,
						       sig);
	}
	return rc;
}

/*
 * write_tag_70_packet can gobble a lot of stack space. We stuff most
 * of the function's parameters in a kmalloc'd struct to help reduce
 * eCryptfs' overall stack usage.
 */
struct ecryptfs_write_tag_70_packet_silly_stack {
	u8 cipher_code;
	size_t max_packet_size;
	size_t packet_size_len;
	size_t block_aligned_filename_size;
	size_t block_size;
	size_t i;
	size_t j;
	size_t num_rand_bytes;
	struct mutex *tfm_mutex;
	char *block_aligned_filename;
	struct ecryptfs_auth_tok *auth_tok;
	struct scatterlist src_sg[2];
	struct scatterlist dst_sg[2];
	struct crypto_skcipher *skcipher_tfm;
	struct skcipher_request *skcipher_req;
	char iv[ECRYPTFS_MAX_IV_BYTES];
	char hash[ECRYPTFS_TAG_70_DIGEST_SIZE];
	char tmp_hash[ECRYPTFS_TAG_70_DIGEST_SIZE];
	struct crypto_shash *hash_tfm;
	struct shash_desc *hash_desc;
};

/*
 * write_tag_70_packet - Write encrypted filename (EFN) packet against FNEK
 * @filename: NULL-terminated filename string
 *
 * This is the simplest mechanism for achieving filename encryption in
 * eCryptfs. It encrypts the given filename with the mount-wide
 * filename encryption key (FNEK) and stores it in a packet to @dest,
 * which the callee will encode and write directly into the dentry
 * name.
 */
int
ecryptfs_write_tag_70_packet(char *dest, size_t *remaining_bytes,
			     size_t *packet_size,
			     struct ecryptfs_mount_crypt_stat *mount_crypt_stat,
			     char *filename, size_t filename_size)
{
	struct ecryptfs_write_tag_70_packet_silly_stack *s;
	struct key *auth_tok_key = NULL;
	int rc = 0;

	s = kzalloc(sizeof(*s), GFP_KERNEL);
	if (!s)
		return -ENOMEM;

	(*packet_size) = 0;
	rc = ecryptfs_find_auth_tok_for_sig(
		&auth_tok_key,
		&s->auth_tok, mount_crypt_stat,
		mount_crypt_stat->global_default_fnek_sig);
	if (rc) {
		printk(KERN_ERR "%s: Error attempting to find auth tok for "
		       "fnek sig [%s]; rc = [%d]\n", __func__,
		       mount_crypt_stat->global_default_fnek_sig, rc);
		goto out;
	}
	rc = ecryptfs_get_tfm_and_mutex_for_cipher_name(
		&s->skcipher_tfm,
		&s->tfm_mutex, mount_crypt_stat->global_default_fn_cipher_name);
	if (unlikely(rc)) {
		printk(KERN_ERR "Internal error whilst attempting to get "
		       "tfm and mutex for cipher name [%s]; rc = [%d]\n",
		       mount_crypt_stat->global_default_fn_cipher_name, rc);
		goto out;
	}
	mutex_lock(s->tfm_mutex);
	s->block_size = crypto_skcipher_blocksize(s->skcipher_tfm);
	/* Plus one for the \0 separator between the random prefix
	 * and the plaintext filename */
	s->num_rand_bytes = (ECRYPTFS_FILENAME_MIN_RANDOM_PREPEND_BYTES + 1);
	s->block_aligned_filename_size = (s->num_rand_bytes + filename_size);
	if ((s->block_aligned_filename_size % s->block_size) != 0) {
		s->num_rand_bytes += (s->block_size
				      - (s->block_aligned_filename_size
					 % s->block_size));
		s->block_aligned_filename_size = (s->num_rand_bytes
						  + filename_size);
	}
	/* Octet 0: Tag 70 identifier
	 * Octets 1-N1: Tag 70 packet size (includes cipher identifier
	 *              and block-aligned encrypted filename size)
	 * Octets N1-N2: FNEK sig (ECRYPTFS_SIG_SIZE)
	 * Octet N2-N3: Cipher identifier (1 octet)
	 * Octets N3-N4: Block-aligned encrypted filename
	 *  - Consists of a minimum number of random characters, a \0
	 *    separator, and then the filename */
	s->max_packet_size = (ECRYPTFS_TAG_70_MAX_METADATA_SIZE
			      + s->block_aligned_filename_size);
	if (!dest) {
		(*packet_size) = s->max_packet_size;
		goto out_unlock;
	}
	if (s->max_packet_size > (*remaining_bytes)) {
		printk(KERN_WARNING "%s: Require [%zd] bytes to write; only "
		       "[%zd] available\n", __func__, s->max_packet_size,
		       (*remaining_bytes));
		rc = -EINVAL;
		goto out_unlock;
	}

	s->skcipher_req = skcipher_request_alloc(s->skcipher_tfm, GFP_KERNEL);
	if (!s->skcipher_req) {
		printk(KERN_ERR "%s: Out of kernel memory whilst attempting to "
		       "skcipher_request_alloc for %s\n", __func__,
		       crypto_skcipher_driver_name(s->skcipher_tfm));
		rc = -ENOMEM;
		goto out_unlock;
	}

	skcipher_request_set_callback(s->skcipher_req,
				      CRYPTO_TFM_REQ_MAY_SLEEP, NULL, NULL);

	s->block_aligned_filename = kzalloc(s->block_aligned_filename_size,
					    GFP_KERNEL);
	if (!s->block_aligned_filename) {
		rc = -ENOMEM;
		goto out_unlock;
	}
	dest[s->i++] = ECRYPTFS_TAG_70_PACKET_TYPE;
	rc = ecryptfs_write_packet_length(&dest[s->i],
					  (ECRYPTFS_SIG_SIZE
					   + 1 /* Cipher code */
					   + s->block_aligned_filename_size),
					  &s->packet_size_len);
	if (rc) {
		printk(KERN_ERR "%s: Error generating tag 70 packet "
		       "header; cannot generate packet length; rc = [%d]\n",
		       __func__, rc);
		goto out_free_unlock;
	}
	s->i += s->packet_size_len;
	ecryptfs_from_hex(&dest[s->i],
			  mount_crypt_stat->global_default_fnek_sig,
			  ECRYPTFS_SIG_SIZE);
	s->i += ECRYPTFS_SIG_SIZE;
	s->cipher_code = ecryptfs_code_for_cipher_string(
		mount_crypt_stat->global_default_fn_cipher_name,
		mount_crypt_stat->global_default_fn_cipher_key_bytes);
	if (s->cipher_code == 0) {
		printk(KERN_WARNING "%s: Unable to generate code for "
		       "cipher [%s] with key bytes [%zd]\n", __func__,
		       mount_crypt_stat->global_default_fn_cipher_name,
		       mount_crypt_stat->global_default_fn_cipher_key_bytes);
		rc = -EINVAL;
		goto out_free_unlock;
	}
	dest[s->i++] = s->cipher_code;
	/* TODO: Support other key modules than passphrase for
	 * filename encryption */
	if (s->auth_tok->token_type != ECRYPTFS_PASSWORD) {
		rc = -EOPNOTSUPP;
		printk(KERN_INFO "%s: Filename encryption only supports "
		       "password tokens\n", __func__);
		goto out_free_unlock;
	}
	s->hash_tfm = crypto_alloc_shash(ECRYPTFS_TAG_70_DIGEST, 0, 0);
	if (IS_ERR(s->hash_tfm)) {
			rc = PTR_ERR(s->hash_tfm);
			printk(KERN_ERR "%s: Error attempting to "
			       "allocate hash crypto context; rc = [%d]\n",
			       __func__, rc);
			goto out_free_unlock;
	}

	s->hash_desc = kmalloc(sizeof(*s->hash_desc) +
			       crypto_shash_descsize(s->hash_tfm), GFP_KERNEL);
	if (!s->hash_desc) {
		rc = -ENOMEM;
		goto out_release_free_unlock;
	}

	s->hash_desc->tfm = s->hash_tfm;

	rc = crypto_shash_digest(s->hash_desc,
				 (u8 *)s->auth_tok->token.password.session_key_encryption_key,
				 s->auth_tok->token.password.session_key_encryption_key_bytes,
				 s->hash);
	if (rc) {
		printk(KERN_ERR
		       "%s: Error computing crypto hash; rc = [%d]\n",
		       __func__, rc);
		goto out_release_free_unlock;
	}
	for (s->j = 0; s->j < (s->num_rand_bytes - 1); s->j++) {
		s->block_aligned_filename[s->j] =
			s->hash[(s->j % ECRYPTFS_TAG_70_DIGEST_SIZE)];
		if ((s->j % ECRYPTFS_TAG_70_DIGEST_SIZE)
		    == (ECRYPTFS_TAG_70_DIGEST_SIZE - 1)) {
			rc = crypto_shash_digest(s->hash_desc, (u8 *)s->hash,
						ECRYPTFS_TAG_70_DIGEST_SIZE,
						s->tmp_hash);
			if (rc) {
				printk(KERN_ERR
				       "%s: Error computing crypto hash; "
				       "rc = [%d]\n", __func__, rc);
				goto out_release_free_unlock;
			}
			memcpy(s->hash, s->tmp_hash,
			       ECRYPTFS_TAG_70_DIGEST_SIZE);
		}
		if (s->block_aligned_filename[s->j] == '\0')
			s->block_aligned_filename[s->j] = ECRYPTFS_NON_NULL;
	}
	memcpy(&s->block_aligned_filename[s->num_rand_bytes], filename,
	       filename_size);
	rc = virt_to_scatterlist(s->block_aligned_filename,
				 s->block_aligned_filename_size, s->src_sg, 2);
	if (rc < 1) {
		printk(KERN_ERR "%s: Internal error whilst attempting to "
		       "convert filename memory to scatterlist; rc = [%d]. "
		       "block_aligned_filename_size = [%zd]\n", __func__, rc,
		       s->block_aligned_filename_size);
		goto out_release_free_unlock;
	}
	rc = virt_to_scatterlist(&dest[s->i], s->block_aligned_filename_size,
				 s->dst_sg, 2);
	if (rc < 1) {
		printk(KERN_ERR "%s: Internal error whilst attempting to "
		       "convert encrypted filename memory to scatterlist; "
		       "rc = [%d]. block_aligned_filename_size = [%zd]\n",
		       __func__, rc, s->block_aligned_filename_size);
		goto out_release_free_unlock;
	}
	/* The characters in the first block effectively do the job
	 * of the IV here, so we just use 0's for the IV. Note the
	 * constraint that ECRYPTFS_FILENAME_MIN_RANDOM_PREPEND_BYTES
	 * >= ECRYPTFS_MAX_IV_BYTES. */
	rc = crypto_skcipher_setkey(
		s->skcipher_tfm,
		s->auth_tok->token.password.session_key_encryption_key,
		mount_crypt_stat->global_default_fn_cipher_key_bytes);
	if (rc < 0) {
		printk(KERN_ERR "%s: Error setting key for crypto context; "
		       "rc = [%d]. s->auth_tok->token.password.session_key_"
		       "encryption_key = [0x%p]; mount_crypt_stat->"
		       "global_default_fn_cipher_key_bytes = [%zd]\n", __func__,
		       rc,
		       s->auth_tok->token.password.session_key_encryption_key,
		       mount_crypt_stat->global_default_fn_cipher_key_bytes);
		goto out_release_free_unlock;
	}
	skcipher_request_set_crypt(s->skcipher_req, s->src_sg, s->dst_sg,
				   s->block_aligned_filename_size, s->iv);
	rc = crypto_skcipher_encrypt(s->skcipher_req);
	if (rc) {
		printk(KERN_ERR "%s: Error attempting to encrypt filename; "
		       "rc = [%d]\n", __func__, rc);
		goto out_release_free_unlock;
	}
	s->i += s->block_aligned_filename_size;
	(*packet_size) = s->i;
	(*remaining_bytes) -= (*packet_size);
out_release_free_unlock:
	crypto_free_shash(s->hash_tfm);
out_free_unlock:
	kfree_sensitive(s->block_aligned_filename);
out_unlock:
	mutex_unlock(s->tfm_mutex);
out:
	if (auth_tok_key) {
		up_write(&(auth_tok_key->sem));
		key_put(auth_tok_key);
	}
	skcipher_request_free(s->skcipher_req);
	kfree_sensitive(s->hash_desc);
	kfree(s);
	return rc;
}

struct ecryptfs_parse_tag_70_packet_silly_stack {
	u8 cipher_code;
	size_t max_packet_size;
	size_t packet_size_len;
	size_t parsed_tag_70_packet_size;
	size_t block_aligned_filename_size;
	size_t block_size;
	size_t i;
	struct mutex *tfm_mutex;
	char *decrypted_filename;
	struct ecryptfs_auth_tok *auth_tok;
	struct scatterlist src_sg[2];
	struct scatterlist dst_sg[2];
	struct crypto_skcipher *skcipher_tfm;
	struct skcipher_request *skcipher_req;
	char fnek_sig_hex[ECRYPTFS_SIG_SIZE_HEX + 1];
	char iv[ECRYPTFS_MAX_IV_BYTES];
	char cipher_string[ECRYPTFS_MAX_CIPHER_NAME_SIZE + 1];
};

/**
 * ecryptfs_parse_tag_70_packet - Parse and process FNEK-encrypted passphrase packet
 * @filename: This function kmalloc's the memory for the filename
 * @filename_size: This function sets this to the amount of memory
 *                 kmalloc'd for the filename
 * @packet_size: This function sets this to the the number of octets
 *               in the packet parsed
 * @mount_crypt_stat: The mount-wide cryptographic context
 * @data: The memory location containing the start of the tag 70
 *        packet
 * @max_packet_size: The maximum legal size of the packet to be parsed
 *                   from @data
 *
 * Returns zero on success; non-zero otherwise
 */
int
ecryptfs_parse_tag_70_packet(char **filename, size_t *filename_size,
			     size_t *packet_size,
			     struct ecryptfs_mount_crypt_stat *mount_crypt_stat,
			     char *data, size_t max_packet_size)
{
	struct ecryptfs_parse_tag_70_packet_silly_stack *s;
	struct key *auth_tok_key = NULL;
	int rc = 0;

	(*packet_size) = 0;
	(*filename_size) = 0;
	(*filename) = NULL;
	s = kzalloc(sizeof(*s), GFP_KERNEL);
	if (!s)
		return -ENOMEM;

	if (max_packet_size < ECRYPTFS_TAG_70_MIN_METADATA_SIZE) {
		printk(KERN_WARNING "%s: max_packet_size is [%zd]; it must be "
		       "at least [%d]\n", __func__, max_packet_size,
		       ECRYPTFS_TAG_70_MIN_METADATA_SIZE);
		rc = -EINVAL;
		goto out;
	}
	/* Octet 0: Tag 70 identifier
	 * Octets 1-N1: Tag 70 packet size (includes cipher identifier
	 *              and block-aligned encrypted filename size)
	 * Octets N1-N2: FNEK sig (ECRYPTFS_SIG_SIZE)
	 * Octet N2-N3: Cipher identifier (1 octet)
	 * Octets N3-N4: Block-aligned encrypted filename
	 *  - Consists of a minimum number of random numbers, a \0
	 *    separator, and then the filename */
	if (data[(*packet_size)++] != ECRYPTFS_TAG_70_PACKET_TYPE) {
		printk(KERN_WARNING "%s: Invalid packet tag [0x%.2x]; must be "
		       "tag [0x%.2x]\n", __func__,
		       data[((*packet_size) - 1)], ECRYPTFS_TAG_70_PACKET_TYPE);
		rc = -EINVAL;
		goto out;
	}
	rc = ecryptfs_parse_packet_length(&data[(*packet_size)],
					  &s->parsed_tag_70_packet_size,
					  &s->packet_size_len);
	if (rc) {
		printk(KERN_WARNING "%s: Error parsing packet length; "
		       "rc = [%d]\n", __func__, rc);
		goto out;
	}
	s->block_aligned_filename_size = (s->parsed_tag_70_packet_size
					  - ECRYPTFS_SIG_SIZE - 1);
	if ((1 + s->packet_size_len + s->parsed_tag_70_packet_size)
	    > max_packet_size) {
		printk(KERN_WARNING "%s: max_packet_size is [%zd]; real packet "
		       "size is [%zd]\n", __func__, max_packet_size,
		       (1 + s->packet_size_len + 1
			+ s->block_aligned_filename_size));
		rc = -EINVAL;
		goto out;
	}
	(*packet_size) += s->packet_size_len;
	ecryptfs_to_hex(s->fnek_sig_hex, &data[(*packet_size)],
			ECRYPTFS_SIG_SIZE);
	s->fnek_sig_hex[ECRYPTFS_SIG_SIZE_HEX] = '\0';
	(*packet_size) += ECRYPTFS_SIG_SIZE;
	s->cipher_code = data[(*packet_size)++];
	rc = ecryptfs_cipher_code_to_string(s->cipher_string, s->cipher_code);
	if (rc) {
		printk(KERN_WARNING "%s: Cipher code [%d] is invalid\n",
		       __func__, s->cipher_code);
		goto out;
	}
	rc = ecryptfs_find_auth_tok_for_sig(&auth_tok_key,
					    &s->auth_tok, mount_crypt_stat,
					    s->fnek_sig_hex);
	if (rc) {
		printk(KERN_ERR "%s: Error attempting to find auth tok for "
		       "fnek sig [%s]; rc = [%d]\n", __func__, s->fnek_sig_hex,
		       rc);
		goto out;
	}
	rc = ecryptfs_get_tfm_and_mutex_for_cipher_name(&s->skcipher_tfm,
							&s->tfm_mutex,
							s->cipher_string);
	if (unlikely(rc)) {
		printk(KERN_ERR "Internal error whilst attempting to get "
		       "tfm and mutex for cipher name [%s]; rc = [%d]\n",
		       s->cipher_string, rc);
		goto out;
	}
	mutex_lock(s->tfm_mutex);
	rc = virt_to_scatterlist(&data[(*packet_size)],
				 s->block_aligned_filename_size, s->src_sg, 2);
	if (rc < 1) {
		printk(KERN_ERR "%s: Internal error whilst attempting to "
		       "convert encrypted filename memory to scatterlist; "
		       "rc = [%d]. block_aligned_filename_size = [%zd]\n",
		       __func__, rc, s->block_aligned_filename_size);
		goto out_unlock;
	}
	(*packet_size) += s->block_aligned_filename_size;
	s->decrypted_filename = kmalloc(s->block_aligned_filename_size,
					GFP_KERNEL);
	if (!s->decrypted_filename) {
		rc = -ENOMEM;
		goto out_unlock;
	}
	rc = virt_to_scatterlist(s->decrypted_filename,
				 s->block_aligned_filename_size, s->dst_sg, 2);
	if (rc < 1) {
		printk(KERN_ERR "%s: Internal error whilst attempting to "
		       "convert decrypted filename memory to scatterlist; "
		       "rc = [%d]. block_aligned_filename_size = [%zd]\n",
		       __func__, rc, s->block_aligned_filename_size);
		goto out_free_unlock;
	}

	s->skcipher_req = skcipher_request_alloc(s->skcipher_tfm, GFP_KERNEL);
	if (!s->skcipher_req) {
		printk(KERN_ERR "%s: Out of kernel memory whilst attempting to "
		       "skcipher_request_alloc for %s\n", __func__,
		       crypto_skcipher_driver_name(s->skcipher_tfm));
		rc = -ENOMEM;
		goto out_free_unlock;
	}

	skcipher_request_set_callback(s->skcipher_req,
				      CRYPTO_TFM_REQ_MAY_SLEEP, NULL, NULL);

	/* The characters in the first block effectively do the job of
	 * the IV here, so we just use 0's for the IV. Note the
	 * constraint that ECRYPTFS_FILENAME_MIN_RANDOM_PREPEND_BYTES
	 * >= ECRYPTFS_MAX_IV_BYTES. */
	/* TODO: Support other key modules than passphrase for
	 * filename encryption */
	if (s->auth_tok->token_type != ECRYPTFS_PASSWORD) {
		rc = -EOPNOTSUPP;
		printk(KERN_INFO "%s: Filename encryption only supports "
		       "password tokens\n", __func__);
		goto out_free_unlock;
	}
	rc = crypto_skcipher_setkey(
		s->skcipher_tfm,
		s->auth_tok->token.password.session_key_encryption_key,
		mount_crypt_stat->global_default_fn_cipher_key_bytes);
	if (rc < 0) {
		printk(KERN_ERR "%s: Error setting key for crypto context; "
		       "rc = [%d]. s->auth_tok->token.password.session_key_"
		       "encryption_key = [0x%p]; mount_crypt_stat->"
		       "global_default_fn_cipher_key_bytes = [%zd]\n", __func__,
		       rc,
		       s->auth_tok->token.password.session_key_encryption_key,
		       mount_crypt_stat->global_default_fn_cipher_key_bytes);
		goto out_free_unlock;
	}
	skcipher_request_set_crypt(s->skcipher_req, s->src_sg, s->dst_sg,
				   s->block_aligned_filename_size, s->iv);
	rc = crypto_skcipher_decrypt(s->skcipher_req);
	if (rc) {
		printk(KERN_ERR "%s: Error attempting to decrypt filename; "
		       "rc = [%d]\n", __func__, rc);
		goto out_free_unlock;
	}

	while (s->i < s->block_aligned_filename_size &&
	       s->decrypted_filename[s->i] != '\0')
		s->i++;
	if (s->i == s->block_aligned_filename_size) {
		printk(KERN_WARNING "%s: Invalid tag 70 packet; could not "
		       "find valid separator between random characters and "
		       "the filename\n", __func__);
		rc = -EINVAL;
		goto out_free_unlock;
	}
	s->i++;
	(*filename_size) = (s->block_aligned_filename_size - s->i);
	if (!((*filename_size) > 0 && (*filename_size < PATH_MAX))) {
		printk(KERN_WARNING "%s: Filename size is [%zd], which is "
		       "invalid\n", __func__, (*filename_size));
		rc = -EINVAL;
		goto out_free_unlock;
	}
	(*filename) = kmalloc(((*filename_size) + 1), GFP_KERNEL);
	if (!(*filename)) {
		rc = -ENOMEM;
		goto out_free_unlock;
	}
	memcpy((*filename), &s->decrypted_filename[s->i], (*filename_size));
	(*filename)[(*filename_size)] = '\0';
out_free_unlock:
	kfree(s->decrypted_filename);
out_unlock:
	mutex_unlock(s->tfm_mutex);
out:
	if (rc) {
		(*packet_size) = 0;
		(*filename_size) = 0;
		(*filename) = NULL;
	}
	if (auth_tok_key) {
		up_write(&(auth_tok_key->sem));
		key_put(auth_tok_key);
	}
	skcipher_request_free(s->skcipher_req);
	kfree(s);
	return rc;
}

static int
ecryptfs_get_auth_tok_sig(char **sig, struct ecryptfs_auth_tok *auth_tok)
{
	int rc = 0;

	(*sig) = NULL;
	switch (auth_tok->token_type) {
	case ECRYPTFS_PASSWORD:
		(*sig) = auth_tok->token.password.signature;
		break;
	case ECRYPTFS_PRIVATE_KEY:
		(*sig) = auth_tok->token.private_key.signature;
		break;
	default:
		printk(KERN_ERR "Cannot get sig for auth_tok of type [%d]\n",
		       auth_tok->token_type);
		rc = -EINVAL;
	}
	return rc;
}

/**
 * decrypt_pki_encrypted_session_key - Decrypt the session key with the given auth_tok.
 * @auth_tok: The key authentication token used to decrypt the session key
 * @crypt_stat: The cryptographic context
 *
 * Returns zero on success; non-zero error otherwise.
 */
static int
decrypt_pki_encrypted_session_key(struct ecryptfs_auth_tok *auth_tok,
				  struct ecryptfs_crypt_stat *crypt_stat)
{
	u8 cipher_code = 0;
	struct ecryptfs_msg_ctx *msg_ctx;
	struct ecryptfs_message *msg = NULL;
	char *auth_tok_sig;
	char *payload = NULL;
	size_t payload_len = 0;
	int rc;

	rc = ecryptfs_get_auth_tok_sig(&auth_tok_sig, auth_tok);
	if (rc) {
		printk(KERN_ERR "Unrecognized auth tok type: [%d]\n",
		       auth_tok->token_type);
		goto out;
	}
	rc = write_tag_64_packet(auth_tok_sig, &(auth_tok->session_key),
				 &payload, &payload_len);
	if (rc) {
		ecryptfs_printk(KERN_ERR, "Failed to write tag 64 packet\n");
		goto out;
	}
	rc = ecryptfs_send_message(payload, payload_len, &msg_ctx);
	if (rc) {
		ecryptfs_printk(KERN_ERR, "Error sending message to "
				"ecryptfsd: %d\n", rc);
		goto out;
	}
	rc = ecryptfs_wait_for_response(msg_ctx, &msg);
	if (rc) {
		ecryptfs_printk(KERN_ERR, "Failed to receive tag 65 packet "
				"from the user space daemon\n");
		rc = -EIO;
		goto out;
	}
	rc = parse_tag_65_packet(&(auth_tok->session_key),
				 &cipher_code, msg);
	if (rc) {
		printk(KERN_ERR "Failed to parse tag 65 packet; rc = [%d]\n",
		       rc);
		goto out;
	}
	auth_tok->session_key.flags |= ECRYPTFS_CONTAINS_DECRYPTED_KEY;
	memcpy(crypt_stat->key, auth_tok->session_key.decrypted_key,
	       auth_tok->session_key.decrypted_key_size);
	crypt_stat->key_size = auth_tok->session_key.decrypted_key_size;
	rc = ecryptfs_cipher_code_to_string(crypt_stat->cipher, cipher_code);
	if (rc) {
		ecryptfs_printk(KERN_ERR, "Cipher code [%d] is invalid\n",
				cipher_code);
		goto out;
	}
	crypt_stat->flags |= ECRYPTFS_KEY_VALID;
	if (ecryptfs_verbosity > 0) {
		ecryptfs_printk(KERN_DEBUG, "Decrypted session key:\n");
		ecryptfs_dump_hex(crypt_stat->key,
				  crypt_stat->key_size);
	}
out:
	kfree(msg);
	kfree(payload);
	return rc;
}

static void wipe_auth_tok_list(struct list_head *auth_tok_list_head)
{
	struct ecryptfs_auth_tok_list_item *auth_tok_list_item;
	struct ecryptfs_auth_tok_list_item *auth_tok_list_item_tmp;

	list_for_each_entry_safe(auth_tok_list_item, auth_tok_list_item_tmp,
				 auth_tok_list_head, list) {
		list_del(&auth_tok_list_item->list);
		kmem_cache_free(ecryptfs_auth_tok_list_item_cache,
				auth_tok_list_item);
	}
}

struct kmem_cache *ecryptfs_auth_tok_list_item_cache;

/**
 * parse_tag_1_packet
 * @crypt_stat: The cryptographic context to modify based on packet contents
 * @data: The raw bytes of the packet.
 * @auth_tok_list: eCryptfs parses packets into authentication tokens;
 *                 a new authentication token will be placed at the
 *                 end of this list for this packet.
 * @new_auth_tok: Pointer to a pointer to memory that this function
 *                allocates; sets the memory address of the pointer to
 *                NULL on error. This object is added to the
 *                auth_tok_list.
 * @packet_size: This function writes the size of the parsed packet
 *               into this memory location; zero on error.
 * @max_packet_size: The maximum allowable packet size
 *
 * Returns zero on success; non-zero on error.
 */
static int
parse_tag_1_packet(struct ecryptfs_crypt_stat *crypt_stat,
		   unsigned char *data, struct list_head *auth_tok_list,
		   struct ecryptfs_auth_tok **new_auth_tok,
		   size_t *packet_size, size_t max_packet_size)
{
	size_t body_size;
	struct ecryptfs_auth_tok_list_item *auth_tok_list_item;
	size_t length_size;
	int rc = 0;

	(*packet_size) = 0;
	(*new_auth_tok) = NULL;
	/**
	 * This format is inspired by OpenPGP; see RFC 2440
	 * packet tag 1
	 *
	 * Tag 1 identifier (1 byte)
	 * Max Tag 1 packet size (max 3 bytes)
	 * Version (1 byte)
	 * Key identifier (8 bytes; ECRYPTFS_SIG_SIZE)
	 * Cipher identifier (1 byte)
	 * Encrypted key size (arbitrary)
	 *
	 * 12 bytes minimum packet size
	 */
	if (unlikely(max_packet_size < 12)) {
		printk(KERN_ERR "Invalid max packet size; must be >=12\n");
		rc = -EINVAL;
		goto out;
	}
	if (data[(*packet_size)++] != ECRYPTFS_TAG_1_PACKET_TYPE) {
		printk(KERN_ERR "Enter w/ first byte != 0x%.2x\n",
		       ECRYPTFS_TAG_1_PACKET_TYPE);
		rc = -EINVAL;
		goto out;
	}
	/* Released: wipe_auth_tok_list called in ecryptfs_parse_packet_set or
	 * at end of function upon failure */
	auth_tok_list_item =
		kmem_cache_zalloc(ecryptfs_auth_tok_list_item_cache,
				  GFP_KERNEL);
	if (!auth_tok_list_item) {
		printk(KERN_ERR "Unable to allocate memory\n");
		rc = -ENOMEM;
		goto out;
	}
	(*new_auth_tok) = &auth_tok_list_item->auth_tok;
	rc = ecryptfs_parse_packet_length(&data[(*packet_size)], &body_size,
					  &length_size);
	if (rc) {
		printk(KERN_WARNING "Error parsing packet length; "
		       "rc = [%d]\n", rc);
		goto out_free;
	}
	if (unlikely(body_size < (ECRYPTFS_SIG_SIZE + 2))) {
		printk(KERN_WARNING "Invalid body size ([%td])\n", body_size);
		rc = -EINVAL;
		goto out_free;
	}
	(*packet_size) += length_size;
	if (unlikely((*packet_size) + body_size > max_packet_size)) {
		printk(KERN_WARNING "Packet size exceeds max\n");
		rc = -EINVAL;
		goto out_free;
	}
	if (unlikely(data[(*packet_size)++] != 0x03)) {
		printk(KERN_WARNING "Unknown version number [%d]\n",
		       data[(*packet_size) - 1]);
		rc = -EINVAL;
		goto out_free;
	}
	ecryptfs_to_hex((*new_auth_tok)->token.private_key.signature,
			&data[(*packet_size)], ECRYPTFS_SIG_SIZE);
	*packet_size += ECRYPTFS_SIG_SIZE;
	/* This byte is skipped because the kernel does not need to
	 * know which public key encryption algorithm was used */
	(*packet_size)++;
	(*new_auth_tok)->session_key.encrypted_key_size =
		body_size - (ECRYPTFS_SIG_SIZE + 2);
	if ((*new_auth_tok)->session_key.encrypted_key_size
	    > ECRYPTFS_MAX_ENCRYPTED_KEY_BYTES) {
		printk(KERN_WARNING "Tag 1 packet contains key larger "
		       "than ECRYPTFS_MAX_ENCRYPTED_KEY_BYTES\n");
		rc = -EINVAL;
		goto out_free;
	}
	memcpy((*new_auth_tok)->session_key.encrypted_key,
	       &data[(*packet_size)], (body_size - (ECRYPTFS_SIG_SIZE + 2)));
	(*packet_size) += (*new_auth_tok)->session_key.encrypted_key_size;
	(*new_auth_tok)->session_key.flags &=
		~ECRYPTFS_CONTAINS_DECRYPTED_KEY;
	(*new_auth_tok)->session_key.flags |=
		ECRYPTFS_CONTAINS_ENCRYPTED_KEY;
	(*new_auth_tok)->token_type = ECRYPTFS_PRIVATE_KEY;
	(*new_auth_tok)->flags = 0;
	(*new_auth_tok)->session_key.flags &=
		~(ECRYPTFS_USERSPACE_SHOULD_TRY_TO_DECRYPT);
	(*new_auth_tok)->session_key.flags &=
		~(ECRYPTFS_USERSPACE_SHOULD_TRY_TO_ENCRYPT);
	list_add(&auth_tok_list_item->list, auth_tok_list);
	goto out;
out_free:
	(*new_auth_tok) = NULL;
	memset(auth_tok_list_item, 0,
	       sizeof(struct ecryptfs_auth_tok_list_item));
	kmem_cache_free(ecryptfs_auth_tok_list_item_cache,
			auth_tok_list_item);
out:
	if (rc)
		(*packet_size) = 0;
	return rc;
}

/**
 * parse_tag_3_packet
 * @crypt_stat: The cryptographic context to modify based on packet
 *              contents.
 * @data: The raw bytes of the packet.
 * @auth_tok_list: eCryptfs parses packets into authentication tokens;
 *                 a new authentication token will be placed at the end
 *                 of this list for this packet.
 * @new_auth_tok: Pointer to a pointer to memory that this function
 *                allocates; sets the memory address of the pointer to
 *                NULL on error. This object is added to the
 *                auth_tok_list.
 * @packet_size: This function writes the size of the parsed packet
 *               into this memory location; zero on error.
 * @max_packet_size: maximum number of bytes to parse
 *
 * Returns zero on success; non-zero on error.
 */
static int
parse_tag_3_packet(struct ecryptfs_crypt_stat *crypt_stat,
		   unsigned char *data, struct list_head *auth_tok_list,
		   struct ecryptfs_auth_tok **new_auth_tok,
		   size_t *packet_size, size_t max_packet_size)
{
	size_t body_size;
	struct ecryptfs_auth_tok_list_item *auth_tok_list_item;
	size_t length_size;
	int rc = 0;

	(*packet_size) = 0;
	(*new_auth_tok) = NULL;
	/**
	 *This format is inspired by OpenPGP; see RFC 2440
	 * packet tag 3
	 *
	 * Tag 3 identifier (1 byte)
	 * Max Tag 3 packet size (max 3 bytes)
	 * Version (1 byte)
	 * Cipher code (1 byte)
	 * S2K specifier (1 byte)
	 * Hash identifier (1 byte)
	 * Salt (ECRYPTFS_SALT_SIZE)
	 * Hash iterations (1 byte)
	 * Encrypted key (arbitrary)
	 *
	 * (ECRYPTFS_SALT_SIZE + 7) minimum packet size
	 */
	if (max_packet_size < (ECRYPTFS_SALT_SIZE + 7)) {
		printk(KERN_ERR "Max packet size too large\n");
		rc = -EINVAL;
		goto out;
	}
	if (data[(*packet_size)++] != ECRYPTFS_TAG_3_PACKET_TYPE) {
		printk(KERN_ERR "First byte != 0x%.2x; invalid packet\n",
		       ECRYPTFS_TAG_3_PACKET_TYPE);
		rc = -EINVAL;
		goto out;
	}
	/* Released: wipe_auth_tok_list called in ecryptfs_parse_packet_set or
	 * at end of function upon failure */
	auth_tok_list_item =
	    kmem_cache_zalloc(ecryptfs_auth_tok_list_item_cache, GFP_KERNEL);
	if (!auth_tok_list_item) {
		printk(KERN_ERR "Unable to allocate memory\n");
		rc = -ENOMEM;
		goto out;
	}
	(*new_auth_tok) = &auth_tok_list_item->auth_tok;
	rc = ecryptfs_parse_packet_length(&data[(*packet_size)], &body_size,
					  &length_size);
	if (rc) {
		printk(KERN_WARNING "Error parsing packet length; rc = [%d]\n",
		       rc);
		goto out_free;
	}
	if (unlikely(body_size < (ECRYPTFS_SALT_SIZE + 5))) {
		printk(KERN_WARNING "Invalid body size ([%td])\n", body_size);
		rc = -EINVAL;
		goto out_free;
	}
	(*packet_size) += length_size;
	if (unlikely((*packet_size) + body_size > max_packet_size)) {
		printk(KERN_ERR "Packet size exceeds max\n");
		rc = -EINVAL;
		goto out_free;
	}
	(*new_auth_tok)->session_key.encrypted_key_size =
		(body_size - (ECRYPTFS_SALT_SIZE + 5));
	if ((*new_auth_tok)->session_key.encrypted_key_size
	    > ECRYPTFS_MAX_ENCRYPTED_KEY_BYTES) {
		printk(KERN_WARNING "Tag 3 packet contains key larger "
		       "than ECRYPTFS_MAX_ENCRYPTED_KEY_BYTES\n");
		rc = -EINVAL;
		goto out_free;
	}
	if (unlikely(data[(*packet_size)++] != 0x04)) {
		printk(KERN_WARNING "Unknown version number [%d]\n",
		       data[(*packet_size) - 1]);
		rc = -EINVAL;
		goto out_free;
	}
	rc = ecryptfs_cipher_code_to_string(crypt_stat->cipher,
					    (u16)data[(*packet_size)]);
	if (rc)
		goto out_free;
	/* A little extra work to differentiate among the AES key
	 * sizes; see RFC2440 */
	switch(data[(*packet_size)++]) {
	case RFC2440_CIPHER_AES_192:
		crypt_stat->key_size = 24;
		break;
	default:
		crypt_stat->key_size =
			(*new_auth_tok)->session_key.encrypted_key_size;
	}
	rc = ecryptfs_init_crypt_ctx(crypt_stat);
	if (rc)
		goto out_free;
	if (unlikely(data[(*packet_size)++] != 0x03)) {
		printk(KERN_WARNING "Only S2K ID 3 is currently supported\n");
		rc = -ENOSYS;
		goto out_free;
	}
	/* TODO: finish the hash mapping */
	switch (data[(*packet_size)++]) {
	case 0x01: /* See RFC2440 for these numbers and their mappings */
		/* Choose MD5 */
		memcpy((*new_auth_tok)->token.password.salt,
		       &data[(*packet_size)], ECRYPTFS_SALT_SIZE);
		(*packet_size) += ECRYPTFS_SALT_SIZE;
		/* This conversion was taken straight from RFC2440 */
		(*new_auth_tok)->token.password.hash_iterations =
			((u32) 16 + (data[(*packet_size)] & 15))
				<< ((data[(*packet_size)] >> 4) + 6);
		(*packet_size)++;
		/* Friendly reminder:
		 * (*new_auth_tok)->session_key.encrypted_key_size =
		 *         (body_size - (ECRYPTFS_SALT_SIZE + 5)); */
		memcpy((*new_auth_tok)->session_key.encrypted_key,
		       &data[(*packet_size)],
		       (*new_auth_tok)->session_key.encrypted_key_size);
		(*packet_size) +=
			(*new_auth_tok)->session_key.encrypted_key_size;
		(*new_auth_tok)->session_key.flags &=
			~ECRYPTFS_CONTAINS_DECRYPTED_KEY;
		(*new_auth_tok)->session_key.flags |=
			ECRYPTFS_CONTAINS_ENCRYPTED_KEY;
		(*new_auth_tok)->token.password.hash_algo = 0x01; /* MD5 */
		break;
	default:
		ecryptfs_printk(KERN_ERR, "Unsupported hash algorithm: "
				"[%d]\n", data[(*packet_size) - 1]);
		rc = -ENOSYS;
		goto out_free;
	}
	(*new_auth_tok)->token_type = ECRYPTFS_PASSWORD;
	/* TODO: Parametarize; we might actually want userspace to
	 * decrypt the session key. */
	(*new_auth_tok)->session_key.flags &=
			    ~(ECRYPTFS_USERSPACE_SHOULD_TRY_TO_DECRYPT);
	(*new_auth_tok)->session_key.flags &=
			    ~(ECRYPTFS_USERSPACE_SHOULD_TRY_TO_ENCRYPT);
	list_add(&auth_tok_list_item->list, auth_tok_list);
	goto out;
out_free:
	(*new_auth_tok) = NULL;
	memset(auth_tok_list_item, 0,
	       sizeof(struct ecryptfs_auth_tok_list_item));
	kmem_cache_free(ecryptfs_auth_tok_list_item_cache,
			auth_tok_list_item);
out:
	if (rc)
		(*packet_size) = 0;
	return rc;
}

/**
 * parse_tag_11_packet
 * @data: The raw bytes of the packet
 * @contents: This function writes the data contents of the literal
 *            packet into this memory location
 * @max_contents_bytes: The maximum number of bytes that this function
 *                      is allowed to write into contents
 * @tag_11_contents_size: This function writes the size of the parsed
 *                        contents into this memory location; zero on
 *                        error
 * @packet_size: This function writes the size of the parsed packet
 *               into this memory location; zero on error
 * @max_packet_size: maximum number of bytes to parse
 *
 * Returns zero on success; non-zero on error.
 */
static int
parse_tag_11_packet(unsigned char *data, unsigned char *contents,
		    size_t max_contents_bytes, size_t *tag_11_contents_size,
		    size_t *packet_size, size_t max_packet_size)
{
	size_t body_size;
	size_t length_size;
	int rc = 0;

	(*packet_size) = 0;
	(*tag_11_contents_size) = 0;
	/* This format is inspired by OpenPGP; see RFC 2440
	 * packet tag 11
	 *
	 * Tag 11 identifier (1 byte)
	 * Max Tag 11 packet size (max 3 bytes)
	 * Binary format specifier (1 byte)
	 * Filename length (1 byte)
	 * Filename ("_CONSOLE") (8 bytes)
	 * Modification date (4 bytes)
	 * Literal data (arbitrary)
	 *
	 * We need at least 16 bytes of data for the packet to even be
	 * valid.
	 */
	if (max_packet_size < 16) {
		printk(KERN_ERR "Maximum packet size too small\n");
		rc = -EINVAL;
		goto out;
	}
	if (data[(*packet_size)++] != ECRYPTFS_TAG_11_PACKET_TYPE) {
		printk(KERN_WARNING "Invalid tag 11 packet format\n");
		rc = -EINVAL;
		goto out;
	}
	rc = ecryptfs_parse_packet_length(&data[(*packet_size)], &body_size,
					  &length_size);
	if (rc) {
		printk(KERN_WARNING "Invalid tag 11 packet format\n");
		goto out;
	}
	if (body_size < 14) {
		printk(KERN_WARNING "Invalid body size ([%td])\n", body_size);
		rc = -EINVAL;
		goto out;
	}
	(*packet_size) += length_size;
	(*tag_11_contents_size) = (body_size - 14);
	if (unlikely((*packet_size) + body_size + 1 > max_packet_size)) {
		printk(KERN_ERR "Packet size exceeds max\n");
		rc = -EINVAL;
		goto out;
	}
	if (unlikely((*tag_11_contents_size) > max_contents_bytes)) {
		printk(KERN_ERR "Literal data section in tag 11 packet exceeds "
		       "expected size\n");
		rc = -EINVAL;
		goto out;
	}
	if (data[(*packet_size)++] != 0x62) {
		printk(KERN_WARNING "Unrecognizable packet\n");
		rc = -EINVAL;
		goto out;
	}
	if (data[(*packet_size)++] != 0x08) {
		printk(KERN_WARNING "Unrecognizable packet\n");
		rc = -EINVAL;
		goto out;
	}
	(*packet_size) += 12; /* Ignore filename and modification date */
	memcpy(contents, &data[(*packet_size)], (*tag_11_contents_size));
	(*packet_size) += (*tag_11_contents_size);
out:
	if (rc) {
		(*packet_size) = 0;
		(*tag_11_contents_size) = 0;
	}
	return rc;
}

int ecryptfs_keyring_auth_tok_for_sig(struct key **auth_tok_key,
				      struct ecryptfs_auth_tok **auth_tok,
				      char *sig)
{
	int rc = 0;

	(*auth_tok_key) = request_key(&key_type_user, sig, NULL);
	if (IS_ERR(*auth_tok_key)) {
		(*auth_tok_key) = ecryptfs_get_encrypted_key(sig);
		if (IS_ERR(*auth_tok_key)) {
			printk(KERN_ERR "Could not find key with description: [%s]\n",
			      sig);
			rc = process_request_key_err(PTR_ERR(*auth_tok_key));
			(*auth_tok_key) = NULL;
			goto out;
		}
	}
	down_write(&(*auth_tok_key)->sem);
	rc = ecryptfs_verify_auth_tok_from_key(*auth_tok_key, auth_tok);
	if (rc) {
		up_write(&(*auth_tok_key)->sem);
		key_put(*auth_tok_key);
		(*auth_tok_key) = NULL;
		goto out;
	}
out:
	return rc;
}

/**
 * decrypt_passphrase_encrypted_session_key - Decrypt the session key with the given auth_tok.
 * @auth_tok: The passphrase authentication token to use to encrypt the FEK
 * @crypt_stat: The cryptographic context
 *
 * Returns zero on success; non-zero error otherwise
 */
static int
decrypt_passphrase_encrypted_session_key(struct ecryptfs_auth_tok *auth_tok,
					 struct ecryptfs_crypt_stat *crypt_stat)
{
	struct scatterlist dst_sg[2];
	struct scatterlist src_sg[2];
	struct mutex *tfm_mutex;
	struct crypto_skcipher *tfm;
	struct skcipher_request *req = NULL;
	int rc = 0;

	if (unlikely(ecryptfs_verbosity > 0)) {
		ecryptfs_printk(
			KERN_DEBUG, "Session key encryption key (size [%d]):\n",
			auth_tok->token.password.session_key_encryption_key_bytes);
		ecryptfs_dump_hex(
			auth_tok->token.password.session_key_encryption_key,
			auth_tok->token.password.session_key_encryption_key_bytes);
	}
	rc = ecryptfs_get_tfm_and_mutex_for_cipher_name(&tfm, &tfm_mutex,
							crypt_stat->cipher);
	if (unlikely(rc)) {
		printk(KERN_ERR "Internal error whilst attempting to get "
		       "tfm and mutex for cipher name [%s]; rc = [%d]\n",
		       crypt_stat->cipher, rc);
		goto out;
	}
	rc = virt_to_scatterlist(auth_tok->session_key.encrypted_key,
				 auth_tok->session_key.encrypted_key_size,
				 src_sg, 2);
	if (rc < 1 || rc > 2) {
		printk(KERN_ERR "Internal error whilst attempting to convert "
			"auth_tok->session_key.encrypted_key to scatterlist; "
			"expected rc = 1; got rc = [%d]. "
		       "auth_tok->session_key.encrypted_key_size = [%d]\n", rc,
			auth_tok->session_key.encrypted_key_size);
		goto out;
	}
	auth_tok->session_key.decrypted_key_size =
		auth_tok->session_key.encrypted_key_size;
	rc = virt_to_scatterlist(auth_tok->session_key.decrypted_key,
				 auth_tok->session_key.decrypted_key_size,
				 dst_sg, 2);
	if (rc < 1 || rc > 2) {
		printk(KERN_ERR "Internal error whilst attempting to convert "
			"auth_tok->session_key.decrypted_key to scatterlist; "
			"expected rc = 1; got rc = [%d]\n", rc);
		goto out;
	}
	mutex_lock(tfm_mutex);
	req = skcipher_request_alloc(tfm, GFP_KERNEL);
	if (!req) {
		mutex_unlock(tfm_mutex);
		printk(KERN_ERR "%s: Out of kernel memory whilst attempting to "
		       "skcipher_request_alloc for %s\n", __func__,
		       crypto_skcipher_driver_name(tfm));
		rc = -ENOMEM;
		goto out;
	}

	skcipher_request_set_callback(req, CRYPTO_TFM_REQ_MAY_SLEEP,
				      NULL, NULL);
	rc = crypto_skcipher_setkey(
		tfm, auth_tok->token.password.session_key_encryption_key,
		crypt_stat->key_size);
	if (unlikely(rc < 0)) {
		mutex_unlock(tfm_mutex);
		printk(KERN_ERR "Error setting key for crypto context\n");
		rc = -EINVAL;
		goto out;
	}
	skcipher_request_set_crypt(req, src_sg, dst_sg,
				   auth_tok->session_key.encrypted_key_size,
				   NULL);
	rc = crypto_skcipher_decrypt(req);
	mutex_unlock(tfm_mutex);
	if (unlikely(rc)) {
		printk(KERN_ERR "Error decrypting; rc = [%d]\n", rc);
		goto out;
	}
	auth_tok->session_key.flags |= ECRYPTFS_CONTAINS_DECRYPTED_KEY;
	memcpy(crypt_stat->key, auth_tok->session_key.decrypted_key,
	       auth_tok->session_key.decrypted_key_size);
	crypt_stat->flags |= ECRYPTFS_KEY_VALID;
	if (unlikely(ecryptfs_verbosity > 0)) {
		ecryptfs_printk(KERN_DEBUG, "FEK of size [%zd]:\n",
				crypt_stat->key_size);
		ecryptfs_dump_hex(crypt_stat->key,
				  crypt_stat->key_size);
	}
out:
	skcipher_request_free(req);
	return rc;
}

/**
 * ecryptfs_parse_packet_set
 * @crypt_stat: The cryptographic context
 * @src: Virtual address of region of memory containing the packets
 * @ecryptfs_dentry: The eCryptfs dentry associated with the packet set
 *
 * Get crypt_stat to have the file's session key if the requisite key
 * is available to decrypt the session key.
 *
 * Returns Zero if a valid authentication token was retrieved and
 * processed; negative value for file not encrypted or for error
 * conditions.
 */
int ecryptfs_parse_packet_set(struct ecryptfs_crypt_stat *crypt_stat,
			      unsigned char *src,
			      struct dentry *ecryptfs_dentry)
{
	size_t i = 0;
	size_t found_auth_tok;
	size_t next_packet_is_auth_tok_packet;
	struct list_head auth_tok_list;
	struct ecryptfs_auth_tok *matching_auth_tok;
	struct ecryptfs_auth_tok *candidate_auth_tok;
	char *candidate_auth_tok_sig;
	size_t packet_size;
	struct ecryptfs_auth_tok *new_auth_tok;
	unsigned char sig_tmp_space[ECRYPTFS_SIG_SIZE];
	struct ecryptfs_auth_tok_list_item *auth_tok_list_item;
	size_t tag_11_contents_size;
	size_t tag_11_packet_size;
	struct key *auth_tok_key = NULL;
	int rc = 0;

	INIT_LIST_HEAD(&auth_tok_list);
	/* Parse the header to find as many packets as we can; these will be
	 * added the our &auth_tok_list */
	next_packet_is_auth_tok_packet = 1;
	while (next_packet_is_auth_tok_packet) {
		size_t max_packet_size = ((PAGE_SIZE - 8) - i);

		switch (src[i]) {
		case ECRYPTFS_TAG_3_PACKET_TYPE:
			rc = parse_tag_3_packet(crypt_stat,
						(unsigned char *)&src[i],
						&auth_tok_list, &new_auth_tok,
						&packet_size, max_packet_size);
			if (rc) {
				ecryptfs_printk(KERN_ERR, "Error parsing "
						"tag 3 packet\n");
				rc = -EIO;
				goto out_wipe_list;
			}
			i += packet_size;
			rc = parse_tag_11_packet((unsigned char *)&src[i],
						 sig_tmp_space,
						 ECRYPTFS_SIG_SIZE,
						 &tag_11_contents_size,
						 &tag_11_packet_size,
						 max_packet_size);
			if (rc) {
				ecryptfs_printk(KERN_ERR, "No valid "
						"(ecryptfs-specific) literal "
						"packet containing "
						"authentication token "
						"signature found after "
						"tag 3 packet\n");
				rc = -EIO;
				goto out_wipe_list;
			}
			i += tag_11_packet_size;
			if (ECRYPTFS_SIG_SIZE != tag_11_contents_size) {
				ecryptfs_printk(KERN_ERR, "Expected "
						"signature of size [%d]; "
						"read size [%zd]\n",
						ECRYPTFS_SIG_SIZE,
						tag_11_contents_size);
				rc = -EIO;
				goto out_wipe_list;
			}
			ecryptfs_to_hex(new_auth_tok->token.password.signature,
					sig_tmp_space, tag_11_contents_size);
			new_auth_tok->token.password.signature[
				ECRYPTFS_PASSWORD_SIG_SIZE] = '\0';
			crypt_stat->flags |= ECRYPTFS_ENCRYPTED;
			break;
		case ECRYPTFS_TAG_1_PACKET_TYPE:
			rc = parse_tag_1_packet(crypt_stat,
						(unsigned char *)&src[i],
						&auth_tok_list, &new_auth_tok,
						&packet_size, max_packet_size);
			if (rc) {
				ecryptfs_printk(KERN_ERR, "Error parsing "
						"tag 1 packet\n");
				rc = -EIO;
				goto out_wipe_list;
			}
			i += packet_size;
			crypt_stat->flags |= ECRYPTFS_ENCRYPTED;
			break;
		case ECRYPTFS_TAG_11_PACKET_TYPE:
			ecryptfs_printk(KERN_WARNING, "Invalid packet set "
					"(Tag 11 not allowed by itself)\n");
			rc = -EIO;
			goto out_wipe_list;
		default:
			ecryptfs_printk(KERN_DEBUG, "No packet at offset [%zd] "
					"of the file header; hex value of "
					"character is [0x%.2x]\n", i, src[i]);
			next_packet_is_auth_tok_packet = 0;
		}
	}
	if (list_empty(&auth_tok_list)) {
		printk(KERN_ERR "The lower file appears to be a non-encrypted "
		       "eCryptfs file; this is not supported in this version "
		       "of the eCryptfs kernel module\n");
		rc = -EINVAL;
		goto out;
	}
	/* auth_tok_list contains the set of authentication tokens
	 * parsed from the metadata. We need to find a matching
	 * authentication token that has the secret component(s)
	 * necessary to decrypt the EFEK in the auth_tok parsed from
	 * the metadata. There may be several potential matches, but
	 * just one will be sufficient to decrypt to get the FEK. */
find_next_matching_auth_tok:
	found_auth_tok = 0;
	list_for_each_entry(auth_tok_list_item, &auth_tok_list, list) {
		candidate_auth_tok = &auth_tok_list_item->auth_tok;
		if (unlikely(ecryptfs_verbosity > 0)) {
			ecryptfs_printk(KERN_DEBUG,
					"Considering candidate auth tok:\n");
			ecryptfs_dump_auth_tok(candidate_auth_tok);
		}
		rc = ecryptfs_get_auth_tok_sig(&candidate_auth_tok_sig,
					       candidate_auth_tok);
		if (rc) {
			printk(KERN_ERR
			       "Unrecognized candidate auth tok type: [%d]\n",
			       candidate_auth_tok->token_type);
			rc = -EINVAL;
			goto out_wipe_list;
		}
		rc = ecryptfs_find_auth_tok_for_sig(&auth_tok_key,
					       &matching_auth_tok,
					       crypt_stat->mount_crypt_stat,
					       candidate_auth_tok_sig);
		if (!rc) {
			found_auth_tok = 1;
			goto found_matching_auth_tok;
		}
	}
	if (!found_auth_tok) {
		ecryptfs_printk(KERN_ERR, "Could not find a usable "
				"authentication token\n");
		rc = -EIO;
		goto out_wipe_list;
	}
found_matching_auth_tok:
	if (candidate_auth_tok->token_type == ECRYPTFS_PRIVATE_KEY) {
		memcpy(&(candidate_auth_tok->token.private_key),
		       &(matching_auth_tok->token.private_key),
		       sizeof(struct ecryptfs_private_key));
		up_write(&(auth_tok_key->sem));
		key_put(auth_tok_key);
		rc = decrypt_pki_encrypted_session_key(candidate_auth_tok,
						       crypt_stat);
	} else if (candidate_auth_tok->token_type == ECRYPTFS_PASSWORD) {
		memcpy(&(candidate_auth_tok->token.password),
		       &(matching_auth_tok->token.password),
		       sizeof(struct ecryptfs_password));
		up_write(&(auth_tok_key->sem));
		key_put(auth_tok_key);
		rc = decrypt_passphrase_encrypted_session_key(
			candidate_auth_tok, crypt_stat);
	} else {
		up_write(&(auth_tok_key->sem));
		key_put(auth_tok_key);
		rc = -EINVAL;
	}
	if (rc) {
		struct ecryptfs_auth_tok_list_item *auth_tok_list_item_tmp;

		ecryptfs_printk(KERN_WARNING, "Error decrypting the "
				"session key for authentication token with sig "
				"[%.*s]; rc = [%d]. Removing auth tok "
				"candidate from the list and searching for "
				"the next match.\n", ECRYPTFS_SIG_SIZE_HEX,
				candidate_auth_tok_sig,	rc);
		list_for_each_entry_safe(auth_tok_list_item,
					 auth_tok_list_item_tmp,
					 &auth_tok_list, list) {
			if (candidate_auth_tok
			    == &auth_tok_list_item->auth_tok) {
				list_del(&auth_tok_list_item->list);
				kmem_cache_free(
					ecryptfs_auth_tok_list_item_cache,
					auth_tok_list_item);
				goto find_next_matching_auth_tok;
			}
		}
		BUG();
	}
	rc = ecryptfs_compute_root_iv(crypt_stat);
	if (rc) {
		ecryptfs_printk(KERN_ERR, "Error computing "
				"the root IV\n");
		goto out_wipe_list;
	}
	rc = ecryptfs_init_crypt_ctx(crypt_stat);
	if (rc) {
		ecryptfs_printk(KERN_ERR, "Error initializing crypto "
				"context for cipher [%s]; rc = [%d]\n",
				crypt_stat->cipher, rc);
	}
out_wipe_list:
	wipe_auth_tok_list(&auth_tok_list);
out:
	return rc;
}

static int
pki_encrypt_session_key(struct key *auth_tok_key,
			struct ecryptfs_auth_tok *auth_tok,
			struct ecryptfs_crypt_stat *crypt_stat,
			struct ecryptfs_key_record *key_rec)
{
	struct ecryptfs_msg_ctx *msg_ctx = NULL;
	char *payload = NULL;
	size_t payload_len = 0;
	struct ecryptfs_message *msg;
	int rc;

	rc = write_tag_66_packet(auth_tok->token.private_key.signature,
				 ecryptfs_code_for_cipher_string(
					 crypt_stat->cipher,
					 crypt_stat->key_size),
				 crypt_stat, &payload, &payload_len);
	up_write(&(auth_tok_key->sem));
	key_put(auth_tok_key);
	if (rc) {
		ecryptfs_printk(KERN_ERR, "Error generating tag 66 packet\n");
		goto out;
	}
	rc = ecryptfs_send_message(payload, payload_len, &msg_ctx);
	if (rc) {
		ecryptfs_printk(KERN_ERR, "Error sending message to "
				"ecryptfsd: %d\n", rc);
		goto out;
	}
	rc = ecryptfs_wait_for_response(msg_ctx, &msg);
	if (rc) {
		ecryptfs_printk(KERN_ERR, "Failed to receive tag 67 packet "
				"from the user space daemon\n");
		rc = -EIO;
		goto out;
	}
	rc = parse_tag_67_packet(key_rec, msg);
	if (rc)
		ecryptfs_printk(KERN_ERR, "Error parsing tag 67 packet\n");
	kfree(msg);
out:
	kfree(payload);
	return rc;
}
/**
 * write_tag_1_packet - Write an RFC2440-compatible tag 1 (public key) packet
 * @dest: Buffer into which to write the packet
 * @remaining_bytes: Maximum number of bytes that can be writtn
 * @auth_tok_key: The authentication token key to unlock and put when done with
 *                @auth_tok
 * @auth_tok: The authentication token used for generating the tag 1 packet
 * @crypt_stat: The cryptographic context
 * @key_rec: The key record struct for the tag 1 packet
 * @packet_size: This function will write the number of bytes that end
 *               up constituting the packet; set to zero on error
 *
 * Returns zero on success; non-zero on error.
 */
static int
write_tag_1_packet(char *dest, size_t *remaining_bytes,
		   struct key *auth_tok_key, struct ecryptfs_auth_tok *auth_tok,
		   struct ecryptfs_crypt_stat *crypt_stat,
		   struct ecryptfs_key_record *key_rec, size_t *packet_size)
{
	size_t i;
	size_t encrypted_session_key_valid = 0;
	size_t packet_size_length;
	size_t max_packet_size;
	int rc = 0;

	(*packet_size) = 0;
	ecryptfs_from_hex(key_rec->sig, auth_tok->token.private_key.signature,
			  ECRYPTFS_SIG_SIZE);
	encrypted_session_key_valid = 0;
	for (i = 0; i < crypt_stat->key_size; i++)
		encrypted_session_key_valid |=
			auth_tok->session_key.encrypted_key[i];
	if (encrypted_session_key_valid) {
		memcpy(key_rec->enc_key,
		       auth_tok->session_key.encrypted_key,
		       auth_tok->session_key.encrypted_key_size);
		up_write(&(auth_tok_key->sem));
		key_put(auth_tok_key);
		goto encrypted_session_key_set;
	}
	if (auth_tok->session_key.encrypted_key_size == 0)
		auth_tok->session_key.encrypted_key_size =
			auth_tok->token.private_key.key_size;
	rc = pki_encrypt_session_key(auth_tok_key, auth_tok, crypt_stat,
				     key_rec);
	if (rc) {
		printk(KERN_ERR "Failed to encrypt session key via a key "
		       "module; rc = [%d]\n", rc);
		goto out;
	}
	if (ecryptfs_verbosity > 0) {
		ecryptfs_printk(KERN_DEBUG, "Encrypted key:\n");
		ecryptfs_dump_hex(key_rec->enc_key, key_rec->enc_key_size);
	}
encrypted_session_key_set:
	/* This format is inspired by OpenPGP; see RFC 2440
	 * packet tag 1 */
	max_packet_size = (1                         /* Tag 1 identifier */
			   + 3                       /* Max Tag 1 packet size */
			   + 1                       /* Version */
			   + ECRYPTFS_SIG_SIZE       /* Key identifier */
			   + 1                       /* Cipher identifier */
			   + key_rec->enc_key_size); /* Encrypted key size */
	if (max_packet_size > (*remaining_bytes)) {
		printk(KERN_ERR "Packet length larger than maximum allowable; "
		       "need up to [%td] bytes, but there are only [%td] "
		       "available\n", max_packet_size, (*remaining_bytes));
		rc = -EINVAL;
		goto out;
	}
	dest[(*packet_size)++] = ECRYPTFS_TAG_1_PACKET_TYPE;
	rc = ecryptfs_write_packet_length(&dest[(*packet_size)],
					  (max_packet_size - 4),
					  &packet_size_length);
	if (rc) {
		ecryptfs_printk(KERN_ERR, "Error generating tag 1 packet "
				"header; cannot generate packet length\n");
		goto out;
	}
	(*packet_size) += packet_size_length;
	dest[(*packet_size)++] = 0x03; /* version 3 */
	memcpy(&dest[(*packet_size)], key_rec->sig, ECRYPTFS_SIG_SIZE);
	(*packet_size) += ECRYPTFS_SIG_SIZE;
	dest[(*packet_size)++] = RFC2440_CIPHER_RSA;
	memcpy(&dest[(*packet_size)], key_rec->enc_key,
	       key_rec->enc_key_size);
	(*packet_size) += key_rec->enc_key_size;
out:
	if (rc)
		(*packet_size) = 0;
	else
		(*remaining_bytes) -= (*packet_size);
	return rc;
}

/**
 * write_tag_11_packet
 * @dest: Target into which Tag 11 packet is to be written
 * @remaining_bytes: Maximum packet length
 * @contents: Byte array of contents to copy in
 * @contents_length: Number of bytes in contents
 * @packet_length: Length of the Tag 11 packet written; zero on error
 *
 * Returns zero on success; non-zero on error.
 */
static int
write_tag_11_packet(char *dest, size_t *remaining_bytes, char *contents,
		    size_t contents_length, size_t *packet_length)
{
	size_t packet_size_length;
	size_t max_packet_size;
	int rc = 0;

	(*packet_length) = 0;
	/* This format is inspired by OpenPGP; see RFC 2440
	 * packet tag 11 */
	max_packet_size = (1                   /* Tag 11 identifier */
			   + 3                 /* Max Tag 11 packet size */
			   + 1                 /* Binary format specifier */
			   + 1                 /* Filename length */
			   + 8                 /* Filename ("_CONSOLE") */
			   + 4                 /* Modification date */
			   + contents_length); /* Literal data */
	if (max_packet_size > (*remaining_bytes)) {
		printk(KERN_ERR "Packet length larger than maximum allowable; "
		       "need up to [%td] bytes, but there are only [%td] "
		       "available\n", max_packet_size, (*remaining_bytes));
		rc = -EINVAL;
		goto out;
	}
	dest[(*packet_length)++] = ECRYPTFS_TAG_11_PACKET_TYPE;
	rc = ecryptfs_write_packet_length(&dest[(*packet_length)],
					  (max_packet_size - 4),
					  &packet_size_length);
	if (rc) {
		printk(KERN_ERR "Error generating tag 11 packet header; cannot "
		       "generate packet length. rc = [%d]\n", rc);
		goto out;
	}
	(*packet_length) += packet_size_length;
	dest[(*packet_length)++] = 0x62; /* binary data format specifier */
	dest[(*packet_length)++] = 8;
	memcpy(&dest[(*packet_length)], "_CONSOLE", 8);
	(*packet_length) += 8;
	memset(&dest[(*packet_length)], 0x00, 4);
	(*packet_length) += 4;
	memcpy(&dest[(*packet_length)], contents, contents_length);
	(*packet_length) += contents_length;
 out:
	if (rc)
		(*packet_length) = 0;
	else
		(*remaining_bytes) -= (*packet_length);
	return rc;
}

/**
 * write_tag_3_packet
 * @dest: Buffer into which to write the packet
 * @remaining_bytes: Maximum number of bytes that can be written
 * @auth_tok: Authentication token
 * @crypt_stat: The cryptographic context
 * @key_rec: encrypted key
 * @packet_size: This function will write the number of bytes that end
 *               up constituting the packet; set to zero on error
 *
 * Returns zero on success; non-zero on error.
 */
static int
write_tag_3_packet(char *dest, size_t *remaining_bytes,
		   struct ecryptfs_auth_tok *auth_tok,
		   struct ecryptfs_crypt_stat *crypt_stat,
		   struct ecryptfs_key_record *key_rec, size_t *packet_size)
{
	size_t i;
	size_t encrypted_session_key_valid = 0;
	char session_key_encryption_key[ECRYPTFS_MAX_KEY_BYTES];
	struct scatterlist dst_sg[2];
	struct scatterlist src_sg[2];
	struct mutex *tfm_mutex = NULL;
	u8 cipher_code;
	size_t packet_size_length;
	size_t max_packet_size;
	struct ecryptfs_mount_crypt_stat *mount_crypt_stat =
		crypt_stat->mount_crypt_stat;
	struct crypto_skcipher *tfm;
	struct skcipher_request *req;
	int rc = 0;

	(*packet_size) = 0;
	ecryptfs_from_hex(key_rec->sig, auth_tok->token.password.signature,
			  ECRYPTFS_SIG_SIZE);
	rc = ecryptfs_get_tfm_and_mutex_for_cipher_name(&tfm, &tfm_mutex,
							crypt_stat->cipher);
	if (unlikely(rc)) {
		printk(KERN_ERR "Internal error whilst attempting to get "
		       "tfm and mutex for cipher name [%s]; rc = [%d]\n",
		       crypt_stat->cipher, rc);
		goto out;
	}
	if (mount_crypt_stat->global_default_cipher_key_size == 0) {
		printk(KERN_WARNING "No key size specified at mount; "
		       "defaulting to [%d]\n",
		       crypto_skcipher_max_keysize(tfm));
		mount_crypt_stat->global_default_cipher_key_size =
			crypto_skcipher_max_keysize(tfm);
	}
	if (crypt_stat->key_size == 0)
		crypt_stat->key_size =
			mount_crypt_stat->global_default_cipher_key_size;
	if (auth_tok->session_key.encrypted_key_size == 0)
		auth_tok->session_key.encrypted_key_size =
			crypt_stat->key_size;
	if (crypt_stat->key_size == 24
	    && strcmp("aes", crypt_stat->cipher) == 0) {
		memset((crypt_stat->key + 24), 0, 8);
		auth_tok->session_key.encrypted_key_size = 32;
	} else
		auth_tok->session_key.encrypted_key_size = crypt_stat->key_size;
	key_rec->enc_key_size =
		auth_tok->session_key.encrypted_key_size;
	encrypted_session_key_valid = 0;
	for (i = 0; i < auth_tok->session_key.encrypted_key_size; i++)
		encrypted_session_key_valid |=
			auth_tok->session_key.encrypted_key[i];
	if (encrypted_session_key_valid) {
		ecryptfs_printk(KERN_DEBUG, "encrypted_session_key_valid != 0; "
				"using auth_tok->session_key.encrypted_key, "
				"where key_rec->enc_key_size = [%zd]\n",
				key_rec->enc_key_size);
		memcpy(key_rec->enc_key,
		       auth_tok->session_key.encrypted_key,
		       key_rec->enc_key_size);
		goto encrypted_session_key_set;
	}
	if (auth_tok->token.password.flags &
	    ECRYPTFS_SESSION_KEY_ENCRYPTION_KEY_SET) {
		ecryptfs_printk(KERN_DEBUG, "Using previously generated "
				"session key encryption key of size [%d]\n",
				auth_tok->token.password.
				session_key_encryption_key_bytes);
		memcpy(session_key_encryption_key,
		       auth_tok->token.password.session_key_encryption_key,
		       crypt_stat->key_size);
		ecryptfs_printk(KERN_DEBUG,
				"Cached session key encryption key:\n");
		if (ecryptfs_verbosity > 0)
			ecryptfs_dump_hex(session_key_encryption_key, 16);
	}
	if (unlikely(ecryptfs_verbosity > 0)) {
		ecryptfs_printk(KERN_DEBUG, "Session key encryption key:\n");
		ecryptfs_dump_hex(session_key_encryption_key, 16);
	}
	rc = virt_to_scatterlist(crypt_stat->key, key_rec->enc_key_size,
				 src_sg, 2);
	if (rc < 1 || rc > 2) {
		ecryptfs_printk(KERN_ERR, "Error generating scatterlist "
				"for crypt_stat session key; expected rc = 1; "
				"got rc = [%d]. key_rec->enc_key_size = [%zd]\n",
				rc, key_rec->enc_key_size);
		rc = -ENOMEM;
		goto out;
	}
	rc = virt_to_scatterlist(key_rec->enc_key, key_rec->enc_key_size,
				 dst_sg, 2);
	if (rc < 1 || rc > 2) {
		ecryptfs_printk(KERN_ERR, "Error generating scatterlist "
				"for crypt_stat encrypted session key; "
				"expected rc = 1; got rc = [%d]. "
				"key_rec->enc_key_size = [%zd]\n", rc,
				key_rec->enc_key_size);
		rc = -ENOMEM;
		goto out;
	}
	mutex_lock(tfm_mutex);
	rc = crypto_skcipher_setkey(tfm, session_key_encryption_key,
				    crypt_stat->key_size);
	if (rc < 0) {
		mutex_unlock(tfm_mutex);
		ecryptfs_printk(KERN_ERR, "Error setting key for crypto "
				"context; rc = [%d]\n", rc);
		goto out;
	}

	req = skcipher_request_alloc(tfm, GFP_KERNEL);
	if (!req) {
		mutex_unlock(tfm_mutex);
		ecryptfs_printk(KERN_ERR, "Out of kernel memory whilst "
				"attempting to skcipher_request_alloc for "
				"%s\n", crypto_skcipher_driver_name(tfm));
		rc = -ENOMEM;
		goto out;
	}

	skcipher_request_set_callback(req, CRYPTO_TFM_REQ_MAY_SLEEP,
				      NULL, NULL);

	rc = 0;
	ecryptfs_printk(KERN_DEBUG, "Encrypting [%zd] bytes of the key\n",
			crypt_stat->key_size);
	skcipher_request_set_crypt(req, src_sg, dst_sg,
				   (*key_rec).enc_key_size, NULL);
	rc = crypto_skcipher_encrypt(req);
	mutex_unlock(tfm_mutex);
	skcipher_request_free(req);
	if (rc) {
		printk(KERN_ERR "Error encrypting; rc = [%d]\n", rc);
		goto out;
	}
	ecryptfs_printk(KERN_DEBUG, "This should be the encrypted key:\n");
	if (ecryptfs_verbosity > 0) {
		ecryptfs_printk(KERN_DEBUG, "EFEK of size [%zd]:\n",
				key_rec->enc_key_size);
		ecryptfs_dump_hex(key_rec->enc_key,
				  key_rec->enc_key_size);
	}
encrypted_session_key_set:
	/* This format is inspired by OpenPGP; see RFC 2440
	 * packet tag 3 */
	max_packet_size = (1                         /* Tag 3 identifier */
			   + 3                       /* Max Tag 3 packet size */
			   + 1                       /* Version */
			   + 1                       /* Cipher code */
			   + 1                       /* S2K specifier */
			   + 1                       /* Hash identifier */
			   + ECRYPTFS_SALT_SIZE      /* Salt */
			   + 1                       /* Hash iterations */
			   + key_rec->enc_key_size); /* Encrypted key size */
	if (max_packet_size > (*remaining_bytes)) {
		printk(KERN_ERR "Packet too large; need up to [%td] bytes, but "
		       "there are only [%td] available\n", max_packet_size,
		       (*remaining_bytes));
		rc = -EINVAL;
		goto out;
	}
	dest[(*packet_size)++] = ECRYPTFS_TAG_3_PACKET_TYPE;
	/* Chop off the Tag 3 identifier(1) and Tag 3 packet size(3)
	 * to get the number of octets in the actual Tag 3 packet */
	rc = ecryptfs_write_packet_length(&dest[(*packet_size)],
					  (max_packet_size - 4),
					  &packet_size_length);
	if (rc) {
		printk(KERN_ERR "Error generating tag 3 packet header; cannot "
		       "generate packet length. rc = [%d]\n", rc);
		goto out;
	}
	(*packet_size) += packet_size_length;
	dest[(*packet_size)++] = 0x04; /* version 4 */
	/* TODO: Break from RFC2440 so that arbitrary ciphers can be
	 * specified with strings */
	cipher_code = ecryptfs_code_for_cipher_string(crypt_stat->cipher,
						      crypt_stat->key_size);
	if (cipher_code == 0) {
		ecryptfs_printk(KERN_WARNING, "Unable to generate code for "
				"cipher [%s]\n", crypt_stat->cipher);
		rc = -EINVAL;
		goto out;
	}
	dest[(*packet_size)++] = cipher_code;
	dest[(*packet_size)++] = 0x03;	/* S2K */
	dest[(*packet_size)++] = 0x01;	/* MD5 (TODO: parameterize) */
	memcpy(&dest[(*packet_size)], auth_tok->token.password.salt,
	       ECRYPTFS_SALT_SIZE);
	(*packet_size) += ECRYPTFS_SALT_SIZE;	/* salt */
	dest[(*packet_size)++] = 0x60;	/* hash iterations (65536) */
	memcpy(&dest[(*packet_size)], key_rec->enc_key,
	       key_rec->enc_key_size);
	(*packet_size) += key_rec->enc_key_size;
out:
	if (rc)
		(*packet_size) = 0;
	else
		(*remaining_bytes) -= (*packet_size);
	return rc;
}

struct kmem_cache *ecryptfs_key_record_cache;

/**
 * ecryptfs_generate_key_packet_set
 * @dest_base: Virtual address from which to write the key record set
 * @crypt_stat: The cryptographic context from which the
 *              authentication tokens will be retrieved
 * @ecryptfs_dentry: The dentry, used to retrieve the mount crypt stat
 *                   for the global parameters
 * @len: The amount written
 * @max: The maximum amount of data allowed to be written
 *
 * Generates a key packet set and writes it to the virtual address
 * passed in.
 *
 * Returns zero on success; non-zero on error.
 */
int
ecryptfs_generate_key_packet_set(char *dest_base,
				 struct ecryptfs_crypt_stat *crypt_stat,
				 struct dentry *ecryptfs_dentry, size_t *len,
				 size_t max)
{
	struct ecryptfs_auth_tok *auth_tok;
	struct key *auth_tok_key = NULL;
	struct ecryptfs_mount_crypt_stat *mount_crypt_stat =
		&ecryptfs_superblock_to_private(
			ecryptfs_dentry->d_sb)->mount_crypt_stat;
	size_t written;
	struct ecryptfs_key_record *key_rec;
	struct ecryptfs_key_sig *key_sig;
	int rc = 0;

	(*len) = 0;
	mutex_lock(&crypt_stat->keysig_list_mutex);
	key_rec = kmem_cache_alloc(ecryptfs_key_record_cache, GFP_KERNEL);
	if (!key_rec) {
		rc = -ENOMEM;
		goto out;
	}
	list_for_each_entry(key_sig, &crypt_stat->keysig_list,
			    crypt_stat_list) {
		memset(key_rec, 0, sizeof(*key_rec));
		rc = ecryptfs_find_global_auth_tok_for_sig(&auth_tok_key,
							   &auth_tok,
							   mount_crypt_stat,
							   key_sig->keysig);
		if (rc) {
			printk(KERN_WARNING "Unable to retrieve auth tok with "
			       "sig = [%s]\n", key_sig->keysig);
			rc = process_find_global_auth_tok_for_sig_err(rc);
			goto out_free;
		}
		if (auth_tok->token_type == ECRYPTFS_PASSWORD) {
			rc = write_tag_3_packet((dest_base + (*len)),
						&max, auth_tok,
						crypt_stat, key_rec,
						&written);
			up_write(&(auth_tok_key->sem));
			key_put(auth_tok_key);
			if (rc) {
				ecryptfs_printk(KERN_WARNING, "Error "
						"writing tag 3 packet\n");
				goto out_free;
			}
			(*len) += written;
			/* Write auth tok signature packet */
			rc = write_tag_11_packet((dest_base + (*len)), &max,
						 key_rec->sig,
						 ECRYPTFS_SIG_SIZE, &written);
			if (rc) {
				ecryptfs_printk(KERN_ERR, "Error writing "
						"auth tok signature packet\n");
				goto out_free;
			}
			(*len) += written;
		} else if (auth_tok->token_type == ECRYPTFS_PRIVATE_KEY) {
			rc = write_tag_1_packet(dest_base + (*len), &max,
						auth_tok_key, auth_tok,
						crypt_stat, key_rec, &written);
			if (rc) {
				ecryptfs_printk(KERN_WARNING, "Error "
						"writing tag 1 packet\n");
				goto out_free;
			}
			(*len) += written;
		} else {
			up_write(&(auth_tok_key->sem));
			key_put(auth_tok_key);
			ecryptfs_printk(KERN_WARNING, "Unsupported "
					"authentication token type\n");
			rc = -EINVAL;
			goto out_free;
		}
	}
	if (likely(max > 0)) {
		dest_base[(*len)] = 0x00;
	} else {
		ecryptfs_printk(KERN_ERR, "Error writing boundary byte\n");
		rc = -EIO;
	}
out_free:
	kmem_cache_free(ecryptfs_key_record_cache, key_rec);
out:
	if (rc)
		(*len) = 0;
	mutex_unlock(&crypt_stat->keysig_list_mutex);
	return rc;
}

struct kmem_cache *ecryptfs_key_sig_cache;

int ecryptfs_add_keysig(struct ecryptfs_crypt_stat *crypt_stat, char *sig)
{
	struct ecryptfs_key_sig *new_key_sig;

	new_key_sig = kmem_cache_alloc(ecryptfs_key_sig_cache, GFP_KERNEL);
	if (!new_key_sig)
		return -ENOMEM;

	memcpy(new_key_sig->keysig, sig, ECRYPTFS_SIG_SIZE_HEX);
	new_key_sig->keysig[ECRYPTFS_SIG_SIZE_HEX] = '\0';
	/* Caller must hold keysig_list_mutex */
	list_add(&new_key_sig->crypt_stat_list, &crypt_stat->keysig_list);

	return 0;
}

struct kmem_cache *ecryptfs_global_auth_tok_cache;

int
ecryptfs_add_global_auth_tok(struct ecryptfs_mount_crypt_stat *mount_crypt_stat,
			     char *sig, u32 global_auth_tok_flags)
{
	struct ecryptfs_global_auth_tok *new_auth_tok;

	new_auth_tok = kmem_cache_zalloc(ecryptfs_global_auth_tok_cache,
					GFP_KERNEL);
	if (!new_auth_tok)
		return -ENOMEM;

	memcpy(new_auth_tok->sig, sig, ECRYPTFS_SIG_SIZE_HEX);
	new_auth_tok->flags = global_auth_tok_flags;
	new_auth_tok->sig[ECRYPTFS_SIG_SIZE_HEX] = '\0';
	mutex_lock(&mount_crypt_stat->global_auth_tok_list_mutex);
	list_add(&new_auth_tok->mount_crypt_stat_list,
		 &mount_crypt_stat->global_auth_tok_list);
	mutex_unlock(&mount_crypt_stat->global_auth_tok_list_mutex);
	return 0;
}

