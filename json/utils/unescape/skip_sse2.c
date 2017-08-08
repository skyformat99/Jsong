// =============================================================================
// <utils/unescape/skip_sse2.c>
//
// Unescape JSON string faster using SSE2.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#if CPU(64BIT)
  const xi128 snpr = _mm_set1_epi8 (31);
  const xi128 sesc = _mm_set1_epi8 ('\\');
#else
  static const u8 svec[2][16] aligned(16) =
  {
    {REPEAT16 (31)}, {REPEAT16 ('\\')}
  };

  const xi128 snpr = _mm_load_si128 ((const xi128*)(svec[0]));
  const xi128 sesc = _mm_load_si128 ((const xi128*)(svec[1]));
#endif

if (true)
{
#if JSON(UNESCAPE_INPLACE)
  register u32 n = (uintptr_t)s & 15u;
  const u8* sa = assume_aligned (16u, ptr_align_floor (16u, s));

  xi128 si = _mm_load_si128 ((const xi128*)sa);

  xi128 sm = _mm_or_si128 (_mm_cmpeq_epi8 (_mm_max_epu8 (si, snpr), snpr)
  , _mm_cmpeq_epi8 (si, sesc));

  register u32 m = _mm_movemask_epi8 (sm) >> n;

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
    si = _mm_load_si128 ((const xi128*)s);

    sm = _mm_or_si128 (_mm_cmpeq_epi8 (_mm_max_epu8 (si, snpr), snpr)
    , _mm_cmpeq_epi8 (si, sesc));

    m = _mm_movemask_epi8 (sm);

    if (likely (m != 0))
    {
      m = bsf32 (m);

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
