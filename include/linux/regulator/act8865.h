/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * act8865.h  --  Voltage regulation for active-semi act88xx PMUs
 *
 * Copyright (C) 2013 Atmel Corporation.
 */

#ifndef __LINUX_REGULATOR_ACT8865_H
#define __LINUX_REGULATOR_ACT8865_H

#include <linux/regulator/machine.h>

enum {
	ACT8600_ID_DCDC1,
	ACT8600_ID_DCDC2,
	ACT8600_ID_DCDC3,
	ACT8600_ID_SUDCDC4,
	ACT8600_ID_LDO5,
	ACT8600_ID_LDO6,
	ACT8600_ID_LDO7,
	ACT8600_ID_LDO8,
	ACT8600_ID_LDO9,
	ACT8600_ID_LDO10,
};

enum {
	ACT8865_ID_DCDC1,
	ACT8865_ID_DCDC2,
	ACT8865_ID_DCDC3,
	ACT8865_ID_LDO1,
	ACT8865_ID_LDO2,
	ACT8865_ID_LDO3,
	ACT8865_ID_LDO4,
	ACT8865_REG_NUM,
};

enum {
	ACT8846_ID_REG1,
	ACT8846_ID_REG2,
	ACT8846_ID_REG3,
	ACT8846_ID_REG4,
	ACT8846_ID_REG5,
	ACT8846_ID_REG6,
	ACT8846_ID_REG7,
	ACT8846_ID_REG8,
	ACT8846_ID_REG9,
	ACT8846_ID_REG10,
	ACT8846_ID_REG11,
	ACT8846_ID_REG12,
	ACT8846_REG_NUM,
};

enum {
	ACT8600,
	ACT8865,
	ACT8846,
};

/**
 * act8865_regulator_data - regulator data
 * @id: regulator id
 * @name: regulator name
 * @init_data: regulator init data
 * @of_node: device tree node (optional)
 */
struct act8865_regulator_data {
	int id;
	const char *name;
	struct regulator_init_data *init_data;
	struct device_node *of_node;
};

/**
 * act8865_platform_data - platform data for act8865
 * @num_regulators: number of regulators used
 * @regulators: pointer to regulators used
 */
struct act8865_platform_data {
	int num_regulators;
	struct act8865_regulator_data *regulators;
};
#endif
