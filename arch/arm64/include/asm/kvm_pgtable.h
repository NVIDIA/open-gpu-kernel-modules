// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2020 Google LLC
 * Author: Will Deacon <will@kernel.org>
 */

#ifndef __ARM64_KVM_PGTABLE_H__
#define __ARM64_KVM_PGTABLE_H__

#include <linux/bits.h>
#include <linux/kvm_host.h>
#include <linux/types.h>

#define KVM_PGTABLE_MAX_LEVELS		4U

static inline u64 kvm_get_parange(u64 mmfr0)
{
	u64 parange = cpuid_feature_extract_unsigned_field(mmfr0,
				ID_AA64MMFR0_PARANGE_SHIFT);
	if (parange > ID_AA64MMFR0_PARANGE_MAX)
		parange = ID_AA64MMFR0_PARANGE_MAX;

	return parange;
}

typedef u64 kvm_pte_t;

/**
 * struct kvm_pgtable_mm_ops - Memory management callbacks.
 * @zalloc_page:	Allocate a single zeroed memory page. The @arg parameter
 *			can be used by the walker to pass a memcache. The
 *			initial refcount of the page is 1.
 * @zalloc_pages_exact:	Allocate an exact number of zeroed memory pages. The
 *			@size parameter is in bytes, and is rounded-up to the
 *			next page boundary. The resulting allocation is
 *			physically contiguous.
 * @free_pages_exact:	Free an exact number of memory pages previously
 *			allocated by zalloc_pages_exact.
 * @get_page:		Increment the refcount on a page.
 * @put_page:		Decrement the refcount on a page. When the refcount
 *			reaches 0 the page is automatically freed.
 * @page_count:		Return the refcount of a page.
 * @phys_to_virt:	Convert a physical address into a virtual address mapped
 *			in the current context.
 * @virt_to_phys:	Convert a virtual address mapped in the current context
 *			into a physical address.
 */
struct kvm_pgtable_mm_ops {
	void*		(*zalloc_page)(void *arg);
	void*		(*zalloc_pages_exact)(size_t size);
	void		(*free_pages_exact)(void *addr, size_t size);
	void		(*get_page)(void *addr);
	void		(*put_page)(void *addr);
	int		(*page_count)(void *addr);
	void*		(*phys_to_virt)(phys_addr_t phys);
	phys_addr_t	(*virt_to_phys)(void *addr);
};

/**
 * enum kvm_pgtable_stage2_flags - Stage-2 page-table flags.
 * @KVM_PGTABLE_S2_NOFWB:	Don't enforce Normal-WB even if the CPUs have
 *				ARM64_HAS_STAGE2_FWB.
 * @KVM_PGTABLE_S2_IDMAP:	Only use identity mappings.
 */
enum kvm_pgtable_stage2_flags {
	KVM_PGTABLE_S2_NOFWB			= BIT(0),
	KVM_PGTABLE_S2_IDMAP			= BIT(1),
};

/**
 * struct kvm_pgtable - KVM page-table.
 * @ia_bits:		Maximum input address size, in bits.
 * @start_level:	Level at which the page-table walk starts.
 * @pgd:		Pointer to the first top-level entry of the page-table.
 * @mm_ops:		Memory management callbacks.
 * @mmu:		Stage-2 KVM MMU struct. Unused for stage-1 page-tables.
 */
struct kvm_pgtable {
	u32					ia_bits;
	u32					start_level;
	kvm_pte_t				*pgd;
	struct kvm_pgtable_mm_ops		*mm_ops;

	/* Stage-2 only */
	struct kvm_s2_mmu			*mmu;
	enum kvm_pgtable_stage2_flags		flags;
};

/**
 * enum kvm_pgtable_prot - Page-table permissions and attributes.
 * @KVM_PGTABLE_PROT_X:		Execute permission.
 * @KVM_PGTABLE_PROT_W:		Write permission.
 * @KVM_PGTABLE_PROT_R:		Read permission.
 * @KVM_PGTABLE_PROT_DEVICE:	Device attributes.
 */
enum kvm_pgtable_prot {
	KVM_PGTABLE_PROT_X			= BIT(0),
	KVM_PGTABLE_PROT_W			= BIT(1),
	KVM_PGTABLE_PROT_R			= BIT(2),

	KVM_PGTABLE_PROT_DEVICE			= BIT(3),
};

#define PAGE_HYP		(KVM_PGTABLE_PROT_R | KVM_PGTABLE_PROT_W)
#define PAGE_HYP_EXEC		(KVM_PGTABLE_PROT_R | KVM_PGTABLE_PROT_X)
#define PAGE_HYP_RO		(KVM_PGTABLE_PROT_R)
#define PAGE_HYP_DEVICE		(PAGE_HYP | KVM_PGTABLE_PROT_DEVICE)

/**
 * struct kvm_mem_range - Range of Intermediate Physical Addresses
 * @start:	Start of the range.
 * @end:	End of the range.
 */
struct kvm_mem_range {
	u64 start;
	u64 end;
};

/**
 * enum kvm_pgtable_walk_flags - Flags to control a depth-first page-table walk.
 * @KVM_PGTABLE_WALK_LEAF:		Visit leaf entries, including invalid
 *					entries.
 * @KVM_PGTABLE_WALK_TABLE_PRE:		Visit table entries before their
 *					children.
 * @KVM_PGTABLE_WALK_TABLE_POST:	Visit table entries after their
 *					children.
 */
enum kvm_pgtable_walk_flags {
	KVM_PGTABLE_WALK_LEAF			= BIT(0),
	KVM_PGTABLE_WALK_TABLE_PRE		= BIT(1),
	KVM_PGTABLE_WALK_TABLE_POST		= BIT(2),
};

typedef int (*kvm_pgtable_visitor_fn_t)(u64 addr, u64 end, u32 level,
					kvm_pte_t *ptep,
					enum kvm_pgtable_walk_flags flag,
					void * const arg);

/**
 * struct kvm_pgtable_walker - Hook into a page-table walk.
 * @cb:		Callback function to invoke during the walk.
 * @arg:	Argument passed to the callback function.
 * @flags:	Bitwise-OR of flags to identify the entry types on which to
 *		invoke the callback function.
 */
struct kvm_pgtable_walker {
	const kvm_pgtable_visitor_fn_t		cb;
	void * const				arg;
	const enum kvm_pgtable_walk_flags	flags;
};

/**
 * kvm_pgtable_hyp_init() - Initialise a hypervisor stage-1 page-table.
 * @pgt:	Uninitialised page-table structure to initialise.
 * @va_bits:	Maximum virtual address bits.
 * @mm_ops:	Memory management callbacks.
 *
 * Return: 0 on success, negative error code on failure.
 */
int kvm_pgtable_hyp_init(struct kvm_pgtable *pgt, u32 va_bits,
			 struct kvm_pgtable_mm_ops *mm_ops);

/**
 * kvm_pgtable_hyp_destroy() - Destroy an unused hypervisor stage-1 page-table.
 * @pgt:	Page-table structure initialised by kvm_pgtable_hyp_init().
 *
 * The page-table is assumed to be unreachable by any hardware walkers prior
 * to freeing and therefore no TLB invalidation is performed.
 */
void kvm_pgtable_hyp_destroy(struct kvm_pgtable *pgt);

/**
 * kvm_pgtable_hyp_map() - Install a mapping in a hypervisor stage-1 page-table.
 * @pgt:	Page-table structure initialised by kvm_pgtable_hyp_init().
 * @addr:	Virtual address at which to place the mapping.
 * @size:	Size of the mapping.
 * @phys:	Physical address of the memory to map.
 * @prot:	Permissions and attributes for the mapping.
 *
 * The offset of @addr within a page is ignored, @size is rounded-up to
 * the next page boundary and @phys is rounded-down to the previous page
 * boundary.
 *
 * If device attributes are not explicitly requested in @prot, then the
 * mapping will be normal, cacheable. Attempts to install a new mapping
 * for a virtual address that is already mapped will be rejected with an
 * error and a WARN().
 *
 * Return: 0 on success, negative error code on failure.
 */
int kvm_pgtable_hyp_map(struct kvm_pgtable *pgt, u64 addr, u64 size, u64 phys,
			enum kvm_pgtable_prot prot);

/**
 * kvm_get_vtcr() - Helper to construct VTCR_EL2
 * @mmfr0:	Sanitized value of SYS_ID_AA64MMFR0_EL1 register.
 * @mmfr1:	Sanitized value of SYS_ID_AA64MMFR1_EL1 register.
 * @phys_shfit:	Value to set in VTCR_EL2.T0SZ.
 *
 * The VTCR value is common across all the physical CPUs on the system.
 * We use system wide sanitised values to fill in different fields,
 * except for Hardware Management of Access Flags. HA Flag is set
 * unconditionally on all CPUs, as it is safe to run with or without
 * the feature and the bit is RES0 on CPUs that don't support it.
 *
 * Return: VTCR_EL2 value
 */
u64 kvm_get_vtcr(u64 mmfr0, u64 mmfr1, u32 phys_shift);

/**
 * kvm_pgtable_stage2_init_flags() - Initialise a guest stage-2 page-table.
 * @pgt:	Uninitialised page-table structure to initialise.
 * @arch:	Arch-specific KVM structure representing the guest virtual
 *		machine.
 * @mm_ops:	Memory management callbacks.
 * @flags:	Stage-2 configuration flags.
 *
 * Return: 0 on success, negative error code on failure.
 */
int kvm_pgtable_stage2_init_flags(struct kvm_pgtable *pgt, struct kvm_arch *arch,
				  struct kvm_pgtable_mm_ops *mm_ops,
				  enum kvm_pgtable_stage2_flags flags);

#define kvm_pgtable_stage2_init(pgt, arch, mm_ops) \
	kvm_pgtable_stage2_init_flags(pgt, arch, mm_ops, 0)

/**
 * kvm_pgtable_stage2_destroy() - Destroy an unused guest stage-2 page-table.
 * @pgt:	Page-table structure initialised by kvm_pgtable_stage2_init*().
 *
 * The page-table is assumed to be unreachable by any hardware walkers prior
 * to freeing and therefore no TLB invalidation is performed.
 */
void kvm_pgtable_stage2_destroy(struct kvm_pgtable *pgt);

/**
 * kvm_pgtable_stage2_map() - Install a mapping in a guest stage-2 page-table.
 * @pgt:	Page-table structure initialised by kvm_pgtable_stage2_init*().
 * @addr:	Intermediate physical address at which to place the mapping.
 * @size:	Size of the mapping.
 * @phys:	Physical address of the memory to map.
 * @prot:	Permissions and attributes for the mapping.
 * @mc:		Cache of pre-allocated and zeroed memory from which to allocate
 *		page-table pages.
 *
 * The offset of @addr within a page is ignored, @size is rounded-up to
 * the next page boundary and @phys is rounded-down to the previous page
 * boundary.
 *
 * If device attributes are not explicitly requested in @prot, then the
 * mapping will be normal, cacheable.
 *
 * Note that the update of a valid leaf PTE in this function will be aborted,
 * if it's trying to recreate the exact same mapping or only change the access
 * permissions. Instead, the vCPU will exit one more time from guest if still
 * needed and then go through the path of relaxing permissions.
 *
 * Note that this function will both coalesce existing table entries and split
 * existing block mappings, relying on page-faults to fault back areas outside
 * of the new mapping lazily.
 *
 * Return: 0 on success, negative error code on failure.
 */
int kvm_pgtable_stage2_map(struct kvm_pgtable *pgt, u64 addr, u64 size,
			   u64 phys, enum kvm_pgtable_prot prot,
			   void *mc);

/**
 * kvm_pgtable_stage2_set_owner() - Unmap and annotate pages in the IPA space to
 *				    track ownership.
 * @pgt:	Page-table structure initialised by kvm_pgtable_stage2_init*().
 * @addr:	Base intermediate physical address to annotate.
 * @size:	Size of the annotated range.
 * @mc:		Cache of pre-allocated and zeroed memory from which to allocate
 *		page-table pages.
 * @owner_id:	Unique identifier for the owner of the page.
 *
 * By default, all page-tables are owned by identifier 0. This function can be
 * used to mark portions of the IPA space as owned by other entities. When a
 * stage 2 is used with identity-mappings, these annotations allow to use the
 * page-table data structure as a simple rmap.
 *
 * Return: 0 on success, negative error code on failure.
 */
int kvm_pgtable_stage2_set_owner(struct kvm_pgtable *pgt, u64 addr, u64 size,
				 void *mc, u8 owner_id);

/**
 * kvm_pgtable_stage2_unmap() - Remove a mapping from a guest stage-2 page-table.
 * @pgt:	Page-table structure initialised by kvm_pgtable_stage2_init*().
 * @addr:	Intermediate physical address from which to remove the mapping.
 * @size:	Size of the mapping.
 *
 * The offset of @addr within a page is ignored and @size is rounded-up to
 * the next page boundary.
 *
 * TLB invalidation is performed for each page-table entry cleared during the
 * unmapping operation and the reference count for the page-table page
 * containing the cleared entry is decremented, with unreferenced pages being
 * freed. Unmapping a cacheable page will ensure that it is clean to the PoC if
 * FWB is not supported by the CPU.
 *
 * Return: 0 on success, negative error code on failure.
 */
int kvm_pgtable_stage2_unmap(struct kvm_pgtable *pgt, u64 addr, u64 size);

/**
 * kvm_pgtable_stage2_wrprotect() - Write-protect guest stage-2 address range
 *                                  without TLB invalidation.
 * @pgt:	Page-table structure initialised by kvm_pgtable_stage2_init*().
 * @addr:	Intermediate physical address from which to write-protect,
 * @size:	Size of the range.
 *
 * The offset of @addr within a page is ignored and @size is rounded-up to
 * the next page boundary.
 *
 * Note that it is the caller's responsibility to invalidate the TLB after
 * calling this function to ensure that the updated permissions are visible
 * to the CPUs.
 *
 * Return: 0 on success, negative error code on failure.
 */
int kvm_pgtable_stage2_wrprotect(struct kvm_pgtable *pgt, u64 addr, u64 size);

/**
 * kvm_pgtable_stage2_mkyoung() - Set the access flag in a page-table entry.
 * @pgt:	Page-table structure initialised by kvm_pgtable_stage2_init*().
 * @addr:	Intermediate physical address to identify the page-table entry.
 *
 * The offset of @addr within a page is ignored.
 *
 * If there is a valid, leaf page-table entry used to translate @addr, then
 * set the access flag in that entry.
 *
 * Return: The old page-table entry prior to setting the flag, 0 on failure.
 */
kvm_pte_t kvm_pgtable_stage2_mkyoung(struct kvm_pgtable *pgt, u64 addr);

/**
 * kvm_pgtable_stage2_mkold() - Clear the access flag in a page-table entry.
 * @pgt:	Page-table structure initialised by kvm_pgtable_stage2_init*().
 * @addr:	Intermediate physical address to identify the page-table entry.
 *
 * The offset of @addr within a page is ignored.
 *
 * If there is a valid, leaf page-table entry used to translate @addr, then
 * clear the access flag in that entry.
 *
 * Note that it is the caller's responsibility to invalidate the TLB after
 * calling this function to ensure that the updated permissions are visible
 * to the CPUs.
 *
 * Return: The old page-table entry prior to clearing the flag, 0 on failure.
 */
kvm_pte_t kvm_pgtable_stage2_mkold(struct kvm_pgtable *pgt, u64 addr);

/**
 * kvm_pgtable_stage2_relax_perms() - Relax the permissions enforced by a
 *				      page-table entry.
 * @pgt:	Page-table structure initialised by kvm_pgtable_stage2_init*().
 * @addr:	Intermediate physical address to identify the page-table entry.
 * @prot:	Additional permissions to grant for the mapping.
 *
 * The offset of @addr within a page is ignored.
 *
 * If there is a valid, leaf page-table entry used to translate @addr, then
 * relax the permissions in that entry according to the read, write and
 * execute permissions specified by @prot. No permissions are removed, and
 * TLB invalidation is performed after updating the entry.
 *
 * Return: 0 on success, negative error code on failure.
 */
int kvm_pgtable_stage2_relax_perms(struct kvm_pgtable *pgt, u64 addr,
				   enum kvm_pgtable_prot prot);

/**
 * kvm_pgtable_stage2_is_young() - Test whether a page-table entry has the
 *				   access flag set.
 * @pgt:	Page-table structure initialised by kvm_pgtable_stage2_init*().
 * @addr:	Intermediate physical address to identify the page-table entry.
 *
 * The offset of @addr within a page is ignored.
 *
 * Return: True if the page-table entry has the access flag set, false otherwise.
 */
bool kvm_pgtable_stage2_is_young(struct kvm_pgtable *pgt, u64 addr);

/**
 * kvm_pgtable_stage2_flush_range() - Clean and invalidate data cache to Point
 * 				      of Coherency for guest stage-2 address
 *				      range.
 * @pgt:	Page-table structure initialised by kvm_pgtable_stage2_init*().
 * @addr:	Intermediate physical address from which to flush.
 * @size:	Size of the range.
 *
 * The offset of @addr within a page is ignored and @size is rounded-up to
 * the next page boundary.
 *
 * Return: 0 on success, negative error code on failure.
 */
int kvm_pgtable_stage2_flush(struct kvm_pgtable *pgt, u64 addr, u64 size);

/**
 * kvm_pgtable_walk() - Walk a page-table.
 * @pgt:	Page-table structure initialised by kvm_pgtable_*_init().
 * @addr:	Input address for the start of the walk.
 * @size:	Size of the range to walk.
 * @walker:	Walker callback description.
 *
 * The offset of @addr within a page is ignored and @size is rounded-up to
 * the next page boundary.
 *
 * The walker will walk the page-table entries corresponding to the input
 * address range specified, visiting entries according to the walker flags.
 * Invalid entries are treated as leaf entries. Leaf entries are reloaded
 * after invoking the walker callback, allowing the walker to descend into
 * a newly installed table.
 *
 * Returning a negative error code from the walker callback function will
 * terminate the walk immediately with the same error code.
 *
 * Return: 0 on success, negative error code on failure.
 */
int kvm_pgtable_walk(struct kvm_pgtable *pgt, u64 addr, u64 size,
		     struct kvm_pgtable_walker *walker);

/**
 * kvm_pgtable_stage2_find_range() - Find a range of Intermediate Physical
 *				     Addresses with compatible permission
 *				     attributes.
 * @pgt:	Page-table structure initialised by kvm_pgtable_stage2_init*().
 * @addr:	Address that must be covered by the range.
 * @prot:	Protection attributes that the range must be compatible with.
 * @range:	Range structure used to limit the search space at call time and
 *		that will hold the result.
 *
 * The offset of @addr within a page is ignored. An IPA is compatible with @prot
 * iff its corresponding stage-2 page-table entry has default ownership and, if
 * valid, is mapped with protection attributes identical to @prot.
 *
 * Return: 0 on success, negative error code on failure.
 */
int kvm_pgtable_stage2_find_range(struct kvm_pgtable *pgt, u64 addr,
				  enum kvm_pgtable_prot prot,
				  struct kvm_mem_range *range);
#endif	/* __ARM64_KVM_PGTABLE_H__ */
