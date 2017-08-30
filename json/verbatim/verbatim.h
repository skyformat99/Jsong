// =============================================================================
// <verbatim/verbatim.h>
//
// USON verbatim string definitions.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#ifndef H_F7073A1A87BE4477B4381AB3CAFA2E3A
#define H_F7073A1A87BE4477B4381AB3CAFA2E3A

// -----------------------------------------------------------------------------

#include "id/id.h"

// -----------------------------------------------------------------------------

#if JSON(SAX)
  #define uson_verb_init()
#else
  #define uson_verb_init() json_val_init (jsnp->elmnt, verb)
#endif

#if !JSON(SAX) && JSON(STREAM)
  #define uson_verb_str_init() json_buf_init (0)
#else
  #define uson_verb_str_init()
#endif

// -----------------------------------------------------------------------------

#endif
