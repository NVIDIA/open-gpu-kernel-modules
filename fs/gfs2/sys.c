// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) Sistina Software, Inc.  1997-2003 All rights reserved.
 * Copyright (C) 2004-2006 Red Hat, Inc.  All rights reserved.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/sched.h>
#include <linux/cred.h>
#include <linux/spinlock.h>
#include <linux/completion.h>
#include <linux/buffer_head.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/uaccess.h>
#include <linux/gfs2_ondisk.h>
#include <linux/genhd.h>

#include "gfs2.h"
#include "incore.h"
#include "sys.h"
#include "super.h"
#include "glock.h"
#include "quota.h"
#include "util.h"
#include "glops.h"
#include "recovery.h"

struct gfs2_attr {
	struct attribute attr;
	ssize_t (*show)(struct gfs2_sbd *, char *);
	ssize_t (*store)(struct gfs2_sbd *, const char *, size_t);
};

static ssize_t gfs2_attr_show(struct kobject *kobj, struct attribute *attr,
			      char *buf)
{
	struct gfs2_sbd *sdp = container_of(kobj, struct gfs2_sbd, sd_kobj);
	struct gfs2_attr *a = container_of(attr, struct gfs2_attr, attr);
	return a->show ? a->show(sdp, buf) : 0;
}

static ssize_t gfs2_attr_store(struct kobject *kobj, struct attribute *attr,
			       const char *buf, size_t len)
{
	struct gfs2_sbd *sdp = container_of(kobj, struct gfs2_sbd, sd_kobj);
	struct gfs2_attr *a = container_of(attr, struct gfs2_attr, attr);
	return a->store ? a->store(sdp, buf, len) : len;
}

static const struct sysfs_ops gfs2_attr_ops = {
	.show  = gfs2_attr_show,
	.store = gfs2_attr_store,
};


static struct kset *gfs2_kset;

static ssize_t id_show(struct gfs2_sbd *sdp, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%u:%u\n",
			MAJOR(sdp->sd_vfs->s_dev), MINOR(sdp->sd_vfs->s_dev));
}

static ssize_t status_show(struct gfs2_sbd *sdp, char *buf)
{
	unsigned long f = sdp->sd_flags;
	ssize_t s;

	s = snprintf(buf, PAGE_SIZE,
		     "Journal Checked:          %d\n"
		     "Journal Live:             %d\n"
		     "Journal ID:               %d\n"
		     "Spectator:                %d\n"
		     "Withdrawn:                %d\n"
		     "No barriers:              %d\n"
		     "No recovery:              %d\n"
		     "Demote:                   %d\n"
		     "No Journal ID:            %d\n"
		     "Mounted RO:               %d\n"
		     "RO Recovery:              %d\n"
		     "Skip DLM Unlock:          %d\n"
		     "Force AIL Flush:          %d\n"
		     "FS Frozen:                %d\n"
		     "Withdrawing:              %d\n"
		     "Withdraw In Prog:         %d\n"
		     "Remote Withdraw:          %d\n"
		     "Withdraw Recovery:        %d\n"
		     "sd_log_error:             %d\n"
		     "sd_log_flush_lock:        %d\n"
		     "sd_log_num_revoke:        %u\n"
		     "sd_log_in_flight:         %d\n"
		     "sd_log_blks_needed:       %d\n"
		     "sd_log_blks_free:         %d\n"
		     "sd_log_flush_head:        %d\n"
		     "sd_log_flush_tail:        %d\n"
		     "sd_log_blks_reserved:     %d\n"
		     "sd_log_revokes_available: %d\n",
		     test_bit(SDF_JOURNAL_CHECKED, &f),
		     test_bit(SDF_JOURNAL_LIVE, &f),
		     (sdp->sd_jdesc ? sdp->sd_jdesc->jd_jid : 0),
		     (sdp->sd_args.ar_spectator ? 1 : 0),
		     test_bit(SDF_WITHDRAWN, &f),
		     test_bit(SDF_NOBARRIERS, &f),
		     test_bit(SDF_NORECOVERY, &f),
		     test_bit(SDF_DEMOTE, &f),
		     test_bit(SDF_NOJOURNALID, &f),
		     (sb_rdonly(sdp->sd_vfs) ? 1 : 0),
		     test_bit(SDF_RORECOVERY, &f),
		     test_bit(SDF_SKIP_DLM_UNLOCK, &f),
		     test_bit(SDF_FORCE_AIL_FLUSH, &f),
		     test_bit(SDF_FS_FROZEN, &f),
		     test_bit(SDF_WITHDRAWING, &f),
		     test_bit(SDF_WITHDRAW_IN_PROG, &f),
		     test_bit(SDF_REMOTE_WITHDRAW, &f),
		     test_bit(SDF_WITHDRAW_RECOVERY, &f),
		     sdp->sd_log_error,
		     rwsem_is_locked(&sdp->sd_log_flush_lock),
		     sdp->sd_log_num_revoke,
		     atomic_read(&sdp->sd_log_in_flight),
		     atomic_read(&sdp->sd_log_blks_needed),
		     atomic_read(&sdp->sd_log_blks_free),
		     sdp->sd_log_flush_head,
		     sdp->sd_log_flush_tail,
		     sdp->sd_log_blks_reserved,
		     atomic_read(&sdp->sd_log_revokes_available));
	return s;
}

static ssize_t fsname_show(struct gfs2_sbd *sdp, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%s\n", sdp->sd_fsname);
}

static ssize_t uuid_show(struct gfs2_sbd *sdp, char *buf)
{
	struct super_block *s = sdp->sd_vfs;

	buf[0] = '\0';
	if (uuid_is_null(&s->s_uuid))
		return 0;
	return snprintf(buf, PAGE_SIZE, "%pUB\n", &s->s_uuid);
}

static ssize_t freeze_show(struct gfs2_sbd *sdp, char *buf)
{
	struct super_block *sb = sdp->sd_vfs;
	int frozen = (sb->s_writers.frozen == SB_UNFROZEN) ? 0 : 1;

	return snprintf(buf, PAGE_SIZE, "%d\n", frozen);
}

static ssize_t freeze_store(struct gfs2_sbd *sdp, const char *buf, size_t len)
{
	int error, n;

	error = kstrtoint(buf, 0, &n);
	if (error)
		return error;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	switch (n) {
	case 0:
		error = thaw_super(sdp->sd_vfs);
		break;
	case 1:
		error = freeze_super(sdp->sd_vfs);
		break;
	default:
		return -EINVAL;
	}

	if (error) {
		fs_warn(sdp, "freeze %d error %d\n", n, error);
		return error;
	}

	return len;
}

static ssize_t withdraw_show(struct gfs2_sbd *sdp, char *buf)
{
	unsigned int b = gfs2_withdrawn(sdp);
	return snprintf(buf, PAGE_SIZE, "%u\n", b);
}

static ssize_t withdraw_store(struct gfs2_sbd *sdp, const char *buf, size_t len)
{
	int error, val;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	error = kstrtoint(buf, 0, &val);
	if (error)
		return error;

	if (val != 1)
		return -EINVAL;

	gfs2_lm(sdp, "withdrawing from cluster at user's request\n");
	gfs2_withdraw(sdp);

	return len;
}

static ssize_t statfs_sync_store(struct gfs2_sbd *sdp, const char *buf,
				 size_t len)
{
	int error, val;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	error = kstrtoint(buf, 0, &val);
	if (error)
		return error;

	if (val != 1)
		return -EINVAL;

	gfs2_statfs_sync(sdp->sd_vfs, 0);
	return len;
}

static ssize_t quota_sync_store(struct gfs2_sbd *sdp, const char *buf,
				size_t len)
{
	int error, val;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	error = kstrtoint(buf, 0, &val);
	if (error)
		return error;

	if (val != 1)
		return -EINVAL;

	gfs2_quota_sync(sdp->sd_vfs, 0);
	return len;
}

static ssize_t quota_refresh_user_store(struct gfs2_sbd *sdp, const char *buf,
					size_t len)
{
	struct kqid qid;
	int error;
	u32 id;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	error = kstrtou32(buf, 0, &id);
	if (error)
		return error;

	qid = make_kqid(current_user_ns(), USRQUOTA, id);
	if (!qid_valid(qid))
		return -EINVAL;

	error = gfs2_quota_refresh(sdp, qid);
	return error ? error : len;
}

static ssize_t quota_refresh_group_store(struct gfs2_sbd *sdp, const char *buf,
					 size_t len)
{
	struct kqid qid;
	int error;
	u32 id;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	error = kstrtou32(buf, 0, &id);
	if (error)
		return error;

	qid = make_kqid(current_user_ns(), GRPQUOTA, id);
	if (!qid_valid(qid))
		return -EINVAL;

	error = gfs2_quota_refresh(sdp, qid);
	return error ? error : len;
}

static ssize_t demote_rq_store(struct gfs2_sbd *sdp, const char *buf, size_t len)
{
	struct gfs2_glock *gl;
	const struct gfs2_glock_operations *glops;
	unsigned int glmode;
	unsigned int gltype;
	unsigned long long glnum;
	char mode[16];
	int rv;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	rv = sscanf(buf, "%u:%llu %15s", &gltype, &glnum,
		    mode);
	if (rv != 3)
		return -EINVAL;

	if (strcmp(mode, "EX") == 0)
		glmode = LM_ST_UNLOCKED;
	else if ((strcmp(mode, "CW") == 0) || (strcmp(mode, "DF") == 0))
		glmode = LM_ST_DEFERRED;
	else if ((strcmp(mode, "PR") == 0) || (strcmp(mode, "SH") == 0))
		glmode = LM_ST_SHARED;
	else
		return -EINVAL;

	if (gltype > LM_TYPE_JOURNAL)
		return -EINVAL;
	if (gltype == LM_TYPE_NONDISK && glnum == GFS2_FREEZE_LOCK)
		glops = &gfs2_freeze_glops;
	else
		glops = gfs2_glops_list[gltype];
	if (glops == NULL)
		return -EINVAL;
	if (!test_and_set_bit(SDF_DEMOTE, &sdp->sd_flags))
		fs_info(sdp, "demote interface used\n");
	rv = gfs2_glock_get(sdp, glnum, glops, 0, &gl);
	if (rv)
		return rv;
	gfs2_glock_cb(gl, glmode);
	gfs2_glock_put(gl);
	return len;
}


#define GFS2_ATTR(name, mode, show, store) \
static struct gfs2_attr gfs2_attr_##name = __ATTR(name, mode, show, store)

GFS2_ATTR(id,                  0444, id_show,       NULL);
GFS2_ATTR(fsname,              0444, fsname_show,   NULL);
GFS2_ATTR(uuid,                0444, uuid_show,     NULL);
GFS2_ATTR(freeze,              0644, freeze_show,   freeze_store);
GFS2_ATTR(withdraw,            0644, withdraw_show, withdraw_store);
GFS2_ATTR(statfs_sync,         0200, NULL,          statfs_sync_store);
GFS2_ATTR(quota_sync,          0200, NULL,          quota_sync_store);
GFS2_ATTR(quota_refresh_user,  0200, NULL,          quota_refresh_user_store);
GFS2_ATTR(quota_refresh_group, 0200, NULL,          quota_refresh_group_store);
GFS2_ATTR(demote_rq,           0200, NULL,	    demote_rq_store);
GFS2_ATTR(status,              0400, status_show,   NULL);

static struct attribute *gfs2_attrs[] = {
	&gfs2_attr_id.attr,
	&gfs2_attr_fsname.attr,
	&gfs2_attr_uuid.attr,
	&gfs2_attr_freeze.attr,
	&gfs2_attr_withdraw.attr,
	&gfs2_attr_statfs_sync.attr,
	&gfs2_attr_quota_sync.attr,
	&gfs2_attr_quota_refresh_user.attr,
	&gfs2_attr_quota_refresh_group.attr,
	&gfs2_attr_demote_rq.attr,
	&gfs2_attr_status.attr,
	NULL,
};
ATTRIBUTE_GROUPS(gfs2);

static void gfs2_sbd_release(struct kobject *kobj)
{
	struct gfs2_sbd *sdp = container_of(kobj, struct gfs2_sbd, sd_kobj);

	complete(&sdp->sd_kobj_unregister);
}

static struct kobj_type gfs2_ktype = {
	.release = gfs2_sbd_release,
	.default_groups = gfs2_groups,
	.sysfs_ops     = &gfs2_attr_ops,
};


/*
 * lock_module. Originally from lock_dlm
 */

static ssize_t proto_name_show(struct gfs2_sbd *sdp, char *buf)
{
	const struct lm_lockops *ops = sdp->sd_lockstruct.ls_ops;
	return sprintf(buf, "%s\n", ops->lm_proto_name);
}

static ssize_t block_show(struct gfs2_sbd *sdp, char *buf)
{
	struct lm_lockstruct *ls = &sdp->sd_lockstruct;
	ssize_t ret;
	int val = 0;

	if (test_bit(DFL_BLOCK_LOCKS, &ls->ls_recover_flags))
		val = 1;
	ret = sprintf(buf, "%d\n", val);
	return ret;
}

static ssize_t block_store(struct gfs2_sbd *sdp, const char *buf, size_t len)
{
	struct lm_lockstruct *ls = &sdp->sd_lockstruct;
	int ret, val;

	ret = kstrtoint(buf, 0, &val);
	if (ret)
		return ret;

	if (val == 1)
		set_bit(DFL_BLOCK_LOCKS, &ls->ls_recover_flags);
	else if (val == 0) {
		clear_bit(DFL_BLOCK_LOCKS, &ls->ls_recover_flags);
		smp_mb__after_atomic();
		gfs2_glock_thaw(sdp);
	} else {
		return -EINVAL;
	}
	return len;
}

static ssize_t wdack_show(struct gfs2_sbd *sdp, char *buf)
{
	int val = completion_done(&sdp->sd_wdack) ? 1 : 0;

	return sprintf(buf, "%d\n", val);
}

static ssize_t wdack_store(struct gfs2_sbd *sdp, const char *buf, size_t len)
{
	int ret, val;

	ret = kstrtoint(buf, 0, &val);
	if (ret)
		return ret;

	if ((val == 1) &&
	    !strcmp(sdp->sd_lockstruct.ls_ops->lm_proto_name, "lock_dlm"))
		complete(&sdp->sd_wdack);
	else
		return -EINVAL;
	return len;
}

static ssize_t lkfirst_show(struct gfs2_sbd *sdp, char *buf)
{
	struct lm_lockstruct *ls = &sdp->sd_lockstruct;
	return sprintf(buf, "%d\n", ls->ls_first);
}

static ssize_t lkfirst_store(struct gfs2_sbd *sdp, const char *buf, size_t len)
{
	unsigned first;
	int rv;

	rv = sscanf(buf, "%u", &first);
	if (rv != 1 || first > 1)
		return -EINVAL;
	rv = wait_for_completion_killable(&sdp->sd_locking_init);
	if (rv)
		return rv;
	spin_lock(&sdp->sd_jindex_spin);
	rv = -EBUSY;
	if (test_bit(SDF_NOJOURNALID, &sdp->sd_flags) == 0)
		goto out;
	rv = -EINVAL;
	if (sdp->sd_args.ar_spectator)
		goto out;
	if (sdp->sd_lockstruct.ls_ops->lm_mount == NULL)
		goto out;
	sdp->sd_lockstruct.ls_first = first;
	rv = 0;
out:
        spin_unlock(&sdp->sd_jindex_spin);
        return rv ? rv : len;
}

static ssize_t first_done_show(struct gfs2_sbd *sdp, char *buf)
{
	struct lm_lockstruct *ls = &sdp->sd_lockstruct;
	return sprintf(buf, "%d\n", !!test_bit(DFL_FIRST_MOUNT_DONE, &ls->ls_recover_flags));
}

int gfs2_recover_set(struct gfs2_sbd *sdp, unsigned jid)
{
	struct gfs2_jdesc *jd;
	int rv;

	/* Wait for our primary journal to be initialized */
	wait_for_completion(&sdp->sd_journal_ready);

	spin_lock(&sdp->sd_jindex_spin);
	rv = -EBUSY;
	/**
	 * If we're a spectator, we use journal0, but it's not really ours.
	 * So we need to wait for its recovery too. If we skip it we'd never
	 * queue work to the recovery workqueue, and so its completion would
	 * never clear the DFL_BLOCK_LOCKS flag, so all our locks would
	 * permanently stop working.
	 */
	if (!sdp->sd_jdesc)
		goto out;
	if (sdp->sd_jdesc->jd_jid == jid && !sdp->sd_args.ar_spectator)
		goto out;
	rv = -ENOENT;
	list_for_each_entry(jd, &sdp->sd_jindex_list, jd_list) {
		if (jd->jd_jid != jid && !sdp->sd_args.ar_spectator)
			continue;
		rv = gfs2_recover_journal(jd, false);
		break;
	}
out:
	spin_unlock(&sdp->sd_jindex_spin);
	return rv;
}

static ssize_t recover_store(struct gfs2_sbd *sdp, const char *buf, size_t len)
{
	unsigned jid;
	int rv;

	rv = sscanf(buf, "%u", &jid);
	if (rv != 1)
		return -EINVAL;

	if (test_bit(SDF_NORECOVERY, &sdp->sd_flags)) {
		rv = -ESHUTDOWN;
		goto out;
	}

	rv = gfs2_recover_set(sdp, jid);
out:
	return rv ? rv : len;
}

static ssize_t recover_done_show(struct gfs2_sbd *sdp, char *buf)
{
	struct lm_lockstruct *ls = &sdp->sd_lockstruct;
	return sprintf(buf, "%d\n", ls->ls_recover_jid_done);
}

static ssize_t recover_status_show(struct gfs2_sbd *sdp, char *buf)
{
	struct lm_lockstruct *ls = &sdp->sd_lockstruct;
	return sprintf(buf, "%d\n", ls->ls_recover_jid_status);
}

static ssize_t jid_show(struct gfs2_sbd *sdp, char *buf)
{
	return sprintf(buf, "%d\n", sdp->sd_lockstruct.ls_jid);
}

static ssize_t jid_store(struct gfs2_sbd *sdp, const char *buf, size_t len)
{
        int jid;
	int rv;

	rv = sscanf(buf, "%d", &jid);
	if (rv != 1)
		return -EINVAL;
	rv = wait_for_completion_killable(&sdp->sd_locking_init);
	if (rv)
		return rv;
	spin_lock(&sdp->sd_jindex_spin);
	rv = -EINVAL;
	if (sdp->sd_lockstruct.ls_ops->lm_mount == NULL)
		goto out;
	rv = -EBUSY;
	if (test_bit(SDF_NOJOURNALID, &sdp->sd_flags) == 0)
		goto out;
	rv = 0;
	if (sdp->sd_args.ar_spectator && jid > 0)
		rv = jid = -EINVAL;
	sdp->sd_lockstruct.ls_jid = jid;
	clear_bit(SDF_NOJOURNALID, &sdp->sd_flags);
	smp_mb__after_atomic();
	wake_up_bit(&sdp->sd_flags, SDF_NOJOURNALID);
out:
	spin_unlock(&sdp->sd_jindex_spin);
	return rv ? rv : len;
}

#define GDLM_ATTR(_name,_mode,_show,_store) \
static struct gfs2_attr gdlm_attr_##_name = __ATTR(_name,_mode,_show,_store)

GDLM_ATTR(proto_name,		0444, proto_name_show,		NULL);
GDLM_ATTR(block,		0644, block_show,		block_store);
GDLM_ATTR(withdraw,		0644, wdack_show,		wdack_store);
GDLM_ATTR(jid,			0644, jid_show,			jid_store);
GDLM_ATTR(first,		0644, lkfirst_show,		lkfirst_store);
GDLM_ATTR(first_done,		0444, first_done_show,		NULL);
GDLM_ATTR(recover,		0600, NULL,			recover_store);
GDLM_ATTR(recover_done,		0444, recover_done_show,	NULL);
GDLM_ATTR(recover_status,	0444, recover_status_show,	NULL);

static struct attribute *lock_module_attrs[] = {
	&gdlm_attr_proto_name.attr,
	&gdlm_attr_block.attr,
	&gdlm_attr_withdraw.attr,
	&gdlm_attr_jid.attr,
	&gdlm_attr_first.attr,
	&gdlm_attr_first_done.attr,
	&gdlm_attr_recover.attr,
	&gdlm_attr_recover_done.attr,
	&gdlm_attr_recover_status.attr,
	NULL,
};

/*
 * get and set struct gfs2_tune fields
 */

static ssize_t quota_scale_show(struct gfs2_sbd *sdp, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%u %u\n",
			sdp->sd_tune.gt_quota_scale_num,
			sdp->sd_tune.gt_quota_scale_den);
}

static ssize_t quota_scale_store(struct gfs2_sbd *sdp, const char *buf,
				 size_t len)
{
	struct gfs2_tune *gt = &sdp->sd_tune;
	unsigned int x, y;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	if (sscanf(buf, "%u %u", &x, &y) != 2 || !y)
		return -EINVAL;

	spin_lock(&gt->gt_spin);
	gt->gt_quota_scale_num = x;
	gt->gt_quota_scale_den = y;
	spin_unlock(&gt->gt_spin);
	return len;
}

static ssize_t tune_set(struct gfs2_sbd *sdp, unsigned int *field,
			int check_zero, const char *buf, size_t len)
{
	struct gfs2_tune *gt = &sdp->sd_tune;
	unsigned int x;
	int error;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	error = kstrtouint(buf, 0, &x);
	if (error)
		return error;

	if (check_zero && !x)
		return -EINVAL;

	spin_lock(&gt->gt_spin);
	*field = x;
	spin_unlock(&gt->gt_spin);
	return len;
}

#define TUNE_ATTR_3(name, show, store)                                        \
static struct gfs2_attr tune_attr_##name = __ATTR(name, 0644, show, store)

#define TUNE_ATTR_2(name, store)                                              \
static ssize_t name##_show(struct gfs2_sbd *sdp, char *buf)                   \
{                                                                             \
	return snprintf(buf, PAGE_SIZE, "%u\n", sdp->sd_tune.gt_##name);      \
}                                                                             \
TUNE_ATTR_3(name, name##_show, store)

#define TUNE_ATTR(name, check_zero)                                           \
static ssize_t name##_store(struct gfs2_sbd *sdp, const char *buf, size_t len)\
{                                                                             \
	return tune_set(sdp, &sdp->sd_tune.gt_##name, check_zero, buf, len);  \
}                                                                             \
TUNE_ATTR_2(name, name##_store)

TUNE_ATTR(quota_warn_period, 0);
TUNE_ATTR(quota_quantum, 0);
TUNE_ATTR(max_readahead, 0);
TUNE_ATTR(complain_secs, 0);
TUNE_ATTR(statfs_slow, 0);
TUNE_ATTR(new_files_jdata, 0);
TUNE_ATTR(statfs_quantum, 1);
TUNE_ATTR_3(quota_scale, quota_scale_show, quota_scale_store);

static struct attribute *tune_attrs[] = {
	&tune_attr_quota_warn_period.attr,
	&tune_attr_quota_quantum.attr,
	&tune_attr_max_readahead.attr,
	&tune_attr_complain_secs.attr,
	&tune_attr_statfs_slow.attr,
	&tune_attr_statfs_quantum.attr,
	&tune_attr_quota_scale.attr,
	&tune_attr_new_files_jdata.attr,
	NULL,
};

static const struct attribute_group tune_group = {
	.name = "tune",
	.attrs = tune_attrs,
};

static const struct attribute_group lock_module_group = {
	.name = "lock_module",
	.attrs = lock_module_attrs,
};

int gfs2_sys_fs_add(struct gfs2_sbd *sdp)
{
	struct super_block *sb = sdp->sd_vfs;
	int error;
	char ro[20];
	char spectator[20];
	char *envp[] = { ro, spectator, NULL };

	sprintf(ro, "RDONLY=%d", sb_rdonly(sb));
	sprintf(spectator, "SPECTATOR=%d", sdp->sd_args.ar_spectator ? 1 : 0);

	init_completion(&sdp->sd_kobj_unregister);
	sdp->sd_kobj.kset = gfs2_kset;
	error = kobject_init_and_add(&sdp->sd_kobj, &gfs2_ktype, NULL,
				     "%s", sdp->sd_table_name);
	if (error)
		goto fail_reg;

	error = sysfs_create_group(&sdp->sd_kobj, &tune_group);
	if (error)
		goto fail_reg;

	error = sysfs_create_group(&sdp->sd_kobj, &lock_module_group);
	if (error)
		goto fail_tune;

	error = sysfs_create_link(&sdp->sd_kobj,
				  &disk_to_dev(sb->s_bdev->bd_disk)->kobj,
				  "device");
	if (error)
		goto fail_lock_module;

	kobject_uevent_env(&sdp->sd_kobj, KOBJ_ADD, envp);
	return 0;

fail_lock_module:
	sysfs_remove_group(&sdp->sd_kobj, &lock_module_group);
fail_tune:
	sysfs_remove_group(&sdp->sd_kobj, &tune_group);
fail_reg:
	fs_err(sdp, "error %d adding sysfs files\n", error);
	kobject_put(&sdp->sd_kobj);
	wait_for_completion(&sdp->sd_kobj_unregister);
	sb->s_fs_info = NULL;
	return error;
}

void gfs2_sys_fs_del(struct gfs2_sbd *sdp)
{
	sysfs_remove_link(&sdp->sd_kobj, "device");
	sysfs_remove_group(&sdp->sd_kobj, &tune_group);
	sysfs_remove_group(&sdp->sd_kobj, &lock_module_group);
	kobject_put(&sdp->sd_kobj);
	wait_for_completion(&sdp->sd_kobj_unregister);
}

static int gfs2_uevent(struct kset *kset, struct kobject *kobj,
		       struct kobj_uevent_env *env)
{
	struct gfs2_sbd *sdp = container_of(kobj, struct gfs2_sbd, sd_kobj);
	struct super_block *s = sdp->sd_vfs;

	add_uevent_var(env, "LOCKTABLE=%s", sdp->sd_table_name);
	add_uevent_var(env, "LOCKPROTO=%s", sdp->sd_proto_name);
	if (!test_bit(SDF_NOJOURNALID, &sdp->sd_flags))
		add_uevent_var(env, "JOURNALID=%d", sdp->sd_lockstruct.ls_jid);
	if (!uuid_is_null(&s->s_uuid))
		add_uevent_var(env, "UUID=%pUB", &s->s_uuid);
	return 0;
}

static const struct kset_uevent_ops gfs2_uevent_ops = {
	.uevent = gfs2_uevent,
};

int gfs2_sys_init(void)
{
	gfs2_kset = kset_create_and_add("gfs2", &gfs2_uevent_ops, fs_kobj);
	if (!gfs2_kset)
		return -ENOMEM;
	return 0;
}

void gfs2_sys_uninit(void)
{
	kset_unregister(gfs2_kset);
}

