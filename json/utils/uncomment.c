// =============================================================================
// <utils/uncomment.c>
//
// JSON comments remover template.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#if JSON(UNCOMMENT_STREAM)
  #define JSON_UNCOMMENT_EXPLICIT
#endif

#if JSON(UNCOMMENT_EXPLICIT)
  #define json_avail() likely (j != e)
  #define json_end() unlikely (j == e)
  #define json_over(p, n) unlikely (((p) + (n)) >= e)
  #define json_zero() false
#else
  #define json_avail() true
  #define json_end() false
  #define json_over(p, n) false
  #define json_zero() unlikely (c == '\0')
#endif

#if JSON(UNCOMMENT_STREAM)
  #define json_state(s) st = s
#else
  #define json_state(s)
#endif

// -----------------------------------------------------------------------------

if (true)
{
  typedef enum uncmnt_state_e
  {
    state_none,   // Initial state
    state_string, // Inside "string"
    state_escape, // String escape sequence `\[btnfru]`
    state_slash,  // Beginning of `//` or `/*`
    state_single, // Inside `// or # single line comment`
    state_multi,  // Inside `/* multiline comment */`
    state_star    // Ending of multiline comment
  } comment_state_t;

  u8* j = json;

#if JSON(UNCOMMENT_EXPLICIT)
  const u8* e = json + size;
#else
  const u8* e;
#endif

#if JSON(UNCOMMENT_STREAM)
  comment_state_t st = *state;
#else
  comment_state_t st = state_none;
#endif

  switch (st) {
case state_none:
  while (json_avail())
  {
    #include "uncomment/simd.c"

    register uint c = *j;

    // String beginning
    if (c == '"')
    {
      json_state (state_string);
      j++;

case state_string:
case_state_string:
      if (json_end()) break;

      #include "uncomment/string/simd.c"

      c = *j;

      // String ending
      if (c == '"')
      {
        json_state (state_none);
        j++;

        continue;
      }

      // Skip escape sequence
      if (c == '\\')
      {
        json_state (state_escape);
        j++;

case state_escape:
        if (json_end()) break;

        c = *j;

        if (json_zero())
        {
zero:
          json_state (state_none);
          break;
        }

        json_state (state_string);
        j++;

        goto case_state_string;
      }

      if (json_zero()) goto zero;

      j++;

      goto case_state_string;
    }

    // Single line comment
    if (c == '#') goto case_state_hash;

    // Single line or multiline comment
    if (c == '/')
    {
      json_state (state_slash);
      *j++ = ' ';

case state_slash:
      if (json_end()) break;

      c = *j;

      // Single line comment
      if (c == '/')
      {
case_state_hash:
        json_state (state_single);
        *j++ = ' ';

case state_single:
case_state_single:
        if (json_end()) break;

        #include "uncomment/single/simd.c"

        c = *j;

        // Ending of single line comment
        if ((c == '\n') || (c == '\r'))
        {
          json_state (state_none);
          j++;

          continue;
        }

        if (json_zero()) goto zero;

        // Zap the comment contents
        *j++ = ' ';

        goto case_state_single;
      }

      // Multiline comment
      if (c == '*')
      {
        json_state (state_multi);
        *j++ = ' ';

case state_multi:
case_state_multi:
        if (json_end()) break;

        #include "uncomment/multi/simd.c"

        c = *j;

        // Might be ending of multiline comment
        if (c == '*')
        {
          json_state (state_star);
          *j++ = ' ';

case state_star:
          if (json_end()) break;

          c = *j;

          // Ending of multiline comment
          if (c == '/')
          {
            json_state (state_none);
            *j++ = ' ';

            continue;
          }

          if (json_zero()) goto zero;

          json_state (state_multi);
          j++;

          goto case_state_multi;
        }

        if (json_zero()) goto zero;

        // Remove the comment data, but preserve the formatting
        *j++ = (c < 33u) ? c : ' ';

        goto case_state_multi;
      }

      if (json_zero()) goto zero;

      json_state (state_none);
      j++;

      continue;
    }

    // Stop on null-terminating character
    if (json_zero()) break;

    j++;
  }}

end:;
#if JSON(UNCOMMENT_STREAM)
  *state = st;
#endif
}

// -----------------------------------------------------------------------------

#undef JSON_UNCOMMENT_STREAM
#undef JSON_UNCOMMENT_EXPLICIT

#undef json_avail
#undef json_end
#undef json_over
#undef json_zero
#undef json_state
