// SPDX-License-Identifier: GPL-2.0-only
/*
 * z3fold.c
 *
 * Author: Vitaly Wool <vitaly.wool@konsulko.com>
 * Copyright (C) 2016, Sony Mobile Communications Inc.
 *
 * This implementation is based on zbud written by Seth Jennings.
 *
 * z3fold is an special purpose allocator for storing compressed pages. It
 * can store up to three compressed pages per page which improves the
 * compression ratio of zbud while retaining its main concepts (e. g. always
 * storing an integral number of objects per page) and simplicity.
 * It still has simple and deterministic reclaim properties that make it
 * preferable to a higher density approach (with no requirement on integral
 * number of object per page) when reclaim is used.
 *
 * As in zbud, pages are divided into "chunks".  The size of the chunks is
 * fixed at compile time and is determined by NCHUNKS_ORDER below.
 *
 * z3fold doesn't export any API and is meant to be used via zpool API.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/atomic.h>
#include <linux/sched.h>
#include <linux/cpumask.h>
#include <linux/list.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/page-flags.h>
#include <linux/migrate.h>
#include <linux/node.h>
#include <linux/compaction.h>
#include <linux/percpu.h>
#include <linux/mount.h>
#include <linux/pseudo_fs.h>
#include <linux/fs.h>
#include <linux/preempt.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/zpool.h>
#include <linux/magic.h>
#include <linux/kmemleak.h>

/*
 * NCHUNKS_ORDER determines the internal allocation granularity, effectively
 * adjusting internal fragmentation.  It also determines the number of
 * freelists maintained in each pool. NCHUNKS_ORDER of 6 means that the
 * allocation granularity will be in chunks of size PAGE_SIZE/64. Some chunks
 * in the beginning of an allocated page are occupied by z3fold header, so
 * NCHUNKS will be calculated to 63 (or 62 in case CONFIG_DEBUG_SPINLOCK=y),
 * which shows the max number of free chunks in z3fold page, also there will
 * be 63, or 62, respectively, freelists per pool.
 */
#define NCHUNKS_ORDER	6

#define CHUNK_SHIFT	(PAGE_SHIFT - NCHUNKS_ORDER)
#define CHUNK_SIZE	(1 << CHUNK_SHIFT)
#define ZHDR_SIZE_ALIGNED round_up(sizeof(struct z3fold_header), CHUNK_SIZE)
#define ZHDR_CHUNKS	(ZHDR_SIZE_ALIGNED >> CHUNK_SHIFT)
#define TOTAL_CHUNKS	(PAGE_SIZE >> CHUNK_SHIFT)
#define NCHUNKS		((PAGE_SIZE - ZHDR_SIZE_ALIGNED) >> CHUNK_SHIFT)

#define BUDDY_MASK	(0x3)
#define BUDDY_SHIFT	2
#define SLOTS_ALIGN	(0x40)

/*****************
 * Structures
*****************/
struct z3fold_pool;
struct z3fold_ops {
	int (*evict)(struct z3fold_pool *pool, unsigned long handle);
};

enum buddy {
	HEADLESS = 0,
	FIRST,
	MIDDLE,
	LAST,
	BUDDIES_MAX = LAST
};

struct z3fold_buddy_slots {
	/*
	 * we are using BUDDY_MASK in handle_to_buddy etc. so there should
	 * be enough slots to hold all possible variants
	 */
	unsigned long slot[BUDDY_MASK + 1];
	unsigned long pool; /* back link */
	rwlock_t lock;
};
#define HANDLE_FLAG_MASK	(0x03)

/*
 * struct z3fold_header - z3fold page metadata occupying first chunks of each
 *			z3fold page, except for HEADLESS pages
 * @buddy:		links the z3fold page into the relevant list in the
 *			pool
 * @page_lock:		per-page lock
 * @refcount:		reference count for the z3fold page
 * @work:		work_struct for page layout optimization
 * @slots:		pointer to the structure holding buddy slots
 * @pool:		pointer to the containing pool
 * @cpu:		CPU which this page "belongs" to
 * @first_chunks:	the size of the first buddy in chunks, 0 if free
 * @middle_chunks:	the size of the middle buddy in chunks, 0 if free
 * @last_chunks:	the size of the last buddy in chunks, 0 if free
 * @first_num:		the starting number (for the first handle)
 * @mapped_count:	the number of objects currently mapped
 */
struct z3fold_header {
	struct list_head buddy;
	spinlock_t page_lock;
	struct kref refcount;
	struct work_struct work;
	struct z3fold_buddy_slots *slots;
	struct z3fold_pool *pool;
	short cpu;
	unsigned short first_chunks;
	unsigned short middle_chunks;
	unsigned short last_chunks;
	unsigned short start_middle;
	unsigned short first_num:2;
	unsigned short mapped_count:2;
	unsigned short foreign_handles:2;
};

/**
 * struct z3fold_pool - stores metadata for each z3fold pool
 * @name:	pool name
 * @lock:	protects pool unbuddied/lru lists
 * @stale_lock:	protects pool stale page list
 * @unbuddied:	per-cpu array of lists tracking z3fold pages that contain 2-
 *		buddies; the list each z3fold page is added to depends on
 *		the size of its free region.
 * @lru:	list tracking the z3fold pages in LRU order by most recently
 *		added buddy.
 * @stale:	list of pages marked for freeing
 * @pages_nr:	number of z3fold pages in the pool.
 * @c_handle:	cache for z3fold_buddy_slots allocation
 * @ops:	pointer to a structure of user defined operations specified at
 *		pool creation time.
 * @compact_wq:	workqueue for page layout background optimization
 * @release_wq:	workqueue for safe page release
 * @work:	work_struct for safe page release
 * @inode:	inode for z3fold pseudo filesystem
 *
 * This structure is allocated at pool creation time and maintains metadata
 * pertaining to a particular z3fold pool.
 */
struct z3fold_pool {
	const char *name;
	spinlock_t lock;
	spinlock_t stale_lock;
	struct list_head *unbuddied;
	struct list_head lru;
	struct list_head stale;
	atomic64_t pages_nr;
	struct kmem_cache *c_handle;
	const struct z3fold_ops *ops;
	struct zpool *zpool;
	const struct zpool_ops *zpool_ops;
	struct workqueue_struct *compact_wq;
	struct workqueue_struct *release_wq;
	struct work_struct work;
	struct inode *inode;
};

/*
 * Internal z3fold page flags
 */
enum z3fold_page_flags {
	PAGE_HEADLESS = 0,
	MIDDLE_CHUNK_MAPPED,
	NEEDS_COMPACTING,
	PAGE_STALE,
	PAGE_CLAIMED, /* by either reclaim or free */
};

/*
 * handle flags, go under HANDLE_FLAG_MASK
 */
enum z3fold_handle_flags {
	HANDLES_NOFREE = 0,
};

/*
 * Forward declarations
 */
static struct z3fold_header *__z3fold_alloc(struct z3fold_pool *, size_t, bool);
static void compact_page_work(struct work_struct *w);

/*****************
 * Helpers
*****************/

/* Converts an allocation size in bytes to size in z3fold chunks */
static int size_to_chunks(size_t size)
{
	return (size + CHUNK_SIZE - 1) >> CHUNK_SHIFT;
}

#define for_each_unbuddied_list(_iter, _begin) \
	for ((_iter) = (_begin); (_iter) < NCHUNKS; (_iter)++)

static inline struct z3fold_buddy_slots *alloc_slots(struct z3fold_pool *pool,
							gfp_t gfp)
{
	struct z3fold_buddy_slots *slots;

	slots = kmem_cache_zalloc(pool->c_handle,
				 (gfp & ~(__GFP_HIGHMEM | __GFP_MOVABLE)));

	if (slots) {
		/* It will be freed separately in free_handle(). */
		kmemleak_not_leak(slots);
		slots->pool = (unsigned long)pool;
		rwlock_init(&slots->lock);
	}

	return slots;
}

static inline struct z3fold_pool *slots_to_pool(struct z3fold_buddy_slots *s)
{
	return (struct z3fold_pool *)(s->pool & ~HANDLE_FLAG_MASK);
}

static inline struct z3fold_buddy_slots *handle_to_slots(unsigned long handle)
{
	return (struct z3fold_buddy_slots *)(handle & ~(SLOTS_ALIGN - 1));
}

/* Lock a z3fold page */
static inline void z3fold_page_lock(struct z3fold_header *zhdr)
{
	spin_lock(&zhdr->page_lock);
}

/* Try to lock a z3fold page */
static inline int z3fold_page_trylock(struct z3fold_header *zhdr)
{
	return spin_trylock(&zhdr->page_lock);
}

/* Unlock a z3fold page */
static inline void z3fold_page_unlock(struct z3fold_header *zhdr)
{
	spin_unlock(&zhdr->page_lock);
}


static inline struct z3fold_header *__get_z3fold_header(unsigned long handle,
							bool lock)
{
	struct z3fold_buddy_slots *slots;
	struct z3fold_header *zhdr;
	int locked = 0;

	if (!(handle & (1 << PAGE_HEADLESS))) {
		slots = handle_to_slots(handle);
		do {
			unsigned long addr;

			read_lock(&slots->lock);
			addr = *(unsigned long *)handle;
			zhdr = (struct z3fold_header *)(addr & PAGE_MASK);
			if (lock)
				locked = z3fold_page_trylock(zhdr);
			read_unlock(&slots->lock);
			if (locked)
				break;
			cpu_relax();
		} while (lock);
	} else {
		zhdr = (struct z3fold_header *)(handle & PAGE_MASK);
	}

	return zhdr;
}

/* Returns the z3fold page where a given handle is stored */
static inline struct z3fold_header *handle_to_z3fold_header(unsigned long h)
{
	return __get_z3fold_header(h, false);
}

/* return locked z3fold page if it's not headless */
static inline struct z3fold_header *get_z3fold_header(unsigned long h)
{
	return __get_z3fold_header(h, true);
}

static inline void put_z3fold_header(struct z3fold_header *zhdr)
{
	struct page *page = virt_to_page(zhdr);

	if (!test_bit(PAGE_HEADLESS, &page->private))
		z3fold_page_unlock(zhdr);
}

static inline void free_handle(unsigned long handle, struct z3fold_header *zhdr)
{
	struct z3fold_buddy_slots *slots;
	int i;
	bool is_free;

	if (handle & (1 << PAGE_HEADLESS))
		return;

	if (WARN_ON(*(unsigned long *)handle == 0))
		return;

	slots = handle_to_slots(handle);
	write_lock(&slots->lock);
	*(unsigned long *)handle = 0;

	if (test_bit(HANDLES_NOFREE, &slots->pool)) {
		write_unlock(&slots->lock);
		return; /* simple case, nothing else to do */
	}

	if (zhdr->slots != slots)
		zhdr->foreign_handles--;

	is_free = true;
	for (i = 0; i <= BUDDY_MASK; i++) {
		if (slots->slot[i]) {
			is_free = false;
			break;
		}
	}
	write_unlock(&slots->lock);

	if (is_free) {
		struct z3fold_pool *pool = slots_to_pool(slots);

		if (zhdr->slots == slots)
			zhdr->slots = NULL;
		kmem_cache_free(pool->c_handle, slots);
	}
}

static int z3fold_init_fs_context(struct fs_context *fc)
{
	return init_pseudo(fc, Z3FOLD_MAGIC) ? 0 : -ENOMEM;
}

static struct file_system_type z3fold_fs = {
	.name		= "z3fold",
	.init_fs_context = z3fold_init_fs_context,
	.kill_sb	= kill_anon_super,
};

static struct vfsmount *z3fold_mnt;
static int z3fold_mount(void)
{
	int ret = 0;

	z3fold_mnt = kern_mount(&z3fold_fs);
	if (IS_ERR(z3fold_mnt))
		ret = PTR_ERR(z3fold_mnt);

	return ret;
}

static void z3fold_unmount(void)
{
	kern_unmount(z3fold_mnt);
}

static const struct address_space_operations z3fold_aops;
static int z3fold_register_migration(struct z3fold_pool *pool)
{
	pool->inode = alloc_anon_inode(z3fold_mnt->mnt_sb);
	if (IS_ERR(pool->inode)) {
		pool->inode = NULL;
		return 1;
	}

	pool->inode->i_mapping->private_data = pool;
	pool->inode->i_mapping->a_ops = &z3fold_aops;
	return 0;
}

static void z3fold_unregister_migration(struct z3fold_pool *pool)
{
	if (pool->inode)
		iput(pool->inode);
}

/* Initializes the z3fold header of a newly allocated z3fold page */
static struct z3fold_header *init_z3fold_page(struct page *page, bool headless,
					struct z3fold_pool *pool, gfp_t gfp)
{
	struct z3fold_header *zhdr = page_address(page);
	struct z3fold_buddy_slots *slots;

	INIT_LIST_HEAD(&page->lru);
	clear_bit(PAGE_HEADLESS, &page->private);
	clear_bit(MIDDLE_CHUNK_MAPPED, &page->private);
	clear_bit(NEEDS_COMPACTING, &page->private);
	clear_bit(PAGE_STALE, &page->private);
	clear_bit(PAGE_CLAIMED, &page->private);
	if (headless)
		return zhdr;

	slots = alloc_slots(pool, gfp);
	if (!slots)
		return NULL;

	memset(zhdr, 0, sizeof(*zhdr));
	spin_lock_init(&zhdr->page_lock);
	kref_init(&zhdr->refcount);
	zhdr->cpu = -1;
	zhdr->slots = slots;
	zhdr->pool = pool;
	INIT_LIST_HEAD(&zhdr->buddy);
	INIT_WORK(&zhdr->work, compact_page_work);
	return zhdr;
}

/* Resets the struct page fields and frees the page */
static void free_z3fold_page(struct page *page, bool headless)
{
	if (!headless) {
		lock_page(page);
		__ClearPageMovable(page);
		unlock_page(page);
	}
	ClearPagePrivate(page);
	__free_page(page);
}

/* Helper function to build the index */
static inline int __idx(struct z3fold_header *zhdr, enum buddy bud)
{
	return (bud + zhdr->first_num) & BUDDY_MASK;
}

/*
 * Encodes the handle of a particular buddy within a z3fold page
 * Pool lock should be held as this function accesses first_num
 */
static unsigned long __encode_handle(struct z3fold_header *zhdr,
				struct z3fold_buddy_slots *slots,
				enum buddy bud)
{
	unsigned long h = (unsigned long)zhdr;
	int idx = 0;

	/*
	 * For a headless page, its handle is its pointer with the extra
	 * PAGE_HEADLESS bit set
	 */
	if (bud == HEADLESS)
		return h | (1 << PAGE_HEADLESS);

	/* otherwise, return pointer to encoded handle */
	idx = __idx(zhdr, bud);
	h += idx;
	if (bud == LAST)
		h |= (zhdr->last_chunks << BUDDY_SHIFT);

	write_lock(&slots->lock);
	slots->slot[idx] = h;
	write_unlock(&slots->lock);
	return (unsigned long)&slots->slot[idx];
}

static unsigned long encode_handle(struct z3fold_header *zhdr, enum buddy bud)
{
	return __encode_handle(zhdr, zhdr->slots, bud);
}

/* only for LAST bud, returns zero otherwise */
static unsigned short handle_to_chunks(unsigned long handle)
{
	struct z3fold_buddy_slots *slots = handle_to_slots(handle);
	unsigned long addr;

	read_lock(&slots->lock);
	addr = *(unsigned long *)handle;
	read_unlock(&slots->lock);
	return (addr & ~PAGE_MASK) >> BUDDY_SHIFT;
}

/*
 * (handle & BUDDY_MASK) < zhdr->first_num is possible in encode_handle
 *  but that doesn't matter. because the masking will result in the
 *  correct buddy number.
 */
static enum buddy handle_to_buddy(unsigned long handle)
{
	struct z3fold_header *zhdr;
	struct z3fold_buddy_slots *slots = handle_to_slots(handle);
	unsigned long addr;

	read_lock(&slots->lock);
	WARN_ON(handle & (1 << PAGE_HEADLESS));
	addr = *(unsigned long *)handle;
	read_unlock(&slots->lock);
	zhdr = (struct z3fold_header *)(addr & PAGE_MASK);
	return (addr - zhdr->first_num) & BUDDY_MASK;
}

static inline struct z3fold_pool *zhdr_to_pool(struct z3fold_header *zhdr)
{
	return zhdr->pool;
}

static void __release_z3fold_page(struct z3fold_header *zhdr, bool locked)
{
	struct page *page = virt_to_page(zhdr);
	struct z3fold_pool *pool = zhdr_to_pool(zhdr);

	WARN_ON(!list_empty(&zhdr->buddy));
	set_bit(PAGE_STALE, &page->private);
	clear_bit(NEEDS_COMPACTING, &page->private);
	spin_lock(&pool->lock);
	if (!list_empty(&page->lru))
		list_del_init(&page->lru);
	spin_unlock(&pool->lock);

	if (locked)
		z3fold_page_unlock(zhdr);

	spin_lock(&pool->stale_lock);
	list_add(&zhdr->buddy, &pool->stale);
	queue_work(pool->release_wq, &pool->work);
	spin_unlock(&pool->stale_lock);
}

static void release_z3fold_page(struct kref *ref)
{
	struct z3fold_header *zhdr = container_of(ref, struct z3fold_header,
						refcount);
	__release_z3fold_page(zhdr, false);
}

static void release_z3fold_page_locked(struct kref *ref)
{
	struct z3fold_header *zhdr = container_of(ref, struct z3fold_header,
						refcount);
	WARN_ON(z3fold_page_trylock(zhdr));
	__release_z3fold_page(zhdr, true);
}

static void release_z3fold_page_locked_list(struct kref *ref)
{
	struct z3fold_header *zhdr = container_of(ref, struct z3fold_header,
					       refcount);
	struct z3fold_pool *pool = zhdr_to_pool(zhdr);

	spin_lock(&pool->lock);
	list_del_init(&zhdr->buddy);
	spin_unlock(&pool->lock);

	WARN_ON(z3fold_page_trylock(zhdr));
	__release_z3fold_page(zhdr, true);
}

static void free_pages_work(struct work_struct *w)
{
	struct z3fold_pool *pool = container_of(w, struct z3fold_pool, work);

	spin_lock(&pool->stale_lock);
	while (!list_empty(&pool->stale)) {
		struct z3fold_header *zhdr = list_first_entry(&pool->stale,
						struct z3fold_header, buddy);
		struct page *page = virt_to_page(zhdr);

		list_del(&zhdr->buddy);
		if (WARN_ON(!test_bit(PAGE_STALE, &page->private)))
			continue;
		spin_unlock(&pool->stale_lock);
		cancel_work_sync(&zhdr->work);
		free_z3fold_page(page, false);
		cond_resched();
		spin_lock(&pool->stale_lock);
	}
	spin_unlock(&pool->stale_lock);
}

/*
 * Returns the number of free chunks in a z3fold page.
 * NB: can't be used with HEADLESS pages.
 */
static int num_free_chunks(struct z3fold_header *zhdr)
{
	int nfree;
	/*
	 * If there is a middle object, pick up the bigger free space
	 * either before or after it. Otherwise just subtract the number
	 * of chunks occupied by the first and the last objects.
	 */
	if (zhdr->middle_chunks != 0) {
		int nfree_before = zhdr->first_chunks ?
			0 : zhdr->start_middle - ZHDR_CHUNKS;
		int nfree_after = zhdr->last_chunks ?
			0 : TOTAL_CHUNKS -
				(zhdr->start_middle + zhdr->middle_chunks);
		nfree = max(nfree_before, nfree_after);
	} else
		nfree = NCHUNKS - zhdr->first_chunks - zhdr->last_chunks;
	return nfree;
}

/* Add to the appropriate unbuddied list */
static inline void add_to_unbuddied(struct z3fold_pool *pool,
				struct z3fold_header *zhdr)
{
	if (zhdr->first_chunks == 0 || zhdr->last_chunks == 0 ||
			zhdr->middle_chunks == 0) {
		struct list_head *unbuddied;
		int freechunks = num_free_chunks(zhdr);

		migrate_disable();
		unbuddied = this_cpu_ptr(pool->unbuddied);
		spin_lock(&pool->lock);
		list_add(&zhdr->buddy, &unbuddied[freechunks]);
		spin_unlock(&pool->lock);
		zhdr->cpu = smp_processor_id();
		migrate_enable();
	}
}

static inline enum buddy get_free_buddy(struct z3fold_header *zhdr, int chunks)
{
	enum buddy bud = HEADLESS;

	if (zhdr->middle_chunks) {
		if (!zhdr->first_chunks &&
		    chunks <= zhdr->start_middle - ZHDR_CHUNKS)
			bud = FIRST;
		else if (!zhdr->last_chunks)
			bud = LAST;
	} else {
		if (!zhdr->first_chunks)
			bud = FIRST;
		else if (!zhdr->last_chunks)
			bud = LAST;
		else
			bud = MIDDLE;
	}

	return bud;
}

static inline void *mchunk_memmove(struct z3fold_header *zhdr,
				unsigned short dst_chunk)
{
	void *beg = zhdr;
	return memmove(beg + (dst_chunk << CHUNK_SHIFT),
		       beg + (zhdr->start_middle << CHUNK_SHIFT),
		       zhdr->middle_chunks << CHUNK_SHIFT);
}

static inline bool buddy_single(struct z3fold_header *zhdr)
{
	return !((zhdr->first_chunks && zhdr->middle_chunks) ||
			(zhdr->first_chunks && zhdr->last_chunks) ||
			(zhdr->middle_chunks && zhdr->last_chunks));
}

static struct z3fold_header *compact_single_buddy(struct z3fold_header *zhdr)
{
	struct z3fold_pool *pool = zhdr_to_pool(zhdr);
	void *p = zhdr;
	unsigned long old_handle = 0;
	size_t sz = 0;
	struct z3fold_header *new_zhdr = NULL;
	int first_idx = __idx(zhdr, FIRST);
	int middle_idx = __idx(zhdr, MIDDLE);
	int last_idx = __idx(zhdr, LAST);
	unsigned short *moved_chunks = NULL;

	/*
	 * No need to protect slots here -- all the slots are "local" and
	 * the page lock is already taken
	 */
	if (zhdr->first_chunks && zhdr->slots->slot[first_idx]) {
		p += ZHDR_SIZE_ALIGNED;
		sz = zhdr->first_chunks << CHUNK_SHIFT;
		old_handle = (unsigned long)&zhdr->slots->slot[first_idx];
		moved_chunks = &zhdr->first_chunks;
	} else if (zhdr->middle_chunks && zhdr->slots->slot[middle_idx]) {
		p += zhdr->start_middle << CHUNK_SHIFT;
		sz = zhdr->middle_chunks << CHUNK_SHIFT;
		old_handle = (unsigned long)&zhdr->slots->slot[middle_idx];
		moved_chunks = &zhdr->middle_chunks;
	} else if (zhdr->last_chunks && zhdr->slots->slot[last_idx]) {
		p += PAGE_SIZE - (zhdr->last_chunks << CHUNK_SHIFT);
		sz = zhdr->last_chunks << CHUNK_SHIFT;
		old_handle = (unsigned long)&zhdr->slots->slot[last_idx];
		moved_chunks = &zhdr->last_chunks;
	}

	if (sz > 0) {
		enum buddy new_bud = HEADLESS;
		short chunks = size_to_chunks(sz);
		void *q;

		new_zhdr = __z3fold_alloc(pool, sz, false);
		if (!new_zhdr)
			return NULL;

		if (WARN_ON(new_zhdr == zhdr))
			goto out_fail;

		new_bud = get_free_buddy(new_zhdr, chunks);
		q = new_zhdr;
		switch (new_bud) {
		case FIRST:
			new_zhdr->first_chunks = chunks;
			q += ZHDR_SIZE_ALIGNED;
			break;
		case MIDDLE:
			new_zhdr->middle_chunks = chunks;
			new_zhdr->start_middle =
				new_zhdr->first_chunks + ZHDR_CHUNKS;
			q += new_zhdr->start_middle << CHUNK_SHIFT;
			break;
		case LAST:
			new_zhdr->last_chunks = chunks;
			q += PAGE_SIZE - (new_zhdr->last_chunks << CHUNK_SHIFT);
			break;
		default:
			goto out_fail;
		}
		new_zhdr->foreign_handles++;
		memcpy(q, p, sz);
		write_lock(&zhdr->slots->lock);
		*(unsigned long *)old_handle = (unsigned long)new_zhdr +
			__idx(new_zhdr, new_bud);
		if (new_bud == LAST)
			*(unsigned long *)old_handle |=
					(new_zhdr->last_chunks << BUDDY_SHIFT);
		write_unlock(&zhdr->slots->lock);
		add_to_unbuddied(pool, new_zhdr);
		z3fold_page_unlock(new_zhdr);

		*moved_chunks = 0;
	}

	return new_zhdr;

out_fail:
	if (new_zhdr) {
		if (kref_put(&new_zhdr->refcount, release_z3fold_page_locked))
			atomic64_dec(&pool->pages_nr);
		else {
			add_to_unbuddied(pool, new_zhdr);
			z3fold_page_unlock(new_zhdr);
		}
	}
	return NULL;

}

#define BIG_CHUNK_GAP	3
/* Has to be called with lock held */
static int z3fold_compact_page(struct z3fold_header *zhdr)
{
	struct page *page = virt_to_page(zhdr);

	if (test_bit(MIDDLE_CHUNK_MAPPED, &page->private))
		return 0; /* can't move middle chunk, it's used */

	if (unlikely(PageIsolated(page)))
		return 0;

	if (zhdr->middle_chunks == 0)
		return 0; /* nothing to compact */

	if (zhdr->first_chunks == 0 && zhdr->last_chunks == 0) {
		/* move to the beginning */
		mchunk_memmove(zhdr, ZHDR_CHUNKS);
		zhdr->first_chunks = zhdr->middle_chunks;
		zhdr->middle_chunks = 0;
		zhdr->start_middle = 0;
		zhdr->first_num++;
		return 1;
	}

	/*
	 * moving data is expensive, so let's only do that if
	 * there's substantial gain (at least BIG_CHUNK_GAP chunks)
	 */
	if (zhdr->first_chunks != 0 && zhdr->last_chunks == 0 &&
	    zhdr->start_middle - (zhdr->first_chunks + ZHDR_CHUNKS) >=
			BIG_CHUNK_GAP) {
		mchunk_memmove(zhdr, zhdr->first_chunks + ZHDR_CHUNKS);
		zhdr->start_middle = zhdr->first_chunks + ZHDR_CHUNKS;
		return 1;
	} else if (zhdr->last_chunks != 0 && zhdr->first_chunks == 0 &&
		   TOTAL_CHUNKS - (zhdr->last_chunks + zhdr->start_middle
					+ zhdr->middle_chunks) >=
			BIG_CHUNK_GAP) {
		unsigned short new_start = TOTAL_CHUNKS - zhdr->last_chunks -
			zhdr->middle_chunks;
		mchunk_memmove(zhdr, new_start);
		zhdr->start_middle = new_start;
		return 1;
	}

	return 0;
}

static void do_compact_page(struct z3fold_header *zhdr, bool locked)
{
	struct z3fold_pool *pool = zhdr_to_pool(zhdr);
	struct page *page;

	page = virt_to_page(zhdr);
	if (locked)
		WARN_ON(z3fold_page_trylock(zhdr));
	else
		z3fold_page_lock(zhdr);
	if (WARN_ON(!test_and_clear_bit(NEEDS_COMPACTING, &page->private))) {
		z3fold_page_unlock(zhdr);
		return;
	}
	spin_lock(&pool->lock);
	list_del_init(&zhdr->buddy);
	spin_unlock(&pool->lock);

	if (kref_put(&zhdr->refcount, release_z3fold_page_locked)) {
		atomic64_dec(&pool->pages_nr);
		return;
	}

	if (test_bit(PAGE_STALE, &page->private) ||
	    test_and_set_bit(PAGE_CLAIMED, &page->private)) {
		z3fold_page_unlock(zhdr);
		return;
	}

	if (!zhdr->foreign_handles && buddy_single(zhdr) &&
	    zhdr->mapped_count == 0 && compact_single_buddy(zhdr)) {
		if (kref_put(&zhdr->refcount, release_z3fold_page_locked))
			atomic64_dec(&pool->pages_nr);
		else {
			clear_bit(PAGE_CLAIMED, &page->private);
			z3fold_page_unlock(zhdr);
		}
		return;
	}

	z3fold_compact_page(zhdr);
	add_to_unbuddied(pool, zhdr);
	clear_bit(PAGE_CLAIMED, &page->private);
	z3fold_page_unlock(zhdr);
}

static void compact_page_work(struct work_struct *w)
{
	struct z3fold_header *zhdr = container_of(w, struct z3fold_header,
						work);

	do_compact_page(zhdr, false);
}

/* returns _locked_ z3fold page header or NULL */
static inline struct z3fold_header *__z3fold_alloc(struct z3fold_pool *pool,
						size_t size, bool can_sleep)
{
	struct z3fold_header *zhdr = NULL;
	struct page *page;
	struct list_head *unbuddied;
	int chunks = size_to_chunks(size), i;

lookup:
	migrate_disable();
	/* First, try to find an unbuddied z3fold page. */
	unbuddied = this_cpu_ptr(pool->unbuddied);
	for_each_unbuddied_list(i, chunks) {
		struct list_head *l = &unbuddied[i];

		zhdr = list_first_entry_or_null(READ_ONCE(l),
					struct z3fold_header, buddy);

		if (!zhdr)
			continue;

		/* Re-check under lock. */
		spin_lock(&pool->lock);
		l = &unbuddied[i];
		if (unlikely(zhdr != list_first_entry(READ_ONCE(l),
						struct z3fold_header, buddy)) ||
		    !z3fold_page_trylock(zhdr)) {
			spin_unlock(&pool->lock);
			zhdr = NULL;
			migrate_enable();
			if (can_sleep)
				cond_resched();
			goto lookup;
		}
		list_del_init(&zhdr->buddy);
		zhdr->cpu = -1;
		spin_unlock(&pool->lock);

		page = virt_to_page(zhdr);
		if (test_bit(NEEDS_COMPACTING, &page->private) ||
		    test_bit(PAGE_CLAIMED, &page->private)) {
			z3fold_page_unlock(zhdr);
			zhdr = NULL;
			migrate_enable();
			if (can_sleep)
				cond_resched();
			goto lookup;
		}

		/*
		 * this page could not be removed from its unbuddied
		 * list while pool lock was held, and then we've taken
		 * page lock so kref_put could not be called before
		 * we got here, so it's safe to just call kref_get()
		 */
		kref_get(&zhdr->refcount);
		break;
	}
	migrate_enable();

	if (!zhdr) {
		int cpu;

		/* look for _exact_ match on other cpus' lists */
		for_each_online_cpu(cpu) {
			struct list_head *l;

			unbuddied = per_cpu_ptr(pool->unbuddied, cpu);
			spin_lock(&pool->lock);
			l = &unbuddied[chunks];

			zhdr = list_first_entry_or_null(READ_ONCE(l),
						struct z3fold_header, buddy);

			if (!zhdr || !z3fold_page_trylock(zhdr)) {
				spin_unlock(&pool->lock);
				zhdr = NULL;
				continue;
			}
			list_del_init(&zhdr->buddy);
			zhdr->cpu = -1;
			spin_unlock(&pool->lock);

			page = virt_to_page(zhdr);
			if (test_bit(NEEDS_COMPACTING, &page->private) ||
			    test_bit(PAGE_CLAIMED, &page->private)) {
				z3fold_page_unlock(zhdr);
				zhdr = NULL;
				if (can_sleep)
					cond_resched();
				continue;
			}
			kref_get(&zhdr->refcount);
			break;
		}
	}

	if (zhdr && !zhdr->slots)
		zhdr->slots = alloc_slots(pool,
					can_sleep ? GFP_NOIO : GFP_ATOMIC);
	return zhdr;
}

/*
 * API Functions
 */

/**
 * z3fold_create_pool() - create a new z3fold pool
 * @name:	pool name
 * @gfp:	gfp flags when allocating the z3fold pool structure
 * @ops:	user-defined operations for the z3fold pool
 *
 * Return: pointer to the new z3fold pool or NULL if the metadata allocation
 * failed.
 */
static struct z3fold_pool *z3fold_create_pool(const char *name, gfp_t gfp,
		const struct z3fold_ops *ops)
{
	struct z3fold_pool *pool = NULL;
	int i, cpu;

	pool = kzalloc(sizeof(struct z3fold_pool), gfp);
	if (!pool)
		goto out;
	pool->c_handle = kmem_cache_create("z3fold_handle",
				sizeof(struct z3fold_buddy_slots),
				SLOTS_ALIGN, 0, NULL);
	if (!pool->c_handle)
		goto out_c;
	spin_lock_init(&pool->lock);
	spin_lock_init(&pool->stale_lock);
	pool->unbuddied = __alloc_percpu(sizeof(struct list_head)*NCHUNKS, 2);
	if (!pool->unbuddied)
		goto out_pool;
	for_each_possible_cpu(cpu) {
		struct list_head *unbuddied =
				per_cpu_ptr(pool->unbuddied, cpu);
		for_each_unbuddied_list(i, 0)
			INIT_LIST_HEAD(&unbuddied[i]);
	}
	INIT_LIST_HEAD(&pool->lru);
	INIT_LIST_HEAD(&pool->stale);
	atomic64_set(&pool->pages_nr, 0);
	pool->name = name;
	pool->compact_wq = create_singlethread_workqueue(pool->name);
	if (!pool->compact_wq)
		goto out_unbuddied;
	pool->release_wq = create_singlethread_workqueue(pool->name);
	if (!pool->release_wq)
		goto out_wq;
	if (z3fold_register_migration(pool))
		goto out_rwq;
	INIT_WORK(&pool->work, free_pages_work);
	pool->ops = ops;
	return pool;

out_rwq:
	destroy_workqueue(pool->release_wq);
out_wq:
	destroy_workqueue(pool->compact_wq);
out_unbuddied:
	free_percpu(pool->unbuddied);
out_pool:
	kmem_cache_destroy(pool->c_handle);
out_c:
	kfree(pool);
out:
	return NULL;
}

/**
 * z3fold_destroy_pool() - destroys an existing z3fold pool
 * @pool:	the z3fold pool to be destroyed
 *
 * The pool should be emptied before this function is called.
 */
static void z3fold_destroy_pool(struct z3fold_pool *pool)
{
	kmem_cache_destroy(pool->c_handle);

	/*
	 * We need to destroy pool->compact_wq before pool->release_wq,
	 * as any pending work on pool->compact_wq will call
	 * queue_work(pool->release_wq, &pool->work).
	 *
	 * There are still outstanding pages until both workqueues are drained,
	 * so we cannot unregister migration until then.
	 */

	destroy_workqueue(pool->compact_wq);
	destroy_workqueue(pool->release_wq);
	z3fold_unregister_migration(pool);
	kfree(pool);
}

/**
 * z3fold_alloc() - allocates a region of a given size
 * @pool:	z3fold pool from which to allocate
 * @size:	size in bytes of the desired allocation
 * @gfp:	gfp flags used if the pool needs to grow
 * @handle:	handle of the new allocation
 *
 * This function will attempt to find a free region in the pool large enough to
 * satisfy the allocation request.  A search of the unbuddied lists is
 * performed first. If no suitable free region is found, then a new page is
 * allocated and added to the pool to satisfy the request.
 *
 * gfp should not set __GFP_HIGHMEM as highmem pages cannot be used
 * as z3fold pool pages.
 *
 * Return: 0 if success and handle is set, otherwise -EINVAL if the size or
 * gfp arguments are invalid or -ENOMEM if the pool was unable to allocate
 * a new page.
 */
static int z3fold_alloc(struct z3fold_pool *pool, size_t size, gfp_t gfp,
			unsigned long *handle)
{
	int chunks = size_to_chunks(size);
	struct z3fold_header *zhdr = NULL;
	struct page *page = NULL;
	enum buddy bud;
	bool can_sleep = gfpflags_allow_blocking(gfp);

	if (!size)
		return -EINVAL;

	if (size > PAGE_SIZE)
		return -ENOSPC;

	if (size > PAGE_SIZE - ZHDR_SIZE_ALIGNED - CHUNK_SIZE)
		bud = HEADLESS;
	else {
retry:
		zhdr = __z3fold_alloc(pool, size, can_sleep);
		if (zhdr) {
			bud = get_free_buddy(zhdr, chunks);
			if (bud == HEADLESS) {
				if (kref_put(&zhdr->refcount,
					     release_z3fold_page_locked))
					atomic64_dec(&pool->pages_nr);
				else
					z3fold_page_unlock(zhdr);
				pr_err("No free chunks in unbuddied\n");
				WARN_ON(1);
				goto retry;
			}
			page = virt_to_page(zhdr);
			goto found;
		}
		bud = FIRST;
	}

	page = NULL;
	if (can_sleep) {
		spin_lock(&pool->stale_lock);
		zhdr = list_first_entry_or_null(&pool->stale,
						struct z3fold_header, buddy);
		/*
		 * Before allocating a page, let's see if we can take one from
		 * the stale pages list. cancel_work_sync() can sleep so we
		 * limit this case to the contexts where we can sleep
		 */
		if (zhdr) {
			list_del(&zhdr->buddy);
			spin_unlock(&pool->stale_lock);
			cancel_work_sync(&zhdr->work);
			page = virt_to_page(zhdr);
		} else {
			spin_unlock(&pool->stale_lock);
		}
	}
	if (!page)
		page = alloc_page(gfp);

	if (!page)
		return -ENOMEM;

	zhdr = init_z3fold_page(page, bud == HEADLESS, pool, gfp);
	if (!zhdr) {
		__free_page(page);
		return -ENOMEM;
	}
	atomic64_inc(&pool->pages_nr);

	if (bud == HEADLESS) {
		set_bit(PAGE_HEADLESS, &page->private);
		goto headless;
	}
	if (can_sleep) {
		lock_page(page);
		__SetPageMovable(page, pool->inode->i_mapping);
		unlock_page(page);
	} else {
		if (trylock_page(page)) {
			__SetPageMovable(page, pool->inode->i_mapping);
			unlock_page(page);
		}
	}
	z3fold_page_lock(zhdr);

found:
	if (bud == FIRST)
		zhdr->first_chunks = chunks;
	else if (bud == LAST)
		zhdr->last_chunks = chunks;
	else {
		zhdr->middle_chunks = chunks;
		zhdr->start_middle = zhdr->first_chunks + ZHDR_CHUNKS;
	}
	add_to_unbuddied(pool, zhdr);

headless:
	spin_lock(&pool->lock);
	/* Add/move z3fold page to beginning of LRU */
	if (!list_empty(&page->lru))
		list_del(&page->lru);

	list_add(&page->lru, &pool->lru);

	*handle = encode_handle(zhdr, bud);
	spin_unlock(&pool->lock);
	if (bud != HEADLESS)
		z3fold_page_unlock(zhdr);

	return 0;
}

/**
 * z3fold_free() - frees the allocation associated with the given handle
 * @pool:	pool in which the allocation resided
 * @handle:	handle associated with the allocation returned by z3fold_alloc()
 *
 * In the case that the z3fold page in which the allocation resides is under
 * reclaim, as indicated by the PG_reclaim flag being set, this function
 * only sets the first|last_chunks to 0.  The page is actually freed
 * once both buddies are evicted (see z3fold_reclaim_page() below).
 */
static void z3fold_free(struct z3fold_pool *pool, unsigned long handle)
{
	struct z3fold_header *zhdr;
	struct page *page;
	enum buddy bud;
	bool page_claimed;

	zhdr = get_z3fold_header(handle);
	page = virt_to_page(zhdr);
	page_claimed = test_and_set_bit(PAGE_CLAIMED, &page->private);

	if (test_bit(PAGE_HEADLESS, &page->private)) {
		/* if a headless page is under reclaim, just leave.
		 * NB: we use test_and_set_bit for a reason: if the bit
		 * has not been set before, we release this page
		 * immediately so we don't care about its value any more.
		 */
		if (!page_claimed) {
			spin_lock(&pool->lock);
			list_del(&page->lru);
			spin_unlock(&pool->lock);
			put_z3fold_header(zhdr);
			free_z3fold_page(page, true);
			atomic64_dec(&pool->pages_nr);
		}
		return;
	}

	/* Non-headless case */
	bud = handle_to_buddy(handle);

	switch (bud) {
	case FIRST:
		zhdr->first_chunks = 0;
		break;
	case MIDDLE:
		zhdr->middle_chunks = 0;
		break;
	case LAST:
		zhdr->last_chunks = 0;
		break;
	default:
		pr_err("%s: unknown bud %d\n", __func__, bud);
		WARN_ON(1);
		put_z3fold_header(zhdr);
		return;
	}

	if (!page_claimed)
		free_handle(handle, zhdr);
	if (kref_put(&zhdr->refcount, release_z3fold_page_locked_list)) {
		atomic64_dec(&pool->pages_nr);
		return;
	}
	if (page_claimed) {
		/* the page has not been claimed by us */
		z3fold_page_unlock(zhdr);
		return;
	}
	if (test_and_set_bit(NEEDS_COMPACTING, &page->private)) {
		put_z3fold_header(zhdr);
		clear_bit(PAGE_CLAIMED, &page->private);
		return;
	}
	if (zhdr->cpu < 0 || !cpu_online(zhdr->cpu)) {
		spin_lock(&pool->lock);
		list_del_init(&zhdr->buddy);
		spin_unlock(&pool->lock);
		zhdr->cpu = -1;
		kref_get(&zhdr->refcount);
		clear_bit(PAGE_CLAIMED, &page->private);
		do_compact_page(zhdr, true);
		return;
	}
	kref_get(&zhdr->refcount);
	clear_bit(PAGE_CLAIMED, &page->private);
	queue_work_on(zhdr->cpu, pool->compact_wq, &zhdr->work);
	put_z3fold_header(zhdr);
}

/**
 * z3fold_reclaim_page() - evicts allocations from a pool page and frees it
 * @pool:	pool from which a page will attempt to be evicted
 * @retries:	number of pages on the LRU list for which eviction will
 *		be attempted before failing
 *
 * z3fold reclaim is different from normal system reclaim in that it is done
 * from the bottom, up. This is because only the bottom layer, z3fold, has
 * information on how the allocations are organized within each z3fold page.
 * This has the potential to create interesting locking situations between
 * z3fold and the user, however.
 *
 * To avoid these, this is how z3fold_reclaim_page() should be called:
 *
 * The user detects a page should be reclaimed and calls z3fold_reclaim_page().
 * z3fold_reclaim_page() will remove a z3fold page from the pool LRU list and
 * call the user-defined eviction handler with the pool and handle as
 * arguments.
 *
 * If the handle can not be evicted, the eviction handler should return
 * non-zero. z3fold_reclaim_page() will add the z3fold page back to the
 * appropriate list and try the next z3fold page on the LRU up to
 * a user defined number of retries.
 *
 * If the handle is successfully evicted, the eviction handler should
 * return 0 _and_ should have called z3fold_free() on the handle. z3fold_free()
 * contains logic to delay freeing the page if the page is under reclaim,
 * as indicated by the setting of the PG_reclaim flag on the underlying page.
 *
 * If all buddies in the z3fold page are successfully evicted, then the
 * z3fold page can be freed.
 *
 * Returns: 0 if page is successfully freed, otherwise -EINVAL if there are
 * no pages to evict or an eviction handler is not registered, -EAGAIN if
 * the retry limit was hit.
 */
static int z3fold_reclaim_page(struct z3fold_pool *pool, unsigned int retries)
{
	int i, ret = -1;
	struct z3fold_header *zhdr = NULL;
	struct page *page = NULL;
	struct list_head *pos;
	unsigned long first_handle = 0, middle_handle = 0, last_handle = 0;
	struct z3fold_buddy_slots slots __attribute__((aligned(SLOTS_ALIGN)));

	rwlock_init(&slots.lock);
	slots.pool = (unsigned long)pool | (1 << HANDLES_NOFREE);

	spin_lock(&pool->lock);
	if (!pool->ops || !pool->ops->evict || retries == 0) {
		spin_unlock(&pool->lock);
		return -EINVAL;
	}
	for (i = 0; i < retries; i++) {
		if (list_empty(&pool->lru)) {
			spin_unlock(&pool->lock);
			return -EINVAL;
		}
		list_for_each_prev(pos, &pool->lru) {
			page = list_entry(pos, struct page, lru);

			zhdr = page_address(page);
			if (test_bit(PAGE_HEADLESS, &page->private)) {
				/*
				 * For non-headless pages, we wait to do this
				 * until we have the page lock to avoid racing
				 * with __z3fold_alloc(). Headless pages don't
				 * have a lock (and __z3fold_alloc() will never
				 * see them), but we still need to test and set
				 * PAGE_CLAIMED to avoid racing with
				 * z3fold_free(), so just do it now before
				 * leaving the loop.
				 */
				if (test_and_set_bit(PAGE_CLAIMED, &page->private))
					continue;

				break;
			}

			if (kref_get_unless_zero(&zhdr->refcount) == 0) {
				zhdr = NULL;
				break;
			}
			if (!z3fold_page_trylock(zhdr)) {
				if (kref_put(&zhdr->refcount,
						release_z3fold_page))
					atomic64_dec(&pool->pages_nr);
				zhdr = NULL;
				continue; /* can't evict at this point */
			}

			/* test_and_set_bit is of course atomic, but we still
			 * need to do it under page lock, otherwise checking
			 * that bit in __z3fold_alloc wouldn't make sense
			 */
			if (zhdr->foreign_handles ||
			    test_and_set_bit(PAGE_CLAIMED, &page->private)) {
				if (kref_put(&zhdr->refcount,
						release_z3fold_page))
					atomic64_dec(&pool->pages_nr);
				else
					z3fold_page_unlock(zhdr);
				zhdr = NULL;
				continue; /* can't evict such page */
			}
			list_del_init(&zhdr->buddy);
			zhdr->cpu = -1;
			break;
		}

		if (!zhdr)
			break;

		list_del_init(&page->lru);
		spin_unlock(&pool->lock);

		if (!test_bit(PAGE_HEADLESS, &page->private)) {
			/*
			 * We need encode the handles before unlocking, and
			 * use our local slots structure because z3fold_free
			 * can zero out zhdr->slots and we can't do much
			 * about that
			 */
			first_handle = 0;
			last_handle = 0;
			middle_handle = 0;
			memset(slots.slot, 0, sizeof(slots.slot));
			if (zhdr->first_chunks)
				first_handle = __encode_handle(zhdr, &slots,
								FIRST);
			if (zhdr->middle_chunks)
				middle_handle = __encode_handle(zhdr, &slots,
								MIDDLE);
			if (zhdr->last_chunks)
				last_handle = __encode_handle(zhdr, &slots,
								LAST);
			/*
			 * it's safe to unlock here because we hold a
			 * reference to this page
			 */
			z3fold_page_unlock(zhdr);
		} else {
			first_handle = encode_handle(zhdr, HEADLESS);
			last_handle = middle_handle = 0;
		}
		/* Issue the eviction callback(s) */
		if (middle_handle) {
			ret = pool->ops->evict(pool, middle_handle);
			if (ret)
				goto next;
		}
		if (first_handle) {
			ret = pool->ops->evict(pool, first_handle);
			if (ret)
				goto next;
		}
		if (last_handle) {
			ret = pool->ops->evict(pool, last_handle);
			if (ret)
				goto next;
		}
next:
		if (test_bit(PAGE_HEADLESS, &page->private)) {
			if (ret == 0) {
				free_z3fold_page(page, true);
				atomic64_dec(&pool->pages_nr);
				return 0;
			}
			spin_lock(&pool->lock);
			list_add(&page->lru, &pool->lru);
			spin_unlock(&pool->lock);
			clear_bit(PAGE_CLAIMED, &page->private);
		} else {
			struct z3fold_buddy_slots *slots = zhdr->slots;
			z3fold_page_lock(zhdr);
			if (kref_put(&zhdr->refcount,
					release_z3fold_page_locked)) {
				kmem_cache_free(pool->c_handle, slots);
				atomic64_dec(&pool->pages_nr);
				return 0;
			}
			/*
			 * if we are here, the page is still not completely
			 * free. Take the global pool lock then to be able
			 * to add it back to the lru list
			 */
			spin_lock(&pool->lock);
			list_add(&page->lru, &pool->lru);
			spin_unlock(&pool->lock);
			z3fold_page_unlock(zhdr);
			clear_bit(PAGE_CLAIMED, &page->private);
		}

		/* We started off locked to we need to lock the pool back */
		spin_lock(&pool->lock);
	}
	spin_unlock(&pool->lock);
	return -EAGAIN;
}

/**
 * z3fold_map() - maps the allocation associated with the given handle
 * @pool:	pool in which the allocation resides
 * @handle:	handle associated with the allocation to be mapped
 *
 * Extracts the buddy number from handle and constructs the pointer to the
 * correct starting chunk within the page.
 *
 * Returns: a pointer to the mapped allocation
 */
static void *z3fold_map(struct z3fold_pool *pool, unsigned long handle)
{
	struct z3fold_header *zhdr;
	struct page *page;
	void *addr;
	enum buddy buddy;

	zhdr = get_z3fold_header(handle);
	addr = zhdr;
	page = virt_to_page(zhdr);

	if (test_bit(PAGE_HEADLESS, &page->private))
		goto out;

	buddy = handle_to_buddy(handle);
	switch (buddy) {
	case FIRST:
		addr += ZHDR_SIZE_ALIGNED;
		break;
	case MIDDLE:
		addr += zhdr->start_middle << CHUNK_SHIFT;
		set_bit(MIDDLE_CHUNK_MAPPED, &page->private);
		break;
	case LAST:
		addr += PAGE_SIZE - (handle_to_chunks(handle) << CHUNK_SHIFT);
		break;
	default:
		pr_err("unknown buddy id %d\n", buddy);
		WARN_ON(1);
		addr = NULL;
		break;
	}

	if (addr)
		zhdr->mapped_count++;
out:
	put_z3fold_header(zhdr);
	return addr;
}

/**
 * z3fold_unmap() - unmaps the allocation associated with the given handle
 * @pool:	pool in which the allocation resides
 * @handle:	handle associated with the allocation to be unmapped
 */
static void z3fold_unmap(struct z3fold_pool *pool, unsigned long handle)
{
	struct z3fold_header *zhdr;
	struct page *page;
	enum buddy buddy;

	zhdr = get_z3fold_header(handle);
	page = virt_to_page(zhdr);

	if (test_bit(PAGE_HEADLESS, &page->private))
		return;

	buddy = handle_to_buddy(handle);
	if (buddy == MIDDLE)
		clear_bit(MIDDLE_CHUNK_MAPPED, &page->private);
	zhdr->mapped_count--;
	put_z3fold_header(zhdr);
}

/**
 * z3fold_get_pool_size() - gets the z3fold pool size in pages
 * @pool:	pool whose size is being queried
 *
 * Returns: size in pages of the given pool.
 */
static u64 z3fold_get_pool_size(struct z3fold_pool *pool)
{
	return atomic64_read(&pool->pages_nr);
}

static bool z3fold_page_isolate(struct page *page, isolate_mode_t mode)
{
	struct z3fold_header *zhdr;
	struct z3fold_pool *pool;

	VM_BUG_ON_PAGE(!PageMovable(page), page);
	VM_BUG_ON_PAGE(PageIsolated(page), page);

	if (test_bit(PAGE_HEADLESS, &page->private))
		return false;

	zhdr = page_address(page);
	z3fold_page_lock(zhdr);
	if (test_bit(NEEDS_COMPACTING, &page->private) ||
	    test_bit(PAGE_STALE, &page->private))
		goto out;

	if (zhdr->mapped_count != 0 || zhdr->foreign_handles != 0)
		goto out;

	if (test_and_set_bit(PAGE_CLAIMED, &page->private))
		goto out;
	pool = zhdr_to_pool(zhdr);
	spin_lock(&pool->lock);
	if (!list_empty(&zhdr->buddy))
		list_del_init(&zhdr->buddy);
	if (!list_empty(&page->lru))
		list_del_init(&page->lru);
	spin_unlock(&pool->lock);

	kref_get(&zhdr->refcount);
	z3fold_page_unlock(zhdr);
	return true;

out:
	z3fold_page_unlock(zhdr);
	return false;
}

static int z3fold_page_migrate(struct address_space *mapping, struct page *newpage,
			       struct page *page, enum migrate_mode mode)
{
	struct z3fold_header *zhdr, *new_zhdr;
	struct z3fold_pool *pool;
	struct address_space *new_mapping;

	VM_BUG_ON_PAGE(!PageMovable(page), page);
	VM_BUG_ON_PAGE(!PageIsolated(page), page);
	VM_BUG_ON_PAGE(!test_bit(PAGE_CLAIMED, &page->private), page);
	VM_BUG_ON_PAGE(!PageLocked(newpage), newpage);

	zhdr = page_address(page);
	pool = zhdr_to_pool(zhdr);

	if (!z3fold_page_trylock(zhdr))
		return -EAGAIN;
	if (zhdr->mapped_count != 0 || zhdr->foreign_handles != 0) {
		z3fold_page_unlock(zhdr);
		clear_bit(PAGE_CLAIMED, &page->private);
		return -EBUSY;
	}
	if (work_pending(&zhdr->work)) {
		z3fold_page_unlock(zhdr);
		return -EAGAIN;
	}
	new_zhdr = page_address(newpage);
	memcpy(new_zhdr, zhdr, PAGE_SIZE);
	newpage->private = page->private;
	page->private = 0;
	z3fold_page_unlock(zhdr);
	spin_lock_init(&new_zhdr->page_lock);
	INIT_WORK(&new_zhdr->work, compact_page_work);
	/*
	 * z3fold_page_isolate() ensures that new_zhdr->buddy is empty,
	 * so we only have to reinitialize it.
	 */
	INIT_LIST_HEAD(&new_zhdr->buddy);
	new_mapping = page_mapping(page);
	__ClearPageMovable(page);
	ClearPagePrivate(page);

	get_page(newpage);
	z3fold_page_lock(new_zhdr);
	if (new_zhdr->first_chunks)
		encode_handle(new_zhdr, FIRST);
	if (new_zhdr->last_chunks)
		encode_handle(new_zhdr, LAST);
	if (new_zhdr->middle_chunks)
		encode_handle(new_zhdr, MIDDLE);
	set_bit(NEEDS_COMPACTING, &newpage->private);
	new_zhdr->cpu = smp_processor_id();
	spin_lock(&pool->lock);
	list_add(&newpage->lru, &pool->lru);
	spin_unlock(&pool->lock);
	__SetPageMovable(newpage, new_mapping);
	z3fold_page_unlock(new_zhdr);

	queue_work_on(new_zhdr->cpu, pool->compact_wq, &new_zhdr->work);

	page_mapcount_reset(page);
	clear_bit(PAGE_CLAIMED, &page->private);
	put_page(page);
	return 0;
}

static void z3fold_page_putback(struct page *page)
{
	struct z3fold_header *zhdr;
	struct z3fold_pool *pool;

	zhdr = page_address(page);
	pool = zhdr_to_pool(zhdr);

	z3fold_page_lock(zhdr);
	if (!list_empty(&zhdr->buddy))
		list_del_init(&zhdr->buddy);
	INIT_LIST_HEAD(&page->lru);
	if (kref_put(&zhdr->refcount, release_z3fold_page_locked)) {
		atomic64_dec(&pool->pages_nr);
		return;
	}
	spin_lock(&pool->lock);
	list_add(&page->lru, &pool->lru);
	spin_unlock(&pool->lock);
	clear_bit(PAGE_CLAIMED, &page->private);
	z3fold_page_unlock(zhdr);
}

static const struct address_space_operations z3fold_aops = {
	.isolate_page = z3fold_page_isolate,
	.migratepage = z3fold_page_migrate,
	.putback_page = z3fold_page_putback,
};

/*****************
 * zpool
 ****************/

static int z3fold_zpool_evict(struct z3fold_pool *pool, unsigned long handle)
{
	if (pool->zpool && pool->zpool_ops && pool->zpool_ops->evict)
		return pool->zpool_ops->evict(pool->zpool, handle);
	else
		return -ENOENT;
}

static const struct z3fold_ops z3fold_zpool_ops = {
	.evict =	z3fold_zpool_evict
};

static void *z3fold_zpool_create(const char *name, gfp_t gfp,
			       const struct zpool_ops *zpool_ops,
			       struct zpool *zpool)
{
	struct z3fold_pool *pool;

	pool = z3fold_create_pool(name, gfp,
				zpool_ops ? &z3fold_zpool_ops : NULL);
	if (pool) {
		pool->zpool = zpool;
		pool->zpool_ops = zpool_ops;
	}
	return pool;
}

static void z3fold_zpool_destroy(void *pool)
{
	z3fold_destroy_pool(pool);
}

static int z3fold_zpool_malloc(void *pool, size_t size, gfp_t gfp,
			unsigned long *handle)
{
	return z3fold_alloc(pool, size, gfp, handle);
}
static void z3fold_zpool_free(void *pool, unsigned long handle)
{
	z3fold_free(pool, handle);
}

static int z3fold_zpool_shrink(void *pool, unsigned int pages,
			unsigned int *reclaimed)
{
	unsigned int total = 0;
	int ret = -EINVAL;

	while (total < pages) {
		ret = z3fold_reclaim_page(pool, 8);
		if (ret < 0)
			break;
		total++;
	}

	if (reclaimed)
		*reclaimed = total;

	return ret;
}

static void *z3fold_zpool_map(void *pool, unsigned long handle,
			enum zpool_mapmode mm)
{
	return z3fold_map(pool, handle);
}
static void z3fold_zpool_unmap(void *pool, unsigned long handle)
{
	z3fold_unmap(pool, handle);
}

static u64 z3fold_zpool_total_size(void *pool)
{
	return z3fold_get_pool_size(pool) * PAGE_SIZE;
}

static struct zpool_driver z3fold_zpool_driver = {
	.type =		"z3fold",
	.sleep_mapped = true,
	.owner =	THIS_MODULE,
	.create =	z3fold_zpool_create,
	.destroy =	z3fold_zpool_destroy,
	.malloc =	z3fold_zpool_malloc,
	.free =		z3fold_zpool_free,
	.shrink =	z3fold_zpool_shrink,
	.map =		z3fold_zpool_map,
	.unmap =	z3fold_zpool_unmap,
	.total_size =	z3fold_zpool_total_size,
};

MODULE_ALIAS("zpool-z3fold");

static int __init init_z3fold(void)
{
	int ret;

	/* Make sure the z3fold header is not larger than the page size */
	BUILD_BUG_ON(ZHDR_SIZE_ALIGNED > PAGE_SIZE);
	ret = z3fold_mount();
	if (ret)
		return ret;

	zpool_register_driver(&z3fold_zpool_driver);

	return 0;
}

static void __exit exit_z3fold(void)
{
	z3fold_unmount();
	zpool_unregister_driver(&z3fold_zpool_driver);
}

module_init(init_z3fold);
module_exit(exit_z3fold);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vitaly Wool <vitalywool@gmail.com>");
MODULE_DESCRIPTION("3-Fold Allocator for Compressed Pages");
