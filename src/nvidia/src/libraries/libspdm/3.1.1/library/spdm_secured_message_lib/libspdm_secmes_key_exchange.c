/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#include "internal/libspdm_secured_message_lib.h"

/**
 * Allocates and Initializes one Diffie-Hellman Ephemeral (DHE) context for subsequent use,
 * based upon negotiated DHE algorithm.
 *
 * @param  dhe_named_group                SPDM dhe_named_group
 * @param  is_initiator                   if the caller is initiator.
 *                                       true: initiator
 *                                       false: not an initiator
 *
 * @return  Pointer to the Diffie-Hellman context that has been initialized.
 **/
void *libspdm_secured_message_dhe_new(spdm_version_number_t spdm_version,
                                      uint16_t dhe_named_group, bool is_initiator)
{
    return libspdm_dhe_new(spdm_version, dhe_named_group, is_initiator);
}

/**
 * Release the specified DHE context,
 * based upon negotiated DHE algorithm.
 *
 * @param  dhe_named_group                SPDM dhe_named_group
 * @param  dhe_context                   Pointer to the DHE context to be released.
 **/
void libspdm_secured_message_dhe_free(uint16_t dhe_named_group, void *dhe_context)
{
    libspdm_dhe_free(dhe_named_group, dhe_context);
}

/**
 * Generates DHE public key,
 * based upon negotiated DHE algorithm.
 *
 * This function generates random secret exponent, and computes the public key, which is
 * returned via parameter public_key and public_key_size. DH context is updated accordingly.
 * If the public_key buffer is too small to hold the public key, false is returned and
 * public_key_size is set to the required buffer size to obtain the public key.
 *
 * @param  dhe_named_group                SPDM dhe_named_group
 * @param  dhe_context                   Pointer to the DHE context.
 * @param  public_key                    Pointer to the buffer to receive generated public key.
 * @param  public_key_size                On input, the size of public_key buffer in bytes.
 *                                     On output, the size of data returned in public_key buffer in bytes.
 *
 * @retval true   DHE public key generation succeeded.
 * @retval false  DHE public key generation failed.
 * @retval false  public_key_size is not large enough.
 **/
bool libspdm_secured_message_dhe_generate_key(uint16_t dhe_named_group,
                                              void *dhe_context,
                                              uint8_t *public_key,
                                              size_t *public_key_size)
{
    return libspdm_dhe_generate_key(dhe_named_group, dhe_context, public_key, public_key_size);
}

/**
 * Computes exchanged common key,
 * based upon negotiated DHE algorithm.
 *
 * Given peer's public key, this function computes the exchanged common key, based on its own
 * context including value of prime modulus and random secret exponent.
 *
 * @param  dhe_named_group                SPDM dhe_named_group
 * @param  dhe_context                   Pointer to the DHE context.
 * @param  peer_public_key                Pointer to the peer's public key.
 * @param  peer_public_key_size            size of peer's public key in bytes.
 * @param  key                          Pointer to the buffer to receive generated key.
 * @param  spdm_secured_message_context    A pointer to the SPDM secured message context.
 *
 * @retval true   DHE exchanged key generation succeeded.
 * @retval false  DHE exchanged key generation failed.
 * @retval false  key_size is not large enough.
 **/
bool libspdm_secured_message_dhe_compute_key(
    uint16_t dhe_named_group, void *dhe_context,
    const uint8_t *peer_public, size_t peer_public_size,
    void *spdm_secured_message_context)
{
    libspdm_secured_message_context_t *secured_message_context;
    uint8_t final_key[LIBSPDM_MAX_DHE_KEY_SIZE];
    size_t final_key_size;
    bool ret;

    secured_message_context = spdm_secured_message_context;

    final_key_size = sizeof(final_key);
    ret = libspdm_dhe_compute_key(dhe_named_group, dhe_context, peer_public,
                                  peer_public_size, final_key,
                                  &final_key_size);
    if (!ret) {
        return ret;
    }
    libspdm_copy_mem(secured_message_context->master_secret.dhe_secret,
                     sizeof(secured_message_context->master_secret.dhe_secret),
                     final_key, final_key_size);
    libspdm_zero_mem(final_key, final_key_size);
    secured_message_context->dhe_key_size = final_key_size;
    return true;
}
