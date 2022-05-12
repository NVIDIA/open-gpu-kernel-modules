/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_POWERPC_MMU_44X_H_
#define _ASM_POWERPC_MMU_44X_H_
/*
 * PPC440 support
 */

#include <asm/asm-const.h>

#define PPC44x_MMUCR_TID	0x000000ff
#define PPC44x_MMUCR_STS	0x00010000

#define	PPC44x_TLB_PAGEID	0
#define	PPC44x_TLB_XLAT		1
#define	PPC44x_TLB_ATTRIB	2

/* Page identification fields */
#define PPC44x_TLB_EPN_MASK	0xfffffc00      /* Effective Page Number */
#define	PPC44x_TLB_VALID	0x00000200      /* Valid flag */
#define PPC44x_TLB_TS		0x00000100	/* Translation address space */
#define PPC44x_TLB_1K		0x00000000	/* Page sizes */
#define PPC44x_TLB_4K		0x00000010
#define PPC44x_TLB_16K		0x00000020
#define PPC44x_TLB_64K		0x00000030
#define PPC44x_TLB_256K		0x00000040
#define PPC44x_TLB_1M		0x00000050
#define PPC44x_TLB_16M		0x00000070
#define	PPC44x_TLB_256M		0x00000090

/* Translation fields */
#define PPC44x_TLB_RPN_MASK	0xfffffc00      /* Real Page Number */
#define	PPC44x_TLB_ERPN_MASK	0x0000000f

/* Storage attribute and access control fields */
#define PPC44x_TLB_ATTR_MASK	0x0000ff80
#define PPC44x_TLB_U0		0x00008000      /* User 0 */
#define PPC44x_TLB_U1		0x00004000      /* User 1 */
#define PPC44x_TLB_U2		0x00002000      /* User 2 */
#define PPC44x_TLB_U3		0x00001000      /* User 3 */
#define PPC44x_TLB_W		0x00000800      /* Caching is write-through */
#define PPC44x_TLB_I		0x00000400      /* Caching is inhibited */
#define PPC44x_TLB_M		0x00000200      /* Memory is coherent */
#define PPC44x_TLB_G		0x00000100      /* Memory is guarded */
#define PPC44x_TLB_E		0x00000080      /* Memory is little endian */

#define PPC44x_TLB_PERM_MASK	0x0000003f
#define PPC44x_TLB_UX		0x00000020      /* User execution */
#define PPC44x_TLB_UW		0x00000010      /* User write */
#define PPC44x_TLB_UR		0x00000008      /* User read */
#define PPC44x_TLB_SX		0x00000004      /* Super execution */
#define PPC44x_TLB_SW		0x00000002      /* Super write */
#define PPC44x_TLB_SR		0x00000001      /* Super read */

/* Number of TLB entries */
#define PPC44x_TLB_SIZE		64

/* 47x bits */
#define PPC47x_MMUCR_TID	0x0000ffff
#define PPC47x_MMUCR_STS	0x00010000

/* Page identification fields */
#define PPC47x_TLB0_EPN_MASK	0xfffff000      /* Effective Page Number */
#define PPC47x_TLB0_VALID	0x00000800      /* Valid flag */
#define PPC47x_TLB0_TS		0x00000400	/* Translation address space */
#define PPC47x_TLB0_4K		0x00000000
#define PPC47x_TLB0_16K		0x00000010
#define PPC47x_TLB0_64K		0x00000030
#define PPC47x_TLB0_1M		0x00000070
#define PPC47x_TLB0_16M		0x000000f0
#define PPC47x_TLB0_256M	0x000001f0
#define PPC47x_TLB0_1G		0x000003f0
#define PPC47x_TLB0_BOLTED_R	0x00000008	/* tlbre only */

/* Translation fields */
#define PPC47x_TLB1_RPN_MASK	0xfffff000      /* Real Page Number */
#define PPC47x_TLB1_ERPN_MASK	0x000003ff

/* Storage attribute and access control fields */
#define PPC47x_TLB2_ATTR_MASK	0x0003ff80
#define PPC47x_TLB2_IL1I	0x00020000      /* Memory is guarded */
#define PPC47x_TLB2_IL1D	0x00010000      /* Memory is guarded */
#define PPC47x_TLB2_U0		0x00008000      /* User 0 */
#define PPC47x_TLB2_U1		0x00004000      /* User 1 */
#define PPC47x_TLB2_U2		0x00002000      /* User 2 */
#define PPC47x_TLB2_U3		0x00001000      /* User 3 */
#define PPC47x_TLB2_W		0x00000800      /* Caching is write-through */
#define PPC47x_TLB2_I		0x00000400      /* Caching is inhibited */
#define PPC47x_TLB2_M		0x00000200      /* Memory is coherent */
#define PPC47x_TLB2_G		0x00000100      /* Memory is guarded */
#define PPC47x_TLB2_E		0x00000080      /* Memory is little endian */
#define PPC47x_TLB2_PERM_MASK	0x0000003f
#define PPC47x_TLB2_UX		0x00000020      /* User execution */
#define PPC47x_TLB2_UW		0x00000010      /* User write */
#define PPC47x_TLB2_UR		0x00000008      /* User read */
#define PPC47x_TLB2_SX		0x00000004      /* Super execution */
#define PPC47x_TLB2_SW		0x00000002      /* Super write */
#define PPC47x_TLB2_SR		0x00000001      /* Super read */
#define PPC47x_TLB2_U_RWX	(PPC47x_TLB2_UX|PPC47x_TLB2_UW|PPC47x_TLB2_UR)
#define PPC47x_TLB2_S_RWX	(PPC47x_TLB2_SX|PPC47x_TLB2_SW|PPC47x_TLB2_SR)
#define PPC47x_TLB2_S_RW	(PPC47x_TLB2_SW | PPC47x_TLB2_SR)
#define PPC47x_TLB2_IMG		(PPC47x_TLB2_I | PPC47x_TLB2_M | PPC47x_TLB2_G)

#ifndef __ASSEMBLY__

extern unsigned int tlb_44x_hwater;
extern unsigned int tlb_44x_index;

typedef struct {
	unsigned int	id;
	unsigned int	active;
	void __user	*vdso;
} mm_context_t;

/* patch sites */
extern s32 patch__tlb_44x_hwater_D, patch__tlb_44x_hwater_I;

#endif /* !__ASSEMBLY__ */

#ifndef CONFIG_PPC_EARLY_DEBUG_44x
#define PPC44x_EARLY_TLBS	1
#else
#define PPC44x_EARLY_TLBS	2
#define PPC44x_EARLY_DEBUG_VIRTADDR	(ASM_CONST(0xf0000000) \
	| (ASM_CONST(CONFIG_PPC_EARLY_DEBUG_44x_PHYSLOW) & 0xffff))
#endif

/* Size of the TLBs used for pinning in lowmem */
#define PPC_PIN_SIZE	(1 << 28)	/* 256M */

#if defined(CONFIG_PPC_4K_PAGES)
#define PPC44x_TLBE_SIZE	PPC44x_TLB_4K
#define PPC47x_TLBE_SIZE	PPC47x_TLB0_4K
#define mmu_virtual_psize	MMU_PAGE_4K
#elif defined(CONFIG_PPC_16K_PAGES)
#define PPC44x_TLBE_SIZE	PPC44x_TLB_16K
#define PPC47x_TLBE_SIZE	PPC47x_TLB0_16K
#define mmu_virtual_psize	MMU_PAGE_16K
#elif defined(CONFIG_PPC_64K_PAGES)
#define PPC44x_TLBE_SIZE	PPC44x_TLB_64K
#define PPC47x_TLBE_SIZE	PPC47x_TLB0_64K
#define mmu_virtual_psize	MMU_PAGE_64K
#elif defined(CONFIG_PPC_256K_PAGES)
#define PPC44x_TLBE_SIZE	PPC44x_TLB_256K
#define mmu_virtual_psize	MMU_PAGE_256K
#else
#error "Unsupported PAGE_SIZE"
#endif

#define mmu_linear_psize	MMU_PAGE_256M

#define PPC44x_PGD_OFF_SHIFT	(32 - PGDIR_SHIFT + PGD_T_LOG2)
#define PPC44x_PGD_OFF_MASK_BIT	(PGDIR_SHIFT - PGD_T_LOG2)
#define PPC44x_PTE_ADD_SHIFT	(32 - PGDIR_SHIFT + PTE_SHIFT + PTE_T_LOG2)
#define PPC44x_PTE_ADD_MASK_BIT	(32 - PTE_T_LOG2 - PTE_SHIFT)

#endif /* _ASM_POWERPC_MMU_44X_H_ */
