/*
 * Copyright © 2007 David Airlie
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
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Authors:
 *     David Airlie
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/tty.h>
#include <linux/sysrq.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/screen_info.h>
#include <linux/vga_switcheroo.h>
#include <linux/console.h>

#include <drm/drm_crtc.h>
#include <drm/drm_crtc_helper.h>
#include <drm/drm_fb_helper.h>
#include <drm/drm_fourcc.h>
#include <drm/drm_atomic.h>

#include "nouveau_drv.h"
#include "nouveau_gem.h"
#include "nouveau_bo.h"
#include "nouveau_fbcon.h"
#include "nouveau_chan.h"
#include "nouveau_vmm.h"

#include "nouveau_crtc.h"

MODULE_PARM_DESC(nofbaccel, "Disable fbcon acceleration");
int nouveau_nofbaccel = 0;
module_param_named(nofbaccel, nouveau_nofbaccel, int, 0400);

MODULE_PARM_DESC(fbcon_bpp, "fbcon bits-per-pixel (default: auto)");
static int nouveau_fbcon_bpp;
module_param_named(fbcon_bpp, nouveau_fbcon_bpp, int, 0400);

static void
nouveau_fbcon_fillrect(struct fb_info *info, const struct fb_fillrect *rect)
{
	struct nouveau_fbdev *fbcon = info->par;
	struct nouveau_drm *drm = nouveau_drm(fbcon->helper.dev);
	struct nvif_device *device = &drm->client.device;
	int ret;

	if (info->state != FBINFO_STATE_RUNNING)
		return;

	ret = -ENODEV;
	if (!in_interrupt() && !(info->flags & FBINFO_HWACCEL_DISABLED) &&
	    mutex_trylock(&drm->client.mutex)) {
		if (device->info.family < NV_DEVICE_INFO_V0_TESLA)
			ret = nv04_fbcon_fillrect(info, rect);
		else
		if (device->info.family < NV_DEVICE_INFO_V0_FERMI)
			ret = nv50_fbcon_fillrect(info, rect);
		else
			ret = nvc0_fbcon_fillrect(info, rect);
		mutex_unlock(&drm->client.mutex);
	}

	if (ret == 0)
		return;

	if (ret != -ENODEV)
		nouveau_fbcon_gpu_lockup(info);
	drm_fb_helper_cfb_fillrect(info, rect);
}

static void
nouveau_fbcon_copyarea(struct fb_info *info, const struct fb_copyarea *image)
{
	struct nouveau_fbdev *fbcon = info->par;
	struct nouveau_drm *drm = nouveau_drm(fbcon->helper.dev);
	struct nvif_device *device = &drm->client.device;
	int ret;

	if (info->state != FBINFO_STATE_RUNNING)
		return;

	ret = -ENODEV;
	if (!in_interrupt() && !(info->flags & FBINFO_HWACCEL_DISABLED) &&
	    mutex_trylock(&drm->client.mutex)) {
		if (device->info.family < NV_DEVICE_INFO_V0_TESLA)
			ret = nv04_fbcon_copyarea(info, image);
		else
		if (device->info.family < NV_DEVICE_INFO_V0_FERMI)
			ret = nv50_fbcon_copyarea(info, image);
		else
			ret = nvc0_fbcon_copyarea(info, image);
		mutex_unlock(&drm->client.mutex);
	}

	if (ret == 0)
		return;

	if (ret != -ENODEV)
		nouveau_fbcon_gpu_lockup(info);
	drm_fb_helper_cfb_copyarea(info, image);
}

static void
nouveau_fbcon_imageblit(struct fb_info *info, const struct fb_image *image)
{
	struct nouveau_fbdev *fbcon = info->par;
	struct nouveau_drm *drm = nouveau_drm(fbcon->helper.dev);
	struct nvif_device *device = &drm->client.device;
	int ret;

	if (info->state != FBINFO_STATE_RUNNING)
		return;

	ret = -ENODEV;
	if (!in_interrupt() && !(info->flags & FBINFO_HWACCEL_DISABLED) &&
	    mutex_trylock(&drm->client.mutex)) {
		if (device->info.family < NV_DEVICE_INFO_V0_TESLA)
			ret = nv04_fbcon_imageblit(info, image);
		else
		if (device->info.family < NV_DEVICE_INFO_V0_FERMI)
			ret = nv50_fbcon_imageblit(info, image);
		else
			ret = nvc0_fbcon_imageblit(info, image);
		mutex_unlock(&drm->client.mutex);
	}

	if (ret == 0)
		return;

	if (ret != -ENODEV)
		nouveau_fbcon_gpu_lockup(info);
	drm_fb_helper_cfb_imageblit(info, image);
}

static int
nouveau_fbcon_sync(struct fb_info *info)
{
	struct nouveau_fbdev *fbcon = info->par;
	struct nouveau_drm *drm = nouveau_drm(fbcon->helper.dev);
	struct nouveau_channel *chan = drm->channel;
	int ret;

	if (!chan || !chan->accel_done || in_interrupt() ||
	    info->state != FBINFO_STATE_RUNNING ||
	    info->flags & FBINFO_HWACCEL_DISABLED)
		return 0;

	if (!mutex_trylock(&drm->client.mutex))
		return 0;

	ret = nouveau_channel_idle(chan);
	mutex_unlock(&drm->client.mutex);
	if (ret) {
		nouveau_fbcon_gpu_lockup(info);
		return 0;
	}

	chan->accel_done = false;
	return 0;
}

static int
nouveau_fbcon_open(struct fb_info *info, int user)
{
	struct nouveau_fbdev *fbcon = info->par;
	struct nouveau_drm *drm = nouveau_drm(fbcon->helper.dev);
	int ret = pm_runtime_get_sync(drm->dev->dev);
	if (ret < 0 && ret != -EACCES) {
		pm_runtime_put(drm->dev->dev);
		return ret;
	}
	return 0;
}

static int
nouveau_fbcon_release(struct fb_info *info, int user)
{
	struct nouveau_fbdev *fbcon = info->par;
	struct nouveau_drm *drm = nouveau_drm(fbcon->helper.dev);
	pm_runtime_put(drm->dev->dev);
	return 0;
}

static const struct fb_ops nouveau_fbcon_ops = {
	.owner = THIS_MODULE,
	DRM_FB_HELPER_DEFAULT_OPS,
	.fb_open = nouveau_fbcon_open,
	.fb_release = nouveau_fbcon_release,
	.fb_fillrect = nouveau_fbcon_fillrect,
	.fb_copyarea = nouveau_fbcon_copyarea,
	.fb_imageblit = nouveau_fbcon_imageblit,
	.fb_sync = nouveau_fbcon_sync,
};

static const struct fb_ops nouveau_fbcon_sw_ops = {
	.owner = THIS_MODULE,
	DRM_FB_HELPER_DEFAULT_OPS,
	.fb_open = nouveau_fbcon_open,
	.fb_release = nouveau_fbcon_release,
	.fb_fillrect = drm_fb_helper_cfb_fillrect,
	.fb_copyarea = drm_fb_helper_cfb_copyarea,
	.fb_imageblit = drm_fb_helper_cfb_imageblit,
};

void
nouveau_fbcon_accel_save_disable(struct drm_device *dev)
{
	struct nouveau_drm *drm = nouveau_drm(dev);
	if (drm->fbcon && drm->fbcon->helper.fbdev) {
		drm->fbcon->saved_flags = drm->fbcon->helper.fbdev->flags;
		drm->fbcon->helper.fbdev->flags |= FBINFO_HWACCEL_DISABLED;
	}
}

void
nouveau_fbcon_accel_restore(struct drm_device *dev)
{
	struct nouveau_drm *drm = nouveau_drm(dev);
	if (drm->fbcon && drm->fbcon->helper.fbdev) {
		drm->fbcon->helper.fbdev->flags = drm->fbcon->saved_flags;
	}
}

static void
nouveau_fbcon_accel_fini(struct drm_device *dev)
{
	struct nouveau_drm *drm = nouveau_drm(dev);
	struct nouveau_fbdev *fbcon = drm->fbcon;
	if (fbcon && drm->channel) {
		console_lock();
		if (fbcon->helper.fbdev)
			fbcon->helper.fbdev->flags |= FBINFO_HWACCEL_DISABLED;
		console_unlock();
		nouveau_channel_idle(drm->channel);
		nvif_object_dtor(&fbcon->twod);
		nvif_object_dtor(&fbcon->blit);
		nvif_object_dtor(&fbcon->gdi);
		nvif_object_dtor(&fbcon->patt);
		nvif_object_dtor(&fbcon->rop);
		nvif_object_dtor(&fbcon->clip);
		nvif_object_dtor(&fbcon->surf2d);
	}
}

static void
nouveau_fbcon_accel_init(struct drm_device *dev)
{
	struct nouveau_drm *drm = nouveau_drm(dev);
	struct nouveau_fbdev *fbcon = drm->fbcon;
	struct fb_info *info = fbcon->helper.fbdev;
	int ret;

	if (drm->client.device.info.family < NV_DEVICE_INFO_V0_TESLA)
		ret = nv04_fbcon_accel_init(info);
	else
	if (drm->client.device.info.family < NV_DEVICE_INFO_V0_FERMI)
		ret = nv50_fbcon_accel_init(info);
	else
		ret = nvc0_fbcon_accel_init(info);

	if (ret == 0)
		info->fbops = &nouveau_fbcon_ops;
}

static void
nouveau_fbcon_zfill(struct drm_device *dev, struct nouveau_fbdev *fbcon)
{
	struct fb_info *info = fbcon->helper.fbdev;
	struct fb_fillrect rect;

	/* Clear the entire fbcon.  The drm will program every connector
	 * with it's preferred mode.  If the sizes differ, one display will
	 * quite likely have garbage around the console.
	 */
	rect.dx = rect.dy = 0;
	rect.width = info->var.xres_virtual;
	rect.height = info->var.yres_virtual;
	rect.color = 0;
	rect.rop = ROP_COPY;
	info->fbops->fb_fillrect(info, &rect);
}

static int
nouveau_fbcon_create(struct drm_fb_helper *helper,
		     struct drm_fb_helper_surface_size *sizes)
{
	struct nouveau_fbdev *fbcon =
		container_of(helper, struct nouveau_fbdev, helper);
	struct drm_device *dev = fbcon->helper.dev;
	struct nouveau_drm *drm = nouveau_drm(dev);
	struct nvif_device *device = &drm->client.device;
	struct fb_info *info;
	struct drm_framebuffer *fb;
	struct nouveau_channel *chan;
	struct nouveau_bo *nvbo;
	struct drm_mode_fb_cmd2 mode_cmd = {};
	int ret;

	mode_cmd.width = sizes->surface_width;
	mode_cmd.height = sizes->surface_height;

	mode_cmd.pitches[0] = mode_cmd.width * (sizes->surface_bpp >> 3);
	mode_cmd.pitches[0] = roundup(mode_cmd.pitches[0], 256);

	mode_cmd.pixel_format = drm_mode_legacy_fb_format(sizes->surface_bpp,
							  sizes->surface_depth);

	ret = nouveau_gem_new(&drm->client, mode_cmd.pitches[0] *
			      mode_cmd.height, 0, NOUVEAU_GEM_DOMAIN_VRAM,
			      0, 0x0000, &nvbo);
	if (ret) {
		NV_ERROR(drm, "failed to allocate framebuffer\n");
		goto out;
	}

	ret = nouveau_framebuffer_new(dev, &mode_cmd, &nvbo->bo.base, &fb);
	if (ret)
		goto out_unref;

	ret = nouveau_bo_pin(nvbo, NOUVEAU_GEM_DOMAIN_VRAM, false);
	if (ret) {
		NV_ERROR(drm, "failed to pin fb: %d\n", ret);
		goto out_unref;
	}

	ret = nouveau_bo_map(nvbo);
	if (ret) {
		NV_ERROR(drm, "failed to map fb: %d\n", ret);
		goto out_unpin;
	}

	chan = nouveau_nofbaccel ? NULL : drm->channel;
	if (chan && device->info.family >= NV_DEVICE_INFO_V0_TESLA) {
		ret = nouveau_vma_new(nvbo, chan->vmm, &fbcon->vma);
		if (ret) {
			NV_ERROR(drm, "failed to map fb into chan: %d\n", ret);
			chan = NULL;
		}
	}

	info = drm_fb_helper_alloc_fbi(helper);
	if (IS_ERR(info)) {
		ret = PTR_ERR(info);
		goto out_unlock;
	}

	/* setup helper */
	fbcon->helper.fb = fb;

	if (!chan)
		info->flags = FBINFO_HWACCEL_DISABLED;
	else
		info->flags = FBINFO_HWACCEL_COPYAREA |
			      FBINFO_HWACCEL_FILLRECT |
			      FBINFO_HWACCEL_IMAGEBLIT;
	info->fbops = &nouveau_fbcon_sw_ops;
	info->fix.smem_start = nvbo->bo.resource->bus.offset;
	info->fix.smem_len = nvbo->bo.base.size;

	info->screen_base = nvbo_kmap_obj_iovirtual(nvbo);
	info->screen_size = nvbo->bo.base.size;

	drm_fb_helper_fill_info(info, &fbcon->helper, sizes);

	/* Use default scratch pixmap (info->pixmap.flags = FB_PIXMAP_SYSTEM) */

	if (chan)
		nouveau_fbcon_accel_init(dev);
	nouveau_fbcon_zfill(dev, fbcon);

	/* To allow resizeing without swapping buffers */
	NV_INFO(drm, "allocated %dx%d fb: 0x%llx, bo %p\n",
		fb->width, fb->height, nvbo->offset, nvbo);

	if (dev_is_pci(dev->dev))
		vga_switcheroo_client_fb_set(to_pci_dev(dev->dev), info);

	return 0;

out_unlock:
	if (chan)
		nouveau_vma_del(&fbcon->vma);
	nouveau_bo_unmap(nvbo);
out_unpin:
	nouveau_bo_unpin(nvbo);
out_unref:
	nouveau_bo_ref(NULL, &nvbo);
out:
	return ret;
}

static int
nouveau_fbcon_destroy(struct drm_device *dev, struct nouveau_fbdev *fbcon)
{
	struct drm_framebuffer *fb = fbcon->helper.fb;
	struct nouveau_bo *nvbo;

	drm_fb_helper_unregister_fbi(&fbcon->helper);
	drm_fb_helper_fini(&fbcon->helper);

	if (fb && fb->obj[0]) {
		nvbo = nouveau_gem_object(fb->obj[0]);
		nouveau_vma_del(&fbcon->vma);
		nouveau_bo_unmap(nvbo);
		nouveau_bo_unpin(nvbo);
		drm_framebuffer_put(fb);
	}

	return 0;
}

void nouveau_fbcon_gpu_lockup(struct fb_info *info)
{
	struct nouveau_fbdev *fbcon = info->par;
	struct nouveau_drm *drm = nouveau_drm(fbcon->helper.dev);

	NV_ERROR(drm, "GPU lockup - switching to software fbcon\n");
	info->flags |= FBINFO_HWACCEL_DISABLED;
}

static const struct drm_fb_helper_funcs nouveau_fbcon_helper_funcs = {
	.fb_probe = nouveau_fbcon_create,
};

static void
nouveau_fbcon_set_suspend_work(struct work_struct *work)
{
	struct nouveau_drm *drm = container_of(work, typeof(*drm), fbcon_work);
	int state = READ_ONCE(drm->fbcon_new_state);

	if (state == FBINFO_STATE_RUNNING)
		pm_runtime_get_sync(drm->dev->dev);

	console_lock();
	if (state == FBINFO_STATE_RUNNING)
		nouveau_fbcon_accel_restore(drm->dev);
	drm_fb_helper_set_suspend(&drm->fbcon->helper, state);
	if (state != FBINFO_STATE_RUNNING)
		nouveau_fbcon_accel_save_disable(drm->dev);
	console_unlock();

	if (state == FBINFO_STATE_RUNNING) {
		nouveau_fbcon_hotplug_resume(drm->fbcon);
		pm_runtime_mark_last_busy(drm->dev->dev);
		pm_runtime_put_sync(drm->dev->dev);
	}
}

void
nouveau_fbcon_set_suspend(struct drm_device *dev, int state)
{
	struct nouveau_drm *drm = nouveau_drm(dev);

	if (!drm->fbcon)
		return;

	drm->fbcon_new_state = state;
	/* Since runtime resume can happen as a result of a sysfs operation,
	 * it's possible we already have the console locked. So handle fbcon
	 * init/deinit from a seperate work thread
	 */
	schedule_work(&drm->fbcon_work);
}

void
nouveau_fbcon_output_poll_changed(struct drm_device *dev)
{
	struct nouveau_drm *drm = nouveau_drm(dev);
	struct nouveau_fbdev *fbcon = drm->fbcon;
	int ret;

	if (!fbcon)
		return;

	mutex_lock(&fbcon->hotplug_lock);

	ret = pm_runtime_get(dev->dev);
	if (ret == 1 || ret == -EACCES) {
		drm_fb_helper_hotplug_event(&fbcon->helper);

		pm_runtime_mark_last_busy(dev->dev);
		pm_runtime_put_autosuspend(dev->dev);
	} else if (ret == 0) {
		/* If the GPU was already in the process of suspending before
		 * this event happened, then we can't block here as we'll
		 * deadlock the runtime pmops since they wait for us to
		 * finish. So, just defer this event for when we runtime
		 * resume again. It will be handled by fbcon_work.
		 */
		NV_DEBUG(drm, "fbcon HPD event deferred until runtime resume\n");
		fbcon->hotplug_waiting = true;
		pm_runtime_put_noidle(drm->dev->dev);
	} else {
		DRM_WARN("fbcon HPD event lost due to RPM failure: %d\n",
			 ret);
	}

	mutex_unlock(&fbcon->hotplug_lock);
}

void
nouveau_fbcon_hotplug_resume(struct nouveau_fbdev *fbcon)
{
	struct nouveau_drm *drm;

	if (!fbcon)
		return;
	drm = nouveau_drm(fbcon->helper.dev);

	mutex_lock(&fbcon->hotplug_lock);
	if (fbcon->hotplug_waiting) {
		fbcon->hotplug_waiting = false;

		NV_DEBUG(drm, "Handling deferred fbcon HPD events\n");
		drm_fb_helper_hotplug_event(&fbcon->helper);
	}
	mutex_unlock(&fbcon->hotplug_lock);
}

int
nouveau_fbcon_init(struct drm_device *dev)
{
	struct nouveau_drm *drm = nouveau_drm(dev);
	struct nouveau_fbdev *fbcon;
	int preferred_bpp = nouveau_fbcon_bpp;
	int ret;

	if (!dev->mode_config.num_crtc ||
	    (to_pci_dev(dev->dev)->class >> 8) != PCI_CLASS_DISPLAY_VGA)
		return 0;

	fbcon = kzalloc(sizeof(struct nouveau_fbdev), GFP_KERNEL);
	if (!fbcon)
		return -ENOMEM;

	drm->fbcon = fbcon;
	INIT_WORK(&drm->fbcon_work, nouveau_fbcon_set_suspend_work);
	mutex_init(&fbcon->hotplug_lock);

	drm_fb_helper_prepare(dev, &fbcon->helper, &nouveau_fbcon_helper_funcs);

	ret = drm_fb_helper_init(dev, &fbcon->helper);
	if (ret)
		goto free;

	if (preferred_bpp != 8 && preferred_bpp != 16 && preferred_bpp != 32) {
		if (drm->client.device.info.ram_size <= 32 * 1024 * 1024)
			preferred_bpp = 8;
		else
		if (drm->client.device.info.ram_size <= 64 * 1024 * 1024)
			preferred_bpp = 16;
		else
			preferred_bpp = 32;
	}

	/* disable all the possible outputs/crtcs before entering KMS mode */
	if (!drm_drv_uses_atomic_modeset(dev))
		drm_helper_disable_unused_functions(dev);

	ret = drm_fb_helper_initial_config(&fbcon->helper, preferred_bpp);
	if (ret)
		goto fini;

	if (fbcon->helper.fbdev)
		fbcon->helper.fbdev->pixmap.buf_align = 4;
	return 0;

fini:
	drm_fb_helper_fini(&fbcon->helper);
free:
	kfree(fbcon);
	drm->fbcon = NULL;
	return ret;
}

void
nouveau_fbcon_fini(struct drm_device *dev)
{
	struct nouveau_drm *drm = nouveau_drm(dev);

	if (!drm->fbcon)
		return;

	nouveau_fbcon_accel_fini(dev);
	nouveau_fbcon_destroy(dev, drm->fbcon);
	kfree(drm->fbcon);
	drm->fbcon = NULL;
}
