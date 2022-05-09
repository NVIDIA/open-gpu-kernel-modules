/*
 * Lightweight protocol buffers.
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

#ifndef __PRB_H__
#define __PRB_H__

// Make sure the generated files can see rmconfig.h

#ifndef _RMCFG_H
#include "rmconfig.h"
#endif

// Maximum depth of message embedding
#ifndef PRB_MAX_DEPTH
#define PRB_MAX_DEPTH 8
#endif

// Maximum number of required fields in a message
#ifndef PRB_MAX_REQUIRED_FIELDS
#define PRB_MAX_REQUIRED_FIELDS 16
#endif

// Provide enum names as strings
#ifndef PRB_ENUM_NAMES
#define PRB_ENUM_NAMES 0
#endif

#if PRB_ENUM_NAMES
#define PRB_MAYBE_ENUM_NAME(n) n,
#else
#define PRB_MAYBE_ENUM_NAME(n)
#endif

// Provide field names as strings
#ifndef PRB_FIELD_NAMES
#define PRB_FIELD_NAMES 0
#endif

#if PRB_FIELD_NAMES
#define PRB_MAYBE_FIELD_NAME(n) n,
#else
#define PRB_MAYBE_FIELD_NAME(n)
#endif

// Provide field default values
#ifndef PRB_FIELD_DEFAULTS
#define PRB_FIELD_DEFAULTS 0
#endif

#if PRB_FIELD_DEFAULTS
#define PRB_MAYBE_FIELD_DEFAULT_DEF(n) n
#define PRB_MAYBE_FIELD_DEFAULT(n) n,
#else
#define PRB_MAYBE_FIELD_DEFAULT_DEF(n)
#define PRB_MAYBE_FIELD_DEFAULT(n)
#endif

// Provide message names as strings
#ifndef PRB_MESSAGE_NAMES
#define PRB_MESSAGE_NAMES 0
#endif

#if PRB_MESSAGE_NAMES
#define PRB_MAYBE_MESSAGE_NAME(n) n,
#else
#define PRB_MAYBE_MESSAGE_NAME(n)
#endif

// Provide method names as strings
#ifndef PRB_METHOD_NAMES
#define PRB_METHOD_NAMES 0
#endif

#if PRB_METHOD_NAMES
#define PRB_MAYBE_METHOD_NAME(n) n,
#else
#define PRB_MAYBE_MESSAGE_NAME(n)
#endif

// Provide service names as strings
#ifndef PRB_SERVICE_NAMES
#define PRB_SERVICE_NAMES 0
#endif

#if PRB_SERVICE_NAMES
#define PRB_MAYBE_SERVICE_NAME(n) n,
#else
#define PRB_MAYBE_SERVICE_NAME(n)
#endif

// Field labels
#define PRB_REQUIRED       0
#define PRB_OPTIONAL       1
#define PRB_REPEATED       2

// Field value types
#define PRB_DOUBLE         0
#define PRB_FLOAT          1
#define PRB_INT32          2
#define PRB_INT64          3
#define PRB_UINT32         4
#define PRB_UINT64         5
#define PRB_SINT32         6
#define PRB_SINT64         7
#define PRB_FIXED32        8
#define PRB_FIXED64        9
#define PRB_SFIXED32       10
#define PRB_SFIXED64       11
#define PRB_BOOL           12
#define PRB_ENUM           13
#define PRB_STRING         14
#define PRB_BYTES          15
#define PRB_MESSAGE        16

// Field flags
#define PRB_HAS_DEFAULT    (1 << 0)
#define PRB_IS_PACKED      (1 << 1)
#define PRB_IS_DEPRECATED  (1 << 2)

typedef struct
{
    unsigned int label : 2;
    unsigned int typ : 6;
    unsigned int flags : 8;
} PRB_FIELD_OPTS;

// Protocol buffer wire types
typedef enum
{
    WT_VARINT = 0,
    WT_64BIT  = 1,
    WT_STRING = 2,
    WT_32BIT  = 5
} WIRE_TYPE;

// Protocol buffer wire values
typedef union
{
    NvU64 varint;
    NvU64 int64;
    struct {
        NvU64 len;
        const void *data;
    } string;
    NvU32 int32;
} WIRE_VALUE;

typedef struct
{
    const char *str;
    NvU32 len;
} PRB_VALUE_STRING;

typedef struct
{
    NvU8 *data;
    NvU32 len;
} PRB_VALUE_BYTES;

typedef struct
{
    void *data;
    NvU32 len;
} PRB_VALUE_MESSAGE;

typedef union
{
    NvF64 double_;
    NvF32 float_;
    NvS32 int32;
    NvS64 int64;
    NvU32 uint32;
    NvU64 uint64;
    NvBool bool_;
    PRB_VALUE_STRING string;
    PRB_VALUE_BYTES bytes;
    PRB_VALUE_MESSAGE message;
    int enum_;
    int null;
} PRB_VALUE;

typedef struct
{
    int value;
#if PRB_ENUM_NAMES
    const char *name;
#endif
} PRB_ENUM_MAPPING;

typedef struct
{
    const PRB_ENUM_MAPPING *mappings;
    NvU32 count;
#if PRB_ENUM_NAMES
    const char *name;
#endif
} PRB_ENUM_DESC;

struct PRB_MSG_DESC;

//* Protocol buffer field descriptor
typedef struct PRB_FIELD_DESC
{
    NvU32 number;
    PRB_FIELD_OPTS opts;
    const struct PRB_MSG_DESC *msg_desc;
    const PRB_ENUM_DESC *enum_desc;
#if PRB_FIELD_NAMES
    const char *name;
#endif
#if PRB_FIELD_DEFAULTS
    const PRB_VALUE *def;
#endif
} PRB_FIELD_DESC;

//* Protocol buffer message descriptor
typedef struct PRB_MSG_DESC
{
    NvU32 num_fields;
    const PRB_FIELD_DESC *fields;
#if PRB_MESSAGE_NAMES
    const char *name;
#endif
} PRB_MSG_DESC;

// Forward declaration
struct PRB_SERVICE_DESC;

// Protocol buffer method descriptor
struct PRB_METHOD_DESC
{
    const struct PRB_SERVICE_DESC *service;
    const PRB_MSG_DESC *req_desc;
    const PRB_MSG_DESC *res_desc;
#if PRB_METHOD_NAMES
    const char *name;
#endif
};

// Protocol buffer service descriptor
typedef struct PRB_SERVICE_DESC
{
    const NvU32 num_methods;
    const struct PRB_METHOD_DESC *methods;
#if PRB_SERVICE_NAMES
    const char *name;
#endif
} PRB_SERVICE_DESC;

// Simple memory buffer
typedef struct
{
    NvU8 *base;
    NvU8 *pos;
    NvU8 *end;
} PRB_BUF;

// Encoder interface
typedef struct
{
    PRB_BUF buf;
    const PRB_FIELD_DESC *field_desc;
    const PRB_MSG_DESC *msg_desc;
} PRB_ENCODER_STACK_FRAME;

typedef NV_STATUS PrbBufferCallback(void *pEncoder, NvBool bBufferFull);

typedef struct
{
    PRB_ENCODER_STACK_FRAME stack[PRB_MAX_DEPTH];
    PrbBufferCallback *pBufferCallback;
    int depth;
    int flags;
} PRB_ENCODER;

// flags
#define PRB_ENCODE_DISABLED  0x01
#define PRB_BUFFER_ALLOCATED 0x02
#define PRB_PACKED_FRAME     0x04
#define PRB_STUBBED_FIELD    0x08
#define PRB_COUNT_ONLY       0x10
#define PRB_FIXED_MODE       0x20

// Slop to allow for message headers, etc.
#define PRB_MSG_OVERHEAD 32

void prbEncStart(PRB_ENCODER *encoder, const PRB_MSG_DESC *msg_desc,
                 void *data, NvU32 len, PrbBufferCallback *pBufferCallback);
NV_STATUS prbEncStartAlloc(PRB_ENCODER *encoder,
                           const PRB_MSG_DESC *msg_desc, NvU32 len,
                           PrbBufferCallback *pBufferCallback);
void prbEncStartCount(PRB_ENCODER *encoder,
                 const PRB_MSG_DESC *msg_desc, NvU32 len);
void prbFreeAllocatedBuffer(PRB_ENCODER *encoder);
NvU32 prbEncFinish(PRB_ENCODER *encoder, void **buff);
NV_STATUS prbEncNestedStart(PRB_ENCODER *encoder,
                            const PRB_FIELD_DESC *field_desc);
NV_STATUS prbEncNestedEnd(PRB_ENCODER *encoder);
NV_STATUS prbEncStubbedAddBytes(PRB_ENCODER *encoder, NvU8 *buffer,
                           NvU32 len);
NV_STATUS prbEncAddInt32(PRB_ENCODER *encoder,
                         const PRB_FIELD_DESC *field_desc,
                         NvS32 int32);
NV_STATUS prbEncAddUInt32(PRB_ENCODER *encoder,
                          const PRB_FIELD_DESC *field_desc,
                          NvU32 uint32);
NV_STATUS prbEncAddInt64(PRB_ENCODER *encoder,
                         const PRB_FIELD_DESC *field_desc,
                         NvS64 int64);
NV_STATUS prbEncAddUInt64(PRB_ENCODER *encoder,
                          const PRB_FIELD_DESC *field_desc,
                          NvU64 uint64);
NV_STATUS prbEncAddBool(PRB_ENCODER *encoder,
                        const PRB_FIELD_DESC *field_desc,
                        NvBool boolVal);
NV_STATUS prbEncAddEnum(PRB_ENCODER *encoder,
                        const PRB_FIELD_DESC *field_desc,
                        int enum_);
NV_STATUS prbEncAddString(PRB_ENCODER *encoder,
                          const PRB_FIELD_DESC *field_desc,
                          const char *str);
NV_STATUS prbEncAddBytes(PRB_ENCODER *encoder,
                         const PRB_FIELD_DESC *field_desc,
                         const NvU8 *data, NvU32 len);
NV_STATUS prbEncCatMsg(PRB_ENCODER *encoder,
                       void *pMsg, NvU32 len);
NvU32     prbEncBufLeft(PRB_ENCODER *encoder);

#endif  // __PRB_H__
