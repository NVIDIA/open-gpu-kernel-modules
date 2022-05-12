/* SPDX-License-Identifier: MIT */
#ifndef KCL_KCL_DRM_APERTURE_H
#define KCL_KCL_DRM_APERTURE_H

#ifndef HAVE_DRM_DRM_APERTURE_H

#include <linux/types.h>

/* Copied from drm/drm_aperture.h */
struct drm_device;
struct pci_dev;

int drm_aperture_remove_conflicting_pci_framebuffers(struct pci_dev *pdev, const char *name);

#endif /* HAVE_DRM_DRM_APERTURE_H */

#endif
