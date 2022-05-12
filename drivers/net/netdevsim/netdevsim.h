/*
 * Copyright (C) 2017 Netronome Systems, Inc.
 *
 * This software is licensed under the GNU General License Version 2,
 * June 1991 as shown in the file COPYING in the top-level directory of this
 * source tree.
 *
 * THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM "AS IS"
 * WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE
 * OF THE PROGRAM IS WITH YOU. SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME
 * THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
 */

#include <linux/debugfs.h>
#include <linux/device.h>
#include <linux/ethtool.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/netdevice.h>
#include <linux/u64_stats_sync.h>
#include <net/devlink.h>
#include <net/udp_tunnel.h>
#include <net/xdp.h>

#define DRV_NAME	"netdevsim"

#define NSIM_XDP_MAX_MTU	4000

#define NSIM_EA(extack, msg)	NL_SET_ERR_MSG_MOD((extack), msg)

#define NSIM_IPSEC_MAX_SA_COUNT		33
#define NSIM_IPSEC_VALID		BIT(31)
#define NSIM_UDP_TUNNEL_N_PORTS		4

struct nsim_sa {
	struct xfrm_state *xs;
	__be32 ipaddr[4];
	u32 key[4];
	u32 salt;
	bool used;
	bool crypt;
	bool rx;
};

struct nsim_ipsec {
	struct nsim_sa sa[NSIM_IPSEC_MAX_SA_COUNT];
	struct dentry *pfile;
	u32 count;
	u32 tx;
	u32 ok;
};

struct nsim_ethtool_pauseparam {
	bool rx;
	bool tx;
	bool report_stats_rx;
	bool report_stats_tx;
};

struct nsim_ethtool {
	u32 get_err;
	u32 set_err;
	struct nsim_ethtool_pauseparam pauseparam;
	struct ethtool_coalesce coalesce;
	struct ethtool_ringparam ring;
	struct ethtool_fecparam fec;
};

struct netdevsim {
	struct net_device *netdev;
	struct nsim_dev *nsim_dev;
	struct nsim_dev_port *nsim_dev_port;

	u64 tx_packets;
	u64 tx_bytes;
	struct u64_stats_sync syncp;

	struct nsim_bus_dev *nsim_bus_dev;

	struct bpf_prog	*bpf_offloaded;
	u32 bpf_offloaded_id;

	struct xdp_attachment_info xdp;
	struct xdp_attachment_info xdp_hw;

	bool bpf_tc_accept;
	bool bpf_tc_non_bound_accept;
	bool bpf_xdpdrv_accept;
	bool bpf_xdpoffload_accept;

	bool bpf_map_accept;
	struct nsim_ipsec ipsec;
	struct {
		u32 inject_error;
		u32 sleep;
		u32 __ports[2][NSIM_UDP_TUNNEL_N_PORTS];
		u32 (*ports)[NSIM_UDP_TUNNEL_N_PORTS];
		struct debugfs_u32_array dfs_ports[2];
	} udp_ports;

	struct nsim_ethtool ethtool;
};

struct netdevsim *
nsim_create(struct nsim_dev *nsim_dev, struct nsim_dev_port *nsim_dev_port);
void nsim_destroy(struct netdevsim *ns);

void nsim_ethtool_init(struct netdevsim *ns);

void nsim_udp_tunnels_debugfs_create(struct nsim_dev *nsim_dev);
int nsim_udp_tunnels_info_create(struct nsim_dev *nsim_dev,
				 struct net_device *dev);
void nsim_udp_tunnels_info_destroy(struct net_device *dev);

#ifdef CONFIG_BPF_SYSCALL
int nsim_bpf_dev_init(struct nsim_dev *nsim_dev);
void nsim_bpf_dev_exit(struct nsim_dev *nsim_dev);
int nsim_bpf_init(struct netdevsim *ns);
void nsim_bpf_uninit(struct netdevsim *ns);
int nsim_bpf(struct net_device *dev, struct netdev_bpf *bpf);
int nsim_bpf_disable_tc(struct netdevsim *ns);
int nsim_bpf_setup_tc_block_cb(enum tc_setup_type type,
			       void *type_data, void *cb_priv);
#else

static inline int nsim_bpf_dev_init(struct nsim_dev *nsim_dev)
{
	return 0;
}

static inline void nsim_bpf_dev_exit(struct nsim_dev *nsim_dev)
{
}
static inline int nsim_bpf_init(struct netdevsim *ns)
{
	return 0;
}

static inline void nsim_bpf_uninit(struct netdevsim *ns)
{
}

static inline int nsim_bpf(struct net_device *dev, struct netdev_bpf *bpf)
{
	return -EOPNOTSUPP;
}

static inline int nsim_bpf_disable_tc(struct netdevsim *ns)
{
	return 0;
}

static inline int
nsim_bpf_setup_tc_block_cb(enum tc_setup_type type, void *type_data,
			   void *cb_priv)
{
	return -EOPNOTSUPP;
}
#endif

enum nsim_resource_id {
	NSIM_RESOURCE_NONE,   /* DEVLINK_RESOURCE_ID_PARENT_TOP */
	NSIM_RESOURCE_IPV4,
	NSIM_RESOURCE_IPV4_FIB,
	NSIM_RESOURCE_IPV4_FIB_RULES,
	NSIM_RESOURCE_IPV6,
	NSIM_RESOURCE_IPV6_FIB,
	NSIM_RESOURCE_IPV6_FIB_RULES,
	NSIM_RESOURCE_NEXTHOPS,
};

struct nsim_dev_health {
	struct devlink_health_reporter *empty_reporter;
	struct devlink_health_reporter *dummy_reporter;
	struct dentry *ddir;
	char *recovered_break_msg;
	u32 binary_len;
	bool fail_recover;
};

int nsim_dev_health_init(struct nsim_dev *nsim_dev, struct devlink *devlink);
void nsim_dev_health_exit(struct nsim_dev *nsim_dev);

#if IS_ENABLED(CONFIG_PSAMPLE)
int nsim_dev_psample_init(struct nsim_dev *nsim_dev);
void nsim_dev_psample_exit(struct nsim_dev *nsim_dev);
#else
static inline int nsim_dev_psample_init(struct nsim_dev *nsim_dev)
{
	return 0;
}

static inline void nsim_dev_psample_exit(struct nsim_dev *nsim_dev)
{
}
#endif

struct nsim_dev_port {
	struct list_head list;
	struct devlink_port devlink_port;
	unsigned int port_index;
	struct dentry *ddir;
	struct netdevsim *ns;
};

struct nsim_dev {
	struct nsim_bus_dev *nsim_bus_dev;
	struct nsim_fib_data *fib_data;
	struct nsim_trap_data *trap_data;
	struct dentry *ddir;
	struct dentry *ports_ddir;
	struct dentry *take_snapshot;
	struct bpf_offload_dev *bpf_dev;
	bool bpf_bind_accept;
	bool bpf_bind_verifier_accept;
	u32 bpf_bind_verifier_delay;
	struct dentry *ddir_bpf_bound_progs;
	u32 prog_id_gen;
	struct list_head bpf_bound_progs;
	struct list_head bpf_bound_maps;
	struct netdev_phys_item_id switch_id;
	struct list_head port_list;
	struct mutex port_list_lock; /* protects port list */
	bool fw_update_status;
	u32 fw_update_overwrite_mask;
	u32 max_macs;
	bool test1;
	bool dont_allow_reload;
	bool fail_reload;
	struct devlink_region *dummy_region;
	struct nsim_dev_health health;
	struct flow_action_cookie *fa_cookie;
	spinlock_t fa_cookie_lock; /* protects fa_cookie */
	bool fail_trap_group_set;
	bool fail_trap_policer_set;
	bool fail_trap_policer_counter_get;
	struct {
		struct udp_tunnel_nic_shared utn_shared;
		u32 __ports[2][NSIM_UDP_TUNNEL_N_PORTS];
		bool sync_all;
		bool open_only;
		bool ipv4_only;
		bool shared;
		bool static_iana_vxlan;
		u32 sleep;
	} udp_ports;
	struct nsim_dev_psample *psample;
};

static inline struct net *nsim_dev_net(struct nsim_dev *nsim_dev)
{
	return devlink_net(priv_to_devlink(nsim_dev));
}

int nsim_dev_init(void);
void nsim_dev_exit(void);
int nsim_dev_probe(struct nsim_bus_dev *nsim_bus_dev);
void nsim_dev_remove(struct nsim_bus_dev *nsim_bus_dev);
int nsim_dev_port_add(struct nsim_bus_dev *nsim_bus_dev,
		      unsigned int port_index);
int nsim_dev_port_del(struct nsim_bus_dev *nsim_bus_dev,
		      unsigned int port_index);

struct nsim_fib_data *nsim_fib_create(struct devlink *devlink,
				      struct netlink_ext_ack *extack);
void nsim_fib_destroy(struct devlink *devlink, struct nsim_fib_data *fib_data);
u64 nsim_fib_get_val(struct nsim_fib_data *fib_data,
		     enum nsim_resource_id res_id, bool max);

#if IS_ENABLED(CONFIG_XFRM_OFFLOAD)
void nsim_ipsec_init(struct netdevsim *ns);
void nsim_ipsec_teardown(struct netdevsim *ns);
bool nsim_ipsec_tx(struct netdevsim *ns, struct sk_buff *skb);
#else
static inline void nsim_ipsec_init(struct netdevsim *ns)
{
}

static inline void nsim_ipsec_teardown(struct netdevsim *ns)
{
}

static inline bool nsim_ipsec_tx(struct netdevsim *ns, struct sk_buff *skb)
{
	return true;
}
#endif

struct nsim_vf_config {
	int link_state;
	u16 min_tx_rate;
	u16 max_tx_rate;
	u16 vlan;
	__be16 vlan_proto;
	u16 qos;
	u8 vf_mac[ETH_ALEN];
	bool spoofchk_enabled;
	bool trusted;
	bool rss_query_enabled;
};

struct nsim_bus_dev {
	struct device dev;
	struct list_head list;
	unsigned int port_count;
	struct net *initial_net; /* Purpose of this is to carry net pointer
				  * during the probe time only.
				  */
	unsigned int num_vfs;
	struct nsim_vf_config *vfconfigs;
	/* Lock for devlink->reload_enabled in netdevsim module */
	struct mutex nsim_bus_reload_lock;
	bool init;
};

int nsim_bus_init(void);
void nsim_bus_exit(void);
