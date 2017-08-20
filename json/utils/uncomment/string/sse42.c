// =============================================================================
// <utils/uncomment/string/sse42.c>
//
// Skip through JSON comments faster using SSE4.2.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  const xi128 t = _mm_setr_epi8
  (
    ' ', '!', '#', '.', '0', 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  );

  register u32 n = (uintptr_t)j & 15u;
  j = assume_aligned (16u, ptr_align_floor (16u, j));

  register u32 m = _mm_cvtsi128_si32 (_mm_cmpistrm (t, _mm_load_si128 ((const xi128*)j)
  , _SIDD_UBYTE_OPS | _SIDD_CMP_RANGES | _SIDD_NEGATIVE_POLARITY | _SIDD_BIT_MASK)) >> n;

  if (likely (m != 0))
  {
    j += n + bsf32 (m);

    if (json_over (j, 0)) goto end;
  }
  else while (true)
  {
    j += 16;

    if (json_over (j, 0)) goto end;

    m = _mm_cmpistri (t, _mm_load_si128 ((const xi128*)j), _SIDD_UBYTE_OPS
    | _SIDD_CMP_RANGES | _SIDD_NEGATIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT);

    if (likely (m != 16u))
    {
      j += m;

      if (json_over (j, 0)) goto end;

      break;
    }
  }
}
