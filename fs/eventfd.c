// SPDX-License-Identifier: GPL-2.0-only
/*
 *  fs/eventfd.c
 *
 *  Copyright (C) 2007  Davide Libenzi <davidel@xmailserver.org>
 *
 */

#include <linux/file.h>
#include <linux/poll.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/sched/signal.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/anon_inodes.h>
#include <linux/syscalls.h>
#include <linux/export.h>
#include <linux/kref.h>
#include <linux/eventfd.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/idr.h>
#include <linux/uio.h>

DEFINE_PER_CPU(int, eventfd_wake_count);

static DEFINE_IDA(eventfd_ida);

struct eventfd_ctx {
	struct kref kref;
	wait_queue_head_t wqh;
	/*
	 * Every time that a write(2) is performed on an eventfd, the
	 * value of the __u64 being written is added to "count" and a
	 * wakeup is performed on "wqh". A read(2) will return the "count"
	 * value to userspace, and will reset "count" to zero. The kernel
	 * side eventfd_signal() also, adds to the "count" counter and
	 * issue a wakeup.
	 */
	__u64 count;
	unsigned int flags;
	int id;
};

/**
 * eventfd_signal - Adds @n to the eventfd counter.
 * @ctx: [in] Pointer to the eventfd context.
 * @n: [in] Value of the counter to be added to the eventfd internal counter.
 *          The value cannot be negative.
 *
 * This function is supposed to be called by the kernel in paths that do not
 * allow sleeping. In this function we allow the counter to reach the ULLONG_MAX
 * value, and we signal this as overflow condition by returning a EPOLLERR
 * to poll(2).
 *
 * Returns the amount by which the counter was incremented.  This will be less
 * than @n if the counter has overflowed.
 */
__u64 eventfd_signal(struct eventfd_ctx *ctx, __u64 n)
{
	unsigned long flags;

	/*
	 * Deadlock or stack overflow issues can happen if we recurse here
	 * through waitqueue wakeup handlers. If the caller users potentially
	 * nested waitqueues with custom wakeup handlers, then it should
	 * check eventfd_signal_count() before calling this function. If
	 * it returns true, the eventfd_signal() call should be deferred to a
	 * safe context.
	 */
	if (WARN_ON_ONCE(this_cpu_read(eventfd_wake_count)))
		return 0;

	spin_lock_irqsave(&ctx->wqh.lock, flags);
	this_cpu_inc(eventfd_wake_count);
	if (ULLONG_MAX - ctx->count < n)
		n = ULLONG_MAX - ctx->count;
	ctx->count += n;
	if (waitqueue_active(&ctx->wqh))
		wake_up_locked_poll(&ctx->wqh, EPOLLIN);
	this_cpu_dec(eventfd_wake_count);
	spin_unlock_irqrestore(&ctx->wqh.lock, flags);

	return n;
}
EXPORT_SYMBOL_GPL(eventfd_signal);

static void eventfd_free_ctx(struct eventfd_ctx *ctx)
{
	if (ctx->id >= 0)
		ida_simple_remove(&eventfd_ida, ctx->id);
	kfree(ctx);
}

static void eventfd_free(struct kref *kref)
{
	struct eventfd_ctx *ctx = container_of(kref, struct eventfd_ctx, kref);

	eventfd_free_ctx(ctx);
}

/**
 * eventfd_ctx_put - Releases a reference to the internal eventfd context.
 * @ctx: [in] Pointer to eventfd context.
 *
 * The eventfd context reference must have been previously acquired either
 * with eventfd_ctx_fdget() or eventfd_ctx_fileget().
 */
void eventfd_ctx_put(struct eventfd_ctx *ctx)
{
	kref_put(&ctx->kref, eventfd_free);
}
EXPORT_SYMBOL_GPL(eventfd_ctx_put);

static int eventfd_release(struct inode *inode, struct file *file)
{
	struct eventfd_ctx *ctx = file->private_data;

	wake_up_poll(&ctx->wqh, EPOLLHUP);
	eventfd_ctx_put(ctx);
	return 0;
}

static __poll_t eventfd_poll(struct file *file, poll_table *wait)
{
	struct eventfd_ctx *ctx = file->private_data;
	__poll_t events = 0;
	u64 count;

	poll_wait(file, &ctx->wqh, wait);

	/*
	 * All writes to ctx->count occur within ctx->wqh.lock.  This read
	 * can be done outside ctx->wqh.lock because we know that poll_wait
	 * takes that lock (through add_wait_queue) if our caller will sleep.
	 *
	 * The read _can_ therefore seep into add_wait_queue's critical
	 * section, but cannot move above it!  add_wait_queue's spin_lock acts
	 * as an acquire barrier and ensures that the read be ordered properly
	 * against the writes.  The following CAN happen and is safe:
	 *
	 *     poll                               write
	 *     -----------------                  ------------
	 *     lock ctx->wqh.lock (in poll_wait)
	 *     count = ctx->count
	 *     __add_wait_queue
	 *     unlock ctx->wqh.lock
	 *                                        lock ctx->qwh.lock
	 *                                        ctx->count += n
	 *                                        if (waitqueue_active)
	 *                                          wake_up_locked_poll
	 *                                        unlock ctx->qwh.lock
	 *     eventfd_poll returns 0
	 *
	 * but the following, which would miss a wakeup, cannot happen:
	 *
	 *     poll                               write
	 *     -----------------                  ------------
	 *     count = ctx->count (INVALID!)
	 *                                        lock ctx->qwh.lock
	 *                                        ctx->count += n
	 *                                        **waitqueue_active is false**
	 *                                        **no wake_up_locked_poll!**
	 *                                        unlock ctx->qwh.lock
	 *     lock ctx->wqh.lock (in poll_wait)
	 *     __add_wait_queue
	 *     unlock ctx->wqh.lock
	 *     eventfd_poll returns 0
	 */
	count = READ_ONCE(ctx->count);

	if (count > 0)
		events |= EPOLLIN;
	if (count == ULLONG_MAX)
		events |= EPOLLERR;
	if (ULLONG_MAX - 1 > count)
		events |= EPOLLOUT;

	return events;
}

void eventfd_ctx_do_read(struct eventfd_ctx *ctx, __u64 *cnt)
{
	lockdep_assert_held(&ctx->wqh.lock);

	*cnt = (ctx->flags & EFD_SEMAPHORE) ? 1 : ctx->count;
	ctx->count -= *cnt;
}
EXPORT_SYMBOL_GPL(eventfd_ctx_do_read);

/**
 * eventfd_ctx_remove_wait_queue - Read the current counter and removes wait queue.
 * @ctx: [in] Pointer to eventfd context.
 * @wait: [in] Wait queue to be removed.
 * @cnt: [out] Pointer to the 64-bit counter value.
 *
 * Returns %0 if successful, or the following error codes:
 *
 * -EAGAIN      : The operation would have blocked.
 *
 * This is used to atomically remove a wait queue entry from the eventfd wait
 * queue head, and read/reset the counter value.
 */
int eventfd_ctx_remove_wait_queue(struct eventfd_ctx *ctx, wait_queue_entry_t *wait,
				  __u64 *cnt)
{
	unsigned long flags;

	spin_lock_irqsave(&ctx->wqh.lock, flags);
	eventfd_ctx_do_read(ctx, cnt);
	__remove_wait_queue(&ctx->wqh, wait);
	if (*cnt != 0 && waitqueue_active(&ctx->wqh))
		wake_up_locked_poll(&ctx->wqh, EPOLLOUT);
	spin_unlock_irqrestore(&ctx->wqh.lock, flags);

	return *cnt != 0 ? 0 : -EAGAIN;
}
EXPORT_SYMBOL_GPL(eventfd_ctx_remove_wait_queue);

static ssize_t eventfd_read(struct kiocb *iocb, struct iov_iter *to)
{
	struct file *file = iocb->ki_filp;
	struct eventfd_ctx *ctx = file->private_data;
	__u64 ucnt = 0;
	DECLARE_WAITQUEUE(wait, current);

	if (iov_iter_count(to) < sizeof(ucnt))
		return -EINVAL;
	spin_lock_irq(&ctx->wqh.lock);
	if (!ctx->count) {
		if ((file->f_flags & O_NONBLOCK) ||
		    (iocb->ki_flags & IOCB_NOWAIT)) {
			spin_unlock_irq(&ctx->wqh.lock);
			return -EAGAIN;
		}
		__add_wait_queue(&ctx->wqh, &wait);
		for (;;) {
			set_current_state(TASK_INTERRUPTIBLE);
			if (ctx->count)
				break;
			if (signal_pending(current)) {
				__remove_wait_queue(&ctx->wqh, &wait);
				__set_current_state(TASK_RUNNING);
				spin_unlock_irq(&ctx->wqh.lock);
				return -ERESTARTSYS;
			}
			spin_unlock_irq(&ctx->wqh.lock);
			schedule();
			spin_lock_irq(&ctx->wqh.lock);
		}
		__remove_wait_queue(&ctx->wqh, &wait);
		__set_current_state(TASK_RUNNING);
	}
	eventfd_ctx_do_read(ctx, &ucnt);
	if (waitqueue_active(&ctx->wqh))
		wake_up_locked_poll(&ctx->wqh, EPOLLOUT);
	spin_unlock_irq(&ctx->wqh.lock);
	if (unlikely(copy_to_iter(&ucnt, sizeof(ucnt), to) != sizeof(ucnt)))
		return -EFAULT;

	return sizeof(ucnt);
}

static ssize_t eventfd_write(struct file *file, const char __user *buf, size_t count,
			     loff_t *ppos)
{
	struct eventfd_ctx *ctx = file->private_data;
	ssize_t res;
	__u64 ucnt;
	DECLARE_WAITQUEUE(wait, current);

	if (count < sizeof(ucnt))
		return -EINVAL;
	if (copy_from_user(&ucnt, buf, sizeof(ucnt)))
		return -EFAULT;
	if (ucnt == ULLONG_MAX)
		return -EINVAL;
	spin_lock_irq(&ctx->wqh.lock);
	res = -EAGAIN;
	if (ULLONG_MAX - ctx->count > ucnt)
		res = sizeof(ucnt);
	else if (!(file->f_flags & O_NONBLOCK)) {
		__add_wait_queue(&ctx->wqh, &wait);
		for (res = 0;;) {
			set_current_state(TASK_INTERRUPTIBLE);
			if (ULLONG_MAX - ctx->count > ucnt) {
				res = sizeof(ucnt);
				break;
			}
			if (signal_pending(current)) {
				res = -ERESTARTSYS;
				break;
			}
			spin_unlock_irq(&ctx->wqh.lock);
			schedule();
			spin_lock_irq(&ctx->wqh.lock);
		}
		__remove_wait_queue(&ctx->wqh, &wait);
		__set_current_state(TASK_RUNNING);
	}
	if (likely(res > 0)) {
		ctx->count += ucnt;
		if (waitqueue_active(&ctx->wqh))
			wake_up_locked_poll(&ctx->wqh, EPOLLIN);
	}
	spin_unlock_irq(&ctx->wqh.lock);

	return res;
}

#ifdef CONFIG_PROC_FS
static void eventfd_show_fdinfo(struct seq_file *m, struct file *f)
{
	struct eventfd_ctx *ctx = f->private_data;

	spin_lock_irq(&ctx->wqh.lock);
	seq_printf(m, "eventfd-count: %16llx\n",
		   (unsigned long long)ctx->count);
	spin_unlock_irq(&ctx->wqh.lock);
	seq_printf(m, "eventfd-id: %d\n", ctx->id);
}
#endif

static const struct file_operations eventfd_fops = {
#ifdef CONFIG_PROC_FS
	.show_fdinfo	= eventfd_show_fdinfo,
#endif
	.release	= eventfd_release,
	.poll		= eventfd_poll,
	.read_iter	= eventfd_read,
	.write		= eventfd_write,
	.llseek		= noop_llseek,
};

/**
 * eventfd_fget - Acquire a reference of an eventfd file descriptor.
 * @fd: [in] Eventfd file descriptor.
 *
 * Returns a pointer to the eventfd file structure in case of success, or the
 * following error pointer:
 *
 * -EBADF    : Invalid @fd file descriptor.
 * -EINVAL   : The @fd file descriptor is not an eventfd file.
 */
struct file *eventfd_fget(int fd)
{
	struct file *file;

	file = fget(fd);
	if (!file)
		return ERR_PTR(-EBADF);
	if (file->f_op != &eventfd_fops) {
		fput(file);
		return ERR_PTR(-EINVAL);
	}

	return file;
}
EXPORT_SYMBOL_GPL(eventfd_fget);

/**
 * eventfd_ctx_fdget - Acquires a reference to the internal eventfd context.
 * @fd: [in] Eventfd file descriptor.
 *
 * Returns a pointer to the internal eventfd context, otherwise the error
 * pointers returned by the following functions:
 *
 * eventfd_fget
 */
struct eventfd_ctx *eventfd_ctx_fdget(int fd)
{
	struct eventfd_ctx *ctx;
	struct fd f = fdget(fd);
	if (!f.file)
		return ERR_PTR(-EBADF);
	ctx = eventfd_ctx_fileget(f.file);
	fdput(f);
	return ctx;
}
EXPORT_SYMBOL_GPL(eventfd_ctx_fdget);

/**
 * eventfd_ctx_fileget - Acquires a reference to the internal eventfd context.
 * @file: [in] Eventfd file pointer.
 *
 * Returns a pointer to the internal eventfd context, otherwise the error
 * pointer:
 *
 * -EINVAL   : The @fd file descriptor is not an eventfd file.
 */
struct eventfd_ctx *eventfd_ctx_fileget(struct file *file)
{
	struct eventfd_ctx *ctx;

	if (file->f_op != &eventfd_fops)
		return ERR_PTR(-EINVAL);

	ctx = file->private_data;
	kref_get(&ctx->kref);
	return ctx;
}
EXPORT_SYMBOL_GPL(eventfd_ctx_fileget);

static int do_eventfd(unsigned int count, int flags)
{
	struct eventfd_ctx *ctx;
	struct file *file;
	int fd;

	/* Check the EFD_* constants for consistency.  */
	BUILD_BUG_ON(EFD_CLOEXEC != O_CLOEXEC);
	BUILD_BUG_ON(EFD_NONBLOCK != O_NONBLOCK);

	if (flags & ~EFD_FLAGS_SET)
		return -EINVAL;

	ctx = kmalloc(sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	kref_init(&ctx->kref);
	init_waitqueue_head(&ctx->wqh);
	ctx->count = count;
	ctx->flags = flags;
	ctx->id = ida_simple_get(&eventfd_ida, 0, 0, GFP_KERNEL);

	flags &= EFD_SHARED_FCNTL_FLAGS;
	flags |= O_RDWR;
	fd = get_unused_fd_flags(flags);
	if (fd < 0)
		goto err;

	file = anon_inode_getfile("[eventfd]", &eventfd_fops, ctx, flags);
	if (IS_ERR(file)) {
		put_unused_fd(fd);
		fd = PTR_ERR(file);
		goto err;
	}

	file->f_mode |= FMODE_NOWAIT;
	fd_install(fd, file);
	return fd;
err:
	eventfd_free_ctx(ctx);
	return fd;
}

SYSCALL_DEFINE2(eventfd2, unsigned int, count, int, flags)
{
	return do_eventfd(count, flags);
}

SYSCALL_DEFINE1(eventfd, unsigned int, count)
{
	return do_eventfd(count, 0);
}

