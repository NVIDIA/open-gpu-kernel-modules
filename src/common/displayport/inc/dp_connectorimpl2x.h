/*
 * SPDX-FileCopyrightText: Copyright (c) 2023-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************* DisplayPort********************************\
*                                                                           *
* Module: dp_connectorimpl2x.cpp                                            *
*    DP connector implementation for DP2.x                                  *
*                                                                           *
\***************************************************************************/
#ifndef INCLUDED_DP_CONNECTORIMPL2X_H
#define INCLUDED_DP_CONNECTORIMPL2X_H


#include "dp_connector.h"
#include "ctrl/ctrl0073/ctrl0073dp.h"

namespace DisplayPort
{
    struct ConnectorImpl2x : public ConnectorImpl
    {
        virtual void configInit();
        virtual void populateAllDpConfigs();
        virtual LinkRates* importDpLinkRates();
        virtual LinkConfiguration getMaxLinkConfig();

        virtual bool train(const LinkConfiguration & lConfig, bool force,
                           LinkTrainingType trainType = NORMAL_LINK_TRAINING);

        virtual bool allocateTimeslice(GroupImpl * targetGroup);
        virtual bool checkIsModePossibleMST(GroupImpl * group);
        virtual bool compoundQueryAttachMSTGeneric(Group * target,
                                                   const DpModesetParams &modesetParams,       // Modeset info
                                                   CompoundQueryAttachMSTInfo * info,          // local info with updates for DSC
                                                   DscParams *pDscParams = NULL,               // DSC parameters
                                                   DP_IMP_ERROR *pErrorCode = NULL);           // Error Status code

        virtual bool notifyAttachBegin(Group *target, const DpModesetParams &modesetParams);

        virtual void notifyAttachEnd(bool modesetCancelled);
        virtual void notifyDetachBegin(Group *target);
        virtual void notifyDetachEnd(bool bKeepOdAlive);

        virtual bool beforeAddStream(GroupImpl * group, bool test = false, bool forFlushMode = false);
        virtual void afterAddStream(GroupImpl * group);
        virtual void beforeDeleteStream(GroupImpl * group, bool forFlushMode);
        virtual void afterDeleteStream(GroupImpl * group);
        virtual bool willLinkSupportModeSST(const LinkConfiguration &linkConfig, const ModesetInfo &modesetInfo,
                                            const DscParams *pDscParams = NULL);

        bool willLinkSupportMode(const LinkConfiguration &linkConfig, const ModesetInfo &modesetInfo,
                                 NvU32 headIndex = 0, Watermark *watermark = NULL, const DscParams *pDscParams = NULL);
        virtual bool getValidLowestLinkConfig(LinkConfiguration &lConfig, LinkConfiguration &lowestSelected,
                                              ModesetInfo queryModesetInfo, const DscParams *pDscParams = NULL);

        virtual bool handlePhyPatternRequest();
        virtual bool handleTestLinkTrainRequest();

        ConnectorImpl2x(MainLink * main, AuxBus * auxBus, Timer * timer, Connector::EventSink * sink);

        bool validateLinkConfiguration(const LinkConfiguration & lConfig);
        bool enableFlush();
        void disableFlush(bool test = false);
        // "data" is an array of NV0073_CTRL_MAX_LANES unsigned ints
        virtual bool getDp2xLaneConfig(NvU32 *numLanes, NvU32 *data);
        virtual bool setDp2xLaneConfig(NvU32 numLanes, NvU32 *data);
        void applyDP2xRegkeyOverrides();
        virtual void applyOuiWARs();
        virtual void handleEdidWARs(Edid &edid, DiscoveryManager::Device &device);
        virtual void applyTimeslotWAR(unsigned &slot_count);

        bool    bSupportUHBR2_50;               // Support UHBR2.5 for internal testing.
        bool    bSupportUHBR2_70;               // Support UHBR2.7 for internal testing.
        bool    bSupportUHBR5_00;               // Support UHBR5.0 for internal testing.
        bool    bFlushSkipped;

        bool    bMstTimeslotBug4968411;
        bool    bApplyManualTimeslotBug4968411;

        // Do not enable downspread while link training.
        bool    bDisableDownspread;

        virtual void setDisableDownspread(bool _bDisableDownspread)
        {
            this->bDisableDownspread = _bDisableDownspread;
        }
        virtual bool getDownspreadDisabled()
        {
            return this->bDisableDownspread;
        }
    };
}

#endif //INCLUDED_DP_CONNECTORIMPL2X_H
