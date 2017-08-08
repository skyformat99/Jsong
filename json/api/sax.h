// =============================================================================
// <api/sax.h>
//
// JSON SAX API private definitions.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#ifndef H_600DCA9FBF8340978AF0610838650659
#define H_600DCA9FBF8340978AF0610838650659

// -----------------------------------------------------------------------------

#include "../sax.h"

// =============================================================================
// Constants
// -----------------------------------------------------------------------------
// Depth stack constraint
#define JSON_DEPTH_MAX (JSON_DEPTH_SIZE * INT_BIT)

// =============================================================================
// Macros
// -----------------------------------------------------------------------------
// Event handler execution
#define jsax_callback(fn, ...) do\
{                                \
  if (unlikely (!((fn)(__VA_ARGS__)))) json_error (JSON_ERROR_CALLBACK);\
} while (0)

// -----------------------------------------------------------------------------
// SAX API JSON values do not need to be initialized
#define json_val_init(elmnt, name)

// -----------------------------------------------------------------------------

#endif
