/* SPDX-License-Identifier: GPL-2.0 */
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

#ifndef __IA_CSS_CTC2_HOST_H
#define __IA_CSS_CTC2_HOST_H

#include "ia_css_ctc2_param.h"
#include "ia_css_ctc2_types.h"

extern const struct ia_css_ctc2_config default_ctc2_config;

/*Encode Functions to translate parameters from userspace into ISP space*/

void ia_css_ctc2_vmem_encode(struct ia_css_isp_ctc2_vmem_params *to,
			     const struct ia_css_ctc2_config *from,
			     size_t size);

void ia_css_ctc2_encode(struct ia_css_isp_ctc2_dmem_params *to,
			struct ia_css_ctc2_config *from,
			size_t size);

#endif /* __IA_CSS_CTC2_HOST_H */
