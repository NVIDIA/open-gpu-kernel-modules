/*
 * Non-physical true random number generator based on timing jitter --
 * Jitter RNG standalone code.
 *
 * Copyright Stephan Mueller <smueller@chronox.de>, 2015 - 2020
 *
 * Design
 * ======
 *
 * See https://www.chronox.de/jent.html
 *
 * License
 * =======
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, and the entire permission notice in its entirety,
 *    including the disclaimer of warranties.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * ALTERNATIVELY, this product may be distributed under the terms of
 * the GNU General Public License, in which case the provisions of the GPL2 are
 * required INSTEAD OF the above restrictions.  (This clause is
 * necessary due to a potential bad interaction between the GPL and
 * the restrictions contained in a BSD-style copyright.)
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, ALL OF
 * WHICH ARE HEREBY DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF NOT ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

/*
 * This Jitterentropy RNG is based on the jitterentropy library
 * version 2.2.0 provided at https://www.chronox.de/jent.html
 */

#ifdef __OPTIMIZE__
 #error "The CPU Jitter random number generator must not be compiled with optimizations. See documentation. Use the compiler switch -O0 for compiling jitterentropy.c."
#endif

typedef	unsigned long long	__u64;
typedef	long long		__s64;
typedef	unsigned int		__u32;
#define NULL    ((void *) 0)

/* The entropy pool */
struct rand_data {
	/* all data values that are vital to maintain the security
	 * of the RNG are marked as SENSITIVE. A user must not
	 * access that information while the RNG executes its loops to
	 * calculate the next random value. */
	__u64 data;		/* SENSITIVE Actual random number */
	__u64 old_data;		/* SENSITIVE Previous random number */
	__u64 prev_time;	/* SENSITIVE Previous time stamp */
#define DATA_SIZE_BITS ((sizeof(__u64)) * 8)
	__u64 last_delta;	/* SENSITIVE stuck test */
	__s64 last_delta2;	/* SENSITIVE stuck test */
	unsigned int osr;	/* Oversample rate */
#define JENT_MEMORY_BLOCKS 64
#define JENT_MEMORY_BLOCKSIZE 32
#define JENT_MEMORY_ACCESSLOOPS 128
#define JENT_MEMORY_SIZE (JENT_MEMORY_BLOCKS*JENT_MEMORY_BLOCKSIZE)
	unsigned char *mem;	/* Memory access location with size of
				 * memblocks * memblocksize */
	unsigned int memlocation; /* Pointer to byte in *mem */
	unsigned int memblocks;	/* Number of memory blocks in *mem */
	unsigned int memblocksize; /* Size of one memory block in bytes */
	unsigned int memaccessloops; /* Number of memory accesses per random
				      * bit generation */

	/* Repetition Count Test */
	int rct_count;			/* Number of stuck values */

	/* Adaptive Proportion Test for a significance level of 2^-30 */
#define JENT_APT_CUTOFF		325	/* Taken from SP800-90B sec 4.4.2 */
#define JENT_APT_WINDOW_SIZE	512	/* Data window size */
	/* LSB of time stamp to process */
#define JENT_APT_LSB		16
#define JENT_APT_WORD_MASK	(JENT_APT_LSB - 1)
	unsigned int apt_observations;	/* Number of collected observations */
	unsigned int apt_count;		/* APT counter */
	unsigned int apt_base;		/* APT base reference */
	unsigned int apt_base_set:1;	/* APT base reference set? */

	unsigned int health_failure:1;	/* Permanent health failure */
};

/* Flags that can be used to initialize the RNG */
#define JENT_DISABLE_MEMORY_ACCESS (1<<2) /* Disable memory access for more
					   * entropy, saves MEMORY_SIZE RAM for
					   * entropy collector */

/* -- error codes for init function -- */
#define JENT_ENOTIME		1 /* Timer service not available */
#define JENT_ECOARSETIME	2 /* Timer too coarse for RNG */
#define JENT_ENOMONOTONIC	3 /* Timer is not monotonic increasing */
#define JENT_EVARVAR		5 /* Timer does not produce variations of
				   * variations (2nd derivation of time is
				   * zero). */
#define JENT_ESTUCK		8 /* Too many stuck results during init. */
#define JENT_EHEALTH		9 /* Health test failed during initialization */
#define JENT_ERCT		10 /* RCT failed during initialization */

#include "jitterentropy.h"

/***************************************************************************
 * Adaptive Proportion Test
 *
 * This test complies with SP800-90B section 4.4.2.
 ***************************************************************************/

/**
 * Reset the APT counter
 *
 * @ec [in] Reference to entropy collector
 */
static void jent_apt_reset(struct rand_data *ec, unsigned int delta_masked)
{
	/* Reset APT counter */
	ec->apt_count = 0;
	ec->apt_base = delta_masked;
	ec->apt_observations = 0;
}

/**
 * Insert a new entropy event into APT
 *
 * @ec [in] Reference to entropy collector
 * @delta_masked [in] Masked time delta to process
 */
static void jent_apt_insert(struct rand_data *ec, unsigned int delta_masked)
{
	/* Initialize the base reference */
	if (!ec->apt_base_set) {
		ec->apt_base = delta_masked;
		ec->apt_base_set = 1;
		return;
	}

	if (delta_masked == ec->apt_base) {
		ec->apt_count++;

		if (ec->apt_count >= JENT_APT_CUTOFF)
			ec->health_failure = 1;
	}

	ec->apt_observations++;

	if (ec->apt_observations >= JENT_APT_WINDOW_SIZE)
		jent_apt_reset(ec, delta_masked);
}

/***************************************************************************
 * Stuck Test and its use as Repetition Count Test
 *
 * The Jitter RNG uses an enhanced version of the Repetition Count Test
 * (RCT) specified in SP800-90B section 4.4.1. Instead of counting identical
 * back-to-back values, the input to the RCT is the counting of the stuck
 * values during the generation of one Jitter RNG output block.
 *
 * The RCT is applied with an alpha of 2^{-30} compliant to FIPS 140-2 IG 9.8.
 *
 * During the counting operation, the Jitter RNG always calculates the RCT
 * cut-off value of C. If that value exceeds the allowed cut-off value,
 * the Jitter RNG output block will be calculated completely but discarded at
 * the end. The caller of the Jitter RNG is informed with an error code.
 ***************************************************************************/

/**
 * Repetition Count Test as defined in SP800-90B section 4.4.1
 *
 * @ec [in] Reference to entropy collector
 * @stuck [in] Indicator whether the value is stuck
 */
static void jent_rct_insert(struct rand_data *ec, int stuck)
{
	/*
	 * If we have a count less than zero, a previous RCT round identified
	 * a failure. We will not overwrite it.
	 */
	if (ec->rct_count < 0)
		return;

	if (stuck) {
		ec->rct_count++;

		/*
		 * The cutoff value is based on the following consideration:
		 * alpha = 2^-30 as recommended in FIPS 140-2 IG 9.8.
		 * In addition, we require an entropy value H of 1/OSR as this
		 * is the minimum entropy required to provide full entropy.
		 * Note, we collect 64 * OSR deltas for inserting them into
		 * the entropy pool which should then have (close to) 64 bits
		 * of entropy.
		 *
		 * Note, ec->rct_count (which equals to value B in the pseudo
		 * code of SP800-90B section 4.4.1) starts with zero. Hence
		 * we need to subtract one from the cutoff value as calculated
		 * following SP800-90B.
		 */
		if ((unsigned int)ec->rct_count >= (31 * ec->osr)) {
			ec->rct_count = -1;
			ec->health_failure = 1;
		}
	} else {
		ec->rct_count = 0;
	}
}

/**
 * Is there an RCT health test failure?
 *
 * @ec [in] Reference to entropy collector
 *
 * @return
 * 	0 No health test failure
 * 	1 Permanent health test failure
 */
static int jent_rct_failure(struct rand_data *ec)
{
	if (ec->rct_count < 0)
		return 1;
	return 0;
}

static inline __u64 jent_delta(__u64 prev, __u64 next)
{
#define JENT_UINT64_MAX		(__u64)(~((__u64) 0))
	return (prev < next) ? (next - prev) :
			       (JENT_UINT64_MAX - prev + 1 + next);
}

/**
 * Stuck test by checking the:
 * 	1st derivative of the jitter measurement (time delta)
 * 	2nd derivative of the jitter measurement (delta of time deltas)
 * 	3rd derivative of the jitter measurement (delta of delta of time deltas)
 *
 * All values must always be non-zero.
 *
 * @ec [in] Reference to entropy collector
 * @current_delta [in] Jitter time delta
 *
 * @return
 * 	0 jitter measurement not stuck (good bit)
 * 	1 jitter measurement stuck (reject bit)
 */
static int jent_stuck(struct rand_data *ec, __u64 current_delta)
{
	__u64 delta2 = jent_delta(ec->last_delta, current_delta);
	__u64 delta3 = jent_delta(ec->last_delta2, delta2);
	unsigned int delta_masked = current_delta & JENT_APT_WORD_MASK;

	ec->last_delta = current_delta;
	ec->last_delta2 = delta2;

	/*
	 * Insert the result of the comparison of two back-to-back time
	 * deltas.
	 */
	jent_apt_insert(ec, delta_masked);

	if (!current_delta || !delta2 || !delta3) {
		/* RCT with a stuck bit */
		jent_rct_insert(ec, 1);
		return 1;
	}

	/* RCT with a non-stuck bit */
	jent_rct_insert(ec, 0);

	return 0;
}

/**
 * Report any health test failures
 *
 * @ec [in] Reference to entropy collector
 *
 * @return
 * 	0 No health test failure
 * 	1 Permanent health test failure
 */
static int jent_health_failure(struct rand_data *ec)
{
	/* Test is only enabled in FIPS mode */
	if (!jent_fips_enabled())
		return 0;

	return ec->health_failure;
}

/***************************************************************************
 * Noise sources
 ***************************************************************************/

/**
 * Update of the loop count used for the next round of
 * an entropy collection.
 *
 * Input:
 * @ec entropy collector struct -- may be NULL
 * @bits is the number of low bits of the timer to consider
 * @min is the number of bits we shift the timer value to the right at
 *	the end to make sure we have a guaranteed minimum value
 *
 * @return Newly calculated loop counter
 */
static __u64 jent_loop_shuffle(struct rand_data *ec,
			       unsigned int bits, unsigned int min)
{
	__u64 time = 0;
	__u64 shuffle = 0;
	unsigned int i = 0;
	unsigned int mask = (1<<bits) - 1;

	jent_get_nstime(&time);
	/*
	 * Mix the current state of the random number into the shuffle
	 * calculation to balance that shuffle a bit more.
	 */
	if (ec)
		time ^= ec->data;
	/*
	 * We fold the time value as much as possible to ensure that as many
	 * bits of the time stamp are included as possible.
	 */
	for (i = 0; ((DATA_SIZE_BITS + bits - 1) / bits) > i; i++) {
		shuffle ^= time & mask;
		time = time >> bits;
	}

	/*
	 * We add a lower boundary value to ensure we have a minimum
	 * RNG loop count.
	 */
	return (shuffle + (1<<min));
}

/**
 * CPU Jitter noise source -- this is the noise source based on the CPU
 *			      execution time jitter
 *
 * This function injects the individual bits of the time value into the
 * entropy pool using an LFSR.
 *
 * The code is deliberately inefficient with respect to the bit shifting
 * and shall stay that way. This function is the root cause why the code
 * shall be compiled without optimization. This function not only acts as
 * folding operation, but this function's execution is used to measure
 * the CPU execution time jitter. Any change to the loop in this function
 * implies that careful retesting must be done.
 *
 * @ec [in] entropy collector struct
 * @time [in] time stamp to be injected
 * @loop_cnt [in] if a value not equal to 0 is set, use the given value as
 *		  number of loops to perform the folding
 * @stuck [in] Is the time stamp identified as stuck?
 *
 * Output:
 * updated ec->data
 *
 * @return Number of loops the folding operation is performed
 */
static void jent_lfsr_time(struct rand_data *ec, __u64 time, __u64 loop_cnt,
			   int stuck)
{
	unsigned int i;
	__u64 j = 0;
	__u64 new = 0;
#define MAX_FOLD_LOOP_BIT 4
#define MIN_FOLD_LOOP_BIT 0
	__u64 fold_loop_cnt =
		jent_loop_shuffle(ec, MAX_FOLD_LOOP_BIT, MIN_FOLD_LOOP_BIT);

	/*
	 * testing purposes -- allow test app to set the counter, not
	 * needed during runtime
	 */
	if (loop_cnt)
		fold_loop_cnt = loop_cnt;
	for (j = 0; j < fold_loop_cnt; j++) {
		new = ec->data;
		for (i = 1; (DATA_SIZE_BITS) >= i; i++) {
			__u64 tmp = time << (DATA_SIZE_BITS - i);

			tmp = tmp >> (DATA_SIZE_BITS - 1);

			/*
			* Fibonacci LSFR with polynomial of
			*  x^64 + x^61 + x^56 + x^31 + x^28 + x^23 + 1 which is
			*  primitive according to
			*   http://poincare.matf.bg.ac.rs/~ezivkovm/publications/primpol1.pdf
			* (the shift values are the polynomial values minus one
			* due to counting bits from 0 to 63). As the current
			* position is always the LSB, the polynomial only needs
			* to shift data in from the left without wrap.
			*/
			tmp ^= ((new >> 63) & 1);
			tmp ^= ((new >> 60) & 1);
			tmp ^= ((new >> 55) & 1);
			tmp ^= ((new >> 30) & 1);
			tmp ^= ((new >> 27) & 1);
			tmp ^= ((new >> 22) & 1);
			new <<= 1;
			new ^= tmp;
		}
	}

	/*
	 * If the time stamp is stuck, do not finally insert the value into
	 * the entropy pool. Although this operation should not do any harm
	 * even when the time stamp has no entropy, SP800-90B requires that
	 * any conditioning operation (SP800-90B considers the LFSR to be a
	 * conditioning operation) to have an identical amount of input
	 * data according to section 3.1.5.
	 */
	if (!stuck)
		ec->data = new;
}

/**
 * Memory Access noise source -- this is a noise source based on variations in
 *				 memory access times
 *
 * This function performs memory accesses which will add to the timing
 * variations due to an unknown amount of CPU wait states that need to be
 * added when accessing memory. The memory size should be larger than the L1
 * caches as outlined in the documentation and the associated testing.
 *
 * The L1 cache has a very high bandwidth, albeit its access rate is  usually
 * slower than accessing CPU registers. Therefore, L1 accesses only add minimal
 * variations as the CPU has hardly to wait. Starting with L2, significant
 * variations are added because L2 typically does not belong to the CPU any more
 * and therefore a wider range of CPU wait states is necessary for accesses.
 * L3 and real memory accesses have even a wider range of wait states. However,
 * to reliably access either L3 or memory, the ec->mem memory must be quite
 * large which is usually not desirable.
 *
 * @ec [in] Reference to the entropy collector with the memory access data -- if
 *	    the reference to the memory block to be accessed is NULL, this noise
 *	    source is disabled
 * @loop_cnt [in] if a value not equal to 0 is set, use the given value
 *		  number of loops to perform the LFSR
 */
static void jent_memaccess(struct rand_data *ec, __u64 loop_cnt)
{
	unsigned int wrap = 0;
	__u64 i = 0;
#define MAX_ACC_LOOP_BIT 7
#define MIN_ACC_LOOP_BIT 0
	__u64 acc_loop_cnt =
		jent_loop_shuffle(ec, MAX_ACC_LOOP_BIT, MIN_ACC_LOOP_BIT);

	if (NULL == ec || NULL == ec->mem)
		return;
	wrap = ec->memblocksize * ec->memblocks;

	/*
	 * testing purposes -- allow test app to set the counter, not
	 * needed during runtime
	 */
	if (loop_cnt)
		acc_loop_cnt = loop_cnt;

	for (i = 0; i < (ec->memaccessloops + acc_loop_cnt); i++) {
		unsigned char *tmpval = ec->mem + ec->memlocation;
		/*
		 * memory access: just add 1 to one byte,
		 * wrap at 255 -- memory access implies read
		 * from and write to memory location
		 */
		*tmpval = (*tmpval + 1) & 0xff;
		/*
		 * Addition of memblocksize - 1 to pointer
		 * with wrap around logic to ensure that every
		 * memory location is hit evenly
		 */
		ec->memlocation = ec->memlocation + ec->memblocksize - 1;
		ec->memlocation = ec->memlocation % wrap;
	}
}

/***************************************************************************
 * Start of entropy processing logic
 ***************************************************************************/
/**
 * This is the heart of the entropy generation: calculate time deltas and
 * use the CPU jitter in the time deltas. The jitter is injected into the
 * entropy pool.
 *
 * WARNING: ensure that ->prev_time is primed before using the output
 *	    of this function! This can be done by calling this function
 *	    and not using its result.
 *
 * @ec [in] Reference to entropy collector
 *
 * @return result of stuck test
 */
static int jent_measure_jitter(struct rand_data *ec)
{
	__u64 time = 0;
	__u64 current_delta = 0;
	int stuck;

	/* Invoke one noise source before time measurement to add variations */
	jent_memaccess(ec, 0);

	/*
	 * Get time stamp and calculate time delta to previous
	 * invocation to measure the timing variations
	 */
	jent_get_nstime(&time);
	current_delta = jent_delta(ec->prev_time, time);
	ec->prev_time = time;

	/* Check whether we have a stuck measurement. */
	stuck = jent_stuck(ec, current_delta);

	/* Now call the next noise sources which also injects the data */
	jent_lfsr_time(ec, current_delta, 0, stuck);

	return stuck;
}

/**
 * Generator of one 64 bit random number
 * Function fills rand_data->data
 *
 * @ec [in] Reference to entropy collector
 */
static void jent_gen_entropy(struct rand_data *ec)
{
	unsigned int k = 0;

	/* priming of the ->prev_time value */
	jent_measure_jitter(ec);

	while (1) {
		/* If a stuck measurement is received, repeat measurement */
		if (jent_measure_jitter(ec))
			continue;

		/*
		 * We multiply the loop value with ->osr to obtain the
		 * oversampling rate requested by the caller
		 */
		if (++k >= (DATA_SIZE_BITS * ec->osr))
			break;
	}
}

/**
 * Entry function: Obtain entropy for the caller.
 *
 * This function invokes the entropy gathering logic as often to generate
 * as many bytes as requested by the caller. The entropy gathering logic
 * creates 64 bit per invocation.
 *
 * This function truncates the last 64 bit entropy value output to the exact
 * size specified by the caller.
 *
 * @ec [in] Reference to entropy collector
 * @data [in] pointer to buffer for storing random data -- buffer must already
 *	      exist
 * @len [in] size of the buffer, specifying also the requested number of random
 *	     in bytes
 *
 * @return 0 when request is fulfilled or an error
 *
 * The following error codes can occur:
 *	-1	entropy_collector is NULL
 *	-2	RCT failed
 *	-3	APT test failed
 */
int jent_read_entropy(struct rand_data *ec, unsigned char *data,
		      unsigned int len)
{
	unsigned char *p = data;

	if (!ec)
		return -1;

	while (len > 0) {
		unsigned int tocopy;

		jent_gen_entropy(ec);

		if (jent_health_failure(ec)) {
			int ret;

			if (jent_rct_failure(ec))
				ret = -2;
			else
				ret = -3;

			/*
			 * Re-initialize the noise source
			 *
			 * If the health test fails, the Jitter RNG remains
			 * in failure state and will return a health failure
			 * during next invocation.
			 */
			if (jent_entropy_init())
				return ret;

			/* Set APT to initial state */
			jent_apt_reset(ec, 0);
			ec->apt_base_set = 0;

			/* Set RCT to initial state */
			ec->rct_count = 0;

			/* Re-enable Jitter RNG */
			ec->health_failure = 0;

			/*
			 * Return the health test failure status to the
			 * caller as the generated value is not appropriate.
			 */
			return ret;
		}

		if ((DATA_SIZE_BITS / 8) < len)
			tocopy = (DATA_SIZE_BITS / 8);
		else
			tocopy = len;
		jent_memcpy(p, &ec->data, tocopy);

		len -= tocopy;
		p += tocopy;
	}

	return 0;
}

/***************************************************************************
 * Initialization logic
 ***************************************************************************/

struct rand_data *jent_entropy_collector_alloc(unsigned int osr,
					       unsigned int flags)
{
	struct rand_data *entropy_collector;

	entropy_collector = jent_zalloc(sizeof(struct rand_data));
	if (!entropy_collector)
		return NULL;

	if (!(flags & JENT_DISABLE_MEMORY_ACCESS)) {
		/* Allocate memory for adding variations based on memory
		 * access
		 */
		entropy_collector->mem = jent_zalloc(JENT_MEMORY_SIZE);
		if (!entropy_collector->mem) {
			jent_zfree(entropy_collector);
			return NULL;
		}
		entropy_collector->memblocksize = JENT_MEMORY_BLOCKSIZE;
		entropy_collector->memblocks = JENT_MEMORY_BLOCKS;
		entropy_collector->memaccessloops = JENT_MEMORY_ACCESSLOOPS;
	}

	/* verify and set the oversampling rate */
	if (osr == 0)
		osr = 1; /* minimum sampling rate is 1 */
	entropy_collector->osr = osr;

	/* fill the data pad with non-zero values */
	jent_gen_entropy(entropy_collector);

	return entropy_collector;
}

void jent_entropy_collector_free(struct rand_data *entropy_collector)
{
	jent_zfree(entropy_collector->mem);
	entropy_collector->mem = NULL;
	jent_zfree(entropy_collector);
}

int jent_entropy_init(void)
{
	int i;
	__u64 delta_sum = 0;
	__u64 old_delta = 0;
	unsigned int nonstuck = 0;
	int time_backwards = 0;
	int count_mod = 0;
	int count_stuck = 0;
	struct rand_data ec = { 0 };

	/* Required for RCT */
	ec.osr = 1;

	/* We could perform statistical tests here, but the problem is
	 * that we only have a few loop counts to do testing. These
	 * loop counts may show some slight skew and we produce
	 * false positives.
	 *
	 * Moreover, only old systems show potentially problematic
	 * jitter entropy that could potentially be caught here. But
	 * the RNG is intended for hardware that is available or widely
	 * used, but not old systems that are long out of favor. Thus,
	 * no statistical tests.
	 */

	/*
	 * We could add a check for system capabilities such as clock_getres or
	 * check for CONFIG_X86_TSC, but it does not make much sense as the
	 * following sanity checks verify that we have a high-resolution
	 * timer.
	 */
	/*
	 * TESTLOOPCOUNT needs some loops to identify edge systems. 100 is
	 * definitely too little.
	 *
	 * SP800-90B requires at least 1024 initial test cycles.
	 */
#define TESTLOOPCOUNT 1024
#define CLEARCACHE 100
	for (i = 0; (TESTLOOPCOUNT + CLEARCACHE) > i; i++) {
		__u64 time = 0;
		__u64 time2 = 0;
		__u64 delta = 0;
		unsigned int lowdelta = 0;
		int stuck;

		/* Invoke core entropy collection logic */
		jent_get_nstime(&time);
		ec.prev_time = time;
		jent_lfsr_time(&ec, time, 0, 0);
		jent_get_nstime(&time2);

		/* test whether timer works */
		if (!time || !time2)
			return JENT_ENOTIME;
		delta = jent_delta(time, time2);
		/*
		 * test whether timer is fine grained enough to provide
		 * delta even when called shortly after each other -- this
		 * implies that we also have a high resolution timer
		 */
		if (!delta)
			return JENT_ECOARSETIME;

		stuck = jent_stuck(&ec, delta);

		/*
		 * up to here we did not modify any variable that will be
		 * evaluated later, but we already performed some work. Thus we
		 * already have had an impact on the caches, branch prediction,
		 * etc. with the goal to clear it to get the worst case
		 * measurements.
		 */
		if (i < CLEARCACHE)
			continue;

		if (stuck)
			count_stuck++;
		else {
			nonstuck++;

			/*
			 * Ensure that the APT succeeded.
			 *
			 * With the check below that count_stuck must be less
			 * than 10% of the overall generated raw entropy values
			 * it is guaranteed that the APT is invoked at
			 * floor((TESTLOOPCOUNT * 0.9) / 64) == 14 times.
			 */
			if ((nonstuck % JENT_APT_WINDOW_SIZE) == 0) {
				jent_apt_reset(&ec,
					       delta & JENT_APT_WORD_MASK);
				if (jent_health_failure(&ec))
					return JENT_EHEALTH;
			}
		}

		/* Validate RCT */
		if (jent_rct_failure(&ec))
			return JENT_ERCT;

		/* test whether we have an increasing timer */
		if (!(time2 > time))
			time_backwards++;

		/* use 32 bit value to ensure compilation on 32 bit arches */
		lowdelta = time2 - time;
		if (!(lowdelta % 100))
			count_mod++;

		/*
		 * ensure that we have a varying delta timer which is necessary
		 * for the calculation of entropy -- perform this check
		 * only after the first loop is executed as we need to prime
		 * the old_data value
		 */
		if (delta > old_delta)
			delta_sum += (delta - old_delta);
		else
			delta_sum += (old_delta - delta);
		old_delta = delta;
	}

	/*
	 * we allow up to three times the time running backwards.
	 * CLOCK_REALTIME is affected by adjtime and NTP operations. Thus,
	 * if such an operation just happens to interfere with our test, it
	 * should not fail. The value of 3 should cover the NTP case being
	 * performed during our test run.
	 */
	if (time_backwards > 3)
		return JENT_ENOMONOTONIC;

	/*
	 * Variations of deltas of time must on average be larger
	 * than 1 to ensure the entropy estimation
	 * implied with 1 is preserved
	 */
	if ((delta_sum) <= 1)
		return JENT_EVARVAR;

	/*
	 * Ensure that we have variations in the time stamp below 10 for at
	 * least 10% of all checks -- on some platforms, the counter increments
	 * in multiples of 100, but not always
	 */
	if ((TESTLOOPCOUNT/10 * 9) < count_mod)
		return JENT_ECOARSETIME;

	/*
	 * If we have more than 90% stuck results, then this Jitter RNG is
	 * likely to not work well.
	 */
	if ((TESTLOOPCOUNT/10 * 9) < count_stuck)
		return JENT_ESTUCK;

	return 0;
}
