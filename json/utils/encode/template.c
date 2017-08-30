// =============================================================================
// <utils/encode/template.c>
//
// USON Base64 string encoding template.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  int need;

#if JSON(EXPLICIT)
  size_t n = size / 3u;

  while (n--)
#else
  while (true)
#endif
  {
#if !JSON(EXPLICIT)
    if ((j[0] == '\0') || (j[1] == '\0') || (j[2] == '\0')) break;
#endif

#if JSON(SIZE)
    size += 4;
#else
    if ((size_t)(m - o) < 4u)
    {
needspace:
      need = 4u - (m - o);

      *end = (u8*)j;
      *num = o - out;

      return need;
    }

    o[0] = uson_base64_set[j[0] >> 2];
    o[1] = uson_base64_set[((j[0] << 4) & 0x3Fu) | (j[1] >> 4)];
    o[2] = uson_base64_set[((j[1] << 2) & 0x3Fu) | (j[2] >> 6)];
    o[3] = uson_base64_set[j[2] & 0x3Fu];

    o += 4;
#endif

    j += 3;
  }

#if JSON(EXPLICIT)
  n = size % 3u;

  if (n != 0)
  {
  #if JSON(SIZE)
    size += 4u;
  #else
    if ((size_t)(m - o) < 4u) goto needspace;
  #endif
  }
  else break;

  #if !JSON(SIZE)
    if (n == 1)
    {
      o[0] = uson_base64_set[j[0] >> 2];
      o[1] = uson_base64_set[(j[0] << 4) & 0x3Fu];
      o[2] = uson_base64_pad;
      o[3] = uson_base64_pad;
    }
    else if (n == 2)
    {
      o[0] = uson_base64_set[j[0] >> 2];
      o[1] = uson_base64_set[((j[0] << 4) & 0x3Fu) | (j[1] >> 4)];
      o[2] = uson_base64_set[(j[1] << 2) & 0x3Fu];
      o[3] = uson_base64_pad;
    }

    o += 4;
  #endif

  j += n;

#else // JSON(EXPLICIT) ][

  if (j[0] == '\0') break;

  #if JSON(SIZE)
    size += 4u;
  #else
    if ((size_t)(m - o) < 4u) goto needspace;
  #endif

  if (j[1] == '\0')
  {
  #if !JSON(SIZE)
    o[0] = uson_base64_set[j[0] >> 2];
    o[1] = uson_base64_set[(j[0] << 4) & 0x3Fu];
    o[2] = uson_base64_pad;
    o[3] = uson_base64_pad;

    o += 4;
  #endif

    j += 1;
  }
  else if (j[2] == '\0')
  {
  #if !JSON(SIZE)
    o[0] = uson_base64_set[j[0] >> 2];
    o[1] = uson_base64_set[((j[0] << 4) & 0x3Fu) | (j[1] >> 4)];
    o[2] = uson_base64_set[(j[1] << 2) & 0x3Fu];
    o[3] = uson_base64_pad;

    o += 4;
  #endif

    j += 2;
  }
#endif // !JSON(EXPLICIT)

  break;
}

// -----------------------------------------------------------------------------

#undef JSON_EXPLICIT
#undef JSON_SIZE
