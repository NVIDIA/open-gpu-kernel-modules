/*
 * SPDX-FileCopyrightText: Copyright (c) 2011 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __NVDP_EVO_INTERFACE_HPP__
#define __NVDP_EVO_INTERFACE_HPP__

#include <nvtypes.h>
#include <nvkms-types.h>
#include <dp_evoadapter.h>

namespace nvkmsDisplayPort
{

class EvoInterface : public DisplayPort::Object,
        public DisplayPort::EvoInterface
{
public:
    const NVConnectorEvoPtr pConnectorEvo;

    EvoInterface(NVConnectorEvoPtr pConnectorEvo);

    // Functions inherited from DisplayPort::EvoInterface
    virtual NvU32 rmControl0073(NvU32 command, void * params, NvU32 paramSize);
    virtual NvU32 rmControl5070(NvU32 command, void * params, NvU32 paramSize);

    virtual NvU32 getSubdeviceIndex();
    virtual NvU32 getDisplayId();
    virtual NvU32 getSorIndex();
    virtual NvU32 getLinkIndex();
    virtual NvU32 getRegkeyValue(const char *key);
    virtual bool isInbandStereoSignalingSupported();

    virtual NvU32 monitorDenylistInfo(
         NvU32 manufId,
         NvU32 productId,
         DisplayPort::DpMonitorDenylistData *pDenylistData);
};

}; // namespace nvkmsDisplayPort

#endif // __NVDP_EVO_INTERFACE_HPP__
