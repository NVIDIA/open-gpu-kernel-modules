// SPDX-License-Identifier: GPL-2.0+
// avermedia-dvbt.h - Keytable for avermedia_dvbt Remote Controller
//
// keymap imported from ir-keymaps.c
//
// Copyright (c) 2010 by Mauro Carvalho Chehab

#include <media/rc-map.h>
#include <linux/module.h>

/* Matt Jesson <dvb@jesson.eclipse.co.uk */

static struct rc_map_table avermedia_dvbt[] = {
	{ 0x28, KEY_NUMERIC_0 },	/* '0' / 'enter' */
	{ 0x22, KEY_NUMERIC_1 },	/* '1' */
	{ 0x12, KEY_NUMERIC_2 },	/* '2' / 'up arrow' */
	{ 0x32, KEY_NUMERIC_3 },	/* '3' */
	{ 0x24, KEY_NUMERIC_4 },	/* '4' / 'left arrow' */
	{ 0x14, KEY_NUMERIC_5 },	/* '5' */
	{ 0x34, KEY_NUMERIC_6 },	/* '6' / 'right arrow' */
	{ 0x26, KEY_NUMERIC_7 },	/* '7' */
	{ 0x16, KEY_NUMERIC_8 },	/* '8' / 'down arrow' */
	{ 0x36, KEY_NUMERIC_9 },	/* '9' */

	{ 0x20, KEY_VIDEO },		/* 'source' */
	{ 0x10, KEY_TEXT },		/* 'teletext' */
	{ 0x00, KEY_POWER },		/* 'power' */
	{ 0x04, KEY_AUDIO },		/* 'audio' */
	{ 0x06, KEY_ZOOM },		/* 'full screen' */
	{ 0x18, KEY_SWITCHVIDEOMODE },	/* 'display' */
	{ 0x38, KEY_SEARCH },		/* 'loop' */
	{ 0x08, KEY_INFO },		/* 'preview' */
	{ 0x2a, KEY_REWIND },		/* 'backward <<' */
	{ 0x1a, KEY_FASTFORWARD },	/* 'forward >>' */
	{ 0x3a, KEY_RECORD },		/* 'capture' */
	{ 0x0a, KEY_MUTE },		/* 'mute' */
	{ 0x2c, KEY_RECORD },		/* 'record' */
	{ 0x1c, KEY_PAUSE },		/* 'pause' */
	{ 0x3c, KEY_STOP },		/* 'stop' */
	{ 0x0c, KEY_PLAY },		/* 'play' */
	{ 0x2e, KEY_RED },		/* 'red' */
	{ 0x01, KEY_BLUE },		/* 'blue' / 'cancel' */
	{ 0x0e, KEY_YELLOW },		/* 'yellow' / 'ok' */
	{ 0x21, KEY_GREEN },		/* 'green' */
	{ 0x11, KEY_CHANNELDOWN },	/* 'channel -' */
	{ 0x31, KEY_CHANNELUP },	/* 'channel +' */
	{ 0x1e, KEY_VOLUMEDOWN },	/* 'volume -' */
	{ 0x3e, KEY_VOLUMEUP },		/* 'volume +' */
};

static struct rc_map_list avermedia_dvbt_map = {
	.map = {
		.scan     = avermedia_dvbt,
		.size     = ARRAY_SIZE(avermedia_dvbt),
		.rc_proto = RC_PROTO_UNKNOWN,	/* Legacy IR type */
		.name     = RC_MAP_AVERMEDIA_DVBT,
	}
};

static int __init init_rc_map_avermedia_dvbt(void)
{
	return rc_map_register(&avermedia_dvbt_map);
}

static void __exit exit_rc_map_avermedia_dvbt(void)
{
	rc_map_unregister(&avermedia_dvbt_map);
}

module_init(init_rc_map_avermedia_dvbt)
module_exit(exit_rc_map_avermedia_dvbt)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mauro Carvalho Chehab");
