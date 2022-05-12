// SPDX-License-Identifier: GPL-2.0+
// avermedia-m135a.c - Keytable for Avermedia M135A Remote Controllers
//
// Copyright (c) 2010 by Mauro Carvalho Chehab
// Copyright (c) 2010 by Herton Ronaldo Krzesinski <herton@mandriva.com.br>

#include <media/rc-map.h>
#include <linux/module.h>

/*
 * Avermedia M135A with RM-JX and RM-K6 remote controls
 *
 * On Avermedia M135A with IR model RM-JX, the same codes exist on both
 * Positivo (BR) and original IR, initial version and remote control codes
 * added by Mauro Carvalho Chehab <mchehab@kernel.org>
 *
 * Positivo also ships Avermedia M135A with model RM-K6, extra control
 * codes added by Herton Ronaldo Krzesinski <herton@mandriva.com.br>
 */

static struct rc_map_table avermedia_m135a[] = {
	/* RM-JX */
	{ 0x0200, KEY_POWER2 },
	{ 0x022e, KEY_DOT },		/* '.' */
	{ 0x0201, KEY_MODE },		/* TV/FM or SOURCE */

	{ 0x0205, KEY_NUMERIC_1 },
	{ 0x0206, KEY_NUMERIC_2 },
	{ 0x0207, KEY_NUMERIC_3 },
	{ 0x0209, KEY_NUMERIC_4 },
	{ 0x020a, KEY_NUMERIC_5 },
	{ 0x020b, KEY_NUMERIC_6 },
	{ 0x020d, KEY_NUMERIC_7 },
	{ 0x020e, KEY_NUMERIC_8 },
	{ 0x020f, KEY_NUMERIC_9 },
	{ 0x0211, KEY_NUMERIC_0 },

	{ 0x0213, KEY_RIGHT },		/* -> or L */
	{ 0x0212, KEY_LEFT },		/* <- or R */

	{ 0x0215, KEY_MENU },
	{ 0x0217, KEY_CAMERA },		/* Capturar Imagem or Snapshot */
	{ 0x0210, KEY_SHUFFLE },	/* Amostra or 16 chan prev */

	{ 0x0303, KEY_CHANNELUP },
	{ 0x0302, KEY_CHANNELDOWN },
	{ 0x021f, KEY_VOLUMEUP },
	{ 0x021e, KEY_VOLUMEDOWN },
	{ 0x020c, KEY_ENTER },		/* Full Screen */

	{ 0x0214, KEY_MUTE },
	{ 0x0208, KEY_AUDIO },

	{ 0x0203, KEY_TEXT },		/* Teletext */
	{ 0x0204, KEY_EPG },
	{ 0x022b, KEY_TV2 },		/* TV2 or PIP */

	{ 0x021d, KEY_RED },
	{ 0x021c, KEY_YELLOW },
	{ 0x0301, KEY_GREEN },
	{ 0x0300, KEY_BLUE },

	{ 0x021a, KEY_PLAYPAUSE },
	{ 0x0219, KEY_RECORD },
	{ 0x0218, KEY_PLAY },
	{ 0x021b, KEY_STOP },

	/* RM-K6 */
	{ 0x0401, KEY_POWER2 },
	{ 0x0406, KEY_MUTE },
	{ 0x0408, KEY_MODE },     /* TV/FM */

	{ 0x0409, KEY_NUMERIC_1 },
	{ 0x040a, KEY_NUMERIC_2 },
	{ 0x040b, KEY_NUMERIC_3 },
	{ 0x040c, KEY_NUMERIC_4 },
	{ 0x040d, KEY_NUMERIC_5 },
	{ 0x040e, KEY_NUMERIC_6 },
	{ 0x040f, KEY_NUMERIC_7 },
	{ 0x0410, KEY_NUMERIC_8 },
	{ 0x0411, KEY_NUMERIC_9 },
	{ 0x044c, KEY_DOT },      /* '.' */
	{ 0x0412, KEY_NUMERIC_0 },
	{ 0x0407, KEY_REFRESH },  /* Refresh/Reload */

	{ 0x0413, KEY_AUDIO },
	{ 0x0440, KEY_SCREEN },   /* Full Screen toggle */
	{ 0x0441, KEY_HOME },
	{ 0x0442, KEY_BACK },
	{ 0x0447, KEY_UP },
	{ 0x0448, KEY_DOWN },
	{ 0x0449, KEY_LEFT },
	{ 0x044a, KEY_RIGHT },
	{ 0x044b, KEY_OK },
	{ 0x0404, KEY_VOLUMEUP },
	{ 0x0405, KEY_VOLUMEDOWN },
	{ 0x0402, KEY_CHANNELUP },
	{ 0x0403, KEY_CHANNELDOWN },

	{ 0x0443, KEY_RED },
	{ 0x0444, KEY_GREEN },
	{ 0x0445, KEY_YELLOW },
	{ 0x0446, KEY_BLUE },

	{ 0x0414, KEY_TEXT },
	{ 0x0415, KEY_EPG },
	{ 0x041a, KEY_TV2 },      /* PIP */
	{ 0x041b, KEY_CAMERA },      /* Snapshot */

	{ 0x0417, KEY_RECORD },
	{ 0x0416, KEY_PLAYPAUSE },
	{ 0x0418, KEY_STOP },
	{ 0x0419, KEY_PAUSE },

	{ 0x041f, KEY_PREVIOUS },
	{ 0x041c, KEY_REWIND },
	{ 0x041d, KEY_FORWARD },
	{ 0x041e, KEY_NEXT },
};

static struct rc_map_list avermedia_m135a_map = {
	.map = {
		.scan     = avermedia_m135a,
		.size     = ARRAY_SIZE(avermedia_m135a),
		.rc_proto = RC_PROTO_NEC,
		.name     = RC_MAP_AVERMEDIA_M135A,
	}
};

static int __init init_rc_map_avermedia_m135a(void)
{
	return rc_map_register(&avermedia_m135a_map);
}

static void __exit exit_rc_map_avermedia_m135a(void)
{
	rc_map_unregister(&avermedia_m135a_map);
}

module_init(init_rc_map_avermedia_m135a)
module_exit(exit_rc_map_avermedia_m135a)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mauro Carvalho Chehab");
