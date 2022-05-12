/*
 * Copyright 2003 José Fonseca.
 * Copyright 2003 Leif Delgass.
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <linux/dma-mapping.h>
#include <linux/export.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/pci.h>
#include <linux/slab.h>

#include <drm/drm.h>
#include <drm/drm_drv.h>
#include <drm/drm_print.h>

#include "drm_internal.h"
#include "drm_legacy.h"

#ifdef CONFIG_DRM_LEGACY
/* List of devices hanging off drivers with stealth attach. */
static LIST_HEAD(legacy_dev_list);
static DEFINE_MUTEX(legacy_dev_list_lock);
#endif

static int drm_get_pci_domain(struct drm_device *dev)
{
#ifndef __alpha__
	/* For historical reasons, drm_get_pci_domain() is busticated
	 * on most archs and has to remain so for userspace interface
	 * < 1.4, except on alpha which was right from the beginning
	 */
	if (dev->if_version < 0x10004)
		return 0;
#endif /* __alpha__ */

	return pci_domain_nr(to_pci_dev(dev->dev)->bus);
}

int drm_pci_set_busid(struct drm_device *dev, struct drm_master *master)
{
	struct pci_dev *pdev = to_pci_dev(dev->dev);

	master->unique = kasprintf(GFP_KERNEL, "pci:%04x:%02x:%02x.%d",
					drm_get_pci_domain(dev),
					pdev->bus->number,
					PCI_SLOT(pdev->devfn),
					PCI_FUNC(pdev->devfn));
	if (!master->unique)
		return -ENOMEM;

	master->unique_len = strlen(master->unique);
	return 0;
}

#ifdef CONFIG_DRM_LEGACY

static int drm_legacy_pci_irq_by_busid(struct drm_device *dev, struct drm_irq_busid *p)
{
	struct pci_dev *pdev = to_pci_dev(dev->dev);

	if ((p->busnum >> 8) != drm_get_pci_domain(dev) ||
	    (p->busnum & 0xff) != pdev->bus->number ||
	    p->devnum != PCI_SLOT(pdev->devfn) || p->funcnum != PCI_FUNC(pdev->devfn))
		return -EINVAL;

	p->irq = pdev->irq;

	DRM_DEBUG("%d:%d:%d => IRQ %d\n", p->busnum, p->devnum, p->funcnum,
		  p->irq);
	return 0;
}

/**
 * drm_legacy_irq_by_busid - Get interrupt from bus ID
 * @dev: DRM device
 * @data: IOCTL parameter pointing to a drm_irq_busid structure
 * @file_priv: DRM file private.
 *
 * Finds the PCI device with the specified bus id and gets its IRQ number.
 * This IOCTL is deprecated, and will now return EINVAL for any busid not equal
 * to that of the device that this DRM instance attached to.
 *
 * Return: 0 on success or a negative error code on failure.
 */
int drm_legacy_irq_by_busid(struct drm_device *dev, void *data,
			    struct drm_file *file_priv)
{
	struct drm_irq_busid *p = data;

	if (!drm_core_check_feature(dev, DRIVER_LEGACY))
		return -EOPNOTSUPP;

	/* UMS was only ever support on PCI devices. */
	if (WARN_ON(!dev_is_pci(dev->dev)))
		return -EINVAL;

	if (!drm_core_check_feature(dev, DRIVER_HAVE_IRQ))
		return -EOPNOTSUPP;

	return drm_legacy_pci_irq_by_busid(dev, p);
}

void drm_legacy_pci_agp_destroy(struct drm_device *dev)
{
	if (dev->agp) {
		arch_phys_wc_del(dev->agp->agp_mtrr);
		drm_legacy_agp_clear(dev);
		kfree(dev->agp);
		dev->agp = NULL;
	}
}

static void drm_legacy_pci_agp_init(struct drm_device *dev)
{
	if (drm_core_check_feature(dev, DRIVER_USE_AGP)) {
		if (pci_find_capability(to_pci_dev(dev->dev), PCI_CAP_ID_AGP))
			dev->agp = drm_legacy_agp_init(dev);
		if (dev->agp) {
			dev->agp->agp_mtrr = arch_phys_wc_add(
				dev->agp->agp_info.aper_base,
				dev->agp->agp_info.aper_size *
				1024 * 1024);
		}
	}
}

static int drm_legacy_get_pci_dev(struct pci_dev *pdev,
				  const struct pci_device_id *ent,
				  const struct drm_driver *driver)
{
	struct drm_device *dev;
	int ret;

	DRM_DEBUG("\n");

	dev = drm_dev_alloc(driver, &pdev->dev);
	if (IS_ERR(dev))
		return PTR_ERR(dev);

	ret = pci_enable_device(pdev);
	if (ret)
		goto err_free;

#ifdef __alpha__
	dev->hose = pdev->sysdata;
#endif

	drm_legacy_pci_agp_init(dev);

	ret = drm_dev_register(dev, ent->driver_data);
	if (ret)
		goto err_agp;

	if (drm_core_check_feature(dev, DRIVER_LEGACY)) {
		mutex_lock(&legacy_dev_list_lock);
		list_add_tail(&dev->legacy_dev_list, &legacy_dev_list);
		mutex_unlock(&legacy_dev_list_lock);
	}

	return 0;

err_agp:
	drm_legacy_pci_agp_destroy(dev);
	pci_disable_device(pdev);
err_free:
	drm_dev_put(dev);
	return ret;
}

/**
 * drm_legacy_pci_init - shadow-attach a legacy DRM PCI driver
 * @driver: DRM device driver
 * @pdriver: PCI device driver
 *
 * This is only used by legacy dri1 drivers and deprecated.
 *
 * Return: 0 on success or a negative error code on failure.
 */
int drm_legacy_pci_init(const struct drm_driver *driver,
			struct pci_driver *pdriver)
{
	struct pci_dev *pdev = NULL;
	const struct pci_device_id *pid;
	int i;

	DRM_DEBUG("\n");

	if (WARN_ON(!(driver->driver_features & DRIVER_LEGACY)))
		return -EINVAL;

	/* If not using KMS, fall back to stealth mode manual scanning. */
	for (i = 0; pdriver->id_table[i].vendor != 0; i++) {
		pid = &pdriver->id_table[i];

		/* Loop around setting up a DRM device for each PCI device
		 * matching our ID and device class.  If we had the internal
		 * function that pci_get_subsys and pci_get_class used, we'd
		 * be able to just pass pid in instead of doing a two-stage
		 * thing.
		 */
		pdev = NULL;
		while ((pdev =
			pci_get_subsys(pid->vendor, pid->device, pid->subvendor,
				       pid->subdevice, pdev)) != NULL) {
			if ((pdev->class & pid->class_mask) != pid->class)
				continue;

			/* stealth mode requires a manual probe */
			pci_dev_get(pdev);
			drm_legacy_get_pci_dev(pdev, pid, driver);
		}
	}
	return 0;
}
EXPORT_SYMBOL(drm_legacy_pci_init);

/**
 * drm_legacy_pci_exit - unregister shadow-attach legacy DRM driver
 * @driver: DRM device driver
 * @pdriver: PCI device driver
 *
 * Unregister a DRM driver shadow-attached through drm_legacy_pci_init(). This
 * is deprecated and only used by dri1 drivers.
 */
void drm_legacy_pci_exit(const struct drm_driver *driver,
			 struct pci_driver *pdriver)
{
	struct drm_device *dev, *tmp;

	DRM_DEBUG("\n");

	if (!(driver->driver_features & DRIVER_LEGACY)) {
		WARN_ON(1);
	} else {
		mutex_lock(&legacy_dev_list_lock);
		list_for_each_entry_safe(dev, tmp, &legacy_dev_list,
					 legacy_dev_list) {
			if (dev->driver == driver) {
				list_del(&dev->legacy_dev_list);
				drm_put_dev(dev);
			}
		}
		mutex_unlock(&legacy_dev_list_lock);
	}
	DRM_INFO("Module unloaded\n");
}
EXPORT_SYMBOL(drm_legacy_pci_exit);

#endif
