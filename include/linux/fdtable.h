/* SPDX-License-Identifier: GPL-2.0 */
/*
 * descriptor table internals; you almost certainly want file.h instead.
 */

#ifndef __LINUX_FDTABLE_H
#define __LINUX_FDTABLE_H

#include <linux/posix_types.h>
#include <linux/compiler.h>
#include <linux/spinlock.h>
#include <linux/rcupdate.h>
#include <linux/nospec.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/fs.h>

#include <linux/atomic.h>

/*
 * The default fd array needs to be at least BITS_PER_LONG,
 * as this is the granularity returned by copy_fdset().
 */
#define NR_OPEN_DEFAULT BITS_PER_LONG
#define NR_OPEN_MAX ~0U

struct fdtable {
	unsigned int max_fds;
	struct file __rcu **fd;      /* current fd array */
	unsigned long *close_on_exec;
	unsigned long *open_fds;
	unsigned long *full_fds_bits;
	struct rcu_head rcu;
};

static inline bool close_on_exec(unsigned int fd, const struct fdtable *fdt)
{
	return test_bit(fd, fdt->close_on_exec);
}

static inline bool fd_is_open(unsigned int fd, const struct fdtable *fdt)
{
	return test_bit(fd, fdt->open_fds);
}

/*
 * Open file table structure
 */
struct files_struct {
  /*
   * read mostly part
   */
	atomic_t count;
	bool resize_in_progress;
	wait_queue_head_t resize_wait;

	struct fdtable __rcu *fdt;
	struct fdtable fdtab;
  /*
   * written part on a separate cache line in SMP
   */
	spinlock_t file_lock ____cacheline_aligned_in_smp;
	unsigned int next_fd;
	unsigned long close_on_exec_init[1];
	unsigned long open_fds_init[1];
	unsigned long full_fds_bits_init[1];
	struct file __rcu * fd_array[NR_OPEN_DEFAULT];
};

struct file_operations;
struct vfsmount;
struct dentry;

#define rcu_dereference_check_fdtable(files, fdtfd) \
	rcu_dereference_check((fdtfd), lockdep_is_held(&(files)->file_lock))

#define files_fdtable(files) \
	rcu_dereference_check_fdtable((files), (files)->fdt)

/*
 * The caller must ensure that fd table isn't shared or hold rcu or file lock
 */
static inline struct file *files_lookup_fd_raw(struct files_struct *files, unsigned int fd)
{
	struct fdtable *fdt = rcu_dereference_raw(files->fdt);

	if (fd < fdt->max_fds) {
		fd = array_index_nospec(fd, fdt->max_fds);
		return rcu_dereference_raw(fdt->fd[fd]);
	}
	return NULL;
}

static inline struct file *files_lookup_fd_locked(struct files_struct *files, unsigned int fd)
{
	RCU_LOCKDEP_WARN(!lockdep_is_held(&files->file_lock),
			   "suspicious rcu_dereference_check() usage");
	return files_lookup_fd_raw(files, fd);
}

static inline struct file *files_lookup_fd_rcu(struct files_struct *files, unsigned int fd)
{
	RCU_LOCKDEP_WARN(!rcu_read_lock_held(),
			   "suspicious rcu_dereference_check() usage");
	return files_lookup_fd_raw(files, fd);
}

static inline struct file *lookup_fd_rcu(unsigned int fd)
{
	return files_lookup_fd_rcu(current->files, fd);
}

struct file *task_lookup_fd_rcu(struct task_struct *task, unsigned int fd);
struct file *task_lookup_next_fd_rcu(struct task_struct *task, unsigned int *fd);

struct task_struct;

void put_files_struct(struct files_struct *fs);
int unshare_files(void);
struct files_struct *dup_fd(struct files_struct *, unsigned, int *) __latent_entropy;
void do_close_on_exec(struct files_struct *);
int iterate_fd(struct files_struct *, unsigned,
		int (*)(const void *, struct file *, unsigned),
		const void *);

extern int close_fd(unsigned int fd);
extern int __close_range(unsigned int fd, unsigned int max_fd, unsigned int flags);
extern int close_fd_get_file(unsigned int fd, struct file **res);
extern int unshare_fd(unsigned long unshare_flags, unsigned int max_fds,
		      struct files_struct **new_fdp);

extern struct kmem_cache *files_cachep;

#endif /* __LINUX_FDTABLE_H */
