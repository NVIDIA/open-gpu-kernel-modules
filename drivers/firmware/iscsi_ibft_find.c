// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright 2007-2010 Red Hat, Inc.
 *  by Peter Jones <pjones@redhat.com>
 *  Copyright 2007 IBM, Inc.
 *  by Konrad Rzeszutek <konradr@linux.vnet.ibm.com>
 *  Copyright 2008
 *  by Konrad Rzeszutek <ketuzsezr@darnok.org>
 *
 * This code finds the iSCSI Boot Format Table.
 */

#include <linux/memblock.h>
#include <linux/blkdev.h>
#include <linux/ctype.h>
#include <linux/device.h>
#include <linux/efi.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/limits.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/stat.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/acpi.h>
#include <linux/iscsi_ibft.h>

#include <asm/mmzone.h>

/*
 * Physical location of iSCSI Boot Format Table.
 */
struct acpi_table_ibft *ibft_addr;
EXPORT_SYMBOL_GPL(ibft_addr);

static const struct {
	char *sign;
} ibft_signs[] = {
	{ "iBFT" },
	{ "BIFT" },	/* Broadcom iSCSI Offload */
};

#define IBFT_SIGN_LEN 4
#define IBFT_START 0x80000 /* 512kB */
#define IBFT_END 0x100000 /* 1MB */
#define VGA_MEM 0xA0000 /* VGA buffer */
#define VGA_SIZE 0x20000 /* 128kB */

static int __init find_ibft_in_mem(void)
{
	unsigned long pos;
	unsigned int len = 0;
	void *virt;
	int i;

	for (pos = IBFT_START; pos < IBFT_END; pos += 16) {
		/* The table can't be inside the VGA BIOS reserved space,
		 * so skip that area */
		if (pos == VGA_MEM)
			pos += VGA_SIZE;
		virt = isa_bus_to_virt(pos);

		for (i = 0; i < ARRAY_SIZE(ibft_signs); i++) {
			if (memcmp(virt, ibft_signs[i].sign, IBFT_SIGN_LEN) ==
			    0) {
				unsigned long *addr =
				    (unsigned long *)isa_bus_to_virt(pos + 4);
				len = *addr;
				/* if the length of the table extends past 1M,
				 * the table cannot be valid. */
				if (pos + len <= (IBFT_END-1)) {
					ibft_addr = (struct acpi_table_ibft *)virt;
					pr_info("iBFT found at 0x%lx.\n", pos);
					goto done;
				}
			}
		}
	}
done:
	return len;
}
/*
 * Routine used to find the iSCSI Boot Format Table. The logical
 * kernel address is set in the ibft_addr global variable.
 */
unsigned long __init find_ibft_region(unsigned long *sizep)
{
	ibft_addr = NULL;

	/* iBFT 1.03 section 1.4.3.1 mandates that UEFI machines will
	 * only use ACPI for this */

	if (!efi_enabled(EFI_BOOT))
		find_ibft_in_mem();

	if (ibft_addr) {
		*sizep = PAGE_ALIGN(ibft_addr->header.length);
		return (u64)virt_to_phys(ibft_addr);
	}

	*sizep = 0;
	return 0;
}
