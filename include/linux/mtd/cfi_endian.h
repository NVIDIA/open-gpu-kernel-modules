/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright © 2001-2010 David Woodhouse <dwmw2@infradead.org>
 */

#include <asm/byteorder.h>

#define CFI_HOST_ENDIAN 1
#define CFI_LITTLE_ENDIAN 2
#define CFI_BIG_ENDIAN 3

#if !defined(CONFIG_MTD_CFI_ADV_OPTIONS) || defined(CONFIG_MTD_CFI_NOSWAP)
#define CFI_DEFAULT_ENDIAN CFI_HOST_ENDIAN
#elif defined(CONFIG_MTD_CFI_LE_BYTE_SWAP)
#define CFI_DEFAULT_ENDIAN CFI_LITTLE_ENDIAN
#elif defined(CONFIG_MTD_CFI_BE_BYTE_SWAP)
#define CFI_DEFAULT_ENDIAN CFI_BIG_ENDIAN
#else
#error No CFI endianness defined
#endif

#define cfi_default(s) ((s)?:CFI_DEFAULT_ENDIAN)
#define cfi_be(s) (cfi_default(s) == CFI_BIG_ENDIAN)
#define cfi_le(s) (cfi_default(s) == CFI_LITTLE_ENDIAN)
#define cfi_host(s) (cfi_default(s) == CFI_HOST_ENDIAN)

#define cpu_to_cfi8(map, x) (x)
#define cfi8_to_cpu(map, x) (x)
#define cpu_to_cfi16(map, x) _cpu_to_cfi(16, (map)->swap, (x))
#define cpu_to_cfi32(map, x) _cpu_to_cfi(32, (map)->swap, (x))
#define cpu_to_cfi64(map, x) _cpu_to_cfi(64, (map)->swap, (x))
#define cfi16_to_cpu(map, x) _cfi_to_cpu(16, (map)->swap, (x))
#define cfi32_to_cpu(map, x) _cfi_to_cpu(32, (map)->swap, (x))
#define cfi64_to_cpu(map, x) _cfi_to_cpu(64, (map)->swap, (x))

#define _cpu_to_cfi(w, s, x) (cfi_host(s)?(x):_swap_to_cfi(w, s, x))
#define _cfi_to_cpu(w, s, x) (cfi_host(s)?(x):_swap_to_cpu(w, s, x))
#define _swap_to_cfi(w, s, x) (cfi_be(s)?cpu_to_be##w(x):cpu_to_le##w(x))
#define _swap_to_cpu(w, s, x) (cfi_be(s)?be##w##_to_cpu(x):le##w##_to_cpu(x))
