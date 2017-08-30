// =============================================================================
// <utils/unescape/skip/sse2.c>
//
// Skip JSON string faster using SSE2.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

const xi128 xnpr = _mm_set1_epi8 (31);
const xi128 xesc = _mm_set1_epi8 ('\\');

#if ENABLED(USON)
  const xi128 xdel = _mm_set1_epi8 (127);
#endif

if (true)
{
  register u32 n = (uintptr_t)j & 15u;
  j = assume_aligned (16u, ptr_align_floor (16u, j));

  xi128 xj = _mm_load_si128 ((const xi128*)j);

  xi128 xm = _mm_or_si128 (_mm_cmpeq_epi8 (_mm_max_epu8 (xj, xnpr), xnpr)
  , _mm_cmpeq_epi8 (xj, xesc));

#if ENABLED(USON)
  xm = _mm_or_si128 (xm, _mm_cmpeq_epi8 (xj, xdel));
#endif

  register u32 b = _mm_movemask_epi8 (xm);
  b = (b >> n) << n;

  if (likely (b != 0))
  {
    b = bsf32 (b);

    if (json_over (j, b))
    {
skip_sse_end:
#if JSON(SIZE)
      size += (size_t)(e - j);
#else
      o = (u8*)e;
#endif

      j = (u8*)e;

      break;
    }

skip_sse_done:
    j += b;

#if JSON(SIZE)
    size += b;
#else
    o = (u8*)j;
#endif

    c = *j;

    repeat (unescape);
  }

  if (json_over (j, 16u)) goto skip_sse_end;

#if JSON(SIZE)
  size += 16u - n;
#endif

  j += 16;

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

      if (json_over (j, b)) goto skip_sse_end;

      goto skip_sse_done;
    }

    if (json_over (j, 16u)) goto skip_sse_end;

#if JSON(SIZE)
    size += 16u;
#endif

    j += 16;
  }
}
