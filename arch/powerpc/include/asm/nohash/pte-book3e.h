/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_POWERPC_NOHASH_PTE_BOOK3E_H
#define _ASM_POWERPC_NOHASH_PTE_BOOK3E_H
#ifdef __KERNEL__

/* PTE bit definitions for processors compliant to the Book3E
 * architecture 2.06 or later. The position of the PTE bits
 * matches the HW definition of the optional Embedded Page Table
 * category.
 */

/* Architected bits */
#define _PAGE_PRESENT	0x000001 /* software: pte contains a translation */
#define _PAGE_SW1	0x000002
#define _PAGE_BIT_SWAP_TYPE	2
#define _PAGE_BAP_SR	0x000004
#define _PAGE_BAP_UR	0x000008
#define _PAGE_BAP_SW	0x000010
#define _PAGE_BAP_UW	0x000020
#define _PAGE_BAP_SX	0x000040
#define _PAGE_BAP_UX	0x000080
#define _PAGE_PSIZE_MSK	0x000f00
#define _PAGE_PSIZE_4K	0x000200
#define _PAGE_PSIZE_8K	0x000300
#define _PAGE_PSIZE_16K	0x000400
#define _PAGE_PSIZE_32K	0x000500
#define _PAGE_PSIZE_64K	0x000600
#define _PAGE_PSIZE_128K	0x000700
#define _PAGE_PSIZE_256K	0x000800
#define _PAGE_PSIZE_512K	0x000900
#define _PAGE_PSIZE_1M	0x000a00
#define _PAGE_PSIZE_2M	0x000b00
#define _PAGE_PSIZE_4M	0x000c00
#define _PAGE_PSIZE_8M	0x000d00
#define _PAGE_PSIZE_16M	0x000e00
#define _PAGE_PSIZE_32M	0x000f00
#define _PAGE_DIRTY	0x001000 /* C: page changed */
#define _PAGE_SW0	0x002000
#define _PAGE_U3	0x004000
#define _PAGE_U2	0x008000
#define _PAGE_U1	0x010000
#define _PAGE_U0	0x020000
#define _PAGE_ACCESSED	0x040000
#define _PAGE_ENDIAN	0x080000
#define _PAGE_GUARDED	0x100000
#define _PAGE_COHERENT	0x200000 /* M: enforce memory coherence */
#define _PAGE_NO_CACHE	0x400000 /* I: cache inhibit */
#define _PAGE_WRITETHRU	0x800000 /* W: cache write-through */

/* "Higher level" linux bit combinations */
#define _PAGE_EXEC		_PAGE_BAP_UX /* .. and was cache cleaned */
#define _PAGE_RW		(_PAGE_BAP_SW | _PAGE_BAP_UW) /* User write permission */
#define _PAGE_KERNEL_RW		(_PAGE_BAP_SW | _PAGE_BAP_SR | _PAGE_DIRTY)
#define _PAGE_KERNEL_RO		(_PAGE_BAP_SR)
#define _PAGE_KERNEL_RWX	(_PAGE_BAP_SW | _PAGE_BAP_SR | _PAGE_DIRTY | _PAGE_BAP_SX)
#define _PAGE_KERNEL_ROX	(_PAGE_BAP_SR | _PAGE_BAP_SX)
#define _PAGE_USER		(_PAGE_BAP_UR | _PAGE_BAP_SR) /* Can be read */
#define _PAGE_PRIVILEGED	(_PAGE_BAP_SR)

#define _PAGE_SPECIAL	_PAGE_SW0

/* Base page size */
#define _PAGE_PSIZE	_PAGE_PSIZE_4K
#define	PTE_RPN_SHIFT	(24)

#define PTE_WIMGE_SHIFT (19)
#define PTE_BAP_SHIFT	(2)

/* On 32-bit, we never clear the top part of the PTE */
#ifdef CONFIG_PPC32
#define _PTE_NONE_MASK	0xffffffff00000000ULL
#define _PMD_PRESENT	0
#define _PMD_PRESENT_MASK (PAGE_MASK)
#define _PMD_BAD	(~PAGE_MASK)
#define _PMD_USER	0
#else
#define _PTE_NONE_MASK	0
#endif

/*
 * We define 2 sets of base prot bits, one for basic pages (ie,
 * cacheable kernel and user pages) and one for non cacheable
 * pages. We always set _PAGE_COHERENT when SMP is enabled or
 * the processor might need it for DMA coherency.
 */
#define _PAGE_BASE_NC	(_PAGE_PRESENT | _PAGE_ACCESSED | _PAGE_PSIZE)
#if defined(CONFIG_SMP)
#define _PAGE_BASE	(_PAGE_BASE_NC | _PAGE_COHERENT)
#else
#define _PAGE_BASE	(_PAGE_BASE_NC)
#endif

/* Permission masks used to generate the __P and __S table */
#define PAGE_NONE	__pgprot(_PAGE_BASE)
#define PAGE_SHARED	__pgprot(_PAGE_BASE | _PAGE_USER | _PAGE_RW)
#define PAGE_SHARED_X	__pgprot(_PAGE_BASE | _PAGE_USER | _PAGE_RW | _PAGE_EXEC)
#define PAGE_COPY	__pgprot(_PAGE_BASE | _PAGE_USER)
#define PAGE_COPY_X	__pgprot(_PAGE_BASE | _PAGE_USER | _PAGE_EXEC)
#define PAGE_READONLY	__pgprot(_PAGE_BASE | _PAGE_USER)
#define PAGE_READONLY_X	__pgprot(_PAGE_BASE | _PAGE_USER | _PAGE_EXEC)

#ifndef __ASSEMBLY__
static inline pte_t pte_mkprivileged(pte_t pte)
{
	return __pte((pte_val(pte) & ~_PAGE_USER) | _PAGE_PRIVILEGED);
}

#define pte_mkprivileged pte_mkprivileged

static inline pte_t pte_mkuser(pte_t pte)
{
	return __pte((pte_val(pte) & ~_PAGE_PRIVILEGED) | _PAGE_USER);
}

#define pte_mkuser pte_mkuser
#endif /* __ASSEMBLY__ */

#endif /* __KERNEL__ */
#endif /*  _ASM_POWERPC_NOHASH_PTE_BOOK3E_H */
