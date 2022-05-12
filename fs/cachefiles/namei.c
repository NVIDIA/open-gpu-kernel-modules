// SPDX-License-Identifier: GPL-2.0-or-later
/* CacheFiles path walking and related routines
 *
 * Copyright (C) 2007 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 */

#include <linux/module.h>
#include <linux/sched.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/fsnotify.h>
#include <linux/quotaops.h>
#include <linux/xattr.h>
#include <linux/mount.h>
#include <linux/namei.h>
#include <linux/security.h>
#include <linux/slab.h>
#include "internal.h"

#define CACHEFILES_KEYBUF_SIZE 512

/*
 * dump debugging info about an object
 */
static noinline
void __cachefiles_printk_object(struct cachefiles_object *object,
				const char *prefix)
{
	struct fscache_cookie *cookie;
	const u8 *k;
	unsigned loop;

	pr_err("%sobject: OBJ%x\n", prefix, object->fscache.debug_id);
	pr_err("%sobjstate=%s fl=%lx wbusy=%x ev=%lx[%lx]\n",
	       prefix, object->fscache.state->name,
	       object->fscache.flags, work_busy(&object->fscache.work),
	       object->fscache.events, object->fscache.event_mask);
	pr_err("%sops=%u inp=%u exc=%u\n",
	       prefix, object->fscache.n_ops, object->fscache.n_in_progress,
	       object->fscache.n_exclusive);
	pr_err("%sparent=%p\n",
	       prefix, object->fscache.parent);

	spin_lock(&object->fscache.lock);
	cookie = object->fscache.cookie;
	if (cookie) {
		pr_err("%scookie=%p [pr=%p nd=%p fl=%lx]\n",
		       prefix,
		       object->fscache.cookie,
		       object->fscache.cookie->parent,
		       object->fscache.cookie->netfs_data,
		       object->fscache.cookie->flags);
		pr_err("%skey=[%u] '", prefix, cookie->key_len);
		k = (cookie->key_len <= sizeof(cookie->inline_key)) ?
			cookie->inline_key : cookie->key;
		for (loop = 0; loop < cookie->key_len; loop++)
			pr_cont("%02x", k[loop]);
		pr_cont("'\n");
	} else {
		pr_err("%scookie=NULL\n", prefix);
	}
	spin_unlock(&object->fscache.lock);
}

/*
 * dump debugging info about a pair of objects
 */
static noinline void cachefiles_printk_object(struct cachefiles_object *object,
					      struct cachefiles_object *xobject)
{
	if (object)
		__cachefiles_printk_object(object, "");
	if (xobject)
		__cachefiles_printk_object(xobject, "x");
}

/*
 * mark the owner of a dentry, if there is one, to indicate that that dentry
 * has been preemptively deleted
 * - the caller must hold the i_mutex on the dentry's parent as required to
 *   call vfs_unlink(), vfs_rmdir() or vfs_rename()
 */
static void cachefiles_mark_object_buried(struct cachefiles_cache *cache,
					  struct dentry *dentry,
					  enum fscache_why_object_killed why)
{
	struct cachefiles_object *object;
	struct rb_node *p;

	_enter(",'%pd'", dentry);

	write_lock(&cache->active_lock);

	p = cache->active_nodes.rb_node;
	while (p) {
		object = rb_entry(p, struct cachefiles_object, active_node);
		if (object->dentry > dentry)
			p = p->rb_left;
		else if (object->dentry < dentry)
			p = p->rb_right;
		else
			goto found_dentry;
	}

	write_unlock(&cache->active_lock);
	trace_cachefiles_mark_buried(NULL, dentry, why);
	_leave(" [no owner]");
	return;

	/* found the dentry for  */
found_dentry:
	kdebug("preemptive burial: OBJ%x [%s] %p",
	       object->fscache.debug_id,
	       object->fscache.state->name,
	       dentry);

	trace_cachefiles_mark_buried(object, dentry, why);

	if (fscache_object_is_live(&object->fscache)) {
		pr_err("\n");
		pr_err("Error: Can't preemptively bury live object\n");
		cachefiles_printk_object(object, NULL);
	} else {
		if (why != FSCACHE_OBJECT_IS_STALE)
			fscache_object_mark_killed(&object->fscache, why);
	}

	write_unlock(&cache->active_lock);
	_leave(" [owner marked]");
}

/*
 * record the fact that an object is now active
 */
static int cachefiles_mark_object_active(struct cachefiles_cache *cache,
					 struct cachefiles_object *object)
{
	struct cachefiles_object *xobject;
	struct rb_node **_p, *_parent = NULL;
	struct dentry *dentry;

	_enter(",%p", object);

try_again:
	write_lock(&cache->active_lock);

	dentry = object->dentry;
	trace_cachefiles_mark_active(object, dentry);

	if (test_and_set_bit(CACHEFILES_OBJECT_ACTIVE, &object->flags)) {
		pr_err("Error: Object already active\n");
		cachefiles_printk_object(object, NULL);
		BUG();
	}

	_p = &cache->active_nodes.rb_node;
	while (*_p) {
		_parent = *_p;
		xobject = rb_entry(_parent,
				   struct cachefiles_object, active_node);

		ASSERT(xobject != object);

		if (xobject->dentry > dentry)
			_p = &(*_p)->rb_left;
		else if (xobject->dentry < dentry)
			_p = &(*_p)->rb_right;
		else
			goto wait_for_old_object;
	}

	rb_link_node(&object->active_node, _parent, _p);
	rb_insert_color(&object->active_node, &cache->active_nodes);

	write_unlock(&cache->active_lock);
	_leave(" = 0");
	return 0;

	/* an old object from a previous incarnation is hogging the slot - we
	 * need to wait for it to be destroyed */
wait_for_old_object:
	trace_cachefiles_wait_active(object, dentry, xobject);
	clear_bit(CACHEFILES_OBJECT_ACTIVE, &object->flags);

	if (fscache_object_is_live(&xobject->fscache)) {
		pr_err("\n");
		pr_err("Error: Unexpected object collision\n");
		cachefiles_printk_object(object, xobject);
	}
	atomic_inc(&xobject->usage);
	write_unlock(&cache->active_lock);

	if (test_bit(CACHEFILES_OBJECT_ACTIVE, &xobject->flags)) {
		wait_queue_head_t *wq;

		signed long timeout = 60 * HZ;
		wait_queue_entry_t wait;
		bool requeue;

		/* if the object we're waiting for is queued for processing,
		 * then just put ourselves on the queue behind it */
		if (work_pending(&xobject->fscache.work)) {
			_debug("queue OBJ%x behind OBJ%x immediately",
			       object->fscache.debug_id,
			       xobject->fscache.debug_id);
			goto requeue;
		}

		/* otherwise we sleep until either the object we're waiting for
		 * is done, or the fscache_object is congested */
		wq = bit_waitqueue(&xobject->flags, CACHEFILES_OBJECT_ACTIVE);
		init_wait(&wait);
		requeue = false;
		do {
			prepare_to_wait(wq, &wait, TASK_UNINTERRUPTIBLE);
			if (!test_bit(CACHEFILES_OBJECT_ACTIVE, &xobject->flags))
				break;

			requeue = fscache_object_sleep_till_congested(&timeout);
		} while (timeout > 0 && !requeue);
		finish_wait(wq, &wait);

		if (requeue &&
		    test_bit(CACHEFILES_OBJECT_ACTIVE, &xobject->flags)) {
			_debug("queue OBJ%x behind OBJ%x after wait",
			       object->fscache.debug_id,
			       xobject->fscache.debug_id);
			goto requeue;
		}

		if (timeout <= 0) {
			pr_err("\n");
			pr_err("Error: Overlong wait for old active object to go away\n");
			cachefiles_printk_object(object, xobject);
			goto requeue;
		}
	}

	ASSERT(!test_bit(CACHEFILES_OBJECT_ACTIVE, &xobject->flags));

	cache->cache.ops->put_object(&xobject->fscache,
		(enum fscache_obj_ref_trace)cachefiles_obj_put_wait_retry);
	goto try_again;

requeue:
	cache->cache.ops->put_object(&xobject->fscache,
		(enum fscache_obj_ref_trace)cachefiles_obj_put_wait_timeo);
	_leave(" = -ETIMEDOUT");
	return -ETIMEDOUT;
}

/*
 * Mark an object as being inactive.
 */
void cachefiles_mark_object_inactive(struct cachefiles_cache *cache,
				     struct cachefiles_object *object,
				     blkcnt_t i_blocks)
{
	struct dentry *dentry = object->dentry;
	struct inode *inode = d_backing_inode(dentry);

	trace_cachefiles_mark_inactive(object, dentry, inode);

	write_lock(&cache->active_lock);
	rb_erase(&object->active_node, &cache->active_nodes);
	clear_bit(CACHEFILES_OBJECT_ACTIVE, &object->flags);
	write_unlock(&cache->active_lock);

	wake_up_bit(&object->flags, CACHEFILES_OBJECT_ACTIVE);

	/* This object can now be culled, so we need to let the daemon know
	 * that there is something it can remove if it needs to.
	 */
	atomic_long_add(i_blocks, &cache->b_released);
	if (atomic_inc_return(&cache->f_released))
		cachefiles_state_changed(cache);
}

/*
 * delete an object representation from the cache
 * - file backed objects are unlinked
 * - directory backed objects are stuffed into the graveyard for userspace to
 *   delete
 * - unlocks the directory mutex
 */
static int cachefiles_bury_object(struct cachefiles_cache *cache,
				  struct cachefiles_object *object,
				  struct dentry *dir,
				  struct dentry *rep,
				  bool preemptive,
				  enum fscache_why_object_killed why)
{
	struct dentry *grave, *trap;
	struct path path, path_to_graveyard;
	char nbuffer[8 + 8 + 1];
	int ret;

	_enter(",'%pd','%pd'", dir, rep);

	_debug("remove %p from %p", rep, dir);

	/* non-directories can just be unlinked */
	if (!d_is_dir(rep)) {
		_debug("unlink stale object");

		path.mnt = cache->mnt;
		path.dentry = dir;
		ret = security_path_unlink(&path, rep);
		if (ret < 0) {
			cachefiles_io_error(cache, "Unlink security error");
		} else {
			trace_cachefiles_unlink(object, rep, why);
			ret = vfs_unlink(&init_user_ns, d_inode(dir), rep,
					 NULL);

			if (preemptive)
				cachefiles_mark_object_buried(cache, rep, why);
		}

		inode_unlock(d_inode(dir));

		if (ret == -EIO)
			cachefiles_io_error(cache, "Unlink failed");

		_leave(" = %d", ret);
		return ret;
	}

	/* directories have to be moved to the graveyard */
	_debug("move stale object to graveyard");
	inode_unlock(d_inode(dir));

try_again:
	/* first step is to make up a grave dentry in the graveyard */
	sprintf(nbuffer, "%08x%08x",
		(uint32_t) ktime_get_real_seconds(),
		(uint32_t) atomic_inc_return(&cache->gravecounter));

	/* do the multiway lock magic */
	trap = lock_rename(cache->graveyard, dir);

	/* do some checks before getting the grave dentry */
	if (rep->d_parent != dir || IS_DEADDIR(d_inode(rep))) {
		/* the entry was probably culled when we dropped the parent dir
		 * lock */
		unlock_rename(cache->graveyard, dir);
		_leave(" = 0 [culled?]");
		return 0;
	}

	if (!d_can_lookup(cache->graveyard)) {
		unlock_rename(cache->graveyard, dir);
		cachefiles_io_error(cache, "Graveyard no longer a directory");
		return -EIO;
	}

	if (trap == rep) {
		unlock_rename(cache->graveyard, dir);
		cachefiles_io_error(cache, "May not make directory loop");
		return -EIO;
	}

	if (d_mountpoint(rep)) {
		unlock_rename(cache->graveyard, dir);
		cachefiles_io_error(cache, "Mountpoint in cache");
		return -EIO;
	}

	grave = lookup_one_len(nbuffer, cache->graveyard, strlen(nbuffer));
	if (IS_ERR(grave)) {
		unlock_rename(cache->graveyard, dir);

		if (PTR_ERR(grave) == -ENOMEM) {
			_leave(" = -ENOMEM");
			return -ENOMEM;
		}

		cachefiles_io_error(cache, "Lookup error %ld",
				    PTR_ERR(grave));
		return -EIO;
	}

	if (d_is_positive(grave)) {
		unlock_rename(cache->graveyard, dir);
		dput(grave);
		grave = NULL;
		cond_resched();
		goto try_again;
	}

	if (d_mountpoint(grave)) {
		unlock_rename(cache->graveyard, dir);
		dput(grave);
		cachefiles_io_error(cache, "Mountpoint in graveyard");
		return -EIO;
	}

	/* target should not be an ancestor of source */
	if (trap == grave) {
		unlock_rename(cache->graveyard, dir);
		dput(grave);
		cachefiles_io_error(cache, "May not make directory loop");
		return -EIO;
	}

	/* attempt the rename */
	path.mnt = cache->mnt;
	path.dentry = dir;
	path_to_graveyard.mnt = cache->mnt;
	path_to_graveyard.dentry = cache->graveyard;
	ret = security_path_rename(&path, rep, &path_to_graveyard, grave, 0);
	if (ret < 0) {
		cachefiles_io_error(cache, "Rename security error %d", ret);
	} else {
		struct renamedata rd = {
			.old_mnt_userns	= &init_user_ns,
			.old_dir	= d_inode(dir),
			.old_dentry	= rep,
			.new_mnt_userns	= &init_user_ns,
			.new_dir	= d_inode(cache->graveyard),
			.new_dentry	= grave,
		};
		trace_cachefiles_rename(object, rep, grave, why);
		ret = vfs_rename(&rd);
		if (ret != 0 && ret != -ENOMEM)
			cachefiles_io_error(cache,
					    "Rename failed with error %d", ret);

		if (preemptive)
			cachefiles_mark_object_buried(cache, rep, why);
	}

	unlock_rename(cache->graveyard, dir);
	dput(grave);
	_leave(" = 0");
	return 0;
}

/*
 * delete an object representation from the cache
 */
int cachefiles_delete_object(struct cachefiles_cache *cache,
			     struct cachefiles_object *object)
{
	struct dentry *dir;
	int ret;

	_enter(",OBJ%x{%p}", object->fscache.debug_id, object->dentry);

	ASSERT(object->dentry);
	ASSERT(d_backing_inode(object->dentry));
	ASSERT(object->dentry->d_parent);

	dir = dget_parent(object->dentry);

	inode_lock_nested(d_inode(dir), I_MUTEX_PARENT);

	if (test_bit(FSCACHE_OBJECT_KILLED_BY_CACHE, &object->fscache.flags)) {
		/* object allocation for the same key preemptively deleted this
		 * object's file so that it could create its own file */
		_debug("object preemptively buried");
		inode_unlock(d_inode(dir));
		ret = 0;
	} else {
		/* we need to check that our parent is _still_ our parent - it
		 * may have been renamed */
		if (dir == object->dentry->d_parent) {
			ret = cachefiles_bury_object(cache, object, dir,
						     object->dentry, false,
						     FSCACHE_OBJECT_WAS_RETIRED);
		} else {
			/* it got moved, presumably by cachefilesd culling it,
			 * so it's no longer in the key path and we can ignore
			 * it */
			inode_unlock(d_inode(dir));
			ret = 0;
		}
	}

	dput(dir);
	_leave(" = %d", ret);
	return ret;
}

/*
 * walk from the parent object to the child object through the backing
 * filesystem, creating directories as we go
 */
int cachefiles_walk_to_object(struct cachefiles_object *parent,
			      struct cachefiles_object *object,
			      const char *key,
			      struct cachefiles_xattr *auxdata)
{
	struct cachefiles_cache *cache;
	struct dentry *dir, *next = NULL;
	struct inode *inode;
	struct path path;
	unsigned long start;
	const char *name;
	int ret, nlen;

	_enter("OBJ%x{%p},OBJ%x,%s,",
	       parent->fscache.debug_id, parent->dentry,
	       object->fscache.debug_id, key);

	cache = container_of(parent->fscache.cache,
			     struct cachefiles_cache, cache);
	path.mnt = cache->mnt;

	ASSERT(parent->dentry);
	ASSERT(d_backing_inode(parent->dentry));

	if (!(d_is_dir(parent->dentry))) {
		// TODO: convert file to dir
		_leave("looking up in none directory");
		return -ENOBUFS;
	}

	dir = dget(parent->dentry);

advance:
	/* attempt to transit the first directory component */
	name = key;
	nlen = strlen(key);

	/* key ends in a double NUL */
	key = key + nlen + 1;
	if (!*key)
		key = NULL;

lookup_again:
	/* search the current directory for the element name */
	_debug("lookup '%s'", name);

	inode_lock_nested(d_inode(dir), I_MUTEX_PARENT);

	start = jiffies;
	next = lookup_one_len(name, dir, nlen);
	cachefiles_hist(cachefiles_lookup_histogram, start);
	if (IS_ERR(next)) {
		trace_cachefiles_lookup(object, next, NULL);
		goto lookup_error;
	}

	inode = d_backing_inode(next);
	trace_cachefiles_lookup(object, next, inode);
	_debug("next -> %p %s", next, inode ? "positive" : "negative");

	if (!key)
		object->new = !inode;

	/* if this element of the path doesn't exist, then the lookup phase
	 * failed, and we can release any readers in the certain knowledge that
	 * there's nothing for them to actually read */
	if (d_is_negative(next))
		fscache_object_lookup_negative(&object->fscache);

	/* we need to create the object if it's negative */
	if (key || object->type == FSCACHE_COOKIE_TYPE_INDEX) {
		/* index objects and intervening tree levels must be subdirs */
		if (d_is_negative(next)) {
			ret = cachefiles_has_space(cache, 1, 0);
			if (ret < 0)
				goto no_space_error;

			path.dentry = dir;
			ret = security_path_mkdir(&path, next, 0);
			if (ret < 0)
				goto create_error;
			start = jiffies;
			ret = vfs_mkdir(&init_user_ns, d_inode(dir), next, 0);
			cachefiles_hist(cachefiles_mkdir_histogram, start);
			if (!key)
				trace_cachefiles_mkdir(object, next, ret);
			if (ret < 0)
				goto create_error;

			if (unlikely(d_unhashed(next))) {
				dput(next);
				inode_unlock(d_inode(dir));
				goto lookup_again;
			}
			ASSERT(d_backing_inode(next));

			_debug("mkdir -> %p{%p{ino=%lu}}",
			       next, d_backing_inode(next), d_backing_inode(next)->i_ino);

		} else if (!d_can_lookup(next)) {
			pr_err("inode %lu is not a directory\n",
			       d_backing_inode(next)->i_ino);
			ret = -ENOBUFS;
			goto error;
		}

	} else {
		/* non-index objects start out life as files */
		if (d_is_negative(next)) {
			ret = cachefiles_has_space(cache, 1, 0);
			if (ret < 0)
				goto no_space_error;

			path.dentry = dir;
			ret = security_path_mknod(&path, next, S_IFREG, 0);
			if (ret < 0)
				goto create_error;
			start = jiffies;
			ret = vfs_create(&init_user_ns, d_inode(dir), next,
					 S_IFREG, true);
			cachefiles_hist(cachefiles_create_histogram, start);
			trace_cachefiles_create(object, next, ret);
			if (ret < 0)
				goto create_error;

			ASSERT(d_backing_inode(next));

			_debug("create -> %p{%p{ino=%lu}}",
			       next, d_backing_inode(next), d_backing_inode(next)->i_ino);

		} else if (!d_can_lookup(next) &&
			   !d_is_reg(next)
			   ) {
			pr_err("inode %lu is not a file or directory\n",
			       d_backing_inode(next)->i_ino);
			ret = -ENOBUFS;
			goto error;
		}
	}

	/* process the next component */
	if (key) {
		_debug("advance");
		inode_unlock(d_inode(dir));
		dput(dir);
		dir = next;
		next = NULL;
		goto advance;
	}

	/* we've found the object we were looking for */
	object->dentry = next;

	/* if we've found that the terminal object exists, then we need to
	 * check its attributes and delete it if it's out of date */
	if (!object->new) {
		_debug("validate '%pd'", next);

		ret = cachefiles_check_object_xattr(object, auxdata);
		if (ret == -ESTALE) {
			/* delete the object (the deleter drops the directory
			 * mutex) */
			object->dentry = NULL;

			ret = cachefiles_bury_object(cache, object, dir, next,
						     true,
						     FSCACHE_OBJECT_IS_STALE);
			dput(next);
			next = NULL;

			if (ret < 0)
				goto delete_error;

			_debug("redo lookup");
			fscache_object_retrying_stale(&object->fscache);
			goto lookup_again;
		}
	}

	/* note that we're now using this object */
	ret = cachefiles_mark_object_active(cache, object);

	inode_unlock(d_inode(dir));
	dput(dir);
	dir = NULL;

	if (ret == -ETIMEDOUT)
		goto mark_active_timed_out;

	_debug("=== OBTAINED_OBJECT ===");

	if (object->new) {
		/* attach data to a newly constructed terminal object */
		ret = cachefiles_set_object_xattr(object, auxdata);
		if (ret < 0)
			goto check_error;
	} else {
		/* always update the atime on an object we've just looked up
		 * (this is used to keep track of culling, and atimes are only
		 * updated by read, write and readdir but not lookup or
		 * open) */
		path.dentry = next;
		touch_atime(&path);
	}

	/* open a file interface onto a data file */
	if (object->type != FSCACHE_COOKIE_TYPE_INDEX) {
		if (d_is_reg(object->dentry)) {
			const struct address_space_operations *aops;

			ret = -EPERM;
			aops = d_backing_inode(object->dentry)->i_mapping->a_ops;
			if (!aops->bmap)
				goto check_error;
			if (object->dentry->d_sb->s_blocksize > PAGE_SIZE)
				goto check_error;

			object->backer = object->dentry;
		} else {
			BUG(); // TODO: open file in data-class subdir
		}
	}

	object->new = 0;
	fscache_obtained_object(&object->fscache);

	_leave(" = 0 [%lu]", d_backing_inode(object->dentry)->i_ino);
	return 0;

no_space_error:
	fscache_object_mark_killed(&object->fscache, FSCACHE_OBJECT_NO_SPACE);
create_error:
	_debug("create error %d", ret);
	if (ret == -EIO)
		cachefiles_io_error(cache, "Create/mkdir failed");
	goto error;

mark_active_timed_out:
	_debug("mark active timed out");
	goto release_dentry;

check_error:
	_debug("check error %d", ret);
	cachefiles_mark_object_inactive(
		cache, object, d_backing_inode(object->dentry)->i_blocks);
release_dentry:
	dput(object->dentry);
	object->dentry = NULL;
	goto error_out;

delete_error:
	_debug("delete error %d", ret);
	goto error_out2;

lookup_error:
	_debug("lookup error %ld", PTR_ERR(next));
	ret = PTR_ERR(next);
	if (ret == -EIO)
		cachefiles_io_error(cache, "Lookup failed");
	next = NULL;
error:
	inode_unlock(d_inode(dir));
	dput(next);
error_out2:
	dput(dir);
error_out:
	_leave(" = error %d", -ret);
	return ret;
}

/*
 * get a subdirectory
 */
struct dentry *cachefiles_get_directory(struct cachefiles_cache *cache,
					struct dentry *dir,
					const char *dirname)
{
	struct dentry *subdir;
	unsigned long start;
	struct path path;
	int ret;

	_enter(",,%s", dirname);

	/* search the current directory for the element name */
	inode_lock(d_inode(dir));

retry:
	start = jiffies;
	subdir = lookup_one_len(dirname, dir, strlen(dirname));
	cachefiles_hist(cachefiles_lookup_histogram, start);
	if (IS_ERR(subdir)) {
		if (PTR_ERR(subdir) == -ENOMEM)
			goto nomem_d_alloc;
		goto lookup_error;
	}

	_debug("subdir -> %p %s",
	       subdir, d_backing_inode(subdir) ? "positive" : "negative");

	/* we need to create the subdir if it doesn't exist yet */
	if (d_is_negative(subdir)) {
		ret = cachefiles_has_space(cache, 1, 0);
		if (ret < 0)
			goto mkdir_error;

		_debug("attempt mkdir");

		path.mnt = cache->mnt;
		path.dentry = dir;
		ret = security_path_mkdir(&path, subdir, 0700);
		if (ret < 0)
			goto mkdir_error;
		ret = vfs_mkdir(&init_user_ns, d_inode(dir), subdir, 0700);
		if (ret < 0)
			goto mkdir_error;

		if (unlikely(d_unhashed(subdir))) {
			dput(subdir);
			goto retry;
		}
		ASSERT(d_backing_inode(subdir));

		_debug("mkdir -> %p{%p{ino=%lu}}",
		       subdir,
		       d_backing_inode(subdir),
		       d_backing_inode(subdir)->i_ino);
	}

	inode_unlock(d_inode(dir));

	/* we need to make sure the subdir is a directory */
	ASSERT(d_backing_inode(subdir));

	if (!d_can_lookup(subdir)) {
		pr_err("%s is not a directory\n", dirname);
		ret = -EIO;
		goto check_error;
	}

	ret = -EPERM;
	if (!(d_backing_inode(subdir)->i_opflags & IOP_XATTR) ||
	    !d_backing_inode(subdir)->i_op->lookup ||
	    !d_backing_inode(subdir)->i_op->mkdir ||
	    !d_backing_inode(subdir)->i_op->create ||
	    !d_backing_inode(subdir)->i_op->rename ||
	    !d_backing_inode(subdir)->i_op->rmdir ||
	    !d_backing_inode(subdir)->i_op->unlink)
		goto check_error;

	_leave(" = [%lu]", d_backing_inode(subdir)->i_ino);
	return subdir;

check_error:
	dput(subdir);
	_leave(" = %d [check]", ret);
	return ERR_PTR(ret);

mkdir_error:
	inode_unlock(d_inode(dir));
	dput(subdir);
	pr_err("mkdir %s failed with error %d\n", dirname, ret);
	return ERR_PTR(ret);

lookup_error:
	inode_unlock(d_inode(dir));
	ret = PTR_ERR(subdir);
	pr_err("Lookup %s failed with error %d\n", dirname, ret);
	return ERR_PTR(ret);

nomem_d_alloc:
	inode_unlock(d_inode(dir));
	_leave(" = -ENOMEM");
	return ERR_PTR(-ENOMEM);
}

/*
 * find out if an object is in use or not
 * - if finds object and it's not in use:
 *   - returns a pointer to the object and a reference on it
 *   - returns with the directory locked
 */
static struct dentry *cachefiles_check_active(struct cachefiles_cache *cache,
					      struct dentry *dir,
					      char *filename)
{
	struct cachefiles_object *object;
	struct rb_node *_n;
	struct dentry *victim;
	unsigned long start;
	int ret;

	//_enter(",%pd/,%s",
	//       dir, filename);

	/* look up the victim */
	inode_lock_nested(d_inode(dir), I_MUTEX_PARENT);

	start = jiffies;
	victim = lookup_one_len(filename, dir, strlen(filename));
	cachefiles_hist(cachefiles_lookup_histogram, start);
	if (IS_ERR(victim))
		goto lookup_error;

	//_debug("victim -> %p %s",
	//       victim, d_backing_inode(victim) ? "positive" : "negative");

	/* if the object is no longer there then we probably retired the object
	 * at the netfs's request whilst the cull was in progress
	 */
	if (d_is_negative(victim)) {
		inode_unlock(d_inode(dir));
		dput(victim);
		_leave(" = -ENOENT [absent]");
		return ERR_PTR(-ENOENT);
	}

	/* check to see if we're using this object */
	read_lock(&cache->active_lock);

	_n = cache->active_nodes.rb_node;

	while (_n) {
		object = rb_entry(_n, struct cachefiles_object, active_node);

		if (object->dentry > victim)
			_n = _n->rb_left;
		else if (object->dentry < victim)
			_n = _n->rb_right;
		else
			goto object_in_use;
	}

	read_unlock(&cache->active_lock);

	//_leave(" = %p", victim);
	return victim;

object_in_use:
	read_unlock(&cache->active_lock);
	inode_unlock(d_inode(dir));
	dput(victim);
	//_leave(" = -EBUSY [in use]");
	return ERR_PTR(-EBUSY);

lookup_error:
	inode_unlock(d_inode(dir));
	ret = PTR_ERR(victim);
	if (ret == -ENOENT) {
		/* file or dir now absent - probably retired by netfs */
		_leave(" = -ESTALE [absent]");
		return ERR_PTR(-ESTALE);
	}

	if (ret == -EIO) {
		cachefiles_io_error(cache, "Lookup failed");
	} else if (ret != -ENOMEM) {
		pr_err("Internal error: %d\n", ret);
		ret = -EIO;
	}

	_leave(" = %d", ret);
	return ERR_PTR(ret);
}

/*
 * cull an object if it's not in use
 * - called only by cache manager daemon
 */
int cachefiles_cull(struct cachefiles_cache *cache, struct dentry *dir,
		    char *filename)
{
	struct dentry *victim;
	int ret;

	_enter(",%pd/,%s", dir, filename);

	victim = cachefiles_check_active(cache, dir, filename);
	if (IS_ERR(victim))
		return PTR_ERR(victim);

	_debug("victim -> %p %s",
	       victim, d_backing_inode(victim) ? "positive" : "negative");

	/* okay... the victim is not being used so we can cull it
	 * - start by marking it as stale
	 */
	_debug("victim is cullable");

	ret = cachefiles_remove_object_xattr(cache, victim);
	if (ret < 0)
		goto error_unlock;

	/*  actually remove the victim (drops the dir mutex) */
	_debug("bury");

	ret = cachefiles_bury_object(cache, NULL, dir, victim, false,
				     FSCACHE_OBJECT_WAS_CULLED);
	if (ret < 0)
		goto error;

	dput(victim);
	_leave(" = 0");
	return 0;

error_unlock:
	inode_unlock(d_inode(dir));
error:
	dput(victim);
	if (ret == -ENOENT) {
		/* file or dir now absent - probably retired by netfs */
		_leave(" = -ESTALE [absent]");
		return -ESTALE;
	}

	if (ret != -ENOMEM) {
		pr_err("Internal error: %d\n", ret);
		ret = -EIO;
	}

	_leave(" = %d", ret);
	return ret;
}

/*
 * find out if an object is in use or not
 * - called only by cache manager daemon
 * - returns -EBUSY or 0 to indicate whether an object is in use or not
 */
int cachefiles_check_in_use(struct cachefiles_cache *cache, struct dentry *dir,
			    char *filename)
{
	struct dentry *victim;

	//_enter(",%pd/,%s",
	//       dir, filename);

	victim = cachefiles_check_active(cache, dir, filename);
	if (IS_ERR(victim))
		return PTR_ERR(victim);

	inode_unlock(d_inode(dir));
	dput(victim);
	//_leave(" = 0");
	return 0;
}
