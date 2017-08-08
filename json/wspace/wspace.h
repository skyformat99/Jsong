// =============================================================================
// <wspace/wspace.h>
//
// JSON whitespace definitions.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#ifndef H_D88E550ED1DB4AF8816DE6B788A97D4E
#define H_D88E550ED1DB4AF8816DE6B788A97D4E

// -----------------------------------------------------------------------------

#if JSON(VALID_WHITESPACE)
  #if !CPU(64BIT)
    // Can reuse the unescape character table to test for valid whitespace
    // on 32-bit systems. On 64-bit systems a faster SWAR test is employed.
    #define json_wspace(c) (json_unescape_tbl[c] == 128u)
  #else
    // Fast whitespace validation on systems with 64-bit+ machine word
    #define json_wspace chr_is_wspace_tnrs
  #endif
#else
  #if JSON(EXPLICIT)
    // No validation at all
    #define json_wspace chr_is_wspace
  #else
    // Minimum validation to catch the terminating null character
    #define json_wspace chr_is_wspace_zero
  #endif
#endif

// -----------------------------------------------------------------------------

#endif
