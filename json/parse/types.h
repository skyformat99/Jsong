// =============================================================================
// <parse/types.c>
//
// Auxiliary types.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#ifndef H_050460D3E2334E8CB7D4E873BA1FD3C0
#define H_050460D3E2334E8CB7D4E873BA1FD3C0

// -----------------------------------------------------------------------------
// Parser object
#if ENABLED(USON)
  #if JSON(SAX)
    typedef usax_t jsnp_t;
  #else
    typedef uson_t jsnp_t;
  #endif
#else
  #if JSON(SAX)
    typedef jsax_t jsnp_t;
  #else
    typedef json_t jsnp_t;
  #endif
#endif

// -----------------------------------------------------------------------------

#endif
