// =============================================================================
// <data/mime/mime.h>
//
// USON data string mime type definitions.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#ifndef H_37D48A80DD8A446E861CA0FD74395E42
#define H_37D48A80DD8A446E861CA0FD74395E42

// -----------------------------------------------------------------------------

#if JSON(SAX) || !JSON(STREAM)
  #define uson_data_mime_init()
#else
  #define uson_data_mime_init() json_buf_init (0)
#endif

// -----------------------------------------------------------------------------

#endif
