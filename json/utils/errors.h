// =============================================================================
// <utils/errors.h>
//
// JSON error definitions.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#ifndef H_D353961A102D4FB0A730B4F06F254C71
#define H_D353961A102D4FB0A730B4F06F254C71

// -----------------------------------------------------------------------------
// Constants
// -----------------------------------------------------------------------------

#define JSON_OK 0

// Internal parser errors not related to JSON data
#define JSON_ERROR_CALLBACK 1
#define JSON_ERROR_MEMORY 2
#define JSON_ERROR_DEPTH 3

// JSON string errors
#define JSON_ERROR_CHARACTER 4
#define JSON_ERROR_ESCAPE 5
#define JSON_ERROR_UNICODE 6

// USON string hexadecimal escape sequence error
#define USON_ERROR_HEXADECIMAL 7

// JSON number, boolean, or null value errors
#define JSON_ERROR_NUMBER 8
#define JSON_ERROR_TOKEN 9

// JSON structure errors
#define JSON_ERROR_EXPECTED_PROPERTY 10
#define JSON_ERROR_EXPECTED_VALUE 11
#define JSON_ERROR_EXPECTED_MORE 12

// -----------------------------------------------------------------------------
// Globals
// -----------------------------------------------------------------------------

extern const u8* json_errors[13];

// -----------------------------------------------------------------------------

#endif
