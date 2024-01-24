/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#include "internal/libspdm_common_lib.h"

#if LIBSPDM_ENABLE_MSG_LOG
void libspdm_init_msg_log (void *spdm_context, void *msg_buffer, size_t msg_buffer_size)
{
    libspdm_context_t *context;

    LIBSPDM_ASSERT((spdm_context != NULL) && (msg_buffer != NULL));
    LIBSPDM_ASSERT(msg_buffer_size != 0);

    context = spdm_context;
    context->msg_log.buffer = msg_buffer;
    context->msg_log.max_buffer_size = msg_buffer_size;
    context->msg_log.buffer_size = 0;
    context->msg_log.mode = 0;
    context->msg_log.status = 0;
}

void libspdm_set_msg_log_mode (void *spdm_context, uint32_t mode)
{
    libspdm_context_t *context;

    LIBSPDM_ASSERT(spdm_context != NULL);

    context = spdm_context;
    context->msg_log.mode = mode;
}

uint32_t libspdm_get_msg_log_status (void *spdm_context)
{
    libspdm_context_t *context;

    LIBSPDM_ASSERT(spdm_context != NULL);

    context = spdm_context;

    return context->msg_log.status;
}

size_t libspdm_get_msg_log_size (void *spdm_context)
{
    libspdm_context_t *context;

    LIBSPDM_ASSERT(spdm_context != NULL);

    context = spdm_context;

    return context->msg_log.buffer_size;
}

void libspdm_reset_msg_log (void *spdm_context)
{
    libspdm_context_t *context;

    LIBSPDM_ASSERT(spdm_context != NULL);

    context = spdm_context;

    context->msg_log.buffer_size = 0;
    context->msg_log.mode = 0;
    context->msg_log.status = 0;
}

void libspdm_append_msg_log(libspdm_context_t *spdm_context, void *message, size_t message_size)
{
    LIBSPDM_ASSERT((spdm_context != NULL) && (message != NULL));

    if (((spdm_context->msg_log.mode & LIBSPDM_MSG_LOG_MODE_ENABLE) != 0) &&
        ((spdm_context->msg_log.status & LIBSPDM_MSG_LOG_STATUS_BUFFER_FULL) == 0)) {
        if (spdm_context->msg_log.buffer_size + message_size >
            spdm_context->msg_log.max_buffer_size) {
            libspdm_copy_mem((uint8_t *)spdm_context->msg_log.buffer +
                             spdm_context->msg_log.buffer_size,
                             spdm_context->msg_log.max_buffer_size, message,
                             spdm_context->msg_log.max_buffer_size -
                             spdm_context->msg_log.buffer_size);
            spdm_context->msg_log.status |= LIBSPDM_MSG_LOG_STATUS_BUFFER_FULL;
            spdm_context->msg_log.buffer_size = spdm_context->msg_log.max_buffer_size;
        } else {
            libspdm_copy_mem((uint8_t *)spdm_context->msg_log.buffer +
                             spdm_context->msg_log.buffer_size,
                             spdm_context->msg_log.max_buffer_size, message, message_size);
            spdm_context->msg_log.buffer_size += message_size;
        }

        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO, "Message Logging Status = [%x] Buffer Size = [%x] "
                       "Max Buffer Size = [%x]\n", spdm_context->msg_log.status,
                       spdm_context->msg_log.buffer_size, spdm_context->msg_log.max_buffer_size));
        LIBSPDM_INTERNAL_DUMP_HEX(spdm_context->msg_log.buffer, spdm_context->msg_log.buffer_size);
    }
}
#endif /* LIBSPDM_ENABLE_MSG_LOG */
