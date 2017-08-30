// =============================================================================
// <api/dom.h>
//
// JSON DOM API private definitions.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#ifndef H_896DE4646B0D4BAAB899BC8C8E87066B
#define H_896DE4646B0D4BAAB899BC8C8E87066B

// -----------------------------------------------------------------------------

#include "../dom.h"

// =============================================================================
// Constants
// -----------------------------------------------------------------------------
// Memory pool constraints
// -----------------------------------------------------------------------------

#if (JSON_POOL_SIZE == 0)
  #define JSON_POOL_SIZE (64u * 1024u)
#endif

#if (JSON_POOL_SIZE < 256)
  #error "Invalid definition of `JSON_POOL_SIZE`"
#endif

// -----------------------------------------------------------------------------

#if (JSON_POOL_MAX == 0)
  #define JSON_POOL_MAX 4096u
#endif

#if (JSON_POOL_MAX < 256)
  #error "Invalid definition of `JSON_POOL_MAX`"
#endif

// -----------------------------------------------------------------------------
// Packed string definitions
// -----------------------------------------------------------------------------

#if JSON(PACK_STRINGS)
  #define JSON_PSTR_SIZE (max2 (sizeof (json_str_t), max2 (sizeof (uint) * 4u, 16u)) - sizeof (uint))
#else
  #define JSON_PSTR_SIZE 0
#endif

// =============================================================================
// Macros
// -----------------------------------------------------------------------------
// JSON value object allocation
#define json_val_init(elmnt, name) do\
{                                    \
  (elmnt)->val.name = mem_pool_alloc (jsnp->pool, obj_size ((elmnt)->val.name));\
  if (unlikely ((elmnt)->val.name == null)) json_error (JSON_ERROR_MEMORY);\
} while (0)

// -----------------------------------------------------------------------------

#endif
