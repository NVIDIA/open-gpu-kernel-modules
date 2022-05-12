// SPDX-License-Identifier: GPL-2.0

#ifndef __KVM_X86_MMU_TDP_MMU_H
#define __KVM_X86_MMU_TDP_MMU_H

#include <linux/kvm_host.h>

hpa_t kvm_tdp_mmu_get_vcpu_root_hpa(struct kvm_vcpu *vcpu);

__must_check static inline bool kvm_tdp_mmu_get_root(struct kvm *kvm,
						     struct kvm_mmu_page *root)
{
	if (root->role.invalid)
		return false;

	return refcount_inc_not_zero(&root->tdp_mmu_root_count);
}

void kvm_tdp_mmu_put_root(struct kvm *kvm, struct kvm_mmu_page *root,
			  bool shared);

bool __kvm_tdp_mmu_zap_gfn_range(struct kvm *kvm, int as_id, gfn_t start,
				 gfn_t end, bool can_yield, bool flush,
				 bool shared);
static inline bool kvm_tdp_mmu_zap_gfn_range(struct kvm *kvm, int as_id,
					     gfn_t start, gfn_t end, bool flush,
					     bool shared)
{
	return __kvm_tdp_mmu_zap_gfn_range(kvm, as_id, start, end, true, flush,
					   shared);
}
static inline bool kvm_tdp_mmu_zap_sp(struct kvm *kvm, struct kvm_mmu_page *sp)
{
	gfn_t end = sp->gfn + KVM_PAGES_PER_HPAGE(sp->role.level);

	/*
	 * Don't allow yielding, as the caller may have a flush pending.  Note,
	 * if mmu_lock is held for write, zapping will never yield in this case,
	 * but explicitly disallow it for safety.  The TDP MMU does not yield
	 * until it has made forward progress (steps sideways), and when zapping
	 * a single shadow page that it's guaranteed to see (thus the mmu_lock
	 * requirement), its "step sideways" will always step beyond the bounds
	 * of the shadow page's gfn range and stop iterating before yielding.
	 */
	lockdep_assert_held_write(&kvm->mmu_lock);
	return __kvm_tdp_mmu_zap_gfn_range(kvm, kvm_mmu_page_as_id(sp),
					   sp->gfn, end, false, false, false);
}

void kvm_tdp_mmu_zap_all(struct kvm *kvm);
void kvm_tdp_mmu_invalidate_all_roots(struct kvm *kvm);
void kvm_tdp_mmu_zap_invalidated_roots(struct kvm *kvm);

int kvm_tdp_mmu_map(struct kvm_vcpu *vcpu, gpa_t gpa, u32 error_code,
		    int map_writable, int max_level, kvm_pfn_t pfn,
		    bool prefault);

bool kvm_tdp_mmu_unmap_gfn_range(struct kvm *kvm, struct kvm_gfn_range *range,
				 bool flush);
bool kvm_tdp_mmu_age_gfn_range(struct kvm *kvm, struct kvm_gfn_range *range);
bool kvm_tdp_mmu_test_age_gfn(struct kvm *kvm, struct kvm_gfn_range *range);
bool kvm_tdp_mmu_set_spte_gfn(struct kvm *kvm, struct kvm_gfn_range *range);

bool kvm_tdp_mmu_wrprot_slot(struct kvm *kvm, struct kvm_memory_slot *slot,
			     int min_level);
bool kvm_tdp_mmu_clear_dirty_slot(struct kvm *kvm,
				  struct kvm_memory_slot *slot);
void kvm_tdp_mmu_clear_dirty_pt_masked(struct kvm *kvm,
				       struct kvm_memory_slot *slot,
				       gfn_t gfn, unsigned long mask,
				       bool wrprot);
bool kvm_tdp_mmu_zap_collapsible_sptes(struct kvm *kvm,
				       const struct kvm_memory_slot *slot,
				       bool flush);

bool kvm_tdp_mmu_write_protect_gfn(struct kvm *kvm,
				   struct kvm_memory_slot *slot, gfn_t gfn);

int kvm_tdp_mmu_get_walk(struct kvm_vcpu *vcpu, u64 addr, u64 *sptes,
			 int *root_level);

#ifdef CONFIG_X86_64
void kvm_mmu_init_tdp_mmu(struct kvm *kvm);
void kvm_mmu_uninit_tdp_mmu(struct kvm *kvm);
static inline bool is_tdp_mmu_enabled(struct kvm *kvm) { return kvm->arch.tdp_mmu_enabled; }
static inline bool is_tdp_mmu_page(struct kvm_mmu_page *sp) { return sp->tdp_mmu_page; }
#else
static inline void kvm_mmu_init_tdp_mmu(struct kvm *kvm) {}
static inline void kvm_mmu_uninit_tdp_mmu(struct kvm *kvm) {}
static inline bool is_tdp_mmu_enabled(struct kvm *kvm) { return false; }
static inline bool is_tdp_mmu_page(struct kvm_mmu_page *sp) { return false; }
#endif

static inline bool is_tdp_mmu_root(struct kvm *kvm, hpa_t hpa)
{
	struct kvm_mmu_page *sp;

	if (!is_tdp_mmu_enabled(kvm))
		return false;
	if (WARN_ON(!VALID_PAGE(hpa)))
		return false;

	sp = to_shadow_page(hpa);
	if (WARN_ON(!sp))
		return false;

	return is_tdp_mmu_page(sp) && sp->root_count;
}

#endif /* __KVM_X86_MMU_TDP_MMU_H */
