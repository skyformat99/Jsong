// =============================================================================
// <utils/uncomment/string/sse2.c>
//
// Skip through JSON comments faster using SSE2.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  const xi128 cq = _mm_set1_epi8 ('"');
  const xi128 cs = _mm_set1_epi8 ('/');

#if !JSON(UNCOMMENT_EXPLICIT)
  const xi128 cz = _mm_setzero_si128();
#endif

  register u32 n = (uintptr_t)j & 15u;
  j = assume_aligned (16u, ptr_align_floor (16u, j));

  while (true)
  {
    xi128 cj = _mm_load_si128 ((const xi128*)j);

    xi128 cm = _mm_or_si128 (_mm_cmpeq_epi8 (cj, cq)
    , _mm_cmpeq_epi8 (cj, cs));

#if !JSON(UNCOMMENT_EXPLICIT)
    cm = _mm_or_si128 (cm, _mm_cmpeq_epi8 (cj, cz));
#endif

    register u32 m = _mm_movemask_epi8 (cm) >> n;

    if (likely (m != 0))
    {
      j += n + bsf32 (m);

      if (json_over (j, 0)) goto end;

      break;
    }

    n = 0;
    j += 16;

    if (json_over (j, 0)) goto end;
  }
}
