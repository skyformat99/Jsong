// =============================================================================
// <utils/uncomment/template.c>
//
// JSON uncommenting template.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#include "../../parse/macros.h"

// -----------------------------------------------------------------------------

#undef json_state

#if JSON(STREAM)
  #ifndef JSON_EXPLICIT
    #define JSON_EXPLICIT
  #endif

  #define json_state(s) st = s
#else
  #define json_state(s)
#endif

// -----------------------------------------------------------------------------

if (true)
{
  u8* j = json;

#if JSON(EXPLICIT)
  const u8* e = json + size;
#else
  const u8* e;
#endif

#if ENABLED(USON)
  #if JSON(STREAM)
    u8* id;
  #else
    const u8* id;
  #endif

  uint id_len;
  uint id_pos;
#endif

#if JSON(STREAM)
  json_comment_state_t st = state->state;
#else
  json_comment_state_t st = state_none;
#endif

  switch (st) {
case state_none:
  while (json_access())
  {
    #include "simd.c"

    register uint c = *j;

#if ENABLED(USON)
    // USON string beginning
    if (c == '(')
    {
      json_state (state_brace);
      j++;

case state_brace:
case_state_brace:
      if (json_end()) break;

      c = *j;

      // Verbatim string
      if (c == '!')
      {
        json_state (state_verb_id_start);
        j++;

  #if JSON(STREAM)
        id = state->id;
  #else
        id = j;
  #endif
        id_len = 0;

case state_verb_id_start:
case_state_verb_id_start:
        if (json_end()) break;

        c = *j;

        if (c == '\n')
        {
          json_state (state_verb);

case state_verb:
case_state_verb:
          if (json_end()) break;

          #include "verbatim/simd.c"

          c = *j;

          if (c == '\n')
          {
            json_state (state_verb_eol);
            j++;

case state_verb_eol:
case_state_verb_eol:
            if (json_end()) break;

            c = *j;

            if (c == '!')
            {
              id_pos = 0;

              json_state (state_verb_id_end);
              j++;

case state_verb_id_end:
case_state_verb_id_end:
              if (json_end()) break;

              c = *j;

              if (c == id[id_pos])
              {
                id_pos++;

                if (id_pos == id_len)
                {
                  json_state (state_verb_done);
                  j++;

case state_verb_done:
case_state_verb_done:
                  if (json_end()) break;

                  c = *j;

                  if (c == ')')
                  {
                    json_state (state_none);
                    j++;

                    continue;
                  }
                }

                j++;

                goto case_state_verb_id_end;
              }
            }

            if (json_zero()) goto zero;

            json_state (state_verb);
            j++;

            goto case_state_verb;
          }

          if (json_zero()) goto zero;

          j++;

          goto case_state_verb;
        }

        if (json_zero()) goto zero;

  #if JSON(STREAM)
        if (id_len == numof (state->id)) return;
        id[id_len++] = c;
  #else
        id_len++;
  #endif

        j++;

        goto case_state_verb_id_start;
      }

      // Encoded string
      else
      {
        json_state (state_data);

case state_data:
case_state_data:
        if (json_end()) break;

        #include "data/simd.c"

        c = *j;

        // USON string ending
        if (c == ')')
        {
          json_state (state_none);
          j++;

          continue;
        }

        if (json_zero()) goto zero;

        j++;

        goto case_state_data;
      }
    }
#endif // USON

    // String beginning
    if (c == '"')
    {
      json_state (state_string);
      j++;

case state_string:
case_state_string:
      if (json_end()) break;

      #include "string/simd.c"

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

        #include "single/simd.c"

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

        #include "multi/simd.c"

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
#if JSON(STREAM)
  state->state = st;

  #if ENABLED(USON)
    state->id_len = id_len;
    state->id_pos = id_pos;
  #endif
#endif
}

// -----------------------------------------------------------------------------

#undef JSON_STREAM
#undef JSON_EXPLICIT

// -----------------------------------------------------------------------------

#include "../../parse/undef.h"
