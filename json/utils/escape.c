// =============================================================================
// <utils/escape.c>
//
// JSON string escaping template.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  int need;

  uf32 e = json_chr_escape (c);

  if (likely (e == 1u))
  {
#if JSON(ESCAPE_SIZE)
    size += 2u;
#else
    if (unlikely ((size_t)(m - d) < 2u))
    {
      need = 2 - (m - d);

needspace:
      *end = (u8*)s;
      *num = (size_t)(d - dst);

      return need;
    }

    d[0] = '\\';
    d[1] = c;

    d += 2;
#endif
  }
  else if (unlikely (e == 2u))
  {
#if JSON(ESCAPE_SIZE)
    size += 2u;
#else
    if (unlikely ((size_t)(m - d) < 2u))
    {
      need = 2 - (m - d);
      goto needspace;
    }

    d[0] = '\\';
  #if HAVE(INT64)
    d[1] = 0x00007266006E7462u >> ((c - 8u) * 8u);
  #else
    d[1] = "\0\0\0\0\0\0\0\0btn\0fr"[c];
  #endif

    d += 2;
#endif
  }
  else if (unlikely (e == 3u))
  {
#if JSON(ESCAPE_SIZE)
    size += 6u;
#else
    if (unlikely ((size_t)(m - d) < 6u))
    {
      need = 6 - (m - d);
      goto needspace;
    }

    d[0] = '\\';
    d[1] = 'u';
    d[2] = '0';
    d[3] = '0';
    int_to_xdig2 (d + 4, c, false);

    d += 6;
#endif
  }
  else
  {
#if JSON(ESCAPE_SIZE)
    size++;
#else
    if (unlikely (m == d))
    {
      need = 1u;
      goto needspace;
    }

    *d++ = c;
#endif
  }

  s++;
}

// -----------------------------------------------------------------------------

#undef JSON_ESCAPE_SIZE
