// =============================================================================
// <utils.h>
//
// JSON processing utilities public API definitions.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#ifndef H_B29179F142A0485FB7A854748C768883
#define H_B29179F142A0485FB7A854748C768883

// -----------------------------------------------------------------------------

#include "api/core.h"
#include "utils/errors.h"

// =============================================================================
// Types
// -----------------------------------------------------------------------------

typedef enum json_comment_state_e
{
  state_none,   // Initial state

  state_string, // Double-quoted string
  state_escape, // String escape sequence

  state_slash,  // Beginning of `//` or `/*` comments
  state_single, // Inside `//` or `#` single line comment
  state_multi,  // Inside `/* multiline comment */`
  state_star,   // Ending of multiline comment`

  // USON strings states
  state_brace,

  state_verb_id_start,
  state_verb,
  state_verb_eol,
  state_verb_id_end,
  state_verb_done,

  state_data
} json_comment_state_t;

typedef struct json_comment_s
{
  json_comment_state_t state;
} json_comment_t;

// -----------------------------------------------------------------------------

typedef struct uson_comment_s
{
  u8 state;

  u8 id_pos;
  u8 id_len;
  u8 id[61];
} uson_comment_t;

// =============================================================================
// Functions
// -----------------------------------------------------------------------------
// JSON comments (not a part of RFC specification)
// -----------------------------------------------------------------------------
// Uncomment JSON in explicit string
extern void json_uncomment (u8* json, size_t size);
// Uncomment JSON in implicit (null-terminated) string
extern void json_uncommenti (u8* json);
// Uncomment JSON stream
extern void json_uncomment_stream (u8* json, size_t size, json_comment_t* state);

// -----------------------------------------------------------------------------

extern void uson_uncomment (u8* json, size_t size);
extern void uson_uncommenti (u8* json);
extern void uson_uncomment_stream (u8* json, size_t size, uson_comment_t* state);

// -----------------------------------------------------------------------------
// JSON string escaping
// -----------------------------------------------------------------------------
// Escape string
// -----------------------------------------------------------------------------

extern int json_escape_length (const u8* restrict in, size_t len, u8** end
, size_t* num);

extern int json_escape (const u8* restrict in, size_t len, u8* restrict out
, size_t size, u8** end, size_t* num);

// -----------------------------------------------------------------------------

extern int uson_escape_length (const u8* restrict in, size_t len, u8** end
, size_t* num);

extern int uson_escape (const u8* restrict in, size_t len, u8* restrict out
, size_t size, u8** end, size_t* num);

// -----------------------------------------------------------------------------
// Escape null-terminated string
// -----------------------------------------------------------------------------

extern int json_escapei_length (const u8* restrict in, u8** end, size_t* num);

extern int json_escapei (const u8* restrict in, u8* restrict out, size_t size
, u8** end, size_t* num);

// -----------------------------------------------------------------------------

extern int uson_escapei_length (const u8* restrict in, u8** end, size_t* num);

extern int uson_escapei (const u8* restrict in, u8* restrict out, size_t size
, u8** end, size_t* num);

// -----------------------------------------------------------------------------
// JSON string unescaping
// -----------------------------------------------------------------------------
// Unescape string
// -----------------------------------------------------------------------------

extern int json_unescape_length (const u8* restrict in, size_t len, u8** end
, size_t* num);

extern int json_unescape (const u8* restrict in, size_t len, u8* restrict out
, size_t size, u8** end, size_t* num);

// -----------------------------------------------------------------------------

extern int uson_unescape_length (const u8* restrict in, size_t len, u8** end
, size_t* num);

extern int uson_unescape (const u8* restrict in, size_t len, u8* restrict out
, size_t size, u8** end, size_t* num);

// -----------------------------------------------------------------------------
// Unescape null-terminated string
// -----------------------------------------------------------------------------

extern int json_unescapei_length (const u8* restrict in, u8** end, size_t* num);

extern int json_unescapei (const u8* restrict in, u8* restrict out, size_t size
, u8** end, size_t* num);

// -----------------------------------------------------------------------------

extern int uson_unescapei_length (const u8* restrict in, u8** end, size_t* num);

extern int uson_unescapei (const u8* restrict in, u8* restrict out, size_t size
, u8** end, size_t* num);

// -----------------------------------------------------------------------------
// Unescape in-place: without copying to separate buffer
// -----------------------------------------------------------------------------

extern int json_unescape_inplace (u8* in, size_t len, u8** end, size_t* num);
extern int json_unescapei_inplace (u8* in, u8** end, size_t* num);

// -----------------------------------------------------------------------------

extern int uson_unescape_inplace (u8* in, size_t len, u8** end, size_t* num);
extern int uson_unescapei_inplace (u8* in, u8** end, size_t* num);

// -----------------------------------------------------------------------------
// USON
// -----------------------------------------------------------------------------

extern int uson_encode_length (const u8* restrict in, size_t len, u8** end
, size_t* num);

extern int uson_encode (const u8* restrict in, size_t len, u8* restrict out
, size_t size, u8** end, size_t* num);

// -----------------------------------------------------------------------------

extern int uson_encodei_length (const u8* restrict in, u8** end, size_t* num);

extern int uson_encodei (const u8* restrict in, u8* restrict out, size_t size
, u8** end, size_t* num);

// -----------------------------------------------------------------------------

extern int uson_decode_length (const u8* restrict in, size_t len, u8** end
, size_t* num);

extern int uson_decode (const u8* restrict in, size_t len, u8* restrict out
, size_t size, u8** end, size_t* num);

// -----------------------------------------------------------------------------

extern int uson_decodei_length (const u8* restrict in, u8** end, size_t* num);

extern int uson_decodei (const u8* restrict in, u8* restrict out, size_t size
, u8** end, size_t* num);

// -----------------------------------------------------------------------------

extern int uson_decode_inplace (u8* in, size_t len, u8** end, size_t* num);
extern int uson_decodei_inplace (u8* in, u8** end, size_t* num);

// -----------------------------------------------------------------------------

#endif
