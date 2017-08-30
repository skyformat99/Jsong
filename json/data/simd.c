// =============================================================================
// <data/simd.c>
//
// Process USON encoded data string faster using SIMD.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#if CPU(SSE42)
  #include "sse42.c"
#elif CPU(SSE41)
  #include "sse41.c"
#elif CPU(SSSE3)
  #include "ssse3.c"
#elif CPU(SSE2)
  #include "sse2.c"
#endif
