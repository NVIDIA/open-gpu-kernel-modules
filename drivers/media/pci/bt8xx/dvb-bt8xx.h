/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Bt8xx based DVB adapter driver
 *
 * Copyright (C) 2002,2003 Florian Schirmer <jolt@tuxbox.org>
 * Copyright (C) 2002 Peter Hettkamp <peter.hettkamp@htp-tel.de>
 * Copyright (C) 1999-2001 Ralph  Metzler & Marcus Metzler for convergence integrated media GmbH
 * Copyright (C) 1998,1999 Christian Theiss <mistert@rz.fh-augsburg.de>
 */

#ifndef DVB_BT8XX_H
#define DVB_BT8XX_H

#include <linux/i2c.h>
#include <linux/mutex.h>
#include <media/dvbdev.h>
#include <media/dvb_net.h>
#include "bttv.h"
#include "mt352.h"
#include "sp887x.h"
#include "dst_common.h"
#include "nxt6000.h"
#include "cx24110.h"
#include "or51211.h"
#include "lgdt330x.h"
#include "zl10353.h"
#include "tuner-simple.h"

struct dvb_bt8xx_card {
	struct mutex lock;
	int nfeeds;
	char card_name[32];
	struct dvb_adapter dvb_adapter;
	struct bt878 *bt;
	unsigned int bttv_nr;
	struct dvb_demux demux;
	struct dmxdev dmxdev;
	struct dmx_frontend fe_hw;
	struct dmx_frontend fe_mem;
	u32 gpio_mode;
	u32 op_sync_orin;
	u32 irq_err_ignore;
	struct i2c_adapter *i2c_adapter;
	struct dvb_net dvbnet;

	struct dvb_frontend* fe;
};

#endif /* DVB_BT8XX_H */
