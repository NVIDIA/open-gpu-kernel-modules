/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ARCH_X86_KERNEL_SYSFB_H
#define _ARCH_X86_KERNEL_SYSFB_H

/*
 * Generic System Framebuffers on x86
 * Copyright (c) 2012-2013 David Herrmann <dh.herrmann@gmail.com>
 */

#include <linux/kernel.h>
#include <linux/platform_data/simplefb.h>
#include <linux/screen_info.h>

enum {
	M_I17,		/* 17-Inch iMac */
	M_I20,		/* 20-Inch iMac */
	M_I20_SR,	/* 20-Inch iMac (Santa Rosa) */
	M_I24,		/* 24-Inch iMac */
	M_I24_8_1,	/* 24-Inch iMac, 8,1th gen */
	M_I24_10_1,	/* 24-Inch iMac, 10,1th gen */
	M_I27_11_1,	/* 27-Inch iMac, 11,1th gen */
	M_MINI,		/* Mac Mini */
	M_MINI_3_1,	/* Mac Mini, 3,1th gen */
	M_MINI_4_1,	/* Mac Mini, 4,1th gen */
	M_MB,		/* MacBook */
	M_MB_2,		/* MacBook, 2nd rev. */
	M_MB_3,		/* MacBook, 3rd rev. */
	M_MB_5_1,	/* MacBook, 5th rev. */
	M_MB_6_1,	/* MacBook, 6th rev. */
	M_MB_7_1,	/* MacBook, 7th rev. */
	M_MB_SR,	/* MacBook, 2nd gen, (Santa Rosa) */
	M_MBA,		/* MacBook Air */
	M_MBA_3,	/* Macbook Air, 3rd rev */
	M_MBP,		/* MacBook Pro */
	M_MBP_2,	/* MacBook Pro 2nd gen */
	M_MBP_2_2,	/* MacBook Pro 2,2nd gen */
	M_MBP_SR,	/* MacBook Pro (Santa Rosa) */
	M_MBP_4,	/* MacBook Pro, 4th gen */
	M_MBP_5_1,	/* MacBook Pro, 5,1th gen */
	M_MBP_5_2,	/* MacBook Pro, 5,2th gen */
	M_MBP_5_3,	/* MacBook Pro, 5,3rd gen */
	M_MBP_6_1,	/* MacBook Pro, 6,1th gen */
	M_MBP_6_2,	/* MacBook Pro, 6,2th gen */
	M_MBP_7_1,	/* MacBook Pro, 7,1th gen */
	M_MBP_8_2,	/* MacBook Pro, 8,2nd gen */
	M_UNKNOWN	/* placeholder */
};

struct efifb_dmi_info {
	char *optname;
	unsigned long base;
	int stride;
	int width;
	int height;
	int flags;
};

#ifdef CONFIG_EFI

extern struct efifb_dmi_info efifb_dmi_list[];
void sysfb_apply_efi_quirks(void);

#else /* CONFIG_EFI */

static inline void sysfb_apply_efi_quirks(void)
{
}

#endif /* CONFIG_EFI */

#ifdef CONFIG_X86_SYSFB

bool parse_mode(const struct screen_info *si,
		struct simplefb_platform_data *mode);
int create_simplefb(const struct screen_info *si,
		    const struct simplefb_platform_data *mode);

#else /* CONFIG_X86_SYSFB */

static inline bool parse_mode(const struct screen_info *si,
			      struct simplefb_platform_data *mode)
{
	return false;
}

static inline int create_simplefb(const struct screen_info *si,
				  const struct simplefb_platform_data *mode)
{
	return -EINVAL;
}

#endif /* CONFIG_X86_SYSFB */

#endif /* _ARCH_X86_KERNEL_SYSFB_H */
