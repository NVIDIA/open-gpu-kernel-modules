// SPDX-License-Identifier: GPL-2.0-only

#ifndef KVM_X86_MMU_SPTE_H
#define KVM_X86_MMU_SPTE_H

#include "mmu_internal.h"

/*
 * A MMU present SPTE is backed by actual memory and may or may not be present
 * in hardware.  E.g. MMIO SPTEs are not considered present.  Use bit 11, as it
 * is ignored by all flavors of SPTEs and checking a low bit often generates
 * better code than for a high bit, e.g. 56+.  MMU present checks are pervasive
 * enough that the improved code generation is noticeable in KVM's footprint.
 */
#define SPTE_MMU_PRESENT_MASK		BIT_ULL(11)

/*
 * TDP SPTES (more specifically, EPT SPTEs) may not have A/D bits, and may also
 * be restricted to using write-protection (for L2 when CPU dirty logging, i.e.
 * PML, is enabled).  Use bits 52 and 53 to hold the type of A/D tracking that
 * is must be employed for a given TDP SPTE.
 *
 * Note, the "enabled" mask must be '0', as bits 62:52 are _reserved_ for PAE
 * paging, including NPT PAE.  This scheme works because legacy shadow paging
 * is guaranteed to have A/D bits and write-protection is forced only for
 * TDP with CPU dirty logging (PML).  If NPT ever gains PML-like support, it
 * must be restricted to 64-bit KVM.
 */
#define SPTE_TDP_AD_SHIFT		52
#define SPTE_TDP_AD_MASK		(3ULL << SPTE_TDP_AD_SHIFT)
#define SPTE_TDP_AD_ENABLED_MASK	(0ULL << SPTE_TDP_AD_SHIFT)
#define SPTE_TDP_AD_DISABLED_MASK	(1ULL << SPTE_TDP_AD_SHIFT)
#define SPTE_TDP_AD_WRPROT_ONLY_MASK	(2ULL << SPTE_TDP_AD_SHIFT)
static_assert(SPTE_TDP_AD_ENABLED_MASK == 0);

#ifdef CONFIG_DYNAMIC_PHYSICAL_MASK
#define PT64_BASE_ADDR_MASK (physical_mask & ~(u64)(PAGE_SIZE-1))
#else
#define PT64_BASE_ADDR_MASK (((1ULL << 52) - 1) & ~(u64)(PAGE_SIZE-1))
#endif
#define PT64_LVL_ADDR_MASK(level) \
	(PT64_BASE_ADDR_MASK & ~((1ULL << (PAGE_SHIFT + (((level) - 1) \
						* PT64_LEVEL_BITS))) - 1))
#define PT64_LVL_OFFSET_MASK(level) \
	(PT64_BASE_ADDR_MASK & ((1ULL << (PAGE_SHIFT + (((level) - 1) \
						* PT64_LEVEL_BITS))) - 1))

#define PT64_PERM_MASK (PT_PRESENT_MASK | PT_WRITABLE_MASK | shadow_user_mask \
			| shadow_x_mask | shadow_nx_mask | shadow_me_mask)

#define ACC_EXEC_MASK    1
#define ACC_WRITE_MASK   PT_WRITABLE_MASK
#define ACC_USER_MASK    PT_USER_MASK
#define ACC_ALL          (ACC_EXEC_MASK | ACC_WRITE_MASK | ACC_USER_MASK)

/* The mask for the R/X bits in EPT PTEs */
#define PT64_EPT_READABLE_MASK			0x1ull
#define PT64_EPT_EXECUTABLE_MASK		0x4ull

#define PT64_LEVEL_BITS 9

#define PT64_LEVEL_SHIFT(level) \
		(PAGE_SHIFT + (level - 1) * PT64_LEVEL_BITS)

#define PT64_INDEX(address, level)\
	(((address) >> PT64_LEVEL_SHIFT(level)) & ((1 << PT64_LEVEL_BITS) - 1))
#define SHADOW_PT_INDEX(addr, level) PT64_INDEX(addr, level)

/* Bits 9 and 10 are ignored by all non-EPT PTEs. */
#define DEFAULT_SPTE_HOST_WRITEABLE	BIT_ULL(9)
#define DEFAULT_SPTE_MMU_WRITEABLE	BIT_ULL(10)

/*
 * The mask/shift to use for saving the original R/X bits when marking the PTE
 * as not-present for access tracking purposes. We do not save the W bit as the
 * PTEs being access tracked also need to be dirty tracked, so the W bit will be
 * restored only when a write is attempted to the page.  This mask obviously
 * must not overlap the A/D type mask.
 */
#define SHADOW_ACC_TRACK_SAVED_BITS_MASK (PT64_EPT_READABLE_MASK | \
					  PT64_EPT_EXECUTABLE_MASK)
#define SHADOW_ACC_TRACK_SAVED_BITS_SHIFT 54
#define SHADOW_ACC_TRACK_SAVED_MASK	(SHADOW_ACC_TRACK_SAVED_BITS_MASK << \
					 SHADOW_ACC_TRACK_SAVED_BITS_SHIFT)
static_assert(!(SPTE_TDP_AD_MASK & SHADOW_ACC_TRACK_SAVED_MASK));

/*
 * Low ignored bits are at a premium for EPT, use high ignored bits, taking care
 * to not overlap the A/D type mask or the saved access bits of access-tracked
 * SPTEs when A/D bits are disabled.
 */
#define EPT_SPTE_HOST_WRITABLE		BIT_ULL(57)
#define EPT_SPTE_MMU_WRITABLE		BIT_ULL(58)

static_assert(!(EPT_SPTE_HOST_WRITABLE & SPTE_TDP_AD_MASK));
static_assert(!(EPT_SPTE_MMU_WRITABLE & SPTE_TDP_AD_MASK));
static_assert(!(EPT_SPTE_HOST_WRITABLE & SHADOW_ACC_TRACK_SAVED_MASK));
static_assert(!(EPT_SPTE_MMU_WRITABLE & SHADOW_ACC_TRACK_SAVED_MASK));

/* Defined only to keep the above static asserts readable. */
#undef SHADOW_ACC_TRACK_SAVED_MASK

/*
 * Due to limited space in PTEs, the MMIO generation is a 19 bit subset of
 * the memslots generation and is derived as follows:
 *
 * Bits 0-7 of the MMIO generation are propagated to spte bits 3-10
 * Bits 8-18 of the MMIO generation are propagated to spte bits 52-62
 *
 * The KVM_MEMSLOT_GEN_UPDATE_IN_PROGRESS flag is intentionally not included in
 * the MMIO generation number, as doing so would require stealing a bit from
 * the "real" generation number and thus effectively halve the maximum number
 * of MMIO generations that can be handled before encountering a wrap (which
 * requires a full MMU zap).  The flag is instead explicitly queried when
 * checking for MMIO spte cache hits.
 */

#define MMIO_SPTE_GEN_LOW_START		3
#define MMIO_SPTE_GEN_LOW_END		10

#define MMIO_SPTE_GEN_HIGH_START	52
#define MMIO_SPTE_GEN_HIGH_END		62

#define MMIO_SPTE_GEN_LOW_MASK		GENMASK_ULL(MMIO_SPTE_GEN_LOW_END, \
						    MMIO_SPTE_GEN_LOW_START)
#define MMIO_SPTE_GEN_HIGH_MASK		GENMASK_ULL(MMIO_SPTE_GEN_HIGH_END, \
						    MMIO_SPTE_GEN_HIGH_START)
static_assert(!(SPTE_MMU_PRESENT_MASK &
		(MMIO_SPTE_GEN_LOW_MASK | MMIO_SPTE_GEN_HIGH_MASK)));

#define MMIO_SPTE_GEN_LOW_BITS		(MMIO_SPTE_GEN_LOW_END - MMIO_SPTE_GEN_LOW_START + 1)
#define MMIO_SPTE_GEN_HIGH_BITS		(MMIO_SPTE_GEN_HIGH_END - MMIO_SPTE_GEN_HIGH_START + 1)

/* remember to adjust the comment above as well if you change these */
static_assert(MMIO_SPTE_GEN_LOW_BITS == 8 && MMIO_SPTE_GEN_HIGH_BITS == 11);

#define MMIO_SPTE_GEN_LOW_SHIFT		(MMIO_SPTE_GEN_LOW_START - 0)
#define MMIO_SPTE_GEN_HIGH_SHIFT	(MMIO_SPTE_GEN_HIGH_START - MMIO_SPTE_GEN_LOW_BITS)

#define MMIO_SPTE_GEN_MASK		GENMASK_ULL(MMIO_SPTE_GEN_LOW_BITS + MMIO_SPTE_GEN_HIGH_BITS - 1, 0)

extern u64 __read_mostly shadow_host_writable_mask;
extern u64 __read_mostly shadow_mmu_writable_mask;
extern u64 __read_mostly shadow_nx_mask;
extern u64 __read_mostly shadow_x_mask; /* mutual exclusive with nx_mask */
extern u64 __read_mostly shadow_user_mask;
extern u64 __read_mostly shadow_accessed_mask;
extern u64 __read_mostly shadow_dirty_mask;
extern u64 __read_mostly shadow_mmio_value;
extern u64 __read_mostly shadow_mmio_mask;
extern u64 __read_mostly shadow_mmio_access_mask;
extern u64 __read_mostly shadow_present_mask;
extern u64 __read_mostly shadow_me_mask;

/*
 * SPTEs in MMUs without A/D bits are marked with SPTE_TDP_AD_DISABLED_MASK;
 * shadow_acc_track_mask is the set of bits to be cleared in non-accessed
 * pages.
 */
extern u64 __read_mostly shadow_acc_track_mask;

/*
 * This mask must be set on all non-zero Non-Present or Reserved SPTEs in order
 * to guard against L1TF attacks.
 */
extern u64 __read_mostly shadow_nonpresent_or_rsvd_mask;

/*
 * The number of high-order 1 bits to use in the mask above.
 */
#define SHADOW_NONPRESENT_OR_RSVD_MASK_LEN 5

/*
 * If a thread running without exclusive control of the MMU lock must perform a
 * multi-part operation on an SPTE, it can set the SPTE to REMOVED_SPTE as a
 * non-present intermediate value. Other threads which encounter this value
 * should not modify the SPTE.
 *
 * Use a semi-arbitrary value that doesn't set RWX bits, i.e. is not-present on
 * bot AMD and Intel CPUs, and doesn't set PFN bits, i.e. doesn't create a L1TF
 * vulnerability.  Use only low bits to avoid 64-bit immediates.
 *
 * Only used by the TDP MMU.
 */
#define REMOVED_SPTE	0x5a0ULL

/* Removed SPTEs must not be misconstrued as shadow present PTEs. */
static_assert(!(REMOVED_SPTE & SPTE_MMU_PRESENT_MASK));

static inline bool is_removed_spte(u64 spte)
{
	return spte == REMOVED_SPTE;
}

/*
 * In some cases, we need to preserve the GFN of a non-present or reserved
 * SPTE when we usurp the upper five bits of the physical address space to
 * defend against L1TF, e.g. for MMIO SPTEs.  To preserve the GFN, we'll
 * shift bits of the GFN that overlap with shadow_nonpresent_or_rsvd_mask
 * left into the reserved bits, i.e. the GFN in the SPTE will be split into
 * high and low parts.  This mask covers the lower bits of the GFN.
 */
extern u64 __read_mostly shadow_nonpresent_or_rsvd_lower_gfn_mask;

/*
 * The number of non-reserved physical address bits irrespective of features
 * that repurpose legal bits, e.g. MKTME.
 */
extern u8 __read_mostly shadow_phys_bits;

static inline bool is_mmio_spte(u64 spte)
{
	return (spte & shadow_mmio_mask) == shadow_mmio_value &&
	       likely(shadow_mmio_value);
}

static inline bool is_shadow_present_pte(u64 pte)
{
	return !!(pte & SPTE_MMU_PRESENT_MASK);
}

static inline bool sp_ad_disabled(struct kvm_mmu_page *sp)
{
	return sp->role.ad_disabled;
}

static inline bool spte_ad_enabled(u64 spte)
{
	MMU_WARN_ON(!is_shadow_present_pte(spte));
	return (spte & SPTE_TDP_AD_MASK) != SPTE_TDP_AD_DISABLED_MASK;
}

static inline bool spte_ad_need_write_protect(u64 spte)
{
	MMU_WARN_ON(!is_shadow_present_pte(spte));
	/*
	 * This is benign for non-TDP SPTEs as SPTE_TDP_AD_ENABLED_MASK is '0',
	 * and non-TDP SPTEs will never set these bits.  Optimize for 64-bit
	 * TDP and do the A/D type check unconditionally.
	 */
	return (spte & SPTE_TDP_AD_MASK) != SPTE_TDP_AD_ENABLED_MASK;
}

static inline u64 spte_shadow_accessed_mask(u64 spte)
{
	MMU_WARN_ON(!is_shadow_present_pte(spte));
	return spte_ad_enabled(spte) ? shadow_accessed_mask : 0;
}

static inline u64 spte_shadow_dirty_mask(u64 spte)
{
	MMU_WARN_ON(!is_shadow_present_pte(spte));
	return spte_ad_enabled(spte) ? shadow_dirty_mask : 0;
}

static inline bool is_access_track_spte(u64 spte)
{
	return !spte_ad_enabled(spte) && (spte & shadow_acc_track_mask) == 0;
}

static inline bool is_large_pte(u64 pte)
{
	return pte & PT_PAGE_SIZE_MASK;
}

static inline bool is_last_spte(u64 pte, int level)
{
	return (level == PG_LEVEL_4K) || is_large_pte(pte);
}

static inline bool is_executable_pte(u64 spte)
{
	return (spte & (shadow_x_mask | shadow_nx_mask)) == shadow_x_mask;
}

static inline kvm_pfn_t spte_to_pfn(u64 pte)
{
	return (pte & PT64_BASE_ADDR_MASK) >> PAGE_SHIFT;
}

static inline bool is_accessed_spte(u64 spte)
{
	u64 accessed_mask = spte_shadow_accessed_mask(spte);

	return accessed_mask ? spte & accessed_mask
			     : !is_access_track_spte(spte);
}

static inline bool is_dirty_spte(u64 spte)
{
	u64 dirty_mask = spte_shadow_dirty_mask(spte);

	return dirty_mask ? spte & dirty_mask : spte & PT_WRITABLE_MASK;
}

static inline bool spte_can_locklessly_be_made_writable(u64 spte)
{
	return (spte & shadow_host_writable_mask) &&
	       (spte & shadow_mmu_writable_mask);
}

static inline u64 get_mmio_spte_generation(u64 spte)
{
	u64 gen;

	gen = (spte & MMIO_SPTE_GEN_LOW_MASK) >> MMIO_SPTE_GEN_LOW_SHIFT;
	gen |= (spte & MMIO_SPTE_GEN_HIGH_MASK) >> MMIO_SPTE_GEN_HIGH_SHIFT;
	return gen;
}

/* Bits which may be returned by set_spte() */
#define SET_SPTE_WRITE_PROTECTED_PT    BIT(0)
#define SET_SPTE_NEED_REMOTE_TLB_FLUSH BIT(1)
#define SET_SPTE_SPURIOUS              BIT(2)

int make_spte(struct kvm_vcpu *vcpu, unsigned int pte_access, int level,
		     gfn_t gfn, kvm_pfn_t pfn, u64 old_spte, bool speculative,
		     bool can_unsync, bool host_writable, bool ad_disabled,
		     u64 *new_spte);
u64 make_nonleaf_spte(u64 *child_pt, bool ad_disabled);
u64 make_mmio_spte(struct kvm_vcpu *vcpu, u64 gfn, unsigned int access);
u64 mark_spte_for_access_track(u64 spte);
u64 kvm_mmu_changed_pte_notifier_make_spte(u64 old_spte, kvm_pfn_t new_pfn);

void kvm_mmu_reset_all_pte_masks(void);

#endif
