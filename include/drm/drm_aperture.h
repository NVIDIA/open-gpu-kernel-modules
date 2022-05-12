/* SPDX-License-Identifier: MIT */

#ifndef _DRM_APERTURE_H_
#define _DRM_APERTURE_H_

#include <linux/types.h>

struct drm_device;
struct pci_dev;

int devm_aperture_acquire_from_firmware(struct drm_device *dev, resource_size_t base,
					resource_size_t size);

int drm_aperture_remove_conflicting_framebuffers(resource_size_t base, resource_size_t size,
						 bool primary, const char *name);

int drm_aperture_remove_conflicting_pci_framebuffers(struct pci_dev *pdev, const char *name);

/**
 * drm_aperture_remove_framebuffers - remove all existing framebuffers
 * @primary: also kick vga16fb if present
 * @name: requesting driver name
 *
 * This function removes all graphics device drivers. Use this function on systems
 * that can have their framebuffer located anywhere in memory.
 *
 * Returns:
 * 0 on success, or a negative errno code otherwise
 */
static inline int drm_aperture_remove_framebuffers(bool primary, const char *name)
{
	return drm_aperture_remove_conflicting_framebuffers(0, (resource_size_t)-1, primary, name);
}

#endif
