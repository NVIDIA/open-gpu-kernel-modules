/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "rmapi/rmapi.h"
#include "entry_points.h"
#include "deprecated_context.h"
#include "os/os.h"

#define RMAPI_DEPRECATED(pFunc, pArgs, bUserModeArgs) \
    do \
    { \
        DEPRECATED_CONTEXT_EXT context; \
        rmapiInitDeprecatedContext(&context, NULL, bUserModeArgs, NV_FALSE); \
        pFunc(&context.parent, pArgs); \
    } while (0)

#define RMAPI_NOT_SUPPORTED(pArgs) \
    pArgs->status = NV_ERR_NOT_SUPPORTED;

// Primary APIs
static void _nv04Alloc(NVOS21_PARAMETERS*, NvBool);
static void _nv01Free(NVOS00_PARAMETERS*, NvBool);
static void _nv04Control(NVOS54_PARAMETERS*, NvBool, NvBool);
static void _nv04DupObject(NVOS55_PARAMETERS*, NvBool);
static void _nv04Share(NVOS57_PARAMETERS*, NvBool);
static void _nv04MapMemory(NVOS33_PARAMETERS*, NvBool, NvBool);
static void _nv04UnmapMemory(NVOS34_PARAMETERS*, NvBool, NvBool);
static void _nv04MapMemoryDma(NVOS46_PARAMETERS*, NvBool);
static void _nv04UnmapMemoryDma(NVOS47_PARAMETERS*, NvBool);

// Legacy APIs
static void _nv01AllocMemory(NVOS02_PARAMETERS *pArgs, NvBool bUserModeArgs)                { RMAPI_DEPRECATED(RmDeprecatedAllocMemory, pArgs, bUserModeArgs); }
static void _nv01AllocObject(NVOS05_PARAMETERS *pArgs, NvBool bUserModeArgs)                { RMAPI_DEPRECATED(RmDeprecatedAllocObject, pArgs, bUserModeArgs); }
static void _nv04AddVblankCallback(NVOS61_PARAMETERS *pArgs, NvBool bUserModeArgs)          { RMAPI_DEPRECATED(RmDeprecatedAddVblankCallback, pArgs, bUserModeArgs); }
static void _nv04AllocContextDma(NVOS39_PARAMETERS *pArgs, NvBool bUserModeArgs)            { RMAPI_DEPRECATED(RmDeprecatedAllocContextDma, pArgs, bUserModeArgs); }
static void _nv04BindContextDma(NVOS49_PARAMETERS *pArgs, NvBool bUserModeArgs)             { RMAPI_DEPRECATED(RmDeprecatedBindContextDma, pArgs, bUserModeArgs); }
static void _nv04I2CAccess(NVOS_I2C_ACCESS_PARAMS *pArgs, NvBool bUserModeArgs)             { RMAPI_DEPRECATED(RmDeprecatedI2CAccess, pArgs, bUserModeArgs); }
static void _nv04IdleChannels(NVOS30_PARAMETERS *pArgs, NvBool bUserModeArgs)               { RMAPI_DEPRECATED(RmDeprecatedIdleChannels, pArgs, bUserModeArgs); }
static void _nv04VidHeapControl(NVOS32_PARAMETERS *pArgs, NvBool bUserModeArgs)             { RMAPI_DEPRECATED(RmDeprecatedVidHeapControl, pArgs, bUserModeArgs); }

static void _nv04AllocWithSecInfo(NVOS21_PARAMETERS*, API_SECURITY_INFO);
static void _nv04AllocWithAccessSecInfo(NVOS64_PARAMETERS*, API_SECURITY_INFO);
static void _nv04ControlWithSecInfo(NVOS54_PARAMETERS*, API_SECURITY_INFO, NvBool bInternalCall);
static void _nv01FreeWithSecInfo(NVOS00_PARAMETERS*, API_SECURITY_INFO);
static void _nv04AllocWithAccess(NVOS64_PARAMETERS*, NvBool);
static void _nv04MapMemoryWithSecInfo(NVOS33_PARAMETERS*, API_SECURITY_INFO);
static void _nv04UnmapMemoryWithSecInfo(NVOS34_PARAMETERS*, API_SECURITY_INFO);
static void _nv04MapMemoryDmaWithSecInfo(NVOS46_PARAMETERS*, API_SECURITY_INFO);
static void _nv04UnmapMemoryDmaWithSecInfo(NVOS47_PARAMETERS*, API_SECURITY_INFO);
static void _nv04DupObjectWithSecInfo(NVOS55_PARAMETERS*, API_SECURITY_INFO);
static void _nv04ShareWithSecInfo(NVOS57_PARAMETERS*, API_SECURITY_INFO);


//
// RM API entry points
//
// User mode clients should call base version (no suffix).
//
// Kernel mode clients should call Kernel or User version
// (call User if the parameters come from a user mode source).
//

void Nv01AllocMemory(NVOS02_PARAMETERS *pArgs)                      { _nv01AllocMemory(pArgs, NV_TRUE); }
void Nv01AllocObject(NVOS05_PARAMETERS *pArgs)                      { _nv01AllocObject(pArgs, NV_TRUE); }
void Nv01Free(NVOS00_PARAMETERS *pArgs)                             { _nv01Free(pArgs, NV_TRUE); }
void Nv04AddVblankCallback(NVOS61_PARAMETERS *pArgs)                { _nv04AddVblankCallback(pArgs, NV_TRUE); }
void Nv04Alloc(NVOS21_PARAMETERS *pArgs)                            { _nv04Alloc(pArgs, NV_TRUE); }
void Nv04AllocWithAccess(NVOS64_PARAMETERS *pArgs)                  { _nv04AllocWithAccess(pArgs, NV_TRUE); }
void Nv04AllocContextDma(NVOS39_PARAMETERS *pArgs)                  { _nv04AllocContextDma(pArgs, NV_TRUE); }
void Nv04BindContextDma(NVOS49_PARAMETERS *pArgs)                   { _nv04BindContextDma(pArgs, NV_TRUE); }
void Nv04Control(NVOS54_PARAMETERS  *pArgs)                         { _nv04Control(pArgs, NV_TRUE, NV_FALSE); }
void Nv04DupObject(NVOS55_PARAMETERS *pArgs)                        { _nv04DupObject(pArgs, NV_TRUE); }
void Nv04Share(NVOS57_PARAMETERS *pArgs)                            { _nv04Share(pArgs, NV_TRUE); }
void Nv04I2CAccess(NVOS_I2C_ACCESS_PARAMS  *pArgs)                  { _nv04I2CAccess(pArgs, NV_TRUE); }
void Nv04IdleChannels(NVOS30_PARAMETERS *pArgs)                     { _nv04IdleChannels(pArgs, NV_TRUE); }
void Nv04MapMemory(NVOS33_PARAMETERS *pArgs)                        { _nv04MapMemory(pArgs, NV_TRUE, NV_FALSE); }
void Nv04MapMemoryDma(NVOS46_PARAMETERS *pArgs)                     { _nv04MapMemoryDma(pArgs, NV_TRUE); }
void Nv04UnmapMemory(NVOS34_PARAMETERS *pArgs)                      { _nv04UnmapMemory(pArgs, NV_TRUE, NV_FALSE); }
void Nv04UnmapMemoryDma(NVOS47_PARAMETERS *pArgs)                   { _nv04UnmapMemoryDma(pArgs, NV_TRUE); }
void Nv04VidHeapControl(NVOS32_PARAMETERS *pArgs)                   { _nv04VidHeapControl(pArgs, NV_TRUE); }

void Nv01AllocMemoryUser(NVOS02_PARAMETERS *pArgs)                  { _nv01AllocMemory(pArgs, NV_TRUE); }
void Nv01AllocObjectUser(NVOS05_PARAMETERS *pArgs)                  { _nv01AllocObject(pArgs, NV_TRUE); }
void Nv01FreeUser(NVOS00_PARAMETERS *pArgs)                         { _nv01Free(pArgs, NV_TRUE); }
void Nv04AddVblankCallbackUser(NVOS61_PARAMETERS *pArgs)            { _nv04AddVblankCallback(pArgs, NV_TRUE); }
void Nv04AllocUser(NVOS21_PARAMETERS *pArgs)                        { _nv04Alloc(pArgs, NV_TRUE); }
void Nv04AllocWithAccessUser(NVOS64_PARAMETERS *pArgs)              { _nv04AllocWithAccess(pArgs, NV_TRUE); }
void Nv04AllocContextDmaUser(NVOS39_PARAMETERS *pArgs)              { _nv04AllocContextDma(pArgs, NV_TRUE); }
void Nv04BindContextDmaUser(NVOS49_PARAMETERS *pArgs)               { _nv04BindContextDma(pArgs, NV_TRUE); }
void Nv04ControlUser(NVOS54_PARAMETERS  *pArgs)                     { _nv04Control(pArgs, NV_TRUE, NV_FALSE); }
void Nv04DupObjectUser(NVOS55_PARAMETERS *pArgs)                    { _nv04DupObject(pArgs, NV_TRUE); }
void Nv04ShareUser(NVOS57_PARAMETERS *pArgs)                        { _nv04Share(pArgs, NV_TRUE); }
void Nv04I2CAccessUser(NVOS_I2C_ACCESS_PARAMS  *pArgs)              { _nv04I2CAccess(pArgs, NV_TRUE); }
void Nv04IdleChannelsUser(NVOS30_PARAMETERS *pArgs)                 { _nv04IdleChannels(pArgs, NV_TRUE); }
void Nv04MapMemoryUser(NVOS33_PARAMETERS *pArgs)                    { _nv04MapMemory(pArgs, NV_TRUE, NV_FALSE); }
void Nv04MapMemoryDmaUser(NVOS46_PARAMETERS *pArgs)                 { _nv04MapMemoryDma(pArgs, NV_TRUE); }
void Nv04UnmapMemoryUser(NVOS34_PARAMETERS *pArgs)                  { _nv04UnmapMemory(pArgs, NV_TRUE, NV_FALSE); }
void Nv04UnmapMemoryDmaUser(NVOS47_PARAMETERS *pArgs)               { _nv04UnmapMemoryDma(pArgs, NV_TRUE); }
void Nv04VidHeapControlUser(NVOS32_PARAMETERS *pArgs)               { _nv04VidHeapControl(pArgs, NV_TRUE); }

void Nv01AllocMemoryKernel(NVOS02_PARAMETERS *pArgs)                { _nv01AllocMemory(pArgs, NV_FALSE); }
void Nv01AllocObjectKernel(NVOS05_PARAMETERS *pArgs)                { _nv01AllocObject(pArgs, NV_FALSE); }
void Nv01FreeKernel(NVOS00_PARAMETERS *pArgs)                       { _nv01Free(pArgs, NV_FALSE); }
void Nv04AddVblankCallbackKernel(NVOS61_PARAMETERS *pArgs)          { _nv04AddVblankCallback(pArgs, NV_FALSE); }
void Nv04AllocKernel(NVOS21_PARAMETERS *pArgs)                      { _nv04Alloc(pArgs, NV_FALSE); }
void Nv04AllocWithAccessKernel(NVOS64_PARAMETERS *pArgs)            { _nv04AllocWithAccess(pArgs, NV_FALSE); }
void Nv04AllocContextDmaKernel(NVOS39_PARAMETERS *pArgs)            { _nv04AllocContextDma(pArgs, NV_FALSE); }
void Nv04BindContextDmaKernel(NVOS49_PARAMETERS *pArgs)             { _nv04BindContextDma(pArgs, NV_FALSE); }
void Nv04ControlKernel(NVOS54_PARAMETERS  *pArgs)                   { _nv04Control(pArgs, NV_FALSE, NV_FALSE); }
void Nv04DupObjectKernel(NVOS55_PARAMETERS *pArgs)                  { _nv04DupObject(pArgs, NV_FALSE); }
void Nv04ShareKernel(NVOS57_PARAMETERS *pArgs)                      { _nv04Share(pArgs, NV_FALSE); }
void Nv04I2CAccessKernel(NVOS_I2C_ACCESS_PARAMS  *pArgs)            { _nv04I2CAccess(pArgs, NV_FALSE); }
void Nv04IdleChannelsKernel(NVOS30_PARAMETERS *pArgs)               { _nv04IdleChannels(pArgs, NV_FALSE); }
void Nv04MapMemoryKernel(NVOS33_PARAMETERS *pArgs)                  { _nv04MapMemory(pArgs, NV_FALSE, NV_FALSE); }
void Nv04MapMemoryDmaKernel(NVOS46_PARAMETERS *pArgs)               { _nv04MapMemoryDma(pArgs, NV_FALSE); }
void Nv04UnmapMemoryKernel(NVOS34_PARAMETERS *pArgs)                { _nv04UnmapMemory(pArgs, NV_FALSE, NV_FALSE); }
void Nv04UnmapMemoryDmaKernel(NVOS47_PARAMETERS *pArgs)             { _nv04UnmapMemoryDma(pArgs, NV_FALSE); }
void Nv04VidHeapControlKernel(NVOS32_PARAMETERS *pArgs)             { _nv04VidHeapControl(pArgs, NV_FALSE); }

// MODS-specific API functions which ignore RM locking model
#if defined(LINUX_MFG)
void Nv04ControlInternal(NVOS54_PARAMETERS  *pArgs)                 { _nv04Control(pArgs, NV_FALSE, NV_TRUE); }
void Nv04MapMemoryInternal(NVOS33_PARAMETERS *pArgs)                { _nv04MapMemory(pArgs, NV_FALSE, NV_TRUE); }
void Nv04UnmapMemoryInternal(NVOS34_PARAMETERS *pArgs)              { _nv04UnmapMemory(pArgs, NV_FALSE, NV_TRUE); }
#endif

#define RMAPI_DEPRECATED_WITH_SECINFO(pFunc, pArgs, secInfo)                            \
    do                                                                                  \
    {                                                                                   \
        DEPRECATED_CONTEXT_EXT context;                                                 \
        rmapiInitDeprecatedContext(&context, &(secInfo),                                \
                                   (secInfo).paramLocation != PARAM_LOCATION_KERNEL,    \
                                   NV_FALSE);                                           \
        (pFunc)(&context.parent, (pArgs));                                              \
    } while (0)

void Nv01AllocMemoryWithSecInfo(NVOS02_PARAMETERS *pArgs, API_SECURITY_INFO secInfo)         { RMAPI_DEPRECATED_WITH_SECINFO(RmDeprecatedAllocMemory, pArgs, secInfo); }
void Nv01AllocObjectWithSecInfo(NVOS05_PARAMETERS *pArgs, API_SECURITY_INFO secInfo)         { RMAPI_DEPRECATED_WITH_SECINFO(RmDeprecatedAllocObject, pArgs, secInfo); }
void Nv04AllocWithSecInfo(NVOS21_PARAMETERS *pArgs, API_SECURITY_INFO secInfo)               { _nv04AllocWithSecInfo(pArgs, secInfo); }
void Nv04AllocWithAccessSecInfo(NVOS64_PARAMETERS *pArgs, API_SECURITY_INFO secInfo)         { _nv04AllocWithAccessSecInfo(pArgs, secInfo); }
void Nv01FreeWithSecInfo(NVOS00_PARAMETERS *pArgs, API_SECURITY_INFO secInfo)                { _nv01FreeWithSecInfo(pArgs, secInfo); }
void Nv04ControlWithSecInfo(NVOS54_PARAMETERS  *pArgs, API_SECURITY_INFO secInfo)            { _nv04ControlWithSecInfo(pArgs, secInfo, NV_FALSE); }
void Nv04VidHeapControlWithSecInfo(NVOS32_PARAMETERS *pArgs, API_SECURITY_INFO secInfo)      { RMAPI_DEPRECATED_WITH_SECINFO(RmDeprecatedVidHeapControl, pArgs, secInfo); }
void Nv04IdleChannelsWithSecInfo(NVOS30_PARAMETERS *pArgs, API_SECURITY_INFO secInfo)        { RMAPI_DEPRECATED_WITH_SECINFO(RmDeprecatedIdleChannels, pArgs, secInfo); }
void Nv04MapMemoryWithSecInfo(NVOS33_PARAMETERS *pArgs, API_SECURITY_INFO secInfo)           { _nv04MapMemoryWithSecInfo(pArgs, secInfo); }
void Nv04UnmapMemoryWithSecInfo(NVOS34_PARAMETERS *pArgs, API_SECURITY_INFO secInfo)         { _nv04UnmapMemoryWithSecInfo(pArgs, secInfo); }
void Nv04I2CAccessWithSecInfo(NVOS_I2C_ACCESS_PARAMS *pArgs, API_SECURITY_INFO secInfo)      { RMAPI_DEPRECATED_WITH_SECINFO(RmDeprecatedI2CAccess, pArgs, secInfo); }
void Nv04AllocContextDmaWithSecInfo(NVOS39_PARAMETERS *pArgs, API_SECURITY_INFO secInfo)     { RMAPI_DEPRECATED_WITH_SECINFO(RmDeprecatedAllocContextDma, pArgs, secInfo); }
void Nv04BindContextDmaWithSecInfo(NVOS49_PARAMETERS *pArgs, API_SECURITY_INFO secInfo)      { RMAPI_DEPRECATED_WITH_SECINFO(RmDeprecatedBindContextDma, pArgs, secInfo); }
void Nv04MapMemoryDmaWithSecInfo(NVOS46_PARAMETERS *pArgs, API_SECURITY_INFO secInfo)        { _nv04MapMemoryDmaWithSecInfo(pArgs, secInfo); }
void Nv04UnmapMemoryDmaWithSecInfo(NVOS47_PARAMETERS *pArgs, API_SECURITY_INFO secInfo)      { _nv04UnmapMemoryDmaWithSecInfo(pArgs, secInfo); }
void Nv04DupObjectWithSecInfo(NVOS55_PARAMETERS *pArgs, API_SECURITY_INFO secInfo)           { _nv04DupObjectWithSecInfo(pArgs, secInfo); }
void Nv04ShareWithSecInfo(NVOS57_PARAMETERS *pArgs, API_SECURITY_INFO secInfo)               { _nv04ShareWithSecInfo(pArgs, secInfo); }


static void
XlateUserModeArgsToSecInfo
(
    NvBool             bUserModeArgs,
    NvBool             bInternalCall,
    API_SECURITY_INFO *pSecInfo
)
{
    portMemSet(pSecInfo, 0, sizeof(*pSecInfo));

    if (bInternalCall == NV_FALSE && bUserModeArgs == NV_TRUE)
    {
        pSecInfo->privLevel     = osIsAdministrator() ? RS_PRIV_LEVEL_USER_ROOT : RS_PRIV_LEVEL_USER;
        pSecInfo->paramLocation = PARAM_LOCATION_USER;
    }
    else
    {
        pSecInfo->privLevel     = RS_PRIV_LEVEL_KERNEL;
        pSecInfo->paramLocation = PARAM_LOCATION_KERNEL;
    }
}

/*
NV04_ALLOC
    NVOS21_PARAMETERS;
        NvHandle hRoot;
        NvHandle hObjectParent;
        NvHandle hObjectNew;
        NvV32    hClass;
        NvP64    pAllocParms;
        NvV32    status;
*/

static void _nv04Alloc
(
    NVOS21_PARAMETERS *pArgs,
    NvBool             bUserModeArgs
)
{
    RM_API            *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL);
    API_SECURITY_INFO  secInfo;

    XlateUserModeArgsToSecInfo(bUserModeArgs, NV_FALSE, &secInfo);

    pArgs->status = pRmApi->AllocWithSecInfo(pRmApi, pArgs->hRoot, pArgs->hObjectParent, &pArgs->hObjectNew,
                                             pArgs->hClass, pArgs->pAllocParms, pArgs->paramsSize, RMAPI_ALLOC_FLAGS_NONE,
                                             NvP64_NULL, &secInfo);
} // end of Nv04Alloc()

/*
NV04_ALLOC
    NVOS21_PARAMETERS;
        NvHandle hRoot;
        NvHandle hObjectParent;
        NvHandle hObjectNew;
        NvV32    hClass;
        NvP64    pAllocParms;
        NvV32    status;
*/

static void _nv04AllocWithSecInfo
(
    NVOS21_PARAMETERS *pArgs,
    API_SECURITY_INFO secInfo
)
{
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL);

    pArgs->status = pRmApi->AllocWithSecInfo(pRmApi, pArgs->hRoot, pArgs->hObjectParent, &pArgs->hObjectNew,
                                             pArgs->hClass, pArgs->pAllocParms, pArgs->paramsSize, RMAPI_ALLOC_FLAGS_NONE,
                                             NvP64_NULL, &secInfo);
} // end of _nv04AllocWithSecInfo()

/*
NV04_ALLOC_WITH_ACCESS
    NVOS64_PARAMETERS;
        NvHandle hRoot;
        NvHandle hObjectParent;
        NvHandle hObjectNew;
        NvV32    hClass;
        NvP64    pAllocParms;
        NvP64    pRightsRequested;
        NvV32    status;
*/

static void _nv04AllocWithAccess
(
    NVOS64_PARAMETERS *pArgs,
    NvBool             bUserModeArgs
)
{
    RM_API            *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL);
    NvU32              flags = RMAPI_ALLOC_FLAGS_NONE;
    API_SECURITY_INFO  secInfo;

    XlateUserModeArgsToSecInfo(bUserModeArgs, NV_FALSE, &secInfo);

    if (pArgs->flags & NVOS64_FLAGS_FINN_SERIALIZED)
        flags |= RMAPI_ALLOC_FLAGS_SERIALIZED;

    pArgs->status = pRmApi->AllocWithSecInfo(pRmApi, pArgs->hRoot, pArgs->hObjectParent, &pArgs->hObjectNew,
                                             pArgs->hClass, pArgs->pAllocParms, pArgs->paramsSize, flags,
                                             pArgs->pRightsRequested, &secInfo);
} // end of _nv04AllocWithAccess()

static void _nv04AllocWithAccessSecInfo
(
    NVOS64_PARAMETERS *pArgs,
    API_SECURITY_INFO secInfo
)
{
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL);
    NvU32   flags = RMAPI_ALLOC_FLAGS_NONE;

    if (pArgs->flags & NVOS64_FLAGS_FINN_SERIALIZED)
        flags |= RMAPI_ALLOC_FLAGS_SERIALIZED;

    pArgs->status = pRmApi->AllocWithSecInfo(pRmApi, pArgs->hRoot, pArgs->hObjectParent, &pArgs->hObjectNew,
                                             pArgs->hClass, pArgs->pAllocParms, pArgs->paramsSize, flags,
                                             pArgs->pRightsRequested, &secInfo);
} // end of _nv04AllocWithAccessSecInfo()

/*
NV01_FREE
    NVOS00_PARAMETERS:
        NvHandle hRoot;
        NvHandle hObjectParent;
        NvHandle hObjectOld;
        NvV32    status;
*/

static void _nv01Free
(
    NVOS00_PARAMETERS *pArgs,
    NvBool             bUserModeArgs
)
{
    RM_API            *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL);
    API_SECURITY_INFO  secInfo;

    XlateUserModeArgsToSecInfo(bUserModeArgs, NV_FALSE, &secInfo);

    pArgs->status = pRmApi->FreeWithSecInfo(pRmApi, pArgs->hRoot, pArgs->hObjectOld, RMAPI_FREE_FLAGS_NONE, &secInfo);
} // end of Nv01Free()

/*
NV01_FREE
    NVOS00_PARAMETERS:
        NvHandle hRoot;
        NvHandle hObjectParent;
        NvHandle hObjectOld;
        NvV32    status;
*/

static void _nv01FreeWithSecInfo
(
    NVOS00_PARAMETERS *pArgs,
    API_SECURITY_INFO  secInfo
)
{
    RM_API            *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL);

    pArgs->status = pRmApi->FreeWithSecInfo(pRmApi, pArgs->hRoot, pArgs->hObjectOld, RMAPI_FREE_FLAGS_NONE, &secInfo);
} // end of Nv01FreeWithSecInfo()

/*
NV04_MAP_MEMORY
    NVOS33_PARAMETERS:
        NvHandle hClient;
        NvHandle hDevice;
        NvHandle hMemory;
        NvU64    offset;
        NvU64    length;
        NvP64    pLinearAddress;
        NvU32    status;
        NvU32    flags;
*/
static void _nv04MapMemory
(
    NVOS33_PARAMETERS *pArgs,
    NvBool             bUserModeArgs,
    NvBool             bInternalCall
)
{
    RM_API             *pRmApi = rmapiGetInterface(bInternalCall ? RMAPI_MODS_LOCK_BYPASS : RMAPI_EXTERNAL);
    API_SECURITY_INFO  secInfo;

    XlateUserModeArgsToSecInfo(bUserModeArgs, NV_FALSE, &secInfo);

    pArgs->status = pRmApi->MapToCpuWithSecInfo(pRmApi, pArgs->hClient, pArgs->hDevice, pArgs->hMemory, pArgs->offset,
                                                pArgs->length, &pArgs->pLinearAddress, pArgs->flags, &secInfo);
} // end of Nv04MapMemory()

static void _nv04MapMemoryWithSecInfo
(
    NVOS33_PARAMETERS *pArgs,
    API_SECURITY_INFO  secInfo
)
{
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL);

    pArgs->status = pRmApi->MapToCpuWithSecInfoV2(pRmApi, pArgs->hClient, pArgs->hDevice, pArgs->hMemory, pArgs->offset,
                                                  pArgs->length, &pArgs->pLinearAddress, &pArgs->flags, &secInfo);
}

/*
NV04_UNMAP_MEMORY
    NVOS34_PARAMETERS:
        NvHandle    hClient;
        NvHandle    hDevice;
        NvHandle    hMemory;
        NvP64       pLinearAddress;
        NvU32       status;
        NvU32       flags;
*/
static void _nv04UnmapMemory
(
    NVOS34_PARAMETERS *pArgs,
    NvBool             bUserModeArgs,
    NvBool             bInternalCall
)
{
    RM_API             *pRmApi = rmapiGetInterface(bInternalCall ? RMAPI_MODS_LOCK_BYPASS : RMAPI_EXTERNAL);
    API_SECURITY_INFO  secInfo;

    XlateUserModeArgsToSecInfo(bUserModeArgs, NV_FALSE, &secInfo);

    pArgs->status = pRmApi->UnmapFromCpuWithSecInfo(pRmApi, pArgs->hClient, pArgs->hDevice, pArgs->hMemory,
                                                    pArgs->pLinearAddress, pArgs->flags, osGetCurrentProcess(), &secInfo);
} // end of Nv04UnmapMemory()

static void _nv04UnmapMemoryWithSecInfo
(
    NVOS34_PARAMETERS *pArgs,
    API_SECURITY_INFO  secInfo
)
{
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL);

    pArgs->status = pRmApi->UnmapFromCpuWithSecInfo(pRmApi, pArgs->hClient, pArgs->hDevice, pArgs->hMemory,
                                                    pArgs->pLinearAddress, pArgs->flags, osGetCurrentProcess(), &secInfo);
}

static void _nv04MapMemoryDma
(
    NVOS46_PARAMETERS *pArgs,
    NvBool             bUserModeArgs
)
{

    RM_API             *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL);
    API_SECURITY_INFO  secInfo;

    XlateUserModeArgsToSecInfo(bUserModeArgs, NV_FALSE, &secInfo);

    pArgs->status = pRmApi->MapWithSecInfo(pRmApi, pArgs->hClient, pArgs->hDevice, pArgs->hDma,
                                           pArgs->hMemory, pArgs->offset, pArgs->length, pArgs->flags,
                                           &pArgs->dmaOffset, &secInfo);
} // end of Nv04MapMemoryDma()

static void _nv04MapMemoryDmaWithSecInfo
(
    NVOS46_PARAMETERS *pArgs,
    API_SECURITY_INFO  secInfo
)
{

    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL);

    pArgs->status = pRmApi->MapWithSecInfo(pRmApi, pArgs->hClient, pArgs->hDevice, pArgs->hDma,
                                           pArgs->hMemory, pArgs->offset, pArgs->length, pArgs->flags,
                                           &pArgs->dmaOffset, &secInfo);
}

/*
NV04_UNMAP_MEMORY_DMA
    NVOS47_PARAMETERS:
        NvHandle    hClient;
        NvHandle    hDevice;
        NvHandle    hDma;
        NvHandle    hMemory;
        NvV32       flags;
        NvU64       dmaOffset;
        NvV32       status;
*/
static void _nv04UnmapMemoryDma
(
    NVOS47_PARAMETERS *pArgs,
    NvBool             bUserModeArgs
)
{

    RM_API             *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL);
    API_SECURITY_INFO  secInfo;

    XlateUserModeArgsToSecInfo(bUserModeArgs, NV_FALSE, &secInfo);

    pArgs->status = pRmApi->UnmapWithSecInfo(pRmApi, pArgs->hClient, pArgs->hDevice, pArgs->hDma,
                                             pArgs->flags, pArgs->dmaOffset, pArgs->size, &secInfo);
} // end of Nv04UnmapMemoryDma()

static void _nv04UnmapMemoryDmaWithSecInfo
(
    NVOS47_PARAMETERS *pArgs,
    API_SECURITY_INFO  secInfo
)
{

    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL);

    pArgs->status = pRmApi->UnmapWithSecInfo(pRmApi, pArgs->hClient, pArgs->hDevice, pArgs->hDma,
                                             pArgs->flags, pArgs->dmaOffset, pArgs->size, &secInfo);
}

/*
NV04_CONTROL
    NVOS54_PARAMETERS:
        NvHandle hClient;
        NvHandle hObject;
        NvV32    cmd;
        NvP64    params;
        NvU32    paramsSize;
        NvV32    status;
*/
static void _nv04ControlWithSecInfo
(
    NVOS54_PARAMETERS *pArgs,
    API_SECURITY_INFO  secInfo,
    NvBool             bInternalCall
)
{
    RmDeprecatedControlHandler pDeprecatedHandler = RmDeprecatedGetControlHandler(pArgs);
    if (pDeprecatedHandler != NULL)
    {
        DEPRECATED_CONTEXT_EXT ctxGraveyard;
        rmapiInitDeprecatedContext(&ctxGraveyard, &secInfo, NV_FALSE, NV_FALSE);
        pArgs->status = pDeprecatedHandler(&secInfo, &ctxGraveyard.parent, pArgs);
    }
    else
    {
        RM_API *pRmApi = rmapiGetInterface(bInternalCall ? RMAPI_MODS_LOCK_BYPASS : RMAPI_EXTERNAL);

        pArgs->status = pRmApi->ControlWithSecInfo(pRmApi, pArgs->hClient, pArgs->hObject, pArgs->cmd,
                                                  pArgs->params, pArgs->paramsSize, pArgs->flags, &secInfo);
    }
} // end of Nv04Control()

/*
NV04_CONTROL
    NVOS54_PARAMETERS:
        NvHandle hClient;
        NvHandle hObject;
        NvV32    cmd;
        NvP64    params;
        NvU32    paramsSize;
        NvV32    status;
*/
static void _nv04Control
(
    NVOS54_PARAMETERS *pArgs,
    NvBool             bUserModeArgs,
    NvBool             bInternalCall
)
{
    API_SECURITY_INFO  secInfo = {0};
    XlateUserModeArgsToSecInfo(bUserModeArgs, bInternalCall, &secInfo);
    _nv04ControlWithSecInfo(pArgs, secInfo, bInternalCall);
} // end of Nv04Control()

/*
NV04_DUP_OBJECT
    NVOS55_PARAMETERS:
        NvHandle hClient;
        NvHandle hParent;
        NvHandle hObject;
        NvHandle hClientSrc;
        NvHandle hObjectSrc;
        NvU32    flags;
        NvU32    status;
*/
static void _nv04DupObject
(
    NVOS55_PARAMETERS *pArgs,
    NvBool             bUserModeArgs
)
{
    RM_API            *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL);
    API_SECURITY_INFO  secInfo;

    XlateUserModeArgsToSecInfo(bUserModeArgs, NV_FALSE, &secInfo);

    pArgs->status = pRmApi->DupObjectWithSecInfo(pRmApi, pArgs->hClient, pArgs->hParent, &pArgs->hObject,
                                                 pArgs->hClientSrc, pArgs->hObjectSrc, pArgs->flags, &secInfo);
} // end of Nv04DupObject()

static void _nv04DupObjectWithSecInfo
(
    NVOS55_PARAMETERS *pArgs,
    API_SECURITY_INFO  secInfo
)
{
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL);

    pArgs->status = pRmApi->DupObjectWithSecInfo(pRmApi, pArgs->hClient, pArgs->hParent, &pArgs->hObject,
                                                 pArgs->hClientSrc, pArgs->hObjectSrc, pArgs->flags, &secInfo);
}

static void _nv04Share
(
    NVOS57_PARAMETERS *pArgs,
    NvBool bUserModeArgs
)
{
    RM_API            *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL);
    API_SECURITY_INFO  secInfo;

    XlateUserModeArgsToSecInfo(bUserModeArgs, NV_FALSE, &secInfo);

    pArgs->status = pRmApi->ShareWithSecInfo(pRmApi, pArgs->hClient, pArgs->hObject,
                                             &pArgs->sharePolicy, &secInfo);
} // end of Nv04Share()

static void _nv04ShareWithSecInfo
(
    NVOS57_PARAMETERS *pArgs,
    API_SECURITY_INFO  secInfo
)
{
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL);

    pArgs->status = pRmApi->ShareWithSecInfo(pRmApi, pArgs->hClient, pArgs->hObject,
                                             &pArgs->sharePolicy, &secInfo);
}
