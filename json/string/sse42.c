// =============================================================================
// <string/sse42.c>
//
// Skip through JSON string faster using SSE4.2.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  register u32 n = (uintptr_t)j & 15u;
  const u8* ja = assume_aligned (16u, ptr_align_floor (16u, j));

  register u32 m = _mm_cvtsi128_si32 (_mm_cmpestrm (st, 6, _mm_load_si128 ((const xi128*)ja), 16
  , _SIDD_UBYTE_OPS | _SIDD_CMP_RANGES | _SIDD_NEGATIVE_POLARITY | _SIDD_BIT_MASK)) >> n;

  if (likely (m != 0))
  {
    m = bsf32 (m);

    if (json_over (j, m))
    {
string_sse_end:
      m = e - j;
      sse_buf_move_left (o, j, m);
      o += m;

      j = e;

      goto string_end;
    }

string_sse_done:
    sse_buf_move_left (o, j, m);
    o += m;
    j += m;

    c = *j;

    repeat (string);
  }

  ja += 16;

  if (json_over (ja, 0)) goto string_sse_end;

  m = 16u - n;
  sse_buf_move_left (o, j, m);
  o += m;

  j = assume_aligned (16u, ja);

  while (true)
  {
    xi128 si = _mm_load_si128 ((const xi128*)j);

    m = _mm_cmpistri (st, si, _SIDD_UBYTE_OPS | _SIDD_CMP_RANGES
    | _SIDD_NEGATIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT);

    if (likely (m != 16u))
    {
      if (json_over (j, m)) goto string_sse_end;
      goto string_sse_done;
    }

    if (json_over (j, 16u)) goto string_sse_end;

    _mm_storeu_si128 ((xi128*)o, si);

    o += 16;
    j += 16;
  }
}
