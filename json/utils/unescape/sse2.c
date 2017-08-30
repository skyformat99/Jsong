// =============================================================================
// <utils/unescape/sse2.c>
//
// Unescape JSON string faster using SSE2.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  register u32 n = (uintptr_t)j & 15u;
  const u8* ja = assume_aligned (16u, ptr_align_floor (16u, j));

  xi128 xj = _mm_load_si128 ((const xi128*)ja);

  xi128 xm = _mm_or_si128 (_mm_cmpeq_epi8 (_mm_max_epu8 (xj, xnpr), xnpr)
  , _mm_cmpeq_epi8 (xj, xesc));

#if ENABLED(USON)
  xm = _mm_or_si128 (xm, _mm_cmpeq_epi8 (xj, xdel));
#endif

  register u32 b = _mm_movemask_epi8 (xm) >> n;

  if (likely (b != 0))
  {
    b = bsf32 (b);

    if (json_over (j, b))
    {
sse_end:
      b = (size_t)(e - j);

#if JSON(SIZE)
      size += b;
#else
  #if !JSON(INPLACE)
      if (unlikely ((size_t)(m - o) < b))
      {
        need = b - (m - o);
        goto needspace;
      }
  #endif

      sse_buf_move_left (o, j, b);
      o += b;
#endif

#if JSON(INPLACE)
      j = (u8*)e;
#else
      j = e;
#endif

      break;
    }

sse_done:
#if JSON(SIZE)
    size += b;
#else
  #if !JSON(INPLACE)
    if (unlikely ((size_t)(m - o) < b))
    {
      need = b - (m - o);
      goto needspace;
    }
  #endif

    sse_buf_move_left (o, j, b);
    o += b;
#endif

    j += b;
    c = *j;

    repeat (unescape);
  }

  ja += 16;

  if (json_over (ja, 0)) goto sse_end;

#if JSON(SIZE)
  size += 16u - n;
#else
  b = 16u - n;

  #if !JSON(INPLACE)
    if (unlikely ((size_t)(m - o) < b))
    {
      need = b - (m - o);
      goto needspace;
    }
  #endif

  sse_buf_move_left (o, j, b);
  o += b;
#endif

  j = assume_aligned (16u, ja);

  while (true)
  {
    xj = _mm_load_si128 ((const xi128*)j);

    xm = _mm_or_si128 (_mm_cmpeq_epi8 (_mm_max_epu8 (xj, xnpr), xnpr)
    , _mm_cmpeq_epi8 (xj, xesc));

#if ENABLED(USON)
    xm = _mm_or_si128 (xm, _mm_cmpeq_epi8 (xj, xdel));
#endif

    b = _mm_movemask_epi8 (xm);

    if (likely (b != 0))
    {
      b = bsf32 (b);

      if (json_over (j, b)) goto sse_end;

      goto sse_done;
    }

    if (json_over (j, 16u)) goto sse_end;

#if JSON(SIZE)
    size += 16u;
#else
  #if !JSON(INPLACE)
    if (unlikely ((size_t)(m - o) < 16u))
    {
      need = 16 - (m - o);
      goto needspace;
    }
  #endif

    _mm_storeu_si128 ((xi128*)o, xj);
    o += 16;
#endif

    j += 16;
  }
}
