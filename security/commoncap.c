// SPDX-License-Identifier: GPL-2.0-or-later
/* Common capabilities, needed by capability.o.
 */

#include <linux/capability.h>
#include <linux/audit.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/lsm_hooks.h>
#include <linux/file.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/pagemap.h>
#include <linux/swap.h>
#include <linux/skbuff.h>
#include <linux/netlink.h>
#include <linux/ptrace.h>
#include <linux/xattr.h>
#include <linux/hugetlb.h>
#include <linux/mount.h>
#include <linux/sched.h>
#include <linux/prctl.h>
#include <linux/securebits.h>
#include <linux/user_namespace.h>
#include <linux/binfmts.h>
#include <linux/personality.h>

/*
 * If a non-root user executes a setuid-root binary in
 * !secure(SECURE_NOROOT) mode, then we raise capabilities.
 * However if fE is also set, then the intent is for only
 * the file capabilities to be applied, and the setuid-root
 * bit is left on either to change the uid (plausible) or
 * to get full privilege on a kernel without file capabilities
 * support.  So in that case we do not raise capabilities.
 *
 * Warn if that happens, once per boot.
 */
static void warn_setuid_and_fcaps_mixed(const char *fname)
{
	static int warned;
	if (!warned) {
		printk(KERN_INFO "warning: `%s' has both setuid-root and"
			" effective capabilities. Therefore not raising all"
			" capabilities.\n", fname);
		warned = 1;
	}
}

/**
 * cap_capable - Determine whether a task has a particular effective capability
 * @cred: The credentials to use
 * @targ_ns:  The user namespace in which we need the capability
 * @cap: The capability to check for
 * @opts: Bitmask of options defined in include/linux/security.h
 *
 * Determine whether the nominated task has the specified capability amongst
 * its effective set, returning 0 if it does, -ve if it does not.
 *
 * NOTE WELL: cap_has_capability() cannot be used like the kernel's capable()
 * and has_capability() functions.  That is, it has the reverse semantics:
 * cap_has_capability() returns 0 when a task has a capability, but the
 * kernel's capable() and has_capability() returns 1 for this case.
 */
int cap_capable(const struct cred *cred, struct user_namespace *targ_ns,
		int cap, unsigned int opts)
{
	struct user_namespace *ns = targ_ns;

	/* See if cred has the capability in the target user namespace
	 * by examining the target user namespace and all of the target
	 * user namespace's parents.
	 */
	for (;;) {
		/* Do we have the necessary capabilities? */
		if (ns == cred->user_ns)
			return cap_raised(cred->cap_effective, cap) ? 0 : -EPERM;

		/*
		 * If we're already at a lower level than we're looking for,
		 * we're done searching.
		 */
		if (ns->level <= cred->user_ns->level)
			return -EPERM;

		/* 
		 * The owner of the user namespace in the parent of the
		 * user namespace has all caps.
		 */
		if ((ns->parent == cred->user_ns) && uid_eq(ns->owner, cred->euid))
			return 0;

		/*
		 * If you have a capability in a parent user ns, then you have
		 * it over all children user namespaces as well.
		 */
		ns = ns->parent;
	}

	/* We never get here */
}

/**
 * cap_settime - Determine whether the current process may set the system clock
 * @ts: The time to set
 * @tz: The timezone to set
 *
 * Determine whether the current process may set the system clock and timezone
 * information, returning 0 if permission granted, -ve if denied.
 */
int cap_settime(const struct timespec64 *ts, const struct timezone *tz)
{
	if (!capable(CAP_SYS_TIME))
		return -EPERM;
	return 0;
}

/**
 * cap_ptrace_access_check - Determine whether the current process may access
 *			   another
 * @child: The process to be accessed
 * @mode: The mode of attachment.
 *
 * If we are in the same or an ancestor user_ns and have all the target
 * task's capabilities, then ptrace access is allowed.
 * If we have the ptrace capability to the target user_ns, then ptrace
 * access is allowed.
 * Else denied.
 *
 * Determine whether a process may access another, returning 0 if permission
 * granted, -ve if denied.
 */
int cap_ptrace_access_check(struct task_struct *child, unsigned int mode)
{
	int ret = 0;
	const struct cred *cred, *child_cred;
	const kernel_cap_t *caller_caps;

	rcu_read_lock();
	cred = current_cred();
	child_cred = __task_cred(child);
	if (mode & PTRACE_MODE_FSCREDS)
		caller_caps = &cred->cap_effective;
	else
		caller_caps = &cred->cap_permitted;
	if (cred->user_ns == child_cred->user_ns &&
	    cap_issubset(child_cred->cap_permitted, *caller_caps))
		goto out;
	if (ns_capable(child_cred->user_ns, CAP_SYS_PTRACE))
		goto out;
	ret = -EPERM;
out:
	rcu_read_unlock();
	return ret;
}

/**
 * cap_ptrace_traceme - Determine whether another process may trace the current
 * @parent: The task proposed to be the tracer
 *
 * If parent is in the same or an ancestor user_ns and has all current's
 * capabilities, then ptrace access is allowed.
 * If parent has the ptrace capability to current's user_ns, then ptrace
 * access is allowed.
 * Else denied.
 *
 * Determine whether the nominated task is permitted to trace the current
 * process, returning 0 if permission is granted, -ve if denied.
 */
int cap_ptrace_traceme(struct task_struct *parent)
{
	int ret = 0;
	const struct cred *cred, *child_cred;

	rcu_read_lock();
	cred = __task_cred(parent);
	child_cred = current_cred();
	if (cred->user_ns == child_cred->user_ns &&
	    cap_issubset(child_cred->cap_permitted, cred->cap_permitted))
		goto out;
	if (has_ns_capability(parent, child_cred->user_ns, CAP_SYS_PTRACE))
		goto out;
	ret = -EPERM;
out:
	rcu_read_unlock();
	return ret;
}

/**
 * cap_capget - Retrieve a task's capability sets
 * @target: The task from which to retrieve the capability sets
 * @effective: The place to record the effective set
 * @inheritable: The place to record the inheritable set
 * @permitted: The place to record the permitted set
 *
 * This function retrieves the capabilities of the nominated task and returns
 * them to the caller.
 */
int cap_capget(struct task_struct *target, kernel_cap_t *effective,
	       kernel_cap_t *inheritable, kernel_cap_t *permitted)
{
	const struct cred *cred;

	/* Derived from kernel/capability.c:sys_capget. */
	rcu_read_lock();
	cred = __task_cred(target);
	*effective   = cred->cap_effective;
	*inheritable = cred->cap_inheritable;
	*permitted   = cred->cap_permitted;
	rcu_read_unlock();
	return 0;
}

/*
 * Determine whether the inheritable capabilities are limited to the old
 * permitted set.  Returns 1 if they are limited, 0 if they are not.
 */
static inline int cap_inh_is_capped(void)
{
	/* they are so limited unless the current task has the CAP_SETPCAP
	 * capability
	 */
	if (cap_capable(current_cred(), current_cred()->user_ns,
			CAP_SETPCAP, CAP_OPT_NONE) == 0)
		return 0;
	return 1;
}

/**
 * cap_capset - Validate and apply proposed changes to current's capabilities
 * @new: The proposed new credentials; alterations should be made here
 * @old: The current task's current credentials
 * @effective: A pointer to the proposed new effective capabilities set
 * @inheritable: A pointer to the proposed new inheritable capabilities set
 * @permitted: A pointer to the proposed new permitted capabilities set
 *
 * This function validates and applies a proposed mass change to the current
 * process's capability sets.  The changes are made to the proposed new
 * credentials, and assuming no error, will be committed by the caller of LSM.
 */
int cap_capset(struct cred *new,
	       const struct cred *old,
	       const kernel_cap_t *effective,
	       const kernel_cap_t *inheritable,
	       const kernel_cap_t *permitted)
{
	if (cap_inh_is_capped() &&
	    !cap_issubset(*inheritable,
			  cap_combine(old->cap_inheritable,
				      old->cap_permitted)))
		/* incapable of using this inheritable set */
		return -EPERM;

	if (!cap_issubset(*inheritable,
			  cap_combine(old->cap_inheritable,
				      old->cap_bset)))
		/* no new pI capabilities outside bounding set */
		return -EPERM;

	/* verify restrictions on target's new Permitted set */
	if (!cap_issubset(*permitted, old->cap_permitted))
		return -EPERM;

	/* verify the _new_Effective_ is a subset of the _new_Permitted_ */
	if (!cap_issubset(*effective, *permitted))
		return -EPERM;

	new->cap_effective   = *effective;
	new->cap_inheritable = *inheritable;
	new->cap_permitted   = *permitted;

	/*
	 * Mask off ambient bits that are no longer both permitted and
	 * inheritable.
	 */
	new->cap_ambient = cap_intersect(new->cap_ambient,
					 cap_intersect(*permitted,
						       *inheritable));
	if (WARN_ON(!cap_ambient_invariant_ok(new)))
		return -EINVAL;
	return 0;
}

/**
 * cap_inode_need_killpriv - Determine if inode change affects privileges
 * @dentry: The inode/dentry in being changed with change marked ATTR_KILL_PRIV
 *
 * Determine if an inode having a change applied that's marked ATTR_KILL_PRIV
 * affects the security markings on that inode, and if it is, should
 * inode_killpriv() be invoked or the change rejected.
 *
 * Return: 1 if security.capability has a value, meaning inode_killpriv()
 * is required, 0 otherwise, meaning inode_killpriv() is not required.
 */
int cap_inode_need_killpriv(struct dentry *dentry)
{
	struct inode *inode = d_backing_inode(dentry);
	int error;

	error = __vfs_getxattr(dentry, inode, XATTR_NAME_CAPS, NULL, 0);
	return error > 0;
}

/**
 * cap_inode_killpriv - Erase the security markings on an inode
 *
 * @mnt_userns:	user namespace of the mount the inode was found from
 * @dentry:	The inode/dentry to alter
 *
 * Erase the privilege-enhancing security markings on an inode.
 *
 * If the inode has been found through an idmapped mount the user namespace of
 * the vfsmount must be passed through @mnt_userns. This function will then
 * take care to map the inode according to @mnt_userns before checking
 * permissions. On non-idmapped mounts or if permission checking is to be
 * performed on the raw inode simply passs init_user_ns.
 *
 * Return: 0 if successful, -ve on error.
 */
int cap_inode_killpriv(struct user_namespace *mnt_userns, struct dentry *dentry)
{
	int error;

	error = __vfs_removexattr(mnt_userns, dentry, XATTR_NAME_CAPS);
	if (error == -EOPNOTSUPP)
		error = 0;
	return error;
}

static bool rootid_owns_currentns(kuid_t kroot)
{
	struct user_namespace *ns;

	if (!uid_valid(kroot))
		return false;

	for (ns = current_user_ns(); ; ns = ns->parent) {
		if (from_kuid(ns, kroot) == 0)
			return true;
		if (ns == &init_user_ns)
			break;
	}

	return false;
}

static __u32 sansflags(__u32 m)
{
	return m & ~VFS_CAP_FLAGS_EFFECTIVE;
}

static bool is_v2header(size_t size, const struct vfs_cap_data *cap)
{
	if (size != XATTR_CAPS_SZ_2)
		return false;
	return sansflags(le32_to_cpu(cap->magic_etc)) == VFS_CAP_REVISION_2;
}

static bool is_v3header(size_t size, const struct vfs_cap_data *cap)
{
	if (size != XATTR_CAPS_SZ_3)
		return false;
	return sansflags(le32_to_cpu(cap->magic_etc)) == VFS_CAP_REVISION_3;
}

/*
 * getsecurity: We are called for security.* before any attempt to read the
 * xattr from the inode itself.
 *
 * This gives us a chance to read the on-disk value and convert it.  If we
 * return -EOPNOTSUPP, then vfs_getxattr() will call the i_op handler.
 *
 * Note we are not called by vfs_getxattr_alloc(), but that is only called
 * by the integrity subsystem, which really wants the unconverted values -
 * so that's good.
 */
int cap_inode_getsecurity(struct user_namespace *mnt_userns,
			  struct inode *inode, const char *name, void **buffer,
			  bool alloc)
{
	int size, ret;
	kuid_t kroot;
	u32 nsmagic, magic;
	uid_t root, mappedroot;
	char *tmpbuf = NULL;
	struct vfs_cap_data *cap;
	struct vfs_ns_cap_data *nscap = NULL;
	struct dentry *dentry;
	struct user_namespace *fs_ns;

	if (strcmp(name, "capability") != 0)
		return -EOPNOTSUPP;

	dentry = d_find_any_alias(inode);
	if (!dentry)
		return -EINVAL;

	size = sizeof(struct vfs_ns_cap_data);
	ret = (int)vfs_getxattr_alloc(mnt_userns, dentry, XATTR_NAME_CAPS,
				      &tmpbuf, size, GFP_NOFS);
	dput(dentry);

	if (ret < 0 || !tmpbuf)
		return ret;

	fs_ns = inode->i_sb->s_user_ns;
	cap = (struct vfs_cap_data *) tmpbuf;
	if (is_v2header((size_t) ret, cap)) {
		root = 0;
	} else if (is_v3header((size_t) ret, cap)) {
		nscap = (struct vfs_ns_cap_data *) tmpbuf;
		root = le32_to_cpu(nscap->rootid);
	} else {
		size = -EINVAL;
		goto out_free;
	}

	kroot = make_kuid(fs_ns, root);

	/* If this is an idmapped mount shift the kuid. */
	kroot = kuid_into_mnt(mnt_userns, kroot);

	/* If the root kuid maps to a valid uid in current ns, then return
	 * this as a nscap. */
	mappedroot = from_kuid(current_user_ns(), kroot);
	if (mappedroot != (uid_t)-1 && mappedroot != (uid_t)0) {
		size = sizeof(struct vfs_ns_cap_data);
		if (alloc) {
			if (!nscap) {
				/* v2 -> v3 conversion */
				nscap = kzalloc(size, GFP_ATOMIC);
				if (!nscap) {
					size = -ENOMEM;
					goto out_free;
				}
				nsmagic = VFS_CAP_REVISION_3;
				magic = le32_to_cpu(cap->magic_etc);
				if (magic & VFS_CAP_FLAGS_EFFECTIVE)
					nsmagic |= VFS_CAP_FLAGS_EFFECTIVE;
				memcpy(&nscap->data, &cap->data, sizeof(__le32) * 2 * VFS_CAP_U32);
				nscap->magic_etc = cpu_to_le32(nsmagic);
			} else {
				/* use allocated v3 buffer */
				tmpbuf = NULL;
			}
			nscap->rootid = cpu_to_le32(mappedroot);
			*buffer = nscap;
		}
		goto out_free;
	}

	if (!rootid_owns_currentns(kroot)) {
		size = -EOVERFLOW;
		goto out_free;
	}

	/* This comes from a parent namespace.  Return as a v2 capability */
	size = sizeof(struct vfs_cap_data);
	if (alloc) {
		if (nscap) {
			/* v3 -> v2 conversion */
			cap = kzalloc(size, GFP_ATOMIC);
			if (!cap) {
				size = -ENOMEM;
				goto out_free;
			}
			magic = VFS_CAP_REVISION_2;
			nsmagic = le32_to_cpu(nscap->magic_etc);
			if (nsmagic & VFS_CAP_FLAGS_EFFECTIVE)
				magic |= VFS_CAP_FLAGS_EFFECTIVE;
			memcpy(&cap->data, &nscap->data, sizeof(__le32) * 2 * VFS_CAP_U32);
			cap->magic_etc = cpu_to_le32(magic);
		} else {
			/* use unconverted v2 */
			tmpbuf = NULL;
		}
		*buffer = cap;
	}
out_free:
	kfree(tmpbuf);
	return size;
}

/**
 * rootid_from_xattr - translate root uid of vfs caps
 *
 * @value:	vfs caps value which may be modified by this function
 * @size:	size of @ivalue
 * @task_ns:	user namespace of the caller
 * @mnt_userns:	user namespace of the mount the inode was found from
 *
 * If the inode has been found through an idmapped mount the user namespace of
 * the vfsmount must be passed through @mnt_userns. This function will then
 * take care to map the inode according to @mnt_userns before checking
 * permissions. On non-idmapped mounts or if permission checking is to be
 * performed on the raw inode simply passs init_user_ns.
 */
static kuid_t rootid_from_xattr(const void *value, size_t size,
				struct user_namespace *task_ns,
				struct user_namespace *mnt_userns)
{
	const struct vfs_ns_cap_data *nscap = value;
	kuid_t rootkid;
	uid_t rootid = 0;

	if (size == XATTR_CAPS_SZ_3)
		rootid = le32_to_cpu(nscap->rootid);

	rootkid = make_kuid(task_ns, rootid);
	return kuid_from_mnt(mnt_userns, rootkid);
}

static bool validheader(size_t size, const struct vfs_cap_data *cap)
{
	return is_v2header(size, cap) || is_v3header(size, cap);
}

/**
 * cap_convert_nscap - check vfs caps
 *
 * @mnt_userns:	user namespace of the mount the inode was found from
 * @dentry:	used to retrieve inode to check permissions on
 * @ivalue:	vfs caps value which may be modified by this function
 * @size:	size of @ivalue
 *
 * User requested a write of security.capability.  If needed, update the
 * xattr to change from v2 to v3, or to fixup the v3 rootid.
 *
 * If the inode has been found through an idmapped mount the user namespace of
 * the vfsmount must be passed through @mnt_userns. This function will then
 * take care to map the inode according to @mnt_userns before checking
 * permissions. On non-idmapped mounts or if permission checking is to be
 * performed on the raw inode simply passs init_user_ns.
 *
 * Return: On success, return the new size; on error, return < 0.
 */
int cap_convert_nscap(struct user_namespace *mnt_userns, struct dentry *dentry,
		      const void **ivalue, size_t size)
{
	struct vfs_ns_cap_data *nscap;
	uid_t nsrootid;
	const struct vfs_cap_data *cap = *ivalue;
	__u32 magic, nsmagic;
	struct inode *inode = d_backing_inode(dentry);
	struct user_namespace *task_ns = current_user_ns(),
		*fs_ns = inode->i_sb->s_user_ns;
	kuid_t rootid;
	size_t newsize;

	if (!*ivalue)
		return -EINVAL;
	if (!validheader(size, cap))
		return -EINVAL;
	if (!capable_wrt_inode_uidgid(mnt_userns, inode, CAP_SETFCAP))
		return -EPERM;
	if (size == XATTR_CAPS_SZ_2 && (mnt_userns == &init_user_ns))
		if (ns_capable(inode->i_sb->s_user_ns, CAP_SETFCAP))
			/* user is privileged, just write the v2 */
			return size;

	rootid = rootid_from_xattr(*ivalue, size, task_ns, mnt_userns);
	if (!uid_valid(rootid))
		return -EINVAL;

	nsrootid = from_kuid(fs_ns, rootid);
	if (nsrootid == -1)
		return -EINVAL;

	newsize = sizeof(struct vfs_ns_cap_data);
	nscap = kmalloc(newsize, GFP_ATOMIC);
	if (!nscap)
		return -ENOMEM;
	nscap->rootid = cpu_to_le32(nsrootid);
	nsmagic = VFS_CAP_REVISION_3;
	magic = le32_to_cpu(cap->magic_etc);
	if (magic & VFS_CAP_FLAGS_EFFECTIVE)
		nsmagic |= VFS_CAP_FLAGS_EFFECTIVE;
	nscap->magic_etc = cpu_to_le32(nsmagic);
	memcpy(&nscap->data, &cap->data, sizeof(__le32) * 2 * VFS_CAP_U32);

	*ivalue = nscap;
	return newsize;
}

/*
 * Calculate the new process capability sets from the capability sets attached
 * to a file.
 */
static inline int bprm_caps_from_vfs_caps(struct cpu_vfs_cap_data *caps,
					  struct linux_binprm *bprm,
					  bool *effective,
					  bool *has_fcap)
{
	struct cred *new = bprm->cred;
	unsigned i;
	int ret = 0;

	if (caps->magic_etc & VFS_CAP_FLAGS_EFFECTIVE)
		*effective = true;

	if (caps->magic_etc & VFS_CAP_REVISION_MASK)
		*has_fcap = true;

	CAP_FOR_EACH_U32(i) {
		__u32 permitted = caps->permitted.cap[i];
		__u32 inheritable = caps->inheritable.cap[i];

		/*
		 * pP' = (X & fP) | (pI & fI)
		 * The addition of pA' is handled later.
		 */
		new->cap_permitted.cap[i] =
			(new->cap_bset.cap[i] & permitted) |
			(new->cap_inheritable.cap[i] & inheritable);

		if (permitted & ~new->cap_permitted.cap[i])
			/* insufficient to execute correctly */
			ret = -EPERM;
	}

	/*
	 * For legacy apps, with no internal support for recognizing they
	 * do not have enough capabilities, we return an error if they are
	 * missing some "forced" (aka file-permitted) capabilities.
	 */
	return *effective ? ret : 0;
}

/**
 * get_vfs_caps_from_disk - retrieve vfs caps from disk
 *
 * @mnt_userns:	user namespace of the mount the inode was found from
 * @dentry:	dentry from which @inode is retrieved
 * @cpu_caps:	vfs capabilities
 *
 * Extract the on-exec-apply capability sets for an executable file.
 *
 * If the inode has been found through an idmapped mount the user namespace of
 * the vfsmount must be passed through @mnt_userns. This function will then
 * take care to map the inode according to @mnt_userns before checking
 * permissions. On non-idmapped mounts or if permission checking is to be
 * performed on the raw inode simply passs init_user_ns.
 */
int get_vfs_caps_from_disk(struct user_namespace *mnt_userns,
			   const struct dentry *dentry,
			   struct cpu_vfs_cap_data *cpu_caps)
{
	struct inode *inode = d_backing_inode(dentry);
	__u32 magic_etc;
	unsigned tocopy, i;
	int size;
	struct vfs_ns_cap_data data, *nscaps = &data;
	struct vfs_cap_data *caps = (struct vfs_cap_data *) &data;
	kuid_t rootkuid;
	struct user_namespace *fs_ns;

	memset(cpu_caps, 0, sizeof(struct cpu_vfs_cap_data));

	if (!inode)
		return -ENODATA;

	fs_ns = inode->i_sb->s_user_ns;
	size = __vfs_getxattr((struct dentry *)dentry, inode,
			      XATTR_NAME_CAPS, &data, XATTR_CAPS_SZ);
	if (size == -ENODATA || size == -EOPNOTSUPP)
		/* no data, that's ok */
		return -ENODATA;

	if (size < 0)
		return size;

	if (size < sizeof(magic_etc))
		return -EINVAL;

	cpu_caps->magic_etc = magic_etc = le32_to_cpu(caps->magic_etc);

	rootkuid = make_kuid(fs_ns, 0);
	switch (magic_etc & VFS_CAP_REVISION_MASK) {
	case VFS_CAP_REVISION_1:
		if (size != XATTR_CAPS_SZ_1)
			return -EINVAL;
		tocopy = VFS_CAP_U32_1;
		break;
	case VFS_CAP_REVISION_2:
		if (size != XATTR_CAPS_SZ_2)
			return -EINVAL;
		tocopy = VFS_CAP_U32_2;
		break;
	case VFS_CAP_REVISION_3:
		if (size != XATTR_CAPS_SZ_3)
			return -EINVAL;
		tocopy = VFS_CAP_U32_3;
		rootkuid = make_kuid(fs_ns, le32_to_cpu(nscaps->rootid));
		break;

	default:
		return -EINVAL;
	}
	/* Limit the caps to the mounter of the filesystem
	 * or the more limited uid specified in the xattr.
	 */
	rootkuid = kuid_into_mnt(mnt_userns, rootkuid);
	if (!rootid_owns_currentns(rootkuid))
		return -ENODATA;

	CAP_FOR_EACH_U32(i) {
		if (i >= tocopy)
			break;
		cpu_caps->permitted.cap[i] = le32_to_cpu(caps->data[i].permitted);
		cpu_caps->inheritable.cap[i] = le32_to_cpu(caps->data[i].inheritable);
	}

	cpu_caps->permitted.cap[CAP_LAST_U32] &= CAP_LAST_U32_VALID_MASK;
	cpu_caps->inheritable.cap[CAP_LAST_U32] &= CAP_LAST_U32_VALID_MASK;

	cpu_caps->rootid = rootkuid;

	return 0;
}

/*
 * Attempt to get the on-exec apply capability sets for an executable file from
 * its xattrs and, if present, apply them to the proposed credentials being
 * constructed by execve().
 */
static int get_file_caps(struct linux_binprm *bprm, struct file *file,
			 bool *effective, bool *has_fcap)
{
	int rc = 0;
	struct cpu_vfs_cap_data vcaps;

	cap_clear(bprm->cred->cap_permitted);

	if (!file_caps_enabled)
		return 0;

	if (!mnt_may_suid(file->f_path.mnt))
		return 0;

	/*
	 * This check is redundant with mnt_may_suid() but is kept to make
	 * explicit that capability bits are limited to s_user_ns and its
	 * descendants.
	 */
	if (!current_in_userns(file->f_path.mnt->mnt_sb->s_user_ns))
		return 0;

	rc = get_vfs_caps_from_disk(file_mnt_user_ns(file),
				    file->f_path.dentry, &vcaps);
	if (rc < 0) {
		if (rc == -EINVAL)
			printk(KERN_NOTICE "Invalid argument reading file caps for %s\n",
					bprm->filename);
		else if (rc == -ENODATA)
			rc = 0;
		goto out;
	}

	rc = bprm_caps_from_vfs_caps(&vcaps, bprm, effective, has_fcap);

out:
	if (rc)
		cap_clear(bprm->cred->cap_permitted);

	return rc;
}

static inline bool root_privileged(void) { return !issecure(SECURE_NOROOT); }

static inline bool __is_real(kuid_t uid, struct cred *cred)
{ return uid_eq(cred->uid, uid); }

static inline bool __is_eff(kuid_t uid, struct cred *cred)
{ return uid_eq(cred->euid, uid); }

static inline bool __is_suid(kuid_t uid, struct cred *cred)
{ return !__is_real(uid, cred) && __is_eff(uid, cred); }

/*
 * handle_privileged_root - Handle case of privileged root
 * @bprm: The execution parameters, including the proposed creds
 * @has_fcap: Are any file capabilities set?
 * @effective: Do we have effective root privilege?
 * @root_uid: This namespace' root UID WRT initial USER namespace
 *
 * Handle the case where root is privileged and hasn't been neutered by
 * SECURE_NOROOT.  If file capabilities are set, they won't be combined with
 * set UID root and nothing is changed.  If we are root, cap_permitted is
 * updated.  If we have become set UID root, the effective bit is set.
 */
static void handle_privileged_root(struct linux_binprm *bprm, bool has_fcap,
				   bool *effective, kuid_t root_uid)
{
	const struct cred *old = current_cred();
	struct cred *new = bprm->cred;

	if (!root_privileged())
		return;
	/*
	 * If the legacy file capability is set, then don't set privs
	 * for a setuid root binary run by a non-root user.  Do set it
	 * for a root user just to cause least surprise to an admin.
	 */
	if (has_fcap && __is_suid(root_uid, new)) {
		warn_setuid_and_fcaps_mixed(bprm->filename);
		return;
	}
	/*
	 * To support inheritance of root-permissions and suid-root
	 * executables under compatibility mode, we override the
	 * capability sets for the file.
	 */
	if (__is_eff(root_uid, new) || __is_real(root_uid, new)) {
		/* pP' = (cap_bset & ~0) | (pI & ~0) */
		new->cap_permitted = cap_combine(old->cap_bset,
						 old->cap_inheritable);
	}
	/*
	 * If only the real uid is 0, we do not set the effective bit.
	 */
	if (__is_eff(root_uid, new))
		*effective = true;
}

#define __cap_gained(field, target, source) \
	!cap_issubset(target->cap_##field, source->cap_##field)
#define __cap_grew(target, source, cred) \
	!cap_issubset(cred->cap_##target, cred->cap_##source)
#define __cap_full(field, cred) \
	cap_issubset(CAP_FULL_SET, cred->cap_##field)

static inline bool __is_setuid(struct cred *new, const struct cred *old)
{ return !uid_eq(new->euid, old->uid); }

static inline bool __is_setgid(struct cred *new, const struct cred *old)
{ return !gid_eq(new->egid, old->gid); }

/*
 * 1) Audit candidate if current->cap_effective is set
 *
 * We do not bother to audit if 3 things are true:
 *   1) cap_effective has all caps
 *   2) we became root *OR* are were already root
 *   3) root is supposed to have all caps (SECURE_NOROOT)
 * Since this is just a normal root execing a process.
 *
 * Number 1 above might fail if you don't have a full bset, but I think
 * that is interesting information to audit.
 *
 * A number of other conditions require logging:
 * 2) something prevented setuid root getting all caps
 * 3) non-setuid root gets fcaps
 * 4) non-setuid root gets ambient
 */
static inline bool nonroot_raised_pE(struct cred *new, const struct cred *old,
				     kuid_t root, bool has_fcap)
{
	bool ret = false;

	if ((__cap_grew(effective, ambient, new) &&
	     !(__cap_full(effective, new) &&
	       (__is_eff(root, new) || __is_real(root, new)) &&
	       root_privileged())) ||
	    (root_privileged() &&
	     __is_suid(root, new) &&
	     !__cap_full(effective, new)) ||
	    (!__is_setuid(new, old) &&
	     ((has_fcap &&
	       __cap_gained(permitted, new, old)) ||
	      __cap_gained(ambient, new, old))))

		ret = true;

	return ret;
}

/**
 * cap_bprm_creds_from_file - Set up the proposed credentials for execve().
 * @bprm: The execution parameters, including the proposed creds
 * @file: The file to pull the credentials from
 *
 * Set up the proposed credentials for a new execution context being
 * constructed by execve().  The proposed creds in @bprm->cred is altered,
 * which won't take effect immediately.
 *
 * Return: 0 if successful, -ve on error.
 */
int cap_bprm_creds_from_file(struct linux_binprm *bprm, struct file *file)
{
	/* Process setpcap binaries and capabilities for uid 0 */
	const struct cred *old = current_cred();
	struct cred *new = bprm->cred;
	bool effective = false, has_fcap = false, is_setid;
	int ret;
	kuid_t root_uid;

	if (WARN_ON(!cap_ambient_invariant_ok(old)))
		return -EPERM;

	ret = get_file_caps(bprm, file, &effective, &has_fcap);
	if (ret < 0)
		return ret;

	root_uid = make_kuid(new->user_ns, 0);

	handle_privileged_root(bprm, has_fcap, &effective, root_uid);

	/* if we have fs caps, clear dangerous personality flags */
	if (__cap_gained(permitted, new, old))
		bprm->per_clear |= PER_CLEAR_ON_SETID;

	/* Don't let someone trace a set[ug]id/setpcap binary with the revised
	 * credentials unless they have the appropriate permit.
	 *
	 * In addition, if NO_NEW_PRIVS, then ensure we get no new privs.
	 */
	is_setid = __is_setuid(new, old) || __is_setgid(new, old);

	if ((is_setid || __cap_gained(permitted, new, old)) &&
	    ((bprm->unsafe & ~LSM_UNSAFE_PTRACE) ||
	     !ptracer_capable(current, new->user_ns))) {
		/* downgrade; they get no more than they had, and maybe less */
		if (!ns_capable(new->user_ns, CAP_SETUID) ||
		    (bprm->unsafe & LSM_UNSAFE_NO_NEW_PRIVS)) {
			new->euid = new->uid;
			new->egid = new->gid;
		}
		new->cap_permitted = cap_intersect(new->cap_permitted,
						   old->cap_permitted);
	}

	new->suid = new->fsuid = new->euid;
	new->sgid = new->fsgid = new->egid;

	/* File caps or setid cancels ambient. */
	if (has_fcap || is_setid)
		cap_clear(new->cap_ambient);

	/*
	 * Now that we've computed pA', update pP' to give:
	 *   pP' = (X & fP) | (pI & fI) | pA'
	 */
	new->cap_permitted = cap_combine(new->cap_permitted, new->cap_ambient);

	/*
	 * Set pE' = (fE ? pP' : pA').  Because pA' is zero if fE is set,
	 * this is the same as pE' = (fE ? pP' : 0) | pA'.
	 */
	if (effective)
		new->cap_effective = new->cap_permitted;
	else
		new->cap_effective = new->cap_ambient;

	if (WARN_ON(!cap_ambient_invariant_ok(new)))
		return -EPERM;

	if (nonroot_raised_pE(new, old, root_uid, has_fcap)) {
		ret = audit_log_bprm_fcaps(bprm, new, old);
		if (ret < 0)
			return ret;
	}

	new->securebits &= ~issecure_mask(SECURE_KEEP_CAPS);

	if (WARN_ON(!cap_ambient_invariant_ok(new)))
		return -EPERM;

	/* Check for privilege-elevated exec. */
	if (is_setid ||
	    (!__is_real(root_uid, new) &&
	     (effective ||
	      __cap_grew(permitted, ambient, new))))
		bprm->secureexec = 1;

	return 0;
}

/**
 * cap_inode_setxattr - Determine whether an xattr may be altered
 * @dentry: The inode/dentry being altered
 * @name: The name of the xattr to be changed
 * @value: The value that the xattr will be changed to
 * @size: The size of value
 * @flags: The replacement flag
 *
 * Determine whether an xattr may be altered or set on an inode, returning 0 if
 * permission is granted, -ve if denied.
 *
 * This is used to make sure security xattrs don't get updated or set by those
 * who aren't privileged to do so.
 */
int cap_inode_setxattr(struct dentry *dentry, const char *name,
		       const void *value, size_t size, int flags)
{
	struct user_namespace *user_ns = dentry->d_sb->s_user_ns;

	/* Ignore non-security xattrs */
	if (strncmp(name, XATTR_SECURITY_PREFIX,
			XATTR_SECURITY_PREFIX_LEN) != 0)
		return 0;

	/*
	 * For XATTR_NAME_CAPS the check will be done in
	 * cap_convert_nscap(), called by setxattr()
	 */
	if (strcmp(name, XATTR_NAME_CAPS) == 0)
		return 0;

	if (!ns_capable(user_ns, CAP_SYS_ADMIN))
		return -EPERM;
	return 0;
}

/**
 * cap_inode_removexattr - Determine whether an xattr may be removed
 *
 * @mnt_userns:	User namespace of the mount the inode was found from
 * @dentry:	The inode/dentry being altered
 * @name:	The name of the xattr to be changed
 *
 * Determine whether an xattr may be removed from an inode, returning 0 if
 * permission is granted, -ve if denied.
 *
 * If the inode has been found through an idmapped mount the user namespace of
 * the vfsmount must be passed through @mnt_userns. This function will then
 * take care to map the inode according to @mnt_userns before checking
 * permissions. On non-idmapped mounts or if permission checking is to be
 * performed on the raw inode simply passs init_user_ns.
 *
 * This is used to make sure security xattrs don't get removed by those who
 * aren't privileged to remove them.
 */
int cap_inode_removexattr(struct user_namespace *mnt_userns,
			  struct dentry *dentry, const char *name)
{
	struct user_namespace *user_ns = dentry->d_sb->s_user_ns;

	/* Ignore non-security xattrs */
	if (strncmp(name, XATTR_SECURITY_PREFIX,
			XATTR_SECURITY_PREFIX_LEN) != 0)
		return 0;

	if (strcmp(name, XATTR_NAME_CAPS) == 0) {
		/* security.capability gets namespaced */
		struct inode *inode = d_backing_inode(dentry);
		if (!inode)
			return -EINVAL;
		if (!capable_wrt_inode_uidgid(mnt_userns, inode, CAP_SETFCAP))
			return -EPERM;
		return 0;
	}

	if (!ns_capable(user_ns, CAP_SYS_ADMIN))
		return -EPERM;
	return 0;
}

/*
 * cap_emulate_setxuid() fixes the effective / permitted capabilities of
 * a process after a call to setuid, setreuid, or setresuid.
 *
 *  1) When set*uiding _from_ one of {r,e,s}uid == 0 _to_ all of
 *  {r,e,s}uid != 0, the permitted and effective capabilities are
 *  cleared.
 *
 *  2) When set*uiding _from_ euid == 0 _to_ euid != 0, the effective
 *  capabilities of the process are cleared.
 *
 *  3) When set*uiding _from_ euid != 0 _to_ euid == 0, the effective
 *  capabilities are set to the permitted capabilities.
 *
 *  fsuid is handled elsewhere. fsuid == 0 and {r,e,s}uid!= 0 should
 *  never happen.
 *
 *  -astor
 *
 * cevans - New behaviour, Oct '99
 * A process may, via prctl(), elect to keep its capabilities when it
 * calls setuid() and switches away from uid==0. Both permitted and
 * effective sets will be retained.
 * Without this change, it was impossible for a daemon to drop only some
 * of its privilege. The call to setuid(!=0) would drop all privileges!
 * Keeping uid 0 is not an option because uid 0 owns too many vital
 * files..
 * Thanks to Olaf Kirch and Peter Benie for spotting this.
 */
static inline void cap_emulate_setxuid(struct cred *new, const struct cred *old)
{
	kuid_t root_uid = make_kuid(old->user_ns, 0);

	if ((uid_eq(old->uid, root_uid) ||
	     uid_eq(old->euid, root_uid) ||
	     uid_eq(old->suid, root_uid)) &&
	    (!uid_eq(new->uid, root_uid) &&
	     !uid_eq(new->euid, root_uid) &&
	     !uid_eq(new->suid, root_uid))) {
		if (!issecure(SECURE_KEEP_CAPS)) {
			cap_clear(new->cap_permitted);
			cap_clear(new->cap_effective);
		}

		/*
		 * Pre-ambient programs expect setresuid to nonroot followed
		 * by exec to drop capabilities.  We should make sure that
		 * this remains the case.
		 */
		cap_clear(new->cap_ambient);
	}
	if (uid_eq(old->euid, root_uid) && !uid_eq(new->euid, root_uid))
		cap_clear(new->cap_effective);
	if (!uid_eq(old->euid, root_uid) && uid_eq(new->euid, root_uid))
		new->cap_effective = new->cap_permitted;
}

/**
 * cap_task_fix_setuid - Fix up the results of setuid() call
 * @new: The proposed credentials
 * @old: The current task's current credentials
 * @flags: Indications of what has changed
 *
 * Fix up the results of setuid() call before the credential changes are
 * actually applied.
 *
 * Return: 0 to grant the changes, -ve to deny them.
 */
int cap_task_fix_setuid(struct cred *new, const struct cred *old, int flags)
{
	switch (flags) {
	case LSM_SETID_RE:
	case LSM_SETID_ID:
	case LSM_SETID_RES:
		/* juggle the capabilities to follow [RES]UID changes unless
		 * otherwise suppressed */
		if (!issecure(SECURE_NO_SETUID_FIXUP))
			cap_emulate_setxuid(new, old);
		break;

	case LSM_SETID_FS:
		/* juggle the capabilties to follow FSUID changes, unless
		 * otherwise suppressed
		 *
		 * FIXME - is fsuser used for all CAP_FS_MASK capabilities?
		 *          if not, we might be a bit too harsh here.
		 */
		if (!issecure(SECURE_NO_SETUID_FIXUP)) {
			kuid_t root_uid = make_kuid(old->user_ns, 0);
			if (uid_eq(old->fsuid, root_uid) && !uid_eq(new->fsuid, root_uid))
				new->cap_effective =
					cap_drop_fs_set(new->cap_effective);

			if (!uid_eq(old->fsuid, root_uid) && uid_eq(new->fsuid, root_uid))
				new->cap_effective =
					cap_raise_fs_set(new->cap_effective,
							 new->cap_permitted);
		}
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

/*
 * Rationale: code calling task_setscheduler, task_setioprio, and
 * task_setnice, assumes that
 *   . if capable(cap_sys_nice), then those actions should be allowed
 *   . if not capable(cap_sys_nice), but acting on your own processes,
 *   	then those actions should be allowed
 * This is insufficient now since you can call code without suid, but
 * yet with increased caps.
 * So we check for increased caps on the target process.
 */
static int cap_safe_nice(struct task_struct *p)
{
	int is_subset, ret = 0;

	rcu_read_lock();
	is_subset = cap_issubset(__task_cred(p)->cap_permitted,
				 current_cred()->cap_permitted);
	if (!is_subset && !ns_capable(__task_cred(p)->user_ns, CAP_SYS_NICE))
		ret = -EPERM;
	rcu_read_unlock();

	return ret;
}

/**
 * cap_task_setscheduler - Detemine if scheduler policy change is permitted
 * @p: The task to affect
 *
 * Detemine if the requested scheduler policy change is permitted for the
 * specified task.
 *
 * Return: 0 if permission is granted, -ve if denied.
 */
int cap_task_setscheduler(struct task_struct *p)
{
	return cap_safe_nice(p);
}

/**
 * cap_task_setioprio - Detemine if I/O priority change is permitted
 * @p: The task to affect
 * @ioprio: The I/O priority to set
 *
 * Detemine if the requested I/O priority change is permitted for the specified
 * task.
 *
 * Return: 0 if permission is granted, -ve if denied.
 */
int cap_task_setioprio(struct task_struct *p, int ioprio)
{
	return cap_safe_nice(p);
}

/**
 * cap_task_setnice - Detemine if task priority change is permitted
 * @p: The task to affect
 * @nice: The nice value to set
 *
 * Detemine if the requested task priority change is permitted for the
 * specified task.
 *
 * Return: 0 if permission is granted, -ve if denied.
 */
int cap_task_setnice(struct task_struct *p, int nice)
{
	return cap_safe_nice(p);
}

/*
 * Implement PR_CAPBSET_DROP.  Attempt to remove the specified capability from
 * the current task's bounding set.  Returns 0 on success, -ve on error.
 */
static int cap_prctl_drop(unsigned long cap)
{
	struct cred *new;

	if (!ns_capable(current_user_ns(), CAP_SETPCAP))
		return -EPERM;
	if (!cap_valid(cap))
		return -EINVAL;

	new = prepare_creds();
	if (!new)
		return -ENOMEM;
	cap_lower(new->cap_bset, cap);
	return commit_creds(new);
}

/**
 * cap_task_prctl - Implement process control functions for this security module
 * @option: The process control function requested
 * @arg2: The argument data for this function
 * @arg3: The argument data for this function
 * @arg4: The argument data for this function
 * @arg5: The argument data for this function
 *
 * Allow process control functions (sys_prctl()) to alter capabilities; may
 * also deny access to other functions not otherwise implemented here.
 *
 * Return: 0 or +ve on success, -ENOSYS if this function is not implemented
 * here, other -ve on error.  If -ENOSYS is returned, sys_prctl() and other LSM
 * modules will consider performing the function.
 */
int cap_task_prctl(int option, unsigned long arg2, unsigned long arg3,
		   unsigned long arg4, unsigned long arg5)
{
	const struct cred *old = current_cred();
	struct cred *new;

	switch (option) {
	case PR_CAPBSET_READ:
		if (!cap_valid(arg2))
			return -EINVAL;
		return !!cap_raised(old->cap_bset, arg2);

	case PR_CAPBSET_DROP:
		return cap_prctl_drop(arg2);

	/*
	 * The next four prctl's remain to assist with transitioning a
	 * system from legacy UID=0 based privilege (when filesystem
	 * capabilities are not in use) to a system using filesystem
	 * capabilities only - as the POSIX.1e draft intended.
	 *
	 * Note:
	 *
	 *  PR_SET_SECUREBITS =
	 *      issecure_mask(SECURE_KEEP_CAPS_LOCKED)
	 *    | issecure_mask(SECURE_NOROOT)
	 *    | issecure_mask(SECURE_NOROOT_LOCKED)
	 *    | issecure_mask(SECURE_NO_SETUID_FIXUP)
	 *    | issecure_mask(SECURE_NO_SETUID_FIXUP_LOCKED)
	 *
	 * will ensure that the current process and all of its
	 * children will be locked into a pure
	 * capability-based-privilege environment.
	 */
	case PR_SET_SECUREBITS:
		if ((((old->securebits & SECURE_ALL_LOCKS) >> 1)
		     & (old->securebits ^ arg2))			/*[1]*/
		    || ((old->securebits & SECURE_ALL_LOCKS & ~arg2))	/*[2]*/
		    || (arg2 & ~(SECURE_ALL_LOCKS | SECURE_ALL_BITS))	/*[3]*/
		    || (cap_capable(current_cred(),
				    current_cred()->user_ns,
				    CAP_SETPCAP,
				    CAP_OPT_NONE) != 0)			/*[4]*/
			/*
			 * [1] no changing of bits that are locked
			 * [2] no unlocking of locks
			 * [3] no setting of unsupported bits
			 * [4] doing anything requires privilege (go read about
			 *     the "sendmail capabilities bug")
			 */
		    )
			/* cannot change a locked bit */
			return -EPERM;

		new = prepare_creds();
		if (!new)
			return -ENOMEM;
		new->securebits = arg2;
		return commit_creds(new);

	case PR_GET_SECUREBITS:
		return old->securebits;

	case PR_GET_KEEPCAPS:
		return !!issecure(SECURE_KEEP_CAPS);

	case PR_SET_KEEPCAPS:
		if (arg2 > 1) /* Note, we rely on arg2 being unsigned here */
			return -EINVAL;
		if (issecure(SECURE_KEEP_CAPS_LOCKED))
			return -EPERM;

		new = prepare_creds();
		if (!new)
			return -ENOMEM;
		if (arg2)
			new->securebits |= issecure_mask(SECURE_KEEP_CAPS);
		else
			new->securebits &= ~issecure_mask(SECURE_KEEP_CAPS);
		return commit_creds(new);

	case PR_CAP_AMBIENT:
		if (arg2 == PR_CAP_AMBIENT_CLEAR_ALL) {
			if (arg3 | arg4 | arg5)
				return -EINVAL;

			new = prepare_creds();
			if (!new)
				return -ENOMEM;
			cap_clear(new->cap_ambient);
			return commit_creds(new);
		}

		if (((!cap_valid(arg3)) | arg4 | arg5))
			return -EINVAL;

		if (arg2 == PR_CAP_AMBIENT_IS_SET) {
			return !!cap_raised(current_cred()->cap_ambient, arg3);
		} else if (arg2 != PR_CAP_AMBIENT_RAISE &&
			   arg2 != PR_CAP_AMBIENT_LOWER) {
			return -EINVAL;
		} else {
			if (arg2 == PR_CAP_AMBIENT_RAISE &&
			    (!cap_raised(current_cred()->cap_permitted, arg3) ||
			     !cap_raised(current_cred()->cap_inheritable,
					 arg3) ||
			     issecure(SECURE_NO_CAP_AMBIENT_RAISE)))
				return -EPERM;

			new = prepare_creds();
			if (!new)
				return -ENOMEM;
			if (arg2 == PR_CAP_AMBIENT_RAISE)
				cap_raise(new->cap_ambient, arg3);
			else
				cap_lower(new->cap_ambient, arg3);
			return commit_creds(new);
		}

	default:
		/* No functionality available - continue with default */
		return -ENOSYS;
	}
}

/**
 * cap_vm_enough_memory - Determine whether a new virtual mapping is permitted
 * @mm: The VM space in which the new mapping is to be made
 * @pages: The size of the mapping
 *
 * Determine whether the allocation of a new virtual mapping by the current
 * task is permitted.
 *
 * Return: 1 if permission is granted, 0 if not.
 */
int cap_vm_enough_memory(struct mm_struct *mm, long pages)
{
	int cap_sys_admin = 0;

	if (cap_capable(current_cred(), &init_user_ns,
				CAP_SYS_ADMIN, CAP_OPT_NOAUDIT) == 0)
		cap_sys_admin = 1;

	return cap_sys_admin;
}

/**
 * cap_mmap_addr - check if able to map given addr
 * @addr: address attempting to be mapped
 *
 * If the process is attempting to map memory below dac_mmap_min_addr they need
 * CAP_SYS_RAWIO.  The other parameters to this function are unused by the
 * capability security module.
 *
 * Return: 0 if this mapping should be allowed or -EPERM if not.
 */
int cap_mmap_addr(unsigned long addr)
{
	int ret = 0;

	if (addr < dac_mmap_min_addr) {
		ret = cap_capable(current_cred(), &init_user_ns, CAP_SYS_RAWIO,
				  CAP_OPT_NONE);
		/* set PF_SUPERPRIV if it turns out we allow the low mmap */
		if (ret == 0)
			current->flags |= PF_SUPERPRIV;
	}
	return ret;
}

int cap_mmap_file(struct file *file, unsigned long reqprot,
		  unsigned long prot, unsigned long flags)
{
	return 0;
}

#ifdef CONFIG_SECURITY

static struct security_hook_list capability_hooks[] __lsm_ro_after_init = {
	LSM_HOOK_INIT(capable, cap_capable),
	LSM_HOOK_INIT(settime, cap_settime),
	LSM_HOOK_INIT(ptrace_access_check, cap_ptrace_access_check),
	LSM_HOOK_INIT(ptrace_traceme, cap_ptrace_traceme),
	LSM_HOOK_INIT(capget, cap_capget),
	LSM_HOOK_INIT(capset, cap_capset),
	LSM_HOOK_INIT(bprm_creds_from_file, cap_bprm_creds_from_file),
	LSM_HOOK_INIT(inode_need_killpriv, cap_inode_need_killpriv),
	LSM_HOOK_INIT(inode_killpriv, cap_inode_killpriv),
	LSM_HOOK_INIT(inode_getsecurity, cap_inode_getsecurity),
	LSM_HOOK_INIT(mmap_addr, cap_mmap_addr),
	LSM_HOOK_INIT(mmap_file, cap_mmap_file),
	LSM_HOOK_INIT(task_fix_setuid, cap_task_fix_setuid),
	LSM_HOOK_INIT(task_prctl, cap_task_prctl),
	LSM_HOOK_INIT(task_setscheduler, cap_task_setscheduler),
	LSM_HOOK_INIT(task_setioprio, cap_task_setioprio),
	LSM_HOOK_INIT(task_setnice, cap_task_setnice),
	LSM_HOOK_INIT(vm_enough_memory, cap_vm_enough_memory),
};

static int __init capability_init(void)
{
	security_add_hooks(capability_hooks, ARRAY_SIZE(capability_hooks),
				"capability");
	return 0;
}

DEFINE_LSM(capability) = {
	.name = "capability",
	.order = LSM_ORDER_FIRST,
	.init = capability_init,
};

#endif /* CONFIG_SECURITY */
