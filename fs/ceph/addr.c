// SPDX-License-Identifier: GPL-2.0
#include <linux/ceph/ceph_debug.h>

#include <linux/backing-dev.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/pagemap.h>
#include <linux/writeback.h>	/* generic_writepages */
#include <linux/slab.h>
#include <linux/pagevec.h>
#include <linux/task_io_accounting_ops.h>
#include <linux/signal.h>
#include <linux/iversion.h>
#include <linux/ktime.h>
#include <linux/netfs.h>

#include "super.h"
#include "mds_client.h"
#include "cache.h"
#include "metric.h"
#include <linux/ceph/osd_client.h>
#include <linux/ceph/striper.h>

/*
 * Ceph address space ops.
 *
 * There are a few funny things going on here.
 *
 * The page->private field is used to reference a struct
 * ceph_snap_context for _every_ dirty page.  This indicates which
 * snapshot the page was logically dirtied in, and thus which snap
 * context needs to be associated with the osd write during writeback.
 *
 * Similarly, struct ceph_inode_info maintains a set of counters to
 * count dirty pages on the inode.  In the absence of snapshots,
 * i_wrbuffer_ref == i_wrbuffer_ref_head == the dirty page count.
 *
 * When a snapshot is taken (that is, when the client receives
 * notification that a snapshot was taken), each inode with caps and
 * with dirty pages (dirty pages implies there is a cap) gets a new
 * ceph_cap_snap in the i_cap_snaps list (which is sorted in ascending
 * order, new snaps go to the tail).  The i_wrbuffer_ref_head count is
 * moved to capsnap->dirty. (Unless a sync write is currently in
 * progress.  In that case, the capsnap is said to be "pending", new
 * writes cannot start, and the capsnap isn't "finalized" until the
 * write completes (or fails) and a final size/mtime for the inode for
 * that snap can be settled upon.)  i_wrbuffer_ref_head is reset to 0.
 *
 * On writeback, we must submit writes to the osd IN SNAP ORDER.  So,
 * we look for the first capsnap in i_cap_snaps and write out pages in
 * that snap context _only_.  Then we move on to the next capsnap,
 * eventually reaching the "live" or "head" context (i.e., pages that
 * are not yet snapped) and are writing the most recently dirtied
 * pages.
 *
 * Invalidate and so forth must take care to ensure the dirty page
 * accounting is preserved.
 */

#define CONGESTION_ON_THRESH(congestion_kb) (congestion_kb >> (PAGE_SHIFT-10))
#define CONGESTION_OFF_THRESH(congestion_kb)				\
	(CONGESTION_ON_THRESH(congestion_kb) -				\
	 (CONGESTION_ON_THRESH(congestion_kb) >> 2))

static int ceph_netfs_check_write_begin(struct file *file, loff_t pos, unsigned int len,
					struct page *page, void **_fsdata);

static inline struct ceph_snap_context *page_snap_context(struct page *page)
{
	if (PagePrivate(page))
		return (void *)page->private;
	return NULL;
}

/*
 * Dirty a page.  Optimistically adjust accounting, on the assumption
 * that we won't race with invalidate.  If we do, readjust.
 */
static int ceph_set_page_dirty(struct page *page)
{
	struct address_space *mapping = page->mapping;
	struct inode *inode;
	struct ceph_inode_info *ci;
	struct ceph_snap_context *snapc;
	int ret;

	if (unlikely(!mapping))
		return !TestSetPageDirty(page);

	if (PageDirty(page)) {
		dout("%p set_page_dirty %p idx %lu -- already dirty\n",
		     mapping->host, page, page->index);
		BUG_ON(!PagePrivate(page));
		return 0;
	}

	inode = mapping->host;
	ci = ceph_inode(inode);

	/* dirty the head */
	spin_lock(&ci->i_ceph_lock);
	BUG_ON(ci->i_wr_ref == 0); // caller should hold Fw reference
	if (__ceph_have_pending_cap_snap(ci)) {
		struct ceph_cap_snap *capsnap =
				list_last_entry(&ci->i_cap_snaps,
						struct ceph_cap_snap,
						ci_item);
		snapc = ceph_get_snap_context(capsnap->context);
		capsnap->dirty_pages++;
	} else {
		BUG_ON(!ci->i_head_snapc);
		snapc = ceph_get_snap_context(ci->i_head_snapc);
		++ci->i_wrbuffer_ref_head;
	}
	if (ci->i_wrbuffer_ref == 0)
		ihold(inode);
	++ci->i_wrbuffer_ref;
	dout("%p set_page_dirty %p idx %lu head %d/%d -> %d/%d "
	     "snapc %p seq %lld (%d snaps)\n",
	     mapping->host, page, page->index,
	     ci->i_wrbuffer_ref-1, ci->i_wrbuffer_ref_head-1,
	     ci->i_wrbuffer_ref, ci->i_wrbuffer_ref_head,
	     snapc, snapc->seq, snapc->num_snaps);
	spin_unlock(&ci->i_ceph_lock);

	/*
	 * Reference snap context in page->private.  Also set
	 * PagePrivate so that we get invalidatepage callback.
	 */
	BUG_ON(PagePrivate(page));
	attach_page_private(page, snapc);

	ret = __set_page_dirty_nobuffers(page);
	WARN_ON(!PageLocked(page));
	WARN_ON(!page->mapping);

	return ret;
}

/*
 * If we are truncating the full page (i.e. offset == 0), adjust the
 * dirty page counters appropriately.  Only called if there is private
 * data on the page.
 */
static void ceph_invalidatepage(struct page *page, unsigned int offset,
				unsigned int length)
{
	struct inode *inode;
	struct ceph_inode_info *ci;
	struct ceph_snap_context *snapc;

	wait_on_page_fscache(page);

	inode = page->mapping->host;
	ci = ceph_inode(inode);

	if (offset != 0 || length != thp_size(page)) {
		dout("%p invalidatepage %p idx %lu partial dirty page %u~%u\n",
		     inode, page, page->index, offset, length);
		return;
	}

	WARN_ON(!PageLocked(page));
	if (!PagePrivate(page))
		return;

	dout("%p invalidatepage %p idx %lu full dirty page\n",
	     inode, page, page->index);

	snapc = detach_page_private(page);
	ceph_put_wrbuffer_cap_refs(ci, 1, snapc);
	ceph_put_snap_context(snapc);
}

static int ceph_releasepage(struct page *page, gfp_t gfp)
{
	dout("%p releasepage %p idx %lu (%sdirty)\n", page->mapping->host,
	     page, page->index, PageDirty(page) ? "" : "not ");

	if (PageFsCache(page)) {
		if (!(gfp & __GFP_DIRECT_RECLAIM) || !(gfp & __GFP_FS))
			return 0;
		wait_on_page_fscache(page);
	}
	return !PagePrivate(page);
}

static void ceph_netfs_expand_readahead(struct netfs_read_request *rreq)
{
	struct inode *inode = rreq->mapping->host;
	struct ceph_inode_info *ci = ceph_inode(inode);
	struct ceph_file_layout *lo = &ci->i_layout;
	u32 blockoff;
	u64 blockno;

	/* Expand the start downward */
	blockno = div_u64_rem(rreq->start, lo->stripe_unit, &blockoff);
	rreq->start = blockno * lo->stripe_unit;
	rreq->len += blockoff;

	/* Now, round up the length to the next block */
	rreq->len = roundup(rreq->len, lo->stripe_unit);
}

static bool ceph_netfs_clamp_length(struct netfs_read_subrequest *subreq)
{
	struct inode *inode = subreq->rreq->mapping->host;
	struct ceph_fs_client *fsc = ceph_inode_to_client(inode);
	struct ceph_inode_info *ci = ceph_inode(inode);
	u64 objno, objoff;
	u32 xlen;

	/* Truncate the extent at the end of the current block */
	ceph_calc_file_object_mapping(&ci->i_layout, subreq->start, subreq->len,
				      &objno, &objoff, &xlen);
	subreq->len = min(xlen, fsc->mount_options->rsize);
	return true;
}

static void finish_netfs_read(struct ceph_osd_request *req)
{
	struct ceph_fs_client *fsc = ceph_inode_to_client(req->r_inode);
	struct ceph_osd_data *osd_data = osd_req_op_extent_osd_data(req, 0);
	struct netfs_read_subrequest *subreq = req->r_priv;
	int num_pages;
	int err = req->r_result;

	ceph_update_read_metrics(&fsc->mdsc->metric, req->r_start_latency,
				 req->r_end_latency, err);

	dout("%s: result %d subreq->len=%zu i_size=%lld\n", __func__, req->r_result,
	     subreq->len, i_size_read(req->r_inode));

	/* no object means success but no data */
	if (err == -ENOENT)
		err = 0;
	else if (err == -EBLOCKLISTED)
		fsc->blocklisted = true;

	if (err >= 0 && err < subreq->len)
		__set_bit(NETFS_SREQ_CLEAR_TAIL, &subreq->flags);

	netfs_subreq_terminated(subreq, err, true);

	num_pages = calc_pages_for(osd_data->alignment, osd_data->length);
	ceph_put_page_vector(osd_data->pages, num_pages, false);
	iput(req->r_inode);
}

static void ceph_netfs_issue_op(struct netfs_read_subrequest *subreq)
{
	struct netfs_read_request *rreq = subreq->rreq;
	struct inode *inode = rreq->mapping->host;
	struct ceph_inode_info *ci = ceph_inode(inode);
	struct ceph_fs_client *fsc = ceph_inode_to_client(inode);
	struct ceph_osd_request *req;
	struct ceph_vino vino = ceph_vino(inode);
	struct iov_iter iter;
	struct page **pages;
	size_t page_off;
	int err = 0;
	u64 len = subreq->len;

	req = ceph_osdc_new_request(&fsc->client->osdc, &ci->i_layout, vino, subreq->start, &len,
			0, 1, CEPH_OSD_OP_READ,
			CEPH_OSD_FLAG_READ | fsc->client->osdc.client->options->read_from_replica,
			NULL, ci->i_truncate_seq, ci->i_truncate_size, false);
	if (IS_ERR(req)) {
		err = PTR_ERR(req);
		req = NULL;
		goto out;
	}

	dout("%s: pos=%llu orig_len=%zu len=%llu\n", __func__, subreq->start, subreq->len, len);
	iov_iter_xarray(&iter, READ, &rreq->mapping->i_pages, subreq->start, len);
	err = iov_iter_get_pages_alloc(&iter, &pages, len, &page_off);
	if (err < 0) {
		dout("%s: iov_ter_get_pages_alloc returned %d\n", __func__, err);
		goto out;
	}

	/* should always give us a page-aligned read */
	WARN_ON_ONCE(page_off);
	len = err;

	osd_req_op_extent_osd_data_pages(req, 0, pages, len, 0, false, false);
	req->r_callback = finish_netfs_read;
	req->r_priv = subreq;
	req->r_inode = inode;
	ihold(inode);

	err = ceph_osdc_start_request(req->r_osdc, req, false);
	if (err)
		iput(inode);
out:
	ceph_osdc_put_request(req);
	if (err)
		netfs_subreq_terminated(subreq, err, false);
	dout("%s: result %d\n", __func__, err);
}

static void ceph_init_rreq(struct netfs_read_request *rreq, struct file *file)
{
}

static void ceph_readahead_cleanup(struct address_space *mapping, void *priv)
{
	struct inode *inode = mapping->host;
	struct ceph_inode_info *ci = ceph_inode(inode);
	int got = (uintptr_t)priv;

	if (got)
		ceph_put_cap_refs(ci, got);
}

const struct netfs_read_request_ops ceph_netfs_read_ops = {
	.init_rreq		= ceph_init_rreq,
	.is_cache_enabled	= ceph_is_cache_enabled,
	.begin_cache_operation	= ceph_begin_cache_operation,
	.issue_op		= ceph_netfs_issue_op,
	.expand_readahead	= ceph_netfs_expand_readahead,
	.clamp_length		= ceph_netfs_clamp_length,
	.check_write_begin	= ceph_netfs_check_write_begin,
	.cleanup		= ceph_readahead_cleanup,
};

/* read a single page, without unlocking it. */
static int ceph_readpage(struct file *file, struct page *page)
{
	struct inode *inode = file_inode(file);
	struct ceph_inode_info *ci = ceph_inode(inode);
	struct ceph_vino vino = ceph_vino(inode);
	u64 off = page_offset(page);
	u64 len = thp_size(page);

	if (ci->i_inline_version != CEPH_INLINE_NONE) {
		/*
		 * Uptodate inline data should have been added
		 * into page cache while getting Fcr caps.
		 */
		if (off == 0) {
			unlock_page(page);
			return -EINVAL;
		}
		zero_user_segment(page, 0, thp_size(page));
		SetPageUptodate(page);
		unlock_page(page);
		return 0;
	}

	dout("readpage ino %llx.%llx file %p off %llu len %llu page %p index %lu\n",
	     vino.ino, vino.snap, file, off, len, page, page->index);

	return netfs_readpage(file, page, &ceph_netfs_read_ops, NULL);
}

static void ceph_readahead(struct readahead_control *ractl)
{
	struct inode *inode = file_inode(ractl->file);
	struct ceph_file_info *fi = ractl->file->private_data;
	struct ceph_rw_context *rw_ctx;
	int got = 0;
	int ret = 0;

	if (ceph_inode(inode)->i_inline_version != CEPH_INLINE_NONE)
		return;

	rw_ctx = ceph_find_rw_context(fi);
	if (!rw_ctx) {
		/*
		 * readahead callers do not necessarily hold Fcb caps
		 * (e.g. fadvise, madvise).
		 */
		int want = CEPH_CAP_FILE_CACHE;

		ret = ceph_try_get_caps(inode, CEPH_CAP_FILE_RD, want, true, &got);
		if (ret < 0)
			dout("start_read %p, error getting cap\n", inode);
		else if (!(got & want))
			dout("start_read %p, no cache cap\n", inode);

		if (ret <= 0)
			return;
	}
	netfs_readahead(ractl, &ceph_netfs_read_ops, (void *)(uintptr_t)got);
}

struct ceph_writeback_ctl
{
	loff_t i_size;
	u64 truncate_size;
	u32 truncate_seq;
	bool size_stable;
	bool head_snapc;
};

/*
 * Get ref for the oldest snapc for an inode with dirty data... that is, the
 * only snap context we are allowed to write back.
 */
static struct ceph_snap_context *
get_oldest_context(struct inode *inode, struct ceph_writeback_ctl *ctl,
		   struct ceph_snap_context *page_snapc)
{
	struct ceph_inode_info *ci = ceph_inode(inode);
	struct ceph_snap_context *snapc = NULL;
	struct ceph_cap_snap *capsnap = NULL;

	spin_lock(&ci->i_ceph_lock);
	list_for_each_entry(capsnap, &ci->i_cap_snaps, ci_item) {
		dout(" cap_snap %p snapc %p has %d dirty pages\n", capsnap,
		     capsnap->context, capsnap->dirty_pages);
		if (!capsnap->dirty_pages)
			continue;

		/* get i_size, truncate_{seq,size} for page_snapc? */
		if (snapc && capsnap->context != page_snapc)
			continue;

		if (ctl) {
			if (capsnap->writing) {
				ctl->i_size = i_size_read(inode);
				ctl->size_stable = false;
			} else {
				ctl->i_size = capsnap->size;
				ctl->size_stable = true;
			}
			ctl->truncate_size = capsnap->truncate_size;
			ctl->truncate_seq = capsnap->truncate_seq;
			ctl->head_snapc = false;
		}

		if (snapc)
			break;

		snapc = ceph_get_snap_context(capsnap->context);
		if (!page_snapc ||
		    page_snapc == snapc ||
		    page_snapc->seq > snapc->seq)
			break;
	}
	if (!snapc && ci->i_wrbuffer_ref_head) {
		snapc = ceph_get_snap_context(ci->i_head_snapc);
		dout(" head snapc %p has %d dirty pages\n",
		     snapc, ci->i_wrbuffer_ref_head);
		if (ctl) {
			ctl->i_size = i_size_read(inode);
			ctl->truncate_size = ci->i_truncate_size;
			ctl->truncate_seq = ci->i_truncate_seq;
			ctl->size_stable = false;
			ctl->head_snapc = true;
		}
	}
	spin_unlock(&ci->i_ceph_lock);
	return snapc;
}

static u64 get_writepages_data_length(struct inode *inode,
				      struct page *page, u64 start)
{
	struct ceph_inode_info *ci = ceph_inode(inode);
	struct ceph_snap_context *snapc = page_snap_context(page);
	struct ceph_cap_snap *capsnap = NULL;
	u64 end = i_size_read(inode);

	if (snapc != ci->i_head_snapc) {
		bool found = false;
		spin_lock(&ci->i_ceph_lock);
		list_for_each_entry(capsnap, &ci->i_cap_snaps, ci_item) {
			if (capsnap->context == snapc) {
				if (!capsnap->writing)
					end = capsnap->size;
				found = true;
				break;
			}
		}
		spin_unlock(&ci->i_ceph_lock);
		WARN_ON(!found);
	}
	if (end > page_offset(page) + thp_size(page))
		end = page_offset(page) + thp_size(page);
	return end > start ? end - start : 0;
}

/*
 * Write a single page, but leave the page locked.
 *
 * If we get a write error, mark the mapping for error, but still adjust the
 * dirty page accounting (i.e., page is no longer dirty).
 */
static int writepage_nounlock(struct page *page, struct writeback_control *wbc)
{
	struct inode *inode = page->mapping->host;
	struct ceph_inode_info *ci = ceph_inode(inode);
	struct ceph_fs_client *fsc = ceph_inode_to_client(inode);
	struct ceph_snap_context *snapc, *oldest;
	loff_t page_off = page_offset(page);
	int err;
	loff_t len = thp_size(page);
	struct ceph_writeback_ctl ceph_wbc;
	struct ceph_osd_client *osdc = &fsc->client->osdc;
	struct ceph_osd_request *req;

	dout("writepage %p idx %lu\n", page, page->index);

	/* verify this is a writeable snap context */
	snapc = page_snap_context(page);
	if (!snapc) {
		dout("writepage %p page %p not dirty?\n", inode, page);
		return 0;
	}
	oldest = get_oldest_context(inode, &ceph_wbc, snapc);
	if (snapc->seq > oldest->seq) {
		dout("writepage %p page %p snapc %p not writeable - noop\n",
		     inode, page, snapc);
		/* we should only noop if called by kswapd */
		WARN_ON(!(current->flags & PF_MEMALLOC));
		ceph_put_snap_context(oldest);
		redirty_page_for_writepage(wbc, page);
		return 0;
	}
	ceph_put_snap_context(oldest);

	/* is this a partial page at end of file? */
	if (page_off >= ceph_wbc.i_size) {
		dout("%p page eof %llu\n", page, ceph_wbc.i_size);
		page->mapping->a_ops->invalidatepage(page, 0, thp_size(page));
		return 0;
	}

	if (ceph_wbc.i_size < page_off + len)
		len = ceph_wbc.i_size - page_off;

	dout("writepage %p page %p index %lu on %llu~%llu snapc %p seq %lld\n",
	     inode, page, page->index, page_off, len, snapc, snapc->seq);

	if (atomic_long_inc_return(&fsc->writeback_count) >
	    CONGESTION_ON_THRESH(fsc->mount_options->congestion_kb))
		set_bdi_congested(inode_to_bdi(inode), BLK_RW_ASYNC);

	set_page_writeback(page);
	req = ceph_osdc_new_request(osdc, &ci->i_layout, ceph_vino(inode), page_off, &len, 0, 1,
				    CEPH_OSD_OP_WRITE, CEPH_OSD_FLAG_WRITE, snapc,
				    ceph_wbc.truncate_seq, ceph_wbc.truncate_size,
				    true);
	if (IS_ERR(req)) {
		redirty_page_for_writepage(wbc, page);
		end_page_writeback(page);
		return PTR_ERR(req);
	}

	/* it may be a short write due to an object boundary */
	WARN_ON_ONCE(len > thp_size(page));
	osd_req_op_extent_osd_data_pages(req, 0, &page, len, 0, false, false);
	dout("writepage %llu~%llu (%llu bytes)\n", page_off, len, len);

	req->r_mtime = inode->i_mtime;
	err = ceph_osdc_start_request(osdc, req, true);
	if (!err)
		err = ceph_osdc_wait_request(osdc, req);

	ceph_update_write_metrics(&fsc->mdsc->metric, req->r_start_latency,
				  req->r_end_latency, err);

	ceph_osdc_put_request(req);
	if (err == 0)
		err = len;

	if (err < 0) {
		struct writeback_control tmp_wbc;
		if (!wbc)
			wbc = &tmp_wbc;
		if (err == -ERESTARTSYS) {
			/* killed by SIGKILL */
			dout("writepage interrupted page %p\n", page);
			redirty_page_for_writepage(wbc, page);
			end_page_writeback(page);
			return err;
		}
		if (err == -EBLOCKLISTED)
			fsc->blocklisted = true;
		dout("writepage setting page/mapping error %d %p\n",
		     err, page);
		mapping_set_error(&inode->i_data, err);
		wbc->pages_skipped++;
	} else {
		dout("writepage cleaned page %p\n", page);
		err = 0;  /* vfs expects us to return 0 */
	}
	oldest = detach_page_private(page);
	WARN_ON_ONCE(oldest != snapc);
	end_page_writeback(page);
	ceph_put_wrbuffer_cap_refs(ci, 1, snapc);
	ceph_put_snap_context(snapc);  /* page's reference */

	if (atomic_long_dec_return(&fsc->writeback_count) <
	    CONGESTION_OFF_THRESH(fsc->mount_options->congestion_kb))
		clear_bdi_congested(inode_to_bdi(inode), BLK_RW_ASYNC);

	return err;
}

static int ceph_writepage(struct page *page, struct writeback_control *wbc)
{
	int err;
	struct inode *inode = page->mapping->host;
	BUG_ON(!inode);
	ihold(inode);
	err = writepage_nounlock(page, wbc);
	if (err == -ERESTARTSYS) {
		/* direct memory reclaimer was killed by SIGKILL. return 0
		 * to prevent caller from setting mapping/page error */
		err = 0;
	}
	unlock_page(page);
	iput(inode);
	return err;
}

/*
 * async writeback completion handler.
 *
 * If we get an error, set the mapping error bit, but not the individual
 * page error bits.
 */
static void writepages_finish(struct ceph_osd_request *req)
{
	struct inode *inode = req->r_inode;
	struct ceph_inode_info *ci = ceph_inode(inode);
	struct ceph_osd_data *osd_data;
	struct page *page;
	int num_pages, total_pages = 0;
	int i, j;
	int rc = req->r_result;
	struct ceph_snap_context *snapc = req->r_snapc;
	struct address_space *mapping = inode->i_mapping;
	struct ceph_fs_client *fsc = ceph_inode_to_client(inode);
	bool remove_page;

	dout("writepages_finish %p rc %d\n", inode, rc);
	if (rc < 0) {
		mapping_set_error(mapping, rc);
		ceph_set_error_write(ci);
		if (rc == -EBLOCKLISTED)
			fsc->blocklisted = true;
	} else {
		ceph_clear_error_write(ci);
	}

	ceph_update_write_metrics(&fsc->mdsc->metric, req->r_start_latency,
				  req->r_end_latency, rc);

	/*
	 * We lost the cache cap, need to truncate the page before
	 * it is unlocked, otherwise we'd truncate it later in the
	 * page truncation thread, possibly losing some data that
	 * raced its way in
	 */
	remove_page = !(ceph_caps_issued(ci) &
			(CEPH_CAP_FILE_CACHE|CEPH_CAP_FILE_LAZYIO));

	/* clean all pages */
	for (i = 0; i < req->r_num_ops; i++) {
		if (req->r_ops[i].op != CEPH_OSD_OP_WRITE)
			break;

		osd_data = osd_req_op_extent_osd_data(req, i);
		BUG_ON(osd_data->type != CEPH_OSD_DATA_TYPE_PAGES);
		num_pages = calc_pages_for((u64)osd_data->alignment,
					   (u64)osd_data->length);
		total_pages += num_pages;
		for (j = 0; j < num_pages; j++) {
			page = osd_data->pages[j];
			BUG_ON(!page);
			WARN_ON(!PageUptodate(page));

			if (atomic_long_dec_return(&fsc->writeback_count) <
			     CONGESTION_OFF_THRESH(
					fsc->mount_options->congestion_kb))
				clear_bdi_congested(inode_to_bdi(inode),
						    BLK_RW_ASYNC);

			ceph_put_snap_context(detach_page_private(page));
			end_page_writeback(page);
			dout("unlocking %p\n", page);

			if (remove_page)
				generic_error_remove_page(inode->i_mapping,
							  page);

			unlock_page(page);
		}
		dout("writepages_finish %p wrote %llu bytes cleaned %d pages\n",
		     inode, osd_data->length, rc >= 0 ? num_pages : 0);

		release_pages(osd_data->pages, num_pages);
	}

	ceph_put_wrbuffer_cap_refs(ci, total_pages, snapc);

	osd_data = osd_req_op_extent_osd_data(req, 0);
	if (osd_data->pages_from_pool)
		mempool_free(osd_data->pages, ceph_wb_pagevec_pool);
	else
		kfree(osd_data->pages);
	ceph_osdc_put_request(req);
}

/*
 * initiate async writeback
 */
static int ceph_writepages_start(struct address_space *mapping,
				 struct writeback_control *wbc)
{
	struct inode *inode = mapping->host;
	struct ceph_inode_info *ci = ceph_inode(inode);
	struct ceph_fs_client *fsc = ceph_inode_to_client(inode);
	struct ceph_vino vino = ceph_vino(inode);
	pgoff_t index, start_index, end = -1;
	struct ceph_snap_context *snapc = NULL, *last_snapc = NULL, *pgsnapc;
	struct pagevec pvec;
	int rc = 0;
	unsigned int wsize = i_blocksize(inode);
	struct ceph_osd_request *req = NULL;
	struct ceph_writeback_ctl ceph_wbc;
	bool should_loop, range_whole = false;
	bool done = false;

	dout("writepages_start %p (mode=%s)\n", inode,
	     wbc->sync_mode == WB_SYNC_NONE ? "NONE" :
	     (wbc->sync_mode == WB_SYNC_ALL ? "ALL" : "HOLD"));

	if (READ_ONCE(fsc->mount_state) >= CEPH_MOUNT_SHUTDOWN) {
		if (ci->i_wrbuffer_ref > 0) {
			pr_warn_ratelimited(
				"writepage_start %p %lld forced umount\n",
				inode, ceph_ino(inode));
		}
		mapping_set_error(mapping, -EIO);
		return -EIO; /* we're in a forced umount, don't write! */
	}
	if (fsc->mount_options->wsize < wsize)
		wsize = fsc->mount_options->wsize;

	pagevec_init(&pvec);

	start_index = wbc->range_cyclic ? mapping->writeback_index : 0;
	index = start_index;

retry:
	/* find oldest snap context with dirty data */
	snapc = get_oldest_context(inode, &ceph_wbc, NULL);
	if (!snapc) {
		/* hmm, why does writepages get called when there
		   is no dirty data? */
		dout(" no snap context with dirty data?\n");
		goto out;
	}
	dout(" oldest snapc is %p seq %lld (%d snaps)\n",
	     snapc, snapc->seq, snapc->num_snaps);

	should_loop = false;
	if (ceph_wbc.head_snapc && snapc != last_snapc) {
		/* where to start/end? */
		if (wbc->range_cyclic) {
			index = start_index;
			end = -1;
			if (index > 0)
				should_loop = true;
			dout(" cyclic, start at %lu\n", index);
		} else {
			index = wbc->range_start >> PAGE_SHIFT;
			end = wbc->range_end >> PAGE_SHIFT;
			if (wbc->range_start == 0 && wbc->range_end == LLONG_MAX)
				range_whole = true;
			dout(" not cyclic, %lu to %lu\n", index, end);
		}
	} else if (!ceph_wbc.head_snapc) {
		/* Do not respect wbc->range_{start,end}. Dirty pages
		 * in that range can be associated with newer snapc.
		 * They are not writeable until we write all dirty pages
		 * associated with 'snapc' get written */
		if (index > 0)
			should_loop = true;
		dout(" non-head snapc, range whole\n");
	}

	ceph_put_snap_context(last_snapc);
	last_snapc = snapc;

	while (!done && index <= end) {
		int num_ops = 0, op_idx;
		unsigned i, pvec_pages, max_pages, locked_pages = 0;
		struct page **pages = NULL, **data_pages;
		struct page *page;
		pgoff_t strip_unit_end = 0;
		u64 offset = 0, len = 0;
		bool from_pool = false;

		max_pages = wsize >> PAGE_SHIFT;

get_more_pages:
		pvec_pages = pagevec_lookup_range_tag(&pvec, mapping, &index,
						end, PAGECACHE_TAG_DIRTY);
		dout("pagevec_lookup_range_tag got %d\n", pvec_pages);
		if (!pvec_pages && !locked_pages)
			break;
		for (i = 0; i < pvec_pages && locked_pages < max_pages; i++) {
			page = pvec.pages[i];
			dout("? %p idx %lu\n", page, page->index);
			if (locked_pages == 0)
				lock_page(page);  /* first page */
			else if (!trylock_page(page))
				break;

			/* only dirty pages, or our accounting breaks */
			if (unlikely(!PageDirty(page)) ||
			    unlikely(page->mapping != mapping)) {
				dout("!dirty or !mapping %p\n", page);
				unlock_page(page);
				continue;
			}
			/* only if matching snap context */
			pgsnapc = page_snap_context(page);
			if (pgsnapc != snapc) {
				dout("page snapc %p %lld != oldest %p %lld\n",
				     pgsnapc, pgsnapc->seq, snapc, snapc->seq);
				if (!should_loop &&
				    !ceph_wbc.head_snapc &&
				    wbc->sync_mode != WB_SYNC_NONE)
					should_loop = true;
				unlock_page(page);
				continue;
			}
			if (page_offset(page) >= ceph_wbc.i_size) {
				dout("%p page eof %llu\n",
				     page, ceph_wbc.i_size);
				if ((ceph_wbc.size_stable ||
				    page_offset(page) >= i_size_read(inode)) &&
				    clear_page_dirty_for_io(page))
					mapping->a_ops->invalidatepage(page,
								0, thp_size(page));
				unlock_page(page);
				continue;
			}
			if (strip_unit_end && (page->index > strip_unit_end)) {
				dout("end of strip unit %p\n", page);
				unlock_page(page);
				break;
			}
			if (PageWriteback(page)) {
				if (wbc->sync_mode == WB_SYNC_NONE) {
					dout("%p under writeback\n", page);
					unlock_page(page);
					continue;
				}
				dout("waiting on writeback %p\n", page);
				wait_on_page_writeback(page);
			}

			if (!clear_page_dirty_for_io(page)) {
				dout("%p !clear_page_dirty_for_io\n", page);
				unlock_page(page);
				continue;
			}

			/*
			 * We have something to write.  If this is
			 * the first locked page this time through,
			 * calculate max possinle write size and
			 * allocate a page array
			 */
			if (locked_pages == 0) {
				u64 objnum;
				u64 objoff;
				u32 xlen;

				/* prepare async write request */
				offset = (u64)page_offset(page);
				ceph_calc_file_object_mapping(&ci->i_layout,
							      offset, wsize,
							      &objnum, &objoff,
							      &xlen);
				len = xlen;

				num_ops = 1;
				strip_unit_end = page->index +
					((len - 1) >> PAGE_SHIFT);

				BUG_ON(pages);
				max_pages = calc_pages_for(0, (u64)len);
				pages = kmalloc_array(max_pages,
						      sizeof(*pages),
						      GFP_NOFS);
				if (!pages) {
					from_pool = true;
					pages = mempool_alloc(ceph_wb_pagevec_pool, GFP_NOFS);
					BUG_ON(!pages);
				}

				len = 0;
			} else if (page->index !=
				   (offset + len) >> PAGE_SHIFT) {
				if (num_ops >= (from_pool ?  CEPH_OSD_SLAB_OPS :
							     CEPH_OSD_MAX_OPS)) {
					redirty_page_for_writepage(wbc, page);
					unlock_page(page);
					break;
				}

				num_ops++;
				offset = (u64)page_offset(page);
				len = 0;
			}

			/* note position of first page in pvec */
			dout("%p will write page %p idx %lu\n",
			     inode, page, page->index);

			if (atomic_long_inc_return(&fsc->writeback_count) >
			    CONGESTION_ON_THRESH(
				    fsc->mount_options->congestion_kb)) {
				set_bdi_congested(inode_to_bdi(inode),
						  BLK_RW_ASYNC);
			}


			pages[locked_pages++] = page;
			pvec.pages[i] = NULL;

			len += thp_size(page);
		}

		/* did we get anything? */
		if (!locked_pages)
			goto release_pvec_pages;
		if (i) {
			unsigned j, n = 0;
			/* shift unused page to beginning of pvec */
			for (j = 0; j < pvec_pages; j++) {
				if (!pvec.pages[j])
					continue;
				if (n < j)
					pvec.pages[n] = pvec.pages[j];
				n++;
			}
			pvec.nr = n;

			if (pvec_pages && i == pvec_pages &&
			    locked_pages < max_pages) {
				dout("reached end pvec, trying for more\n");
				pagevec_release(&pvec);
				goto get_more_pages;
			}
		}

new_request:
		offset = page_offset(pages[0]);
		len = wsize;

		req = ceph_osdc_new_request(&fsc->client->osdc,
					&ci->i_layout, vino,
					offset, &len, 0, num_ops,
					CEPH_OSD_OP_WRITE, CEPH_OSD_FLAG_WRITE,
					snapc, ceph_wbc.truncate_seq,
					ceph_wbc.truncate_size, false);
		if (IS_ERR(req)) {
			req = ceph_osdc_new_request(&fsc->client->osdc,
						&ci->i_layout, vino,
						offset, &len, 0,
						min(num_ops,
						    CEPH_OSD_SLAB_OPS),
						CEPH_OSD_OP_WRITE,
						CEPH_OSD_FLAG_WRITE,
						snapc, ceph_wbc.truncate_seq,
						ceph_wbc.truncate_size, true);
			BUG_ON(IS_ERR(req));
		}
		BUG_ON(len < page_offset(pages[locked_pages - 1]) +
			     thp_size(page) - offset);

		req->r_callback = writepages_finish;
		req->r_inode = inode;

		/* Format the osd request message and submit the write */
		len = 0;
		data_pages = pages;
		op_idx = 0;
		for (i = 0; i < locked_pages; i++) {
			u64 cur_offset = page_offset(pages[i]);
			if (offset + len != cur_offset) {
				if (op_idx + 1 == req->r_num_ops)
					break;
				osd_req_op_extent_dup_last(req, op_idx,
							   cur_offset - offset);
				dout("writepages got pages at %llu~%llu\n",
				     offset, len);
				osd_req_op_extent_osd_data_pages(req, op_idx,
							data_pages, len, 0,
							from_pool, false);
				osd_req_op_extent_update(req, op_idx, len);

				len = 0;
				offset = cur_offset; 
				data_pages = pages + i;
				op_idx++;
			}

			set_page_writeback(pages[i]);
			len += thp_size(page);
		}

		if (ceph_wbc.size_stable) {
			len = min(len, ceph_wbc.i_size - offset);
		} else if (i == locked_pages) {
			/* writepages_finish() clears writeback pages
			 * according to the data length, so make sure
			 * data length covers all locked pages */
			u64 min_len = len + 1 - thp_size(page);
			len = get_writepages_data_length(inode, pages[i - 1],
							 offset);
			len = max(len, min_len);
		}
		dout("writepages got pages at %llu~%llu\n", offset, len);

		osd_req_op_extent_osd_data_pages(req, op_idx, data_pages, len,
						 0, from_pool, false);
		osd_req_op_extent_update(req, op_idx, len);

		BUG_ON(op_idx + 1 != req->r_num_ops);

		from_pool = false;
		if (i < locked_pages) {
			BUG_ON(num_ops <= req->r_num_ops);
			num_ops -= req->r_num_ops;
			locked_pages -= i;

			/* allocate new pages array for next request */
			data_pages = pages;
			pages = kmalloc_array(locked_pages, sizeof(*pages),
					      GFP_NOFS);
			if (!pages) {
				from_pool = true;
				pages = mempool_alloc(ceph_wb_pagevec_pool, GFP_NOFS);
				BUG_ON(!pages);
			}
			memcpy(pages, data_pages + i,
			       locked_pages * sizeof(*pages));
			memset(data_pages + i, 0,
			       locked_pages * sizeof(*pages));
		} else {
			BUG_ON(num_ops != req->r_num_ops);
			index = pages[i - 1]->index + 1;
			/* request message now owns the pages array */
			pages = NULL;
		}

		req->r_mtime = inode->i_mtime;
		rc = ceph_osdc_start_request(&fsc->client->osdc, req, true);
		BUG_ON(rc);
		req = NULL;

		wbc->nr_to_write -= i;
		if (pages)
			goto new_request;

		/*
		 * We stop writing back only if we are not doing
		 * integrity sync. In case of integrity sync we have to
		 * keep going until we have written all the pages
		 * we tagged for writeback prior to entering this loop.
		 */
		if (wbc->nr_to_write <= 0 && wbc->sync_mode == WB_SYNC_NONE)
			done = true;

release_pvec_pages:
		dout("pagevec_release on %d pages (%p)\n", (int)pvec.nr,
		     pvec.nr ? pvec.pages[0] : NULL);
		pagevec_release(&pvec);
	}

	if (should_loop && !done) {
		/* more to do; loop back to beginning of file */
		dout("writepages looping back to beginning of file\n");
		end = start_index - 1; /* OK even when start_index == 0 */

		/* to write dirty pages associated with next snapc,
		 * we need to wait until current writes complete */
		if (wbc->sync_mode != WB_SYNC_NONE &&
		    start_index == 0 && /* all dirty pages were checked */
		    !ceph_wbc.head_snapc) {
			struct page *page;
			unsigned i, nr;
			index = 0;
			while ((index <= end) &&
			       (nr = pagevec_lookup_tag(&pvec, mapping, &index,
						PAGECACHE_TAG_WRITEBACK))) {
				for (i = 0; i < nr; i++) {
					page = pvec.pages[i];
					if (page_snap_context(page) != snapc)
						continue;
					wait_on_page_writeback(page);
				}
				pagevec_release(&pvec);
				cond_resched();
			}
		}

		start_index = 0;
		index = 0;
		goto retry;
	}

	if (wbc->range_cyclic || (range_whole && wbc->nr_to_write > 0))
		mapping->writeback_index = index;

out:
	ceph_osdc_put_request(req);
	ceph_put_snap_context(last_snapc);
	dout("writepages dend - startone, rc = %d\n", rc);
	return rc;
}



/*
 * See if a given @snapc is either writeable, or already written.
 */
static int context_is_writeable_or_written(struct inode *inode,
					   struct ceph_snap_context *snapc)
{
	struct ceph_snap_context *oldest = get_oldest_context(inode, NULL, NULL);
	int ret = !oldest || snapc->seq <= oldest->seq;

	ceph_put_snap_context(oldest);
	return ret;
}

/**
 * ceph_find_incompatible - find an incompatible context and return it
 * @page: page being dirtied
 *
 * We are only allowed to write into/dirty a page if the page is
 * clean, or already dirty within the same snap context. Returns a
 * conflicting context if there is one, NULL if there isn't, or a
 * negative error code on other errors.
 *
 * Must be called with page lock held.
 */
static struct ceph_snap_context *
ceph_find_incompatible(struct page *page)
{
	struct inode *inode = page->mapping->host;
	struct ceph_fs_client *fsc = ceph_inode_to_client(inode);
	struct ceph_inode_info *ci = ceph_inode(inode);

	if (READ_ONCE(fsc->mount_state) >= CEPH_MOUNT_SHUTDOWN) {
		dout(" page %p forced umount\n", page);
		return ERR_PTR(-EIO);
	}

	for (;;) {
		struct ceph_snap_context *snapc, *oldest;

		wait_on_page_writeback(page);

		snapc = page_snap_context(page);
		if (!snapc || snapc == ci->i_head_snapc)
			break;

		/*
		 * this page is already dirty in another (older) snap
		 * context!  is it writeable now?
		 */
		oldest = get_oldest_context(inode, NULL, NULL);
		if (snapc->seq > oldest->seq) {
			/* not writeable -- return it for the caller to deal with */
			ceph_put_snap_context(oldest);
			dout(" page %p snapc %p not current or oldest\n", page, snapc);
			return ceph_get_snap_context(snapc);
		}
		ceph_put_snap_context(oldest);

		/* yay, writeable, do it now (without dropping page lock) */
		dout(" page %p snapc %p not current, but oldest\n", page, snapc);
		if (clear_page_dirty_for_io(page)) {
			int r = writepage_nounlock(page, NULL);
			if (r < 0)
				return ERR_PTR(r);
		}
	}
	return NULL;
}

static int ceph_netfs_check_write_begin(struct file *file, loff_t pos, unsigned int len,
					struct page *page, void **_fsdata)
{
	struct inode *inode = file_inode(file);
	struct ceph_inode_info *ci = ceph_inode(inode);
	struct ceph_snap_context *snapc;

	snapc = ceph_find_incompatible(page);
	if (snapc) {
		int r;

		unlock_page(page);
		put_page(page);
		if (IS_ERR(snapc))
			return PTR_ERR(snapc);

		ceph_queue_writeback(inode);
		r = wait_event_killable(ci->i_cap_wq,
					context_is_writeable_or_written(inode, snapc));
		ceph_put_snap_context(snapc);
		return r == 0 ? -EAGAIN : r;
	}
	return 0;
}

/*
 * We are only allowed to write into/dirty the page if the page is
 * clean, or already dirty within the same snap context.
 */
static int ceph_write_begin(struct file *file, struct address_space *mapping,
			    loff_t pos, unsigned len, unsigned flags,
			    struct page **pagep, void **fsdata)
{
	struct inode *inode = file_inode(file);
	struct ceph_inode_info *ci = ceph_inode(inode);
	struct page *page = NULL;
	pgoff_t index = pos >> PAGE_SHIFT;
	int r;

	/*
	 * Uninlining should have already been done and everything updated, EXCEPT
	 * for inline_version sent to the MDS.
	 */
	if (ci->i_inline_version != CEPH_INLINE_NONE) {
		page = grab_cache_page_write_begin(mapping, index, flags);
		if (!page)
			return -ENOMEM;

		/*
		 * The inline_version on a new inode is set to 1. If that's the
		 * case, then the page is brand new and isn't yet Uptodate.
		 */
		r = 0;
		if (index == 0 && ci->i_inline_version != 1) {
			if (!PageUptodate(page)) {
				WARN_ONCE(1, "ceph: write_begin called on still-inlined inode (inline_version %llu)!\n",
					  ci->i_inline_version);
				r = -EINVAL;
			}
			goto out;
		}
		zero_user_segment(page, 0, thp_size(page));
		SetPageUptodate(page);
		goto out;
	}

	r = netfs_write_begin(file, inode->i_mapping, pos, len, 0, &page, NULL,
			      &ceph_netfs_read_ops, NULL);
out:
	if (r == 0)
		wait_on_page_fscache(page);
	if (r < 0) {
		if (page)
			put_page(page);
	} else {
		WARN_ON_ONCE(!PageLocked(page));
		*pagep = page;
	}
	return r;
}

/*
 * we don't do anything in here that simple_write_end doesn't do
 * except adjust dirty page accounting
 */
static int ceph_write_end(struct file *file, struct address_space *mapping,
			  loff_t pos, unsigned len, unsigned copied,
			  struct page *page, void *fsdata)
{
	struct inode *inode = file_inode(file);
	bool check_cap = false;

	dout("write_end file %p inode %p page %p %d~%d (%d)\n", file,
	     inode, page, (int)pos, (int)copied, (int)len);

	/* zero the stale part of the page if we did a short copy */
	if (!PageUptodate(page)) {
		if (copied < len) {
			copied = 0;
			goto out;
		}
		SetPageUptodate(page);
	}

	/* did file size increase? */
	if (pos+copied > i_size_read(inode))
		check_cap = ceph_inode_set_size(inode, pos+copied);

	set_page_dirty(page);

out:
	unlock_page(page);
	put_page(page);

	if (check_cap)
		ceph_check_caps(ceph_inode(inode), CHECK_CAPS_AUTHONLY, NULL);

	return copied;
}

/*
 * we set .direct_IO to indicate direct io is supported, but since we
 * intercept O_DIRECT reads and writes early, this function should
 * never get called.
 */
static ssize_t ceph_direct_io(struct kiocb *iocb, struct iov_iter *iter)
{
	WARN_ON(1);
	return -EINVAL;
}

const struct address_space_operations ceph_aops = {
	.readpage = ceph_readpage,
	.readahead = ceph_readahead,
	.writepage = ceph_writepage,
	.writepages = ceph_writepages_start,
	.write_begin = ceph_write_begin,
	.write_end = ceph_write_end,
	.set_page_dirty = ceph_set_page_dirty,
	.invalidatepage = ceph_invalidatepage,
	.releasepage = ceph_releasepage,
	.direct_IO = ceph_direct_io,
};

static void ceph_block_sigs(sigset_t *oldset)
{
	sigset_t mask;
	siginitsetinv(&mask, sigmask(SIGKILL));
	sigprocmask(SIG_BLOCK, &mask, oldset);
}

static void ceph_restore_sigs(sigset_t *oldset)
{
	sigprocmask(SIG_SETMASK, oldset, NULL);
}

/*
 * vm ops
 */
static vm_fault_t ceph_filemap_fault(struct vm_fault *vmf)
{
	struct vm_area_struct *vma = vmf->vma;
	struct inode *inode = file_inode(vma->vm_file);
	struct ceph_inode_info *ci = ceph_inode(inode);
	struct ceph_file_info *fi = vma->vm_file->private_data;
	loff_t off = (loff_t)vmf->pgoff << PAGE_SHIFT;
	int want, got, err;
	sigset_t oldset;
	vm_fault_t ret = VM_FAULT_SIGBUS;

	ceph_block_sigs(&oldset);

	dout("filemap_fault %p %llx.%llx %llu trying to get caps\n",
	     inode, ceph_vinop(inode), off);
	if (fi->fmode & CEPH_FILE_MODE_LAZY)
		want = CEPH_CAP_FILE_CACHE | CEPH_CAP_FILE_LAZYIO;
	else
		want = CEPH_CAP_FILE_CACHE;

	got = 0;
	err = ceph_get_caps(vma->vm_file, CEPH_CAP_FILE_RD, want, -1, &got);
	if (err < 0)
		goto out_restore;

	dout("filemap_fault %p %llu got cap refs on %s\n",
	     inode, off, ceph_cap_string(got));

	if ((got & (CEPH_CAP_FILE_CACHE | CEPH_CAP_FILE_LAZYIO)) ||
	    ci->i_inline_version == CEPH_INLINE_NONE) {
		CEPH_DEFINE_RW_CONTEXT(rw_ctx, got);
		ceph_add_rw_context(fi, &rw_ctx);
		ret = filemap_fault(vmf);
		ceph_del_rw_context(fi, &rw_ctx);
		dout("filemap_fault %p %llu drop cap refs %s ret %x\n",
		     inode, off, ceph_cap_string(got), ret);
	} else
		err = -EAGAIN;

	ceph_put_cap_refs(ci, got);

	if (err != -EAGAIN)
		goto out_restore;

	/* read inline data */
	if (off >= PAGE_SIZE) {
		/* does not support inline data > PAGE_SIZE */
		ret = VM_FAULT_SIGBUS;
	} else {
		struct address_space *mapping = inode->i_mapping;
		struct page *page = find_or_create_page(mapping, 0,
						mapping_gfp_constraint(mapping,
						~__GFP_FS));
		if (!page) {
			ret = VM_FAULT_OOM;
			goto out_inline;
		}
		err = __ceph_do_getattr(inode, page,
					 CEPH_STAT_CAP_INLINE_DATA, true);
		if (err < 0 || off >= i_size_read(inode)) {
			unlock_page(page);
			put_page(page);
			ret = vmf_error(err);
			goto out_inline;
		}
		if (err < PAGE_SIZE)
			zero_user_segment(page, err, PAGE_SIZE);
		else
			flush_dcache_page(page);
		SetPageUptodate(page);
		vmf->page = page;
		ret = VM_FAULT_MAJOR | VM_FAULT_LOCKED;
out_inline:
		dout("filemap_fault %p %llu read inline data ret %x\n",
		     inode, off, ret);
	}
out_restore:
	ceph_restore_sigs(&oldset);
	if (err < 0)
		ret = vmf_error(err);

	return ret;
}

static vm_fault_t ceph_page_mkwrite(struct vm_fault *vmf)
{
	struct vm_area_struct *vma = vmf->vma;
	struct inode *inode = file_inode(vma->vm_file);
	struct ceph_inode_info *ci = ceph_inode(inode);
	struct ceph_file_info *fi = vma->vm_file->private_data;
	struct ceph_cap_flush *prealloc_cf;
	struct page *page = vmf->page;
	loff_t off = page_offset(page);
	loff_t size = i_size_read(inode);
	size_t len;
	int want, got, err;
	sigset_t oldset;
	vm_fault_t ret = VM_FAULT_SIGBUS;

	prealloc_cf = ceph_alloc_cap_flush();
	if (!prealloc_cf)
		return VM_FAULT_OOM;

	sb_start_pagefault(inode->i_sb);
	ceph_block_sigs(&oldset);

	if (ci->i_inline_version != CEPH_INLINE_NONE) {
		struct page *locked_page = NULL;
		if (off == 0) {
			lock_page(page);
			locked_page = page;
		}
		err = ceph_uninline_data(vma->vm_file, locked_page);
		if (locked_page)
			unlock_page(locked_page);
		if (err < 0)
			goto out_free;
	}

	if (off + thp_size(page) <= size)
		len = thp_size(page);
	else
		len = offset_in_thp(page, size);

	dout("page_mkwrite %p %llx.%llx %llu~%zd getting caps i_size %llu\n",
	     inode, ceph_vinop(inode), off, len, size);
	if (fi->fmode & CEPH_FILE_MODE_LAZY)
		want = CEPH_CAP_FILE_BUFFER | CEPH_CAP_FILE_LAZYIO;
	else
		want = CEPH_CAP_FILE_BUFFER;

	got = 0;
	err = ceph_get_caps(vma->vm_file, CEPH_CAP_FILE_WR, want, off + len, &got);
	if (err < 0)
		goto out_free;

	dout("page_mkwrite %p %llu~%zd got cap refs on %s\n",
	     inode, off, len, ceph_cap_string(got));

	/* Update time before taking page lock */
	file_update_time(vma->vm_file);
	inode_inc_iversion_raw(inode);

	do {
		struct ceph_snap_context *snapc;

		lock_page(page);

		if (page_mkwrite_check_truncate(page, inode) < 0) {
			unlock_page(page);
			ret = VM_FAULT_NOPAGE;
			break;
		}

		snapc = ceph_find_incompatible(page);
		if (!snapc) {
			/* success.  we'll keep the page locked. */
			set_page_dirty(page);
			ret = VM_FAULT_LOCKED;
			break;
		}

		unlock_page(page);

		if (IS_ERR(snapc)) {
			ret = VM_FAULT_SIGBUS;
			break;
		}

		ceph_queue_writeback(inode);
		err = wait_event_killable(ci->i_cap_wq,
				context_is_writeable_or_written(inode, snapc));
		ceph_put_snap_context(snapc);
	} while (err == 0);

	if (ret == VM_FAULT_LOCKED ||
	    ci->i_inline_version != CEPH_INLINE_NONE) {
		int dirty;
		spin_lock(&ci->i_ceph_lock);
		ci->i_inline_version = CEPH_INLINE_NONE;
		dirty = __ceph_mark_dirty_caps(ci, CEPH_CAP_FILE_WR,
					       &prealloc_cf);
		spin_unlock(&ci->i_ceph_lock);
		if (dirty)
			__mark_inode_dirty(inode, dirty);
	}

	dout("page_mkwrite %p %llu~%zd dropping cap refs on %s ret %x\n",
	     inode, off, len, ceph_cap_string(got), ret);
	ceph_put_cap_refs_async(ci, got);
out_free:
	ceph_restore_sigs(&oldset);
	sb_end_pagefault(inode->i_sb);
	ceph_free_cap_flush(prealloc_cf);
	if (err < 0)
		ret = vmf_error(err);
	return ret;
}

void ceph_fill_inline_data(struct inode *inode, struct page *locked_page,
			   char	*data, size_t len)
{
	struct address_space *mapping = inode->i_mapping;
	struct page *page;

	if (locked_page) {
		page = locked_page;
	} else {
		if (i_size_read(inode) == 0)
			return;
		page = find_or_create_page(mapping, 0,
					   mapping_gfp_constraint(mapping,
					   ~__GFP_FS));
		if (!page)
			return;
		if (PageUptodate(page)) {
			unlock_page(page);
			put_page(page);
			return;
		}
	}

	dout("fill_inline_data %p %llx.%llx len %zu locked_page %p\n",
	     inode, ceph_vinop(inode), len, locked_page);

	if (len > 0) {
		void *kaddr = kmap_atomic(page);
		memcpy(kaddr, data, len);
		kunmap_atomic(kaddr);
	}

	if (page != locked_page) {
		if (len < PAGE_SIZE)
			zero_user_segment(page, len, PAGE_SIZE);
		else
			flush_dcache_page(page);

		SetPageUptodate(page);
		unlock_page(page);
		put_page(page);
	}
}

int ceph_uninline_data(struct file *filp, struct page *locked_page)
{
	struct inode *inode = file_inode(filp);
	struct ceph_inode_info *ci = ceph_inode(inode);
	struct ceph_fs_client *fsc = ceph_inode_to_client(inode);
	struct ceph_osd_request *req;
	struct page *page = NULL;
	u64 len, inline_version;
	int err = 0;
	bool from_pagecache = false;

	spin_lock(&ci->i_ceph_lock);
	inline_version = ci->i_inline_version;
	spin_unlock(&ci->i_ceph_lock);

	dout("uninline_data %p %llx.%llx inline_version %llu\n",
	     inode, ceph_vinop(inode), inline_version);

	if (inline_version == 1 || /* initial version, no data */
	    inline_version == CEPH_INLINE_NONE)
		goto out;

	if (locked_page) {
		page = locked_page;
		WARN_ON(!PageUptodate(page));
	} else if (ceph_caps_issued(ci) &
		   (CEPH_CAP_FILE_CACHE|CEPH_CAP_FILE_LAZYIO)) {
		page = find_get_page(inode->i_mapping, 0);
		if (page) {
			if (PageUptodate(page)) {
				from_pagecache = true;
				lock_page(page);
			} else {
				put_page(page);
				page = NULL;
			}
		}
	}

	if (page) {
		len = i_size_read(inode);
		if (len > PAGE_SIZE)
			len = PAGE_SIZE;
	} else {
		page = __page_cache_alloc(GFP_NOFS);
		if (!page) {
			err = -ENOMEM;
			goto out;
		}
		err = __ceph_do_getattr(inode, page,
					CEPH_STAT_CAP_INLINE_DATA, true);
		if (err < 0) {
			/* no inline data */
			if (err == -ENODATA)
				err = 0;
			goto out;
		}
		len = err;
	}

	req = ceph_osdc_new_request(&fsc->client->osdc, &ci->i_layout,
				    ceph_vino(inode), 0, &len, 0, 1,
				    CEPH_OSD_OP_CREATE, CEPH_OSD_FLAG_WRITE,
				    NULL, 0, 0, false);
	if (IS_ERR(req)) {
		err = PTR_ERR(req);
		goto out;
	}

	req->r_mtime = inode->i_mtime;
	err = ceph_osdc_start_request(&fsc->client->osdc, req, false);
	if (!err)
		err = ceph_osdc_wait_request(&fsc->client->osdc, req);
	ceph_osdc_put_request(req);
	if (err < 0)
		goto out;

	req = ceph_osdc_new_request(&fsc->client->osdc, &ci->i_layout,
				    ceph_vino(inode), 0, &len, 1, 3,
				    CEPH_OSD_OP_WRITE, CEPH_OSD_FLAG_WRITE,
				    NULL, ci->i_truncate_seq,
				    ci->i_truncate_size, false);
	if (IS_ERR(req)) {
		err = PTR_ERR(req);
		goto out;
	}

	osd_req_op_extent_osd_data_pages(req, 1, &page, len, 0, false, false);

	{
		__le64 xattr_buf = cpu_to_le64(inline_version);
		err = osd_req_op_xattr_init(req, 0, CEPH_OSD_OP_CMPXATTR,
					    "inline_version", &xattr_buf,
					    sizeof(xattr_buf),
					    CEPH_OSD_CMPXATTR_OP_GT,
					    CEPH_OSD_CMPXATTR_MODE_U64);
		if (err)
			goto out_put;
	}

	{
		char xattr_buf[32];
		int xattr_len = snprintf(xattr_buf, sizeof(xattr_buf),
					 "%llu", inline_version);
		err = osd_req_op_xattr_init(req, 2, CEPH_OSD_OP_SETXATTR,
					    "inline_version",
					    xattr_buf, xattr_len, 0, 0);
		if (err)
			goto out_put;
	}

	req->r_mtime = inode->i_mtime;
	err = ceph_osdc_start_request(&fsc->client->osdc, req, false);
	if (!err)
		err = ceph_osdc_wait_request(&fsc->client->osdc, req);

	ceph_update_write_metrics(&fsc->mdsc->metric, req->r_start_latency,
				  req->r_end_latency, err);

out_put:
	ceph_osdc_put_request(req);
	if (err == -ECANCELED)
		err = 0;
out:
	if (page && page != locked_page) {
		if (from_pagecache) {
			unlock_page(page);
			put_page(page);
		} else
			__free_pages(page, 0);
	}

	dout("uninline_data %p %llx.%llx inline_version %llu = %d\n",
	     inode, ceph_vinop(inode), inline_version, err);
	return err;
}

static const struct vm_operations_struct ceph_vmops = {
	.fault		= ceph_filemap_fault,
	.page_mkwrite	= ceph_page_mkwrite,
};

int ceph_mmap(struct file *file, struct vm_area_struct *vma)
{
	struct address_space *mapping = file->f_mapping;

	if (!mapping->a_ops->readpage)
		return -ENOEXEC;
	file_accessed(file);
	vma->vm_ops = &ceph_vmops;
	return 0;
}

enum {
	POOL_READ	= 1,
	POOL_WRITE	= 2,
};

static int __ceph_pool_perm_get(struct ceph_inode_info *ci,
				s64 pool, struct ceph_string *pool_ns)
{
	struct ceph_fs_client *fsc = ceph_inode_to_client(&ci->vfs_inode);
	struct ceph_mds_client *mdsc = fsc->mdsc;
	struct ceph_osd_request *rd_req = NULL, *wr_req = NULL;
	struct rb_node **p, *parent;
	struct ceph_pool_perm *perm;
	struct page **pages;
	size_t pool_ns_len;
	int err = 0, err2 = 0, have = 0;

	down_read(&mdsc->pool_perm_rwsem);
	p = &mdsc->pool_perm_tree.rb_node;
	while (*p) {
		perm = rb_entry(*p, struct ceph_pool_perm, node);
		if (pool < perm->pool)
			p = &(*p)->rb_left;
		else if (pool > perm->pool)
			p = &(*p)->rb_right;
		else {
			int ret = ceph_compare_string(pool_ns,
						perm->pool_ns,
						perm->pool_ns_len);
			if (ret < 0)
				p = &(*p)->rb_left;
			else if (ret > 0)
				p = &(*p)->rb_right;
			else {
				have = perm->perm;
				break;
			}
		}
	}
	up_read(&mdsc->pool_perm_rwsem);
	if (*p)
		goto out;

	if (pool_ns)
		dout("__ceph_pool_perm_get pool %lld ns %.*s no perm cached\n",
		     pool, (int)pool_ns->len, pool_ns->str);
	else
		dout("__ceph_pool_perm_get pool %lld no perm cached\n", pool);

	down_write(&mdsc->pool_perm_rwsem);
	p = &mdsc->pool_perm_tree.rb_node;
	parent = NULL;
	while (*p) {
		parent = *p;
		perm = rb_entry(parent, struct ceph_pool_perm, node);
		if (pool < perm->pool)
			p = &(*p)->rb_left;
		else if (pool > perm->pool)
			p = &(*p)->rb_right;
		else {
			int ret = ceph_compare_string(pool_ns,
						perm->pool_ns,
						perm->pool_ns_len);
			if (ret < 0)
				p = &(*p)->rb_left;
			else if (ret > 0)
				p = &(*p)->rb_right;
			else {
				have = perm->perm;
				break;
			}
		}
	}
	if (*p) {
		up_write(&mdsc->pool_perm_rwsem);
		goto out;
	}

	rd_req = ceph_osdc_alloc_request(&fsc->client->osdc, NULL,
					 1, false, GFP_NOFS);
	if (!rd_req) {
		err = -ENOMEM;
		goto out_unlock;
	}

	rd_req->r_flags = CEPH_OSD_FLAG_READ;
	osd_req_op_init(rd_req, 0, CEPH_OSD_OP_STAT, 0);
	rd_req->r_base_oloc.pool = pool;
	if (pool_ns)
		rd_req->r_base_oloc.pool_ns = ceph_get_string(pool_ns);
	ceph_oid_printf(&rd_req->r_base_oid, "%llx.00000000", ci->i_vino.ino);

	err = ceph_osdc_alloc_messages(rd_req, GFP_NOFS);
	if (err)
		goto out_unlock;

	wr_req = ceph_osdc_alloc_request(&fsc->client->osdc, NULL,
					 1, false, GFP_NOFS);
	if (!wr_req) {
		err = -ENOMEM;
		goto out_unlock;
	}

	wr_req->r_flags = CEPH_OSD_FLAG_WRITE;
	osd_req_op_init(wr_req, 0, CEPH_OSD_OP_CREATE, CEPH_OSD_OP_FLAG_EXCL);
	ceph_oloc_copy(&wr_req->r_base_oloc, &rd_req->r_base_oloc);
	ceph_oid_copy(&wr_req->r_base_oid, &rd_req->r_base_oid);

	err = ceph_osdc_alloc_messages(wr_req, GFP_NOFS);
	if (err)
		goto out_unlock;

	/* one page should be large enough for STAT data */
	pages = ceph_alloc_page_vector(1, GFP_KERNEL);
	if (IS_ERR(pages)) {
		err = PTR_ERR(pages);
		goto out_unlock;
	}

	osd_req_op_raw_data_in_pages(rd_req, 0, pages, PAGE_SIZE,
				     0, false, true);
	err = ceph_osdc_start_request(&fsc->client->osdc, rd_req, false);

	wr_req->r_mtime = ci->vfs_inode.i_mtime;
	err2 = ceph_osdc_start_request(&fsc->client->osdc, wr_req, false);

	if (!err)
		err = ceph_osdc_wait_request(&fsc->client->osdc, rd_req);
	if (!err2)
		err2 = ceph_osdc_wait_request(&fsc->client->osdc, wr_req);

	if (err >= 0 || err == -ENOENT)
		have |= POOL_READ;
	else if (err != -EPERM) {
		if (err == -EBLOCKLISTED)
			fsc->blocklisted = true;
		goto out_unlock;
	}

	if (err2 == 0 || err2 == -EEXIST)
		have |= POOL_WRITE;
	else if (err2 != -EPERM) {
		if (err2 == -EBLOCKLISTED)
			fsc->blocklisted = true;
		err = err2;
		goto out_unlock;
	}

	pool_ns_len = pool_ns ? pool_ns->len : 0;
	perm = kmalloc(sizeof(*perm) + pool_ns_len + 1, GFP_NOFS);
	if (!perm) {
		err = -ENOMEM;
		goto out_unlock;
	}

	perm->pool = pool;
	perm->perm = have;
	perm->pool_ns_len = pool_ns_len;
	if (pool_ns_len > 0)
		memcpy(perm->pool_ns, pool_ns->str, pool_ns_len);
	perm->pool_ns[pool_ns_len] = 0;

	rb_link_node(&perm->node, parent, p);
	rb_insert_color(&perm->node, &mdsc->pool_perm_tree);
	err = 0;
out_unlock:
	up_write(&mdsc->pool_perm_rwsem);

	ceph_osdc_put_request(rd_req);
	ceph_osdc_put_request(wr_req);
out:
	if (!err)
		err = have;
	if (pool_ns)
		dout("__ceph_pool_perm_get pool %lld ns %.*s result = %d\n",
		     pool, (int)pool_ns->len, pool_ns->str, err);
	else
		dout("__ceph_pool_perm_get pool %lld result = %d\n", pool, err);
	return err;
}

int ceph_pool_perm_check(struct inode *inode, int need)
{
	struct ceph_inode_info *ci = ceph_inode(inode);
	struct ceph_string *pool_ns;
	s64 pool;
	int ret, flags;

	/* Only need to do this for regular files */
	if (!S_ISREG(inode->i_mode))
		return 0;

	if (ci->i_vino.snap != CEPH_NOSNAP) {
		/*
		 * Pool permission check needs to write to the first object.
		 * But for snapshot, head of the first object may have alread
		 * been deleted. Skip check to avoid creating orphan object.
		 */
		return 0;
	}

	if (ceph_test_mount_opt(ceph_inode_to_client(inode),
				NOPOOLPERM))
		return 0;

	spin_lock(&ci->i_ceph_lock);
	flags = ci->i_ceph_flags;
	pool = ci->i_layout.pool_id;
	spin_unlock(&ci->i_ceph_lock);
check:
	if (flags & CEPH_I_POOL_PERM) {
		if ((need & CEPH_CAP_FILE_RD) && !(flags & CEPH_I_POOL_RD)) {
			dout("ceph_pool_perm_check pool %lld no read perm\n",
			     pool);
			return -EPERM;
		}
		if ((need & CEPH_CAP_FILE_WR) && !(flags & CEPH_I_POOL_WR)) {
			dout("ceph_pool_perm_check pool %lld no write perm\n",
			     pool);
			return -EPERM;
		}
		return 0;
	}

	pool_ns = ceph_try_get_string(ci->i_layout.pool_ns);
	ret = __ceph_pool_perm_get(ci, pool, pool_ns);
	ceph_put_string(pool_ns);
	if (ret < 0)
		return ret;

	flags = CEPH_I_POOL_PERM;
	if (ret & POOL_READ)
		flags |= CEPH_I_POOL_RD;
	if (ret & POOL_WRITE)
		flags |= CEPH_I_POOL_WR;

	spin_lock(&ci->i_ceph_lock);
	if (pool == ci->i_layout.pool_id &&
	    pool_ns == rcu_dereference_raw(ci->i_layout.pool_ns)) {
		ci->i_ceph_flags |= flags;
        } else {
		pool = ci->i_layout.pool_id;
		flags = ci->i_ceph_flags;
	}
	spin_unlock(&ci->i_ceph_lock);
	goto check;
}

void ceph_pool_perm_destroy(struct ceph_mds_client *mdsc)
{
	struct ceph_pool_perm *perm;
	struct rb_node *n;

	while (!RB_EMPTY_ROOT(&mdsc->pool_perm_tree)) {
		n = rb_first(&mdsc->pool_perm_tree);
		perm = rb_entry(n, struct ceph_pool_perm, node);
		rb_erase(n, &mdsc->pool_perm_tree);
		kfree(perm);
	}
}
