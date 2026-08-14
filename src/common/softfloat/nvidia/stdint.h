#ifndef _NV_STDINT_H_
#define _NV_STDINT_H_

#if defined(NV_KERNEL_INTERFACE_LAYER) && defined(NV_LINUX)
  #include <linux/types.h>

  /* Linux lacks the '*fast/least*_t' variants. */
  typedef  int8_t   int_fast8_t;
  typedef uint8_t  uint_fast8_t;

  typedef  int16_t  int_fast16_t;
  typedef uint16_t uint_fast16_t;

  typedef  int32_t  int_fast32_t;
  typedef uint32_t uint_fast32_t;

  typedef  int64_t  int_fast64_t;
  typedef uint64_t uint_fast64_t;

  typedef  int8_t   int_least8_t;
  typedef uint8_t  uint_least8_t;

  typedef  int16_t  int_least16_t;
  typedef uint16_t uint_least16_t;

  typedef  int32_t  int_least32_t;
  typedef uint32_t uint_least32_t;

  typedef  int64_t  int_least64_t;
  typedef uint64_t uint_least64_t;

  #define  INT64_C(x) S64_C(x)
  #define UINT64_C(x) U64_C(x)
#else
  #include_next <stdint.h>
#endif

#endif // _NV_STDINT_H_
