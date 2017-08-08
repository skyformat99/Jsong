// =============================================================================
// <number/compute.h>
//
// JSON number computation definitions.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#ifndef H_F663D62C63D34BF1B7FE2451A6B1BD72
#define H_F663D62C63D34BF1B7FE2451A6B1BD72

// -----------------------------------------------------------------------------

#include <quantum/float.h>
#include <quantum/flint.h>
#include <quantum/math.h>

// Integral number -------------------------------------------------------------

#define JSON_NUM_UINT_DIG UINTMAX_DIG
#define JSON_NUM_UINT_MAX UINTMAX_MAX
#define JSON_NUM_INT_MAX INTMAX_MAX

// Floating point number -------------------------------------------------------

#define JSON_NUM_HUGE_VAL HUGE_VAL

#define JSON_NUM_EXP10_MIN DBL_MIN_10_EXP
#define JSON_NUM_EXP10_MAX DBL_MAX_10_EXP

#define JSON_NUM_FLINT_DIG10 flint_dbl_dig_max()
#define JSON_NUM_FLINT_POW10 flint_dbl_pow_max()

// -----------------------------------------------------------------------------
// Big number computation definitions
#if JSON(BIG_NUMBERS)
  // Integral
  #if HAVE(INT128)
    #define JSON_NUM_BIG_UINT_DIG UINT128_DIG
    #define JSON_NUM_BIG_UINT_MAX UINT128_MAX
    #define JSON_NUM_BIG_INT_MAX INT128_MAX
  #else
    #define JSON_NUM_BIG_INT_DIG JSON_NUM_INT_DIG
    #define JSON_NUM_BIG_INT_MAX JSON_NUM_INT_MAX
  #endif

  // Floating point
  #if defined(FLT128_MAX)
    #define JSON_NUM_BIG_HUGE_VAL HUGE_VALQ

    #define JSON_NUM_BIG_EXP10_MIN FLT128_MIN_10_EXP
    #define JSON_NUM_BIG_EXP10_MAX FLT128_MAX_10_EXP

    #define JSON_NUM_BIG_FLINT_DIG10 flint_f128_dig_max()
    #define JSON_NUM_BIG_FLINT_POW10 flint_f128_pow_max()
  #elif defined(FLT80_MAX)
    #define JSON_NUM_BIG_HUGE_VAL HUGE_VALW

    #define JSON_NUM_BIG_EXP10_MIN FLT80_MIN_10_EXP
    #define JSON_NUM_BIG_EXP10_MAX FLT80_MAX_10_EXP

    #define JSON_NUM_BIG_FLINT_DIG10 flint_f80_dig_max()
    #define JSON_NUM_BIG_FLINT_POW10 flint_f80_pow_max()
  #else
    #define JSON_NUM_BIG_HUGE_VAL HUGE_VALL

    #define JSON_NUM_BIG_EXP10_MIN LDBL_MIN_10_EXP
    #define JSON_NUM_BIG_EXP10_MAX LDBL_MAX_10_EXP

    #define JSON_NUM_BIG_FLINT_DIG10 flint_ldbl_dig_max()
    #define JSON_NUM_BIG_FLINT_POW10 flint_ldbl_pow_max()
  #endif

  #define JSON_NUM_DIG_MAX (JSON_NUM_BIG_EXP10_MAX + 1)
#else
  #define JSON_NUM_DIG_MAX (JSON_NUM_EXP10_MAX + 1)
#endif

// -----------------------------------------------------------------------------
// `pow()` functions
// -----------------------------------------------------------------------------

#if (DBL_BINARY == 64) || (FLT_BINARY == 32)
  #define json_pow(ten, exp) flt_pow10_tbl[exp]
#else
  #define json_pow(ten, exp) pow (10.0, exp)
#endif

#if JSON(BIG_NUMBERS)
  #if defined(FLT128_MAX)
    #define json_big_pow(ten, exp) powq (10.0q, exp)
  #elif defined(FLT80_MAX)
    #define json_big_pow(ten, exp) __builtin_powl (10.0w, exp)
  #else
    #define json_big_pow(ten, exp) powl (10.0l, exp)
  #endif
#endif

// -----------------------------------------------------------------------------

#endif
