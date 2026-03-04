/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#include "internal/libspdm_requester_lib.h"

uint16_t libspdm_allocate_req_session_id(libspdm_context_t *spdm_context, bool use_psk)
{
    uint16_t req_session_id;
    libspdm_session_info_t *session_info;
    size_t index;

    if (use_psk) {
        if ((spdm_context->max_psk_session_count != 0) &&
            (spdm_context->current_psk_session_count >= spdm_context->max_psk_session_count)) {
            LIBSPDM_DEBUG((LIBSPDM_DEBUG_ERROR,
                           "libspdm_allocate_req_session_id - MAX PSK session\n"));
            return (INVALID_SESSION_ID & 0xFFFF);
        }
    } else {
        if ((spdm_context->max_dhe_session_count != 0) &&
            (spdm_context->current_dhe_session_count >= spdm_context->max_dhe_session_count)) {
            LIBSPDM_DEBUG((LIBSPDM_DEBUG_ERROR,
                           "libspdm_allocate_req_session_id - MAX DHE session\n"));
            return (INVALID_SESSION_ID & 0xFFFF);
        }
    }

    session_info = spdm_context->session_info;
    for (index = 0; index < LIBSPDM_MAX_SESSION_COUNT; index++) {
        if ((session_info[index].session_id & 0xFFFF) == (INVALID_SESSION_ID & 0xFFFF)) {
            req_session_id = (uint16_t)(0xFFFF - index);
            return req_session_id;
        }
    }

    LIBSPDM_DEBUG((LIBSPDM_DEBUG_ERROR, "libspdm_allocate_req_session_id - MAX session_id\n"));
    return (INVALID_SESSION_ID & 0xFFFF);
}

void libspdm_build_opaque_data_supported_version_data(libspdm_context_t *spdm_context,
                                                      size_t *data_out_size,
                                                      void *data_out)
{
    size_t final_data_size;
    secured_message_general_opaque_data_table_header_t *general_opaque_data_table_header;
    spdm_general_opaque_data_table_header_t *spdm_general_opaque_data_table_header;
    secured_message_opaque_element_table_header_t *opaque_element_table_header;
    secured_message_opaque_element_supported_version_t *opaque_element_support_version;
    spdm_version_number_t *versions_list;
    void *end;

    if (spdm_context->local_context.secured_message_version.spdm_version_count == 0) {
        *data_out_size = 0;
        return;
    }

    final_data_size = libspdm_get_opaque_data_supported_version_data_size(spdm_context);
    LIBSPDM_ASSERT(*data_out_size >= final_data_size);

    if (libspdm_get_connection_version (spdm_context) >= SPDM_MESSAGE_VERSION_12) {
        spdm_general_opaque_data_table_header = data_out;
        spdm_general_opaque_data_table_header->total_elements = 1;
        libspdm_write_uint24(spdm_general_opaque_data_table_header->reserved, 0);
        opaque_element_table_header = (void *)(spdm_general_opaque_data_table_header + 1);
    } else {
        general_opaque_data_table_header = data_out;
        general_opaque_data_table_header->spec_id = SECURED_MESSAGE_OPAQUE_DATA_SPEC_ID;
        general_opaque_data_table_header->opaque_version = SECURED_MESSAGE_OPAQUE_VERSION;
        general_opaque_data_table_header->total_elements = 1;
        general_opaque_data_table_header->reserved = 0;
        opaque_element_table_header = (void *)(general_opaque_data_table_header + 1);
    }

    opaque_element_table_header->id = SPDM_REGISTRY_ID_DMTF;
    opaque_element_table_header->vendor_len = 0;
    opaque_element_table_header->opaque_element_data_len =
        sizeof(secured_message_opaque_element_supported_version_t) +
        sizeof(spdm_version_number_t) *
        spdm_context->local_context.secured_message_version.spdm_version_count;

    opaque_element_support_version = (void *)(opaque_element_table_header + 1);
    opaque_element_support_version->sm_data_version =
        SECURED_MESSAGE_OPAQUE_ELEMENT_SMDATA_DATA_VERSION;
    opaque_element_support_version->sm_data_id =
        SECURED_MESSAGE_OPAQUE_ELEMENT_SMDATA_ID_SUPPORTED_VERSION;
    opaque_element_support_version->version_count =
        spdm_context->local_context.secured_message_version.spdm_version_count;

    versions_list = (void *)(opaque_element_support_version + 1);
    libspdm_copy_mem(versions_list,
                     *data_out_size - ((uint8_t*)versions_list - (uint8_t*)data_out),
                     spdm_context->local_context.secured_message_version.spdm_version,
                     spdm_context->local_context.secured_message_version.spdm_version_count *
                     sizeof(spdm_version_number_t));

    /* Zero Padding. *data_out_size does not need to be changed, because data is 0 padded */
    end = versions_list + spdm_context->local_context.secured_message_version.spdm_version_count;
    libspdm_zero_mem(end, (size_t)data_out + final_data_size - (size_t)end);
}

/**
 * Process opaque data version selection.
 *
 * This function should be called in KEY_EXCHANGE/PSK_EXCHANGE response parsing in requester.
 *
 * @param  data_in_size                   size in bytes of the data_in.
 * @param  data_in                       A pointer to the buffer to store the opaque data version selection.
 *
 * @retval RETURN_SUCCESS               The opaque data version selection is processed successfully.
 * @retval RETURN_UNSUPPORTED           The data_in is NOT opaque data version selection.
 **/
libspdm_return_t libspdm_process_opaque_data_version_selection_data(libspdm_context_t *spdm_context,
                                                                    size_t data_in_size,
                                                                    void *data_in)
{
    const secured_message_opaque_element_table_header_t
    *opaque_element_table_header;
    const secured_message_opaque_element_version_selection_t
    *opaque_element_version_section;

    bool result;
    uint8_t secured_message_version_index;
    const void *get_element_ptr;
    size_t get_element_len;

    result = false;
    get_element_ptr = NULL;

    if (spdm_context->local_context.secured_message_version.spdm_version_count == 0) {
        return LIBSPDM_STATUS_SUCCESS;
    }

    result = libspdm_get_element_from_opaque_data(
        spdm_context, data_in_size,
        data_in, SPDM_REGISTRY_ID_DMTF,
        SECURED_MESSAGE_OPAQUE_ELEMENT_SMDATA_ID_VERSION_SELECTION,
        &get_element_ptr, &get_element_len);
    if ((!result) || (get_element_ptr == NULL)) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO,"get element error!\n"));
        return LIBSPDM_STATUS_INVALID_MSG_FIELD;
    }

    opaque_element_table_header = (const secured_message_opaque_element_table_header_t*)
                                  get_element_ptr;

    /*check for selection vesion data*/
    if ((opaque_element_table_header->vendor_len != 0) ||
        (opaque_element_table_header->opaque_element_data_len !=
         sizeof(secured_message_opaque_element_version_selection_t))) {
        return LIBSPDM_STATUS_INVALID_MSG_FIELD;
    }
    opaque_element_version_section = (const void *)(opaque_element_table_header + 1);

    if ((const uint8_t *)opaque_element_version_section +
        sizeof(secured_message_opaque_element_version_selection_t) >
        (const uint8_t *)opaque_element_table_header + get_element_len) {
        return LIBSPDM_STATUS_INVALID_MSG_FIELD;
    }

    for (secured_message_version_index = 0;
         secured_message_version_index <
         spdm_context->local_context.secured_message_version.spdm_version_count;
         secured_message_version_index++) {
        if (libspdm_get_version_from_version_number(opaque_element_version_section->
                                                    selected_version)
            ==
            libspdm_get_version_from_version_number(
                spdm_context->local_context.secured_message_version.spdm_version[
                    secured_message_version_index])) {
            libspdm_copy_mem(&(spdm_context->connection_info.secured_message_version),
                             sizeof(spdm_context->connection_info.secured_message_version),
                             &(opaque_element_version_section->selected_version),
                             sizeof(spdm_version_number_t));
            return LIBSPDM_STATUS_SUCCESS;
        }
    }

    return LIBSPDM_STATUS_UNSUPPORTED_CAP;
}
