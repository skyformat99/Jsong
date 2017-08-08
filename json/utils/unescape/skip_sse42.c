// =============================================================================
// <utils/unescape/skip_sse42.c>
//
// Unescape JSON string faster using SSE4.2.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

const xi128 st = _mm_setr_epi8
(
  ' ', '[', ']', 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
);

if (true)
{
#if JSON(UNESCAPE_INPLACE)
  register u32 n = (uintptr_t)s & 15u;
  const u8* sa = assume_aligned (16u, ptr_align_floor (16u, s));

  register u32 m = _mm_cvtsi128_si32 (_mm_cmpestrm (st, 4, _mm_load_si128 ((const xi128*)sa), 16
  , _SIDD_UBYTE_OPS | _SIDD_CMP_RANGES | _SIDD_NEGATIVE_POLARITY | _SIDD_BIT_MASK)) >> n;

  if (likely (m != 0))
  {
    m = bsf32 (m);

    if (json_over (s, m))
    {
skip_sse_end:
  #if JSON(UNESCAPE_SIZE)
      size += (size_t)(e - s);
  #else
      o = (u8*)e;
  #endif

      s = (u8*)e;

      break;
    }

skip_sse_done:
    s += m;

  #if JSON(UNESCAPE_SIZE)
    size += m;
  #else
    o = s;
  #endif

    repeat (unescape);
  }

  sa += 16;

  if (json_over (sa, 0)) goto skip_sse_end;

  #if JSON(UNESCAPE_SIZE)
    size += 16u - n;
  #endif

  s = assume_aligned (16u, sa);

  while (true)
  {
    xi128 si = _mm_load_si128 ((const xi128*)s);

    m = _mm_cmpistri (st, si, _SIDD_UBYTE_OPS | _SIDD_CMP_RANGES
    | _SIDD_NEGATIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT);

    if (likely (m != 16u))
    {
      if (json_over (s, m)) goto skip_sse_end;
      goto skip_sse_done;
    }

    if (json_over (s, 16u)) goto skip_sse_end;

  #if JSON(UNESCAPE_SIZE)
    size += 16u;
  #endif

    s += 16;
  }
#endif
}
