/*
 * SPDX-FileCopyrightText: Copyright (c) 2016 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __NVKMS_EVO_1_H__
#define __NVKMS_EVO_1_H__

#include "nvkms-types.h"

NvBool nvEvo1IsChannelIdle(NVDevEvoPtr, NVEvoChannelPtr, NvU32 sd,
                           NvBool *result);
NvBool nvEvo1IsChannelMethodPending(NVDevEvoPtr, NVEvoChannelPtr, NvU32 sd,
                                    NvBool *result);

void nvEvo1IsModePossible(NVDispEvoPtr pDispEvo,
                          const NVEvoIsModePossibleDispInput *pInput,
                          NVEvoIsModePossibleDispOutput *pOutput);
void nvEvo1PrePostIMP(NVDispEvoPtr pDispEvo, NvBool isPre);

void nvEvo1SetDscParams(const NVDispEvoRec *pDispEvo,
                        const NvU32 head,
                        const NVDscInfoEvoRec *pDscInfo,
                        const enum nvKmsPixelDepth pixelDepth);

NVEvoChannel* nvEvo1AllocateCoreChannel(NVDevEvoRec *pDevEvo);
void nvEvo1FreeCoreChannel(NVDevEvoRec *pDevEvo, NVEvoChannel *pChannel);

NvBool nvEvo1NvtToHdmiInfoFramePacketType(const NvU32 srcType, NvU8 *pDstType);

void nvEvo1SendHdmiInfoFrame(const NVDispEvoRec *pDispEvo,
                             const NvU32 head,
                             const NvEvoInfoFrameTransmitControl transmitCtrl,
                             const NVT_INFOFRAME_HEADER *pInfoFrameHeader,
                             const NvU32 infoframeSize,
                             NvBool needChecksum);

void nvEvo1DisableHdmiInfoFrame(const NVDispEvoRec *pDispEvo,
                                const NvU32 head,
                                const NvU8 nvtInfoFrameType);

void nvEvo1SendDpInfoFrameSdp(const NVDispEvoRec *pDispEvo,
                              const NvU32 head,
                              const NvEvoInfoFrameTransmitControl transmitCtrl,
                              const DPSDP_DESCRIPTOR *sdp);

static inline NvU16 nvEvo1GetColorSpaceFlag(NVDevEvoPtr pDevEvo,
                                            const NvBool colorSpaceOverride)
{
    NvU16 colorSpaceFlag = 0;

    if (colorSpaceOverride) {
        nvAssert(pDevEvo->caps.supportsDP13);
        colorSpaceFlag = 1 << 11;
    }

    return colorSpaceFlag;
}

#endif /* __NVKMS_EVO_1_H__ */
