// =============================================================================
// <utils/unescape.c>
//
// JSON string unescaping.
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
// Explicit string
// -----------------------------------------------------------------------------

int json_prefix (unescape_length) (const u8* in, size_t len
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
    #define JSON_EXPLICIT

    #include "unescape/template.c"
  }

  *end = (u8*)j;
  *num = size;

  return 0;
}

// -----------------------------------------------------------------------------

int json_prefix (unescape) (const u8* restrict in, size_t len
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

    #define JSON_EXPLICIT

    #include "unescape/template.c"
  }

  *end = (u8*)j;
  *num = o - out;

  return 0;
}

// -----------------------------------------------------------------------------
// Implicit string
// -----------------------------------------------------------------------------

int json_prefix (unescapei_length) (const u8* in
, u8** end, size_t* num)
{
  register uint c;

  const u8* j = in;
  const u8* e;

  size_t size = 1u;

  while ((c = *j) != '\0')
  {
    #define JSON_SIZE

    #include "unescape/template.c"
  }

  *end = (u8*)j;
  *num = size;

  return 0;
}

// -----------------------------------------------------------------------------

int json_prefix (unescapei) (const u8* restrict in
, u8* restrict out, size_t size
, u8** end, size_t* num)
{
  register uint c;

  const u8* restrict j = in;
  const u8* e;

  u8* restrict o = out;
  const u8* m = out + size;

  while ((c = *j) != '\0')
  {
    #include "unescape/template.c"
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

// -----------------------------------------------------------------------------
// In-place unescaping
// -----------------------------------------------------------------------------

int json_prefix (unescape_inplace) (u8* in, size_t len
, u8** end, size_t* num)
{
  register uint c;

  u8* j = in;
  const u8* e = in + len;

  u8* o = in;
  const u8* out = in;

  while (j != e)
  {
    c = *j;

    #define JSON_EXPLICIT
    #define JSON_INPLACE

    #include "unescape/template.c"
  }

  *end = j;
  *num = o - out;

  return 0;
}

// -----------------------------------------------------------------------------

int json_prefix (unescapei_inplace) (u8* in
, u8** end, size_t* num)
{
  register uint c;

  u8* j = in;
  const u8* e;

  u8* o = in;
  const u8* out = in;

  while ((c = *j) != '\0')
  {
    #define JSON_INPLACE

    #include "unescape/template.c"
  }

  *o = '\0';

  *end = j;
  *num = o - out;

  return 0;
}
