// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright(c) 1999 - 2004 Intel Corporation. All rights reserved.
 */

#include <linux/skbuff.h>
#include <linux/if_ether.h>
#include <linux/netdevice.h>
#include <linux/spinlock.h>
#include <linux/ethtool.h>
#include <linux/etherdevice.h>
#include <linux/if_bonding.h>
#include <linux/pkt_sched.h>
#include <net/net_namespace.h>
#include <net/bonding.h>
#include <net/bond_3ad.h>
#include <net/netlink.h>

/* General definitions */
#define AD_SHORT_TIMEOUT           1
#define AD_LONG_TIMEOUT            0
#define AD_STANDBY                 0x2
#define AD_MAX_TX_IN_SECOND        3
#define AD_COLLECTOR_MAX_DELAY     0

/* Timer definitions (43.4.4 in the 802.3ad standard) */
#define AD_FAST_PERIODIC_TIME      1
#define AD_SLOW_PERIODIC_TIME      30
#define AD_SHORT_TIMEOUT_TIME      (3*AD_FAST_PERIODIC_TIME)
#define AD_LONG_TIMEOUT_TIME       (3*AD_SLOW_PERIODIC_TIME)
#define AD_CHURN_DETECTION_TIME    60
#define AD_AGGREGATE_WAIT_TIME     2

/* Port Variables definitions used by the State Machines (43.4.7 in the
 * 802.3ad standard)
 */
#define AD_PORT_BEGIN           0x1
#define AD_PORT_LACP_ENABLED    0x2
#define AD_PORT_ACTOR_CHURN     0x4
#define AD_PORT_PARTNER_CHURN   0x8
#define AD_PORT_READY           0x10
#define AD_PORT_READY_N         0x20
#define AD_PORT_MATCHED         0x40
#define AD_PORT_STANDBY         0x80
#define AD_PORT_SELECTED        0x100
#define AD_PORT_MOVED           0x200
#define AD_PORT_CHURNED         (AD_PORT_ACTOR_CHURN | AD_PORT_PARTNER_CHURN)

/* Port Key definitions
 * key is determined according to the link speed, duplex and
 * user key (which is yet not supported)
 *           --------------------------------------------------------------
 * Port key  | User key (10 bits)           | Speed (5 bits)      | Duplex|
 *           --------------------------------------------------------------
 *           |15                           6|5                   1|0
 */
#define  AD_DUPLEX_KEY_MASKS    0x1
#define  AD_SPEED_KEY_MASKS     0x3E
#define  AD_USER_KEY_MASKS      0xFFC0

enum ad_link_speed_type {
	AD_LINK_SPEED_1MBPS = 1,
	AD_LINK_SPEED_10MBPS,
	AD_LINK_SPEED_100MBPS,
	AD_LINK_SPEED_1000MBPS,
	AD_LINK_SPEED_2500MBPS,
	AD_LINK_SPEED_5000MBPS,
	AD_LINK_SPEED_10000MBPS,
	AD_LINK_SPEED_14000MBPS,
	AD_LINK_SPEED_20000MBPS,
	AD_LINK_SPEED_25000MBPS,
	AD_LINK_SPEED_40000MBPS,
	AD_LINK_SPEED_50000MBPS,
	AD_LINK_SPEED_56000MBPS,
	AD_LINK_SPEED_100000MBPS,
	AD_LINK_SPEED_200000MBPS,
	AD_LINK_SPEED_400000MBPS,
};

/* compare MAC addresses */
#define MAC_ADDRESS_EQUAL(A, B)	\
	ether_addr_equal_64bits((const u8 *)A, (const u8 *)B)

static const u8 null_mac_addr[ETH_ALEN + 2] __long_aligned = {
	0, 0, 0, 0, 0, 0
};
static u16 ad_ticks_per_sec;
static const int ad_delta_in_ticks = (AD_TIMER_INTERVAL * HZ) / 1000;

static const u8 lacpdu_mcast_addr[ETH_ALEN + 2] __long_aligned =
	MULTICAST_LACPDU_ADDR;

/* ================= main 802.3ad protocol functions ================== */
static int ad_lacpdu_send(struct port *port);
static int ad_marker_send(struct port *port, struct bond_marker *marker);
static void ad_mux_machine(struct port *port, bool *update_slave_arr);
static void ad_rx_machine(struct lacpdu *lacpdu, struct port *port);
static void ad_tx_machine(struct port *port);
static void ad_periodic_machine(struct port *port);
static void ad_port_selection_logic(struct port *port, bool *update_slave_arr);
static void ad_agg_selection_logic(struct aggregator *aggregator,
				   bool *update_slave_arr);
static void ad_clear_agg(struct aggregator *aggregator);
static void ad_initialize_agg(struct aggregator *aggregator);
static void ad_initialize_port(struct port *port, int lacp_fast);
static void ad_enable_collecting_distributing(struct port *port,
					      bool *update_slave_arr);
static void ad_disable_collecting_distributing(struct port *port,
					       bool *update_slave_arr);
static void ad_marker_info_received(struct bond_marker *marker_info,
				    struct port *port);
static void ad_marker_response_received(struct bond_marker *marker,
					struct port *port);
static void ad_update_actor_keys(struct port *port, bool reset);


/* ================= api to bonding and kernel code ================== */

/**
 * __get_bond_by_port - get the port's bonding struct
 * @port: the port we're looking at
 *
 * Return @port's bonding struct, or %NULL if it can't be found.
 */
static inline struct bonding *__get_bond_by_port(struct port *port)
{
	if (port->slave == NULL)
		return NULL;

	return bond_get_bond_by_slave(port->slave);
}

/**
 * __get_first_agg - get the first aggregator in the bond
 * @port: the port we're looking at
 *
 * Return the aggregator of the first slave in @bond, or %NULL if it can't be
 * found.
 * The caller must hold RCU or RTNL lock.
 */
static inline struct aggregator *__get_first_agg(struct port *port)
{
	struct bonding *bond = __get_bond_by_port(port);
	struct slave *first_slave;
	struct aggregator *agg;

	/* If there's no bond for this port, or bond has no slaves */
	if (bond == NULL)
		return NULL;

	rcu_read_lock();
	first_slave = bond_first_slave_rcu(bond);
	agg = first_slave ? &(SLAVE_AD_INFO(first_slave)->aggregator) : NULL;
	rcu_read_unlock();

	return agg;
}

/**
 * __agg_has_partner - see if we have a partner
 * @agg: the agregator we're looking at
 *
 * Return nonzero if aggregator has a partner (denoted by a non-zero ether
 * address for the partner). Return 0 if not.
 */
static inline int __agg_has_partner(struct aggregator *agg)
{
	return !is_zero_ether_addr(agg->partner_system.mac_addr_value);
}

/**
 * __disable_port - disable the port's slave
 * @port: the port we're looking at
 */
static inline void __disable_port(struct port *port)
{
	bond_set_slave_inactive_flags(port->slave, BOND_SLAVE_NOTIFY_LATER);
}

/**
 * __enable_port - enable the port's slave, if it's up
 * @port: the port we're looking at
 */
static inline void __enable_port(struct port *port)
{
	struct slave *slave = port->slave;

	if ((slave->link == BOND_LINK_UP) && bond_slave_is_up(slave))
		bond_set_slave_active_flags(slave, BOND_SLAVE_NOTIFY_LATER);
}

/**
 * __port_is_enabled - check if the port's slave is in active state
 * @port: the port we're looking at
 */
static inline int __port_is_enabled(struct port *port)
{
	return bond_is_active_slave(port->slave);
}

/**
 * __get_agg_selection_mode - get the aggregator selection mode
 * @port: the port we're looking at
 *
 * Get the aggregator selection mode. Can be %STABLE, %BANDWIDTH or %COUNT.
 */
static inline u32 __get_agg_selection_mode(struct port *port)
{
	struct bonding *bond = __get_bond_by_port(port);

	if (bond == NULL)
		return BOND_AD_STABLE;

	return bond->params.ad_select;
}

/**
 * __check_agg_selection_timer - check if the selection timer has expired
 * @port: the port we're looking at
 */
static inline int __check_agg_selection_timer(struct port *port)
{
	struct bonding *bond = __get_bond_by_port(port);

	if (bond == NULL)
		return 0;

	return BOND_AD_INFO(bond).agg_select_timer ? 1 : 0;
}

/**
 * __get_link_speed - get a port's speed
 * @port: the port we're looking at
 *
 * Return @port's speed in 802.3ad enum format. i.e. one of:
 *     0,
 *     %AD_LINK_SPEED_10MBPS,
 *     %AD_LINK_SPEED_100MBPS,
 *     %AD_LINK_SPEED_1000MBPS,
 *     %AD_LINK_SPEED_2500MBPS,
 *     %AD_LINK_SPEED_5000MBPS,
 *     %AD_LINK_SPEED_10000MBPS
 *     %AD_LINK_SPEED_14000MBPS,
 *     %AD_LINK_SPEED_20000MBPS
 *     %AD_LINK_SPEED_25000MBPS
 *     %AD_LINK_SPEED_40000MBPS
 *     %AD_LINK_SPEED_50000MBPS
 *     %AD_LINK_SPEED_56000MBPS
 *     %AD_LINK_SPEED_100000MBPS
 *     %AD_LINK_SPEED_200000MBPS
 *     %AD_LINK_SPEED_400000MBPS
 */
static u16 __get_link_speed(struct port *port)
{
	struct slave *slave = port->slave;
	u16 speed;

	/* this if covers only a special case: when the configuration starts
	 * with link down, it sets the speed to 0.
	 * This is done in spite of the fact that the e100 driver reports 0
	 * to be compatible with MVT in the future.
	 */
	if (slave->link != BOND_LINK_UP)
		speed = 0;
	else {
		switch (slave->speed) {
		case SPEED_10:
			speed = AD_LINK_SPEED_10MBPS;
			break;

		case SPEED_100:
			speed = AD_LINK_SPEED_100MBPS;
			break;

		case SPEED_1000:
			speed = AD_LINK_SPEED_1000MBPS;
			break;

		case SPEED_2500:
			speed = AD_LINK_SPEED_2500MBPS;
			break;

		case SPEED_5000:
			speed = AD_LINK_SPEED_5000MBPS;
			break;

		case SPEED_10000:
			speed = AD_LINK_SPEED_10000MBPS;
			break;

		case SPEED_14000:
			speed = AD_LINK_SPEED_14000MBPS;
			break;

		case SPEED_20000:
			speed = AD_LINK_SPEED_20000MBPS;
			break;

		case SPEED_25000:
			speed = AD_LINK_SPEED_25000MBPS;
			break;

		case SPEED_40000:
			speed = AD_LINK_SPEED_40000MBPS;
			break;

		case SPEED_50000:
			speed = AD_LINK_SPEED_50000MBPS;
			break;

		case SPEED_56000:
			speed = AD_LINK_SPEED_56000MBPS;
			break;

		case SPEED_100000:
			speed = AD_LINK_SPEED_100000MBPS;
			break;

		case SPEED_200000:
			speed = AD_LINK_SPEED_200000MBPS;
			break;

		case SPEED_400000:
			speed = AD_LINK_SPEED_400000MBPS;
			break;

		default:
			/* unknown speed value from ethtool. shouldn't happen */
			if (slave->speed != SPEED_UNKNOWN)
				pr_err_once("%s: (slave %s): unknown ethtool speed (%d) for port %d (set it to 0)\n",
					    slave->bond->dev->name,
					    slave->dev->name, slave->speed,
					    port->actor_port_number);
			speed = 0;
			break;
		}
	}

	slave_dbg(slave->bond->dev, slave->dev, "Port %d Received link speed %d update from adapter\n",
		  port->actor_port_number, speed);
	return speed;
}

/**
 * __get_duplex - get a port's duplex
 * @port: the port we're looking at
 *
 * Return @port's duplex in 802.3ad bitmask format. i.e.:
 *     0x01 if in full duplex
 *     0x00 otherwise
 */
static u8 __get_duplex(struct port *port)
{
	struct slave *slave = port->slave;
	u8 retval = 0x0;

	/* handling a special case: when the configuration starts with
	 * link down, it sets the duplex to 0.
	 */
	if (slave->link == BOND_LINK_UP) {
		switch (slave->duplex) {
		case DUPLEX_FULL:
			retval = 0x1;
			slave_dbg(slave->bond->dev, slave->dev, "Port %d Received status full duplex update from adapter\n",
				  port->actor_port_number);
			break;
		case DUPLEX_HALF:
		default:
			retval = 0x0;
			slave_dbg(slave->bond->dev, slave->dev, "Port %d Received status NOT full duplex update from adapter\n",
				  port->actor_port_number);
			break;
		}
	}
	return retval;
}

static void __ad_actor_update_port(struct port *port)
{
	const struct bonding *bond = bond_get_bond_by_slave(port->slave);

	port->actor_system = BOND_AD_INFO(bond).system.sys_mac_addr;
	port->actor_system_priority = BOND_AD_INFO(bond).system.sys_priority;
}

/* Conversions */

/**
 * __ad_timer_to_ticks - convert a given timer type to AD module ticks
 * @timer_type:	which timer to operate
 * @par: timer parameter. see below
 *
 * If @timer_type is %current_while_timer, @par indicates long/short timer.
 * If @timer_type is %periodic_timer, @par is one of %FAST_PERIODIC_TIME,
 *						     %SLOW_PERIODIC_TIME.
 */
static u16 __ad_timer_to_ticks(u16 timer_type, u16 par)
{
	u16 retval = 0; /* to silence the compiler */

	switch (timer_type) {
	case AD_CURRENT_WHILE_TIMER:	/* for rx machine usage */
		if (par)
			retval = (AD_SHORT_TIMEOUT_TIME*ad_ticks_per_sec);
		else
			retval = (AD_LONG_TIMEOUT_TIME*ad_ticks_per_sec);
		break;
	case AD_ACTOR_CHURN_TIMER:	/* for local churn machine */
		retval = (AD_CHURN_DETECTION_TIME*ad_ticks_per_sec);
		break;
	case AD_PERIODIC_TIMER:		/* for periodic machine */
		retval = (par*ad_ticks_per_sec); /* long timeout */
		break;
	case AD_PARTNER_CHURN_TIMER:	/* for remote churn machine */
		retval = (AD_CHURN_DETECTION_TIME*ad_ticks_per_sec);
		break;
	case AD_WAIT_WHILE_TIMER:	/* for selection machine */
		retval = (AD_AGGREGATE_WAIT_TIME*ad_ticks_per_sec);
		break;
	}

	return retval;
}


/* ================= ad_rx_machine helper functions ================== */

/**
 * __choose_matched - update a port's matched variable from a received lacpdu
 * @lacpdu: the lacpdu we've received
 * @port: the port we're looking at
 *
 * Update the value of the matched variable, using parameter values from a
 * newly received lacpdu. Parameter values for the partner carried in the
 * received PDU are compared with the corresponding operational parameter
 * values for the actor. Matched is set to TRUE if all of these parameters
 * match and the PDU parameter partner_state.aggregation has the same value as
 * actor_oper_port_state.aggregation and lacp will actively maintain the link
 * in the aggregation. Matched is also set to TRUE if the value of
 * actor_state.aggregation in the received PDU is set to FALSE, i.e., indicates
 * an individual link and lacp will actively maintain the link. Otherwise,
 * matched is set to FALSE. LACP is considered to be actively maintaining the
 * link if either the PDU's actor_state.lacp_activity variable is TRUE or both
 * the actor's actor_oper_port_state.lacp_activity and the PDU's
 * partner_state.lacp_activity variables are TRUE.
 *
 * Note: the AD_PORT_MATCHED "variable" is not specified by 802.3ad; it is
 * used here to implement the language from 802.3ad 43.4.9 that requires
 * recordPDU to "match" the LACPDU parameters to the stored values.
 */
static void __choose_matched(struct lacpdu *lacpdu, struct port *port)
{
	/* check if all parameters are alike
	 * or this is individual link(aggregation == FALSE)
	 * then update the state machine Matched variable.
	 */
	if (((ntohs(lacpdu->partner_port) == port->actor_port_number) &&
	     (ntohs(lacpdu->partner_port_priority) == port->actor_port_priority) &&
	     MAC_ADDRESS_EQUAL(&(lacpdu->partner_system), &(port->actor_system)) &&
	     (ntohs(lacpdu->partner_system_priority) == port->actor_system_priority) &&
	     (ntohs(lacpdu->partner_key) == port->actor_oper_port_key) &&
	     ((lacpdu->partner_state & LACP_STATE_AGGREGATION) == (port->actor_oper_port_state & LACP_STATE_AGGREGATION))) ||
	    ((lacpdu->actor_state & LACP_STATE_AGGREGATION) == 0)
		) {
		port->sm_vars |= AD_PORT_MATCHED;
	} else {
		port->sm_vars &= ~AD_PORT_MATCHED;
	}
}

/**
 * __record_pdu - record parameters from a received lacpdu
 * @lacpdu: the lacpdu we've received
 * @port: the port we're looking at
 *
 * Record the parameter values for the Actor carried in a received lacpdu as
 * the current partner operational parameter values and sets
 * actor_oper_port_state.defaulted to FALSE.
 */
static void __record_pdu(struct lacpdu *lacpdu, struct port *port)
{
	if (lacpdu && port) {
		struct port_params *partner = &port->partner_oper;

		__choose_matched(lacpdu, port);
		/* record the new parameter values for the partner
		 * operational
		 */
		partner->port_number = ntohs(lacpdu->actor_port);
		partner->port_priority = ntohs(lacpdu->actor_port_priority);
		partner->system = lacpdu->actor_system;
		partner->system_priority = ntohs(lacpdu->actor_system_priority);
		partner->key = ntohs(lacpdu->actor_key);
		partner->port_state = lacpdu->actor_state;

		/* set actor_oper_port_state.defaulted to FALSE */
		port->actor_oper_port_state &= ~LACP_STATE_DEFAULTED;

		/* set the partner sync. to on if the partner is sync,
		 * and the port is matched
		 */
		if ((port->sm_vars & AD_PORT_MATCHED) &&
		    (lacpdu->actor_state & LACP_STATE_SYNCHRONIZATION)) {
			partner->port_state |= LACP_STATE_SYNCHRONIZATION;
			slave_dbg(port->slave->bond->dev, port->slave->dev,
				  "partner sync=1\n");
		} else {
			partner->port_state &= ~LACP_STATE_SYNCHRONIZATION;
			slave_dbg(port->slave->bond->dev, port->slave->dev,
				  "partner sync=0\n");
		}
	}
}

/**
 * __record_default - record default parameters
 * @port: the port we're looking at
 *
 * This function records the default parameter values for the partner carried
 * in the Partner Admin parameters as the current partner operational parameter
 * values and sets actor_oper_port_state.defaulted to TRUE.
 */
static void __record_default(struct port *port)
{
	if (port) {
		/* record the partner admin parameters */
		memcpy(&port->partner_oper, &port->partner_admin,
		       sizeof(struct port_params));

		/* set actor_oper_port_state.defaulted to true */
		port->actor_oper_port_state |= LACP_STATE_DEFAULTED;
	}
}

/**
 * __update_selected - update a port's Selected variable from a received lacpdu
 * @lacpdu: the lacpdu we've received
 * @port: the port we're looking at
 *
 * Update the value of the selected variable, using parameter values from a
 * newly received lacpdu. The parameter values for the Actor carried in the
 * received PDU are compared with the corresponding operational parameter
 * values for the ports partner. If one or more of the comparisons shows that
 * the value(s) received in the PDU differ from the current operational values,
 * then selected is set to FALSE and actor_oper_port_state.synchronization is
 * set to out_of_sync. Otherwise, selected remains unchanged.
 */
static void __update_selected(struct lacpdu *lacpdu, struct port *port)
{
	if (lacpdu && port) {
		const struct port_params *partner = &port->partner_oper;

		/* check if any parameter is different then
		 * update the state machine selected variable.
		 */
		if (ntohs(lacpdu->actor_port) != partner->port_number ||
		    ntohs(lacpdu->actor_port_priority) != partner->port_priority ||
		    !MAC_ADDRESS_EQUAL(&lacpdu->actor_system, &partner->system) ||
		    ntohs(lacpdu->actor_system_priority) != partner->system_priority ||
		    ntohs(lacpdu->actor_key) != partner->key ||
		    (lacpdu->actor_state & LACP_STATE_AGGREGATION) != (partner->port_state & LACP_STATE_AGGREGATION)) {
			port->sm_vars &= ~AD_PORT_SELECTED;
		}
	}
}

/**
 * __update_default_selected - update a port's Selected variable from Partner
 * @port: the port we're looking at
 *
 * This function updates the value of the selected variable, using the partner
 * administrative parameter values. The administrative values are compared with
 * the corresponding operational parameter values for the partner. If one or
 * more of the comparisons shows that the administrative value(s) differ from
 * the current operational values, then Selected is set to FALSE and
 * actor_oper_port_state.synchronization is set to OUT_OF_SYNC. Otherwise,
 * Selected remains unchanged.
 */
static void __update_default_selected(struct port *port)
{
	if (port) {
		const struct port_params *admin = &port->partner_admin;
		const struct port_params *oper = &port->partner_oper;

		/* check if any parameter is different then
		 * update the state machine selected variable.
		 */
		if (admin->port_number != oper->port_number ||
		    admin->port_priority != oper->port_priority ||
		    !MAC_ADDRESS_EQUAL(&admin->system, &oper->system) ||
		    admin->system_priority != oper->system_priority ||
		    admin->key != oper->key ||
		    (admin->port_state & LACP_STATE_AGGREGATION)
			!= (oper->port_state & LACP_STATE_AGGREGATION)) {
			port->sm_vars &= ~AD_PORT_SELECTED;
		}
	}
}

/**
 * __update_ntt - update a port's ntt variable from a received lacpdu
 * @lacpdu: the lacpdu we've received
 * @port: the port we're looking at
 *
 * Updates the value of the ntt variable, using parameter values from a newly
 * received lacpdu. The parameter values for the partner carried in the
 * received PDU are compared with the corresponding operational parameter
 * values for the Actor. If one or more of the comparisons shows that the
 * value(s) received in the PDU differ from the current operational values,
 * then ntt is set to TRUE. Otherwise, ntt remains unchanged.
 */
static void __update_ntt(struct lacpdu *lacpdu, struct port *port)
{
	/* validate lacpdu and port */
	if (lacpdu && port) {
		/* check if any parameter is different then
		 * update the port->ntt.
		 */
		if ((ntohs(lacpdu->partner_port) != port->actor_port_number) ||
		    (ntohs(lacpdu->partner_port_priority) != port->actor_port_priority) ||
		    !MAC_ADDRESS_EQUAL(&(lacpdu->partner_system), &(port->actor_system)) ||
		    (ntohs(lacpdu->partner_system_priority) != port->actor_system_priority) ||
		    (ntohs(lacpdu->partner_key) != port->actor_oper_port_key) ||
		    ((lacpdu->partner_state & LACP_STATE_LACP_ACTIVITY) != (port->actor_oper_port_state & LACP_STATE_LACP_ACTIVITY)) ||
		    ((lacpdu->partner_state & LACP_STATE_LACP_TIMEOUT) != (port->actor_oper_port_state & LACP_STATE_LACP_TIMEOUT)) ||
		    ((lacpdu->partner_state & LACP_STATE_SYNCHRONIZATION) != (port->actor_oper_port_state & LACP_STATE_SYNCHRONIZATION)) ||
		    ((lacpdu->partner_state & LACP_STATE_AGGREGATION) != (port->actor_oper_port_state & LACP_STATE_AGGREGATION))
		   ) {
			port->ntt = true;
		}
	}
}

/**
 * __agg_ports_are_ready - check if all ports in an aggregator are ready
 * @aggregator: the aggregator we're looking at
 *
 */
static int __agg_ports_are_ready(struct aggregator *aggregator)
{
	struct port *port;
	int retval = 1;

	if (aggregator) {
		/* scan all ports in this aggregator to verfy if they are
		 * all ready.
		 */
		for (port = aggregator->lag_ports;
		     port;
		     port = port->next_port_in_aggregator) {
			if (!(port->sm_vars & AD_PORT_READY_N)) {
				retval = 0;
				break;
			}
		}
	}

	return retval;
}

/**
 * __set_agg_ports_ready - set value of Ready bit in all ports of an aggregator
 * @aggregator: the aggregator we're looking at
 * @val: Should the ports' ready bit be set on or off
 *
 */
static void __set_agg_ports_ready(struct aggregator *aggregator, int val)
{
	struct port *port;

	for (port = aggregator->lag_ports; port;
	     port = port->next_port_in_aggregator) {
		if (val)
			port->sm_vars |= AD_PORT_READY;
		else
			port->sm_vars &= ~AD_PORT_READY;
	}
}

static int __agg_active_ports(struct aggregator *agg)
{
	struct port *port;
	int active = 0;

	for (port = agg->lag_ports; port;
	     port = port->next_port_in_aggregator) {
		if (port->is_enabled)
			active++;
	}

	return active;
}

/**
 * __get_agg_bandwidth - get the total bandwidth of an aggregator
 * @aggregator: the aggregator we're looking at
 *
 */
static u32 __get_agg_bandwidth(struct aggregator *aggregator)
{
	int nports = __agg_active_ports(aggregator);
	u32 bandwidth = 0;

	if (nports) {
		switch (__get_link_speed(aggregator->lag_ports)) {
		case AD_LINK_SPEED_1MBPS:
			bandwidth = nports;
			break;
		case AD_LINK_SPEED_10MBPS:
			bandwidth = nports * 10;
			break;
		case AD_LINK_SPEED_100MBPS:
			bandwidth = nports * 100;
			break;
		case AD_LINK_SPEED_1000MBPS:
			bandwidth = nports * 1000;
			break;
		case AD_LINK_SPEED_2500MBPS:
			bandwidth = nports * 2500;
			break;
		case AD_LINK_SPEED_5000MBPS:
			bandwidth = nports * 5000;
			break;
		case AD_LINK_SPEED_10000MBPS:
			bandwidth = nports * 10000;
			break;
		case AD_LINK_SPEED_14000MBPS:
			bandwidth = nports * 14000;
			break;
		case AD_LINK_SPEED_20000MBPS:
			bandwidth = nports * 20000;
			break;
		case AD_LINK_SPEED_25000MBPS:
			bandwidth = nports * 25000;
			break;
		case AD_LINK_SPEED_40000MBPS:
			bandwidth = nports * 40000;
			break;
		case AD_LINK_SPEED_50000MBPS:
			bandwidth = nports * 50000;
			break;
		case AD_LINK_SPEED_56000MBPS:
			bandwidth = nports * 56000;
			break;
		case AD_LINK_SPEED_100000MBPS:
			bandwidth = nports * 100000;
			break;
		case AD_LINK_SPEED_200000MBPS:
			bandwidth = nports * 200000;
			break;
		case AD_LINK_SPEED_400000MBPS:
			bandwidth = nports * 400000;
			break;
		default:
			bandwidth = 0; /* to silence the compiler */
		}
	}
	return bandwidth;
}

/**
 * __get_active_agg - get the current active aggregator
 * @aggregator: the aggregator we're looking at
 *
 * Caller must hold RCU lock.
 */
static struct aggregator *__get_active_agg(struct aggregator *aggregator)
{
	struct bonding *bond = aggregator->slave->bond;
	struct list_head *iter;
	struct slave *slave;

	bond_for_each_slave_rcu(bond, slave, iter)
		if (SLAVE_AD_INFO(slave)->aggregator.is_active)
			return &(SLAVE_AD_INFO(slave)->aggregator);

	return NULL;
}

/**
 * __update_lacpdu_from_port - update a port's lacpdu fields
 * @port: the port we're looking at
 */
static inline void __update_lacpdu_from_port(struct port *port)
{
	struct lacpdu *lacpdu = &port->lacpdu;
	const struct port_params *partner = &port->partner_oper;

	/* update current actual Actor parameters
	 * lacpdu->subtype                   initialized
	 * lacpdu->version_number            initialized
	 * lacpdu->tlv_type_actor_info       initialized
	 * lacpdu->actor_information_length  initialized
	 */

	lacpdu->actor_system_priority = htons(port->actor_system_priority);
	lacpdu->actor_system = port->actor_system;
	lacpdu->actor_key = htons(port->actor_oper_port_key);
	lacpdu->actor_port_priority = htons(port->actor_port_priority);
	lacpdu->actor_port = htons(port->actor_port_number);
	lacpdu->actor_state = port->actor_oper_port_state;
	slave_dbg(port->slave->bond->dev, port->slave->dev,
		  "update lacpdu: actor port state %x\n",
		  port->actor_oper_port_state);

	/* lacpdu->reserved_3_1              initialized
	 * lacpdu->tlv_type_partner_info     initialized
	 * lacpdu->partner_information_length initialized
	 */

	lacpdu->partner_system_priority = htons(partner->system_priority);
	lacpdu->partner_system = partner->system;
	lacpdu->partner_key = htons(partner->key);
	lacpdu->partner_port_priority = htons(partner->port_priority);
	lacpdu->partner_port = htons(partner->port_number);
	lacpdu->partner_state = partner->port_state;

	/* lacpdu->reserved_3_2              initialized
	 * lacpdu->tlv_type_collector_info   initialized
	 * lacpdu->collector_information_length initialized
	 * collector_max_delay                initialized
	 * reserved_12[12]                   initialized
	 * tlv_type_terminator               initialized
	 * terminator_length                 initialized
	 * reserved_50[50]                   initialized
	 */
}

/* ================= main 802.3ad protocol code ========================= */

/**
 * ad_lacpdu_send - send out a lacpdu packet on a given port
 * @port: the port we're looking at
 *
 * Returns:   0 on success
 *          < 0 on error
 */
static int ad_lacpdu_send(struct port *port)
{
	struct slave *slave = port->slave;
	struct sk_buff *skb;
	struct lacpdu_header *lacpdu_header;
	int length = sizeof(struct lacpdu_header);

	skb = dev_alloc_skb(length);
	if (!skb)
		return -ENOMEM;

	atomic64_inc(&SLAVE_AD_INFO(slave)->stats.lacpdu_tx);
	atomic64_inc(&BOND_AD_INFO(slave->bond).stats.lacpdu_tx);

	skb->dev = slave->dev;
	skb_reset_mac_header(skb);
	skb->network_header = skb->mac_header + ETH_HLEN;
	skb->protocol = PKT_TYPE_LACPDU;
	skb->priority = TC_PRIO_CONTROL;

	lacpdu_header = skb_put(skb, length);

	ether_addr_copy(lacpdu_header->hdr.h_dest, lacpdu_mcast_addr);
	/* Note: source address is set to be the member's PERMANENT address,
	 * because we use it to identify loopback lacpdus in receive.
	 */
	ether_addr_copy(lacpdu_header->hdr.h_source, slave->perm_hwaddr);
	lacpdu_header->hdr.h_proto = PKT_TYPE_LACPDU;

	lacpdu_header->lacpdu = port->lacpdu;

	dev_queue_xmit(skb);

	return 0;
}

/**
 * ad_marker_send - send marker information/response on a given port
 * @port: the port we're looking at
 * @marker: marker data to send
 *
 * Returns:   0 on success
 *          < 0 on error
 */
static int ad_marker_send(struct port *port, struct bond_marker *marker)
{
	struct slave *slave = port->slave;
	struct sk_buff *skb;
	struct bond_marker_header *marker_header;
	int length = sizeof(struct bond_marker_header);

	skb = dev_alloc_skb(length + 16);
	if (!skb)
		return -ENOMEM;

	switch (marker->tlv_type) {
	case AD_MARKER_INFORMATION_SUBTYPE:
		atomic64_inc(&SLAVE_AD_INFO(slave)->stats.marker_tx);
		atomic64_inc(&BOND_AD_INFO(slave->bond).stats.marker_tx);
		break;
	case AD_MARKER_RESPONSE_SUBTYPE:
		atomic64_inc(&SLAVE_AD_INFO(slave)->stats.marker_resp_tx);
		atomic64_inc(&BOND_AD_INFO(slave->bond).stats.marker_resp_tx);
		break;
	}

	skb_reserve(skb, 16);

	skb->dev = slave->dev;
	skb_reset_mac_header(skb);
	skb->network_header = skb->mac_header + ETH_HLEN;
	skb->protocol = PKT_TYPE_LACPDU;

	marker_header = skb_put(skb, length);

	ether_addr_copy(marker_header->hdr.h_dest, lacpdu_mcast_addr);
	/* Note: source address is set to be the member's PERMANENT address,
	 * because we use it to identify loopback MARKERs in receive.
	 */
	ether_addr_copy(marker_header->hdr.h_source, slave->perm_hwaddr);
	marker_header->hdr.h_proto = PKT_TYPE_LACPDU;

	marker_header->marker = *marker;

	dev_queue_xmit(skb);

	return 0;
}

/**
 * ad_mux_machine - handle a port's mux state machine
 * @port: the port we're looking at
 * @update_slave_arr: Does slave array need update?
 */
static void ad_mux_machine(struct port *port, bool *update_slave_arr)
{
	mux_states_t last_state;

	/* keep current State Machine state to compare later if it was
	 * changed
	 */
	last_state = port->sm_mux_state;

	if (port->sm_vars & AD_PORT_BEGIN) {
		port->sm_mux_state = AD_MUX_DETACHED;
	} else {
		switch (port->sm_mux_state) {
		case AD_MUX_DETACHED:
			if ((port->sm_vars & AD_PORT_SELECTED)
			    || (port->sm_vars & AD_PORT_STANDBY))
				/* if SELECTED or STANDBY */
				port->sm_mux_state = AD_MUX_WAITING;
			break;
		case AD_MUX_WAITING:
			/* if SELECTED == FALSE return to DETACH state */
			if (!(port->sm_vars & AD_PORT_SELECTED)) {
				port->sm_vars &= ~AD_PORT_READY_N;
				/* in order to withhold the Selection Logic to
				 * check all ports READY_N value every callback
				 * cycle to update ready variable, we check
				 * READY_N and update READY here
				 */
				__set_agg_ports_ready(port->aggregator, __agg_ports_are_ready(port->aggregator));
				port->sm_mux_state = AD_MUX_DETACHED;
				break;
			}

			/* check if the wait_while_timer expired */
			if (port->sm_mux_timer_counter
			    && !(--port->sm_mux_timer_counter))
				port->sm_vars |= AD_PORT_READY_N;

			/* in order to withhold the selection logic to check
			 * all ports READY_N value every callback cycle to
			 * update ready variable, we check READY_N and update
			 * READY here
			 */
			__set_agg_ports_ready(port->aggregator, __agg_ports_are_ready(port->aggregator));

			/* if the wait_while_timer expired, and the port is
			 * in READY state, move to ATTACHED state
			 */
			if ((port->sm_vars & AD_PORT_READY)
			    && !port->sm_mux_timer_counter)
				port->sm_mux_state = AD_MUX_ATTACHED;
			break;
		case AD_MUX_ATTACHED:
			/* check also if agg_select_timer expired (so the
			 * edable port will take place only after this timer)
			 */
			if ((port->sm_vars & AD_PORT_SELECTED) &&
			    (port->partner_oper.port_state & LACP_STATE_SYNCHRONIZATION) &&
			    !__check_agg_selection_timer(port)) {
				if (port->aggregator->is_active)
					port->sm_mux_state =
					    AD_MUX_COLLECTING_DISTRIBUTING;
			} else if (!(port->sm_vars & AD_PORT_SELECTED) ||
				   (port->sm_vars & AD_PORT_STANDBY)) {
				/* if UNSELECTED or STANDBY */
				port->sm_vars &= ~AD_PORT_READY_N;
				/* in order to withhold the selection logic to
				 * check all ports READY_N value every callback
				 * cycle to update ready variable, we check
				 * READY_N and update READY here
				 */
				__set_agg_ports_ready(port->aggregator, __agg_ports_are_ready(port->aggregator));
				port->sm_mux_state = AD_MUX_DETACHED;
			} else if (port->aggregator->is_active) {
				port->actor_oper_port_state |=
				    LACP_STATE_SYNCHRONIZATION;
			}
			break;
		case AD_MUX_COLLECTING_DISTRIBUTING:
			if (!(port->sm_vars & AD_PORT_SELECTED) ||
			    (port->sm_vars & AD_PORT_STANDBY) ||
			    !(port->partner_oper.port_state & LACP_STATE_SYNCHRONIZATION) ||
			    !(port->actor_oper_port_state & LACP_STATE_SYNCHRONIZATION)) {
				port->sm_mux_state = AD_MUX_ATTACHED;
			} else {
				/* if port state hasn't changed make
				 * sure that a collecting distributing
				 * port in an active aggregator is enabled
				 */
				if (port->aggregator &&
				    port->aggregator->is_active &&
				    !__port_is_enabled(port)) {

					__enable_port(port);
				}
			}
			break;
		default:
			break;
		}
	}

	/* check if the state machine was changed */
	if (port->sm_mux_state != last_state) {
		slave_dbg(port->slave->bond->dev, port->slave->dev,
			  "Mux Machine: Port=%d, Last State=%d, Curr State=%d\n",
			  port->actor_port_number,
			  last_state,
			  port->sm_mux_state);
		switch (port->sm_mux_state) {
		case AD_MUX_DETACHED:
			port->actor_oper_port_state &= ~LACP_STATE_SYNCHRONIZATION;
			ad_disable_collecting_distributing(port,
							   update_slave_arr);
			port->actor_oper_port_state &= ~LACP_STATE_COLLECTING;
			port->actor_oper_port_state &= ~LACP_STATE_DISTRIBUTING;
			port->ntt = true;
			break;
		case AD_MUX_WAITING:
			port->sm_mux_timer_counter = __ad_timer_to_ticks(AD_WAIT_WHILE_TIMER, 0);
			break;
		case AD_MUX_ATTACHED:
			if (port->aggregator->is_active)
				port->actor_oper_port_state |=
				    LACP_STATE_SYNCHRONIZATION;
			else
				port->actor_oper_port_state &=
				    ~LACP_STATE_SYNCHRONIZATION;
			port->actor_oper_port_state &= ~LACP_STATE_COLLECTING;
			port->actor_oper_port_state &= ~LACP_STATE_DISTRIBUTING;
			ad_disable_collecting_distributing(port,
							   update_slave_arr);
			port->ntt = true;
			break;
		case AD_MUX_COLLECTING_DISTRIBUTING:
			port->actor_oper_port_state |= LACP_STATE_COLLECTING;
			port->actor_oper_port_state |= LACP_STATE_DISTRIBUTING;
			port->actor_oper_port_state |= LACP_STATE_SYNCHRONIZATION;
			ad_enable_collecting_distributing(port,
							  update_slave_arr);
			port->ntt = true;
			break;
		default:
			break;
		}
	}
}

/**
 * ad_rx_machine - handle a port's rx State Machine
 * @lacpdu: the lacpdu we've received
 * @port: the port we're looking at
 *
 * If lacpdu arrived, stop previous timer (if exists) and set the next state as
 * CURRENT. If timer expired set the state machine in the proper state.
 * In other cases, this function checks if we need to switch to other state.
 */
static void ad_rx_machine(struct lacpdu *lacpdu, struct port *port)
{
	rx_states_t last_state;

	/* keep current State Machine state to compare later if it was
	 * changed
	 */
	last_state = port->sm_rx_state;

	if (lacpdu) {
		atomic64_inc(&SLAVE_AD_INFO(port->slave)->stats.lacpdu_rx);
		atomic64_inc(&BOND_AD_INFO(port->slave->bond).stats.lacpdu_rx);
	}
	/* check if state machine should change state */

	/* first, check if port was reinitialized */
	if (port->sm_vars & AD_PORT_BEGIN) {
		port->sm_rx_state = AD_RX_INITIALIZE;
		port->sm_vars |= AD_PORT_CHURNED;
	/* check if port is not enabled */
	} else if (!(port->sm_vars & AD_PORT_BEGIN) && !port->is_enabled)
		port->sm_rx_state = AD_RX_PORT_DISABLED;
	/* check if new lacpdu arrived */
	else if (lacpdu && ((port->sm_rx_state == AD_RX_EXPIRED) ||
		 (port->sm_rx_state == AD_RX_DEFAULTED) ||
		 (port->sm_rx_state == AD_RX_CURRENT))) {
		if (port->sm_rx_state != AD_RX_CURRENT)
			port->sm_vars |= AD_PORT_CHURNED;
		port->sm_rx_timer_counter = 0;
		port->sm_rx_state = AD_RX_CURRENT;
	} else {
		/* if timer is on, and if it is expired */
		if (port->sm_rx_timer_counter &&
		    !(--port->sm_rx_timer_counter)) {
			switch (port->sm_rx_state) {
			case AD_RX_EXPIRED:
				port->sm_rx_state = AD_RX_DEFAULTED;
				break;
			case AD_RX_CURRENT:
				port->sm_rx_state = AD_RX_EXPIRED;
				break;
			default:
				break;
			}
		} else {
			/* if no lacpdu arrived and no timer is on */
			switch (port->sm_rx_state) {
			case AD_RX_PORT_DISABLED:
				if (port->is_enabled &&
				    (port->sm_vars & AD_PORT_LACP_ENABLED))
					port->sm_rx_state = AD_RX_EXPIRED;
				else if (port->is_enabled
					 && ((port->sm_vars
					      & AD_PORT_LACP_ENABLED) == 0))
					port->sm_rx_state = AD_RX_LACP_DISABLED;
				break;
			default:
				break;

			}
		}
	}

	/* check if the State machine was changed or new lacpdu arrived */
	if ((port->sm_rx_state != last_state) || (lacpdu)) {
		slave_dbg(port->slave->bond->dev, port->slave->dev,
			  "Rx Machine: Port=%d, Last State=%d, Curr State=%d\n",
			  port->actor_port_number,
			  last_state,
			  port->sm_rx_state);
		switch (port->sm_rx_state) {
		case AD_RX_INITIALIZE:
			if (!(port->actor_oper_port_key & AD_DUPLEX_KEY_MASKS))
				port->sm_vars &= ~AD_PORT_LACP_ENABLED;
			else
				port->sm_vars |= AD_PORT_LACP_ENABLED;
			port->sm_vars &= ~AD_PORT_SELECTED;
			__record_default(port);
			port->actor_oper_port_state &= ~LACP_STATE_EXPIRED;
			port->sm_rx_state = AD_RX_PORT_DISABLED;

			fallthrough;
		case AD_RX_PORT_DISABLED:
			port->sm_vars &= ~AD_PORT_MATCHED;
			break;
		case AD_RX_LACP_DISABLED:
			port->sm_vars &= ~AD_PORT_SELECTED;
			__record_default(port);
			port->partner_oper.port_state &= ~LACP_STATE_AGGREGATION;
			port->sm_vars |= AD_PORT_MATCHED;
			port->actor_oper_port_state &= ~LACP_STATE_EXPIRED;
			break;
		case AD_RX_EXPIRED:
			/* Reset of the Synchronization flag (Standard 43.4.12)
			 * This reset cause to disable this port in the
			 * COLLECTING_DISTRIBUTING state of the mux machine in
			 * case of EXPIRED even if LINK_DOWN didn't arrive for
			 * the port.
			 */
			port->partner_oper.port_state &= ~LACP_STATE_SYNCHRONIZATION;
			port->sm_vars &= ~AD_PORT_MATCHED;
			port->partner_oper.port_state |= LACP_STATE_LACP_TIMEOUT;
			port->partner_oper.port_state |= LACP_STATE_LACP_ACTIVITY;
			port->sm_rx_timer_counter = __ad_timer_to_ticks(AD_CURRENT_WHILE_TIMER, (u16)(AD_SHORT_TIMEOUT));
			port->actor_oper_port_state |= LACP_STATE_EXPIRED;
			port->sm_vars |= AD_PORT_CHURNED;
			break;
		case AD_RX_DEFAULTED:
			__update_default_selected(port);
			__record_default(port);
			port->sm_vars |= AD_PORT_MATCHED;
			port->actor_oper_port_state &= ~LACP_STATE_EXPIRED;
			break;
		case AD_RX_CURRENT:
			/* detect loopback situation */
			if (MAC_ADDRESS_EQUAL(&(lacpdu->actor_system),
					      &(port->actor_system))) {
				slave_err(port->slave->bond->dev, port->slave->dev, "An illegal loopback occurred on slave\n"
					  "Check the configuration to verify that all adapters are connected to 802.3ad compliant switch ports\n");
				return;
			}
			__update_selected(lacpdu, port);
			__update_ntt(lacpdu, port);
			__record_pdu(lacpdu, port);
			port->sm_rx_timer_counter = __ad_timer_to_ticks(AD_CURRENT_WHILE_TIMER, (u16)(port->actor_oper_port_state & LACP_STATE_LACP_TIMEOUT));
			port->actor_oper_port_state &= ~LACP_STATE_EXPIRED;
			break;
		default:
			break;
		}
	}
}

/**
 * ad_churn_machine - handle port churn's state machine
 * @port: the port we're looking at
 *
 */
static void ad_churn_machine(struct port *port)
{
	if (port->sm_vars & AD_PORT_CHURNED) {
		port->sm_vars &= ~AD_PORT_CHURNED;
		port->sm_churn_actor_state = AD_CHURN_MONITOR;
		port->sm_churn_partner_state = AD_CHURN_MONITOR;
		port->sm_churn_actor_timer_counter =
			__ad_timer_to_ticks(AD_ACTOR_CHURN_TIMER, 0);
		port->sm_churn_partner_timer_counter =
			 __ad_timer_to_ticks(AD_PARTNER_CHURN_TIMER, 0);
		return;
	}
	if (port->sm_churn_actor_timer_counter &&
	    !(--port->sm_churn_actor_timer_counter) &&
	    port->sm_churn_actor_state == AD_CHURN_MONITOR) {
		if (port->actor_oper_port_state & LACP_STATE_SYNCHRONIZATION) {
			port->sm_churn_actor_state = AD_NO_CHURN;
		} else {
			port->churn_actor_count++;
			port->sm_churn_actor_state = AD_CHURN;
		}
	}
	if (port->sm_churn_partner_timer_counter &&
	    !(--port->sm_churn_partner_timer_counter) &&
	    port->sm_churn_partner_state == AD_CHURN_MONITOR) {
		if (port->partner_oper.port_state & LACP_STATE_SYNCHRONIZATION) {
			port->sm_churn_partner_state = AD_NO_CHURN;
		} else {
			port->churn_partner_count++;
			port->sm_churn_partner_state = AD_CHURN;
		}
	}
}

/**
 * ad_tx_machine - handle a port's tx state machine
 * @port: the port we're looking at
 */
static void ad_tx_machine(struct port *port)
{
	/* check if tx timer expired, to verify that we do not send more than
	 * 3 packets per second
	 */
	if (port->sm_tx_timer_counter && !(--port->sm_tx_timer_counter)) {
		/* check if there is something to send */
		if (port->ntt && (port->sm_vars & AD_PORT_LACP_ENABLED)) {
			__update_lacpdu_from_port(port);

			if (ad_lacpdu_send(port) >= 0) {
				slave_dbg(port->slave->bond->dev,
					  port->slave->dev,
					  "Sent LACPDU on port %d\n",
					  port->actor_port_number);

				/* mark ntt as false, so it will not be sent
				 * again until demanded
				 */
				port->ntt = false;
			}
		}
		/* restart tx timer(to verify that we will not exceed
		 * AD_MAX_TX_IN_SECOND
		 */
		port->sm_tx_timer_counter = ad_ticks_per_sec/AD_MAX_TX_IN_SECOND;
	}
}

/**
 * ad_periodic_machine - handle a port's periodic state machine
 * @port: the port we're looking at
 *
 * Turn ntt flag on priodically to perform periodic transmission of lacpdu's.
 */
static void ad_periodic_machine(struct port *port)
{
	periodic_states_t last_state;

	/* keep current state machine state to compare later if it was changed */
	last_state = port->sm_periodic_state;

	/* check if port was reinitialized */
	if (((port->sm_vars & AD_PORT_BEGIN) || !(port->sm_vars & AD_PORT_LACP_ENABLED) || !port->is_enabled) ||
	    (!(port->actor_oper_port_state & LACP_STATE_LACP_ACTIVITY) && !(port->partner_oper.port_state & LACP_STATE_LACP_ACTIVITY))
	   ) {
		port->sm_periodic_state = AD_NO_PERIODIC;
	}
	/* check if state machine should change state */
	else if (port->sm_periodic_timer_counter) {
		/* check if periodic state machine expired */
		if (!(--port->sm_periodic_timer_counter)) {
			/* if expired then do tx */
			port->sm_periodic_state = AD_PERIODIC_TX;
		} else {
			/* If not expired, check if there is some new timeout
			 * parameter from the partner state
			 */
			switch (port->sm_periodic_state) {
			case AD_FAST_PERIODIC:
				if (!(port->partner_oper.port_state
				      & LACP_STATE_LACP_TIMEOUT))
					port->sm_periodic_state = AD_SLOW_PERIODIC;
				break;
			case AD_SLOW_PERIODIC:
				if ((port->partner_oper.port_state & LACP_STATE_LACP_TIMEOUT)) {
					port->sm_periodic_timer_counter = 0;
					port->sm_periodic_state = AD_PERIODIC_TX;
				}
				break;
			default:
				break;
			}
		}
	} else {
		switch (port->sm_periodic_state) {
		case AD_NO_PERIODIC:
			port->sm_periodic_state = AD_FAST_PERIODIC;
			break;
		case AD_PERIODIC_TX:
			if (!(port->partner_oper.port_state &
			    LACP_STATE_LACP_TIMEOUT))
				port->sm_periodic_state = AD_SLOW_PERIODIC;
			else
				port->sm_periodic_state = AD_FAST_PERIODIC;
			break;
		default:
			break;
		}
	}

	/* check if the state machine was changed */
	if (port->sm_periodic_state != last_state) {
		slave_dbg(port->slave->bond->dev, port->slave->dev,
			  "Periodic Machine: Port=%d, Last State=%d, Curr State=%d\n",
			  port->actor_port_number, last_state,
			  port->sm_periodic_state);
		switch (port->sm_periodic_state) {
		case AD_NO_PERIODIC:
			port->sm_periodic_timer_counter = 0;
			break;
		case AD_FAST_PERIODIC:
			/* decrement 1 tick we lost in the PERIODIC_TX cycle */
			port->sm_periodic_timer_counter = __ad_timer_to_ticks(AD_PERIODIC_TIMER, (u16)(AD_FAST_PERIODIC_TIME))-1;
			break;
		case AD_SLOW_PERIODIC:
			/* decrement 1 tick we lost in the PERIODIC_TX cycle */
			port->sm_periodic_timer_counter = __ad_timer_to_ticks(AD_PERIODIC_TIMER, (u16)(AD_SLOW_PERIODIC_TIME))-1;
			break;
		case AD_PERIODIC_TX:
			port->ntt = true;
			break;
		default:
			break;
		}
	}
}

/**
 * ad_port_selection_logic - select aggregation groups
 * @port: the port we're looking at
 * @update_slave_arr: Does slave array need update?
 *
 * Select aggregation groups, and assign each port for it's aggregetor. The
 * selection logic is called in the inititalization (after all the handshkes),
 * and after every lacpdu receive (if selected is off).
 */
static void ad_port_selection_logic(struct port *port, bool *update_slave_arr)
{
	struct aggregator *aggregator, *free_aggregator = NULL, *temp_aggregator;
	struct port *last_port = NULL, *curr_port;
	struct list_head *iter;
	struct bonding *bond;
	struct slave *slave;
	int found = 0;

	/* if the port is already Selected, do nothing */
	if (port->sm_vars & AD_PORT_SELECTED)
		return;

	bond = __get_bond_by_port(port);

	/* if the port is connected to other aggregator, detach it */
	if (port->aggregator) {
		/* detach the port from its former aggregator */
		temp_aggregator = port->aggregator;
		for (curr_port = temp_aggregator->lag_ports; curr_port;
		     last_port = curr_port,
		     curr_port = curr_port->next_port_in_aggregator) {
			if (curr_port == port) {
				temp_aggregator->num_of_ports--;
				/* if it is the first port attached to the
				 * aggregator
				 */
				if (!last_port) {
					temp_aggregator->lag_ports =
						port->next_port_in_aggregator;
				} else {
					/* not the first port attached to the
					 * aggregator
					 */
					last_port->next_port_in_aggregator =
						port->next_port_in_aggregator;
				}

				/* clear the port's relations to this
				 * aggregator
				 */
				port->aggregator = NULL;
				port->next_port_in_aggregator = NULL;
				port->actor_port_aggregator_identifier = 0;

				slave_dbg(bond->dev, port->slave->dev, "Port %d left LAG %d\n",
					  port->actor_port_number,
					  temp_aggregator->aggregator_identifier);
				/* if the aggregator is empty, clear its
				 * parameters, and set it ready to be attached
				 */
				if (!temp_aggregator->lag_ports)
					ad_clear_agg(temp_aggregator);
				break;
			}
		}
		if (!curr_port) {
			/* meaning: the port was related to an aggregator
			 * but was not on the aggregator port list
			 */
			net_warn_ratelimited("%s: (slave %s): Warning: Port %d was related to aggregator %d but was not on its port list\n",
					     port->slave->bond->dev->name,
					     port->slave->dev->name,
					     port->actor_port_number,
					     port->aggregator->aggregator_identifier);
		}
	}
	/* search on all aggregators for a suitable aggregator for this port */
	bond_for_each_slave(bond, slave, iter) {
		aggregator = &(SLAVE_AD_INFO(slave)->aggregator);

		/* keep a free aggregator for later use(if needed) */
		if (!aggregator->lag_ports) {
			if (!free_aggregator)
				free_aggregator = aggregator;
			continue;
		}
		/* check if current aggregator suits us */
		if (((aggregator->actor_oper_aggregator_key == port->actor_oper_port_key) && /* if all parameters match AND */
		     MAC_ADDRESS_EQUAL(&(aggregator->partner_system), &(port->partner_oper.system)) &&
		     (aggregator->partner_system_priority == port->partner_oper.system_priority) &&
		     (aggregator->partner_oper_aggregator_key == port->partner_oper.key)
		    ) &&
		    ((!MAC_ADDRESS_EQUAL(&(port->partner_oper.system), &(null_mac_addr)) && /* partner answers */
		      !aggregator->is_individual)  /* but is not individual OR */
		    )
		   ) {
			/* attach to the founded aggregator */
			port->aggregator = aggregator;
			port->actor_port_aggregator_identifier =
				port->aggregator->aggregator_identifier;
			port->next_port_in_aggregator = aggregator->lag_ports;
			port->aggregator->num_of_ports++;
			aggregator->lag_ports = port;
			slave_dbg(bond->dev, slave->dev, "Port %d joined LAG %d (existing LAG)\n",
				  port->actor_port_number,
				  port->aggregator->aggregator_identifier);

			/* mark this port as selected */
			port->sm_vars |= AD_PORT_SELECTED;
			found = 1;
			break;
		}
	}

	/* the port couldn't find an aggregator - attach it to a new
	 * aggregator
	 */
	if (!found) {
		if (free_aggregator) {
			/* assign port a new aggregator */
			port->aggregator = free_aggregator;
			port->actor_port_aggregator_identifier =
				port->aggregator->aggregator_identifier;

			/* update the new aggregator's parameters
			 * if port was responsed from the end-user
			 */
			if (port->actor_oper_port_key & AD_DUPLEX_KEY_MASKS)
				/* if port is full duplex */
				port->aggregator->is_individual = false;
			else
				port->aggregator->is_individual = true;

			port->aggregator->actor_admin_aggregator_key =
				port->actor_admin_port_key;
			port->aggregator->actor_oper_aggregator_key =
				port->actor_oper_port_key;
			port->aggregator->partner_system =
				port->partner_oper.system;
			port->aggregator->partner_system_priority =
				port->partner_oper.system_priority;
			port->aggregator->partner_oper_aggregator_key = port->partner_oper.key;
			port->aggregator->receive_state = 1;
			port->aggregator->transmit_state = 1;
			port->aggregator->lag_ports = port;
			port->aggregator->num_of_ports++;

			/* mark this port as selected */
			port->sm_vars |= AD_PORT_SELECTED;

			slave_dbg(bond->dev, port->slave->dev, "Port %d joined LAG %d (new LAG)\n",
				  port->actor_port_number,
				  port->aggregator->aggregator_identifier);
		} else {
			slave_err(bond->dev, port->slave->dev,
				  "Port %d did not find a suitable aggregator\n",
				  port->actor_port_number);
		}
	}
	/* if all aggregator's ports are READY_N == TRUE, set ready=TRUE
	 * in all aggregator's ports, else set ready=FALSE in all
	 * aggregator's ports
	 */
	__set_agg_ports_ready(port->aggregator,
			      __agg_ports_are_ready(port->aggregator));

	aggregator = __get_first_agg(port);
	ad_agg_selection_logic(aggregator, update_slave_arr);

	if (!port->aggregator->is_active)
		port->actor_oper_port_state &= ~LACP_STATE_SYNCHRONIZATION;
}

/* Decide if "agg" is a better choice for the new active aggregator that
 * the current best, according to the ad_select policy.
 */
static struct aggregator *ad_agg_selection_test(struct aggregator *best,
						struct aggregator *curr)
{
	/* 0. If no best, select current.
	 *
	 * 1. If the current agg is not individual, and the best is
	 *    individual, select current.
	 *
	 * 2. If current agg is individual and the best is not, keep best.
	 *
	 * 3. Therefore, current and best are both individual or both not
	 *    individual, so:
	 *
	 * 3a. If current agg partner replied, and best agg partner did not,
	 *     select current.
	 *
	 * 3b. If current agg partner did not reply and best agg partner
	 *     did reply, keep best.
	 *
	 * 4.  Therefore, current and best both have partner replies or
	 *     both do not, so perform selection policy:
	 *
	 * BOND_AD_COUNT: Select by count of ports.  If count is equal,
	 *     select by bandwidth.
	 *
	 * BOND_AD_STABLE, BOND_AD_BANDWIDTH: Select by bandwidth.
	 */
	if (!best)
		return curr;

	if (!curr->is_individual && best->is_individual)
		return curr;

	if (curr->is_individual && !best->is_individual)
		return best;

	if (__agg_has_partner(curr) && !__agg_has_partner(best))
		return curr;

	if (!__agg_has_partner(curr) && __agg_has_partner(best))
		return best;

	switch (__get_agg_selection_mode(curr->lag_ports)) {
	case BOND_AD_COUNT:
		if (__agg_active_ports(curr) > __agg_active_ports(best))
			return curr;

		if (__agg_active_ports(curr) < __agg_active_ports(best))
			return best;

		fallthrough;
	case BOND_AD_STABLE:
	case BOND_AD_BANDWIDTH:
		if (__get_agg_bandwidth(curr) > __get_agg_bandwidth(best))
			return curr;

		break;

	default:
		net_warn_ratelimited("%s: (slave %s): Impossible agg select mode %d\n",
				     curr->slave->bond->dev->name,
				     curr->slave->dev->name,
				     __get_agg_selection_mode(curr->lag_ports));
		break;
	}

	return best;
}

static int agg_device_up(const struct aggregator *agg)
{
	struct port *port = agg->lag_ports;

	if (!port)
		return 0;

	for (port = agg->lag_ports; port;
	     port = port->next_port_in_aggregator) {
		if (netif_running(port->slave->dev) &&
		    netif_carrier_ok(port->slave->dev))
			return 1;
	}

	return 0;
}

/**
 * ad_agg_selection_logic - select an aggregation group for a team
 * @agg: the aggregator we're looking at
 * @update_slave_arr: Does slave array need update?
 *
 * It is assumed that only one aggregator may be selected for a team.
 *
 * The logic of this function is to select the aggregator according to
 * the ad_select policy:
 *
 * BOND_AD_STABLE: select the aggregator with the most ports attached to
 * it, and to reselect the active aggregator only if the previous
 * aggregator has no more ports related to it.
 *
 * BOND_AD_BANDWIDTH: select the aggregator with the highest total
 * bandwidth, and reselect whenever a link state change takes place or the
 * set of slaves in the bond changes.
 *
 * BOND_AD_COUNT: select the aggregator with largest number of ports
 * (slaves), and reselect whenever a link state change takes place or the
 * set of slaves in the bond changes.
 *
 * FIXME: this function MUST be called with the first agg in the bond, or
 * __get_active_agg() won't work correctly. This function should be better
 * called with the bond itself, and retrieve the first agg from it.
 */
static void ad_agg_selection_logic(struct aggregator *agg,
				   bool *update_slave_arr)
{
	struct aggregator *best, *active, *origin;
	struct bonding *bond = agg->slave->bond;
	struct list_head *iter;
	struct slave *slave;
	struct port *port;

	rcu_read_lock();
	origin = agg;
	active = __get_active_agg(agg);
	best = (active && agg_device_up(active)) ? active : NULL;

	bond_for_each_slave_rcu(bond, slave, iter) {
		agg = &(SLAVE_AD_INFO(slave)->aggregator);

		agg->is_active = 0;

		if (__agg_active_ports(agg) && agg_device_up(agg))
			best = ad_agg_selection_test(best, agg);
	}

	if (best &&
	    __get_agg_selection_mode(best->lag_ports) == BOND_AD_STABLE) {
		/* For the STABLE policy, don't replace the old active
		 * aggregator if it's still active (it has an answering
		 * partner) or if both the best and active don't have an
		 * answering partner.
		 */
		if (active && active->lag_ports &&
		    __agg_active_ports(active) &&
		    (__agg_has_partner(active) ||
		     (!__agg_has_partner(active) &&
		     !__agg_has_partner(best)))) {
			if (!(!active->actor_oper_aggregator_key &&
			      best->actor_oper_aggregator_key)) {
				best = NULL;
				active->is_active = 1;
			}
		}
	}

	if (best && (best == active)) {
		best = NULL;
		active->is_active = 1;
	}

	/* if there is new best aggregator, activate it */
	if (best) {
		netdev_dbg(bond->dev, "(slave %s): best Agg=%d; P=%d; a k=%d; p k=%d; Ind=%d; Act=%d\n",
			   best->slave ? best->slave->dev->name : "NULL",
			   best->aggregator_identifier, best->num_of_ports,
			   best->actor_oper_aggregator_key,
			   best->partner_oper_aggregator_key,
			   best->is_individual, best->is_active);
		netdev_dbg(bond->dev, "(slave %s): best ports %p slave %p\n",
			   best->slave ? best->slave->dev->name : "NULL",
			   best->lag_ports, best->slave);

		bond_for_each_slave_rcu(bond, slave, iter) {
			agg = &(SLAVE_AD_INFO(slave)->aggregator);

			slave_dbg(bond->dev, slave->dev, "Agg=%d; P=%d; a k=%d; p k=%d; Ind=%d; Act=%d\n",
				  agg->aggregator_identifier, agg->num_of_ports,
				  agg->actor_oper_aggregator_key,
				  agg->partner_oper_aggregator_key,
				  agg->is_individual, agg->is_active);
		}

		/* check if any partner replies */
		if (best->is_individual)
			net_warn_ratelimited("%s: Warning: No 802.3ad response from the link partner for any adapters in the bond\n",
					     bond->dev->name);

		best->is_active = 1;
		netdev_dbg(bond->dev, "(slave %s): LAG %d chosen as the active LAG\n",
			   best->slave ? best->slave->dev->name : "NULL",
			   best->aggregator_identifier);
		netdev_dbg(bond->dev, "(slave %s): Agg=%d; P=%d; a k=%d; p k=%d; Ind=%d; Act=%d\n",
			   best->slave ? best->slave->dev->name : "NULL",
			   best->aggregator_identifier, best->num_of_ports,
			   best->actor_oper_aggregator_key,
			   best->partner_oper_aggregator_key,
			   best->is_individual, best->is_active);

		/* disable the ports that were related to the former
		 * active_aggregator
		 */
		if (active) {
			for (port = active->lag_ports; port;
			     port = port->next_port_in_aggregator) {
				__disable_port(port);
			}
		}
		/* Slave array needs update. */
		*update_slave_arr = true;
	}

	/* if the selected aggregator is of join individuals
	 * (partner_system is NULL), enable their ports
	 */
	active = __get_active_agg(origin);

	if (active) {
		if (!__agg_has_partner(active)) {
			for (port = active->lag_ports; port;
			     port = port->next_port_in_aggregator) {
				__enable_port(port);
			}
		}
	}

	rcu_read_unlock();

	bond_3ad_set_carrier(bond);
}

/**
 * ad_clear_agg - clear a given aggregator's parameters
 * @aggregator: the aggregator we're looking at
 */
static void ad_clear_agg(struct aggregator *aggregator)
{
	if (aggregator) {
		aggregator->is_individual = false;
		aggregator->actor_admin_aggregator_key = 0;
		aggregator->actor_oper_aggregator_key = 0;
		eth_zero_addr(aggregator->partner_system.mac_addr_value);
		aggregator->partner_system_priority = 0;
		aggregator->partner_oper_aggregator_key = 0;
		aggregator->receive_state = 0;
		aggregator->transmit_state = 0;
		aggregator->lag_ports = NULL;
		aggregator->is_active = 0;
		aggregator->num_of_ports = 0;
		pr_debug("%s: LAG %d was cleared\n",
			 aggregator->slave ?
			 aggregator->slave->dev->name : "NULL",
			 aggregator->aggregator_identifier);
	}
}

/**
 * ad_initialize_agg - initialize a given aggregator's parameters
 * @aggregator: the aggregator we're looking at
 */
static void ad_initialize_agg(struct aggregator *aggregator)
{
	if (aggregator) {
		ad_clear_agg(aggregator);

		eth_zero_addr(aggregator->aggregator_mac_address.mac_addr_value);
		aggregator->aggregator_identifier = 0;
		aggregator->slave = NULL;
	}
}

/**
 * ad_initialize_port - initialize a given port's parameters
 * @port: the port we're looking at
 * @lacp_fast: boolean. whether fast periodic should be used
 */
static void ad_initialize_port(struct port *port, int lacp_fast)
{
	static const struct port_params tmpl = {
		.system_priority = 0xffff,
		.key             = 1,
		.port_number     = 1,
		.port_priority   = 0xff,
		.port_state      = 1,
	};
	static const struct lacpdu lacpdu = {
		.subtype		= 0x01,
		.version_number = 0x01,
		.tlv_type_actor_info = 0x01,
		.actor_information_length = 0x14,
		.tlv_type_partner_info = 0x02,
		.partner_information_length = 0x14,
		.tlv_type_collector_info = 0x03,
		.collector_information_length = 0x10,
		.collector_max_delay = htons(AD_COLLECTOR_MAX_DELAY),
	};

	if (port) {
		port->actor_port_priority = 0xff;
		port->actor_port_aggregator_identifier = 0;
		port->ntt = false;
		port->actor_admin_port_state = LACP_STATE_AGGREGATION |
					       LACP_STATE_LACP_ACTIVITY;
		port->actor_oper_port_state  = LACP_STATE_AGGREGATION |
					       LACP_STATE_LACP_ACTIVITY;

		if (lacp_fast)
			port->actor_oper_port_state |= LACP_STATE_LACP_TIMEOUT;

		memcpy(&port->partner_admin, &tmpl, sizeof(tmpl));
		memcpy(&port->partner_oper, &tmpl, sizeof(tmpl));

		port->is_enabled = true;
		/* private parameters */
		port->sm_vars = AD_PORT_BEGIN | AD_PORT_LACP_ENABLED;
		port->sm_rx_state = 0;
		port->sm_rx_timer_counter = 0;
		port->sm_periodic_state = 0;
		port->sm_periodic_timer_counter = 0;
		port->sm_mux_state = 0;
		port->sm_mux_timer_counter = 0;
		port->sm_tx_state = 0;
		port->aggregator = NULL;
		port->next_port_in_aggregator = NULL;
		port->transaction_id = 0;

		port->sm_churn_actor_timer_counter = 0;
		port->sm_churn_actor_state = 0;
		port->churn_actor_count = 0;
		port->sm_churn_partner_timer_counter = 0;
		port->sm_churn_partner_state = 0;
		port->churn_partner_count = 0;

		memcpy(&port->lacpdu, &lacpdu, sizeof(lacpdu));
	}
}

/**
 * ad_enable_collecting_distributing - enable a port's transmit/receive
 * @port: the port we're looking at
 * @update_slave_arr: Does slave array need update?
 *
 * Enable @port if it's in an active aggregator
 */
static void ad_enable_collecting_distributing(struct port *port,
					      bool *update_slave_arr)
{
	if (port->aggregator->is_active) {
		slave_dbg(port->slave->bond->dev, port->slave->dev,
			  "Enabling port %d (LAG %d)\n",
			  port->actor_port_number,
			  port->aggregator->aggregator_identifier);
		__enable_port(port);
		/* Slave array needs update */
		*update_slave_arr = true;
	}
}

/**
 * ad_disable_collecting_distributing - disable a port's transmit/receive
 * @port: the port we're looking at
 * @update_slave_arr: Does slave array need update?
 */
static void ad_disable_collecting_distributing(struct port *port,
					       bool *update_slave_arr)
{
	if (port->aggregator &&
	    !MAC_ADDRESS_EQUAL(&(port->aggregator->partner_system),
			       &(null_mac_addr))) {
		slave_dbg(port->slave->bond->dev, port->slave->dev,
			  "Disabling port %d (LAG %d)\n",
			  port->actor_port_number,
			  port->aggregator->aggregator_identifier);
		__disable_port(port);
		/* Slave array needs an update */
		*update_slave_arr = true;
	}
}

/**
 * ad_marker_info_received - handle receive of a Marker information frame
 * @marker_info: Marker info received
 * @port: the port we're looking at
 */
static void ad_marker_info_received(struct bond_marker *marker_info,
				    struct port *port)
{
	struct bond_marker marker;

	atomic64_inc(&SLAVE_AD_INFO(port->slave)->stats.marker_rx);
	atomic64_inc(&BOND_AD_INFO(port->slave->bond).stats.marker_rx);

	/* copy the received marker data to the response marker */
	memcpy(&marker, marker_info, sizeof(struct bond_marker));
	/* change the marker subtype to marker response */
	marker.tlv_type = AD_MARKER_RESPONSE_SUBTYPE;

	/* send the marker response */
	if (ad_marker_send(port, &marker) >= 0)
		slave_dbg(port->slave->bond->dev, port->slave->dev,
			  "Sent Marker Response on port %d\n",
			  port->actor_port_number);
}

/**
 * ad_marker_response_received - handle receive of a marker response frame
 * @marker: marker PDU received
 * @port: the port we're looking at
 *
 * This function does nothing since we decided not to implement send and handle
 * response for marker PDU's, in this stage, but only to respond to marker
 * information.
 */
static void ad_marker_response_received(struct bond_marker *marker,
					struct port *port)
{
	atomic64_inc(&SLAVE_AD_INFO(port->slave)->stats.marker_resp_rx);
	atomic64_inc(&BOND_AD_INFO(port->slave->bond).stats.marker_resp_rx);

	/* DO NOTHING, SINCE WE DECIDED NOT TO IMPLEMENT THIS FEATURE FOR NOW */
}

/* ========= AD exported functions to the main bonding code ========= */

/* Check aggregators status in team every T seconds */
#define AD_AGGREGATOR_SELECTION_TIMER  8

/**
 * bond_3ad_initiate_agg_selection - initate aggregator selection
 * @bond: bonding struct
 * @timeout: timeout value to set
 *
 * Set the aggregation selection timer, to initiate an agg selection in
 * the very near future.  Called during first initialization, and during
 * any down to up transitions of the bond.
 */
void bond_3ad_initiate_agg_selection(struct bonding *bond, int timeout)
{
	BOND_AD_INFO(bond).agg_select_timer = timeout;
}

/**
 * bond_3ad_initialize - initialize a bond's 802.3ad parameters and structures
 * @bond: bonding struct to work on
 * @tick_resolution: tick duration (millisecond resolution)
 *
 * Can be called only after the mac address of the bond is set.
 */
void bond_3ad_initialize(struct bonding *bond, u16 tick_resolution)
{
	/* check that the bond is not initialized yet */
	if (!MAC_ADDRESS_EQUAL(&(BOND_AD_INFO(bond).system.sys_mac_addr),
				bond->dev->dev_addr)) {

		BOND_AD_INFO(bond).aggregator_identifier = 0;

		BOND_AD_INFO(bond).system.sys_priority =
			bond->params.ad_actor_sys_prio;
		if (is_zero_ether_addr(bond->params.ad_actor_system))
			BOND_AD_INFO(bond).system.sys_mac_addr =
			    *((struct mac_addr *)bond->dev->dev_addr);
		else
			BOND_AD_INFO(bond).system.sys_mac_addr =
			    *((struct mac_addr *)bond->params.ad_actor_system);

		/* initialize how many times this module is called in one
		 * second (should be about every 100ms)
		 */
		ad_ticks_per_sec = tick_resolution;

		bond_3ad_initiate_agg_selection(bond,
						AD_AGGREGATOR_SELECTION_TIMER *
						ad_ticks_per_sec);
	}
}

/**
 * bond_3ad_bind_slave - initialize a slave's port
 * @slave: slave struct to work on
 *
 * Returns:   0 on success
 *          < 0 on error
 */
void bond_3ad_bind_slave(struct slave *slave)
{
	struct bonding *bond = bond_get_bond_by_slave(slave);
	struct port *port;
	struct aggregator *aggregator;

	/* check that the slave has not been initialized yet. */
	if (SLAVE_AD_INFO(slave)->port.slave != slave) {

		/* port initialization */
		port = &(SLAVE_AD_INFO(slave)->port);

		ad_initialize_port(port, bond->params.lacp_fast);

		port->slave = slave;
		port->actor_port_number = SLAVE_AD_INFO(slave)->id;
		/* key is determined according to the link speed, duplex and
		 * user key
		 */
		port->actor_admin_port_key = bond->params.ad_user_port_key << 6;
		ad_update_actor_keys(port, false);
		/* actor system is the bond's system */
		__ad_actor_update_port(port);
		/* tx timer(to verify that no more than MAX_TX_IN_SECOND
		 * lacpdu's are sent in one second)
		 */
		port->sm_tx_timer_counter = ad_ticks_per_sec/AD_MAX_TX_IN_SECOND;

		__disable_port(port);

		/* aggregator initialization */
		aggregator = &(SLAVE_AD_INFO(slave)->aggregator);

		ad_initialize_agg(aggregator);

		aggregator->aggregator_mac_address = *((struct mac_addr *)bond->dev->dev_addr);
		aggregator->aggregator_identifier = ++BOND_AD_INFO(bond).aggregator_identifier;
		aggregator->slave = slave;
		aggregator->is_active = 0;
		aggregator->num_of_ports = 0;
	}
}

/**
 * bond_3ad_unbind_slave - deinitialize a slave's port
 * @slave: slave struct to work on
 *
 * Search for the aggregator that is related to this port, remove the
 * aggregator and assign another aggregator for other port related to it
 * (if any), and remove the port.
 */
void bond_3ad_unbind_slave(struct slave *slave)
{
	struct port *port, *prev_port, *temp_port;
	struct aggregator *aggregator, *new_aggregator, *temp_aggregator;
	int select_new_active_agg = 0;
	struct bonding *bond = slave->bond;
	struct slave *slave_iter;
	struct list_head *iter;
	bool dummy_slave_update; /* Ignore this value as caller updates array */

	/* Sync against bond_3ad_state_machine_handler() */
	spin_lock_bh(&bond->mode_lock);
	aggregator = &(SLAVE_AD_INFO(slave)->aggregator);
	port = &(SLAVE_AD_INFO(slave)->port);

	/* if slave is null, the whole port is not initialized */
	if (!port->slave) {
		slave_warn(bond->dev, slave->dev, "Trying to unbind an uninitialized port\n");
		goto out;
	}

	slave_dbg(bond->dev, slave->dev, "Unbinding Link Aggregation Group %d\n",
		  aggregator->aggregator_identifier);

	/* Tell the partner that this port is not suitable for aggregation */
	port->actor_oper_port_state &= ~LACP_STATE_SYNCHRONIZATION;
	port->actor_oper_port_state &= ~LACP_STATE_COLLECTING;
	port->actor_oper_port_state &= ~LACP_STATE_DISTRIBUTING;
	port->actor_oper_port_state &= ~LACP_STATE_AGGREGATION;
	__update_lacpdu_from_port(port);
	ad_lacpdu_send(port);

	/* check if this aggregator is occupied */
	if (aggregator->lag_ports) {
		/* check if there are other ports related to this aggregator
		 * except the port related to this slave(thats ensure us that
		 * there is a reason to search for new aggregator, and that we
		 * will find one
		 */
		if ((aggregator->lag_ports != port) ||
		    (aggregator->lag_ports->next_port_in_aggregator)) {
			/* find new aggregator for the related port(s) */
			bond_for_each_slave(bond, slave_iter, iter) {
				new_aggregator = &(SLAVE_AD_INFO(slave_iter)->aggregator);
				/* if the new aggregator is empty, or it is
				 * connected to our port only
				 */
				if (!new_aggregator->lag_ports ||
				    ((new_aggregator->lag_ports == port) &&
				     !new_aggregator->lag_ports->next_port_in_aggregator))
					break;
			}
			if (!slave_iter)
				new_aggregator = NULL;

			/* if new aggregator found, copy the aggregator's
			 * parameters and connect the related lag_ports to the
			 * new aggregator
			 */
			if ((new_aggregator) && ((!new_aggregator->lag_ports) || ((new_aggregator->lag_ports == port) && !new_aggregator->lag_ports->next_port_in_aggregator))) {
				slave_dbg(bond->dev, slave->dev, "Some port(s) related to LAG %d - replacing with LAG %d\n",
					  aggregator->aggregator_identifier,
					  new_aggregator->aggregator_identifier);

				if ((new_aggregator->lag_ports == port) &&
				    new_aggregator->is_active) {
					slave_info(bond->dev, slave->dev, "Removing an active aggregator\n");
					select_new_active_agg = 1;
				}

				new_aggregator->is_individual = aggregator->is_individual;
				new_aggregator->actor_admin_aggregator_key = aggregator->actor_admin_aggregator_key;
				new_aggregator->actor_oper_aggregator_key = aggregator->actor_oper_aggregator_key;
				new_aggregator->partner_system = aggregator->partner_system;
				new_aggregator->partner_system_priority = aggregator->partner_system_priority;
				new_aggregator->partner_oper_aggregator_key = aggregator->partner_oper_aggregator_key;
				new_aggregator->receive_state = aggregator->receive_state;
				new_aggregator->transmit_state = aggregator->transmit_state;
				new_aggregator->lag_ports = aggregator->lag_ports;
				new_aggregator->is_active = aggregator->is_active;
				new_aggregator->num_of_ports = aggregator->num_of_ports;

				/* update the information that is written on
				 * the ports about the aggregator
				 */
				for (temp_port = aggregator->lag_ports; temp_port;
				     temp_port = temp_port->next_port_in_aggregator) {
					temp_port->aggregator = new_aggregator;
					temp_port->actor_port_aggregator_identifier = new_aggregator->aggregator_identifier;
				}

				ad_clear_agg(aggregator);

				if (select_new_active_agg)
					ad_agg_selection_logic(__get_first_agg(port),
							       &dummy_slave_update);
			} else {
				slave_warn(bond->dev, slave->dev, "unbinding aggregator, and could not find a new aggregator for its ports\n");
			}
		} else {
			/* in case that the only port related to this
			 * aggregator is the one we want to remove
			 */
			select_new_active_agg = aggregator->is_active;
			ad_clear_agg(aggregator);
			if (select_new_active_agg) {
				slave_info(bond->dev, slave->dev, "Removing an active aggregator\n");
				/* select new active aggregator */
				temp_aggregator = __get_first_agg(port);
				if (temp_aggregator)
					ad_agg_selection_logic(temp_aggregator,
							       &dummy_slave_update);
			}
		}
	}

	slave_dbg(bond->dev, slave->dev, "Unbinding port %d\n", port->actor_port_number);

	/* find the aggregator that this port is connected to */
	bond_for_each_slave(bond, slave_iter, iter) {
		temp_aggregator = &(SLAVE_AD_INFO(slave_iter)->aggregator);
		prev_port = NULL;
		/* search the port in the aggregator's related ports */
		for (temp_port = temp_aggregator->lag_ports; temp_port;
		     prev_port = temp_port,
		     temp_port = temp_port->next_port_in_aggregator) {
			if (temp_port == port) {
				/* the aggregator found - detach the port from
				 * this aggregator
				 */
				if (prev_port)
					prev_port->next_port_in_aggregator = temp_port->next_port_in_aggregator;
				else
					temp_aggregator->lag_ports = temp_port->next_port_in_aggregator;
				temp_aggregator->num_of_ports--;
				if (__agg_active_ports(temp_aggregator) == 0) {
					select_new_active_agg = temp_aggregator->is_active;
					ad_clear_agg(temp_aggregator);
					if (select_new_active_agg) {
						slave_info(bond->dev, slave->dev, "Removing an active aggregator\n");
						/* select new active aggregator */
						ad_agg_selection_logic(__get_first_agg(port),
							               &dummy_slave_update);
					}
				}
				break;
			}
		}
	}
	port->slave = NULL;

out:
	spin_unlock_bh(&bond->mode_lock);
}

/**
 * bond_3ad_update_ad_actor_settings - reflect change of actor settings to ports
 * @bond: bonding struct to work on
 *
 * If an ad_actor setting gets changed we need to update the individual port
 * settings so the bond device will use the new values when it gets upped.
 */
void bond_3ad_update_ad_actor_settings(struct bonding *bond)
{
	struct list_head *iter;
	struct slave *slave;

	ASSERT_RTNL();

	BOND_AD_INFO(bond).system.sys_priority = bond->params.ad_actor_sys_prio;
	if (is_zero_ether_addr(bond->params.ad_actor_system))
		BOND_AD_INFO(bond).system.sys_mac_addr =
		    *((struct mac_addr *)bond->dev->dev_addr);
	else
		BOND_AD_INFO(bond).system.sys_mac_addr =
		    *((struct mac_addr *)bond->params.ad_actor_system);

	spin_lock_bh(&bond->mode_lock);
	bond_for_each_slave(bond, slave, iter) {
		struct port *port = &(SLAVE_AD_INFO(slave))->port;

		__ad_actor_update_port(port);
		port->ntt = true;
	}
	spin_unlock_bh(&bond->mode_lock);
}

/**
 * bond_3ad_state_machine_handler - handle state machines timeout
 * @work: work context to fetch bonding struct to work on from
 *
 * The state machine handling concept in this module is to check every tick
 * which state machine should operate any function. The execution order is
 * round robin, so when we have an interaction between state machines, the
 * reply of one to each other might be delayed until next tick.
 *
 * This function also complete the initialization when the agg_select_timer
 * times out, and it selects an aggregator for the ports that are yet not
 * related to any aggregator, and selects the active aggregator for a bond.
 */
void bond_3ad_state_machine_handler(struct work_struct *work)
{
	struct bonding *bond = container_of(work, struct bonding,
					    ad_work.work);
	struct aggregator *aggregator;
	struct list_head *iter;
	struct slave *slave;
	struct port *port;
	bool should_notify_rtnl = BOND_SLAVE_NOTIFY_LATER;
	bool update_slave_arr = false;

	/* Lock to protect data accessed by all (e.g., port->sm_vars) and
	 * against running with bond_3ad_unbind_slave. ad_rx_machine may run
	 * concurrently due to incoming LACPDU as well.
	 */
	spin_lock_bh(&bond->mode_lock);
	rcu_read_lock();

	/* check if there are any slaves */
	if (!bond_has_slaves(bond))
		goto re_arm;

	/* check if agg_select_timer timer after initialize is timed out */
	if (BOND_AD_INFO(bond).agg_select_timer &&
	    !(--BOND_AD_INFO(bond).agg_select_timer)) {
		slave = bond_first_slave_rcu(bond);
		port = slave ? &(SLAVE_AD_INFO(slave)->port) : NULL;

		/* select the active aggregator for the bond */
		if (port) {
			if (!port->slave) {
				net_warn_ratelimited("%s: Warning: bond's first port is uninitialized\n",
						     bond->dev->name);
				goto re_arm;
			}

			aggregator = __get_first_agg(port);
			ad_agg_selection_logic(aggregator, &update_slave_arr);
		}
		bond_3ad_set_carrier(bond);
	}

	/* for each port run the state machines */
	bond_for_each_slave_rcu(bond, slave, iter) {
		port = &(SLAVE_AD_INFO(slave)->port);
		if (!port->slave) {
			net_warn_ratelimited("%s: Warning: Found an uninitialized port\n",
					    bond->dev->name);
			goto re_arm;
		}

		ad_rx_machine(NULL, port);
		ad_periodic_machine(port);
		ad_port_selection_logic(port, &update_slave_arr);
		ad_mux_machine(port, &update_slave_arr);
		ad_tx_machine(port);
		ad_churn_machine(port);

		/* turn off the BEGIN bit, since we already handled it */
		if (port->sm_vars & AD_PORT_BEGIN)
			port->sm_vars &= ~AD_PORT_BEGIN;
	}

re_arm:
	bond_for_each_slave_rcu(bond, slave, iter) {
		if (slave->should_notify) {
			should_notify_rtnl = BOND_SLAVE_NOTIFY_NOW;
			break;
		}
	}
	rcu_read_unlock();
	spin_unlock_bh(&bond->mode_lock);

	if (update_slave_arr)
		bond_slave_arr_work_rearm(bond, 0);

	if (should_notify_rtnl && rtnl_trylock()) {
		bond_slave_state_notify(bond);
		rtnl_unlock();
	}
	queue_delayed_work(bond->wq, &bond->ad_work, ad_delta_in_ticks);
}

/**
 * bond_3ad_rx_indication - handle a received frame
 * @lacpdu: received lacpdu
 * @slave: slave struct to work on
 *
 * It is assumed that frames that were sent on this NIC don't returned as new
 * received frames (loopback). Since only the payload is given to this
 * function, it check for loopback.
 */
static int bond_3ad_rx_indication(struct lacpdu *lacpdu, struct slave *slave)
{
	struct bonding *bond = slave->bond;
	int ret = RX_HANDLER_ANOTHER;
	struct bond_marker *marker;
	struct port *port;
	atomic64_t *stat;

	port = &(SLAVE_AD_INFO(slave)->port);
	if (!port->slave) {
		net_warn_ratelimited("%s: Warning: port of slave %s is uninitialized\n",
				     slave->dev->name, slave->bond->dev->name);
		return ret;
	}

	switch (lacpdu->subtype) {
	case AD_TYPE_LACPDU:
		ret = RX_HANDLER_CONSUMED;
		slave_dbg(slave->bond->dev, slave->dev,
			  "Received LACPDU on port %d\n",
			  port->actor_port_number);
		/* Protect against concurrent state machines */
		spin_lock(&slave->bond->mode_lock);
		ad_rx_machine(lacpdu, port);
		spin_unlock(&slave->bond->mode_lock);
		break;
	case AD_TYPE_MARKER:
		ret = RX_HANDLER_CONSUMED;
		/* No need to convert fields to Little Endian since we
		 * don't use the marker's fields.
		 */
		marker = (struct bond_marker *)lacpdu;
		switch (marker->tlv_type) {
		case AD_MARKER_INFORMATION_SUBTYPE:
			slave_dbg(slave->bond->dev, slave->dev, "Received Marker Information on port %d\n",
				  port->actor_port_number);
			ad_marker_info_received(marker, port);
			break;
		case AD_MARKER_RESPONSE_SUBTYPE:
			slave_dbg(slave->bond->dev, slave->dev, "Received Marker Response on port %d\n",
				  port->actor_port_number);
			ad_marker_response_received(marker, port);
			break;
		default:
			slave_dbg(slave->bond->dev, slave->dev, "Received an unknown Marker subtype on port %d\n",
				  port->actor_port_number);
			stat = &SLAVE_AD_INFO(slave)->stats.marker_unknown_rx;
			atomic64_inc(stat);
			stat = &BOND_AD_INFO(bond).stats.marker_unknown_rx;
			atomic64_inc(stat);
		}
		break;
	default:
		atomic64_inc(&SLAVE_AD_INFO(slave)->stats.lacpdu_unknown_rx);
		atomic64_inc(&BOND_AD_INFO(bond).stats.lacpdu_unknown_rx);
	}

	return ret;
}

/**
 * ad_update_actor_keys - Update the oper / admin keys for a port based on
 * its current speed and duplex settings.
 *
 * @port: the port we'are looking at
 * @reset: Boolean to just reset the speed and the duplex part of the key
 *
 * The logic to change the oper / admin keys is:
 * (a) A full duplex port can participate in LACP with partner.
 * (b) When the speed is changed, LACP need to be reinitiated.
 */
static void ad_update_actor_keys(struct port *port, bool reset)
{
	u8 duplex = 0;
	u16 ospeed = 0, speed = 0;
	u16 old_oper_key = port->actor_oper_port_key;

	port->actor_admin_port_key &= ~(AD_SPEED_KEY_MASKS|AD_DUPLEX_KEY_MASKS);
	if (!reset) {
		speed = __get_link_speed(port);
		ospeed = (old_oper_key & AD_SPEED_KEY_MASKS) >> 1;
		duplex = __get_duplex(port);
		port->actor_admin_port_key |= (speed << 1) | duplex;
	}
	port->actor_oper_port_key = port->actor_admin_port_key;

	if (old_oper_key != port->actor_oper_port_key) {
		/* Only 'duplex' port participates in LACP */
		if (duplex)
			port->sm_vars |= AD_PORT_LACP_ENABLED;
		else
			port->sm_vars &= ~AD_PORT_LACP_ENABLED;

		if (!reset) {
			if (!speed) {
				slave_err(port->slave->bond->dev,
					  port->slave->dev,
					  "speed changed to 0 on port %d\n",
					  port->actor_port_number);
			} else if (duplex && ospeed != speed) {
				/* Speed change restarts LACP state-machine */
				port->sm_vars |= AD_PORT_BEGIN;
			}
		}
	}
}

/**
 * bond_3ad_adapter_speed_duplex_changed - handle a slave's speed / duplex
 * change indication
 *
 * @slave: slave struct to work on
 *
 * Handle reselection of aggregator (if needed) for this port.
 */
void bond_3ad_adapter_speed_duplex_changed(struct slave *slave)
{
	struct port *port;

	port = &(SLAVE_AD_INFO(slave)->port);

	/* if slave is null, the whole port is not initialized */
	if (!port->slave) {
		slave_warn(slave->bond->dev, slave->dev,
			   "speed/duplex changed for uninitialized port\n");
		return;
	}

	spin_lock_bh(&slave->bond->mode_lock);
	ad_update_actor_keys(port, false);
	spin_unlock_bh(&slave->bond->mode_lock);
	slave_dbg(slave->bond->dev, slave->dev, "Port %d changed speed/duplex\n",
		  port->actor_port_number);
}

/**
 * bond_3ad_handle_link_change - handle a slave's link status change indication
 * @slave: slave struct to work on
 * @link: whether the link is now up or down
 *
 * Handle reselection of aggregator (if needed) for this port.
 */
void bond_3ad_handle_link_change(struct slave *slave, char link)
{
	struct aggregator *agg;
	struct port *port;
	bool dummy;

	port = &(SLAVE_AD_INFO(slave)->port);

	/* if slave is null, the whole port is not initialized */
	if (!port->slave) {
		slave_warn(slave->bond->dev, slave->dev, "link status changed for uninitialized port\n");
		return;
	}

	spin_lock_bh(&slave->bond->mode_lock);
	/* on link down we are zeroing duplex and speed since
	 * some of the adaptors(ce1000.lan) report full duplex/speed
	 * instead of N/A(duplex) / 0(speed).
	 *
	 * on link up we are forcing recheck on the duplex and speed since
	 * some of he adaptors(ce1000.lan) report.
	 */
	if (link == BOND_LINK_UP) {
		port->is_enabled = true;
		ad_update_actor_keys(port, false);
	} else {
		/* link has failed */
		port->is_enabled = false;
		ad_update_actor_keys(port, true);
	}
	agg = __get_first_agg(port);
	ad_agg_selection_logic(agg, &dummy);

	spin_unlock_bh(&slave->bond->mode_lock);

	slave_dbg(slave->bond->dev, slave->dev, "Port %d changed link status to %s\n",
		  port->actor_port_number,
		  link == BOND_LINK_UP ? "UP" : "DOWN");

	/* RTNL is held and mode_lock is released so it's safe
	 * to update slave_array here.
	 */
	bond_update_slave_arr(slave->bond, NULL);
}

/**
 * bond_3ad_set_carrier - set link state for bonding master
 * @bond: bonding structure
 *
 * if we have an active aggregator, we're up, if not, we're down.
 * Presumes that we cannot have an active aggregator if there are
 * no slaves with link up.
 *
 * This behavior complies with IEEE 802.3 section 43.3.9.
 *
 * Called by bond_set_carrier(). Return zero if carrier state does not
 * change, nonzero if it does.
 */
int bond_3ad_set_carrier(struct bonding *bond)
{
	struct aggregator *active;
	struct slave *first_slave;
	int ret = 1;

	rcu_read_lock();
	first_slave = bond_first_slave_rcu(bond);
	if (!first_slave) {
		ret = 0;
		goto out;
	}
	active = __get_active_agg(&(SLAVE_AD_INFO(first_slave)->aggregator));
	if (active) {
		/* are enough slaves available to consider link up? */
		if (__agg_active_ports(active) < bond->params.min_links) {
			if (netif_carrier_ok(bond->dev)) {
				netif_carrier_off(bond->dev);
				goto out;
			}
		} else if (!netif_carrier_ok(bond->dev)) {
			netif_carrier_on(bond->dev);
			goto out;
		}
	} else if (netif_carrier_ok(bond->dev)) {
		netif_carrier_off(bond->dev);
	}
out:
	rcu_read_unlock();
	return ret;
}

/**
 * __bond_3ad_get_active_agg_info - get information of the active aggregator
 * @bond: bonding struct to work on
 * @ad_info: ad_info struct to fill with the bond's info
 *
 * Returns:   0 on success
 *          < 0 on error
 */
int __bond_3ad_get_active_agg_info(struct bonding *bond,
				   struct ad_info *ad_info)
{
	struct aggregator *aggregator = NULL;
	struct list_head *iter;
	struct slave *slave;
	struct port *port;

	bond_for_each_slave_rcu(bond, slave, iter) {
		port = &(SLAVE_AD_INFO(slave)->port);
		if (port->aggregator && port->aggregator->is_active) {
			aggregator = port->aggregator;
			break;
		}
	}

	if (!aggregator)
		return -1;

	ad_info->aggregator_id = aggregator->aggregator_identifier;
	ad_info->ports = __agg_active_ports(aggregator);
	ad_info->actor_key = aggregator->actor_oper_aggregator_key;
	ad_info->partner_key = aggregator->partner_oper_aggregator_key;
	ether_addr_copy(ad_info->partner_system,
			aggregator->partner_system.mac_addr_value);
	return 0;
}

int bond_3ad_get_active_agg_info(struct bonding *bond, struct ad_info *ad_info)
{
	int ret;

	rcu_read_lock();
	ret = __bond_3ad_get_active_agg_info(bond, ad_info);
	rcu_read_unlock();

	return ret;
}

int bond_3ad_lacpdu_recv(const struct sk_buff *skb, struct bonding *bond,
			 struct slave *slave)
{
	struct lacpdu *lacpdu, _lacpdu;

	if (skb->protocol != PKT_TYPE_LACPDU)
		return RX_HANDLER_ANOTHER;

	if (!MAC_ADDRESS_EQUAL(eth_hdr(skb)->h_dest, lacpdu_mcast_addr))
		return RX_HANDLER_ANOTHER;

	lacpdu = skb_header_pointer(skb, 0, sizeof(_lacpdu), &_lacpdu);
	if (!lacpdu) {
		atomic64_inc(&SLAVE_AD_INFO(slave)->stats.lacpdu_illegal_rx);
		atomic64_inc(&BOND_AD_INFO(bond).stats.lacpdu_illegal_rx);
		return RX_HANDLER_ANOTHER;
	}

	return bond_3ad_rx_indication(lacpdu, slave);
}

/**
 * bond_3ad_update_lacp_rate - change the lacp rate
 * @bond: bonding struct
 *
 * When modify lacp_rate parameter via sysfs,
 * update actor_oper_port_state of each port.
 *
 * Hold bond->mode_lock,
 * so we can modify port->actor_oper_port_state,
 * no matter bond is up or down.
 */
void bond_3ad_update_lacp_rate(struct bonding *bond)
{
	struct port *port = NULL;
	struct list_head *iter;
	struct slave *slave;
	int lacp_fast;

	lacp_fast = bond->params.lacp_fast;
	spin_lock_bh(&bond->mode_lock);
	bond_for_each_slave(bond, slave, iter) {
		port = &(SLAVE_AD_INFO(slave)->port);
		if (lacp_fast)
			port->actor_oper_port_state |= LACP_STATE_LACP_TIMEOUT;
		else
			port->actor_oper_port_state &= ~LACP_STATE_LACP_TIMEOUT;
	}
	spin_unlock_bh(&bond->mode_lock);
}

size_t bond_3ad_stats_size(void)
{
	return nla_total_size_64bit(sizeof(u64)) + /* BOND_3AD_STAT_LACPDU_RX */
	       nla_total_size_64bit(sizeof(u64)) + /* BOND_3AD_STAT_LACPDU_TX */
	       nla_total_size_64bit(sizeof(u64)) + /* BOND_3AD_STAT_LACPDU_UNKNOWN_RX */
	       nla_total_size_64bit(sizeof(u64)) + /* BOND_3AD_STAT_LACPDU_ILLEGAL_RX */
	       nla_total_size_64bit(sizeof(u64)) + /* BOND_3AD_STAT_MARKER_RX */
	       nla_total_size_64bit(sizeof(u64)) + /* BOND_3AD_STAT_MARKER_TX */
	       nla_total_size_64bit(sizeof(u64)) + /* BOND_3AD_STAT_MARKER_RESP_RX */
	       nla_total_size_64bit(sizeof(u64)) + /* BOND_3AD_STAT_MARKER_RESP_TX */
	       nla_total_size_64bit(sizeof(u64)); /* BOND_3AD_STAT_MARKER_UNKNOWN_RX */
}

int bond_3ad_stats_fill(struct sk_buff *skb, struct bond_3ad_stats *stats)
{
	u64 val;

	val = atomic64_read(&stats->lacpdu_rx);
	if (nla_put_u64_64bit(skb, BOND_3AD_STAT_LACPDU_RX, val,
			      BOND_3AD_STAT_PAD))
		return -EMSGSIZE;
	val = atomic64_read(&stats->lacpdu_tx);
	if (nla_put_u64_64bit(skb, BOND_3AD_STAT_LACPDU_TX, val,
			      BOND_3AD_STAT_PAD))
		return -EMSGSIZE;
	val = atomic64_read(&stats->lacpdu_unknown_rx);
	if (nla_put_u64_64bit(skb, BOND_3AD_STAT_LACPDU_UNKNOWN_RX, val,
			      BOND_3AD_STAT_PAD))
		return -EMSGSIZE;
	val = atomic64_read(&stats->lacpdu_illegal_rx);
	if (nla_put_u64_64bit(skb, BOND_3AD_STAT_LACPDU_ILLEGAL_RX, val,
			      BOND_3AD_STAT_PAD))
		return -EMSGSIZE;

	val = atomic64_read(&stats->marker_rx);
	if (nla_put_u64_64bit(skb, BOND_3AD_STAT_MARKER_RX, val,
			      BOND_3AD_STAT_PAD))
		return -EMSGSIZE;
	val = atomic64_read(&stats->marker_tx);
	if (nla_put_u64_64bit(skb, BOND_3AD_STAT_MARKER_TX, val,
			      BOND_3AD_STAT_PAD))
		return -EMSGSIZE;
	val = atomic64_read(&stats->marker_resp_rx);
	if (nla_put_u64_64bit(skb, BOND_3AD_STAT_MARKER_RESP_RX, val,
			      BOND_3AD_STAT_PAD))
		return -EMSGSIZE;
	val = atomic64_read(&stats->marker_resp_tx);
	if (nla_put_u64_64bit(skb, BOND_3AD_STAT_MARKER_RESP_TX, val,
			      BOND_3AD_STAT_PAD))
		return -EMSGSIZE;
	val = atomic64_read(&stats->marker_unknown_rx);
	if (nla_put_u64_64bit(skb, BOND_3AD_STAT_MARKER_UNKNOWN_RX, val,
			      BOND_3AD_STAT_PAD))
		return -EMSGSIZE;

	return 0;
}
