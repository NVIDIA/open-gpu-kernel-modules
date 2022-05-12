// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2021 Advanced Micro Devices, Inc.
 *
 * Authors: AMD
 */

#ifndef _DCN303_RESOURCE_H_
#define _DCN303_RESOURCE_H_

#include "core_types.h"

struct resource_pool *dcn303_create_resource_pool(const struct dc_init_data *init_data, struct dc *dc);

void dcn303_update_bw_bounding_box(struct dc *dc, struct clk_bw_params *bw_params);

#endif /* _DCN303_RESOURCE_H_ */
