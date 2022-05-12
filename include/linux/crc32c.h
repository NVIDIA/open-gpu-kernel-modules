/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_CRC32C_H
#define _LINUX_CRC32C_H

#include <linux/types.h>

extern u32 crc32c(u32 crc, const void *address, unsigned int length);
extern const char *crc32c_impl(void);

/* This macro exists for backwards-compatibility. */
#define crc32c_le crc32c

#endif	/* _LINUX_CRC32C_H */
