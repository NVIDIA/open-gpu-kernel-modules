/*
 *  linux/drivers/video/fbmem.c
 *
 *  Copyright (C) 1994 Martin Schaller
 *
 *	2001 - Documented with DocBook
 *	- Brad Douglas <brad@neruo.com>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of this archive
 * for more details.
 */

#include <kcl/kcl_drm_fb.h>

/* Copied from drivers/video/fbdev/core/fbmem.c and modified for KCL */
#if !defined(HAVE_REMOVE_CONFLICTING_PCI_FRAMEBUFFERS) && \
	!defined(HAVE_DRM_DRM_APERTURE_H)
int remove_conflicting_pci_framebuffers(struct pci_dev *pdev, const char *name)
{
	struct apertures_struct *ap;
	bool primary = false;
	int err, idx, bar;

	for (idx = 0, bar = 0; bar < PCI_ROM_RESOURCE; bar++) {
		if (!(pci_resource_flags(pdev, bar) & IORESOURCE_MEM))
			continue;
		idx++;
	}

	ap = alloc_apertures(idx);
	if (!ap)
		return -ENOMEM;

	for (idx = 0, bar = 0; bar < PCI_ROM_RESOURCE; bar++) {
		if (!(pci_resource_flags(pdev, bar) & IORESOURCE_MEM))
			continue;
		ap->ranges[idx].base = pci_resource_start(pdev, bar);
		ap->ranges[idx].size = pci_resource_len(pdev, bar);
		dev_dbg(&pdev->dev, "%s: bar %d: 0x%lx -> 0x%lx\n", __func__, bar,
			(unsigned long)pci_resource_start(pdev, bar),
			(unsigned long)pci_resource_end(pdev, bar));
		idx++;
	}

#ifdef CONFIG_X86
	primary = pdev->resource[PCI_ROM_RESOURCE].flags &
					IORESOURCE_ROM_SHADOW;
#endif
	err = remove_conflicting_framebuffers(ap, name, primary);
	kfree(ap);
	return err;
}
EXPORT_SYMBOL(remove_conflicting_pci_framebuffers);
#endif

#ifndef HAVE_IS_FIRMWARE_FRAMEBUFFER
bool is_firmware_framebuffer(struct apertures_struct *a)
{
	pr_warn_once("%s:enable the runtime pm\n", __func__);
	return false;
}
EXPORT_SYMBOL(is_firmware_framebuffer);
#endif
