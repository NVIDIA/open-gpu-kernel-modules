// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * sun4i-ss-core.c - hardware cryptographic accelerator for Allwinner A20 SoC
 *
 * Copyright (C) 2013-2015 Corentin LABBE <clabbe.montjoie@gmail.com>
 *
 * Core file which registers crypto algorithms supported by the SS.
 *
 * You could find a link for the datasheet in Documentation/arm/sunxi.rst
 */
#include <linux/clk.h>
#include <linux/crypto.h>
#include <linux/debugfs.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <crypto/scatterwalk.h>
#include <linux/scatterlist.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/reset.h>

#include "sun4i-ss.h"

static const struct ss_variant ss_a10_variant = {
	.sha1_in_be = false,
};

static const struct ss_variant ss_a33_variant = {
	.sha1_in_be = true,
};

static struct sun4i_ss_alg_template ss_algs[] = {
{       .type = CRYPTO_ALG_TYPE_AHASH,
	.mode = SS_OP_MD5,
	.alg.hash = {
		.init = sun4i_hash_init,
		.update = sun4i_hash_update,
		.final = sun4i_hash_final,
		.finup = sun4i_hash_finup,
		.digest = sun4i_hash_digest,
		.export = sun4i_hash_export_md5,
		.import = sun4i_hash_import_md5,
		.halg = {
			.digestsize = MD5_DIGEST_SIZE,
			.statesize = sizeof(struct md5_state),
			.base = {
				.cra_name = "md5",
				.cra_driver_name = "md5-sun4i-ss",
				.cra_priority = 300,
				.cra_alignmask = 3,
				.cra_blocksize = MD5_HMAC_BLOCK_SIZE,
				.cra_ctxsize = sizeof(struct sun4i_req_ctx),
				.cra_module = THIS_MODULE,
				.cra_init = sun4i_hash_crainit,
				.cra_exit = sun4i_hash_craexit,
			}
		}
	}
},
{       .type = CRYPTO_ALG_TYPE_AHASH,
	.mode = SS_OP_SHA1,
	.alg.hash = {
		.init = sun4i_hash_init,
		.update = sun4i_hash_update,
		.final = sun4i_hash_final,
		.finup = sun4i_hash_finup,
		.digest = sun4i_hash_digest,
		.export = sun4i_hash_export_sha1,
		.import = sun4i_hash_import_sha1,
		.halg = {
			.digestsize = SHA1_DIGEST_SIZE,
			.statesize = sizeof(struct sha1_state),
			.base = {
				.cra_name = "sha1",
				.cra_driver_name = "sha1-sun4i-ss",
				.cra_priority = 300,
				.cra_alignmask = 3,
				.cra_blocksize = SHA1_BLOCK_SIZE,
				.cra_ctxsize = sizeof(struct sun4i_req_ctx),
				.cra_module = THIS_MODULE,
				.cra_init = sun4i_hash_crainit,
				.cra_exit = sun4i_hash_craexit,
			}
		}
	}
},
{       .type = CRYPTO_ALG_TYPE_SKCIPHER,
	.alg.crypto = {
		.setkey         = sun4i_ss_aes_setkey,
		.encrypt        = sun4i_ss_cbc_aes_encrypt,
		.decrypt        = sun4i_ss_cbc_aes_decrypt,
		.min_keysize	= AES_MIN_KEY_SIZE,
		.max_keysize	= AES_MAX_KEY_SIZE,
		.ivsize		= AES_BLOCK_SIZE,
		.base = {
			.cra_name = "cbc(aes)",
			.cra_driver_name = "cbc-aes-sun4i-ss",
			.cra_priority = 300,
			.cra_blocksize = AES_BLOCK_SIZE,
			.cra_flags = CRYPTO_ALG_KERN_DRIVER_ONLY | CRYPTO_ALG_NEED_FALLBACK,
			.cra_ctxsize = sizeof(struct sun4i_tfm_ctx),
			.cra_module = THIS_MODULE,
			.cra_alignmask = 3,
			.cra_init = sun4i_ss_cipher_init,
			.cra_exit = sun4i_ss_cipher_exit,
		}
	}
},
{       .type = CRYPTO_ALG_TYPE_SKCIPHER,
	.alg.crypto = {
		.setkey         = sun4i_ss_aes_setkey,
		.encrypt        = sun4i_ss_ecb_aes_encrypt,
		.decrypt        = sun4i_ss_ecb_aes_decrypt,
		.min_keysize	= AES_MIN_KEY_SIZE,
		.max_keysize	= AES_MAX_KEY_SIZE,
		.base = {
			.cra_name = "ecb(aes)",
			.cra_driver_name = "ecb-aes-sun4i-ss",
			.cra_priority = 300,
			.cra_blocksize = AES_BLOCK_SIZE,
			.cra_flags = CRYPTO_ALG_KERN_DRIVER_ONLY | CRYPTO_ALG_NEED_FALLBACK,
			.cra_ctxsize = sizeof(struct sun4i_tfm_ctx),
			.cra_module = THIS_MODULE,
			.cra_alignmask = 3,
			.cra_init = sun4i_ss_cipher_init,
			.cra_exit = sun4i_ss_cipher_exit,
		}
	}
},
{       .type = CRYPTO_ALG_TYPE_SKCIPHER,
	.alg.crypto = {
		.setkey         = sun4i_ss_des_setkey,
		.encrypt        = sun4i_ss_cbc_des_encrypt,
		.decrypt        = sun4i_ss_cbc_des_decrypt,
		.min_keysize    = DES_KEY_SIZE,
		.max_keysize    = DES_KEY_SIZE,
		.ivsize         = DES_BLOCK_SIZE,
		.base = {
			.cra_name = "cbc(des)",
			.cra_driver_name = "cbc-des-sun4i-ss",
			.cra_priority = 300,
			.cra_blocksize = DES_BLOCK_SIZE,
			.cra_flags = CRYPTO_ALG_KERN_DRIVER_ONLY | CRYPTO_ALG_NEED_FALLBACK,
			.cra_ctxsize = sizeof(struct sun4i_req_ctx),
			.cra_module = THIS_MODULE,
			.cra_alignmask = 3,
			.cra_init = sun4i_ss_cipher_init,
			.cra_exit = sun4i_ss_cipher_exit,
		}
	}
},
{       .type = CRYPTO_ALG_TYPE_SKCIPHER,
	.alg.crypto = {
		.setkey         = sun4i_ss_des_setkey,
		.encrypt        = sun4i_ss_ecb_des_encrypt,
		.decrypt        = sun4i_ss_ecb_des_decrypt,
		.min_keysize    = DES_KEY_SIZE,
		.max_keysize    = DES_KEY_SIZE,
		.base = {
			.cra_name = "ecb(des)",
			.cra_driver_name = "ecb-des-sun4i-ss",
			.cra_priority = 300,
			.cra_blocksize = DES_BLOCK_SIZE,
			.cra_flags = CRYPTO_ALG_KERN_DRIVER_ONLY | CRYPTO_ALG_NEED_FALLBACK,
			.cra_ctxsize = sizeof(struct sun4i_req_ctx),
			.cra_module = THIS_MODULE,
			.cra_alignmask = 3,
			.cra_init = sun4i_ss_cipher_init,
			.cra_exit = sun4i_ss_cipher_exit,
		}
	}
},
{       .type = CRYPTO_ALG_TYPE_SKCIPHER,
	.alg.crypto = {
		.setkey         = sun4i_ss_des3_setkey,
		.encrypt        = sun4i_ss_cbc_des3_encrypt,
		.decrypt        = sun4i_ss_cbc_des3_decrypt,
		.min_keysize    = DES3_EDE_KEY_SIZE,
		.max_keysize    = DES3_EDE_KEY_SIZE,
		.ivsize         = DES3_EDE_BLOCK_SIZE,
		.base = {
			.cra_name = "cbc(des3_ede)",
			.cra_driver_name = "cbc-des3-sun4i-ss",
			.cra_priority = 300,
			.cra_blocksize = DES3_EDE_BLOCK_SIZE,
			.cra_flags = CRYPTO_ALG_KERN_DRIVER_ONLY | CRYPTO_ALG_NEED_FALLBACK,
			.cra_ctxsize = sizeof(struct sun4i_req_ctx),
			.cra_module = THIS_MODULE,
			.cra_alignmask = 3,
			.cra_init = sun4i_ss_cipher_init,
			.cra_exit = sun4i_ss_cipher_exit,
		}
	}
},
{       .type = CRYPTO_ALG_TYPE_SKCIPHER,
	.alg.crypto = {
		.setkey         = sun4i_ss_des3_setkey,
		.encrypt        = sun4i_ss_ecb_des3_encrypt,
		.decrypt        = sun4i_ss_ecb_des3_decrypt,
		.min_keysize    = DES3_EDE_KEY_SIZE,
		.max_keysize    = DES3_EDE_KEY_SIZE,
		.base = {
			.cra_name = "ecb(des3_ede)",
			.cra_driver_name = "ecb-des3-sun4i-ss",
			.cra_priority = 300,
			.cra_blocksize = DES3_EDE_BLOCK_SIZE,
			.cra_flags = CRYPTO_ALG_KERN_DRIVER_ONLY | CRYPTO_ALG_NEED_FALLBACK,
			.cra_ctxsize = sizeof(struct sun4i_req_ctx),
			.cra_module = THIS_MODULE,
			.cra_alignmask = 3,
			.cra_init = sun4i_ss_cipher_init,
			.cra_exit = sun4i_ss_cipher_exit,
		}
	}
},
#ifdef CONFIG_CRYPTO_DEV_SUN4I_SS_PRNG
{
	.type = CRYPTO_ALG_TYPE_RNG,
	.alg.rng = {
		.base = {
			.cra_name		= "stdrng",
			.cra_driver_name	= "sun4i_ss_rng",
			.cra_priority		= 300,
			.cra_ctxsize		= 0,
			.cra_module		= THIS_MODULE,
		},
		.generate               = sun4i_ss_prng_generate,
		.seed                   = sun4i_ss_prng_seed,
		.seedsize               = SS_SEED_LEN / BITS_PER_BYTE,
	}
},
#endif
};

static int sun4i_ss_dbgfs_read(struct seq_file *seq, void *v)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(ss_algs); i++) {
		if (!ss_algs[i].ss)
			continue;
		switch (ss_algs[i].type) {
		case CRYPTO_ALG_TYPE_SKCIPHER:
			seq_printf(seq, "%s %s reqs=%lu opti=%lu fallback=%lu tsize=%lu\n",
				   ss_algs[i].alg.crypto.base.cra_driver_name,
				   ss_algs[i].alg.crypto.base.cra_name,
				   ss_algs[i].stat_req, ss_algs[i].stat_opti, ss_algs[i].stat_fb,
				   ss_algs[i].stat_bytes);
			break;
		case CRYPTO_ALG_TYPE_RNG:
			seq_printf(seq, "%s %s reqs=%lu tsize=%lu\n",
				   ss_algs[i].alg.rng.base.cra_driver_name,
				   ss_algs[i].alg.rng.base.cra_name,
				   ss_algs[i].stat_req, ss_algs[i].stat_bytes);
			break;
		case CRYPTO_ALG_TYPE_AHASH:
			seq_printf(seq, "%s %s reqs=%lu\n",
				   ss_algs[i].alg.hash.halg.base.cra_driver_name,
				   ss_algs[i].alg.hash.halg.base.cra_name,
				   ss_algs[i].stat_req);
			break;
		}
	}
	return 0;
}

static int sun4i_ss_dbgfs_open(struct inode *inode, struct file *file)
{
	return single_open(file, sun4i_ss_dbgfs_read, inode->i_private);
}

static const struct file_operations sun4i_ss_debugfs_fops = {
	.owner = THIS_MODULE,
	.open = sun4i_ss_dbgfs_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

/*
 * Power management strategy: The device is suspended unless a TFM exists for
 * one of the algorithms proposed by this driver.
 */
static int sun4i_ss_pm_suspend(struct device *dev)
{
	struct sun4i_ss_ctx *ss = dev_get_drvdata(dev);

	reset_control_assert(ss->reset);

	clk_disable_unprepare(ss->ssclk);
	clk_disable_unprepare(ss->busclk);
	return 0;
}

static int sun4i_ss_pm_resume(struct device *dev)
{
	struct sun4i_ss_ctx *ss = dev_get_drvdata(dev);

	int err;

	err = clk_prepare_enable(ss->busclk);
	if (err) {
		dev_err(ss->dev, "Cannot prepare_enable busclk\n");
		goto err_enable;
	}

	err = clk_prepare_enable(ss->ssclk);
	if (err) {
		dev_err(ss->dev, "Cannot prepare_enable ssclk\n");
		goto err_enable;
	}

	err = reset_control_deassert(ss->reset);
	if (err) {
		dev_err(ss->dev, "Cannot deassert reset control\n");
		goto err_enable;
	}

	return err;
err_enable:
	sun4i_ss_pm_suspend(dev);
	return err;
}

static const struct dev_pm_ops sun4i_ss_pm_ops = {
	SET_RUNTIME_PM_OPS(sun4i_ss_pm_suspend, sun4i_ss_pm_resume, NULL)
};

/*
 * When power management is enabled, this function enables the PM and set the
 * device as suspended
 * When power management is disabled, this function just enables the device
 */
static int sun4i_ss_pm_init(struct sun4i_ss_ctx *ss)
{
	int err;

	pm_runtime_use_autosuspend(ss->dev);
	pm_runtime_set_autosuspend_delay(ss->dev, 2000);

	err = pm_runtime_set_suspended(ss->dev);
	if (err)
		return err;
	pm_runtime_enable(ss->dev);
	return err;
}

static void sun4i_ss_pm_exit(struct sun4i_ss_ctx *ss)
{
	pm_runtime_disable(ss->dev);
}

static int sun4i_ss_probe(struct platform_device *pdev)
{
	u32 v;
	int err, i;
	unsigned long cr;
	const unsigned long cr_ahb = 24 * 1000 * 1000;
	const unsigned long cr_mod = 150 * 1000 * 1000;
	struct sun4i_ss_ctx *ss;

	if (!pdev->dev.of_node)
		return -ENODEV;

	ss = devm_kzalloc(&pdev->dev, sizeof(*ss), GFP_KERNEL);
	if (!ss)
		return -ENOMEM;

	ss->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(ss->base)) {
		dev_err(&pdev->dev, "Cannot request MMIO\n");
		return PTR_ERR(ss->base);
	}

	ss->variant = of_device_get_match_data(&pdev->dev);
	if (!ss->variant) {
		dev_err(&pdev->dev, "Missing Security System variant\n");
		return -EINVAL;
	}

	ss->ssclk = devm_clk_get(&pdev->dev, "mod");
	if (IS_ERR(ss->ssclk)) {
		err = PTR_ERR(ss->ssclk);
		dev_err(&pdev->dev, "Cannot get SS clock err=%d\n", err);
		return err;
	}
	dev_dbg(&pdev->dev, "clock ss acquired\n");

	ss->busclk = devm_clk_get(&pdev->dev, "ahb");
	if (IS_ERR(ss->busclk)) {
		err = PTR_ERR(ss->busclk);
		dev_err(&pdev->dev, "Cannot get AHB SS clock err=%d\n", err);
		return err;
	}
	dev_dbg(&pdev->dev, "clock ahb_ss acquired\n");

	ss->reset = devm_reset_control_get_optional(&pdev->dev, "ahb");
	if (IS_ERR(ss->reset))
		return PTR_ERR(ss->reset);
	if (!ss->reset)
		dev_info(&pdev->dev, "no reset control found\n");

	/*
	 * Check that clock have the correct rates given in the datasheet
	 * Try to set the clock to the maximum allowed
	 */
	err = clk_set_rate(ss->ssclk, cr_mod);
	if (err) {
		dev_err(&pdev->dev, "Cannot set clock rate to ssclk\n");
		return err;
	}

	/*
	 * The only impact on clocks below requirement are bad performance,
	 * so do not print "errors"
	 * warn on Overclocked clocks
	 */
	cr = clk_get_rate(ss->busclk);
	if (cr >= cr_ahb)
		dev_dbg(&pdev->dev, "Clock bus %lu (%lu MHz) (must be >= %lu)\n",
			cr, cr / 1000000, cr_ahb);
	else
		dev_warn(&pdev->dev, "Clock bus %lu (%lu MHz) (must be >= %lu)\n",
			 cr, cr / 1000000, cr_ahb);

	cr = clk_get_rate(ss->ssclk);
	if (cr <= cr_mod)
		if (cr < cr_mod)
			dev_warn(&pdev->dev, "Clock ss %lu (%lu MHz) (must be <= %lu)\n",
				 cr, cr / 1000000, cr_mod);
		else
			dev_dbg(&pdev->dev, "Clock ss %lu (%lu MHz) (must be <= %lu)\n",
				cr, cr / 1000000, cr_mod);
	else
		dev_warn(&pdev->dev, "Clock ss is at %lu (%lu MHz) (must be <= %lu)\n",
			 cr, cr / 1000000, cr_mod);

	ss->dev = &pdev->dev;
	platform_set_drvdata(pdev, ss);

	spin_lock_init(&ss->slock);

	err = sun4i_ss_pm_init(ss);
	if (err)
		return err;

	/*
	 * Datasheet named it "Die Bonding ID"
	 * I expect to be a sort of Security System Revision number.
	 * Since the A80 seems to have an other version of SS
	 * this info could be useful
	 */

	err = pm_runtime_resume_and_get(ss->dev);
	if (err < 0)
		goto error_pm;

	writel(SS_ENABLED, ss->base + SS_CTL);
	v = readl(ss->base + SS_CTL);
	v >>= 16;
	v &= 0x07;
	dev_info(&pdev->dev, "Die ID %d\n", v);
	writel(0, ss->base + SS_CTL);

	pm_runtime_put_sync(ss->dev);

	for (i = 0; i < ARRAY_SIZE(ss_algs); i++) {
		ss_algs[i].ss = ss;
		switch (ss_algs[i].type) {
		case CRYPTO_ALG_TYPE_SKCIPHER:
			err = crypto_register_skcipher(&ss_algs[i].alg.crypto);
			if (err) {
				dev_err(ss->dev, "Fail to register %s\n",
					ss_algs[i].alg.crypto.base.cra_name);
				goto error_alg;
			}
			break;
		case CRYPTO_ALG_TYPE_AHASH:
			err = crypto_register_ahash(&ss_algs[i].alg.hash);
			if (err) {
				dev_err(ss->dev, "Fail to register %s\n",
					ss_algs[i].alg.hash.halg.base.cra_name);
				goto error_alg;
			}
			break;
		case CRYPTO_ALG_TYPE_RNG:
			err = crypto_register_rng(&ss_algs[i].alg.rng);
			if (err) {
				dev_err(ss->dev, "Fail to register %s\n",
					ss_algs[i].alg.rng.base.cra_name);
			}
			break;
		}
	}

	/* Ignore error of debugfs */
	ss->dbgfs_dir = debugfs_create_dir("sun4i-ss", NULL);
	ss->dbgfs_stats = debugfs_create_file("stats", 0444, ss->dbgfs_dir, ss,
					      &sun4i_ss_debugfs_fops);

	return 0;
error_alg:
	i--;
	for (; i >= 0; i--) {
		switch (ss_algs[i].type) {
		case CRYPTO_ALG_TYPE_SKCIPHER:
			crypto_unregister_skcipher(&ss_algs[i].alg.crypto);
			break;
		case CRYPTO_ALG_TYPE_AHASH:
			crypto_unregister_ahash(&ss_algs[i].alg.hash);
			break;
		case CRYPTO_ALG_TYPE_RNG:
			crypto_unregister_rng(&ss_algs[i].alg.rng);
			break;
		}
	}
error_pm:
	sun4i_ss_pm_exit(ss);
	return err;
}

static int sun4i_ss_remove(struct platform_device *pdev)
{
	int i;
	struct sun4i_ss_ctx *ss = platform_get_drvdata(pdev);

	for (i = 0; i < ARRAY_SIZE(ss_algs); i++) {
		switch (ss_algs[i].type) {
		case CRYPTO_ALG_TYPE_SKCIPHER:
			crypto_unregister_skcipher(&ss_algs[i].alg.crypto);
			break;
		case CRYPTO_ALG_TYPE_AHASH:
			crypto_unregister_ahash(&ss_algs[i].alg.hash);
			break;
		case CRYPTO_ALG_TYPE_RNG:
			crypto_unregister_rng(&ss_algs[i].alg.rng);
			break;
		}
	}

	sun4i_ss_pm_exit(ss);
	return 0;
}

static const struct of_device_id a20ss_crypto_of_match_table[] = {
	{ .compatible = "allwinner,sun4i-a10-crypto",
	  .data = &ss_a10_variant
	},
	{ .compatible = "allwinner,sun8i-a33-crypto",
	  .data = &ss_a33_variant
	},
	{}
};
MODULE_DEVICE_TABLE(of, a20ss_crypto_of_match_table);

static struct platform_driver sun4i_ss_driver = {
	.probe          = sun4i_ss_probe,
	.remove         = sun4i_ss_remove,
	.driver         = {
		.name           = "sun4i-ss",
		.pm		= &sun4i_ss_pm_ops,
		.of_match_table	= a20ss_crypto_of_match_table,
	},
};

module_platform_driver(sun4i_ss_driver);

MODULE_ALIAS("platform:sun4i-ss");
MODULE_DESCRIPTION("Allwinner Security System cryptographic accelerator");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Corentin LABBE <clabbe.montjoie@gmail.com>");
