/* SPDX-License-Identifier: GPL-2.0-or-later */
  /*
     DVB Driver for Philips tda827x / tda827xa Silicon tuners

     (c) 2005 Hartmut Hackmann
     (c) 2007 Michael Krufky


  */

#ifndef __DVB_TDA827X_H__
#define __DVB_TDA827X_H__

#include <linux/i2c.h>
#include <media/dvb_frontend.h>
#include "tda8290.h"

struct tda827x_config
{
	/* saa7134 - provided callbacks */
	int (*init) (struct dvb_frontend *fe);
	int (*sleep) (struct dvb_frontend *fe);

	/* interface to tda829x driver */
	enum tda8290_lna config;
	int	     switch_addr;

	void (*agcf)(struct dvb_frontend *fe);
};


/**
 * tda827x_attach() - Attach a tda827x tuner to the supplied frontend structure
 *
 * @fe: Frontend to attach to.
 * @addr: i2c address of the tuner.
 * @i2c: i2c adapter to use.
 * @cfg: optional callback function pointers.
 * @return FE pointer on success, NULL on failure.
 */
#if IS_REACHABLE(CONFIG_MEDIA_TUNER_TDA827X)
extern struct dvb_frontend* tda827x_attach(struct dvb_frontend *fe, int addr,
					   struct i2c_adapter *i2c,
					   struct tda827x_config *cfg);
#else
static inline struct dvb_frontend* tda827x_attach(struct dvb_frontend *fe,
						  int addr,
						  struct i2c_adapter *i2c,
						  struct tda827x_config *cfg)
{
	printk(KERN_WARNING "%s: driver disabled by Kconfig\n", __func__);
	return NULL;
}
#endif // CONFIG_MEDIA_TUNER_TDA827X

#endif // __DVB_TDA827X_H__
