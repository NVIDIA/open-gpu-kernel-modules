/*******************************************************************************
    Copyright (c) 2021-2022 NVIDIA Corporation

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

        The above copyright notice and this permission notice shall be
        included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

*******************************************************************************/

#include "nvtypes.h"

#ifndef _clcba2_h_
#define _clcba2_h_

#ifdef __cplusplus
extern "C" {
#endif

#define HOPPER_SEC2_WORK_LAUNCH_A                                               (0x0000CBA2)

#define NVCBA2_DECRYPT_COPY_SRC_ADDR_HI                                         (0x00000400)
#define NVCBA2_DECRYPT_COPY_SRC_ADDR_HI_DATA                                    24:0
#define NVCBA2_DECRYPT_COPY_SRC_ADDR_LO                                         (0x00000404)
#define NVCBA2_DECRYPT_COPY_SRC_ADDR_LO_DATA                                    31:4
#define NVCBA2_DECRYPT_COPY_DST_ADDR_HI                                         (0x00000408)
#define NVCBA2_DECRYPT_COPY_DST_ADDR_HI_DATA                                    24:0
#define NVCBA2_DECRYPT_COPY_DST_ADDR_LO                                         (0x0000040c)
#define NVCBA2_DECRYPT_COPY_DST_ADDR_LO_DATA                                    31:4
#define NVCBA2_DECRYPT_COPY_SIZE                                                (0x00000410)
#define NVCBA2_DECRYPT_COPY_SIZE_DATA                                           31:2
#define NVCBA2_DECRYPT_COPY_AUTH_TAG_ADDR_HI                                    (0x00000414)
#define NVCBA2_DECRYPT_COPY_AUTH_TAG_ADDR_HI_DATA                               24:0
#define NVCBA2_DECRYPT_COPY_AUTH_TAG_ADDR_LO                                    (0x00000418)
#define NVCBA2_DECRYPT_COPY_AUTH_TAG_ADDR_LO_DATA                               31:4
#define NVCBA2_METHOD_STREAM_AUTH_TAG_ADDR_HI                                   (0x0000041C)
#define NVCBA2_METHOD_STREAM_AUTH_TAG_ADDR_HI_DATA                              24:0
#define NVCBA2_METHOD_STREAM_AUTH_TAG_ADDR_LO                                   (0x00000420)
#define NVCBA2_METHOD_STREAM_AUTH_TAG_ADDR_LO_DATA                              31:4
#define NVCBA2_SEMAPHORE_A                                                      (0x00000440)
#define NVCBA2_SEMAPHORE_A_UPPER                                                24:0
#define NVCBA2_SEMAPHORE_B                                                      (0x00000444)
#define NVCBA2_SEMAPHORE_B_LOWER                                                31:2
#define NVCBA2_SET_SEMAPHORE_PAYLOAD_LOWER                                      (0x00000448)
#define NVCBA2_SET_SEMAPHORE_PAYLOAD_LOWER_DATA                                 31:0
#define NVCBA2_SET_SEMAPHORE_PAYLOAD_UPPER                                      (0x0000044C)
#define NVCBA2_SET_SEMAPHORE_PAYLOAD_UPPER_DATA                                 31:0
#define NVCBA2_SEMAPHORE_D                                                      (0x00000450)
#define NVCBA2_SEMAPHORE_D_NOTIFY_INTR                                          0:0
#define NVCBA2_SEMAPHORE_D_NOTIFY_INTR_DISABLE                                  (0x00000000)
#define NVCBA2_SEMAPHORE_D_NOTIFY_INTR_ENABLE                                   (0x00000001)
#define NVCBA2_SEMAPHORE_D_PAYLOAD_SIZE                                         1:1
#define NVCBA2_SEMAPHORE_D_PAYLOAD_SIZE_32_BIT                                  (0x00000000)
#define NVCBA2_SEMAPHORE_D_PAYLOAD_SIZE_64_BIT                                  (0x00000001)
#define NVCBA2_SEMAPHORE_D_TIMESTAMP                                            2:2
#define NVCBA2_SEMAPHORE_D_TIMESTAMP_DISABLE                                    (0x00000000)
#define NVCBA2_SEMAPHORE_D_TIMESTAMP_ENABLE                                     (0x00000001)
#define NVCBA2_SEMAPHORE_D_FLUSH_DISABLE                                        3:3
#define NVCBA2_SEMAPHORE_D_FLUSH_DISABLE_FALSE                                  (0x00000000)
#define NVCBA2_SEMAPHORE_D_FLUSH_DISABLE_TRUE                                   (0x00000001)
#define NVCBA2_EXECUTE                                                          (0x00000470)
#define NVCBA2_EXECUTE_NOTIFY                                                   0:0
#define NVCBA2_EXECUTE_NOTIFY_DISABLE                                           (0x00000000)
#define NVCBA2_EXECUTE_NOTIFY_ENABLE                                            (0x00000001)
#define NVCBA2_EXECUTE_NOTIFY_ON                                                1:1
#define NVCBA2_EXECUTE_NOTIFY_ON_END                                            (0x00000000)
#define NVCBA2_EXECUTE_NOTIFY_ON_BEGIN                                          (0x00000001)
#define NVCBA2_EXECUTE_FLUSH_DISABLE                                            2:2
#define NVCBA2_EXECUTE_FLUSH_DISABLE_FALSE                                      (0x00000000)
#define NVCBA2_EXECUTE_FLUSH_DISABLE_TRUE                                       (0x00000001)
#define NVCBA2_EXECUTE_NOTIFY_INTR                                              3:3
#define NVCBA2_EXECUTE_NOTIFY_INTR_DISABLE                                      (0x00000000)
#define NVCBA2_EXECUTE_NOTIFY_INTR_ENABLE                                       (0x00000001)
#define NVCBA2_EXECUTE_PAYLOAD_SIZE                                             4:4
#define NVCBA2_EXECUTE_PAYLOAD_SIZE_32_BIT                                      (0x00000000)
#define NVCBA2_EXECUTE_PAYLOAD_SIZE_64_BIT                                      (0x00000001)
#define NVCBA2_EXECUTE_TIMESTAMP                                                5:5
#define NVCBA2_EXECUTE_TIMESTAMP_DISABLE                                        (0x00000000)
#define NVCBA2_EXECUTE_TIMESTAMP_ENABLE                                         (0x00000001)

#ifdef __cplusplus
};     /* extern "C" */
#endif
#endif // _clcba2_h
