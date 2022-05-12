/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2008 Michal Simek <monstr@monstr.eu>
 * Copyright (C) 2006 Atmark Techno, Inc.
 */

#ifndef _ASM_MICROBLAZE_CHECKSUM_H
#define _ASM_MICROBLAZE_CHECKSUM_H

/*
 * computes the checksum of the TCP/UDP pseudo-header
 * returns a 16-bit checksum, already complemented
 */
#define csum_tcpudp_nofold	csum_tcpudp_nofold
static inline __wsum
csum_tcpudp_nofold(__be32 saddr, __be32 daddr, __u32 len,
		   __u8 proto, __wsum sum)
{
	__asm__("add %0, %0, %1\n\t"
		"addc %0, %0, %2\n\t"
		"addc %0, %0, %3\n\t"
		"addc %0, %0, r0\n\t"
		: "+&d" (sum)
		: "d" (saddr), "d" (daddr),
#ifdef __MICROBLAZEEL__
	"d" ((len + proto) << 8)
#else
	"d" (len + proto)
#endif
);
	return sum;
}

#include <asm-generic/checksum.h>

#endif /* _ASM_MICROBLAZE_CHECKSUM_H */
