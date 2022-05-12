/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef __KVM_HOST_H
#define __KVM_HOST_H


#include <linux/types.h>
#include <linux/hardirq.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/sched/stat.h>
#include <linux/bug.h>
#include <linux/minmax.h>
#include <linux/mm.h>
#include <linux/mmu_notifier.h>
#include <linux/preempt.h>
#include <linux/msi.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/rcupdate.h>
#include <linux/ratelimit.h>
#include <linux/err.h>
#include <linux/irqflags.h>
#include <linux/context_tracking.h>
#include <linux/irqbypass.h>
#include <linux/rcuwait.h>
#include <linux/refcount.h>
#include <linux/nospec.h>
#include <asm/signal.h>

#include <linux/kvm.h>
#include <linux/kvm_para.h>

#include <linux/kvm_types.h>

#include <asm/kvm_host.h>
#include <linux/kvm_dirty_ring.h>

#ifndef KVM_MAX_VCPU_ID
#define KVM_MAX_VCPU_ID KVM_MAX_VCPUS
#endif

/*
 * The bit 16 ~ bit 31 of kvm_memory_region::flags are internally used
 * in kvm, other bits are visible for userspace which are defined in
 * include/linux/kvm_h.
 */
#define KVM_MEMSLOT_INVALID	(1UL << 16)

/*
 * Bit 63 of the memslot generation number is an "update in-progress flag",
 * e.g. is temporarily set for the duration of install_new_memslots().
 * This flag effectively creates a unique generation number that is used to
 * mark cached memslot data, e.g. MMIO accesses, as potentially being stale,
 * i.e. may (or may not) have come from the previous memslots generation.
 *
 * This is necessary because the actual memslots update is not atomic with
 * respect to the generation number update.  Updating the generation number
 * first would allow a vCPU to cache a spte from the old memslots using the
 * new generation number, and updating the generation number after switching
 * to the new memslots would allow cache hits using the old generation number
 * to reference the defunct memslots.
 *
 * This mechanism is used to prevent getting hits in KVM's caches while a
 * memslot update is in-progress, and to prevent cache hits *after* updating
 * the actual generation number against accesses that were inserted into the
 * cache *before* the memslots were updated.
 */
#define KVM_MEMSLOT_GEN_UPDATE_IN_PROGRESS	BIT_ULL(63)

/* Two fragments for cross MMIO pages. */
#define KVM_MAX_MMIO_FRAGMENTS	2

#ifndef KVM_ADDRESS_SPACE_NUM
#define KVM_ADDRESS_SPACE_NUM	1
#endif

/*
 * For the normal pfn, the highest 12 bits should be zero,
 * so we can mask bit 62 ~ bit 52  to indicate the error pfn,
 * mask bit 63 to indicate the noslot pfn.
 */
#define KVM_PFN_ERR_MASK	(0x7ffULL << 52)
#define KVM_PFN_ERR_NOSLOT_MASK	(0xfffULL << 52)
#define KVM_PFN_NOSLOT		(0x1ULL << 63)

#define KVM_PFN_ERR_FAULT	(KVM_PFN_ERR_MASK)
#define KVM_PFN_ERR_HWPOISON	(KVM_PFN_ERR_MASK + 1)
#define KVM_PFN_ERR_RO_FAULT	(KVM_PFN_ERR_MASK + 2)

/*
 * error pfns indicate that the gfn is in slot but faild to
 * translate it to pfn on host.
 */
static inline bool is_error_pfn(kvm_pfn_t pfn)
{
	return !!(pfn & KVM_PFN_ERR_MASK);
}

/*
 * error_noslot pfns indicate that the gfn can not be
 * translated to pfn - it is not in slot or failed to
 * translate it to pfn.
 */
static inline bool is_error_noslot_pfn(kvm_pfn_t pfn)
{
	return !!(pfn & KVM_PFN_ERR_NOSLOT_MASK);
}

/* noslot pfn indicates that the gfn is not in slot. */
static inline bool is_noslot_pfn(kvm_pfn_t pfn)
{
	return pfn == KVM_PFN_NOSLOT;
}

/*
 * architectures with KVM_HVA_ERR_BAD other than PAGE_OFFSET (e.g. s390)
 * provide own defines and kvm_is_error_hva
 */
#ifndef KVM_HVA_ERR_BAD

#define KVM_HVA_ERR_BAD		(PAGE_OFFSET)
#define KVM_HVA_ERR_RO_BAD	(PAGE_OFFSET + PAGE_SIZE)

static inline bool kvm_is_error_hva(unsigned long addr)
{
	return addr >= PAGE_OFFSET;
}

#endif

#define KVM_ERR_PTR_BAD_PAGE	(ERR_PTR(-ENOENT))

static inline bool is_error_page(struct page *page)
{
	return IS_ERR(page);
}

#define KVM_REQUEST_MASK           GENMASK(7,0)
#define KVM_REQUEST_NO_WAKEUP      BIT(8)
#define KVM_REQUEST_WAIT           BIT(9)
/*
 * Architecture-independent vcpu->requests bit members
 * Bits 4-7 are reserved for more arch-independent bits.
 */
#define KVM_REQ_TLB_FLUSH         (0 | KVM_REQUEST_WAIT | KVM_REQUEST_NO_WAKEUP)
#define KVM_REQ_MMU_RELOAD        (1 | KVM_REQUEST_WAIT | KVM_REQUEST_NO_WAKEUP)
#define KVM_REQ_UNBLOCK           2
#define KVM_REQ_UNHALT            3
#define KVM_REQUEST_ARCH_BASE     8

#define KVM_ARCH_REQ_FLAGS(nr, flags) ({ \
	BUILD_BUG_ON((unsigned)(nr) >= (sizeof_field(struct kvm_vcpu, requests) * 8) - KVM_REQUEST_ARCH_BASE); \
	(unsigned)(((nr) + KVM_REQUEST_ARCH_BASE) | (flags)); \
})
#define KVM_ARCH_REQ(nr)           KVM_ARCH_REQ_FLAGS(nr, 0)

#define KVM_USERSPACE_IRQ_SOURCE_ID		0
#define KVM_IRQFD_RESAMPLE_IRQ_SOURCE_ID	1

extern struct mutex kvm_lock;
extern struct list_head vm_list;

struct kvm_io_range {
	gpa_t addr;
	int len;
	struct kvm_io_device *dev;
};

#define NR_IOBUS_DEVS 1000

struct kvm_io_bus {
	int dev_count;
	int ioeventfd_count;
	struct kvm_io_range range[];
};

enum kvm_bus {
	KVM_MMIO_BUS,
	KVM_PIO_BUS,
	KVM_VIRTIO_CCW_NOTIFY_BUS,
	KVM_FAST_MMIO_BUS,
	KVM_NR_BUSES
};

int kvm_io_bus_write(struct kvm_vcpu *vcpu, enum kvm_bus bus_idx, gpa_t addr,
		     int len, const void *val);
int kvm_io_bus_write_cookie(struct kvm_vcpu *vcpu, enum kvm_bus bus_idx,
			    gpa_t addr, int len, const void *val, long cookie);
int kvm_io_bus_read(struct kvm_vcpu *vcpu, enum kvm_bus bus_idx, gpa_t addr,
		    int len, void *val);
int kvm_io_bus_register_dev(struct kvm *kvm, enum kvm_bus bus_idx, gpa_t addr,
			    int len, struct kvm_io_device *dev);
int kvm_io_bus_unregister_dev(struct kvm *kvm, enum kvm_bus bus_idx,
			      struct kvm_io_device *dev);
struct kvm_io_device *kvm_io_bus_get_dev(struct kvm *kvm, enum kvm_bus bus_idx,
					 gpa_t addr);

#ifdef CONFIG_KVM_ASYNC_PF
struct kvm_async_pf {
	struct work_struct work;
	struct list_head link;
	struct list_head queue;
	struct kvm_vcpu *vcpu;
	struct mm_struct *mm;
	gpa_t cr2_or_gpa;
	unsigned long addr;
	struct kvm_arch_async_pf arch;
	bool   wakeup_all;
	bool notpresent_injected;
};

void kvm_clear_async_pf_completion_queue(struct kvm_vcpu *vcpu);
void kvm_check_async_pf_completion(struct kvm_vcpu *vcpu);
bool kvm_setup_async_pf(struct kvm_vcpu *vcpu, gpa_t cr2_or_gpa,
			unsigned long hva, struct kvm_arch_async_pf *arch);
int kvm_async_pf_wakeup_all(struct kvm_vcpu *vcpu);
#endif

#ifdef KVM_ARCH_WANT_MMU_NOTIFIER
struct kvm_gfn_range {
	struct kvm_memory_slot *slot;
	gfn_t start;
	gfn_t end;
	pte_t pte;
	bool may_block;
};
bool kvm_unmap_gfn_range(struct kvm *kvm, struct kvm_gfn_range *range);
bool kvm_age_gfn(struct kvm *kvm, struct kvm_gfn_range *range);
bool kvm_test_age_gfn(struct kvm *kvm, struct kvm_gfn_range *range);
bool kvm_set_spte_gfn(struct kvm *kvm, struct kvm_gfn_range *range);
#endif

enum {
	OUTSIDE_GUEST_MODE,
	IN_GUEST_MODE,
	EXITING_GUEST_MODE,
	READING_SHADOW_PAGE_TABLES,
};

#define KVM_UNMAPPED_PAGE	((void *) 0x500 + POISON_POINTER_DELTA)

struct kvm_host_map {
	/*
	 * Only valid if the 'pfn' is managed by the host kernel (i.e. There is
	 * a 'struct page' for it. When using mem= kernel parameter some memory
	 * can be used as guest memory but they are not managed by host
	 * kernel).
	 * If 'pfn' is not managed by the host kernel, this field is
	 * initialized to KVM_UNMAPPED_PAGE.
	 */
	struct page *page;
	void *hva;
	kvm_pfn_t pfn;
	kvm_pfn_t gfn;
};

/*
 * Used to check if the mapping is valid or not. Never use 'kvm_host_map'
 * directly to check for that.
 */
static inline bool kvm_vcpu_mapped(struct kvm_host_map *map)
{
	return !!map->hva;
}

static inline bool kvm_vcpu_can_poll(ktime_t cur, ktime_t stop)
{
	return single_task_running() && !need_resched() && ktime_before(cur, stop);
}

/*
 * Sometimes a large or cross-page mmio needs to be broken up into separate
 * exits for userspace servicing.
 */
struct kvm_mmio_fragment {
	gpa_t gpa;
	void *data;
	unsigned len;
};

struct kvm_vcpu {
	struct kvm *kvm;
#ifdef CONFIG_PREEMPT_NOTIFIERS
	struct preempt_notifier preempt_notifier;
#endif
	int cpu;
	int vcpu_id; /* id given by userspace at creation */
	int vcpu_idx; /* index in kvm->vcpus array */
	int srcu_idx;
	int mode;
	u64 requests;
	unsigned long guest_debug;

	int pre_pcpu;
	struct list_head blocked_vcpu_list;

	struct mutex mutex;
	struct kvm_run *run;

	struct rcuwait wait;
	struct pid __rcu *pid;
	int sigset_active;
	sigset_t sigset;
	struct kvm_vcpu_stat stat;
	unsigned int halt_poll_ns;
	bool valid_wakeup;

#ifdef CONFIG_HAS_IOMEM
	int mmio_needed;
	int mmio_read_completed;
	int mmio_is_write;
	int mmio_cur_fragment;
	int mmio_nr_fragments;
	struct kvm_mmio_fragment mmio_fragments[KVM_MAX_MMIO_FRAGMENTS];
#endif

#ifdef CONFIG_KVM_ASYNC_PF
	struct {
		u32 queued;
		struct list_head queue;
		struct list_head done;
		spinlock_t lock;
	} async_pf;
#endif

#ifdef CONFIG_HAVE_KVM_CPU_RELAX_INTERCEPT
	/*
	 * Cpu relax intercept or pause loop exit optimization
	 * in_spin_loop: set when a vcpu does a pause loop exit
	 *  or cpu relax intercepted.
	 * dy_eligible: indicates whether vcpu is eligible for directed yield.
	 */
	struct {
		bool in_spin_loop;
		bool dy_eligible;
	} spin_loop;
#endif
	bool preempted;
	bool ready;
	struct kvm_vcpu_arch arch;
	struct kvm_dirty_ring dirty_ring;
};

/* must be called with irqs disabled */
static __always_inline void guest_enter_irqoff(void)
{
	/*
	 * This is running in ioctl context so its safe to assume that it's the
	 * stime pending cputime to flush.
	 */
	instrumentation_begin();
	vtime_account_guest_enter();
	instrumentation_end();

	/*
	 * KVM does not hold any references to rcu protected data when it
	 * switches CPU into a guest mode. In fact switching to a guest mode
	 * is very similar to exiting to userspace from rcu point of view. In
	 * addition CPU may stay in a guest mode for quite a long time (up to
	 * one time slice). Lets treat guest mode as quiescent state, just like
	 * we do with user-mode execution.
	 */
	if (!context_tracking_guest_enter()) {
		instrumentation_begin();
		rcu_virt_note_context_switch(smp_processor_id());
		instrumentation_end();
	}
}

static __always_inline void guest_exit_irqoff(void)
{
	context_tracking_guest_exit();

	instrumentation_begin();
	/* Flush the guest cputime we spent on the guest */
	vtime_account_guest_exit();
	instrumentation_end();
}

static inline void guest_exit(void)
{
	unsigned long flags;

	local_irq_save(flags);
	guest_exit_irqoff();
	local_irq_restore(flags);
}

static inline int kvm_vcpu_exiting_guest_mode(struct kvm_vcpu *vcpu)
{
	/*
	 * The memory barrier ensures a previous write to vcpu->requests cannot
	 * be reordered with the read of vcpu->mode.  It pairs with the general
	 * memory barrier following the write of vcpu->mode in VCPU RUN.
	 */
	smp_mb__before_atomic();
	return cmpxchg(&vcpu->mode, IN_GUEST_MODE, EXITING_GUEST_MODE);
}

/*
 * Some of the bitops functions do not support too long bitmaps.
 * This number must be determined not to exceed such limits.
 */
#define KVM_MEM_MAX_NR_PAGES ((1UL << 31) - 1)

struct kvm_memory_slot {
	gfn_t base_gfn;
	unsigned long npages;
	unsigned long *dirty_bitmap;
	struct kvm_arch_memory_slot arch;
	unsigned long userspace_addr;
	u32 flags;
	short id;
	u16 as_id;
};

static inline bool kvm_slot_dirty_track_enabled(struct kvm_memory_slot *slot)
{
	return slot->flags & KVM_MEM_LOG_DIRTY_PAGES;
}

static inline unsigned long kvm_dirty_bitmap_bytes(struct kvm_memory_slot *memslot)
{
	return ALIGN(memslot->npages, BITS_PER_LONG) / 8;
}

static inline unsigned long *kvm_second_dirty_bitmap(struct kvm_memory_slot *memslot)
{
	unsigned long len = kvm_dirty_bitmap_bytes(memslot);

	return memslot->dirty_bitmap + len / sizeof(*memslot->dirty_bitmap);
}

#ifndef KVM_DIRTY_LOG_MANUAL_CAPS
#define KVM_DIRTY_LOG_MANUAL_CAPS KVM_DIRTY_LOG_MANUAL_PROTECT_ENABLE
#endif

struct kvm_s390_adapter_int {
	u64 ind_addr;
	u64 summary_addr;
	u64 ind_offset;
	u32 summary_offset;
	u32 adapter_id;
};

struct kvm_hv_sint {
	u32 vcpu;
	u32 sint;
};

struct kvm_kernel_irq_routing_entry {
	u32 gsi;
	u32 type;
	int (*set)(struct kvm_kernel_irq_routing_entry *e,
		   struct kvm *kvm, int irq_source_id, int level,
		   bool line_status);
	union {
		struct {
			unsigned irqchip;
			unsigned pin;
		} irqchip;
		struct {
			u32 address_lo;
			u32 address_hi;
			u32 data;
			u32 flags;
			u32 devid;
		} msi;
		struct kvm_s390_adapter_int adapter;
		struct kvm_hv_sint hv_sint;
	};
	struct hlist_node link;
};

#ifdef CONFIG_HAVE_KVM_IRQ_ROUTING
struct kvm_irq_routing_table {
	int chip[KVM_NR_IRQCHIPS][KVM_IRQCHIP_NUM_PINS];
	u32 nr_rt_entries;
	/*
	 * Array indexed by gsi. Each entry contains list of irq chips
	 * the gsi is connected to.
	 */
	struct hlist_head map[];
};
#endif

#ifndef KVM_PRIVATE_MEM_SLOTS
#define KVM_PRIVATE_MEM_SLOTS 0
#endif

#define KVM_MEM_SLOTS_NUM SHRT_MAX
#define KVM_USER_MEM_SLOTS (KVM_MEM_SLOTS_NUM - KVM_PRIVATE_MEM_SLOTS)

#ifndef __KVM_VCPU_MULTIPLE_ADDRESS_SPACE
static inline int kvm_arch_vcpu_memslots_id(struct kvm_vcpu *vcpu)
{
	return 0;
}
#endif

/*
 * Note:
 * memslots are not sorted by id anymore, please use id_to_memslot()
 * to get the memslot by its id.
 */
struct kvm_memslots {
	u64 generation;
	/* The mapping table from slot id to the index in memslots[]. */
	short id_to_index[KVM_MEM_SLOTS_NUM];
	atomic_t lru_slot;
	int used_slots;
	struct kvm_memory_slot memslots[];
};

struct kvm {
#ifdef KVM_HAVE_MMU_RWLOCK
	rwlock_t mmu_lock;
#else
	spinlock_t mmu_lock;
#endif /* KVM_HAVE_MMU_RWLOCK */

	struct mutex slots_lock;
	struct mm_struct *mm; /* userspace tied to this vm */
	struct kvm_memslots __rcu *memslots[KVM_ADDRESS_SPACE_NUM];
	struct kvm_vcpu *vcpus[KVM_MAX_VCPUS];

	/*
	 * created_vcpus is protected by kvm->lock, and is incremented
	 * at the beginning of KVM_CREATE_VCPU.  online_vcpus is only
	 * incremented after storing the kvm_vcpu pointer in vcpus,
	 * and is accessed atomically.
	 */
	atomic_t online_vcpus;
	int created_vcpus;
	int last_boosted_vcpu;
	struct list_head vm_list;
	struct mutex lock;
	struct kvm_io_bus __rcu *buses[KVM_NR_BUSES];
#ifdef CONFIG_HAVE_KVM_EVENTFD
	struct {
		spinlock_t        lock;
		struct list_head  items;
		struct list_head  resampler_list;
		struct mutex      resampler_lock;
	} irqfds;
	struct list_head ioeventfds;
#endif
	struct kvm_vm_stat stat;
	struct kvm_arch arch;
	refcount_t users_count;
#ifdef CONFIG_KVM_MMIO
	struct kvm_coalesced_mmio_ring *coalesced_mmio_ring;
	spinlock_t ring_lock;
	struct list_head coalesced_zones;
#endif

	struct mutex irq_lock;
#ifdef CONFIG_HAVE_KVM_IRQCHIP
	/*
	 * Update side is protected by irq_lock.
	 */
	struct kvm_irq_routing_table __rcu *irq_routing;
#endif
#ifdef CONFIG_HAVE_KVM_IRQFD
	struct hlist_head irq_ack_notifier_list;
#endif

#if defined(CONFIG_MMU_NOTIFIER) && defined(KVM_ARCH_WANT_MMU_NOTIFIER)
	struct mmu_notifier mmu_notifier;
	unsigned long mmu_notifier_seq;
	long mmu_notifier_count;
	unsigned long mmu_notifier_range_start;
	unsigned long mmu_notifier_range_end;
#endif
	long tlbs_dirty;
	struct list_head devices;
	u64 manual_dirty_log_protect;
	struct dentry *debugfs_dentry;
	struct kvm_stat_data **debugfs_stat_data;
	struct srcu_struct srcu;
	struct srcu_struct irq_srcu;
	pid_t userspace_pid;
	unsigned int max_halt_poll_ns;
	u32 dirty_ring_size;
};

#define kvm_err(fmt, ...) \
	pr_err("kvm [%i]: " fmt, task_pid_nr(current), ## __VA_ARGS__)
#define kvm_info(fmt, ...) \
	pr_info("kvm [%i]: " fmt, task_pid_nr(current), ## __VA_ARGS__)
#define kvm_debug(fmt, ...) \
	pr_debug("kvm [%i]: " fmt, task_pid_nr(current), ## __VA_ARGS__)
#define kvm_debug_ratelimited(fmt, ...) \
	pr_debug_ratelimited("kvm [%i]: " fmt, task_pid_nr(current), \
			     ## __VA_ARGS__)
#define kvm_pr_unimpl(fmt, ...) \
	pr_err_ratelimited("kvm [%i]: " fmt, \
			   task_tgid_nr(current), ## __VA_ARGS__)

/* The guest did something we don't support. */
#define vcpu_unimpl(vcpu, fmt, ...)					\
	kvm_pr_unimpl("vcpu%i, guest rIP: 0x%lx " fmt,			\
			(vcpu)->vcpu_id, kvm_rip_read(vcpu), ## __VA_ARGS__)

#define vcpu_debug(vcpu, fmt, ...)					\
	kvm_debug("vcpu%i " fmt, (vcpu)->vcpu_id, ## __VA_ARGS__)
#define vcpu_debug_ratelimited(vcpu, fmt, ...)				\
	kvm_debug_ratelimited("vcpu%i " fmt, (vcpu)->vcpu_id,           \
			      ## __VA_ARGS__)
#define vcpu_err(vcpu, fmt, ...)					\
	kvm_err("vcpu%i " fmt, (vcpu)->vcpu_id, ## __VA_ARGS__)

static inline bool kvm_dirty_log_manual_protect_and_init_set(struct kvm *kvm)
{
	return !!(kvm->manual_dirty_log_protect & KVM_DIRTY_LOG_INITIALLY_SET);
}

static inline struct kvm_io_bus *kvm_get_bus(struct kvm *kvm, enum kvm_bus idx)
{
	return srcu_dereference_check(kvm->buses[idx], &kvm->srcu,
				      lockdep_is_held(&kvm->slots_lock) ||
				      !refcount_read(&kvm->users_count));
}

static inline struct kvm_vcpu *kvm_get_vcpu(struct kvm *kvm, int i)
{
	int num_vcpus = atomic_read(&kvm->online_vcpus);
	i = array_index_nospec(i, num_vcpus);

	/* Pairs with smp_wmb() in kvm_vm_ioctl_create_vcpu.  */
	smp_rmb();
	return kvm->vcpus[i];
}

#define kvm_for_each_vcpu(idx, vcpup, kvm) \
	for (idx = 0; \
	     idx < atomic_read(&kvm->online_vcpus) && \
	     (vcpup = kvm_get_vcpu(kvm, idx)) != NULL; \
	     idx++)

static inline struct kvm_vcpu *kvm_get_vcpu_by_id(struct kvm *kvm, int id)
{
	struct kvm_vcpu *vcpu = NULL;
	int i;

	if (id < 0)
		return NULL;
	if (id < KVM_MAX_VCPUS)
		vcpu = kvm_get_vcpu(kvm, id);
	if (vcpu && vcpu->vcpu_id == id)
		return vcpu;
	kvm_for_each_vcpu(i, vcpu, kvm)
		if (vcpu->vcpu_id == id)
			return vcpu;
	return NULL;
}

static inline int kvm_vcpu_get_idx(struct kvm_vcpu *vcpu)
{
	return vcpu->vcpu_idx;
}

#define kvm_for_each_memslot(memslot, slots)				\
	for (memslot = &slots->memslots[0];				\
	     memslot < slots->memslots + slots->used_slots; memslot++)	\
		if (WARN_ON_ONCE(!memslot->npages)) {			\
		} else

void kvm_vcpu_destroy(struct kvm_vcpu *vcpu);

void vcpu_load(struct kvm_vcpu *vcpu);
void vcpu_put(struct kvm_vcpu *vcpu);

#ifdef __KVM_HAVE_IOAPIC
void kvm_arch_post_irq_ack_notifier_list_update(struct kvm *kvm);
void kvm_arch_post_irq_routing_update(struct kvm *kvm);
#else
static inline void kvm_arch_post_irq_ack_notifier_list_update(struct kvm *kvm)
{
}
static inline void kvm_arch_post_irq_routing_update(struct kvm *kvm)
{
}
#endif

#ifdef CONFIG_HAVE_KVM_IRQFD
int kvm_irqfd_init(void);
void kvm_irqfd_exit(void);
#else
static inline int kvm_irqfd_init(void)
{
	return 0;
}

static inline void kvm_irqfd_exit(void)
{
}
#endif
int kvm_init(void *opaque, unsigned vcpu_size, unsigned vcpu_align,
		  struct module *module);
void kvm_exit(void);

void kvm_get_kvm(struct kvm *kvm);
void kvm_put_kvm(struct kvm *kvm);
bool file_is_kvm(struct file *file);
void kvm_put_kvm_no_destroy(struct kvm *kvm);

static inline struct kvm_memslots *__kvm_memslots(struct kvm *kvm, int as_id)
{
	as_id = array_index_nospec(as_id, KVM_ADDRESS_SPACE_NUM);
	return srcu_dereference_check(kvm->memslots[as_id], &kvm->srcu,
			lockdep_is_held(&kvm->slots_lock) ||
			!refcount_read(&kvm->users_count));
}

static inline struct kvm_memslots *kvm_memslots(struct kvm *kvm)
{
	return __kvm_memslots(kvm, 0);
}

static inline struct kvm_memslots *kvm_vcpu_memslots(struct kvm_vcpu *vcpu)
{
	int as_id = kvm_arch_vcpu_memslots_id(vcpu);

	return __kvm_memslots(vcpu->kvm, as_id);
}

static inline
struct kvm_memory_slot *id_to_memslot(struct kvm_memslots *slots, int id)
{
	int index = slots->id_to_index[id];
	struct kvm_memory_slot *slot;

	if (index < 0)
		return NULL;

	slot = &slots->memslots[index];

	WARN_ON(slot->id != id);
	return slot;
}

/*
 * KVM_SET_USER_MEMORY_REGION ioctl allows the following operations:
 * - create a new memory slot
 * - delete an existing memory slot
 * - modify an existing memory slot
 *   -- move it in the guest physical memory space
 *   -- just change its flags
 *
 * Since flags can be changed by some of these operations, the following
 * differentiation is the best we can do for __kvm_set_memory_region():
 */
enum kvm_mr_change {
	KVM_MR_CREATE,
	KVM_MR_DELETE,
	KVM_MR_MOVE,
	KVM_MR_FLAGS_ONLY,
};

int kvm_set_memory_region(struct kvm *kvm,
			  const struct kvm_userspace_memory_region *mem);
int __kvm_set_memory_region(struct kvm *kvm,
			    const struct kvm_userspace_memory_region *mem);
void kvm_arch_free_memslot(struct kvm *kvm, struct kvm_memory_slot *slot);
void kvm_arch_memslots_updated(struct kvm *kvm, u64 gen);
int kvm_arch_prepare_memory_region(struct kvm *kvm,
				struct kvm_memory_slot *memslot,
				const struct kvm_userspace_memory_region *mem,
				enum kvm_mr_change change);
void kvm_arch_commit_memory_region(struct kvm *kvm,
				const struct kvm_userspace_memory_region *mem,
				struct kvm_memory_slot *old,
				const struct kvm_memory_slot *new,
				enum kvm_mr_change change);
/* flush all memory translations */
void kvm_arch_flush_shadow_all(struct kvm *kvm);
/* flush memory translations pointing to 'slot' */
void kvm_arch_flush_shadow_memslot(struct kvm *kvm,
				   struct kvm_memory_slot *slot);

int gfn_to_page_many_atomic(struct kvm_memory_slot *slot, gfn_t gfn,
			    struct page **pages, int nr_pages);

struct page *gfn_to_page(struct kvm *kvm, gfn_t gfn);
unsigned long gfn_to_hva(struct kvm *kvm, gfn_t gfn);
unsigned long gfn_to_hva_prot(struct kvm *kvm, gfn_t gfn, bool *writable);
unsigned long gfn_to_hva_memslot(struct kvm_memory_slot *slot, gfn_t gfn);
unsigned long gfn_to_hva_memslot_prot(struct kvm_memory_slot *slot, gfn_t gfn,
				      bool *writable);
void kvm_release_page_clean(struct page *page);
void kvm_release_page_dirty(struct page *page);
void kvm_set_page_accessed(struct page *page);

kvm_pfn_t gfn_to_pfn(struct kvm *kvm, gfn_t gfn);
kvm_pfn_t gfn_to_pfn_prot(struct kvm *kvm, gfn_t gfn, bool write_fault,
		      bool *writable);
kvm_pfn_t gfn_to_pfn_memslot(struct kvm_memory_slot *slot, gfn_t gfn);
kvm_pfn_t gfn_to_pfn_memslot_atomic(struct kvm_memory_slot *slot, gfn_t gfn);
kvm_pfn_t __gfn_to_pfn_memslot(struct kvm_memory_slot *slot, gfn_t gfn,
			       bool atomic, bool *async, bool write_fault,
			       bool *writable, hva_t *hva);

void kvm_release_pfn_clean(kvm_pfn_t pfn);
void kvm_release_pfn_dirty(kvm_pfn_t pfn);
void kvm_set_pfn_dirty(kvm_pfn_t pfn);
void kvm_set_pfn_accessed(kvm_pfn_t pfn);
void kvm_get_pfn(kvm_pfn_t pfn);

void kvm_release_pfn(kvm_pfn_t pfn, bool dirty, struct gfn_to_pfn_cache *cache);
int kvm_read_guest_page(struct kvm *kvm, gfn_t gfn, void *data, int offset,
			int len);
int kvm_read_guest(struct kvm *kvm, gpa_t gpa, void *data, unsigned long len);
int kvm_read_guest_cached(struct kvm *kvm, struct gfn_to_hva_cache *ghc,
			   void *data, unsigned long len);
int kvm_read_guest_offset_cached(struct kvm *kvm, struct gfn_to_hva_cache *ghc,
				 void *data, unsigned int offset,
				 unsigned long len);
int kvm_write_guest_page(struct kvm *kvm, gfn_t gfn, const void *data,
			 int offset, int len);
int kvm_write_guest(struct kvm *kvm, gpa_t gpa, const void *data,
		    unsigned long len);
int kvm_write_guest_cached(struct kvm *kvm, struct gfn_to_hva_cache *ghc,
			   void *data, unsigned long len);
int kvm_write_guest_offset_cached(struct kvm *kvm, struct gfn_to_hva_cache *ghc,
				  void *data, unsigned int offset,
				  unsigned long len);
int kvm_gfn_to_hva_cache_init(struct kvm *kvm, struct gfn_to_hva_cache *ghc,
			      gpa_t gpa, unsigned long len);

#define __kvm_get_guest(kvm, gfn, offset, v)				\
({									\
	unsigned long __addr = gfn_to_hva(kvm, gfn);			\
	typeof(v) __user *__uaddr = (typeof(__uaddr))(__addr + offset);	\
	int __ret = -EFAULT;						\
									\
	if (!kvm_is_error_hva(__addr))					\
		__ret = get_user(v, __uaddr);				\
	__ret;								\
})

#define kvm_get_guest(kvm, gpa, v)					\
({									\
	gpa_t __gpa = gpa;						\
	struct kvm *__kvm = kvm;					\
									\
	__kvm_get_guest(__kvm, __gpa >> PAGE_SHIFT,			\
			offset_in_page(__gpa), v);			\
})

#define __kvm_put_guest(kvm, gfn, offset, v)				\
({									\
	unsigned long __addr = gfn_to_hva(kvm, gfn);			\
	typeof(v) __user *__uaddr = (typeof(__uaddr))(__addr + offset);	\
	int __ret = -EFAULT;						\
									\
	if (!kvm_is_error_hva(__addr))					\
		__ret = put_user(v, __uaddr);				\
	if (!__ret)							\
		mark_page_dirty(kvm, gfn);				\
	__ret;								\
})

#define kvm_put_guest(kvm, gpa, v)					\
({									\
	gpa_t __gpa = gpa;						\
	struct kvm *__kvm = kvm;					\
									\
	__kvm_put_guest(__kvm, __gpa >> PAGE_SHIFT,			\
			offset_in_page(__gpa), v);			\
})

int kvm_clear_guest(struct kvm *kvm, gpa_t gpa, unsigned long len);
struct kvm_memory_slot *gfn_to_memslot(struct kvm *kvm, gfn_t gfn);
bool kvm_is_visible_gfn(struct kvm *kvm, gfn_t gfn);
bool kvm_vcpu_is_visible_gfn(struct kvm_vcpu *vcpu, gfn_t gfn);
unsigned long kvm_host_page_size(struct kvm_vcpu *vcpu, gfn_t gfn);
void mark_page_dirty_in_slot(struct kvm *kvm, struct kvm_memory_slot *memslot, gfn_t gfn);
void mark_page_dirty(struct kvm *kvm, gfn_t gfn);

struct kvm_memslots *kvm_vcpu_memslots(struct kvm_vcpu *vcpu);
struct kvm_memory_slot *kvm_vcpu_gfn_to_memslot(struct kvm_vcpu *vcpu, gfn_t gfn);
kvm_pfn_t kvm_vcpu_gfn_to_pfn_atomic(struct kvm_vcpu *vcpu, gfn_t gfn);
kvm_pfn_t kvm_vcpu_gfn_to_pfn(struct kvm_vcpu *vcpu, gfn_t gfn);
int kvm_vcpu_map(struct kvm_vcpu *vcpu, gpa_t gpa, struct kvm_host_map *map);
int kvm_map_gfn(struct kvm_vcpu *vcpu, gfn_t gfn, struct kvm_host_map *map,
		struct gfn_to_pfn_cache *cache, bool atomic);
struct page *kvm_vcpu_gfn_to_page(struct kvm_vcpu *vcpu, gfn_t gfn);
void kvm_vcpu_unmap(struct kvm_vcpu *vcpu, struct kvm_host_map *map, bool dirty);
int kvm_unmap_gfn(struct kvm_vcpu *vcpu, struct kvm_host_map *map,
		  struct gfn_to_pfn_cache *cache, bool dirty, bool atomic);
unsigned long kvm_vcpu_gfn_to_hva(struct kvm_vcpu *vcpu, gfn_t gfn);
unsigned long kvm_vcpu_gfn_to_hva_prot(struct kvm_vcpu *vcpu, gfn_t gfn, bool *writable);
int kvm_vcpu_read_guest_page(struct kvm_vcpu *vcpu, gfn_t gfn, void *data, int offset,
			     int len);
int kvm_vcpu_read_guest_atomic(struct kvm_vcpu *vcpu, gpa_t gpa, void *data,
			       unsigned long len);
int kvm_vcpu_read_guest(struct kvm_vcpu *vcpu, gpa_t gpa, void *data,
			unsigned long len);
int kvm_vcpu_write_guest_page(struct kvm_vcpu *vcpu, gfn_t gfn, const void *data,
			      int offset, int len);
int kvm_vcpu_write_guest(struct kvm_vcpu *vcpu, gpa_t gpa, const void *data,
			 unsigned long len);
void kvm_vcpu_mark_page_dirty(struct kvm_vcpu *vcpu, gfn_t gfn);

void kvm_sigset_activate(struct kvm_vcpu *vcpu);
void kvm_sigset_deactivate(struct kvm_vcpu *vcpu);

void kvm_vcpu_block(struct kvm_vcpu *vcpu);
void kvm_arch_vcpu_blocking(struct kvm_vcpu *vcpu);
void kvm_arch_vcpu_unblocking(struct kvm_vcpu *vcpu);
bool kvm_vcpu_wake_up(struct kvm_vcpu *vcpu);
void kvm_vcpu_kick(struct kvm_vcpu *vcpu);
int kvm_vcpu_yield_to(struct kvm_vcpu *target);
void kvm_vcpu_on_spin(struct kvm_vcpu *vcpu, bool usermode_vcpu_not_eligible);

void kvm_flush_remote_tlbs(struct kvm *kvm);
void kvm_reload_remote_mmus(struct kvm *kvm);

#ifdef KVM_ARCH_NR_OBJS_PER_MEMORY_CACHE
int kvm_mmu_topup_memory_cache(struct kvm_mmu_memory_cache *mc, int min);
int kvm_mmu_memory_cache_nr_free_objects(struct kvm_mmu_memory_cache *mc);
void kvm_mmu_free_memory_cache(struct kvm_mmu_memory_cache *mc);
void *kvm_mmu_memory_cache_alloc(struct kvm_mmu_memory_cache *mc);
#endif

bool kvm_make_vcpus_request_mask(struct kvm *kvm, unsigned int req,
				 struct kvm_vcpu *except,
				 unsigned long *vcpu_bitmap, cpumask_var_t tmp);
bool kvm_make_all_cpus_request(struct kvm *kvm, unsigned int req);
bool kvm_make_all_cpus_request_except(struct kvm *kvm, unsigned int req,
				      struct kvm_vcpu *except);
bool kvm_make_cpus_request_mask(struct kvm *kvm, unsigned int req,
				unsigned long *vcpu_bitmap);

long kvm_arch_dev_ioctl(struct file *filp,
			unsigned int ioctl, unsigned long arg);
long kvm_arch_vcpu_ioctl(struct file *filp,
			 unsigned int ioctl, unsigned long arg);
vm_fault_t kvm_arch_vcpu_fault(struct kvm_vcpu *vcpu, struct vm_fault *vmf);

int kvm_vm_ioctl_check_extension(struct kvm *kvm, long ext);

void kvm_arch_mmu_enable_log_dirty_pt_masked(struct kvm *kvm,
					struct kvm_memory_slot *slot,
					gfn_t gfn_offset,
					unsigned long mask);
void kvm_arch_sync_dirty_log(struct kvm *kvm, struct kvm_memory_slot *memslot);

#ifdef CONFIG_KVM_GENERIC_DIRTYLOG_READ_PROTECT
void kvm_arch_flush_remote_tlbs_memslot(struct kvm *kvm,
					const struct kvm_memory_slot *memslot);
#else /* !CONFIG_KVM_GENERIC_DIRTYLOG_READ_PROTECT */
int kvm_vm_ioctl_get_dirty_log(struct kvm *kvm, struct kvm_dirty_log *log);
int kvm_get_dirty_log(struct kvm *kvm, struct kvm_dirty_log *log,
		      int *is_dirty, struct kvm_memory_slot **memslot);
#endif

int kvm_vm_ioctl_irq_line(struct kvm *kvm, struct kvm_irq_level *irq_level,
			bool line_status);
int kvm_vm_ioctl_enable_cap(struct kvm *kvm,
			    struct kvm_enable_cap *cap);
long kvm_arch_vm_ioctl(struct file *filp,
		       unsigned int ioctl, unsigned long arg);

int kvm_arch_vcpu_ioctl_get_fpu(struct kvm_vcpu *vcpu, struct kvm_fpu *fpu);
int kvm_arch_vcpu_ioctl_set_fpu(struct kvm_vcpu *vcpu, struct kvm_fpu *fpu);

int kvm_arch_vcpu_ioctl_translate(struct kvm_vcpu *vcpu,
				    struct kvm_translation *tr);

int kvm_arch_vcpu_ioctl_get_regs(struct kvm_vcpu *vcpu, struct kvm_regs *regs);
int kvm_arch_vcpu_ioctl_set_regs(struct kvm_vcpu *vcpu, struct kvm_regs *regs);
int kvm_arch_vcpu_ioctl_get_sregs(struct kvm_vcpu *vcpu,
				  struct kvm_sregs *sregs);
int kvm_arch_vcpu_ioctl_set_sregs(struct kvm_vcpu *vcpu,
				  struct kvm_sregs *sregs);
int kvm_arch_vcpu_ioctl_get_mpstate(struct kvm_vcpu *vcpu,
				    struct kvm_mp_state *mp_state);
int kvm_arch_vcpu_ioctl_set_mpstate(struct kvm_vcpu *vcpu,
				    struct kvm_mp_state *mp_state);
int kvm_arch_vcpu_ioctl_set_guest_debug(struct kvm_vcpu *vcpu,
					struct kvm_guest_debug *dbg);
int kvm_arch_vcpu_ioctl_run(struct kvm_vcpu *vcpu);

int kvm_arch_init(void *opaque);
void kvm_arch_exit(void);

void kvm_arch_sched_in(struct kvm_vcpu *vcpu, int cpu);

void kvm_arch_vcpu_load(struct kvm_vcpu *vcpu, int cpu);
void kvm_arch_vcpu_put(struct kvm_vcpu *vcpu);
int kvm_arch_vcpu_precreate(struct kvm *kvm, unsigned int id);
int kvm_arch_vcpu_create(struct kvm_vcpu *vcpu);
void kvm_arch_vcpu_postcreate(struct kvm_vcpu *vcpu);
void kvm_arch_vcpu_destroy(struct kvm_vcpu *vcpu);

#ifdef __KVM_HAVE_ARCH_VCPU_DEBUGFS
void kvm_arch_create_vcpu_debugfs(struct kvm_vcpu *vcpu, struct dentry *debugfs_dentry);
#endif

int kvm_arch_hardware_enable(void);
void kvm_arch_hardware_disable(void);
int kvm_arch_hardware_setup(void *opaque);
void kvm_arch_hardware_unsetup(void);
int kvm_arch_check_processor_compat(void *opaque);
int kvm_arch_vcpu_runnable(struct kvm_vcpu *vcpu);
bool kvm_arch_vcpu_in_kernel(struct kvm_vcpu *vcpu);
int kvm_arch_vcpu_should_kick(struct kvm_vcpu *vcpu);
bool kvm_arch_dy_runnable(struct kvm_vcpu *vcpu);
bool kvm_arch_dy_has_pending_interrupt(struct kvm_vcpu *vcpu);
int kvm_arch_post_init_vm(struct kvm *kvm);
void kvm_arch_pre_destroy_vm(struct kvm *kvm);

#ifndef __KVM_HAVE_ARCH_VM_ALLOC
/*
 * All architectures that want to use vzalloc currently also
 * need their own kvm_arch_alloc_vm implementation.
 */
static inline struct kvm *kvm_arch_alloc_vm(void)
{
	return kzalloc(sizeof(struct kvm), GFP_KERNEL);
}

static inline void kvm_arch_free_vm(struct kvm *kvm)
{
	kfree(kvm);
}
#endif

#ifndef __KVM_HAVE_ARCH_FLUSH_REMOTE_TLB
static inline int kvm_arch_flush_remote_tlb(struct kvm *kvm)
{
	return -ENOTSUPP;
}
#endif

#ifdef __KVM_HAVE_ARCH_NONCOHERENT_DMA
void kvm_arch_register_noncoherent_dma(struct kvm *kvm);
void kvm_arch_unregister_noncoherent_dma(struct kvm *kvm);
bool kvm_arch_has_noncoherent_dma(struct kvm *kvm);
#else
static inline void kvm_arch_register_noncoherent_dma(struct kvm *kvm)
{
}

static inline void kvm_arch_unregister_noncoherent_dma(struct kvm *kvm)
{
}

static inline bool kvm_arch_has_noncoherent_dma(struct kvm *kvm)
{
	return false;
}
#endif
#ifdef __KVM_HAVE_ARCH_ASSIGNED_DEVICE
void kvm_arch_start_assignment(struct kvm *kvm);
void kvm_arch_end_assignment(struct kvm *kvm);
bool kvm_arch_has_assigned_device(struct kvm *kvm);
#else
static inline void kvm_arch_start_assignment(struct kvm *kvm)
{
}

static inline void kvm_arch_end_assignment(struct kvm *kvm)
{
}

static inline bool kvm_arch_has_assigned_device(struct kvm *kvm)
{
	return false;
}
#endif

static inline struct rcuwait *kvm_arch_vcpu_get_wait(struct kvm_vcpu *vcpu)
{
#ifdef __KVM_HAVE_ARCH_WQP
	return vcpu->arch.waitp;
#else
	return &vcpu->wait;
#endif
}

#ifdef __KVM_HAVE_ARCH_INTC_INITIALIZED
/*
 * returns true if the virtual interrupt controller is initialized and
 * ready to accept virtual IRQ. On some architectures the virtual interrupt
 * controller is dynamically instantiated and this is not always true.
 */
bool kvm_arch_intc_initialized(struct kvm *kvm);
#else
static inline bool kvm_arch_intc_initialized(struct kvm *kvm)
{
	return true;
}
#endif

int kvm_arch_init_vm(struct kvm *kvm, unsigned long type);
void kvm_arch_destroy_vm(struct kvm *kvm);
void kvm_arch_sync_events(struct kvm *kvm);

int kvm_cpu_has_pending_timer(struct kvm_vcpu *vcpu);

bool kvm_is_reserved_pfn(kvm_pfn_t pfn);
bool kvm_is_zone_device_pfn(kvm_pfn_t pfn);
bool kvm_is_transparent_hugepage(kvm_pfn_t pfn);

struct kvm_irq_ack_notifier {
	struct hlist_node link;
	unsigned gsi;
	void (*irq_acked)(struct kvm_irq_ack_notifier *kian);
};

int kvm_irq_map_gsi(struct kvm *kvm,
		    struct kvm_kernel_irq_routing_entry *entries, int gsi);
int kvm_irq_map_chip_pin(struct kvm *kvm, unsigned irqchip, unsigned pin);

int kvm_set_irq(struct kvm *kvm, int irq_source_id, u32 irq, int level,
		bool line_status);
int kvm_set_msi(struct kvm_kernel_irq_routing_entry *irq_entry, struct kvm *kvm,
		int irq_source_id, int level, bool line_status);
int kvm_arch_set_irq_inatomic(struct kvm_kernel_irq_routing_entry *e,
			       struct kvm *kvm, int irq_source_id,
			       int level, bool line_status);
bool kvm_irq_has_notifier(struct kvm *kvm, unsigned irqchip, unsigned pin);
void kvm_notify_acked_gsi(struct kvm *kvm, int gsi);
void kvm_notify_acked_irq(struct kvm *kvm, unsigned irqchip, unsigned pin);
void kvm_register_irq_ack_notifier(struct kvm *kvm,
				   struct kvm_irq_ack_notifier *kian);
void kvm_unregister_irq_ack_notifier(struct kvm *kvm,
				   struct kvm_irq_ack_notifier *kian);
int kvm_request_irq_source_id(struct kvm *kvm);
void kvm_free_irq_source_id(struct kvm *kvm, int irq_source_id);
bool kvm_arch_irqfd_allowed(struct kvm *kvm, struct kvm_irqfd *args);

/*
 * search_memslots() and __gfn_to_memslot() are here because they are
 * used in non-modular code in arch/powerpc/kvm/book3s_hv_rm_mmu.c.
 * gfn_to_memslot() itself isn't here as an inline because that would
 * bloat other code too much.
 *
 * IMPORTANT: Slots are sorted from highest GFN to lowest GFN!
 */
static inline struct kvm_memory_slot *
search_memslots(struct kvm_memslots *slots, gfn_t gfn)
{
	int start = 0, end = slots->used_slots;
	int slot = atomic_read(&slots->lru_slot);
	struct kvm_memory_slot *memslots = slots->memslots;

	if (unlikely(!slots->used_slots))
		return NULL;

	if (gfn >= memslots[slot].base_gfn &&
	    gfn < memslots[slot].base_gfn + memslots[slot].npages)
		return &memslots[slot];

	while (start < end) {
		slot = start + (end - start) / 2;

		if (gfn >= memslots[slot].base_gfn)
			end = slot;
		else
			start = slot + 1;
	}

	if (start < slots->used_slots && gfn >= memslots[start].base_gfn &&
	    gfn < memslots[start].base_gfn + memslots[start].npages) {
		atomic_set(&slots->lru_slot, start);
		return &memslots[start];
	}

	return NULL;
}

static inline struct kvm_memory_slot *
__gfn_to_memslot(struct kvm_memslots *slots, gfn_t gfn)
{
	return search_memslots(slots, gfn);
}

static inline unsigned long
__gfn_to_hva_memslot(const struct kvm_memory_slot *slot, gfn_t gfn)
{
	/*
	 * The index was checked originally in search_memslots.  To avoid
	 * that a malicious guest builds a Spectre gadget out of e.g. page
	 * table walks, do not let the processor speculate loads outside
	 * the guest's registered memslots.
	 */
	unsigned long offset = gfn - slot->base_gfn;
	offset = array_index_nospec(offset, slot->npages);
	return slot->userspace_addr + offset * PAGE_SIZE;
}

static inline int memslot_id(struct kvm *kvm, gfn_t gfn)
{
	return gfn_to_memslot(kvm, gfn)->id;
}

static inline gfn_t
hva_to_gfn_memslot(unsigned long hva, struct kvm_memory_slot *slot)
{
	gfn_t gfn_offset = (hva - slot->userspace_addr) >> PAGE_SHIFT;

	return slot->base_gfn + gfn_offset;
}

static inline gpa_t gfn_to_gpa(gfn_t gfn)
{
	return (gpa_t)gfn << PAGE_SHIFT;
}

static inline gfn_t gpa_to_gfn(gpa_t gpa)
{
	return (gfn_t)(gpa >> PAGE_SHIFT);
}

static inline hpa_t pfn_to_hpa(kvm_pfn_t pfn)
{
	return (hpa_t)pfn << PAGE_SHIFT;
}

static inline struct page *kvm_vcpu_gpa_to_page(struct kvm_vcpu *vcpu,
						gpa_t gpa)
{
	return kvm_vcpu_gfn_to_page(vcpu, gpa_to_gfn(gpa));
}

static inline bool kvm_is_error_gpa(struct kvm *kvm, gpa_t gpa)
{
	unsigned long hva = gfn_to_hva(kvm, gpa_to_gfn(gpa));

	return kvm_is_error_hva(hva);
}

enum kvm_stat_kind {
	KVM_STAT_VM,
	KVM_STAT_VCPU,
};

struct kvm_stat_data {
	struct kvm *kvm;
	struct kvm_stats_debugfs_item *dbgfs_item;
};

struct kvm_stats_debugfs_item {
	const char *name;
	int offset;
	enum kvm_stat_kind kind;
	int mode;
};

#define KVM_DBGFS_GET_MODE(dbgfs_item)                                         \
	((dbgfs_item)->mode ? (dbgfs_item)->mode : 0644)

#define VM_STAT(n, x, ...) 							\
	{ n, offsetof(struct kvm, stat.x), KVM_STAT_VM, ## __VA_ARGS__ }
#define VCPU_STAT(n, x, ...)							\
	{ n, offsetof(struct kvm_vcpu, stat.x), KVM_STAT_VCPU, ## __VA_ARGS__ }

extern struct kvm_stats_debugfs_item debugfs_entries[];
extern struct dentry *kvm_debugfs_dir;

#if defined(CONFIG_MMU_NOTIFIER) && defined(KVM_ARCH_WANT_MMU_NOTIFIER)
static inline int mmu_notifier_retry(struct kvm *kvm, unsigned long mmu_seq)
{
	if (unlikely(kvm->mmu_notifier_count))
		return 1;
	/*
	 * Ensure the read of mmu_notifier_count happens before the read
	 * of mmu_notifier_seq.  This interacts with the smp_wmb() in
	 * mmu_notifier_invalidate_range_end to make sure that the caller
	 * either sees the old (non-zero) value of mmu_notifier_count or
	 * the new (incremented) value of mmu_notifier_seq.
	 * PowerPC Book3s HV KVM calls this under a per-page lock
	 * rather than under kvm->mmu_lock, for scalability, so
	 * can't rely on kvm->mmu_lock to keep things ordered.
	 */
	smp_rmb();
	if (kvm->mmu_notifier_seq != mmu_seq)
		return 1;
	return 0;
}

static inline int mmu_notifier_retry_hva(struct kvm *kvm,
					 unsigned long mmu_seq,
					 unsigned long hva)
{
	lockdep_assert_held(&kvm->mmu_lock);
	/*
	 * If mmu_notifier_count is non-zero, then the range maintained by
	 * kvm_mmu_notifier_invalidate_range_start contains all addresses that
	 * might be being invalidated. Note that it may include some false
	 * positives, due to shortcuts when handing concurrent invalidations.
	 */
	if (unlikely(kvm->mmu_notifier_count) &&
	    hva >= kvm->mmu_notifier_range_start &&
	    hva < kvm->mmu_notifier_range_end)
		return 1;
	if (kvm->mmu_notifier_seq != mmu_seq)
		return 1;
	return 0;
}
#endif

#ifdef CONFIG_HAVE_KVM_IRQ_ROUTING

#define KVM_MAX_IRQ_ROUTES 4096 /* might need extension/rework in the future */

bool kvm_arch_can_set_irq_routing(struct kvm *kvm);
int kvm_set_irq_routing(struct kvm *kvm,
			const struct kvm_irq_routing_entry *entries,
			unsigned nr,
			unsigned flags);
int kvm_set_routing_entry(struct kvm *kvm,
			  struct kvm_kernel_irq_routing_entry *e,
			  const struct kvm_irq_routing_entry *ue);
void kvm_free_irq_routing(struct kvm *kvm);

#else

static inline void kvm_free_irq_routing(struct kvm *kvm) {}

#endif

int kvm_send_userspace_msi(struct kvm *kvm, struct kvm_msi *msi);

#ifdef CONFIG_HAVE_KVM_EVENTFD

void kvm_eventfd_init(struct kvm *kvm);
int kvm_ioeventfd(struct kvm *kvm, struct kvm_ioeventfd *args);

#ifdef CONFIG_HAVE_KVM_IRQFD
int kvm_irqfd(struct kvm *kvm, struct kvm_irqfd *args);
void kvm_irqfd_release(struct kvm *kvm);
void kvm_irq_routing_update(struct kvm *);
#else
static inline int kvm_irqfd(struct kvm *kvm, struct kvm_irqfd *args)
{
	return -EINVAL;
}

static inline void kvm_irqfd_release(struct kvm *kvm) {}
#endif

#else

static inline void kvm_eventfd_init(struct kvm *kvm) {}

static inline int kvm_irqfd(struct kvm *kvm, struct kvm_irqfd *args)
{
	return -EINVAL;
}

static inline void kvm_irqfd_release(struct kvm *kvm) {}

#ifdef CONFIG_HAVE_KVM_IRQCHIP
static inline void kvm_irq_routing_update(struct kvm *kvm)
{
}
#endif

static inline int kvm_ioeventfd(struct kvm *kvm, struct kvm_ioeventfd *args)
{
	return -ENOSYS;
}

#endif /* CONFIG_HAVE_KVM_EVENTFD */

void kvm_arch_irq_routing_update(struct kvm *kvm);

static inline void kvm_make_request(int req, struct kvm_vcpu *vcpu)
{
	/*
	 * Ensure the rest of the request is published to kvm_check_request's
	 * caller.  Paired with the smp_mb__after_atomic in kvm_check_request.
	 */
	smp_wmb();
	set_bit(req & KVM_REQUEST_MASK, (void *)&vcpu->requests);
}

static inline bool kvm_request_pending(struct kvm_vcpu *vcpu)
{
	return READ_ONCE(vcpu->requests);
}

static inline bool kvm_test_request(int req, struct kvm_vcpu *vcpu)
{
	return test_bit(req & KVM_REQUEST_MASK, (void *)&vcpu->requests);
}

static inline void kvm_clear_request(int req, struct kvm_vcpu *vcpu)
{
	clear_bit(req & KVM_REQUEST_MASK, (void *)&vcpu->requests);
}

static inline bool kvm_check_request(int req, struct kvm_vcpu *vcpu)
{
	if (kvm_test_request(req, vcpu)) {
		kvm_clear_request(req, vcpu);

		/*
		 * Ensure the rest of the request is visible to kvm_check_request's
		 * caller.  Paired with the smp_wmb in kvm_make_request.
		 */
		smp_mb__after_atomic();
		return true;
	} else {
		return false;
	}
}

extern bool kvm_rebooting;

extern unsigned int halt_poll_ns;
extern unsigned int halt_poll_ns_grow;
extern unsigned int halt_poll_ns_grow_start;
extern unsigned int halt_poll_ns_shrink;

struct kvm_device {
	const struct kvm_device_ops *ops;
	struct kvm *kvm;
	void *private;
	struct list_head vm_node;
};

/* create, destroy, and name are mandatory */
struct kvm_device_ops {
	const char *name;

	/*
	 * create is called holding kvm->lock and any operations not suitable
	 * to do while holding the lock should be deferred to init (see
	 * below).
	 */
	int (*create)(struct kvm_device *dev, u32 type);

	/*
	 * init is called after create if create is successful and is called
	 * outside of holding kvm->lock.
	 */
	void (*init)(struct kvm_device *dev);

	/*
	 * Destroy is responsible for freeing dev.
	 *
	 * Destroy may be called before or after destructors are called
	 * on emulated I/O regions, depending on whether a reference is
	 * held by a vcpu or other kvm component that gets destroyed
	 * after the emulated I/O.
	 */
	void (*destroy)(struct kvm_device *dev);

	/*
	 * Release is an alternative method to free the device. It is
	 * called when the device file descriptor is closed. Once
	 * release is called, the destroy method will not be called
	 * anymore as the device is removed from the device list of
	 * the VM. kvm->lock is held.
	 */
	void (*release)(struct kvm_device *dev);

	int (*set_attr)(struct kvm_device *dev, struct kvm_device_attr *attr);
	int (*get_attr)(struct kvm_device *dev, struct kvm_device_attr *attr);
	int (*has_attr)(struct kvm_device *dev, struct kvm_device_attr *attr);
	long (*ioctl)(struct kvm_device *dev, unsigned int ioctl,
		      unsigned long arg);
	int (*mmap)(struct kvm_device *dev, struct vm_area_struct *vma);
};

void kvm_device_get(struct kvm_device *dev);
void kvm_device_put(struct kvm_device *dev);
struct kvm_device *kvm_device_from_filp(struct file *filp);
int kvm_register_device_ops(const struct kvm_device_ops *ops, u32 type);
void kvm_unregister_device_ops(u32 type);

extern struct kvm_device_ops kvm_mpic_ops;
extern struct kvm_device_ops kvm_arm_vgic_v2_ops;
extern struct kvm_device_ops kvm_arm_vgic_v3_ops;

#ifdef CONFIG_HAVE_KVM_CPU_RELAX_INTERCEPT

static inline void kvm_vcpu_set_in_spin_loop(struct kvm_vcpu *vcpu, bool val)
{
	vcpu->spin_loop.in_spin_loop = val;
}
static inline void kvm_vcpu_set_dy_eligible(struct kvm_vcpu *vcpu, bool val)
{
	vcpu->spin_loop.dy_eligible = val;
}

#else /* !CONFIG_HAVE_KVM_CPU_RELAX_INTERCEPT */

static inline void kvm_vcpu_set_in_spin_loop(struct kvm_vcpu *vcpu, bool val)
{
}

static inline void kvm_vcpu_set_dy_eligible(struct kvm_vcpu *vcpu, bool val)
{
}
#endif /* CONFIG_HAVE_KVM_CPU_RELAX_INTERCEPT */

static inline bool kvm_is_visible_memslot(struct kvm_memory_slot *memslot)
{
	return (memslot && memslot->id < KVM_USER_MEM_SLOTS &&
		!(memslot->flags & KVM_MEMSLOT_INVALID));
}

struct kvm_vcpu *kvm_get_running_vcpu(void);
struct kvm_vcpu * __percpu *kvm_get_running_vcpus(void);

#ifdef CONFIG_HAVE_KVM_IRQ_BYPASS
bool kvm_arch_has_irq_bypass(void);
int kvm_arch_irq_bypass_add_producer(struct irq_bypass_consumer *,
			   struct irq_bypass_producer *);
void kvm_arch_irq_bypass_del_producer(struct irq_bypass_consumer *,
			   struct irq_bypass_producer *);
void kvm_arch_irq_bypass_stop(struct irq_bypass_consumer *);
void kvm_arch_irq_bypass_start(struct irq_bypass_consumer *);
int kvm_arch_update_irqfd_routing(struct kvm *kvm, unsigned int host_irq,
				  uint32_t guest_irq, bool set);
#endif /* CONFIG_HAVE_KVM_IRQ_BYPASS */

#ifdef CONFIG_HAVE_KVM_INVALID_WAKEUPS
/* If we wakeup during the poll time, was it a sucessful poll? */
static inline bool vcpu_valid_wakeup(struct kvm_vcpu *vcpu)
{
	return vcpu->valid_wakeup;
}

#else
static inline bool vcpu_valid_wakeup(struct kvm_vcpu *vcpu)
{
	return true;
}
#endif /* CONFIG_HAVE_KVM_INVALID_WAKEUPS */

#ifdef CONFIG_HAVE_KVM_NO_POLL
/* Callback that tells if we must not poll */
bool kvm_arch_no_poll(struct kvm_vcpu *vcpu);
#else
static inline bool kvm_arch_no_poll(struct kvm_vcpu *vcpu)
{
	return false;
}
#endif /* CONFIG_HAVE_KVM_NO_POLL */

#ifdef CONFIG_HAVE_KVM_VCPU_ASYNC_IOCTL
long kvm_arch_vcpu_async_ioctl(struct file *filp,
			       unsigned int ioctl, unsigned long arg);
#else
static inline long kvm_arch_vcpu_async_ioctl(struct file *filp,
					     unsigned int ioctl,
					     unsigned long arg)
{
	return -ENOIOCTLCMD;
}
#endif /* CONFIG_HAVE_KVM_VCPU_ASYNC_IOCTL */

void kvm_arch_mmu_notifier_invalidate_range(struct kvm *kvm,
					    unsigned long start, unsigned long end);

#ifdef CONFIG_HAVE_KVM_VCPU_RUN_PID_CHANGE
int kvm_arch_vcpu_run_pid_change(struct kvm_vcpu *vcpu);
#else
static inline int kvm_arch_vcpu_run_pid_change(struct kvm_vcpu *vcpu)
{
	return 0;
}
#endif /* CONFIG_HAVE_KVM_VCPU_RUN_PID_CHANGE */

typedef int (*kvm_vm_thread_fn_t)(struct kvm *kvm, uintptr_t data);

int kvm_vm_create_worker_thread(struct kvm *kvm, kvm_vm_thread_fn_t thread_fn,
				uintptr_t data, const char *name,
				struct task_struct **thread_ptr);

#ifdef CONFIG_KVM_XFER_TO_GUEST_WORK
static inline void kvm_handle_signal_exit(struct kvm_vcpu *vcpu)
{
	vcpu->run->exit_reason = KVM_EXIT_INTR;
	vcpu->stat.signal_exits++;
}
#endif /* CONFIG_KVM_XFER_TO_GUEST_WORK */

/*
 * This defines how many reserved entries we want to keep before we
 * kick the vcpu to the userspace to avoid dirty ring full.  This
 * value can be tuned to higher if e.g. PML is enabled on the host.
 */
#define  KVM_DIRTY_RING_RSVD_ENTRIES  64

/* Max number of entries allowed for each kvm dirty ring */
#define  KVM_DIRTY_RING_MAX_ENTRIES  65536

#endif
