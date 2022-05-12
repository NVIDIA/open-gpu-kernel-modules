/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Common Definitions for Janz MODULbus devices
 *
 * Copyright (c) 2010 Ira W. Snyder <iws@ovro.caltech.edu>
 */

#ifndef JANZ_H
#define JANZ_H

struct janz_platform_data {
	/* MODULbus Module Number */
	unsigned int modno;
};

/* PLX bridge chip onboard registers */
struct janz_cmodio_onboard_regs {
	u8 unused1;

	/*
	 * Read access: interrupt status
	 * Write access: interrupt disable
	 */
	u8 int_disable;
	u8 unused2;

	/*
	 * Read access: MODULbus number (hex switch)
	 * Write access: interrupt enable
	 */
	u8 int_enable;
	u8 unused3;

	/* write-only */
	u8 reset_assert;
	u8 unused4;

	/* write-only */
	u8 reset_deassert;
	u8 unused5;

	/* read-write access to serial EEPROM */
	u8 eep;
	u8 unused6;

	/* write-only access to EEPROM chip select */
	u8 enid;
};

#endif /* JANZ_H */
