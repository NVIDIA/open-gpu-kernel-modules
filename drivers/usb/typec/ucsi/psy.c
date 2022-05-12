// SPDX-License-Identifier: GPL-2.0
/*
 * Power Supply for UCSI
 *
 * Copyright (C) 2020, Intel Corporation
 * Author: K V, Abhilash <abhilash.k.v@intel.com>
 * Author: Heikki Krogerus <heikki.krogerus@linux.intel.com>
 */

#include <linux/property.h>
#include <linux/usb/pd.h>

#include "ucsi.h"

/* Power Supply access to expose source power information */
enum ucsi_psy_online_states {
	UCSI_PSY_OFFLINE = 0,
	UCSI_PSY_FIXED_ONLINE,
	UCSI_PSY_PROG_ONLINE,
};

static enum power_supply_property ucsi_psy_props[] = {
	POWER_SUPPLY_PROP_USB_TYPE,
	POWER_SUPPLY_PROP_ONLINE,
	POWER_SUPPLY_PROP_VOLTAGE_MIN,
	POWER_SUPPLY_PROP_VOLTAGE_MAX,
	POWER_SUPPLY_PROP_VOLTAGE_NOW,
	POWER_SUPPLY_PROP_CURRENT_MAX,
	POWER_SUPPLY_PROP_CURRENT_NOW,
};

static int ucsi_psy_get_online(struct ucsi_connector *con,
			       union power_supply_propval *val)
{
	val->intval = UCSI_PSY_OFFLINE;
	if (con->status.flags & UCSI_CONSTAT_CONNECTED &&
	    (con->status.flags & UCSI_CONSTAT_PWR_DIR) == TYPEC_SINK)
		val->intval = UCSI_PSY_FIXED_ONLINE;
	return 0;
}

static int ucsi_psy_get_voltage_min(struct ucsi_connector *con,
				    union power_supply_propval *val)
{
	u32 pdo;

	switch (UCSI_CONSTAT_PWR_OPMODE(con->status.flags)) {
	case UCSI_CONSTAT_PWR_OPMODE_PD:
		pdo = con->src_pdos[0];
		val->intval = pdo_fixed_voltage(pdo) * 1000;
		break;
	case UCSI_CONSTAT_PWR_OPMODE_TYPEC3_0:
	case UCSI_CONSTAT_PWR_OPMODE_TYPEC1_5:
	case UCSI_CONSTAT_PWR_OPMODE_BC:
	case UCSI_CONSTAT_PWR_OPMODE_DEFAULT:
		val->intval = UCSI_TYPEC_VSAFE5V * 1000;
		break;
	default:
		val->intval = 0;
		break;
	}
	return 0;
}

static int ucsi_psy_get_voltage_max(struct ucsi_connector *con,
				    union power_supply_propval *val)
{
	u32 pdo;

	switch (UCSI_CONSTAT_PWR_OPMODE(con->status.flags)) {
	case UCSI_CONSTAT_PWR_OPMODE_PD:
		if (con->num_pdos > 0) {
			pdo = con->src_pdos[con->num_pdos - 1];
			val->intval = pdo_fixed_voltage(pdo) * 1000;
		} else {
			val->intval = 0;
		}
		break;
	case UCSI_CONSTAT_PWR_OPMODE_TYPEC3_0:
	case UCSI_CONSTAT_PWR_OPMODE_TYPEC1_5:
	case UCSI_CONSTAT_PWR_OPMODE_BC:
	case UCSI_CONSTAT_PWR_OPMODE_DEFAULT:
		val->intval = UCSI_TYPEC_VSAFE5V * 1000;
		break;
	default:
		val->intval = 0;
		break;
	}
	return 0;
}

static int ucsi_psy_get_voltage_now(struct ucsi_connector *con,
				    union power_supply_propval *val)
{
	int index;
	u32 pdo;

	switch (UCSI_CONSTAT_PWR_OPMODE(con->status.flags)) {
	case UCSI_CONSTAT_PWR_OPMODE_PD:
		index = rdo_index(con->rdo);
		if (index > 0) {
			pdo = con->src_pdos[index - 1];
			val->intval = pdo_fixed_voltage(pdo) * 1000;
		} else {
			val->intval = 0;
		}
		break;
	case UCSI_CONSTAT_PWR_OPMODE_TYPEC3_0:
	case UCSI_CONSTAT_PWR_OPMODE_TYPEC1_5:
	case UCSI_CONSTAT_PWR_OPMODE_BC:
	case UCSI_CONSTAT_PWR_OPMODE_DEFAULT:
		val->intval = UCSI_TYPEC_VSAFE5V * 1000;
		break;
	default:
		val->intval = 0;
		break;
	}
	return 0;
}

static int ucsi_psy_get_current_max(struct ucsi_connector *con,
				    union power_supply_propval *val)
{
	u32 pdo;

	switch (UCSI_CONSTAT_PWR_OPMODE(con->status.flags)) {
	case UCSI_CONSTAT_PWR_OPMODE_PD:
		if (con->num_pdos > 0) {
			pdo = con->src_pdos[con->num_pdos - 1];
			val->intval = pdo_max_current(pdo) * 1000;
		} else {
			val->intval = 0;
		}
		break;
	case UCSI_CONSTAT_PWR_OPMODE_TYPEC1_5:
		val->intval = UCSI_TYPEC_1_5_CURRENT * 1000;
		break;
	case UCSI_CONSTAT_PWR_OPMODE_TYPEC3_0:
		val->intval = UCSI_TYPEC_3_0_CURRENT * 1000;
		break;
	case UCSI_CONSTAT_PWR_OPMODE_BC:
	case UCSI_CONSTAT_PWR_OPMODE_DEFAULT:
	/* UCSI can't tell b/w DCP/CDP or USB2/3x1/3x2 SDP chargers */
	default:
		val->intval = 0;
		break;
	}
	return 0;
}

static int ucsi_psy_get_current_now(struct ucsi_connector *con,
				    union power_supply_propval *val)
{
	u16 flags = con->status.flags;

	if (UCSI_CONSTAT_PWR_OPMODE(flags) == UCSI_CONSTAT_PWR_OPMODE_PD)
		val->intval = rdo_op_current(con->rdo) * 1000;
	else
		val->intval = 0;
	return 0;
}

static int ucsi_psy_get_usb_type(struct ucsi_connector *con,
				 union power_supply_propval *val)
{
	u16 flags = con->status.flags;

	val->intval = POWER_SUPPLY_USB_TYPE_C;
	if (flags & UCSI_CONSTAT_CONNECTED &&
	    UCSI_CONSTAT_PWR_OPMODE(flags) == UCSI_CONSTAT_PWR_OPMODE_PD)
		val->intval = POWER_SUPPLY_USB_TYPE_PD;

	return 0;
}

static int ucsi_psy_get_prop(struct power_supply *psy,
			     enum power_supply_property psp,
			     union power_supply_propval *val)
{
	struct ucsi_connector *con = power_supply_get_drvdata(psy);

	switch (psp) {
	case POWER_SUPPLY_PROP_USB_TYPE:
		return ucsi_psy_get_usb_type(con, val);
	case POWER_SUPPLY_PROP_ONLINE:
		return ucsi_psy_get_online(con, val);
	case POWER_SUPPLY_PROP_VOLTAGE_MIN:
		return ucsi_psy_get_voltage_min(con, val);
	case POWER_SUPPLY_PROP_VOLTAGE_MAX:
		return ucsi_psy_get_voltage_max(con, val);
	case POWER_SUPPLY_PROP_VOLTAGE_NOW:
		return ucsi_psy_get_voltage_now(con, val);
	case POWER_SUPPLY_PROP_CURRENT_MAX:
		return ucsi_psy_get_current_max(con, val);
	case POWER_SUPPLY_PROP_CURRENT_NOW:
		return ucsi_psy_get_current_now(con, val);
	default:
		return -EINVAL;
	}
}

static enum power_supply_usb_type ucsi_psy_usb_types[] = {
	POWER_SUPPLY_USB_TYPE_C,
	POWER_SUPPLY_USB_TYPE_PD,
	POWER_SUPPLY_USB_TYPE_PD_PPS,
};

int ucsi_register_port_psy(struct ucsi_connector *con)
{
	struct power_supply_config psy_cfg = {};
	struct device *dev = con->ucsi->dev;
	char *psy_name;

	psy_cfg.drv_data = con;
	psy_cfg.fwnode = dev_fwnode(dev);

	psy_name = devm_kasprintf(dev, GFP_KERNEL, "ucsi-source-psy-%s%d",
				  dev_name(dev), con->num);
	if (!psy_name)
		return -ENOMEM;

	con->psy_desc.name = psy_name;
	con->psy_desc.type = POWER_SUPPLY_TYPE_USB;
	con->psy_desc.usb_types = ucsi_psy_usb_types;
	con->psy_desc.num_usb_types = ARRAY_SIZE(ucsi_psy_usb_types);
	con->psy_desc.properties = ucsi_psy_props;
	con->psy_desc.num_properties = ARRAY_SIZE(ucsi_psy_props);
	con->psy_desc.get_property = ucsi_psy_get_prop;

	con->psy = power_supply_register(dev, &con->psy_desc, &psy_cfg);

	return PTR_ERR_OR_ZERO(con->psy);
}

void ucsi_unregister_port_psy(struct ucsi_connector *con)
{
	if (IS_ERR_OR_NULL(con->psy))
		return;

	power_supply_unregister(con->psy);
	con->psy = NULL;
}

void ucsi_port_psy_changed(struct ucsi_connector *con)
{
	if (IS_ERR_OR_NULL(con->psy))
		return;

	power_supply_changed(con->psy);
}
