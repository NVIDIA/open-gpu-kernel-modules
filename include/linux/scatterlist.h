/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_SCATTERLIST_H
#define _LINUX_SCATTERLIST_H

#include <linux/string.h>
#include <linux/types.h>
#include <linux/bug.h>
#include <linux/mm.h>
#include <asm/io.h>

struct scatterlist {
	unsigned long	page_link;
	unsigned int	offset;
	unsigned int	length;
	dma_addr_t	dma_address;
#ifdef CONFIG_NEED_SG_DMA_LENGTH
	unsigned int	dma_length;
#endif
};

/*
 * These macros should be used after a dma_map_sg call has been done
 * to get bus addresses of each of the SG entries and their lengths.
 * You should only work with the number of sg entries dma_map_sg
 * returns, or alternatively stop on the first sg_dma_len(sg) which
 * is 0.
 */
#define sg_dma_address(sg)	((sg)->dma_address)

#ifdef CONFIG_NEED_SG_DMA_LENGTH
#define sg_dma_len(sg)		((sg)->dma_length)
#else
#define sg_dma_len(sg)		((sg)->length)
#endif

struct sg_table {
	struct scatterlist *sgl;	/* the list */
	unsigned int nents;		/* number of mapped entries */
	unsigned int orig_nents;	/* original size of list */
};

/*
 * Notes on SG table design.
 *
 * We use the unsigned long page_link field in the scatterlist struct to place
 * the page pointer AND encode information about the sg table as well. The two
 * lower bits are reserved for this information.
 *
 * If bit 0 is set, then the page_link contains a pointer to the next sg
 * table list. Otherwise the next entry is at sg + 1.
 *
 * If bit 1 is set, then this sg entry is the last element in a list.
 *
 * See sg_next().
 *
 */

#define SG_CHAIN	0x01UL
#define SG_END		0x02UL

/*
 * We overload the LSB of the page pointer to indicate whether it's
 * a valid sg entry, or whether it points to the start of a new scatterlist.
 * Those low bits are there for everyone! (thanks mason :-)
 */
#define sg_is_chain(sg)		((sg)->page_link & SG_CHAIN)
#define sg_is_last(sg)		((sg)->page_link & SG_END)
#define sg_chain_ptr(sg)	\
	((struct scatterlist *) ((sg)->page_link & ~(SG_CHAIN | SG_END)))

/**
 * sg_assign_page - Assign a given page to an SG entry
 * @sg:		    SG entry
 * @page:	    The page
 *
 * Description:
 *   Assign page to sg entry. Also see sg_set_page(), the most commonly used
 *   variant.
 *
 **/
static inline void sg_assign_page(struct scatterlist *sg, struct page *page)
{
	unsigned long page_link = sg->page_link & (SG_CHAIN | SG_END);

	/*
	 * In order for the low bit stealing approach to work, pages
	 * must be aligned at a 32-bit boundary as a minimum.
	 */
	BUG_ON((unsigned long) page & (SG_CHAIN | SG_END));
#ifdef CONFIG_DEBUG_SG
	BUG_ON(sg_is_chain(sg));
#endif
	sg->page_link = page_link | (unsigned long) page;
}

/**
 * sg_set_page - Set sg entry to point at given page
 * @sg:		 SG entry
 * @page:	 The page
 * @len:	 Length of data
 * @offset:	 Offset into page
 *
 * Description:
 *   Use this function to set an sg entry pointing at a page, never assign
 *   the page directly. We encode sg table information in the lower bits
 *   of the page pointer. See sg_page() for looking up the page belonging
 *   to an sg entry.
 *
 **/
static inline void sg_set_page(struct scatterlist *sg, struct page *page,
			       unsigned int len, unsigned int offset)
{
	sg_assign_page(sg, page);
	sg->offset = offset;
	sg->length = len;
}

static inline struct page *sg_page(struct scatterlist *sg)
{
#ifdef CONFIG_DEBUG_SG
	BUG_ON(sg_is_chain(sg));
#endif
	return (struct page *)((sg)->page_link & ~(SG_CHAIN | SG_END));
}

/**
 * sg_set_buf - Set sg entry to point at given data
 * @sg:		 SG entry
 * @buf:	 Data
 * @buflen:	 Data length
 *
 **/
static inline void sg_set_buf(struct scatterlist *sg, const void *buf,
			      unsigned int buflen)
{
#ifdef CONFIG_DEBUG_SG
	BUG_ON(!virt_addr_valid(buf));
#endif
	sg_set_page(sg, virt_to_page(buf), buflen, offset_in_page(buf));
}

/*
 * Loop over each sg element, following the pointer to a new list if necessary
 */
#define for_each_sg(sglist, sg, nr, __i)	\
	for (__i = 0, sg = (sglist); __i < (nr); __i++, sg = sg_next(sg))

/*
 * Loop over each sg element in the given sg_table object.
 */
#define for_each_sgtable_sg(sgt, sg, i)		\
	for_each_sg((sgt)->sgl, sg, (sgt)->orig_nents, i)

/*
 * Loop over each sg element in the given *DMA mapped* sg_table object.
 * Please use sg_dma_address(sg) and sg_dma_len(sg) to extract DMA addresses
 * of the each element.
 */
#define for_each_sgtable_dma_sg(sgt, sg, i)	\
	for_each_sg((sgt)->sgl, sg, (sgt)->nents, i)

static inline void __sg_chain(struct scatterlist *chain_sg,
			      struct scatterlist *sgl)
{
	/*
	 * offset and length are unused for chain entry. Clear them.
	 */
	chain_sg->offset = 0;
	chain_sg->length = 0;

	/*
	 * Set lowest bit to indicate a link pointer, and make sure to clear
	 * the termination bit if it happens to be set.
	 */
	chain_sg->page_link = ((unsigned long) sgl | SG_CHAIN) & ~SG_END;
}

/**
 * sg_chain - Chain two sglists together
 * @prv:	First scatterlist
 * @prv_nents:	Number of entries in prv
 * @sgl:	Second scatterlist
 *
 * Description:
 *   Links @prv@ and @sgl@ together, to form a longer scatterlist.
 *
 **/
static inline void sg_chain(struct scatterlist *prv, unsigned int prv_nents,
			    struct scatterlist *sgl)
{
	__sg_chain(&prv[prv_nents - 1], sgl);
}

/**
 * sg_mark_end - Mark the end of the scatterlist
 * @sg:		 SG entryScatterlist
 *
 * Description:
 *   Marks the passed in sg entry as the termination point for the sg
 *   table. A call to sg_next() on this entry will return NULL.
 *
 **/
static inline void sg_mark_end(struct scatterlist *sg)
{
	/*
	 * Set termination bit, clear potential chain bit
	 */
	sg->page_link |= SG_END;
	sg->page_link &= ~SG_CHAIN;
}

/**
 * sg_unmark_end - Undo setting the end of the scatterlist
 * @sg:		 SG entryScatterlist
 *
 * Description:
 *   Removes the termination marker from the given entry of the scatterlist.
 *
 **/
static inline void sg_unmark_end(struct scatterlist *sg)
{
	sg->page_link &= ~SG_END;
}

/**
 * sg_phys - Return physical address of an sg entry
 * @sg:	     SG entry
 *
 * Description:
 *   This calls page_to_phys() on the page in this sg entry, and adds the
 *   sg offset. The caller must know that it is legal to call page_to_phys()
 *   on the sg page.
 *
 **/
static inline dma_addr_t sg_phys(struct scatterlist *sg)
{
	return page_to_phys(sg_page(sg)) + sg->offset;
}

/**
 * sg_virt - Return virtual address of an sg entry
 * @sg:      SG entry
 *
 * Description:
 *   This calls page_address() on the page in this sg entry, and adds the
 *   sg offset. The caller must know that the sg page has a valid virtual
 *   mapping.
 *
 **/
static inline void *sg_virt(struct scatterlist *sg)
{
	return page_address(sg_page(sg)) + sg->offset;
}

/**
 * sg_init_marker - Initialize markers in sg table
 * @sgl:	   The SG table
 * @nents:	   Number of entries in table
 *
 **/
static inline void sg_init_marker(struct scatterlist *sgl,
				  unsigned int nents)
{
	sg_mark_end(&sgl[nents - 1]);
}

int sg_nents(struct scatterlist *sg);
int sg_nents_for_len(struct scatterlist *sg, u64 len);
struct scatterlist *sg_next(struct scatterlist *);
struct scatterlist *sg_last(struct scatterlist *s, unsigned int);
void sg_init_table(struct scatterlist *, unsigned int);
void sg_init_one(struct scatterlist *, const void *, unsigned int);
int sg_split(struct scatterlist *in, const int in_mapped_nents,
	     const off_t skip, const int nb_splits,
	     const size_t *split_sizes,
	     struct scatterlist **out, int *out_mapped_nents,
	     gfp_t gfp_mask);

typedef struct scatterlist *(sg_alloc_fn)(unsigned int, gfp_t);
typedef void (sg_free_fn)(struct scatterlist *, unsigned int);

void __sg_free_table(struct sg_table *, unsigned int, unsigned int,
		     sg_free_fn *);
void sg_free_table(struct sg_table *);
int __sg_alloc_table(struct sg_table *, unsigned int, unsigned int,
		     struct scatterlist *, unsigned int, gfp_t, sg_alloc_fn *);
int sg_alloc_table(struct sg_table *, unsigned int, gfp_t);
struct scatterlist *__sg_alloc_table_from_pages(struct sg_table *sgt,
		struct page **pages, unsigned int n_pages, unsigned int offset,
		unsigned long size, unsigned int max_segment,
		struct scatterlist *prv, unsigned int left_pages,
		gfp_t gfp_mask);
int sg_alloc_table_from_pages(struct sg_table *sgt, struct page **pages,
			      unsigned int n_pages, unsigned int offset,
			      unsigned long size, gfp_t gfp_mask);

#ifdef CONFIG_SGL_ALLOC
struct scatterlist *sgl_alloc_order(unsigned long long length,
				    unsigned int order, bool chainable,
				    gfp_t gfp, unsigned int *nent_p);
struct scatterlist *sgl_alloc(unsigned long long length, gfp_t gfp,
			      unsigned int *nent_p);
void sgl_free_n_order(struct scatterlist *sgl, int nents, int order);
void sgl_free_order(struct scatterlist *sgl, int order);
void sgl_free(struct scatterlist *sgl);
#endif /* CONFIG_SGL_ALLOC */

size_t sg_copy_buffer(struct scatterlist *sgl, unsigned int nents, void *buf,
		      size_t buflen, off_t skip, bool to_buffer);

size_t sg_copy_from_buffer(struct scatterlist *sgl, unsigned int nents,
			   const void *buf, size_t buflen);
size_t sg_copy_to_buffer(struct scatterlist *sgl, unsigned int nents,
			 void *buf, size_t buflen);

size_t sg_pcopy_from_buffer(struct scatterlist *sgl, unsigned int nents,
			    const void *buf, size_t buflen, off_t skip);
size_t sg_pcopy_to_buffer(struct scatterlist *sgl, unsigned int nents,
			  void *buf, size_t buflen, off_t skip);
size_t sg_zero_buffer(struct scatterlist *sgl, unsigned int nents,
		       size_t buflen, off_t skip);

/*
 * Maximum number of entries that will be allocated in one piece, if
 * a list larger than this is required then chaining will be utilized.
 */
#define SG_MAX_SINGLE_ALLOC		(PAGE_SIZE / sizeof(struct scatterlist))

/*
 * The maximum number of SG segments that we will put inside a
 * scatterlist (unless chaining is used). Should ideally fit inside a
 * single page, to avoid a higher order allocation.  We could define this
 * to SG_MAX_SINGLE_ALLOC to pack correctly at the highest order.  The
 * minimum value is 32
 */
#define SG_CHUNK_SIZE	128

/*
 * Like SG_CHUNK_SIZE, but for archs that have sg chaining. This limit
 * is totally arbitrary, a setting of 2048 will get you at least 8mb ios.
 */
#ifdef CONFIG_ARCH_NO_SG_CHAIN
#define SG_MAX_SEGMENTS	SG_CHUNK_SIZE
#else
#define SG_MAX_SEGMENTS	2048
#endif

#ifdef CONFIG_SG_POOL
void sg_free_table_chained(struct sg_table *table,
			   unsigned nents_first_chunk);
int sg_alloc_table_chained(struct sg_table *table, int nents,
			   struct scatterlist *first_chunk,
			   unsigned nents_first_chunk);
#endif

/*
 * sg page iterator
 *
 * Iterates over sg entries page-by-page.  On each successful iteration, you
 * can call sg_page_iter_page(@piter) to get the current page.
 * @piter->sg will point to the sg holding this page and @piter->sg_pgoffset to
 * the page's page offset within the sg. The iteration will stop either when a
 * maximum number of sg entries was reached or a terminating sg
 * (sg_last(sg) == true) was reached.
 */
struct sg_page_iter {
	struct scatterlist	*sg;		/* sg holding the page */
	unsigned int		sg_pgoffset;	/* page offset within the sg */

	/* these are internal states, keep away */
	unsigned int		__nents;	/* remaining sg entries */
	int			__pg_advance;	/* nr pages to advance at the
						 * next step */
};

/*
 * sg page iterator for DMA addresses
 *
 * This is the same as sg_page_iter however you can call
 * sg_page_iter_dma_address(@dma_iter) to get the page's DMA
 * address. sg_page_iter_page() cannot be called on this iterator.
 */
struct sg_dma_page_iter {
	struct sg_page_iter base;
};

bool __sg_page_iter_next(struct sg_page_iter *piter);
bool __sg_page_iter_dma_next(struct sg_dma_page_iter *dma_iter);
void __sg_page_iter_start(struct sg_page_iter *piter,
			  struct scatterlist *sglist, unsigned int nents,
			  unsigned long pgoffset);
/**
 * sg_page_iter_page - get the current page held by the page iterator
 * @piter:	page iterator holding the page
 */
static inline struct page *sg_page_iter_page(struct sg_page_iter *piter)
{
	return nth_page(sg_page(piter->sg), piter->sg_pgoffset);
}

/**
 * sg_page_iter_dma_address - get the dma address of the current page held by
 * the page iterator.
 * @dma_iter:	page iterator holding the page
 */
static inline dma_addr_t
sg_page_iter_dma_address(struct sg_dma_page_iter *dma_iter)
{
	return sg_dma_address(dma_iter->base.sg) +
	       (dma_iter->base.sg_pgoffset << PAGE_SHIFT);
}

/**
 * for_each_sg_page - iterate over the pages of the given sg list
 * @sglist:	sglist to iterate over
 * @piter:	page iterator to hold current page, sg, sg_pgoffset
 * @nents:	maximum number of sg entries to iterate over
 * @pgoffset:	starting page offset (in pages)
 *
 * Callers may use sg_page_iter_page() to get each page pointer.
 * In each loop it operates on PAGE_SIZE unit.
 */
#define for_each_sg_page(sglist, piter, nents, pgoffset)		   \
	for (__sg_page_iter_start((piter), (sglist), (nents), (pgoffset)); \
	     __sg_page_iter_next(piter);)

/**
 * for_each_sg_dma_page - iterate over the pages of the given sg list
 * @sglist:	sglist to iterate over
 * @dma_iter:	DMA page iterator to hold current page
 * @dma_nents:	maximum number of sg entries to iterate over, this is the value
 *              returned from dma_map_sg
 * @pgoffset:	starting page offset (in pages)
 *
 * Callers may use sg_page_iter_dma_address() to get each page's DMA address.
 * In each loop it operates on PAGE_SIZE unit.
 */
#define for_each_sg_dma_page(sglist, dma_iter, dma_nents, pgoffset)            \
	for (__sg_page_iter_start(&(dma_iter)->base, sglist, dma_nents,        \
				  pgoffset);                                   \
	     __sg_page_iter_dma_next(dma_iter);)

/**
 * for_each_sgtable_page - iterate over all pages in the sg_table object
 * @sgt:	sg_table object to iterate over
 * @piter:	page iterator to hold current page
 * @pgoffset:	starting page offset (in pages)
 *
 * Iterates over the all memory pages in the buffer described by
 * a scatterlist stored in the given sg_table object.
 * See also for_each_sg_page(). In each loop it operates on PAGE_SIZE unit.
 */
#define for_each_sgtable_page(sgt, piter, pgoffset)	\
	for_each_sg_page((sgt)->sgl, piter, (sgt)->orig_nents, pgoffset)

/**
 * for_each_sgtable_dma_page - iterate over the DMA mapped sg_table object
 * @sgt:	sg_table object to iterate over
 * @dma_iter:	DMA page iterator to hold current page
 * @pgoffset:	starting page offset (in pages)
 *
 * Iterates over the all DMA mapped pages in the buffer described by
 * a scatterlist stored in the given sg_table object.
 * See also for_each_sg_dma_page(). In each loop it operates on PAGE_SIZE
 * unit.
 */
#define for_each_sgtable_dma_page(sgt, dma_iter, pgoffset)	\
	for_each_sg_dma_page((sgt)->sgl, dma_iter, (sgt)->nents, pgoffset)


/*
 * Mapping sg iterator
 *
 * Iterates over sg entries mapping page-by-page.  On each successful
 * iteration, @miter->page points to the mapped page and
 * @miter->length bytes of data can be accessed at @miter->addr.  As
 * long as an interation is enclosed between start and stop, the user
 * is free to choose control structure and when to stop.
 *
 * @miter->consumed is set to @miter->length on each iteration.  It
 * can be adjusted if the user can't consume all the bytes in one go.
 * Also, a stopped iteration can be resumed by calling next on it.
 * This is useful when iteration needs to release all resources and
 * continue later (e.g. at the next interrupt).
 */

#define SG_MITER_ATOMIC		(1 << 0)	 /* use kmap_atomic */
#define SG_MITER_TO_SG		(1 << 1)	/* flush back to phys on unmap */
#define SG_MITER_FROM_SG	(1 << 2)	/* nop */

struct sg_mapping_iter {
	/* the following three fields can be accessed directly */
	struct page		*page;		/* currently mapped page */
	void			*addr;		/* pointer to the mapped area */
	size_t			length;		/* length of the mapped area */
	size_t			consumed;	/* number of consumed bytes */
	struct sg_page_iter	piter;		/* page iterator */

	/* these are internal states, keep away */
	unsigned int		__offset;	/* offset within page */
	unsigned int		__remaining;	/* remaining bytes on page */
	unsigned int		__flags;
};

void sg_miter_start(struct sg_mapping_iter *miter, struct scatterlist *sgl,
		    unsigned int nents, unsigned int flags);
bool sg_miter_skip(struct sg_mapping_iter *miter, off_t offset);
bool sg_miter_next(struct sg_mapping_iter *miter);
void sg_miter_stop(struct sg_mapping_iter *miter);

#endif /* _LINUX_SCATTERLIST_H */
