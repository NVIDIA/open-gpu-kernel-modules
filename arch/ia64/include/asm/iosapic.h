/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_IA64_IOSAPIC_H
#define __ASM_IA64_IOSAPIC_H

#define	IOSAPIC_REG_SELECT	0x0
#define	IOSAPIC_WINDOW		0x10
#define	IOSAPIC_EOI		0x40

#define	IOSAPIC_VERSION		0x1

/*
 * Redirection table entry
 */
#define	IOSAPIC_RTE_LOW(i)	(0x10+i*2)
#define	IOSAPIC_RTE_HIGH(i)	(0x11+i*2)

#define	IOSAPIC_DEST_SHIFT		16

/*
 * Delivery mode
 */
#define	IOSAPIC_DELIVERY_SHIFT		8
#define	IOSAPIC_FIXED			0x0
#define	IOSAPIC_LOWEST_PRIORITY	0x1
#define	IOSAPIC_PMI			0x2
#define	IOSAPIC_NMI			0x4
#define	IOSAPIC_INIT			0x5
#define	IOSAPIC_EXTINT			0x7

/*
 * Interrupt polarity
 */
#define	IOSAPIC_POLARITY_SHIFT		13
#define	IOSAPIC_POL_HIGH		0
#define	IOSAPIC_POL_LOW		1

/*
 * Trigger mode
 */
#define	IOSAPIC_TRIGGER_SHIFT		15
#define	IOSAPIC_EDGE			0
#define	IOSAPIC_LEVEL			1

/*
 * Mask bit
 */

#define	IOSAPIC_MASK_SHIFT		16
#define	IOSAPIC_MASK			(1<<IOSAPIC_MASK_SHIFT)

#define IOSAPIC_VECTOR_MASK		0xffffff00

#ifndef __ASSEMBLY__

#define NR_IOSAPICS			256

#define iosapic_pcat_compat_init	ia64_native_iosapic_pcat_compat_init
#define __iosapic_read			__ia64_native_iosapic_read
#define __iosapic_write			__ia64_native_iosapic_write
#define iosapic_get_irq_chip		ia64_native_iosapic_get_irq_chip

extern void __init ia64_native_iosapic_pcat_compat_init(void);
extern struct irq_chip *ia64_native_iosapic_get_irq_chip(unsigned long trigger);

static inline unsigned int
__ia64_native_iosapic_read(char __iomem *iosapic, unsigned int reg)
{
	writel(reg, iosapic + IOSAPIC_REG_SELECT);
	return readl(iosapic + IOSAPIC_WINDOW);
}

static inline void
__ia64_native_iosapic_write(char __iomem *iosapic, unsigned int reg, u32 val)
{
	writel(reg, iosapic + IOSAPIC_REG_SELECT);
	writel(val, iosapic + IOSAPIC_WINDOW);
}

static inline void iosapic_eoi(char __iomem *iosapic, u32 vector)
{
	writel(vector, iosapic + IOSAPIC_EOI);
}

extern void __init iosapic_system_init (int pcat_compat);
extern int iosapic_init (unsigned long address, unsigned int gsi_base);
extern int iosapic_remove (unsigned int gsi_base);
extern int gsi_to_irq (unsigned int gsi);
extern int iosapic_register_intr (unsigned int gsi, unsigned long polarity,
				  unsigned long trigger);
extern void iosapic_unregister_intr (unsigned int irq);
extern void iosapic_override_isa_irq (unsigned int isa_irq, unsigned int gsi,
				      unsigned long polarity,
				      unsigned long trigger);
extern int __init iosapic_register_platform_intr (u32 int_type,
					   unsigned int gsi,
					   int pmi_vector,
					   u16 eid, u16 id,
					   unsigned long polarity,
					   unsigned long trigger);

#ifdef CONFIG_NUMA
extern void map_iosapic_to_node (unsigned int, int);
#endif

# endif /* !__ASSEMBLY__ */
#endif /* __ASM_IA64_IOSAPIC_H */
