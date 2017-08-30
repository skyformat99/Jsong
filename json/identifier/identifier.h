// =============================================================================
// <identifier/identifier.h>
//
// USON identifier definitions.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#ifndef H_0C6B7A4EC4C0431381FB2EEAE56CABC4
#define H_0C6B7A4EC4C0431381FB2EEAE56CABC4

// -----------------------------------------------------------------------------

#if JSON(STREAM)
  #define uson_ident_init() json_buf_init (sizeof (json_istr_t))
#else
  #define uson_ident_init()
#endif

// -----------------------------------------------------------------------------

#endif
