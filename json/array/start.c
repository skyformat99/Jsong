// =============================================================================
// <array/start.c>
//
// JSON array start handling.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#if JSON(SAX)
  #include "start/sax.c"
#else
  #include "start/dom.c"
#endif
