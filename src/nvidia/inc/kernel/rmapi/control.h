/*
 * SPDX-FileCopyrightText: Copyright (c) 2004-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "core/core.h"

#include "resserv/rs_resource.h"
#include "resserv/resserv.h"

#include "utils/nvmacro.h"
#include "rmapi/param_copy.h"

struct NVOC_EXPORTED_METHOD_DEF;
typedef RS_RES_CONTROL_PARAMS_INTERNAL RmCtrlParams;

//
// RmCtrlExecuteCookie
//
// This typedef describes the data used by the rmctrl cmd execution
// path.  The data is filled at the beginning of rmControlCmdExecute()
// and used as necessary in the other stages.
//
struct RS_CONTROL_COOKIE
{
    // Rmctrl Command ID
    NvU32        cmd;

    // Rmctrl Flags
    NvU32        ctrlFlags;

    // Required Access Rights for this command
    const RS_ACCESS_MASK rightsRequired;

    NvBool              bFreeParamCopy;    ///< Indicates that param copies should be cleaned up
    NvBool              bFreeEmbeddedCopy; ///< Indicates embedded param copies should be cleaned up

    RMAPI_PARAM_COPY    paramCopy;
    RMAPI_PARAM_COPY    embeddedParamCopies[4];   // Up to 4 embedded pointers per one RmControl identified
};
typedef RS_CONTROL_COOKIE RmCtrlExecuteCookie;

// values for RmCtrlDeferredCmd.pending
#define RMCTRL_DEFERRED_FREE        0            // buffer is free
#define RMCTRL_DEFERRED_ACQUIRED    1            // buffer is acquired to fill in data
#define RMCTRL_DEFERRED_READY       2            // buffer is acquired and data has been copied.

#define RMCTRL_DEFERRED_MAX_PARAM_SIZE       128 // 128 bytes internal buffer for rmctrl param

typedef struct
{
   NvS32 volatile pending;
   NvU32 cpuInst;
   RmCtrlParams rmCtrlDeferredParams;
   NvU8 paramBuffer[RMCTRL_DEFERRED_MAX_PARAM_SIZE];    // buffer to hold rmCtrlDeferredParams.pParams
} RmCtrlDeferredCmd;

// catch commands misdirected to non-existent engines
#define VERIFY_OBJ_PTR(p)   if (p == NULL) return NV_ERR_INVALID_ARGUMENT

// macros to get/set/clear cap bits
#define RMCTRL_GET_CAP(tbl,cap,field)             (((NvU8)tbl[((1?cap##field)>=cap##_TBL_SIZE) ? 0/0 : (1?cap##field)]) & (0?cap##field))
#define RMCTRL_SET_CAP(tbl,cap,field)             ((tbl[((1?cap##field)>=cap##_TBL_SIZE) ? 0/0 : (1?cap##field)]) |= (0?cap##field))
#define RMCTRL_CLEAR_CAP(tbl,cap,field)           ((tbl[((1?cap##field)>=cap##_TBL_SIZE) ? 0/0 : (1?cap##field)]) &= ~(0?cap##field))

// macros to AND/OR caps between two tables
#define RMCTRL_AND_CAP(finaltbl,tmptbl,tmp,cap,field)     \
    tmp = ((finaltbl[((1?cap##field)>=cap##_TBL_SIZE) ? 0/0 : (1?cap##field)] & tmptbl[((1?cap##field)>=cap##_TBL_SIZE) ? 0/0 : (1?cap##field)]) & (0?cap##field)); \
    finaltbl[((1?cap##field)>=cap##_TBL_SIZE) ? 0/0 : (1?cap##field)] &= ~(0?cap##field); \
    finaltbl[((1?cap##field)>=cap##_TBL_SIZE) ? 0/0 : (1?cap##field)] |= tmp;

#define RMCTRL_OR_CAP(finaltbl,tmptbl,tmp,cap,field)      \
    tmp = ((finaltbl[((1?cap##field)>=cap##_TBL_SIZE) ? 0/0 : (1?cap##field)] | tmptbl[((1?cap##field)>=cap##_TBL_SIZE) ? 0/0 : (1?cap##field)]) & (0?cap##field)); \
    finaltbl[((1?cap##field)>=cap##_TBL_SIZE) ? 0/0 : (1?cap##field)] &= ~(0?cap##field); \
    finaltbl[((1?cap##field)>=cap##_TBL_SIZE) ? 0/0 : (1?cap##field)] |= tmp;

// Whether the command ID is a NULL command?
//    We allow NVXXXX_CTRL_CMD_NULL (0x00000000) as well as the
//    per-class NULL cmd ( _CATEGORY==0x00 and _INDEX==0x00 )
#define RMCTRL_IS_NULL_CMD(cmd)             ((cmd == NVXXXX_CTRL_CMD_NULL) || \
                                             (FLD_TEST_DRF_NUM(XXXX, _CTRL_CMD, _CATEGORY, 0x00, cmd) && \
                                              FLD_TEST_DRF_NUM(XXXX, _CTRL_CMD, _INDEX,    0x00, cmd)))

// top-level internal RM Control interface
NV_STATUS   rmControl_Deferred(RmCtrlDeferredCmd *pRmCtrlDeferredCmd);

// Helper functions for handling embedded parameter copies
NV_STATUS embeddedParamCopyIn(RMAPI_PARAM_COPY  *pParamCopy, RmCtrlParams *pRmCtrlParams);
NV_STATUS embeddedParamCopyOut(RMAPI_PARAM_COPY  *pParamCopy, RmCtrlParams *pRmCtrlParams);

#define RM_CLIENT_PTR_ACCESS_CHECK_READ     NVBIT(0)
#define RM_CLIENT_PTR_ACCESS_CHECK_WRITE    NVBIT(1)

//
// For NVOC Exported functions
//
//   RMCTRL_FLAGS(A, B, C) is expanded to
//      0 | RMCTRL_FLAGS_A | RMCTRL_FLAGS_B | RMCTRL_FLAGS_C
//
//   ACCESS_RIGHTS(A, B, C) is expanded to
//      0 | NVBIT(RS_ACCESS_A) | NVBIT(RS_ACCESS_B) | NVBIT(RS_ACCESS_C)
//
#define RMCTRL_EXPORT(cmdId, ...)      [[nvoc::export(cmdId, __VA_ARGS__)]]
#define _RMCTRL_PREP_FLAG_ARG(x)       | NV_CONCATENATE(RMCTRL_FLAGS_, x)
#define RMCTRL_FLAGS(...)              (0 NV_FOREACH_ARG_NOCOMMA(_RMCTRL_PREP_FLAG_ARG, __VA_ARGS__))
#define _RMCTRL_PREP_ACCESS_ARG(x)     | NVBIT(NV_CONCATENATE(RS_ACCESS_, x))
#define ACCESS_RIGHTS(...)             (0 NV_FOREACH_ARG_NOCOMMA(_RMCTRL_PREP_ACCESS_ARG, __VA_ARGS__))

#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(ctrlFlags)           \
    (                                                              \
        (ctrlFlags & RMCTRL_FLAGS_ROUTE_TO_PHYSICAL) &&            \
        !RMCFG_FEATURE_PHYSICAL_RM                        \
    )

// per-rmcontrol flags values
#define RMCTRL_FLAGS_NONE                                     0x000000000
#define RMCTRL_FLAGS_NO_STATIC                                0x000000000 // internal to chip-config. TODO -- delete
#define RMCTRL_FLAGS_ONLY_IF_CMD_DEFINED                      0x000000000 // internal to chip-config. TODO -- delete
#define RMCTRL_FLAGS_KERNEL_PRIVILEGED                        0x000000000
#define RMCTRL_FLAGS_NO_GPUS_LOCK                             0x000000001
#define RMCTRL_FLAGS_NO_GPUS_ACCESS                           0x000000002
#define RMCTRL_FLAGS_PRIVILEGED                               0x000000004
#define RMCTRL_FLAGS_HACK_USED_ON_MULTIPLE_CLASSES            0x000000008
#define RMCTRL_FLAGS_NON_PRIVILEGED                           0x000000010
#define RMCTRL_FLAGS_BIG_PAYLOAD                              0x000000020
#define RMCTRL_FLAGS_GPU_LOCK_DEVICE_ONLY                     0x000000040
#define RMCTRL_FLAGS_PRIVILEGED_IF_RS_ACCESS_DISABLED         0x000000100 // for Resserv Access Rights migration
#define RMCTRL_FLAGS_ROUTE_TO_PHYSICAL                        0x000000200
#define RMCTRL_FLAGS_INTERNAL                                 0x000000400
#define RMCTRL_FLAGS_API_LOCK_READONLY                        0x000000800
#define RMCTRL_FLAGS_GPU_LOCK_READONLY                        0x000001000
#define RMCTRL_FLAGS_ROUTE_TO_VGPU_HOST                       0x000002000
#define RMCTRL_FLAGS_CACHEABLE                                0x000004000
#define RMCTRL_FLAGS_COPYOUT_ON_ERROR                         0x000008000
#define RMCTRL_FLAGS_ALLOW_WITHOUT_SYSMEM_ACCESS              0x000010000

#endif // _CONTROL_H_


