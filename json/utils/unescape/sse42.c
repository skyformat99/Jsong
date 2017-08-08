// =============================================================================
// <utils/unescape/sse42.c>
//
// Unescape JSON string faster using SSE4.2.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  register u32 n = (uintptr_t)s & 15u;
  const u8* sa = assume_aligned (16u, ptr_align_floor (16u, s));

  register u32 m = _mm_cvtsi128_si32 (_mm_cmpestrm (st, 4, _mm_load_si128 ((const xi128*)sa), 16
  , _SIDD_UBYTE_OPS | _SIDD_CMP_RANGES | _SIDD_NEGATIVE_POLARITY | _SIDD_BIT_MASK)) >> n;

  if (likely (m != 0))
  {
    m = bsf32 (m);

    if (json_over (s, m))
    {
sse_end:
      m = (size_t)(e - s);

#if JSON(UNESCAPE_SIZE)
      size += m;
#else
  #if !JSON(UNESCAPE_INPLACE)
      if (unlikely ((size_t)(b - o) < m))
      {
        need = m - (b - o);
        goto needspace;
      }
  #endif

      sse_buf_move_left (o, s, m);
      o += m;
#endif

#if JSON(UNESCAPE_INPLACE)
      s = (u8*)e;
#else
      s = e;
#endif

      break;
    }

sse_done:
#if JSON(UNESCAPE_SIZE)
    size += m;
#else
  #if !JSON(UNESCAPE_INPLACE)
    if (unlikely ((size_t)(b - o) < m))
    {
      need = m - (b - o);
      goto needspace;
    }
  #endif

    sse_buf_move_left (o, s, m);
    o += m;
#endif

    s += m;

    repeat (unescape);
  }

  sa += 16;

  if (json_over (sa, 0)) goto sse_end;

#if JSON(UNESCAPE_SIZE)
  size += 16u - n;
#else
  m = 16u - n;

  #if !JSON(UNESCAPE_INPLACE)
    if (unlikely ((size_t)(b - o) < m))
    {
      need = m - (b - o);
      goto needspace;
    }
  #endif

  sse_buf_move_left (o, s, m);
  o += m;
#endif

  s = assume_aligned (16u, sa);

  while (true)
  {
    xi128 si = _mm_load_si128 ((const xi128*)s);

    m = _mm_cmpistri (st, si, _SIDD_UBYTE_OPS | _SIDD_CMP_RANGES
    | _SIDD_NEGATIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT);

    if (likely (m != 16u))
    {
      if (json_over (s, m)) goto sse_end;
      goto sse_done;
    }

    if (json_over (s, 16u)) goto sse_end;

#if JSON(UNESCAPE_SIZE)
    size += 16u;
#else
  #if !JSON(UNESCAPE_INPLACE)
    if (unlikely ((size_t)(b - o) < 16u))
    {
      need = 16 - (b - o);
      goto needspace;
    }
  #endif

    _mm_storeu_si128 ((xi128*)o, si);
    o += 16;
#endif

    s += 16;
  }
}
