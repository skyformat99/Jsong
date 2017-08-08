// =============================================================================
// <number/number.h>
//
// JSON number definitions.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#ifndef H_C9825340C8AD4FF7851ED13ED5885F4E
#define H_C9825340C8AD4FF7851ED13ED5885F4E

// -----------------------------------------------------------------------------

#if JSON(STREAM)
  #define json_num_init() json_buf_init (sizeof (json_num_istr_t))
#else
  #define json_num_init()
#endif

// -----------------------------------------------------------------------------

#if JSON(SAX)
  #include "sax.h"
#else
  #include "dom.h"
#endif

// -----------------------------------------------------------------------------

#define JSON_NUM_MANT_LEN_MAX 120u
#define JSON_NUM_EXP_LEN_MAX 5u

// -----------------------------------------------------------------------------

#if !JSON(STRING_NUMBERS)
  #include "compute.h"
#endif

// -----------------------------------------------------------------------------

#endif
