// =============================================================================
// <utils/encode.c>
//
// USON Base64 string encoding.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#include <quantum/build.h>
#include <quantum/core.h>

// -----------------------------------------------------------------------------

#include "../api/core.h"
#include "../api/macros.h"

#include "../utils.h"
#include "../utils/misc.h"

// -----------------------------------------------------------------------------
// Explicit string
// -----------------------------------------------------------------------------

int uson_encode_length (const u8* in, size_t len
, u8** end, size_t* num)
{
  const u8* j = in;

  size_t size = 0;

  while (true)
  {
    #define JSON_SIZE

    #include "encode/template.c"
  }

  *end = (u8*)j;
  *num = size;

  return 0;
}

// -----------------------------------------------------------------------------

int uson_encode (const u8* restrict in, size_t len
, u8* restrict out, size_t size
, u8** end, size_t* num)
{
  const u8* restrict j = in;

  u8* restrict o = out;
  const u8* m = out + size;

  while (true)
  {
    #include "encode/template.c"
  }

  *end = (u8*)j;
  *num = o - out;

  return 0;
}

// -----------------------------------------------------------------------------
// Implicit string
// -----------------------------------------------------------------------------

int uson_encodei_length (const u8* in
, u8** end, size_t* num)
{
  const u8* j = in;

  size_t size = 1u;

  while (true)
  {
    #define JSON_SIZE

    #include "encode/template.c"
  }

  *end = (u8*)j;
  *num = size;

  return 0;
}

// -----------------------------------------------------------------------------

int uson_encodei (const u8* restrict in
, u8* restrict out, size_t size
, u8** end, size_t* num)
{
  const u8* restrict j = in;

  u8* restrict o = out;
  const u8* m = out + size;

  while (true)
  {
    #include "encode/template.c"
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
