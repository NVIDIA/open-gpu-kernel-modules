/*
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
 * VA LINUX SYSTEMS AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef KCL_BITMAP_H
#define KCL_BITMAP_H

#ifndef HAVE_BITMAP_FUNCS
/* Copied from include/linux/bitmap.h*/

/*
 * v4.17-3-gc42b65e363ce
 * bitmap: Add bitmap_alloc(), bitmap_zalloc() and bitmap_free()
 */

/*
 * Allocation and deallocation of bitmap.
 * Provided in lib/bitmap.c to avoid circular dependency.
 */
unsigned long *kcl_bitmap_alloc(unsigned int nbits, gfp_t flags);
unsigned long *kcl_bitmap_zalloc(unsigned int nbits, gfp_t flags);
void kcl_bitmap_free(const unsigned long *bitmap);
#endif /* HAVE_BITMAP_FUNCS */

#endif /* KCL_BITMAP_H */
