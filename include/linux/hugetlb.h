/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_HUGETLB_H
#define _LINUX_HUGETLB_H

#include <linux/mm_types.h>
#include <linux/mmdebug.h>
#include <linux/fs.h>
#include <linux/hugetlb_inline.h>
#include <linux/cgroup.h>
#include <linux/list.h>
#include <linux/kref.h>
#include <linux/pgtable.h>
#include <linux/gfp.h>
#include <linux/userfaultfd_k.h>

struct ctl_table;
struct user_struct;
struct mmu_gather;

#ifndef is_hugepd
typedef struct { unsigned long pd; } hugepd_t;
#define is_hugepd(hugepd) (0)
#define __hugepd(x) ((hugepd_t) { (x) })
#endif

#ifdef CONFIG_HUGETLB_PAGE

#include <linux/mempolicy.h>
#include <linux/shm.h>
#include <asm/tlbflush.h>

struct hugepage_subpool {
	spinlock_t lock;
	long count;
	long max_hpages;	/* Maximum huge pages or -1 if no maximum. */
	long used_hpages;	/* Used count against maximum, includes */
				/* both alloced and reserved pages. */
	struct hstate *hstate;
	long min_hpages;	/* Minimum huge pages or -1 if no minimum. */
	long rsv_hpages;	/* Pages reserved against global pool to */
				/* satisfy minimum size. */
};

struct resv_map {
	struct kref refs;
	spinlock_t lock;
	struct list_head regions;
	long adds_in_progress;
	struct list_head region_cache;
	long region_cache_count;
#ifdef CONFIG_CGROUP_HUGETLB
	/*
	 * On private mappings, the counter to uncharge reservations is stored
	 * here. If these fields are 0, then either the mapping is shared, or
	 * cgroup accounting is disabled for this resv_map.
	 */
	struct page_counter *reservation_counter;
	unsigned long pages_per_hpage;
	struct cgroup_subsys_state *css;
#endif
};

/*
 * Region tracking -- allows tracking of reservations and instantiated pages
 *                    across the pages in a mapping.
 *
 * The region data structures are embedded into a resv_map and protected
 * by a resv_map's lock.  The set of regions within the resv_map represent
 * reservations for huge pages, or huge pages that have already been
 * instantiated within the map.  The from and to elements are huge page
 * indicies into the associated mapping.  from indicates the starting index
 * of the region.  to represents the first index past the end of  the region.
 *
 * For example, a file region structure with from == 0 and to == 4 represents
 * four huge pages in a mapping.  It is important to note that the to element
 * represents the first element past the end of the region. This is used in
 * arithmetic as 4(to) - 0(from) = 4 huge pages in the region.
 *
 * Interval notation of the form [from, to) will be used to indicate that
 * the endpoint from is inclusive and to is exclusive.
 */
struct file_region {
	struct list_head link;
	long from;
	long to;
#ifdef CONFIG_CGROUP_HUGETLB
	/*
	 * On shared mappings, each reserved region appears as a struct
	 * file_region in resv_map. These fields hold the info needed to
	 * uncharge each reservation.
	 */
	struct page_counter *reservation_counter;
	struct cgroup_subsys_state *css;
#endif
};

extern struct resv_map *resv_map_alloc(void);
void resv_map_release(struct kref *ref);

extern spinlock_t hugetlb_lock;
extern int hugetlb_max_hstate __read_mostly;
#define for_each_hstate(h) \
	for ((h) = hstates; (h) < &hstates[hugetlb_max_hstate]; (h)++)

struct hugepage_subpool *hugepage_new_subpool(struct hstate *h, long max_hpages,
						long min_hpages);
void hugepage_put_subpool(struct hugepage_subpool *spool);

void reset_vma_resv_huge_pages(struct vm_area_struct *vma);
int hugetlb_sysctl_handler(struct ctl_table *, int, void *, size_t *, loff_t *);
int hugetlb_overcommit_handler(struct ctl_table *, int, void *, size_t *,
		loff_t *);
int hugetlb_treat_movable_handler(struct ctl_table *, int, void *, size_t *,
		loff_t *);
int hugetlb_mempolicy_sysctl_handler(struct ctl_table *, int, void *, size_t *,
		loff_t *);

int copy_hugetlb_page_range(struct mm_struct *, struct mm_struct *, struct vm_area_struct *);
long follow_hugetlb_page(struct mm_struct *, struct vm_area_struct *,
			 struct page **, struct vm_area_struct **,
			 unsigned long *, unsigned long *, long, unsigned int,
			 int *);
void unmap_hugepage_range(struct vm_area_struct *,
			  unsigned long, unsigned long, struct page *);
void __unmap_hugepage_range_final(struct mmu_gather *tlb,
			  struct vm_area_struct *vma,
			  unsigned long start, unsigned long end,
			  struct page *ref_page);
void __unmap_hugepage_range(struct mmu_gather *tlb, struct vm_area_struct *vma,
				unsigned long start, unsigned long end,
				struct page *ref_page);
void hugetlb_report_meminfo(struct seq_file *);
int hugetlb_report_node_meminfo(char *buf, int len, int nid);
void hugetlb_show_meminfo(void);
unsigned long hugetlb_total_pages(void);
vm_fault_t hugetlb_fault(struct mm_struct *mm, struct vm_area_struct *vma,
			unsigned long address, unsigned int flags);
#ifdef CONFIG_USERFAULTFD
int hugetlb_mcopy_atomic_pte(struct mm_struct *dst_mm, pte_t *dst_pte,
				struct vm_area_struct *dst_vma,
				unsigned long dst_addr,
				unsigned long src_addr,
				enum mcopy_atomic_mode mode,
				struct page **pagep);
#endif /* CONFIG_USERFAULTFD */
bool hugetlb_reserve_pages(struct inode *inode, long from, long to,
						struct vm_area_struct *vma,
						vm_flags_t vm_flags);
long hugetlb_unreserve_pages(struct inode *inode, long start, long end,
						long freed);
bool isolate_huge_page(struct page *page, struct list_head *list);
int get_hwpoison_huge_page(struct page *page, bool *hugetlb);
void putback_active_hugepage(struct page *page);
void move_hugetlb_state(struct page *oldpage, struct page *newpage, int reason);
void free_huge_page(struct page *page);
void hugetlb_fix_reserve_counts(struct inode *inode);
extern struct mutex *hugetlb_fault_mutex_table;
u32 hugetlb_fault_mutex_hash(struct address_space *mapping, pgoff_t idx);

pte_t *huge_pmd_share(struct mm_struct *mm, struct vm_area_struct *vma,
		      unsigned long addr, pud_t *pud);

struct address_space *hugetlb_page_mapping_lock_write(struct page *hpage);

extern int sysctl_hugetlb_shm_group;
extern struct list_head huge_boot_pages;

/* arch callbacks */

pte_t *huge_pte_alloc(struct mm_struct *mm, struct vm_area_struct *vma,
			unsigned long addr, unsigned long sz);
pte_t *huge_pte_offset(struct mm_struct *mm,
		       unsigned long addr, unsigned long sz);
int huge_pmd_unshare(struct mm_struct *mm, struct vm_area_struct *vma,
				unsigned long *addr, pte_t *ptep);
void adjust_range_if_pmd_sharing_possible(struct vm_area_struct *vma,
				unsigned long *start, unsigned long *end);
struct page *follow_huge_addr(struct mm_struct *mm, unsigned long address,
			      int write);
struct page *follow_huge_pd(struct vm_area_struct *vma,
			    unsigned long address, hugepd_t hpd,
			    int flags, int pdshift);
struct page *follow_huge_pmd(struct mm_struct *mm, unsigned long address,
				pmd_t *pmd, int flags);
struct page *follow_huge_pud(struct mm_struct *mm, unsigned long address,
				pud_t *pud, int flags);
struct page *follow_huge_pgd(struct mm_struct *mm, unsigned long address,
			     pgd_t *pgd, int flags);

int pmd_huge(pmd_t pmd);
int pud_huge(pud_t pud);
unsigned long hugetlb_change_protection(struct vm_area_struct *vma,
		unsigned long address, unsigned long end, pgprot_t newprot);

bool is_hugetlb_entry_migration(pte_t pte);
void hugetlb_unshare_all_pmds(struct vm_area_struct *vma);

#else /* !CONFIG_HUGETLB_PAGE */

static inline void reset_vma_resv_huge_pages(struct vm_area_struct *vma)
{
}

static inline unsigned long hugetlb_total_pages(void)
{
	return 0;
}

static inline struct address_space *hugetlb_page_mapping_lock_write(
							struct page *hpage)
{
	return NULL;
}

static inline int huge_pmd_unshare(struct mm_struct *mm,
					struct vm_area_struct *vma,
					unsigned long *addr, pte_t *ptep)
{
	return 0;
}

static inline void adjust_range_if_pmd_sharing_possible(
				struct vm_area_struct *vma,
				unsigned long *start, unsigned long *end)
{
}

static inline long follow_hugetlb_page(struct mm_struct *mm,
			struct vm_area_struct *vma, struct page **pages,
			struct vm_area_struct **vmas, unsigned long *position,
			unsigned long *nr_pages, long i, unsigned int flags,
			int *nonblocking)
{
	BUG();
	return 0;
}

static inline struct page *follow_huge_addr(struct mm_struct *mm,
					unsigned long address, int write)
{
	return ERR_PTR(-EINVAL);
}

static inline int copy_hugetlb_page_range(struct mm_struct *dst,
			struct mm_struct *src, struct vm_area_struct *vma)
{
	BUG();
	return 0;
}

static inline void hugetlb_report_meminfo(struct seq_file *m)
{
}

static inline int hugetlb_report_node_meminfo(char *buf, int len, int nid)
{
	return 0;
}

static inline void hugetlb_show_meminfo(void)
{
}

static inline struct page *follow_huge_pd(struct vm_area_struct *vma,
				unsigned long address, hugepd_t hpd, int flags,
				int pdshift)
{
	return NULL;
}

static inline struct page *follow_huge_pmd(struct mm_struct *mm,
				unsigned long address, pmd_t *pmd, int flags)
{
	return NULL;
}

static inline struct page *follow_huge_pud(struct mm_struct *mm,
				unsigned long address, pud_t *pud, int flags)
{
	return NULL;
}

static inline struct page *follow_huge_pgd(struct mm_struct *mm,
				unsigned long address, pgd_t *pgd, int flags)
{
	return NULL;
}

static inline int prepare_hugepage_range(struct file *file,
				unsigned long addr, unsigned long len)
{
	return -EINVAL;
}

static inline int pmd_huge(pmd_t pmd)
{
	return 0;
}

static inline int pud_huge(pud_t pud)
{
	return 0;
}

static inline int is_hugepage_only_range(struct mm_struct *mm,
					unsigned long addr, unsigned long len)
{
	return 0;
}

static inline void hugetlb_free_pgd_range(struct mmu_gather *tlb,
				unsigned long addr, unsigned long end,
				unsigned long floor, unsigned long ceiling)
{
	BUG();
}

#ifdef CONFIG_USERFAULTFD
static inline int hugetlb_mcopy_atomic_pte(struct mm_struct *dst_mm,
						pte_t *dst_pte,
						struct vm_area_struct *dst_vma,
						unsigned long dst_addr,
						unsigned long src_addr,
						enum mcopy_atomic_mode mode,
						struct page **pagep)
{
	BUG();
	return 0;
}
#endif /* CONFIG_USERFAULTFD */

static inline pte_t *huge_pte_offset(struct mm_struct *mm, unsigned long addr,
					unsigned long sz)
{
	return NULL;
}

static inline bool isolate_huge_page(struct page *page, struct list_head *list)
{
	return false;
}

static inline int get_hwpoison_huge_page(struct page *page, bool *hugetlb)
{
	return 0;
}

static inline void putback_active_hugepage(struct page *page)
{
}

static inline void move_hugetlb_state(struct page *oldpage,
					struct page *newpage, int reason)
{
}

static inline unsigned long hugetlb_change_protection(
			struct vm_area_struct *vma, unsigned long address,
			unsigned long end, pgprot_t newprot)
{
	return 0;
}

static inline void __unmap_hugepage_range_final(struct mmu_gather *tlb,
			struct vm_area_struct *vma, unsigned long start,
			unsigned long end, struct page *ref_page)
{
	BUG();
}

static inline void __unmap_hugepage_range(struct mmu_gather *tlb,
			struct vm_area_struct *vma, unsigned long start,
			unsigned long end, struct page *ref_page)
{
	BUG();
}

static inline vm_fault_t hugetlb_fault(struct mm_struct *mm,
			struct vm_area_struct *vma, unsigned long address,
			unsigned int flags)
{
	BUG();
	return 0;
}

static inline void hugetlb_unshare_all_pmds(struct vm_area_struct *vma) { }

#endif /* !CONFIG_HUGETLB_PAGE */
/*
 * hugepages at page global directory. If arch support
 * hugepages at pgd level, they need to define this.
 */
#ifndef pgd_huge
#define pgd_huge(x)	0
#endif
#ifndef p4d_huge
#define p4d_huge(x)	0
#endif

#ifndef pgd_write
static inline int pgd_write(pgd_t pgd)
{
	BUG();
	return 0;
}
#endif

#define HUGETLB_ANON_FILE "anon_hugepage"

enum {
	/*
	 * The file will be used as an shm file so shmfs accounting rules
	 * apply
	 */
	HUGETLB_SHMFS_INODE     = 1,
	/*
	 * The file is being created on the internal vfs mount and shmfs
	 * accounting rules do not apply
	 */
	HUGETLB_ANONHUGE_INODE  = 2,
};

#ifdef CONFIG_HUGETLBFS
struct hugetlbfs_sb_info {
	long	max_inodes;   /* inodes allowed */
	long	free_inodes;  /* inodes free */
	spinlock_t	stat_lock;
	struct hstate *hstate;
	struct hugepage_subpool *spool;
	kuid_t	uid;
	kgid_t	gid;
	umode_t mode;
};

static inline struct hugetlbfs_sb_info *HUGETLBFS_SB(struct super_block *sb)
{
	return sb->s_fs_info;
}

struct hugetlbfs_inode_info {
	struct shared_policy policy;
	struct inode vfs_inode;
	unsigned int seals;
};

static inline struct hugetlbfs_inode_info *HUGETLBFS_I(struct inode *inode)
{
	return container_of(inode, struct hugetlbfs_inode_info, vfs_inode);
}

extern const struct file_operations hugetlbfs_file_operations;
extern const struct vm_operations_struct hugetlb_vm_ops;
struct file *hugetlb_file_setup(const char *name, size_t size, vm_flags_t acct,
				struct user_struct **user, int creat_flags,
				int page_size_log);

static inline bool is_file_hugepages(struct file *file)
{
	if (file->f_op == &hugetlbfs_file_operations)
		return true;

	return is_file_shm_hugepages(file);
}

static inline struct hstate *hstate_inode(struct inode *i)
{
	return HUGETLBFS_SB(i->i_sb)->hstate;
}
#else /* !CONFIG_HUGETLBFS */

#define is_file_hugepages(file)			false
static inline struct file *
hugetlb_file_setup(const char *name, size_t size, vm_flags_t acctflag,
		struct user_struct **user, int creat_flags,
		int page_size_log)
{
	return ERR_PTR(-ENOSYS);
}

static inline struct hstate *hstate_inode(struct inode *i)
{
	return NULL;
}
#endif /* !CONFIG_HUGETLBFS */

#ifdef HAVE_ARCH_HUGETLB_UNMAPPED_AREA
unsigned long hugetlb_get_unmapped_area(struct file *file, unsigned long addr,
					unsigned long len, unsigned long pgoff,
					unsigned long flags);
#endif /* HAVE_ARCH_HUGETLB_UNMAPPED_AREA */

/*
 * huegtlb page specific state flags.  These flags are located in page.private
 * of the hugetlb head page.  Functions created via the below macros should be
 * used to manipulate these flags.
 *
 * HPG_restore_reserve - Set when a hugetlb page consumes a reservation at
 *	allocation time.  Cleared when page is fully instantiated.  Free
 *	routine checks flag to restore a reservation on error paths.
 *	Synchronization:  Examined or modified by code that knows it has
 *	the only reference to page.  i.e. After allocation but before use
 *	or when the page is being freed.
 * HPG_migratable  - Set after a newly allocated page is added to the page
 *	cache and/or page tables.  Indicates the page is a candidate for
 *	migration.
 *	Synchronization:  Initially set after new page allocation with no
 *	locking.  When examined and modified during migration processing
 *	(isolate, migrate, putback) the hugetlb_lock is held.
 * HPG_temporary - - Set on a page that is temporarily allocated from the buddy
 *	allocator.  Typically used for migration target pages when no pages
 *	are available in the pool.  The hugetlb free page path will
 *	immediately free pages with this flag set to the buddy allocator.
 *	Synchronization: Can be set after huge page allocation from buddy when
 *	code knows it has only reference.  All other examinations and
 *	modifications require hugetlb_lock.
 * HPG_freed - Set when page is on the free lists.
 *	Synchronization: hugetlb_lock held for examination and modification.
 */
enum hugetlb_page_flags {
	HPG_restore_reserve = 0,
	HPG_migratable,
	HPG_temporary,
	HPG_freed,
	__NR_HPAGEFLAGS,
};

/*
 * Macros to create test, set and clear function definitions for
 * hugetlb specific page flags.
 */
#ifdef CONFIG_HUGETLB_PAGE
#define TESTHPAGEFLAG(uname, flname)				\
static inline int HPage##uname(struct page *page)		\
	{ return test_bit(HPG_##flname, &(page->private)); }

#define SETHPAGEFLAG(uname, flname)				\
static inline void SetHPage##uname(struct page *page)		\
	{ set_bit(HPG_##flname, &(page->private)); }

#define CLEARHPAGEFLAG(uname, flname)				\
static inline void ClearHPage##uname(struct page *page)		\
	{ clear_bit(HPG_##flname, &(page->private)); }
#else
#define TESTHPAGEFLAG(uname, flname)				\
static inline int HPage##uname(struct page *page)		\
	{ return 0; }

#define SETHPAGEFLAG(uname, flname)				\
static inline void SetHPage##uname(struct page *page)		\
	{ }

#define CLEARHPAGEFLAG(uname, flname)				\
static inline void ClearHPage##uname(struct page *page)		\
	{ }
#endif

#define HPAGEFLAG(uname, flname)				\
	TESTHPAGEFLAG(uname, flname)				\
	SETHPAGEFLAG(uname, flname)				\
	CLEARHPAGEFLAG(uname, flname)				\

/*
 * Create functions associated with hugetlb page flags
 */
HPAGEFLAG(RestoreReserve, restore_reserve)
HPAGEFLAG(Migratable, migratable)
HPAGEFLAG(Temporary, temporary)
HPAGEFLAG(Freed, freed)

#ifdef CONFIG_HUGETLB_PAGE

#define HSTATE_NAME_LEN 32
/* Defines one hugetlb page size */
struct hstate {
	struct mutex resize_lock;
	int next_nid_to_alloc;
	int next_nid_to_free;
	unsigned int order;
	unsigned long mask;
	unsigned long max_huge_pages;
	unsigned long nr_huge_pages;
	unsigned long free_huge_pages;
	unsigned long resv_huge_pages;
	unsigned long surplus_huge_pages;
	unsigned long nr_overcommit_huge_pages;
	struct list_head hugepage_activelist;
	struct list_head hugepage_freelists[MAX_NUMNODES];
	unsigned int nr_huge_pages_node[MAX_NUMNODES];
	unsigned int free_huge_pages_node[MAX_NUMNODES];
	unsigned int surplus_huge_pages_node[MAX_NUMNODES];
#ifdef CONFIG_CGROUP_HUGETLB
	/* cgroup control files */
	struct cftype cgroup_files_dfl[7];
	struct cftype cgroup_files_legacy[9];
#endif
	char name[HSTATE_NAME_LEN];
};

struct huge_bootmem_page {
	struct list_head list;
	struct hstate *hstate;
};

int isolate_or_dissolve_huge_page(struct page *page, struct list_head *list);
struct page *alloc_huge_page(struct vm_area_struct *vma,
				unsigned long addr, int avoid_reserve);
struct page *alloc_huge_page_nodemask(struct hstate *h, int preferred_nid,
				nodemask_t *nmask, gfp_t gfp_mask);
struct page *alloc_huge_page_vma(struct hstate *h, struct vm_area_struct *vma,
				unsigned long address);
int huge_add_to_page_cache(struct page *page, struct address_space *mapping,
			pgoff_t idx);
void restore_reserve_on_error(struct hstate *h, struct vm_area_struct *vma,
				unsigned long address, struct page *page);

/* arch callback */
int __init __alloc_bootmem_huge_page(struct hstate *h);
int __init alloc_bootmem_huge_page(struct hstate *h);

void __init hugetlb_add_hstate(unsigned order);
bool __init arch_hugetlb_valid_size(unsigned long size);
struct hstate *size_to_hstate(unsigned long size);

#ifndef HUGE_MAX_HSTATE
#define HUGE_MAX_HSTATE 1
#endif

extern struct hstate hstates[HUGE_MAX_HSTATE];
extern unsigned int default_hstate_idx;

#define default_hstate (hstates[default_hstate_idx])

/*
 * hugetlb page subpool pointer located in hpage[1].private
 */
static inline struct hugepage_subpool *hugetlb_page_subpool(struct page *hpage)
{
	return (struct hugepage_subpool *)(hpage+1)->private;
}

static inline void hugetlb_set_page_subpool(struct page *hpage,
					struct hugepage_subpool *subpool)
{
	set_page_private(hpage+1, (unsigned long)subpool);
}

static inline struct hstate *hstate_file(struct file *f)
{
	return hstate_inode(file_inode(f));
}

static inline struct hstate *hstate_sizelog(int page_size_log)
{
	if (!page_size_log)
		return &default_hstate;

	return size_to_hstate(1UL << page_size_log);
}

static inline struct hstate *hstate_vma(struct vm_area_struct *vma)
{
	return hstate_file(vma->vm_file);
}

static inline unsigned long huge_page_size(struct hstate *h)
{
	return (unsigned long)PAGE_SIZE << h->order;
}

extern unsigned long vma_kernel_pagesize(struct vm_area_struct *vma);

extern unsigned long vma_mmu_pagesize(struct vm_area_struct *vma);

static inline unsigned long huge_page_mask(struct hstate *h)
{
	return h->mask;
}

static inline unsigned int huge_page_order(struct hstate *h)
{
	return h->order;
}

static inline unsigned huge_page_shift(struct hstate *h)
{
	return h->order + PAGE_SHIFT;
}

static inline bool hstate_is_gigantic(struct hstate *h)
{
	return huge_page_order(h) >= MAX_ORDER;
}

static inline unsigned int pages_per_huge_page(struct hstate *h)
{
	return 1 << h->order;
}

static inline unsigned int blocks_per_huge_page(struct hstate *h)
{
	return huge_page_size(h) / 512;
}

#include <asm/hugetlb.h>

#ifndef is_hugepage_only_range
static inline int is_hugepage_only_range(struct mm_struct *mm,
					unsigned long addr, unsigned long len)
{
	return 0;
}
#define is_hugepage_only_range is_hugepage_only_range
#endif

#ifndef arch_clear_hugepage_flags
static inline void arch_clear_hugepage_flags(struct page *page) { }
#define arch_clear_hugepage_flags arch_clear_hugepage_flags
#endif

#ifndef arch_make_huge_pte
static inline pte_t arch_make_huge_pte(pte_t entry, struct vm_area_struct *vma,
				       struct page *page, int writable)
{
	return entry;
}
#endif

static inline struct hstate *page_hstate(struct page *page)
{
	VM_BUG_ON_PAGE(!PageHuge(page), page);
	return size_to_hstate(page_size(page));
}

static inline unsigned hstate_index_to_shift(unsigned index)
{
	return hstates[index].order + PAGE_SHIFT;
}

static inline int hstate_index(struct hstate *h)
{
	return h - hstates;
}

extern int dissolve_free_huge_page(struct page *page);
extern int dissolve_free_huge_pages(unsigned long start_pfn,
				    unsigned long end_pfn);

#ifdef CONFIG_ARCH_ENABLE_HUGEPAGE_MIGRATION
#ifndef arch_hugetlb_migration_supported
static inline bool arch_hugetlb_migration_supported(struct hstate *h)
{
	if ((huge_page_shift(h) == PMD_SHIFT) ||
		(huge_page_shift(h) == PUD_SHIFT) ||
			(huge_page_shift(h) == PGDIR_SHIFT))
		return true;
	else
		return false;
}
#endif
#else
static inline bool arch_hugetlb_migration_supported(struct hstate *h)
{
	return false;
}
#endif

static inline bool hugepage_migration_supported(struct hstate *h)
{
	return arch_hugetlb_migration_supported(h);
}

/*
 * Movability check is different as compared to migration check.
 * It determines whether or not a huge page should be placed on
 * movable zone or not. Movability of any huge page should be
 * required only if huge page size is supported for migration.
 * There wont be any reason for the huge page to be movable if
 * it is not migratable to start with. Also the size of the huge
 * page should be large enough to be placed under a movable zone
 * and still feasible enough to be migratable. Just the presence
 * in movable zone does not make the migration feasible.
 *
 * So even though large huge page sizes like the gigantic ones
 * are migratable they should not be movable because its not
 * feasible to migrate them from movable zone.
 */
static inline bool hugepage_movable_supported(struct hstate *h)
{
	if (!hugepage_migration_supported(h))
		return false;

	if (hstate_is_gigantic(h))
		return false;
	return true;
}

/* Movability of hugepages depends on migration support. */
static inline gfp_t htlb_alloc_mask(struct hstate *h)
{
	if (hugepage_movable_supported(h))
		return GFP_HIGHUSER_MOVABLE;
	else
		return GFP_HIGHUSER;
}

static inline gfp_t htlb_modify_alloc_mask(struct hstate *h, gfp_t gfp_mask)
{
	gfp_t modified_mask = htlb_alloc_mask(h);

	/* Some callers might want to enforce node */
	modified_mask |= (gfp_mask & __GFP_THISNODE);

	modified_mask |= (gfp_mask & __GFP_NOWARN);

	return modified_mask;
}

static inline spinlock_t *huge_pte_lockptr(struct hstate *h,
					   struct mm_struct *mm, pte_t *pte)
{
	if (huge_page_size(h) == PMD_SIZE)
		return pmd_lockptr(mm, (pmd_t *) pte);
	VM_BUG_ON(huge_page_size(h) == PAGE_SIZE);
	return &mm->page_table_lock;
}

#ifndef hugepages_supported
/*
 * Some platform decide whether they support huge pages at boot
 * time. Some of them, such as powerpc, set HPAGE_SHIFT to 0
 * when there is no such support
 */
#define hugepages_supported() (HPAGE_SHIFT != 0)
#endif

void hugetlb_report_usage(struct seq_file *m, struct mm_struct *mm);

static inline void hugetlb_count_add(long l, struct mm_struct *mm)
{
	atomic_long_add(l, &mm->hugetlb_usage);
}

static inline void hugetlb_count_sub(long l, struct mm_struct *mm)
{
	atomic_long_sub(l, &mm->hugetlb_usage);
}

#ifndef set_huge_swap_pte_at
static inline void set_huge_swap_pte_at(struct mm_struct *mm, unsigned long addr,
					pte_t *ptep, pte_t pte, unsigned long sz)
{
	set_huge_pte_at(mm, addr, ptep, pte);
}
#endif

#ifndef huge_ptep_modify_prot_start
#define huge_ptep_modify_prot_start huge_ptep_modify_prot_start
static inline pte_t huge_ptep_modify_prot_start(struct vm_area_struct *vma,
						unsigned long addr, pte_t *ptep)
{
	return huge_ptep_get_and_clear(vma->vm_mm, addr, ptep);
}
#endif

#ifndef huge_ptep_modify_prot_commit
#define huge_ptep_modify_prot_commit huge_ptep_modify_prot_commit
static inline void huge_ptep_modify_prot_commit(struct vm_area_struct *vma,
						unsigned long addr, pte_t *ptep,
						pte_t old_pte, pte_t pte)
{
	set_huge_pte_at(vma->vm_mm, addr, ptep, pte);
}
#endif

#else	/* CONFIG_HUGETLB_PAGE */
struct hstate {};

static inline int isolate_or_dissolve_huge_page(struct page *page,
						struct list_head *list)
{
	return -ENOMEM;
}

static inline struct page *alloc_huge_page(struct vm_area_struct *vma,
					   unsigned long addr,
					   int avoid_reserve)
{
	return NULL;
}

static inline struct page *
alloc_huge_page_nodemask(struct hstate *h, int preferred_nid,
			nodemask_t *nmask, gfp_t gfp_mask)
{
	return NULL;
}

static inline struct page *alloc_huge_page_vma(struct hstate *h,
					       struct vm_area_struct *vma,
					       unsigned long address)
{
	return NULL;
}

static inline int __alloc_bootmem_huge_page(struct hstate *h)
{
	return 0;
}

static inline struct hstate *hstate_file(struct file *f)
{
	return NULL;
}

static inline struct hstate *hstate_sizelog(int page_size_log)
{
	return NULL;
}

static inline struct hstate *hstate_vma(struct vm_area_struct *vma)
{
	return NULL;
}

static inline struct hstate *page_hstate(struct page *page)
{
	return NULL;
}

static inline unsigned long huge_page_size(struct hstate *h)
{
	return PAGE_SIZE;
}

static inline unsigned long huge_page_mask(struct hstate *h)
{
	return PAGE_MASK;
}

static inline unsigned long vma_kernel_pagesize(struct vm_area_struct *vma)
{
	return PAGE_SIZE;
}

static inline unsigned long vma_mmu_pagesize(struct vm_area_struct *vma)
{
	return PAGE_SIZE;
}

static inline unsigned int huge_page_order(struct hstate *h)
{
	return 0;
}

static inline unsigned int huge_page_shift(struct hstate *h)
{
	return PAGE_SHIFT;
}

static inline bool hstate_is_gigantic(struct hstate *h)
{
	return false;
}

static inline unsigned int pages_per_huge_page(struct hstate *h)
{
	return 1;
}

static inline unsigned hstate_index_to_shift(unsigned index)
{
	return 0;
}

static inline int hstate_index(struct hstate *h)
{
	return 0;
}

static inline int dissolve_free_huge_page(struct page *page)
{
	return 0;
}

static inline int dissolve_free_huge_pages(unsigned long start_pfn,
					   unsigned long end_pfn)
{
	return 0;
}

static inline bool hugepage_migration_supported(struct hstate *h)
{
	return false;
}

static inline bool hugepage_movable_supported(struct hstate *h)
{
	return false;
}

static inline gfp_t htlb_alloc_mask(struct hstate *h)
{
	return 0;
}

static inline gfp_t htlb_modify_alloc_mask(struct hstate *h, gfp_t gfp_mask)
{
	return 0;
}

static inline spinlock_t *huge_pte_lockptr(struct hstate *h,
					   struct mm_struct *mm, pte_t *pte)
{
	return &mm->page_table_lock;
}

static inline void hugetlb_report_usage(struct seq_file *f, struct mm_struct *m)
{
}

static inline void hugetlb_count_sub(long l, struct mm_struct *mm)
{
}

static inline void set_huge_swap_pte_at(struct mm_struct *mm, unsigned long addr,
					pte_t *ptep, pte_t pte, unsigned long sz)
{
}
#endif	/* CONFIG_HUGETLB_PAGE */

static inline spinlock_t *huge_pte_lock(struct hstate *h,
					struct mm_struct *mm, pte_t *pte)
{
	spinlock_t *ptl;

	ptl = huge_pte_lockptr(h, mm, pte);
	spin_lock(ptl);
	return ptl;
}

#if defined(CONFIG_HUGETLB_PAGE) && defined(CONFIG_CMA)
extern void __init hugetlb_cma_reserve(int order);
extern void __init hugetlb_cma_check(void);
#else
static inline __init void hugetlb_cma_reserve(int order)
{
}
static inline __init void hugetlb_cma_check(void)
{
}
#endif

bool want_pmd_share(struct vm_area_struct *vma, unsigned long addr);

#ifndef __HAVE_ARCH_FLUSH_HUGETLB_TLB_RANGE
/*
 * ARCHes with special requirements for evicting HUGETLB backing TLB entries can
 * implement this.
 */
#define flush_hugetlb_tlb_range(vma, addr, end)	flush_tlb_range(vma, addr, end)
#endif

#endif /* _LINUX_HUGETLB_H */
