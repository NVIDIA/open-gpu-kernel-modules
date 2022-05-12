// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Simplified MAC Kernel (smack) security module
 *
 *  This file contains the smack hook function implementations.
 *
 *  Authors:
 *	Casey Schaufler <casey@schaufler-ca.com>
 *	Jarkko Sakkinen <jarkko.sakkinen@intel.com>
 *
 *  Copyright (C) 2007 Casey Schaufler <casey@schaufler-ca.com>
 *  Copyright (C) 2009 Hewlett-Packard Development Company, L.P.
 *                Paul Moore <paul@paul-moore.com>
 *  Copyright (C) 2010 Nokia Corporation
 *  Copyright (C) 2011 Intel Corporation.
 */

#include <linux/xattr.h>
#include <linux/pagemap.h>
#include <linux/mount.h>
#include <linux/stat.h>
#include <linux/kd.h>
#include <asm/ioctls.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/dccp.h>
#include <linux/icmpv6.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <net/cipso_ipv4.h>
#include <net/ip.h>
#include <net/ipv6.h>
#include <linux/audit.h>
#include <linux/magic.h>
#include <linux/dcache.h>
#include <linux/personality.h>
#include <linux/msg.h>
#include <linux/shm.h>
#include <linux/binfmts.h>
#include <linux/parser.h>
#include <linux/fs_context.h>
#include <linux/fs_parser.h>
#include <linux/watch_queue.h>
#include "smack.h"

#define TRANS_TRUE	"TRUE"
#define TRANS_TRUE_SIZE	4

#define SMK_CONNECTING	0
#define SMK_RECEIVING	1
#define SMK_SENDING	2

static DEFINE_MUTEX(smack_ipv6_lock);
static LIST_HEAD(smk_ipv6_port_list);
struct kmem_cache *smack_rule_cache;
int smack_enabled;

#define A(s) {"smack"#s, sizeof("smack"#s) - 1, Opt_##s}
static struct {
	const char *name;
	int len;
	int opt;
} smk_mount_opts[] = {
	{"smackfsdef", sizeof("smackfsdef") - 1, Opt_fsdefault},
	A(fsdefault), A(fsfloor), A(fshat), A(fsroot), A(fstransmute)
};
#undef A

static int match_opt_prefix(char *s, int l, char **arg)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(smk_mount_opts); i++) {
		size_t len = smk_mount_opts[i].len;
		if (len > l || memcmp(s, smk_mount_opts[i].name, len))
			continue;
		if (len == l || s[len] != '=')
			continue;
		*arg = s + len + 1;
		return smk_mount_opts[i].opt;
	}
	return Opt_error;
}

#ifdef CONFIG_SECURITY_SMACK_BRINGUP
static char *smk_bu_mess[] = {
	"Bringup Error",	/* Unused */
	"Bringup",		/* SMACK_BRINGUP_ALLOW */
	"Unconfined Subject",	/* SMACK_UNCONFINED_SUBJECT */
	"Unconfined Object",	/* SMACK_UNCONFINED_OBJECT */
};

static void smk_bu_mode(int mode, char *s)
{
	int i = 0;

	if (mode & MAY_READ)
		s[i++] = 'r';
	if (mode & MAY_WRITE)
		s[i++] = 'w';
	if (mode & MAY_EXEC)
		s[i++] = 'x';
	if (mode & MAY_APPEND)
		s[i++] = 'a';
	if (mode & MAY_TRANSMUTE)
		s[i++] = 't';
	if (mode & MAY_LOCK)
		s[i++] = 'l';
	if (i == 0)
		s[i++] = '-';
	s[i] = '\0';
}
#endif

#ifdef CONFIG_SECURITY_SMACK_BRINGUP
static int smk_bu_note(char *note, struct smack_known *sskp,
		       struct smack_known *oskp, int mode, int rc)
{
	char acc[SMK_NUM_ACCESS_TYPE + 1];

	if (rc <= 0)
		return rc;
	if (rc > SMACK_UNCONFINED_OBJECT)
		rc = 0;

	smk_bu_mode(mode, acc);
	pr_info("Smack %s: (%s %s %s) %s\n", smk_bu_mess[rc],
		sskp->smk_known, oskp->smk_known, acc, note);
	return 0;
}
#else
#define smk_bu_note(note, sskp, oskp, mode, RC) (RC)
#endif

#ifdef CONFIG_SECURITY_SMACK_BRINGUP
static int smk_bu_current(char *note, struct smack_known *oskp,
			  int mode, int rc)
{
	struct task_smack *tsp = smack_cred(current_cred());
	char acc[SMK_NUM_ACCESS_TYPE + 1];

	if (rc <= 0)
		return rc;
	if (rc > SMACK_UNCONFINED_OBJECT)
		rc = 0;

	smk_bu_mode(mode, acc);
	pr_info("Smack %s: (%s %s %s) %s %s\n", smk_bu_mess[rc],
		tsp->smk_task->smk_known, oskp->smk_known,
		acc, current->comm, note);
	return 0;
}
#else
#define smk_bu_current(note, oskp, mode, RC) (RC)
#endif

#ifdef CONFIG_SECURITY_SMACK_BRINGUP
static int smk_bu_task(struct task_struct *otp, int mode, int rc)
{
	struct task_smack *tsp = smack_cred(current_cred());
	struct smack_known *smk_task = smk_of_task_struct_obj(otp);
	char acc[SMK_NUM_ACCESS_TYPE + 1];

	if (rc <= 0)
		return rc;
	if (rc > SMACK_UNCONFINED_OBJECT)
		rc = 0;

	smk_bu_mode(mode, acc);
	pr_info("Smack %s: (%s %s %s) %s to %s\n", smk_bu_mess[rc],
		tsp->smk_task->smk_known, smk_task->smk_known, acc,
		current->comm, otp->comm);
	return 0;
}
#else
#define smk_bu_task(otp, mode, RC) (RC)
#endif

#ifdef CONFIG_SECURITY_SMACK_BRINGUP
static int smk_bu_inode(struct inode *inode, int mode, int rc)
{
	struct task_smack *tsp = smack_cred(current_cred());
	struct inode_smack *isp = smack_inode(inode);
	char acc[SMK_NUM_ACCESS_TYPE + 1];

	if (isp->smk_flags & SMK_INODE_IMPURE)
		pr_info("Smack Unconfined Corruption: inode=(%s %ld) %s\n",
			inode->i_sb->s_id, inode->i_ino, current->comm);

	if (rc <= 0)
		return rc;
	if (rc > SMACK_UNCONFINED_OBJECT)
		rc = 0;
	if (rc == SMACK_UNCONFINED_SUBJECT &&
	    (mode & (MAY_WRITE | MAY_APPEND)))
		isp->smk_flags |= SMK_INODE_IMPURE;

	smk_bu_mode(mode, acc);

	pr_info("Smack %s: (%s %s %s) inode=(%s %ld) %s\n", smk_bu_mess[rc],
		tsp->smk_task->smk_known, isp->smk_inode->smk_known, acc,
		inode->i_sb->s_id, inode->i_ino, current->comm);
	return 0;
}
#else
#define smk_bu_inode(inode, mode, RC) (RC)
#endif

#ifdef CONFIG_SECURITY_SMACK_BRINGUP
static int smk_bu_file(struct file *file, int mode, int rc)
{
	struct task_smack *tsp = smack_cred(current_cred());
	struct smack_known *sskp = tsp->smk_task;
	struct inode *inode = file_inode(file);
	struct inode_smack *isp = smack_inode(inode);
	char acc[SMK_NUM_ACCESS_TYPE + 1];

	if (isp->smk_flags & SMK_INODE_IMPURE)
		pr_info("Smack Unconfined Corruption: inode=(%s %ld) %s\n",
			inode->i_sb->s_id, inode->i_ino, current->comm);

	if (rc <= 0)
		return rc;
	if (rc > SMACK_UNCONFINED_OBJECT)
		rc = 0;

	smk_bu_mode(mode, acc);
	pr_info("Smack %s: (%s %s %s) file=(%s %ld %pD) %s\n", smk_bu_mess[rc],
		sskp->smk_known, smk_of_inode(inode)->smk_known, acc,
		inode->i_sb->s_id, inode->i_ino, file,
		current->comm);
	return 0;
}
#else
#define smk_bu_file(file, mode, RC) (RC)
#endif

#ifdef CONFIG_SECURITY_SMACK_BRINGUP
static int smk_bu_credfile(const struct cred *cred, struct file *file,
				int mode, int rc)
{
	struct task_smack *tsp = smack_cred(cred);
	struct smack_known *sskp = tsp->smk_task;
	struct inode *inode = file_inode(file);
	struct inode_smack *isp = smack_inode(inode);
	char acc[SMK_NUM_ACCESS_TYPE + 1];

	if (isp->smk_flags & SMK_INODE_IMPURE)
		pr_info("Smack Unconfined Corruption: inode=(%s %ld) %s\n",
			inode->i_sb->s_id, inode->i_ino, current->comm);

	if (rc <= 0)
		return rc;
	if (rc > SMACK_UNCONFINED_OBJECT)
		rc = 0;

	smk_bu_mode(mode, acc);
	pr_info("Smack %s: (%s %s %s) file=(%s %ld %pD) %s\n", smk_bu_mess[rc],
		sskp->smk_known, smk_of_inode(inode)->smk_known, acc,
		inode->i_sb->s_id, inode->i_ino, file,
		current->comm);
	return 0;
}
#else
#define smk_bu_credfile(cred, file, mode, RC) (RC)
#endif

/**
 * smk_fetch - Fetch the smack label from a file.
 * @name: type of the label (attribute)
 * @ip: a pointer to the inode
 * @dp: a pointer to the dentry
 *
 * Returns a pointer to the master list entry for the Smack label,
 * NULL if there was no label to fetch, or an error code.
 */
static struct smack_known *smk_fetch(const char *name, struct inode *ip,
					struct dentry *dp)
{
	int rc;
	char *buffer;
	struct smack_known *skp = NULL;

	if (!(ip->i_opflags & IOP_XATTR))
		return ERR_PTR(-EOPNOTSUPP);

	buffer = kzalloc(SMK_LONGLABEL, GFP_NOFS);
	if (buffer == NULL)
		return ERR_PTR(-ENOMEM);

	rc = __vfs_getxattr(dp, ip, name, buffer, SMK_LONGLABEL);
	if (rc < 0)
		skp = ERR_PTR(rc);
	else if (rc == 0)
		skp = NULL;
	else
		skp = smk_import_entry(buffer, rc);

	kfree(buffer);

	return skp;
}

/**
 * init_inode_smack - initialize an inode security blob
 * @inode: inode to extract the info from
 * @skp: a pointer to the Smack label entry to use in the blob
 *
 */
static void init_inode_smack(struct inode *inode, struct smack_known *skp)
{
	struct inode_smack *isp = smack_inode(inode);

	isp->smk_inode = skp;
	isp->smk_flags = 0;
}

/**
 * init_task_smack - initialize a task security blob
 * @tsp: blob to initialize
 * @task: a pointer to the Smack label for the running task
 * @forked: a pointer to the Smack label for the forked task
 *
 */
static void init_task_smack(struct task_smack *tsp, struct smack_known *task,
					struct smack_known *forked)
{
	tsp->smk_task = task;
	tsp->smk_forked = forked;
	INIT_LIST_HEAD(&tsp->smk_rules);
	INIT_LIST_HEAD(&tsp->smk_relabel);
	mutex_init(&tsp->smk_rules_lock);
}

/**
 * smk_copy_rules - copy a rule set
 * @nhead: new rules header pointer
 * @ohead: old rules header pointer
 * @gfp: type of the memory for the allocation
 *
 * Returns 0 on success, -ENOMEM on error
 */
static int smk_copy_rules(struct list_head *nhead, struct list_head *ohead,
				gfp_t gfp)
{
	struct smack_rule *nrp;
	struct smack_rule *orp;
	int rc = 0;

	list_for_each_entry_rcu(orp, ohead, list) {
		nrp = kmem_cache_zalloc(smack_rule_cache, gfp);
		if (nrp == NULL) {
			rc = -ENOMEM;
			break;
		}
		*nrp = *orp;
		list_add_rcu(&nrp->list, nhead);
	}
	return rc;
}

/**
 * smk_copy_relabel - copy smk_relabel labels list
 * @nhead: new rules header pointer
 * @ohead: old rules header pointer
 * @gfp: type of the memory for the allocation
 *
 * Returns 0 on success, -ENOMEM on error
 */
static int smk_copy_relabel(struct list_head *nhead, struct list_head *ohead,
				gfp_t gfp)
{
	struct smack_known_list_elem *nklep;
	struct smack_known_list_elem *oklep;

	list_for_each_entry(oklep, ohead, list) {
		nklep = kzalloc(sizeof(struct smack_known_list_elem), gfp);
		if (nklep == NULL) {
			smk_destroy_label_list(nhead);
			return -ENOMEM;
		}
		nklep->smk_label = oklep->smk_label;
		list_add(&nklep->list, nhead);
	}

	return 0;
}

/**
 * smk_ptrace_mode - helper function for converting PTRACE_MODE_* into MAY_*
 * @mode - input mode in form of PTRACE_MODE_*
 *
 * Returns a converted MAY_* mode usable by smack rules
 */
static inline unsigned int smk_ptrace_mode(unsigned int mode)
{
	if (mode & PTRACE_MODE_ATTACH)
		return MAY_READWRITE;
	if (mode & PTRACE_MODE_READ)
		return MAY_READ;

	return 0;
}

/**
 * smk_ptrace_rule_check - helper for ptrace access
 * @tracer: tracer process
 * @tracee_known: label entry of the process that's about to be traced
 * @mode: ptrace attachment mode (PTRACE_MODE_*)
 * @func: name of the function that called us, used for audit
 *
 * Returns 0 on access granted, -error on error
 */
static int smk_ptrace_rule_check(struct task_struct *tracer,
				 struct smack_known *tracee_known,
				 unsigned int mode, const char *func)
{
	int rc;
	struct smk_audit_info ad, *saip = NULL;
	struct task_smack *tsp;
	struct smack_known *tracer_known;
	const struct cred *tracercred;

	if ((mode & PTRACE_MODE_NOAUDIT) == 0) {
		smk_ad_init(&ad, func, LSM_AUDIT_DATA_TASK);
		smk_ad_setfield_u_tsk(&ad, tracer);
		saip = &ad;
	}

	rcu_read_lock();
	tracercred = __task_cred(tracer);
	tsp = smack_cred(tracercred);
	tracer_known = smk_of_task(tsp);

	if ((mode & PTRACE_MODE_ATTACH) &&
	    (smack_ptrace_rule == SMACK_PTRACE_EXACT ||
	     smack_ptrace_rule == SMACK_PTRACE_DRACONIAN)) {
		if (tracer_known->smk_known == tracee_known->smk_known)
			rc = 0;
		else if (smack_ptrace_rule == SMACK_PTRACE_DRACONIAN)
			rc = -EACCES;
		else if (smack_privileged_cred(CAP_SYS_PTRACE, tracercred))
			rc = 0;
		else
			rc = -EACCES;

		if (saip)
			smack_log(tracer_known->smk_known,
				  tracee_known->smk_known,
				  0, rc, saip);

		rcu_read_unlock();
		return rc;
	}

	/* In case of rule==SMACK_PTRACE_DEFAULT or mode==PTRACE_MODE_READ */
	rc = smk_tskacc(tsp, tracee_known, smk_ptrace_mode(mode), saip);

	rcu_read_unlock();
	return rc;
}

/*
 * LSM hooks.
 * We he, that is fun!
 */

/**
 * smack_ptrace_access_check - Smack approval on PTRACE_ATTACH
 * @ctp: child task pointer
 * @mode: ptrace attachment mode (PTRACE_MODE_*)
 *
 * Returns 0 if access is OK, an error code otherwise
 *
 * Do the capability checks.
 */
static int smack_ptrace_access_check(struct task_struct *ctp, unsigned int mode)
{
	struct smack_known *skp;

	skp = smk_of_task_struct_obj(ctp);

	return smk_ptrace_rule_check(current, skp, mode, __func__);
}

/**
 * smack_ptrace_traceme - Smack approval on PTRACE_TRACEME
 * @ptp: parent task pointer
 *
 * Returns 0 if access is OK, an error code otherwise
 *
 * Do the capability checks, and require PTRACE_MODE_ATTACH.
 */
static int smack_ptrace_traceme(struct task_struct *ptp)
{
	int rc;
	struct smack_known *skp;

	skp = smk_of_task(smack_cred(current_cred()));

	rc = smk_ptrace_rule_check(ptp, skp, PTRACE_MODE_ATTACH, __func__);
	return rc;
}

/**
 * smack_syslog - Smack approval on syslog
 * @typefrom_file: unused
 *
 * Returns 0 on success, error code otherwise.
 */
static int smack_syslog(int typefrom_file)
{
	int rc = 0;
	struct smack_known *skp = smk_of_current();

	if (smack_privileged(CAP_MAC_OVERRIDE))
		return 0;

	if (smack_syslog_label != NULL && smack_syslog_label != skp)
		rc = -EACCES;

	return rc;
}

/*
 * Superblock Hooks.
 */

/**
 * smack_sb_alloc_security - allocate a superblock blob
 * @sb: the superblock getting the blob
 *
 * Returns 0 on success or -ENOMEM on error.
 */
static int smack_sb_alloc_security(struct super_block *sb)
{
	struct superblock_smack *sbsp = smack_superblock(sb);

	sbsp->smk_root = &smack_known_floor;
	sbsp->smk_default = &smack_known_floor;
	sbsp->smk_floor = &smack_known_floor;
	sbsp->smk_hat = &smack_known_hat;
	/*
	 * SMK_SB_INITIALIZED will be zero from kzalloc.
	 */

	return 0;
}

struct smack_mnt_opts {
	const char *fsdefault, *fsfloor, *fshat, *fsroot, *fstransmute;
};

static void smack_free_mnt_opts(void *mnt_opts)
{
	struct smack_mnt_opts *opts = mnt_opts;
	kfree(opts->fsdefault);
	kfree(opts->fsfloor);
	kfree(opts->fshat);
	kfree(opts->fsroot);
	kfree(opts->fstransmute);
	kfree(opts);
}

static int smack_add_opt(int token, const char *s, void **mnt_opts)
{
	struct smack_mnt_opts *opts = *mnt_opts;

	if (!opts) {
		opts = kzalloc(sizeof(struct smack_mnt_opts), GFP_KERNEL);
		if (!opts)
			return -ENOMEM;
		*mnt_opts = opts;
	}
	if (!s)
		return -ENOMEM;

	switch (token) {
	case Opt_fsdefault:
		if (opts->fsdefault)
			goto out_opt_err;
		opts->fsdefault = s;
		break;
	case Opt_fsfloor:
		if (opts->fsfloor)
			goto out_opt_err;
		opts->fsfloor = s;
		break;
	case Opt_fshat:
		if (opts->fshat)
			goto out_opt_err;
		opts->fshat = s;
		break;
	case Opt_fsroot:
		if (opts->fsroot)
			goto out_opt_err;
		opts->fsroot = s;
		break;
	case Opt_fstransmute:
		if (opts->fstransmute)
			goto out_opt_err;
		opts->fstransmute = s;
		break;
	}
	return 0;

out_opt_err:
	pr_warn("Smack: duplicate mount options\n");
	return -EINVAL;
}

/**
 * smack_fs_context_dup - Duplicate the security data on fs_context duplication
 * @fc: The new filesystem context.
 * @src_fc: The source filesystem context being duplicated.
 *
 * Returns 0 on success or -ENOMEM on error.
 */
static int smack_fs_context_dup(struct fs_context *fc,
				struct fs_context *src_fc)
{
	struct smack_mnt_opts *dst, *src = src_fc->security;

	if (!src)
		return 0;

	fc->security = kzalloc(sizeof(struct smack_mnt_opts), GFP_KERNEL);
	if (!fc->security)
		return -ENOMEM;
	dst = fc->security;

	if (src->fsdefault) {
		dst->fsdefault = kstrdup(src->fsdefault, GFP_KERNEL);
		if (!dst->fsdefault)
			return -ENOMEM;
	}
	if (src->fsfloor) {
		dst->fsfloor = kstrdup(src->fsfloor, GFP_KERNEL);
		if (!dst->fsfloor)
			return -ENOMEM;
	}
	if (src->fshat) {
		dst->fshat = kstrdup(src->fshat, GFP_KERNEL);
		if (!dst->fshat)
			return -ENOMEM;
	}
	if (src->fsroot) {
		dst->fsroot = kstrdup(src->fsroot, GFP_KERNEL);
		if (!dst->fsroot)
			return -ENOMEM;
	}
	if (src->fstransmute) {
		dst->fstransmute = kstrdup(src->fstransmute, GFP_KERNEL);
		if (!dst->fstransmute)
			return -ENOMEM;
	}
	return 0;
}

static const struct fs_parameter_spec smack_fs_parameters[] = {
	fsparam_string("smackfsdef",		Opt_fsdefault),
	fsparam_string("smackfsdefault",	Opt_fsdefault),
	fsparam_string("smackfsfloor",		Opt_fsfloor),
	fsparam_string("smackfshat",		Opt_fshat),
	fsparam_string("smackfsroot",		Opt_fsroot),
	fsparam_string("smackfstransmute",	Opt_fstransmute),
	{}
};

/**
 * smack_fs_context_parse_param - Parse a single mount parameter
 * @fc: The new filesystem context being constructed.
 * @param: The parameter.
 *
 * Returns 0 on success, -ENOPARAM to pass the parameter on or anything else on
 * error.
 */
static int smack_fs_context_parse_param(struct fs_context *fc,
					struct fs_parameter *param)
{
	struct fs_parse_result result;
	int opt, rc;

	opt = fs_parse(fc, smack_fs_parameters, param, &result);
	if (opt < 0)
		return opt;

	rc = smack_add_opt(opt, param->string, &fc->security);
	if (!rc)
		param->string = NULL;
	return rc;
}

static int smack_sb_eat_lsm_opts(char *options, void **mnt_opts)
{
	char *from = options, *to = options;
	bool first = true;

	while (1) {
		char *next = strchr(from, ',');
		int token, len, rc;
		char *arg = NULL;

		if (next)
			len = next - from;
		else
			len = strlen(from);

		token = match_opt_prefix(from, len, &arg);
		if (token != Opt_error) {
			arg = kmemdup_nul(arg, from + len - arg, GFP_KERNEL);
			rc = smack_add_opt(token, arg, mnt_opts);
			if (unlikely(rc)) {
				kfree(arg);
				if (*mnt_opts)
					smack_free_mnt_opts(*mnt_opts);
				*mnt_opts = NULL;
				return rc;
			}
		} else {
			if (!first) {	// copy with preceding comma
				from--;
				len++;
			}
			if (to != from)
				memmove(to, from, len);
			to += len;
			first = false;
		}
		if (!from[len])
			break;
		from += len + 1;
	}
	*to = '\0';
	return 0;
}

/**
 * smack_set_mnt_opts - set Smack specific mount options
 * @sb: the file system superblock
 * @mnt_opts: Smack mount options
 * @kern_flags: mount option from kernel space or user space
 * @set_kern_flags: where to store converted mount opts
 *
 * Returns 0 on success, an error code on failure
 *
 * Allow filesystems with binary mount data to explicitly set Smack mount
 * labels.
 */
static int smack_set_mnt_opts(struct super_block *sb,
		void *mnt_opts,
		unsigned long kern_flags,
		unsigned long *set_kern_flags)
{
	struct dentry *root = sb->s_root;
	struct inode *inode = d_backing_inode(root);
	struct superblock_smack *sp = smack_superblock(sb);
	struct inode_smack *isp;
	struct smack_known *skp;
	struct smack_mnt_opts *opts = mnt_opts;
	bool transmute = false;

	if (sp->smk_flags & SMK_SB_INITIALIZED)
		return 0;

	if (inode->i_security == NULL) {
		int rc = lsm_inode_alloc(inode);

		if (rc)
			return rc;
	}

	if (!smack_privileged(CAP_MAC_ADMIN)) {
		/*
		 * Unprivileged mounts don't get to specify Smack values.
		 */
		if (opts)
			return -EPERM;
		/*
		 * Unprivileged mounts get root and default from the caller.
		 */
		skp = smk_of_current();
		sp->smk_root = skp;
		sp->smk_default = skp;
		/*
		 * For a handful of fs types with no user-controlled
		 * backing store it's okay to trust security labels
		 * in the filesystem. The rest are untrusted.
		 */
		if (sb->s_user_ns != &init_user_ns &&
		    sb->s_magic != SYSFS_MAGIC && sb->s_magic != TMPFS_MAGIC &&
		    sb->s_magic != RAMFS_MAGIC) {
			transmute = true;
			sp->smk_flags |= SMK_SB_UNTRUSTED;
		}
	}

	sp->smk_flags |= SMK_SB_INITIALIZED;

	if (opts) {
		if (opts->fsdefault) {
			skp = smk_import_entry(opts->fsdefault, 0);
			if (IS_ERR(skp))
				return PTR_ERR(skp);
			sp->smk_default = skp;
		}
		if (opts->fsfloor) {
			skp = smk_import_entry(opts->fsfloor, 0);
			if (IS_ERR(skp))
				return PTR_ERR(skp);
			sp->smk_floor = skp;
		}
		if (opts->fshat) {
			skp = smk_import_entry(opts->fshat, 0);
			if (IS_ERR(skp))
				return PTR_ERR(skp);
			sp->smk_hat = skp;
		}
		if (opts->fsroot) {
			skp = smk_import_entry(opts->fsroot, 0);
			if (IS_ERR(skp))
				return PTR_ERR(skp);
			sp->smk_root = skp;
		}
		if (opts->fstransmute) {
			skp = smk_import_entry(opts->fstransmute, 0);
			if (IS_ERR(skp))
				return PTR_ERR(skp);
			sp->smk_root = skp;
			transmute = true;
		}
	}

	/*
	 * Initialize the root inode.
	 */
	init_inode_smack(inode, sp->smk_root);

	if (transmute) {
		isp = smack_inode(inode);
		isp->smk_flags |= SMK_INODE_TRANSMUTE;
	}

	return 0;
}

/**
 * smack_sb_statfs - Smack check on statfs
 * @dentry: identifies the file system in question
 *
 * Returns 0 if current can read the floor of the filesystem,
 * and error code otherwise
 */
static int smack_sb_statfs(struct dentry *dentry)
{
	struct superblock_smack *sbp = smack_superblock(dentry->d_sb);
	int rc;
	struct smk_audit_info ad;

	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_DENTRY);
	smk_ad_setfield_u_fs_path_dentry(&ad, dentry);

	rc = smk_curacc(sbp->smk_floor, MAY_READ, &ad);
	rc = smk_bu_current("statfs", sbp->smk_floor, MAY_READ, rc);
	return rc;
}

/*
 * BPRM hooks
 */

/**
 * smack_bprm_creds_for_exec - Update bprm->cred if needed for exec
 * @bprm: the exec information
 *
 * Returns 0 if it gets a blob, -EPERM if exec forbidden and -ENOMEM otherwise
 */
static int smack_bprm_creds_for_exec(struct linux_binprm *bprm)
{
	struct inode *inode = file_inode(bprm->file);
	struct task_smack *bsp = smack_cred(bprm->cred);
	struct inode_smack *isp;
	struct superblock_smack *sbsp;
	int rc;

	isp = smack_inode(inode);
	if (isp->smk_task == NULL || isp->smk_task == bsp->smk_task)
		return 0;

	sbsp = smack_superblock(inode->i_sb);
	if ((sbsp->smk_flags & SMK_SB_UNTRUSTED) &&
	    isp->smk_task != sbsp->smk_root)
		return 0;

	if (bprm->unsafe & LSM_UNSAFE_PTRACE) {
		struct task_struct *tracer;
		rc = 0;

		rcu_read_lock();
		tracer = ptrace_parent(current);
		if (likely(tracer != NULL))
			rc = smk_ptrace_rule_check(tracer,
						   isp->smk_task,
						   PTRACE_MODE_ATTACH,
						   __func__);
		rcu_read_unlock();

		if (rc != 0)
			return rc;
	}
	if (bprm->unsafe & ~LSM_UNSAFE_PTRACE)
		return -EPERM;

	bsp->smk_task = isp->smk_task;
	bprm->per_clear |= PER_CLEAR_ON_SETID;

	/* Decide if this is a secure exec. */
	if (bsp->smk_task != bsp->smk_forked)
		bprm->secureexec = 1;

	return 0;
}

/*
 * Inode hooks
 */

/**
 * smack_inode_alloc_security - allocate an inode blob
 * @inode: the inode in need of a blob
 *
 * Returns 0
 */
static int smack_inode_alloc_security(struct inode *inode)
{
	struct smack_known *skp = smk_of_current();

	init_inode_smack(inode, skp);
	return 0;
}

/**
 * smack_inode_init_security - copy out the smack from an inode
 * @inode: the newly created inode
 * @dir: containing directory object
 * @qstr: unused
 * @name: where to put the attribute name
 * @value: where to put the attribute value
 * @len: where to put the length of the attribute
 *
 * Returns 0 if it all works out, -ENOMEM if there's no memory
 */
static int smack_inode_init_security(struct inode *inode, struct inode *dir,
				     const struct qstr *qstr, const char **name,
				     void **value, size_t *len)
{
	struct inode_smack *issp = smack_inode(inode);
	struct smack_known *skp = smk_of_current();
	struct smack_known *isp = smk_of_inode(inode);
	struct smack_known *dsp = smk_of_inode(dir);
	int may;

	if (name)
		*name = XATTR_SMACK_SUFFIX;

	if (value && len) {
		rcu_read_lock();
		may = smk_access_entry(skp->smk_known, dsp->smk_known,
				       &skp->smk_rules);
		rcu_read_unlock();

		/*
		 * If the access rule allows transmutation and
		 * the directory requests transmutation then
		 * by all means transmute.
		 * Mark the inode as changed.
		 */
		if (may > 0 && ((may & MAY_TRANSMUTE) != 0) &&
		    smk_inode_transmutable(dir)) {
			isp = dsp;
			issp->smk_flags |= SMK_INODE_CHANGED;
		}

		*value = kstrdup(isp->smk_known, GFP_NOFS);
		if (*value == NULL)
			return -ENOMEM;

		*len = strlen(isp->smk_known);
	}

	return 0;
}

/**
 * smack_inode_link - Smack check on link
 * @old_dentry: the existing object
 * @dir: unused
 * @new_dentry: the new object
 *
 * Returns 0 if access is permitted, an error code otherwise
 */
static int smack_inode_link(struct dentry *old_dentry, struct inode *dir,
			    struct dentry *new_dentry)
{
	struct smack_known *isp;
	struct smk_audit_info ad;
	int rc;

	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_DENTRY);
	smk_ad_setfield_u_fs_path_dentry(&ad, old_dentry);

	isp = smk_of_inode(d_backing_inode(old_dentry));
	rc = smk_curacc(isp, MAY_WRITE, &ad);
	rc = smk_bu_inode(d_backing_inode(old_dentry), MAY_WRITE, rc);

	if (rc == 0 && d_is_positive(new_dentry)) {
		isp = smk_of_inode(d_backing_inode(new_dentry));
		smk_ad_setfield_u_fs_path_dentry(&ad, new_dentry);
		rc = smk_curacc(isp, MAY_WRITE, &ad);
		rc = smk_bu_inode(d_backing_inode(new_dentry), MAY_WRITE, rc);
	}

	return rc;
}

/**
 * smack_inode_unlink - Smack check on inode deletion
 * @dir: containing directory object
 * @dentry: file to unlink
 *
 * Returns 0 if current can write the containing directory
 * and the object, error code otherwise
 */
static int smack_inode_unlink(struct inode *dir, struct dentry *dentry)
{
	struct inode *ip = d_backing_inode(dentry);
	struct smk_audit_info ad;
	int rc;

	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_DENTRY);
	smk_ad_setfield_u_fs_path_dentry(&ad, dentry);

	/*
	 * You need write access to the thing you're unlinking
	 */
	rc = smk_curacc(smk_of_inode(ip), MAY_WRITE, &ad);
	rc = smk_bu_inode(ip, MAY_WRITE, rc);
	if (rc == 0) {
		/*
		 * You also need write access to the containing directory
		 */
		smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_INODE);
		smk_ad_setfield_u_fs_inode(&ad, dir);
		rc = smk_curacc(smk_of_inode(dir), MAY_WRITE, &ad);
		rc = smk_bu_inode(dir, MAY_WRITE, rc);
	}
	return rc;
}

/**
 * smack_inode_rmdir - Smack check on directory deletion
 * @dir: containing directory object
 * @dentry: directory to unlink
 *
 * Returns 0 if current can write the containing directory
 * and the directory, error code otherwise
 */
static int smack_inode_rmdir(struct inode *dir, struct dentry *dentry)
{
	struct smk_audit_info ad;
	int rc;

	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_DENTRY);
	smk_ad_setfield_u_fs_path_dentry(&ad, dentry);

	/*
	 * You need write access to the thing you're removing
	 */
	rc = smk_curacc(smk_of_inode(d_backing_inode(dentry)), MAY_WRITE, &ad);
	rc = smk_bu_inode(d_backing_inode(dentry), MAY_WRITE, rc);
	if (rc == 0) {
		/*
		 * You also need write access to the containing directory
		 */
		smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_INODE);
		smk_ad_setfield_u_fs_inode(&ad, dir);
		rc = smk_curacc(smk_of_inode(dir), MAY_WRITE, &ad);
		rc = smk_bu_inode(dir, MAY_WRITE, rc);
	}

	return rc;
}

/**
 * smack_inode_rename - Smack check on rename
 * @old_inode: unused
 * @old_dentry: the old object
 * @new_inode: unused
 * @new_dentry: the new object
 *
 * Read and write access is required on both the old and
 * new directories.
 *
 * Returns 0 if access is permitted, an error code otherwise
 */
static int smack_inode_rename(struct inode *old_inode,
			      struct dentry *old_dentry,
			      struct inode *new_inode,
			      struct dentry *new_dentry)
{
	int rc;
	struct smack_known *isp;
	struct smk_audit_info ad;

	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_DENTRY);
	smk_ad_setfield_u_fs_path_dentry(&ad, old_dentry);

	isp = smk_of_inode(d_backing_inode(old_dentry));
	rc = smk_curacc(isp, MAY_READWRITE, &ad);
	rc = smk_bu_inode(d_backing_inode(old_dentry), MAY_READWRITE, rc);

	if (rc == 0 && d_is_positive(new_dentry)) {
		isp = smk_of_inode(d_backing_inode(new_dentry));
		smk_ad_setfield_u_fs_path_dentry(&ad, new_dentry);
		rc = smk_curacc(isp, MAY_READWRITE, &ad);
		rc = smk_bu_inode(d_backing_inode(new_dentry), MAY_READWRITE, rc);
	}
	return rc;
}

/**
 * smack_inode_permission - Smack version of permission()
 * @inode: the inode in question
 * @mask: the access requested
 *
 * This is the important Smack hook.
 *
 * Returns 0 if access is permitted, an error code otherwise
 */
static int smack_inode_permission(struct inode *inode, int mask)
{
	struct superblock_smack *sbsp = smack_superblock(inode->i_sb);
	struct smk_audit_info ad;
	int no_block = mask & MAY_NOT_BLOCK;
	int rc;

	mask &= (MAY_READ|MAY_WRITE|MAY_EXEC|MAY_APPEND);
	/*
	 * No permission to check. Existence test. Yup, it's there.
	 */
	if (mask == 0)
		return 0;

	if (sbsp->smk_flags & SMK_SB_UNTRUSTED) {
		if (smk_of_inode(inode) != sbsp->smk_root)
			return -EACCES;
	}

	/* May be droppable after audit */
	if (no_block)
		return -ECHILD;
	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_INODE);
	smk_ad_setfield_u_fs_inode(&ad, inode);
	rc = smk_curacc(smk_of_inode(inode), mask, &ad);
	rc = smk_bu_inode(inode, mask, rc);
	return rc;
}

/**
 * smack_inode_setattr - Smack check for setting attributes
 * @dentry: the object
 * @iattr: for the force flag
 *
 * Returns 0 if access is permitted, an error code otherwise
 */
static int smack_inode_setattr(struct dentry *dentry, struct iattr *iattr)
{
	struct smk_audit_info ad;
	int rc;

	/*
	 * Need to allow for clearing the setuid bit.
	 */
	if (iattr->ia_valid & ATTR_FORCE)
		return 0;
	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_DENTRY);
	smk_ad_setfield_u_fs_path_dentry(&ad, dentry);

	rc = smk_curacc(smk_of_inode(d_backing_inode(dentry)), MAY_WRITE, &ad);
	rc = smk_bu_inode(d_backing_inode(dentry), MAY_WRITE, rc);
	return rc;
}

/**
 * smack_inode_getattr - Smack check for getting attributes
 * @path: path to extract the info from
 *
 * Returns 0 if access is permitted, an error code otherwise
 */
static int smack_inode_getattr(const struct path *path)
{
	struct smk_audit_info ad;
	struct inode *inode = d_backing_inode(path->dentry);
	int rc;

	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_PATH);
	smk_ad_setfield_u_fs_path(&ad, *path);
	rc = smk_curacc(smk_of_inode(inode), MAY_READ, &ad);
	rc = smk_bu_inode(inode, MAY_READ, rc);
	return rc;
}

/**
 * smack_inode_setxattr - Smack check for setting xattrs
 * @dentry: the object
 * @name: name of the attribute
 * @value: value of the attribute
 * @size: size of the value
 * @flags: unused
 *
 * This protects the Smack attribute explicitly.
 *
 * Returns 0 if access is permitted, an error code otherwise
 */
static int smack_inode_setxattr(struct user_namespace *mnt_userns,
				struct dentry *dentry, const char *name,
				const void *value, size_t size, int flags)
{
	struct smk_audit_info ad;
	struct smack_known *skp;
	int check_priv = 0;
	int check_import = 0;
	int check_star = 0;
	int rc = 0;

	/*
	 * Check label validity here so import won't fail in post_setxattr
	 */
	if (strcmp(name, XATTR_NAME_SMACK) == 0 ||
	    strcmp(name, XATTR_NAME_SMACKIPIN) == 0 ||
	    strcmp(name, XATTR_NAME_SMACKIPOUT) == 0) {
		check_priv = 1;
		check_import = 1;
	} else if (strcmp(name, XATTR_NAME_SMACKEXEC) == 0 ||
		   strcmp(name, XATTR_NAME_SMACKMMAP) == 0) {
		check_priv = 1;
		check_import = 1;
		check_star = 1;
	} else if (strcmp(name, XATTR_NAME_SMACKTRANSMUTE) == 0) {
		check_priv = 1;
		if (size != TRANS_TRUE_SIZE ||
		    strncmp(value, TRANS_TRUE, TRANS_TRUE_SIZE) != 0)
			rc = -EINVAL;
	} else
		rc = cap_inode_setxattr(dentry, name, value, size, flags);

	if (check_priv && !smack_privileged(CAP_MAC_ADMIN))
		rc = -EPERM;

	if (rc == 0 && check_import) {
		skp = size ? smk_import_entry(value, size) : NULL;
		if (IS_ERR(skp))
			rc = PTR_ERR(skp);
		else if (skp == NULL || (check_star &&
		    (skp == &smack_known_star || skp == &smack_known_web)))
			rc = -EINVAL;
	}

	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_DENTRY);
	smk_ad_setfield_u_fs_path_dentry(&ad, dentry);

	if (rc == 0) {
		rc = smk_curacc(smk_of_inode(d_backing_inode(dentry)), MAY_WRITE, &ad);
		rc = smk_bu_inode(d_backing_inode(dentry), MAY_WRITE, rc);
	}

	return rc;
}

/**
 * smack_inode_post_setxattr - Apply the Smack update approved above
 * @dentry: object
 * @name: attribute name
 * @value: attribute value
 * @size: attribute size
 * @flags: unused
 *
 * Set the pointer in the inode blob to the entry found
 * in the master label list.
 */
static void smack_inode_post_setxattr(struct dentry *dentry, const char *name,
				      const void *value, size_t size, int flags)
{
	struct smack_known *skp;
	struct inode_smack *isp = smack_inode(d_backing_inode(dentry));

	if (strcmp(name, XATTR_NAME_SMACKTRANSMUTE) == 0) {
		isp->smk_flags |= SMK_INODE_TRANSMUTE;
		return;
	}

	if (strcmp(name, XATTR_NAME_SMACK) == 0) {
		skp = smk_import_entry(value, size);
		if (!IS_ERR(skp))
			isp->smk_inode = skp;
	} else if (strcmp(name, XATTR_NAME_SMACKEXEC) == 0) {
		skp = smk_import_entry(value, size);
		if (!IS_ERR(skp))
			isp->smk_task = skp;
	} else if (strcmp(name, XATTR_NAME_SMACKMMAP) == 0) {
		skp = smk_import_entry(value, size);
		if (!IS_ERR(skp))
			isp->smk_mmap = skp;
	}

	return;
}

/**
 * smack_inode_getxattr - Smack check on getxattr
 * @dentry: the object
 * @name: unused
 *
 * Returns 0 if access is permitted, an error code otherwise
 */
static int smack_inode_getxattr(struct dentry *dentry, const char *name)
{
	struct smk_audit_info ad;
	int rc;

	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_DENTRY);
	smk_ad_setfield_u_fs_path_dentry(&ad, dentry);

	rc = smk_curacc(smk_of_inode(d_backing_inode(dentry)), MAY_READ, &ad);
	rc = smk_bu_inode(d_backing_inode(dentry), MAY_READ, rc);
	return rc;
}

/**
 * smack_inode_removexattr - Smack check on removexattr
 * @dentry: the object
 * @name: name of the attribute
 *
 * Removing the Smack attribute requires CAP_MAC_ADMIN
 *
 * Returns 0 if access is permitted, an error code otherwise
 */
static int smack_inode_removexattr(struct user_namespace *mnt_userns,
				   struct dentry *dentry, const char *name)
{
	struct inode_smack *isp;
	struct smk_audit_info ad;
	int rc = 0;

	if (strcmp(name, XATTR_NAME_SMACK) == 0 ||
	    strcmp(name, XATTR_NAME_SMACKIPIN) == 0 ||
	    strcmp(name, XATTR_NAME_SMACKIPOUT) == 0 ||
	    strcmp(name, XATTR_NAME_SMACKEXEC) == 0 ||
	    strcmp(name, XATTR_NAME_SMACKTRANSMUTE) == 0 ||
	    strcmp(name, XATTR_NAME_SMACKMMAP) == 0) {
		if (!smack_privileged(CAP_MAC_ADMIN))
			rc = -EPERM;
	} else
		rc = cap_inode_removexattr(mnt_userns, dentry, name);

	if (rc != 0)
		return rc;

	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_DENTRY);
	smk_ad_setfield_u_fs_path_dentry(&ad, dentry);

	rc = smk_curacc(smk_of_inode(d_backing_inode(dentry)), MAY_WRITE, &ad);
	rc = smk_bu_inode(d_backing_inode(dentry), MAY_WRITE, rc);
	if (rc != 0)
		return rc;

	isp = smack_inode(d_backing_inode(dentry));
	/*
	 * Don't do anything special for these.
	 *	XATTR_NAME_SMACKIPIN
	 *	XATTR_NAME_SMACKIPOUT
	 */
	if (strcmp(name, XATTR_NAME_SMACK) == 0) {
		struct super_block *sbp = dentry->d_sb;
		struct superblock_smack *sbsp = smack_superblock(sbp);

		isp->smk_inode = sbsp->smk_default;
	} else if (strcmp(name, XATTR_NAME_SMACKEXEC) == 0)
		isp->smk_task = NULL;
	else if (strcmp(name, XATTR_NAME_SMACKMMAP) == 0)
		isp->smk_mmap = NULL;
	else if (strcmp(name, XATTR_NAME_SMACKTRANSMUTE) == 0)
		isp->smk_flags &= ~SMK_INODE_TRANSMUTE;

	return 0;
}

/**
 * smack_inode_getsecurity - get smack xattrs
 * @inode: the object
 * @name: attribute name
 * @buffer: where to put the result
 * @alloc: duplicate memory
 *
 * Returns the size of the attribute or an error code
 */
static int smack_inode_getsecurity(struct user_namespace *mnt_userns,
				   struct inode *inode, const char *name,
				   void **buffer, bool alloc)
{
	struct socket_smack *ssp;
	struct socket *sock;
	struct super_block *sbp;
	struct inode *ip = (struct inode *)inode;
	struct smack_known *isp;

	if (strcmp(name, XATTR_SMACK_SUFFIX) == 0)
		isp = smk_of_inode(inode);
	else {
		/*
		 * The rest of the Smack xattrs are only on sockets.
		 */
		sbp = ip->i_sb;
		if (sbp->s_magic != SOCKFS_MAGIC)
			return -EOPNOTSUPP;

		sock = SOCKET_I(ip);
		if (sock == NULL || sock->sk == NULL)
			return -EOPNOTSUPP;

		ssp = sock->sk->sk_security;

		if (strcmp(name, XATTR_SMACK_IPIN) == 0)
			isp = ssp->smk_in;
		else if (strcmp(name, XATTR_SMACK_IPOUT) == 0)
			isp = ssp->smk_out;
		else
			return -EOPNOTSUPP;
	}

	if (alloc) {
		*buffer = kstrdup(isp->smk_known, GFP_KERNEL);
		if (*buffer == NULL)
			return -ENOMEM;
	}

	return strlen(isp->smk_known);
}


/**
 * smack_inode_listsecurity - list the Smack attributes
 * @inode: the object
 * @buffer: where they go
 * @buffer_size: size of buffer
 */
static int smack_inode_listsecurity(struct inode *inode, char *buffer,
				    size_t buffer_size)
{
	int len = sizeof(XATTR_NAME_SMACK);

	if (buffer != NULL && len <= buffer_size)
		memcpy(buffer, XATTR_NAME_SMACK, len);

	return len;
}

/**
 * smack_inode_getsecid - Extract inode's security id
 * @inode: inode to extract the info from
 * @secid: where result will be saved
 */
static void smack_inode_getsecid(struct inode *inode, u32 *secid)
{
	struct smack_known *skp = smk_of_inode(inode);

	*secid = skp->smk_secid;
}

/*
 * File Hooks
 */

/*
 * There is no smack_file_permission hook
 *
 * Should access checks be done on each read or write?
 * UNICOS and SELinux say yes.
 * Trusted Solaris, Trusted Irix, and just about everyone else says no.
 *
 * I'll say no for now. Smack does not do the frequent
 * label changing that SELinux does.
 */

/**
 * smack_file_alloc_security - assign a file security blob
 * @file: the object
 *
 * The security blob for a file is a pointer to the master
 * label list, so no allocation is done.
 *
 * f_security is the owner security information. It
 * isn't used on file access checks, it's for send_sigio.
 *
 * Returns 0
 */
static int smack_file_alloc_security(struct file *file)
{
	struct smack_known **blob = smack_file(file);

	*blob = smk_of_current();
	return 0;
}

/**
 * smack_file_ioctl - Smack check on ioctls
 * @file: the object
 * @cmd: what to do
 * @arg: unused
 *
 * Relies heavily on the correct use of the ioctl command conventions.
 *
 * Returns 0 if allowed, error code otherwise
 */
static int smack_file_ioctl(struct file *file, unsigned int cmd,
			    unsigned long arg)
{
	int rc = 0;
	struct smk_audit_info ad;
	struct inode *inode = file_inode(file);

	if (unlikely(IS_PRIVATE(inode)))
		return 0;

	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_PATH);
	smk_ad_setfield_u_fs_path(&ad, file->f_path);

	if (_IOC_DIR(cmd) & _IOC_WRITE) {
		rc = smk_curacc(smk_of_inode(inode), MAY_WRITE, &ad);
		rc = smk_bu_file(file, MAY_WRITE, rc);
	}

	if (rc == 0 && (_IOC_DIR(cmd) & _IOC_READ)) {
		rc = smk_curacc(smk_of_inode(inode), MAY_READ, &ad);
		rc = smk_bu_file(file, MAY_READ, rc);
	}

	return rc;
}

/**
 * smack_file_lock - Smack check on file locking
 * @file: the object
 * @cmd: unused
 *
 * Returns 0 if current has lock access, error code otherwise
 */
static int smack_file_lock(struct file *file, unsigned int cmd)
{
	struct smk_audit_info ad;
	int rc;
	struct inode *inode = file_inode(file);

	if (unlikely(IS_PRIVATE(inode)))
		return 0;

	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_PATH);
	smk_ad_setfield_u_fs_path(&ad, file->f_path);
	rc = smk_curacc(smk_of_inode(inode), MAY_LOCK, &ad);
	rc = smk_bu_file(file, MAY_LOCK, rc);
	return rc;
}

/**
 * smack_file_fcntl - Smack check on fcntl
 * @file: the object
 * @cmd: what action to check
 * @arg: unused
 *
 * Generally these operations are harmless.
 * File locking operations present an obvious mechanism
 * for passing information, so they require write access.
 *
 * Returns 0 if current has access, error code otherwise
 */
static int smack_file_fcntl(struct file *file, unsigned int cmd,
			    unsigned long arg)
{
	struct smk_audit_info ad;
	int rc = 0;
	struct inode *inode = file_inode(file);

	if (unlikely(IS_PRIVATE(inode)))
		return 0;

	switch (cmd) {
	case F_GETLK:
		break;
	case F_SETLK:
	case F_SETLKW:
		smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_PATH);
		smk_ad_setfield_u_fs_path(&ad, file->f_path);
		rc = smk_curacc(smk_of_inode(inode), MAY_LOCK, &ad);
		rc = smk_bu_file(file, MAY_LOCK, rc);
		break;
	case F_SETOWN:
	case F_SETSIG:
		smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_PATH);
		smk_ad_setfield_u_fs_path(&ad, file->f_path);
		rc = smk_curacc(smk_of_inode(inode), MAY_WRITE, &ad);
		rc = smk_bu_file(file, MAY_WRITE, rc);
		break;
	default:
		break;
	}

	return rc;
}

/**
 * smack_mmap_file :
 * Check permissions for a mmap operation.  The @file may be NULL, e.g.
 * if mapping anonymous memory.
 * @file contains the file structure for file to map (may be NULL).
 * @reqprot contains the protection requested by the application.
 * @prot contains the protection that will be applied by the kernel.
 * @flags contains the operational flags.
 * Return 0 if permission is granted.
 */
static int smack_mmap_file(struct file *file,
			   unsigned long reqprot, unsigned long prot,
			   unsigned long flags)
{
	struct smack_known *skp;
	struct smack_known *mkp;
	struct smack_rule *srp;
	struct task_smack *tsp;
	struct smack_known *okp;
	struct inode_smack *isp;
	struct superblock_smack *sbsp;
	int may;
	int mmay;
	int tmay;
	int rc;

	if (file == NULL)
		return 0;

	if (unlikely(IS_PRIVATE(file_inode(file))))
		return 0;

	isp = smack_inode(file_inode(file));
	if (isp->smk_mmap == NULL)
		return 0;
	sbsp = smack_superblock(file_inode(file)->i_sb);
	if (sbsp->smk_flags & SMK_SB_UNTRUSTED &&
	    isp->smk_mmap != sbsp->smk_root)
		return -EACCES;
	mkp = isp->smk_mmap;

	tsp = smack_cred(current_cred());
	skp = smk_of_current();
	rc = 0;

	rcu_read_lock();
	/*
	 * For each Smack rule associated with the subject
	 * label verify that the SMACK64MMAP also has access
	 * to that rule's object label.
	 */
	list_for_each_entry_rcu(srp, &skp->smk_rules, list) {
		okp = srp->smk_object;
		/*
		 * Matching labels always allows access.
		 */
		if (mkp->smk_known == okp->smk_known)
			continue;
		/*
		 * If there is a matching local rule take
		 * that into account as well.
		 */
		may = smk_access_entry(srp->smk_subject->smk_known,
				       okp->smk_known,
				       &tsp->smk_rules);
		if (may == -ENOENT)
			may = srp->smk_access;
		else
			may &= srp->smk_access;
		/*
		 * If may is zero the SMACK64MMAP subject can't
		 * possibly have less access.
		 */
		if (may == 0)
			continue;

		/*
		 * Fetch the global list entry.
		 * If there isn't one a SMACK64MMAP subject
		 * can't have as much access as current.
		 */
		mmay = smk_access_entry(mkp->smk_known, okp->smk_known,
					&mkp->smk_rules);
		if (mmay == -ENOENT) {
			rc = -EACCES;
			break;
		}
		/*
		 * If there is a local entry it modifies the
		 * potential access, too.
		 */
		tmay = smk_access_entry(mkp->smk_known, okp->smk_known,
					&tsp->smk_rules);
		if (tmay != -ENOENT)
			mmay &= tmay;

		/*
		 * If there is any access available to current that is
		 * not available to a SMACK64MMAP subject
		 * deny access.
		 */
		if ((may | mmay) != mmay) {
			rc = -EACCES;
			break;
		}
	}

	rcu_read_unlock();

	return rc;
}

/**
 * smack_file_set_fowner - set the file security blob value
 * @file: object in question
 *
 */
static void smack_file_set_fowner(struct file *file)
{
	struct smack_known **blob = smack_file(file);

	*blob = smk_of_current();
}

/**
 * smack_file_send_sigiotask - Smack on sigio
 * @tsk: The target task
 * @fown: the object the signal come from
 * @signum: unused
 *
 * Allow a privileged task to get signals even if it shouldn't
 *
 * Returns 0 if a subject with the object's smack could
 * write to the task, an error code otherwise.
 */
static int smack_file_send_sigiotask(struct task_struct *tsk,
				     struct fown_struct *fown, int signum)
{
	struct smack_known **blob;
	struct smack_known *skp;
	struct smack_known *tkp = smk_of_task(smack_cred(tsk->cred));
	const struct cred *tcred;
	struct file *file;
	int rc;
	struct smk_audit_info ad;

	/*
	 * struct fown_struct is never outside the context of a struct file
	 */
	file = container_of(fown, struct file, f_owner);

	/* we don't log here as rc can be overriden */
	blob = smack_file(file);
	skp = *blob;
	rc = smk_access(skp, tkp, MAY_DELIVER, NULL);
	rc = smk_bu_note("sigiotask", skp, tkp, MAY_DELIVER, rc);

	rcu_read_lock();
	tcred = __task_cred(tsk);
	if (rc != 0 && smack_privileged_cred(CAP_MAC_OVERRIDE, tcred))
		rc = 0;
	rcu_read_unlock();

	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_TASK);
	smk_ad_setfield_u_tsk(&ad, tsk);
	smack_log(skp->smk_known, tkp->smk_known, MAY_DELIVER, rc, &ad);
	return rc;
}

/**
 * smack_file_receive - Smack file receive check
 * @file: the object
 *
 * Returns 0 if current has access, error code otherwise
 */
static int smack_file_receive(struct file *file)
{
	int rc;
	int may = 0;
	struct smk_audit_info ad;
	struct inode *inode = file_inode(file);
	struct socket *sock;
	struct task_smack *tsp;
	struct socket_smack *ssp;

	if (unlikely(IS_PRIVATE(inode)))
		return 0;

	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_PATH);
	smk_ad_setfield_u_fs_path(&ad, file->f_path);

	if (inode->i_sb->s_magic == SOCKFS_MAGIC) {
		sock = SOCKET_I(inode);
		ssp = sock->sk->sk_security;
		tsp = smack_cred(current_cred());
		/*
		 * If the receiving process can't write to the
		 * passed socket or if the passed socket can't
		 * write to the receiving process don't accept
		 * the passed socket.
		 */
		rc = smk_access(tsp->smk_task, ssp->smk_out, MAY_WRITE, &ad);
		rc = smk_bu_file(file, may, rc);
		if (rc < 0)
			return rc;
		rc = smk_access(ssp->smk_in, tsp->smk_task, MAY_WRITE, &ad);
		rc = smk_bu_file(file, may, rc);
		return rc;
	}
	/*
	 * This code relies on bitmasks.
	 */
	if (file->f_mode & FMODE_READ)
		may = MAY_READ;
	if (file->f_mode & FMODE_WRITE)
		may |= MAY_WRITE;

	rc = smk_curacc(smk_of_inode(inode), may, &ad);
	rc = smk_bu_file(file, may, rc);
	return rc;
}

/**
 * smack_file_open - Smack dentry open processing
 * @file: the object
 *
 * Set the security blob in the file structure.
 * Allow the open only if the task has read access. There are
 * many read operations (e.g. fstat) that you can do with an
 * fd even if you have the file open write-only.
 *
 * Returns 0 if current has access, error code otherwise
 */
static int smack_file_open(struct file *file)
{
	struct task_smack *tsp = smack_cred(file->f_cred);
	struct inode *inode = file_inode(file);
	struct smk_audit_info ad;
	int rc;

	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_PATH);
	smk_ad_setfield_u_fs_path(&ad, file->f_path);
	rc = smk_tskacc(tsp, smk_of_inode(inode), MAY_READ, &ad);
	rc = smk_bu_credfile(file->f_cred, file, MAY_READ, rc);

	return rc;
}

/*
 * Task hooks
 */

/**
 * smack_cred_alloc_blank - "allocate" blank task-level security credentials
 * @cred: the new credentials
 * @gfp: the atomicity of any memory allocations
 *
 * Prepare a blank set of credentials for modification.  This must allocate all
 * the memory the LSM module might require such that cred_transfer() can
 * complete without error.
 */
static int smack_cred_alloc_blank(struct cred *cred, gfp_t gfp)
{
	init_task_smack(smack_cred(cred), NULL, NULL);
	return 0;
}


/**
 * smack_cred_free - "free" task-level security credentials
 * @cred: the credentials in question
 *
 */
static void smack_cred_free(struct cred *cred)
{
	struct task_smack *tsp = smack_cred(cred);
	struct smack_rule *rp;
	struct list_head *l;
	struct list_head *n;

	smk_destroy_label_list(&tsp->smk_relabel);

	list_for_each_safe(l, n, &tsp->smk_rules) {
		rp = list_entry(l, struct smack_rule, list);
		list_del(&rp->list);
		kmem_cache_free(smack_rule_cache, rp);
	}
}

/**
 * smack_cred_prepare - prepare new set of credentials for modification
 * @new: the new credentials
 * @old: the original credentials
 * @gfp: the atomicity of any memory allocations
 *
 * Prepare a new set of credentials for modification.
 */
static int smack_cred_prepare(struct cred *new, const struct cred *old,
			      gfp_t gfp)
{
	struct task_smack *old_tsp = smack_cred(old);
	struct task_smack *new_tsp = smack_cred(new);
	int rc;

	init_task_smack(new_tsp, old_tsp->smk_task, old_tsp->smk_task);

	rc = smk_copy_rules(&new_tsp->smk_rules, &old_tsp->smk_rules, gfp);
	if (rc != 0)
		return rc;

	rc = smk_copy_relabel(&new_tsp->smk_relabel, &old_tsp->smk_relabel,
				gfp);
	return rc;
}

/**
 * smack_cred_transfer - Transfer the old credentials to the new credentials
 * @new: the new credentials
 * @old: the original credentials
 *
 * Fill in a set of blank credentials from another set of credentials.
 */
static void smack_cred_transfer(struct cred *new, const struct cred *old)
{
	struct task_smack *old_tsp = smack_cred(old);
	struct task_smack *new_tsp = smack_cred(new);

	new_tsp->smk_task = old_tsp->smk_task;
	new_tsp->smk_forked = old_tsp->smk_task;
	mutex_init(&new_tsp->smk_rules_lock);
	INIT_LIST_HEAD(&new_tsp->smk_rules);

	/* cbs copy rule list */
}

/**
 * smack_cred_getsecid - get the secid corresponding to a creds structure
 * @cred: the object creds
 * @secid: where to put the result
 *
 * Sets the secid to contain a u32 version of the smack label.
 */
static void smack_cred_getsecid(const struct cred *cred, u32 *secid)
{
	struct smack_known *skp;

	rcu_read_lock();
	skp = smk_of_task(smack_cred(cred));
	*secid = skp->smk_secid;
	rcu_read_unlock();
}

/**
 * smack_kernel_act_as - Set the subjective context in a set of credentials
 * @new: points to the set of credentials to be modified.
 * @secid: specifies the security ID to be set
 *
 * Set the security data for a kernel service.
 */
static int smack_kernel_act_as(struct cred *new, u32 secid)
{
	struct task_smack *new_tsp = smack_cred(new);

	new_tsp->smk_task = smack_from_secid(secid);
	return 0;
}

/**
 * smack_kernel_create_files_as - Set the file creation label in a set of creds
 * @new: points to the set of credentials to be modified
 * @inode: points to the inode to use as a reference
 *
 * Set the file creation context in a set of credentials to the same
 * as the objective context of the specified inode
 */
static int smack_kernel_create_files_as(struct cred *new,
					struct inode *inode)
{
	struct inode_smack *isp = smack_inode(inode);
	struct task_smack *tsp = smack_cred(new);

	tsp->smk_forked = isp->smk_inode;
	tsp->smk_task = tsp->smk_forked;
	return 0;
}

/**
 * smk_curacc_on_task - helper to log task related access
 * @p: the task object
 * @access: the access requested
 * @caller: name of the calling function for audit
 *
 * Return 0 if access is permitted
 */
static int smk_curacc_on_task(struct task_struct *p, int access,
				const char *caller)
{
	struct smk_audit_info ad;
	struct smack_known *skp = smk_of_task_struct_subj(p);
	int rc;

	smk_ad_init(&ad, caller, LSM_AUDIT_DATA_TASK);
	smk_ad_setfield_u_tsk(&ad, p);
	rc = smk_curacc(skp, access, &ad);
	rc = smk_bu_task(p, access, rc);
	return rc;
}

/**
 * smack_task_setpgid - Smack check on setting pgid
 * @p: the task object
 * @pgid: unused
 *
 * Return 0 if write access is permitted
 */
static int smack_task_setpgid(struct task_struct *p, pid_t pgid)
{
	return smk_curacc_on_task(p, MAY_WRITE, __func__);
}

/**
 * smack_task_getpgid - Smack access check for getpgid
 * @p: the object task
 *
 * Returns 0 if current can read the object task, error code otherwise
 */
static int smack_task_getpgid(struct task_struct *p)
{
	return smk_curacc_on_task(p, MAY_READ, __func__);
}

/**
 * smack_task_getsid - Smack access check for getsid
 * @p: the object task
 *
 * Returns 0 if current can read the object task, error code otherwise
 */
static int smack_task_getsid(struct task_struct *p)
{
	return smk_curacc_on_task(p, MAY_READ, __func__);
}

/**
 * smack_task_getsecid_subj - get the subjective secid of the task
 * @p: the task
 * @secid: where to put the result
 *
 * Sets the secid to contain a u32 version of the task's subjective smack label.
 */
static void smack_task_getsecid_subj(struct task_struct *p, u32 *secid)
{
	struct smack_known *skp = smk_of_task_struct_subj(p);

	*secid = skp->smk_secid;
}

/**
 * smack_task_getsecid_obj - get the objective secid of the task
 * @p: the task
 * @secid: where to put the result
 *
 * Sets the secid to contain a u32 version of the task's objective smack label.
 */
static void smack_task_getsecid_obj(struct task_struct *p, u32 *secid)
{
	struct smack_known *skp = smk_of_task_struct_obj(p);

	*secid = skp->smk_secid;
}

/**
 * smack_task_setnice - Smack check on setting nice
 * @p: the task object
 * @nice: unused
 *
 * Return 0 if write access is permitted
 */
static int smack_task_setnice(struct task_struct *p, int nice)
{
	return smk_curacc_on_task(p, MAY_WRITE, __func__);
}

/**
 * smack_task_setioprio - Smack check on setting ioprio
 * @p: the task object
 * @ioprio: unused
 *
 * Return 0 if write access is permitted
 */
static int smack_task_setioprio(struct task_struct *p, int ioprio)
{
	return smk_curacc_on_task(p, MAY_WRITE, __func__);
}

/**
 * smack_task_getioprio - Smack check on reading ioprio
 * @p: the task object
 *
 * Return 0 if read access is permitted
 */
static int smack_task_getioprio(struct task_struct *p)
{
	return smk_curacc_on_task(p, MAY_READ, __func__);
}

/**
 * smack_task_setscheduler - Smack check on setting scheduler
 * @p: the task object
 *
 * Return 0 if read access is permitted
 */
static int smack_task_setscheduler(struct task_struct *p)
{
	return smk_curacc_on_task(p, MAY_WRITE, __func__);
}

/**
 * smack_task_getscheduler - Smack check on reading scheduler
 * @p: the task object
 *
 * Return 0 if read access is permitted
 */
static int smack_task_getscheduler(struct task_struct *p)
{
	return smk_curacc_on_task(p, MAY_READ, __func__);
}

/**
 * smack_task_movememory - Smack check on moving memory
 * @p: the task object
 *
 * Return 0 if write access is permitted
 */
static int smack_task_movememory(struct task_struct *p)
{
	return smk_curacc_on_task(p, MAY_WRITE, __func__);
}

/**
 * smack_task_kill - Smack check on signal delivery
 * @p: the task object
 * @info: unused
 * @sig: unused
 * @cred: identifies the cred to use in lieu of current's
 *
 * Return 0 if write access is permitted
 *
 */
static int smack_task_kill(struct task_struct *p, struct kernel_siginfo *info,
			   int sig, const struct cred *cred)
{
	struct smk_audit_info ad;
	struct smack_known *skp;
	struct smack_known *tkp = smk_of_task_struct_obj(p);
	int rc;

	if (!sig)
		return 0; /* null signal; existence test */

	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_TASK);
	smk_ad_setfield_u_tsk(&ad, p);
	/*
	 * Sending a signal requires that the sender
	 * can write the receiver.
	 */
	if (cred == NULL) {
		rc = smk_curacc(tkp, MAY_DELIVER, &ad);
		rc = smk_bu_task(p, MAY_DELIVER, rc);
		return rc;
	}
	/*
	 * If the cred isn't NULL we're dealing with some USB IO
	 * specific behavior. This is not clean. For one thing
	 * we can't take privilege into account.
	 */
	skp = smk_of_task(smack_cred(cred));
	rc = smk_access(skp, tkp, MAY_DELIVER, &ad);
	rc = smk_bu_note("USB signal", skp, tkp, MAY_DELIVER, rc);
	return rc;
}

/**
 * smack_task_to_inode - copy task smack into the inode blob
 * @p: task to copy from
 * @inode: inode to copy to
 *
 * Sets the smack pointer in the inode security blob
 */
static void smack_task_to_inode(struct task_struct *p, struct inode *inode)
{
	struct inode_smack *isp = smack_inode(inode);
	struct smack_known *skp = smk_of_task_struct_obj(p);

	isp->smk_inode = skp;
	isp->smk_flags |= SMK_INODE_INSTANT;
}

/*
 * Socket hooks.
 */

/**
 * smack_sk_alloc_security - Allocate a socket blob
 * @sk: the socket
 * @family: unused
 * @gfp_flags: memory allocation flags
 *
 * Assign Smack pointers to current
 *
 * Returns 0 on success, -ENOMEM is there's no memory
 */
static int smack_sk_alloc_security(struct sock *sk, int family, gfp_t gfp_flags)
{
	struct smack_known *skp = smk_of_current();
	struct socket_smack *ssp;

	ssp = kzalloc(sizeof(struct socket_smack), gfp_flags);
	if (ssp == NULL)
		return -ENOMEM;

	/*
	 * Sockets created by kernel threads receive web label.
	 */
	if (unlikely(current->flags & PF_KTHREAD)) {
		ssp->smk_in = &smack_known_web;
		ssp->smk_out = &smack_known_web;
	} else {
		ssp->smk_in = skp;
		ssp->smk_out = skp;
	}
	ssp->smk_packet = NULL;

	sk->sk_security = ssp;

	return 0;
}

/**
 * smack_sk_free_security - Free a socket blob
 * @sk: the socket
 *
 * Clears the blob pointer
 */
static void smack_sk_free_security(struct sock *sk)
{
#ifdef SMACK_IPV6_PORT_LABELING
	struct smk_port_label *spp;

	if (sk->sk_family == PF_INET6) {
		rcu_read_lock();
		list_for_each_entry_rcu(spp, &smk_ipv6_port_list, list) {
			if (spp->smk_sock != sk)
				continue;
			spp->smk_can_reuse = 1;
			break;
		}
		rcu_read_unlock();
	}
#endif
	kfree(sk->sk_security);
}

/**
* smack_ipv4host_label - check host based restrictions
* @sip: the object end
*
* looks for host based access restrictions
*
* This version will only be appropriate for really small sets of single label
* hosts.  The caller is responsible for ensuring that the RCU read lock is
* taken before calling this function.
*
* Returns the label of the far end or NULL if it's not special.
*/
static struct smack_known *smack_ipv4host_label(struct sockaddr_in *sip)
{
	struct smk_net4addr *snp;
	struct in_addr *siap = &sip->sin_addr;

	if (siap->s_addr == 0)
		return NULL;

	list_for_each_entry_rcu(snp, &smk_net4addr_list, list)
		/*
		 * we break after finding the first match because
		 * the list is sorted from longest to shortest mask
		 * so we have found the most specific match
		 */
		if (snp->smk_host.s_addr ==
		    (siap->s_addr & snp->smk_mask.s_addr))
			return snp->smk_label;

	return NULL;
}

/*
 * smk_ipv6_localhost - Check for local ipv6 host address
 * @sip: the address
 *
 * Returns boolean true if this is the localhost address
 */
static bool smk_ipv6_localhost(struct sockaddr_in6 *sip)
{
	__be16 *be16p = (__be16 *)&sip->sin6_addr;
	__be32 *be32p = (__be32 *)&sip->sin6_addr;

	if (be32p[0] == 0 && be32p[1] == 0 && be32p[2] == 0 && be16p[6] == 0 &&
	    ntohs(be16p[7]) == 1)
		return true;
	return false;
}

/**
* smack_ipv6host_label - check host based restrictions
* @sip: the object end
*
* looks for host based access restrictions
*
* This version will only be appropriate for really small sets of single label
* hosts.  The caller is responsible for ensuring that the RCU read lock is
* taken before calling this function.
*
* Returns the label of the far end or NULL if it's not special.
*/
static struct smack_known *smack_ipv6host_label(struct sockaddr_in6 *sip)
{
	struct smk_net6addr *snp;
	struct in6_addr *sap = &sip->sin6_addr;
	int i;
	int found = 0;

	/*
	 * It's local. Don't look for a host label.
	 */
	if (smk_ipv6_localhost(sip))
		return NULL;

	list_for_each_entry_rcu(snp, &smk_net6addr_list, list) {
		/*
		 * If the label is NULL the entry has
		 * been renounced. Ignore it.
		 */
		if (snp->smk_label == NULL)
			continue;
		/*
		* we break after finding the first match because
		* the list is sorted from longest to shortest mask
		* so we have found the most specific match
		*/
		for (found = 1, i = 0; i < 8; i++) {
			if ((sap->s6_addr16[i] & snp->smk_mask.s6_addr16[i]) !=
			    snp->smk_host.s6_addr16[i]) {
				found = 0;
				break;
			}
		}
		if (found)
			return snp->smk_label;
	}

	return NULL;
}

/**
 * smack_netlbl_add - Set the secattr on a socket
 * @sk: the socket
 *
 * Attach the outbound smack value (smk_out) to the socket.
 *
 * Returns 0 on success or an error code
 */
static int smack_netlbl_add(struct sock *sk)
{
	struct socket_smack *ssp = sk->sk_security;
	struct smack_known *skp = ssp->smk_out;
	int rc;

	local_bh_disable();
	bh_lock_sock_nested(sk);

	rc = netlbl_sock_setattr(sk, sk->sk_family, &skp->smk_netlabel);
	switch (rc) {
	case 0:
		ssp->smk_state = SMK_NETLBL_LABELED;
		break;
	case -EDESTADDRREQ:
		ssp->smk_state = SMK_NETLBL_REQSKB;
		rc = 0;
		break;
	}

	bh_unlock_sock(sk);
	local_bh_enable();

	return rc;
}

/**
 * smack_netlbl_delete - Remove the secattr from a socket
 * @sk: the socket
 *
 * Remove the outbound smack value from a socket
 */
static void smack_netlbl_delete(struct sock *sk)
{
	struct socket_smack *ssp = sk->sk_security;

	/*
	 * Take the label off the socket if one is set.
	 */
	if (ssp->smk_state != SMK_NETLBL_LABELED)
		return;

	local_bh_disable();
	bh_lock_sock_nested(sk);
	netlbl_sock_delattr(sk);
	bh_unlock_sock(sk);
	local_bh_enable();
	ssp->smk_state = SMK_NETLBL_UNLABELED;
}

/**
 * smk_ipv4_check - Perform IPv4 host access checks
 * @sk: the socket
 * @sap: the destination address
 *
 * Set the correct secattr for the given socket based on the destination
 * address and perform any outbound access checks needed.
 *
 * Returns 0 on success or an error code.
 *
 */
static int smk_ipv4_check(struct sock *sk, struct sockaddr_in *sap)
{
	struct smack_known *skp;
	int rc = 0;
	struct smack_known *hkp;
	struct socket_smack *ssp = sk->sk_security;
	struct smk_audit_info ad;

	rcu_read_lock();
	hkp = smack_ipv4host_label(sap);
	if (hkp != NULL) {
#ifdef CONFIG_AUDIT
		struct lsm_network_audit net;

		smk_ad_init_net(&ad, __func__, LSM_AUDIT_DATA_NET, &net);
		ad.a.u.net->family = sap->sin_family;
		ad.a.u.net->dport = sap->sin_port;
		ad.a.u.net->v4info.daddr = sap->sin_addr.s_addr;
#endif
		skp = ssp->smk_out;
		rc = smk_access(skp, hkp, MAY_WRITE, &ad);
		rc = smk_bu_note("IPv4 host check", skp, hkp, MAY_WRITE, rc);
		/*
		 * Clear the socket netlabel if it's set.
		 */
		if (!rc)
			smack_netlbl_delete(sk);
	}
	rcu_read_unlock();

	return rc;
}

/**
 * smk_ipv6_check - check Smack access
 * @subject: subject Smack label
 * @object: object Smack label
 * @address: address
 * @act: the action being taken
 *
 * Check an IPv6 access
 */
static int smk_ipv6_check(struct smack_known *subject,
				struct smack_known *object,
				struct sockaddr_in6 *address, int act)
{
#ifdef CONFIG_AUDIT
	struct lsm_network_audit net;
#endif
	struct smk_audit_info ad;
	int rc;

#ifdef CONFIG_AUDIT
	smk_ad_init_net(&ad, __func__, LSM_AUDIT_DATA_NET, &net);
	ad.a.u.net->family = PF_INET6;
	ad.a.u.net->dport = ntohs(address->sin6_port);
	if (act == SMK_RECEIVING)
		ad.a.u.net->v6info.saddr = address->sin6_addr;
	else
		ad.a.u.net->v6info.daddr = address->sin6_addr;
#endif
	rc = smk_access(subject, object, MAY_WRITE, &ad);
	rc = smk_bu_note("IPv6 check", subject, object, MAY_WRITE, rc);
	return rc;
}

#ifdef SMACK_IPV6_PORT_LABELING
/**
 * smk_ipv6_port_label - Smack port access table management
 * @sock: socket
 * @address: address
 *
 * Create or update the port list entry
 */
static void smk_ipv6_port_label(struct socket *sock, struct sockaddr *address)
{
	struct sock *sk = sock->sk;
	struct sockaddr_in6 *addr6;
	struct socket_smack *ssp = sock->sk->sk_security;
	struct smk_port_label *spp;
	unsigned short port = 0;

	if (address == NULL) {
		/*
		 * This operation is changing the Smack information
		 * on the bound socket. Take the changes to the port
		 * as well.
		 */
		rcu_read_lock();
		list_for_each_entry_rcu(spp, &smk_ipv6_port_list, list) {
			if (sk != spp->smk_sock)
				continue;
			spp->smk_in = ssp->smk_in;
			spp->smk_out = ssp->smk_out;
			rcu_read_unlock();
			return;
		}
		/*
		 * A NULL address is only used for updating existing
		 * bound entries. If there isn't one, it's OK.
		 */
		rcu_read_unlock();
		return;
	}

	addr6 = (struct sockaddr_in6 *)address;
	port = ntohs(addr6->sin6_port);
	/*
	 * This is a special case that is safely ignored.
	 */
	if (port == 0)
		return;

	/*
	 * Look for an existing port list entry.
	 * This is an indication that a port is getting reused.
	 */
	rcu_read_lock();
	list_for_each_entry_rcu(spp, &smk_ipv6_port_list, list) {
		if (spp->smk_port != port || spp->smk_sock_type != sock->type)
			continue;
		if (spp->smk_can_reuse != 1) {
			rcu_read_unlock();
			return;
		}
		spp->smk_port = port;
		spp->smk_sock = sk;
		spp->smk_in = ssp->smk_in;
		spp->smk_out = ssp->smk_out;
		spp->smk_can_reuse = 0;
		rcu_read_unlock();
		return;
	}
	rcu_read_unlock();
	/*
	 * A new port entry is required.
	 */
	spp = kzalloc(sizeof(*spp), GFP_KERNEL);
	if (spp == NULL)
		return;

	spp->smk_port = port;
	spp->smk_sock = sk;
	spp->smk_in = ssp->smk_in;
	spp->smk_out = ssp->smk_out;
	spp->smk_sock_type = sock->type;
	spp->smk_can_reuse = 0;

	mutex_lock(&smack_ipv6_lock);
	list_add_rcu(&spp->list, &smk_ipv6_port_list);
	mutex_unlock(&smack_ipv6_lock);
	return;
}
#endif

/**
 * smk_ipv6_port_check - check Smack port access
 * @sk: socket
 * @address: address
 * @act: the action being taken
 *
 * Create or update the port list entry
 */
static int smk_ipv6_port_check(struct sock *sk, struct sockaddr_in6 *address,
				int act)
{
	struct smk_port_label *spp;
	struct socket_smack *ssp = sk->sk_security;
	struct smack_known *skp = NULL;
	unsigned short port;
	struct smack_known *object;

	if (act == SMK_RECEIVING) {
		skp = smack_ipv6host_label(address);
		object = ssp->smk_in;
	} else {
		skp = ssp->smk_out;
		object = smack_ipv6host_label(address);
	}

	/*
	 * The other end is a single label host.
	 */
	if (skp != NULL && object != NULL)
		return smk_ipv6_check(skp, object, address, act);
	if (skp == NULL)
		skp = smack_net_ambient;
	if (object == NULL)
		object = smack_net_ambient;

	/*
	 * It's remote, so port lookup does no good.
	 */
	if (!smk_ipv6_localhost(address))
		return smk_ipv6_check(skp, object, address, act);

	/*
	 * It's local so the send check has to have passed.
	 */
	if (act == SMK_RECEIVING)
		return 0;

	port = ntohs(address->sin6_port);
	rcu_read_lock();
	list_for_each_entry_rcu(spp, &smk_ipv6_port_list, list) {
		if (spp->smk_port != port || spp->smk_sock_type != sk->sk_type)
			continue;
		object = spp->smk_in;
		if (act == SMK_CONNECTING)
			ssp->smk_packet = spp->smk_out;
		break;
	}
	rcu_read_unlock();

	return smk_ipv6_check(skp, object, address, act);
}

/**
 * smack_inode_setsecurity - set smack xattrs
 * @inode: the object
 * @name: attribute name
 * @value: attribute value
 * @size: size of the attribute
 * @flags: unused
 *
 * Sets the named attribute in the appropriate blob
 *
 * Returns 0 on success, or an error code
 */
static int smack_inode_setsecurity(struct inode *inode, const char *name,
				   const void *value, size_t size, int flags)
{
	struct smack_known *skp;
	struct inode_smack *nsp = smack_inode(inode);
	struct socket_smack *ssp;
	struct socket *sock;
	int rc = 0;

	if (value == NULL || size > SMK_LONGLABEL || size == 0)
		return -EINVAL;

	skp = smk_import_entry(value, size);
	if (IS_ERR(skp))
		return PTR_ERR(skp);

	if (strcmp(name, XATTR_SMACK_SUFFIX) == 0) {
		nsp->smk_inode = skp;
		nsp->smk_flags |= SMK_INODE_INSTANT;
		return 0;
	}
	/*
	 * The rest of the Smack xattrs are only on sockets.
	 */
	if (inode->i_sb->s_magic != SOCKFS_MAGIC)
		return -EOPNOTSUPP;

	sock = SOCKET_I(inode);
	if (sock == NULL || sock->sk == NULL)
		return -EOPNOTSUPP;

	ssp = sock->sk->sk_security;

	if (strcmp(name, XATTR_SMACK_IPIN) == 0)
		ssp->smk_in = skp;
	else if (strcmp(name, XATTR_SMACK_IPOUT) == 0) {
		ssp->smk_out = skp;
		if (sock->sk->sk_family == PF_INET) {
			rc = smack_netlbl_add(sock->sk);
			if (rc != 0)
				printk(KERN_WARNING
					"Smack: \"%s\" netlbl error %d.\n",
					__func__, -rc);
		}
	} else
		return -EOPNOTSUPP;

#ifdef SMACK_IPV6_PORT_LABELING
	if (sock->sk->sk_family == PF_INET6)
		smk_ipv6_port_label(sock, NULL);
#endif

	return 0;
}

/**
 * smack_socket_post_create - finish socket setup
 * @sock: the socket
 * @family: protocol family
 * @type: unused
 * @protocol: unused
 * @kern: unused
 *
 * Sets the netlabel information on the socket
 *
 * Returns 0 on success, and error code otherwise
 */
static int smack_socket_post_create(struct socket *sock, int family,
				    int type, int protocol, int kern)
{
	struct socket_smack *ssp;

	if (sock->sk == NULL)
		return 0;

	/*
	 * Sockets created by kernel threads receive web label.
	 */
	if (unlikely(current->flags & PF_KTHREAD)) {
		ssp = sock->sk->sk_security;
		ssp->smk_in = &smack_known_web;
		ssp->smk_out = &smack_known_web;
	}

	if (family != PF_INET)
		return 0;
	/*
	 * Set the outbound netlbl.
	 */
	return smack_netlbl_add(sock->sk);
}

/**
 * smack_socket_socketpair - create socket pair
 * @socka: one socket
 * @sockb: another socket
 *
 * Cross reference the peer labels for SO_PEERSEC
 *
 * Returns 0
 */
static int smack_socket_socketpair(struct socket *socka,
		                   struct socket *sockb)
{
	struct socket_smack *asp = socka->sk->sk_security;
	struct socket_smack *bsp = sockb->sk->sk_security;

	asp->smk_packet = bsp->smk_out;
	bsp->smk_packet = asp->smk_out;

	return 0;
}

#ifdef SMACK_IPV6_PORT_LABELING
/**
 * smack_socket_bind - record port binding information.
 * @sock: the socket
 * @address: the port address
 * @addrlen: size of the address
 *
 * Records the label bound to a port.
 *
 * Returns 0 on success, and error code otherwise
 */
static int smack_socket_bind(struct socket *sock, struct sockaddr *address,
				int addrlen)
{
	if (sock->sk != NULL && sock->sk->sk_family == PF_INET6) {
		if (addrlen < SIN6_LEN_RFC2133 ||
		    address->sa_family != AF_INET6)
			return -EINVAL;
		smk_ipv6_port_label(sock, address);
	}
	return 0;
}
#endif /* SMACK_IPV6_PORT_LABELING */

/**
 * smack_socket_connect - connect access check
 * @sock: the socket
 * @sap: the other end
 * @addrlen: size of sap
 *
 * Verifies that a connection may be possible
 *
 * Returns 0 on success, and error code otherwise
 */
static int smack_socket_connect(struct socket *sock, struct sockaddr *sap,
				int addrlen)
{
	int rc = 0;

	if (sock->sk == NULL)
		return 0;
	if (sock->sk->sk_family != PF_INET &&
	    (!IS_ENABLED(CONFIG_IPV6) || sock->sk->sk_family != PF_INET6))
		return 0;
	if (addrlen < offsetofend(struct sockaddr, sa_family))
		return 0;
	if (IS_ENABLED(CONFIG_IPV6) && sap->sa_family == AF_INET6) {
		struct sockaddr_in6 *sip = (struct sockaddr_in6 *)sap;
		struct smack_known *rsp = NULL;

		if (addrlen < SIN6_LEN_RFC2133)
			return 0;
		if (__is_defined(SMACK_IPV6_SECMARK_LABELING))
			rsp = smack_ipv6host_label(sip);
		if (rsp != NULL) {
			struct socket_smack *ssp = sock->sk->sk_security;

			rc = smk_ipv6_check(ssp->smk_out, rsp, sip,
					    SMK_CONNECTING);
		}
		if (__is_defined(SMACK_IPV6_PORT_LABELING))
			rc = smk_ipv6_port_check(sock->sk, sip, SMK_CONNECTING);

		return rc;
	}
	if (sap->sa_family != AF_INET || addrlen < sizeof(struct sockaddr_in))
		return 0;
	rc = smk_ipv4_check(sock->sk, (struct sockaddr_in *)sap);
	return rc;
}

/**
 * smack_flags_to_may - convert S_ to MAY_ values
 * @flags: the S_ value
 *
 * Returns the equivalent MAY_ value
 */
static int smack_flags_to_may(int flags)
{
	int may = 0;

	if (flags & S_IRUGO)
		may |= MAY_READ;
	if (flags & S_IWUGO)
		may |= MAY_WRITE;
	if (flags & S_IXUGO)
		may |= MAY_EXEC;

	return may;
}

/**
 * smack_msg_msg_alloc_security - Set the security blob for msg_msg
 * @msg: the object
 *
 * Returns 0
 */
static int smack_msg_msg_alloc_security(struct msg_msg *msg)
{
	struct smack_known **blob = smack_msg_msg(msg);

	*blob = smk_of_current();
	return 0;
}

/**
 * smack_of_ipc - the smack pointer for the ipc
 * @isp: the object
 *
 * Returns a pointer to the smack value
 */
static struct smack_known *smack_of_ipc(struct kern_ipc_perm *isp)
{
	struct smack_known **blob = smack_ipc(isp);

	return *blob;
}

/**
 * smack_ipc_alloc_security - Set the security blob for ipc
 * @isp: the object
 *
 * Returns 0
 */
static int smack_ipc_alloc_security(struct kern_ipc_perm *isp)
{
	struct smack_known **blob = smack_ipc(isp);

	*blob = smk_of_current();
	return 0;
}

/**
 * smk_curacc_shm : check if current has access on shm
 * @isp : the object
 * @access : access requested
 *
 * Returns 0 if current has the requested access, error code otherwise
 */
static int smk_curacc_shm(struct kern_ipc_perm *isp, int access)
{
	struct smack_known *ssp = smack_of_ipc(isp);
	struct smk_audit_info ad;
	int rc;

#ifdef CONFIG_AUDIT
	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_IPC);
	ad.a.u.ipc_id = isp->id;
#endif
	rc = smk_curacc(ssp, access, &ad);
	rc = smk_bu_current("shm", ssp, access, rc);
	return rc;
}

/**
 * smack_shm_associate - Smack access check for shm
 * @isp: the object
 * @shmflg: access requested
 *
 * Returns 0 if current has the requested access, error code otherwise
 */
static int smack_shm_associate(struct kern_ipc_perm *isp, int shmflg)
{
	int may;

	may = smack_flags_to_may(shmflg);
	return smk_curacc_shm(isp, may);
}

/**
 * smack_shm_shmctl - Smack access check for shm
 * @isp: the object
 * @cmd: what it wants to do
 *
 * Returns 0 if current has the requested access, error code otherwise
 */
static int smack_shm_shmctl(struct kern_ipc_perm *isp, int cmd)
{
	int may;

	switch (cmd) {
	case IPC_STAT:
	case SHM_STAT:
	case SHM_STAT_ANY:
		may = MAY_READ;
		break;
	case IPC_SET:
	case SHM_LOCK:
	case SHM_UNLOCK:
	case IPC_RMID:
		may = MAY_READWRITE;
		break;
	case IPC_INFO:
	case SHM_INFO:
		/*
		 * System level information.
		 */
		return 0;
	default:
		return -EINVAL;
	}
	return smk_curacc_shm(isp, may);
}

/**
 * smack_shm_shmat - Smack access for shmat
 * @isp: the object
 * @shmaddr: unused
 * @shmflg: access requested
 *
 * Returns 0 if current has the requested access, error code otherwise
 */
static int smack_shm_shmat(struct kern_ipc_perm *isp, char __user *shmaddr,
			   int shmflg)
{
	int may;

	may = smack_flags_to_may(shmflg);
	return smk_curacc_shm(isp, may);
}

/**
 * smk_curacc_sem : check if current has access on sem
 * @isp : the object
 * @access : access requested
 *
 * Returns 0 if current has the requested access, error code otherwise
 */
static int smk_curacc_sem(struct kern_ipc_perm *isp, int access)
{
	struct smack_known *ssp = smack_of_ipc(isp);
	struct smk_audit_info ad;
	int rc;

#ifdef CONFIG_AUDIT
	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_IPC);
	ad.a.u.ipc_id = isp->id;
#endif
	rc = smk_curacc(ssp, access, &ad);
	rc = smk_bu_current("sem", ssp, access, rc);
	return rc;
}

/**
 * smack_sem_associate - Smack access check for sem
 * @isp: the object
 * @semflg: access requested
 *
 * Returns 0 if current has the requested access, error code otherwise
 */
static int smack_sem_associate(struct kern_ipc_perm *isp, int semflg)
{
	int may;

	may = smack_flags_to_may(semflg);
	return smk_curacc_sem(isp, may);
}

/**
 * smack_sem_shmctl - Smack access check for sem
 * @isp: the object
 * @cmd: what it wants to do
 *
 * Returns 0 if current has the requested access, error code otherwise
 */
static int smack_sem_semctl(struct kern_ipc_perm *isp, int cmd)
{
	int may;

	switch (cmd) {
	case GETPID:
	case GETNCNT:
	case GETZCNT:
	case GETVAL:
	case GETALL:
	case IPC_STAT:
	case SEM_STAT:
	case SEM_STAT_ANY:
		may = MAY_READ;
		break;
	case SETVAL:
	case SETALL:
	case IPC_RMID:
	case IPC_SET:
		may = MAY_READWRITE;
		break;
	case IPC_INFO:
	case SEM_INFO:
		/*
		 * System level information
		 */
		return 0;
	default:
		return -EINVAL;
	}

	return smk_curacc_sem(isp, may);
}

/**
 * smack_sem_semop - Smack checks of semaphore operations
 * @isp: the object
 * @sops: unused
 * @nsops: unused
 * @alter: unused
 *
 * Treated as read and write in all cases.
 *
 * Returns 0 if access is allowed, error code otherwise
 */
static int smack_sem_semop(struct kern_ipc_perm *isp, struct sembuf *sops,
			   unsigned nsops, int alter)
{
	return smk_curacc_sem(isp, MAY_READWRITE);
}

/**
 * smk_curacc_msq : helper to check if current has access on msq
 * @isp : the msq
 * @access : access requested
 *
 * return 0 if current has access, error otherwise
 */
static int smk_curacc_msq(struct kern_ipc_perm *isp, int access)
{
	struct smack_known *msp = smack_of_ipc(isp);
	struct smk_audit_info ad;
	int rc;

#ifdef CONFIG_AUDIT
	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_IPC);
	ad.a.u.ipc_id = isp->id;
#endif
	rc = smk_curacc(msp, access, &ad);
	rc = smk_bu_current("msq", msp, access, rc);
	return rc;
}

/**
 * smack_msg_queue_associate - Smack access check for msg_queue
 * @isp: the object
 * @msqflg: access requested
 *
 * Returns 0 if current has the requested access, error code otherwise
 */
static int smack_msg_queue_associate(struct kern_ipc_perm *isp, int msqflg)
{
	int may;

	may = smack_flags_to_may(msqflg);
	return smk_curacc_msq(isp, may);
}

/**
 * smack_msg_queue_msgctl - Smack access check for msg_queue
 * @isp: the object
 * @cmd: what it wants to do
 *
 * Returns 0 if current has the requested access, error code otherwise
 */
static int smack_msg_queue_msgctl(struct kern_ipc_perm *isp, int cmd)
{
	int may;

	switch (cmd) {
	case IPC_STAT:
	case MSG_STAT:
	case MSG_STAT_ANY:
		may = MAY_READ;
		break;
	case IPC_SET:
	case IPC_RMID:
		may = MAY_READWRITE;
		break;
	case IPC_INFO:
	case MSG_INFO:
		/*
		 * System level information
		 */
		return 0;
	default:
		return -EINVAL;
	}

	return smk_curacc_msq(isp, may);
}

/**
 * smack_msg_queue_msgsnd - Smack access check for msg_queue
 * @isp: the object
 * @msg: unused
 * @msqflg: access requested
 *
 * Returns 0 if current has the requested access, error code otherwise
 */
static int smack_msg_queue_msgsnd(struct kern_ipc_perm *isp, struct msg_msg *msg,
				  int msqflg)
{
	int may;

	may = smack_flags_to_may(msqflg);
	return smk_curacc_msq(isp, may);
}

/**
 * smack_msg_queue_msgsnd - Smack access check for msg_queue
 * @isp: the object
 * @msg: unused
 * @target: unused
 * @type: unused
 * @mode: unused
 *
 * Returns 0 if current has read and write access, error code otherwise
 */
static int smack_msg_queue_msgrcv(struct kern_ipc_perm *isp, struct msg_msg *msg,
			struct task_struct *target, long type, int mode)
{
	return smk_curacc_msq(isp, MAY_READWRITE);
}

/**
 * smack_ipc_permission - Smack access for ipc_permission()
 * @ipp: the object permissions
 * @flag: access requested
 *
 * Returns 0 if current has read and write access, error code otherwise
 */
static int smack_ipc_permission(struct kern_ipc_perm *ipp, short flag)
{
	struct smack_known **blob = smack_ipc(ipp);
	struct smack_known *iskp = *blob;
	int may = smack_flags_to_may(flag);
	struct smk_audit_info ad;
	int rc;

#ifdef CONFIG_AUDIT
	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_IPC);
	ad.a.u.ipc_id = ipp->id;
#endif
	rc = smk_curacc(iskp, may, &ad);
	rc = smk_bu_current("svipc", iskp, may, rc);
	return rc;
}

/**
 * smack_ipc_getsecid - Extract smack security id
 * @ipp: the object permissions
 * @secid: where result will be saved
 */
static void smack_ipc_getsecid(struct kern_ipc_perm *ipp, u32 *secid)
{
	struct smack_known **blob = smack_ipc(ipp);
	struct smack_known *iskp = *blob;

	*secid = iskp->smk_secid;
}

/**
 * smack_d_instantiate - Make sure the blob is correct on an inode
 * @opt_dentry: dentry where inode will be attached
 * @inode: the object
 *
 * Set the inode's security blob if it hasn't been done already.
 */
static void smack_d_instantiate(struct dentry *opt_dentry, struct inode *inode)
{
	struct super_block *sbp;
	struct superblock_smack *sbsp;
	struct inode_smack *isp;
	struct smack_known *skp;
	struct smack_known *ckp = smk_of_current();
	struct smack_known *final;
	char trattr[TRANS_TRUE_SIZE];
	int transflag = 0;
	int rc;
	struct dentry *dp;

	if (inode == NULL)
		return;

	isp = smack_inode(inode);

	/*
	 * If the inode is already instantiated
	 * take the quick way out
	 */
	if (isp->smk_flags & SMK_INODE_INSTANT)
		return;

	sbp = inode->i_sb;
	sbsp = smack_superblock(sbp);
	/*
	 * We're going to use the superblock default label
	 * if there's no label on the file.
	 */
	final = sbsp->smk_default;

	/*
	 * If this is the root inode the superblock
	 * may be in the process of initialization.
	 * If that is the case use the root value out
	 * of the superblock.
	 */
	if (opt_dentry->d_parent == opt_dentry) {
		switch (sbp->s_magic) {
		case CGROUP_SUPER_MAGIC:
		case CGROUP2_SUPER_MAGIC:
			/*
			 * The cgroup filesystem is never mounted,
			 * so there's no opportunity to set the mount
			 * options.
			 */
			sbsp->smk_root = &smack_known_star;
			sbsp->smk_default = &smack_known_star;
			isp->smk_inode = sbsp->smk_root;
			break;
		case TMPFS_MAGIC:
			/*
			 * What about shmem/tmpfs anonymous files with dentry
			 * obtained from d_alloc_pseudo()?
			 */
			isp->smk_inode = smk_of_current();
			break;
		case PIPEFS_MAGIC:
			isp->smk_inode = smk_of_current();
			break;
		case SOCKFS_MAGIC:
			/*
			 * Socket access is controlled by the socket
			 * structures associated with the task involved.
			 */
			isp->smk_inode = &smack_known_star;
			break;
		default:
			isp->smk_inode = sbsp->smk_root;
			break;
		}
		isp->smk_flags |= SMK_INODE_INSTANT;
		return;
	}

	/*
	 * This is pretty hackish.
	 * Casey says that we shouldn't have to do
	 * file system specific code, but it does help
	 * with keeping it simple.
	 */
	switch (sbp->s_magic) {
	case SMACK_MAGIC:
	case CGROUP_SUPER_MAGIC:
	case CGROUP2_SUPER_MAGIC:
		/*
		 * Casey says that it's a little embarrassing
		 * that the smack file system doesn't do
		 * extended attributes.
		 *
		 * Cgroupfs is special
		 */
		final = &smack_known_star;
		break;
	case DEVPTS_SUPER_MAGIC:
		/*
		 * devpts seems content with the label of the task.
		 * Programs that change smack have to treat the
		 * pty with respect.
		 */
		final = ckp;
		break;
	case PROC_SUPER_MAGIC:
		/*
		 * Casey says procfs appears not to care.
		 * The superblock default suffices.
		 */
		break;
	case TMPFS_MAGIC:
		/*
		 * Device labels should come from the filesystem,
		 * but watch out, because they're volitile,
		 * getting recreated on every reboot.
		 */
		final = &smack_known_star;
		/*
		 * If a smack value has been set we want to use it,
		 * but since tmpfs isn't giving us the opportunity
		 * to set mount options simulate setting the
		 * superblock default.
		 */
		fallthrough;
	default:
		/*
		 * This isn't an understood special case.
		 * Get the value from the xattr.
		 */

		/*
		 * UNIX domain sockets use lower level socket data.
		 */
		if (S_ISSOCK(inode->i_mode)) {
			final = &smack_known_star;
			break;
		}
		/*
		 * No xattr support means, alas, no SMACK label.
		 * Use the aforeapplied default.
		 * It would be curious if the label of the task
		 * does not match that assigned.
		 */
		if (!(inode->i_opflags & IOP_XATTR))
		        break;
		/*
		 * Get the dentry for xattr.
		 */
		dp = dget(opt_dentry);
		skp = smk_fetch(XATTR_NAME_SMACK, inode, dp);
		if (!IS_ERR_OR_NULL(skp))
			final = skp;

		/*
		 * Transmuting directory
		 */
		if (S_ISDIR(inode->i_mode)) {
			/*
			 * If this is a new directory and the label was
			 * transmuted when the inode was initialized
			 * set the transmute attribute on the directory
			 * and mark the inode.
			 *
			 * If there is a transmute attribute on the
			 * directory mark the inode.
			 */
			if (isp->smk_flags & SMK_INODE_CHANGED) {
				isp->smk_flags &= ~SMK_INODE_CHANGED;
				rc = __vfs_setxattr(&init_user_ns, dp, inode,
					XATTR_NAME_SMACKTRANSMUTE,
					TRANS_TRUE, TRANS_TRUE_SIZE,
					0);
			} else {
				rc = __vfs_getxattr(dp, inode,
					XATTR_NAME_SMACKTRANSMUTE, trattr,
					TRANS_TRUE_SIZE);
				if (rc >= 0 && strncmp(trattr, TRANS_TRUE,
						       TRANS_TRUE_SIZE) != 0)
					rc = -EINVAL;
			}
			if (rc >= 0)
				transflag = SMK_INODE_TRANSMUTE;
		}
		/*
		 * Don't let the exec or mmap label be "*" or "@".
		 */
		skp = smk_fetch(XATTR_NAME_SMACKEXEC, inode, dp);
		if (IS_ERR(skp) || skp == &smack_known_star ||
		    skp == &smack_known_web)
			skp = NULL;
		isp->smk_task = skp;

		skp = smk_fetch(XATTR_NAME_SMACKMMAP, inode, dp);
		if (IS_ERR(skp) || skp == &smack_known_star ||
		    skp == &smack_known_web)
			skp = NULL;
		isp->smk_mmap = skp;

		dput(dp);
		break;
	}

	if (final == NULL)
		isp->smk_inode = ckp;
	else
		isp->smk_inode = final;

	isp->smk_flags |= (SMK_INODE_INSTANT | transflag);

	return;
}

/**
 * smack_getprocattr - Smack process attribute access
 * @p: the object task
 * @name: the name of the attribute in /proc/.../attr
 * @value: where to put the result
 *
 * Places a copy of the task Smack into value
 *
 * Returns the length of the smack label or an error code
 */
static int smack_getprocattr(struct task_struct *p, char *name, char **value)
{
	struct smack_known *skp = smk_of_task_struct_subj(p);
	char *cp;
	int slen;

	if (strcmp(name, "current") != 0)
		return -EINVAL;

	cp = kstrdup(skp->smk_known, GFP_KERNEL);
	if (cp == NULL)
		return -ENOMEM;

	slen = strlen(cp);
	*value = cp;
	return slen;
}

/**
 * smack_setprocattr - Smack process attribute setting
 * @name: the name of the attribute in /proc/.../attr
 * @value: the value to set
 * @size: the size of the value
 *
 * Sets the Smack value of the task. Only setting self
 * is permitted and only with privilege
 *
 * Returns the length of the smack label or an error code
 */
static int smack_setprocattr(const char *name, void *value, size_t size)
{
	struct task_smack *tsp = smack_cred(current_cred());
	struct cred *new;
	struct smack_known *skp;
	struct smack_known_list_elem *sklep;
	int rc;

	if (!smack_privileged(CAP_MAC_ADMIN) && list_empty(&tsp->smk_relabel))
		return -EPERM;

	if (value == NULL || size == 0 || size >= SMK_LONGLABEL)
		return -EINVAL;

	if (strcmp(name, "current") != 0)
		return -EINVAL;

	skp = smk_import_entry(value, size);
	if (IS_ERR(skp))
		return PTR_ERR(skp);

	/*
	 * No process is ever allowed the web ("@") label
	 * and the star ("*") label.
	 */
	if (skp == &smack_known_web || skp == &smack_known_star)
		return -EINVAL;

	if (!smack_privileged(CAP_MAC_ADMIN)) {
		rc = -EPERM;
		list_for_each_entry(sklep, &tsp->smk_relabel, list)
			if (sklep->smk_label == skp) {
				rc = 0;
				break;
			}
		if (rc)
			return rc;
	}

	new = prepare_creds();
	if (new == NULL)
		return -ENOMEM;

	tsp = smack_cred(new);
	tsp->smk_task = skp;
	/*
	 * process can change its label only once
	 */
	smk_destroy_label_list(&tsp->smk_relabel);

	commit_creds(new);
	return size;
}

/**
 * smack_unix_stream_connect - Smack access on UDS
 * @sock: one sock
 * @other: the other sock
 * @newsk: unused
 *
 * Return 0 if a subject with the smack of sock could access
 * an object with the smack of other, otherwise an error code
 */
static int smack_unix_stream_connect(struct sock *sock,
				     struct sock *other, struct sock *newsk)
{
	struct smack_known *skp;
	struct smack_known *okp;
	struct socket_smack *ssp = sock->sk_security;
	struct socket_smack *osp = other->sk_security;
	struct socket_smack *nsp = newsk->sk_security;
	struct smk_audit_info ad;
	int rc = 0;
#ifdef CONFIG_AUDIT
	struct lsm_network_audit net;
#endif

	if (!smack_privileged(CAP_MAC_OVERRIDE)) {
		skp = ssp->smk_out;
		okp = osp->smk_in;
#ifdef CONFIG_AUDIT
		smk_ad_init_net(&ad, __func__, LSM_AUDIT_DATA_NET, &net);
		smk_ad_setfield_u_net_sk(&ad, other);
#endif
		rc = smk_access(skp, okp, MAY_WRITE, &ad);
		rc = smk_bu_note("UDS connect", skp, okp, MAY_WRITE, rc);
		if (rc == 0) {
			okp = osp->smk_out;
			skp = ssp->smk_in;
			rc = smk_access(okp, skp, MAY_WRITE, &ad);
			rc = smk_bu_note("UDS connect", okp, skp,
						MAY_WRITE, rc);
		}
	}

	/*
	 * Cross reference the peer labels for SO_PEERSEC.
	 */
	if (rc == 0) {
		nsp->smk_packet = ssp->smk_out;
		ssp->smk_packet = osp->smk_out;
	}

	return rc;
}

/**
 * smack_unix_may_send - Smack access on UDS
 * @sock: one socket
 * @other: the other socket
 *
 * Return 0 if a subject with the smack of sock could access
 * an object with the smack of other, otherwise an error code
 */
static int smack_unix_may_send(struct socket *sock, struct socket *other)
{
	struct socket_smack *ssp = sock->sk->sk_security;
	struct socket_smack *osp = other->sk->sk_security;
	struct smk_audit_info ad;
	int rc;

#ifdef CONFIG_AUDIT
	struct lsm_network_audit net;

	smk_ad_init_net(&ad, __func__, LSM_AUDIT_DATA_NET, &net);
	smk_ad_setfield_u_net_sk(&ad, other->sk);
#endif

	if (smack_privileged(CAP_MAC_OVERRIDE))
		return 0;

	rc = smk_access(ssp->smk_out, osp->smk_in, MAY_WRITE, &ad);
	rc = smk_bu_note("UDS send", ssp->smk_out, osp->smk_in, MAY_WRITE, rc);
	return rc;
}

/**
 * smack_socket_sendmsg - Smack check based on destination host
 * @sock: the socket
 * @msg: the message
 * @size: the size of the message
 *
 * Return 0 if the current subject can write to the destination host.
 * For IPv4 this is only a question if the destination is a single label host.
 * For IPv6 this is a check against the label of the port.
 */
static int smack_socket_sendmsg(struct socket *sock, struct msghdr *msg,
				int size)
{
	struct sockaddr_in *sip = (struct sockaddr_in *) msg->msg_name;
#if IS_ENABLED(CONFIG_IPV6)
	struct sockaddr_in6 *sap = (struct sockaddr_in6 *) msg->msg_name;
#endif
#ifdef SMACK_IPV6_SECMARK_LABELING
	struct socket_smack *ssp = sock->sk->sk_security;
	struct smack_known *rsp;
#endif
	int rc = 0;

	/*
	 * Perfectly reasonable for this to be NULL
	 */
	if (sip == NULL)
		return 0;

	switch (sock->sk->sk_family) {
	case AF_INET:
		if (msg->msg_namelen < sizeof(struct sockaddr_in) ||
		    sip->sin_family != AF_INET)
			return -EINVAL;
		rc = smk_ipv4_check(sock->sk, sip);
		break;
#if IS_ENABLED(CONFIG_IPV6)
	case AF_INET6:
		if (msg->msg_namelen < SIN6_LEN_RFC2133 ||
		    sap->sin6_family != AF_INET6)
			return -EINVAL;
#ifdef SMACK_IPV6_SECMARK_LABELING
		rsp = smack_ipv6host_label(sap);
		if (rsp != NULL)
			rc = smk_ipv6_check(ssp->smk_out, rsp, sap,
						SMK_CONNECTING);
#endif
#ifdef SMACK_IPV6_PORT_LABELING
		rc = smk_ipv6_port_check(sock->sk, sap, SMK_SENDING);
#endif
#endif /* IS_ENABLED(CONFIG_IPV6) */
		break;
	}
	return rc;
}

/**
 * smack_from_secattr - Convert a netlabel attr.mls.lvl/attr.mls.cat pair to smack
 * @sap: netlabel secattr
 * @ssp: socket security information
 *
 * Returns a pointer to a Smack label entry found on the label list.
 */
static struct smack_known *smack_from_secattr(struct netlbl_lsm_secattr *sap,
						struct socket_smack *ssp)
{
	struct smack_known *skp;
	int found = 0;
	int acat;
	int kcat;

	/*
	 * Netlabel found it in the cache.
	 */
	if ((sap->flags & NETLBL_SECATTR_CACHE) != 0)
		return (struct smack_known *)sap->cache->data;

	if ((sap->flags & NETLBL_SECATTR_SECID) != 0)
		/*
		 * Looks like a fallback, which gives us a secid.
		 */
		return smack_from_secid(sap->attr.secid);

	if ((sap->flags & NETLBL_SECATTR_MLS_LVL) != 0) {
		/*
		 * Looks like a CIPSO packet.
		 * If there are flags but no level netlabel isn't
		 * behaving the way we expect it to.
		 *
		 * Look it up in the label table
		 * Without guidance regarding the smack value
		 * for the packet fall back on the network
		 * ambient value.
		 */
		rcu_read_lock();
		list_for_each_entry_rcu(skp, &smack_known_list, list) {
			if (sap->attr.mls.lvl != skp->smk_netlabel.attr.mls.lvl)
				continue;
			/*
			 * Compare the catsets. Use the netlbl APIs.
			 */
			if ((sap->flags & NETLBL_SECATTR_MLS_CAT) == 0) {
				if ((skp->smk_netlabel.flags &
				     NETLBL_SECATTR_MLS_CAT) == 0)
					found = 1;
				break;
			}
			for (acat = -1, kcat = -1; acat == kcat; ) {
				acat = netlbl_catmap_walk(sap->attr.mls.cat,
							  acat + 1);
				kcat = netlbl_catmap_walk(
					skp->smk_netlabel.attr.mls.cat,
					kcat + 1);
				if (acat < 0 || kcat < 0)
					break;
			}
			if (acat == kcat) {
				found = 1;
				break;
			}
		}
		rcu_read_unlock();

		if (found)
			return skp;

		if (ssp != NULL && ssp->smk_in == &smack_known_star)
			return &smack_known_web;
		return &smack_known_star;
	}
	/*
	 * Without guidance regarding the smack value
	 * for the packet fall back on the network
	 * ambient value.
	 */
	return smack_net_ambient;
}

#if IS_ENABLED(CONFIG_IPV6)
static int smk_skb_to_addr_ipv6(struct sk_buff *skb, struct sockaddr_in6 *sip)
{
	u8 nexthdr;
	int offset;
	int proto = -EINVAL;
	struct ipv6hdr _ipv6h;
	struct ipv6hdr *ip6;
	__be16 frag_off;
	struct tcphdr _tcph, *th;
	struct udphdr _udph, *uh;
	struct dccp_hdr _dccph, *dh;

	sip->sin6_port = 0;

	offset = skb_network_offset(skb);
	ip6 = skb_header_pointer(skb, offset, sizeof(_ipv6h), &_ipv6h);
	if (ip6 == NULL)
		return -EINVAL;
	sip->sin6_addr = ip6->saddr;

	nexthdr = ip6->nexthdr;
	offset += sizeof(_ipv6h);
	offset = ipv6_skip_exthdr(skb, offset, &nexthdr, &frag_off);
	if (offset < 0)
		return -EINVAL;

	proto = nexthdr;
	switch (proto) {
	case IPPROTO_TCP:
		th = skb_header_pointer(skb, offset, sizeof(_tcph), &_tcph);
		if (th != NULL)
			sip->sin6_port = th->source;
		break;
	case IPPROTO_UDP:
	case IPPROTO_UDPLITE:
		uh = skb_header_pointer(skb, offset, sizeof(_udph), &_udph);
		if (uh != NULL)
			sip->sin6_port = uh->source;
		break;
	case IPPROTO_DCCP:
		dh = skb_header_pointer(skb, offset, sizeof(_dccph), &_dccph);
		if (dh != NULL)
			sip->sin6_port = dh->dccph_sport;
		break;
	}
	return proto;
}
#endif /* CONFIG_IPV6 */

/**
 * smack_from_skb - Smack data from the secmark in an skb
 * @skb: packet
 *
 * Returns smack_known of the secmark or NULL if that won't work.
 */
#ifdef CONFIG_NETWORK_SECMARK
static struct smack_known *smack_from_skb(struct sk_buff *skb)
{
	if (skb == NULL || skb->secmark == 0)
		return NULL;

	return smack_from_secid(skb->secmark);
}
#else
static inline struct smack_known *smack_from_skb(struct sk_buff *skb)
{
	return NULL;
}
#endif

/**
 * smack_from_netlbl - Smack data from the IP options in an skb
 * @sk: socket data came in on
 * @family: address family
 * @skb: packet
 *
 * Find the Smack label in the IP options. If it hasn't been
 * added to the netlabel cache, add it here.
 *
 * Returns smack_known of the IP options or NULL if that won't work.
 */
static struct smack_known *smack_from_netlbl(const struct sock *sk, u16 family,
					     struct sk_buff *skb)
{
	struct netlbl_lsm_secattr secattr;
	struct socket_smack *ssp = NULL;
	struct smack_known *skp = NULL;

	netlbl_secattr_init(&secattr);

	if (sk)
		ssp = sk->sk_security;

	if (netlbl_skbuff_getattr(skb, family, &secattr) == 0) {
		skp = smack_from_secattr(&secattr, ssp);
		if (secattr.flags & NETLBL_SECATTR_CACHEABLE)
			netlbl_cache_add(skb, family, &skp->smk_netlabel);
	}

	netlbl_secattr_destroy(&secattr);

	return skp;
}

/**
 * smack_socket_sock_rcv_skb - Smack packet delivery access check
 * @sk: socket
 * @skb: packet
 *
 * Returns 0 if the packet should be delivered, an error code otherwise
 */
static int smack_socket_sock_rcv_skb(struct sock *sk, struct sk_buff *skb)
{
	struct socket_smack *ssp = sk->sk_security;
	struct smack_known *skp = NULL;
	int rc = 0;
	struct smk_audit_info ad;
	u16 family = sk->sk_family;
#ifdef CONFIG_AUDIT
	struct lsm_network_audit net;
#endif
#if IS_ENABLED(CONFIG_IPV6)
	struct sockaddr_in6 sadd;
	int proto;

	if (family == PF_INET6 && skb->protocol == htons(ETH_P_IP))
		family = PF_INET;
#endif /* CONFIG_IPV6 */

	switch (family) {
	case PF_INET:
		/*
		 * If there is a secmark use it rather than the CIPSO label.
		 * If there is no secmark fall back to CIPSO.
		 * The secmark is assumed to reflect policy better.
		 */
		skp = smack_from_skb(skb);
		if (skp == NULL) {
			skp = smack_from_netlbl(sk, family, skb);
			if (skp == NULL)
				skp = smack_net_ambient;
		}

#ifdef CONFIG_AUDIT
		smk_ad_init_net(&ad, __func__, LSM_AUDIT_DATA_NET, &net);
		ad.a.u.net->family = family;
		ad.a.u.net->netif = skb->skb_iif;
		ipv4_skb_to_auditdata(skb, &ad.a, NULL);
#endif
		/*
		 * Receiving a packet requires that the other end
		 * be able to write here. Read access is not required.
		 * This is the simplist possible security model
		 * for networking.
		 */
		rc = smk_access(skp, ssp->smk_in, MAY_WRITE, &ad);
		rc = smk_bu_note("IPv4 delivery", skp, ssp->smk_in,
					MAY_WRITE, rc);
		if (rc != 0)
			netlbl_skbuff_err(skb, family, rc, 0);
		break;
#if IS_ENABLED(CONFIG_IPV6)
	case PF_INET6:
		proto = smk_skb_to_addr_ipv6(skb, &sadd);
		if (proto != IPPROTO_UDP && proto != IPPROTO_UDPLITE &&
		    proto != IPPROTO_TCP && proto != IPPROTO_DCCP)
			break;
#ifdef SMACK_IPV6_SECMARK_LABELING
		skp = smack_from_skb(skb);
		if (skp == NULL) {
			if (smk_ipv6_localhost(&sadd))
				break;
			skp = smack_ipv6host_label(&sadd);
			if (skp == NULL)
				skp = smack_net_ambient;
		}
#ifdef CONFIG_AUDIT
		smk_ad_init_net(&ad, __func__, LSM_AUDIT_DATA_NET, &net);
		ad.a.u.net->family = family;
		ad.a.u.net->netif = skb->skb_iif;
		ipv6_skb_to_auditdata(skb, &ad.a, NULL);
#endif /* CONFIG_AUDIT */
		rc = smk_access(skp, ssp->smk_in, MAY_WRITE, &ad);
		rc = smk_bu_note("IPv6 delivery", skp, ssp->smk_in,
					MAY_WRITE, rc);
#endif /* SMACK_IPV6_SECMARK_LABELING */
#ifdef SMACK_IPV6_PORT_LABELING
		rc = smk_ipv6_port_check(sk, &sadd, SMK_RECEIVING);
#endif /* SMACK_IPV6_PORT_LABELING */
		if (rc != 0)
			icmpv6_send(skb, ICMPV6_DEST_UNREACH,
					ICMPV6_ADM_PROHIBITED, 0);
		break;
#endif /* CONFIG_IPV6 */
	}

	return rc;
}

/**
 * smack_socket_getpeersec_stream - pull in packet label
 * @sock: the socket
 * @optval: user's destination
 * @optlen: size thereof
 * @len: max thereof
 *
 * returns zero on success, an error code otherwise
 */
static int smack_socket_getpeersec_stream(struct socket *sock,
					  char __user *optval,
					  int __user *optlen, unsigned len)
{
	struct socket_smack *ssp;
	char *rcp = "";
	int slen = 1;
	int rc = 0;

	ssp = sock->sk->sk_security;
	if (ssp->smk_packet != NULL) {
		rcp = ssp->smk_packet->smk_known;
		slen = strlen(rcp) + 1;
	}

	if (slen > len)
		rc = -ERANGE;
	else if (copy_to_user(optval, rcp, slen) != 0)
		rc = -EFAULT;

	if (put_user(slen, optlen) != 0)
		rc = -EFAULT;

	return rc;
}


/**
 * smack_socket_getpeersec_dgram - pull in packet label
 * @sock: the peer socket
 * @skb: packet data
 * @secid: pointer to where to put the secid of the packet
 *
 * Sets the netlabel socket state on sk from parent
 */
static int smack_socket_getpeersec_dgram(struct socket *sock,
					 struct sk_buff *skb, u32 *secid)

{
	struct socket_smack *ssp = NULL;
	struct smack_known *skp;
	struct sock *sk = NULL;
	int family = PF_UNSPEC;
	u32 s = 0;	/* 0 is the invalid secid */

	if (skb != NULL) {
		if (skb->protocol == htons(ETH_P_IP))
			family = PF_INET;
#if IS_ENABLED(CONFIG_IPV6)
		else if (skb->protocol == htons(ETH_P_IPV6))
			family = PF_INET6;
#endif /* CONFIG_IPV6 */
	}
	if (family == PF_UNSPEC && sock != NULL)
		family = sock->sk->sk_family;

	switch (family) {
	case PF_UNIX:
		ssp = sock->sk->sk_security;
		s = ssp->smk_out->smk_secid;
		break;
	case PF_INET:
		skp = smack_from_skb(skb);
		if (skp) {
			s = skp->smk_secid;
			break;
		}
		/*
		 * Translate what netlabel gave us.
		 */
		if (sock != NULL)
			sk = sock->sk;
		skp = smack_from_netlbl(sk, family, skb);
		if (skp != NULL)
			s = skp->smk_secid;
		break;
	case PF_INET6:
#ifdef SMACK_IPV6_SECMARK_LABELING
		skp = smack_from_skb(skb);
		if (skp)
			s = skp->smk_secid;
#endif
		break;
	}
	*secid = s;
	if (s == 0)
		return -EINVAL;
	return 0;
}

/**
 * smack_sock_graft - Initialize a newly created socket with an existing sock
 * @sk: child sock
 * @parent: parent socket
 *
 * Set the smk_{in,out} state of an existing sock based on the process that
 * is creating the new socket.
 */
static void smack_sock_graft(struct sock *sk, struct socket *parent)
{
	struct socket_smack *ssp;
	struct smack_known *skp = smk_of_current();

	if (sk == NULL ||
	    (sk->sk_family != PF_INET && sk->sk_family != PF_INET6))
		return;

	ssp = sk->sk_security;
	ssp->smk_in = skp;
	ssp->smk_out = skp;
	/* cssp->smk_packet is already set in smack_inet_csk_clone() */
}

/**
 * smack_inet_conn_request - Smack access check on connect
 * @sk: socket involved
 * @skb: packet
 * @req: unused
 *
 * Returns 0 if a task with the packet label could write to
 * the socket, otherwise an error code
 */
static int smack_inet_conn_request(const struct sock *sk, struct sk_buff *skb,
				   struct request_sock *req)
{
	u16 family = sk->sk_family;
	struct smack_known *skp;
	struct socket_smack *ssp = sk->sk_security;
	struct sockaddr_in addr;
	struct iphdr *hdr;
	struct smack_known *hskp;
	int rc;
	struct smk_audit_info ad;
#ifdef CONFIG_AUDIT
	struct lsm_network_audit net;
#endif

#if IS_ENABLED(CONFIG_IPV6)
	if (family == PF_INET6) {
		/*
		 * Handle mapped IPv4 packets arriving
		 * via IPv6 sockets. Don't set up netlabel
		 * processing on IPv6.
		 */
		if (skb->protocol == htons(ETH_P_IP))
			family = PF_INET;
		else
			return 0;
	}
#endif /* CONFIG_IPV6 */

	/*
	 * If there is a secmark use it rather than the CIPSO label.
	 * If there is no secmark fall back to CIPSO.
	 * The secmark is assumed to reflect policy better.
	 */
	skp = smack_from_skb(skb);
	if (skp == NULL) {
		skp = smack_from_netlbl(sk, family, skb);
		if (skp == NULL)
			skp = &smack_known_huh;
	}

#ifdef CONFIG_AUDIT
	smk_ad_init_net(&ad, __func__, LSM_AUDIT_DATA_NET, &net);
	ad.a.u.net->family = family;
	ad.a.u.net->netif = skb->skb_iif;
	ipv4_skb_to_auditdata(skb, &ad.a, NULL);
#endif
	/*
	 * Receiving a packet requires that the other end be able to write
	 * here. Read access is not required.
	 */
	rc = smk_access(skp, ssp->smk_in, MAY_WRITE, &ad);
	rc = smk_bu_note("IPv4 connect", skp, ssp->smk_in, MAY_WRITE, rc);
	if (rc != 0)
		return rc;

	/*
	 * Save the peer's label in the request_sock so we can later setup
	 * smk_packet in the child socket so that SO_PEERCRED can report it.
	 */
	req->peer_secid = skp->smk_secid;

	/*
	 * We need to decide if we want to label the incoming connection here
	 * if we do we only need to label the request_sock and the stack will
	 * propagate the wire-label to the sock when it is created.
	 */
	hdr = ip_hdr(skb);
	addr.sin_addr.s_addr = hdr->saddr;
	rcu_read_lock();
	hskp = smack_ipv4host_label(&addr);
	rcu_read_unlock();

	if (hskp == NULL)
		rc = netlbl_req_setattr(req, &skp->smk_netlabel);
	else
		netlbl_req_delattr(req);

	return rc;
}

/**
 * smack_inet_csk_clone - Copy the connection information to the new socket
 * @sk: the new socket
 * @req: the connection's request_sock
 *
 * Transfer the connection's peer label to the newly created socket.
 */
static void smack_inet_csk_clone(struct sock *sk,
				 const struct request_sock *req)
{
	struct socket_smack *ssp = sk->sk_security;
	struct smack_known *skp;

	if (req->peer_secid != 0) {
		skp = smack_from_secid(req->peer_secid);
		ssp->smk_packet = skp;
	} else
		ssp->smk_packet = NULL;
}

/*
 * Key management security hooks
 *
 * Casey has not tested key support very heavily.
 * The permission check is most likely too restrictive.
 * If you care about keys please have a look.
 */
#ifdef CONFIG_KEYS

/**
 * smack_key_alloc - Set the key security blob
 * @key: object
 * @cred: the credentials to use
 * @flags: unused
 *
 * No allocation required
 *
 * Returns 0
 */
static int smack_key_alloc(struct key *key, const struct cred *cred,
			   unsigned long flags)
{
	struct smack_known *skp = smk_of_task(smack_cred(cred));

	key->security = skp;
	return 0;
}

/**
 * smack_key_free - Clear the key security blob
 * @key: the object
 *
 * Clear the blob pointer
 */
static void smack_key_free(struct key *key)
{
	key->security = NULL;
}

/**
 * smack_key_permission - Smack access on a key
 * @key_ref: gets to the object
 * @cred: the credentials to use
 * @need_perm: requested key permission
 *
 * Return 0 if the task has read and write to the object,
 * an error code otherwise
 */
static int smack_key_permission(key_ref_t key_ref,
				const struct cred *cred,
				enum key_need_perm need_perm)
{
	struct key *keyp;
	struct smk_audit_info ad;
	struct smack_known *tkp = smk_of_task(smack_cred(cred));
	int request = 0;
	int rc;

	/*
	 * Validate requested permissions
	 */
	switch (need_perm) {
	case KEY_NEED_READ:
	case KEY_NEED_SEARCH:
	case KEY_NEED_VIEW:
		request |= MAY_READ;
		break;
	case KEY_NEED_WRITE:
	case KEY_NEED_LINK:
	case KEY_NEED_SETATTR:
		request |= MAY_WRITE;
		break;
	case KEY_NEED_UNSPECIFIED:
	case KEY_NEED_UNLINK:
	case KEY_SYSADMIN_OVERRIDE:
	case KEY_AUTHTOKEN_OVERRIDE:
	case KEY_DEFER_PERM_CHECK:
		return 0;
	default:
		return -EINVAL;
	}

	keyp = key_ref_to_ptr(key_ref);
	if (keyp == NULL)
		return -EINVAL;
	/*
	 * If the key hasn't been initialized give it access so that
	 * it may do so.
	 */
	if (keyp->security == NULL)
		return 0;
	/*
	 * This should not occur
	 */
	if (tkp == NULL)
		return -EACCES;

	if (smack_privileged(CAP_MAC_OVERRIDE))
		return 0;

#ifdef CONFIG_AUDIT
	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_KEY);
	ad.a.u.key_struct.key = keyp->serial;
	ad.a.u.key_struct.key_desc = keyp->description;
#endif
	rc = smk_access(tkp, keyp->security, request, &ad);
	rc = smk_bu_note("key access", tkp, keyp->security, request, rc);
	return rc;
}

/*
 * smack_key_getsecurity - Smack label tagging the key
 * @key points to the key to be queried
 * @_buffer points to a pointer that should be set to point to the
 * resulting string (if no label or an error occurs).
 * Return the length of the string (including terminating NUL) or -ve if
 * an error.
 * May also return 0 (and a NULL buffer pointer) if there is no label.
 */
static int smack_key_getsecurity(struct key *key, char **_buffer)
{
	struct smack_known *skp = key->security;
	size_t length;
	char *copy;

	if (key->security == NULL) {
		*_buffer = NULL;
		return 0;
	}

	copy = kstrdup(skp->smk_known, GFP_KERNEL);
	if (copy == NULL)
		return -ENOMEM;
	length = strlen(copy) + 1;

	*_buffer = copy;
	return length;
}


#ifdef CONFIG_KEY_NOTIFICATIONS
/**
 * smack_watch_key - Smack access to watch a key for notifications.
 * @key: The key to be watched
 *
 * Return 0 if the @watch->cred has permission to read from the key object and
 * an error otherwise.
 */
static int smack_watch_key(struct key *key)
{
	struct smk_audit_info ad;
	struct smack_known *tkp = smk_of_current();
	int rc;

	if (key == NULL)
		return -EINVAL;
	/*
	 * If the key hasn't been initialized give it access so that
	 * it may do so.
	 */
	if (key->security == NULL)
		return 0;
	/*
	 * This should not occur
	 */
	if (tkp == NULL)
		return -EACCES;

	if (smack_privileged_cred(CAP_MAC_OVERRIDE, current_cred()))
		return 0;

#ifdef CONFIG_AUDIT
	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_KEY);
	ad.a.u.key_struct.key = key->serial;
	ad.a.u.key_struct.key_desc = key->description;
#endif
	rc = smk_access(tkp, key->security, MAY_READ, &ad);
	rc = smk_bu_note("key watch", tkp, key->security, MAY_READ, rc);
	return rc;
}
#endif /* CONFIG_KEY_NOTIFICATIONS */
#endif /* CONFIG_KEYS */

#ifdef CONFIG_WATCH_QUEUE
/**
 * smack_post_notification - Smack access to post a notification to a queue
 * @w_cred: The credentials of the watcher.
 * @cred: The credentials of the event source (may be NULL).
 * @n: The notification message to be posted.
 */
static int smack_post_notification(const struct cred *w_cred,
				   const struct cred *cred,
				   struct watch_notification *n)
{
	struct smk_audit_info ad;
	struct smack_known *subj, *obj;
	int rc;

	/* Always let maintenance notifications through. */
	if (n->type == WATCH_TYPE_META)
		return 0;

	if (!cred)
		return 0;
	subj = smk_of_task(smack_cred(cred));
	obj = smk_of_task(smack_cred(w_cred));

	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_NOTIFICATION);
	rc = smk_access(subj, obj, MAY_WRITE, &ad);
	rc = smk_bu_note("notification", subj, obj, MAY_WRITE, rc);
	return rc;
}
#endif /* CONFIG_WATCH_QUEUE */

/*
 * Smack Audit hooks
 *
 * Audit requires a unique representation of each Smack specific
 * rule. This unique representation is used to distinguish the
 * object to be audited from remaining kernel objects and also
 * works as a glue between the audit hooks.
 *
 * Since repository entries are added but never deleted, we'll use
 * the smack_known label address related to the given audit rule as
 * the needed unique representation. This also better fits the smack
 * model where nearly everything is a label.
 */
#ifdef CONFIG_AUDIT

/**
 * smack_audit_rule_init - Initialize a smack audit rule
 * @field: audit rule fields given from user-space (audit.h)
 * @op: required testing operator (=, !=, >, <, ...)
 * @rulestr: smack label to be audited
 * @vrule: pointer to save our own audit rule representation
 *
 * Prepare to audit cases where (@field @op @rulestr) is true.
 * The label to be audited is created if necessay.
 */
static int smack_audit_rule_init(u32 field, u32 op, char *rulestr, void **vrule)
{
	struct smack_known *skp;
	char **rule = (char **)vrule;
	*rule = NULL;

	if (field != AUDIT_SUBJ_USER && field != AUDIT_OBJ_USER)
		return -EINVAL;

	if (op != Audit_equal && op != Audit_not_equal)
		return -EINVAL;

	skp = smk_import_entry(rulestr, 0);
	if (IS_ERR(skp))
		return PTR_ERR(skp);

	*rule = skp->smk_known;

	return 0;
}

/**
 * smack_audit_rule_known - Distinguish Smack audit rules
 * @krule: rule of interest, in Audit kernel representation format
 *
 * This is used to filter Smack rules from remaining Audit ones.
 * If it's proved that this rule belongs to us, the
 * audit_rule_match hook will be called to do the final judgement.
 */
static int smack_audit_rule_known(struct audit_krule *krule)
{
	struct audit_field *f;
	int i;

	for (i = 0; i < krule->field_count; i++) {
		f = &krule->fields[i];

		if (f->type == AUDIT_SUBJ_USER || f->type == AUDIT_OBJ_USER)
			return 1;
	}

	return 0;
}

/**
 * smack_audit_rule_match - Audit given object ?
 * @secid: security id for identifying the object to test
 * @field: audit rule flags given from user-space
 * @op: required testing operator
 * @vrule: smack internal rule presentation
 *
 * The core Audit hook. It's used to take the decision of
 * whether to audit or not to audit a given object.
 */
static int smack_audit_rule_match(u32 secid, u32 field, u32 op, void *vrule)
{
	struct smack_known *skp;
	char *rule = vrule;

	if (unlikely(!rule)) {
		WARN_ONCE(1, "Smack: missing rule\n");
		return -ENOENT;
	}

	if (field != AUDIT_SUBJ_USER && field != AUDIT_OBJ_USER)
		return 0;

	skp = smack_from_secid(secid);

	/*
	 * No need to do string comparisons. If a match occurs,
	 * both pointers will point to the same smack_known
	 * label.
	 */
	if (op == Audit_equal)
		return (rule == skp->smk_known);
	if (op == Audit_not_equal)
		return (rule != skp->smk_known);

	return 0;
}

/*
 * There is no need for a smack_audit_rule_free hook.
 * No memory was allocated.
 */

#endif /* CONFIG_AUDIT */

/**
 * smack_ismaclabel - check if xattr @name references a smack MAC label
 * @name: Full xattr name to check.
 */
static int smack_ismaclabel(const char *name)
{
	return (strcmp(name, XATTR_SMACK_SUFFIX) == 0);
}


/**
 * smack_secid_to_secctx - return the smack label for a secid
 * @secid: incoming integer
 * @secdata: destination
 * @seclen: how long it is
 *
 * Exists for networking code.
 */
static int smack_secid_to_secctx(u32 secid, char **secdata, u32 *seclen)
{
	struct smack_known *skp = smack_from_secid(secid);

	if (secdata)
		*secdata = skp->smk_known;
	*seclen = strlen(skp->smk_known);
	return 0;
}

/**
 * smack_secctx_to_secid - return the secid for a smack label
 * @secdata: smack label
 * @seclen: how long result is
 * @secid: outgoing integer
 *
 * Exists for audit and networking code.
 */
static int smack_secctx_to_secid(const char *secdata, u32 seclen, u32 *secid)
{
	struct smack_known *skp = smk_find_entry(secdata);

	if (skp)
		*secid = skp->smk_secid;
	else
		*secid = 0;
	return 0;
}

/*
 * There used to be a smack_release_secctx hook
 * that did nothing back when hooks were in a vector.
 * Now that there's a list such a hook adds cost.
 */

static int smack_inode_notifysecctx(struct inode *inode, void *ctx, u32 ctxlen)
{
	return smack_inode_setsecurity(inode, XATTR_SMACK_SUFFIX, ctx,
				       ctxlen, 0);
}

static int smack_inode_setsecctx(struct dentry *dentry, void *ctx, u32 ctxlen)
{
	return __vfs_setxattr_noperm(&init_user_ns, dentry, XATTR_NAME_SMACK,
				     ctx, ctxlen, 0);
}

static int smack_inode_getsecctx(struct inode *inode, void **ctx, u32 *ctxlen)
{
	struct smack_known *skp = smk_of_inode(inode);

	*ctx = skp->smk_known;
	*ctxlen = strlen(skp->smk_known);
	return 0;
}

static int smack_inode_copy_up(struct dentry *dentry, struct cred **new)
{

	struct task_smack *tsp;
	struct smack_known *skp;
	struct inode_smack *isp;
	struct cred *new_creds = *new;

	if (new_creds == NULL) {
		new_creds = prepare_creds();
		if (new_creds == NULL)
			return -ENOMEM;
	}

	tsp = smack_cred(new_creds);

	/*
	 * Get label from overlay inode and set it in create_sid
	 */
	isp = smack_inode(d_inode(dentry->d_parent));
	skp = isp->smk_inode;
	tsp->smk_task = skp;
	*new = new_creds;
	return 0;
}

static int smack_inode_copy_up_xattr(const char *name)
{
	/*
	 * Return 1 if this is the smack access Smack attribute.
	 */
	if (strcmp(name, XATTR_NAME_SMACK) == 0)
		return 1;

	return -EOPNOTSUPP;
}

static int smack_dentry_create_files_as(struct dentry *dentry, int mode,
					struct qstr *name,
					const struct cred *old,
					struct cred *new)
{
	struct task_smack *otsp = smack_cred(old);
	struct task_smack *ntsp = smack_cred(new);
	struct inode_smack *isp;
	int may;

	/*
	 * Use the process credential unless all of
	 * the transmuting criteria are met
	 */
	ntsp->smk_task = otsp->smk_task;

	/*
	 * the attribute of the containing directory
	 */
	isp = smack_inode(d_inode(dentry->d_parent));

	if (isp->smk_flags & SMK_INODE_TRANSMUTE) {
		rcu_read_lock();
		may = smk_access_entry(otsp->smk_task->smk_known,
				       isp->smk_inode->smk_known,
				       &otsp->smk_task->smk_rules);
		rcu_read_unlock();

		/*
		 * If the directory is transmuting and the rule
		 * providing access is transmuting use the containing
		 * directory label instead of the process label.
		 */
		if (may > 0 && (may & MAY_TRANSMUTE))
			ntsp->smk_task = isp->smk_inode;
	}
	return 0;
}

struct lsm_blob_sizes smack_blob_sizes __lsm_ro_after_init = {
	.lbs_cred = sizeof(struct task_smack),
	.lbs_file = sizeof(struct smack_known *),
	.lbs_inode = sizeof(struct inode_smack),
	.lbs_ipc = sizeof(struct smack_known *),
	.lbs_msg_msg = sizeof(struct smack_known *),
	.lbs_superblock = sizeof(struct superblock_smack),
};

static struct security_hook_list smack_hooks[] __lsm_ro_after_init = {
	LSM_HOOK_INIT(ptrace_access_check, smack_ptrace_access_check),
	LSM_HOOK_INIT(ptrace_traceme, smack_ptrace_traceme),
	LSM_HOOK_INIT(syslog, smack_syslog),

	LSM_HOOK_INIT(fs_context_dup, smack_fs_context_dup),
	LSM_HOOK_INIT(fs_context_parse_param, smack_fs_context_parse_param),

	LSM_HOOK_INIT(sb_alloc_security, smack_sb_alloc_security),
	LSM_HOOK_INIT(sb_free_mnt_opts, smack_free_mnt_opts),
	LSM_HOOK_INIT(sb_eat_lsm_opts, smack_sb_eat_lsm_opts),
	LSM_HOOK_INIT(sb_statfs, smack_sb_statfs),
	LSM_HOOK_INIT(sb_set_mnt_opts, smack_set_mnt_opts),

	LSM_HOOK_INIT(bprm_creds_for_exec, smack_bprm_creds_for_exec),

	LSM_HOOK_INIT(inode_alloc_security, smack_inode_alloc_security),
	LSM_HOOK_INIT(inode_init_security, smack_inode_init_security),
	LSM_HOOK_INIT(inode_link, smack_inode_link),
	LSM_HOOK_INIT(inode_unlink, smack_inode_unlink),
	LSM_HOOK_INIT(inode_rmdir, smack_inode_rmdir),
	LSM_HOOK_INIT(inode_rename, smack_inode_rename),
	LSM_HOOK_INIT(inode_permission, smack_inode_permission),
	LSM_HOOK_INIT(inode_setattr, smack_inode_setattr),
	LSM_HOOK_INIT(inode_getattr, smack_inode_getattr),
	LSM_HOOK_INIT(inode_setxattr, smack_inode_setxattr),
	LSM_HOOK_INIT(inode_post_setxattr, smack_inode_post_setxattr),
	LSM_HOOK_INIT(inode_getxattr, smack_inode_getxattr),
	LSM_HOOK_INIT(inode_removexattr, smack_inode_removexattr),
	LSM_HOOK_INIT(inode_getsecurity, smack_inode_getsecurity),
	LSM_HOOK_INIT(inode_setsecurity, smack_inode_setsecurity),
	LSM_HOOK_INIT(inode_listsecurity, smack_inode_listsecurity),
	LSM_HOOK_INIT(inode_getsecid, smack_inode_getsecid),

	LSM_HOOK_INIT(file_alloc_security, smack_file_alloc_security),
	LSM_HOOK_INIT(file_ioctl, smack_file_ioctl),
	LSM_HOOK_INIT(file_lock, smack_file_lock),
	LSM_HOOK_INIT(file_fcntl, smack_file_fcntl),
	LSM_HOOK_INIT(mmap_file, smack_mmap_file),
	LSM_HOOK_INIT(mmap_addr, cap_mmap_addr),
	LSM_HOOK_INIT(file_set_fowner, smack_file_set_fowner),
	LSM_HOOK_INIT(file_send_sigiotask, smack_file_send_sigiotask),
	LSM_HOOK_INIT(file_receive, smack_file_receive),

	LSM_HOOK_INIT(file_open, smack_file_open),

	LSM_HOOK_INIT(cred_alloc_blank, smack_cred_alloc_blank),
	LSM_HOOK_INIT(cred_free, smack_cred_free),
	LSM_HOOK_INIT(cred_prepare, smack_cred_prepare),
	LSM_HOOK_INIT(cred_transfer, smack_cred_transfer),
	LSM_HOOK_INIT(cred_getsecid, smack_cred_getsecid),
	LSM_HOOK_INIT(kernel_act_as, smack_kernel_act_as),
	LSM_HOOK_INIT(kernel_create_files_as, smack_kernel_create_files_as),
	LSM_HOOK_INIT(task_setpgid, smack_task_setpgid),
	LSM_HOOK_INIT(task_getpgid, smack_task_getpgid),
	LSM_HOOK_INIT(task_getsid, smack_task_getsid),
	LSM_HOOK_INIT(task_getsecid_subj, smack_task_getsecid_subj),
	LSM_HOOK_INIT(task_getsecid_obj, smack_task_getsecid_obj),
	LSM_HOOK_INIT(task_setnice, smack_task_setnice),
	LSM_HOOK_INIT(task_setioprio, smack_task_setioprio),
	LSM_HOOK_INIT(task_getioprio, smack_task_getioprio),
	LSM_HOOK_INIT(task_setscheduler, smack_task_setscheduler),
	LSM_HOOK_INIT(task_getscheduler, smack_task_getscheduler),
	LSM_HOOK_INIT(task_movememory, smack_task_movememory),
	LSM_HOOK_INIT(task_kill, smack_task_kill),
	LSM_HOOK_INIT(task_to_inode, smack_task_to_inode),

	LSM_HOOK_INIT(ipc_permission, smack_ipc_permission),
	LSM_HOOK_INIT(ipc_getsecid, smack_ipc_getsecid),

	LSM_HOOK_INIT(msg_msg_alloc_security, smack_msg_msg_alloc_security),

	LSM_HOOK_INIT(msg_queue_alloc_security, smack_ipc_alloc_security),
	LSM_HOOK_INIT(msg_queue_associate, smack_msg_queue_associate),
	LSM_HOOK_INIT(msg_queue_msgctl, smack_msg_queue_msgctl),
	LSM_HOOK_INIT(msg_queue_msgsnd, smack_msg_queue_msgsnd),
	LSM_HOOK_INIT(msg_queue_msgrcv, smack_msg_queue_msgrcv),

	LSM_HOOK_INIT(shm_alloc_security, smack_ipc_alloc_security),
	LSM_HOOK_INIT(shm_associate, smack_shm_associate),
	LSM_HOOK_INIT(shm_shmctl, smack_shm_shmctl),
	LSM_HOOK_INIT(shm_shmat, smack_shm_shmat),

	LSM_HOOK_INIT(sem_alloc_security, smack_ipc_alloc_security),
	LSM_HOOK_INIT(sem_associate, smack_sem_associate),
	LSM_HOOK_INIT(sem_semctl, smack_sem_semctl),
	LSM_HOOK_INIT(sem_semop, smack_sem_semop),

	LSM_HOOK_INIT(d_instantiate, smack_d_instantiate),

	LSM_HOOK_INIT(getprocattr, smack_getprocattr),
	LSM_HOOK_INIT(setprocattr, smack_setprocattr),

	LSM_HOOK_INIT(unix_stream_connect, smack_unix_stream_connect),
	LSM_HOOK_INIT(unix_may_send, smack_unix_may_send),

	LSM_HOOK_INIT(socket_post_create, smack_socket_post_create),
	LSM_HOOK_INIT(socket_socketpair, smack_socket_socketpair),
#ifdef SMACK_IPV6_PORT_LABELING
	LSM_HOOK_INIT(socket_bind, smack_socket_bind),
#endif
	LSM_HOOK_INIT(socket_connect, smack_socket_connect),
	LSM_HOOK_INIT(socket_sendmsg, smack_socket_sendmsg),
	LSM_HOOK_INIT(socket_sock_rcv_skb, smack_socket_sock_rcv_skb),
	LSM_HOOK_INIT(socket_getpeersec_stream, smack_socket_getpeersec_stream),
	LSM_HOOK_INIT(socket_getpeersec_dgram, smack_socket_getpeersec_dgram),
	LSM_HOOK_INIT(sk_alloc_security, smack_sk_alloc_security),
	LSM_HOOK_INIT(sk_free_security, smack_sk_free_security),
	LSM_HOOK_INIT(sock_graft, smack_sock_graft),
	LSM_HOOK_INIT(inet_conn_request, smack_inet_conn_request),
	LSM_HOOK_INIT(inet_csk_clone, smack_inet_csk_clone),

 /* key management security hooks */
#ifdef CONFIG_KEYS
	LSM_HOOK_INIT(key_alloc, smack_key_alloc),
	LSM_HOOK_INIT(key_free, smack_key_free),
	LSM_HOOK_INIT(key_permission, smack_key_permission),
	LSM_HOOK_INIT(key_getsecurity, smack_key_getsecurity),
#ifdef CONFIG_KEY_NOTIFICATIONS
	LSM_HOOK_INIT(watch_key, smack_watch_key),
#endif
#endif /* CONFIG_KEYS */

#ifdef CONFIG_WATCH_QUEUE
	LSM_HOOK_INIT(post_notification, smack_post_notification),
#endif

 /* Audit hooks */
#ifdef CONFIG_AUDIT
	LSM_HOOK_INIT(audit_rule_init, smack_audit_rule_init),
	LSM_HOOK_INIT(audit_rule_known, smack_audit_rule_known),
	LSM_HOOK_INIT(audit_rule_match, smack_audit_rule_match),
#endif /* CONFIG_AUDIT */

	LSM_HOOK_INIT(ismaclabel, smack_ismaclabel),
	LSM_HOOK_INIT(secid_to_secctx, smack_secid_to_secctx),
	LSM_HOOK_INIT(secctx_to_secid, smack_secctx_to_secid),
	LSM_HOOK_INIT(inode_notifysecctx, smack_inode_notifysecctx),
	LSM_HOOK_INIT(inode_setsecctx, smack_inode_setsecctx),
	LSM_HOOK_INIT(inode_getsecctx, smack_inode_getsecctx),
	LSM_HOOK_INIT(inode_copy_up, smack_inode_copy_up),
	LSM_HOOK_INIT(inode_copy_up_xattr, smack_inode_copy_up_xattr),
	LSM_HOOK_INIT(dentry_create_files_as, smack_dentry_create_files_as),
};


static __init void init_smack_known_list(void)
{
	/*
	 * Initialize rule list locks
	 */
	mutex_init(&smack_known_huh.smk_rules_lock);
	mutex_init(&smack_known_hat.smk_rules_lock);
	mutex_init(&smack_known_floor.smk_rules_lock);
	mutex_init(&smack_known_star.smk_rules_lock);
	mutex_init(&smack_known_web.smk_rules_lock);
	/*
	 * Initialize rule lists
	 */
	INIT_LIST_HEAD(&smack_known_huh.smk_rules);
	INIT_LIST_HEAD(&smack_known_hat.smk_rules);
	INIT_LIST_HEAD(&smack_known_star.smk_rules);
	INIT_LIST_HEAD(&smack_known_floor.smk_rules);
	INIT_LIST_HEAD(&smack_known_web.smk_rules);
	/*
	 * Create the known labels list
	 */
	smk_insert_entry(&smack_known_huh);
	smk_insert_entry(&smack_known_hat);
	smk_insert_entry(&smack_known_star);
	smk_insert_entry(&smack_known_floor);
	smk_insert_entry(&smack_known_web);
}

/**
 * smack_init - initialize the smack system
 *
 * Returns 0 on success, -ENOMEM is there's no memory
 */
static __init int smack_init(void)
{
	struct cred *cred = (struct cred *) current->cred;
	struct task_smack *tsp;

	smack_rule_cache = KMEM_CACHE(smack_rule, 0);
	if (!smack_rule_cache)
		return -ENOMEM;

	/*
	 * Set the security state for the initial task.
	 */
	tsp = smack_cred(cred);
	init_task_smack(tsp, &smack_known_floor, &smack_known_floor);

	/*
	 * Register with LSM
	 */
	security_add_hooks(smack_hooks, ARRAY_SIZE(smack_hooks), "smack");
	smack_enabled = 1;

	pr_info("Smack:  Initializing.\n");
#ifdef CONFIG_SECURITY_SMACK_NETFILTER
	pr_info("Smack:  Netfilter enabled.\n");
#endif
#ifdef SMACK_IPV6_PORT_LABELING
	pr_info("Smack:  IPv6 port labeling enabled.\n");
#endif
#ifdef SMACK_IPV6_SECMARK_LABELING
	pr_info("Smack:  IPv6 Netfilter enabled.\n");
#endif

	/* initialize the smack_known_list */
	init_smack_known_list();

	return 0;
}

/*
 * Smack requires early initialization in order to label
 * all processes and objects when they are created.
 */
DEFINE_LSM(smack) = {
	.name = "smack",
	.flags = LSM_FLAG_LEGACY_MAJOR | LSM_FLAG_EXCLUSIVE,
	.blobs = &smack_blob_sizes,
	.init = smack_init,
};
