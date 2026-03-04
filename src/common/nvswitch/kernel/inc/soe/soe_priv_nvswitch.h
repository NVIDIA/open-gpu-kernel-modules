/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _SOE_PRIV_NVSWITCH_H_
#define _SOE_PRIV_NVSWITCH_H_

#include "soe/haldefs_soe_nvswitch.h"
#include "soe/soeifcmn.h"

#include "flcn/flcnqueue_nvswitch.h"
#include "flcn/flcnable_nvswitch.h"

#define SOE_DMEM_ALIGNMENT (4)

struct SOE
{
    // needs to be the first thing in this struct so that a PSOE can be
    // re-interpreted as a PFLCNABLE and vise-versa. While it is possible
    // to remove this restriction by using (&pSoe->parent) instead of a cast,
    // 1) the reverse (getting a PSOE from a PFLCNABLE) would be difficult and
    // spooky 2) that would force anybody wanting to do the conversion
    // to know the layout of an SOE object (not a big deal, but still annoying)
    union {
        // pointer to our function table - should always be the first thing in any object (including parent)
        soe_hal *pHal;
        FLCNABLE parent;
    } base;

    // Other member variables specific to SOE go here

    /*!
     * Structure tracking all information for active and inactive SEC2 sequences.
     */
    FLCN_QMGR_SEQ_INFO      seqInfo[RM_SOE_MAX_NUM_SEQUENCES];

    /*! The event descriptor for the Thermal event handler */
    NvU32                   thermEvtDesc;
    /*! The event descriptor for the CCI event handler */
    NvU32                   cciEvtDesc;

    /*! The event descriptor for the Heartbeat event handler */
    NvU32                   heartbeatEvtDesc;
};

#endif //_SOE_PRIV_NVSWITCH_H_
