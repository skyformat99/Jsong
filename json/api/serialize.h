// =============================================================================
// <api/serialize.h>
//
// JSON serialization private definitions.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#ifndef H_9DC0B808D0C647E9A9E4F3074B317BDF
#define H_9DC0B808D0C647E9A9E4F3074B317BDF

// -----------------------------------------------------------------------------

#include "macros.h"

// -----------------------------------------------------------------------------

#ifndef JSON_FLT_BUF_SIZE
  #define JSON_FLT_BUF_SIZE 32u
#endif

#if (JSON_FLT_BUF_SIZE < 32)
  #error "Invalid definition of `JSON_FLT_BUF_SIZE`"
#endif

// -----------------------------------------------------------------------------

#ifndef JSON_BIG_FLT_BUF_SIZE
  #define JSON_BIG_FLT_BUF_SIZE 64u
#endif

#if (JSON_BIG_FLT_BUF_SIZE < 64)
  #error "Invalid definition of `JSON_BIG_FLT_BUF_SIZE`"
#endif

// -----------------------------------------------------------------------------

#endif
