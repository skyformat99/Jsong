// =============================================================================
// <verbatim/sse2.c>
//
// Skip through USON verbatim string faster using SSE2.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  const xi128 vt = _mm_set1_epi8 ('\t');
  const xi128 vr = _mm_set1_epi8 ('\r');
  const xi128 vs = _mm_set1_epi8 (' ');
  const xi128 vd = _mm_set1_epi8 (0x7F);

#if !JSON(EXPLICIT)
  const xi128 vz = _mm_setzero_si128();
#endif

  // Misalignment compensation to mask out
  // the garbage characters read in the beginning
  register u32 n = (0xFFFFu << ((uintptr_t)j & 15u)) & 0xFFFFu;

  // Round down to the first 16-byte aligned address
  j = assume_aligned (16u, ptr_align_floor (16u, j));

  // Load the input vector
  xi128 vi = _mm_load_si128 ((const xi128*)j);

  // Test for invalid characters
  xi128 vv = _mm_or_si128 (_mm_cmpeq_epi8 (vi, vt), _mm_cmpeq_epi8 (vi, vr));
  vv = _mm_or_si128 (vv, _mm_cmpeq_epi8 (_mm_max_epu8 (vi, vs), vs));
  vv = _mm_xor_si128 (vv, _mm_cmpeq_epi8 (vi, vd));

#if !JSON(EXPLICIT)
  // Catch the null-terminating character
  vv = _mm_xor_si128 (vv, _mm_cmpeq_epi8 (vi, vz));
#endif

  // Get the result mask
  register u32 m = ~_mm_movemask_epi8 (vv) & n;

  // Some of the tested characters are not valid
  if (likely (m != 0))
  {
    // Move to the first offending character
    j += bsf32 (m);

    // Check if the new position overflows the input buffer
    if (json_over (j, 0))
    {
verb_sse_end:
      j = e;

      goto verb_end;
    }

verb_sse_done:
    // Get the offending character
    c = *j;

    goto verb_done;
  }

  // The rest of input is now aligned
  j += 16;

  if (json_over (j, 0)) goto verb_sse_end;

  while (true)
  {
    vi = _mm_load_si128 ((const xi128*)j);

    vv = _mm_or_si128 (_mm_cmpeq_epi8 (vi, vt), _mm_cmpeq_epi8 (vi, vr));
    vv = _mm_or_si128 (vv, _mm_cmpeq_epi8 (_mm_max_epu8 (vi, vs), vs));
    vv = _mm_xor_si128 (vv, _mm_cmpeq_epi8 (vi, vd));

#if !JSON(EXPLICIT)
    vv = _mm_xor_si128 (vv, _mm_cmpeq_epi8 (vi, vz));
#endif

    register u32 m = ~_mm_movemask_epi8 (vv);

    if (likely (m != 0))
    {
      j += bsf32 (m);

      if (json_over (j, 0)) goto verb_sse_end;

      goto verb_sse_done;
    }

    j += 16;

    if (json_over (j, 0)) goto verb_sse_end;
  }
}
