// =============================================================================
// <string/string.h>
//
// JSON string definitions.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#ifndef H_E653857F17CE42C688F82888BCBF72AB
#define H_E653857F17CE42C688F82888BCBF72AB

// -----------------------------------------------------------------------------

#if JSON(STREAM)
  #define json_str_init() json_buf_init (sizeof (json_istr_t))
#else
  #define json_str_init()
#endif

// -----------------------------------------------------------------------------

#endif
