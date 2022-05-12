// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *  Copyright (C) 2001  Andrea Arcangeli <andrea@suse.de> SuSE
 *  Copyright (C) 2016 - 2020 Christoph Hellwig
 */

#include <linux/init.h>
#include <linux/mm.h>
#include <linux/fcntl.h>
#include <linux/slab.h>
#include <linux/kmod.h>
#include <linux/major.h>
#include <linux/device_cgroup.h>
#include <linux/highmem.h>
#include <linux/blkdev.h>
#include <linux/backing-dev.h>
#include <linux/module.h>
#include <linux/blkpg.h>
#include <linux/magic.h>
#include <linux/buffer_head.h>
#include <linux/swap.h>
#include <linux/pagevec.h>
#include <linux/writeback.h>
#include <linux/mpage.h>
#include <linux/mount.h>
#include <linux/pseudo_fs.h>
#include <linux/uio.h>
#include <linux/namei.h>
#include <linux/log2.h>
#include <linux/cleancache.h>
#include <linux/task_io_accounting_ops.h>
#include <linux/falloc.h>
#include <linux/part_stat.h>
#include <linux/uaccess.h>
#include <linux/suspend.h>
#include "internal.h"

struct bdev_inode {
	struct block_device bdev;
	struct inode vfs_inode;
};

static const struct address_space_operations def_blk_aops;

static inline struct bdev_inode *BDEV_I(struct inode *inode)
{
	return container_of(inode, struct bdev_inode, vfs_inode);
}

struct block_device *I_BDEV(struct inode *inode)
{
	return &BDEV_I(inode)->bdev;
}
EXPORT_SYMBOL(I_BDEV);

static void bdev_write_inode(struct block_device *bdev)
{
	struct inode *inode = bdev->bd_inode;
	int ret;

	spin_lock(&inode->i_lock);
	while (inode->i_state & I_DIRTY) {
		spin_unlock(&inode->i_lock);
		ret = write_inode_now(inode, true);
		if (ret) {
			char name[BDEVNAME_SIZE];
			pr_warn_ratelimited("VFS: Dirty inode writeback failed "
					    "for block device %s (err=%d).\n",
					    bdevname(bdev, name), ret);
		}
		spin_lock(&inode->i_lock);
	}
	spin_unlock(&inode->i_lock);
}

/* Kill _all_ buffers and pagecache , dirty or not.. */
static void kill_bdev(struct block_device *bdev)
{
	struct address_space *mapping = bdev->bd_inode->i_mapping;

	if (mapping_empty(mapping))
		return;

	invalidate_bh_lrus();
	truncate_inode_pages(mapping, 0);
}

/* Invalidate clean unused buffers and pagecache. */
void invalidate_bdev(struct block_device *bdev)
{
	struct address_space *mapping = bdev->bd_inode->i_mapping;

	if (mapping->nrpages) {
		invalidate_bh_lrus();
		lru_add_drain_all();	/* make sure all lru add caches are flushed */
		invalidate_mapping_pages(mapping, 0, -1);
	}
	/* 99% of the time, we don't need to flush the cleancache on the bdev.
	 * But, for the strange corners, lets be cautious
	 */
	cleancache_invalidate_inode(mapping);
}
EXPORT_SYMBOL(invalidate_bdev);

/*
 * Drop all buffers & page cache for given bdev range. This function bails
 * with error if bdev has other exclusive owner (such as filesystem).
 */
int truncate_bdev_range(struct block_device *bdev, fmode_t mode,
			loff_t lstart, loff_t lend)
{
	/*
	 * If we don't hold exclusive handle for the device, upgrade to it
	 * while we discard the buffer cache to avoid discarding buffers
	 * under live filesystem.
	 */
	if (!(mode & FMODE_EXCL)) {
		int err = bd_prepare_to_claim(bdev, truncate_bdev_range);
		if (err)
			goto invalidate;
	}

	truncate_inode_pages_range(bdev->bd_inode->i_mapping, lstart, lend);
	if (!(mode & FMODE_EXCL))
		bd_abort_claiming(bdev, truncate_bdev_range);
	return 0;

invalidate:
	/*
	 * Someone else has handle exclusively open. Try invalidating instead.
	 * The 'end' argument is inclusive so the rounding is safe.
	 */
	return invalidate_inode_pages2_range(bdev->bd_inode->i_mapping,
					     lstart >> PAGE_SHIFT,
					     lend >> PAGE_SHIFT);
}

static void set_init_blocksize(struct block_device *bdev)
{
	unsigned int bsize = bdev_logical_block_size(bdev);
	loff_t size = i_size_read(bdev->bd_inode);

	while (bsize < PAGE_SIZE) {
		if (size & bsize)
			break;
		bsize <<= 1;
	}
	bdev->bd_inode->i_blkbits = blksize_bits(bsize);
}

int set_blocksize(struct block_device *bdev, int size)
{
	/* Size must be a power of two, and between 512 and PAGE_SIZE */
	if (size > PAGE_SIZE || size < 512 || !is_power_of_2(size))
		return -EINVAL;

	/* Size cannot be smaller than the size supported by the device */
	if (size < bdev_logical_block_size(bdev))
		return -EINVAL;

	/* Don't change the size if it is same as current */
	if (bdev->bd_inode->i_blkbits != blksize_bits(size)) {
		sync_blockdev(bdev);
		bdev->bd_inode->i_blkbits = blksize_bits(size);
		kill_bdev(bdev);
	}
	return 0;
}

EXPORT_SYMBOL(set_blocksize);

int sb_set_blocksize(struct super_block *sb, int size)
{
	if (set_blocksize(sb->s_bdev, size))
		return 0;
	/* If we get here, we know size is power of two
	 * and it's value is between 512 and PAGE_SIZE */
	sb->s_blocksize = size;
	sb->s_blocksize_bits = blksize_bits(size);
	return sb->s_blocksize;
}

EXPORT_SYMBOL(sb_set_blocksize);

int sb_min_blocksize(struct super_block *sb, int size)
{
	int minsize = bdev_logical_block_size(sb->s_bdev);
	if (size < minsize)
		size = minsize;
	return sb_set_blocksize(sb, size);
}

EXPORT_SYMBOL(sb_min_blocksize);

static int
blkdev_get_block(struct inode *inode, sector_t iblock,
		struct buffer_head *bh, int create)
{
	bh->b_bdev = I_BDEV(inode);
	bh->b_blocknr = iblock;
	set_buffer_mapped(bh);
	return 0;
}

static struct inode *bdev_file_inode(struct file *file)
{
	return file->f_mapping->host;
}

static unsigned int dio_bio_write_op(struct kiocb *iocb)
{
	unsigned int op = REQ_OP_WRITE | REQ_SYNC | REQ_IDLE;

	/* avoid the need for a I/O completion work item */
	if (iocb->ki_flags & IOCB_DSYNC)
		op |= REQ_FUA;
	return op;
}

#define DIO_INLINE_BIO_VECS 4

static void blkdev_bio_end_io_simple(struct bio *bio)
{
	struct task_struct *waiter = bio->bi_private;

	WRITE_ONCE(bio->bi_private, NULL);
	blk_wake_io_task(waiter);
}

static ssize_t
__blkdev_direct_IO_simple(struct kiocb *iocb, struct iov_iter *iter,
		unsigned int nr_pages)
{
	struct file *file = iocb->ki_filp;
	struct block_device *bdev = I_BDEV(bdev_file_inode(file));
	struct bio_vec inline_vecs[DIO_INLINE_BIO_VECS], *vecs;
	loff_t pos = iocb->ki_pos;
	bool should_dirty = false;
	struct bio bio;
	ssize_t ret;
	blk_qc_t qc;

	if ((pos | iov_iter_alignment(iter)) &
	    (bdev_logical_block_size(bdev) - 1))
		return -EINVAL;

	if (nr_pages <= DIO_INLINE_BIO_VECS)
		vecs = inline_vecs;
	else {
		vecs = kmalloc_array(nr_pages, sizeof(struct bio_vec),
				     GFP_KERNEL);
		if (!vecs)
			return -ENOMEM;
	}

	bio_init(&bio, vecs, nr_pages);
	bio_set_dev(&bio, bdev);
	bio.bi_iter.bi_sector = pos >> 9;
	bio.bi_write_hint = iocb->ki_hint;
	bio.bi_private = current;
	bio.bi_end_io = blkdev_bio_end_io_simple;
	bio.bi_ioprio = iocb->ki_ioprio;

	ret = bio_iov_iter_get_pages(&bio, iter);
	if (unlikely(ret))
		goto out;
	ret = bio.bi_iter.bi_size;

	if (iov_iter_rw(iter) == READ) {
		bio.bi_opf = REQ_OP_READ;
		if (iter_is_iovec(iter))
			should_dirty = true;
	} else {
		bio.bi_opf = dio_bio_write_op(iocb);
		task_io_account_write(ret);
	}
	if (iocb->ki_flags & IOCB_NOWAIT)
		bio.bi_opf |= REQ_NOWAIT;
	if (iocb->ki_flags & IOCB_HIPRI)
		bio_set_polled(&bio, iocb);

	qc = submit_bio(&bio);
	for (;;) {
		set_current_state(TASK_UNINTERRUPTIBLE);
		if (!READ_ONCE(bio.bi_private))
			break;
		if (!(iocb->ki_flags & IOCB_HIPRI) ||
		    !blk_poll(bdev_get_queue(bdev), qc, true))
			blk_io_schedule();
	}
	__set_current_state(TASK_RUNNING);

	bio_release_pages(&bio, should_dirty);
	if (unlikely(bio.bi_status))
		ret = blk_status_to_errno(bio.bi_status);

out:
	if (vecs != inline_vecs)
		kfree(vecs);

	bio_uninit(&bio);

	return ret;
}

struct blkdev_dio {
	union {
		struct kiocb		*iocb;
		struct task_struct	*waiter;
	};
	size_t			size;
	atomic_t		ref;
	bool			multi_bio : 1;
	bool			should_dirty : 1;
	bool			is_sync : 1;
	struct bio		bio;
};

static struct bio_set blkdev_dio_pool;

static int blkdev_iopoll(struct kiocb *kiocb, bool wait)
{
	struct block_device *bdev = I_BDEV(kiocb->ki_filp->f_mapping->host);
	struct request_queue *q = bdev_get_queue(bdev);

	return blk_poll(q, READ_ONCE(kiocb->ki_cookie), wait);
}

static void blkdev_bio_end_io(struct bio *bio)
{
	struct blkdev_dio *dio = bio->bi_private;
	bool should_dirty = dio->should_dirty;

	if (bio->bi_status && !dio->bio.bi_status)
		dio->bio.bi_status = bio->bi_status;

	if (!dio->multi_bio || atomic_dec_and_test(&dio->ref)) {
		if (!dio->is_sync) {
			struct kiocb *iocb = dio->iocb;
			ssize_t ret;

			if (likely(!dio->bio.bi_status)) {
				ret = dio->size;
				iocb->ki_pos += ret;
			} else {
				ret = blk_status_to_errno(dio->bio.bi_status);
			}

			dio->iocb->ki_complete(iocb, ret, 0);
			if (dio->multi_bio)
				bio_put(&dio->bio);
		} else {
			struct task_struct *waiter = dio->waiter;

			WRITE_ONCE(dio->waiter, NULL);
			blk_wake_io_task(waiter);
		}
	}

	if (should_dirty) {
		bio_check_pages_dirty(bio);
	} else {
		bio_release_pages(bio, false);
		bio_put(bio);
	}
}

static ssize_t __blkdev_direct_IO(struct kiocb *iocb, struct iov_iter *iter,
		unsigned int nr_pages)
{
	struct file *file = iocb->ki_filp;
	struct inode *inode = bdev_file_inode(file);
	struct block_device *bdev = I_BDEV(inode);
	struct blk_plug plug;
	struct blkdev_dio *dio;
	struct bio *bio;
	bool is_poll = (iocb->ki_flags & IOCB_HIPRI) != 0;
	bool is_read = (iov_iter_rw(iter) == READ), is_sync;
	loff_t pos = iocb->ki_pos;
	blk_qc_t qc = BLK_QC_T_NONE;
	int ret = 0;

	if ((pos | iov_iter_alignment(iter)) &
	    (bdev_logical_block_size(bdev) - 1))
		return -EINVAL;

	bio = bio_alloc_bioset(GFP_KERNEL, nr_pages, &blkdev_dio_pool);

	dio = container_of(bio, struct blkdev_dio, bio);
	dio->is_sync = is_sync = is_sync_kiocb(iocb);
	if (dio->is_sync) {
		dio->waiter = current;
		bio_get(bio);
	} else {
		dio->iocb = iocb;
	}

	dio->size = 0;
	dio->multi_bio = false;
	dio->should_dirty = is_read && iter_is_iovec(iter);

	/*
	 * Don't plug for HIPRI/polled IO, as those should go straight
	 * to issue
	 */
	if (!is_poll)
		blk_start_plug(&plug);

	for (;;) {
		bio_set_dev(bio, bdev);
		bio->bi_iter.bi_sector = pos >> 9;
		bio->bi_write_hint = iocb->ki_hint;
		bio->bi_private = dio;
		bio->bi_end_io = blkdev_bio_end_io;
		bio->bi_ioprio = iocb->ki_ioprio;

		ret = bio_iov_iter_get_pages(bio, iter);
		if (unlikely(ret)) {
			bio->bi_status = BLK_STS_IOERR;
			bio_endio(bio);
			break;
		}

		if (is_read) {
			bio->bi_opf = REQ_OP_READ;
			if (dio->should_dirty)
				bio_set_pages_dirty(bio);
		} else {
			bio->bi_opf = dio_bio_write_op(iocb);
			task_io_account_write(bio->bi_iter.bi_size);
		}
		if (iocb->ki_flags & IOCB_NOWAIT)
			bio->bi_opf |= REQ_NOWAIT;

		dio->size += bio->bi_iter.bi_size;
		pos += bio->bi_iter.bi_size;

		nr_pages = bio_iov_vecs_to_alloc(iter, BIO_MAX_VECS);
		if (!nr_pages) {
			bool polled = false;

			if (iocb->ki_flags & IOCB_HIPRI) {
				bio_set_polled(bio, iocb);
				polled = true;
			}

			qc = submit_bio(bio);

			if (polled)
				WRITE_ONCE(iocb->ki_cookie, qc);
			break;
		}

		if (!dio->multi_bio) {
			/*
			 * AIO needs an extra reference to ensure the dio
			 * structure which is embedded into the first bio
			 * stays around.
			 */
			if (!is_sync)
				bio_get(bio);
			dio->multi_bio = true;
			atomic_set(&dio->ref, 2);
		} else {
			atomic_inc(&dio->ref);
		}

		submit_bio(bio);
		bio = bio_alloc(GFP_KERNEL, nr_pages);
	}

	if (!is_poll)
		blk_finish_plug(&plug);

	if (!is_sync)
		return -EIOCBQUEUED;

	for (;;) {
		set_current_state(TASK_UNINTERRUPTIBLE);
		if (!READ_ONCE(dio->waiter))
			break;

		if (!(iocb->ki_flags & IOCB_HIPRI) ||
		    !blk_poll(bdev_get_queue(bdev), qc, true))
			blk_io_schedule();
	}
	__set_current_state(TASK_RUNNING);

	if (!ret)
		ret = blk_status_to_errno(dio->bio.bi_status);
	if (likely(!ret))
		ret = dio->size;

	bio_put(&dio->bio);
	return ret;
}

static ssize_t
blkdev_direct_IO(struct kiocb *iocb, struct iov_iter *iter)
{
	unsigned int nr_pages;

	if (!iov_iter_count(iter))
		return 0;

	nr_pages = bio_iov_vecs_to_alloc(iter, BIO_MAX_VECS + 1);
	if (is_sync_kiocb(iocb) && nr_pages <= BIO_MAX_VECS)
		return __blkdev_direct_IO_simple(iocb, iter, nr_pages);

	return __blkdev_direct_IO(iocb, iter, bio_max_segs(nr_pages));
}

static __init int blkdev_init(void)
{
	return bioset_init(&blkdev_dio_pool, 4, offsetof(struct blkdev_dio, bio), BIOSET_NEED_BVECS);
}
module_init(blkdev_init);

int __sync_blockdev(struct block_device *bdev, int wait)
{
	if (!bdev)
		return 0;
	if (!wait)
		return filemap_flush(bdev->bd_inode->i_mapping);
	return filemap_write_and_wait(bdev->bd_inode->i_mapping);
}

/*
 * Write out and wait upon all the dirty data associated with a block
 * device via its mapping.  Does not take the superblock lock.
 */
int sync_blockdev(struct block_device *bdev)
{
	return __sync_blockdev(bdev, 1);
}
EXPORT_SYMBOL(sync_blockdev);

/*
 * Write out and wait upon all dirty data associated with this
 * device.   Filesystem data as well as the underlying block
 * device.  Takes the superblock lock.
 */
int fsync_bdev(struct block_device *bdev)
{
	struct super_block *sb = get_super(bdev);
	if (sb) {
		int res = sync_filesystem(sb);
		drop_super(sb);
		return res;
	}
	return sync_blockdev(bdev);
}
EXPORT_SYMBOL(fsync_bdev);

/**
 * freeze_bdev  --  lock a filesystem and force it into a consistent state
 * @bdev:	blockdevice to lock
 *
 * If a superblock is found on this device, we take the s_umount semaphore
 * on it to make sure nobody unmounts until the snapshot creation is done.
 * The reference counter (bd_fsfreeze_count) guarantees that only the last
 * unfreeze process can unfreeze the frozen filesystem actually when multiple
 * freeze requests arrive simultaneously. It counts up in freeze_bdev() and
 * count down in thaw_bdev(). When it becomes 0, thaw_bdev() will unfreeze
 * actually.
 */
int freeze_bdev(struct block_device *bdev)
{
	struct super_block *sb;
	int error = 0;

	mutex_lock(&bdev->bd_fsfreeze_mutex);
	if (++bdev->bd_fsfreeze_count > 1)
		goto done;

	sb = get_active_super(bdev);
	if (!sb)
		goto sync;
	if (sb->s_op->freeze_super)
		error = sb->s_op->freeze_super(sb);
	else
		error = freeze_super(sb);
	deactivate_super(sb);

	if (error) {
		bdev->bd_fsfreeze_count--;
		goto done;
	}
	bdev->bd_fsfreeze_sb = sb;

sync:
	sync_blockdev(bdev);
done:
	mutex_unlock(&bdev->bd_fsfreeze_mutex);
	return error;
}
EXPORT_SYMBOL(freeze_bdev);

/**
 * thaw_bdev  -- unlock filesystem
 * @bdev:	blockdevice to unlock
 *
 * Unlocks the filesystem and marks it writeable again after freeze_bdev().
 */
int thaw_bdev(struct block_device *bdev)
{
	struct super_block *sb;
	int error = -EINVAL;

	mutex_lock(&bdev->bd_fsfreeze_mutex);
	if (!bdev->bd_fsfreeze_count)
		goto out;

	error = 0;
	if (--bdev->bd_fsfreeze_count > 0)
		goto out;

	sb = bdev->bd_fsfreeze_sb;
	if (!sb)
		goto out;

	if (sb->s_op->thaw_super)
		error = sb->s_op->thaw_super(sb);
	else
		error = thaw_super(sb);
	if (error)
		bdev->bd_fsfreeze_count++;
	else
		bdev->bd_fsfreeze_sb = NULL;
out:
	mutex_unlock(&bdev->bd_fsfreeze_mutex);
	return error;
}
EXPORT_SYMBOL(thaw_bdev);

static int blkdev_writepage(struct page *page, struct writeback_control *wbc)
{
	return block_write_full_page(page, blkdev_get_block, wbc);
}

static int blkdev_readpage(struct file * file, struct page * page)
{
	return block_read_full_page(page, blkdev_get_block);
}

static void blkdev_readahead(struct readahead_control *rac)
{
	mpage_readahead(rac, blkdev_get_block);
}

static int blkdev_write_begin(struct file *file, struct address_space *mapping,
			loff_t pos, unsigned len, unsigned flags,
			struct page **pagep, void **fsdata)
{
	return block_write_begin(mapping, pos, len, flags, pagep,
				 blkdev_get_block);
}

static int blkdev_write_end(struct file *file, struct address_space *mapping,
			loff_t pos, unsigned len, unsigned copied,
			struct page *page, void *fsdata)
{
	int ret;
	ret = block_write_end(file, mapping, pos, len, copied, page, fsdata);

	unlock_page(page);
	put_page(page);

	return ret;
}

/*
 * private llseek:
 * for a block special file file_inode(file)->i_size is zero
 * so we compute the size by hand (just as in block_read/write above)
 */
static loff_t block_llseek(struct file *file, loff_t offset, int whence)
{
	struct inode *bd_inode = bdev_file_inode(file);
	loff_t retval;

	inode_lock(bd_inode);
	retval = fixed_size_llseek(file, offset, whence, i_size_read(bd_inode));
	inode_unlock(bd_inode);
	return retval;
}
	
int blkdev_fsync(struct file *filp, loff_t start, loff_t end, int datasync)
{
	struct inode *bd_inode = bdev_file_inode(filp);
	struct block_device *bdev = I_BDEV(bd_inode);
	int error;
	
	error = file_write_and_wait_range(filp, start, end);
	if (error)
		return error;

	/*
	 * There is no need to serialise calls to blkdev_issue_flush with
	 * i_mutex and doing so causes performance issues with concurrent
	 * O_SYNC writers to a block device.
	 */
	error = blkdev_issue_flush(bdev);
	if (error == -EOPNOTSUPP)
		error = 0;

	return error;
}
EXPORT_SYMBOL(blkdev_fsync);

/**
 * bdev_read_page() - Start reading a page from a block device
 * @bdev: The device to read the page from
 * @sector: The offset on the device to read the page to (need not be aligned)
 * @page: The page to read
 *
 * On entry, the page should be locked.  It will be unlocked when the page
 * has been read.  If the block driver implements rw_page synchronously,
 * that will be true on exit from this function, but it need not be.
 *
 * Errors returned by this function are usually "soft", eg out of memory, or
 * queue full; callers should try a different route to read this page rather
 * than propagate an error back up the stack.
 *
 * Return: negative errno if an error occurs, 0 if submission was successful.
 */
int bdev_read_page(struct block_device *bdev, sector_t sector,
			struct page *page)
{
	const struct block_device_operations *ops = bdev->bd_disk->fops;
	int result = -EOPNOTSUPP;

	if (!ops->rw_page || bdev_get_integrity(bdev))
		return result;

	result = blk_queue_enter(bdev->bd_disk->queue, 0);
	if (result)
		return result;
	result = ops->rw_page(bdev, sector + get_start_sect(bdev), page,
			      REQ_OP_READ);
	blk_queue_exit(bdev->bd_disk->queue);
	return result;
}

/**
 * bdev_write_page() - Start writing a page to a block device
 * @bdev: The device to write the page to
 * @sector: The offset on the device to write the page to (need not be aligned)
 * @page: The page to write
 * @wbc: The writeback_control for the write
 *
 * On entry, the page should be locked and not currently under writeback.
 * On exit, if the write started successfully, the page will be unlocked and
 * under writeback.  If the write failed already (eg the driver failed to
 * queue the page to the device), the page will still be locked.  If the
 * caller is a ->writepage implementation, it will need to unlock the page.
 *
 * Errors returned by this function are usually "soft", eg out of memory, or
 * queue full; callers should try a different route to write this page rather
 * than propagate an error back up the stack.
 *
 * Return: negative errno if an error occurs, 0 if submission was successful.
 */
int bdev_write_page(struct block_device *bdev, sector_t sector,
			struct page *page, struct writeback_control *wbc)
{
	int result;
	const struct block_device_operations *ops = bdev->bd_disk->fops;

	if (!ops->rw_page || bdev_get_integrity(bdev))
		return -EOPNOTSUPP;
	result = blk_queue_enter(bdev->bd_disk->queue, 0);
	if (result)
		return result;

	set_page_writeback(page);
	result = ops->rw_page(bdev, sector + get_start_sect(bdev), page,
			      REQ_OP_WRITE);
	if (result) {
		end_page_writeback(page);
	} else {
		clean_page_buffers(page);
		unlock_page(page);
	}
	blk_queue_exit(bdev->bd_disk->queue);
	return result;
}

/*
 * pseudo-fs
 */

static  __cacheline_aligned_in_smp DEFINE_SPINLOCK(bdev_lock);
static struct kmem_cache * bdev_cachep __read_mostly;

static struct inode *bdev_alloc_inode(struct super_block *sb)
{
	struct bdev_inode *ei = kmem_cache_alloc(bdev_cachep, GFP_KERNEL);

	if (!ei)
		return NULL;
	memset(&ei->bdev, 0, sizeof(ei->bdev));
	ei->bdev.bd_bdi = &noop_backing_dev_info;
	return &ei->vfs_inode;
}

static void bdev_free_inode(struct inode *inode)
{
	struct block_device *bdev = I_BDEV(inode);

	free_percpu(bdev->bd_stats);
	kfree(bdev->bd_meta_info);

	kmem_cache_free(bdev_cachep, BDEV_I(inode));
}

static void init_once(void *data)
{
	struct bdev_inode *ei = data;

	inode_init_once(&ei->vfs_inode);
}

static void bdev_evict_inode(struct inode *inode)
{
	struct block_device *bdev = &BDEV_I(inode)->bdev;
	truncate_inode_pages_final(&inode->i_data);
	invalidate_inode_buffers(inode); /* is it needed here? */
	clear_inode(inode);
	/* Detach inode from wb early as bdi_put() may free bdi->wb */
	inode_detach_wb(inode);
	if (bdev->bd_bdi != &noop_backing_dev_info) {
		bdi_put(bdev->bd_bdi);
		bdev->bd_bdi = &noop_backing_dev_info;
	}
}

static const struct super_operations bdev_sops = {
	.statfs = simple_statfs,
	.alloc_inode = bdev_alloc_inode,
	.free_inode = bdev_free_inode,
	.drop_inode = generic_delete_inode,
	.evict_inode = bdev_evict_inode,
};

static int bd_init_fs_context(struct fs_context *fc)
{
	struct pseudo_fs_context *ctx = init_pseudo(fc, BDEVFS_MAGIC);
	if (!ctx)
		return -ENOMEM;
	fc->s_iflags |= SB_I_CGROUPWB;
	ctx->ops = &bdev_sops;
	return 0;
}

static struct file_system_type bd_type = {
	.name		= "bdev",
	.init_fs_context = bd_init_fs_context,
	.kill_sb	= kill_anon_super,
};

struct super_block *blockdev_superblock __read_mostly;
EXPORT_SYMBOL_GPL(blockdev_superblock);

void __init bdev_cache_init(void)
{
	int err;
	static struct vfsmount *bd_mnt;

	bdev_cachep = kmem_cache_create("bdev_cache", sizeof(struct bdev_inode),
			0, (SLAB_HWCACHE_ALIGN|SLAB_RECLAIM_ACCOUNT|
				SLAB_MEM_SPREAD|SLAB_ACCOUNT|SLAB_PANIC),
			init_once);
	err = register_filesystem(&bd_type);
	if (err)
		panic("Cannot register bdev pseudo-fs");
	bd_mnt = kern_mount(&bd_type);
	if (IS_ERR(bd_mnt))
		panic("Cannot create bdev pseudo-fs");
	blockdev_superblock = bd_mnt->mnt_sb;   /* For writeback */
}

struct block_device *bdev_alloc(struct gendisk *disk, u8 partno)
{
	struct block_device *bdev;
	struct inode *inode;

	inode = new_inode(blockdev_superblock);
	if (!inode)
		return NULL;
	inode->i_mode = S_IFBLK;
	inode->i_rdev = 0;
	inode->i_data.a_ops = &def_blk_aops;
	mapping_set_gfp_mask(&inode->i_data, GFP_USER);

	bdev = I_BDEV(inode);
	mutex_init(&bdev->bd_mutex);
	mutex_init(&bdev->bd_fsfreeze_mutex);
	spin_lock_init(&bdev->bd_size_lock);
	bdev->bd_disk = disk;
	bdev->bd_partno = partno;
	bdev->bd_inode = inode;
#ifdef CONFIG_SYSFS
	INIT_LIST_HEAD(&bdev->bd_holder_disks);
#endif
	bdev->bd_stats = alloc_percpu(struct disk_stats);
	if (!bdev->bd_stats) {
		iput(inode);
		return NULL;
	}
	return bdev;
}

void bdev_add(struct block_device *bdev, dev_t dev)
{
	bdev->bd_dev = dev;
	bdev->bd_inode->i_rdev = dev;
	bdev->bd_inode->i_ino = dev;
	insert_inode_hash(bdev->bd_inode);
}

static struct block_device *bdget(dev_t dev)
{
	struct inode *inode;

	inode = ilookup(blockdev_superblock, dev);
	if (!inode)
		return NULL;
	return &BDEV_I(inode)->bdev;
}

/**
 * bdgrab -- Grab a reference to an already referenced block device
 * @bdev:	Block device to grab a reference to.
 *
 * Returns the block_device with an additional reference when successful,
 * or NULL if the inode is already beeing freed.
 */
struct block_device *bdgrab(struct block_device *bdev)
{
	if (!igrab(bdev->bd_inode))
		return NULL;
	return bdev;
}
EXPORT_SYMBOL(bdgrab);

long nr_blockdev_pages(void)
{
	struct inode *inode;
	long ret = 0;

	spin_lock(&blockdev_superblock->s_inode_list_lock);
	list_for_each_entry(inode, &blockdev_superblock->s_inodes, i_sb_list)
		ret += inode->i_mapping->nrpages;
	spin_unlock(&blockdev_superblock->s_inode_list_lock);

	return ret;
}

void bdput(struct block_device *bdev)
{
	iput(bdev->bd_inode);
}
EXPORT_SYMBOL(bdput);
 
/**
 * bd_may_claim - test whether a block device can be claimed
 * @bdev: block device of interest
 * @whole: whole block device containing @bdev, may equal @bdev
 * @holder: holder trying to claim @bdev
 *
 * Test whether @bdev can be claimed by @holder.
 *
 * CONTEXT:
 * spin_lock(&bdev_lock).
 *
 * RETURNS:
 * %true if @bdev can be claimed, %false otherwise.
 */
static bool bd_may_claim(struct block_device *bdev, struct block_device *whole,
			 void *holder)
{
	if (bdev->bd_holder == holder)
		return true;	 /* already a holder */
	else if (bdev->bd_holder != NULL)
		return false; 	 /* held by someone else */
	else if (whole == bdev)
		return true;  	 /* is a whole device which isn't held */

	else if (whole->bd_holder == bd_may_claim)
		return true; 	 /* is a partition of a device that is being partitioned */
	else if (whole->bd_holder != NULL)
		return false;	 /* is a partition of a held device */
	else
		return true;	 /* is a partition of an un-held device */
}

/**
 * bd_prepare_to_claim - claim a block device
 * @bdev: block device of interest
 * @holder: holder trying to claim @bdev
 *
 * Claim @bdev.  This function fails if @bdev is already claimed by another
 * holder and waits if another claiming is in progress. return, the caller
 * has ownership of bd_claiming and bd_holder[s].
 *
 * RETURNS:
 * 0 if @bdev can be claimed, -EBUSY otherwise.
 */
int bd_prepare_to_claim(struct block_device *bdev, void *holder)
{
	struct block_device *whole = bdev_whole(bdev);

	if (WARN_ON_ONCE(!holder))
		return -EINVAL;
retry:
	spin_lock(&bdev_lock);
	/* if someone else claimed, fail */
	if (!bd_may_claim(bdev, whole, holder)) {
		spin_unlock(&bdev_lock);
		return -EBUSY;
	}

	/* if claiming is already in progress, wait for it to finish */
	if (whole->bd_claiming) {
		wait_queue_head_t *wq = bit_waitqueue(&whole->bd_claiming, 0);
		DEFINE_WAIT(wait);

		prepare_to_wait(wq, &wait, TASK_UNINTERRUPTIBLE);
		spin_unlock(&bdev_lock);
		schedule();
		finish_wait(wq, &wait);
		goto retry;
	}

	/* yay, all mine */
	whole->bd_claiming = holder;
	spin_unlock(&bdev_lock);
	return 0;
}
EXPORT_SYMBOL_GPL(bd_prepare_to_claim); /* only for the loop driver */

static void bd_clear_claiming(struct block_device *whole, void *holder)
{
	lockdep_assert_held(&bdev_lock);
	/* tell others that we're done */
	BUG_ON(whole->bd_claiming != holder);
	whole->bd_claiming = NULL;
	wake_up_bit(&whole->bd_claiming, 0);
}

/**
 * bd_finish_claiming - finish claiming of a block device
 * @bdev: block device of interest
 * @holder: holder that has claimed @bdev
 *
 * Finish exclusive open of a block device. Mark the device as exlusively
 * open by the holder and wake up all waiters for exclusive open to finish.
 */
static void bd_finish_claiming(struct block_device *bdev, void *holder)
{
	struct block_device *whole = bdev_whole(bdev);

	spin_lock(&bdev_lock);
	BUG_ON(!bd_may_claim(bdev, whole, holder));
	/*
	 * Note that for a whole device bd_holders will be incremented twice,
	 * and bd_holder will be set to bd_may_claim before being set to holder
	 */
	whole->bd_holders++;
	whole->bd_holder = bd_may_claim;
	bdev->bd_holders++;
	bdev->bd_holder = holder;
	bd_clear_claiming(whole, holder);
	spin_unlock(&bdev_lock);
}

/**
 * bd_abort_claiming - abort claiming of a block device
 * @bdev: block device of interest
 * @holder: holder that has claimed @bdev
 *
 * Abort claiming of a block device when the exclusive open failed. This can be
 * also used when exclusive open is not actually desired and we just needed
 * to block other exclusive openers for a while.
 */
void bd_abort_claiming(struct block_device *bdev, void *holder)
{
	spin_lock(&bdev_lock);
	bd_clear_claiming(bdev_whole(bdev), holder);
	spin_unlock(&bdev_lock);
}
EXPORT_SYMBOL(bd_abort_claiming);

#ifdef CONFIG_SYSFS
struct bd_holder_disk {
	struct list_head	list;
	struct gendisk		*disk;
	int			refcnt;
};

static struct bd_holder_disk *bd_find_holder_disk(struct block_device *bdev,
						  struct gendisk *disk)
{
	struct bd_holder_disk *holder;

	list_for_each_entry(holder, &bdev->bd_holder_disks, list)
		if (holder->disk == disk)
			return holder;
	return NULL;
}

static int add_symlink(struct kobject *from, struct kobject *to)
{
	return sysfs_create_link(from, to, kobject_name(to));
}

static void del_symlink(struct kobject *from, struct kobject *to)
{
	sysfs_remove_link(from, kobject_name(to));
}

/**
 * bd_link_disk_holder - create symlinks between holding disk and slave bdev
 * @bdev: the claimed slave bdev
 * @disk: the holding disk
 *
 * DON'T USE THIS UNLESS YOU'RE ALREADY USING IT.
 *
 * This functions creates the following sysfs symlinks.
 *
 * - from "slaves" directory of the holder @disk to the claimed @bdev
 * - from "holders" directory of the @bdev to the holder @disk
 *
 * For example, if /dev/dm-0 maps to /dev/sda and disk for dm-0 is
 * passed to bd_link_disk_holder(), then:
 *
 *   /sys/block/dm-0/slaves/sda --> /sys/block/sda
 *   /sys/block/sda/holders/dm-0 --> /sys/block/dm-0
 *
 * The caller must have claimed @bdev before calling this function and
 * ensure that both @bdev and @disk are valid during the creation and
 * lifetime of these symlinks.
 *
 * CONTEXT:
 * Might sleep.
 *
 * RETURNS:
 * 0 on success, -errno on failure.
 */
int bd_link_disk_holder(struct block_device *bdev, struct gendisk *disk)
{
	struct bd_holder_disk *holder;
	int ret = 0;

	mutex_lock(&bdev->bd_mutex);

	WARN_ON_ONCE(!bdev->bd_holder);

	/* FIXME: remove the following once add_disk() handles errors */
	if (WARN_ON(!disk->slave_dir || !bdev->bd_holder_dir))
		goto out_unlock;

	holder = bd_find_holder_disk(bdev, disk);
	if (holder) {
		holder->refcnt++;
		goto out_unlock;
	}

	holder = kzalloc(sizeof(*holder), GFP_KERNEL);
	if (!holder) {
		ret = -ENOMEM;
		goto out_unlock;
	}

	INIT_LIST_HEAD(&holder->list);
	holder->disk = disk;
	holder->refcnt = 1;

	ret = add_symlink(disk->slave_dir, bdev_kobj(bdev));
	if (ret)
		goto out_free;

	ret = add_symlink(bdev->bd_holder_dir, &disk_to_dev(disk)->kobj);
	if (ret)
		goto out_del;
	/*
	 * bdev could be deleted beneath us which would implicitly destroy
	 * the holder directory.  Hold on to it.
	 */
	kobject_get(bdev->bd_holder_dir);

	list_add(&holder->list, &bdev->bd_holder_disks);
	goto out_unlock;

out_del:
	del_symlink(disk->slave_dir, bdev_kobj(bdev));
out_free:
	kfree(holder);
out_unlock:
	mutex_unlock(&bdev->bd_mutex);
	return ret;
}
EXPORT_SYMBOL_GPL(bd_link_disk_holder);

/**
 * bd_unlink_disk_holder - destroy symlinks created by bd_link_disk_holder()
 * @bdev: the calimed slave bdev
 * @disk: the holding disk
 *
 * DON'T USE THIS UNLESS YOU'RE ALREADY USING IT.
 *
 * CONTEXT:
 * Might sleep.
 */
void bd_unlink_disk_holder(struct block_device *bdev, struct gendisk *disk)
{
	struct bd_holder_disk *holder;

	mutex_lock(&bdev->bd_mutex);

	holder = bd_find_holder_disk(bdev, disk);

	if (!WARN_ON_ONCE(holder == NULL) && !--holder->refcnt) {
		del_symlink(disk->slave_dir, bdev_kobj(bdev));
		del_symlink(bdev->bd_holder_dir, &disk_to_dev(disk)->kobj);
		kobject_put(bdev->bd_holder_dir);
		list_del_init(&holder->list);
		kfree(holder);
	}

	mutex_unlock(&bdev->bd_mutex);
}
EXPORT_SYMBOL_GPL(bd_unlink_disk_holder);
#endif

static void __blkdev_put(struct block_device *bdev, fmode_t mode, int for_part);

int bdev_disk_changed(struct block_device *bdev, bool invalidate)
{
	struct gendisk *disk = bdev->bd_disk;
	int ret = 0;

	lockdep_assert_held(&bdev->bd_mutex);

	if (!(disk->flags & GENHD_FL_UP))
		return -ENXIO;

rescan:
	if (bdev->bd_part_count)
		return -EBUSY;
	sync_blockdev(bdev);
	invalidate_bdev(bdev);
	blk_drop_partitions(disk);

	clear_bit(GD_NEED_PART_SCAN, &disk->state);

	/*
	 * Historically we only set the capacity to zero for devices that
	 * support partitions (independ of actually having partitions created).
	 * Doing that is rather inconsistent, but changing it broke legacy
	 * udisks polling for legacy ide-cdrom devices.  Use the crude check
	 * below to get the sane behavior for most device while not breaking
	 * userspace for this particular setup.
	 */
	if (invalidate) {
		if (disk_part_scan_enabled(disk) ||
		    !(disk->flags & GENHD_FL_REMOVABLE))
			set_capacity(disk, 0);
	}

	if (get_capacity(disk)) {
		ret = blk_add_partitions(disk, bdev);
		if (ret == -EAGAIN)
			goto rescan;
	} else if (invalidate) {
		/*
		 * Tell userspace that the media / partition table may have
		 * changed.
		 */
		kobject_uevent(&disk_to_dev(disk)->kobj, KOBJ_CHANGE);
	}

	return ret;
}
/*
 * Only exported for loop and dasd for historic reasons.  Don't use in new
 * code!
 */
EXPORT_SYMBOL_GPL(bdev_disk_changed);

/*
 * bd_mutex locking:
 *
 *  mutex_lock(part->bd_mutex)
 *    mutex_lock_nested(whole->bd_mutex, 1)
 */
static int __blkdev_get(struct block_device *bdev, fmode_t mode)
{
	struct gendisk *disk = bdev->bd_disk;
	int ret = 0;

	if (!(disk->flags & GENHD_FL_UP))
		return -ENXIO;

	if (!bdev->bd_openers) {
		if (!bdev_is_partition(bdev)) {
			ret = 0;
			if (disk->fops->open)
				ret = disk->fops->open(bdev, mode);

			if (!ret)
				set_init_blocksize(bdev);

			/*
			 * If the device is invalidated, rescan partition
			 * if open succeeded or failed with -ENOMEDIUM.
			 * The latter is necessary to prevent ghost
			 * partitions on a removed medium.
			 */
			if (test_bit(GD_NEED_PART_SCAN, &disk->state) &&
			    (!ret || ret == -ENOMEDIUM))
				bdev_disk_changed(bdev, ret == -ENOMEDIUM);

			if (ret)
				return ret;
		} else {
			struct block_device *whole = bdgrab(disk->part0);

			mutex_lock_nested(&whole->bd_mutex, 1);
			ret = __blkdev_get(whole, mode);
			if (ret) {
				mutex_unlock(&whole->bd_mutex);
				bdput(whole);
				return ret;
			}
			whole->bd_part_count++;
			mutex_unlock(&whole->bd_mutex);

			if (!bdev_nr_sectors(bdev)) {
				__blkdev_put(whole, mode, 1);
				bdput(whole);
				return -ENXIO;
			}
			set_init_blocksize(bdev);
		}

		if (bdev->bd_bdi == &noop_backing_dev_info)
			bdev->bd_bdi = bdi_get(disk->queue->backing_dev_info);
	} else {
		if (!bdev_is_partition(bdev)) {
			if (bdev->bd_disk->fops->open)
				ret = bdev->bd_disk->fops->open(bdev, mode);
			/* the same as first opener case, read comment there */
			if (test_bit(GD_NEED_PART_SCAN, &disk->state) &&
			    (!ret || ret == -ENOMEDIUM))
				bdev_disk_changed(bdev, ret == -ENOMEDIUM);
			if (ret)
				return ret;
		}
	}
	bdev->bd_openers++;
	return 0;
}

struct block_device *blkdev_get_no_open(dev_t dev)
{
	struct block_device *bdev;
	struct gendisk *disk;

	bdev = bdget(dev);
	if (!bdev) {
		blk_request_module(dev);
		bdev = bdget(dev);
		if (!bdev)
			return NULL;
	}

	disk = bdev->bd_disk;
	if (!kobject_get_unless_zero(&disk_to_dev(disk)->kobj))
		goto bdput;
	if ((disk->flags & (GENHD_FL_UP | GENHD_FL_HIDDEN)) != GENHD_FL_UP)
		goto put_disk;
	if (!try_module_get(bdev->bd_disk->fops->owner))
		goto put_disk;
	return bdev;
put_disk:
	put_disk(disk);
bdput:
	bdput(bdev);
	return NULL;
}

void blkdev_put_no_open(struct block_device *bdev)
{
	module_put(bdev->bd_disk->fops->owner);
	put_disk(bdev->bd_disk);
	bdput(bdev);
}

/**
 * blkdev_get_by_dev - open a block device by device number
 * @dev: device number of block device to open
 * @mode: FMODE_* mask
 * @holder: exclusive holder identifier
 *
 * Open the block device described by device number @dev. If @mode includes
 * %FMODE_EXCL, the block device is opened with exclusive access.  Specifying
 * %FMODE_EXCL with a %NULL @holder is invalid.  Exclusive opens may nest for
 * the same @holder.
 *
 * Use this interface ONLY if you really do not have anything better - i.e. when
 * you are behind a truly sucky interface and all you are given is a device
 * number.  Everything else should use blkdev_get_by_path().
 *
 * CONTEXT:
 * Might sleep.
 *
 * RETURNS:
 * Reference to the block_device on success, ERR_PTR(-errno) on failure.
 */
struct block_device *blkdev_get_by_dev(dev_t dev, fmode_t mode, void *holder)
{
	bool unblock_events = true;
	struct block_device *bdev;
	struct gendisk *disk;
	int ret;

	ret = devcgroup_check_permission(DEVCG_DEV_BLOCK,
			MAJOR(dev), MINOR(dev),
			((mode & FMODE_READ) ? DEVCG_ACC_READ : 0) |
			((mode & FMODE_WRITE) ? DEVCG_ACC_WRITE : 0));
	if (ret)
		return ERR_PTR(ret);

	bdev = blkdev_get_no_open(dev);
	if (!bdev)
		return ERR_PTR(-ENXIO);
	disk = bdev->bd_disk;

	if (mode & FMODE_EXCL) {
		ret = bd_prepare_to_claim(bdev, holder);
		if (ret)
			goto put_blkdev;
	}

	disk_block_events(disk);

	mutex_lock(&bdev->bd_mutex);
	ret =__blkdev_get(bdev, mode);
	if (ret)
		goto abort_claiming;
	if (mode & FMODE_EXCL) {
		bd_finish_claiming(bdev, holder);

		/*
		 * Block event polling for write claims if requested.  Any write
		 * holder makes the write_holder state stick until all are
		 * released.  This is good enough and tracking individual
		 * writeable reference is too fragile given the way @mode is
		 * used in blkdev_get/put().
		 */
		if ((mode & FMODE_WRITE) && !bdev->bd_write_holder &&
		    (disk->flags & GENHD_FL_BLOCK_EVENTS_ON_EXCL_WRITE)) {
			bdev->bd_write_holder = true;
			unblock_events = false;
		}
	}
	mutex_unlock(&bdev->bd_mutex);

	if (unblock_events)
		disk_unblock_events(disk);
	return bdev;

abort_claiming:
	if (mode & FMODE_EXCL)
		bd_abort_claiming(bdev, holder);
	mutex_unlock(&bdev->bd_mutex);
	disk_unblock_events(disk);
put_blkdev:
	blkdev_put_no_open(bdev);
	return ERR_PTR(ret);
}
EXPORT_SYMBOL(blkdev_get_by_dev);

/**
 * blkdev_get_by_path - open a block device by name
 * @path: path to the block device to open
 * @mode: FMODE_* mask
 * @holder: exclusive holder identifier
 *
 * Open the block device described by the device file at @path.  If @mode
 * includes %FMODE_EXCL, the block device is opened with exclusive access.
 * Specifying %FMODE_EXCL with a %NULL @holder is invalid.  Exclusive opens may
 * nest for the same @holder.
 *
 * CONTEXT:
 * Might sleep.
 *
 * RETURNS:
 * Reference to the block_device on success, ERR_PTR(-errno) on failure.
 */
struct block_device *blkdev_get_by_path(const char *path, fmode_t mode,
					void *holder)
{
	struct block_device *bdev;
	dev_t dev;
	int error;

	error = lookup_bdev(path, &dev);
	if (error)
		return ERR_PTR(error);

	bdev = blkdev_get_by_dev(dev, mode, holder);
	if (!IS_ERR(bdev) && (mode & FMODE_WRITE) && bdev_read_only(bdev)) {
		blkdev_put(bdev, mode);
		return ERR_PTR(-EACCES);
	}

	return bdev;
}
EXPORT_SYMBOL(blkdev_get_by_path);

static int blkdev_open(struct inode * inode, struct file * filp)
{
	struct block_device *bdev;

	/*
	 * Preserve backwards compatibility and allow large file access
	 * even if userspace doesn't ask for it explicitly. Some mkfs
	 * binary needs it. We might want to drop this workaround
	 * during an unstable branch.
	 */
	filp->f_flags |= O_LARGEFILE;

	filp->f_mode |= FMODE_NOWAIT | FMODE_BUF_RASYNC;

	if (filp->f_flags & O_NDELAY)
		filp->f_mode |= FMODE_NDELAY;
	if (filp->f_flags & O_EXCL)
		filp->f_mode |= FMODE_EXCL;
	if ((filp->f_flags & O_ACCMODE) == 3)
		filp->f_mode |= FMODE_WRITE_IOCTL;

	bdev = blkdev_get_by_dev(inode->i_rdev, filp->f_mode, filp);
	if (IS_ERR(bdev))
		return PTR_ERR(bdev);
	filp->f_mapping = bdev->bd_inode->i_mapping;
	filp->f_wb_err = filemap_sample_wb_err(filp->f_mapping);
	return 0;
}

static void __blkdev_put(struct block_device *bdev, fmode_t mode, int for_part)
{
	struct gendisk *disk = bdev->bd_disk;
	struct block_device *victim = NULL;

	/*
	 * Sync early if it looks like we're the last one.  If someone else
	 * opens the block device between now and the decrement of bd_openers
	 * then we did a sync that we didn't need to, but that's not the end
	 * of the world and we want to avoid long (could be several minute)
	 * syncs while holding the mutex.
	 */
	if (bdev->bd_openers == 1)
		sync_blockdev(bdev);

	mutex_lock_nested(&bdev->bd_mutex, for_part);
	if (for_part)
		bdev->bd_part_count--;

	if (!--bdev->bd_openers) {
		WARN_ON_ONCE(bdev->bd_holders);
		sync_blockdev(bdev);
		kill_bdev(bdev);
		bdev_write_inode(bdev);
		if (bdev_is_partition(bdev))
			victim = bdev_whole(bdev);
	}

	if (!bdev_is_partition(bdev) && disk->fops->release)
		disk->fops->release(disk, mode);
	mutex_unlock(&bdev->bd_mutex);
	if (victim) {
		__blkdev_put(victim, mode, 1);
		bdput(victim);
	}
}

void blkdev_put(struct block_device *bdev, fmode_t mode)
{
	struct gendisk *disk = bdev->bd_disk;

	mutex_lock(&bdev->bd_mutex);

	if (mode & FMODE_EXCL) {
		struct block_device *whole = bdev_whole(bdev);
		bool bdev_free;

		/*
		 * Release a claim on the device.  The holder fields
		 * are protected with bdev_lock.  bd_mutex is to
		 * synchronize disk_holder unlinking.
		 */
		spin_lock(&bdev_lock);

		WARN_ON_ONCE(--bdev->bd_holders < 0);
		WARN_ON_ONCE(--whole->bd_holders < 0);

		if ((bdev_free = !bdev->bd_holders))
			bdev->bd_holder = NULL;
		if (!whole->bd_holders)
			whole->bd_holder = NULL;

		spin_unlock(&bdev_lock);

		/*
		 * If this was the last claim, remove holder link and
		 * unblock evpoll if it was a write holder.
		 */
		if (bdev_free && bdev->bd_write_holder) {
			disk_unblock_events(disk);
			bdev->bd_write_holder = false;
		}
	}

	/*
	 * Trigger event checking and tell drivers to flush MEDIA_CHANGE
	 * event.  This is to ensure detection of media removal commanded
	 * from userland - e.g. eject(1).
	 */
	disk_flush_events(disk, DISK_EVENT_MEDIA_CHANGE);
	mutex_unlock(&bdev->bd_mutex);

	__blkdev_put(bdev, mode, 0);
	blkdev_put_no_open(bdev);
}
EXPORT_SYMBOL(blkdev_put);

static int blkdev_close(struct inode * inode, struct file * filp)
{
	struct block_device *bdev = I_BDEV(bdev_file_inode(filp));
	blkdev_put(bdev, filp->f_mode);
	return 0;
}

static long block_ioctl(struct file *file, unsigned cmd, unsigned long arg)
{
	struct block_device *bdev = I_BDEV(bdev_file_inode(file));
	fmode_t mode = file->f_mode;

	/*
	 * O_NDELAY can be altered using fcntl(.., F_SETFL, ..), so we have
	 * to updated it before every ioctl.
	 */
	if (file->f_flags & O_NDELAY)
		mode |= FMODE_NDELAY;
	else
		mode &= ~FMODE_NDELAY;

	return blkdev_ioctl(bdev, mode, cmd, arg);
}

/*
 * Write data to the block device.  Only intended for the block device itself
 * and the raw driver which basically is a fake block device.
 *
 * Does not take i_mutex for the write and thus is not for general purpose
 * use.
 */
ssize_t blkdev_write_iter(struct kiocb *iocb, struct iov_iter *from)
{
	struct file *file = iocb->ki_filp;
	struct inode *bd_inode = bdev_file_inode(file);
	loff_t size = i_size_read(bd_inode);
	struct blk_plug plug;
	size_t shorted = 0;
	ssize_t ret;

	if (bdev_read_only(I_BDEV(bd_inode)))
		return -EPERM;

	if (IS_SWAPFILE(bd_inode) && !is_hibernate_resume_dev(bd_inode->i_rdev))
		return -ETXTBSY;

	if (!iov_iter_count(from))
		return 0;

	if (iocb->ki_pos >= size)
		return -ENOSPC;

	if ((iocb->ki_flags & (IOCB_NOWAIT | IOCB_DIRECT)) == IOCB_NOWAIT)
		return -EOPNOTSUPP;

	size -= iocb->ki_pos;
	if (iov_iter_count(from) > size) {
		shorted = iov_iter_count(from) - size;
		iov_iter_truncate(from, size);
	}

	blk_start_plug(&plug);
	ret = __generic_file_write_iter(iocb, from);
	if (ret > 0)
		ret = generic_write_sync(iocb, ret);
	iov_iter_reexpand(from, iov_iter_count(from) + shorted);
	blk_finish_plug(&plug);
	return ret;
}
EXPORT_SYMBOL_GPL(blkdev_write_iter);

ssize_t blkdev_read_iter(struct kiocb *iocb, struct iov_iter *to)
{
	struct file *file = iocb->ki_filp;
	struct inode *bd_inode = bdev_file_inode(file);
	loff_t size = i_size_read(bd_inode);
	loff_t pos = iocb->ki_pos;
	size_t shorted = 0;
	ssize_t ret;

	if (pos >= size)
		return 0;

	size -= pos;
	if (iov_iter_count(to) > size) {
		shorted = iov_iter_count(to) - size;
		iov_iter_truncate(to, size);
	}

	ret = generic_file_read_iter(iocb, to);
	iov_iter_reexpand(to, iov_iter_count(to) + shorted);
	return ret;
}
EXPORT_SYMBOL_GPL(blkdev_read_iter);

/*
 * Try to release a page associated with block device when the system
 * is under memory pressure.
 */
static int blkdev_releasepage(struct page *page, gfp_t wait)
{
	struct super_block *super = BDEV_I(page->mapping->host)->bdev.bd_super;

	if (super && super->s_op->bdev_try_to_free_page)
		return super->s_op->bdev_try_to_free_page(super, page, wait);

	return try_to_free_buffers(page);
}

static int blkdev_writepages(struct address_space *mapping,
			     struct writeback_control *wbc)
{
	return generic_writepages(mapping, wbc);
}

static const struct address_space_operations def_blk_aops = {
	.readpage	= blkdev_readpage,
	.readahead	= blkdev_readahead,
	.writepage	= blkdev_writepage,
	.write_begin	= blkdev_write_begin,
	.write_end	= blkdev_write_end,
	.writepages	= blkdev_writepages,
	.releasepage	= blkdev_releasepage,
	.direct_IO	= blkdev_direct_IO,
	.migratepage	= buffer_migrate_page_norefs,
	.is_dirty_writeback = buffer_check_dirty_writeback,
};

#define	BLKDEV_FALLOC_FL_SUPPORTED					\
		(FALLOC_FL_KEEP_SIZE | FALLOC_FL_PUNCH_HOLE |		\
		 FALLOC_FL_ZERO_RANGE | FALLOC_FL_NO_HIDE_STALE)

static long blkdev_fallocate(struct file *file, int mode, loff_t start,
			     loff_t len)
{
	struct block_device *bdev = I_BDEV(bdev_file_inode(file));
	loff_t end = start + len - 1;
	loff_t isize;
	int error;

	/* Fail if we don't recognize the flags. */
	if (mode & ~BLKDEV_FALLOC_FL_SUPPORTED)
		return -EOPNOTSUPP;

	/* Don't go off the end of the device. */
	isize = i_size_read(bdev->bd_inode);
	if (start >= isize)
		return -EINVAL;
	if (end >= isize) {
		if (mode & FALLOC_FL_KEEP_SIZE) {
			len = isize - start;
			end = start + len - 1;
		} else
			return -EINVAL;
	}

	/*
	 * Don't allow IO that isn't aligned to logical block size.
	 */
	if ((start | len) & (bdev_logical_block_size(bdev) - 1))
		return -EINVAL;

	/* Invalidate the page cache, including dirty pages. */
	error = truncate_bdev_range(bdev, file->f_mode, start, end);
	if (error)
		return error;

	switch (mode) {
	case FALLOC_FL_ZERO_RANGE:
	case FALLOC_FL_ZERO_RANGE | FALLOC_FL_KEEP_SIZE:
		error = blkdev_issue_zeroout(bdev, start >> 9, len >> 9,
					    GFP_KERNEL, BLKDEV_ZERO_NOUNMAP);
		break;
	case FALLOC_FL_PUNCH_HOLE | FALLOC_FL_KEEP_SIZE:
		error = blkdev_issue_zeroout(bdev, start >> 9, len >> 9,
					     GFP_KERNEL, BLKDEV_ZERO_NOFALLBACK);
		break;
	case FALLOC_FL_PUNCH_HOLE | FALLOC_FL_KEEP_SIZE | FALLOC_FL_NO_HIDE_STALE:
		error = blkdev_issue_discard(bdev, start >> 9, len >> 9,
					     GFP_KERNEL, 0);
		break;
	default:
		return -EOPNOTSUPP;
	}
	if (error)
		return error;

	/*
	 * Invalidate the page cache again; if someone wandered in and dirtied
	 * a page, we just discard it - userspace has no way of knowing whether
	 * the write happened before or after discard completing...
	 */
	return truncate_bdev_range(bdev, file->f_mode, start, end);
}

const struct file_operations def_blk_fops = {
	.open		= blkdev_open,
	.release	= blkdev_close,
	.llseek		= block_llseek,
	.read_iter	= blkdev_read_iter,
	.write_iter	= blkdev_write_iter,
	.iopoll		= blkdev_iopoll,
	.mmap		= generic_file_mmap,
	.fsync		= blkdev_fsync,
	.unlocked_ioctl	= block_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl	= compat_blkdev_ioctl,
#endif
	.splice_read	= generic_file_splice_read,
	.splice_write	= iter_file_splice_write,
	.fallocate	= blkdev_fallocate,
};

/**
 * lookup_bdev  - lookup a struct block_device by name
 * @pathname:	special file representing the block device
 * @dev:	return value of the block device's dev_t
 *
 * Get a reference to the blockdevice at @pathname in the current
 * namespace if possible and return it.  Return ERR_PTR(error)
 * otherwise.
 */
int lookup_bdev(const char *pathname, dev_t *dev)
{
	struct inode *inode;
	struct path path;
	int error;

	if (!pathname || !*pathname)
		return -EINVAL;

	error = kern_path(pathname, LOOKUP_FOLLOW, &path);
	if (error)
		return error;

	inode = d_backing_inode(path.dentry);
	error = -ENOTBLK;
	if (!S_ISBLK(inode->i_mode))
		goto out_path_put;
	error = -EACCES;
	if (!may_open_dev(&path))
		goto out_path_put;

	*dev = inode->i_rdev;
	error = 0;
out_path_put:
	path_put(&path);
	return error;
}
EXPORT_SYMBOL(lookup_bdev);

int __invalidate_device(struct block_device *bdev, bool kill_dirty)
{
	struct super_block *sb = get_super(bdev);
	int res = 0;

	if (sb) {
		/*
		 * no need to lock the super, get_super holds the
		 * read mutex so the filesystem cannot go away
		 * under us (->put_super runs with the write lock
		 * hold).
		 */
		shrink_dcache_sb(sb);
		res = invalidate_inodes(sb, kill_dirty);
		drop_super(sb);
	}
	invalidate_bdev(bdev);
	return res;
}
EXPORT_SYMBOL(__invalidate_device);

void iterate_bdevs(void (*func)(struct block_device *, void *), void *arg)
{
	struct inode *inode, *old_inode = NULL;

	spin_lock(&blockdev_superblock->s_inode_list_lock);
	list_for_each_entry(inode, &blockdev_superblock->s_inodes, i_sb_list) {
		struct address_space *mapping = inode->i_mapping;
		struct block_device *bdev;

		spin_lock(&inode->i_lock);
		if (inode->i_state & (I_FREEING|I_WILL_FREE|I_NEW) ||
		    mapping->nrpages == 0) {
			spin_unlock(&inode->i_lock);
			continue;
		}
		__iget(inode);
		spin_unlock(&inode->i_lock);
		spin_unlock(&blockdev_superblock->s_inode_list_lock);
		/*
		 * We hold a reference to 'inode' so it couldn't have been
		 * removed from s_inodes list while we dropped the
		 * s_inode_list_lock  We cannot iput the inode now as we can
		 * be holding the last reference and we cannot iput it under
		 * s_inode_list_lock. So we keep the reference and iput it
		 * later.
		 */
		iput(old_inode);
		old_inode = inode;
		bdev = I_BDEV(inode);

		mutex_lock(&bdev->bd_mutex);
		if (bdev->bd_openers)
			func(bdev, arg);
		mutex_unlock(&bdev->bd_mutex);

		spin_lock(&blockdev_superblock->s_inode_list_lock);
	}
	spin_unlock(&blockdev_superblock->s_inode_list_lock);
	iput(old_inode);
}
