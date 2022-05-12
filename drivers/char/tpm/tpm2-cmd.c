// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2014, 2015 Intel Corporation
 *
 * Authors:
 * Jarkko Sakkinen <jarkko.sakkinen@linux.intel.com>
 *
 * Maintained by: <tpmdd-devel@lists.sourceforge.net>
 *
 * This file contains TPM2 protocol implementations of the commands
 * used by the kernel internally.
 */

#include "tpm.h"
#include <crypto/hash_info.h>

static struct tpm2_hash tpm2_hash_map[] = {
	{HASH_ALGO_SHA1, TPM_ALG_SHA1},
	{HASH_ALGO_SHA256, TPM_ALG_SHA256},
	{HASH_ALGO_SHA384, TPM_ALG_SHA384},
	{HASH_ALGO_SHA512, TPM_ALG_SHA512},
	{HASH_ALGO_SM3_256, TPM_ALG_SM3_256},
};

int tpm2_get_timeouts(struct tpm_chip *chip)
{
	/* Fixed timeouts for TPM2 */
	chip->timeout_a = msecs_to_jiffies(TPM2_TIMEOUT_A);
	chip->timeout_b = msecs_to_jiffies(TPM2_TIMEOUT_B);
	chip->timeout_c = msecs_to_jiffies(TPM2_TIMEOUT_C);
	chip->timeout_d = msecs_to_jiffies(TPM2_TIMEOUT_D);

	/* PTP spec timeouts */
	chip->duration[TPM_SHORT] = msecs_to_jiffies(TPM2_DURATION_SHORT);
	chip->duration[TPM_MEDIUM] = msecs_to_jiffies(TPM2_DURATION_MEDIUM);
	chip->duration[TPM_LONG] = msecs_to_jiffies(TPM2_DURATION_LONG);

	/* Key creation commands long timeouts */
	chip->duration[TPM_LONG_LONG] =
		msecs_to_jiffies(TPM2_DURATION_LONG_LONG);

	chip->flags |= TPM_CHIP_FLAG_HAVE_TIMEOUTS;

	return 0;
}

/**
 * tpm2_ordinal_duration_index() - returns an index to the chip duration table
 * @ordinal: TPM command ordinal.
 *
 * The function returns an index to the chip duration table
 * (enum tpm_duration), that describes the maximum amount of
 * time the chip could take to return the result for a  particular ordinal.
 *
 * The values of the MEDIUM, and LONG durations are taken
 * from the PC Client Profile (PTP) specification (750, 2000 msec)
 *
 * LONG_LONG is for commands that generates keys which empirically takes
 * a longer time on some systems.
 *
 * Return:
 * * TPM_MEDIUM
 * * TPM_LONG
 * * TPM_LONG_LONG
 * * TPM_UNDEFINED
 */
static u8 tpm2_ordinal_duration_index(u32 ordinal)
{
	switch (ordinal) {
	/* Startup */
	case TPM2_CC_STARTUP:                 /* 144 */
		return TPM_MEDIUM;

	case TPM2_CC_SELF_TEST:               /* 143 */
		return TPM_LONG;

	case TPM2_CC_GET_RANDOM:              /* 17B */
		return TPM_LONG;

	case TPM2_CC_SEQUENCE_UPDATE:         /* 15C */
		return TPM_MEDIUM;
	case TPM2_CC_SEQUENCE_COMPLETE:       /* 13E */
		return TPM_MEDIUM;
	case TPM2_CC_EVENT_SEQUENCE_COMPLETE: /* 185 */
		return TPM_MEDIUM;
	case TPM2_CC_HASH_SEQUENCE_START:     /* 186 */
		return TPM_MEDIUM;

	case TPM2_CC_VERIFY_SIGNATURE:        /* 177 */
		return TPM_LONG;

	case TPM2_CC_PCR_EXTEND:              /* 182 */
		return TPM_MEDIUM;

	case TPM2_CC_HIERARCHY_CONTROL:       /* 121 */
		return TPM_LONG;
	case TPM2_CC_HIERARCHY_CHANGE_AUTH:   /* 129 */
		return TPM_LONG;

	case TPM2_CC_GET_CAPABILITY:          /* 17A */
		return TPM_MEDIUM;

	case TPM2_CC_NV_READ:                 /* 14E */
		return TPM_LONG;

	case TPM2_CC_CREATE_PRIMARY:          /* 131 */
		return TPM_LONG_LONG;
	case TPM2_CC_CREATE:                  /* 153 */
		return TPM_LONG_LONG;
	case TPM2_CC_CREATE_LOADED:           /* 191 */
		return TPM_LONG_LONG;

	default:
		return TPM_UNDEFINED;
	}
}

/**
 * tpm2_calc_ordinal_duration() - calculate the maximum command duration
 * @chip:    TPM chip to use.
 * @ordinal: TPM command ordinal.
 *
 * The function returns the maximum amount of time the chip could take
 * to return the result for a particular ordinal in jiffies.
 *
 * Return: A maximal duration time for an ordinal in jiffies.
 */
unsigned long tpm2_calc_ordinal_duration(struct tpm_chip *chip, u32 ordinal)
{
	unsigned int index;

	index = tpm2_ordinal_duration_index(ordinal);

	if (index != TPM_UNDEFINED)
		return chip->duration[index];
	else
		return msecs_to_jiffies(TPM2_DURATION_DEFAULT);
}


struct tpm2_pcr_read_out {
	__be32	update_cnt;
	__be32	pcr_selects_cnt;
	__be16	hash_alg;
	u8	pcr_select_size;
	u8	pcr_select[TPM2_PCR_SELECT_MIN];
	__be32	digests_cnt;
	__be16	digest_size;
	u8	digest[];
} __packed;

/**
 * tpm2_pcr_read() - read a PCR value
 * @chip:	TPM chip to use.
 * @pcr_idx:	index of the PCR to read.
 * @digest:	PCR bank and buffer current PCR value is written to.
 * @digest_size_ptr:	pointer to variable that stores the digest size.
 *
 * Return: Same as with tpm_transmit_cmd.
 */
int tpm2_pcr_read(struct tpm_chip *chip, u32 pcr_idx,
		  struct tpm_digest *digest, u16 *digest_size_ptr)
{
	int i;
	int rc;
	struct tpm_buf buf;
	struct tpm2_pcr_read_out *out;
	u8 pcr_select[TPM2_PCR_SELECT_MIN] = {0};
	u16 digest_size;
	u16 expected_digest_size = 0;

	if (pcr_idx >= TPM2_PLATFORM_PCR)
		return -EINVAL;

	if (!digest_size_ptr) {
		for (i = 0; i < chip->nr_allocated_banks &&
		     chip->allocated_banks[i].alg_id != digest->alg_id; i++)
			;

		if (i == chip->nr_allocated_banks)
			return -EINVAL;

		expected_digest_size = chip->allocated_banks[i].digest_size;
	}

	rc = tpm_buf_init(&buf, TPM2_ST_NO_SESSIONS, TPM2_CC_PCR_READ);
	if (rc)
		return rc;

	pcr_select[pcr_idx >> 3] = 1 << (pcr_idx & 0x7);

	tpm_buf_append_u32(&buf, 1);
	tpm_buf_append_u16(&buf, digest->alg_id);
	tpm_buf_append_u8(&buf, TPM2_PCR_SELECT_MIN);
	tpm_buf_append(&buf, (const unsigned char *)pcr_select,
		       sizeof(pcr_select));

	rc = tpm_transmit_cmd(chip, &buf, 0, "attempting to read a pcr value");
	if (rc)
		goto out;

	out = (struct tpm2_pcr_read_out *)&buf.data[TPM_HEADER_SIZE];
	digest_size = be16_to_cpu(out->digest_size);
	if (digest_size > sizeof(digest->digest) ||
	    (!digest_size_ptr && digest_size != expected_digest_size)) {
		rc = -EINVAL;
		goto out;
	}

	if (digest_size_ptr)
		*digest_size_ptr = digest_size;

	memcpy(digest->digest, out->digest, digest_size);
out:
	tpm_buf_destroy(&buf);
	return rc;
}

struct tpm2_null_auth_area {
	__be32  handle;
	__be16  nonce_size;
	u8  attributes;
	__be16  auth_size;
} __packed;

/**
 * tpm2_pcr_extend() - extend a PCR value
 *
 * @chip:	TPM chip to use.
 * @pcr_idx:	index of the PCR.
 * @digests:	list of pcr banks and corresponding digest values to extend.
 *
 * Return: Same as with tpm_transmit_cmd.
 */
int tpm2_pcr_extend(struct tpm_chip *chip, u32 pcr_idx,
		    struct tpm_digest *digests)
{
	struct tpm_buf buf;
	struct tpm2_null_auth_area auth_area;
	int rc;
	int i;

	rc = tpm_buf_init(&buf, TPM2_ST_SESSIONS, TPM2_CC_PCR_EXTEND);
	if (rc)
		return rc;

	tpm_buf_append_u32(&buf, pcr_idx);

	auth_area.handle = cpu_to_be32(TPM2_RS_PW);
	auth_area.nonce_size = 0;
	auth_area.attributes = 0;
	auth_area.auth_size = 0;

	tpm_buf_append_u32(&buf, sizeof(struct tpm2_null_auth_area));
	tpm_buf_append(&buf, (const unsigned char *)&auth_area,
		       sizeof(auth_area));
	tpm_buf_append_u32(&buf, chip->nr_allocated_banks);

	for (i = 0; i < chip->nr_allocated_banks; i++) {
		tpm_buf_append_u16(&buf, digests[i].alg_id);
		tpm_buf_append(&buf, (const unsigned char *)&digests[i].digest,
			       chip->allocated_banks[i].digest_size);
	}

	rc = tpm_transmit_cmd(chip, &buf, 0, "attempting extend a PCR value");

	tpm_buf_destroy(&buf);

	return rc;
}

struct tpm2_get_random_out {
	__be16 size;
	u8 buffer[TPM_MAX_RNG_DATA];
} __packed;

/**
 * tpm2_get_random() - get random bytes from the TPM RNG
 *
 * @chip:	a &tpm_chip instance
 * @dest:	destination buffer
 * @max:	the max number of random bytes to pull
 *
 * Return:
 *   size of the buffer on success,
 *   -errno otherwise (positive TPM return codes are masked to -EIO)
 */
int tpm2_get_random(struct tpm_chip *chip, u8 *dest, size_t max)
{
	struct tpm2_get_random_out *out;
	struct tpm_buf buf;
	u32 recd;
	u32 num_bytes = max;
	int err;
	int total = 0;
	int retries = 5;
	u8 *dest_ptr = dest;

	if (!num_bytes || max > TPM_MAX_RNG_DATA)
		return -EINVAL;

	err = tpm_buf_init(&buf, 0, 0);
	if (err)
		return err;

	do {
		tpm_buf_reset(&buf, TPM2_ST_NO_SESSIONS, TPM2_CC_GET_RANDOM);
		tpm_buf_append_u16(&buf, num_bytes);
		err = tpm_transmit_cmd(chip, &buf,
				       offsetof(struct tpm2_get_random_out,
						buffer),
				       "attempting get random");
		if (err) {
			if (err > 0)
				err = -EIO;
			goto out;
		}

		out = (struct tpm2_get_random_out *)
			&buf.data[TPM_HEADER_SIZE];
		recd = min_t(u32, be16_to_cpu(out->size), num_bytes);
		if (tpm_buf_length(&buf) <
		    TPM_HEADER_SIZE +
		    offsetof(struct tpm2_get_random_out, buffer) +
		    recd) {
			err = -EFAULT;
			goto out;
		}
		memcpy(dest_ptr, out->buffer, recd);

		dest_ptr += recd;
		total += recd;
		num_bytes -= recd;
	} while (retries-- && total < max);

	tpm_buf_destroy(&buf);
	return total ? total : -EIO;
out:
	tpm_buf_destroy(&buf);
	return err;
}

/**
 * tpm2_flush_context() - execute a TPM2_FlushContext command
 * @chip:	TPM chip to use
 * @handle:	context handle
 */
void tpm2_flush_context(struct tpm_chip *chip, u32 handle)
{
	struct tpm_buf buf;
	int rc;

	rc = tpm_buf_init(&buf, TPM2_ST_NO_SESSIONS, TPM2_CC_FLUSH_CONTEXT);
	if (rc) {
		dev_warn(&chip->dev, "0x%08x was not flushed, out of memory\n",
			 handle);
		return;
	}

	tpm_buf_append_u32(&buf, handle);

	tpm_transmit_cmd(chip, &buf, 0, "flushing context");
	tpm_buf_destroy(&buf);
}
EXPORT_SYMBOL_GPL(tpm2_flush_context);

struct tpm2_get_cap_out {
	u8 more_data;
	__be32 subcap_id;
	__be32 property_cnt;
	__be32 property_id;
	__be32 value;
} __packed;

/**
 * tpm2_get_tpm_pt() - get value of a TPM_CAP_TPM_PROPERTIES type property
 * @chip:		a &tpm_chip instance
 * @property_id:	property ID.
 * @value:		output variable.
 * @desc:		passed to tpm_transmit_cmd()
 *
 * Return:
 *   0 on success,
 *   -errno or a TPM return code otherwise
 */
ssize_t tpm2_get_tpm_pt(struct tpm_chip *chip, u32 property_id,  u32 *value,
			const char *desc)
{
	struct tpm2_get_cap_out *out;
	struct tpm_buf buf;
	int rc;

	rc = tpm_buf_init(&buf, TPM2_ST_NO_SESSIONS, TPM2_CC_GET_CAPABILITY);
	if (rc)
		return rc;
	tpm_buf_append_u32(&buf, TPM2_CAP_TPM_PROPERTIES);
	tpm_buf_append_u32(&buf, property_id);
	tpm_buf_append_u32(&buf, 1);
	rc = tpm_transmit_cmd(chip, &buf, 0, NULL);
	if (!rc) {
		out = (struct tpm2_get_cap_out *)
			&buf.data[TPM_HEADER_SIZE];
		*value = be32_to_cpu(out->value);
	}
	tpm_buf_destroy(&buf);
	return rc;
}
EXPORT_SYMBOL_GPL(tpm2_get_tpm_pt);

/**
 * tpm2_shutdown() - send a TPM shutdown command
 *
 * Sends a TPM shutdown command. The shutdown command is used in call
 * sites where the system is going down. If it fails, there is not much
 * that can be done except print an error message.
 *
 * @chip:		a &tpm_chip instance
 * @shutdown_type:	TPM_SU_CLEAR or TPM_SU_STATE.
 */
void tpm2_shutdown(struct tpm_chip *chip, u16 shutdown_type)
{
	struct tpm_buf buf;
	int rc;

	rc = tpm_buf_init(&buf, TPM2_ST_NO_SESSIONS, TPM2_CC_SHUTDOWN);
	if (rc)
		return;
	tpm_buf_append_u16(&buf, shutdown_type);
	tpm_transmit_cmd(chip, &buf, 0, "stopping the TPM");
	tpm_buf_destroy(&buf);
}

/**
 * tpm2_do_selftest() - ensure that all self tests have passed
 *
 * @chip: TPM chip to use
 *
 * Return: Same as with tpm_transmit_cmd.
 *
 * The TPM can either run all self tests synchronously and then return
 * RC_SUCCESS once all tests were successful. Or it can choose to run the tests
 * asynchronously and return RC_TESTING immediately while the self tests still
 * execute in the background. This function handles both cases and waits until
 * all tests have completed.
 */
static int tpm2_do_selftest(struct tpm_chip *chip)
{
	struct tpm_buf buf;
	int full;
	int rc;

	for (full = 0; full < 2; full++) {
		rc = tpm_buf_init(&buf, TPM2_ST_NO_SESSIONS, TPM2_CC_SELF_TEST);
		if (rc)
			return rc;

		tpm_buf_append_u8(&buf, full);
		rc = tpm_transmit_cmd(chip, &buf, 0,
				      "attempting the self test");
		tpm_buf_destroy(&buf);

		if (rc == TPM2_RC_TESTING)
			rc = TPM2_RC_SUCCESS;
		if (rc == TPM2_RC_INITIALIZE || rc == TPM2_RC_SUCCESS)
			return rc;
	}

	return rc;
}

/**
 * tpm2_probe() - probe for the TPM 2.0 protocol
 * @chip:	a &tpm_chip instance
 *
 * Send an idempotent TPM 2.0 command and see whether there is TPM2 chip in the
 * other end based on the response tag. The flag TPM_CHIP_FLAG_TPM2 is set by
 * this function if this is the case.
 *
 * Return:
 *   0 on success,
 *   -errno otherwise
 */
int tpm2_probe(struct tpm_chip *chip)
{
	struct tpm_header *out;
	struct tpm_buf buf;
	int rc;

	rc = tpm_buf_init(&buf, TPM2_ST_NO_SESSIONS, TPM2_CC_GET_CAPABILITY);
	if (rc)
		return rc;
	tpm_buf_append_u32(&buf, TPM2_CAP_TPM_PROPERTIES);
	tpm_buf_append_u32(&buf, TPM_PT_TOTAL_COMMANDS);
	tpm_buf_append_u32(&buf, 1);
	rc = tpm_transmit_cmd(chip, &buf, 0, NULL);
	/* We ignore TPM return codes on purpose. */
	if (rc >=  0) {
		out = (struct tpm_header *)buf.data;
		if (be16_to_cpu(out->tag) == TPM2_ST_NO_SESSIONS)
			chip->flags |= TPM_CHIP_FLAG_TPM2;
	}
	tpm_buf_destroy(&buf);
	return 0;
}
EXPORT_SYMBOL_GPL(tpm2_probe);

static int tpm2_init_bank_info(struct tpm_chip *chip, u32 bank_index)
{
	struct tpm_bank_info *bank = chip->allocated_banks + bank_index;
	struct tpm_digest digest = { .alg_id = bank->alg_id };
	int i;

	/*
	 * Avoid unnecessary PCR read operations to reduce overhead
	 * and obtain identifiers of the crypto subsystem.
	 */
	for (i = 0; i < ARRAY_SIZE(tpm2_hash_map); i++) {
		enum hash_algo crypto_algo = tpm2_hash_map[i].crypto_id;

		if (bank->alg_id != tpm2_hash_map[i].tpm_id)
			continue;

		bank->digest_size = hash_digest_size[crypto_algo];
		bank->crypto_id = crypto_algo;
		return 0;
	}

	bank->crypto_id = HASH_ALGO__LAST;

	return tpm2_pcr_read(chip, 0, &digest, &bank->digest_size);
}

struct tpm2_pcr_selection {
	__be16  hash_alg;
	u8  size_of_select;
	u8  pcr_select[3];
} __packed;

ssize_t tpm2_get_pcr_allocation(struct tpm_chip *chip)
{
	struct tpm2_pcr_selection pcr_selection;
	struct tpm_buf buf;
	void *marker;
	void *end;
	void *pcr_select_offset;
	u32 sizeof_pcr_selection;
	u32 nr_possible_banks;
	u32 nr_alloc_banks = 0;
	u16 hash_alg;
	u32 rsp_len;
	int rc;
	int i = 0;

	rc = tpm_buf_init(&buf, TPM2_ST_NO_SESSIONS, TPM2_CC_GET_CAPABILITY);
	if (rc)
		return rc;

	tpm_buf_append_u32(&buf, TPM2_CAP_PCRS);
	tpm_buf_append_u32(&buf, 0);
	tpm_buf_append_u32(&buf, 1);

	rc = tpm_transmit_cmd(chip, &buf, 9, "get tpm pcr allocation");
	if (rc)
		goto out;

	nr_possible_banks = be32_to_cpup(
		(__be32 *)&buf.data[TPM_HEADER_SIZE + 5]);

	chip->allocated_banks = kcalloc(nr_possible_banks,
					sizeof(*chip->allocated_banks),
					GFP_KERNEL);
	if (!chip->allocated_banks) {
		rc = -ENOMEM;
		goto out;
	}

	marker = &buf.data[TPM_HEADER_SIZE + 9];

	rsp_len = be32_to_cpup((__be32 *)&buf.data[2]);
	end = &buf.data[rsp_len];

	for (i = 0; i < nr_possible_banks; i++) {
		pcr_select_offset = marker +
			offsetof(struct tpm2_pcr_selection, size_of_select);
		if (pcr_select_offset >= end) {
			rc = -EFAULT;
			break;
		}

		memcpy(&pcr_selection, marker, sizeof(pcr_selection));
		hash_alg = be16_to_cpu(pcr_selection.hash_alg);

		pcr_select_offset = memchr_inv(pcr_selection.pcr_select, 0,
					       pcr_selection.size_of_select);
		if (pcr_select_offset) {
			chip->allocated_banks[nr_alloc_banks].alg_id = hash_alg;

			rc = tpm2_init_bank_info(chip, nr_alloc_banks);
			if (rc < 0)
				break;

			nr_alloc_banks++;
		}

		sizeof_pcr_selection = sizeof(pcr_selection.hash_alg) +
			sizeof(pcr_selection.size_of_select) +
			pcr_selection.size_of_select;
		marker = marker + sizeof_pcr_selection;
	}

	chip->nr_allocated_banks = nr_alloc_banks;
out:
	tpm_buf_destroy(&buf);

	return rc;
}

int tpm2_get_cc_attrs_tbl(struct tpm_chip *chip)
{
	struct tpm_buf buf;
	u32 nr_commands;
	__be32 *attrs;
	u32 cc;
	int i;
	int rc;

	rc = tpm2_get_tpm_pt(chip, TPM_PT_TOTAL_COMMANDS, &nr_commands, NULL);
	if (rc)
		goto out;

	if (nr_commands > 0xFFFFF) {
		rc = -EFAULT;
		goto out;
	}

	chip->cc_attrs_tbl = devm_kcalloc(&chip->dev, 4, nr_commands,
					  GFP_KERNEL);
	if (!chip->cc_attrs_tbl) {
		rc = -ENOMEM;
		goto out;
	}

	rc = tpm_buf_init(&buf, TPM2_ST_NO_SESSIONS, TPM2_CC_GET_CAPABILITY);
	if (rc)
		goto out;

	tpm_buf_append_u32(&buf, TPM2_CAP_COMMANDS);
	tpm_buf_append_u32(&buf, TPM2_CC_FIRST);
	tpm_buf_append_u32(&buf, nr_commands);

	rc = tpm_transmit_cmd(chip, &buf, 9 + 4 * nr_commands, NULL);
	if (rc) {
		tpm_buf_destroy(&buf);
		goto out;
	}

	if (nr_commands !=
	    be32_to_cpup((__be32 *)&buf.data[TPM_HEADER_SIZE + 5])) {
		rc = -EFAULT;
		tpm_buf_destroy(&buf);
		goto out;
	}

	chip->nr_commands = nr_commands;

	attrs = (__be32 *)&buf.data[TPM_HEADER_SIZE + 9];
	for (i = 0; i < nr_commands; i++, attrs++) {
		chip->cc_attrs_tbl[i] = be32_to_cpup(attrs);
		cc = chip->cc_attrs_tbl[i] & 0xFFFF;

		if (cc == TPM2_CC_CONTEXT_SAVE || cc == TPM2_CC_FLUSH_CONTEXT) {
			chip->cc_attrs_tbl[i] &=
				~(GENMASK(2, 0) << TPM2_CC_ATTR_CHANDLES);
			chip->cc_attrs_tbl[i] |= 1 << TPM2_CC_ATTR_CHANDLES;
		}
	}

	tpm_buf_destroy(&buf);

out:
	if (rc > 0)
		rc = -ENODEV;
	return rc;
}
EXPORT_SYMBOL_GPL(tpm2_get_cc_attrs_tbl);

/**
 * tpm2_startup - turn on the TPM
 * @chip: TPM chip to use
 *
 * Normally the firmware should start the TPM. This function is provided as a
 * workaround if this does not happen. A legal case for this could be for
 * example when a TPM emulator is used.
 *
 * Return: same as tpm_transmit_cmd()
 */

static int tpm2_startup(struct tpm_chip *chip)
{
	struct tpm_buf buf;
	int rc;

	dev_info(&chip->dev, "starting up the TPM manually\n");

	rc = tpm_buf_init(&buf, TPM2_ST_NO_SESSIONS, TPM2_CC_STARTUP);
	if (rc < 0)
		return rc;

	tpm_buf_append_u16(&buf, TPM2_SU_CLEAR);
	rc = tpm_transmit_cmd(chip, &buf, 0, "attempting to start the TPM");
	tpm_buf_destroy(&buf);

	return rc;
}

/**
 * tpm2_auto_startup - Perform the standard automatic TPM initialization
 *                     sequence
 * @chip: TPM chip to use
 *
 * Returns 0 on success, < 0 in case of fatal error.
 */
int tpm2_auto_startup(struct tpm_chip *chip)
{
	int rc;

	rc = tpm2_get_timeouts(chip);
	if (rc)
		goto out;

	rc = tpm2_do_selftest(chip);
	if (rc && rc != TPM2_RC_INITIALIZE)
		goto out;

	if (rc == TPM2_RC_INITIALIZE) {
		rc = tpm2_startup(chip);
		if (rc)
			goto out;

		rc = tpm2_do_selftest(chip);
		if (rc)
			goto out;
	}

	rc = tpm2_get_cc_attrs_tbl(chip);

out:
	if (rc > 0)
		rc = -ENODEV;
	return rc;
}

int tpm2_find_cc(struct tpm_chip *chip, u32 cc)
{
	int i;

	for (i = 0; i < chip->nr_commands; i++)
		if (cc == (chip->cc_attrs_tbl[i] & GENMASK(15, 0)))
			return i;

	return -1;
}
