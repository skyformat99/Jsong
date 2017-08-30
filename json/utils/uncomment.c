// =============================================================================
// <utils/uncomment.c>
//
// JSON uncommenting.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#include <quantum/build.h>
#include <quantum/core.h>

#include <quantum/integer.h>
#include <quantum/character.h>
#include <quantum/buffer.h>
#include <quantum/simd.h>

#include <utf/utf.h>

// -----------------------------------------------------------------------------

#include "../api/core.h"
#include "../api/macros.h"

#include "../utils.h"
#include "../utils/misc.h"

// -----------------------------------------------------------------------------

#if JSON(STREAM)
  void json_prefix (uncomment_stream) (u8* json, size_t size, json_prefix (comment_t)* state)
#elif JSON(EXPLICIT)
  void json_prefix (uncomment) (u8* json, size_t size)
#else
  void json_prefix (uncommenti) (u8* json)
#endif
{
  #include "uncomment/template.c"
}
