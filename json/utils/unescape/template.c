// =============================================================================
// <utils/unescape/template.c>
//
// JSON string unescaping template.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#include "../../parse/macros.h"

// -----------------------------------------------------------------------------

if (true)
{
  int need;

#if CPU(SSE2)
  #include "skip/simd.c"
#elif JSON(INPLACE)
  while (json_peek (4u))
  {
    if (unlikely (!json_prefix (str_tbl) (c))) break;
    if (unlikely (!json_prefix (str_tbl) (j[1]))) {c = j[1]; j++; break;}
    if (unlikely (!json_prefix (str_tbl) (j[2]))) {c = j[2]; j += 2; break;}
    if (unlikely (!json_prefix (str_tbl) (j[3]))) {c = j[3]; j += 3; break;}

    j += 4;
  }

  #if JSON(SIZE)
    size = (size_t)(j - in);
  #else
    o = j;
  #endif
#endif

loop (unescape);
  if (unlikely (c == '\\'))
  {
    int expect;

    if (json_broken (2u))
    {
      expect = 2 - (e - j);

tooshort:
      *end = (u8*)j;
#if JSON(SIZE)
      *num = size;
#else
      *num = (size_t)(o - out);
#endif

      return expect;
    }

    c = j[1];

#if ENABLED(USON)
    if (unlikely (c == 'x'))
    {
      if (json_broken (4u))
      {
        expect = 4 - (e - j);
        goto tooshort;
      }

  #if JSON(EXPLICIT)
      uint x;

      uint u = chr_xdig_to_int (j[2]);
      c = u << 4;
      x = u;

      u = chr_xdig_to_int (j[3]);
      c |= u;
      x |= u;

      if (unlikely (x > 0xFu)) goto malformed;
  #else
      uint u = chr_xdig_to_int (j[2]);
      if (unlikely (u > 0xFu)) goto malformed;
      c = u << 4;

      u = chr_xdig_to_int (j[3]);
      if (unlikely (u > 0xFu)) goto malformed;
      c |= u;
  #endif

  #if JSON(SIZE)
      size++;
  #else
    #if !JSON(INPLACE)
      if (unlikely (o == m))
      {
        need = 1;
        goto needspace;
      }
    #endif

      *o++ = c;
  #endif

      j += 4;
    }

#else // USON ][

    if (unlikely (c == 'u'))
    {
      if (json_broken (6u))
      {
        expect = 6 - (e - j);
        goto tooshort;
      }

  #if JSON(EXPLICIT)
      uint x;

      uint u = chr_xdig_to_int (j[2]);
      c = u << 12;
      x = u;

      u = chr_xdig_to_int (j[3]);
      c |= u << 8;
      x |= u;

      u = chr_xdig_to_int (j[4]);
      c |= u << 4;
      x |= u;

      u = chr_xdig_to_int (j[5]);
      c |= u;
      x |= u;

      if (unlikely (x > 0xFu)) goto malformed;
  #else
      uint u = chr_xdig_to_int (j[2]);
      if (unlikely (u > 0xFu)) goto malformed;
      c = u << 12;

      u = chr_xdig_to_int (j[3]);
      if (unlikely (u > 0xFu)) goto malformed;
      c |= u << 8;

      u = chr_xdig_to_int (j[4]);
      if (unlikely (u > 0xFu)) goto malformed;
      c |= u << 4;

      u = chr_xdig_to_int (j[5]);
      if (unlikely (u > 0xFu)) goto malformed;
      c |= u;
  #endif

      if (likely (utf16_chr_is8_lead1 (c)))
      {
  #if JSON(SIZE)
        size++;
  #else
    #if !JSON(INPLACE)
        if (unlikely (o == m))
        {
          need = 1;
          goto needspace;
        }
    #endif

        *o++ = c;
  #endif

        j += 6;
      }
      else if (likely (utf16_chr_is8_lead2 (c)))
      {
  #if JSON(SIZE)
        size += 2u;
  #else
    #if !JSON(INPLACE)
        if (unlikely ((size_t)(m - o) < 2u))
        {
          need = 2 - (m - o);
          goto needspace;
        }
    #endif

        o[0] = utf8_codep_decomp_head2 (c);
        o[1] = utf8_codep_decomp_tail1 (c);

        o += 2;
  #endif

        j += 6;
      }
      else if (likely (!utf16_chr_is_surr (c)))
      {
  #if JSON(SIZE)
        size += 3u;
  #else
    #if !JSON(INPLACE)
        if (unlikely ((size_t)(m - o) < 3u))
        {
          need = 3 - (m - o);
          goto needspace;
        }
    #endif

        o[0] = utf8_codep_decomp_head3 (c);
        o[1] = utf8_codep_decomp_tail2 (c);
        o[2] = utf8_codep_decomp_tail1 (c);

        o += 3;
  #endif

        j += 6;
      }
      else if (unlikely (utf16_chr_is_surr_high (c)))
      {
        if (json_broken (12u))
        {
          expect = 12 - (e - j);
          goto tooshort;
        }

        uint sc;

  #if JSON(EXPLICIT)
        if (unlikely (!buf_equ2 (j + 6, '\\', 'u'))) goto malformed;

        u = chr_xdig_to_int (j[8]);
        sc = u << 12;
        x = u;

        u = chr_xdig_to_int (j[9]);
        sc |= u << 8;
        x |= u;

        u = chr_xdig_to_int (j[10]);
        sc |= u << 4;
        x |= u;

        u = chr_xdig_to_int (j[11]);
        sc |= u;
        x |= u;

        if (unlikely (x > 0xFu)) goto malformed;
  #else
        if (unlikely ((j[6] != '\\') || (j[7] != 'u'))) goto malformed;

        u = chr_xdig_to_int (j[8]);
        if (unlikely (u > 0xFu)) goto malformed;
        sc = u << 12;

        u = chr_xdig_to_int (j[9]);
        if (unlikely (u > 0xFu)) goto malformed;
        sc |= u << 8;

        u = chr_xdig_to_int (j[10]);
        if (unlikely (u > 0xFu)) goto malformed;
        sc |= u << 4;

        u = chr_xdig_to_int (j[11]);
        if (unlikely (u > 0xFu)) goto malformed;
        sc |= u;
  #endif

        if (unlikely (!utf16_chr_is_surr_low (sc))) goto malformed;

        uf32 a = utf16_surr_to32 (c, sc);

  #if JSON(SIZE)
        size += 4u;
  #else
    #if !JSON(INPLACE)
        if (unlikely ((size_t)(m - o) < 4u))
        {
          need = 4 - (m - o);
          goto needspace;
        }
    #endif

        o[0] = utf8_codep_decomp_head4 (a);
        o[1] = utf8_codep_decomp_tail3 (a);
        o[2] = utf8_codep_decomp_tail2 (a);
        o[3] = utf8_codep_decomp_tail1 (a);

        o += 4;
  #endif

        j += 12;
      }
      else goto malformed;
    }
#endif // !USON
    else
    {
      c = json_prefix (chr_unescape) (c);

      if (unlikely (c > 0x7Fu)) goto malformed;

#if JSON(SIZE)
      size++;
#else
  #if !JSON(INPLACE)
      if (unlikely (o == m))
      {
        need = 1;
        goto needspace;
      }
  #endif

      *o++ = c;
#endif

      j += 2;
    }
  }
  else
  {
#if ENABLED(USON)
    if (unlikely (!chr_is_print_strict (c)))
#else
    if (unlikely (!chr_is_print (c)))
#endif
    {
#if !JSON(EXPLICIT)
      if (c == '\0') break;
#endif

malformed:
      *end = (u8*)j;
#if JSON(SIZE)
      *num = size;
#else
      *num = (size_t)(o - out);
#endif

      return INT_MIN;
    }

    #include "simd.c"

#if JSON(SIZE)
    size++;
#else
  #if !JSON(INPLACE)
    if (unlikely (o == m))
    {
      need = 1;

needspace:
      *end = (u8*)j;
      *num = (size_t)(o - out);

      return need;
    }
  #endif

    *o++ = c;
#endif

    j++;
  }

  if (json_end()) break;

  c = *j;

  repeat (unescape);
}

// -----------------------------------------------------------------------------

#undef JSON_EXPLICIT
#undef JSON_INPLACE
#undef JSON_SIZE

// -----------------------------------------------------------------------------

#include "../../parse/undef.h"
