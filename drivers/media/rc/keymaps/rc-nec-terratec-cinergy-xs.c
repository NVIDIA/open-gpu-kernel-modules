// SPDX-License-Identifier: GPL-2.0+
// nec-terratec-cinergy-xs.h - Keytable for nec_terratec_cinergy_xs Remote Controller
//
// keymap imported from ir-keymaps.c
//
// Copyright (c) 2010 by Mauro Carvalho Chehab

#include <media/rc-map.h>
#include <linux/module.h>

/* Terratec Cinergy Hybrid T USB XS FM
   Mauro Carvalho Chehab
 */

static struct rc_map_table nec_terratec_cinergy_xs[] = {

	/* Terratec Grey IR, with most keys in orange */
	{ 0x1441, KEY_HOME},
	{ 0x1401, KEY_POWER2},

	{ 0x1442, KEY_MENU},		/* DVD menu */
	{ 0x1443, KEY_SUBTITLE},
	{ 0x1444, KEY_TEXT},		/* Teletext */
	{ 0x1445, KEY_DELETE},

	{ 0x1402, KEY_NUMERIC_1},
	{ 0x1403, KEY_NUMERIC_2},
	{ 0x1404, KEY_NUMERIC_3},
	{ 0x1405, KEY_NUMERIC_4},
	{ 0x1406, KEY_NUMERIC_5},
	{ 0x1407, KEY_NUMERIC_6},
	{ 0x1408, KEY_NUMERIC_7},
	{ 0x1409, KEY_NUMERIC_8},
	{ 0x140a, KEY_NUMERIC_9},
	{ 0x140c, KEY_NUMERIC_0},

	{ 0x140b, KEY_TUNER},		/* AV */
	{ 0x140d, KEY_MODE},		/* A.B */

	{ 0x1446, KEY_TV},
	{ 0x1447, KEY_DVD},
	{ 0x1449, KEY_VIDEO},
	{ 0x144a, KEY_RADIO},		/* Music */
	{ 0x144b, KEY_CAMERA},		/* PIC */

	{ 0x1410, KEY_UP},
	{ 0x1411, KEY_LEFT},
	{ 0x1412, KEY_OK},
	{ 0x1413, KEY_RIGHT},
	{ 0x1414, KEY_DOWN},

	{ 0x140f, KEY_EPG},
	{ 0x1416, KEY_INFO},
	{ 0x144d, KEY_BACKSPACE},

	{ 0x141c, KEY_VOLUMEUP},
	{ 0x141e, KEY_VOLUMEDOWN},

	{ 0x144c, KEY_PLAY},
	{ 0x141d, KEY_MUTE},

	{ 0x141b, KEY_CHANNELUP},
	{ 0x141f, KEY_CHANNELDOWN},

	{ 0x1417, KEY_RED},
	{ 0x1418, KEY_GREEN},
	{ 0x1419, KEY_YELLOW},
	{ 0x141a, KEY_BLUE},

	{ 0x1458, KEY_RECORD},
	{ 0x1448, KEY_STOP},
	{ 0x1440, KEY_PAUSE},

	{ 0x1454, KEY_LAST},
	{ 0x144e, KEY_REWIND},
	{ 0x144f, KEY_FASTFORWARD},
	{ 0x145c, KEY_NEXT},

	/* Terratec Black IR, with most keys in black */
	{ 0x04eb01, KEY_POWER2},

	{ 0x04eb02, KEY_NUMERIC_1},
	{ 0x04eb03, KEY_NUMERIC_2},
	{ 0x04eb04, KEY_NUMERIC_3},
	{ 0x04eb05, KEY_NUMERIC_4},
	{ 0x04eb06, KEY_NUMERIC_5},
	{ 0x04eb07, KEY_NUMERIC_6},
	{ 0x04eb08, KEY_NUMERIC_7},
	{ 0x04eb09, KEY_NUMERIC_8},
	{ 0x04eb0a, KEY_NUMERIC_9},
	{ 0x04eb0c, KEY_NUMERIC_0},

	{ 0x04eb0b, KEY_TEXT},		/* TXT */
	{ 0x04eb0d, KEY_REFRESH},	/* Refresh */

	{ 0x04eb0e, KEY_HOME},
	{ 0x04eb0f, KEY_EPG},

	{ 0x04eb10, KEY_UP},
	{ 0x04eb11, KEY_LEFT},
	{ 0x04eb12, KEY_OK},
	{ 0x04eb13, KEY_RIGHT},
	{ 0x04eb14, KEY_DOWN},

	{ 0x04eb15, KEY_BACKSPACE},
	{ 0x04eb16, KEY_INFO},

	{ 0x04eb17, KEY_RED},
	{ 0x04eb18, KEY_GREEN},
	{ 0x04eb19, KEY_YELLOW},
	{ 0x04eb1a, KEY_BLUE},

	{ 0x04eb1c, KEY_VOLUMEUP},
	{ 0x04eb1e, KEY_VOLUMEDOWN},

	{ 0x04eb1d, KEY_MUTE},

	{ 0x04eb1b, KEY_CHANNELUP},
	{ 0x04eb1f, KEY_CHANNELDOWN},

	{ 0x04eb40, KEY_RECORD},
	{ 0x04eb4c, KEY_PLAY},
	{ 0x04eb58, KEY_PAUSE},

	{ 0x04eb54, KEY_REWIND},
	{ 0x04eb48, KEY_STOP},
	{ 0x04eb5c, KEY_NEXT},
};

static struct rc_map_list nec_terratec_cinergy_xs_map = {
	.map = {
		.scan     = nec_terratec_cinergy_xs,
		.size     = ARRAY_SIZE(nec_terratec_cinergy_xs),
		.rc_proto = RC_PROTO_NEC,
		.name     = RC_MAP_NEC_TERRATEC_CINERGY_XS,
	}
};

static int __init init_rc_map_nec_terratec_cinergy_xs(void)
{
	return rc_map_register(&nec_terratec_cinergy_xs_map);
}

static void __exit exit_rc_map_nec_terratec_cinergy_xs(void)
{
	rc_map_unregister(&nec_terratec_cinergy_xs_map);
}

module_init(init_rc_map_nec_terratec_cinergy_xs)
module_exit(exit_rc_map_nec_terratec_cinergy_xs)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mauro Carvalho Chehab");
