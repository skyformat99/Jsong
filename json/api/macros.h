// =============================================================================
// <api/macros.h>
//
// Function prefixes.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#ifndef H_DDDD7AAC98164EE2BDFB26D31F4D411C
#define H_DDDD7AAC98164EE2BDFB26D31F4D411C

// -----------------------------------------------------------------------------

#if ENABLED(USON)
  #define json_prefix(ident) uson_##ident
  #define jsax_prefix(ident) usax_##ident
#else
  #define json_prefix(ident) json_##ident
  #define jsax_prefix(ident) jsax_##ident
#endif

// -----------------------------------------------------------------------------

#endif
