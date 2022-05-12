// SPDX-License-Identifier: GPL-2.0+
// proteus-2309.h - Keytable for proteus_2309 Remote Controller
//
// keymap imported from ir-keymaps.c
//
// Copyright (c) 2010 by Mauro Carvalho Chehab

#include <media/rc-map.h>
#include <linux/module.h>

/* Michal Majchrowicz <mmajchrowicz@gmail.com> */

static struct rc_map_table proteus_2309[] = {
	/* numeric */
	{ 0x00, KEY_NUMERIC_0 },
	{ 0x01, KEY_NUMERIC_1 },
	{ 0x02, KEY_NUMERIC_2 },
	{ 0x03, KEY_NUMERIC_3 },
	{ 0x04, KEY_NUMERIC_4 },
	{ 0x05, KEY_NUMERIC_5 },
	{ 0x06, KEY_NUMERIC_6 },
	{ 0x07, KEY_NUMERIC_7 },
	{ 0x08, KEY_NUMERIC_8 },
	{ 0x09, KEY_NUMERIC_9 },

	{ 0x5c, KEY_POWER },		/* power       */
	{ 0x20, KEY_ZOOM },		/* full screen */
	{ 0x0f, KEY_BACKSPACE },	/* recall      */
	{ 0x1b, KEY_ENTER },		/* mute        */
	{ 0x41, KEY_RECORD },		/* record      */
	{ 0x43, KEY_STOP },		/* stop        */
	{ 0x16, KEY_S },
	{ 0x1a, KEY_POWER2 },		/* off         */
	{ 0x2e, KEY_RED },
	{ 0x1f, KEY_CHANNELDOWN },	/* channel -   */
	{ 0x1c, KEY_CHANNELUP },	/* channel +   */
	{ 0x10, KEY_VOLUMEDOWN },	/* volume -    */
	{ 0x1e, KEY_VOLUMEUP },		/* volume +    */
	{ 0x14, KEY_F1 },
};

static struct rc_map_list proteus_2309_map = {
	.map = {
		.scan     = proteus_2309,
		.size     = ARRAY_SIZE(proteus_2309),
		.rc_proto = RC_PROTO_UNKNOWN,	/* Legacy IR type */
		.name     = RC_MAP_PROTEUS_2309,
	}
};

static int __init init_rc_map_proteus_2309(void)
{
	return rc_map_register(&proteus_2309_map);
}

static void __exit exit_rc_map_proteus_2309(void)
{
	rc_map_unregister(&proteus_2309_map);
}

module_init(init_rc_map_proteus_2309)
module_exit(exit_rc_map_proteus_2309)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mauro Carvalho Chehab");
