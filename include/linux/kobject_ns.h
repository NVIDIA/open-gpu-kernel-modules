// SPDX-License-Identifier: GPL-2.0
/* Kernel object name space definitions
 *
 * Copyright (c) 2002-2003 Patrick Mochel
 * Copyright (c) 2002-2003 Open Source Development Labs
 * Copyright (c) 2006-2008 Greg Kroah-Hartman <greg@kroah.com>
 * Copyright (c) 2006-2008 Novell Inc.
 *
 * Split from kobject.h by David Howells (dhowells@redhat.com)
 *
 * Please read Documentation/core-api/kobject.rst before using the kobject
 * interface, ESPECIALLY the parts about reference counts and object
 * destructors.
 */

#ifndef _LINUX_KOBJECT_NS_H
#define _LINUX_KOBJECT_NS_H

struct sock;
struct kobject;

/*
 * Namespace types which are used to tag kobjects and sysfs entries.
 * Network namespace will likely be the first.
 */
enum kobj_ns_type {
	KOBJ_NS_TYPE_NONE = 0,
	KOBJ_NS_TYPE_NET,
	KOBJ_NS_TYPES
};

/*
 * Callbacks so sysfs can determine namespaces
 *   @grab_current_ns: return a new reference to calling task's namespace
 *   @netlink_ns: return namespace to which a sock belongs (right?)
 *   @initial_ns: return the initial namespace (i.e. init_net_ns)
 *   @drop_ns: drops a reference to namespace
 */
struct kobj_ns_type_operations {
	enum kobj_ns_type type;
	bool (*current_may_mount)(void);
	void *(*grab_current_ns)(void);
	const void *(*netlink_ns)(struct sock *sk);
	const void *(*initial_ns)(void);
	void (*drop_ns)(void *);
};

int kobj_ns_type_register(const struct kobj_ns_type_operations *ops);
int kobj_ns_type_registered(enum kobj_ns_type type);
const struct kobj_ns_type_operations *kobj_child_ns_ops(struct kobject *parent);
const struct kobj_ns_type_operations *kobj_ns_ops(struct kobject *kobj);

bool kobj_ns_current_may_mount(enum kobj_ns_type type);
void *kobj_ns_grab_current(enum kobj_ns_type type);
const void *kobj_ns_netlink(enum kobj_ns_type type, struct sock *sk);
const void *kobj_ns_initial(enum kobj_ns_type type);
void kobj_ns_drop(enum kobj_ns_type type, void *ns);

#endif /* _LINUX_KOBJECT_NS_H */
