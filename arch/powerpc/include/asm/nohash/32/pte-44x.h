/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_POWERPC_NOHASH_32_PTE_44x_H
#define _ASM_POWERPC_NOHASH_32_PTE_44x_H
#ifdef __KERNEL__

/*
 * Definitions for PPC440
 *
 * Because of the 3 word TLB entries to support 36-bit addressing,
 * the attribute are difficult to map in such a fashion that they
 * are easily loaded during exception processing.  I decided to
 * organize the entry so the ERPN is the only portion in the
 * upper word of the PTE and the attribute bits below are packed
 * in as sensibly as they can be in the area below a 4KB page size
 * oriented RPN.  This at least makes it easy to load the RPN and
 * ERPN fields in the TLB. -Matt
 *
 * This isn't entirely true anymore, at least some bits are now
 * easier to move into the TLB from the PTE. -BenH.
 *
 * Note that these bits preclude future use of a page size
 * less than 4KB.
 *
 *
 * PPC 440 core has following TLB attribute fields;
 *
 *   TLB1:
 *   0  1  2  3  4  ... 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
 *   RPN.................................  -  -  -  -  -  - ERPN.......
 *
 *   TLB2:
 *   0  1  2  3  4  ... 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
 *   -  -  -  -  -    - U0 U1 U2 U3 W  I  M  G  E   - UX UW UR SX SW SR
 *
 * Newer 440 cores (440x6 as used on AMCC 460EX/460GT) have additional
 * TLB2 storage attribute fields. Those are:
 *
 *   TLB2:
 *   0...10    11   12   13   14   15   16...31
 *   no change WL1  IL1I IL1D IL2I IL2D no change
 *
 * There are some constrains and options, to decide mapping software bits
 * into TLB entry.
 *
 *   - PRESENT *must* be in the bottom three bits because swap cache
 *     entries use the top 29 bits for TLB2.
 *
 *   - CACHE COHERENT bit (M) has no effect on original PPC440 cores,
 *     because it doesn't support SMP. However, some later 460 variants
 *     have -some- form of SMP support and so I keep the bit there for
 *     future use
 *
 * With the PPC 44x Linux implementation, the 0-11th LSBs of the PTE are used
 * for memory protection related functions (see PTE structure in
 * include/asm-ppc/mmu.h).  The _PAGE_XXX definitions in this file map to the
 * above bits.  Note that the bit values are CPU specific, not architecture
 * specific.
 *
 * The kernel PTE entry holds an arch-dependent swp_entry structure under
 * certain situations. In other words, in such situations some portion of
 * the PTE bits are used as a swp_entry. In the PPC implementation, the
 * 3-24th LSB are shared with swp_entry, however the 0-2nd three LSB still
 * hold protection values. That means the three protection bits are
 * reserved for both PTE and SWAP entry at the most significant three
 * LSBs.
 *
 * There are three protection bits available for SWAP entry:
 *	_PAGE_PRESENT
 *	_PAGE_HASHPTE (if HW has)
 *
 * So those three bits have to be inside of 0-2nd LSB of PTE.
 *
 */

#define _PAGE_PRESENT	0x00000001		/* S: PTE valid */
#define _PAGE_RW	0x00000002		/* S: Write permission */
#define _PAGE_EXEC	0x00000004		/* H: Execute permission */
#define _PAGE_ACCESSED	0x00000008		/* S: Page referenced */
#define _PAGE_DIRTY	0x00000010		/* S: Page dirty */
#define _PAGE_SPECIAL	0x00000020		/* S: Special page */
#define _PAGE_USER	0x00000040		/* S: User page */
#define _PAGE_ENDIAN	0x00000080		/* H: E bit */
#define _PAGE_GUARDED	0x00000100		/* H: G bit */
#define _PAGE_COHERENT	0x00000200		/* H: M bit */
#define _PAGE_NO_CACHE	0x00000400		/* H: I bit */
#define _PAGE_WRITETHRU	0x00000800		/* H: W bit */

/* No page size encoding in the linux PTE */
#define _PAGE_PSIZE		0

#define _PAGE_KERNEL_RO		0
#define _PAGE_KERNEL_ROX	_PAGE_EXEC
#define _PAGE_KERNEL_RW		(_PAGE_DIRTY | _PAGE_RW)
#define _PAGE_KERNEL_RWX	(_PAGE_DIRTY | _PAGE_RW | _PAGE_EXEC)

/* TODO: Add large page lowmem mapping support */
#define _PMD_PRESENT	0
#define _PMD_PRESENT_MASK (PAGE_MASK)
#define _PMD_BAD	(~PAGE_MASK)
#define _PMD_USER	0

/* ERPN in a PTE never gets cleared, ignore it */
#define _PTE_NONE_MASK	0xffffffff00000000ULL

/*
 * We define 2 sets of base prot bits, one for basic pages (ie,
 * cacheable kernel and user pages) and one for non cacheable
 * pages. We always set _PAGE_COHERENT when SMP is enabled or
 * the processor might need it for DMA coherency.
 */
#define _PAGE_BASE_NC	(_PAGE_PRESENT | _PAGE_ACCESSED)
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

#endif /* __KERNEL__ */
#endif /*  _ASM_POWERPC_NOHASH_32_PTE_44x_H */
