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
#include "rmapi/rmapi.h"

#include "ctrl/ctrlxxxx.h"

#include "g_finn_rm_api.h"

/**
 * Serialize parameters for servicing command
 *
 * If unserialized params are passed in, try to serialize into CALL_CONTEXT.
 * If serialized params are passed in, set CALL_CONTEXT variables.
 *
 * Caller's parameters pointer and size will be overridden and restored on *CtrlUp call
 */
NV_STATUS
serverSerializeCtrlDown
(
    CALL_CONTEXT *pCallContext,
    NvU32 cmd,
    void **ppParams,
    NvU32 *pParamsSize,
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
        NvU32 serializedSize = (NvU32)FinnRmApiGetSerializedSize(interface_id, message_id, *ppParams);

        pCallContext->pDeserializedParams = *ppParams;
        pCallContext->deserializedSize = *pParamsSize;

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

        status = FinnRmApiSerializeDown(interface_id, message_id, *ppParams, pSerBuffer, serializedSize);

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Serialization failed for cmd 0x%06x with status %s (0x%02x)\n",
                      cmd, nvstatusToString(status), status);
            portMemFree(pCallContext->pSerializedParams);
            pCallContext->pSerializedParams = NULL;
            return status;
        }

        pCallContext->serializedSize = serializedSize;
        *flags |= NVOS54_FLAGS_FINN_SERIALIZED;

        // Override passed in parameters
        *ppParams = pCallContext->pSerializedParams;
        *pParamsSize = pCallContext->serializedSize;
        pCallContext->bLocalSerialization = NV_TRUE;
    }
    else
    {
        // Params are already serialized, just copy them in
        pCallContext->pSerializedParams = *ppParams;
        pCallContext->serializedSize = *pParamsSize;
    }

    return NV_OK;
}

/**
 * Deserialize parameters for servicing command
 *
 * If serialized params are passed in, deserialize them into CALL_CONTEXT.
 * If deserialized params are passed in, set CALL_CONTEXT variables.
 *
 * Caller's parameters pointer and size will be overridden and restored on *CtrlUp call
 */
NV_STATUS
serverDeserializeCtrlDown
(
    CALL_CONTEXT *pCallContext,
    NvU32 cmd,
    void **ppParams,
    NvU32 *pParamsSize,
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

        pSerBuffer = (NvU8 *)*ppParams;

        status = FinnRmApiDeserializeDown(pSerBuffer, *pParamsSize, pDeserParams, unserializedSize);

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Deserialization failed for cmd 0x%06x with status %s (0x%02x)\n",
                      cmd, nvstatusToString(status), status);
            portMemFree(pCallContext->pDeserializedParams);
            pCallContext->pDeserializedParams = NULL;
            return status;
        }

        pCallContext->deserializedSize = unserializedSize;
        pCallContext->pSerializedParams = *ppParams;
        pCallContext->serializedSize = *pParamsSize;

        // Override passed in parameters
        *ppParams = pCallContext->pDeserializedParams;
        *pParamsSize = pCallContext->deserializedSize;
    }
    else
    {
        // Not serialized, copy into deser params
        pCallContext->pDeserializedParams = *ppParams;
        pCallContext->deserializedSize = *pParamsSize;
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
 *
 * Caller's parameters pointer and size will be restored from *CtrlDown call
 */
NV_STATUS
serverSerializeCtrlUp
(
    CALL_CONTEXT *pCallContext,
    NvU32 cmd,
    void **ppParams,
    NvU32 *pParamsSize,
    NvU32 *flags
)
{
    //
    // We add NVOS54_FLAGS_FINN_SERIALIZED if the control can be serialized but wasn't in serverSerializeCtrlDown
    // We don't want to return a serialized buffer if one wasn't given to us in the first place
    //
    if ((*flags & NVOS54_FLAGS_FINN_SERIALIZED) && !pCallContext->bLocalSerialization)
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

        status = FinnRmApiSerializeUp(interface_id, message_id, pDeserBuffer, pSerBuffer, pCallContext->serializedSize);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Serialization failed for cmd 0x%06x with status %s (0x%02x)\n",
                      cmd, nvstatusToString(status), status);
            return status;
        }

        *ppParams = pCallContext->pSerializedParams;
        *pParamsSize = pCallContext->serializedSize;
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
 *
 * Caller's parameters pointer and size will be restored from *CtrlDown call
 */
NV_STATUS
serverDeserializeCtrlUp
(
    CALL_CONTEXT *pCallContext,
    NvU32 cmd,
    void **ppParams,
    NvU32 *pParamsSize,
    NvU32 *flags
)
{
    if (*flags & NVOS54_FLAGS_FINN_SERIALIZED)
    {
        NV_STATUS status;
        NvU8 *pSerBuffer;
        void *pDeserBuffer;
        const NvU32 interface_id = (DRF_VAL(XXXX, _CTRL_CMD, _CLASS, cmd) << 8) |
                                    DRF_VAL(XXXX, _CTRL_CMD, _CATEGORY, cmd);
        const NvU32 message_id = DRF_VAL(XXXX, _CTRL_CMD, _INDEX, cmd);
        NvU32 unserializedSize = (NvU32)FinnRmApiGetUnserializedSize(interface_id, message_id);

        if (!pCallContext->bLocalSerialization)
        {
            // We didn't serialize down, so don't deserialize up
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

        // DeserializeUp expects pointers to be set up already
        pDeserBuffer = pCallContext->pDeserializedParams;
        if (pDeserBuffer == NULL)
            return NV_ERR_INVALID_STATE;

        pSerBuffer = (NvU8 *) pCallContext->pSerializedParams;
        if (pSerBuffer == NULL)
            return NV_ERR_INVALID_STATE;

        status = FinnRmApiDeserializeUp(pSerBuffer, pCallContext->serializedSize, pDeserBuffer, unserializedSize);

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                          "Deserialization failed for cmd 0x%06x with status %s (0x%02x)\n",
                          cmd, nvstatusToString(status), status);
            return status;
        }

        pCallContext->deserializedSize = unserializedSize;

        *ppParams = pCallContext->pDeserializedParams;
        *pParamsSize = pCallContext->deserializedSize;
    }

    return NV_OK;
}

NV_STATUS
serverSerializeAllocDown
(
    CALL_CONTEXT *pCallContext,
    NvU32 classId,
    void **ppParams,
    NvU32 *pParamsSize,
    NvU32 *flags
)
{
    const NvU32 interface_id = NV_RM_ALLOC_INTERFACE_INTERFACE_ID;
    const NvU32 message_id = classId;
    if (!(*flags & RMAPI_ALLOC_FLAGS_SERIALIZED))
    {
        NV_STATUS status;
        NvU8 *pSerBuffer;
        NvU32 serializedSize = (NvU32)FinnRmApiGetSerializedSize(interface_id, message_id, *ppParams);

        pCallContext->pDeserializedParams = *ppParams;
        pCallContext->deserializedSize = *pParamsSize;

        // Nothing to do if FINN doesn't support serializing this allocation
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

        status = FinnRmApiSerializeDown(interface_id, message_id, *ppParams, pSerBuffer, serializedSize);

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Serialization failed for classId 0x%06x with status %s (0x%02x)\n",
                      classId, nvstatusToString(status), status);
            portMemFree(pCallContext->pSerializedParams);
            pCallContext->pSerializedParams = NULL;
            return status;
        }

        pCallContext->serializedSize = serializedSize;
        *flags |= RMAPI_ALLOC_FLAGS_SERIALIZED;
        *ppParams = pCallContext->pSerializedParams;
        *pParamsSize = serializedSize;
        pCallContext->bLocalSerialization = NV_TRUE;
    }
    else
    {
        // Params are already serialized, just copy them in
        pCallContext->pSerializedParams = *ppParams;
        pCallContext->serializedSize = *pParamsSize;
    }

    return NV_OK;
}


NV_STATUS
serverDeserializeAllocDown
(
    CALL_CONTEXT *pCallContext,
    NvU32 classId,
    void **ppParams,
    NvU32 *pParamsSize,
    NvU32 *flags
)
{
    const NvU32 interface_id = NV_RM_ALLOC_INTERFACE_INTERFACE_ID;
    const NvU32 message_id = classId;
    if (*flags & RMAPI_ALLOC_FLAGS_SERIALIZED)
    {
        NV_STATUS status;
        NvU8 *pSerBuffer;
        void *pDeserParams;
        NvU32 unserializedSize = (NvU32)FinnRmApiGetUnserializedSize(interface_id, message_id);

        // Report error if FINN can't deserialize but RM is reporting the alloc as serialized
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

        pSerBuffer = (NvU8 *)*ppParams;

        status = FinnRmApiDeserializeDown(pSerBuffer, *pParamsSize, pDeserParams, unserializedSize);

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Deserialization failed for classId 0x%06x with status %s (0x%02x)\n",
                      classId, nvstatusToString(status), status);
            portMemFree(pCallContext->pDeserializedParams);
            pCallContext->pDeserializedParams = NULL;
            return status;
        }

        pCallContext->serializedSize = *pParamsSize;
        pCallContext->deserializedSize = unserializedSize;
        pCallContext->pSerializedParams = *ppParams;

        *ppParams = pCallContext->pDeserializedParams;
        *pParamsSize = pCallContext->deserializedSize;
    }
    else
    {
        // Not serialized, copy into deser params
        pCallContext->pDeserializedParams = *ppParams;
        pCallContext->deserializedSize = *pParamsSize;
    }

    // Automatically request reserialization in case the alloc goes to GSP
    pCallContext->bReserialize = NV_TRUE;

    return NV_OK;
}

NV_STATUS
serverSerializeAllocUp
(
    CALL_CONTEXT *pCallContext,
    NvU32 classId,
    void **ppParams,
    NvU32 *pParamsSize,
    NvU32 *flags
)
{
    //
    // We add RMAPI_ALLOC_FLAGS_SERIALIZED if the alloc can be serialized but wasn't in serverSerializeAllocDown
    // We don't want to return a serialized buffer if one wasn't given to us in the first place
    // Check if pSerializedParams matches pParams to make sure the caller expects serialized info returned
    //
    if ((*flags & RMAPI_ALLOC_FLAGS_SERIALIZED) && !pCallContext->bLocalSerialization)
    {
        NV_STATUS status;
        NvU8 *pSerBuffer;
        void *pDeserBuffer;
        const NvU32 interface_id = NV_RM_ALLOC_INTERFACE_INTERFACE_ID;
        const NvU32 message_id = classId;

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

        status = FinnRmApiSerializeUp(interface_id, message_id, pDeserBuffer, pSerBuffer, pCallContext->serializedSize);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Serialization failed for classId 0x%06x with status %s (0x%02x)\n",
                      classId, nvstatusToString(status), status);
            return status;
        }

        *ppParams = pCallContext->pSerializedParams;
        *pParamsSize = pCallContext->serializedSize;
    }
    else if (*flags & RMAPI_ALLOC_FLAGS_SERIALIZED)
    {
        // We serialized the alloc. Unset the flag
        *flags &= ~RMAPI_ALLOC_FLAGS_SERIALIZED;
    }

    return NV_OK;
}

NV_STATUS
serverDeserializeAllocUp
(
    CALL_CONTEXT *pCallContext,
    NvU32 classId,
    void **ppParams,
    NvU32 *pParamsSize,
    NvU32 *flags
)
{
    const NvU32 interface_id = NV_RM_ALLOC_INTERFACE_INTERFACE_ID;
    const NvU32 message_id = classId;
    if (*flags & RMAPI_ALLOC_FLAGS_SERIALIZED)
    {
        NV_STATUS status;
        NvU8 *pSerBuffer;
        void *pDeserBuffer;
        NvU32 unserializedSize = (NvU32)FinnRmApiGetUnserializedSize(interface_id, message_id);

        if (!pCallContext->bLocalSerialization)
        {
            // We didn't serialize down, so don't deserialize up
            return NV_OK;
        }
        else
        {
            // Serialized, but not passing back up serialized params, clear flag
            *flags &= ~RMAPI_ALLOC_FLAGS_SERIALIZED;
        }

        // Report error if FINN can't deserialize but RM is reporting the alloc as serialized
        if (unserializedSize == 0)
            return NV_ERR_LIB_RM_VERSION_MISMATCH;

        // DeserializeUp expects pointers to be set up already
        pDeserBuffer = pCallContext->pDeserializedParams;
        if (pDeserBuffer == NULL)
            return NV_ERR_INVALID_STATE;

        pSerBuffer = (NvU8 *) pCallContext->pSerializedParams;
        if (pSerBuffer == NULL)
            return NV_ERR_INVALID_STATE;

        status = FinnRmApiDeserializeUp(pSerBuffer, pCallContext->serializedSize, pDeserBuffer, unserializedSize);

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                          "Deserialization failed for classId 0x%06x with status %s (0x%02x)\n",
                          classId, nvstatusToString(status), status);
            return status;
        }

        pCallContext->deserializedSize = unserializedSize;
        *ppParams = pCallContext->pDeserializedParams;
        *pParamsSize = unserializedSize;
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
