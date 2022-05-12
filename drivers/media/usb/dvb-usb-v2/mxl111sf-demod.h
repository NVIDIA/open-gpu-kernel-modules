/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  mxl111sf-demod.h - driver for the MaxLinear MXL111SF DVB-T demodulator
 *
 *  Copyright (C) 2010-2014 Michael Krufky <mkrufky@linuxtv.org>
 */

#ifndef __MXL111SF_DEMOD_H__
#define __MXL111SF_DEMOD_H__

#include <media/dvb_frontend.h>
#include "mxl111sf.h"

struct mxl111sf_demod_config {
	int (*read_reg)(struct mxl111sf_state *state, u8 addr, u8 *data);
	int (*write_reg)(struct mxl111sf_state *state, u8 addr, u8 data);
	int (*program_regs)(struct mxl111sf_state *state,
			    struct mxl111sf_reg_ctrl_info *ctrl_reg_info);
};

#if IS_ENABLED(CONFIG_DVB_USB_MXL111SF)
extern
struct dvb_frontend *mxl111sf_demod_attach(struct mxl111sf_state *mxl_state,
				   const struct mxl111sf_demod_config *cfg);
#else
static inline
struct dvb_frontend *mxl111sf_demod_attach(struct mxl111sf_state *mxl_state,
				   const struct mxl111sf_demod_config *cfg)
{
	printk(KERN_WARNING "%s: driver disabled by Kconfig\n", __func__);
	return NULL;
}
#endif /* CONFIG_DVB_USB_MXL111SF */

#endif /* __MXL111SF_DEMOD_H__ */
