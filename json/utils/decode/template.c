// =============================================================================
// <utils/decode.c>
//
// USON Base64 string decoding template.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#include "../../parse/macros.h"

// -----------------------------------------------------------------------------

if (true)
{
  int expect;
  int need;

#if !JSON(EXPLICIT) && !CPU(PAGE_SIZE)
loop (data);
#endif

  while (json_wspace (c))
  {
#if JSON(EXPLICIT) || CPU(PAGE_SIZE)
data_wspace:
#endif

    j++;

    if (json_end()) goto done;

    c = *j;
  }

#if JSON(EXPLICIT) || CPU(PAGE_SIZE)
  #if !JSON(EXPLICIT)
    const u8* e;

loop (data);
    e = ptr_align_ceil (CPU_PAGE_SIZE, j);
  #endif

  #include "../data/simd.c"

  while ((size_t)(e - j) >= 4u)
  {
    uint b0 = uson_base64_tbl[j[0]];
    uint b1 = uson_base64_tbl[j[1]];
    uint b2 = uson_base64_tbl[j[2]];
    uint b3 = uson_base64_tbl[j[3]];

    if (unlikely ((b0 | b1 | b2 | b3) > 63u)) break;

  #if JSON(SIZE)
    size += 3;
  #else
    #if !JSON(INPLACE)
      if ((size_t)(m - o) < 3u)
      {
        need = 3u - (m - o);
        goto needspace;
      }
    #endif

    o[0] = (b0 << 2) | (b1 >> 4);
    o[1] = (b1 << 4) | (b2 >> 2);
    o[2] = (b2 << 6) | b3;

    o += 3;
  #endif

    j += 4;
  }

  if (json_end()) break;

data_skip:
  if (likely (json_wspace (j[0]))) goto data_wspace;
#endif // JSON(EXPLICIT) || CPU(PAGE_SIZE)

  uint b = uson_base64_tbl[j[0]];

  if (unlikely (b > 63u))
  {
#if !JSON(EXPLICIT)
    if (likely (j[0] == '\0')) break;
#endif

data_error:
    *end = (u8*)j;

#if JSON(SIZE)
    *num = size;
#else
    *num = o - out;
#endif

    return INT_MIN;
  }

  if (json_broken (4u))
  {
    *end = (u8*)j;

#if JSON(SIZE)
    *num = size;
#else
    *num = o - out;
#endif

    return 4 - (e - j);
  }

  c = b;
  b = uson_base64_tbl[j[1]];

  if (unlikely (b > 63u)) goto data_error;

#if JSON(SIZE)
  size++;
#else
  #if !JSON(INPLACE)
    if (m == o)
    {
need_space_count:
      for (need = 2; need < 4; need++)
      {
        if (uson_base64_tbl[j[need]] > 63u) break;
      }

      need = (need - 1) - (m - o);

needspace:
      *end = (u8*)j;

    #if JSON(SIZE)
      *num = size;
    #else
      *num = o - out;
    #endif

      return need;
    }
  #endif

  o[0] = (c << 2) | (b >> 4);
#endif

  c = b;
  b = uson_base64_tbl[j[2]];

  if (unlikely (b > 63u))
  {
    if (unlikely (b != 64u)) goto data_error;

    if (unlikely (uson_base64_tbl[j[3]] != 64u)) goto data_error;

#if !JSON(SIZE)
    o++;
#endif

    j += 4;
  }
  else
  {
#if JSON(SIZE)
    size++;
#else
  #if !JSON(INPLACE)
    if ((size_t)(m - o) < 2u) goto need_space_count;
  #endif

    o[1] = (c << 4) | (b >> 2);
#endif

    c = b;
    b = uson_base64_tbl[j[3]];

    if (unlikely (b > 63))
    {
      if (unlikely (b != 64)) goto data_error;

#if !JSON(SIZE)
      o += 2;
#endif

      j += 4;
    }
    else
    {
#if JSON(SIZE)
      size++;
#else
  #if !JSON(INPLACE)
    if ((size_t)(m - o) < 3u) goto need_space_count;
  #endif

      o[2] = (c << 6) | b;
#endif

#if !JSON(SIZE)
      o += 3;
#endif

      j += 4;
    }
  }

#if !JSON(EXPLICIT)
  repeat (data);
#endif

done:
  break;
}

// -----------------------------------------------------------------------------

#undef JSON_EXPLICIT
#undef JSON_INPLACE
#undef JSON_SIZE

// -----------------------------------------------------------------------------

#include "../../parse/undef.h"
