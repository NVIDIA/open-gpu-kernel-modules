/*
 * Marvell MBUS common definitions.
 *
 * Copyright (C) 2008 Marvell Semiconductor
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#ifndef __LINUX_MBUS_H
#define __LINUX_MBUS_H

#include <linux/errno.h>

struct resource;

struct mbus_dram_target_info
{
	/*
	 * The 4-bit MBUS target ID of the DRAM controller.
	 */
	u8		mbus_dram_target_id;

	/*
	 * The base address, size, and MBUS attribute ID for each
	 * of the possible DRAM chip selects.  Peripherals are
	 * required to support at least 4 decode windows.
	 */
	int		num_cs;
	struct mbus_dram_window {
		u8	cs_index;
		u8	mbus_attr;
		u64	base;
		u64	size;
	} cs[4];
};

/* Flags for PCI/PCIe address decoding regions */
#define MVEBU_MBUS_PCI_IO  0x1
#define MVEBU_MBUS_PCI_MEM 0x2
#define MVEBU_MBUS_PCI_WA  0x3

/*
 * Magic value that explicits that we don't need a remapping-capable
 * address decoding window.
 */
#define MVEBU_MBUS_NO_REMAP (0xffffffff)

/* Maximum size of a mbus window name */
#define MVEBU_MBUS_MAX_WINNAME_SZ 32

/*
 * The Marvell mbus is to be found only on SOCs from the Orion family
 * at the moment.  Provide a dummy stub for other architectures.
 */
#ifdef CONFIG_PLAT_ORION
extern const struct mbus_dram_target_info *mv_mbus_dram_info(void);
extern const struct mbus_dram_target_info *mv_mbus_dram_info_nooverlap(void);
int mvebu_mbus_get_io_win_info(phys_addr_t phyaddr, u32 *size, u8 *target,
			       u8 *attr);
#else
static inline const struct mbus_dram_target_info *mv_mbus_dram_info(void)
{
	return NULL;
}
static inline const struct mbus_dram_target_info *mv_mbus_dram_info_nooverlap(void)
{
	return NULL;
}
static inline int mvebu_mbus_get_io_win_info(phys_addr_t phyaddr, u32 *size,
					     u8 *target, u8 *attr)
{
	/*
	 * On all ARM32 MVEBU platforms with MBus support, this stub
	 * function will not get called. The real function from the
	 * MBus driver is called instead. ARM64 MVEBU platforms like
	 * the Armada 3700 could use the mv_xor device driver which calls
	 * into this function
	 */
	return -EINVAL;
}
#endif

#ifdef CONFIG_MVEBU_MBUS
int mvebu_mbus_save_cpu_target(u32 __iomem *store_addr);
void mvebu_mbus_get_pcie_mem_aperture(struct resource *res);
void mvebu_mbus_get_pcie_io_aperture(struct resource *res);
int mvebu_mbus_get_dram_win_info(phys_addr_t phyaddr, u8 *target, u8 *attr);
int mvebu_mbus_add_window_remap_by_id(unsigned int target,
				      unsigned int attribute,
				      phys_addr_t base, size_t size,
				      phys_addr_t remap);
int mvebu_mbus_add_window_by_id(unsigned int target, unsigned int attribute,
				phys_addr_t base, size_t size);
int mvebu_mbus_del_window(phys_addr_t base, size_t size);
int mvebu_mbus_init(const char *soc, phys_addr_t mbus_phys_base,
		    size_t mbus_size, phys_addr_t sdram_phys_base,
		    size_t sdram_size);
int mvebu_mbus_dt_init(bool is_coherent);
#else
static inline int mvebu_mbus_get_dram_win_info(phys_addr_t phyaddr, u8 *target,
					       u8 *attr)
{
	return -EINVAL;
}
#endif /* CONFIG_MVEBU_MBUS */

#endif /* __LINUX_MBUS_H */
