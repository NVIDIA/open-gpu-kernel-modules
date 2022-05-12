/* SPDX-License-Identifier: GPL-2.0 */
/*
 *  lzodefs.h -- architecture, OS and compiler specific defines
 *
 *  Copyright (C) 1996-2012 Markus F.X.J. Oberhumer <markus@oberhumer.com>
 *
 *  The full LZO package can be found at:
 *  http://www.oberhumer.com/opensource/lzo/
 *
 *  Changed for Linux kernel use by:
 *  Nitin Gupta <nitingupta910@gmail.com>
 *  Richard Purdie <rpurdie@openedhand.com>
 */


/* Version
 * 0: original lzo version
 * 1: lzo with support for RLE
 */
#define LZO_VERSION 1

#define COPY4(dst, src)	\
		put_unaligned(get_unaligned((const u32 *)(src)), (u32 *)(dst))
#if defined(CONFIG_X86_64) || defined(CONFIG_ARM64)
#define COPY8(dst, src)	\
		put_unaligned(get_unaligned((const u64 *)(src)), (u64 *)(dst))
#else
#define COPY8(dst, src)	\
		COPY4(dst, src); COPY4((dst) + 4, (src) + 4)
#endif

#if defined(__BIG_ENDIAN) && defined(__LITTLE_ENDIAN)
#error "conflicting endian definitions"
#elif defined(CONFIG_X86_64) || defined(CONFIG_ARM64)
#define LZO_USE_CTZ64	1
#define LZO_USE_CTZ32	1
#define LZO_FAST_64BIT_MEMORY_ACCESS
#elif defined(CONFIG_X86) || defined(CONFIG_PPC)
#define LZO_USE_CTZ32	1
#elif defined(CONFIG_ARM) && (__LINUX_ARM_ARCH__ >= 5)
#define LZO_USE_CTZ32	1
#endif

#define M1_MAX_OFFSET	0x0400
#define M2_MAX_OFFSET	0x0800
#define M3_MAX_OFFSET	0x4000
#define M4_MAX_OFFSET_V0	0xbfff
#define M4_MAX_OFFSET_V1	0xbffe

#define M1_MIN_LEN	2
#define M1_MAX_LEN	2
#define M2_MIN_LEN	3
#define M2_MAX_LEN	8
#define M3_MIN_LEN	3
#define M3_MAX_LEN	33
#define M4_MIN_LEN	3
#define M4_MAX_LEN	9

#define M1_MARKER	0
#define M2_MARKER	64
#define M3_MARKER	32
#define M4_MARKER	16

#define MIN_ZERO_RUN_LENGTH	4
#define MAX_ZERO_RUN_LENGTH	(2047 + MIN_ZERO_RUN_LENGTH)

#define lzo_dict_t      unsigned short
#define D_BITS		13
#define D_SIZE		(1u << D_BITS)
#define D_MASK		(D_SIZE - 1)
#define D_HIGH		((D_MASK >> 1) + 1)
