/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Infineon TUA9001 silicon tuner driver
 *
 * Copyright (C) 2009 Antti Palosaari <crope@iki.fi>
 */

#ifndef TUA9001_H
#define TUA9001_H

#include <media/dvb_frontend.h>

/*
 * I2C address
 * 0x60,
 */

/**
 * struct tua9001_platform_data - Platform data for the tua9001 driver
 * @dvb_frontend: DVB frontend.
 */
struct tua9001_platform_data {
	struct dvb_frontend *dvb_frontend;
};

/*
 * TUA9001 I/O PINs:
 *
 * CEN - chip enable
 * 0 = chip disabled (chip off)
 * 1 = chip enabled (chip on)
 *
 * RESETN - chip reset
 * 0 = reset disabled (chip reset off)
 * 1 = reset enabled (chip reset on)
 *
 * RXEN - RX enable
 * 0 = RX disabled (chip idle)
 * 1 = RX enabled (chip tuned)
 */

#define TUA9001_CMD_CEN     0
#define TUA9001_CMD_RESETN  1
#define TUA9001_CMD_RXEN    2

#endif
