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

#include "utils/support.h"
#include "utils/errors.h"

// -----------------------------------------------------------------------------
// JSON comments (not a part of RFC specification)
// -----------------------------------------------------------------------------
// Uncomment JSON in explicit string
extern void json_uncomment (u8* json, size_t size);
// Uncomment JSON in implicit (null-terminated) string
extern void json_uncommenti (u8* json);
// Uncomment JSON stream
extern void json_uncomment_stream (u8* json, size_t size, uint* state);

// -----------------------------------------------------------------------------
// JSON string escaping
// -----------------------------------------------------------------------------
// Escape string
// -----------------------------------------------------------------------------

extern int json_escape_length (const u8* restrict src, size_t len, u8** end
, size_t* num);

extern int json_escape (const u8* restrict src, size_t len, u8* restrict dst
, size_t size, u8** end, size_t* num);

// -----------------------------------------------------------------------------
// Escape null-terminated string
// -----------------------------------------------------------------------------

extern int json_escapei_length (const u8* restrict src, u8** end, size_t* num);

extern int json_escapei (const u8* restrict src, u8* restrict dst, size_t size
, u8** end, size_t* num);

// -----------------------------------------------------------------------------
// JSON string unescaping
// -----------------------------------------------------------------------------
// Unescape string
// -----------------------------------------------------------------------------

extern int json_unescape_length (const u8* restrict src, size_t len, u8** end
, size_t* num);

extern int json_unescape (const u8* restrict src, size_t len, u8* restrict dst
, size_t size, u8** end, size_t* num);

// -----------------------------------------------------------------------------
// Unescape null-terminated string
// -----------------------------------------------------------------------------

extern int json_unescapei_length (const u8* restrict src, u8** end, size_t* num);

extern int json_unescapei (const u8* restrict src, u8* restrict dst, size_t size
, u8** end, size_t* num);

// -----------------------------------------------------------------------------
// Unescape in-place: without copying to separate buffer
// -----------------------------------------------------------------------------

extern int json_unescape_inplace (u8* src, size_t len, u8** end, size_t* num);
extern int json_unescapei_inplace (u8* src, u8** end, size_t* num);

// -----------------------------------------------------------------------------

#endif
