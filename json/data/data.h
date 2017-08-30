// =============================================================================
// <data/data.h>
//
// USON data string definitions.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#ifndef H_2255387BAA304491B6449A512085B789
#define H_2255387BAA304491B6449A512085B789

// -----------------------------------------------------------------------------

#include "scheme/scheme.h"
#include "mime/mime.h"

// -----------------------------------------------------------------------------

#if JSON(SAX)
  #define uson_data_init()
#else
  #define uson_data_init() json_val_init (jsnp->elmnt, data)
#endif

#if !JSON(SAX) && JSON(STREAM)
  #define uson_data_str_init() json_buf_init (0)
#else
  #define uson_data_str_init()
#endif

// -----------------------------------------------------------------------------

#endif
