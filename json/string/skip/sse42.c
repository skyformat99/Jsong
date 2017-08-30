// =============================================================================
// <string/skip/sse42.c>
//
// Skip JSON string faster using SSE4.2.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

const xi128 xt = _mm_setr_epi8
(
#if ENABLED(USON)
  ' ', '!', '#', '[', ']', 126, 128, 255, 0, 0, 0, 0, 0, 0, 0, 0
#else
  ' ', '!', '#', '[', ']', 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
#endif
);

if (true)
{
  register u32 n = (uintptr_t)j & 15u;
  j = assume_aligned (16u, ptr_align_floor (16u, j));

  register u32 b = _mm_cvtsi128_si32 (_mm_cmpestrm (xt, 6, _mm_load_si128 ((const xi128*)j), 16
  , _SIDD_UBYTE_OPS | _SIDD_CMP_RANGES | _SIDD_NEGATIVE_POLARITY | _SIDD_BIT_MASK));

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
    xi128 xj = _mm_load_si128 ((const xi128*)j);

    register u32 b = _mm_cmpistri (xt, xj, _SIDD_UBYTE_OPS | _SIDD_CMP_RANGES
    | _SIDD_NEGATIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT);

    if (likely (b != 16u))
    {
      j += b;

      if (json_over (j, 0)) goto string_skip_sse_end;

      goto string_skip_sse_done;
    }

    j += 16;

    if (json_over (j, 0)) goto string_skip_sse_end;
  }
}
