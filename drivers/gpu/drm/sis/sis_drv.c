/* sis.c -- sis driver -*- linux-c -*-
 *
 * Copyright 1999 Precision Insight, Inc., Cedar Park, Texas.
 * Copyright 2000 VA Linux Systems, Inc., Sunnyvale, California.
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * PRECISION INSIGHT AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

#include <linux/module.h>
#include <linux/pci.h>

#include <drm/drm_drv.h>
#include <drm/drm_file.h>
#include <drm/drm_pciids.h>
#include <drm/sis_drm.h>

#include "sis_drv.h"

static struct pci_device_id pciidlist[] = {
	sisdrv_PCI_IDS
};

static int sis_driver_load(struct drm_device *dev, unsigned long chipset)
{
	struct pci_dev *pdev = to_pci_dev(dev->dev);
	drm_sis_private_t *dev_priv;

	pci_set_master(pdev);

	dev_priv = kzalloc(sizeof(drm_sis_private_t), GFP_KERNEL);
	if (dev_priv == NULL)
		return -ENOMEM;

	idr_init(&dev_priv->object_idr);
	dev->dev_private = (void *)dev_priv;
	dev_priv->chipset = chipset;

	return 0;
}

static void sis_driver_unload(struct drm_device *dev)
{
	drm_sis_private_t *dev_priv = dev->dev_private;

	idr_destroy(&dev_priv->object_idr);

	kfree(dev_priv);
}

static const struct file_operations sis_driver_fops = {
	.owner = THIS_MODULE,
	.open = drm_open,
	.release = drm_release,
	.unlocked_ioctl = drm_ioctl,
	.mmap = drm_legacy_mmap,
	.poll = drm_poll,
	.compat_ioctl = drm_compat_ioctl,
	.llseek = noop_llseek,
};

static int sis_driver_open(struct drm_device *dev, struct drm_file *file)
{
	struct sis_file_private *file_priv;

	DRM_DEBUG_DRIVER("\n");
	file_priv = kmalloc(sizeof(*file_priv), GFP_KERNEL);
	if (!file_priv)
		return -ENOMEM;

	file->driver_priv = file_priv;

	INIT_LIST_HEAD(&file_priv->obj_list);

	return 0;
}

static void sis_driver_postclose(struct drm_device *dev, struct drm_file *file)
{
	struct sis_file_private *file_priv = file->driver_priv;

	kfree(file_priv);
}

static struct drm_driver driver = {
	.driver_features = DRIVER_USE_AGP | DRIVER_LEGACY,
	.load = sis_driver_load,
	.unload = sis_driver_unload,
	.open = sis_driver_open,
	.preclose = sis_reclaim_buffers_locked,
	.postclose = sis_driver_postclose,
	.dma_quiescent = sis_idle,
	.lastclose = sis_lastclose,
	.ioctls = sis_ioctls,
	.fops = &sis_driver_fops,
	.name = DRIVER_NAME,
	.desc = DRIVER_DESC,
	.date = DRIVER_DATE,
	.major = DRIVER_MAJOR,
	.minor = DRIVER_MINOR,
	.patchlevel = DRIVER_PATCHLEVEL,
};

static struct pci_driver sis_pci_driver = {
	.name = DRIVER_NAME,
	.id_table = pciidlist,
};

static int __init sis_init(void)
{
	driver.num_ioctls = sis_max_ioctl;
	return drm_legacy_pci_init(&driver, &sis_pci_driver);
}

static void __exit sis_exit(void)
{
	drm_legacy_pci_exit(&driver, &sis_pci_driver);
}

module_init(sis_init);
module_exit(sis_exit);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL and additional rights");
