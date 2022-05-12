// SPDX-License-Identifier: GPL-2.0-or-later
/* audit_watch.c -- watching inodes
 *
 * Copyright 2003-2009 Red Hat, Inc.
 * Copyright 2005 Hewlett-Packard Development Company, L.P.
 * Copyright 2005 IBM Corporation
 */

#include <linux/file.h>
#include <linux/kernel.h>
#include <linux/audit.h>
#include <linux/kthread.h>
#include <linux/mutex.h>
#include <linux/fs.h>
#include <linux/fsnotify_backend.h>
#include <linux/namei.h>
#include <linux/netlink.h>
#include <linux/refcount.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/security.h>
#include "audit.h"

/*
 * Reference counting:
 *
 * audit_parent: lifetime is from audit_init_parent() to receipt of an FS_IGNORED
 * 	event.  Each audit_watch holds a reference to its associated parent.
 *
 * audit_watch: if added to lists, lifetime is from audit_init_watch() to
 * 	audit_remove_watch().  Additionally, an audit_watch may exist
 * 	temporarily to assist in searching existing filter data.  Each
 * 	audit_krule holds a reference to its associated watch.
 */

struct audit_watch {
	refcount_t		count;	/* reference count */
	dev_t			dev;	/* associated superblock device */
	char			*path;	/* insertion path */
	unsigned long		ino;	/* associated inode number */
	struct audit_parent	*parent; /* associated parent */
	struct list_head	wlist;	/* entry in parent->watches list */
	struct list_head	rules;	/* anchor for krule->rlist */
};

struct audit_parent {
	struct list_head	watches; /* anchor for audit_watch->wlist */
	struct fsnotify_mark mark; /* fsnotify mark on the inode */
};

/* fsnotify handle. */
static struct fsnotify_group *audit_watch_group;

/* fsnotify events we care about. */
#define AUDIT_FS_WATCH (FS_MOVE | FS_CREATE | FS_DELETE | FS_DELETE_SELF |\
			FS_MOVE_SELF | FS_UNMOUNT)

static void audit_free_parent(struct audit_parent *parent)
{
	WARN_ON(!list_empty(&parent->watches));
	kfree(parent);
}

static void audit_watch_free_mark(struct fsnotify_mark *entry)
{
	struct audit_parent *parent;

	parent = container_of(entry, struct audit_parent, mark);
	audit_free_parent(parent);
}

static void audit_get_parent(struct audit_parent *parent)
{
	if (likely(parent))
		fsnotify_get_mark(&parent->mark);
}

static void audit_put_parent(struct audit_parent *parent)
{
	if (likely(parent))
		fsnotify_put_mark(&parent->mark);
}

/*
 * Find and return the audit_parent on the given inode.  If found a reference
 * is taken on this parent.
 */
static inline struct audit_parent *audit_find_parent(struct inode *inode)
{
	struct audit_parent *parent = NULL;
	struct fsnotify_mark *entry;

	entry = fsnotify_find_mark(&inode->i_fsnotify_marks, audit_watch_group);
	if (entry)
		parent = container_of(entry, struct audit_parent, mark);

	return parent;
}

void audit_get_watch(struct audit_watch *watch)
{
	refcount_inc(&watch->count);
}

void audit_put_watch(struct audit_watch *watch)
{
	if (refcount_dec_and_test(&watch->count)) {
		WARN_ON(watch->parent);
		WARN_ON(!list_empty(&watch->rules));
		kfree(watch->path);
		kfree(watch);
	}
}

static void audit_remove_watch(struct audit_watch *watch)
{
	list_del(&watch->wlist);
	audit_put_parent(watch->parent);
	watch->parent = NULL;
	audit_put_watch(watch); /* match initial get */
}

char *audit_watch_path(struct audit_watch *watch)
{
	return watch->path;
}

int audit_watch_compare(struct audit_watch *watch, unsigned long ino, dev_t dev)
{
	return (watch->ino != AUDIT_INO_UNSET) &&
		(watch->ino == ino) &&
		(watch->dev == dev);
}

/* Initialize a parent watch entry. */
static struct audit_parent *audit_init_parent(struct path *path)
{
	struct inode *inode = d_backing_inode(path->dentry);
	struct audit_parent *parent;
	int ret;

	parent = kzalloc(sizeof(*parent), GFP_KERNEL);
	if (unlikely(!parent))
		return ERR_PTR(-ENOMEM);

	INIT_LIST_HEAD(&parent->watches);

	fsnotify_init_mark(&parent->mark, audit_watch_group);
	parent->mark.mask = AUDIT_FS_WATCH;
	ret = fsnotify_add_inode_mark(&parent->mark, inode, 0);
	if (ret < 0) {
		audit_free_parent(parent);
		return ERR_PTR(ret);
	}

	return parent;
}

/* Initialize a watch entry. */
static struct audit_watch *audit_init_watch(char *path)
{
	struct audit_watch *watch;

	watch = kzalloc(sizeof(*watch), GFP_KERNEL);
	if (unlikely(!watch))
		return ERR_PTR(-ENOMEM);

	INIT_LIST_HEAD(&watch->rules);
	refcount_set(&watch->count, 1);
	watch->path = path;
	watch->dev = AUDIT_DEV_UNSET;
	watch->ino = AUDIT_INO_UNSET;

	return watch;
}

/* Translate a watch string to kernel representation. */
int audit_to_watch(struct audit_krule *krule, char *path, int len, u32 op)
{
	struct audit_watch *watch;

	if (!audit_watch_group)
		return -EOPNOTSUPP;

	if (path[0] != '/' || path[len-1] == '/' ||
	    krule->listnr != AUDIT_FILTER_EXIT ||
	    op != Audit_equal ||
	    krule->inode_f || krule->watch || krule->tree)
		return -EINVAL;

	watch = audit_init_watch(path);
	if (IS_ERR(watch))
		return PTR_ERR(watch);

	krule->watch = watch;

	return 0;
}

/* Duplicate the given audit watch.  The new watch's rules list is initialized
 * to an empty list and wlist is undefined. */
static struct audit_watch *audit_dupe_watch(struct audit_watch *old)
{
	char *path;
	struct audit_watch *new;

	path = kstrdup(old->path, GFP_KERNEL);
	if (unlikely(!path))
		return ERR_PTR(-ENOMEM);

	new = audit_init_watch(path);
	if (IS_ERR(new)) {
		kfree(path);
		goto out;
	}

	new->dev = old->dev;
	new->ino = old->ino;
	audit_get_parent(old->parent);
	new->parent = old->parent;

out:
	return new;
}

static void audit_watch_log_rule_change(struct audit_krule *r, struct audit_watch *w, char *op)
{
	struct audit_buffer *ab;

	if (!audit_enabled)
		return;
	ab = audit_log_start(audit_context(), GFP_NOFS, AUDIT_CONFIG_CHANGE);
	if (!ab)
		return;
	audit_log_session_info(ab);
	audit_log_format(ab, "op=%s path=", op);
	audit_log_untrustedstring(ab, w->path);
	audit_log_key(ab, r->filterkey);
	audit_log_format(ab, " list=%d res=1", r->listnr);
	audit_log_end(ab);
}

/* Update inode info in audit rules based on filesystem event. */
static void audit_update_watch(struct audit_parent *parent,
			       const struct qstr *dname, dev_t dev,
			       unsigned long ino, unsigned invalidating)
{
	struct audit_watch *owatch, *nwatch, *nextw;
	struct audit_krule *r, *nextr;
	struct audit_entry *oentry, *nentry;

	mutex_lock(&audit_filter_mutex);
	/* Run all of the watches on this parent looking for the one that
	 * matches the given dname */
	list_for_each_entry_safe(owatch, nextw, &parent->watches, wlist) {
		if (audit_compare_dname_path(dname, owatch->path,
					     AUDIT_NAME_FULL))
			continue;

		/* If the update involves invalidating rules, do the inode-based
		 * filtering now, so we don't omit records. */
		if (invalidating && !audit_dummy_context())
			audit_filter_inodes(current, audit_context());

		/* updating ino will likely change which audit_hash_list we
		 * are on so we need a new watch for the new list */
		nwatch = audit_dupe_watch(owatch);
		if (IS_ERR(nwatch)) {
			mutex_unlock(&audit_filter_mutex);
			audit_panic("error updating watch, skipping");
			return;
		}
		nwatch->dev = dev;
		nwatch->ino = ino;

		list_for_each_entry_safe(r, nextr, &owatch->rules, rlist) {

			oentry = container_of(r, struct audit_entry, rule);
			list_del(&oentry->rule.rlist);
			list_del_rcu(&oentry->list);

			nentry = audit_dupe_rule(&oentry->rule);
			if (IS_ERR(nentry)) {
				list_del(&oentry->rule.list);
				audit_panic("error updating watch, removing");
			} else {
				int h = audit_hash_ino((u32)ino);

				/*
				 * nentry->rule.watch == oentry->rule.watch so
				 * we must drop that reference and set it to our
				 * new watch.
				 */
				audit_put_watch(nentry->rule.watch);
				audit_get_watch(nwatch);
				nentry->rule.watch = nwatch;
				list_add(&nentry->rule.rlist, &nwatch->rules);
				list_add_rcu(&nentry->list, &audit_inode_hash[h]);
				list_replace(&oentry->rule.list,
					     &nentry->rule.list);
			}
			if (oentry->rule.exe)
				audit_remove_mark(oentry->rule.exe);

			call_rcu(&oentry->rcu, audit_free_rule_rcu);
		}

		audit_remove_watch(owatch);
		goto add_watch_to_parent; /* event applies to a single watch */
	}
	mutex_unlock(&audit_filter_mutex);
	return;

add_watch_to_parent:
	list_add(&nwatch->wlist, &parent->watches);
	mutex_unlock(&audit_filter_mutex);
	return;
}

/* Remove all watches & rules associated with a parent that is going away. */
static void audit_remove_parent_watches(struct audit_parent *parent)
{
	struct audit_watch *w, *nextw;
	struct audit_krule *r, *nextr;
	struct audit_entry *e;

	mutex_lock(&audit_filter_mutex);
	list_for_each_entry_safe(w, nextw, &parent->watches, wlist) {
		list_for_each_entry_safe(r, nextr, &w->rules, rlist) {
			e = container_of(r, struct audit_entry, rule);
			audit_watch_log_rule_change(r, w, "remove_rule");
			if (e->rule.exe)
				audit_remove_mark(e->rule.exe);
			list_del(&r->rlist);
			list_del(&r->list);
			list_del_rcu(&e->list);
			call_rcu(&e->rcu, audit_free_rule_rcu);
		}
		audit_remove_watch(w);
	}
	mutex_unlock(&audit_filter_mutex);

	fsnotify_destroy_mark(&parent->mark, audit_watch_group);
}

/* Get path information necessary for adding watches. */
static int audit_get_nd(struct audit_watch *watch, struct path *parent)
{
	struct dentry *d = kern_path_locked(watch->path, parent);
	if (IS_ERR(d))
		return PTR_ERR(d);
	if (d_is_positive(d)) {
		/* update watch filter fields */
		watch->dev = d->d_sb->s_dev;
		watch->ino = d_backing_inode(d)->i_ino;
	}
	inode_unlock(d_backing_inode(parent->dentry));
	dput(d);
	return 0;
}

/* Associate the given rule with an existing parent.
 * Caller must hold audit_filter_mutex. */
static void audit_add_to_parent(struct audit_krule *krule,
				struct audit_parent *parent)
{
	struct audit_watch *w, *watch = krule->watch;
	int watch_found = 0;

	BUG_ON(!mutex_is_locked(&audit_filter_mutex));

	list_for_each_entry(w, &parent->watches, wlist) {
		if (strcmp(watch->path, w->path))
			continue;

		watch_found = 1;

		/* put krule's ref to temporary watch */
		audit_put_watch(watch);

		audit_get_watch(w);
		krule->watch = watch = w;

		audit_put_parent(parent);
		break;
	}

	if (!watch_found) {
		watch->parent = parent;

		audit_get_watch(watch);
		list_add(&watch->wlist, &parent->watches);
	}
	list_add(&krule->rlist, &watch->rules);
}

/* Find a matching watch entry, or add this one.
 * Caller must hold audit_filter_mutex. */
int audit_add_watch(struct audit_krule *krule, struct list_head **list)
{
	struct audit_watch *watch = krule->watch;
	struct audit_parent *parent;
	struct path parent_path;
	int h, ret = 0;

	/*
	 * When we will be calling audit_add_to_parent, krule->watch might have
	 * been updated and watch might have been freed.
	 * So we need to keep a reference of watch.
	 */
	audit_get_watch(watch);

	mutex_unlock(&audit_filter_mutex);

	/* Avoid calling path_lookup under audit_filter_mutex. */
	ret = audit_get_nd(watch, &parent_path);

	/* caller expects mutex locked */
	mutex_lock(&audit_filter_mutex);

	if (ret) {
		audit_put_watch(watch);
		return ret;
	}

	/* either find an old parent or attach a new one */
	parent = audit_find_parent(d_backing_inode(parent_path.dentry));
	if (!parent) {
		parent = audit_init_parent(&parent_path);
		if (IS_ERR(parent)) {
			ret = PTR_ERR(parent);
			goto error;
		}
	}

	audit_add_to_parent(krule, parent);

	h = audit_hash_ino((u32)watch->ino);
	*list = &audit_inode_hash[h];
error:
	path_put(&parent_path);
	audit_put_watch(watch);
	return ret;
}

void audit_remove_watch_rule(struct audit_krule *krule)
{
	struct audit_watch *watch = krule->watch;
	struct audit_parent *parent = watch->parent;

	list_del(&krule->rlist);

	if (list_empty(&watch->rules)) {
		/*
		 * audit_remove_watch() drops our reference to 'parent' which
		 * can get freed. Grab our own reference to be safe.
		 */
		audit_get_parent(parent);
		audit_remove_watch(watch);
		if (list_empty(&parent->watches))
			fsnotify_destroy_mark(&parent->mark, audit_watch_group);
		audit_put_parent(parent);
	}
}

/* Update watch data in audit rules based on fsnotify events. */
static int audit_watch_handle_event(struct fsnotify_mark *inode_mark, u32 mask,
				    struct inode *inode, struct inode *dir,
				    const struct qstr *dname, u32 cookie)
{
	struct audit_parent *parent;

	parent = container_of(inode_mark, struct audit_parent, mark);

	if (WARN_ON_ONCE(inode_mark->group != audit_watch_group) ||
	    WARN_ON_ONCE(!inode))
		return 0;

	if (mask & (FS_CREATE|FS_MOVED_TO) && inode)
		audit_update_watch(parent, dname, inode->i_sb->s_dev, inode->i_ino, 0);
	else if (mask & (FS_DELETE|FS_MOVED_FROM))
		audit_update_watch(parent, dname, AUDIT_DEV_UNSET, AUDIT_INO_UNSET, 1);
	else if (mask & (FS_DELETE_SELF|FS_UNMOUNT|FS_MOVE_SELF))
		audit_remove_parent_watches(parent);

	return 0;
}

static const struct fsnotify_ops audit_watch_fsnotify_ops = {
	.handle_inode_event =	audit_watch_handle_event,
	.free_mark =		audit_watch_free_mark,
};

static int __init audit_watch_init(void)
{
	audit_watch_group = fsnotify_alloc_group(&audit_watch_fsnotify_ops);
	if (IS_ERR(audit_watch_group)) {
		audit_watch_group = NULL;
		audit_panic("cannot create audit fsnotify group");
	}
	return 0;
}
device_initcall(audit_watch_init);

int audit_dupe_exe(struct audit_krule *new, struct audit_krule *old)
{
	struct audit_fsnotify_mark *audit_mark;
	char *pathname;

	pathname = kstrdup(audit_mark_path(old->exe), GFP_KERNEL);
	if (!pathname)
		return -ENOMEM;

	audit_mark = audit_alloc_mark(new, pathname, strlen(pathname));
	if (IS_ERR(audit_mark)) {
		kfree(pathname);
		return PTR_ERR(audit_mark);
	}
	new->exe = audit_mark;

	return 0;
}

int audit_exe_compare(struct task_struct *tsk, struct audit_fsnotify_mark *mark)
{
	struct file *exe_file;
	unsigned long ino;
	dev_t dev;

	exe_file = get_task_exe_file(tsk);
	if (!exe_file)
		return 0;
	ino = file_inode(exe_file)->i_ino;
	dev = file_inode(exe_file)->i_sb->s_dev;
	fput(exe_file);
	return audit_mark_compare(mark, ino, dev);
}
