/*
 * Implementation of the protocol buffers encoder.
 *
 * Based on code taken from
 * https://code.google.com/archive/p/lwpb/source/default/source
 *
 * The code there is licensed as Apache 2.0.  However, NVIDIA has received the
 * code from the original author under MIT license terms.
 *
 *
 * Copyright 2009 Simon Kallweit
 * Copyright 2009-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "os/os.h"
#include "lib/protobuf/prb.h"

static NV_STATUS prbEncAddField(PRB_ENCODER *encoder,
                                const PRB_FIELD_DESC *field_desc,
                                PRB_VALUE *value);
static NV_STATUS prbEndPackedField(PRB_ENCODER *encoder);
static NV_STATUS prbStartPackedField(PRB_ENCODER *encoder,
                                     const PRB_FIELD_DESC *field_desc);

#define MSG_RESERVE_BYTES 10

// Handy macros to check flags
#define COUNT_FLAG(encoder) ((encoder->flags & PRB_COUNT_ONLY) != 0)
#define FIXED_FLAG(encoder) ((encoder->flags & PRB_FIXED_MODE) != 0)

/*!
 * @brief Initializes a memory buffer. Sets the position to the base address.
 * @param [in] buf Memory buffer
 * @param [in] data Base address of memory
 * @param [in] len Length of memory
 *
 */
static void
prbBufInit(PRB_BUF *buf, void *data, NvU32 len)
{
    buf->base = data;
    buf->pos = data;
    buf->end = &buf->base[len];
}

/*!
 * @brief frees a buffer
 * @param [in] buff Memory buffer
 */

static void
prbBuffFree(PRB_BUF *buf)
{
    portMemFree(buf->base);

    buf->base = NULL;
    buf->pos = NULL;
    buf->end = NULL;
}

/*!
 * @brief Returns the number of used bytes in the buffer.
 * @param [in] buf Memory buffer
 *
 * @returns the number of used bytes.
 */
static NvU32
prbBufUsed(PRB_BUF *buf)
{
    return (NvU32)(buf->pos - buf->base);
}

/*!
 * @brief Returns the number of bytes left in the buffer.
 * @param [in] buf Memory buffer
 *
 * @returns the number of bytes left.
 */
static NvU32
prbBufLeft(PRB_BUF *buf)
{
    return (NvU32)(buf->end - buf->pos);
}

// Encoder utilities

/*!
 * @brief Encodes a variable integer in base-128 format.
 * See https://code.google.com/apis/protocolbuffers/docs/encoding.html for more
 * information.
 * @param [in] buf Memory buffer
 * @param [in] varint Value to encode
 * @param [in] countOnly Set to just count the bytes
 * @returns NV_STATUS
 */

static
NV_STATUS
encode_varint(PRB_BUF *buf, NvU64 varint, NvBool countOnly)
{
    do {
        if (prbBufLeft(buf) < 1)
            return NV_ERR_BUFFER_TOO_SMALL;
        if (!countOnly)
        {
            if (varint > 127) {
                *buf->pos = (NvU8)(0x80 | (varint & 0x7F));
            } else {
                *buf->pos = (NvU8)(varint & 0x7F);
            }
        }
        varint >>= 7;
        buf->pos++;
    } while (varint);

    return NV_OK;
}

/**
 * Decodes a variable integer in base-128 format.
 * See https://code.google.com/apis/protocolbuffers/docs/encoding.html for more
 * information.
 * @param [in] pBuff Buffer to decode
 * @param [in] len Length of input buffer
 * @param [out] pUsed Number of bytes used
 * @param [out] pData Data value returned
 * @return Returns NV_STATUS
 */
static NV_STATUS
decode_varint(NvU8 *pBuff, NvU64 len, NvU32 *pUsed, NvU64 *pData)
{
    NvU32 bitpos;

    if (pBuff == NULL)
    {
        return NV_ERR_INVALID_POINTER;
    }

    *pUsed = 0;

    *pData = 0;
    for (bitpos = 0; *pBuff & 0x80 && bitpos < 64; bitpos += 7, pBuff++)
    {
        *pData |= (NvU64) (*pBuff & 0x7f) << bitpos;
        (*pUsed)++;
        if (--len < 1)
            return NV_ERR_BUFFER_TOO_SMALL;
    }
    *pData |= (NvU64)(*pBuff & 0x7f) << bitpos;
    (*pUsed)++;

    return NV_OK;
}

/*!
 * @brief Encodes a 32 bit integer.
 * @param [in] buf Memory buffer
 * @param [in] value Value to encode
 * @param [in] countOnly Set to just count the bytes
 * @returns NV_STATUS
 */
static NV_STATUS
encode_32bit(PRB_BUF *buf, NvU32 value, NvBool countOnly)
{
    if (prbBufLeft(buf) < 4)
        return NV_ERR_BUFFER_TOO_SMALL;

    if (!countOnly)
    {
        buf->pos[0] = (NvU8)((value) & 0xff);
        buf->pos[1] = (NvU8)((value >> 8) & 0xff);
        buf->pos[2] = (NvU8)((value >> 16) & 0xff);
        buf->pos[3] = (NvU8)((value >> 24) & 0xff);
    }
    buf->pos += 4;

    return NV_OK;
}

/*!
 * @brief Encodes a 64 bit integer.
 * @param [in] buf Memory buffer
 * @param [in] value Value to encode
 * @param [in] countOnly Set to just count the bytes
 * @returns NV_STATUS
 */
static NV_STATUS
encode_64bit(PRB_BUF *buf, NvU64 value, NvBool countOnly)
{
    if (prbBufLeft(buf) < 8)
        return NV_ERR_BUFFER_TOO_SMALL;

    if (!countOnly)
    {
        buf->pos[0] = (NvU8)((value) & 0xff);
        buf->pos[1] = (NvU8)((value >> 8) & 0xff);
        buf->pos[2] = (NvU8)((value >> 16) & 0xff);
        buf->pos[3] = (NvU8)((value >> 24) & 0xff);
        value >>= 32;
        buf->pos[4] = (NvU8)((value) & 0xff);
        buf->pos[5] = (NvU8)((value >> 8) & 0xff);
        buf->pos[6] = (NvU8)((value >> 16) & 0xff);
        buf->pos[7] = (NvU8)((value >> 24) & 0xff);
    }
    buf->pos += 8;

    return NV_OK;
}

// Encoder

/*!
 * @brief Starts the encoder with the first message
 *   This variant allows the caller to pass in a buffer to use.
 * @param [in] encoder The encoder structure
 * @param [in] msg_desc The message to encode
 * @param [in] data The buffer to use
 * @param [in] len Length of the buffer
 */

void
prbEncStart
(
    PRB_ENCODER *encoder,
    const PRB_MSG_DESC *msg_desc,
    void *data,
    NvU32 len,
    PrbBufferCallback *pBufferCallback
)
{
    encoder->flags = 0;
    encoder->depth = 1;
    prbBufInit(&encoder->stack[0].buf, data, len);
    encoder->stack[0].field_desc = NULL;
    encoder->stack[0].msg_desc = msg_desc;

    encoder->pBufferCallback = pBufferCallback;
    if (pBufferCallback != NULL)
    {
        encoder->flags |= PRB_FIXED_MODE;
    }
}

/*!
 * @brief Starts the encoder with the first message
 *   Allocates memory to hold the data.
 *   If no memory was available, logging will be disabled
 *   and future calls to the encoding  routines with this encoder
 *   will succeed but no data will be logged.
 * @param [in] encoder The encoder structure
 * @param [in] msg_desc The message to encode
 * @param [in] len The caller's estimate of the number of bytes needed
 * @returns NV_STATUS
 */

NV_STATUS
prbEncStartAlloc
(
    PRB_ENCODER *encoder,
    const PRB_MSG_DESC *msg_desc,
    NvU32 len,
    PrbBufferCallback *pBufferCallback
)
{
    void *data = NULL;
    NV_STATUS rmstatus = NV_OK;

    data = portMemAllocNonPaged(len);
    if (data != NULL)
    {
        prbEncStart(encoder, msg_desc, data, len, pBufferCallback);
        encoder->flags |= PRB_BUFFER_ALLOCATED;
    }
    else
    {
        rmstatus = NV_ERR_NO_MEMORY;
        NV_PRINTF(LEVEL_ERROR,
                  "Can't allocate memory for protocol buffers.\n");
        // Disable all encoding
        encoder->flags = PRB_ENCODE_DISABLED;
    }
    return rmstatus;
}

/*!
 * @brief Starts the encoder with the first message
 *   This variant just counts the length of the message
 *   No data is stored
 * @param [in] encoder The encoder structure
 * @param [in] msg_desc The message to encode
 * @param [in] len Length of the buffer
 */

void
prbEncStartCount(PRB_ENCODER *encoder,
                 const PRB_MSG_DESC *msg_desc,
                 NvU32 len)
{
    prbEncStart(encoder, msg_desc, NULL, len, NULL);
    encoder->flags |= PRB_COUNT_ONLY;
}

/*!
 * @brief free an allocated buffer and disables encoding.
 * @param [in] encoder the encoder structure.
 */

void
prbFreeAllocatedBuffer(PRB_ENCODER *encoder)
{
    if (encoder->flags & PRB_BUFFER_ALLOCATED)
        prbBuffFree(&encoder->stack[0].buf);

    encoder->flags &= ~PRB_BUFFER_ALLOCATED;
    encoder->flags |= PRB_ENCODE_DISABLED;
    encoder->depth = 1;
}

/*!
 * @brief Finish encoding
 * @param [in] encoder The current encoder structure
 * @param [out] buff The address of the data buffer.
 * @returns the number of bytes encoded
 */

NvU32
prbEncFinish(PRB_ENCODER *encoder, void **buff)
{
    if (!(encoder->flags & PRB_ENCODE_DISABLED))
        prbEndPackedField(encoder);

    *buff = encoder->stack[0].buf.base;
    encoder->flags |= PRB_ENCODE_DISABLED;
    return prbBufUsed(&encoder->stack[0].buf);
}

/*!
 * @brief Start a nested message
 * @param [in] encoder The current encoder
 * @param [in] field_desc The field where the message starts
 * @returns NV_STATUS
 */

NV_STATUS
prbEncNestedStart(PRB_ENCODER *encoder,
                  const PRB_FIELD_DESC *field_desc)
{
    NV_STATUS status;

    PRB_ENCODER_STACK_FRAME *frame, *new_frame;

    if (encoder->flags & PRB_ENCODE_DISABLED)
    {
        return NV_ERR_INVALID_REQUEST;
    }

    status = prbEndPackedField(encoder);
    if (status != NV_OK)
        return status;

    // The field must be a message
    if (field_desc->opts.typ != PRB_MESSAGE)
    {
        DBG_BREAKPOINT();
        return NV_ERR_INVALID_REQUEST;
    }

    // Check max depth
    if (encoder->depth >= PRB_MAX_DEPTH)
    {
        DBG_BREAKPOINT();
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    // Get parent frame
    frame = &encoder->stack[encoder->depth - 1];

    // Create a new frame
    encoder->depth++;
    new_frame = &encoder->stack[encoder->depth - 1];
    new_frame->field_desc = field_desc;
    new_frame->msg_desc = field_desc->msg_desc;

    // Reserve a few bytes for the field on the parent frame. This is where
    // the field key (message) and the message length will be stored, once it
    // is known.
    if (prbBufLeft(&frame->buf) < MSG_RESERVE_BYTES)
    {
        encoder->depth--;
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    prbBufInit(&new_frame->buf, frame->buf.pos + MSG_RESERVE_BYTES,
                  prbBufLeft(&frame->buf) - MSG_RESERVE_BYTES);
    return NV_OK;
}

/*!
 * @brief Add bytes to a stubbed message.
 * @param [in] encoder The current encoder
 * @param [in] buffer The data to copy
 * @param [in] len The data length
 */

NV_STATUS
prbEncStubbedAddBytes(PRB_ENCODER *encoder, NvU8 *buffer, NvU32 len)
{
    PRB_ENCODER_STACK_FRAME *frame;
    NV_STATUS status = NV_OK;

    frame = &encoder->stack[encoder->depth - 1];

    // Check length
    if (len <= 0) {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Check if we have enough space
    if (prbBufLeft(&frame->buf) < len) {
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    status = prbEndPackedField(encoder);
    if (status != NV_OK)
        return status;

    // Move the (possibly overlapping) memory
    if (!COUNT_FLAG(encoder))
        portMemMove(frame->buf.pos, len, buffer, len);

    // Adjust current buffer position
    frame->buf.pos += len;
    return NV_OK;
}

/*!
 * @brief Internal helper routine to keep track of packed fields.
 * @param [in] encoder The encoder
 * @param [in] field_desc The field being packed
 * @returns NV_STATUS
 */

static NV_STATUS
prbStartPackedField(PRB_ENCODER *encoder,
                    const PRB_FIELD_DESC *field_desc)
{
    PRB_ENCODER_STACK_FRAME *frame, *new_frame;

    // The field must be packed
    if (!(field_desc->opts.flags & PRB_IS_PACKED))
    {
        DBG_BREAKPOINT();
        return NV_ERR_INVALID_REQUEST;
    }

    // Check max depth
    if (encoder->depth >= PRB_MAX_DEPTH)
    {
        DBG_BREAKPOINT();
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    // Get parent frame
    frame = &encoder->stack[encoder->depth - 1];

    // Create a new frame
    encoder->depth++;
    new_frame = &encoder->stack[encoder->depth - 1];
    new_frame->field_desc = field_desc;
    new_frame->msg_desc = frame->msg_desc;
    encoder->flags |= PRB_PACKED_FRAME;

    // Reserve a few bytes for the field on the parent frame. This is where
    // the field key (message) and the message length will be stored, once it
    // is known.
    if (prbBufLeft(&frame->buf) < MSG_RESERVE_BYTES)
    {
        encoder->depth--;
        encoder->flags &= ~PRB_PACKED_FRAME;
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    prbBufInit(&new_frame->buf, frame->buf.pos + MSG_RESERVE_BYTES,
                  prbBufLeft(&frame->buf) - MSG_RESERVE_BYTES);
    return NV_OK;
}

/*!
 * @brief End a nested message
 * @param [in] encoder The current encoder
 * @returns NV_STATUS
 */

NV_STATUS
prbEncNestedEnd(PRB_ENCODER *encoder)
{
    PRB_ENCODER_STACK_FRAME *frame;
    PRB_VALUE value;
    NV_STATUS status;

    if (encoder->flags & PRB_ENCODE_DISABLED)
    {
        return NV_ERR_INVALID_REQUEST;
    }

    status = prbEndPackedField(encoder);
    if (status != NV_OK)
        return status;

    if (encoder->depth <= 1)
    {
        DBG_BREAKPOINT();
        return NV_ERR_INVALID_REQUEST;
    }

    frame = &encoder->stack[encoder->depth - 1];

    encoder->depth--;

    value.message.data = frame->buf.base;
    value.message.len = prbBufUsed(&frame->buf);

    return prbEncAddField(encoder, frame->field_desc, &value);
}

/*!
 * @brief Return size of remaining buffer
 * @param [in] encoder The encoder to use
 * @returns bytes left
 */

NvU32
prbEncBufLeft(PRB_ENCODER *encoder)
{
    PRB_ENCODER_STACK_FRAME *frame;

    if (encoder->depth <= 0)
    {
        DBG_BREAKPOINT();
        return 0;
    }

    frame = &encoder->stack[encoder->depth - 1];

    return prbBufLeft(&frame->buf);
}

/*!
 * @brief End a packed field
 * @param [in] encoder The encoder to use
 * @returns NV_STATUS
 */

static NV_STATUS
prbEndPackedField(PRB_ENCODER *encoder)
{
    PRB_ENCODER_STACK_FRAME *frame;
    const PRB_FIELD_DESC *field_desc;
    NV_STATUS ret;
    NvU64 key;
    NvU8 *packed_start;
    NvU32 packed_len;

    if (encoder->depth < 1)
    {
        DBG_BREAKPOINT();
        return NV_ERR_INVALID_REQUEST;
    }

    frame = &encoder->stack[encoder->depth - 1];
    if (encoder->flags & PRB_PACKED_FRAME)
    {
        // Clear the flag
        encoder->flags &= ~PRB_PACKED_FRAME;

        // Pick up field desc, packed start and length
        packed_start = frame->buf.base;
        packed_len = prbBufUsed(&frame->buf);
        field_desc = frame->field_desc;

        if (encoder->depth == 1)
        {
            DBG_BREAKPOINT();
            return NV_ERR_INVALID_REQUEST;
        }

        // Switch to parent frame
        encoder->depth--;
        frame = &encoder->stack[encoder->depth - 1];

        if (packed_len == 0)
            return(NV_OK);

        key = WT_STRING | (field_desc->number << 3);
        ret = encode_varint(&frame->buf, key, COUNT_FLAG(encoder));
        if (ret != NV_OK)
            return ret;

        ret = encode_varint(&frame->buf, packed_len, COUNT_FLAG(encoder));
        if (ret != NV_OK)
            return ret;

        if (prbBufLeft(&frame->buf) < packed_len)
            return NV_ERR_BUFFER_TOO_SMALL;

        if (!COUNT_FLAG(encoder))
            portMemMove(frame->buf.pos, packed_len, packed_start, packed_len);
        frame->buf.pos += packed_len;
    }
    return NV_OK;
}

/*!
 * @brief Add a field to a message
 * @param [in] encoder The encoder to use
 * @param [in] field_desc Which field to add
 * @param [in] value The value to send
 * @returns NV_STATUS
 *   NV_ERR_INVALID_REQUEST if the field is not found
 */

static NV_STATUS
prbEncAddField(PRB_ENCODER *encoder,
               const PRB_FIELD_DESC *field_desc,
               PRB_VALUE *value)
{
    NV_STATUS ret;
    PRB_ENCODER_STACK_FRAME *frame;
    NvU32 i;
    NvU64 key;
    WIRE_TYPE wire_type = WT_32BIT;
    WIRE_VALUE wire_value = {0};

    if (encoder->flags & PRB_ENCODE_DISABLED)
    {
        return NV_ERR_INVALID_REQUEST;
    }

    if (encoder->depth <= 0)
    {
        DBG_BREAKPOINT();
        return NV_ERR_INVALID_REQUEST;
    }

    frame = &encoder->stack[encoder->depth - 1];

    // If this field is not stubbed, then...
    if (!(field_desc->opts.flags & PRB_STUBBED_FIELD)) {

        // Check that field belongs to the current message
        for (i = 0; i < frame->msg_desc->num_fields; i++)
            if (field_desc == &frame->msg_desc->fields[i])
                break;
        if (i == frame->msg_desc->num_fields)
        {
            DBG_BREAKPOINT();
            return NV_ERR_INVALID_REQUEST;
        }
    }

    if (field_desc->opts.flags & PRB_IS_DEPRECATED)
    {
        // OK to read deprecated fields, but we should not write them.
        DBG_BREAKPOINT();
    }

    if (field_desc->opts.flags & PRB_IS_PACKED)
    {
        // Need to start a new packed field?
        if (frame->field_desc != field_desc)
        {
            ret = prbEndPackedField(encoder);
            if (ret != NV_OK)
                return ret;

            ret = prbStartPackedField(encoder, field_desc);
            if (ret != NV_OK)
                return ret;
        }

    }
    else
    {
        ret = prbEndPackedField(encoder);
        if (ret != NV_OK)
            return ret;
    }

    // Messing with packed fields may have changed depth
    frame = &encoder->stack[encoder->depth - 1];

    // Encode wire value
    switch (field_desc->opts.typ)
    {
        case PRB_DOUBLE:
            wire_type = WT_64BIT;
            wire_value.int64 = *((NvU64 *) &value->double_);
            break;
        case PRB_FLOAT:
            wire_type = WT_32BIT;
            wire_value.int32 = *((NvU32 *) &value->float_);
            break;
        case PRB_INT32:
            wire_type = WT_VARINT;
            wire_value.varint = value->int32;
            break;
        case PRB_UINT32:
            wire_type = WT_VARINT;
            wire_value.varint = value->uint32;
            break;
        case PRB_SINT32:
            // Zig-zag encoding
            wire_type = WT_VARINT;
            wire_value.varint = (NvU32) ((value->int32 << 1) ^ (value->int32 >> 31));
            break;
        case PRB_INT64:
            wire_type = WT_VARINT;
            wire_value.varint = value->int64;
            break;
        case PRB_UINT64:
            wire_type = WT_VARINT;
            wire_value.varint = value->uint64;
            break;
        case PRB_SINT64:
            // Zig-zag encoding
            wire_type = WT_VARINT;
            wire_value.varint = (NvU64) ((value->int64 << 1) ^ (value->int64 >> 63));
            break;
        case PRB_FIXED32:
            wire_type = WT_32BIT;
            wire_value.int32 = value->uint32;
            break;
        case PRB_FIXED64:
            wire_type = WT_64BIT;
            wire_value.int64 = value->uint64;
            break;
        case PRB_SFIXED32:
            wire_type = WT_32BIT;
            wire_value.int32 = value->int32;
            break;
        case PRB_SFIXED64:
            wire_type = WT_64BIT;
            wire_value.int64 = value->int64;
            break;
        case PRB_BOOL:
            wire_type = WT_VARINT;
            wire_value.varint = value->bool_;
            break;
        case PRB_ENUM:
            wire_type = WT_VARINT;
            wire_value.varint = value->enum_;
            break;
        case PRB_STRING:
            wire_type = WT_STRING;
            wire_value.string.data = value->string.str;
            wire_value.string.len = value->string.len;
            break;
        case PRB_BYTES:
            wire_type = WT_STRING;
            wire_value.string.data = value->bytes.data;
            wire_value.string.len = value->bytes.len;
            break;
        case PRB_MESSAGE:
            wire_type = WT_STRING;
            wire_value.string.data = value->message.data;
            wire_value.string.len = value->message.len;
            break;
    }

    if (!(field_desc->opts.flags & PRB_IS_PACKED))
    {
        key = wire_type | (field_desc->number << 3);
        ret = encode_varint(&frame->buf, key, COUNT_FLAG(encoder));
        if (ret != NV_OK)
            return ret;
    }

    switch (wire_type)
    {
        case WT_VARINT:
            ret = encode_varint(&frame->buf, wire_value.varint, COUNT_FLAG(encoder));
            if (ret != NV_OK)
                return ret;
            break;
        case WT_64BIT:
            ret = encode_64bit(&frame->buf, wire_value.int64, COUNT_FLAG(encoder));
            if (ret != NV_OK)
                return ret;
            break;
        case WT_STRING:
            ret = encode_varint(&frame->buf, wire_value.string.len, COUNT_FLAG(encoder));
            if (ret != NV_OK)
                return ret;
            if (prbBufLeft(&frame->buf) < wire_value.string.len)
                return NV_ERR_BUFFER_TOO_SMALL;
            if (!COUNT_FLAG(encoder))
            {
                // Use memmove() when writing a message field as the memory areas are
                // overlapping.
                if (field_desc->opts.typ == PRB_MESSAGE)
                {
                    portMemMove(frame->buf.pos, (NvU32)wire_value.string.len,
                                wire_value.string.data, (NvU32)wire_value.string.len);
                }
                else
                {
                    portMemCopy(frame->buf.pos, (NvU32)wire_value.string.len, wire_value.string.data, (NvU32)wire_value.string.len);
                }
            }
            frame->buf.pos += wire_value.string.len;
            break;
        case WT_32BIT:
            ret = encode_32bit(&frame->buf, wire_value.int32, COUNT_FLAG(encoder));
            if (ret != NV_OK)
                return ret;
            break;
        default:
            DBG_BREAKPOINT();
            break;
    }

    return NV_OK;
}

/*!
 * @brief Encode a signed 32 bit integer argument
 * @param [in] encoder The encoder to use
 * @param [in] field_desc The field descriptor to use
 * @param [in] int32 The value to encode
 * @returns NV_STATUS
 */

NV_STATUS
prbEncAddInt32(PRB_ENCODER *encoder,
               const PRB_FIELD_DESC *field_desc,
               NvS32 int32)
{
    PRB_VALUE value;

    value.int32 = int32;
    return prbEncAddField(encoder, field_desc, &value);
}

/*!
 * @brief Encode an unsigned 32 bit integer argument
 * @param [in] encoder The encoder to use
 * @param [in] field_desc The field descriptor to use
 * @param [in] uint32 The value to encode
 * @returns NV_STATUS
 */

NV_STATUS
prbEncAddUInt32(PRB_ENCODER *encoder,
                const PRB_FIELD_DESC *field_desc,
                NvU32 uint32)
{
    PRB_VALUE value;

    value.uint32 = uint32;
    return prbEncAddField(encoder, field_desc, &value);
}

/*!
 * @brief Encode a 64 bit signed integer
 * @param [in] encoder The encoder to use
 * @param [in] field_desc The field descriptor to use
 * @param [in] int64 The value to encode
 * @returns NV_STATUS
 */

NV_STATUS
prbEncAddInt64(PRB_ENCODER *encoder,
               const PRB_FIELD_DESC *field_desc,
               NvS64 int64)
{
    PRB_VALUE value;

    value.int64 = int64;
    return prbEncAddField(encoder, field_desc, &value);
}

/*!
 * @brief Encode a 64 bit unsigned integer
 * @param [in] encoder The encoder to use
 * @param [in] field_desc The field descriptor to use
 * @param [in] uint64 The value to encode
 * @returns NV_STATUS
 */

NV_STATUS
prbEncAddUInt64(PRB_ENCODER *encoder,
                const PRB_FIELD_DESC *field_desc,
                NvU64 uint64)
{
    PRB_VALUE value;

    value.uint64 = uint64;
    return prbEncAddField(encoder, field_desc, &value);
}

/*!
 * @brief Encode a boolean
 * @param [in] encoder The encoder to use
 * @param [in] field_desc The field descriptor to use
 * @param [in] bool_ The value to encode
 * @returns NV_STATUS
 */

NV_STATUS
prbEncAddBool(PRB_ENCODER *encoder,
              const PRB_FIELD_DESC *field_desc,
              NvBool bool_)
{
    PRB_VALUE value;

    value.bool_ = bool_;
    return prbEncAddField(encoder, field_desc, &value);
}

/*!
 * @brief Encode an enum
 * @param [in] encoder The encoder to use
 * @param [in] field_desc The field descriptor to use
 * @param [in] enum_ The value to encode
 * @returns NV_STATUS
 */

NV_STATUS
prbEncAddEnum(PRB_ENCODER *encoder,
              const PRB_FIELD_DESC *field_desc,
              int enum_)
{
    PRB_VALUE value;

    value.enum_ = enum_;
    return prbEncAddField(encoder, field_desc, &value);
}

/*!
 * @brief Encode a null terminated string
 * @param [in] encoder The encoder to use
 * @param [in] field_desc The field descriptor to use
 * @param [in] str The value to encode
 * @returns NV_STATUS
 */

NV_STATUS
prbEncAddString(PRB_ENCODER *encoder,
                const PRB_FIELD_DESC *field_desc,
                const char *str)
{
    PRB_VALUE value;

    value.string.str = str;
    value.string.len = (NvU32)portStringLength(str);
    return prbEncAddField(encoder, field_desc, &value);
}

/*!
 * @brief Encode a byte array
 * @param [in] encoder The encoder to use
 * @param [in] field_desc The field descriptor to use
 * @param [in] data The value to encode
 * @param [in] len The length of the byte array
 * @returns NV_STATUS
 */

NV_STATUS
prbEncAddBytes(PRB_ENCODER *encoder,
               const PRB_FIELD_DESC *field_desc,
               const NvU8 *data,
               NvU32 len)
{
    PRB_VALUE value;

    value.string.str = (const char *) data;
    value.string.len = len;
    return prbEncAddField(encoder, field_desc, &value);
}

/*!
 * @brief concatenate a message to an encoded message stream
 * @param [in] encoder The encoder to use
 * @param [in] pMsg The pointer to the message to add to the steam
 * @param [in] len Length of the message
 * @returns NV_STATUS
 */

NV_STATUS
prbEncCatMsg(PRB_ENCODER *encoder, void *pMsg, NvU32 len)
{
    NV_STATUS               status;
    NvU8                    *pBuff = pMsg;
    NvU32                   used;
    NvU64                   msgVarint1;
    NvU64                   msgVarint2;
    WIRE_TYPE               msgWireType;
    NvU32                   msgField;
    NvU32                   msgLen;
    PRB_ENCODER_STACK_FRAME *frame;
    const PRB_FIELD_DESC    *field_desc;
    NvU32                   i;

    // Get field, wiretype
    status = decode_varint(pBuff, len, &used, &msgVarint1);
    if (status != NV_OK)
        return status;

    if (used >= len)
        return NV_ERR_BUFFER_TOO_SMALL;

    msgWireType = (WIRE_TYPE)msgVarint1 & 0x7;
    if (msgWireType != WT_STRING)
        return NV_ERR_INVALID_REQUEST;

    msgField = (NvU32)msgVarint1 >> 3;

    // Find a field descriptor for the new message
    frame = &encoder->stack[encoder->depth - 1];
    for (i = 0; i < frame->msg_desc->num_fields; i++)
    {
        if (frame->msg_desc->fields[i].number == msgField)
            break;
    }

    if (i == frame->msg_desc->num_fields)
    {
        DBG_BREAKPOINT();
        return NV_ERR_INVALID_REQUEST;
    }
    else
    {
        field_desc = &frame->msg_desc->fields[i];
    }

    pBuff += used;
    len -= used;

    // Get length
    status = decode_varint(pBuff, len, &used, &msgVarint2);
    if (status != NV_OK)
        return status;

    msgLen = (NvU32)msgVarint2;
    if (msgLen > len - used)
        return NV_ERR_BUFFER_TOO_SMALL;

    return prbEncAddBytes(encoder, field_desc, pBuff + used, msgLen);
}

