// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *	Spanning tree protocol; interface code
 *	Linux ethernet bridge
 *
 *	Authors:
 *	Lennert Buytenhek		<buytenh@gnu.org>
 */

#include <linux/kernel.h>
#include <linux/kmod.h>
#include <linux/etherdevice.h>
#include <linux/rtnetlink.h>
#include <net/switchdev.h>

#include "br_private.h"
#include "br_private_stp.h"


/* Port id is composed of priority and port number.
 * NB: some bits of priority are dropped to
 *     make room for more ports.
 */
static inline port_id br_make_port_id(__u8 priority, __u16 port_no)
{
	return ((u16)priority << BR_PORT_BITS)
		| (port_no & ((1<<BR_PORT_BITS)-1));
}

#define BR_MAX_PORT_PRIORITY ((u16)~0 >> BR_PORT_BITS)

/* called under bridge lock */
void br_init_port(struct net_bridge_port *p)
{
	int err;

	p->port_id = br_make_port_id(p->priority, p->port_no);
	br_become_designated_port(p);
	br_set_state(p, BR_STATE_BLOCKING);
	p->topology_change_ack = 0;
	p->config_pending = 0;

	err = __set_ageing_time(p->dev, p->br->ageing_time);
	if (err)
		netdev_err(p->dev, "failed to offload ageing time\n");
}

/* NO locks held */
void br_stp_enable_bridge(struct net_bridge *br)
{
	struct net_bridge_port *p;

	spin_lock_bh(&br->lock);
	if (br->stp_enabled == BR_KERNEL_STP)
		mod_timer(&br->hello_timer, jiffies + br->hello_time);
	mod_delayed_work(system_long_wq, &br->gc_work, HZ / 10);

	br_config_bpdu_generation(br);

	list_for_each_entry(p, &br->port_list, list) {
		if (netif_running(p->dev) && netif_oper_up(p->dev))
			br_stp_enable_port(p);

	}
	spin_unlock_bh(&br->lock);
}

/* NO locks held */
void br_stp_disable_bridge(struct net_bridge *br)
{
	struct net_bridge_port *p;

	spin_lock_bh(&br->lock);
	list_for_each_entry(p, &br->port_list, list) {
		if (p->state != BR_STATE_DISABLED)
			br_stp_disable_port(p);

	}

	__br_set_topology_change(br, 0);
	br->topology_change_detected = 0;
	spin_unlock_bh(&br->lock);

	del_timer_sync(&br->hello_timer);
	del_timer_sync(&br->topology_change_timer);
	del_timer_sync(&br->tcn_timer);
	cancel_delayed_work_sync(&br->gc_work);
}

/* called under bridge lock */
void br_stp_enable_port(struct net_bridge_port *p)
{
	br_init_port(p);
	br_port_state_selection(p->br);
	br_ifinfo_notify(RTM_NEWLINK, NULL, p);
}

/* called under bridge lock */
void br_stp_disable_port(struct net_bridge_port *p)
{
	struct net_bridge *br = p->br;
	int wasroot;

	wasroot = br_is_root_bridge(br);
	br_become_designated_port(p);
	br_set_state(p, BR_STATE_DISABLED);
	p->topology_change_ack = 0;
	p->config_pending = 0;

	br_ifinfo_notify(RTM_NEWLINK, NULL, p);

	del_timer(&p->message_age_timer);
	del_timer(&p->forward_delay_timer);
	del_timer(&p->hold_timer);

	if (!rcu_access_pointer(p->backup_port))
		br_fdb_delete_by_port(br, p, 0, 0);
	br_multicast_disable_port(p);

	br_configuration_update(br);

	br_port_state_selection(br);

	if (br_is_root_bridge(br) && !wasroot)
		br_become_root_bridge(br);
}

static int br_stp_call_user(struct net_bridge *br, char *arg)
{
	char *argv[] = { BR_STP_PROG, br->dev->name, arg, NULL };
	char *envp[] = { NULL };
	int rc;

	/* call userspace STP and report program errors */
	rc = call_usermodehelper(BR_STP_PROG, argv, envp, UMH_WAIT_PROC);
	if (rc > 0) {
		if (rc & 0xff)
			br_debug(br, BR_STP_PROG " received signal %d\n",
				 rc & 0x7f);
		else
			br_debug(br, BR_STP_PROG " exited with code %d\n",
				 (rc >> 8) & 0xff);
	}

	return rc;
}

static void br_stp_start(struct net_bridge *br)
{
	int err = -ENOENT;

	if (net_eq(dev_net(br->dev), &init_net))
		err = br_stp_call_user(br, "start");

	if (err && err != -ENOENT)
		br_err(br, "failed to start userspace STP (%d)\n", err);

	spin_lock_bh(&br->lock);

	if (br->bridge_forward_delay < BR_MIN_FORWARD_DELAY)
		__br_set_forward_delay(br, BR_MIN_FORWARD_DELAY);
	else if (br->bridge_forward_delay > BR_MAX_FORWARD_DELAY)
		__br_set_forward_delay(br, BR_MAX_FORWARD_DELAY);

	if (!err) {
		br->stp_enabled = BR_USER_STP;
		br_debug(br, "userspace STP started\n");
	} else {
		br->stp_enabled = BR_KERNEL_STP;
		br_debug(br, "using kernel STP\n");

		/* To start timers on any ports left in blocking */
		if (br->dev->flags & IFF_UP)
			mod_timer(&br->hello_timer, jiffies + br->hello_time);
		br_port_state_selection(br);
	}

	spin_unlock_bh(&br->lock);
}

static void br_stp_stop(struct net_bridge *br)
{
	int err;

	if (br->stp_enabled == BR_USER_STP) {
		err = br_stp_call_user(br, "stop");
		if (err)
			br_err(br, "failed to stop userspace STP (%d)\n", err);

		/* To start timers on any ports left in blocking */
		spin_lock_bh(&br->lock);
		br_port_state_selection(br);
		spin_unlock_bh(&br->lock);
	}

	br->stp_enabled = BR_NO_STP;
}

int br_stp_set_enabled(struct net_bridge *br, unsigned long val,
		       struct netlink_ext_ack *extack)
{
	ASSERT_RTNL();

	if (br_mrp_enabled(br)) {
		NL_SET_ERR_MSG_MOD(extack,
				   "STP can't be enabled if MRP is already enabled");
		return -EINVAL;
	}

	if (val) {
		if (br->stp_enabled == BR_NO_STP)
			br_stp_start(br);
	} else {
		if (br->stp_enabled != BR_NO_STP)
			br_stp_stop(br);
	}

	return 0;
}

/* called under bridge lock */
void br_stp_change_bridge_id(struct net_bridge *br, const unsigned char *addr)
{
	/* should be aligned on 2 bytes for ether_addr_equal() */
	unsigned short oldaddr_aligned[ETH_ALEN >> 1];
	unsigned char *oldaddr = (unsigned char *)oldaddr_aligned;
	struct net_bridge_port *p;
	int wasroot;

	wasroot = br_is_root_bridge(br);

	br_fdb_change_mac_address(br, addr);

	memcpy(oldaddr, br->bridge_id.addr, ETH_ALEN);
	memcpy(br->bridge_id.addr, addr, ETH_ALEN);
	memcpy(br->dev->dev_addr, addr, ETH_ALEN);

	list_for_each_entry(p, &br->port_list, list) {
		if (ether_addr_equal(p->designated_bridge.addr, oldaddr))
			memcpy(p->designated_bridge.addr, addr, ETH_ALEN);

		if (ether_addr_equal(p->designated_root.addr, oldaddr))
			memcpy(p->designated_root.addr, addr, ETH_ALEN);
	}

	br_configuration_update(br);
	br_port_state_selection(br);
	if (br_is_root_bridge(br) && !wasroot)
		br_become_root_bridge(br);
}

/* should be aligned on 2 bytes for ether_addr_equal() */
static const unsigned short br_mac_zero_aligned[ETH_ALEN >> 1];

/* called under bridge lock */
bool br_stp_recalculate_bridge_id(struct net_bridge *br)
{
	const unsigned char *br_mac_zero =
			(const unsigned char *)br_mac_zero_aligned;
	const unsigned char *addr = br_mac_zero;
	struct net_bridge_port *p;

	/* user has chosen a value so keep it */
	if (br->dev->addr_assign_type == NET_ADDR_SET)
		return false;

	list_for_each_entry(p, &br->port_list, list) {
		if (addr == br_mac_zero ||
		    memcmp(p->dev->dev_addr, addr, ETH_ALEN) < 0)
			addr = p->dev->dev_addr;

	}

	if (ether_addr_equal(br->bridge_id.addr, addr))
		return false;	/* no change */

	br_stp_change_bridge_id(br, addr);
	return true;
}

/* Acquires and releases bridge lock */
void br_stp_set_bridge_priority(struct net_bridge *br, u16 newprio)
{
	struct net_bridge_port *p;
	int wasroot;

	spin_lock_bh(&br->lock);
	wasroot = br_is_root_bridge(br);

	list_for_each_entry(p, &br->port_list, list) {
		if (p->state != BR_STATE_DISABLED &&
		    br_is_designated_port(p)) {
			p->designated_bridge.prio[0] = (newprio >> 8) & 0xFF;
			p->designated_bridge.prio[1] = newprio & 0xFF;
		}

	}

	br->bridge_id.prio[0] = (newprio >> 8) & 0xFF;
	br->bridge_id.prio[1] = newprio & 0xFF;
	br_configuration_update(br);
	br_port_state_selection(br);
	if (br_is_root_bridge(br) && !wasroot)
		br_become_root_bridge(br);
	spin_unlock_bh(&br->lock);
}

/* called under bridge lock */
int br_stp_set_port_priority(struct net_bridge_port *p, unsigned long newprio)
{
	port_id new_port_id;

	if (newprio > BR_MAX_PORT_PRIORITY)
		return -ERANGE;

	new_port_id = br_make_port_id(newprio, p->port_no);
	if (br_is_designated_port(p))
		p->designated_port = new_port_id;

	p->port_id = new_port_id;
	p->priority = newprio;
	if (!memcmp(&p->br->bridge_id, &p->designated_bridge, 8) &&
	    p->port_id < p->designated_port) {
		br_become_designated_port(p);
		br_port_state_selection(p->br);
	}

	return 0;
}

/* called under bridge lock */
int br_stp_set_path_cost(struct net_bridge_port *p, unsigned long path_cost)
{
	if (path_cost < BR_MIN_PATH_COST ||
	    path_cost > BR_MAX_PATH_COST)
		return -ERANGE;

	p->flags |= BR_ADMIN_COST;
	p->path_cost = path_cost;
	br_configuration_update(p->br);
	br_port_state_selection(p->br);
	return 0;
}

ssize_t br_show_bridge_id(char *buf, const struct bridge_id *id)
{
	return sprintf(buf, "%.2x%.2x.%.2x%.2x%.2x%.2x%.2x%.2x\n",
	       id->prio[0], id->prio[1],
	       id->addr[0], id->addr[1], id->addr[2],
	       id->addr[3], id->addr[4], id->addr[5]);
}
