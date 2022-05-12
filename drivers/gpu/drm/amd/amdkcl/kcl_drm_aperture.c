// SPDX-License-Identifier: MIT

#ifndef HAVE_DRM_DRM_APERTURE_H

#include <linux/fb.h>
#include <linux/vgaarb.h>

#include <drm/drm_aperture.h>
#include <kcl/kcl_drm_aperture.h>
#include "kcl_fbmem.h"

int drm_aperture_remove_conflicting_pci_framebuffers(struct pci_dev *pdev, const char *name)
{
	int ret = 0;

	/*
	 * WARNING: Apparently we must kick fbdev drivers before vgacon,
	 * otherwise the vga fbdev driver falls over.
	 */
#if IS_REACHABLE(CONFIG_FB)
	ret = _kcl_remove_conflicting_pci_framebuffers(pdev, name);
#endif
#ifdef HAVE_VGA_REMOVE_VGACON
	if (ret == 0)
		ret = vga_remove_vgacon(pdev);
#endif
	return ret;
}
EXPORT_SYMBOL(drm_aperture_remove_conflicting_pci_framebuffers);
#endif /* HAVE_DRM_DRM_APERTURE_H */
