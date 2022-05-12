/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_USER_NAMESPACE_H
#define _LINUX_USER_NAMESPACE_H

#include <linux/kref.h>
#include <linux/nsproxy.h>
#include <linux/ns_common.h>
#include <linux/sched.h>
#include <linux/workqueue.h>
#include <linux/rwsem.h>
#include <linux/sysctl.h>
#include <linux/err.h>

#define UID_GID_MAP_MAX_BASE_EXTENTS 5
#define UID_GID_MAP_MAX_EXTENTS 340

struct uid_gid_extent {
	u32 first;
	u32 lower_first;
	u32 count;
};

struct uid_gid_map { /* 64 bytes -- 1 cache line */
	u32 nr_extents;
	union {
		struct uid_gid_extent extent[UID_GID_MAP_MAX_BASE_EXTENTS];
		struct {
			struct uid_gid_extent *forward;
			struct uid_gid_extent *reverse;
		};
	};
};

#define USERNS_SETGROUPS_ALLOWED 1UL

#define USERNS_INIT_FLAGS USERNS_SETGROUPS_ALLOWED

struct ucounts;

enum ucount_type {
	UCOUNT_USER_NAMESPACES,
	UCOUNT_PID_NAMESPACES,
	UCOUNT_UTS_NAMESPACES,
	UCOUNT_IPC_NAMESPACES,
	UCOUNT_NET_NAMESPACES,
	UCOUNT_MNT_NAMESPACES,
	UCOUNT_CGROUP_NAMESPACES,
	UCOUNT_TIME_NAMESPACES,
#ifdef CONFIG_INOTIFY_USER
	UCOUNT_INOTIFY_INSTANCES,
	UCOUNT_INOTIFY_WATCHES,
#endif
#ifdef CONFIG_FANOTIFY
	UCOUNT_FANOTIFY_GROUPS,
	UCOUNT_FANOTIFY_MARKS,
#endif
	UCOUNT_COUNTS,
};

struct user_namespace {
	struct uid_gid_map	uid_map;
	struct uid_gid_map	gid_map;
	struct uid_gid_map	projid_map;
	struct user_namespace	*parent;
	int			level;
	kuid_t			owner;
	kgid_t			group;
	struct ns_common	ns;
	unsigned long		flags;
	/* parent_could_setfcap: true if the creator if this ns had CAP_SETFCAP
	 * in its effective capability set at the child ns creation time. */
	bool			parent_could_setfcap;

#ifdef CONFIG_KEYS
	/* List of joinable keyrings in this namespace.  Modification access of
	 * these pointers is controlled by keyring_sem.  Once
	 * user_keyring_register is set, it won't be changed, so it can be
	 * accessed directly with READ_ONCE().
	 */
	struct list_head	keyring_name_list;
	struct key		*user_keyring_register;
	struct rw_semaphore	keyring_sem;
#endif

	/* Register of per-UID persistent keyrings for this namespace */
#ifdef CONFIG_PERSISTENT_KEYRINGS
	struct key		*persistent_keyring_register;
#endif
	struct work_struct	work;
#ifdef CONFIG_SYSCTL
	struct ctl_table_set	set;
	struct ctl_table_header *sysctls;
#endif
	struct ucounts		*ucounts;
	int ucount_max[UCOUNT_COUNTS];
} __randomize_layout;

struct ucounts {
	struct hlist_node node;
	struct user_namespace *ns;
	kuid_t uid;
	int count;
	atomic_t ucount[UCOUNT_COUNTS];
};

extern struct user_namespace init_user_ns;

bool setup_userns_sysctls(struct user_namespace *ns);
void retire_userns_sysctls(struct user_namespace *ns);
struct ucounts *inc_ucount(struct user_namespace *ns, kuid_t uid, enum ucount_type type);
void dec_ucount(struct ucounts *ucounts, enum ucount_type type);

#ifdef CONFIG_USER_NS

static inline struct user_namespace *get_user_ns(struct user_namespace *ns)
{
	if (ns)
		refcount_inc(&ns->ns.count);
	return ns;
}

extern int create_user_ns(struct cred *new);
extern int unshare_userns(unsigned long unshare_flags, struct cred **new_cred);
extern void __put_user_ns(struct user_namespace *ns);

static inline void put_user_ns(struct user_namespace *ns)
{
	if (ns && refcount_dec_and_test(&ns->ns.count))
		__put_user_ns(ns);
}

struct seq_operations;
extern const struct seq_operations proc_uid_seq_operations;
extern const struct seq_operations proc_gid_seq_operations;
extern const struct seq_operations proc_projid_seq_operations;
extern ssize_t proc_uid_map_write(struct file *, const char __user *, size_t, loff_t *);
extern ssize_t proc_gid_map_write(struct file *, const char __user *, size_t, loff_t *);
extern ssize_t proc_projid_map_write(struct file *, const char __user *, size_t, loff_t *);
extern ssize_t proc_setgroups_write(struct file *, const char __user *, size_t, loff_t *);
extern int proc_setgroups_show(struct seq_file *m, void *v);
extern bool userns_may_setgroups(const struct user_namespace *ns);
extern bool in_userns(const struct user_namespace *ancestor,
		       const struct user_namespace *child);
extern bool current_in_userns(const struct user_namespace *target_ns);
struct ns_common *ns_get_owner(struct ns_common *ns);
#else

static inline struct user_namespace *get_user_ns(struct user_namespace *ns)
{
	return &init_user_ns;
}

static inline int create_user_ns(struct cred *new)
{
	return -EINVAL;
}

static inline int unshare_userns(unsigned long unshare_flags,
				 struct cred **new_cred)
{
	if (unshare_flags & CLONE_NEWUSER)
		return -EINVAL;
	return 0;
}

static inline void put_user_ns(struct user_namespace *ns)
{
}

static inline bool userns_may_setgroups(const struct user_namespace *ns)
{
	return true;
}

static inline bool in_userns(const struct user_namespace *ancestor,
			     const struct user_namespace *child)
{
	return true;
}

static inline bool current_in_userns(const struct user_namespace *target_ns)
{
	return true;
}

static inline struct ns_common *ns_get_owner(struct ns_common *ns)
{
	return ERR_PTR(-EPERM);
}
#endif

#endif /* _LINUX_USER_H */
