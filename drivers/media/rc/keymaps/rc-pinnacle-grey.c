// SPDX-License-Identifier: GPL-2.0+
// pinnacle-grey.h - Keytable for pinnacle_grey Remote Controller
//
// keymap imported from ir-keymaps.c
//
// Copyright (c) 2010 by Mauro Carvalho Chehab

#include <media/rc-map.h>
#include <linux/module.h>

static struct rc_map_table pinnacle_grey[] = {
	{ 0x3a, KEY_NUMERIC_0 },
	{ 0x31, KEY_NUMERIC_1 },
	{ 0x32, KEY_NUMERIC_2 },
	{ 0x33, KEY_NUMERIC_3 },
	{ 0x34, KEY_NUMERIC_4 },
	{ 0x35, KEY_NUMERIC_5 },
	{ 0x36, KEY_NUMERIC_6 },
	{ 0x37, KEY_NUMERIC_7 },
	{ 0x38, KEY_NUMERIC_8 },
	{ 0x39, KEY_NUMERIC_9 },

	{ 0x2f, KEY_POWER },

	{ 0x2e, KEY_P },
	{ 0x1f, KEY_L },
	{ 0x2b, KEY_I },

	{ 0x2d, KEY_SCREEN },
	{ 0x1e, KEY_ZOOM },
	{ 0x1b, KEY_VOLUMEUP },
	{ 0x0f, KEY_VOLUMEDOWN },
	{ 0x17, KEY_CHANNELUP },
	{ 0x1c, KEY_CHANNELDOWN },
	{ 0x25, KEY_INFO },

	{ 0x3c, KEY_MUTE },

	{ 0x3d, KEY_LEFT },
	{ 0x3b, KEY_RIGHT },

	{ 0x3f, KEY_UP },
	{ 0x3e, KEY_DOWN },
	{ 0x1a, KEY_ENTER },

	{ 0x1d, KEY_MENU },
	{ 0x19, KEY_AGAIN },
	{ 0x16, KEY_PREVIOUSSONG },
	{ 0x13, KEY_NEXTSONG },
	{ 0x15, KEY_PAUSE },
	{ 0x0e, KEY_REWIND },
	{ 0x0d, KEY_PLAY },
	{ 0x0b, KEY_STOP },
	{ 0x07, KEY_FORWARD },
	{ 0x27, KEY_RECORD },
	{ 0x26, KEY_TUNER },
	{ 0x29, KEY_TEXT },
	{ 0x2a, KEY_MEDIA },
	{ 0x18, KEY_EPG },
};

static struct rc_map_list pinnacle_grey_map = {
	.map = {
		.scan     = pinnacle_grey,
		.size     = ARRAY_SIZE(pinnacle_grey),
		.rc_proto = RC_PROTO_UNKNOWN,	/* Legacy IR type */
		.name     = RC_MAP_PINNACLE_GREY,
	}
};

static int __init init_rc_map_pinnacle_grey(void)
{
	return rc_map_register(&pinnacle_grey_map);
}

static void __exit exit_rc_map_pinnacle_grey(void)
{
	rc_map_unregister(&pinnacle_grey_map);
}

module_init(init_rc_map_pinnacle_grey)
module_exit(exit_rc_map_pinnacle_grey)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mauro Carvalho Chehab");
