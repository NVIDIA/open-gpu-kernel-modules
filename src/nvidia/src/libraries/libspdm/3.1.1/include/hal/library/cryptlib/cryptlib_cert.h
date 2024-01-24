/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#ifndef CRYPTLIB_CERT_H
#define CRYPTLIB_CERT_H

#if LIBSPDM_CERT_PARSE_SUPPORT

/**
 * Retrieve the tag and length of the tag.
 *
 * @param ptr      The position in the ASN.1 data.
 * @param end      End of data.
 * @param length   The variable that will receive the length.
 * @param tag      The expected tag.
 *
 * @retval  true   Get tag successful.
 * @retval  false  Failed to get tag or tag not match.
 **/
extern bool libspdm_asn1_get_tag(uint8_t **ptr, const uint8_t *end, size_t *length, uint32_t tag);

/**
 * Retrieve the subject bytes from one X.509 certificate.
 *
 * If cert is NULL, then return false.
 * If subject_size is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[in]      cert          Pointer to the DER-encoded X509 certificate.
 * @param[in]      cert_size     Size of the X509 certificate in bytes.
 * @param[out]     cert_subject  Pointer to the retrieved certificate subject bytes.
 * @param[in, out] subject_size  The size in bytes of the cert_subject buffer on input,
 *                               and the size of buffer returned cert_subject on output.
 *
 * @retval  true   The certificate subject retrieved successfully.
 * @retval  false  Invalid certificate, or the subject_size is too small for the result.
 *                 The subject_size will be updated with the required size.
 * @retval  false  This interface is not supported.
 **/
extern bool libspdm_x509_get_subject_name(const uint8_t *cert, size_t cert_size,
                                          uint8_t *cert_subject,
                                          size_t *subject_size);

/**
 * Retrieve the version from one X.509 certificate.
 *
 * If cert is NULL, then return false.
 * If cert_size is 0, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[in]      cert       Pointer to the DER-encoded X509 certificate.
 * @param[in]      cert_size  Size of the X509 certificate in bytes.
 * @param[out]     version    Pointer to the retrieved version integer.
 *
 * @retval  true
 * @retval  false
 **/
extern bool libspdm_x509_get_version(const uint8_t *cert, size_t cert_size, size_t *version);

/**
 * Retrieve the serialNumber from one X.509 certificate.
 *
 * If cert is NULL, then return false.
 * If cert_size is 0, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[in]      cert                Pointer to the DER-encoded X509 certificate.
 * @param[in]      cert_size           Size of the X509 certificate in bytes.
 * @param[out]     serial_number       Pointer to the retrieved certificate serial_number bytes.
 * @param[in, out] serial_number_size  The size in bytes of the serial_number buffer on input,
 *                                     and the size of buffer returned serial_number on output.
 *
 * @retval  true
 * @retval  false
 **/
extern bool libspdm_x509_get_serial_number(const uint8_t *cert, size_t cert_size,
                                           uint8_t *serial_number,
                                           size_t *serial_number_size);

/**
 * Retrieve the issuer bytes from one X.509 certificate.
 *
 * If cert is NULL, then return false.
 * If issuer_size is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[in]      cert         Pointer to the DER-encoded X509 certificate.
 * @param[in]      cert_size    Size of the X509 certificate in bytes.
 * @param[out]     cert_issuer  Pointer to the retrieved certificate subject bytes.
 * @param[in, out] issuer_size  The size in bytes of the cert_issuer buffer on input,
 *                              and the size of buffer returned cert_issuer on output.
 *
 * @retval  true   The certificate issuer retrieved successfully.
 * @retval  false  Invalid certificate, or the issuer_size is too small for the result.
 *                 The issuer_size will be updated with the required size.
 * @retval  false  This interface is not supported.
 **/
extern bool libspdm_x509_get_issuer_name(const uint8_t *cert, size_t cert_size,
                                         uint8_t *cert_issuer,
                                         size_t *issuer_size);

/**
 * Retrieve Extension data from one X.509 certificate.
 *
 * @param[in]      cert                 Pointer to the DER-encoded X509 certificate.
 * @param[in]      cert_size            Size of the X509 certificate in bytes.
 * @param[in]      oid                  Object identifier buffer
 * @param[in]      oid_size             Object identifier buffer size
 * @param[out]     extension_data       Extension bytes.
 * @param[in, out] extension_data_size  Extension bytes size.
 *
 * @retval  true
 * @retval  false
 **/
extern bool libspdm_x509_get_extension_data(const uint8_t *cert, size_t cert_size,
                                            const uint8_t *oid, size_t oid_size,
                                            uint8_t *extension_data,
                                            size_t *extension_data_size);

/**
 * Retrieve the Validity from one X.509 certificate
 *
 * If cert is NULL, then return false.
 * If CertIssuerSize is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[in]      cert       Pointer to the DER-encoded X509 certificate.
 * @param[in]      cert_size  Size of the X509 certificate in bytes.
 * @param[out]     from       notBefore Pointer to date_time object.
 * @param[in,out]  from_size  notBefore date_time object size.
 * @param[out]     to         notAfter Pointer to date_time object.
 * @param[in,out]  to_size    notAfter date_time object size.
 *
 * Note: libspdm_x509_compare_date_time to compare date_time oject
 *       x509SetDateTime to get a date_time object from a date_time_str
 *
 * @retval  true   The certificate Validity retrieved successfully.
 * @retval  false  Invalid certificate, or Validity retrieve failed.
 * @retval  false  This interface is not supported.
 **/
extern bool libspdm_x509_get_validity(const uint8_t *cert, size_t cert_size,
                                      uint8_t *from, size_t *from_size, uint8_t *to,
                                      size_t *to_size);

/**
 * Format a date_time object into DataTime buffer
 *
 * If date_time_str is NULL, then return false.
 * If date_time_size is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[in]      date_time_str    date_time string like YYYYMMDDhhmmssZ
 *                                  Ref: https://www.w3.org/TR/NOTE-datetime
 *                                  Z stand for UTC time
 * @param[out]     date_time       Pointer to a date_time object.
 * @param[in,out]  date_time_size  date_time object buffer size.
 *
 * @retval  true
 * @retval  false
 **/
extern bool libspdm_x509_set_date_time(const char *date_time_str, void *date_time,
                                       size_t *date_time_size);

/**
 * Compare date_time1 object and date_time2 object.
 *
 * If date_time1 is NULL, then return -2.
 * If date_time2 is NULL, then return -2.
 * If date_time1 == date_time2, then return 0
 * If date_time1 > date_time2, then return 1
 * If date_time1 < date_time2, then return -1
 *
 * @param[in]      date_time1  Pointer to a date_time Ojbect
 * @param[in]      date_time2  Pointer to a date_time Object
 *
 * @retval   0  If date_time1 == date_time2
 * @retval   1  If date_time1 > date_time2
 * @retval  -1  If date_time1 < date_time2
 **/
extern int32_t libspdm_x509_compare_date_time(const void *date_time1, const void *date_time2);

/**
 * Retrieve the key usage from one X.509 certificate.
 *
 * @param[in]      cert       Pointer to the DER-encoded X509 certificate.
 * @param[in]      cert_size  Size of the X509 certificate in bytes.
 * @param[out]     usage      Key usage (LIBSPDM_CRYPTO_X509_KU_*)
 *
 * @retval  true   The certificate key usage retrieved successfully.
 * @retval  false  Invalid certificate, or usage is NULL
 * @retval  false  This interface is not supported.
 **/
extern bool libspdm_x509_get_key_usage(const uint8_t *cert, size_t cert_size, size_t *usage);

/**
 * Retrieve the Extended key usage from one X.509 certificate.
 *
 * @param[in]      cert        Pointer to the DER-encoded X509 certificate.
 * @param[in]      cert_size   Size of the X509 certificate in bytes.
 * @param[out]     usage       Key usage bytes.
 * @param[in, out] usage_size  Key usage buffer size in bytes.
 *
 * @retval  true
 * @retval  false
 **/
extern bool libspdm_x509_get_extended_key_usage(const uint8_t *cert,
                                                size_t cert_size, uint8_t *usage,
                                                size_t *usage_size);

/**
 * Retrieve the basic constraints from one X.509 certificate.
 *
 * @param[in]      cert                     Pointer to the DER-encoded X509 certificate.
 * @param[in]      cert_size                Size of the X509 certificate in bytes.
 * @param[out]     basic_constraints        Basic constraints bytes.
 * @param[in, out] basic_constraints_size   Basic constraints buffer size in bytes.
 *
 * @retval  true
 * @retval  false
 **/
extern bool libspdm_x509_get_extended_basic_constraints(const uint8_t *cert,
                                                        size_t cert_size,
                                                        uint8_t *basic_constraints,
                                                        size_t *basic_constraints_size);

/**
 * Verify one X509 certificate was issued by the trusted CA.
 *
 * If cert is NULL, then return false.
 * If ca_cert is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[in]      cert          Pointer to the DER-encoded X509 certificate to be verified.
 * @param[in]      cert_size     Size of the X509 certificate in bytes.
 * @param[in]      ca_cert       Pointer to the DER-encoded trusted CA certificate.
 * @param[in]      ca_cert_size  Size of the CA Certificate in bytes.
 *
 * @retval  true   The certificate was issued by the trusted CA.
 * @retval  false  Invalid certificate or the certificate was not issued by the given
 *                 trusted CA.
 * @retval  false  This interface is not supported.
 *
 **/
extern bool libspdm_x509_verify_cert(const uint8_t *cert, size_t cert_size,
                                     const uint8_t *ca_cert, size_t ca_cert_size);

/**
 * Verify one X509 certificate was issued by the trusted CA.
 *
 * @param[in]      cert_chain         One or more ASN.1 DER-encoded X.509 certificates
 *                                    where the first certificate is signed by the Root
 *                                    Certificate or is the Root Certificate itself. and
 *                                    subsequent certificate is signed by the preceding
 *                                    certificate.
 * @param[in]      cert_chain_length  Total length of the certificate chain, in bytes.
 *
 * @param[in]      root_cert          Trusted Root Certificate buffer.
 *
 * @param[in]      root_cert_length   Trusted Root Certificate buffer length.
 *
 * @retval  true   All certificates were issued by the first certificate in X509Certchain.
 * @retval  false  Invalid certificate or the certificate was not issued by the given
 *                 trusted CA.
 **/
extern bool libspdm_x509_verify_cert_chain(const uint8_t *root_cert, size_t root_cert_length,
                                           const uint8_t *cert_chain,
                                           size_t cert_chain_length);

/**
 * Get one X509 certificate from cert_chain.
 *
 * @param[in]      cert_chain         One or more ASN.1 DER-encoded X.509 certificates
 *                                    where the first certificate is signed by the Root
 *                                    Certificate or is the Root Certificate itself. and
 *                                    subsequent certificate is signed by the preceding
 *                                    certificate.
 * @param[in]      cert_chain_length  Total length of the certificate chain, in bytes.
 *
 * @param[in]      cert_index         Index of certificate. If index is -1 indicates the
 *                                    last certificate in cert_chain.
 *
 * @param[out]     cert              The certificate at the index of cert_chain.
 * @param[out]     cert_length       The length certificate at the index of cert_chain.
 *
 * @retval  true   Success.
 * @retval  false  Failed to get certificate from certificate chain.
 **/
extern bool libspdm_x509_get_cert_from_cert_chain(const uint8_t *cert_chain,
                                                  size_t cert_chain_length,
                                                  const int32_t cert_index, const uint8_t **cert,
                                                  size_t *cert_length);

#if (LIBSPDM_RSA_SSA_SUPPORT) || (LIBSPDM_RSA_PSS_SUPPORT)
/**
 * Retrieve the RSA public key from one DER-encoded X509 certificate.
 *
 * If cert is NULL, then return false.
 * If rsa_context is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[in]  cert         Pointer to the DER-encoded X509 certificate.
 * @param[in]  cert_size    Size of the X509 certificate in bytes.
 * @param[out] rsa_context  Pointer to newly generated RSA context which contain the retrieved
 *                          RSA public key component. Use libspdm_rsa_free() function to free the
 *                          resource.
 *
 * @retval  true   RSA public key was retrieved successfully.
 * @retval  false  Fail to retrieve RSA public key from X509 certificate.
 * @retval  false  This interface is not supported.
 **/
extern bool libspdm_rsa_get_public_key_from_x509(const uint8_t *cert, size_t cert_size,
                                                 void **rsa_context);
#endif /* (LIBSPDM_RSA_SSA_SUPPORT) || (LIBSPDM_RSA_PSS_SUPPORT) */

#if LIBSPDM_ECDSA_SUPPORT
/**
 * Retrieve the EC public key from one DER-encoded X509 certificate.
 *
 * @param[in]  cert        Pointer to the DER-encoded X509 certificate.
 * @param[in]  cert_size   Size of the X509 certificate in bytes.
 * @param[out] ec_context  Pointer to newly generated EC DSA context which contain the retrieved
 *                         EC public key component. Use libspdm_ec_free() function to free the
 *                         resource.
 *
 * If cert is NULL, then return false.
 * If ec_context is NULL, then return false.
 *
 * @retval  true   EC public key was retrieved successfully.
 * @retval  false  Fail to retrieve EC public key from X509 certificate.
 *
 **/
extern bool libspdm_ec_get_public_key_from_x509(const uint8_t *cert, size_t cert_size,
                                                void **ec_context);
#endif /* LIBSPDM_ECDSA_SUPPORT */

#if (LIBSPDM_EDDSA_ED25519_SUPPORT) || (LIBSPDM_EDDSA_ED448_SUPPORT)
/**
 * Retrieve the Ed public key from one DER-encoded X509 certificate.
 *
 * @param[in]  cert         Pointer to the DER-encoded X509 certificate.
 * @param[in]  cert_size    Size of the X509 certificate in bytes.
 * @param[out] ecd_context  Pointer to newly generated Ed DSA context which contain the retrieved
 *                          Ed public key component. Use libspdm_ecd_free() function to free the
 *                          resource.
 *
 * If cert is NULL, then return false.
 * If ecd_context is NULL, then return false.
 *
 * @retval  true   Ed public key was retrieved successfully.
 * @retval  false  Fail to retrieve Ed public key from X509 certificate.
 *
 **/
extern bool libspdm_ecd_get_public_key_from_x509(const uint8_t *cert, size_t cert_size,
                                                 void **ecd_context);
#endif /* (LIBSPDM_EDDSA_ED25519_SUPPORT) || (LIBSPDM_EDDSA_ED448_SUPPORT) */

#if LIBSPDM_SM2_DSA_SUPPORT
/**
 * Retrieve the sm2 public key from one DER-encoded X509 certificate.
 *
 * @param[in]  cert         Pointer to the DER-encoded X509 certificate.
 * @param[in]  cert_size    Size of the X509 certificate in bytes.
 * @param[out] sm2_context  Pointer to newly generated sm2 context which contain the retrieved
 *                          sm2 public key component. Use sm2_free() function to free the
 *                          resource.
 *
 * If cert is NULL, then return false.
 * If sm2_context is NULL, then return false.
 *
 * @retval  true   sm2 public key was retrieved successfully.
 * @retval  false  Fail to retrieve sm2 public key from X509 certificate.
 *
 **/
extern bool libspdm_sm2_get_public_key_from_x509(const uint8_t *cert, size_t cert_size,
                                                 void **sm2_context);
#endif /* LIBSPDM_SM2_DSA_SUPPORT */

#endif /* LIBSPDM_CERT_PARSE_SUPPORT */

#endif /* CRYPTLIB_CERT_H */
