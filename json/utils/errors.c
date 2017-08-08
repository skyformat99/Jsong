// =============================================================================
// <utils/errors.c>
//
// JSON error string.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#include <quantum/build.h>
#include <quantum/core.h>

#include <quantum/unicode.h>

// -----------------------------------------------------------------------------

#include "errors.h"

// -----------------------------------------------------------------------------

const u8* json_errors[12] =
{
  U8("JSON is OK"),
  U8("Interrupted by callback"),
  U8("Out of memory"),
  U8("Maximum depth reached"),
  U8("Illegal string character"),
  U8("Illegal escape sequence character"),
  U8("Illegal Unicode escape sequence character"),
  U8("Illegal number character"),
  U8("Unexpected token"),
  U8("Expected object property key"),
  U8("Expected object property value"),
  U8("Expected more data")
};
