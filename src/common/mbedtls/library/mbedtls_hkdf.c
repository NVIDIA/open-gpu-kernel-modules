/*
 *  HKDF implementation -- RFC 5869
 */
//
//  Copyright The Mbed TLS Contributors
//  NVIDIA_EDIT: This version of MbedTLS is dual-licensed - NVIDIA
//  receives it under the GPL-2.0 license - these headers are updated
//  accordingly.
//
//  SPDX-License-Identifier: GPL-2.0
//
//  This file is provided under the GNU General Public License v2.0.
//
//  **********
//  GNU General Public License v2.0:
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation version 2 of the License.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along
//  with this program; if not, write to the Free Software Foundation, Inc.,
//  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
//  **********
//
#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_HKDF_C)

#ifndef NVRM
#include <string.h>
#endif
#include "mbedtls/hkdf.h"
#include "mbedtls/platform_util.h"

int mbedtls_hkdf( const mbedtls_md_info_t *md, const unsigned char *salt,
                  size_t salt_len, const unsigned char *ikm, size_t ikm_len,
                  const unsigned char *info, size_t info_len,
                  unsigned char *okm, size_t okm_len )
{
    int ret;
    unsigned char prk[MBEDTLS_MD_MAX_SIZE];

    ret = mbedtls_hkdf_extract( md, salt, salt_len, ikm, ikm_len, prk );

    if( ret == 0 )
    {
        ret = mbedtls_hkdf_expand( md, prk, mbedtls_md_get_size( md ),
                                   info, info_len, okm, okm_len );
    }

    mbedtls_platform_zeroize( prk, sizeof( prk ) );

    return( ret );
}

int mbedtls_hkdf_extract( const mbedtls_md_info_t *md,
                          const unsigned char *salt, size_t salt_len,
                          const unsigned char *ikm, size_t ikm_len,
                          unsigned char *prk )
{
    unsigned char null_salt[MBEDTLS_MD_MAX_SIZE] = { '\0' };

    if( salt == NULL )
    {
        size_t hash_len;

        if( salt_len != 0 )
        {
            return MBEDTLS_ERR_HKDF_BAD_INPUT_DATA;
        }

        hash_len = mbedtls_md_get_size( md );

        if( hash_len == 0 )
        {
            return MBEDTLS_ERR_HKDF_BAD_INPUT_DATA;
        }

        salt = null_salt;
        salt_len = hash_len;
    }

    return( mbedtls_md_hmac( md, salt, salt_len, ikm, ikm_len, prk ) );
}

int mbedtls_hkdf_expand( const mbedtls_md_info_t *md, const unsigned char *prk,
                         size_t prk_len, const unsigned char *info,
                         size_t info_len, unsigned char *okm, size_t okm_len )
{
    size_t hash_len;
    size_t where = 0;
    size_t n;
    size_t t_len = 0;
    size_t i;
    int ret = 0;
    mbedtls_md_context_t ctx;
    unsigned char t[MBEDTLS_MD_MAX_SIZE];

    if( okm == NULL )
    {
        return( MBEDTLS_ERR_HKDF_BAD_INPUT_DATA );
    }

    hash_len = mbedtls_md_get_size( md );

    if( prk_len < hash_len || hash_len == 0 )
    {
        return( MBEDTLS_ERR_HKDF_BAD_INPUT_DATA );
    }

    if( info == NULL )
    {
        info = (const unsigned char *) "";
        info_len = 0;
    }

    n = okm_len / hash_len;

    if( (okm_len % hash_len) != 0 )
    {
        n++;
    }

    /*
     * Per RFC 5869 Section 2.3, okm_len must not exceed
     * 255 times the hash length
     */
    if( n > 255 )
    {
        return( MBEDTLS_ERR_HKDF_BAD_INPUT_DATA );
    }

    mbedtls_md_init( &ctx );

    if( (ret = mbedtls_md_setup( &ctx, md, 1) ) != 0 )
    {
        goto exit;
    }

    /*
     * Compute T = T(1) | T(2) | T(3) | ... | T(N)
     * Where T(N) is defined in RFC 5869 Section 2.3
     */
    for( i = 1; i <= n; i++ )
    {
        size_t num_to_copy;
        unsigned char c = i & 0xff;

        ret = mbedtls_md_hmac_starts( &ctx, prk, prk_len );
        if( ret != 0 )
        {
            goto exit;
        }

        ret = mbedtls_md_hmac_update( &ctx, t, t_len );
        if( ret != 0 )
        {
            goto exit;
        }

        ret = mbedtls_md_hmac_update( &ctx, info, info_len );
        if( ret != 0 )
        {
            goto exit;
        }

        /* The constant concatenated to the end of each T(n) is a single octet.
         * */
        ret = mbedtls_md_hmac_update( &ctx, &c, 1 );
        if( ret != 0 )
        {
            goto exit;
        }

        ret = mbedtls_md_hmac_finish( &ctx, t );
        if( ret != 0 )
        {
            goto exit;
        }

        num_to_copy = i != n ? hash_len : okm_len - where;
        memcpy( okm + where, t, num_to_copy );
        where += hash_len;
        t_len = hash_len;
    }

exit:
    mbedtls_md_free( &ctx );
    mbedtls_platform_zeroize( t, sizeof( t ) );

    return( ret );
}

#endif /* MBEDTLS_HKDF_C */
