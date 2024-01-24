/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#ifndef LIBSPDM_MACRO_CHECK_H
#define LIBSPDM_MACRO_CHECK_H

#define LIBSPDM_ASYM_ALGO_SUPPORT \
    ((LIBSPDM_RSA_SSA_SUPPORT) || (LIBSPDM_RSA_PSS_SUPPORT) || (LIBSPDM_ECDSA_SUPPORT) || \
     (LIBSPDM_SM2_DSA_SUPPORT) || (LIBSPDM_EDDSA_SUPPORT))

#define LIBSPDM_DHE_ALGO_SUPPORT \
    ((LIBSPDM_FFDHE_SUPPORT) || (LIBSPDM_ECDHE_SUPPORT) || (LIBSPDM_SM2_KEY_EXCHANGE_SUPPORT))

#define LIBSPDM_AEAD_ALGO_SUPPORT \
    ((LIBSPDM_AEAD_GCM_SUPPORT) || (LIBSPDM_AEAD_CHACHA20_POLY1305_SUPPORT) || \
     (LIBSPDM_AEAD_SM4_SUPPORT))

#define LIBSPDM_HASH_ALGO_SUPPORT \
    ((LIBSPDM_SHA2_SUPPORT) || (LIBSPDM_SHA3_SUPPORT) || \
     (LIBSPDM_SM3_SUPPORT))

#if LIBSPDM_FIPS_MODE
#define LIBSPDM_FIPS_ASYM_ALGO_SUPPORT \
    ((LIBSPDM_RSA_SSA_SUPPORT) || (LIBSPDM_RSA_PSS_SUPPORT) || (LIBSPDM_ECDSA_SUPPORT) || \
     (LIBSPDM_EDDSA_SUPPORT))

#define LIBSPDM_FIPS_DHE_ALGO_SUPPORT \
    ((LIBSPDM_FFDHE_SUPPORT) || (LIBSPDM_ECDHE_SUPPORT))

#define LIBSPDM_FIPS_AEAD_ALGO_SUPPORT (LIBSPDM_AEAD_GCM_SUPPORT)

#define LIBSPDM_FIPS_HASH_ALGO_SUPPORT \
    ((LIBSPDM_SHA2_SUPPORT) || (LIBSPDM_SHA3_SUPPORT))
#endif /*LIBSPDM_FIPS_MODE*/

#if (LIBSPDM_ENABLE_CAPABILITY_KEY_EX_CAP) && !LIBSPDM_ASYM_ALGO_SUPPORT
    #error If KEY_EX_CAP is enabled then at least one asymmetric algorithm must also be enabled.
#endif

#if (LIBSPDM_ENABLE_CAPABILITY_CERT_CAP) && !LIBSPDM_ASYM_ALGO_SUPPORT
    #error If CERT_CAP is enabled then at least one asymmetric algorithm must also be enabled.
#endif

#if (LIBSPDM_ENABLE_CAPABILITY_CHAL_CAP) && !LIBSPDM_ASYM_ALGO_SUPPORT
    #error If CHAL_CAP is enabled then at least one asymmetric algorithm must also be enabled.
#endif

#if (LIBSPDM_ENABLE_CAPABILITY_KEY_EX_CAP) && !LIBSPDM_DHE_ALGO_SUPPORT
    #error If KEY_EX_CAP is enabled then at least one DHE algorithm must also be enabled.
#endif

#if (LIBSPDM_ENABLE_CAPABILITY_KEY_EX_CAP) && !LIBSPDM_AEAD_ALGO_SUPPORT
    #error If KEY_EX_CAP is enabled then at least one AEAD algorithm must also be enabled.
#endif

#if (LIBSPDM_ENABLE_CAPABILITY_PSK_CAP) && !LIBSPDM_AEAD_ALGO_SUPPORT
    #error If PSK_CAP is enabled then at least one AEAD algorithm must also be enabled.
#endif

#if (LIBSPDM_ENABLE_CAPABILITY_CERT_CAP) && !LIBSPDM_HASH_ALGO_SUPPORT
    #error If CERT_CAP is enabled then at least one HASH algorithm must also be enabled.
#endif

#if (LIBSPDM_ENABLE_CAPABILITY_CHAL_CAP) && !LIBSPDM_HASH_ALGO_SUPPORT
    #error If CHAL_CAP is enabled then at least one HASH algorithm must also be enabled.
#endif

#if (LIBSPDM_ENABLE_CAPABILITY_KEY_EX_CAP) && !LIBSPDM_HASH_ALGO_SUPPORT
    #error If KEY_EX_CAP is enabled then at least one HASH algorithm must also be enabled.
#endif

#if (LIBSPDM_ENABLE_CAPABILITY_PSK_CAP) && !LIBSPDM_HASH_ALGO_SUPPORT
    #error If PSK_CAP is enabled then at least one HASH algorithm must also be enabled.
#endif

#if ((LIBSPDM_MAX_VERSION_COUNT) == 0) || ((LIBSPDM_MAX_VERSION_COUNT) > 255)
    #error LIBSPDM_MAX_VERSION_COUNT must be between 1 and 255 inclusive.
#endif

#if (LIBSPDM_MAX_SESSION_COUNT) > 65535
    #error LIBSPDM_MAX_SESSION_COUNT must be less than 65536.
#endif

#if LIBSPDM_FIPS_MODE
#if (LIBSPDM_ASYM_ALGO_SUPPORT) && !LIBSPDM_FIPS_ASYM_ALGO_SUPPORT
    #error ASYM algo is cleared after FIPS enforcement.
#endif

#if (LIBSPDM_DHE_ALGO_SUPPORT) && !LIBSPDM_FIPS_DHE_ALGO_SUPPORT
    #error DHE algo is cleared after FIPS enforcement.
#endif

#if (LIBSPDM_AEAD_ALGO_SUPPORT) && !LIBSPDM_FIPS_AEAD_ALGO_SUPPORT
    #error AEAD algo is cleared after FIPS enforcement.
#endif

#if (LIBSPDM_HASH_ALGO_SUPPORT) && !LIBSPDM_FIPS_HASH_ALGO_SUPPORT
    #error HASH algo is cleared after FIPS enforcement.
#endif
#endif /*LIBSPDM_FIPS_MODE*/

#undef LIBSPDM_ASYM_ALGO_SUPPORT
#undef LIBSPDM_DHE_ALGO_SUPPORT
#undef LIBSPDM_AEAD_ALGO_SUPPORT
#undef LIBSPDM_HASH_ALGO_SUPPORT

#if LIBSPDM_FIPS_MODE
#undef LIBSPDM_FIPS_ASYM_ALGO_SUPPORT
#undef LIBSPDM_FIPS_DHE_ALGO_SUPPORT
#undef LIBSPDM_FIPS_AEAD_ALGO_SUPPORT
#undef LIBSPDM_FIPS_HASH_ALGO_SUPPORT
#endif /*LIBSPDM_FIPS_MODE*/

#endif /* LIBSPDM_MACRO_CHECK_H */
