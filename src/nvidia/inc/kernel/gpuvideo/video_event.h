/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef VIDEO_EVENT_H
#define VIDEO_EVENT_H

/*!
**************  Defines used for Video HW event logging! ***********************
*/

/*!
* Defines a skip factor when event buffer is full,
* i.e. 1/(RM_VIDEO_TRACE_SURFACE_SKIP_FACTOR) of the event buffers
* will be skipped and over-written.
*/
#define RM_VIDEO_TRACE_SURFACE_SKIP_FACTOR                        (16UL)

/*!
* Defines max size of a variable data in one record. For event data has a size
* that is larger than this define, multiple event records will be used for logging.
*/
#define RM_VIDEO_TRACE_MAX_VARIABLE_DATA_SIZE                     (0x1000UL)

/*!
* Defines magic value for hardware video events
*/
#define ENG_VIDEO_TRACE_EVENT_MAGIC_HI                            (0xdeadbeefUL)
#define ENG_VIDEO_TRACE_EVENT_MAGIC_LO                            (0xdead0001UL)

/*!
* Define the type of the engine. Since engine uCode can not determine the actual id,
* such as NVENC0, NVENC1 etc., we will defer this to RM to determine the actual id
* and populate it in final event data structure.
*/
typedef enum
{
    VIDEO_ENGINE_TYPE__NVDEC = 0,
    VIDEO_ENGINE_TYPE__NVENC,
    VIDEO_ENGINE_TYPE__NVJPG,
    VIDEO_ENGINE_TYPE__OFA,
    VIDEO_ENGINE_TYPE__NUM
} VIDEO_ENGINE_TYPE;

/*!
* Defines video codec types
*/
typedef enum
{
    VIDEO_ENGINE_CODEC_ID__OFA = 0,                 /* Only codec supported for OFA engine. */
    VIDEO_ENGINE_CODEC_ID__JPEG = 0,                /* Only codec supported for NVJPEG engine. */
    VIDEO_ENGINE_CODEC_ID__MPEG2 = 0,
    VIDEO_ENGINE_CODEC_ID__VC1,
    VIDEO_ENGINE_CODEC_ID__H264,
    VIDEO_ENGINE_CODEC_ID__MPEG4,
    VIDEO_ENGINE_CODEC_ID__VP8,
    VIDEO_ENGINE_CODEC_ID__VP9,
    VIDEO_ENGINE_CODEC_ID__HEVC,
    VIDEO_ENGINE_CODEC_ID__AVD,
    VIDEO_ENGINE_CODEC_ID__NUM
} VIDEO_ENGINE_CODEC_ID;

/*!
* Defines types of video events
*/
typedef enum
{
    VIDEO_ENGINE_EVENT_ID__SESSION_START = 0,
    VIDEO_ENGINE_EVENT_ID__SESSION_END,
    VIDEO_ENGINE_EVENT_ID__POWER_STATE_CHANGE,
    VIDEO_ENGINE_EVENT_ID__LOG_DATA,
    VIDEO_ENGINE_EVENT_ID__NUM
} VIDEO_ENGINE_EVENT_ID;

/*!
* Defines types of the log data of video events
*/
typedef enum
{
    VIDEO_ENGINE_EVENT_LOG_DATA_TYPE__GENERIC = 0,  /* Generic opaque data */
    VIDEO_ENGINE_EVENT_LOG_DATA_TYPE__STR,          /* Log data can be interpret as string */
    VIDEO_ENGINE_EVENT_LOG_DATA_TYPE__BIN,          /* Log data are binary data */
    VIDEO_ENGINE_EVENT_LOG_DATA_TYPE__NUM
} VIDEO_ENGINE_EVENT_LOG_DATA_TYPE;

/*!
* Defines return status for VIDEO_ENGINE_EVENT_ID__SESSION_END
*/
typedef enum
{
    VIDEO_ENGINE_STATUS__OK = 0,
    VIDEO_ENGINE_STATUS__ERR,
    VIDEO_ENGINE_STATUS__NUM
} VIDEO_ENGINE_STATUS;

/*!
* Defines structs for various types of trace events
*/
typedef struct {
    NvU8     engine_type;
    NvU8     engine_id;
    NvU16    codec_id;
    NvU32    reserved1;
} VIDEO_ENGINE_EVENT__SESSION_START;

typedef struct {
    NvU8     engine_type;
    NvU8     engine_id;
    NvU16    codec_id;
    NvU32    status;
} VIDEO_ENGINE_EVENT__SESSION_END;


typedef struct {
    NvU16    from;
    NvU16    to;
    NvU32    reserved2;
} VIDEO_ENGINE_EVENT__POWER_STATE_CHANGE;

typedef struct
{
    NvU8     engine_type;
    NvU8     engine_id;
    NvU16    type;              /* VIDEO_ENGINE_EVENT_LOG_DATA_TYPE */
    NvU32    size;              /* size of data being logged in NvU32 */
} VIDEO_ENGINE_EVENT__LOG_DATA;

/*!
* Defines the struct for a full trace record contains various event structs.
*/
typedef struct
{

    NvU32                 magic_lo;
    NvU32                 magic_hi;

    union
    {
        NvU64             ts;
        struct
        {
            NvU32         ts_lo;
            NvU32         ts_hi;
        };
    };

    VIDEO_ENGINE_EVENT_ID event_id;
    NvU32                 seq_no;

    union {

        NvU64                                   event_data;

        VIDEO_ENGINE_EVENT__SESSION_START       event_start;
        VIDEO_ENGINE_EVENT__SESSION_END         event_end;
        VIDEO_ENGINE_EVENT__POWER_STATE_CHANGE  event_pstate_change;
        VIDEO_ENGINE_EVENT__LOG_DATA            event_log_data;
    };

    // Client information from RM
    NvU64 userInfo;
    NvU32 context_id;
    NvU32 pid;
    NvU64 api_id;
    NvU32 gfid;
    NvU32 reserved;

} VIDEO_ENGINE_EVENT__RECORD;

#define VIDEO_ENGINE_EVENT__LOG_DATA_SIZE(s) (sizeof(VIDEO_ENGINE_EVENT__RECORD) + s.event_log_data.size)

#define VIDEO_ENGINE_EVENT__TRACE_ADDR__OFFSET_LO  (0xF90UL)
#define VIDEO_ENGINE_EVENT__TRACE_ADDR__OFFSET_HI  (0xF94UL)
#define VIDEO_ENGINE_EVENT__TRACE_ADDR__MEM_TARGET (0xF98UL)

/*!
* Client information passing down by RM and saved at offset VIDEO_ENGINE_EVENT__LOG_INFO__OFFSET
* with the size of VIDEO_ENGINE_EVENT__LOG_INFO__SIZE in context allocation.
*/
#define VIDEO_ENGINE_EVENT__LOG_INFO__OFFSET   (0xFA0UL)
#define VIDEO_ENGINE_EVENT__LOG_INFO__SIZE     (0x20UL)
#define VIDEO_ENGINE_EVENT__LOG_INFO__LOG_SIZE (5UL)
#define VIDEO_ENGINE_EVENT__KERNEL_PID         (0xFFFFFFFFUL)
#define VIDEO_ENGINE_EVENT__KERNEL_CONTEXT     (0xFFFFFFFFUL)

typedef struct {
    NvU64 userInfo;
    NvU32 context_id;
    NvU32 pid;
    NvU32 gfid;
    NvU8  engine_id;
    NvU8  reserved3[31];
} VIDEO_ENGINE_EVENT__LOG_INFO;

/*!
* Data-structure for ringbuffer structure related to video ringbuffer surfaces.
*/
typedef struct {
    NvU32   bufferSize;
    NvU32   readPtr;
    NvU32   writePtr;
    NvU32   flags;
    NvU8    pData[];
} VIDEO_TRACE_RING_BUFFER, *PVIDEO_TRACE_RING_BUFFER;

/* Macro convert data offer to ringbuffer offset*/
#define VIDEO_TRACE_RING_BUFFER__DATA_OFFSET(o) (o + sizeof(VIDEO_TRACE_RING_BUFFER))
#define VIDEO_TRACE_RING_BUFFER__OFFSET(t) (&(((PVIDEO_TRACE_RING_BUFFER)0)->t))
#define VIDEO_TRACE_RING_BUFFER__SIZEOF(t) (sizeof(((PVIDEO_TRACE_RING_BUFFER)0)->t))

#define VIDEO_TRACE_FLAG__LOGGING_ENABLED (0x00000001UL)
#define VIDEO_TRACE_FLAG__RESERVED        (0xFFFFFFFEUL)

#endif // VIDEO_EVENT_H
