/*
 * RNG driver for VIA RNGs
 *
 * Copyright 2005 (c) MontaVista Software, Inc.
 *
 * with the majority of the code coming from:
 *
 * Hardware driver for the Intel/AMD/VIA Random Number Generators (RNG)
 * (c) Copyright 2003 Red Hat Inc <jgarzik@redhat.com>
 *
 * derived from
 *
 * Hardware driver for the AMD 768 Random Number Generator (RNG)
 * (c) Copyright 2001 Red Hat Inc
 *
 * derived from
 *
 * Hardware driver for Intel i810 Random Number Generator (RNG)
 * Copyright 2000,2001 Jeff Garzik <jgarzik@pobox.com>
 * Copyright 2000,2001 Philipp Rumpf <prumpf@mandrakesoft.com>
 *
 * This file is licensed under  the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <crypto/padlock.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/hw_random.h>
#include <linux/delay.h>
#include <asm/cpu_device_id.h>
#include <asm/io.h>
#include <asm/msr.h>
#include <asm/cpufeature.h>
#include <asm/fpu/api.h>




enum {
	VIA_STRFILT_CNT_SHIFT	= 16,
	VIA_STRFILT_FAIL	= (1 << 15),
	VIA_STRFILT_ENABLE	= (1 << 14),
	VIA_RAWBITS_ENABLE	= (1 << 13),
	VIA_RNG_ENABLE		= (1 << 6),
	VIA_NOISESRC1		= (1 << 8),
	VIA_NOISESRC2		= (1 << 9),
	VIA_XSTORE_CNT_MASK	= 0x0F,

	VIA_RNG_CHUNK_8		= 0x00,	/* 64 rand bits, 64 stored bits */
	VIA_RNG_CHUNK_4		= 0x01,	/* 32 rand bits, 32 stored bits */
	VIA_RNG_CHUNK_4_MASK	= 0xFFFFFFFF,
	VIA_RNG_CHUNK_2		= 0x02,	/* 16 rand bits, 32 stored bits */
	VIA_RNG_CHUNK_2_MASK	= 0xFFFF,
	VIA_RNG_CHUNK_1		= 0x03,	/* 8 rand bits, 32 stored bits */
	VIA_RNG_CHUNK_1_MASK	= 0xFF,
};

/*
 * Investigate using the 'rep' prefix to obtain 32 bits of random data
 * in one insn.  The upside is potentially better performance.  The
 * downside is that the instruction becomes no longer atomic.  Due to
 * this, just like familiar issues with /dev/random itself, the worst
 * case of a 'rep xstore' could potentially pause a cpu for an
 * unreasonably long time.  In practice, this condition would likely
 * only occur when the hardware is failing.  (or so we hope :))
 *
 * Another possible performance boost may come from simply buffering
 * until we have 4 bytes, thus returning a u32 at a time,
 * instead of the current u8-at-a-time.
 *
 * Padlock instructions can generate a spurious DNA fault, but the
 * kernel doesn't use CR0.TS, so this doesn't matter.
 */

static inline u32 xstore(u32 *addr, u32 edx_in)
{
	u32 eax_out;

	asm(".byte 0x0F,0xA7,0xC0 /* xstore %%edi (addr=%0) */"
		: "=m" (*addr), "=a" (eax_out), "+d" (edx_in), "+D" (addr));

	return eax_out;
}

static int via_rng_data_present(struct hwrng *rng, int wait)
{
	char buf[16 + PADLOCK_ALIGNMENT - STACK_ALIGN] __attribute__
		((aligned(STACK_ALIGN)));
	u32 *via_rng_datum = (u32 *)PTR_ALIGN(&buf[0], PADLOCK_ALIGNMENT);
	u32 bytes_out;
	int i;

	/* We choose the recommended 1-byte-per-instruction RNG rate,
	 * for greater randomness at the expense of speed.  Larger
	 * values 2, 4, or 8 bytes-per-instruction yield greater
	 * speed at lesser randomness.
	 *
	 * If you change this to another VIA_CHUNK_n, you must also
	 * change the ->n_bytes values in rng_vendor_ops[] tables.
	 * VIA_CHUNK_8 requires further code changes.
	 *
	 * A copy of MSR_VIA_RNG is placed in eax_out when xstore
	 * completes.
	 */

	for (i = 0; i < 20; i++) {
		*via_rng_datum = 0; /* paranoia, not really necessary */
		bytes_out = xstore(via_rng_datum, VIA_RNG_CHUNK_1);
		bytes_out &= VIA_XSTORE_CNT_MASK;
		if (bytes_out || !wait)
			break;
		udelay(10);
	}
	rng->priv = *via_rng_datum;
	return bytes_out ? 1 : 0;
}

static int via_rng_data_read(struct hwrng *rng, u32 *data)
{
	u32 via_rng_datum = (u32)rng->priv;

	*data = via_rng_datum;

	return 1;
}

static int via_rng_init(struct hwrng *rng)
{
	struct cpuinfo_x86 *c = &cpu_data(0);
	u32 lo, hi, old_lo;

	/* VIA Nano CPUs don't have the MSR_VIA_RNG anymore.  The RNG
	 * is always enabled if CPUID rng_en is set.  There is no
	 * RNG configuration like it used to be the case in this
	 * register */
	if (((c->x86 == 6) && (c->x86_model >= 0x0f))  || (c->x86 > 6)){
		if (!boot_cpu_has(X86_FEATURE_XSTORE_EN)) {
			pr_err(PFX "can't enable hardware RNG "
				"if XSTORE is not enabled\n");
			return -ENODEV;
		}
		return 0;
	}

	/* Control the RNG via MSR.  Tread lightly and pay very close
	 * close attention to values written, as the reserved fields
	 * are documented to be "undefined and unpredictable"; but it
	 * does not say to write them as zero, so I make a guess that
	 * we restore the values we find in the register.
	 */
	rdmsr(MSR_VIA_RNG, lo, hi);

	old_lo = lo;
	lo &= ~(0x7f << VIA_STRFILT_CNT_SHIFT);
	lo &= ~VIA_XSTORE_CNT_MASK;
	lo &= ~(VIA_STRFILT_ENABLE | VIA_STRFILT_FAIL | VIA_RAWBITS_ENABLE);
	lo |= VIA_RNG_ENABLE;
	lo |= VIA_NOISESRC1;

	/* Enable secondary noise source on CPUs where it is present. */

	/* Nehemiah stepping 8 and higher */
	if ((c->x86_model == 9) && (c->x86_stepping > 7))
		lo |= VIA_NOISESRC2;

	/* Esther */
	if (c->x86_model >= 10)
		lo |= VIA_NOISESRC2;

	if (lo != old_lo)
		wrmsr(MSR_VIA_RNG, lo, hi);

	/* perhaps-unnecessary sanity check; remove after testing if
	   unneeded */
	rdmsr(MSR_VIA_RNG, lo, hi);
	if ((lo & VIA_RNG_ENABLE) == 0) {
		pr_err(PFX "cannot enable VIA C3 RNG, aborting\n");
		return -ENODEV;
	}

	return 0;
}


static struct hwrng via_rng = {
	.name		= "via",
	.init		= via_rng_init,
	.data_present	= via_rng_data_present,
	.data_read	= via_rng_data_read,
};


static int __init mod_init(void)
{
	int err;

	if (!boot_cpu_has(X86_FEATURE_XSTORE))
		return -ENODEV;

	pr_info("VIA RNG detected\n");
	err = hwrng_register(&via_rng);
	if (err) {
		pr_err(PFX "RNG registering failed (%d)\n",
		       err);
		goto out;
	}
out:
	return err;
}
module_init(mod_init);

static void __exit mod_exit(void)
{
	hwrng_unregister(&via_rng);
}
module_exit(mod_exit);

static struct x86_cpu_id __maybe_unused via_rng_cpu_id[] = {
	X86_MATCH_FEATURE(X86_FEATURE_XSTORE, NULL),
	{}
};
MODULE_DEVICE_TABLE(x86cpu, via_rng_cpu_id);

MODULE_DESCRIPTION("H/W RNG driver for VIA CPU with PadLock");
MODULE_LICENSE("GPL");
