// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2008 Oracle.  All rights reserved.
 */

#include <linux/kernel.h>
#include <linux/bio.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/pagemap.h>
#include <linux/highmem.h>
#include <linux/time.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/backing-dev.h>
#include <linux/writeback.h>
#include <linux/slab.h>
#include <linux/sched/mm.h>
#include <linux/log2.h>
#include <crypto/hash.h>
#include "misc.h"
#include "ctree.h"
#include "disk-io.h"
#include "transaction.h"
#include "btrfs_inode.h"
#include "volumes.h"
#include "ordered-data.h"
#include "compression.h"
#include "extent_io.h"
#include "extent_map.h"
#include "zoned.h"

static const char* const btrfs_compress_types[] = { "", "zlib", "lzo", "zstd" };

const char* btrfs_compress_type2str(enum btrfs_compression_type type)
{
	switch (type) {
	case BTRFS_COMPRESS_ZLIB:
	case BTRFS_COMPRESS_LZO:
	case BTRFS_COMPRESS_ZSTD:
	case BTRFS_COMPRESS_NONE:
		return btrfs_compress_types[type];
	default:
		break;
	}

	return NULL;
}

bool btrfs_compress_is_valid_type(const char *str, size_t len)
{
	int i;

	for (i = 1; i < ARRAY_SIZE(btrfs_compress_types); i++) {
		size_t comp_len = strlen(btrfs_compress_types[i]);

		if (len < comp_len)
			continue;

		if (!strncmp(btrfs_compress_types[i], str, comp_len))
			return true;
	}
	return false;
}

static int compression_compress_pages(int type, struct list_head *ws,
               struct address_space *mapping, u64 start, struct page **pages,
               unsigned long *out_pages, unsigned long *total_in,
               unsigned long *total_out)
{
	switch (type) {
	case BTRFS_COMPRESS_ZLIB:
		return zlib_compress_pages(ws, mapping, start, pages,
				out_pages, total_in, total_out);
	case BTRFS_COMPRESS_LZO:
		return lzo_compress_pages(ws, mapping, start, pages,
				out_pages, total_in, total_out);
	case BTRFS_COMPRESS_ZSTD:
		return zstd_compress_pages(ws, mapping, start, pages,
				out_pages, total_in, total_out);
	case BTRFS_COMPRESS_NONE:
	default:
		/*
		 * This can happen when compression races with remount setting
		 * it to 'no compress', while caller doesn't call
		 * inode_need_compress() to check if we really need to
		 * compress.
		 *
		 * Not a big deal, just need to inform caller that we
		 * haven't allocated any pages yet.
		 */
		*out_pages = 0;
		return -E2BIG;
	}
}

static int compression_decompress_bio(int type, struct list_head *ws,
		struct compressed_bio *cb)
{
	switch (type) {
	case BTRFS_COMPRESS_ZLIB: return zlib_decompress_bio(ws, cb);
	case BTRFS_COMPRESS_LZO:  return lzo_decompress_bio(ws, cb);
	case BTRFS_COMPRESS_ZSTD: return zstd_decompress_bio(ws, cb);
	case BTRFS_COMPRESS_NONE:
	default:
		/*
		 * This can't happen, the type is validated several times
		 * before we get here.
		 */
		BUG();
	}
}

static int compression_decompress(int type, struct list_head *ws,
               unsigned char *data_in, struct page *dest_page,
               unsigned long start_byte, size_t srclen, size_t destlen)
{
	switch (type) {
	case BTRFS_COMPRESS_ZLIB: return zlib_decompress(ws, data_in, dest_page,
						start_byte, srclen, destlen);
	case BTRFS_COMPRESS_LZO:  return lzo_decompress(ws, data_in, dest_page,
						start_byte, srclen, destlen);
	case BTRFS_COMPRESS_ZSTD: return zstd_decompress(ws, data_in, dest_page,
						start_byte, srclen, destlen);
	case BTRFS_COMPRESS_NONE:
	default:
		/*
		 * This can't happen, the type is validated several times
		 * before we get here.
		 */
		BUG();
	}
}

static int btrfs_decompress_bio(struct compressed_bio *cb);

static inline int compressed_bio_size(struct btrfs_fs_info *fs_info,
				      unsigned long disk_size)
{
	return sizeof(struct compressed_bio) +
		(DIV_ROUND_UP(disk_size, fs_info->sectorsize)) * fs_info->csum_size;
}

static int check_compressed_csum(struct btrfs_inode *inode, struct bio *bio,
				 u64 disk_start)
{
	struct btrfs_fs_info *fs_info = inode->root->fs_info;
	SHASH_DESC_ON_STACK(shash, fs_info->csum_shash);
	const u32 csum_size = fs_info->csum_size;
	const u32 sectorsize = fs_info->sectorsize;
	struct page *page;
	unsigned long i;
	char *kaddr;
	u8 csum[BTRFS_CSUM_SIZE];
	struct compressed_bio *cb = bio->bi_private;
	u8 *cb_sum = cb->sums;

	if (!fs_info->csum_root || (inode->flags & BTRFS_INODE_NODATASUM))
		return 0;

	shash->tfm = fs_info->csum_shash;

	for (i = 0; i < cb->nr_pages; i++) {
		u32 pg_offset;
		u32 bytes_left = PAGE_SIZE;
		page = cb->compressed_pages[i];

		/* Determine the remaining bytes inside the page first */
		if (i == cb->nr_pages - 1)
			bytes_left = cb->compressed_len - i * PAGE_SIZE;

		/* Hash through the page sector by sector */
		for (pg_offset = 0; pg_offset < bytes_left;
		     pg_offset += sectorsize) {
			kaddr = kmap_atomic(page);
			crypto_shash_digest(shash, kaddr + pg_offset,
					    sectorsize, csum);
			kunmap_atomic(kaddr);

			if (memcmp(&csum, cb_sum, csum_size) != 0) {
				btrfs_print_data_csum_error(inode, disk_start,
						csum, cb_sum, cb->mirror_num);
				if (btrfs_io_bio(bio)->device)
					btrfs_dev_stat_inc_and_print(
						btrfs_io_bio(bio)->device,
						BTRFS_DEV_STAT_CORRUPTION_ERRS);
				return -EIO;
			}
			cb_sum += csum_size;
			disk_start += sectorsize;
		}
	}
	return 0;
}

/* when we finish reading compressed pages from the disk, we
 * decompress them and then run the bio end_io routines on the
 * decompressed pages (in the inode address space).
 *
 * This allows the checksumming and other IO error handling routines
 * to work normally
 *
 * The compressed pages are freed here, and it must be run
 * in process context
 */
static void end_compressed_bio_read(struct bio *bio)
{
	struct compressed_bio *cb = bio->bi_private;
	struct inode *inode;
	struct page *page;
	unsigned long index;
	unsigned int mirror = btrfs_io_bio(bio)->mirror_num;
	int ret = 0;

	if (bio->bi_status)
		cb->errors = 1;

	/* if there are more bios still pending for this compressed
	 * extent, just exit
	 */
	if (!refcount_dec_and_test(&cb->pending_bios))
		goto out;

	/*
	 * Record the correct mirror_num in cb->orig_bio so that
	 * read-repair can work properly.
	 */
	btrfs_io_bio(cb->orig_bio)->mirror_num = mirror;
	cb->mirror_num = mirror;

	/*
	 * Some IO in this cb have failed, just skip checksum as there
	 * is no way it could be correct.
	 */
	if (cb->errors == 1)
		goto csum_failed;

	inode = cb->inode;
	ret = check_compressed_csum(BTRFS_I(inode), bio,
				    bio->bi_iter.bi_sector << 9);
	if (ret)
		goto csum_failed;

	/* ok, we're the last bio for this extent, lets start
	 * the decompression.
	 */
	ret = btrfs_decompress_bio(cb);

csum_failed:
	if (ret)
		cb->errors = 1;

	/* release the compressed pages */
	index = 0;
	for (index = 0; index < cb->nr_pages; index++) {
		page = cb->compressed_pages[index];
		page->mapping = NULL;
		put_page(page);
	}

	/* do io completion on the original bio */
	if (cb->errors) {
		bio_io_error(cb->orig_bio);
	} else {
		struct bio_vec *bvec;
		struct bvec_iter_all iter_all;

		/*
		 * we have verified the checksum already, set page
		 * checked so the end_io handlers know about it
		 */
		ASSERT(!bio_flagged(bio, BIO_CLONED));
		bio_for_each_segment_all(bvec, cb->orig_bio, iter_all)
			SetPageChecked(bvec->bv_page);

		bio_endio(cb->orig_bio);
	}

	/* finally free the cb struct */
	kfree(cb->compressed_pages);
	kfree(cb);
out:
	bio_put(bio);
}

/*
 * Clear the writeback bits on all of the file
 * pages for a compressed write
 */
static noinline void end_compressed_writeback(struct inode *inode,
					      const struct compressed_bio *cb)
{
	unsigned long index = cb->start >> PAGE_SHIFT;
	unsigned long end_index = (cb->start + cb->len - 1) >> PAGE_SHIFT;
	struct page *pages[16];
	unsigned long nr_pages = end_index - index + 1;
	int i;
	int ret;

	if (cb->errors)
		mapping_set_error(inode->i_mapping, -EIO);

	while (nr_pages > 0) {
		ret = find_get_pages_contig(inode->i_mapping, index,
				     min_t(unsigned long,
				     nr_pages, ARRAY_SIZE(pages)), pages);
		if (ret == 0) {
			nr_pages -= 1;
			index += 1;
			continue;
		}
		for (i = 0; i < ret; i++) {
			if (cb->errors)
				SetPageError(pages[i]);
			end_page_writeback(pages[i]);
			put_page(pages[i]);
		}
		nr_pages -= ret;
		index += ret;
	}
	/* the inode may be gone now */
}

/*
 * do the cleanup once all the compressed pages hit the disk.
 * This will clear writeback on the file pages and free the compressed
 * pages.
 *
 * This also calls the writeback end hooks for the file pages so that
 * metadata and checksums can be updated in the file.
 */
static void end_compressed_bio_write(struct bio *bio)
{
	struct compressed_bio *cb = bio->bi_private;
	struct inode *inode;
	struct page *page;
	unsigned long index;

	if (bio->bi_status)
		cb->errors = 1;

	/* if there are more bios still pending for this compressed
	 * extent, just exit
	 */
	if (!refcount_dec_and_test(&cb->pending_bios))
		goto out;

	/* ok, we're the last bio for this extent, step one is to
	 * call back into the FS and do all the end_io operations
	 */
	inode = cb->inode;
	cb->compressed_pages[0]->mapping = cb->inode->i_mapping;
	btrfs_record_physical_zoned(inode, cb->start, bio);
	btrfs_writepage_endio_finish_ordered(cb->compressed_pages[0],
			cb->start, cb->start + cb->len - 1,
			bio->bi_status == BLK_STS_OK);
	cb->compressed_pages[0]->mapping = NULL;

	end_compressed_writeback(inode, cb);
	/* note, our inode could be gone now */

	/*
	 * release the compressed pages, these came from alloc_page and
	 * are not attached to the inode at all
	 */
	index = 0;
	for (index = 0; index < cb->nr_pages; index++) {
		page = cb->compressed_pages[index];
		page->mapping = NULL;
		put_page(page);
	}

	/* finally free the cb struct */
	kfree(cb->compressed_pages);
	kfree(cb);
out:
	bio_put(bio);
}

/*
 * worker function to build and submit bios for previously compressed pages.
 * The corresponding pages in the inode should be marked for writeback
 * and the compressed pages should have a reference on them for dropping
 * when the IO is complete.
 *
 * This also checksums the file bytes and gets things ready for
 * the end io hooks.
 */
blk_status_t btrfs_submit_compressed_write(struct btrfs_inode *inode, u64 start,
				 unsigned long len, u64 disk_start,
				 unsigned long compressed_len,
				 struct page **compressed_pages,
				 unsigned long nr_pages,
				 unsigned int write_flags,
				 struct cgroup_subsys_state *blkcg_css)
{
	struct btrfs_fs_info *fs_info = inode->root->fs_info;
	struct bio *bio = NULL;
	struct compressed_bio *cb;
	unsigned long bytes_left;
	int pg_index = 0;
	struct page *page;
	u64 first_byte = disk_start;
	blk_status_t ret;
	int skip_sum = inode->flags & BTRFS_INODE_NODATASUM;
	const bool use_append = btrfs_use_zone_append(inode, disk_start);
	const unsigned int bio_op = use_append ? REQ_OP_ZONE_APPEND : REQ_OP_WRITE;

	WARN_ON(!PAGE_ALIGNED(start));
	cb = kmalloc(compressed_bio_size(fs_info, compressed_len), GFP_NOFS);
	if (!cb)
		return BLK_STS_RESOURCE;
	refcount_set(&cb->pending_bios, 0);
	cb->errors = 0;
	cb->inode = &inode->vfs_inode;
	cb->start = start;
	cb->len = len;
	cb->mirror_num = 0;
	cb->compressed_pages = compressed_pages;
	cb->compressed_len = compressed_len;
	cb->orig_bio = NULL;
	cb->nr_pages = nr_pages;

	bio = btrfs_bio_alloc(first_byte);
	bio->bi_opf = bio_op | write_flags;
	bio->bi_private = cb;
	bio->bi_end_io = end_compressed_bio_write;

	if (use_append) {
		struct extent_map *em;
		struct map_lookup *map;
		struct block_device *bdev;

		em = btrfs_get_chunk_map(fs_info, disk_start, PAGE_SIZE);
		if (IS_ERR(em)) {
			kfree(cb);
			bio_put(bio);
			return BLK_STS_NOTSUPP;
		}

		map = em->map_lookup;
		/* We only support single profile for now */
		ASSERT(map->num_stripes == 1);
		bdev = map->stripes[0].dev->bdev;

		bio_set_dev(bio, bdev);
		free_extent_map(em);
	}

	if (blkcg_css) {
		bio->bi_opf |= REQ_CGROUP_PUNT;
		kthread_associate_blkcg(blkcg_css);
	}
	refcount_set(&cb->pending_bios, 1);

	/* create and submit bios for the compressed pages */
	bytes_left = compressed_len;
	for (pg_index = 0; pg_index < cb->nr_pages; pg_index++) {
		int submit = 0;
		int len = 0;

		page = compressed_pages[pg_index];
		page->mapping = inode->vfs_inode.i_mapping;
		if (bio->bi_iter.bi_size)
			submit = btrfs_bio_fits_in_stripe(page, PAGE_SIZE, bio,
							  0);

		/*
		 * Page can only be added to bio if the current bio fits in
		 * stripe.
		 */
		if (!submit) {
			if (pg_index == 0 && use_append)
				len = bio_add_zone_append_page(bio, page,
							       PAGE_SIZE, 0);
			else
				len = bio_add_page(bio, page, PAGE_SIZE, 0);
		}

		page->mapping = NULL;
		if (submit || len < PAGE_SIZE) {
			/*
			 * inc the count before we submit the bio so
			 * we know the end IO handler won't happen before
			 * we inc the count.  Otherwise, the cb might get
			 * freed before we're done setting it up
			 */
			refcount_inc(&cb->pending_bios);
			ret = btrfs_bio_wq_end_io(fs_info, bio,
						  BTRFS_WQ_ENDIO_DATA);
			BUG_ON(ret); /* -ENOMEM */

			if (!skip_sum) {
				ret = btrfs_csum_one_bio(inode, bio, start, 1);
				BUG_ON(ret); /* -ENOMEM */
			}

			ret = btrfs_map_bio(fs_info, bio, 0);
			if (ret) {
				bio->bi_status = ret;
				bio_endio(bio);
			}

			bio = btrfs_bio_alloc(first_byte);
			bio->bi_opf = bio_op | write_flags;
			bio->bi_private = cb;
			bio->bi_end_io = end_compressed_bio_write;
			if (blkcg_css)
				bio->bi_opf |= REQ_CGROUP_PUNT;
			/*
			 * Use bio_add_page() to ensure the bio has at least one
			 * page.
			 */
			bio_add_page(bio, page, PAGE_SIZE, 0);
		}
		if (bytes_left < PAGE_SIZE) {
			btrfs_info(fs_info,
					"bytes left %lu compress len %lu nr %lu",
			       bytes_left, cb->compressed_len, cb->nr_pages);
		}
		bytes_left -= PAGE_SIZE;
		first_byte += PAGE_SIZE;
		cond_resched();
	}

	ret = btrfs_bio_wq_end_io(fs_info, bio, BTRFS_WQ_ENDIO_DATA);
	BUG_ON(ret); /* -ENOMEM */

	if (!skip_sum) {
		ret = btrfs_csum_one_bio(inode, bio, start, 1);
		BUG_ON(ret); /* -ENOMEM */
	}

	ret = btrfs_map_bio(fs_info, bio, 0);
	if (ret) {
		bio->bi_status = ret;
		bio_endio(bio);
	}

	if (blkcg_css)
		kthread_associate_blkcg(NULL);

	return 0;
}

static u64 bio_end_offset(struct bio *bio)
{
	struct bio_vec *last = bio_last_bvec_all(bio);

	return page_offset(last->bv_page) + last->bv_len + last->bv_offset;
}

static noinline int add_ra_bio_pages(struct inode *inode,
				     u64 compressed_end,
				     struct compressed_bio *cb)
{
	unsigned long end_index;
	unsigned long pg_index;
	u64 last_offset;
	u64 isize = i_size_read(inode);
	int ret;
	struct page *page;
	unsigned long nr_pages = 0;
	struct extent_map *em;
	struct address_space *mapping = inode->i_mapping;
	struct extent_map_tree *em_tree;
	struct extent_io_tree *tree;
	u64 end;
	int misses = 0;

	last_offset = bio_end_offset(cb->orig_bio);
	em_tree = &BTRFS_I(inode)->extent_tree;
	tree = &BTRFS_I(inode)->io_tree;

	if (isize == 0)
		return 0;

	end_index = (i_size_read(inode) - 1) >> PAGE_SHIFT;

	while (last_offset < compressed_end) {
		pg_index = last_offset >> PAGE_SHIFT;

		if (pg_index > end_index)
			break;

		page = xa_load(&mapping->i_pages, pg_index);
		if (page && !xa_is_value(page)) {
			misses++;
			if (misses > 4)
				break;
			goto next;
		}

		page = __page_cache_alloc(mapping_gfp_constraint(mapping,
								 ~__GFP_FS));
		if (!page)
			break;

		if (add_to_page_cache_lru(page, mapping, pg_index, GFP_NOFS)) {
			put_page(page);
			goto next;
		}

		/*
		 * at this point, we have a locked page in the page cache
		 * for these bytes in the file.  But, we have to make
		 * sure they map to this compressed extent on disk.
		 */
		ret = set_page_extent_mapped(page);
		if (ret < 0) {
			unlock_page(page);
			put_page(page);
			break;
		}

		end = last_offset + PAGE_SIZE - 1;
		lock_extent(tree, last_offset, end);
		read_lock(&em_tree->lock);
		em = lookup_extent_mapping(em_tree, last_offset,
					   PAGE_SIZE);
		read_unlock(&em_tree->lock);

		if (!em || last_offset < em->start ||
		    (last_offset + PAGE_SIZE > extent_map_end(em)) ||
		    (em->block_start >> 9) != cb->orig_bio->bi_iter.bi_sector) {
			free_extent_map(em);
			unlock_extent(tree, last_offset, end);
			unlock_page(page);
			put_page(page);
			break;
		}
		free_extent_map(em);

		if (page->index == end_index) {
			size_t zero_offset = offset_in_page(isize);

			if (zero_offset) {
				int zeros;
				zeros = PAGE_SIZE - zero_offset;
				memzero_page(page, zero_offset, zeros);
				flush_dcache_page(page);
			}
		}

		ret = bio_add_page(cb->orig_bio, page,
				   PAGE_SIZE, 0);

		if (ret == PAGE_SIZE) {
			nr_pages++;
			put_page(page);
		} else {
			unlock_extent(tree, last_offset, end);
			unlock_page(page);
			put_page(page);
			break;
		}
next:
		last_offset += PAGE_SIZE;
	}
	return 0;
}

/*
 * for a compressed read, the bio we get passed has all the inode pages
 * in it.  We don't actually do IO on those pages but allocate new ones
 * to hold the compressed pages on disk.
 *
 * bio->bi_iter.bi_sector points to the compressed extent on disk
 * bio->bi_io_vec points to all of the inode pages
 *
 * After the compressed pages are read, we copy the bytes into the
 * bio we were passed and then call the bio end_io calls
 */
blk_status_t btrfs_submit_compressed_read(struct inode *inode, struct bio *bio,
				 int mirror_num, unsigned long bio_flags)
{
	struct btrfs_fs_info *fs_info = btrfs_sb(inode->i_sb);
	struct extent_map_tree *em_tree;
	struct compressed_bio *cb;
	unsigned long compressed_len;
	unsigned long nr_pages;
	unsigned long pg_index;
	struct page *page;
	struct bio *comp_bio;
	u64 cur_disk_byte = bio->bi_iter.bi_sector << 9;
	u64 em_len;
	u64 em_start;
	struct extent_map *em;
	blk_status_t ret = BLK_STS_RESOURCE;
	int faili = 0;
	u8 *sums;

	em_tree = &BTRFS_I(inode)->extent_tree;

	/* we need the actual starting offset of this extent in the file */
	read_lock(&em_tree->lock);
	em = lookup_extent_mapping(em_tree,
				   page_offset(bio_first_page_all(bio)),
				   fs_info->sectorsize);
	read_unlock(&em_tree->lock);
	if (!em)
		return BLK_STS_IOERR;

	compressed_len = em->block_len;
	cb = kmalloc(compressed_bio_size(fs_info, compressed_len), GFP_NOFS);
	if (!cb)
		goto out;

	refcount_set(&cb->pending_bios, 0);
	cb->errors = 0;
	cb->inode = inode;
	cb->mirror_num = mirror_num;
	sums = cb->sums;

	cb->start = em->orig_start;
	em_len = em->len;
	em_start = em->start;

	free_extent_map(em);
	em = NULL;

	cb->len = bio->bi_iter.bi_size;
	cb->compressed_len = compressed_len;
	cb->compress_type = extent_compress_type(bio_flags);
	cb->orig_bio = bio;

	nr_pages = DIV_ROUND_UP(compressed_len, PAGE_SIZE);
	cb->compressed_pages = kcalloc(nr_pages, sizeof(struct page *),
				       GFP_NOFS);
	if (!cb->compressed_pages)
		goto fail1;

	for (pg_index = 0; pg_index < nr_pages; pg_index++) {
		cb->compressed_pages[pg_index] = alloc_page(GFP_NOFS |
							      __GFP_HIGHMEM);
		if (!cb->compressed_pages[pg_index]) {
			faili = pg_index - 1;
			ret = BLK_STS_RESOURCE;
			goto fail2;
		}
	}
	faili = nr_pages - 1;
	cb->nr_pages = nr_pages;

	add_ra_bio_pages(inode, em_start + em_len, cb);

	/* include any pages we added in add_ra-bio_pages */
	cb->len = bio->bi_iter.bi_size;

	comp_bio = btrfs_bio_alloc(cur_disk_byte);
	comp_bio->bi_opf = REQ_OP_READ;
	comp_bio->bi_private = cb;
	comp_bio->bi_end_io = end_compressed_bio_read;
	refcount_set(&cb->pending_bios, 1);

	for (pg_index = 0; pg_index < nr_pages; pg_index++) {
		u32 pg_len = PAGE_SIZE;
		int submit = 0;

		/*
		 * To handle subpage case, we need to make sure the bio only
		 * covers the range we need.
		 *
		 * If we're at the last page, truncate the length to only cover
		 * the remaining part.
		 */
		if (pg_index == nr_pages - 1)
			pg_len = min_t(u32, PAGE_SIZE,
					compressed_len - pg_index * PAGE_SIZE);

		page = cb->compressed_pages[pg_index];
		page->mapping = inode->i_mapping;
		page->index = em_start >> PAGE_SHIFT;

		if (comp_bio->bi_iter.bi_size)
			submit = btrfs_bio_fits_in_stripe(page, pg_len,
							  comp_bio, 0);

		page->mapping = NULL;
		if (submit || bio_add_page(comp_bio, page, pg_len, 0) < pg_len) {
			unsigned int nr_sectors;

			ret = btrfs_bio_wq_end_io(fs_info, comp_bio,
						  BTRFS_WQ_ENDIO_DATA);
			BUG_ON(ret); /* -ENOMEM */

			/*
			 * inc the count before we submit the bio so
			 * we know the end IO handler won't happen before
			 * we inc the count.  Otherwise, the cb might get
			 * freed before we're done setting it up
			 */
			refcount_inc(&cb->pending_bios);

			ret = btrfs_lookup_bio_sums(inode, comp_bio, sums);
			BUG_ON(ret); /* -ENOMEM */

			nr_sectors = DIV_ROUND_UP(comp_bio->bi_iter.bi_size,
						  fs_info->sectorsize);
			sums += fs_info->csum_size * nr_sectors;

			ret = btrfs_map_bio(fs_info, comp_bio, mirror_num);
			if (ret) {
				comp_bio->bi_status = ret;
				bio_endio(comp_bio);
			}

			comp_bio = btrfs_bio_alloc(cur_disk_byte);
			comp_bio->bi_opf = REQ_OP_READ;
			comp_bio->bi_private = cb;
			comp_bio->bi_end_io = end_compressed_bio_read;

			bio_add_page(comp_bio, page, pg_len, 0);
		}
		cur_disk_byte += pg_len;
	}

	ret = btrfs_bio_wq_end_io(fs_info, comp_bio, BTRFS_WQ_ENDIO_DATA);
	BUG_ON(ret); /* -ENOMEM */

	ret = btrfs_lookup_bio_sums(inode, comp_bio, sums);
	BUG_ON(ret); /* -ENOMEM */

	ret = btrfs_map_bio(fs_info, comp_bio, mirror_num);
	if (ret) {
		comp_bio->bi_status = ret;
		bio_endio(comp_bio);
	}

	return 0;

fail2:
	while (faili >= 0) {
		__free_page(cb->compressed_pages[faili]);
		faili--;
	}

	kfree(cb->compressed_pages);
fail1:
	kfree(cb);
out:
	free_extent_map(em);
	return ret;
}

/*
 * Heuristic uses systematic sampling to collect data from the input data
 * range, the logic can be tuned by the following constants:
 *
 * @SAMPLING_READ_SIZE - how many bytes will be copied from for each sample
 * @SAMPLING_INTERVAL  - range from which the sampled data can be collected
 */
#define SAMPLING_READ_SIZE	(16)
#define SAMPLING_INTERVAL	(256)

/*
 * For statistical analysis of the input data we consider bytes that form a
 * Galois Field of 256 objects. Each object has an attribute count, ie. how
 * many times the object appeared in the sample.
 */
#define BUCKET_SIZE		(256)

/*
 * The size of the sample is based on a statistical sampling rule of thumb.
 * The common way is to perform sampling tests as long as the number of
 * elements in each cell is at least 5.
 *
 * Instead of 5, we choose 32 to obtain more accurate results.
 * If the data contain the maximum number of symbols, which is 256, we obtain a
 * sample size bound by 8192.
 *
 * For a sample of at most 8KB of data per data range: 16 consecutive bytes
 * from up to 512 locations.
 */
#define MAX_SAMPLE_SIZE		(BTRFS_MAX_UNCOMPRESSED *		\
				 SAMPLING_READ_SIZE / SAMPLING_INTERVAL)

struct bucket_item {
	u32 count;
};

struct heuristic_ws {
	/* Partial copy of input data */
	u8 *sample;
	u32 sample_size;
	/* Buckets store counters for each byte value */
	struct bucket_item *bucket;
	/* Sorting buffer */
	struct bucket_item *bucket_b;
	struct list_head list;
};

static struct workspace_manager heuristic_wsm;

static void free_heuristic_ws(struct list_head *ws)
{
	struct heuristic_ws *workspace;

	workspace = list_entry(ws, struct heuristic_ws, list);

	kvfree(workspace->sample);
	kfree(workspace->bucket);
	kfree(workspace->bucket_b);
	kfree(workspace);
}

static struct list_head *alloc_heuristic_ws(unsigned int level)
{
	struct heuristic_ws *ws;

	ws = kzalloc(sizeof(*ws), GFP_KERNEL);
	if (!ws)
		return ERR_PTR(-ENOMEM);

	ws->sample = kvmalloc(MAX_SAMPLE_SIZE, GFP_KERNEL);
	if (!ws->sample)
		goto fail;

	ws->bucket = kcalloc(BUCKET_SIZE, sizeof(*ws->bucket), GFP_KERNEL);
	if (!ws->bucket)
		goto fail;

	ws->bucket_b = kcalloc(BUCKET_SIZE, sizeof(*ws->bucket_b), GFP_KERNEL);
	if (!ws->bucket_b)
		goto fail;

	INIT_LIST_HEAD(&ws->list);
	return &ws->list;
fail:
	free_heuristic_ws(&ws->list);
	return ERR_PTR(-ENOMEM);
}

const struct btrfs_compress_op btrfs_heuristic_compress = {
	.workspace_manager = &heuristic_wsm,
};

static const struct btrfs_compress_op * const btrfs_compress_op[] = {
	/* The heuristic is represented as compression type 0 */
	&btrfs_heuristic_compress,
	&btrfs_zlib_compress,
	&btrfs_lzo_compress,
	&btrfs_zstd_compress,
};

static struct list_head *alloc_workspace(int type, unsigned int level)
{
	switch (type) {
	case BTRFS_COMPRESS_NONE: return alloc_heuristic_ws(level);
	case BTRFS_COMPRESS_ZLIB: return zlib_alloc_workspace(level);
	case BTRFS_COMPRESS_LZO:  return lzo_alloc_workspace(level);
	case BTRFS_COMPRESS_ZSTD: return zstd_alloc_workspace(level);
	default:
		/*
		 * This can't happen, the type is validated several times
		 * before we get here.
		 */
		BUG();
	}
}

static void free_workspace(int type, struct list_head *ws)
{
	switch (type) {
	case BTRFS_COMPRESS_NONE: return free_heuristic_ws(ws);
	case BTRFS_COMPRESS_ZLIB: return zlib_free_workspace(ws);
	case BTRFS_COMPRESS_LZO:  return lzo_free_workspace(ws);
	case BTRFS_COMPRESS_ZSTD: return zstd_free_workspace(ws);
	default:
		/*
		 * This can't happen, the type is validated several times
		 * before we get here.
		 */
		BUG();
	}
}

static void btrfs_init_workspace_manager(int type)
{
	struct workspace_manager *wsm;
	struct list_head *workspace;

	wsm = btrfs_compress_op[type]->workspace_manager;
	INIT_LIST_HEAD(&wsm->idle_ws);
	spin_lock_init(&wsm->ws_lock);
	atomic_set(&wsm->total_ws, 0);
	init_waitqueue_head(&wsm->ws_wait);

	/*
	 * Preallocate one workspace for each compression type so we can
	 * guarantee forward progress in the worst case
	 */
	workspace = alloc_workspace(type, 0);
	if (IS_ERR(workspace)) {
		pr_warn(
	"BTRFS: cannot preallocate compression workspace, will try later\n");
	} else {
		atomic_set(&wsm->total_ws, 1);
		wsm->free_ws = 1;
		list_add(workspace, &wsm->idle_ws);
	}
}

static void btrfs_cleanup_workspace_manager(int type)
{
	struct workspace_manager *wsman;
	struct list_head *ws;

	wsman = btrfs_compress_op[type]->workspace_manager;
	while (!list_empty(&wsman->idle_ws)) {
		ws = wsman->idle_ws.next;
		list_del(ws);
		free_workspace(type, ws);
		atomic_dec(&wsman->total_ws);
	}
}

/*
 * This finds an available workspace or allocates a new one.
 * If it's not possible to allocate a new one, waits until there's one.
 * Preallocation makes a forward progress guarantees and we do not return
 * errors.
 */
struct list_head *btrfs_get_workspace(int type, unsigned int level)
{
	struct workspace_manager *wsm;
	struct list_head *workspace;
	int cpus = num_online_cpus();
	unsigned nofs_flag;
	struct list_head *idle_ws;
	spinlock_t *ws_lock;
	atomic_t *total_ws;
	wait_queue_head_t *ws_wait;
	int *free_ws;

	wsm = btrfs_compress_op[type]->workspace_manager;
	idle_ws	 = &wsm->idle_ws;
	ws_lock	 = &wsm->ws_lock;
	total_ws = &wsm->total_ws;
	ws_wait	 = &wsm->ws_wait;
	free_ws	 = &wsm->free_ws;

again:
	spin_lock(ws_lock);
	if (!list_empty(idle_ws)) {
		workspace = idle_ws->next;
		list_del(workspace);
		(*free_ws)--;
		spin_unlock(ws_lock);
		return workspace;

	}
	if (atomic_read(total_ws) > cpus) {
		DEFINE_WAIT(wait);

		spin_unlock(ws_lock);
		prepare_to_wait(ws_wait, &wait, TASK_UNINTERRUPTIBLE);
		if (atomic_read(total_ws) > cpus && !*free_ws)
			schedule();
		finish_wait(ws_wait, &wait);
		goto again;
	}
	atomic_inc(total_ws);
	spin_unlock(ws_lock);

	/*
	 * Allocation helpers call vmalloc that can't use GFP_NOFS, so we have
	 * to turn it off here because we might get called from the restricted
	 * context of btrfs_compress_bio/btrfs_compress_pages
	 */
	nofs_flag = memalloc_nofs_save();
	workspace = alloc_workspace(type, level);
	memalloc_nofs_restore(nofs_flag);

	if (IS_ERR(workspace)) {
		atomic_dec(total_ws);
		wake_up(ws_wait);

		/*
		 * Do not return the error but go back to waiting. There's a
		 * workspace preallocated for each type and the compression
		 * time is bounded so we get to a workspace eventually. This
		 * makes our caller's life easier.
		 *
		 * To prevent silent and low-probability deadlocks (when the
		 * initial preallocation fails), check if there are any
		 * workspaces at all.
		 */
		if (atomic_read(total_ws) == 0) {
			static DEFINE_RATELIMIT_STATE(_rs,
					/* once per minute */ 60 * HZ,
					/* no burst */ 1);

			if (__ratelimit(&_rs)) {
				pr_warn("BTRFS: no compression workspaces, low memory, retrying\n");
			}
		}
		goto again;
	}
	return workspace;
}

static struct list_head *get_workspace(int type, int level)
{
	switch (type) {
	case BTRFS_COMPRESS_NONE: return btrfs_get_workspace(type, level);
	case BTRFS_COMPRESS_ZLIB: return zlib_get_workspace(level);
	case BTRFS_COMPRESS_LZO:  return btrfs_get_workspace(type, level);
	case BTRFS_COMPRESS_ZSTD: return zstd_get_workspace(level);
	default:
		/*
		 * This can't happen, the type is validated several times
		 * before we get here.
		 */
		BUG();
	}
}

/*
 * put a workspace struct back on the list or free it if we have enough
 * idle ones sitting around
 */
void btrfs_put_workspace(int type, struct list_head *ws)
{
	struct workspace_manager *wsm;
	struct list_head *idle_ws;
	spinlock_t *ws_lock;
	atomic_t *total_ws;
	wait_queue_head_t *ws_wait;
	int *free_ws;

	wsm = btrfs_compress_op[type]->workspace_manager;
	idle_ws	 = &wsm->idle_ws;
	ws_lock	 = &wsm->ws_lock;
	total_ws = &wsm->total_ws;
	ws_wait	 = &wsm->ws_wait;
	free_ws	 = &wsm->free_ws;

	spin_lock(ws_lock);
	if (*free_ws <= num_online_cpus()) {
		list_add(ws, idle_ws);
		(*free_ws)++;
		spin_unlock(ws_lock);
		goto wake;
	}
	spin_unlock(ws_lock);

	free_workspace(type, ws);
	atomic_dec(total_ws);
wake:
	cond_wake_up(ws_wait);
}

static void put_workspace(int type, struct list_head *ws)
{
	switch (type) {
	case BTRFS_COMPRESS_NONE: return btrfs_put_workspace(type, ws);
	case BTRFS_COMPRESS_ZLIB: return btrfs_put_workspace(type, ws);
	case BTRFS_COMPRESS_LZO:  return btrfs_put_workspace(type, ws);
	case BTRFS_COMPRESS_ZSTD: return zstd_put_workspace(ws);
	default:
		/*
		 * This can't happen, the type is validated several times
		 * before we get here.
		 */
		BUG();
	}
}

/*
 * Adjust @level according to the limits of the compression algorithm or
 * fallback to default
 */
static unsigned int btrfs_compress_set_level(int type, unsigned level)
{
	const struct btrfs_compress_op *ops = btrfs_compress_op[type];

	if (level == 0)
		level = ops->default_level;
	else
		level = min(level, ops->max_level);

	return level;
}

/*
 * Given an address space and start and length, compress the bytes into @pages
 * that are allocated on demand.
 *
 * @type_level is encoded algorithm and level, where level 0 means whatever
 * default the algorithm chooses and is opaque here;
 * - compression algo are 0-3
 * - the level are bits 4-7
 *
 * @out_pages is an in/out parameter, holds maximum number of pages to allocate
 * and returns number of actually allocated pages
 *
 * @total_in is used to return the number of bytes actually read.  It
 * may be smaller than the input length if we had to exit early because we
 * ran out of room in the pages array or because we cross the
 * max_out threshold.
 *
 * @total_out is an in/out parameter, must be set to the input length and will
 * be also used to return the total number of compressed bytes
 *
 * @max_out tells us the max number of bytes that we're allowed to
 * stuff into pages
 */
int btrfs_compress_pages(unsigned int type_level, struct address_space *mapping,
			 u64 start, struct page **pages,
			 unsigned long *out_pages,
			 unsigned long *total_in,
			 unsigned long *total_out)
{
	int type = btrfs_compress_type(type_level);
	int level = btrfs_compress_level(type_level);
	struct list_head *workspace;
	int ret;

	level = btrfs_compress_set_level(type, level);
	workspace = get_workspace(type, level);
	ret = compression_compress_pages(type, workspace, mapping, start, pages,
					 out_pages, total_in, total_out);
	put_workspace(type, workspace);
	return ret;
}

/*
 * pages_in is an array of pages with compressed data.
 *
 * disk_start is the starting logical offset of this array in the file
 *
 * orig_bio contains the pages from the file that we want to decompress into
 *
 * srclen is the number of bytes in pages_in
 *
 * The basic idea is that we have a bio that was created by readpages.
 * The pages in the bio are for the uncompressed data, and they may not
 * be contiguous.  They all correspond to the range of bytes covered by
 * the compressed extent.
 */
static int btrfs_decompress_bio(struct compressed_bio *cb)
{
	struct list_head *workspace;
	int ret;
	int type = cb->compress_type;

	workspace = get_workspace(type, 0);
	ret = compression_decompress_bio(type, workspace, cb);
	put_workspace(type, workspace);

	return ret;
}

/*
 * a less complex decompression routine.  Our compressed data fits in a
 * single page, and we want to read a single page out of it.
 * start_byte tells us the offset into the compressed data we're interested in
 */
int btrfs_decompress(int type, unsigned char *data_in, struct page *dest_page,
		     unsigned long start_byte, size_t srclen, size_t destlen)
{
	struct list_head *workspace;
	int ret;

	workspace = get_workspace(type, 0);
	ret = compression_decompress(type, workspace, data_in, dest_page,
				     start_byte, srclen, destlen);
	put_workspace(type, workspace);

	return ret;
}

void __init btrfs_init_compress(void)
{
	btrfs_init_workspace_manager(BTRFS_COMPRESS_NONE);
	btrfs_init_workspace_manager(BTRFS_COMPRESS_ZLIB);
	btrfs_init_workspace_manager(BTRFS_COMPRESS_LZO);
	zstd_init_workspace_manager();
}

void __cold btrfs_exit_compress(void)
{
	btrfs_cleanup_workspace_manager(BTRFS_COMPRESS_NONE);
	btrfs_cleanup_workspace_manager(BTRFS_COMPRESS_ZLIB);
	btrfs_cleanup_workspace_manager(BTRFS_COMPRESS_LZO);
	zstd_cleanup_workspace_manager();
}

/*
 * Copy uncompressed data from working buffer to pages.
 *
 * buf_start is the byte offset we're of the start of our workspace buffer.
 *
 * total_out is the last byte of the buffer
 */
int btrfs_decompress_buf2page(const char *buf, unsigned long buf_start,
			      unsigned long total_out, u64 disk_start,
			      struct bio *bio)
{
	unsigned long buf_offset;
	unsigned long current_buf_start;
	unsigned long start_byte;
	unsigned long prev_start_byte;
	unsigned long working_bytes = total_out - buf_start;
	unsigned long bytes;
	struct bio_vec bvec = bio_iter_iovec(bio, bio->bi_iter);

	/*
	 * start byte is the first byte of the page we're currently
	 * copying into relative to the start of the compressed data.
	 */
	start_byte = page_offset(bvec.bv_page) - disk_start;

	/* we haven't yet hit data corresponding to this page */
	if (total_out <= start_byte)
		return 1;

	/*
	 * the start of the data we care about is offset into
	 * the middle of our working buffer
	 */
	if (total_out > start_byte && buf_start < start_byte) {
		buf_offset = start_byte - buf_start;
		working_bytes -= buf_offset;
	} else {
		buf_offset = 0;
	}
	current_buf_start = buf_start;

	/* copy bytes from the working buffer into the pages */
	while (working_bytes > 0) {
		bytes = min_t(unsigned long, bvec.bv_len,
				PAGE_SIZE - (buf_offset % PAGE_SIZE));
		bytes = min(bytes, working_bytes);

		memcpy_to_page(bvec.bv_page, bvec.bv_offset, buf + buf_offset,
			       bytes);
		flush_dcache_page(bvec.bv_page);

		buf_offset += bytes;
		working_bytes -= bytes;
		current_buf_start += bytes;

		/* check if we need to pick another page */
		bio_advance(bio, bytes);
		if (!bio->bi_iter.bi_size)
			return 0;
		bvec = bio_iter_iovec(bio, bio->bi_iter);
		prev_start_byte = start_byte;
		start_byte = page_offset(bvec.bv_page) - disk_start;

		/*
		 * We need to make sure we're only adjusting
		 * our offset into compression working buffer when
		 * we're switching pages.  Otherwise we can incorrectly
		 * keep copying when we were actually done.
		 */
		if (start_byte != prev_start_byte) {
			/*
			 * make sure our new page is covered by this
			 * working buffer
			 */
			if (total_out <= start_byte)
				return 1;

			/*
			 * the next page in the biovec might not be adjacent
			 * to the last page, but it might still be found
			 * inside this working buffer. bump our offset pointer
			 */
			if (total_out > start_byte &&
			    current_buf_start < start_byte) {
				buf_offset = start_byte - buf_start;
				working_bytes = total_out - start_byte;
				current_buf_start = buf_start + buf_offset;
			}
		}
	}

	return 1;
}

/*
 * Shannon Entropy calculation
 *
 * Pure byte distribution analysis fails to determine compressibility of data.
 * Try calculating entropy to estimate the average minimum number of bits
 * needed to encode the sampled data.
 *
 * For convenience, return the percentage of needed bits, instead of amount of
 * bits directly.
 *
 * @ENTROPY_LVL_ACEPTABLE - below that threshold, sample has low byte entropy
 *			    and can be compressible with high probability
 *
 * @ENTROPY_LVL_HIGH - data are not compressible with high probability
 *
 * Use of ilog2() decreases precision, we lower the LVL to 5 to compensate.
 */
#define ENTROPY_LVL_ACEPTABLE		(65)
#define ENTROPY_LVL_HIGH		(80)

/*
 * For increasead precision in shannon_entropy calculation,
 * let's do pow(n, M) to save more digits after comma:
 *
 * - maximum int bit length is 64
 * - ilog2(MAX_SAMPLE_SIZE)	-> 13
 * - 13 * 4 = 52 < 64		-> M = 4
 *
 * So use pow(n, 4).
 */
static inline u32 ilog2_w(u64 n)
{
	return ilog2(n * n * n * n);
}

static u32 shannon_entropy(struct heuristic_ws *ws)
{
	const u32 entropy_max = 8 * ilog2_w(2);
	u32 entropy_sum = 0;
	u32 p, p_base, sz_base;
	u32 i;

	sz_base = ilog2_w(ws->sample_size);
	for (i = 0; i < BUCKET_SIZE && ws->bucket[i].count > 0; i++) {
		p = ws->bucket[i].count;
		p_base = ilog2_w(p);
		entropy_sum += p * (sz_base - p_base);
	}

	entropy_sum /= ws->sample_size;
	return entropy_sum * 100 / entropy_max;
}

#define RADIX_BASE		4U
#define COUNTERS_SIZE		(1U << RADIX_BASE)

static u8 get4bits(u64 num, int shift) {
	u8 low4bits;

	num >>= shift;
	/* Reverse order */
	low4bits = (COUNTERS_SIZE - 1) - (num % COUNTERS_SIZE);
	return low4bits;
}

/*
 * Use 4 bits as radix base
 * Use 16 u32 counters for calculating new position in buf array
 *
 * @array     - array that will be sorted
 * @array_buf - buffer array to store sorting results
 *              must be equal in size to @array
 * @num       - array size
 */
static void radix_sort(struct bucket_item *array, struct bucket_item *array_buf,
		       int num)
{
	u64 max_num;
	u64 buf_num;
	u32 counters[COUNTERS_SIZE];
	u32 new_addr;
	u32 addr;
	int bitlen;
	int shift;
	int i;

	/*
	 * Try avoid useless loop iterations for small numbers stored in big
	 * counters.  Example: 48 33 4 ... in 64bit array
	 */
	max_num = array[0].count;
	for (i = 1; i < num; i++) {
		buf_num = array[i].count;
		if (buf_num > max_num)
			max_num = buf_num;
	}

	buf_num = ilog2(max_num);
	bitlen = ALIGN(buf_num, RADIX_BASE * 2);

	shift = 0;
	while (shift < bitlen) {
		memset(counters, 0, sizeof(counters));

		for (i = 0; i < num; i++) {
			buf_num = array[i].count;
			addr = get4bits(buf_num, shift);
			counters[addr]++;
		}

		for (i = 1; i < COUNTERS_SIZE; i++)
			counters[i] += counters[i - 1];

		for (i = num - 1; i >= 0; i--) {
			buf_num = array[i].count;
			addr = get4bits(buf_num, shift);
			counters[addr]--;
			new_addr = counters[addr];
			array_buf[new_addr] = array[i];
		}

		shift += RADIX_BASE;

		/*
		 * Normal radix expects to move data from a temporary array, to
		 * the main one.  But that requires some CPU time. Avoid that
		 * by doing another sort iteration to original array instead of
		 * memcpy()
		 */
		memset(counters, 0, sizeof(counters));

		for (i = 0; i < num; i ++) {
			buf_num = array_buf[i].count;
			addr = get4bits(buf_num, shift);
			counters[addr]++;
		}

		for (i = 1; i < COUNTERS_SIZE; i++)
			counters[i] += counters[i - 1];

		for (i = num - 1; i >= 0; i--) {
			buf_num = array_buf[i].count;
			addr = get4bits(buf_num, shift);
			counters[addr]--;
			new_addr = counters[addr];
			array[new_addr] = array_buf[i];
		}

		shift += RADIX_BASE;
	}
}

/*
 * Size of the core byte set - how many bytes cover 90% of the sample
 *
 * There are several types of structured binary data that use nearly all byte
 * values. The distribution can be uniform and counts in all buckets will be
 * nearly the same (eg. encrypted data). Unlikely to be compressible.
 *
 * Other possibility is normal (Gaussian) distribution, where the data could
 * be potentially compressible, but we have to take a few more steps to decide
 * how much.
 *
 * @BYTE_CORE_SET_LOW  - main part of byte values repeated frequently,
 *                       compression algo can easy fix that
 * @BYTE_CORE_SET_HIGH - data have uniform distribution and with high
 *                       probability is not compressible
 */
#define BYTE_CORE_SET_LOW		(64)
#define BYTE_CORE_SET_HIGH		(200)

static int byte_core_set_size(struct heuristic_ws *ws)
{
	u32 i;
	u32 coreset_sum = 0;
	const u32 core_set_threshold = ws->sample_size * 90 / 100;
	struct bucket_item *bucket = ws->bucket;

	/* Sort in reverse order */
	radix_sort(ws->bucket, ws->bucket_b, BUCKET_SIZE);

	for (i = 0; i < BYTE_CORE_SET_LOW; i++)
		coreset_sum += bucket[i].count;

	if (coreset_sum > core_set_threshold)
		return i;

	for (; i < BYTE_CORE_SET_HIGH && bucket[i].count > 0; i++) {
		coreset_sum += bucket[i].count;
		if (coreset_sum > core_set_threshold)
			break;
	}

	return i;
}

/*
 * Count byte values in buckets.
 * This heuristic can detect textual data (configs, xml, json, html, etc).
 * Because in most text-like data byte set is restricted to limited number of
 * possible characters, and that restriction in most cases makes data easy to
 * compress.
 *
 * @BYTE_SET_THRESHOLD - consider all data within this byte set size:
 *	less - compressible
 *	more - need additional analysis
 */
#define BYTE_SET_THRESHOLD		(64)

static u32 byte_set_size(const struct heuristic_ws *ws)
{
	u32 i;
	u32 byte_set_size = 0;

	for (i = 0; i < BYTE_SET_THRESHOLD; i++) {
		if (ws->bucket[i].count > 0)
			byte_set_size++;
	}

	/*
	 * Continue collecting count of byte values in buckets.  If the byte
	 * set size is bigger then the threshold, it's pointless to continue,
	 * the detection technique would fail for this type of data.
	 */
	for (; i < BUCKET_SIZE; i++) {
		if (ws->bucket[i].count > 0) {
			byte_set_size++;
			if (byte_set_size > BYTE_SET_THRESHOLD)
				return byte_set_size;
		}
	}

	return byte_set_size;
}

static bool sample_repeated_patterns(struct heuristic_ws *ws)
{
	const u32 half_of_sample = ws->sample_size / 2;
	const u8 *data = ws->sample;

	return memcmp(&data[0], &data[half_of_sample], half_of_sample) == 0;
}

static void heuristic_collect_sample(struct inode *inode, u64 start, u64 end,
				     struct heuristic_ws *ws)
{
	struct page *page;
	u64 index, index_end;
	u32 i, curr_sample_pos;
	u8 *in_data;

	/*
	 * Compression handles the input data by chunks of 128KiB
	 * (defined by BTRFS_MAX_UNCOMPRESSED)
	 *
	 * We do the same for the heuristic and loop over the whole range.
	 *
	 * MAX_SAMPLE_SIZE - calculated under assumption that heuristic will
	 * process no more than BTRFS_MAX_UNCOMPRESSED at a time.
	 */
	if (end - start > BTRFS_MAX_UNCOMPRESSED)
		end = start + BTRFS_MAX_UNCOMPRESSED;

	index = start >> PAGE_SHIFT;
	index_end = end >> PAGE_SHIFT;

	/* Don't miss unaligned end */
	if (!IS_ALIGNED(end, PAGE_SIZE))
		index_end++;

	curr_sample_pos = 0;
	while (index < index_end) {
		page = find_get_page(inode->i_mapping, index);
		in_data = kmap_local_page(page);
		/* Handle case where the start is not aligned to PAGE_SIZE */
		i = start % PAGE_SIZE;
		while (i < PAGE_SIZE - SAMPLING_READ_SIZE) {
			/* Don't sample any garbage from the last page */
			if (start > end - SAMPLING_READ_SIZE)
				break;
			memcpy(&ws->sample[curr_sample_pos], &in_data[i],
					SAMPLING_READ_SIZE);
			i += SAMPLING_INTERVAL;
			start += SAMPLING_INTERVAL;
			curr_sample_pos += SAMPLING_READ_SIZE;
		}
		kunmap_local(in_data);
		put_page(page);

		index++;
	}

	ws->sample_size = curr_sample_pos;
}

/*
 * Compression heuristic.
 *
 * For now is's a naive and optimistic 'return true', we'll extend the logic to
 * quickly (compared to direct compression) detect data characteristics
 * (compressible/uncompressible) to avoid wasting CPU time on uncompressible
 * data.
 *
 * The following types of analysis can be performed:
 * - detect mostly zero data
 * - detect data with low "byte set" size (text, etc)
 * - detect data with low/high "core byte" set
 *
 * Return non-zero if the compression should be done, 0 otherwise.
 */
int btrfs_compress_heuristic(struct inode *inode, u64 start, u64 end)
{
	struct list_head *ws_list = get_workspace(0, 0);
	struct heuristic_ws *ws;
	u32 i;
	u8 byte;
	int ret = 0;

	ws = list_entry(ws_list, struct heuristic_ws, list);

	heuristic_collect_sample(inode, start, end, ws);

	if (sample_repeated_patterns(ws)) {
		ret = 1;
		goto out;
	}

	memset(ws->bucket, 0, sizeof(*ws->bucket)*BUCKET_SIZE);

	for (i = 0; i < ws->sample_size; i++) {
		byte = ws->sample[i];
		ws->bucket[byte].count++;
	}

	i = byte_set_size(ws);
	if (i < BYTE_SET_THRESHOLD) {
		ret = 2;
		goto out;
	}

	i = byte_core_set_size(ws);
	if (i <= BYTE_CORE_SET_LOW) {
		ret = 3;
		goto out;
	}

	if (i >= BYTE_CORE_SET_HIGH) {
		ret = 0;
		goto out;
	}

	i = shannon_entropy(ws);
	if (i <= ENTROPY_LVL_ACEPTABLE) {
		ret = 4;
		goto out;
	}

	/*
	 * For the levels below ENTROPY_LVL_HIGH, additional analysis would be
	 * needed to give green light to compression.
	 *
	 * For now just assume that compression at that level is not worth the
	 * resources because:
	 *
	 * 1. it is possible to defrag the data later
	 *
	 * 2. the data would turn out to be hardly compressible, eg. 150 byte
	 * values, every bucket has counter at level ~54. The heuristic would
	 * be confused. This can happen when data have some internal repeated
	 * patterns like "abbacbbc...". This can be detected by analyzing
	 * pairs of bytes, which is too costly.
	 */
	if (i < ENTROPY_LVL_HIGH) {
		ret = 5;
		goto out;
	} else {
		ret = 0;
		goto out;
	}

out:
	put_workspace(0, ws_list);
	return ret;
}

/*
 * Convert the compression suffix (eg. after "zlib" starting with ":") to
 * level, unrecognized string will set the default level
 */
unsigned int btrfs_compress_str2level(unsigned int type, const char *str)
{
	unsigned int level = 0;
	int ret;

	if (!type)
		return 0;

	if (str[0] == ':') {
		ret = kstrtouint(str + 1, 10, &level);
		if (ret)
			level = 0;
	}

	level = btrfs_compress_set_level(type, level);

	return level;
}
