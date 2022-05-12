// SPDX-License-Identifier: GPL-2.0-only
/*
 *  linux/fs/file_table.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *  Copyright (C) 1997 David S. Miller (davem@caip.rutgers.edu)
 */

#include <linux/string.h>
#include <linux/slab.h>
#include <linux/file.h>
#include <linux/fdtable.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/security.h>
#include <linux/cred.h>
#include <linux/eventpoll.h>
#include <linux/rcupdate.h>
#include <linux/mount.h>
#include <linux/capability.h>
#include <linux/cdev.h>
#include <linux/fsnotify.h>
#include <linux/sysctl.h>
#include <linux/percpu_counter.h>
#include <linux/percpu.h>
#include <linux/task_work.h>
#include <linux/ima.h>
#include <linux/swap.h>

#include <linux/atomic.h>

#include "internal.h"

/* sysctl tunables... */
struct files_stat_struct files_stat = {
	.max_files = NR_FILE
};

/* SLAB cache for file structures */
static struct kmem_cache *filp_cachep __read_mostly;

static struct percpu_counter nr_files __cacheline_aligned_in_smp;

static void file_free_rcu(struct rcu_head *head)
{
	struct file *f = container_of(head, struct file, f_u.fu_rcuhead);

	put_cred(f->f_cred);
	kmem_cache_free(filp_cachep, f);
}

static inline void file_free(struct file *f)
{
	security_file_free(f);
	if (!(f->f_mode & FMODE_NOACCOUNT))
		percpu_counter_dec(&nr_files);
	call_rcu(&f->f_u.fu_rcuhead, file_free_rcu);
}

/*
 * Return the total number of open files in the system
 */
static long get_nr_files(void)
{
	return percpu_counter_read_positive(&nr_files);
}

/*
 * Return the maximum number of open files in the system
 */
unsigned long get_max_files(void)
{
	return files_stat.max_files;
}
EXPORT_SYMBOL_GPL(get_max_files);

/*
 * Handle nr_files sysctl
 */
#if defined(CONFIG_SYSCTL) && defined(CONFIG_PROC_FS)
int proc_nr_files(struct ctl_table *table, int write,
                     void *buffer, size_t *lenp, loff_t *ppos)
{
	files_stat.nr_files = get_nr_files();
	return proc_doulongvec_minmax(table, write, buffer, lenp, ppos);
}
#else
int proc_nr_files(struct ctl_table *table, int write,
                     void *buffer, size_t *lenp, loff_t *ppos)
{
	return -ENOSYS;
}
#endif

static struct file *__alloc_file(int flags, const struct cred *cred)
{
	struct file *f;
	int error;

	f = kmem_cache_zalloc(filp_cachep, GFP_KERNEL);
	if (unlikely(!f))
		return ERR_PTR(-ENOMEM);

	f->f_cred = get_cred(cred);
	error = security_file_alloc(f);
	if (unlikely(error)) {
		file_free_rcu(&f->f_u.fu_rcuhead);
		return ERR_PTR(error);
	}

	atomic_long_set(&f->f_count, 1);
	rwlock_init(&f->f_owner.lock);
	spin_lock_init(&f->f_lock);
	mutex_init(&f->f_pos_lock);
	f->f_flags = flags;
	f->f_mode = OPEN_FMODE(flags);
	/* f->f_version: 0 */

	return f;
}

/* Find an unused file structure and return a pointer to it.
 * Returns an error pointer if some error happend e.g. we over file
 * structures limit, run out of memory or operation is not permitted.
 *
 * Be very careful using this.  You are responsible for
 * getting write access to any mount that you might assign
 * to this filp, if it is opened for write.  If this is not
 * done, you will imbalance int the mount's writer count
 * and a warning at __fput() time.
 */
struct file *alloc_empty_file(int flags, const struct cred *cred)
{
	static long old_max;
	struct file *f;

	/*
	 * Privileged users can go above max_files
	 */
	if (get_nr_files() >= files_stat.max_files && !capable(CAP_SYS_ADMIN)) {
		/*
		 * percpu_counters are inaccurate.  Do an expensive check before
		 * we go and fail.
		 */
		if (percpu_counter_sum_positive(&nr_files) >= files_stat.max_files)
			goto over;
	}

	f = __alloc_file(flags, cred);
	if (!IS_ERR(f))
		percpu_counter_inc(&nr_files);

	return f;

over:
	/* Ran out of filps - report that */
	if (get_nr_files() > old_max) {
		pr_info("VFS: file-max limit %lu reached\n", get_max_files());
		old_max = get_nr_files();
	}
	return ERR_PTR(-ENFILE);
}

/*
 * Variant of alloc_empty_file() that doesn't check and modify nr_files.
 *
 * Should not be used unless there's a very good reason to do so.
 */
struct file *alloc_empty_file_noaccount(int flags, const struct cred *cred)
{
	struct file *f = __alloc_file(flags, cred);

	if (!IS_ERR(f))
		f->f_mode |= FMODE_NOACCOUNT;

	return f;
}

/**
 * alloc_file - allocate and initialize a 'struct file'
 *
 * @path: the (dentry, vfsmount) pair for the new file
 * @flags: O_... flags with which the new file will be opened
 * @fop: the 'struct file_operations' for the new file
 */
static struct file *alloc_file(const struct path *path, int flags,
		const struct file_operations *fop)
{
	struct file *file;

	file = alloc_empty_file(flags, current_cred());
	if (IS_ERR(file))
		return file;

	file->f_path = *path;
	file->f_inode = path->dentry->d_inode;
	file->f_mapping = path->dentry->d_inode->i_mapping;
	file->f_wb_err = filemap_sample_wb_err(file->f_mapping);
	file->f_sb_err = file_sample_sb_err(file);
	if ((file->f_mode & FMODE_READ) &&
	     likely(fop->read || fop->read_iter))
		file->f_mode |= FMODE_CAN_READ;
	if ((file->f_mode & FMODE_WRITE) &&
	     likely(fop->write || fop->write_iter))
		file->f_mode |= FMODE_CAN_WRITE;
	file->f_mode |= FMODE_OPENED;
	file->f_op = fop;
	if ((file->f_mode & (FMODE_READ | FMODE_WRITE)) == FMODE_READ)
		i_readcount_inc(path->dentry->d_inode);
	return file;
}

struct file *alloc_file_pseudo(struct inode *inode, struct vfsmount *mnt,
				const char *name, int flags,
				const struct file_operations *fops)
{
	static const struct dentry_operations anon_ops = {
		.d_dname = simple_dname
	};
	struct qstr this = QSTR_INIT(name, strlen(name));
	struct path path;
	struct file *file;

	path.dentry = d_alloc_pseudo(mnt->mnt_sb, &this);
	if (!path.dentry)
		return ERR_PTR(-ENOMEM);
	if (!mnt->mnt_sb->s_d_op)
		d_set_d_op(path.dentry, &anon_ops);
	path.mnt = mntget(mnt);
	d_instantiate(path.dentry, inode);
	file = alloc_file(&path, flags, fops);
	if (IS_ERR(file)) {
		ihold(inode);
		path_put(&path);
	}
	return file;
}
EXPORT_SYMBOL(alloc_file_pseudo);

struct file *alloc_file_clone(struct file *base, int flags,
				const struct file_operations *fops)
{
	struct file *f = alloc_file(&base->f_path, flags, fops);
	if (!IS_ERR(f)) {
		path_get(&f->f_path);
		f->f_mapping = base->f_mapping;
	}
	return f;
}

/* the real guts of fput() - releasing the last reference to file
 */
static void __fput(struct file *file)
{
	struct dentry *dentry = file->f_path.dentry;
	struct vfsmount *mnt = file->f_path.mnt;
	struct inode *inode = file->f_inode;
	fmode_t mode = file->f_mode;

	if (unlikely(!(file->f_mode & FMODE_OPENED)))
		goto out;

	might_sleep();

	fsnotify_close(file);
	/*
	 * The function eventpoll_release() should be the first called
	 * in the file cleanup chain.
	 */
	eventpoll_release(file);
	locks_remove_file(file);

	ima_file_free(file);
	if (unlikely(file->f_flags & FASYNC)) {
		if (file->f_op->fasync)
			file->f_op->fasync(-1, file, 0);
	}
	if (file->f_op->release)
		file->f_op->release(inode, file);
	if (unlikely(S_ISCHR(inode->i_mode) && inode->i_cdev != NULL &&
		     !(mode & FMODE_PATH))) {
		cdev_put(inode->i_cdev);
	}
	fops_put(file->f_op);
	put_pid(file->f_owner.pid);
	if ((mode & (FMODE_READ | FMODE_WRITE)) == FMODE_READ)
		i_readcount_dec(inode);
	if (mode & FMODE_WRITER) {
		put_write_access(inode);
		__mnt_drop_write(mnt);
	}
	dput(dentry);
	if (unlikely(mode & FMODE_NEED_UNMOUNT))
		dissolve_on_fput(mnt);
	mntput(mnt);
out:
	file_free(file);
}

static LLIST_HEAD(delayed_fput_list);
static void delayed_fput(struct work_struct *unused)
{
	struct llist_node *node = llist_del_all(&delayed_fput_list);
	struct file *f, *t;

	llist_for_each_entry_safe(f, t, node, f_u.fu_llist)
		__fput(f);
}

static void ____fput(struct callback_head *work)
{
	__fput(container_of(work, struct file, f_u.fu_rcuhead));
}

/*
 * If kernel thread really needs to have the final fput() it has done
 * to complete, call this.  The only user right now is the boot - we
 * *do* need to make sure our writes to binaries on initramfs has
 * not left us with opened struct file waiting for __fput() - execve()
 * won't work without that.  Please, don't add more callers without
 * very good reasons; in particular, never call that with locks
 * held and never call that from a thread that might need to do
 * some work on any kind of umount.
 */
void flush_delayed_fput(void)
{
	delayed_fput(NULL);
}
EXPORT_SYMBOL_GPL(flush_delayed_fput);

static DECLARE_DELAYED_WORK(delayed_fput_work, delayed_fput);

void fput_many(struct file *file, unsigned int refs)
{
	if (atomic_long_sub_and_test(refs, &file->f_count)) {
		struct task_struct *task = current;

		if (likely(!in_interrupt() && !(task->flags & PF_KTHREAD))) {
			init_task_work(&file->f_u.fu_rcuhead, ____fput);
			if (!task_work_add(task, &file->f_u.fu_rcuhead, TWA_RESUME))
				return;
			/*
			 * After this task has run exit_task_work(),
			 * task_work_add() will fail.  Fall through to delayed
			 * fput to avoid leaking *file.
			 */
		}

		if (llist_add(&file->f_u.fu_llist, &delayed_fput_list))
			schedule_delayed_work(&delayed_fput_work, 1);
	}
}

void fput(struct file *file)
{
	fput_many(file, 1);
}

/*
 * synchronous analog of fput(); for kernel threads that might be needed
 * in some umount() (and thus can't use flush_delayed_fput() without
 * risking deadlocks), need to wait for completion of __fput() and know
 * for this specific struct file it won't involve anything that would
 * need them.  Use only if you really need it - at the very least,
 * don't blindly convert fput() by kernel thread to that.
 */
void __fput_sync(struct file *file)
{
	if (atomic_long_dec_and_test(&file->f_count)) {
		struct task_struct *task = current;
		BUG_ON(!(task->flags & PF_KTHREAD));
		__fput(file);
	}
}

EXPORT_SYMBOL(fput);

void __init files_init(void)
{
	filp_cachep = kmem_cache_create("filp", sizeof(struct file), 0,
			SLAB_HWCACHE_ALIGN | SLAB_PANIC | SLAB_ACCOUNT, NULL);
	percpu_counter_init(&nr_files, 0, GFP_KERNEL);
}

/*
 * One file with associated inode and dcache is very roughly 1K. Per default
 * do not use more than 10% of our memory for files.
 */
void __init files_maxfiles_init(void)
{
	unsigned long n;
	unsigned long nr_pages = totalram_pages();
	unsigned long memreserve = (nr_pages - nr_free_pages()) * 3/2;

	memreserve = min(memreserve, nr_pages - 1);
	n = ((nr_pages - memreserve) * (PAGE_SIZE / 1024)) / 10;

	files_stat.max_files = max_t(unsigned long, n, NR_FILE);
}
