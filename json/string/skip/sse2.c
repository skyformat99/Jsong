// =============================================================================
// <string/skip/sse2.c>
//
// Skip JSON string faster using SSE2.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

const xi128 xnpr = _mm_set1_epi8 (31);
const xi128 xesc = _mm_set1_epi8 ('\\');
const xi128 xend = _mm_set1_epi8 ('"');

#if ENABLED(USON)
  const xi128 xdel = _mm_set1_epi8 (127);
#endif

if (true)
{
  // Align the input pointer
  register u32 n = (uintptr_t)j & 15u;
  j = assume_aligned (16u, ptr_align_floor (16u, j));

  // Load the input vector
  xi128 xj = _mm_load_si128 ((const xi128*)j);

  // Test the input vector for invalid string characters,
  // escape sequence start and string end
  xi128 xm = _mm_or_si128 (_mm_cmpeq_epi8 (_mm_max_epu8 (xj, xnpr), xnpr)
  , _mm_or_si128 (_mm_cmpeq_epi8 (xj, xesc), _mm_cmpeq_epi8 (xj, xend)));

#if ENABLED(USON)
  xm = _mm_or_si128 (xm, _mm_cmpeq_epi8 (xj, xdel));
#endif

  // Move the comparison mask into the general purpose register
  // and shift out the garbage bits (due to likely misaligned input)
  register u32 b = _mm_movemask_epi8 (xm);
  b = (b >> n) << n;

  if (likely (b != 0))
  {
    j += bsf32 (b);

    if (json_over (j, 0))
    {
string_skip_sse_end:
      j = e;
      o = (u8*)e;

      goto string_end;
    }

string_skip_sse_done:
    o = (u8*)j;
    c = *j;

    repeat (string);
  }

  j += 16;

  if (json_over (j, 0)) goto string_skip_sse_end;

  while (true)
  {
    xj = _mm_load_si128 ((const xi128*)j);

    xm = _mm_or_si128 (_mm_cmpeq_epi8 (_mm_max_epu8 (xj, xnpr), xnpr)
    , _mm_or_si128 (_mm_cmpeq_epi8 (xj, xesc), _mm_cmpeq_epi8 (xj, xend)));

#if ENABLED(USON)
    xm = _mm_or_si128 (xm, _mm_cmpeq_epi8 (xj, xdel));
#endif

    b = _mm_movemask_epi8 (xm);

    if (likely (b != 0))
    {
      j += bsf32 (b);

      if (json_over (j, 0)) goto string_skip_sse_end;

      goto string_skip_sse_done;
    }

    j += 16;

    if (json_over (j, 0)) goto string_skip_sse_end;
  }
}
