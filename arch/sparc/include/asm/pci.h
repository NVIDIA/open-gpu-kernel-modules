/* SPDX-License-Identifier: GPL-2.0 */
#ifndef ___ASM_SPARC_PCI_H
#define ___ASM_SPARC_PCI_H


/* Can be used to override the logic in pci_scan_bus for skipping
 * already-configured bus numbers - to be used for buggy BIOSes
 * or architectures with incomplete PCI setup by the loader.
 */
#define pcibios_assign_all_busses()	0

#define PCIBIOS_MIN_IO		0UL
#define PCIBIOS_MIN_MEM		0UL

#define PCI_IRQ_NONE		0xffffffff


#ifdef CONFIG_SPARC64

/* PCI IOMMU mapping bypass support. */

/* PCI 64-bit addressing works for all slots on all controller
 * types on sparc64.  However, it requires that the device
 * can drive enough of the 64 bits.
 */
#define PCI64_REQUIRED_MASK	(~(u64)0)
#define PCI64_ADDR_BASE		0xfffc000000000000UL

/* Return the index of the PCI controller for device PDEV. */
int pci_domain_nr(struct pci_bus *bus);
static inline int pci_proc_domain(struct pci_bus *bus)
{
	return 1;
}

/* Platform support for /proc/bus/pci/X/Y mmap()s. */
#define HAVE_PCI_MMAP
#define arch_can_pci_mmap_io()	1
#define HAVE_ARCH_PCI_GET_UNMAPPED_AREA
#define get_pci_unmapped_area get_fb_unmapped_area
#endif /* CONFIG_SPARC64 */

#if defined(CONFIG_SPARC64) || defined(CONFIG_LEON_PCI)
static inline int pci_get_legacy_ide_irq(struct pci_dev *dev, int channel)
{
	return PCI_IRQ_NONE;
}
#else
#include <asm-generic/pci.h>
#endif

#endif /* ___ASM_SPARC_PCI_H */
