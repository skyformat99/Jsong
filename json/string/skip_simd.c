// =============================================================================
// <string/skip_simd.c>
//
// Skip through JSON string faster using SIMD.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#if CPU(SSE42)
  #include "skip_sse42.c"
#elif CPU(SSE2)
  #include "skip_sse2.c"
#endif
