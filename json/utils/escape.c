// =============================================================================
// <utils/escape.c>
//
// JSON string escaping.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#include <quantum/build.h>
#include <quantum/core.h>

#include <quantum/integer.h>

// -----------------------------------------------------------------------------

#include "../api/core.h"
#include "../api/macros.h"

#include "../utils.h"
#include "../utils/misc.h"

// -----------------------------------------------------------------------------
// Explicit string
// -----------------------------------------------------------------------------

int json_prefix (escape_length) (const u8* in, size_t len
, u8** end, size_t* num)
{
  register uint c;

  const u8* j = in;
  const u8* e = in + len;

  size_t size = 0;

  while (j != e)
  {
    c = *j;

    #define JSON_SIZE

    #include "escape/template.c"
  }

  *end = (u8*)j;
  *num = size;

  return 0;
}

// -----------------------------------------------------------------------------

int json_prefix (escape) (const u8* restrict in, size_t len
, u8* restrict out, size_t size
, u8** end, size_t* num)
{
  register uint c;

  const u8* restrict j = in;
  const u8* e = in + len;

  u8* restrict o = out;
  const u8* m = out + size;

  while (j != e)
  {
    c = *j;

    #include "escape/template.c"
  }

  *end = (u8*)j;
  *num = o - out;

  return 0;
}

// -----------------------------------------------------------------------------
// Implicit string
// -----------------------------------------------------------------------------

int json_prefix (escapei_length) (const u8* in
, u8** end, size_t* num)
{
  register uint c;

  const u8* j = in;

  size_t size = 1u;

  while ((c = *j) != '\0')
  {
    #define JSON_SIZE

    #include "escape/template.c"
  }

  *end = (u8*)j;
  *num = size;

  return 0;
}

// -----------------------------------------------------------------------------

int json_prefix (escapei) (const u8* restrict in
, u8* restrict out, size_t size
, u8** end, size_t* num)
{
  register uint c;

  const u8* restrict j = in;

  u8* restrict o = out;
  const u8* m = out + size;

  while ((c = *j) != '\0')
  {
    #include "escape/template.c"
  }

  if (o == m)
  {
    *end = (u8*)j;
    *num = o - out;

    return 1;
  }

  *o = '\0';

  *end = (u8*)j;
  *num = o - out;

  return 0;
}
