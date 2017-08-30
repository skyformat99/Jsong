// =============================================================================
// <data/sse42.c>
//
// Process USON encoded data faster using SSE4.2.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

                                       // '-' '0-9' 'A-O' 'P-Z' 'a-o' 'p-z'                        '_'
const xi128 xlut = _mm_setr_epi8 (0, 0, 0x11, 0x04, 0xBF, 0xBF, 0xB9, 0xB9, 0, 0, 0, 0, 0, 0, 0, 0xE0);
const xi128 xcmp = _mm_setr_epi8 ('A', 'Z', 'a', 'z', '0', '9', '-', '-', '_', '_', 0, 0, 0, 0, 0, 0);

const xi128 xshf = _mm_setr_epi8 (
   2,  1,  0,
   6,  5,  4,
  10,  9,  8,
  14, 13, 12,
  0xFF, 0xFF,
  0xFF, 0xFF
);

// Decode 16 bytes at a time
while ((size_t)(e - j) >= 16u)
{
  // Get the input chunk
  const xi128 dj = _mm_loadu_si128 ((const xi128*)j);

  // Mask the underscore '_' characters
  xi128 du = _mm_or_si128 (_mm_cmpeq_epi8 (dj, _mm_set1_epi8 (0x5F)), dj);

  // Generate the shuffle mask with underscore characters referring
  // to the 15th byte in the lookup table (if there are any)
  xi128 dl = _mm_srli_epi32 (_mm_and_si128 (du, _mm_set1_epi8 (0xF0)), 4);

  // Construct the result addition vector dynamically
  xi128 lv = _mm_shuffle_epi8 (xlut, dl);

  // Validate the input vector
  register uint b = _mm_cmpistri (xcmp, dj, _SIDD_UBYTE_OPS | _SIDD_CMP_RANGES
  | _SIDD_NEGATIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT);

  // Sum the input vector with the addition vector
  // to get the 16 intermediate result bytes
  xi128 dr = _mm_add_epi8 (dj, lv);

  // Reshuffle them into the final 12 8-bit bytes
  dr = _mm_maddubs_epi16 (dr, _mm_set1_epi32 (0x01400140));
  dr = _mm_madd_epi16 (dr, _mm_set1_epi32 (0x00011000));

  dr = _mm_shuffle_epi8 (dr, xshf);

  // Store the result vector
  #include "sse_outro.c"
}
