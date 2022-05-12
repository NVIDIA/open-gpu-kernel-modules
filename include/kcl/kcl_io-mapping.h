/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright © 2008 Keith Packard <keithp@keithp.com>
 */

#ifndef KCL_KCL_IO_MAPPING_H
#define KCL_KCL_IO_MAPPING_H

#include <linux/io-mapping.h>

#ifndef HAVE_IO_MAPPING_UNMAP_LOCAL
static inline void io_mapping_unmap_local(void __iomem *vaddr)
{
        io_mapping_unmap(vaddr);
}
#endif

#ifndef HAVE_IO_MAPPING_MAP_LOCAL_WC
static inline void __iomem *
io_mapping_map_local_wc(struct io_mapping *mapping, unsigned long offset)
{
#ifdef HAVE_IO_MAPPING_MAP_WC_HAS_SIZE_ARG
        return io_mapping_map_wc(mapping, offset, PAGE_SIZE);
#else
	return io_mapping_map_wc(mapping, offset);
#endif
}
#endif

#endif /* KCL_KCL_IO_MAPPING_H */
