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
* Module: dp_mainlink.h                                                     *
*    Mainlink interface implemented by client.                              *
*                                                                           *
\***************************************************************************/
#ifndef INCLUDED_DP_MAINLINK_H
#define INCLUDED_DP_MAINLINK_H

#include "dp_linkconfig.h"
#include "dp_vrr.h"
#include "dp_wardatabase.h"
#include "dp_auxdefs.h"
#include "displayport.h"
#include "ctrl/ctrl0073/ctrl0073dp.h"
#include "dp_regkeydatabase.h"

#define HDCP_DUMMY_CN    (0x1)
#define HDCP_DUMMY_CKSV  (0xFFFFF)

namespace DisplayPort
{
    typedef enum
    {
        NONE,    //Abort it manually
        UNTRUST, //Abort due to Kp mismatch
        UNRELBL, //Abort due to repeated link failure
        KSV_LEN, //Abort due to KSV length
        KSV_SIG, //Abort due to KSV signature
        SRM_SIG, //Abort due to SRM signature
        SRM_REV, //Abort due to SRM revocation
        NORDY,   //Abort due to repeater not ready
        KSVTOP,  //Abort due to KSV topology error
        BADBKSV  //Abort due to invalid Bksv
    }AbortAuthReason;

    // This is also used for DPCD offset 10B. 249
    enum LinkQualityPatternType
    {
        LINK_QUAL_DISABLED,
        LINK_QUAL_D10_2,
        LINK_QUAL_SYM_ERROR,
        LINK_QUAL_PRBS7,
        LINK_QUAL_80BIT_CUST,
        LINK_QUAL_HBR2_COMPLIANCE_EYE,
        LINK_QUAL_CP2520PAT3,
        LINK_QUAL_128B132B_TPS1,
        LINK_QUAL_128B132B_TPS2,
        LINK_QUAL_PRBS9,
        LINK_QUAL_PRBS11,
        LINK_QUAL_PRBS15,
        LINK_QUAL_PRBS23,
        LINK_QUAL_PRBS31,
        LINK_QUAL_264BIT_CUST,
        LINK_QUAL_SQUARE_SEQ_WITH_PRESHOOT_ON_DE_EMPHASIS_ON,
        LINK_QUAL_SQUARE_SEQ_WITH_PRESHOOT_OFF_DE_EMPHASIS_ON,
        LINK_QUAL_SQUARE_SEQ_WITH_PRESHOOT_ON_DE_EMPHASIS_OFF,
        LINK_QUAL_SQUARE_SEQ_WITH_PRESHOOT_OFF_DE_EMPHASIS_OFF,
    };

    typedef struct
    {
        LinkQualityPatternType lqsPattern;

        //
        // 80 bits DP CSTM Test Pattern data;
        //   ctsmLower    takes bits 31:0  (lowest 32 bits)
        //   ctsmMiddle   takes bits 63:32 (middle 32 bits)
        //   ctsmUpper    takes bits 79:64 (highest 16 bits)
        //
        int ctsmLower;
        int ctsmMiddle;
        int ctsmUpper;
    } PatternInfo;

    typedef struct
    {
        LinkQualityPatternType lqsPattern;

        //
        // DP CSTM Test Pattern data
        //   For 264 bits: ctsmData[0]-ctsmData[32]
        //        padding: ctsmData[33-35]
        //
        NvU8    ctsmData[36];
        NvU8    sqNum;
    } DP2xPatternInfo;

    typedef struct
    {
        unsigned char       bcaps;
        unsigned char       bksv[5];
        bool                hdcpCapable;
        unsigned char       updMask;
    }RmDfpCache;

    typedef enum
    {
        NORMAL_LINK_TRAINING,  // full LT
        NO_LINK_TRAINING,
        FAST_LINK_TRAINING,
    }LinkTrainingType;

    typedef enum
    {
        FlushModePhase1,
        FlushModePhase2,
    } FlushModePhase;

    class MainLink : virtual public Object
    {
    public:
        virtual bool physicalLayerSetTestPattern(PatternInfo * patternInfo) = 0;
        virtual bool physicalLayerSetDP2xTestPattern(DP2xPatternInfo * patternInfo) = 0;
        virtual bool getUSBCCableIDInfo(NV0073_CTRL_DP_USBC_CABLEID_INFO *cableIDInfo) = 0;
        //
        //  Wrappers for existing link training RM control calls
        //
        virtual bool train(const LinkConfiguration & link, bool force, LinkTrainingType linkTrainingType,
                           LinkConfiguration *retLink, bool bSkipLt = false, bool isPostLtAdjRequestGranted = false,
                           unsigned phyRepeaterCount = 0) = 0;

        // RM control call to retrieve buffer from RM for DP Library to dump logs
        virtual bool retrieveRingBuffer(NvU8 dpRingBuffertype, NvU32 numRecords) = 0;

        //
        //  Requests to DD to perform pre & post link training steps
        //  which may disconnect and later reconnect the head (For Pre-gf119 GPUs)
        //
        virtual void preLinkTraining(NvU32 head) = 0;
        virtual void postLinkTraining(NvU32 head) = 0;
        virtual NvU32 getRegkeyValue(const char *key) = 0;
        virtual const DP_REGKEY_DATABASE& getRegkeyDatabase() = 0;
        virtual NvU32 getSorIndex() = 0;
        virtual bool isInbandStereoSignalingSupported() = 0;


        virtual bool isEDP() = 0;
        virtual bool supportMSAOverMST() = 0;
        virtual bool isForceRmEdidRequired() = 0;
        virtual bool fetchEdidByRmCtrl(NvU8* edidBuffer, NvU32 bufferSize) = 0;
        virtual bool applyEdidOverrideByRmCtrl(NvU8* edidBuffer, NvU32 bufferSize) = 0;

        // Return if Panel is Dynamic MUX capable
        virtual bool isDynamicMuxCapable() = 0;

        // Return the current mux state. Returns false if not mux capable
        virtual bool getDynamicMuxState(NvU32 *muxState) = 0;

        // Return if Internal panel is Dynamic Mux capable
        virtual bool isInternalPanelDynamicMuxCapable() = 0;

        // Check if we should skip power down eDP when head detached.
        virtual bool skipPowerdownEdpPanelWhenHeadDetach() = 0;

        // Check if we should skip reading PCON Caps in MST case.
        virtual bool isMSTPCONCapsReadDisabled() = 0;

        // Get GPU DSC capabilities
        virtual void getDscCaps(bool *pbDscSupported = NULL,
                                unsigned *pEncoderColorFormatMask = NULL,
                                unsigned *pLineBufferSizeKB = NULL,
                                unsigned *pRateBufferSizeKB = NULL,
                                unsigned *pBitsPerPixelPrecision = NULL,
                                unsigned *pMaxNumHztSlices = NULL,
                                unsigned *pLineBufferBitDepth = NULL) = 0;

        //
        //  Get the current link config.
        //      (Used for the boot case where EFI/VBIOS may have already trained
        //       the link.  We need this to confirm the programming since
        //       we cannot rely on the DPCD registers being correct or sane)
        //
        virtual void getLinkConfig(unsigned &laneCount, NvU64 & linkRate) = 0;
        
        // Get the current link config with FEC
        virtual void getLinkConfigWithFEC(unsigned &laneCount, NvU64 &linkRate, bool &bFECEnable) {};
        // Get the max link config from UEFI.
        virtual bool getMaxLinkConfigFromUefi(NvU8 &linkRate, NvU8 &laneCount) = 0;
        //
        //  Query if a head is attached to this DisplayId
        //
        virtual bool isActive() = 0;

        virtual bool hasIncreasedWatermarkLimits() = 0;
        virtual bool hasMultistream() = 0;
        virtual bool isPC2Disabled() = 0;
        virtual NvU32 getGpuDpSupportedVersions() = 0;
        virtual NvU32 getUHBRSupported() {return 0;}
        virtual bool  isRgFlushSequenceUsed() {return false;}
        virtual bool isStreamCloningEnabled() = 0;
        virtual NvU32 maxLinkRateSupported() = 0;
        virtual bool isLttprSupported() = 0;
        virtual bool isFECSupported() = 0;

        virtual bool setDpMSAParameters(bool bStereoEnable, const NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES_PARAMS &msaparams) = 0;
        virtual bool setDpStereoMSAParameters(bool bStereoEnable, const NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES_PARAMS &msaparams) = 0;
        virtual bool setFlushMode() = 0;
        virtual void clearFlushMode(unsigned headMask, bool testMode=false) = 0;

        //
        //  HDCP Renegotiate and trigger ACT.
        //
        virtual void configureHDCPRenegotiate(NvU64 cN = HDCP_DUMMY_CN, NvU64 cKsv = HDCP_DUMMY_CKSV, bool bForceReAuth = false, bool bRxIDMsgPending = false) = 0;
        virtual void triggerACT() = 0;
        virtual void configureHDCPGetHDCPState(HDCPState &hdcpState) = 0;

        virtual NvU32 headToStream(NvU32 head, bool bSidebandMessageSupported,
            DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID streamIdentifier = DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID_PRIMARY) = 0;

        virtual void configureSingleStream(NvU32 head,
                                    NvU32 hBlankSym,
                                    NvU32 vBlankSym,
                                    bool  bEnhancedFraming,
                                    NvU32 tuSize,
                                    NvU32 waterMark,
                                    DP_COLORFORMAT colorFormat,
                                    DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID streamId = DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID_PRIMARY,
                                    DP_SINGLE_HEAD_MULTI_STREAM_MODE singleHeadMultistreamMode = DP_SINGLE_HEAD_MULTI_STREAM_MODE_NONE,
                                    bool bEnableAudioOverRightPanel = false,
                                    bool bEnable2Head1Or = false)= 0;

        virtual void configureMultiStream(NvU32 head,
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
                                    bool bEnable2Head1Or = false)= 0;

        virtual void configureSingleHeadMultiStreamMode(NvU32 displayIDs[],
                                                        NvU32 numStreams,
                                                        NvU32 mode,
                                                        bool  bSetConfig,
                                                        NvU8  vbiosPrimaryDispIdIndex = DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID_PRIMARY)= 0;

        virtual void configureMsScratchRegisters(NvU32 address,
                                                 NvU32 hopCount,
                                                 NvU32 driverState) = 0;

        virtual bool controlRateGoverning(NvU32 head, bool enable, bool updateNow = true) = 0;
        virtual bool getDpTestPattern(NV0073_CTRL_DP_TESTPATTERN * testPattern) = 0;
        virtual bool setDpTestPattern(NV0073_CTRL_DP_TESTPATTERN testPattern,
                                      NvU8 laneMask, NV0073_CTRL_DP_CSTM cstm,
                                      NvBool bIsHBR2, NvBool bSkipLaneDataOverride = false) = 0;
        virtual bool getDpLaneData(NvU32 *numLanes, NvU32 *data) = 0;
        virtual bool setDpLaneData(NvU32 numLanes, NvU32 *data) = 0;
        virtual bool rmUpdateDynamicDfpCache(NvU32 headIndex, RmDfpCache * dfpCache, NvBool bResetDfp) = 0;
        virtual void configurePowerState(bool bPowerUp) = 0;
        virtual NvU32 monitorDenylistInfo(NvU32 ManufacturerID, NvU32 ProductID, DpMonitorDenylistData *pDenylistData) = 0;
        virtual NvU32 getRootDisplayId() = 0;
        virtual NvU32 allocDisplayId() = 0;
        virtual bool freeDisplayId(NvU32 displayId) = 0;
        virtual bool queryGPUCapability() {return false;}
        virtual bool isAvoidHBR3WAREnabled() = 0;
        virtual bool queryAndUpdateDfpParams() = 0;
        virtual void updateFallbackMap(NvU32 maxLaneCount, LinkRate maxLinkRate, NvU32 sinkUhbrCaps = 0) { return; }
        virtual bool isConnectorUSBTypeC() { return false; }
        virtual bool isCableVconnSourceUnknown() { return false; }
        virtual void invalidateLinkRatesInFallbackTable(const LinkRate linkRate) { return; }

        virtual bool setFlushMode(FlushModePhase phase) { return false; }
        virtual bool clearFlushMode(FlushModePhase phase, NvU32 attachFailedHeadMask = 0, NvU32 headIndex = 0) { return false; }
        virtual bool getDp2xLaneData(NvU32 *numLanes, NvU32 *data) { return false; }
        virtual bool setDp2xLaneData(NvU32 numLanes, NvU32 *data) { return false; }
        virtual bool isSupportedDPLinkConfig(LinkConfiguration &link) {return false; };
        virtual bool getEdpPowerData(bool *panelPowerOn, bool *bDPCDPowerStateD0) = 0;
        virtual bool vrrRunEnablementStage(unsigned stage, NvU32 *status) = 0;

        virtual void configureTriggerSelect(NvU32 head,
            DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID streamIdentifier = DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID_PRIMARY) = 0;

        virtual void configureTriggerAll(NvU32 head, bool enable) = 0;
        virtual bool dscCrcTransaction(NvBool bEnable, gpuDscCrc *data, NvU16 *headIndex){ return false; }
        virtual bool configureLinkRateTable(const NvU16 *pLinkRateTable, LinkRates *pLinkRates) = 0;
        virtual bool configureFec(const bool bEnableFec) = 0;
    };
}

#endif //INCLUDED_DP_MAINLINK_H
