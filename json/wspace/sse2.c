// =============================================================================
// <wspace/sse2.c>
//
// Skip through JSON whitespace faster using SSE2.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
#if JSON(VALID_WHITESPACE)
  const xi128 ws = _mm_set1_epi8 (' ');
  const xi128 wt = _mm_set1_epi8 ('\t');
  const xi128 wn = _mm_set1_epi8 ('\n');
  const xi128 wr = _mm_set1_epi8 ('\r');
#else
  const xi128 ws = _mm_set1_epi8 (' ');

  #if !JSON(EXPLICIT)
    const xi128 wz = _mm_setzero_si128();
  #endif
#endif

  // Misalignment compensation to mask out
  // the garbage characters read in the beginning
  register u32 n = (0xFFFFu << ((uintptr_t)j & 15u)) & 0xFFFFu;

  // Round down to the first 16-byte aligned address
  j = assume_aligned (16u, ptr_align_floor (16u, j));

  while (true)
  {
    // Load the input vector
    xi128 wi = _mm_load_si128 ((const xi128*)j);

#if JSON(VALID_WHITESPACE)
    // Test for all valid whitespace characters
    xi128 wm = _mm_or_si128 (_mm_or_si128 (_mm_cmpeq_epi8 (wi, ws), _mm_cmpeq_epi8 (wi, wt))
    , _mm_or_si128 (_mm_cmpeq_epi8 (wi, wn), _mm_cmpeq_epi8 (wi, wr)));
#else
    // Test for any whitespace character
    xi128 wm = _mm_cmpeq_epi8 (_mm_max_epu8 (wi, ws), ws);

  #if !JSON(EXPLICIT)
    // Catch the null-terminating character to terminate the processing
    wm = _mm_xor_si128 (wm, _mm_cmpeq_epi8 (wi, wz));
  #endif
#endif

    // Get the result mask
    register u32 m = ~_mm_movemask_epi8 (wm) & n;

    // Subsequent tests don't need to be compensated,
    // because they are going to be aligned until the end
    n = 0xFFFFu;

    // Some of the tested characters are not whitespace
    if (likely (m != 0))
    {
      // Move to the first detected character
      j += bsf32 (m);

      // Check if the new position overflows the input buffer
      if (json_over (j, 0))
      {
        j = e;
        json_more();
      }

      // Get the character
      c = *j;

      break;
    }

    // Test the next 16 bytes of the input
    j += 16;

    if (json_over (j, 0))
    {
      j = e;
      json_more();
    }
  }

  break;
}
