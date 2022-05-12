/*
 * Copyright (c) 2004 Topspin Communications.  All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _CORE_PRIV_H
#define _CORE_PRIV_H

#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/cgroup_rdma.h>
#include <net/net_namespace.h>
#include <net/netns/generic.h>

#include <rdma/ib_verbs.h>
#include <rdma/opa_addr.h>
#include <rdma/ib_mad.h>
#include <rdma/restrack.h>
#include "mad_priv.h"
#include "restrack.h"

/* Total number of ports combined across all struct ib_devices's */
#define RDMA_MAX_PORTS 8192

struct pkey_index_qp_list {
	struct list_head    pkey_index_list;
	u16                 pkey_index;
	/* Lock to hold while iterating the qp_list. */
	spinlock_t          qp_list_lock;
	struct list_head    qp_list;
};

/**
 * struct rdma_dev_net - rdma net namespace metadata for a net
 * @nl_sock:	Pointer to netlink socket
 * @net:	Pointer to owner net namespace
 * @id:		xarray id to identify the net namespace.
 */
struct rdma_dev_net {
	struct sock *nl_sock;
	possible_net_t net;
	u32 id;
};

extern const struct attribute_group ib_dev_attr_group;
extern bool ib_devices_shared_netns;
extern unsigned int rdma_dev_net_id;

static inline struct rdma_dev_net *rdma_net_to_dev_net(struct net *net)
{
	return net_generic(net, rdma_dev_net_id);
}

int ib_device_register_sysfs(struct ib_device *device);
void ib_device_unregister_sysfs(struct ib_device *device);
int ib_device_rename(struct ib_device *ibdev, const char *name);
int ib_device_set_dim(struct ib_device *ibdev, u8 use_dim);

typedef void (*roce_netdev_callback)(struct ib_device *device, u32 port,
	      struct net_device *idev, void *cookie);

typedef bool (*roce_netdev_filter)(struct ib_device *device, u32 port,
				   struct net_device *idev, void *cookie);

struct net_device *ib_device_get_netdev(struct ib_device *ib_dev,
					u32 port);

void ib_enum_roce_netdev(struct ib_device *ib_dev,
			 roce_netdev_filter filter,
			 void *filter_cookie,
			 roce_netdev_callback cb,
			 void *cookie);
void ib_enum_all_roce_netdevs(roce_netdev_filter filter,
			      void *filter_cookie,
			      roce_netdev_callback cb,
			      void *cookie);

typedef int (*nldev_callback)(struct ib_device *device,
			      struct sk_buff *skb,
			      struct netlink_callback *cb,
			      unsigned int idx);

int ib_enum_all_devs(nldev_callback nldev_cb, struct sk_buff *skb,
		     struct netlink_callback *cb);

struct ib_client_nl_info {
	struct sk_buff *nl_msg;
	struct device *cdev;
	u32 port;
	u64 abi;
};
int ib_get_client_nl_info(struct ib_device *ibdev, const char *client_name,
			  struct ib_client_nl_info *res);

enum ib_cache_gid_default_mode {
	IB_CACHE_GID_DEFAULT_MODE_SET,
	IB_CACHE_GID_DEFAULT_MODE_DELETE
};

int ib_cache_gid_parse_type_str(const char *buf);

const char *ib_cache_gid_type_str(enum ib_gid_type gid_type);

void ib_cache_gid_set_default_gid(struct ib_device *ib_dev, u32 port,
				  struct net_device *ndev,
				  unsigned long gid_type_mask,
				  enum ib_cache_gid_default_mode mode);

int ib_cache_gid_add(struct ib_device *ib_dev, u32 port,
		     union ib_gid *gid, struct ib_gid_attr *attr);

int ib_cache_gid_del(struct ib_device *ib_dev, u32 port,
		     union ib_gid *gid, struct ib_gid_attr *attr);

int ib_cache_gid_del_all_netdev_gids(struct ib_device *ib_dev, u32 port,
				     struct net_device *ndev);

int roce_gid_mgmt_init(void);
void roce_gid_mgmt_cleanup(void);

unsigned long roce_gid_type_mask_support(struct ib_device *ib_dev, u32 port);

int ib_cache_setup_one(struct ib_device *device);
void ib_cache_cleanup_one(struct ib_device *device);
void ib_cache_release_one(struct ib_device *device);
void ib_dispatch_event_clients(struct ib_event *event);

#ifdef CONFIG_CGROUP_RDMA
void ib_device_register_rdmacg(struct ib_device *device);
void ib_device_unregister_rdmacg(struct ib_device *device);

int ib_rdmacg_try_charge(struct ib_rdmacg_object *cg_obj,
			 struct ib_device *device,
			 enum rdmacg_resource_type resource_index);

void ib_rdmacg_uncharge(struct ib_rdmacg_object *cg_obj,
			struct ib_device *device,
			enum rdmacg_resource_type resource_index);
#else
static inline void ib_device_register_rdmacg(struct ib_device *device)
{
}

static inline void ib_device_unregister_rdmacg(struct ib_device *device)
{
}

static inline int ib_rdmacg_try_charge(struct ib_rdmacg_object *cg_obj,
				       struct ib_device *device,
				       enum rdmacg_resource_type resource_index)
{
	return 0;
}

static inline void ib_rdmacg_uncharge(struct ib_rdmacg_object *cg_obj,
				      struct ib_device *device,
				      enum rdmacg_resource_type resource_index)
{
}
#endif

static inline bool rdma_is_upper_dev_rcu(struct net_device *dev,
					 struct net_device *upper)
{
	return netdev_has_upper_dev_all_rcu(dev, upper);
}

int addr_init(void);
void addr_cleanup(void);

int ib_mad_init(void);
void ib_mad_cleanup(void);

int ib_sa_init(void);
void ib_sa_cleanup(void);

void rdma_nl_init(void);
void rdma_nl_exit(void);

int ib_nl_handle_resolve_resp(struct sk_buff *skb,
			      struct nlmsghdr *nlh,
			      struct netlink_ext_ack *extack);
int ib_nl_handle_set_timeout(struct sk_buff *skb,
			     struct nlmsghdr *nlh,
			     struct netlink_ext_ack *extack);
int ib_nl_handle_ip_res_resp(struct sk_buff *skb,
			     struct nlmsghdr *nlh,
			     struct netlink_ext_ack *extack);

int ib_get_cached_subnet_prefix(struct ib_device *device,
				u32 port_num,
				u64 *sn_pfx);

#ifdef CONFIG_SECURITY_INFINIBAND
void ib_security_release_port_pkey_list(struct ib_device *device);

void ib_security_cache_change(struct ib_device *device,
			      u32 port_num,
			      u64 subnet_prefix);

int ib_security_modify_qp(struct ib_qp *qp,
			  struct ib_qp_attr *qp_attr,
			  int qp_attr_mask,
			  struct ib_udata *udata);

int ib_create_qp_security(struct ib_qp *qp, struct ib_device *dev);
void ib_destroy_qp_security_begin(struct ib_qp_security *sec);
void ib_destroy_qp_security_abort(struct ib_qp_security *sec);
void ib_destroy_qp_security_end(struct ib_qp_security *sec);
int ib_open_shared_qp_security(struct ib_qp *qp, struct ib_device *dev);
void ib_close_shared_qp_security(struct ib_qp_security *sec);
int ib_mad_agent_security_setup(struct ib_mad_agent *agent,
				enum ib_qp_type qp_type);
void ib_mad_agent_security_cleanup(struct ib_mad_agent *agent);
int ib_mad_enforce_security(struct ib_mad_agent_private *map, u16 pkey_index);
void ib_mad_agent_security_change(void);
#else
static inline void ib_security_release_port_pkey_list(struct ib_device *device)
{
}

static inline void ib_security_cache_change(struct ib_device *device,
					    u32 port_num,
					    u64 subnet_prefix)
{
}

static inline int ib_security_modify_qp(struct ib_qp *qp,
					struct ib_qp_attr *qp_attr,
					int qp_attr_mask,
					struct ib_udata *udata)
{
	return qp->device->ops.modify_qp(qp->real_qp,
					 qp_attr,
					 qp_attr_mask,
					 udata);
}

static inline int ib_create_qp_security(struct ib_qp *qp,
					struct ib_device *dev)
{
	return 0;
}

static inline void ib_destroy_qp_security_begin(struct ib_qp_security *sec)
{
}

static inline void ib_destroy_qp_security_abort(struct ib_qp_security *sec)
{
}

static inline void ib_destroy_qp_security_end(struct ib_qp_security *sec)
{
}

static inline int ib_open_shared_qp_security(struct ib_qp *qp,
					     struct ib_device *dev)
{
	return 0;
}

static inline void ib_close_shared_qp_security(struct ib_qp_security *sec)
{
}

static inline int ib_mad_agent_security_setup(struct ib_mad_agent *agent,
					      enum ib_qp_type qp_type)
{
	return 0;
}

static inline void ib_mad_agent_security_cleanup(struct ib_mad_agent *agent)
{
}

static inline int ib_mad_enforce_security(struct ib_mad_agent_private *map,
					  u16 pkey_index)
{
	return 0;
}

static inline void ib_mad_agent_security_change(void)
{
}
#endif

struct ib_device *ib_device_get_by_index(const struct net *net, u32 index);

/* RDMA device netlink */
void nldev_init(void);
void nldev_exit(void);

static inline struct ib_qp *
_ib_create_qp(struct ib_device *dev, struct ib_pd *pd,
	      struct ib_qp_init_attr *attr, struct ib_udata *udata,
	      struct ib_uqp_object *uobj, const char *caller)
{
	struct ib_qp *qp;

	if (!dev->ops.create_qp)
		return ERR_PTR(-EOPNOTSUPP);

	qp = dev->ops.create_qp(pd, attr, udata);
	if (IS_ERR(qp))
		return qp;

	qp->device = dev;
	qp->pd = pd;
	qp->uobject = uobj;
	qp->real_qp = qp;

	qp->qp_type = attr->qp_type;
	qp->rwq_ind_tbl = attr->rwq_ind_tbl;
	qp->send_cq = attr->send_cq;
	qp->recv_cq = attr->recv_cq;
	qp->srq = attr->srq;
	qp->rwq_ind_tbl = attr->rwq_ind_tbl;
	qp->event_handler = attr->event_handler;
	qp->port = attr->port_num;

	atomic_set(&qp->usecnt, 0);
	spin_lock_init(&qp->mr_lock);
	INIT_LIST_HEAD(&qp->rdma_mrs);
	INIT_LIST_HEAD(&qp->sig_mrs);

	rdma_restrack_new(&qp->res, RDMA_RESTRACK_QP);
	WARN_ONCE(!udata && !caller, "Missing kernel QP owner");
	rdma_restrack_set_name(&qp->res, udata ? NULL : caller);
	rdma_restrack_add(&qp->res);
	return qp;
}

struct rdma_dev_addr;
int rdma_resolve_ip_route(struct sockaddr *src_addr,
			  const struct sockaddr *dst_addr,
			  struct rdma_dev_addr *addr);

int rdma_addr_find_l2_eth_by_grh(const union ib_gid *sgid,
				 const union ib_gid *dgid,
				 u8 *dmac, const struct ib_gid_attr *sgid_attr,
				 int *hoplimit);
void rdma_copy_src_l2_addr(struct rdma_dev_addr *dev_addr,
			   const struct net_device *dev);

struct sa_path_rec;
int roce_resolve_route_from_path(struct sa_path_rec *rec,
				 const struct ib_gid_attr *attr);

struct net_device *rdma_read_gid_attr_ndev_rcu(const struct ib_gid_attr *attr);

void ib_free_port_attrs(struct ib_core_device *coredev);
int ib_setup_port_attrs(struct ib_core_device *coredev);

int rdma_compatdev_set(u8 enable);

int ib_port_register_module_stat(struct ib_device *device, u32 port_num,
				 struct kobject *kobj, struct kobj_type *ktype,
				 const char *name);
void ib_port_unregister_module_stat(struct kobject *kobj);

int ib_device_set_netns_put(struct sk_buff *skb,
			    struct ib_device *dev, u32 ns_fd);

int rdma_nl_net_init(struct rdma_dev_net *rnet);
void rdma_nl_net_exit(struct rdma_dev_net *rnet);

struct rdma_umap_priv {
	struct vm_area_struct *vma;
	struct list_head list;
	struct rdma_user_mmap_entry *entry;
};

void rdma_umap_priv_init(struct rdma_umap_priv *priv,
			 struct vm_area_struct *vma,
			 struct rdma_user_mmap_entry *entry);

void ib_cq_pool_cleanup(struct ib_device *dev);

#endif /* _CORE_PRIV_H */
