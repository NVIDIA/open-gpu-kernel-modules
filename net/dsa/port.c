// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Handling of a single switch port
 *
 * Copyright (c) 2017 Savoir-faire Linux Inc.
 *	Vivien Didelot <vivien.didelot@savoirfairelinux.com>
 */

#include <linux/if_bridge.h>
#include <linux/notifier.h>
#include <linux/of_mdio.h>
#include <linux/of_net.h>

#include "dsa_priv.h"

/**
 * dsa_port_notify - Notify the switching fabric of changes to a port
 * @dp: port on which change occurred
 * @e: event, must be of type DSA_NOTIFIER_*
 * @v: event-specific value.
 *
 * Notify all switches in the DSA tree that this port's switch belongs to,
 * including this switch itself, of an event. Allows the other switches to
 * reconfigure themselves for cross-chip operations. Can also be used to
 * reconfigure ports without net_devices (CPU ports, DSA links) whenever
 * a user port's state changes.
 */
static int dsa_port_notify(const struct dsa_port *dp, unsigned long e, void *v)
{
	return dsa_tree_notify(dp->ds->dst, e, v);
}

int dsa_port_set_state(struct dsa_port *dp, u8 state)
{
	struct dsa_switch *ds = dp->ds;
	int port = dp->index;

	if (!ds->ops->port_stp_state_set)
		return -EOPNOTSUPP;

	ds->ops->port_stp_state_set(ds, port, state);

	if (ds->ops->port_fast_age) {
		/* Fast age FDB entries or flush appropriate forwarding database
		 * for the given port, if we are moving it from Learning or
		 * Forwarding state, to Disabled or Blocking or Listening state.
		 */

		if ((dp->stp_state == BR_STATE_LEARNING ||
		     dp->stp_state == BR_STATE_FORWARDING) &&
		    (state == BR_STATE_DISABLED ||
		     state == BR_STATE_BLOCKING ||
		     state == BR_STATE_LISTENING))
			ds->ops->port_fast_age(ds, port);
	}

	dp->stp_state = state;

	return 0;
}

static void dsa_port_set_state_now(struct dsa_port *dp, u8 state)
{
	int err;

	err = dsa_port_set_state(dp, state);
	if (err)
		pr_err("DSA: failed to set STP state %u (%d)\n", state, err);
}

int dsa_port_enable_rt(struct dsa_port *dp, struct phy_device *phy)
{
	struct dsa_switch *ds = dp->ds;
	int port = dp->index;
	int err;

	if (ds->ops->port_enable) {
		err = ds->ops->port_enable(ds, port, phy);
		if (err)
			return err;
	}

	if (!dp->bridge_dev)
		dsa_port_set_state_now(dp, BR_STATE_FORWARDING);

	if (dp->pl)
		phylink_start(dp->pl);

	return 0;
}

int dsa_port_enable(struct dsa_port *dp, struct phy_device *phy)
{
	int err;

	rtnl_lock();
	err = dsa_port_enable_rt(dp, phy);
	rtnl_unlock();

	return err;
}

void dsa_port_disable_rt(struct dsa_port *dp)
{
	struct dsa_switch *ds = dp->ds;
	int port = dp->index;

	if (dp->pl)
		phylink_stop(dp->pl);

	if (!dp->bridge_dev)
		dsa_port_set_state_now(dp, BR_STATE_DISABLED);

	if (ds->ops->port_disable)
		ds->ops->port_disable(ds, port);
}

void dsa_port_disable(struct dsa_port *dp)
{
	rtnl_lock();
	dsa_port_disable_rt(dp);
	rtnl_unlock();
}

static int dsa_port_inherit_brport_flags(struct dsa_port *dp,
					 struct netlink_ext_ack *extack)
{
	const unsigned long mask = BR_LEARNING | BR_FLOOD | BR_MCAST_FLOOD |
				   BR_BCAST_FLOOD;
	struct net_device *brport_dev = dsa_port_to_bridge_port(dp);
	int flag, err;

	for_each_set_bit(flag, &mask, 32) {
		struct switchdev_brport_flags flags = {0};

		flags.mask = BIT(flag);

		if (br_port_flag_is_set(brport_dev, BIT(flag)))
			flags.val = BIT(flag);

		err = dsa_port_bridge_flags(dp, flags, extack);
		if (err && err != -EOPNOTSUPP)
			return err;
	}

	return 0;
}

static void dsa_port_clear_brport_flags(struct dsa_port *dp)
{
	const unsigned long val = BR_FLOOD | BR_MCAST_FLOOD | BR_BCAST_FLOOD;
	const unsigned long mask = BR_LEARNING | BR_FLOOD | BR_MCAST_FLOOD |
				   BR_BCAST_FLOOD;
	int flag, err;

	for_each_set_bit(flag, &mask, 32) {
		struct switchdev_brport_flags flags = {0};

		flags.mask = BIT(flag);
		flags.val = val & BIT(flag);

		err = dsa_port_bridge_flags(dp, flags, NULL);
		if (err && err != -EOPNOTSUPP)
			dev_err(dp->ds->dev,
				"failed to clear bridge port flag %lu: %pe\n",
				flags.val, ERR_PTR(err));
	}
}

static int dsa_port_switchdev_sync(struct dsa_port *dp,
				   struct netlink_ext_ack *extack)
{
	struct net_device *brport_dev = dsa_port_to_bridge_port(dp);
	struct net_device *br = dp->bridge_dev;
	int err;

	err = dsa_port_inherit_brport_flags(dp, extack);
	if (err)
		return err;

	err = dsa_port_set_state(dp, br_port_get_stp_state(brport_dev));
	if (err && err != -EOPNOTSUPP)
		return err;

	err = dsa_port_vlan_filtering(dp, br_vlan_enabled(br), extack);
	if (err && err != -EOPNOTSUPP)
		return err;

	err = dsa_port_mrouter(dp->cpu_dp, br_multicast_router(br), extack);
	if (err && err != -EOPNOTSUPP)
		return err;

	err = dsa_port_ageing_time(dp, br_get_ageing_time(br));
	if (err && err != -EOPNOTSUPP)
		return err;

	err = br_mdb_replay(br, brport_dev,
			    &dsa_slave_switchdev_blocking_notifier,
			    extack);
	if (err && err != -EOPNOTSUPP)
		return err;

	err = br_fdb_replay(br, brport_dev, &dsa_slave_switchdev_notifier);
	if (err && err != -EOPNOTSUPP)
		return err;

	err = br_vlan_replay(br, brport_dev,
			     &dsa_slave_switchdev_blocking_notifier,
			     extack);
	if (err && err != -EOPNOTSUPP)
		return err;

	return 0;
}

static void dsa_port_switchdev_unsync(struct dsa_port *dp)
{
	/* Configure the port for standalone mode (no address learning,
	 * flood everything).
	 * The bridge only emits SWITCHDEV_ATTR_ID_PORT_BRIDGE_FLAGS events
	 * when the user requests it through netlink or sysfs, but not
	 * automatically at port join or leave, so we need to handle resetting
	 * the brport flags ourselves. But we even prefer it that way, because
	 * otherwise, some setups might never get the notification they need,
	 * for example, when a port leaves a LAG that offloads the bridge,
	 * it becomes standalone, but as far as the bridge is concerned, no
	 * port ever left.
	 */
	dsa_port_clear_brport_flags(dp);

	/* Port left the bridge, put in BR_STATE_DISABLED by the bridge layer,
	 * so allow it to be in BR_STATE_FORWARDING to be kept functional
	 */
	dsa_port_set_state_now(dp, BR_STATE_FORWARDING);

	/* VLAN filtering is handled by dsa_switch_bridge_leave */

	/* Some drivers treat the notification for having a local multicast
	 * router by allowing multicast to be flooded to the CPU, so we should
	 * allow this in standalone mode too.
	 */
	dsa_port_mrouter(dp->cpu_dp, true, NULL);

	/* Ageing time may be global to the switch chip, so don't change it
	 * here because we have no good reason (or value) to change it to.
	 */
}

int dsa_port_bridge_join(struct dsa_port *dp, struct net_device *br,
			 struct netlink_ext_ack *extack)
{
	struct dsa_notifier_bridge_info info = {
		.tree_index = dp->ds->dst->index,
		.sw_index = dp->ds->index,
		.port = dp->index,
		.br = br,
	};
	int err;

	/* Here the interface is already bridged. Reflect the current
	 * configuration so that drivers can program their chips accordingly.
	 */
	dp->bridge_dev = br;

	err = dsa_broadcast(DSA_NOTIFIER_BRIDGE_JOIN, &info);
	if (err)
		goto out_rollback;

	err = dsa_port_switchdev_sync(dp, extack);
	if (err)
		goto out_rollback_unbridge;

	return 0;

out_rollback_unbridge:
	dsa_broadcast(DSA_NOTIFIER_BRIDGE_LEAVE, &info);
out_rollback:
	dp->bridge_dev = NULL;
	return err;
}

void dsa_port_bridge_leave(struct dsa_port *dp, struct net_device *br)
{
	struct dsa_notifier_bridge_info info = {
		.tree_index = dp->ds->dst->index,
		.sw_index = dp->ds->index,
		.port = dp->index,
		.br = br,
	};
	int err;

	/* Here the port is already unbridged. Reflect the current configuration
	 * so that drivers can program their chips accordingly.
	 */
	dp->bridge_dev = NULL;

	err = dsa_broadcast(DSA_NOTIFIER_BRIDGE_LEAVE, &info);
	if (err)
		pr_err("DSA: failed to notify DSA_NOTIFIER_BRIDGE_LEAVE\n");

	dsa_port_switchdev_unsync(dp);
}

int dsa_port_lag_change(struct dsa_port *dp,
			struct netdev_lag_lower_state_info *linfo)
{
	struct dsa_notifier_lag_info info = {
		.sw_index = dp->ds->index,
		.port = dp->index,
	};
	bool tx_enabled;

	if (!dp->lag_dev)
		return 0;

	/* On statically configured aggregates (e.g. loadbalance
	 * without LACP) ports will always be tx_enabled, even if the
	 * link is down. Thus we require both link_up and tx_enabled
	 * in order to include it in the tx set.
	 */
	tx_enabled = linfo->link_up && linfo->tx_enabled;

	if (tx_enabled == dp->lag_tx_enabled)
		return 0;

	dp->lag_tx_enabled = tx_enabled;

	return dsa_port_notify(dp, DSA_NOTIFIER_LAG_CHANGE, &info);
}

int dsa_port_lag_join(struct dsa_port *dp, struct net_device *lag,
		      struct netdev_lag_upper_info *uinfo,
		      struct netlink_ext_ack *extack)
{
	struct dsa_notifier_lag_info info = {
		.sw_index = dp->ds->index,
		.port = dp->index,
		.lag = lag,
		.info = uinfo,
	};
	struct net_device *bridge_dev;
	int err;

	dsa_lag_map(dp->ds->dst, lag);
	dp->lag_dev = lag;

	err = dsa_port_notify(dp, DSA_NOTIFIER_LAG_JOIN, &info);
	if (err)
		goto err_lag_join;

	bridge_dev = netdev_master_upper_dev_get(lag);
	if (!bridge_dev || !netif_is_bridge_master(bridge_dev))
		return 0;

	err = dsa_port_bridge_join(dp, bridge_dev, extack);
	if (err)
		goto err_bridge_join;

	return 0;

err_bridge_join:
	dsa_port_notify(dp, DSA_NOTIFIER_LAG_LEAVE, &info);
err_lag_join:
	dp->lag_dev = NULL;
	dsa_lag_unmap(dp->ds->dst, lag);
	return err;
}

void dsa_port_lag_leave(struct dsa_port *dp, struct net_device *lag)
{
	struct dsa_notifier_lag_info info = {
		.sw_index = dp->ds->index,
		.port = dp->index,
		.lag = lag,
	};
	int err;

	if (!dp->lag_dev)
		return;

	/* Port might have been part of a LAG that in turn was
	 * attached to a bridge.
	 */
	if (dp->bridge_dev)
		dsa_port_bridge_leave(dp, dp->bridge_dev);

	dp->lag_tx_enabled = false;
	dp->lag_dev = NULL;

	err = dsa_port_notify(dp, DSA_NOTIFIER_LAG_LEAVE, &info);
	if (err)
		pr_err("DSA: failed to notify DSA_NOTIFIER_LAG_LEAVE: %d\n",
		       err);

	dsa_lag_unmap(dp->ds->dst, lag);
}

/* Must be called under rcu_read_lock() */
static bool dsa_port_can_apply_vlan_filtering(struct dsa_port *dp,
					      bool vlan_filtering,
					      struct netlink_ext_ack *extack)
{
	struct dsa_switch *ds = dp->ds;
	int err, i;

	/* VLAN awareness was off, so the question is "can we turn it on".
	 * We may have had 8021q uppers, those need to go. Make sure we don't
	 * enter an inconsistent state: deny changing the VLAN awareness state
	 * as long as we have 8021q uppers.
	 */
	if (vlan_filtering && dsa_is_user_port(ds, dp->index)) {
		struct net_device *upper_dev, *slave = dp->slave;
		struct net_device *br = dp->bridge_dev;
		struct list_head *iter;

		netdev_for_each_upper_dev_rcu(slave, upper_dev, iter) {
			struct bridge_vlan_info br_info;
			u16 vid;

			if (!is_vlan_dev(upper_dev))
				continue;

			vid = vlan_dev_vlan_id(upper_dev);

			/* br_vlan_get_info() returns -EINVAL or -ENOENT if the
			 * device, respectively the VID is not found, returning
			 * 0 means success, which is a failure for us here.
			 */
			err = br_vlan_get_info(br, vid, &br_info);
			if (err == 0) {
				NL_SET_ERR_MSG_MOD(extack,
						   "Must first remove VLAN uppers having VIDs also present in bridge");
				return false;
			}
		}
	}

	if (!ds->vlan_filtering_is_global)
		return true;

	/* For cases where enabling/disabling VLAN awareness is global to the
	 * switch, we need to handle the case where multiple bridges span
	 * different ports of the same switch device and one of them has a
	 * different setting than what is being requested.
	 */
	for (i = 0; i < ds->num_ports; i++) {
		struct net_device *other_bridge;

		other_bridge = dsa_to_port(ds, i)->bridge_dev;
		if (!other_bridge)
			continue;
		/* If it's the same bridge, it also has same
		 * vlan_filtering setting => no need to check
		 */
		if (other_bridge == dp->bridge_dev)
			continue;
		if (br_vlan_enabled(other_bridge) != vlan_filtering) {
			NL_SET_ERR_MSG_MOD(extack,
					   "VLAN filtering is a global setting");
			return false;
		}
	}
	return true;
}

int dsa_port_vlan_filtering(struct dsa_port *dp, bool vlan_filtering,
			    struct netlink_ext_ack *extack)
{
	struct dsa_switch *ds = dp->ds;
	bool apply;
	int err;

	if (!ds->ops->port_vlan_filtering)
		return -EOPNOTSUPP;

	/* We are called from dsa_slave_switchdev_blocking_event(),
	 * which is not under rcu_read_lock(), unlike
	 * dsa_slave_switchdev_event().
	 */
	rcu_read_lock();
	apply = dsa_port_can_apply_vlan_filtering(dp, vlan_filtering, extack);
	rcu_read_unlock();
	if (!apply)
		return -EINVAL;

	if (dsa_port_is_vlan_filtering(dp) == vlan_filtering)
		return 0;

	err = ds->ops->port_vlan_filtering(ds, dp->index, vlan_filtering,
					   extack);
	if (err)
		return err;

	if (ds->vlan_filtering_is_global)
		ds->vlan_filtering = vlan_filtering;
	else
		dp->vlan_filtering = vlan_filtering;

	return 0;
}

/* This enforces legacy behavior for switch drivers which assume they can't
 * receive VLAN configuration when enslaved to a bridge with vlan_filtering=0
 */
bool dsa_port_skip_vlan_configuration(struct dsa_port *dp)
{
	struct dsa_switch *ds = dp->ds;

	if (!dp->bridge_dev)
		return false;

	return (!ds->configure_vlan_while_not_filtering &&
		!br_vlan_enabled(dp->bridge_dev));
}

int dsa_port_ageing_time(struct dsa_port *dp, clock_t ageing_clock)
{
	unsigned long ageing_jiffies = clock_t_to_jiffies(ageing_clock);
	unsigned int ageing_time = jiffies_to_msecs(ageing_jiffies);
	struct dsa_notifier_ageing_time_info info;
	int err;

	info.ageing_time = ageing_time;

	err = dsa_port_notify(dp, DSA_NOTIFIER_AGEING_TIME, &info);
	if (err)
		return err;

	dp->ageing_time = ageing_time;

	return 0;
}

int dsa_port_pre_bridge_flags(const struct dsa_port *dp,
			      struct switchdev_brport_flags flags,
			      struct netlink_ext_ack *extack)
{
	struct dsa_switch *ds = dp->ds;

	if (!ds->ops->port_pre_bridge_flags)
		return -EINVAL;

	return ds->ops->port_pre_bridge_flags(ds, dp->index, flags, extack);
}

int dsa_port_bridge_flags(const struct dsa_port *dp,
			  struct switchdev_brport_flags flags,
			  struct netlink_ext_ack *extack)
{
	struct dsa_switch *ds = dp->ds;

	if (!ds->ops->port_bridge_flags)
		return -EOPNOTSUPP;

	return ds->ops->port_bridge_flags(ds, dp->index, flags, extack);
}

int dsa_port_mrouter(struct dsa_port *dp, bool mrouter,
		     struct netlink_ext_ack *extack)
{
	struct dsa_switch *ds = dp->ds;

	if (!ds->ops->port_set_mrouter)
		return -EOPNOTSUPP;

	return ds->ops->port_set_mrouter(ds, dp->index, mrouter, extack);
}

int dsa_port_mtu_change(struct dsa_port *dp, int new_mtu,
			bool propagate_upstream)
{
	struct dsa_notifier_mtu_info info = {
		.sw_index = dp->ds->index,
		.propagate_upstream = propagate_upstream,
		.port = dp->index,
		.mtu = new_mtu,
	};

	return dsa_port_notify(dp, DSA_NOTIFIER_MTU, &info);
}

int dsa_port_fdb_add(struct dsa_port *dp, const unsigned char *addr,
		     u16 vid)
{
	struct dsa_notifier_fdb_info info = {
		.sw_index = dp->ds->index,
		.port = dp->index,
		.addr = addr,
		.vid = vid,
	};

	return dsa_port_notify(dp, DSA_NOTIFIER_FDB_ADD, &info);
}

int dsa_port_fdb_del(struct dsa_port *dp, const unsigned char *addr,
		     u16 vid)
{
	struct dsa_notifier_fdb_info info = {
		.sw_index = dp->ds->index,
		.port = dp->index,
		.addr = addr,
		.vid = vid,

	};

	return dsa_port_notify(dp, DSA_NOTIFIER_FDB_DEL, &info);
}

int dsa_port_fdb_dump(struct dsa_port *dp, dsa_fdb_dump_cb_t *cb, void *data)
{
	struct dsa_switch *ds = dp->ds;
	int port = dp->index;

	if (!ds->ops->port_fdb_dump)
		return -EOPNOTSUPP;

	return ds->ops->port_fdb_dump(ds, port, cb, data);
}

int dsa_port_mdb_add(const struct dsa_port *dp,
		     const struct switchdev_obj_port_mdb *mdb)
{
	struct dsa_notifier_mdb_info info = {
		.sw_index = dp->ds->index,
		.port = dp->index,
		.mdb = mdb,
	};

	return dsa_port_notify(dp, DSA_NOTIFIER_MDB_ADD, &info);
}

int dsa_port_mdb_del(const struct dsa_port *dp,
		     const struct switchdev_obj_port_mdb *mdb)
{
	struct dsa_notifier_mdb_info info = {
		.sw_index = dp->ds->index,
		.port = dp->index,
		.mdb = mdb,
	};

	return dsa_port_notify(dp, DSA_NOTIFIER_MDB_DEL, &info);
}

int dsa_port_vlan_add(struct dsa_port *dp,
		      const struct switchdev_obj_port_vlan *vlan,
		      struct netlink_ext_ack *extack)
{
	struct dsa_notifier_vlan_info info = {
		.sw_index = dp->ds->index,
		.port = dp->index,
		.vlan = vlan,
		.extack = extack,
	};

	return dsa_port_notify(dp, DSA_NOTIFIER_VLAN_ADD, &info);
}

int dsa_port_vlan_del(struct dsa_port *dp,
		      const struct switchdev_obj_port_vlan *vlan)
{
	struct dsa_notifier_vlan_info info = {
		.sw_index = dp->ds->index,
		.port = dp->index,
		.vlan = vlan,
	};

	return dsa_port_notify(dp, DSA_NOTIFIER_VLAN_DEL, &info);
}

int dsa_port_mrp_add(const struct dsa_port *dp,
		     const struct switchdev_obj_mrp *mrp)
{
	struct dsa_notifier_mrp_info info = {
		.sw_index = dp->ds->index,
		.port = dp->index,
		.mrp = mrp,
	};

	return dsa_port_notify(dp, DSA_NOTIFIER_MRP_ADD, &info);
}

int dsa_port_mrp_del(const struct dsa_port *dp,
		     const struct switchdev_obj_mrp *mrp)
{
	struct dsa_notifier_mrp_info info = {
		.sw_index = dp->ds->index,
		.port = dp->index,
		.mrp = mrp,
	};

	return dsa_port_notify(dp, DSA_NOTIFIER_MRP_DEL, &info);
}

int dsa_port_mrp_add_ring_role(const struct dsa_port *dp,
			       const struct switchdev_obj_ring_role_mrp *mrp)
{
	struct dsa_notifier_mrp_ring_role_info info = {
		.sw_index = dp->ds->index,
		.port = dp->index,
		.mrp = mrp,
	};

	return dsa_port_notify(dp, DSA_NOTIFIER_MRP_ADD_RING_ROLE, &info);
}

int dsa_port_mrp_del_ring_role(const struct dsa_port *dp,
			       const struct switchdev_obj_ring_role_mrp *mrp)
{
	struct dsa_notifier_mrp_ring_role_info info = {
		.sw_index = dp->ds->index,
		.port = dp->index,
		.mrp = mrp,
	};

	return dsa_port_notify(dp, DSA_NOTIFIER_MRP_DEL_RING_ROLE, &info);
}

void dsa_port_set_tag_protocol(struct dsa_port *cpu_dp,
			       const struct dsa_device_ops *tag_ops)
{
	cpu_dp->filter = tag_ops->filter;
	cpu_dp->rcv = tag_ops->rcv;
	cpu_dp->tag_ops = tag_ops;
}

static struct phy_device *dsa_port_get_phy_device(struct dsa_port *dp)
{
	struct device_node *phy_dn;
	struct phy_device *phydev;

	phy_dn = of_parse_phandle(dp->dn, "phy-handle", 0);
	if (!phy_dn)
		return NULL;

	phydev = of_phy_find_device(phy_dn);
	if (!phydev) {
		of_node_put(phy_dn);
		return ERR_PTR(-EPROBE_DEFER);
	}

	of_node_put(phy_dn);
	return phydev;
}

static void dsa_port_phylink_validate(struct phylink_config *config,
				      unsigned long *supported,
				      struct phylink_link_state *state)
{
	struct dsa_port *dp = container_of(config, struct dsa_port, pl_config);
	struct dsa_switch *ds = dp->ds;

	if (!ds->ops->phylink_validate)
		return;

	ds->ops->phylink_validate(ds, dp->index, supported, state);
}

static void dsa_port_phylink_mac_pcs_get_state(struct phylink_config *config,
					       struct phylink_link_state *state)
{
	struct dsa_port *dp = container_of(config, struct dsa_port, pl_config);
	struct dsa_switch *ds = dp->ds;
	int err;

	/* Only called for inband modes */
	if (!ds->ops->phylink_mac_link_state) {
		state->link = 0;
		return;
	}

	err = ds->ops->phylink_mac_link_state(ds, dp->index, state);
	if (err < 0) {
		dev_err(ds->dev, "p%d: phylink_mac_link_state() failed: %d\n",
			dp->index, err);
		state->link = 0;
	}
}

static void dsa_port_phylink_mac_config(struct phylink_config *config,
					unsigned int mode,
					const struct phylink_link_state *state)
{
	struct dsa_port *dp = container_of(config, struct dsa_port, pl_config);
	struct dsa_switch *ds = dp->ds;

	if (!ds->ops->phylink_mac_config)
		return;

	ds->ops->phylink_mac_config(ds, dp->index, mode, state);
}

static void dsa_port_phylink_mac_an_restart(struct phylink_config *config)
{
	struct dsa_port *dp = container_of(config, struct dsa_port, pl_config);
	struct dsa_switch *ds = dp->ds;

	if (!ds->ops->phylink_mac_an_restart)
		return;

	ds->ops->phylink_mac_an_restart(ds, dp->index);
}

static void dsa_port_phylink_mac_link_down(struct phylink_config *config,
					   unsigned int mode,
					   phy_interface_t interface)
{
	struct dsa_port *dp = container_of(config, struct dsa_port, pl_config);
	struct phy_device *phydev = NULL;
	struct dsa_switch *ds = dp->ds;

	if (dsa_is_user_port(ds, dp->index))
		phydev = dp->slave->phydev;

	if (!ds->ops->phylink_mac_link_down) {
		if (ds->ops->adjust_link && phydev)
			ds->ops->adjust_link(ds, dp->index, phydev);
		return;
	}

	ds->ops->phylink_mac_link_down(ds, dp->index, mode, interface);
}

static void dsa_port_phylink_mac_link_up(struct phylink_config *config,
					 struct phy_device *phydev,
					 unsigned int mode,
					 phy_interface_t interface,
					 int speed, int duplex,
					 bool tx_pause, bool rx_pause)
{
	struct dsa_port *dp = container_of(config, struct dsa_port, pl_config);
	struct dsa_switch *ds = dp->ds;

	if (!ds->ops->phylink_mac_link_up) {
		if (ds->ops->adjust_link && phydev)
			ds->ops->adjust_link(ds, dp->index, phydev);
		return;
	}

	ds->ops->phylink_mac_link_up(ds, dp->index, mode, interface, phydev,
				     speed, duplex, tx_pause, rx_pause);
}

const struct phylink_mac_ops dsa_port_phylink_mac_ops = {
	.validate = dsa_port_phylink_validate,
	.mac_pcs_get_state = dsa_port_phylink_mac_pcs_get_state,
	.mac_config = dsa_port_phylink_mac_config,
	.mac_an_restart = dsa_port_phylink_mac_an_restart,
	.mac_link_down = dsa_port_phylink_mac_link_down,
	.mac_link_up = dsa_port_phylink_mac_link_up,
};

static int dsa_port_setup_phy_of(struct dsa_port *dp, bool enable)
{
	struct dsa_switch *ds = dp->ds;
	struct phy_device *phydev;
	int port = dp->index;
	int err = 0;

	phydev = dsa_port_get_phy_device(dp);
	if (!phydev)
		return 0;

	if (IS_ERR(phydev))
		return PTR_ERR(phydev);

	if (enable) {
		err = genphy_resume(phydev);
		if (err < 0)
			goto err_put_dev;

		err = genphy_read_status(phydev);
		if (err < 0)
			goto err_put_dev;
	} else {
		err = genphy_suspend(phydev);
		if (err < 0)
			goto err_put_dev;
	}

	if (ds->ops->adjust_link)
		ds->ops->adjust_link(ds, port, phydev);

	dev_dbg(ds->dev, "enabled port's phy: %s", phydev_name(phydev));

err_put_dev:
	put_device(&phydev->mdio.dev);
	return err;
}

static int dsa_port_fixed_link_register_of(struct dsa_port *dp)
{
	struct device_node *dn = dp->dn;
	struct dsa_switch *ds = dp->ds;
	struct phy_device *phydev;
	int port = dp->index;
	phy_interface_t mode;
	int err;

	err = of_phy_register_fixed_link(dn);
	if (err) {
		dev_err(ds->dev,
			"failed to register the fixed PHY of port %d\n",
			port);
		return err;
	}

	phydev = of_phy_find_device(dn);

	err = of_get_phy_mode(dn, &mode);
	if (err)
		mode = PHY_INTERFACE_MODE_NA;
	phydev->interface = mode;

	genphy_read_status(phydev);

	if (ds->ops->adjust_link)
		ds->ops->adjust_link(ds, port, phydev);

	put_device(&phydev->mdio.dev);

	return 0;
}

static int dsa_port_phylink_register(struct dsa_port *dp)
{
	struct dsa_switch *ds = dp->ds;
	struct device_node *port_dn = dp->dn;
	phy_interface_t mode;
	int err;

	err = of_get_phy_mode(port_dn, &mode);
	if (err)
		mode = PHY_INTERFACE_MODE_NA;

	dp->pl_config.dev = ds->dev;
	dp->pl_config.type = PHYLINK_DEV;
	dp->pl_config.pcs_poll = ds->pcs_poll;

	dp->pl = phylink_create(&dp->pl_config, of_fwnode_handle(port_dn),
				mode, &dsa_port_phylink_mac_ops);
	if (IS_ERR(dp->pl)) {
		pr_err("error creating PHYLINK: %ld\n", PTR_ERR(dp->pl));
		return PTR_ERR(dp->pl);
	}

	err = phylink_of_phy_connect(dp->pl, port_dn, 0);
	if (err && err != -ENODEV) {
		pr_err("could not attach to PHY: %d\n", err);
		goto err_phy_connect;
	}

	return 0;

err_phy_connect:
	phylink_destroy(dp->pl);
	return err;
}

int dsa_port_link_register_of(struct dsa_port *dp)
{
	struct dsa_switch *ds = dp->ds;
	struct device_node *phy_np;
	int port = dp->index;

	if (!ds->ops->adjust_link) {
		phy_np = of_parse_phandle(dp->dn, "phy-handle", 0);
		if (of_phy_is_fixed_link(dp->dn) || phy_np) {
			if (ds->ops->phylink_mac_link_down)
				ds->ops->phylink_mac_link_down(ds, port,
					MLO_AN_FIXED, PHY_INTERFACE_MODE_NA);
			return dsa_port_phylink_register(dp);
		}
		return 0;
	}

	dev_warn(ds->dev,
		 "Using legacy PHYLIB callbacks. Please migrate to PHYLINK!\n");

	if (of_phy_is_fixed_link(dp->dn))
		return dsa_port_fixed_link_register_of(dp);
	else
		return dsa_port_setup_phy_of(dp, true);
}

void dsa_port_link_unregister_of(struct dsa_port *dp)
{
	struct dsa_switch *ds = dp->ds;

	if (!ds->ops->adjust_link && dp->pl) {
		rtnl_lock();
		phylink_disconnect_phy(dp->pl);
		rtnl_unlock();
		phylink_destroy(dp->pl);
		dp->pl = NULL;
		return;
	}

	if (of_phy_is_fixed_link(dp->dn))
		of_phy_deregister_fixed_link(dp->dn);
	else
		dsa_port_setup_phy_of(dp, false);
}

int dsa_port_get_phy_strings(struct dsa_port *dp, uint8_t *data)
{
	struct phy_device *phydev;
	int ret = -EOPNOTSUPP;

	if (of_phy_is_fixed_link(dp->dn))
		return ret;

	phydev = dsa_port_get_phy_device(dp);
	if (IS_ERR_OR_NULL(phydev))
		return ret;

	ret = phy_ethtool_get_strings(phydev, data);
	put_device(&phydev->mdio.dev);

	return ret;
}
EXPORT_SYMBOL_GPL(dsa_port_get_phy_strings);

int dsa_port_get_ethtool_phy_stats(struct dsa_port *dp, uint64_t *data)
{
	struct phy_device *phydev;
	int ret = -EOPNOTSUPP;

	if (of_phy_is_fixed_link(dp->dn))
		return ret;

	phydev = dsa_port_get_phy_device(dp);
	if (IS_ERR_OR_NULL(phydev))
		return ret;

	ret = phy_ethtool_get_stats(phydev, NULL, data);
	put_device(&phydev->mdio.dev);

	return ret;
}
EXPORT_SYMBOL_GPL(dsa_port_get_ethtool_phy_stats);

int dsa_port_get_phy_sset_count(struct dsa_port *dp)
{
	struct phy_device *phydev;
	int ret = -EOPNOTSUPP;

	if (of_phy_is_fixed_link(dp->dn))
		return ret;

	phydev = dsa_port_get_phy_device(dp);
	if (IS_ERR_OR_NULL(phydev))
		return ret;

	ret = phy_ethtool_get_sset_count(phydev);
	put_device(&phydev->mdio.dev);

	return ret;
}
EXPORT_SYMBOL_GPL(dsa_port_get_phy_sset_count);

int dsa_port_hsr_join(struct dsa_port *dp, struct net_device *hsr)
{
	struct dsa_notifier_hsr_info info = {
		.sw_index = dp->ds->index,
		.port = dp->index,
		.hsr = hsr,
	};
	int err;

	dp->hsr_dev = hsr;

	err = dsa_port_notify(dp, DSA_NOTIFIER_HSR_JOIN, &info);
	if (err)
		dp->hsr_dev = NULL;

	return err;
}

void dsa_port_hsr_leave(struct dsa_port *dp, struct net_device *hsr)
{
	struct dsa_notifier_hsr_info info = {
		.sw_index = dp->ds->index,
		.port = dp->index,
		.hsr = hsr,
	};
	int err;

	dp->hsr_dev = NULL;

	err = dsa_port_notify(dp, DSA_NOTIFIER_HSR_LEAVE, &info);
	if (err)
		pr_err("DSA: failed to notify DSA_NOTIFIER_HSR_LEAVE\n");
}
