/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvkms-types.h"
#include "nvkms-cursor.h"
#include "nvkms-hal.h"
#include "nvkms-rm.h"



#include "class/clc570.h" // NVC570_DISPLAY
#include "class/clc670.h" // NVC670_DISPLAY
#include "class/clc770.h" // NVC770_DISPLAY
#include "class/clc970.h" // NVC970_DISPLAY
#include "class/clca70.h" // NVCA70_DISPLAY
#include "class/clcb70.h" // NVCB70_DISPLAY
#include "class/clcc70.h" // NVCC70_DISPLAY

#include "class/clc57d.h" // NVC57D_CORE_CHANNEL_DMA
#include "class/clc57e.h" // NVC57E_WINDOW_CHANNEL_DMA
#include "class/clc67d.h" // NVC67D_CORE_CHANNEL_DMA
#include "class/clc67e.h" // NVC67E_WINDOW_CHANNEL_DMA
#include "class/clc77d.h" // NVC67D_CORE_CHANNEL_DMA
#include "class/clc97d.h" // NVC97D_CORE_CHANNEL_DMA
#include "class/clc97e.h" // NVC97E_WINDOW_CHANNEL_DMA
#include "class/clca7d.h" // NVCA7D_CORE_CHANNEL_DMA
#include "class/clca7e.h" // NVCA7E_WINDOW_CHANNEL_DMA
#include "class/clcb7d.h" // NVCB7D_CORE_CHANNEL_DMA
#include "class/clcb7e.h" // NVCB7E_WINDOW_CHANNEL_DMA
#include "class/clcc7d.h" // NVCC7D_CORE_CHANNEL_DMA
#include "class/clcc7e.h" // NVCC7E_WINDOW_CHANNEL_DMA

extern NVEvoHAL nvEvoC5;
extern NVEvoHAL nvEvoC6;
extern NVEvoHAL nvEvoC9;
extern NVEvoHAL nvEvoCA;

enum NvKmsAllocDeviceStatus nvAssignEvoCaps(NVDevEvoPtr pDevEvo)
{
#define ENTRY(_classPrefix,                                               \
              _pEvoHal,                                                   \
              _hdmiTmds10BpcMaxPClkMHz,                                   \
              _rasterLockAcrossProtocolsAllowed,                          \
              _validNIsoFormatMask,                                       \
              _maxPitch,                                                  \
              _maxWidthInBytes,                                           \
              _maxWidthInPixels,                                          \
              _maxHeight,                                                 \
              _coreChannelDmaArmedOffset,                                 \
              _dmaArmedSize)                                              \
    {                                                                     \
        .class = NV ## _classPrefix ## 70_DISPLAY,                        \
        .pEvoHal = _pEvoHal,                                              \
        .coreChannelDma = {                                               \
            .coreChannelClass =                                           \
                NV ## _classPrefix ## 7D_CORE_CHANNEL_DMA,                \
            .dmaArmedSize = _dmaArmedSize,                                \
            .dmaArmedOffset =                                             \
                _coreChannelDmaArmedOffset,                               \
        },                                                                \
        .evoCaps = {                                                      \
            .validNIsoFormatMask       = _validNIsoFormatMask,            \
            .maxPitchValue             = _maxPitch,                       \
            .maxWidthInBytes           = _maxWidthInBytes,                \
            .maxWidthInPixels          = _maxWidthInPixels,               \
            .maxHeight                 = _maxHeight,                      \
            .maxRasterWidth  = DRF_MASK(NV ## _classPrefix ## 7D_HEAD_SET_RASTER_SIZE_WIDTH), \
            .maxRasterHeight = DRF_MASK(NV ## _classPrefix ## 7D_HEAD_SET_RASTER_SIZE_HEIGHT),\
            .hdmiTmds10BpcMaxPClkMHz = _hdmiTmds10BpcMaxPClkMHz,          \
            .rasterLockAcrossProtocolsAllowed = _rasterLockAcrossProtocolsAllowed, \
        }                                                                 \
    }

/*
 * The file
 * https://github.com/NVIDIA/open-gpu-doc/blob/master/manuals/volta/gv100/dev_display_withoffset.ref.txt
 * defines:
 *
 *   #define NV_UDISP_FE_CHN_ASSY_BASEADR_CORE               0x00680000
 *   #define NV_UDISP_FE_CHN_ARMED_BASEADR_CORE              (0x00680000+32768)
 *
 * The NVD_CORE_CHANNEL_DMA_ARMED_OFFSET is calculated as
 * (NV_UDISP_FE_CHN_ARMED_BASEADR_CORE - NV_UDISP_FE_CHN_ASSY_BASEADR_CORE).
 */
#define NVD_CORE_CHANNEL_DMA_ARMED_OFFSET 0x8000

/*
 * From the above in dev_display_withoffset.ref.txt, ARMED is the upper
 * 32k of the core channel's 64k space.
 */
#define NVD_CORE_CHANNEL_DMA_ARMED_SIZE 0x8000

/* NVDisplay and later entries */
#define ENTRY_NVD(_coreClassPrefix, _windowClassPrefix, ...) \
    ENTRY(_coreClassPrefix, __VA_ARGS__,  \
          (1 << NVKMS_NISO_FORMAT_FOUR_WORD_NVDISPLAY), \
          DRF_MASK(NV ## _windowClassPrefix ## 7E_SET_PLANAR_STORAGE_PITCH), \
          DRF_MASK(NV ## _windowClassPrefix ## 7E_SET_PLANAR_STORAGE_PITCH) * \
                   NVKMS_BLOCK_LINEAR_GOB_WIDTH, \
          DRF_MASK(NV ## _windowClassPrefix ## 7E_SET_SIZE_IN_WIDTH), \
          DRF_MASK(NV ## _windowClassPrefix ## 7E_SET_SIZE_IN_WIDTH), \
          NVD_CORE_CHANNEL_DMA_ARMED_OFFSET, \
          NVD_CORE_CHANNEL_DMA_ARMED_SIZE)

    static const struct {
        NvU32 class;
        const NVEvoHAL *pEvoHal;
        const NVEvoCoreChannelDmaRec coreChannelDma;
        const NVEvoCapsRec evoCaps;
    } dispTable[] = {
        /*
         * rasterLockAcrossProtocolsAllowed
         * hdmiTmds10BpcMaxPClkMHz--+    |
         * pEvoHal --------------+  |    |
         * windowClassPrefix     |  |    |
         * classPrefix |         |  |    |
         *         |   |         |  |    |
         */
        ENTRY_NVD(CC, CC, &nvEvoCA, 324, 0),
        ENTRY_NVD(CB, CB, &nvEvoCA, 324, 0),
        /* Blackwell GB20X */
        ENTRY_NVD(CA, CA, &nvEvoCA, 324, 1),
        /* Blackwell */
        ENTRY_NVD(C9, C9, &nvEvoC9, 324, 1),
        /* Ada */
        ENTRY_NVD(C7, C6, &nvEvoC6, 324, 1),
        /* Ampere */
        ENTRY_NVD(C6, C6, &nvEvoC6, 324, 1),
        /* Turing */
        ENTRY_NVD(C5, C5, &nvEvoC5, 0,   1),
    };

    int i;

    if (nvkms_test_fail_alloc_core_channel(FAIL_ALLOC_CORE_CHANNEL_NO_CLASS)) {
        return NVKMS_ALLOC_DEVICE_STATUS_NO_HARDWARE_AVAILABLE;
    }

    for (i = 0; i < ARRAY_LEN(dispTable); i++) {
        if (nvRmEvoClassListCheck(pDevEvo, dispTable[i].class)) {
            pDevEvo->hal = dispTable[i].pEvoHal;
            pDevEvo->dispClass = dispTable[i].class;
            pDevEvo->caps = dispTable[i].evoCaps;

            pDevEvo->coreChannelDma = dispTable[i].coreChannelDma;
            nvAssert(nvRmEvoClassListCheck(
                        pDevEvo,
                        pDevEvo->coreChannelDma.coreChannelClass));

            return nvInitDispHalCursorEvo(pDevEvo);
        }
    }

    return NVKMS_ALLOC_DEVICE_STATUS_NO_HARDWARE_AVAILABLE;
}
