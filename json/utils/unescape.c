// =============================================================================
// <utils/unescape.c>
//
// JSON string unescaping template.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#if JSON(UNESCAPE_EXPLICIT)
  #define json_broken(n) unlikely ((e - s) < (n))
  #define json_peek(n) likely ((size_t)(e - s) >= (n))
  #define json_over(p, n) unlikely (((p) + (n)) >= e)
#else
  #define json_broken(n) false
  #define json_peek(n) true
  #define json_over(p, n) false
#endif

// -----------------------------------------------------------------------------

if (true)
{
  int need;

#if CPU(SSE2)
  #include "unescape/skip_simd.c"
#elif JSON(UNESCAPE_INPLACE)
  while (json_peek (4u))
  {
    if (unlikely (!json_str_tbl[c])) break;
    if (unlikely (!json_str_tbl[s[1]])) {c = s[1]; s++; break;}
    if (unlikely (!json_str_tbl[s[2]])) {c = s[2]; s += 2; break;}
    if (unlikely (!json_str_tbl[s[3]])) {c = s[3]; s += 3; break;}

    c = s[4];
    s += 4;
  }

  #if JSON(UNESCAPE_SIZE)
    size = (size_t)(s - src);
  #else
    o = s;
  #endif
#endif

loop (unescape);
  c = *s;

  if (unlikely (c == '\\'))
  {
    int expect;

    if (json_broken (2u))
    {
      expect = 2 - (e - s);

tooshort:
      *end = (u8*)s;
#if JSON(UNESCAPE_SIZE)
      *num = size;
#else
  #if !JSON(UNESCAPE_INPLACE)
      *num = (size_t)(o - dst);
  #else
      *num = (size_t)(o - src);
  #endif
#endif

      return expect;
    }

    c = s[1];

    if (unlikely (c == 'u'))
    {
      if (json_broken (6u))
      {
        expect = 6 - (e - s);
        goto tooshort;
      }

#if JSON(UNESCAPE_EXPLICIT)
      uint x;

      uint u = chr_xdig_to_int (s[2]);
      c = u << 12;
      x = u;

      u = chr_xdig_to_int (s[3]);
      c |= u << 8;
      x |= u;

      u = chr_xdig_to_int (s[4]);
      c |= u << 4;
      x |= u;

      u = chr_xdig_to_int (s[5]);
      c |= u;
      x |= u;

      if (unlikely (x > 0xFu)) goto malformed;
#else
      uint u = chr_xdig_to_int (s[2]);
      if (unlikely (u > 0xFu)) goto malformed;
      c = u << 12;

      u = chr_xdig_to_int (s[3]);
      if (unlikely (u > 0xFu)) goto malformed;
      c |= u << 8;

      u = chr_xdig_to_int (s[4]);
      if (unlikely (u > 0xFu)) goto malformed;
      c |= u << 4;

      u = chr_xdig_to_int (s[5]);
      if (unlikely (u > 0xFu)) goto malformed;
      c |= u;
#endif

      if (likely (utf16_chr_is8_lead1 (c)))
      {
#if JSON(UNESCAPE_SIZE)
        size++;
#else
  #if !JSON(UNESCAPE_INPLACE)
        if (unlikely (o == b))
        {
          need = 1;
          goto needspace;
        }
  #endif

        *o++ = c;
#endif
        s += 6;
      }
      else if (likely (utf16_chr_is8_lead2 (c)))
      {
#if JSON(UNESCAPE_SIZE)
        size += 2u;
#else
  #if !JSON(UNESCAPE_INPLACE)
        if (unlikely ((size_t)(b - o) < 2u))
        {
          need = 2 - (b - o);
          goto needspace;
        }
  #endif

        o[0] = utf8_codep_decomp_head2 (c);
        o[1] = utf8_codep_decomp_tail1 (c);

        o += 2;
#endif
        s += 6;
      }
      else if (likely (!utf16_chr_is_surr (c)))
      {
#if JSON(UNESCAPE_SIZE)
        size += 3u;
#else
  #if !JSON(UNESCAPE_INPLACE)
        if (unlikely ((size_t)(b - o) < 3u))
        {
          need = 3 - (b - o);
          goto needspace;
        }
  #endif

        o[0] = utf8_codep_decomp_head3 (c);
        o[1] = utf8_codep_decomp_tail2 (c);
        o[2] = utf8_codep_decomp_tail1 (c);

        o += 3;
#endif
        s += 6;
      }
      else if (unlikely (utf16_chr_is_surr_high (c)))
      {
        if (json_broken (12u))
        {
          expect = 12 - (e - s);
          goto tooshort;
        }

        uint sc;

#if JSON(UNESCAPE_EXPLICIT)
        if (unlikely (!buf_equ2 (s + 6, '\\', 'u'))) goto malformed;

        u = chr_xdig_to_int (s[8]);
        sc = u << 12;
        x = u;

        u = chr_xdig_to_int (s[9]);
        sc |= u << 8;
        x |= u;

        u = chr_xdig_to_int (s[10]);
        sc |= u << 4;
        x |= u;

        u = chr_xdig_to_int (s[11]);
        sc |= u;
        x |= u;

        if (unlikely (x > 0xFu)) goto malformed;
#else
        if (unlikely ((s[6] != '\\') || (s[7] != 'u'))) goto malformed;

        u = chr_xdig_to_int (s[8]);
        if (unlikely (u > 0xFu)) goto malformed;
        sc = u << 12;

        u = chr_xdig_to_int (s[9]);
        if (unlikely (u > 0xFu)) goto malformed;
        sc |= u << 8;

        u = chr_xdig_to_int (s[10]);
        if (unlikely (u > 0xFu)) goto malformed;
        sc |= u << 4;

        u = chr_xdig_to_int (s[11]);
        if (unlikely (u > 0xFu)) goto malformed;
        sc |= u;
#endif

        if (unlikely (!utf16_chr_is_surr_low (sc))) goto malformed;

        uf32 a = utf16_surr_to32 (c, sc);

#if JSON(UNESCAPE_SIZE)
        size += 4u;
#else
  #if !JSON(UNESCAPE_INPLACE)
        if (unlikely ((size_t)(b - o) < 4u))
        {
          need = 4 - (b - o);
          goto needspace;
        }
  #endif

        o[0] = utf8_codep_decomp_head4 (a);
        o[1] = utf8_codep_decomp_tail3 (a);
        o[2] = utf8_codep_decomp_tail2 (a);
        o[3] = utf8_codep_decomp_tail1 (a);

        o += 4;
#endif
        s += 12;
      }
      else goto malformed;
    }
    else
    {
      c = json_chr_unescape (c);

      if (unlikely (c > 0x7Fu)) goto malformed;

#if JSON(UNESCAPE_SIZE)
      size++;
#else
  #if !JSON(UNESCAPE_INPLACE)
      if (unlikely (o == b))
      {
        need = 1;
        goto needspace;
      }
  #endif

      *o++ = c;
#endif
      s += 2;
    }
  }
  else
  {
    if (unlikely (!chr_is_print (c)))
    {
#if JSON(UNESCAPE_EXPLICIT)
      if (c == '\0') break;
#endif

malformed:
      *end = (u8*)s;
#if JSON(UNESCAPE_SIZE)
      *num = size;
#else
  #if !JSON(UNESCAPE_INPLACE)
      *num = (size_t)(o - dst);
  #else
      *num = (size_t)(o - src);
  #endif
#endif

      return INT_MIN;
    }

    #include "unescape/simd.c"

#if JSON(UNESCAPE_SIZE)
    size++;
#else
  #if !JSON(UNESCAPE_INPLACE)
    if (unlikely (o == b))
    {
      need = 1;

needspace:
      *end = (u8*)s;
      *num = (size_t)(o - dst);

      return need;
    }
  #endif

    *o++ = c;
#endif
    s++;
  }

  repeat (unescape);
}

// -----------------------------------------------------------------------------

#undef JSON_UNESCAPE_SIZE
#undef JSON_UNESCAPE_EXPLICIT
#undef JSON_UNESCAPE_INPLACE

#undef json_broken
#undef json_peek
#undef json_over
