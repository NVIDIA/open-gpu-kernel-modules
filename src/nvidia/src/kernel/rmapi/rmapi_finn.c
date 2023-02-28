/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "resserv/resserv.h"

#include "ctrl/ctrlxxxx.h"

#include "g_finn_rm_api.h"

/**
 * Serialize parameters for servicing command
 *
 * If unserialized params are passed in, try to serialize into CALL_CONTEXT.
 * If serialized params are passed in, set CALL_CONTEXT variables.
 */
NV_STATUS
serverSerializeCtrlDown
(
    CALL_CONTEXT *pCallContext,
    NvU32 cmd,
    void *pParams,
    NvU32 paramsSize,
    NvU32 *flags
)
{
    if (!(*flags & NVOS54_FLAGS_FINN_SERIALIZED))
    {
        NV_STATUS status;
        NvU8 *pSerBuffer;
        const NvU32 interface_id = (DRF_VAL(XXXX, _CTRL_CMD, _CLASS, cmd) << 8) |
                                    DRF_VAL(XXXX, _CTRL_CMD, _CATEGORY, cmd);
        const NvU32 message_id = DRF_VAL(XXXX, _CTRL_CMD, _INDEX, cmd);
        NvU32 serializedSize = (NvU32)FinnRmApiGetSerializedSize(interface_id, message_id, pParams);

        pCallContext->pDeserializedParams = pParams;
        pCallContext->deserializedSize = paramsSize;

        // Nothing to do if FINN doesn't support serializing this control
        if (serializedSize == 0)
            return NV_OK;

        // Assume we've already serialized if it exists
        if (pCallContext->pSerializedParams != NULL)
        {
            NV_ASSERT_OR_RETURN(pCallContext->serializedSize == serializedSize, NV_ERR_INVALID_STATE);
            return NV_OK;
        }
        else
        {
            pSerBuffer = portMemAllocNonPaged(serializedSize);
            if (pSerBuffer == NULL)
                return NV_ERR_INSUFFICIENT_RESOURCES;

            pCallContext->pSerializedParams = (FINN_RM_API *)pSerBuffer;
        }

        status = FinnRmApiSerializeDown(interface_id, message_id, pParams, &pSerBuffer, serializedSize);

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Serialization failed for cmd 0x%x with status %s (0x%x) at index 0x%llx\n",
                      cmd, nvAssertStatusToString(status), status,
                      (NvUPtr)(pSerBuffer - (NvU8 *)pCallContext->pSerializedParams));
            portMemFree(pCallContext->pSerializedParams);
            pCallContext->pSerializedParams = NULL;
            return status;
        }

        pCallContext->serializedSize = serializedSize;
        *flags |= NVOS54_FLAGS_FINN_SERIALIZED;
    }
    else
    {
        // Params are already serialized, just copy them in
        pCallContext->pSerializedParams = pParams;
        pCallContext->serializedSize = paramsSize;
    }

    return NV_OK;
}

/**
 * Deserialize parameters for servicing command
 *
 * If serialized params are passed in, deserialize them into CALL_CONTEXT.
 * If deserialized params are passed in, set CALL_CONTEXT variables.
 */
NV_STATUS
serverDeserializeCtrlDown
(
    CALL_CONTEXT *pCallContext,
    NvU32 cmd,
    void *pParams,
    NvU32 paramsSize,
    NvU32 *flags
)
{
    if (*flags & NVOS54_FLAGS_FINN_SERIALIZED)
    {
        NV_STATUS status;
        NvU8 *pSerBuffer;
        void *pDeserParams;
        const NvU32 interface_id = (DRF_VAL(XXXX, _CTRL_CMD, _CLASS, cmd) << 8) |
                                    DRF_VAL(XXXX, _CTRL_CMD, _CATEGORY, cmd);
        const NvU32 message_id = DRF_VAL(XXXX, _CTRL_CMD, _INDEX, cmd);
        NvU32 unserializedSize = (NvU32)FinnRmApiGetUnserializedSize(interface_id, message_id);

        // Report error if FINN can't deserialize but RM is reporting the control as serialized
        if (unserializedSize == 0)
            return NV_ERR_LIB_RM_VERSION_MISMATCH;

        // Assume we've already deserialized if it exists
        if (pCallContext->pDeserializedParams != NULL)
        {
            NV_ASSERT_OR_RETURN(pCallContext->deserializedSize == unserializedSize, NV_ERR_INVALID_STATE);
            return NV_OK;
        }
        else
        {
            pDeserParams = portMemAllocNonPaged(unserializedSize);
            if (pDeserParams == NULL)
                return NV_ERR_INSUFFICIENT_RESOURCES;

            pCallContext->pDeserializedParams = pDeserParams;
        }

        pSerBuffer = (NvU8 *)pParams;

        status = FinnRmApiDeserializeDown(&pSerBuffer, paramsSize, pDeserParams, unserializedSize);

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Deserialization failed for cmd 0x%x with status %s (0x%x) at index 0x%llx\n",
                      cmd, nvAssertStatusToString(status), status,
                      (NvUPtr)(pSerBuffer - (NvU8 *)pParams));
            portMemFree(pCallContext->pDeserializedParams);
            pCallContext->pDeserializedParams = NULL;
            return status;
        }

        pCallContext->deserializedSize = unserializedSize;
        pCallContext->pSerializedParams = pParams;
        pCallContext->serializedSize = paramsSize;
    }
    else
    {
        // Not serialized, copy into deser params
        pCallContext->pDeserializedParams = pParams;
        pCallContext->deserializedSize = paramsSize;
    }

    // Automatically request reserialization in case the control goes to GSP
    pCallContext->bReserialize = NV_TRUE;

    return NV_OK;
}

/**
 * Serialize parameters for returning from command
 *
 * If serialized params are passed in, serialize into them from CALL_CONTEXT deserialized params.
 * If deserialized params are passed in, unset the serialized flag. We expect that this means that
 * the parameters were serialized locally and not by the caller.
 */
NV_STATUS
serverSerializeCtrlUp
(
    CALL_CONTEXT *pCallContext,
    NvU32 cmd,
    void *pParams,
    NvU32 paramsSize,
    NvU32 *flags
)
{
    //
    // We add NVOS54_FLAGS_FINN_SERIALIZED if the control can be serialized but wasn't in serverSerializeCtrlDown
    // We don't want to return a serialized buffer if one wasn't given to us in the first place
    // Check if pSerializedParams matches pParams to make sure the caller expects serialized info returned
    //
    if ((*flags & NVOS54_FLAGS_FINN_SERIALIZED) &&
        (pCallContext->pSerializedParams == pParams))
    {
        NV_STATUS status;
        NvU8 *pSerBuffer;
        void *pDeserBuffer;
        const NvU32 interface_id = (DRF_VAL(XXXX, _CTRL_CMD, _CLASS, cmd) << 8) |
                                    DRF_VAL(XXXX, _CTRL_CMD, _CATEGORY, cmd);
        const NvU32 message_id = DRF_VAL(XXXX, _CTRL_CMD, _INDEX, cmd);

        // Should be serialized at this point. Expect that serializedSize is set otherwise something is wrong
        if (pCallContext->serializedSize == 0)
            return NV_ERR_INVALID_STATE;

        pSerBuffer = (NvU8 *) pCallContext->pSerializedParams;
        pDeserBuffer = (NvU8 *) pCallContext->pDeserializedParams;

        if (pSerBuffer == NULL)
            return NV_ERR_INVALID_STATE;

        // It's possible that we have nothing to do if we're just passing parameters along
        if (pDeserBuffer == NULL)
            return NV_OK;

        status = FinnRmApiSerializeUp(interface_id, message_id, pDeserBuffer, &pSerBuffer, pCallContext->serializedSize);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Serialization failed for cmd 0x%x with status %s (0x%x) at index 0x%llx\n",
                      cmd, nvAssertStatusToString(status), status,
                      (NvUPtr)(pSerBuffer - (NvU8 *)pCallContext->pSerializedParams));
            return status;
        }
    }
    else if (*flags & NVOS54_FLAGS_FINN_SERIALIZED)
    {
        // We serialized the control. Unset the flag
        *flags &= ~NVOS54_FLAGS_FINN_SERIALIZED;
    }

    return NV_OK;
}

/**
 * Deserialize parameters for returning from command
 *
 * If serialized params are passed in with the serialized flag set, do nothing. We expect this means we are just
 *  passing along the parameters to another layer.
 * If deserialized params are passed in with the serialized flag set, unset flag and deserialize into params.
 *  We expect that this means that the parameters were serialized locally and not by the caller.
 * If deserialized params are passed in without the flag set just copy to CALL_CONTEXT
 */
NV_STATUS
serverDeserializeCtrlUp
(
    CALL_CONTEXT *pCallContext,
    NvU32 cmd,
    void *pParams,
    NvU32 paramsSize,
    NvU32 *flags
)
{
    if (*flags & NVOS54_FLAGS_FINN_SERIALIZED)
    {
        NV_STATUS status;
        NvU8 *pSerBuffer;
        const NvU32 interface_id = (DRF_VAL(XXXX, _CTRL_CMD, _CLASS, cmd) << 8) |
                                    DRF_VAL(XXXX, _CTRL_CMD, _CATEGORY, cmd);
        const NvU32 message_id = DRF_VAL(XXXX, _CTRL_CMD, _INDEX, cmd);
        NvU32 unserializedSize = (NvU32)FinnRmApiGetUnserializedSize(interface_id, message_id);

        if (pParams == pCallContext->pSerializedParams)
        {
            // Received serialized params from caller, no reason to deserialize up
            return NV_OK;
        }
        else
        {
            // Serialized, but not passing back up serialized params, clear flag
            *flags &= ~NVOS54_FLAGS_FINN_SERIALIZED;
        }

        // Report error if FINN can't deserialize but RM is reporting the control as serialized
        if (unserializedSize == 0)
            return NV_ERR_LIB_RM_VERSION_MISMATCH;

        pCallContext->pDeserializedParams = pParams;

        // DeserializeUp expects pointers to be set up already
        pSerBuffer = (NvU8 *) pCallContext->pSerializedParams;
        if (pSerBuffer == NULL)
            return NV_ERR_INVALID_STATE;

        status = FinnRmApiDeserializeUp(&pSerBuffer, pCallContext->serializedSize, pParams, unserializedSize);

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                          "Deserialization failed for cmd 0x%x with status %s (0x%x) at index 0x%llx\n",
                          cmd, nvAssertStatusToString(status), status,
                          (NvUPtr)(pSerBuffer - (NvU8 *)pCallContext->pSerializedParams));
            return status;
        }

        pCallContext->deserializedSize = unserializedSize;
    }
    else
    {
        // already in deserialized state
        pCallContext->pDeserializedParams = pParams;
        pCallContext->deserializedSize = paramsSize;
    }

    return NV_OK;
}

void
serverFreeSerializeStructures
(
    CALL_CONTEXT *pCallContext,
    void *pParams
)
{
    if (pCallContext->pSerializedParams != pParams)
        portMemFree(pCallContext->pSerializedParams);
    if (pCallContext->pDeserializedParams != pParams)
        portMemFree(pCallContext->pDeserializedParams);

    pCallContext->pSerializedParams = NULL;
    pCallContext->pDeserializedParams = NULL;
    pCallContext->serializedSize = 0;
    pCallContext->deserializedSize = 0;
}
