// =============================================================================
// <data/scheme/scheme.h>
//
// USON data string scheme definitions.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#ifndef H_7D341E8B61CF4C95B097C5A147440852
#define H_7D341E8B61CF4C95B097C5A147440852

// -----------------------------------------------------------------------------

#if JSON(SAX) || !JSON(STREAM)
  #define uson_data_scheme_init()
#else
  #define uson_data_scheme_init() json_buf_init (0)
#endif

// -----------------------------------------------------------------------------

#endif
