// SPDX-License-Identifier: GPL-2.0
/* Maximum size of each resync request */
#define RESYNC_BLOCK_SIZE (64*1024)
#define RESYNC_PAGES ((RESYNC_BLOCK_SIZE + PAGE_SIZE-1) / PAGE_SIZE)

/*
 * Number of guaranteed raid bios in case of extreme VM load:
 */
#define	NR_RAID_BIOS 256

/* when we get a read error on a read-only array, we redirect to another
 * device without failing the first device, or trying to over-write to
 * correct the read error.  To keep track of bad blocks on a per-bio
 * level, we store IO_BLOCKED in the appropriate 'bios' pointer
 */
#define IO_BLOCKED ((struct bio *)1)
/* When we successfully write to a known bad-block, we need to remove the
 * bad-block marking which must be done from process context.  So we record
 * the success by setting devs[n].bio to IO_MADE_GOOD
 */
#define IO_MADE_GOOD ((struct bio *)2)

#define BIO_SPECIAL(bio) ((unsigned long)bio <= 2)

/* When there are this many requests queue to be written by
 * the raid thread, we become 'congested' to provide back-pressure
 * for writeback.
 */
static int max_queued_requests = 1024;

/* for managing resync I/O pages */
struct resync_pages {
	void		*raid_bio;
	struct page	*pages[RESYNC_PAGES];
};

static void rbio_pool_free(void *rbio, void *data)
{
	kfree(rbio);
}

static inline int resync_alloc_pages(struct resync_pages *rp,
				     gfp_t gfp_flags)
{
	int i;

	for (i = 0; i < RESYNC_PAGES; i++) {
		rp->pages[i] = alloc_page(gfp_flags);
		if (!rp->pages[i])
			goto out_free;
	}

	return 0;

out_free:
	while (--i >= 0)
		put_page(rp->pages[i]);
	return -ENOMEM;
}

static inline void resync_free_pages(struct resync_pages *rp)
{
	int i;

	for (i = 0; i < RESYNC_PAGES; i++)
		put_page(rp->pages[i]);
}

static inline void resync_get_all_pages(struct resync_pages *rp)
{
	int i;

	for (i = 0; i < RESYNC_PAGES; i++)
		get_page(rp->pages[i]);
}

static inline struct page *resync_fetch_page(struct resync_pages *rp,
					     unsigned idx)
{
	if (WARN_ON_ONCE(idx >= RESYNC_PAGES))
		return NULL;
	return rp->pages[idx];
}

/*
 * 'strct resync_pages' stores actual pages used for doing the resync
 *  IO, and it is per-bio, so make .bi_private points to it.
 */
static inline struct resync_pages *get_resync_pages(struct bio *bio)
{
	return bio->bi_private;
}

/* generally called after bio_reset() for reseting bvec */
static void md_bio_reset_resync_pages(struct bio *bio, struct resync_pages *rp,
			       int size)
{
	int idx = 0;

	/* initialize bvec table again */
	do {
		struct page *page = resync_fetch_page(rp, idx);
		int len = min_t(int, size, PAGE_SIZE);

		/*
		 * won't fail because the vec table is big
		 * enough to hold all these pages
		 */
		bio_add_page(bio, page, len, 0);
		size -= len;
	} while (idx++ < RESYNC_PAGES && size > 0);
}
