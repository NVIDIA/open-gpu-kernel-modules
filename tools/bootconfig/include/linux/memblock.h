/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _XBC_LINUX_MEMBLOCK_H
#define _XBC_LINUX_MEMBLOCK_H

#include <stdlib.h>

#define __pa(addr)	(addr)
#define SMP_CACHE_BYTES	0
#define memblock_alloc(size, align)	malloc(size)
#define memblock_free(paddr, size)	free(paddr)

#endif
