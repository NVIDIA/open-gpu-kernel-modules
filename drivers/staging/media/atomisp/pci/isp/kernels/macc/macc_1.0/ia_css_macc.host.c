// SPDX-License-Identifier: GPL-2.0
/*
 * Support for Intel Camera Imaging ISP subsystem.
 * Copyright (c) 2015, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

#include "ia_css_types.h"
#include "sh_css_defs.h"
#include "ia_css_debug.h"
#include "sh_css_frac.h"

#include "ia_css_macc.host.h"

const struct ia_css_macc_config default_macc_config = {
	1,
};

void
ia_css_macc_encode(
    struct sh_css_isp_macc_params *to,
    const struct ia_css_macc_config *from,
    unsigned int size)
{
	(void)size;
	to->exp = from->exp;
}

void
ia_css_macc_dump(
    const struct sh_css_isp_macc_params *macc,
    unsigned int level);

void
ia_css_macc_debug_dtrace(
    const struct ia_css_macc_config *config,
    unsigned int level)
{
	ia_css_debug_dtrace(level,
			    "config.exp=%d\n",
			    config->exp);
}
