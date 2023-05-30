/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef CONF_COMPUTE_KEYSTORE_H
#define CONF_COMPUTE_KEYSTORE_H

#include "nvtypes.h"
#include "nvstatus.h"
#include "cc_drv.h"
#include "kernel/gpu/fifo/kernel_channel.h"

// Named values for ccKeyStoreDeposit()'s slotNumber parameter.
#define KEYSTORE_SLOT_GSP             0
#define KEYSTORE_SLOT_SEC2            1
// LCE index can range from 0 to 7 inclusive.
#define KEYSTORE_SLOT_LCE_UMD(index)  (2 + (index) * 3)
#define KEYSTORE_SLOT_LCE_KMD(index)  (3 + (index) * 3)
#define KEYSTORE_SLOT_LCE_MGPU(index) (4 + (index) * 3)

/*!
 * @brief Initialize the keystore.
 *
 * @param[in] pGpu Pointer to GPU object.
 *
 * @return NV_ERR_INVALID_STATE if system is not in Confidential Compute mode.
 */
NV_STATUS
ccKeyStoreInit (OBJGPU *pGpu);

/*!
 * @brief Deposits a KMB into a keystore slot.
 *
 * @param[in] slotNumber        Slot number into which the KMB will be deposited.
 * @param[in] keyMaterialBundle Pair of key / IV / IV mask tuples. IV will be set to 0 by function.
 *
 * @return NV_ERR_INVALID_INDEX if slotNumber is illegal, NV_OK otherwise.
 */
NV_STATUS
ccKeyStoreDeposit
(
    NvU32 slotNumber,
    CC_KMB keyMaterialBundle
);

/*!
 * @brief Retrieves a KMB based on the channel. The keystore uses channel
          information such as engine type and the associated privilege mode
          to determine which KMB to fetch.
 *        The IV's channel counter for the given direction is pre-incremented.
 *
 * @param[in]  pKernelChannel
 * @param[in]  rotateOperation    Either:
                                  ROTATE_IV_ENCRYPT
                                  ROTATE_IV_DECRYPT
                                  ROTATE_IV_ALL_VALID
 * @param[out] keyMaterialBundle  The KMB for the given channel.
 *
 * @return NV_ERR_INSUFFICIENT_RESOURCES if channel counter overflow would occur.
           NV_ERR_GENERIC for other errors.
 *         NV_OK otherwise.
 */
NV_STATUS
ccKeyStoreRetrieveViaChannel
(
    KernelChannel *pKernelChannel,
    ROTATE_IV_TYPE rotateOperation,
    PCC_KMB keyMaterialBundle
);

/*!
 * @brief Clears the contents of the keystore.
 */
void
cckeyStoreClear (void);

#endif // CONF_COMPUTE_KEYSTORE_H
