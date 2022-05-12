// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2013 Intel Corporation; author Matt Fleming
 */

#include <linux/console.h>
#include <linux/efi.h>
#include <linux/font.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/serial_core.h>
#include <linux/screen_info.h>

#include <asm/early_ioremap.h>

static const struct console *earlycon_console __initdata;
static const struct font_desc *font;
static u32 efi_x, efi_y;
static u64 fb_base;
static bool fb_wb;
static void *efi_fb;

/*
 * EFI earlycon needs to use early_memremap() to map the framebuffer.
 * But early_memremap() is not usable for 'earlycon=efifb keep_bootcon',
 * memremap() should be used instead. memremap() will be available after
 * paging_init() which is earlier than initcall callbacks. Thus adding this
 * early initcall function early_efi_map_fb() to map the whole EFI framebuffer.
 */
static int __init efi_earlycon_remap_fb(void)
{
	/* bail if there is no bootconsole or it has been disabled already */
	if (!earlycon_console || !(earlycon_console->flags & CON_ENABLED))
		return 0;

	efi_fb = memremap(fb_base, screen_info.lfb_size,
			  fb_wb ? MEMREMAP_WB : MEMREMAP_WC);

	return efi_fb ? 0 : -ENOMEM;
}
early_initcall(efi_earlycon_remap_fb);

static int __init efi_earlycon_unmap_fb(void)
{
	/* unmap the bootconsole fb unless keep_bootcon has left it enabled */
	if (efi_fb && !(earlycon_console->flags & CON_ENABLED))
		memunmap(efi_fb);
	return 0;
}
late_initcall(efi_earlycon_unmap_fb);

static __ref void *efi_earlycon_map(unsigned long start, unsigned long len)
{
	pgprot_t fb_prot;

	if (efi_fb)
		return efi_fb + start;

	fb_prot = fb_wb ? PAGE_KERNEL : pgprot_writecombine(PAGE_KERNEL);
	return early_memremap_prot(fb_base + start, len, pgprot_val(fb_prot));
}

static __ref void efi_earlycon_unmap(void *addr, unsigned long len)
{
	if (efi_fb)
		return;

	early_memunmap(addr, len);
}

static void efi_earlycon_clear_scanline(unsigned int y)
{
	unsigned long *dst;
	u16 len;

	len = screen_info.lfb_linelength;
	dst = efi_earlycon_map(y*len, len);
	if (!dst)
		return;

	memset(dst, 0, len);
	efi_earlycon_unmap(dst, len);
}

static void efi_earlycon_scroll_up(void)
{
	unsigned long *dst, *src;
	u16 len;
	u32 i, height;

	len = screen_info.lfb_linelength;
	height = screen_info.lfb_height;

	for (i = 0; i < height - font->height; i++) {
		dst = efi_earlycon_map(i*len, len);
		if (!dst)
			return;

		src = efi_earlycon_map((i + font->height) * len, len);
		if (!src) {
			efi_earlycon_unmap(dst, len);
			return;
		}

		memmove(dst, src, len);

		efi_earlycon_unmap(src, len);
		efi_earlycon_unmap(dst, len);
	}
}

static void efi_earlycon_write_char(u32 *dst, unsigned char c, unsigned int h)
{
	const u32 color_black = 0x00000000;
	const u32 color_white = 0x00ffffff;
	const u8 *src;
	int m, n, bytes;
	u8 x;

	bytes = BITS_TO_BYTES(font->width);
	src = font->data + c * font->height * bytes + h * bytes;

	for (m = 0; m < font->width; m++) {
		n = m % 8;
		x = *(src + m / 8);
		if ((x >> (7 - n)) & 1)
			*dst = color_white;
		else
			*dst = color_black;
		dst++;
	}
}

static void
efi_earlycon_write(struct console *con, const char *str, unsigned int num)
{
	struct screen_info *si;
	unsigned int len;
	const char *s;
	void *dst;

	si = &screen_info;
	len = si->lfb_linelength;

	while (num) {
		unsigned int linemax;
		unsigned int h, count = 0;

		for (s = str; *s && *s != '\n'; s++) {
			if (count == num)
				break;
			count++;
		}

		linemax = (si->lfb_width - efi_x) / font->width;
		if (count > linemax)
			count = linemax;

		for (h = 0; h < font->height; h++) {
			unsigned int n, x;

			dst = efi_earlycon_map((efi_y + h) * len, len);
			if (!dst)
				return;

			s = str;
			n = count;
			x = efi_x;

			while (n-- > 0) {
				efi_earlycon_write_char(dst + x*4, *s, h);
				x += font->width;
				s++;
			}

			efi_earlycon_unmap(dst, len);
		}

		num -= count;
		efi_x += count * font->width;
		str += count;

		if (num > 0 && *s == '\n') {
			efi_x = 0;
			efi_y += font->height;
			str++;
			num--;
		}

		if (efi_x + font->width > si->lfb_width) {
			efi_x = 0;
			efi_y += font->height;
		}

		if (efi_y + font->height > si->lfb_height) {
			u32 i;

			efi_y -= font->height;
			efi_earlycon_scroll_up();

			for (i = 0; i < font->height; i++)
				efi_earlycon_clear_scanline(efi_y + i);
		}
	}
}

static int __init efi_earlycon_setup(struct earlycon_device *device,
				     const char *opt)
{
	struct screen_info *si;
	u16 xres, yres;
	u32 i;

	if (screen_info.orig_video_isVGA != VIDEO_TYPE_EFI)
		return -ENODEV;

	fb_base = screen_info.lfb_base;
	if (screen_info.capabilities & VIDEO_CAPABILITY_64BIT_BASE)
		fb_base |= (u64)screen_info.ext_lfb_base << 32;

	fb_wb = opt && !strcmp(opt, "ram");

	si = &screen_info;
	xres = si->lfb_width;
	yres = si->lfb_height;

	/*
	 * efi_earlycon_write_char() implicitly assumes a framebuffer with
	 * 32 bits per pixel.
	 */
	if (si->lfb_depth != 32)
		return -ENODEV;

	font = get_default_font(xres, yres, -1, -1);
	if (!font)
		return -ENODEV;

	efi_y = rounddown(yres, font->height) - font->height;
	for (i = 0; i < (yres - efi_y) / font->height; i++)
		efi_earlycon_scroll_up();

	device->con->write = efi_earlycon_write;
	earlycon_console = device->con;
	return 0;
}
EARLYCON_DECLARE(efifb, efi_earlycon_setup);
