// =============================================================================
// <utils.c>
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

#include "utils/support.h"
#include "utils/misc.h"
#include "utils.h"

// -----------------------------------------------------------------------------
// Remove comments (`# ...`, `// ...`, and `/* ... */`)
// -----------------------------------------------------------------------------

void json_uncomment_stream (u8* json, size_t size, uint* state)
{
  #define JSON_UNCOMMENT_STREAM

  #include "utils/uncomment.c"
}

// -----------------------------------------------------------------------------

void json_uncomment (u8* json, size_t size)
{
  #define JSON_UNCOMMENT_EXPLICIT

  #include "utils/uncomment.c"
}

// -----------------------------------------------------------------------------

void json_uncommenti (u8* json)
{
  #include "utils/uncomment.c"
}

// -----------------------------------------------------------------------------
// Escape UTF-8 string
// -----------------------------------------------------------------------------

int json_escape_length (const u8* restrict src, size_t len, u8** end, size_t* num)
{
  register uint c;

  const u8* restrict s = src;
  const u8* restrict e = src + len;

  size_t size = 0;

  while (s != e)
  {
    c = *s;

    #define JSON_ESCAPE_SIZE

    #include "utils/escape.c"
  }

  *end = (u8*)s;
  *num = size;

  return 0;
}

// -----------------------------------------------------------------------------

int json_escape (const u8* restrict src, size_t len, u8* restrict dst
, size_t size, u8** end, size_t* num)
{
  register uint c;

  const u8* restrict s = src;
  const u8* restrict e = src + len;

  u8* restrict d = dst;
  u8* restrict m = dst + size;

  while (s != e)
  {
    c = *s;

    #include "utils/escape.c"
  }

  *end = (u8*)s;
  *num = (size_t)(d - dst);

  return 0;
}

// -----------------------------------------------------------------------------
// Escape null-terminated UTF-8 string
// -----------------------------------------------------------------------------

int json_escapei_length (const u8* restrict src, u8** end, size_t* num)
{
  register uint c;

  const u8* restrict s = src;

  size_t size = 1u;

  while ((c = *s))
  {
    #define JSON_ESCAPE_SIZE

    #include "utils/escape.c"
  }

  *end = (u8*)s;
  *num = size;

  return 0;
}

// -----------------------------------------------------------------------------

int json_escapei (const u8* restrict src, u8* restrict dst, size_t size
, u8** end, size_t* num)
{
  register uint c;

  const u8* restrict s = src;

  u8* restrict d = dst;
  u8* restrict m = dst + size;

  while ((c = *s))
  {
    #include "utils/escape.c"
  }

  if (d == m)
  {
    *end = (u8*)s;
    *num = (size_t)(d - dst);

    return 1;
  }

  *d = '\0';

  *end = (u8*)s;
  *num = (size_t)(d - dst);

  return 0;
}

// -----------------------------------------------------------------------------
// Unescape JSON string
// -----------------------------------------------------------------------------

int json_unescape_length (const u8* restrict src, size_t len, u8** end
, size_t* num)
{
  register uint c;

  const u8* restrict s = src;
  const u8* restrict e = src + len;

  size_t size = 0;

  while (s != e)
  {
    #define JSON_UNESCAPE_SIZE
    #define JSON_UNESCAPE_EXPLICIT

    #include "utils/unescape.c"
  }

  *end = (u8*)s;
  *num = size;

  return 0;
}

// -----------------------------------------------------------------------------

int json_unescape (const u8* restrict src, size_t len, u8* restrict dst
, size_t size, u8** end, size_t* num)
{
  register uint c;

  const u8* restrict s = src;
  const u8* restrict e = src + len;

  u8* restrict o = dst;
  u8* restrict b = dst + size;

  while (s != e)
  {
    #define JSON_UNESCAPE_EXPLICIT

    #include "utils/unescape.c"
  }

  *end = (u8*)s;
  *num = (size_t)(o - dst);

  return 0;
}

// -----------------------------------------------------------------------------
// Unescape null-terminated JSON string
// -----------------------------------------------------------------------------

int json_unescapei_length (const u8* restrict src, u8** end, size_t* num)
{
  register uint c;

  const u8* restrict s = src;
  const u8* restrict e;

  size_t size = 1u;

  while ((c = *s))
  {
    #define JSON_UNESCAPE_SIZE

    #include "utils/unescape.c"
  }

  *end = (u8*)s;
  *num = size;

  return 0;
}

// -----------------------------------------------------------------------------

int json_unescapei (const u8* restrict src, u8* restrict dst, size_t size
, u8** end, size_t* num)
{
  register uint c;

  const u8* restrict s = src;
  const u8* restrict e;

  u8* restrict o = dst;
  u8* restrict b = dst + size;

  while ((c = *s))
  {
    #include "utils/unescape.c"
  }

  if (o == b)
  {
    *end = (u8*)s;
    *num = (size_t)(o - dst);

    return 1;
  }

  *o = '\0';

  *end = (u8*)s;
  *num = (size_t)(o - dst);

  return 0;
}

// -----------------------------------------------------------------------------
// In-place unescaping
// -----------------------------------------------------------------------------

int json_unescape_inplace (u8* src, size_t len, u8** end, size_t* num)
{
  register uint c;

  u8* s = src;
  const u8* e = src + len;

  u8* o = src;

  while (s != e)
  {
    #define JSON_UNESCAPE_EXPLICIT
    #define JSON_UNESCAPE_INPLACE

    #include "utils/unescape.c"
  }

  *end = s;
  *num = (size_t)(o - src);

  return 0;
}

// -----------------------------------------------------------------------------

int json_unescapei_inplace (u8* src, u8** end, size_t* num)
{
  register uint c;

  u8* s = src;
  const u8* e;

  u8* o = src;

  while ((c = *s))
  {
    #define JSON_UNESCAPE_INPLACE

    #include "utils/unescape.c"
  }

  *o = '\0';

  *end = s;
  *num = (size_t)(o - src);

  return 0;
}
