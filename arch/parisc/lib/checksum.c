// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * INET		An implementation of the TCP/IP protocol suite for the LINUX
 *		operating system.  INET is implemented using the  BSD Socket
 *		interface as the means of communication with the user level.
 *
 *		MIPS specific IP/TCP/UDP checksumming routines
 *
 * Authors:	Ralf Baechle, <ralf@waldorf-gmbh.de>
 *		Lots of code moved from tcp.c and ip.c; see those files
 *		for more names.
 */
#include <linux/module.h>
#include <linux/types.h>

#include <net/checksum.h>
#include <asm/byteorder.h>
#include <asm/string.h>
#include <linux/uaccess.h>

#define addc(_t,_r)                     \
	__asm__ __volatile__ (          \
"       add             %0, %1, %0\n"   \
"       addc            %0, %%r0, %0\n" \
	: "=r"(_t)                      \
	: "r"(_r), "0"(_t));

static inline unsigned short from32to16(unsigned int x)
{
	/* 32 bits --> 16 bits + carry */
	x = (x & 0xffff) + (x >> 16);
	/* 16 bits + carry --> 16 bits including carry */
	x = (x & 0xffff) + (x >> 16);
	return (unsigned short)x;
}

static inline unsigned int do_csum(const unsigned char * buff, int len)
{
	int odd, count;
	unsigned int result = 0;

	if (len <= 0)
		goto out;
	odd = 1 & (unsigned long) buff;
	if (odd) {
		result = be16_to_cpu(*buff);
		len--;
		buff++;
	}
	count = len >> 1;		/* nr of 16-bit words.. */
	if (count) {
		if (2 & (unsigned long) buff) {
			result += *(unsigned short *) buff;
			count--;
			len -= 2;
			buff += 2;
		}
		count >>= 1;		/* nr of 32-bit words.. */
		if (count) {
			while (count >= 4) {
				unsigned int r1, r2, r3, r4;
				r1 = *(unsigned int *)(buff + 0);
				r2 = *(unsigned int *)(buff + 4);
				r3 = *(unsigned int *)(buff + 8);
				r4 = *(unsigned int *)(buff + 12);
				addc(result, r1);
				addc(result, r2);
				addc(result, r3);
				addc(result, r4);
				count -= 4;
				buff += 16;
			}
			while (count) {
				unsigned int w = *(unsigned int *) buff;
				count--;
				buff += 4;
				addc(result, w);
			}
			result = (result & 0xffff) + (result >> 16);
		}
		if (len & 2) {
			result += *(unsigned short *) buff;
			buff += 2;
		}
	}
	if (len & 1)
		result += le16_to_cpu(*buff);
	result = from32to16(result);
	if (odd)
		result = swab16(result);
out:
	return result;
}

/*
 * computes a partial checksum, e.g. for TCP/UDP fragments
 */
/*
 * why bother folding?
 */
__wsum csum_partial(const void *buff, int len, __wsum sum)
{
	unsigned int result = do_csum(buff, len);
	addc(result, sum);
	return (__force __wsum)from32to16(result);
}

EXPORT_SYMBOL(csum_partial);
