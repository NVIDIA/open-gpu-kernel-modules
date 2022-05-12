// SPDX-License-Identifier: GPL-2.0-only
/*
 * Mac80211 SDIO driver for ST-Ericsson CW1200 device
 *
 * Copyright (c) 2010, ST-Ericsson
 * Author: Dmitry Tarnyagin <dmitry.tarnyagin@lockless.no>
 */

#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/mmc/host.h>
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/card.h>
#include <linux/mmc/sdio.h>
#include <linux/mmc/sdio_ids.h>
#include <net/mac80211.h>

#include "cw1200.h"
#include "hwbus.h"
#include <linux/platform_data/net-cw1200.h>
#include "hwio.h"

MODULE_AUTHOR("Dmitry Tarnyagin <dmitry.tarnyagin@lockless.no>");
MODULE_DESCRIPTION("mac80211 ST-Ericsson CW1200 SDIO driver");
MODULE_LICENSE("GPL");

#define SDIO_BLOCK_SIZE (512)

/* Default platform data for Sagrad modules */
static struct cw1200_platform_data_sdio sagrad_109x_evk_platform_data = {
	.ref_clk = 38400,
	.have_5ghz = false,
	.sdd_file = "sdd_sagrad_1091_1098.bin",
};

/* Allow platform data to be overridden */
static struct cw1200_platform_data_sdio *global_plat_data = &sagrad_109x_evk_platform_data;

void __init cw1200_sdio_set_platform_data(struct cw1200_platform_data_sdio *pdata)
{
	global_plat_data = pdata;
}

struct hwbus_priv {
	struct sdio_func	*func;
	struct cw1200_common	*core;
	const struct cw1200_platform_data_sdio *pdata;
};

static const struct sdio_device_id cw1200_sdio_ids[] = {
	{ SDIO_DEVICE(SDIO_VENDOR_ID_STE, SDIO_DEVICE_ID_STE_CW1200) },
	{ /* end: all zeroes */			},
};

/* hwbus_ops implemetation */

static int cw1200_sdio_memcpy_fromio(struct hwbus_priv *self,
				     unsigned int addr,
				     void *dst, int count)
{
	return sdio_memcpy_fromio(self->func, dst, addr, count);
}

static int cw1200_sdio_memcpy_toio(struct hwbus_priv *self,
				   unsigned int addr,
				   const void *src, int count)
{
	return sdio_memcpy_toio(self->func, addr, (void *)src, count);
}

static void cw1200_sdio_lock(struct hwbus_priv *self)
{
	sdio_claim_host(self->func);
}

static void cw1200_sdio_unlock(struct hwbus_priv *self)
{
	sdio_release_host(self->func);
}

static void cw1200_sdio_irq_handler(struct sdio_func *func)
{
	struct hwbus_priv *self = sdio_get_drvdata(func);

	/* note:  sdio_host already claimed here. */
	if (self->core)
		cw1200_irq_handler(self->core);
}

static irqreturn_t cw1200_gpio_hardirq(int irq, void *dev_id)
{
	return IRQ_WAKE_THREAD;
}

static irqreturn_t cw1200_gpio_irq(int irq, void *dev_id)
{
	struct hwbus_priv *self = dev_id;

	if (self->core) {
		cw1200_sdio_lock(self);
		cw1200_irq_handler(self->core);
		cw1200_sdio_unlock(self);
		return IRQ_HANDLED;
	} else {
		return IRQ_NONE;
	}
}

static int cw1200_request_irq(struct hwbus_priv *self)
{
	int ret;
	u8 cccr;

	cccr = sdio_f0_readb(self->func, SDIO_CCCR_IENx, &ret);
	if (WARN_ON(ret))
		goto err;

	/* Master interrupt enable ... */
	cccr |= BIT(0);

	/* ... for our function */
	cccr |= BIT(self->func->num);

	sdio_f0_writeb(self->func, cccr, SDIO_CCCR_IENx, &ret);
	if (WARN_ON(ret))
		goto err;

	ret = enable_irq_wake(self->pdata->irq);
	if (WARN_ON(ret))
		goto err;

	/* Request the IRQ */
	ret =  request_threaded_irq(self->pdata->irq, cw1200_gpio_hardirq,
				    cw1200_gpio_irq,
				    IRQF_TRIGGER_HIGH | IRQF_ONESHOT,
				    "cw1200_wlan_irq", self);
	if (WARN_ON(ret))
		goto err;

	return 0;

err:
	return ret;
}

static int cw1200_sdio_irq_subscribe(struct hwbus_priv *self)
{
	int ret = 0;

	pr_debug("SW IRQ subscribe\n");
	sdio_claim_host(self->func);
	if (self->pdata->irq)
		ret = cw1200_request_irq(self);
	else
		ret = sdio_claim_irq(self->func, cw1200_sdio_irq_handler);

	sdio_release_host(self->func);
	return ret;
}

static int cw1200_sdio_irq_unsubscribe(struct hwbus_priv *self)
{
	int ret = 0;

	pr_debug("SW IRQ unsubscribe\n");

	if (self->pdata->irq) {
		disable_irq_wake(self->pdata->irq);
		free_irq(self->pdata->irq, self);
	} else {
		sdio_claim_host(self->func);
		ret = sdio_release_irq(self->func);
		sdio_release_host(self->func);
	}
	return ret;
}

static int cw1200_sdio_off(const struct cw1200_platform_data_sdio *pdata)
{
	if (pdata->reset) {
		gpio_set_value(pdata->reset, 0);
		msleep(30); /* Min is 2 * CLK32K cycles */
		gpio_free(pdata->reset);
	}

	if (pdata->power_ctrl)
		pdata->power_ctrl(pdata, false);
	if (pdata->clk_ctrl)
		pdata->clk_ctrl(pdata, false);

	return 0;
}

static int cw1200_sdio_on(const struct cw1200_platform_data_sdio *pdata)
{
	/* Ensure I/Os are pulled low */
	if (pdata->reset) {
		gpio_request(pdata->reset, "cw1200_wlan_reset");
		gpio_direction_output(pdata->reset, 0);
	}
	if (pdata->powerup) {
		gpio_request(pdata->powerup, "cw1200_wlan_powerup");
		gpio_direction_output(pdata->powerup, 0);
	}
	if (pdata->reset || pdata->powerup)
		msleep(10); /* Settle time? */

	/* Enable 3v3 and 1v8 to hardware */
	if (pdata->power_ctrl) {
		if (pdata->power_ctrl(pdata, true)) {
			pr_err("power_ctrl() failed!\n");
			return -1;
		}
	}

	/* Enable CLK32K */
	if (pdata->clk_ctrl) {
		if (pdata->clk_ctrl(pdata, true)) {
			pr_err("clk_ctrl() failed!\n");
			return -1;
		}
		msleep(10); /* Delay until clock is stable for 2 cycles */
	}

	/* Enable POWERUP signal */
	if (pdata->powerup) {
		gpio_set_value(pdata->powerup, 1);
		msleep(250); /* or more..? */
	}
	/* Enable RSTn signal */
	if (pdata->reset) {
		gpio_set_value(pdata->reset, 1);
		msleep(50); /* Or more..? */
	}
	return 0;
}

static size_t cw1200_sdio_align_size(struct hwbus_priv *self, size_t size)
{
	if (self->pdata->no_nptb)
		size = round_up(size, SDIO_BLOCK_SIZE);
	else
		size = sdio_align_size(self->func, size);

	return size;
}

static int cw1200_sdio_pm(struct hwbus_priv *self, bool suspend)
{
	int ret = 0;

	if (self->pdata->irq)
		ret = irq_set_irq_wake(self->pdata->irq, suspend);
	return ret;
}

static const struct hwbus_ops cw1200_sdio_hwbus_ops = {
	.hwbus_memcpy_fromio	= cw1200_sdio_memcpy_fromio,
	.hwbus_memcpy_toio	= cw1200_sdio_memcpy_toio,
	.lock			= cw1200_sdio_lock,
	.unlock			= cw1200_sdio_unlock,
	.align_size		= cw1200_sdio_align_size,
	.power_mgmt		= cw1200_sdio_pm,
};

/* Probe Function to be called by SDIO stack when device is discovered */
static int cw1200_sdio_probe(struct sdio_func *func,
			     const struct sdio_device_id *id)
{
	struct hwbus_priv *self;
	int status;

	pr_info("cw1200_wlan_sdio: Probe called\n");

	/* We are only able to handle the wlan function */
	if (func->num != 0x01)
		return -ENODEV;

	self = kzalloc(sizeof(*self), GFP_KERNEL);
	if (!self) {
		pr_err("Can't allocate SDIO hwbus_priv.\n");
		return -ENOMEM;
	}

	func->card->quirks |= MMC_QUIRK_LENIENT_FN0;

	self->pdata = global_plat_data; /* FIXME */
	self->func = func;
	sdio_set_drvdata(func, self);
	sdio_claim_host(func);
	sdio_enable_func(func);
	sdio_release_host(func);

	status = cw1200_sdio_irq_subscribe(self);

	status = cw1200_core_probe(&cw1200_sdio_hwbus_ops,
				   self, &func->dev, &self->core,
				   self->pdata->ref_clk,
				   self->pdata->macaddr,
				   self->pdata->sdd_file,
				   self->pdata->have_5ghz);
	if (status) {
		cw1200_sdio_irq_unsubscribe(self);
		sdio_claim_host(func);
		sdio_disable_func(func);
		sdio_release_host(func);
		sdio_set_drvdata(func, NULL);
		kfree(self);
	}

	return status;
}

/* Disconnect Function to be called by SDIO stack when
 * device is disconnected
 */
static void cw1200_sdio_disconnect(struct sdio_func *func)
{
	struct hwbus_priv *self = sdio_get_drvdata(func);

	if (self) {
		cw1200_sdio_irq_unsubscribe(self);
		if (self->core) {
			cw1200_core_release(self->core);
			self->core = NULL;
		}
		sdio_claim_host(func);
		sdio_disable_func(func);
		sdio_release_host(func);
		sdio_set_drvdata(func, NULL);
		kfree(self);
	}
}

#ifdef CONFIG_PM
static int cw1200_sdio_suspend(struct device *dev)
{
	int ret;
	struct sdio_func *func = dev_to_sdio_func(dev);
	struct hwbus_priv *self = sdio_get_drvdata(func);

	if (!cw1200_can_suspend(self->core))
		return -EAGAIN;

	/* Notify SDIO that CW1200 will remain powered during suspend */
	ret = sdio_set_host_pm_flags(func, MMC_PM_KEEP_POWER);
	if (ret)
		pr_err("Error setting SDIO pm flags: %i\n", ret);

	return ret;
}

static int cw1200_sdio_resume(struct device *dev)
{
	return 0;
}

static const struct dev_pm_ops cw1200_pm_ops = {
	.suspend = cw1200_sdio_suspend,
	.resume = cw1200_sdio_resume,
};
#endif

static struct sdio_driver sdio_driver = {
	.name		= "cw1200_wlan_sdio",
	.id_table	= cw1200_sdio_ids,
	.probe		= cw1200_sdio_probe,
	.remove		= cw1200_sdio_disconnect,
#ifdef CONFIG_PM
	.drv = {
		.pm = &cw1200_pm_ops,
	}
#endif
};

/* Init Module function -> Called by insmod */
static int __init cw1200_sdio_init(void)
{
	const struct cw1200_platform_data_sdio *pdata;
	int ret;

	/* FIXME -- this won't support multiple devices */
	pdata = global_plat_data;

	if (cw1200_sdio_on(pdata)) {
		ret = -1;
		goto err;
	}

	ret = sdio_register_driver(&sdio_driver);
	if (ret)
		goto err;

	return 0;

err:
	cw1200_sdio_off(pdata);
	return ret;
}

/* Called at Driver Unloading */
static void __exit cw1200_sdio_exit(void)
{
	const struct cw1200_platform_data_sdio *pdata;

	/* FIXME -- this won't support multiple devices */
	pdata = global_plat_data;
	sdio_unregister_driver(&sdio_driver);
	cw1200_sdio_off(pdata);
}


module_init(cw1200_sdio_init);
module_exit(cw1200_sdio_exit);
