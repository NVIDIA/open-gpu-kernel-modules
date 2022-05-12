/*
 * Copyright (c) 2016 Laurent Pinchart <laurent.pinchart@ideasonboard.com>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the name of the copyright holders not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  The copyright holders make no representations
 * about the suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

#ifndef __KCL_BACKPORT_KCL_DRM_FOURCC_H__
#define __KCL_BACKPORT_KCL_DRM_FOURCC_H__

#include <linux/ctype.h>
#include <kcl/kcl_drm_fourcc.h>

#if !defined(HAVE_DRM_GET_FORMAT_NAME_I_P)
/**
 * struct drm_format_name_buf - name of a DRM format
 * @str: string buffer containing the format name
 */
struct drm_format_name_buf {
	char str[32];
};

static char printable_char(int c)
{
	return isascii(c) && isprint(c) ? c : '?';
}

static inline const char *_kcl_drm_get_format_name(uint32_t format, struct drm_format_name_buf *buf)
{
	snprintf(buf->str, sizeof(buf->str),
		 "%c%c%c%c %s-endian (0x%08x)",
		 printable_char(format & 0xff),
		 printable_char((format >> 8) & 0xff),
		 printable_char((format >> 16) & 0xff),
		 printable_char((format >> 24) & 0x7f),
		 format & DRM_FORMAT_BIG_ENDIAN ? "big" : "little",
		 format);

	return buf->str;
}
#define drm_get_format_name _kcl_drm_get_format_name
#endif

#endif
