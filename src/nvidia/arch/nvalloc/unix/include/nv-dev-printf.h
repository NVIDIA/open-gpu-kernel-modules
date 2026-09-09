/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef _NV_DEV_PRINTF_H_
#define _NV_DEV_PRINTF_H_

#include "nv_stdarg.h"
#include "nv-kernel-interface-api.h"

/*
 * Device logging (kernel/system log) levels.
 *
 * These are distinct from NV_DBG_* which are used for internal driver filtering.
 */
typedef enum NV_LOG_LEVEL
{
    NV_LOG_LEVEL_DEBUG   = 0,
    NV_LOG_LEVEL_INFO    = 1,
    NV_LOG_LEVEL_NOTICE  = 2,
    NV_LOG_LEVEL_WARNING = 3,
    NV_LOG_LEVEL_ERROR   = 4,
    NV_LOG_LEVEL_CRIT    = 5,
    NV_LOG_LEVEL_ALERT   = 6,
} NV_LOG_LEVEL;

struct nv_state_t;

int  NV_API_CALL  nv_dev_printf(struct nv_state_t *, NV_LOG_LEVEL, const char *, ...);
int  NV_API_CALL  nv_dev_vprintf(struct nv_state_t *, NV_LOG_LEVEL, const char *, va_list);

#endif // _NV_DEV_PRINTF_H_
