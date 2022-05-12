/*
 * linux/drivers/video/hecubafb.c -- FB driver for Hecuba/Apollo controller
 *
 * Copyright (C) 2006, Jaya Kumar
 * This work was sponsored by CIS(M) Sdn Bhd
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License. See the file COPYING in the main directory of this archive for
 * more details.
 *
 * Layout is based on skeletonfb.c by James Simmons and Geert Uytterhoeven.
 * This work was possible because of apollo display code from E-Ink's website
 * http://support.eink.com/community
 * All information used to write this code is from public material made
 * available by E-Ink on its support site. Some commands such as 0xA4
 * were found by looping through cmd=0x00 thru 0xFF and supplying random
 * values. There are other commands that the display is capable of,
 * beyond the 5 used here but they are more complex.
 *
 * This driver is written to be used with the Hecuba display architecture.
 * The actual display chip is called Apollo and the interface electronics
 * it needs is called Hecuba.
 *
 * It is intended to be architecture independent. A board specific driver
 * must be used to perform all the physical IO interactions. An example
 * is provided as n411.c
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/list.h>
#include <linux/uaccess.h>

#include <video/hecubafb.h>

/* Display specific information */
#define DPY_W 600
#define DPY_H 800

static const struct fb_fix_screeninfo hecubafb_fix = {
	.id =		"hecubafb",
	.type =		FB_TYPE_PACKED_PIXELS,
	.visual =	FB_VISUAL_MONO01,
	.xpanstep =	0,
	.ypanstep =	0,
	.ywrapstep =	0,
	.line_length =	DPY_W,
	.accel =	FB_ACCEL_NONE,
};

static const struct fb_var_screeninfo hecubafb_var = {
	.xres		= DPY_W,
	.yres		= DPY_H,
	.xres_virtual	= DPY_W,
	.yres_virtual	= DPY_H,
	.bits_per_pixel	= 1,
	.nonstd		= 1,
};

/* main hecubafb functions */

static void apollo_send_data(struct hecubafb_par *par, unsigned char data)
{
	/* set data */
	par->board->set_data(par, data);

	/* set DS low */
	par->board->set_ctl(par, HCB_DS_BIT, 0);

	/* wait for ack */
	par->board->wait_for_ack(par, 0);

	/* set DS hi */
	par->board->set_ctl(par, HCB_DS_BIT, 1);

	/* wait for ack to clear */
	par->board->wait_for_ack(par, 1);
}

static void apollo_send_command(struct hecubafb_par *par, unsigned char data)
{
	/* command so set CD to high */
	par->board->set_ctl(par, HCB_CD_BIT, 1);

	/* actually strobe with command */
	apollo_send_data(par, data);

	/* clear CD back to low */
	par->board->set_ctl(par, HCB_CD_BIT, 0);
}

static void hecubafb_dpy_update(struct hecubafb_par *par)
{
	int i;
	unsigned char *buf = (unsigned char __force *)par->info->screen_base;

	apollo_send_command(par, APOLLO_START_NEW_IMG);

	for (i=0; i < (DPY_W*DPY_H/8); i++) {
		apollo_send_data(par, *(buf++));
	}

	apollo_send_command(par, APOLLO_STOP_IMG_DATA);
	apollo_send_command(par, APOLLO_DISPLAY_IMG);
}

/* this is called back from the deferred io workqueue */
static void hecubafb_dpy_deferred_io(struct fb_info *info,
				struct list_head *pagelist)
{
	hecubafb_dpy_update(info->par);
}

static void hecubafb_fillrect(struct fb_info *info,
				   const struct fb_fillrect *rect)
{
	struct hecubafb_par *par = info->par;

	sys_fillrect(info, rect);

	hecubafb_dpy_update(par);
}

static void hecubafb_copyarea(struct fb_info *info,
				   const struct fb_copyarea *area)
{
	struct hecubafb_par *par = info->par;

	sys_copyarea(info, area);

	hecubafb_dpy_update(par);
}

static void hecubafb_imageblit(struct fb_info *info,
				const struct fb_image *image)
{
	struct hecubafb_par *par = info->par;

	sys_imageblit(info, image);

	hecubafb_dpy_update(par);
}

/*
 * this is the slow path from userspace. they can seek and write to
 * the fb. it's inefficient to do anything less than a full screen draw
 */
static ssize_t hecubafb_write(struct fb_info *info, const char __user *buf,
				size_t count, loff_t *ppos)
{
	struct hecubafb_par *par = info->par;
	unsigned long p = *ppos;
	void *dst;
	int err = 0;
	unsigned long total_size;

	if (info->state != FBINFO_STATE_RUNNING)
		return -EPERM;

	total_size = info->fix.smem_len;

	if (p > total_size)
		return -EFBIG;

	if (count > total_size) {
		err = -EFBIG;
		count = total_size;
	}

	if (count + p > total_size) {
		if (!err)
			err = -ENOSPC;

		count = total_size - p;
	}

	dst = (void __force *) (info->screen_base + p);

	if (copy_from_user(dst, buf, count))
		err = -EFAULT;

	if  (!err)
		*ppos += count;

	hecubafb_dpy_update(par);

	return (err) ? err : count;
}

static const struct fb_ops hecubafb_ops = {
	.owner		= THIS_MODULE,
	.fb_read        = fb_sys_read,
	.fb_write	= hecubafb_write,
	.fb_fillrect	= hecubafb_fillrect,
	.fb_copyarea	= hecubafb_copyarea,
	.fb_imageblit	= hecubafb_imageblit,
};

static struct fb_deferred_io hecubafb_defio = {
	.delay		= HZ,
	.deferred_io	= hecubafb_dpy_deferred_io,
};

static int hecubafb_probe(struct platform_device *dev)
{
	struct fb_info *info;
	struct hecuba_board *board;
	int retval = -ENOMEM;
	int videomemorysize;
	unsigned char *videomemory;
	struct hecubafb_par *par;

	/* pick up board specific routines */
	board = dev->dev.platform_data;
	if (!board)
		return -EINVAL;

	/* try to count device specific driver, if can't, platform recalls */
	if (!try_module_get(board->owner))
		return -ENODEV;

	videomemorysize = (DPY_W*DPY_H)/8;

	videomemory = vzalloc(videomemorysize);
	if (!videomemory)
		goto err_videomem_alloc;

	info = framebuffer_alloc(sizeof(struct hecubafb_par), &dev->dev);
	if (!info)
		goto err_fballoc;

	info->screen_base = (char __force __iomem *)videomemory;
	info->fbops = &hecubafb_ops;

	info->var = hecubafb_var;
	info->fix = hecubafb_fix;
	info->fix.smem_len = videomemorysize;
	par = info->par;
	par->info = info;
	par->board = board;
	par->send_command = apollo_send_command;
	par->send_data = apollo_send_data;

	info->flags = FBINFO_FLAG_DEFAULT | FBINFO_VIRTFB;

	info->fbdefio = &hecubafb_defio;
	fb_deferred_io_init(info);

	retval = register_framebuffer(info);
	if (retval < 0)
		goto err_fbreg;
	platform_set_drvdata(dev, info);

	fb_info(info, "Hecuba frame buffer device, using %dK of video memory\n",
		videomemorysize >> 10);

	/* this inits the dpy */
	retval = par->board->init(par);
	if (retval < 0)
		goto err_fbreg;

	return 0;
err_fbreg:
	framebuffer_release(info);
err_fballoc:
	vfree(videomemory);
err_videomem_alloc:
	module_put(board->owner);
	return retval;
}

static int hecubafb_remove(struct platform_device *dev)
{
	struct fb_info *info = platform_get_drvdata(dev);

	if (info) {
		struct hecubafb_par *par = info->par;
		fb_deferred_io_cleanup(info);
		unregister_framebuffer(info);
		vfree((void __force *)info->screen_base);
		if (par->board->remove)
			par->board->remove(par);
		module_put(par->board->owner);
		framebuffer_release(info);
	}
	return 0;
}

static struct platform_driver hecubafb_driver = {
	.probe	= hecubafb_probe,
	.remove = hecubafb_remove,
	.driver	= {
		.name	= "hecubafb",
	},
};
module_platform_driver(hecubafb_driver);

MODULE_DESCRIPTION("fbdev driver for Hecuba/Apollo controller");
MODULE_AUTHOR("Jaya Kumar");
MODULE_LICENSE("GPL");
