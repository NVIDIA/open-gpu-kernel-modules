/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#ifndef LIBSPDM_MACRO_CHECK_H
#define LIBSPDM_MACRO_CHECK_H

#define LIBSPDM_ASYM_ALGO_SUPPORT \
    ((LIBSPDM_RSA_SSA_SUPPORT) || (LIBSPDM_RSA_PSS_SUPPORT) || (LIBSPDM_ECDSA_SUPPORT) || \
     (LIBSPDM_SM2_DSA_SUPPORT) || (LIBSPDM_EDDSA_ED25519_SUPPORT) || (LIBSPDM_EDDSA_ED448_SUPPORT))

#define LIBSPDM_DHE_ALGO_SUPPORT \
    ((LIBSPDM_FFDHE_SUPPORT) || (LIBSPDM_ECDHE_SUPPORT) || (LIBSPDM_SM2_KEY_EXCHANGE_SUPPORT))

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

#if ((LIBSPDM_MAX_VERSION_COUNT) == 0) || ((LIBSPDM_MAX_VERSION_COUNT) > 255)
    #error LIBSPDM_MAX_VERSION_COUNT must be between 1 and 255 inclusive.
#endif

#if (LIBSPDM_MAX_SESSION_COUNT) > 65535
    #error LIBSPDM_MAX_SESSION_COUNT must be less than 65536.
#endif

#undef LIBSPDM_ASYM_ALGO_SUPPORT
#undef LIBSPDM_DHE_ALGO_SUPPORT

#endif /* LIBSPDM_MACRO_CHECK_H */
