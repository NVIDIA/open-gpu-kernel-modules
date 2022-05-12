// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright IBM Corp. 2006, 2015
 * Author(s): Jan Glauber <jan.glauber@de.ibm.com>
 *	      Harald Freudenberger <freude@de.ibm.com>
 * Driver for the s390 pseudo random number generator
 */

#define KMSG_COMPONENT "prng"
#define pr_fmt(fmt) KMSG_COMPONENT ": " fmt

#include <linux/fs.h>
#include <linux/fips.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/mutex.h>
#include <linux/cpufeature.h>
#include <linux/random.h>
#include <linux/slab.h>
#include <linux/sched/signal.h>

#include <asm/debug.h>
#include <linux/uaccess.h>
#include <asm/timex.h>
#include <asm/cpacf.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("IBM Corporation");
MODULE_DESCRIPTION("s390 PRNG interface");


#define PRNG_MODE_AUTO	  0
#define PRNG_MODE_TDES	  1
#define PRNG_MODE_SHA512  2

static unsigned int prng_mode = PRNG_MODE_AUTO;
module_param_named(mode, prng_mode, int, 0);
MODULE_PARM_DESC(prng_mode, "PRNG mode: 0 - auto, 1 - TDES, 2 - SHA512");


#define PRNG_CHUNKSIZE_TDES_MIN   8
#define PRNG_CHUNKSIZE_TDES_MAX   (64*1024)
#define PRNG_CHUNKSIZE_SHA512_MIN 64
#define PRNG_CHUNKSIZE_SHA512_MAX (64*1024)

static unsigned int prng_chunk_size = 256;
module_param_named(chunksize, prng_chunk_size, int, 0);
MODULE_PARM_DESC(prng_chunk_size, "PRNG read chunk size in bytes");


#define PRNG_RESEED_LIMIT_TDES		 4096
#define PRNG_RESEED_LIMIT_TDES_LOWER	 4096
#define PRNG_RESEED_LIMIT_SHA512       100000
#define PRNG_RESEED_LIMIT_SHA512_LOWER	10000

static unsigned int prng_reseed_limit;
module_param_named(reseed_limit, prng_reseed_limit, int, 0);
MODULE_PARM_DESC(prng_reseed_limit, "PRNG reseed limit");

static bool trng_available;

/*
 * Any one who considers arithmetical methods of producing random digits is,
 * of course, in a state of sin. -- John von Neumann
 */

static int prng_errorflag;

#define PRNG_GEN_ENTROPY_FAILED  1
#define PRNG_SELFTEST_FAILED	 2
#define PRNG_INSTANTIATE_FAILED  3
#define PRNG_SEED_FAILED	 4
#define PRNG_RESEED_FAILED	 5
#define PRNG_GEN_FAILED		 6

struct prng_ws_s {
	u8  parm_block[32];
	u32 reseed_counter;
	u64 byte_counter;
};

struct prno_ws_s {
	u32 res;
	u32 reseed_counter;
	u64 stream_bytes;
	u8  V[112];
	u8  C[112];
};

struct prng_data_s {
	struct mutex mutex;
	union {
		struct prng_ws_s prngws;
		struct prno_ws_s prnows;
	};
	u8 *buf;
	u32 rest;
	u8 *prev;
};

static struct prng_data_s *prng_data;

/* initial parameter block for tdes mode, copied from libica */
static const u8 initial_parm_block[32] __initconst = {
	0x0F, 0x2B, 0x8E, 0x63, 0x8C, 0x8E, 0xD2, 0x52,
	0x64, 0xB7, 0xA0, 0x7B, 0x75, 0x28, 0xB8, 0xF4,
	0x75, 0x5F, 0xD2, 0xA6, 0x8D, 0x97, 0x11, 0xFF,
	0x49, 0xD8, 0x23, 0xF3, 0x7E, 0x21, 0xEC, 0xA0 };


/*** helper functions ***/

/*
 * generate_entropy:
 * This function fills a given buffer with random bytes. The entropy within
 * the random bytes given back is assumed to have at least 50% - meaning
 * a 64 bytes buffer has at least 64 * 8 / 2 = 256 bits of entropy.
 * Within the function the entropy generation is done in junks of 64 bytes.
 * So the caller should also ask for buffer fill in multiples of 64 bytes.
 * The generation of the entropy is based on the assumption that every stckf()
 * invocation produces 0.5 bits of entropy. To accumulate 256 bits of entropy
 * at least 512 stckf() values are needed. The entropy relevant part of the
 * stckf value is bit 51 (counting starts at the left with bit nr 0) so
 * here we use the lower 4 bytes and exor the values into 2k of bufferspace.
 * To be on the save side, if there is ever a problem with stckf() the
 * other half of the page buffer is filled with bytes from urandom via
 * get_random_bytes(), so this function consumes 2k of urandom for each
 * requested 64 bytes output data. Finally the buffer page is condensed into
 * a 64 byte value by hashing with a SHA512 hash.
 */
static int generate_entropy(u8 *ebuf, size_t nbytes)
{
	int n, ret = 0;
	u8 *pg, pblock[80] = {
		/* 8 x 64 bit init values */
		0x6A, 0x09, 0xE6, 0x67, 0xF3, 0xBC, 0xC9, 0x08,
		0xBB, 0x67, 0xAE, 0x85, 0x84, 0xCA, 0xA7, 0x3B,
		0x3C, 0x6E, 0xF3, 0x72, 0xFE, 0x94, 0xF8, 0x2B,
		0xA5, 0x4F, 0xF5, 0x3A, 0x5F, 0x1D, 0x36, 0xF1,
		0x51, 0x0E, 0x52, 0x7F, 0xAD, 0xE6, 0x82, 0xD1,
		0x9B, 0x05, 0x68, 0x8C, 0x2B, 0x3E, 0x6C, 0x1F,
		0x1F, 0x83, 0xD9, 0xAB, 0xFB, 0x41, 0xBD, 0x6B,
		0x5B, 0xE0, 0xCD, 0x19, 0x13, 0x7E, 0x21, 0x79,
		/* 128 bit counter total message bit length */
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00 };

	/* allocate one page stckf buffer */
	pg = (u8 *) __get_free_page(GFP_KERNEL);
	if (!pg) {
		prng_errorflag = PRNG_GEN_ENTROPY_FAILED;
		return -ENOMEM;
	}

	/* fill the ebuf in chunks of 64 byte each */
	while (nbytes) {
		/* fill lower 2k with urandom bytes */
		get_random_bytes(pg, PAGE_SIZE / 2);
		/* exor upper 2k with 512 stckf values, offset 4 bytes each */
		for (n = 0; n < 512; n++) {
			int offset = (PAGE_SIZE / 2) + (n * 4) - 4;
			u64 *p = (u64 *)(pg + offset);
			*p ^= get_tod_clock_fast();
		}
		/* hash over the filled page */
		cpacf_klmd(CPACF_KLMD_SHA_512, pblock, pg, PAGE_SIZE);
		n = (nbytes < 64) ? nbytes : 64;
		memcpy(ebuf, pblock, n);
		ret += n;
		ebuf += n;
		nbytes -= n;
	}

	memzero_explicit(pblock, sizeof(pblock));
	memzero_explicit(pg, PAGE_SIZE);
	free_page((unsigned long)pg);
	return ret;
}


/*** tdes functions ***/

static void prng_tdes_add_entropy(void)
{
	__u64 entropy[4];
	unsigned int i;

	for (i = 0; i < 16; i++) {
		cpacf_kmc(CPACF_KMC_PRNG, prng_data->prngws.parm_block,
			  (char *) entropy, (char *) entropy,
			  sizeof(entropy));
		memcpy(prng_data->prngws.parm_block, entropy, sizeof(entropy));
	}
}


static void prng_tdes_seed(int nbytes)
{
	char buf[16];
	int i = 0;

	BUG_ON(nbytes > sizeof(buf));

	get_random_bytes(buf, nbytes);

	/* Add the entropy */
	while (nbytes >= 8) {
		*((__u64 *)prng_data->prngws.parm_block) ^= *((__u64 *)(buf+i));
		prng_tdes_add_entropy();
		i += 8;
		nbytes -= 8;
	}
	prng_tdes_add_entropy();
	prng_data->prngws.reseed_counter = 0;
}


static int __init prng_tdes_instantiate(void)
{
	int datalen;

	pr_debug("prng runs in TDES mode with "
		 "chunksize=%d and reseed_limit=%u\n",
		 prng_chunk_size, prng_reseed_limit);

	/* memory allocation, prng_data struct init, mutex init */
	datalen = sizeof(struct prng_data_s) + prng_chunk_size;
	prng_data = kzalloc(datalen, GFP_KERNEL);
	if (!prng_data) {
		prng_errorflag = PRNG_INSTANTIATE_FAILED;
		return -ENOMEM;
	}
	mutex_init(&prng_data->mutex);
	prng_data->buf = ((u8 *)prng_data) + sizeof(struct prng_data_s);
	memcpy(prng_data->prngws.parm_block, initial_parm_block, 32);

	/* initialize the PRNG, add 128 bits of entropy */
	prng_tdes_seed(16);

	return 0;
}


static void prng_tdes_deinstantiate(void)
{
	pr_debug("The prng module stopped "
		 "after running in triple DES mode\n");
	kfree_sensitive(prng_data);
}


/*** sha512 functions ***/

static int __init prng_sha512_selftest(void)
{
	/* NIST DRBG testvector for Hash Drbg, Sha-512, Count #0 */
	static const u8 seed[] __initconst = {
		0x6b, 0x50, 0xa7, 0xd8, 0xf8, 0xa5, 0x5d, 0x7a,
		0x3d, 0xf8, 0xbb, 0x40, 0xbc, 0xc3, 0xb7, 0x22,
		0xd8, 0x70, 0x8d, 0xe6, 0x7f, 0xda, 0x01, 0x0b,
		0x03, 0xc4, 0xc8, 0x4d, 0x72, 0x09, 0x6f, 0x8c,
		0x3e, 0xc6, 0x49, 0xcc, 0x62, 0x56, 0xd9, 0xfa,
		0x31, 0xdb, 0x7a, 0x29, 0x04, 0xaa, 0xf0, 0x25 };
	static const u8 V0[] __initconst = {
		0x00, 0xad, 0xe3, 0x6f, 0x9a, 0x01, 0xc7, 0x76,
		0x61, 0x34, 0x35, 0xf5, 0x4e, 0x24, 0x74, 0x22,
		0x21, 0x9a, 0x29, 0x89, 0xc7, 0x93, 0x2e, 0x60,
		0x1e, 0xe8, 0x14, 0x24, 0x8d, 0xd5, 0x03, 0xf1,
		0x65, 0x5d, 0x08, 0x22, 0x72, 0xd5, 0xad, 0x95,
		0xe1, 0x23, 0x1e, 0x8a, 0xa7, 0x13, 0xd9, 0x2b,
		0x5e, 0xbc, 0xbb, 0x80, 0xab, 0x8d, 0xe5, 0x79,
		0xab, 0x5b, 0x47, 0x4e, 0xdd, 0xee, 0x6b, 0x03,
		0x8f, 0x0f, 0x5c, 0x5e, 0xa9, 0x1a, 0x83, 0xdd,
		0xd3, 0x88, 0xb2, 0x75, 0x4b, 0xce, 0x83, 0x36,
		0x57, 0x4b, 0xf1, 0x5c, 0xca, 0x7e, 0x09, 0xc0,
		0xd3, 0x89, 0xc6, 0xe0, 0xda, 0xc4, 0x81, 0x7e,
		0x5b, 0xf9, 0xe1, 0x01, 0xc1, 0x92, 0x05, 0xea,
		0xf5, 0x2f, 0xc6, 0xc6, 0xc7, 0x8f, 0xbc, 0xf4 };
	static const u8 C0[] __initconst = {
		0x00, 0xf4, 0xa3, 0xe5, 0xa0, 0x72, 0x63, 0x95,
		0xc6, 0x4f, 0x48, 0xd0, 0x8b, 0x5b, 0x5f, 0x8e,
		0x6b, 0x96, 0x1f, 0x16, 0xed, 0xbc, 0x66, 0x94,
		0x45, 0x31, 0xd7, 0x47, 0x73, 0x22, 0xa5, 0x86,
		0xce, 0xc0, 0x4c, 0xac, 0x63, 0xb8, 0x39, 0x50,
		0xbf, 0xe6, 0x59, 0x6c, 0x38, 0x58, 0x99, 0x1f,
		0x27, 0xa7, 0x9d, 0x71, 0x2a, 0xb3, 0x7b, 0xf9,
		0xfb, 0x17, 0x86, 0xaa, 0x99, 0x81, 0xaa, 0x43,
		0xe4, 0x37, 0xd3, 0x1e, 0x6e, 0xe5, 0xe6, 0xee,
		0xc2, 0xed, 0x95, 0x4f, 0x53, 0x0e, 0x46, 0x8a,
		0xcc, 0x45, 0xa5, 0xdb, 0x69, 0x0d, 0x81, 0xc9,
		0x32, 0x92, 0xbc, 0x8f, 0x33, 0xe6, 0xf6, 0x09,
		0x7c, 0x8e, 0x05, 0x19, 0x0d, 0xf1, 0xb6, 0xcc,
		0xf3, 0x02, 0x21, 0x90, 0x25, 0xec, 0xed, 0x0e };
	static const u8 random[] __initconst = {
		0x95, 0xb7, 0xf1, 0x7e, 0x98, 0x02, 0xd3, 0x57,
		0x73, 0x92, 0xc6, 0xa9, 0xc0, 0x80, 0x83, 0xb6,
		0x7d, 0xd1, 0x29, 0x22, 0x65, 0xb5, 0xf4, 0x2d,
		0x23, 0x7f, 0x1c, 0x55, 0xbb, 0x9b, 0x10, 0xbf,
		0xcf, 0xd8, 0x2c, 0x77, 0xa3, 0x78, 0xb8, 0x26,
		0x6a, 0x00, 0x99, 0x14, 0x3b, 0x3c, 0x2d, 0x64,
		0x61, 0x1e, 0xee, 0xb6, 0x9a, 0xcd, 0xc0, 0x55,
		0x95, 0x7c, 0x13, 0x9e, 0x8b, 0x19, 0x0c, 0x7a,
		0x06, 0x95, 0x5f, 0x2c, 0x79, 0x7c, 0x27, 0x78,
		0xde, 0x94, 0x03, 0x96, 0xa5, 0x01, 0xf4, 0x0e,
		0x91, 0x39, 0x6a, 0xcf, 0x8d, 0x7e, 0x45, 0xeb,
		0xdb, 0xb5, 0x3b, 0xbf, 0x8c, 0x97, 0x52, 0x30,
		0xd2, 0xf0, 0xff, 0x91, 0x06, 0xc7, 0x61, 0x19,
		0xae, 0x49, 0x8e, 0x7f, 0xbc, 0x03, 0xd9, 0x0f,
		0x8e, 0x4c, 0x51, 0x62, 0x7a, 0xed, 0x5c, 0x8d,
		0x42, 0x63, 0xd5, 0xd2, 0xb9, 0x78, 0x87, 0x3a,
		0x0d, 0xe5, 0x96, 0xee, 0x6d, 0xc7, 0xf7, 0xc2,
		0x9e, 0x37, 0xee, 0xe8, 0xb3, 0x4c, 0x90, 0xdd,
		0x1c, 0xf6, 0xa9, 0xdd, 0xb2, 0x2b, 0x4c, 0xbd,
		0x08, 0x6b, 0x14, 0xb3, 0x5d, 0xe9, 0x3d, 0xa2,
		0xd5, 0xcb, 0x18, 0x06, 0x69, 0x8c, 0xbd, 0x7b,
		0xbb, 0x67, 0xbf, 0xe3, 0xd3, 0x1f, 0xd2, 0xd1,
		0xdb, 0xd2, 0xa1, 0xe0, 0x58, 0xa3, 0xeb, 0x99,
		0xd7, 0xe5, 0x1f, 0x1a, 0x93, 0x8e, 0xed, 0x5e,
		0x1c, 0x1d, 0xe2, 0x3a, 0x6b, 0x43, 0x45, 0xd3,
		0x19, 0x14, 0x09, 0xf9, 0x2f, 0x39, 0xb3, 0x67,
		0x0d, 0x8d, 0xbf, 0xb6, 0x35, 0xd8, 0xe6, 0xa3,
		0x69, 0x32, 0xd8, 0x10, 0x33, 0xd1, 0x44, 0x8d,
		0x63, 0xb4, 0x03, 0xdd, 0xf8, 0x8e, 0x12, 0x1b,
		0x6e, 0x81, 0x9a, 0xc3, 0x81, 0x22, 0x6c, 0x13,
		0x21, 0xe4, 0xb0, 0x86, 0x44, 0xf6, 0x72, 0x7c,
		0x36, 0x8c, 0x5a, 0x9f, 0x7a, 0x4b, 0x3e, 0xe2 };

	u8 buf[sizeof(random)];
	struct prno_ws_s ws;

	memset(&ws, 0, sizeof(ws));

	/* initial seed */
	cpacf_prno(CPACF_PRNO_SHA512_DRNG_SEED,
		   &ws, NULL, 0, seed, sizeof(seed));

	/* check working states V and C */
	if (memcmp(ws.V, V0, sizeof(V0)) != 0
	    || memcmp(ws.C, C0, sizeof(C0)) != 0) {
		pr_err("The prng self test state test "
		       "for the SHA-512 mode failed\n");
		prng_errorflag = PRNG_SELFTEST_FAILED;
		return -EIO;
	}

	/* generate random bytes */
	cpacf_prno(CPACF_PRNO_SHA512_DRNG_GEN,
		   &ws, buf, sizeof(buf), NULL, 0);
	cpacf_prno(CPACF_PRNO_SHA512_DRNG_GEN,
		   &ws, buf, sizeof(buf), NULL, 0);

	/* check against expected data */
	if (memcmp(buf, random, sizeof(random)) != 0) {
		pr_err("The prng self test data test "
		       "for the SHA-512 mode failed\n");
		prng_errorflag = PRNG_SELFTEST_FAILED;
		return -EIO;
	}

	return 0;
}


static int __init prng_sha512_instantiate(void)
{
	int ret, datalen, seedlen;
	u8 seed[128 + 16];

	pr_debug("prng runs in SHA-512 mode "
		 "with chunksize=%d and reseed_limit=%u\n",
		 prng_chunk_size, prng_reseed_limit);

	/* memory allocation, prng_data struct init, mutex init */
	datalen = sizeof(struct prng_data_s) + prng_chunk_size;
	if (fips_enabled)
		datalen += prng_chunk_size;
	prng_data = kzalloc(datalen, GFP_KERNEL);
	if (!prng_data) {
		prng_errorflag = PRNG_INSTANTIATE_FAILED;
		return -ENOMEM;
	}
	mutex_init(&prng_data->mutex);
	prng_data->buf = ((u8 *)prng_data) + sizeof(struct prng_data_s);

	/* selftest */
	ret = prng_sha512_selftest();
	if (ret)
		goto outfree;

	/* generate initial seed, we need at least  256 + 128 bits entropy. */
	if (trng_available) {
		/*
		 * Trng available, so use it. The trng works in chunks of
		 * 32 bytes and produces 100% entropy. So we pull 64 bytes
		 * which gives us 512 bits entropy.
		 */
		seedlen = 2 * 32;
		cpacf_trng(NULL, 0, seed, seedlen);
	} else {
		/*
		 * No trng available, so use the generate_entropy() function.
		 * This function works in 64 byte junks and produces
		 * 50% entropy. So we pull 2*64 bytes which gives us 512 bits
		 * of entropy.
		 */
		seedlen = 2 * 64;
		ret = generate_entropy(seed, seedlen);
		if (ret != seedlen)
			goto outfree;
	}

	/* append the seed by 16 bytes of unique nonce */
	store_tod_clock_ext((union tod_clock *)(seed + seedlen));
	seedlen += 16;

	/* now initial seed of the prno drng */
	cpacf_prno(CPACF_PRNO_SHA512_DRNG_SEED,
		   &prng_data->prnows, NULL, 0, seed, seedlen);
	memzero_explicit(seed, sizeof(seed));

	/* if fips mode is enabled, generate a first block of random
	   bytes for the FIPS 140-2 Conditional Self Test */
	if (fips_enabled) {
		prng_data->prev = prng_data->buf + prng_chunk_size;
		cpacf_prno(CPACF_PRNO_SHA512_DRNG_GEN,
			   &prng_data->prnows,
			   prng_data->prev, prng_chunk_size, NULL, 0);
	}

	return 0;

outfree:
	kfree(prng_data);
	return ret;
}


static void prng_sha512_deinstantiate(void)
{
	pr_debug("The prng module stopped after running in SHA-512 mode\n");
	kfree_sensitive(prng_data);
}


static int prng_sha512_reseed(void)
{
	int ret, seedlen;
	u8 seed[64];

	/* We need at least 256 bits of fresh entropy for reseeding */
	if (trng_available) {
		/* trng produces 256 bits entropy in 32 bytes */
		seedlen = 32;
		cpacf_trng(NULL, 0, seed, seedlen);
	} else {
		/* generate_entropy() produces 256 bits entropy in 64 bytes */
		seedlen = 64;
		ret = generate_entropy(seed, seedlen);
		if (ret != sizeof(seed))
			return ret;
	}

	/* do a reseed of the prno drng with this bytestring */
	cpacf_prno(CPACF_PRNO_SHA512_DRNG_SEED,
		   &prng_data->prnows, NULL, 0, seed, seedlen);
	memzero_explicit(seed, sizeof(seed));

	return 0;
}


static int prng_sha512_generate(u8 *buf, size_t nbytes)
{
	int ret;

	/* reseed needed ? */
	if (prng_data->prnows.reseed_counter > prng_reseed_limit) {
		ret = prng_sha512_reseed();
		if (ret)
			return ret;
	}

	/* PRNO generate */
	cpacf_prno(CPACF_PRNO_SHA512_DRNG_GEN,
		   &prng_data->prnows, buf, nbytes, NULL, 0);

	/* FIPS 140-2 Conditional Self Test */
	if (fips_enabled) {
		if (!memcmp(prng_data->prev, buf, nbytes)) {
			prng_errorflag = PRNG_GEN_FAILED;
			return -EILSEQ;
		}
		memcpy(prng_data->prev, buf, nbytes);
	}

	return nbytes;
}


/*** file io functions ***/

static int prng_open(struct inode *inode, struct file *file)
{
	return nonseekable_open(inode, file);
}


static ssize_t prng_tdes_read(struct file *file, char __user *ubuf,
			      size_t nbytes, loff_t *ppos)
{
	int chunk, n, ret = 0;

	/* lock prng_data struct */
	if (mutex_lock_interruptible(&prng_data->mutex))
		return -ERESTARTSYS;

	while (nbytes) {
		if (need_resched()) {
			if (signal_pending(current)) {
				if (ret == 0)
					ret = -ERESTARTSYS;
				break;
			}
			/* give mutex free before calling schedule() */
			mutex_unlock(&prng_data->mutex);
			schedule();
			/* occopy mutex again */
			if (mutex_lock_interruptible(&prng_data->mutex)) {
				if (ret == 0)
					ret = -ERESTARTSYS;
				return ret;
			}
		}

		/*
		 * we lose some random bytes if an attacker issues
		 * reads < 8 bytes, but we don't care
		 */
		chunk = min_t(int, nbytes, prng_chunk_size);

		/* PRNG only likes multiples of 8 bytes */
		n = (chunk + 7) & -8;

		if (prng_data->prngws.reseed_counter > prng_reseed_limit)
			prng_tdes_seed(8);

		/* if the CPU supports PRNG stckf is present too */
		*((unsigned long long *)prng_data->buf) = get_tod_clock_fast();

		/*
		 * Beside the STCKF the input for the TDES-EDE is the output
		 * of the last operation. We differ here from X9.17 since we
		 * only store one timestamp into the buffer. Padding the whole
		 * buffer with timestamps does not improve security, since
		 * successive stckf have nearly constant offsets.
		 * If an attacker knows the first timestamp it would be
		 * trivial to guess the additional values. One timestamp
		 * is therefore enough and still guarantees unique input values.
		 *
		 * Note: you can still get strict X9.17 conformity by setting
		 * prng_chunk_size to 8 bytes.
		 */
		cpacf_kmc(CPACF_KMC_PRNG, prng_data->prngws.parm_block,
			  prng_data->buf, prng_data->buf, n);

		prng_data->prngws.byte_counter += n;
		prng_data->prngws.reseed_counter += n;

		if (copy_to_user(ubuf, prng_data->buf, chunk)) {
			ret = -EFAULT;
			break;
		}

		nbytes -= chunk;
		ret += chunk;
		ubuf += chunk;
	}

	/* unlock prng_data struct */
	mutex_unlock(&prng_data->mutex);

	return ret;
}


static ssize_t prng_sha512_read(struct file *file, char __user *ubuf,
				size_t nbytes, loff_t *ppos)
{
	int n, ret = 0;
	u8 *p;

	/* if errorflag is set do nothing and return 'broken pipe' */
	if (prng_errorflag)
		return -EPIPE;

	/* lock prng_data struct */
	if (mutex_lock_interruptible(&prng_data->mutex))
		return -ERESTARTSYS;

	while (nbytes) {
		if (need_resched()) {
			if (signal_pending(current)) {
				if (ret == 0)
					ret = -ERESTARTSYS;
				break;
			}
			/* give mutex free before calling schedule() */
			mutex_unlock(&prng_data->mutex);
			schedule();
			/* occopy mutex again */
			if (mutex_lock_interruptible(&prng_data->mutex)) {
				if (ret == 0)
					ret = -ERESTARTSYS;
				return ret;
			}
		}
		if (prng_data->rest) {
			/* push left over random bytes from the previous read */
			p = prng_data->buf + prng_chunk_size - prng_data->rest;
			n = (nbytes < prng_data->rest) ?
				nbytes : prng_data->rest;
			prng_data->rest -= n;
		} else {
			/* generate one chunk of random bytes into read buf */
			p = prng_data->buf;
			n = prng_sha512_generate(p, prng_chunk_size);
			if (n < 0) {
				ret = n;
				break;
			}
			if (nbytes < prng_chunk_size) {
				n = nbytes;
				prng_data->rest = prng_chunk_size - n;
			} else {
				n = prng_chunk_size;
				prng_data->rest = 0;
			}
		}
		if (copy_to_user(ubuf, p, n)) {
			ret = -EFAULT;
			break;
		}
		memzero_explicit(p, n);
		ubuf += n;
		nbytes -= n;
		ret += n;
	}

	/* unlock prng_data struct */
	mutex_unlock(&prng_data->mutex);

	return ret;
}


/*** sysfs stuff ***/

static const struct file_operations prng_sha512_fops = {
	.owner		= THIS_MODULE,
	.open		= &prng_open,
	.release	= NULL,
	.read		= &prng_sha512_read,
	.llseek		= noop_llseek,
};
static const struct file_operations prng_tdes_fops = {
	.owner		= THIS_MODULE,
	.open		= &prng_open,
	.release	= NULL,
	.read		= &prng_tdes_read,
	.llseek		= noop_llseek,
};

/* chunksize attribute (ro) */
static ssize_t prng_chunksize_show(struct device *dev,
				   struct device_attribute *attr,
				   char *buf)
{
	return scnprintf(buf, PAGE_SIZE, "%u\n", prng_chunk_size);
}
static DEVICE_ATTR(chunksize, 0444, prng_chunksize_show, NULL);

/* counter attribute (ro) */
static ssize_t prng_counter_show(struct device *dev,
				 struct device_attribute *attr,
				 char *buf)
{
	u64 counter;

	if (mutex_lock_interruptible(&prng_data->mutex))
		return -ERESTARTSYS;
	if (prng_mode == PRNG_MODE_SHA512)
		counter = prng_data->prnows.stream_bytes;
	else
		counter = prng_data->prngws.byte_counter;
	mutex_unlock(&prng_data->mutex);

	return scnprintf(buf, PAGE_SIZE, "%llu\n", counter);
}
static DEVICE_ATTR(byte_counter, 0444, prng_counter_show, NULL);

/* errorflag attribute (ro) */
static ssize_t prng_errorflag_show(struct device *dev,
				   struct device_attribute *attr,
				   char *buf)
{
	return scnprintf(buf, PAGE_SIZE, "%d\n", prng_errorflag);
}
static DEVICE_ATTR(errorflag, 0444, prng_errorflag_show, NULL);

/* mode attribute (ro) */
static ssize_t prng_mode_show(struct device *dev,
			      struct device_attribute *attr,
			      char *buf)
{
	if (prng_mode == PRNG_MODE_TDES)
		return scnprintf(buf, PAGE_SIZE, "TDES\n");
	else
		return scnprintf(buf, PAGE_SIZE, "SHA512\n");
}
static DEVICE_ATTR(mode, 0444, prng_mode_show, NULL);

/* reseed attribute (w) */
static ssize_t prng_reseed_store(struct device *dev,
				 struct device_attribute *attr,
				 const char *buf, size_t count)
{
	if (mutex_lock_interruptible(&prng_data->mutex))
		return -ERESTARTSYS;
	prng_sha512_reseed();
	mutex_unlock(&prng_data->mutex);

	return count;
}
static DEVICE_ATTR(reseed, 0200, NULL, prng_reseed_store);

/* reseed limit attribute (rw) */
static ssize_t prng_reseed_limit_show(struct device *dev,
				      struct device_attribute *attr,
				      char *buf)
{
	return scnprintf(buf, PAGE_SIZE, "%u\n", prng_reseed_limit);
}
static ssize_t prng_reseed_limit_store(struct device *dev,
				       struct device_attribute *attr,
				       const char *buf, size_t count)
{
	unsigned limit;

	if (sscanf(buf, "%u\n", &limit) != 1)
		return -EINVAL;

	if (prng_mode == PRNG_MODE_SHA512) {
		if (limit < PRNG_RESEED_LIMIT_SHA512_LOWER)
			return -EINVAL;
	} else {
		if (limit < PRNG_RESEED_LIMIT_TDES_LOWER)
			return -EINVAL;
	}

	prng_reseed_limit = limit;

	return count;
}
static DEVICE_ATTR(reseed_limit, 0644,
		   prng_reseed_limit_show, prng_reseed_limit_store);

/* strength attribute (ro) */
static ssize_t prng_strength_show(struct device *dev,
				  struct device_attribute *attr,
				  char *buf)
{
	return scnprintf(buf, PAGE_SIZE, "256\n");
}
static DEVICE_ATTR(strength, 0444, prng_strength_show, NULL);

static struct attribute *prng_sha512_dev_attrs[] = {
	&dev_attr_errorflag.attr,
	&dev_attr_chunksize.attr,
	&dev_attr_byte_counter.attr,
	&dev_attr_mode.attr,
	&dev_attr_reseed.attr,
	&dev_attr_reseed_limit.attr,
	&dev_attr_strength.attr,
	NULL
};
ATTRIBUTE_GROUPS(prng_sha512_dev);

static struct attribute *prng_tdes_dev_attrs[] = {
	&dev_attr_chunksize.attr,
	&dev_attr_byte_counter.attr,
	&dev_attr_mode.attr,
	NULL
};
ATTRIBUTE_GROUPS(prng_tdes_dev);

static struct miscdevice prng_sha512_dev = {
	.name	= "prandom",
	.minor	= MISC_DYNAMIC_MINOR,
	.mode	= 0644,
	.fops	= &prng_sha512_fops,
	.groups = prng_sha512_dev_groups,
};

static struct miscdevice prng_tdes_dev = {
	.name	= "prandom",
	.minor	= MISC_DYNAMIC_MINOR,
	.mode	= 0644,
	.fops	= &prng_tdes_fops,
	.groups = prng_tdes_dev_groups,
};


/*** module init and exit ***/

static int __init prng_init(void)
{
	int ret;

	/* check if the CPU has a PRNG */
	if (!cpacf_query_func(CPACF_KMC, CPACF_KMC_PRNG))
		return -ENODEV;

	/* check if TRNG subfunction is available */
	if (cpacf_query_func(CPACF_PRNO, CPACF_PRNO_TRNG))
		trng_available = true;

	/* choose prng mode */
	if (prng_mode != PRNG_MODE_TDES) {
		/* check for MSA5 support for PRNO operations */
		if (!cpacf_query_func(CPACF_PRNO, CPACF_PRNO_SHA512_DRNG_GEN)) {
			if (prng_mode == PRNG_MODE_SHA512) {
				pr_err("The prng module cannot "
				       "start in SHA-512 mode\n");
				return -ENODEV;
			}
			prng_mode = PRNG_MODE_TDES;
		} else
			prng_mode = PRNG_MODE_SHA512;
	}

	if (prng_mode == PRNG_MODE_SHA512) {

		/* SHA512 mode */

		if (prng_chunk_size < PRNG_CHUNKSIZE_SHA512_MIN
		    || prng_chunk_size > PRNG_CHUNKSIZE_SHA512_MAX)
			return -EINVAL;
		prng_chunk_size = (prng_chunk_size + 0x3f) & ~0x3f;

		if (prng_reseed_limit == 0)
			prng_reseed_limit = PRNG_RESEED_LIMIT_SHA512;
		else if (prng_reseed_limit < PRNG_RESEED_LIMIT_SHA512_LOWER)
			return -EINVAL;

		ret = prng_sha512_instantiate();
		if (ret)
			goto out;

		ret = misc_register(&prng_sha512_dev);
		if (ret) {
			prng_sha512_deinstantiate();
			goto out;
		}

	} else {

		/* TDES mode */

		if (prng_chunk_size < PRNG_CHUNKSIZE_TDES_MIN
		    || prng_chunk_size > PRNG_CHUNKSIZE_TDES_MAX)
			return -EINVAL;
		prng_chunk_size = (prng_chunk_size + 0x07) & ~0x07;

		if (prng_reseed_limit == 0)
			prng_reseed_limit = PRNG_RESEED_LIMIT_TDES;
		else if (prng_reseed_limit < PRNG_RESEED_LIMIT_TDES_LOWER)
			return -EINVAL;

		ret = prng_tdes_instantiate();
		if (ret)
			goto out;

		ret = misc_register(&prng_tdes_dev);
		if (ret) {
			prng_tdes_deinstantiate();
			goto out;
		}
	}

out:
	return ret;
}


static void __exit prng_exit(void)
{
	if (prng_mode == PRNG_MODE_SHA512) {
		misc_deregister(&prng_sha512_dev);
		prng_sha512_deinstantiate();
	} else {
		misc_deregister(&prng_tdes_dev);
		prng_tdes_deinstantiate();
	}
}

module_cpu_feature_match(MSA, prng_init);
module_exit(prng_exit);
