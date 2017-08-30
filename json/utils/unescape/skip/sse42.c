// =============================================================================
// <utils/unescape/skip/sse42.c>
//
// Skip JSON string faster using SSE4.2.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

const xi128 xt = _mm_setr_epi8
(
#if ENABLED(USON)
  ' ', '[', ']', 126, 128, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
#else
  ' ', '[', ']', 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
#endif
);

if (true)
{
  register u32 n = (uintptr_t)j & 15u;
  j = assume_aligned (16u, ptr_align_floor (16u, j));

  register u32 b = _mm_cvtsi128_si32 (_mm_cmpestrm (xt, 4, _mm_load_si128 ((const xi128*)j), 16
  , _SIDD_UBYTE_OPS | _SIDD_CMP_RANGES | _SIDD_NEGATIVE_POLARITY | _SIDD_BIT_MASK));

  b = (b >> n) << n;

  if (likely (b != 0))
  {
    b = bsf32 (b);

    if (json_over (j, b))
    {
skip_sse_end:
#if JSON(SIZE)
      size += (size_t)(e - j);
#else
      o = (u8*)e;
#endif

      j = (u8*)e;

      break;
    }

skip_sse_done:
    j += b;

#if JSON(SIZE)
    size += b;
#else
    o = (u8*)j;
#endif

    c = *j;

    repeat (unescape);
  }

  if (json_over (j, 16u)) goto skip_sse_end;

#if JSON(SIZE)
  size += 16u - n;
#endif

  j += 16;

  while (true)
  {
    xi128 si = _mm_load_si128 ((const xi128*)j);

    b = _mm_cmpistri (xt, si, _SIDD_UBYTE_OPS | _SIDD_CMP_RANGES
    | _SIDD_NEGATIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT);

    if (likely (b != 16u))
    {
      if (json_over (j, b)) goto skip_sse_end;
      goto skip_sse_done;
    }

    if (json_over (j, 16u)) goto skip_sse_end;

#if JSON(SIZE)
    size += 16u;
#endif

    j += 16;
  }
}
