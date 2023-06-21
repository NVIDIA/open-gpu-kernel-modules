/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

/** @file
 * Pseudorandom Number generator Wrapper Implementation.
 **/

#include "internal_crypt_lib.h"
#include "library/rnglib.h"

/**
 * Generates a random byte stream of the specified size.
 *
 * If output is NULL, then return false.
 * If this interface is not supported, then return false.
 *
 * @param[out]  output  Pointer to buffer to receive random value.
 * @param[in]   size    Size of random bytes to generate.
 *
 * @retval true   Random byte stream generated successfully.
 * @retval false  Generation of random byte stream failed.
 **/
bool libspdm_random_bytes(uint8_t *output, size_t size)
{
    bool ret;
    uint64_t temp_rand;
    size_t dst_size = size;

    ret = false;

    while (size > 0) {
        /* Use rnglib to get random number*/
        ret = libspdm_get_random_number_64(&temp_rand);

        if (!ret) {
            return ret;
        }

        if (size >= sizeof(uint64_t)) {
            libspdm_copy_mem(output, dst_size, &temp_rand, sizeof(uint64_t));
            output += sizeof(uint64_t);
            size -= sizeof(uint64_t);
        } else {
            libspdm_copy_mem(output, dst_size, &temp_rand, size);
            size = 0;
        }
    }

    return ret;
}

int libspdm_myrand(void *rng_state, unsigned char *output, size_t len)
{
    bool result = libspdm_random_bytes(output, len);


    /* The MbedTLS function f_rng, which myrand implements, is not
     * documented well. From looking at code: zero is considered success,
     * while non-zero return value is considered failure.*/

    return result ? 0 : -1;
}
