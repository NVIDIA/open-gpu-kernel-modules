/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#ifndef CRYPTLIB_EXT_H
#define CRYPTLIB_EXT_H

#include "hal/base.h"
#include "hal/library/cryptlib.h"

/**
 * Retrieve the common name (CN) string from one X.509 certificate.
 *
 * @param[in]      cert              Pointer to the DER-encoded X509 certificate.
 * @param[in]      cert_size         Size of the X509 certificate in bytes.
 * @param[out]     common_name       Buffer to contain the retrieved certificate common
 *                                   name string (UTF8). At most common_name_size bytes will be
 *                                   written and the string will be null terminated. May be
 *                                   NULL in order to determine the size buffer needed.
 * @param[in,out]  common_name_size  The size in bytes of the common_name buffer on input,
 *                                   and the size of buffer returned common_name on output.
 *                                   If common_name is NULL then the amount of space needed
 *                                   in buffer (including the final null) is returned.
 *
 * @retval  true
 * @retval  false
 **/
extern bool libspdm_x509_get_common_name(const uint8_t *cert, size_t cert_size,
                                         char *common_name,
                                         size_t *common_name_size);

/**
 * Retrieve the organization name (O) string from one X.509 certificate.
 *
 * @param[in]      cert              Pointer to the DER-encoded X509 certificate.
 * @param[in]      cert_size         Size of the X509 certificate in bytes.
 * @param[out]     name_buffer       Buffer to contain the retrieved certificate organization
 *                                   name string. At most name_buffer_size bytes will be
 *                                   written and the string will be null terminated. May be
 *                                   NULL in order to determine the size buffer needed.
 * @param[in,out]  name_buffer_size  The size in bytes of the name buffer on input,
 *                                   and the size of buffer returned name on output.
 *                                   If name_buffer is NULL then the amount of space needed
 *                                   in buffer (including the final null) is returned.
 *
 * @retval  true
 * @retval  false
 **/
extern bool libspdm_x509_get_organization_name(const uint8_t *cert, size_t cert_size,
                                               char *name_buffer,
                                               size_t *name_buffer_size);

/**
 * Retrieve the issuer common name (CN) string from one X.509 certificate.
 *
 * @param[in]      cert              Pointer to the DER-encoded X509 certificate.
 * @param[in]      cert_size         Size of the X509 certificate in bytes.
 * @param[out]     common_name       Buffer to contain the retrieved certificate issuer common
 *                                   name string. At most common_name_size bytes will be
 *                                   written and the string will be null terminated. May be
 *                                   NULL in order to determine the size buffer needed.
 * @param[in,out]  common_name_size  The size in bytes of the common_name buffer on input,
 *                                   and the size of buffer returned common_name on output.
 *                                   If common_name is NULL then the amount of space needed
 *                                   in buffer (including the final null) is returned.
 *
 * @retval  true
 * @retval  false
 **/
extern bool libspdm_x509_get_issuer_common_name(const uint8_t *cert, size_t cert_size,
                                                char *common_name,
                                                size_t *common_name_size);

/**
 * Retrieve the issuer organization name (O) string from one X.509 certificate.
 *
 * @param[in]      cert              Pointer to the DER-encoded X509 certificate.
 * @param[in]      cert_size         Size of the X509 certificate in bytes.
 * @param[out]     name_buffer       Buffer to contain the retrieved certificate issuer organization
 *                                   name string. At most name_buffer_size bytes will be
 *                                   written and the string will be null terminated. May be
 *                                   NULL in order to determine the size buffer needed.
 * @param[in,out]  name_buffer_size  The size in bytes of the name buffer on input,
 *                                   and the size of buffer returned name on output.
 *                                   If name_buffer is NULL then the amount of space needed
 *                                   in buffer (including the final null) is returned.
 *
 * @retval  true
 * @retval  false
 **/
extern bool libspdm_x509_get_issuer_orgnization_name(const uint8_t *cert, size_t cert_size,
                                                     char *name_buffer,
                                                     size_t *name_buffer_size);

/**
 * Retrieve the signature algorithm from one X.509 certificate.
 *
 * @param[in]      cert       Pointer to the DER-encoded X509 certificate.
 * @param[in]      cert_size  Size of the X509 certificate in bytes.
 * @param[out]     oid        Signature algorithm Object identifier buffer.
 * @param[in,out]  oid_size   Signature algorithm Object identifier buffer size.
 *
 * @retval  true
 * @retval  false
 **/
extern bool libspdm_x509_get_signature_algorithm(const uint8_t *cert,
                                                 size_t cert_size, uint8_t *oid,
                                                 size_t *oid_size);

/**
 * Construct a X509 object from DER-encoded certificate data.
 *
 * If cert is NULL, then return false.
 * If single_x509_cert is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[in]  cert              Pointer to the DER-encoded certificate data.
 * @param[in]  cert_size         The size of certificate data in bytes.
 * @param[out] single_x509_cert  The generated X509 object.
 *
 * @retval  true   The X509 object generation succeeded.
 * @retval  false  The operation failed.
 * @retval  false  This interface is not supported.
 **/
extern bool libspdm_x509_construct_certificate(const uint8_t *cert, size_t cert_size,
                                               uint8_t **single_x509_cert);

/**
 * Construct a X509 stack object from a list of DER-encoded certificate data.
 *
 * If x509_stack is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[in, out]  x509_stack  On input, pointer to an existing or NULL X509 stack object.
 *                              On output, pointer to the X509 stack object with new
 *                              inserted X509 certificate.
 * @param           ...         A list of DER-encoded single certificate data followed
 *                              by certificate size. A NULL terminates the list. The
 *                              pairs are the arguments to libspdm_x509_construct_certificate().
 *
 * @retval  true   The X509 stack construction succeeded.
 * @retval  false  The construction operation failed.
 * @retval  false  This interface is not supported.
 **/
extern bool libspdm_x509_construct_certificate_stack(uint8_t **x509_stack, ...);

/**
 * Release the specified X509 object.
 *
 * If the interface is not supported, then ASSERT().
 *
 * @param[in]  x509_cert  Pointer to the X509 object to be released.
 **/
extern void libspdm_x509_free(void *x509_cert);

/**
 * Release the specified X509 stack object.
 *
 * If the interface is not supported, then ASSERT().
 *
 * @param[in]  x509_stack  Pointer to the X509 stack object to be released.
 **/
extern void libspdm_x509_stack_free(void *x509_stack);

/**
 * Retrieve the TBSCertificate from one given X.509 certificate.
 *
 * @param[in]      cert         Pointer to the given DER-encoded X509 certificate.
 * @param[in]      cert_size     size of the X509 certificate in bytes.
 * @param[out]     tbs_cert      DER-Encoded to-Be-Signed certificate.
 * @param[out]     tbs_cert_size  size of the TBS certificate in bytes.
 *
 * If cert is NULL, then return false.
 * If tbs_cert is NULL, then return false.
 * If tbs_cert_size is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @retval  true   The TBSCertificate was retrieved successfully.
 * @retval  false  Invalid X.509 certificate.
 **/
extern bool libspdm_x509_get_tbs_cert(const uint8_t *cert, size_t cert_size,
                                      uint8_t **tbs_cert, size_t *tbs_cert_size);

/**
 * Retrieve the RSA Private key from the password-protected PEM key data.
 *
 * If pem_data is NULL, then return false.
 * If rsa_context is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[in]  pem_data     Pointer to the PEM-encoded key data to be retrieved.
 * @param[in]  pem_size     Size of the PEM key data in bytes.
 * @param[in]  password     NULL-terminated passphrase used for encrypted PEM key data.
 * @param[out] rsa_context  Pointer to newly generated RSA context which contain the retrieved
 *                          RSA private key component. Use libspdm_rsa_free() function to free the
 *                          resource.
 *
 * @retval  true   RSA Private key was retrieved successfully.
 * @retval  false  Invalid PEM key data or incorrect password.
 * @retval  false  This interface is not supported.
 **/
extern bool libspdm_rsa_get_private_key_from_pem(const uint8_t *pem_data,
                                                 size_t pem_size,
                                                 const char *password,
                                                 void **rsa_context);

#if (LIBSPDM_RSA_SSA_SUPPORT) || (LIBSPDM_RSA_PSS_SUPPORT)
/**
 * Gets the tag-designated RSA key component from the established RSA context.
 *
 * This function retrieves the tag-designated RSA key component from the
 * established RSA context as a non-negative integer (octet string format
 * represented in RSA PKCS#1).
 * If specified key component has not been set or has been cleared, then returned
 * bn_size is set to 0.
 * If the big_number buffer is too small to hold the contents of the key, false
 * is returned and bn_size is set to the required buffer size to obtain the key.
 *
 * If rsa_context is NULL, then return false.
 * If bn_size is NULL, then return false.
 * If bn_size is large enough but big_number is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[in, out]  rsa_context  Pointer to RSA context being set.
 * @param[in]       key_tag      Tag of RSA key component being set.
 * @param[out]      big_number   Pointer to octet integer buffer.
 * @param[in, out]  bn_size      On input, the size of big number buffer in bytes.
 *                               On output, the size of data returned in big number buffer in bytes.
 *
 * @retval  true   RSA key component was retrieved successfully.
 * @retval  false  Invalid RSA key component tag.
 * @retval  false  bn_size is too small.
 * @retval  false  This interface is not supported.
 **/
extern bool libspdm_rsa_get_key(void *rsa_context, const libspdm_rsa_key_tag_t key_tag,
                                uint8_t *big_number, size_t *bn_size);

/**
 * Validates key components of RSA context.
 * NOTE: This function performs integrity checks on all the RSA key material, so
 *      the RSA key structure must contain all the private key data.
 *
 * This function validates key components of RSA context in following aspects:
 * - Whether p is a prime
 * - Whether q is a prime
 * - Whether n = p * q
 * - Whether d*e = 1  mod lcm(p-1,q-1)
 *
 * If rsa_context is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[in]  rsa_context  Pointer to RSA context to check.
 *
 * @retval  true   RSA key components are valid.
 * @retval  false  RSA key components are not valid.
 * @retval  false  This interface is not supported.
 **/
extern bool libspdm_rsa_check_key(void *rsa_context);

/**
 * Generates RSA key components.
 *
 * This function generates RSA key components. It takes RSA public exponent E and
 * length in bits of RSA modulus N as input, and generates all key components.
 * If public_exponent is NULL, the default RSA public exponent (0x10001) will be used.
 *
 * If rsa_context is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[in, out]  rsa_context           Pointer to RSA context being set.
 * @param[in]       modulus_length        Length of RSA modulus N in bits.
 * @param[in]       public_exponent       Pointer to RSA public exponent.
 * @param[in]       public_exponent_size  Size of RSA public exponent buffer in bytes.
 *
 * @retval  true   RSA key component was generated successfully.
 * @retval  false  Invalid RSA key component tag.
 * @retval  false  This interface is not supported.
 **/
extern bool libspdm_rsa_generate_key(void *rsa_context, size_t modulus_length,
                                     const uint8_t *public_exponent,
                                     size_t public_exponent_size);
#endif /* (LIBSPDM_RSA_SSA_SUPPORT) || (LIBSPDM_RSA_PSS_SUPPORT) */

/**
 * Retrieve the EC Private key from the password-protected PEM key data.
 *
 * @param[in]  pem_data    Pointer to the PEM-encoded key data to be retrieved.
 * @param[in]  pem_size    Size of the PEM key data in bytes.
 * @param[in]  password    NULL-terminated passphrase used for encrypted PEM key data.
 * @param[out] ec_context  Pointer to newly generated EC DSA context which contain the retrieved
 *                         EC private key component. Use libspdm_ec_free() function to free the
 *                         resource.
 *
 * If pem_data is NULL, then return false.
 * If ec_context is NULL, then return false.
 *
 * @retval  true   EC Private key was retrieved successfully.
 * @retval  false  Invalid PEM key data or incorrect password.
 *
 **/
extern bool libspdm_ec_get_private_key_from_pem(const uint8_t *pem_data, size_t pem_size,
                                                const char *password,
                                                void **ec_context);

/**
 * Retrieve the Ed Private key from the password-protected PEM key data.
 *
 * @param[in]  pem_data     Pointer to the PEM-encoded key data to be retrieved.
 * @param[in]  pem_size     Size of the PEM key data in bytes.
 * @param[in]  password     NULL-terminated passphrase used for encrypted PEM key data.
 * @param[out] ecd_context  Pointer to newly generated Ed DSA context which contain the retrieved
 *                          Ed private key component. Use libspdm_ecd_free() function to free the
 *                          resource.
 *
 * If pem_data is NULL, then return false.
 * If ecd_context is NULL, then return false.
 *
 * @retval  true   Ed Private key was retrieved successfully.
 * @retval  false  Invalid PEM key data or incorrect password.
 **/
extern bool libspdm_ecd_get_private_key_from_pem(const uint8_t *pem_data,
                                                 size_t pem_size,
                                                 const char *password,
                                                 void **ecd_context);

/**
 * Retrieve the sm2 Private key from the password-protected PEM key data.
 *
 * @param[in]  pem_data     Pointer to the PEM-encoded key data to be retrieved.
 * @param[in]  pem_size     Size of the PEM key data in bytes.
 * @param[in]  password     NULL-terminated passphrase used for encrypted PEM key data.
 * @param[out] sm2_context  Pointer to newly generated sm2 context which contain the retrieved
 *                          sm2 private key component. Use sm2_free() function to free the
 *                          resource.
 *
 * If pem_data is NULL, then return false.
 * If sm2_context is NULL, then return false.
 *
 * @retval  true   sm2 Private key was retrieved successfully.
 * @retval  false  Invalid PEM key data or incorrect password.
 *
 **/
extern bool libspdm_sm2_get_private_key_from_pem(const uint8_t *pem_data,
                                                 size_t pem_size,
                                                 const char *password,
                                                 void **sm2_context);

/**
 * Derive key data using HMAC-SHA256 based KDF.
 *
 * @param[in]   key        Pointer to the user-supplied key.
 * @param[in]   key_size   Key size in bytes.
 * @param[in]   salt       Pointer to the salt value.
 * @param[in]   salt_size  Salt size in bytes.
 * @param[in]   info       Pointer to the application specific info.
 * @param[in]   info_size  Info size in bytes.
 * @param[out]  out        Pointer to buffer to receive hkdf value.
 * @param[in]   out_size   Size of hkdf bytes to generate.
 *
 * @retval true   Hkdf generated successfully.
 * @retval false  Hkdf generation failed.
 **/
extern bool libspdm_hkdf_sha256_extract_and_expand(const uint8_t *key, size_t key_size,
                                                   const uint8_t *salt, size_t salt_size,
                                                   const uint8_t *info, size_t info_size,
                                                   uint8_t *out, size_t out_size);

/**
 * Derive key data using HMAC-SHA384 based KDF.
 *
 * @param[in]   key        Pointer to the user-supplied key.
 * @param[in]   key_size   Key size in bytes.
 * @param[in]   salt       Pointer to the salt value.
 * @param[in]   salt_size  Salt size in bytes.
 * @param[in]   info       Pointer to the application specific info.
 * @param[in]   info_size  Info size in bytes.
 * @param[out]  out        Pointer to buffer to receive hkdf value.
 * @param[in]   out_size   Size of hkdf bytes to generate.
 *
 * @retval true   Hkdf generated successfully.
 * @retval false  Hkdf generation failed.
 **/
extern bool libspdm_hkdf_sha384_extract_and_expand(const uint8_t *key, size_t key_size,
                                                   const uint8_t *salt, size_t salt_size,
                                                   const uint8_t *info, size_t info_size,
                                                   uint8_t *out, size_t out_size);

/**
 * Derive key data using HMAC-SHA512 based KDF.
 *
 * @param[in]   key        Pointer to the user-supplied key.
 * @param[in]   key_size   Key size in bytes.
 * @param[in]   salt       Pointer to the salt value.
 * @param[in]   salt_size  Salt size in bytes.
 * @param[in]   info       Pointer to the application specific info.
 * @param[in]   info_size  Info size in bytes.
 * @param[out]  out        Pointer to buffer to receive hkdf value.
 * @param[in]   out_size   Size of hkdf bytes to generate.
 *
 * @retval true   Hkdf generated successfully.
 * @retval false  Hkdf generation failed.
 **/
extern bool libspdm_hkdf_sha512_extract_and_expand(const uint8_t *key, size_t key_size,
                                                   const uint8_t *salt, size_t salt_size,
                                                   const uint8_t *info, size_t info_size,
                                                   uint8_t *out, size_t out_size);

/**
 * Derive SHA3_256 HMAC-based Extract-and-Expand key Derivation Function (HKDF).
 *
 * @param[in]   key        Pointer to the user-supplied key.
 * @param[in]   key_size   Key size in bytes.
 * @param[in]   salt       Pointer to the salt value.
 * @param[in]   salt_size  Salt size in bytes.
 * @param[in]   info       Pointer to the application specific info.
 * @param[in]   info_size  Info size in bytes.
 * @param[out]  out        Pointer to buffer to receive hkdf value.
 * @param[in]   out_size   Size of hkdf bytes to generate.
 *
 * @retval true   Hkdf generated successfully.
 * @retval false  Hkdf generation failed.
 **/
extern bool libspdm_hkdf_sha3_256_extract_and_expand(const uint8_t *key, size_t key_size,
                                                     const uint8_t *salt, size_t salt_size,
                                                     const uint8_t *info, size_t info_size,
                                                     uint8_t *out, size_t out_size);

/**
 * Derive SHA3_384 HMAC-based Extract-and-Expand key Derivation Function (HKDF).
 *
 * @param[in]   key        Pointer to the user-supplied key.
 * @param[in]   key_size   Key size in bytes.
 * @param[in]   salt       Pointer to the salt value.
 * @param[in]   salt_size  Salt size in bytes.
 * @param[in]   info       Pointer to the application specific info.
 * @param[in]   info_size  Info size in bytes.
 * @param[out]  out        Pointer to buffer to receive hkdf value.
 * @param[in]   out_size   Size of hkdf bytes to generate.
 *
 * @retval true   Hkdf generated successfully.
 * @retval false  Hkdf generation failed.
 **/
extern bool libspdm_hkdf_sha3_384_extract_and_expand(const uint8_t *key, size_t key_size,
                                                     const uint8_t *salt, size_t salt_size,
                                                     const uint8_t *info, size_t info_size,
                                                     uint8_t *out, size_t out_size);

/**
 * Derive SHA3_512 HMAC-based Extract-and-Expand key Derivation Function (HKDF).
 *
 * @param[in]   key        Pointer to the user-supplied key.
 * @param[in]   key_size   Key size in bytes.
 * @param[in]   salt       Pointer to the salt value.
 * @param[in]   salt_size  Salt size in bytes.
 * @param[in]   info       Pointer to the application specific info.
 * @param[in]   info_size  Info size in bytes.
 * @param[out]  out        Pointer to buffer to receive hkdf value.
 * @param[in]   out_size   Size of hkdf bytes to generate.
 *
 * @retval true   Hkdf generated successfully.
 * @retval false  Hkdf generation failed.
 **/
extern bool libspdm_hkdf_sha3_512_extract_and_expand(const uint8_t *key, size_t key_size,
                                                     const uint8_t *salt, size_t salt_size,
                                                     const uint8_t *info, size_t info_size,
                                                     uint8_t *out, size_t out_size);

/**
 * Derive SM3_256 HMAC-based Extract-and-Expand key Derivation Function (HKDF).
 *
 * @param[in]   key        Pointer to the user-supplied key.
 * @param[in]   key_size   Key size in bytes.
 * @param[in]   salt       Pointer to the salt value.
 * @param[in]   salt_size  Salt size in bytes.
 * @param[in]   info       Pointer to the application specific info.
 * @param[in]   info_size  Info size in bytes.
 * @param[out]  out        Pointer to buffer to receive hkdf value.
 * @param[in]   out_size   Size of hkdf bytes to generate.
 *
 * @retval true   Hkdf generated successfully.
 * @retval false  Hkdf generation failed.
 **/
extern bool libspdm_hkdf_sm3_256_extract_and_expand(const uint8_t *key, size_t key_size,
                                                    const uint8_t *salt, size_t salt_size,
                                                    const uint8_t *info, size_t info_size,
                                                    uint8_t *out, size_t out_size);

/**
 * Sets the private key component into the established EC context.
 *
 * For P-256, the private_key_size is 32 byte.
 * For P-384, the private_key_size is 48 byte.
 * For P-521, the private_key_size is 66 byte.
 *
 * @param[in, out]  ec_context       Pointer to EC context being set.
 * @param[in]       private_key      Pointer to the private key buffer.
 * @param[in]       private_key_size The size of private key buffer in bytes.
 *
 * @retval  true   EC private key component was set successfully.
 * @retval  false  Invalid EC private key component.
 *
 **/
extern bool libspdm_ec_set_priv_key(void *ec_context, const uint8_t *private_key,
                                    size_t private_key_size);

/**
 * Sets the public key component into the established EC context.
 *
 * For P-256, the public_size is 64. first 32-byte is X, second 32-byte is Y.
 * For P-384, the public_size is 96. first 48-byte is X, second 48-byte is Y.
 * For P-521, the public_size is 132. first 66-byte is X, second 66-byte is Y.
 *
 * @param[in, out]  ec_context   Pointer to EC context being set.
 * @param[in]       public       Pointer to the buffer to receive generated public X,Y.
 * @param[in]       public_size  The size of public buffer in bytes.
 *
 * @retval  true   EC public key component was set successfully.
 * @retval  false  Invalid EC public key component.
 **/
extern bool libspdm_ec_set_pub_key(void *ec_context, const uint8_t *public_key,
                                   size_t public_key_size);

/**
 * Gets the public key component from the established EC context.
 *
 * For P-256, the public_size is 64. first 32-byte is X, second 32-byte is Y.
 * For P-384, the public_size is 96. first 48-byte is X, second 48-byte is Y.
 * For P-521, the public_size is 132. first 66-byte is X, second 66-byte is Y.
 *
 * @param[in, out]  ec_context   Pointer to EC context being set.
 * @param[out]      public       Pointer to the buffer to receive generated public X,Y.
 * @param[in, out]  public_size  On input, the size of public buffer in bytes.
 *                               On output, the size of data returned in public buffer in bytes.
 *
 * @retval  true   EC key component was retrieved successfully.
 * @retval  false  Invalid EC key component.
 **/
extern bool libspdm_ec_get_pub_key(void *ec_context, uint8_t *public_key, size_t *public_key_size);

/**
 * Validates key components of EC context.
 * NOTE: This function performs integrity checks on all the EC key material, so
 *       the EC key structure must contain all the private key data.
 *
 * If ec_context is NULL, then return false.
 *
 * @param[in]  ec_context  Pointer to EC context to check.
 *
 * @retval  true   EC key components are valid.
 * @retval  false  EC key components are not valid.
 **/
extern bool libspdm_ec_check_key(const void *ec_context);

/**
 * Validates key components of Ed context.
 * NOTE: This function performs integrity checks on all the Ed key material, so
 *       the Ed key structure must contain all the private key data.
 *
 * If ecd_context is NULL, then return false.
 *
 * @param[in]  ecd_context  Pointer to Ed context to check.
 *
 * @retval  true   Ed key components are valid.
 * @retval  false  Ed key components are not valid.
 **/
extern bool libspdm_ecd_check_key(const void *ecd_context);

/**
 * Generates Ed key and returns Ed public key.
 *
 * For ed25519, the public_size is 32.
 * For ed448, the public_size is 57.
 *
 * If ecd_context is NULL, then return false.
 * If public_size is NULL, then return false.
 * If public_size is large enough but public is NULL, then return false.
 *
 * @param[in, out]  ecd_context      Pointer to the Ed context.
 * @param[out]      public_key       Pointer to the buffer to receive generated public key.
 * @param[in, out]  public_key_size  On input, the size of public buffer in bytes.
 *                                   On output, the size of data returned in public buffer in bytes.
 *
 * @retval true   Ed public key generation succeeded.
 * @retval false  Ed public key generation failed.
 * @retval false  public_size is not large enough.
 **/
extern bool libspdm_ecd_generate_key(void *ecd_context, uint8_t *public_key,
                                     size_t *public_key_size);

/**
 * Generates DH parameter.
 *
 * Given generator g, and length of prime number p in bits, this function generates p,
 * and sets DH context according to value of g and p.
 *
 * If dh_context is NULL, then return false.
 * If prime is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[in, out]  dh_context    Pointer to the DH context.
 * @param[in]       generator     Value of generator.
 * @param[in]       prime_length  Length in bits of prime to be generated.
 * @param[out]      prime         Pointer to the buffer to receive the generated prime number.
 *
 * @retval true   DH parameter generation succeeded.
 * @retval false  Value of generator is not supported.
 * @retval false  Random number generator fails to generate random prime number with prime_length.
 * @retval false  This interface is not supported.
 **/
extern bool libspdm_dh_generate_parameter(void *dh_context, size_t generator,
                                          size_t prime_length, uint8_t *prime);

/**
 * Sets generator and prime parameters for DH.
 *
 * Given generator g, and prime number p, this function and sets DH context accordingly.
 *
 * If dh_context is NULL, then return false.
 * If prime is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[in, out]  dh_context    Pointer to the DH context.
 * @param[in]       generator     Value of generator.
 * @param[in]       prime_length  Length in bits of prime to be generated.
 * @param[in]       prime         Pointer to the prime number.
 *
 * @retval true   DH parameter setting succeeded.
 * @retval false  Value of generator is not supported.
 * @retval false  Value of generator is not suitable for the prime.
 * @retval false  Value of prime is not a prime number.
 * @retval false  Value of prime is not a safe prime number.
 * @retval false  This interface is not supported.
 **/
extern bool libspdm_dh_set_parameter(void *dh_context, size_t generator,
                                     size_t prime_length, const uint8_t *prime);

/**
 * Sets the public key component into the established sm2 context.
 *
 * The public_size is 64. first 32-byte is X, second 32-byte is Y.
 *
 * @param[in, out]  ec_context       Pointer to sm2 context being set.
 * @param[in]       public_key       Pointer to the buffer to receive generated public X,Y.
 * @param[in]       public_key_size  The size of public buffer in bytes.
 *
 * @retval  true   sm2 public key component was set successfully.
 * @retval  false  Invalid sm2 public key component.
 **/
extern bool libspdm_sm2_dsa_set_pub_key(void *sm2_context, const uint8_t *public_key,
                                        size_t public_key_size);

/**
 * Gets the public key component from the established sm2 context.
 *
 * The public_size is 64. first 32-byte is X, second 32-byte is Y.
 *
 * @param[in, out]  sm2_context      Pointer to sm2 context being set.
 * @param[out]      public_key       Pointer to the buffer to receive generated public X,Y.
 * @param[in, out]  public_key_size  On input, the size of public buffer in bytes.
 *                                   On output, the size of data returned in public buffer in bytes.
 *
 * @retval  true   sm2 key component was retrieved successfully.
 * @retval  false  Invalid sm2 key component.
 **/
extern bool libspdm_sm2_dsa_get_pub_key(void *sm2_context, uint8_t *public_key,
                                        size_t *public_key_size);

/**
 * Validates key components of sm2 context.
 * NOTE: This function performs integrity checks on all the sm2 key material, so
 *       the sm2 key structure must contain all the private key data.
 *
 * If sm2_context is NULL, then return false.
 *
 * @param[in]  sm2_context  Pointer to sm2 context to check.
 *
 * @retval  true   sm2 key components are valid.
 * @retval  false  sm2 key components are not valid.
 **/
extern bool libspdm_sm2_dsa_check_key(const void *sm2_context);

/**
 * Generates sm2 key and returns sm2 public key (X, Y), based upon GB/T 32918.3-2016: SM2 - Part3.
 *
 * This function generates random secret, and computes the public key (X, Y), which is
 * returned via parameter public, public_size.
 * X is the first half of public with size being public_size / 2,
 * Y is the second half of public with size being public_size / 2.
 * sm2 context is updated accordingly.
 * If the public buffer is too small to hold the public X, Y, false is returned and
 * public_size is set to the required buffer size to obtain the public X, Y.
 *
 * The public_size is 64. first 32-byte is X, second 32-byte is Y.
 *
 * If sm2_context is NULL, then return false.
 * If public_size is NULL, then return false.
 * If public_size is large enough but public is NULL, then return false.
 *
 * @param[in, out]  sm2_context  Pointer to the sm2 context.
 * @param[out]      public_data  Pointer to the buffer to receive generated public X,Y.
 * @param[in, out]  public_size  On input, the size of public buffer in bytes.
 *                               On output, the size of data returned in public buffer in bytes.
 *
 * @retval true   sm2 public X,Y generation succeeded.
 * @retval false  sm2 public X,Y generation failed.
 * @retval false  public_size is not large enough.
 **/
extern bool libspdm_sm2_dsa_generate_key(void *sm2_context, uint8_t *public_data,
                                         size_t *public_size);

#if LIBSPDM_ENABLE_CAPABILITY_CSR_CAP
/**
 * Generate a CSR.
 *
 * @param[in]      hash_nid              hash algo for sign
 * @param[in]      asym_nid              asym algo for sign
 *
 * @param[in]      requester_info        requester info to gen CSR
 * @param[in]      requester_info_length The len of requester info
 *
 * @param[in]       is_ca                if true, set basic_constraints: CA:true; Otherwise, set to false.
 *
 * @param[in]      context               Pointer to asymmetric context
 * @param[in]      subject_name          Subject name: should be break with ',' in the middle
 *                                       example: "C=AA,CN=BB"
 *
 * Subject names should contain a comma-separated list of OID types and values:
 * The valid OID type name is in:
 * {"CN", "commonName", "C", "countryName", "O", "organizationName","L",
 * "OU", "organizationalUnitName", "ST", "stateOrProvinceName", "emailAddress",
 * "serialNumber", "postalAddress", "postalCode", "dnQualifier", "title",
 * "SN","givenName","GN", "initials", "pseudonym", "generationQualifier", "domainComponent", "DC"}.
 * Note: The object of C and countryName should be CSR Supported Country Codes
 *
 * @param[in, out]      csr_len               For input, csr_len is the size of store CSR buffer.
 *                                            For output, csr_len is CSR len for DER format
 * @param[in, out]      csr_pointer           For input, csr_pointer is buffer address to store CSR.
 *                                            For output, csr_pointer is address for stored CSR.
 *                                            The csr_pointer address will be changed.
 *
 * @retval  true   Success.
 * @retval  false  Failed to gen CSR.
 **/
extern bool libspdm_gen_x509_csr(size_t hash_nid, size_t asym_nid,
                                 uint8_t *requester_info, size_t requester_info_length,
                                 bool is_ca,
                                 void *context, char *subject_name,
                                 size_t *csr_len, uint8_t *csr_pointer);
#endif /* LIBSPDM_ENABLE_CAPABILITY_CSR_CAP */

#endif /* CRYPTLIB_EXT_H */
