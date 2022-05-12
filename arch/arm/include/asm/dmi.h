/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ASM_DMI_H
#define __ASM_DMI_H

#include <linux/io.h>
#include <linux/slab.h>

#define dmi_early_remap(x, l)		memremap(x, l, MEMREMAP_WB)
#define dmi_early_unmap(x, l)		memunmap(x)
#define dmi_remap(x, l)			memremap(x, l, MEMREMAP_WB)
#define dmi_unmap(x)			memunmap(x)
#define dmi_alloc(l)			kzalloc(l, GFP_KERNEL)

#endif
