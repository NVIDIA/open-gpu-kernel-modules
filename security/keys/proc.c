// SPDX-License-Identifier: GPL-2.0-or-later
/* procfs files for key database enumeration
 *
 * Copyright (C) 2004 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 */

#include <linux/init.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/errno.h>
#include "internal.h"

static void *proc_keys_start(struct seq_file *p, loff_t *_pos);
static void *proc_keys_next(struct seq_file *p, void *v, loff_t *_pos);
static void proc_keys_stop(struct seq_file *p, void *v);
static int proc_keys_show(struct seq_file *m, void *v);

static const struct seq_operations proc_keys_ops = {
	.start	= proc_keys_start,
	.next	= proc_keys_next,
	.stop	= proc_keys_stop,
	.show	= proc_keys_show,
};

static void *proc_key_users_start(struct seq_file *p, loff_t *_pos);
static void *proc_key_users_next(struct seq_file *p, void *v, loff_t *_pos);
static void proc_key_users_stop(struct seq_file *p, void *v);
static int proc_key_users_show(struct seq_file *m, void *v);

static const struct seq_operations proc_key_users_ops = {
	.start	= proc_key_users_start,
	.next	= proc_key_users_next,
	.stop	= proc_key_users_stop,
	.show	= proc_key_users_show,
};

/*
 * Declare the /proc files.
 */
static int __init key_proc_init(void)
{
	struct proc_dir_entry *p;

	p = proc_create_seq("keys", 0, NULL, &proc_keys_ops);
	if (!p)
		panic("Cannot create /proc/keys\n");

	p = proc_create_seq("key-users", 0, NULL, &proc_key_users_ops);
	if (!p)
		panic("Cannot create /proc/key-users\n");

	return 0;
}

__initcall(key_proc_init);

/*
 * Implement "/proc/keys" to provide a list of the keys on the system that
 * grant View permission to the caller.
 */
static struct rb_node *key_serial_next(struct seq_file *p, struct rb_node *n)
{
	struct user_namespace *user_ns = seq_user_ns(p);

	n = rb_next(n);
	while (n) {
		struct key *key = rb_entry(n, struct key, serial_node);
		if (kuid_has_mapping(user_ns, key->user->uid))
			break;
		n = rb_next(n);
	}
	return n;
}

static struct key *find_ge_key(struct seq_file *p, key_serial_t id)
{
	struct user_namespace *user_ns = seq_user_ns(p);
	struct rb_node *n = key_serial_tree.rb_node;
	struct key *minkey = NULL;

	while (n) {
		struct key *key = rb_entry(n, struct key, serial_node);
		if (id < key->serial) {
			if (!minkey || minkey->serial > key->serial)
				minkey = key;
			n = n->rb_left;
		} else if (id > key->serial) {
			n = n->rb_right;
		} else {
			minkey = key;
			break;
		}
		key = NULL;
	}

	if (!minkey)
		return NULL;

	for (;;) {
		if (kuid_has_mapping(user_ns, minkey->user->uid))
			return minkey;
		n = rb_next(&minkey->serial_node);
		if (!n)
			return NULL;
		minkey = rb_entry(n, struct key, serial_node);
	}
}

static void *proc_keys_start(struct seq_file *p, loff_t *_pos)
	__acquires(key_serial_lock)
{
	key_serial_t pos = *_pos;
	struct key *key;

	spin_lock(&key_serial_lock);

	if (*_pos > INT_MAX)
		return NULL;
	key = find_ge_key(p, pos);
	if (!key)
		return NULL;
	*_pos = key->serial;
	return &key->serial_node;
}

static inline key_serial_t key_node_serial(struct rb_node *n)
{
	struct key *key = rb_entry(n, struct key, serial_node);
	return key->serial;
}

static void *proc_keys_next(struct seq_file *p, void *v, loff_t *_pos)
{
	struct rb_node *n;

	n = key_serial_next(p, v);
	if (n)
		*_pos = key_node_serial(n);
	else
		(*_pos)++;
	return n;
}

static void proc_keys_stop(struct seq_file *p, void *v)
	__releases(key_serial_lock)
{
	spin_unlock(&key_serial_lock);
}

static int proc_keys_show(struct seq_file *m, void *v)
{
	struct rb_node *_p = v;
	struct key *key = rb_entry(_p, struct key, serial_node);
	unsigned long flags;
	key_ref_t key_ref, skey_ref;
	time64_t now, expiry;
	char xbuf[16];
	short state;
	u64 timo;
	int rc;

	struct keyring_search_context ctx = {
		.index_key		= key->index_key,
		.cred			= m->file->f_cred,
		.match_data.cmp		= lookup_user_key_possessed,
		.match_data.raw_data	= key,
		.match_data.lookup_type	= KEYRING_SEARCH_LOOKUP_DIRECT,
		.flags			= (KEYRING_SEARCH_NO_STATE_CHECK |
					   KEYRING_SEARCH_RECURSE),
	};

	key_ref = make_key_ref(key, 0);

	/* determine if the key is possessed by this process (a test we can
	 * skip if the key does not indicate the possessor can view it
	 */
	if (key->perm & KEY_POS_VIEW) {
		rcu_read_lock();
		skey_ref = search_cred_keyrings_rcu(&ctx);
		rcu_read_unlock();
		if (!IS_ERR(skey_ref)) {
			key_ref_put(skey_ref);
			key_ref = make_key_ref(key, 1);
		}
	}

	/* check whether the current task is allowed to view the key */
	rc = key_task_permission(key_ref, ctx.cred, KEY_NEED_VIEW);
	if (rc < 0)
		return 0;

	now = ktime_get_real_seconds();

	rcu_read_lock();

	/* come up with a suitable timeout value */
	expiry = READ_ONCE(key->expiry);
	if (expiry == 0) {
		memcpy(xbuf, "perm", 5);
	} else if (now >= expiry) {
		memcpy(xbuf, "expd", 5);
	} else {
		timo = expiry - now;

		if (timo < 60)
			sprintf(xbuf, "%llus", timo);
		else if (timo < 60*60)
			sprintf(xbuf, "%llum", div_u64(timo, 60));
		else if (timo < 60*60*24)
			sprintf(xbuf, "%lluh", div_u64(timo, 60 * 60));
		else if (timo < 60*60*24*7)
			sprintf(xbuf, "%llud", div_u64(timo, 60 * 60 * 24));
		else
			sprintf(xbuf, "%lluw", div_u64(timo, 60 * 60 * 24 * 7));
	}

	state = key_read_state(key);

#define showflag(FLAGS, LETTER, FLAG) \
	((FLAGS & (1 << FLAG)) ? LETTER : '-')

	flags = READ_ONCE(key->flags);
	seq_printf(m, "%08x %c%c%c%c%c%c%c %5d %4s %08x %5d %5d %-9.9s ",
		   key->serial,
		   state != KEY_IS_UNINSTANTIATED ? 'I' : '-',
		   showflag(flags, 'R', KEY_FLAG_REVOKED),
		   showflag(flags, 'D', KEY_FLAG_DEAD),
		   showflag(flags, 'Q', KEY_FLAG_IN_QUOTA),
		   showflag(flags, 'U', KEY_FLAG_USER_CONSTRUCT),
		   state < 0 ? 'N' : '-',
		   showflag(flags, 'i', KEY_FLAG_INVALIDATED),
		   refcount_read(&key->usage),
		   xbuf,
		   key->perm,
		   from_kuid_munged(seq_user_ns(m), key->uid),
		   from_kgid_munged(seq_user_ns(m), key->gid),
		   key->type->name);

#undef showflag

	if (key->type->describe)
		key->type->describe(key, m);
	seq_putc(m, '\n');

	rcu_read_unlock();
	return 0;
}

static struct rb_node *__key_user_next(struct user_namespace *user_ns, struct rb_node *n)
{
	while (n) {
		struct key_user *user = rb_entry(n, struct key_user, node);
		if (kuid_has_mapping(user_ns, user->uid))
			break;
		n = rb_next(n);
	}
	return n;
}

static struct rb_node *key_user_next(struct user_namespace *user_ns, struct rb_node *n)
{
	return __key_user_next(user_ns, rb_next(n));
}

static struct rb_node *key_user_first(struct user_namespace *user_ns, struct rb_root *r)
{
	struct rb_node *n = rb_first(r);
	return __key_user_next(user_ns, n);
}

static void *proc_key_users_start(struct seq_file *p, loff_t *_pos)
	__acquires(key_user_lock)
{
	struct rb_node *_p;
	loff_t pos = *_pos;

	spin_lock(&key_user_lock);

	_p = key_user_first(seq_user_ns(p), &key_user_tree);
	while (pos > 0 && _p) {
		pos--;
		_p = key_user_next(seq_user_ns(p), _p);
	}

	return _p;
}

static void *proc_key_users_next(struct seq_file *p, void *v, loff_t *_pos)
{
	(*_pos)++;
	return key_user_next(seq_user_ns(p), (struct rb_node *)v);
}

static void proc_key_users_stop(struct seq_file *p, void *v)
	__releases(key_user_lock)
{
	spin_unlock(&key_user_lock);
}

static int proc_key_users_show(struct seq_file *m, void *v)
{
	struct rb_node *_p = v;
	struct key_user *user = rb_entry(_p, struct key_user, node);
	unsigned maxkeys = uid_eq(user->uid, GLOBAL_ROOT_UID) ?
		key_quota_root_maxkeys : key_quota_maxkeys;
	unsigned maxbytes = uid_eq(user->uid, GLOBAL_ROOT_UID) ?
		key_quota_root_maxbytes : key_quota_maxbytes;

	seq_printf(m, "%5u: %5d %d/%d %d/%d %d/%d\n",
		   from_kuid_munged(seq_user_ns(m), user->uid),
		   refcount_read(&user->usage),
		   atomic_read(&user->nkeys),
		   atomic_read(&user->nikeys),
		   user->qnkeys,
		   maxkeys,
		   user->qnbytes,
		   maxbytes);

	return 0;
}
