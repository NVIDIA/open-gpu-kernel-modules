/**********************************************************************
 * Author: Cavium, Inc.
 *
 * Contact: support@cavium.com
 *          Please include "LiquidIO" in the subject.
 *
 * Copyright (c) 2003-2016 Cavium, Inc.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, Version 2, as
 * published by the Free Software Foundation.
 *
 * This file is distributed in the hope that it will be useful, but
 * AS-IS and WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, TITLE, or
 * NONINFRINGEMENT.  See the GNU General Public License for more
 * details.
 **********************************************************************/
#include <linux/netdevice.h>
#include "liquidio_common.h"
#include "octeon_droq.h"
#include "octeon_iq.h"
#include "response_manager.h"
#include "octeon_device.h"
#include "octeon_mem_ops.h"

#define MEMOPS_IDX   BAR1_INDEX_DYNAMIC_MAP

#ifdef __BIG_ENDIAN_BITFIELD
static inline void
octeon_toggle_bar1_swapmode(struct octeon_device *oct, u32 idx)
{
	u32 mask;

	mask = oct->fn_list.bar1_idx_read(oct, idx);
	mask = (mask & 0x2) ? (mask & ~2) : (mask | 2);
	oct->fn_list.bar1_idx_write(oct, idx, mask);
}
#else
#define octeon_toggle_bar1_swapmode(oct, idx)
#endif

static void
octeon_pci_fastwrite(struct octeon_device *oct, u8 __iomem *mapped_addr,
		     u8 *hostbuf, u32 len)
{
	while ((len) && ((unsigned long)mapped_addr) & 7) {
		writeb(*(hostbuf++), mapped_addr++);
		len--;
	}

	octeon_toggle_bar1_swapmode(oct, MEMOPS_IDX);

	while (len >= 8) {
		writeq(*((u64 *)hostbuf), mapped_addr);
		mapped_addr += 8;
		hostbuf += 8;
		len -= 8;
	}

	octeon_toggle_bar1_swapmode(oct, MEMOPS_IDX);

	while (len--)
		writeb(*(hostbuf++), mapped_addr++);
}

static void
octeon_pci_fastread(struct octeon_device *oct, u8 __iomem *mapped_addr,
		    u8 *hostbuf, u32 len)
{
	while ((len) && ((unsigned long)mapped_addr) & 7) {
		*(hostbuf++) = readb(mapped_addr++);
		len--;
	}

	octeon_toggle_bar1_swapmode(oct, MEMOPS_IDX);

	while (len >= 8) {
		*((u64 *)hostbuf) = readq(mapped_addr);
		mapped_addr += 8;
		hostbuf += 8;
		len -= 8;
	}

	octeon_toggle_bar1_swapmode(oct, MEMOPS_IDX);

	while (len--)
		*(hostbuf++) = readb(mapped_addr++);
}

/* Core mem read/write with temporary bar1 settings. */
/* op = 1 to read, op = 0 to write. */
static void
__octeon_pci_rw_core_mem(struct octeon_device *oct, u64 addr,
			 u8 *hostbuf, u32 len, u32 op)
{
	u32 copy_len = 0, index_reg_val = 0;
	unsigned long flags;
	u8 __iomem *mapped_addr;
	u64 static_mapping_base;

	static_mapping_base = oct->console_nb_info.dram_region_base;

	if (static_mapping_base &&
	    static_mapping_base == (addr & ~(OCTEON_BAR1_ENTRY_SIZE - 1ULL))) {
		int bar1_index = oct->console_nb_info.bar1_index;

		mapped_addr = oct->mmio[1].hw_addr
			+ (bar1_index << ilog2(OCTEON_BAR1_ENTRY_SIZE))
			+ (addr & (OCTEON_BAR1_ENTRY_SIZE - 1ULL));

		if (op)
			octeon_pci_fastread(oct, mapped_addr, hostbuf, len);
		else
			octeon_pci_fastwrite(oct, mapped_addr, hostbuf, len);

		return;
	}

	spin_lock_irqsave(&oct->mem_access_lock, flags);

	/* Save the original index reg value. */
	index_reg_val = oct->fn_list.bar1_idx_read(oct, MEMOPS_IDX);
	do {
		oct->fn_list.bar1_idx_setup(oct, addr, MEMOPS_IDX, 1);
		mapped_addr = oct->mmio[1].hw_addr
		    + (MEMOPS_IDX << 22) + (addr & 0x3fffff);

		/* If operation crosses a 4MB boundary, split the transfer
		 * at the 4MB
		 * boundary.
		 */
		if (((addr + len - 1) & ~(0x3fffff)) != (addr & ~(0x3fffff))) {
			copy_len = (u32)(((addr & ~(0x3fffff)) +
				   (MEMOPS_IDX << 22)) - addr);
		} else {
			copy_len = len;
		}

		if (op) {	/* read from core */
			octeon_pci_fastread(oct, mapped_addr, hostbuf,
					    copy_len);
		} else {
			octeon_pci_fastwrite(oct, mapped_addr, hostbuf,
					     copy_len);
		}

		len -= copy_len;
		addr += copy_len;
		hostbuf += copy_len;

	} while (len);

	oct->fn_list.bar1_idx_write(oct, MEMOPS_IDX, index_reg_val);

	spin_unlock_irqrestore(&oct->mem_access_lock, flags);
}

void
octeon_pci_read_core_mem(struct octeon_device *oct,
			 u64 coreaddr,
			 u8 *buf,
			 u32 len)
{
	__octeon_pci_rw_core_mem(oct, coreaddr, buf, len, 1);
}

void
octeon_pci_write_core_mem(struct octeon_device *oct,
			  u64 coreaddr,
			  const u8 *buf,
			  u32 len)
{
	__octeon_pci_rw_core_mem(oct, coreaddr, (u8 *)buf, len, 0);
}

u64 octeon_read_device_mem64(struct octeon_device *oct, u64 coreaddr)
{
	__be64 ret;

	__octeon_pci_rw_core_mem(oct, coreaddr, (u8 *)&ret, 8, 1);

	return be64_to_cpu(ret);
}

u32 octeon_read_device_mem32(struct octeon_device *oct, u64 coreaddr)
{
	__be32 ret;

	__octeon_pci_rw_core_mem(oct, coreaddr, (u8 *)&ret, 4, 1);

	return be32_to_cpu(ret);
}

void octeon_write_device_mem32(struct octeon_device *oct, u64 coreaddr,
			       u32 val)
{
	__be32 t = cpu_to_be32(val);

	__octeon_pci_rw_core_mem(oct, coreaddr, (u8 *)&t, 4, 0);
}
