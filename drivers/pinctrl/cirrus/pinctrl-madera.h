/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Pinctrl for Cirrus Logic Madera codecs
 *
 * Copyright (C) 2016-2017 Cirrus Logic
 */

#ifndef PINCTRL_MADERA_H
#define PINCTRL_MADERA_H

struct madera_pin_groups {
	const char *name;
	const unsigned int *pins;
	unsigned int n_pins;
};

struct madera_pin_chip {
	unsigned int n_pins;

	const struct madera_pin_groups *pin_groups;
	unsigned int n_pin_groups;
};

struct madera_pin_private {
	struct madera *madera;

	const struct madera_pin_chip *chip; /* chip-specific groups */

	struct device *dev;
	struct pinctrl_dev *pctl;
};

extern const struct madera_pin_chip cs47l15_pin_chip;
extern const struct madera_pin_chip cs47l35_pin_chip;
extern const struct madera_pin_chip cs47l85_pin_chip;
extern const struct madera_pin_chip cs47l90_pin_chip;
extern const struct madera_pin_chip cs47l92_pin_chip;

#endif
