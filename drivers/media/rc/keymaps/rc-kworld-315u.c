// SPDX-License-Identifier: GPL-2.0+
// kworld-315u.h - Keytable for kworld_315u Remote Controller
//
// keymap imported from ir-keymaps.c
//
// Copyright (c) 2010 by Mauro Carvalho Chehab

#include <media/rc-map.h>
#include <linux/module.h>

/* Kworld 315U
 */

static struct rc_map_table kworld_315u[] = {
	{ 0x6143, KEY_POWER },
	{ 0x6101, KEY_VIDEO },		/* source */
	{ 0x610b, KEY_ZOOM },
	{ 0x6103, KEY_POWER2 },		/* shutdown */

	{ 0x6104, KEY_NUMERIC_1 },
	{ 0x6108, KEY_NUMERIC_2 },
	{ 0x6102, KEY_NUMERIC_3 },
	{ 0x6109, KEY_CHANNELUP },

	{ 0x610f, KEY_NUMERIC_4 },
	{ 0x6105, KEY_NUMERIC_5 },
	{ 0x6106, KEY_NUMERIC_6 },
	{ 0x6107, KEY_CHANNELDOWN },

	{ 0x610c, KEY_NUMERIC_7 },
	{ 0x610d, KEY_NUMERIC_8 },
	{ 0x610a, KEY_NUMERIC_9 },
	{ 0x610e, KEY_VOLUMEUP },

	{ 0x6110, KEY_LAST },
	{ 0x6111, KEY_NUMERIC_0 },
	{ 0x6112, KEY_ENTER },
	{ 0x6113, KEY_VOLUMEDOWN },

	{ 0x6114, KEY_RECORD },
	{ 0x6115, KEY_STOP },
	{ 0x6116, KEY_PLAY },
	{ 0x6117, KEY_MUTE },

	{ 0x6118, KEY_UP },
	{ 0x6119, KEY_DOWN },
	{ 0x611a, KEY_LEFT },
	{ 0x611b, KEY_RIGHT },

	{ 0x611c, KEY_RED },
	{ 0x611d, KEY_GREEN },
	{ 0x611e, KEY_YELLOW },
	{ 0x611f, KEY_BLUE },
};

static struct rc_map_list kworld_315u_map = {
	.map = {
		.scan     = kworld_315u,
		.size     = ARRAY_SIZE(kworld_315u),
		.rc_proto = RC_PROTO_NEC,
		.name     = RC_MAP_KWORLD_315U,
	}
};

static int __init init_rc_map_kworld_315u(void)
{
	return rc_map_register(&kworld_315u_map);
}

static void __exit exit_rc_map_kworld_315u(void)
{
	rc_map_unregister(&kworld_315u_map);
}

module_init(init_rc_map_kworld_315u)
module_exit(exit_rc_map_kworld_315u)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mauro Carvalho Chehab");
