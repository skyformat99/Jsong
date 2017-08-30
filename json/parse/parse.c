// =============================================================================
// <parse/parse.c>
//
// Parsing function template.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#include "macros.h"

// -----------------------------------------------------------------------------

#ifndef JSON_INPLACE
  #define JSON_INPLACE
#endif

// -----------------------------------------------------------------------------

if (true)
{
  // Prevent the parser restart without initializing
  // it again after parsing the previous input
  if (unlikely (jsnp->state == json_state_error)) goto error;

  // Input position
  const u8* j = json;

  // Input end
#if JSON(EXPLICIT)
  const u8* e = json + size;
#else
  const u8* e;
#endif

#if JSON(STREAM)
  // Get the parser state and flags
  json_state_t state = jsnp->state;
  state |= (json_state_t)last << json_flags_last_bit;
#elif ENABLED(USON)
  // The configuration flag might be set
  json_state_t state = jsnp->state;
#elif JSON(EXPLICIT)
  json_state_t state = json_state_root;
#else
  json_state_t state = 0;
#endif

  // Parsing loop
  while (json_access())
  {
  // Get the next character
  register uint c = *j;

#if JSON(EXPLICIT)
  // GCC compiles this switch into the jump table
  switch (json_state_get())
#else
  switch (json_state_root)
#endif
  {
// -----------------------------------------------------------------------------
// Key
case json_state_key:
case_json_state_key:
    // Whitespace
    if (likely (json_wspace (c)))
    {
      #include "../wspace/wspace.c"
    }

root_key:
    // String
    if (likely (c == '"'))
    {
      json_str_init();

#if JSON(STREAM)
      // Set the string state now because the stream
      // might just end after the opening double quote
      jsnp->st.str.pos = (size_t)(j - json) + 1u;
#endif

      state |= json_flag_key;

      json_break (json_state_string, 1u);
    }

    // Object end
    if (unlikely ((c == '}')
#if !ENABLED(USON)
    // JSON object must be empty to end like that
    && (json_flags_obj_empty (state))
#endif
    ))
    {
      json_state (json_state_comma);
      goto object_end;
    }

#if ENABLED(USON)
    // Check for configuration end
    if (uson_flags_config (state))
    {
      if (json_last())
      {
        if (json_done()) goto config_end;
      }
    }

    // Unquoted identifier string
    uson_ident_init();

  #if JSON(STREAM)
    jsnp->st.str.pos = j - json;
  #endif

    state |= json_flag_key;

    json_state (uson_state_ident);

    goto case_uson_state_ident;
#else
    // Unexpected token
    json_error (JSON_ERROR_EXPECTED_PROPERTY);
#endif

    // Unreachable
    assume_unreachable();
    break;

// -----------------------------------------------------------------------------
// Colon
case json_state_colon:
case_json_state_colon:
    // Whitespace
    if (unlikely (json_wspace (c)))
    {
#if ENABLED(USON)
      state |= uson_flag_wspace;
#endif

      #define NO_SIMD

      #include "../wspace/wspace.c"
    }

    // Check if colon is present
    if (likely (c == ':'))
    {
#if ENABLED(USON)
      state = flag_clr (state, uson_flag_wspace);
#endif

      json_break (json_state_value, 1u);
    }

#if ENABLED(USON)
    // USON object and array opening bracket is allowed to consume
    // the preceding colon if there was whitespace in-between
    if (uson_flags_wspace (state))
    {
      state = flag_clr (state, uson_flag_wspace);

      if (likely (c == '{')) goto object_start;
      if (likely (c == '[')) goto array_start;
    }
#endif

    // Unexpected token
    json_error (JSON_ERROR_EXPECTED_VALUE);

    // Unreachable
    assume_unreachable();
    break;

// -----------------------------------------------------------------------------
// String
case json_state_string:
case_json_state_string:
{
    // String start
    const u8* p = j;

    // In-place modifier
    u8* o;

    // Skip the part of string without escape sequences.
    // Good for object property keys.
#if CPU(SSE2)
    #include "../string/skip/simd.c"
#else
    while (json_peek (4u))
    {
      if (unlikely (!json_prefix (str_tbl) (c))) break;
      if (unlikely (!json_prefix (str_tbl) (j[1]))) {c = j[1]; j++; break;}
      if (unlikely (!json_prefix (str_tbl) (j[2]))) {c = j[2]; j += 2; break;}
      if (unlikely (!json_prefix (str_tbl) (j[3]))) {c = j[3]; j += 3; break;}

      c = j[4];
      j += 4;
    }

    o = (u8*)j;
#endif

loop (string);
    // Check for string end
    if (unlikely (c == '"')) goto string_done;

    // Escape sequence
    if (unlikely (c == '\\'))
    {
      if (json_broken (2u))
      {
        json_seq (2u);
        goto string_end;
      }

      // See what kind of escape sequence it is
      c = j[1];

#if ENABLED(USON)
      // Hexadecimal escape sequence
      if (unlikely (c == 'x'))
      {
        if (json_broken (4u))
        {
          json_seq (4u);
          goto string_end;
        }

        // Decode the byte
  #if JSON(EXPLICIT)
        uint x;

        uint d = chr_xdig_to_int (j[2]);
        c = d << 4;
        x = d;

        d = chr_xdig_to_int (j[3]);
        c |= d;
        x |= d;

        if (unlikely (x > 0xFu))
        {
          json_track (j - p);
          json_error (USON_ERROR_HEXADECIMAL);
        }
  #else
        uint d = chr_xdig_to_int (j[2]);
        if (unlikely (d > 0xFu)) goto error_hex;
        c |= d << 4;

        d = chr_xdig_to_int (j[3]);

        if (unlikely (d > 0xFu))
        {
error_hex:
          json_track (j - p);
          json_error (USON_ERROR_HEXADECIMAL);
        }

        c |= d;
  #endif

        *o++ = c;
        j += 4;
      }

#else // USON ][

      // Unicode escape sequence
      if (unlikely (c == 'u'))
      {
        if (json_broken (6u))
        {
          json_seq (6u);
          goto string_end;
        }

        // Decode the UTF-16 character
  #if JSON(EXPLICIT)
        uint x;

        uint u = chr_xdig_to_int (j[2]);
        c = u << 12;
        x = u;

        u = chr_xdig_to_int (j[3]);
        c |= u << 8;
        x |= u;

        u = chr_xdig_to_int (j[4]);
        c |= u << 4;
        x |= u;

        u = chr_xdig_to_int (j[5]);
        c |= u;
        x |= u;

        if (unlikely (x > 0xFu)) goto error_unicode;
  #else
        uint u = chr_xdig_to_int (j[2]);
        if (unlikely (u > 0xFu)) goto error_unicode;
        c = u << 12;

        u = chr_xdig_to_int (j[3]);
        if (unlikely (u > 0xFu)) goto error_unicode;
        c |= u << 8;

        u = chr_xdig_to_int (j[4]);
        if (unlikely (u > 0xFu)) goto error_unicode;
        c |= u << 4;

        u = chr_xdig_to_int (j[5]);
        if (unlikely (u > 0xFu)) goto error_unicode;
        c |= u;
  #endif

        // 1 UTF-8 byte
        if (likely (utf16_chr_is8_lead1 (c)))
        {
          *o++ = c;
          j += 6;
        }
        // 2 UTF-8 bytes
        else if (likely (utf16_chr_is8_lead2 (c)))
        {
          o[0] = utf8_codep_decomp_head2 (c);
          o[1] = utf8_codep_decomp_tail1 (c);

          o += 2;
          j += 6;
        }
        // 3 UTF-8 bytes
        else if (likely (!utf16_chr_is_surr (c)))
        {
          o[0] = utf8_codep_decomp_head3 (c);
          o[1] = utf8_codep_decomp_tail2 (c);
          o[2] = utf8_codep_decomp_tail1 (c);

          o += 3;
          j += 6;
        }
        // 4 UTF-8 bytes (UTF-16 surrogate pair)
        else if (likely (utf16_chr_is_surr_high (c)))
        {
          if (json_broken (12u))
          {
            json_seq (12u);
            goto string_end;
          }

          // Decode the low surrogate UTF-16 character
          uint s;

  #if JSON(EXPLICIT)
          if (unlikely (!buf_equ2 (j + 6, '\\', 'u'))) goto error_unicode;

          u = chr_xdig_to_int (j[8]);
          s = u << 12;
          x = u;

          u = chr_xdig_to_int (j[9]);
          s |= u << 8;
          x |= u;

          u = chr_xdig_to_int (j[10]);
          s |= u << 4;
          x |= u;

          u = chr_xdig_to_int (j[11]);
          s |= u;
          x |= u;

          if (unlikely (x > 0xFu)) goto error_unicode;
  #else
          if (unlikely ((j[6] != '\\') || (j[7] != 'u'))) goto error_unicode;

          u = chr_xdig_to_int (j[8]);
          if (unlikely (u > 0xFu)) goto error_unicode;
          s = u << 12;

          u = chr_xdig_to_int (j[9]);
          if (unlikely (u > 0xFu)) goto error_unicode;
          s |= u << 8;

          u = chr_xdig_to_int (j[10]);
          if (unlikely (u > 0xFu)) goto error_unicode;
          s |= u << 4;

          u = chr_xdig_to_int (j[11]);
          if (unlikely (u > 0xFu)) goto error_unicode;
          s |= u;
  #endif

          // Check if it's actually a low surrogate character
          if (unlikely (!utf16_chr_is_surr_low (s))) goto error_unicode;

          // Make pair with the high surrogate
          uf32 a = utf16_surr_to32 (c, s);

          // Encode as a UTF-8 character
          o[0] = utf8_codep_decomp_head4 (a);
          o[1] = utf8_codep_decomp_tail3 (a);
          o[2] = utf8_codep_decomp_tail2 (a);
          o[3] = utf8_codep_decomp_tail1 (a);

          o += 4;
          j += 12;
        }
        // Invalid UTF-16 surrogate pair (must begin with high surrogate)
        else
        {
error_unicode:
          json_track (j - p);
          json_error (JSON_ERROR_UNICODE);
        }
      }
#endif // !USON

      // Character escape sequence
      else
      {
        // Unescape the character using the lookup table
        c = json_prefix (chr_unescape) (c);

        // Check if the escape sequence is valid
        if (unlikely (c > 0x7Fu))
        {
          json_track (j - p);
          json_error (JSON_ERROR_ESCAPE);
        }

        // Save the unescaped character
        *o++ = c;
        j += 2;
      }
    }

    // Check if the character is a valid JSON string character
#if ENABLED(USON)
    else if (likely (chr_is_print_strict (c)))
#else
    else if (likely (chr_is_print (c)))
#endif
    {
      // Include the SIMD processing code path
      #include "../string/simd.c"

      *o++ = c;
      j++;
    }

    // Invalid string character
    else
    {
      json_track (j - p);
      json_error (JSON_ERROR_CHARACTER);
    }

    // Check for input end
    if (json_end()) goto string_end;

    // Get the next string character
    c = *j;

    repeat (string);

    // Save the string
string_done:
    j++;
    json_track (j - p);

    #include "../string/string.c"

    json_break (json_state_comma, 0);

    // Save the string state
string_end:
#if JSON(EXPLICIT)
    json_track (j - p);

  #if JSON(STREAM)
    jsnp->st.str.len = (size_t)(o - p);
  #endif

    json_more();
#endif

    // Unreachable
    assume_unreachable();
    break;
}

// -----------------------------------------------------------------------------
// Comma
case json_state_comma:
case_json_state_comma:
    // Whitespace
    if (likely (json_wspace (c)))
    {
#if ENABLED(USON)
      state |= uson_flag_wspace;
#endif

      #include "../wspace/wspace.c"
    }

    // Object
    if (likely (json_flags_obj (state)))
    {
#if !ENABLED(USON)
      // Next property
      if (likely (c == ','))
      {
        #include "../object/grow.c"

        state = flag_clr (state, json_flag_empty);

        json_break (json_state_key, 1u);
      }
#endif

      // Object end
      if (unlikely (c == '}'))
      {
object_end:
        #include "../object/end.c"

#if ENABLED(USON)
        state = flag_clr (state, uson_flag_wspace) | uson_flag_coll_end;
#endif

        json_continue (json_state_comma, 1u);
      }

#if ENABLED(USON)
      // USON object properties are separated by semicolons
      if (likely (c == ';'))
      {
        #include "../object/grow.c"

        state = flag_clr (state, json_flag_empty | uson_flag_wspace | uson_flag_coll_end);

        json_break (json_state_key, 1u);
      }

      // USON object and array closing bracket is allowed to consume semicolons.
      // But if there is a property that follows it must additionally be separated
      // by some whitespace.
      if (unlikely (uson_flags_wspace_coll_end (state)))
      {
        #include "../object/grow.c"

        state = flag_clr (state, json_flag_empty | uson_flag_wspace | uson_flag_coll_end);

        json_state (json_state_key);

        goto root_key;
      }

      // UCFG end
      if (unlikely (uson_flags_config_coll_end (state)))
      {
        if (json_last())
        {
          if (json_done())
          {
            #define USON_CONFIG

config_end:
            #include "../object/end.c"
          }
        }
      }
#endif

      // Unexpected token
      json_error (JSON_ERROR_EXPECTED_VALUE);
    }

    // Array
    if (likely (json_flags_arr (state)))
    {
#if !ENABLED(USON)
      // Next item
      if (likely (c == ','))
      {
        #include "../array/grow.c"

        state = flag_clr (state, json_flag_empty);

        json_break (json_state_value, 1u);
      }
#endif

      // Array end
      if (unlikely (c == ']'))
      {
array_end:
        #include "../array/end.c"

#if ENABLED(USON)
        state = flag_clr (state, uson_flag_wspace) | uson_flag_coll_end;
#endif

        json_continue (json_state_comma, 1u);
      }

#if ENABLED(USON)
      // USON array items are separated simply by whitespace
      if (likely (uson_flags_wspace (state)))
      {
        #include "../array/grow.c"

        state = flag_clr (state, json_flag_empty | uson_flag_wspace | uson_flag_coll_end);

        json_state (json_state_value);

        goto root_value;
      }
#endif

      // Unexpected token
      json_error (JSON_ERROR_EXPECTED_VALUE);
    }

    // Value end
    if (json_last())
    {
      if (json_done())
      {
        #include "../root/end.c"
      }
    }

    // Unexpected token
    json_error (JSON_ERROR_TOKEN);

    // Unreachable
    assume_unreachable();
    break;

// -----------------------------------------------------------------------------
// Value
case json_state_value:
case_json_state_value:
    // Whitespace
    if (likely (json_wspace (c)))
    {
      #include "../wspace/wspace.c"
    }

root_value:
    // String
    if (likely (c == '"'))
    {
      json_str_init();

#if JSON(STREAM)
      jsnp->st.str.pos = (size_t)(j - json) + 1u;
#endif

      json_break (json_state_string, 1u);
    }

    // Object
    if (unlikely (c == '{'))
    {
object_start:
      #include "../object/start.c"

#if ENABLED(USON) || JSON(STREAM)
      state = flag_clr (state, json_flag_arr) | json_flag_empty;
#else
      state = json_flag_empty;
#endif

      json_break (json_state_key, 1u);
    }

    // Array
    if (unlikely (c == '['))
    {
array_start:
      #include "../array/start.c"

#if ENABLED(USON) || JSON(STREAM)
      state = flag_clr (state, json_flag_arr) | json_flag_arr | json_flag_empty;
#else
      state = json_flag_arr | json_flag_empty;
#endif

      json_break (json_state_value, 1u);
    }

#if !ENABLED(USON)
    // Boolean true
    if (unlikely (c == 't'))
    {
      if (json_broken (4u))
      {
        json_seq (4u);
        json_more();
      }

  #if JSON(EXPLICIT)
      if (likely (buf_equ4 (j, 't', 'r', 'u', 'e')))
  #else
      if (likely ((j[1] == 'r') && (j[2] == 'u') && (j[3] == 'e')))
  #endif
      {
        json_bool_evnt (true);
        json_break (json_state_comma, 4u);
      }

      json_error (JSON_ERROR_TOKEN);
    }

    // Boolean false
    if (unlikely (c == 'f'))
    {
      if (json_broken (5u))
      {
        json_seq (5u);
        json_more();
      }

  #if JSON(EXPLICIT)
      if (likely (buf_equ4 (j + 1, 'a', 'l', 's', 'e')))
  #else
      if (likely ((j[1] == 'a') && (j[2] == 'l') && (j[3] == 's') && (j[4] == 'e')))
  #endif
      {
        json_bool_evnt (false);
        json_break (json_state_comma, 5u);
      }

      json_error (JSON_ERROR_TOKEN);
    }

    // Null value
    if (unlikely (c == 'n'))
    {
      if (json_broken (4u))
      {
        json_seq (4u);
        json_more();
      }

  #if JSON(EXPLICIT)
      if (likely (buf_equ4 (j, 'n', 'u', 'l', 'l')))
  #else
      if (likely ((j[1] == 'u') && (j[2] == 'l') && (j[3] == 'l')))
  #endif
      {
        json_null_evnt();
        json_break (json_state_comma, 4u);
      }

      json_error (JSON_ERROR_TOKEN);
    }

#else // !USON ][

    // USON string
    if (unlikely (c == '('))
    {
      if (json_broken (2u))
      {
        json_seq (2u);
        json_more();
      }

      c = j[1];

      // Verbatim string
      if (likely (c == '!'))
      {
        uson_verb_init();
        uson_verb_id_init();

  #if JSON(STREAM)
        jsnp->st.verb.pos = (size_t)(j - json) + 2u;
  #endif

        // Parse the delimiting identifier now
        json_break (uson_state_verb_id, 2u);
      }

      // Encoded data string with metadata
      if (unlikely (c == '?'))
      {
        uson_data_init();
        uson_data_scheme_init();

  #if JSON(STREAM)
        jsnp->st.data.pos = (size_t)(j - json) + 2u;
  #endif

        // Parse the metadata now
        json_break (uson_state_data_scheme, 2u);
      }

      // Encoded data string without metadata
      // (default Base64 USON encoding without MIME type)
      uson_data_init();
      uson_data_str_init();

      j++;

  #if JSON(STREAM)
      jsnp->st.data.pos = j - json;
  #endif
      jsnp->st.data.scheme = 0;

      json_state (uson_state_data);

      goto case_uson_state_data;
    }
#endif // USON

    // Array end
    if (unlikely ((c == ']')
#if !ENABLED(USON)
    // JSON array must be empty to end like that
    && (json_flags_arr_empty (state))
#endif
    ))
    {
      json_state (json_state_comma);
      goto array_end;
    }

// -----------------------------------------------------------------------------
// Number
else
{
    register size_t d;

    const u8* p;
    const u8* r;

#if !JSON(STRING_NUMBERS)
    double mf;
    uintmax_t mi;

  #if CPU(32BIT)
    uf32 mi32;
  #endif

    uint ex;
    int ep;

  #if JSON(BIG_NUMBERS)
    json_big_flt_t bf;
    json_big_uint_t bi;
  #endif
#endif

    size_t lc;
    size_t lf = 0;
    size_t le = 0;

    json_num_meta_t meta = {0};

    // Initialize the number
    json_num_init();

    p = j;
    r = j;

    // Check for sign
#if ENABLED(USON)
    if (unlikely (chr_is_sign (c)))
#else
    if (unlikely (c == '-'))
#endif
    {
      if (json_broken (2u))
      {
        json_seq (2u);
        json_more();
      }

#if ENABLED(USON)
      meta.sign = c >> 2;
#else
      meta.sign = true;
#endif

      c = *++j;
      r = j;
    }

    // Set the number state
#if JSON(STREAM)
    json_state (json_state_number);
    jsnp->st.num.pos = p - json;
#endif

    d = chr_dig_to_int (c);

    // Negative zero means floating point number
    meta.floating = meta.sign & !d;

#if JSON(STRING_NUMBERS)
    // Save for the leading zero test
    meta.reserved = !d;
#else
  #if CPU(32BIT)
    mi32 = 0;
  #else
    mi = 0;
  #endif
#endif

    // 32-bit mantissa ---------------------------------------------------------

#if CPU(32BIT)
loop (cardinal32);
    if (unlikely (d > 9u)) goto cardinal32_done;

  #if !JSON(STRING_NUMBERS)
    if (int_can_oflow_dig (mi32, UINT32_MAX)) goto cardinal32_oflow;
    mi32 = (mi32 * 10u) + d;
  #endif

    j++;

    if (json_end())
    {
      mi = mi32;
      goto cardinal_end;
    }

    c = *j;
    d = chr_dig_to_int (c);

    repeat (cardinal32);

cardinal32_oflow:
    if (!int_will_oflow_dig (mi32, UINT32_MAX, d)) mi32 = (mi32 * 10u) + d;
    mi = mi32;

    goto cardinal_continue;

cardinal32_done:
  #if JSON(STREAM)
    lc = meta.len_cardinal + (size_t)(j - r);
    meta.len_cardinal = lc;
  #else
    lc = j - r;

    #if JSON(STRING_NUMBERS)
      meta.len_cardinal = lc;
    #endif
  #endif

    if (unlikely (lc == 0))
    {
  #if ENABLED(USON)
      if (unlikely (c != '.')) goto ident_from_number;
  #else
      json_error (JSON_ERROR_TOKEN);
  #endif
    }

  #if JSON(STRING_NUMBERS)
    if (unlikely (meta.reserved && (lc != 1u)))
  #else
    if (unlikely ((mi32 == 0) && (lc != 1u)))
  #endif
    {
  #if ENABLED(USON)
      goto ident_from_number;
  #else
      json_error (JSON_ERROR_NUMBER);
  #endif
    }

    if (unlikely (c == '.'))
    {
      meta.floating = true;
      meta.fraction = true;

      r = ++j;

loop (fraction32);
      if (json_end())
      {
        mi = mi32;
        goto fraction_end;
      }

      c = *j;
      d = chr_dig_to_int (c);

      if (unlikely (d > 9u)) goto fraction32_done;

  #if !JSON(STRING_NUMBERS)
      if (int_can_oflow_dig (mi32, UINT32_MAX)) goto fraction32_oflow;
      mi32 = (mi32 * 10u) + d;
  #endif

      j++;

      repeat (fraction32);

fraction32_oflow:
      if (!int_will_oflow_dig (mi32, UINT32_MAX, d)) mi32 = (mi32 * 10u) + d;
      mi = mi32;

      goto fraction_continue;

fraction32_done:
  #if JSON(STREAM)
      lf = meta.len_fraction + (size_t)(j - r);
      meta.len_fraction = lf;
  #else
      lf = j - r;

    #if JSON(STRING_NUMBERS)
      meta.len_fraction = lf;
    #endif
  #endif

      if (unlikely (lf == 0))
      {
  #if ENABLED(USON)
        if (unlikely ((lc == 0) || (chr_is_exp (c)))) goto ident_from_number;
  #else
        json_error (JSON_ERROR_NUMBER);
  #endif
      }
    }

    mi = mi32;

    goto exponent;
#endif // CPU(32BIT)

    // Cardinal ----------------------------------------------------------------

loop (cardinal);
    if (unlikely (d > 9u)) goto cardinal_done;

#if !JSON(STRING_NUMBERS)
    mi = (mi * 10u) + d;
#endif

    j++;

    if (json_end()) goto cardinal_end;

    c = *j;

cardinal_continue:
    d = chr_dig_to_int (c);

    repeat (cardinal);

    // Save the cardinal part state
cardinal_end:
#if JSON(EXPLICIT)
  #if JSON(STREAM)
    d = j - r;
    lc = meta.len_cardinal + d;

    if (unlikely ((d > JSON_NUM_MANT_LEN_MAX)
    || (lc > JSON_NUM_MANT_LEN_MAX))) json_error (JSON_ERROR_MEMORY);

    meta.len_cardinal = lc;
  #else
    lc = j - r;

    #if JSON(STRING_NUMBERS)
      if (unlikely (lc > JSON_NUM_MANT_LEN_MAX)) json_error (JSON_ERROR_MEMORY);
      meta.len_cardinal = lc;
    #endif
  #endif

    if (json_last())
    {
      // Check if cardinal part starts with a zero decimal digit
      // and is not itself a zero
  #if JSON(STRING_NUMBERS)
      if (unlikely (meta.reserved && (lc != 1u)))
  #else
      if (unlikely ((mi == 0) && (lc != 1u)))
  #endif
      {
  #if ENABLED(USON)
        goto ident_done;
  #else
        json_error (JSON_ERROR_NUMBER);
  #endif
      }

      goto number_done;
    }

    goto number_end;
#endif

    // Cardinal part has ended
cardinal_done:
#if JSON(STREAM)
    d = j - r;
    lc = meta.len_cardinal + d;

    if (unlikely ((d > JSON_NUM_MANT_LEN_MAX)
    || (lc > JSON_NUM_MANT_LEN_MAX))) json_error (JSON_ERROR_MEMORY);

    meta.len_cardinal = lc;
#else
    lc = j - r;

  #if JSON(STRING_NUMBERS)
    if (unlikely (lc > JSON_NUM_MANT_LEN_MAX)) json_error (JSON_ERROR_MEMORY);
    meta.len_cardinal = lc;
  #endif
#endif

    // Check for empty cardinal part
    if (unlikely (lc == 0))
    {
#if ENABLED(USON)
      if (unlikely (c != '.')) goto ident_from_number;
#else
      json_error (JSON_ERROR_TOKEN);
#endif
    }

    // Check if leading zero is followed by a decimal digit
#if JSON(STRING_NUMBERS)
    if (unlikely (meta.reserved && (lc != 1u)))
#else
    if (unlikely ((mi == 0) && (lc != 1u)))
#endif
    {
#if ENABLED(USON)
      goto ident_from_number;
#else
      json_error (JSON_ERROR_NUMBER);
#endif
    }

    // Fraction ----------------------------------------------------------------

    if (unlikely (c == '.'))
    {
      meta.floating = true;
      meta.fraction = true;

      r = ++j;

loop (fraction);
      if (json_end()) goto fraction_end;

      c = *j;

fraction_continue:
      d = chr_dig_to_int (c);

      if (unlikely (d > 9u)) goto fraction_done;

#if !JSON(STRING_NUMBERS)
      mi = (mi * 10u) + d;
#endif

      j++;

      repeat (fraction);

      // Save the fractional part state
fraction_end:
#if JSON(EXPLICIT)
  #if JSON(STREAM)
      d = j - r;
      lf = meta.len_fraction + d;

      if (unlikely ((d > JSON_NUM_MANT_LEN_MAX)
      || (lf > JSON_NUM_MANT_LEN_MAX))) json_error (JSON_ERROR_MEMORY);

      meta.len_fraction = lf;
  #else
      lf = j - r;

    #if JSON(STRING_NUMBERS)
      if (unlikely (lf > JSON_NUM_MANT_LEN_MAX)) json_error (JSON_ERROR_MEMORY);
      meta.len_fraction = lf;
    #endif
  #endif

      if (json_last())
      {
        // Check if number ends with a dot
        if (unlikely (lf == 0))
        {
  #if ENABLED(USON)
          if (unlikely ((lc == 0) || (chr_is_exp (c)))) goto ident_done;
  #else
          json_error (JSON_ERROR_EXPECTED_MORE);
  #endif
        }

        goto number_done;
      }

      goto number_end;
#endif

      // Fractional part has ended
fraction_done:
#if JSON(STREAM)
      d = j - r;
      lf = meta.len_fraction + d;

      if (unlikely ((d > JSON_NUM_MANT_LEN_MAX)
      || (lf > JSON_NUM_MANT_LEN_MAX))) json_error (JSON_ERROR_MEMORY);

      meta.len_fraction = lf;
#else
      lf = j - r;

  #if JSON(STRING_NUMBERS)
      if (unlikely (lf > JSON_NUM_MANT_LEN_MAX)) json_error (JSON_ERROR_MEMORY);
      meta.len_fraction = lf;
  #endif
#endif

      // Check for empty fractional part
      if (unlikely (lf == 0))
      {
#if ENABLED(USON)
        if (unlikely ((lc == 0) || (chr_is_exp (c)))) goto ident_from_number;
#else
        json_error (JSON_ERROR_NUMBER);
#endif
      }
    }

    // Exponent ----------------------------------------------------------------

exponent:
    if (unlikely (chr_is_exp (c)))
    {
      // Check if exponential part is incomplete
      if (json_broken (2u))
      {
        json_seq (2u);
        goto number_end;
      }

      // Check for exponent sign
      c = j[1];
      d = chr_is_sign (c);

      meta.floating = true;
      meta.exponent = true;
#if JSON(STRING_NUMBERS)
      meta.exponent_sign = d;
#else
      meta.exponent_sign = d & (c >> 2);
#endif

      j += d;
      r = j + 1;

#if !JSON(STRING_NUMBERS)
      ex = 0;
#endif

loop (exponent);
      j++;

      if (json_end()) goto exponent_end;

      c = *j;

exponent_continue:
      d = chr_dig_to_int (c);

      if (unlikely (d > 9u)) goto exponent_done;

#if !JSON(STRING_NUMBERS)
      ex = (ex * 10u) + d;
#endif

      repeat (exponent);

      // Save the exponential part state
exponent_end:
#if JSON(EXPLICIT)
  #if JSON(STREAM)
      d = j - r;
      le = meta.len_number + d;

      if (unlikely ((d > JSON_NUM_EXP_LEN_MAX)
      || (le > JSON_NUM_EXP_LEN_MAX))) json_error (JSON_ERROR_MEMORY);

      // Temporal exponent length storage
      meta.len_number = le;
  #else
      le = j - r;

      if (unlikely (le > JSON_NUM_EXP_LEN_MAX)) json_error (JSON_ERROR_MEMORY);
  #endif

      if (json_last())
      {
        // Check if exponential part ends without any decimal digits
        if (unlikely (le == 0))
        {
  #if ENABLED(USON)
          goto ident_done;
  #else
          json_error (JSON_ERROR_NUMBER);
  #endif
        }

        goto number_done;
      }

      goto number_end;
#endif

      // Exponential part has ended
exponent_done:
#if JSON(STREAM)
      d = j - r;
      le = meta.len_number + d;

      if (unlikely ((d > JSON_NUM_EXP_LEN_MAX)
      || (le > JSON_NUM_EXP_LEN_MAX))) json_error (JSON_ERROR_MEMORY);
#else
      le = j - r;

      if (unlikely (le > JSON_NUM_EXP_LEN_MAX)) json_error (JSON_ERROR_MEMORY);
#endif

      // Check for empty exponential part
      if (unlikely (le == 0))
      {
#if ENABLED(USON)
        goto ident_from_number;
#else
        json_error (JSON_ERROR_NUMBER);
#endif
      }
    }

    // Finale ------------------------------------------------------------------

number_done:
    json_track (j - p);

    #include "../number/number.c"

    // The next character is already read
    json_state (json_state_comma);

    goto case_json_state_comma;

    // Save the number state
number_end:;
#if JSON(EXPLICIT)
    json_track (j - p);

  #if JSON(STREAM)
    jsnp->st.num.len = j - p;
    jsnp->st.num.meta = meta;

    #if !JSON(STRING_NUMBERS)
      jsnp->st.num.exponent = ex;
      jsnp->st.num.mantissa = mi;
    #endif
  #endif

    json_more();
#endif

// -----------------------------------------------------------------------------

#if ENABLED(USON)

// -----------------------------------------------------------------------------
// Identifier
case uson_state_ident:
case_uson_state_ident:
    // Identifier start
    p = j;

loop (ident);
    // A number that has failed to parse due to *syntactic* errors
    // is promoted (or demoted, if one wishes) to an identifier
ident_from_number:

    // Check for identifier end
    if (unlikely (!uson_chr_ident (c))) goto ident_done;

    // Check for input end
    j++;

    if (json_end()) goto ident_end;

    // Get the next identifier character
    c = *j;

    repeat (ident);

    // Save the identifier
ident_done:
    json_track (j - p);

    #include "../identifier/identifier.c"

ident_special:
    // The next character is already read
    json_state (json_state_comma);

    goto case_json_state_comma;

    // Save the identifier state
ident_end:;
#if JSON(EXPLICIT)
    json_track (j - p);

  #if JSON(STREAM)
    jsnp->st.str.len = j - p;
  #endif

    json_more();
#endif

// -----------------------------------------------------------------------------

#endif // USON

// -----------------------------------------------------------------------------
// Restart number parsing
#if JSON(STREAM)
case json_state_number:
case_json_state_number:
    p = j;
    r = j;

    meta = jsnp->st.num.meta;

  #if !JSON(STRING_NUMBERS)
    mi = jsnp->st.num.mantissa;
  #endif

    if (unlikely (meta.exponent))
    {
      lc = meta.len_cardinal;
      lf = meta.len_fraction;

  #if !JSON(STRING_NUMBERS)
      ex = jsnp->st.num.exponent;
  #endif

      goto exponent_continue;
    }

    if (unlikely (meta.fraction))
    {
      lc = meta.len_cardinal;
      goto fraction_continue;
    }

    goto cardinal_continue;
#endif
}

    // Unreachable
    assume_unreachable();
    break;

// -----------------------------------------------------------------------------

#if ENABLED(USON)

// -----------------------------------------------------------------------------
// Verbatim string
// -----------------------------------------------------------------------------
// Delimiting identifier
case uson_state_verb_id:
case_uson_state_verb_id:
{
    const u8* p = j;

loop (id);
    if (unlikely (!uson_chr_verb_id (c))) goto id_done;

    j++;

    if (json_end()) goto id_end;

    c = *j;

    repeat (id);

    // Save the delimiting identifier string
id_done:
    if (unlikely (c != '\n')) json_error (JSON_ERROR_TOKEN);

    json_track (j - p);

    #include "../verbatim/id/id.c"

    // Initialize the verbatim string buffer
    uson_verb_str_init();

#if JSON(STREAM)
    jsnp->st.verb.pos = j - json;
#endif

    json_state (uson_state_verb);

    goto case_uson_state_verb;

    // Save the delimiting identifier string state
id_end:
#if JSON(EXPLICIT)
    json_track (j - p);

  #if JSON(STREAM)
    jsnp->st.verb.len = j - p;
  #endif

    json_more();
#endif

    // Unreachable
    assume_unreachable();
    break;
}

// -----------------------------------------------------------------------------
// String itself
case uson_state_verb:
case_uson_state_verb:
{
    const u8* p = j;

#if JSON(LINE_COL)
    const u8* q = j;
#endif

    // Get the saved identifier length
    size_t l = jsnp->st.verb.len_id;

loop (verb);
    // Include the SIMD processing code path
    #include "../verbatim/simd.c"

verb_scalar:
    if (unlikely (!uson_chr_verb (c))) goto verb_done;

    j++;

verb_continue:
    if (json_end()) goto verb_end;

    c = *j;

    repeat (verb);

    // Save the verbatim string
verb_done:
    if (unlikely (c != '\n')) json_error (JSON_ERROR_TOKEN);

    // Check if delimiting identifier is available for comparison
    if (json_broken (2u + l + 1u))
    {
      json_seq (2u + l + 1u);
      goto verb_end;
    }

#if JSON(LINE_COL)
    // Reset the current character number
    q = j + 1;

    jsnp->line++;
    jsnp->col = 1u;
#endif

    // New line must be followed by an exclamation mark,
    // or it's not an ending
    if (j[1] != '!')
    {
      j += 2;
      goto verb_continue;
    }

    // Delimiting identifiers must match exactly (case-sensitively),
    // or it's not an ending
#if JSON(EXPLICIT)
    if (!json_str_equal (j + 2, jsnp->st.verb.id, l))
#else
    if (!json_stri_nequal (j + 2, jsnp->st.verb.id, l))
#endif
    {
      j += 2;
      goto verb_continue;
    }

    // Finally, delimiting identifier must be followed
    // by a closing round bracket
    if (j[2 + l] != ')')
    {
      j += 2 + l;
      goto verb_continue;
    }

    #include "../verbatim/verbatim.c"

    j += 2 + l + 1;
    json_track (j - q);

    json_break (json_state_comma, 0);

    // Save the verbatim string state
verb_end:
#if JSON(EXPLICIT)
    json_track (j - q);

  #if JSON(STREAM)
    jsnp->st.verb.len = j - p;
  #endif

    json_more();
#endif

    // Unreachable
    assume_unreachable();
    break;
}

// -----------------------------------------------------------------------------
// Encoded data string
// -----------------------------------------------------------------------------
// Scheme
case uson_state_data_scheme:
case_uson_state_data_scheme:
{
    const u8* p = j;

loop (scheme);
    if (unlikely (!uson_chr_scheme (c))) goto scheme_done;

    j++;

    if (json_end()) goto scheme_end;

    c = *j;

    repeat (scheme);

    // Save the scheme string
scheme_done:
    if (unlikely ((c != '?') && (c != ':'))) json_error (JSON_ERROR_TOKEN);

    json_track (j - p);

    #include "../data/scheme/scheme.c"

#if JSON(STREAM)
    jsnp->st.data.pos = (size_t)(j - json) + 1u;
#endif

    // Parse the MIME type
    if (c == ':')
    {
      uson_data_mime_init();
      json_break (uson_state_data_mime, 1);
    }

    // Parse the encoded data
    uson_data_str_init();
    json_break (uson_state_data, 1);

    // Save the scheme string state
scheme_end:
#if JSON(EXPLICIT)
    json_track (j - p);

  #if JSON(STREAM)
    jsnp->st.data.len = j - p;
  #endif

    json_more();
#endif

    // Unreachable
    assume_unreachable();
    break;
}

// -----------------------------------------------------------------------------
// MIME type
case uson_state_data_mime:
case_uson_state_data_mime:
{
    const u8* p = j;

loop (mime);
    if (unlikely (!uson_chr_mime (c))) goto mime_done;

    j++;

    if (json_end()) goto mime_end;

    c = *j;

    repeat (mime);

    // Save the MIME type string
mime_done:
    if (unlikely (c != '?')) json_error (JSON_ERROR_TOKEN);

    json_track (j - p);

    #include "../data/mime/mime.c"

    // Initialize the data string buffer
    uson_data_str_init();

#if JSON(STREAM)
    jsnp->st.data.pos = (size_t)(j - json) + 1u;
#endif

    json_break (uson_state_data, 1);

    // Save the MIME type string state
mime_end:
#if JSON(EXPLICIT)
    json_track (j - p);

  #if JSON(STREAM)
    jsnp->st.data.len = j - p;
  #endif

    json_more();
#endif

    // Unreachable
    assume_unreachable();
    break;
}

// -----------------------------------------------------------------------------
// Encoded data
case uson_state_data:
case_uson_state_data:
{
    const u8* p = j;
    u8* o = (u8*)j;

#if !JSON(EXPLICIT) && !CPU(PAGE_SIZE)
    // A case when vectorizing decoding is impossible
loop (data);
#endif

    if (unlikely (json_wspace (c)))
    {
      #define NO_SIMD

#if JSON(EXPLICIT) || CPU(PAGE_SIZE)
      // Skip remaining whitespace
data_wspace:
#endif

      #include "../wspace/wspace.c"
    }

#if JSON(EXPLICIT) || CPU(PAGE_SIZE)
    // Vectorized decoding
  #if !JSON(EXPLICIT)
    const u8* e;

loop (data);
    e = ptr_align_ceil (CPU_PAGE_SIZE, j);
  #endif

    // Include the SIMD processing code path
    #include "../data/simd.c"

    // Can't use `json_peek()` here
    while ((size_t)(e - j) >= 4u)
    {
      // Decode the vector of 4 characters
      uint b0 = uson_base64_tbl[j[0]];
      uint b1 = uson_base64_tbl[j[1]];
      uint b2 = uson_base64_tbl[j[2]];
      uint b3 = uson_base64_tbl[j[3]];

      // Check if some character is either a padding character,
      // a round closing bracket, or if there's an encoding error
      if (unlikely ((b0 | b1 | b2 | b3) > 63u)) break;

      o[0] = (b0 << 2) | (b1 >> 4);
      o[1] = (b1 << 4) | (b2 >> 2);
      o[2] = (b2 << 6) | b3;

      o += 3;
      j += 4;
    }

    // Check for input end
    if (json_end()) goto data_end;

data_skip:
    // Whitespace can occur in-between 4-byte sequences
    if (likely (json_wspace (j[0]))) goto data_wspace;
#endif // JSON(EXPLICIT) || CPU(PAGE_SIZE)

    // Decode the 1st character
    uint b = uson_base64_tbl[j[0]];

    // Data cannot end on the 1st character
    if (unlikely (b > 63u))
    {
      // Unless the whole string ends with closing bracket
      if (likely (b == 128u)) goto data_done;

data_error:
      json_track (j - p);
      json_error (JSON_ERROR_TOKEN);
    }

    // Check if the input ends abruptly
    if (json_broken (4u))
    {
      json_seq (4u);
      goto data_end;
    }

    // Decode the 2nd character
    c = b;
    b = uson_base64_tbl[j[1]];

    // Data cannot end on the 2nd character
    if (unlikely (b > 63u)) goto data_error;

    // Save the 1st byte
    o[0] = (c << 2) | (b >> 4);

    // Decode the 3rd character
    c = b;
    b = uson_base64_tbl[j[2]];

    // Check for data end on the 3rd character
    if (unlikely (b > 63u))
    {
      // Check if the 3rd character is a padding character
      if (unlikely (b != 64u)) goto data_error;

      // Check if the 4th character is a padding character
      if (unlikely (uson_base64_tbl[j[3]] != 64u)) goto data_error;

      o++;
    }

    // Data doesn't end on the 3rd character
    else
    {
      // Save the 2nd byte
      o[1] = (c << 4) | (b >> 2);

      // Decode the 4th character
      c = b;
      b = uson_base64_tbl[j[3]];

      // Check for data end on the 4th character
      if (unlikely (b > 63))
      {
        // Check if the last character is a padding character
        if (unlikely (b != 64)) goto data_error;

        o += 2;
      }

      // Data doesn't end on the 4th character
      else
      {
        // Save the 3rd byte
        o[2] = (c << 6) | b;

        o += 3;
      }
    }

    j += 4;

#if !JSON(EXPLICIT)
    // Back to vectorized processing
    repeat (data);
#endif

    // Save the decoded string
data_done:
    j++;
    json_track (j - p);

    #include "../data/data.c"

    json_break (json_state_comma, 0);

    // Save the decoded string state
data_end:
#if JSON(EXPLICIT)
    json_track (j - p);

  #if JSON(STREAM)
    jsnp->st.data.len = o - p;
  #endif

    json_more();
#endif

    // Unreachable
    assume_unreachable();
    break;
}

// -----------------------------------------------------------------------------

#endif // USON

// -----------------------------------------------------------------------------
// Root
case json_state_root:
case_json_state_root:
    // Whitespace before the root element
    if (unlikely (json_wspace (c)))
    {
      #define NO_SIMD

      #include "../wspace/wspace.c"
    }

#if ENABLED(USON)
    // Implicit root object start
    if (unlikely (uson_flags_config (state)))
    {
      #define USON_CONFIG

      #include "../root/start.c"
      #include "../object/start.c"

      goto root_key;
    }
#endif

    // Check if the root element isn't an object or an array
    if (unlikely (chr_to_lcase_fast (c) != '{'))
    {
#if JSON(STREAM)
      state |= json_flag_root;
#else
      state = json_flag_root;
#endif
    }

    // Set up the root element
    #include "../root/start.c"

    // Parse the root value
    json_state (json_state_value);
    goto root_value;

    // Unreachable
    assume_unreachable();
    break;
  }}

#if JSON(STREAM)
stream_end:
  // Check if this is the last chunk
  if (json_last()) goto value_end;

  // Save the parser state and flags
  jsnp->state = state;
  jsnp->err = JSON_ERROR_EXPECTED_MORE;

  // Copy the partial value string into the intermediate buffer
  if (likely (json_state_get() >= json_state_number))
  {
    json_val_st_t* st = &(jsnp->st.val);

  #if JSON(SAX)
    // Chunked string value processing (only for SAX)
    if (unlikely (jsnp->chunked
    // Must not be a number string
    && (json_state_get() >= json_state_string)
    // Must not be a key or be empty
    && !json_flags_key (state) && (st->len != 0)))
    {
      jsax_str_t str;

      str.buf = json + st->pos;
      str.len = st->len;

      jsax_callback (jsnp->on_str, jsnp, str);
    }
    else
  #endif
    {
      u8* buf = jsnp->buf;

      json_buf_grow (&buf, st->len);
      str_copy (buf + jsnp->used, json + st->pos, st->len);

      jsnp->used += st->len;
    }

    // Reset the value state
    st->pos = 0;
    st->len = 0;
  }

  // Done
  jsnp->pos = j - json;
  jsnp->need = jsnp->need - (size_t)(e - j);

  json_return();
#endif // JSON(STREAM)

#if JSON(EXPLICIT)
value_end:
  if (likely (json_state_get() == json_state_comma))
  {
    // Root value end
    if (likely (json_flags_root (state))) goto root_end;

  #if ENABLED(USON)
    // Configuration end
    if (likely (uson_flags_config (state)))
    {
      if (likely (json_flags_key (state)
      || uson_flags_coll_end (state))) goto config_end;
    }
  #endif
  }

  jsnp->err = JSON_ERROR_EXPECTED_MORE;
#endif // JSON(EXPLICIT)

error:
  // The input is invalid
  jsnp->state = json_state_error;
  jsnp->pos = j - json;
  jsnp->need = 0;

  json_return();

#if JSON(SAX)
pause:
  // Stopped from callback
  jsnp->err = JSON_ERROR_CALLBACK;
  jsnp->pos = j - json;
  jsnp->need = 0;

  json_return();
#endif
}

// -----------------------------------------------------------------------------

#include "undef.h"
