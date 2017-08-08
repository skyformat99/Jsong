// =============================================================================
// <root/end.c>
//
// JSON root element end handling.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#if JSON(SAX)
  #include "end/sax.c"
#else
  #include "end/dom.c"
#endif
