// =============================================================================
// <json.c>
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#include <quantum/build.h>
#include <quantum/core.h>

#include <quantum/integer.h>
#include <quantum/memory.h>
#include <quantum/buffer.h>
#include <quantum/character.h>
#include <quantum/string.h>

#include <utf/utf.h>

// -----------------------------------------------------------------------------

#define MEM_POOL_STATIC

#include "json.h"

#include "utils/misc.h"
#include "utils/errors.h"

#include "buffer/buffer.h"
#include "string/string.h"
#include "number/number.h"
#include "primitive/primitive.h"
#include "wspace/wspace.h"

// -----------------------------------------------------------------------------

#if (MEM_POOL_ALIGNMENT == 0) && (INTPTR_BIT <= 64)
  #define MEM_POOL_ALIGNMENT 8u
#endif

#include <quantum/memory/pool.c>

// -----------------------------------------------------------------------------
// Auxiliary types
// -----------------------------------------------------------------------------

#if JSON(SAX)
  typedef jsax_t jsnp_t;
#else
  typedef json_t jsnp_t;
#endif

// -----------------------------------------------------------------------------
// State macros
// -----------------------------------------------------------------------------

#if JSON(EXPLICIT)
  #define json_state_get() (state >> 8)
  #define json_state_set(new) state = ((state & 0xFFu) | ((new) << 8))
#endif

// -----------------------------------------------------------------------------
// Control flow macros
// -----------------------------------------------------------------------------

#if JSON(LINE_COL)
  // Current character number tracking
  #define json_track(n) jsnp->col += (size_t)(n)
#else
  #define json_track(n)
#endif

#if JSON(EXPLICIT)
  // Check if there's still data available in the input
  #define json_avail() likely (j != e)
  // Check if the input has been parsed completely
  #define json_done() likely (j == e)

  // Check if the end of the input has been reached
  #define json_end() unlikely (j == e)
  // Check if the pointer would overflow the input (for SIMD code)
  #define json_over(p, n) unlikely (((p) + (n)) >= e)

  // Check if the specified number of characters is present in the input
  #define json_peek(n) likely ((size_t)(e - j) >= (n))
  // Check if the input lacks the specified number of characters
  #define json_broken(n) unlikely ((size_t)(e - j) < (n))

  #if JSON(STREAM)
    // Set the number of characters the current sequence is supposed to be
    #define json_seq(n) jsnp->need = (n)
    // Check if this is the last input chunk
    #define json_last() unlikely (json_flags_last (state))
    // Request more JSON data
    #define json_more() goto stream_end
  #else
    // Unused
    #define json_seq(n)
    // Always the last chunk
    #define json_last() true
    // No more data to request
    #define json_more() json_error (JSON_ERROR_EXPECTED_MORE)
  #endif

  // Set the current parser state
  #define json_state(s) json_state_set (s)

  // Advance the specified number of characters and repeat the current state
  #define json_continue(s, n)\
    json_track (n);\
    j += (n);\
    break

  // Advance the specified number of characters and break to a different state
  #define json_break(s, n)\
    json_state (s);\
    json_track (n);\
    j += (n);\
    break
#else
  // The input data is available until the terminating null is reached
  #define json_avail() true
  // Check if the terminating null has been reached
  #define json_done() likely (c == '\0')

  // The input end isn't explicitly known
  #define json_end() false
  // No overflow can occur
  #define json_over(p, n) false

  // Characters are available until the null character
  #define json_peek(n) true
  // The sequence isn't broken, unless it has the null character in it
  #define json_broken(n) false

  // Unused
  #define json_seq(n)
  // Only one chunk to parse
  #define json_last() true
  // Unreachable
  #define json_more()

  // No state to set (jump directly)
  #define json_state(s)

  // Read the next character after the specified number of characters
  // and repeat the current state
  #define json_continue(s, n)\
    json_track (n);\
    c = j[n];\
    j += (n);\
    goto case_##s

  // Read the next character after the specified number of characters
  // and advance to the next state
  #define json_break(s, n)\
    json_track (n);\
    c = j[n];\
    j += (n);\
    goto case_##s
#endif

// Default return value on stream end or error
#if JSON(SAX)
  #define json_return() return false
#else
  #define json_return() return json_node_null
#endif

// Terminate the parsing due to an unrecoverable error
#define json_error(e) do\
{                 \
  jsnp->err = (e);\
  goto error;\
} while (0)

// -----------------------------------------------------------------------------
// RFC 7159 JSON parser implementation
// -----------------------------------------------------------------------------

#if JSON(SAX)
  #if JSON(STREAM)
void jsax_init_stream (jsax_t* jsnp, u8* buf, size_t size)
  #elif JSON(EXPLICIT)
void jsax_init (jsax_t* jsnp, u8* buf, size_t size)
  #else
void jsax_initi (jsax_t* jsnp, u8* buf, size_t size)
  #endif
#else
  #if JSON(STREAM)
void json_init_stream (json_t* jsnp, mem_pool_t* pool)
  #elif JSON(EXPLICIT)
void json_init (json_t* jsnp, mem_pool_t* pool)
  #else
void json_initi (json_t* jsnp, mem_pool_t* pool)
  #endif
#endif
{
  static const jsnp_t jsnp_zero;

  *jsnp = jsnp_zero;

#if JSON(LINE_COL)
  jsnp->line = 1u;
  jsnp->col  = 1u;
#endif

#if JSON(SAX)
  jsnp->buf  = buf;
  jsnp->size = size;

  jsnp->on_start = jsax_start_trap;
  jsnp->on_key   = jsax_key_trap;
  jsnp->on_str   = jsax_str_trap;
  jsnp->on_num   = jsax_num_trap;
  jsnp->on_int   = jsax_int_trap;
  jsnp->on_flt   = jsax_flt_trap;
  jsnp->on_bool  = jsax_bool_trap;
  jsnp->on_null  = jsax_null_trap;
  jsnp->on_end   = jsax_end_trap;

  #if JSON(STREAM)
    jsnp->on_mem = jsax_mem_trap;
  #endif
#else
  jsnp->pool = pool;
#endif
}

// -----------------------------------------------------------------------------

#if JSON(SAX)
  #if JSON(STREAM)
bool jsax_parse_stream (jsax_t* jsnp, u8* json, size_t size, bool last)
  #elif JSON(EXPLICIT)
bool jsax_parse (jsax_t* jsnp, u8* json, size_t size)
  #else
bool jsax_parsei (jsax_t* jsnp, u8* json)
  #endif
#else
  #if JSON(STREAM)
json_node_t json_parse_stream (json_t* jsnp, u8* json, size_t size, bool last)
  #elif JSON(EXPLICIT)
json_node_t json_parse (json_t* jsnp, u8* json, size_t size)
  #else
json_node_t json_parsei (json_t* jsnp, u8* json)
  #endif
#endif
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

  // Get the parser state and flags
#if JSON(STREAM)
  json_state_t state = jsnp->state;
  state |= last << json_flags_last_bit;
#elif JSON(EXPLICIT)
  json_state_t state = json_state_root;
#else
  json_state_t state = 0;
#endif

  // Parsing loop
  while (json_avail())
  {
  // Get the next character
  register uint c = *j;

  // GCC compiles this switch into the jump table
  // when it's not optimized away
#if JSON(EXPLICIT)
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
      #include "wspace/wspace.c"
    }

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

    // Empty object
    if (unlikely ((c == '}') && (json_flags_obj_empty (state))))
    {
      json_state (json_state_comma);
      goto object_end;
    }

    // Unexpected token
    json_error (JSON_ERROR_EXPECTED_PROPERTY);

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
      #define T_NOSIMD

      #include "wspace/wspace.c"
    }

    // Check if colon is present
    if (likely (c == ':')) json_break (json_state_value, 1u);

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
    #include "string/skip_simd.c"
#else
    while (json_peek (4u))
    {
      if (unlikely (!json_str_tbl[c])) break;
      if (unlikely (!json_str_tbl[j[1]])) {c = j[1]; j++; break;}
      if (unlikely (!json_str_tbl[j[2]])) {c = j[2]; j += 2; break;}
      if (unlikely (!json_str_tbl[j[3]])) {c = j[3]; j += 3; break;}

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

      // Character escape sequence
      else
      {
        // Unescape the character using the lookup table
        c = json_chr_unescape (c);

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
    else if (likely (chr_is_print (c)))
    {
      // Include the SIMD processing code path
      #include "string/simd.c"

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

    #include "string/string.c"

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
      #include "wspace/wspace.c"
    }

    // Object
    if (likely (json_flags_obj (state)))
    {
      // Next key / value pair
      if (likely (c == ','))
      {
        #include "object/grow.c"

        state = flag_clr (state, json_flag_empty);

        json_break (json_state_key, 1u);
      }

      // Object end
      if (unlikely (c == '}'))
      {
object_end:
        #include "object/end.c"

        json_continue (json_state_comma, 1u);
      }

      // Unexpected token
      json_error (JSON_ERROR_EXPECTED_VALUE);
    }

    // Array
    if (likely (json_flags_arr (state)))
    {
      // Next item
      if (likely (c == ','))
      {
        #include "array/grow.c"

        state = flag_clr (state, json_flag_empty);

        json_break (json_state_value, 1u);
      }

      // Array end
      if (unlikely (c == ']'))
      {
array_end:
        #include "array/end.c"

        json_continue (json_state_comma, 1u);
      }

      // Unexpected token
      json_error (JSON_ERROR_EXPECTED_VALUE);
    }

    // Value end
    if (json_last())
    {
      if (json_done())
      {
        #include "root/end.c"
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
      #include "wspace/wspace.c"
    }

case_json_state_value_root:
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
      #include "object/start.c"

#if JSON(STREAM)
      state = flag_clr (state, json_flag_arr) | json_flag_empty;
#else
      state = json_flag_empty;
#endif

      json_break (json_state_key, 1u);
    }

    // Array
    if (unlikely (c == '['))
    {
      #include "array/start.c"

#if JSON(STREAM)
      state = flag_clr (state, json_flag_arr) | json_flag_arr | json_flag_empty;
#else
      state = json_flag_arr | json_flag_empty;
#endif

      json_break (json_state_value, 1u);
    }

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

    // Empty array
    if (unlikely ((c == ']') && (json_flags_arr_empty (state))))
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

    // Check for negative sign
    if (unlikely (c == '-'))
    {
      if (json_broken (2u))
      {
        json_seq (2u);
        json_more();
      }

      meta.sign = true;

      c = j[1];
      r = ++j;
    }

    // Set the number state
#if JSON(STREAM)
    json_state (json_state_number);
    jsnp->st.num.pos = (size_t)(p - json);
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
    lc = (size_t)(j - r);

    #if JSON(STRING_NUMBERS)
      meta.len_cardinal = lc;
    #endif
  #endif

    if (unlikely (lc == 0)) json_error (JSON_ERROR_TOKEN);

  #if JSON(STRING_NUMBERS)
    if (unlikely (meta.reserved && (lc != 1u))) json_error (JSON_ERROR_NUMBER);
  #else
    if (unlikely ((mi32 == 0) && (lc != 1u))) json_error (JSON_ERROR_NUMBER);
  #endif

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
      lf = (size_t)(j - r);

    #if JSON(STRING_NUMBERS)
      meta.len_fraction = lf;
    #endif
  #endif

      if (unlikely (lf == 0)) json_error (JSON_ERROR_NUMBER);
    }

    mi = mi32;

    goto exponent;
#endif

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
    d = (size_t)(j - r);
    lc = meta.len_cardinal + d;

    if (unlikely ((d > JSON_NUM_MANT_LEN_MAX)
    || (lc > JSON_NUM_MANT_LEN_MAX))) json_error (JSON_ERROR_MEMORY);

    meta.len_cardinal = lc;
  #else
    lc = (size_t)(j - r);

    #if JSON(STRING_NUMBERS)
      if (unlikely (lc > JSON_NUM_MANT_LEN_MAX)) json_error (JSON_ERROR_MEMORY);
      meta.len_cardinal = lc;
    #endif
  #endif

    if (json_last())
    {
      // Check if cardinal number starts with a zero decimal digit
      // and is not itself a zero
  #if JSON(STRING_NUMBERS)
      if (unlikely (meta.reserved && (lc != 1u))) json_error (JSON_ERROR_NUMBER);
  #else
      if (unlikely ((mi == 0) && (lc != 1u))) json_error (JSON_ERROR_NUMBER);
  #endif

      goto number_done;
    }

    goto number_end;
#endif

    // Cardinal part has ended
cardinal_done:
#if JSON(STREAM)
    d = (size_t)(j - r);
    lc = meta.len_cardinal + d;

    if (unlikely ((d > JSON_NUM_MANT_LEN_MAX)
    || (lc > JSON_NUM_MANT_LEN_MAX))) json_error (JSON_ERROR_MEMORY);

    meta.len_cardinal = lc;
#else
    lc = (size_t)(j - r);

  #if JSON(STRING_NUMBERS)
    if (unlikely (lc > JSON_NUM_MANT_LEN_MAX)) json_error (JSON_ERROR_MEMORY);
    meta.len_cardinal = lc;
  #endif
#endif

    // Check for empty cardinal part
    if (unlikely (lc == 0)) json_error (JSON_ERROR_TOKEN);

    // Check if leading zero is followed by a decimal digit
#if JSON(STRING_NUMBERS)
    if (unlikely (meta.reserved && (lc != 1u))) json_error (JSON_ERROR_NUMBER);
#else
    if (unlikely ((mi == 0) && (lc != 1u))) json_error (JSON_ERROR_NUMBER);
#endif

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
      d = (size_t)(j - r);
      lf = meta.len_fraction + d;

      if (unlikely ((d > JSON_NUM_MANT_LEN_MAX)
      || (lf > JSON_NUM_MANT_LEN_MAX))) json_error (JSON_ERROR_MEMORY);

      meta.len_fraction = lf;
  #else
      lf = (size_t)(j - r);

    #if JSON(STRING_NUMBERS)
      if (unlikely (lf > JSON_NUM_MANT_LEN_MAX)) json_error (JSON_ERROR_MEMORY);
      meta.len_fraction = lf;
    #endif
  #endif

      if (json_last())
      {
        // Check if number ends with a dot
        if (unlikely (lf == 0)) json_error (JSON_ERROR_EXPECTED_MORE);
        goto number_done;
      }

      goto number_end;
#endif

      // Fractional part has ended
fraction_done:
#if JSON(STREAM)
      d = (size_t)(j - r);
      lf = meta.len_fraction + d;

      if (unlikely ((d > JSON_NUM_MANT_LEN_MAX)
      || (lf > JSON_NUM_MANT_LEN_MAX))) json_error (JSON_ERROR_MEMORY);

      meta.len_fraction = lf;
#else
      lf = (size_t)(j - r);

  #if JSON(STRING_NUMBERS)
      if (unlikely (lf > JSON_NUM_MANT_LEN_MAX)) json_error (JSON_ERROR_MEMORY);
      meta.len_fraction = lf;
  #endif
#endif

      // Check for empty fractional part
      if (unlikely (lf == 0)) json_error (JSON_ERROR_NUMBER);
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
      d = (size_t)(j - r);
      le = meta.len_number + d;

      if (unlikely ((d > JSON_NUM_EXP_LEN_MAX)
      || (le > JSON_NUM_EXP_LEN_MAX))) json_error (JSON_ERROR_MEMORY);

      // Temporal exponent storage
      meta.len_number = le;
  #else
      le = (size_t)(j - r);

      if (unlikely (le > JSON_NUM_EXP_LEN_MAX)) json_error (JSON_ERROR_MEMORY);
  #endif

      if (json_last())
      {
        // Check if exponential part ends without any decimal digits
        if (unlikely (le == 0)) json_error (JSON_ERROR_NUMBER);
        goto number_done;
      }

      goto number_end;
#endif

      // Exponential part has ended
exponent_done:
#if JSON(STREAM)
      d = (size_t)(j - r);
      le = meta.len_number + d;

      if (unlikely ((d > JSON_NUM_EXP_LEN_MAX)
      || (le > JSON_NUM_EXP_LEN_MAX))) json_error (JSON_ERROR_MEMORY);
#else
      le = (size_t)(j - r);

      if (unlikely (le > JSON_NUM_EXP_LEN_MAX)) json_error (JSON_ERROR_MEMORY);
#endif

      // Check for empty exponential part
      if (unlikely (le == 0)) json_error (JSON_ERROR_NUMBER);
    }

    // Finale ------------------------------------------------------------------

number_done:
    json_track (j - p);

    #include "number/number.c"

    json_state (json_state_comma);

    goto case_json_state_comma;

    // Save the number state
number_end:;
#if JSON(EXPLICIT)
    json_track (j - p);

  #if JSON(STREAM)
    jsnp->st.num.len = (size_t)(j - p);
    jsnp->st.num.meta = meta;

    #if !JSON(STRING_NUMBERS)
      jsnp->st.num.exponent = ex;
      jsnp->st.num.mantissa = mi;
    #endif
  #endif

    json_more();
#endif

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
// Root
case json_state_root:
case_json_state_root:
    // Whitespace
    if (unlikely (json_wspace (c)))
    {
      #define T_NOSIMD

      #include "wspace/wspace.c"
    }

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
    #include "root/start.c"

    // Parse the root value
    json_state (json_state_value);
    goto case_json_state_value_root;

    // Unreachable
    assume_unreachable();
    break;
  }}

#if JSON(STREAM)
stream_end:
  // Handle the last chunk
  if (json_last()) goto value_end;

  // Save the parser state and flags
  jsnp->state = state;

  // Indicate that the JSON input is incomplete
  jsnp->err = JSON_ERROR_EXPECTED_MORE;

  // Copy the partial value string into the intermediate buffer
  if (likely (json_state_get() >= json_state_string))
  {
    json_val_st_t* st = &(jsnp->st.val);

  #if JSON(SAX)
    // Chunked string value processing (only for SAX)
    if (unlikely (jsnp->chunked
    && (json_state_get() == json_state_string)
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

      json_buf_grow (buf, st->len);
      str_copy (buf + jsnp->used, json + st->pos, st->len);

      jsnp->used += st->len;
    }

    st->pos = 0;
    st->len = 0;
  }

  // Done
  jsnp->pos = (size_t)(j - json);
  jsnp->need = jsnp->need - (size_t)(e - j);

  json_return();
#endif

#if JSON(EXPLICIT)
value_end:
  // Root value end
  if (likely ((json_state_get() == json_state_comma)
  && json_flags_root (state))) goto root_end;

  jsnp->err = JSON_ERROR_EXPECTED_MORE;
#endif

error:
  // Invalid input
  jsnp->state = json_state_error;
  jsnp->pos = (size_t)(j - json);

  json_return();
}
