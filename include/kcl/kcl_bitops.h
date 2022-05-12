/* SPDX-License-Identifier: GPL-2.0 */
#ifndef AMDKCL_BITOPS_BACKPORT_H
#define AMDKCL_BITOPS_BACKPORT_H

#include <linux/bitops.h>
/* Copied froma include/linux/bitops.h */
#ifndef BITS_PER_TYPE
#define BITS_PER_TYPE(type)     (sizeof(type) * BITS_PER_BYTE)
#endif

#endif
