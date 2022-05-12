// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2004 Richard Purdie
 *  Copyright (C) 2008 Dmitry Baryshkov
 *
 *  Based on Sharp's NAND driver sharp_sl.c
 */

#include <linux/genhd.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand-ecc-sw-hamming.h>
#include <linux/mtd/rawnand.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/sharpsl.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/io.h>

struct sharpsl_nand {
	struct nand_controller	controller;
	struct nand_chip	chip;

	void __iomem		*io;
};

static inline struct sharpsl_nand *mtd_to_sharpsl(struct mtd_info *mtd)
{
	return container_of(mtd_to_nand(mtd), struct sharpsl_nand, chip);
}

/* register offset */
#define ECCLPLB		0x00	/* line parity 7 - 0 bit */
#define ECCLPUB		0x04	/* line parity 15 - 8 bit */
#define ECCCP		0x08	/* column parity 5 - 0 bit */
#define ECCCNTR		0x0C	/* ECC byte counter */
#define ECCCLRR		0x10	/* cleare ECC */
#define FLASHIO		0x14	/* Flash I/O */
#define FLASHCTL	0x18	/* Flash Control */

/* Flash control bit */
#define FLRYBY		(1 << 5)
#define FLCE1		(1 << 4)
#define FLWP		(1 << 3)
#define FLALE		(1 << 2)
#define FLCLE		(1 << 1)
#define FLCE0		(1 << 0)

/*
 *	hardware specific access to control-lines
 *	ctrl:
 *	NAND_CNE: bit 0 -> ! bit 0 & 4
 *	NAND_CLE: bit 1 -> bit 1
 *	NAND_ALE: bit 2 -> bit 2
 *
 */
static void sharpsl_nand_hwcontrol(struct nand_chip *chip, int cmd,
				   unsigned int ctrl)
{
	struct sharpsl_nand *sharpsl = mtd_to_sharpsl(nand_to_mtd(chip));

	if (ctrl & NAND_CTRL_CHANGE) {
		unsigned char bits = ctrl & 0x07;

		bits |= (ctrl & 0x01) << 4;

		bits ^= 0x11;

		writeb((readb(sharpsl->io + FLASHCTL) & ~0x17) | bits, sharpsl->io + FLASHCTL);
	}

	if (cmd != NAND_CMD_NONE)
		writeb(cmd, chip->legacy.IO_ADDR_W);
}

static int sharpsl_nand_dev_ready(struct nand_chip *chip)
{
	struct sharpsl_nand *sharpsl = mtd_to_sharpsl(nand_to_mtd(chip));
	return !((readb(sharpsl->io + FLASHCTL) & FLRYBY) == 0);
}

static void sharpsl_nand_enable_hwecc(struct nand_chip *chip, int mode)
{
	struct sharpsl_nand *sharpsl = mtd_to_sharpsl(nand_to_mtd(chip));
	writeb(0, sharpsl->io + ECCCLRR);
}

static int sharpsl_nand_calculate_ecc(struct nand_chip *chip,
				      const u_char * dat, u_char * ecc_code)
{
	struct sharpsl_nand *sharpsl = mtd_to_sharpsl(nand_to_mtd(chip));
	ecc_code[0] = ~readb(sharpsl->io + ECCLPUB);
	ecc_code[1] = ~readb(sharpsl->io + ECCLPLB);
	ecc_code[2] = (~readb(sharpsl->io + ECCCP) << 2) | 0x03;
	return readb(sharpsl->io + ECCCNTR) != 0;
}

static int sharpsl_nand_correct_ecc(struct nand_chip *chip,
				    unsigned char *buf,
				    unsigned char *read_ecc,
				    unsigned char *calc_ecc)
{
	return ecc_sw_hamming_correct(buf, read_ecc, calc_ecc,
				      chip->ecc.size, false);
}

static int sharpsl_attach_chip(struct nand_chip *chip)
{
	if (chip->ecc.engine_type != NAND_ECC_ENGINE_TYPE_ON_HOST)
		return 0;

	chip->ecc.size = 256;
	chip->ecc.bytes = 3;
	chip->ecc.strength = 1;
	chip->ecc.hwctl = sharpsl_nand_enable_hwecc;
	chip->ecc.calculate = sharpsl_nand_calculate_ecc;
	chip->ecc.correct = sharpsl_nand_correct_ecc;

	return 0;
}

static const struct nand_controller_ops sharpsl_ops = {
	.attach_chip = sharpsl_attach_chip,
};

/*
 * Main initialization routine
 */
static int sharpsl_nand_probe(struct platform_device *pdev)
{
	struct nand_chip *this;
	struct mtd_info *mtd;
	struct resource *r;
	int err = 0;
	struct sharpsl_nand *sharpsl;
	struct sharpsl_nand_platform_data *data = dev_get_platdata(&pdev->dev);

	if (!data) {
		dev_err(&pdev->dev, "no platform data!\n");
		return -EINVAL;
	}

	/* Allocate memory for MTD device structure and private data */
	sharpsl = kzalloc(sizeof(struct sharpsl_nand), GFP_KERNEL);
	if (!sharpsl)
		return -ENOMEM;

	r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!r) {
		dev_err(&pdev->dev, "no io memory resource defined!\n");
		err = -ENODEV;
		goto err_get_res;
	}

	/* map physical address */
	sharpsl->io = ioremap(r->start, resource_size(r));
	if (!sharpsl->io) {
		dev_err(&pdev->dev, "ioremap to access Sharp SL NAND chip failed\n");
		err = -EIO;
		goto err_ioremap;
	}

	/* Get pointer to private data */
	this = (struct nand_chip *)(&sharpsl->chip);

	nand_controller_init(&sharpsl->controller);
	sharpsl->controller.ops = &sharpsl_ops;
	this->controller = &sharpsl->controller;

	/* Link the private data with the MTD structure */
	mtd = nand_to_mtd(this);
	mtd->dev.parent = &pdev->dev;
	mtd_set_ooblayout(mtd, data->ecc_layout);

	platform_set_drvdata(pdev, sharpsl);

	/*
	 * PXA initialize
	 */
	writeb(readb(sharpsl->io + FLASHCTL) | FLWP, sharpsl->io + FLASHCTL);

	/* Set address of NAND IO lines */
	this->legacy.IO_ADDR_R = sharpsl->io + FLASHIO;
	this->legacy.IO_ADDR_W = sharpsl->io + FLASHIO;
	/* Set address of hardware control function */
	this->legacy.cmd_ctrl = sharpsl_nand_hwcontrol;
	this->legacy.dev_ready = sharpsl_nand_dev_ready;
	/* 15 us command delay time */
	this->legacy.chip_delay = 15;
	this->badblock_pattern = data->badblock_pattern;

	/* Scan to find existence of the device */
	err = nand_scan(this, 1);
	if (err)
		goto err_scan;

	/* Register the partitions */
	mtd->name = "sharpsl-nand";

	err = mtd_device_parse_register(mtd, data->part_parsers, NULL,
					data->partitions, data->nr_partitions);
	if (err)
		goto err_add;

	/* Return happy */
	return 0;

err_add:
	nand_cleanup(this);

err_scan:
	iounmap(sharpsl->io);
err_ioremap:
err_get_res:
	kfree(sharpsl);
	return err;
}

/*
 * Clean up routine
 */
static int sharpsl_nand_remove(struct platform_device *pdev)
{
	struct sharpsl_nand *sharpsl = platform_get_drvdata(pdev);
	struct nand_chip *chip = &sharpsl->chip;
	int ret;

	/* Unregister device */
	ret = mtd_device_unregister(nand_to_mtd(chip));
	WARN_ON(ret);

	/* Release resources */
	nand_cleanup(chip);

	iounmap(sharpsl->io);

	/* Free the driver's structure */
	kfree(sharpsl);

	return 0;
}

static struct platform_driver sharpsl_nand_driver = {
	.driver = {
		.name	= "sharpsl-nand",
	},
	.probe		= sharpsl_nand_probe,
	.remove		= sharpsl_nand_remove,
};

module_platform_driver(sharpsl_nand_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Richard Purdie <rpurdie@rpsys.net>");
MODULE_DESCRIPTION("Device specific logic for NAND flash on Sharp SL-C7xx Series");
