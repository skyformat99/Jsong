// =============================================================================
// <utils/uncomment/single/sse42.c>
//
// Skip through JSON comments faster using SSE4.2.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  const xi128 ct = _mm_setr_epi8
  (
    0, 9, 11, 12, 14, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  );

  const xi128 cs = _mm_set1_epi8 (' ');

  register u32 n = (uintptr_t)j & 15u;
  const u8* p = assume_aligned (16u, ptr_align_floor (16u, j));

  register u32 m = _mm_cvtsi128_si32 (_mm_cmpistrm (ct, _mm_load_si128 ((const xi128*)p)
  , _SIDD_UBYTE_OPS | _SIDD_CMP_RANGES | _SIDD_NEGATIVE_POLARITY | _SIDD_BIT_MASK)) >> n;

  if (likely (m != 0))
  {
    m = bsf32 (m);

    if (json_over (j, m))
    {
single_sse_end:
      m = (size_t)(e - j);
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
      xi128 cj = _mm_load_si128 ((const xi128*)j);

      m = _mm_cmpistri (ct, cj, _SIDD_UBYTE_OPS | _SIDD_CMP_RANGES
      | _SIDD_NEGATIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT);

      if (likely (m != 16u))
      {
        if (json_over (j, m)) goto single_sse_end;
        goto single_sse_done;
      }

      if (json_over (j, 16u)) goto single_sse_end;

      _mm_storeu_si128 ((xi128*)j, cs);

      j += 16;
    }
  }
}
