// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2006-2009 Simtec Electronics
 *	http://armlinux.simtec.co.uk/
 *	Ben Dooks <ben@simtec.co.uk>
 *	Vincent Sanders <vince@simtec.co.uk>
 *
 * S3C2440/S3C2442 CPU Frequency scaling
*/

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/cpufreq.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/soc/samsung/s3c-cpufreq-core.h>
#include <linux/soc/samsung/s3c-pm.h>

#include <asm/mach/arch.h>
#include <asm/mach/map.h>

#define S3C2440_CLKDIVN_PDIVN	     (1<<0)
#define S3C2440_CLKDIVN_HDIVN_MASK   (3<<1)
#define S3C2440_CLKDIVN_HDIVN_1      (0<<1)
#define S3C2440_CLKDIVN_HDIVN_2      (1<<1)
#define S3C2440_CLKDIVN_HDIVN_4_8    (2<<1)
#define S3C2440_CLKDIVN_HDIVN_3_6    (3<<1)
#define S3C2440_CLKDIVN_UCLK         (1<<3)

#define S3C2440_CAMDIVN_CAMCLK_MASK  (0xf<<0)
#define S3C2440_CAMDIVN_CAMCLK_SEL   (1<<4)
#define S3C2440_CAMDIVN_HCLK3_HALF   (1<<8)
#define S3C2440_CAMDIVN_HCLK4_HALF   (1<<9)
#define S3C2440_CAMDIVN_DVSEN        (1<<12)

#define S3C2442_CAMDIVN_CAMCLK_DIV3  (1<<5)

static struct clk *xtal;
static struct clk *fclk;
static struct clk *hclk;
static struct clk *armclk;

/* HDIV: 1, 2, 3, 4, 6, 8 */

static inline int within_khz(unsigned long a, unsigned long b)
{
	long diff = a - b;

	return (diff >= -1000 && diff <= 1000);
}

/**
 * s3c2440_cpufreq_calcdivs - calculate divider settings
 * @cfg: The cpu frequency settings.
 *
 * Calcualte the divider values for the given frequency settings
 * specified in @cfg. The values are stored in @cfg for later use
 * by the relevant set routine if the request settings can be reached.
 */
static int s3c2440_cpufreq_calcdivs(struct s3c_cpufreq_config *cfg)
{
	unsigned int hdiv, pdiv;
	unsigned long hclk, fclk, armclk;
	unsigned long hclk_max;

	fclk = cfg->freq.fclk;
	armclk = cfg->freq.armclk;
	hclk_max = cfg->max.hclk;

	s3c_freq_dbg("%s: fclk is %lu, armclk %lu, max hclk %lu\n",
		     __func__, fclk, armclk, hclk_max);

	if (armclk > fclk) {
		pr_warn("%s: armclk > fclk\n", __func__);
		armclk = fclk;
	}

	/* if we are in DVS, we need HCLK to be <= ARMCLK */
	if (armclk < fclk && armclk < hclk_max)
		hclk_max = armclk;

	for (hdiv = 1; hdiv < 9; hdiv++) {
		if (hdiv == 5 || hdiv == 7)
			hdiv++;

		hclk = (fclk / hdiv);
		if (hclk <= hclk_max || within_khz(hclk, hclk_max))
			break;
	}

	s3c_freq_dbg("%s: hclk %lu, div %d\n", __func__, hclk, hdiv);

	if (hdiv > 8)
		goto invalid;

	pdiv = (hclk > cfg->max.pclk) ? 2 : 1;

	if ((hclk / pdiv) > cfg->max.pclk)
		pdiv++;

	s3c_freq_dbg("%s: pdiv %d\n", __func__, pdiv);

	if (pdiv > 2)
		goto invalid;

	pdiv *= hdiv;

	/* calculate a valid armclk */

	if (armclk < hclk)
		armclk = hclk;

	/* if we're running armclk lower than fclk, this really means
	 * that the system should go into dvs mode, which means that
	 * armclk is connected to hclk. */
	if (armclk < fclk) {
		cfg->divs.dvs = 1;
		armclk = hclk;
	} else
		cfg->divs.dvs = 0;

	cfg->freq.armclk = armclk;

	/* store the result, and then return */

	cfg->divs.h_divisor = hdiv;
	cfg->divs.p_divisor = pdiv;

	return 0;

 invalid:
	return -EINVAL;
}

#define CAMDIVN_HCLK_HALF (S3C2440_CAMDIVN_HCLK3_HALF | \
			   S3C2440_CAMDIVN_HCLK4_HALF)

/**
 * s3c2440_cpufreq_setdivs - set the cpu frequency divider settings
 * @cfg: The cpu frequency settings.
 *
 * Set the divisors from the settings in @cfg, which where generated
 * during the calculation phase by s3c2440_cpufreq_calcdivs().
 */
static void s3c2440_cpufreq_setdivs(struct s3c_cpufreq_config *cfg)
{
	unsigned long clkdiv, camdiv;

	s3c_freq_dbg("%s: divisors: h=%d, p=%d\n", __func__,
		     cfg->divs.h_divisor, cfg->divs.p_divisor);

	clkdiv = s3c24xx_read_clkdivn();
	camdiv = s3c2440_read_camdivn();

	clkdiv &= ~(S3C2440_CLKDIVN_HDIVN_MASK | S3C2440_CLKDIVN_PDIVN);
	camdiv &= ~CAMDIVN_HCLK_HALF;

	switch (cfg->divs.h_divisor) {
	case 1:
		clkdiv |= S3C2440_CLKDIVN_HDIVN_1;
		break;

	case 2:
		clkdiv |= S3C2440_CLKDIVN_HDIVN_2;
		break;

	case 6:
		camdiv |= S3C2440_CAMDIVN_HCLK3_HALF;
	case 3:
		clkdiv |= S3C2440_CLKDIVN_HDIVN_3_6;
		break;

	case 8:
		camdiv |= S3C2440_CAMDIVN_HCLK4_HALF;
	case 4:
		clkdiv |= S3C2440_CLKDIVN_HDIVN_4_8;
		break;

	default:
		BUG();	/* we don't expect to get here. */
	}

	if (cfg->divs.p_divisor != cfg->divs.h_divisor)
		clkdiv |= S3C2440_CLKDIVN_PDIVN;

	/* todo - set pclk. */

	/* Write the divisors first with hclk intentionally halved so that
	 * when we write clkdiv we will under-frequency instead of over. We
	 * then make a short delay and remove the hclk halving if necessary.
	 */

	s3c2440_write_camdivn(camdiv | CAMDIVN_HCLK_HALF);
	s3c24xx_write_clkdivn(clkdiv);

	ndelay(20);
	s3c2440_write_camdivn(camdiv);

	clk_set_parent(armclk, cfg->divs.dvs ? hclk : fclk);
}

static int run_freq_for(unsigned long max_hclk, unsigned long fclk,
			int *divs,
			struct cpufreq_frequency_table *table,
			size_t table_size)
{
	unsigned long freq;
	int index = 0;
	int div;

	for (div = *divs; div > 0; div = *divs++) {
		freq = fclk / div;

		if (freq > max_hclk && div != 1)
			continue;

		freq /= 1000; /* table is in kHz */
		index = s3c_cpufreq_addfreq(table, index, table_size, freq);
		if (index < 0)
			break;
	}

	return index;
}

static int hclk_divs[] = { 1, 2, 3, 4, 6, 8, -1 };

static int s3c2440_cpufreq_calctable(struct s3c_cpufreq_config *cfg,
				     struct cpufreq_frequency_table *table,
				     size_t table_size)
{
	int ret;

	WARN_ON(cfg->info == NULL);
	WARN_ON(cfg->board == NULL);

	ret = run_freq_for(cfg->info->max.hclk,
			   cfg->info->max.fclk,
			   hclk_divs,
			   table, table_size);

	s3c_freq_dbg("%s: returning %d\n", __func__, ret);

	return ret;
}

static struct s3c_cpufreq_info s3c2440_cpufreq_info = {
	.max		= {
		.fclk	= 400000000,
		.hclk	= 133333333,
		.pclk	=  66666666,
	},

	.locktime_m	= 300,
	.locktime_u	= 300,
	.locktime_bits	= 16,

	.name		= "s3c244x",
	.calc_iotiming	= s3c2410_iotiming_calc,
	.set_iotiming	= s3c2410_iotiming_set,
	.get_iotiming	= s3c2410_iotiming_get,
	.set_fvco	= s3c2410_set_fvco,

	.set_refresh	= s3c2410_cpufreq_setrefresh,
	.set_divs	= s3c2440_cpufreq_setdivs,
	.calc_divs	= s3c2440_cpufreq_calcdivs,
	.calc_freqtable	= s3c2440_cpufreq_calctable,

	.debug_io_show  = s3c_cpufreq_debugfs_call(s3c2410_iotiming_debugfs),
};

static int s3c2440_cpufreq_add(struct device *dev,
			       struct subsys_interface *sif)
{
	xtal = s3c_cpufreq_clk_get(NULL, "xtal");
	hclk = s3c_cpufreq_clk_get(NULL, "hclk");
	fclk = s3c_cpufreq_clk_get(NULL, "fclk");
	armclk = s3c_cpufreq_clk_get(NULL, "armclk");

	if (IS_ERR(xtal) || IS_ERR(hclk) || IS_ERR(fclk) || IS_ERR(armclk)) {
		pr_err("%s: failed to get clocks\n", __func__);
		return -ENOENT;
	}

	return s3c_cpufreq_register(&s3c2440_cpufreq_info);
}

static struct subsys_interface s3c2440_cpufreq_interface = {
	.name		= "s3c2440_cpufreq",
	.subsys		= &s3c2440_subsys,
	.add_dev	= s3c2440_cpufreq_add,
};

static int s3c2440_cpufreq_init(void)
{
	return subsys_interface_register(&s3c2440_cpufreq_interface);
}

/* arch_initcall adds the clocks we need, so use subsys_initcall. */
subsys_initcall(s3c2440_cpufreq_init);

static struct subsys_interface s3c2442_cpufreq_interface = {
	.name		= "s3c2442_cpufreq",
	.subsys		= &s3c2442_subsys,
	.add_dev	= s3c2440_cpufreq_add,
};

static int s3c2442_cpufreq_init(void)
{
	return subsys_interface_register(&s3c2442_cpufreq_interface);
}
subsys_initcall(s3c2442_cpufreq_init);
