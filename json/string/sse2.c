// =============================================================================
// <string/sse2.c>
//
// Skip through JSON string faster using SSE2.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  // Align the input pointer
  register u32 n = (uintptr_t)j & 15u;
  const u8* ja = assume_aligned (16u, ptr_align_floor (16u, j));

  // Load the input vector
  xi128 xj = _mm_load_si128 ((const xi128*)ja);

  // Test the input vector for invalid string characters,
  // escape sequence start and string end
  xi128 xm = _mm_or_si128 (_mm_cmpeq_epi8 (_mm_max_epu8 (xj, xnpr), xnpr)
  , _mm_or_si128 (_mm_cmpeq_epi8 (xj, xesc), _mm_cmpeq_epi8 (xj, xend)));

#if ENABLED(USON)
  xm = _mm_or_si128 (xm, _mm_cmpeq_epi8 (xj, xdel));
#endif

  // Move the comparison mask into the general purpose register
  // and shift out the garbage bits (due to likely misaligned input)
  register u32 b = _mm_movemask_epi8 (xm) >> n;

  if (likely (b != 0))
  {
    b = bsf32 (b);

    if (json_over (j, b))
    {
string_sse_end:
      b = e - j;
      sse_buf_move_left (o, j, b);
      o += b;
      j = e;

      goto string_end;
    }

string_sse_done:
    sse_buf_move_left (o, j, b);
    o += b;
    j += b;

    c = *j;

    repeat (string);
  }

  ja += 16;

  if (json_over (ja, 0)) goto string_sse_end;

  b = 16u - n;
  sse_buf_move_left (o, j, b);
  o += b;
  j = assume_aligned (16u, ja);

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
      b = bsf32 (b);

      if (json_over (j, b)) goto string_sse_end;

      goto string_sse_done;
    }

    if (json_over (j, 16u)) goto string_sse_end;

    _mm_storeu_si128 ((xi128*)o, xj);

    o += 16;
    j += 16;
  }
}
