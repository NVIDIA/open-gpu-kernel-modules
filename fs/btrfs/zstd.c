// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2016-present, Facebook, Inc.
 * All rights reserved.
 *
 */

#include <linux/bio.h>
#include <linux/bitmap.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/sched/mm.h>
#include <linux/pagemap.h>
#include <linux/refcount.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/zstd.h>
#include "misc.h"
#include "compression.h"
#include "ctree.h"

#define ZSTD_BTRFS_MAX_WINDOWLOG 17
#define ZSTD_BTRFS_MAX_INPUT (1 << ZSTD_BTRFS_MAX_WINDOWLOG)
#define ZSTD_BTRFS_DEFAULT_LEVEL 3
#define ZSTD_BTRFS_MAX_LEVEL 15
/* 307s to avoid pathologically clashing with transaction commit */
#define ZSTD_BTRFS_RECLAIM_JIFFIES (307 * HZ)

static ZSTD_parameters zstd_get_btrfs_parameters(unsigned int level,
						 size_t src_len)
{
	ZSTD_parameters params = ZSTD_getParams(level, src_len, 0);

	if (params.cParams.windowLog > ZSTD_BTRFS_MAX_WINDOWLOG)
		params.cParams.windowLog = ZSTD_BTRFS_MAX_WINDOWLOG;
	WARN_ON(src_len > ZSTD_BTRFS_MAX_INPUT);
	return params;
}

struct workspace {
	void *mem;
	size_t size;
	char *buf;
	unsigned int level;
	unsigned int req_level;
	unsigned long last_used; /* jiffies */
	struct list_head list;
	struct list_head lru_list;
	ZSTD_inBuffer in_buf;
	ZSTD_outBuffer out_buf;
};

/*
 * Zstd Workspace Management
 *
 * Zstd workspaces have different memory requirements depending on the level.
 * The zstd workspaces are managed by having individual lists for each level
 * and a global lru.  Forward progress is maintained by protecting a max level
 * workspace.
 *
 * Getting a workspace is done by using the bitmap to identify the levels that
 * have available workspaces and scans up.  This lets us recycle higher level
 * workspaces because of the monotonic memory guarantee.  A workspace's
 * last_used is only updated if it is being used by the corresponding memory
 * level.  Putting a workspace involves adding it back to the appropriate places
 * and adding it back to the lru if necessary.
 *
 * A timer is used to reclaim workspaces if they have not been used for
 * ZSTD_BTRFS_RECLAIM_JIFFIES.  This helps keep only active workspaces around.
 * The upper bound is provided by the workqueue limit which is 2 (percpu limit).
 */

struct zstd_workspace_manager {
	const struct btrfs_compress_op *ops;
	spinlock_t lock;
	struct list_head lru_list;
	struct list_head idle_ws[ZSTD_BTRFS_MAX_LEVEL];
	unsigned long active_map;
	wait_queue_head_t wait;
	struct timer_list timer;
};

static struct zstd_workspace_manager wsm;

static size_t zstd_ws_mem_sizes[ZSTD_BTRFS_MAX_LEVEL];

static inline struct workspace *list_to_workspace(struct list_head *list)
{
	return container_of(list, struct workspace, list);
}

void zstd_free_workspace(struct list_head *ws);
struct list_head *zstd_alloc_workspace(unsigned int level);
/*
 * zstd_reclaim_timer_fn - reclaim timer
 * @t: timer
 *
 * This scans the lru_list and attempts to reclaim any workspace that hasn't
 * been used for ZSTD_BTRFS_RECLAIM_JIFFIES.
 */
static void zstd_reclaim_timer_fn(struct timer_list *timer)
{
	unsigned long reclaim_threshold = jiffies - ZSTD_BTRFS_RECLAIM_JIFFIES;
	struct list_head *pos, *next;

	spin_lock_bh(&wsm.lock);

	if (list_empty(&wsm.lru_list)) {
		spin_unlock_bh(&wsm.lock);
		return;
	}

	list_for_each_prev_safe(pos, next, &wsm.lru_list) {
		struct workspace *victim = container_of(pos, struct workspace,
							lru_list);
		unsigned int level;

		if (time_after(victim->last_used, reclaim_threshold))
			break;

		/* workspace is in use */
		if (victim->req_level)
			continue;

		level = victim->level;
		list_del(&victim->lru_list);
		list_del(&victim->list);
		zstd_free_workspace(&victim->list);

		if (list_empty(&wsm.idle_ws[level - 1]))
			clear_bit(level - 1, &wsm.active_map);

	}

	if (!list_empty(&wsm.lru_list))
		mod_timer(&wsm.timer, jiffies + ZSTD_BTRFS_RECLAIM_JIFFIES);

	spin_unlock_bh(&wsm.lock);
}

/*
 * zstd_calc_ws_mem_sizes - calculate monotonic memory bounds
 *
 * It is possible based on the level configurations that a higher level
 * workspace uses less memory than a lower level workspace.  In order to reuse
 * workspaces, this must be made a monotonic relationship.  This precomputes
 * the required memory for each level and enforces the monotonicity between
 * level and memory required.
 */
static void zstd_calc_ws_mem_sizes(void)
{
	size_t max_size = 0;
	unsigned int level;

	for (level = 1; level <= ZSTD_BTRFS_MAX_LEVEL; level++) {
		ZSTD_parameters params =
			zstd_get_btrfs_parameters(level, ZSTD_BTRFS_MAX_INPUT);
		size_t level_size =
			max_t(size_t,
			      ZSTD_CStreamWorkspaceBound(params.cParams),
			      ZSTD_DStreamWorkspaceBound(ZSTD_BTRFS_MAX_INPUT));

		max_size = max_t(size_t, max_size, level_size);
		zstd_ws_mem_sizes[level - 1] = max_size;
	}
}

void zstd_init_workspace_manager(void)
{
	struct list_head *ws;
	int i;

	zstd_calc_ws_mem_sizes();

	wsm.ops = &btrfs_zstd_compress;
	spin_lock_init(&wsm.lock);
	init_waitqueue_head(&wsm.wait);
	timer_setup(&wsm.timer, zstd_reclaim_timer_fn, 0);

	INIT_LIST_HEAD(&wsm.lru_list);
	for (i = 0; i < ZSTD_BTRFS_MAX_LEVEL; i++)
		INIT_LIST_HEAD(&wsm.idle_ws[i]);

	ws = zstd_alloc_workspace(ZSTD_BTRFS_MAX_LEVEL);
	if (IS_ERR(ws)) {
		pr_warn(
		"BTRFS: cannot preallocate zstd compression workspace\n");
	} else {
		set_bit(ZSTD_BTRFS_MAX_LEVEL - 1, &wsm.active_map);
		list_add(ws, &wsm.idle_ws[ZSTD_BTRFS_MAX_LEVEL - 1]);
	}
}

void zstd_cleanup_workspace_manager(void)
{
	struct workspace *workspace;
	int i;

	spin_lock_bh(&wsm.lock);
	for (i = 0; i < ZSTD_BTRFS_MAX_LEVEL; i++) {
		while (!list_empty(&wsm.idle_ws[i])) {
			workspace = container_of(wsm.idle_ws[i].next,
						 struct workspace, list);
			list_del(&workspace->list);
			list_del(&workspace->lru_list);
			zstd_free_workspace(&workspace->list);
		}
	}
	spin_unlock_bh(&wsm.lock);

	del_timer_sync(&wsm.timer);
}

/*
 * zstd_find_workspace - find workspace
 * @level: compression level
 *
 * This iterates over the set bits in the active_map beginning at the requested
 * compression level.  This lets us utilize already allocated workspaces before
 * allocating a new one.  If the workspace is of a larger size, it is used, but
 * the place in the lru_list and last_used times are not updated.  This is to
 * offer the opportunity to reclaim the workspace in favor of allocating an
 * appropriately sized one in the future.
 */
static struct list_head *zstd_find_workspace(unsigned int level)
{
	struct list_head *ws;
	struct workspace *workspace;
	int i = level - 1;

	spin_lock_bh(&wsm.lock);
	for_each_set_bit_from(i, &wsm.active_map, ZSTD_BTRFS_MAX_LEVEL) {
		if (!list_empty(&wsm.idle_ws[i])) {
			ws = wsm.idle_ws[i].next;
			workspace = list_to_workspace(ws);
			list_del_init(ws);
			/* keep its place if it's a lower level using this */
			workspace->req_level = level;
			if (level == workspace->level)
				list_del(&workspace->lru_list);
			if (list_empty(&wsm.idle_ws[i]))
				clear_bit(i, &wsm.active_map);
			spin_unlock_bh(&wsm.lock);
			return ws;
		}
	}
	spin_unlock_bh(&wsm.lock);

	return NULL;
}

/*
 * zstd_get_workspace - zstd's get_workspace
 * @level: compression level
 *
 * If @level is 0, then any compression level can be used.  Therefore, we begin
 * scanning from 1.  We first scan through possible workspaces and then after
 * attempt to allocate a new workspace.  If we fail to allocate one due to
 * memory pressure, go to sleep waiting for the max level workspace to free up.
 */
struct list_head *zstd_get_workspace(unsigned int level)
{
	struct list_head *ws;
	unsigned int nofs_flag;

	/* level == 0 means we can use any workspace */
	if (!level)
		level = 1;

again:
	ws = zstd_find_workspace(level);
	if (ws)
		return ws;

	nofs_flag = memalloc_nofs_save();
	ws = zstd_alloc_workspace(level);
	memalloc_nofs_restore(nofs_flag);

	if (IS_ERR(ws)) {
		DEFINE_WAIT(wait);

		prepare_to_wait(&wsm.wait, &wait, TASK_UNINTERRUPTIBLE);
		schedule();
		finish_wait(&wsm.wait, &wait);

		goto again;
	}

	return ws;
}

/*
 * zstd_put_workspace - zstd put_workspace
 * @ws: list_head for the workspace
 *
 * When putting back a workspace, we only need to update the LRU if we are of
 * the requested compression level.  Here is where we continue to protect the
 * max level workspace or update last_used accordingly.  If the reclaim timer
 * isn't set, it is also set here.  Only the max level workspace tries and wakes
 * up waiting workspaces.
 */
void zstd_put_workspace(struct list_head *ws)
{
	struct workspace *workspace = list_to_workspace(ws);

	spin_lock_bh(&wsm.lock);

	/* A node is only taken off the lru if we are the corresponding level */
	if (workspace->req_level == workspace->level) {
		/* Hide a max level workspace from reclaim */
		if (list_empty(&wsm.idle_ws[ZSTD_BTRFS_MAX_LEVEL - 1])) {
			INIT_LIST_HEAD(&workspace->lru_list);
		} else {
			workspace->last_used = jiffies;
			list_add(&workspace->lru_list, &wsm.lru_list);
			if (!timer_pending(&wsm.timer))
				mod_timer(&wsm.timer,
					  jiffies + ZSTD_BTRFS_RECLAIM_JIFFIES);
		}
	}

	set_bit(workspace->level - 1, &wsm.active_map);
	list_add(&workspace->list, &wsm.idle_ws[workspace->level - 1]);
	workspace->req_level = 0;

	spin_unlock_bh(&wsm.lock);

	if (workspace->level == ZSTD_BTRFS_MAX_LEVEL)
		cond_wake_up(&wsm.wait);
}

void zstd_free_workspace(struct list_head *ws)
{
	struct workspace *workspace = list_entry(ws, struct workspace, list);

	kvfree(workspace->mem);
	kfree(workspace->buf);
	kfree(workspace);
}

struct list_head *zstd_alloc_workspace(unsigned int level)
{
	struct workspace *workspace;

	workspace = kzalloc(sizeof(*workspace), GFP_KERNEL);
	if (!workspace)
		return ERR_PTR(-ENOMEM);

	workspace->size = zstd_ws_mem_sizes[level - 1];
	workspace->level = level;
	workspace->req_level = level;
	workspace->last_used = jiffies;
	workspace->mem = kvmalloc(workspace->size, GFP_KERNEL);
	workspace->buf = kmalloc(PAGE_SIZE, GFP_KERNEL);
	if (!workspace->mem || !workspace->buf)
		goto fail;

	INIT_LIST_HEAD(&workspace->list);
	INIT_LIST_HEAD(&workspace->lru_list);

	return &workspace->list;
fail:
	zstd_free_workspace(&workspace->list);
	return ERR_PTR(-ENOMEM);
}

int zstd_compress_pages(struct list_head *ws, struct address_space *mapping,
		u64 start, struct page **pages, unsigned long *out_pages,
		unsigned long *total_in, unsigned long *total_out)
{
	struct workspace *workspace = list_entry(ws, struct workspace, list);
	ZSTD_CStream *stream;
	int ret = 0;
	int nr_pages = 0;
	struct page *in_page = NULL;  /* The current page to read */
	struct page *out_page = NULL; /* The current page to write to */
	unsigned long tot_in = 0;
	unsigned long tot_out = 0;
	unsigned long len = *total_out;
	const unsigned long nr_dest_pages = *out_pages;
	unsigned long max_out = nr_dest_pages * PAGE_SIZE;
	ZSTD_parameters params = zstd_get_btrfs_parameters(workspace->req_level,
							   len);

	*out_pages = 0;
	*total_out = 0;
	*total_in = 0;

	/* Initialize the stream */
	stream = ZSTD_initCStream(params, len, workspace->mem,
			workspace->size);
	if (!stream) {
		pr_warn("BTRFS: ZSTD_initCStream failed\n");
		ret = -EIO;
		goto out;
	}

	/* map in the first page of input data */
	in_page = find_get_page(mapping, start >> PAGE_SHIFT);
	workspace->in_buf.src = kmap(in_page);
	workspace->in_buf.pos = 0;
	workspace->in_buf.size = min_t(size_t, len, PAGE_SIZE);


	/* Allocate and map in the output buffer */
	out_page = alloc_page(GFP_NOFS | __GFP_HIGHMEM);
	if (out_page == NULL) {
		ret = -ENOMEM;
		goto out;
	}
	pages[nr_pages++] = out_page;
	workspace->out_buf.dst = kmap(out_page);
	workspace->out_buf.pos = 0;
	workspace->out_buf.size = min_t(size_t, max_out, PAGE_SIZE);

	while (1) {
		size_t ret2;

		ret2 = ZSTD_compressStream(stream, &workspace->out_buf,
				&workspace->in_buf);
		if (ZSTD_isError(ret2)) {
			pr_debug("BTRFS: ZSTD_compressStream returned %d\n",
					ZSTD_getErrorCode(ret2));
			ret = -EIO;
			goto out;
		}

		/* Check to see if we are making it bigger */
		if (tot_in + workspace->in_buf.pos > 8192 &&
				tot_in + workspace->in_buf.pos <
				tot_out + workspace->out_buf.pos) {
			ret = -E2BIG;
			goto out;
		}

		/* We've reached the end of our output range */
		if (workspace->out_buf.pos >= max_out) {
			tot_out += workspace->out_buf.pos;
			ret = -E2BIG;
			goto out;
		}

		/* Check if we need more output space */
		if (workspace->out_buf.pos == workspace->out_buf.size) {
			tot_out += PAGE_SIZE;
			max_out -= PAGE_SIZE;
			kunmap(out_page);
			if (nr_pages == nr_dest_pages) {
				out_page = NULL;
				ret = -E2BIG;
				goto out;
			}
			out_page = alloc_page(GFP_NOFS | __GFP_HIGHMEM);
			if (out_page == NULL) {
				ret = -ENOMEM;
				goto out;
			}
			pages[nr_pages++] = out_page;
			workspace->out_buf.dst = kmap(out_page);
			workspace->out_buf.pos = 0;
			workspace->out_buf.size = min_t(size_t, max_out,
							PAGE_SIZE);
		}

		/* We've reached the end of the input */
		if (workspace->in_buf.pos >= len) {
			tot_in += workspace->in_buf.pos;
			break;
		}

		/* Check if we need more input */
		if (workspace->in_buf.pos == workspace->in_buf.size) {
			tot_in += PAGE_SIZE;
			kunmap(in_page);
			put_page(in_page);

			start += PAGE_SIZE;
			len -= PAGE_SIZE;
			in_page = find_get_page(mapping, start >> PAGE_SHIFT);
			workspace->in_buf.src = kmap(in_page);
			workspace->in_buf.pos = 0;
			workspace->in_buf.size = min_t(size_t, len, PAGE_SIZE);
		}
	}
	while (1) {
		size_t ret2;

		ret2 = ZSTD_endStream(stream, &workspace->out_buf);
		if (ZSTD_isError(ret2)) {
			pr_debug("BTRFS: ZSTD_endStream returned %d\n",
					ZSTD_getErrorCode(ret2));
			ret = -EIO;
			goto out;
		}
		if (ret2 == 0) {
			tot_out += workspace->out_buf.pos;
			break;
		}
		if (workspace->out_buf.pos >= max_out) {
			tot_out += workspace->out_buf.pos;
			ret = -E2BIG;
			goto out;
		}

		tot_out += PAGE_SIZE;
		max_out -= PAGE_SIZE;
		kunmap(out_page);
		if (nr_pages == nr_dest_pages) {
			out_page = NULL;
			ret = -E2BIG;
			goto out;
		}
		out_page = alloc_page(GFP_NOFS | __GFP_HIGHMEM);
		if (out_page == NULL) {
			ret = -ENOMEM;
			goto out;
		}
		pages[nr_pages++] = out_page;
		workspace->out_buf.dst = kmap(out_page);
		workspace->out_buf.pos = 0;
		workspace->out_buf.size = min_t(size_t, max_out, PAGE_SIZE);
	}

	if (tot_out >= tot_in) {
		ret = -E2BIG;
		goto out;
	}

	ret = 0;
	*total_in = tot_in;
	*total_out = tot_out;
out:
	*out_pages = nr_pages;
	/* Cleanup */
	if (in_page) {
		kunmap(in_page);
		put_page(in_page);
	}
	if (out_page)
		kunmap(out_page);
	return ret;
}

int zstd_decompress_bio(struct list_head *ws, struct compressed_bio *cb)
{
	struct workspace *workspace = list_entry(ws, struct workspace, list);
	struct page **pages_in = cb->compressed_pages;
	u64 disk_start = cb->start;
	struct bio *orig_bio = cb->orig_bio;
	size_t srclen = cb->compressed_len;
	ZSTD_DStream *stream;
	int ret = 0;
	unsigned long page_in_index = 0;
	unsigned long total_pages_in = DIV_ROUND_UP(srclen, PAGE_SIZE);
	unsigned long buf_start;
	unsigned long total_out = 0;

	stream = ZSTD_initDStream(
			ZSTD_BTRFS_MAX_INPUT, workspace->mem, workspace->size);
	if (!stream) {
		pr_debug("BTRFS: ZSTD_initDStream failed\n");
		ret = -EIO;
		goto done;
	}

	workspace->in_buf.src = kmap(pages_in[page_in_index]);
	workspace->in_buf.pos = 0;
	workspace->in_buf.size = min_t(size_t, srclen, PAGE_SIZE);

	workspace->out_buf.dst = workspace->buf;
	workspace->out_buf.pos = 0;
	workspace->out_buf.size = PAGE_SIZE;

	while (1) {
		size_t ret2;

		ret2 = ZSTD_decompressStream(stream, &workspace->out_buf,
				&workspace->in_buf);
		if (ZSTD_isError(ret2)) {
			pr_debug("BTRFS: ZSTD_decompressStream returned %d\n",
					ZSTD_getErrorCode(ret2));
			ret = -EIO;
			goto done;
		}
		buf_start = total_out;
		total_out += workspace->out_buf.pos;
		workspace->out_buf.pos = 0;

		ret = btrfs_decompress_buf2page(workspace->out_buf.dst,
				buf_start, total_out, disk_start, orig_bio);
		if (ret == 0)
			break;

		if (workspace->in_buf.pos >= srclen)
			break;

		/* Check if we've hit the end of a frame */
		if (ret2 == 0)
			break;

		if (workspace->in_buf.pos == workspace->in_buf.size) {
			kunmap(pages_in[page_in_index++]);
			if (page_in_index >= total_pages_in) {
				workspace->in_buf.src = NULL;
				ret = -EIO;
				goto done;
			}
			srclen -= PAGE_SIZE;
			workspace->in_buf.src = kmap(pages_in[page_in_index]);
			workspace->in_buf.pos = 0;
			workspace->in_buf.size = min_t(size_t, srclen, PAGE_SIZE);
		}
	}
	ret = 0;
	zero_fill_bio(orig_bio);
done:
	if (workspace->in_buf.src)
		kunmap(pages_in[page_in_index]);
	return ret;
}

int zstd_decompress(struct list_head *ws, unsigned char *data_in,
		struct page *dest_page, unsigned long start_byte, size_t srclen,
		size_t destlen)
{
	struct workspace *workspace = list_entry(ws, struct workspace, list);
	ZSTD_DStream *stream;
	int ret = 0;
	size_t ret2;
	unsigned long total_out = 0;
	unsigned long pg_offset = 0;

	stream = ZSTD_initDStream(
			ZSTD_BTRFS_MAX_INPUT, workspace->mem, workspace->size);
	if (!stream) {
		pr_warn("BTRFS: ZSTD_initDStream failed\n");
		ret = -EIO;
		goto finish;
	}

	destlen = min_t(size_t, destlen, PAGE_SIZE);

	workspace->in_buf.src = data_in;
	workspace->in_buf.pos = 0;
	workspace->in_buf.size = srclen;

	workspace->out_buf.dst = workspace->buf;
	workspace->out_buf.pos = 0;
	workspace->out_buf.size = PAGE_SIZE;

	ret2 = 1;
	while (pg_offset < destlen
	       && workspace->in_buf.pos < workspace->in_buf.size) {
		unsigned long buf_start;
		unsigned long buf_offset;
		unsigned long bytes;

		/* Check if the frame is over and we still need more input */
		if (ret2 == 0) {
			pr_debug("BTRFS: ZSTD_decompressStream ended early\n");
			ret = -EIO;
			goto finish;
		}
		ret2 = ZSTD_decompressStream(stream, &workspace->out_buf,
				&workspace->in_buf);
		if (ZSTD_isError(ret2)) {
			pr_debug("BTRFS: ZSTD_decompressStream returned %d\n",
					ZSTD_getErrorCode(ret2));
			ret = -EIO;
			goto finish;
		}

		buf_start = total_out;
		total_out += workspace->out_buf.pos;
		workspace->out_buf.pos = 0;

		if (total_out <= start_byte)
			continue;

		if (total_out > start_byte && buf_start < start_byte)
			buf_offset = start_byte - buf_start;
		else
			buf_offset = 0;

		bytes = min_t(unsigned long, destlen - pg_offset,
				workspace->out_buf.size - buf_offset);

		memcpy_to_page(dest_page, pg_offset,
			       workspace->out_buf.dst + buf_offset, bytes);

		pg_offset += bytes;
	}
	ret = 0;
finish:
	if (pg_offset < destlen) {
		memzero_page(dest_page, pg_offset, destlen - pg_offset);
	}
	return ret;
}

const struct btrfs_compress_op btrfs_zstd_compress = {
	/* ZSTD uses own workspace manager */
	.workspace_manager = NULL,
	.max_level	= ZSTD_BTRFS_MAX_LEVEL,
	.default_level	= ZSTD_BTRFS_DEFAULT_LEVEL,
};
