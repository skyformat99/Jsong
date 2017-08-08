// =============================================================================
// <utils/misc.h>
//
// Internal JSON processing utility functions.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#ifndef H_D116F7C713D04697B5D21C4D7394897C
#define H_D116F7C713D04697B5D21C4D7394897C

// -----------------------------------------------------------------------------

#include <quantum/hash.h>

// -----------------------------------------------------------------------------
// String validation
// -----------------------------------------------------------------------------

static const u8 json_str_tbl[256] =
{
  REPEAT16 (0), // 0
  REPEAT16 (0), // 16
  1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 32
  REPEAT16 (1), // 48
  REPEAT16 (1), // 64
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, // 80
  REPEAT16 (1), // 96
  REPEAT16 (1), // 112
  REPEAT64 (1), REPEAT64 (1) // 128
};

// -----------------------------------------------------------------------------
// String unescaping
// -----------------------------------------------------------------------------

static const u8 json_unescape_tbl[256] =
{
  255, 255, 255, 255, 255, 255, 255, 255, // 0
  255, 128, 128, 255, 255, 128, 255, 255, // 8
  255, 255, 255, 255, 255, 255, 255, 255, // 16
  255, 255, 255, 255, 255, 255, 255, 255, // 24
  128, 255, '"', 255, 255, 255, 255, 255, // 32
  255, 255, 255, 255, 255, 255, 255, '/', // 40
  255, 255, 255, 255, 255, 255, 255, 255, // 48
  255, 255, 255, 255, 255, 255, 255, 255, // 56
  255, 255, 255, 255, 255, 255, 255, 255, // 64
  255, 255, 255, 255, 255, 255, 255, 255, // 72
  255, 255, 255, 255, 255, 255, 255, 255, // 80
  255, 255, 255, 255,'\\', 255, 255, 255, // 88
  255, 255,'\b', 255, 255, 255,'\f', 255, // 96
  255, 255, 255, 255, 255, 255,'\n', 255, // 104
  255, 255,'\r', 255,'\t', 255, 255, 255, // 112
  255, 255, 255, 255, 255, 255, 255, 255, // 120
  REPEAT64 (255), REPEAT64 (255)          // 128
};

#define json_chr_unescape(c) json_unescape_tbl[c]

// -----------------------------------------------------------------------------
// String escaping
// -----------------------------------------------------------------------------

static const ul32 json_escape_tbl[16] =
{
  0xFAEAFFFFu, 0xFFFFFFFFu, // 0
  0x00003010u, 0x33000000u, // 32
  0x00000000u, 0x01000000u, // 64
  0x00000000u, 0xC0000000u, // 96
  0x00000000u, 0x00000000u, // 128
  0x00000000u, 0x00000000u, // 160
  0x00000000u, 0x00000000u, // 192
  0x00000000u, 0x00000000u  // 224
};

#define json_chr_escape(c) ((json_escape_tbl[(c) / (INT_BIT / 2u)] >> (((c) % (INT_BIT / 2u)) * 2u)) & 3u)

// -----------------------------------------------------------------------------
// Object property key hashing
// -----------------------------------------------------------------------------

static inline uint json_hash_key (const u8* str, size_t len)
{
#if JSON(HASH_KEYS)
  uf32 hash = hash_fnv1a_fast (str, len);
  return hash_fold (hash, 16) | len;
#else
  return len;
#endif
}

// -----------------------------------------------------------------------------
// Inline memory functions for small strings
// -----------------------------------------------------------------------------

static inline void json_str_copy (u8* restrict dst, const u8* restrict src, size_t size)
{
  #define T_RESTRICT

  #include <quantum/string/buffer/copy/generic.c>
}

// -----------------------------------------------------------------------------

static inline void json_str_move_left (u8* dst, const u8* src, size_t size)
{
  #include <quantum/string/buffer/copy/generic.c>
}

static inline void json_str_move_right (u8* dst, const u8* src, size_t size)
{
  #include <quantum/string/buffer/rcopy/generic.c>
}

// -----------------------------------------------------------------------------

static inline void json_str_fill (u8* buf, size_t size, char_t chr)
{
  #include <quantum/string/buffer/fill/generic.c>
}

// -----------------------------------------------------------------------------

static inline bint json_str_equal (const u8* buf1, const u8* buf2, size_t size)
{
  #define T_EQUALITY

  #include <quantum/string/buffer/compare/generic.c>
}

// -----------------------------------------------------------------------------

#endif
