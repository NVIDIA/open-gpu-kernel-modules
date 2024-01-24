/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

/**
 * Provides memory copy, fill, and comparison.
 **/

#ifndef MEMLIB_H
#define MEMLIB_H

#include "hal/base.h"

/**
 * Copies bytes from a source buffer to a destination buffer.
 *
 * This function copies src_len bytes from src_buf to dst_buf. The following properties are required
 * of the caller:
 * - dst_buf and src_buf are non-NULL.
 * - src_len is less than or equal to dst_len.
 * - The source and destination buffers do not overlap.
 *
 * The implementer of this function is free to check that these properties hold and take action,
 * such as assertion handling, if the properties are violated.
 *
 * @param    dst_buf   Destination buffer to copy to.
 * @param    dst_len   Size, in bytes, of the destination buffer.
 * @param    src_buf   Source buffer to copy from.
 * @param    src_len   The number of bytes to copy from the source buffer.
 **/
void libspdm_copy_mem(void *dst_buf, size_t dst_len,
                      const void *src_buf, size_t src_len);

/**
 * Fills a target buffer with a byte value.
 *
 * @param  buffer    The memory to set.
 * @param  length    The number of bytes to set.
 * @param  value     The value with which to fill length bytes of buffer.
 **/
void libspdm_set_mem(void *buffer, size_t length, uint8_t value);

/**
 * Fills a target buffer with zeros.
 *
 * @param  buffer      The pointer to the target buffer to fill with zeros.
 * @param  length      The number of bytes in buffer to fill with zeros.
 **/
void libspdm_zero_mem(void *buffer, size_t length);

/**
 * Compares the contents of two buffers in constant time.
 *
 * For a given length, the time to complete the comparison is always the same regardless of the
 * contents of the two buffers.
 *
 * @param  destination_buffer  A pointer to the destination buffer to compare.
 * @param  source_buffer       A pointer to the source buffer to compare.
 * @param  length              The number of bytes to compare.
 *
 * @return true   The contents of the two buffers are the same.
 * @retval false  The contents of the two buffers are not the same.
 **/
bool libspdm_consttime_is_mem_equal(const void *destination_buffer,
                                    const void *source_buffer, size_t length);

#endif /* MEMLIB_H */
