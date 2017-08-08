// =============================================================================
// <string/skip_sse2.c>
//
// Skip through JSON string faster using SSE2.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

// Load the test vector constants
#if CPU(64BIT)
  const xi128 snpr = _mm_set1_epi8 (31);
  const xi128 sesc = _mm_set1_epi8 ('\\');
  const xi128 send = _mm_set1_epi8 ('"');
#else
  static const u8 svec[3][16] aligned(16) =
  {
    {REPEAT16 (31)}, {REPEAT16 ('\\')}, {REPEAT16 ('"')}
  };

  const xi128 snpr = _mm_load_si128 ((const xi128*)(svec[0]));
  const xi128 sesc = _mm_load_si128 ((const xi128*)(svec[1]));
  const xi128 send = _mm_load_si128 ((const xi128*)(svec[2]));
#endif

if (true)
{
  // Align the input pointer
  register u32 n = (uintptr_t)j & 15u;
  const u8* ja = assume_aligned (16u, ptr_align_floor (16u, j));

  // Load the input vector
  xi128 si = _mm_load_si128 ((const xi128*)ja);

  // Test the input vector for invalid string characters,
  // escape sequence start and string end
  xi128 sm = _mm_or_si128 (_mm_cmpeq_epi8 (_mm_max_epu8 (si, snpr), snpr)
  , _mm_or_si128 (_mm_cmpeq_epi8 (si, sesc), _mm_cmpeq_epi8 (si, send)));

  // Move the comparison mask into the general purpose register
  // and shift out the garbage bits (due to likely misaligned input)
  register u32 m = _mm_movemask_epi8 (sm) >> n;

  if (likely (m != 0))
  {
    m = bsf32 (m);

    if (json_over (j, m))
    {
string_skip_sse_end:
      j = e;
      o = (u8*)e;

      goto string_end;
    }

string_skip_sse_done:
    j += m;
    o = (u8*)j;

    c = *j;

    repeat (string);
  }

  ja += 16;

  if (json_over (ja, 0)) goto string_skip_sse_end;

  j = assume_aligned (16u, ja);

  while (true)
  {
    si = _mm_load_si128 ((const xi128*)j);

    sm = _mm_or_si128 (_mm_cmpeq_epi8 (_mm_max_epu8 (si, snpr), snpr)
    , _mm_or_si128 (_mm_cmpeq_epi8 (si, sesc), _mm_cmpeq_epi8 (si, send)));

    m = _mm_movemask_epi8 (sm);

    if (likely (m != 0))
    {
      m = bsf32 (m);

      if (json_over (j, m)) goto string_skip_sse_end;

      goto string_skip_sse_done;
    }

    if (json_over (j, 16u)) goto string_skip_sse_end;

    j += 16;
  }
}
