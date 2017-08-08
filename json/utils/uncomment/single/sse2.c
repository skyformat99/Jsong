// =============================================================================
// <utils/uncomment/single/sse2.c>
//
// Skip through JSON comments faster using SSE2.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  const xi128 cn = _mm_set1_epi8 ('\n');
  const xi128 cr = _mm_set1_epi8 ('\r');
  const xi128 cs = _mm_set1_epi8 (' ');

#if !JSON(UNCOMMENT_EXPLICIT)
  const xi128 cz = _mm_setzero_si128();
#endif

  register u32 n = (uintptr_t)j & 15u;
  const u8* p = assume_aligned (16u, ptr_align_floor (16u, j));

  xi128 cj = _mm_load_si128 ((const xi128*)p);

  xi128 cm = _mm_or_si128 (_mm_cmpeq_epi8 (cj, cn)
  , _mm_cmpeq_epi8 (cj, cr));

#if !JSON(UNCOMMENT_EXPLICIT)
  cm = _mm_or_si128 (cm, _mm_cmpeq_epi8 (cj, cz));
#endif

  register u32 m = _mm_movemask_epi8 (cm) >> n;

  if (likely (m != 0))
  {
    m = bsf32 (m);

    if (json_over (j, m))
    {
single_sse_end:
      m = e - j;
      sse_buf_fill (j, ' ', m);

      goto end;
    }

single_sse_done:
    sse_buf_fill (j, ' ', m);

    j += m;
  }
  else
  {
    p += 16;

    if (json_over (p, 0)) goto single_sse_end;

    sse_buf_fill (j, ' ', 16u - n);
    j = assume_aligned (16u, p);

    while (true)
    {
      cj = _mm_load_si128 ((const xi128*)j);

      cm = _mm_or_si128 (_mm_cmpeq_epi8 (cj, cn)
      , _mm_cmpeq_epi8 (cj, cr));

#if !JSON(UNCOMMENT_EXPLICIT)
      cm = _mm_or_si128 (cm, _mm_cmpeq_epi8 (cj, cz));
#endif

      m = _mm_movemask_epi8 (cm);

      if (likely (m != 0))
      {
        m = bsf32 (m);

        if (json_over (j, m)) goto single_sse_end;

        goto single_sse_done;
      }

      if (json_over (j, 16u)) goto single_sse_end;

      _mm_storeu_si128 ((xi128*)j, cs);

      j += 16;
    }
  }
}
