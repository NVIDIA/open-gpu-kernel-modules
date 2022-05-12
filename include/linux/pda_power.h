/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Common power driver for PDAs and phones with one or two external
 * power supplies (AC/USB) connected to main and backup batteries,
 * and optional builtin charger.
 *
 * Copyright © 2007 Anton Vorontsov <cbou@mail.ru>
 */

#ifndef __PDA_POWER_H__
#define __PDA_POWER_H__

#define PDA_POWER_CHARGE_AC  (1 << 0)
#define PDA_POWER_CHARGE_USB (1 << 1)

struct device;

struct pda_power_pdata {
	int (*init)(struct device *dev);
	int (*is_ac_online)(void);
	int (*is_usb_online)(void);
	void (*set_charge)(int flags);
	void (*exit)(struct device *dev);
	int (*suspend)(pm_message_t state);
	int (*resume)(void);

	char **supplied_to;
	size_t num_supplicants;

	unsigned int wait_for_status; /* msecs, default is 500 */
	unsigned int wait_for_charger; /* msecs, default is 500 */
	unsigned int polling_interval; /* msecs, default is 2000 */

	unsigned long ac_max_uA; /* current to draw when on AC */

	bool use_otg_notifier;
};

#endif /* __PDA_POWER_H__ */
