// SPDX-License-Identifier: GPL-2.0-or-later
/* rc-technisat-ts35.c - Keytable for TechniSat TS35 remote
 *
 * Copyright (c) 2013 by Jan Klötzke <jan@kloetzke.net>
 */

#include <media/rc-map.h>
#include <linux/module.h>

static struct rc_map_table technisat_ts35[] = {
	{0x32, KEY_MUTE},
	{0x07, KEY_MEDIA},
	{0x1c, KEY_AB},
	{0x33, KEY_POWER},

	{0x3e, KEY_NUMERIC_1},
	{0x3d, KEY_NUMERIC_2},
	{0x3c, KEY_NUMERIC_3},
	{0x3b, KEY_NUMERIC_4},
	{0x3a, KEY_NUMERIC_5},
	{0x39, KEY_NUMERIC_6},
	{0x38, KEY_NUMERIC_7},
	{0x37, KEY_NUMERIC_8},
	{0x36, KEY_NUMERIC_9},
	{0x3f, KEY_NUMERIC_0},
	{0x35, KEY_DIGITS},
	{0x2c, KEY_TV},

	{0x20, KEY_INFO},
	{0x2d, KEY_MENU},
	{0x1f, KEY_UP},
	{0x1e, KEY_DOWN},
	{0x2e, KEY_LEFT},
	{0x2f, KEY_RIGHT},
	{0x28, KEY_OK},
	{0x10, KEY_EPG},
	{0x1d, KEY_BACK},

	{0x14, KEY_RED},
	{0x13, KEY_GREEN},
	{0x12, KEY_YELLOW},
	{0x11, KEY_BLUE},

	{0x09, KEY_SELECT},
	{0x03, KEY_TEXT},
	{0x16, KEY_STOP},
	{0x30, KEY_HELP},
};

static struct rc_map_list technisat_ts35_map = {
	.map = {
		.scan     = technisat_ts35,
		.size     = ARRAY_SIZE(technisat_ts35),
		.rc_proto = RC_PROTO_UNKNOWN,
		.name     = RC_MAP_TECHNISAT_TS35,
	}
};

static int __init init_rc_map(void)
{
	return rc_map_register(&technisat_ts35_map);
}

static void __exit exit_rc_map(void)
{
	rc_map_unregister(&technisat_ts35_map);
}

module_init(init_rc_map)
module_exit(exit_rc_map)

MODULE_LICENSE("GPL");
