/*
 * SPDX-FileCopyrightText: Copyright (c) 2014 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvkms-utils.h"
#include "nvkms-types.h"
#include "nv_mode_timings_utils.h"
#include "nv_vasprintf.h"

#include "nv_list.h" /* for nv_container_of() */

void nvVEvoLog(NVEvoLogType logType, NvU8 gpuLogIndex,
               const char *fmt, va_list ap)
{
    char *msg, prefix[10];
    const char *gpuPrefix = "";
    int level;

    switch (logType) {
    default:
    case EVO_LOG_INFO:  level = NVKMS_LOG_LEVEL_INFO; break;
    case EVO_LOG_WARN:  level = NVKMS_LOG_LEVEL_WARN; break;
    case EVO_LOG_ERROR: level = NVKMS_LOG_LEVEL_ERROR; break;
    }

    msg = nv_vasprintf(fmt, ap);
    if (msg == NULL) {
        return;
    }

    if (gpuLogIndex != NV_INVALID_GPU_LOG_INDEX) {
        nvkms_snprintf(prefix, sizeof(prefix), "GPU:%d: ", gpuLogIndex);
        gpuPrefix = prefix;
    }

    nvkms_log(level, gpuPrefix, msg);

    nvFree(msg);
}

void nvEvoLogDev(const NVDevEvoRec *pDevEvo, NVEvoLogType logType,
                 const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    nvVEvoLog(logType, pDevEvo->gpuLogIndex, fmt, ap);
    va_end(ap);
}

void nvEvoLogDisp(const NVDispEvoRec *pDispEvo, NVEvoLogType logType,
                  const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    nvVEvoLog(logType, pDispEvo->gpuLogIndex, fmt, ap);
    va_end(ap);
}

void nvEvoLog(NVEvoLogType logType, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    nvVEvoLog(logType, NV_INVALID_GPU_LOG_INDEX, fmt, ap);
    va_end(ap);
}

#if defined(DEBUG)

void nvEvoLogDebug(NVEvoLogType logType, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    nvVEvoLog(logType, NV_INVALID_GPU_LOG_INDEX, fmt, ap);
    va_end(ap);
}

void nvEvoLogDevDebug(const NVDevEvoRec *pDevEvo, NVEvoLogType logType,
                      const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    nvVEvoLog(logType, pDevEvo->gpuLogIndex, fmt, ap);
    va_end(ap);
}

void nvEvoLogDispDebug(const NVDispEvoRec *pDispEvo, NVEvoLogType logType,
                       const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    nvVEvoLog(logType, pDispEvo->gpuLogIndex, fmt, ap);
    va_end(ap);
}

#endif /* DEBUG */


/*!
 * Initialize the given NVEvoInfoString.
 *
 * Point the infoString at the specified character array.
 */
void nvInitInfoString(NVEvoInfoStringPtr pInfoString,
                      char *s, NvU16 totalLength)
{
    nvkms_memset(pInfoString, 0, sizeof(*pInfoString));
    pInfoString->s = s;
    pInfoString->totalLength = totalLength;
}


/*!
 * Append the text, described by 'format' and 'ap', to the infoString.
 */
static void LogInfoString(NVEvoInfoStringPtr pInfoString,
                          const char *format, va_list ap)
{
    char *s;
    size_t size = pInfoString->totalLength - pInfoString->length;
    int ret;

    if (pInfoString->s == NULL) {
        return;
    }
    if (size <= 1) {
        nvAssert(!"pInfoString too small");
        return;
    }

    s = pInfoString->s + pInfoString->length;

    ret = nvkms_vsnprintf(s, size, format, ap);

    if (ret > 0) {
        pInfoString->length += NV_MIN((size_t)ret, size - 1);
    }

    /*
     * If ret is larger than size, then we may need to increase
     * totalLength to support logging everything that we are trying to
     * log to this buffer.
     */
    nvAssert(ret <= size);

    nvAssert(pInfoString->length < pInfoString->totalLength);
    pInfoString->s[pInfoString->length] = '\0';
}


/*!
 * Append to the infoString, without any additions.
 */
void nvEvoLogInfoStringRaw(NVEvoInfoStringPtr pInfoString,
                           const char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    LogInfoString(pInfoString, format, ap);
    va_end(ap);
}


/*!
 * Append to the infoString, appending a newline.
 */
void nvEvoLogInfoString(NVEvoInfoStringPtr pInfoString,
                        const char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    LogInfoString(pInfoString, format, ap);
    va_end(ap);

    nvEvoLogInfoStringRaw(pInfoString, "\n");
}


/*!
 * The NVEvoApiHandlesRec-related functions below are used to manage
 * sets of NvKms API handles.  For the various NvKms objects (e.g.,
 * devices, disps, connectors, surfaces) clients will specify the
 * object by handle, and NVKMS will look up the corresponding object.
 *
 * We store a pointer to the object in a dynamically allocated array,
 * and use the handle to look up the pointer in the array.
 *
 * Note that handles are 1-based (valid handles are in the range
 * [1,numPointers], and 0 is an invalid handle), while indices to the
 * corresponding pointers are 0-based (valid indices are in the range
 * [0,numPointers-1]).  Subtract 1 from the handle to get the index
 * for the pointer.
 */

/*!
 * Increase the size of the NVEvoApiHandles::pointers array.
 *
 * Reallocate the pointers array, increasing by defaultSize.
 * Initialize the new region of memory.
 */
static NvBool GrowApiHandlesPointersArray(NVEvoApiHandlesPtr pEvoApiHandles)
{
    NvU32 newNumPointers =
        pEvoApiHandles->numPointers + pEvoApiHandles->defaultSize;
    size_t oldSize = pEvoApiHandles->numPointers * sizeof(void *);
    size_t newSize = newNumPointers * sizeof(void *);
    void **newPointers;

    /* Check for wrap in the newNumPointers computation. */
    if (newSize <= oldSize) {
        return FALSE;
    }

    newPointers = nvRealloc(pEvoApiHandles->pointers, newSize);

    if (newPointers == NULL) {
        return FALSE;
    }

    nvkms_memset(&newPointers[pEvoApiHandles->numPointers], 0, newSize - oldSize);

    pEvoApiHandles->pointers = newPointers;
    pEvoApiHandles->numPointers = newNumPointers;

    return TRUE;
}


/*!
 * Attempt to shrink the NVEvoApiHandles::pointers array.
 *
 * If high elements in the array are unused, reduce the array size in
 * multiples of defaultSize.
 */
static void ShrinkApiHandlesPointersArray(NVEvoApiHandlesPtr pEvoApiHandles)
{
    NvU32 index;
    NvU32 newNumPointers;
    void **newPointers;

    /* If the array is already as small as it can be, we are done. */

    if (pEvoApiHandles->numPointers == pEvoApiHandles->defaultSize) {
        return;
    }

    /* Find the highest non-empty element. */

    for (index = pEvoApiHandles->numPointers - 1; index > 0; index--) {
        if (pEvoApiHandles->pointers[index] != NULL) {
            break;
        }
    }

    /*
     * Compute the new array size by rounding index up to the next
     * multiple of defaultSize.
     */
    newNumPointers = ((index / pEvoApiHandles->defaultSize) + 1) *
        pEvoApiHandles->defaultSize;

    /* If the array is already that size, we are done. */

    if (pEvoApiHandles->numPointers == newNumPointers) {
        return;
    }

    newPointers =
        nvRealloc(pEvoApiHandles->pointers, newNumPointers * sizeof(void *));

    if (newPointers != NULL) {
        pEvoApiHandles->pointers = newPointers;
        pEvoApiHandles->numPointers = newNumPointers;
    }
}


/*!
 * Return true if 'pointer' is already present in pEvoApiHandles
 */
NvBool nvEvoApiHandlePointerIsPresent(NVEvoApiHandlesPtr pEvoApiHandles,
                                      void *pointer)
{
    NvU32 index;

    for (index = 0; index < pEvoApiHandles->numPointers; index++) {
        if (pEvoApiHandles->pointers[index] == pointer) {
            return TRUE;
        }
    }

    return FALSE;
}


/*!
 * Create an NvKms API handle.
 *
 * Create an available handle from pEvoApiHandles, and associate
 * 'pointer' with the handle.
 */
NvKmsGenericHandle
nvEvoCreateApiHandle(NVEvoApiHandlesPtr pEvoApiHandles, void *pointer)
{
    NvU32 index;

    if (pointer == NULL) {
        return 0;
    }

    for (index = 0; index < pEvoApiHandles->numPointers; index++) {
        if (pEvoApiHandles->pointers[index] == NULL) {
            goto availableIndex;
        }
    }

    /*
     * Otherwise, there are no free elements in the pointers array:
     * grow the array and try again.
     */
    if (!GrowApiHandlesPointersArray(pEvoApiHandles)) {
        return 0;
    }

    /* fall through */

availableIndex:

    nvAssert(index < pEvoApiHandles->numPointers);
    nvAssert(pEvoApiHandles->pointers[index] == NULL);

    pEvoApiHandles->pointers[index] = pointer;

    return index + 1;
}


/*!
 * Retrieve a pointer that maps to an NvKms API handle.
 *
 * Return the pointer that nvEvoCreateApiHandle() associated with 'handle'.
 */
void *nvEvoGetPointerFromApiHandle(const NVEvoApiHandlesRec *pEvoApiHandles,
                                   NvKmsGenericHandle handle)
{
    NvU32 index;

    if (handle == 0) {
        return NULL;
    }

    index = handle - 1;

    if (index >= pEvoApiHandles->numPointers) {
        return NULL;
    }

    return pEvoApiHandles->pointers[index];
}


/*!
 * Retrieve a pointer that maps to the next NvKms API handle.
 *
 * This is intended to be used by the
 * FOR_ALL_POINTERS_IN_EVO_API_HANDLES() macro.  On the first
 * iteration, *pHandle == 0, and this will return the first pointer it
 * finds in the pointer array.  The returned *pHandle will be the
 * location to begin searching on the next iteration, and so on.
 *
 * Once there are no more non-zero elements in the pointer array,
 * return NULL.
 */
void *nvEvoGetPointerFromApiHandleNext(const NVEvoApiHandlesRec *pEvoApiHandles,
                                       NvKmsGenericHandle *pHandle)
{
    NvU32 index = *pHandle;

    for (; index < pEvoApiHandles->numPointers; index++) {
        if (pEvoApiHandles->pointers[index] != NULL) {
            *pHandle = index + 1;
            return pEvoApiHandles->pointers[index];
        }
    }

    return NULL;
}


/*!
 * Remove an NvKms API handle.
 *
 * Clear the 'handle' entry, and its corresponding pointer, from pEvoApiHandles.
 */
void nvEvoDestroyApiHandle(NVEvoApiHandlesPtr pEvoApiHandles,
                           NvKmsGenericHandle handle)
{
    NvU32 index;

    if (handle == 0) {
        return;
    }

    index = handle - 1;

    if (index >= pEvoApiHandles->numPointers) {
        return;
    }

    pEvoApiHandles->pointers[index] = NULL;

    ShrinkApiHandlesPointersArray(pEvoApiHandles);
}


/* Only used in nvAssert, so only build into debug builds to avoid never-used
 * warnings */
#if defined(DEBUG)
/*!
 * Return the number of non-NULL pointers in the pointer array.
 */
static NvU32
CountApiHandles(const NVEvoApiHandlesRec *pEvoApiHandles)
{
    NvU32 index, count = 0;

    for (index = 0; index < pEvoApiHandles->numPointers; index++) {
        if (pEvoApiHandles->pointers[index] != NULL) {
            count++;
        }
    }

    return count;
}
#endif /* DEBUG */


/*!
 * Initialize the NVEvoApiHandlesRec.
 *
 * This should be called before any
 * nvEvo{Create,GetPointerFrom,Destroy}ApiHandle() calls on this
 * pEvoApiHandles.
 *
 * The pointer array for the pEvoApiHandles will be managed in
 * multiples of 'defaultSize'.
 */
NvBool nvEvoInitApiHandles(NVEvoApiHandlesPtr pEvoApiHandles, NvU32 defaultSize)
{
    nvkms_memset(pEvoApiHandles, 0, sizeof(*pEvoApiHandles));

    pEvoApiHandles->defaultSize = defaultSize;

    return GrowApiHandlesPointersArray(pEvoApiHandles);
}


/*!
 * Free the NVEvoApiHandlesPtr resources.
 */
void nvEvoDestroyApiHandles(NVEvoApiHandlesPtr pEvoApiHandles)
{
    nvAssert(CountApiHandles(pEvoApiHandles) == 0);

    nvFree(pEvoApiHandles->pointers);

    nvkms_memset(pEvoApiHandles, 0, sizeof(*pEvoApiHandles));
}

NvU8 nvPixelDepthToBitsPerComponent(enum nvKmsPixelDepth pixelDepth)
{
    /*
     * Note: The 444 formats have three components per pixel, thus we compute
     * bpc as depth/3. The 422 formats effectively store two components per
     * pixel, so we compute bpc for those as depth/2.
     */
    switch (pixelDepth) {
    case NVKMS_PIXEL_DEPTH_18_444:
        return 6;
    case NVKMS_PIXEL_DEPTH_24_444:
    case NVKMS_PIXEL_DEPTH_16_422:
        return 8;
    case NVKMS_PIXEL_DEPTH_30_444:
    case NVKMS_PIXEL_DEPTH_20_422:
        return 10;
    }
    nvAssert(!"Unknown NVKMS_PIXEL_DEPTH");
    return 0;
}

/* Import function required by nvBuildModeName() */

int nvBuildModeNameSnprintf(char *str, size_t size, const char *format, ...)
{
    va_list ap;
    int ret;

    va_start(ap, format);
    ret = nvkms_vsnprintf(str, size, format, ap);
    va_end(ap);

    return ret;
}

/* Import functions required by nv_vasprintf() */

void *nv_vasprintf_alloc(size_t size)
{
    return nvAlloc(size);
}

void nv_vasprintf_free(void *ptr)
{
    nvFree(ptr);
}

int nv_vasprintf_vsnprintf(char *str, size_t size,
                           const char *format, va_list ap)
{
    return nvkms_vsnprintf(str, size, format, ap);
}

/*
 * Track the size of each allocation, so that it can be passed to
 * nvkms_free().
 */
typedef struct {
    size_t size; /* includes sizeof(nvkms_memory_info_t) */
    char data[] __attribute__((aligned(8)));
} nvkms_memory_info_t;

void *nvInternalAlloc(size_t size, const NvBool zero)
{
    size_t totalSize = size + sizeof(nvkms_memory_info_t);
    nvkms_memory_info_t *p;

    if (totalSize < size) { /* overflow in the above addition */
        return NULL;
    }

    p = nvkms_alloc(totalSize, zero);

    if (p == NULL) {
        return NULL;
    }

    p->size = totalSize;

    return p->data;
}

void *nvInternalRealloc(void *ptr, size_t size)
{
    nvkms_memory_info_t *p = NULL;
    void *newptr;

    if (ptr == NULL) {
        /* realloc with a ptr of NULL is equivalent to alloc. */
        return nvInternalAlloc(size, FALSE);
    }

    if (size == 0) {
        /* realloc with a size of 0 is equivalent to free. */
        nvInternalFree(ptr);
        return NULL;
    }

    p = nv_container_of(ptr, nvkms_memory_info_t, data);

    newptr = nvInternalAlloc(size, FALSE);

    if (newptr != NULL) {
        size_t oldsize = p->size - sizeof(nvkms_memory_info_t);
        size_t copysize = (size < oldsize) ? size : oldsize;
        nvkms_memcpy(newptr, ptr, copysize);
        nvInternalFree(ptr);
    }

    return newptr;
}

void nvInternalFree(void *ptr)
{
    nvkms_memory_info_t *p;

    if (ptr == NULL) {
        return;
    }

    p = nv_container_of(ptr, nvkms_memory_info_t, data);

    nvkms_free(p, p->size);
}

char *nvInternalStrDup(const char *str)
{
    size_t len;
    char *newstr;

    if (str == NULL) {
        return NULL;
    }

    len = nvkms_strlen(str) + 1;

    newstr = nvInternalAlloc(len, FALSE);

    if (newstr == NULL) {
        return NULL;
    }

    nvkms_memcpy(newstr, str, len);

    return newstr;
}

/*!
 * Look up the value of a key in the set of registry keys provided at device
 * allocation time, copied from the client request during nvAllocDevEvo().
 *
 * \param[in]   pDevEvo  The device with regkeys to be checked.
 *
 * \param[in]   key      The name of the key to look up.
 *
 * \param[out]  val      The value of the key, if the key was specified.
 *
 * \return      Whether the key was specified in the registry.
 */
NvBool nvGetRegkeyValue(const NVDevEvoRec *pDevEvo,
                        const char *key, NvU32 *val)
{
    int i;

    for (i = 0; i < ARRAY_LEN(pDevEvo->registryKeys); i++) {
        if (nvkms_strcmp(key, pDevEvo->registryKeys[i].name) == 0) {
            *val = pDevEvo->registryKeys[i].value;
            return TRUE;
        }
    }

    return FALSE;
}

#if defined(DEBUG)

#include "nv_memory_tracker.h"

void *nvDebugAlloc(size_t size, int line, const char *file)
{
    return nvMemoryTrackerTrackedAlloc(&nvEvoGlobal.debugMemoryAllocationList,
                                       size, line, file);
}

void *nvDebugCalloc(size_t nmemb, size_t size, int line, const char *file)
{
    return nvMemoryTrackerTrackedCalloc(&nvEvoGlobal.debugMemoryAllocationList,
                                        nmemb, size, line, file);
}

void *nvDebugRealloc(void *ptr, size_t size, int line, const char *file)
{
    return nvMemoryTrackerTrackedRealloc(&nvEvoGlobal.debugMemoryAllocationList,
                                         ptr, size, line, file);
}

void nvDebugFree(void *ptr)
{
    nvMemoryTrackerTrackedFree(ptr);
}

char *nvDebugStrDup(const char *str, int line, const char *file)
{
    size_t size = nvkms_strlen(str);
    char *newStr = nvDebugAlloc(size + 1, line, file);

    if (newStr == NULL) {
        return NULL;
    }

    nvkms_memcpy(newStr, str, size);
    newStr[size] = '\0';

    return newStr;
}

void nvReportUnfreedAllocations(void)
{
    nvMemoryTrackerPrintUnfreedAllocations(
        &nvEvoGlobal.debugMemoryAllocationList);
}

void nvMemoryTrackerPrintf(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    nvVEvoLog(EVO_LOG_WARN, NV_INVALID_GPU_LOG_INDEX, format, ap);
    va_end(ap);
}

void *nvMemoryTrackerAlloc(size_t size)
{
    return nvkms_alloc(size, FALSE);
}

void nvMemoryTrackerFree(void *ptr, size_t size)
{
    nvkms_free(ptr, size);
}

void nvMemoryTrackerMemset(void *s, int c, size_t n)
{
    nvkms_memset(s, c, n);
}

void nvMemoryTrackerMemcpy(void *dest, const void *src, size_t n)
{
    nvkms_memcpy(dest, src, n);
}

#endif /* DEBUG */

/*
 * The C++ displayPort library source code introduces a reference to
 * __cxa_pure_virtual.  This should never actually get called, so
 * simply assert.
 */
void __cxa_pure_virtual(void);

void __cxa_pure_virtual(void)
{
    nvAssert(!"Pure virtual function called");
}

/* Import functions required by unix_rm_handle */

#if defined(DEBUG)

void nvUnixRmHandleDebugAssert(const char *expString,
                               const char *filenameString,
                               const char *funcString,
                               const unsigned lineNumber)
{
    nvDebugAssert(expString, filenameString, funcString, lineNumber);
}

void nvUnixRmHandleLogMsg(NvU32 level, const char *fmt, ...)
{

    va_list ap;
    va_start(ap, fmt);

    /* skip verbose messages */
    if (level < NV_UNIX_RM_HANDLE_DEBUG_VERBOSE) {
        nvVEvoLog(EVO_LOG_WARN, NV_INVALID_GPU_LOG_INDEX, fmt, ap);
    }

    va_end(ap);
}

#endif /* DEBUG */

void *nvUnixRmHandleReallocMem(void *oldPtr, NvLength newSize)
{
    return nvRealloc(oldPtr, newSize);
}

void nvUnixRmHandleFreeMem(void *ptr)
{
    nvFree(ptr);
}

/* Import functions required by nv_assert */

#if defined(DEBUG)

void nvDebugAssert(const char *expString, const char *filenameString,
                   const char *funcString, const unsigned int lineNumber)
{
    nvEvoLog(EVO_LOG_WARN, "NVKMS Assert @%s:%d:%s(): '%s'",
             filenameString, lineNumber, funcString, expString);
}

#endif /* DEBUG */
