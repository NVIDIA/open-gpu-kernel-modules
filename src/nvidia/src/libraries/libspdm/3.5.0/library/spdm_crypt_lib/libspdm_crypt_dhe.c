/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#include "internal/libspdm_crypt_lib.h"

/**
 * This function returns the SPDM DHE algorithm key size.
 *
 * @param  dhe_named_group                SPDM dhe_named_group
 *
 * @return SPDM DHE algorithm key size.
 **/
uint32_t libspdm_get_dhe_pub_key_size(uint16_t dhe_named_group)
{
    switch (dhe_named_group) {
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_FFDHE_2048:
#if LIBSPDM_FFDHE_2048_SUPPORT
        return 256;
#else
        return 0;
#endif
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_FFDHE_3072:
#if LIBSPDM_FFDHE_3072_SUPPORT
        return 384;
#else
        return 0;
#endif
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_FFDHE_4096:
#if LIBSPDM_FFDHE_4096_SUPPORT
        return 512;
#else
        return 0;
#endif
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_SECP_256_R1:
#if LIBSPDM_ECDHE_P256_SUPPORT
        return 32 * 2;
#else
        return 0;
#endif
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_SECP_384_R1:
#if LIBSPDM_ECDHE_P384_SUPPORT
        return 48 * 2;
#else
        return 0;
#endif
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_SECP_521_R1:
#if LIBSPDM_ECDHE_P521_SUPPORT
        return 66 * 2;
#else
        return 0;
#endif
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_SM2_P256:
#if LIBSPDM_SM2_KEY_EXCHANGE_P256_SUPPORT
        return 32 * 2;
#else
        return 0;
#endif
    default:
        return 0;
    }
}

/**
 * Return cipher ID, based upon the negotiated DHE algorithm.
 *
 * @param  dhe_named_group                SPDM dhe_named_group
 *
 * @return DHE cipher ID
 **/
static size_t libspdm_get_dhe_nid(uint16_t dhe_named_group)
{
    switch (dhe_named_group) {
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_FFDHE_2048:
        return LIBSPDM_CRYPTO_NID_FFDHE2048;
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_FFDHE_3072:
        return LIBSPDM_CRYPTO_NID_FFDHE3072;
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_FFDHE_4096:
        return LIBSPDM_CRYPTO_NID_FFDHE4096;
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_SECP_256_R1:
        return LIBSPDM_CRYPTO_NID_SECP256R1;
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_SECP_384_R1:
        return LIBSPDM_CRYPTO_NID_SECP384R1;
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_SECP_521_R1:
        return LIBSPDM_CRYPTO_NID_SECP521R1;
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_SM2_P256:
        return LIBSPDM_CRYPTO_NID_SM2_KEY_EXCHANGE_P256;
    default:
        return LIBSPDM_CRYPTO_NID_NULL;
    }
}

void *libspdm_dhe_new(spdm_version_number_t spdm_version,
                      uint16_t dhe_named_group, bool is_initiator)
{
    size_t nid;
#if LIBSPDM_SM2_KEY_EXCHANGE_SUPPORT
    void *context;
    bool result;
    uint8_t spdm12_key_change_requester_context[
        SPDM_VERSION_1_2_KEY_EXCHANGE_REQUESTER_CONTEXT_SIZE];
    uint8_t spdm12_key_change_responder_context[
        SPDM_VERSION_1_2_KEY_EXCHANGE_RESPONDER_CONTEXT_SIZE];
#endif /* LIBSPDM_SM2_KEY_EXCHANGE_SUPPORT */

    nid = libspdm_get_dhe_nid(dhe_named_group);
    if (nid == 0) {
        return NULL;
    }

    switch (dhe_named_group) {
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_FFDHE_2048:
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_FFDHE_3072:
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_FFDHE_4096:
#if LIBSPDM_FFDHE_SUPPORT
#if !LIBSPDM_FFDHE_2048_SUPPORT
        LIBSPDM_ASSERT(dhe_named_group != SPDM_ALGORITHMS_DHE_NAMED_GROUP_FFDHE_2048);
#endif
#if !LIBSPDM_FFDHE_3072_SUPPORT
        LIBSPDM_ASSERT(dhe_named_group != SPDM_ALGORITHMS_DHE_NAMED_GROUP_FFDHE_3072);
#endif
#if !LIBSPDM_FFDHE_4096_SUPPORT
        LIBSPDM_ASSERT(dhe_named_group != SPDM_ALGORITHMS_DHE_NAMED_GROUP_FFDHE_4096);
#endif
        return libspdm_dh_new_by_nid(nid);
#else
        LIBSPDM_ASSERT(false);
        return NULL;
#endif
        break;
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_SECP_256_R1:
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_SECP_384_R1:
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_SECP_521_R1:
#if LIBSPDM_ECDHE_SUPPORT
#if !LIBSPDM_ECDHE_P256_SUPPORT
        LIBSPDM_ASSERT(dhe_named_group != SPDM_ALGORITHMS_DHE_NAMED_GROUP_SECP_256_R1);
#endif
#if !LIBSPDM_ECDHE_P384_SUPPORT
        LIBSPDM_ASSERT(dhe_named_group != SPDM_ALGORITHMS_DHE_NAMED_GROUP_SECP_384_R1);
#endif
#if !LIBSPDM_ECDHE_P521_SUPPORT
        LIBSPDM_ASSERT(dhe_named_group != SPDM_ALGORITHMS_DHE_NAMED_GROUP_SECP_521_R1);
#endif
        return libspdm_ec_new_by_nid(nid);
#else
        LIBSPDM_ASSERT(false);
        return NULL;
#endif
        break;
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_SM2_P256:
#if LIBSPDM_SM2_KEY_EXCHANGE_SUPPORT
        context = libspdm_sm2_key_exchange_new_by_nid(nid);

        libspdm_copy_mem(spdm12_key_change_requester_context,
                         sizeof(spdm12_key_change_requester_context),
                         SPDM_VERSION_1_2_KEY_EXCHANGE_REQUESTER_CONTEXT,
                         SPDM_VERSION_1_2_KEY_EXCHANGE_REQUESTER_CONTEXT_SIZE);
        libspdm_copy_mem(spdm12_key_change_responder_context,
                         sizeof(spdm12_key_change_responder_context),
                         SPDM_VERSION_1_2_KEY_EXCHANGE_RESPONDER_CONTEXT,
                         SPDM_VERSION_1_2_KEY_EXCHANGE_RESPONDER_CONTEXT_SIZE);
        /* patch the version*/
        spdm12_key_change_requester_context[25] = (char)('0' + ((spdm_version >> 12) & 0xF));
        spdm12_key_change_requester_context[27] = (char)('0' + ((spdm_version >> 8) & 0xF));
        spdm12_key_change_responder_context[25] = (char)('0' + ((spdm_version >> 12) & 0xF));
        spdm12_key_change_responder_context[27] = (char)('0' + ((spdm_version >> 8) & 0xF));

        result = libspdm_sm2_key_exchange_init (context, LIBSPDM_CRYPTO_NID_SM3_256,
                                                spdm12_key_change_requester_context,
                                                SPDM_VERSION_1_2_KEY_EXCHANGE_REQUESTER_CONTEXT_SIZE,
                                                spdm12_key_change_responder_context,
                                                SPDM_VERSION_1_2_KEY_EXCHANGE_RESPONDER_CONTEXT_SIZE,
                                                is_initiator);
        if (!result) {
            libspdm_sm2_key_exchange_free (context);
            return NULL;
        }
        return context;
#else
        LIBSPDM_ASSERT(false);
        return NULL;
#endif
        break;
    default:
        LIBSPDM_ASSERT(false);
        return NULL;
    }
}

void libspdm_dhe_free(uint16_t dhe_named_group, void *context)
{
    if (context == NULL) {
        return;
    }
    switch (dhe_named_group) {
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_FFDHE_2048:
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_FFDHE_3072:
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_FFDHE_4096:
#if LIBSPDM_FFDHE_SUPPORT
        libspdm_dh_free(context);
#else
        LIBSPDM_ASSERT(false);
#endif
        break;
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_SECP_256_R1:
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_SECP_384_R1:
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_SECP_521_R1:
#if LIBSPDM_ECDHE_SUPPORT
        libspdm_ec_free(context);
#else
        LIBSPDM_ASSERT(false);
#endif
        break;
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_SM2_P256:
#if LIBSPDM_SM2_KEY_EXCHANGE_SUPPORT
        libspdm_sm2_key_exchange_free(context);
#else
        LIBSPDM_ASSERT(false);
#endif
        break;
    default:
        LIBSPDM_ASSERT(false);
        break;
    }
}

bool libspdm_dhe_generate_key(uint16_t dhe_named_group, void *context,
                              uint8_t *public_key,
                              size_t *public_key_size)
{
    switch (dhe_named_group) {
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_FFDHE_2048:
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_FFDHE_3072:
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_FFDHE_4096:
#if LIBSPDM_FFDHE_SUPPORT
#if !LIBSPDM_FFDHE_2048_SUPPORT
        LIBSPDM_ASSERT(dhe_named_group != SPDM_ALGORITHMS_DHE_NAMED_GROUP_FFDHE_2048);
#endif
#if !LIBSPDM_FFDHE_3072_SUPPORT
        LIBSPDM_ASSERT(dhe_named_group != SPDM_ALGORITHMS_DHE_NAMED_GROUP_FFDHE_3072);
#endif
#if !LIBSPDM_FFDHE_4096_SUPPORT
        LIBSPDM_ASSERT(dhe_named_group != SPDM_ALGORITHMS_DHE_NAMED_GROUP_FFDHE_4096);
#endif
        return libspdm_dh_generate_key(context, public_key, public_key_size);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_SECP_256_R1:
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_SECP_384_R1:
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_SECP_521_R1:
#if LIBSPDM_ECDHE_SUPPORT
#if !LIBSPDM_ECDHE_P256_SUPPORT
        LIBSPDM_ASSERT(dhe_named_group != SPDM_ALGORITHMS_DHE_NAMED_GROUP_SECP_256_R1);
#endif
#if !LIBSPDM_ECDHE_P384_SUPPORT
        LIBSPDM_ASSERT(dhe_named_group != SPDM_ALGORITHMS_DHE_NAMED_GROUP_SECP_384_R1);
#endif
#if !LIBSPDM_ECDHE_P521_SUPPORT
        LIBSPDM_ASSERT(dhe_named_group != SPDM_ALGORITHMS_DHE_NAMED_GROUP_SECP_521_R1);
#endif
        return libspdm_ec_generate_key(context, public_key, public_key_size);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_SM2_P256:
#if LIBSPDM_SM2_KEY_EXCHANGE_SUPPORT
        return libspdm_sm2_key_exchange_generate_key(context, public_key, public_key_size);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    default:
        LIBSPDM_ASSERT(false);
        return false;
    }
}

bool libspdm_dhe_compute_key(uint16_t dhe_named_group, void *context,
                             const uint8_t *peer_public,
                             size_t peer_public_size, uint8_t *key,
                             size_t *key_size)
{
#if LIBSPDM_SM2_KEY_EXCHANGE_SUPPORT
    bool ret;
#endif

    switch (dhe_named_group) {
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_FFDHE_2048:
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_FFDHE_3072:
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_FFDHE_4096:
#if LIBSPDM_FFDHE_SUPPORT
#if !LIBSPDM_FFDHE_2048_SUPPORT
        LIBSPDM_ASSERT(dhe_named_group != SPDM_ALGORITHMS_DHE_NAMED_GROUP_FFDHE_2048);
#endif
#if !LIBSPDM_FFDHE_3072_SUPPORT
        LIBSPDM_ASSERT(dhe_named_group != SPDM_ALGORITHMS_DHE_NAMED_GROUP_FFDHE_3072);
#endif
#if !LIBSPDM_FFDHE_4096_SUPPORT
        LIBSPDM_ASSERT(dhe_named_group != SPDM_ALGORITHMS_DHE_NAMED_GROUP_FFDHE_4096);
#endif
        return libspdm_dh_compute_key(context, peer_public, peer_public_size, key, key_size);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_SECP_256_R1:
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_SECP_384_R1:
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_SECP_521_R1:
#if LIBSPDM_ECDHE_SUPPORT
#if !LIBSPDM_ECDHE_P256_SUPPORT
        LIBSPDM_ASSERT(dhe_named_group != SPDM_ALGORITHMS_DHE_NAMED_GROUP_SECP_256_R1);
#endif
#if !LIBSPDM_ECDHE_P384_SUPPORT
        LIBSPDM_ASSERT(dhe_named_group != SPDM_ALGORITHMS_DHE_NAMED_GROUP_SECP_384_R1);
#endif
#if !LIBSPDM_ECDHE_P521_SUPPORT
        LIBSPDM_ASSERT(dhe_named_group != SPDM_ALGORITHMS_DHE_NAMED_GROUP_SECP_521_R1);
#endif
        return libspdm_ec_compute_key(context, peer_public, peer_public_size, key, key_size);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_DHE_NAMED_GROUP_SM2_P256:
#if LIBSPDM_SM2_KEY_EXCHANGE_SUPPORT
        ret = libspdm_sm2_key_exchange_compute_key(context, peer_public,
                                                   peer_public_size, key, key_size);
        if (!ret) {
            return false;
        } else {
            /* SM2 key exchange can generate arbitrary length key_size.
             * SPDM requires SM2 key_size to be 16. */
            LIBSPDM_ASSERT (*key_size >= 16);
            *key_size = 16;

            return true;
        }
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    default:
        LIBSPDM_ASSERT(false);
        return false;
    }
}
