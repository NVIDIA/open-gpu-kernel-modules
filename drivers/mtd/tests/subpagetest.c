// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2006-2007 Nokia Corporation
 *
 * Test sub-page read and write on MTD device.
 * Author: Adrian Hunter <ext-adrian.hunter@nokia.com>
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/err.h>
#include <linux/mtd/mtd.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/random.h>

#include "mtd_test.h"

static int dev = -EINVAL;
module_param(dev, int, S_IRUGO);
MODULE_PARM_DESC(dev, "MTD device number to use");

static struct mtd_info *mtd;
static unsigned char *writebuf;
static unsigned char *readbuf;
static unsigned char *bbt;

static int subpgsize;
static int bufsize;
static int ebcnt;
static int pgcnt;
static int errcnt;
static struct rnd_state rnd_state;

static inline void clear_data(unsigned char *buf, size_t len)
{
	memset(buf, 0, len);
}

static int write_eraseblock(int ebnum)
{
	size_t written;
	int err = 0;
	loff_t addr = (loff_t)ebnum * mtd->erasesize;

	prandom_bytes_state(&rnd_state, writebuf, subpgsize);
	err = mtd_write(mtd, addr, subpgsize, &written, writebuf);
	if (unlikely(err || written != subpgsize)) {
		pr_err("error: write failed at %#llx\n",
		       (long long)addr);
		if (written != subpgsize) {
			pr_err("  write size: %#x\n", subpgsize);
			pr_err("  written: %#zx\n", written);
		}
		return err ? err : -1;
	}

	addr += subpgsize;

	prandom_bytes_state(&rnd_state, writebuf, subpgsize);
	err = mtd_write(mtd, addr, subpgsize, &written, writebuf);
	if (unlikely(err || written != subpgsize)) {
		pr_err("error: write failed at %#llx\n",
		       (long long)addr);
		if (written != subpgsize) {
			pr_err("  write size: %#x\n", subpgsize);
			pr_err("  written: %#zx\n", written);
		}
		return err ? err : -1;
	}

	return err;
}

static int write_eraseblock2(int ebnum)
{
	size_t written;
	int err = 0, k;
	loff_t addr = (loff_t)ebnum * mtd->erasesize;

	for (k = 1; k < 33; ++k) {
		if (addr + (subpgsize * k) > (loff_t)(ebnum + 1) * mtd->erasesize)
			break;
		prandom_bytes_state(&rnd_state, writebuf, subpgsize * k);
		err = mtd_write(mtd, addr, subpgsize * k, &written, writebuf);
		if (unlikely(err || written != subpgsize * k)) {
			pr_err("error: write failed at %#llx\n",
			       (long long)addr);
			if (written != subpgsize * k) {
				pr_err("  write size: %#x\n",
				       subpgsize * k);
				pr_err("  written: %#08zx\n",
				       written);
			}
			return err ? err : -1;
		}
		addr += subpgsize * k;
	}

	return err;
}

static void print_subpage(unsigned char *p)
{
	int i, j;

	for (i = 0; i < subpgsize; ) {
		for (j = 0; i < subpgsize && j < 32; ++i, ++j)
			printk("%02x", *p++);
		printk("\n");
	}
}

static int verify_eraseblock(int ebnum)
{
	size_t read;
	int err = 0;
	loff_t addr = (loff_t)ebnum * mtd->erasesize;

	prandom_bytes_state(&rnd_state, writebuf, subpgsize);
	clear_data(readbuf, subpgsize);
	err = mtd_read(mtd, addr, subpgsize, &read, readbuf);
	if (unlikely(err || read != subpgsize)) {
		if (mtd_is_bitflip(err) && read == subpgsize) {
			pr_info("ECC correction at %#llx\n",
			       (long long)addr);
			err = 0;
		} else {
			pr_err("error: read failed at %#llx\n",
			       (long long)addr);
			return err ? err : -1;
		}
	}
	if (unlikely(memcmp(readbuf, writebuf, subpgsize))) {
		pr_err("error: verify failed at %#llx\n",
		       (long long)addr);
		pr_info("------------- written----------------\n");
		print_subpage(writebuf);
		pr_info("------------- read ------------------\n");
		print_subpage(readbuf);
		pr_info("-------------------------------------\n");
		errcnt += 1;
	}

	addr += subpgsize;

	prandom_bytes_state(&rnd_state, writebuf, subpgsize);
	clear_data(readbuf, subpgsize);
	err = mtd_read(mtd, addr, subpgsize, &read, readbuf);
	if (unlikely(err || read != subpgsize)) {
		if (mtd_is_bitflip(err) && read == subpgsize) {
			pr_info("ECC correction at %#llx\n",
			       (long long)addr);
			err = 0;
		} else {
			pr_err("error: read failed at %#llx\n",
			       (long long)addr);
			return err ? err : -1;
		}
	}
	if (unlikely(memcmp(readbuf, writebuf, subpgsize))) {
		pr_info("error: verify failed at %#llx\n",
		       (long long)addr);
		pr_info("------------- written----------------\n");
		print_subpage(writebuf);
		pr_info("------------- read ------------------\n");
		print_subpage(readbuf);
		pr_info("-------------------------------------\n");
		errcnt += 1;
	}

	return err;
}

static int verify_eraseblock2(int ebnum)
{
	size_t read;
	int err = 0, k;
	loff_t addr = (loff_t)ebnum * mtd->erasesize;

	for (k = 1; k < 33; ++k) {
		if (addr + (subpgsize * k) > (loff_t)(ebnum + 1) * mtd->erasesize)
			break;
		prandom_bytes_state(&rnd_state, writebuf, subpgsize * k);
		clear_data(readbuf, subpgsize * k);
		err = mtd_read(mtd, addr, subpgsize * k, &read, readbuf);
		if (unlikely(err || read != subpgsize * k)) {
			if (mtd_is_bitflip(err) && read == subpgsize * k) {
				pr_info("ECC correction at %#llx\n",
				       (long long)addr);
				err = 0;
			} else {
				pr_err("error: read failed at "
				       "%#llx\n", (long long)addr);
				return err ? err : -1;
			}
		}
		if (unlikely(memcmp(readbuf, writebuf, subpgsize * k))) {
			pr_err("error: verify failed at %#llx\n",
			       (long long)addr);
			errcnt += 1;
		}
		addr += subpgsize * k;
	}

	return err;
}

static int verify_eraseblock_ff(int ebnum)
{
	uint32_t j;
	size_t read;
	int err = 0;
	loff_t addr = (loff_t)ebnum * mtd->erasesize;

	memset(writebuf, 0xff, subpgsize);
	for (j = 0; j < mtd->erasesize / subpgsize; ++j) {
		clear_data(readbuf, subpgsize);
		err = mtd_read(mtd, addr, subpgsize, &read, readbuf);
		if (unlikely(err || read != subpgsize)) {
			if (mtd_is_bitflip(err) && read == subpgsize) {
				pr_info("ECC correction at %#llx\n",
				       (long long)addr);
				err = 0;
			} else {
				pr_err("error: read failed at "
				       "%#llx\n", (long long)addr);
				return err ? err : -1;
			}
		}
		if (unlikely(memcmp(readbuf, writebuf, subpgsize))) {
			pr_err("error: verify 0xff failed at "
			       "%#llx\n", (long long)addr);
			errcnt += 1;
		}
		addr += subpgsize;
	}

	return err;
}

static int verify_all_eraseblocks_ff(void)
{
	int err;
	unsigned int i;

	pr_info("verifying all eraseblocks for 0xff\n");
	for (i = 0; i < ebcnt; ++i) {
		if (bbt[i])
			continue;
		err = verify_eraseblock_ff(i);
		if (err)
			return err;
		if (i % 256 == 0)
			pr_info("verified up to eraseblock %u\n", i);

		err = mtdtest_relax();
		if (err)
			return err;
	}
	pr_info("verified %u eraseblocks\n", i);
	return 0;
}

static int __init mtd_subpagetest_init(void)
{
	int err = 0;
	uint32_t i;
	uint64_t tmp;

	printk(KERN_INFO "\n");
	printk(KERN_INFO "=================================================\n");

	if (dev < 0) {
		pr_info("Please specify a valid mtd-device via module parameter\n");
		pr_crit("CAREFUL: This test wipes all data on the specified MTD device!\n");
		return -EINVAL;
	}

	pr_info("MTD device: %d\n", dev);

	mtd = get_mtd_device(NULL, dev);
	if (IS_ERR(mtd)) {
		err = PTR_ERR(mtd);
		pr_err("error: cannot get MTD device\n");
		return err;
	}

	if (!mtd_type_is_nand(mtd)) {
		pr_info("this test requires NAND flash\n");
		goto out;
	}

	subpgsize = mtd->writesize >> mtd->subpage_sft;
	tmp = mtd->size;
	do_div(tmp, mtd->erasesize);
	ebcnt = tmp;
	pgcnt = mtd->erasesize / mtd->writesize;

	pr_info("MTD device size %llu, eraseblock size %u, "
	       "page size %u, subpage size %u, count of eraseblocks %u, "
	       "pages per eraseblock %u, OOB size %u\n",
	       (unsigned long long)mtd->size, mtd->erasesize,
	       mtd->writesize, subpgsize, ebcnt, pgcnt, mtd->oobsize);

	err = -ENOMEM;
	bufsize = subpgsize * 32;
	writebuf = kmalloc(bufsize, GFP_KERNEL);
	if (!writebuf)
		goto out;
	readbuf = kmalloc(bufsize, GFP_KERNEL);
	if (!readbuf)
		goto out;
	bbt = kzalloc(ebcnt, GFP_KERNEL);
	if (!bbt)
		goto out;

	err = mtdtest_scan_for_bad_eraseblocks(mtd, bbt, 0, ebcnt);
	if (err)
		goto out;

	err = mtdtest_erase_good_eraseblocks(mtd, bbt, 0, ebcnt);
	if (err)
		goto out;

	pr_info("writing whole device\n");
	prandom_seed_state(&rnd_state, 1);
	for (i = 0; i < ebcnt; ++i) {
		if (bbt[i])
			continue;
		err = write_eraseblock(i);
		if (unlikely(err))
			goto out;
		if (i % 256 == 0)
			pr_info("written up to eraseblock %u\n", i);

		err = mtdtest_relax();
		if (err)
			goto out;
	}
	pr_info("written %u eraseblocks\n", i);

	prandom_seed_state(&rnd_state, 1);
	pr_info("verifying all eraseblocks\n");
	for (i = 0; i < ebcnt; ++i) {
		if (bbt[i])
			continue;
		err = verify_eraseblock(i);
		if (unlikely(err))
			goto out;
		if (i % 256 == 0)
			pr_info("verified up to eraseblock %u\n", i);

		err = mtdtest_relax();
		if (err)
			goto out;
	}
	pr_info("verified %u eraseblocks\n", i);

	err = mtdtest_erase_good_eraseblocks(mtd, bbt, 0, ebcnt);
	if (err)
		goto out;

	err = verify_all_eraseblocks_ff();
	if (err)
		goto out;

	/* Write all eraseblocks */
	prandom_seed_state(&rnd_state, 3);
	pr_info("writing whole device\n");
	for (i = 0; i < ebcnt; ++i) {
		if (bbt[i])
			continue;
		err = write_eraseblock2(i);
		if (unlikely(err))
			goto out;
		if (i % 256 == 0)
			pr_info("written up to eraseblock %u\n", i);

		err = mtdtest_relax();
		if (err)
			goto out;
	}
	pr_info("written %u eraseblocks\n", i);

	/* Check all eraseblocks */
	prandom_seed_state(&rnd_state, 3);
	pr_info("verifying all eraseblocks\n");
	for (i = 0; i < ebcnt; ++i) {
		if (bbt[i])
			continue;
		err = verify_eraseblock2(i);
		if (unlikely(err))
			goto out;
		if (i % 256 == 0)
			pr_info("verified up to eraseblock %u\n", i);

		err = mtdtest_relax();
		if (err)
			goto out;
	}
	pr_info("verified %u eraseblocks\n", i);

	err = mtdtest_erase_good_eraseblocks(mtd, bbt, 0, ebcnt);
	if (err)
		goto out;

	err = verify_all_eraseblocks_ff();
	if (err)
		goto out;

	pr_info("finished with %d errors\n", errcnt);

out:
	kfree(bbt);
	kfree(readbuf);
	kfree(writebuf);
	put_mtd_device(mtd);
	if (err)
		pr_info("error %d occurred\n", err);
	printk(KERN_INFO "=================================================\n");
	return err;
}
module_init(mtd_subpagetest_init);

static void __exit mtd_subpagetest_exit(void)
{
	return;
}
module_exit(mtd_subpagetest_exit);

MODULE_DESCRIPTION("Subpage test module");
MODULE_AUTHOR("Adrian Hunter");
MODULE_LICENSE("GPL");
