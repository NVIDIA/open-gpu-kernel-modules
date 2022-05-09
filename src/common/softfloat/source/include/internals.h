
/*============================================================================

This C header file is part of the SoftFloat IEEE Floating-Point Arithmetic
Package, Release 3d, by John R. Hauser.

Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 The Regents of the
University of California.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice,
    this list of conditions, and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions, and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

 3. Neither the name of the University nor the names of its contributors may
    be used to endorse or promote products derived from this software without
    specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS "AS IS", AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, ARE
DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=============================================================================*/

#ifndef internals_h
#define internals_h 1

#include <stdbool.h>
#include <stdint.h>
#include "primitives.h"
#include "softfloat_types.h"

union ui16_f16 { uint16_t ui; float16_t f; };
union ui32_f32 { uint32_t ui; float32_t f; };
union ui64_f64 { uint64_t ui; float64_t f; };

union extF80M_extF80 { struct extFloat80M fM; extFloat80_t f; };
union ui128_f128 { struct uint128 ui; float128_t f; };

enum {
    softfloat_mulAdd_subC    = 1,
    softfloat_mulAdd_subProd = 2
};

/*----------------------------------------------------------------------------
*----------------------------------------------------------------------------*/
uint_fast32_t softfloat_roundToUI32( bool, uint_fast64_t, uint_fast8_t, bool );

uint_fast64_t
 softfloat_roundToUI64(
     bool, uint_fast64_t, uint_fast64_t, uint_fast8_t, bool );

int_fast32_t softfloat_roundToI32( bool, uint_fast64_t, uint_fast8_t, bool );

int_fast64_t
 softfloat_roundToI64(
     bool, uint_fast64_t, uint_fast64_t, uint_fast8_t, bool );

/*----------------------------------------------------------------------------
*----------------------------------------------------------------------------*/
#define signF16UI( a ) ((bool) ((uint16_t) (a)>>15))
#define expF16UI( a ) ((int_fast8_t) ((a)>>10) & 0x1F)
#define fracF16UI( a ) ((a) & 0x03FF)
#define packToF16UI( sign, exp, sig ) (((uint16_t) (sign)<<15) + ((uint16_t) (exp)<<10) + (sig))

#define isNaNF16UI( a ) (((~(a) & 0x7C00) == 0) && ((a) & 0x03FF))

float16_t softfloat_roundPackToF16( bool, int_fast16_t, uint_fast16_t );

/*----------------------------------------------------------------------------
*----------------------------------------------------------------------------*/
#define signF32UI( a ) ((bool) ((uint32_t) (a)>>31))
#define expF32UI( a ) ((int_fast16_t) ((a)>>23) & 0xFF)
#define fracF32UI( a ) ((a) & 0x007FFFFF)
#define packToF32UI( sign, exp, sig ) (((uint32_t) (sign)<<31) + ((uint32_t) (exp)<<23) + (sig))

#define isNaNF32UI( a ) (((~(a) & 0x7F800000) == 0) && ((a) & 0x007FFFFF))

struct exp16_sig32 { int_fast16_t exp; uint_fast32_t sig; };
struct exp16_sig32 softfloat_normSubnormalF32Sig( uint_fast32_t );

float32_t softfloat_roundPackToF32( bool, int_fast16_t, uint_fast32_t );
float32_t softfloat_normRoundPackToF32( bool, int_fast16_t, uint_fast32_t );

float32_t softfloat_addMagsF32( uint_fast32_t, uint_fast32_t );
float32_t softfloat_subMagsF32( uint_fast32_t, uint_fast32_t );
float32_t
 softfloat_mulAddF32(
     uint_fast32_t, uint_fast32_t, uint_fast32_t, uint_fast8_t );

/*----------------------------------------------------------------------------
*----------------------------------------------------------------------------*/
#define signF64UI( a ) ((bool) ((uint64_t) (a)>>63))
#define expF64UI( a ) ((int_fast16_t) ((a)>>52) & 0x7FF)
#define fracF64UI( a ) ((a) & UINT64_C( 0x000FFFFFFFFFFFFF ))
#define packToF64UI( sign, exp, sig ) ((uint64_t) (((uint_fast64_t) (sign)<<63) + ((uint_fast64_t) (exp)<<52) + (sig)))

#define isNaNF64UI( a ) (((~(a) & UINT64_C( 0x7FF0000000000000 )) == 0) && ((a) & UINT64_C( 0x000FFFFFFFFFFFFF )))

struct exp16_sig64 { int_fast16_t exp; uint_fast64_t sig; };
struct exp16_sig64 softfloat_normSubnormalF64Sig( uint_fast64_t );

float64_t softfloat_roundPackToF64( bool, int_fast16_t, uint_fast64_t );
float64_t softfloat_normRoundPackToF64( bool, int_fast16_t, uint_fast64_t );

float64_t softfloat_addMagsF64( uint_fast64_t, uint_fast64_t, bool );
float64_t softfloat_subMagsF64( uint_fast64_t, uint_fast64_t, bool );
float64_t
 softfloat_mulAddF64(
     uint_fast64_t, uint_fast64_t, uint_fast64_t, uint_fast8_t );

/*----------------------------------------------------------------------------
*----------------------------------------------------------------------------*/
#define signExtF80UI64( a64 ) ((bool) ((uint16_t) (a64)>>15))
#define expExtF80UI64( a64 ) ((a64) & 0x7FFF)
#define packToExtF80UI64( sign, exp ) ((uint_fast16_t) (sign)<<15 | (exp))

#define isNaNExtF80UI( a64, a0 ) ((((a64) & 0x7FFF) == 0x7FFF) && ((a0) & UINT64_C( 0x7FFFFFFFFFFFFFFF )))


/*----------------------------------------------------------------------------
*----------------------------------------------------------------------------*/
#define signF128UI64( a64 ) ((bool) ((uint64_t) (a64)>>63))
#define expF128UI64( a64 ) ((int_fast32_t) ((a64)>>48) & 0x7FFF)
#define fracF128UI64( a64 ) ((a64) & UINT64_C( 0x0000FFFFFFFFFFFF ))
#define packToF128UI64( sign, exp, sig64 ) (((uint_fast64_t) (sign)<<63) + ((uint_fast64_t) (exp)<<48) + (sig64))

#define isNaNF128UI( a64, a0 ) (((~(a64) & UINT64_C( 0x7FFF000000000000 )) == 0) && (a0 || ((a64) & UINT64_C( 0x0000FFFFFFFFFFFF ))))


#endif

