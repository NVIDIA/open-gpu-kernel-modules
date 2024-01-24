/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#include "internal/libspdm_crypt_lib.h"
#include "library/spdm_common_lib.h"

typedef struct {
    bool is_requester;
    uint8_t op_code;
    const void *context;
    size_t context_size;
    size_t zero_pad_size;
} libspdm_signing_context_str_t;

static const libspdm_signing_context_str_t m_libspdm_signing_context_str_table[] = {
    {false, SPDM_CHALLENGE_AUTH, SPDM_CHALLENGE_AUTH_SIGN_CONTEXT,
     SPDM_CHALLENGE_AUTH_SIGN_CONTEXT_SIZE, 36 - SPDM_CHALLENGE_AUTH_SIGN_CONTEXT_SIZE},
    {true, SPDM_CHALLENGE_AUTH, SPDM_MUT_CHALLENGE_AUTH_SIGN_CONTEXT,
     SPDM_MUT_CHALLENGE_AUTH_SIGN_CONTEXT_SIZE, 36 - SPDM_MUT_CHALLENGE_AUTH_SIGN_CONTEXT_SIZE},
    {false, SPDM_MEASUREMENTS, SPDM_MEASUREMENTS_SIGN_CONTEXT, SPDM_MEASUREMENTS_SIGN_CONTEXT_SIZE,
     36 - SPDM_MEASUREMENTS_SIGN_CONTEXT_SIZE},
    {false, SPDM_KEY_EXCHANGE_RSP, SPDM_KEY_EXCHANGE_RESPONSE_SIGN_CONTEXT,
     SPDM_KEY_EXCHANGE_RESPONSE_SIGN_CONTEXT_SIZE,
     36 - SPDM_KEY_EXCHANGE_RESPONSE_SIGN_CONTEXT_SIZE},
    {true, SPDM_FINISH, SPDM_FINISH_SIGN_CONTEXT, SPDM_FINISH_SIGN_CONTEXT_SIZE,
     36 - SPDM_FINISH_SIGN_CONTEXT_SIZE},
};

#if LIBSPDM_RSA_SSA_SUPPORT
static bool libspdm_rsa_pkcs1_sign_with_nid_wrap (void *context, size_t hash_nid,
                                                  const uint8_t *param, size_t param_size,
                                                  const uint8_t *message,
                                                  size_t message_size, uint8_t *signature,
                                                  size_t *sig_size)
{
    return libspdm_rsa_pkcs1_sign_with_nid (context, hash_nid,
                                            message, message_size, signature, sig_size);
}
#endif

#if LIBSPDM_RSA_PSS_SUPPORT
static bool libspdm_rsa_pss_sign_wrap (void *context, size_t hash_nid,
                                       const uint8_t *param, size_t param_size,
                                       const uint8_t *message,
                                       size_t message_size, uint8_t *signature,
                                       size_t *sig_size)
{
    return libspdm_rsa_pss_sign (context, hash_nid,
                                 message, message_size, signature, sig_size);
}
#endif

#if LIBSPDM_ECDSA_SUPPORT
static bool libspdm_ecdsa_sign_wrap (void *context, size_t hash_nid,
                                     const uint8_t *param, size_t param_size,
                                     const uint8_t *message,
                                     size_t message_size, uint8_t *signature,
                                     size_t *sig_size)
{
    return libspdm_ecdsa_sign (context, hash_nid,
                               message, message_size, signature, sig_size);
}
#endif

#if (LIBSPDM_EDDSA_ED25519_SUPPORT) || (LIBSPDM_EDDSA_ED448_SUPPORT)
static bool libspdm_eddsa_sign_wrap (void *context, size_t hash_nid,
                                     const uint8_t *param, size_t param_size,
                                     const uint8_t *message,
                                     size_t message_size, uint8_t *signature,
                                     size_t *sig_size)
{
    return libspdm_eddsa_sign (context, hash_nid, param, param_size,
                               message, message_size, signature, sig_size);
}
#endif

#if LIBSPDM_SM2_DSA_SUPPORT
static bool libspdm_sm2_dsa_sign_wrap (void *context, size_t hash_nid,
                                       const uint8_t *param, size_t param_size,
                                       const uint8_t *message,
                                       size_t message_size, uint8_t *signature,
                                       size_t *sig_size)
{
    return libspdm_sm2_dsa_sign (context, hash_nid, param, param_size,
                                 message, message_size, signature, sig_size);
}
#endif

/**
 * Get the SPDM signing context string, which is required since SPDM 1.2.
 *
 * @param  spdm_version                         negotiated SPDM version
 * @param  op_code                              the SPDM opcode which requires the signing
 * @param  is_requester                         indicate if the signing is from a requester
 * @param  context_size                         SPDM signing context size
 **/
static const void *libspdm_get_signing_context_string (
    spdm_version_number_t spdm_version,
    uint8_t op_code,
    bool is_requester,
    size_t *context_size)
{
    size_t index;

    /* It is introduced in SPDM 1.2*/
    LIBSPDM_ASSERT((spdm_version >> SPDM_VERSION_NUMBER_SHIFT_BIT) > SPDM_MESSAGE_VERSION_11);

    for (index = 0; index < LIBSPDM_ARRAY_SIZE(m_libspdm_signing_context_str_table); index++) {
        if (m_libspdm_signing_context_str_table[index].is_requester == is_requester &&
            m_libspdm_signing_context_str_table[index].op_code == op_code) {
            *context_size = m_libspdm_signing_context_str_table[index].context_size;
            return m_libspdm_signing_context_str_table[index].context;
        }
    }
    LIBSPDM_ASSERT(false);
    return NULL;
}

/**
 * Create SPDM signing context, which is required since SPDM 1.2.
 *
 * @param  spdm_version                         negotiated SPDM version
 * @param  op_code                              the SPDM opcode which requires the signing
 * @param  is_requester                         indicate if the signing is from a requester
 * @param  spdm_signing_context                 SPDM signing context
 **/
static void libspdm_create_signing_context (
    spdm_version_number_t spdm_version,
    uint8_t op_code,
    bool is_requester,
    void *spdm_signing_context)
{
    size_t index;
    char *context_str;

    /* It is introduced in SPDM 1.2*/
    LIBSPDM_ASSERT((spdm_version >> SPDM_VERSION_NUMBER_SHIFT_BIT) > SPDM_MESSAGE_VERSION_11);

    /* So far, it only leaves 1 bytes for version*/
    LIBSPDM_ASSERT((((spdm_version >> 12) & 0xF) < 10) &&
                   (((spdm_version >> 8) & 0xF) < 10));

    context_str = spdm_signing_context;
    for (index = 0; index < 4; index++) {
        libspdm_copy_mem(context_str,
                         SPDM_VERSION_1_2_SIGNING_PREFIX_CONTEXT_SIZE,
                         SPDM_VERSION_1_2_SIGNING_PREFIX_CONTEXT,
                         SPDM_VERSION_1_2_SIGNING_PREFIX_CONTEXT_SIZE);
        /* patch the version*/
        context_str[11] = (char)('0' + ((spdm_version >> 12) & 0xF));
        context_str[13] = (char)('0' + ((spdm_version >> 8) & 0xF));
        context_str[15] = (char)('*');
        context_str += SPDM_VERSION_1_2_SIGNING_PREFIX_CONTEXT_SIZE;
    }
    for (index = 0; index < LIBSPDM_ARRAY_SIZE(m_libspdm_signing_context_str_table); index++) {
        if (m_libspdm_signing_context_str_table[index].is_requester == is_requester &&
            m_libspdm_signing_context_str_table[index].op_code == op_code) {
            libspdm_zero_mem (
                context_str,
                m_libspdm_signing_context_str_table[index].zero_pad_size);
            libspdm_copy_mem(context_str + m_libspdm_signing_context_str_table[index].zero_pad_size,
                             m_libspdm_signing_context_str_table[index].context_size,
                             m_libspdm_signing_context_str_table[index].context,
                             m_libspdm_signing_context_str_table[index].context_size);
            return;
        }
    }
    LIBSPDM_ASSERT(false);
}

uint32_t libspdm_get_asym_signature_size(uint32_t base_asym_algo)
{
    switch (base_asym_algo) {
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_2048:
#if LIBSPDM_RSA_SSA_2048_SUPPORT
        return 256;
#else
        return 0;
#endif
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_2048:
#if LIBSPDM_RSA_PSS_2048_SUPPORT
        return 256;
#else
        return 0;
#endif
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_3072:
#if LIBSPDM_RSA_SSA_3072_SUPPORT
        return 384;
#else
        return 0;
#endif
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_3072:
#if LIBSPDM_RSA_PSS_3072_SUPPORT
        return 384;
#else
        return 0;
#endif
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_4096:
#if LIBSPDM_RSA_SSA_4096_SUPPORT
        return 512;
#else
        return 0;
#endif
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_4096:
#if LIBSPDM_RSA_PSS_4096_SUPPORT
        return 512;
#else
        return 0;
#endif
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P256:
#if LIBSPDM_ECDSA_P256_SUPPORT
        return 32 * 2;
#else
        return 0;
#endif
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P384:
#if LIBSPDM_ECDSA_P384_SUPPORT
        return 48 * 2;
#else
        return 0;
#endif
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P521:
#if LIBSPDM_ECDSA_P521_SUPPORT
        return 66 * 2;
#else
        return 0;
#endif
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_SM2_ECC_SM2_P256:
#if LIBSPDM_SM2_DSA_P256_SUPPORT
        return 32 * 2;
#else
        return 0;
#endif
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED25519:
#if LIBSPDM_EDDSA_ED25519_SUPPORT
        return 32 * 2;
#else
        return 0;
#endif
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED448:
#if LIBSPDM_EDDSA_ED448_SUPPORT
        return 57 * 2;
#else
        return 0;
#endif
    default:
        return 0;
    }
}

static bool libspdm_asym_sign_wrap (void *context, size_t hash_nid, uint32_t base_asym_algo,
                                    const uint8_t *param, size_t param_size,
                                    const uint8_t *message, size_t message_size,
                                    uint8_t *signature, size_t *sig_size)
{
    switch (base_asym_algo) {
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_2048:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_3072:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_4096:
#if LIBSPDM_RSA_SSA_SUPPORT
#if !LIBSPDM_RSA_SSA_2048_SUPPORT
        LIBSPDM_ASSERT(base_asym_algo!= SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_2048);
#endif
#if !LIBSPDM_RSA_SSA_3072_SUPPORT
        LIBSPDM_ASSERT(base_asym_algo!= SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_3072);
#endif
#if !LIBSPDM_RSA_SSA_4096_SUPPORT
        LIBSPDM_ASSERT(base_asym_algo!= SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_4096);
#endif
        return libspdm_rsa_pkcs1_sign_with_nid_wrap(context, hash_nid,
                                                    param, param_size,
                                                    message, message_size,
                                                    signature, sig_size);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_2048:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_3072:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_4096:
#if LIBSPDM_RSA_PSS_SUPPORT
#if !LIBSPDM_RSA_PSS_2048_SUPPORT
        LIBSPDM_ASSERT(base_asym_algo!= SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_2048);
#endif
#if !LIBSPDM_RSA_PSS_3072_SUPPORT
        LIBSPDM_ASSERT(base_asym_algo!= SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_3072);
#endif
#if !LIBSPDM_RSA_PSS_4096_SUPPORT
        LIBSPDM_ASSERT(base_asym_algo!= SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_4096);
#endif
        return libspdm_rsa_pss_sign_wrap(context, hash_nid,
                                         param, param_size,
                                         message, message_size,
                                         signature, sig_size);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P256:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P384:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P521:
#if LIBSPDM_ECDSA_SUPPORT
#if !LIBSPDM_ECDSA_P256_SUPPORT
        LIBSPDM_ASSERT(base_asym_algo!= SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P256);
#endif
#if !LIBSPDM_ECDSA_P384_SUPPORT
        LIBSPDM_ASSERT(base_asym_algo!= SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P384);
#endif
#if !LIBSPDM_ECDSA_P521_SUPPORT
        LIBSPDM_ASSERT(base_asym_algo!= SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P521);
#endif
        return libspdm_ecdsa_sign_wrap(context, hash_nid,
                                       param, param_size,
                                       message, message_size,
                                       signature, sig_size);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED25519:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED448:
#if (LIBSPDM_EDDSA_ED25519_SUPPORT) || (LIBSPDM_EDDSA_ED448_SUPPORT)
#if !LIBSPDM_EDDSA_ED25519_SUPPORT
        LIBSPDM_ASSERT(base_asym_algo!= SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED25519);
#endif
#if !LIBSPDM_EDDSA_ED448_SUPPORT
        LIBSPDM_ASSERT(base_asym_algo!= SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED448);
#endif
        return libspdm_eddsa_sign_wrap(context, hash_nid,
                                       param, param_size,
                                       message, message_size,
                                       signature, sig_size);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_SM2_ECC_SM2_P256:
#if LIBSPDM_SM2_DSA_SUPPORT
        return libspdm_sm2_dsa_sign_wrap(context, hash_nid,
                                         param, param_size,
                                         message, message_size,
                                         signature, sig_size);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    default:
        LIBSPDM_ASSERT(false);
        return false;
    }
}

void libspdm_asym_free(uint32_t base_asym_algo, void *context)
{
    if (context == NULL) {
        return;
    }
    switch (base_asym_algo) {
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_2048:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_3072:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_4096:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_2048:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_3072:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_4096:
#if (LIBSPDM_RSA_SSA_SUPPORT) || (LIBSPDM_RSA_PSS_SUPPORT)
        libspdm_rsa_free(context);
#else
        LIBSPDM_ASSERT(false);
#endif
        break;
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P256:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P384:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P521:
#if LIBSPDM_ECDSA_SUPPORT
        libspdm_ec_free(context);
#else
        LIBSPDM_ASSERT(false);
#endif
        break;
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED25519:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED448:
#if (LIBSPDM_EDDSA_ED25519_SUPPORT) || (LIBSPDM_EDDSA_ED448_SUPPORT)
        libspdm_ecd_free(context);
#else
        LIBSPDM_ASSERT(false);
#endif
        break;
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_SM2_ECC_SM2_P256:
#if LIBSPDM_SM2_DSA_SUPPORT
        libspdm_sm2_dsa_free(context);
#else
        LIBSPDM_ASSERT(false);
#endif
        break;
    default:
        LIBSPDM_ASSERT(false);
        break;
    }
}

static bool libspdm_asym_get_public_key_from_der_wrap(uint32_t base_asym_algo,
                                                      const uint8_t *der_data,
                                                      size_t der_size,
                                                      void **context)
{
    switch (base_asym_algo) {
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_2048:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_3072:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_4096:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_2048:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_3072:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_4096:
#if (LIBSPDM_RSA_SSA_SUPPORT) || (LIBSPDM_RSA_PSS_SUPPORT)
#if !LIBSPDM_RSA_SSA_2048_SUPPORT
        LIBSPDM_ASSERT(base_asym_algo!= SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_2048);
#endif
#if !LIBSPDM_RSA_SSA_3072_SUPPORT
        LIBSPDM_ASSERT(base_asym_algo!= SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_3072);
#endif
#if !LIBSPDM_RSA_SSA_4096_SUPPORT
        LIBSPDM_ASSERT(base_asym_algo!= SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_4096);
#endif
#if !LIBSPDM_RSA_PSS_2048_SUPPORT
        LIBSPDM_ASSERT(base_asym_algo!= SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_2048);
#endif
#if !LIBSPDM_RSA_PSS_3072_SUPPORT
        LIBSPDM_ASSERT(base_asym_algo!= SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_3072);
#endif
#if !LIBSPDM_RSA_PSS_4096_SUPPORT
        LIBSPDM_ASSERT(base_asym_algo!= SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_4096);
#endif
        return libspdm_rsa_get_public_key_from_der(der_data, der_size, context);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P256:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P384:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P521:
#if LIBSPDM_ECDSA_SUPPORT
#if !LIBSPDM_ECDSA_P256_SUPPORT
        LIBSPDM_ASSERT(base_asym_algo!= SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P256);
#endif
#if !LIBSPDM_ECDSA_P384_SUPPORT
        LIBSPDM_ASSERT(base_asym_algo!= SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P384);
#endif
#if !LIBSPDM_ECDSA_P521_SUPPORT
        LIBSPDM_ASSERT(base_asym_algo!= SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P521);
#endif
        return libspdm_ec_get_public_key_from_der(der_data, der_size, context);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED25519:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED448:
#if (LIBSPDM_EDDSA_ED25519_SUPPORT) || (LIBSPDM_EDDSA_ED448_SUPPORT)
        return libspdm_ecd_get_public_key_from_der(der_data, der_size, context);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_SM2_ECC_SM2_P256:
#if LIBSPDM_SM2_DSA_SUPPORT
#if !LIBSPDM_EDDSA_ED25519_SUPPORT
        LIBSPDM_ASSERT(base_asym_algo!= SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED25519);
#endif
#if !LIBSPDM_EDDSA_ED448_SUPPORT
        LIBSPDM_ASSERT(base_asym_algo!= SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED448);
#endif
        return libspdm_sm2_get_public_key_from_der(der_data, der_size, context);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    default:
        LIBSPDM_ASSERT(false);
        return false;
    }
}

bool libspdm_asym_get_public_key_from_der(uint32_t base_asym_algo,
                                          const uint8_t *der_data,
                                          size_t der_size,
                                          void **context)
{
    return libspdm_asym_get_public_key_from_der_wrap(base_asym_algo,
                                                     der_data,
                                                     der_size,
                                                     context);
}

/**
 * Return if asymmetric function need message hash.
 *
 * @param  base_asym_algo               SPDM base_asym_algo
 *
 * @retval true  asymmetric function need message hash
 * @retval false asymmetric function need raw message
 **/
static bool libspdm_asym_func_need_hash(uint32_t base_asym_algo)
{
    switch (base_asym_algo) {
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_2048:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_3072:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_4096:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_2048:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_3072:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_4096:
        return true;
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P256:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P384:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P521:
        return true;
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED25519:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED448:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_SM2_ECC_SM2_P256:
        return false;
    default:
        LIBSPDM_ASSERT(false);
        break;
    }

    return false;
}

/**
 * libspdm_copy_signature_swap_endian_rsa
 * Swaps the endianness of a RSA signature buffer. The ECDSA signature buffer is
 * actually single internal buffer and can be swapped as a single whole buffer.
 *
 * There are two known usage models for buffers for this function.
 * 1) Source and dest are the same (and their sizes).  This would be an in-place swap.
 * 2) Source and dest are completely different (and dest size >= src size). No overlap.
 *
 * The case where source and dest are overlapped (and not the exact same buffer)
 * is not allowed and guarded against with an assert.
 **/
static void libspdm_copy_signature_swap_endian_rsa(
    uint8_t* dst,
    size_t dst_size,
    const uint8_t* src,
    size_t src_size)
{
    /* RSA signature is a single buffer to be swapped */
    size_t i;

    if (src == dst) {
        LIBSPDM_ASSERT(dst_size == src_size);

        /* src and dst are same buffer. Swap in place. */

        uint8_t byte;
        for (i = 0; i < dst_size / 2; i++) {
            byte = dst[i];
            dst[i] = dst[dst_size - i - 1];
            dst[dst_size - i - 1] = byte;
        }
    } else {
        /* src and dst are different buffers.
         * Guard against overlap case with assert.
         * Overlap case is not an expected usage model. */
        LIBSPDM_ASSERT(dst_size >= src_size);
        LIBSPDM_ASSERT((src < dst && src + src_size <= dst) ||
                       (dst < src && dst + dst_size <= src));

        for (i = 0; i < src_size; i++) {
            dst[i] = src[src_size - i - 1];
        }
    }
}

/**
 * libspdm_copy_signature_swap_endian_ecdsa
 * Swaps the endianness of a ECDSA signature buffer. The ECDSA signature buffer is
 * actually two internal buffers, and each internal buffer must be swapped individually.
 *
 * There are two known usage models for buffers for this function.
 * 1) Source and dest are the same (and their sizes).  This would be an in-place swap.
 * 2) Source and dest are completely different (and dest size >= src size). No overlap.
 *
 * The case where source and dest are overlapped (and not the exact same buffer)
 * is not allowed and guarded against with an assert.
 **/
static void libspdm_copy_signature_swap_endian_ecdsa(
    uint8_t* dst,
    size_t dst_size,
    const uint8_t* src,
    size_t src_size)
{
    /* ECDSA signature is actually 2 buffers (x & y)
     * and each must be swapped individually */
    size_t i;

    if (src == dst) {
        LIBSPDM_ASSERT(dst_size == src_size);

        /* src and dst are same buffer. Swap ecdsa 2 internal buffers in place. */

        size_t x_size;
        size_t y_size;
        uint8_t* x;
        uint8_t* y;
        uint8_t byte;

        x_size = dst_size / 2;
        y_size = x_size;

        x = dst;
        y = x + x_size;

        for (i = 0; i < x_size / 2; i++) {
            byte = x[i];
            x[i] = x[x_size - i - 1];
            x[x_size - i - 1] = byte;
        }

        for (i = 0; i < y_size / 2; i++) {
            byte = y[i];
            y[i] = y[y_size - i - 1];
            y[y_size - i - 1] = byte;
        }
    } else {
        /* src and dst are different buffers.
         * Guard against overlap case with assert.
         * Overlap case is not an expected usage model. */
        LIBSPDM_ASSERT(dst_size >= src_size);
        LIBSPDM_ASSERT((src < dst && src + src_size <= dst) ||
                       (dst < src && dst + dst_size <= src));

        size_t x_size;
        size_t y_size;

        const uint8_t* src_x;
        const uint8_t* src_y;

        uint8_t* dst_x;
        uint8_t* dst_y;

        x_size = src_size / 2;
        y_size = x_size;

        src_x = src;
        src_y = src_x + x_size;

        dst_x = dst;
        dst_y = dst_x + x_size;

        for (i = 0; i < x_size; i++) {
            dst_x[i] = src_x[x_size - i - 1];
        }

        for (i = 0; i < y_size; i++) {
            dst_y[i] = src_y[y_size - i - 1];
        }
    }
}

void libspdm_copy_signature_swap_endian(
    uint32_t base_asym_algo,
    uint8_t* dst,
    size_t dst_size,
    const uint8_t* src,
    size_t src_size)
{
    const uint32_t spdm_10_11_rsa_algos =
        SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_2048 |
        SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_2048 |
        SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_3072 |
        SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_3072 |
        SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_4096 |
        SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_4096;

    const uint32_t spdm_10_11_ecdsa_algos =
        SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P256 |
        SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P384 |
        SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P521;

    if (base_asym_algo & spdm_10_11_rsa_algos) {
        libspdm_copy_signature_swap_endian_rsa(dst, dst_size, src, src_size);
    } else if (base_asym_algo & spdm_10_11_ecdsa_algos) {
        libspdm_copy_signature_swap_endian_ecdsa(dst, dst_size, src, src_size);
    } else {
        /* Currently do not expect asymmetric algorithms other than RSA and ECDSA */
        LIBSPDM_ASSERT(0);
    }
}

/**
 * libspdm_is_palindrome
 * Checks to see if a buffer is a palindrome.
 * If the buffer is a palindrone, it is the same for both endians,
 * and therefore endianness cannot be determined.
 **/
bool libspdm_is_palindrome(const uint8_t* buf, size_t buf_size)
{
    size_t head;
    size_t tail;

    head = 0;
    tail = buf_size - 1;

    while (head < tail) {
        if (buf[head] != buf[tail]) {
            return false;
        }
        head++;
        tail--;
    }

    return true;
}

bool libspdm_is_signature_buffer_palindrome(
    uint32_t base_asym_algo, const uint8_t *buf, size_t buf_size)
{
    const uint32_t spdm_10_11_rsa_algos =
        SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_2048 |
        SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_2048 |
        SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_3072 |
        SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_3072 |
        SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_4096 |
        SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_4096;

    const uint32_t spdm_10_11_ecdsa_algos =
        SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P256 |
        SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P384 |
        SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P521;

    if (base_asym_algo & spdm_10_11_rsa_algos) {
        return libspdm_is_palindrome(buf, buf_size);
    } else if (base_asym_algo & spdm_10_11_ecdsa_algos) {
        const size_t x_size = buf_size / 2;
        const size_t y_size = x_size;

        const uint8_t *x = buf;
        const uint8_t *y = buf + x_size;

        return libspdm_is_palindrome(x, x_size) && libspdm_is_palindrome(y, y_size);
    } else {
        /* Currently do not expect asymmetric algorithms other than RSA and ECDSA */
        LIBSPDM_ASSERT(0);
        return false;
    }
}

#if LIBSPDM_RSA_SSA_SUPPORT
static bool libspdm_rsa_pkcs1_verify_with_nid_wrap (void *context, size_t hash_nid,
                                                    const uint8_t *param, size_t param_size,
                                                    const uint8_t *message,
                                                    size_t message_size,
                                                    const uint8_t *signature,
                                                    size_t sig_size)
{
    return libspdm_rsa_pkcs1_verify_with_nid (context, hash_nid,
                                              message, message_size, signature, sig_size);
}
#endif

#if LIBSPDM_RSA_PSS_SUPPORT
static bool libspdm_rsa_pss_verify_wrap (void *context, size_t hash_nid,
                                         const uint8_t *param, size_t param_size,
                                         const uint8_t *message,
                                         size_t message_size,
                                         const uint8_t *signature,
                                         size_t sig_size)
{
    return libspdm_rsa_pss_verify (context, hash_nid, message, message_size, signature, sig_size);
}
#endif

#if LIBSPDM_ECDSA_SUPPORT
bool libspdm_ecdsa_verify_wrap (void *context, size_t hash_nid,
                                const uint8_t *param, size_t param_size,
                                const uint8_t *message,
                                size_t message_size,
                                const uint8_t *signature,
                                size_t sig_size)
{
    return libspdm_ecdsa_verify (context, hash_nid, message, message_size, signature, sig_size);
}
#endif

#if (LIBSPDM_EDDSA_ED25519_SUPPORT) || (LIBSPDM_EDDSA_ED448_SUPPORT)
bool libspdm_eddsa_verify_wrap (void *context, size_t hash_nid,
                                const uint8_t *param, size_t param_size,
                                const uint8_t *message,
                                size_t message_size,
                                const uint8_t *signature,
                                size_t sig_size)
{
    return libspdm_eddsa_verify (context, hash_nid, param, param_size,
                                 message, message_size, signature, sig_size);
}
#endif

#if LIBSPDM_SM2_DSA_SUPPORT
bool libspdm_sm2_dsa_verify_wrap (void *context, size_t hash_nid,
                                  const uint8_t *param, size_t param_size,
                                  const uint8_t *message,
                                  size_t message_size,
                                  const uint8_t *signature,
                                  size_t sig_size)
{
    return libspdm_sm2_dsa_verify (context, hash_nid, param, param_size,
                                   message, message_size, signature, sig_size);
}
#endif

static bool libspdm_asym_verify_wrap(
    void *context, size_t hash_nid, uint32_t base_asym_algo,
    const uint8_t *param, size_t param_size,
    const uint8_t *message, size_t message_size,
    const uint8_t *signature, size_t sig_size)
{
    switch (base_asym_algo) {
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_2048:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_3072:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_4096:
#if LIBSPDM_RSA_SSA_SUPPORT
#if !LIBSPDM_RSA_SSA_2048_SUPPORT
        LIBSPDM_ASSERT(base_asym_algo!= SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_2048);
#endif
#if !LIBSPDM_RSA_SSA_3072_SUPPORT
        LIBSPDM_ASSERT(base_asym_algo!= SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_3072);
#endif
#if !LIBSPDM_RSA_SSA_4096_SUPPORT
        LIBSPDM_ASSERT(base_asym_algo!= SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_4096);
#endif
        return libspdm_rsa_pkcs1_verify_with_nid_wrap(context, hash_nid,
                                                      param, param_size,
                                                      message, message_size,
                                                      signature, sig_size);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_2048:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_3072:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_4096:
#if LIBSPDM_RSA_PSS_SUPPORT
#if !LIBSPDM_RSA_PSS_2048_SUPPORT
        LIBSPDM_ASSERT(base_asym_algo!= SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_2048);
#endif
#if !LIBSPDM_RSA_PSS_3072_SUPPORT
        LIBSPDM_ASSERT(base_asym_algo!= SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_3072);
#endif
#if !LIBSPDM_RSA_PSS_4096_SUPPORT
        LIBSPDM_ASSERT(base_asym_algo!= SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_4096);
#endif
        return libspdm_rsa_pss_verify_wrap(context, hash_nid,
                                           param, param_size,
                                           message, message_size,
                                           signature, sig_size);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P256:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P384:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P521:
#if LIBSPDM_ECDSA_SUPPORT
#if !LIBSPDM_ECDSA_P256_SUPPORT
        LIBSPDM_ASSERT(base_asym_algo!= SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P256);
#endif
#if !LIBSPDM_ECDSA_P384_SUPPORT
        LIBSPDM_ASSERT(base_asym_algo!= SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P384);
#endif
#if !LIBSPDM_ECDSA_P521_SUPPORT
        LIBSPDM_ASSERT(base_asym_algo!= SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P521);
#endif
        return libspdm_ecdsa_verify_wrap(context, hash_nid,
                                         param, param_size,
                                         message, message_size,
                                         signature, sig_size);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED25519:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED448:
#if (LIBSPDM_EDDSA_ED25519_SUPPORT) || (LIBSPDM_EDDSA_ED448_SUPPORT)
#if !LIBSPDM_EDDSA_ED25519_SUPPORT
        LIBSPDM_ASSERT(base_asym_algo!= SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED25519);
#endif
#if !LIBSPDM_EDDSA_ED448_SUPPORT
        LIBSPDM_ASSERT(base_asym_algo!= SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED448);
#endif
        return libspdm_eddsa_verify_wrap(context, hash_nid,
                                         param, param_size,
                                         message, message_size,
                                         signature, sig_size);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_SM2_ECC_SM2_P256:
#if LIBSPDM_SM2_DSA_SUPPORT
        return libspdm_sm2_dsa_verify_wrap(context, hash_nid,
                                           param, param_size,
                                           message, message_size,
                                           signature, sig_size);
#else
        LIBSPDM_ASSERT(false);
        return false;
#endif
    default:
        LIBSPDM_ASSERT(false);
        return false;
    }
}

bool libspdm_asym_verify_ex(
    spdm_version_number_t spdm_version, uint8_t op_code,
    uint32_t base_asym_algo, uint32_t base_hash_algo,
    void *context,
    const uint8_t *message, size_t message_size,
    const uint8_t *signature, size_t sig_size,
    uint8_t *endian)
{
    bool need_hash;
    uint8_t message_hash[LIBSPDM_MAX_HASH_SIZE];
    size_t hash_size;
    bool result;
    size_t hash_nid;
    uint8_t spdm12_signing_context_with_hash[SPDM_VERSION_1_2_SIGNING_CONTEXT_SIZE +
                                             LIBSPDM_MAX_HASH_SIZE];
    const void *param;
    size_t param_size;

    bool try_big_endian;
    bool try_little_endian;
    bool little_endian_succeeded;
    uint8_t endian_swapped_signature[LIBSPDM_MAX_ASYM_SIG_SIZE];

    hash_nid = libspdm_get_hash_nid(base_hash_algo);
    need_hash = libspdm_asym_func_need_hash(base_asym_algo);

    param = NULL;
    param_size = 0;

    if ((spdm_version >> SPDM_VERSION_NUMBER_SHIFT_BIT) > SPDM_MESSAGE_VERSION_11) {
        /* Need use SPDM 1.2 signing. */
        switch (base_asym_algo) {
        case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_SM2_ECC_SM2_P256:
            param = "";
            param_size = 0;
            break;
        case SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED25519:
            hash_nid = LIBSPDM_CRYPTO_NID_NULL;
            break;
        case SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED448:
            hash_nid = LIBSPDM_CRYPTO_NID_NULL;
            param = libspdm_get_signing_context_string (spdm_version, op_code, false, &param_size);
            break;
        default:
            /* pass thru for rest algorithm */
            break;
        }

        libspdm_create_signing_context (spdm_version, op_code, false,
                                        spdm12_signing_context_with_hash);
        hash_size = libspdm_get_hash_size(base_hash_algo);
        result = libspdm_hash_all(base_hash_algo, message, message_size,
                                  &spdm12_signing_context_with_hash[
                                      SPDM_VERSION_1_2_SIGNING_CONTEXT_SIZE]);
        if (!result) {
            return false;
        }

        /* re-assign message and message_size for signing */
        message = spdm12_signing_context_with_hash;
        message_size = SPDM_VERSION_1_2_SIGNING_CONTEXT_SIZE + hash_size;
        try_big_endian = true;
        try_little_endian = false;
        little_endian_succeeded = false;
        /* Passthru */
    } else {
        try_big_endian =
            (*endian == LIBSPDM_SPDM_10_11_VERIFY_SIGNATURE_ENDIAN_BIG_ONLY
             || *endian == LIBSPDM_SPDM_10_11_VERIFY_SIGNATURE_ENDIAN_BIG_OR_LITTLE);

        try_little_endian =
            (*endian == LIBSPDM_SPDM_10_11_VERIFY_SIGNATURE_ENDIAN_LITTLE_ONLY
             || *endian == LIBSPDM_SPDM_10_11_VERIFY_SIGNATURE_ENDIAN_BIG_OR_LITTLE);

        little_endian_succeeded = false;
    }

    if (need_hash) {
        hash_size = libspdm_get_hash_size(base_hash_algo);
        result = libspdm_hash_all(base_hash_algo, message, message_size, message_hash);
        if (!result) {
            return false;
        }
        result = false;
        if (try_big_endian) {
            result = libspdm_asym_verify_wrap(context, hash_nid, base_asym_algo,
                                              param, param_size,
                                              message_hash, hash_size,
                                              signature, sig_size);
        }
        if (!result && try_little_endian) {
            libspdm_copy_signature_swap_endian(
                base_asym_algo,
                endian_swapped_signature, sizeof(endian_swapped_signature),
                signature, sig_size);

            result = libspdm_asym_verify_wrap(context, hash_nid, base_asym_algo,
                                              param, param_size,
                                              message_hash, hash_size,
                                              endian_swapped_signature, sig_size);
            little_endian_succeeded = result;
        }
    } else {
        result = false;
        if (try_big_endian) {
            result = libspdm_asym_verify_wrap(context, hash_nid, base_asym_algo,
                                              param, param_size,
                                              message, message_size,
                                              signature, sig_size);
        }
        if (!result && try_little_endian) {
            libspdm_copy_signature_swap_endian(
                base_asym_algo,
                endian_swapped_signature, sizeof(endian_swapped_signature),
                signature, sig_size);

            result = libspdm_asym_verify_wrap(context, hash_nid, base_asym_algo,
                                              param, param_size,
                                              message, message_size,
                                              endian_swapped_signature, sig_size);
            little_endian_succeeded = result;
        }
    }
    if (try_big_endian && try_little_endian && result) {
        if (!libspdm_is_signature_buffer_palindrome(base_asym_algo, signature, sig_size)) {
            if (little_endian_succeeded) {
                *endian = LIBSPDM_SPDM_10_11_VERIFY_SIGNATURE_ENDIAN_LITTLE_ONLY;
            } else {
                *endian = LIBSPDM_SPDM_10_11_VERIFY_SIGNATURE_ENDIAN_BIG_ONLY;
            }
        }
    }
    return result;
}


bool libspdm_asym_verify_hash_ex(
    spdm_version_number_t spdm_version, uint8_t op_code,
    uint32_t base_asym_algo, uint32_t base_hash_algo,
    void *context, const uint8_t *message_hash,
    size_t hash_size, const uint8_t *signature,
    size_t sig_size, uint8_t *endian)
{
    bool need_hash;
    uint8_t *message;
    size_t message_size;
    uint8_t full_message_hash[LIBSPDM_MAX_HASH_SIZE];
    bool result;
    size_t hash_nid;
    uint8_t spdm12_signing_context_with_hash[SPDM_VERSION_1_2_SIGNING_CONTEXT_SIZE +
                                             LIBSPDM_MAX_HASH_SIZE];
    const void *param;
    size_t param_size;

    bool try_big_endian;
    bool try_little_endian;
    bool little_endian_succeeded;
    uint8_t endian_swapped_signature[LIBSPDM_MAX_ASYM_SIG_SIZE];

    hash_nid = libspdm_get_hash_nid(base_hash_algo);
    need_hash = libspdm_asym_func_need_hash(base_asym_algo);
    LIBSPDM_ASSERT (hash_size == libspdm_get_hash_size(base_hash_algo));

    param = NULL;
    param_size = 0;
    try_big_endian = true;
    try_little_endian = false;
    little_endian_succeeded = false;

    if ((spdm_version >> SPDM_VERSION_NUMBER_SHIFT_BIT) > SPDM_MESSAGE_VERSION_11) {
        /* Need use SPDM 1.2 signing */
        switch (base_asym_algo) {
        case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_SM2_ECC_SM2_P256:
            param = "";
            param_size = 0;
            break;
        case SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED25519:
            hash_nid = LIBSPDM_CRYPTO_NID_NULL;
            break;
        case SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED448:
            hash_nid = LIBSPDM_CRYPTO_NID_NULL;
            param = libspdm_get_signing_context_string (spdm_version, op_code, false, &param_size);
            break;
        default:
            /* pass thru for rest algorithm */
            break;
        }

        libspdm_create_signing_context (spdm_version, op_code, false,
                                        spdm12_signing_context_with_hash);
        libspdm_copy_mem(&spdm12_signing_context_with_hash[SPDM_VERSION_1_2_SIGNING_CONTEXT_SIZE],
                         sizeof(spdm12_signing_context_with_hash)
                         - (&spdm12_signing_context_with_hash[SPDM_VERSION_1_2_SIGNING_CONTEXT_SIZE]
                            - spdm12_signing_context_with_hash),
                         message_hash, hash_size);

        /* assign message and message_size for signing */
        message = spdm12_signing_context_with_hash;
        message_size = SPDM_VERSION_1_2_SIGNING_CONTEXT_SIZE + hash_size;

        if (need_hash) {
            result = libspdm_hash_all(base_hash_algo, message, message_size, full_message_hash);
            if (!result) {
                return false;
            }
            return libspdm_asym_verify_wrap(context, hash_nid, base_asym_algo,
                                            param, param_size,
                                            full_message_hash, hash_size,
                                            signature, sig_size);
        } else {
            return libspdm_asym_verify_wrap(context, hash_nid, base_asym_algo,
                                            param, param_size,
                                            message, message_size,
                                            signature, sig_size);
        }
        /* SPDM 1.2 signing done. */
    } else {
        try_big_endian =
            (*endian == LIBSPDM_SPDM_10_11_VERIFY_SIGNATURE_ENDIAN_BIG_ONLY
             || *endian == LIBSPDM_SPDM_10_11_VERIFY_SIGNATURE_ENDIAN_BIG_OR_LITTLE);

        try_little_endian =
            (*endian == LIBSPDM_SPDM_10_11_VERIFY_SIGNATURE_ENDIAN_LITTLE_ONLY
             || *endian == LIBSPDM_SPDM_10_11_VERIFY_SIGNATURE_ENDIAN_BIG_OR_LITTLE);

        little_endian_succeeded = false;
    }

    if (need_hash) {
        result = false;
        if (try_big_endian) {
            result = libspdm_asym_verify_wrap(context, hash_nid, base_asym_algo,
                                              param, param_size,
                                              message_hash, hash_size,
                                              signature, sig_size);
        }
        if (!result && try_little_endian) {
            libspdm_copy_signature_swap_endian(
                base_asym_algo,
                endian_swapped_signature, sizeof(endian_swapped_signature),
                signature, sig_size);

            result = libspdm_asym_verify_wrap(context, hash_nid, base_asym_algo,
                                              param, param_size,
                                              message_hash, hash_size,
                                              endian_swapped_signature, sig_size);
            little_endian_succeeded = result;
        }
        if (try_big_endian && try_little_endian && result) {
            if (!libspdm_is_signature_buffer_palindrome(base_asym_algo, signature, sig_size)) {
                if (little_endian_succeeded) {
                    *endian = LIBSPDM_SPDM_10_11_VERIFY_SIGNATURE_ENDIAN_LITTLE_ONLY;
                } else {
                    *endian = LIBSPDM_SPDM_10_11_VERIFY_SIGNATURE_ENDIAN_BIG_ONLY;
                }
            }
        }
        return result;

    } else {
        LIBSPDM_ASSERT(false);
        return false;
    }
}

bool libspdm_asym_verify(
    spdm_version_number_t spdm_version, uint8_t op_code,
    uint32_t base_asym_algo, uint32_t base_hash_algo,
    void* context, const uint8_t* message,
    size_t message_size, const uint8_t* signature,
    size_t sig_size)
{
    uint8_t endian = LIBSPDM_SPDM_10_11_VERIFY_SIGNATURE_ENDIAN_BIG_ONLY;
    return libspdm_asym_verify_ex(
        spdm_version, op_code, base_asym_algo, base_hash_algo,
        context, message, message_size, signature, sig_size, &endian);
}

bool libspdm_asym_verify_hash(
    spdm_version_number_t spdm_version, uint8_t op_code,
    uint32_t base_asym_algo, uint32_t base_hash_algo,
    void* context, const uint8_t* message_hash,
    size_t hash_size, const uint8_t* signature,
    size_t sig_size)
{
    uint8_t endian = LIBSPDM_SPDM_10_11_VERIFY_SIGNATURE_ENDIAN_BIG_ONLY;
    return libspdm_asym_verify_hash_ex(
        spdm_version, op_code, base_asym_algo, base_hash_algo,
        context, message_hash, hash_size, signature, sig_size, &endian);
}

bool libspdm_asym_sign(
    spdm_version_number_t spdm_version, uint8_t op_code,
    uint32_t base_asym_algo, uint32_t base_hash_algo,
    void *context, const uint8_t *message,
    size_t message_size, uint8_t *signature,
    size_t *sig_size)
{
    bool need_hash;
    uint8_t message_hash[LIBSPDM_MAX_HASH_SIZE];
    size_t hash_size;
    bool result;
    size_t hash_nid;
    uint8_t spdm12_signing_context_with_hash[SPDM_VERSION_1_2_SIGNING_CONTEXT_SIZE +
                                             LIBSPDM_MAX_HASH_SIZE];
    const void *param;
    size_t param_size;

    hash_nid = libspdm_get_hash_nid(base_hash_algo);
    need_hash = libspdm_asym_func_need_hash(base_asym_algo);

    param = NULL;
    param_size = 0;

    if ((spdm_version >> SPDM_VERSION_NUMBER_SHIFT_BIT) > SPDM_MESSAGE_VERSION_11) {
        /* Need use SPDM 1.2 signing */
        switch (base_asym_algo) {
        case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_SM2_ECC_SM2_P256:
            param = "";
            param_size = 0;
            break;
        case SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED25519:
            hash_nid = LIBSPDM_CRYPTO_NID_NULL;
            break;
        case SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED448:
            hash_nid = LIBSPDM_CRYPTO_NID_NULL;
            param = libspdm_get_signing_context_string (spdm_version, op_code, false, &param_size);
            break;
        default:
            /* pass thru for rest algorithm */
            break;
        }

        libspdm_create_signing_context (spdm_version, op_code, false,
                                        spdm12_signing_context_with_hash);
        hash_size = libspdm_get_hash_size(base_hash_algo);
        result = libspdm_hash_all(base_hash_algo, message, message_size,
                                  &spdm12_signing_context_with_hash[
                                      SPDM_VERSION_1_2_SIGNING_CONTEXT_SIZE]);
        if (!result) {
            return false;
        }

        /* re-assign message and message_size for signing */
        message = spdm12_signing_context_with_hash;
        message_size = SPDM_VERSION_1_2_SIGNING_CONTEXT_SIZE + hash_size;

        /* Passthru*/
    }

    if (need_hash) {
        hash_size = libspdm_get_hash_size(base_hash_algo);
        result = libspdm_hash_all(base_hash_algo, message, message_size, message_hash);
        if (!result) {
            return false;
        }
        return libspdm_asym_sign_wrap(context, hash_nid, base_asym_algo,
                                      param, param_size,
                                      message_hash, hash_size,
                                      signature, sig_size);
    } else {
        return libspdm_asym_sign_wrap(context, hash_nid, base_asym_algo,
                                      param, param_size,
                                      message, message_size,
                                      signature, sig_size);
    }
}

bool libspdm_asym_sign_hash(
    spdm_version_number_t spdm_version, uint8_t op_code,
    uint32_t base_asym_algo, uint32_t base_hash_algo,
    void *context, const uint8_t *message_hash,
    size_t hash_size, uint8_t *signature,
    size_t *sig_size)
{
    bool need_hash;
    uint8_t *message;
    size_t message_size;
    uint8_t full_message_hash[LIBSPDM_MAX_HASH_SIZE];
    bool result;
    size_t hash_nid;
    uint8_t spdm12_signing_context_with_hash[SPDM_VERSION_1_2_SIGNING_CONTEXT_SIZE +
                                             LIBSPDM_MAX_HASH_SIZE];
    const void *param;
    size_t param_size;

    hash_nid = libspdm_get_hash_nid(base_hash_algo);
    need_hash = libspdm_asym_func_need_hash(base_asym_algo);
    LIBSPDM_ASSERT (hash_size == libspdm_get_hash_size(base_hash_algo));

    param = NULL;
    param_size = 0;

    if ((spdm_version >> SPDM_VERSION_NUMBER_SHIFT_BIT) > SPDM_MESSAGE_VERSION_11) {
        /* Need use SPDM 1.2 signing */
        switch (base_asym_algo) {
        case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_SM2_ECC_SM2_P256:
            param = "";
            param_size = 0;
            break;
        case SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED25519:
            hash_nid = LIBSPDM_CRYPTO_NID_NULL;
            break;
        case SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED448:
            hash_nid = LIBSPDM_CRYPTO_NID_NULL;
            param = libspdm_get_signing_context_string (spdm_version, op_code, false, &param_size);
            break;
        default:
            /* pass thru for rest algorithm */
            break;
        }

        libspdm_create_signing_context (spdm_version, op_code, false,
                                        spdm12_signing_context_with_hash);
        libspdm_copy_mem(&spdm12_signing_context_with_hash[SPDM_VERSION_1_2_SIGNING_CONTEXT_SIZE],
                         sizeof(spdm12_signing_context_with_hash)
                         - (&spdm12_signing_context_with_hash[SPDM_VERSION_1_2_SIGNING_CONTEXT_SIZE]
                            - spdm12_signing_context_with_hash),
                         message_hash, hash_size);

        /* assign message and message_size for signing */
        message = spdm12_signing_context_with_hash;
        message_size = SPDM_VERSION_1_2_SIGNING_CONTEXT_SIZE + hash_size;

        if (need_hash) {
            result = libspdm_hash_all(base_hash_algo, message, message_size, full_message_hash);
            if (!result) {
                return false;
            }
            return libspdm_asym_sign_wrap(context, hash_nid, base_asym_algo,
                                          param, param_size,
                                          full_message_hash, hash_size,
                                          signature, sig_size);
        } else {
            return libspdm_asym_sign_wrap(context, hash_nid, base_asym_algo,
                                          param, param_size,
                                          message, message_size,
                                          signature, sig_size);
        }

        /* SPDM 1.2 signing done. */
    }

    if (need_hash) {
        return libspdm_asym_sign_wrap(context, hash_nid, base_asym_algo,
                                      param, param_size,
                                      message_hash, hash_size,
                                      signature, sig_size);
    } else {
        LIBSPDM_ASSERT (false);
        return false;
    }
}

uint32_t libspdm_get_req_asym_signature_size(uint16_t req_base_asym_alg)
{
    return libspdm_get_asym_signature_size(req_base_asym_alg);
}

void libspdm_req_asym_free(uint16_t req_base_asym_alg, void *context)
{
    libspdm_asym_free(req_base_asym_alg, context);
}

bool libspdm_req_asym_get_public_key_from_der(uint16_t req_base_asym_alg,
                                              const uint8_t *der_data,
                                              size_t der_size,
                                              void **context)
{
    return libspdm_asym_get_public_key_from_der_wrap(req_base_asym_alg,
                                                     der_data,
                                                     der_size,
                                                     context);
}

bool libspdm_req_asym_func_need_hash(uint16_t req_base_asym_alg)
{
    return libspdm_asym_func_need_hash(req_base_asym_alg);
}

bool libspdm_req_asym_verify_ex(
    spdm_version_number_t spdm_version, uint8_t op_code,
    uint16_t req_base_asym_alg,
    uint32_t base_hash_algo, void *context,
    const uint8_t *message, size_t message_size,
    const uint8_t *signature, size_t sig_size, uint8_t *endian)
{
    bool need_hash;
    uint8_t message_hash[LIBSPDM_MAX_HASH_SIZE];
    size_t hash_size;
    bool result;
    size_t hash_nid;
    uint8_t spdm12_signing_context_with_hash[SPDM_VERSION_1_2_SIGNING_CONTEXT_SIZE +
                                             LIBSPDM_MAX_HASH_SIZE];
    const void *param;
    size_t param_size;

    bool try_big_endian;
    bool try_little_endian;
    bool little_endian_succeeded;
    uint8_t endian_swapped_signature[LIBSPDM_MAX_ASYM_SIG_SIZE];

    hash_nid = libspdm_get_hash_nid(base_hash_algo);
    need_hash = libspdm_req_asym_func_need_hash(req_base_asym_alg);

    param = NULL;
    param_size = 0;

    if ((spdm_version >> SPDM_VERSION_NUMBER_SHIFT_BIT) > SPDM_MESSAGE_VERSION_11) {
        /* Need use SPDM 1.2 signing */
        switch (req_base_asym_alg) {
        case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_SM2_ECC_SM2_P256:
            param = "";
            param_size = 0;
            break;
        case SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED25519:
            hash_nid = LIBSPDM_CRYPTO_NID_NULL;
            break;
        case SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED448:
            hash_nid = LIBSPDM_CRYPTO_NID_NULL;
            param = libspdm_get_signing_context_string (spdm_version, op_code, true, &param_size);
            break;
        default:
            /* pass thru for rest algorithm */
            break;
        }

        libspdm_create_signing_context (spdm_version, op_code, true,
                                        spdm12_signing_context_with_hash);
        hash_size = libspdm_get_hash_size(base_hash_algo);
        result = libspdm_hash_all(base_hash_algo, message, message_size,
                                  &spdm12_signing_context_with_hash[
                                      SPDM_VERSION_1_2_SIGNING_CONTEXT_SIZE]);
        if (!result) {
            return false;
        }

        /* re-assign message and message_size for signing */
        message = spdm12_signing_context_with_hash;
        message_size = SPDM_VERSION_1_2_SIGNING_CONTEXT_SIZE + hash_size;
        try_big_endian = true;
        try_little_endian = false;
        little_endian_succeeded = false;
        /* Passthru */
    } else {
        try_big_endian =
            (*endian == LIBSPDM_SPDM_10_11_VERIFY_SIGNATURE_ENDIAN_BIG_ONLY
             || *endian == LIBSPDM_SPDM_10_11_VERIFY_SIGNATURE_ENDIAN_BIG_OR_LITTLE);

        try_little_endian =
            (*endian == LIBSPDM_SPDM_10_11_VERIFY_SIGNATURE_ENDIAN_LITTLE_ONLY
             || *endian == LIBSPDM_SPDM_10_11_VERIFY_SIGNATURE_ENDIAN_BIG_OR_LITTLE);

        little_endian_succeeded = false;
    }

    if (need_hash) {
        hash_size = libspdm_get_hash_size(base_hash_algo);
        result = libspdm_hash_all(base_hash_algo, message, message_size, message_hash);
        if (!result) {
            return false;
        }
        result = false;
        if (try_big_endian) {
            result = libspdm_asym_verify_wrap(context, hash_nid, req_base_asym_alg,
                                              param, param_size,
                                              message_hash, hash_size,
                                              signature, sig_size);
        }
        if (!result && try_little_endian) {
            libspdm_copy_signature_swap_endian(
                req_base_asym_alg,
                endian_swapped_signature, sizeof(endian_swapped_signature),
                signature, sig_size);

            result = libspdm_asym_verify_wrap(context, hash_nid, req_base_asym_alg,
                                              param, param_size,
                                              message_hash, hash_size,
                                              endian_swapped_signature, sig_size);
            little_endian_succeeded = result;
        }
    } else {
        result = false;
        if (try_big_endian) {
            result = libspdm_asym_verify_wrap(context, hash_nid, req_base_asym_alg,
                                              param, param_size,
                                              message, message_size,
                                              signature, sig_size);
        }
        if (!result && try_little_endian) {
            libspdm_copy_signature_swap_endian(
                req_base_asym_alg,
                endian_swapped_signature, sizeof(endian_swapped_signature),
                signature, sig_size);

            result = libspdm_asym_verify_wrap(context, hash_nid, req_base_asym_alg,
                                              param, param_size,
                                              message, message_size,
                                              endian_swapped_signature, sig_size);
            little_endian_succeeded = result;
        }
    }
    if (try_big_endian && try_little_endian && result) {
        if (!libspdm_is_signature_buffer_palindrome(req_base_asym_alg, signature, sig_size)) {
            if (little_endian_succeeded) {
                *endian = LIBSPDM_SPDM_10_11_VERIFY_SIGNATURE_ENDIAN_LITTLE_ONLY;
            } else {
                *endian = LIBSPDM_SPDM_10_11_VERIFY_SIGNATURE_ENDIAN_BIG_ONLY;
            }
        }
    }
    return result;
}

bool libspdm_req_asym_verify_hash_ex(
    spdm_version_number_t spdm_version, uint8_t op_code,
    uint16_t req_base_asym_alg,
    uint32_t base_hash_algo, void *context,
    const uint8_t *message_hash, size_t hash_size,
    const uint8_t *signature, size_t sig_size, uint8_t *endian)
{
    bool need_hash;
    uint8_t *message;
    size_t message_size;
    uint8_t full_message_hash[LIBSPDM_MAX_HASH_SIZE];
    bool result;
    size_t hash_nid;
    uint8_t spdm12_signing_context_with_hash[SPDM_VERSION_1_2_SIGNING_CONTEXT_SIZE +
                                             LIBSPDM_MAX_HASH_SIZE];
    const void *param;
    size_t param_size;

    bool try_big_endian;
    bool try_little_endian;
    bool little_endian_succeeded;
    uint8_t endian_swapped_signature[LIBSPDM_MAX_ASYM_SIG_SIZE];

    hash_nid = libspdm_get_hash_nid(base_hash_algo);
    need_hash = libspdm_req_asym_func_need_hash(req_base_asym_alg);
    LIBSPDM_ASSERT (hash_size == libspdm_get_hash_size(base_hash_algo));

    param = NULL;
    param_size = 0;

    if ((spdm_version >> SPDM_VERSION_NUMBER_SHIFT_BIT) > SPDM_MESSAGE_VERSION_11) {
        /* Need use SPDM 1.2 signing */
        switch (req_base_asym_alg) {
        case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_SM2_ECC_SM2_P256:
            param = "";
            param_size = 0;
            break;
        case SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED25519:
            hash_nid = LIBSPDM_CRYPTO_NID_NULL;
            break;
        case SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED448:
            hash_nid = LIBSPDM_CRYPTO_NID_NULL;
            param = libspdm_get_signing_context_string (spdm_version, op_code, true, &param_size);
            break;
        default:
            /* pass thru for rest algorithm */
            break;
        }

        libspdm_create_signing_context (spdm_version, op_code, true,
                                        spdm12_signing_context_with_hash);
        libspdm_copy_mem(&spdm12_signing_context_with_hash[SPDM_VERSION_1_2_SIGNING_CONTEXT_SIZE],
                         sizeof(spdm12_signing_context_with_hash)
                         - (&spdm12_signing_context_with_hash[SPDM_VERSION_1_2_SIGNING_CONTEXT_SIZE]
                            - spdm12_signing_context_with_hash),
                         message_hash, hash_size);

        /* assign message and message_size for signing */
        message = spdm12_signing_context_with_hash;
        message_size = SPDM_VERSION_1_2_SIGNING_CONTEXT_SIZE + hash_size;

        if (need_hash) {
            result = libspdm_hash_all(base_hash_algo, message, message_size,
                                      full_message_hash);
            if (!result) {
                return false;
            }
            return libspdm_asym_verify_wrap(context, hash_nid, req_base_asym_alg,
                                            param, param_size,
                                            full_message_hash, hash_size,
                                            signature, sig_size);
        } else {
            return libspdm_asym_verify_wrap(context, hash_nid, req_base_asym_alg,
                                            param, param_size,
                                            message, message_size,
                                            signature, sig_size);
        }
        /* SPDM 1.2 signing done. */
    } else {
        try_big_endian =
            (*endian == LIBSPDM_SPDM_10_11_VERIFY_SIGNATURE_ENDIAN_BIG_ONLY
             || *endian == LIBSPDM_SPDM_10_11_VERIFY_SIGNATURE_ENDIAN_BIG_OR_LITTLE);

        try_little_endian =
            (*endian == LIBSPDM_SPDM_10_11_VERIFY_SIGNATURE_ENDIAN_LITTLE_ONLY
             || *endian == LIBSPDM_SPDM_10_11_VERIFY_SIGNATURE_ENDIAN_BIG_OR_LITTLE);

        little_endian_succeeded = false;
    }

    if (need_hash) {
        result = false;
        if (try_big_endian) {
            result = libspdm_asym_verify_wrap(context, hash_nid, req_base_asym_alg,
                                              param, param_size,
                                              message_hash, hash_size,
                                              signature, sig_size);
        }
        if (!result && try_little_endian) {
            libspdm_copy_signature_swap_endian(
                req_base_asym_alg,
                endian_swapped_signature, sizeof(endian_swapped_signature),
                signature, sig_size);

            result = libspdm_asym_verify_wrap(context, hash_nid, req_base_asym_alg,
                                              param, param_size,
                                              message_hash, hash_size,
                                              endian_swapped_signature, sig_size);
            little_endian_succeeded = result;
        }
        if (try_big_endian && try_little_endian && result) {
            if (!libspdm_is_signature_buffer_palindrome(req_base_asym_alg, signature, sig_size)) {
                if (little_endian_succeeded) {
                    *endian = LIBSPDM_SPDM_10_11_VERIFY_SIGNATURE_ENDIAN_LITTLE_ONLY;
                } else {
                    *endian = LIBSPDM_SPDM_10_11_VERIFY_SIGNATURE_ENDIAN_BIG_ONLY;
                }
            }
        }
        return result;
    } else {
        LIBSPDM_ASSERT (false);
        return false;
    }
}

bool libspdm_req_asym_verify(
    spdm_version_number_t spdm_version, uint8_t op_code,
    uint16_t req_base_asym_alg,
    uint32_t base_hash_algo, void* context,
    const uint8_t* message, size_t message_size,
    const uint8_t* signature, size_t sig_size)
{
    uint8_t endian = LIBSPDM_SPDM_10_11_VERIFY_SIGNATURE_ENDIAN_BIG_ONLY;
    return libspdm_req_asym_verify_ex(
        spdm_version, op_code, req_base_asym_alg, base_hash_algo, context,
        message, message_size, signature, sig_size, &endian);
}

bool libspdm_req_asym_verify_hash(
    spdm_version_number_t spdm_version, uint8_t op_code,
    uint16_t req_base_asym_alg,
    uint32_t base_hash_algo, void* context,
    const uint8_t* message_hash, size_t hash_size,
    const uint8_t* signature, size_t sig_size)
{
    uint8_t endian = LIBSPDM_SPDM_10_11_VERIFY_SIGNATURE_ENDIAN_BIG_ONLY;
    return libspdm_req_asym_verify_hash_ex(
        spdm_version, op_code, req_base_asym_alg, base_hash_algo, context,
        message_hash, hash_size, signature, sig_size, &endian);
}

bool libspdm_req_asym_sign(
    spdm_version_number_t spdm_version, uint8_t op_code,
    uint16_t req_base_asym_alg,
    uint32_t base_hash_algo, void *context,
    const uint8_t *message, size_t message_size,
    uint8_t *signature, size_t *sig_size)
{
    bool need_hash;
    uint8_t message_hash[LIBSPDM_MAX_HASH_SIZE];
    size_t hash_size;
    bool result;
    size_t hash_nid;
    uint8_t spdm12_signing_context_with_hash[SPDM_VERSION_1_2_SIGNING_CONTEXT_SIZE +
                                             LIBSPDM_MAX_HASH_SIZE];
    const void *param;
    size_t param_size;

    hash_nid = libspdm_get_hash_nid(base_hash_algo);
    need_hash = libspdm_req_asym_func_need_hash(req_base_asym_alg);

    param = NULL;
    param_size = 0;

    if ((spdm_version >> SPDM_VERSION_NUMBER_SHIFT_BIT) > SPDM_MESSAGE_VERSION_11) {
        /* Need use SPDM 1.2 signing */
        switch (req_base_asym_alg) {
        case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_SM2_ECC_SM2_P256:
            param = "";
            param_size = 0;
            break;
        case SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED25519:
            hash_nid = LIBSPDM_CRYPTO_NID_NULL;
            break;
        case SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED448:
            hash_nid = LIBSPDM_CRYPTO_NID_NULL;
            param = libspdm_get_signing_context_string (spdm_version, op_code, true, &param_size);
            break;
        default:
            /* pass thru for rest algorithm */
            break;
        }

        libspdm_create_signing_context (spdm_version, op_code, true,
                                        spdm12_signing_context_with_hash);
        hash_size = libspdm_get_hash_size(base_hash_algo);
        result = libspdm_hash_all(base_hash_algo, message, message_size,
                                  &spdm12_signing_context_with_hash[
                                      SPDM_VERSION_1_2_SIGNING_CONTEXT_SIZE]);
        if (!result) {
            return false;
        }

        /* re-assign message and message_size for signing */
        message = spdm12_signing_context_with_hash;
        message_size = SPDM_VERSION_1_2_SIGNING_CONTEXT_SIZE + hash_size;

        /* Passthru */
    }

    if (need_hash) {
        hash_size = libspdm_get_hash_size(base_hash_algo);
        result = libspdm_hash_all(base_hash_algo, message, message_size,
                                  message_hash);
        if (!result) {
            return false;
        }
        return libspdm_asym_sign_wrap(context, hash_nid, req_base_asym_alg,
                                      param, param_size,
                                      message_hash, hash_size,
                                      signature, sig_size);
    } else {
        return libspdm_asym_sign_wrap(context, hash_nid, req_base_asym_alg,
                                      param, param_size,
                                      message, message_size,
                                      signature, sig_size);
    }
}

bool libspdm_req_asym_sign_hash(
    spdm_version_number_t spdm_version, uint8_t op_code,
    uint16_t req_base_asym_alg,
    uint32_t base_hash_algo, void *context,
    const uint8_t *message_hash, size_t hash_size,
    uint8_t *signature, size_t *sig_size)
{
    bool need_hash;
    uint8_t *message;
    size_t message_size;
    uint8_t full_message_hash[LIBSPDM_MAX_HASH_SIZE];
    bool result;
    size_t hash_nid;
    uint8_t spdm12_signing_context_with_hash[SPDM_VERSION_1_2_SIGNING_CONTEXT_SIZE +
                                             LIBSPDM_MAX_HASH_SIZE];
    const void *param;
    size_t param_size;

    hash_nid = libspdm_get_hash_nid(base_hash_algo);
    need_hash = libspdm_req_asym_func_need_hash(req_base_asym_alg);
    LIBSPDM_ASSERT (hash_size == libspdm_get_hash_size(base_hash_algo));

    param = NULL;
    param_size = 0;

    if ((spdm_version >> SPDM_VERSION_NUMBER_SHIFT_BIT) > SPDM_MESSAGE_VERSION_11) {
        /* Need use SPDM 1.2 signing */
        switch (req_base_asym_alg) {
        case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_SM2_ECC_SM2_P256:
            param = "";
            param_size = 0;
            break;
        case SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED25519:
            hash_nid = LIBSPDM_CRYPTO_NID_NULL;
            break;
        case SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED448:
            hash_nid = LIBSPDM_CRYPTO_NID_NULL;
            param = libspdm_get_signing_context_string (spdm_version, op_code, true, &param_size);
            break;
        default:
            /* pass thru for rest algorithm */
            break;
        }

        libspdm_create_signing_context (spdm_version, op_code, true,
                                        spdm12_signing_context_with_hash);
        libspdm_copy_mem(&spdm12_signing_context_with_hash[SPDM_VERSION_1_2_SIGNING_CONTEXT_SIZE],
                         sizeof(spdm12_signing_context_with_hash)
                         - (&spdm12_signing_context_with_hash[SPDM_VERSION_1_2_SIGNING_CONTEXT_SIZE]
                            - spdm12_signing_context_with_hash),
                         message_hash, hash_size);

        /* assign message and message_size for signing */
        message = spdm12_signing_context_with_hash;
        message_size = SPDM_VERSION_1_2_SIGNING_CONTEXT_SIZE + hash_size;

        if (need_hash) {
            result = libspdm_hash_all(base_hash_algo, message, message_size,
                                      full_message_hash);
            if (!result) {
                return false;
            }
            return libspdm_asym_sign_wrap(context, hash_nid, req_base_asym_alg,
                                          param, param_size,
                                          full_message_hash, hash_size,
                                          signature, sig_size);
        } else {
            return libspdm_asym_sign_wrap(context, hash_nid, req_base_asym_alg,
                                          param, param_size,
                                          message, message_size,
                                          signature, sig_size);
        }

        /* SPDM 1.2 signing done. */
    }

    if (need_hash) {
        return libspdm_asym_sign_wrap(context, hash_nid, req_base_asym_alg,
                                      param, param_size,
                                      message_hash, hash_size,
                                      signature, sig_size);
    } else {
        LIBSPDM_ASSERT (false);
        return false;
    }
}
