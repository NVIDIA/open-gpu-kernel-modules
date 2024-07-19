/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "deprecated/rmapi_deprecated.h"

#include "utils/nvmacro.h"
#include "nvctassert.h"

#include "ctrl/ctrl2080/ctrl2080bios.h" // NV2080_CTRL_CMD_BIOS_GET_INFO
#include "ctrl/ctrl2080/ctrl2080bus.h" // NV2080_CTRL_CMD_BUS_GET_INFO
#include "ctrl/ctrl2080/ctrl2080clk.h" // NV2080_CTRL_CMD_CLK_*
#include "ctrl/ctrl2080/ctrl2080fb.h"
#include "ctrl/ctrl2080/ctrl2080gpu.h" // NV2080_CTRL_CMD_GPU_GET_INFO
#include "ctrl/ctrl2080/ctrl2080perf.h" // NV2080_CTRL_CMD_PERF_*
#include "ctrl/ctrl0080/ctrl0080perf.h" // NV0080_CTRL_CMD_PERF_GET_CAPS
#include "ctrl/ctrl0080/ctrl0080bsp.h" // NV0080_CTRL_CMD_BSP_GET_CAPS
#include "ctrl/ctrl0080/ctrl0080msenc.h" // NV0080_CTRL_CMD_MSENC_GET_CAPS
#include "ctrl/ctrl0073/ctrl0073dp.h" // NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES
#include "rmapi/rmapi_utils.h"

//
// TODO - deprecation shim shouldn't depend on RM internals 
// Bug 3188307
// 
#include "core/core.h"
#include "gpu/gpu.h"
#include "rmapi/rs_utils.h"
#include "rmapi/rmapi.h"

#include <stddef.h> // NULL

/**
 * Kernel-space deprecated control conversion.
 * A conversion here is applicable to all RM clients on most platforms.
 *
 * See also arch/nvalloc/unix/lib/rmapi-user-deprecated-control.c
 */

#define V2_CONVERTER(cmd) NV_CONCATENATE(_ctrl_convert_v2, cmd)
#define V3_CONVERTER(cmd) NV_CONCATENATE(_ctrl_convert_v3, cmd)

// Need to declare them first to add to table below.
// Maybe reshuffle this file around so that we don't need separate decl+def for these?
static NV_STATUS V2_CONVERTER(_NV0000_CTRL_CMD_SYSTEM_GET_BUILD_VERSION)(API_SECURITY_INFO *pSecInfo, DEPRECATED_CONTEXT *pContextInternal, NVOS54_PARAMETERS *pArgs);
static NV_STATUS V2_CONVERTER(_NV2080_CTRL_CMD_BIOS_GET_INFO)(API_SECURITY_INFO *pSecInfo, DEPRECATED_CONTEXT *pContextInternal, NVOS54_PARAMETERS *pArgs);
static NV_STATUS V2_CONVERTER(_NV2080_CTRL_CMD_BUS_GET_INFO)(API_SECURITY_INFO *pSecInfo, DEPRECATED_CONTEXT *pContextInternal, NVOS54_PARAMETERS *pArgs);
static NV_STATUS V2_CONVERTER(_NV0080_CTRL_CMD_BSP_GET_CAPS)(API_SECURITY_INFO *pSecInfo, DEPRECATED_CONTEXT *pContextInternal, NVOS54_PARAMETERS *pArgs);
static NV_STATUS V2_CONVERTER(_NV2080_CTRL_CMD_GPU_GET_INFO)(API_SECURITY_INFO *pSecInfo, DEPRECATED_CONTEXT *pContextInternal, NVOS54_PARAMETERS *pArgs);
static NV_STATUS V2_CONVERTER(_NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES)(API_SECURITY_INFO *pSecInfo, DEPRECATED_CONTEXT *pContextInternal, NVOS54_PARAMETERS *pArgs);
static NV_STATUS V2_CONVERTER(_NV0080_CTRL_CMD_MSENC_GET_CAPS)(API_SECURITY_INFO *pSecInfo, DEPRECATED_CONTEXT *pContextInternal, NVOS54_PARAMETERS *pArgs);
static NV_STATUS V2_CONVERTER(_NV2080_CTRL_CMD_FB_GET_INFO)(API_SECURITY_INFO *pSecInfo, DEPRECATED_CONTEXT *pContextInternal, NVOS54_PARAMETERS *pArgs);

typedef struct
{
    NvU32                       cmd;      // NVXXXX_CTRL_CMD_* value
    RmDeprecatedControlHandler  func;     // pointer to handler
} RmDeprecatedControlEntry;


static const RmDeprecatedControlEntry rmDeprecatedControlTable[] =
{
    { NV0000_CTRL_CMD_SYSTEM_GET_BUILD_VERSION,             V2_CONVERTER(_NV0000_CTRL_CMD_SYSTEM_GET_BUILD_VERSION)             },
    { NV2080_CTRL_CMD_BIOS_GET_INFO,                        V2_CONVERTER(_NV2080_CTRL_CMD_BIOS_GET_INFO)                        },
    { NV2080_CTRL_CMD_BUS_GET_INFO,                         V2_CONVERTER(_NV2080_CTRL_CMD_BUS_GET_INFO)                         },
    { NV0080_CTRL_CMD_BSP_GET_CAPS,                         V2_CONVERTER(_NV0080_CTRL_CMD_BSP_GET_CAPS)                         },
    { NV2080_CTRL_CMD_GPU_GET_INFO,                         V2_CONVERTER(_NV2080_CTRL_CMD_GPU_GET_INFO)                         },
    { NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES,                V2_CONVERTER(_NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES)                },
    { NV0080_CTRL_CMD_MSENC_GET_CAPS,                       V2_CONVERTER(_NV0080_CTRL_CMD_MSENC_GET_CAPS)                       },
    { NV2080_CTRL_CMD_FB_GET_INFO,                          V2_CONVERTER(_NV2080_CTRL_CMD_FB_GET_INFO)                          },
    { 0, NULL }
};

/*!
 * Check whether the cmd is part of the GSS legacy
 * commands.
 */
static NvBool IsGssLegacyCall(NvU32 cmd)
{
    return !!(cmd & RM_GSS_LEGACY_MASK);
}

RmDeprecatedControlHandler RmDeprecatedGetControlHandler(NVOS54_PARAMETERS *pArgs)
{
    NvU32     i;
    NV_STATUS nvStatus;
    RsClient *pClient     = NULL;
    OBJGPU   *pGpu        = NULL;
    NvBool gssLegacyVgpuCall = NV_FALSE;

    NV_CHECK_OK_OR_ELSE(nvStatus,
                        LEVEL_ERROR,
                        serverGetClientUnderLock(&g_resServ, pArgs->hClient, &pClient),
                        return NULL);

    // pGpu is expected to be NULL on some controls, addi ng a void to avoid coverity failure.
    (void)gpuGetByHandle(pClient, pArgs->hObject, NULL, &pGpu);

    // search rmDeprecatedControlTable for handler
    for (i = 0; rmDeprecatedControlTable[i].cmd != 0; i++)
    {
        if (pArgs->cmd == rmDeprecatedControlTable[i].cmd)
        {
            RmDeprecatedControlHandler rmDeprecatedControlHandler = rmDeprecatedControlTable[i].func;

            NV_ASSERT_OR_ELSE_STR(pArgs->flags == 0, "IRQL and BYPASS_LOCK control calls currently unsupported for deprecated control calls",
                                  return NULL);

            return rmDeprecatedControlHandler;
        }
    }

    gssLegacyVgpuCall = ((pGpu != NULL) && IS_VIRTUAL(pGpu));

    // Check if the cmd is part of the legacy GSS control.
    if (pGpu != NULL && IsGssLegacyCall(pArgs->cmd) && (IS_GSP_CLIENT(pGpu) || gssLegacyVgpuCall))
    {
        extern NV_STATUS RmGssLegacyRpcCmd(API_SECURITY_INFO*, DEPRECATED_CONTEXT*, NVOS54_PARAMETERS*);
        return RmGssLegacyRpcCmd;
    }

    // If no handler found, the control is not deprecated and can be routed normally
    return NULL;
}

//
// Perform deep copy in/out of the user arguments
// Handlers that just reroute controls without changing their param structures
// don't need to call these.
//

#define CONTROL_PARAM_TOKEN_MAX_POINTERS 4
typedef struct
{
    struct
    {
        void *pKernelParams;
        NvP64 pUserParams;
        NvU32 paramSize;
        NvU32 paramStructOffset;
        NvU32 listSizeOffset;
        NvBool bListSizeIsCount;
        NvU32 maxListSize;
    } params[CONTROL_PARAM_TOKEN_MAX_POINTERS];
    NvU32 paramCount;
} CONTROL_PARAM_TOKEN;

static void _ctrlparamsTokenInit
(
    CONTROL_PARAM_TOKEN *pToken,
    NvP64 pParams,
    NvU32 paramSize
)
{
    // Don't want any non-header dependency, including nvport..
    NVMISC_MEMSET(pToken, 0, sizeof(*pToken));
    pToken->params[0].pUserParams = pParams;
    pToken->params[0].paramSize = paramSize;
    pToken->params[0].paramStructOffset = 0;
    pToken->params[0].listSizeOffset = (NvU32)~0;
    pToken->params[0].bListSizeIsCount = NV_FALSE;
    pToken->params[0].maxListSize = 0;
    pToken->paramCount = 1;
}

static NV_STATUS _ctrlparamsTokenAddEmbeddedPtr
(
    CONTROL_PARAM_TOKEN *pToken,
    NvU32 paramStructOffset,
    NvU32 listSizeOffset,
    NvBool bListSizeIsCount,
    NvU32 listItemSize,
    NvU32 maxListSize
)
{
    if (pToken->paramCount >= CONTROL_PARAM_TOKEN_MAX_POINTERS)
        return NV_ERR_INSUFFICIENT_RESOURCES;
    if (paramStructOffset >= pToken->params[0].paramSize)
        return NV_ERR_INVALID_OFFSET;
    if (!NV_IS_ALIGNED(paramStructOffset, 8))
        return NV_ERR_INVALID_OFFSET;
    if (listItemSize == 0)
        return NV_ERR_INVALID_ARGUMENT;

    pToken->params[pToken->paramCount].paramStructOffset = paramStructOffset;
    pToken->params[pToken->paramCount].listSizeOffset = listSizeOffset;
    pToken->params[pToken->paramCount].bListSizeIsCount = bListSizeIsCount;
    pToken->params[pToken->paramCount].paramSize = listItemSize;
    pToken->params[pToken->paramCount].maxListSize = maxListSize;
    pToken->paramCount++;
    return NV_OK;
}

#define CTRL_PARAMS_TOKEN_INIT(token, paramType, pArgs) \
    _ctrlparamsTokenInit(&token, pArgs->params, sizeof(paramType))

#define CTRL_PARAMS_TOKEN_ADD_EMBEDDED(token, paramType, pArgs, listField, listSizeField, \
                                       listSizeIsCount, listElemType, maxListSize)        \
    _ctrlparamsTokenAddEmbeddedPtr(&token, NV_OFFSETOF(paramType, listField),             \
                                   NV_OFFSETOF(paramType, listSizeField),                 \
                                   listSizeIsCount, sizeof(listElemType), maxListSize)

static NV_STATUS ctrlparamAcquire
(
    API_SECURITY_INFO *pSecInfo,
    CONTROL_PARAM_TOKEN *pToken,
    NVOS54_PARAMETERS   *pArgs
)
{
    NV_STATUS status;
    NvU32 i;

    if (pToken->paramCount == 0 || pToken->paramCount > CONTROL_PARAM_TOKEN_MAX_POINTERS)
        return NV_ERR_INVALID_ARGUMENT;

    status = RmCopyUserForDeprecatedApi(RMAPI_DEPRECATED_COPYIN,
                                        RMAPI_DEPRECATED_BUFFER_ALLOCATE,
                                        pToken->params[0].pUserParams,
                                        pToken->params[0].paramSize,
                                        &pToken->params[0].pKernelParams,
                                        pSecInfo->paramLocation == PARAM_LOCATION_USER);
    if (status != NV_OK)
        goto done;

    pArgs->params = NV_PTR_TO_NvP64(pToken->params[0].pKernelParams);
    for (i = 1; i < pToken->paramCount; i++)
    {
        NvU32 offset = pToken->params[i].paramStructOffset;

        if (pToken->params[i].listSizeOffset != (NvU32)~0)
        {
            NvU32 listItemCount = *(NvU32*)((NvUPtr)pArgs->params + pToken->params[i].listSizeOffset);

            if (listItemCount == 0)
            {
                //
                // User passed a zero sized list. Do not copy parameters.
                // Let the RM control decide about its parameter policy.
                //
                continue;
            }

            if (listItemCount > pToken->params[i].maxListSize)
            {
                status = NV_ERR_OUT_OF_RANGE;
                goto done;
            }

            //
            // Either the listItemCount is the actual count of items, in which
            // case we need to read the size of individual items and (safely)
            // multiply them, or it is the total count in bytes, in which case
            // we just attempt to copyin all of it.
            //
            if (pToken->params[i].bListSizeIsCount)
            {
                //
                // Overflow check: unsigned multiplication overflows are well defined
                //     so it is safe to first multiply and then perform the check.
                //     We already checked that listItemSize != 0 in _ctrlparamsTokenAddEmbeddedPtr.
                //
                NvU32 listItemSize  = pToken->params[i].paramSize;
                pToken->params[i].paramSize = listItemSize * listItemCount;
                if (listItemCount != (pToken->params[i].paramSize / listItemSize))
                {
                    status = NV_ERR_INVALID_ARGUMENT;
                    goto done;
                }
            }
            else
            {
                pToken->params[i].paramSize = listItemCount;
            }
        }

        pToken->params[i].pUserParams = *(NvP64*)((NvUPtr)pArgs->params + offset);
        status = RmCopyUserForDeprecatedApi(RMAPI_DEPRECATED_COPYIN,
                                            RMAPI_DEPRECATED_BUFFER_ALLOCATE,
                                            pToken->params[i].pUserParams,
                                            pToken->params[i].paramSize,
                                            &pToken->params[i].pKernelParams,
                                            pSecInfo->paramLocation != PARAM_LOCATION_KERNEL);
        if (status != NV_OK)
            goto done;

        // Update the pointer in the top level struct to point to the internal copy
        *(NvP64*)((NvUPtr)pArgs->params + offset) = NV_PTR_TO_NvP64(pToken->params[i].pKernelParams);
    }
done:
    return status;
}

static void ctrlparamRelease
(
    API_SECURITY_INFO *pSecInfo,
    CONTROL_PARAM_TOKEN *pToken,
    NVOS54_PARAMETERS   *pArgs
)
{
    NvU32 i;

    if (pToken->params[0].pKernelParams == NULL)
        return;

    for (i = pToken->paramCount - 1; i > 0; i--)
    {
        NvU32 offset = pToken->params[i].paramStructOffset;
        NvU32 listItemCount = *(NvU32*)((NvUPtr)pArgs->params + pToken->params[i].listSizeOffset);

        if (pToken->params[i].pKernelParams == NULL)
            continue;

        if (listItemCount == 0)
            continue;

        // We already checked the item size in ctrlparamAcquire

        // Update the pointer in the top level struct back to external copy
        *(NvP64*)((NvUPtr)pArgs->params + offset) = pToken->params[i].pUserParams;

        // The copy out could fail, but nothing we can do there.
        RmCopyUserForDeprecatedApi(RMAPI_DEPRECATED_COPYOUT,
                                   RMAPI_DEPRECATED_BUFFER_ALLOCATE,
                                   pToken->params[i].pUserParams,
                                   pToken->params[i].paramSize,
                                   &pToken->params[i].pKernelParams,
                                   pSecInfo->paramLocation != PARAM_LOCATION_KERNEL);
    }

    RmCopyUserForDeprecatedApi(RMAPI_DEPRECATED_COPYOUT,
                               RMAPI_DEPRECATED_BUFFER_ALLOCATE,
                               pToken->params[0].pUserParams,
                               pToken->params[0].paramSize,
                               &pToken->params[0].pKernelParams,
                               pSecInfo->paramLocation == PARAM_LOCATION_USER);
    pArgs->params = pToken->params[0].pUserParams;
}

//
// Many controls have embedded pointers and a _v2 variant that has it flattened.
// This macro automatically generates the converter function so that the old
// API can be added to the graveyard and translated to the new one as needed.
//
#define CONVERT_TO_V2_EX(newCmd, oldParamsType, newParamsType,                                \
                         oldListField, newListField, listSizeField, listSizeIsCount, listElemType, \
                         set, get, customSet, customGet)                                      \
    do {                                                                                      \
        NV_STATUS status;                                                                     \
        CONTROL_PARAM_TOKEN token;                                                            \
        newParamsType *pParams2;                                                              \
        NvU64 maxListSize = sizeof(pParams2->newListField);                                   \
        if (listSizeIsCount)                                                                  \
            maxListSize /= sizeof(listElemType);                                              \
        CTRL_PARAMS_TOKEN_INIT(token, oldParamsType, pArgs);                                  \
                                                                                              \
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,                                                    \
            CTRL_PARAMS_TOKEN_ADD_EMBEDDED(token, oldParamsType, pArgs, oldListField,         \
                                           listSizeField, listSizeIsCount, listElemType, maxListSize)); \
        status = ctrlparamAcquire(pSecInfo, &token, pArgs);                                   \
        if (status == NV_OK)                                                                  \
        {                                                                                     \
            oldParamsType *pParams = NvP64_VALUE(pArgs->params);                              \
            NvU32 listSize = listSizeIsCount ? /* overflow checked in ctrlparamAcquire() */   \
                pParams->listSizeField * sizeof(listElemType) :                               \
                pParams->listSizeField;                                                       \
                                                                                              \
            pParams2 = portMemAllocNonPaged(sizeof(newParamsType));                           \
            if (pParams2 == NULL)                                                             \
            {                                                                                 \
                NV_PRINTF(LEVEL_ERROR, "No memory for pParams2\n");                           \
                ctrlparamRelease(pSecInfo, &token, pArgs);                                    \
                return NV_ERR_NO_MEMORY;                                                      \
            }                                                                                 \
            if (sizeof(pParams2->newListField) < listSize)                                    \
            {                                                                                 \
                NV_PRINTF(LEVEL_ERROR, "pParams2 static array too small\n");                  \
                portMemFree(pParams2);                                                        \
                ctrlparamRelease(pSecInfo, &token, pArgs);                                    \
                return NV_ERR_INSUFFICIENT_RESOURCES;                                         \
            }                                                                                 \
                                                                                              \
            if (set)                                                                          \
            {                                                                                 \
                do { customSet; } while(0);                                                   \
                NVMISC_MEMCPY(pParams2->newListField, (NvU8*)(NvUPtr)pParams->oldListField,   \
                              listSize);                                                      \
            }                                                                                 \
                                                                                              \
            status = pContextInternal->RmControl(pContextInternal, pArgs->hClient,            \
                                                 pArgs->hObject,                              \
                                                 newCmd,                                      \
                                                 pParams2,                                    \
                                                 sizeof(newParamsType));                      \
            if ((status == NV_OK) && get)                                                     \
            {                                                                                 \
                do { customGet; } while(0);                                                   \
                NVMISC_MEMCPY((NvU8*)(NvUPtr)pParams->oldListField, pParams2->newListField,   \
                              listSize);                                                      \
            }                                                                                 \
            portMemFree(pParams2);                                                            \
        }                                                                                     \
        ctrlparamRelease(pSecInfo, &token, pArgs);                                            \
        return status;                                                                        \
    } while (0)

//
// The default version will copy any fields from the V1 control to the same offset in the V2
// NOTE: This only works if the actual pointer/list field is the last field in the structure.
//       If that is not the case, use CONVERT_TO_V2_EX and write custom field copy logic
//
#define CONVERT_TO_V2(newCmd, oldParamsType, newParamsType, oldListField, newListField,       \
                      listSizeField, listSizeIsCount, listElemType, set, get)                 \
    CONVERT_TO_V2_EX(newCmd, oldParamsType, newParamsType, oldListField, newListField,        \
                     listSizeField, listSizeIsCount, listElemType, set, get,                  \
                     {                                                                        \
                         ct_assert(sizeof(*pParams) <= sizeof(*pParams2));                    \
                         NVMISC_MEMCPY(pParams2, pParams, sizeof(*pParams));                  \
                     },                                                                       \
                     {}                                                                       \
                    )


// TODO: Maybe move more of this to the macro above so we have a simple table, one row per V1->V2 converter.
static NV_STATUS V2_CONVERTER(_NV0000_CTRL_CMD_SYSTEM_GET_BUILD_VERSION)
(
    API_SECURITY_INFO *pSecInfo,
    DEPRECATED_CONTEXT *pContextInternal,
    NVOS54_PARAMETERS *pArgs
)
{
    NV_STATUS status;
    CONTROL_PARAM_TOKEN token;
    NV0000_CTRL_SYSTEM_GET_BUILD_VERSION_V2_PARAMS *pParams2;

    pParams2 = portMemAllocNonPaged(sizeof(*pParams2));

    if (pParams2 == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "No memory for pParams2\n");
        return NV_ERR_NO_MEMORY;
    }

    // First fetch build strings using the V2 ctrl call
    status = pContextInternal->RmControl(pContextInternal, pArgs->hClient,
                                         pArgs->hObject,
                                         NV0000_CTRL_CMD_SYSTEM_GET_BUILD_VERSION_V2,
                                         pParams2, sizeof(*pParams2));

    if (status == NV_OK)
    {
        //
        // Only use the deprecated copy-in macro's for the params struct, this control
        // call has some legacy behavior that does NOT return an error if one or more
        // of the embedded pointers is NULL
        //
        CTRL_PARAMS_TOKEN_INIT(token, NV0000_CTRL_SYSTEM_GET_BUILD_VERSION_PARAMS, pArgs);
        status = ctrlparamAcquire(pSecInfo, &token, pArgs);

        if (status == NV_OK)
        {
            NvU32 maxSizeOfStrings;
            NvU32 driverVersionBufferLen;
            NvU32 versionBufferLen;
            NvU32 titleBufferLen;
            NV0000_CTRL_SYSTEM_GET_BUILD_VERSION_PARAMS *pParams = NvP64_VALUE(pArgs->params);

            // Get the maximum string size as per legacy behavior
            driverVersionBufferLen = portStringLengthSafe(pParams2->driverVersionBuffer,
                                                          sizeof(pParams2->driverVersionBuffer)) + 1;
            versionBufferLen = portStringLengthSafe(pParams2->versionBuffer,
                                                    sizeof(pParams2->versionBuffer)) + 1;
            titleBufferLen = portStringLengthSafe(pParams2->titleBuffer,
                                                  sizeof(pParams2->titleBuffer)) + 1;

            maxSizeOfStrings = NV_MAX(driverVersionBufferLen,
                                      NV_MAX(versionBufferLen, titleBufferLen));
            
            //
            // In the case that one or more of the embedded pointers is NULL, the user
            // simply wants to know the maximum size of each of these strings so they know
            // how much memory to dynamically allocate.
            //
            if (NvP64_VALUE(pParams->pDriverVersionBuffer) == NULL ||
                NvP64_VALUE(pParams->pVersionBuffer) == NULL ||
                NvP64_VALUE(pParams->pTitleBuffer) == NULL)
            {
                pParams->sizeOfStrings = maxSizeOfStrings;
                portMemFree(pParams2);
                ctrlparamRelease(pSecInfo, &token, pArgs);
                return NV_OK;
            }

            // Embedded char pointers aren't NULL, perform the copy out
            else
            {
                // Ensure that the user-provided buffers are big enough
                if (pParams->sizeOfStrings < maxSizeOfStrings)
                {
                    portMemFree(pParams2);
                    ctrlparamRelease(pSecInfo, &token, pArgs);
                    return NV_ERR_INVALID_PARAM_STRUCT;
                }

                // Copy the build version info to the client's memory.
                if (rmapiParamsCopyOut(NULL,
                                       pParams2->driverVersionBuffer,
                                       pParams->pDriverVersionBuffer,
                                       driverVersionBufferLen,
                                       (pSecInfo->paramLocation != PARAM_LOCATION_KERNEL)) != NV_OK ||
                    rmapiParamsCopyOut(NULL,
                                       pParams2->versionBuffer,
                                       pParams->pVersionBuffer,
                                       versionBufferLen,
                                       (pSecInfo->paramLocation != PARAM_LOCATION_KERNEL)) != NV_OK ||
                    rmapiParamsCopyOut(NULL,
                                       pParams2->titleBuffer,
                                       pParams->pTitleBuffer,
                                       titleBufferLen,
                                       (pSecInfo->paramLocation != PARAM_LOCATION_KERNEL)) != NV_OK)
                {
                    NV_PRINTF(LEVEL_ERROR,
                              "Unable to copy out build version info to User Space.\n");
                    status = NV_ERR_INVALID_PARAM_STRUCT;
                }
                else
                {
                    // Copy out CL numbers
                    pParams->changelistNumber = pParams2->changelistNumber;
                    pParams->officialChangelistNumber = pParams2->officialChangelistNumber;
                    status = NV_OK;
                }
            }
        }

        // Free embedded pointer memory
        ctrlparamRelease(pSecInfo, &token, pArgs);
    }

    // Free allocated memory
    portMemFree(pParams2);
    return status;
}

static NV_STATUS V2_CONVERTER(_NV2080_CTRL_CMD_BIOS_GET_INFO)
(
    API_SECURITY_INFO *pSecInfo,
    DEPRECATED_CONTEXT *pContextInternal,
    NVOS54_PARAMETERS *pArgs
)
{
    CONVERT_TO_V2(NV2080_CTRL_CMD_BIOS_GET_INFO_V2,
                  NV2080_CTRL_BIOS_GET_INFO_PARAMS,
                  NV2080_CTRL_BIOS_GET_INFO_V2_PARAMS,
                  biosInfoList,
                  biosInfoList,
                  biosInfoListSize,
                  NV_TRUE, // List size is a count of elements
                  NV2080_CTRL_BIOS_INFO,
                  NV_TRUE, // Command SETs which info to fetch, do copy-in
                  NV_TRUE  // Command GETs info, do copy-out
                 );
}

static NV_STATUS V2_CONVERTER(_NV2080_CTRL_CMD_BUS_GET_INFO)
(
    API_SECURITY_INFO *pSecInfo,
    DEPRECATED_CONTEXT *pContextInternal,
    NVOS54_PARAMETERS *pArgs
)
{
    CONVERT_TO_V2(NV2080_CTRL_CMD_BUS_GET_INFO_V2,
                  NV2080_CTRL_BUS_GET_INFO_PARAMS,
                  NV2080_CTRL_BUS_GET_INFO_V2_PARAMS,
                  busInfoList,
                  busInfoList,
                  busInfoListSize,
                  NV_TRUE, // List size is a count of elements
                  NV2080_CTRL_BUS_INFO,
                  NV_TRUE, // Command SETs which info to fetch, do copy-in
                  NV_TRUE  // Command GETs info, do copy-out
                 );
}

// Note: see check "listItemCount == 0" in ctrlparamAcquire(). For capsTblSize
// of 0, it will let it fall thru to here. So we need to explicitly check for 
// capsTblSize here to not cause regression.
static NV_STATUS V2_CONVERTER(_NV0080_CTRL_CMD_BSP_GET_CAPS)
(
    API_SECURITY_INFO *pSecInfo,
    DEPRECATED_CONTEXT *pContextInternal,
    NVOS54_PARAMETERS *pArgs
)
{
    CONVERT_TO_V2_EX(NV0080_CTRL_CMD_BSP_GET_CAPS_V2,   // newCmd
                     NV0080_CTRL_BSP_GET_CAPS_PARAMS,   // oldParamsType
                     NV0080_CTRL_BSP_GET_CAPS_PARAMS_V2,// newParamsType
                     capsTbl,                           // oldListField
                     capsTbl,                           // newListField
                     capsTblSize,                       // listSizeField
                     NV_TRUE,                           // listSizeIsCount
                     NvU8,                              // listElemType
                     NV_TRUE, // for below custom to run, this must be TRUE.
                     NV_TRUE, // return path, ie, copy out back to user
                     // Custom data in from user
                     {
                        if (pParams->capsTblSize != NV0080_CTRL_BSP_CAPS_TBL_SIZE)
                        {
                            NV_PRINTF(LEVEL_ERROR, "pParams capsTblSize %d invalid\n",
                                   pParams->capsTblSize);
                            portMemFree(pParams2);
                            ctrlparamRelease(pSecInfo, &token, pArgs);
                            return NV_ERR_INVALID_ARGUMENT;
                        }
                        pParams2->instanceId = pParams->instanceId;
                     },
                     // Custom data out back to user
                     {}   // array has been filled. 
                    );
}

static NV_STATUS V2_CONVERTER(_NV2080_CTRL_CMD_GPU_GET_INFO)
(
    API_SECURITY_INFO *pSecInfo,
    DEPRECATED_CONTEXT *pContextInternal,
    NVOS54_PARAMETERS *pArgs
)
{
    CONVERT_TO_V2(NV2080_CTRL_CMD_GPU_GET_INFO_V2,
                  NV2080_CTRL_GPU_GET_INFO_PARAMS,
                  NV2080_CTRL_GPU_GET_INFO_V2_PARAMS,
                  gpuInfoList,
                  gpuInfoList,
                  gpuInfoListSize,
                  NV_TRUE, // List size is a count of elements
                  NV2080_CTRL_GPU_INFO,
                  NV_TRUE, // Command SETs which info to fetch, do copy-in
                  NV_TRUE  // Command GETs info, do copy-out
                 );
}

static NV_STATUS V2_CONVERTER(_NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES)
(
    API_SECURITY_INFO *pSecInfo,
    DEPRECATED_CONTEXT *pContextInternal,
    NVOS54_PARAMETERS *pArgs
)
{
    NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES_PARAMS *pParams = NULL;
    NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES_V2_PARAMS *pParams2 = NULL;
    NV_STATUS status;

    pParams = portMemAllocNonPaged(sizeof(*pParams));
    if (pParams == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "No memory for pParams\n");
        return NV_ERR_NO_MEMORY;
    }

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
        RmCopyUserForDeprecatedApi(RMAPI_DEPRECATED_COPYIN,
                                   RMAPI_DEPRECATED_BUFFER_EMPLACE,
                                   pArgs->params,
                                   sizeof(*pParams),
                                   (void**)&pParams,
                                   pSecInfo->paramLocation == PARAM_LOCATION_USER),
        done);

    pParams2 = portMemAllocNonPaged(sizeof(*pParams2));
    if (pParams2 == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "No memory for pParams2\n");
        status = NV_ERR_NO_MEMORY;
        goto done;
    }

    NVMISC_MEMSET(pParams2, 0, sizeof(*pParams2));

    pParams2->subDeviceInstance = pParams->subDeviceInstance;
    pParams2->displayId = pParams->displayId;
    pParams2->bEnableMSA = pParams->bEnableMSA;
    pParams2->bStereoPhaseInverse = pParams->bStereoPhaseInverse;
    pParams2->bCacheMsaOverrideForNextModeset = pParams->bCacheMsaOverrideForNextModeset;
    pParams2->featureMask = pParams->featureMask;
    pParams2->featureValues = pParams->featureValues;

    if (pParams->pFeatureDebugValues != NULL)
    {
        pParams2->bDebugValues = NV_TRUE;
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            RmCopyUserForDeprecatedApi(RMAPI_DEPRECATED_COPYIN,
                                       RMAPI_DEPRECATED_BUFFER_EMPLACE,
                                       pParams->pFeatureDebugValues,
                                       sizeof(pParams2->featureDebugValues),
                                       (void**)&pParams2->featureDebugValues,
                                       pSecInfo->paramLocation != PARAM_LOCATION_KERNEL),
            done);
    }
    status = pContextInternal->RmControl(pContextInternal, pArgs->hClient,
                                         pArgs->hObject,
                                         NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES_V2,
                                         pParams2,
                                         sizeof(NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES_V2_PARAMS));
    if (status == NV_OK)
    {
        if (pParams->pFeatureDebugValues != NULL)
        {
            NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                RmCopyUserForDeprecatedApi(RMAPI_DEPRECATED_COPYOUT,
                                           RMAPI_DEPRECATED_BUFFER_EMPLACE,
                                           pParams->pFeatureDebugValues,
                                           sizeof(pParams2->featureDebugValues),
                                           (void**)&pParams2->featureDebugValues,
                                           pSecInfo->paramLocation != PARAM_LOCATION_KERNEL),
                done);
        }

        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
             RmCopyUserForDeprecatedApi(RMAPI_DEPRECATED_COPYOUT,
                                        RMAPI_DEPRECATED_BUFFER_EMPLACE,
                                        pArgs->params,
                                        sizeof(*pParams),
                                        (void**)&pParams,
                                        pSecInfo->paramLocation == PARAM_LOCATION_USER),
             done);
    }

done:
    if (pParams2 != NULL)
    {
        portMemFree(pParams2);
    }
    if (pParams != NULL)
    {
        portMemFree(pParams);
    }

    return status;
}

static NV_STATUS V2_CONVERTER(_NV0080_CTRL_CMD_MSENC_GET_CAPS)
(
    API_SECURITY_INFO *pSecInfo,
    DEPRECATED_CONTEXT *pContextInternal,
    NVOS54_PARAMETERS *pArgs
)
{
    CONVERT_TO_V2_EX(NV0080_CTRL_CMD_MSENC_GET_CAPS_V2,   // newCmd
                     NV0080_CTRL_MSENC_GET_CAPS_PARAMS,   // oldParamsType
                     NV0080_CTRL_MSENC_GET_CAPS_V2_PARAMS,// newParamsType
                     capsTbl,                           // oldListField
                     capsTbl,                           // newListField
                     capsTblSize,                       // listSizeField
                     NV_TRUE,                           // listSizeIsCount
                     NvU8,                              // listElemType
                     NV_TRUE, // for below custom to run, this must be TRUE.
                     NV_TRUE, // return path, ie, copy out back to user
                     // Custom data in from user
                     {
                        if (pParams->capsTblSize != NV0080_CTRL_MSENC_CAPS_TBL_SIZE)
                        {
                            NV_PRINTF(LEVEL_ERROR, "pParams capsTblSize %d invalid\n",
                                       pParams->capsTblSize);
                            portMemFree(pParams2);
                            ctrlparamRelease(pSecInfo, &token, pArgs);
                            return NV_ERR_INVALID_ARGUMENT;
                        }
                        pParams2->instanceId = 0;
                     },
                     // Custom data out back to user
                     {}   // array has been filled.
                    );
}

static NV_STATUS V2_CONVERTER(_NV2080_CTRL_CMD_FB_GET_INFO)
(
    API_SECURITY_INFO *pSecInfo,
    DEPRECATED_CONTEXT *pContextInternal,
    NVOS54_PARAMETERS *pArgs
)
{
    CONVERT_TO_V2(NV2080_CTRL_CMD_FB_GET_INFO_V2,   // newCmd
                     NV2080_CTRL_FB_GET_INFO_PARAMS,   // oldParamsType
                     NV2080_CTRL_FB_GET_INFO_V2_PARAMS,// newParamsType
                     fbInfoList,                           // oldListField
                     fbInfoList,                           // newListField
                     fbInfoListSize,                       // listSizeField
                     NV_TRUE,                           // listSizeIsCount
                     NV2080_CTRL_FB_INFO,               // listElemType
                     NV_TRUE, // for below custom to run, this must be TRUE.
                     NV_TRUE // return path, ie, copy out back to user
                    );
}
