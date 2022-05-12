/* SPDX-License-Identifier: GPL-2.0 */
/*
 * (C) 2001 Clemson University and The University of Chicago
 *
 * See COPYING in top-level directory.
 */

/*
 *  The ORANGEFS Linux kernel support allows ORANGEFS volumes to be mounted and
 *  accessed through the Linux VFS (i.e. using standard I/O system calls).
 *  This support is only needed on clients that wish to mount the file system.
 *
 */

/*
 *  Declarations and macros for the ORANGEFS Linux kernel support.
 */

#ifndef __ORANGEFSKERNEL_H
#define __ORANGEFSKERNEL_H

#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/statfs.h>
#include <linux/backing-dev.h>
#include <linux/device.h>
#include <linux/mpage.h>
#include <linux/namei.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/vmalloc.h>

#include <linux/aio.h>
#include <linux/posix_acl.h>
#include <linux/posix_acl_xattr.h>
#include <linux/compat.h>
#include <linux/mount.h>
#include <linux/uaccess.h>
#include <linux/atomic.h>
#include <linux/uio.h>
#include <linux/sched/signal.h>
#include <linux/mm.h>
#include <linux/wait.h>
#include <linux/dcache.h>
#include <linux/pagemap.h>
#include <linux/poll.h>
#include <linux/rwsem.h>
#include <linux/xattr.h>
#include <linux/exportfs.h>
#include <linux/hashtable.h>

#include <asm/unaligned.h>

#include "orangefs-dev-proto.h"

#define ORANGEFS_DEFAULT_OP_TIMEOUT_SECS       20

#define ORANGEFS_BUFMAP_WAIT_TIMEOUT_SECS   30

#define ORANGEFS_DEFAULT_SLOT_TIMEOUT_SECS     900	/* 15 minutes */

#define ORANGEFS_REQDEVICE_NAME          "pvfs2-req"

#define ORANGEFS_DEVREQ_MAGIC             0x20030529
#define ORANGEFS_PURGE_RETRY_COUNT     0x00000005

#define MAX_DEV_REQ_UPSIZE (2 * sizeof(__s32) +   \
sizeof(__u64) + sizeof(struct orangefs_upcall_s))
#define MAX_DEV_REQ_DOWNSIZE (2 * sizeof(__s32) + \
sizeof(__u64) + sizeof(struct orangefs_downcall_s))

/*
 * valid orangefs kernel operation states
 *
 * unknown  - op was just initialized
 * waiting  - op is on request_list (upward bound)
 * inprogr  - op is in progress (waiting for downcall)
 * serviced - op has matching downcall; ok
 * purged   - op has to start a timer since client-core
 *            exited uncleanly before servicing op
 * given up - submitter has given up waiting for it
 */
enum orangefs_vfs_op_states {
	OP_VFS_STATE_UNKNOWN = 0,
	OP_VFS_STATE_WAITING = 1,
	OP_VFS_STATE_INPROGR = 2,
	OP_VFS_STATE_SERVICED = 4,
	OP_VFS_STATE_PURGED = 8,
	OP_VFS_STATE_GIVEN_UP = 16,
};

/*
 * orangefs kernel memory related flags
 */

#if (defined CONFIG_DEBUG_SLAB)
#define ORANGEFS_CACHE_CREATE_FLAGS SLAB_RED_ZONE
#else
#define ORANGEFS_CACHE_CREATE_FLAGS 0
#endif

extern int orangefs_init_acl(struct inode *inode, struct inode *dir);
extern const struct xattr_handler *orangefs_xattr_handlers[];

extern struct posix_acl *orangefs_get_acl(struct inode *inode, int type);
extern int orangefs_set_acl(struct user_namespace *mnt_userns,
			    struct inode *inode, struct posix_acl *acl,
			    int type);

/*
 * orangefs data structures
 */
struct orangefs_kernel_op_s {
	enum orangefs_vfs_op_states op_state;
	__u64 tag;

	/*
	 * Set uses_shared_memory to non zero if this operation uses
	 * shared memory. If true, then a retry on the op must also
	 * get a new shared memory buffer and re-populate it.
	 * Cancels don't care - it only matters for service_operation()
	 * retry logics and cancels don't go through it anymore. It
	 * safely stays non-zero when we use it as slot_to_free.
	 */
	union {
		int uses_shared_memory;
		int slot_to_free;
	};

	struct orangefs_upcall_s upcall;
	struct orangefs_downcall_s downcall;

	struct completion waitq;
	spinlock_t lock;

	int attempts;

	struct list_head list;
};

#define set_op_state_waiting(op)     ((op)->op_state = OP_VFS_STATE_WAITING)
#define set_op_state_inprogress(op)  ((op)->op_state = OP_VFS_STATE_INPROGR)
#define set_op_state_given_up(op)  ((op)->op_state = OP_VFS_STATE_GIVEN_UP)
static inline void set_op_state_serviced(struct orangefs_kernel_op_s *op)
{
	op->op_state = OP_VFS_STATE_SERVICED;
	complete(&op->waitq);
}

#define op_state_waiting(op)     ((op)->op_state & OP_VFS_STATE_WAITING)
#define op_state_in_progress(op) ((op)->op_state & OP_VFS_STATE_INPROGR)
#define op_state_serviced(op)    ((op)->op_state & OP_VFS_STATE_SERVICED)
#define op_state_purged(op)      ((op)->op_state & OP_VFS_STATE_PURGED)
#define op_state_given_up(op)    ((op)->op_state & OP_VFS_STATE_GIVEN_UP)
#define op_is_cancel(op)         ((op)->upcall.type == ORANGEFS_VFS_OP_CANCEL)

void op_release(struct orangefs_kernel_op_s *op);

extern void orangefs_bufmap_put(int);
static inline void put_cancel(struct orangefs_kernel_op_s *op)
{
	orangefs_bufmap_put(op->slot_to_free);
	op_release(op);
}

static inline void set_op_state_purged(struct orangefs_kernel_op_s *op)
{
	spin_lock(&op->lock);
	if (unlikely(op_is_cancel(op))) {
		list_del_init(&op->list);
		spin_unlock(&op->lock);
		put_cancel(op);
	} else {
		op->op_state |= OP_VFS_STATE_PURGED;
		complete(&op->waitq);
		spin_unlock(&op->lock);
	}
}

/* per inode private orangefs info */
struct orangefs_inode_s {
	struct orangefs_object_kref refn;
	char link_target[ORANGEFS_NAME_MAX];
	/*
	 * Reading/Writing Extended attributes need to acquire the appropriate
	 * reader/writer semaphore on the orangefs_inode_s structure.
	 */
	struct rw_semaphore xattr_sem;

	struct inode vfs_inode;
	sector_t last_failed_block_index_read;

	unsigned long getattr_time;
	unsigned long mapping_time;
	int attr_valid;
	kuid_t attr_uid;
	kgid_t attr_gid;
	unsigned long bitlock;

	DECLARE_HASHTABLE(xattr_cache, 4);
};

/* per superblock private orangefs info */
struct orangefs_sb_info_s {
	struct orangefs_khandle root_khandle;
	__s32 fs_id;
	int id;
	int flags;
#define ORANGEFS_OPT_INTR	0x01
#define ORANGEFS_OPT_LOCAL_LOCK	0x02
	char devname[ORANGEFS_MAX_SERVER_ADDR_LEN];
	struct super_block *sb;
	int mount_pending;
	int no_list;
	struct list_head list;
};

struct orangefs_stats {
	unsigned long cache_hits;
	unsigned long cache_misses;
	unsigned long reads;
	unsigned long writes;
};

struct orangefs_cached_xattr {
	struct hlist_node node;
	char key[ORANGEFS_MAX_XATTR_NAMELEN];
	char val[ORANGEFS_MAX_XATTR_VALUELEN];
	ssize_t length;
	unsigned long timeout;
};

struct orangefs_write_range {
	loff_t pos;
	size_t len;
	kuid_t uid;
	kgid_t gid;
};

extern struct orangefs_stats orangefs_stats;

/*
 * NOTE: See Documentation/filesystems/porting.rst for information
 * on implementing FOO_I and properly accessing fs private data
 */
static inline struct orangefs_inode_s *ORANGEFS_I(struct inode *inode)
{
	return container_of(inode, struct orangefs_inode_s, vfs_inode);
}

static inline struct orangefs_sb_info_s *ORANGEFS_SB(struct super_block *sb)
{
	return (struct orangefs_sb_info_s *) sb->s_fs_info;
}

/* ino_t descends from "unsigned long", 8 bytes, 64 bits. */
static inline ino_t orangefs_khandle_to_ino(struct orangefs_khandle *khandle)
{
	union {
		unsigned char u[8];
		__u64 ino;
	} ihandle;

	ihandle.u[0] = khandle->u[0] ^ khandle->u[4];
	ihandle.u[1] = khandle->u[1] ^ khandle->u[5];
	ihandle.u[2] = khandle->u[2] ^ khandle->u[6];
	ihandle.u[3] = khandle->u[3] ^ khandle->u[7];
	ihandle.u[4] = khandle->u[12] ^ khandle->u[8];
	ihandle.u[5] = khandle->u[13] ^ khandle->u[9];
	ihandle.u[6] = khandle->u[14] ^ khandle->u[10];
	ihandle.u[7] = khandle->u[15] ^ khandle->u[11];

	return ihandle.ino;
}

static inline struct orangefs_khandle *get_khandle_from_ino(struct inode *inode)
{
	return &(ORANGEFS_I(inode)->refn.khandle);
}

static inline int is_root_handle(struct inode *inode)
{
	gossip_debug(GOSSIP_DCACHE_DEBUG,
		     "%s: root handle: %pU, this handle: %pU:\n",
		     __func__,
		     &ORANGEFS_SB(inode->i_sb)->root_khandle,
		     get_khandle_from_ino(inode));

	if (ORANGEFS_khandle_cmp(&(ORANGEFS_SB(inode->i_sb)->root_khandle),
			     get_khandle_from_ino(inode)))
		return 0;
	else
		return 1;
}

static inline int match_handle(struct orangefs_khandle resp_handle,
			       struct inode *inode)
{
	gossip_debug(GOSSIP_DCACHE_DEBUG,
		     "%s: one handle: %pU, another handle:%pU:\n",
		     __func__,
		     &resp_handle,
		     get_khandle_from_ino(inode));

	if (ORANGEFS_khandle_cmp(&resp_handle, get_khandle_from_ino(inode)))
		return 0;
	else
		return 1;
}

/*
 * defined in orangefs-cache.c
 */
int op_cache_initialize(void);
int op_cache_finalize(void);
struct orangefs_kernel_op_s *op_alloc(__s32 type);
void orangefs_new_tag(struct orangefs_kernel_op_s *op);
char *get_opname_string(struct orangefs_kernel_op_s *new_op);

int orangefs_inode_cache_initialize(void);
int orangefs_inode_cache_finalize(void);

/*
 * defined in orangefs-mod.c
 */
void purge_inprogress_ops(void);

/*
 * defined in waitqueue.c
 */
void purge_waiting_ops(void);

/*
 * defined in super.c
 */
extern uint64_t orangefs_features;

struct dentry *orangefs_mount(struct file_system_type *fst,
			   int flags,
			   const char *devname,
			   void *data);

void orangefs_kill_sb(struct super_block *sb);
int orangefs_remount(struct orangefs_sb_info_s *);

int fsid_key_table_initialize(void);
void fsid_key_table_finalize(void);

/*
 * defined in inode.c
 */
vm_fault_t orangefs_page_mkwrite(struct vm_fault *);
struct inode *orangefs_new_inode(struct super_block *sb,
			      struct inode *dir,
			      int mode,
			      dev_t dev,
			      struct orangefs_object_kref *ref);

int __orangefs_setattr(struct inode *, struct iattr *);
int orangefs_setattr(struct user_namespace *, struct dentry *, struct iattr *);

int orangefs_getattr(struct user_namespace *mnt_userns, const struct path *path,
		     struct kstat *stat, u32 request_mask, unsigned int flags);

int orangefs_permission(struct user_namespace *mnt_userns,
			struct inode *inode, int mask);

int orangefs_update_time(struct inode *, struct timespec64 *, int);

/*
 * defined in xattr.c
 */
ssize_t orangefs_listxattr(struct dentry *dentry, char *buffer, size_t size);

/*
 * defined in namei.c
 */
struct inode *orangefs_iget(struct super_block *sb,
			 struct orangefs_object_kref *ref);

/*
 * defined in devorangefs-req.c
 */
extern uint32_t orangefs_userspace_version;

int orangefs_dev_init(void);
void orangefs_dev_cleanup(void);
int is_daemon_in_service(void);
bool __is_daemon_in_service(void);

/*
 * defined in file.c
 */
int orangefs_revalidate_mapping(struct inode *);
ssize_t wait_for_direct_io(enum ORANGEFS_io_type, struct inode *, loff_t *,
    struct iov_iter *, size_t, loff_t, struct orangefs_write_range *, int *,
    struct file *);
ssize_t do_readv_writev(enum ORANGEFS_io_type, struct file *, loff_t *,
    struct iov_iter *);

/*
 * defined in orangefs-utils.c
 */
__s32 fsid_of_op(struct orangefs_kernel_op_s *op);

ssize_t orangefs_inode_getxattr(struct inode *inode,
			     const char *name,
			     void *buffer,
			     size_t size);

int orangefs_inode_setxattr(struct inode *inode,
			 const char *name,
			 const void *value,
			 size_t size,
			 int flags);

#define ORANGEFS_GETATTR_NEW 1
#define ORANGEFS_GETATTR_SIZE 2

int orangefs_inode_getattr(struct inode *, int);

int orangefs_inode_check_changed(struct inode *inode);

int orangefs_inode_setattr(struct inode *inode);

bool orangefs_cancel_op_in_progress(struct orangefs_kernel_op_s *op);

int orangefs_normalize_to_errno(__s32 error_code);

extern struct mutex orangefs_request_mutex;
extern int op_timeout_secs;
extern int slot_timeout_secs;
extern int orangefs_cache_timeout_msecs;
extern int orangefs_dcache_timeout_msecs;
extern int orangefs_getattr_timeout_msecs;
extern struct list_head orangefs_superblocks;
extern spinlock_t orangefs_superblocks_lock;
extern struct list_head orangefs_request_list;
extern spinlock_t orangefs_request_list_lock;
extern wait_queue_head_t orangefs_request_list_waitq;
extern struct list_head *orangefs_htable_ops_in_progress;
extern spinlock_t orangefs_htable_ops_in_progress_lock;
extern int hash_table_size;

extern const struct file_operations orangefs_file_operations;
extern const struct inode_operations orangefs_symlink_inode_operations;
extern const struct inode_operations orangefs_dir_inode_operations;
extern const struct file_operations orangefs_dir_operations;
extern const struct dentry_operations orangefs_dentry_operations;

/*
 * misc convenience macros
 */

#define ORANGEFS_OP_INTERRUPTIBLE 1   /* service_operation() is interruptible */
#define ORANGEFS_OP_PRIORITY      2   /* service_operation() is high priority */
#define ORANGEFS_OP_CANCELLATION  4   /* this is a cancellation */
#define ORANGEFS_OP_NO_MUTEX      8   /* don't acquire request_mutex */
#define ORANGEFS_OP_ASYNC         16  /* Queue it, but don't wait */
#define ORANGEFS_OP_WRITEBACK     32

int service_operation(struct orangefs_kernel_op_s *op,
		      const char *op_name,
		      int flags);

#define get_interruptible_flag(inode) \
	((ORANGEFS_SB(inode->i_sb)->flags & ORANGEFS_OPT_INTR) ? \
		ORANGEFS_OP_INTERRUPTIBLE : 0)

#define fill_default_sys_attrs(sys_attr, type, mode)			\
do {									\
	sys_attr.owner = from_kuid(&init_user_ns, current_fsuid()); \
	sys_attr.group = from_kgid(&init_user_ns, current_fsgid()); \
	sys_attr.perms = ORANGEFS_util_translate_mode(mode);		\
	sys_attr.mtime = 0;						\
	sys_attr.atime = 0;						\
	sys_attr.ctime = 0;						\
	sys_attr.mask = ORANGEFS_ATTR_SYS_ALL_SETABLE;			\
} while (0)

static inline void orangefs_set_timeout(struct dentry *dentry)
{
	unsigned long time = jiffies + orangefs_dcache_timeout_msecs*HZ/1000;

	dentry->d_fsdata = (void *) time;
}

#endif /* __ORANGEFSKERNEL_H */
