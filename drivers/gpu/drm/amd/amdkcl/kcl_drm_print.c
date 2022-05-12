/*
 * Copyright (C) 2016 Red Hat
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors:
 * Rob Clark <robdclark@gmail.com>
 */
#include <kcl/kcl_drm_print.h>
#include <kcl/kcl_bitops.h>
#include <stdarg.h>

#if !defined(HAVE_DRM_DRM_PRINT_H)
void drm_printf(struct drm_printer *p, const char *f, ...)
{
	struct va_format vaf;
	va_list args;

	va_start(args, f);
	vaf.fmt = f;
	vaf.va = &args;
	p->printfn(p, &vaf);
	va_end(args);
}
EXPORT_SYMBOL(drm_printf);

void __drm_printfn_seq_file(struct drm_printer *p, struct va_format *vaf)
{
	seq_printf(p->arg, "%pV", vaf);
}
EXPORT_SYMBOL(__drm_printfn_seq_file);
#endif

#if !defined(HAVE_DRM_PRINTER_PREFIX)
void __drm_printfn_debug(struct drm_printer *p, struct va_format *vaf)
{
#ifndef HAVE_DRM_DRM_PRINT_H
	printk(KERN_DEBUG "[" DRM_NAME ":]" "%s %pV", p->prefix, vaf);
#else
	printk(KERN_DEBUG "[" DRM_NAME ":]" "%s %pV", "no prefix", vaf);
#endif
}
EXPORT_SYMBOL(__drm_printfn_debug);
#endif

#if !defined(HAVE_DRM_DEV_DBG)
void drm_dev_dbg(const struct device *dev, int category,
		 const char *format, ...)
{
	struct va_format vaf;
	va_list args;

	if (!drm_debug_enabled(category))
		return;

	va_start(args, format);
	vaf.fmt = format;
	vaf.va = &args;

	if (dev)
		dev_printk(KERN_DEBUG, dev, "[" DRM_NAME ":%ps] %pV",
			   __builtin_return_address(0), &vaf);
	else
		printk(KERN_DEBUG "[" DRM_NAME ":%ps] %pV",
		       __builtin_return_address(0), &vaf);

	va_end(args);
}
EXPORT_SYMBOL(drm_dev_dbg);
#endif

#if !defined(HAVE_DRM_ERR_MACRO)
void kcl_drm_err(const char *format, ...)
{
        struct va_format vaf;
        va_list args;

        va_start(args, format);
        vaf.fmt = format;
        vaf.va = &args;

        printk(KERN_ERR "[" DRM_NAME ":%ps] *ERROR* %pV",
               __builtin_return_address(0), &vaf);

        va_end(args);
}
EXPORT_SYMBOL(kcl_drm_err);

#endif

#ifndef HAVE_DRM_PRINT_BITS
/* Copied from v5.3-rc1-684-g141f6357f45c drivers/gpu/drm/drm_print.c */
void drm_print_bits(struct drm_printer *p, unsigned long value,
		    const char * const bits[], unsigned int nbits)
{
	bool first = true;
	unsigned int i;

	if (WARN_ON_ONCE(nbits > BITS_PER_TYPE(value)))
		nbits = BITS_PER_TYPE(value);

	for_each_set_bit(i, &value, nbits) {
		if (WARN_ON_ONCE(!bits[i]))
			continue;
		drm_printf(p, "%s%s", first ? "" : ",",
			   bits[i]);
		first = false;
	}
	if (first)
		drm_printf(p, "(none)");
}
EXPORT_SYMBOL(drm_print_bits);
#endif
