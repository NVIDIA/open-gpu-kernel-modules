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


// Copied from include/drm/drm_print.h
#ifndef _KCL_BACKPORT_KCL__DRM_PRINT_H__H_
#define _KCL_BACKPORT_KCL__DRM_PRINT_H__H_

#include <drm/drm_print.h>
#include <kcl/kcl_drm_print.h>

#if !defined(HAVE_DRM_PRINT_BITS_4ARGS) && \
	defined(HAVE_DRM_PRINT_BITS)
static inline
void _kcl_drm_print_bits(struct drm_printer *p, unsigned long value,
		    const char * const bits[], unsigned int nbits)
{
	unsigned int from, to;

	from = ffs(value);
	to = fls(value);
	WARN_ON_ONCE(to > nbits);

	drm_print_bits(p, value, bits, from, nbits);
}
#define drm_print_bits _kcl_drm_print_bits
#endif

#endif
