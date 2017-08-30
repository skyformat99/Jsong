// =============================================================================
// <utils/unescape/sse42.c>
//
// Unescape JSON string faster using SSE4.2.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  register u32 n = (uintptr_t)j & 15u;
  const u8* ja = assume_aligned (16u, ptr_align_floor (16u, j));

  register u32 b = _mm_cvtsi128_si32 (_mm_cmpestrm (xt, 4, _mm_load_si128 ((const xi128*)ja), 16
  , _SIDD_UBYTE_OPS | _SIDD_CMP_RANGES | _SIDD_NEGATIVE_POLARITY | _SIDD_BIT_MASK)) >> n;

  if (likely (b != 0))
  {
    b = bsf32 (b);

    if (json_over (j, b))
    {
sse_end:
      b = (size_t)(e - j);

#if JSON(SIZE)
      size += b;
#else
  #if !JSON(INPLACE)
      if (unlikely ((size_t)(m - o) < b))
      {
        need = b - (m - o);
        goto needspace;
      }
  #endif

      sse_buf_move_left (o, j, b);
      o += b;
#endif

#if JSON(INPLACE)
      j = (u8*)e;
#else
      j = e;
#endif

      break;
    }

sse_done:
#if JSON(SIZE)
    size += b;
#else
  #if !JSON(INPLACE)
    if (unlikely ((size_t)(m - o) < b))
    {
      need = b - (m - o);
      goto needspace;
    }
  #endif

    sse_buf_move_left (o, j, b);
    o += b;
#endif

    j += b;
    c = *j;

    repeat (unescape);
  }

  ja += 16;

  if (json_over (ja, 0)) goto sse_end;

#if JSON(SIZE)
  size += 16u - n;
#else
  b = 16u - n;

  #if !JSON(INPLACE)
    if (unlikely ((size_t)(m - o) < b))
    {
      need = b - (m - o);
      goto needspace;
    }
  #endif

  sse_buf_move_left (o, j, b);
  o += b;
#endif

  j = assume_aligned (16u, ja);

  while (true)
  {
    xi128 xj = _mm_load_si128 ((const xi128*)j);

    b = _mm_cmpistri (xt, xj, _SIDD_UBYTE_OPS | _SIDD_CMP_RANGES
    | _SIDD_NEGATIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT);

    if (likely (b != 16u))
    {
      if (json_over (j, b)) goto sse_end;
      goto sse_done;
    }

    if (json_over (j, 16u)) goto sse_end;

#if JSON(SIZE)
    size += 16u;
#else
  #if !JSON(INPLACE)
    if (unlikely ((size_t)(m - o) < 16u))
    {
      need = 16 - (m - o);
      goto needspace;
    }
  #endif

    _mm_storeu_si128 ((xi128*)o, xj);
    o += 16;
#endif

    j += 16;
  }
}
