/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _AMDKCL_KCL_FBMEM_H_
#define _AMDKCL_KCL_FBMEM_H_

#include <linux/fb.h>
#include <linux/pci.h>

/* Copied from include/linux/fb.h */
#if !defined(HAVE_REMOVE_CONFLICTING_PCI_FRAMEBUFFERS) && \
	!defined(HAVE_DRM_DRM_APERTURE_H)
extern int remove_conflicting_pci_framebuffers(struct pci_dev *pdev,
					       const char *name);
#endif
static inline
int _kcl_remove_conflicting_pci_framebuffers(struct pci_dev *pdev,
						       const char *name)
{
#ifdef HAVE_REMOVE_CONFLICTING_PCI_FRAMEBUFFERS_PIP
	/**
	 * v5.1-rc3-20-gb0e999c95581 fbdev: list all pci memory bars as conflicting apertures
	 * handle bar 0 directly.
	 * as remove_conflicting_pci_framebuffers() for bar 2/5 fails on rhel7.9
	int bar, err;

	for (bar = 0; bar < PCI_ROM_RESOURCE; bar++) {
		if (!(pci_resource_flags(pdev, bar) & IORESOURCE_MEM))
			continue;
		err = remove_conflicting_pci_framebuffers(pdev, bar, name);
		if (err)
			return err;
	}
	*/
	pr_warn_once("remove conflicting pci framebuffers on bar 0\n");
	return remove_conflicting_pci_framebuffers(pdev, 0, name);
#else
	return remove_conflicting_pci_framebuffers(pdev, name);
#endif
}
#endif
