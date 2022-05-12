/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * drivers/net/bond/bond_options.h - bonding options
 * Copyright (c) 2013 Nikolay Aleksandrov <nikolay@redhat.com>
 */

#ifndef _NET_BOND_OPTIONS_H
#define _NET_BOND_OPTIONS_H

#define BOND_OPT_MAX_NAMELEN 32
#define BOND_OPT_VALID(opt) ((opt) < BOND_OPT_LAST)
#define BOND_MODE_ALL_EX(x) (~(x))

/* Option flags:
 * BOND_OPTFLAG_NOSLAVES - check if the bond device is empty before setting
 * BOND_OPTFLAG_IFDOWN - check if the bond device is down before setting
 * BOND_OPTFLAG_RAWVAL - the option parses the value itself
 */
enum {
	BOND_OPTFLAG_NOSLAVES	= BIT(0),
	BOND_OPTFLAG_IFDOWN	= BIT(1),
	BOND_OPTFLAG_RAWVAL	= BIT(2)
};

/* Value type flags:
 * BOND_VALFLAG_DEFAULT - mark the value as default
 * BOND_VALFLAG_(MIN|MAX) - mark the value as min/max
 */
enum {
	BOND_VALFLAG_DEFAULT	= BIT(0),
	BOND_VALFLAG_MIN	= BIT(1),
	BOND_VALFLAG_MAX	= BIT(2)
};

/* Option IDs, their bit positions correspond to their IDs */
enum {
	BOND_OPT_MODE,
	BOND_OPT_PACKETS_PER_SLAVE,
	BOND_OPT_XMIT_HASH,
	BOND_OPT_ARP_VALIDATE,
	BOND_OPT_ARP_ALL_TARGETS,
	BOND_OPT_FAIL_OVER_MAC,
	BOND_OPT_ARP_INTERVAL,
	BOND_OPT_ARP_TARGETS,
	BOND_OPT_DOWNDELAY,
	BOND_OPT_UPDELAY,
	BOND_OPT_LACP_RATE,
	BOND_OPT_MINLINKS,
	BOND_OPT_AD_SELECT,
	BOND_OPT_NUM_PEER_NOTIF,
	BOND_OPT_MIIMON,
	BOND_OPT_PRIMARY,
	BOND_OPT_PRIMARY_RESELECT,
	BOND_OPT_USE_CARRIER,
	BOND_OPT_ACTIVE_SLAVE,
	BOND_OPT_QUEUE_ID,
	BOND_OPT_ALL_SLAVES_ACTIVE,
	BOND_OPT_RESEND_IGMP,
	BOND_OPT_LP_INTERVAL,
	BOND_OPT_SLAVES,
	BOND_OPT_TLB_DYNAMIC_LB,
	BOND_OPT_AD_ACTOR_SYS_PRIO,
	BOND_OPT_AD_ACTOR_SYSTEM,
	BOND_OPT_AD_USER_PORT_KEY,
	BOND_OPT_NUM_PEER_NOTIF_ALIAS,
	BOND_OPT_PEER_NOTIF_DELAY,
	BOND_OPT_LAST
};

/* This structure is used for storing option values and for passing option
 * values when changing an option. The logic when used as an arg is as follows:
 * - if string != NULL -> parse it, if the opt is RAW type then return it, else
 *   return the parse result
 * - if string == NULL -> parse value
 */
struct bond_opt_value {
	char *string;
	u64 value;
	u32 flags;
};

struct bonding;

struct bond_option {
	int id;
	const char *name;
	const char *desc;
	u32 flags;

	/* unsuppmodes is used to denote modes in which the option isn't
	 * supported.
	 */
	unsigned long unsuppmodes;
	/* supported values which this option can have, can be a subset of
	 * BOND_OPTVAL_RANGE's value range
	 */
	const struct bond_opt_value *values;

	int (*set)(struct bonding *bond, const struct bond_opt_value *val);
};

int __bond_opt_set(struct bonding *bond, unsigned int option,
		   struct bond_opt_value *val);
int __bond_opt_set_notify(struct bonding *bond, unsigned int option,
			  struct bond_opt_value *val);
int bond_opt_tryset_rtnl(struct bonding *bond, unsigned int option, char *buf);

const struct bond_opt_value *bond_opt_parse(const struct bond_option *opt,
					    struct bond_opt_value *val);
const struct bond_option *bond_opt_get(unsigned int option);
const struct bond_option *bond_opt_get_by_name(const char *name);
const struct bond_opt_value *bond_opt_get_val(unsigned int option, u64 val);

/* This helper is used to initialize a bond_opt_value structure for parameter
 * passing. There should be either a valid string or value, but not both.
 * When value is ULLONG_MAX then string will be used.
 */
static inline void __bond_opt_init(struct bond_opt_value *optval,
				   char *string, u64 value)
{
	memset(optval, 0, sizeof(*optval));
	optval->value = ULLONG_MAX;
	if (value == ULLONG_MAX)
		optval->string = string;
	else
		optval->value = value;
}
#define bond_opt_initval(optval, value) __bond_opt_init(optval, NULL, value)
#define bond_opt_initstr(optval, str) __bond_opt_init(optval, str, ULLONG_MAX)

void bond_option_arp_ip_targets_clear(struct bonding *bond);

#endif /* _NET_BOND_OPTIONS_H */
