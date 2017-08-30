// =============================================================================
// <verbatim/sse42.c>
//
// Skip through USON verbatim string faster using SSE4.2.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  const xi128 x = _mm_setr_epi8
  (
    '\t', '\t', '\r', '\r', ' ', 126, 128, 255, 0, 0, 0, 0, 0, 0, 0, 0
  );

  register u32 n = (uintptr_t)j & 15u;
  j = assume_aligned (16u, ptr_align_floor (16u, j));

  register u32 m = _mm_cvtsi128_si32 (_mm_cmpestrm (x, 6, _mm_load_si128 ((const xi128*)j), 16
  , _SIDD_UBYTE_OPS | _SIDD_CMP_RANGES | _SIDD_NEGATIVE_POLARITY | _SIDD_BIT_MASK));

  m = (m >> n) << n;

  if (likely (m != 0))
  {
    j += bsf32 (m);

    if (json_over (j, 0))
    {
verb_sse_end:
      j = e;

      goto verb_end;
    }

verb_sse_done:
    c = *j;

    goto verb_scalar;
  }

  j += 16;

  if (json_over (j, 0)) goto verb_sse_end;

  while (true)
  {
    register u32 b = _mm_cmpistri (x, _mm_load_si128 ((const xi128*)j)
    , _SIDD_UBYTE_OPS | _SIDD_CMP_RANGES | _SIDD_NEGATIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT);

    if (likely (b != 16u))
    {
      j += b;

      if (json_over (j, 0)) goto verb_sse_end;

      goto verb_sse_done;
    }

    j += 16;

    if (json_over (j, 0)) goto verb_sse_end;
  }
}
