// =============================================================================
// <number/number.c>
//
// JSON number handling.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#if JSON(STRING_NUMBERS)
  #if JSON(SAX)
    #include "sax.c"
  #else
    #include "dom.c"
  #endif
#else
  #include "compute.c"
#endif
