/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Driver for Quantek QT1010 silicon tuner
 *
 *  Copyright (C) 2006 Antti Palosaari <crope@iki.fi>
 *                     Aapo Tahkola <aet@rasterburn.org>
 */

#ifndef QT1010_H
#define QT1010_H

#include <media/dvb_frontend.h>

struct qt1010_config {
	u8 i2c_address;
};

/**
 * qt1010_attach() - Attach a qt1010 tuner to the supplied frontend structure
 *
 * @fe:   frontend to attach to
 * @i2c:  i2c adapter to use
 * @cfg:  tuner hw based configuration
 * @return fe  pointer on success, NULL on failure
 */
#if IS_REACHABLE(CONFIG_MEDIA_TUNER_QT1010)
extern struct dvb_frontend *qt1010_attach(struct dvb_frontend *fe,
					  struct i2c_adapter *i2c,
					  struct qt1010_config *cfg);
#else
static inline struct dvb_frontend *qt1010_attach(struct dvb_frontend *fe,
						 struct i2c_adapter *i2c,
						 struct qt1010_config *cfg)
{
	printk(KERN_WARNING "%s: driver disabled by Kconfig\n", __func__);
	return NULL;
}
#endif // CONFIG_MEDIA_TUNER_QT1010

#endif
