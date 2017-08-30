// =============================================================================
// <data/sse_outro.c>
//
// Store partially decoded USON Base64 chunk.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#if CPU(SSE42)
  if (unlikely (b != 16u))
#else
  if (unlikely (m != 0))
#endif
{
#if CPU(SSE42)
  register uint n = b >> 2;
#else
  register uint n = (__builtin_ffs (m) - 1) >> 2;
#endif

  xi128_t v = {.x = dr};

#if JSON(SIZE)
  size += (n << 1) + n;
#else
  #if !JSON(INPLACE)
    if ((size_t)(m - o) < ((n << 1) + n))
    {
      need = ((n << 1) + n) - (size_t)(m - o);
      goto needspace;
    }
  #endif

  sse_buf_copy (o, v.ui8, (n << 1) + n);

  o += (n << 1) + n;
#endif

  j += n << 2;

  goto data_skip;
}

#if JSON(SIZE)
  size += 12;
#else
  #if !JSON(INPLACE)
    if ((size_t)(m - o) < 12u)
    {
      need = 12u - (size_t)(m - o);
      goto needspace;
    }
  #endif

  _mm_storeu_si128 ((xi128*)o, dr);

  o += 12;
#endif

j += 16;
