/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

/** @file
 * libspdm_const_compare_mem() implementation.
 **/

#include "base.h"

/**
 * Compares the contents of two buffers in const time.
 *
 * This function compares length bytes of source_buffer to length bytes of destination_buffer.
 * If all length bytes of the two buffers are identical, then 0 is returned.  Otherwise, the
 * value returned is the first mismatched byte in source_buffer subtracted from the first
 * mismatched byte in destination_buffer.
 *
 * If length > 0 and destination_buffer is NULL, then LIBSPDM_ASSERT().
 * If length > 0 and source_buffer is NULL, then LIBSPDM_ASSERT().
 * If length is greater than (MAX_ADDRESS - destination_buffer + 1), then LIBSPDM_ASSERT().
 * If length is greater than (MAX_ADDRESS - source_buffer + 1), then LIBSPDM_ASSERT().
 *
 * @param  destination_buffer A pointer to the destination buffer to compare.
 * @param  source_buffer      A pointer to the source buffer to compare.
 * @param  length            The number of bytes to compare.
 *
 * @return 0                 All length bytes of the two buffers are identical.
 * @retval Non-zero          There is mismatched between source_buffer and destination_buffer.
 *
 **/
int32_t libspdm_const_compare_mem(const void *destination_buffer,
                                  const void *source_buffer, size_t length)
{
    const volatile uint8_t *pointer_dst;
    const volatile uint8_t *pointer_src;
    uint8_t delta;

    pointer_dst = (const uint8_t *)destination_buffer;
    pointer_src = (const uint8_t *)source_buffer;
    delta = 0;
    while ((length-- != 0)) {
        delta |= *(pointer_dst++) ^ *(pointer_src++);
    }

    return delta;
}
