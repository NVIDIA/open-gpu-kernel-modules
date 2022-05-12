/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * nodemanager.h
 *
 * Function prototypes
 *
 * Copyright (C) 2004 Oracle.  All rights reserved.
 */

#ifndef O2CLUSTER_NODEMANAGER_H
#define O2CLUSTER_NODEMANAGER_H

#include "ocfs2_nodemanager.h"

/* This totally doesn't belong here. */
#include <linux/configfs.h>
#include <linux/rbtree.h>

enum o2nm_fence_method {
	O2NM_FENCE_RESET	= 0,
	O2NM_FENCE_PANIC,
	O2NM_FENCE_METHODS,	/* Number of fence methods */
};

struct o2nm_node {
	spinlock_t		nd_lock;
	struct config_item	nd_item;
	char			nd_name[O2NM_MAX_NAME_LEN+1]; /* replace? */
	__u8			nd_num;
	/* only one address per node, as attributes, for now. */
	__be32			nd_ipv4_address;
	__be16			nd_ipv4_port;
	struct rb_node		nd_ip_node;
	/* there can be only one local node for now */
	int			nd_local;

	unsigned long		nd_set_attributes;
};

struct o2nm_cluster {
	struct config_group	cl_group;
	unsigned		cl_has_local:1;
	u8			cl_local_node;
	rwlock_t		cl_nodes_lock;
	struct o2nm_node  	*cl_nodes[O2NM_MAX_NODES];
	struct rb_root		cl_node_ip_tree;
	unsigned int		cl_idle_timeout_ms;
	unsigned int		cl_keepalive_delay_ms;
	unsigned int		cl_reconnect_delay_ms;
	enum o2nm_fence_method	cl_fence_method;

	/* this bitmap is part of a hack for disk bitmap.. will go eventually. - zab */
	unsigned long	cl_nodes_bitmap[BITS_TO_LONGS(O2NM_MAX_NODES)];
};

extern struct o2nm_cluster *o2nm_single_cluster;

u8 o2nm_this_node(void);

int o2nm_configured_node_map(unsigned long *map, unsigned bytes);
struct o2nm_node *o2nm_get_node_by_num(u8 node_num);
struct o2nm_node *o2nm_get_node_by_ip(__be32 addr);
void o2nm_node_get(struct o2nm_node *node);
void o2nm_node_put(struct o2nm_node *node);

int o2nm_depend_item(struct config_item *item);
void o2nm_undepend_item(struct config_item *item);
int o2nm_depend_this_node(void);
void o2nm_undepend_this_node(void);

#endif /* O2CLUSTER_NODEMANAGER_H */
