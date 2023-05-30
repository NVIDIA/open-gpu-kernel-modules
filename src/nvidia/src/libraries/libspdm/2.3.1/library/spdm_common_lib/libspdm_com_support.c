/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#include "internal/libspdm_common_lib.h"

#if LIBSPDM_DEBUG_PRINT_ENABLE
void libspdm_internal_dump_hex_str(const uint8_t *data, size_t size)
{
    size_t index;
    for (index = 0; index < size; index++) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "%02x", (size_t)data[index]));
    }
}

void libspdm_internal_dump_data(const uint8_t *data, size_t size)
{
    size_t index;
    for (index = 0; index < size; index++) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "%02x ", (size_t)data[index]));
    }
}

void libspdm_internal_dump_hex(const uint8_t *data, size_t size)
{
    size_t index;
    size_t count;
    size_t left;

    #define COLUMN_SIZE (16 * 2)

    count = size / COLUMN_SIZE;
    left = size % COLUMN_SIZE;
    for (index = 0; index < count; index++) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "%04x: ", index * COLUMN_SIZE));
        LIBSPDM_INTERNAL_DUMP_DATA(data + index * COLUMN_SIZE, COLUMN_SIZE);
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "\n"));
    }

    if (left != 0) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "%04x: ", index * COLUMN_SIZE));
        LIBSPDM_INTERNAL_DUMP_DATA(data + index * COLUMN_SIZE, left);
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "\n"));
    }
}
#endif /* LIBSPDM_DEBUG_PRINT_ENABLE */

/**
 * Reads a 24-bit value from memory that may be unaligned.
 *
 * @param  buffer  The pointer to a 24-bit value that may be unaligned.
 *
 * @return The 24-bit value read from buffer.
 **/
uint32_t libspdm_read_uint24(const uint8_t *buffer)
{
    return (uint32_t)(buffer[0] | buffer[1] << 8 | buffer[2] << 16);
}

/**
 * Writes a 24-bit value to memory that may be unaligned.
 *
 * @param  buffer  The pointer to a 24-bit value that may be unaligned.
 * @param  value   24-bit value to write to buffer.
 **/
void libspdm_write_uint24(uint8_t *buffer, uint32_t value)
{
    buffer[0] = (uint8_t)(value & 0xFF);
    buffer[1] = (uint8_t)((value >> 8) & 0xFF);
    buffer[2] = (uint8_t)((value >> 16) & 0xFF);
}

/**
 * Reads a 16-bit value from memory that may be unaligned.
 *
 * @param  buffer  The pointer to a 16-bit value that may be unaligned.
 *
 * @return The 16-bit value read from buffer.
 **/
uint16_t libspdm_read_uint16(const uint8_t *buffer)
{
    return (uint16_t)(buffer[0] | buffer[1] << 8);
}

/**
 * Writes a 16-bit value to memory that may be unaligned.
 *
 * @param  buffer  The pointer to a 16-bit value that may be unaligned.
 * @param  value   16-bit value to write to buffer.
 **/
void libspdm_write_uint16(uint8_t *buffer, uint16_t value)
{
    buffer[0] = (uint8_t)(value & 0xFF);
    buffer[1] = (uint8_t)((value >> 8) & 0xFF);
}

/**
 * Reads a 32-bit value from memory that may be unaligned.
 *
 * @param  buffer  The pointer to a 32-bit value that may be unaligned.
 *
 * @return The 32-bit value read from buffer.
 **/
uint32_t libspdm_read_uint32(const uint8_t *buffer)
{
    return (uint32_t)(buffer[0] | buffer[1] << 8 | buffer[2] << 16 | buffer[3] << 24);
}

/**
 * Writes a 32-bit value to memory that may be unaligned.
 *
 * @param  buffer  The pointer to a 32-bit value that may be unaligned.
 * @param  value   32-bit value to write to buffer.
 **/
void libspdm_write_uint32(uint8_t *buffer, uint32_t value)
{
    buffer[0] = (uint8_t)(value & 0xFF);
    buffer[1] = (uint8_t)((value >> 8) & 0xFF);
    buffer[2] = (uint8_t)((value >> 16) & 0xFF);
    buffer[3] = (uint8_t)((value >> 24) & 0xFF);
}

/**
 * Reads a 64-bit value from memory that may be unaligned.
 *
 * @param  buffer  The pointer to a 64-bit value that may be unaligned.
 *
 * @return The 64-bit value read from buffer.
 **/
uint64_t libspdm_read_uint64(const uint8_t *buffer)
{
    return (uint64_t)(buffer[0]) |
           ((uint64_t)(buffer[1]) << 8) |
           ((uint64_t)(buffer[2]) << 16) |
           ((uint64_t)(buffer[3]) << 24) |
           ((uint64_t)(buffer[4]) << 32) |
           ((uint64_t)(buffer[5]) << 40) |
           ((uint64_t)(buffer[6]) << 48) |
           ((uint64_t)(buffer[7]) << 56);
}

/**
 * Writes a 64-bit value to memory that may be unaligned.
 *
 * @param  buffer  The pointer to a 64-bit value that may be unaligned.
 * @param  value   64-bit value to write to buffer.
 **/
void libspdm_write_uint64(uint8_t *buffer, uint64_t value)
{
    buffer[0] = (uint8_t)(value & 0xFF);
    buffer[1] = (uint8_t)((value >> 8) & 0xFF);
    buffer[2] = (uint8_t)((value >> 16) & 0xFF);
    buffer[3] = (uint8_t)((value >> 24) & 0xFF);
    buffer[4] = (uint8_t)((value >> 32) & 0xFF);
    buffer[5] = (uint8_t)((value >> 40) & 0xFF);
    buffer[6] = (uint8_t)((value >> 48) & 0xFF);
    buffer[7] = (uint8_t)((value >> 56) & 0xFF);
}

libspdm_return_t libspdm_append_managed_buffer(void *m_buffer, const void *buffer,
                                               size_t buffer_size)
{
    libspdm_managed_buffer_t *managed_buffer;

    LIBSPDM_ASSERT(buffer != NULL);

    if (buffer_size == 0) {
        return LIBSPDM_STATUS_SUCCESS;
    }

    managed_buffer = m_buffer;

    LIBSPDM_ASSERT(buffer_size != 0);
    LIBSPDM_ASSERT(managed_buffer->max_buffer_size >= managed_buffer->buffer_size);

    if (buffer_size > managed_buffer->max_buffer_size - managed_buffer->buffer_size) {
        /* Do not LIBSPDM_ASSERT here, because command processor will append message from external.*/
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_ERROR,
                       "libspdm_append_managed_buffer 0x%x fail, rest 0x%x only\n",
                       (uint32_t)buffer_size,
                       (uint32_t)(managed_buffer->max_buffer_size - managed_buffer->buffer_size)));
        return LIBSPDM_STATUS_BUFFER_FULL;
    }
    LIBSPDM_ASSERT(buffer_size <= managed_buffer->max_buffer_size - managed_buffer->buffer_size);

    libspdm_copy_mem((uint8_t *)(managed_buffer + 1) + managed_buffer->buffer_size,
                     buffer_size, buffer, buffer_size);
    managed_buffer->buffer_size += buffer_size;

    return LIBSPDM_STATUS_SUCCESS;
}

void libspdm_reset_managed_buffer(void *m_buffer)
{
    libspdm_managed_buffer_t *managed_buffer;

    managed_buffer = m_buffer;
    managed_buffer->buffer_size = 0;

    libspdm_zero_mem(managed_buffer + 1, managed_buffer->max_buffer_size);
}

size_t libspdm_get_managed_buffer_size(void *m_buffer)
{
    libspdm_managed_buffer_t *managed_buffer;

    managed_buffer = m_buffer;

    return managed_buffer->buffer_size;
}

void *libspdm_get_managed_buffer(void *m_buffer)
{
    libspdm_managed_buffer_t *managed_buffer;

    managed_buffer = m_buffer;

    return (managed_buffer + 1);
}

void libspdm_init_managed_buffer(void *m_buffer, size_t max_buffer_size)
{
    libspdm_managed_buffer_t *managed_buffer;

    managed_buffer = m_buffer;
    managed_buffer->max_buffer_size = max_buffer_size;

    libspdm_reset_managed_buffer(m_buffer);
}
