/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#include "internal/libspdm_requester_lib.h"

#if LIBSPDM_ENABLE_CAPABILITY_ENCAP_CAP

libspdm_return_t libspdm_generate_encap_error_response(const void *spdm_context,
                                                       uint8_t error_code,
                                                       uint8_t error_data,
                                                       size_t *response_size,
                                                       void *response)
{
    spdm_error_response_t *spdm_response;

    LIBSPDM_ASSERT(*response_size >= sizeof(spdm_error_response_t));
    *response_size = sizeof(spdm_error_response_t);
    spdm_response = response;

    spdm_response->header.spdm_version = libspdm_get_connection_version (spdm_context);
    spdm_response->header.request_response_code = SPDM_ERROR;
    spdm_response->header.param1 = error_code;
    spdm_response->header.param2 = error_data;

    return LIBSPDM_STATUS_SUCCESS;
}

libspdm_return_t libspdm_generate_encap_extended_error_response(
    const void *spdm_context, uint8_t error_code, uint8_t error_data,
    size_t extended_error_data_size, const uint8_t *extended_error_data,
    size_t *response_size, void *response)
{
    spdm_error_response_t *spdm_response;
    LIBSPDM_ASSERT(*response_size >=
                   sizeof(spdm_error_response_t) + extended_error_data_size);

    spdm_response = response;
    spdm_response->header.spdm_version = libspdm_get_connection_version (spdm_context);
    spdm_response->header.request_response_code = SPDM_ERROR;
    spdm_response->header.param1 = error_code;
    spdm_response->header.param2 = error_data;
    libspdm_copy_mem(spdm_response + 1, *response_size - (sizeof(spdm_error_response_t)),
                     extended_error_data, extended_error_data_size);
    *response_size =
        sizeof(spdm_error_response_t) + extended_error_data_size;
    return LIBSPDM_STATUS_SUCCESS;
}

#endif /* LIBSPDM_ENABLE_CAPABILITY_ENCAP_CAP */
