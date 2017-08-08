// =============================================================================
// <utils/uncomment/single/simd.c>
//
// Skip through JSON comments faster using SIMD.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#if CPU(SSE42)
  #include "sse42.c"
#elif CPU(SSE2)
  #include "sse2.c"
#endif
