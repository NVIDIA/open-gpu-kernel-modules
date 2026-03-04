/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "common_nvswitch.h"
#include "inforom/inforom_nvswitch.h"
#include "inforom/inforom_nvl_v3_nvswitch.h"
#include "lr10/lr10.h"

NvlStatus inforom_nvl_v3_map_error
(
    INFOROM_NVLINK_ERROR_TYPES error,
    NvU8  *pHeader,
    NvU16 *pMetadata,
    NvU8  *pErrorSubtype,
    INFOROM_NVL_ERROR_BLOCK_TYPE *pBlockType
)
{
    static const struct
    { NvU8  header;
      NvU16 metadata;
      NvU8  errorSubtype;
      INFOROM_NVL_ERROR_BLOCK_TYPE blockType;
    } lut[] =
    {
        LUT_ELEMENT(DL,     _RX, _FAULT_DL_PROTOCOL_FATAL,              _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(DL,     _RX, _FAULT_SUBLINK_CHANGE_FATAL,           _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(DL,     _RX, _FLIT_CRC_CORR,                        _ACCUM, _CORRECTABLE),
        LUT_ELEMENT(DL,     _RX, _LANE0_CRC_CORR,                       _ACCUM, _CORRECTABLE),
        LUT_ELEMENT(DL,     _RX, _LANE1_CRC_CORR,                       _ACCUM, _CORRECTABLE),
        LUT_ELEMENT(DL,     _RX, _LANE2_CRC_CORR,                       _ACCUM, _CORRECTABLE),
        LUT_ELEMENT(DL,     _RX, _LANE3_CRC_CORR,                       _ACCUM, _CORRECTABLE),
        LUT_ELEMENT(DL,     _RX, _LINK_REPLAY_EVENTS_CORR,              _ACCUM, _CORRECTABLE),
        LUT_ELEMENT(DL,     _TX, _FAULT_RAM_FATAL,                      _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(DL,     _TX, _FAULT_INTERFACE_FATAL,                _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(DL,     _TX, _FAULT_SUBLINK_CHANGE_FATAL,           _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(DL,     _TX, _LINK_REPLAY_EVENTS_CORR,              _ACCUM, _CORRECTABLE),
        LUT_ELEMENT(DL,     _NA, _LTSSM_FAULT_UP_FATAL,                 _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(DL,     _NA, _LTSSM_FAULT_DOWN_FATAL,               _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(DL,     _NA, _LINK_RECOVERY_EVENTS_CORR,            _ACCUM, _CORRECTABLE),
        LUT_ELEMENT(TLC,    _RX, _DL_HDR_PARITY_ERR_FATAL,              _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(TLC,    _RX, _DL_DATA_PARITY_ERR_FATAL,             _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(TLC,    _RX, _DL_CTRL_PARITY_ERR_FATAL,             _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(TLC,    _RX, _INVALID_AE_FATAL,                     _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(TLC,    _RX, _INVALID_BE_FATAL,                     _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(TLC,    _RX, _INVALID_ADDR_ALIGN_FATAL,             _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(TLC,    _RX, _PKTLEN_ERR_FATAL,                     _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(TLC,    _RX, _RSVD_PACKET_STATUS_ERR_FATAL,         _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(TLC,    _RX, _RSVD_CACHE_ATTR_PROBE_REQ_ERR_FATAL,  _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(TLC,    _RX, _RSVD_CACHE_ATTR_PROBE_RSP_ERR_FATAL,  _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(TLC,    _RX, _DATLEN_GT_RMW_REQ_MAX_ERR_FATAL,      _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(TLC,    _RX, _DATLEN_LT_ATR_RSP_MIN_ERR_FATAL,      _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(TLC,    _RX, _INVALID_CR_FATAL,                     _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(TLC,    _RX, _INVALID_COLLAPSED_RESPONSE_FATAL,     _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(TLC,    _RX, _HDR_OVERFLOW_FATAL,                   _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(TLC,    _RX, _DATA_OVERFLOW_FATAL,                  _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(TLC,    _RX, _STOMP_DETECTED_FATAL,                 _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(TLC,    _RX, _RSVD_CMD_ENC_FATAL,                   _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(TLC,    _RX, _RSVD_DAT_LEN_ENC_FATAL,               _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(TLC,    _RX, _INVALID_PO_FOR_CACHE_ATTR_FATAL,      _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(TLC,    _RX, _RSP_STATUS_HW_ERR_NONFATAL,           _COUNT, _UNCORRECTABLE_NONFATAL),
        LUT_ELEMENT(TLC,    _RX, _RSP_STATUS_UR_ERR_NONFATAL,           _COUNT, _UNCORRECTABLE_NONFATAL),
        LUT_ELEMENT(TLC,    _RX, _RSP_STATUS_PRIV_ERR_NONFATAL,         _COUNT, _UNCORRECTABLE_NONFATAL),
        LUT_ELEMENT(TLC,    _RX, _POISON_NONFATAL,                      _COUNT, _UNCORRECTABLE_NONFATAL),
        LUT_ELEMENT(TLC,    _RX, _AN1_HEARTBEAT_TIMEOUT_NONFATAL,       _COUNT, _UNCORRECTABLE_NONFATAL),
        LUT_ELEMENT(TLC,    _RX, _ILLEGAL_PRI_WRITE_NONFATAL,           _COUNT, _UNCORRECTABLE_NONFATAL),
        LUT_ELEMENT(TLC,    _TX, _DL_CREDIT_PARITY_ERR_FATAL,           _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(TLC,    _TX, _NCISOC_HDR_ECC_DBE_FATAL,             _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(TLC,    _TX, _NCISOC_PARITY_ERR_FATAL,              _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(TLC,    _TX, _ILLEGAL_PRI_WRITE_NONFATAL,           _COUNT, _UNCORRECTABLE_NONFATAL),
        LUT_ELEMENT(TLC,    _TX, _AN1_TIMEOUT_VC0_NONFATAL,             _COUNT, _UNCORRECTABLE_NONFATAL),
        LUT_ELEMENT(TLC,    _TX, _AN1_TIMEOUT_VC1_NONFATAL,             _COUNT, _UNCORRECTABLE_NONFATAL),
        LUT_ELEMENT(TLC,    _TX, _AN1_TIMEOUT_VC2_NONFATAL,             _COUNT, _UNCORRECTABLE_NONFATAL),
        LUT_ELEMENT(TLC,    _TX, _AN1_TIMEOUT_VC3_NONFATAL,             _COUNT, _UNCORRECTABLE_NONFATAL),
        LUT_ELEMENT(TLC,    _TX, _AN1_TIMEOUT_VC4_NONFATAL,             _COUNT, _UNCORRECTABLE_NONFATAL),
        LUT_ELEMENT(TLC,    _TX, _AN1_TIMEOUT_VC5_NONFATAL,             _COUNT, _UNCORRECTABLE_NONFATAL),
        LUT_ELEMENT(TLC,    _TX, _AN1_TIMEOUT_VC6_NONFATAL,             _COUNT, _UNCORRECTABLE_NONFATAL),
        LUT_ELEMENT(TLC,    _TX, _AN1_TIMEOUT_VC7_NONFATAL,             _COUNT, _UNCORRECTABLE_NONFATAL),
        LUT_ELEMENT(TLC,    _TX, _POISON_NONFATAL,                      _COUNT, _UNCORRECTABLE_NONFATAL),
        LUT_ELEMENT(TLC,    _TX, _RSP_STATUS_HW_ERR_NONFATAL,           _COUNT, _UNCORRECTABLE_NONFATAL),
        LUT_ELEMENT(TLC,    _TX, _RSP_STATUS_UR_ERR_NONFATAL,           _COUNT, _UNCORRECTABLE_NONFATAL),
        LUT_ELEMENT(TLC,    _TX, _RSP_STATUS_PRIV_ERR_NONFATAL,         _COUNT, _UNCORRECTABLE_NONFATAL),
        LUT_ELEMENT(NVLIPT, _NA, _SLEEP_WHILE_ACTIVE_LINK_FATAL,        _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(NVLIPT, _NA, _RSTSEQ_PHYCTL_TIMEOUT_FATAL,          _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(NVLIPT, _NA, _RSTSEQ_CLKCTL_TIMEOUT_FATAL,          _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(NVLIPT, _NA, _CLKCTL_ILLEGAL_REQUEST_FATAL,         _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(NVLIPT, _NA, _RSTSEQ_PLL_TIMEOUT_FATAL,             _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(NVLIPT, _NA, _RSTSEQ_PHYARB_TIMEOUT_FATAL,          _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(NVLIPT, _NA, _ILLEGAL_LINK_STATE_REQUEST_NONFATAL,  _COUNT, _UNCORRECTABLE_NONFATAL),
        LUT_ELEMENT(NVLIPT, _NA, _FAILED_MINION_REQUEST_NONFATAL,       _COUNT, _UNCORRECTABLE_NONFATAL),
        LUT_ELEMENT(NVLIPT, _NA, _RESERVED_REQUEST_VALUE_NONFATAL,      _COUNT, _UNCORRECTABLE_NONFATAL),
        LUT_ELEMENT(NVLIPT, _NA, _LINK_STATE_WRITE_WHILE_BUSY_NONFATAL, _COUNT, _UNCORRECTABLE_NONFATAL),
        LUT_ELEMENT(NVLIPT, _NA, _WRITE_TO_LOCKED_SYSTEM_REG_NONFATAL,  _COUNT, _UNCORRECTABLE_NONFATAL),
        LUT_ELEMENT(NVLIPT, _NA, _LINK_STATE_REQUEST_TIMEOUT_NONFATAL,  _COUNT, _UNCORRECTABLE_NONFATAL),
        // TODO 3014908 log these in the NVL object until we have ECC object support
        LUT_ELEMENT(TLC,    _RX, _HDR_RAM_ECC_DBE_FATAL,                _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(TLC,    _RX, _DAT0_RAM_ECC_DBE_FATAL,               _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(TLC,    _RX, _DAT1_RAM_ECC_DBE_FATAL,               _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(TLC,    _TX, _CREQ_DAT_RAM_ECC_DBE_NONFATAL,        _COUNT, _UNCORRECTABLE_NONFATAL),
        LUT_ELEMENT(TLC,    _TX, _RSP_DAT_RAM_ECC_DBE_NONFATAL,         _COUNT, _UNCORRECTABLE_NONFATAL),
        LUT_ELEMENT(TLC,    _TX, _COM_DAT_RAM_ECC_DBE_NONFATAL,         _COUNT, _UNCORRECTABLE_NONFATAL),
        LUT_ELEMENT(TLC,    _TX, _RSP1_DAT_RAM_ECC_DBE_FATAL,           _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(DL,     _NA, _PHY_A_FATAL,                          _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(DL,     _RX, _CRC_COUNTER_FATAL,                    _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(DL,     _TX, _PL_ERROR_FATAL,                       _COUNT, _UNCORRECTABLE_FATAL),
        LUT_ELEMENT(DL,     _RX, _PL_ERROR_FATAL,                       _COUNT, _UNCORRECTABLE_FATAL)
    };

    ct_assert(INFOROM_NVLINK_DL_RX_FAULT_DL_PROTOCOL_FATAL == 0);
    ct_assert(INFOROM_NVLINK_DL_RX_FAULT_SUBLINK_CHANGE_FATAL == 1);
    ct_assert(INFOROM_NVLINK_DL_RX_FLIT_CRC_CORR == 2);
    ct_assert(INFOROM_NVLINK_DL_RX_LANE0_CRC_CORR == 3);
    ct_assert(INFOROM_NVLINK_DL_RX_LANE1_CRC_CORR == 4);
    ct_assert(INFOROM_NVLINK_DL_RX_LANE2_CRC_CORR == 5);
    ct_assert(INFOROM_NVLINK_DL_RX_LANE3_CRC_CORR == 6);
    ct_assert(INFOROM_NVLINK_DL_RX_LINK_REPLAY_EVENTS_CORR == 7);
    ct_assert(INFOROM_NVLINK_DL_TX_FAULT_RAM_FATAL == 8);
    ct_assert(INFOROM_NVLINK_DL_TX_FAULT_INTERFACE_FATAL == 9);
    ct_assert(INFOROM_NVLINK_DL_TX_FAULT_SUBLINK_CHANGE_FATAL == 10);
    ct_assert(INFOROM_NVLINK_DL_TX_LINK_REPLAY_EVENTS_CORR == 11);
    ct_assert(INFOROM_NVLINK_DL_LTSSM_FAULT_UP_FATAL == 12);
    ct_assert(INFOROM_NVLINK_DL_LTSSM_FAULT_DOWN_FATAL == 13);
    ct_assert(INFOROM_NVLINK_DL_LINK_RECOVERY_EVENTS_CORR == 14);
    ct_assert(INFOROM_NVLINK_TLC_RX_DL_HDR_PARITY_ERR_FATAL == 15);
    ct_assert(INFOROM_NVLINK_TLC_RX_DL_DATA_PARITY_ERR_FATAL == 16);
    ct_assert(INFOROM_NVLINK_TLC_RX_DL_CTRL_PARITY_ERR_FATAL == 17);
    ct_assert(INFOROM_NVLINK_TLC_RX_INVALID_AE_FATAL == 18);
    ct_assert(INFOROM_NVLINK_TLC_RX_INVALID_BE_FATAL == 19);
    ct_assert(INFOROM_NVLINK_TLC_RX_INVALID_ADDR_ALIGN_FATAL == 20);
    ct_assert(INFOROM_NVLINK_TLC_RX_PKTLEN_ERR_FATAL == 21);
    ct_assert(INFOROM_NVLINK_TLC_RX_RSVD_PACKET_STATUS_ERR_FATAL == 22);
    ct_assert(INFOROM_NVLINK_TLC_RX_RSVD_CACHE_ATTR_PROBE_REQ_ERR_FATAL == 23);
    ct_assert(INFOROM_NVLINK_TLC_RX_RSVD_CACHE_ATTR_PROBE_RSP_ERR_FATAL == 24);
    ct_assert(INFOROM_NVLINK_TLC_RX_DATLEN_GT_RMW_REQ_MAX_ERR_FATAL == 25);
    ct_assert(INFOROM_NVLINK_TLC_RX_DATLEN_LT_ATR_RSP_MIN_ERR_FATAL == 26);
    ct_assert(INFOROM_NVLINK_TLC_RX_INVALID_CR_FATAL == 27);
    ct_assert(INFOROM_NVLINK_TLC_RX_INVALID_COLLAPSED_RESPONSE_FATAL == 28);
    ct_assert(INFOROM_NVLINK_TLC_RX_HDR_OVERFLOW_FATAL == 29);
    ct_assert(INFOROM_NVLINK_TLC_RX_DATA_OVERFLOW_FATAL == 30);
    ct_assert(INFOROM_NVLINK_TLC_RX_STOMP_DETECTED_FATAL == 31);
    ct_assert(INFOROM_NVLINK_TLC_RX_RSVD_CMD_ENC_FATAL == 32);
    ct_assert(INFOROM_NVLINK_TLC_RX_RSVD_DAT_LEN_ENC_FATAL == 33);
    ct_assert(INFOROM_NVLINK_TLC_RX_INVALID_PO_FOR_CACHE_ATTR_FATAL == 34);
    ct_assert(INFOROM_NVLINK_TLC_RX_RSP_STATUS_HW_ERR_NONFATAL == 35);
    ct_assert(INFOROM_NVLINK_TLC_RX_RSP_STATUS_UR_ERR_NONFATAL == 36);
    ct_assert(INFOROM_NVLINK_TLC_RX_RSP_STATUS_PRIV_ERR_NONFATAL == 37);
    ct_assert(INFOROM_NVLINK_TLC_RX_POISON_NONFATAL == 38);
    ct_assert(INFOROM_NVLINK_TLC_RX_AN1_HEARTBEAT_TIMEOUT_NONFATAL == 39);
    ct_assert(INFOROM_NVLINK_TLC_RX_ILLEGAL_PRI_WRITE_NONFATAL == 40);
    ct_assert(INFOROM_NVLINK_TLC_TX_DL_CREDIT_PARITY_ERR_FATAL == 41);
    ct_assert(INFOROM_NVLINK_TLC_TX_NCISOC_HDR_ECC_DBE_FATAL == 42);
    ct_assert(INFOROM_NVLINK_TLC_TX_NCISOC_PARITY_ERR_FATAL == 43);
    ct_assert(INFOROM_NVLINK_TLC_TX_ILLEGAL_PRI_WRITE_NONFATAL == 44);
    ct_assert(INFOROM_NVLINK_TLC_TX_AN1_TIMEOUT_VC0_NONFATAL == 45);
    ct_assert(INFOROM_NVLINK_TLC_TX_AN1_TIMEOUT_VC1_NONFATAL == 46);
    ct_assert(INFOROM_NVLINK_TLC_TX_AN1_TIMEOUT_VC2_NONFATAL == 47);
    ct_assert(INFOROM_NVLINK_TLC_TX_AN1_TIMEOUT_VC3_NONFATAL == 48);
    ct_assert(INFOROM_NVLINK_TLC_TX_AN1_TIMEOUT_VC4_NONFATAL == 49);
    ct_assert(INFOROM_NVLINK_TLC_TX_AN1_TIMEOUT_VC5_NONFATAL == 50);
    ct_assert(INFOROM_NVLINK_TLC_TX_AN1_TIMEOUT_VC6_NONFATAL == 51);
    ct_assert(INFOROM_NVLINK_TLC_TX_AN1_TIMEOUT_VC7_NONFATAL == 52);
    ct_assert(INFOROM_NVLINK_TLC_TX_POISON_NONFATAL == 53);
    ct_assert(INFOROM_NVLINK_TLC_TX_RSP_STATUS_HW_ERR_NONFATAL == 54);
    ct_assert(INFOROM_NVLINK_TLC_TX_RSP_STATUS_UR_ERR_NONFATAL == 55);
    ct_assert(INFOROM_NVLINK_TLC_TX_RSP_STATUS_PRIV_ERR_NONFATAL == 56);
    ct_assert(INFOROM_NVLINK_NVLIPT_SLEEP_WHILE_ACTIVE_LINK_FATAL == 57);
    ct_assert(INFOROM_NVLINK_NVLIPT_RSTSEQ_PHYCTL_TIMEOUT_FATAL == 58);
    ct_assert(INFOROM_NVLINK_NVLIPT_RSTSEQ_CLKCTL_TIMEOUT_FATAL == 59);
    ct_assert(INFOROM_NVLINK_NVLIPT_CLKCTL_ILLEGAL_REQUEST_FATAL == 60);
    ct_assert(INFOROM_NVLINK_NVLIPT_RSTSEQ_PLL_TIMEOUT_FATAL == 61);
    ct_assert(INFOROM_NVLINK_NVLIPT_RSTSEQ_PHYARB_TIMEOUT_FATAL == 62);
    ct_assert(INFOROM_NVLINK_NVLIPT_ILLEGAL_LINK_STATE_REQUEST_NONFATAL == 63);
    ct_assert(INFOROM_NVLINK_NVLIPT_FAILED_MINION_REQUEST_NONFATAL == 64);
    ct_assert(INFOROM_NVLINK_NVLIPT_RESERVED_REQUEST_VALUE_NONFATAL == 65);
    ct_assert(INFOROM_NVLINK_NVLIPT_LINK_STATE_WRITE_WHILE_BUSY_NONFATAL == 66);
    ct_assert(INFOROM_NVLINK_NVLIPT_WRITE_TO_LOCKED_SYSTEM_REG_NONFATAL == 67);
    ct_assert(INFOROM_NVLINK_NVLIPT_LINK_STATE_REQUEST_TIMEOUT_NONFATAL == 68);
    ct_assert(INFOROM_NVLINK_TLC_RX_HDR_RAM_ECC_DBE_FATAL == 69);
    ct_assert(INFOROM_NVLINK_TLC_RX_DAT0_RAM_ECC_DBE_FATAL == 70);
    ct_assert(INFOROM_NVLINK_TLC_RX_DAT1_RAM_ECC_DBE_FATAL == 71);
    ct_assert(INFOROM_NVLINK_TLC_TX_CREQ_DAT_RAM_ECC_DBE_NONFATAL == 72);
    ct_assert(INFOROM_NVLINK_TLC_TX_RSP_DAT_RAM_ECC_DBE_NONFATAL == 73);
    ct_assert(INFOROM_NVLINK_TLC_TX_COM_DAT_RAM_ECC_DBE_NONFATAL == 74);
    ct_assert(INFOROM_NVLINK_TLC_TX_RSP1_DAT_RAM_ECC_DBE_FATAL == 75);
    ct_assert(INFOROM_NVLINK_DL_PHY_A_FATAL == 76);
    ct_assert(INFOROM_NVLINK_DL_RX_CRC_COUNTER_FATAL == 77);
    ct_assert(INFOROM_NVLINK_DL_TX_PL_ERROR_FATAL == 78);
    ct_assert(INFOROM_NVLINK_DL_RX_PL_ERROR_FATAL == 79);

    ct_assert(NV_ARRAY_ELEMENTS(lut) == INFOROM_NVLINK_MAX_ERROR_TYPE);

    if (error >= NV_ARRAY_ELEMENTS(lut))
    {
        return -NVL_BAD_ARGS;
    }

    *pHeader       = lut[error].header;
    *pMetadata     = lut[error].metadata;
    *pErrorSubtype = lut[error].errorSubtype;
    *pBlockType    = lut[error].blockType;
    return NVL_SUCCESS;
}

NvlStatus
inforom_nvl_v3_encode_nvlipt_error_subtype
(
    NvU8 localLinkIdx,
    NvU8 *pSubtype
)
{
    static const NvBool linkIdxValidLut[] =
    {
        NV_TRUE,
        NV_TRUE,
        NV_TRUE,
        NV_FALSE,
        NV_FALSE,
        NV_FALSE,
        NV_TRUE,
        NV_TRUE,
        NV_TRUE,
        NV_TRUE,
        NV_TRUE,
        NV_TRUE
    };

    ct_assert(NVLIPT_NA_SLEEP_WHILE_ACTIVE_LINK_FATAL_COUNT == 0);
    ct_assert(NVLIPT_NA_RSTSEQ_PHYCTL_TIMEOUT_FATAL_COUNT == 1);
    ct_assert(NVLIPT_NA_RSTSEQ_CLKCTL_TIMEOUT_FATAL_COUNT == 2);
    ct_assert(NVLIPT_NA_CLKCTL_ILLEGAL_REQUEST_FATAL_COUNT == 3);
    ct_assert(NVLIPT_NA_RSTSEQ_PLL_TIMEOUT_FATAL_COUNT == 4);
    ct_assert(NVLIPT_NA_RSTSEQ_PHYARB_TIMEOUT_FATAL_COUNT == 5);
    ct_assert(NVLIPT_NA_ILLEGAL_LINK_STATE_REQUEST_NONFATAL_COUNT == 6);
    ct_assert(NVLIPT_NA_FAILED_MINION_REQUEST_NONFATAL_COUNT == 7);
    ct_assert(NVLIPT_NA_RESERVED_REQUEST_VALUE_NONFATAL_COUNT == 8);
    ct_assert(NVLIPT_NA_LINK_STATE_WRITE_WHILE_BUSY_NONFATAL_COUNT == 9);
    ct_assert(NVLIPT_NA_WRITE_TO_LOCKED_SYSTEM_REG_NONFATAL_COUNT == 10);
    ct_assert(NVLIPT_NA_LINK_STATE_REQUEST_TIMEOUT_NONFATAL_COUNT == 11);

    if ((localLinkIdx >= NV_INFOROM_NVL_OBJECT_V3_NVLIPT_ERROR_LINK_ID_COMMON) ||
            (*pSubtype >= NV_ARRAY_ELEMENTS(linkIdxValidLut)))
    {
        return -NVL_BAD_ARGS;
    }

    if (linkIdxValidLut[*pSubtype])
    {
        *pSubtype = FLD_SET_DRF_NUM(_INFOROM_NVL_OBJECT_V3, _NVLIPT_ERROR,
                                    _LINK_ID, localLinkIdx, *pSubtype);
    }
    else
    {
        *pSubtype = FLD_SET_DRF(_INFOROM_NVL_OBJECT_V3, _NVLIPT_ERROR, _LINK_ID,
                                _COMMON, *pSubtype);
    }

    return NVL_SUCCESS;
}

NvBool
inforom_nvl_v3_should_replace_error_rate_entry
(
    INFOROM_NVL_OBJECT_V3_CORRECTABLE_ERROR_RATE *pErrorRate,
    NvU32  flitCrcRate,
    NvU32 *pLaneCrcRates
)
{
    NvU32 i;
    NvU64 currentLaneCrcRateSum = 0;
    NvU64 maxLaneCrcRateSum     = 0;

    for (i = 0; i < NV_ARRAY_ELEMENTS(pErrorRate->laneCrcErrorsPerMinute); i++)
    {
        currentLaneCrcRateSum += pLaneCrcRates[i];
        maxLaneCrcRateSum     += pErrorRate->laneCrcErrorsPerMinute[i];
    }

    return (flitCrcRate > pErrorRate->flitCrcErrorsPerMinute) ||
                (currentLaneCrcRateSum > maxLaneCrcRateSum);
}

void
inforom_nvl_v3_seconds_to_day_and_month
(
    NvU32 sec,
    NvU32 *pDay,
    NvU32 *pMonth
)
{
    *pDay   = sec / (60 * 60 * 24);
    *pMonth = *pDay / 30;
}

void
inforom_nvl_v3_update_error_rate_entry
(
    INFOROM_NVL_OBJECT_V3_CORRECTABLE_ERROR_RATE *pErrorRate,
    NvU32 newSec,
    NvU32 newFlitCrcRate,
    NvU32 *pNewLaneCrcRates
)
{
    pErrorRate->lastUpdated = newSec;
    pErrorRate->flitCrcErrorsPerMinute = newFlitCrcRate;
    nvswitch_os_memcpy(pErrorRate->laneCrcErrorsPerMinute, pNewLaneCrcRates,
                       sizeof(pErrorRate->laneCrcErrorsPerMinute));
}

NvlStatus 
inforom_nvl_v3_map_error_to_userspace_error
(
    nvswitch_device *device,
    INFOROM_NVL_OBJECT_V3_ERROR_ENTRY *pErrorLog,
    NVSWITCH_NVLINK_ERROR_ENTRY *pNvlError
)
{
    static const NvU32 DL_RX_ERRORS[] = 
    {
        NVSWITCH_NVLINK_ERR_DL_RX_FAULT_DL_PROTOCOL_FATAL,
        NVSWITCH_NVLINK_ERR_DL_RX_FAULT_SUBLINK_CHANGE_FATAL,
        NVSWITCH_NVLINK_ERR_DL_RX_FLIT_CRC_CORR, 
        NVSWITCH_NVLINK_ERR_DL_RX_LANE0_CRC_CORR,
        NVSWITCH_NVLINK_ERR_DL_RX_LANE1_CRC_CORR,
        NVSWITCH_NVLINK_ERR_DL_RX_LANE2_CRC_CORR,
        NVSWITCH_NVLINK_ERR_DL_RX_LANE3_CRC_CORR,
        NVSWITCH_NVLINK_ERR_DL_RX_LINK_REPLAY_EVENTS_CORR
    };

    static const NvU32 DL_TX_ERRORS[] = 
    {
        NVSWITCH_NVLINK_ERR_DL_TX_FAULT_RAM_FATAL,
        NVSWITCH_NVLINK_ERR_DL_TX_FAULT_INTERFACE_FATAL,
        NVSWITCH_NVLINK_ERR_DL_TX_FAULT_SUBLINK_CHANGE_FATAL,
        NVSWITCH_NVLINK_ERR_DL_TX_LINK_REPLAY_EVENTS_CORR
    };

    static const NvU32 DL_NA_ERRORS[] = 
    {
        NVSWITCH_NVLINK_ERR_DL_LTSSM_FAULT_UP_FATAL,
        NVSWITCH_NVLINK_ERR_DL_LTSSM_FAULT_DOWN_FATAL,
        NVSWITCH_NVLINK_ERR_DL_LINK_RECOVERY_EVENTS_CORR
    };

    static const NvU32 TLC_RX_ERRORS[] = 
    {
        NVSWITCH_NVLINK_ERR_TLC_RX_DL_HDR_PARITY_ERR_FATAL,
        NVSWITCH_NVLINK_ERR_TLC_RX_DL_DATA_PARITY_ERR_FATAL,
        NVSWITCH_NVLINK_ERR_TLC_RX_DL_CTRL_PARITY_ERR_FATAL,
        NVSWITCH_NVLINK_ERR_TLC_RX_INVALID_AE_FATAL,
        NVSWITCH_NVLINK_ERR_TLC_RX_INVALID_BE_FATAL,
        NVSWITCH_NVLINK_ERR_TLC_RX_INVALID_ADDR_ALIGN_FATAL,
        NVSWITCH_NVLINK_ERR_TLC_RX_PKTLEN_ERR_FATAL,
        NVSWITCH_NVLINK_ERR_TLC_RX_RSVD_PACKET_STATUS_ERR_FATAL,
        NVSWITCH_NVLINK_ERR_TLC_RX_RSVD_CACHE_ATTR_PROBE_REQ_ERR_FATAL,
        NVSWITCH_NVLINK_ERR_TLC_RX_RSVD_CACHE_ATTR_PROBE_RSP_ERR_FATAL,
        NVSWITCH_NVLINK_ERR_TLC_RX_DATLEN_GT_RMW_REQ_MAX_ERR_FATAL,
        NVSWITCH_NVLINK_ERR_TLC_RX_DATLEN_LT_ATR_RSP_MIN_ERR_FATAL,
        NVSWITCH_NVLINK_ERR_TLC_RX_INVALID_CR_FATAL,
        NVSWITCH_NVLINK_ERR_TLC_RX_INVALID_COLLAPSED_RESPONSE_FATAL,
        NVSWITCH_NVLINK_ERR_TLC_RX_HDR_OVERFLOW_FATAL,
        NVSWITCH_NVLINK_ERR_TLC_RX_DATA_OVERFLOW_FATAL,
        NVSWITCH_NVLINK_ERR_TLC_RX_STOMP_DETECTED_FATAL,
        NVSWITCH_NVLINK_ERR_TLC_RX_RSVD_CMD_ENC_FATAL,
        NVSWITCH_NVLINK_ERR_TLC_RX_RSVD_DAT_LEN_ENC_FATAL,
        NVSWITCH_NVLINK_ERR_TLC_RX_INVALID_PO_FOR_CACHE_ATTR_FATAL,
        NVSWITCH_NVLINK_ERR_TLC_RX_RSP_STATUS_HW_ERR_NONFATAL,
        NVSWITCH_NVLINK_ERR_TLC_RX_RSP_STATUS_UR_ERR_NONFATAL,
        NVSWITCH_NVLINK_ERR_TLC_RX_RSP_STATUS_PRIV_ERR_NONFATAL,
        NVSWITCH_NVLINK_ERR_TLC_RX_POISON_NONFATAL,
        NVSWITCH_NVLINK_ERR_TLC_RX_AN1_HEARTBEAT_TIMEOUT_NONFATAL,
        NVSWITCH_NVLINK_ERR_TLC_RX_ILLEGAL_PRI_WRITE_NONFATAL,
        NVSWITCH_NVLINK_ERR_TLC_RX_HDR_RAM_ECC_DBE_FATAL,
        NVSWITCH_NVLINK_ERR_TLC_RX_DAT0_RAM_ECC_DBE_FATAL,
        NVSWITCH_NVLINK_ERR_TLC_RX_DAT1_RAM_ECC_DBE_FATAL
    };

    static const NvU32 TLC_TX_ERRORS[] = 
    {
        NVSWITCH_NVLINK_ERR_TLC_TX_DL_CREDIT_PARITY_ERR_FATAL,
        NVSWITCH_NVLINK_ERR_TLC_TX_NCISOC_HDR_ECC_DBE_FATAL,
        NVSWITCH_NVLINK_ERR_TLC_TX_NCISOC_PARITY_ERR_FATAL,
        NVSWITCH_NVLINK_ERR_TLC_TX_ILLEGAL_PRI_WRITE_NONFATAL,
        NVSWITCH_NVLINK_ERR_TLC_TX_AN1_TIMEOUT_VC0_NONFATAL,
        NVSWITCH_NVLINK_ERR_TLC_TX_AN1_TIMEOUT_VC1_NONFATAL,
        NVSWITCH_NVLINK_ERR_TLC_TX_AN1_TIMEOUT_VC2_NONFATAL,
        NVSWITCH_NVLINK_ERR_TLC_TX_AN1_TIMEOUT_VC3_NONFATAL,
        NVSWITCH_NVLINK_ERR_TLC_TX_AN1_TIMEOUT_VC4_NONFATAL,
        NVSWITCH_NVLINK_ERR_TLC_TX_AN1_TIMEOUT_VC5_NONFATAL,
        NVSWITCH_NVLINK_ERR_TLC_TX_AN1_TIMEOUT_VC6_NONFATAL,
        NVSWITCH_NVLINK_ERR_TLC_TX_AN1_TIMEOUT_VC7_NONFATAL,
        NVSWITCH_NVLINK_ERR_TLC_TX_POISON_NONFATAL,
        NVSWITCH_NVLINK_ERR_TLC_TX_RSP_STATUS_HW_ERR_NONFATAL,
        NVSWITCH_NVLINK_ERR_TLC_TX_RSP_STATUS_UR_ERR_NONFATAL,
        NVSWITCH_NVLINK_ERR_TLC_TX_RSP_STATUS_PRIV_ERR_NONFATAL,
        NVSWITCH_NVLINK_ERR_TLC_TX_CREQ_DAT_RAM_ECC_DBE_NONFATAL,
        NVSWITCH_NVLINK_ERR_TLC_TX_RSP_DAT_RAM_ECC_DBE_NONFATAL,
        NVSWITCH_NVLINK_ERR_TLC_TX_COM_DAT_RAM_ECC_DBE_NONFATAL,
        NVSWITCH_NVLINK_ERR_TLC_TX_RSP1_DAT_RAM_ECC_DBE_FATAL
    };

    static const NvU32 LIPT_ERRORS[] = 
    {
        NVSWITCH_NVLINK_ERR_NVLIPT_SLEEP_WHILE_ACTIVE_LINK_FATAL,
        NVSWITCH_NVLINK_ERR_NVLIPT_RSTSEQ_PHYCTL_TIMEOUT_FATAL,
        NVSWITCH_NVLINK_ERR_NVLIPT_RSTSEQ_CLKCTL_TIMEOUT_FATAL,
        NVSWITCH_NVLINK_ERR_NVLIPT_CLKCTL_ILLEGAL_REQUEST_FATAL,
        NVSWITCH_NVLINK_ERR_NVLIPT_RSTSEQ_PLL_TIMEOUT_FATAL,
        NVSWITCH_NVLINK_ERR_NVLIPT_RSTSEQ_PHYARB_TIMEOUT_FATAL,
        NVSWITCH_NVLINK_ERR_NVLIPT_ILLEGAL_LINK_STATE_REQUEST_NONFATAL,
        NVSWITCH_NVLINK_ERR_NVLIPT_FAILED_MINION_REQUEST_NONFATAL,
        NVSWITCH_NVLINK_ERR_NVLIPT_RESERVED_REQUEST_VALUE_NONFATAL,
        NVSWITCH_NVLINK_ERR_NVLIPT_LINK_STATE_WRITE_WHILE_BUSY_NONFATAL,
        NVSWITCH_NVLINK_ERR_NVLIPT_WRITE_TO_LOCKED_SYSTEM_REG_NONFATAL,
        NVSWITCH_NVLINK_ERR_NVLIPT_LINK_STATE_REQUEST_TIMEOUT_NONFATAL
    };

    NvU32 subType = 0;
    NvU8 nvliptInstance = 0, localLinkIdx = 0;
    NvU8 numLinksPerNvlipt = device->hal.nvswitch_get_num_links_per_nvlipt(device);;

    if ((pErrorLog == NULL) || (pNvlError == NULL))
    {
        return -NVL_BAD_ARGS;
    }

    subType = DRF_VAL(_INFOROM_NVL_OBJECT_V3, _NVLIPT_ERROR, _SUBTYPE, pErrorLog->errorSubtype);
    nvliptInstance = DRF_VAL(_INFOROM_NVL_OBJECT_V3, _ERROR_METADATA, _NVLIPT_INSTANCE_ID, pErrorLog->metadata);
    pNvlError->timeStamp = pErrorLog->data.event.lastError;
    
    if (pErrorLog->header == INFOROM_NVL_ERROR_TYPE_COUNT)
    {
        pNvlError->count = (NvU64)pErrorLog->data.event.totalCount;
    }
    else if (pErrorLog->header == INFOROM_NVL_ERROR_TYPE_ACCUM)
    {
        pNvlError->count = pErrorLog->data.accum.totalCount.hi;
        pNvlError->count = (pNvlError->count << 32) | pErrorLog->data.accum.totalCount.lo;
    }
    else
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    if (FLD_TEST_DRF(_INFOROM_NVL_OBJECT_V3, _ERROR_METADATA, _BLOCK_ID, _DL0, pErrorLog->metadata) ||
        FLD_TEST_DRF(_INFOROM_NVL_OBJECT_V3, _ERROR_METADATA, _BLOCK_ID, _DL1, pErrorLog->metadata) ||
        FLD_TEST_DRF(_INFOROM_NVL_OBJECT_V3, _ERROR_METADATA, _BLOCK_ID, _DL2, pErrorLog->metadata) ||
        FLD_TEST_DRF(_INFOROM_NVL_OBJECT_V3, _ERROR_METADATA, _BLOCK_ID, _DL3, pErrorLog->metadata) ||
        FLD_TEST_DRF(_INFOROM_NVL_OBJECT_V3, _ERROR_METADATA, _BLOCK_ID, _DL4, pErrorLog->metadata) ||
        FLD_TEST_DRF(_INFOROM_NVL_OBJECT_V3, _ERROR_METADATA, _BLOCK_ID, _DL5, pErrorLog->metadata))
    {
        localLinkIdx = DRF_VAL(_INFOROM_NVL_OBJECT_V3, _ERROR_METADATA, _BLOCK_ID, pErrorLog->metadata);
        pNvlError->instance = nvliptInstance * numLinksPerNvlipt + localLinkIdx;

        if (FLD_TEST_DRF(_INFOROM_NVL_OBJECT_V3, _ERROR_METADATA, _DIRECTION, _NA, pErrorLog->metadata) &&
            (subType < (sizeof(DL_NA_ERRORS) / sizeof(DL_NA_ERRORS[0]))))
        {
            pNvlError->error = DL_NA_ERRORS[subType];
            return NVL_SUCCESS;
        }
        else if (FLD_TEST_DRF(_INFOROM_NVL_OBJECT_V3, _ERROR_METADATA, _DIRECTION, _RX, pErrorLog->metadata) &&
            (subType < (sizeof(DL_RX_ERRORS) / sizeof(DL_RX_ERRORS[0]))))
        {
            pNvlError->error = DL_RX_ERRORS[subType];
            return NVL_SUCCESS;
        }
        else if (FLD_TEST_DRF(_INFOROM_NVL_OBJECT_V3, _ERROR_METADATA, _DIRECTION, _TX, pErrorLog->metadata) &&
                (subType < (sizeof(DL_TX_ERRORS) / sizeof(DL_TX_ERRORS[0]))))
        {
            pNvlError->error = DL_TX_ERRORS[subType];
            return NVL_SUCCESS;
        }
    }

    else if (FLD_TEST_DRF(_INFOROM_NVL_OBJECT_V3, _ERROR_METADATA, _BLOCK_ID, _TLC0, pErrorLog->metadata) ||
             FLD_TEST_DRF(_INFOROM_NVL_OBJECT_V3, _ERROR_METADATA, _BLOCK_ID, _TLC1, pErrorLog->metadata) ||
             FLD_TEST_DRF(_INFOROM_NVL_OBJECT_V3, _ERROR_METADATA, _BLOCK_ID, _TLC2, pErrorLog->metadata) ||
             FLD_TEST_DRF(_INFOROM_NVL_OBJECT_V3, _ERROR_METADATA, _BLOCK_ID, _TLC3, pErrorLog->metadata) ||
             FLD_TEST_DRF(_INFOROM_NVL_OBJECT_V3, _ERROR_METADATA, _BLOCK_ID, _TLC4, pErrorLog->metadata) ||
             FLD_TEST_DRF(_INFOROM_NVL_OBJECT_V3, _ERROR_METADATA, _BLOCK_ID, _TLC5, pErrorLog->metadata))
    {
        localLinkIdx = DRF_VAL(_INFOROM_NVL_OBJECT_V3, _ERROR_METADATA, _BLOCK_ID, pErrorLog->metadata)
                                  - NV_INFOROM_NVL_OBJECT_V3_ERROR_METADATA_BLOCK_ID_TLC0;
        pNvlError->instance = nvliptInstance * numLinksPerNvlipt + localLinkIdx;
        
        if (FLD_TEST_DRF(_INFOROM_NVL_OBJECT_V3, _ERROR_METADATA, _DIRECTION, _RX, pErrorLog->metadata) &&
            (subType < (sizeof(TLC_RX_ERRORS) / sizeof(TLC_RX_ERRORS[0]))))
        {
            pNvlError->error = TLC_RX_ERRORS[subType];
            return NVL_SUCCESS;
        }
        else if (FLD_TEST_DRF(_INFOROM_NVL_OBJECT_V3, _ERROR_METADATA, _DIRECTION, _TX, pErrorLog->metadata) &&
            (subType < (sizeof(TLC_TX_ERRORS) / sizeof(TLC_TX_ERRORS[0]))))
        {
            pNvlError->error = TLC_TX_ERRORS[subType];
            return NVL_SUCCESS;
        }
    }
    else if (FLD_TEST_DRF(_INFOROM_NVL_OBJECT_V3, _ERROR_METADATA, _BLOCK_ID, _NVLIPT, pErrorLog->metadata))
    {
        if (subType < (sizeof(LIPT_ERRORS) / sizeof(LIPT_ERRORS[0])))
        {
            if (FLD_TEST_DRF(_INFOROM_NVL_OBJECT_V3, _NVLIPT_ERROR, _LINK_ID, _COMMON, pErrorLog->errorSubtype))
            {
                localLinkIdx = 0; //common nvlipt error
            }
            else
            {
                localLinkIdx = DRF_VAL(_INFOROM_NVL_OBJECT_V3, _NVLIPT_ERROR, _LINK_ID, pErrorLog->errorSubtype);
            }

            pNvlError->instance = nvliptInstance * numLinksPerNvlipt + localLinkIdx;
            pNvlError->error = LIPT_ERRORS[subType];
            return NVL_SUCCESS;
        }
    }

    return -NVL_ERR_NOT_SUPPORTED;
}

void
inforom_nvl_v3_update_correctable_error_rates
(
    INFOROM_NVL_CORRECTABLE_ERROR_RATE_STATE_V3S *pState,
    NvU8 link,
    INFOROM_NVLINK_CORRECTABLE_ERROR_COUNTS *pCounts
)
{
    NvU32 i;
    NvU32 tempFlitCrc, tempRxLinkReplay, tempTxLinkReplay, tempLinkRecovery;
    NvU32 tempLaneCrc[4];

    //
    // If the registers have decreased from last reported, then
    // they must have been reset or have overflowed. Set the last
    // register value to 0.
    //
    if (pCounts->flitCrc < pState->lastRead[link].flitCrc)
    {
        pState->lastRead[link].flitCrc = 0;
    }

    for (i = 0; i < NV_ARRAY_ELEMENTS(pState->lastRead[link].laneCrc); i++)
    {
        if (pCounts->laneCrc[i] < pState->lastRead[link].laneCrc[i])
        {
            pState->lastRead[link].laneCrc[i] = 0;
        }
    }

    // Get number of new errors since the last register read
    tempFlitCrc       = pCounts->flitCrc;
    pCounts->flitCrc -= pState->lastRead[link].flitCrc;

    // Update errors per minute with error delta
    m_inforom_nvl_get_new_errors_per_minute(pCounts->flitCrc,
            &pState->errorsPerMinute[link].flitCrc);

    // Save the current register value for the next callback
    pState->lastRead[link].flitCrc = tempFlitCrc;

    for (i = 0; i < NV_ARRAY_ELEMENTS(pState->lastRead[link].laneCrc); i++)
    {
        tempLaneCrc[i] = pCounts->laneCrc[i];
        pCounts->laneCrc[i] -= pState->lastRead[link].laneCrc[i];
        m_inforom_nvl_get_new_errors_per_minute(pCounts->laneCrc[i],
                &pState->errorsPerMinute[link].laneCrc[i]);

        pState->lastRead[link].laneCrc[i] = tempLaneCrc[i];
    }

    //
    // We don't track rates for the following errors. We just need to stash
    // the current register value and update pCounts with the delta since
    // the last register read.
    //
    if (pCounts->rxLinkReplay < pState->lastRead[link].rxLinkReplay)
    {
        pState->lastRead[link].rxLinkReplay = 0;
    }
    tempRxLinkReplay = pCounts->rxLinkReplay;
    pCounts->rxLinkReplay -= pState->lastRead[link].rxLinkReplay;
    pState->lastRead[link].rxLinkReplay = tempRxLinkReplay;

    if (pCounts->txLinkReplay < pState->lastRead[link].txLinkReplay)
    {
        pState->lastRead[link].txLinkReplay = 0;
    }
    tempTxLinkReplay = pCounts->txLinkReplay;
    pCounts->txLinkReplay -= pState->lastRead[link].txLinkReplay;
    pState->lastRead[link].txLinkReplay = tempTxLinkReplay;

    if (pCounts->linkRecovery < pState->lastRead[link].linkRecovery)
    {
        pState->lastRead[link].linkRecovery = 0;
    }
    tempLinkRecovery = pCounts->linkRecovery;
    pCounts->linkRecovery -= pState->lastRead[link].linkRecovery;
    pState->lastRead[link].linkRecovery = tempLinkRecovery;
}

