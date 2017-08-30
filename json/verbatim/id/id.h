// =============================================================================
// <verbatim/id/id.h>
//
// USON verbatim string identifier definitions.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#ifndef H_5BB0E6794551466993F84A42914DDA69
#define H_5BB0E6794551466993F84A42914DDA69

// -----------------------------------------------------------------------------

#if JSON(SAX) || !JSON(STREAM)
  #define uson_verb_id_init()
#else
  #define uson_verb_id_init() json_buf_init (0)
#endif

// -----------------------------------------------------------------------------

#endif
