// =============================================================================
// <wspace/sse42.c>
//
// Skip through JSON whitespace faster using SSE4.2.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  // With SSE4.2 we can validate whitespace for free
  const xi128 w = _mm_setr_epi8
  (
    ' ', '\t', '\n', '\r', 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
  );

  // SSE4.2 code uses the `pcmpestri` and `pcmpistri` insns
  // for explicit and implicit (null-terminated) strings.
  // Otherwise, it is identical to the SSS 2 code.
  register u32 n = 0xFFFFu << ((uintptr_t)j & 15u);
  j = assume_aligned (16u, ptr_align_floor (16u, j));

  // The first chunk of input is tested using the `pcmpestri`
  // (explicit strings insn). We cannot use `pcmpistri` for it.
  register u32 m = _mm_cvtsi128_si32 (_mm_cmpestrm (w, 4
  , _mm_load_si128 ((const xi128*)j), 16, _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY
  | _SIDD_NEGATIVE_POLARITY | _SIDD_BIT_MASK)) & n;

  if (likely (m != 0))
  {
    j += bsf32 (m);

    if (json_over (j, 0))
    {
      j = e;
      json_more();
    }

    c = *j;

    break;
  }

  while (true)
  {
    j += 16;

    if (json_over (j, 0))
    {
      j = e;
      json_more();
    }

    // Subsequent chunks are tested using the `pcmpistri` insn,
    // which happens to perform faster than `pcmpestri`
    m = _mm_cmpistri (w, _mm_load_si128 ((const xi128*)j), _SIDD_UBYTE_OPS
    | _SIDD_CMP_EQUAL_ANY | _SIDD_NEGATIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT);

    if (likely (m != 16u))
    {
      j += m;

      if (json_over (j, 0))
      {
        j = e;
        json_more();
      }

      c = *j;

      break;
    }
  }

  break;
}
