// =============================================================================
// <utils/escape/template.c>
//
// JSON string escaping template.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  int need;

  uint k = json_prefix (chr_escape) (c);

  if (unlikely (k == 1u))
  {
#if JSON(SIZE)
    size += 2u;
#else
    if (unlikely ((size_t)(m - o) < 2u))
    {
      need = 2 - (m - o);

needspace:
      *end = (u8*)j;
      *num = (size_t)(o - out);

      return need;
    }

    o[0] = '\\';
    o[1] = c;

    o += 2;
#endif
  }
  else if (unlikely (k == 2u))
  {
#if JSON(SIZE)
    size += 2u;
#else
    if (unlikely ((size_t)(m - o) < 2u))
    {
      need = 2 - (m - o);
      goto needspace;
    }

    o[0] = '\\';
  #if HAVE(INT64)
    #if ENABLED(USON)
      o[1] = 0x00007200006E7400u >> ((c - 8u) * 8u);
    #else
      o[1] = 0x00007266006E7462u >> ((c - 8u) * 8u);
    #endif
  #else
    #if ENABLED(USON)
      o[1] = "\0\0\0\0\0\0\0\0\0tn\0\0r"[c];
    #else
      o[1] = "\0\0\0\0\0\0\0\0btn\0fr"[c];
    #endif
  #endif

    o += 2;
#endif
  }
  else if (unlikely (k == 3u))
  {
#if JSON(SIZE)
    size += 6u;
#else
    if (unlikely ((size_t)(m - o) < 6u))
    {
      need = 6 - (m - o);
      goto needspace;
    }

  #if ENABLED(USON)
    o[0] = '\\';
    o[1] = 'x';
    int_to_xdig2 (o + 2, c, false);

    o += 4;
  #else
    o[0] = '\\';
    o[1] = 'u';
    o[2] = '0';
    o[3] = '0';
    int_to_xdig2 (o + 4, c, false);

    o += 6;
  #endif
#endif
  }
  else
  {
#if JSON(SIZE)
    size++;
#else
    if (unlikely (o == m))
    {
      need = 1u;
      goto needspace;
    }

    *o++ = c;
#endif
  }

  j++;
}

// -----------------------------------------------------------------------------

#undef JSON_SIZE
