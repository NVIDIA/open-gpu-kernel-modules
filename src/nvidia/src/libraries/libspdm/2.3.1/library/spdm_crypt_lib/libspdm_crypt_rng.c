/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#include "internal/libspdm_crypt_lib.h"

bool libspdm_get_random_number(size_t size, uint8_t *rand)
{
    if (size == 0) {
        return true;
    }
    return libspdm_random_bytes(rand, size);
}
