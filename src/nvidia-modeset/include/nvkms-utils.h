/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __NVKMS_UTILS_H__
#define __NVKMS_UTILS_H__

#include "nvkms-types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "nvidia-modeset-os-interface.h"

/*!
 * Subtract B from A, and handle wrap around.
 *
 * This is useful for cases where A is a number that is incremented and wrapped;
 * e.g.,
 *
 *  a = (a + 1) % max;
 *
 * and we want to subtract some amount from A to get one of its previous values.
 */
static inline NvU8 A_minus_b_with_wrap_U8(NvU8 a, NvU8 b, NvU8 max)
{
    return (a + max - b) % max;
}

/*!
 * Return whether (A + B) > C, avoiding integer overflow in the addition.
 */
static inline NvBool A_plus_B_greater_than_C_U16(NvU16 a, NvU16 b, NvU16 c)
{
    return (NV_U16_MAX - a < b) || ((a + b) > c);
}

static inline NvBool A_plus_B_greater_than_C_U64(NvU64 a, NvU64 b, NvU64 c)
{
    return (NV_U64_MAX - a < b) || ((a + b) > c);
}

static inline NvS32 clamp_S32(NvS32 val, NvS32 lo, NvS32 hi)
{
    if (val < lo) {
        return lo;
    } else if (val > hi) {
        return hi;
    } else {
        return val;
    }
}

/*!
 * Return whether the bitmask contains bits greater than or equal to
 * the maximum.
 */
static inline NvBool nvHasBitAboveMax(NvU32 bitmask, NvU8 max)
{
    nvAssert(max <= 32);
    if (max == 32) {
        return FALSE;
    }
    return (bitmask & ~((1 << max) - 1)) != 0;
}

/*!
 * Check if a timeout is exceeded.
 *
 * This is intended to be used when busy waiting in a loop, like this:
 *
 *  NvU64 startTime = 0;
 *
 *  do {
 *      if (SOME-CONDITION) {
 *          break;
 *      }
 *
 *      if (nvExceedsTimeoutUSec(pDevEvo, &startTime, TIMEOUT-IN-USEC)) {
 *          break;
 *      }
 *
 *      nvkms_yield();
 *
 *  } while (TRUE);
 *
 * The caller should zero-initialize startTime, and nvExceedsTimeoutUSec() will
 * set startTime to the starting time on the first call.  This is structured
 * this way to avoid the nvkms_get_usec() call in the common case where
 * SOME-CONDITION is true on the first iteration (nvkms_get_usec() is not
 * expected to be a large penalty, but it still seems nice to avoid it when not
 * needed).
 */
static inline NvBool nvExceedsTimeoutUSec(
    const NVDevEvoRec *pDevEvo,
    NvU64 *pStartTime,
    NvU64 timeoutPeriod)
{
    const NvU64 currentTime = nvkms_get_usec();

    if (nvIsEmulationEvo(pDevEvo) && !nvIsDfpgaEvo(pDevEvo)) {
        timeoutPeriod *= 100;
    }

    if (*pStartTime == 0) {
        *pStartTime = currentTime;
        return FALSE;
    }

    if (currentTime < *pStartTime) { /* wraparound?! */
        return TRUE;
    }

    return (currentTime - *pStartTime) > timeoutPeriod;
}

/*!
 * Return a non-NULL string.
 *
 * The first argument, stringMightBeNull, could be NULL.  In which
 * case, return the second argument, safeString, which the caller
 * should ensure is not NULL (e.g., by providing a literal).
 *
 * This is intended as a convenience for situations like this:
 *
 *   char *s = FunctionThatMightReturnNull();
 *   printf("%s\n", nvSafeString(s, "stringLiteral"));
 */
static inline const char *nvSafeString(char *stringMightBeNull,
                                       const char *safeString)
{
    return (stringMightBeNull != NULL) ? stringMightBeNull : safeString;
}

static inline NvU64 nvCtxDmaOffsetFromBytes(NvU64 ctxDmaOffset)
{
    nvAssert((ctxDmaOffset & ((1 << NV_SURFACE_OFFSET_ALIGNMENT_SHIFT) - 1))
             == 0);

    return (ctxDmaOffset >> 8);
}

NvU8 nvPixelDepthToBitsPerComponent(enum nvKmsPixelDepth pixelDepth);

typedef enum {
    EVO_LOG_WARN,
    EVO_LOG_ERROR,
    EVO_LOG_INFO,
} NVEvoLogType;

void *nvInternalAlloc(size_t size, NvBool zero);
void *nvInternalRealloc(void *ptr, size_t size);
void  nvInternalFree(void *ptr);
char *nvInternalStrDup(const char *str);
NvBool nvGetRegkeyValue(const NVDevEvoRec *pDevEvo,
                        const char *key, NvU32 *val);

#if defined(DEBUG)

void nvReportUnfreedAllocations(void);

void *nvDebugAlloc(size_t size, int line, const char *file);
void *nvDebugCalloc(size_t nmemb, size_t size, int line, const char *file);
void *nvDebugRealloc(void *ptr, size_t size, int line, const char *file);
void  nvDebugFree(void *ptr);
char *nvDebugStrDup(const char *str, int line, const char *file);

#define nvAlloc(s)      nvDebugAlloc((s), __LINE__, __FILE__)
#define nvCalloc(n,s)   nvDebugCalloc((n), (s), __LINE__, __FILE__)
#define nvFree(p)       nvDebugFree(p)
#define nvRealloc(p,s)  nvDebugRealloc((p), (s), __LINE__, __FILE__)
#define nvStrDup(s)     nvDebugStrDup((s), __LINE__, __FILE__)

#else

#define nvAlloc(s) nvInternalAlloc((s), FALSE)
#define nvCalloc(n,s) nvInternalAlloc((n)*(s), TRUE)
#define nvRealloc(p,s) nvInternalRealloc((p),(s))
#define nvFree(s) nvInternalFree(s)
#define nvStrDup(s) nvInternalStrDup(s)

#endif

void nvVEvoLog(NVEvoLogType logType, NvU8 gpuLogIndex,
               const char *fmt, va_list ap);

void nvEvoLogDev(const NVDevEvoRec *pDevEvo, NVEvoLogType logType,
                 const char *fmt, ...)
    __attribute__((format (printf, 3, 4)));

void nvEvoLogDisp(const NVDispEvoRec *pDispEvo, NVEvoLogType logType,
                  const char *fmt, ...)
    __attribute__((format (printf, 3, 4)));

void nvEvoLog(NVEvoLogType logType, const char *fmt, ...)
    __attribute__((format (printf, 2, 3)));



#if defined(DEBUG)

void nvEvoLogDebug(NVEvoLogType logType, const char *fmt, ...)
    __attribute__((format (printf, 2, 3)));

void nvEvoLogDevDebug(const NVDevEvoRec *pDevEvo, NVEvoLogType logType,
                      const char *fmt, ...)
    __attribute__((format (printf, 3, 4)));

void nvEvoLogDispDebug(const NVDispEvoRec *pDispEvo, NVEvoLogType logType,
                       const char *fmt, ...)
    __attribute__((format (printf, 3, 4)));

#else

#  define nvEvoLogDebug(...)
#  define nvEvoLogDevDebug(pDevEvo, ...)
#  define nvEvoLogDispDebug(pDispEvo, ...)

#endif  /* DEBUG */

void nvInitInfoString(NVEvoInfoStringPtr pInfoString,
                      char *s, NvU16 totalLength);

void nvEvoLogInfoStringRaw(NVEvoInfoStringPtr pInfoString,
                           const char *format, ...)
    __attribute__((format (printf, 2, 3)));
void nvEvoLogInfoString(NVEvoInfoStringPtr pInfoString,
                        const char *format, ...)
    __attribute__((format (printf, 2, 3)));


typedef NvU32 NvKmsGenericHandle;

NvBool nvEvoApiHandlePointerIsPresent(NVEvoApiHandlesPtr pEvoApiHandles,
                                      void *pointer);
NvKmsGenericHandle nvEvoCreateApiHandle(NVEvoApiHandlesPtr pEvoApiHandles,
                                        void *pointer);
void *nvEvoGetPointerFromApiHandle(const NVEvoApiHandlesRec *pEvoApiHandles,
                                   NvKmsGenericHandle handle);
void *nvEvoGetPointerFromApiHandleNext(const NVEvoApiHandlesRec *pEvoApiHandles,
                                       NvKmsGenericHandle *pHandle);
void nvEvoDestroyApiHandle(NVEvoApiHandlesPtr pEvoApiHandles,
                           NvKmsGenericHandle handle);
NvBool nvEvoInitApiHandles(NVEvoApiHandlesPtr pEvoApiHandles,
                           NvU32 defaultSize);
void nvEvoDestroyApiHandles(NVEvoApiHandlesPtr pEvoApiHandles);

#define FOR_ALL_POINTERS_IN_EVO_API_HANDLES(_pEvoApiHandles,            \
                                            _pointer, _handle)          \
    for ((_handle) = 0,                                                 \
         (_pointer) = nvEvoGetPointerFromApiHandleNext(_pEvoApiHandles, \
                                                       &(_handle));     \
         (_pointer) != NULL;                                            \
         (_pointer) = nvEvoGetPointerFromApiHandleNext(_pEvoApiHandles, \
                                                       &(_handle)))



#ifdef __cplusplus
};
#endif

#endif /* __NVKMS_UTILS_H__ */
