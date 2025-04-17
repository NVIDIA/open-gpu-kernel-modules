/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************* List **************************************\
*                                                                           *
* Module: dp_evoadapter.h                                                   *
*    Interface for low level access to the aux bus.                         *
*    This is the synchronous version of the interface.                      *
*                                                                           *
\***************************************************************************/
#ifndef INCLUDED_DP_EVOADAPTER_H
#define INCLUDED_DP_EVOADAPTER_H

#include "dp_timer.h"
#include "dp_auxbus.h"
#include "dp_mainlink.h"
#include "dp_wardatabase.h"
#include "dp_auxdefs.h"
#include "dp_regkeydatabase.h"

#include <nvos.h>
#include <ctrl/ctrl0073/ctrl0073dfp.h>
#include <ctrl/ctrl0073/ctrl0073dp.h>

#define HDCP_DUMMY_CN    (0x1)
#define HDCP_DUMMY_CKSV  (0xFFFFF)

namespace DisplayPort
{
    class EvoInterface
    {
    public:
        //
        //  IOCTL access to RM class DISPLAY_COMMON and NV50_DISPLAY
        //
        virtual NvU32 rmControl0073(NvU32 command, void * params, NvU32 paramSize) = 0;
        virtual NvU32 rmControl5070(NvU32 command, void * params, NvU32 paramSize) = 0;

        virtual bool getMaxLinkConfigFromUefi(NvU8 &linkRate, NvU8 &laneCount)
        {
            linkRate = 0; laneCount = 0;
            return true;
        }

        //
        // Call to tell DD that linkTraining will be performed.
        // Required when head is attached & we enter in flush mode GPUs.
        // Required to enable/disable Audio.
        //
        // Derived classes that override these functions must call down to
        // DisplayPort::EvoInterface::pre/postLinkTraining() to inherit this
        // implementation.
        //
        virtual void  preLinkTraining(NvU32 head)
        {
        }
        virtual void  postLinkTraining(NvU32 head)
        {
        }

        virtual NvU32 getSubdeviceIndex() = 0;
        virtual NvU32 getDisplayId() = 0;
        virtual NvU32 getSorIndex() = 0;
        virtual NvU32 getLinkIndex() = 0;       // Link A = 0, Link B = 1
        //
        // Query the value of a registry key.  Implementations should return 0
        // if the regkey is not set.
        //
        virtual NvU32 getRegkeyValue(const char *key)
        {
            return 0;
        }
        virtual NvU32 monitorDenylistInfo(NvU32 manufId, NvU32 productId, DpMonitorDenylistData *pDenylistData)
        {
            return 0;
        }

        virtual bool isInbandStereoSignalingSupported()
        {
            return false;
        }
    };

    MainLink * MakeEvoMainLink(EvoInterface * provider, Timer * timer);
    AuxBus   * MakeEvoAuxBus(EvoInterface * provider, Timer * timer);

    class EvoAuxBus : public AuxBus
    {
      public:
        EvoAuxBus(EvoInterface * provider, Timer * timer)
            : provider(provider),
            timer(timer),
            displayId(provider->getDisplayId()),
            subdeviceIndex(provider->getSubdeviceIndex()),
            devicePlugged(false)
        {
        }

        virtual status transaction(Action action, Type type, int address, NvU8 * buffer,
                                   unsigned sizeRequested, unsigned * sizeCompleted,
                                   unsigned * pNakReason = NULL,
                                   NvU8 offset = 0, NvU8 nWriteTransactions = 0);
        virtual unsigned transactionSize();
        virtual void setDevicePlugged(bool);

      private:
        EvoInterface * provider;
        Timer * timer;
        NvU32 displayId;
        NvU32 subdeviceIndex;
        bool devicePlugged;
    };

    class EvoMainLink : public MainLink
    {
      private:
        NvU32 _maxLinkRateSupportedGpu;
        NvU32 _maxLinkRateSupportedDfp;
        bool _hasIncreasedWatermarkLimits;
        bool _hasMultistream;
        bool _isPC2Disabled;
        bool _isEDP;

        //
        // Bit mask for GPU supported DP versions.
        // Defines the same as NV0073_CTRL_CMD_DP_GET_CAPS_PARAMS.dpVersionsSupported
        //
        NvU32   _gpuSupportedDpVersions;

        bool _isStreamCloningEnabled;
        bool _needForceRmEdid;
        bool _skipPowerdownEDPPanelWhenHeadDetach;
        bool _isDscDisabledByRegkey;
        bool _isMstDisabledByRegkey;
        bool _isFECSupported;
        bool _useDfpMaxLinkRateCaps;
        bool _applyLinkBwOverrideWarRegVal;
        bool _isDynamicMuxCapable;
        bool _enableMSAOverrideOverMST;
        bool _isLTPhyRepeaterSupported;
        bool _isMSTPCONCapsReadDisabled;
        bool _isDownspreadSupported;
        bool _bAvoidHBR3;
        bool _bAvoidHBR3DisabledByRegkey;
        //
        // LTTPR count reported by RM, it might not be the same with DPLib probe
        // For example, some Intel LTTPR might not be ready to response 0xF0000 probe
        // done by RM, but when DPLib checks the same DPCD offsets it responses
        // properly. This will cause serious LT problem.
        //
        unsigned _rmPhyRepeaterCount;

        struct DSC
        {
            bool isDscSupported;
            unsigned encoderColorFormatMask;
            unsigned lineBufferSizeKB;
            unsigned rateBufferSizeKB;
            unsigned bitsPerPixelPrecision;
            unsigned maxNumHztSlices;
            unsigned lineBufferBitDepth;
        }_DSC;
        void initializeRegkeyDatabase();
        void applyRegkeyOverrides();

    protected:
        EvoInterface * provider;
        Timer * timer;

        NvU32 displayId;
        NvU32 subdeviceIndex;
        unsigned allHeadMask;

        NV0073_CTRL_DFP_GET_INFO_PARAMS     dfpParams;
        NV0073_CTRL_CMD_DP_GET_CAPS_PARAMS  dpParams;

        bool _isDownspreadDisabledByRegkey;

    public:
        EvoMainLink(EvoInterface * provider, Timer * timer);

        virtual bool hasIncreasedWatermarkLimits()
        {
            return _hasIncreasedWatermarkLimits;
        }

        virtual bool hasMultistream()
        {
            return _hasMultistream;
        }

        virtual bool isPC2Disabled()
        {
            return _isPC2Disabled;
        }

        virtual NvU32 getGpuDpSupportedVersions()
        {
            return _gpuSupportedDpVersions;
        }
        virtual NvU32 getUHBRSupported()
        {
            return 0U;
        }
        virtual bool isFECSupported()
        {
            return _isFECSupported;
        }

        virtual bool isStreamCloningEnabled()
        {
            return _isStreamCloningEnabled;
        }

        virtual NvU32 maxLinkRateSupported()
        {
            //
            // For cases where RM asks dplib to honor the maxLinkRate limit defined in DCB, always use
            // this as the limit. Regkey has no meaning in this case.
            // In other cases, based on regkey either honor the dcb limit or the max link rate for the
            // specific GPU architecture. This is needed to avoid regressions on existing chips.
            //
            if ((_applyLinkBwOverrideWarRegVal || _useDfpMaxLinkRateCaps) &&
                (_maxLinkRateSupportedDfp < _maxLinkRateSupportedGpu))
            {
                return _maxLinkRateSupportedDfp;
            }
            return _maxLinkRateSupportedGpu;
        }

        virtual bool isForceRmEdidRequired()
        {
            return _needForceRmEdid;
        }

        virtual bool fetchEdidByRmCtrl(NvU8* edidBuffer, NvU32 bufferSize);
        virtual bool applyEdidOverrideByRmCtrl(NvU8* edidBuffer, NvU32 bufferSize);

        virtual bool isDynamicMuxCapable()
        {
            return _isDynamicMuxCapable;
        }

        virtual bool isInternalPanelDynamicMuxCapable()
        {
            return (_isDynamicMuxCapable && _isEDP);
        }

        virtual bool isDownspreadSupported()
        {
            return _isDownspreadSupported;
        }

        virtual bool isAvoidHBR3WAREnabled()
        {
            return _bAvoidHBR3 && !_bAvoidHBR3DisabledByRegkey;
        }

        // Get GPU DSC capabilities
        virtual void getDscCaps(bool *pbDscSupported,
                                unsigned *pEncoderColorFormatMask,
                                unsigned *pLineBufferSizeKB,
                                unsigned *pRateBufferSizeKB,
                                unsigned *pBitsPerPixelPrecision,
                                unsigned *pMaxNumHztSlices,
                                unsigned *pLineBufferBitDepth)
        {
            if (pbDscSupported)
            {
                *pbDscSupported = _DSC.isDscSupported;
            }

            if (pEncoderColorFormatMask)
            {
                *pEncoderColorFormatMask = _DSC.encoderColorFormatMask;
            }

            if (pLineBufferSizeKB)
            {
                *pLineBufferSizeKB = _DSC.lineBufferSizeKB;
            }

            if (pRateBufferSizeKB)
            {
                *pRateBufferSizeKB = _DSC.rateBufferSizeKB;
            }

            if (pBitsPerPixelPrecision)
            {
                *pBitsPerPixelPrecision = _DSC.bitsPerPixelPrecision;
            }

            if (pMaxNumHztSlices)
            {
                *pMaxNumHztSlices = _DSC.maxNumHztSlices;
            }

            if (pLineBufferBitDepth)
            {
                *pLineBufferBitDepth = _DSC.lineBufferBitDepth;
            }
        }

        virtual NvU32 getRootDisplayId()
        {
            return this->displayId;
        }

        virtual bool isLttprSupported()
        {
            return this->_isLTPhyRepeaterSupported;
        }

        EvoInterface * getProvider()
        {
            return this->provider;
        }

        // Return the current mux state. Returns false if device is not mux capable
        bool getDynamicMuxState(NvU32 *muxState);

        virtual bool physicalLayerSetTestPattern(PatternInfo * patternInfo);
        virtual bool physicalLayerSetDP2xTestPattern(DP2xPatternInfo *patternInfo)
        {
            DP_ASSERT(0 && "DP1x should never get this request.");
            return false;
        }
        virtual bool getUSBCCableIDInfo(NV0073_CTRL_DP_USBC_CABLEID_INFO *cableIDInfo) { return false; }
        virtual void preLinkTraining(NvU32 head);
        virtual void postLinkTraining(NvU32 head);
        virtual NvU32 getRegkeyValue(const char *key);
        virtual const DP_REGKEY_DATABASE& getRegkeyDatabase();
        virtual NvU32 getSorIndex();
        virtual bool isInbandStereoSignalingSupported();
        virtual bool train(const LinkConfiguration & link, bool force, LinkTrainingType linkTrainingType,
                           LinkConfiguration *retLink, bool bSkipLt = false, bool isPostLtAdjRequestGranted = false,
                           unsigned phyRepeaterCount = 0);
        virtual bool retrieveRingBuffer(NvU8 dpRingBuffertype, NvU32 numRecords);
        virtual void getLinkConfig(unsigned & laneCount, NvU64 & linkRate);
        void getLinkConfigWithFEC(unsigned & laneCount, NvU64 & linkRate, bool &bFECEnabled);
        virtual bool getMaxLinkConfigFromUefi(NvU8 &linkRate, NvU8 &laneCount);
        virtual bool setDpMSAParameters(bool bStereoEnable, const NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES_PARAMS &msaparams);
        virtual bool setDpStereoMSAParameters(bool bStereoEnable, const NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES_PARAMS &msaparams);
        bool setFlushMode();
        void clearFlushMode(unsigned headMask, bool testMode=false);

        virtual bool dscCrcTransaction(NvBool bEnable, gpuDscCrc *data, NvU16 *headIndex);

        void triggerACT();
        void configureHDCPRenegotiate(NvU64 cN = HDCP_DUMMY_CN, NvU64 cKsv = HDCP_DUMMY_CKSV, bool bForceReAuth = false,
                                      bool bRxIDMsgPending = false);
        void configureHDCPGetHDCPState(HDCPState &hdcpState);
        bool rmUpdateDynamicDfpCache(NvU32 headIndex, RmDfpCache * dfpCache, NvBool bResetDfp);

        virtual NvU32 headToStream(NvU32 head, bool bSidebandMessageSupported, DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID streamIdentifier = DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID_PRIMARY);

        void configureSingleStream(NvU32 head,
            NvU32 hBlankSym,
            NvU32 vBlankSym,
            bool  bEnhancedFraming,
            NvU32 tuSize,
            NvU32 waterMark,
            DP_COLORFORMAT colorFormat,
            DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID streamId = DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID_PRIMARY,
            DP_SINGLE_HEAD_MULTI_STREAM_MODE singleHeadMultistreamMode = DP_SINGLE_HEAD_MULTI_STREAM_MODE_NONE,
            bool bEnableAudioOverRightPanel = false,
            bool bEnable2Head1Or = false);

        void configureMultiStream(NvU32 head,
            NvU32 hBlankSym,
            NvU32 vBlankSym,
            NvU32 slotStart,
            NvU32 slotEnd,
            NvU32 PBN,
            NvU32 Timeslice,
            DP_COLORFORMAT colorFormat,
            DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID streamIdentifier = DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID_PRIMARY,
            DP_SINGLE_HEAD_MULTI_STREAM_MODE singleHeadMultistreamMode = DP_SINGLE_HEAD_MULTI_STREAM_MODE_NONE,
            bool bEnableAudioOverRightPanel = false,
            bool bEnable2Head1Or = false);

        void configureSingleHeadMultiStreamMode(NvU32 displayIDs[],
                                                NvU32 numStreams,
                                                NvU32 mode,
                                                bool bSetConfig,
                                                NvU8  vbiosPrimaryDispIdIndex = DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID_PRIMARY);

        void configureMsScratchRegisters(NvU32 address,
                                         NvU32 hopCount,
                                         NvU32 driverState);

        bool isActive();
        bool isEDP();
        bool skipPowerdownEdpPanelWhenHeadDetach();
        bool isMSTPCONCapsReadDisabled();
        bool supportMSAOverMST();
        bool controlRateGoverning(NvU32 head, bool enable, bool updateNow);

        bool getDpTestPattern(NV0073_CTRL_DP_TESTPATTERN *testPattern);
        bool setDpTestPattern(NV0073_CTRL_DP_TESTPATTERN testPattern,
                              NvU8 laneMask, NV0073_CTRL_DP_CSTM cstm,
                              NvBool bIsHBR2, NvBool bSkipLaneDataOverride);
        bool getDpLaneData(NvU32 *numLanes, NvU32 *data);
        bool setDpLaneData(NvU32 numLanes, NvU32 *data);
        void configurePowerState(bool bPowerUp);
        NvU32 monitorDenylistInfo(NvU32 ManufacturerID, NvU32 ProductID, DpMonitorDenylistData *pDenylistData);
        NvU32 allocDisplayId();
        bool freeDisplayId(NvU32 displayId);
        virtual bool queryAndUpdateDfpParams();
        virtual bool queryGPUCapability();

        bool getEdpPowerData(bool *panelPowerOn, bool *dpcdPowerStateD0);
        virtual bool vrrRunEnablementStage(unsigned stage, NvU32 *status);

        void configureTriggerSelect(NvU32 head,
            DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID streamIdentifier = DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID_PRIMARY);
        void configureTriggerAll(NvU32 head, bool enable);
        virtual bool configureLinkRateTable(const NvU16 *pLinkRateTable, LinkRates *pLinkRates);
        bool configureFec(const bool bEnableFec);
    };

}

#endif //INCLUDED_DP_EVOADAPTER_H
