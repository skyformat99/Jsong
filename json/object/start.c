// =============================================================================
// <object/start.c>
//
// JSON object start handling.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#if JSON(SAX)
  #include "start/sax.c"
#else
  #include "start/dom.c"
#endif

// -----------------------------------------------------------------------------

#undef USON_CONFIG
