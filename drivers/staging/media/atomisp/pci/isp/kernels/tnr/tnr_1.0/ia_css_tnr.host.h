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

#ifndef __IA_CSS_TNR_HOST_H
#define __IA_CSS_TNR_HOST_H

#include "ia_css_binary.h"
#include "ia_css_tnr_state.h"
#include "ia_css_tnr_types.h"
#include "ia_css_tnr_param.h"

extern const struct ia_css_tnr_config default_tnr_config;

void
ia_css_tnr_encode(
    struct sh_css_isp_tnr_params *to,
    const struct ia_css_tnr_config *from,
    unsigned int size);

void
ia_css_tnr_dump(
    const struct sh_css_isp_tnr_params *tnr,
    unsigned int level);

void
ia_css_tnr_debug_dtrace(
    const struct ia_css_tnr_config *config,
    unsigned int level);

void
ia_css_tnr_config(
    struct sh_css_isp_tnr_isp_config      *to,
    const struct ia_css_tnr_configuration *from,
    unsigned int size);

void
ia_css_tnr_configure(
    const struct ia_css_binary     *binary,
    const struct ia_css_frame * const *frames);

void
ia_css_init_tnr_state(
    struct sh_css_isp_tnr_dmem_state *state,
    size_t size);
#endif /* __IA_CSS_TNR_HOST_H */
