// =============================================================================
// <utils/unescape/sse2.c>
//
// Unescape JSON string faster using SSE2.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  register u32 n = (uintptr_t)s & 15u;
  const u8* sa = assume_aligned (16u, ptr_align_floor (16u, s));

  xi128 si = _mm_load_si128 ((const xi128*)sa);

  xi128 sm = _mm_or_si128 (_mm_cmpeq_epi8 (_mm_max_epu8 (si, snpr), snpr)
  , _mm_cmpeq_epi8 (si, sesc));

  register u32 m = _mm_movemask_epi8 (sm) >> n;

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
    si = _mm_load_si128 ((const xi128*)s);

    sm = _mm_or_si128 (_mm_cmpeq_epi8 (_mm_max_epu8 (si, snpr), snpr)
    , _mm_cmpeq_epi8 (si, sesc));

    m = _mm_movemask_epi8 (sm);

    if (likely (m != 0))
    {
      m = bsf32 (m);

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
