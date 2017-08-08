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

// JSON number, boolean, or null value errors
#define JSON_ERROR_NUMBER 7
#define JSON_ERROR_TOKEN 8

// JSON structure errors
#define JSON_ERROR_EXPECTED_PROPERTY 9
#define JSON_ERROR_EXPECTED_VALUE 10
#define JSON_ERROR_EXPECTED_MORE 11

// -----------------------------------------------------------------------------
// Strings
// -----------------------------------------------------------------------------

extern const u8* json_errors[12];

// -----------------------------------------------------------------------------

#endif
