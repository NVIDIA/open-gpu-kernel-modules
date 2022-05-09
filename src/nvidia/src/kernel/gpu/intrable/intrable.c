/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/gpu.h"
#include "kernel/gpu/intrable/intrable.h"
#include "kernel/gpu/intr/intr.h"

/*!
 * @brief Intrable object constructor
 *
 * @param[in] pIntrable     OBJINTRABLE pointer
 *
 * @return NV_OK
 */
NV_STATUS
intrableConstruct_IMPL
(
    OBJINTRABLE *pIntrable
)
{
    pIntrable->partitionAssignedNotificationVector = NV_INTR_VECTOR_INVALID;
    pIntrable->originalNotificationIntrVector = NV_INTR_VECTOR_INVALID;

    return NV_OK;
}

/*!
 * @brief Placeholder function to return error
 *
 * Inheriting classes should override and return the notification interrupt vector
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pIntrable     OBJINTRABLE pointer
 * @param[in] maxIntrs      max interrupt vectors to return
 * @param[out] pIntrVectors table of interrupt vectors
 * @param[out] pMcEngineIdxs table of MC_ENGINE_IDXs. Only required for non-engines
 * @param[out] pCount       how many interrupt vectors were found
 *
 * @return NV_ERR_NOT_SUPPORTED
 */
NV_STATUS
intrableGetPhysicalIntrVectors_IMPL
(
    OBJGPU       *pGpu,
    OBJINTRABLE  *pIntrable,
    NvU32         maxIntrs,
    NvU32        *pIntrVectors,
    NvU32        *pMcEngineIdxs,
    NvU32        *pCount
)
{
    return NV_ERR_NOT_SUPPORTED;
}

/*!
 * @brief Placeholder function to return error
 *
 * Inheriting classes should override and return the notification interrupt vector
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pIntrable     OBJINTRABLE pointer
 * @param[in] maxIntrs      max interrupt vectors to return
 * @param[out] pIntrVectors table of interrupt vectors
 * @param[out] pMcEngineIdxs table of MC_ENGINE_IDXs. Only required for non-engines
 * @param[out] pCount       how many interrupt vectors were found
 *
 * @return NV_ERR_NOT_SUPPORTED
 */
NV_STATUS
intrableGetKernelIntrVectors_IMPL
(
    OBJGPU       *pGpu,
    OBJINTRABLE  *pIntrable,
    NvU32         maxIntrs,
    NvU32        *pIntrVectors,
    NvU32        *pMcEngineIdxs,
    NvU32        *pCount
)
{
    return NV_ERR_NOT_SUPPORTED;
}

/*!
 * @brief Placeholder function to return error
 *
 * Inheriting classes should override and return the notification interrupt vector
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pIntrable     OBJINTRABLE pointer
 * @param[in] pIntrVector   Pointer to store the interrupt vector
 *
 * @return NV_ERR_NOT_SUPPORTED
 */
NV_STATUS
intrableGetNotificationIntrVector_IMPL
(
    OBJGPU      *pGpu,
    OBJINTRABLE *pIntrable,
    NvU32       *pIntrVector
)
{
    // Should never be called
    DBG_BREAKPOINT();
    return NV_ERR_NOT_SUPPORTED;
}

/*!
 * @brief Placeholder function to return error
 *
 * Inheriting classes should override and set the notification interrupt vector # here
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pIntrable     OBJINTRABLE pointer
 * @param[in] intrVector   the interrupt vector
 *
 * @return NV_ERR_NOT_SUPPORTED
 */
NV_STATUS
intrableSetNotificationIntrVector_IMPL
(
    OBJGPU      *pGpu,
    OBJINTRABLE *pIntrable,
    NvU32        intrVector
)
{
    return NV_ERR_NOT_SUPPORTED;
}

/*!
 * @brief Cache (and then write to HW) the partition assigned notification
 * intr vector
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pIntrable     OBJINTRABLE pointer
 * @param[in] intrVector    the interrupt vector
 *
 * @return NV_STATUS
 */
NV_STATUS
intrableCacheAndSetPartitionNotificationIntrVector_IMPL
(
    OBJGPU      *pGpu,
    OBJINTRABLE *pIntrable,
    NvU32        intrVector
)
{
    if (pIntrable->originalNotificationIntrVector == NV_INTR_VECTOR_INVALID)
    {
        // Remember the initial HW value before we overwrite it
        NV_ASSERT_OK_OR_RETURN(intrableGetNotificationIntrVector(pGpu, pIntrable,
            &pIntrable->originalNotificationIntrVector));
    }

    pIntrable->partitionAssignedNotificationVector = intrVector;
    return intrableSetNotificationIntrVector(pGpu, pIntrable, intrVector);
}

/*!
 * @brief Write the cached partition assigned notification interrupt vector
 * to HW again
 *
 * If the partition value is default, do nothing.
 * Else write it to HW again.
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pIntrable     OBJINTRABLE pointer
 *
 * @return NV_STATUS
 */
NV_STATUS
intrableSetPartitionNotificationIntrVector_IMPL
(
    OBJGPU      *pGpu,
    OBJINTRABLE *pIntrable
)
{
    if (pIntrable->partitionAssignedNotificationVector == NV_INTR_VECTOR_INVALID)
    {
        // No-op if this has not been configured
        return NV_OK;
    }

    return intrableSetNotificationIntrVector(
        pGpu, pIntrable, pIntrable->partitionAssignedNotificationVector);
}

/*!
 * @brief Returns the partition assigned notification interrupt vector
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pIntrable     OBJINTRABLE pointer
 *
 * @return partitionAssignedNotificationVector
 */
NvU32
intrableGetPartitionNotificationIntrVector_IMPL
(
    OBJGPU      *pGpu,
    OBJINTRABLE *pIntrable
)
{
    return pIntrable->partitionAssignedNotificationVector;
}

/*!
 * @brief Return the notification interrupt vector to HW's original value, if
 * is is not at its default value
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pIntrable     OBJINTRABLE pointer
 *
 * @return NV_STATUS
 */
NV_STATUS
intrableRevertNotificationIntrVector_IMPL
(
    OBJGPU      *pGpu,
    OBJINTRABLE *pIntrable
)
{
    if (pIntrable->originalNotificationIntrVector == NV_INTR_VECTOR_INVALID)
    {
        // Don't expect to revert when there is nothing to revert to
        return NV_ERR_INVALID_STATE;
    }

    //
    // Forget the partition assigned vector so that it does not get written
    // again
    //
    pIntrable->partitionAssignedNotificationVector = NV_INTR_VECTOR_INVALID;

    return intrableSetNotificationIntrVector(
        pGpu, pIntrable, pIntrable->originalNotificationIntrVector);
}
