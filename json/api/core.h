// =============================================================================
// <core.h>
//
// RFC 7159 JSON parser.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#ifndef H_D226490539444F71BCE58F714CBA6173
#define H_D226490539444F71BCE58F714CBA6173

// =============================================================================
// Configuration
// -----------------------------------------------------------------------------
// #define JSON_SAX: build the SAX parser with event callback API
// instead of DOM API.
// -----------------------------------------------------------------------------
// #define JSON_EXPLICIT: by default the parser accepts UTF-8Z as its input
// (implicitly null-terminated UTF-8 strings). This option will make the parser
// accept UTF-8 strings of explicitly specified length instead.
//
// Enable this option if the terminating null isn't easily available.
// For example, if one can't provide it without full buffer reallocation.
//
// On POSIX-compliant systems with `mmap()` the terminating null can be obtained
// by putting a zeroed page right after the memory-mapped file.
//
// Enabling this option makes parsing function accept three arguments
// with the third being the size of the JSON data pointed by the buffer.
// -----------------------------------------------------------------------------
// #define JSON_STREAM: make the parser fully restartable, allowing it to handle
// non-contiguous streams of JSON data. Implies `JSON_EXPLICIT`.
//
// Enabling this option allows one to parse JSON data in chunks using the buffer
// of specific size (but no less than `JSON_BUF_SIZE_MIN` bytes) without a need
// to have whole JSON data immediately available.
//
// Enabling this option makes parsing function accept four arguments
// with the fourth indicating whether the current chunk is the last one.
// -----------------------------------------------------------------------------
// #define JSON_HASH_KEYS: hash JSON key strings. The hash value would be merged
// with the length value of the key string and stored together in one field.
//
// Enabling this option might improve the performance of JSOM pointer lookups
// if they are used frequently enough on large objects. Otherwise, it might
// not be very useful.
//
// If this option is disabled, then the `hash` field of the key string structure
// is actually just its length. This option is automatically disabled if
// the target platform's `int` width is less than 32 bits.
// -----------------------------------------------------------------------------
// #define JSON_PACK_STRINGS: make copies of small strings in DOM placing them
// on the same CPU cache line together with their respective DOM elements.
//
// Only valid for non-streaming DOM parser.
// -----------------------------------------------------------------------------
// #define JSON_BIG_NUMBERS: enable experimental support for 128-bit
// integral and floating point numbers (if compiler provides them).
// -----------------------------------------------------------------------------
// #define JSON_STRING_NUMBERS: present JSON number values as original
// null-terminated strings instead of actually computing them.
// Disables `JSON_BIG_NUMBERS`.
//
// This option may be enabled when one intends to parse JSON floating point
// numbers on their own, probably utilizing arbitrary precision arithmetic.
//
// By default floating point numbers are parsed with standard precision.
// Whenever floating point precision can be preserved, it is preserved.
// -----------------------------------------------------------------------------
// #define JSON_ERROR_STRING_NUMBERS: same as above, but preserve number values
// as original strings only when they fail to be parsed due to
// value range constraints.
//
// If this option is disabled, then number range errors are represented
// by a special number error type value instead, with metadata describing
// the kind of an error.
// -----------------------------------------------------------------------------
// #define JSON_VALID_WHITESPACE: force strict JSON whitespace validation.
//
// The JSON specification allows only space (`0x20`), tab (`0x09`)
// and new line sequence characters (`0x10` and `0x13`)
// to appear between JSON tokens.
//
// Enforcing this rule incurs a noticeable performance hit when
// parsing documents with lots of indentation. Since whitespace is
// essentially irrelevant to serialized data and often data itself is
// minified (stripped of all whitespace) anyway, it is better to leave
// this option off.
//
// One should only enable this option when parsing configuration files,
// or when strict JSON validation is actually required.
// -----------------------------------------------------------------------------
// #define JSON_LINE_COL: track the current line and character numbers
// while parsing JSON data. Incurs significant performance hit.
// -----------------------------------------------------------------------------

#define JSON(feat) (defined(JSON_##feat))
#define USON(feat) (defined(USON_##feat))

// -----------------------------------------------------------------------------
// Validate the configuration options
// -----------------------------------------------------------------------------

#if JSON(STREAM)
  #ifndef JSON_EXPLICIT
    #define JSON_EXPLICIT
  #endif
#endif

#if JSON(HASH_KEYS) && (INT_BIT < 32)
  #undef JSON_HASH_KEYS
#endif

#if JSON(STREAM) || JSON(SAX)
  #undef JSON_PACK_STRINGS
#endif

#if JSON(STRING_NUMBERS)
  #undef JSON_ERROR_STRING_NUMBERS
  #undef JSON_BIG_NUMBERS
#endif

// -----------------------------------------------------------------------------

#endif
