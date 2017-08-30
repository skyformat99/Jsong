// =============================================================================
// <parse.c>
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#include <quantum/build.h>
#include <quantum/core.h>

#include <quantum/integer.h>
#include <quantum/memory.h>
#include <quantum/buffer.h>
#include <quantum/character.h>
#include <quantum/string.h>

#include <utf/utf.h>

// -----------------------------------------------------------------------------

#define MEM_POOL_STATIC

#include "api/core.h"
#include "api/macros.h"
#include "api/api.h"

// -----------------------------------------------------------------------------

#include "utils/misc.h"
#include "utils/errors.h"

// -----------------------------------------------------------------------------

#include "buffer/buffer.h"
#include "string/string.h"
#include "number/number.h"
#include "primitive/primitive.h"
#include "wspace/wspace.h"

#if ENABLED(USON)
  #include "identifier/identifier.h"
  #include "verbatim/verbatim.h"
  #include "data/data.h"
#endif

// -----------------------------------------------------------------------------

#include "parse/types.h"

// -----------------------------------------------------------------------------

#if !JSON(SAX)
  #if (MEM_POOL_ALIGNMENT == 0) && (INTPTR_BIT <= 64)
    #define MEM_POOL_ALIGNMENT 8u
  #endif

  #include <quantum/memory/pool.c>
#endif

// -----------------------------------------------------------------------------

#if JSON(SAX)
  #if JSON(STREAM)
void jsax_prefix (init_stream) (jsnp_t* jsnp, u8* buf, size_t size)
  #elif JSON(EXPLICIT)
void jsax_prefix (init) (jsnp_t* jsnp, u8* buf, size_t size)
  #else
void jsax_prefix (initi) (jsnp_t* jsnp, u8* buf, size_t size)
  #endif
#else
  #if JSON(STREAM)
void json_prefix (init_stream) (jsnp_t* jsnp, mem_pool_t* pool)
  #elif JSON(EXPLICIT)
void json_prefix (init) (jsnp_t* jsnp, mem_pool_t* pool)
  #else
void json_prefix (initi) (jsnp_t* jsnp, mem_pool_t* pool)
  #endif
#endif
{
  #include "parse/initialize.c"
}

// -----------------------------------------------------------------------------

#if JSON(SAX)
  #if JSON(STREAM)
bool jsax_prefix (parse_stream) (jsnp_t* jsnp, u8* json, size_t size, bool last)
  #elif JSON(EXPLICIT)
bool jsax_prefix (parse) (jsnp_t* jsnp, u8* json, size_t size)
  #else
bool jsax_prefix (parsei) (jsnp_t* jsnp, u8* json)
  #endif
#else
  #if JSON(STREAM)
json_node_t json_prefix (parse_stream) (jsnp_t* jsnp, u8* json, size_t size, bool last)
  #elif JSON(EXPLICIT)
json_node_t json_prefix (parse) (jsnp_t* jsnp, u8* json, size_t size)
  #else
json_node_t json_prefix (parsei) (jsnp_t* jsnp, u8* json)
  #endif
#endif
{
  #include "parse/parse.c"
}
