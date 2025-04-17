/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2016 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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



#include "class/cl9470.h" // NV9470_DISPLAY
#include "class/cl9570.h" // NV9570_DISPLAY
#include "class/cl9770.h" // NV9770_DISPLAY
#include "class/cl9870.h" // NV9870_DISPLAY
#include "class/clc370.h" // NVC370_DISPLAY
#include "class/clc570.h" // NVC570_DISPLAY
#include "class/clc670.h" // NVC670_DISPLAY
#include "class/clc770.h" // NVC770_DISPLAY
#include "class/clca70.h" // NVCA70_DISPLAY
#include "class/clcb70.h" // NVCB70_DISPLAY

#include "class/cl947d.h" // NV947D_CORE_CHANNEL_DMA
#include "class/cl957d.h" // NV957D_CORE_CHANNEL_DMA
#include "class/cl977d.h" // NV977D_CORE_CHANNEL_DMA
#include "class/cl987d.h" // NV987D_CORE_CHANNEL_DMA
#include "class/clc37d.h" // NVC37D_CORE_CHANNEL_DMA
#include "class/clc37e.h" // NVC37E_WINDOW_CHANNEL_DMA
#include "class/clc57d.h" // NVC57D_CORE_CHANNEL_DMA
#include "class/clc57e.h" // NVC57E_WINDOW_CHANNEL_DMA
#include "class/clc67d.h" // NVC67D_CORE_CHANNEL_DMA
#include "class/clc67e.h" // NVC67E_WINDOW_CHANNEL_DMA
#include "class/clc77d.h" // NVC67D_CORE_CHANNEL_DMA
#include "class/clca7d.h" // NVCA7D_CORE_CHANNEL_DMA
#include "class/clca7e.h" // NVCA7E_WINDOW_CHANNEL_DMA
#include "class/clcb7d.h" // NVCB7D_CORE_CHANNEL_DMA
#include "class/clcb7e.h" // NVCB7E_WINDOW_CHANNEL_DMA

extern NVEvoHAL nvEvo94;
extern NVEvoHAL nvEvo97;
extern NVEvoHAL nvEvoC3;
extern NVEvoHAL nvEvoC5;
extern NVEvoHAL nvEvoC6;
extern NVEvoHAL nvEvoCA;

enum NvKmsAllocDeviceStatus nvAssignEvoCaps(NVDevEvoPtr pDevEvo)
{
#define ENTRY(_classPrefix,                                               \
              _pEvoHal,                                                   \
              _supportsDP13,                                              \
              _supportsHDMI20,                                            \
              _supportsYUV2020,                                           \
              _inputLutAppliesToBase,                                     \
              _dpYCbCr422MaxBpc,                                          \
              _hdmiYCbCr422MaxBpc,                                        \
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
            .supportsDP13              = _supportsDP13,                   \
            .supportsHDMI20            = _supportsHDMI20,                 \
            .supportsYUV2020           = _supportsYUV2020,                \
            .validNIsoFormatMask       = _validNIsoFormatMask,            \
            .inputLutAppliesToBase     = _inputLutAppliesToBase,          \
            .maxPitchValue             = _maxPitch,                       \
            .maxWidthInBytes           = _maxWidthInBytes,                \
            .maxWidthInPixels          = _maxWidthInPixels,               \
            .maxHeight                 = _maxHeight,                      \
            .maxRasterWidth  = DRF_MASK(NV ## _classPrefix ## 7D_HEAD_SET_RASTER_SIZE_WIDTH), \
            .maxRasterHeight = DRF_MASK(NV ## _classPrefix ## 7D_HEAD_SET_RASTER_SIZE_HEIGHT),\
            .dpYCbCr422MaxBpc = _dpYCbCr422MaxBpc,                        \
            .hdmiYCbCr422MaxBpc = _hdmiYCbCr422MaxBpc,                    \
        }                                                                 \
    }

#define EVO_CORE_CHANNEL_DMA_ARMED_OFFSET 0x0

#define EVO_CORE_CHANNEL_DMA_ARMED_SIZE 0x1000


/* Pre-NVDisplay EVO entries */
#define ENTRY_EVO(_classPrefix, ...) \
    ENTRY(_classPrefix, __VA_ARGS__,  \
          ((1 << NVKMS_NISO_FORMAT_LEGACY) | \
           (1 << NVKMS_NISO_FORMAT_FOUR_WORD)), \
          DRF_MASK(NV ## _classPrefix ## 7D_HEAD_SET_STORAGE_PITCH), \
          DRF_MASK(NV ## _classPrefix ## 7D_HEAD_SET_STORAGE_PITCH) * \
                   NVKMS_BLOCK_LINEAR_GOB_WIDTH, \
          DRF_MASK(NV ## _classPrefix ## 7D_HEAD_SET_SIZE_WIDTH), \
          DRF_MASK(NV ## _classPrefix ## 7D_HEAD_SET_SIZE_HEIGHT), \
          EVO_CORE_CHANNEL_DMA_ARMED_OFFSET, \
          EVO_CORE_CHANNEL_DMA_ARMED_SIZE)


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
         * hdmiYCbCr422MaxBpc-----------------------+
         * dpYCbCr422MaxBpc---------------------+   |
         * inputLutAppliesToBase ------------+  |   |
         * supportsYUV2020 ---------------+  |  |   |
         * supportsHDMI20 -------------+  |  |  |   |
         * supportsDP13 ------------+  |  |  |  |   |
         * pEvoHal --------------+  |  |  |  |  |   |
         * windowClassPrefix     |  |  |  |  |  |   |
         * classPrefix |         |  |  |  |  |  |   |
         *         |   |         |  |  |  |  |  |   |
         */
        ENTRY_NVD(CB, CB, &nvEvoCA, 1, 1, 1, 0, 12, 12),
        /* Blackwell GB20X */
        ENTRY_NVD(CA, CA, &nvEvoCA, 1, 1, 1, 0, 12, 12),
        /* Ada */
        ENTRY_NVD(C7, C6, &nvEvoC6, 1, 1, 1, 0, 12, 12),
        /* Ampere */
        ENTRY_NVD(C6, C6, &nvEvoC6, 1, 1, 1, 0, 12, 12),
        /* Turing */
        ENTRY_NVD(C5, C5, &nvEvoC5, 1, 1, 1, 0, 12, 12),
        /* Volta */
        ENTRY_NVD(C3, C3, &nvEvoC3, 1, 1, 1, 0, 12, 12),
        /* gp10x */
        ENTRY_EVO(98,     &nvEvo97, 1, 1, 1, 1, 12, 12),
        /* gp100 */
        ENTRY_EVO(97,     &nvEvo97, 1, 1, 1, 1, 12, 12),
        /* gm20x */
        ENTRY_EVO(95,     &nvEvo94, 0, 1, 0, 1, 8,  0),
        /* gm10x */
        ENTRY_EVO(94,     &nvEvo94, 0, 0, 0, 1, 8,  0),
    };

    int i;

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
