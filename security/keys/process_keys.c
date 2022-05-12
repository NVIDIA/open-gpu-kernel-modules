// SPDX-License-Identifier: GPL-2.0-or-later
/* Manage a process's keyrings
 *
 * Copyright (C) 2004-2005, 2008 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 */

#include <linux/init.h>
#include <linux/sched.h>
#include <linux/sched/user.h>
#include <linux/keyctl.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/mutex.h>
#include <linux/security.h>
#include <linux/user_namespace.h>
#include <linux/uaccess.h>
#include <linux/init_task.h>
#include <keys/request_key_auth-type.h>
#include "internal.h"

/* Session keyring create vs join semaphore */
static DEFINE_MUTEX(key_session_mutex);

/* The root user's tracking struct */
struct key_user root_key_user = {
	.usage		= REFCOUNT_INIT(3),
	.cons_lock	= __MUTEX_INITIALIZER(root_key_user.cons_lock),
	.lock		= __SPIN_LOCK_UNLOCKED(root_key_user.lock),
	.nkeys		= ATOMIC_INIT(2),
	.nikeys		= ATOMIC_INIT(2),
	.uid		= GLOBAL_ROOT_UID,
};

/*
 * Get or create a user register keyring.
 */
static struct key *get_user_register(struct user_namespace *user_ns)
{
	struct key *reg_keyring = READ_ONCE(user_ns->user_keyring_register);

	if (reg_keyring)
		return reg_keyring;

	down_write(&user_ns->keyring_sem);

	/* Make sure there's a register keyring.  It gets owned by the
	 * user_namespace's owner.
	 */
	reg_keyring = user_ns->user_keyring_register;
	if (!reg_keyring) {
		reg_keyring = keyring_alloc(".user_reg",
					    user_ns->owner, INVALID_GID,
					    &init_cred,
					    KEY_POS_WRITE | KEY_POS_SEARCH |
					    KEY_USR_VIEW | KEY_USR_READ,
					    0,
					    NULL, NULL);
		if (!IS_ERR(reg_keyring))
			smp_store_release(&user_ns->user_keyring_register,
					  reg_keyring);
	}

	up_write(&user_ns->keyring_sem);

	/* We don't return a ref since the keyring is pinned by the user_ns */
	return reg_keyring;
}

/*
 * Look up the user and user session keyrings for the current process's UID,
 * creating them if they don't exist.
 */
int look_up_user_keyrings(struct key **_user_keyring,
			  struct key **_user_session_keyring)
{
	const struct cred *cred = current_cred();
	struct user_namespace *user_ns = current_user_ns();
	struct key *reg_keyring, *uid_keyring, *session_keyring;
	key_perm_t user_keyring_perm;
	key_ref_t uid_keyring_r, session_keyring_r;
	uid_t uid = from_kuid(user_ns, cred->user->uid);
	char buf[20];
	int ret;

	user_keyring_perm = (KEY_POS_ALL & ~KEY_POS_SETATTR) | KEY_USR_ALL;

	kenter("%u", uid);

	reg_keyring = get_user_register(user_ns);
	if (IS_ERR(reg_keyring))
		return PTR_ERR(reg_keyring);

	down_write(&user_ns->keyring_sem);
	ret = 0;

	/* Get the user keyring.  Note that there may be one in existence
	 * already as it may have been pinned by a session, but the user_struct
	 * pointing to it may have been destroyed by setuid.
	 */
	snprintf(buf, sizeof(buf), "_uid.%u", uid);
	uid_keyring_r = keyring_search(make_key_ref(reg_keyring, true),
				       &key_type_keyring, buf, false);
	kdebug("_uid %p", uid_keyring_r);
	if (uid_keyring_r == ERR_PTR(-EAGAIN)) {
		uid_keyring = keyring_alloc(buf, cred->user->uid, INVALID_GID,
					    cred, user_keyring_perm,
					    KEY_ALLOC_UID_KEYRING |
					    KEY_ALLOC_IN_QUOTA,
					    NULL, reg_keyring);
		if (IS_ERR(uid_keyring)) {
			ret = PTR_ERR(uid_keyring);
			goto error;
		}
	} else if (IS_ERR(uid_keyring_r)) {
		ret = PTR_ERR(uid_keyring_r);
		goto error;
	} else {
		uid_keyring = key_ref_to_ptr(uid_keyring_r);
	}

	/* Get a default session keyring (which might also exist already) */
	snprintf(buf, sizeof(buf), "_uid_ses.%u", uid);
	session_keyring_r = keyring_search(make_key_ref(reg_keyring, true),
					   &key_type_keyring, buf, false);
	kdebug("_uid_ses %p", session_keyring_r);
	if (session_keyring_r == ERR_PTR(-EAGAIN)) {
		session_keyring = keyring_alloc(buf, cred->user->uid, INVALID_GID,
						cred, user_keyring_perm,
						KEY_ALLOC_UID_KEYRING |
						KEY_ALLOC_IN_QUOTA,
						NULL, NULL);
		if (IS_ERR(session_keyring)) {
			ret = PTR_ERR(session_keyring);
			goto error_release;
		}

		/* We install a link from the user session keyring to
		 * the user keyring.
		 */
		ret = key_link(session_keyring, uid_keyring);
		if (ret < 0)
			goto error_release_session;

		/* And only then link the user-session keyring to the
		 * register.
		 */
		ret = key_link(reg_keyring, session_keyring);
		if (ret < 0)
			goto error_release_session;
	} else if (IS_ERR(session_keyring_r)) {
		ret = PTR_ERR(session_keyring_r);
		goto error_release;
	} else {
		session_keyring = key_ref_to_ptr(session_keyring_r);
	}

	up_write(&user_ns->keyring_sem);

	if (_user_session_keyring)
		*_user_session_keyring = session_keyring;
	else
		key_put(session_keyring);
	if (_user_keyring)
		*_user_keyring = uid_keyring;
	else
		key_put(uid_keyring);
	kleave(" = 0");
	return 0;

error_release_session:
	key_put(session_keyring);
error_release:
	key_put(uid_keyring);
error:
	up_write(&user_ns->keyring_sem);
	kleave(" = %d", ret);
	return ret;
}

/*
 * Get the user session keyring if it exists, but don't create it if it
 * doesn't.
 */
struct key *get_user_session_keyring_rcu(const struct cred *cred)
{
	struct key *reg_keyring = READ_ONCE(cred->user_ns->user_keyring_register);
	key_ref_t session_keyring_r;
	char buf[20];

	struct keyring_search_context ctx = {
		.index_key.type		= &key_type_keyring,
		.index_key.description	= buf,
		.cred			= cred,
		.match_data.cmp		= key_default_cmp,
		.match_data.raw_data	= buf,
		.match_data.lookup_type	= KEYRING_SEARCH_LOOKUP_DIRECT,
		.flags			= KEYRING_SEARCH_DO_STATE_CHECK,
	};

	if (!reg_keyring)
		return NULL;

	ctx.index_key.desc_len = snprintf(buf, sizeof(buf), "_uid_ses.%u",
					  from_kuid(cred->user_ns,
						    cred->user->uid));

	session_keyring_r = keyring_search_rcu(make_key_ref(reg_keyring, true),
					       &ctx);
	if (IS_ERR(session_keyring_r))
		return NULL;
	return key_ref_to_ptr(session_keyring_r);
}

/*
 * Install a thread keyring to the given credentials struct if it didn't have
 * one already.  This is allowed to overrun the quota.
 *
 * Return: 0 if a thread keyring is now present; -errno on failure.
 */
int install_thread_keyring_to_cred(struct cred *new)
{
	struct key *keyring;

	if (new->thread_keyring)
		return 0;

	keyring = keyring_alloc("_tid", new->uid, new->gid, new,
				KEY_POS_ALL | KEY_USR_VIEW,
				KEY_ALLOC_QUOTA_OVERRUN,
				NULL, NULL);
	if (IS_ERR(keyring))
		return PTR_ERR(keyring);

	new->thread_keyring = keyring;
	return 0;
}

/*
 * Install a thread keyring to the current task if it didn't have one already.
 *
 * Return: 0 if a thread keyring is now present; -errno on failure.
 */
static int install_thread_keyring(void)
{
	struct cred *new;
	int ret;

	new = prepare_creds();
	if (!new)
		return -ENOMEM;

	ret = install_thread_keyring_to_cred(new);
	if (ret < 0) {
		abort_creds(new);
		return ret;
	}

	return commit_creds(new);
}

/*
 * Install a process keyring to the given credentials struct if it didn't have
 * one already.  This is allowed to overrun the quota.
 *
 * Return: 0 if a process keyring is now present; -errno on failure.
 */
int install_process_keyring_to_cred(struct cred *new)
{
	struct key *keyring;

	if (new->process_keyring)
		return 0;

	keyring = keyring_alloc("_pid", new->uid, new->gid, new,
				KEY_POS_ALL | KEY_USR_VIEW,
				KEY_ALLOC_QUOTA_OVERRUN,
				NULL, NULL);
	if (IS_ERR(keyring))
		return PTR_ERR(keyring);

	new->process_keyring = keyring;
	return 0;
}

/*
 * Install a process keyring to the current task if it didn't have one already.
 *
 * Return: 0 if a process keyring is now present; -errno on failure.
 */
static int install_process_keyring(void)
{
	struct cred *new;
	int ret;

	new = prepare_creds();
	if (!new)
		return -ENOMEM;

	ret = install_process_keyring_to_cred(new);
	if (ret < 0) {
		abort_creds(new);
		return ret;
	}

	return commit_creds(new);
}

/*
 * Install the given keyring as the session keyring of the given credentials
 * struct, replacing the existing one if any.  If the given keyring is NULL,
 * then install a new anonymous session keyring.
 * @cred can not be in use by any task yet.
 *
 * Return: 0 on success; -errno on failure.
 */
int install_session_keyring_to_cred(struct cred *cred, struct key *keyring)
{
	unsigned long flags;
	struct key *old;

	might_sleep();

	/* create an empty session keyring */
	if (!keyring) {
		flags = KEY_ALLOC_QUOTA_OVERRUN;
		if (cred->session_keyring)
			flags = KEY_ALLOC_IN_QUOTA;

		keyring = keyring_alloc("_ses", cred->uid, cred->gid, cred,
					KEY_POS_ALL | KEY_USR_VIEW | KEY_USR_READ,
					flags, NULL, NULL);
		if (IS_ERR(keyring))
			return PTR_ERR(keyring);
	} else {
		__key_get(keyring);
	}

	/* install the keyring */
	old = cred->session_keyring;
	cred->session_keyring = keyring;

	if (old)
		key_put(old);

	return 0;
}

/*
 * Install the given keyring as the session keyring of the current task,
 * replacing the existing one if any.  If the given keyring is NULL, then
 * install a new anonymous session keyring.
 *
 * Return: 0 on success; -errno on failure.
 */
static int install_session_keyring(struct key *keyring)
{
	struct cred *new;
	int ret;

	new = prepare_creds();
	if (!new)
		return -ENOMEM;

	ret = install_session_keyring_to_cred(new, keyring);
	if (ret < 0) {
		abort_creds(new);
		return ret;
	}

	return commit_creds(new);
}

/*
 * Handle the fsuid changing.
 */
void key_fsuid_changed(struct cred *new_cred)
{
	/* update the ownership of the thread keyring */
	if (new_cred->thread_keyring) {
		down_write(&new_cred->thread_keyring->sem);
		new_cred->thread_keyring->uid = new_cred->fsuid;
		up_write(&new_cred->thread_keyring->sem);
	}
}

/*
 * Handle the fsgid changing.
 */
void key_fsgid_changed(struct cred *new_cred)
{
	/* update the ownership of the thread keyring */
	if (new_cred->thread_keyring) {
		down_write(&new_cred->thread_keyring->sem);
		new_cred->thread_keyring->gid = new_cred->fsgid;
		up_write(&new_cred->thread_keyring->sem);
	}
}

/*
 * Search the process keyrings attached to the supplied cred for the first
 * matching key under RCU conditions (the caller must be holding the RCU read
 * lock).
 *
 * The search criteria are the type and the match function.  The description is
 * given to the match function as a parameter, but doesn't otherwise influence
 * the search.  Typically the match function will compare the description
 * parameter to the key's description.
 *
 * This can only search keyrings that grant Search permission to the supplied
 * credentials.  Keyrings linked to searched keyrings will also be searched if
 * they grant Search permission too.  Keys can only be found if they grant
 * Search permission to the credentials.
 *
 * Returns a pointer to the key with the key usage count incremented if
 * successful, -EAGAIN if we didn't find any matching key or -ENOKEY if we only
 * matched negative keys.
 *
 * In the case of a successful return, the possession attribute is set on the
 * returned key reference.
 */
key_ref_t search_cred_keyrings_rcu(struct keyring_search_context *ctx)
{
	struct key *user_session;
	key_ref_t key_ref, ret, err;
	const struct cred *cred = ctx->cred;

	/* we want to return -EAGAIN or -ENOKEY if any of the keyrings were
	 * searchable, but we failed to find a key or we found a negative key;
	 * otherwise we want to return a sample error (probably -EACCES) if
	 * none of the keyrings were searchable
	 *
	 * in terms of priority: success > -ENOKEY > -EAGAIN > other error
	 */
	key_ref = NULL;
	ret = NULL;
	err = ERR_PTR(-EAGAIN);

	/* search the thread keyring first */
	if (cred->thread_keyring) {
		key_ref = keyring_search_rcu(
			make_key_ref(cred->thread_keyring, 1), ctx);
		if (!IS_ERR(key_ref))
			goto found;

		switch (PTR_ERR(key_ref)) {
		case -EAGAIN: /* no key */
		case -ENOKEY: /* negative key */
			ret = key_ref;
			break;
		default:
			err = key_ref;
			break;
		}
	}

	/* search the process keyring second */
	if (cred->process_keyring) {
		key_ref = keyring_search_rcu(
			make_key_ref(cred->process_keyring, 1), ctx);
		if (!IS_ERR(key_ref))
			goto found;

		switch (PTR_ERR(key_ref)) {
		case -EAGAIN: /* no key */
			if (ret)
				break;
			fallthrough;
		case -ENOKEY: /* negative key */
			ret = key_ref;
			break;
		default:
			err = key_ref;
			break;
		}
	}

	/* search the session keyring */
	if (cred->session_keyring) {
		key_ref = keyring_search_rcu(
			make_key_ref(cred->session_keyring, 1), ctx);

		if (!IS_ERR(key_ref))
			goto found;

		switch (PTR_ERR(key_ref)) {
		case -EAGAIN: /* no key */
			if (ret)
				break;
			fallthrough;
		case -ENOKEY: /* negative key */
			ret = key_ref;
			break;
		default:
			err = key_ref;
			break;
		}
	}
	/* or search the user-session keyring */
	else if ((user_session = get_user_session_keyring_rcu(cred))) {
		key_ref = keyring_search_rcu(make_key_ref(user_session, 1),
					     ctx);
		key_put(user_session);

		if (!IS_ERR(key_ref))
			goto found;

		switch (PTR_ERR(key_ref)) {
		case -EAGAIN: /* no key */
			if (ret)
				break;
			fallthrough;
		case -ENOKEY: /* negative key */
			ret = key_ref;
			break;
		default:
			err = key_ref;
			break;
		}
	}

	/* no key - decide on the error we're going to go for */
	key_ref = ret ? ret : err;

found:
	return key_ref;
}

/*
 * Search the process keyrings attached to the supplied cred for the first
 * matching key in the manner of search_my_process_keyrings(), but also search
 * the keys attached to the assumed authorisation key using its credentials if
 * one is available.
 *
 * The caller must be holding the RCU read lock.
 *
 * Return same as search_cred_keyrings_rcu().
 */
key_ref_t search_process_keyrings_rcu(struct keyring_search_context *ctx)
{
	struct request_key_auth *rka;
	key_ref_t key_ref, ret = ERR_PTR(-EACCES), err;

	key_ref = search_cred_keyrings_rcu(ctx);
	if (!IS_ERR(key_ref))
		goto found;
	err = key_ref;

	/* if this process has an instantiation authorisation key, then we also
	 * search the keyrings of the process mentioned there
	 * - we don't permit access to request_key auth keys via this method
	 */
	if (ctx->cred->request_key_auth &&
	    ctx->cred == current_cred() &&
	    ctx->index_key.type != &key_type_request_key_auth
	    ) {
		const struct cred *cred = ctx->cred;

		if (key_validate(cred->request_key_auth) == 0) {
			rka = ctx->cred->request_key_auth->payload.data[0];

			//// was search_process_keyrings() [ie. recursive]
			ctx->cred = rka->cred;
			key_ref = search_cred_keyrings_rcu(ctx);
			ctx->cred = cred;

			if (!IS_ERR(key_ref))
				goto found;
			ret = key_ref;
		}
	}

	/* no key - decide on the error we're going to go for */
	if (err == ERR_PTR(-ENOKEY) || ret == ERR_PTR(-ENOKEY))
		key_ref = ERR_PTR(-ENOKEY);
	else if (err == ERR_PTR(-EACCES))
		key_ref = ret;
	else
		key_ref = err;

found:
	return key_ref;
}
/*
 * See if the key we're looking at is the target key.
 */
bool lookup_user_key_possessed(const struct key *key,
			       const struct key_match_data *match_data)
{
	return key == match_data->raw_data;
}

/*
 * Look up a key ID given us by userspace with a given permissions mask to get
 * the key it refers to.
 *
 * Flags can be passed to request that special keyrings be created if referred
 * to directly, to permit partially constructed keys to be found and to skip
 * validity and permission checks on the found key.
 *
 * Returns a pointer to the key with an incremented usage count if successful;
 * -EINVAL if the key ID is invalid; -ENOKEY if the key ID does not correspond
 * to a key or the best found key was a negative key; -EKEYREVOKED or
 * -EKEYEXPIRED if the best found key was revoked or expired; -EACCES if the
 * found key doesn't grant the requested permit or the LSM denied access to it;
 * or -ENOMEM if a special keyring couldn't be created.
 *
 * In the case of a successful return, the possession attribute is set on the
 * returned key reference.
 */
key_ref_t lookup_user_key(key_serial_t id, unsigned long lflags,
			  enum key_need_perm need_perm)
{
	struct keyring_search_context ctx = {
		.match_data.cmp		= lookup_user_key_possessed,
		.match_data.lookup_type	= KEYRING_SEARCH_LOOKUP_DIRECT,
		.flags			= (KEYRING_SEARCH_NO_STATE_CHECK |
					   KEYRING_SEARCH_RECURSE),
	};
	struct request_key_auth *rka;
	struct key *key, *user_session;
	key_ref_t key_ref, skey_ref;
	int ret;

try_again:
	ctx.cred = get_current_cred();
	key_ref = ERR_PTR(-ENOKEY);

	switch (id) {
	case KEY_SPEC_THREAD_KEYRING:
		if (!ctx.cred->thread_keyring) {
			if (!(lflags & KEY_LOOKUP_CREATE))
				goto error;

			ret = install_thread_keyring();
			if (ret < 0) {
				key_ref = ERR_PTR(ret);
				goto error;
			}
			goto reget_creds;
		}

		key = ctx.cred->thread_keyring;
		__key_get(key);
		key_ref = make_key_ref(key, 1);
		break;

	case KEY_SPEC_PROCESS_KEYRING:
		if (!ctx.cred->process_keyring) {
			if (!(lflags & KEY_LOOKUP_CREATE))
				goto error;

			ret = install_process_keyring();
			if (ret < 0) {
				key_ref = ERR_PTR(ret);
				goto error;
			}
			goto reget_creds;
		}

		key = ctx.cred->process_keyring;
		__key_get(key);
		key_ref = make_key_ref(key, 1);
		break;

	case KEY_SPEC_SESSION_KEYRING:
		if (!ctx.cred->session_keyring) {
			/* always install a session keyring upon access if one
			 * doesn't exist yet */
			ret = look_up_user_keyrings(NULL, &user_session);
			if (ret < 0)
				goto error;
			if (lflags & KEY_LOOKUP_CREATE)
				ret = join_session_keyring(NULL);
			else
				ret = install_session_keyring(user_session);

			key_put(user_session);
			if (ret < 0)
				goto error;
			goto reget_creds;
		} else if (test_bit(KEY_FLAG_UID_KEYRING,
				    &ctx.cred->session_keyring->flags) &&
			   lflags & KEY_LOOKUP_CREATE) {
			ret = join_session_keyring(NULL);
			if (ret < 0)
				goto error;
			goto reget_creds;
		}

		key = ctx.cred->session_keyring;
		__key_get(key);
		key_ref = make_key_ref(key, 1);
		break;

	case KEY_SPEC_USER_KEYRING:
		ret = look_up_user_keyrings(&key, NULL);
		if (ret < 0)
			goto error;
		key_ref = make_key_ref(key, 1);
		break;

	case KEY_SPEC_USER_SESSION_KEYRING:
		ret = look_up_user_keyrings(NULL, &key);
		if (ret < 0)
			goto error;
		key_ref = make_key_ref(key, 1);
		break;

	case KEY_SPEC_GROUP_KEYRING:
		/* group keyrings are not yet supported */
		key_ref = ERR_PTR(-EINVAL);
		goto error;

	case KEY_SPEC_REQKEY_AUTH_KEY:
		key = ctx.cred->request_key_auth;
		if (!key)
			goto error;

		__key_get(key);
		key_ref = make_key_ref(key, 1);
		break;

	case KEY_SPEC_REQUESTOR_KEYRING:
		if (!ctx.cred->request_key_auth)
			goto error;

		down_read(&ctx.cred->request_key_auth->sem);
		if (test_bit(KEY_FLAG_REVOKED,
			     &ctx.cred->request_key_auth->flags)) {
			key_ref = ERR_PTR(-EKEYREVOKED);
			key = NULL;
		} else {
			rka = ctx.cred->request_key_auth->payload.data[0];
			key = rka->dest_keyring;
			__key_get(key);
		}
		up_read(&ctx.cred->request_key_auth->sem);
		if (!key)
			goto error;
		key_ref = make_key_ref(key, 1);
		break;

	default:
		key_ref = ERR_PTR(-EINVAL);
		if (id < 1)
			goto error;

		key = key_lookup(id);
		if (IS_ERR(key)) {
			key_ref = ERR_CAST(key);
			goto error;
		}

		key_ref = make_key_ref(key, 0);

		/* check to see if we possess the key */
		ctx.index_key			= key->index_key;
		ctx.match_data.raw_data		= key;
		kdebug("check possessed");
		rcu_read_lock();
		skey_ref = search_process_keyrings_rcu(&ctx);
		rcu_read_unlock();
		kdebug("possessed=%p", skey_ref);

		if (!IS_ERR(skey_ref)) {
			key_put(key);
			key_ref = skey_ref;
		}

		break;
	}

	/* unlink does not use the nominated key in any way, so can skip all
	 * the permission checks as it is only concerned with the keyring */
	if (need_perm != KEY_NEED_UNLINK) {
		if (!(lflags & KEY_LOOKUP_PARTIAL)) {
			ret = wait_for_key_construction(key, true);
			switch (ret) {
			case -ERESTARTSYS:
				goto invalid_key;
			default:
				if (need_perm != KEY_AUTHTOKEN_OVERRIDE &&
				    need_perm != KEY_DEFER_PERM_CHECK)
					goto invalid_key;
				break;
			case 0:
				break;
			}
		} else if (need_perm != KEY_DEFER_PERM_CHECK) {
			ret = key_validate(key);
			if (ret < 0)
				goto invalid_key;
		}

		ret = -EIO;
		if (!(lflags & KEY_LOOKUP_PARTIAL) &&
		    key_read_state(key) == KEY_IS_UNINSTANTIATED)
			goto invalid_key;
	}

	/* check the permissions */
	ret = key_task_permission(key_ref, ctx.cred, need_perm);
	if (ret < 0)
		goto invalid_key;

	key->last_used_at = ktime_get_real_seconds();

error:
	put_cred(ctx.cred);
	return key_ref;

invalid_key:
	key_ref_put(key_ref);
	key_ref = ERR_PTR(ret);
	goto error;

	/* if we attempted to install a keyring, then it may have caused new
	 * creds to be installed */
reget_creds:
	put_cred(ctx.cred);
	goto try_again;
}
EXPORT_SYMBOL(lookup_user_key);

/*
 * Join the named keyring as the session keyring if possible else attempt to
 * create a new one of that name and join that.
 *
 * If the name is NULL, an empty anonymous keyring will be installed as the
 * session keyring.
 *
 * Named session keyrings are joined with a semaphore held to prevent the
 * keyrings from going away whilst the attempt is made to going them and also
 * to prevent a race in creating compatible session keyrings.
 */
long join_session_keyring(const char *name)
{
	const struct cred *old;
	struct cred *new;
	struct key *keyring;
	long ret, serial;

	new = prepare_creds();
	if (!new)
		return -ENOMEM;
	old = current_cred();

	/* if no name is provided, install an anonymous keyring */
	if (!name) {
		ret = install_session_keyring_to_cred(new, NULL);
		if (ret < 0)
			goto error;

		serial = new->session_keyring->serial;
		ret = commit_creds(new);
		if (ret == 0)
			ret = serial;
		goto okay;
	}

	/* allow the user to join or create a named keyring */
	mutex_lock(&key_session_mutex);

	/* look for an existing keyring of this name */
	keyring = find_keyring_by_name(name, false);
	if (PTR_ERR(keyring) == -ENOKEY) {
		/* not found - try and create a new one */
		keyring = keyring_alloc(
			name, old->uid, old->gid, old,
			KEY_POS_ALL | KEY_USR_VIEW | KEY_USR_READ | KEY_USR_LINK,
			KEY_ALLOC_IN_QUOTA, NULL, NULL);
		if (IS_ERR(keyring)) {
			ret = PTR_ERR(keyring);
			goto error2;
		}
	} else if (IS_ERR(keyring)) {
		ret = PTR_ERR(keyring);
		goto error2;
	} else if (keyring == new->session_keyring) {
		ret = 0;
		goto error3;
	}

	/* we've got a keyring - now to install it */
	ret = install_session_keyring_to_cred(new, keyring);
	if (ret < 0)
		goto error3;

	commit_creds(new);
	mutex_unlock(&key_session_mutex);

	ret = keyring->serial;
	key_put(keyring);
okay:
	return ret;

error3:
	key_put(keyring);
error2:
	mutex_unlock(&key_session_mutex);
error:
	abort_creds(new);
	return ret;
}

/*
 * Replace a process's session keyring on behalf of one of its children when
 * the target  process is about to resume userspace execution.
 */
void key_change_session_keyring(struct callback_head *twork)
{
	const struct cred *old = current_cred();
	struct cred *new = container_of(twork, struct cred, rcu);

	if (unlikely(current->flags & PF_EXITING)) {
		put_cred(new);
		return;
	}

	new->  uid	= old->  uid;
	new-> euid	= old-> euid;
	new-> suid	= old-> suid;
	new->fsuid	= old->fsuid;
	new->  gid	= old->  gid;
	new-> egid	= old-> egid;
	new-> sgid	= old-> sgid;
	new->fsgid	= old->fsgid;
	new->user	= get_uid(old->user);
	new->user_ns	= get_user_ns(old->user_ns);
	new->group_info	= get_group_info(old->group_info);

	new->securebits	= old->securebits;
	new->cap_inheritable	= old->cap_inheritable;
	new->cap_permitted	= old->cap_permitted;
	new->cap_effective	= old->cap_effective;
	new->cap_ambient	= old->cap_ambient;
	new->cap_bset		= old->cap_bset;

	new->jit_keyring	= old->jit_keyring;
	new->thread_keyring	= key_get(old->thread_keyring);
	new->process_keyring	= key_get(old->process_keyring);

	security_transfer_creds(new, old);

	commit_creds(new);
}

/*
 * Make sure that root's user and user-session keyrings exist.
 */
static int __init init_root_keyring(void)
{
	return look_up_user_keyrings(NULL, NULL);
}

late_initcall(init_root_keyring);
