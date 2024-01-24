/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#include "internal/libspdm_crypt_lib.h"

#if LIBSPDM_CERT_PARSE_SUPPORT

/**pathLenConstraint is optional.
 * In https://www.pkisolutions.com/basic-constraints-certificate-extension/:
 * pathLenConstraint: How many CAs are allowed in the chain below current CA certificate.
 * This setting has no meaning for end entity certificates.
 **/

/**
 * leaf cert spdm extension len
 * len > 2 * (spdm id-DMTF-spdm size + 2)
 **/

#ifndef LIBSPDM_MAX_EXTENSION_LEN
#define LIBSPDM_MAX_EXTENSION_LEN 30
#endif

#ifndef LIBSPDM_MAX_NAME_SIZE
#define LIBSPDM_MAX_NAME_SIZE 100
#endif

/*max public key encryption algo oid len*/
#ifndef LIBSPDM_MAX_ENCRYPTION_ALGO_OID_LEN
#define LIBSPDM_MAX_ENCRYPTION_ALGO_OID_LEN 10
#endif

/*leaf cert basic constraints len,CA = false: 30 03 01 01 00*/
#ifndef LIBSPDM_MAX_BASIC_CONSTRAINTS_CA_LEN
#define LIBSPDM_MAX_BASIC_CONSTRAINTS_CA_LEN 5
#endif

/**
 * 0x02 is integer;
 * 0x82 indicates that the length is expressed in two bytes;
 * 0x01 and 0x01 are rsa key len;
 **/
#if (LIBSPDM_RSA_SSA_2048_SUPPORT) || (LIBSPDM_RSA_PSS_2048_SUPPORT)
#define KEY_ENCRY_ALGO_RSA2048_FLAG {0x02, 0x82, 0x01, 0x01}
/* the other case is ASN1 code different when integer is 1 on highest position*/
#define KEY_ENCRY_ALGO_RSA2048_FLAG_OTHER {0x02, 0x82, 0x01, 0x00}
#endif
#if (LIBSPDM_RSA_SSA_3072_SUPPORT) || (LIBSPDM_RSA_PSS_3072_SUPPORT)
#define KEY_ENCRY_ALGO_RSA3072_FLAG {0x02, 0x82, 0x01, 0x81}
/* the other case is ASN1 code different when integer is 1 on highest position*/
#define KEY_ENCRY_ALGO_RSA3072_FLAG_OTHER {0x02, 0x82, 0x01, 0x80}
#endif
#if (LIBSPDM_RSA_SSA_4096_SUPPORT) || (LIBSPDM_RSA_PSS_4096_SUPPORT)
#define KEY_ENCRY_ALGO_RSA4096_FLAG {0x02, 0x82, 0x02, 0x01}
/* the other case is ASN1 code different when integer is 1 on highest position*/
#define KEY_ENCRY_ALGO_RSA4096_FLAG_OTHER {0x02, 0x82, 0x02, 0x00}
#endif

/**
 * https://oidref.com/1.2.840.10045.3.1.7
 * ECC256 curve OID: 1.2.840.10045.3.1.7
 * https://oidref.com/1.3.132.0.34
 * ECC384 curve OID: 1.3.132.0.34
 * https://oidref.com/1.3.132.0.35
 * ECC521 curve OID: 1.3.132.0.35
 **/
#if LIBSPDM_ECDSA_P256_SUPPORT
#define KEY_ENCRY_ALGO_ECC256_OID {0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x07}
#endif
#if LIBSPDM_ECDSA_P384_SUPPORT
#define KEY_ENCRY_ALGO_ECC384_OID {0x2B, 0x81, 0x04, 0x00, 0x22}
#endif
#if LIBSPDM_ECDSA_P521_SUPPORT
#define KEY_ENCRY_ALGO_ECC521_OID {0x2B, 0x81, 0x04, 0x00, 0x23}
#endif

/**
 * EDxxx OID: https://datatracker.ietf.org/doc/html/rfc8420
 * ED448 OID: 1.3.101.113
 * ED25519 OID: 1.3.101.112
 **/
#if LIBSPDM_EDDSA_ED25519_SUPPORT
#define ENCRY_ALGO_ED25519_OID {0x2B, 0x65, 0x70}
#endif
#if LIBSPDM_EDDSA_ED448_SUPPORT
#define ENCRY_ALGO_ED448_OID {0x2B, 0x65, 0x71}
#endif

/*leaf cert basic_constraints false case1: CA: false and CA object is excluded */
#define BASIC_CONSTRAINTS_STRING_FALSE_CASE1 {0x30, 0x00}

/*leaf cert basic_constraints false case2: CA: false */
#define BASIC_CONSTRAINTS_STRING_FALSE_CASE2 {0x30, 0x03, 0x01, 0x01, 0x00}

/*leaf cert basic_constraints true case: CA: true */
#define BASIC_CONSTRAINTS_STRING_TRUE_CASE {0x30, 0x03, 0x01, 0x01, 0xFF}

/**
 * Retrieve the asymmetric public key from one DER-encoded X509 certificate.
 *
 * @param  cert       Pointer to the DER-encoded X509 certificate.
 * @param  cert_size  Size of the X509 certificate in bytes.
 * @param  context    Pointer to newly generated asymmetric context which contain the retrieved public
 *                    key component. Use libspdm_asym_free() function to free the resource.
 *
 * @retval  true   public key was retrieved successfully.
 * @retval  false  Fail to retrieve public key from X509 certificate.
 **/
typedef bool (*libspdm_asym_get_public_key_from_x509_func)(const uint8_t *cert,
                                                           size_t cert_size,
                                                           void **context);

/**
 * Return asymmetric GET_PUBLIC_KEY_FROM_X509 function, based upon the negotiated asymmetric algorithm.
 *
 * @param  base_asym_algo                 SPDM base_asym_algo
 *
 * @return asymmetric GET_PUBLIC_KEY_FROM_X509 function
 **/
libspdm_asym_get_public_key_from_x509_func
static libspdm_get_asym_get_public_key_from_x509(uint32_t base_asym_algo)
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
        return libspdm_rsa_get_public_key_from_x509;
#else
        LIBSPDM_ASSERT(false);
        break;
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
        return libspdm_ec_get_public_key_from_x509;
#else
        LIBSPDM_ASSERT(false);
        break;
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
        return libspdm_ecd_get_public_key_from_x509;
#else
        LIBSPDM_ASSERT(false);
        break;
#endif
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_SM2_ECC_SM2_P256:
#if LIBSPDM_SM2_DSA_SUPPORT
        return libspdm_sm2_get_public_key_from_x509;
#else
        LIBSPDM_ASSERT(false);
        break;
#endif
    default:
        LIBSPDM_ASSERT(false);
        break;
    }

    return NULL;
}

/**
 * Retrieve the asymmetric public key from one DER-encoded X509 certificate,
 * based upon negotiated asymmetric algorithm.
 *
 * @param  base_asym_algo                 SPDM base_asym_algo
 * @param  cert                         Pointer to the DER-encoded X509 certificate.
 * @param  cert_size                     size of the X509 certificate in bytes.
 * @param  context                      Pointer to newly generated asymmetric context which contain the retrieved public key component.
 *                                     Use libspdm_asym_free() function to free the resource.
 *
 * @retval  true   public key was retrieved successfully.
 * @retval  false  Fail to retrieve public key from X509 certificate.
 **/
bool libspdm_asym_get_public_key_from_x509(uint32_t base_asym_algo,
                                           const uint8_t *cert,
                                           size_t cert_size,
                                           void **context)
{
    libspdm_asym_get_public_key_from_x509_func get_public_key_from_x509_function;
    get_public_key_from_x509_function = libspdm_get_asym_get_public_key_from_x509(base_asym_algo);
    if (get_public_key_from_x509_function == NULL) {
        return false;
    }
    return get_public_key_from_x509_function(cert, cert_size, context);
}

/**
 * Return requester asymmetric GET_PUBLIC_KEY_FROM_X509 function, based upon the negotiated requester asymmetric algorithm.
 *
 * @param  req_base_asym_alg               SPDM req_base_asym_alg
 *
 * @return requester asymmetric GET_PUBLIC_KEY_FROM_X509 function
 **/
static libspdm_asym_get_public_key_from_x509_func
libspdm_get_req_asym_get_public_key_from_x509(uint16_t req_base_asym_alg)
{
    return libspdm_get_asym_get_public_key_from_x509(req_base_asym_alg);
}

/**
 * Retrieve the asymmetric public key from one DER-encoded X509 certificate,
 * based upon negotiated requester asymmetric algorithm.
 *
 * @param  req_base_asym_alg               SPDM req_base_asym_alg
 * @param  cert                         Pointer to the DER-encoded X509 certificate.
 * @param  cert_size                     size of the X509 certificate in bytes.
 * @param  context                      Pointer to newly generated asymmetric context which contain the retrieved public key component.
 *                                     Use libspdm_asym_free() function to free the resource.
 *
 * @retval  true   public key was retrieved successfully.
 * @retval  false  Fail to retrieve public key from X509 certificate.
 **/
bool libspdm_req_asym_get_public_key_from_x509(uint16_t req_base_asym_alg,
                                               const uint8_t *cert,
                                               size_t cert_size,
                                               void **context)
{
    libspdm_asym_get_public_key_from_x509_func get_public_key_from_x509_function;
    get_public_key_from_x509_function =
        libspdm_get_req_asym_get_public_key_from_x509(req_base_asym_alg);
    if (get_public_key_from_x509_function == NULL) {
        return false;
    }
    return get_public_key_from_x509_function(cert, cert_size, context);
}

/**
 * Check the X509 DataTime is within a valid range.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  from                         notBefore Pointer to date_time object.
 * @param  from_size                     notBefore date_time object size.
 * @param  to                           notAfter Pointer to date_time object.
 * @param  to_size                       notAfter date_time object size.
 *
 * @retval  true   verification pass.
 * @retval  false  verification fail.
 **/
static bool libspdm_internal_x509_date_time_check(const uint8_t *from,
                                                  size_t from_size,
                                                  const uint8_t *to,
                                                  size_t to_size)
{
    int32_t ret;
    bool status;
    uint8_t f0[64];
    uint8_t t0[64];
    size_t f0_size;
    size_t t0_size;

    f0_size = 64;
    t0_size = 64;

    status = libspdm_x509_set_date_time("19700101000000Z", f0, &f0_size);
    if (!status) {
        return false;
    }

    status = libspdm_x509_set_date_time("99991231235959Z", t0, &t0_size);
    if (!status) {
        return false;
    }

    /* from >= f0*/
    ret = libspdm_x509_compare_date_time(from, f0);
    if (ret < 0) {
        return false;
    }

    /* to <= t0*/
    ret = libspdm_x509_compare_date_time(t0, to);
    if (ret < 0) {
        return false;
    }

    return true;
}

/**
 * This function returns the SPDM public key encryption algorithm OID len.
 *
 * @param[in]  base_asym_algo          SPDM base_asym_algo
 *
 * @return SPDM public key encryption algorithms OID len.
 **/
static uint32_t libspdm_get_public_key_algo_OID_len(uint32_t base_asym_algo)
{
    switch (base_asym_algo) {
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_2048:
#if LIBSPDM_RSA_SSA_2048_SUPPORT
        return 4;
#else
        return 0;
#endif
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_2048:
#if LIBSPDM_RSA_PSS_2048_SUPPORT
        return 4;
#else
        return 0;
#endif
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_3072:
#if LIBSPDM_RSA_SSA_3072_SUPPORT
        return 4;
#else
        return 0;
#endif
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_3072:
#if LIBSPDM_RSA_PSS_3072_SUPPORT
        return 4;
#else
        return 0;
#endif
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_4096:
#if LIBSPDM_RSA_SSA_4096_SUPPORT
        return 4;
#else
        return 0;
#endif
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_4096:
#if LIBSPDM_RSA_PSS_4096_SUPPORT
        return 4;
#else
        return 0;
#endif
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P256:
#if LIBSPDM_ECDSA_P256_SUPPORT
        return 8;
#else
        return 0;
#endif
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P384:
#if LIBSPDM_ECDSA_P384_SUPPORT
        return 5;
#else
        return 0;
#endif
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P521:
#if LIBSPDM_ECDSA_P521_SUPPORT
        return 5;
#else
        return 0;
#endif
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED25519:
#if LIBSPDM_EDDSA_ED25519_SUPPORT
        return 3;
#else
        return 0;
#endif
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED448:
#if LIBSPDM_EDDSA_ED448_SUPPORT
        return 3;
#else
        return 0;
#endif
    default:
        LIBSPDM_ASSERT(false);
        return 0;
    }
}

/**
 * This function get the SPDM public key encryption algorithm OID.
 *
 * @param[in]      base_asym_algo                 SPDM base_asym_algo
 * @param[in,out]  oid                            SPDM public key encryption algorithm OID
 * @param[in,out]  oid_other                      Other SPDM public key encryption algorithm OID
 *                                                because of ASN1 code for integer
 *
 * @retval  true   get OID sucessful.
 * @retval  false  get OID fail.
 **/
static bool libspdm_get_public_key_algo_OID(uint32_t base_asym_algo, uint8_t *oid,
                                            uint8_t *oid_other)
{
    uint32_t oid_len;
    oid_len = libspdm_get_public_key_algo_OID_len(base_asym_algo);
    if(oid_len == 0) {
        return false;
    }

    switch (base_asym_algo) {
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_2048:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_2048: {
#if (LIBSPDM_RSA_SSA_2048_SUPPORT) || (LIBSPDM_RSA_PSS_2048_SUPPORT)
        uint8_t encry_algo_oid_rsa2048[] = KEY_ENCRY_ALGO_RSA2048_FLAG;
        uint8_t encry_algo_oid_rsa2048_ohter[] = KEY_ENCRY_ALGO_RSA2048_FLAG_OTHER;
        libspdm_copy_mem(oid, oid_len, encry_algo_oid_rsa2048, oid_len);
        libspdm_copy_mem(oid_other, oid_len, encry_algo_oid_rsa2048_ohter, oid_len);
        return true;
#else
        return false;
#endif
    }
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_3072:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_3072: {
#if (LIBSPDM_RSA_SSA_3072_SUPPORT) || (LIBSPDM_RSA_PSS_3072_SUPPORT)
        uint8_t encry_algo_oid_rsa3072[] = KEY_ENCRY_ALGO_RSA3072_FLAG;
        uint8_t encry_algo_oid_rsa3072_ohter[] = KEY_ENCRY_ALGO_RSA3072_FLAG_OTHER;
        libspdm_copy_mem(oid, oid_len, encry_algo_oid_rsa3072, oid_len);
        libspdm_copy_mem(oid_other, oid_len, encry_algo_oid_rsa3072_ohter, oid_len);
        return true;
#else
        return false;
#endif
    }
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_4096:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_4096: {
#if (LIBSPDM_RSA_SSA_4096_SUPPORT) || (LIBSPDM_RSA_PSS_4096_SUPPORT)
        uint8_t encry_algo_oid_rsa4096[] = KEY_ENCRY_ALGO_RSA4096_FLAG;
        uint8_t encry_algo_oid_rsa4096_ohter[] = KEY_ENCRY_ALGO_RSA4096_FLAG_OTHER;
        libspdm_copy_mem(oid, oid_len, encry_algo_oid_rsa4096, oid_len);
        libspdm_copy_mem(oid_other, oid_len, encry_algo_oid_rsa4096_ohter, oid_len);
        return true;
#else
        return false;
#endif
    }

    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P256: {
#if LIBSPDM_ECDSA_P256_SUPPORT
        uint8_t encry_algo_oid_ecc256[] = KEY_ENCRY_ALGO_ECC256_OID;
        libspdm_copy_mem(oid, oid_len, encry_algo_oid_ecc256, oid_len);
        return true;
#else
        return false;
#endif
    }
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P384: {
#if LIBSPDM_ECDSA_P384_SUPPORT
        uint8_t encry_algo_oid_ecc384[] = KEY_ENCRY_ALGO_ECC384_OID;
        libspdm_copy_mem(oid, oid_len, encry_algo_oid_ecc384, oid_len);
        return true;
#else
        return false;
#endif
    }
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P521: {
#if LIBSPDM_ECDSA_P521_SUPPORT
        uint8_t encry_algo_oid_ecc521[] = KEY_ENCRY_ALGO_ECC521_OID;
        libspdm_copy_mem(oid, oid_len, encry_algo_oid_ecc521, oid_len);
        return true;
#else
        return false;
#endif
    }

    /*sm2 oid  TBD*/
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_SM2_ECC_SM2_P256:
        return true;

    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED25519: {
#if LIBSPDM_EDDSA_ED25519_SUPPORT
        uint8_t encry_algo_oid_ed25519[] = ENCRY_ALGO_ED25519_OID;
        libspdm_copy_mem(oid, oid_len, encry_algo_oid_ed25519, oid_len);
        return true;
#else
        return false;
#endif
        break;
    }
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED448: {
#if LIBSPDM_EDDSA_ED448_SUPPORT
        uint8_t encry_algo_oid_ed448[] = ENCRY_ALGO_ED448_OID;
        libspdm_copy_mem(oid, oid_len, encry_algo_oid_ed448, oid_len);
        return true;
#else
        return false;
#endif
        break;
    }

    default:
        LIBSPDM_ASSERT(false);
        return false;
    }
}

/**
 * Verify cert public key encryption algorithm is matched to negotiated base_aysm algo
 *
 * @param[in]      cert                  Pointer to the DER-encoded certificate data.
 * @param[in]      cert_size             The size of certificate data in bytes.
 * @param[in]      base_asym_algo        SPDM base_asym_algo
 * @param[out]     oid                   cert public key encryption algorithm OID
 * @param[in]      oid_size              the buffer size for required OID
 *
 * @retval  true   get public key oid from cert successfully
 * @retval  false  get public key oid from cert fail
 **/
static bool libspdm_get_public_key_oid(const uint8_t *cert, size_t cert_size,
                                       uint8_t *oid, size_t oid_size, uint32_t base_asym_algo)
{
    bool ret;
    uint8_t *ptr;
    int32_t length;
    size_t obj_len;
    uint8_t *end;
    uint8_t index;
    uint8_t sequence_time;

    length = (int32_t)cert_size;
    ptr = (uint8_t*)(size_t)cert;
    obj_len = 0;
    end = ptr + length;
    ret = true;

    /* TBSCertificate have 5 sequence before subjectPublicKeyInfo*/
    sequence_time = 5;

    /*all cert sequence*/
    ret = libspdm_asn1_get_tag(&ptr, end, &obj_len,
                               LIBSPDM_CRYPTO_ASN1_SEQUENCE | LIBSPDM_CRYPTO_ASN1_CONSTRUCTED);
    if (!ret) {
        return false;
    }

    /*TBSCertificate sequence*/
    ret = libspdm_asn1_get_tag(&ptr, end, &obj_len,
                               LIBSPDM_CRYPTO_ASN1_SEQUENCE | LIBSPDM_CRYPTO_ASN1_CONSTRUCTED);
    if (!ret) {
        return false;
    }

    end = ptr + obj_len;
    /*version*/
    ret = libspdm_asn1_get_tag(&ptr, end, &obj_len,
                               LIBSPDM_CRYPTO_ASN1_CONTEXT_SPECIFIC |
                               LIBSPDM_CRYPTO_ASN1_CONSTRUCTED);
    if (!ret) {
        return false;
    }

    ptr += obj_len;
    /*serialNumber*/
    ret = libspdm_asn1_get_tag(&ptr, end, &obj_len, LIBSPDM_CRYPTO_ASN1_INTEGER);
    if (!ret) {
        return false;
    }

    /**
     * signature AlgorithmIdentifier,
     * issuer Name,
     * validity Validity,
     * subject Name,
     * subjectPublicKeyInfo
     **/
    for (index = 0; index < sequence_time; index++) {
        ptr += obj_len;
        ret = libspdm_asn1_get_tag(&ptr, end, &obj_len,
                                   LIBSPDM_CRYPTO_ASN1_SEQUENCE | LIBSPDM_CRYPTO_ASN1_CONSTRUCTED);
        if (!ret) {
            return false;
        }
    }

    switch (base_asym_algo)
    {
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_2048:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_2048:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_3072:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_3072:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_4096:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_4096:
        ret = libspdm_asn1_get_tag(&ptr, end, &obj_len,
                                   LIBSPDM_CRYPTO_ASN1_SEQUENCE | LIBSPDM_CRYPTO_ASN1_CONSTRUCTED);
        if (!ret) {
            return false;
        }

        ptr += obj_len;
        ret = libspdm_asn1_get_tag(&ptr, end, &obj_len, LIBSPDM_CRYPTO_ASN1_BIT_STRING);
        if (!ret) {
            return false;
        }

        /*get rsa key len*/
        ptr++;
        ret = libspdm_asn1_get_tag(&ptr, end, &obj_len,
                                   LIBSPDM_CRYPTO_ASN1_SEQUENCE | LIBSPDM_CRYPTO_ASN1_CONSTRUCTED);
        if (!ret) {
            return false;
        }
        libspdm_copy_mem(oid, oid_size, ptr, oid_size);
        break;
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P256:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P384:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P521:
        ret = libspdm_asn1_get_tag(&ptr, end, &obj_len,
                                   LIBSPDM_CRYPTO_ASN1_SEQUENCE | LIBSPDM_CRYPTO_ASN1_CONSTRUCTED);
        if (!ret) {
            return false;
        }
        ret = libspdm_asn1_get_tag(&ptr, end, &obj_len, LIBSPDM_CRYPTO_ASN1_OID);
        if (!ret) {
            return false;
        }

        /*get ecc second oid*/
        ptr +=obj_len;
        ret = libspdm_asn1_get_tag(&ptr, end, &obj_len, LIBSPDM_CRYPTO_ASN1_OID);
        if (!ret) {
            return false;
        }

        if (oid_size != obj_len) {
            return false;
        }

        libspdm_copy_mem(oid, oid_size, ptr, obj_len);
        break;
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED25519:
    case SPDM_ALGORITHMS_BASE_ASYM_ALGO_EDDSA_ED448:
        ret = libspdm_asn1_get_tag(&ptr, end, &obj_len,
                                   LIBSPDM_CRYPTO_ASN1_SEQUENCE | LIBSPDM_CRYPTO_ASN1_CONSTRUCTED);
        if (!ret) {
            return false;
        }

        /*get eddsa oid*/
        ret = libspdm_asn1_get_tag(&ptr, end, &obj_len, LIBSPDM_CRYPTO_ASN1_OID);
        if (!ret) {
            return false;
        }

        if (oid_size != obj_len) {
            return false;
        }

        libspdm_copy_mem(oid, oid_size, ptr, obj_len);
        break;
    default:
        LIBSPDM_ASSERT(false);
        return false;
    }

    return true;
}

/**
 * Verify cert public key encryption algorithm is matched to negotiated base_aysm algo
 *
 * @param[in]  cert                  Pointer to the DER-encoded certificate data.
 * @param[in]  cert_size             The size of certificate data in bytes.
 * @param[in]  base_asym_algo        SPDM base_asym_algo
 *
 * @retval  true   verify pass
 * @retval  false  verify fail
 **/
static bool libspdm_verify_cert_subject_public_key_info(const uint8_t *cert, size_t cert_size,
                                                        uint32_t base_asym_algo)
{
    size_t oid_len;
    bool status;

    /*public key encrypt algo OID from cert*/
    uint8_t cert_public_key_crypt_algo_oid[LIBSPDM_MAX_ENCRYPTION_ALGO_OID_LEN];
    /*public key encrypt algo OID from libspdm stored*/
    uint8_t libspdm_public_key_crypt_algo_oid[LIBSPDM_MAX_ENCRYPTION_ALGO_OID_LEN];
    uint8_t libspdm_public_key_crypt_algo_oid_other[LIBSPDM_MAX_ENCRYPTION_ALGO_OID_LEN];

    libspdm_zero_mem(libspdm_public_key_crypt_algo_oid, LIBSPDM_MAX_ENCRYPTION_ALGO_OID_LEN);
    libspdm_zero_mem(libspdm_public_key_crypt_algo_oid_other, LIBSPDM_MAX_ENCRYPTION_ALGO_OID_LEN);

    /*work around: skip the sm2*/
    if (base_asym_algo == SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_SM2_ECC_SM2_P256) {
        return true;
    }

    oid_len = libspdm_get_public_key_algo_OID_len(base_asym_algo);
    if(oid_len == 0) {
        return false;
    }
    /*get public key encrypt algo OID from libspdm stored*/
    status = libspdm_get_public_key_algo_OID(base_asym_algo,
                                             libspdm_public_key_crypt_algo_oid,
                                             libspdm_public_key_crypt_algo_oid_other);
    if (!status) {
        return status;
    }

    /*get public key encrypt algo OID from cert*/
    status = libspdm_get_public_key_oid(cert, cert_size, cert_public_key_crypt_algo_oid, oid_len,
                                        base_asym_algo);
    if (!status || (!libspdm_consttime_is_mem_equal(cert_public_key_crypt_algo_oid,
                                                    libspdm_public_key_crypt_algo_oid, oid_len) &&
                    !libspdm_consttime_is_mem_equal(cert_public_key_crypt_algo_oid,
                                                    libspdm_public_key_crypt_algo_oid_other,
                                                    oid_len))) {
        return false;
    }

    return status;
}

/**
 * Verify leaf cert basic_constraints CA is false
 *
 * @param[in]  cert                  Pointer to the DER-encoded certificate data.
 * @param[in]  cert_size             The size of certificate data in bytes.
 *
 * @retval  true   verify pass,two case: 1.basic constraints is not present in cert;
 *                                       2. cert basic_constraints CA is false;
 * @retval  false  verify fail
 **/
static bool libspdm_verify_leaf_cert_basic_constraints(const uint8_t *cert, size_t cert_size)
{
    bool status;
    /*basic_constraints from cert*/
    uint8_t cert_basic_constraints[LIBSPDM_MAX_BASIC_CONSTRAINTS_CA_LEN];
    size_t len;

    uint8_t basic_constraints_false_case1[] = BASIC_CONSTRAINTS_STRING_FALSE_CASE1;
    uint8_t basic_constraints_false_case2[] = BASIC_CONSTRAINTS_STRING_FALSE_CASE2;

    len = LIBSPDM_MAX_BASIC_CONSTRAINTS_CA_LEN;

    status = libspdm_x509_get_extended_basic_constraints(cert, cert_size,
                                                         cert_basic_constraints, &len);

    if (len == 0) {
        /* basic constraints is not present in cert */
        return true;
    } else if (!status ) {
        return false;
    }

    if ((len == sizeof(basic_constraints_false_case1)) &&
        (libspdm_consttime_is_mem_equal(cert_basic_constraints,
                                        basic_constraints_false_case1,
                                        sizeof(basic_constraints_false_case1)))) {
        return true;
    }

    if ((len == sizeof(basic_constraints_false_case2)) &&
        (libspdm_consttime_is_mem_equal(cert_basic_constraints,
                                        basic_constraints_false_case2,
                                        sizeof(basic_constraints_false_case2)))) {
        return true;
    }

    return false;
}

/**
 * Verify leaf cert basic_constraints CA is false
 *
 * @param[in]  cert                  Pointer to the DER-encoded certificate data.
 * @param[in]  cert_size             The size of certificate data in bytes.
 *
 * @retval  true   verify pass,two case: 1.basic constraints is not present in cert;
 *                                       2. cert basic_constraints CA is false;
 * @retval  false  verify fail
 **/
static bool libspdm_verify_set_cert_leaf_cert_basic_constraints(
    const uint8_t *cert, size_t cert_size, bool is_device_cert_model)
{
    bool status;
    /*basic_constraints from cert*/
    uint8_t cert_basic_constraints[LIBSPDM_MAX_BASIC_CONSTRAINTS_CA_LEN];
    size_t len;

    uint8_t basic_constraints_false_case1[] = BASIC_CONSTRAINTS_STRING_FALSE_CASE1;
    uint8_t basic_constraints_false_case2[] = BASIC_CONSTRAINTS_STRING_FALSE_CASE2;
    uint8_t basic_constraints_true_case[] = BASIC_CONSTRAINTS_STRING_TRUE_CASE;

    len = LIBSPDM_MAX_BASIC_CONSTRAINTS_CA_LEN;

    status = libspdm_x509_get_extended_basic_constraints(cert, cert_size,
                                                         cert_basic_constraints, &len);

    if (is_device_cert_model) {
        /*device cert model*/
        if (len == 0) {
            /* basic constraints is not present in cert */
            return true;
        } else if (!status ) {
            return false;
        }

        if ((len == sizeof(basic_constraints_false_case1)) &&
            (libspdm_consttime_is_mem_equal(cert_basic_constraints,
                                            basic_constraints_false_case1,
                                            sizeof(basic_constraints_false_case1)))) {
            return true;
        }

        if ((len == sizeof(basic_constraints_false_case2)) &&
            (libspdm_consttime_is_mem_equal(cert_basic_constraints,
                                            basic_constraints_false_case2,
                                            sizeof(basic_constraints_false_case2)))) {
            return true;
        }
    } else {
        /*alias cert model*/
        if (status && (len == sizeof(basic_constraints_true_case)) &&
            (libspdm_consttime_is_mem_equal(cert_basic_constraints,
                                            basic_constraints_true_case,
                                            sizeof(basic_constraints_true_case)))) {
            return true;
        }
    }

    return false;
}

/**
 * Verify leaf cert spdm defined extended key usage
 *
 * @param[in]  cert                  Pointer to the DER-encoded certificate data.
 * @param[in]  cert_size             The size of certificate data in bytes.
 * @param[in]  is_requester_cert     Is the function verifying requester or responder cert.
 *
 * @retval  true   verify pass, two cases:
 *                 1. spdm defined eku is not present in cert;
 *                 2. spdm defined eku is compliant with requester/responder identity;
 * @retval  false  verify fail, two cases:
 *                 1. requester's cert has only responder auth oid in eku;
 *                 2. responder's cert has only requester auth oid in eku;
 **/
static bool libspdm_verify_leaf_cert_spdm_eku(const uint8_t *cert, size_t cert_size,
                                              bool is_requester_cert)
{
    bool status;
    uint8_t eku[256];
    size_t eku_size;
    size_t index;
    bool req_auth_oid_find_success;
    bool rsp_auth_oid_find_success;

    /* SPDM defined OID */
    uint8_t eku_requester_auth_oid[] = SPDM_OID_DMTF_EKU_REQUESTER_AUTH;
    uint8_t eku_responder_auth_oid[] = SPDM_OID_DMTF_EKU_RESPONDER_AUTH;

    eku_size = sizeof(eku);
    status = libspdm_x509_get_extended_key_usage(cert, cert_size, eku, &eku_size);
    if (eku_size == 0) {
        /* eku is not present in cert */
        return true;
    } else if (!status ) {
        return false;
    }

    req_auth_oid_find_success = false;
    rsp_auth_oid_find_success = false;

    for(index = 0; index <= eku_size - sizeof(eku_requester_auth_oid); index++) {
        if (libspdm_consttime_is_mem_equal(eku + index, eku_requester_auth_oid,
                                           sizeof(eku_requester_auth_oid))) {
            req_auth_oid_find_success = true;
            break;
        }
    }

    for(index = 0; index <= eku_size - sizeof(eku_responder_auth_oid); index++) {
        if (libspdm_consttime_is_mem_equal(eku + index, eku_responder_auth_oid,
                                           sizeof(eku_responder_auth_oid))) {
            rsp_auth_oid_find_success = true;
            break;
        }
    }

    if (is_requester_cert) {
        /* it should not only contain responder auth oid */
        if (!req_auth_oid_find_success && rsp_auth_oid_find_success) {
            return false;
        }
    } else {
        /* it should not only contain requester auth oid */
        if (req_auth_oid_find_success && !rsp_auth_oid_find_success) {
            return false;
        }
    }

    return true;
}

/**
 * Verify leaf cert spdm defined extension
 *
 * @param[in]  cert                  Pointer to the DER-encoded certificate data.
 * @param[in]  cert_size             The size of certificate data in bytes.
 * @param[in]  is_requester_cert     Is the function verifying requester or responder cert.
 *
 * @retval  true   verify pass
 * @retval  false  verify fail,two case: 1. return is not RETURN_SUCCESS or RETURN_NOT_FOUND;
 *                                       2. hardware_identity_oid is found in AliasCert model;
 **/
static bool libspdm_verify_leaf_cert_spdm_extension(const uint8_t *cert, size_t cert_size,
                                                    bool is_requester_cert,
                                                    bool is_device_cert_model)
{
    bool status;
    bool find_sucessful;
    uint8_t spdm_extension[LIBSPDM_MAX_EXTENSION_LEN];
    size_t index;
    size_t len;

    /* SPDM defined OID */

    uint8_t oid_spdm_extension[] = SPDM_OID_DMTF_SPDM_EXTENSION;
    uint8_t hardware_identity_oid[] = SPDM_OID_DMTF_HARDWARE_IDENTITY;

    len = LIBSPDM_MAX_EXTENSION_LEN;

    if (cert == NULL || cert_size == 0) {
        return false;
    }

    status = libspdm_x509_get_extension_data(cert, cert_size,
                                             (const uint8_t *)oid_spdm_extension,
                                             sizeof(oid_spdm_extension),
                                             spdm_extension,
                                             &len);

    if(len == 0) {
        return true;
    } else if(!status) {
        return false;
    }

    /*find the spdm hardware identity OID*/
    find_sucessful = false;
    for(index = 0; index <= len - sizeof(hardware_identity_oid); index++) {
        if (libspdm_consttime_is_mem_equal(spdm_extension + index, hardware_identity_oid,
                                           sizeof(hardware_identity_oid))) {
            find_sucessful = true;
            break;
        }
    }

    /* Responder does not determine Requester's certificate model */
    if (!is_requester_cert) {
        if ((find_sucessful) && (!is_device_cert_model)) {
            /* Hardware_identity_OID is found in alias cert model */
            return false;
        }
    }

    return true;
}

/**
 * Certificate common Check for SPDM leaf cert when get_cert and set_cert.
 *
 * @param[in]  cert                  Pointer to the DER-encoded certificate data.
 * @param[in]  cert_size             The size of certificate data in bytes.
 * @param[in]  base_asym_algo        SPDM base_asym_algo
 * @param[in]  base_hash_algo        SPDM base_hash_algo
 * @param[in]  is_requester_cert     Is the function verifying requester or responder cert.
 * @param[in]  is_device_cert_model  If true, the local endpoint uses the DeviceCert model.
 *                                   If false, the local endpoint uses the AliasCert model.
 *
 * @retval  true   Success.
 * @retval  false  Certificate is not valid.
 **/
bool libspdm_x509_common_certificate_check(const uint8_t *cert, size_t cert_size,
                                           uint32_t base_asym_algo, uint32_t base_hash_algo,
                                           bool is_requester_cert, bool is_device_cert_model)
{
    uint8_t end_cert_from[64];
    size_t end_cert_from_len;
    uint8_t end_cert_to[64];
    size_t end_cert_to_len;
    size_t asn1_buffer_len;
    bool status;
    size_t cert_version;
    size_t value;
    void *context;

    if (cert == NULL || cert_size == 0) {
        return false;
    }

    status = true;
    context = NULL;
    end_cert_from_len = 64;
    end_cert_to_len = 64;

    /* 1. version*/
    cert_version = 0;
    status = libspdm_x509_get_version(cert, cert_size, &cert_version);
    if (!status) {
        goto cleanup;
    }
    if (cert_version != 2) {
        status = false;
        goto cleanup;
    }

    /* 2. serial_number*/
    asn1_buffer_len = 0;
    status = libspdm_x509_get_serial_number(cert, cert_size, NULL, &asn1_buffer_len);
    if (asn1_buffer_len == 0) {
        status = false;
        goto cleanup;
    }

    /* 3. Verify public key algorithm. */
    status = libspdm_verify_cert_subject_public_key_info(cert, cert_size, base_asym_algo);
    if (!status) {
        goto cleanup;
    }

    /* 4. issuer_name*/
    asn1_buffer_len = 0;
    status = libspdm_x509_get_issuer_name(cert, cert_size, NULL, &asn1_buffer_len);
    if (asn1_buffer_len == 0) {
        status = false;
        goto cleanup;
    }

    /* 5. subject_name*/
    asn1_buffer_len = 0;
    status = libspdm_x509_get_subject_name(cert, cert_size, NULL, &asn1_buffer_len);
    if (asn1_buffer_len == 0) {
        status = false;
        goto cleanup;
    }

    /* 6. validaity*/
    status = libspdm_x509_get_validity(cert, cert_size, end_cert_from,
                                       &end_cert_from_len, end_cert_to,
                                       &end_cert_to_len);
    if (!status) {
        goto cleanup;
    }

    status = libspdm_internal_x509_date_time_check(
        end_cert_from, end_cert_from_len, end_cert_to, end_cert_to_len);
    if (!status) {
        goto cleanup;
    }

    /* 7. subject_public_key*/
    status = libspdm_asym_get_public_key_from_x509(base_asym_algo, cert, cert_size, &context);
    if (!status) {
        goto cleanup;
    }

    /* 8. key_usage*/
    value = 0;
    status = libspdm_x509_get_key_usage(cert, cert_size, &value);
    if (!status) {
        goto cleanup;
    }
    if (LIBSPDM_CRYPTO_X509_KU_DIGITAL_SIGNATURE & value) {
        status = true;
    } else {
        status = false;
        goto cleanup;
    }

    /* 9. verify spdm defined extended key usage*/
    status = libspdm_verify_leaf_cert_spdm_eku(cert, cert_size, is_requester_cert);
    if (!status) {
        goto cleanup;
    }

    /* 10. verify spdm defined extension*/
    status = libspdm_verify_leaf_cert_spdm_extension(cert, cert_size,
                                                     is_requester_cert, is_device_cert_model);
    if (!status) {
        goto cleanup;
    }

cleanup:
    libspdm_asym_free(base_asym_algo, context);
    return status;
}

/**
 * Certificate Check for SPDM leaf cert when get_cert command
 *
 * @param[in]  cert                  Pointer to the DER-encoded certificate data.
 * @param[in]  cert_size             The size of certificate data in bytes.
 * @param[in]  base_asym_algo        SPDM base_asym_algo
 * @param[in]  base_hash_algo        SPDM base_hash_algo
 * @param[in]  is_requester_cert     Is the function verifying requester or responder cert.
 * @param[in]  is_device_cert_model  If true, the local endpoint uses the DeviceCert model.
 *                                   If false, the local endpoint uses the AliasCert model.
 *
 * @retval  true   Success.
 * @retval  false  Certificate is not valid
 **/
bool libspdm_x509_certificate_check(const uint8_t *cert, size_t cert_size,
                                    uint32_t base_asym_algo,
                                    uint32_t base_hash_algo,
                                    bool is_requester_cert,
                                    bool is_device_cert_model)
{
    bool status;

    status = libspdm_x509_common_certificate_check(cert, cert_size, base_asym_algo,
                                                   base_hash_algo, is_requester_cert,
                                                   is_device_cert_model);
    if (!status) {
        return false;
    }

    /* verify basic constraints: the leaf cert always is ca:fasle in get_cert*/
    status = libspdm_verify_leaf_cert_basic_constraints(cert, cert_size);
    return status;
}

/**
 * Certificate Check for SPDM leaf cert when set_cert command.
 *
 * @param[in]  cert                  Pointer to the DER-encoded certificate data.
 * @param[in]  cert_size             The size of certificate data in bytes.
 * @param[in]  base_asym_algo        SPDM base_asym_algo
 * @param[in]  base_hash_algo        SPDM base_hash_algo
 * @param[in]  is_requester_cert     Is the function verifying requester or responder cert.
 * @param[in]  is_device_cert_model  If true, the local endpoint uses the DeviceCert model.
 *                                   If false, the local endpoint uses the AliasCert model.
 *
 * @retval  true   Success.
 * @retval  false  Certificate is not valid.
 **/
bool libspdm_x509_set_cert_certificate_check(const uint8_t *cert, size_t cert_size,
                                             uint32_t base_asym_algo, uint32_t base_hash_algo,
                                             bool is_requester_cert, bool is_device_cert_model)
{
    bool status;

    status = libspdm_x509_common_certificate_check(cert, cert_size, base_asym_algo,
                                                   base_hash_algo, is_requester_cert,
                                                   is_device_cert_model);
    if (!status) {
        return false;
    }

    /* verify basic constraints: need check with is_device_cert_model*/
    status = libspdm_verify_set_cert_leaf_cert_basic_constraints(
        cert, cert_size, is_device_cert_model);
    return status;
}

/**
 * Return certificate is root cert or not.
 * Certificate is considered as a root certificate if the subjectname equal issuername.
 *
 * @param[in]  cert            Pointer to the DER-encoded certificate data.
 * @param[in]  cert_size        The size of certificate data in bytes.
 *
 * @retval  true   Certificate is self-signed.
 * @retval  false  Certificate is not self-signed.
 **/
bool libspdm_is_root_certificate(const uint8_t *cert, size_t cert_size)
{
    uint8_t issuer_name[LIBSPDM_MAX_NAME_SIZE];
    size_t issuer_name_len;
    uint8_t subject_name[LIBSPDM_MAX_NAME_SIZE];
    size_t subject_name_len;
    bool result;

    if (cert == NULL || cert_size == 0) {
        return false;
    }

    /* 1. issuer_name*/
    issuer_name_len = sizeof(issuer_name);
    result = libspdm_x509_get_issuer_name(cert, cert_size, issuer_name, &issuer_name_len);
    if (!result) {
        return false;
    }

    /* 2. subject_name*/
    subject_name_len = sizeof(subject_name);
    result = libspdm_x509_get_subject_name(cert, cert_size, subject_name, &subject_name_len);
    if (!result) {
        return false;
    }

    if (issuer_name_len != subject_name_len) {
        return false;
    }
    if (!libspdm_consttime_is_mem_equal(issuer_name, subject_name, issuer_name_len)) {
        return false;
    }

    return true;
}

/**
 * Retrieve the SubjectAltName from SubjectAltName Bytes.
 *
 * @param[in]      buffer           Pointer to subjectAltName oct bytes.
 * @param[in]      len              size of buffer in bytes.
 * @param[out]     name_buffer       buffer to contain the retrieved certificate
 *                                 SubjectAltName. At most name_buffer_size bytes will be
 *                                 written. Maybe NULL in order to determine the size
 *                                 buffer needed.
 * @param[in,out]  name_buffer_size   The size in bytes of the name buffer on input,
 *                                 and the size of buffer returned name on output.
 *                                 If name_buffer is NULL then the amount of space needed
 *                                 in buffer (including the final null) is returned.
 * @param[out]     oid              OID of otherName
 * @param[in,out]  oid_size          the buffersize for required OID
 *
 * @retval true                     get the subjectAltName string successfully
 * @retval failed                   get the subjectAltName string failed
 **/
bool libspdm_get_dmtf_subject_alt_name_from_bytes(
    uint8_t *buffer, const size_t len, char *name_buffer,
    size_t *name_buffer_size, uint8_t *oid,
    size_t *oid_size)
{
    uint8_t *ptr;
    int32_t length;
    size_t obj_len;
    int32_t ret;

    /*copy mem variable*/
    volatile uint8_t* dst;
    const volatile uint8_t* src;
    size_t dst_len;
    size_t src_len;

    length = (int32_t)len;
    ptr = buffer;
    obj_len = 0;

    /* Sequence*/
    ret = libspdm_asn1_get_tag(&ptr, ptr + length, &obj_len,
                               LIBSPDM_CRYPTO_ASN1_SEQUENCE | LIBSPDM_CRYPTO_ASN1_CONSTRUCTED);
    if (!ret) {
        return false;
    }

    ret = libspdm_asn1_get_tag(&ptr, ptr + obj_len, &obj_len,
                               LIBSPDM_CRYPTO_ASN1_CONTEXT_SPECIFIC |
                               LIBSPDM_CRYPTO_ASN1_CONSTRUCTED);

    ret = libspdm_asn1_get_tag(&ptr, ptr + obj_len, &obj_len, LIBSPDM_CRYPTO_ASN1_OID);
    if (!ret) {
        return false;
    }
    /* CopyData to OID*/
    if (*oid_size < (size_t)obj_len) {
        *oid_size = (size_t)obj_len;
        return false;
    }
    if (oid != NULL) {
        libspdm_copy_mem(oid, *oid_size, ptr, obj_len);
        *oid_size = obj_len;
    }

    /* Move to next element*/
    ptr += obj_len;

    ret = libspdm_asn1_get_tag(&ptr, (uint8_t *)(buffer + length), &obj_len,
                               LIBSPDM_CRYPTO_ASN1_CONTEXT_SPECIFIC |
                               LIBSPDM_CRYPTO_ASN1_CONSTRUCTED);
    ret = libspdm_asn1_get_tag(&ptr, (uint8_t *)(buffer + length), &obj_len,
                               LIBSPDM_CRYPTO_ASN1_UTF8_STRING);
    if (!ret) {
        return false;
    }

    if (*name_buffer_size < (size_t)obj_len + 1) {
        *name_buffer_size = (size_t)obj_len + 1;
        return false;
    }

    /* the src and dst adress are overlap,
    * When the function is called by libspdm_get_dmtf_subject_alt_name.
    * libspdm_copy_mem can not be uesed */
    if ((name_buffer != NULL) && (ptr != NULL)) {
        dst = (volatile uint8_t*) name_buffer;
        src = (const volatile uint8_t*) ptr;
        dst_len = *name_buffer_size;
        src_len = obj_len;

        /* Check for case where "dst_len" may be invalid. Do not zero "dst" in this case. */
        if (dst_len > (SIZE_MAX >> 1)) {
            LIBSPDM_ASSERT(0);
            return false;
        }

        /* Guard against invalid lengths. Zero "dst" in these cases. */
        if (src_len > dst_len ||
            src_len > (SIZE_MAX >> 1)) {
            libspdm_zero_mem(name_buffer, dst_len);
            LIBSPDM_ASSERT(0);
            return false;
        }

        while (src_len-- != 0) {
            *(dst++) = *(src++);
        }

        /*encode name buffer to string*/
        *name_buffer_size = obj_len + 1;
        name_buffer[obj_len] = 0;
        return true;
    }

    return false;
}

/**
 * Retrieve the SubjectAltName from one X.509 certificate.
 *
 * @param[in]      cert             Pointer to the DER-encoded X509 certificate.
 * @param[in]      cert_size         size of the X509 certificate in bytes.
 * @param[out]     name_buffer       buffer to contain the retrieved certificate
 *                                 SubjectAltName. At most name_buffer_size bytes will be
 *                                 written. Maybe NULL in order to determine the size
 *                                 buffer needed.
 * @param[in,out]  name_buffer_size   The size in bytes of the name buffer on input,
 *                                 and the size of buffer returned name on output.
 *                                 If name_buffer is NULL then the amount of space needed
 *                                 in buffer (including the final null) is returned.
 * @param[out]     oid              OID of otherName
 * @param[in,out]  oid_size          the buffersize for required OID
 *
 * @retval true                     get the subjectAltName string successfully
 * @retval failed                   get the subjectAltName string failed
 **/
bool libspdm_get_dmtf_subject_alt_name(const uint8_t *cert, const size_t cert_size,
                                       char *name_buffer,
                                       size_t *name_buffer_size,
                                       uint8_t *oid, size_t *oid_size)
{
    bool status;
    size_t extension_data_size;
    uint8_t oid_subject_alt_name[] = { 0x55, 0x1D, 0x11 };

    extension_data_size = 0;
    status = libspdm_x509_get_extension_data(cert, cert_size,
                                             oid_subject_alt_name,
                                             sizeof(oid_subject_alt_name), NULL,
                                             &extension_data_size);
    if (status || (extension_data_size == 0)) {
        *name_buffer_size = 0;
        return false;
    }
    if (extension_data_size > *name_buffer_size) {
        *name_buffer_size = extension_data_size;
        return false;
    }
    status =
        libspdm_x509_get_extension_data(cert, cert_size,
                                        oid_subject_alt_name,
                                        sizeof(oid_subject_alt_name),
                                        (uint8_t *)name_buffer, name_buffer_size);
    if (!status) {
        return status;
    }

    return libspdm_get_dmtf_subject_alt_name_from_bytes(
        (uint8_t *)name_buffer, *name_buffer_size, name_buffer,
        name_buffer_size, oid, oid_size);
}

/**
 * This function verifies the integrity of certificate chain data without spdm_cert_chain_t header.
 *
 * @param  cert_chain_data          The certificate chain data without spdm_cert_chain_t header.
 * @param  cert_chain_data_size      size in bytes of the certificate chain data.
 * @param  base_asym_algo            SPDM base_asym_algo
 * @param  base_hash_algo            SPDM base_hash_algo
 * @param  is_requester_cert         Is the function verifying requester or responder cert.
 * @param  is_device_cert_model      If true, the cert chain is DeviceCert model;
 *                                   If false, the cert chain is AliasCert model;
 *
 * @retval true  certificate chain data integrity verification pass.
 * @retval false certificate chain data integrity verification fail.
 **/
bool libspdm_verify_cert_chain_data(uint8_t *cert_chain_data, size_t cert_chain_data_size,
                                    uint32_t base_asym_algo, uint32_t base_hash_algo,
                                    bool is_requester_cert, bool is_device_cert_model)
{
    const uint8_t *root_cert_buffer;
    size_t root_cert_buffer_size;
    const uint8_t *leaf_cert_buffer;
    size_t leaf_cert_buffer_size;

    if (cert_chain_data_size >
        0xFFFF - (sizeof(spdm_cert_chain_t) + LIBSPDM_MAX_HASH_SIZE)) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO,
                       "!!! VerifyCertificateChainData - FAIL (chain size too large) !!!\n"));
        return false;
    }

    if (!libspdm_x509_get_cert_from_cert_chain(
            cert_chain_data, cert_chain_data_size, 0, &root_cert_buffer,
            &root_cert_buffer_size)) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO,
                       "!!! VerifyCertificateChainData - FAIL (get root certificate failed)!!!\n"));
        return false;
    }

    if (!libspdm_x509_verify_cert_chain(root_cert_buffer, root_cert_buffer_size,
                                        cert_chain_data, cert_chain_data_size)) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO,
                       "!!! VerifyCertificateChainData - FAIL (cert chain verify failed)!!!\n"));
        return false;
    }

    if (!libspdm_x509_get_cert_from_cert_chain(
            cert_chain_data, cert_chain_data_size, -1,
            &leaf_cert_buffer, &leaf_cert_buffer_size)) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO,
                       "!!! VerifyCertificateChainData - FAIL (get leaf certificate failed)!!!\n"));
        return false;
    }

    if (!libspdm_x509_certificate_check(leaf_cert_buffer, leaf_cert_buffer_size,
                                        base_asym_algo, base_hash_algo,
                                        is_requester_cert, is_device_cert_model)) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO,
                       "!!! VerifyCertificateChainData - FAIL (leaf certificate check failed)!!!\n"));
        return false;
    }

    return true;
}

bool libspdm_verify_certificate_chain_buffer(uint32_t base_hash_algo, uint32_t base_asym_algo,
                                             const void *cert_chain_buffer,
                                             size_t cert_chain_buffer_size,
                                             bool is_requester_cert,
                                             bool is_device_cert_model)
{
    const uint8_t *cert_chain_data;
    size_t cert_chain_data_size;
    const uint8_t *first_cert_buffer;
    size_t first_cert_buffer_size;
    size_t hash_size;
    uint8_t calc_root_cert_hash[LIBSPDM_MAX_HASH_SIZE];
    const uint8_t *leaf_cert_buffer;
    size_t leaf_cert_buffer_size;
    bool result;
    const spdm_cert_chain_t *cert_chain_header;

    hash_size = libspdm_get_hash_size(base_hash_algo);

    if (cert_chain_buffer_size <= sizeof(spdm_cert_chain_t) + hash_size) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO,
                       "!!! VerifyCertificateChainBuffer - FAIL (buffer too small) !!!\n"));
        return false;
    }

    cert_chain_header = cert_chain_buffer;
    if (cert_chain_header->length != cert_chain_buffer_size) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO,
                       "!!! VerifyCertificateChainBuffer - FAIL (cert_chain->length mismatch) !!!\n"));
        return false;
    }

    cert_chain_data = (const uint8_t *)cert_chain_buffer + sizeof(spdm_cert_chain_t) + hash_size;
    cert_chain_data_size = cert_chain_buffer_size - sizeof(spdm_cert_chain_t) - hash_size;
    if (!libspdm_x509_get_cert_from_cert_chain(
            cert_chain_data, cert_chain_data_size, 0, &first_cert_buffer,
            &first_cert_buffer_size)) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO,
                       "!!! VerifyCertificateChainBuffer - FAIL (get root certificate failed)!!!\n"));
        return false;
    }

    if (libspdm_is_root_certificate(first_cert_buffer, first_cert_buffer_size)) {
        result = libspdm_hash_all(base_hash_algo, first_cert_buffer, first_cert_buffer_size,
                                  calc_root_cert_hash);
        if (!result) {
            LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO,
                           "!!! VerifyCertificateChainBuffer - FAIL (hash calculation fail) !!!\n"));
            return false;
        }
        if (!libspdm_consttime_is_mem_equal((const uint8_t *)cert_chain_buffer +
                                            sizeof(spdm_cert_chain_t),
                                            calc_root_cert_hash, hash_size)) {
            LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO,
                           "!!! VerifyCertificateChainBuffer - FAIL (cert root hash mismatch) !!!\n"));
            return false;
        }
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO,
                       "!!! VerifyCertificateChainBuffer - PASS (cert root hash match) !!!\n"));
    }

    /*If the number of certificates in the certificate chain is more than 1,
     * other certificates need to be verified.*/
    if (cert_chain_data_size > first_cert_buffer_size) {
        if (!libspdm_x509_verify_cert_chain(first_cert_buffer, first_cert_buffer_size,
                                            cert_chain_data + first_cert_buffer_size,
                                            cert_chain_data_size - first_cert_buffer_size)) {
            LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO,
                           "!!! VerifyCertificateChainBuffer - FAIL (cert chain verify failed)!!!\n"));
            return false;
        }
    }

    if (!libspdm_x509_get_cert_from_cert_chain(
            cert_chain_data, cert_chain_data_size, -1,
            &leaf_cert_buffer, &leaf_cert_buffer_size)) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO,
                       "!!! VerifyCertificateChainBuffer - FAIL (get leaf certificate failed)!!!\n"));
        return false;
    }

    if (!libspdm_x509_certificate_check(leaf_cert_buffer, leaf_cert_buffer_size,
                                        base_asym_algo, base_hash_algo,
                                        is_requester_cert, is_device_cert_model)) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_INFO,
                       "!!! VerifyCertificateChainBuffer - FAIL (leaf certificate check failed)!!!\n"));
        return false;
    }

    return true;
}

bool libspdm_get_leaf_cert_public_key_from_cert_chain(uint32_t base_hash_algo,
                                                      uint32_t base_asym_alg,
                                                      uint8_t *cert_chain_data,
                                                      size_t cert_chain_data_size,
                                                      void **public_key)
{
    size_t hash_size;
    const uint8_t *cert_buffer;
    size_t cert_buffer_size;
    bool result;

    hash_size = libspdm_get_hash_size(base_hash_algo);

    cert_chain_data = cert_chain_data + sizeof(spdm_cert_chain_t) + hash_size;
    cert_chain_data_size = cert_chain_data_size - (sizeof(spdm_cert_chain_t) + hash_size);

    /* Get leaf cert from cert chain */
    result = libspdm_x509_get_cert_from_cert_chain(cert_chain_data,
                                                   cert_chain_data_size, -1,
                                                   &cert_buffer, &cert_buffer_size);
    if (!result) {
        return false;
    }

    result = libspdm_asym_get_public_key_from_x509(
        base_asym_alg,
        cert_buffer, cert_buffer_size, public_key);
    if (!result) {
        return false;
    }

    return true;
}

bool libspdm_verify_req_info(uint8_t *req_info, uint16_t req_info_len)
{
    bool ret;
    uint8_t *ptr;
    int32_t length;
    size_t obj_len;
    uint8_t *end;

    length = (int32_t)req_info_len;
    ptr = req_info;
    obj_len = 0;
    end = ptr + length;
    ret = true;

    if (req_info_len == 0) {
        return true;
    }

    /*req_info sequence*/
    ret = libspdm_asn1_get_tag(&ptr, end, &obj_len,
                               LIBSPDM_CRYPTO_ASN1_SEQUENCE | LIBSPDM_CRYPTO_ASN1_CONSTRUCTED);
    if (!ret) {
        return false;
    }

    /*integer:version*/
    ret = libspdm_asn1_get_tag(&ptr, end, &obj_len, LIBSPDM_CRYPTO_ASN1_INTEGER);
    if (!ret) {
        return false;
    } else {
        ptr += obj_len;
    }

    /*sequence:subject name*/
    ret = libspdm_asn1_get_tag(&ptr, end, &obj_len,
                               LIBSPDM_CRYPTO_ASN1_SEQUENCE | LIBSPDM_CRYPTO_ASN1_CONSTRUCTED);
    if (!ret) {
        return false;
    } else {
        ptr += obj_len;
    }

    /*sequence:subject pkinfo*/
    ret = libspdm_asn1_get_tag(&ptr, end, &obj_len,
                               LIBSPDM_CRYPTO_ASN1_SEQUENCE | LIBSPDM_CRYPTO_ASN1_CONSTRUCTED);
    if (!ret) {
        return false;
    } else {
        ptr += obj_len;
    }

    /*[0]: attributes*/
    ret = libspdm_asn1_get_tag(&ptr, end, &obj_len,
                               LIBSPDM_CRYPTO_ASN1_CONTEXT_SPECIFIC |
                               LIBSPDM_CRYPTO_ASN1_CONSTRUCTED);
    /*req_info format error, don't have attributes tag*/
    if (!ret) {
        return false;
    }

    /*there is no attributes object*/
    if (ptr == end) {
        return true;
    }

    /*there is some attributes object: 0,1,2 ...*/
    while (ret)
    {
        ret = libspdm_asn1_get_tag(&ptr, end, &obj_len,
                                   LIBSPDM_CRYPTO_ASN1_SEQUENCE |
                                   LIBSPDM_CRYPTO_ASN1_CONSTRUCTED);
        if (ret) {
            ptr += obj_len;
        } else {
            break;
        }
    }

    if (ptr == end) {
        return true;
    } else {
        return false;
    }
}

#endif
