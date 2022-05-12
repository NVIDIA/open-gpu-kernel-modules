// SPDX-License-Identifier: GPL-2.0-or-later
/*

    bttv-gpio.c  --  gpio sub drivers

    sysfs-based sub driver interface for bttv
    mainly intended for gpio access


    Copyright (C) 1996,97,98 Ralph  Metzler (rjkm@thp.uni-koeln.de)
			   & Marcus Metzler (mocm@thp.uni-koeln.de)
    (c) 1999-2003 Gerd Knorr <kraxel@bytesex.org>


*/

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <asm/io.h>

#include "bttvp.h"

/* ----------------------------------------------------------------------- */
/* internal: the bttv "bus"                                                */

static int bttv_sub_bus_match(struct device *dev, struct device_driver *drv)
{
	struct bttv_sub_driver *sub = to_bttv_sub_drv(drv);
	int len = strlen(sub->wanted);

	if (0 == strncmp(dev_name(dev), sub->wanted, len))
		return 1;
	return 0;
}

static int bttv_sub_probe(struct device *dev)
{
	struct bttv_sub_device *sdev = to_bttv_sub_dev(dev);
	struct bttv_sub_driver *sub = to_bttv_sub_drv(dev->driver);

	return sub->probe ? sub->probe(sdev) : -ENODEV;
}

static int bttv_sub_remove(struct device *dev)
{
	struct bttv_sub_device *sdev = to_bttv_sub_dev(dev);
	struct bttv_sub_driver *sub = to_bttv_sub_drv(dev->driver);

	if (sub->remove)
		sub->remove(sdev);
	return 0;
}

struct bus_type bttv_sub_bus_type = {
	.name   = "bttv-sub",
	.match  = &bttv_sub_bus_match,
	.probe  = bttv_sub_probe,
	.remove = bttv_sub_remove,
};

static void release_sub_device(struct device *dev)
{
	struct bttv_sub_device *sub = to_bttv_sub_dev(dev);
	kfree(sub);
}

int bttv_sub_add_device(struct bttv_core *core, char *name)
{
	struct bttv_sub_device *sub;
	int err;

	sub = kzalloc(sizeof(*sub),GFP_KERNEL);
	if (NULL == sub)
		return -ENOMEM;

	sub->core        = core;
	sub->dev.parent  = &core->pci->dev;
	sub->dev.bus     = &bttv_sub_bus_type;
	sub->dev.release = release_sub_device;
	dev_set_name(&sub->dev, "%s%d", name, core->nr);

	err = device_register(&sub->dev);
	if (0 != err) {
		put_device(&sub->dev);
		return err;
	}
	pr_info("%d: add subdevice \"%s\"\n", core->nr, dev_name(&sub->dev));
	list_add_tail(&sub->list,&core->subs);
	return 0;
}

int bttv_sub_del_devices(struct bttv_core *core)
{
	struct bttv_sub_device *sub, *save;

	list_for_each_entry_safe(sub, save, &core->subs, list) {
		list_del(&sub->list);
		device_unregister(&sub->dev);
	}
	return 0;
}

/* ----------------------------------------------------------------------- */
/* external: sub-driver register/unregister                                */

int bttv_sub_register(struct bttv_sub_driver *sub, char *wanted)
{
	sub->drv.bus = &bttv_sub_bus_type;
	snprintf(sub->wanted,sizeof(sub->wanted),"%s",wanted);
	return driver_register(&sub->drv);
}
EXPORT_SYMBOL(bttv_sub_register);

int bttv_sub_unregister(struct bttv_sub_driver *sub)
{
	driver_unregister(&sub->drv);
	return 0;
}
EXPORT_SYMBOL(bttv_sub_unregister);

/* ----------------------------------------------------------------------- */
/* external: gpio access functions                                         */

void bttv_gpio_inout(struct bttv_core *core, u32 mask, u32 outbits)
{
	struct bttv *btv = container_of(core, struct bttv, c);
	unsigned long flags;
	u32 data;

	spin_lock_irqsave(&btv->gpio_lock,flags);
	data = btread(BT848_GPIO_OUT_EN);
	data = data & ~mask;
	data = data | (mask & outbits);
	btwrite(data,BT848_GPIO_OUT_EN);
	spin_unlock_irqrestore(&btv->gpio_lock,flags);
}

u32 bttv_gpio_read(struct bttv_core *core)
{
	struct bttv *btv = container_of(core, struct bttv, c);
	u32 value;

	value = btread(BT848_GPIO_DATA);
	return value;
}

void bttv_gpio_write(struct bttv_core *core, u32 value)
{
	struct bttv *btv = container_of(core, struct bttv, c);

	btwrite(value,BT848_GPIO_DATA);
}

void bttv_gpio_bits(struct bttv_core *core, u32 mask, u32 bits)
{
	struct bttv *btv = container_of(core, struct bttv, c);
	unsigned long flags;
	u32 data;

	spin_lock_irqsave(&btv->gpio_lock,flags);
	data = btread(BT848_GPIO_DATA);
	data = data & ~mask;
	data = data | (mask & bits);
	btwrite(data,BT848_GPIO_DATA);
	spin_unlock_irqrestore(&btv->gpio_lock,flags);
}
