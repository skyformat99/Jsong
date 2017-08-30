// =============================================================================
// <wspace/wspace.c>
//
// Skip JSON whitespace optionally validating it.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#if JSON(LINE_COL)

if (true)
{
  // Skip JSON whitespace and track the current line and column numbers
  uint line = jsnp->line;
  uint col = jsnp->col;

  do
  {
    c = (c == '\n');

    line += c;
  #if 1
    col = c ? 1u : (col + 1u);
  #else
    col = (col & (c - 1u)) + 1u;
  #endif

    j++;

    if (json_end())
    {
      jsnp->line = line;
      jsnp->col = col;

      json_more();
    }

    c = *j;
  }
  while (json_wspace (c));

  jsnp->line = line;
  jsnp->col = col;
}

#else // JSON(LINE_COL) ][

  // Skip the first whitespace character
  j++;

  if (json_end()) json_more();

  c = *j;

  // See if the next character is a whitespace as well.
  //
  // The reason for double check is that we don't want to furiously
  // unleash SIMD processing on a single tiny whitespace character,
  // which does occur frequently after colon in object properties
  // or after comma separating array values.
  while (json_wspace (c))
  {
    // Between some tokens SIMD whitespace processing
    // is undesirable
  #if !NO(SIMD)
    #include "simd.c"
  #endif

    j++;

    if (json_end()) json_more();

    c = *j;
  }

#endif

// -----------------------------------------------------------------------------

#undef NO_SIMD
