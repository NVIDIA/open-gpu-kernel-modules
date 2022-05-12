// SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0
/* Copyright (c) 2019-2020 Marvell International Ltd. All rights reserved */

#include <linux/etherdevice.h>
#include <linux/jiffies.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/netdev_features.h>
#include <linux/of.h>
#include <linux/of_net.h>

#include "prestera.h"
#include "prestera_hw.h"
#include "prestera_rxtx.h"
#include "prestera_devlink.h"
#include "prestera_ethtool.h"
#include "prestera_switchdev.h"

#define PRESTERA_MTU_DEFAULT	1536

#define PRESTERA_STATS_DELAY_MS	1000

#define PRESTERA_MAC_ADDR_NUM_MAX	255

static struct workqueue_struct *prestera_wq;

int prestera_port_pvid_set(struct prestera_port *port, u16 vid)
{
	enum prestera_accept_frm_type frm_type;
	int err;

	frm_type = PRESTERA_ACCEPT_FRAME_TYPE_TAGGED;

	if (vid) {
		err = prestera_hw_vlan_port_vid_set(port, vid);
		if (err)
			return err;

		frm_type = PRESTERA_ACCEPT_FRAME_TYPE_ALL;
	}

	err = prestera_hw_port_accept_frm_type(port, frm_type);
	if (err && frm_type == PRESTERA_ACCEPT_FRAME_TYPE_ALL)
		prestera_hw_vlan_port_vid_set(port, port->pvid);

	port->pvid = vid;
	return 0;
}

struct prestera_port *prestera_port_find_by_hwid(struct prestera_switch *sw,
						 u32 dev_id, u32 hw_id)
{
	struct prestera_port *port = NULL;

	read_lock(&sw->port_list_lock);
	list_for_each_entry(port, &sw->port_list, list) {
		if (port->dev_id == dev_id && port->hw_id == hw_id)
			break;
	}
	read_unlock(&sw->port_list_lock);

	return port;
}

struct prestera_port *prestera_find_port(struct prestera_switch *sw, u32 id)
{
	struct prestera_port *port = NULL;

	read_lock(&sw->port_list_lock);
	list_for_each_entry(port, &sw->port_list, list) {
		if (port->id == id)
			break;
	}
	read_unlock(&sw->port_list_lock);

	return port;
}

static int prestera_port_open(struct net_device *dev)
{
	struct prestera_port *port = netdev_priv(dev);
	int err;

	err = prestera_hw_port_state_set(port, true);
	if (err)
		return err;

	netif_start_queue(dev);

	return 0;
}

static int prestera_port_close(struct net_device *dev)
{
	struct prestera_port *port = netdev_priv(dev);

	netif_stop_queue(dev);

	return prestera_hw_port_state_set(port, false);
}

static netdev_tx_t prestera_port_xmit(struct sk_buff *skb,
				      struct net_device *dev)
{
	return prestera_rxtx_xmit(netdev_priv(dev), skb);
}

static int prestera_is_valid_mac_addr(struct prestera_port *port, u8 *addr)
{
	if (!is_valid_ether_addr(addr))
		return -EADDRNOTAVAIL;

	/* firmware requires that port's MAC address contains first 5 bytes
	 * of the base MAC address
	 */
	if (memcmp(port->sw->base_mac, addr, ETH_ALEN - 1))
		return -EINVAL;

	return 0;
}

static int prestera_port_set_mac_address(struct net_device *dev, void *p)
{
	struct prestera_port *port = netdev_priv(dev);
	struct sockaddr *addr = p;
	int err;

	err = prestera_is_valid_mac_addr(port, addr->sa_data);
	if (err)
		return err;

	err = prestera_hw_port_mac_set(port, addr->sa_data);
	if (err)
		return err;

	ether_addr_copy(dev->dev_addr, addr->sa_data);

	return 0;
}

static int prestera_port_change_mtu(struct net_device *dev, int mtu)
{
	struct prestera_port *port = netdev_priv(dev);
	int err;

	err = prestera_hw_port_mtu_set(port, mtu);
	if (err)
		return err;

	dev->mtu = mtu;

	return 0;
}

static void prestera_port_get_stats64(struct net_device *dev,
				      struct rtnl_link_stats64 *stats)
{
	struct prestera_port *port = netdev_priv(dev);
	struct prestera_port_stats *port_stats = &port->cached_hw_stats.stats;

	stats->rx_packets = port_stats->broadcast_frames_received +
				port_stats->multicast_frames_received +
				port_stats->unicast_frames_received;

	stats->tx_packets = port_stats->broadcast_frames_sent +
				port_stats->multicast_frames_sent +
				port_stats->unicast_frames_sent;

	stats->rx_bytes = port_stats->good_octets_received;

	stats->tx_bytes = port_stats->good_octets_sent;

	stats->rx_errors = port_stats->rx_error_frame_received;
	stats->tx_errors = port_stats->mac_trans_error;

	stats->rx_dropped = port_stats->buffer_overrun;
	stats->tx_dropped = 0;

	stats->multicast = port_stats->multicast_frames_received;
	stats->collisions = port_stats->excessive_collision;

	stats->rx_crc_errors = port_stats->bad_crc;
}

static void prestera_port_get_hw_stats(struct prestera_port *port)
{
	prestera_hw_port_stats_get(port, &port->cached_hw_stats.stats);
}

static void prestera_port_stats_update(struct work_struct *work)
{
	struct prestera_port *port =
		container_of(work, struct prestera_port,
			     cached_hw_stats.caching_dw.work);

	prestera_port_get_hw_stats(port);

	queue_delayed_work(prestera_wq, &port->cached_hw_stats.caching_dw,
			   msecs_to_jiffies(PRESTERA_STATS_DELAY_MS));
}

static const struct net_device_ops prestera_netdev_ops = {
	.ndo_open = prestera_port_open,
	.ndo_stop = prestera_port_close,
	.ndo_start_xmit = prestera_port_xmit,
	.ndo_change_mtu = prestera_port_change_mtu,
	.ndo_get_stats64 = prestera_port_get_stats64,
	.ndo_set_mac_address = prestera_port_set_mac_address,
	.ndo_get_devlink_port = prestera_devlink_get_port,
};

int prestera_port_autoneg_set(struct prestera_port *port, bool enable,
			      u64 adver_link_modes, u8 adver_fec)
{
	bool refresh = false;
	u64 link_modes;
	int err;
	u8 fec;

	if (port->caps.type != PRESTERA_PORT_TYPE_TP)
		return enable ? -EINVAL : 0;

	if (!enable)
		goto set_autoneg;

	link_modes = port->caps.supp_link_modes & adver_link_modes;
	fec = port->caps.supp_fec & adver_fec;

	if (!link_modes && !fec)
		return -EOPNOTSUPP;

	if (link_modes && port->adver_link_modes != link_modes) {
		port->adver_link_modes = link_modes;
		refresh = true;
	}

	if (fec && port->adver_fec != fec) {
		port->adver_fec = fec;
		refresh = true;
	}

set_autoneg:
	if (port->autoneg == enable && !refresh)
		return 0;

	err = prestera_hw_port_autoneg_set(port, enable, port->adver_link_modes,
					   port->adver_fec);
	if (err)
		return err;

	port->autoneg = enable;

	return 0;
}

static void prestera_port_list_add(struct prestera_port *port)
{
	write_lock(&port->sw->port_list_lock);
	list_add(&port->list, &port->sw->port_list);
	write_unlock(&port->sw->port_list_lock);
}

static void prestera_port_list_del(struct prestera_port *port)
{
	write_lock(&port->sw->port_list_lock);
	list_del(&port->list);
	write_unlock(&port->sw->port_list_lock);
}

static int prestera_port_create(struct prestera_switch *sw, u32 id)
{
	struct prestera_port *port;
	struct net_device *dev;
	int err;

	dev = alloc_etherdev(sizeof(*port));
	if (!dev)
		return -ENOMEM;

	port = netdev_priv(dev);

	INIT_LIST_HEAD(&port->vlans_list);
	port->pvid = PRESTERA_DEFAULT_VID;
	port->dev = dev;
	port->id = id;
	port->sw = sw;

	err = prestera_hw_port_info_get(port, &port->dev_id, &port->hw_id,
					&port->fp_id);
	if (err) {
		dev_err(prestera_dev(sw), "Failed to get port(%u) info\n", id);
		goto err_port_info_get;
	}

	err = prestera_devlink_port_register(port);
	if (err)
		goto err_dl_port_register;

	dev->features |= NETIF_F_NETNS_LOCAL;
	dev->netdev_ops = &prestera_netdev_ops;
	dev->ethtool_ops = &prestera_ethtool_ops;

	netif_carrier_off(dev);

	dev->mtu = min_t(unsigned int, sw->mtu_max, PRESTERA_MTU_DEFAULT);
	dev->min_mtu = sw->mtu_min;
	dev->max_mtu = sw->mtu_max;

	err = prestera_hw_port_mtu_set(port, dev->mtu);
	if (err) {
		dev_err(prestera_dev(sw), "Failed to set port(%u) mtu(%d)\n",
			id, dev->mtu);
		goto err_port_init;
	}

	if (port->fp_id >= PRESTERA_MAC_ADDR_NUM_MAX) {
		err = -EINVAL;
		goto err_port_init;
	}

	/* firmware requires that port's MAC address consist of the first
	 * 5 bytes of the base MAC address
	 */
	memcpy(dev->dev_addr, sw->base_mac, dev->addr_len - 1);
	dev->dev_addr[dev->addr_len - 1] = port->fp_id;

	err = prestera_hw_port_mac_set(port, dev->dev_addr);
	if (err) {
		dev_err(prestera_dev(sw), "Failed to set port(%u) mac addr\n", id);
		goto err_port_init;
	}

	err = prestera_hw_port_cap_get(port, &port->caps);
	if (err) {
		dev_err(prestera_dev(sw), "Failed to get port(%u) caps\n", id);
		goto err_port_init;
	}

	port->adver_fec = BIT(PRESTERA_PORT_FEC_OFF);
	prestera_port_autoneg_set(port, true, port->caps.supp_link_modes,
				  port->caps.supp_fec);

	err = prestera_hw_port_state_set(port, false);
	if (err) {
		dev_err(prestera_dev(sw), "Failed to set port(%u) down\n", id);
		goto err_port_init;
	}

	err = prestera_rxtx_port_init(port);
	if (err)
		goto err_port_init;

	INIT_DELAYED_WORK(&port->cached_hw_stats.caching_dw,
			  &prestera_port_stats_update);

	prestera_port_list_add(port);

	err = register_netdev(dev);
	if (err)
		goto err_register_netdev;

	prestera_devlink_port_set(port);

	return 0;

err_register_netdev:
	prestera_port_list_del(port);
err_port_init:
	prestera_devlink_port_unregister(port);
err_dl_port_register:
err_port_info_get:
	free_netdev(dev);
	return err;
}

static void prestera_port_destroy(struct prestera_port *port)
{
	struct net_device *dev = port->dev;

	cancel_delayed_work_sync(&port->cached_hw_stats.caching_dw);
	prestera_devlink_port_clear(port);
	unregister_netdev(dev);
	prestera_port_list_del(port);
	prestera_devlink_port_unregister(port);
	free_netdev(dev);
}

static void prestera_destroy_ports(struct prestera_switch *sw)
{
	struct prestera_port *port, *tmp;

	list_for_each_entry_safe(port, tmp, &sw->port_list, list)
		prestera_port_destroy(port);
}

static int prestera_create_ports(struct prestera_switch *sw)
{
	struct prestera_port *port, *tmp;
	u32 port_idx;
	int err;

	for (port_idx = 0; port_idx < sw->port_count; port_idx++) {
		err = prestera_port_create(sw, port_idx);
		if (err)
			goto err_port_create;
	}

	return 0;

err_port_create:
	list_for_each_entry_safe(port, tmp, &sw->port_list, list)
		prestera_port_destroy(port);

	return err;
}

static void prestera_port_handle_event(struct prestera_switch *sw,
				       struct prestera_event *evt, void *arg)
{
	struct delayed_work *caching_dw;
	struct prestera_port *port;

	port = prestera_find_port(sw, evt->port_evt.port_id);
	if (!port || !port->dev)
		return;

	caching_dw = &port->cached_hw_stats.caching_dw;

	if (evt->id == PRESTERA_PORT_EVENT_STATE_CHANGED) {
		if (evt->port_evt.data.oper_state) {
			netif_carrier_on(port->dev);
			if (!delayed_work_pending(caching_dw))
				queue_delayed_work(prestera_wq, caching_dw, 0);
		} else if (netif_running(port->dev) &&
			   netif_carrier_ok(port->dev)) {
			netif_carrier_off(port->dev);
			if (delayed_work_pending(caching_dw))
				cancel_delayed_work(caching_dw);
		}
	}
}

static int prestera_event_handlers_register(struct prestera_switch *sw)
{
	return prestera_hw_event_handler_register(sw, PRESTERA_EVENT_TYPE_PORT,
						  prestera_port_handle_event,
						  NULL);
}

static void prestera_event_handlers_unregister(struct prestera_switch *sw)
{
	prestera_hw_event_handler_unregister(sw, PRESTERA_EVENT_TYPE_PORT,
					     prestera_port_handle_event);
}

static int prestera_switch_set_base_mac_addr(struct prestera_switch *sw)
{
	struct device_node *base_mac_np;
	struct device_node *np;
	int ret;

	np = of_find_compatible_node(NULL, NULL, "marvell,prestera");
	base_mac_np = of_parse_phandle(np, "base-mac-provider", 0);

	ret = of_get_mac_address(base_mac_np, sw->base_mac);
	if (ret) {
		eth_random_addr(sw->base_mac);
		dev_info(prestera_dev(sw), "using random base mac address\n");
	}
	of_node_put(base_mac_np);

	return prestera_hw_switch_mac_set(sw, sw->base_mac);
}

bool prestera_netdev_check(const struct net_device *dev)
{
	return dev->netdev_ops == &prestera_netdev_ops;
}

static int prestera_lower_dev_walk(struct net_device *dev,
				   struct netdev_nested_priv *priv)
{
	struct prestera_port **pport = (struct prestera_port **)priv->data;

	if (prestera_netdev_check(dev)) {
		*pport = netdev_priv(dev);
		return 1;
	}

	return 0;
}

struct prestera_port *prestera_port_dev_lower_find(struct net_device *dev)
{
	struct prestera_port *port = NULL;
	struct netdev_nested_priv priv = {
		.data = (void *)&port,
	};

	if (prestera_netdev_check(dev))
		return netdev_priv(dev);

	netdev_walk_all_lower_dev(dev, prestera_lower_dev_walk, &priv);

	return port;
}

static int prestera_netdev_port_event(struct net_device *dev,
				      unsigned long event, void *ptr)
{
	switch (event) {
	case NETDEV_PRECHANGEUPPER:
	case NETDEV_CHANGEUPPER:
		return prestera_bridge_port_event(dev, event, ptr);
	default:
		return 0;
	}
}

static int prestera_netdev_event_handler(struct notifier_block *nb,
					 unsigned long event, void *ptr)
{
	struct net_device *dev = netdev_notifier_info_to_dev(ptr);
	int err = 0;

	if (prestera_netdev_check(dev))
		err = prestera_netdev_port_event(dev, event, ptr);

	return notifier_from_errno(err);
}

static int prestera_netdev_event_handler_register(struct prestera_switch *sw)
{
	sw->netdev_nb.notifier_call = prestera_netdev_event_handler;

	return register_netdevice_notifier(&sw->netdev_nb);
}

static void prestera_netdev_event_handler_unregister(struct prestera_switch *sw)
{
	unregister_netdevice_notifier(&sw->netdev_nb);
}

static int prestera_switch_init(struct prestera_switch *sw)
{
	int err;

	err = prestera_hw_switch_init(sw);
	if (err) {
		dev_err(prestera_dev(sw), "Failed to init Switch device\n");
		return err;
	}

	rwlock_init(&sw->port_list_lock);
	INIT_LIST_HEAD(&sw->port_list);

	err = prestera_switch_set_base_mac_addr(sw);
	if (err)
		return err;

	err = prestera_netdev_event_handler_register(sw);
	if (err)
		return err;

	err = prestera_switchdev_init(sw);
	if (err)
		goto err_swdev_register;

	err = prestera_rxtx_switch_init(sw);
	if (err)
		goto err_rxtx_register;

	err = prestera_event_handlers_register(sw);
	if (err)
		goto err_handlers_register;

	err = prestera_devlink_register(sw);
	if (err)
		goto err_dl_register;

	err = prestera_create_ports(sw);
	if (err)
		goto err_ports_create;

	return 0;

err_ports_create:
	prestera_devlink_unregister(sw);
err_dl_register:
	prestera_event_handlers_unregister(sw);
err_handlers_register:
	prestera_rxtx_switch_fini(sw);
err_rxtx_register:
	prestera_switchdev_fini(sw);
err_swdev_register:
	prestera_netdev_event_handler_unregister(sw);
	prestera_hw_switch_fini(sw);

	return err;
}

static void prestera_switch_fini(struct prestera_switch *sw)
{
	prestera_destroy_ports(sw);
	prestera_devlink_unregister(sw);
	prestera_event_handlers_unregister(sw);
	prestera_rxtx_switch_fini(sw);
	prestera_switchdev_fini(sw);
	prestera_netdev_event_handler_unregister(sw);
	prestera_hw_switch_fini(sw);
}

int prestera_device_register(struct prestera_device *dev)
{
	struct prestera_switch *sw;
	int err;

	sw = prestera_devlink_alloc();
	if (!sw)
		return -ENOMEM;

	dev->priv = sw;
	sw->dev = dev;

	err = prestera_switch_init(sw);
	if (err) {
		prestera_devlink_free(sw);
		return err;
	}

	return 0;
}
EXPORT_SYMBOL(prestera_device_register);

void prestera_device_unregister(struct prestera_device *dev)
{
	struct prestera_switch *sw = dev->priv;

	prestera_switch_fini(sw);
	prestera_devlink_free(sw);
}
EXPORT_SYMBOL(prestera_device_unregister);

static int __init prestera_module_init(void)
{
	prestera_wq = alloc_workqueue("prestera", 0, 0);
	if (!prestera_wq)
		return -ENOMEM;

	return 0;
}

static void __exit prestera_module_exit(void)
{
	destroy_workqueue(prestera_wq);
}

module_init(prestera_module_init);
module_exit(prestera_module_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("Marvell Prestera switch driver");
