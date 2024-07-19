/*
 * SPDX-FileCopyrightText: Copyright (c) 2005-2017 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */



#include "nvidia-3d.h"
#include "nvidia-3d-surface.h"
#include "nvidia-3d-types-priv.h"

#include "nvidia-3d-fermi.h"
#include "nvidia-3d-kepler.h"
#include "nvidia-3d-maxwell.h"
#include "nvidia-3d-pascal.h"
#include "nvidia-3d-volta.h"
#include "nvidia-3d-turing.h"
#include "nvidia-3d-hopper.h"

#include "nvidia-push-init.h" // nvPushGetSupportedClassIndex()
#include "nvidia-push-utils.h" // nvPushIsAmodel()

#include <class/clcd97.h> // BLACKWELL_A
#include <class/clcb97.h> // HOPPER_A
#include <class/clc997.h> // ADA_A
#include <class/clc797.h> // AMPERE_B
#include <class/clc697.h> // AMPERE_A
#include <class/clc597.h> // TURING_A
#include <class/clc397.h> // VOLTA_A
#include <class/clc197.h> // PASCAL_B
#include <class/clc097.h> // PASCAL_A
#include <class/clb197.h> // MAXWELL_B
#include <class/clb097.h> // MAXWELL_A

#include <ctrl/ctrl2080/ctrl2080gr.h>
#include <nvos.h>

#include "g_maxwell_shader_info.h"
#include "g_pascal_shader_info.h"
#include "g_volta_shader_info.h"
#include "g_turing_shader_info.h"
#include "g_ampere_shader_info.h"
#include "g_hopper_shader_info.h"
#include "g_blackwell_shader_info.h"

#define _NV3D_CHANNEL_PROGRAMS_ENTRY(_archLower, _archCamel, _archUpper) \
    [NV3D_SHADER_ARCH_ ## _archUpper ] = {                               \
        .num                   = NUM_PROGRAMS,                           \
        .info                  = _archCamel ## ProgramInfo,              \
        .maxLocalBytes         = _archCamel ## ShaderMaxLocalBytes,      \
        .maxStackBytes         = _archCamel ## ShaderMaxStackBytes,      \
        .code.decompressedSize = _archCamel ## ProgramHeapSize,          \
        .code.compressedStart  =                                         \
            ({ extern const unsigned char                                \
                    _binary_ ## _archLower ## _shaders_xz_start[];       \
                _binary_ ## _archLower ## _shaders_xz_start; }),         \
        .code.compressedEnd    =                                         \
            ({ extern const unsigned char                                \
                    _binary_ ## _archLower ## _shaders_xz_end[];         \
                _binary_ ## _archLower ## _shaders_xz_end; }),           \
        .constants.info        = _archCamel ## ConstBufInfo,             \
        .constants.count       =                                         \
            (NvU32)ARRAY_LEN(_archCamel ## ConstBufInfo),                \
        .constants.size        = _archCamel ## ConstBufSize,             \
        .constants.sizeAlign   = _archCamel ## ConstBufSizeAlign,        \
    }

static Nv3dChannelProgramsRec PickProgramsRec(
    const Nv3dDeviceRec *p3dDevice)
{
    const Nv3dChannelProgramsRec programsTable[NV3D_SHADER_ARCH_COUNT] = {

        _NV3D_CHANNEL_PROGRAMS_ENTRY(maxwell, Maxwell, MAXWELL),
        _NV3D_CHANNEL_PROGRAMS_ENTRY(pascal, Pascal, PASCAL),
        _NV3D_CHANNEL_PROGRAMS_ENTRY(volta, Volta, VOLTA),
        _NV3D_CHANNEL_PROGRAMS_ENTRY(turing, Turing, TURING),
        _NV3D_CHANNEL_PROGRAMS_ENTRY(ampere, Ampere, AMPERE),
        _NV3D_CHANNEL_PROGRAMS_ENTRY(hopper, Hopper, HOPPER),
        _NV3D_CHANNEL_PROGRAMS_ENTRY(blackwell, Blackwell, BLACKWELL),
    };

    return programsTable[p3dDevice->shaderArch];
}

#undef _NV3D_CHANNEL_PROGRAMS_ENTRY


static NvBool QueryThreadsAndWarpsOneSd(
    Nv3dDevicePtr p3dDevice,
    NvU32 sd,
    NvU32 *pMaxWarps,
    NvU32 *pThreadsPerWarp)
{
    NvPushDevicePtr pPushDevice = p3dDevice->pPushDevice;
    const NvPushImports *pImports = pPushDevice->pImports;
    NvU32 ret;

    NV2080_CTRL_GR_GET_INFO_PARAMS grInfoParams = { 0 };
    struct {
        NV2080_CTRL_GR_INFO numSMs;
        NV2080_CTRL_GR_INFO maxWarpsPerSM;
        NV2080_CTRL_GR_INFO threadsPerWarp;
    } grInfo;

    NVMISC_MEMSET(&grInfo, 0, sizeof(grInfo));

    grInfo.numSMs.index =
        NV2080_CTRL_GR_INFO_INDEX_THREAD_STACK_SCALING_FACTOR;
    grInfo.maxWarpsPerSM.index =
        NV2080_CTRL_GR_INFO_INDEX_MAX_WARPS_PER_SM;
    grInfo.threadsPerWarp.index =
        NV2080_CTRL_GR_INFO_INDEX_MAX_THREADS_PER_WARP;

    grInfoParams.grInfoListSize =
        sizeof(grInfo) / sizeof(NV2080_CTRL_GR_INFO);

    grInfoParams.grInfoList = NV_PTR_TO_NvP64(&grInfo);

    ret = pImports->rmApiControl(pPushDevice,
                                 pPushDevice->subDevice[sd].handle,
                                 NV2080_CTRL_CMD_GR_GET_INFO,
                                 &grInfoParams,
                                 sizeof(grInfoParams));

    if (ret != NVOS_STATUS_SUCCESS) {
        return FALSE;
    }

    *pMaxWarps = grInfo.numSMs.data * grInfo.maxWarpsPerSM.data;
    *pThreadsPerWarp = grInfo.threadsPerWarp.data;

    return TRUE;
}

static NvBool GetMaxThreadsAndWarps(Nv3dDevicePtr p3dDevice)
{
    NvU32 sd;

    p3dDevice->maxThreadsPerWarp = 0;
    p3dDevice->maxWarps = 0;

    for (sd = 0; sd < p3dDevice->pPushDevice->numSubDevices; sd++) {

        NvU32 maxWarps, threadsPerWarp;

        if (!QueryThreadsAndWarpsOneSd(p3dDevice, sd,
                                       &maxWarps, &threadsPerWarp)) {
            return FALSE;
        }

        p3dDevice->maxThreadsPerWarp =
            NV_MAX(p3dDevice->maxThreadsPerWarp, threadsPerWarp);

        p3dDevice->maxWarps = NV_MAX(p3dDevice->maxWarps, maxWarps);
    }

    return TRUE;
}

/*!
 * Get the SM version reported by resman.
 *
 * \params  pPushDevice  The nvidia-push device corresponding to the GPU.
 *
 * \return  The SM version of this device.
 */
static NvU32 GetSmVersion(
    NvPushDevicePtr pPushDevice)
{
    NvU32 sd, smVersion = NV2080_CTRL_GR_INFO_SM_VERSION_NONE;

    if (nvPushIsAModel(pPushDevice)) {
        /*
         * On amodel resman cannot tell us the SM version, so we pick
         * the SM version based on NVAModelConfig.
         */
        static const NvU32 table[] = {
            [NV_AMODEL_MAXWELL]     = NV2080_CTRL_GR_INFO_SM_VERSION_5_0,
            [NV_AMODEL_PASCAL]      = NV2080_CTRL_GR_INFO_SM_VERSION_6_0,
            [NV_AMODEL_VOLTA]       = NV2080_CTRL_GR_INFO_SM_VERSION_7_0,
            [NV_AMODEL_TURING]      = NV2080_CTRL_GR_INFO_SM_VERSION_7_5,
            [NV_AMODEL_AMPERE]      = NV2080_CTRL_GR_INFO_SM_VERSION_8_2,
            [NV_AMODEL_ADA]         = NV2080_CTRL_GR_INFO_SM_VERSION_8_9,
            [NV_AMODEL_HOPPER]      = NV2080_CTRL_GR_INFO_SM_VERSION_9_0,
            [NV_AMODEL_BLACKWELL]   = NV2080_CTRL_GR_INFO_SM_VERSION_10_0,
        };

        if (pPushDevice->amodelConfig >= ARRAY_LEN(table)) {
            return NV2080_CTRL_GR_INFO_SM_VERSION_NONE;
        }

        return table[pPushDevice->amodelConfig];
    }

    /*
     * Query the SM version from resman.  This query is per-subDevice,
     * but we use SM version per-device, so assert that the SM version
     * matches across subDevices.
     */
    for (sd = 0; sd < pPushDevice->numSubDevices; sd++) {

        const NvPushImports *pImports = pPushDevice->pImports;
        NV2080_CTRL_GR_GET_INFO_PARAMS params = { };
        NV2080_CTRL_GR_INFO smVersionParams = { };
        NvU32 ret;

        smVersionParams.index = NV2080_CTRL_GR_INFO_INDEX_SM_VERSION;
        params.grInfoListSize = 1;
        params.grInfoList = NV_PTR_TO_NvP64(&smVersionParams);

        ret = pImports->rmApiControl(pPushDevice,
                                     pPushDevice->subDevice[sd].handle,
                                     NV2080_CTRL_CMD_GR_GET_INFO,
                                     &params,
                                     sizeof(params));

        if (ret != NVOS_STATUS_SUCCESS) {
            return NV2080_CTRL_GR_INFO_SM_VERSION_NONE;
        }

        if (sd == 0) {
            smVersion = smVersionParams.data;
        } else {
            nvAssert(smVersion == smVersionParams.data);
        }
    }

    return smVersion;
}

/*!
 * Get the SPA version to use with the 3D Class.
 *
 * Note that resman only reports the SM version (the "hardware
 * revision"), not the SPA version (the ISA version).  So we use a
 * table to map from SM version to SPA version.
 *
 * \params  pPushDevice  The nvidia-push device corresponding to the GPU.
 * \params  pSpaVersion  The spaVersion to assign.
 *
 * \return  TRUE if the SPA version could be assigned.
 */
static NvBool GetSpaVersion(
    NvPushDevicePtr pPushDevice,
    Nv3dDeviceSpaVersionRec *pSpaVersion)
{
    static const struct {
        NvU32 smVersion;
        Nv3dDeviceSpaVersionRec spaVersion;
    } table[] = {
        /* Maxwell */
        { NV2080_CTRL_GR_INFO_SM_VERSION_5_0, { 5,0 } },
        { NV2080_CTRL_GR_INFO_SM_VERSION_5_2, { 5,2 } },
        { NV2080_CTRL_GR_INFO_SM_VERSION_5_3, { 5,3 } },

        /* Pascal */
        { NV2080_CTRL_GR_INFO_SM_VERSION_6_0, { 5,5 } },
        { NV2080_CTRL_GR_INFO_SM_VERSION_6_1, { 5,5 } },
        { NV2080_CTRL_GR_INFO_SM_VERSION_6_2, { 5,6 } },

        /* Volta */
        { NV2080_CTRL_GR_INFO_SM_VERSION_7_0, { 7,0 } },
        { NV2080_CTRL_GR_INFO_SM_VERSION_7_2, { 7,2 } },

        /* Turing */
        { NV2080_CTRL_GR_INFO_SM_VERSION_7_3, { 7,3 } },
        { NV2080_CTRL_GR_INFO_SM_VERSION_7_5, { 7,5 } },

        /* Ampere */
        { NV2080_CTRL_GR_INFO_SM_VERSION_8_2, { 8,2 } },
        { NV2080_CTRL_GR_INFO_SM_VERSION_8_6, { 8,6 } },
        { NV2080_CTRL_GR_INFO_SM_VERSION_8_7, { 8,6 } },
        { NV2080_CTRL_GR_INFO_SM_VERSION_8_8, { 8,6 } },

        /* Ada */
        { NV2080_CTRL_GR_INFO_SM_VERSION_8_9, { 8,9 } },

        /* Hopper */
        { NV2080_CTRL_GR_INFO_SM_VERSION_9_0, { 9,0 } },

        /* Blackwell */
        { NV2080_CTRL_GR_INFO_SM_VERSION_10_0, { 10,0 } },
        { NV2080_CTRL_GR_INFO_SM_VERSION_10_1, { 10,1 } },
    };

    const NvU32 smVersion = GetSmVersion(pPushDevice);
    NvU32 i;

    for (i = 0; i < ARRAY_LEN(table); i++) {
        if (table[i].smVersion == smVersion) {
            *pSpaVersion = table[i].spaVersion;
            return TRUE;
        }
    }

    return FALSE;
}

static const Nv3dHal _nv3dHalMaxwell = {
    _nv3dSetSpaVersionKepler,                       /* setSpaVersion */
    _nv3dInitChannelMaxwell,                        /* initChannel */
    _nv3dUploadDataInlineKepler,                    /* uploadDataInline */
    _nv3dSetProgramOffsetFermi,                     /* setProgramOffset */
    _nv3dAssignNv3dTextureMaxwell,                  /* assignNv3dTexture */
    _nv3dSetVertexStreamEndFermi,                   /* setVertexStreamEnd */
};

static const Nv3dHal _nv3dHalPascal = {
    _nv3dSetSpaVersionKepler,                       /* setSpaVersion */
    _nv3dInitChannelPascal,                         /* initChannel */
    _nv3dUploadDataInlineKepler,                    /* uploadDataInline */
    _nv3dSetProgramOffsetFermi,                     /* setProgramOffset */
    _nv3dAssignNv3dTexturePascal,                   /* assignNv3dTexture */
    _nv3dSetVertexStreamEndFermi,                   /* setVertexStreamEnd */
};

static const Nv3dHal _nv3dHalVolta = {
    _nv3dSetSpaVersionKepler,                       /* setSpaVersion */
    _nv3dInitChannelPascal,                         /* initChannel */
    _nv3dUploadDataInlineKepler,                    /* uploadDataInline */
    _nv3dSetProgramOffsetVolta,                     /* setProgramOffset */
    _nv3dAssignNv3dTexturePascal,                   /* assignNv3dTexture */
    _nv3dSetVertexStreamEndFermi,                   /* setVertexStreamEnd */
};

static const Nv3dHal _nv3dHalTuring = {
    _nv3dSetSpaVersionKepler,                       /* setSpaVersion */
    _nv3dInitChannelTuring,                         /* initChannel */
    _nv3dUploadDataInlineKepler,                    /* uploadDataInline */
    _nv3dSetProgramOffsetVolta,                     /* setProgramOffset */
    _nv3dAssignNv3dTexturePascal,                   /* assignNv3dTexture */
    _nv3dSetVertexStreamEndTuring,                  /* setVertexStreamEnd */
};

static const Nv3dHal _nv3dHalAmpere = {
    _nv3dSetSpaVersionKepler,                       /* setSpaVersion */
    _nv3dInitChannelTuring,                         /* initChannel */
    _nv3dUploadDataInlineKepler,                    /* uploadDataInline */
    _nv3dSetProgramOffsetVolta,                     /* setProgramOffset */
    _nv3dAssignNv3dTexturePascal,                   /* assignNv3dTexture */
    _nv3dSetVertexStreamEndTuring,                  /* setVertexStreamEnd */
};

static const Nv3dHal _nv3dHalHopper = {
    _nv3dSetSpaVersionKepler,                       /* setSpaVersion */
    _nv3dInitChannelHopper,                         /* initChannel */
    _nv3dUploadDataInlineKepler,                    /* uploadDataInline */
    _nv3dSetProgramOffsetVolta,                     /* setProgramOffset */
    _nv3dAssignNv3dTextureHopper,                   /* assignNv3dTexture */
    _nv3dSetVertexStreamEndTuring,                  /* setVertexStreamEnd */
};

NvBool nv3dAllocDevice(
    const Nv3dAllocDeviceParams *pParams,
    Nv3dDevicePtr p3dDevice)
{
    static const struct {
        NvPushSupportedClass base;
        const Nv3dDeviceCapsRec caps;
        const Nv3dHal *hal;
        enum Nv3dShaderArch shaderArch;
    } table[] = {

#define ENTRY(_classNumber,                                          \
              _arch,                                                 \
              _amodelArch,                                           \
              _hasSetBindlessTexture,                                \
              _hasProgramRegion,                                     \
              _maxDim,                                               \
              _hal)                                                  \
        {                                                            \
            .base.classNumber           = _classNumber,              \
            .base.amodelConfig          = NV_AMODEL_ ## _amodelArch, \
            .caps.hasSetBindlessTexture = _hasSetBindlessTexture,    \
            .caps.hasProgramRegion      = _hasProgramRegion,         \
            .caps.maxDim                = _maxDim,                   \
            .hal                        = &_nv3dHal ## _hal,         \
            .shaderArch                 = NV3D_SHADER_ARCH_ ## _arch,\
        }

        /*
         * hal--------------------------------------------------+
         * maxDim----------------------------------------+      |
         * hasProgramRegion---------------------------+  |      |
         * hasSetBindlessTexture-------------------+  |  |      |
         * amodel arch----------------+            |  |  |      |
         * shader arch---+            |            |  |  |      |
         * classNumber   |            |            |  |  |      |
         *    |          |            |            |  |  |      |
         */
        ENTRY(BLACKWELL_A,BLACKWELL,  BLACKWELL,   0, 0, 32768, Hopper),
        ENTRY(HOPPER_A,  HOPPER,      HOPPER,      0, 0, 32768, Hopper),
        ENTRY(ADA_A,     AMPERE,      ADA,         0, 0, 32768, Ampere),
        ENTRY(AMPERE_B,  AMPERE,      AMPERE,      0, 0, 32768, Ampere),
        ENTRY(AMPERE_A,  AMPERE,      AMPERE,      0, 0, 32768, Ampere),
        ENTRY(TURING_A,  TURING,      TURING,      0, 0, 32768, Turing),
        ENTRY(VOLTA_A,   VOLTA,       VOLTA,       0, 0, 32768, Volta),
        ENTRY(PASCAL_B,  PASCAL,      PASCAL,      1, 1, 32768, Pascal),
        ENTRY(PASCAL_A,  PASCAL,      PASCAL,      1, 1, 32768, Pascal),
        ENTRY(MAXWELL_B, MAXWELL,     MAXWELL,     1, 1, 16384, Maxwell),
        ENTRY(MAXWELL_A, MAXWELL,     MAXWELL,     1, 1, 16384, Maxwell),
    };

    int i;

    NVMISC_MEMSET(p3dDevice, 0, sizeof(*p3dDevice));

    /* find the first supported 3D HAL */

    i = nvPushGetSupportedClassIndex(pParams->pPushDevice,
                                     table,
                                     sizeof(table[0]),
                                     ARRAY_LEN(table));
    if (i == -1) {
        goto fail;
    }

    if (!GetSpaVersion(pParams->pPushDevice, &p3dDevice->spaVersion)) {
        goto fail;
    }

    p3dDevice->pPushDevice = pParams->pPushDevice;
    p3dDevice->caps = table[i].caps;
    p3dDevice->classNumber = table[i].base.classNumber;
    p3dDevice->hal = table[i].hal;
    p3dDevice->shaderArch = table[i].shaderArch;

    if (!GetMaxThreadsAndWarps(p3dDevice)) {
        goto fail;
    }

    return TRUE;

fail:
    nv3dFreeDevice(p3dDevice);
    return FALSE;
}

void nv3dFreeDevice(Nv3dDevicePtr p3dDevice)
{
    /*
     * So far, there is nothing to free: Nv3dDevicePtr only stores
     * queried information.
     */
    NVMISC_MEMSET(p3dDevice, 0, sizeof(*p3dDevice));
}

static NvU64 ComputeProgramLocalMemorySize(
    const Nv3dChannelRec *p3dChannel)
{
    const Nv3dDeviceRec *p3dDevice = p3dChannel->p3dDevice;

    // LocalMemorySizePerSM needs to be a multiple of 512
    // Note that maxLocalBytes and/or maxStackBytes might be zero.
    const NvU64 defaultSizePerWarp =
        NV_ALIGN_UP(p3dChannel->programs.maxLocalBytes *
                    p3dDevice->maxThreadsPerWarp +
                    p3dChannel->programs.maxStackBytes, 512);

    // shader local memory lower bits must be a multiple of 128kB
    return NV_ALIGN_UP(defaultSizePerWarp * p3dDevice->maxWarps, 128*1024);
}

NvBool nv3dAllocChannelState(
    const Nv3dAllocChannelStateParams *pParams,
    Nv3dChannelPtr p3dChannel)
{
    NVMISC_MEMSET(p3dChannel, 0, sizeof(*p3dChannel));

    p3dChannel->p3dDevice = pParams->p3dDevice;

    p3dChannel->numTextures = pParams->numTextures;
    p3dChannel->numTextureBindings = pParams->numTextureBindings;

    p3dChannel->hasFrameBoundaries = pParams->hasFrameBoundaries;

    p3dChannel->programs = PickProgramsRec(pParams->p3dDevice);

    p3dChannel->programLocalMemorySize =
        ComputeProgramLocalMemorySize(p3dChannel);

    _nv3dAssignSurfaceOffsets(pParams, p3dChannel);

    return TRUE;
}

void nv3dFreeChannelState(Nv3dChannelPtr p3dChannel)
{
    int sd;
    for (sd = 0; sd < NV_MAX_SUBDEVICES; sd++) {
        nvAssert(p3dChannel->surface.handle[sd] == 0);
    }
    nvAssert(p3dChannel->pPushChannel == NULL);

    NVMISC_MEMSET(p3dChannel, 0, sizeof(*p3dChannel));
}

