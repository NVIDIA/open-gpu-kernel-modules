// SPDX-License-Identifier: GPL-2.0-or-later
/* AFS volume management
 *
 * Copyright (C) 2002, 2007 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 */

#include <linux/kernel.h>
#include <linux/slab.h>
#include "internal.h"

unsigned __read_mostly afs_volume_gc_delay = 10;
unsigned __read_mostly afs_volume_record_life = 60 * 60;

/*
 * Insert a volume into a cell.  If there's an existing volume record, that is
 * returned instead with a ref held.
 */
static struct afs_volume *afs_insert_volume_into_cell(struct afs_cell *cell,
						      struct afs_volume *volume)
{
	struct afs_volume *p;
	struct rb_node *parent = NULL, **pp;

	write_seqlock(&cell->volume_lock);

	pp = &cell->volumes.rb_node;
	while (*pp) {
		parent = *pp;
		p = rb_entry(parent, struct afs_volume, cell_node);
		if (p->vid < volume->vid) {
			pp = &(*pp)->rb_left;
		} else if (p->vid > volume->vid) {
			pp = &(*pp)->rb_right;
		} else {
			volume = afs_get_volume(p, afs_volume_trace_get_cell_insert);
			goto found;
		}
	}

	rb_link_node_rcu(&volume->cell_node, parent, pp);
	rb_insert_color(&volume->cell_node, &cell->volumes);
	hlist_add_head_rcu(&volume->proc_link, &cell->proc_volumes);

found:
	write_sequnlock(&cell->volume_lock);
	return volume;

}

static void afs_remove_volume_from_cell(struct afs_volume *volume)
{
	struct afs_cell *cell = volume->cell;

	if (!hlist_unhashed(&volume->proc_link)) {
		trace_afs_volume(volume->vid, atomic_read(&volume->usage),
				 afs_volume_trace_remove);
		write_seqlock(&cell->volume_lock);
		hlist_del_rcu(&volume->proc_link);
		rb_erase(&volume->cell_node, &cell->volumes);
		write_sequnlock(&cell->volume_lock);
	}
}

/*
 * Allocate a volume record and load it up from a vldb record.
 */
static struct afs_volume *afs_alloc_volume(struct afs_fs_context *params,
					   struct afs_vldb_entry *vldb,
					   unsigned long type_mask)
{
	struct afs_server_list *slist;
	struct afs_volume *volume;
	int ret = -ENOMEM, nr_servers = 0, i;

	for (i = 0; i < vldb->nr_servers; i++)
		if (vldb->fs_mask[i] & type_mask)
			nr_servers++;

	volume = kzalloc(sizeof(struct afs_volume), GFP_KERNEL);
	if (!volume)
		goto error_0;

	volume->vid		= vldb->vid[params->type];
	volume->update_at	= ktime_get_real_seconds() + afs_volume_record_life;
	volume->cell		= afs_get_cell(params->cell, afs_cell_trace_get_vol);
	volume->type		= params->type;
	volume->type_force	= params->force;
	volume->name_len	= vldb->name_len;

	atomic_set(&volume->usage, 1);
	INIT_HLIST_NODE(&volume->proc_link);
	rwlock_init(&volume->servers_lock);
	rwlock_init(&volume->cb_v_break_lock);
	memcpy(volume->name, vldb->name, vldb->name_len + 1);

	slist = afs_alloc_server_list(params->cell, params->key, vldb, type_mask);
	if (IS_ERR(slist)) {
		ret = PTR_ERR(slist);
		goto error_1;
	}

	refcount_set(&slist->usage, 1);
	rcu_assign_pointer(volume->servers, slist);
	trace_afs_volume(volume->vid, 1, afs_volume_trace_alloc);
	return volume;

error_1:
	afs_put_cell(volume->cell, afs_cell_trace_put_vol);
	kfree(volume);
error_0:
	return ERR_PTR(ret);
}

/*
 * Look up or allocate a volume record.
 */
static struct afs_volume *afs_lookup_volume(struct afs_fs_context *params,
					    struct afs_vldb_entry *vldb,
					    unsigned long type_mask)
{
	struct afs_volume *candidate, *volume;

	candidate = afs_alloc_volume(params, vldb, type_mask);
	if (IS_ERR(candidate))
		return candidate;

	volume = afs_insert_volume_into_cell(params->cell, candidate);
	if (volume != candidate)
		afs_put_volume(params->net, candidate, afs_volume_trace_put_cell_dup);
	return volume;
}

/*
 * Look up a VLDB record for a volume.
 */
static struct afs_vldb_entry *afs_vl_lookup_vldb(struct afs_cell *cell,
						 struct key *key,
						 const char *volname,
						 size_t volnamesz)
{
	struct afs_vldb_entry *vldb = ERR_PTR(-EDESTADDRREQ);
	struct afs_vl_cursor vc;
	int ret;

	if (!afs_begin_vlserver_operation(&vc, cell, key))
		return ERR_PTR(-ERESTARTSYS);

	while (afs_select_vlserver(&vc)) {
		vldb = afs_vl_get_entry_by_name_u(&vc, volname, volnamesz);
	}

	ret = afs_end_vlserver_operation(&vc);
	return ret < 0 ? ERR_PTR(ret) : vldb;
}

/*
 * Look up a volume in the VL server and create a candidate volume record for
 * it.
 *
 * The volume name can be one of the following:
 *	"%[cell:]volume[.]"		R/W volume
 *	"#[cell:]volume[.]"		R/O or R/W volume (rwparent=0),
 *					 or R/W (rwparent=1) volume
 *	"%[cell:]volume.readonly"	R/O volume
 *	"#[cell:]volume.readonly"	R/O volume
 *	"%[cell:]volume.backup"		Backup volume
 *	"#[cell:]volume.backup"		Backup volume
 *
 * The cell name is optional, and defaults to the current cell.
 *
 * See "The Rules of Mount Point Traversal" in Chapter 5 of the AFS SysAdmin
 * Guide
 * - Rule 1: Explicit type suffix forces access of that type or nothing
 *           (no suffix, then use Rule 2 & 3)
 * - Rule 2: If parent volume is R/O, then mount R/O volume by preference, R/W
 *           if not available
 * - Rule 3: If parent volume is R/W, then only mount R/W volume unless
 *           explicitly told otherwise
 */
struct afs_volume *afs_create_volume(struct afs_fs_context *params)
{
	struct afs_vldb_entry *vldb;
	struct afs_volume *volume;
	unsigned long type_mask = 1UL << params->type;

	vldb = afs_vl_lookup_vldb(params->cell, params->key,
				  params->volname, params->volnamesz);
	if (IS_ERR(vldb))
		return ERR_CAST(vldb);

	if (test_bit(AFS_VLDB_QUERY_ERROR, &vldb->flags)) {
		volume = ERR_PTR(vldb->error);
		goto error;
	}

	/* Make the final decision on the type we want */
	volume = ERR_PTR(-ENOMEDIUM);
	if (params->force) {
		if (!(vldb->flags & type_mask))
			goto error;
	} else if (test_bit(AFS_VLDB_HAS_RO, &vldb->flags)) {
		params->type = AFSVL_ROVOL;
	} else if (test_bit(AFS_VLDB_HAS_RW, &vldb->flags)) {
		params->type = AFSVL_RWVOL;
	} else {
		goto error;
	}

	type_mask = 1UL << params->type;
	volume = afs_lookup_volume(params, vldb, type_mask);

error:
	kfree(vldb);
	return volume;
}

/*
 * Destroy a volume record
 */
static void afs_destroy_volume(struct afs_net *net, struct afs_volume *volume)
{
	_enter("%p", volume);

#ifdef CONFIG_AFS_FSCACHE
	ASSERTCMP(volume->cache, ==, NULL);
#endif

	afs_remove_volume_from_cell(volume);
	afs_put_serverlist(net, rcu_access_pointer(volume->servers));
	afs_put_cell(volume->cell, afs_cell_trace_put_vol);
	trace_afs_volume(volume->vid, atomic_read(&volume->usage),
			 afs_volume_trace_free);
	kfree_rcu(volume, rcu);

	_leave(" [destroyed]");
}

/*
 * Get a reference on a volume record.
 */
struct afs_volume *afs_get_volume(struct afs_volume *volume,
				  enum afs_volume_trace reason)
{
	if (volume) {
		int u = atomic_inc_return(&volume->usage);
		trace_afs_volume(volume->vid, u, reason);
	}
	return volume;
}


/*
 * Drop a reference on a volume record.
 */
void afs_put_volume(struct afs_net *net, struct afs_volume *volume,
		    enum afs_volume_trace reason)
{
	if (volume) {
		afs_volid_t vid = volume->vid;
		int u = atomic_dec_return(&volume->usage);
		trace_afs_volume(vid, u, reason);
		if (u == 0)
			afs_destroy_volume(net, volume);
	}
}

/*
 * Activate a volume.
 */
void afs_activate_volume(struct afs_volume *volume)
{
#ifdef CONFIG_AFS_FSCACHE
	volume->cache = fscache_acquire_cookie(volume->cell->cache,
					       &afs_volume_cache_index_def,
					       &volume->vid, sizeof(volume->vid),
					       NULL, 0,
					       volume, 0, true);
#endif
}

/*
 * Deactivate a volume.
 */
void afs_deactivate_volume(struct afs_volume *volume)
{
	_enter("%s", volume->name);

#ifdef CONFIG_AFS_FSCACHE
	fscache_relinquish_cookie(volume->cache, NULL,
				  test_bit(AFS_VOLUME_DELETED, &volume->flags));
	volume->cache = NULL;
#endif

	_leave("");
}

/*
 * Query the VL service to update the volume status.
 */
static int afs_update_volume_status(struct afs_volume *volume, struct key *key)
{
	struct afs_server_list *new, *old, *discard;
	struct afs_vldb_entry *vldb;
	char idbuf[16];
	int ret, idsz;

	_enter("");

	/* We look up an ID by passing it as a decimal string in the
	 * operation's name parameter.
	 */
	idsz = sprintf(idbuf, "%llu", volume->vid);

	vldb = afs_vl_lookup_vldb(volume->cell, key, idbuf, idsz);
	if (IS_ERR(vldb)) {
		ret = PTR_ERR(vldb);
		goto error;
	}

	/* See if the volume got renamed. */
	if (vldb->name_len != volume->name_len ||
	    memcmp(vldb->name, volume->name, vldb->name_len) != 0) {
		/* TODO: Use RCU'd string. */
		memcpy(volume->name, vldb->name, AFS_MAXVOLNAME);
		volume->name_len = vldb->name_len;
	}

	/* See if the volume's server list got updated. */
	new = afs_alloc_server_list(volume->cell, key,
				    vldb, (1 << volume->type));
	if (IS_ERR(new)) {
		ret = PTR_ERR(new);
		goto error_vldb;
	}

	write_lock(&volume->servers_lock);

	discard = new;
	old = rcu_dereference_protected(volume->servers,
					lockdep_is_held(&volume->servers_lock));
	if (afs_annotate_server_list(new, old)) {
		new->seq = volume->servers_seq + 1;
		rcu_assign_pointer(volume->servers, new);
		smp_wmb();
		volume->servers_seq++;
		discard = old;
	}

	volume->update_at = ktime_get_real_seconds() + afs_volume_record_life;
	write_unlock(&volume->servers_lock);
	ret = 0;

	afs_put_serverlist(volume->cell->net, discard);
error_vldb:
	kfree(vldb);
error:
	_leave(" = %d", ret);
	return ret;
}

/*
 * Make sure the volume record is up to date.
 */
int afs_check_volume_status(struct afs_volume *volume, struct afs_operation *op)
{
	int ret, retries = 0;

	_enter("");

retry:
	if (test_bit(AFS_VOLUME_WAIT, &volume->flags))
		goto wait;
	if (volume->update_at <= ktime_get_real_seconds() ||
	    test_bit(AFS_VOLUME_NEEDS_UPDATE, &volume->flags))
		goto update;
	_leave(" = 0");
	return 0;

update:
	if (!test_and_set_bit_lock(AFS_VOLUME_UPDATING, &volume->flags)) {
		clear_bit(AFS_VOLUME_NEEDS_UPDATE, &volume->flags);
		ret = afs_update_volume_status(volume, op->key);
		if (ret < 0)
			set_bit(AFS_VOLUME_NEEDS_UPDATE, &volume->flags);
		clear_bit_unlock(AFS_VOLUME_WAIT, &volume->flags);
		clear_bit_unlock(AFS_VOLUME_UPDATING, &volume->flags);
		wake_up_bit(&volume->flags, AFS_VOLUME_WAIT);
		_leave(" = %d", ret);
		return ret;
	}

wait:
	if (!test_bit(AFS_VOLUME_WAIT, &volume->flags)) {
		_leave(" = 0 [no wait]");
		return 0;
	}

	ret = wait_on_bit(&volume->flags, AFS_VOLUME_WAIT,
			  (op->flags & AFS_OPERATION_UNINTR) ?
			  TASK_UNINTERRUPTIBLE : TASK_INTERRUPTIBLE);
	if (ret == -ERESTARTSYS) {
		_leave(" = %d", ret);
		return ret;
	}

	retries++;
	if (retries == 4) {
		_leave(" = -ESTALE");
		return -ESTALE;
	}
	goto retry;
}
