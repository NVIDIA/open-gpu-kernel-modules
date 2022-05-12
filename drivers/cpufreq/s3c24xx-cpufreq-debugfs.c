// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2009 Simtec Electronics
 *	http://armlinux.simtec.co.uk/
 *	Ben Dooks <ben@simtec.co.uk>
 *
 * S3C24XX CPU Frequency scaling - debugfs status support
*/

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/init.h>
#include <linux/export.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/cpufreq.h>
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <linux/err.h>

#include <linux/soc/samsung/s3c-cpufreq-core.h>

static struct dentry *dbgfs_root;
static struct dentry *dbgfs_file_io;
static struct dentry *dbgfs_file_info;
static struct dentry *dbgfs_file_board;

#define print_ns(x) ((x) / 10), ((x) % 10)

static void show_max(struct seq_file *seq, struct s3c_freq *f)
{
	seq_printf(seq, "MAX: F=%lu, H=%lu, P=%lu, A=%lu\n",
		   f->fclk, f->hclk, f->pclk, f->armclk);
}

static int board_show(struct seq_file *seq, void *p)
{
	struct s3c_cpufreq_config *cfg;
	struct s3c_cpufreq_board *brd;

	cfg = s3c_cpufreq_getconfig();
	if (!cfg) {
		seq_printf(seq, "no configuration registered\n");
		return 0;
	}

	brd = cfg->board;
	if (!brd) {
		seq_printf(seq, "no board definition set?\n");
		return 0;
	}

	seq_printf(seq, "SDRAM refresh %u ns\n", brd->refresh);
	seq_printf(seq, "auto_io=%u\n", brd->auto_io);
	seq_printf(seq, "need_io=%u\n", brd->need_io);

	show_max(seq, &brd->max);


	return 0;
}

DEFINE_SHOW_ATTRIBUTE(board);

static int info_show(struct seq_file *seq, void *p)
{
	struct s3c_cpufreq_config *cfg;

	cfg = s3c_cpufreq_getconfig();
	if (!cfg) {
		seq_printf(seq, "no configuration registered\n");
		return 0;
	}

	seq_printf(seq, "  FCLK %ld Hz\n", cfg->freq.fclk);
	seq_printf(seq, "  HCLK %ld Hz (%lu.%lu ns)\n",
		   cfg->freq.hclk, print_ns(cfg->freq.hclk_tns));
	seq_printf(seq, "  PCLK %ld Hz\n", cfg->freq.hclk);
	seq_printf(seq, "ARMCLK %ld Hz\n", cfg->freq.armclk);
	seq_printf(seq, "\n");

	show_max(seq, &cfg->max);

	seq_printf(seq, "Divisors: P=%d, H=%d, A=%d, dvs=%s\n",
		   cfg->divs.h_divisor, cfg->divs.p_divisor,
		   cfg->divs.arm_divisor, cfg->divs.dvs ? "on" : "off");
	seq_printf(seq, "\n");

	seq_printf(seq, "lock_pll=%u\n", cfg->lock_pll);

	return 0;
}

DEFINE_SHOW_ATTRIBUTE(info);

static int io_show(struct seq_file *seq, void *p)
{
	void (*show_bank)(struct seq_file *, struct s3c_cpufreq_config *, union s3c_iobank *);
	struct s3c_cpufreq_config *cfg;
	struct s3c_iotimings *iot;
	union s3c_iobank *iob;
	int bank;

	cfg = s3c_cpufreq_getconfig();
	if (!cfg) {
		seq_printf(seq, "no configuration registered\n");
		return 0;
	}

	show_bank = cfg->info->debug_io_show;
	if (!show_bank) {
		seq_printf(seq, "no code to show bank timing\n");
		return 0;
	}

	iot = s3c_cpufreq_getiotimings();
	if (!iot) {
		seq_printf(seq, "no io timings registered\n");
		return 0;
	}

	seq_printf(seq, "hclk period is %lu.%lu ns\n", print_ns(cfg->freq.hclk_tns));

	for (bank = 0; bank < MAX_BANKS; bank++) {
		iob = &iot->bank[bank];

		seq_printf(seq, "bank %d: ", bank);

		if (!iob->io_2410) {
			seq_printf(seq, "nothing set\n");
			continue;
		}

		show_bank(seq, cfg, iob);
	}

	return 0;
}

DEFINE_SHOW_ATTRIBUTE(io);

static int __init s3c_freq_debugfs_init(void)
{
	dbgfs_root = debugfs_create_dir("s3c-cpufreq", NULL);
	if (IS_ERR(dbgfs_root)) {
		pr_err("%s: error creating debugfs root\n", __func__);
		return PTR_ERR(dbgfs_root);
	}

	dbgfs_file_io = debugfs_create_file("io-timing", S_IRUGO, dbgfs_root,
					    NULL, &io_fops);

	dbgfs_file_info = debugfs_create_file("info", S_IRUGO, dbgfs_root,
					      NULL, &info_fops);

	dbgfs_file_board = debugfs_create_file("board", S_IRUGO, dbgfs_root,
					       NULL, &board_fops);

	return 0;
}

late_initcall(s3c_freq_debugfs_init);

