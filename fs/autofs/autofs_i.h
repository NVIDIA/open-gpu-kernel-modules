/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Copyright 1997-1998 Transmeta Corporation - All Rights Reserved
 *  Copyright 2005-2006 Ian Kent <raven@themaw.net>
 */

/* Internal header file for autofs */

#include <linux/auto_fs.h>
#include <linux/auto_dev-ioctl.h>

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/string.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/mount.h>
#include <linux/namei.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <linux/list.h>
#include <linux/completion.h>
#include <linux/file.h>
#include <linux/magic.h>

/* This is the range of ioctl() numbers we claim as ours */
#define AUTOFS_IOC_FIRST     AUTOFS_IOC_READY
#define AUTOFS_IOC_COUNT     32

#define AUTOFS_DEV_IOCTL_IOC_FIRST	(AUTOFS_DEV_IOCTL_VERSION)
#define AUTOFS_DEV_IOCTL_IOC_COUNT \
	(AUTOFS_DEV_IOCTL_ISMOUNTPOINT_CMD - AUTOFS_DEV_IOCTL_VERSION_CMD)

#ifdef pr_fmt
#undef pr_fmt
#endif
#define pr_fmt(fmt) KBUILD_MODNAME ":pid:%d:%s: " fmt, current->pid, __func__

extern struct file_system_type autofs_fs_type;

/*
 * Unified info structure.  This is pointed to by both the dentry and
 * inode structures.  Each file in the filesystem has an instance of this
 * structure.  It holds a reference to the dentry, so dentries are never
 * flushed while the file exists.  All name lookups are dealt with at the
 * dentry level, although the filesystem can interfere in the validation
 * process.  Readdir is implemented by traversing the dentry lists.
 */
struct autofs_info {
	struct dentry	*dentry;
	struct inode	*inode;

	int		flags;

	struct completion expire_complete;

	struct list_head active;

	struct list_head expiring;

	struct autofs_sb_info *sbi;
	unsigned long last_used;
	int count;

	kuid_t uid;
	kgid_t gid;
	struct rcu_head rcu;
};

#define AUTOFS_INF_EXPIRING	(1<<0) /* dentry in the process of expiring */
#define AUTOFS_INF_WANT_EXPIRE	(1<<1) /* the dentry is being considered
					* for expiry, so RCU_walk is
					* not permitted.  If it progresses to
					* actual expiry attempt, the flag is
					* not cleared when EXPIRING is set -
					* in that case it gets cleared only
					* when it comes to clearing EXPIRING.
					*/
#define AUTOFS_INF_PENDING	(1<<2) /* dentry pending mount */

struct autofs_wait_queue {
	wait_queue_head_t queue;
	struct autofs_wait_queue *next;
	autofs_wqt_t wait_queue_token;
	/* We use the following to see what we are waiting for */
	struct qstr name;
	u32 offset;
	u32 dev;
	u64 ino;
	kuid_t uid;
	kgid_t gid;
	pid_t pid;
	pid_t tgid;
	/* This is for status reporting upon return */
	int status;
	unsigned int wait_ctr;
};

#define AUTOFS_SBI_MAGIC 0x6d4a556d

#define AUTOFS_SBI_CATATONIC	0x0001
#define AUTOFS_SBI_STRICTEXPIRE 0x0002
#define AUTOFS_SBI_IGNORE	0x0004

struct autofs_sb_info {
	u32 magic;
	int pipefd;
	struct file *pipe;
	struct pid *oz_pgrp;
	int version;
	int sub_version;
	int min_proto;
	int max_proto;
	unsigned int flags;
	unsigned long exp_timeout;
	unsigned int type;
	struct super_block *sb;
	struct mutex wq_mutex;
	struct mutex pipe_mutex;
	spinlock_t fs_lock;
	struct autofs_wait_queue *queues; /* Wait queue pointer */
	spinlock_t lookup_lock;
	struct list_head active_list;
	struct list_head expiring_list;
	struct rcu_head rcu;
};

static inline struct autofs_sb_info *autofs_sbi(struct super_block *sb)
{
	return (struct autofs_sb_info *)(sb->s_fs_info);
}

static inline struct autofs_info *autofs_dentry_ino(struct dentry *dentry)
{
	return (struct autofs_info *)(dentry->d_fsdata);
}

/* autofs_oz_mode(): do we see the man behind the curtain?  (The
 * processes which do manipulations for us in user space sees the raw
 * filesystem without "magic".)
 */
static inline int autofs_oz_mode(struct autofs_sb_info *sbi)
{
	return ((sbi->flags & AUTOFS_SBI_CATATONIC) ||
		 task_pgrp(current) == sbi->oz_pgrp);
}

struct inode *autofs_get_inode(struct super_block *, umode_t);
void autofs_free_ino(struct autofs_info *);

/* Expiration */
int is_autofs_dentry(struct dentry *);
int autofs_expire_wait(const struct path *path, int rcu_walk);
int autofs_expire_run(struct super_block *, struct vfsmount *,
		      struct autofs_sb_info *,
		      struct autofs_packet_expire __user *);
int autofs_do_expire_multi(struct super_block *sb, struct vfsmount *mnt,
			   struct autofs_sb_info *sbi, unsigned int how);
int autofs_expire_multi(struct super_block *, struct vfsmount *,
			struct autofs_sb_info *, int __user *);

/* Device node initialization */

int autofs_dev_ioctl_init(void);
void autofs_dev_ioctl_exit(void);

/* Operations structures */

extern const struct inode_operations autofs_symlink_inode_operations;
extern const struct inode_operations autofs_dir_inode_operations;
extern const struct file_operations autofs_dir_operations;
extern const struct file_operations autofs_root_operations;
extern const struct dentry_operations autofs_dentry_operations;

/* VFS automount flags management functions */
static inline void __managed_dentry_set_managed(struct dentry *dentry)
{
	dentry->d_flags |= (DCACHE_NEED_AUTOMOUNT|DCACHE_MANAGE_TRANSIT);
}

static inline void managed_dentry_set_managed(struct dentry *dentry)
{
	spin_lock(&dentry->d_lock);
	__managed_dentry_set_managed(dentry);
	spin_unlock(&dentry->d_lock);
}

static inline void __managed_dentry_clear_managed(struct dentry *dentry)
{
	dentry->d_flags &= ~(DCACHE_NEED_AUTOMOUNT|DCACHE_MANAGE_TRANSIT);
}

static inline void managed_dentry_clear_managed(struct dentry *dentry)
{
	spin_lock(&dentry->d_lock);
	__managed_dentry_clear_managed(dentry);
	spin_unlock(&dentry->d_lock);
}

/* Initializing function */

int autofs_fill_super(struct super_block *, void *, int);
struct autofs_info *autofs_new_ino(struct autofs_sb_info *);
void autofs_clean_ino(struct autofs_info *);

static inline int autofs_prepare_pipe(struct file *pipe)
{
	if (!(pipe->f_mode & FMODE_CAN_WRITE))
		return -EINVAL;
	if (!S_ISFIFO(file_inode(pipe)->i_mode))
		return -EINVAL;
	/* We want a packet pipe */
	pipe->f_flags |= O_DIRECT;
	/* We don't expect -EAGAIN */
	pipe->f_flags &= ~O_NONBLOCK;
	return 0;
}

/* Queue management functions */

int autofs_wait(struct autofs_sb_info *,
		 const struct path *, enum autofs_notify);
int autofs_wait_release(struct autofs_sb_info *, autofs_wqt_t, int);
void autofs_catatonic_mode(struct autofs_sb_info *);

static inline u32 autofs_get_dev(struct autofs_sb_info *sbi)
{
	return new_encode_dev(sbi->sb->s_dev);
}

static inline u64 autofs_get_ino(struct autofs_sb_info *sbi)
{
	return d_inode(sbi->sb->s_root)->i_ino;
}

static inline void __autofs_add_expiring(struct dentry *dentry)
{
	struct autofs_sb_info *sbi = autofs_sbi(dentry->d_sb);
	struct autofs_info *ino = autofs_dentry_ino(dentry);

	if (ino) {
		if (list_empty(&ino->expiring))
			list_add(&ino->expiring, &sbi->expiring_list);
	}
}

static inline void autofs_add_expiring(struct dentry *dentry)
{
	struct autofs_sb_info *sbi = autofs_sbi(dentry->d_sb);
	struct autofs_info *ino = autofs_dentry_ino(dentry);

	if (ino) {
		spin_lock(&sbi->lookup_lock);
		if (list_empty(&ino->expiring))
			list_add(&ino->expiring, &sbi->expiring_list);
		spin_unlock(&sbi->lookup_lock);
	}
}

static inline void autofs_del_expiring(struct dentry *dentry)
{
	struct autofs_sb_info *sbi = autofs_sbi(dentry->d_sb);
	struct autofs_info *ino = autofs_dentry_ino(dentry);

	if (ino) {
		spin_lock(&sbi->lookup_lock);
		if (!list_empty(&ino->expiring))
			list_del_init(&ino->expiring);
		spin_unlock(&sbi->lookup_lock);
	}
}

void autofs_kill_sb(struct super_block *);
